#pragma once

#include <iostream>
#include <string.h>
#include <ctime>
#include <vector>
#include "Utlityfunctions.h"
#include <time.h>

#include <cstdint>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

struct Post
{
	Post(bsoncxx::oid u_id, bsoncxx::oid community_id, std::string ttle, std::string bdy, std::string u_name);

	Post(const Post &post); //Copy constructor

	bsoncxx::builder::stream::document convertToBson(); //Returns a bson document

	bsoncxx::document::value getBsonValue(); //Returns a bson value

	void insertCommentID(bsoncxx::oid comment_id);

	//Post elements
	bsoncxx::oid user_id, community_id;
	std::tm timestamp; //Universal time
	std::string title, body, username;
	std::vector<bsoncxx::oid> comment_ids;

	//Not sure what we want for post details yet (pics, etc)
};

