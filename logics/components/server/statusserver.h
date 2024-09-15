#pragma once
#include <cstdint>

enum class StatusServer : uint8_t
{
	CREATE_ROOM = 1,
	JOIN_ROOM = 2,
	NOT_LISTEN = 3,
	EXIT = 4
};