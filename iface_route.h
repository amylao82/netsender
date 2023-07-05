
// =======================================================
//
// 读取系统内的/proc/net/route,从里面拿到每个网卡的gateway信息
//
// Justin
// 2023.05.12
// =======================================================

#ifndef _IFACE_ROUTE_H_
#define _IFACE_ROUTE_H_

#include <string>
#include <vector>
#include <cstdint>

using namespace std;

class iface_route
{
    typedef struct _ROUTE_INFO
    {
	string str_eth_name;
	uint32_t destination;
	uint32_t gateway;
//	uint32_t flags;
	uint32_t mask;
    } ROUTE_INFO;
    public:
	iface_route();
	~iface_route();

	uint32_t get_route(string eth_name);
	uint32_t get_mask(string eth_name);

    protected:
	void init();

    protected:
	vector<ROUTE_INFO> m_route;
};

#endif

