#ifndef __DB__HPP__CLASS__
#define __DB__HPP__CLASS__
#include "header.h"
#include <map>
/*
добавление записи, удаление, просмотр списка, просмотр одной записи.
Записи в базе данных хранятся построчно в файле, а отдельные поля записи разделены символами-разделителями.
*/
class FILEDB{
    public:
    FILEDB();
    FILEDB(std::string filename);
    ~FILEDB();

    // добавить запись в бд 
    int write(std::string key, std::string value);
    // читать 1 запись из бд
    std::string read_one(std::string key);
    // прочитать все записи из бд
    std::map<std::string, std::string>* read_all();
    // удалить запись из бд 
    int erase(std::string key);
    private:
    std::string dbfilename;
};

#endif  // __DB__HPP__CLASS__
