#pragma once
// support functions for TString & c-string handling...
// v1.27

#include <tchar.h>
#include <cstdlib>
#include <codecvt>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#include <concepts>

#include <gsl/gsl>

//#if __has_include("StrToNum/StrToNum.h")
//#define STRINGSUPPORT_STRTONUM 1
//#include "StrToNum/StrToNum.h"
//#endif

// determine whether _Ty is a Number type (excluding char / wchar)
template<class _Ty>
struct is_Numeric
	: std::integral_constant<bool,
	(std::is_arithmetic_v<_Ty> || std::is_same_v<_Ty, std::byte> || std::is_enum_v<_Ty>)
	&& !std::is_same_v<_Ty, wchar_t>
	&& !std::is_same_v<_Ty, char>
	&& !std::is_pointer_v<_Ty>
	>
{
};

// determine whether T is a Number type (excluding char / wchar), same as is_Numeric<T>::value
template <typename T>
constexpr bool is_Numeric_v = is_Numeric<T>::value;

namespace std
{
	inline string to_string(_In_ const wstring& wstr)
	{
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;

		//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
		return converter.to_bytes(wstr);
	}

	inline wstring to_wstring(_In_ const string& str)
	{
		wstring wstr;
		if (!str.empty())
			wstr.assign(str.begin(), str.end());
		return wstr;
	}

	inline u32string to_u32string(_In_ const string& str)
	{
		// Convert to utf-32.
		using convert_type = std::codecvt_utf8<char32_t>;
		std::wstring_convert<convert_type, char32_t> converter;

		return converter.from_bytes(str);
	}

	inline u32string to_u32string(_In_ const wstring& wstr)
	{
		// Convert to utf-8.
		GSL_SUPPRESS(type.4) const std::u16string s((char16_t*)wstr.c_str());
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv8;
		const std::string utf8_str(conv8.to_bytes(s));

		// Convert to utf-32.
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv32;
		return conv32.from_bytes(utf8_str);
	}

	inline string to_string(_In_ const std::byte& num)
	{
		return to_string(gsl::narrow_cast<uint8_t>(num));
	}

	inline wstring to_wstring(_In_ const std::byte& num)
	{
		return to_wstring(gsl::narrow_cast<uint8_t>(num));
	}
}

struct ci_char_traits
	: public std::char_traits<char>
{
	static char to_upper(_In_ char ch) noexcept
	{
		if (ch >= 'a' && ch <= 'z') return _toupper(ch);
		return ch;
	}
	static bool eq(_In_ char c1, _In_ char c2) noexcept
	{
		return to_upper(c1) == to_upper(c2);
	}
	static bool lt(_In_ char c1, _In_ char c2) noexcept
	{
		return to_upper(c1) < to_upper(c2);
	}
	static int compare(_In_z_ const char* s1, _In_z_ const char* s2, _In_ size_t n) noexcept
	{
		while (n-- != 0)
		{
			if (to_upper(*s1) < to_upper(*s2)) return -1;
			if (to_upper(*s1) > to_upper(*s2)) return 1;
			++s1; ++s2;
		}
		return 0;
	}
	static const char* find(_In_z_ const char* s, _In_ int n, _In_ char a) noexcept
	{
		if (s)
			for (auto const ua(to_upper(a)); (n != 0); ++s, --n)
			{
				if (to_upper(*s) == ua)
					return s;
			}
		return nullptr;
	}
};
struct ci_wchar_traits
	: public std::char_traits<wchar_t>
{
	static wchar_t to_upper(_In_ wchar_t ch) noexcept
	{
		if (ch >= L'a' && ch <= L'z') return _toupper(ch);
		return ch;
	}
	static bool eq(_In_ wchar_t c1, _In_ wchar_t c2) noexcept
	{
		return to_upper(c1) == to_upper(c2);
	}
	static bool lt(_In_ wchar_t c1, _In_ wchar_t c2) noexcept
	{
		return to_upper(c1) < to_upper(c2);
	}
	static int compare(_In_z_ const wchar_t* s1, _In_z_ const wchar_t* s2, _In_ size_t n) noexcept
	{
		while (n-- != 0)
		{
			if (to_upper(*s1) < to_upper(*s2)) return -1;
			if (to_upper(*s1) > to_upper(*s2)) return 1;
			++s1; ++s2;
		}
		return 0;
	}
	static const wchar_t* find(_In_z_ const wchar_t* s, _In_ int n, _In_ wchar_t a) noexcept
	{
		if (s)
			for (auto const ua(to_upper(a)); (n-- != 0); ++s)
			{
				if (to_upper(*s) == ua)
					return s;
			}
		return nullptr;
	}
};
using ci_stringview = std::basic_string_view<char, ci_char_traits>;
using ci_wstringview = std::basic_string_view<wchar_t, ci_wchar_traits>;

namespace details {
	template<class _Ty>
	struct is_pod
		: std::integral_constant<bool, std::is_standard_layout_v<_Ty>&& std::is_trivial_v<_Ty> >
	{
	};

	template <typename T>
	constexpr bool is_pod_v = is_pod<T>::value;

	template<typename T>
	concept IsPODText = std::is_same_v<std::remove_all_extents_t<std::remove_cv_t<std::remove_pointer_t<T>>>, char> || std::is_same_v<std::remove_all_extents_t<std::remove_cv_t<std::remove_pointer_t<T>>>, wchar_t>;

	template <class T>
	concept IsNumeric = is_Numeric_v<std::remove_cvref_t<T>>;

	template <class T>
	concept HasDataFunction = requires(T t)
	{
		t.data();
	};

	template <class T>
	concept HasChrFunction = requires(T t)
	{
		t.to_chr();
	};

	template <class T>
	concept HasSizeFunction = requires(T t)
	{
		static_cast<std::size_t>(t.size());
	};

	template <class T>
	concept HasCapacityFunction = requires(T t)
	{
		static_cast<std::size_t>(t.capacity_cch());
	};

	template <class T>
	concept HasDataSizeNoChr = HasDataFunction<T> && HasSizeFunction<T> && !HasChrFunction<T>;

	template <class T>
	concept HasDataSizeChr = HasDataFunction<T> && HasSizeFunction<T> && HasChrFunction<T>;

	template <class T>
	concept HasCapacityChr = HasCapacityFunction<T> && HasChrFunction<T>;

	constexpr WCHAR make_upper(_In_ const WCHAR c) noexcept
	{
		if (c >= L'a' && c <= L'z') return _toupper(c);
		return c;
	}
	constexpr char make_upper(_In_ const char c) noexcept
	{
		if (c >= 'a' && c <= 'z') return _toupper(c);
		return c;
	}
	constexpr bool CompareChar(_In_ const WCHAR c, _In_ const WCHAR other, _In_ const bool bCase) noexcept
	{
		return (bCase) ? c == other : make_upper(c) == make_upper(other);
	}
	constexpr bool CompareChar(_In_ const char c, _In_ const char other, _In_ const bool bCase) noexcept
	{
		return (bCase) ? c == other : make_upper(c) == make_upper(other);
	}

	template <typename Result, typename Format>
	Result& _ts_printf_do(_Inout_ Result& res, _In_ const Format& fmt)
	{
		res += fmt;
		return res;
	}

	template <typename Result, typename Format, typename Value, typename... Arguments>
	GSL_SUPPRESS(con.3) Result& _ts_printf_do(_Inout_ Result& res, _In_ const Format& fmt, _In_ const Value& val, _In_ Arguments&&... args)
	{
		auto i = 0U;
		auto bSkip = false;

		for (auto c = fmt[0]; c != decltype(c){'\0'}; c = fmt[++i])
		{
			if (!bSkip)
			{
				if (c == decltype(c){'\\'})
				{
					bSkip = true;
					continue;
				}
				else if (c == decltype(c){'%'})
				{
					if constexpr (is_Numeric_v<Value>)
					{
						if constexpr (std::is_same_v<std::string, std::remove_cv_t<Result>>)
							res += std::to_string(val);
						else if constexpr (std::is_same_v<std::wstring, std::remove_cv_t<Result>>)
							res += std::to_wstring(val);
						else
							res += val;	// assumes this type can handle adding numbers.
					}
					else
						res += val;

					if constexpr (std::is_array_v<Format> && details::is_pod_v<Format>)
						return _ts_printf_do(res, &fmt[0] + i + 1, args...);
					else
						return _ts_printf_do(res, fmt + i + 1, args...);
				}
			}
			else
				bSkip = false;
			res += c;
		}

		return res;
	}

	template<typename T, class TR = std::char_traits<T> >
	struct impl_strstr {
		using ptype = typename TR::char_type*;
		using cptype = typename const ptype;

		ptype operator()(ptype input, cptype find)
		{
			do {
				ptype p{}, q{};
				for (p = input, q = find; !TR::eq(*q, 0) && TR::eq(*p, *q); p++, q++) {}

				if (TR::eq(*q, 0))
					return input;

			} while (!TR::eq(*(input++), 0));
			return nullptr;
		}
	};

	// Test if a string is empty, works for char & wchar & char* & wchar* & std::basic_string & TString objects
	template <typename T>
	bool _ts_isEmpty(_In_ const T& str) noexcept
	{
		if constexpr (std::is_pointer_v<T>)
		{
			// T is a pointer
			// Test if a string is empty, works for C String char * or wchar_t *
			using value_type = std::remove_cv_t<std::remove_pointer_t<T> >;

			static_assert(IsPODText<value_type>, "Invalid Type used");
			return ((str == nullptr) || (str[0] == value_type()));
		}
		else if constexpr (std::is_standard_layout_v<T> && std::is_trivial_v<T>) // same as old std::is_pod_v<T>
		{
			if constexpr (std::is_array_v<T>)
			{
				// T is NOT a pointer but IS POD and IS an array (char[3] ...)
				// Test if a string is empty, works for C char or wchar_t
				using value_type = std::remove_all_extents_t<std::remove_cv_t<T>>;

				static_assert(IsPODText<value_type>, "Invalid Type used");
				return (str[0] == value_type());
			}
			else {
				// T is NOT a pointer but IS POD and is NOT an array (single char ...)
				// Test if a string is empty, works for C char or wchar_t
				using value_type = std::remove_cv_t<T>;

				static_assert(IsPODText<value_type>, "Invalid Type used");
				return (str == value_type());
			}
		}
		else if constexpr (std::is_member_function_pointer_v<decltype(&T::empty)>)
		{
			// T is NOT a pointer and is NOT POD
			// Test if a container is empty, works for std::basic_string & TString objects or any container with an empty() member function.
			return str.empty();
		}
	}
	template <typename T>
	bool _ts_isEmpty(_In_ const gsl::strict_not_null<T>& str) noexcept
	{
		return _ts_isEmpty(str.get());
	}

	// Get String length
	template <typename T, IsNumeric size_type = std::size_t>
	constexpr size_type _ts_strlen(_In_ const T& str) noexcept
	{
		if constexpr (std::is_pointer_v<T>)
		{
			// T is a pointer
			//  return length of string...

			static_assert(IsPODText<T>, "Invalid Type used");
			auto iLen = size_type();
			for (auto p = str; p && *p; ++p)
				++iLen;
			return iLen;
		}
		else if constexpr (IsPODText<T>)
		{
			// T is POD, char or wchar_t
			// checks if char is zero or not
			return (str == T() ? 0U : 1U);
		}
		else if constexpr (std::is_member_function_pointer_v<decltype(&T::length)>)
		{
			// T has a member function length()
			return gsl::narrow_cast<size_type>(str.length());
		}
	}

	template <IsPODText T, IsNumeric size_type = std::size_t, std::size_t N>
	constexpr inline size_type _ts_strlen(_In_z_ T const (&)[N]) noexcept
	{
		// T is a fixed array
		return (N == 0U ? 0U : N - 1);
	}

	template <typename T>
	struct _impl_strnlen {
	};
	template <>
	struct _impl_strnlen<char*> {
		size_t operator()(_In_reads_or_z_(length) const char* const ptr, _In_ size_t length) noexcept
		{
			return strnlen(ptr, length);
		}
	};
	template <>
	struct _impl_strnlen<wchar_t*> {
		size_t operator()(_In_reads_or_z_(length) const wchar_t* const ptr, _In_ size_t length) noexcept
		{
			return wcsnlen(ptr, length);
		}
	};
	template <>
	struct _impl_strnlen<char> {
		size_t operator()(_In_ const char& ptr, _In_ size_t length) noexcept
		{
			return (ptr == 0 || length == 0 ? 0U : 1U);
		}
	};
	template <>
	struct _impl_strnlen<wchar_t> {
		size_t operator()(_In_ const wchar_t& ptr, _In_ size_t length) noexcept
		{
			return (ptr == 0 || length == 0 ? 0U : 1U);
		}
	};

	template <typename T>
	struct _impl_strcpyn {
	};
	template <>
	struct _impl_strcpyn<char> {
		char* operator()(_Out_writes_(length) char* const pDest, _In_reads_or_z_(length) const char* const pSrc, _In_ const size_t length) noexcept
		{
			//auto t = strncpy(pDest, pSrc, length);	// doesn't guarantee zero termination!
			//if (t)
			//	t[length - 1] = 0;	// make sure it ends in zero
			//return t;

			return lstrcpynA(pDest, pSrc, gsl::narrow_cast<int>(length));
		}
	};
	template <>
	struct _impl_strcpyn<wchar_t> {
		wchar_t* operator()(_Out_writes_(length) wchar_t* const pDest, _In_reads_or_z_(length) const wchar_t* const pSrc, _In_ const size_t length) noexcept
		{
			//auto t = wcsncpy(pDest, pSrc, length); // doesn't guarantee zero termination!
			//if (t)
			//	t[length - 1] = 0;	// make sure it ends in zero
			//return t;

			return lstrcpynW(pDest, pSrc, gsl::narrow_cast<int>(length));
		}
	};

	template <typename T>
	struct _impl_strcpy {
	};
	template <>
	struct _impl_strcpy<char> {
		char* operator()(_Out_writes_z_(_String_length_(pSrc) + 1) char* const pDest, _In_z_ const char* const pSrc) noexcept
		{
			return strcpy(pDest, pSrc);
		}
	};
	template <>
	struct _impl_strcpy<wchar_t> {
		wchar_t* operator()(_Out_writes_z_(_String_length_(pSrc) + 1) wchar_t* const pDest, _In_z_ const wchar_t* const pSrc) noexcept
		{
			return wcscpy(pDest, pSrc);
		}
	};

	template <typename T>
	struct _impl_strncat {
	};
	template <>
	struct _impl_strncat<char> {
		char* operator()(_Inout_updates_z_(length) char* const pDest, _In_reads_or_z_(length) const char* const pSrc, _In_ const size_t length) noexcept
		{
			return strncat(pDest, pSrc, length);
		}
	};
	template <>
	struct _impl_strncat<wchar_t> {
		wchar_t* operator()(_Inout_updates_z_(length) wchar_t* const pDest, _In_reads_or_z_(length) const wchar_t* const pSrc, _In_ const size_t length) noexcept
		{
			return wcsncat(pDest, pSrc, length);
		}
	};

	template <typename T>
	struct _impl_strcat {
	};
	template <>
	struct _impl_strcat<char> {
		char* operator()(_Inout_updates_z_(_String_length_(pDest) + _String_length_(pSrc) + 1) char* const pDest, _In_z_ const char* const pSrc) noexcept
		{
			return strcat(pDest, pSrc);
		}
	};
	template <>
	struct _impl_strcat<wchar_t> {
		wchar_t* operator()(_Inout_updates_z_(_String_length_(pDest) + _String_length_(pSrc) + 1) wchar_t* const pDest, _In_z_ const wchar_t* const pSrc) noexcept
		{
			return wcscat(pDest, pSrc);
		}
	};

	template <typename T>
	struct _impl_strchr {
	};
	template <>
	struct _impl_strchr<char> {
		char* operator()(_In_z_ char* const pString, _In_ const char ch)
		{
			return strchr(pString, ch);
		}
	};
	template <>
	struct _impl_strchr<wchar_t> {
		wchar_t* operator()(_In_z_ wchar_t* const pString, _In_ const wchar_t ch)
		{
			return wcschr(pString, ch);
		}
	};

	template <typename T>
	struct _impl_strncmp {
	};
	template <>
	struct _impl_strncmp<char> {
		int operator()(_In_reads_or_z_(length) const char* const pDest, _In_reads_or_z_(length) const char* const pSrc, _In_ const size_t length) noexcept
		{
			return strncmp(pDest, pSrc, length);
		}
	};
	template <>
	struct _impl_strncmp<wchar_t> {
		int operator()(_In_reads_or_z_(length) const wchar_t* const pDest, _In_reads_or_z_(length) const wchar_t* const pSrc, _In_ const size_t length) noexcept
		{
			return wcsncmp(pDest, pSrc, length);
		}
	};

	template <typename T>
	struct _impl_strnicmp {
	};
	template <>
	struct _impl_strnicmp<char> {
		int operator()(_In_reads_or_z_(length) const char* const pDest, _In_reads_or_z_(length) const char* const pSrc, _In_ const size_t length) noexcept
		{
			return _strnicmp(pDest, pSrc, length);
		}
	};
	template <>
	struct _impl_strnicmp<wchar_t> {
		int operator()(_In_reads_or_z_(length) const wchar_t* const pDest, _In_reads_or_z_(length) const wchar_t* const pSrc, _In_ const size_t length) noexcept
		{
			return _wcsnicmp(pDest, pSrc, length);
		}
	};

	template <typename T>
	struct _impl_strcmp {
	};
	template <>
	struct _impl_strcmp<char> {
		int operator()(_In_z_ const char* const pDest, _In_z_ const char* const pSrc) noexcept
		{
			return strcmp(pDest, pSrc);
		}
	};
	template <>
	struct _impl_strcmp<wchar_t> {
		int operator()(_In_z_ const wchar_t* const pDest, _In_z_ const wchar_t* const pSrc) noexcept
		{
			return wcscmp(pDest, pSrc);
		}
	};

	template <typename T>
	struct _impl_stricmp {
	};
	template <>
	struct _impl_stricmp<char> {
		int operator()(_In_z_ const char* const pDest, _In_z_ const char* const pSrc) noexcept
		{
			return _stricmp(pDest, pSrc);
		}
	};
	template <>
	struct _impl_stricmp<wchar_t> {
		int operator()(_In_z_ const wchar_t* const pDest, _In_z_ const wchar_t* const pSrc) noexcept
		{
			return _wcsicmp(pDest, pSrc);
		}
	};

	template <typename T>
	struct _impl_ts_find {
	};
	template <>
	struct _impl_ts_find<char> {
		const char* operator()(_In_z_ const char* const str, _In_ const char& srch) noexcept
		{
			return strchr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<const char> {
		const char* operator()(_In_z_ const char* const str, _In_ const char& srch) noexcept
		{
			return strchr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<char*> {
		const char* operator()(_In_z_ const char* const str, _In_z_ const char* srch) noexcept
		{
			return strstr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<const char*> {
		const char* operator()(_In_z_ const char* const str, _In_z_ const char* srch) noexcept
		{
			return strstr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<wchar_t> {
		const wchar_t* operator()(_In_z_ const wchar_t* const str, _In_ const wchar_t& srch) noexcept
		{
			return wcschr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<const wchar_t> {
		const wchar_t* operator()(_In_z_ const wchar_t* const str, _In_ const wchar_t& srch) noexcept
		{
			return wcschr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<wchar_t*> {
		const wchar_t* operator()(_In_z_ const wchar_t* const str, _In_ const wchar_t* srch) noexcept
		{
			return wcsstr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<const wchar_t*> {
		const wchar_t* operator()(_In_z_ const wchar_t* const str, _In_ const wchar_t* srch) noexcept
		{
			return wcsstr(str, srch);
		}
	};

	template <typename T>
	struct _impl_vscprintf {
	};
	template <>
	struct _impl_vscprintf<char> {
		GSL_SUPPRESS(f.55) _Check_return_ const int operator()(_In_z_ _Printf_format_string_ const char* const fmt, _In_ const va_list args) noexcept
		{
			return _vscprintf(fmt, args);
		}
	};
	template <>
	struct _impl_vscprintf<wchar_t> {
		GSL_SUPPRESS(f.55) _Success_(return >= 0) _Check_return_ const int operator()(_In_z_ _Printf_format_string_ const wchar_t* const fmt, _In_ const va_list args) noexcept
		{
			return _vscwprintf(fmt, args);
		}
	};

	template <typename T>
	struct _impl_vsprintf {
	};
	template <>
	struct _impl_vsprintf<char> {
		GSL_SUPPRESS(f.55) _Success_(return >= 0) _Check_return_opt_ const int operator()(_Out_writes_opt_(nCount) _Always_(_Post_z_) char* const buf, _In_ size_t const nCount, _In_z_ _Printf_format_string_ const char* const fmt, _In_ const va_list args) noexcept
		{
			return vsnprintf(buf, nCount, fmt, args);
		}
	};
	template <>
	struct _impl_vsprintf<wchar_t> {
		GSL_SUPPRESS(f.55) _Success_(return >= 0) _Check_return_opt_ const int operator()(_Out_writes_opt_(nCount) _Always_(_Post_z_) wchar_t* const buf, _In_ size_t const nCount, _In_z_ _Printf_format_string_ const wchar_t* const fmt, _In_ const va_list args) noexcept
		{
			return vswprintf(buf, nCount, fmt, args);
		}
	};

	template <typename T, typename Format, typename... Arguments>
	struct _impl_snprintf {
	};
	template <typename... Arguments>
	struct _impl_snprintf<char, char, Arguments...> {
		_Success_(return >= 0) _Check_return_opt_ const int operator()(_Out_writes_opt_(nCount) _Always_(_Post_z_) char* const buf, _In_ const size_t nCount, _In_z_ _Printf_format_string_ const char* const fmt, _In_ const Arguments&&... args) noexcept
		{
			return snprintf(buf, nCount, fmt, args...);
		}
	};
	template <typename... Arguments>
	struct _impl_snprintf<wchar_t, wchar_t, Arguments...> {
		_Success_(return >= 0) _Check_return_opt_ const int operator()(_Out_writes_opt_(nCount) _Always_(_Post_z_) wchar_t* const buf, _In_ const size_t nCount, _In_z_ _Printf_format_string_ const wchar_t* const fmt, _In_ const Arguments&&... args) noexcept
		{
			return _snwprintf(buf, nCount, fmt, args...);
		}
	};

	template <HasDataSizeNoChr T, typename... Arguments>
	struct _impl_snprintf<T, typename T::value_type, Arguments...> {
		_Success_(return >= 0) _Check_return_opt_ const int operator()(_Out_ T& buf, _In_z_ _Printf_format_string_ const typename T::value_type* const fmt, _In_ const Arguments&&... args) noexcept
		{
			if constexpr (std::is_same_v<char, typename T::value_type>)
				return snprintf(buf.data(), buf.size(), fmt, args...);
			else
				return _snwprintf(buf.data(), buf.size(), fmt, args...);
		}
	};
	template <HasCapacityChr T, typename... Arguments>
	struct _impl_snprintf<T, typename T::value_type, Arguments...> {
		_Success_(return >= 0) _Check_return_opt_ const int operator()(_Out_ T& buf, _In_z_ _Printf_format_string_ const typename T::value_type* const fmt, _In_ const Arguments&&... args) noexcept
		{
			if constexpr (std::is_same_v<char, typename T::value_type>)
				return snprintf(buf.to_chr(), buf.capacity_cch(), fmt, args...);
			else
				return _snwprintf(buf.to_chr(), buf.capacity_cch(), fmt, args...);
		}
	};

	template <typename T>
	struct _impl_atoi {
	};
	template <>
	struct _impl_atoi<char> {
		auto operator()(_In_z_ const char* const buf) noexcept
		{
			return atoi(buf);
		}
	};
	template <>
	struct _impl_atoi<wchar_t> {
		auto operator()(_In_z_ const wchar_t* const buf) noexcept
		{
			return _wtoi(buf);
		}
	};

	template <typename T>
	struct _impl_atoi64 {
	};
	template <>
	struct _impl_atoi64<char> {
		auto operator()(_In_z_ const char* const buf) noexcept
		{
			return _atoi64(buf);
		}
	};
	template <>
	struct _impl_atoi64<wchar_t> {
		auto operator()(_In_z_ const wchar_t* const buf) noexcept
		{
			return _wtoi64(buf);
		}
	};

	template <typename T>
	struct _impl_atof {
	};
	template <>
	struct _impl_atof<char> {
		auto operator()(_In_z_ const char* const buf) noexcept
		{
			return atof(buf);
		}
	};
	template <>
	struct _impl_atof<wchar_t> {
		auto operator()(_In_z_ const wchar_t* const buf) noexcept
		{
			return _wtof(buf);
		}
	};

	template <typename T>
	struct _impl_itoa {
	};
	template <>
	struct _impl_itoa<char> {
		auto operator()(_In_ const int val, _Pre_notnull_ _Post_z_ char* const buf, _In_ const int radix) noexcept
		{
			return _itoa(val, buf, radix);
		}
	};
	template <>
	struct _impl_itoa<wchar_t> {
		auto operator()(_In_ const int val, _Pre_notnull_ _Post_z_ wchar_t* const buf, _In_ const int radix) noexcept
		{
			return _itow(val, buf, radix);
		}
	};

	template <typename T>
	struct _impl_strtoul {
	};
	template <>
	struct _impl_strtoul<char> {
		auto operator()(_In_z_ const char* const buf, _Out_opt_ _Deref_post_z_ char** endptr, _In_ int radx) noexcept
		{
			return strtoul(buf, endptr, radx);
		}
	};
	template <>
	struct _impl_strtoul<wchar_t> {
		auto operator()(_In_z_ const wchar_t* const buf, _Out_opt_ _Deref_post_z_ wchar_t** endptr, _In_ int radx) noexcept
		{
			return wcstoul(buf, endptr, radx);
		}
	};

	/// <summary>
	/// Convert a string representation of a binary number to an unsigned long
	/// </summary>
	/// <typeparam name="T">- char or wchar_t</typeparam>
	template <typename T>
	struct _impl_bstrtoul {
	};
	template <>
	struct _impl_bstrtoul<char> {
		auto operator()(_In_z_ const char* const buf) noexcept
		{
			return strtoul(buf, nullptr, 2);
		}
	};
	template <>
	struct _impl_bstrtoul<wchar_t> {
		auto operator()(_In_z_ const wchar_t* const buf) noexcept
		{
			return wcstoul(buf, nullptr, 2);
		}
	};
	template <typename T>
	struct _impl_strtok {
	};
	template <>
	struct _impl_strtok<char> {
		auto operator()(_Inout_opt_z_ char* const buf, _In_z_ const char* delim, _Inout_ _Deref_prepost_opt_z_ char** contex) noexcept
		{
			return strtok_s(buf, delim, contex);
		}
	};

	template <>
	struct _impl_strtok<wchar_t> {
		auto operator()(_Inout_opt_z_ wchar_t* const buf, _In_z_ const wchar_t* delim, _Inout_ _Deref_prepost_opt_z_ wchar_t** contex) noexcept
		{
			return wcstok_s(buf, delim, contex);
		}
	};
}

#if 0 && __has_include(<ranges>) && __has_include(<algorithm>) && __has_include(<string>)
#include <ranges>
#include <algorithm>
//#include <string>

template <details::IsPODText T>
bool iequals(const std::basic_string_view<T>& lhs, const std::basic_string_view<T>& rhs)
{
	auto to_lower{ std::ranges::views::transform(std::tolower) };
	return std::ranges::equal(lhs | to_lower, rhs | to_lower);
}
#else
template <details::IsPODText T>
bool iequals(const std::basic_string<T>& a, const std::basic_string<T>& b)
{
	const std::locale loc("");	// needs to be ""
	return std::equal(a.begin(), a.end(),
		b.begin(), b.end(),
		[loc](T a, T b) noexcept {
			return std::tolower<T>(a, loc) == std::tolower<T>(b, loc);
		});
}

//bool iequals(const std::string& a, const std::string& b)
//{
//	const unsigned int sz = a.size();
//	if (b.size() != sz)
//		return false;
//	for (unsigned int i = 0; i < sz; ++i)
//		if (tolower(a[i]) != tolower(b[i]))
//			return false;
//	return true;
//}
//

#endif

// Check string bounds, make sure dest is not within the source string & vice versa (this could be a possible reason for some strcpyn() fails we see)
template <details::IsPODText T>
constexpr bool isInBounds(_In_ const T* const sDest, _In_ const T* const sSrc, _In_ const std::size_t iLen) noexcept
{
	return (sSrc >= sDest && sSrc <= (sDest + iLen)) || (sDest >= sSrc && sDest <= (sSrc + iLen));
}

template <typename Result, typename Format, typename Value, typename... Arguments>
Result& _ts_sprintf(_Inout_ Result& res, _In_ const Format& fmt, _In_ const Value& val, _In_ Arguments&&... args)
{
	static_assert(details::IsPODText<Format>, "Format string must be char or wchar_t");
	res.clear();
	return details::_ts_printf_do(res, fmt, val, args...);
}

template <details::IsPODText T>
T* _ts_strstr(T* input, const std::remove_const_t<T>* find)
{
	return details::impl_strstr<T>()(input, find);
}

// Get String length
template <typename T>
inline auto _ts_strlen(_In_ const T& str) noexcept
{
	return details::_ts_strlen(str);
}

// Get String length (with buffer size limit)
template <details::IsPODText T>
inline size_t _ts_strnlen(_In_ const T& str, _In_ const size_t& nBufSize) noexcept
{
	return details::_impl_strnlen<T>()(str, nBufSize);
}

// Test if a string is empty, works for any object that has an empty() member function as well as C strings.
template <typename T>
inline bool _ts_isEmpty(_In_ const T& str) noexcept
{
	return details::_ts_isEmpty(str);
}

// Finds a string or character within a string & returns a pointer to it.
template <details::IsPODText HayStack, details::IsPODText Needle>
inline HayStack* _ts_find(_In_ HayStack* const str, _In_ Needle srch) noexcept
{
	return const_cast<HayStack*>(details::_impl_ts_find<Needle>()(str, srch));
}
template <details::IsPODText HayStack, details::IsPODText Needle>
inline const HayStack* _ts_find(_In_ const HayStack* const str, _In_ Needle srch) noexcept
{
	return details::_impl_ts_find<Needle>()(str, srch);
}

// Finds a string literal within a string & returns a pointer to it.
template <typename T, size_t N>
inline T* _ts_find(_In_ T* const str, _In_reads_or_z_(N) const T(&srch)[N]) noexcept
{
	return const_cast<T*>(details::_impl_ts_find<std::add_pointer_t<T>>()(str, &srch[0]));
}

template <details::IsPODText T>
T* _ts_strcpyn(_Out_writes_(iChars) T* const sDest, _In_reads_or_z_(iChars) const T* const sSrc, _In_ const size_t iChars) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	//#if DCX_DEBUG_OUTPUT
	//	if ((sDest == nullptr) || (sSrc == nullptr) || isInBounds<std::remove_cv_t<T> >(sDest, sSrc, iChars))
	//		return nullptr;
	//#else
	if ((!sDest) || (!sSrc))
		return nullptr;
	//#endif

	return details::_impl_strcpyn<T>()(sDest, sSrc, iChars);
}

template <details::IsPODText T>
T* _ts_strcpy(_Out_writes_z_(_String_length_(sSrc) + 1) T* const sDest, _In_z_ const T* const sSrc) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	if ((!sDest) || (!sSrc))
		return nullptr;

	return details::_impl_strcpy<T>()(sDest, sSrc);
}

template <details::IsPODText T>
T* _ts_strncat(_Inout_updates_z_(iChars) T* const sDest, _In_reads_or_z_(iChars) const T* const sSrc, _In_ const size_t iChars) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strncat<T>()(sDest, sSrc, iChars);
}

template <details::IsPODText T>
T* _ts_strcat(_Inout_updates_z_(_String_length_(sDest) + _String_length_(sSrc) + 1) T* const sDest, _In_z_ const T* const sSrc) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strcat<T>()(sDest, sSrc);
}

template <details::IsPODText T>
T* _ts_strchr(_In_z_ T* const sDest, _In_ const T ch)
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strchr<T>()(sDest, ch);
}

template <details::IsPODText T>
int _ts_strncmp(_In_reads_or_z_(iChars) const T* const sDest, _In_reads_or_z_(iChars) const T* const sSrc, _In_ const size_t iChars) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strncmp<T>()(sDest, sSrc, iChars);
}

template <details::IsPODText T>
int _ts_strnicmp(_In_reads_or_z_(iChars) const T* const sDest, _In_reads_or_z_(iChars) const T* const sSrc, _In_ const size_t iChars) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strnicmp<T>()(sDest, sSrc, iChars);
}

template <details::IsPODText T>
int _ts_strcmp(_In_z_ const T* const sDest, _In_z_ const T* const sSrc) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strcmp<T>()(sDest, sSrc);
}

template <details::IsPODText T>
int _ts_stricmp(_In_z_ const T* const sDest, _In_z_ const T* const sSrc) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_stricmp<T>()(sDest, sSrc);
}

template <details::IsPODText T>
_Check_return_ int _ts_vscprintf(_In_z_ _Printf_format_string_ const T* const _Format, _In_ va_list _ArgList) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_vscprintf<T>()(_Format, _ArgList);
}

template <details::IsPODText T>
_Success_(return >= 0) _Check_return_opt_ int _ts_vsprintf(_Out_writes_opt_(nCount) _Always_(_Post_z_) T* const buf, _In_ size_t nCount, _In_z_ _Printf_format_string_ const T* const fmt, const va_list args) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_vsprintf<T>()(buf, nCount, fmt, args);
}

template <details::IsPODText T, typename Format, typename... Arguments>
_Success_(return >= 0) _Check_return_opt_ int _ts_snprintf(_Out_writes_opt_(nCount) _Always_(_Post_z_) T* const buf, _In_ const size_t nCount, _In_z_ _Printf_format_string_ const Format* const fmt, _In_ Arguments&&... args) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");
	static_assert(std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<Format>>, "Buffer & format must have same type.");

	return details::_impl_snprintf<T, Format, Arguments...>()(buf, nCount, fmt, std::forward<Arguments>(args)...);
}

template <typename T, details::IsPODText Format, typename... Arguments>
_Success_(return >= 0) _Check_return_opt_ int _ts_snprintf(_Out_ T& buf, _In_z_ _Printf_format_string_ const Format* const fmt, _In_ Arguments&&... args) noexcept
{
	static_assert(details::IsPODText<Format>, "Only char & wchar_t supported...");

	return details::_impl_snprintf<T, Format, Arguments...>()(buf, fmt, std::forward<Arguments>(args)...);
}

template <details::IsPODText T>
auto _ts_atoi(_In_z_ const T* const buf) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_atoi<T>()(buf);
}

template <details::IsPODText T>
auto _ts_atoi64(_In_z_ const T* const buf) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_atoi64<T>()(buf);
}

template <details::IsPODText T>
auto _ts_atof(_In_z_ const T* const buf) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_atof<T>()(buf);
}

template <details::IsPODText T>
auto _ts_itoa(_In_ const int val, _Pre_notnull_ _Post_z_ T* const buf, _In_ const int radix) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_itoa<T>()(val, buf, radix);
}

template <details::IsPODText T>
auto _ts_strtoul(_In_z_ const T* const buf, _Out_opt_ _Deref_post_z_ T** endptr, _In_ int base) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strtoul<T>()(buf, endptr, base);
}

/// <summary>
/// Convert a string representation of a binary number to an unsigned long
/// </summary>
/// <param name="buf">- String representing a binary number.</param>
/// <returns>Binary number</returns>
template <details::IsPODText T>
auto _ts_bstrtoul(_In_z_ const T* const buf) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_bstrtoul<T>()(buf);
}

/// <summary>
/// same as strtok_s()
/// </summary>
/// <param name="buf"></param>
/// <param name="delim"></param>
/// <param name="contex"></param>
/// <returns></returns>
template <details::IsPODText T>
auto _ts_strtok(_Inout_opt_z_ T* const buf, _In_z_ const T* delim, _Inout_ _Deref_prepost_opt_z_ T** contex) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strtok<T>()(buf, delim, contex);
}

/// <summary>
/// Converts a number into a binary string representing that number (whole integers only)
/// </summary>
/// <typeparam name="Result">A string type object</typeparam>
/// <param name="a">- The number to convert.</param>
/// <returns>The string representation of the number.</returns>
template <details::IsNumeric T, class Result>
Result DecimalToBinaryString(_In_ const T& a)
{
	Result binary;
	constexpr auto sz = sizeof(a) * 8;
	std::remove_const_t<T> mask = 1;
	for (std::remove_const_t<decltype(sz)> i = 0; i < sz; ++i)
	{
		if ((mask & a))
			binary = '1' + binary;
		else
			binary = '0' + binary;
		mask <<= 1;
	}
	return binary;
}

/// <summary>
/// Replace all instances of 'rep' in 'str' with 'with'
/// </summary>
/// <typeparam name="T"></typeparam>
/// <typeparam name="RepThis"></typeparam>
/// <typeparam name="WithThis"></typeparam>
/// <param name="str">- string to search</param>
/// <param name="rep">- string to search for</param>
/// <param name="with">- replacement string</param>
/// <returns>The string it was passed with the changes made.</returns>
template <class T, class RepThis, class WithThis>
T& _ts_replace(_Inout_ T& str, _In_ RepThis rep, _In_ WithThis with)
{
	for (std::size_t wlen = _ts_strlen(with), rlen = _ts_strlen(rep), pos = 0U; ; pos += wlen)
	{
		pos = str.find(rep, pos);
		if (pos == T::npos)
			break;

		if constexpr (std::is_same_v<char, std::remove_cv_t<WithThis>> || std::is_same_v<wchar_t, std::remove_cv_t<WithThis>>)
		{
			str.replace(pos, rlen, 1, with);
		}
		else
		{
			str.replace(pos, rlen, with);
		}
	}
	return str;
}

/// <summary>
/// Replace all instances of each character in 'rep' in 'str' with 'with'
/// </summary>
/// <typeparam name="T"></typeparam>
/// <typeparam name="RepThis"></typeparam>
/// <typeparam name="WithThis"></typeparam>
/// <param name="str">- string to search</param>
/// <param name="rep">- string of characters to search for</param>
/// <param name="with">- replacement string</param>
/// <returns>The string it was passed with the changes made.</returns>
template <class T, class RepThis, class WithThis>
T& _ts_mreplace(_Inout_ T& str, _In_ RepThis rep, _In_ WithThis with)
{
	for (std::ptrdiff_t i = 0; rep[i] != 0; ++i)
		_ts_replace(str, rep[i], with);
	return str;
}

/// <summary>
/// Remove all instances of 'rep' within 'str'
/// </summary>
/// <typeparam name="T"></typeparam>
/// <typeparam name="RemThis"></typeparam>
/// <param name="str">- string to search.</param>
/// <param name="rep">- string to remove.</param>
/// <returns>The string it was passed with the changes made.</returns>
template <class T, class RemThis>
T& _ts_remove(_Inout_ T& str, _In_ RemThis rep)
{
	for (auto itEnd = str.end(), itStart = str.begin(); str.erase(std::find(itStart, itEnd, rep)) != itEnd; ) {}

	return str;
}

/// <summary>
/// Remove spaces from the front and back of the string.
/// </summary>
/// <typeparam name="T">A string type object</typeparam>
/// <param name="str">- The string to trim.</param>
/// <returns>A reference to the same string object it was passed.</returns>
template <class T>
T& _ts_trim(_Inout_ T& str)
{
	if (str.empty())
		return str;

	while (str.front() == _T(' '))
		str.erase(0, 1);

	while (str.back() == _T(' '))
		str.pop_back();

	return str;
}

/// <summary>
/// Copies the string then remove spaces from the front and back of the string.
/// </summary>
/// <typeparam name="T">A string type object</typeparam>
/// <param name="str">- The string to trim.</param>
/// <returns>A copy of the string object it was passed.</returns>
template <class T>
T _ts_trim_and_copy(_In_ const T& str)
{
	T res(str);

	return _ts_trim(res);
}

/// <summary>
/// Trim the string and remove double spaces.
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="str">- string to modify.</param>
/// <returns>The string it was passed with the changes made.</returns>
template <class T>
T& _ts_strip(_Inout_ T& str)
{
	_ts_remove(str, _T('\x02'));	//02
	_ts_remove(str, _T('\x0F'));	//15
	_ts_remove(str, _T('\x16'));	//22
	_ts_remove(str, _T('\x1D'));	//29

	// NB: TODO: add ctrl-k removal for colour codes.
	//for (auto itEnd = str.end(), itStart = str.begin(), itGet = std::find(itStart, itEnd, _T('\x03')); itGet != itEnd; itGet = std::find(itStart, itEnd, _T('\x03')))
	//{
	//	str.erase(itGet);
	//}

	_ts_replace(str, _T("  "), _T(' '));

	return _ts_trim(str);
}

/// <summary>
/// Trim the string and remove double spaces.
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="str">- string to modify.</param>
/// <returns>A copy of the string it was passed with the changes made.</returns>
template <class T>
T _ts_strip_and_copy(_In_ const T& str)
{
	T tmp(str);

	return _ts_strip(tmp);
}

/// <summary>
/// Change a string to uppercase.
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="str">- string to modify.</param>
/// <returns>The string it was passed with the changes made.</returns>
template <class T>
T& _ts_toupper(_Inout_ T& str)
{
	for (auto& a : str)
		a = details::make_upper(a);
	return str;
}

template <details::IsPODText T>
T _ts_toupper_c(_In_ const T& c) noexcept
{
	return details::make_upper(c);
}

template <details::IsPODText T>
bool _ts_isupper(_In_ const T& c) noexcept
{
	if constexpr (std::is_same_v<wchar_t, T>)
		return (iswupper(c) != 0);
	else
		return (isupper(c) != 0);
}

/// <summary>
/// Convert a string to a number.
/// </summary>
/// <typeparam name="T"></typeparam>
/// <typeparam name="Input"></typeparam>
/// <param name="str">- string to convert.</param>
/// <returns>The number contained in the string.</returns>
template <class T, class Input>
T _ts_to_(_In_ const Input& str)
{
	static_assert(is_Numeric_v<T>, "Type T must be (int, long, float, double, ....)");

#ifdef STRINGSUPPORT_STRTONUM
	if (!str)
		return 0;

	return stn::StrToNum<T>(str).value_or(0); // no copy made.
#else
	std::basic_istringstream<TCHAR> ss(str);	// makes copy of string :(
	T result{};
	return ss >> result ? result : T();
#endif
}

#define STRINGSUPPORT_WILDT 0
#define STRINGSUPPORT_WILDA 0
#define STRINGSUPPORT_WILDE 0
#define STRINGSUPPORT_WILDW 0

/// <summary>
/// Internal wildcard match function.
/// </summary>
/// <typeparam name="_Wild"></typeparam>
/// <typeparam name="TameString"></typeparam>
/// <typeparam name="WildString"></typeparam>
/// <param name="pszString"></param>
/// <param name="pszMatch"></param>
/// <param name="bCase"></param>
/// <returns></returns>
template <class _Wild, typename TameString, typename WildString>
GSL_SUPPRESS(bounds.4) bool _ts_InnerWildcardMatch(_In_ const TameString& pszString, _In_ const WildString& pszMatch, _In_ const bool bCase) noexcept(std::is_nothrow_move_assignable_v<WildString>)
{
	ptrdiff_t MatchPlaceholder = 0;
	ptrdiff_t StringPlaceholder = 0;
	ptrdiff_t iWildOffset = 0;
	ptrdiff_t iTameOffset = 0;

	constexpr _Wild _zero{};
	constexpr _Wild _star = static_cast<_Wild>('*');
	constexpr _Wild _question = static_cast<_Wild>('?');
#if STRINGSUPPORT_WILDT || STRINGSUPPORT_WILDW
	constexpr _Wild _space = static_cast<_Wild>(' ');
#endif
#if STRINGSUPPORT_WILDT
	constexpr _Wild _tilda = static_cast<_Wild>('~');
#endif
#if STRINGSUPPORT_WILDE
	constexpr _Wild _slash = static_cast<_Wild>('\\');
#endif
#if STRINGSUPPORT_WILDA
	constexpr _Wild _power = static_cast<_Wild>('^');
#endif
#if STRINGSUPPORT_WILDW
	constexpr _Wild _hash = static_cast<_Wild>('#');
	constexpr _Wild _tab = static_cast<_Wild>('\t');
	constexpr _Wild _n = static_cast<_Wild>('\n');
	constexpr _Wild _r = static_cast<_Wild>('\r');
#endif

	while (pszString[iTameOffset])
	{
		if (pszMatch[iWildOffset] == _star)
		{
			if (pszMatch[++iWildOffset] == _zero)
				return true;
			MatchPlaceholder = iWildOffset;
			StringPlaceholder = iTameOffset + 1;
		}
#if STRINGSUPPORT_WILDT
		else if (pszMatch[iWildOffset] == _tilda && pszString[iTameOffset] == _space)
		{
			++iWildOffset;
			while (pszString[iTameOffset] == _space)
				++iTameOffset;
		}
#endif
#if STRINGSUPPORT_WILDA
		else if (pszMatch[iWildOffset] == _power)
		{
			++iWildOffset;
			if (details::CompareChar(pszMatch[iWildOffset], pszString[iTameOffset], bCase))
				++iTameOffset;
			++iWildOffset;
		}
#endif
#if STRINGSUPPORT_WILDE
		else if (pszMatch[iWildOffset] == _slash)
		{
			// any character following a '\' is taken as a literal character.
			++iWildOffset;
			if (!details::CompareChar(pszMatch[iWildOffset], pszString[iTameOffset], bCase))
				return false;
			++iTameOffset;
		}
#endif
#if STRINGSUPPORT_WILDW
		else if (pszMatch[iWildOffset] == _hash)
		{
			++iWildOffset;
			while (pszString[iTameOffset] && (pszString[iTameOffset] != _space || pszString[iTameOffset] != _tab || pszString[iTameOffset] != _n || pszString[iTameOffset] != _r))
				++iTameOffset;
		}
#endif
		//else if (pszMatch[iWildOffset] == _question || _toupper(pszMatch[iWildOffset]) == _toupper(pszString[iTameOffset]))
		else if (pszMatch[iWildOffset] == _question || details::CompareChar(pszMatch[iWildOffset], pszString[iTameOffset], bCase))
		{
			++iWildOffset;
			++iTameOffset;
		}
		else if (StringPlaceholder == 0)
			return false;
		else
		{
			iWildOffset = MatchPlaceholder;
			iTameOffset = StringPlaceholder++;
		}
	}

	while (pszMatch[iWildOffset] == _star)
		++iWildOffset;

	return !pszMatch[iWildOffset];
}

/// <summary>
/// Wildcard match
/// </summary>
/// <typeparam name="TameString"></typeparam>
/// <typeparam name="WildString"></typeparam>
/// <param name="pszString">- string to search</param>
/// <param name="pszMatch">- the wildcard string to search for.</param>
/// <param name="bCase">- does case matter</param>
/// <returns>did match succeed true/false</returns>
template <typename TameString, typename WildString>
GSL_SUPPRESS(bounds) bool _ts_WildcardMatch(_In_ const TameString& pszString, _In_ const WildString& pszMatch, _In_ const bool bCase = false) noexcept(std::is_nothrow_move_assignable_v<WildString>)
{
	if (_ts_isEmpty(pszMatch) || _ts_isEmpty(pszString))
		return false;

	if constexpr (details::is_pod_v<std::remove_pointer_t<std::remove_cv_t<std::remove_all_extents_t<WildString>>>>)
		return _ts_InnerWildcardMatch<std::remove_pointer_t<std::remove_cv_t<std::remove_all_extents_t<WildString>>>>(pszString, pszMatch, bCase);
	else
		return _ts_InnerWildcardMatch<std::remove_pointer_t<std::remove_cv_t<std::remove_all_extents_t<typename WildString::value_type>>>>(pszString, pszMatch, bCase);
}

template <typename T> T* _ts_AddMem(_Inout_ std::vector<std::shared_ptr<std::vector<char>>>& mem, _In_ std::size_t sz = sizeof(T))
{
	std::shared_ptr<std::vector<char>> x = std::make_shared<std::vector<char>>();
	x->resize(sz);
	mem.push_back(x);
	T* d = (T*)mem[mem.size() - 1].get()->data();
	return d;
}

/// <summary>
/// Sort a vector
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="str">- The vector to sort.</param>
/// <returns>A sorted copy of the input.</returns>
template <class T>
std::vector<T> _ts_SortString(_In_ const std::vector<T>& str)
{
	std::vector<T> out(str);

	std::sort(out.begin(), out.end(), std::less<T>);

	return out;
}
