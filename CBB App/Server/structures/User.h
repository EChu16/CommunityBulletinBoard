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

struct User
{
	User(std::string u_name, std::string pssword);

	User(const User &user); //Copy constructor

	bsoncxx::builder::stream::document convertToBson(); //Returns a bson document

	bsoncxx::document::value getBsonValue(); //Returns a bson value

	void addCommunity(bsoncxx::oid community_id); //Adds community to list of communities joined

	//User elements
	std::string user_name, password;
	bsoncxx::builder::stream::document settings{};
	bsoncxx::builder::stream::document profile{}; 
	std::vector<bsoncxx::oid> communities_joined; //communities that the user has joined
};




