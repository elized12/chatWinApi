#pragma once
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <unordered_map>
#include "statusserver.h"
#include "../../../include/socket/tcpsock.h"
#include "../../models/message.h"
#include "../../other/Convector.h"
#include "../../struct/thread_safe_queue.h"
#include "../../struct/msgstore.h"
#include "../../struct/listuser.h"

struct StatusCreateServer
{
	bool isBind;
	bool isListen;
};

class Server
{
private:
	StatusServer _status;
	TcpSocket _socket;
	Address _address;
	std::jthread _thread;
	bool _isWorkThread;
	std::mutex lockForStatus;
	std::mutex _lockForException;
	std::exception_ptr _exception;
	std::atomic<bool> _isNewException;
	bool _isClosedSocket;
	std::unordered_map<int, std::function<void(Connection& conn, thread_safe_queue<Message>& queueMessageTask, Message& message, ListUser& list)>> _callbackFunction;
public:
	Server(Address addressServer, int maxClients = AUTO_MAX_CONNECT);
	Server(const Server&) = delete;
	~Server();
public:
	StatusCreateServer start(thread_safe_queue<Message>& queue, ListUser& listUser);
	StatusServer getStatus();
	std::exception_ptr tryGetException();
	void setCallback(int event, std::function<void(Connection& conn, thread_safe_queue<Message>& queueMessageTask, Message& message, ListUser& list)>&& callbackFunction);
	void SetStatus(StatusServer status);
	void stop();
private:
	void proccessThread(std::stop_token stop_token, thread_safe_queue<Message>& queue, ListUser& listUser);
	bool tryAccept(Connection& conn);
};