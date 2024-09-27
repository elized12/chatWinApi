#pragma once
#include <vector>
#include <memory>
#include "../models/user.h"

class ListUser
{
private:
	std::vector<User> _list;
public:
	ListUser() = default;
public:
	void addUser(User user);
	bool eraseUser(std::wstring nickname);
	bool userExist(User& user);
	std::vector<User> getConnectUsers();
	std::unique_ptr<User> getUserByNickname(std::wstring nick);
	void clear();
};