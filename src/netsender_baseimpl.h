
#ifndef _NETSENDER_BASE_IMPL_H_
#define _NETSENDER_BASE_IMPL_H_

#include "netsender.h"

class netsender_base_impl: public netsender
{
    protected:
	//用于读取流时帧分割.
	typedef struct _MSGHEAD {
	    char syncword[2];
	    uint16_t msg_len;
	} MSGHEAD;

	//二个同步字.
	const static char SYNC1 = 'A';
	const static char SYNC2 = 'M';

	MSGHEAD m_msghead;

	constexpr static int BUFFER_SIZE = 1536;

    public:
	netsender_base_impl(string server, int port, protocol_interface* protocol_iface);

	virtual ~netsender_base_impl();

	//从netsender继承过来的接口.
	virtual bool disconnect();

	virtual int send_buf(std::string str, const SOCKETINFO* socketinfo = nullptr);
	virtual int send_buf(const char* data, int len, const SOCKETINFO* socketinfo = nullptr);

	virtual bool init(socketopt* opt) = 0;


	//内部发送函数.加上同步字后发送,子类必须实现
	virtual int send_internal(const SOCKETINFO* socketinfo = nullptr) = 0;
    protected:

	//type可以取值SOCK_STREAM/SOCK_DGRAM
	bool open_socket(int type);
	bool close_socket();

	void set_socket_option(socketopt* opt);
	//从网络地址或者域名里得到in_addr_t信息
	in_addr_t get_net_addr(string str_dest);

    protected:
	string m_server;
	int m_port;
	protocol_interface* m_protocol_iface;
//	socketopt* opt;

	int m_socket;

	//用来组帧往外发.
	vector<char> m_vecSend;
};

#endif
