

// ===========================================
//
// 处理协议时经常要用到的函数.
// 整理为统一的基础类,以后不用重写.
// Justin
// 2023.03.34
//
// ===========================================

#ifndef _PROTOCOL_GENERAL_H_
#define _PROTOCOL_GENERAL_H_

#include <cstdint>
#include <cstdbool>

class protocol_general
{
    public:
	static uint32_t htonl(uint8_t* p);
	static uint16_t htons(uint8_t* p);
	static uint32_t htonl(uint32_t val);
	static uint16_t htons(uint16_t val);

	static uint8_t cal_checksum_xor(uint8_t* data, int len);
};

#endif

