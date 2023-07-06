
#include <iostream>
 
#include "iface_info.h"
#include "iface_route.h"

iface_info* iface_info::m_instance = nullptr;
iface_info::GC iface_info::myGC;

iface_info* iface_info::get_instance()
{
    if(m_instance == nullptr)
    {
	m_instance = new iface_info();
	m_instance->init();
    }

    return m_instance;
}

iface_info::iface_info()
    :m_default_eth_name("")
{
}

iface_info::~iface_info()
{
    m_ipinfo.clear();
//    printf("iface_info release\n");
}

uint32_t iface_info::get_ip_addr(const string ethcard)
{
    //一个网卡都没有.
    if(0 == m_ipinfo.size())
	return 0;

    string strcard = ethcard;

    //如果传进来的参数为空,而且设置的default网卡也为空.取第一个.
    if(ethcard == "")
    {
	if(m_default_eth_name == "")
	    return m_ipinfo[0].ip;

	strcard = m_default_eth_name;
    }


    vector<IPINFO>::iterator it;
    for(it = m_ipinfo.begin(); it != m_ipinfo.end(); ++it)
    {
	if(it->str_eth_name == strcard)
	    return it->ip;
    }
    //如果没找到, 明显这个网卡没有连网.默认取第一个网卡的地址
    return m_ipinfo[0].ip;
//    return 0;
}

uint32_t iface_info::get_netmask(const string ethcard)
{
    if(ethcard == "")
    {
	return m_ipinfo[0].mask;
    }

    vector<IPINFO>::iterator it;
    for(it = m_ipinfo.begin(); it != m_ipinfo.end(); ++it)
    {
	if(it->str_eth_name == ethcard)
	    return it->mask;
    }
    return 0;
}


void iface_info::set_default_ethcard(const string ethcard)
{
    m_default_eth_name = ethcard;
}

const string iface_info::get_default_ethcard()
{
    if(m_default_eth_name != "")
	return m_default_eth_name;

    return m_ipinfo[0].str_eth_name;
}


bool iface_info::update_ip_info()
{
    m_ipinfo.clear();

    return init();
}

bool iface_info::init()
{
    int sockfd;
    struct ifconf ifconf;
    struct ifreq *ifr;
    char buf[512];//缓冲区

    iface_route route;	//读取路由的类.

    //初始化ifconf
    ifconf.ifc_len =512;
    ifconf.ifc_buf = buf;
    if ((sockfd =socket(AF_INET,SOCK_DGRAM,0))<0)
    {
        perror("socket" );
	return false;
    }
    ioctl(sockfd, SIOCGIFCONF, &ifconf); //获取所有接口信息

    //接下来一个一个的获取IP地址
    ifr = (struct ifreq*)ifconf.ifc_buf;

    int num = ifconf.ifc_len/sizeof (struct ifreq);

    IPINFO node;
    for (int i = 0; i < num; ++i, ++ifr)
    {
	if (ifr->ifr_addr.sa_family != AF_INET)
	{
	    continue;
	}

	node.str_eth_name = string(ifr->ifr_name);
	//get_ethcard_ip(node.str_eth_name, node.ip, node.mask);

	//一个网卡存在多个地址,只要第一个.
	if( eth_is_exist(node.str_eth_name))
	    continue;
	//网卡存在冒号,为别名.
	if(string::npos != node.str_eth_name.find(":"))
	    continue;

	//为回环网卡.
	if(node.str_eth_name == "lo")
	    continue;

	//取IP地址
	{
	    struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr->ifr_addr);
	    node.ip = sin->sin_addr.s_addr;
	}

	//取子网掩码
	{
	    struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr->ifr_netmask);
	    ioctl(sockfd, SIOCGIFNETMASK, ifr);
	    node.mask = sin->sin_addr.s_addr;
	}

	//取MAC地址.
	{
	    ioctl(sockfd, SIOCGIFHWADDR, ifr);
	    memcpy(node.hwaddr, ifr->ifr_hwaddr.sa_data, 6);
	}

	node.gateway = route.get_route(node.str_eth_name);

	m_ipinfo.push_back(node);
    }

    close(sockfd);

    return true;
}

void iface_info::print_ip_info()
{
    vector<IPINFO>::iterator it;
    for(it = m_ipinfo.begin(); it != m_ipinfo.end(); ++it)
    {
	printf(" %s \n\tip = %s\n", it->str_eth_name.c_str(), ip2string(it->ip).c_str());
	printf("\tmask = %s\n", ip2string(it->mask).c_str());
	printf("\tgateway = %s\n", ip2string(it->gateway).c_str());
        printf("\thwaddr = %02x:%02x:%02x:%02x:%02x:%02x\n",
            (unsigned char)it->hwaddr[0],
            (unsigned char)it->hwaddr[1],
            (unsigned char)it->hwaddr[2],
            (unsigned char)it->hwaddr[3],
            (unsigned char)it->hwaddr[4],
            (unsigned char)it->hwaddr[5]);
    }
}

string iface_info::ip2string(uint32_t ip)
{
    struct in_addr addr;
    addr.s_addr = ip;
    return inet_ntoa(addr);
}

bool iface_info::eth_is_exist(const string ethname)
{
    vector<IPINFO>::iterator it;
    for(it = m_ipinfo.begin(); it != m_ipinfo.end(); ++it)
    {
	if(it->str_eth_name == ethname)
	    return true;
    }

    return false;
}


int iface_info::get_iface_count()
{
    return m_ipinfo.size();
}


string iface_info::get_iface_name(int idx)
{
    if(idx > m_ipinfo.size())
	return "";

    return m_ipinfo[idx].str_eth_name;
}

