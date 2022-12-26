#ifndef __HTTP_HPP_SESSION_CLASS__
#define __HTTP_HPP_SESSION_CLASS__
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <map>
#include "HTTP.hpp"
class HTTP_session : public HTTP
{
	protected:
	std::string sessionID;
	public:
	std::stringstream buf;
	HTTP_session();
	~HTTP_session();
	 
};
class Session
{
protected:
	std::string sessionID;
public:
	Session();
	~Session();
	std::string get();
	std::string set();
	void set_sessionID(std::string _sessionID);
	std::string generate_sessionID();
};

#endif