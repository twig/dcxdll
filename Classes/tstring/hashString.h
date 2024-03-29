//
// By Ook
// v2.6
//

#pragma once

#define HASH_USE_CRC32 0
#define HASH_USE_FNV 1
#define HASH_USE_ZOB 0
#define HASH_USE_XXH 0
#define HASH_ENABLE_STRING 1
#define HASH_ENABLE_CSTRING 0

#if __has_include("tstring.h")
#define HASH_ENABLE_TSTRING 1
#include "tstring.h"
#else
#define HASH_ENABLE_TSTRING 0
#endif
#if __has_include("refString.h")
#include "refString.h"
#endif
#if __has_include("simpleString.h")
#include "simpleString.h"
#endif

#include <string>
#include <xhash>

// Change string into a hash/number at compile time to allow easy switch()/case usage.
//
// for string literals this is all done at compile time...
//	switch(TEXT("text"_hash)) {
//		case TEXT("text"_hash): do stuff
//}
//
// for objects or other strings this is done at runtime...
//	TString tsObj(TEXT("text"));
//	switch(std::hash<TString>{}(tsObj)) {
//		case TEXT("text"_hash): do stuff
//}
//	const char *str = "text";
//	switch(std::hash<const char *>{}(str)) {
//		case "text"_hash: do stuff
//}
//	const wchar_t *str = L"text";
//	switch(std::hash<const wchar_t *>{}(str)) {
//		case L"text"_hash: do stuff
//}
//	const TCHAR *str = TEXT("text");
//	switch(std::hash<const TCHAR *>{}(str)) {
//		case TEXT("text"_hash): do stuff
//}
// NB: you cannot mix char & wchar_t hashes, they WILL be different.
//

#if defined(HASH_ENABLE_CSTRING) && HASH_ENABLE_CSTRING
#include <afxstr.h>
#endif

#ifdef __cpp_consteval
#define _CONSTEVAL consteval
#else
#define _CONSTEVAL constexpr
#endif

#ifdef __cpp_lib_concepts
namespace HashConcepts
{
	template <class T>
	concept IsCharText = std::is_same_v<std::remove_cvref_t<std::remove_all_extents_t<T>>, char>;

	template <class T>
	concept IsWCharText = std::is_same_v<std::remove_cvref_t<std::remove_all_extents_t<T>>, wchar_t>;

	template <class T>
	concept IsPODText = IsCharText<T> || IsWCharText<T>;

	template <class T>
	concept IsPODTextPointer = std::is_pointer_v<T> && IsPODText<std::remove_pointer_t<T>>;
}
#endif

// mostly taken from https://stackoverflow.com/questions/2111667/compile-time-string-hashing or based on this.

namespace CRC32
{
	// create a crc8 of a const string.
	uint32_t tcrc8(const char* str) noexcept;
	// create a crc32
	uint32_t crc32_1byte(const void* data, size_t length, uint32_t previousCrc32 = 0) noexcept;

	namespace detail
	{
		// CRC32 Table (zlib polynomial)
		static constexpr unsigned int crc_table[256] = {
			0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
			0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
			0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
			0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
			0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
			0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
			0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
			0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
			0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
			0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
			0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
			0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
			0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
			0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
			0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
			0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
			0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
			0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
			0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
			0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
			0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
			0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
			0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
			0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
			0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
			0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
			0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
			0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
			0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
			0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
			0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
			0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
			0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
			0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
			0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
			0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
			0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
			0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
			0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
			0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
			0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
			0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
			0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
		};

	} //namespace detail
	  // This gives us compile time hashing...
	constexpr unsigned int crc32_char(const char v, const unsigned int crc) noexcept
	{
		return detail::crc_table[(crc ^ v) & 0xff] ^ (crc >> 8);
	}

	constexpr unsigned int crc32_helper(const char* v, const unsigned int c, const unsigned int crc) noexcept
	{
		return c == 0 ?
			~crc :
			crc32_helper(&v[1], c - 1, crc32_char(v[0], crc));
	}

	constexpr unsigned int crc32_helper(const wchar_t* v, const unsigned int c, const unsigned int crc) noexcept
	{
		return c == 0 ?
			~crc :
			crc32_helper(&v[1], c - 1, crc32_char(((v[0] >> 8) & 0xFF), crc32_char((v[0] & 0xFF), crc)));
	}
}

// turns a literal string into a hash number at compile time.
constexpr unsigned int operator "" _crc32(const char* v, size_t c)
{
	return CRC32::crc32_helper(v, gsl::narrow_cast<UINT>(c), 0xFFFFFFFF);
}
// turns a literal string into a hash number at compile time.
constexpr size_t operator""_crc32(const wchar_t* p, size_t N)
{
	return CRC32::crc32_helper(p, gsl::narrow_cast<UINT>(N), 0xFFFFFFFF);
}

namespace ZobHash
{
	constexpr uint32_t RotateLeft(const uint32_t value, const int32_t count) noexcept
	{
		const int32_t r = count % 32;
		return (value << r) | (value >> (32 - r));
	}
	constexpr uint64_t RotateLeftLong(const uint64_t value, const int32_t count) noexcept
	{
		const int32_t r = count % 64;
		return (value << r) | (value >> (64 - r));
	}
	static constexpr uint32_t RandomNumbers[] = {
		0x5D60D139,
		0x464D952B,
		0x59D0ABFD,
		0x7552995C,
		0x4403A21F,
		0x5F2D890C,
		0x2E52372B,
		0x292614BF,
		0x650A2FAC,
		0x2AE32EC8,
		0x10B46261,
		0x3173E4B7,
		0x157675BA,
		0x29BC961C,
		0x5D2E9DB6,
		0x368F7F24,
		0x398340A8,
		0x57F22CAE,
		0x74F547CE,
		0x3CDD3EB4,
		0x2235E2BF,
		0x61F6D2C1,
		0x1B2921BF,
		0x3C981448,
		0x177EFA9B,
		0x14029CA8,
		0x2F5E4B1A,
		0x56846712,
		0x66C34128,
		0x19B60BB1,
		0x43EC9A97,
		0x463915C7,
		0x7C12A8B4,
		0x2D3441C0,
		0xECF4E49,
		0x28B6A932,
		0x34954E39,
		0x95AF2BD,
		0x40DEDFD9,
		0x38DAA522,
		0x5C98E835,
		0x31BF45F7,
		0x20ED4335,
		0x447ACDED,
		0x78A5958D,
		0x7A3E0D2F,
		0xE384DFE,
		0x5D8E6F28,
		0x2EC9FBFF,
		0x49510F4B,
		0xD6B1EED,
		0x23C20008,
		0x36896ABC,
		0xA0225E,
		0x14C77C0B,
		0x7019FC4C,
		0x42EFCAA4,
		0x4FEF8081,
		0x5DD39EC1,
		0x30010577,
		0x2FCF3DF2,
		0x57CDD018,
		0x4262D396,
		0x4B5423FB,
		0x66F69430,
		0x4A7B4C99,
		0x35613C02,
		0x684233F9,
		0x1AED47D3,
		0x3477F484,
		0x1FA30EB,
		0x30284DEB,
		0x17134CD5,
		0x3C1AA2AC,
		0x6044567E,
		0x70769CC7,
		0x41098F8C,
		0x56AE53D1,
		0x43F27EBA,
		0x1D40ED6B,
		0x5CA4EAA,
		0x51CFDBF1,
		0x594D720E,
		0x21AF5318,
		0x502E19D8,
		0x137D2F1C,
		0x6A5F0B60,
		0x659414A3,
		0x2133A705,
		0x13654FD1,
		0x7D9235C5,
		0x175CB684,
		0x2B8753FB,
		0x10DDDA62,
		0x90B6730,
		0x4CB181D,
		0x6F5C7260,
		0x55991F39,
		0x5D8439BC,
		0x2E2A48F4,
		0x44DCD12D,
		0x25F61A04,
		0x42A12755,
		0x48BF007F,
		0x4319BD25,
		0x195FA3ED,
		0x195C1EA6,
		0x1FBF67BA,
		0x363A3D92,
		0x1B89FDF3,
		0x3A606EEB,
		0x54761F9A,
		0x594618FA,
		0x4092B156,
		0x2A36B6CD,
		0x5DFF6200,
		0x7E805E09,
		0x20B3807E,
		0x7B260A22,
		0x53796514,
		0x601C4138,
		0x4FCD275E,
		0x470E8CF4,
		0x787F802,
		0x36E5BEBE,
		0x6A9D7A66,
		0x4A0F9F0,
		0x6357273,
		0x330F3B7C,
		0x5B793E61,
		0x11A2A67,
		0x6B707052,
		0x792A1A14,
		0x15C835C6,
		0x58641C3D,
		0x5FD434A5,
		0xF2EB49C,
		0x108E718F,
		0x5E9595F2,
		0x36CE75EB,
		0x7A211075,
		0x4A9FA3A6,
		0x2F59D711,
		0x5A9A912,
		0x5904E0E5,
		0x291C162B,
		0x3E169D89,
		0x6AF4A2A4,
		0x66A72394,
		0x2B0C052F,
		0x64ABA13,
		0x4EA8F1E2,
		0x5A731516,
		0xA0AD4A8,
		0x4E0E07BB,
		0x750FA9CE,
		0x5EE78D0F,
		0x3B192F53,
		0x11D941C1,
		0x587C42BE,
		0x14BEA9FD,
		0x1326AC33,
		0x6CB02C3D,
		0x5AC0FF30,
		0x1A6FD38C,
		0x4EEFFE98,
		0x5B4C0585,
		0x437DE334,
		0x682E9518,
		0x4A628227,
		0x4ED0AD64,
		0x6DF1EC7A,
		0x421DEA8B,
		0x44579437,
		0x5958549E,
		0x157C9D92,
		0x2073504D,
		0x4164E3E2,
		0x2E83171C,
		0xDC9A893,
		0x2C86DCDD,
		0x19AC574B,
		0x1F8E4EDE,
		0x803364D,
		0x552E844E,
		0x32713884,
		0x10FD5B3C,
		0x6F1F456C,
		0x47BA2E0A,
		0x6354726E,
		0xECA796,
		0x54158548,
		0x7EB52FCD,
		0x6195334,
		0x220FCBEE,
		0x66FA6442,
		0x5DEF7768,
		0x5498D7E0,
		0x6B39D585,
		0xA14E24D,
		0x4DD010A5,
		0x7A98BA54,
		0x2C60D8C,
		0x1C44A16D,
		0x5C3B57CA,
		0x1858CD98,
		0xC8B0757,
		0x161B80DF,
		0x30B28009,
		0x38916A29,
		0x549C5981,
		0x1D82A92D,
		0xC961837,
		0x40F992E,
		0x2BF565E1,
		0x7B1252B1,
		0x73985D54,
		0x64ACF5F0,
		0x5927AE2,
		0x67FE9B07,
		0x3DF2A35C,
		0x6C2CC018,
		0x7BC3B529,
		0x4DA22AA,
		0x4975DA91,
		0x7ABB281B,
		0x6F3CBCAC,
		0x3C049757,
		0x2247C849,
		0x725DF05B,
		0x378D2629,
		0x4BDA786C,
		0x562B0E5C,
		0x246E34CF,
		0x3FF997ED,
		0x31EE2288,
		0x16E649BF,
		0x349AD71,
		0x2BC7DE82,
		0x3CD5B6B6,
		0x25E6312D,
		0x7AE1DA5C,
		0x3E6CC563,
		0xF28C3E1,
		0xEB818ED,
		0x6369FE68,
		0x477F6D11,
		0x7AA5969F,
		0x5A23ED52,
		0x26FD793C,
		0x736206ED,
		0x79428177,
		0x4F065CFE,
		0x33B3A7E,
		0x4C223EF0,
		0x61A3508C
	};

	static constexpr uint32_t RandomStart = 0x7ED5052A;
	static constexpr uint64_t RandomLongStart = 0xFCDD00A8D5D2FC6E;

	constexpr uint32_t ZobHash(const char* const data, const int32_t length) noexcept
	{
		auto hash = RandomStart;
		for (int32_t i = 0; i < length; ++i)
			hash ^= RotateLeft(RandomNumbers[static_cast<int32_t>(data[i])], i + 1);
		return hash;
	}

	constexpr uint64_t ZobHashLong(const char* const data, const int32_t length) noexcept
	{
		auto hash = RandomLongStart;
		for (int32_t i = 0; i < length; ++i)
			hash ^= RotateLeftLong(RandomNumbers[static_cast<int32_t>(data[i])], i + 1);
		return hash;
	}
}	// namespace ZobHash

// turns a literal string into a hash number at compile time.
constexpr unsigned int operator "" _zob(const char* v, size_t c)
{
	return ZobHash::ZobHash(v, gsl::narrow_cast<int32_t>(c));
}

namespace FNV1a
{
	// From https://github.com/foonathan/string_id. As usually, thanks Jonathan.

#if defined(_WIN64)
	// FNV-1a 64 bit hash
	using hash_t = std::uint64_t;

	// See http://www.isthe.com/chongo/tech/comp/fnv/#FNV-param
	constexpr hash_t fnv_basis = 14695981039346656037ull;
	constexpr hash_t fnv_prime = 1099511628211ull;
#else
	// FNV-1a 32 bit hash
	using hash_t = std::size_t;
	constexpr hash_t fnv_basis = 2166136261U;
	constexpr hash_t fnv_prime = 16777619U;
#endif
	constexpr hash_t fnv1a_hash(const char* str, hash_t hash = fnv_basis)
	{
		return *str ? fnv1a_hash(str + 1, (hash ^ *str) * fnv_prime) : hash;
	}

	_CONSTEVAL hash_t cfnv1a_hash(const char* str, hash_t hash = fnv_basis)
	{
		return *str ? cfnv1a_hash(str + 1, (hash ^ *str) * fnv_prime) : hash;
	}

	constexpr hash_t fnv1a_hash(const wchar_t* str, hash_t hash = fnv_basis)
	{
		return *str ? fnv1a_hash(str + 1, (((hash ^ (*str & 0xFF)) * fnv_prime) ^ ((*str >> 8) & 0xFF)) * fnv_prime) : hash;
	}

	_CONSTEVAL hash_t cfnv1a_hash(const wchar_t* str, hash_t hash = fnv_basis)
	{
		return *str ? cfnv1a_hash(str + 1, (((hash ^ (*str & 0xFF)) * fnv_prime) ^ ((*str >> 8) & 0xFF)) * fnv_prime) : hash;
	}
}
// turns a literal string into a hash number at compile time.
_CONSTEVAL FNV1a::hash_t operator""_fnv1a(const char* p, size_t N)
{
	//return FNV1a::fnv1a_hash(N - 1, p);
	return FNV1a::cfnv1a_hash(p);
}

// turns a literal string into a hash number at compile time.
_CONSTEVAL FNV1a::hash_t operator""_fnv1a(const wchar_t* p, size_t N)
{
	//return FNV1a::fnv1a_hash(N - 1, p);
	return FNV1a::cfnv1a_hash(p);
}

namespace XXH3
{
	// Taken from https://cyan4973.github.io/xxHash/

	namespace detail
	{
		constexpr uint32_t rotl32(uint32_t v, int x)
		{
			return std::rotl(v, x);
		}

		constexpr uint64_t rotl64(uint64_t v, int x)
		{
			return std::rotl(v, x);
		}

		constexpr uint8_t read_u8(const char* input, int inputLen, int pos)
		{
			if (pos >= inputLen)
				return 0;

			GSL_SUPPRESS(type.1) return static_cast<uint8_t>(input[pos]);
		}
		constexpr uint32_t read_u32le(const char* input, int inputLen, int pos)
		{
			const uint32_t b0 = read_u8(input, inputLen, pos + 0);
			const uint32_t b1 = read_u8(input, inputLen, pos + 1);
			const uint32_t b2 = read_u8(input, inputLen, pos + 2);
			const uint32_t b3 = read_u8(input, inputLen, pos + 3);
			return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
		}

		constexpr uint64_t read_u64le(const char* input, int inputLen, int pos)
		{
			const uint64_t l0 = read_u32le(input, inputLen, pos + 0);
			const uint64_t l1 = read_u32le(input, inputLen, pos + 4);
			return l0 | (l1 << 32);
		}
		namespace xxh32
		{
			constexpr uint32_t prime32_1 = 0x9E3779B1U;
			constexpr uint32_t prime32_2 = 0x85EBCA77U;
			constexpr uint32_t prime32_3 = 0xC2B2AE3DU;
			constexpr uint32_t prime32_4 = 0x27D4EB2FU;
			constexpr uint32_t prime32_5 = 0x165667B1U;
			constexpr uint32_t default_seed = 0;

			constexpr uint32_t xxh32_round(uint32_t acc, const uint32_t d32)
			{
				acc += d32 * prime32_2;
				acc = rotl32(acc, 13) * prime32_1;
				return acc;
			}

			constexpr uint32_t xxh32_avalanche(uint32_t h32)
			{
				h32 ^= h32 >> 15;
				h32 *= prime32_2;
				h32 ^= h32 >> 13;
				h32 *= prime32_3;
				h32 ^= h32 >> 16;
				return h32;
			}

			constexpr uint32_t xxh32_finalize(const char* input, int inputLen, int pos, uint32_t h32)
			{
				// XXH_PROCESS4
				while ((inputLen - pos) >= 4)
				{
					const uint32_t d32 = read_u32le(input, inputLen, pos);
					pos += 4;
					h32 += d32 * prime32_3;
					h32 = rotl32(h32, 17) * prime32_4;
				}
				// XXH_PROCESS1
				while ((inputLen - pos) > 0)
				{
					const uint32_t d8 = read_u8(input, inputLen, pos);
					pos += 1;
					h32 += d8 * prime32_5;
					h32 = rotl32(h32, 11) * prime32_1;
				}
				return h32;
			}

			constexpr uint32_t xxh32_digest(
				const char* input, int inputLen, int pos,
				uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4
			)
			{
				uint32_t h32 = 0;
				if (inputLen >= 16)
				{
					h32 = rotl32(v1, 1) + rotl32(v2, 7) + rotl32(v3, 12) + rotl32(v4, 18);
				}
				else {
					h32 = v3 + prime32_5;
				}
				h32 += inputLen;
				h32 = xxh32_finalize(input, inputLen, pos, h32);
				return xxh32_avalanche(h32);
			}

			constexpr uint32_t xxh32(const char* input, int inputLen, uint32_t seed)
			{
				uint32_t v1 = seed + prime32_1 + prime32_2;
				uint32_t v2 = seed + prime32_2;
				uint32_t v3 = seed;
				uint32_t v4 = seed - prime32_1;
				int pos = 0;
				while (pos + 16 <= inputLen)
				{
					v1 = xxh32_round(v1, read_u32le(input, inputLen, pos + 0 * 4));
					v2 = xxh32_round(v2, read_u32le(input, inputLen, pos + 1 * 4));
					v3 = xxh32_round(v3, read_u32le(input, inputLen, pos + 2 * 4));
					v4 = xxh32_round(v4, read_u32le(input, inputLen, pos + 3 * 4));
					pos += 16;
				}
				return xxh32_digest(input, inputLen, pos, v1, v2, v3, v4);
			}

		}
		namespace xxh64
		{
			constexpr uint64_t prime64_1 = 0x9E3779B185EBCA87ULL;
			constexpr uint64_t prime64_2 = 0xC2B2AE3D27D4EB4FULL;
			constexpr uint64_t prime64_3 = 0x165667B19E3779F9ULL;
			constexpr uint64_t prime64_4 = 0x85EBCA77C2B2AE63ULL;
			constexpr uint64_t prime64_5 = 0x27D4EB2F165667C5ULL;
			constexpr uint64_t default_seed = 0;

			constexpr uint64_t xxh64_round(uint64_t acc, uint64_t d64)
			{
				acc += d64 * prime64_2;
				acc = rotl64(acc, 31);
				acc *= prime64_1;
				return acc;
			}

			constexpr uint64_t xxh64_mergeRound(uint64_t acc, uint64_t val)
			{
				val = xxh64_round(0, val);
				acc ^= val;
				acc = acc * prime64_1 + prime64_4;
				return acc;
			}

			constexpr uint64_t xxh64_avalanche(uint64_t h64)
			{
				// XXH64_avalanche
				h64 ^= h64 >> 33;
				h64 *= prime64_2;
				h64 ^= h64 >> 29;
				h64 *= prime64_3;
				h64 ^= h64 >> 32;
				return h64;
			}

			constexpr uint64_t xxh64_finalize(const char* input, int inputLen, int pos, uint64_t h64)
			{
				// XXH_PROCESS8_64
				while ((inputLen - pos) >= 8)
				{
					const uint64_t d64 = read_u64le(input, inputLen, pos);
					pos += 8;
					h64 ^= xxh64_round(0, d64);
					h64 = rotl64(h64, 27) * prime64_1 + prime64_4;
				}
				// XXH_PROCESS4_64
				while ((inputLen - pos) >= 4)
				{
					const uint64_t d32 = read_u32le(input, inputLen, pos);
					pos += 4;
					h64 ^= d32 * prime64_1;
					h64 = rotl64(h64, 23) * prime64_2 + prime64_3;
				}
				// XXH_PROCESS1_64
				while ((inputLen - pos) > 0)
				{
					const uint64_t d8 = read_u8(input, inputLen, pos);
					pos += 1;
					h64 ^= d8 * prime64_5;
					h64 = rotl64(h64, 11) * prime64_1;
				}
				return xxh64_avalanche(h64);
			}

			constexpr uint64_t xxh64_digest(
				const char* input, int inputLen, int pos,
				uint64_t v1, uint64_t v2, uint64_t v3, uint64_t v4
			)
			{
				uint64_t h64 = 0;
				if (inputLen >= 32)
				{
					h64 = rotl64(v1, 1) + rotl64(v2, 7) + rotl64(v3, 12) + rotl64(v4, 18);
					h64 = xxh64_mergeRound(h64, v1);
					h64 = xxh64_mergeRound(h64, v2);
					h64 = xxh64_mergeRound(h64, v3);
					h64 = xxh64_mergeRound(h64, v4);
				}
				else {
					h64 = v3 + prime64_5;
				}
				h64 += inputLen;
				return xxh64_finalize(input, inputLen, pos, h64);
			}

			constexpr uint64_t xxh64(const char* input, int inputLen, uint64_t seed)
			{
				// XXH64_reset
				uint64_t v1 = seed + prime64_1 + prime64_2;
				uint64_t v2 = seed + prime64_2;
				uint64_t v3 = seed;
				uint64_t v4 = seed - prime64_1;

				// XXH64_update
				int pos = 0;
				while (pos + 32 <= inputLen)
				{
					v1 = xxh64_round(v1, read_u64le(input, inputLen, pos + 0 * 8));
					v2 = xxh64_round(v2, read_u64le(input, inputLen, pos + 1 * 8));
					v3 = xxh64_round(v3, read_u64le(input, inputLen, pos + 2 * 8));
					v4 = xxh64_round(v4, read_u64le(input, inputLen, pos + 3 * 8));
					pos += 32;
				}
				return xxh64_digest(input, inputLen, pos, v1, v2, v3, v4);
			}
		}
	}
	constexpr uint32_t xxh32(const char* input, int inputLen, uint32_t seed)
	{
		return detail::xxh32::xxh32(input, inputLen, seed);
	}

	constexpr uint64_t xxh64(const char* input, int inputLen, uint64_t seed)
	{
		return detail::xxh64::xxh64(input, inputLen, seed);
	}

}

// _xxh32 suffix for string which provides compile time string to XXH32 conversion
_CONSTEVAL uint32_t operator "" _xxh32(const char* p, size_t N)
{
	constexpr uint32_t seed = 0;
	return XXH3::xxh32(p, gsl::narrow_cast<int>(N - 1), seed);
}


// _xxh64 suffix for string which provides compile time string to XXH64 conversion
_CONSTEVAL uint64_t operator "" _xxh64(const char* p, size_t N)
{
	constexpr uint64_t seed = 0;
	return XXH3::xxh64(p, gsl::narrow_cast<int>(N - 1), seed);
}

/// <summary>
/// Create a compile time hash of a const string. (no overflow bug, but causes dll size increase) 
/// </summary>
/// <param name="input">- String to hash</param>
/// <returns>The hash value for the given string.</returns>
#ifdef __cpp_lib_concepts
template <HashConcepts::IsPODText T>
#else
template <class T>
#endif
_NODISCARD constexpr size_t const_hash(const T* const input) noexcept
{
	return *input ?
		(static_cast<size_t>(*input) + 33U) + CRC32::detail::crc_table[((const_hash(input + 1) ^ static_cast<size_t>(*input)) & 0xFF)] :
		5381U;
}

/// <summary>
/// Get compile time hash of string
/// </summary>
/// <typeparam name="T">HashConcepts::IsPODText</typeparam>
/// <param name="input">- String to hash</param>
/// <param name="N">- String length in characters</param>
/// <returns>The hash value for the given string.</returns>
#ifdef __cpp_lib_concepts
template <HashConcepts::IsPODText T>
#else
template <class T>
#endif
_NODISCARD size_t dcx_hash(const T* const input, const size_t& N) noexcept
{
	static_assert(std::is_same_v<char, std::remove_cv_t<T>> || std::is_same_v<wchar_t, std::remove_cv_t<T>>, "Type must be char or wchar_t");
#if defined(HASH_USE_CRC32) && HASH_USE_CRC32
	return CRC32::crc32_1byte(input, N * sizeof(T));
#else
#if defined(HASH_USE_FNV) && HASH_USE_FNV
	//return _Hash_array_representation(input, N);
	//return FNV1a::fnv1a_hash(N, input);
	UNREFERENCED_PARAMETER(N);
	return FNV1a::fnv1a_hash(input);
#else
#if defined(HASH_USE_ZOB) && HASH_USE_ZOB
	return ZobHash::ZobHash(input, N);
#else
#if defined(HASH_USE_XXH) && HASH_USE_XXH
	constexpr uint32_t seed = 0;

	return XXH3::xxh3(input, N, seed);
#else
#error "No Hash functions selected!"
#endif
#endif
#endif
#endif
}

/// <summary>
/// Get compile time hash of string
/// </summary>
/// <typeparam name="T">HashConcepts::IsPODText</typeparam>
/// <param name="input">- String to hash</param>
/// <returns>The hash value for the given string.</returns>
#ifdef __cpp_lib_concepts
template <HashConcepts::IsPODText T>
#else
template <class T>
#endif
_NODISCARD size_t dcx_hash(const T* const input) noexcept
{
	static_assert(std::is_same_v<char, std::remove_cv_t<T>> || std::is_same_v<wchar_t, std::remove_cv_t<T>>, "Type must be char or wchar_t");
#if defined(HASH_USE_CRC32) && HASH_USE_CRC32
	return dcx_hash(input, _ts_strlen(input));
#else
#if defined(HASH_USE_FNV) && HASH_USE_FNV
	//return _Hash_array_representation(input, _ts_strlen(input));
	//return FNV1a::fnv1a_hash(_ts_strlen(input), input);
	return FNV1a::fnv1a_hash(input);
#else
#if defined(HASH_USE_ZOB) && HASH_USE_ZOB
	return dcx_hash(input, _ts_strlen(input));
#else
#if defined(HASH_USE_XXH) && HASH_USE_XXH
	return dcx_hash(input, _ts_strlen(input));
#else
#error "No Hash functions selected!"
#endif
#endif
#endif
#endif
}

// turns a literal string into a hash number at compile time.
_CONSTEVAL size_t operator""_hash(const char* p, size_t N)
{
#if defined(HASH_USE_CRC32) && HASH_USE_CRC32
	return CRC32::crc32_helper(p, N, 0xFFFFFFFF);
#else
#if defined(HASH_USE_FNV) && HASH_USE_FNV
	return FNV1a::cfnv1a_hash(p);
#else
#if defined(HASH_USE_ZOB) && HASH_USE_ZOB
	return ZobHash::ZobHash(p, N);
#else
#if defined(HASH_USE_XXH) && HASH_USE_XXH
	constexpr uint32_t seed = 0;

	return XXH3::xxh3(input, N, seed);
#else
#error "No Hash functions selected!"
#endif
#endif
#endif
#endif
}

// turns a literal string into a hash number at compile time.
_CONSTEVAL size_t operator""_hash(const wchar_t* p, size_t N)
{
#if defined(HASH_USE_CRC32) && HASH_USE_CRC32
	return CRC32::crc32_helper(p, N, 0xFFFFFFFF);
#else
#if defined(HASH_USE_FNV) && HASH_USE_FNV
	return FNV1a::cfnv1a_hash(p);
#else
#if defined(HASH_USE_ZOB) && HASH_USE_ZOB
	return ZobHash::ZobHash(p, N);
#else
#if defined(HASH_USE_XXH) && HASH_USE_XXH
	constexpr uint32_t seed = 0;

	return XXH3::xxh3(input, N, seed);
#else
#error "No Hash functions selected!"
#endif
#endif
#endif
#endif
}

// This gives us runtime hashing...

namespace std
{
#if defined(HASH_ENABLE_TSTRING) && HASH_ENABLE_TSTRING
	// STRUCT TEMPLATE SPECIALIZATION hash for TString
	template<> struct hash<TString>
	{
		typedef TString argument_type;
		typedef std::size_t result_type;

		GSL_SUPPRESS(r.30)
			GSL_SUPPRESS(r.36)
			_NODISCARD result_type operator()(argument_type const& s) const noexcept
		{
			return dcx_hash(s.to_chr(), s.len());
			//return dcx_hash(s.to_chr().get(), s.len());
		}
	};
#endif

#if defined(HASH_ENABLE_CSTRING) && HASH_ENABLE_CSTRING
	// STRUCT TEMPLATE SPECIALIZATION hash for CString
	template<> struct hash<CString>
	{
		typedef CString argument_type;
		typedef size_t result_type;

		GSL_SUPPRESS(r.30)
			GSL_SUPPRESS(r.36)
			_NODISCARD result_type operator()(argument_type const& s) const noexcept
		{
			return dcx_hash(s.GetString(), s.GetLength());
		}
	};
#endif

#if defined(HASH_ENABLE_STRING) && HASH_ENABLE_STRING
	// STRUCT TEMPLATE SPECIALIZATION hash for const char*
	template<> struct hash<const char*>
	{
		typedef const char* argument_type;
		typedef std::size_t result_type;

		_NODISCARD result_type operator()(argument_type const& s) const noexcept
		{
			return operator()(s, _ts_strlen(s));
		}
		_NODISCARD result_type operator()(argument_type const& s, std::size_t N) const noexcept
		{
			return dcx_hash(s, N);
		}
	};

	// STRUCT TEMPLATE SPECIALIZATION hash for const wchar_t*
	template<> struct hash<const wchar_t*>
	{
		typedef const wchar_t* argument_type;
		typedef std::size_t result_type;

		_NODISCARD result_type operator()(argument_type const& s) const noexcept
		{
			return operator()(s, _ts_strlen(s));
		}
		_NODISCARD result_type operator()(argument_type const& s, std::size_t N) const noexcept
		{
			return dcx_hash(s, N);
		}
	};
#endif
}

//
