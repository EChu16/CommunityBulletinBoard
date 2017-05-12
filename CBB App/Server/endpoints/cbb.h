#pragma once 

#include "../database.h"
#include "../auth.h"
#include "functions.h"

typedef CBB::Server<CBB::HTTPS> HttpsServer;
typedef CBB::Client<CBB::HTTPS> HttpsClient;

using namespace std;
using namespace boost::property_tree;

void cbb_home(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
    auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
    	string session_id = extractSession(request);
    	if (isValidSession(session_table, session_id)) {
            auto user_table = accessCollection(dbCon, "sample_users");
            auto community_table = accessCollection(dbCon, "sample_communities");

            //extract the user's communities
            auto username = sessionToUsername(session_table, session_id);
            auto user_doc = findOneDocumentByKeyAndValue(user_table, "username", username);
            if (!user_doc) {
                throw std::runtime_error("User doc not found!");
            }
            auto users_communities = user_doc->view()["communities_joined"].get_array().value;
            bool community_found = false;

            //build response as list of comma-separated JSON docs  --  [ {json}, {json}, {json} ]
            std::string reply = "[";
            for (auto it = users_communities.begin(); it != users_communities.end(); ++it ) {
                if (it != users_communities.begin()) {
                    reply += ", ";
                }
                auto community = findOneDocumentByID(community_table, it->get_oid().value);
                if (!community) {
                    throw std::runtime_error("Community not found!");
                }
                reply += bsoncxx::to_json(community->view());
                community_found = true;
            }
            //append user doc
            if (community_found) {
                reply += ", ";
            }
            reply += bsoncxx::to_json(user_doc->view());
            reply += " ]";

            //server status message 
            cout << "Session " << session_id << " requested /cbb/home" << endl;

            //send response
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

void cbb_new(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
    auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) { 
            auto user_table = accessCollection(dbCon, "sample_users");
            auto community_table = accessCollection(dbCon, "sample_communities");

            //extract community name from POST
            ptree pt;
            read_json(request->content, pt);
            string c_name = pt.get<string>("community_name");
            string c_desc = pt.get<string>("community_desc");

            //server status message 
            cout << "Session " << session_id << " is creating a new community '" << c_name << "'" << endl;

            // check to make sure name is unique
            auto cbb_doc = findOneDocumentByKeyAndValue(community_table, "community_name", c_name);
            if (cbb_doc) {   // we found the cbb name in the database
                throw std::runtime_error("CBB board name already exists!");
            }

            //if unique name
            else { 
                // create a new community
                auto new_community = Community(c_name);
                auto user_id = sessionToUserID(user_table, session_table, session_id);
                new_community.owner_id = user_id;
                new_community.description = c_desc;
                
                // insert user_id into community.user_ids
                new_community.insertUserID(user_id);

                // insert into the community table
                bsoncxx::document::value new_community_doc = new_community.getBsonValue();
                auto insert_result = insertDocument(community_table, new_community_doc);
                bsoncxx::oid c_id = insert_result->inserted_id().get_oid().value;

                // insert c_id into User.communities_joined and update user in user_table
                auto result = addIDtoDocumentArray(user_table, user_id, "communities_joined", c_id);

                std::string reply = "Successfully created new community";
                *response << "HTTP/1.1 200 OK\r\n"
                    << "Content-Type: application/json\r\n"
                    << "Content-Length: " << reply.length() << "\r\n\r\n"
                    << reply;
            }
        }
    }
    catch(std::runtime_error& e) { 
        cout << "ERROR " << e.what() << endl;
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
    }
}

void cbb_search(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
    auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) { 
            //if we have a valid user session
            auto user_table = accessCollection(dbCon, "sample_users");
            auto community_table = accessCollection(dbCon, "sample_communities");

            //extract data from POST
            ptree pt;
            read_json(request->content, pt);
            string search_name = pt.get<string>("search_name");
            string search_name_lowercase = search_name;
            std::transform(search_name_lowercase.begin(), search_name_lowercase.end(), search_name_lowercase.begin(), ::tolower);

            //server status message 
            cout << "Session " << session_id << " is searching for community: " << search_name << endl;

            //get entire cbb table
            auto cbb_cursor = findAllDocuments(community_table);

            std::string reply = "[";
            size_t board_count = 0;
            //for each cbb
            for (auto cbb : cbb_cursor) {

                //if cbb_name.find(search_name) != string::npos
                std::string cbb_name = cbb["community_name"].get_utf8().value.to_string();
                std::string cbb_name_lower = cbb_name;
                std::transform(cbb_name_lower.begin(), cbb_name_lower.end(), cbb_name_lower.begin(), ::tolower);

                if (cbb_name_lower.find(search_name_lowercase) != std::string::npos) {

                    //add the board to the list to return
                    if (board_count > 0) {
                        reply += ", ";
                    }
                    reply += bsoncxx::to_json(cbb);
                    board_count++;
                }
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

void cbb_userlist(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
    auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) { 
            //if we have a valid user session
            auto user_table = accessCollection(dbCon, "sample_users");
            auto community_table = accessCollection(dbCon, "sample_communities");

            //extract data from POST
            ptree pt;
            read_json(request->content, pt);
            string c_name = pt.get<string>("community_name");

            //server status message 
            cout << "Session " << session_id << " is requesting a userlist from community '" << c_name << "'"<< endl;

            //get community document
            auto cbb_doc = communityNameToCommunityDoc(community_table, c_name);

            //get user_ids array from community
            auto user_ids = cbb_doc->view()["user_ids"].get_array().value;
            std::string reply = "[";

            //for each user_id, get info from user table and append it to reply
            //build response as list of comma-separated JSON docs  --  [ {json}, {json}, {json} ]
            for (auto it = user_ids.begin(); it != user_ids.end(); ++it) {
                if (it != user_ids.begin()) {
                    reply += ", ";
                }
                auto user = findOneDocumentByID(user_table, it->get_oid().value);
                if (!user) {
                    throw std::runtime_error("User not found!");
                }
                reply += bsoncxx::to_json(user->view());
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

void cbb_join(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
    auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) { 
            //if we have a valid user session
            auto user_table = accessCollection(dbCon, "sample_users");
            auto community_table = accessCollection(dbCon, "sample_communities");

            //extract data from POST
            ptree pt;
            read_json(request->content, pt);
            string c_name = pt.get<string>("community_name");

            //server status message 
            cout << "Session " << session_id << " is joining community: '" << c_name << "'" << endl;

            //get community document and cbb/user ids
            auto cbb_doc = communityNameToCommunityDoc(community_table, c_name);
            auto c_id = cbb_doc->view()["_id"].get_oid().value;
            auto u_id = sessionToUserID(user_table, session_table, session_id);

            // insert u_id into Community.user_ids
            auto comm_result = addIDtoDocumentArray(community_table, c_id, "user_ids", u_id);

            // insert c_id into User.communities_joined 
            auto user_result = addIDtoDocumentArray(user_table, u_id, "communities_joined", c_id);

            std::string reply = "User has joined community '" + c_name + "'";
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

void cbb_view(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
    auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) { 
            //if we have a valid user session
            auto user_table = accessCollection(dbCon, "sample_users");
            auto community_table = accessCollection(dbCon, "sample_communities");

            //extract data from POST
            ptree pt;
            read_json(request->content, pt);
            string c_name = pt.get<string>("community_name");

            //server status message 
            cout << "Session " << session_id << " is viewing community: '" << c_name << "'" << endl;

            //get community document and return it back as JSON
            auto cbb_doc = communityNameToCommunityDoc(community_table, c_name);
            std::string reply = "[" + bsoncxx::to_json(cbb_doc->view()) + "]";

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

void cbb_hasuser(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
    auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) { 
            //if we have a valid user session
            auto user_table = accessCollection(dbCon, "sample_users");
            auto community_table = accessCollection(dbCon, "sample_communities");

            //extract data from POST
            ptree pt;
            read_json(request->content, pt);
            string c_name = pt.get<string>("community_name");

            //server status message 
            cout << "Checking if session " << session_id << " is in community: '" << c_name << "'" << endl;

            //get community document
            auto cbb_doc = communityNameToCommunityDoc(community_table, c_name);
            auto c_id = cbb_doc->view()["_id"].get_oid().value;
            auto user_doc = sessionToUserDoc(user_table, session_table, session_id);
            auto users_communities = user_doc->view()["communities_joined"].get_array().value;

            bool user_in_community = false;
            for (auto it = users_communities.begin(); it != users_communities.end(); ++it) {
                if(it->get_oid().value == c_id) {
                    user_in_community = true;
                    break;
                }
            }

            std::string reply = "";
            reply = (user_in_community) ? "Leave" : "Join";

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

void cbb_leave(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
    auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
        string session_id = extractSession(request);
        if (isValidSession(session_table, session_id)) { 
            //if we have a valid user session
            auto user_table = accessCollection(dbCon, "sample_users");
            auto community_table = accessCollection(dbCon, "sample_communities");

            //extract data from POST
            ptree pt;
            read_json(request->content, pt);
            string c_name = pt.get<string>("community_name");

            //server status message 
            cout << "Session " << session_id << " is leaving community: '" << c_name << "'" << endl;

            //get community document and cbb/user ids
            auto cbb_doc = communityNameToCommunityDoc(community_table, c_name);
            auto c_id = cbb_doc->view()["_id"].get_oid().value;
            auto u_id = sessionToUserID(user_table, session_table, session_id);

            // remove c_id from User.communities_joined 
            auto user_result = deleteIDfromDocumentArray(user_table, u_id, "communities_joined", c_id);

            std::string reply = "User has left community '" + c_name + "'";
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

