
// =========================================
// 用于TCP协议从流式数据读取帧时方便分包.
// 这个类包含数据帧头的同步字,同步字的位数,包长度的位数.
// 用于TCP读取时分包.
//
// 2023.07.25
//
// =========================================

#ifndef _SYNCWORD_INFO_H_
#define _SYNCWORD_INFO_H_

#include <string>
#include <cstdint>

using namespace std;

class syncword_info
{
    public:
	syncword_info(string sync_word, uint16_t len_sync_word,uint16_t len_packetsize, bool network_order_byte = false)
	    :m_sync_word(sync_word)
	     ,m_len_field_sync_word(len_sync_word)
	     ,m_len_field_packetsize(len_packetsize)
    		,m_network_order_byte(network_order_byte) {
		m_len_head = m_len_field_sync_word + m_len_field_packetsize;
		};

	//默认构造函数,使用自己定义的头.
	syncword_info()
	    :m_sync_word("AM")
	     ,m_len_field_sync_word(2)
	     ,m_len_field_packetsize(2)
    		,m_network_order_byte(false) {
		m_len_head = m_len_field_sync_word + m_len_field_packetsize;
		};

	bool operator==(syncword_info& rhs) const {
	    if(m_sync_word != rhs.m_sync_word)
		return false;
	    if(m_len_field_sync_word != rhs.m_len_field_sync_word)
		return false;
	    if(m_len_field_packetsize != rhs.m_len_field_packetsize)
		return false;
	    if(m_network_order_byte != rhs.m_network_order_byte)
		return false;

	    return true;
	}

	string m_sync_word;
	uint16_t m_len_field_sync_word;
	uint16_t m_len_field_packetsize;
	bool m_network_order_byte;	//是否网络字节序.

	//通过配置,计算出这个同步头的长度.
	uint16_t m_len_head;
};

#endif

