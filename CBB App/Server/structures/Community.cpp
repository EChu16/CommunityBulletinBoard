#include "Community.h"

Community::Community(std::string c_name)
{
	community_name = c_name;
	description = "";

	//Not sure what the settings are at the moment
	settings
		<< "notifications" << true
		<< "time_zone" << "EST";
}

Community::Community(const Community &community)
{
	//Copy strings
	community_name = community.community_name;
	description = community.description;

	//Copy vectors
	user_ids = copyOidVector(community.user_ids);
	post_ids = copyOidVector(community.post_ids);
}

//Returns a usuable bson document from class
bsoncxx::builder::stream::document Community::convertToBson()
{
	//Copy user vector values into array
	auto user_array = bsoncxx::builder::basic::array{};
	for (size_t i = 0; i < user_ids.size(); ++i)
	{
		user_array.append(user_ids[i]);
	}

	//Copy post vector values into array
	auto post_array = bsoncxx::builder::basic::array{};
	for (size_t i = 0; i < post_ids.size(); ++i)
	{
		post_array.append(post_ids[i]);
	}
	
	bsoncxx::builder::stream::document builder{};
	builder
		//<< "settings" << settings
		<< "community_name" << community_name
		<< "description" << description
		<< "owner_id" << owner_id
		<< "user_ids" << user_array
		<< "post_ids" << post_array;
	return builder;
}

//Returns a bson value
bsoncxx::document::value Community::getBsonValue()
{
	return convertToBson() << bsoncxx::builder::stream::finalize; //It is imperative that document::values outlive any document::views that use them.
}

void Community::insertUserID(bsoncxx::oid user_id)
{
	//Search through vector for user id
	for (size_t i = 0; i < user_ids.size(); ++i)
	{
		if (user_ids[i] == user_id) return; //returns if it finds a duplicate user id
	}
	user_ids.push_back(user_id);
}

void Community::insertPostID(bsoncxx::oid post_id)
{
	//Search through vector for post id
	for (size_t i = 0; i < post_ids.size(); ++i)
	{
		if (post_ids[i] == post_id) return; //returns if it finds a duplicate post id
	}
	post_ids.push_back(post_id);
}
