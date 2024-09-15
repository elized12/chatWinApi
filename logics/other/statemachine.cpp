#include "statemachine.h"

void StateMachine::setState(int state)
{
	if (_stateActions.find(state) != _stateActions.end())
	{
		_stateStatus = state;
		_stateActions[state]();
	}
	else
	{
		throw StateNotExist();
	}
}

void StateMachine::insertState(int state, std::function<void()>&& action) noexcept
{
	_stateActions[state] = std::move(action);
}

int& StateMachine::getState() noexcept
{
	return _stateStatus;
}

void StateMachine::changeStatus(int stateStatus) throw(StateNotExist)
{
	if (_stateActions.find(stateStatus) != _stateActions.end())
	{
		_stateStatus = stateStatus;
	}
	else
	{
		throw StateNotExist();
	}
}

void StateMachine::workActionForActualState()
{
	_stateActions[_stateStatus]();
}
