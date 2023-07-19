
#include "netsender_udp.h"

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

int netsender_udp::send_internal(const SOCKETINFO* socketinfo_in)
{
    const SOCKETINFO* socketinfo = socketinfo_in;
    if(socketinfo == nullptr)
	socketinfo = &m_svrSockAddr;

    sendto(m_socket, &m_vecSend[0], m_vecSend.size(), 0, &socketinfo->udp.socket, sizeof(*socketinfo));

    return m_vecSend.size();
}

bool netsender_udp::initServer(socketopt* opt)
{
    int bRet;
    struct sockaddr_in* ser_addr = (struct sockaddr_in*)&m_svrSockAddr;

    bRet = open_socket(SOCK_DGRAM);
    if (false == bRet)
    {
	return false;
    }


    set_socket_option(opt);

#ifdef PLATFORM_WINDOWS
    //在windows平台下,不能使用shutdown(m_socket, SHUT_RDWR);来结束阻塞recvfrom调用.只能使用非阻塞socket来工作.
    //但在linux下,可以使用阻塞recvfrom来读数据,减少资源使用.
    set_socket_timeout(3);
#endif

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


    bRet = open_socket(SOCK_DGRAM);
    if (false == bRet)
    {
	return false;
    }

    set_socket_option(opt);

#ifdef PLATFORM_WINDOWS
    //在windows平台下,不能使用shutdown(m_socket, SHUT_RDWR);来结束阻塞recvfrom调用.只能使用非阻塞socket来工作.
    //但在linux下,可以使用阻塞recvfrom来读数据,减少资源使用.
    set_socket_timeout(3);
#endif

    memset(ser_addr, 0, sizeof(struct sockaddr_in));
    ser_addr->sin_family = AF_INET;

    ser_addr->sin_addr.s_addr = get_net_addr(m_server);

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
    char buf[BUFFER_SIZE];  //接收缓冲区，1024字节
    socklen_t len;
    int recv_count;
    struct sockaddr_in client_addr;  //client_addr用于记录发送方的地址信息

    while(!m_bexit)
    {
	memset(buf, 0, BUFFER_SIZE);
	len = sizeof(client_addr);
	//这个要想办法在退出时结束一下.暂时先不考虑.
	//使用shutdown结束.
	recv_count = recvfrom(m_socket, buf, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &len);  //recvfrom是阻塞函数，没有数据就一直阻塞

	//windows平台因为使用的是超时机制，在超时时会返回-1。
	if(recv_count == -1)
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
	if(recv_count == 0)
	    continue;

	//printf("udp recvfrom len = %d\n", recv_count);
	m_protocol_iface->recv_data(buf + sizeof(MSGHEAD), recv_count - sizeof(MSGHEAD), (const SOCKETINFO&)client_addr);
    }

    //printf("end thread receive\n");
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


