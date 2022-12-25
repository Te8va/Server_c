#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include "HTTP.hpp"
using namespace std;
int main()
{
    HTTP http;
    http.httpSendFile("uploadFile.html");
    std::string getparam = http.rawURLDecode(string(http.httpGet("get")));
    std::string getallparam = string(http.httpGet("getall"));
    
    for(auto i = http.filesData.begin();i != http.filesData.end();i++){
        auto key = (*i).first;
        auto val = (*i).second;
        // cout << key <<endl;
        if(val.error == -1){
            http.output <<" filename:"<<val.filename<<" size:" << (val.size)<< " is bigger then MAX FILE SIZE </br>";
            unlink(val.tmp_name.c_str());
            continue;
        }
        int res = http.move_uploaded_file(val,"./usrfiles/"+val.filename);
        // int res = http.move_uploaded_file(val,"/tmp/"+val.filename);
        if(res != 0){
            http.output << strerror(errno)<<" ";
            http.output <<res<<" filename:"<<val.filename<<" size:" << (val.size) << "</br>";
            //удаление временного файла
            unlink(val.tmp_name.c_str());
        }else{
            http.output <<res<<" filename:"<<val.filename<<" size:" << (val.size) <<" OK" << "</br>";
        }
    }
    http.send();
}
