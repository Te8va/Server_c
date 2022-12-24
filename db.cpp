#ifndef __DB__CPP__CLASS__
#define __DB__CPP__CLASS__
#include "db.hpp"
#include <errno.h>
#include <fstream>
#include <sstream>

// читать 1 запись из бд
std::string FILEDB::read_one(std::string key) {
    std::ifstream ifile(this->dbfilename);
    std::string ikey, ival, nstr;
    while (std::getline(ifile, ikey, '=')) {
        while (ikey[0] == '\n'){
            ikey.erase(ikey.begin());
        }

        std::getline(ifile, ival);
        if (std::string(key) == ikey) {
            ifile.close();
            return std::string(ival);
        }
    }
    ifile.close();
    return "";
}
 // добавить запись в бд 
int FILEDB::write(std::string key, std::string value) {
    std::ofstream ofile;
    ofile.open(this->dbfilename, std::ios::app);
    if (!ofile) {
        return -1;
    }
    if (this->read_one(key) != std::string()) {
        ofile.close();
        return -1;
    }
    ofile << std::endl << std::string(key) << "=" << std::string(value);
    if (ofile.bad()) {
        ofile.close();
        return -1;
    }
    ofile.close();

    return 0;
}

// прочитать все записи из бд
std::map<std::string, std::string>* FILEDB::read_all() {
    std::map<std::string, std::string> *res = new std::map<std::string, std::string>;
    std::ifstream ifile(this->dbfilename);
    std::string ikey, ival;
    std::string str;
    

    while (std::getline(ifile, str, '\n')) {
        // (*res)["ikey"] = str.c_str()[0]+32; 
        if(str != "\r") { // linux: \n ; win32: \r\n
            std::stringstream buffer(str);
            std::getline(buffer, ikey, '=');
            std::getline(buffer, ival, '\r');
            (*res)[ikey] = ival;
        } else {
            continue;
        }        
    }
    ifile.close();
    return res;
}

// удалить запись из бд 
int FILEDB::erase(std::string key) {
    std::fstream ofile;
    // получение всех keys из файла
    std::map<std::string, std::string>* res = this->read_all();
    ofile.open(this->dbfilename, std::ios::out);
    if (!ofile) {
        return -1;
    }
    // Запись всех keys в файл кроме "key"
    for (auto [ ikey, ival ] : *res) {
        if (key != ikey){
            #ifdef DEBUG
            std::nummnum << ikey << ": " << ival << "<br>"; //DEBUG
            #endif
            ofile << std::string(ikey) << "=" << std::string(ival) << std::endl;
        }
    }
    delete res;
    if (ofile.bad()) {
        return -1;
    }
    ofile.close();

    return 0;
}

FILEDB::FILEDB() {
    this->dbfilename = "db.txt";
}

FILEDB::~FILEDB() {
    return;
}

#endif
