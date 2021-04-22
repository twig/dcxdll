//
// By Ook
// v2.1
//

#include "defines.h"

namespace CRC32 {
	uint32_t tcrc8(const char * str) noexcept
	{
		const auto sepl = _ts_strlen(str);
		uint8_t crc = 0;
		for (auto i = decltype(sepl){0}; i < sepl; ++i)
		{
			crc = gsl::narrow_cast<uint8_t>((crc ^ *str++));

			for (auto j = 0U; j < 8U; ++j)
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

	/// compute CRC32 (standard algorithm)
	uint32_t crc32_1byte(const void* data, size_t length, uint32_t previousCrc32) noexcept
	{
		uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
		const uint8_t* current = static_cast<const uint8_t*>(data);

		while (length-- > 0)
			crc = (crc >> 8) ^ detail::crc_table[(crc & 0xFF) ^ *current++];

		return ~crc; // same as crc ^ 0xFFFFFFFF
	}

}
