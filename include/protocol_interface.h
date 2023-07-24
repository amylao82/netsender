
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

using namespace std;

class netsender;

class protocol_interface: public recvcb_interface
{

    public:
	virtual ~protocol_interface()
	{
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


	void set_netsender(shared_ptr<netsender> sender)
	{
	    m_netsender = sender;
	}

    private:
	shared_ptr<netsender> m_netsender;
};

#endif

