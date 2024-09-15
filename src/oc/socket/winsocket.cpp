#include "../../../include/oc/socket/window_socket/winsocket.h"

socket_interface::WinSocket::WinSocket(Socket_Type socketType) : _isSetAddress(false), _isClosed(false), _isWasConnected(false)
{
	_type = socketType;

	_socket = INVALID_SOCKET;

	_address = Address(Address::createDefaultValueAddress());

	_socket = getSoket(_type);

}

socket_interface::WinSocket::~WinSocket()
{
	if (!_isClosed)
		close();
}

void socket_interface::WinSocket::send(Connection& conn, std::vector<char>& buffer)
{
	using ::send;

	send(conn.getSocket(), buffer.data(), (int)buffer.size(), 0);
}

void socket_interface::WinSocket::send(Connection& conn, char* buffer, int bufferSize)
{
	using ::send;

	send(conn.getSocket(), buffer, bufferSize, 0);
}

void socket_interface::WinSocket::bind(const Address& address) throw(BindException())
{
	using ::bind;
	int errorCode = bind(_socket, (SOCKADDR*)&address, sizeof(address));

	if (errorCode == SOCKET_ERROR)
		throw BindException();

	_address = address;

}

std::vector<char> socket_interface::WinSocket::recv(Connection& conn)
{
	using ::recv;

	const int bufferSize = 4096;

	std::vector<char> bufferTemp(bufferSize);

	std::vector<char> data;

	while (true)
	{
		int bytesRead = recv(const_cast<SOCKET&>(conn.getSocket()), bufferTemp.data(), bufferSize, NULL);

		if (bytesRead == SOCKET_ERROR)
		{
			throw "error Send Socket";
			break;
		}
		else if (bytesRead == 0)
		{
			break;
		}
		else
		{
			data.insert(data.end(), bufferTemp.begin(), bufferTemp.begin() + bytesRead);
		}
	}

	return data;


}

void socket_interface::WinSocket::listen(int maxListen)
{
	using ::listen;

	int errorCode;

	if (maxListen == AUTO_MAX_CONNECT)
		errorCode = listen(_socket, SOMAXCONN);
	else
		errorCode = listen(_socket, maxListen);

	if (errorCode == SOCKET_ERROR)
		throw ListenException();
}

Connection socket_interface::WinSocket::accept()
{
	using ::accept;

	sockaddr_in address;

	int sizeAddress = sizeof(address);

	SOCKET socket = accept(_socket, (SOCKADDR*)&address, &sizeAddress);

	if (socket == INVALID_SOCKET)
		throw AcceptException();


	char clientIP[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(address.sin_addr), clientIP, INET_ADDRSTRLEN);
	int clientPort = ntohs(address.sin_port);

	Address addressConnection(TypeAddress::IPV4, clientIP, clientPort);

	return Connection(std::move(addressConnection), std::move(socket));
}

Connection socket_interface::WinSocket::connect(const Address& address)
{
	using ::connect;

	if (_isWasConnected)
	{
		_socket = getSoket(_type);
	}

	int result = connect(_socket, (SOCKADDR*)&address, sizeof(address));

	if (result == SOCKET_ERROR)
		throw ConnectionException();

	Address copyAddress = _address;

	SOCKET copySocket = _socket;

	_isWasConnected = true;

	return Connection(std::move(copyAddress), std::move(copySocket));
}

Address socket_interface::WinSocket::getAddress() const
{
	return _address;
}

void socket_interface::WinSocket::close()
{
	if (_isClosed)
		throw "socket is was closed";

	closesocket(_socket);
	_isClosed = true;
}

void socket_interface::WinSocket::sendTo(std::vector<char>& buffer, Address& recipient, int flags)
{
	const sockaddr_in addr = recipient();

	int errCode = sendto(_socket, buffer.data(), buffer.size(), flags, (SOCKADDR*)(&addr), sizeof(addr));

	if (errCode == SOCKET_ERROR)
		throw "error sendTo socket";
}

std::vector<char> socket_interface::WinSocket::recvFrom(Address& sender, int flags)
{
	std::vector<char> tempBuffer(4096);
	std::vector<char> buffer;

	int readBytes = 0;

	int readAllBytes = 0;

	int sizeAddress = sizeof(sender);

	while (true)
	{
		readBytes = recvfrom(_socket, tempBuffer.data(), tempBuffer.size(), flags, (SOCKADDR*)&sender, &sizeAddress);

		if (readBytes > 0)
		{
			buffer.insert(buffer.end(), tempBuffer.begin(), tempBuffer.begin() + readBytes);

			readAllBytes += readBytes;
		}
		else if (readBytes == 0)
		{
			break;
		}
		else
		{
			throw "error recvTo socket";
		}
	}

	return buffer;
}

SOCKET socket_interface::WinSocket::getSoket(Socket_Type socketType)
{

	SOCKET temp;

	switch (_type)
	{
	case Socket_Type::SOCKET_TCP:
		temp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		break;
	case Socket_Type::SOCKET_UDP:
		temp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		break;
	default:
		break;
	}

	return temp;
}

