#include "../../include/socket/tcpsock.h"


void TcpSocket::listen(int maxConnect)
{
	if (_isClosed)
		throw CloseException();

	if (_isListen)
		throw ListenException();

	if (!_isBind)
		throw BindException();

	_socket.listen(maxConnect); //exceptions are possible
	
	_isListen = true;
}

void TcpSocket::bind(const Address& addressBind)
{
	if (_isBind)
		throw BindException();

	_socket.bind(addressBind); //exceptions are possible
	
	_isBind = true;
}

Connection TcpSocket::accept()
{

	if (_isClosed)
		throw CloseException();

	if (!_isBind)
		throw BindException();

	if (!_isListen)
		throw ListenException();

	return _socket.accept(); 
}

Connection TcpSocket::connect(const Address& addressSocket)
{
	if (_isClosed)
		throw CloseException();

	if (_isListen)
		throw ListenException();

	return _socket.connect(addressSocket);
}

void TcpSocket::close()
{
	if (_isClosed)
		throw CloseException();

	_socket.close();
}

std::future<Connection> TcpSocket::asyncAccept()
{
	std::future<Connection> conn = std::async(std::launch::async, [this]() {return _socket.accept(); });
	return conn;
}




