
#include "iface_route.h"

iface_route::iface_route()
{
    init();
}

iface_route::~iface_route()
{
}

uint32_t iface_route::get_route(string eth_name)
{
    vector<ROUTE_INFO>::iterator it;

    for(it = m_route.begin(); it != m_route.end(); ++it)
    {
	if(it->str_eth_name == eth_name)
	    return it->gateway;
    }

    return 0;
}

uint32_t iface_route::get_mask(string eth_name)
{
    vector<ROUTE_INFO>::iterator it;

    for(it = m_route.begin(); it != m_route.end(); ++it)
    {
	if(it->str_eth_name == eth_name)
	    return it->mask;
    }

    return 0;
}

void iface_route::init()
{
    char name[100];
    uint32_t destination, gateway, flags, refcnt, use, metric, mask, mtu, window, irtt;

    FILE *fp = fopen("/proc/net/route" , "r");
    if (nullptr == fp)
    {
	return;
    }

    ROUTE_INFO info;

    //读掉第一行.
    char* line = nullptr;
    size_t len, nread;
    nread = getline(&line, &len, fp);

//    printf("%s\n", line);

    while(EOF != fscanf(fp, "%s\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x", name,
    	&destination, &gateway, &flags, &refcnt, &use, &metric, &mask, &mtu, &window, &irtt))
    {
	if(destination != 0)
	    continue;

//	printf("%s\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\n\n", name, 
//		destination, gateway, flags, refcnt, use, metric, mask, mtu, window, irtt);

	info.destination = destination;
	info.gateway = gateway;
	info.mask = mask;
	info.str_eth_name = name;
	m_route.push_back(info);
    }

    fclose(fp);
}


