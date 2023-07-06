﻿
#include "netsender.h"
#include "netsender_udp.h"
#include "netsender_tcp_server.h"
#include "netsender_tcp_client.h"

netsender* netsender::createSender(netsender::PROTOCOL_TYPE protocol, netsender::NETSENDER_TYPE type, std::string connectServer, int port, protocol_interface* protocol_iface)
{
    bool ret;
    switch(protocol)
    {
	case netsender::PROTOCOL_UDP:
	    {
		netsender_udp* pSender = new netsender_udp(type, port, protocol_iface);
		//如果IP地址为广播地址,需要设置广播属性.
		if(connectServer == "255.255.255.255")
		{
		    pSender->set_broadcast(true);
		}

//		if(protocol_iface == nullptr)
//		{
//		    protocol_iface = shared_ptr<protocol_interface>(new protocol_interface());
//		}
		//sender与protocol实例联结
//		pSender->m_protocol_iface = protocol_iface;
		protocol_iface->set_netsender(pSender);

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
	    {
		if(type == NETSENDER_TYPE::TYPE_SERVER)
		{
		    netsender_tcp_server* sender = new netsender_tcp_server(connectServer, port, protocol_iface);
		    if(false == sender->init())
		    {
			delete sender;
			return nullptr;
		    }

		    protocol_iface->set_netsender(sender);

		    return sender;
		}
		else
		{
		    netsender_tcp_client* sender = new netsender_tcp_client(connectServer, port, protocol_iface);

		    if(false == sender->init())
		    {
			delete sender;
			return nullptr;
		    }

		    protocol_iface->set_netsender(sender);

		    return sender;
		}
	    }
	case netsender::PROTOCOL_WEBSOCKET:
	case netsender::PROTOCOL_HTTP:
	default:
	    //TODO: add new protocol,not support now.
	    return nullptr;
    }
}


netsender::netsender()
    : m_protocol_iface(nullptr)
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

    m_protocol_iface = nullptr;
}

