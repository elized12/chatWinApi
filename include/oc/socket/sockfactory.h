#pragma once
#include "window_socket/winsocket.h"
#include "inface_socket.h"
#include <memory>


class SocketFactory
{
public:
	SocketFactory() = delete;
	SocketFactory(const SocketFactory&) = delete;
public:
	static std::unique_ptr<socket_interface::ISocket> createSocket(Socket_Type type);
};