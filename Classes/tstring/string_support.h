#pragma once
// support functions for TString & c-string handling...
// v1.12

#include <tchar.h>
#include <stdlib.h>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#include <concepts>

#include <gsl/gsl>

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

	constexpr WCHAR make_upper(const WCHAR c) noexcept
	{
		if (c >= L'a' && c <= L'z') return _toupper(c);
		return c;
	}
	constexpr char make_upper(const char c) noexcept
	{
		if (c >= 'a' && c <= 'z') return _toupper(c);
		return c;
	}
	constexpr bool CompareChar(const WCHAR c, const WCHAR other, const bool bCase) noexcept
	{
		return (bCase) ? c == other : make_upper(c) == make_upper(other);
	}
	constexpr bool CompareChar(const char c, const char other, const bool bCase) noexcept
	{
		return (bCase) ? c == other : make_upper(c) == make_upper(other);
	}

	template <typename Result, typename Format>
	Result& _ts_printf_do(Result& res, const Format& fmt)
	{
		res += fmt;
		return res;
	}

	template <typename Result, typename Format, typename Value, typename... Arguments>
	Result& _ts_printf_do(Result& res, const Format& fmt, const Value& val, Arguments&&... args)
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

		//// Ook: needs checked may need work
		//for (const auto &c: fmt)
		//{
		//	if (!bSkip)
		//	{
		//		if (c == decltype(c){'\\'})
		//		{
		//			bSkip = true;
		//			continue;
		//		}
		//		else if (c == decltype(c){'%'})
		//		{
		//			if constexpr (is_Numeric_v<Value>)
		//			{
		//				if constexpr (std::is_same_v<std::string, std::remove_cv_t<Result>>)
		//					res += std::to_string(val);
		//				else if constexpr (std::is_same_v<std::wstring, std::remove_cv_t<Result>>)
		//					res += std::to_wstring(val);
		//				else
		//					res += val;	// assumes this type can handle adding numbers.
		//			}
		//			else
		//				res += val;
		//			if constexpr (std::is_array_v<Format> && std::is_pod_v<Format>)
		//				return _ts_printf_do(res, &fmt[0] + i + 1, args...);
		//			else
		//				return _ts_printf_do(res, fmt + i + 1, args...);
		//		}
		//	}
		//	else
		//		bSkip = false;
		//	res += c;
		//}
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

	//// Test if a string is empty, works for std::basic_string & TString objects
	//template <typename T>
	//std::enable_if_t<!std::is_pointer_v<T> && !std::is_pod_v<T> && std::is_member_function_pointer_v<decltype(&T::empty)>, bool> _ts_isEmpty(const T &str) noexcept
	//{
	//	return str.empty();
	//}
	//
	//// Test if a string is empty, works for C String char * or wchar_t *
	//template <typename T>
	//std::enable_if_t<std::is_pointer_v<T>, bool> _ts_isEmpty(const T &str) noexcept
	//{
	//	using value_type = std::remove_cv_t<std::remove_pointer_t<T> >;
	//
	//	static_assert(std::is_same_v<value_type, char> || std::is_same_v<value_type, wchar_t>, "Invalid Type used");
	//	return ((str == nullptr) || (str[0] == value_type()));
	//}
	//
	//// Test if a string is empty, works for C char or wchar_t
	//template <typename T>
	//std::enable_if_t<!std::is_pointer_v<T> && std::is_pod_v<T>, bool> _ts_isEmpty(const T &str) noexcept
	//{
	//	using value_type = std::remove_cv_t<T>;
	//
	//	static_assert(std::is_same_v<value_type, char> || std::is_same_v<value_type, wchar_t>, "Invalid Type used");
	//	return (str == value_type());
	//}

	template <typename T>
	bool _ts_isEmpty(const T& str) noexcept
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

	//// Get String length
	//template <typename T, typename size_type = std::size_t>
	//std::enable_if_t<std::is_pointer_v<T>, size_type> _ts_strlen(const T &str) noexcept
	//{
	//	using value_type = std::remove_cv_t<std::remove_pointer_t<T> >;
	//
	//	static_assert(std::is_same_v<value_type, char> || std::is_same_v<value_type, wchar_t>, "Invalid Type used");
	//	auto iLen = size_type();
	//	for (auto p = str; p && *p; ++p)
	//		++iLen;
	//	return iLen;
	//}
	//
	//template <typename T, typename size_type = std::size_t>
	//constexpr inline std::enable_if_t<!std::is_pointer_v<T> && std::is_same_v<std::remove_cv_t<T>, wchar_t>, size_type> _ts_strlen(const T &str) noexcept
	//{
	//	return (str == T() ? 0U : 1U);
	//}
	//
	//template <typename T, typename size_type = std::size_t>
	//constexpr inline std::enable_if_t<!std::is_pointer_v<T> && std::is_same_v<std::remove_cv_t<T>, char>, size_type> _ts_strlen(const T &str) noexcept
	//{
	//	return (str == T() ? 0U : 1U);
	//}
	//
	//template <typename T, typename size_type = std::size_t, std::size_t N>
	//constexpr inline size_type _ts_strlen(T const (&)[N]) noexcept
	//{
	//	return (N == 0U ? 0U : N - 1);
	//}
	//
	//template <typename T, typename size_type = T::size_type>
	//inline std::enable_if_t<!std::is_pointer_v<T> && std::is_member_function_pointer_v<decltype(&T::length)>, size_type> _ts_strlen(const T &str)
	//{
	//	return str.length();
	//}

	// Get String length
	template <typename T, typename size_type = std::size_t>
	constexpr size_type _ts_strlen(const T& str) noexcept
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

	//template <typename T, typename size_type = std::size_t, std::size_t N>
	//constexpr inline size_type _ts_strlen(T const (&)[N]) noexcept
	//{
	//	// T is a fixed array
	//	return (N == 0U ? 0U : N - 1);
	//}

	template <IsPODText T, typename size_type = std::size_t, std::size_t N>
	constexpr inline size_type _ts_strlen(T const (&)[N]) noexcept
	{
		// T is a fixed array
		return (N == 0U ? 0U : N - 1);
	}

	template <typename T>
	struct _impl_strnlen {
	};
	template <>
	struct _impl_strnlen<char*> {
		size_t operator()(const char* const ptr, size_t length) noexcept
		{
			return strnlen(ptr, length);
		}
	};
	template <>
	struct _impl_strnlen<wchar_t*> {
		size_t operator()(const wchar_t* const ptr, size_t length) noexcept
		{
			return wcsnlen(ptr, length);
		}
	};
	template <>
	struct _impl_strnlen<char> {
		size_t operator()(const char& ptr, size_t length) noexcept
		{
			return (ptr == 0 || length == 0 ? 0U : 1U);
		}
	};
	template <>
	struct _impl_strnlen<wchar_t> {
		size_t operator()(const wchar_t& ptr, size_t length) noexcept
		{
			return (ptr == 0 || length == 0 ? 0U : 1U);
		}
	};

	template <typename T>
	struct _impl_strcpyn {
	};
	template <>
	struct _impl_strcpyn<char> {
		char* operator()(char* const pDest, const char* const pSrc, const size_t length) noexcept
		{
			//auto t = strncpy(pDest, pSrc, length);	// doesn't guarantee NULL termination!
			//if (t)
			//	t[length - 1] = 0;	// make sure it ends in NULL
			//return t;

			return lstrcpynA(pDest, pSrc, gsl::narrow_cast<int>(length));
		}
	};
	template <>
	struct _impl_strcpyn<wchar_t> {
		wchar_t* operator()(wchar_t* const pDest, const wchar_t* const pSrc, const size_t length) noexcept
		{
			//auto t = wcsncpy(pDest, pSrc, length); // doesn't guarantee NULL termination!
			//if (t)
			//	t[length - 1] = 0;	// make sure it ends in NULL
			//return t;

			return lstrcpynW(pDest, pSrc, gsl::narrow_cast<int>(length));
		}
	};

	template <typename T>
	struct _impl_strcpy {
	};
	template <>
	struct _impl_strcpy<char> {
		char* operator()(char* const pDest, const char* const pSrc) noexcept
		{
			return strcpy(pDest, pSrc);
		}
	};
	template <>
	struct _impl_strcpy<wchar_t> {
		wchar_t* operator()(wchar_t* const pDest, const wchar_t* const pSrc) noexcept
		{
			return wcscpy(pDest, pSrc);
		}
	};

	template <typename T>
	struct _impl_strncat {
	};
	template <>
	struct _impl_strncat<char> {
		char* operator()(char* const pDest, const char* const pSrc, const size_t length) noexcept
		{
			return strncat(pDest, pSrc, length);
		}
	};
	template <>
	struct _impl_strncat<wchar_t> {
		wchar_t* operator()(wchar_t* const pDest, const wchar_t* const pSrc, const size_t length) noexcept
		{
			return wcsncat(pDest, pSrc, length);
		}
	};

	template <typename T>
	struct _impl_strcat {
	};
	template <>
	struct _impl_strcat<char> {
		char* operator()(char* const pDest, const char* const pSrc) noexcept
		{
			return strcat(pDest, pSrc);
		}
	};
	template <>
	struct _impl_strcat<wchar_t> {
		wchar_t* operator()(wchar_t* const pDest, const wchar_t* const pSrc) noexcept
		{
			return wcscat(pDest, pSrc);
		}
	};

	template <typename T>
	struct _impl_strncmp {
	};
	template <>
	struct _impl_strncmp<char> {
		int operator()(const char* const pDest, const char* const pSrc, const size_t length) noexcept
		{
			return strncmp(pDest, pSrc, length);
		}
	};
	template <>
	struct _impl_strncmp<wchar_t> {
		int operator()(const wchar_t* const pDest, const wchar_t* const pSrc, const size_t length) noexcept
		{
			return wcsncmp(pDest, pSrc, length);
		}
	};

	template <typename T>
	struct _impl_strnicmp {
	};
	template <>
	struct _impl_strnicmp<char> {
		int operator()(const char* const pDest, const char* const pSrc, const size_t length) noexcept
		{
			return _strnicmp(pDest, pSrc, length);
		}
	};
	template <>
	struct _impl_strnicmp<wchar_t> {
		int operator()(const wchar_t* const pDest, const wchar_t* const pSrc, const size_t length) noexcept
		{
			return _wcsnicmp(pDest, pSrc, length);
		}
	};

	template <typename T>
	struct _impl_strcmp {
	};
	template <>
	struct _impl_strcmp<char> {
		int operator()(const char* const pDest, const char* const pSrc) noexcept
		{
			return strcmp(pDest, pSrc);
		}
	};
	template <>
	struct _impl_strcmp<wchar_t> {
		int operator()(const wchar_t* const pDest, const wchar_t* const pSrc) noexcept
		{
			return wcscmp(pDest, pSrc);
		}
	};

	template <typename T>
	struct _impl_stricmp {
	};
	template <>
	struct _impl_stricmp<char> {
		int operator()(const char* const pDest, const char* const pSrc) noexcept
		{
			return _stricmp(pDest, pSrc);
		}
	};
	template <>
	struct _impl_stricmp<wchar_t> {
		int operator()(const wchar_t* const pDest, const wchar_t* const pSrc) noexcept
		{
			return _wcsicmp(pDest, pSrc);
		}
	};

	template <typename T>
	struct _impl_ts_find {
	};
	template <>
	struct _impl_ts_find<char> {
		const char* operator()(const char* const str, const char& srch) noexcept
		{
			return strchr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<const char> {
		const char* operator()(const char* const str, const char& srch) noexcept
		{
			return strchr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<char*> {
		const char* operator()(const char* const str, const char* srch) noexcept
		{
			return strstr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<const char*> {
		const char* operator()(const char* const str, const char* srch) noexcept
		{
			return strstr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<wchar_t> {
		const wchar_t* operator()(const wchar_t* const str, const wchar_t& srch) noexcept
		{
			return wcschr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<const wchar_t> {
		const wchar_t* operator()(const wchar_t* const str, const wchar_t& srch) noexcept
		{
			return wcschr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<wchar_t*> {
		const wchar_t* operator()(const wchar_t* const str, const wchar_t* srch) noexcept
		{
			return wcsstr(str, srch);
		}
	};
	template <>
	struct _impl_ts_find<const wchar_t*> {
		const wchar_t* operator()(const wchar_t* const str, const wchar_t* srch) noexcept
		{
			return wcsstr(str, srch);
		}
	};

	template <typename T>
	struct _impl_vscprintf {
	};
	template <>
	struct _impl_vscprintf<char> {
		const int operator()(const char* const fmt, const va_list args) noexcept
		{
			return _vscprintf(fmt, args);
		}
	};
	template <>
	struct _impl_vscprintf<wchar_t> {
		const int operator()(const wchar_t* const fmt, const va_list args) noexcept
		{
			return _vscwprintf(fmt, args);
		}
	};

	template <typename T>
	struct _impl_vsprintf {
	};
	template <>
	struct _impl_vsprintf<char> {
		const int operator()(char* const buf, size_t nCount, const char* const fmt, const va_list args) noexcept
		{
			return vsnprintf(buf, nCount, fmt, args);
		}
	};
	template <>
	struct _impl_vsprintf<wchar_t> {
		const int operator()(wchar_t* const buf, size_t nCount, const wchar_t* const fmt, const va_list args) noexcept
		{
			return vswprintf(buf, nCount, fmt, args);
		}
	};

	template <typename T, typename Format, typename... Arguments>
	struct _impl_snprintf {
	};
	template <typename... Arguments>
	struct _impl_snprintf<char, char, Arguments...> {
		const int operator()(char* const buf, const size_t nCount, const char* const fmt, const Arguments&&... args) noexcept
		{
			return snprintf(buf, nCount, fmt, args...);
		}
	};
	template <typename... Arguments>
	struct _impl_snprintf<wchar_t, wchar_t, Arguments...> {
		const int operator()(wchar_t* const buf, const size_t nCount, const wchar_t* const fmt, const Arguments&&... args) noexcept
		{
			return _snwprintf(buf, nCount, fmt, args...);
		}
	};
	template <typename T, typename... Arguments>
	struct _impl_snprintf<T, typename T::value_type, Arguments...> {
		const std::enable_if_t<std::is_member_function_pointer_v<decltype(&T::data)> && std::is_member_function_pointer_v<decltype(&T::size)>, int> operator()(T& buf, const typename T::value_type* const fmt, const Arguments&&... args) noexcept
		{
			//return _ts_snprintf(buf.data(), buf.size(), fmt, args...);
			//return _impl_snprintf<T::value_type, T::value_type, Arguments...>()(buf.data(), buf.size(), fmt, std::forward<Arguments>(args)...);

			if constexpr (std::is_same_v<char, T::value_type>)
				return snprintf(buf.data(), buf.size(), fmt, args...);
			else
				return _snwprintf(buf.data(), buf.size(), fmt, args...);
		}
	};

	template <typename T>
	struct _impl_atoi {
	};
	template <>
	struct _impl_atoi<char> {
		auto operator()(const char* const buf) noexcept
		{
			return atoi(buf);
		}
	};
	template <>
	struct _impl_atoi<wchar_t> {
		auto operator()(const wchar_t* const buf) noexcept
		{
			return _wtoi(buf);
		}
	};

	template <typename T>
	struct _impl_atoi64 {
	};
	template <>
	struct _impl_atoi64<char> {
		auto operator()(const char* const buf) noexcept
		{
			return _atoi64(buf);
		}
	};
	template <>
	struct _impl_atoi64<wchar_t> {
		auto operator()(const wchar_t* const buf) noexcept
		{
			return _wtoi64(buf);
		}
	};

	template <typename T>
	struct _impl_atof {
	};
	template <>
	struct _impl_atof<char> {
		auto operator()(const char* const buf) noexcept
		{
			return atof(buf);
		}
	};
	template <>
	struct _impl_atof<wchar_t> {
		auto operator()(const wchar_t* const buf) noexcept
		{
			return _wtof(buf);
		}
	};

	template <typename T>
	struct _impl_itoa {
	};
	template <>
	struct _impl_itoa<char> {
		auto operator()(const int val, char* const buf, const int radix) noexcept
		{
			return _itoa(val, buf, radix);
		}
	};
	template <>
	struct _impl_itoa<wchar_t> {
		auto operator()(const int val, wchar_t* const buf, const int radix) noexcept
		{
			return _itow(val, buf, radix);
		}
	};

	template <typename T>
	struct _impl_strtoul {
	};
	template <>
	struct _impl_strtoul<char> {
		auto operator()(const char* const buf, char** endptr, int radx) noexcept
		{
			return strtoul(buf, endptr, radx);
		}
	};
	template <>
	struct _impl_strtoul<wchar_t> {
		auto operator()(const wchar_t* const buf, wchar_t** endptr, int radx) noexcept
		{
			return wcstoul(buf, endptr, radx);
		}
	};

	template <typename T>
	struct _impl_bstrtoul {
	};
	template <>
	struct _impl_bstrtoul<char> {
		auto operator()(const char* const buf) noexcept
		{
			return strtoul(buf, nullptr, 2);
		}
	};
	template <>
	struct _impl_bstrtoul<wchar_t> {
		auto operator()(const wchar_t* const buf) noexcept
		{
			return wcstoul(buf, nullptr, 2);
		}
	};
}

// Check string bounds, make sure dest is not within the source string & vice versa (this could be a possible reason for some strcpyn() fails we see)
template <details::IsPODText T>
constexpr bool isInBounds(const T* const sDest, const T* const sSrc, const std::size_t iLen) noexcept
{
	return (sSrc >= sDest && sSrc <= (sDest + iLen)) || (sDest >= sSrc && sDest <= (sSrc + iLen));
}

template <typename Result, typename Format, typename Value, typename... Arguments>
Result& _ts_sprintf(Result& res, const Format& fmt, const Value& val, Arguments&&... args)
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
inline auto _ts_strlen(const T& str) noexcept
{
	return details::_ts_strlen(str);
}

// Get String length (with buffer size limit)
template <details::IsPODText T>
inline size_t _ts_strnlen(const T& str, const size_t& nBufSize) noexcept
{
	return details::_impl_strnlen<T>()(str, nBufSize);
}

// Test if a string is empty, works for any object that has an empty() member function as well as C strings.
template <typename T>
inline bool _ts_isEmpty(const T& str) noexcept
{
	return details::_ts_isEmpty(str);
}

// Finds a string or character within a string & returns a pointer to it.
template <details::IsPODText HayStack, details::IsPODText Needle>
inline HayStack* _ts_find(HayStack* const str, Needle srch) noexcept
{
	return const_cast<HayStack*>(details::_impl_ts_find<Needle>()(str, srch));
}
template <details::IsPODText HayStack, details::IsPODText Needle>
inline const HayStack* _ts_find(const HayStack* const str, Needle srch) noexcept
{
	return details::_impl_ts_find<Needle>()(str, srch);
}

// Finds a string literal within a string & returns a pointer to it.
template <typename T, size_t N>
inline T* _ts_find(T* const str, const T(&srch)[N]) noexcept
{
	return const_cast<T*>(details::_impl_ts_find<std::add_pointer_t<T>>()(str, &srch[0]));
}

//// Finds a string or character within a string & returns a pointer to it.
//template <typename T, typename TStr>
//inline TStr *_ts_find(TStr *const str, T &srch)
//{
//	return const_cast<TStr *>(details::_impl_ts_find<std::remove_cv_t<T>>()(str, srch));
//}
//
//// Finds a string literal within a string & returns a pointer to it.
//template <typename T, typename TStr, size_t N>
//inline TStr *_ts_find(TStr *const str, const T (&srch)[N])
//{
//	return const_cast<TStr *>(details::_impl_ts_find<std::add_pointer_t<T>>()(str, &srch[0]));
//}

//template <typename T, typename TStr>
//inline const TStr *_ts_find(const TStr *const str, const T &srch)
//{
//	return details::_impl_ts_find<std::remove_cv_t<T>>()(str, srch);
//}
//
//template <typename T, typename TStr>
//inline TStr *_ts_find(TStr *const str, const T &srch)
//{
//	return const_cast<TStr *>(details::_impl_ts_find<std::remove_cv_t<T>>()(str, srch));
//}

template <details::IsPODText T>
T* _ts_strcpyn(T* const sDest, const T* const sSrc, const size_t iChars) noexcept
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
T* _ts_strcpy(T* const sDest, const T* const sSrc) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	if ((!sDest) || (!sSrc))
		return nullptr;

	return details::_impl_strcpy<T>()(sDest, sSrc);
}

template <details::IsPODText T>
T* _ts_strncat(T* const sDest, const T* const sSrc, const size_t iChars) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strncat<T>()(sDest, sSrc, iChars);
}

template <details::IsPODText T>
T* _ts_strcat(T* const sDest, const T* const sSrc) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strcat<T>()(sDest, sSrc);
}

template <details::IsPODText T>
int _ts_strncmp(const T* const sDest, const T* const sSrc, const size_t iChars) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strncmp<T>()(sDest, sSrc, iChars);
}

template <details::IsPODText T>
int _ts_strnicmp(const T* const sDest, const T* const sSrc, const size_t iChars) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strnicmp<T>()(sDest, sSrc, iChars);
}

template <details::IsPODText T>
int _ts_strcmp(const T* const sDest, const T* const sSrc) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strcmp<T>()(sDest, sSrc);
}

template <details::IsPODText T>
int _ts_stricmp(const T* const sDest, const T* const sSrc) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_stricmp<T>()(sDest, sSrc);
}

template <details::IsPODText T>
int _ts_vscprintf(__format_string const T* const _Format, va_list _ArgList) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_vscprintf<T>()(_Format, _ArgList);
}

template <details::IsPODText T>
int _ts_vsprintf(T* const buf, size_t nCount, __format_string const T* const fmt, const va_list args) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_vsprintf<T>()(buf, nCount, fmt, args);
}

template <details::IsPODText T, typename Format, typename... Arguments>
int _ts_snprintf(T* const buf, const size_t nCount, __format_string const Format* const fmt, Arguments&&... args) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");
	static_assert(std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<Format>>, "Buffer & format must have same type.");

	return details::_impl_snprintf<T, Format, Arguments...>()(buf, nCount, fmt, std::forward<Arguments>(args)...);
}

template <typename T, details::IsPODText Format, typename... Arguments>
int _ts_snprintf(T& buf, __format_string const Format* const fmt, Arguments&&... args) noexcept
{
	static_assert(details::IsPODText<Format>, "Only char & wchar_t supported...");

	return details::_impl_snprintf<T, Format, Arguments...>()(buf, fmt, std::forward<Arguments>(args)...);
}

template <details::IsPODText T>
auto _ts_atoi(const T* const buf) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_atoi<T>()(buf);
}

template <details::IsPODText T>
auto _ts_atoi64(const T* const buf) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_atoi64<T>()(buf);
}

template <details::IsPODText T>
auto _ts_atof(const T* const buf) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_atof<T>()(buf);
}

template <details::IsPODText T>
auto _ts_itoa(const int val, T* const buf, const int radix) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_itoa<T>()(val, buf, radix);
}

template <details::IsPODText T>
auto _ts_strtoul(const T* const buf, T** endptr, int base) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_strtoul<T>()(buf, endptr, base);
}

// _ts_bstrtoul()
// Binary string to unsigned long
template <details::IsPODText T>
auto _ts_bstrtoul(const T* const buf) noexcept
{
	static_assert(details::IsPODText<T>, "Only char & wchar_t supported...");

	return details::_impl_bstrtoul<T>()(buf);
}

template <class T, class Result>
Result DecimalToBinaryString(const T& a)
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

	//std::set<T> tmp(a);
	//return tmp.ToString();
}

// replace all instances of 'rep' in 'str' with 'with'
template <class T, class RepThis, class WithThis>
T& _ts_replace(T& str, RepThis rep, WithThis with)
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
		//str.erase(pos, rlen);
		//str.insert(pos, with);
	}
	return str;
}

//void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace)
//{
//	size_t pos = 0;
//	while ((pos = subject.find(search, pos)) != std::string::npos)
//	{
//		subject.replace(pos, search.length(), replace);
//		pos += replace.length();
//	}
//}

// replace all instances of each character in 'rep' in 'str' with 'with'
template <class T, class RepThis, class WithThis>
T& _ts_mreplace(T& str, RepThis rep, WithThis with)
{
	for (std::ptrdiff_t i = 0; rep[i] != 0; ++i)
		_ts_replace(str, rep[i], with);
	return str;
}

template <class T, class RemThis>
T& _ts_remove(T& str, RemThis rep)
{
	for (auto itEnd = str.end(), itStart = str.begin(); str.erase(std::find(itStart, itEnd, rep)) != itEnd; ) {}

	return str;
}

template <class T>
T& _ts_trim(T& str)
{
	while (str.front() == _T(' '))
		str.erase(0, 1);

	while (str.back() == _T(' '))
		str.pop_back();

	return str;
}

template <class T>
T& _ts_strip(T& str)
{
	//_ts_remove(str, _T('\0x02'));	//02
	//_ts_remove(str, _T('\0x0F'));	//15
	//_ts_remove(str, _T('\0x16'));	//22
	//_ts_remove(str, _T('\0x1D'));	//29
	_ts_replace(str, _T("  "), _T(' '));
	// NB: TODO: add ctrl-k removal for colour codes.
	return _ts_trim(str);
}

template <class T>
T& _ts_toupper(T& str)
{
	for (auto& a : str)
		a = details::make_upper(a);
	return str;
}

template <class T, class Input>
T _ts_to_(Input& str)
{
	static_assert(is_Numeric_v<T>, "Type T must be (int, long, float, double, ....)");

	std::basic_istringstream<TCHAR> ss(str);	// makes copy of string :(
	T result{};
	return ss >> result ? result : T();
}

#define TSTRING_WILDT 0
#define TSTRING_WILDA 0
#define TSTRING_WILDE 0
#define TSTRING_WILDW 0

//template <class T>
//constexpr auto getWildType(T s) noexcept
//{
//	if constexpr (details::is_pod_v<T>)
//		return typename std::remove_pointer_t<std::remove_cv_t<std::remove_all_extents_t<T>>>;
//	else
//		return typename std::remove_pointer_t<std::remove_cv_t<std::remove_all_extents_t<T::value_type>>>;
//}
//
//template <typename TameString, typename WildString>
//[[gsl::suppress(bounds)]] bool _ts_WildcardMatch(const TameString& pszString, const WildString& pszMatch, const bool bCase = false)
//{
//	//if ((!pszMatch) || (!pszString))
//	//	return false;
//
//	if (_ts_isEmpty(pszMatch) || _ts_isEmpty(pszString))
//		return false;
//
//	ptrdiff_t MatchPlaceholder = 0;
//	ptrdiff_t StringPlaceholder = 0;
//	ptrdiff_t iWildOffset = 0;
//	ptrdiff_t iTameOffset = 0;
//
//	//using _WildString = typename std::remove_pointer_t<std::remove_cv_t<std::remove_all_extents_t<WildString::value_type>>>;
//	using _WildString = typename std::remove_pointer_t<std::remove_cv_t<std::remove_all_extents_t<WildString>>>;
//	//auto a = getWildType(pszMatch);
//
//	constexpr _WildString _zero{};
//	constexpr _WildString _star = static_cast<_WildString>('*');
//	constexpr _WildString _question = static_cast<_WildString>('?');
//	constexpr _WildString _tilda = static_cast<_WildString>('~');
//	constexpr _WildString _space = static_cast<_WildString>(' ');
//	constexpr _WildString _power = static_cast<_WildString>('^');
//	constexpr _WildString _hash = static_cast<_WildString>('#');
//	constexpr _WildString _slash = static_cast<_WildString>('\\');
//	constexpr _WildString _tab = static_cast<_WildString>('\t');
//	constexpr _WildString _n = static_cast<_WildString>('\n');
//	constexpr _WildString _r = static_cast<_WildString>('\r');
//
//	while (pszString[iTameOffset])
//	{
//		if (pszMatch[iWildOffset] == _star)
//		{
//			if (pszMatch[++iWildOffset] == _zero)
//				return true;
//			MatchPlaceholder = iWildOffset;
//			StringPlaceholder = iTameOffset + 1;
//		}
//#if TSTRING_WILDT
//		else if (pszMatch[iWildOffset] == _tilda && pszString[iTameOffset] == _space)
//		{
//			++iWildOffset;
//			while (pszString[iTameOffset] == _space)
//				++iTameOffset;
//		}
//#endif
//#if TSTRING_WILDA
//		else if (pszMatch[iWildOffset] == _power)
//		{
//			++iWildOffset;
//			if (details::CompareChar(pszMatch[iWildOffset], pszString[iTameOffset], bCase))
//				++iTameOffset;
//			++iWildOffset;
//		}
//#endif
//#if TSTRING_WILDE
//		else if (pszMatch[iWildOffset] == _slash)
//		{
//			// any character following a '\' is taken as a literal character.
//			++iWildOffset;
//			if (!details::CompareChar(pszMatch[iWildOffset], pszString[iTameOffset], bCase))
//				return false;
//			++iTameOffset;
//		}
//#endif
//#if TSTRING_WILDW
//		else if (pszMatch[iWildOffset] == _hash)
//		{
//			++iWildOffset;
//			while (pszString[iTameOffset] && (pszString[iTameOffset] != _space || pszString[iTameOffset] != _tab || pszString[iTameOffset] != _n || pszString[iTameOffset] != _r))
//				++iTameOffset;
//		}
//#endif
//		//else if (pszMatch[iWildOffset] == TEXT('?') || _toupper(pszMatch[iWildOffset]) == _toupper(pszString[iTameOffset]))
//		else if (pszMatch[iWildOffset] == _question || details::CompareChar(pszMatch[iWildOffset], pszString[iTameOffset], bCase))
//		{
//			++iWildOffset;
//			++iTameOffset;
//		}
//		else if (StringPlaceholder == 0)
//			return false;
//		else
//		{
//			iWildOffset = MatchPlaceholder;
//			iTameOffset = StringPlaceholder++;
//		}
//	}
//
//	while (pszMatch[iWildOffset] == _star)
//		++iWildOffset;
//
//	return !pszMatch[iWildOffset];
//}

template <class _Wild, typename TameString, typename WildString>
GSL_SUPPRESS(bounds.4) bool _ts_InnerWildcardMatch(const TameString& pszString, const WildString& pszMatch, const bool bCase) noexcept(std::is_nothrow_move_assignable_v<WildString>)
{
	ptrdiff_t MatchPlaceholder = 0;
	ptrdiff_t StringPlaceholder = 0;
	ptrdiff_t iWildOffset = 0;
	ptrdiff_t iTameOffset = 0;

	constexpr _Wild _zero{};
	constexpr _Wild _star = static_cast<_Wild>('*');
	constexpr _Wild _question = static_cast<_Wild>('?');
	constexpr _Wild _tilda = static_cast<_Wild>('~');
	constexpr _Wild _space = static_cast<_Wild>(' ');
	constexpr _Wild _power = static_cast<_Wild>('^');
	constexpr _Wild _hash = static_cast<_Wild>('#');
	constexpr _Wild _slash = static_cast<_Wild>('\\');
	constexpr _Wild _tab = static_cast<_Wild>('\t');
	constexpr _Wild _n = static_cast<_Wild>('\n');
	constexpr _Wild _r = static_cast<_Wild>('\r');

	while (pszString[iTameOffset])
	{
		if (pszMatch[iWildOffset] == _star)
		{
			if (pszMatch[++iWildOffset] == _zero)
				return true;
			MatchPlaceholder = iWildOffset;
			StringPlaceholder = iTameOffset + 1;
		}
#if TSTRING_WILDT
		else if (pszMatch[iWildOffset] == _tilda && pszString[iTameOffset] == _space)
		{
			++iWildOffset;
			while (pszString[iTameOffset] == _space)
				++iTameOffset;
		}
#endif
#if TSTRING_WILDA
		else if (pszMatch[iWildOffset] == _power)
		{
			++iWildOffset;
			if (details::CompareChar(pszMatch[iWildOffset], pszString[iTameOffset], bCase))
				++iTameOffset;
			++iWildOffset;
		}
#endif
#if TSTRING_WILDE
		else if (pszMatch[iWildOffset] == _slash)
		{
			// any character following a '\' is taken as a literal character.
			++iWildOffset;
			if (!details::CompareChar(pszMatch[iWildOffset], pszString[iTameOffset], bCase))
				return false;
			++iTameOffset;
		}
#endif
#if TSTRING_WILDW
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

template <typename TameString, typename WildString>
GSL_SUPPRESS(bounds) bool _ts_WildcardMatch(const TameString& pszString, const WildString& pszMatch, const bool bCase = false) noexcept(std::is_nothrow_move_assignable_v<WildString>)
{
	if (_ts_isEmpty(pszMatch) || _ts_isEmpty(pszString))
		return false;

	if constexpr (details::is_pod_v<std::remove_pointer_t<std::remove_cv_t<std::remove_all_extents_t<WildString>>>>)
		return _ts_InnerWildcardMatch<std::remove_pointer_t<std::remove_cv_t<std::remove_all_extents_t<WildString>>>>(pszString, pszMatch, bCase);
	else
		return _ts_InnerWildcardMatch<std::remove_pointer_t<std::remove_cv_t<std::remove_all_extents_t<WildString::value_type>>>>(pszString, pszMatch, bCase);
}

template <typename T> T* _ts_AddMem(std::vector<std::shared_ptr<std::vector<char>>>& mem, std::size_t sz = sizeof(T))
{
	std::shared_ptr<std::vector<char>> x = std::make_shared<std::vector<char>>();
	x->resize(sz);
	mem.push_back(x);
	T* d = (T*)mem[mem.size() - 1].get()->data();
	return d;
}

//template <class T>
//struct TokenIterator
//{
//	std::basic_string_view<T>	Data{};
//
//	struct iter
//	{
//		T operator * () const noexcept { return n; }
//		iter& operator ++() noexcept { ++n; return *this; }
//		friend
//			bool operator != (iter const& lhs, iter const& rhs) noexcept
//		{
//			return lhs.n != rhs.n;
//		}
//		ptrdiff_t n;
//	}
//	iter begin() const noexcept { return{ 1 }; }
//	iter end() const noexcept { return { 0 }; }
//};
