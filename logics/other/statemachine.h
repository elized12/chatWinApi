#pragma once
#include <functional>
#include <map>
#include <exception>


class StateNotExist : public std::exception
{
public:
	const char* what() const override
	{
		return "StateNotExist";
	}
};

class StateMachine
{
private:
	int _stateStatus = 0;
	std::map<int, std::function<void()>> _stateActions;
public:
	StateMachine(int startState) : _stateStatus(startState) {}
public:
	void setState(int state) throw (StateNotExist);
	void insertState(int state, std::function<void()>&& action) noexcept;
	int& getState() noexcept;
	void changeStatus(int stateStatus) throw(StateNotExist);
	void workActionForActualState();
};