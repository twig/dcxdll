/*!
 * \file TString.cpp
 * \brief String and Token Management Class Functions
 *
 * This experimental library was built to reproduce easy to use string containers managing effectively
 * comparisons and token manipulations as done in the mIRC scripting language.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.21
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
 *	1.20
 *		Added IsPointer concept
 *		Added to_<T>() for changing the data to other pointer types.
 *
 *  1.21
 *      Added some SAL
 *
 * © ScriptsDB.org - 2005-2016
 */
#include "defines.h"

 //#include <windows.h>

#include "TString.h"

TString::TString(const WCHAR chr) noexcept
	: TString()
{
	m_pString[0] = chr;
	m_pString[1] = value_type{};

	m_bDirty = false;
	m_iLen = 1;
}

/****************************/
/*! \fn TString::TString( const char chr )
\brief Constructor input from C char

\param chr Character to initialize string to
*/
/****************************/

//TString::TString(const char chr)
//	: TString(2U)
//{
//	if (chr != '\0')
//	{
//		const char cString[] = { chr, '\0' };
//
//		if (const auto sTemp = charToWchar(&cString[0], &m_buffersize); sTemp)
//		{
//			m_pString = sTemp;
//			m_bUsingInternal = false;
//		}
//	}
//}

TString::TString(const char chr) noexcept
	: TString(static_cast<WCHAR>(chr))
{
}

/****************************/
/*! \fn TString::TString( const TString && tString )
\brief Move Constructor

\param tString TString object to copy
*/
/****************************/
TString::TString(TString&& tString) noexcept
	: TString()
{
	swap(tString);
}

/****************************/
/*! \fn TString::TString( const std::initializer_list<TString> lt )
\brief Initializer list constructor

\param lt - List of TString objects to concate into the new TString object.
*/
/****************************/
TString::TString(const std::initializer_list<TString>& lt)
	: TString()
{
	for (const auto& s : lt)
	{
		append(s);
	}
}

/*!
 * \brief Allocates a buffer tsSize long.
 *
 * NB tsSize is in characters! not bytes.
 *
 */
TString::TString(const UINT tsSize)
{
	//if (tsSize > TSTRING_INTERNALBUFFERSIZE_CCH)
	//{
	//	m_pString = allocstr_cch(tsSize + 1U);
	//	ts_zeromem(m_pString, m_buffersize);
	//}

	setup_buffer((tsSize + 1U) * sizeof(TCHAR));
}

TString::~TString() noexcept
{
	//if (!m_bUsingInternal)
	//	delete[] this->m_pString;

	//delete[] this->m_pTempString;
}

/****************************/
TString& TString::operator =(TString&& tString) noexcept
{
	if (this == &tString)	// self assignment check.
		return *this;

	this->swap(tString);

	return *this;
}

TString& TString::operator =(const TString& tString)
{
	//TString tmp(tString);
	//this->swap(tmp);

	this->copy(tString);

	return *this;
}

/****************************/
/*! \fn void TString::operator=( const TCHAR * string )
	\brief Symbol Overload - TString assignement from C TCHAR string

	\param cString Character string to assign string to
*/
/****************************/
TString& TString::operator =(const WCHAR* const cString)
{
	//TString tmp(cString);
	//this->swap(tmp);

	this->copy(cString);

	return *this;
}

/****************************/
TString& TString::operator =(const char* const cString)
{
	//TString tmp(cString);
	//this->swap(tmp);

	this->copy(cString);

	return *this;
}

/****************************/
/*! \fn void TString::operator=( const TCHAR chr )
	\brief Symbol Overload - TString assignement from C TCHAR

	\param chr Character to assign string to
*/
/****************************/
TString& TString::operator =(const WCHAR chr) noexcept
{
	TString tmp(chr);
	this->swap(tmp);

	//this->copy(chr); // cant convert because constructor is explicit

	[[gsl::suppress(lifetime.4)]] return *this;
}

/****************************/
TString& TString::operator =(const char chr) noexcept
{
	TString tmp(chr);
	this->swap(tmp);

	//this->copy(chr); // cant convert because constructor is explicit

	[[gsl::suppress(lifetime.4)]] return *this;
}

/****************************/
/*! \fn TString TString::operator *( const int N )
	\brief Symbol Overload - TString string Repetition

	\param N Number of repetitions

	\return Returns a TString object of the the new string

	\note Using \b 0 for N makes the string being empty
*/
/****************************/

TString TString::operator *(const int& N) const
{
	if (N < 0 || N == 1)
		return *this;

	const auto sz = this->len();

	if ((N == 0) || (sz == 0))
		return {};

	TString tmp(gsl::narrow_cast<UINT>((sz * N) + 1));

	for (auto i = decltype(N){0}; i < N; ++i)
		ts_strcat_throw(tmp.m_pString, this->m_pString);

	return tmp;
}

/****************************/
/*! \fn void TString::operator *=( const int N )
	\brief Symbol Overload - TString string Repetition

	\param N Number of repetitions

	\note Using \b 0 for N makes the string being empty
*/
/****************************/

TString& TString::operator *=(const int& N)
{
	if (N == 0)
	{
		this->clear();
		return *this;
	}

	const auto sz = len();

	// if string is null just return.
	if ((N == 1) || (sz == 0))
		return *this;

	TString tmp(gsl::narrow_cast<UINT>((sz * N) + 1U));

	for (auto i = decltype(N){0}; i < N; ++i)
	{
		//append(*this);
		ts_strcat_throw(tmp.m_pString, m_pString);
	}

	this->swap(tmp);

	[[gsl::suppress(lifetime.4)]] return *this;
}

/****************************/
/*! \fn size_t TString::len( )
	\brief Returns string length

	\return String length
*/

const TString::size_type& TString::len() const noexcept
{
	if (empty())	// check for zero length string.
		m_iLen = 0U;
	else if (m_bDirty)
		m_iLen = _ts_strlen(m_pString);

	m_bDirty = false;
	return m_iLen;
}

#if !TSTRING_TESTCODE
/****************************/
/*! \fn int TString::find( const TCHAR * substring, int N )
	\brief Function to find position or number of occurences of a substring in the string

	\param substring Substring to search
	\param N Nth substring to search (N = \b 0 > Total number of matches)

	\return > Number of occurrences (N = 0)\n
			> Starting position of \b substring \n
			> \b -1 if function fails or no substring was found
*/
/****************************/

int TString::find(const_pointer_const substring, const int N) const noexcept
{
	//if ((substring != nullptr) && (m_pString != nullptr)) {
	//
	//	const TCHAR * temp = nullptr, *temp2 = m_pString;
	//
	//	auto i = decltype(N){0};
	//	const auto subl = _ts_strlen(substring);
	//	while ((temp = ts_strstr(temp2, substring)) != nullptr) {
	//		i++;
	//		//if ( N != 0 && i == N )
	//		if (i == N) // i is never zero
	//			return static_cast<int>(temp - m_pString);
	//		temp2 = (temp + subl); // Ook
	//	}
	//	if (N == 0)
	//		return i;
	//}
	//return -1;

	if (!_ts_isEmpty(substring) && !_ts_isEmpty(m_pString))
	{
		const TCHAR* temp = nullptr, * temp2 = m_pString;

		auto i = decltype(N){0};
		const auto subl = _ts_strlen(substring);
		while ((temp = _ts_find(temp2, substring)) != nullptr)
		{
			i++;
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

/****************************/
/*! \fn int TString::find( const TCHAR chr, int N )
	\brief Function to find position or number of occurrences of a TCHAR in the string
*/
/****************************/
int TString::find(const_value_type chr, const int N) const noexcept
{
	//auto c = decltype(N){0};
	//const auto len = this->len();
	//
	//for (auto i = decltype(len){0}; i < len; i++) {
	//	// found a match, increase counter
	//	if (this->m_pString[i] == chr)
	//		c++;
	//
	//	// if we've reached the Nth match we want, return the position
	//	if ((N > 0) && (c == N))
	//		return static_cast<int>(i);
	//}
	//
	//// return number of results
	//if (N == 0)
	//	return c;
	//
	//// -1 if no matches
	//return -1;

	auto c = 0;

	for (auto p = m_pString; p && *p; p++)
	{
		// found a match, increase counter
		if (*p == chr)
		{
			c++;

			// if we've reached the Nth match we want, return the position
			if (c == N)
				return static_cast<int>(p - m_pString);
		}
	}

	// return number of results
	if (N == 0)
		return c;

	// -1 if no matches
	return -1;
}
#endif

/****************************/
/*! \fn TString TString::sub( int N, int M )
	\brief Returns a substring of the initial string

	\param N start index
	\param M character count

	\return returns a new TString object containing the substring

	\note > Index starts at \b 0 \n
		  > Index can be negative, \b -1 being the last character
*/
/****************************/

TString TString::sub(int N, int M) const
{
	if (empty())
		return {};

	const decltype(N) size = gsl::narrow_cast<int>(len());

	if (N < 0)
		N += size;

	if (N > size - 1 || N < 0)
		return TString();

	if (N + M > size)
		M = size - N;

	const_pointer_const temp = (m_pString + N);

	return TString(temp, (temp + M));
}

#if !TSTRING_TESTCODE
/*
remove function, called by the other remove()/-/-= functions.
Ook
*/

// NB: This needs looked at!!!! doesnt remove all instances of substring correctly!
TString& TString::remove(const_pointer_const subString)
{
	// no str to remove or no str to remove from
	if ((subString == nullptr) || (empty()) || (subString[0] == TEXT('\0')))
		return *this;

	const_pointer sub = nullptr, p = m_pString;
	auto cnt = 0;
	const auto subl = _ts_strlen(subString);

	TString tmp;
	tmp.reserve(len() + 1);

	while ((sub = ts_strstr(p, subString)) != nullptr)
	{
		ts_strncat_throw(tmp.m_pString, p, (sub - p)); // copy bit before substring. if any.

		cnt++;
		p = sub + subl; // update pointer to skip substring.
	}

	if (cnt > 0)
	{
		ts_strcat_throw(tmp.m_pString, p); // append the end text, if any.

		swap(tmp);
	}

	return *this;
}

TString& TString::remove(const TString& subString)
{
	return remove(subString.m_pString);
}

TString& TString::remove(const_reference chr)
{
	const_value_type tmp[] = { chr, TEXT('\0') };
	return remove(tmp);
}
#endif

#if !TSTRING_TESTCODE
/*
	internal replace function, called by the other replace() functions.
	Ook
*/
UINT TString::i_replace(const_pointer_const subString, const_pointer_const rString)
{
	if ((subString == nullptr) || (rString == nullptr) || this->empty())
		return 0;

	const_pointer sub, p = this->m_pString;
	auto cnt = 0U;
	const auto subl = _ts_strlen(subString);

	// if length of sub string is 0 return 0
	// may change this.
	if (subl == 0)
		return 0;

	// Ook: needs looked at to avoid double loop if we can.
	// see if we have any matches & how many.
	while ((sub = ts_strstr(p, subString)) != nullptr)
	{
		cnt++;
		p = sub + subl;
	}
	// make final string if we have any matches.
	if (cnt > 0)
	{
		p = this->m_pString;

		TString tmp((UINT)((cnt * _ts_strlen(rString)) + (this->len() - (cnt * subl)) + 1)); // allocate new string.

		while ((sub = ts_strstr(p, subString)) != nullptr)
		{
			ts_strncat_throw(tmp.m_pString, p, (sub - p)); // copy bit before substring. if any.
			ts_strcat_throw(tmp.m_pString, rString); // append new replacement string.

			p = sub + subl; // update pointer to skip substring.
		}
		ts_strcat_throw(tmp.m_pString, p); // append the end text, if any.

		this->swap(tmp);
	}
	return cnt;
}

/****************************/
/*! \fn int TString::replace( const TCHAR * substring, const TCHAR * replace )
	\brief blah!
*/
/****************************/

UINT TString::replace(const_pointer_const subString, const_pointer_const rString)
{
	return this->i_replace(subString, rString);
}

/****************************/
/*! \fn int TString::replace( const TCHAR * substring, const TCHAR rchr )
	\brief blah!
*/
/****************************/

UINT TString::replace(const_pointer_const subString, const_value_type rchr)
{
	const_value_type tmp[] = { rchr, TEXT('\0') };
	return this->i_replace(subString, tmp);
}

/****************************/
/*! \fn int TString::replace( const TCHAR chr, const TCHAR * replace )
	\brief blah!
*/
/****************************/

UINT TString::replace(const_value_type chr, const_pointer_const rString)
{
	const_value_type tmp[] = { chr, TEXT('\0') };
	return this->i_replace(tmp, rString);
}

/****************************/
/*! \fn int TString::replace( const TCHAR chr, const TCHAR rchr )
	\brief blah!
*/
/****************************/

UINT TString::replace(const_value_type chr, const_value_type rchr)
{
	auto cnt = 0U;

	for (auto p = this->m_pString; p && *p != TEXT('\0'); p++)
	{
		if (*p == chr)
		{
			*p = rchr;
			cnt++;
		}
	}
	return cnt;
}
#endif

/****************************/
/*! \fn int TString::mreplace( const TCHAR chr, const TCHAR *fmt )
\brief replace every character in string that matches a character in `fmt` with `chr`
*/
/****************************/
UINT TString::mreplace(const_value_type chr, const_pointer_const fmt) noexcept
{
	auto cnt = 0U;

	if (fmt == nullptr)
		return cnt;

	for (auto p = m_pString; p && *p != value_type{}; ++p)
	{
		auto i = 0U;
		for (auto rchr = fmt[0]; rchr != value_type{}; rchr = fmt[i])
		{
			if (*p == rchr)
			{
				*p = chr;
				++cnt;
			}
			++i;
		}
	}
	return cnt;
}

#if !TSTRING_TESTCODE
/*!
 * \brief blah
 *
 * blah
 */

TString TString::gettok(int N, const_pointer_const sepChars) const
{
	if (sepChars == nullptr)
		return *this;

	const auto nToks = this->numtok(sepChars);

	if ((N > static_cast<int>(nToks)) || (nToks == 0))
		return TString();

	if (N < 0)
	{
		N += (nToks + 1);
		if (N < 1)
			return TString();
	}

	const_pointer p_cStart = this->m_pString, p_cEnd = nullptr;
	const_pointer_const p_fEnd = last();
	auto iCount = decltype(N){0};
	const auto sepl = _ts_strlen(sepChars); // Ook

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr)
	{
		iCount++;

		if (iCount == N)
			return TString(p_cStart, p_cEnd);

		p_cStart = p_cEnd + sepl; // Ook
		if (p_cStart >= p_fEnd)	// look out for overrun...
			break;
	}

	if (iCount == N - 1)
		return TString(p_cStart);

	return TString();
}

/*!
 * \brief Get the Nth token from the string
 *
 * params
 * N		-	the token number to retreive
 *				If Zero - returns an empty string.
 *				If Negative gets adjusted to N + (total number of tokens + 1)
 * M		-	End token to get (from N until M)
 *				If Zero returns an empty string
 *				If -1 returns all tokens untill the end of string.
 * sepChars	-	The token seperator string
 *				If nullptr returns the whole current string.
 */

TString TString::gettok(int N, int M, const_pointer_const sepChars) const
{
	if (sepChars == nullptr)
		return *this;

	if (((M < N) && (M != -1)) || (N == 0) || (M == 0))
		return TString();

	if (N == M)
		return this->gettok(N, sepChars);

	const auto nToks = this->numtok(sepChars);

	if ((N > static_cast<int>(nToks)) || (nToks == 0))
		return TString();

	if (N < 0)
	{
		N += (nToks + 1);
		if (N < 1)
			return TString();
	}

	if (M > static_cast<int>(nToks - 1))
		M = -1;

	const_pointer p_cStart = this->m_pString, p_cEnd = nullptr;
	const_pointer_const p_fEnd = last();
	const_pointer p_cFirst = nullptr, p_cLast = nullptr;
	size_t iCount = 0;
	const auto sepl = _ts_strlen(sepChars); // Ook

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr)
	{
		iCount++;

		if (static_cast<int>(iCount) == N)
		{
			p_cFirst = p_cStart;

			if (M == -1)
				break;
		}

		if (static_cast<int>(iCount) == M)
		{
			//p_cLast = p_cStart;
			p_cLast = p_cEnd;
			break;
		}

		p_cStart = p_cEnd + sepl;
		if (p_cStart >= p_fEnd)	// look out for overrun...
			break;
	}

	if (M == -1)
	{
		if (iCount == nToks - 1)
			p_cFirst = p_cStart;

		return TString(p_cFirst);
	}
	if (iCount == nToks - 1)
		p_cLast = p_cEnd;
#ifndef NDEBUG
	if (p_cFirst > p_cLast)
		throw std::runtime_error("gettok(int,int,sepchars): start of string > end");
#endif
	return TString(p_cFirst, p_cLast);
}

/*!
* \brief Get the first token from the string, used at the start of a getnexttok() sequence
*
* params
* N		-	the token number to retreive
*				If Zero - returns an empty string.
*				If Negative gets adjusted to N + (total number of tokens + 1)
* sepChars	-	The token seperator string
*				If nullptr returns the whole current string.
*/
TString TString::getfirsttok(const UINT N, const_pointer_const sepChars) const
{
	this->m_savedtotaltoks = this->numtok(sepChars);
	this->m_savedpos = this->m_pString;
	this->m_savedcurrenttok = N;

	// N == 0 is used to pre load the vars for a loop of next toks where we need to start at 1

	//if ((sepChars == nullptr) || (N == m_savedtotaltoks))
	//	return *this;
	if (sepChars == nullptr)
		return *this;

	if ((N > this->m_savedtotaltoks) || (N == 0))
		return TString();

	const_pointer p_cStart = this->m_pString, p_cEnd = this->m_pString;
	const_pointer_const p_fEnd = last();
	auto iCount = decltype(N){0};
	const auto sepl = _ts_strlen(sepChars); // Ook

	//this->m_savedSepCharsLength = sepl;
	//this->m_tSavedSepChars = std::make_unique<TCHAR[]>(sepl);
	//ts_strcpy_throw(this->m_tSavedSepChars.get(), sepChars);

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr)
	{
		iCount++;

		if (iCount == N) {
			this->m_savedpos = p_cEnd + sepl;
			return TString(p_cStart, p_cEnd);
		}
		p_cStart = p_cEnd + sepl; // Ook
		if (p_cStart >= p_fEnd)
		{
			this->m_savedpos = nullptr;
			break;
		}
	}

	if (iCount == N - 1)
	{
		//this->m_savedpos = nullptr;	// this line causes TString++ operator to fail to get the last token correctly, instead it returns the whole string...
		return TString(p_cStart, p_fEnd);
	}

	return TString();
}

TString TString::getfirsttok(const size_type N, const_reference sepChar) const
{
	this->m_savedtotaltoks = this->numtok(sepChar);
	this->m_savedpos = this->m_pString;
	this->m_savedcurrenttok = N;

	// N == 0 is used to pre load the vars for a loop of next toks where we need to start at 1

	if (sepChar == value_type())
		return *this;

	if ((N > this->m_savedtotaltoks) || (N == 0))
		return TString();

	const_pointer p_cStart = this->m_pString, p_cEnd = this->m_pString;
	const_pointer_const p_fEnd = last();
	auto iCount = decltype(N){0};

	while ((p_cEnd = ts_strchr(p_cStart, sepChar)) != nullptr)
	{
		iCount++;

		if (iCount == N)
		{
			this->m_savedpos = p_cEnd + 1U;
			return TString(p_cStart, p_cEnd);
		}
		p_cStart = p_cEnd + 1U;
		if (p_cStart >= p_fEnd)
		{
			this->m_savedpos = nullptr;
			break;
		}
	}

	if (iCount == N - 1)
		return TString(p_cStart, p_fEnd);

	return TString();
}

/*!
* \brief Get the next token from the string (can only be used after a getfirsttok() call)
*
* params
* sepChars	-	The token seperator string
*				If nullptr returns the whole current string.
*/
TString TString::getnexttok(const_pointer_const sepChars) const
{
	if (sepChars == nullptr || this->m_pString == nullptr)
		return *this;

	this->m_savedcurrenttok++;
	const auto* const p_cStart = this->m_savedpos;

	if ((this->m_savedcurrenttok > this->m_savedtotaltoks) || (p_cStart == nullptr))
		return TString();

	if (this->m_savedcurrenttok == this->m_savedtotaltoks)
	{
		this->m_savedpos = nullptr;
		return TString(p_cStart);
	}
	else {
		const auto* const p_cEnd = ts_strstr(p_cStart, sepChars);
		if (p_cEnd != nullptr)
		{
			this->m_savedpos = (p_cEnd + _ts_strlen(sepChars));
			return TString(p_cStart, p_cEnd);
		}
	}
	return TString();
}

TString TString::getnexttok(const_reference sepChars) const
{
	if (sepChars == value_type() || this->m_pString == nullptr)
		return *this;

	this->m_savedcurrenttok++;
	const_pointer_const p_cStart = this->m_savedpos;

	if ((this->m_savedcurrenttok > this->m_savedtotaltoks) || (p_cStart == nullptr))
		return TString();

	if (this->m_savedcurrenttok == this->m_savedtotaltoks)
	{
		this->m_savedpos = nullptr;
		return TString(p_cStart);
	}
	else {
		const_pointer_const p_cEnd = ts_strchr(p_cStart, sepChars);
		if (p_cEnd != nullptr)
		{
			this->m_savedpos = (p_cEnd + 1U);
			return TString(p_cStart, p_cEnd);
		}
	}
	return TString();
}
#endif

/*!
* \brief Get the last tokens from the string (can only be called after a getfirsttok() call)
*
* params
*/
TString TString::getlasttoks() const
{
	if (empty())
		return TString();

	// set current token to the last one.
	m_savedcurrenttok = m_savedtotaltoks;
	// get saved position (this can be nullptr)
	const_pointer_const p_cStart = m_savedpos;
	// set saved position to nullptr
	m_savedpos = nullptr;
	// return end of string (can be nullptr as constructor handles this)
	return TString(p_cStart);
}

#if !TSTRING_TESTCODE
/*!
 * \brief Returns the number of tokens in the string
 *
 * params
 *	sepChars = The token seperator.
 */
size_t TString::numtok(const_pointer_const sepChars) const noexcept
{
	if (sepChars == nullptr || this->empty())
		return 0U;

	//if (this->m_savedtotaltoks > 0)
	//	return this->m_savedtotaltoks;

	const_pointer p_cStart = this->m_pString, p_cEnd = nullptr;
	auto iCount = 0U;
	const auto sepl = _ts_strlen(sepChars); // Ook

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr)
	{
		iCount++;
		p_cStart = p_cEnd + sepl; // Ook
	}
	//this->m_savedtotaltoks = iCount + 1;
	//return this->m_savedtotaltoks;
	return iCount + 1U;
}

/*!
* \brief Returns the number of tokens in the string
*
* params
*	sepChars = The token seperator.
*/
size_t TString::numtok(const_reference sepChar) const noexcept
{
	if (empty())
		return size_t();

	const_pointer p_cStart = this->m_pString, p_cEnd = nullptr;
	auto iCount = 0U;

	while ((p_cEnd = ts_strchr(p_cStart, sepChar)) != nullptr)
	{
		iCount++;
		p_cStart = ++p_cEnd;
	}
	return iCount + 1U;
}

/*!
* \brief blah
*
* blah
*/

void TString::deltok(const UINT N, const_pointer_const sepChars)
{
	if (sepChars == nullptr || N < 1 || this->empty())
		return;

	const auto nToks = this->numtok(sepChars);

	if (N > nToks)
		return;

	if (N == 1 && nToks == 1)
	{
		this->clear();
		return;
	}

	pointer p_cStart = this->m_pString, p_cEnd = nullptr;
	auto i = decltype(N){0};
	const auto sepl = _ts_strlen(sepChars); // Ook

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr)
	{
		i++;

		if (i == N)
			break;

		p_cStart = p_cEnd + sepl; // Ook
	}

	const auto l = this->len();

	TString tmp(l);

	// last token
	if (p_cEnd == nullptr)
	{
		p_cStart--;
		*p_cStart = 0;
		p_cEnd = this->m_pString;
	}
	// delete the first token
	else if (p_cStart == this->m_pString)
		p_cEnd++;
	// inbound token
	else {
		*p_cStart = 0;
		p_cEnd++;

		ts_strncat_throw(tmp.m_pString, this->m_pString, l);
	}
	ts_strncat_throw(tmp.m_pString, p_cEnd, l);

	this->swap(tmp);
}

/*!
* \brief blah
*
* blah
*/

TString TString::matchtok(const_pointer_const mString, UINT N, const_pointer_const sepChars) const
{
	//size_t count = 0;
	//for (auto itStart = begin(sepChars), itEnd = end(); itStart != itEnd; ++itStart)
	//{
	//	const TString tmp((*itStart));
	//	if (ts_strstr(tmp.to_chr(), mString) != nullptr)
	//	{
	//		++count;
	//		if (count >= N)
	//			return tmp;
	//	}
	//}
	//return TString();

	size_t count = 0U;
	auto itEnd = end();
	auto itGot = std::find_if(begin(sepChars), itEnd, [&count, &N, &mString](const auto& x) {
		if (ts_strstr(x.to_chr(), mString) != nullptr)
		{
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
#endif

/*!
* \brief blah
*
* blah
*/

//The default is an alphabetic sort, however you can specify n = numeric sort, c = channel nick prefix sort, r = reverse sort, a = alphanumeric sort.
void TString::sorttok(const_pointer_const sortOptions, const_pointer_const sepChars)
{
	SortOptions opts;

	for (auto p = sortOptions; *p; ++p)
	{
		switch (*p)
		{
		case L'a':
		{
			opts.bAlpha = true;
			opts.bNumeric = true;
			break;
		}
		case L'n':
		{
			opts.bNumeric = true;
			break;
		}
		case L'c':
		{
			opts.bPrefix = true;
			break;
		}
		case L'r':
		{
			opts.bReverse = true;
			break;
		}
		default:
			break;
		}
	}
	if (!opts.bAlpha && !opts.bNumeric && !opts.bPrefix)
		opts.bAlpha = true;

	sorttok(opts, sepChars);
}

/*!
* \brief blah
*
* blah
*/

void TString::sorttok(const SortOptions& sortOptions, const_pointer_const sepChars)
{
	// copy string into a vector, each vector entry is a token
	std::vector<TString> v(begin(sepChars), end());

	if (sortOptions.bAlpha)
	{
		// alpha or alphanumeric sort
		if (sortOptions.bNumeric)
		{
			// alphanumeric sort (needs work)
			if (sortOptions.bReverse)
				std::sort(v.begin(), v.end(),
					[](auto& a, auto& b) {
						if (!_istdigit(a[0]) || !_istdigit(b[0]))
							return (a > b);
						return (a.to_int() > b.to_int());
					});
			else
				std::sort(v.begin(), v.end(),
					[](auto& a, auto& b) {
						if (!_istdigit(a[0]) || !_istdigit(b[0]))
							return (a < b);
						return (a.to_int() < b.to_int());
					});
		}
		else {
			// alpha sort
			if (sortOptions.bReverse)
				std::sort(v.begin(), v.end(), std::greater<TString>());
			else
				std::sort(v.begin(), v.end(), std::less<TString>());
			//if (sortOptions.bReverse)
			//	std::sort(v.begin(), v.end(), [](auto &a, auto &b) { return (a > b); });
			//else
			//	std::sort(v.begin(), v.end(), [](auto &a, auto &b) { return (a < b); });
		}
	}
	else if (sortOptions.bNumeric)
	{
		// numeric sort
		if (sortOptions.bReverse)
			std::sort(v.begin(), v.end(), [](auto& a, auto& b) { return (a.to_int() > b.to_int()); });
		else
			std::sort(v.begin(), v.end(), [](auto& a, auto& b) { return (a.to_int() < b.to_int()); });
	}
	else if (sortOptions.bPrefix)
	{
		// prefix sort
		if (sortOptions.bReverse)
			std::sort(v.begin(), v.end(), [](auto& a, auto& b) { return (a[0] > b[0]); });
		else
			std::sort(v.begin(), v.end(), [](auto& a, auto& b) { return (a[0] < b[0]); });
	}

	// clear old unsorted data.
	clear();

	// join all tokens into a new sorted list
	join(v, sepChars);
}

/*!
 * \brief blah
 *
 * blah
 */
TString& TString::trim()
{
	if (empty())
		return *this;

	const_pointer start = m_pString;
	auto end = m_pString + len();
	const auto* const oldEnd = end;

	// Trim from start
	while (start != end && *start == L' ')
		++start;

	// Trim from end
	while (end != start && *(--end) == L' ');

	// only alloc new string if string changed.

	++end;	// end must be increased by one to account for the previous decrement

	if (end != oldEnd)
		*end = value_type{};	// place zero at new ending

	// only allocate new string if start of string modified.
	if (start != m_pString)
	{
		const auto new_len = (end - start) + 1U;

		TString tmp(gsl::narrow_cast<UINT>(new_len));

		ts_strcpyn_throw(tmp.m_pString, start, new_len);

		this->swap(tmp);
	}
	return *this;
}

#if INCLUDE_MIRC_EXTRAS
/*
* isnum(option)
*   returns TRUE if string is a valid number, if option is true,
*   it'll take negative numbers as valid numbers
*   returns FALSE in an invalid string
*/
bool TString::isnum(const bool bAllowNegatives) const noexcept
{
	auto* p = this->m_pString;
	auto c = 0;
	if (*p == TEXT('-'))
	{
		if (!bAllowNegatives)
			return false;
		++p;
	}
	if (!bAllowNegatives && *p == TEXT('-'))
		return false;
	while (*p)
	{
		if (*p < TEXT('0') || *p > TEXT('9'))
			return false;
		++c;
		++p;
	}
	return (c > 0);
}

/*
* isincs(letter)
*   returns TRUE if letter is in word (case sensitive)
*   otherwise, a FALSE is returned
*/
bool TString::isincs(const_value_type let) const noexcept
{
	//auto tmp = this->m_pString;
	//while (*tmp) {
	//	if (*tmp++ == let)
	//		return true;
	//}
	//return false;

	for (auto tmp = this->m_pString; tmp && *tmp; ++tmp)
	{
		if (*tmp == let)
			return true;
	}
	return false;
}

/*
* countchar(character)
*/
UINT TString::countchar(const_value_type chr) const noexcept
{
	UINT r = 0;

	for (auto aux = this->m_pString; aux && *aux; ++aux)
	{
		if (*aux == chr)
			++r;
	}
	return r;
}

/*
* ishostmask(mask)
*/
bool TString::ishostmask(void) const noexcept
{
	auto /*nick = false,*/ s1 = false, /*ident = false,*/ s2 = false, host = false;
	auto c = this->m_pString;
	while (*c)
	{
		if (*c == ' ')
			return false;
		else if (*c == TEXT('!'))
		{
			if (s1)
				return false;
			s1 = true;
		}
		else if (*c == TEXT('@'))
		{
			if (!s1 || s2)
				return false;
			s2 = true;
		}
		else if (s1 && s2)
		{
			if (!host && *c == TEXT('.'))
				return false;
			host = true;
		}
		//else if (s1)
		//	ident = true;
		//else
		//	nick = true;
		++c;
	}
	if (host)
	{
		if (*--c == TEXT('.'))
			return false;
		return true;
	}
	return false;
}

TString TString::toupper(void) const
{
	auto tmp(*this);
	UINT c = 0;
	const auto l = tmp.len();
	for (auto p = tmp.to_chr(); c < l; c++)
		p[c] = ts_toupper(p[c]);

	return tmp;
}
TString TString::tolower(void) const
{
	auto tmp(*this);
	const auto l = tmp.len();
	auto c = decltype(l){0};
	for (auto* p = tmp.to_chr(); c < l; c++)
		p[c] = ts_tolower(p[c]);

	return tmp;
}
#endif

// pos must be within length of string, n can be < 0
TString TString::mid(const int pos, int n) const
{
	const auto l = gsl::narrow_cast<int>(this->len());
	if ((n == 0) || (l == 0) || (pos > l) || (pos < 0))
		return {};
	if (n < 0)
		n = l - pos + n;
	else if ((pos + n) > l)
		n = l - pos;
	if (n < 1)
		return {};
	++n;

	return TString(&this->m_pString[pos], &this->m_pString[pos + n]);
}

// if n > string length its truncated, n can be < 0
TString TString::left(int n) const
{
	return this->mid(0, n);
}

// if n > string length its truncated, n can be < 0
TString TString::right(int n) const
{
	const auto l = gsl::narrow_cast<int>(this->len());
	if ((n == 0) || (l == 0))
		return {};
	if (n > l)
		n = l;
	auto start = l - n, len = n + 1;
	if (n < 0)
	{
		start = abs(n);
		if (start > l)
			return {};
		len = (l - start) + 1;
	}

	return TString(&this->m_pString[start], &this->m_pString[start + len]);
}

// Ook - match() function taken from aircdll.dll by Tabo source.
/* taken from the hybrid-5.3p7 source */
inline int TString::rfc_tolower(const int c) noexcept
{
	return ::tolower(c);
}

inline int TString::rfc_toupper(const int c) noexcept
{
	return ::toupper(c);
}

#if !TSTRING_TESTCODE
/*
 * The next function is based on eggdrop's wild_match (by robey pointer)
 * I added an option to perform a case sensitive match
 */

#define QUOTE TEXT('\\')
#define WILDS TEXT('*')
#define WILDP TEXT('%')
#define WILDQ TEXT('?')
 /* #define WILDT TEXT('~') */

#define MATCH (match+saved+sofar)
#define NOMATCH 0

/*
 * EGGDROP:   wild_match_per(TCHAR *m, TCHAR *n)
 * IrcII:     wild_match(TCHAR *m, TCHAR *n)
 *
 * Features:  Forward, case-insensitive, ?, *, %, ~(optional)
 * Best use:  Generic string matching, such as in IrcII-esque bindings
 */
UINT TString::match(const_pointer m, const_pointer n, const bool cs /* case sensitive */)
{
	const_pointer ma = m, lsm = nullptr, lsn = nullptr, lpm = nullptr, lpn = nullptr;
	UINT match = 1, saved = 0;
	UINT sofar = 0;

#ifdef WILDT
	int space;
#endif

	/* take care of null strings (should never match) */
	if ((m == nullptr) || (n == nullptr) || (!*n))
		return NOMATCH;
	/* (!*m) test used to be here, too, but I got rid of it.  After all,
	 * If (!*n) was false, there must be a character in the name (the
	 * second string), so if the mask is empty it is a non-match.  Since
	 * the algorithm handles this correctly without testing for it here
	 * and this shouldn't be called with null masks anyway, it should be
	 * a bit faster this way */

	while (*n)
	{
		/* Used to test for (!*m) here, but this scheme seems to work better */
#ifdef WILDT
		if (*m == WILDT)
		{			/* Match >=1 space */
			space = 0;				/* Don't need any spaces */
			do {
				++m;
				++space;
			}							/* Tally 1 more space ... */
			while ((*m == WILDT) || (*m == TEXT(' ')));	/*  for each space or ~ */
			sofar += space;				/* Each counts as exact */
			while (*n == TEXT(' '))
			{
				++n;
				space--;
			}							/* Do we have enough? */
			if (space <= 0)
				continue;				/* Had enough spaces! */
		}
		/* Do the fallback       */
		else {
#endif
			switch (*m)
			{
			case 0:
				do
					m--;			/* Search backwards */
				while ((m > ma) && (*m == TEXT('?')));	/* For first non-? TCHAR */
				if ((m > ma) ? ((*m == TEXT('*')) && (m[-1] != QUOTE)) : (*m == TEXT('*')))
					return MATCH;	/* nonquoted * = match */
				break;
			case WILDP:
				while (*(++m) == WILDP);	/* Zap redundant %s */
				if (*m != WILDS)
				{		/* Don't both if next=* */
					if (*n != TEXT(' '))
					{	/* WILDS can't match ' ' */
						lpm = m;
						lpn = n;		/* Save '%' fallback spot */
						saved += sofar;
						sofar = 0;		/* And save tally count */
					}
					continue;		/* Done with '%' */
				}
				/* FALL THROUGH */
			case WILDS:
				do
					++m;			/* Zap redundant wilds */
				while ((*m == WILDS) || (*m == WILDP));
				lsm = m;
				lsn = n;
				lpm = 0;		/* Save '*' fallback spot */
				match += (saved + sofar);	/* Save tally count */
				saved = sofar = 0;
				continue;		/* Done with '*' */
			case WILDQ:
				++m;
				++n;
				continue;		/* Match one TCHAR */
			case QUOTE:
				++m;			/* Handle quoting */
			}
			if (cs)
			{ /* case sensitive */
				if (*m == *n)
				{		/* If matching */
					++m;
					++n;
					++sofar;
					continue;		/* Tally the match */
				}
			}
			else if (rfc_toupper(*m) == rfc_toupper(*n))
			{		/* If matching */
				++m;
				++n;
				++sofar;
				continue;		/* Tally the match */
			}
#ifdef WILDT
		}
#endif
		if (lpm)
		{			/* Try to fallback on '%' */
			n = ++lpn;
			m = lpm;
			sofar = 0;		/* Restore position */
			if ((*n | 32) == 32)
				lpm = 0;		/* CanTEXT('t match 0 or ') ' */
			continue;			/* Next TCHAR, please */
		}
		if (lsm)
		{			/* Try to fallback on '*' */
			n = ++lsn;
			m = lsm;			/* Restore position */
			/* Used to test for (!*n) here but it wasn't necessary so it's gone */
			saved = sofar = 0;
			continue;			/* Next TCHAR, please */
		}
		return NOMATCH;		/* No fallbacks=No match */
	}
	while ((*m == WILDS) || (*m == WILDP))
		++m;			/* Zap leftover %s & *s */
	return (*m) ? NOMATCH : MATCH;	/* End of both = match */
}

TString TString::wildtok(const_pointer_const wildString, const UINT N, const_pointer_const sepChars) const
{
	if (!sepChars || !wildString || N == 0 || this->empty())
		return {};

	if (N > this->numtok(sepChars))
		return {};

	auto m = 0U;
	if (auto itEnd = end(), itGot = std::find_if(begin(sepChars), itEnd, [&wildString, &m, &N](const TString& x) {
		if (match(wildString, x.to_chr(), false) != NOMATCH)
		{
			++m;
			if (m == N)
				return true;
		}
		return false;
		}); itGot != itEnd)
		return *itGot;

		return {};
}

UINT TString::nwildtok(const_pointer_const wildString, const_pointer_const sepChars) const
{
	if (!sepChars || !wildString || this->empty())
		return 0;

	auto m = 0U;
	std::for_each(begin(sepChars), end(), [&wildString, &m](const TString& x) {
		if (match(wildString, x.to_chr(), false) != NOMATCH)
			++m;
		});
	return m;
}
#else
TString TString::wildtok(const_pointer_const wildString, const UINT N, const_pointer_const sepChars) const
{
	if (!sepChars || !wildString || N == 0 || this->empty())
		return TString();

	if (N > this->numtok(sepChars))
		return TString();

	auto m = 0U;
	if (const auto itEnd = end(), itGot = std::find_if(begin(sepChars), itEnd, [&wildString, &m, &N](const TString& x) noexcept
		{
			if (_ts_WildcardMatch(x, wildString, false))
			{
				++m;
				if (m == N)
					return true;
			}
			return false;
		}); itGot != itEnd)
		return *itGot;

		return TString();
}

UINT TString::nwildtok(const_pointer_const wildString, const_pointer_const sepChars) const
{
	if (!sepChars || !wildString || this->empty())
		return 0;

	auto m = 0U;
	std::for_each(begin(sepChars), end(), [&wildString, &m](const TString& x) noexcept {
		if (_ts_WildcardMatch(x, wildString, false))
			++m;
		});
	return m;
}
#endif

GSL_SUPPRESS(es.47)
GSL_SUPPRESS(type.3)
int TString::tsprintf(_In_z_ _Printf_format_string_ const_pointer_const fmt, ...)
{
	va_list args = nullptr;
	va_start(args, fmt);
	const auto cnt = tvprintf(fmt, args);
	va_end(args);
	return cnt;
}

int TString::tvprintf(_In_z_ _Printf_format_string_ const_pointer_const fmt, _In_ va_list args)
{
	const auto cnt = ts_vscprintf(fmt, args);
	if (cnt > 0)
	{
		TString tmp(gsl::narrow_cast<UINT>((cnt + 2)));

		// warning C4996: 'vsprintf' was declared deprecated
		// http://forums.microsoft.com/MSDN/ShowPost.aspx?PostID=10254&SiteID=1
		ts_vsprintf(tmp.m_pString, gsl::narrow_cast<size_t>(cnt) + 1U, fmt, args);

		this->swap(tmp);
	}
	return cnt;
}

#if !TSTRING_TESTCODE
/*
 * iswm(*mask*)
 *    returns TRUE if *mask* matches string
 *    else it returns FALSE
 *    case insensitive
 */
bool TString::iswm(const_pointer_const a) const noexcept
{
	return (match(a, this->m_pString, false) != NOMATCH);
	//return _ts_WildcardMatch(m_pString, a);
}

/*
 * iswmcs(*mask*)
 *    returns TRUE if *mask* matches string
 *    else it returns FALSE
 *    case sensitive
 */
bool TString::iswmcs(const_pointer_const a) const noexcept
{
	return (match(a, this->m_pString, true) != NOMATCH);
	//return _ts_WildcardMatch(m_pString, a);
}
#endif

//int wildcmp(const TCHAR *wild, const TCHAR *string) {
//	// Written by Jack Handy - jakkhandy@hotmail.com
//	const TCHAR *cp = nullptr, *mp = nullptr;
//
//	while ((*string) && (*wild != TEXT('*'))) {
//		if ((*wild != *string) && (*wild != TEXT('?'))) {
//			return 0;
//		}
//		wild++;
//		string++;
//	}
//
//	while (*string) {
//		if (*wild == TEXT('*')) {
//			if (!*++wild) {
//				return 1;
//			}
//			mp = wild;
//			cp = string+1;
//		} else if ((*wild == *string) || (*wild == TEXT('?'))) {
//			wild++;
//			string++;
//		} else {
//			wild = mp;
//			string = cp++;
//		}
//	}
//
//	while (*wild == TEXT('*')) {
//		wild++;
//	}
//	return !*wild;
//}

/*!
* \brief blah
*
* Convert a cstring/utf8 string to a utf16 string
*/
// this version works fine but uses to many allocation via new/delete
//WCHAR *TString::charToWchar(const char *const cString, size_t *const buffer_size)
//{
//	// try UTF8 encoded first, but error on invalid chars.
//	WCHAR *res = nullptr;
//	WCHAR *normRes = nullptr;
//	auto buf_size = std::remove_pointer_t < decltype(buffer_size) > {0};
//	if (cString != nullptr) {
//		try {
//			auto widelen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, cString, -1, nullptr, 0);
//			if (widelen == 0) {
//				// zero result, error maybe?
//				if (GetLastError() == ERROR_NO_UNICODE_TRANSLATION) {
//					// invalid chars, assume its NOT a utf8 string then, try ascii->utf16
//					widelen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cString, -1, nullptr, 0);
//					if (widelen != 0) {
//						buf_size = TS_wgetmemsize(widelen + 1);
//						res = new WCHAR[buf_size];	// TS_getmemsize() gives a much larger buffer than we really want, but it will do.
//						if (MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cString, -1, res, widelen) == 0)
//							res[0] = L'\0';
//					}
//				}
//				else {
//					// if no error, assume zero length string.
//					buf_size = TS_wgetmemsize(1);
//					res = new WCHAR[buf_size];
//					res[0] = L'\0';
//				}
//			}
//			else {
//				buf_size = TS_wgetmemsize(widelen + 1);
//				res = new WCHAR[buf_size];
//				if (MultiByteToWideChar(CP_UTF8, 0, cString, -1, res, widelen) == 0)
//					res[0] = L'\0';
//			}
//			// NB: NormalizeString() is Vista+ ONLY
//			auto normLen = NormalizeString(NormalizationC, res, -1, nullptr, 0);
//			if (normLen > 0) {
//				normLen = TS_wgetmemsize(normLen + 1);
//				normRes = new WCHAR[normLen];
//				if (NormalizeString(NormalizationC, res, -1, normRes, normLen) > 0)
//				{
//					delete[] res;
//					res = normRes;
//				}
//				else
//					delete[] normRes;
//
//				normRes = nullptr;
//			}
//		}
//		catch (const std::bad_alloc)
//		{
//			delete[] res;
//			delete[] normRes;
//			res = nullptr;
//		}
//	}
//	if (buffer_size != nullptr)
//		*buffer_size = (buf_size *sizeof(WCHAR));
//
//	return res;
//}

/*!
* \brief blah
*
* Convert a cstring/utf8 string to a utf16 string
* New version using unique_ptr instead of new/delete
*/
//WCHAR* TString::charToWchar(const char* const cString, size_t* const buffer_size)
//{
//	// try UTF8 encoded first, but error on invalid chars.
//	std::unique_ptr<WCHAR[]> res;
//
//	auto buf_size = std::remove_pointer_t < decltype(buffer_size) >{ 0 };
//	if (cString)
//	{
//		if (auto widelen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, cString, -1, nullptr, 0); widelen == 0)
//		{
//			// zero result, error maybe?
//			if (GetLastError() == ERROR_NO_UNICODE_TRANSLATION)
//			{
//				// invalid chars, assume its NOT a utf8 string then, try ascii->utf16
//				widelen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cString, -1, nullptr, 0);
//				if (widelen != 0)
//				{
//					buf_size = TS_wgetmemsize(widelen + 1);
//					res = std::make_unique<WCHAR[]>(buf_size);	// TS_getmemsize() gives a much larger buffer than we really want, but it will do.
//					if (MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cString, -1, res.get(), widelen) == 0)
//						gsl::at(res, 0) = L'\0';
//				}
//			}
//			else {
//				// if no error, assume zero length string.
//				buf_size = TS_wgetmemsize(1);
//				res = std::make_unique<WCHAR[]>(buf_size);
//				gsl::at(res, 0) = L'\0';
//			}
//		}
//		else {
//			buf_size = TS_wgetmemsize(widelen + 1);
//			res = std::make_unique<WCHAR[]>(buf_size);
//			if (MultiByteToWideChar(CP_UTF8, 0, cString, -1, res.get(), widelen) == 0)
//				gsl::at(res, 0) = L'\0';
//		}
//		// NB: NormalizeString() is Vista+ ONLY
//		if (const auto normLen = NormalizeString(NormalizationC, res.get(), -1, nullptr, 0); normLen > 0)
//		{
//			const auto uNewNormLen = TS_wgetmemsize(normLen + 1);
//			if (auto normRes = std::make_unique<WCHAR[]>(uNewNormLen); NormalizeString(NormalizationC, res.get(), -1, normRes.get(), gsl::narrow_cast<int>(uNewNormLen)) > 0)
//				res = std::move(normRes);
//		}
//	}
//	if (buffer_size)
//		*buffer_size = (buf_size * sizeof(WCHAR));
//
//	return res.release();
//}

//void TString::Normalize()
//{
//	if (const auto normLen = NormalizeString(NormalizationC, m_pString, -1, nullptr, 0); normLen > 0)
//	{
//		const auto uNewNormLen = TS_wgetmemsize(normLen + 1);
//		if (auto normRes = std::make_unique<WCHAR[]>(uNewNormLen); NormalizeString(NormalizationC, m_pString, -1, normRes.get(), static_cast<int>(uNewNormLen)) > 0)
//			*this = normRes.get();
//	}
//}

//char* TString::WcharTochar(const WCHAR* const wString, size_t* const buffer_size)
//{
//	std::unique_ptr<char[]> res = nullptr;
//	auto buf_size = std::remove_pointer_t < decltype(buffer_size) >{ 0 };
//
//	if (const auto l = WideCharToMultiByte(CP_UTF8, 0, wString, -1, nullptr, 0, nullptr, nullptr); l > 0)
//	{
//		buf_size = TS_getmemsize(l + 1);
//		res = std::make_unique<char[]>(buf_size);
//		if (WideCharToMultiByte(CP_UTF8, 0, wString, -1, res.get(), l, nullptr, nullptr) == 0)
//			gsl::at(res, 0) = '\0';
//	}
//	if (buffer_size != nullptr)
//		*buffer_size = buf_size;
//
//	return res.release();
//}

std::unique_ptr<WCHAR[]> TString::charToWchar(const char* const cString, size_t* const buffer_size)
{
	// try UTF8 encoded first, but error on invalid chars.
	std::unique_ptr<WCHAR[]> res;

	auto buf_size = std::remove_pointer_t < decltype(buffer_size) >{ 0 };
	if (cString)
	{
		if (auto widelen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, cString, -1, nullptr, 0); widelen == 0)
		{
			// zero result, error maybe?
			if (GetLastError() == ERROR_NO_UNICODE_TRANSLATION)
			{
				// invalid chars, assume its NOT a utf8 string then, try ascii->utf16
				widelen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cString, -1, nullptr, 0);
				if (widelen != 0)
				{
					buf_size = TS_wgetmemsize(widelen + 1);
					res = std::make_unique<WCHAR[]>(buf_size);	// TS_getmemsize() gives a much larger buffer than we really want, but it will do.
					if (MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cString, -1, res.get(), widelen) == 0)
						gsl::at(res, 0) = L'\0';
				}
			}
			else {
				// if no error, assume zero length string.
				buf_size = TS_wgetmemsize(1);
				res = std::make_unique<WCHAR[]>(buf_size);
				gsl::at(res, 0) = L'\0';
			}
		}
		else {
			buf_size = TS_wgetmemsize(widelen + 1);
			res = std::make_unique<WCHAR[]>(buf_size);
			if (MultiByteToWideChar(CP_UTF8, 0, cString, -1, res.get(), widelen) == 0)
				gsl::at(res, 0) = L'\0';
		}
		// NB: NormalizeString() is Vista+ ONLY
		if (const auto normLen = NormalizeString(NormalizationC, res.get(), -1, nullptr, 0); normLen > 0)
		{
			const auto uNewNormLen = TS_wgetmemsize(normLen + 1);
			if (auto normRes = std::make_unique<WCHAR[]>(uNewNormLen); NormalizeString(NormalizationC, res.get(), -1, normRes.get(), gsl::narrow_cast<int>(uNewNormLen)) > 0)
				res = std::move(normRes);
		}
	}
	if (buffer_size)
		*buffer_size = (buf_size * sizeof(WCHAR));

	return res;
}
std::unique_ptr<char[]> TString::WcharTochar(const WCHAR* const wString)
{
	std::unique_ptr<char[]> res = nullptr;

	if (const auto l = WideCharToMultiByte(CP_UTF8, 0, wString, -1, nullptr, 0, nullptr, nullptr); l > 0)
	{
		const auto buf_size = TS_getmemsize(l + 1);
		res = std::make_unique<char[]>(buf_size);
		if (WideCharToMultiByte(CP_UTF8, 0, wString, -1, res.get(), l, nullptr, nullptr) == 0)
			gsl::at(res, 0) = '\0';
	}

	return res;
}

/*!
 * \brief blah
 *
 * blah
 */
TString& TString::strip()
{
	if (this->empty())
		return *this;

	const auto* start = this->m_pString;
	const auto* end = this->m_pString + this->len();

	// Trim from start
	while (start != end && *start == L' ')
		++start;

	// Trim from end
	while (end != start && *(--end) == L' ');

	const size_t new_len = gsl::narrow_cast<size_t>((end - start) + 1);	// add one to take into account the previous decrement

	TString tmp(gsl::narrow_cast<UINT>(new_len + 1));				// add one for zero byte.

	// now strip all ctrl codes.
	const auto* const wtxt = start;
	auto* p = tmp.m_pString;
	auto pos = decltype(new_len){0};

	// strip out ctrl codes to correctly position text.
	for (auto c = wtxt[pos]; pos < new_len; c = wtxt[++pos])
	{
		switch (c)
		{
		case 2:  // ctrl-b Bold
		case 15: // ctrl-o
		case 22: // ctrl-r Reverse
		case 29: // ctrl-i Italics
		case 31: // ctrl-u Underline
			break;
		case 3: // ctrl-k Colour
		{
			while (wtxt[pos + 1] == 3)
				++pos; // remove multiple consecutive ctrl-k's

			if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
			{
				++pos;

				if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
					++pos;

				// maybe a background color
				if (wtxt[pos + 1] == L',')
				{
					++pos;

					if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
					{
						++pos;

						if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
							++pos;
					}
				}
			}
		}
		break;
		case 32:	// space character
		{
			while (wtxt[pos + 1] == 32)
				++pos; // remove multiple consecutive spaces
			// fall through to save first space
		}
		[[fallthrough]];
		default:
			*p++ = c;
			break;
		}
	}

	this->swap(tmp);

	GSL_SUPPRESS(lifetime.4) return *this;
}

// swap function taken from: http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
void TString::swap(TString& second) noexcept
{
	// enable ADL (not necessary in our case, but good practice)
	using std::swap;

	// NB: 'second' is expected to be disposed of after the swap, not used, some mods are needed to make it fully usable when using internal buffers.

	// by swapping the members of two classes,
	// the two classes are effectively swapped

	swap(this->m_buffer, second.m_buffer);
	swap(this->m_pString, second.m_pString);
	swap(this->m_savedpos, second.m_savedpos);
	swap(this->m_pTempString, second.m_pTempString);
	swap(this->m_buffersize, second.m_buffersize);
	swap(this->m_savedcurrenttok, second.m_savedcurrenttok);
	swap(this->m_savedtotaltoks, second.m_savedtotaltoks);
	swap(this->m_bDirty, second.m_bDirty);
	swap(this->m_iLen, second.m_iLen);

	swap(this->m_bUsingInternal, second.m_bUsingInternal);
	m_InternalBuffer[0] = value_type{};

	if (this->m_bUsingInternal)
	{
		ts_copymem(&m_InternalBuffer[0], &second.m_InternalBuffer[0], TSTRING_INTERNALBUFFERSIZE_BYTES);

		this->m_pString = &m_InternalBuffer[0];
		// make savedpos point to the same offset within our buffer...
		if (this->m_savedpos)
			this->m_savedpos = (this->m_pString + (this->m_savedpos - &second.m_InternalBuffer[0]));
	}

	if (second.m_bUsingInternal)
	{
		second.m_pString = &second.m_InternalBuffer[0];
		//if (second.m_savedpos != nullptr)
		//	second.m_savedpos = (second.m_pString + (second.m_savedpos - this->m_InternalBuffer));
		second.m_savedpos = nullptr;
		second.m_iLen = 0;
		second.m_bDirty = false;
		second.m_InternalBuffer[0] = value_type{};
	}
}

void TString::copy(TString other) noexcept // <- copy made here, so just swap them
{
	this->swap(other);
}

int TString::compare(const TString& other) const noexcept
{
	return this->compare(other.data());
}

int TString::compare(const_reference other) const noexcept
{
	const_value_type tmp[] = { other, value_type{} };
	return this->compare(&tmp[0]);
}

int TString::compare(const_pointer_const other) const noexcept
{
	// compare to self or both nullptr, match!
	if (m_pString == other)
		return 0;

	// if we are nullptr other can't be, so no match
	if (!m_pString)
		return 1;

	// if other is nullptr, we are not, so no match
	if (!other)
		return -1;

	// check if strings match...
	return (ts_strcmp(m_pString, other));
}

int TString::compare(const_pointer_const other, const size_t iLength) const noexcept
{
	// compare to self or both nullptr, match!
	if (m_pString == other)
		return 0;

	// if we are nullptr other can't be, so no match
	if (!m_pString)
		return 1;

	// if other is nullptr, we are not, so no match
	if (!other)
		return -1;

	// check if strings match...
	return (ts_strncmp(this->m_pString, other, iLength));
}

ULONG TString::to_addr() const
{
	const BYTE first = gsl::narrow_cast<BYTE>(getfirsttok(1, L'.').to_int() & 0xFF);
	const BYTE second = gsl::narrow_cast<BYTE>(getnexttok(L'.').to_int() & 0xFF);
	const BYTE third = gsl::narrow_cast<BYTE>(getnexttok(L'.').to_int() & 0xFF);
	const BYTE forth = gsl::narrow_cast<BYTE>(getnexttok(L'.').to_int() & 0xFF);

	//return gsl::narrow_cast<ULONG>(MAKELONG(MAKEWORD(first,second),MAKEWORD(third,forth)));
	//return gsl::narrow_cast<ULONG>(MAKELONG(MAKEWORD(forth, third), MAKEWORD(second, first)));
	return gsl::narrow_cast<ULONG>(MAKEIPADDRESS(first, second, third, forth));
}

void TString::clear(void) noexcept
{
	TString tmp;
	this->swap(tmp);
}

void TString::shrink_to_fit(void)
{
	TString tmp(this->m_pString);
	this->swap(tmp);
}

void TString::reserve(const size_t tsSize)
{
	// check if buffer is already big enough
	if (tsSize <= (m_buffersize / sizeof(value_type)))
		return;

	TString tmp(tsSize);
	if ((this->m_pString) && (tmp.m_pString))
		ts_strcpy_throw(tmp.m_pString, this->m_pString);
	this->swap(tmp);
}

TString& TString::append(const WCHAR& chr)
{
	if (chr != L'\0')
	{
		const WCHAR tmp[] = { chr, L'\0' };
		append(&tmp[0], 1);
	}
	return *this;
}

GSL_SUPPRESS(lifetime.4)
TString& TString::append(const char& chr)
{
	const TString tmp(chr);
	return append(tmp);
}

TString& TString::append(const WCHAR* const cString)
{
	return append(cString, _ts_strlen(cString));
}

GSL_SUPPRESS(lifetime.4)
TString& TString::append(const char* const cString)
{
	const TString tmp(cString);
	return append(tmp);
}

TString& TString::append(const TString& tString)
{
	return append(tString.data(), tString.length());
}

TString& TString::append(const_pointer_const cString, const size_t iChars)
{
	if ((cString) && (iChars != 0))
	{
		const auto sz = this->len();
		decltype(sz) l = (sz + iChars + 1);	// plus 1 for zero byte.

		if ((l * sizeof(value_type)) <= m_buffersize)
		{
			// new text fits within existing buffer, so append.
			ts_strncat_throw(this->m_pString, cString, iChars);

			m_bDirty = true;

			DCX_DEBUG_ENSURES(m_pString[l - 1] == 0);
		}
		else {
			TString tmp(l);

			if (sz > 0)
			{
				ts_strcpyn_throw(tmp.m_pString, this->m_pString, l);
			}
			ts_strncat_throw(tmp.m_pString, cString, iChars);

			this->swap(tmp);

			DCX_DEBUG_ENSURES(m_pString[l - 1] == 0);
		}

		DCX_DEBUG_ENSURES(m_pString[l - 1] == 0);
	}
	return *this;
}

// literal operator
// allows "sometext"_ts to be interpreted as TString("sometext")
TString operator"" _ts(const char* p, size_t N)
{
	return TString{ p, N };
}
TString operator"" _ts(const WCHAR * p, size_t N)
{
	return TString{ p, N };
}
