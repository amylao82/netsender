
// ======================================================
// 设置socket的参数
// 用于传递到createSender创建函数里,设置socket的属性.
// netsender里会调用setsockopt来把每一个属性设置到socket里
//
// 比如要设置广播,　 set_opt_value(SOL_SOCKET, SO_BROADCAST, 1);
// 设置关闭nagle算法　set_opt_value(IPPROTO_TCP, TCP_NODELAY, 1);
// 设置发送/接收超时, windows与linux有差别,windows需要的是int值/linux需要的是struct timeval
// 	这个差异放到上层应用去处理.
// 	如果在linux,可以调用set_opt_value(SOL_SOCKET, SO_RCVTIMEO, &timeval, sizeof(timeval)
// 			set_opt_value(SOL_SOCKET, SO_SNDTIMEO, &timeval, sizeof(timeval)
// 	在windows下,可以写set_opt_value(SOL_SOCKET, SO_SNDTIMEO, timeout);
//
// amy(amylao82@hotmail)
//
// 通过实际使用,发现这个类的实现有点问题.
// 如果在创建了UDP连接,但是又设置了TCP_NODELAY,网络发送会不能工作.
// 即是TCP和UDP的参数不能相互使用.但这个类里却可以设置.
// 这个在后续想想别的实现方法规避.
//
// ======================================================

#ifndef _SOCKETOPT_H_
#define _SOCKETOPT_H_

#include <vector>
#include <cstring>

#ifdef PLATFORM_WINDOWS
#include <winsock2.h>
#include<ws2tcpip.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/tcp.h>
#endif

using namespace std;

class socketopt
{
    public:
	//单个选项类
	class opt_item
	{
	    public:
		opt_item(int level, int opt_name, const void* data, int len)
		{
		    m_level = level;
		    m_opt_name = opt_name;
		    m_data = new char[len];
		    memcpy(m_data, data, len);
		    m_len = len;
		}
		~opt_item()
		{
		    delete [] m_data;
		}
		opt_item(const opt_item& rhs)
		{
		    m_level = rhs.m_level;
		    m_opt_name = rhs.m_opt_name;
		    m_data = new char[rhs.m_len];
		    memcpy(m_data, rhs.m_data, rhs.m_len);
		    m_len = rhs.m_len;
		}

	    public:
		int m_level;
		int m_opt_name;
		char* m_data;
		int m_len;

	};

    public:
	socketopt();
	~socketopt();

	void set_opt_value(int level, int opt_name, const void* data, int len);
	void set_opt_value(int level, int opt_name, int value);

	vector<opt_item> m_vector;
};

#endif
