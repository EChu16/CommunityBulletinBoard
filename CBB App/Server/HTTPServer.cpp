// Modified version of simple web server

#include "server_https.hpp"
#include "client_https.hpp"
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <vector>
#include <algorithm>
#include "crypto.hpp"

#include <cstdint>
#include <iostream>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include "endpoints/user.h"
#include "endpoints/cbb.h"
#include "endpoints/post.h"
#include "endpoints/comment.h"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using namespace std;
using namespace boost::property_tree;

typedef CBB::Server<CBB::HTTPS> HttpsServer;
typedef CBB::Client<CBB::HTTPS> HttpsClient;

//Added for the default_resource example
void default_resource_send(const HttpsServer &server, const shared_ptr<HttpsServer::Response> &response,
                           const shared_ptr<ifstream> &ifs);

mongocxx::database dbCon; // global database connection 

int main() {
    //HTTPS-server at port 8080 using 1 thread
    //Server Certificate and key required - See steps in README
    HttpsServer server("cert1.pem", "privkey1.pem");
    server.config.port=8080;

    cout << "Starting the server...\r\n";
    mongocxx::instance inst{};
    mongocxx::client client{mongocxx::uri{"mongodb://ec2624:DiasDeusCam@code.engineering.nyu.edu:27017/?authSource=ec2624"}};
    dbCon = client["ec2624"];

    server.resource["^/user/login$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        user_login(response, request, dbCon);
    };
    
    server.resource["^/user/register$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        user_register(response, request, dbCon);
    };

    server.resource["^/user/logout$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        user_logout(response, request, dbCon);
    };
    
    server.resource["^/cbb/home$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        cbb_home(response, request, dbCon);
    };

    server.resource["^/cbb/new$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        cbb_new(response, request, dbCon);
    };

    server.resource["^/cbb/search$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        cbb_search(response, request, dbCon);
    };

    server.resource["^/cbb/userlist$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        cbb_userlist(response, request, dbCon);
    };

    server.resource["^/cbb/join$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        cbb_join(response, request, dbCon);
    };

    server.resource["^/cbb/leave$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        cbb_leave(response, request, dbCon);
    };

    server.resource["^/cbb/view$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        cbb_view(response, request, dbCon);
    };

    server.resource["^/cbb/hasuser$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        cbb_hasuser(response, request, dbCon);
    };

    server.resource["^/posts/get$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        posts_get(response, request, dbCon);
    };

    server.resource["^/posts/new$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        posts_new(response, request, dbCon);
    };

    server.resource["^/posts/edit$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        posts_edit(response, request, dbCon);
    };
    
    server.resource["^/posts/delete$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        posts_delete(response, request, dbCon);
    };
    
    server.resource["^/comments/get$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        comments_get(response, request, dbCon);
    };

    server.resource["^/comments/new$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        comments_new(response, request, dbCon);
    };

    server.resource["^/comments/edit$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        comments_edit(response, request, dbCon);
    };
    
    server.resource["^/comments/delete$"]["POST"]=[](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
        comments_delete(response, request, dbCon);
    };


    //Default file: 404.html
    //Requesting invalid destination
    
    server.default_resource["GET"]=[&server](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
            string content="Could not find path "+request->path;
            *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
    };

    server.default_resource["POST"]=[&server](shared_ptr<HttpsServer::Response> response, shared_ptr<HttpsServer::Request> request) {
            string content="Could not find path "+request->path;
            *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
    };

    thread server_thread([&server](){
        //Start server
        server.start();
    });
    
    //Wait for server to start so that the client can connect
    this_thread::sleep_for(chrono::seconds(1));
    
    server_thread.join();

    return 0;
}

void default_resource_send(const HttpsServer &server, const shared_ptr<HttpsServer::Response> &response,
                           const shared_ptr<ifstream> &ifs) {
    //read and send 128 KB at a time
    static vector<char> buffer(131072); // Safe when server is running on one thread
    streamsize read_length;
    if((read_length=ifs->read(&buffer[0], buffer.size()).gcount())>0) {
        response->write(&buffer[0], read_length);
        if(read_length==static_cast<streamsize>(buffer.size())) {
            server.send(response, [&server, response, ifs](const boost::system::error_code &ec) {
                if(!ec)
                    default_resource_send(server, response, ifs);
                else
                    cerr << "Connection interrupted" << endl;
            });
        }
    }
}
