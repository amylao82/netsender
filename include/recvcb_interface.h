
// ======================================================
//
// 数据接收的回调函数接口,网络层通过些接口往上层传输数据.
// 处理流程对接收到的数据增加自己的处理流程.
//
// AmyLao
// 2023.07.24
//
// ======================================================

#ifndef _RECVCB_INTERFACE_H_
#define _RECVCB_INTERFACE_H_

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

#include <string>
#include <memory>
#include <vector>

using namespace std;

//定义一个返回的结构体,里面包含接收到数据的相关信息.
//如果是udp,里面是struct sockaddr
//如果是TCP,里面是socket.
//如果是WEBSOCKET,里面是websocket相关的接口信息.
typedef union {
    struct _udp {
	struct sockaddr socket;
    } udp;
    struct _tcp {
	int socket;
    } tcp;
} SOCKETINFO;


class recvcb_interface
{
    public:
	//接收到数据包的结构
	class recv_packet
	{
	    public:
		recv_packet(char* data, int len, const SOCKETINFO& info)
		    :vec(data, data + len)
		     ,socketinfo(info) {}

		vector<char> vec;
		SOCKETINFO socketinfo;
	};

    public:
	virtual ~recvcb_interface() {};
	virtual void recv_data(shared_ptr<recv_packet> packet) = 0;
};

#endif

