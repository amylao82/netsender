
#include "netsender.h"
#include "udpsender.h"

//shared_ptr<netsender> createSender(netsender::PROTOCOL_TYPE protocol, netsender::NETSENDER_TYPE type, std::string connectServer, int port)
netsender* netsender::createSender(netsender::PROTOCOL_TYPE protocol, netsender::NETSENDER_TYPE type, std::string connectServer, int port)
{
    bool ret;
    switch(protocol)
    {
	case netsender::PROTOCOL_UDP:
	    {
		udpSender* pSender = new udpSender(type, port);
		//如果IP地址为广播地址,需要设置广播属性.
		if(connectServer == "255.255.255.255")
		{
		    pSender->set_broadcast(true);
		}
		if(type == netsender::TYPE_SERVER)
		    ret = pSender->initServer();
		else
		    ret = pSender->connectServer(connectServer);

		//连接不成功,把实例删除.
		if(false == ret)
		{
		    delete pSender;
		    pSender = nullptr;
		}

		return dynamic_cast<netsender*>(pSender);
	    }
	case netsender::PROTOCOL_TCP:
	case netsender::PROTOCOL_WEBSOCKET:
	case netsender::PROTOCOL_HTTP:
	default:
	    //TODO: add new protocol,not support now.
	    return nullptr;
    }
}


netsender::netsender()
    :m_cbFunc(nullptr)
     ,m_cbArg(nullptr)
{
#ifdef PLATFORM_WINDOWS
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Call to WSAStartup failed." << std::endl;
    }
#endif
}

netsender::~netsender()
{
#ifdef PLATFORM_WINDOWS
    WSACleanup();
#endif
}

