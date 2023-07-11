
#include "protocol_interface.h"
#include <iostream>
#include <netinet/in.h>

#include "netsender.h"

void protocol_interface::recv_data(void* data, int len, const SOCKETINFO& socket)
{
    printf("recv_data len = %d, data = %s\n", len, data);
    send_data(data, len, socket);
}

int protocol_interface::send_data(const void* data, int len, const SOCKETINFO& socket)
{
//    printf("do send data, len = %d, data = %s\n", len, data);
    if(m_netsender != nullptr)
	m_netsender->send_buf((const char*)data, len, &socket);

    return len;
}

int protocol_interface::send_data(const void* data, int len)
{
    if(m_netsender != nullptr)
	m_netsender->send_buf((const char*)data, len);

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


