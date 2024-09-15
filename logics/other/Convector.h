#pragma once
#include <vector>
#include <algorithm>
#include "../models/message.h"

class Convector
{
public:
	Convector() = delete;
	Convector(const Convector&) = delete;
	Convector(Convector&&) = delete;
public:
	static Message bytesToMessage(std::vector<char>& bytes) throw();
	static std::vector<char> messageToBytes(Message& message);
	static bool tryBytesToMessage(std::vector<char>& bytes, Message& message);

};