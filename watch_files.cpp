#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <filesystem>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <vector>
#include "HTTP.hpp"
using namespace std;
namespace fs = std::filesystem;

int SendFile(HTTP & http,std::string name);

int main()
{
    HTTP http;
    std::map<std::string,int> pathVector;
    fs::current_path("./usrfiles");
    std::string path = ".";
    http.output << "<a href=\"./\">Наверх</a>";

    for (const auto & entry : fs::directory_iterator(path)){
        pathVector[entry.path()] = 1;
    }
    if(http.httpPost("delete") != ""){
            fs::path tpath = fs::path(http.httpPost("delete"));
            if (pathVector[http.rawURLDecode(tpath)] == 1){
                http.output << "</br>File "<< http.rawURLDecode(tpath) <<"delete</br>";
                remove(http.rawURLDecode(tpath).c_str());
            }
            else{
                http.output << "</br>File not found</br>";
            }
    }
    for (const auto & entry : fs::directory_iterator(path)){
        http.output << "<h1 align=\"center\">----------------------------------------------</h1>";
        http.output << "<h3>" << entry.path() << "</h3>";
        http.output << "<form enctype=\"application/x-www-form-urlencoded\" method=\"post\"><input name=\"delete\" value="; 
        http.output << entry.path() << " hidden=\"hidden\"><input type=\"submit\" value=\"удалить\"></form>";
        pathVector[entry.path()] = 1;
        SendFile(http,entry.path());
    }
    http.output << "<h1 align=\"center\">----------------------------------------------</h1>";
    
    http.send();
}

int SendFile(HTTP & http,std::string name){
    std::ifstream ifile;
    ifile.open(name, std::ios::in);
    if (!ifile){
        return -1;
    }
    std::string str;
    while (std::getline(ifile, str, '\n')) {
        http.output << str << "</br>";
    }
    return 0;
}