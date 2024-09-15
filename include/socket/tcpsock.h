#pragma once
#include <exception>
#include <future>
#include "../oc/socket/socket.h"


class TcpSocket
{
private:
	Socket _socket;
	bool _isBind;
	bool _isListen;
	bool _isConnect;
	bool _isClosed;
public:
	TcpSocket() : _socket(Socket(Socket_Type::SOCKET_TCP)), _isBind(false), _isListen(false), _isConnect(false),_isClosed(false) {}
public:
	void listen(int maxConnect = AUTO_MAX_CONNECT) throw(ListenException);
	void bind(const Address& addressBind) throw(BindException);
	Connection accept() throw (AcceptException);
	Connection connect(const Address& addressSocket) throw(ConnectionException);
	void close();
	std::future<Connection> asyncAccept();
};