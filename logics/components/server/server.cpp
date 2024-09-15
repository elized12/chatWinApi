#include "server.h"

void Server::proccessThread(std::stop_token stop_token, thread_safe_queue<Message>& queue, ListUser& listUser)
{
	while (!stop_token.stop_requested())
	{
		try
		{
			Connection conn = _socket.accept();

			std::vector<char> buffer = conn.recv();

			Message message;

			if (Convector::tryBytesToMessage(buffer, message))
			{
				StatusServer tempStatus = getStatus();

				if (_callbackFunction.find(static_cast<int>(tempStatus)) != _callbackFunction.end())
				{
					_callbackFunction[static_cast<int>(tempStatus)](std::ref(conn), queue, message, listUser);
				}
			}
		}
		catch (...)
		{

			_isNewException.store(true);

			std::unique_lock<std::mutex> lock(_lockForException);

			_exception = std::current_exception();

			lock.unlock();

		}
	}
}

bool Server::tryAccept(Connection& conn)
{
	bool isAcceptSuccess = false;
	try
	{
		conn = _socket.accept();
		isAcceptSuccess = true;
	}
	catch (...)
	{
		isAcceptSuccess = false;
	}

	return isAcceptSuccess;

}

Server::~Server()
{
	stop();
}

StatusCreateServer Server::start(thread_safe_queue<Message>& queue, ListUser& listUser)
{
	StatusCreateServer status;

	status.isBind = false;
	status.isListen = false;

	try
	{
		_socket.bind(_address);
		status.isBind = true;

		_socket.listen();
		status.isListen = true;
	}
	catch (const BindException& ex)
	{
		status.isBind = false;
	}
	catch (const ListenException& ex)
	{
		status.isListen = false;
	}

	if (status.isBind && status.isListen)
	{
		_thread = std::jthread(std::bind(&Server::proccessThread, this, std::placeholders::_1, std::ref(queue), std::ref(listUser)));
	}

	return status;
}

void Server::SetStatus(StatusServer status)
{
	std::lock_guard<std::mutex> lock(lockForStatus);

	_status = status;
}

void Server::stop()
{
	if (!_isClosedSocket)
	{
		if (_thread.joinable())
		{
			TerminateThread(_thread.native_handle(), 0);

			_thread.join();
		}

		_isClosedSocket = true;
	}
}

StatusServer Server::getStatus()
{
	std::lock_guard<std::mutex> lock(lockForStatus);

	return _status;
}

Server::Server(Address addressServer, int maxClients) : _address(addressServer), _status(StatusServer::NOT_LISTEN), _isClosedSocket(false)
{
	_exception = nullptr;
	_isNewException.store(false);
	_isWorkThread = true;
}

std::exception_ptr Server::tryGetException()
{
	if (_isNewException)
	{
		std::lock_guard<std::mutex> lock(_lockForException);
		_isNewException.store(false);

		auto copyPtr = _exception;

		_exception = nullptr;

		return copyPtr;
	}
	return nullptr;
}

void Server::setCallback(int event, std::function<void(Connection& conn, thread_safe_queue<Message>& queueMessageTask, Message& message, ListUser& list)>&& callbackFunction)
{
	_callbackFunction[event] = std::move(callbackFunction);
}
