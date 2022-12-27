#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include "HTTP_session.hpp"
#include "db.hpp"
#include "db.cpp"
#include <cpp_redis/cpp_redis>
using namespace std;
int main()
{
    HTTP_session http;
    std::srand(std::time(nullptr));
    // http.init();
    http.httpSendFile("login.html");
    // FILE * fp = fopen("err.txt", "w");
    http.output<<" session2 " << std::rand() <<endl;
    try{
    cpp_redis::client rclient;
    rclient.connect();
    rclient.set("hello", "45");
    // std::stringstream buf;
    // vector<string> rics;
    // rclient.get("hello", [&buf](cpp_redis::reply& reply) mutable {
    // rclient.get("hello", [&rics](cpp_redis::reply& reply){
    rclient.get("hello", [&http](cpp_redis::reply& reply) mutable {
        // buf << reply;
        // buf << 22;
        // rics.push_back(reply.as_string());
        http.output <<" redis reply: "<< reply << std::endl;
        // http.buf <<" redis reply: "<< reply << std::endl;
    });
        // http.output <<  *(rics.begin()) << std::endl;
        http.output << "out" << http.buf.str() << std::endl;
    rclient.sync_commit();
    // http.output<<" redis "<<endl;
    // fprintf(fp,"aft\n");
    // std::string 
    // auto rep = rclient.get("hello");
    // std::future<cpp_redis::reply> rep = rclient.get("hello");
    // http.output << rep.get() ;
    }
	catch (cpp_redis::redis_error e)
	{
        // fprintf(fp,"before::%s\n","s");
		http.output << e.what() << endl;
	}


    //! also support std::future
    //! std::future<cpp_redis::reply> get_reply = client.get("hello");

    // client.sync_commit();
    //! or client.commit(); for asynchronous call

    http.send();
}