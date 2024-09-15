#pragma once
#include <string>
#include "../../include/oc/socket/components/address.h"

class User
{
public:
	std::wstring nickname;
	Address addressSocket;
public:
	User(std::wstring _nickname) : nickname(_nickname) {}
	User() = default;
};