
// =======================================================
//
// 读取系统内的网卡信息.
// 如果系统内存在多个网卡情况下,使用程序读取一个IP地址,可能会读取第一个网卡的地址,也可能读到第二个网卡的地址.
// 这个类提供一个手段,可以指定网卡的名称来读取IP地址.
// 如果不指定网卡名称,则返回第一个网卡的地址.
//
// TODO: 现在不考虑一个网卡有多个IP地址的情况. 
// 	使用ip命令可以对同一个网卡设置多个IP地址,现时只考虑同一网卡的第一个地址.
// 	使用ifconfig eth0:1的方法,也会设置一个别名给网卡.此类IP亦暂不考虑.
//
// Justin
// 2023.05.08
// =======================================================

#ifndef _JUSTIN_IF_INFO_H_
#define _JUSTIN_IF_INFO_H_

#include <string>
#include <cstdint>
#include <vector>
#include <cstring>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>

using namespace std;

class iface_info
{
    typedef struct _IPINFO
    {
	string str_eth_name;
	uint32_t ip;
	uint32_t mask;
	uint32_t gateway;	//网关
	char hwaddr[6];		//MAC地址.
    } IPINFO;

    public:
	~iface_info();
	static iface_info* get_instance();

	//读取网卡的IP地址.
	//如果参数为空字符串,取第一个网卡地址.
	//要修改默认网卡地址,可以使用set_default_ethcard设置一个默认网卡,这样可以通过无参数取得.
	//或者可以在这个函数增加一个网卡名称,指定要取IP地址的网卡.
	uint32_t get_ip_addr(const string ethcard = "");
	uint32_t get_netmask(const string ethcard = "");

	//设置默认网卡名称.
	void set_default_ethcard(const string ethcard);
	const string get_default_ethcard();

	//如果在程序运行的过程中,有新网卡地址变动,需要更新时,由外部调用.
	bool update_ip_info();

	//输出IP地址列表.用于调试.
	void print_ip_info();

	static string ip2string(uint32_t ip);

	int get_iface_count();

	string get_iface_name(int idx);
    protected:
	iface_info();

	bool get_ethcard_ip(const string& ethcard_name, uint32_t& ip, uint32_t& netmask);
	
	bool init();


	//判断网卡是否已经在列表里.
	bool eth_is_exist(const string ethname);


    protected:
	static iface_info* m_instance;

	string m_default_eth_name;

	vector<IPINFO> m_ipinfo;

    private:
	class GC
	{
	    public :
		~GC()
		{
		    // We can destory all the resouce here
		    if (m_instance != nullptr )
		    {
			delete m_instance;
			m_instance = nullptr ;
		    }
		}
	};
	static GC myGC;
};

#endif
