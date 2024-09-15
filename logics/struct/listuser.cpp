#include "listuser.h"

void ListUser::addUser(User user)
{
	_list.push_back(user);
}

bool ListUser::eraseUser(std::wstring nickname)
{
	for (int i = 0; i < _list.size(); i++)
	{
		if (_list[i].nickname == nickname)
		{
			_list.erase(_list.begin() + i);
			return true;
		}
	}
	return false;
}

bool ListUser::userExist(User& user)
{
	for (int i = 0; i < _list.size(); i++)
	{
		if (_list[i].nickname == user.nickname)
		{
			return true;
		}
	}

	return false;
}

std::vector<User> ListUser::getConnectUsers()
{
	return _list;
}

std::unique_ptr<User> ListUser::getUserByNickname(std::wstring nick)
{
	for (int i = 0; i < _list.size(); i++)
	{
		if (nick == _list[i].nickname)
		{
			return std::make_unique<User>(_list[i]);
		}
	}

	return nullptr;
}
