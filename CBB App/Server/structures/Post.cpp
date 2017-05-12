#include "Post.h"

Post::Post(bsoncxx::oid u_id, bsoncxx::oid c_id, std::string ttle, std::string bdy, std::string u_name)
{
	user_id = u_id;
	title = ttle;
	body = bdy;
	community_id = c_id;
	username = u_name;

	//Get time stamp
	time_t curr_time = std::time(nullptr);
	timestamp = *std::localtime(&curr_time); //gets the tm struct (Local server time)
}

Post::Post(const Post &post)
{
	//Oid copy
	user_id = post.user_id;
	community_id = post.community_id;

	//String copy
	title = post.title;
	body = post.body;
	username = post.username;

	//Copy comment vector
	comment_ids = copyOidVector(post.comment_ids);

	//Copy timestamp
	timestamp = copyTmStruct(post.timestamp);
}

//Returns a usuable bson document from class
bsoncxx::builder::stream::document Post::convertToBson()
{
	//Copy comment vector values into array
	auto comment_array = bsoncxx::builder::basic::array{};
	for (size_t i = 0; i < comment_ids.size(); ++i)
	{
		comment_array.append(comment_ids[i]);
	}

	//Build the document
	bsoncxx::builder::stream::document builder{};
	builder
		<< "user_id" << user_id
		<< "username" << username
		<< "timestamp" << std::asctime(&timestamp)
		<< "community_id" << community_id
		<< "title" << title
		<< "body" << body
		<< "comment_ids" << comment_array; 
	return builder;
}


//Returns a bson value
bsoncxx::document::value Post::getBsonValue()
{
	return convertToBson() << bsoncxx::builder::stream::finalize; //It is imperative that document::values outlive any document::views that use them.
}

void Post::insertCommentID(bsoncxx::oid comment_id)
{
	//Search through vector for comment id
	for (size_t i = 0; i < comment_ids.size(); ++i)
	{
		if (comment_ids[i] == comment_id) return; //exits if comment already there
	}
	comment_ids.push_back(comment_id);
}
