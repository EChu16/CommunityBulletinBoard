#pragma once 

#include "../database.h"
#include "../auth.h"
#include "functions.h"

typedef CBB::Server<CBB::HTTPS> HttpsServer;
typedef CBB::Client<CBB::HTTPS> HttpsClient;

using namespace std;
using namespace boost::property_tree;


void comments_get(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
	auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) { 
            auto post_table = accessCollection(dbCon, "sample_posts");
            auto comment_table = accessCollection(dbCon, "sample_comments");

            //extract data from POST
            ptree pt;
            read_json(request->content, pt);
            string post_id = pt.get<string>("post_id");
            
            //server status message 
            cout << "Session " << session_id << " is requesting all comments from post_id: " << post_id << endl;

            //get post from db
			auto post_doc = findOneDocumentByID(post_table, post_id);
            if (!post_doc) {
                throw std::runtime_error("Could not find post");
            }

            //get comment_ids array from post
            auto comment_ids = post_doc->view()["comment_ids"].get_array().value;
            std::string reply = "[";

            //for each comment_id, get info from comment table and append it to reply
            //build response as list of comma-separated JSON docs  --  [ {json}, {json}, {json} ]
            for (auto it = comment_ids.begin(); it != comment_ids.end(); ++it) {
                if (it != comment_ids.begin()) {
                    reply += ", ";
                }
                auto comment = findOneDocumentByID(comment_table, it->get_oid().value);
                if (!comment) {
                	throw std::runtime_error("Comment not found!");
                }
                reply += bsoncxx::to_json(comment->view());
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

void comments_new(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
	auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) { 
            auto user_table = accessCollection(dbCon, "sample_users");
            auto post_table = accessCollection(dbCon, "sample_posts");
            auto comment_table = accessCollection(dbCon, "sample_comments");

            //extract data from POST
            ptree pt;
            read_json(request->content, pt);
            string post_id = pt.get<string>("post_id");
            string text = pt.get<string> ("comment_text");

            //server status message 
            cout << "Session " << session_id << " is creating a new comment in post_id '" << post_id << "'" << endl;

            //get post doc
            auto post_doc = findOneDocumentByID(post_table, post_id);
            if (!post_doc) {
            	throw std::runtime_error("Post ID not found!");
            }

            // instantiate new comment
			auto u_id = sessionToUserID(user_table, session_table, session_id);
			bsoncxx::oid p_id{post_id};
			auto session_doc = findOneDocumentByID(session_table, session_id);
			auto username = session_doc->view()["username"].get_utf8().value.to_string();
			auto new_comment = Comment(u_id, p_id, text, username);

            // insert new comment into comment table
            auto comment_result = insertDocument(comment_table, new_comment.getBsonValue());
            if (!comment_result) {
            	throw std::runtime_error("Error inserting new comment");
            }
            auto comment_id = comment_result->inserted_id().get_oid().value;

            // insert comment id into post
            addIDtoDocumentArray(post_table, p_id, "comment_ids", comment_id);

            std::string reply = "Successfully created new comment";
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

void comments_edit(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
	auto session_table = accessCollection(dbCon, "sample_sessions");
	try {
	    string session_id = extractSession(request);
	    if (isValidSession(session_table, session_id)) {
	        auto user_table = accessCollection(dbCon, "sample_users");
	        auto comment_table = accessCollection(dbCon, "sample_comments");	

	        //extract data from POST
	        ptree pt;
	        read_json(request->content, pt);
	        string comment_id = pt.get<string>("comment_id");
	        string comment_text = pt.get<string>("comment_text");

	        //server status message 
	        cout << "Session " << session_id << " is editing comment_id: '" << comment_id << "'" << endl;	

	        //get comment from db
	        auto comment_obj = commentIDtoObject(comment_table, comment_id);
	        bsoncxx::oid cid{comment_id};	

	        //check to see if we have permission to edit
	        auto u_id = sessionToUserID(user_table, session_table, session_id);
	        if (comment_obj.user_id != u_id) {
	            throw std::runtime_error("You cannot edit a comment that you did not create");
	        }	

	        //instantiate new comment
	        auto new_comment = Comment(comment_obj);

	        new_comment.text = comment_text;
	        
	        //replace old comment
	        auto result = replaceDocument(comment_table, cid, new_comment.getBsonValue());	
	        std::string reply = "Successfully edited comment";
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

void comments_delete(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
    auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) {
            auto user_table = accessCollection(dbCon, "sample_users");
            auto post_table = accessCollection(dbCon, "sample_posts");
            auto comment_table = accessCollection(dbCon, "sample_comments");


            //extract data from POST
            ptree pt;
            read_json(request->content, pt);
            string comment_id = pt.get<string>("comment_id");

            //server status message 
            cout << "Session " << session_id << " is deleting comment_id: '" << comment_id << "'" << endl;

            //get comment from db
            auto comment_doc = findOneDocumentByID(comment_table, comment_id);
            if (!comment_doc) {
            	throw std::runtime_error("Could not find comment with that id!");
            }
            auto c_id = comment_doc->view()["_id"].get_oid().value;
            auto post_id = comment_doc->view()["post_id"].get_oid().value;


            //check to see if we have permission to edit
            auto u_id = sessionToUserID(user_table, session_table, session_id);
            if (comment_doc->view()["user_id"].get_oid().value != u_id) {
                throw std::runtime_error("You cannot delete a post that you did not create");
            }

            //delete comment_id from post
            deleteIDfromDocumentArray(post_table, post_id, "comment_ids", c_id);

            //delete comment from comment table
            deleteDocumentByID(comment_table, c_id);

            std::string reply = "Successfully deleted comment";
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


