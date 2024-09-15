#pragma once
#include "preemarr.h"
#include "../models/message.h"

class MessageStore
{
private:
	PreemptiveArray<Message> arr;
public:
	MessageStore(size_t _size) : arr(PreemptiveArray<Message>(_size)) {}
public:
	void addMessage(Message msg);
	void clearMessage();
	std::vector<Message> getData() const;
};