#include "Convector.h"
#include <sstream>
#include <string>
#include <codecvt>


Message Convector::bytesToMessage(std::vector<char>& bytes) 
{

    std::wstring s(charToWchar(bytes.data(), bytes.size()));

    std::wstringstream sstream(s);

    std::wstring typeString;

    std::getline(sstream, typeString, L'\0');

    Command type = static_cast<Command>(std::wcstol(typeString.c_str(), nullptr, NULL));

    if (static_cast<int>(type) < 1 && static_cast<int>(type) > 10)
        throw "Message not Validate";

    User user;

    std::getline(sstream, user.nickname, L'\0');

    if (user.nickname.size() == 0)
        throw "Message not Validate";

    std::wstring message;

    std::getline(sstream, message, L'\0');

    std::wstring date;

    if (!sstream.eof())
        std::getline(sstream, date);

    date.erase(std::remove_if(date.begin(), date.end(), [](wchar_t& ch)
        {
            if (ch == 0)
                return true;
            return false;
        }), date.end());

    Message resultMessage(user, message, type);

    resultMessage.setDate(date);

    return resultMessage;

}

std::vector<char> Convector::messageToBytes(Message& message)
{
    std::wstring temp = std::to_wstring(static_cast<int>(message.type));

    temp += L'\0';

    temp += message.user.nickname;

    temp += L'\0';

    temp += message.message;

    temp += L'\0';

    temp += message.getDate();

    temp += L'\0';

    std::string strTemp = wcharToChar(temp);

    return std::vector<char>(strTemp.begin(), strTemp.end());
}

bool Convector::tryBytesToMessage(std::vector<char>& bytes, Message& message)
{
    bool isConvect;

    try
    {
        message = Convector::bytesToMessage(bytes);
        isConvect = true;
    }
    catch (...)
    {
        isConvect = false;
    }

    return isConvect;

}


