
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

	virtual bool init(socketopt* opt) = 0;

    protected:

    protected:
	string m_server;
	int m_port;
	protocol_interface* m_protocol_iface;
//	socketopt* opt;

	int m_socket;
};

#endif
