#pragma once
#include <vector>
#include "components/address.h"
#include "components/conn.h"
#include "exception/except.h"

#define AUTO_MAX_CONNECT 0

namespace socket_interface
{
	class ISocket
	{
	public:
		virtual void send(Connection& conn, std::vector<char>& buffer) = 0;
		virtual void send(Connection& conn, char* buffer, int sizeBuffer) = 0;
		virtual std::vector<char> recv(Connection& conn) = 0;
		virtual Address getAddress() const = 0;
		virtual Connection accept() throw(AcceptException) = 0;
		virtual Connection connect(const Address& socketAddress) throw(ConnectionException) = 0;
		virtual void listen(int maxConnect = AUTO_MAX_CONNECT) throw(ListenException) = 0;
		virtual void bind(const Address& addressBind) throw(BindException) = 0;
		virtual void close() throw(CloseException) = 0;
		virtual void sendTo(std::vector<char>& buffer, Address& recipient, int flags = 0) = 0;
		virtual std::vector<char> recvFrom(Address& sender, int flags = 0) = 0;
	};

}
