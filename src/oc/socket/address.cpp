#include "../../../include/oc/socket/components/address.h"


#ifdef _WIN32

Address::Address(TypeAddress _type, std::string _ip, unsigned int port) : ip(_ip)
{
    sockaddr_in address;
    memset(&address, 0, sizeof(address));

    switch (_type)
    {
    case TypeAddress::IPV4:
        address.sin_family = static_cast<int>(_type);
        break;
    case TypeAddress::IPV6:
        address.sin_family = static_cast<int>(_type);
        break;
    case TypeAddress::BTH:
        address.sin_family = static_cast<int>(_type);
        break;
    default:
        break;
    }

    int codeError = inet_pton(static_cast<int>(_type), ip.c_str(), &address.sin_addr);
    
    if (codeError != ADDRESS_VALIDATE)
    {
        throw "not validate ip";
    }

    if (port > 65535)
        throw "not validate port";

    address.sin_port = htons(port);

    _address = std::move(address);
}

sockaddr_in Address::getAddress() const
{
    return _address;
}

sockaddr_in Address::operator()() const
{
    return _address;
}

const std::string& Address::getIpAddress() const
{
    return ip;
}

bool Address::operator==(const Address& other) const
{
    if (ip == other.ip && _address.sin_port == other._address.sin_port)
    {
        return true;
    }

    return false;
}

bool Address::operator!=(const Address& other) const
{
    return !this->operator==(other);
}

sockaddr_in Address::createDefaultValueAddress()
{
    sockaddr_in invalidAddr;

    invalidAddr.sin_family = AF_UNSPEC;
    invalidAddr.sin_addr.s_addr = INADDR_NONE;
    invalidAddr.sin_port = htons(0);

    return invalidAddr;
    
}

#endif // _WIN32

