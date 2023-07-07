
#include "netsender_udp.h"

#define SERVER_PORT 8003 
#define BUFF_LEN 1024

netsender_udp::netsender_udp(NETSENDER_TYPE type, string server, int port, protocol_interface* protocol_iface)
    :netsender_base_impl(server, port, protocol_iface)
    , m_type(type)
//    , m_port(port)
    , m_broadcast(false)
    ,  m_connectOK(false)
//    , m_socket(-1)
    , m_socket_mode_block_io(true)
     , m_bexit(true)
{
}

netsender_udp::~netsender_udp()
{
    stop_recv_thread();
    disconnect();
}

void netsender_udp::set_broadcast(bool broadcast)
{
    m_broadcast = broadcast;
}

bool netsender_udp::isConnect()
{
    return m_connectOK;
}

bool netsender_udp::init(socketopt* opt)
{
    if(m_type == NETSENDER_TYPE::TYPE_SERVER)
	return initServer(opt);
    else
	return connectServer(m_server, opt);

}

//对于客户端,只能往服务器上发.
//int netsender_udp::send_buf(std::string str)
//{
//    return send_buf(str.c_str(), str.length());
//}

//对于服务端,需要指定往哪一个客户端发送数据.
int netsender_udp::send_buf(std::string str, const SOCKETINFO* socketinfo)
{
    return send_buf(str.c_str(), str.length(), socketinfo);
}

//int netsender_udp::send_buf(const char* data, int len)
//{
//    sendto(m_socket, data, len, 0, &m_svrSockAddr, sizeof(m_svrSockAddr));
//    return 1;
//}

//对于服务端,需要指定往哪一个客户端发送数据.
int netsender_udp::send_buf(const char* data, int len, const SOCKETINFO* socketinfo_in)
{
    const SOCKETINFO* socketinfo = socketinfo_in;
    if(socketinfo == nullptr)
	socketinfo = &m_svrSockAddr;

    sendto(m_socket, data, len, 0, &socketinfo->udp.socket, sizeof(*socketinfo));
    return len;
}

bool netsender_udp::initServer(socketopt* opt)
{
    int bRet;
    struct sockaddr_in* ser_addr = (struct sockaddr_in*)&m_svrSockAddr;

    if(false == create_socket(opt))
	return false;

    memset(ser_addr, 0, sizeof(&ser_addr));
    ser_addr->sin_family = AF_INET;
    ser_addr->sin_addr.s_addr = htonl(INADDR_ANY); //IP地址，需要进行网络序转换，INADDR_ANY：本地地址
						   //设置为广播地址.
    ser_addr->sin_port = htons(m_port);  //端口号，需要网络序转换

    //如果为广播,需要设置socket属性.addr也需要修改
    if(m_broadcast)
    {
	set_socket_broadcast();
	ser_addr->sin_addr.s_addr = htonl(INADDR_BROADCAST); //套接字地址为广播地址
    }

    bRet = bind(m_socket, &m_svrSockAddr.udp.socket, sizeof(m_svrSockAddr));
    if(bRet < 0)
    {
	printf("socket bind fail!\n");
	return false;
    }


    bRet = create_recv_thread();

    if(bRet == false)
    {
	printf("create_recv_thread fail!\n");
	return false;
    }

    m_connectOK = true;

    return true;
}

bool netsender_udp::connectServer(std::string strServer, socketopt* opt)
{
    bool bRet;
    struct sockaddr_in* ser_addr = (struct sockaddr_in*)&m_svrSockAddr;

    if(false == create_socket(opt))
	return false;

    //如果传进来的是一个域名,需要通过域名获取一个host.
    struct hostent *host = gethostbyname(strServer.c_str());

    memset(ser_addr, 0, sizeof(struct sockaddr_in));
    ser_addr->sin_family = AF_INET;

    if(host != nullptr)
	ser_addr->sin_addr.s_addr = inet_addr(inet_ntoa( *(struct in_addr*)host->h_addr_list[0] ));
    else
	ser_addr->sin_addr.s_addr = inet_addr(strServer.c_str());

    ser_addr->sin_port = htons(m_port);  //注意网络序转换

    //如果为广播,需要设置socket属性.addr也需要修改
    if(m_broadcast)
    {
	set_socket_broadcast();
	ser_addr->sin_addr.s_addr = htonl(INADDR_BROADCAST); //套接字地址为广播地址
    }

    //==============================
    bRet = create_recv_thread();

    if(bRet == false)
    {
	printf("create_recv_thread fail!\n");
	return false;
    }

    m_connectOK = true;

    return true;
}

void netsender_udp::thread_recv(void* args)
{
    netsender_udp* p = (netsender_udp*)args;
    p->thread_recv_proc();
}

bool netsender_udp::create_recv_thread()
{
    m_bexit = false;
    //pthread_create(&m_pidRecv, nullptr, &threadReceive, this);
//    m_thread_recv.reset(new std::thread([this]
//		{
//		threadReceiveProc();
//		}));

    thread th(thread_recv, this);
    th.detach();

    return true;
}

void netsender_udp::stop_recv_thread()
{
    m_bexit = true;

    //关闭读写通道,否则无法退出recvfrom的调用线程.
#ifdef PLATFORM_WINDOWS
    shutdown(m_socket, SD_BOTH);
#else
    shutdown(m_socket, SHUT_RDWR);
#endif

//    m_thread_recv->join();
//    m_thread_recv.reset();
}

void netsender_udp::set_socket_timeout(int timeout_sec)
{
#ifdef PLATFORM_WINDOWS
    int timeout = timeout_sec * 1000;
#else
    struct timeval timeout;
    timeout.tv_sec = timeout_sec;//秒
    timeout.tv_usec = 0;//微秒
#endif

    if (-1 == setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)))
    {
	cout << "setsockopt SO_RCVTIMEO failed:" << endl;
    }
    if (-1 == setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)))
    {
	cout << "setsockopt SO_RCVTIMEO failed:" << endl;
    }

    m_socket_mode_block_io = false;
}


void netsender_udp::thread_recv_proc()
{
    char buf[BUFF_LEN];  //接收缓冲区，1024字节
    socklen_t len;
    int count;
    struct sockaddr_in client_addr;  //client_addr用于记录发送方的地址信息

    while(!m_bexit)
    {
	memset(buf, 0, BUFF_LEN);
	len = sizeof(client_addr);
	//这个要想办法在退出时结束一下.暂时先不考虑.
	//使用shutdown结束.
	count = recvfrom(m_socket, buf, BUFF_LEN, 0, (struct sockaddr*)&client_addr, &len);  //recvfrom是阻塞函数，没有数据就一直阻塞

	//windows平台因为使用的是超时机制，在超时时会返回-1。
	if(count == -1)
	{
	    if(m_socket_mode_block_io)
	    {
		printf("receive data fail!\n");
	    }
	    else
	    {
		continue;
	    }
	}

	//如果接收到的长度为0.说明是使用shutdown函数结束时退出recvfrom.
	if(count == 0)
	    continue;

	m_protocol_iface->recv_data((const SOCKETINFO&)client_addr, buf, len);
    }

    //printf("end thread receive\n");

}

bool netsender_udp::disconnect()
{
    //cout << "netsender_udp disconnect " << endl;
    if(m_socket >= 0)
    {
#ifdef PLATFORM_WINDOWS
	closesocket(m_socket);
#else
	close(m_socket);
#endif
	m_socket = -1;
    }
    //cout << "netsender_udp disconnect end" << endl;

    return true;
}

bool netsender_udp::create_socket(socketopt* opt)
{
    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(m_socket < 0)
    {
	printf("create socket fail!\n");
	return false;
    }

    if(opt != nullptr)
	opt->set_socket_option(m_socket);

#ifdef PLATFORM_WINDOWS
    //在windows平台下,不能使用shutdown(m_socket, SHUT_RDWR);来结束阻塞recvfrom调用.只能使用非阻塞socket来工作.
    //但在linux下,可以使用阻塞recvfrom来读数据,减少资源使用.
    set_socket_timeout(3);
#endif

    return true;
}

bool netsender_udp::set_socket_broadcast()
{
    int opt=-1;
    int nb=0;
    nb = setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));
    if(nb == -1)
    {
	std::cout<<"set socket errror..."<<std::endl;
	return false;
    }

    return true;
}


