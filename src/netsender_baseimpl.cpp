
#include "netsender_baseimpl.h"

netsender_base_impl::netsender_base_impl(string server, int port, protocol_interface* protocol_iface)
    :netsender()
     ,m_server(server)
     ,m_port(port)
     ,m_protocol_iface(protocol_iface)
     ,m_socket(-1)
{
#ifdef PLATFORM_WINDOWS
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
	std::cerr << "Call to WSAStartup failed." << std::endl;
    }
#endif
}

netsender_base_impl::~netsender_base_impl()
{
#ifdef PLATFORM_WINDOWS
    WSACleanup();
#endif
}


