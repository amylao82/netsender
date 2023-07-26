
#ifndef _NETSENDER_TCP_CLIENT_H_
#define _NETSENDER_TCP_CLIENT_H_

#include "netsender_baseimpl.h"

#include <cstring>

class netsender_tcp_client : public netsender_base_impl
{
    public:
	//继承来的对外接口
	virtual bool isConnect();

	virtual bool init(socketopt* opt);

//	virtual bool set_sync_info(string sync_word, int fieldlen_sync_word, int fieldlen_packetsize, bool network_byte_order = false);

    public:
	//非继承来的对外函数.
	netsender_tcp_client(string server, int port, recvcb_interface* protocol);
	~netsender_tcp_client();

    protected:
	//继承而来的接口.
	virtual int send_internal(const SOCKETINFO* socketinfo);
	virtual int recv_net_packet(char* buf, int buf_len, SOCKETINFO& socketinfo);
	virtual void close_communicate_socket(SOCKETINFO& socketinfo);

    protected:
//	static void thread_recv(netsender_tcp_client* args);
//	void thread_recv_proc();
	void stop_recv_thread();

    protected:

};

#endif

