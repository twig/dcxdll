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
 *
 * © ScriptsDB.org - 2005-2014
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

#define TS_getmemsize(x) (((x) - 1) + (16 - (((x) - 1) % 16) ))
//#define TS_wgetmemsize(x) (unsigned long)(((((x) - 1)*sizeof(WCHAR)) + (16 - ((((x) - 1)*sizeof(WCHAR)) % 16)))/sizeof(WCHAR))
#define TS_wgetmemsize(x) (TS_getmemsize((x)*sizeof(WCHAR))/sizeof(WCHAR))

// enable this define if you wish to use the mIRC extra functions
//#define INCLUDE_MIRC_EXTRAS 1

// enable this define to include code to support: for (auto x: TString)
#define TSTRING_PARTS 1

// enable this define to enable using templates to convert TString to numbers. (TString->to_<int>() etc..)
#define TSTRING_TEMPLATES 1

#ifdef TSTRING_PARTS
#include <iterator>
#endif

#ifdef TSTRING_TEMPLATES
#include <sstream>
#endif

// enable this to include a small internal buffer that avoids an allocation for small strings.
#define TSTRING_INTERNALBUFFER 1
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

#ifdef TSTRING_PARTS
class TString;

typedef std::vector<TString> TStringList;
#endif

#ifdef UNICODE
#define ts_atoi(x) _wtoi((x))
#define ts_atoi64(x) _wtoi64((x));
#define ts_atof(x) _wtof((x));
#define ts_strtoul(x) wcstoul((x), nullptr, 10)
#define ts_itoa(x,y,z) _itow((x),(y),(z))
#define ts_upr(x,y) _wcsupr_s((x),(y))
#define ts_strstr(x,y) wcsstr((x),(y))
#define ts_strcat_s(x,y,z) wcscat_s((x),(z),(y))
#define ts_strcat(x,y) lstrcat((x),(y))
#define ts_strncat(x,y,z) wcsncat((x),(y),(z))
#define ts_toupper(c) ((((c) >= TEXT('a')) && ((c) <= TEXT('z'))) ? _toupper((c)) : (c) )
#define ts_tolower(c) ((((c) >= TEXT('A')) && ((c) <= TEXT('Z'))) ? _tolower((c)) : (c) )
#define ts_strlen(x) lstrlen((x))
#define ts_strcpyn(dest,src,len) lstrcpyn((dest),(src),(len))
#define ts_strcpy(dest,src) lstrcpy((dest),(src))
#define ts_strcmp(x,y) lstrcmp((x),(y))
#define ts_strncmp(x,y,z) wcsncmp((x),(y),(z))
#define ts_vscprintf(fmt, args) _vscwprintf((fmt), (args))
#define ts_vsprintf(txt, cnt, fmt, args ) vswprintf((txt), (cnt), (fmt), (args))
#else
#define ts_atoi(x) atoi((x))
#define ts_atoi64(x) _atoi64((x));
#define ts_atof(x) atof((x));
#define ts_strtoul(x) strtoul((x), nullptr, 10)
#define ts_itoa(x,y,z) _itoa((x),(y),(z))
#define ts_upr(x,y) _strupr_s((x),(y))
#define ts_strstr(x,y) strstr((x),(y))
#define ts_strcat_s(x,y,z) strcat_s((x),(z),(y))
#define ts_strcat(x,y) lstrcat((x),(y))
#define ts_strncat(x,y,z) strncat((x),(y),(z))
//#define ts_toupper(c) ( (((c) >= TEXT('a')) && ((c) <= TEXT('z'))) ? _toupper((c)) : (c) )
#define ts_toupper(c) rfc_toupper((c))
#define ts_tolower(c) rfc_tolower((c))
#define ts_strlen(x) lstrlen((x))
#define ts_strcpyn(dest,src,len) lstrcpyn((dest),(src),(len))
#define ts_strcpy(dest,src) lstrcpy((dest),(src))
#define ts_strcmp(x,y) lstrcmp((x),(y))
#define ts_strncmp(x,y,z) strncmp((x),(y),(z))
#define ts_vscprintf(fmt, args) _vscprintf((fmt), (args))
#define ts_vsprintf(txt, cnt, fmt, args ) vsprintf_s((txt), (cnt), (fmt), (args))
#endif

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

private:

	void deleteString(const bool bKeepBufferSize = false);
	void deleteTempString(const bool bKeepBufferSize = false);
	int i_replace(const TCHAR *const subString, const TCHAR *const rString); // Ook
	int i_remove(const TCHAR *const subString);
	static int match(register const TCHAR *m, register const TCHAR *n, const bool cs /* case sensitive */);

	//const size_t set_buffersize(const size_t size) { m_buffersize = TS_getmemsize(size); return m_buffersize; };	// make buffersize a multiple of 16
	//allocate a buffer thats sized to multiples of 16 bytes, (%byte + (16 - (%byte % 16)))
	//TCHAR *allocstr_bytes(const size_t size) { return (TCHAR *)(new BYTE[set_buffersize(size)]); };
	TCHAR *allocstr_bytes(const size_t size) {
#if TSTRING_INTERNALBUFFER
		if (size <= TSTRING_INTERNALBUFFERSIZE_BYTES)
		{
			m_bUsingInternal = true;
			m_InternalBuffer[0] = TEXT('\0');
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
	TCHAR *allocstr_cch(const size_t size) { return allocstr_bytes(size*sizeof(TCHAR)); };
	//TCHAR *allocstr_cch(const size_t size) { return allocstr_bytes(size*sizeof(TCHAR), m_buffersize); };
	// allocate a buffer thats size characters long & its a multiple of 16bytes. NB: iActual contains the buffer size in BYTES
	static TCHAR *allocstr_cch(const size_t size, size_t &iActual) { return allocstr_bytes(size*sizeof(TCHAR), iActual); };

	// swap contents of second with this
	void swap(TString &second); // nothrow

	// make test string m_pTempString which is m_pString converted to either WCHAR or char
	void MakeTemp() const
	{
		if (this->m_pString == nullptr)
			return;

#if UNICODE
		if (this->m_pTempString == nullptr)
			this->m_pTempString = WcharTochar(this->m_pString);
#else
		if (this->m_pTempString == nullptr)
			this->m_pTempString = charToWchar(this->m_pString);
#endif
	}

	// check if requested character is within buffer (not within string)
	void CheckRange(long int N) const
	{
		if ((N < 0) || (N >= static_cast<long int>(m_buffersize / sizeof(TCHAR))))
			throw std::out_of_range("TString::at()");
	}

#if !UNICODE
	static unsigned char tolowertab[];
	static unsigned char touppertab[];
#endif
	static WCHAR *charToWchar(const char *const cString, size_t *const buffer_size = nullptr);
	static char *WcharTochar(const WCHAR *const wString, size_t *const buffer_size = nullptr);

	mutable const TCHAR	*m_savedpos;
	mutable UINT	m_savedtotaltoks;
	mutable UINT	m_savedcurrenttok;
	//mutable std::unique_ptr<TCHAR[]> m_tSavedSepChars;
	//mutable size_t	m_savedSepCharsLength;

	size_t			m_buffersize;	// size of string buffer in use. (size in bytes)
	mutable bool	m_bDirty;		// is buffer `dirty` (string length is unknown)?
	mutable size_t	m_iLen;			// the string length of m_pString

#if TSTRING_INTERNALBUFFER
	mutable TCHAR	m_InternalBuffer[TSTRING_INTERNALBUFFERSIZE_CCH];
	mutable bool	m_bUsingInternal;
#endif

#ifdef TSTRING_PARTS
	mutable TStringList	m_SplitParts;
	typedef TStringList::iterator iterator;
	typedef TStringList::const_iterator const_iterator;
#endif

public:

	TCHAR * m_pString; //!< String buffer
	// Temp string buffer used for string conversion to/from wchar/char or vice versa depending on what TCHAR is.
	// changes made to m_pTempString are NOT reflected in m_pString!
#ifdef UNICODE
	mutable char * m_pTempString;
#else
	mutable WCHAR * m_pTempString;
#endif
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
	TString(const std::unique_ptr<TCHAR[]> &unique);
	//template <typename T>
	//TString::TString(const std::unique_ptr<T> &unique)
	//	: TString(unique.get())
	//{
	//}

	explicit TString(const WCHAR chr);
	explicit TString(const char chr);
	explicit TString(const UINT tsSize);

	//! Destructor
	~TString( );

	// Operator Overloads
	TString & operator =( const TString &tString );
	TString & operator =(TString &&tString);		// move assignment...
	TString & operator =(const WCHAR *const cString);
	TString & operator =( const WCHAR chr );
	TString & operator =( const char *const cString );
	TString & operator =( const char chr );

	TString operator +( const TCHAR *const cString );
	TString operator +( const TCHAR chr );
	TString operator +( const TString & tString );

	TString operator -( const TCHAR *const cString );
	TString operator -( const TCHAR chr );
	TString operator -( const TString & tString );

	TString & operator +=( const TCHAR *const cString );
	TString & operator +=( const TCHAR chr );
	TString & operator +=( const TString & tString );
	TString & operator +=(const int num);

	TString & operator -=( const TCHAR *const cString );
	TString & operator -=( const TCHAR chr );
	TString & operator -=( const TString & tString );

	bool operator ==( const int iNull ) const;
	bool operator ==( const TCHAR *const cString ) const;
	bool operator ==( const TCHAR chr ) const;
	bool operator ==( const TString & tString ) const;

	bool operator !=( const int iNull ) const;
	bool operator !=( const TCHAR *const cString ) const;
	bool operator !=( const TCHAR chr ) const;
	bool operator !=( const TString & tString ) const;

	bool operator >( const TCHAR *const cString ) const;
	bool operator >( const TCHAR chr ) const;
	bool operator >( const TString & tString ) const;

	bool operator >=( const TCHAR *const cString ) const;
	bool operator >=( const TCHAR chr ) const;
	bool operator >=( const TString & tString ) const;

	bool operator <( const TCHAR *const cString ) const;
	bool operator <( const TCHAR chr ) const;
	bool operator <( const TString & tString ) const;

	bool operator <=( const TCHAR *const cString ) const;
	bool operator <=( const TCHAR chr ) const;
	bool operator <=( const TString & tString ) const;

	TString operator *( const int &N );
	TString & operator *=( const int &N );

	TString operator ++(int N) const {
		// testing this code for getting multiple tokens in a row
		// NB: only works with space token
		// NB: post-increment works for us as we want to return a copy (the token)
		// NB: pre-increment doesn't work for this
		if (this->m_savedpos == nullptr) return this->getfirsttok(1);
		return this->getnexttok();
	}

	TCHAR & operator []( long int N );
	TCHAR operator [](long int N) const;

	//conversion operators.
#ifdef TSTRING_TEMPLATES
	//friend TString &operator <<(TString &other, const int &N) { other += N; return other; }
	//friend TString &operator >>(TString &other, int &N) { N = other.to_int(); return other; }
	template <class T>
	friend TString &operator <<(TString &other, const T &N) { other += N; return other; }
	template <class T>
	friend TString &operator >>(TString &other, T &N) { N = other.to_<T>(); return other; }
	//template <class T>
	//operator T() const { return to_<T>(); }
	operator int() const { return this->to_<int>(); }
	operator __int64() const { return this->to_<__int64>(); }
	operator double() const { return this->to_<double>(); }
#else
	operator int() const { return this->to_int(); }
	operator __int64() const { return this->to_num(); }
	operator double() const { return this->to_float(); }
#endif

	friend TString operator +(const TString & tString, const TCHAR *const cString);

	// General String Lib

	// get length of string in characters
	const size_t len( ) const;
	// alias for len()
	const size_t size() const { return len(); };
	// capacity of buffer
	const size_t &capacity() const { return m_buffersize; }
	// clear string buffer & reset all vars & pointers (doesn't free buffer, just zeros it)
	void clear();
	// shrink string buffer to min size required for string (while still being a multiple of 16)
	void shrink_to_fit();
	// append a wide char string.
	TString &append(const WCHAR *const cString);
	// append a char string.
	TString &append(const char *const cString);
	// append another TString object
	TString &append(const TString &tString);
	// append a string thats limited to iChars characters.
	TString &append(const TCHAR *const cString, const size_t iChars);
	// is string empty?
	const bool empty() const { return (m_pString == nullptr || m_pString[0] == TEXT('\0')); };
	// refrence to char at N
	TCHAR &at(long int N)
	{
		CheckRange(N);
		return m_pString[N];
	}
	// copy of char at N
	TCHAR at(long int N) const
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

	int find(const TCHAR *const substring, const int N) const;	// find Nth matching subString
	int find( const TCHAR chr, const int N ) const;				// find Nth matching chr

	TString sub( int N, int M ) const;

	TString &trim();	// removes spaces at start & end of text.
	TString &strip();	// removes spaces at start & end of text & all ctrl codes in text.

	int replace(const TCHAR *const subString, const TCHAR *const rString);	// replace subString with rString
	int replace(const TCHAR *const subString, const TCHAR rchr);			// replace subString with rchr
	int replace(const TCHAR chr, const TCHAR *const rString);				// replace chr with rString
	int replace(const TCHAR chr, const TCHAR rchr );						// replace chr with rchr
	int mreplace(const TCHAR chr, const TCHAR *const fmt);					// replace any char in fmt with chr

	// Token Lib
	void addtok( const TCHAR *const cToken, const TCHAR *const sepChars = SPACE );
	void addtok( const __int64 nToken, const TCHAR *const sepChars = SPACE );
	//void addtok( const TString &cToken, const TCHAR * sepChars = SPACE );
	void deltok(const UINT N, const TCHAR *const sepChars = SPACE);
	size_t findtok(const TCHAR *const cToken, const UINT N, const TCHAR *const sepChars = SPACE) const;
	TString gettok(int N, const TCHAR *const sepChars = SPACE) const;
	TString gettok(int N, int M, const TCHAR *const sepChars = SPACE) const;
	TString getfirsttok(const UINT N, const TCHAR *const sepChars = SPACE) const;			// must becalled before the first getnexttok()
	TString getnexttok(const TCHAR *const sepChars = SPACE) const;							// gets subsequent tokens after a getfirsttok() call.
	TString getlasttoks() const;															// gets all remaining tokens after a getfirsttok()/getnexttok() call.
	void instok(const TCHAR *const cToken, const UINT N, const TCHAR *const sepChars = SPACE);
	bool istok( const TCHAR *const cToken, const TCHAR *const sepChars = SPACE ) const;
	TString matchtok(const TCHAR *const mString, int N, const TCHAR *const sepChars = SPACE) const;
	UINT numtok(const TCHAR *const sepChars = SPACE) const;
	void puttok(const TCHAR *const cToken, const UINT N, const TCHAR *const sepChars = SPACE);
	void remtok(const TCHAR *const cToken, const UINT N, const TCHAR *const sepChars = SPACE);
	void reptok(const TCHAR *const cToken, const TCHAR *const newToken, int N, const TCHAR *const sepChars = SPACE);
	void sorttok(const TCHAR *const sortOptions, const TCHAR *const sepChars = SPACE);

	// added by Ook
	TString wildtok(const TCHAR *const wildString, const UINT N, const TCHAR *const sepChars = SPACE) const;
	UINT nwildtok(const TCHAR *const wildString, const TCHAR *const sepChars = SPACE) const;

#ifdef TSTRING_PARTS
	size_t split(const TCHAR *const sepChars = SPACE) const;
	const TString &part(const size_t N) const;
	const TStringList &parts(const TCHAR *const sepChars = SPACE) const;
	void ClearParts() const { m_SplitParts.clear(); }
	iterator begin();
	iterator begin(const TCHAR *const sepChars);
	iterator end();
	const const_iterator begin() const;
	const const_iterator begin(const TCHAR *const sepChars) const;
	const const_iterator end() const;
#endif

	////-------------------------------------------------------------------
	//// Raw iterator with random access
	////-------------------------------------------------------------------
	//template<typename blDataType>
	//class blRawIterator : public std::iterator<std::random_access_iterator_tag,
	//	blDataType,
	//	ptrdiff_t,
	//	blDataType*,
	//	blDataType&>
	//{
	//public:
	//
	//	blRawIterator(blDataType* ptr = nullptr){ m_ptr = ptr; }
	//	blRawIterator(const blRawIterator<blDataType>& rawIterator) = default;
	//	~blRawIterator(){}
	//
	//	blRawIterator<blDataType>&                  operator=(const blRawIterator<blDataType>& rawIterator) = default;
	//	blRawIterator<blDataType>&                  operator=(blDataType* ptr){ m_ptr = ptr; return (*this); }
	//
	//	operator bool()const
	//	{
	//		if (m_ptr)
	//			return true;
	//		else
	//			return false;
	//	}
	//
	//	bool                                        operator==(const blRawIterator<blDataType>& rawIterator)const{ return (m_ptr == rawIterator.getConstPtr()); }
	//	bool                                        operator!=(const blRawIterator<blDataType>& rawIterator)const{ return (m_ptr != rawIterator.getConstPtr()); }
	//
	//	blRawIterator<blDataType>&                  operator+=(const ptrdiff_t& movement){ m_ptr += movement; return (*this); }
	//	blRawIterator<blDataType>&                  operator-=(const ptrdiff_t& movement){ m_ptr -= movement; return (*this); }
	//	blRawIterator<blDataType>&                  operator++(){ ++m_ptr; return (*this); }
	//	blRawIterator<blDataType>&                  operator--(){ --m_ptr; return (*this); }
	//	blRawIterator<blDataType>                   operator++(ptrdiff_t){ auto temp(*this); ++m_ptr; return temp; }
	//	blRawIterator<blDataType>                   operator--(ptrdiff_t){ auto temp(*this); --m_ptr; return temp; }
	//	blRawIterator<blDataType>                   operator+(const ptrdiff_t& movement){ auto oldPtr = m_ptr; m_ptr += movement; auto temp(*this); m_ptr = oldPtr; return temp; }
	//	blRawIterator<blDataType>                   operator-(const ptrdiff_t& movement){ auto oldPtr = m_ptr; m_ptr -= movement; auto temp(*this); m_ptr = oldPtr; return temp; }
	//
	//	ptrdiff_t                                   operator-(const blRawIterator<blDataType>& rawIterator){ return std::distance(rawIterator.getPtr(), this->getPtr()); }
	//
	//	blDataType&                                 operator*(){ return *m_ptr; }
	//	const blDataType&                           operator*()const{ return *m_ptr; }
	//	blDataType*                                 operator->(){ return m_ptr; }
	//
	//	blDataType*                                 getPtr()const{ return m_ptr; }
	//	const blDataType*                           getConstPtr()const{ return m_ptr; }
	//
	//protected:
	//
	//	blDataType*                                 m_ptr;
	//};
	////-------------------------------------------------------------------

	//template <typename A, typename T>
	//class CTypeArrayIterator
	//{
	//public:
	//	CTypeArrayIterator(A& collection, INT_PTR const index) :
	//		m_index(index),
	//		m_collection(collection)
	//	{
	//	}
	//
	//	bool operator!= (CTypeArrayIterator const & other) const
	//	{
	//		return m_index != other.m_index;
	//	}
	//
	//	T& operator* () const
	//	{
	//		return m_collection[m_index];
	//	}
	//
	//	CTypeArrayIterator const & operator++ ()
	//	{
	//		++m_index;
	//		return *this;
	//	}
	//
	//private:
	//	INT_PTR  m_index;
	//	A&       m_collection;
	//};
	//
	//inline CTypeArrayIterator<CStringArray, CString> begin(CStringArray& collection)
	//{
	//	return CTypeArrayIterator<CStringArray, CString>(collection, 0);
	//}
	//
	//inline CTypeArrayIterator<CStringArray, CString> end(CStringArray& collection)
	//{
	//	return CTypeArrayIterator<CStringArray, CString>(collection, collection.GetCount());
	//}

//#ifdef TSTRING_PARTS
//	template <typename tsType, typename T>
//	class tsIterator
//		: public std::iterator<std::forward_iterator_tag, tsType, ptrdiff_t, tsType*, tsType&>
//	{
//		tsIterator(const tsIterator<tsType> &other) = default;
//		tsIterator(tsType *ptr = nullptr) : m_ptr(ptr), m_iIndex(0), m_sepChars(TSSPACE) {}
//		tsIterator(tsType *ptr, T *sepChars) : m_ptr(ptr), m_iIndex(0), m_sepChars(sepChars) {}
//		~tsIterator(){}
//
//		tsIterator<tsType> &operator = (const tsIterator<tsType> &other) = default;
//		tsIterator<tsType> &operator = (tsType *ptr) { m_ptr = ptr; return *this; }
//
//		operator bool() const { return (m_ptr != nullptr); }
//
//		bool operator == (const tsIterator<tsType> &other)const{ return (m_ptr == other.getConstPtr()); }
//		bool operator != (const tsIterator<tsType> &other)const{ return (m_ptr != other.getConstPtr()); }
//
//		const tsIterator &operator++ ()
//		{
//			++m_index;
//			return *this;
//		}
//
//		tsType &operator* () const
//		{
//			return m_ptr->gettok(m_iIndex, m_sepChars);
//		}
//
//		tsType *getPtr() const { return m_ptr; }
//		const tsType *getConstPtr() const { return m_ptr; }
//
//	protected:
//		tsType *m_ptr;
//		size_t	m_iIndex;
//		T *m_sepChars;
//	};
//	typedef tsIterator<TString, TCHAR> iterator;
//	typedef tsIterator<const TString, TCHAR> const_iterator;
//
//	//void split(const TCHAR *const sepChars = SPACE) const;
//	iterator begin() { return iterator(this); }
//	iterator begin(TCHAR *sepChars) { return iterator(this, sepChars); }
//	iterator end() { return iterator(); }
//	const const_iterator begin() const;
//	const const_iterator begin(TCHAR *sepChars) const;
//	const const_iterator end() const;
//#endif

#ifdef INCLUDE_MIRC_EXTRAS
	// extras for mIRC
	bool isnum(const int f) const;
	bool isincs(const TCHAR let) const;
	UINT countchar(const TCHAR chr) const;
	bool ishostmask(void) const;

	TString toupper(void) const;
	TString tolower(void) const;

#endif

	bool iswm(const TCHAR *const a) const;
	bool iswmcs(const TCHAR *const a) const;

	// extract left/right/mid
	TString mid(const int pos, int n) const;
	TString left(int n) const;
	TString right(int n) const;

	int tsprintf(const TCHAR *const fmt, ...);
	int tvprintf(const TCHAR *const fmt, va_list * args);

	TCHAR * to_chr() { m_bDirty = true;  return this->m_pString; };	// returns the string in the projects current format.
	const TCHAR * to_chr() const { return this->m_pString; };	// returns the string in the projects current format.
#ifdef UNICODE
	WCHAR *to_wchr() { m_bDirty = true;  return this->m_pString; };	// returns the string in wide format
	const WCHAR *to_wchr() const { return this->m_pString; };	// returns the string in wide format
	char * c_str(void)	// returns the string as a char *
	{
		MakeTemp();
		return m_pTempString;
	}
	const char * c_str(void) const	// returns the string as a char *
	{
		MakeTemp();
		return m_pTempString;
	}
#else
	WCHAR *to_wchr()	// returns the string in wide format
	{
		MakeTemp();
		return m_pTempString;
	}
	const WCHAR *to_wchr() const	// returns the string in wide format
	{
		MakeTemp();
		return m_pTempString;
	}
	char * c_str() { m_bDirty = true;  return this->m_pString; };	// returns the string as a char *
	const char * c_str() const { return this->m_pString; };	// returns the string as a const char *
#endif
	ULONG to_addr() const;

#ifdef TSTRING_TEMPLATES
	template <typename T>
	T to_() const {
		static_assert(std::is_pod<T>::value, "Type T must be Plain Old Data (int, long, float, double, ....)");
		//static_assert(std::is_same<chr, WCHAR>::value || std::is_same<chr, CHAR>::value, "Type T must be WCHAR, CHAR, or TCHAR");

		//std::basic_istringstream<chr> ss(this->m_pString);	// makes copy of string :(
		std::basic_istringstream<TCHAR> ss(this->m_pString);	// makes copy of string :(
		T result;
		return ss >> result ? result : 0;
	}
	template <typename T>
	TString &append_number(T Number)
	{
		static_assert(std::is_pod<T>::value, "Type T must be Plain Old Data (int, long, float, double, ....)");

		//std::basic_ostringstream<TCHAR> ss;
		//ss << Number;
		//return this->append(ss.str().data());

		//if (sizeof(TCHAR) == sizeof(WCHAR))
		//	return this->append(std::to_wstring(Number).data());
		//
		//return this->append(std::to_string(Number).data());

#ifdef UNICODE
		return this->append(std::to_wstring(Number).data());
#else
		return this->append(std::to_string(Number).data());
#endif
	}
	int to_int() const { return to_<int>(); };
	__int64 to_num() const { return to_<__int64>(); };
	float to_float() const { return to_<float>(); };
	DWORD to_dword() const { return to_<DWORD>(); };
#else
	int to_int() const { return ts_atoi(this->m_pString); };
	__int64 to_num() const { return ts_atoi64(this->m_pString); };
	double to_float() const { return ts_atof(this->m_pString); };
	DWORD to_dword() const { return ts_strtoul(this->m_pString); };
#endif

	static inline int rfc_tolower(const int c);
	static inline int rfc_toupper(const int c);
	//
};
#define TSTAB TString::TAB
#define TSSPACE TString::SPACE
#define TSCOMMA TString::COMMA

//#pragma comment(lib,"tstring.lib")

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // TSTRING_H_
