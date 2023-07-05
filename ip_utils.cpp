
#include "ip_utils.h"


std::string ip_utils::ip2string(uint32_t ipaddr)
{
    struct in_addr addr;

    addr.s_addr = ipaddr;
    return inet_ntoa(addr);
}

#ifdef PLATFORM_WINDOWS
bool ip_utils::get_local_ip(const char* ethcard_name, uint32_t& ip, uint32_t& netmask)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Call to WSAStartup failed." << std::endl;
        return false;
    }

    //SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
    SOCKET sd = socket(AF_INET, SOCK_DGRAM, 0);
    std::string strIP;

    if (sd == SOCKET_ERROR)
    {
        closesocket(sd);
        return false;
    }

    INTERFACE_INFO InterfaceList[20];
    unsigned long nBytesReturned;

    if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList, sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR)
    {
        closesocket(sd);
        return false;
    }

    int nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);

    //cout << " nNumInterfaces = " << nNumInterfaces << endl;

    for (int i = 0; i < nNumInterfaces; i++)
    {
        sockaddr_in* pAddress;
        pAddress = (sockaddr_in*)&(InterfaceList[i].iiAddress);
        strIP = inet_ntoa(pAddress->sin_addr);
        if (strIP == "127.0.0.1")
        {
            continue;
        }

        ip = pAddress->sin_addr.s_addr;
        pAddress = (sockaddr_in*)&(InterfaceList[i].iiNetmask);
        netmask = pAddress->sin_addr.s_addr;

        //cout << " ip = " << hex << ip << " netmask = " << netmask << endl;
        break;
    }
    closesocket(sd);
    return true;
}

#else

bool ip_utils::get_local_ip(const char* ethcard_name, uint32_t& ip, uint32_t& netmask)
{
    int inet_sock;
    struct ifreq ifr;
//    char ip[32]={nullptr};

    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(inet_sock < 0)
    return false;

    //strcpy(ifr.ifr_name, "eth0");
//  strcpy(ifr.ifr_name, "eno1");
    strcpy(ifr.ifr_name, ethcard_name);
    ioctl(inet_sock, SIOCGIFADDR, &ifr);

    ip = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
    //cout << " ip uint32_t = " << hex << temp << endl;

    ioctl(inet_sock, SIOCGIFNETMASK, &ifr);
    netmask = ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr;
    //cout << "netmask = 0x" << hex << mask << endl;
    return true;
}

#endif

