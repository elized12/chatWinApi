#pragma once
#include "sockfactory.h"
#pragma comment(lib, "ws2_32.lib")


class Socket
{
private:
	std::unique_ptr <socket_interface::ISocket> _socket;
public:
	Socket(Socket_Type type);
	virtual ~Socket();
public:
	void send(Connection& conn, std::vector<char>& buffer);
	void send(Connection& conn, char* buffer, int sizeBuffer);
	void sendTo(std::vector<char>& buffer, Address& recipient, int flags);
	std::vector<char> recvFrom(Address& sender, int flags);
	std::vector<char> recv(Connection& conn);
	Address getAddress() const;
	Connection accept() throw(AcceptException);
	Connection connect(const Address& socketAddress) throw(ConnectionException);
	void listen(int maxConnect = AUTO_MAX_CONNECT) throw(ListenException);
	void bind(const Address& addressBind) throw(BindException);
	void close() throw(CloseException);
};