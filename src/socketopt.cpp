
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


