#pragma once

#include <iostream>
#include <string.h>

#include <cstdint>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include "database.h"

typedef CBB::Server<CBB::HTTPS> HttpsServer;
typedef CBB::Client<CBB::HTTPS> HttpsClient;
using namespace std;
using namespace boost::property_tree;

mongocxx::stdx::optional<mongocxx::result::insert_one> authenticateUser(mongocxx::collection& users_coll, mongocxx::collection& sessions_coll, std::string& username, std::string& entered_password) {
	auto user_doc = findOneDocumentByKeyAndValue(users_coll, "username", username);
	if (user_doc) {   // we found the username in the database
		// change the read-only binary type to a viewable type
		bsoncxx::document::view view_user = user_doc->view();

		// set user_password equal to the password in the database
		auto user_password = view_user["password"].get_utf8().value.to_string();
		
		if (entered_password == user_password) { //if the passwords match
			// build a session document
			auto builder = bsoncxx::builder::stream::document{};
			bsoncxx::document::value user_session = builder << "username" << username << finalize;

  			// Delete all sessions with this username
  			auto delete_result = sessions_coll.delete_many(document{} << "username" << username << finalize);

  			// Insert username into sessions table -- returns type mongocxx::stdx::optional<result::insert_one> (ID of document)
  			return insertDocument(sessions_coll, user_session);
  		}
		else { // if passwords don't match
			throw std::runtime_error("Password is incorrect!");
		}
	}
	else { // if username doesn't exist in the database
		throw std::runtime_error("Username not found!");
	}
}

string extractSession(shared_ptr<HttpsServer::Request>& request) {
    stringstream stream;
    string tempstr;
    string id;
	
	//find the cookie header
    auto it = request->header.find("Cookie");

    // if the Cookie section exists
    if(it != request->header.end()) { 
    	stream << it->second;

    	// split the cookie string on the '=' delim  -- it should look like 'session_id=abcasdkfjha123aksdjfh' 
    	while(getline(stream, tempstr, '=')) { //split the cookie string on the '=' delim
    		if (tempstr != "session_id") {
    			id = tempstr;
    		}
    	}
		return id;
    }

    // the cookie section was not found
    else { 
    	throw std::runtime_error("Header missing cookies");
    }
}

bool isValidSession(mongocxx::collection& sessions_coll, string session_id) {
	// Search for the session_id in the database
	auto session_doc = findOneDocumentByID(sessions_coll, session_id);

	// If the session_id doesn't exist, session is not valid
    if (!session_doc) 
    	throw std::runtime_error("Session ID does not exist");

    // Session is valid
	return 1;
}

