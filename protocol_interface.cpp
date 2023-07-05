
#include "protocol_interface.h"
#include <iostream>
#include <netinet/in.h>

#include "netsender.h"

void protocol_interface::recv_data(const SOCKETINFO& socket, void* data, int len)
{
    printf("recv_data len = %d, data = %s\n", len, data);
    send_data(socket, data, len);
}

int protocol_interface::send_data(const SOCKETINFO& socket, void* data, int len)
{
    printf("do send data, len = %d, data = %s\n", len, data);
//    send(socket, data, len, 0);
    m_netsender->send_buf((const char*)data, len, &socket);

    return len;
}


