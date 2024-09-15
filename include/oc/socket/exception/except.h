#pragma once
#include <exception>
#include <string>

class BindException : public std::exception
{
private:
	std::string _message;
public:
	BindException() : _message("bind error") {}
public:
	const char* what() const noexcept override
	{
		return _message.c_str();
	}
};

class ListenException : std::exception
{
private:
	std::string _message;
public:
	ListenException() : _message("listen error") {}
public:
	const char* what() const noexcept override
	{
		return _message.c_str();
	}
};

class AcceptException : std::exception
{
private:
	std::string _message;
public:
	AcceptException() : _message("Accept error") {}
public:
	const char* what() const noexcept override
	{
		return _message.c_str();
	}
};

class ConnectionException : std::exception
{
private:
	std::string _message;
public:
	ConnectionException() : _message("Connection error") {}
public:
	const char* what() const noexcept override
	{
		return _message.c_str();
	}
};

class CloseException : std::exception
{
private:
	std::string _message;
public:
	CloseException() : _message("Close error") {}
public:
	const char* what() const noexcept override
	{
		return _message.c_str();
	}
};