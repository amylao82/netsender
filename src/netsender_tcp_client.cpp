
#include "netsender_tcp_client.h"

netsender_tcp_client::netsender_tcp_client(string server, int port, protocol_interface* protocol)
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

int netsender_tcp_client::send_buf(std::string str, const SOCKETINFO* socketinfo)
{
    return send_buf(str.c_str(), str.size(), socketinfo);
}

int netsender_tcp_client::send_buf(const char* data, int len, const SOCKETINFO* socketinfo)
{
    //只能往服务器上发.
    send(m_socket, data,len, 0);
    return len;
}

void netsender_tcp_client::thread_recv(netsender_tcp_client* args)
{
    args->thread_recv_proc();
}

void netsender_tcp_client::thread_recv_proc()
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    SOCKETINFO socketinfo;
    socketinfo.tcp.socket = m_socket;

    while(!m_exit)
    {
        int bytesRead = recv(m_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            // 客户端连接断开或出错，关闭套接字
            close(m_socket);
            break;
        }

        // 处理客户端消息...
	m_protocol_iface->recv_data(buffer, bytesRead, socketinfo);

        // 清空缓冲区
        std::memset(buffer, 0, sizeof(buffer));
    }
}


void netsender_tcp_client::stop_recv_thread()
{
    m_exit = true;
}
