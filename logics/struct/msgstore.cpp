#include "msgstore.h"

void MessageStore::addMessage(Message msg)
{
	arr.push(msg);
}

void MessageStore::clearMessage()
{
	arr.clear();
}

std::vector<Message> MessageStore::getData() const
{
	return arr.getData();
}
