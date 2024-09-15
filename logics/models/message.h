#pragma once
#include <string>
#include <chrono>
#include <ctime>
#include "user.h"

enum class Command : uint8_t
{
	NONE = 0,
	INIT_USER = 1,
	SEND_MESSAGE_TO_SERVER = 2,
	SEND_ALL_USERS = 3,
	GET_ALL_MESSAGE = 4,
	EXIT_USER = 5,
	CLOSE_ROOM = 6,
	RECV_MESSAGE = 7,
	INIT_USER_NOTIFICATION = 8,
	EXIT_USER_NOTIFICATION = 9,
	ROOM_IS_NOT_CREATE_NOTIFICATION = 10
};

class Message
{
private:
	std::wstring _date;
public:
	Command type;
	std::wstring message;
	User user;
	Address addressServer;
public:
	Message(User _user , std::wstring _message, Command _type): user(_user), message(_message), type(_type) {}
	Message() = default;
public:
	void setDate();
	void setDate(std::wstring date);
	std::wstring getDate();

};

std::wstring charToWchar(const char* str, int byteLenth);
std::string wcharToChar(const std::wstring& wstr);