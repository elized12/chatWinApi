#define _CRT_SECURE_NO_WARNINGS
#include "message.h"
#include <codecvt>


void Message::setDate()
{
	std::time_t now = std::time(nullptr);

	std::tm nowTm;

	localtime_s(&nowTm, &now);

	char buffer[80];

	strftime(buffer, sizeof(buffer), "%H:%M", &nowTm);

    _date = charToWchar(buffer, strlen(buffer));

}

void Message::setDate(std::wstring date)
{
	this->_date = date;
}

std::wstring Message::getDate()
{
	return _date;
}

std::wstring charToWchar(const char* str, int byteLength)
{
    int length = MultiByteToWideChar(CP_ACP, 0, str, byteLength, nullptr, 0);

    if (length == 0) 
    {
        throw std::runtime_error("Failed to convert char to wchar_t");
    }

    std::wstring wstr(length, L'\0');

  
    if (MultiByteToWideChar(CP_ACP, 0, str, byteLength, &wstr[0], length) == 0)
    {
        throw std::runtime_error("Failed to convert char to wchar_t");
    }

    return wstr;
}

std::string wcharToChar(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();

    int sizeNeed = WideCharToMultiByte(CP_ACP, 0, wstr.data(),
        wstr.size(), NULL, 0, NULL, NULL);

    std::string strTo(sizeNeed, 0);

    WideCharToMultiByte(CP_ACP, 0, wstr.data(), wstr.size(), strTo.data(),
        sizeNeed, NULL, NULL);

    return strTo;
}


