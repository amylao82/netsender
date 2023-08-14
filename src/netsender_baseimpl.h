
#ifndef _NETSENDER_BASE_IMPL_H_
#define _NETSENDER_BASE_IMPL_H_

#include <cassert>

#include "netsender.h"

#include "protocol_general.h"

class netsender_base_impl: public netsender
{
    protected:
	//从网络接收函数recv_net_packet可能返回的错误值.
	enum {
	    RECV_NET_TIMEOUT = -1,
	    RECV_NET_BROKEN = -2
	};

    protected:
	//用于读取流时帧分割.
	typedef struct _MSGHEAD {
	    char syncword[2];
	    uint16_t msg_len;
	} MSGHEAD;

	//二个同步字.
	const static char SYNC1 = 'A';
	const static char SYNC2 = 'M';

	static const syncword_info m_constsyncinfo;

	MSGHEAD m_msghead;

	constexpr static int BUFFER_SIZE = 1536;

    public:
	netsender_base_impl(string server, int port, recvcb_interface* protocol_iface, syncword_info* sync_info = nullptr);

	virtual ~netsender_base_impl();

	//从netsender继承过来的接口.
	virtual bool disconnect();

	virtual int send_buf(std::string str, const SOCKETINFO* socketinfo = nullptr);
	virtual int send_buf(const char* data, int len, const SOCKETINFO* socketinfo = nullptr);

	//需要子类实现的函数.
	virtual bool init(socketopt* opt) = 0;

    protected:
	//内部发送函数.加上同步字后发送,子类必须实现
	virtual int send_internal(const SOCKETINFO* socketinfo = nullptr) = 0;

	//接收一包数据.子类必须实现.
	//在udp里,使用的是recvfrom. socketinfo返回的是通信端的IP信息
	//在tcp里,使用的是recv.socketinfo是传进去的socket,里面需要用这个socket来接收数据.
	//返回接收到的数据长度,如果出错,返回负数.
	virtual int recv_net_packet(char* buf, int buf_len, SOCKETINFO& socketinfo) = 0;
	//使用TCP和使用UDP,调用的close_socket并不一样.需要子类实现.UDP不用关闭.
	virtual void close_communicate_socket(SOCKETINFO& socketinfo) = 0;

    protected:

	//调用回调函数.
	int call_callback(char* data, int len, const SOCKETINFO& socketinfo);

	//type可以取值SOCK_STREAM/SOCK_DGRAM
	bool open_socket(int type);

	//这个函数命名不好,因为关闭socket的函数,在linux下,叫close;在windows下,叫closesocket.
	//这个名字与windows下的名字太过相似,会让人无所适从.
	//后期要整理.
	bool close_socket(int& socket);

	void set_socket_option(socketopt* opt);
	//从网络地址或者域名里得到in_addr_t信息
	//in_addr_t get_net_addr(string str_dest);
	uint32_t get_net_addr(string str_dest);


	//static void thread_recv(netsender_base_impl* args, SOCKETINFO& socket_communicate);
	static void thread_recv(void* args, SOCKETINFO& socket_communicate);
	void thread_recv_proc(SOCKETINFO socketinfo);

    protected:
	string m_server;
	int m_port;
	recvcb_interface* m_protocol_iface;
//	socketopt* opt;

	int m_socket;

	bool m_bexit;

	//用来组帧往外发.
	vector<char> m_vecSend;

//	syncword_info m_syncword_info;

	shared_ptr<syncword_info> m_syncword_info;
};

#endif
