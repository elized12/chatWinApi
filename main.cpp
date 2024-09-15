#include <iostream>
#include <string>
#include <future>
#include <WS2tcpip.h>
#include <Windows.h>
#include <conio.h>
#include <tchar.h>
#include <CommCtrl.h>
#include "include/socket/tcpsock.h"
#include "logics/other/Convector.h"
#include "logics/struct/thread_safe_queue.h"
#include "logics/components/server/server.h"
#include "logics/components/sender/sender.h"
#include "logics/other/statemachine.h"

#pragma warning(push)
#pragma warning(disable: 4996)

#ifdef _UNICODE
	#define tstring	std::wstring
#else
	#define tstring std::string
#endif

#ifdef _UNICODE
#define tchar wchar_t
#else
#define tchar char
#endif

//DEFINE USERS
#define WM_CHANGE_STATE (WM_USER + 1)
#define ID_CHANGE_NICKNAME (WM_USER  + 2)
#define lineFeed L"\r\n"
#define IDB_BUTTON_SEND 1
#define IBD_BUTTON_CONNECT 2
#define IBD_BUTTON_CREATE 3
#define IBD_BUTTON_DISCONNECT 4
#define IBD_EDIT_ADDRESS_ROOM 5

enum State : int
{
	NOT_ACTIVE = 0,
	CREATE_ACTIVE = 1,
	CONNECT_ACTIVE = 2,
	DISCONNECT = 3
};

//GLOBAL FUNCTIONS
LRESULT CALLBACK editAddressProc(HWND hWnd, UINT  msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefDara);
LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
tstring getStringFromComponent(HWND hWnd);
std::wstring validateMessage(std::wstring& message, HWND window);
void viewMessage(Message& message, HWND& windowView, bool isSetDate = false);
void initSender(Sender& sender);
bool connectWithTimeout(Address& address, int timeoutSeconds, User& user);
void sendNotification(Message& message, thread_safe_queue<Message>& queue, Command changeType = Command::NONE);
void clearAndSetStringComponent(HWND& component, const tstring& s);
WNDCLASS initWindowClass(HINSTANCE& hInstance, LRESULT(CALLBACK* func)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam));
HWND createWindow(tstring& className,tstring& windowName,HINSTANCE& hInstance, int modeShow) throw (std::exception);
void initServer(Server& server, HWND mainWindow, HWND* editViewMessage, Address& addressServer, int* state);
INITCOMMONCONTROLSEX initCommoonContEx();
void initStateMachine(StateMachine& machine, HWND mainWindow, HWND* editMessageView, HWND* editPostMessage, 
	HWND* buttonSend, HWND* buttonDisconnect, HWND* buttonConnect, HWND* buttonCreate, HWND* editAddressRoom);
void changeState(StateMachine& machine, int State, HWND mainWindow);
void clearMessage(tstring& message);
std::wstring generateNickname();
void initRandom();

//GLOBAL VARS
tstring className = _T("ChatApplication");
tstring windowName = _T("Chat by Elized");

thread_safe_queue<Message> queueMessage;
MessageStore messageStore(30);
ListUser connectionUsers;
std::wstring bufferMessage;
User user(L"");
Address serverAddres(TypeAddress::IPV4, std::string("127.0.0.1"), 12345);
Address connectRoom;
Server server(serverAddres);
Sender sender;
StateMachine stateMachine(State::NOT_ACTIVE);

int widthEditMessageView = 270;
std::once_flag initRand;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstanceOld, LPSTR cmd, int modeShow)
{
	INITCOMMONCONTROLSEX icex = initCommoonContEx();
	WNDCLASS wndClass = initWindowClass(hInstance, wndProc);
	HWND window = createWindow(className, windowName, hInstance, modeShow);
	user.nickname = generateNickname();

	initSender(sender);

	MSG msg;

	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;

}

LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND editMessageComponent;
	static HWND editPostMessageComponent;
	static HWND editAddressRoom;
	static HWND buttonSendMessage;
	static HWND buttonConnectRoom;
	static HWND buttonCreateRoom;
	static HWND buttonDisconnect;
	static HMENU menu;
	static HMENU subMenu;

	switch (msg)
	{
	case WM_CREATE:
	{
		RECT rectSize;

		GetClientRect(hWnd, &rectSize);

		int widthScrollBar = GetSystemMetrics(SM_CXVSCROLL);
		
		widthEditMessageView = 280 - widthScrollBar;

		HINSTANCE hInstance = GetModuleHandle(NULL);

		editMessageComponent = CreateWindow(_T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL 
			| ES_MULTILINE | ES_READONLY | WS_VSCROLL, 0, 0,
			(rectSize.right / 2) - 10, rectSize.bottom, hWnd, 
			NULL, GetModuleHandle(NULL), NULL
		);

		editPostMessageComponent = CreateWindow(_T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL
			| ES_MULTILINE  | WS_VSCROLL, rectSize.right / 2, 0, (rectSize.right / 2), 100, 
			hWnd, NULL, GetModuleHandle(NULL), NULL
		);

		editAddressRoom = CreateWindow(_T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
			rectSize.right / 2, 200, (rectSize.right / 2) - widthScrollBar, 20,
			hWnd, (HMENU)IBD_EDIT_ADDRESS_ROOM, GetModuleHandle(NULL), NULL
		);

		SetWindowSubclass(editAddressRoom, editAddressProc, NULL, NULL);

		buttonSendMessage = CreateWindow(_T("BUTTON"), _T("отправить"), WS_CHILD | WS_VISIBLE | WS_BORDER,
			rectSize.right / 2, 105, (rectSize.right / 2) - widthScrollBar, 50, hWnd, 
			(HMENU)IDB_BUTTON_SEND, GetModuleHandle(NULL), NULL
		);


		buttonCreateRoom = CreateWindow(_T("BUTTON"), _T("создать"), WS_CHILD | WS_VISIBLE | WS_BORDER,
			rectSize.right / 2, 230, (((rectSize.right / 2) - widthScrollBar) / 2) - 5, 25,
			hWnd, (HMENU)IBD_BUTTON_CREATE, hInstance, NULL
		);

		buttonConnectRoom = CreateWindow(_T("BUTTON"), _T("подключиться"), WS_CHILD | WS_VISIBLE | WS_BORDER,
			((rectSize.right / 2) + ((rectSize.right / 2) - widthScrollBar) / 2) + 10, 230, (((rectSize.right / 2) - widthScrollBar) / 2) - 5, 25,
			hWnd, (HMENU)IBD_BUTTON_CONNECT, hInstance, NULL
		);

		buttonDisconnect = CreateWindow(_T("BUTTON"), _T("покинуть"),
			WS_CHILD | WS_VISIBLE | WS_BORDER, (rectSize.right / 2), 270,
			rectSize.right / 2 - widthScrollBar, 25, hWnd, (HMENU)IBD_BUTTON_DISCONNECT, 
			hInstance, NULL
		);
	
		menu = CreateMenu();
		subMenu = CreatePopupMenu();
		AppendMenu(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(subMenu), _T("пользователь"));
		AppendMenu(subMenu, MF_STRING, ID_CHANGE_NICKNAME, _T("изменить ник"));
		SetMenu(hWnd, menu);

		connectRoom = serverAddres;

		initServer(server, hWnd, &editMessageComponent, serverAddres, &stateMachine.getState());
		initStateMachine(stateMachine, hWnd, &editMessageComponent, &editPostMessageComponent,
			&buttonSendMessage, &buttonDisconnect, &buttonConnectRoom,
			&buttonCreateRoom, &editAddressRoom);

		stateMachine.workActionForActualState();

		server.SetStatus(StatusServer::NOT_LISTEN);

		server.start(queueMessage, connectionUsers);
		sender.start(queueMessage, messageStore, connectionUsers, user);
		
		break;
	}
	case WM_DESTROY:
	{
		if (stateMachine.getState() == State::CREATE_ACTIVE)
		{
			sender.sendClosedMessageAll(connectionUsers);
		}
		else if (stateMachine.getState() == State::CONNECT_ACTIVE)
		{
			sender.sendQueryToExit(user, connectRoom);
		}

		server.stop();
		sender.stop();

		PostQuitMessage(0);
		break;
	}
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case IDB_BUTTON_SEND:
		{
			int lenghtPostMessage = GetWindowTextLength(editPostMessageComponent);
			
			if (lenghtPostMessage == 0)
			{
				MessageBox(NULL, _T("сообщение пустое"), _T("message"), MB_OK);
				break;
			}
			else
			{
				tstring s = getStringFromComponent(editPostMessageComponent);		
				clearMessage(s);
				StatusSend status = sender.sendMessage(user, s, connectRoom);

				if (status.isConnect == false)
				{
					changeState(stateMachine, State::DISCONNECT, hWnd);
					break;
				}
				else if (status.isSend == false)
				{
					MessageBox(NULL, _T("отправка не завершилось попробуйте снова"), _T("info"), MB_OK);
					break;
				}
				else
				{
					SetWindowText(editPostMessageComponent, _T(""));
				}
			}
			break;
		}
		case IBD_BUTTON_CONNECT:
		{
			tstring addressConnectString = getStringFromComponent(editAddressRoom);
			std::string addressConnectStr = wcharToChar(addressConnectString);
			auto sockAddress = connectRoom.getAddress();

			if (inet_pton(AF_INET, addressConnectStr.data(), &sockAddress) == 1)
			{
				connectRoom = Address(TypeAddress::IPV4, addressConnectStr, 12345);

				if (connectRoom == serverAddres)
				{
					MessageBox(hWnd, _T("нельзя подключатся к самому себе"), _T("error"), MB_OK);
					break;
				}

				bool statusConnect = connectWithTimeout(connectRoom, 1, user);

				if (statusConnect)
				{
					changeState(stateMachine, State::CONNECT_ACTIVE, hWnd);
					server.SetStatus(StatusServer::JOIN_ROOM);
				}
				else
				{
					MessageBox(NULL, _T("такой комнаты нету"), _T("information"), MB_OK);
				}
			}
			break;
		}
		case IBD_BUTTON_DISCONNECT:
		{
			if (State::CONNECT_ACTIVE == stateMachine.getState())
			{
				sender.sendQueryToExit(user, connectRoom);
			}
			if (State::CREATE_ACTIVE == stateMachine.getState())
			{
				sender.sendClosedMessageAll(connectionUsers);
			}
			changeState(stateMachine, State::DISCONNECT, hWnd);
			server.SetStatus(StatusServer::NOT_LISTEN);
			break;
		}
		case IBD_BUTTON_CREATE:
		{
			connectRoom = serverAddres;
			server.SetStatus(StatusServer::CREATE_ROOM);
			changeState(stateMachine, State::CREATE_ACTIVE, hWnd);
			break;
		}
		case ID_CHANGE_NICKNAME:
		{
			MessageBox(hWnd, _T("скоро будет"), _T("заглушка"), MB_OK);
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_CHANGE_STATE:
	{		
		if (stateMachine.getState() == State::DISCONNECT)
			server.SetStatus(StatusServer::NOT_LISTEN);

		stateMachine.workActionForActualState();
		break;
	}
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
		break;
	}

	return 0;
}
LRESULT CALLBACK editAddressProc(HWND hWnd, UINT  msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefDara)
{
	switch (msg)
	{
	case WM_CHAR:
	{
		tchar key = wParam;
		
		if (_istdigit(key) || key == L'.' || key == L'\b')
		{
			return DefSubclassProc(hWnd, msg, wParam, lParam);
		}
		else
		{
			return 0;
		}	
		break;
	}
	default:
		return DefSubclassProc(hWnd, msg, wParam, lParam);
		break;
	}
}
tstring getStringFromComponent(HWND hWnd)
{
	int lenghtText = GetWindowTextLength(hWnd);

	tstring string(lenghtText, L'\0');

	GetWindowText(hWnd, string.data(), lenghtText + 1);


	return string;
}
void setStringComponent(HWND* hWnd, tstring& s)
{
	SetWindowText(*hWnd, s.data());
}
void initSender(Sender& sender)
{
	sender.setCallback(static_cast<int>(Command::SEND_MESSAGE_TO_SERVER), [](Message& message, Sender& sender, ListUser& listUser)
	{
		auto task = std::async(std::launch::async, &Sender::sendMessageToServer, &sender, std::ref(message.user), std::ref(message.addressServer), std::ref(message.message));
	});
	sender.setCallback(static_cast<int>(Command::SEND_ALL_USERS), [](Message& message, Sender& sender, ListUser& listUser)
	{
		message.type = Command::RECV_MESSAGE;
		auto task = sender.asyncSendMessageAll(listUser, message);
	});
	sender.setCallback(static_cast<int>(Command::INIT_USER_NOTIFICATION), [](Message& message, Sender& sender, ListUser& listUser)
	{
		auto task = sender.asyncSendMessageAll(listUser, message);
	});
	sender.setCallback(static_cast<int>(Command::EXIT_USER_NOTIFICATION), [](Message& message, Sender& sender, ListUser& listUser)
	{
		auto task = sender.asyncSendMessageAll(listUser, message);			
	});
	sender.setCallback(static_cast<int>(Command::ROOM_IS_NOT_CREATE_NOTIFICATION),[](Message& message, Sender& sender, ListUser& listUser)
	{
		auto task = std::async(std::launch::async, &Sender::sendMessageToUser, &sender, std::ref(message.user), std::ref(message));
 	});
}
void initServer(Server& server, HWND mainWindow, HWND* editViewMessage, Address& addressServer, int* state)
{
	server.setCallback(static_cast<int>(StatusServer::CREATE_ROOM), [mainWindow, editViewMessage, state, addressServer]
	(Connection& conn, thread_safe_queue<Message>& queue, Message& message, ListUser& listUsers)
	{		
		switch (message.type)
		{
			case Command::SEND_MESSAGE_TO_SERVER:
			{
				if (conn.getAddress().getIpAddress() != addressServer.getIpAddress())
				{
					if (!listUsers.userExist(message.user))
					{
						return;
					}
				}

				auto userPtr = listUsers.getUserByNickname(message.user.nickname);

				if (userPtr != nullptr)
				{
					message.user.addressSocket = userPtr->addressSocket;
				}
				
				viewMessage(message, *editViewMessage, true);
				sendNotification(message, queue, Command::SEND_ALL_USERS);
				
				break;
			}
			case Command::INIT_USER:
			{
				//TODO ИСПРАВИТЬ ПОТОМ ПОРТ НА 12345
				message.user.addressSocket = Address(TypeAddress::IPV4, conn.getAddress().getIpAddress(), 1234);
				listUsers.addUser(message.user);			
				viewMessage(message, *editViewMessage, false);
				sendNotification(message, queueMessage, Command::INIT_USER_NOTIFICATION);
				break;
			}
			case Command::EXIT_USER:
			{
				listUsers.eraseUser(message.user.nickname);
				viewMessage(message, *editViewMessage, false);
				sendNotification(message, queueMessage, Command::EXIT_USER_NOTIFICATION);
				break;
			}
			default:
				break;
		}
	});
	server.setCallback(static_cast<int>(StatusServer::JOIN_ROOM), [mainWindow, editViewMessage, state]
	(Connection& conn, thread_safe_queue<Message>& queue, Message& message, ListUser& listUsers)
	{
		switch (message.type)
		{
			case Command::RECV_MESSAGE:
			{
				viewMessage(message, *editViewMessage, false);
				break;
			}
			case Command::INIT_USER_NOTIFICATION:
			{
				viewMessage(message, *editViewMessage, false);
				break;
			}
			case Command::EXIT_USER_NOTIFICATION:
			{
				viewMessage(message, *editViewMessage, false);
				break;
			}
			case Command::CLOSE_ROOM:
			{
				tstring viewMessage = L" Closed Room";
				viewMessage += lineFeed;
				clearAndSetStringComponent(*editViewMessage, viewMessage);
				*state = 3;
				PostMessage(mainWindow, WM_CHANGE_STATE, NULL, NULL);
				break;
			}
			case Command::ROOM_IS_NOT_CREATE_NOTIFICATION:
			{
				*state = 3;
				PostMessage(mainWindow, WM_CHANGE_STATE, NULL, NULL);
				MessageBox(mainWindow, _T("комната еще не создана"), _T("info"), MB_OK);
				break;
			}
			default:
			{
				message.user.addressSocket = Address(TypeAddress::IPV4, conn.getAddress().getIpAddress(), 1234);
				sendNotification(message, queue, Command::ROOM_IS_NOT_CREATE_NOTIFICATION);
				break;
			}
		}
	});
	server.setCallback(static_cast<int>(StatusServer::NOT_LISTEN), [mainWindow, editViewMessage, state]
	(Connection& conn, thread_safe_queue<Message>& queue, Message& message, ListUser& listUsers)
	{
		switch (message.type)
		{
			case Command::SEND_MESSAGE_TO_SERVER:
			case Command::INIT_USER:
			{
				message.user.addressSocket = Address(TypeAddress::IPV4, conn.getAddress().getIpAddress(), 1234);
				sendNotification(message, queue, Command::ROOM_IS_NOT_CREATE_NOTIFICATION);
				break;
			}
			case Command::ROOM_IS_NOT_CREATE_NOTIFICATION:
			{
				*state = 3;
				PostMessage(mainWindow, WM_CHANGE_STATE, NULL, NULL);
				MessageBox(mainWindow, _T("комната еще не создана"), _T("info"), MB_OK);
				break;
			}
			default:
				break;
		}
	});
}
void initStateMachine(StateMachine& machine,HWND mainWindow, HWND* editMessageView, HWND* editPostMessage, HWND* buttonSend,
HWND* buttonDisconnect, HWND* buttonConnect, HWND* buttonCreate, HWND* editAddressRoom)
{
	machine.insertState(State::NOT_ACTIVE, [mainWindow, editMessageView, editPostMessage, buttonSend, buttonDisconnect]()
	{
			EnableWindow(*buttonSend, FALSE);
			EnableWindow(*editPostMessage, FALSE);
			EnableWindow(*buttonDisconnect, FALSE);

			SetWindowText(*editMessageView, _T(""));
			SetWindowText(*editPostMessage, _T(""));
	});

	machine.insertState(State::CREATE_ACTIVE, [mainWindow, editMessageView, editPostMessage, buttonSend, 
		buttonDisconnect, buttonConnect, buttonCreate, editAddressRoom]()
		{
			EnableWindow(*buttonConnect, FALSE);
			EnableWindow(*buttonCreate, FALSE);
			EnableWindow(*editPostMessage, TRUE);
			EnableWindow(*buttonSend, TRUE);
			EnableWindow(*editAddressRoom, FALSE);
			EnableWindow(*buttonDisconnect, TRUE);
			SetWindowText(*editMessageView, _T(""));
			SetWindowText(*editPostMessage, _T(""));
		});

	machine.insertState(State::CONNECT_ACTIVE, [mainWindow, editMessageView, editPostMessage, buttonSend,
		buttonDisconnect, buttonConnect, buttonCreate, editAddressRoom]()
		{
			EnableWindow(*buttonConnect, FALSE);
			EnableWindow(*buttonCreate, FALSE);
			EnableWindow(*editPostMessage, TRUE);
			EnableWindow(*buttonSend, TRUE);
			EnableWindow(*editAddressRoom, FALSE);
			EnableWindow(*buttonDisconnect, TRUE);
			SetWindowText(*editMessageView, _T(""));
			SetWindowText(*editPostMessage, _T(""));
		});

	machine.insertState(State::DISCONNECT, [mainWindow, editMessageView, editPostMessage, buttonSend,
		buttonDisconnect, buttonConnect, buttonCreate, editAddressRoom]()
		{
			EnableWindow(*buttonSend, FALSE);
			EnableWindow(*editPostMessage, FALSE);
			EnableWindow(*buttonDisconnect, FALSE);
			EnableWindow(*buttonCreate, TRUE);
			EnableWindow(*buttonConnect, TRUE);
			EnableWindow(*editAddressRoom, TRUE);
			SetWindowText(*editPostMessage, _T(""));
		});

}
void viewMessage(Message& message, HWND& windowView, bool isSetDate)
{
	if (isSetDate)
		message.setDate();

	tstring textWindow = getStringFromComponent(windowView);

	switch (message.type)
	{
	case Command::RECV_MESSAGE:
	case Command::SEND_MESSAGE_TO_SERVER:
	{
		auto messageText = (message.getDate() + L" " + message.user.nickname + L" : " + message.message + lineFeed);
		messageText = validateMessage(messageText, windowView);

		textWindow += messageText;
		break;
	}
	case Command::EXIT_USER_NOTIFICATION:
	case Command::EXIT_USER:
	{
		auto messageText = (message.user.nickname + L" left the room" + lineFeed);
		messageText = validateMessage(messageText, windowView);

		textWindow += messageText;
		break;
	}
	case Command::INIT_USER_NOTIFICATION:
	case Command::INIT_USER:
	{
		auto messageText = (message.user.nickname + L" connected in the room" + lineFeed);
		messageText = validateMessage(messageText, windowView);

		textWindow += message.user.nickname + L" connected in the room" + lineFeed;
		break;
	}
	default:
		break;
	}
	setStringComponent(&windowView, textWindow);
	
}
void sendNotification(Message& message, thread_safe_queue<Message>& queue, Command changeType)
{
	if (changeType != Command::NONE)
	{
		message.type = changeType;
	}

	queue.push(message);
}
bool connectWithTimeout(Address& address, int timeoutSeconds, User& user)
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Переводим сокет в неблокирующий режим
	u_long nonBlockingMode = 1;

	if (ioctlsocket(sock, FIONBIO, &nonBlockingMode) != 0)
	{
		return false;
	}

	sockaddr_in sockAddr = address.getAddress();

	// Устанавливаем соединение
	int result = connect(sock, (SOCKADDR*)&sockAddr, sizeof(sockAddr));

	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			return false;
		}
	}

	// Используем select для ожидания завершения соединения или таймаута
	fd_set writeSet;
	FD_ZERO(&writeSet);
	FD_SET(sock, &writeSet);

	timeval timeout;
	timeout.tv_sec = timeoutSeconds;
	timeout.tv_usec = 0;

	result = select(0, NULL, &writeSet, NULL, &timeout);

	if (result == SOCKET_ERROR)
	{
		return false;
	}
	else if (result == 0)
	{
		return false;
	}

	// Проверяем, было ли соединение установлено
	if (FD_ISSET(sock, &writeSet))
	{
		// Возвращаем сокет в блокирующий режим
		u_long blockingMode = 0;

		if (ioctlsocket(sock, FIONBIO, &blockingMode) != 0)
		{
			return false;
		}

		Message message(user, L"", Command::INIT_USER);

		std::vector<char> bytes = Convector::messageToBytes(message);

		send(sock, bytes.data(), bytes.size(), 0);

		closesocket(sock);

		return true;
	}
	return false;
}
void clearAndSetStringComponent(HWND& component, const tstring& s)
{
	SetWindowText(component, s.c_str());
}
WNDCLASS initWindowClass(HINSTANCE& hInstance, LRESULT (CALLBACK *func)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)) throw(std::exception)
{
	WNDCLASS wndClass = { 0 };

	wndClass.lpszClassName = className.c_str();
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = func;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	if (!RegisterClass(&wndClass))
	{
		throw std::exception("ERROR");
	}

	return wndClass;
}
HWND createWindow(tstring& className, tstring& windowName, HINSTANCE& hInstance, int modeShow) throw(std::exception)
{
	HWND window = CreateWindow(className.c_str(), windowName.c_str(), WS_OVERLAPPED | WS_CAPTION |
		WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
		NULL, NULL, hInstance, NULL);

	if (!window)
	{
		throw std::exception("Window Create");
	}

	ShowWindow(window, modeShow);
	UpdateWindow(window);

	return window;
}
INITCOMMONCONTROLSEX initCommoonContEx()
{
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);

	return icex;
}
void changeState(StateMachine& machine, int State, HWND mainWindow)
{
	machine.changeStatus(State);
	PostMessage(mainWindow, WM_CHANGE_STATE, NULL, NULL);
}
void clearMessage(tstring& message)
{
	auto start = std::find_if_not(message.begin(), message.end(), ::iswspace);
	auto end = std::find_if_not(message.rbegin(), message.rend(), ::iswspace).base();
	message = tstring(start, end);
}
std::wstring generateNickname()
{
	std::call_once(initRand, initRandom);

	std::wstring user = L"user";

	for (int i = 0; i < 3; i++)
	{
		int tempNumber = rand() % 10;
		user += std::to_wstring(tempNumber);
	}

	return user;
}
void initRandom()
{
	srand(time(NULL));
}
std::wstring validateMessage(std::wstring& message, HWND window)
{
	std::wstringstream sstream(message);
	std::wstring resultMessage;
	std::wstring word;

	int widthLineCurrent = 0;

	HDC hdc = GetDC(window);

	while (sstream >> word)
	{
		SIZE size;
		
		word += L" ";

		GetTextExtentPoint32(hdc, word.c_str(), word.size(), &size);

		if ((widthLineCurrent + size.cx) > widthEditMessageView)
		{
			resultMessage += lineFeed;

			if (size.cx >= widthEditMessageView)
			{
				int widthTemp = 0;

				for (const wchar_t& wch : word)
				{
					SIZE sizeChar;
					GetTextExtentPoint32(hdc, static_cast<LPCWSTR>(&wch), 1, &sizeChar);

					if ((widthTemp + sizeChar.cx) >= widthEditMessageView)
					{
						resultMessage += lineFeed;
						widthTemp = sizeChar.cx;
						resultMessage += wch;
					}
					else
					{
						resultMessage += wch;
					}

					widthTemp += sizeChar.cx;
				}
				widthLineCurrent = widthTemp;
			}
			else
			{
				widthLineCurrent = size.cx;
				resultMessage += word;
			}
		}
		else
		{
			resultMessage += word;
			widthLineCurrent += size.cx;
		}

	}

	resultMessage += lineFeed;
	
	ReleaseDC(window, hdc);
	
	return resultMessage;
}
