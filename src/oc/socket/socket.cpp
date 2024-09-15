#include "../../../include/oc/socket/socket.h"

Socket::Socket(Socket_Type type)
{
	_socket = SocketFactory::createSocket(type);
}

Socket::~Socket()
{
	_socket->close();
}

void Socket::send(Connection& conn, std::vector<char>& buffer)
{
	_socket->send(conn, buffer);
}

void Socket::send(Connection& conn, char* buffer, int sizeBuffer)
{

	_socket->send(conn, buffer, sizeBuffer);
}

void Socket::sendTo(std::vector<char>& buffer, Address& recipient, int flags)
{
	_socket->sendTo(buffer, recipient, flags);
}

std::vector<char> Socket::recvFrom(Address& sender, int flags)
{
	return _socket->recvFrom(sender, flags);
}

std::vector<char> Socket::recv(Connection& conn)
{
	return _socket->recv(conn);
}

Address Socket::getAddress() const
{
	return _socket->getAddress();
}

Connection Socket::accept() throw(AcceptException)
{
	return _socket->accept();
}

Connection Socket::connect(const Address& socketAddress) throw(ConnectionException)
{
	return _socket->connect(socketAddress);
}

void Socket::listen(int maxConnect) throw(ListenException)
{
	_socket->listen(maxConnect);
}

void Socket::bind(const Address& addressBind) throw(BindException)
{
	_socket->bind(addressBind);
}

void Socket::close()
{
	_socket->close();
}




