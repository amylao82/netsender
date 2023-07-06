
#include "netsender_tcp_client.h"

netsender_tcp_client::netsender_tcp_client(string server, int port, protocol_interface* protocol)
    :netsender(protocol)
     ,m_server(server)
     ,m_socket(-1)
     ,m_port(port)
     ,m_exit(true)
{
}


netsender_tcp_client::~netsender_tcp_client()
{
    stopRecvThread();
    disconnect();
}


bool netsender_tcp_client::init()
{
    struct sockaddr_in ser_addr {};

    // 创建客户端套接字
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == -1) {
        std::cerr << "无法创建套接字" << std::endl;
        return false;
    }

    //如果传进来的是一个域名,需要通过域名获取一个host.
    struct hostent *host = gethostbyname(m_server.c_str());

    if(host != nullptr)
	ser_addr.sin_addr.s_addr = inet_addr(inet_ntoa( *(struct in_addr*)host->h_addr_list[0] ));
    else
	ser_addr.sin_addr.s_addr = inet_addr(m_server.c_str());

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

bool netsender_tcp_client::disconnect()
{
    if(m_socket == -1)
	return true;

    close(m_socket);
    m_socket = -1;

    return true;
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
	    cout << " close socket! in handleClient!" << endl;
            break;
        }

        // 处理客户端消息...
	m_protocol_iface->recv_data(socketinfo, buffer, bytesRead);

        // 清空缓冲区
        std::memset(buffer, 0, sizeof(buffer));
    }
}


void netsender_tcp_client::stopRecvThread()
{
    m_exit = true;

    //关闭读写通道,否则无法退出recvfrom的调用线程.
#ifdef PLATFORM_WINDOWS
    shutdown(m_socket, SD_BOTH);
#else
    shutdown(m_socket, SHUT_RDWR);
#endif

}
