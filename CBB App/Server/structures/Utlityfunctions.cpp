#include "Utlityfunctions.h"


//Returns a copy of a document
bsoncxx::builder::stream::document copyDocument(const bsoncxx::builder::stream::document &document)
{
	bsoncxx::builder::stream::document doc{};
	//Iterates the document to get each element 
	for (bsoncxx::document::view::const_iterator iter = document.view().begin(); iter != document.view().end(); ++iter)
	{
		//String case
		if (bsoncxx::type::k_utf8 == iter->type())
		{
			doc << iter->key() << iter->get_utf8().value.to_string();
		}
		//Boolean case
		else if (bsoncxx::type::k_bool == iter->type())
		{
			if (iter->get_bool().value == 1)
			{
				doc << iter->key() << true;
			} else
			{
				doc << iter->key() << false;
			}
		}
		//Integer case
		else if (bsoncxx::type::k_int32 == iter->type())
		{
			doc << iter->key() << iter->get_int32().value;
		}
		//Array case
		else if (bsoncxx::type::k_array == iter->type())
		{
			doc << iter->key() << iter->get_array().value; //Puts an array 
		}
		//Sub document case
		else if (bsoncxx::type::k_document == iter->type())
		{
			doc << iter->key() << iter->get_document().value;
			//if(doc[iter->key()].type() == bsoncxx::type::k_document) cout << "Doc type" << endl;
		}
	}

	return doc;
}

//Returns a copy of a vector of object ids 
std::vector<bsoncxx::oid> copyOidVector(const std::vector<bsoncxx::oid> &oid_vector)
{
	std::vector<bsoncxx::oid> new_oid_vector;
	for (size_t index = 0; index < oid_vector.size(); ++index)
	{
		new_oid_vector.push_back(oid_vector[index]);
	}
	return new_oid_vector;
}

//string has following format Www Mmm dd hh:mm:ss yyyy, ei.Wed Feb 13 15:46:11 2017
std::tm convertStringToTm(std::string timestampStr)
{
	std::tm timestamp;

	//Tokenize string
	std::vector<std::string> string_vector;
	string_vector = split(timestampStr);

	//Days since Sunday
	timestamp.tm_wday = convertDayToInt(string_vector[0]);

	//months since January 	0-11
	timestamp.tm_mon = convertToMonthToNumerical(string_vector[1]);

	//Day of the month 	1-31
	timestamp.tm_mday = atoi(string_vector[2].c_str());

	//Hour minute seconds
	timestamp.tm_hour = atoi(string_vector[3].c_str()); //hours since midnight 0-23
	timestamp.tm_min = atoi(string_vector[4].c_str()); //minutes after the hour 0-59
	timestamp.tm_sec = atoi(string_vector[5].c_str()); //seconds after the minute 0-61

	//year since 1900
	timestamp.tm_year = atoi(string_vector[6].c_str()) - 1900;

	//Other non important fields of tm
	timestamp.tm_isdst = -1; //daylight savings flag
	timestamp.tm_yday = 0;
    
    return timestamp;
}

//Delimits for white space and semicolon, returns a vector of the delimited strings
std::vector<std::string> split(const std::string &s)
{
	std::vector<std::string> tokens;
	char charArray[512]; //Used to store the full string
	char *delimitedstring;

	strncpy(charArray, s.c_str(), sizeof(charArray)); //Copy string into char array

	//Start pushing into vector
	delimitedstring = strtok(charArray, " :"); //Delimit for white space and semicolon
	std::string str(delimitedstring); //Convert back from char array to string
	tokens.push_back(str);

	while ( (delimitedstring = strtok(NULL, " :")) ) //Delimits for white space and semicolon
	{
		std::string str(delimitedstring); //Creates a string from char array
		tokens.push_back(str); //Pushes string into vector
	}

	return tokens;
}

//Converts month name into numerical value
int convertToMonthToNumerical(std::string monthName)
{
	if (monthName == "Jan")
	{
		return 0;
	}
	else if (monthName == "Feb")
	{
		return 1;
	}
	else if (monthName == "Mar")
	{
		return 2;
	}
	else if (monthName == "Apr")
	{
		return 3;
	}
	else if (monthName == "May")
	{
		return 4;
	}
	else if (monthName == "Jun")
	{
		return 5;
	}
	else if (monthName == "Jul")
	{
		return 6;
	}
	else if (monthName == "Aug")
	{
		return 7;
	}
	else if (monthName == "Sep")
	{
		return 8;
	}
	else if (monthName == "Oct")
	{
		return 9;
	}
	else if (monthName == "Nov")
	{
		return 10;
	} 
	else if (monthName == "Dec")
	{
		return 11;
	}
	else
	{
		return -1; //Error
	}
}

//Converts day name into a numerical value
int convertDayToInt(std::string dayName)
{
	if (dayName == "Sun")
	{
		return 0;
	} else if (dayName == "Mon")
	{
		return 1;
	} else if (dayName == "Tue")
	{
		return 2;
	} else if (dayName == "Wed")
	{
		return 3;
	} else if (dayName == "Thu")
	{
		return 4;
	} else if (dayName == "Fri")
	{
		return 5;
	} else if (dayName == "Sat")
	{
		return 6;
	} else
	{
		return -1; //Error
	}
}

//Makes a copy of a tm structure
std::tm copyTmStruct(const std::tm &tmStruct)
{
	std::tm tmcpy;

	//Days since Sunday
	tmcpy.tm_wday = tmStruct.tm_wday;

	//months since January 	0-11
	tmcpy.tm_mon = tmStruct.tm_mon;

	//Day of the month 	1-31
	tmcpy.tm_mday = tmStruct.tm_mday;

	//Hour minute seconds
	tmcpy.tm_hour = tmStruct.tm_hour;
	tmcpy.tm_min = tmStruct.tm_min;
	tmcpy.tm_sec = tmStruct.tm_sec;

	//year since 1900
	tmcpy.tm_year = tmStruct.tm_year;

	//Other non important fields of tm
	tmcpy.tm_isdst = tmStruct.tm_isdst;
	tmcpy.tm_yday = tmStruct.tm_yday;

	return tmcpy;
}
