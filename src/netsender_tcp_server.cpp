
#include "netsender_tcp_server.h"

netsender_tcp_server::netsender_tcp_server(string server, int port, protocol_interface* protocol)
    :netsender_base_impl(server, port, protocol)
     ,m_exit(true)
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

    m_socket = socket(AF_INET, SOCK_STREAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
    if(m_socket < 0)
    {
	printf("create socket fail!\n");
	return false;
    }

    //设置socket的参数
    if(opt != nullptr)
	opt->set_socket_option(m_socket);

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

    m_exit = false;
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

    while(!m_exit)
    {
        sockaddr_in clientAddress{};
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(m_socket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressSize);
        if (clientSocket == -1) {
            continue;
        }

	SOCKETINFO socketinfo;
	socketinfo.tcp.socket = clientSocket;
	thread th(thread_client, this, socketinfo);
	th.detach();
    }
}

void netsender_tcp_server::thread_client(netsender_tcp_server* args, SOCKETINFO client_socket)
{
    args->thread_client_proc(client_socket);
}

void netsender_tcp_server::thread_client_proc(SOCKETINFO client_socket)
{
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, sizeof(buffer));
    while(!m_exit)
    {
        int bytesRead = recv(client_socket.tcp.socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            // 客户端连接断开或出错，关闭套接字
            close(client_socket.tcp.socket);
	    cout << " close socket! in handleClient!" << endl;
            break;
        }

        // 处理客户端消息...
	m_protocol_iface->recv_data(client_socket, buffer, bytesRead);

        // 清空缓冲区
        std::memset(buffer, 0, sizeof(buffer));
    }
}

int netsender_tcp_server::send_buf(std::string str, const SOCKETINFO* socketinfo)
{
    return send_buf(str.c_str(), str.size(), socketinfo);
}

int netsender_tcp_server::send_buf(const char* data, int len, const SOCKETINFO* socketinfo)
{
    //如果socketinfo为空指针,是客户端往服务器端发送
    if(socketinfo == nullptr)
	send(m_socket, data,len, 0);
    else
	send(socketinfo->tcp.socket, data, len, 0);
    return len;
}


bool netsender_tcp_server::disconnect()
{
    if(m_socket >= 0)
    {
	close(m_socket);
	m_socket = -1;
    }

    return true;
}

bool netsender_tcp_server::isConnect()
{
    return true;
}

void netsender_tcp_server::stop_recv_thread()
{
    m_exit = true;

    //关闭读写通道,否则无法退出recvfrom的调用线程.
#ifdef PLATFORM_WINDOWS
    shutdown(m_socket, SD_BOTH);
#else
    shutdown(m_socket, SHUT_RDWR);
#endif

}
