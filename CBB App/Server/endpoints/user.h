#pragma once 

#include "../database.h"
#include "../auth.h"
#include "functions.h"

typedef CBB::Server<CBB::HTTPS> HttpsServer;
typedef CBB::Client<CBB::HTTPS> HttpsClient;

using namespace std;
using namespace boost::property_tree;


void user_login(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
	auto user_table = accessCollection(dbCon, "sample_users");
    auto session_table = accessCollection(dbCon, "sample_sessions");

    try {
        ptree pt;
        read_json(request->content, pt);

        string username = pt.get<string>("username"); // extract the username from the json request
        string password = pt.get<string>("password"); // extract the password from the json request
        cout << "Attempting authentication...\tUser: " << username << "\tPassword: " << password << endl;

        //try to authenticate the user, then send the response with new cookie
        auto session_doc = authenticateUser(user_table, session_table, username, password);
        bsoncxx::oid oid = session_doc->inserted_id().get_oid().value;
        cout << "Authenticated user (" << username << ") with id: " << oid.to_string() << endl;

        //craft response
        std::string reply = "User has successfully logged in";
        *response << "HTTP/1.1 200 OK\r\n"
              << "Content-Type: application/json\r\n"
              << "Set-Cookie: " << "session_id=" << oid.to_string() << "; Path=/; Secure \r\n"
              << "Content-Length: " << reply.length() << "\r\n\r\n"
              << reply;
    }
    catch(std::runtime_error& e) { //report authentication errors to console and return 400 with that error as the response message
        cout << "ERROR " << e.what() << endl;
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
    }
}

void user_register(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
	auto user_table = accessCollection(dbCon, "sample_users");
    auto session_table = accessCollection(dbCon, "sample_sessions");

    try {
        ptree pt;
        read_json(request->content, pt);

        string username = pt.get<string>("username"); // extract the username from the json request
        string password = pt.get<string>("password"); // extract the password from the json request
        cout << "Attempting registration...\tUser: " << username << "\tPassword: " << password << endl;

        //try to register the user, then send the response with new cookie
        auto session_doc = registerUser(user_table, session_table, username, password);
        bsoncxx::oid oid = session_doc->inserted_id().get_oid().value;
        cout << "Authenticated new user (" << username << ") with id: " << oid.to_string() << endl;

        //craft response
        std::string reply = "Successfully registered and authenticated";
        *response << "HTTP/1.1 200 OK\r\n"
              << "Content-Type: application/json\r\n"
              << "Set-Cookie: " << "session_id=" << oid.to_string() << "; Path=/; Secure \r\n"
              << "Content-Length: " << reply.length() << "\r\n\r\n"
              << reply;
    }
    catch(std::runtime_error& e) { //report authentication errors to console and return 400 with that error as the response message
        cout << "ERROR " << e.what() << endl;
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
    }
}

void user_logout(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
    auto session_table = accessCollection(dbCon, "sample_sessions");
    try {
    	//get session_id from headers
    	string session_id = extractSession(request);

    	//check for valid session
    	if (isValidSession(session_table, session_id)) {

    		//try to delete session_id document from session table
    		auto deleteSession = deleteDocumentByID(session_table, session_id);

    		//if successful deletion
    		if (deleteSession) {
    			cout << "Logout successful from session_id: " << session_id << endl;
    			std::string reply = "Successfully logged out";
            	*response << "HTTP/1.1 200 OK\r\n"
                  << "Content-Type: application/json\r\n"
                  << "Content-Length: " << reply.length() << "\r\n\r\n"
                  << reply;
    		}

    		//if not successful
    		else {
    			throw std::runtime_error("Unable to locate session ID for deletion");
    		}
    	}
    }
    catch(std::runtime_error& e) { 
        cout << "ERROR " << e.what() << endl;
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
    }
}

// void user_settings(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
// 	exit;
// }

// void user_settings_get(shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request, mongocxx::database dbCon) {
// 	exit;
// }


