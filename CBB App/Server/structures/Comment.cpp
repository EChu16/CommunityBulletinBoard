#include "Comment.h"


Comment::Comment(bsoncxx::oid u_id, bsoncxx::oid p_id, std::string txt, std::string u_name)
{
	user_id = u_id;
	post_id = p_id;
	text = txt;
	username = u_name;

	time_t curr_time = std::time(nullptr); 
	timestamp = *std::localtime(&curr_time); //gets the tm struct (local server time)
}

Comment::Comment(const Comment &comment)
{
	//Copy strings
	text = comment.text;
	username = comment.username;

	//Copy object ids
	user_id = comment.user_id;
	post_id = comment.post_id;

	//Copy timestamp
	timestamp = copyTmStruct(comment.timestamp);
}

//Returns a usuable bson document from class
bsoncxx::builder::stream::document Comment::convertToBson() //Returns a bson document
{
	bsoncxx::builder::stream::document builder{};
	builder
		<< "user_id" << user_id
		<< "post_id" << post_id
		<< "timestamp" << std::asctime(&timestamp)
		<< "text" << text
		<< "username" << username;
	return builder;
}

//Returns a bson value
bsoncxx::document::value Comment::getBsonValue()
{
	return convertToBson() << bsoncxx::builder::stream::finalize; //It is imperative that document::values outlive any document::views that use them.
}
