
#include "netsender_tcp_server.h"

netsender_tcp_server::netsender_tcp_server(string server, int port, recvcb_interface* protocol, syncword_info* syncinfo)
    :netsender_base_impl(server, port, protocol, syncinfo)
{
}

netsender_tcp_server::~netsender_tcp_server()
{
    stop_recv_thread();
    disconnect();
}

bool netsender_tcp_server::init(socketopt* opt)
{
    int iret;
    struct sockaddr_in* ser_addr = (struct sockaddr_in*)&m_svrSockAddr;

    bool ret;
    ret = open_socket(SOCK_STREAM);
    if (false == ret)
    {
	return false;
    }

    //设置socket的参数
    set_socket_option(opt);

    memset(ser_addr, 0, sizeof(&ser_addr));
    ser_addr->sin_family = AF_INET;
    ser_addr->sin_addr.s_addr = htonl(INADDR_ANY); //IP地址，需要进行网络序转换，INADDR_ANY：本地地址
    ser_addr->sin_port = htons(m_port);  //端口号，需要网络序转换

    iret = bind(m_socket, &m_svrSockAddr, sizeof(m_svrSockAddr));
    if(iret < 0)
    {
	printf("socket bind fail!\n");
	return false;
    }
    //
    // 开始监听连接
    if (listen(m_socket, 5) == -1) {
        close(m_socket);
	printf("listen error!\n");
        return false;
    }

    m_bexit = false;
    thread th(thread_accept, this);
    th.detach();

    return true;
}

void netsender_tcp_server::thread_accept(netsender_tcp_server* args)
{
    args->thread_accept_proc();
}

void netsender_tcp_server::thread_accept_proc()
{
    //TODO: 接收客户端的连接,并创建对应的数据处理线程.

    while(!m_bexit)
    {
        sockaddr_in clientAddress{};
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(m_socket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressSize);
        if (clientSocket == -1) {
            continue;
        }

//	SOCKETINFO socketinfo;
//	socketinfo.tcp.socket = clientSocket;
	//thread th(thread_client, this, socketinfo);
	thread th([=]() {
		SOCKETINFO socketinfo;
		socketinfo.tcp.socket = clientSocket;
		//netsender_base_impl::thread_recv(this, socketinfo);
		thread_recv_proc(socketinfo);
		});
	th.detach();
    }
}

//void netsender_tcp_server::thread_client(netsender_tcp_server* args, SOCKETINFO client_socket)
//{
//    args->thread_client_proc(client_socket);
//}
//
//void netsender_tcp_server::thread_client_proc(SOCKETINFO client_socket)
//{
//    char buffer[BUFFER_SIZE];
//    std::memset(buffer, 0, sizeof(buffer));
//
//    MSGHEAD* p = (MSGHEAD*)buffer;
//    char* pData = buffer + sizeof(MSGHEAD);
//
//    int bytesRead;
//    while(!m_bexit)
//    {
//	//取同步字
//        bytesRead = recv(client_socket.tcp.socket, &buffer[0], 1, 0);
//        if (bytesRead <= 0) {
//            break;
//        }
//	if(buffer[0] != SYNC1)
//	    continue;
//
//        bytesRead = recv(client_socket.tcp.socket, &buffer[1], 1, 0);
//        if (bytesRead <= 0) {
//            break;
//        }
//	if(buffer[1] != SYNC2)
//	    continue;
//
//        bytesRead = recv(client_socket.tcp.socket, &buffer[2], 2, 0);
//        if (bytesRead <= 0) {
//            break;
//        }
//
//	//取到的数据长度为0,不处理.
//	if(p->msg_len == 0)
//	    continue;
//
//        int bytesRead = recv(client_socket.tcp.socket, pData, p->msg_len, 0);
//        if (bytesRead <= 0) {
//            break;
//        }
//
//        // 处理客户端消息...
//	call_callback(pData, bytesRead, client_socket);
//
//        // 清空缓冲区
//        std::memset(buffer, 0, sizeof(buffer));
//    }
//
//    // 客户端连接断开或出错，关闭套接字
//    close(client_socket.tcp.socket);
//    cout << " close socket! in handleClient!" << endl;
//}

int netsender_tcp_server::send_internal(const SOCKETINFO* socketinfo)
{
    //如果socketinfo为空指针,是客户端往服务器端发送
    if(socketinfo == nullptr)
	send(m_socket, &m_vecSend[0], m_vecSend.size(), 0);
    else
	send(socketinfo->tcp.socket, &m_vecSend[0], m_vecSend.size(), 0);

    return m_vecSend.size();
}

bool netsender_tcp_server::isConnect()
{
    return true;
}

void netsender_tcp_server::stop_recv_thread()
{
    m_bexit = true;

    //TODO: 这里关闭的只是主socket,而不是针对每个客户socket来关闭,会有问题.
    //下一版把每个客户端设为select/poll模型时修改.
}

int netsender_tcp_server::recv_net_packet(char* buffer, int buf_len, SOCKETINFO& socketinfo)
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

    printf("mgs_len = %d\n", msg_len);
    assert(msg_len > 0);

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
	bytesRead = recv(socketinfo.tcp.socket, pData, msg_len - len_read , 0);
	if(bytesRead <= 0)
	    return RECV_NET_BROKEN;

	len_read += bytesRead;
    }

//    if(m_syncword_info == nullptr)
//    {
//	len_ret -= m_constsyncinfo.m_len_head;
//    }

    return len_ret;
}

void netsender_tcp_server::close_communicate_socket(SOCKETINFO& socketinfo)
{
    close(socketinfo.tcp.socket);
}


