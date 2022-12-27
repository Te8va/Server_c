#ifndef __HTTP_CPP_SESSION_CLASS__
#define __HTTP_CPP_SESSION_CLASS__
#include "HTTP_session.hpp"
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <iterator>
#include <sstream>
#include <fstream>
#include <map>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <ctime>
std::string generateUUID();

HTTP_session::HTTP_session() : HTTP(){
    std::string sessionID = this->getCookie("session");
    Session session;
    if(sessionID == ""){
        sessionID = session.generate_sessionID();
        this->setCookie("session",sessionID);
    }else{
        session.set_sessionID(sessionID);
    }
    // if(this->httpPost("login") != ""){
    //   http.setCookie("login", login);  

/*        http.setCookie("get", GetP);
        std::string res = db.read_one(GetP);
*/
    // }
    
    // if(this->httpGet("logout") == "true"){
    //     this->setCookie("session","");
    // }
}


HTTP_session::~HTTP_session(){}


Session::Session()
{
}

Session::~Session()
{
}
void Session::set_sessionID(std::string _sessionID){
    this->sessionID = _sessionID;
}
std::string Session::generate_sessionID(){
    std::srand(std::time(nullptr)); 
    // this->sessionID = std::to_string(std::rand());
    this->sessionID = generateUUID(); 
    return sessionID;
}

const std::string CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
std::string generateUUID(){
std::srand(std::time(nullptr)); 
std::string uuid = std::string (36,' ');
int rnd = 0;
int r = 0;

uuid[8]  = '-';
uuid[13] = '-';
uuid[18] = '-';
uuid[23] = '-';
uuid[14] = '4';
for(int i=0;i<36;i++) {
if (i != 8 && i != 13 && i != 18 && i != 14 && i != 23) {
if (rnd <= 0x02) {
rnd = 0x2000000 + (std::rand() * 0x1000000) | 0;
}
rnd >>= 4;
uuid[i] = CHARS[(i == 19) ? ((rnd & 0xf) & 0x3) | 0x8 : rnd & 0xf];
}
}
return uuid;
}
#endif