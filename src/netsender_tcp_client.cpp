
#include "netsender_tcp_client.h"

netsender_tcp_client::netsender_tcp_client(string server, int port, recvcb_interface* protocol)
    :netsender_base_impl(server, port, protocol)
     ,m_exit(true)
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

    m_exit = false;
    thread th(thread_recv, this);
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

void netsender_tcp_client::thread_recv(netsender_tcp_client* args)
{
    args->thread_recv_proc();
}

void netsender_tcp_client::thread_recv_proc()
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    MSGHEAD* p = (MSGHEAD*)buffer;
    char* pData = buffer + sizeof(MSGHEAD);

    SOCKETINFO socketinfo;
    socketinfo.tcp.socket = m_socket;


    bool bconnect_break = false;

    int bytesRead;

    //真实的数据长度.
    int data_len;

    int drop_cnt = 0;

    while(!m_exit)
    {
	//取同步字
        bytesRead = recv(m_socket, &buffer[0], 1, 0);
        if (bytesRead <= 0) {
	    bconnect_break = true;
            break;
        }
	if(buffer[0] != SYNC1)
	{
	    ++drop_cnt;
	    continue;
	}

        bytesRead = recv(m_socket, &buffer[1], 1, 0);
        if (bytesRead <= 0) {
	    bconnect_break = true;
            break;
        }
	if(buffer[1] != SYNC2)
	{
	    ++drop_cnt;
	    continue;
	}

        bytesRead = recv(m_socket, &buffer[2], 2, 0);
        if (bytesRead <= 0) {
	    bconnect_break = true;
            break;
        }

	//取到的数据长度为0,不处理.
	if(p->msg_len == 0)
	    continue;

	data_len = 0;
	while(data_len != p->msg_len)
	{
	    bytesRead = recv(m_socket, pData + data_len, p->msg_len - data_len, 0);
	    if(bytesRead <= 0)
	    {
		bconnect_break = true;
		break;
	    }
	    data_len += bytesRead;
	}

	if(drop_cnt != 0)
	{
	    printf("\t\tdrop_cnt = %d\n", drop_cnt);
	    drop_cnt = 0;
	}
	//如果是因为断开而到这里,不要把数据往上发.
	if(bconnect_break)
	    break;

	if(p->msg_len != data_len)
	{
	    printf("recv data not enough\n");
	}

        // 处理客户端消息...
	call_callback(pData, data_len, socketinfo);

        // 清空缓冲区
        std::memset(buffer, 0, sizeof(buffer));
    }

    // 客户端连接断开或出错，关闭套接字
    close(m_socket);
}


void netsender_tcp_client::stop_recv_thread()
{
    m_exit = true;
}
