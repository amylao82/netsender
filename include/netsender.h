
// =======================================================
//
// 网络发送的接口类,定义tcp/udp发送共用的接口,
// 其下继承的子类,通过调用不同的网络接口实现功能
// 这个接口把二种协议的差异抹平.上层只要使用参数创建对应的sender即可.
//
// Amy
//
// =======================================================
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

#include "recvcb_interface.h"
#include "socketopt.h"
#include "syncword_info.h"

using namespace std;

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
	//2023.02.20. 对于服务器端, connectServer也是需要的,因为有的设备有二个网卡,需要指定绑定到哪一个网卡上.
	static netsender* createSender(netsender::PROTOCOL_TYPE protocol	//使用的连接协议
		, netsender::NETSENDER_TYPE type	//服务器还是客户端
		, std::string connectServer		//要连接的服务器地址,
		, int port				//使用的端口
		, recvcb_interface* recv_iface		//接收数据的接口
		, syncword_info* syncinfo		// 同步字相关信息
		, socketopt* opt = nullptr);		//对socket的额外设置

	virtual ~netsender() {};	//需要一个虚函数的析构函数,才能实现从基类的指针链式调用释放子类.

	virtual bool isConnect() = 0;

	//设置接收数据包的帧头信息,
	//sync_word,同步字
	//len_field_sync_word, 同步字字段的长度
	//len_field_packetsize, 数据长度占用的字节数.
//	virtual bool set_sync_info(string sync_word, int fieldlen_sync_word, int fieldlen_packetsize, bool network_byte_order = false) = 0;

	//发送数据函数分为二组,如果是发送可读数据,使用std::string来存储,
	//但是如果要发送的是二进制数据,需要使用char数组来存储.
	// 2023.03.02. 减掉二个接口,而使用默认参数代替.
	// 一般客户端往服务器发送,因为已经连接,里面已经保存了socketinfo.所以可以使用空参数
	// 而如果是服务器端往客户端发送.因为服务端需要服务多个客户端,因此只能使用需要应答的数据帧的源地址作为目的地址.
	virtual int send_buf(std::string str, const SOCKETINFO* socketinfo = nullptr) = 0;
	virtual int send_buf(const char* data, int len, const SOCKETINFO* socketinfo = nullptr) = 0;

	virtual bool disconnect() = 0;
};

#endif

