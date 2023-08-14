
// =================================================
// ip相关的工具类
// Justin
//
// =================================================

#ifndef _IP_UTILS_H_
#define _IP_UTILS_H_

#include <string>
#include <cstdint>
#include <cstring>
#include <iostream>

#ifdef PLATFORM_WINDOWS
#include <winsock2.h>
#include<ws2tcpip.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif

using namespace std;

class ip_utils
{
    public:
	static bool get_local_ip(const char* ethcard_name, uint32_t& ip, uint32_t& netmask);
	static std::string ip2string(uint32_t ipaddr);
};

#endif

