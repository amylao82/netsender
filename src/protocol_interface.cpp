
#include "protocol_interface.h"
#include <iostream>
#include <netinet/in.h>

#include "netsender.h"

int protocol_interface::send_data(const char* data, int len, const SOCKETINFO& socket)
{
//    printf("do send data, len = %d, data = %s\n", len, data);
    if(m_netsender != nullptr)
	m_netsender->send_buf(data, len, &socket);

    return len;
}

int protocol_interface::send_data(const char* data, int len)
{
    if(m_netsender != nullptr)
	m_netsender->send_buf(data, len);

    return len;
}


int protocol_interface::send_data(string str)
{
    return send_data(str.c_str(), str.size());
}

int protocol_interface::send_data(string str, const SOCKETINFO& socket)
{
    return send_data(str.c_str(), str.size(), socket);
}

bool protocol_interface::create_sender(netsender::PROTOCOL_TYPE protocol
	, netsender::NETSENDER_TYPE type
	, std::string connectServer
	, int port
	, syncword_info* syncinfo
	, socketopt* opt)
{
    m_netsender.reset(netsender::createSender(protocol, type, connectServer, port, this, syncinfo, opt));

    if(m_netsender == nullptr)
    {
	cout << " createSender error!" << endl;
	return false;
    }

    //如果是服务器端,不用再处理.
    if(type == netsender::TYPE_SERVER)
	return true;

    bool bRet;
    bRet = m_netsender->isConnect();
//    cout << "isconnect = " << bRet << endl;

    if(bRet != true)
    {
	cout << " connect server return false!" << endl;
	return false;
    }

    return true;
}


