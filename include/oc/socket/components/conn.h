#pragma once
#include "address.h"
#include <vector>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>

#endif // _WIN32


class Connection
{
private:
	mutable Address _address;
	bool _isClosed;

#ifdef _WIN32
	SOCKET _socket;
#endif // _WIN32

public:

#ifdef _WIN32
	Connection(Address&& address, SOCKET&& socket) : _address(address), _socket(socket), _isClosed(false) {}
	Connection(Connection&& other) noexcept;
	Connection() = default;

#endif // _WIN32

public:
#ifdef _WIN32
	Connection& operator=(Connection&& other) noexcept;


#endif // _WIN32

public:
#ifdef _WIN32
	const SOCKET& getSocket() const noexcept;
	 const Address& getAddress() const noexcept;
	 void send(std::vector<char>& buffer, int flags = 0) throw();
	 std::vector<char> recv(int flags = 0) throw();
	 void close();
#endif // _WIN32


public:
#ifdef _WIN32
	~Connection();
#endif // _WIN32



};
