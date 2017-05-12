#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using namespace mongocxx;

collection accessCollection(database& db, const std::string& table) {
	return db[table];
}

cursor getDocsFromCollection( collection& coll) {
	return coll.find(document{} << finalize);
}

void printDoc(stdx::optional<bsoncxx::document::value>& doc) {
	if(doc) {
        std::cout << bsoncxx::to_json(doc->view()) << std::endl; 
    }
}

void printDoc(bsoncxx::document::value doc) {
	std::cout << bsoncxx::to_json(doc.view()) << std::endl; 
}

void printDocsInCursor(cursor& c) {
	for (auto&& doc : c) {
		std::cout << bsoncxx::to_json(doc) << std::endl;
	}
}

stdx::optional<bsoncxx::document::value> findOneDocumentByKeyAndValue(collection& coll, const std::string& key, const std::string& value) {
	return coll.find_one(document{} << key << value << finalize);
}

stdx::optional<bsoncxx::document::value> findOneDocumentByID(collection& coll, const std::string& id) {
	bsoncxx::oid oid{id};
	return coll.find_one(document{} << "_id" << oid << finalize);
}

stdx::optional<bsoncxx::document::value> findOneDocumentByID(collection& coll, const bsoncxx::oid id) {
	return coll.find_one(document{} << "_id" << id << finalize);
}

cursor findAllDocumentsByKeyAndValue(collection& coll, const std::string& key, const std::string& value) {
	return coll.find(document{} << key << value << finalize);
}

cursor findAllDocuments(collection& coll) {
	return coll.find(document{} << finalize);
}

stdx::optional<result::insert_one> insertDocument(collection& coll, const bsoncxx::document::value& doc_value) {
	bsoncxx::document::view view = doc_value.view();
	return coll.insert_one(view);
}

stdx::optional<mongocxx::result::update> addIDtoDocumentArray(collection& coll, bsoncxx::oid& doc_id, const std::string& key, bsoncxx::oid& new_val) {
	return coll.update_one(document{} << "_id" << doc_id << finalize, 
		document{} << "$addToSet" << open_document << key << new_val << close_document << finalize);
}

stdx::optional<mongocxx::result::update> deleteIDfromDocumentArray(collection& coll, bsoncxx::oid& doc_id, const std::string& key, bsoncxx::oid& val) {
	return coll.update_one(document{} << "_id" << doc_id << finalize, 
		document{} << "$pull" << open_document << key << val << close_document << finalize);
}

stdx::optional<result::replace_one> replaceDocument(collection& coll, bsoncxx::oid& doc_id, const bsoncxx::document::view replacement) {
	return coll.replace_one(document{} << "_id" << doc_id << finalize, replacement);
}

stdx::optional<bsoncxx::document::value> deleteDocumentByID(collection& coll, const std::string& id) {
	bsoncxx::oid oid{id};
	return coll.find_one_and_delete(document{} << "_id" << oid << finalize);
}

stdx::optional<bsoncxx::document::value> deleteDocumentByID(collection& coll, bsoncxx::oid& oid) {
	return coll.find_one_and_delete(document{} << "_id" << oid << finalize);
}

stdx::optional<result::delete_result> deleteManyDocumentsByKeyAndID(collection& coll, const std::string& key, bsoncxx::oid& oid) {
	return coll.delete_many(document{} << key << oid << finalize);
}

