/*! 
 * \file TString.cpp
 * \brief String and Token Management Class Functions
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
#include "defines.h"

//#include <windows.h>

#include "TString.h"

const TCHAR *TString::m_cSpace = TEXT(" ");
const TCHAR *TString::m_cComma = TEXT(",");
const TCHAR *TString::m_cTab = TEXT("\t");

/*! 
 * \brief Default Constructor
 */
/****************************/

//TString::TString( )
//: m_pTempString(NULL), m_pString(NULL)
//, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(0)
//{
//	this->m_pString = allocstr_cch(1);
//	this->m_pString[0] = 0;
//}

TString::TString()
	: TString((UINT)1)
{
}

/****************************/
/*! \fn TString::TString( const WCHAR * cString )
    \brief Constructor input from C WCHAR string

    \param cString Character string to initialize string to
*/
/****************************/

TString::TString(const char *const cString)
	: m_pTempString(nullptr), m_pString(nullptr)
	, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(nullptr)
	, m_bDirty(true), m_iLen(0)
#if TSTRING_INTERNALBUFFER
	, m_bUsingInternal(false)
#endif
{
#ifdef UNICODE
	if (cString != nullptr)
		this->m_pString = charToWchar(cString, &m_buffersize);
#else
	if (cString != nullptr) {
		const auto l = ts_strlen(cString) + 1;
		this->m_pString = allocstr_cch(l);
		ts_strcpyn_throw(this->m_pString, cString, l);
	}
#endif

	if (this->m_pString == nullptr) {
		this->m_pString = allocstr_cch(1);
		this->m_pString[0] = TEXT('\0');
	}
}

TString::TString( const WCHAR *const cString )
	: m_pTempString(nullptr), m_pString(nullptr)
	, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(nullptr)
	, m_bDirty(true), m_iLen(0)
#if TSTRING_INTERNALBUFFER
	, m_bUsingInternal(false)
#endif
{
#ifdef UNICODE
	if (cString != nullptr) {
		const auto l = ts_strlen(cString) + 1;
		this->m_pString = allocstr_cch(l);
		ts_strcpyn_throw(this->m_pString, cString, l);
	}
#else
	if (cString != nullptr)
		this->m_pString = WcharTochar(cString, &m_buffersize);
#endif

	if (this->m_pString == nullptr) {
		this->m_pString = allocstr_cch( 1 );
		this->m_pString[0] = TEXT('\0');
	}
}

/****************************/
/*! \fn TString::TString( const WCHAR chr )
    \brief Constructor input from C WCHAR

    \param chr Character to initialize string to
*/
/****************************/

#ifdef UNICODE
#if TSTRING_INTERNALBUFFER
TString::TString(const WCHAR chr)
	: m_pTempString(nullptr), m_pString(m_InternalBuffer)
	, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(nullptr)
	, m_bUsingInternal(true), m_bDirty(true), m_iLen(0), m_buffersize(TSTRING_INTERNALBUFFERSIZE_BYTES)
{
	this->m_pString[0] = chr;
	this->m_pString[1] = TEXT('\0');
}
#else
TString::TString(const WCHAR chr)
	: m_pTempString(nullptr), m_pString(nullptr)
	, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(nullptr)
	, m_bDirty(true), m_iLen(0)
{
	this->m_pString = this->allocstr_cch(2);
	this->m_pString[0] = chr;
	this->m_pString[1] = TEXT('\0');
}
#endif
#else
TString::TString( const WCHAR chr )
	: m_pTempString(nullptr), m_pString(nullptr)
	, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(nullptr)
	, m_bDirty(true), m_iLen(0)
#if TSTRING_INTERNALBUFFER
	, m_bUsingInternal(false)
#endif
{
	if (chr != L'\0')
	{
		const WCHAR wStr[] = { chr, L'\0' };

		this->m_pString = this->WcharTochar(wStr, &m_buffersize);
	}

	if (this->m_pString == nullptr)
	{
		this->m_pString = allocstr_cch(1);
		this->m_pString[0] = 0;
	}
}
#endif

/****************************/
/*! \fn TString::TString( const char chr )
\brief Constructor input from C char

\param chr Character to initialize string to
*/
/****************************/

#ifdef UNICODE
TString::TString(const char chr)
	: m_pTempString(nullptr), m_pString(nullptr)
	, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(nullptr)
	, m_bDirty(true), m_iLen(0)
#if TSTRING_INTERNALBUFFER
	, m_bUsingInternal(false)
#endif
{
	if (chr != '\0') {
		const char cString[] = { chr, '\0' };

		this->m_pString = charToWchar(cString, &m_buffersize);
	}

	if (this->m_pString == nullptr) {
		this->m_pString = allocstr_cch(1);
		this->m_pString[0] = TEXT('\0');
	}
}
#else
#if TSTRING_INTERNALBUFFER
TString::TString( const char chr )
	: m_pTempString(nullptr), m_pString(m_InternalBuffer)
	, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(nullptr)
	, m_bUsingInternal(true), m_bDirty(true), m_iLen(0), m_buffersize(TSTRING_INTERNALBUFFERSIZE_BYTES)
{
	this->m_pString[0] = chr;
	this->m_pString[1] = 0;
}
#else
TString::TString( const char chr )
	: m_pTempString(nullptr), m_pString(nullptr)
	, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(nullptr)
	, m_bDirty(true), m_iLen(0)
{
	this->m_pString = allocstr_cch(2);
	this->m_pString[0] = chr;
	this->m_pString[1] = 0;
}
#endif
#endif

/****************************/
/*! \fn TString::TString( const TString & tString )
    \brief Copy Constructor

    \param tString TString object to copy
*/
/****************************/

#if TSTRING_INTERNALBUFFER
TString::TString(const TString & tString)
	: m_pTempString(nullptr), m_pString(m_InternalBuffer)
	, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(nullptr)
	, m_bDirty(true), m_iLen(0), m_buffersize(TSTRING_INTERNALBUFFERSIZE_BYTES)
	, m_bUsingInternal(true)
{
	this->m_InternalBuffer[0] = 0;

	if (!tString.empty()) {
		// copy full buffer from source TString object, this buffer supplies any zero byte.
		this->m_pString = allocstr_bytes(tString.m_buffersize);
		ts_copymem(this->m_pString, tString.m_pString, tString.m_buffersize);
	}
}
#else
TString::TString(const TString & tString)
	: m_pTempString(nullptr), m_pString(nullptr)
	, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(nullptr)
	, m_bDirty(true), m_iLen(0)
{
	if (!tString.empty()) {
		// copy full buffer from source TString object, this buffer supplies any zero byte.
		this->m_pString = allocstr_bytes(tString.m_buffersize);
		ts_copymem(this->m_pString, tString.m_pString, tString.m_buffersize);
	}
	else {
		this->m_pString = allocstr_cch(1);
		this->m_pString[0] = 0;
	}
}
#endif

/****************************/
/*! \fn TString::TString( const TString && tString )
\brief Move Constructor

\param tString TString object to copy
*/
/****************************/
TString::TString(TString && tString)
	: TString()
{
	swap(tString);
}

//TString::TString(TString && tString)
//: m_buffersize(tString.m_buffersize)
//, m_pString(tString.m_pString)
//, m_pTempString(tString.m_pTempString)
//, m_savedcurrenttok(tString.m_savedcurrenttok)
//, m_savedpos(tString.m_savedpos)
//, m_savedtotaltoks(tString.m_savedtotaltoks)
//{
//#ifdef TSTRING_PARTS
//	m_SplitParts = std::move(tString.m_SplitParts);
//#endif
//
//	tString.m_buffersize = 0;
//	tString.m_pString = NULL;
//	tString.m_pTempString = NULL;
//	tString.m_savedcurrenttok = 0;
//	tString.m_savedpos = 0;
//	tString.m_savedtotaltoks = 0;
//}

/****************************/
/*! \fn TString::TString( const std::initializer_list<TString> lt )
\brief Initializer list constructor

\param lt - List of TString objects to concate into the new TString object.
*/
/****************************/
TString::TString(const std::initializer_list<TString> &lt)
	: TString()
{
	for (const auto &s : lt)
	{
		append(s);
	}
}

/****************************/
/*! \fn TString::TString(const std::unique_ptr<TCHAR[]> &unique)
\brief unique_ptr constructor

\param unique - A unique_ptr object to an array of TCHAR's
*/
/****************************/

TString::TString(const std::unique_ptr<TCHAR[]> &unique)
	: TString(unique.get())
{
}

/****************************/
/*! \fn TString::TString(const TCHAR *pStart, const TCHAR *pEnd)
\brief Constructor

\param pStart	- pointer to start of string to turn into a TString
\param pEnd		- pointer to end of string
*/
/****************************/
TString::TString(const TCHAR *const pStart, const TCHAR *const pEnd)
	: m_pTempString(nullptr), m_pString(nullptr)
	, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(nullptr)
	, m_bDirty(true), m_iLen(0)
#if TSTRING_INTERNALBUFFER
	, m_bUsingInternal(false)
#endif
{
	if ((pStart != nullptr) && (pEnd != nullptr) && (pEnd > pStart)) {
		const size_t size = (pEnd - pStart);
		this->m_pString = allocstr_cch(size + 1);
		ts_strcpyn_throw(this->m_pString, pStart, size + 1);
	}
	else {
		this->m_pString = allocstr_cch(1);
		this->m_pString[0] = TEXT('\0');
	}
}

/*!
 * \brief Allocates a buffer tsSize long.
 *
 * NB tsSize is in characters! not bytes.
 *
 */
TString::TString( const UINT tsSize )
	: m_pTempString(nullptr), m_pString(nullptr)
	, m_savedtotaltoks(0), m_savedcurrenttok(0), m_savedpos(nullptr)
	, m_bDirty(true), m_iLen(0)
#if TSTRING_INTERNALBUFFER
	, m_bUsingInternal(false)
#endif
{
	this->m_pString = allocstr_cch(tsSize + 1);
	ts_zeromem(this->m_pString, this->m_buffersize);
}

TString::~TString( ) {

	this->deleteString( );
}

/*!
 * \brief Deletes the allocated TCHAR buffer
 */
void TString::deleteString(const bool bKeepBufferSize)
{
#if TSTRING_INTERNALBUFFER
	if (!m_bUsingInternal)
		delete [] this->m_pString; 
#else
	delete[] this->m_pString;
#endif

	this->m_pString = nullptr;

	this->deleteTempString(bKeepBufferSize);
}

void TString::deleteTempString(const bool bKeepBufferSize) {

	delete [] this->m_pTempString; 

	this->m_pTempString = nullptr;
	this->m_savedtotaltoks = 0;
	this->m_savedcurrenttok = 0;
	this->m_savedpos = 0;
	if (!bKeepBufferSize)
		this->m_buffersize = 0;
}

/****************************/
//TString& TString::operator =(TString tString)
//{
//	this->swap(tString);
//
//	return *this;
//}

TString& TString::operator =(const TString &tString)
{
	//TString tmp(tString);
	//this->swap(tmp);

	this->copy(tString);

	return *this;
}

/****************************/
TString& TString::operator =(TString &&tString)
{
	if (this == &tString)	// self assignment check.
		return *this;

	this->swap(tString);

	return *this;
}

/****************************/
/*! \fn void TString::operator=( const TCHAR * string )
    \brief Symbol Overload - TString assignement from C TCHAR string

    \param cString Character string to assign string to
*/
/****************************/
TString& TString::operator =( const WCHAR *const cString )
{
	//TString tmp(cString);
	//this->swap(tmp);

	this->copy(cString);

	return *this;
}

/****************************/
TString& TString::operator =( const char *const cString )
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
TString& TString::operator =( const WCHAR chr )
{
	TString tmp(chr);
	this->swap(tmp);

	//this->copy(chr); // cant convert for some reason?

	return *this;
}

/****************************/
TString& TString::operator =( const char chr )
{
	TString tmp(chr);
	this->swap(tmp);

	//this->copy(chr); // cant convert for some reason?

	return *this;
}

/****************************/
/*! \fn TString TString::operator +( const TCHAR * string )
    \brief Symbol Overload - TString and TCHAR string concatenation

    \param cString Character string to concatenate

    \return Returns the new TString object
*/
/****************************/

TString TString::operator +( const TCHAR *const cString ) {

	//if (cString == nullptr)
	//	return *this; // return an object not a ref or pointer to object, so object is copied on return.
	//
	//const size_t sz = this->len();
	//const size_t l = (sz + ts_strlen(cString) + 1);
	//TString newTString((UINT)l);
	//
	//if (sz > 0)
	//{
	//	ts_strcpyn_throw(newTString.m_pString, this->m_pString, l);
	//}
	//// NB: Buffer is zerod for us already.
	//ts_strcat_throw(newTString.m_pString, cString);
	//
	//return newTString;

	TString newTString(*this);
	return newTString.append(cString);
}

/****************************/
/*! \fn TString TString::operator +( const TCHAR chr )
    \brief Symbol Overload - TString and TCHAR concatenation

    \param chr Character to concatenate

    \return Returns the new TString object
*/
/****************************/

TString TString::operator +( const TCHAR chr )
{
	//const size_t size = this->len();
	//TString newTString((UINT)(size + 2));
	//
	//if (size > 0)
	//{
	//	ts_strcpyn_throw(newTString.m_pString, this->m_pString, size + 1);
	//}
	//
	//newTString.m_pString[size] = chr;
	//newTString.m_pString[size + 1] = 0;
	//
	//return newTString;

	TString newTString(*this);
	newTString += chr;
	return newTString;
}

/****************************/
/*! \fn TString TString::operator +( const TString & tString )
    \brief Symbol Overload - TString and TString concatenation

    \param tString TString object to concatenate

    \return Returns the new TString object
*/
/****************************/

TString TString::operator +( const TString & tString ) {

	//if (tString.empty())
	//	return *this;
	//
	//const size_t sz = this->len();
	//const size_t l = (sz + tString.len() + 1);	// we could use the 2x m_buffersize instead, but this could lead to a very large unneeded buffer.
	//
	//TString newTString((UINT)l);
	//
	//if (sz > 0) {
	//	ts_strcpyn_throw(newTString.m_pString, this->m_pString, l);
	//}
	//ts_strcat_throw(newTString.m_pString, tString.m_pString);
	//
	//return newTString;

	TString newTString(*this);
	return newTString.append(tString);
}

/****************************/
/*! \fn TString TString::operator -( const TCHAR * string )
    \brief Symbol Overload - remove TCHAR string from TString

    \param cString Character string to remove

    \return Returns the new TString object
*/
/****************************/

TString TString::operator -( const TCHAR *const cString ) {

	if (cString == nullptr)
		return *this;

	TString newTString(*this);
	newTString.i_remove(cString);
	return newTString;
}

TString TString::operator -( const TCHAR chr) {

	if ( chr == 0 )
		return *this;

	TString newTString(*this);
	const TCHAR str[] = { chr, TEXT('\0') };

	newTString.i_remove(str);
	return newTString;
}

TString TString::operator -( const TString & tString ) {

	if ( tString.empty() )
		return *this;

	TString newTString(*this);
	newTString.i_remove(tString.m_pString);
	return newTString;
}

/****************************/
/*! \fn void TString::operator +=( const TCHAR * string )
    \brief Symbol Overload - TString and TCHAR string concatenation

    \param cString Character string to concatenate
*/
/****************************/

TString & TString::operator +=( const TCHAR *const cString ) {

	return append(cString);
}

/****************************/
/*! \fn void TString::operator +=( const TCHAR chr )
    \brief Symbol Overload - TString and TCHAR concatenation

    \param chr Character to concatenate
*/
/****************************/

TString & TString::operator +=( const TCHAR chr ) {

	const TCHAR tmp[] = { chr, TEXT('\0') };
	return append(tmp, 1);
}

/****************************/
/*! \fn void TString::operator +=( const TString & tString )
    \brief Symbol Overload - TString and TString concatenation

    \param tString TString object to concatenate
*/
/****************************/

TString & TString::operator +=( const TString & tString ) {

	return append(tString);
}

/****************************/
/*! \fn void TString::operator +=( const int num )
\brief Symbol Overload - TString and INT concatenation

\param num Number to concatenate (is converted to text)
*/
/****************************/

#ifdef TSTRING_TEMPLATES
TString & TString::operator +=(const int num) {	return append_number(num); }
#else
TString & TString::operator +=(const int num) {

	TCHAR tNum[36];
	tNum[0] = TEXT('\0');

	ts_itoa(num, tNum, 10);

	return append(tNum);
}
#endif

/****************************/
/*! \fn void TString::operator -=( const TCHAR * string )
\brief Symbol Overload - remove TCHAR string from TString

\param cString Character string to remove
*/
/****************************/

TString & TString::operator -=( const TCHAR *const cString ) {

	if (cString != nullptr)
		this->i_remove(cString);

	return *this;
}

TString & TString::operator -=( const TCHAR chr ) {

	if ( chr != 0 ) {
		const TCHAR str[] = { chr, TEXT('\0') };
		this->i_remove(str);
	}

	return *this;
}

TString & TString::operator -=( const TString &tString ) {

	if ( !tString.empty() )
		this->i_remove(tString.m_pString);

	return *this;
}

/****************************/
/*! \fn bool TString::operator ==( const int iNull )
    \brief Symbol Overload - TString and NULL comparison

    \param iNull Value of comparison (usually 0 for NULL)

    \return Returns \b true or \b false based on the comparison
*/

bool TString::operator ==( const int iNull ) const {

	return (this->m_pString == nullptr && !iNull);
}

/****************************/
/*! \fn void TString::operator ==( const TCHAR * string )
    \brief Symbol Overload - TString and TCHAR string comparison

    \param cString Character string to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator ==( const TCHAR *const cString ) const
{
	if (cString == nullptr || this->m_pString == nullptr)
		return false;

	return (ts_strcmp(this->m_pString, cString) == 0);
}

/****************************/
/*! \fn void TString::operator ==( const TCHAR chr )
    \brief Symbol Overload - TString and TCHAR comparison

    \param chr Character to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator ==( const TCHAR chr ) const
{
	if (this->m_pString == nullptr)
		return false;

	const TCHAR temp[] = { chr, TEXT('\0') };

	return (ts_strcmp(this->m_pString, temp) == 0);
}

/****************************/
/*! \fn void TString::operator ==( const TString & tString )
    \brief Symbol Overload - TString and TString comparison

    \param tString TString object to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator ==( const TString & tString ) const
{
	return (this->compare(tString) == 0);
}

/****************************/
/*! \fn bool TString::operator !=( const int iNull )
    \brief Symbol Overload - TString and NULL comparison

    \param iNull Value of comparison (usually 0 for NULL)

    \return Returns \b true or \b false based on the comparison
*/

bool TString::operator !=( const int iNull ) const
{
	return (this->m_pString != nullptr && !iNull);
}

/****************************/
/*! \fn void TString::operator !=( const TCHAR * cString )
    \brief Symbol Overload - TString and TCHAR string comparison

    \param cString Character string to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator !=( const TCHAR *const cString ) const
{
	if (cString == nullptr || this->m_pString == nullptr)
		return false;

	return (ts_strcmp(this->m_pString, cString) != 0);
}

/****************************/
/*! \fn void TString::operator !=( const TCHAR chr )
    \brief Symbol Overload - TString and TCHAR comparison

    \param chr Character to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator !=( const TCHAR chr ) const
{
	if (this->m_pString == nullptr)
		return false;

	const TCHAR temp[] = { chr, TEXT('\0') };

	return (ts_strcmp(this->m_pString, temp) != 0);
}

/****************************/
/*! \fn void TString::operator !=( const TString & tString )
    \brief Symbol Overload - TString and TString comparison

    \param tString TString object to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator !=( const TString & tString ) const
{
	return (this->compare(tString) != 0);
}

/****************************/
/*! \fn void TString::operator >( const TCHAR * cString )
    \brief Symbol Overload - TString and TCHAR string comparison

    \param cString Character string to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator >( const TCHAR *const cString ) const {

	if (cString == nullptr || this->m_pString == nullptr)
		return false;

	return (ts_strcmp(this->m_pString, cString) > 0);
}

/****************************/
/*! \fn void TString::operator >( const TCHAR chr )
    \brief Symbol Overload - TString and TCHAR comparison

    \param chr Character to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator >( const TCHAR chr ) const {

	if (this->m_pString == nullptr)
		return false;

	const TCHAR temp[] = { chr, TEXT('\0') };
	return (ts_strcmp(this->m_pString, temp) > 0);
}

/****************************/
/*! \fn void TString::operator >( const TString & tString )
    \brief Symbol Overload - TString and TString comparison

    \param tString TString object to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator >( const TString & tString ) const
{
	return (this->compare(tString) > 0);
}

/****************************/
/*! \fn void TString::operator >=( const TCHAR * cString )
    \brief Symbol Overload - TString and TCHAR string comparison

    \param cString Character string to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator >=( const TCHAR *const cString ) const {

	if (cString == nullptr || this->m_pString == nullptr)
		return false;

	return (ts_strcmp(this->m_pString, cString) >= 0);
}

/****************************/
/*! \fn void TString::operator >=( const TCHAR chr )
    \brief Symbol Overload - TString and TCHAR comparison

    \param chr Character to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator >=( const TCHAR chr ) const {

	if (this->m_pString == nullptr)
		return false;

	const TCHAR temp[] = { chr, TEXT('\0') };

	return (ts_strcmp(this->m_pString, temp) >= 0);
}

/****************************/
/*! \fn void TString::operator >=( const TString & tString )
    \brief Symbol Overload - TString and TString comparison

    \param tString TString object to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator >=( const TString & tString ) const
{
	return (this->compare(tString) >= 0);
}

/****************************/
/*! \fn void TString::operator <( const TCHAR * cString )
    \brief Symbol Overload - TString and TCHAR string comparison

    \param cString Character string to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator <( const TCHAR *const cString ) const {

	if (cString == nullptr || this->m_pString == nullptr)
		return false;

	return (ts_strcmp(this->m_pString, cString) < 0);
}

/****************************/
/*! \fn void TString::operator <( const TCHAR chr )
    \brief Symbol Overload - TString and TCHAR comparison

    \param chr Character to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator <( const TCHAR chr ) const {

	if (this->m_pString == nullptr)
		return false;

	const TCHAR temp[] = { chr, TEXT('\0') };

	return (ts_strcmp(this->m_pString, temp) < 0);
}

/****************************/
/*! \fn void TString::operator <( const TString & tString )
    \brief Symbol Overload - TString and TString comparison

    \param tString TString object to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator <( const TString & tString ) const
{
	return (this->compare(tString) < 0);
}

/****************************/
/*! \fn void TString::operator <=( const TCHAR * cString )
    \brief Symbol Overload - TString and TCHAR string comparison

    \param cString Character string to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator <=( const TCHAR *const cString ) const {

	if (cString == nullptr || this->m_pString == nullptr)
		return false;

	return (ts_strcmp(this->m_pString, cString) <= 0);
}

/****************************/
/*! \fn void TString::operator <=( const TCHAR chr )
    \brief Symbol Overload - TString and TCHAR comparison

    \param chr Character to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator <=( const TCHAR chr ) const {

	if (this->m_pString == nullptr)
		return false;

	const TCHAR temp[] = { chr, TEXT('\0') };

	return (ts_strcmp(this->m_pString, temp) <= 0);
}

/****************************/
/*! \fn void TString::operator <=( const TString & tString )
    \brief Symbol Overload - TString and TString comparison

    \param tString TString object to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator <=( const TString & tString ) const
{
	return (this->compare(tString) <= 0);
}

/****************************/
/*! \fn TString TString::operator *( const int N )
    \brief Symbol Overload - TString string Repetition

    \param N Number of repetitions

    \return Returns a TString object of the the new string

    \note Using \b 0 for N makes the string being empty
*/
/****************************/

TString TString::operator *( const int &N ) {

	if (N < 0 || N == 1)
		return *this;

	const size_t sz = this->len();

	if ((N == 0) || (sz == 0))
		return TEXT("");

	TString tmp((UINT)((sz *N) + 1));

	for (int i = 0; i < N; i++)
	{
		//append(*this);
		ts_strcat_throw(tmp.m_pString, this->m_pString);
	}
	return tmp;
}

/****************************/
/*! \fn void TString::operator *=( const int N )
    \brief Symbol Overload - TString string Repetition

    \param N Number of repetitions

    \note Using \b 0 for N makes the string being empty
*/
/****************************/

TString & TString::operator *=( const int &N ) {

	if (N == 0) {
		//this->deleteString();
		this->clear();
		return *this;
	}

	const size_t sz = this->len();

	// if string is null just return.
	if ((N == 1) || (sz == 0))
		return *this;

	TString tmp((UINT)((sz * N) + 1));

	for (int i = 0; i < N; i++)
	{
		//append(*this);
		ts_strcat_throw(tmp.m_pString, this->m_pString);
	}

	this->swap(tmp);

	return *this;
}

/****************************/
/*! \fn TCHAR & TString::operator []( long int N )
    \brief Symbol Overload - Array Subscript

    \param N Character index

    \return Returns the Nth TCHAR of string

    \note > Index starts at \b 0 \n
          > Index can be negative, \b -1 being last character
		  > If Index is out of range throws std::out_of_range exception
*/
/****************************/

TCHAR & TString::operator []( long int N )
{
	// NB: size needs to be in characters, m_buffersize is in bytes (multiples of TCHAR)
	if (N < 0)
		N += (this->m_buffersize / sizeof(TCHAR));

	CheckRange(N);

	m_bDirty = true;

	return this->m_pString[N];
}

/****************************/
/*! \fn TCHAR TString::operator []( long int N ) const
	\brief Symbol Overload - Array Subscript

	\param N Character index

	\return Returns the Nth TCHAR of string

	\note	> Index starts at \b 0 \n
			> Index can be negative, \b -1 being last character
			> If Index is out of range a 0 is returned.
*/
/****************************/
TCHAR TString::operator [](long int N) const {

	static TCHAR chr = TEXT('\0');

	if (this->m_pString == NULL)
		return chr;

	// NB: size needs to be in characters, m_buffersize is in bytes (multiples of TCHAR)
	const long int size = (this->m_buffersize / sizeof(TCHAR));

	if (N < 0) N += size;

	if ((N >(size - 1)) || (N < 0))
		return chr;

	return this->m_pString[N];
}

TString operator +(const TString & tString, const TCHAR *const cString)
{
	if ((cString == nullptr) || (cString[0] == TEXT('\0')))
		return tString;

	const size_t sz = tString.len();
	TString newTString((UINT)(sz + ts_strlen(cString)) + 1);

	if (sz > 0) {
		ts_strcpy_throw(newTString.m_pString, tString.m_pString);
	}
	ts_strcat_throw(newTString.m_pString, cString);

	return newTString;
}

/****************************/
/*! \fn size_t TString::len( )
    \brief Returns string length

    \return String length
*/

const size_t TString::len( ) const {
	if (this->m_pString == nullptr)
		return 0;

	if (this->m_pString[0] == TEXT('\0'))	// check for zero length string.
		return 0;

	if (m_bDirty)
	{
		m_iLen = static_cast<size_t>(ts_strlen(this->m_pString));
		m_bDirty = false;
	}
	return m_iLen;
}

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

int TString::find(const TCHAR *const substring, const int N) const {

	if ((substring != nullptr) && (this->m_pString != nullptr)) {

		const TCHAR * temp = nullptr, *temp2 = this->m_pString;

		int i = 0;
		const size_t subl = ts_strlen(substring); // Ook
		while ((temp = ts_strstr(temp2, substring)) != nullptr) {
			i++;
			//if ( N != 0 && i == N )
			if (i == N) // i is never zero
				return (int)(temp - this->m_pString);
			temp2 = (temp + subl); // Ook
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
int TString::find(const TCHAR chr, const int N) const {
	int c = 0;
	const size_t len = this->len();

	for (size_t i = 0; i < len; i++) {
		// found a match, increase counter
		if (this->m_pString[i] == chr)
			c++;

		// if we've reached the Nth match we want, return the position
		if ((N > 0) && (c == N))
			return (int)i;
	}

	// return number of results
	if (N == 0)
		return c;

	// -1 if no matches
	return -1;
}


/****************************/
/*! \fn TString TString::findtok(TCHAR * cToken, int N, TCHAR * sepChars)
    \brief Returns the index of a specific token

	 \param cToken the token to find
    \param N the Nth match
    \param sepChars separating character

    \return index of found token

    \note > Index starts at \b 1 \n
*/
/****************************/
size_t TString::findtok(const TCHAR *const cToken, const UINT N, const TCHAR *const sepChars) const {

	const UINT nToks = this->numtok(sepChars);

	this->getfirsttok( 0, sepChars );

	for (UINT i = 1, count = 0; i <= nToks; i++) {
		if (this->getnexttok(sepChars) == cToken) {
			count++;

			if (count == N)
				return i;
		}
	}
	return 0;
}


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

TString TString::sub( int N, int M ) const {

	//TString newTString;
	//
	//if (this->m_pString != NULL) {
	//
	//	const int size = (int)ts_strlen(this->m_pString);
	//
	//	if (N < 0) N += size;
	//
	//	if (N > size - 1 || N < 0)
	//		return newTString;
	//
	//	if (N + M > size)
	//		M = size - N;
	//
	//	delete[] newTString.m_pString; // change by Ook
	//
	//	newTString.m_pString = newTString.allocstr_cch(M + 1);
	//	newTString.m_pString[0] = 0;
	//
	//	TCHAR * temp = this->m_pString;
	//	temp += N;
	//
	//	if (ts_strcpyn(newTString.m_pString, temp, M + 1) == NULL)
	//		throw std::logic_error("strcpyn() failed!");
	//
	//	newTString.m_pString[M] = 0;
	//}
	//return newTString;


	if (this->empty())
		return TEXT("");

	const int size = (int)len();

	if (N < 0) N += size;

	if (N > size - 1 || N < 0)
		return TEXT("");

	if (N + M > size)
		M = size - N;

	const TCHAR *const temp = (this->m_pString + N);

	return TString(temp, (temp + M));
}

/*
internal remove function, called by the other remove()/-/-= functions.
Ook
*/

// NB: This needs looked at!!!! doesnt remove all instances of substring correctly!
int TString::i_remove(const TCHAR *const subString)
{
	// no str to remove or no str to remove from
	if ((subString == nullptr) || (this->empty()))
		return 0;

	const TCHAR *sub, *p = this->m_pString;
	int cnt = 0;
	const size_t subl = ts_strlen(subString), ol = this->len();
	// if length of sub string is 0 return 0
	// may change this.
	if (subl == 0)
		return 0;

	TString tmp((UINT)(ol + 1));

	while ((sub = ts_strstr(p, subString)) != NULL) {
		ts_strncat_throw(tmp.m_pString, p, (sub - p)); // copy bit before substring. if any.

		cnt++;
		p = sub + subl; // update pointer to skip substring.
	}

	if (cnt > 0) {
		ts_strcat_throw(tmp.m_pString, p); // append the end text, if any.

		this->swap(tmp);
	}

	return cnt;
}

/*
	internal replace function, called by the other replace() functions.
	Ook
*/
int TString::i_replace(const TCHAR *const subString, const TCHAR *const rString)
{
	if ((subString == nullptr) || (rString == nullptr) || this->empty())
		return 0;

	const TCHAR *sub, *p = this->m_pString;
	int cnt = 0;
	const size_t subl = ts_strlen(subString), repl = ts_strlen(rString), ol = this->len();

	// if length of sub string is 0 return 0
	// may change this.
	if (subl == 0)
		return 0;

	// Ook: needs looked at to avoid double loop if we can.
	// see if we have any matches & how many.
	while ((sub = ts_strstr(p, subString)) != nullptr) {
		cnt++;
		p = sub + subl;
	}
	// make final string if we have any matches.
	if (cnt > 0) {
		p = this->m_pString;
		//TCHAR *out = allocstr_cch((cnt * repl) + (ol - (cnt * subl)) + 1); // allocate new string.
		//out[0] = 0;
		//while ((sub = ts_strstr(p, subString)) != NULL) {
		//	if (ts_strncat(out, p, (sub - p)) == NULL) // copy bit before substring. if any.
		//	{
		//		delete[] out;
		//		throw std::logic_error("strncat() failed!");
		//	}
		//	if (ts_strcat(out, rString) == NULL) // append new replacement string.
		//	{
		//		delete[] out;
		//		throw std::logic_error("strcat() failed!");
		//	}
		//	p = sub + subl; // update pointer to skip substring.
		//}
		//if (ts_strcat(out, p) == NULL) // append the end text, if any.
		//{
		//	delete[] out;
		//	throw std::logic_error("strcat() failed!");
		//}
		//this->deleteString(true); // delete old string
		//this->m_pString = out; // save new one.

		TString tmp((UINT)((cnt * repl) + (ol - (cnt * subl)) + 1)); // allocate new string.

		while ((sub = ts_strstr(p, subString)) != nullptr) {
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

int TString::replace(const TCHAR *const subString, const TCHAR *const rString) {
	return this->i_replace(subString,rString);
}

/****************************/
/*! \fn int TString::replace( const TCHAR * substring, const TCHAR rchr )
    \brief blah!
*/
/****************************/

int TString::replace(const TCHAR *const subString, const TCHAR rchr) {

	const TCHAR tmp[] = { rchr, TEXT('\0') };
	return this->i_replace(subString,tmp);
}

/****************************/
/*! \fn int TString::replace( const TCHAR chr, const TCHAR * replace )
    \brief blah!
*/
/****************************/

int TString::replace(const TCHAR chr, const TCHAR *const rString) {

	const TCHAR tmp[] = { chr, TEXT('\0') };
	return this->i_replace(tmp,rString);
}

/****************************/
/*! \fn int TString::replace( const TCHAR chr, const TCHAR rchr )
    \brief blah!
*/
/****************************/

int TString::replace( const TCHAR chr, const TCHAR rchr ) {

	int cnt = 0;
	
	for (TCHAR *p = this->m_pString; *p != TEXT('\0'); p++)
	{
		if (*p == chr) {
			*p = rchr;
			cnt++;
		}
	}
	return cnt;
}

/****************************/
/*! \fn int TString::mreplace( const TCHAR chr, const TCHAR *fmt )
\brief replace every character in string that matches a character in `fmt` with `chr`
*/
/****************************/
int TString::mreplace(const TCHAR chr, const TCHAR *const fmt)
{
	int cnt = 0;

	for (TCHAR *p = this->m_pString; *p != TEXT('\0'); p++)
	{
		UINT i = 0;
		for (TCHAR rchr = fmt[0]; rchr != TEXT('\0'); rchr = fmt[i])
		{
			if (*p == rchr) {
				*p = chr;
				cnt++;
			}
			i++;
		}
	}
	return cnt;
}

/*!
 * \brief blah
 *
 * blah
 */

TString TString::gettok(int N, const TCHAR *const sepChars) const {

	if (sepChars == nullptr)
		return *this;

	const UINT nToks = this->numtok(sepChars);

	if ((N > (int)nToks) || (nToks == 0))
		return TEXT("");

	if (N < 0) {

		N += (nToks + 1);
		if (N < 1)
			return TEXT("");
	}

	//TString token;
	//
	//TCHAR * p_cStart = this->m_pString, *p_cEnd = this->m_pString;
	//long iCount = 0;
	//const size_t sepl = ts_strlen(sepChars); // Ook
	//
	//while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != NULL) {
	//	iCount++;
	//
	//	if (iCount == N) {
	//
	//		const size_t len = p_cEnd - p_cStart; // gives cch diff
	//		token.deleteString();
	//		token.m_pString = token.allocstr_cch(len + 1);
	//
	//		if (ts_strcpyn(token.m_pString, p_cStart, len + 1) == NULL)
	//			throw std::logic_error("strcpyn() failed!");
	//
	//		break;
	//	}
	//	p_cStart = p_cEnd + sepl; // Ook
	//}
	//
	//if (iCount == N - 1) {
	//
	//	p_cEnd = this->m_pString + ts_strlen(this->m_pString);
	//	const size_t len = p_cEnd - p_cStart;
	//
	//	token.deleteString();
	//	token.m_pString = token.allocstr_cch(len + 1);
	//
	//	if (ts_strcpyn(token.m_pString, p_cStart, len + 1) == NULL)
	//		throw std::logic_error("strcpyn() failed!");
	//}
	//
	//return token;

	const TCHAR *p_cStart = this->m_pString, *p_cEnd = nullptr, *const p_fEnd = this->m_pString + this->len();
	long iCount = 0;
	const size_t sepl = ts_strlen(sepChars); // Ook

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr) {
		iCount++;

		if (iCount == N)
			return TString(p_cStart, p_cEnd);

		p_cStart = p_cEnd + sepl; // Ook
		if (p_cStart >= p_fEnd)	// look out for overrun...
			break;
	}

	if (iCount == N - 1)
		return p_cStart;

	return TEXT("");
}

/*!
 * \brief blah
 *
 * params
 * N		-	the token number to retreive
 *				If Zero - returns an empty string.
 *				If Negative gets adjusted to N + (total number of tokens + 1)
 * M		-	End token to get (from N until M)
 *				If Zero returns an empty string
 *				If -1 returns all tokens untill the end of string.
 * sepChars	-	The token seperator string
 *				If NULL returns the whole current string.
 */

TString TString::gettok(int N, int M, const TCHAR *const sepChars) const {

	if (sepChars == nullptr)
		return *this;

	if (((M < N) && (M != -1)) || (N == 0) || (M == 0))
		return TEXT("");

	if (N == M)
		return this->gettok( N, sepChars );

	const UINT nToks = this->numtok( sepChars );

	if ((N > (int)nToks) || (nToks == 0))
		return TEXT("");

	if ( N < 0 ) {

		N += ( nToks + 1 );
		if ( N < 1 )
			return TEXT("");
	}

	if ( M > (int)(nToks - 1) )
		M = -1;

	const TCHAR * p_cStart = this->m_pString, *p_cEnd = nullptr, *const p_fEnd = this->m_pString + this->len();
	const TCHAR * p_cFirst = nullptr, *p_cLast = nullptr;
	size_t iCount = 0;
	const size_t sepl = ts_strlen( sepChars ); // Ook

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr) {
		iCount++;

		if ( (int)iCount == N ) {

			p_cFirst = p_cStart;

			if ( M == -1 )
				break;
		}

		if ( (int)iCount == M ) {

			//p_cLast = p_cStart;
			p_cLast = p_cEnd;
			break;
		}

		p_cStart = p_cEnd + sepl;
		if (p_cStart >= p_fEnd)	// look out for overrun...
			break;
	}

	if ( M == -1 ) {

		if ( iCount == nToks - 1 )
			p_cFirst = p_cStart;

		return p_cFirst;
	}
	if (iCount == nToks - 1)
		p_cLast = p_cEnd;

	return TString(p_cFirst, p_cLast);
}

TString TString::getfirsttok(const UINT N, const TCHAR *const sepChars) const
{
	this->m_savedtotaltoks = this->numtok(sepChars);
	this->m_savedpos = this->m_pString;
	this->m_savedcurrenttok = N;

	// N == 0 is used to pre load the vars for a loop of next toks where we need to start at 1

	if (sepChars == nullptr)
		return *this;

	if ((N > this->m_savedtotaltoks) || (N == 0))
		return TEXT("");

	const TCHAR * p_cStart = this->m_pString, *p_cEnd = this->m_pString, *const p_fEnd = (this->m_pString + this->len());
	UINT iCount = 0;
	const size_t sepl = ts_strlen(sepChars); // Ook

	//this->m_savedSepCharsLength = sepl;
	//this->m_tSavedSepChars = std::make_unique<TCHAR[]>(sepl);
	//ts_strcpy_throw(this->m_tSavedSepChars.get(), sepChars);

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr) {
		iCount++;

		if (iCount == N) {
			this->m_savedpos = p_cEnd + sepl;
			return TString(p_cStart, p_cEnd);
		}
		p_cStart = p_cEnd + sepl; // Ook
		if (p_cStart >= p_fEnd) {
			this->m_savedpos = nullptr;
			break;
		}
	}

	if (iCount == N - 1) {
		this->m_savedpos = nullptr;
		return TString(p_cStart, p_fEnd);
	}

	return TEXT("");
}

TString TString::getnexttok(const TCHAR *const sepChars) const {

	if (sepChars == nullptr || this->m_pString == nullptr)
		return *this;

	this->m_savedcurrenttok++;
	const TCHAR *const p_cStart = this->m_savedpos;

	if ((this->m_savedcurrenttok > this->m_savedtotaltoks) || (p_cStart == nullptr))
		return TEXT("");

	if (this->m_savedcurrenttok == this->m_savedtotaltoks) {
		this->m_savedpos = nullptr;
		return p_cStart;
	}
	else {
		const TCHAR *const p_cEnd = ts_strstr(p_cStart, sepChars);
		if (p_cEnd != nullptr) {
			this->m_savedpos = (p_cEnd + ts_strlen(sepChars));
			return TString(p_cStart, p_cEnd);
		}
	}
	return TEXT("");
}

TString TString::getlasttoks( ) const
{
	if (this->empty())
		return TEXT("");

	// set current token to the last one.
	this->m_savedcurrenttok = this->m_savedtotaltoks;
	// get saved position (this can be NULL)
	const TCHAR *const p_cStart = this->m_savedpos;
	// set saved position to NULL
	this->m_savedpos = nullptr;
	// return end of string (can be NULL as constructor handles this)
	return p_cStart;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT TString::numtok(const TCHAR *const sepChars) const {

	if (sepChars == nullptr || this->empty())
		return 0;

	//if (this->m_savedtotaltoks > 0)
	//	return this->m_savedtotaltoks;

	const TCHAR * p_cStart = this->m_pString, *p_cEnd = nullptr;
	UINT iCount = 0;
	const UINT sepl = ts_strlen( sepChars ); // Ook

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr) {
		iCount++;
		p_cStart = p_cEnd + sepl; // Ook
	}
	//this->m_savedtotaltoks = iCount + 1;
	//return this->m_savedtotaltoks;
	return iCount + 1;
}

/*!
 * \brief blah
 *
 * blah
 */

void TString::deltok(const UINT N, const TCHAR *const sepChars) {

	if (sepChars == nullptr || this->empty())
		return;

	const UINT nToks = this->numtok( sepChars );

	if ( N > nToks || N < 1 )
		return;

	if ( N == 1 && nToks == 1 ) {
		this->clear();
		//this->deleteString();
		return;
	}

	TCHAR * p_cStart = this->m_pString, *p_cEnd = nullptr;
	size_t i = 0;
	const UINT sepl = ts_strlen( sepChars ); // Ook

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr) {
		i++;

		if ( i == N )
			break;

		p_cStart = p_cEnd + sepl; // Ook
	}

	//// last token
	//if ( p_cEnd == NULL ) {
	//
	//	p_cStart--;
	//	*p_cStart = 0;
	//
	//	const int l = ts_strlen( this->m_pString ) + 1;
	//	TCHAR * pNew = allocstr_cch(l);
	//	if (ts_strcpyn(pNew, this->m_pString, l) == NULL)
	//	{
	//		delete[] pNew;
	//		throw std::logic_error("strcpyn() failed!");
	//	}
	//	this->deleteString(true);
	//	this->m_pString = pNew;
	//}
	//// delete the first token
	//else if ( p_cStart == this->m_pString ) {
	//
	//	p_cEnd++;
	//
	//	const int l = ts_strlen( p_cEnd ) + 1;
	//	TCHAR * pNew = allocstr_cch(l);
	//	if (ts_strcpyn(pNew, p_cEnd, l) == NULL)
	//	{
	//		delete[] pNew;
	//		throw std::logic_error("strcpyn() failed!");
	//	}
	//	this->deleteString(true);
	//	this->m_pString = pNew;
	//}
	//// inbound token
	//else {
	//
	//	*p_cStart = 0;
	//	p_cEnd++;
	//
	//	const size_t l = (ts_strlen( this->m_pString ) + ts_strlen( p_cEnd ) + 1);
	//	TCHAR * pNew = allocstr_cch(l);
	//	if (ts_strcpyn(pNew, this->m_pString, l) == NULL)
	//	{
	//		delete[] pNew;
	//		throw std::logic_error("strcpyn() failed!");
	//	}
	//	if (ts_strcat(pNew, p_cEnd) == NULL)
	//	{
	//		delete[] pNew;
	//		throw std::logic_error("strcpyn() failed!");
	//	}
	//
	//	this->deleteString(true);
	//	this->m_pString = pNew;
	//}

	const size_t l = this->len();

	TString tmp((UINT)l);

	// last token
	if (p_cEnd == nullptr) {
		p_cStart--;
		*p_cStart = 0;
		p_cEnd = this->m_pString;
	}
	// delete the first token
	else if ( p_cStart == this->m_pString )
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

void TString::instok(const TCHAR *const cToken, const UINT N, const TCHAR *const sepChars) {

	if (cToken == nullptr || sepChars == nullptr || this->m_pString == nullptr)
		return;

	if ( N < 1 )
		return;

	TCHAR * p_cStart = this->m_pString, *p_cEnd = nullptr;
	size_t i = 1;
	const size_t sepl = ts_strlen( sepChars ); // Ook

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr) {
		if ( i == N )
			break;

		i++;

		p_cStart = p_cEnd + sepl; // Ook
	}

	//TCHAR * pNew = allocstr_cch(ts_strlen(cToken) + ts_strlen(this->m_pString) + (sepl *2) + 1);
	//
	//// delete the first token
	//if (p_cStart == this->m_pString) {
	//
	//	if (ts_strcpy(pNew, cToken) == NULL)
	//	{
	//		delete[] pNew;
	//		throw std::logic_error("strcpyn() failed!");
	//	}
	//	if (ts_strcat(pNew, sepChars) == NULL)
	//	{
	//		delete[] pNew;
	//		throw std::logic_error("strcat() failed!");
	//	}
	//	if (ts_strcat(pNew, this->m_pString) == NULL)
	//	{
	//		delete[] pNew;
	//		throw std::logic_error("strcat() failed!");
	//	}
	//}
	//else if ( p_cEnd == NULL ) {
	//
	//	if ( i == N ) {
	//
	//		p_cStart -= sepl; // Ook
	//		*p_cStart = 0;
	//		p_cStart += sepl; // Ook
	//
	//		if (ts_strcpy(pNew, this->m_pString) == NULL)
	//		{
	//			delete[] pNew;
	//			throw std::logic_error("strcpy() failed!");
	//		}
	//		if (ts_strcat(pNew, sepChars) == NULL)
	//		{
	//			delete[] pNew;
	//			throw std::logic_error("strcat() failed!");
	//		}
	//		if (ts_strcat(pNew, cToken) == NULL)
	//		{
	//			delete[] pNew;
	//			throw std::logic_error("strcat() failed!");
	//		}
	//		if (ts_strcat(pNew, sepChars) == NULL)
	//		{
	//			delete[] pNew;
	//			throw std::logic_error("strcat() failed!");
	//		}
	//		if (ts_strcat(pNew, p_cStart) == NULL)
	//		{
	//			delete[] pNew;
	//			throw std::logic_error("strcat() failed!");
	//		}
	//	}
	//	else {
	//		if (ts_strcpy(pNew, this->m_pString) == NULL)
	//		{
	//			delete[] pNew;
	//			throw std::logic_error("strcpy() failed!");
	//		}
	//		if (ts_strcat(pNew, sepChars) == NULL)
	//		{
	//			delete[] pNew;
	//			throw std::logic_error("strcat() failed!");
	//		}
	//		if (ts_strcat(pNew, cToken) == NULL)
	//		{
	//			delete[] pNew;
	//			throw std::logic_error("strcat() failed!");
	//		}
	//	}
	//}
	//else {
	//	p_cStart -= sepl; // Ook
	//	*p_cStart = 0;
	//	p_cStart += sepl; // Ook
	//
	//	if (ts_strcpy(pNew, this->m_pString) == NULL)
	//	{
	//		delete[] pNew;
	//		throw std::logic_error("strcpy() failed!");
	//	}
	//	if (ts_strcat(pNew, sepChars) == NULL)
	//	{
	//		delete[] pNew;
	//		throw std::logic_error("strcat() failed!");
	//	}
	//	if (ts_strcat(pNew, cToken) == NULL)
	//	{
	//		delete[] pNew;
	//		throw std::logic_error("strcat() failed!");
	//	}
	//	if (ts_strcat(pNew, sepChars) == NULL)
	//	{
	//		delete[] pNew;
	//		throw std::logic_error("strcat() failed!");
	//	}
	//	if (ts_strcat(pNew, p_cStart) == NULL)
	//	{
	//		delete[] pNew;
	//		throw std::logic_error("strcat() failed!");
	//	}
	//}
	//this->deleteString(true);
	//this->m_pString = pNew;

	//const size_t l = (ts_strlen(cToken) + ts_strlen(this->m_pString) + (sepl * 2) + 1);
	//
	//TString tmp((UINT)l);
	//
	//// insert at start
	//if (p_cStart == this->m_pString) {
	//
	//	if (ts_strcpyn(tmp.m_pString, cToken, l) == NULL)
	//		throw std::logic_error("strcpyn() failed!");
	//	if (ts_strcat(tmp.m_pString, sepChars) == NULL)
	//		throw std::logic_error("strcat() failed!");
	//	if (ts_strcat(tmp.m_pString, this->m_pString) == NULL)
	//		throw std::logic_error("strcat() failed!");
	//}
	//else if (p_cEnd == NULL) {
	//	if (i == N) {
	//		// insert at pos
	//		p_cStart -= sepl; // Ook
	//		*p_cStart = 0;
	//		p_cStart += sepl; // Ook
	//
	//		if (ts_strcpyn(tmp.m_pString, this->m_pString, l) == NULL)
	//			throw std::logic_error("strcpyn() failed!");
	//		if (ts_strcat(tmp.m_pString, sepChars) == NULL)
	//			throw std::logic_error("strcat() failed!");
	//		if (ts_strcat(tmp.m_pString, cToken) == NULL)
	//			throw std::logic_error("strcat() failed!");
	//		if (ts_strcat(tmp.m_pString, sepChars) == NULL)
	//			throw std::logic_error("strcat() failed!");
	//		if (ts_strcat(tmp.m_pString, p_cStart) == NULL)
	//			throw std::logic_error("strcat() failed!");
	//	}
	//	else {
	//		// insert at end
	//		if (ts_strcpyn(tmp.m_pString, this->m_pString, l) == NULL)
	//			throw std::logic_error("strcpyn() failed!");
	//		if (ts_strcat(tmp.m_pString, sepChars) == NULL)
	//			throw std::logic_error("strcat() failed!");
	//		if (ts_strcat(tmp.m_pString, cToken) == NULL)
	//			throw std::logic_error("strcat() failed!");
	//	}
	//}
	//else {
	//	// insert at pos
	//	p_cStart -= sepl; // Ook
	//	*p_cStart = 0;
	//	p_cStart += sepl; // Ook
	//
	//	if (ts_strcpyn(tmp.m_pString, this->m_pString, l) == NULL)
	//		throw std::logic_error("strcpyn() failed!");
	//	if (ts_strcat(tmp.m_pString, sepChars) == NULL)
	//		throw std::logic_error("strcat() failed!");
	//	if (ts_strcat(tmp.m_pString, cToken) == NULL)
	//		throw std::logic_error("strcat() failed!");
	//	if (ts_strcat(tmp.m_pString, sepChars) == NULL)
	//		throw std::logic_error("strcat() failed!");
	//	if (ts_strcat(tmp.m_pString, p_cStart) == NULL)
	//		throw std::logic_error("strcat() failed!");
	//}
	//
	//this->swap(tmp);

	const size_t l = (ts_strlen(cToken) + this->len() + (sepl * 2) + 1);

	TString tmp((UINT)l);

	// insert at start
	if (p_cStart == this->m_pString) {

		ts_strcpyn_throw(tmp.m_pString, cToken, l);
		ts_strcat_throw(tmp.m_pString, sepChars);
		ts_strcat_throw(tmp.m_pString, this->m_pString);
	}
	else {
		if (i == N) {
			// insert at pos N
			p_cStart -= sepl; // Ook
			*p_cStart = 0;
			p_cStart += sepl; // Ook
		}
		// common code to insert at pos N & insert at end
		ts_strcpyn_throw(tmp.m_pString, this->m_pString, l);
		ts_strcat_throw(tmp.m_pString, sepChars);
		ts_strcat_throw(tmp.m_pString, cToken);

		if (i == N) {
			ts_strcat_throw(tmp.m_pString, sepChars);
			ts_strcat_throw(tmp.m_pString, p_cStart);
		}
	}

	this->swap(tmp);
}

/*!
 * \brief blah
 * added by Ook
 * blah
 */
void TString::addtok( const TCHAR *const cToken, const TCHAR *const sepChars )
{
	if (cToken == nullptr || sepChars == nullptr || this->m_pString == nullptr)
		return;

	//const size_t mp_len = this->len();
	//const size_t l = (ts_strlen(cToken) + mp_len + ts_strlen(sepChars) + 1);
	//
	//if (m_buffersize >= (l *sizeof(TCHAR)))
	//{
	//	if (mp_len > 0) {
	//		ts_strcat_throw(this->m_pString, sepChars);
	//	}
	//	ts_strcat_throw(this->m_pString, cToken);
	//	m_bDirty = true;
	//}
	//else {
	//	TString tmp((UINT)l);
	//
	//	if (mp_len > 0) {
	//		ts_strcpy_throw(tmp.m_pString, this->m_pString);
	//		ts_strcat_throw(tmp.m_pString, sepChars);
	//	}
	//	ts_strcat_throw(tmp.m_pString, cToken);
	//
	//	this->swap(tmp);
	//}

	if (!empty())
		append(sepChars);
	append(cToken);
}

void TString::addtok( const __int64 nToken, const TCHAR *const sepChars ) {

	if (sepChars == nullptr)
		return;

	TString tmp;
	if (this->empty())
		tmp.tsprintf(TEXT("%I64d"), nToken);
	else
		tmp.tsprintf(TEXT("%s%s%I64d"), this->m_pString, sepChars, nToken);

	this->swap(tmp);
}

bool TString::istok(const TCHAR *const cToken, const TCHAR *const sepChars ) const {

	if (sepChars == nullptr || this->empty())
		return false;

	const TCHAR * p_cStart = this->m_pString, *p_cEnd = nullptr;
	const UINT sepl = ts_strlen( sepChars );

	if (sepl == 0)
		return false;

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr) {
		const size_t l = (p_cEnd - p_cStart);
		if (l > 0) {
			if (ts_strncmp(cToken,p_cStart,l) == 0)
				return true;
		}
		p_cStart = p_cEnd + sepl;
	}
	return (ts_strcmp(cToken, p_cStart) == 0);
}

/*!
 * \brief blah
 *
 * Overwrite the Nth token
 */

void TString::puttok(const TCHAR *const cToken, const UINT N, const TCHAR *const sepChars) {

	//if ( cToken == NULL || sepChars == NULL || this->m_pString == NULL )
	//	return;
	//
	//TCHAR * p_cStart = this->m_pString, * p_cEnd = NULL;
	//long int i = 0;
	//const UINT sepl = ts_strlen( sepChars ); // Ook
	//const size_t tok_len = ts_strlen(cToken);
	//
	//while ( ( p_cEnd = ts_strstr( p_cStart, sepChars ) ) != NULL ) {
	//	i++;
	//
	//	if ( i == N )
	//		break;
	//
	//	p_cStart = p_cEnd + sepl; // Ook
	//}
	//
	//TCHAR *pNew = NULL;
	//
	//// last token
	//if ( p_cEnd == NULL ) {
	//
	//	*p_cStart = 0;
	//
	//	pNew = allocstr_cch(tok_len + ts_strlen(this->m_pString) + 1);
	//
	//	ts_strcpy( pNew, this->m_pString );
	//	ts_strcat( pNew, cToken );
	//}
	//// delete the first token
	//else if ( p_cStart == this->m_pString ) {
	//
	//	pNew = allocstr_cch(tok_len + ts_strlen(p_cEnd) + 1);
	//
	//	ts_strcpy(pNew, cToken);
	//	ts_strcat(pNew, p_cEnd);
	//}
	//// inbound token
	//else {
	//
	//	*p_cStart = 0;
	//
	//	pNew = allocstr_cch(ts_strlen(this->m_pString) + tok_len + ts_strlen(p_cEnd) + 1);
	//
	//	ts_strcpy(pNew, this->m_pString);
	//	ts_strcat(pNew, cToken);
	//	ts_strcat(pNew, p_cEnd);
	//}
	//this->deleteString(true);
	//this->m_pString = pNew;

	if (cToken == nullptr || sepChars == nullptr || this->m_pString == nullptr)
		return;

	TCHAR * p_cStart = this->m_pString, *p_cEnd = nullptr;
	size_t i = 0;
	const UINT sepl = ts_strlen(sepChars); // Ook
	const size_t tok_len = ts_strlen(cToken);

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr) {
		i++;

		if (i == N)
			break;

		p_cStart = p_cEnd + sepl; // Ook
	}

	TString tmp((UINT)(tok_len + this->len() + 1));	// max size that could be needed

	// replace the first token?
	if (p_cStart != this->m_pString) {
		// no, so fix end of first part & copy
		*p_cStart = 0;

		ts_strcpy_throw(tmp.m_pString, this->m_pString);
	}
	// add token at either start or middle or end
	ts_strcat_throw(tmp.m_pString, cToken);
	// add end of string if any.
	if (p_cEnd != NULL) {
		ts_strcat_throw(tmp.m_pString, p_cEnd);
	}

	// do shrink_to_fit() here?
	this->swap(tmp);
}

void TString::remtok(const TCHAR *const cToken, const UINT N, const TCHAR *const sepChars) {
	const UINT tokennr = findtok(cToken, N, sepChars);
	if (tokennr > 0)
		deltok(tokennr, sepChars);
}

/*!
 * \brief blah
 *
 * blah
 */
TString &TString::trim()
{
	if (this->empty())
		return *this;

	const TCHAR *start = this->m_pString;
	TCHAR *end = this->m_pString + this->len();
	const TCHAR *const oldEnd = end;

	// Trim from start
	while (start != end && *start == 32)
		start++;

	// Trim from end
	while (end != start && *(--end) == 32);

	// only alloc new string is string changed.

	end++;	// end must be increased by one to account for the previous decrement

	if (end != oldEnd)
		*end = TEXT('\0');	// place zero at new ending

	// only allocate new string if start of string modified.
	if (start != this->m_pString)
	{
		const size_t new_len = (end - start) + 1;

		TString tmp((UINT)new_len);

		ts_strcpyn_throw(tmp.m_pString, start, new_len);

		this->swap(tmp);
	}
	return *this;
}

#ifdef INCLUDE_MIRC_EXTRAS
/*
* isnum(option)
*   returns TRUE if string is a valid number, if option is > 0,
*   it'll take negative numbers as valid numbers
*   returns FALSE in an invalid string
*/
bool TString::isnum(const int f) const
{
	TCHAR *p = this->m_pString;
	int c = 0;
	if (*p == TEXT('-')) {
		if (f)
			return false;
		++p;
	}
	if (!f && *p == TEXT('-'))
		return false;
	while (*p) {
		if (*p < TEXT('0') || *p > TEXT('9'))
			return false;
		++c;
		++p;
	}
	return c ? true : false;
}

/*
* isincs(letter)
*   returns TRUE if letter is in word (case sensitive)
*   otherwise, a FALSE is returned
*/
bool TString::isincs(const char let) const
{
	//TCHAR *tmp = this->m_pString;
	//while (*tmp) {
	//	if (*tmp++ == let)
	//		return true;
	//}
	//return false;

	for (TCHAR *tmp = this->m_pString; *tmp; ++tmp)
	{
		if (*tmp == let)
			return true;
	}
	return false;
}

/*
* countchar(character)
*/
UINT TString::countchar(const TCHAR chr) const
{
	UINT r = 0;

	for (TCHAR *aux = this->m_pString; *aux; ++aux) {
		if (*aux == chr)
			++r;
	}
	return r;
}

/*
* ishostmask(mask)
*/
bool TString::ishostmask(void) const
{
	register bool /*nick = false,*/ s1 = false, /*ident = false,*/ s2 = false, host = false;
	register TCHAR *c = this->m_pString;
	while (*c) {
		if (*c == ' ')
			return false;
		else if (*c == TEXT('!')) {
			if (s1)
				return false;
			s1 = true;
		}
		else if (*c == TEXT('@')) {
			if (!s1 || s2)
				return false;
			s2 = true;
		}
		else if (s1 && s2) {
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
	if (host) {
		if (*--c == TEXT('.'))
			return false;
		return true;
	}
	return false;
}

TString TString::toupper(void) const
{
	//TString tmp(*this);
	//UINT c = 0;
	//char *p = tmp.to_chr();
	//const UINT l = tmp.len();
	//
	//while (c < l) {
	//	p[c] = (char)rfc_toupper(p[c]);
	//	c++;
	//}

	TString tmp(*this);
	UINT c = 0;
	const UINT l = tmp.len();
	for (TCHAR *p = tmp.to_chr(); c < l; c++)
		p[c] = ts_toupper(p[c]);

	return tmp;
}
TString TString::tolower(void) const
{
	//TString tmp(*this);
	//UINT c = 0;
	//char *p = tmp.to_chr();
	//while (c < tmp.len()) {
	//	p[c] = (char)rfc_tolower(p[c]);
	//	c++;
	//}

	TString tmp(*this);
	UINT c = 0;
	const UINT l = tmp.len();
	for (TCHAR *p = tmp.to_chr(); c < l; c++)
		p[c] = ts_tolower(p[c]);

	return tmp;
}
#endif

// pos must be within length of string, n can be < 0
TString TString::mid(const int pos, int n) const
{
	const int l = this->len();
	if ((n == 0) || (l == 0) || (pos > l) || (pos < 0))
		return TEXT("");
	if (n < 0)
		n = l - pos + n;
	else if ((pos + n) > l)
		n = l - pos;
	if (n < 1)
		return TEXT("");
	n++;

	//TString tmp((UINT)n);
	//
	//ts_strcpyn_throw(tmp.m_pString, &this->m_pString[pos], n);
	//
	//return tmp;
	return TString(&this->m_pString[pos], &this->m_pString[pos + n]);
}

// if n > string length its truncated, n can be < 0
TString TString::left(int n) const
{
	//const size_t l = ts_strlen(this->m_pString);
	//if ((l == 0) || (n == 0))
	//	return TEXT("");
	//if (n < 0)
	//	n = l + n;
	//if (n > l)
	//	n = l;
	//n++;
	//return TString(this->m_pString, (this->m_pString + n));

	return this->mid(0, n);
}

// if n > string length its truncated, n can be < 0
TString TString::right(int n) const
{
	const int l = this->len();
	if ((n == 0) || (l == 0))
		return TEXT("");
	if (n > l)
		n = l;
	int start = l - n, len = n + 1;
	if (n < 0) {
		start = abs(n);
		if (start > l)
			return TEXT("");
		len = (l - start) + 1;
	}

	//TString tmp((UINT)len);
	//
	//ts_strcpyn(tmp.m_pString, &this->m_pString[start], len);
	//
	//return tmp;
	return TString(&this->m_pString[start], &this->m_pString[start + len]);

	//const int l = this->len();
	//return this->mid(l - n,l);
}

// Ook - match() function taken from aircdll.dll by Tabo source.
/* taken from the hybrid-5.3p7 source */
#if UNICODE
inline int TString::rfc_tolower(const int c)
{
	//TCHAR tmp[2];
	//tmp[0] = (TCHAR)c;
	//tmp[1] = 0;
	//_wcslwr(tmp);
	//return (int)tmp[0];
	return ::tolower(c);
}

inline int TString::rfc_toupper(const int c)
{
	//TCHAR tmp[2];
	//tmp[0] = (TCHAR)c;
	//tmp[1] = 0;
	//_wcsupr(tmp);
	//return (int)tmp[0];
	return ::toupper(c);
}
#else
unsigned char TString::tolowertab[] = { 0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, TEXT(' '), TEXT('!'), TEXT('"'), TEXT('#'), TEXT('$'), TEXT('%'), TEXT('&'), 0x27, TEXT('('), TEXT(')'), TEXT('*'), TEXT('+'), TEXT(','), TEXT('-'), TEXT('.'), TEXT('/'), TEXT('0'), TEXT('1'), TEXT('2'), TEXT('3'), TEXT('4'), TEXT('5'), TEXT('6'), TEXT('7'), TEXT('8'), TEXT('9'), TEXT(':'), TEXT(';'), TEXT('<'), TEXT('='), TEXT('>'), TEXT('?'), TEXT('@'), TEXT('a'), TEXT('b'), TEXT('c'), TEXT('d'), TEXT('e'), TEXT('f'), TEXT('g'), TEXT('h'), TEXT('i'), TEXT('j'), TEXT('k'), TEXT('l'), TEXT('m'), TEXT('n'), TEXT('o'), TEXT('p'), TEXT('q'), TEXT('r'), TEXT('s'), TEXT('t'), TEXT('u'), TEXT('v'), TEXT('w'), TEXT('x'), TEXT('y'), TEXT('z'), TEXT('{'), TEXT('|'),  TEXT('}'), TEXT('~'), TEXT('_'),  TEXT('`'), TEXT('a'), TEXT('b'), TEXT('c'), TEXT('d'), TEXT('e'), TEXT('f'), TEXT('g'), TEXT('h'), TEXT('i'), TEXT('j'), TEXT('k'), TEXT('l'), TEXT('m'), TEXT('n'), TEXT('o'), TEXT('p'), TEXT('q'), TEXT('r'), TEXT('s'), TEXT('t'), TEXT('u'), TEXT('v'), TEXT('w'), TEXT('x'), TEXT('y'), TEXT('z'), TEXT('{'), TEXT('|'),  TEXT('}'), TEXT('~'), 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
unsigned char TString::touppertab[] = { 0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, TEXT(' '), TEXT('!'), TEXT('"'), TEXT('#'), TEXT('$'), TEXT('%'), TEXT('&'), 0x27, TEXT('('), TEXT(')'), TEXT('*'), TEXT('+'), TEXT(','), TEXT('-'), TEXT('.'), TEXT('/'), TEXT('0'), TEXT('1'), TEXT('2'), TEXT('3'), TEXT('4'), TEXT('5'), TEXT('6'), TEXT('7'), TEXT('8'), TEXT('9'), TEXT(':'), TEXT(';'), TEXT('<'), TEXT('='), TEXT('>'), TEXT('?'), TEXT('@'), TEXT('A'), TEXT('B'), TEXT('C'), TEXT('D'), TEXT('E'), TEXT('F'), TEXT('G'), TEXT('H'), TEXT('I'), TEXT('J'), TEXT('K'), TEXT('L'), TEXT('M'), TEXT('N'), TEXT('O'), TEXT('P'), TEXT('Q'), TEXT('R'), TEXT('S'), TEXT('T'), TEXT('U'), TEXT('V'), TEXT('W'), TEXT('X'), TEXT('Y'), TEXT('Z'), TEXT('['), TEXT('\\'), TEXT(']'), TEXT('^'), 0x5f, TEXT('`'), TEXT('A'), TEXT('B'), TEXT('C'), TEXT('D'), TEXT('E'), TEXT('F'), TEXT('G'), TEXT('H'), TEXT('I'), TEXT('J'), TEXT('K'), TEXT('L'), TEXT('M'), TEXT('N'), TEXT('O'), TEXT('P'), TEXT('Q'), TEXT('R'), TEXT('S'), TEXT('T'), TEXT('U'), TEXT('V'), TEXT('W'), TEXT('X'), TEXT('Y'), TEXT('Z'), TEXT('['), TEXT('\\'), TEXT(']'), TEXT('^'), 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
inline int TString::rfc_tolower(const int c)
{
  return tolowertab[(unsigned char)(c)];
}
inline int TString::rfc_toupper(const int c)
{
  return touppertab[(unsigned char)(c)];
}
#endif
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
int TString::match(register const TCHAR *m, register const TCHAR *n, const bool cs /* case sensitive */)
{
	const TCHAR *ma = m, *lsm = 0, *lsn = 0, *lpm = 0, *lpn = 0;
	int match = 1, saved = 0;
	register UINT sofar = 0;

#ifdef WILDT
  int space;
#endif

	/* take care of null strings (should never match) */
	if ((m == 0) || (n == 0) || (!*n))
		return NOMATCH;
	/* (!*m) test used to be here, too, but I got rid of it.  After all,
	 * If (!*n) was false, there must be a character in the name (the
	 * second string), so if the mask is empty it is a non-match.  Since
	 * the algorithm handles this correctly without testing for it here
	 * and this shouldn't be called with null masks anyway, it should be
	 * a bit faster this way */

	while (*n) {
		/* Used to test for (!*m) here, but this scheme seems to work better */
#ifdef WILDT
		if (*m == WILDT) {			/* Match >=1 space */
			space = 0;				/* Don't need any spaces */
			do {
				m++;
				space++;
			}							/* Tally 1 more space ... */
			while ((*m == WILDT) || (*m == TEXT(' ')));	/*  for each space or ~ */
			sofar += space;				/* Each counts as exact */
			while (*n == TEXT(' ')) {
				n++;
				space--;
			}							/* Do we have enough? */
			if (space <= 0)
				continue;				/* Had enough spaces! */
		}
		/* Do the fallback       */
		else {
#endif
			switch (*m) {
			case 0:
				do
					m--;			/* Search backwards */
				while ((m > ma) && (*m == TEXT('?')));	/* For first non-? TCHAR */
				if ((m > ma) ? ((*m == TEXT('*')) && (m[-1] != QUOTE)) : (*m == TEXT('*')))
					return MATCH;	/* nonquoted * = match */
				break;
			case WILDP:
				while (*(++m) == WILDP);	/* Zap redundant %s */
				if (*m != WILDS) {		/* Don't both if next=* */
					if (*n != TEXT(' ')) {	/* WILDS canTEXT('t match ') ' */
						lpm = m;
						lpn = n;		/* Save TEXT('%') fallback spot */
						saved += sofar;
						sofar = 0;		/* And save tally count */
					}
					continue;		/* Done with TEXT('%') */
				}
				/* FALL THROUGH */
			case WILDS:
				do
					m++;			/* Zap redundant wilds */
				while ((*m == WILDS) || (*m == WILDP));
				lsm = m;
				lsn = n;
				lpm = 0;		/* Save TEXT('*') fallback spot */
				match += (saved + sofar);	/* Save tally count */
				saved = sofar = 0;
				continue;		/* Done with TEXT('*') */
			case WILDQ:
				m++;
				n++;
				continue;		/* Match one TCHAR */
			case QUOTE:
				m++;			/* Handle quoting */
			}
			if (cs) { /* case sensitive */
				if (*m == *n) {		/* If matching */
					m++;
					n++;
					sofar++;
					continue;		/* Tally the match */
				}
			}
			else if (rfc_toupper(*m) == rfc_toupper(*n)) {		/* If matching */
				m++;
				n++;
				sofar++;
				continue;		/* Tally the match */
			}
#ifdef WILDT
		}
#endif
		if (lpm) {			/* Try to fallback on TEXT('%') */
			n = ++lpn;
			m = lpm;
			sofar = 0;		/* Restore position */
			if ((*n | 32) == 32)
				lpm = 0;		/* CanTEXT('t match 0 or ') ' */
			continue;			/* Next TCHAR, please */
		}
		if (lsm) {			/* Try to fallback on TEXT('*') */
			n = ++lsn;
			m = lsm;			/* Restore position */
			/* Used to test for (!*n) here but it wasnTEXT('t necessary so it')s gone */
			saved = sofar = 0;
			continue;			/* Next TCHAR, please */
		}
		return NOMATCH;		/* No fallbacks=No match */
	}
	while ((*m == WILDS) || (*m == WILDP))
		m++;			/* Zap leftover %s & *s */
	return (*m) ? NOMATCH : MATCH;	/* End of both = match */
}

TString TString::wildtok(const TCHAR *const wildString, const UINT N, const TCHAR *const sepChars) const
{
	if (sepChars == nullptr || N == 0 || this->empty())
		return TEXT("");

	const UINT nToks = this->numtok( sepChars );

	if ( N > nToks )
		return TEXT("");

	UINT m = 0;
#if TSTRING_PARTS
	for (const auto &tmp: *this)
	{
		if (match(wildString,tmp.to_chr(),false) != NOMATCH) {
			m++;
			if (m == N)
				return tmp;
		}
	}
#else
	for (TString tmp(this->getfirsttok(1, sepChars)); !tmp.empty(); tmp = this->getnexttok(sepChars))
	{
		if (match(wildString,tmp.to_chr(),false) != NOMATCH) {
			m++;
			if (m == N)
				return tmp;
		}
	}
#endif
	return TEXT("");
}

UINT TString::nwildtok(const TCHAR *const wildString, const TCHAR *const sepChars) const
{
	if ( sepChars == nullptr || this->empty() )
		return 0;

	UINT m = 0;
#if TSTRING_PARTS
	for (const auto &tmp: *this)
	{
		if (match(wildString, tmp.to_chr(), false) != NOMATCH)
			m++;
	}
	return m;
#else
	for (TString tmp(this->getfirsttok(1, sepChars)); !tmp.empty(); tmp = this->getnexttok(sepChars))
	{
		if (match(wildString,tmp.to_chr(),false) != NOMATCH)
			m++;
	}
	return m;
#endif
}

int TString::tsprintf(const TCHAR *const fmt, ...)
{
	va_list args;
	va_start( args, fmt );
	const int cnt = tvprintf(fmt, &args);
	va_end( args );
	return cnt;
}

int TString::tvprintf(const TCHAR *const fmt, va_list * args)
{
	const int cnt = ts_vscprintf(fmt, *args);
	if (cnt > 0) {
		TString tmp((UINT)(cnt + 2));

		// warning C4996: 'vsprintf' was declared deprecated
		// http://forums.microsoft.com/MSDN/ShowPost.aspx?PostID=10254&SiteID=1
		ts_vsprintf(tmp.m_pString, cnt + 1, fmt, *args);

		this->swap(tmp);
	}
	return cnt;
}

/*
 * iswm(*mask*)
 *    returns TRUE if *mask* matches string
 *    else it returns FALSE
 *    case insensitive
 */
bool TString::iswm(const TCHAR *const a) const
{
	return (match(a, this->m_pString,false) != NOMATCH);
}

/*
 * iswmcs(*mask*)
 *    returns TRUE if *mask* matches string
 *    else it returns FALSE
 *    case sensitive
 */
bool TString::iswmcs(const TCHAR *const a) const
{
	return (match(a, this->m_pString, true) != NOMATCH);
}
//int wildcmp(const TCHAR *wild, const TCHAR *string) {
//	// Written by Jack Handy - jakkhandy@hotmail.com
//	const TCHAR *cp = NULL, *mp = NULL;
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
WCHAR *TString::charToWchar(const char *const cString, size_t *const buffer_size)
{
	// try UTF8 encoded first, but error on invalid chars.
	WCHAR *res = nullptr;
	WCHAR *normRes = nullptr;
	size_t buf_size = 0;
	if (cString != nullptr) {
		try {
			int widelen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, cString, -1, nullptr, 0);
			if (widelen == 0) {
				// zero result, error maybe?
				if (GetLastError() == ERROR_NO_UNICODE_TRANSLATION) {
					// invalid chars, assume its NOT a utf8 string then, try ascii->utf16
					widelen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cString, -1, nullptr, 0);
					if (widelen != 0) {
						buf_size = TS_wgetmemsize(widelen + 1);
						res = new WCHAR[buf_size];	// TS_getmemsize() gives a much larger buffer than we really want, but it will do.
						if (MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cString, -1, res, widelen) == 0)
							res[0] = L'\0';
					}
				}
				else {
					// if no error, assume zero length string.
					buf_size = TS_wgetmemsize(1);
					res = new WCHAR[buf_size];
					res[0] = L'\0';
				}
			}
			else {
				buf_size = TS_wgetmemsize(widelen + 1);
				res = new WCHAR[buf_size];
				if (MultiByteToWideChar(CP_UTF8, 0, cString, -1, res, widelen) == 0)
					res[0] = L'\0';
			}
			// NB: NormalizeString() is Vista+ ONLY
			int normLen = NormalizeString(NormalizationC, res, -1, nullptr, 0);
			if (normLen > 0) {
				normLen = TS_wgetmemsize(normLen + 1);
				normRes = new WCHAR[normLen];
				if (NormalizeString(NormalizationC, res, -1, normRes, normLen) > 0)
				{
					delete[] res;
					res = normRes;
				}
				else
					delete[] normRes;
			}
		}
		catch (std::bad_alloc)
		{
			delete[] res;
			delete[] normRes;
			res = nullptr;
		}
	}
	if (buffer_size != nullptr)
		*buffer_size = (buf_size *sizeof(WCHAR));

	return res;
}

/*!
* \brief blah
*
* Convert a utf16 string to a utf8 string
*/
char *TString::WcharTochar(const WCHAR *const wString, size_t *const buffer_size)
{
	const int l = WideCharToMultiByte(CP_UTF8, 0, wString, -1, nullptr, 0, nullptr, nullptr);
	char *res = nullptr;
	size_t buf_size = 0;

	if (l > 0) {
		buf_size = TS_getmemsize(l + 1);
		res = new char[buf_size];
		if (WideCharToMultiByte(CP_UTF8, 0, wString, -1, res, l, nullptr, nullptr) == 0)
			res[0] = '\0';
	}
	if (buffer_size != nullptr)
		*buffer_size = buf_size;

	return res;
}

/*!
 * \brief blah
 *
 * blah
 */
TString &TString::strip()
{
	if (this->empty())
		return *this;

	const TCHAR *start = this->m_pString;
	const TCHAR *end = this->m_pString + this->len();

	// Trim from start
	while (start != end && *start == TEXT(' '))
		start++;

	// Trim from end
	while (end != start && *(--end) == TEXT(' '));

	const size_t new_len = (end - start) + 1;	// add one to take into account the previous decrement

	TString tmp((UINT)new_len + 1);				// add one for zero byte.

	// now strip all ctrl codes.
	const TCHAR *const wtxt = start;
	TCHAR *p = tmp.m_pString;
	size_t pos = 0;

	// strip out ctrl codes to correctly position text.
	for (TCHAR c = wtxt[pos]; pos < new_len; c = wtxt[++pos]) {
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
					pos++; // remove multiple consecutive ctrl-k's

				if (wtxt[pos + 1] >= TEXT('0') && wtxt[pos + 1] <= TEXT('9')) {
					++pos;

					if (wtxt[pos + 1] >= TEXT('0') && wtxt[pos + 1] <= TEXT('9'))
						pos++;

					// maybe a background color
					if (wtxt[pos + 1] == TEXT(',')) {
						++pos;

						if (wtxt[pos + 1] >= TEXT('0') && wtxt[pos + 1] <= TEXT('9')) {
							pos++;

							if (wtxt[pos + 1] >= TEXT('0') && wtxt[pos + 1] <= TEXT('9'))
								++pos;
						}
					}
				}
			}
			break;
		case 32:	// space character
		{
				   while (wtxt[pos + 1] == 32)
					   pos++; // remove multiple consecutive spaces
				   // fall through to save first space
		}
		default:
			*p++ = c;
			break;
		}
	}

	this->swap(tmp);

	return *this;
}

// swap function taken from: http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
void TString::swap(TString &second) // nothrow noexcept
{
	// enable ADL (not necessary in our case, but good practice)
	using std::swap;

	// NB: 'second' is expected to be disposed of after the swap, not used, some mods are needed to make it fully usable when using internal buffers.

	// by swapping the members of two classes,
	// the two classes are effectively swapped
	swap(this->m_pString, second.m_pString);
	swap(this->m_savedpos, second.m_savedpos);
	swap(this->m_bUsingInternal, second.m_bUsingInternal);
	swap(this->m_pTempString, second.m_pTempString);
	swap(this->m_buffersize, second.m_buffersize);
	swap(this->m_savedcurrenttok, second.m_savedcurrenttok);
	swap(this->m_savedtotaltoks, second.m_savedtotaltoks);
#ifdef TSTRING_PARTS
	swap(this->m_SplitParts, second.m_SplitParts);
#endif
	swap(this->m_bDirty, second.m_bDirty);
	swap(this->m_iLen, second.m_iLen);

#if TSTRING_INTERNALBUFFER
	if (this->m_bUsingInternal)
	{
		ts_copymem(m_InternalBuffer, second.m_InternalBuffer, TSTRING_INTERNALBUFFERSIZE_BYTES);

		this->m_pString = m_InternalBuffer;
		// make savedpos point to the same offset within our buffer...
		if (this->m_savedpos != nullptr)
			this->m_savedpos = (this->m_pString + (this->m_savedpos - second.m_InternalBuffer));
	}
	if (second.m_bUsingInternal)
	{
		second.m_pString = second.m_InternalBuffer;
		//if (second.m_savedpos != nullptr)
		//	second.m_savedpos = (second.m_pString + (second.m_savedpos - this->m_InternalBuffer));
		second.m_savedpos = nullptr;
		second.m_iLen = 0;
		second.m_bDirty = false;
		second.m_InternalBuffer[0] = TEXT('\0');
	}
#endif
}

void TString::copy(TString other) // <- copy made here, so just swap them
{
	this->swap(other);
}

int TString::compare(const TString &other) const
{
	if (this->m_pString == nullptr)
		return (other.m_pString == nullptr) ? 0 : 1;

	if (other.m_pString == nullptr)
		return -1;

	return (ts_strcmp(this->m_pString, other.m_pString));
}

ULONG TString::to_addr() const
{
	BYTE first, second, third, forth;

	first = (BYTE)(this->getfirsttok(1, TEXT(".")).to_int() & 0xFF);
	second = (BYTE)(this->getnexttok(TEXT(".")).to_int() & 0xFF);
	third = (BYTE)(this->getnexttok(TEXT(".")).to_int() & 0xFF);
	forth = (BYTE)(this->getnexttok(TEXT(".")).to_int() & 0xFF);

	//return MAKELONG(MAKEWORD(first,second),MAKEWORD(third,forth));
	//return MAKELONG(MAKEWORD(forth, third), MAKEWORD(second, first));
	return MAKEIPADDRESS(first, second, third, forth);
}

void TString::clear(void)
{
//	if (this->m_pString == nullptr)
//		this->m_pString = allocstr_bytes(1);
//
//	delete[] this->m_pTempString;
//
//	this->m_pTempString = nullptr;
//	this->m_savedcurrenttok = 0;
//	this->m_savedpos = nullptr;
//	this->m_savedtotaltoks = 0;
//
//#ifdef TSTRING_PARTS
//	this->m_SplitParts.clear();
//#endif
//
//	ts_zeromem(this->m_pString, this->m_buffersize);

	// this method differs from above in that it does free the memory.
	TString tmp;
	this->swap(tmp);
}

void TString::shrink_to_fit(void)
{
	//const size_t l = this->len();
	//
	//if (m_buffersize <= TS_getmemsize(l * sizeof(TCHAR)))
	//	return;
	//
	//TString tmp((UINT)l + 1);
	//
	//if (l > 0) {
	//	ts_strcpyn_throw(tmp.m_pString, this->m_pString, l + 1);
	//}
	//
	//this->swap(tmp);

	// this does the same as above, but is neater
	TString tmp(this->m_pString);
	this->swap(tmp);
}

void TString::reserve(const size_t tsSize)
{
	// check if buffer is already big enough
	if (tsSize <= (m_buffersize / sizeof(TCHAR)))
		return;

	TString tmp((UINT)tsSize);
	ts_strcpy_throw(tmp.m_pString, this->m_pString);
	this->swap(tmp);
}

TString &TString::append(const WCHAR *const cString)
{
#ifdef UNICODE
	if (cString != nullptr)
		append(cString, ts_strlen(cString));

	return *this;
#else
	const TString tmp(cString);
	return append(tmp);
#endif
}

TString &TString::append(const char *const cString)
{
#ifdef UNICODE
	const TString tmp(cString);
	return append(tmp);
#else
	if (cString != nullptr)
		append(cString, ts_strlen(cString));

	return *this;

#endif

}

TString &TString::append(const TString &tString)
{
	return append(tString.m_pString, tString.len());
}

TString &TString::append(const TCHAR *const cString, const size_t iChars)
{
	if ((cString != nullptr) && (iChars != 0))
	{
		const size_t sz = this->len();
		const size_t l = (sz + iChars + 1);	// plus 1 for zero byte.

		if ((l * sizeof(TCHAR)) <= m_buffersize) {
			// new text fits within existing buffer, so append.
			ts_strncat_throw(this->m_pString, cString, iChars);

			m_bDirty = true;
		}
		else {
			TString tmp((UINT)l);

			if (sz > 0) {
				ts_strcpyn_throw(tmp.m_pString, this->m_pString, l);
			}
			ts_strncat_throw(tmp.m_pString, cString, iChars);

			this->swap(tmp);
		}
	}
	return *this;
}

#ifdef TSTRING_PARTS
size_t TString::split(const TCHAR *const sepChars) const
{
	m_SplitParts.clear();

	if (sepChars == nullptr || this->empty() || (sepChars[0] == TEXT('\0')))
		return 0;

	TCHAR * p_cStart = this->m_pString, *p_cEnd = this->m_pString;
	size_t iCount = 0;
	const int sepl = ts_strlen(sepChars);

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != nullptr) {
		iCount++;
		m_SplitParts.push_back(TString(p_cStart, p_cEnd));

		p_cStart = p_cEnd + sepl;
	}
	if (p_cStart[0] != TEXT('\0'))
	{
		m_SplitParts.push_back(TString(p_cStart));
		iCount++;
	}
	return iCount;
}

const TString &TString::part(const size_t N) const
{
	static const TString tsNull;

	if (m_SplitParts.empty())
		return tsNull;

	return m_SplitParts[N];
}

const TStringList &TString::parts(const TCHAR *const sepChars) const
{
	if (m_SplitParts.empty())
		split(sepChars);
	return m_SplitParts;
}

TString::iterator TString::begin()
{
	parts();
	return m_SplitParts.begin();
}

TString::iterator TString::begin(const TCHAR *const sepChars)
{
	parts(sepChars);
	return m_SplitParts.begin();
}

TString::iterator TString::end()
{
	return m_SplitParts.end();
}

const TString::const_iterator TString::begin() const
{
	return parts().begin();
}

const TString::const_iterator TString::begin(const TCHAR *const sepChars) const
{
	parts(sepChars);
	return m_SplitParts.cbegin();
}

const TString::const_iterator TString::end() const
{
	return m_SplitParts.end();
}
#endif

//#ifdef TSTRING_PARTS
//inline TString::iterator begin(TString& collection)
//{
//	return TString::iterator(collection);
//}
//
//inline TString::iterator end(TString &collection)
//{
//	return TString::iterator(collection);
//}
//TString::iterator TString::begin(TCHAR *sepChars)
//{
//	return TString::iterator(this, sepChars);
//}
//
//const TString::const_iterator TString::begin() const
//{
//	return parts().begin();
//}
//
//const TString::const_iterator TString::begin() const
//{
//	return parts().begin();
//}
//
//const TString::const_iterator TString::end() const
//{
//	return m_SplitParts.end();
//}
//#endif

#if _MSC_VER > 1800
// literal operator
// allows "sometext"ts to be interpreted as TString("sometext")
TString operator"" ts(const char *p, size_t)
{
	return TString{ p };
}
#endif

//#ifdef TSTRING_TEMPLATES
//template <class T>
//TString &operator >>(TString &ts, T &N) const { N = ts.to_<T>(); return ts; }
//#endif
