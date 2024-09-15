#pragma once
#include <string>

#ifdef _WIN32
#include <WS2tcpip.h>
#include <WinSock2.h>

#endif // _WIN32


#define ADDRESS_VALIDATE 1

#ifdef _WIN32

enum class TypeAddress : int
{
	IPV4 = AF_INET,
	IPV6 = AF_INET6,
	BTH = AF_BTH
};

#endif // _WIN32




class Address
{
private:
#ifdef _WIN32
	sockaddr_in _address;
	mutable std::string ip;
#endif // _WIN32
public:
	Address(sockaddr_in address) : _address(address) {}
	Address() = default;
	Address(TypeAddress _type, std::string ip, unsigned int port);

#ifdef _WIN32
	sockaddr_in getAddress() const;
	sockaddr_in operator()() const;
	const std::string& getIpAddress() const;

	bool operator==(const Address& other) const;
	bool operator!=(const Address& other) const;
#endif // _WIN32

public:
#ifdef _WIN32
	static sockaddr_in createDefaultValueAddress();

#endif // _WIN32


};
