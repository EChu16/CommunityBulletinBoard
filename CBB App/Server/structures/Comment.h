#pragma once

#include <iostream>
#include <string.h>
#include <ctime>
#include "Utlityfunctions.h"
#include <time.h>

#include <cstdint>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>



struct Comment
{
	Comment(bsoncxx::oid u_id, bsoncxx::oid p_id, std::string txt, std::string u_name);

	Comment(const Comment &comment);

	bsoncxx::builder::stream::document convertToBson(); //Returns a bson document

	bsoncxx::document::value getBsonValue(); //Returns a bson value

	//Comment elements
	bsoncxx::oid user_id, post_id;
	std::tm timestamp; //Universal time
	std::string text, username;
};