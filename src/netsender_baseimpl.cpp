
#include "netsender_baseimpl.h"
#include <stdio.h>


const syncword_info netsender_base_impl::m_constsyncinfo;

netsender_base_impl::netsender_base_impl(string server, int port, recvcb_interface* protocol_iface, syncword_info* syncinfo)
    :netsender()
     ,m_server(server)
     ,m_port(port)
     ,m_protocol_iface(protocol_iface)
     ,m_socket(-1)
     ,m_bexit(true)
{
    //如果没有填同步信息,则自己造一个出来.
    m_syncword_info.reset(syncinfo);

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

int netsender_base_impl::send_buf(std::string str, const SOCKETINFO* socketinfo)
{
    return send_buf(str.c_str(), str.size(), socketinfo);
}

int netsender_base_impl::send_buf(const char* data, int len, const SOCKETINFO* socketinfo)
{
    m_vecSend.reserve(BUFFER_SIZE);
    m_vecSend.erase(m_vecSend.begin(), m_vecSend.end());

    //如果没有设置帧头,增加自定义的帧头.
    if(m_syncword_info == nullptr)
    {
	m_msghead.syncword[0] = SYNC1;
	m_msghead.syncword[1] = SYNC2;
	m_msghead.msg_len = len;
	m_vecSend.insert(m_vecSend.end(), (char*)&m_msghead, (char*)&m_msghead + sizeof(m_msghead));
    }

    m_vecSend.insert(m_vecSend.end(), data, data + len);

//    debug_tool::print_data(&m_vecSend[0], m_vecSend.size());

    return send_internal(socketinfo);
}

int netsender_base_impl::call_callback(char* data, int len, const SOCKETINFO& socketinfo)
{
    shared_ptr<recvcb_interface::recv_packet> packet;

    //如果上层协议没有设置帧头,使用的是自己增加的帧头,需要去掉.
    if(m_syncword_info == nullptr)
    {
	packet.reset(new recvcb_interface::recv_packet(data + m_constsyncinfo.m_len_head, len - m_constsyncinfo.m_len_head, socketinfo));
    }
    else
    {
	packet.reset(new recvcb_interface::recv_packet(data, len, socketinfo));
    }

    m_protocol_iface->recv_data(packet);

    return len;
}

//void netsender_base_impl::thread_recv(netsender_base_impl* args, SOCKETINFO& socket_communicate)
void netsender_base_impl::thread_recv(void* args, SOCKETINFO& socket_communicate)
{
    netsender_base_impl* p = (netsender_base_impl*)args;
    p->thread_recv_proc(socket_communicate);
}

void netsender_base_impl::thread_recv_proc(SOCKETINFO socketinfo)
{
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, sizeof(buffer));

    MSGHEAD* p = (MSGHEAD*)buffer;
    char* pData = buffer + sizeof(MSGHEAD);

    bool bconnect_break = false;

    int bytesRead;

    //真实的数据长度.
    int data_len;

    while(!m_bexit)
    {
	bytesRead = recv_net_packet(buffer, BUFFER_SIZE, socketinfo);

	if(bytesRead == RECV_NET_TIMEOUT)
	{
	    continue;
	} else if(bytesRead == RECV_NET_BROKEN)
	{
	    bconnect_break = true;
	    break;
	}

	call_callback(buffer, bytesRead, socketinfo);

        // 清空缓冲区
        std::memset(buffer, 0, sizeof(buffer));
    }

    // 客户端连接断开或出错，关闭套接字
    close_communicate_socket(socketinfo);
}
