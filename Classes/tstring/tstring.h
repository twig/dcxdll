/*!
 * \file tstring.h
 * \brief String and Token Management Class
 *
 * This experimental library was built to reproduce easy to use string containers managing effectively 
 * comparisons and token manipulations as done in the mIRC scripting language.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.12
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
 * © ScriptsDB.org - 2005-2015
 */
#if _MSC_VER > 1000
#pragma once
#endif
#ifndef _TSTRING_H_
#define _TSTRING_H_

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

#define TS_getmemsize(x) (size_t)(((x) - 1) + (16 - (((x) - 1) % 16) ))
//#define TS_wgetmemsize(x) (unsigned long)(((((x) - 1)*sizeof(WCHAR)) + (16 - ((((x) - 1)*sizeof(WCHAR)) % 16)))/sizeof(WCHAR))
#define TS_wgetmemsize(x) (TS_getmemsize((x)*sizeof(WCHAR))/sizeof(WCHAR))

// enable this define if you wish to use the mIRC extra functions
#define INCLUDE_MIRC_EXTRAS 0

// enable this define to include code to support: for (auto x: TString)
#define TSTRING_PARTS 1
#define TSTRING_ITERATOR 1

// enable this define to enable using templates to convert TString to numbers. (TString->to_<int>() etc..)
#define TSTRING_TEMPLATES 1

// Enable/Disable test code...
#define TSTRING_TESTCODE 1

#include <iterator>

#include <sstream>

// enable this to include a small internal buffer that avoids an allocation for small strings.
#define TSTRING_INTERNALBUFFER 0
// internal buffer size in characters
#define TSTRING_INTERNALBUFFERSIZE_CCH TS_getmemsize(64)
// internal buffer size in bytes
#define TSTRING_INTERNALBUFFERSIZE_BYTES (TSTRING_INTERNALBUFFERSIZE_CCH*sizeof(TCHAR))

#define SPACE m_cSpace
#define COMMA m_cComma
#define TAB m_cTab

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2304 )
#pragma warning( disable : 2287 )
#endif

#define ts_atoi(x) _wtoi((x))
#define ts_atoi64(x) _wtoi64((x));
#define ts_atof(x) _wtof((x));
#define ts_strtoul(x) wcstoul((x), nullptr, 10)
#define ts_itoa(x,y,z) _itow((x),(y),(z))
#define ts_upr(x,y) _wcsupr_s((x),(y))
#define ts_strstr(x,y) wcsstr((x),(y))
//#define ts_strstr(x,y) _ts_strstr((x),(y))
#define ts_strcat_s(x,y,z) wcscat_s((x),(z),(y))
#define ts_strcat(x,y) lstrcat((x),(y))
#define ts_strncat(x,y,z) wcsncat((x),(y),(size_t)(z))
#define ts_toupper(c) ((((c) >= TEXT('a')) && ((c) <= TEXT('z'))) ? _toupper((c)) : (c) )
#define ts_tolower(c) ((((c) >= TEXT('A')) && ((c) <= TEXT('Z'))) ? _tolower((c)) : (c) )
#define ts_strlen(x) lstrlen((x))
#define ts_strcpyn(dest,src,len) lstrcpyn((dest),(src),(int)(len))
#define ts_strcpy(dest,src) lstrcpy((dest),(src))
#define ts_strcmp(x,y) lstrcmp((x),(y))
#define ts_strncmp(x,y,z) wcsncmp((x),(y),(size_t)(z))
#define ts_vscprintf(fmt, args) _vscwprintf((fmt), (args))
#define ts_vsprintf(txt, cnt, fmt, args ) vswprintf((txt), (cnt), (fmt), (args))

#define ts_copymem(dest,src,sz) CopyMemory((dest),(src),(sz))
#define ts_zeromem(dest, sz) ZeroMemory((dest),(sz))

#define ts_strcat_throw(x,y) if (ts_strcat((x), (y)) == nullptr) throw std::logic_error("strcat() failed!");
#define ts_strncat_throw(x,y,z) if (ts_strncat((x), (y), (z)) == nullptr) throw std::logic_error("strncat() failed!");
#define ts_strcpy_throw(x,y) if (ts_strcpy((x), (y)) == nullptr) throw std::logic_error("strcpy() failed!");
#define ts_strcpyn_throw(x,y,z) if (ts_strcpyn((x), (y), (z)) == nullptr) throw std::logic_error("strcpyn() failed!");

/*!
 * \brief String and Token Management Class
 */
class TString {

	template<class _Ty>
	struct is_Numeric
		: std::_Cat_base<std::is_arithmetic<_Ty>::value
		&& !std::is_same<_Ty, wchar_t>::value
		&& !std::is_same<_Ty, char>::value >
	{	// determine whether _Ty is a Number type (excluding char / wchar)
	};

private:

	void deleteString(const bool bKeepBufferSize = false);
	void deleteTempString(const bool bKeepBufferSize = false);
	UINT i_replace(const TCHAR *const subString, const TCHAR *const rString); // Ook
	static UINT match(register const TCHAR *m, register const TCHAR *n, const bool cs /* case sensitive */);

	//const size_t set_buffersize(const size_t size) { m_buffersize = TS_getmemsize(size); return m_buffersize; };	// make buffersize a multiple of 16
	//allocate a buffer thats sized to multiples of 16 bytes, (%byte + (16 - (%byte % 16)))
	//TCHAR *allocstr_bytes(const size_t size) { return (TCHAR *)(new BYTE[set_buffersize(size)]); };
	auto allocstr_bytes(const size_t size) {
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
	static TCHAR *allocstr_bytes(const size_t size, size_t &iActual)
	{
		iActual = TS_getmemsize(size);
		return reinterpret_cast<TCHAR *>(new BYTE[iActual]);
	};
	// allocate a buffer thats size characters long & its a multiple of 16bytes.
	auto allocstr_cch(const size_t size) { return allocstr_bytes(size*sizeof(TCHAR)); };
	//TCHAR *allocstr_cch(const size_t size) { return allocstr_bytes(size*sizeof(TCHAR), m_buffersize); };
	// allocate a buffer thats size characters long & its a multiple of 16bytes. NB: iActual contains the buffer size in BYTES
	static auto allocstr_cch(const size_t size, size_t &iActual) { return allocstr_bytes(size*sizeof(TCHAR), iActual); };

	// swap contents of second with this
	void swap(TString &second) noexcept;

	// make test string m_pTempString which is m_pString converted to either WCHAR or char
	void MakeTemp() const
	{
		if (this->m_pString == nullptr)
			return;

		if (this->m_pTempString == nullptr)
			this->m_pTempString = WcharTochar(this->m_pString);
	}

	// check if requested character is within buffer (not within string)
	template <class T>
	std::enable_if_t<std::is_signed<T>::value> CheckRange(const T &N) const
	{
		static_assert(is_Numeric<T>::value, "Only Numeric Types accepted.");

		if ((m_pString == nullptr) || (N < 0) || ((size_t)N >= (m_buffersize / sizeof(TCHAR))))
			throw std::out_of_range("TString::at()");
	}
	template <class T>
	std::enable_if_t<std::is_unsigned<T>::value> CheckRange(const T &N) const
	{
		static_assert(is_Numeric<T>::value, "Only Numeric Types accepted.");

		if ((m_pString == nullptr) || (N >= (m_buffersize / sizeof(TCHAR))))
			throw std::out_of_range("TString::at()");
	}

	static WCHAR *charToWchar(const char *const cString, size_t *const buffer_size = nullptr);
	static char *WcharTochar(const WCHAR *const wString, size_t *const buffer_size = nullptr);

	mutable const TCHAR	*m_savedpos;
	mutable UINT	m_savedtotaltoks;
	mutable UINT	m_savedcurrenttok;
	//mutable std::unique_ptr<TCHAR[]> m_tSavedSepChars;
	//mutable size_t	m_savedSepCharsLength;

	size_t			m_buffersize;	// size of string buffer in use. (size in bytes)
	mutable size_t	m_iLen;			// the string length of m_pString
	mutable bool	m_bDirty;		// is buffer `dirty` (string length is unknown)?

#if TSTRING_INTERNALBUFFER
	mutable TCHAR	m_InternalBuffer[TSTRING_INTERNALBUFFERSIZE_CCH];
	mutable bool	m_bUsingInternal;
#endif

public:

	TCHAR * m_pString; //!< String buffer
	// Temp string buffer used for string conversion to/from wchar/char or vice versa depending on what TCHAR is.
	// changes made to m_pTempString are NOT reflected in m_pString!
	mutable char * m_pTempString;
	static const TCHAR *m_cSpace;
	static const TCHAR *m_cComma;
	static const TCHAR *m_cTab;

	TString();
	TString(const WCHAR *const cString);		// we don't want these 3 as explicits
	TString(const char *const cString);
	TString(const TString & tString);
	TString(const TCHAR *const pStart, const TCHAR *const pEnd);
	TString(TString &&tString);					// move constructor C++11 only
	TString(const std::initializer_list<TString> &lt);	// Initializer list constructor (allows TString name{ "text", "text2", othertstring } )

	// template version of the unique ptr constructor, handles WCHAR * & CHAR *
	// maybe add TString * support....
	template <typename T>
	TString(const std::unique_ptr<T> &unique)
		: TString(unique.get())
	{
	}

	explicit TString(const WCHAR chr);
	explicit TString(const char chr);
	explicit TString(const UINT tsSize);

	//! Destructor
	~TString( );

	// Operator Overloads
	TString & operator =(TString &&tString);		// move assignment...

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
	//	static_assert(!is_Numeric<T>::value, "Must be a TString, WCHAR, CHAR, WCHAR * or CHAR *");
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
	std::enable_if_t<!is_Numeric<T>::value, TString> & operator +=(const T &other) { return append(other); }
	template <class T>
	std::enable_if_t<is_Numeric<T>::value, TString> & operator +=(const T &num) { return append_number(num); }

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
	bool operator <=( const T &other ) const { return !(*this > other); }

	template <class T>
	bool operator >=(const T &other) const { return !(*this < other); }

	template <class T>
	bool operator ==(const T &other) const { return (compare(other) == 0); }
	template <>
	bool operator ==(const int &iNull) const { return (this->m_pString == nullptr && !iNull); }

	template <class T>
	bool operator !=(const T &other) const { return !(*this == other); }

	template <class T>
	bool operator <(const T &other) const { return (compare(other) < 0); }

	template <class T>
	bool operator >(const T &other) const { return (compare(other) > 0); }

	template <class T>
	TString operator -( const T &other ) {

		if ( !other )
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
		if (this->m_savedpos == nullptr) return this->getfirsttok(1);
		return this->getnexttok();
	}

	// returns the current token as set by ++ operator above (or getfirsttok()/getnexttok() with a space sepchar)
	const TString operator* () const {
		const TCHAR * p_cStart = m_pString;
		if (m_savedpos != nullptr)
			p_cStart = m_savedpos;
		auto p_cEnd = ts_strstr(p_cStart, SPACE);
		if (p_cEnd == nullptr)
			return TString(p_cStart);

		return TString(p_cStart, p_cEnd);
	}

	template <class T>
	TCHAR & operator [](const T &N) noexcept
	{
#ifndef NDEBUG
		CheckRange(N);	// only debug code checks range here
#endif

		m_bDirty = true;

		return m_pString[N];
	}

	template <class T>
	TCHAR operator [](const T &N) const noexcept
	{
#ifndef NDEBUG
		CheckRange(N);	// only debug code checks range here
#endif
		return m_pString[N];
	}

	template <class T>
	friend TString &operator <<(TString &other, const T &N) { other += N; return other; }
	template <class T>
	friend TString &operator >>(TString &other, T &N) { N = other.to_<T>(); return other; }
	template <class T>
	friend TString operator +(const TString & tString, const T &other)
	{
		auto newTString(tString);
		newTString += other;
		return newTString;
	}

	//conversion operators.
	template <class T, typename std::enable_if_t<is_Numeric<T>::value,int> = 0>
	//template <typename T>
	operator T() const {
		static_assert(is_Numeric<T>::value, "Type T must be (int, long, float, double, ....)");
		return to_<T>();
	}
	explicit operator bool() const noexcept { return !empty(); }

	// General String Lib

	// the actual string data
	const TCHAR *const data() const noexcept { return m_pString; }
	// get length of string in characters
	const size_t len( ) const noexcept;
	// alias for len()
	const size_t length() const noexcept { return len(); };
	// alias for len()
	const size_t size() const noexcept { return len(); };
	// capacity of buffer
	const size_t &capacity() const noexcept { return m_buffersize; }
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
	// remove sub string from string
	TString &remove(const TCHAR *const subString);
	// remove character from string
	TString &remove(const TCHAR &chr);
	// remove sub string from string
	TString &remove(const TString &subString);
	// is string empty?
	const bool empty() const noexcept { return (m_pString == nullptr || m_pString[0] == TEXT('\0')); };
	// refrence to char at N
	TCHAR &at(const size_t N)
	{
		CheckRange(N);
		return m_pString[N];
	}
	// copy of char at N
	TCHAR at(const size_t N) const
	{
		CheckRange(N);
		return m_pString[N];
	}
	// allocate memory for string, preserves contents...
	void reserve(const size_t tsSize);
	// copy string...
	void copy(TString other);
	// compare strings...
	int compare(const TString &other) const;
	int compare(const TCHAR &other) const;
	int compare(const TCHAR *const other) const;
	int compare(const TCHAR *const other, const size_t iLength) const;

	// compare 'this' to an array, array type can be anything supported by the == operator.
	// returns the index of the matching item, or zero for failure.
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

#ifdef GSL_GSL_H
	// messing about with this, dont use in normal code.
	template <typename T>
	UINT acompare(const gsl::span<T> &arr) const
	{
		//UINT i = 0;
		//for (auto &x: arr)
		//{
		//	i++;
		//	if (*this == x)
		//		return i;
		//}
		//return 0U;

		auto itEnd = arr.end();
		auto itGot = std::find(arr.begin(), itEnd, *this);
		if (itGot != arr.end())
			return itGot;
		return 0U;
	}
#endif

	int find(const TCHAR *const substring, const int N) const;	// find Nth matching subString
	int find( const TCHAR chr, const int N ) const;				// find Nth matching chr

	TString sub( int N, int M ) const;

	TString &trim();	// removes spaces at start & end of text.
	TString &strip();	// removes spaces at start & end of text & all ctrl codes in text.

	UINT replace(const TCHAR *const subString, const TCHAR *const rString);	// replace subString with rString
	UINT replace(const TCHAR *const subString, const TCHAR rchr);			// replace subString with rchr
	UINT replace(const TCHAR chr, const TCHAR *const rString);				// replace chr with rString
	UINT replace(const TCHAR chr, const TCHAR rchr );						// replace chr with rchr
	UINT mreplace(const TCHAR chr, const TCHAR *const fmt);					// replace any char in fmt with chr

#if 0
	// messing about with this, dont use in normal code.
	template <typename T, typename M>
	UINT test_replace(const T &subString, const M &rString)
	{
		if (this->empty())
			return 0U;

		auto subl = subString.length();

		if (subl == 0)
			return 0U;

		auto c = 0U;
		auto p = subString.data();
		_ts_strstr(data(), p);
		return c;
	}
#endif

	// Token Lib
	template <typename T>
	void addtok(const T &tToken)
	{
		addtok(tToken, SPACE);
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

		const auto nToks = this->numtok(sepChars);

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
			tsTmp = this->gettok(1, N -1, sepChars);
			// then new token
			tsTmp += sepChars;
			tsTmp += cToken;
			// and any tokens after the inserted one.
			if (N < nToks) {
				tsTmp += sepChars;
				tsTmp += this->gettok(N, -1, sepChars);
			}
		}
		this->swap(tsTmp);
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
			TString tmp{ gettok(1, N - 1, sepChars), sepChars, cToken, sepChars, gettok(N + 1, -1, sepChars) };
			this->swap(tmp);
		}
		else {
			// replace last token
			TString tmp{ gettok(1, N - 1, sepChars), sepChars, cToken };
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
	TString getfirsttok(const UINT N, const TCHAR *const sepChars = SPACE) const;			// must be called before the first getnexttok()
	TString getnexttok(const TCHAR *const sepChars = SPACE) const;							// gets subsequent tokens after a getfirsttok() call.
	TString getlasttoks() const;															// gets all remaining tokens after a getfirsttok()/getnexttok() call.
	const bool moretokens() const noexcept {	return (m_savedpos != nullptr);	}
	void resettokens() const noexcept {
		m_savedcurrenttok = 0;
		m_savedpos = nullptr;
		m_savedtotaltoks = 0;
	}
	TString matchtok(const TCHAR *const mString, UINT N, const TCHAR *const sepChars = SPACE) const;
	UINT numtok(const TCHAR *const sepChars = SPACE) const;

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

	// added by Ook
	TString wildtok(const TCHAR *const wildString, const UINT N, const TCHAR *const sepChars = SPACE) const;
	UINT nwildtok(const TCHAR *const wildString, const TCHAR *const sepChars = SPACE) const;

	template <typename tsType, typename T = const TCHAR>
	class tsIterator
		: public std::iterator<std::forward_iterator_tag, tsType, ptrdiff_t, tsType*, tsType&>
	{
	public:
		tsIterator(const tsIterator<tsType, T> &other) = default;
		tsIterator(tsType *ptr = nullptr) : tsIterator(ptr, SPACE) { if (m_ptr == nullptr) m_iIndex = 0; }
		tsIterator(tsType *ptr, T *const sepChars)
			: m_ptr(ptr), m_iIndex(1), m_sepChars(sepChars), m_toks(0)/*, m_iPrevIndex(1)*/
			, m_savedStart(nullptr), m_savedEnd(nullptr), m_savedFinal(nullptr)
		{
			if (m_sepChars == nullptr)
			{
				m_sepChars = SPACE;
				m_sepCharsLen = 1;
			}
			else
				m_sepCharsLen = ts_strlen(m_sepChars);

			if (m_ptr != nullptr)
			{
				m_toks = m_ptr->numtok(sepChars);
				m_savedStart = m_ptr->m_pString;
				if (m_savedStart != nullptr)
				{
					m_savedEnd = ts_strstr(m_savedStart, m_sepChars);
					m_savedFinal = (TCHAR *)(m_savedStart + m_ptr->len());
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
					m_savedFinal = (TCHAR *)(m_savedStart + m_ptr->len());
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
				return TEXT("");

			if ((m_iIndex == m_toks) || (m_savedEnd == nullptr))
				return m_savedStart;

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
		TCHAR *m_savedFinal;
	};
	typedef tsIterator<TString, const TCHAR> iterator;
	typedef tsIterator<const TString, const TCHAR> const_iterator;

	inline iterator begin() { return iterator(this); }
	inline iterator begin(const TCHAR *const sepChars) { return iterator(this, sepChars); }
	inline iterator end() { return iterator(); }
	inline const const_iterator begin() const { return const_iterator(this); };
	inline const const_iterator begin(const TCHAR *const sepChars) const { return const_iterator(this, sepChars); }
	inline const const_iterator end() const { return const_iterator(); }

	// add the contents of any container class to the TString using the specified seperator (can also add other TString's like this)
	template <class T, class TS> auto &join(T &Cont, TS &sepChars)
	{
		for (const auto &x : Cont)
		{
			addtok(x, sepChars);
		}
		return *this;
	}

	// fill a container class with a split up TString
	template <class T, class TS> void expand(T &Cont, TS &sepChars)
	{
		for (auto itStart = begin(sepChars), itEnd = end(); itStart != itEnd; ++itStart)
		{
			Cont.push_back((*itStart));
		}
	}

#if INCLUDE_MIRC_EXTRAS
	// extras for mIRC
	bool isnum(const int f) const;
	bool isincs(const TCHAR let) const;
	UINT countchar(const TCHAR chr) const;
	bool ishostmask(void) const;

	TString toupper(void) const;
	TString tolower(void) const;

#endif

#if TSTRING_TESTCODE
	template <typename T> bool iswm(const T &a) { return _ts_WildcardMatch(*this, a); }
#else
	bool iswm(const TCHAR *const a) const;
#endif

	bool iswmcs(const TCHAR *const a) const;

	// extract left/right/mid
	TString mid(const int pos, int n) const;
	TString left(int n) const;
	TString right(int n) const;

	int tsprintf(const TCHAR *const fmt, ...);
	int tvprintf(const TCHAR *const fmt, va_list args);

	TCHAR * to_chr() noexcept { m_bDirty = true;  return this->m_pString; };	// returns the string in the projects current format. (string can be altered)
	const TCHAR * to_chr() const noexcept { return this->m_pString; };	// returns the string in the projects current format. (string can't be altered)
	WCHAR *to_wchr() noexcept { m_bDirty = true;  return this->m_pString; };	// returns the string in wide format (string can be altered)
	const WCHAR *const to_wchr() const noexcept { return this->m_pString; };	// returns the string in wide format (string can't be altered)
	char * c_str(void)	// returns the string as a char * (string can be altered)
	{
		MakeTemp();
		return m_pTempString;
	}
	const char *const c_str(void) const	// returns the string as a char * (string can't be altered)
	{
		MakeTemp();
		return m_pTempString;
	}
	ULONG to_addr() const;

	template <typename T>
	T to_() const {
		static_assert(is_Numeric<T>::value, "Type T must be (int, long, float, double, ....)");

		std::basic_istringstream<TCHAR> ss(this->m_pString);	// makes copy of string :(
		T result;
		return ss >> result ? result : 0;
	}
	template <typename T>
	auto &append_number(T Number)
	{
		static_assert(is_Numeric<T>::value, "Type T must be (int, long, float, double, ....)");

		//std::basic_ostringstream<TCHAR> ss;
		//ss << Number;
		//return this->append(ss.str().data());

		//if (sizeof(TCHAR) == sizeof(WCHAR))
		//	return this->append(std::to_wstring(Number).data());
		//
		//return this->append(std::to_string(Number).data());

		return this->append(std::to_wstring(Number).data());
	}
	auto to_int() const { return to_<int>(); };
	auto to_num() const { return to_<__int64>(); };
	auto to_float() const { return to_<float>(); };
	auto to_dword() const { return to_<DWORD>(); };

	static inline int rfc_tolower(const int c);
	static inline int rfc_toupper(const int c);
	//
};
#define TSTAB TString::TAB
#define TSSPACE TString::SPACE
#define TSCOMMA TString::COMMA

// literal operator
// allows "sometext"_ts to be interpreted as TString("sometext")
TString operator"" _ts(const char *p, size_t);

namespace detail {
	template <typename Result, typename Format>
	Result &_ts_printf_do(Result &res, const Format &fmt)
	{
		res += fmt;
		return res;
	}

	template <typename Result, typename Format, typename Value, typename... Arguments>
	Result &_ts_printf_do(Result &res, const Format &fmt, const Value val, Arguments&&... args)
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
				if (c == decltype(c){'%'})
				{
					res += val;
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
	struct impl_strstr{
		typedef typename TR::char_type* ptype;
		typedef const typename TR::char_type* cptype;

		ptype operator()(ptype input, cptype find){
			do {
				cptype p, q;
				for (p = input, q = find; !TR::eq(*q, 0) && TR::eq(*p, *q); p++, q++) {	}

				if (TR::eq(*q, 0))
					return input;

			} while (!TR::eq(*(input++), 0));
			return nullptr;
		}
	};
}

template <typename Result, typename Format, typename Value, typename... Arguments>
Result &_ts_sprintf(Result &res, const Format &fmt, const Value val, Arguments&&... args)
{
	res.clear();
	return detail::_ts_printf_do(res, fmt, val, args...);
}

template <class T>
T *_ts_strstr(T *input, const std::remove_const_t<T> *find)
{
	return detail::impl_strstr<T>()(input, find);
}

template <typename TameString, typename WildString>
bool _ts_WildcardMatch(const TameString &pszString, const WildString &pszMatch)
{
	if ((pszMatch == nullptr) || (pszString == nullptr))
		return false;

	ptrdiff_t MatchPlaceholder = 0;
	ptrdiff_t StringPlaceholder = 0;
	ptrdiff_t iWildOffset = 0;
	ptrdiff_t iTameOffset = 0;

	while (pszString[iTameOffset])
	{
		if (pszMatch[iWildOffset] == TEXT('*'))
		{
			if (pszMatch[++iWildOffset] == TEXT('\0'))
				return true;
			MatchPlaceholder = iWildOffset;
			StringPlaceholder = iTameOffset + 1;
		}
#if TSTRING_WILDT
		else if (pszMatch[iWildOffset] == TEXT('~') && pszString[iTameOffset] == TEXT(' '))
		{
			iWildOffset++;
			while (pszString[iTameOffset] == TEXT(' '))
				iTameOffset++;
		}
#endif
#if TSTRING_WILDA
		else if (pszMatch[iWildOffset] == TEXT('^'))
		{
			iWildOffset++;
			if (_toupper(pszMatch[iWildOffset]) == _toupper(pszString[iTameOffset]))
				iTameOffset++;
			iWildOffset++;
		}
#endif
#if TSTRING_WILDE
		else if (pszMatch[iWildOffset] == TEXT('\\'))
		{
			// any character following a '\' is taken as a literal character.
			iWildOffset++;
			if (_toupper(pszMatch[iWildOffset]) != _toupper(pszString[iTameOffset]))
				return false;
			iTameOffset++;
		}
#endif
#if TSTRING_WILDW
		else if (pszMatch[iWildOffset] == TEXT('#'))
		{
			iWildOffset++;
			while (pszString[iTameOffset] && (pszString[iTameOffset] != TEXT(' ') || pszString[iTameOffset] != TEXT('\t') || pszString[iTameOffset] != TEXT('\n') || pszString[iTameOffset] != TEXT('\r')))
				iTameOffset++;
		}
#endif
		else if (pszMatch[iWildOffset] == TEXT('?')|| _toupper(pszMatch[iWildOffset]) == _toupper(pszString[iTameOffset]))
		{
			iWildOffset++;
			iTameOffset++;
		}
		else if (StringPlaceholder == 0)
			return false;
		else
		{
			iWildOffset = MatchPlaceholder;
			iTameOffset = StringPlaceholder++;
		}
	}

	while (pszMatch[iWildOffset] == TEXT('*'))
		iWildOffset++;

	return (pszMatch[iWildOffset] == TEXT('\0'));
}

//#pragma comment(lib,"tstring.lib")

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // TSTRING_H_
