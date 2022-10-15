#ifndef INET_CLASS
#define INET_CLASS
#pragma once
#include <string>
#include <map>
#include <Windows.h>
namespace Network{
	enum INetMethods{
		INetGet=0xf25,INetPost,INetHTTP10,INetHTTP11
	};
	std::string Connect(std::string host,std::string page,INetMethods method=INetGet,INetMethods http=INetHTTP10,int port=80,int timeout=15,bool alive=false);
	std::string ReturnError(std::string error);
	void GetIP(const char*,char *);
	char *GetIP(const char*);
}
#endif