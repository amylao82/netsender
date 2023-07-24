
// =======================================
// 数据处理基类.
// 通过与netsender协同工作,处理所有数据相关的解释处理
// amy
// 2023.07.05
//
// =======================================

#ifndef _PROTOCOL_INTERFACE_H_
#define _PROTOCOL_INTERFACE_H_

#include "protocol_general.h"
#include "recvcb_interface.h"

#include "netsender.h"

using namespace std;

//class netsender;

class protocol_interface: public recvcb_interface
{

    public:
	protocol_interface() : m_netsender(nullptr) {};

	virtual ~protocol_interface()
	{
	    if(m_netsender != nullptr)
	    {
		m_netsender.reset();
		m_netsender = nullptr;
	    }
	};

	//接收数据接口
//	virtual void recv_data(char* data, int len, const SOCKETINFO& socket);
//	virtual void recv_data(shared_ptr<recv_packet> packet);


	//发送函数提供继承,内部可能要增加帧头.
	virtual int send_data(const char* data, int len);
	virtual int send_data(string str);
	//只有服务器端回应客户时,才会有socket参数.
	virtual int send_data(const char* data, int len, const SOCKETINFO& socket);
	virtual int send_data(string str, const SOCKETINFO& socket);

	//创建一个网络发送者.与后面的set_netsender只选一个.
//	virtual bool create_sender() = 0;

	virtual bool create_sender(netsender::PROTOCOL_TYPE protocol	//使用的连接协议
		, netsender::NETSENDER_TYPE type	//服务器还是客户端
		, std::string connectServer		//要连接的服务器地址,
		, int port				//使用的端口
		, socketopt* opt = nullptr);		//对socket的额外设置

//	void set_netsender(shared_ptr<netsender> sender)
//	{
//	    m_netsender = sender;
//	}

    protected:
	shared_ptr<netsender> m_netsender;
};

#endif

