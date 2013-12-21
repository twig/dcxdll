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
 *		changed findtok() to take an unsigned int arg.
 *		changed deltok() to take an unsigned int arg.
 *		changed instok() to take an unsigned arg.
 *		and loads more...
 *
 * © ScriptsDB.org - 2005
 */
#include "defines.h"
// Required for VS 2005
#define _CRT_SECURE_NO_DEPRECATE 1
// end VS2005
// for VS2012
#define _CRT_SECURE_NO_WARNINGS 1
// end VS2012

#include <windows.h>

#include "tstring.h"

const TCHAR *TString::m_cSpace = TEXT(" ");
const TCHAR *TString::m_cComma = TEXT(",");
const TCHAR *TString::m_cTab = TEXT("\t");

/*! 
 * \brief Default Constructor
 */
/****************************/

TString::TString( )
: m_pTempString(NULL),
 m_pString(NULL),
 m_savedtotaltoks(0)
{
	this->m_pString = allocstr_cch(1);
	this->m_pString[0] = 0;
}

/****************************/
/*! \fn TString::TString( const WCHAR * cString )
    \brief Constructor input from C WCHAR string

    \param cString Character string to initialize string to
*/
/****************************/

TString::TString( const char * cString )
: m_pTempString(NULL),
 m_pString(NULL),
 m_savedtotaltoks(0)
{
#ifdef UNICODE
	TCHAR *temp = NULL;

	if (cString != NULL)
		temp = charToWchar(cString, &m_buffersize);

	if (temp == NULL) {
		this->m_pString = allocstr_cch( 1 );
		this->m_pString[0] = TEXT('\0');
	}
	else
		this->m_pString = temp;	// no need to copy
#else
	if ( cString != NULL ) {
		const int l = ts_strlen( cString ) + 1;
		this->m_pString = allocstr_cch( l );
		ts_strcpyn( this->m_pString, cString, l );
	}
	else {
		this->m_pString = allocstr_cch( 1 );
		this->m_pString[0] = 0;
	}
#endif
}

TString::TString( const WCHAR * cString )
: m_pTempString(NULL),
 m_pString(NULL),
 m_savedtotaltoks(0)
{
#ifdef UNICODE
	if (cString != NULL) {
		const int l = ts_strlen(cString) + 1;
		this->m_pString = allocstr_cch(l);
		ts_strcpyn(this->m_pString, cString, l);
	}
	else {
		this->m_pString = allocstr_cch(1);
		this->m_pString[0] = 0;
	}
#else
	TCHAR *temp = NULL;

	if (cString != NULL)
		temp = WcharTochar(cString, &m_buffersize);

	if (temp == NULL) {
		this->m_pString = allocstr_cch( 1 );
		this->m_pString[0] = TEXT('\0');
	}
	else
		this->m_pString = temp;
#endif
}

/****************************/
/*! \fn TString::TString( const TCHAR chr )
    \brief Constructor input from C TCHAR

    \param chr Character to initialize string to
*/
/****************************/

TString::TString( const WCHAR chr )
: m_pTempString(NULL),
 m_pString(NULL),
 m_savedtotaltoks(0)
{
#ifdef UNICODE
	this->m_pString = allocstr_cch(2);
	this->m_pString[0] = chr;
	this->m_pString[1] = TEXT('\0');
#else
	char *temp = NULL;

	if (chr != L'\0')
	{
		WCHAR wStr[2];
		wStr[0] = chr;
		wStr[1] = L'\0';

		temp = this->WcharTochar(wStr, &m_buffersize);
	}

	if (temp == NULL)
	{
		this->m_pString = allocstr_cch(1);
		this->m_pString[0] = 0;
	}
	else
		this->m_pString = temp;
#endif
}

TString::TString( const char chr )
: m_pTempString(NULL),
 m_pString(NULL),
 m_savedtotaltoks(0)
{
#ifdef UNICODE
	TCHAR *temp = NULL;

	if (chr != '\0') {
		char cString[2];
		cString[0] = chr;
		cString[1] = '\0';

		temp = charToWchar(cString, &m_buffersize);
	}

	if (temp == NULL) {
		this->m_pString = allocstr_cch(1);
		this->m_pString[0] = TEXT('\0');
	}
	else
		this->m_pString = temp;
#else
	this->m_pString = allocstr_cch(2);
	this->m_pString[0] = chr;
	this->m_pString[1] = 0;
#endif
}

/****************************/
/*! \fn TString::TString( const TString & tString )
    \brief Copy Constructor

    \param tString TString object to copy
*/
/****************************/

TString::TString( const TString & tString )
: m_pTempString(NULL),
 m_pString(NULL),
 m_savedtotaltoks(0)
{
	if (tString.m_pString != NULL) {
		//const int l = ts_strlen( tString.m_pString ) +1;
		//this->m_pString = allocstr_cch( l );
		//ts_strcpyn( this->m_pString, tString.m_pString, l );

		// copy full buffer from source TString object, this buffer supplies any zero byte.
		this->m_pString = allocstr_bytes(tString.m_buffersize);
		CopyMemory(this->m_pString, tString.m_pString, tString.m_buffersize);
	}
	else {
		this->m_pString = allocstr_cch(1);
		this->m_pString[0] = 0;
	}
}

TString::TString( const TCHAR *pStart, const TCHAR *pEnd )
: m_pTempString(NULL),
 m_pString(NULL),
 m_savedtotaltoks(0)
{
	if ((pStart != NULL) && (pEnd != NULL) && (pEnd > pStart)) {
		const size_t size = (pEnd - pStart);
		this->m_pString = allocstr_cch(size + 1);
		ts_strcpyn(this->m_pString, pStart, size);
		//CopyMemory(this->m_pString, pStart, size*sizeof(TCHAR));
		//this->m_pString[size] = TEXT('\0');
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
TString::TString( const unsigned int tsSize )
: m_pTempString(NULL),
 m_pString(NULL),
 m_savedtotaltoks(0)
{
	this->m_pString = allocstr_cch(tsSize + 1);
	ZeroMemory(this->m_pString, ((tsSize + 1)*sizeof(TCHAR)));
}

TString::~TString( ) {

	this->deleteString( );
}

/*!
 * \brief Deletes the allocated TCHAR buffer
 */
void TString::deleteString(const bool bKeepBufferSize) {

	if ( this->m_pString != NULL ) {
		delete [] this->m_pString; 
		this->m_pString = NULL;
	}
	this->deleteWString(bKeepBufferSize);
}
void TString::deleteWString(const bool bKeepBufferSize) {

	if ( this->m_pTempString != NULL ) {
		delete [] this->m_pTempString; 
		this->m_pTempString = NULL;
	}
	this->m_savedtotaltoks = 0;
	if (!bKeepBufferSize)
		this->m_buffersize = 0;
}

/****************************/
TString& TString::operator =( const TString & tString ) {

	this->deleteString( );

	if (tString.m_pString != NULL) {
		this->m_pString = allocstr_bytes(tString.m_buffersize);
		CopyMemory(this->m_pString, tString.m_pString, m_buffersize);
	}
	return *this;
}

/****************************/
/*! \fn void TString::operator=( const TCHAR * string )
    \brief Symbol Overload - TString assignement from C TCHAR string

    \param cString Character string to assign string to
*/
/****************************/
TString& TString::operator =( const WCHAR * cString ) {
#ifdef UNICODE
	this->deleteString();

	if (cString != NULL) {
		const int l = ts_strlen(cString) + 1;
		this->m_pString = allocstr_cch(l);
		ts_strcpyn(this->m_pString, cString, l);
	}
	return *this;
#else
	this->deleteString( );

	if ( cString != NULL )
		this->m_pString = WcharTochar( cString, &m_buffersize);

	return *this;
#endif
}
/****************************/
TString& TString::operator =( const char * cString ) {
#ifdef UNICODE
	this->deleteString();

	if (cString != NULL)
		this->m_pString = charToWchar(cString, &m_buffersize);

	return *this;
#else
	this->deleteString( );

	if ( cString != NULL ) {
		const int l = ts_strlen(cString) + 1;
		this->m_pString = allocstr_cch(l);
		ts_strcpyn(this->m_pString, cString, l);
	}
	return *this;
#endif
}

/****************************/
/*! \fn void TString::operator=( const TCHAR chr )
    \brief Symbol Overload - TString assignement from C TCHAR

    \param chr Character to assign string to
*/
/****************************/
TString& TString::operator =( const WCHAR chr ) {
#ifdef UNICODE
	this->deleteString();

	this->m_pString = allocstr_cch(2);
	this->m_pString[0] = chr;
	this->m_pString[1] = TEXT('\0');

	return *this;
#else
	//this->deleteString( );

	WCHAR wStr[2];
	wStr[0] = chr;
	wStr[1] = L'\0';

	*this = wStr;

	//TCHAR *temp = WcharTochar( wStr, &m_buffersize );

	//if (temp != NULL) {
	//	this->m_pString = allocstr_cch( 2 );

	//	this->m_pString[0] = temp[0];
	//	this->m_pString[1] = TEXT('\0');

	//	delete [] temp;
	//}
	//else {
	//	this->m_pString = allocstr_cch( 1 );
	//	this->m_pString[0] = TEXT('\0');
	//}
	return *this;
#endif
}

/****************************/
TString& TString::operator =( const char chr ) {
#ifdef UNICODE
	//this->deleteString();
	char str[2];
	str[0] = chr;
	str[1] = 0;

	*this = str;

	//TCHAR *temp = charToWchar(str, &m_buffersize);

	//if (temp != NULL) {
	//	this->m_pString = allocstr_cch( 2 );

	//	this->m_pString[0] = temp[0];
	//	this->m_pString[1] = TEXT('\0');

	//	delete [] temp;
	//}
	//else {
	//	this->m_pString = allocstr_cch( 1 );
	//	this->m_pString[0] = 0;
	//}

	return *this;
#else
	this->deleteString();

	this->m_pString = allocstr_cch(2);
	this->m_pString[0] = chr;
	this->m_pString[1] = 0;

	return *this;
#endif
}

/****************************/
/*! \fn TString TString::operator +( const TCHAR * string )
    \brief Symbol Overload - TString and TCHAR string concatenation

    \param cString Character string to concatenate

    \return Returns the new TString object
*/
/****************************/

TString TString::operator +( const TCHAR * cString ) {

	if (cString != NULL) {
		const size_t l = (ts_strlen(this->m_pString) + ts_strlen(cString) + 1);
		TString newTString((UINT)l);

		ts_strcpyn(newTString.m_pString, this->m_pString, l);
		ts_strcat(newTString.m_pString, cString);
		return newTString;
	}
	return *this; // return an object not a ref or pointer to object, so object is copied on return.
}

/****************************/
/*! \fn TString TString::operator +( const TCHAR chr )
    \brief Symbol Overload - TString and TCHAR concatenation

    \param chr Character to concatenate

    \return Returns the new TString object
*/
/****************************/

TString TString::operator +( const TCHAR chr ) {

	const size_t size = ts_strlen(this->m_pString);
	TString newTString((UINT)(size + 2));

	ts_strcpyn(newTString.m_pString, this->m_pString, size + 1);
	newTString.m_pString[size] = chr;
	newTString.m_pString[size + 1] = 0;

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

	if (tString.m_pString) {
		const size_t l = (ts_strlen(this->m_pString) + ts_strlen(tString.m_pString) + 1);	// we could use the 2x m_buffersize instead, but this could lead to a very large unneeded buffer.
		TString newTString((UINT)l);

		ts_strcpyn(newTString.m_pString, this->m_pString, l);
		ts_strcat(newTString.m_pString, tString.m_pString);
		return newTString;
	}
	return *this;
}

/****************************/
/*! \fn TString TString::operator -( const TCHAR * string )
    \brief Symbol Overload - remove TCHAR string from TString

    \param cString Character string to remove

    \return Returns the new TString object
*/
/****************************/

TString TString::operator -( const TCHAR * cString ) {

	if ( cString != NULL ) {
		TString newTString(*this);
		newTString.i_remove(cString);
		return newTString;
	}
	return *this;
}

TString TString::operator -( const TCHAR chr) {

	if ( chr == 0 ) return *this;

	TString newTString(*this);
	TCHAR str[2];
	str[0] = chr;
	str[1] = 0;
	newTString.i_remove(str);
	return newTString;
}

TString TString::operator -( const TString & tString ) {

	if ( tString.m_pString != NULL ) {
		TString newTString(*this);
		newTString.i_remove(tString.m_pString);
		return newTString;
	}
	return *this;
}

/****************************/
/*! \fn void TString::operator +=( const TCHAR * string )
    \brief Symbol Overload - TString and TCHAR string concatenation

    \param cString Character string to concatenate
*/
/****************************/

TString & TString::operator +=( const TCHAR * cString ) {

	if (cString != NULL) {
		const size_t l = (ts_strlen(this->m_pString) + ts_strlen(cString) + 1);

		if ((l * sizeof(TCHAR)) <= m_buffersize) {
			// new text fits within existing buffer, so append.
			ts_strcat(this->m_pString, cString);
		}
		else {
			TCHAR * temp = allocstr_cch(l);

			ts_strcpyn(temp, this->m_pString, l);
			ts_strcat(temp, cString);

			this->deleteString(true);
			this->m_pString = temp;
		}
	}
	return *this;
}

/****************************/
/*! \fn void TString::operator +=( const TCHAR chr )
    \brief Symbol Overload - TString and TCHAR concatenation

    \param chr Character to concatenate
*/
/****************************/

TString & TString::operator +=( const TCHAR chr ) {

	if (this->m_pString != NULL) {
		const size_t len = ts_strlen(this->m_pString);

		if (((len + 1) * sizeof(TCHAR)) < m_buffersize) {
			this->m_pString[len] = chr;
			this->m_pString[len + 1] = TEXT('\0');
		}
		else {
			TCHAR * temp = allocstr_cch(len + 2);

			ts_strcpyn(temp, this->m_pString, len + 1);
			temp[len] = chr;
			temp[len + 1] = TEXT('\0');

			this->deleteString(true);
			this->m_pString = temp;
		}
	}
	else {
		this->m_pString = allocstr_cch(2);
		this->m_pString[0] = chr;
		this->m_pString[1] = 0;
	}
	return *this;
}

/****************************/
/*! \fn void TString::operator +=( const TString & tString )
    \brief Symbol Overload - TString and TString concatenation

    \param tString TString object to concatenate
*/
/****************************/

TString & TString::operator +=( const TString & tString ) {

	if (tString.m_pString) {
		const size_t l = (ts_strlen(this->m_pString) + ts_strlen(tString.m_pString) + 1);

		if ((l * sizeof(TCHAR)) <= m_buffersize) {
			ts_strcat(this->m_pString, tString.m_pString);
		}
		else {
			TCHAR * temp = allocstr_cch(l);

			ts_strcpyn(temp, this->m_pString, l);
			ts_strcat(temp, tString.m_pString);

			this->deleteString(true);
			this->m_pString = temp;
		}
	}
	return *this;
}

/****************************/
/*! \fn void TString::operator -=( const TCHAR * string )
\brief Symbol Overload - remove TCHAR string from TString

\param cString Character string to remove
*/
/****************************/

TString & TString::operator -=( const TCHAR * cString ) {

	if ( cString != NULL )
		this->i_remove(cString);

	return *this;
}

TString & TString::operator -=( const TCHAR chr ) {

	if ( chr != 0 ) {
		TCHAR str[2];
		str[0] = chr;
		str[1] = 0;
		this->i_remove(str);
	}

	return *this;
}

TString & TString::operator -=( const TString &tString ) {

	if ( tString.m_pString != NULL )
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

	if ( !this->m_pString && !iNull )
		return true;

	return false;
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

bool TString::operator ==( const TCHAR * cString ) const {

	if ( cString && this->m_pString ) {
		if ( ts_strcmp( this->m_pString, cString ) == 0)
			return true;
	}
	return false;
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

bool TString::operator ==( const TCHAR chr ) const {

	if ( this->m_pString ) {

		TCHAR temp[2];
		temp[0] = chr;
		temp[1] = 0;

		if ( ts_strcmp( this->m_pString, temp ) == 0)
			return true;
	}
	return false;
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

bool TString::operator ==( const TString & tString ) const {

	if ( this->m_pString && tString.m_pString ) {
		if ( ts_strcmp( this->m_pString, tString.m_pString ) == 0)
			return true;
	}
	return false;
}

/****************************/
/*! \fn bool TString::operator !=( const int iNull )
    \brief Symbol Overload - TString and NULL comparison

    \param iNull Value of comparison (usually 0 for NULL)

    \return Returns \b true or \b false based on the comparison
*/

bool TString::operator !=( const int iNull ) const {

	if ( this->m_pString && !iNull )
		return true;

	return false;
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

bool TString::operator !=( const TCHAR * cString ) const {

	if ( cString && this->m_pString ) {

		if ( ts_strcmp( this->m_pString, cString ) != 0 )
			return true;
	}
	return false;
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

bool TString::operator !=( const TCHAR chr ) const {

	if ( this->m_pString != NULL ) {

		TCHAR temp[2];
		temp[0] = chr;
		temp[1] = 0;

		if ( ts_strcmp( this->m_pString, temp ) != 0 )
			return true;
	}
	return false;
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

bool TString::operator !=( const TString & tString ) const {

	if ( this->m_pString != NULL && tString.m_pString != NULL ) {

		if ( ts_strcmp( this->m_pString, tString.m_pString ) != 0 )
			return true;
	}
	return false;
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

bool TString::operator >( const TCHAR * cString ) const {

	if ( cString != NULL && this->m_pString != NULL ) {
		if ( ts_strcmp( this->m_pString, cString ) > 0 )
			return true;
	}
	return false;
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

	if ( this->m_pString != NULL ) {

		TCHAR temp[2];
		temp[0] = chr;
		temp[1] = 0;

		if ( ts_strcmp( this->m_pString, temp ) > 0 )
			return true;
	}
	return false;
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

bool TString::operator >( const TString & tString ) const {

	if ( this->m_pString != NULL && tString.m_pString != NULL ) {

		if ( ts_strcmp( this->m_pString, tString.m_pString ) > 0 )
			return true;
	}
	return false;
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

bool TString::operator >=( const TCHAR * cString ) const {

	if ( cString != NULL && this->m_pString != NULL ) {

		if ( ts_strcmp( this->m_pString, cString ) >= 0 )
			return true;
	}
	return false;
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

	if ( this->m_pString != NULL ) {

		TCHAR temp[2];
		temp[0] = chr;
		temp[1] = 0;

		if ( ts_strcmp( this->m_pString, temp ) >= 0 )
			return true;
	}
	return false;
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

bool TString::operator >=( const TString & tString ) const {

	if ( this->m_pString != NULL && tString.m_pString != NULL ) {

		if ( ts_strcmp( this->m_pString, tString.m_pString ) >= 0 )
			return true;
	}
	return false;
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

bool TString::operator <( const TCHAR * cString ) const {

	if ( cString != NULL && this->m_pString != NULL ) {

		if ( ts_strcmp( this->m_pString, cString ) < 0 )
			return true;
	}
	return false;
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

	if ( this->m_pString != NULL ) {

		TCHAR temp[2];
		temp[0] = chr;
		temp[1] = 0;

		if ( ts_strcmp( this->m_pString, temp ) < 0 )
			return true;
	}
	return false;
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

bool TString::operator <( const TString & tString ) const {

	if ( this->m_pString != NULL && tString.m_pString != NULL ) {

		if ( ts_strcmp( this->m_pString, tString.m_pString ) < 0 )
			return true;
	}
	return false;
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

bool TString::operator <=( const TCHAR * cString ) const {

	if ( cString != NULL && this->m_pString != NULL ) {

		if ( ts_strcmp( this->m_pString, cString ) <= 0 )
			return true;
	}
	return false;
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

	if ( this->m_pString != NULL ) {

		TCHAR temp[2];
		temp[0] = chr;
		temp[1] = 0;

		if ( ts_strcmp( this->m_pString, temp ) <= 0 )
			return true;
	}
	return false;
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

bool TString::operator <=( const TString & tString ) const {

	if ( this->m_pString != NULL && tString.m_pString != NULL ) {

		if ( ts_strcmp( this->m_pString, tString.m_pString ) <= 0 )
			return true;
	}
	return false;
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

	if (N == 0)
		return TEXT("");

	TString tmp((UINT)((ts_strlen(this->m_pString) *N) + 1));

	for (int i = 0; i < N; i++)
		ts_strcat(tmp.m_pString, this->m_pString);

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
		this->deleteString();
		return *this;
	}

	// if string is null just return.
	if ((N == 1) || (this->m_pString == NULL))
		return *this;
	const size_t l = (ts_strlen(this->m_pString)*N) + 1;
	TCHAR *temp = this->m_pString;

	this->m_pString = allocstr_cch(l);
	this->m_pString[0] = 0;

	for (int i = 0; i < N; i++)
		ts_strcat(this->m_pString, temp);
	this->m_savedtotaltoks = 0;

	delete[] temp;

	return *this;
}

/****************************/
/*! \fn TCHAR & TString::operator []( long int N )
    \brief Symbol Overload - Array Subscript

    \param N Character index

    \return Returns the Nth TCHAR of string

    \note > Index starts at \b 0 \n
          > Index can be negative, \b -1 being last character
*/
/****************************/

TCHAR & TString::operator []( long int N ) const {

	static TCHAR chr = TEXT('\0');

	if (this->m_pString) {
		long int size = (long int)ts_strlen(this->m_pString);
		if (N < 0) N += size;

		if ((N >(size - 1)) || (N < 0))
			return chr;

		return this->m_pString[N];
	}
	return chr;
}

/****************************/
/*! \fn size_t TString::len( )
    \brief Returns string length

    \return String length
*/

size_t TString::len( ) const {
	if (this->m_pString)
		return (size_t)ts_strlen(this->m_pString);

	return 0;
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

int TString::find( const TCHAR * substring, const int N ) const {

	if ((substring != NULL) && (this->m_pString != NULL)) {

		TCHAR * temp = NULL, *temp2 = this->m_pString;

		int i = 0;
		const size_t subl = ts_strlen(substring); // Ook
		while ( ( temp = ts_strstr( temp2, substring ) ) != NULL ) {
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
size_t TString::findtok(const TCHAR * cToken, const unsigned int N, const TCHAR * sepChars) const {
	size_t count = 0;
	const unsigned int nToks = this->numtok(sepChars);

	this->getfirsttok( 0 );

	for (unsigned int i = 1; i <= nToks; i++) {
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

	TString newTString;

	if (this->m_pString) {

		const int size = (int)ts_strlen(this->m_pString);

		if (N < 0) N += size;

		if (N > size - 1 || N < 0)
			return newTString;

		if (N + M > size)
			M = size - N;

		delete[] newTString.m_pString; // change by Ook

		newTString.m_pString = newTString.allocstr_cch(M + 1);
		newTString.m_pString[0] = 0;

		TCHAR * temp = this->m_pString;
		temp += N;

		ts_strcpyn(newTString.m_pString, temp, M + 1);
		newTString.m_pString[M] = 0;
	}
	return newTString;
}

/*
internal remove function, called by the other remove()/-/-= functions.
Ook
*/

// NB: This needs looked at!!!! doesnt remove all instances of substring correctly!
int TString::i_remove(const TCHAR *subString)
{
	// no str to remove or no str to remove from
	if ((subString == NULL) || (this->m_pString == NULL))
		return 0;

	TCHAR *sub, *p = this->m_pString;
	int cnt = 0;
	const int subl = ts_strlen(subString), ol = ts_strlen(p);
	// if length of sub string or old string is 0 return 0
	// may change this.
	if ((subl == 0) || (ol == 0))
		return 0;
	// allocate new string
	const size_t l = TS_getmemsize(ol + 1);
	PTCHAR tmp = new TCHAR[l];
	while ((sub = ts_strstr(p, subString)) != NULL) {
		ts_strncat(tmp, p, (sub - p)); // copy bit before substring. if any.
		cnt++;
		p = sub + subl; // update pointer to skip substring.
	}

	if (cnt > 0) {
		ts_strcat(tmp, p); // append the end text, if any.
		this->deleteString(); // delete old string
		this->m_pString = tmp; // save new one.
		this->m_buffersize = l;
	}
	else delete[] tmp;

	return cnt;
}

/*
	internal replace function, called by the other replace() functions.
	Ook
*/
int TString::i_replace(const TCHAR *subString, const TCHAR *rString)
{
	if ((subString == NULL) || (rString == NULL) || (this->m_pString == NULL))
		return 0;

	TCHAR *sub, *p = this->m_pString;
	int cnt = 0;
	const int subl = ts_strlen(subString), repl = ts_strlen(rString), ol = ts_strlen(p);
	// if length of sub string or old string is 0 return 0
	// may change this.
	if ((subl == 0) || (ol == 0)) return 0;
	// see if we have any matches & how many.
	while ((sub = ts_strstr(p, subString)) != NULL) {
		cnt++;
		p = sub + subl;
	}
	// make final string if we have any matches.
	if (cnt > 0) {
		p = this->m_pString;
		TCHAR *out = allocstr_cch((cnt * repl) + (ol - (cnt * subl)) + 1); // allocate new string.
		out[0] = 0;
		while ((sub = ts_strstr(p, subString)) != NULL) {
			ts_strncat(out, p, (sub - p)); // copy bit before substring. if any.
			ts_strcat(out,rString); // append new replacement string.
			p = sub + subl; // update pointer to skip substring.
		}
		ts_strcat(out, p); // append the end text, if any.
		this->deleteString(true); // delete old string
		this->m_pString = out; // save new one.
	}
	return cnt;
}

/****************************/
/*! \fn int TString::replace( const TCHAR * substring, const TCHAR * replace )
    \brief blah!
*/
/****************************/

int TString::replace( const TCHAR * subString, const TCHAR * rString ) {
	return this->i_replace(subString,rString);
}

/****************************/
/*! \fn int TString::replace( const TCHAR * substring, const TCHAR rchr )
    \brief blah!
*/
/****************************/

int TString::replace( const TCHAR * subString, const TCHAR rchr ) {

	TCHAR tmp[2];
	tmp[0] = rchr;
	tmp[1] = 0;
	return this->i_replace(subString,tmp);
}

/****************************/
/*! \fn int TString::replace( const TCHAR chr, const TCHAR * replace )
    \brief blah!
*/
/****************************/

int TString::replace( const TCHAR chr, const TCHAR * rString ) {

	TCHAR tmp[2];
	tmp[0] = chr;
	tmp[1] = 0;
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

int TString::mreplace(const TCHAR chr, const TCHAR *fmt)
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

TString TString::gettok( int N, const TCHAR * sepChars ) const {

	if (sepChars == NULL || this->m_pString == NULL)
		return *this;

	const int nToks = this->numtok(sepChars);

	TString token;

	if (N > nToks)
		return token;

	if (N < 0) {

		N += (nToks + 1);
		if (N < 1)
			return token;
	}

	TCHAR * p_cStart = this->m_pString, *p_cEnd = this->m_pString;
	long iCount = 0;
	const size_t sepl = ts_strlen(sepChars); // Ook

	while ((p_cEnd = ts_strstr(p_cStart, sepChars)) != NULL) {
		iCount++;

		if (iCount == N) {

			const size_t len = p_cEnd - p_cStart; // gives cch diff
			token.deleteString();
			token.m_pString = token.allocstr_cch(len + 1);
			ts_strcpyn(token.m_pString, p_cStart, len + 1);

			break;
		}
		p_cStart = p_cEnd + sepl; // Ook
	}

	if (iCount == N - 1) {

		p_cEnd = this->m_pString + ts_strlen(this->m_pString);
		const size_t len = p_cEnd - p_cStart;

		token.deleteString();
		token.m_pString = token.allocstr_cch(len + 1);
		ts_strcpyn(token.m_pString, p_cStart, len + 1);
	}

	return token;
}

/*!
 * \brief blah
 *
 * blah
 */

TString TString::gettok( int N, int M, const TCHAR * sepChars ) const {

	if ( sepChars == NULL || this->m_pString == NULL )
		return *this;

	if ( N == M )
		return this->gettok( N, sepChars );

	if ( M <= N && M != -1 )
		return TEXT("");

	const unsigned int nToks = this->numtok( sepChars );

	if ( N > (int)nToks )
		return TEXT("");

	if ( N < 0 ) {

		N += ( nToks + 1 );
		if ( N < 1 )
			return TEXT("");
	}

	if ( M > (int)(nToks - 1) )
		M = -1;

	TCHAR * p_cStart = this->m_pString, * p_cEnd = NULL;
	TCHAR * p_cFirst = NULL, * p_cLast = NULL;
	unsigned long iCount = 0;
	const int sepl = ts_strlen( sepChars ); // Ook

	while ( ( p_cEnd = ts_strstr( p_cStart, sepChars ) ) != NULL ) {
		iCount++;

		if ( (int)iCount == N ) {

			p_cFirst = p_cStart;

			if ( M == -1 )
				break;
		}

		if ( (int)iCount == M ) {

			p_cLast = p_cStart;
			break;
		}

		p_cStart = p_cEnd + sepl;
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


TString TString::getfirsttok( const unsigned int N, const TCHAR * sepChars ) const {

	if ( sepChars == NULL || this->m_pString == NULL )
		return *this;

	this->m_savedtotaltoks = this->numtok( sepChars );
	this->m_savedpos = this->m_pString;
	this->m_savedcurrenttok = N;
	// N == 0 is used top pre load the vars for a loop of next toks where we need to start at 1

	if ( (N > this->m_savedtotaltoks) || (N == 0))
		return TEXT("");

	TString token; // no need to set token to contents of this here?

	TCHAR * p_cStart = this->m_pString, * p_cEnd = this->m_pString, * p_fEnd = (this->m_pString + ts_strlen(this->m_pString));
	unsigned int iCount = 0;
	const int sepl = ts_strlen( sepChars ); // Ook

	while ( ( p_cEnd = ts_strstr( p_cStart, sepChars ) ) != NULL ) {
		iCount++;

		if ( iCount == N ) {

			const int len = (p_cEnd - p_cStart) + 1; // gives cch diff
			delete [] token.m_pString; // change by Ook
			token.m_pString = token.allocstr_cch(len);
			ts_strcpyn(token.m_pString, p_cStart, len);
			this->m_savedpos = p_cEnd + sepl;

			break;
		}
		p_cStart = p_cEnd + sepl; // Ook
		if (p_cStart >= p_fEnd) {
			this->m_savedpos = NULL;
			break;
		}
	}

	if ( iCount == N - 1 ) {

		p_cEnd = p_fEnd;
		const size_t len = (p_cEnd - p_cStart) + 1;

		delete [] token.m_pString; // change by Ook
		token.m_pString = token.allocstr_cch(len);
		ts_strcpyn(token.m_pString, p_cStart, len);

		this->m_savedpos = NULL;
	}

	return token;
}

TString TString::getnexttok( const TCHAR * sepChars ) const {

	if ( sepChars == NULL || this->m_pString == NULL )
		return *this;

	this->m_savedcurrenttok++;
	TString token;
	TCHAR * p_cStart = this->m_savedpos, * p_cEnd = this->m_savedpos;
	const int sepl = ts_strlen( sepChars ); // Ook

	if ( (this->m_savedcurrenttok > this->m_savedtotaltoks ) || (p_cStart == NULL) )
		return token;

	if ( this->m_savedcurrenttok == this->m_savedtotaltoks ) {
		p_cEnd = (this->m_pString + ts_strlen(this->m_pString));
		const int len = (p_cEnd - p_cStart) + 1;

		delete [] token.m_pString; // change by Ook
		token.m_pString = token.allocstr_cch(len);
		ts_strcpyn(token.m_pString, p_cStart, len);

		p_cStart = NULL;
	}
	else if ( ( p_cEnd = ts_strstr( p_cStart, sepChars ) ) != NULL ) {
		const unsigned int len = (p_cEnd - p_cStart) + 1; // gives cch diff
		delete [] token.m_pString; // change by Ook
		token.m_pString = token.allocstr_cch(len);
		ts_strcpyn(token.m_pString, p_cStart, len);

		p_cStart = p_cEnd + sepl; // Ook
	}

	this->m_savedpos = p_cStart;

	return token;
}

/*!
 * \brief blah
 *
 * blah
 */

unsigned int TString::numtok( const TCHAR * sepChars ) const {

	if ( sepChars == NULL || this->m_pString == NULL)
		return 0;

	//if (this->m_savedtotaltoks > 0)
	//	return this->m_savedtotaltoks;

	if (lstrlen(this->m_pString) == 0)
		return 0;

	TCHAR * p_cStart = this->m_pString, * p_cEnd = NULL;
	unsigned int iCount = 0;
	const unsigned int sepl = ts_strlen( sepChars ); // Ook

	while ( ( p_cEnd = ts_strstr( p_cStart, sepChars ) ) != NULL ) {
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

void TString::deltok( const unsigned int N, const TCHAR * sepChars ) {

	if ( sepChars == NULL || this->m_pString == NULL )
		return;

	const unsigned int nToks = this->numtok( sepChars );

	if ( N > nToks || N < 1 )
		return;

	if ( N == 1 && nToks == 1 ) {
		this->deleteString();
		return;
	}

	TCHAR * p_cStart = this->m_pString, * p_cEnd = NULL;
	unsigned long int i = 0;
	const int sepl = ts_strlen( sepChars ); // Ook

	while ( ( p_cEnd = ts_strstr( p_cStart, sepChars ) ) != NULL ) {
		i++;

		if ( i == N )
			break;

		p_cStart = p_cEnd + sepl; // Ook
	}

	// last token
	if ( p_cEnd == NULL ) {

		p_cStart--;
		*p_cStart = 0;

		const int l = lstrlen( this->m_pString ) + 1;
		TCHAR * pNew = allocstr_cch(l);
		ts_strcpyn(pNew, this->m_pString, l);
		this->deleteString(true);
		this->m_pString = pNew;
	}
	// delete the first token
	else if ( p_cStart == this->m_pString ) {

		p_cEnd++;

		const int l = lstrlen( p_cEnd ) + 1;
		TCHAR * pNew = allocstr_cch(l);
		ts_strcpyn(pNew, p_cEnd, l);
		this->deleteString(true);
		this->m_pString = pNew;
	}
	// inbound token
	else {

		*p_cStart = 0;
		p_cEnd++;

		const size_t l = (ts_strlen( this->m_pString ) + ts_strlen( p_cEnd ) + 1);
		TCHAR * pNew = allocstr_cch(l);
		ts_strcpyn(pNew, this->m_pString, l);
		ts_strcat(pNew, p_cEnd);

		this->deleteString(true);
		this->m_pString = pNew;
	}
}
/*!
 * \brief blah
 *
 * blah
 */

void TString::instok( const TCHAR * cToken, const unsigned int N, const TCHAR * sepChars ) {

	if ( cToken == NULL || sepChars == NULL || this->m_pString == NULL )
		return;

	if ( N < 1 )
		return;

	TCHAR * p_cStart = this->m_pString, * p_cEnd = NULL;
	unsigned long int i = 1;
	const int sepl = ts_strlen( sepChars ); // Ook

	while ( ( p_cEnd = ts_strstr( p_cStart, sepChars ) ) != NULL ) {
		if ( i == N )
			break;

		i++;

		p_cStart = p_cEnd + sepl; // Ook
	}

	TCHAR * pNew = allocstr_cch(ts_strlen(cToken) + ts_strlen(this->m_pString) + (ts_strlen(sepChars) *2) + 1);

	// delete the first token
	if ( p_cStart == this->m_pString ) {

		ts_strcpy(pNew, cToken);
		ts_strcat(pNew, sepChars);
		ts_strcat(pNew, this->m_pString);
	}
	else if ( p_cEnd == NULL ) {

		if ( i == N ) {

			p_cStart -= sepl; // Ook
			*p_cStart = 0;
			p_cStart += sepl; // Ook

			ts_strcpy(pNew, this->m_pString);
			ts_strcat(pNew, sepChars);
			ts_strcat(pNew, cToken);
			ts_strcat(pNew, sepChars);
			ts_strcat(pNew, p_cStart);
		}
		else {
			ts_strcpy(pNew, this->m_pString);
			ts_strcat(pNew, sepChars);
			ts_strcat(pNew, cToken);
		}
	}
	else {
		p_cStart -= sepl; // Ook
		*p_cStart = 0;
		p_cStart += sepl; // Ook

		ts_strcpy(pNew, this->m_pString);
		ts_strcat(pNew, sepChars);
		ts_strcat(pNew, cToken);
		ts_strcat(pNew, sepChars);
		ts_strcat(pNew, p_cStart);
	}
	this->deleteString(true);
	this->m_pString = pNew;
}

/*!
 * \brief blah
 * added by Ook
 * blah
 */
void TString::addtok( const TCHAR * cToken, const TCHAR * sepChars ) {

	if (cToken == NULL || sepChars == NULL || this->m_pString == NULL)
		return;

	const size_t mp_len = ts_strlen(this->m_pString);
	const size_t l = (ts_strlen(cToken) + mp_len + ts_strlen(sepChars) + 1);

	if (m_buffersize >= (l *sizeof(TCHAR)))
	{
		if (mp_len) {
			ts_strcat(this->m_pString, sepChars);
			ts_strcat(this->m_pString, cToken);
		}
		else
			ts_strcpyn(this->m_pString, cToken, l);
	}
	else {
		TCHAR *pNew = allocstr_cch(l);

		if (mp_len) {
			ts_strcpy(pNew, this->m_pString);
			ts_strcat(pNew, sepChars);
			ts_strcat(pNew, cToken);
		}
		else
			ts_strcpyn(pNew, cToken, l);

		this->deleteString(true);
		this->m_pString = pNew;
	}
}
//void TString::addtok( const TString &cToken, const TCHAR * sepChars ) {
//	this->addtok(cToken.to_chr(), sepChars);
//}
void TString::addtok( const __int64 nToken, const TCHAR * sepChars ) {

	if ( sepChars == NULL || this->m_pString == NULL )
		return;

	TString tmp;
	tmp.tsprintf(TEXT("%s%s%d"),this->m_pString, sepChars, nToken);
	this->deleteString();
	this->m_pString = tmp.m_pString;
	this->m_buffersize = tmp.m_buffersize;
	tmp.m_pString = NULL;
	this->m_savedtotaltoks = 0;
}

bool TString::istok(const TCHAR * cToken, const TCHAR * sepChars ) const {

	if ( sepChars == NULL || this->m_pString == NULL )
		return false;

	TCHAR * p_cStart = this->m_pString, * p_cEnd = NULL;
	const int sepl = ts_strlen( sepChars );

	while ( ( p_cEnd = ts_strstr( p_cStart, sepChars ) ) != NULL ) {
		const size_t l = (p_cEnd - p_cStart);
		if (l > 0) {
			if (ts_strncmp(cToken,p_cStart,l) == 0) return true;
		}
		p_cStart = p_cEnd + sepl;
	}
	if (ts_strcmp(cToken,p_cStart) == 0) return true;

	return false;
}

/*!
 * \brief blah
 *
 * blah
 */

void TString::puttok( const TCHAR * cToken, int N, const TCHAR * sepChars ) {

	if ( cToken == NULL || sepChars == NULL || this->m_pString == NULL )
		return;

	TCHAR * p_cStart = this->m_pString, * p_cEnd = NULL;
	long int i = 0;
	const int sepl = ts_strlen( sepChars ); // Ook

	while ( ( p_cEnd = ts_strstr( p_cStart, sepChars ) ) != NULL ) {
		i++;

		if ( i == N )
			break;

		p_cStart = p_cEnd + sepl; // Ook
	}

	TCHAR *pNew = NULL;

	// last token
	if ( p_cEnd == NULL ) {

		*p_cStart = 0;

		pNew = allocstr_cch(ts_strlen(cToken) + ts_strlen(this->m_pString) + 1);

		ts_strcpy( pNew, this->m_pString );
		ts_strcat( pNew, cToken );
	}
	// delete the first token
	else if ( p_cStart == this->m_pString ) {

		pNew = allocstr_cch(ts_strlen(cToken) + ts_strlen(p_cEnd) + 1);

		ts_strcpy(pNew, cToken);
		ts_strcat(pNew, p_cEnd);
	}
	// inbound token
	else {

		*p_cStart = 0;

		pNew = allocstr_cch(ts_strlen(this->m_pString) + ts_strlen(cToken) + ts_strlen(p_cEnd) + 1);

		ts_strcpy(pNew, this->m_pString);
		ts_strcat(pNew, cToken);
		ts_strcat(pNew, p_cEnd);
	}
	this->deleteString(true);
	this->m_pString = pNew;
}

void TString::remtok(const TCHAR * cToken, int N, const TCHAR * sepChars) {
	const unsigned int tokennr = findtok(cToken, N, sepChars);
	if (tokennr > 0)
		deltok(tokennr, sepChars);
}
/*!
 * \brief blah
 *
 * blah
 */
TString &TString::trim() {
	TCHAR *start = this->m_pString;
	TCHAR *end = this->m_pString + ts_strlen(this->m_pString);
	const TCHAR *oldEnd = end;

	if ((start == NULL) || (end == NULL))
		return *this;

	// Trim from start
	while (start != end && *start == 32)
		start++;

	// Trim from end
	while (end != start && *(--end) == 32);

	// only alloc new string is string changed.
	if ((start != this->m_pString) || (end != oldEnd))
	{
		const size_t new_len = (end - start) + 2;

		TCHAR *temp = allocstr_cch(new_len);

		ts_strcpyn(temp, start, new_len);

		this->deleteString(true);

		this->m_pString = temp;
	}
	return *this;
}

// pos must be within length of string, n can be < 0
TString TString::mid(const int pos, int n) const
{
	TString tmp;
	const int l = ts_strlen(this->m_pString);
	if ((n == 0) || (l == 0) || (pos > l) || (pos < 0))
		return tmp;
	if (n < 0)
		n = l - pos + n;
	else if ((pos + n) > l)
		n = l - pos;
	if (n < 1)
		return tmp;
	n++;
	tmp.deleteString();
	TCHAR *p = tmp.allocstr_cch(n);
	ts_strcpyn(p, &this->m_pString[pos], n);
	tmp.m_pString = p;
	return tmp;
}
// if n > string length its truncated, n can be < 0
TString TString::left(int n) const
{
	//TString tmp;
	//const size_t l = ts_strlen(this->m_pString);
	//if ((l == 0) || (n == 0))
	//	return tmp;
	//if (n < 0)
	//	n = l + n;
	//if (n > l)
	//	n = l;
	//n++;
	//tmp.deleteString();
	//TCHAR *p = tmp.allocstr_cch(n);
	//ts_strcpyn(p, this->m_pString, n);
	//tmp.m_pString = p;
	//return tmp;
	return this->mid(0, n);
}
// if n > string length its truncated, n can be < 0
TString TString::right(int n) const
{
	TString tmp;
	const int l = ts_strlen(this->m_pString);
	if ((n == 0) || (l == 0))
		return tmp;
	if (n > l)
		n = l;
	int start = l - n, len = n + 1;
	if (n < 0) {
		start = abs(n);
		if (start > l) return tmp;
		len = (l - start) + 1;
	}
	tmp.deleteString();

	TCHAR *p = tmp.allocstr_cch(len);
	ts_strcpyn(p, &this->m_pString[start], len);
	tmp.m_pString = p;
	return tmp;
	//const int l = lstrlen(this->m_pString);
	//return this->mid(l - n,l);
}
#if UNICODE
char *TString::c_str(void) const
{
	if (this->m_pString == NULL)
		return NULL;

	if (this->m_pTempString == NULL)
		this->m_pTempString = TString::WcharTochar(this->m_pString);

	return this->m_pTempString;
}
#endif
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
	return tolower(c);
}
inline int TString::rfc_toupper(const int c)
{
	//TCHAR tmp[2];
	//tmp[0] = (TCHAR)c;
	//tmp[1] = 0;
	//_wcsupr(tmp);
	//return (int)tmp[0];
	return toupper(c);
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
int TString::match (register TCHAR *m, register TCHAR *n, const bool cs /* case sensitive */)
{
	TCHAR *ma = m, *lsm = 0, *lsn = 0, *lpm = 0, *lpn = 0;
	int match = 1, saved = 0;
	register unsigned int sofar = 0;

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
TString TString::wildtok( TCHAR * wildString, int N, const TCHAR * sepChars ) const
{
	if ( sepChars == NULL || this->m_pString == NULL )
		return TEXT("");

	const int nToks = this->numtok( sepChars );

	if ( N > nToks )
		return TEXT("");

	int m = 0;
	for (TString tmp(this->getfirsttok(1,sepChars)); tmp != TEXT(""); tmp = this->getnexttok(sepChars))
	{
		if (match(wildString,tmp.to_chr(),false)) {
			m++;
			if (m == N) return tmp;
		}
	}
	return TEXT("");
}
int TString::nwildtok( TCHAR * wildString, const TCHAR * sepChars ) const
{
	if ( sepChars == NULL || this->m_pString == NULL )
		return 0;

	int m = 0;
	for (TString tmp(this->getfirsttok(1,sepChars)); tmp != TEXT(""); tmp = this->getnexttok(sepChars))
	{
		if (match(wildString,tmp.to_chr(),false))
			m++;
	}
	return m;
}
int TString::tsprintf(const TCHAR *fmt, ...)
{
	va_list args;
	va_start( args, fmt );
	int cnt = tvprintf(fmt, &args);
	va_end( args );
	return cnt;
}

int TString::tvprintf(const TCHAR *fmt, va_list * args)
{
	int cnt = ts_vscprintf(fmt, *args);
	if (cnt > 0) {
		TCHAR *txt = allocstr_cch(cnt + 2);

		// warning C4996: 'vsprintf' was declared deprecated
		// http://forums.microsoft.com/MSDN/ShowPost.aspx?PostID=10254&SiteID=1
		ts_vsprintf(txt, cnt + 1, fmt, *args);

		this->deleteString(true);
		this->m_pString = txt;
	}
	return cnt;
}

/*
 * iswm(*mask*)
 *    returns TRUE if *mask* matches string
 *    else it returns FALSE
 *    case insensitive
 */
bool TString::iswm(TCHAR *a) const
{
	return match(a, this->m_pString,false)?true:false;
}

/*
 * iswmcs(*mask*)
 *    returns TRUE if *mask* matches string
 *    else it returns FALSE
 *    case sensitive
 */
bool TString::iswmcs(TCHAR *a) const
{
	return match(a, this->m_pString,true)?true:false;
}
//void TString::strncpy(const TCHAR *cString, const int n)
//{
//	this->deleteString();
//	this->m_pString = new TCHAR[n+1];
//	lstrcpyn(this->m_pString,cString,n);
//}
//void TString::strncpy(const TString &tString, const int n)
//{
//	this->deleteString();
//	this->m_pString = new TCHAR[n+1];
//	lstrcpyn(this->m_pString,tString.to_chr(),n);
//}
//void TString::strncat(const TCHAR *cString, const int n)
//{
//	TCHAR *tmp = new TCHAR[n + lstrlen(this->m_pString) + 1];
//	lstrcpy(tmp,this->m_pString);
//	::strncat(tmp,cString,n);
//	this->deleteString();
//	this->m_pString = tmp;
//}
//void TString::strncat(const TString &tString, const int n)
//{
//	TCHAR *tmp = new TCHAR[n + lstrlen(this->m_pString) + 1];
//	lstrcpy(tmp,this->m_pString);
//	::strncat(tmp,tString.to_chr(),n);
//	this->deleteString();
//	this->m_pString = tmp;
//}
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

// convert a cstring/utf8 string to a utf16 string
WCHAR *TString::charToWchar(const char *cString, size_t *buffer_size)
{
	// try UTF8 encoded first, but error on invalid chars.
	WCHAR *res = NULL;
	size_t buf_size = 0;
	if ( cString != NULL ) {
		int widelen = MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,cString,-1, NULL, 0);
		if (widelen == 0) {
			// zero result, error maybe?
			if (GetLastError() == ERROR_NO_UNICODE_TRANSLATION) {
				// invalid chars, assume its NOT a utf8 string then, try ascii->utf16
				widelen = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,cString,-1, NULL, 0);
				if (widelen != 0) {
					buf_size = TS_wgetmemsize(widelen + 1);
					res = new WCHAR[buf_size];	// TS_getmemsize() gives a much larger buffer than we really want, but it will do.
					MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,cString,-1, res, widelen);
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
			MultiByteToWideChar(CP_UTF8,0,cString,-1, res, widelen);
		}
	}
	if (buffer_size != NULL)
		*buffer_size = buf_size;

	return res;
}
char *TString::WcharTochar(const WCHAR *wString, size_t *buffer_size)
{
	const int l = WideCharToMultiByte( CP_UTF8, 0, wString, -1, NULL, 0, NULL, NULL );
	char *res = NULL;
	size_t buf_size = 0;

	if (l > 0) {
		buf_size = TS_getmemsize(l + 1);
		res = new char[buf_size];
		WideCharToMultiByte( CP_UTF8, 0, wString, -1, res, l, NULL, NULL );
	}
	if (buffer_size != NULL)
		*buffer_size = buf_size;

	return res;
}
/*!
 * \brief blah
 *
 * blah
 */
TString &TString::strip() {
	TCHAR *start = this->m_pString;
	TCHAR *end = this->m_pString + ts_strlen(this->m_pString);

	if ((start == NULL) || (end == NULL))
		return *this;

	// Trim from start
	while (start != end && *start == TEXT(' '))
		start++;

	// Trim from end
	while (end != start && *(--end) == TEXT(' '));

	const size_t new_len = (end - start) +1;

	TCHAR *temp = allocstr_cch(new_len + 1);
	temp[0] = 0;

	// now strip all ctrl codes.
	TCHAR *wtxt = start, *p = temp;
	UINT pos = 0, tpos = 0;

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
				while (wtxt[pos+1] == 3) pos++; // remove multiple consecutive ctrl-k's
				if (wtxt[pos +1] >= TEXT('0') && wtxt[pos +1] <= TEXT('9')) {
					++pos;

					if (wtxt[pos +1] >= TEXT('0') && wtxt[pos +1] <= TEXT('9'))
						pos++;

					// maybe a background color
					if (wtxt[pos+1] == TEXT(',')) {
						++pos;

						if (wtxt[pos +1] >= TEXT('0') && wtxt[pos +1] <= TEXT('9')) {
							pos++;

							if (wtxt[pos +1] >= TEXT('0') && wtxt[pos +1] <= TEXT('9'))
								++pos;
						}
					}
				}
			}
			break;
		default:
			p[tpos++] = c;
			break;
		}
	}
	p[tpos] = 0;

	this->deleteString(true);
	this->m_pString = temp;

	return *this;
}
