#include "../../../include/oc/socket/components/conn.h"


#ifdef _WIN32

const SOCKET& Connection::getSocket() const noexcept
{
    return _socket;
}

const Address& Connection::getAddress() const noexcept
{
    return _address;
}

void Connection::send(std::vector<char>& buffer, int flags)
{
	if (_isClosed)
		throw "not send Connection is was closed";

    using ::send;

    int errCode = send(_socket, buffer.data(), buffer.size(), flags);

	if (errCode == SOCKET_ERROR)
		throw "error send Connection";
}

std::vector<char> Connection::recv(int flags)
{
	if (_isClosed)
		throw "not recv Connection is was closed";

	using ::recv;

	const int bufferSize = 4096;

	std::vector<char> bufferTemp(bufferSize);

	std::vector<char> data;

	while (true)
	{
		int bytesRead = recv(_socket, bufferTemp.data(), bufferSize, NULL);

		if (bytesRead == SOCKET_ERROR)
		{
			throw "error recv Connection";
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


Connection::Connection(Connection&& other) noexcept
{
    _socket = std::move(other._socket);

    other._socket = INVALID_SOCKET;

    _address = other._address;

	_isClosed = other._isClosed;
}

Connection& Connection::operator=(Connection&& other) noexcept
{
	_socket = std::move(other._socket);
	other._socket = INVALID_SOCKET;
	_address = other._address;

	return *this;
}

void Connection::close()
{
	if (_isClosed)
		throw "connection is was closed";

	closesocket(_socket);

	_isClosed = true;
}

Connection::~Connection()
{
	if (!_isClosed)
		closesocket(_socket);
}

#endif //_WIN32


