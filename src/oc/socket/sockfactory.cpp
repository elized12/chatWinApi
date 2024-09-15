#include "../../../include/oc/socket/sockfactory.h"

std::unique_ptr<socket_interface::ISocket> SocketFactory::createSocket(Socket_Type type)
{
#ifdef _WIN32

	
	std::unique_ptr<socket_interface::ISocket> ptr = std::make_unique<socket_interface::WinSocket>(type);
	return ptr;
#endif // _WIN32

}
