
#ifndef _NETSENDER_BASE_IMPL_H_
#define _NETSENDER_BASE_IMPL_H_

#include "netsender.h"

class netsender_base_impl: public netsender
{
    protected:
	constexpr static int BUFFER_SIZE = 2048;

    public:
	netsender_base_impl(string server, int port, protocol_interface* protocol_iface);

	virtual ~netsender_base_impl();

	//从netsender继承过来的接口.
	virtual bool disconnect();

	virtual bool init(socketopt* opt) = 0;

    protected:

	//type可以取值SOCK_STREAM/SOCK_DGRAM
	bool open_socket(int type);
	bool close_socket();

	void set_socket_option(socketopt* opt);
	//从网络地址或者域名里得到in_addr_t信息
	in_addr_t get_net_addr(string str_dest);

    protected:
	string m_server;
	int m_port;
	protocol_interface* m_protocol_iface;
//	socketopt* opt;

	int m_socket;
};

#endif
