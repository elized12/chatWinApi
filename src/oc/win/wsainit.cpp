#include "../../../include/oc/win/wsainit.h"

#ifdef _WIN32

WsaData::~WsaData()
{
	WSACleanup();
}

WsaData& WsaData::getInstance()
{
	static WsaData instance;

	return instance;
}

WSADATA& WsaData::getData()
{
	return _data;
}

WsaData::WsaData()
{
	if (WSAStartup(MAKEWORD(2, 2), &_data) == SOCKET_ERROR)
		throw "error init WSA";
}

WsaData wsaData = WsaData::getInstance();

#endif // _WIN32

