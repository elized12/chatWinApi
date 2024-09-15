#include "sender.h"

Sender::Sender() : _isStopped(false)
{
}

Sender::~Sender()
{   
    stop();
}

StatusSend Sender::sendMessage(User& userSender, std::wstring& message, Address& addressRoom)
{
    
    StatusSend status = sendMessageToServer(userSender, addressRoom, message);

    return status;
}

StatusSend Sender::sendMessageToUser(User& user, Message& message)
{
    StatusSend status;

    status.isConnect = false;
    status.isSend = false;
    try
    {
        Connection conn = _socket.connect(user.addressSocket);

        status.isConnect = true;

        std::vector<char> bytes = Convector::messageToBytes(message);

        conn.send(bytes);

        status.isSend = true;
    }
    catch (...)
    {
    }

    return status;

}

StatusSend Sender::sendQueryToConnect(User& user, Address& address)
{
    Message _message(user, L"", Command::INIT_USER);

    StatusSend status;

    status.isConnect = false;
    status.isSend = false;

    try
    {
        Connection conn = _socket.connect(address);

        status.isConnect = true;

        std::vector<char> bytes = Convector::messageToBytes(_message);

        conn.send(bytes);

        status.isSend = true;
    }
    catch (...)
    {
    }

    return status;
    
}

StatusSend Sender::sendQueryToExit(User& user, Address& address)
{
    Message _message(user, L"", Command::EXIT_USER);

    StatusSend status;

    status.isConnect = false;
    status.isSend = false;

    try
    {
        Connection conn = _socket.connect(address);

        status.isConnect = true;

        std::vector<char> bytes = Convector::messageToBytes(_message);

        conn.send(bytes);

        status.isSend = true;
    }
    catch (...)
    {
    }

    return status;
}

StatusSend Sender::sendMessageToServer(User& user, Address& address, std::wstring& _message)
{
    Message message(user, _message, Command::SEND_MESSAGE_TO_SERVER);

    StatusSend status;

    status.isConnect = false;
    status.isSend = false;

    try
    {
        Connection conn = _socket.connect(address);

        status.isConnect = true;

        std::vector<char> bytes = Convector::messageToBytes(message);

        conn.send(bytes);

        status.isSend = true;
    }
    catch (...)
    {
    }


    return status;
}

void Sender::sendClosedMessageAll(ListUser& list)
{
    std::vector<User> users = list.getConnectUsers();


    for (int i = 0; i < users.size(); i++)
    {
        Message message(users[i], L"", Command::CLOSE_ROOM);

        StatusSend status = this->sendMessageToUser(users[i], message);

        if (status.isConnect == false)
            list.eraseUser(users[i].nickname);

        if (status.isSend == false)
        {
            StatusSend statusReSend = this->sendMessageToUser(users[i], message);

            if (statusReSend.isConnect == false || statusReSend.isSend == false)
            {
                list.eraseUser(users[i].nickname);
            }
        }
    }
}

void Sender::setCallback(int event, std::function<void(Message& message, Sender& sender, ListUser& listUser)>&& callbackFuction)
{
    _callbackFunction[event] = std::move(callbackFuction);
}

void Sender::start(thread_safe_queue<Message>& queueMessage, MessageStore& messageStore, ListUser& list, User& user)
{
    _thread = std::jthread([&queueMessage, &messageStore, &list, &user, this](std::stop_token stopToken)
        {
            while (!stopToken.stop_requested())
            {
                auto messagePtr = queueMessage.wait_pop();

                if (_callbackFunction.find(static_cast<int>(messagePtr->type)) != _callbackFunction.end())
                {
                    _callbackFunction[static_cast<int>(messagePtr->type)](*messagePtr, *this, list);
                }
            }
        }
    );
}

void Sender::stop()
{
    if (!_isStopped)
    {
        if (_thread.joinable())
        {
            TerminateThread(_thread.native_handle(), 0);
            _thread.join();
        }
        _isStopped = true;
    }

}

std::future<void> Sender::asyncSendMessageAll(ListUser& list, Message message)
{
    std::future<void> sendMessage = std::async(&Sender::_asyncSendMessageAll, this, std::ref(list), message);

    return sendMessage;
}

void Sender::_asyncSendMessageAll(ListUser& list, Message message)
{
    std::vector<User> users = list.getConnectUsers();

    for (int i = 0; i < users.size(); i++)
    {
        StatusSend status = this->sendMessageToUser(users[i], message);

        if (status.isConnect == false)
            list.eraseUser(users[i].nickname);

        if (status.isSend == false)
        {
            StatusSend statusReSend = this->sendMessageToUser(users[i], message);

            if (statusReSend.isConnect == false || statusReSend.isSend == false)
            {
                list.eraseUser(users[i].nickname);
            }
        }
    }
}

