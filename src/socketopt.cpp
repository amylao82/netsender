
#include "socketopt.h"
#include <algorithm>

socketopt::socketopt()
{
}

socketopt::~socketopt()
{
    m_vector.clear();
}

void socketopt::set_opt_value(int level, int opt_name, const void* data, int len)
{
    opt_item item(level, opt_name, data, len);

    m_vector.push_back(item);

}

void socketopt::set_opt_value(int level, int opt_name, int value)
{
    set_opt_value(level, opt_name, (const void*)&value, sizeof(value));
}


void socketopt::set_socket_option(int socket)
{
    vector<opt_item>::iterator it;

    for(it = m_vector.begin(); it != m_vector.end(); ++it)
    {
	setsockopt(socket, it->m_level, it->m_opt_name, it->m_data, it->m_len);
    }
}

