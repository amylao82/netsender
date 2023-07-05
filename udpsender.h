
#pragma once

#include "netsender.h"

class udpSender: public netsender
{
    public:
	//继承来的对外接口
	virtual bool isConnect();
	//virtual int send_buf(std::string str, struct sockaddr* addr);
//	virtual int send_buf(std::string str);
	virtual int send_buf(std::string str, SOCKETINFO* socketinfo);
//	virtual int send_buf(const char* data, int len);
	virtual int send_buf(const char* data, int len, SOCKETINFO* socketinfo);

	virtual bool disconnect();

    public:
	//非继承来的对外函数.
	udpSender(NETSENDER_TYPE type, int port);
	virtual ~udpSender();
	//设置是否使用广播通信.
	void set_broadcast(bool broadcast);
	bool initServer();
	bool connectServer(std::string strSever);

    protected:
	//不对外函数
	//recevie thread.
	void* threadReceiveProc(); 
	bool createRecvThread();
	void stopRecvThread();

	bool create_socket();
	void set_socket_timeout(int timeout_sec);
	//设置socket的广播属性.
	bool set_socket_broadcast();

    protected:
	NETSENDER_TYPE m_type;
	int m_port;
	bool m_broadcast;
	bool m_connectOK;

#ifdef PLATFORM_WINDOWS
	SOCKET m_socket;
#else
	int m_socket;
#endif

    bool m_socket_mode_block_io; //socket为非阻塞状态。

	//接收数据线程.
	//pthread_t m_pidRecv;
	std::shared_ptr<std::thread> m_thread_recv;
	bool m_bStopRecv;
	//struct sockaddr m_svrSockAddr;
	SOCKETINFO m_svrSockAddr;
    private:
	udpSender(){};
};

