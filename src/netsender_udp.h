﻿
#ifndef _NETSENDER_UDP_H_
#define _NETSENDER_UDP_H_

#include "netsender_baseimpl.h"

class netsender_udp: public netsender_base_impl
{
    public:
	//继承来的对外接口
	virtual bool isConnect();
	virtual int send_buf(std::string str, const SOCKETINFO* socketinfo);
	virtual int send_buf(const char* data, int len, const SOCKETINFO* socketinfo);

	virtual bool disconnect();

	virtual bool init(socketopt* opt);

    public:
	//非继承来的对外函数.
	netsender_udp(NETSENDER_TYPE type, string server, int port, protocol_interface* protocol_iface);
	virtual ~netsender_udp();
	//设置是否使用广播通信.
	void set_broadcast(bool broadcast);

    protected:
	//不对外函数
	//recevie thread.
	static void thread_recv(void* args);
	void thread_recv_proc(); 
	bool create_recv_thread();
	void stop_recv_thread();

	bool initServer(socketopt* opt = nullptr);
	bool connectServer(std::string strSever, socketopt* opt = nullptr);

	bool create_socket(socketopt* opt = nullptr);
	void set_socket_timeout(int timeout_sec);
	//设置socket的广播属性.
	bool set_socket_broadcast();

    protected:
	NETSENDER_TYPE m_type;
//	int m_port;
	bool m_broadcast;
	bool m_connectOK;

//#ifdef PLATFORM_WINDOWS
//	SOCKET m_socket;
//#else
//	int m_socket;
//#endif

    bool m_socket_mode_block_io; //socket为非阻塞状态。

	//接收数据线程.
	//pthread_t m_pidRecv;
//	std::shared_ptr<std::thread> m_thread_recv;
	bool m_bexit;
	//struct sockaddr m_svrSockAddr;
	SOCKETINFO m_svrSockAddr;
//    private:
//	netsender_udp(){};
};

#endif


