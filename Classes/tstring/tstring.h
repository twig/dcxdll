/*!
 * \file tstring.h
 * \brief String and Token Management Class
 *
 * This experimental library was built to reproduce easy to use string containers managing effectively 
 * comparisons and token manipulations as done in the mIRC scripting language.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.15
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
 * © ScriptsDB.org - 2005-2017
 */

#pragma once

#ifndef _TSTRING_H_
#define _TSTRING_H_

 // VS2015+ only
#if _MSC_VER < 1900
#error "This version of TString needs Visual Studio 2015 or newer"
#endif
 
// Required for VS 2005
//#define _CRT_SECURE_NO_DEPRECATE 1
// end VS2005
// for VS2012
//#define _CRT_SECURE_NO_WARNINGS 1
// end VS2012

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>
#include <sstream>

#include "string_support.h"

#define TS_getmemsize(x) (size_t)(((x) - 1U) + (16U - (((x) - 1U) % 16U) ))
//#define TS_wgetmemsize(x) (unsigned long)(((((x) - 1)*sizeof(WCHAR)) + (16 - ((((x) - 1)*sizeof(WCHAR)) % 16)))/sizeof(WCHAR))
#define TS_wgetmemsize(x) (TS_getmemsize((x)*sizeof(WCHAR))/sizeof(WCHAR))

// enable this define if you wish to use the mIRC extra functions
#define INCLUDE_MIRC_EXTRAS 0

// enable this define to include code to support: for (auto x: TString)
//#define TSTRING_PARTS 1
//#define TSTRING_ITERATOR 1

// enable this define to enable using templates to convert TString to numbers. (TString->to_<int>() etc..)
//#define TSTRING_TEMPLATES 1

// Enable/Disable test code...
#define TSTRING_TESTCODE 1

// enable this to include a small internal buffer that avoids an allocation for small strings.
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
//#define ts_strstr(x,y) wcsstr((x),(y))
//#define ts_strchr(x,y) wcschr((x),(y))
#define ts_strstr(x,y) _ts_find((x),(y))
#define ts_strchr(x,y) _ts_find((x),(y))
#define ts_strcat_s(x,y,z) wcscat_s((x),(z),(y))
#define ts_strcat(x,y) _ts_strcat((x),(y))
#define ts_strncat(x,y,z) _ts_strncat((x),(y),static_cast<size_t>(z))
#define ts_toupper(c) ((((c) >= TEXT('a')) && ((c) <= TEXT('z'))) ? _toupper((c)) : (c) )
#define ts_tolower(c) ((((c) >= TEXT('A')) && ((c) <= TEXT('Z'))) ? _tolower((c)) : (c) )
#define ts_strcpyn(dest,src,len) _ts_strcpyn((dest),(src),static_cast<size_t>(len))
#define ts_strcpy(dest,src) _ts_strcpy((dest),(src))
//#define ts_strcpyn(dest,src,len) lstrcpyn((dest),(src),static_cast<size_t>(len))
//#define ts_strcpy(dest,src) lstrcpy((dest),(src))
#define ts_strcmp(x,y) _ts_strcmp((x),(y))
#define ts_strncmp(x,y,z) _ts_strncmp((x),(y),static_cast<size_t>(z))
#define ts_stricmp(x,y) _ts_stricmp((x),(y))
#define ts_strnicmp(x,y,z) _ts_strnicmp((x),(y),static_cast<size_t>(z))
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

/*!
 * \brief String and Token Management Class
 */
class TString {

public:
	using value_type = typename TCHAR;
	using size_type = std::size_t;
	using const_value_type = std::add_const_t<value_type>;
	using pointer = std::add_pointer_t<value_type>;
	using const_pointer = std::add_pointer_t<const_value_type>;
	using reference = std::add_lvalue_reference_t<value_type>;
	using const_reference = std::add_lvalue_reference_t<const_value_type>;

private:

	void deleteString(const bool bKeepBufferSize = false);
	void deleteTempString(const bool bKeepBufferSize = false);
	UINT i_replace(const TCHAR *const subString, const TCHAR *const rString);
	static UINT match(register const TCHAR *m, register const TCHAR *n, const bool cs /* case sensitive */);

	//const size_t set_buffersize(const size_t size) { m_buffersize = TS_getmemsize(size); return m_buffersize; };	// make buffersize a multiple of 16
	//allocate a buffer thats sized to multiples of 16 bytes, (%byte + (16 - (%byte % 16)))
	//TCHAR *allocstr_bytes(const size_t size) { return (TCHAR *)(new BYTE[set_buffersize(size)]); };
	auto allocstr_bytes(const size_type size) {
#if TSTRING_INTERNALBUFFER
		m_InternalBuffer[0] = TEXT('\0');
		if (size <= TSTRING_INTERNALBUFFERSIZE_BYTES)
		{
			m_bUsingInternal = true;
			//m_iLen = 0;
			//m_bDirty = true;
			m_buffersize = TSTRING_INTERNALBUFFERSIZE_BYTES;
			return m_InternalBuffer;
		}
		else
			m_bUsingInternal = false;
#endif
		return allocstr_bytes(size, m_buffersize);
	};
	//allocate a buffer thats sized to multiples of 16 bytes, (%byte + (16 - (%byte % 16))), iActual contains the buffer size allocated
	static pointer allocstr_bytes(const size_type size, size_type &iActual)
	{
		iActual = TS_getmemsize(size);
		return reinterpret_cast<pointer>(new BYTE[iActual]);
	};
	// allocate a buffer thats size characters long & its a multiple of 16bytes.
	auto allocstr_cch(const size_type size) { return allocstr_bytes(size*sizeof(value_type)); };
	//TCHAR *allocstr_cch(const size_t size) { return allocstr_bytes(size*sizeof(TCHAR), m_buffersize); };
	// allocate a buffer thats size characters long & its a multiple of 16bytes. NB: iActual contains the buffer size in BYTES
	static auto allocstr_cch(const size_type size, size_type &iActual) { return allocstr_bytes(size*sizeof(value_type), iActual); };

	// swap contents of second with this
	void swap(TString &second) noexcept;

	// make test string m_pTempString which is m_pString converted to either WCHAR or char
	void MakeTemp() const
	{
		if (m_pString == nullptr)
			return;

		if (m_pTempString == nullptr)
			m_pTempString = WcharTochar(m_pString);
	}

	// check if requested character is within buffer (not within string)
	template <class T>
	std::enable_if_t<std::is_signed_v<T> > CheckRange(const T &N) const
	{
		static_assert(is_Numeric_v<T>, "Only Numeric Types accepted.");

		if ((m_pString == nullptr) || (N < 0) || (static_cast<size_type>(N) >= (m_buffersize / sizeof(value_type))))
			throw std::out_of_range("TString::at()");
	}
	template <class T>
	std::enable_if_t<std::is_unsigned_v<T> > CheckRange(const T &N) const
	{
		static_assert(is_Numeric_v<T>, "Only Numeric Types accepted.");

		if ((m_pString == nullptr) || (N >= (m_buffersize / sizeof(value_type))))
			throw std::out_of_range("TString::at()");
	}

	static WCHAR *charToWchar(const char *const cString, size_t *const buffer_size = nullptr);
	static char *WcharTochar(const WCHAR *const wString, size_t *const buffer_size = nullptr);

	mutable const_pointer m_savedpos;
	mutable UINT	m_savedtotaltoks;
	mutable UINT	m_savedcurrenttok;
	//mutable std::unique_ptr<TCHAR[]> m_tSavedSepChars;
	//mutable size_t	m_savedSepCharsLength;

	size_type			m_buffersize;	// size of string buffer in use. (size in bytes)
	mutable size_type	m_iLen;			// the string length of m_pString
	mutable bool		m_bDirty;		// is buffer `dirty` (string length is unknown)?

#if TSTRING_INTERNALBUFFER
	mutable bool	m_bUsingInternal;
	mutable value_type	m_InternalBuffer[TSTRING_INTERNALBUFFERSIZE_CCH];
#endif

public:

	pointer m_pString; //!< String buffer
	// Temp string buffer used for string conversion to/from wchar/char or vice versa depending on what TCHAR is.
	// changes made to m_pTempString are NOT reflected in m_pString!
	mutable char * m_pTempString;
	static const_pointer m_cSpace;
	static const_pointer m_cComma;
	static const_pointer m_cTab;

	TString()
		: TString(0U)
	{
	}
	
	template <typename T, size_t N>
	TString(const T (&cString)[N])
		: TString(cString, N-1)		// NB: N here also counts the zero char at the end
	{
		static_assert(std::is_same_v<T, WCHAR> || std::is_same_v<T, char>, "MUST be a WCHAR or char string");
	}

	template <typename T, typename = std::enable_if_t<std::is_pointer_v<T>> >
	TString(const T cString)
		: TString(cString, _ts_strlen(cString))
	{
		static_assert(std::is_same_v<std::remove_cv_t<std::remove_pointer_t<T>>, WCHAR> || std::is_same_v<std::remove_cv_t<std::remove_pointer_t<T>>, char>, "MUST be a WCHAR or char string");
	}

	TString(const TString & tString)
		: TString(tString.data(), tString.len())
	{
	}
	
	TString(TString &&tString);					// move constructor C++11 only
	
	TString(const std::initializer_list<TString> &lt);	// Initializer list constructor (allows TString name{ "text", "text2", othertstring } )

#if TSTRING_TESTCODE
	using pair_type = std::pair<pointer, pointer>;
	using tuple_type = std::tuple<const_pointer, const_pointer, size_type>;

	TString(const pair_type &rng)
		: TString(rng.first, rng.second)
	{}

	TString(const tuple_type &rng)
		: TString(std::get<0>(rng), std::get<1>(rng))
	{}
#endif

	// template version of the unique ptr constructor, handles WCHAR * & CHAR *
	// maybe add TString * support....
	template <typename T>
	TString(const std::unique_ptr<T> &unique)
		: TString(unique.get())
	{
		//static_assert(std::is_same_v<std::remove_cv_t<T>, WCHAR> || std::is_same_v<std::remove_cv_t<T>, char>, "MUST be a WCHAR or char string");
	}

	//	Constructor
	//		cString	== string to add (wchar_t *)
	//		iLen	== Length of string in characters.
	TString(const WCHAR *const cString, const size_type iLen)
		: TString(iLen+1U)
	{
		if (cString != nullptr) {
			if (cString[0] != TEXT('\0')) {
				ts_strcpyn_throw(m_pString, cString, iLen + 1);
				m_iLen = iLen;
			}
		}
		m_bDirty = false;
	}

	//	Constructor
	//		cString	== string to add (char *)
	//		iLen	== Length of string in characters.
	TString(const char *const cString, const size_type iLen)
		: m_pTempString(nullptr), m_pString(nullptr)
		, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(nullptr)
		, m_bDirty(false), m_iLen(0)
#if TSTRING_INTERNALBUFFER
		, m_bUsingInternal(false)
#endif
	{
		if (cString != nullptr) {
			if (cString[0] != 0) {
				m_pString = charToWchar(cString, &m_buffersize);
				//m_iLen = ts_strlen(m_pString);
				m_iLen = iLen;
			}
		}
		if (m_pString == nullptr) {
			m_pString = allocstr_cch(1);
			m_pString[0] = TEXT('\0');
		}
	}

	//	Constructor
	//		pStart	== start of string to add (wchar_t * or char *)
	//		pEnd	== end of string to add.
	template <typename T>
	TString(const T *const pStart, const T *const pEnd)
		: TString(pStart, static_cast<size_type>(pEnd - pStart))
	{
		static_assert(std::is_same_v<T,WCHAR> || std::is_same_v<T,char>, "MUST be a WCHAR or char string");

		if (pEnd < pStart)
			throw std::invalid_argument("TString(): End of string < start");
	}

	//	Constructor
	//		chr	== character to add (wchar_t)
	explicit TString(const WCHAR chr);
	//	Constructor
	//		chr	== character to add (char)
	explicit TString(const char chr);
	//	Constructor
	//		tsSize	== Length of buffer to allocate in characters.
	explicit TString(const UINT tsSize);

	//! Destructor
	~TString( );

	// Operator Overloads

	//	Move Assignment Operator
	TString & operator =(TString &&tString) noexcept;

	// single + operator that handles all supported types (type checking handled by += operator)
	template <class T>
	TString operator +(const T &other) {
		auto newTString(*this);
		newTString += other;
		return newTString;
	}

	TString & operator =(const TString &tString);
	TString & operator =(const WCHAR *const cString);
	TString & operator =(const WCHAR chr);
	TString & operator =(const char *const cString);
	TString & operator =(const char chr);

	// code below is broken & causes crashes, reason unknown at this time
	//template <typename T>
	//TString &operator =(const T &other) {
	//	static_assert(!is_Numeric_v<T>, "Must be a TString, WCHAR, CHAR, WCHAR * or CHAR *");
	//	TString tmp(other);
	//	this->swap(tmp);
	//	return *this;
	//	//this->copy(other);
	//	//return *this;
	//}
	//template <>
	//TString &operator =(const WCHAR &other) {
	//	TString tmp(other);
	//	this->swap(tmp);
	//	return *this;
	//}
	//template <>
	//TString &operator =(const char &other) {
	//	TString tmp(other);
	//	this->swap(tmp);
	//	return *this;
	//}

	// final version of the += operator, properly handles numeric types (char, wchar classified as non-numeric here)
	template <class T>
	std::enable_if_t<!is_Numeric_v<T>, TString> & operator +=(const T &other) { return append(other); }
	template <class T>
	std::enable_if_t<is_Numeric_v<T>, TString> & operator +=(const T &num) { return append_number(num); }

	// code below is the same as above but without the is_Numeric() macro (kept as reference only)
	//template <class T>
	//std::enable_if_t<!std::is_arithmetic<T>::value || std::is_same<T, WCHAR>::value || std::is_same<T, CHAR>::value, TString> & operator +=(const T &other) { return append(other); }
	//template <class T>
	//std::enable_if_t<std::is_arithmetic<T>::value && !std::is_same<T, WCHAR>::value && !std::is_same<T, CHAR>::value, TString> & operator +=(const T &num) { return append_number(num); }

	// template below works great, but has limited type support for numbers (kept as reference only)
	//template <class T>
	//TString & operator +=(const T &other) { return append(other); }
	//template <>
	//TString & operator +=(const int &num) { return append_number(num); }
	//template <>
	//TString & operator +=(const float &num) { return append_number(num); }
	//template <>
	//TString & operator +=(const double &num) { return append_number(num); }
	//template <>
	//TString & operator +=(const unsigned long &num) { return append_number(num); }
	//template <>
	//TString & operator +=(const __int64 &num) { return append_number(num); }

	template <class T>
	bool operator <=( const T &other ) const noexcept { return !(*this > other); }

	template <class T>
	bool operator >=(const T &other) const noexcept { return !(*this < other); }

	template <class T>
	bool operator ==(const T &other) const noexcept { return (compare(other) == 0); }
	template <>
	bool operator ==(const int &iNull) const noexcept { return (m_pString == nullptr && !iNull); }

	template <class T>
	bool operator !=(const T &other) const noexcept { return !(*this == other); }

	template <class T>
	bool operator <(const T &other) const noexcept { return (compare(other) < 0); }

	template <class T>
	bool operator >(const T &other) const noexcept { return (compare(other) > 0); }

	template <class T>
	TString operator -( const T &other ) {

		if ( _ts_isEmpty(other) )
			return *this;

		auto newTString(*this);
		newTString -= other;
		return newTString;
	}

	template <class T>
	TString & operator -=(const T &other)
	{
		return remove(other);
	}

	TString operator *( const int &N ) const;
	TString & operator *=( const int &N );

	TString operator ++(int N) const {
		// NB: only works with space token
		// NB: post-increment works for us as we want to return a copy (the token)
		// NB: pre-increment doesn't work for this
		if (m_savedpos == nullptr) return getfirsttok(1);
		return getnexttok();
	}
	
	// returns the current token as set by ++ operator above (or getfirsttok()/getnexttok() with a space sepchar)
	const TString operator* () const {
		const_pointer p_cStart = m_pString;
		if (m_savedpos != nullptr)
			p_cStart = m_savedpos;
		auto p_cEnd = ts_strchr(p_cStart, SPACECHAR);
		if (p_cEnd == nullptr)
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
	//	const TCHAR * p_cStart = m_pString;
	//	if (m_savedpos != nullptr)
	//		p_cStart = m_savedpos;
	//	auto p_cEnd = ts_strchr(p_cStart, SPACECHAR);
	//	if (p_cEnd == nullptr)
	//		return TString(p_cStart);
	//
	//	return TString(p_cStart, p_cEnd);
	//}

#ifndef NDEBUG
	template <class T>
	reference operator [](const T &N)
	{
		CheckRange(N);	// only debug code checks range here

		m_bDirty = true;

		return m_pString[N];
	}

	template <class T>
	value_type operator [](const T &N) const
	{
		CheckRange(N);	// only debug code checks range here
		return m_pString[N];
	}
#else
	template <class T>
	TCHAR & operator [](const T &N) noexcept
	{

		m_bDirty = true;

		return m_pString[N];
	}

	template <class T>
	TCHAR operator [](const T &N) const noexcept
	{
		return m_pString[N];
	}
#endif

	template <class T>
	friend TString &operator <<(TString &other, const T &N) { other += N; return other; }
	template <class T>
	friend TString &operator >>(const TString &other, T &N) { N = other.to_<T>(); return other; }
	template <class T>
	friend TString operator +(const TString & tString, const T &other)
	{
		auto newTString(tString);
		newTString += other;
		return newTString;
	}

	//conversion operators.
	template <class T, typename = std::enable_if_t<is_Numeric_v<T>> >
	operator T() const {
		static_assert(is_Numeric_v<T>, "Type T must be (int, long, float, double, ....)");
		return to_<T>();
	}
	explicit operator bool() const noexcept { return !empty(); }
	//explicit operator WCHAR *() const noexcept { return m_pString; }

	// General String Lib

	// the actual string data
	const TCHAR *const data() const noexcept { return m_pString; }
	// get length of string in characters
	const size_t &len( ) const noexcept;
	// alias for len()
	const size_t &length() const noexcept { return len(); };
	// alias for len()
	const size_t &size() const noexcept { return len(); };
	// capacity of buffer
	const size_t &capacity() const noexcept { return m_buffersize; }
	// pointer to the end of the buffer
	const_pointer last() const noexcept { return m_pString + len(); }
	// clear string buffer & reset all vars & pointers (frees buffer)
	void clear();
	// shrink string buffer to min size required for string (while still being a multiple of 16)
	void shrink_to_fit();
	// append a single wide char
	TString &append(const WCHAR &chr);
	// append a single char
	TString &append(const char &chr);
	// append a wide char string.
	TString &append(const WCHAR *const cString);
	// append a char string.
	TString &append(const char *const cString);
	// append another TString object
	TString &append(const TString &tString);
	// append a string thats limited to iChars characters.
	TString &append(const TCHAR *const cString, const size_t iChars);
	// is string empty?
	const bool empty() const noexcept { return (m_pString == nullptr || m_pString[0] == TEXT('\0')); };
	// refrence to char at N
	reference at(const size_type N)
	{
		CheckRange(N);
		return m_pString[N];
	}
	// copy of char at N
	value_type at(const size_type N) const
	{
		CheckRange(N);
		return m_pString[N];
	}
	// allocate memory for string, preserves contents...
	void reserve(const size_t tsSize);
	// copy string...
	void copy(TString other);
	// compare strings...
	int compare(const TString &other) const noexcept;
	int compare(const TCHAR &other) const noexcept;
	int compare(const TCHAR *const other) const noexcept;
	int compare(const TCHAR *const other, const size_t iLength) const noexcept;

	// compare 'this' to an array, array type can be anything supported by the == operator.
	// returns the index of the matching item, or zero for failure, Index is One based.
	template <typename T, UINT iArraySize>
	UINT acompare(const T (&array)[iArraySize]) const
	{
		for (auto i = decltype(iArraySize){0}; i < iArraySize; ++i)
		{
			if (*this == array[i])
				return i + 1;
		}
		return 0U;
	}

#if TSTRING_TESTCODE
	/*! \fn int find( const T &substring, int N )
	\brief Function to find position or number of occurences of a substring in the string

	\param substring Substring to search
	\param N Nth substring to search (N = \b 0 > Total number of matches)

	\return > Number of occurrences (N = 0)\n
	> Starting position of \b substring \n
	> \b -1 if function fails or no substring was found
	*/
	template <typename T>
	int find(const T &substring, const int N) const noexcept
	{
		if (!_ts_isEmpty(substring) && !_ts_isEmpty(m_pString)) {

			const_pointer temp = nullptr, temp2 = m_pString;

			auto i = decltype(N){0};
			const auto subl = _ts_strlen(substring);
			while ((temp = _ts_find(temp2, substring)) != nullptr) {
				++i;
				//if ( N != 0 && i == N )
				if (i == N) // i is never zero
					return static_cast<int>(temp - m_pString);
				temp2 = (temp + subl);
			}
			if (N == 0)
				return i;
		}
		return -1;
	}

#else
	int find(const TCHAR *const substring, const int N) const noexcept;	// find Nth matching subString
	int find( const TCHAR chr, const int N ) const noexcept;			// find Nth matching chr
#endif

	TString sub( int N, int M ) const;

	TString &trim();	// removes spaces at start & end of text.
	TString &strip();	// removes spaces at start & end of text & all ctrl codes in text.

#if TSTRING_TESTCODE
	//	Replace
	//		subString	-	String to be replaced (can be char/wchar_t/char */wchar_t */TString/std:string/std::wstring)
	//		rString		-	String to replace subString with (can be any of the types supported but doesnt have to be the same type as subString)
	//						rString can also be zero, if zero then this function does a remove.
	template <typename T, typename M>
	UINT replace(const T &subString, const M &rString)
	{
		if (empty())
			return 0U;

		auto subl = _ts_strlen(subString);

		if (subl == 0)
			return 0U;

		auto repl = _ts_strlen(rString);

		auto c = 0U;
		TString tmp;
		auto pStart = m_pString, pfEnd = m_pString + length();
		auto pEnd = decltype(pStart){0};

		while ((pEnd = _ts_find(pStart, subString)) != nullptr)
		{
			if (pStart != pEnd)
				//tmp += TString(pStart, pEnd);
				tmp.append(pStart, static_cast<size_t>(pEnd - pStart));

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
	UINT mreplace(const TCHAR chr, const TCHAR *const fmt);					// replace any char in fmt with chr

	template <typename T>
	TString &remove(const T &str)
	{
		const_value_type sTmp[2] = { 0 };
		replace(str, &sTmp[0]);
		return *this;
	}

	//TString &setzero(const TCHAR *const subString)
	//{
	//	mreplace(TEXT('\0'), subString);
	//	return *this;
	//}

#else
	// remove sub string from string
	TString &remove(const TCHAR *const subString);
	// remove character from string
	TString &remove(const TCHAR &chr);
	// remove sub string from string
	TString &remove(const TString &subString);
	UINT replace(const TCHAR *const subString, const TCHAR *const rString);	// replace subString with rString
	UINT replace(const TCHAR *const subString, const TCHAR rchr);			// replace subString with rchr
	UINT replace(const TCHAR chr, const TCHAR *const rString);				// replace chr with rString
	UINT replace(const TCHAR chr, const TCHAR rchr );						// replace chr with rchr
	UINT mreplace(const TCHAR chr, const TCHAR *const fmt);					// replace any char in fmt with chr
#endif

	// Token Lib
#if !TSTRING_TESTCODE
	template <typename T>
	void addtok(const T &tToken)
	{
		addtok(tToken, SPACECHAR);
	}

	template <typename T, typename TS>
	void addtok(const T &tToken, const TS &sepChars)
	{
		if (!tToken || !sepChars)
			return;

		if (!empty())
			*this += sepChars;

		*this += tToken;
	}

	template <class T, class R = size_t>
	R findtok(const T &cToken, const UINT N, const TCHAR *const sepChars = SPACE) const
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
		return std::find_if(begin(sepChars), itEnd, [&count, &N, &cToken](const auto &x) {
			if (x == cToken) {
				if (++count == N)
					return true;
			}
			return false;
		});
	}

	template <class T>
	void remtok(const T &cToken, const UINT N, const TCHAR *const sepChars = SPACE) {
		//const auto tokennr = findtok(cToken, N, sepChars);
		//if (tokennr > 0)
		//	deltok(tokennr, sepChars);

		deltok(findtok(cToken, N, sepChars), sepChars);
	}

	template <class T>
	auto istok(const T &cToken, const TCHAR *const sepChars = SPACE) const { return findtok<T,bool>(cToken, 1, sepChars); }

	template <typename T>
	void instok(const T &cToken, const UINT N, const TCHAR *const sepChars = SPACE)
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
			tsTmp = gettok(1, N -1, sepChars);
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
	void puttok(const T &cToken, const UINT N, const TCHAR *const sepChars = SPACE)
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
	void reptok(const T &cToken, const M &newToken, const UINT N, const TCHAR * const sepChars = SPACE)
	{
		const auto pos = findtok(cToken, N, sepChars);
		if (pos > 0)
			puttok(newToken, pos, sepChars);
	}

	void deltok(const UINT N, const TCHAR *const sepChars = SPACE);

	TString gettok(int N, const TCHAR *const sepChars = SPACE) const;
	TString gettok(int N, int M, const TCHAR *const sepChars = SPACE) const;
	TString getfirsttok(const UINT N, const TCHAR *const sepChars) const;					// must be called before the first getnexttok()
	TString getfirsttok(const size_type N, const value_type &sepChar = SPACECHAR) const;	// must be called before the first getnexttok()
	TString getnexttok(const TCHAR *const sepChars) const;									// gets subsequent tokens after a getfirsttok() call.
	TString getnexttok(const_value_type &sepChars = SPACECHAR) const;						// gets subsequent tokens after a getfirsttok() call.

	TString getlasttoks() const;															// gets all remaining tokens after a getfirsttok()/getnexttok() call.
	const bool moretokens() const noexcept { return (m_savedpos != nullptr); }
	void resettokens() const noexcept {
		m_savedcurrenttok = 0;
		m_savedpos = nullptr;
		m_savedtotaltoks = 0;
	}
	TString matchtok(const TCHAR *const mString, UINT N, const TCHAR *const sepChars = SPACE) const;
	size_t numtok(const TCHAR *const sepChars) const noexcept;
	size_t numtok(const TCHAR &sepChar = SPACECHAR) const noexcept;
#else
	template <typename T, typename TSepChars = const_reference>
	void addtok(const T &tToken, TSepChars sepChars = SPACECHAR)
	{
		// allows adding zero number & empty tokens.
		if (_ts_isEmpty(sepChars))
			return;

		// cant add a zero number or an empty token with this
		//if (!tToken || _ts_isEmpty(sepChars))
		//	return;

		// Ook: this code has an issue when a number is being added
		//if (_ts_isEmpty(tToken) || _ts_isEmpty(sepChars))
		//	return;

		if (!empty())
			*this += sepChars;

		*this += tToken;
	}

	template <class T, class R = size_t, typename TSepChars = const_reference>
	R findtok(const T &cToken, const UINT N, TSepChars sepChars = SPACECHAR) const
	{
		auto itEnd = end();

		if (_ts_isEmpty(sepChars) || empty() || !cToken)
			return itEnd;

		//code below has issues due to way iterator works & gets converted to numbers on return (fixed)
		auto count = decltype(N){0};
		return std::find_if(begin(sepChars), itEnd, [&count, &N, &cToken](const auto &x) {
			if (x == cToken) {
				if (++count == N)
					return true;
			}
			return false;
		});
	}

	template <class T, typename TSepChars = const_reference>
	void remtok(const T &cToken, const UINT N, TSepChars sepChars = SPACECHAR) {
		deltok(findtok(cToken, N, sepChars), sepChars);
	}

	template <class T, typename TSepChars = const_reference>
	auto istok(const T &cToken, TSepChars sepChars = SPACECHAR) const { return findtok<T, bool>(cToken, 1, sepChars); }

	template <typename T, typename TSepChars = const_reference>
	void instok(const T &cToken, const UINT N, TSepChars sepChars = SPACECHAR)
	{
		if (_ts_isEmpty(sepChars))
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
			tsTmp = gettok(1, static_cast<int>(N) - 1, sepChars);
			// then new token
			tsTmp += sepChars;
			tsTmp += cToken;
			// and any tokens after the inserted one.
			if (N < nToks) {
				tsTmp += sepChars;
				tsTmp += gettok(static_cast<int>(N), -1, sepChars);
			}
		}
		swap(tsTmp);
	}

	template <typename T, typename TSepChars = const_reference>
	void puttok(const T &cToken, const UINT N, TSepChars sepChars = SPACECHAR)
	{
		const auto nToks = numtok(sepChars);
		if (N == 1)
		{
			// replace first token
			TString tmp(cToken);
			if (nToks > N)
			{
				tmp += sepChars;
				tmp += gettok(static_cast<int>(N) + 1, -1, sepChars);
			}
			swap(tmp);
		}
		else if (nToks > N)
		{
			// replace middle token
			TString tmp(gettok(1, static_cast<int>(N) - 1, sepChars));
			tmp.addtok(cToken, sepChars);
			tmp.addtok(gettok(static_cast<int>(N) + 1, -1, sepChars), sepChars);

			swap(tmp);
		}
		else if (nToks < N) {
			// add token to end (nothing to replace)
			addtok(cToken, sepChars);
		}
		else {
			// replace last token
			TString tmp(gettok(1, static_cast<int>(N) - 1, sepChars));
			tmp.addtok(cToken, sepChars);

			swap(tmp);
		}
	}

	template <typename T, typename M, typename TSepChars = const_reference>
	void reptok(const T &cToken, const M &newToken, const UINT N, TSepChars sepChars = SPACECHAR)
	{
		const auto pos = findtok(cToken, N, sepChars);
		if (pos > 0)
			puttok(newToken, pos, sepChars);
	}

	/*!
	* \brief blah
	*
	* blah
	*/

	template <typename TSepChars = const_reference>
	void deltok(const UINT N, TSepChars sepChars = SPACECHAR)
	{
		if (_ts_isEmpty(sepChars) || N < 1 || empty())
			return;

		const auto nToks = numtok(sepChars);

		if (N > nToks)
			return;

		if (N == 1 && nToks == 1) {
			clear();
			//deleteString();
			return;
		}

		pointer p_cStart = m_pString, p_cEnd = nullptr;
		auto i = decltype(N){0};
		const auto sepl = _ts_strlen(sepChars);

		while ((p_cEnd = _ts_find(p_cStart, sepChars)) != nullptr) {
			++i;

			if (i == N)
				break;

			p_cStart = p_cEnd + sepl;
		}

		const auto l = len();

		TString tmp(l);

		// last token
		if (p_cEnd == nullptr) {
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
	void resettokens() const noexcept {
		m_savedcurrenttok = 0;
		m_savedpos = nullptr;
		m_savedtotaltoks = 0;
	}

	/*!
	* \brief blah
	*
	* blah
	*/
	template <typename TSepChars = const_reference>
	TString matchtok(const TCHAR * const mString, UINT N, TSepChars sepChars = SPACECHAR) const
	{
		auto count = decltype(N){0};
		auto itEnd = end();
		auto itGot = std::find_if(begin(sepChars), itEnd, [&count, &N, &mString](const auto &x) {
			if (_ts_find(x.to_chr(), mString) != nullptr) {
				++count;
				if (count >= N)
					return true;
			}
			return false;
		});
		if (itGot != itEnd)
			return *itGot;
		return TString();
	}

	/*!
	* \brief Returns the number of tokens in the string
	*
	* params
	*	sepChars = The token seperator.
	*/
	template <typename T = const_reference>
	size_t numtok(T sepChars = SPACECHAR) const noexcept
	{
		if (_ts_isEmpty(sepChars) || empty())
			return 0U;

		const_pointer p_cStart = m_pString, p_cEnd = nullptr;
		auto iCount = 0U;
		const auto sepl = _ts_strlen(sepChars);

		while ((p_cEnd = _ts_find(p_cStart, sepChars)) != nullptr) {
			++iCount;
			p_cStart = p_cEnd + sepl;
		}
		return iCount + 1U;
	}

	//const tuple_type gettokenrange(const int nStart, const int nEnd, const_pointer sepChars) const;
	//const tuple_type gettokenrange(const int nStart, const int nEnd, const_reference sepChars = SPACECHAR) const;

	template <typename TSepChars = const_reference>
	const TString::tuple_type gettokenrange(const int nStart, const int nEnd, TSepChars sepChars = SPACECHAR) const
	{
		const_pointer p_cStart = m_pString, p_cEnd = nullptr, p_fEnd = last();

		if (_ts_isEmpty(sepChars))
			return std::make_tuple(p_cStart, p_fEnd, decltype(m_savedtotaltoks){1});

		const auto nToks = numtok(sepChars);
		auto iStart = nStart;

		if (iStart < 0)
			iStart += (nToks + 1);

		if (p_cStart == nullptr || p_fEnd == nullptr || iStart < 1 || ((nEnd < iStart) && (nEnd != -1)) || static_cast<size_t>(iStart) > nToks)
			return std::make_tuple(nullptr, nullptr, decltype(m_savedtotaltoks){0});

		const auto bFullstring = ((static_cast<size_t>(nEnd) >= nToks) || (nEnd < 0));

		const_pointer p_cFirst = nullptr, p_cLast = nullptr;
		auto iCount = 0;
		const auto sepl = _ts_strlen(sepChars);

		while ((p_cEnd = _ts_find(p_cStart, sepChars)) != nullptr) {
			++iCount;

			if (iCount == iStart) {

				p_cFirst = p_cStart;

				if (bFullstring)
					break;
			}

			if (iCount == nEnd) {
				p_cLast = p_cEnd;
				break;
			}

			p_cStart = p_cEnd + sepl;
			if (p_cStart >= p_fEnd)	// look out for overrun...
				break;
		}

		if (bFullstring) {

			if (static_cast<size_t>(iCount) == (nToks - 1))
				p_cFirst = p_cStart;

			p_cLast = p_fEnd;
		}
		else if (static_cast<size_t>(iCount) == (nToks - 1))
			p_cLast = p_cEnd;

		return std::make_tuple(p_cFirst, p_cLast, nToks);
	}

	template <typename T = const_reference>
	inline TString gettok(const int N, const int M, T sepChars = SPACECHAR) const
	{
		return gettokenrange(N, M, sepChars);
	}

	template <typename T = const_reference>
	inline TString gettok(const int N, T sepChars = SPACECHAR) const
	{
		return gettokenrange(N, N, sepChars);
	}

	template <typename T = const_reference>
	inline TString getfirsttok(const int N, T sepChars = SPACECHAR) const
	{
		//std::tie(std::ignore, m_savedpos, m_savedtotaltoks) = gettokenrange(N, N, sepChars);

		auto rng = gettokenrange(N, N, sepChars);

		m_savedcurrenttok = static_cast<UINT>(N);
		m_savedtotaltoks = std::get<2>(rng);
		m_savedpos = std::get<1>(rng);

		if (m_savedpos != nullptr)
			++m_savedpos;

		return rng;
	}

	/*!
	* \brief Get the next token as a specified integral type (can only be used after a getfirsttok() call)
	*
	* params
	* sepChars	-	The token seperator string/character
	*				If NULL returns zero.
	*/
	template <typename T, typename TSepChar = const_reference>
	T getnexttokas(TSepChar sepChars = SPACECHAR) const
	{
		//return getnexttok(sepChars).to_<T>();

		++m_savedcurrenttok;

		if (_ts_isEmpty(sepChars) || m_pString == nullptr || m_savedpos == nullptr || m_savedcurrenttok > m_savedtotaltoks)
			return T();

		const auto *const p_cStart = m_savedpos;

		if (m_savedcurrenttok == m_savedtotaltoks) {
			m_savedpos = nullptr;
			//return TString(p_cStart).to_<T>();
			return Dcx::parse_string<T, value_type>(p_cStart);
		}
		else {
			const auto *const p_cEnd = _ts_find(p_cStart, sepChars);
			if (p_cEnd != nullptr) {
				m_savedpos = (p_cEnd + _ts_strlen(sepChars));
				return TString(p_cStart, p_cEnd).to_<T>();
			}
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
	template <typename TSepChar = const_reference>
	TString getnexttok(TSepChar sepChars = SPACECHAR) const
	{
		++m_savedcurrenttok;

		if (_ts_isEmpty(sepChars) || m_pString == nullptr)
			return *this;

		if (m_savedpos == nullptr || m_savedcurrenttok > m_savedtotaltoks)
			return TString();

		const auto *const p_cStart = m_savedpos;

		if (m_savedcurrenttok == m_savedtotaltoks) {
			m_savedpos = nullptr;
			return TString(p_cStart);
		}
		else {
			const auto *const p_cEnd = _ts_find(p_cStart, sepChars);
			if (p_cEnd != nullptr) {
				m_savedpos = (p_cEnd + _ts_strlen(sepChars));
				return TString(p_cStart, p_cEnd);
			}
		}
		return TString();
	}

#endif

	struct SortOptions {
		bool bAlpha;
		bool bNumeric;
		bool bPrefix;
		bool bReverse;

		SortOptions()
			: bAlpha(false), bNumeric(false), bPrefix(false), bReverse(false) {}
	};

	void sorttok(const TCHAR *const sortOptions, const TCHAR *const sepChars = SPACE);		// The default is an alphabetic sort, however you can specify n = numeric sort, c = channel nick prefix sort, r = reverse sort, a = alphanumeric sort.
	void sorttok(const SortOptions &sortOptions, const TCHAR *const sepChars = SPACE);		// The default is an alphabetic sort, however you can specify n = numeric sort, c = channel nick prefix sort, r = reverse sort, a = alphanumeric sort.

	TString wildtok(const TCHAR *const wildString, const UINT N, const TCHAR *const sepChars = SPACE) const;
	UINT nwildtok(const TCHAR *const wildString, const TCHAR *const sepChars = SPACE) const;

	template <typename tsType, typename T = const TCHAR>
	class tsIterator
		: public std::iterator<std::forward_iterator_tag, tsType, ptrdiff_t, tsType*, tsType&>
	{
	public:
		tsIterator(const tsIterator<tsType, T> &other) = default;
		tsIterator(tsType *ptr = nullptr) : tsIterator(ptr, nullptr) { if (m_ptr == nullptr) m_iIndex = 0; }
		tsIterator(tsType *ptr, T *const sepChars)
			: m_ptr(ptr), m_iIndex(1), m_sepChars(sepChars), m_sepChar(), m_toks(0)/*, m_iPrevIndex(1)*/
			, m_savedStart(nullptr), m_savedEnd(nullptr), m_savedFinal(nullptr)
		{
			if (m_sepChars == nullptr)
			{
				//m_sepChars = SPACE;
				//m_sepCharsLen = 1;

				m_sepChar[0] = SPACECHAR;
				m_sepChar[1] = T();
				m_sepChars = &m_sepChar[0];
				m_sepCharsLen = 1;
			}
			else
				m_sepCharsLen = _ts_strlen(m_sepChars);

			if (m_ptr != nullptr)
			{
				m_toks = m_ptr->numtok(m_sepChars);
				m_savedStart = m_ptr->m_pString;
				if (m_savedStart != nullptr)
				{
					m_savedEnd = ts_strstr(m_savedStart, m_sepChars);
					//m_savedFinal = (TCHAR *)(m_savedStart + m_ptr->len());
					m_savedFinal = m_ptr->last();
				}
			}
		}
		tsIterator(tsType *ptr, T &sepChar)
			: m_ptr(ptr), m_iIndex(1), m_sepChars(&m_sepChar[0]), m_toks(0)/*, m_iPrevIndex(1)*/
			, m_savedStart(nullptr), m_savedEnd(nullptr), m_savedFinal(nullptr), m_sepChar{ sepChar, T() }
		{
			if (sepChar == T())
				m_sepChar[0] = SPACECHAR;
			//else
			//	m_sepChar[0] = sepChar;

			//m_sepChar[1] = T();
			m_sepCharsLen = 1;

			if (m_ptr != nullptr)
			{
				m_toks = m_ptr->numtok(m_sepChars);
				m_savedStart = m_ptr->m_pString;
				if (m_savedStart != nullptr)
				{
					m_savedEnd = ts_strstr(m_savedStart, m_sepChars);
					//m_savedFinal = (TCHAR *)(m_savedStart + m_ptr->len());
					m_savedFinal = m_ptr->last();
				}
			}
		}
		~tsIterator(){}

		tsIterator<tsType, T> &operator = (const tsIterator<tsType, T> &other) = default;
		tsIterator<tsType, T> &operator = (tsType *ptr) {
			m_ptr = ptr;
			m_sepChars = SPACE;
			m_sepCharsLen = 1;
			m_iIndex = 0;
			m_toks = 0;
			m_savedStart = nullptr;
			m_savedEnd = nullptr;
			m_savedFinal = nullptr;

			if (m_ptr != nullptr)
			{
				m_iIndex = 1;
				m_toks = m_ptr->numtok(m_sepChars);
				m_savedStart = m_ptr->m_pString;
				if (m_savedStart != nullptr)
				{
					m_savedEnd = ts_strstr(m_savedStart, m_sepChars);
					//m_savedFinal = (TCHAR *)(m_savedStart + m_ptr->len());
					m_savedFinal = m_ptr->last();
				}
			}
			return *this;
		}

		operator bool() const noexcept { return (m_ptr != nullptr); }
		operator size_t() const noexcept { return m_iIndex; }

		bool operator == (const tsIterator<tsType, T> &other) const noexcept { return (m_ptr == other.getConstPtr()); }
		bool operator != (const tsIterator<tsType, T> &other) const noexcept { return (m_ptr != other.getConstPtr()); }

		const tsIterator<tsType, T> &operator++ ()
		{
			++m_iIndex;

			m_savedStart = (TCHAR *)(m_savedEnd + m_sepCharsLen);

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
			if (m_ptr == nullptr)
				throw std::out_of_range("TString::iterator");
			//if (m_ptr == nullptr)
			//	return TEXT("");

			//return m_ptr->gettok((int)m_iIndex, m_sepChars);

			//const size_t iTmp = m_iPrevIndex + 1;
			//m_iPrevIndex = m_iIndex;
			//if (m_iIndex != iTmp)
			//	return m_ptr->getfirsttok(m_iIndex, m_sepChars);
			//return m_ptr->getnexttok(m_sepChars);

			if (m_sepChars == nullptr || m_savedStart == nullptr)
				return tsType();

			if ((m_iIndex == m_toks) || (m_savedEnd == nullptr))
				return tsType( m_savedStart );

			return tsType(m_savedStart, m_savedEnd);
		}

		tsType *getPtr() const noexcept { return m_ptr; }
		const tsType *getConstPtr() const noexcept { return m_ptr; }
		const size_t &getIndex() const noexcept { return m_iIndex; }

	//protected:
	private:
		tsType *m_ptr;
		size_t	m_iIndex;
		size_t	m_sepCharsLen;
		size_t	m_toks;
		T		*m_sepChars;
		mutable TCHAR *m_savedStart;
		mutable TCHAR *m_savedEnd;
		const TCHAR *m_savedFinal;
		std::remove_cv_t<T>		m_sepChar[2];
	};

	using iterator = tsIterator<TString, const_value_type>;
	using const_iterator = tsIterator<const TString, const_value_type>;

	inline iterator begin() { return iterator(this); }
	inline iterator begin(const TCHAR *const sepChars) { return iterator(this, sepChars); }
	inline iterator begin(const_reference sepChar) { return iterator(this, sepChar); }
	inline iterator end() { return iterator(); }
	inline const const_iterator begin() const { return const_iterator(this); };
	inline const const_iterator begin(const TCHAR *const sepChars) const { return const_iterator(this, sepChars); }
	inline const const_iterator begin(const_reference sepChar) const { return const_iterator(this, sepChar); }
	inline const const_iterator end() const { return const_iterator(); }

	// add the contents of any container class to the TString using the specified seperator (can also add other TString's like this)
	template <class T, class TS = const_reference> auto &join(const T &Cont, const TS sepChars = SPACECHAR)
	{
		for (const auto &x : Cont)
		{
			addtok(x, sepChars);
		}
		return *this;
	}

	// fill a container class with a split up TString
	template <class T, class TS = const_reference> void expand(T &Cont, const TS sepChars = SPACECHAR)
	{
		for (auto itStart = begin(sepChars), itEnd = end(); itStart != itEnd; ++itStart)
		{
			Cont.push_back((*itStart));
			//Cont.emplace_back((*itStart));
		}
	}

#if INCLUDE_MIRC_EXTRAS
	// extras for mIRC
	bool isnum(const int f) const noexcept;
	bool isincs(const TCHAR let) const noexcept;
	UINT countchar(const TCHAR chr) const noexcept;
	bool ishostmask(void) const noexcept;

	TString toupper(void) const;
	TString tolower(void) const;

#endif

#if TSTRING_TESTCODE
	template <typename T> bool iswm(const T &a) const noexcept { return _ts_WildcardMatch(*this, a); }
	template <typename T> bool iswmcs(const T &a) const noexcept { return _ts_WildcardMatch(*this, a); }
#else
	bool iswm(const TCHAR *const a) const noexcept;
	bool iswmcs(const TCHAR *const a) const noexcept;
#endif

	// extract left/right/mid
	TString mid(const int pos, int n) const;
	TString left(int n) const;
	TString right(int n) const;

	int tsprintf(const TCHAR *const fmt, ...);
	int tvprintf(const TCHAR *const fmt, va_list args);

	pointer to_chr() noexcept { m_bDirty = true;  return m_pString; };	// returns the string in the projects current format. (string can be altered)
	const_pointer to_chr() const noexcept { return m_pString; };		// returns the string in the projects current format. (string can't be altered)
	WCHAR *to_wchr() noexcept { m_bDirty = true;  return m_pString; };	// returns the string in wide format (string can be altered)
	const WCHAR *const to_wchr() const noexcept { return m_pString; };	// returns the string in wide format (string can't be altered)
	//char * c_str(void)														// returns the string as a char * (string can be altered)
	//{
	//	MakeTemp();
	//	return m_pTempString;
	//}
	const char *const c_str(void) const											// returns the string as a const char * (string can't be altered)
	{
		MakeTemp();
		return m_pTempString;
	}
	ULONG to_addr() const;

	template <typename T>
	T to_() const {
		static_assert(is_Numeric_v<T>, "Type T must be (int, long, float, double, ....)");

		std::basic_istringstream<TCHAR> ss(m_pString);	// makes copy of string :(
		T result;
		return ss >> result ? result : T();
	}
	auto to_int() const { return to_<int>(); };
	auto to_num() const { return to_<__int64>(); };
	auto to_float() const { return to_<float>(); };
	auto to_dword() const { return to_<DWORD>(); };

	template <typename T>
	auto &append_number(T Number)
	{
		static_assert(is_Numeric_v<T>, "Type T must be (int, long, float, double, ....)");

		return append(std::to_wstring(Number).data());
	}

	static inline int rfc_tolower(const int c);
	static inline int rfc_toupper(const int c);
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
TString operator"" _ts(const char *p, size_t N);
TString operator"" _ts(const WCHAR *p, size_t N);


//#pragma comment(lib,"tstring.lib")

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // TSTRING_H_
