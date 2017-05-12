#pragma once

#include <iostream>
#include <string.h>
#include <vector>
#include "Utlityfunctions.h"

#include <cstdint>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>


struct Community
{
	Community(std::string c_name);

	Community(const Community &community);

	bsoncxx::builder::stream::document convertToBson(); //Returns a bson document

	bsoncxx::document::value getBsonValue(); //Returns a bson value

	void insertUserID(bsoncxx::oid user_id);

	void insertPostID(bsoncxx::oid post_id);

	//Community elements
	std::string community_name, description;
	bsoncxx::builder::stream::document settings;
	bsoncxx::oid owner_id;
	std::vector<bsoncxx::oid> user_ids; //List of users that joined
	std::vector<bsoncxx::oid> post_ids; //List of posts posted in community
};

