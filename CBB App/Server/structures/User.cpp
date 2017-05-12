#include "User.h"

User::User(std::string u_name, std::string pssword)
{
	user_name = u_name;
	password = pssword;
	//Not sure what the settings are at the moment
	settings
		<< "notifications" << true
		<< "time_zone" << "EST";
	//TODO: Add more stuff here
	profile
		<< "bio" << "Nothing here";
	//TODO: Add more stuff here
}

//Deep copy constructor
User::User(const User &user)
{
	//Copy strings
	user_name = user.user_name;
	password = user.password;

	//Copy vectors
	communities_joined = copyOidVector(user.communities_joined);

	//Copy documents
	settings = copyDocument(user.settings);
	profile = copyDocument(user.profile);
}

//Returns a usuable bson document from class
bsoncxx::builder::stream::document User::convertToBson()
{
    //Copy community vector values into array
    auto community_array = bsoncxx::builder::basic::array{};
    for (size_t i = 0; i < communities_joined.size(); ++i)
    {
        community_array.append(communities_joined[i]);
    }

	bsoncxx::builder::stream::document builder{};
	builder
		//<< "settings" << settings
		//<< "profile" << profile
		<< "username" << user_name
		<< "password" << password
		<< "communities_joined" << community_array;
	return builder;
}

//Returns a bson value
bsoncxx::document::value User::getBsonValue()
{
	return convertToBson() << bsoncxx::builder::stream::finalize; //It is imperative that document::values outlive any document::views that use them.
}



//Adds community to list of communities joined
void User::addCommunity(bsoncxx::oid community_id)
{
	//Search through vector for community id
	for (size_t i = 0; i < communities_joined.size(); ++i)
	{
		if (communities_joined[i] == community_id) return; //returns if it finds a duplicate community id
	}
	communities_joined.push_back(community_id);
}
