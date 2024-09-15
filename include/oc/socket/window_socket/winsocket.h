#pragma once
#include <WinSock2.h>
#include <vector>
#include "../components/socket_type.h"
#include "../inface_socket.h"
#include "../../win/wsainit.h"

#define AUTO_MAX_LISTEN 0

namespace socket_interface
{

	class WinSocket : public ISocket
	{
	private:
		SOCKET _socket;
		Socket_Type _type;
		Address _address;
		bool _isSetAddress;
		bool _isClosed;
		bool _isWasConnected;
	public:
		WinSocket(Socket_Type socketType);
		virtual ~WinSocket();
	public:
		WinSocket(const WinSocket&) = delete;
		WinSocket& operator=(const WinSocket&) = delete;
	public:
		void send(Connection& conn, std::vector<char>& buffer) override;
		void send(Connection& conn, char* buffer, int sizeBuffer) override;
		std::vector<char> recv(Connection& conn) override;
		void bind(const Address& address) throw(BindException) override;
		void listen(int maxConnect = AUTO_MAX_CONNECT) throw(ListenException) override;
		Connection accept() throw(AcceptException) override;
		Connection connect(const Address& addres) throw(ConnectionException) override;
		Address getAddress() const override;
		void close() throw(CloseException)override;
		void sendTo(std::vector<char>& buffer, Address& recipient, int flags = 0) override;
		std::vector<char> recvFrom(Address& sender, int flags = 0) override;
	private:
		SOCKET getSoket(Socket_Type socketType);
	};



}