#include "header.h"
#include "HTTP.hpp"
#include "db.hpp"
//using namespace std;

void writeStatus(HTTP &http, int param) {
    if (param == 0){
            http.output << "201 OK";
        } else {
            http.output << "500 Server Error";
        }
}

int main()
{
    HTTP http;
    FILEDB db;
    http.httpSendFile("db.html");
    std::string GetP = http.rawURLDecode(std::string(http.httpGet("get")));
    std::string AllP = std::string(http.httpGet("getall"));
    std::string EraseP = std::string(http.httpPost("erase"));
    std::string SetP = std::string(http.httpPost("set"));
    int res = db.erase(http.rawURLDecode(EraseP));
    
    if (EraseP != ""){
        writeStatus(http, res);
    }
    if (GetP != ""){
        http.setCookie("get", GetP);
        std::string res = db.read_one(GetP);
        if (res != ""){
            std::string outStr_New_01 = "<table><tr><th>Ключ</th><th>Значение</th><th>Удаление</th></tr>";
            std::string outStr_New_02 = "<td>" + http.replaceEscapeSymbols(GetP) + "</td><td>" + http.replaceEscapeSymbols(res) + "</td>";
            std::string outStr_New_03 = "<td><form enctype=\"application/x-www-form-urlencoded\" method=\"post\"><input name=\"erase\" value=\""; 
            std::string outStr_New_04 = GetP + "\" hidden=\"hidden\"><input type=\"submit\" value=\"Удалить\"></form></td></tr></table>";
            http.output << outStr_New_01 + outStr_New_02 + outStr_New_03 + outStr_New_04;
        }
    }
    if (AllP != ""){
        std::map<std::string, std::string> *res = db.read_all();
        int i = 0;
        std::string outStr_New_01 = "<table><tr><th>Ключ</th><th>Значение</th><th>Удаление</th></tr>";
        http.output << outStr_New_01;
        for (auto [key, val] : *res){
            std::string outStr_New_02 = "<tr><td>" + http.replaceEscapeSymbols(key) + "</td><td>" + http.replaceEscapeSymbols(val) + "</td>";
            http.output << outStr_New_02;
            // http.output << "\"" << http.replaceEscapeSymbols(key) << "\": \"" << val << "\"";
            // http.output << "\"" << http.replaceEscapeSymbols(key) << "\": \"" << http.replaceEscapeSymbols(val) << "\"";
            i++;
            http.output << "<td><form enctype=\"application/x-www-form-urlencoded\" method=\"post\"><input id=\"keyinpgetall\" name=\"erase\" value=\"";
            http.output << key;
            http.output << "\" hidden=\"hidden\"><input type=\"submit\" value=\"Удалить\"></form></td></tr>";
        }
        http.output << "</table>";
    }
    if (SetP != ""){
        http.setCookie("get", http.rawURLDecode(SetP));
        int res = db.write(http.rawURLDecode(SetP), http.rawURLDecode(std::string(http.httpPost("value"))));
        writeStatus(http, res);
    }
    http.send();
}
