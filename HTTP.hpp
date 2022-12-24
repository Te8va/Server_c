#ifndef __HTTP_HPP_CLASS__
#define __HTTP_HPP_CLASS__
#include "header.h"
#include <map>
class HTTP
{
    public:

	std::stringstream output;

	typedef struct {
		std::string filename; 	// реальное имя файла
		std::string type;	// MIME-тип файла
		std::string tmp_name; // временное имя файла
		int error; 	// код ошибки (0, если нет)
		int size; 	// размер загружаемого файла
	} UploadedFile;

    std::map<std::string, std::string> getparams;
    std::map<std::string, std::string> postparams;
    std::map<std::string, std::string> cookie;
    std::map<std::string, std::string> outcookie;
	std::map <std::string, UploadedFile> filesData;
	protected:
	std::map<std::string, std::string> escape = {
		{"\"","\\&quot;"}, {"<","&lt;"}, {">","&gt;"}
		};
	

	public:
	HTTP();
	~HTTP();
	// Отправка headers, cookie и user set data
	void send();
	//replace e
	std::string replaceEscapeSymbols(std::string str);
	/*Послать данные в stdout stream
	вернуть 0: OK; -1: failure*/
	int httpSendFile(std::string name);
	// Получить GET параметры запроса
	std::string httpGet(std::string name);
	// Получить POST параметры запроса
	std::string httpPost(std::string name);
	std::string getCookie(std::string name);
	std::string setCookie(std::string name, std::string value);
	// Преобразование "application/x-www-form-urlencoded" to utf-8 string
	std::string rawURLDecode(std::string str);
	// Возвращание значения HTTP-заголовка “name”
	std::string getHeader(std::string name);
	// Возвращание свойства файла “name”
	UploadedFile getFile(std::string name); 
	// Загрузка файл “tmpFile” в директорию “path”
	int move_uploaded_file(UploadedFile tmpFile, std::string path); 
    
	private:
	// Преобразование char (A-F,0-9) в int
	unsigned int CharToHEX(char a);
	// Преобразование символов в int
	unsigned int CharactersToInt(char ch1, char ch2);
};
#endif
