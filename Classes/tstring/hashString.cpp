#include "defines.h"

uint32_t tcrc8(const char * str)
{
	auto sepl = _ts_strlen(str);
	uint8_t crc = 0;
	for (auto i = decltype(sepl){0}; i < sepl; i++)
	{
		crc = (uint8_t)(crc ^ *str++);

		for (auto j = 0U; j < 8U; j++)
		{
			if ((crc & 0x80) != 0)
			{
				crc <<= 1;
				crc ^= 0x07;
			}
			else
				crc <<= 1;
		}
	}
	return crc;
}
