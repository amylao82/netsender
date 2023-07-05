
#ifndef _NETSENDER_H_
#define _NETSENDER_H_

#include <string>
#include <memory>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <thread>

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

#include "protocol_interface.h"

using namespace std;

class protocol_interface;

//定义一个返回的结构体,里面包含接收到数据的相关信息.
//如果是udp,里面是struct sockaddr
//如果是TCP,里面是socket.
//如果是WEBSOCKET,里面是websocket相关的接口信息.
//typedef union {
//    struct _udp {
//	struct sockaddr socket;
//    } udp;
//    struct _tcp {
//	int socket;
//    } tcp;
//} SOCKETINFO;
//data callback function .
//typedef void (*DATAPROC)(std::string, SOCKETINFO*, void*);

class netsender {
    public:
	typedef enum {
	    PROTOCOL_UDP = 0,
	    PROTOCOL_TCP = 1,
	    PROTOCOL_WEBSOCKET = 2,
	    PROTOCOL_HTTP = 3,
	} PROTOCOL_TYPE;

	typedef enum {
	    TYPE_SERVER = 0,
	    TYPE_CLIENT = 1,
	} NETSENDER_TYPE;

	//指定创建的是服务器端还是客户端, 如果是客户端,后面的参数是要连接的服务器.
	//shared_ptr<netsender> createSender(netsender::PROTOCOL_TYPE protocol, netsender::NETSENDER_TYPE type, std::string connectServer, int port);
	//2023.02.20. 对于服务器端, connectServer也是需要的,因为有的设备有二个网卡,需要指定绑定到哪一个网卡上.
	static netsender* createSender(netsender::PROTOCOL_TYPE protocol, netsender::NETSENDER_TYPE type, std::string connectServer, int port, shared_ptr<protocol_interface> protocol_iface);

	netsender();
	virtual ~netsender();	//需要一个虚函数的析构函数,才能实现从基类的指针链式调用释放子类.
//	void setDataCB(DATAPROC dataproc, void* arg) {m_cbFunc = dataproc; m_cbArg = arg;};

	virtual bool isConnect() = 0;

	//发送数据函数分为二组,如果是发送可读数据,使用std::string来存储,
	//但是如果要发送的是二进制数据,需要使用char数组来存储.
	// 2023.03.02. 减掉二个接口,而使用默认参数代替.
	// 一般客户端往服务器发送,因为已经连接,里面已经保存了socketinfo.所以可以使用空参数
	// 而如果是服务器端往客户端发送.因为服务端需要服务多个客户端,因此只能使用需要应答的数据帧的源地址作为目的地址.
//	virtual int send_buf(std::string str) = 0;
	virtual int send_buf(std::string str, const SOCKETINFO* socketinfo = nullptr) = 0;
//	virtual int send_buf(const char* data, int len) = 0;
	virtual int send_buf(const char* data, int len, const SOCKETINFO* socketinfo = nullptr) = 0;

	virtual bool disconnect() = 0;

    protected:
//	DATAPROC m_cbFunc;
//	void* m_cbArg;
	shared_ptr<protocol_interface> m_protocol_iface;
};

#endif

