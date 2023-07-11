
// =======================================
// 数据处理基类.
// 通过与netsender协同工作,处理所有数据相关的解释处理
// amy
// 2023.07.05
//
// =======================================

#ifndef _PROTOCOL_INTERFACE_H_
#define _PROTOCOL_INTERFACE_H_

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


using namespace std;

class netsender;

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

class protocol_interface
{
    public:
	virtual ~protocol_interface()
	{
	};

	virtual void recv_data(void* data, int len, const SOCKETINFO& socket);


	//发送函数提供继承,内部可能要增加帧头.
	virtual int send_data(const void* data, int len);
	virtual int send_data(string str);
	//只有服务器端回应客户时,才会有socket参数.
	virtual int send_data(const void* data, int len, const SOCKETINFO& socket);
	virtual int send_data(string str, const SOCKETINFO& socket);


	void set_netsender(netsender* sender)
	{
	    m_netsender = sender;
	}

    private:
	//shared_ptr<netsender> m_netsender;
	netsender* m_netsender;

};

#endif

