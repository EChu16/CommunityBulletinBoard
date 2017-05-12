#pragma once

#include <iostream>
#include <string.h>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include "structures/Community.h"
#include "structures/Comment.h"
#include "structures/Post.h"
#include "structures/User.h"

#include "database.h"

mongocxx::stdx::optional<mongocxx::result::insert_one> registerUser(mongocxx::collection& users_table, mongocxx::collection& sessions_table, const std::string& username, const std::string& entered_password) {
	auto user_doc = findOneDocumentByKeyAndValue(users_table, "username", username);
	
	if (user_doc) {   // we found the username in the database
		throw std::runtime_error("Username already exists!");
	}
	else { 
		// create a new user
		auto new_user = User(username, entered_password);
		
		// insert into the user table
		bsoncxx::document::value new_user_doc = new_user.convertToBson() << bsoncxx::builder::stream::finalize;
		insertDocument(users_table, new_user_doc);
		
		// create session
		auto builder = bsoncxx::builder::stream::document{};
		bsoncxx::document::value user_session = builder << "username" << username << finalize;

  		// Delete all sessions with this username (they shouldn't exist but doesn't cost much to check and avoid weird auth errors)
  		auto delete_result = sessions_table.delete_many(document{} << "username" << username << finalize);
  	
  		// Insert username into sessions table and return ID of document
  		return insertDocument(sessions_table, user_session);
  	}
}

std::string sessionToUsername(mongocxx::collection& session_table, const std::string& s_id) {
	auto session_doc = findOneDocumentByID(session_table, s_id);
	if (session_doc) {
		auto view_session = session_doc->view();
		return view_session["username"].get_utf8().value.to_string();
	}
	else {
		throw std::runtime_error("sessionToUsername could not find session id");
	}
}

stdx::optional<bsoncxx::document::value> sessionToUserDoc(mongocxx::collection& user_table, mongocxx::collection& session_table, const std::string& s_id) {
	std::string username = sessionToUsername(session_table, s_id);
	auto user_doc = findOneDocumentByKeyAndValue(user_table, "username", username);
	if (!user_doc) {
		throw std::runtime_error("sessionToUserDoc could not find username");
	}
	return user_doc;
}

stdx::optional<bsoncxx::document::value> communityNameToCommunityDoc(mongocxx::collection& community_table, const std::string& c_name) {
	auto cbb_doc = findOneDocumentByKeyAndValue(community_table, "community_name", c_name);
	if (!cbb_doc) {
		throw std::runtime_error("communityNameToCommunityDoc could not locate community");
	}
	return cbb_doc;
}

bsoncxx::oid sessionToUserID(mongocxx::collection& user_table, mongocxx::collection& session_table, const std::string& s_id) {
	std::string username = sessionToUsername(session_table, s_id);
	auto user_doc = findOneDocumentByKeyAndValue(user_table, "username", username);
	if (user_doc) {
		auto view_user = user_doc->view();
		auto user_id = view_user["_id"];
		return user_id.get_oid().value;
	}
	else {
		throw std::runtime_error("sessionToUserID could not find username in user table");
	}
}

User sessionToUserObject(mongocxx::collection& user_table, mongocxx::collection& session_table, std::string& s_id) {
	std::string username = sessionToUsername(session_table, s_id);
	auto user_doc = findOneDocumentByKeyAndValue(user_table, "username", username);
	if (user_doc) {
		auto view_user = user_doc->view();
		string username = view_user["username"].get_utf8().value.to_string();
		string password = view_user["password"].get_utf8().value.to_string();
		auto u = User(username, password);
/*
//NOTE: Removing references to embedded documents (settings and profile) until the end of project
		u.settings = view_user["settings"].get_document();
		u.profile = view_user["profile"].get_document();
*/
		auto comm_joined_array = view_user["communities_joined"].get_array().value;
		for (auto item : comm_joined_array) {
			u.addCommunity(item.get_oid().value);
		}
		return u;
	}
	else {
		throw std::runtime_error("sessionToUserObject could not find username in user table");
	}
}

Community communityIDtoObject(mongocxx::collection& community_table, std::string& c_id) {
	auto comm_doc = findOneDocumentByID(community_table, c_id);
	auto view_comm = comm_doc->view();
	auto c = Community(view_comm["community_name"].get_utf8().value.to_string());
	auto userid_array = view_comm["user_ids"].get_array().value;
	auto postid_array = view_comm["post_ids"].get_array().value;
	for (auto item : userid_array) {
		c.insertUserID(item.get_oid().value);
	}
	for (auto item : postid_array) {
		c.insertPostID(item.get_oid().value);
	}
	c.owner_id = view_comm["owner_id"].get_oid().value;
	return c;
}

Comment commentIDtoObject(mongocxx::collection& comment_table, std::string& c_id) {
	auto comm_doc = findOneDocumentByID(comment_table, c_id);
	if (!comm_doc) {
		throw std::runtime_error("Could not find comment");
	}
	auto view_comm = comm_doc->view();
	bsoncxx::oid u_id = view_comm["user_id"].get_oid().value;
	bsoncxx::oid p_id = view_comm["post_id"].get_oid().value;
	std::string text = view_comm["text"].get_utf8().value.to_string();
	std::string username = view_comm["username"].get_utf8().value.to_string();
	auto c = Comment(u_id, p_id, text, username);
    std::string timestamp = view_comm["timestamp"].get_utf8().value.to_string();
    c.timestamp = convertStringToTm(timestamp);
	return c;
}

Post postIDtoObject(mongocxx::collection& post_table, std::string& p_id) {
	auto post_doc = findOneDocumentByID(post_table, p_id);
	if (!post_doc) {
		throw std::runtime_error("Could not find post");
	}
	auto view_post = post_doc->view();
	bsoncxx::oid u_id = view_post["user_id"].get_oid().value;
	bsoncxx::oid community_id = view_post["community_id"].get_oid().value;
	std::string title = view_post["title"].get_utf8().value.to_string();
	std::string body = view_post["body"].get_utf8().value.to_string();
	std::string username = view_post["username"].get_utf8().value.to_string();
	auto p = Post(u_id, community_id, title, body, username);
	auto commentid_array = view_post["comment_ids"].get_array().value;
	for (auto item : commentid_array) {
		p.insertCommentID(item.get_oid().value);
	}
    std::string timestamp = view_post["timestamp"].get_utf8().value.to_string();
    p.timestamp = convertStringToTm(timestamp);
	return p;
}


