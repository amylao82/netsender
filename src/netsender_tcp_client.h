
#ifndef _NETSENDER_TCP_CLIENT_H_
#define _NETSENDER_TCP_CLIENT_H_

#include "netsender.h"

#include <cstring>

class netsender_tcp_client : public netsender
{
    constexpr static int BUFFER_SIZE = 2048;
    public:
	//继承来的对外接口
	virtual bool isConnect();
	virtual int send_buf(std::string str, const SOCKETINFO* socketinfo);
	virtual int send_buf(const char* data, int len, const SOCKETINFO* socketinfo);

	virtual bool disconnect();

    public:
	//非继承来的对外函数.
	netsender_tcp_client(string server, int port, protocol_interface* protocol);
	~netsender_tcp_client();

	bool init();

    protected:
	static void thread_recv(netsender_tcp_client* args);
	void thread_recv_proc();
	void stopRecvThread();

    protected:
	string m_server;
	int m_socket;
	int m_port;

	bool m_exit;
};

#endif

