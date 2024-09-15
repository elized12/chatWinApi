#pragma once
#include <future>
#include <thread>
#include <unordered_map>
#include "../../../include/socket/tcpsock.h"
#include "../../struct/msgstore.h"
#include "../../struct/listuser.h"
#include "../../other/Convector.h"
#include "../../struct/thread_safe_queue.h"
#include "../../struct/msgstore.h"

struct StatusSend
{
	bool isConnect;
	bool isSend;
};

class Sender
{
private:
	TcpSocket _socket;
	std::jthread _thread;
	bool _isStopped;
	std::unordered_map<int, std::function<void(Message& message, Sender& sender, ListUser& listUser)>> _callbackFunction;
public:
	Sender();
	~Sender();
public:
	/*		FOR SERVER		*/
	std::future<void> asyncSendMessageAll(ListUser& list, Message message);
	void sendClosedMessageAll(ListUser& list);
	StatusSend sendMessageToUser(User& user, Message& message);
	/*		FOR CLIENT		*/
	StatusSend sendMessageToServer(User& user, Address& address, std::wstring& message);
	StatusSend sendMessage(User& userSender, std::wstring& message, Address& addressRoom);
	StatusSend sendQueryToConnect(User& user, Address& address);
	StatusSend sendQueryToExit(User& user, Address& address);
	void start(thread_safe_queue<Message>& queue, MessageStore& messageStore, ListUser& list, User& user);
	void setCallback(int event, std::function<void(Message& message, Sender& sender, ListUser& listUser)>&& callbackFuction);
	void stop();
private:
	void _asyncSendMessageAll(ListUser& list, Message message);
};