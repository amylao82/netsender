
#include "protocol_general.h"

uint32_t protocol_general::htonl(const uint8_t* val)
{
	uint32_t ret = 0;
	for(int i = 0; i < 4; ++i, ++val)
	{
		ret <<= 8;
		ret += *val;
	}

	return ret;
}

uint32_t protocol_general::htonl(const char* val)
{
    return htonl((const uint8_t*)val);
}

uint16_t protocol_general::htons(const uint8_t* val)
{
	uint16_t ret = 0;
	for(int i = 0; i < 2; ++i, ++val)
	{
		ret <<= 8;
		ret += *val;
	}
	return ret;
}

uint16_t protocol_general::htons(const char* val)
{
    return htons((const uint8_t*)val);
}

uint32_t protocol_general::htonl(const uint32_t val)
{
	uint32_t ret = 0;
	const uint8_t* p = (const uint8_t*) &val;
	int i;
	for(i = 0; i < 4; ++i, ++p)
	{
		ret <<= 8;
		ret += *p;
	}

	return ret;
}

uint16_t protocol_general::htons(const uint16_t val)
{
	uint16_t ret;
	ret = (val >> 8) | ((val & 0xFF) << 8);
	return ret;
}

uint8_t protocol_general::cal_checksum_xor(const uint8_t* data, int len)
{
   uint8_t result = 0; 
   const uint8_t* p = data;

   for(int i = 0; i < len; ++i, ++p)
   {
       result ^= *p;
   }

   return result;
}
