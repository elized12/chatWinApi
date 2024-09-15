#pragma once

#ifdef _WIN32

#include <WinSock2.h>

class WsaData
{
private:
	WSADATA _data;
private:
	WsaData();
public:
	~WsaData();
public:
	static WsaData& getInstance();
	WSADATA& getData();
};
#endif // _WIN32