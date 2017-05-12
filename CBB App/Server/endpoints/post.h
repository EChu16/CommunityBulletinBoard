#pragma once 

#include "../database.h"
#include "../auth.h"
#include "functions.h"

typedef CBB::Server<CBB::HTTPS> HttpsServer;
typedef CBB::Client<CBB::HTTPS> HttpsClient;

using namespace std;
using namespace boost::property_tree;

void posts_get(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
	auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) { 
            auto post_table = accessCollection(dbCon, "sample_posts");
            auto user_table = accessCollection(dbCon, "sample_users");
            auto community_table = accessCollection(dbCon, "sample_communities");

            //extract data from POST
            ptree pt;
            read_json(request->content, pt);
            string c_name = pt.get<string>("community_name");
            
            //server status message 
            cout << "Session " << session_id << " is requesting all posts from: " << c_name << endl;

            //get community object
            auto cbb_doc = communityNameToCommunityDoc(community_table, c_name);

            //check to see if we have permission to access this board
            auto u = sessionToUserObject(user_table, session_table, session_id);
            auto found = std::find(std::begin(u.communities_joined), std::end(u.communities_joined), cbb_doc->view()["_id"].get_oid().value);
			if (found == std::end(u.communities_joined)) {
			    throw std::runtime_error("User does not have access to this community");
			}

            //get post_ids array from community
            auto post_ids = cbb_doc->view()["post_ids"].get_array().value;
            std::string reply = "[";

            //for each post_id, get info from post table and append it to reply
            //build response as list of comma-separated JSON docs  --  [ {json}, {json}, {json} ]
            for (auto it = post_ids.begin(); it != post_ids.end(); ++it) {
                if (it != post_ids.begin()) {
                    reply += ", ";
                }
                auto post = findOneDocumentByID(post_table, it->get_oid().value);
                if (!post) {
                	throw std::runtime_error("Post not found!");
                }
                reply += bsoncxx::to_json(post->view());
            }   
            reply += " ]";
            *response << "HTTP/1.1 200 OK\r\n"
                << "Content-Type: application/json\r\n"
                << "Content-Length: " << reply.length() << "\r\n\r\n"
                << reply;             
        }
    }
    catch(std::runtime_error& e) { 
        cout << "ERROR " << e.what() << endl;
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
    }
}

void posts_new(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
	auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) { 
            auto user_table = accessCollection(dbCon, "sample_users");
            auto post_table = accessCollection(dbCon, "sample_posts");
            auto community_table = accessCollection(dbCon, "sample_communities");

            //extract data from POST
            ptree pt;
            read_json(request->content, pt);
            string post_title = pt.get<string>("post_title");
            string post_body = pt.get<string>("post_body");
            string c_name = pt.get<string> ("community_name");

            //server status message 
            cout << "Session " << session_id << " is creating a new post: '" << post_title << "' in '" << c_name << "'" << endl;

            //get community object
            auto cbb_doc = communityNameToCommunityDoc(community_table, c_name);
            if (!cbb_doc) {
                throw std::runtime_error("Could not find community by that name");
            }

            //check to see if we have permission to access this board
            auto u = sessionToUserObject(user_table, session_table, session_id);
            auto found = std::find(std::begin(u.communities_joined), std::end(u.communities_joined), cbb_doc->view()["_id"].get_oid().value);
			if (found == std::end(u.communities_joined)) {
			    throw std::runtime_error("User does not have access to this community");
			}

            // instantiate new post
			auto u_id = sessionToUserID(user_table, session_table, session_id);
			auto c_id = cbb_doc->view()["_id"].get_oid().value;
            auto new_post = Post(u_id, c_id, post_title, post_body, u.user_name);

            // insert new post into post table
            auto post_result = insertDocument(post_table, new_post.getBsonValue());
            if (!post_result) {
            	throw std::runtime_error("Error inserting new post");
            }
            auto post_id = post_result->inserted_id().get_oid().value;

            // insert post id into cbb
            addIDtoDocumentArray(community_table, c_id, "post_ids", post_id);

            std::string reply = "Successfully created new post";
            *response << "HTTP/1.1 200 OK\r\n"
                << "Content-Type: application/json\r\n"
                << "Content-Length: " << reply.length() << "\r\n\r\n"
                << reply;
        }
    }
    catch(std::runtime_error& e) { 
        cout << "ERROR " << e.what() << endl;
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
    }
}


void posts_edit(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
    auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) {
            auto user_table = accessCollection(dbCon, "sample_users");
            auto post_table = accessCollection(dbCon, "sample_posts");
            auto community_table = accessCollection(dbCon, "sample_communities");

            //extract data from POST
            ptree pt;
            read_json(request->content, pt);
            string post_id = pt.get<string>("post_id");
            string post_title = pt.get<string>("post_title");
            string post_body = pt.get<string>("post_body");

            //server status message 
            cout << "Session " << session_id << " is editing post_id: '" << post_id << "'" << endl;

            //get post from db
            auto post_obj = postIDtoObject(post_table, post_id);
            bsoncxx::oid pid{post_id};

            //check to see if we have permission to edit
            auto u_id = sessionToUserID(user_table, session_table, session_id);
            if (post_obj.user_id != u_id) {
                throw std::runtime_error("You cannot edit a post that you did not create");
            }

            //instantiate new post
            auto new_post = Post(post_obj);
            new_post.title = post_title;
            new_post.body = post_body;

            //replace old post
            auto result = replaceDocument(post_table, pid, new_post.getBsonValue());

            std::string reply = "Successfully edited post";
            *response << "HTTP/1.1 200 OK\r\n"
                << "Content-Type: application/json\r\n"
                << "Content-Length: " << reply.length() << "\r\n\r\n"
                << reply;
        }
    }
    catch(std::runtime_error& e) { 
        cout << "ERROR " << e.what() << endl;
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
    }
}


void posts_delete(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
    auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) {
            auto user_table = accessCollection(dbCon, "sample_users");
            auto post_table = accessCollection(dbCon, "sample_posts");
            auto community_table = accessCollection(dbCon, "sample_communities");
            auto comment_table = accessCollection(dbCon, "sample_comments");


            //extract data from POST
            ptree pt;
            read_json(request->content, pt);
            string post_id = pt.get<string>("post_id");

            //server status message 
            cout << "Session " << session_id << " is deleting post_id: '" << post_id << "'" << endl;

            //get post from db
            auto post_doc = findOneDocumentByID(post_table, post_id);
            if (!post_doc) {
                throw std::runtime_error("Could not find post");
            }
            auto p_id = post_doc->view()["_id"].get_oid().value;
            auto comm_id = post_doc->view()["community_id"].get_oid().value;


            //check to see if we have permission to edit
            auto u_id = sessionToUserID(user_table, session_table, session_id);
            if (post_doc->view()["user_id"].get_oid().value != u_id) {
                throw std::runtime_error("You cannot delete a post that you did not create");
            }

            //delete corresponding comments
            deleteManyDocumentsByKeyAndID(comment_table, "post_id", p_id);

            //delete post_id from community
            deleteIDfromDocumentArray(community_table, comm_id, "post_ids", p_id);

            //delete post from post table
            deleteDocumentByID(post_table, p_id);

            std::string reply = "Successfully deleted post";
            *response << "HTTP/1.1 200 OK\r\n"
                << "Content-Type: application/json\r\n"
                << "Content-Length: " << reply.length() << "\r\n\r\n"
                << reply;
        }
    }
    catch(std::runtime_error& e) { 
        cout << "ERROR " << e.what() << endl;
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
    }
}
