
#include "netsender_tcp_client.h"

netsender_tcp_client::netsender_tcp_client(string server, int port, recvcb_interface* protocol, syncword_info* syncinfo)
    :netsender_base_impl(server, port, protocol, syncinfo)
{
}


netsender_tcp_client::~netsender_tcp_client()
{
    stop_recv_thread();
    disconnect();
}

bool netsender_tcp_client::init(socketopt* opt)
{
    struct sockaddr_in ser_addr {};

    bool ret;

    // 创建客户端套接字
    ret = open_socket(SOCK_STREAM);
    if (false == ret)
    {
	return false;
    }

    set_socket_option(opt);

    ser_addr.sin_addr.s_addr = get_net_addr(m_server);

    // 准备服务器地址和端口
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(m_port);

    // 连接到服务器
    if (connect(m_socket, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0) {
        std::cerr << "连接失败" << std::endl;
        return false;
    }

    m_bexit = false;

    //thread th(thread_recv, this);

    thread th([this]() {
	    SOCKETINFO socketinfo;
	    socketinfo.tcp.socket = this->m_socket;
	    //netsender_base_impl::thread_recv(this, socketinfo);
	    thread_recv_proc(socketinfo);
    });
    th.detach();

    return true;
}

bool netsender_tcp_client::isConnect()
{
    return m_socket != -1;
}

int netsender_tcp_client::send_internal(const SOCKETINFO* socketinfo)
{
    //只能往服务器上发.
    send(m_socket, &m_vecSend[0], m_vecSend.size(), 0);
    return m_vecSend.size();
}

void netsender_tcp_client::stop_recv_thread()
{
    m_bexit = true;
}

int netsender_tcp_client::recv_net_packet(char* buffer, int buf_len, SOCKETINFO& socketinfo)
{
    int len_ret = 0;
    int bytesRead;
    int idxRead = 0;

    const syncword_info* psync = m_syncword_info.get();

    if(psync == nullptr)
	psync = &m_constsyncinfo;

    char* pData = buffer + psync->m_len_head;
    //需要读取的数据长度.
    int msg_len;
    int len_read = 0;	//已经读取到的长度.

    //读出同步字.
    while(idxRead < psync->m_len_field_sync_word)
    {
	bytesRead = recv(socketinfo.tcp.socket, &buffer[idxRead], 1, 0);
	if(bytesRead <= 0)
	    return RECV_NET_BROKEN;

	if(buffer[idxRead] == psync->m_sync_word[idxRead])
	    ++idxRead;
    }

    //读长度字段
    bytesRead = recv(socketinfo.tcp.socket, &buffer[idxRead], psync->m_len_field_packetsize, 0);
    if(bytesRead <= 0)
	return RECV_NET_BROKEN;

    len_ret += psync->m_len_head;

    //如果需要转换字节序.需要交换才能知道后面的帧数据长度.
    if(psync->m_network_order_byte)
    {
	//这里认为数据长度字段只有2字节和4字节.可能并不能满足所有场景.
	if(psync->m_len_field_packetsize == 2)
	{
	    msg_len = protocol_general::htons(&buffer[idxRead]);
	    *(uint16_t*)&buffer[idxRead] = *(uint16_t*)&msg_len;
	}
	else if(psync->m_len_field_packetsize == 4)
	{
	    msg_len = protocol_general::htonl(&buffer[idxRead]);
	    *(uint32_t*)&buffer[idxRead] = protocol_general::htonl(&buffer[idxRead]);
	}
    }
    else
    {
	if(psync->m_len_field_packetsize == 2)
	{
	    msg_len = *(uint16_t*)&buffer[idxRead];
	}
	else if(psync->m_len_field_packetsize == 4)
	{
	    msg_len = *(uint32_t*)&buffer[idxRead];
	}
    }

//    printf("msg_len = %d\n", msg_len);
//    assert(msg_len > 0);
    if(msg_len <= 0)
    {
	//读到的长度为0,应该是出错了.
	return 0;
    }

    //如果没有设置帧头.是自己增加的帧头,把接下来接收到的整个数据返回上层.
//    if(m_syncword_info == nullptr)
//    {
//	pData = buffer;
//	len_ret = msg_len;
//    }
//    else 
//    {
//	len_ret += msg_len;
//    }


    len_ret += msg_len;

    while(len_read != msg_len)
    {
	bytesRead = recv(socketinfo.tcp.socket, pData + len_read, msg_len - len_read , 0);
	if(bytesRead <= 0)
	    return RECV_NET_BROKEN;

	len_read += bytesRead;
    }


//    if(m_syncword_info == nullptr)
//    {
//	len_ret -= m_constsyncinfo.m_len_head;
//    }

//    debug_tool::print_data(buffer, len_ret, "tcp_client");

    return len_ret;
}

void netsender_tcp_client::close_communicate_socket(SOCKETINFO& socketinfo)
{
    close(socketinfo.tcp.socket);
}
