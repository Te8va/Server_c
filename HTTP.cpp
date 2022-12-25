#ifndef __HTTP_CPP_CLASS__
#define __HTTP_CPP_CLASS__
#include "HTTP.hpp"
#include <unistd.h>
#include "header.h"
#include <cstring>
#include <iterator>
#include <fstream>
#include <map>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#define MAX_FILESIZE 20480 
// удаление первого пробела
std::string leftTrim(std::string * str)
{
  if (*((*str).begin()) == ' '){
  (*str).erase((*str).begin());
  }
  if ((*((*str).end()-1) == '\n') or (*((*str).end()-1) == '\r')){
  (*str).erase((*str).end()-1);
  }
  return *str;
}
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}
// защита от XSS
std::string HTTP::replaceEscapeSymbols(std::string str)
{
    // for()
    for (auto [key, val] : this->escape){
        str = ReplaceAll(str, key, val);
    }
    return str;
}
std::string HTTP::httpGet(std::string name){
    return this->getparams[name];
}
std::string HTTP::httpPost(std::string name){
    return this->postparams[name];
}
std::string HTTP::getCookie(std::string name){
    return this->cookie[name];
}
std::string HTTP::setCookie(std::string name, std::string value){
    this->outcookie[name] = value;
    return value;
}
void HTTP::send(){
    // отправка headers
    fprintf(stdout, "Content-Type: text/html; charset=utf-8\n");
    // отправка cookie
    for (const auto& [key, value] : this->outcookie){
        fprintf(stdout, "Set-Cookie:  %s=%s; ", key.c_str(), value.c_str());
    }
    // fprintf(stdout, "\r\n\r\n");
    fprintf(stdout, "\n\n");
    fprintf(stdout, "%s", this->output.str().c_str());
}

HTTP::HTTP(){
    // FILE * fp = fopen("err.txt", "w");
    std::stringstream getstrstm(getenv("QUERY_STRING"));
    auto key = std::string{};
    auto val = std::string{};

    // парсинг GET 
    while (std::getline(getstrstm, key, '=' )) {
        std::getline(getstrstm, val, '&');
        this->getparams[key] = val;
    }
    // парсинг cookie
    if (getenv("HTTP_COOKIE") != nullptr){
        std::stringstream cookiestrstm(getenv("HTTP_COOKIE"));
        while (std::getline(cookiestrstm, key, '=')) {
            std::getline(cookiestrstm, val, ';');
            key = leftTrim(&key);
            key = replaceEscapeSymbols(key);
            val = replaceEscapeSymbols(val);
            this->cookie[key] = val;
        }
    }
    if (getenv("CONTENT_TYPE") != nullptr){
        UploadedFile upf;
        std::string boundary;
        std::stringstream strstm(getenv("CONTENT_TYPE"));
        std::getline(strstm, val, ';');  // Get Content-Type
        if (val == "multipart/form-data"){
            std::getline(strstm, val, '=');  // skip " boundary="
            std::getline(strstm, boundary);  // get boundary
            int flag = 100000;
            int tmpfd = -1;
            std::ofstream ofile;
            std::string name;
            std::string filename;
            /*
                /////POST /send-message.html HTTP/1.1
env             Host: webmail.example.com
                User-Agent: BrowserForDummies/4.67b
                Content-Type: multipart/form-data; boundary=Asrf456BGe4h 
                Content-Length: (суммарный объём, включая дочерние заголовки)
                Connection: keep-alive
                Keep-Alive: 300
                
stdin           --Asrf456BGe4h
                Content-Disposition: form-data; name="AttachedFile1"; filename="horror.txt"
                Content-Type: text/plain

                brutal-vasya@example.com
                primer
                --Asrf456BGe4h
                Content-Disposition: form-data; name="AttachedFile2"; filename="err.txt"
                Content-Type: text/plain

                error
                --Asrf456BGe4h--
                */
            while (flag)
            {
                std::string buffer;
                std::string ContentType;
                std::getline(std::cin, buffer); 
                if(std::cin.eof()){
                    return;
                }
                long long int fs = 0;
                // Если строка - это boundary
                if (leftTrim(&buffer) == ("--" + boundary)){
                    std::getline(std::cin, buffer, '\"');  // SKIP Content-Disposition: form-data; name="
                    std::getline(std::cin, name, '\"');  // get name
                    std::getline(std::cin, buffer, '\"');  // SKIP "; filename="
                    std::getline(std::cin, filename, '\"');  // get filename
                    std::getline(std::cin, buffer, '\n');  // skip '"\n'
                    std::getline(std::cin, buffer, ' ');  // skip "Content-Type: "
                    std::getline(std::cin, ContentType, '\n');  // get content type
                    std::getline(std::cin, buffer, '\n');  // skip empty line

                    filesData[name].mimetype = ContentType;
                    filesData[name].filename = filename;
                    filesData[name].size = 0;
                    filesData[name].error = 0;
                    // закрытие файла, если он открыт
                    if (ofile.is_open()){
                        ofile.close();
                    }
                    
                    // Создание временного файла
                    char tmpFile[30];
                    tmpnam(tmpFile);
                    
                    ofile.open(tmpFile);
                    filesData[name].tmp_name = tmpFile;
                }else{
                    if (leftTrim(&buffer) == ("--" + boundary + "--")){
                        break;
                    }
                    
                    ofile << buffer <<std::endl;
                    filesData[name].size += buffer.size()+1;
                    if (filesData[name].size > MAX_FILESIZE){
                        filesData[name].error = -1;
                        break;
                    }
                    
                }
                flag--;
            }
        }else{
            if (val == "application/x-www-form-urlencoded"){
                    // parse POST params
                std::string postdata;
                std::getline(std::cin, postdata, static_cast<char>(0));
                std::stringstream poststrstm(postdata);
                while (std::getline(poststrstm, key, '=' )) {
                    std::getline(poststrstm, val, '&');
                    this->postparams[key] = val;
                }
            }
        }
    }
    return;
}
// Перемещение загруженного файла “tmpFile” в директорию “path”
int HTTP::move_uploaded_file(UploadedFile tmpFile, std::string path){
    std::ifstream  src(tmpFile.tmp_name, std::ios::binary);
    std::ofstream  dst(path,   std::ios::binary);
    if (!src.is_open()){
        return -1;
    }
    if (!dst.is_open()){
        return -2;
    }
    dst << src.rdbuf();
    unlink(tmpFile.tmp_name.c_str());
    return 0;
}

int HTTP::httpSendFile(std::string name){
    std::ifstream ifile;
    ifile.open(name, std::ios::in);
    if (!ifile){
        return -1;
    }
    std::string str;
    while (std::getline(ifile, str, '\n')) {
        this->output << str << std::endl;
    }
    return 0;
}

HTTP::~HTTP(){
    return;
}
// Декодирование "application/x-www-form-urlencoded" в нормальный вид
std::string HTTP::rawURLDecode(std::string str)
{
  std::string res = "";
  for (int i = 0; i < str.length (); i++)
    {
      if (str[i] != '%')
	{
	  res.append(1, str[i]);
	}
      if (str[i] == '%')
	{
	    i++;  // skip '%'
        res.append(1, static_cast<char>(CharactersToInt(str[i], str[i + 1])));
        i++;  // skip one hex (other hex will be skiped by i++ in for)
        continue;
	}
    }
  return res;
}

unsigned int HTTP::CharToHEX(char c){
    if (c >= 'A'){
	      return static_cast<int>(c - 'A' + 10);
	  }else{
	      return static_cast<int>(c - '0');
	  }
}

unsigned int HTTP::CharactersToInt(char c1, char c2){
    return ((CharToHEX(c1) << 4) + CharToHEX(c2));
}
//основные HTTP-заголовки запроса
std::string HTTP::getHeader(std::string name)
{
    return getenv(name.c_str());
}
// Возвращание сруктуры свойства файла загруженного файла (name)
UploadedFile HTTP::getFile(std::string name)
{
    return filesData[name];
}

#endif
