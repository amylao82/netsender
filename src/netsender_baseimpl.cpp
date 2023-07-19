
#include "netsender_baseimpl.h"
#include <stdio.h>


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

void netsender_base_impl::set_socket_option(socketopt* opt)
{
    if(opt == nullptr)
	return ;

    vector<socketopt::opt_item>::iterator it;
    for(it = opt->m_vector.begin(); it != opt->m_vector.end(); ++it)
    {
	setsockopt(m_socket, it->m_level, it->m_opt_name, it->m_data, it->m_len);
    }
}

bool netsender_base_impl::open_socket(int type)
{
    m_socket = socket(AF_INET, type, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
    if(m_socket < 0)
    {
	printf("create socket fail!\n");
	return false;
    }
    return true;
}

bool netsender_base_impl::close_socket()
{
    if(m_socket >= 0)
    {
	//关闭读写通道,否则无法退出recvfrom的调用线程.
#ifdef PLATFORM_WINDOWS
	shutdown(m_socket, SD_BOTH);
	closesocket(m_socket);
#else
	shutdown(m_socket, SHUT_RDWR);
	close(m_socket);
#endif
	m_socket = -1;
    }

    return true;
}

in_addr_t netsender_base_impl::get_net_addr(string str_dest)
{
    //如果传进来的是一个域名,需要通过域名获取一个host.
    struct hostent *host = gethostbyname(str_dest.c_str());

    if(host != nullptr)
	return inet_addr(inet_ntoa( *(struct in_addr*)host->h_addr_list[0] ));
    else
	return inet_addr(m_server.c_str());
}

bool netsender_base_impl::disconnect()
{
//    printf("base_impl::disconnect\n");
    close_socket();
    return true;
}
