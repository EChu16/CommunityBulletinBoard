#pragma once

#include <iostream>
#include <string.h>
#include <vector>
#include <sstream>
#include <ctime>
#include <time.h>
#include <cstring>

#include <cstdint>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>


//Returns a copy of a document
bsoncxx::builder::stream::document copyDocument(const bsoncxx::builder::stream::document &document);

//Returns a copy of a vector of object ids 
std::vector<bsoncxx::oid> copyOidVector(const std::vector<bsoncxx::oid> &oid_vector);

//Turns a time stamp string into a tm struct, string has following format Www Mmm dd hh:mm:ss yyyy, ei.Wed Feb 13 15:46:11 2017
std::tm convertStringToTm(std::string timestampStr);

//String tokenizer
std::vector<std::string> split(const std::string &s);

//Converts month name into numerical value
int convertToMonthToNumerical(std::string monthName);

//Converts day name into a numerical value
int convertDayToInt(std::string dayName);

//Makes a copy of a tm structure
std::tm copyTmStruct(const std::tm &tmStruct);