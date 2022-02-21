/*!
 * \file tstring.h
 * \brief String and Token Management Class
 *
 * This experimental library was built to reproduce easy to use string containers managing effectively
 * comparisons and token manipulations as done in the mIRC scripting language.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.19
 *
 * \b Revisions
 *	1.1
 *		Fixed some mem leaks. Ook
 *	1.2
 *		Added left() mid() right() functions. Ook
 *		Added wildtok() nwildtok() functions. Ook
 *	1.3
 *		Changed += operators to only need one lstrcpy(). Ook
 *	1.4
 *		Changed *= operator to only need one new call. Ook
 *	1.5
 *		Added replace() functions. Ook
 *		Added _replace() function for use by the replace() functions. Ook
 *		Changed token functions to only call lstrlen( sepChars ) once. Ook
 *		Changed find() to only call lstrlen( substring ) once. Ook
 *		Added istok() function. Ook
 *	1.6
 *		Added addtok() function. Ook
 *		Added Visual Studio 2005 specific defines. Ook
 *	1.7
 *		Added sprintf() function. Ook
 *	1.8
 *		Added iswm() & iswmcs(). Ook
 *	1.9
 *		More changes & shit than i can remember. Ook
 *	1.10
 *		Fixed a bug in charToWchar() that caused the conversion to always fail.
 *	1.11
 *		Added strip() function.
 *	1.12
 *		changed gettok() to not copy the contents to token first.
 *		changed findtok() to take an UINT arg.
 *		changed deltok() to take an UINT arg.
 *		changed instok() to take an unsigned arg.
 *		and loads more...
 *	1.13
 *      loads of changes !!!
 *  1.14
 *		loads of changes again :)
 *
 *	1.15
 *		Added iterator
 *		Added template versions of most functions
 *
 *  1.16
 *		Added concepts
 *
 *  1.17
 *      Changed to use cstdio & cstdlib
 * 
 *  1.18
 *		Fixed constructor bug.
 * 
 *  1.19
 *		Fixed issue with getnexttok() when changing tokens used.
 * 
 * © ScriptsDB.org - 2005-2021
 */

#pragma once

#ifndef _TSTRING_H_
#define _TSTRING_H_

 // VS2019+ only
#if !defined(_MSC_FULL_VER) || _MSC_FULL_VER < 192829914
#error "This version of TString needs Visual Studio 2019 16.9.4 or newer"
#endif

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>
#include <sstream>

#include "string_support.h"

// pre-defined for concepts
class TString;

namespace TStringConcepts {
	template <class T>
	concept IsCharText = std::is_same_v<std::remove_cvref_t<std::remove_all_extents_t<T>>, char>;

	template <class T>
	concept IsWCharText = std::is_same_v<std::remove_cvref_t<std::remove_all_extents_t<T>>, wchar_t>;

	template <class T>
	concept IsTString = std::is_same_v<std::remove_cvref_t<std::remove_all_extents_t<T>>, TString>;

	template <class T>
	concept IsPODText = IsCharText<T> || IsWCharText<T>;

	template <class T>
	concept IsPODTextArray = IsPODText<T> && std::is_bounded_array_v<T>;

	template <class T>
	concept IsPODTextPointer = std::is_pointer_v<T> && IsPODText<std::remove_pointer_t<T>> && !IsPODTextArray<T>;

	template <class T>
	concept IsNumeric = is_Numeric_v<T>;

	// container type
	template<typename Iter, typename Container>
	concept container_iterator =
		std::same_as<Iter, typename Container::iterator> ||
		std::same_as<Iter, typename Container::const_iterator>;
	template<typename T>
	concept ImplementsBeginFunction = requires(T t)
	{
		{ t.begin() } -> container_iterator<T>;
	};
	template<typename T>
	concept ImplementsEndFunction = requires(T t)
	{
		{ t.end() } -> container_iterator<T>;
	};
	template<typename T>
	concept ImplementsEraseFunction = requires(T t)
	{
		//t.erase();
		//std::is_member_function_pointer_v<decltype(&T::erase)>;
		//std::is_member_function_pointer_v<decltype(std::declval(t).erase())>;
		//std::declval(t).erase();
		{ t.erase() } -> container_iterator<T>;
	};

	template<typename T>
	concept IsContainer = ImplementsBeginFunction<T> && ImplementsEndFunction<T> /*&& ImplementsEraseFunction<T>*/;

	template <class T>
	concept IsSupportedCompareType = IsPODText<T> || IsPODTextPointer<T> || IsTString<T> || std::is_convertible_v<std::remove_cvref_t<T>, wchar_t> || std::is_convertible_v<std::remove_cvref_t<T>, char> || std::is_convertible_v<std::remove_cv_t<T>, wchar_t*> || std::is_convertible_v<std::remove_cv_t<T>, char*>;

	template <class T>
	concept IsSupportedAddType = IsNumeric<T> || IsSupportedCompareType<T>;

	template <class T>
	concept IsSupportedRemoveType = IsSupportedCompareType<T>;

	template <class T>
	concept IsSupportedSeperatorType = IsSupportedCompareType<T>;
}

template<TStringConcepts::IsNumeric T>
constexpr auto TS_getmemsize(T x) noexcept { return gsl::narrow_cast<size_t>(((x)-1U) + (16U - (((x)-1U) % 16U))); }
//#define TS_wgetmemsize(x) (unsigned long)(((((x) - 1)*sizeof(WCHAR)) + (16 - ((((x) - 1)*sizeof(WCHAR)) % 16)))/sizeof(WCHAR))
template<TStringConcepts::IsNumeric T>
constexpr auto TS_wgetmemsize(T x) noexcept { return (TS_getmemsize((x) * sizeof(WCHAR)) / sizeof(WCHAR)); }

// enable this define if you wish to use the mIRC extra functions
#define INCLUDE_MIRC_EXTRAS 0

// Enable/Disable test code...
#define TSTRING_TESTCODE 1

// enable this to include a small internal buffer that avoids an allocation for small strings. (always used now)
#define TSTRING_INTERNALBUFFER 1
// internal buffer size in characters
#define TSTRING_INTERNALBUFFERSIZE_CCH TS_getmemsize(64)
// internal buffer size in bytes
#define TSTRING_INTERNALBUFFERSIZE_BYTES (TSTRING_INTERNALBUFFERSIZE_CCH*sizeof(TCHAR))

#define SPACE m_cSpace
#define SPACECHAR m_cSpace[0]
#define COMMA m_cComma
#define COMMACHAR m_cComma[0]
#define TAB m_cTab
#define TABCHAR m_cTab[0]

#define ts_atoi(x) _ts_atoi((x))
#define ts_atoi64(x) _ts_atoi64((x));
#define ts_atof(x) _ts_atof((x));
#define ts_strtoul(x) wcstoul((x), nullptr, 10)
#define ts_itoa(x,y,z) _itow((x),(y),(z))
#define ts_upr(x,y) _wcsupr_s((x),(y))
//#define ts_upr(x,y) _ts_toupper((x),(y))
//#define ts_strstr(x,y) wcsstr((x),(y))
//#define ts_strchr(x,y) wcschr((x),(y))
#define ts_strstr(x,y) _ts_find((x),(y))
#define ts_strchr(x,y) _ts_find((x),(y))
#define ts_strcat_s(x,y,z) wcscat_s((x),(z),(y))
#define ts_strcat(x,y) _ts_strcat((x),(y))
#define ts_strncat(x,y,z) _ts_strncat((x),(y),gsl::narrow_cast<size_t>(z))
template<TStringConcepts::IsPODText T>
constexpr auto ts_toupper(T c) { return ((((c) >= TEXT('a')) && ((c) <= TEXT('z'))) ? _toupper((c)) : (c)); }
template<TStringConcepts::IsPODText T>
constexpr auto ts_tolower(T c) { return ((((c) >= TEXT('A')) && ((c) <= TEXT('Z'))) ? _tolower((c)) : (c)); }
#define ts_strcpyn(dest,src,len) _ts_strcpyn((dest),(src),gsl::narrow_cast<size_t>(len))
#define ts_strcpy(dest,src) _ts_strcpy((dest),(src))
//#define ts_strcpyn(dest,src,len) lstrcpyn((dest),(src),gsl::narrow_cast<size_t>(len))
//#define ts_strcpy(dest,src) lstrcpy((dest),(src))
#define ts_strcmp(x,y) _ts_strcmp((x),(y))
#define ts_strncmp(x,y,z) _ts_strncmp((x),(y),gsl::narrow_cast<size_t>(z))
#define ts_stricmp(x,y) _ts_stricmp((x),(y))
#define ts_strnicmp(x,y,z) _ts_strnicmp((x),(y),gsl::narrow_cast<size_t>(z))
#define ts_vscprintf(fmt, args) _ts_vscprintf((fmt), (args))
#define ts_vsprintf(txt, cnt, fmt, args ) _ts_vsprintf((txt), (cnt), (fmt), (args))

#define ts_copymem(dest,src,sz) CopyMemory((dest),(src),(sz))
#define ts_zeromem(dest, sz) ZeroMemory((dest),(sz))

#define ts_strcat_throw(x,y) if (ts_strcat((x), (y)) == nullptr) throw std::logic_error("strcat() failed!");
#define ts_strncat_throw(x,y,z) if (ts_strncat((x), (y), (z)) == nullptr) throw std::logic_error("strncat() failed!");
#define ts_strcpy_throw(x,y) if (ts_strcpy((x), (y)) == nullptr) throw std::logic_error("strcpy() failed!");
#define ts_strcpyn_throw(x,y,z) if (ts_strcpyn((x), (y), (z)) == nullptr) throw std::logic_error("strcpyn() failed!");

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2304 )
#pragma warning( disable : 2287 )
#endif

#if INCLUDE_MIRC_EXTRAS
#pragma message ("### TString OPTION: mIRC Extras - Enabled")
#else
#pragma message ("### TString OPTION: mIRC Extras - Disabled")
#endif
#if TSTRING_TESTCODE
#pragma message ("### TString OPTION: Test Code - Enabled")
#else
#pragma message ("### TString OPTION: Test Code - Disabled")
#endif
#if TSTRING_INTERNALBUFFER
#pragma message ("### TString OPTION: Internal Buffer - Enabled")
#else
#pragma message ("### TString OPTION: Internal Buffer - Disabled")
#endif

/*!
 * \brief String and Token Management Class
 */
class TString final
{
public:
	using value_type = wchar_t;
	using convert_type = char;
	using size_type = std::size_t;
	using const_value_type = std::add_const_t<value_type>;
	using pointer = std::add_pointer_t<value_type>;
	using const_pointer = std::add_pointer_t<const_value_type>;
	using const_pointer_const = std::add_const_t<const_pointer>;
	using reference = std::add_lvalue_reference_t<value_type>;
	using const_reference = std::add_lvalue_reference_t<const_value_type>;

private:

	struct TokenRange final
	{
		const_pointer m_pStart{ nullptr };
		const_pointer m_pEnd{ nullptr };
		size_type	m_count{};
	};

	std::unique_ptr<value_type[]> m_buffer{};

	/// <summary>
	/// Allocate a buffer thats sized to multiples of 16 bytes, (%byte + (16 - (%byte % 16)))
	/// </summary>
	/// <param name="size">- The desired buffer size in bytes, buffer is guaranteed to be atleast this big.</param>
	/// <returns></returns>
	GSL_SUPPRESS(bounds.3)
		void setup_buffer(const size_type size)
	{
		m_InternalBuffer[0] = TEXT('\0');
		if (size <= TSTRING_INTERNALBUFFERSIZE_BYTES)
		{
			m_bUsingInternal = true;
			m_buffersize = std::size(m_InternalBuffer) * sizeof(TCHAR);
			m_pString = &m_InternalBuffer[0];
			m_buffer.reset(nullptr);
		}
		else
		{
			m_bUsingInternal = false;

			m_buffersize = TS_getmemsize(size);
			m_buffer = std::make_unique<TCHAR[]>(m_buffersize / sizeof(TCHAR));
			m_pString = m_buffer.get();
		}
		ts_zeromem(m_pString, m_buffersize);
	};

	// swap contents of second with this
	void swap(TString& second) noexcept;

	/// <summary>
	/// Make temp string m_pTempString which is m_pString converted to either WCHAR or char
	/// </summary>
	void MakeTemp() const
	{
		if (!m_pString)
			return;

		if (!m_pTempString)
			m_pTempString = WcharTochar(m_pString);
	}

	/// <summary>
	/// Check if requested character is within buffer (not within string)
	/// </summary>
	/// <typeparam name="T">Any Numeric type as defined by is_Numeric_v</typeparam>
	/// <param name="N">- The offset to be checked</param>
	template <TStringConcepts::IsNumeric T>
	void CheckRange(const T& N) const
	{
		static_assert(is_Numeric_v<T>, "Only Numeric Types accepted.");

		if constexpr (std::is_signed_v<T>)
		{
			if ((!m_pString) || (N < 0) || (gsl::narrow_cast<size_type>(N) >= (m_buffersize / sizeof(value_type))))
				throw std::out_of_range("TString::at()");
		}
		else {
			if ((!m_pString) || (N >= (m_buffersize / sizeof(value_type))))
				throw std::out_of_range("TString::at()");
		}
	}

	/// <summary>
	/// Convert a char string to a wchar string.
	/// </summary>
	/// <param name="cString">- The char string to convert.</param>
	/// <param name="buffer_size">- Contains the buffer size of the returned string in bytes. (can be nullptr)</param>
	/// <returns>The sting converted to wchar or nullptr on failure.</returns>
	static std::unique_ptr<WCHAR[]> charToWchar(const char* const cString, size_t* const buffer_size);
	/// <summary>
	/// Convert a wchar string to a char string.
	/// </summary>
	/// <param name="wString">- The wchar string to convert.</param>
	/// <returns>The sting converted to char or nullptr on failure.</returns>
	static std::unique_ptr<char[]> WcharTochar(const WCHAR* const wString);

	mutable const_pointer m_savedpos{ nullptr };
	mutable UINT		m_savedtotaltoks{};
	mutable UINT		m_savedcurrenttok{};

	size_type			m_buffersize{ TSTRING_INTERNALBUFFERSIZE_BYTES };		// size of string buffer in use. (size in bytes)
	mutable size_type	m_iLen{};												// the string length of m_pString
	mutable bool		m_bDirty{ true };										// is buffer `dirty` (string length is unknown)?
	mutable bool		m_bUsingInternal{ true };								// using internal buffer?
	mutable value_type	m_InternalBuffer[TSTRING_INTERNALBUFFERSIZE_CCH]{};		// the internal small buffer

	pointer m_pString{ &m_InternalBuffer[0] }; //!< String buffer
	// Temp string buffer used for string conversion to/from wchar/char or vice versa depending on what TCHAR is.
	// changes made to m_pTempString are NOT reflected in m_pString!
	mutable std::unique_ptr<convert_type[]> m_pTempString{ nullptr };

public:

	static inline const_pointer_const m_cSpace = TEXT(" ");
	static inline const_pointer_const m_cComma = TEXT(",");
	static inline const_pointer_const m_cTab = TEXT("\t");

	/// <summary>
	/// Default constructor
	/// </summary>
	/// <returns></returns>
	constexpr TString() noexcept
		: m_bDirty(false)
	{
	}

	/// <summary>
	/// Construct from a literal string
	/// like: "some text"
	/// </summary>
	/// <param name="cString">- The literal string</param>
	/// <returns></returns>
	template <TStringConcepts::IsPODText T, size_t N>
	explicit TString(const T(&cString)[N]) noexcept(N <= TSTRING_INTERNALBUFFERSIZE_CCH)
		: TString(&cString[0], N - 1)		// NB: N here also counts the zero char at the end
	{
		static_assert(TStringConcepts::IsPODText<T>, "MUST be a WCHAR or char string");
	}

#pragma warning(push)
#pragma warning(disable: 26495) // warning C26495 : Variable 'TString::m_savedpos' is uninitialized.Always initialize a member variable(type.6: http://go.microsoft.com/fwlink/p/?LinkID=620422).
	//template <typename T, typename = std::enable_if_t<std::is_pointer_v<T>> >

	/// <summary>
	/// Construct from a POD string pointer
	/// </summary>
	/// <param name="cString">- A string of type char * or wchar_t *</param>
	template <TStringConcepts::IsPODTextPointer T>
	TString(const T cString)
		: TString(cString, _ts_strlen(cString))
	{
		static_assert(TStringConcepts::IsPODTextPointer<T>, "MUST be a WCHAR or char string");
	}
#pragma warning(pop)

	/// <summary>
	/// Copy constructor
	/// </summary>
	/// <param name="tString"></param>
	TString(const TString& tString)
		: TString(tString.data(), tString.len())
	{
	}

	/// <summary>
	/// Move constructor
	/// </summary>
	/// <param name="tString"></param>
	/// <returns></returns>
	TString(TString&& tString) noexcept;

	/// <summary>
	/// Construct from an initializer list
	/// </summary>
	/// <param name="lt"></param>
	explicit TString(const std::initializer_list<TString>& lt);	// Initializer list constructor (allows TString name{ "text", "text2", othertstring } )

#if TSTRING_TESTCODE
#pragma warning(push)
#pragma warning(disable: 26495) // warning C26495 : Variable 'TString::m_savedpos' is uninitialized.Always initialize a member variable(type.6: http://go.microsoft.com/fwlink/p/?LinkID=620422).

	/// <summary>
	/// Construct fropm a range of tokens
	/// *only used internally*
	/// </summary>
	/// <param name="rng"></param>
	explicit TString(const TokenRange& rng)
		: TString(rng.m_pStart, rng.m_pEnd)
	{}

#pragma warning(pop)
#endif

	// template version of the unique ptr constructor, handles WCHAR * & CHAR *
	// maybe add TString * support....

	/// <summary>
	/// Construct from a unique_ptr
	/// </summary>
	/// <param name="unique">- A unique_ptr of type char* or wchar_t*</param>
	template <TStringConcepts::IsPODText T>
	explicit TString(const std::unique_ptr<T>& unique)
		: TString(unique.get())
	{
		static_assert(TStringConcepts::IsPODText<T>, "MUST be a WCHAR or char string");
	}

	/// <summary>
	/// Construct from a string of a known length
	/// </summary>
	/// <param name="cString">- A string of type wchar_t*</param>
	/// <param name="iLen">- The strings length in characters</param>
	TString(const_pointer_const cString, const size_type iLen)
		: TString(iLen + 1U)
	{
		if ((cString) && (cString[0] != TEXT('\0')) && iLen > 0)
		{
			ts_strcpyn_throw(m_pString, cString, iLen + 1);
			//_ts_strcpyn(m_pString, cString, iLen + 1);
			m_iLen = iLen;
		}
		m_bDirty = false;
	}

	/// <summary>
	/// Construct from a string of a known length
	/// </summary>
	/// <param name="cString">- A string of type char*</param>
	/// <param name="iLen">- The strings length in characters</param>
	TString(const char* const cString, const size_type iLen)
		: TString()
	{
		if ((cString) && (cString[0] != 0) && iLen > 0)
		{
			m_buffer = charToWchar(cString, &m_buffersize);
			m_pString = m_buffer.get();

			if (!m_pString)
			{
				m_pString = &m_InternalBuffer[0];
				GSL_SUPPRESS(bounds.3) m_buffersize = std::size(m_InternalBuffer);
			}
			else {
				m_iLen = iLen;
				m_bUsingInternal = false;
			}
		}
		m_bDirty = false;
	}

#pragma warning(push)
#pragma warning(disable: 26495) // warning C26495 : Variable 'TString::m_savedpos' is uninitialized.Always initialize a member variable(type.6: http://go.microsoft.com/fwlink/p/?LinkID=620422).
	/// <summary>
	/// Construct from a sub string,
	/// start and end MUST be within the same string
	/// end must come after start
	/// </summary>
	/// <param name="pStart">- The start of the string to copy, can be char* or wchar_t*</param>
	/// <param name="pEnd">- The end of the string to copy</param>
	template <TStringConcepts::IsPODText T>
	TString(const T* const pStart, const T* const pEnd)
		: TString(pStart, gsl::narrow_cast<size_type>(pEnd - pStart))
	{
		static_assert(TStringConcepts::IsPODText<T>, "MUST be a WCHAR or char string");

		if (pEnd < pStart)
			throw std::invalid_argument("TString(): End of string < start");
	}
#pragma warning(pop)

	/// <summary>
	/// Construct from a single wchar_t
	/// </summary>
	/// <param name="chr"></param>
	/// <returns></returns>
	explicit TString(const WCHAR chr) noexcept;

	/// <summary>
	/// Construct from a single char
	/// </summary>
	/// <param name="chr"></param>
	/// <returns></returns>
	explicit TString(const char chr) noexcept;

	/// <summary>
	/// Construct with a pre-allocated buffer.
	/// </summary>
	/// <param name="tsSize">- Size of the buffer to allocate in characters.</param>
	explicit TString(const UINT tsSize);

	//! Destructor
	~TString() noexcept;

	// Operator Overloads

	/// <summary>
	/// Move assignment operator
	/// </summary>
	/// <param name="tString"></param>
	/// <returns></returns>
	TString& operator =(TString&& tString) noexcept;

	/// <summary>
	/// Single + operator that handles all supported types
	/// </summary>
	/// <param name="other">- The item to add to the string.</param>
	/// <returns>Previous string + new string.</returns>
	template <TStringConcepts::IsSupportedAddType T>
	TString operator +(const T& other)
	{
		auto newTString(*this);
		newTString += other;
		return newTString;
	}

	/// <summary>
	/// Single + operator that handles all supported types
	/// </summary>
	/// <param name="other">- The item to add to the string.</param>
	/// <param name="tString">- The string to add the new item to.</param>
	/// <returns>Previous string + new string.</returns>
	template <TStringConcepts::IsSupportedAddType T>
	friend TString operator +(const T& other, const TString& tString)
	{
		TString newTString(other);
		newTString += tString;
		return newTString;
	}

	TString& operator =(const TString& tString);
	TString& operator =(const WCHAR* const cString);
	TString& operator =(const WCHAR chr) noexcept;
	TString& operator =(const char* const cString);
	TString& operator =(const char chr) noexcept;

	/// <summary>
	/// Final version of the += operator, properly handles numeric types (char, wchar classified as non-numeric here)
	/// </summary>
	/// <param name="other">- The item to add to the string.</param>
	/// <returns>Previous string + new string.</returns>
	template <TStringConcepts::IsSupportedAddType T>
	GSL_SUPPRESS(lifetime.4)
		TString& operator +=(const T& other)
	{
		if constexpr (is_Numeric_v<T>)
			return append_number(other);
		else if constexpr (std::is_array_v<T>)
			return append(&other[0]);
		else
			return append(other);
	}

	template <TStringConcepts::IsSupportedCompareType T>
	bool operator <=(const T& other) const noexcept { return !(*this > other); }

	template <TStringConcepts::IsSupportedCompareType T>
	bool operator >=(const T& other) const noexcept { return !(*this < other); }

	template <TStringConcepts::IsSupportedCompareType T>
	bool operator ==(const T& other) const noexcept
	{
		if constexpr (std::is_array_v<T>)
			return (compare(&other[0]) == 0);
		else if constexpr (std::is_same_v<T, TString>)
			return (compare(other.to_chr()) == 0);
		else
			return (compare(other) == 0);
	}
	//template <>
	//bool operator ==(const nullptr_t& iNull) const noexcept { return (!m_pString && !iNull); }

	template <TStringConcepts::IsSupportedCompareType T>
	bool operator !=(const T& other) const noexcept { return !(*this == other); }

	template <TStringConcepts::IsSupportedCompareType T>
	bool operator <(const T& other) const noexcept
	{
		if constexpr (std::is_array_v<T>)
			return (compare(&other[0]) < 0);
		else
			return (compare(other) < 0);
	}

	template <TStringConcepts::IsSupportedCompareType T>
	bool operator >(const T& other) const noexcept
	{
		if constexpr (std::is_array_v<T>)
			return (compare(&other[0]) > 0);
		else
			return (compare(other) > 0);
	}

	template <TStringConcepts::IsSupportedRemoveType T>
	TString operator -(const T& other)
	{
		if (_ts_isEmpty(other))
			return *this;

		auto newTString(*this);
		newTString -= other;
		return newTString;
	}

	template <TStringConcepts::IsSupportedRemoveType T>
	TString& operator -=(const T& other)
	{
		return remove(other);
	}

	TString operator *(const int& N) const;
	TString& operator *=(const int& N);

	TString operator ++(int N) const
	{
		UNREFERENCED_PARAMETER(N);
		// NB: only works with space token
		// NB: post-increment works for us as we want to return a copy (the token)
		// NB: pre-increment doesn't work for this
		if (!m_savedpos) return getfirsttok(1);
		return getnexttok();
	}

	// returns the current token as set by ++ operator above (or getfirsttok()/getnexttok() with a space sepchar)
	const TString operator* () const
	{
		const_pointer p_cStart = m_pString;
		if (m_savedpos)
			p_cStart = m_savedpos;

		const auto* const p_cEnd = ts_strchr(p_cStart, SPACECHAR);
		if (!p_cEnd)
			return TString(p_cStart);

		return TString(p_cStart, p_cEnd);
	}

	//TString operator ++(int N) const {
	//	// NB: only works with space token
	//	// NB: post-increment works for us as we want to return a copy (the token)
	//	// NB: pre-increment doesn't work for this
	//	auto iCurrentTok = static_cast<int>(m_savedcurrenttok);
	//	pointer p_cStart, p_cEnd;
	//	size_type iToks;
	//
	//	std::tie(p_cStart, p_cEnd, iToks) = gettokenrange(iCurrentTok, iCurrentTok + 1);
	//
	//}
	//
	//TString &operator ++() const {
	//	// NB: only works with space token
	//}
	//
	//// returns the current token as set by ++ operator above (or getfirsttok()/getnexttok() with a space sepchar)
	//const TString operator* () const {
	//	const_pointer p_cStart = m_pString;
	//	if (m_savedpos != nullptr)
	//		p_cStart = m_savedpos;
	//	auto p_cEnd = ts_strchr(p_cStart, SPACECHAR);
	//	if (p_cEnd == nullptr)
	//		return TString(p_cStart);
	//
	//	return TString(p_cStart, p_cEnd);
	//}

#ifndef NDEBUG
	template <TStringConcepts::IsNumeric T>
	reference operator [](const T& N)
	{
		CheckRange(N);	// only debug code checks range here

		m_bDirty = true;

		return m_pString[N];
	}

	template <TStringConcepts::IsNumeric T>
	value_type operator [](const T& N) const
	{
		CheckRange(N);	// only debug code checks range here
		return m_pString[N];
	}
#else
	template <TStringConcepts::IsNumeric T>
	reference operator [](const T& N) noexcept
	{

		m_bDirty = true;

		return m_pString[N];
	}

	template <TStringConcepts::IsNumeric T>
	value_type operator [](const T& N) const noexcept
	{
		return m_pString[N];
	}
#endif

	template <TStringConcepts::IsSupportedAddType T>
	friend TString& operator <<(TString& other, const T& N) { other += N; return other; }
	template <TStringConcepts::IsNumeric T>
	friend TString& operator >>(const TString& other, T& N) { N = other.to_<T>(); return other; }
	template <TStringConcepts::IsSupportedAddType T>
	friend TString operator +(const TString& tString, const T& other)
	{
		auto newTString(tString);
		newTString += other;
		return newTString;
	}

	//conversion operators.
	//template <class T, typename = std::enable_if_t<is_Numeric_v<T>> >
	template <TStringConcepts::IsNumeric T>
	operator T() const
	{
		static_assert(is_Numeric_v<T>, "Type T must be (int, long, float, double, ....)");
		return to_<T>();
	}

	/// <summary>
	/// Convert to a bool
	/// returns false if the string is empty.
	/// </summary>
	/// <returns>true/false</returns>
	explicit operator bool() const noexcept { return !empty(); }
	//explicit operator WCHAR *() const noexcept { return m_pString; }

	// General String Lib

	/// <summary>
	/// Get the raw data.
	/// </summary>
	/// <returns>The actual string data.</returns>
	[[nodiscard]] const_pointer_const data() const noexcept { return m_pString; }
	//gsl::strict_not_null<const_pointer_const> data() const noexcept { return gsl::make_strict_not_null(m_pString); }
	//pointer data() const noexcept { return m_pString; }

	/// <summary>
	/// Get length of string in characters.
	/// </summary>
	/// <returns>The size of the string in characters.</returns>
	const size_t& len() const noexcept;
	/// <summary>
	/// Alias for len()
	/// </summary>
	/// <returns>The size of the string in characters.</returns>
	inline const size_t& length() const noexcept { return len(); };
	/// <summary>
	/// Alias for len()
	/// </summary>
	/// <returns>The size of the string in characters.</returns>
	inline const size_t& size() const noexcept { return len(); };
	/// <summary>
	/// Get the capacity of buffer in bytes.
	/// </summary>
	/// <returns>Capacity of buffer in bytes.</returns>
	const size_t& capacity() const noexcept { return m_buffersize; }
	/// <summary>
	/// Get the capacity of buffer in characters.
	/// </summary>
	/// <returns>Capacity of buffer in characters.</returns>
	const size_t capacity_cch() const noexcept { return capacity() / sizeof(value_type); }
	/// <summary>
	/// Get a pointer to the end of the buffer.
	/// </summary>
	/// <returns>Pointer to the end of the buffer.</returns>
	[[nodiscard]] const_pointer_const last() const noexcept { return m_pString + len(); }
	/// <summary>
	/// Clear string buffer and reset all vars and pointers (frees buffer)
	/// </summary>
	void clear() noexcept;
	/// <summary>
	/// Shrink string buffer to min size required for string (while still being a multiple of 16, & a min of 64)
	/// </summary>
	void shrink_to_fit();

	/// <summary>
	/// Append a single wide char
	/// </summary>
	/// <param name="chr">- wide character to append</param>
	/// <returns></returns>
	TString& append(const WCHAR& chr);
	/// <summary>
	/// Append a single ascii char
	/// </summary>
	/// <param name="chr">- ascii character to append</param>
	/// <returns></returns>
	TString& append(const char& chr);
	/// <summary>
	/// Append a wide char string.
	/// </summary>
	/// <param name="cString">- wide string to append</param>
	/// <returns></returns>
	TString& append(const WCHAR* const cString);
	/// <summary>
	/// Append a ascii char string.
	/// </summary>
	/// <param name="cString">- ascii string to append</param>
	/// <returns></returns>
	TString& append(const char* const cString);
	/// <summary>
	/// Append a TString object.
	/// </summary>
	/// <param name="tString">- TString to append</param>
	/// <returns></returns>
	TString& append(const TString& tString);
	// append a string thats limited to iChars characters.

	/// <summary>
	/// Append a character limited string.
	/// </summary>
	/// <param name="cString">- The start of the string to append.</param>
	/// <param name="iChars">- The number of characters to append.</param>
	/// <returns></returns>
	TString& append(const_pointer_const cString, const size_t iChars);

	/// <summary>
	/// Test if the string is empty.
	/// </summary>
	/// <returns>true/false</returns>
	const bool empty() const noexcept { return (!m_pString || m_pString[0] == TEXT('\0')); };

	/// <summary>
	/// Get a reference to the character at N.
	/// <para>This function range checks N.</para>
	/// </summary>
	/// <param name="N">- The character offset.</param>
	/// <returns>A reference to the character.</returns>
	reference at(const size_type N)
	{
		CheckRange(N);
		return m_pString[N];
	}
	/// <summary>
	/// Get a copy of the character at N.
	/// <para>This function range checks N.</para>
	/// </summary>
	/// <param name="N">- The character offset.</param>
	/// <returns>A copy of the character.</returns>
	value_type at(const size_type N) const
	{
		CheckRange(N);
		return m_pString[N];
	}
	// allocate memory for string, preserves contents...

	/// <summary>
	/// Reserve a buffer of X characters.
	/// </summary>
	/// <param name="tsSize">Characters to reserve.</param>
	void reserve(const size_t tsSize);
	// copy string...

	/// <summary>
	/// Copy another TString to this one.
	/// </summary>
	/// <param name="other">- TString to copy</param>
	/// <returns></returns>
	void copy(TString other) noexcept;

	/// <summary>
	/// Compare this object to a TString.
	/// </summary>
	/// <param name="other"></param>
	/// <returns>-1 if less, 0 if equal, 1 if greater</returns>
	int compare(const TString& other) const noexcept;
	/// <summary>
	/// Compare this object to a character.
	/// </summary>
	/// <param name="other">- Character to compare against.</param>
	/// <returns>-1 if less, 0 if equal, 1 if greater</returns>
	int compare(const_reference other) const noexcept;
	/// <summary>
	/// Compare this object to a pod string.
	/// </summary>
	/// <param name="other">- String to compare against.</param>
	/// <returns>-1 if less, 0 if equal, 1 if greater</returns>
	int compare(const_pointer_const other) const noexcept;
	/// <summary>
	/// Compare this object to a pod string of limited length.
	/// </summary>
	/// <param name="other">- String to compare against.</param>
	/// <param name="iLength">- Number of characters to compare.</param>
	/// <returns>-1 if less, 0 if equal, 1 if greater</returns>
	int compare(const_pointer_const other, const size_t iLength) const noexcept;
	/// <summary>
	/// Compare 'this' to an array.
	/// </summary>
	/// <typeparam name="T">Array type can be anything supported by the == operator.</typeparam>
	/// <param name="array">- The array to compare against</param>
	/// <returns>The index of the matching item, or zero for failure, Index is One based.</returns>
	template <TStringConcepts::IsSupportedCompareType T, UINT iArraySize>
	UINT acompare(const T(&array)[iArraySize]) const noexcept
	{
		for (auto i = decltype(iArraySize){0}; i < iArraySize; ++i)
		{
			if (*this == array[i])
				return i + 1;
		}
		return 0U;

		//UINT i{};
		//for (const auto& a : array)
		//{
		//	if (*this == a)
		//		return i + 1;
		//}
		//return 0U;
	}

#if TSTRING_TESTCODE
	/// <summary>
	/// Function to find position or number of occurences of a substring in the string.
	/// </summary>
	/// <param name="substring">- Substring to search</param>
	/// <param name="N">- Nth substring to search (N = 0 > Total number of matches)</param>
	/// <returns><para>Number of occurrences (N = 0),</para><para>Starting position of substring,</para><para>-1 if function fails or no substring was found</para></returns>
	template <TStringConcepts::IsSupportedCompareType T>
	int find(const T& substring, const int N) const noexcept
	{
		if (!_ts_isEmpty(substring) && !_ts_isEmpty(m_pString))
		{
			const_pointer temp = nullptr, temp2 = m_pString;

			auto i = decltype(N){0};
			const auto subl = _ts_strlen(substring);
			while ((temp = _ts_find(temp2, substring)) != nullptr)
			{
				++i;
				//if ( N != 0 && i == N )
				if (i == N) // i is never zero
					return gsl::narrow_cast<int>(temp - m_pString);
				temp2 = (temp + subl);
			}
			if (N == 0)
				return i;
		}
		return -1;
	}

#else
	int find(const_pointer_const substring, const int N) const noexcept;	// find Nth matching subString
	int find(const_value_type chr, const int N) const noexcept;			// find Nth matching chr
#endif
	/// <summary>
	/// Get a sub string.
	/// </summary>
	/// <param name="N">- The character offset to start from.</param>
	/// <param name="M">- The character offset to finish at.</param>
	/// <returns>A new object containing the substring</returns>
	TString sub(int N, int M) const;

	/// <summary>
	/// Remove extra spaces at the start and end of the string.
	/// </summary>
	/// <returns>A reference to this object after its trimmed.</returns>
	TString& trim();
	/// <summary>
	/// Remove extra spaces at the start and end of the string, and all ctrl codes in text.
	/// </summary>
	/// <returns>A reference to this object after its stripped.</returns>
	TString& strip();

#if TSTRING_TESTCODE
	//	Replace
	//		subString	-	String to be replaced (can be char/wchar_t/char */wchar_t */TString/std:string/std::wstring)
	//		rString		-	String to replace subString with (can be any of the types supported but doesnt have to be the same type as subString)
	//						rString can also be zero, if zero then this function does a remove.
	template <TStringConcepts::IsSupportedRemoveType T, TStringConcepts::IsSupportedRemoveType M>
	UINT replace(const T& subString, const M& rString)
	{
		if (empty())
			return 0U;

		const auto subl = _ts_strlen(subString);

		if (subl == 0)
			return 0U;

		const auto repl = _ts_strlen(rString);

		auto c = 0U;
		TString tmp;
		auto pStart = m_pString;
		const auto* const pfEnd = m_pString + length();
		auto pEnd = decltype(pStart){0};

		while ((pEnd = _ts_find(pStart, subString)) != nullptr)
		{
			if (pStart != pEnd)
				//tmp += TString(pStart, pEnd);
				tmp.append(pStart, gsl::narrow_cast<size_t>(pEnd - pStart));

			if (repl > 0)
				tmp += rString;

			++c;

			pStart = (pEnd + subl);
			if (pStart > pfEnd)
				break;
		}
		if (pStart < pfEnd)
			tmp += pStart;

		swap(tmp);

		return c;
	}
	UINT mreplace(const_value_type chr, const_pointer_const fmt) noexcept;					// replace any char in fmt with chr

	template <TStringConcepts::IsSupportedRemoveType T>
	TString& remove(const T& str)
	{
		const_value_type sTmp[2]{};
		replace(str, &sTmp[0]);
		return *this;
	}

#else
	// remove sub string from string
	TString& remove(const_pointer_const subString);
	// remove character from string
	TString& remove(const_reference chr);
	// remove sub string from string
	TString& remove(const TString& subString);
	UINT replace(const_pointer_const subString, const_pointer_const rString);	// replace subString with rString
	UINT replace(const_pointer_const subString, constvalue_type rchr);			// replace subString with rchr
	UINT replace(const_value_type chr, const_pointer_const rString);				// replace chr with rString
	UINT replace(const_value_type chr, const_value_type rchr);						// replace chr with rchr
	UINT mreplace(const_value_type chr, const_pointer_const fmt);					// replace any char in fmt with chr
#endif

	// Token Lib
#if !TSTRING_TESTCODE
	template <typename T>
	void addtok(const T& tToken)
	{
		addtok(tToken, SPACECHAR);
	}

	template <typename T, typename TS>
	void addtok(const T& tToken, const TS& sepChars)
	{
		if (!tToken || !sepChars)
			return;

		if (!empty())
			*this += sepChars;

		*this += tToken;
	}

	template <class T, class R = size_t>
	R findtok(const T& cToken, const UINT N, const TCHAR* const sepChars = SPACE) const
	{
		//size_t count = 0;
		//for (auto itStart = begin(sepChars), itEnd = end(); itStart != itEnd; ++itStart)
		//{
		//	if (*itStart == cToken)
		//	{
		//		++count;
		//
		//		if (count == N)
		//			return itStart;
		//	}
		//}

		auto itEnd = end();

		if (sepChars == nullptr || this->empty() || !cToken)
			return itEnd;

		//auto count = decltype(N){0};
		//for (auto got = std::find(begin(sepChars), itEnd, cToken); got != itEnd; got = std::find(got, itEnd, cToken))
		//{
		//	if (++count == N)
		//		return got;
		//}
		//return itEnd;

		//code below has issues due to way iterator works & gets converted to numbers on return (fixed)
		auto count = decltype(N){0};
		return std::find_if(begin(sepChars), itEnd, [&count, &N, &cToken](const auto& x) {
			if (x == cToken) {
				if (++count == N)
					return true;
			}
			return false;
			});
	}

	template <class T>
	void remtok(const T& cToken, const UINT N, const TCHAR* const sepChars = SPACE) {
		//const auto tokennr = findtok(cToken, N, sepChars);
		//if (tokennr > 0)
		//	deltok(tokennr, sepChars);

		deltok(findtok(cToken, N, sepChars), sepChars);
	}

	template <class T>
	auto istok(const T& cToken, const TCHAR* const sepChars = SPACE) const { return findtok<T, bool>(cToken, 1, sepChars); }

	template <typename T>
	void instok(const T& cToken, const UINT N, const TCHAR* const sepChars = SPACE)
	{
		if (sepChars == nullptr)
			return;

		if (N < 1)
			return;

		const auto nToks = numtok(sepChars);

		TString tsTmp;
		if (N == 1) {
			// N == 1
			// so just add new token to start & add existing token(s) at end.
			tsTmp += cToken;
			if (nToks > 0) {
				tsTmp += sepChars;
				tsTmp += *this;
			}
		}
		else {
			// N >= 2
			// so add preceding tokens
			tsTmp = gettok(1, N - 1, sepChars);
			// then new token
			tsTmp += sepChars;
			tsTmp += cToken;
			// and any tokens after the inserted one.
			if (N < nToks) {
				tsTmp += sepChars;
				tsTmp += gettok(N, -1, sepChars);
			}
		}
		swap(tsTmp);
	}

	template <typename T>
	void puttok(const T& cToken, const UINT N, const TCHAR* const sepChars = SPACE)
	{
		const auto nToks = numtok(sepChars);
		if (N == 1)
		{
			// replace first token
			TString tmp(cToken);
			if (nToks > N)
			{
				tmp += sepChars;
				tmp += gettok(N + 1, -1, sepChars);
			}
			this->swap(tmp);
		}
		else if (nToks > N)
		{
			// replace middle token
			//TString tmp{ gettok(1, N - 1, sepChars), sepChars, cToken, sepChars, gettok(N + 1, -1, sepChars) };
			TString tmp(gettok(1, N - 1, sepChars));
			tmp.addtok(cToken, sepChars);
			tmp.addtok(gettok(N + 1, -1, sepChars), sepChars);

			this->swap(tmp);
		}
		else {
			// replace last token
			//TString tmp{ gettok(1, N - 1, sepChars), sepChars, cToken };
			TString tmp(gettok(1, N - 1, sepChars));
			tmp.addtok(cToken, sepChars);

			this->swap(tmp);
		}
	}

	template <typename T, typename M>
	void reptok(const T& cToken, const M& newToken, const UINT N, const TCHAR* const sepChars = SPACE)
	{
		const auto pos = findtok(cToken, N, sepChars);
		if (pos > 0)
			puttok(newToken, pos, sepChars);
	}

	void deltok(const UINT N, const TCHAR* const sepChars = SPACE);

	TString gettok(int N, const_pointer_const sepChars = SPACE) const;
	TString gettok(int N, int M, const_pointer_const sepChars = SPACE) const;
	TString getfirsttok(const UINT N, const_pointer_const sepChars) const;					// must be called before the first getnexttok()
	TString getfirsttok(const size_type N, const_reference sepChar = SPACECHAR) const;	// must be called before the first getnexttok()
	TString getnexttok(const_pointer_const sepChars) const;									// gets subsequent tokens after a getfirsttok() call.
	TString getnexttok(const_reference sepChars = SPACECHAR) const;						// gets subsequent tokens after a getfirsttok() call.

	TString getlasttoks() const;															// gets all remaining tokens after a getfirsttok()/getnexttok() call.
	const bool moretokens() const noexcept { return (m_savedpos != nullptr); }
	void resettokens() const noexcept {
		m_savedcurrenttok = 0;
		m_savedpos = nullptr;
		m_savedtotaltoks = 0;
	}
	TString matchtok(const_pointer_const mString, UINT N, const_pointer_const sepChars = SPACE) const;
	size_t numtok(const_pointer_const sepChars) const noexcept;
	size_t numtok(const_reference sepChar = SPACECHAR) const noexcept;
#else
	template <TStringConcepts::IsSupportedAddType T, TStringConcepts::IsSupportedSeperatorType TSepChars = const_reference>
	void addtok(const T& tToken, TSepChars sepChars = SPACECHAR)
	{
		// allows adding zero number & empty tokens.
		if (_ts_isEmpty(sepChars))
			return;

		if (!empty())
			*this += sepChars;

		*this += tToken;
	}

	template <TStringConcepts::IsSupportedCompareType T, TStringConcepts::IsNumeric R = size_t, TStringConcepts::IsSupportedSeperatorType TSepChars = const_reference>
	R findtok(const T& cToken, const UINT N, TSepChars sepChars = SPACECHAR) const
	{
		const auto itEnd = end();

		if (_ts_isEmpty(sepChars) || empty() || !cToken)
			return itEnd;

		//code below has issues due to way iterator works & gets converted to numbers on return (fixed)
		auto count = decltype(N){0};
		return std::find_if(begin(sepChars), itEnd, [&count, &N, &cToken](const auto& x) noexcept {
			if (x == cToken)
			{
				if (++count == N)
					return true;
			}
			return false;
			});
	}

	template <TStringConcepts::IsSupportedCompareType T, TStringConcepts::IsSupportedSeperatorType TSepChars = const_reference>
	void remtok(const T& cToken, const UINT N, TSepChars sepChars = SPACECHAR)
	{
		deltok(findtok(cToken, N, sepChars), sepChars);
	}

	template <TStringConcepts::IsSupportedCompareType T, TStringConcepts::IsSupportedSeperatorType TSepChars = const_reference>
	auto istok(const T& cToken, TSepChars sepChars = SPACECHAR) const { return findtok<T, bool>(cToken, 1, sepChars); }

	template <TStringConcepts::IsSupportedAddType T, TStringConcepts::IsSupportedSeperatorType TSepChars = const_reference>
	void instok(const T& cToken, const UINT N, TSepChars sepChars = SPACECHAR)
	{
		if (_ts_isEmpty(sepChars))
			return;

		if (N < 1)
			return;

		TString tsTmp;
		if (const auto nToks = numtok(sepChars); N == 1)
		{
			// N == 1
			// so just add new token to start & add existing token(s) at end.
			tsTmp += cToken;
			if (nToks > 0)
			{
				tsTmp += sepChars;
				tsTmp += *this;
			}
		}
		else {
			// N >= 2
			// so add preceding tokens
			tsTmp = gettok(1, gsl::narrow_cast<int>(N) - 1, sepChars);
			// then new token
			tsTmp += sepChars;
			tsTmp += cToken;
			// and any tokens after the inserted one.
			if (N < nToks)
			{
				tsTmp += sepChars;
				tsTmp += gettok(gsl::narrow_cast<int>(N), -1, sepChars);
			}
		}
		swap(tsTmp);
	}

	template <TStringConcepts::IsSupportedAddType T, TStringConcepts::IsSupportedSeperatorType TSepChars = const_reference>
	void puttok(const T& cToken, const UINT N, TSepChars sepChars = SPACECHAR)
	{
		if (const auto nToks = numtok(sepChars); N == 1)
		{
			// replace first token
			TString tmp(cToken);
			if (nToks > N)
			{
				tmp += sepChars;
				tmp += gettok(gsl::narrow_cast<int>(N) + 1, -1, sepChars);
			}
			swap(tmp);
		}
		else if (nToks > N)
		{
			// replace middle token
			TString tmp(gettok(1, gsl::narrow_cast<int>(N) - 1, sepChars));
			tmp.addtok(cToken, sepChars);
			tmp.addtok(gettok(gsl::narrow_cast<int>(N) + 1, -1, sepChars), sepChars);

			swap(tmp);
		}
		else if (nToks < N)
		{
			// add token to end (nothing to replace)
			addtok(cToken, sepChars);
		}
		else {
			// replace last token
			TString tmp(gettok(1, gsl::narrow_cast<int>(N) - 1, sepChars));
			tmp.addtok(cToken, sepChars);

			swap(tmp);
		}
	}

	template <TStringConcepts::IsSupportedCompareType T, TStringConcepts::IsSupportedAddType M, TStringConcepts::IsSupportedSeperatorType TSepChars = const_reference>
	void reptok(const T& cToken, const M& newToken, const UINT N, TSepChars sepChars = SPACECHAR)
	{
		if (const auto pos = findtok(cToken, N, sepChars); pos > 0)
			puttok(newToken, pos, sepChars);
	}

	/*!
	* \brief blah
	*
	* blah
	*/

	template <TStringConcepts::IsSupportedSeperatorType TSepChars = const_reference>
	void deltok(const UINT N, TSepChars sepChars = SPACECHAR)
	{
		if (_ts_isEmpty(sepChars) || N < 1 || empty())
			return;

		const auto nToks = numtok(sepChars);

		if (N > nToks)
			return;

		if (N == 1 && nToks == 1)
		{
			clear();
			return;
		}

		pointer p_cStart = m_pString, p_cEnd = nullptr;
		auto i = decltype(N){0};
		const auto sepl = _ts_strlen(sepChars);

		while ((p_cEnd = _ts_find(p_cStart, sepChars)) != nullptr)
		{
			++i;

			if (i == N)
				break;

			p_cStart = p_cEnd + sepl;
		}

		const auto l = len();

		TString tmp(l);

		// last token
		if (!p_cEnd)
		{
			--p_cStart;
			*p_cStart = 0;
			p_cEnd = m_pString;
		}
		// delete the first token
		else if (p_cStart == m_pString)
			++p_cEnd;
		// inbound token
		else {
			*p_cStart = 0;
			++p_cEnd;

			ts_strncat_throw(tmp.m_pString, m_pString, l);
		}
		ts_strncat_throw(tmp.m_pString, p_cEnd, l);

		swap(tmp);
	}

	TString getlasttoks() const;															// gets all remaining tokens after a getfirsttok()/getnexttok() call.
	const bool moretokens() const noexcept { return (m_savedpos != nullptr); }
	void resettokens() const noexcept
	{
		m_savedcurrenttok = 0;
		m_savedpos = nullptr;
		m_savedtotaltoks = 0;
	}

	/*!
	* \brief blah
	*
	* blah
	*/
	template <TStringConcepts::IsSupportedSeperatorType TSepChars = const_reference>
	TString matchtok(const_pointer_const mString, UINT N, TSepChars sepChars = SPACECHAR) const
	{
		auto count = decltype(N){0};

		if (const auto itEnd = end(), itGot = std::find_if(begin(sepChars), itEnd, [&count, &N, &mString](const auto& x) noexcept {
			if (_ts_find(x.to_chr(), mString))
			{
				++count;
				if (count >= N)
					return true;
			}
			return false;
			}); itGot != itEnd)
			return *itGot;

			return TString();
	}

	/*!
	* \brief Returns the number of tokens in the string
	*
	* params
	*	sepChars = The token seperator.
	*/
	template <TStringConcepts::IsSupportedSeperatorType T = const_reference>
	size_t numtok(T sepChars = SPACECHAR) const noexcept
	{
		if (_ts_isEmpty(sepChars) || empty())
			return 0U;

		const_pointer p_cStart = m_pString, p_cEnd = nullptr;
		auto iCount = 0U;
		const auto sepl = _ts_strlen(sepChars);

		while ((p_cEnd = _ts_find(p_cStart, sepChars)) != nullptr)
		{
			++iCount;
			p_cStart = p_cEnd + sepl;
		}
		return iCount + 1U;
	}

	//const tuple_type gettokenrange(const int nStart, const int nEnd, const_pointer sepChars) const;
	//const tuple_type gettokenrange(const int nStart, const int nEnd, const_reference sepChars = SPACECHAR) const;
	//template <typename TSepChars = const_reference>
	//const TString::tuple_type gettokenrange(const int nStart, const int nEnd, TSepChars sepChars = SPACECHAR) const noexcept
	//{
	//	const_pointer p_cStart = m_pString, p_cEnd = nullptr, p_fEnd = last();
	//
	//	if (_ts_isEmpty(sepChars))
	//		return { p_cStart, p_fEnd, decltype(m_savedtotaltoks){1} };
	//
	//	const auto nToks = numtok(sepChars);
	//	auto iStart = nStart;
	//
	//	if (iStart < 0)
	//		iStart += (nToks + 1);
	//
	//	if (p_cStart == nullptr || p_fEnd == nullptr || iStart < 1 || ((nEnd < iStart) && (nEnd != -1)) || static_cast<size_t>(iStart) > nToks)
	//		return { nullptr, nullptr, decltype(m_savedtotaltoks){0} };
	//
	//	const auto bFullstring = ((static_cast<size_t>(nEnd) >= nToks) || (nEnd < 0));
	//
	//	const_pointer p_cFirst = nullptr, p_cLast = nullptr;
	//	auto iCount = 0;
	//	const auto sepl = _ts_strlen(sepChars);
	//
	//	while ((p_cEnd = _ts_find(p_cStart, sepChars)) != nullptr)
	//	{
	//		++iCount;
	//
	//		if (iCount == iStart)
	//		{
	//			p_cFirst = p_cStart;
	//
	//			if (bFullstring)
	//				break;
	//		}
	//
	//		if (iCount == nEnd)
	//		{
	//			p_cLast = p_cEnd;
	//			break;
	//		}
	//
	//		p_cStart = p_cEnd + sepl;
	//		if (p_cStart >= p_fEnd)	// look out for overrun...
	//			break;
	//	}
	//
	//	if (bFullstring)
	//	{
	//		if (static_cast<size_t>(iCount) == (nToks - 1))
	//			p_cFirst = p_cStart;
	//
	//		p_cLast = p_fEnd;
	//	}
	//	else if (static_cast<size_t>(iCount) == (nToks - 1))
	//		p_cLast = p_cEnd;
	//
	//	return { p_cFirst, p_cLast, nToks };
	//}

	template <TStringConcepts::IsSupportedSeperatorType TSepChars = const_reference>
	const TokenRange gettokenrange(const int nStart, const int nEnd, TSepChars sepChars = SPACECHAR) const noexcept
	{
		const_pointer p_cStart = m_pString, p_cEnd = nullptr, p_fEnd = last();

		if (_ts_isEmpty(sepChars))
			return { p_cStart, p_fEnd, decltype(m_savedtotaltoks){1} };

		const auto nToks = numtok(sepChars);
		auto iStart = nStart;

		if (iStart < 0)
			iStart += (nToks + 1);

		if (!p_cStart || !p_fEnd || iStart < 1 || ((nEnd < iStart) && (nEnd != -1)) || gsl::narrow_cast<size_t>(iStart) > nToks)
			return { };

		const auto bFullstring = ((gsl::narrow_cast<size_t>(nEnd) >= nToks) || (nEnd < 0));

		const_pointer p_cFirst = nullptr, p_cLast = nullptr;
		auto iCount = 0;
		const auto sepl = _ts_strlen(sepChars);

		while ((p_cEnd = _ts_find(p_cStart, sepChars)) != nullptr)
		{
			++iCount;

			if (iCount == iStart)
			{
				p_cFirst = p_cStart;

				if (bFullstring)
					break;
			}

			if (iCount == nEnd)
			{
				p_cLast = p_cEnd;
				break;
			}

			p_cStart = p_cEnd + sepl;
			if (p_cStart >= p_fEnd)	// look out for overrun...
				break;
		}

		if (bFullstring)
		{
			if (gsl::narrow_cast<size_t>(iCount) == (nToks - 1))
				p_cFirst = p_cStart;

			p_cLast = p_fEnd;
		}
		else if (gsl::narrow_cast<size_t>(iCount) == (nToks - 1))
			p_cLast = p_cEnd;

		return { p_cFirst, p_cLast, nToks };
	}

	template <TStringConcepts::IsSupportedSeperatorType T = const_reference>
	inline TString gettok(const int N, const int M, T sepChars = SPACECHAR) const
	{
		return TString(gettokenrange(N, M, sepChars));
	}

	template <TStringConcepts::IsSupportedSeperatorType T = const_reference>
	inline TString gettok(const int N, T sepChars = SPACECHAR) const
	{
		return TString(gettokenrange(N, N, sepChars));
	}

	template <TStringConcepts::IsSupportedSeperatorType T = const_reference>
	inline TString getfirsttok(const int N, T sepChars = SPACECHAR) const
	{
		//const auto rng = gettokenrange(N, N, sepChars);
		//
		//m_savedcurrenttok = static_cast<UINT>(N);
		//m_savedtotaltoks = std::get<2>(rng);
		//m_savedpos = std::get<1>(rng);
		//
		//if (m_savedpos != nullptr)
		//	++m_savedpos;
		//
		//return rng;

		const auto rng = gettokenrange(N, N, sepChars);

		m_savedcurrenttok = gsl::narrow_cast<UINT>(N);
		m_savedtotaltoks = rng.m_count;
		m_savedpos = rng.m_pEnd;

		if (m_savedpos)
			++m_savedpos;

		return TString(rng);
		//return rng;
	}

	/*!
	* \brief Get the next token as a specified integral type (can only be used after a getfirsttok() call)
	*
	* params
	* sepChars	-	The token seperator string/character
	*				If NULL returns zero.
	*/
	template <TStringConcepts::IsNumeric T, TStringConcepts::IsSupportedSeperatorType TSepChar = const_reference>
	T getnexttokas(TSepChar sepChars = SPACECHAR/*, bool bReCount = false*/) const
	{
		//return getnexttok(sepChars).to_<T>();

		++m_savedcurrenttok;

		//if (bReCount)
		//	m_savedtotaltoks = numtok(sepChars);

		if (_ts_isEmpty(sepChars) || !m_pString || !m_savedpos || m_savedcurrenttok > m_savedtotaltoks)
			return T();

		if (const_pointer_const p_cStart = m_savedpos; m_savedcurrenttok == m_savedtotaltoks)
		{
			m_savedpos = nullptr;
			return TString(p_cStart).to_<T>();
			//return Dcx::template parse_string<T, value_type>(p_cStart);
		}
		else if (const_pointer_const p_cEnd = _ts_find(p_cStart, sepChars); p_cEnd)
		{
			m_savedpos = (p_cEnd + _ts_strlen(sepChars));
			return TString(p_cStart, p_cEnd).to_<T>();
		}
		return T();
	}

	/*!
	* \brief Get the next token from the string (can only be used after a getfirsttok() call)
	*
	* params
	* sepChars	-	The token seperator string
	*				If NULL returns the whole current string.
	*/
	template <TStringConcepts::IsSupportedSeperatorType TSepChar = const_reference>
	TString getnexttok(TSepChar sepChars = SPACECHAR/*, bool bReCount = false*/) const
	{
		++m_savedcurrenttok;

		if (_ts_isEmpty(sepChars) || !m_pString)
			return *this;

		//if (bReCount)
		//	m_savedtotaltoks = numtok(sepChars);

		if (!m_savedpos || m_savedcurrenttok > m_savedtotaltoks)
			return TString();

		if (const_pointer_const p_cStart = m_savedpos; m_savedcurrenttok == m_savedtotaltoks)
		{
			m_savedpos = nullptr;
			return TString(p_cStart);
		}
		else if (const auto* const p_cEnd = _ts_find(p_cStart, sepChars); p_cEnd)
		{
			m_savedpos = (p_cEnd + _ts_strlen(sepChars));
			return TString(p_cStart, p_cEnd);
		}
		return TString();
	}

#endif

	struct SortOptions final
	{
		bool bAlpha{ false };
		bool bNumeric{ false };
		bool bPrefix{ false };
		bool bReverse{ false };
	};

	void sorttok(const_pointer_const sortOptions, const_pointer_const sepChars = SPACE);	// The default is an alphabetic sort, however you can specify n = numeric sort, c = channel nick prefix sort, r = reverse sort, a = alphanumeric sort.
	void sorttok(const SortOptions& sortOptions, const_pointer_const sepChars = SPACE);		// The default is an alphabetic sort, however you can specify n = numeric sort, c = channel nick prefix sort, r = reverse sort, a = alphanumeric sort.

	TString wildtok(const_pointer_const wildString, const UINT N, const_pointer_const sepChars = SPACE) const;
	UINT nwildtok(const_pointer_const wildString, const_pointer_const sepChars = SPACE) const;

	template <TStringConcepts::IsTString tsType, TStringConcepts::IsPODText T = const TCHAR>
	class tsIterator final
		//: public std::iterator<std::forward_iterator_tag, tsType, ptrdiff_t, tsType*, tsType&>
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = tsType;
		using difference_type = ptrdiff_t;
		using pointer = tsType*;
		using reference = tsType&;

		tsIterator(const tsIterator<tsType, T>& other) noexcept = default;
		tsIterator(tsType* ptr = nullptr) noexcept
			: tsIterator(ptr, nullptr)
		{
			if (!m_ptr)
				m_iIndex = 0;
		}
		tsIterator(tsType* ptr, T* const sepChars) noexcept
			: m_ptr{ ptr }, m_iIndex{ 1 }, m_sepChars{ sepChars }, m_sepChar{ T{}, T{} }
		{
			if (!m_sepChars)
			{
				m_sepChar[0] = SPACECHAR;
				m_sepChar[1] = T();
				m_sepChars = &m_sepChar[0];
				m_sepCharsLen = 1;
			}
			else
				m_sepCharsLen = _ts_strlen(m_sepChars);

			if (m_ptr)
			{
				m_toks = m_ptr->numtok(m_sepChars);
				m_savedStart = m_ptr->m_pString;
				if (m_savedStart)
				{
					m_savedEnd = ts_strstr(m_savedStart, m_sepChars);
					m_savedFinal = m_ptr->last();
				}
			}
		}
		tsIterator(tsType* ptr, T& sepChar) noexcept
			: m_ptr{ ptr }, m_iIndex{ 1 }, m_sepChar{ sepChar, T() }, m_sepChars{ &m_sepChar[0] }
		{
			if (sepChar == T())
				m_sepChar[0] = SPACECHAR;

			m_sepCharsLen = 1;

			if (m_ptr)
			{
				m_toks = m_ptr->numtok(m_sepChars);
				m_savedStart = m_ptr->m_pString;
				if (m_savedStart)
				{
					m_savedEnd = ts_strstr(m_savedStart, m_sepChars);
					m_savedFinal = m_ptr->last();
				}
			}
		}
		//~tsIterator() noexcept {}

		tsIterator<tsType, T>& operator = (const tsIterator<tsType, T>& other) = default;
		tsIterator<tsType, T>& operator = (tsType* ptr) noexcept
		{
			m_ptr = ptr;
			m_sepChars = SPACE;
			m_sepCharsLen = 1;
			m_iIndex = 0;
			m_toks = 0;
			m_savedStart = nullptr;
			m_savedEnd = nullptr;
			m_savedFinal = nullptr;

			if (m_ptr)
			{
				m_iIndex = 1;
				m_toks = m_ptr->numtok(m_sepChars);
				m_savedStart = m_ptr->m_pString;
				if (m_savedStart)
				{
					m_savedEnd = ts_strstr(m_savedStart, m_sepChars);
					//m_savedFinal = (TCHAR *)(m_savedStart + m_ptr->len());
					m_savedFinal = m_ptr->last();
				}
			}
			return *this;
		}

		explicit operator bool() const noexcept { return (m_ptr != nullptr); }
		operator size_t() const noexcept { return m_iIndex; }

		bool operator == (const tsIterator<tsType, T>& other) const noexcept { return (m_ptr == other.getConstPtr()); }
		bool operator != (const tsIterator<tsType, T>& other) const noexcept { return (m_ptr != other.getConstPtr()); }

		const tsIterator<tsType, T>& operator++ () noexcept
		{
			++m_iIndex;

			m_savedStart = ((m_savedEnd + m_sepCharsLen));

			if ((m_iIndex > m_toks) || (m_savedStart >= m_savedFinal))
			{
				m_iIndex = 0;
				m_ptr = nullptr;
				m_sepChars = nullptr;
				m_sepCharsLen = 0;
				m_toks = 0;
				m_savedStart = nullptr;
				m_savedEnd = nullptr;
				m_savedFinal = nullptr;
			}
			else
				m_savedEnd = ts_strstr(m_savedStart, m_sepChars);

			return *this;
		}

		tsType operator* () const
		{
			// should we return a blank/empty tsType here instead of throwing an exception?
			if (!m_ptr)
				throw std::out_of_range("TString::iterator");

			if (!m_sepChars || !m_savedStart)
				return tsType();

			if ((m_iIndex == m_toks) || (!m_savedEnd))
				return tsType(m_savedStart);

			return tsType(m_savedStart, m_savedEnd);
		}

		//std::wstring_view operator* () const noexcept
		//{
		//	// should we return a blank/empty tsType here instead of throwing an exception?
		//	if (!m_ptr || !m_sepChars || !m_savedStart)
		//		return{};
		//
		//	if ((m_iIndex == m_toks) || (!m_savedEnd))
		//		return std::wstring_view(m_savedStart);
		//
		//	return std::wstring_view(m_savedStart, m_savedEnd - m_savedStart);
		//}

		tsType* getPtr() const noexcept { return m_ptr; }
		const tsType* getConstPtr() const noexcept { return m_ptr; }
		const size_t& getIndex() const noexcept { return m_iIndex; }

		//protected:
	private:
		tsType* m_ptr{ nullptr };
		size_t	m_iIndex{};
		size_t	m_sepCharsLen{};
		size_t	m_toks{};
		T* m_sepChars{ nullptr };
		mutable const_pointer m_savedStart{ nullptr };
		mutable const_pointer m_savedEnd{ nullptr };
		const_pointer m_savedFinal{ nullptr };
		std::remove_cv_t<T>		m_sepChar[2]{};
	};

	using iterator = tsIterator<TString, const_value_type>;
	using const_iterator = tsIterator<const TString, const_value_type>;

	inline iterator begin() noexcept { return iterator(this); }
	inline iterator begin(const_pointer_const sepChars) noexcept { return iterator(this, sepChars); }
	inline iterator begin(const_reference sepChar) noexcept { return iterator(this, sepChar); }
	inline iterator end() noexcept { return iterator(); }
	inline const const_iterator begin() const noexcept { return const_iterator(this); };
	inline const const_iterator begin(const_pointer_const sepChars) const noexcept { return const_iterator(this, sepChars); }
	inline const const_iterator begin(const_reference sepChar) const noexcept { return const_iterator(this, sepChar); }
	inline const const_iterator end() const noexcept { return const_iterator(); }

	// add the contents of any container class to the TString using the specified seperator (can also add other TString's like this)
	template <TStringConcepts::IsContainer T, TStringConcepts::IsSupportedSeperatorType TS = const_reference>
	auto& join(const T& Cont, const TS sepChars = SPACECHAR)
	{
		for (const auto& x : Cont)
		{
			addtok(x, sepChars);
		}
		return *this;
	}

	// fill a container class with a split up TString
	template <TStringConcepts::IsContainer T, TStringConcepts::IsSupportedSeperatorType TS = const_reference>
	void expand(T& Cont, const TS sepChars = SPACECHAR)
	{
		for (auto itStart = begin(sepChars), itEnd = end(); itStart != itEnd; ++itStart)
		{
			Cont.push_back((*itStart));
			//Cont.emplace_back((*itStart));
		}
	}

#if INCLUDE_MIRC_EXTRAS
	// extras for mIRC
	bool isnum(const bool bAllowNegatives) const noexcept;
	bool isincs(const_value_type let) const noexcept;
	UINT countchar(const_value_type chr) const noexcept;
	bool ishostmask(void) const noexcept;

	TString toupper(void) const;
	TString tolower(void) const;

#endif

#if TSTRING_TESTCODE
	template <TStringConcepts::IsSupportedCompareType T> bool iswm(const T& a) const noexcept(std::is_nothrow_move_assignable_v<T>) { return _ts_WildcardMatch(*this, a); }
	template <TStringConcepts::IsSupportedCompareType T> bool iswmcs(const T& a) const noexcept(std::is_nothrow_move_assignable_v<T>) { return _ts_WildcardMatch(*this, a, true); }
#else
	bool iswm(const_pointer_const a) const noexcept;
	bool iswmcs(const_pointer_const a) const noexcept;
#endif

	// extract left/right/mid
	TString mid(const int pos, int n) const;
	TString left(int n) const;
	TString right(int n) const;

	int tsprintf(const_pointer_const fmt, ...);
	int tvprintf(const_pointer_const fmt, va_list args);

	[[nodiscard]] pointer to_chr() noexcept { m_bDirty = true;  return m_pString; };	// returns the string in the projects current format. (string can be altered)
	//gsl::strict_not_null<pointer> to_chr() noexcept { m_bDirty = true;  return gsl::make_strict_not_null(m_pString); };	// returns the string in the projects current format. (string can be altered)
	[[nodiscard]] const_pointer to_chr() const noexcept { return m_pString; };		// returns the string in the projects current format. (string can't be altered)
	[[nodiscard]] WCHAR* to_wchr() noexcept { m_bDirty = true;  return m_pString; };	// returns the string in wide format (string can be altered)
	[[nodiscard]] const WCHAR* const to_wchr() const noexcept { return m_pString; };	// returns the string in wide format (string can't be altered)
	//char * c_str(void)														// returns the string as a char * (string can be altered)
	//{
	//	MakeTemp();
	//	return m_pTempString;
	//}
	[[nodiscard]] const char* const c_str(void) const									// returns the string as a const char * (string can't be altered)
	{
		MakeTemp();
		return m_pTempString.get();
	}
	ULONG to_addr() const;

	template <TStringConcepts::IsNumeric T>
	T to_() const
	{
		static_assert(is_Numeric_v<T>, "Type T must be (int, long, float, double, ....)");

		std::basic_istringstream<value_type> ss(m_pString);	// makes copy of string :(
		T result{};
		return ss >> result ? result : T();
	}
	auto to_int() const { return to_<int>(); };
	auto to_num() const { return to_<__int64>(); };
	auto to_float() const { return to_<float>(); };
	auto to_dword() const { return to_<DWORD>(); };

	template <TStringConcepts::IsNumeric T>
	TString& append_number(T Number)
	{
		static_assert(is_Numeric_v<T>, "Type T must be (int, long, float, double, ....)");

		const auto str(std::to_wstring(Number));
		return append(str.data(), str.length());
	}

	static inline int rfc_tolower(const int c) noexcept;
	static inline int rfc_toupper(const int c) noexcept;
	//
};
#define TSTAB TString::m_cTab
#define TSSPACE TString::m_cSpace
#define TSCOMMA TString::m_cComma
#define TSSPACECHAR TString::m_cSpace[0]
#define TSTABCHAR TString::m_cTab[0]
#define TSCOMMACHAR TString::m_cComma[0]

// literal operator
// allows "sometext"_ts to be interpreted as TString("sometext")
TString operator"" _ts(const char* p, size_t N);
TString operator"" _ts(const WCHAR * p, size_t N);


//#pragma comment(lib,"tstring.lib")

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // TSTRING_H_
