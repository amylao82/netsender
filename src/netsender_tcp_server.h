
// =================================================
// tcp服务器.
// Justin
// 2023.07.04
// =================================================
#ifndef _NETSENDER_TCP_SERVER_H_
#define _NETSENDER_TCP_SERVER_H_


//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>
//#include <arpa/inet.h>
//#include <cstring>
//#include <thread>
//#include <iostream>

#include "netsender_baseimpl.h"

//using namespace std;

class netsender_tcp_server:public netsender_base_impl
{
    public:
	//继承来的对外接口
	virtual bool isConnect();
	virtual int send_buf(std::string str, const SOCKETINFO* socketinfo);
	virtual int send_buf(const char* data, int len, const SOCKETINFO* socketinfo);

	virtual bool disconnect();

    public:
	//非继承来的对外函数.
	netsender_tcp_server(string server, int port, protocol_interface* protocol);
	virtual ~netsender_tcp_server();

	virtual bool init(socketopt* opt = nullptr);

    protected:
	//接收连接线程
	static void thread_accept(netsender_tcp_server* args);
	void thread_accept_proc();

	//客户端处理线程
	static void thread_client(netsender_tcp_server* args, SOCKETINFO client_socket);
	void thread_client_proc(SOCKETINFO client_socket);

	void stop_recv_thread();
    protected:

//	int m_socket;
//	int m_port;

	struct sockaddr m_svrSockAddr;

	bool m_exit;
    private:
};

#endif

