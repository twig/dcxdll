/*! 
 * \file TString.cpp
 * \brief String and Token Management Class Functions
 *
 * This experimental library was built to reproduce easy to use string containers managing effectively 
 * comparisons and token manipulations as done in the mIRC scripting language.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.9
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
 *
 * © ScriptsDB.org - 2005
 */
#include "defines.h"
// Required for VS 2005
#define _CRT_SECURE_NO_DEPRECATE 1
// end VS2005

#include <windows.h>

#include "tstring.h"

const char *TString::m_cSpace = " ";
const char *TString::m_cComma = ",";
const char *TString::m_cTab = "\t";

/*! 
 * \brief Default Constructor
 */
/****************************/

TString::TString( ) {

	this->m_pString = new char[1];
	this->m_pString[0] = 0;
	this->m_pWString = NULL;
}

/****************************/
/*! \fn TString::TString( const char * cString )
    \brief Constructor input from C char string

    \param cString Character string to initialize string to
*/
/****************************/

TString::TString( const char * cString ) {

	this->m_pString = NULL;
	this->m_pWString = NULL;

	if ( cString != NULL ) {
		this->m_pString = new char [ lstrlen( cString ) + 1 ];
		lstrcpy( this->m_pString, cString );
	}
	else {
		this->m_pString = new char[1];
		this->m_pString[0] = 0;
	}
}

TString::TString( const WCHAR * cString ) {

	this->m_pString = NULL;
	this->m_pWString = NULL;

	if ( cString != NULL ) {
		this->m_pWString = new WCHAR [ lstrlenW( cString ) + 1 ];
		lstrcpyW( this->m_pWString, cString );
	}
}

/****************************/
/*! \fn TString::TString( const char chr )
    \brief Constructor input from C char

    \param chr Character to initialize string to
*/
/****************************/

TString::TString( const char chr ) {

	this->m_pString = new char [2];
	this->m_pWString = NULL;
	this->m_pString[0] = chr;
	this->m_pString[1] = 0;
}

TString::TString( const WCHAR chr ) {

	this->m_pWString = new WCHAR [2];
	this->m_pString = NULL;
	this->m_pWString[0] = chr;
	this->m_pWString[1] = 0;
}

/****************************/
/*! \fn TString::TString( const TString & tString )
    \brief Copy Constructor

    \param tString TString object to copy
*/
/****************************/

TString::TString( const TString & tString ) {

	this->m_pWString = NULL;
	if ( tString.m_pString != NULL ) {
		this->m_pString = new char [lstrlen( tString.m_pString ) +1];
		lstrcpy( this->m_pString, tString.m_pString );
	}
	else {
		this->m_pString = new char[1];
		this->m_pString[0] = 0;
	}
}

TString::TString( const char *pStart, const char *pEnd )
{
	this->m_pWString = NULL;
	if ((pStart != NULL) && (pEnd != NULL) && (pEnd > pStart)) {
		size_t size = (pEnd - pStart);
		this->m_pString = new char[size+1];
		CopyMemory(this->m_pString, pStart, size);
		this->m_pString[size] = 0;
	}
	else {
		this->m_pString = new char[1];
		this->m_pString[0] = 0;
	}
}

TString::TString( const unsigned int tsSize )
{
	this->m_pWString = NULL;
	this->m_pString = new char[tsSize+1];
	ZeroMemory(this->m_pString,tsSize+1);
}

TString::~TString( ) {

	this->deleteString( );
}

/*!
 * \brief Deletes the allocated char buffer
 */
void TString::deleteString( ) {

	if ( this->m_pString != NULL ) {
		delete [] this->m_pString; 
		this->m_pString = NULL;
	}
	this->deleteWString();
}
void TString::deleteWString( ) {

	if ( this->m_pWString != NULL ) {
		delete [] this->m_pWString; 
		this->m_pWString = NULL;
	}
}

/****************************/
TString& TString::operator =( const TString & tString ) {

	this->deleteString( );

	if ( tString.m_pString != NULL ) {
		this->m_pString = new char [ lstrlen( tString.m_pString ) + 1 ];
		lstrcpy( this->m_pString, tString.m_pString );
	}
	return *this;
}

/****************************/
/*! \fn void TString::operator=( const char * string )
    \brief Symbol Overload - TString assignement from C char string

    \param cString Character string to assign string to
*/
/****************************/
TString& TString::operator =( const char * cString ) {

	this->deleteString( );

	if ( cString != NULL ) {
		this->m_pString = new char [ lstrlen( cString ) + 1 ];
		lstrcpy( this->m_pString, cString );
	}
	return *this;
}

/****************************/
/*! \fn void TString::operator=( const char chr )
    \brief Symbol Overload - TString assignement from C char

    \param chr Character to assign string to
*/
/****************************/
TString& TString::operator =( const char chr ) {

	this->deleteString( );
	this->m_pString = new char[2];
	this->m_pString[0] = chr;
	this->m_pString[1] = 0;
	return *this;
}

TString& TString::operator =( const WCHAR &chr ) {

	this->deleteString( );
	this->m_pWString = new WCHAR[2];
	this->m_pWString[0] = chr;
	this->m_pWString[1] = 0;
	return *this;
}
TString& TString::operator =( const WCHAR * cString ) {

	this->deleteString( );

	if ( cString != NULL ) {
		size_t size = lstrlenW( cString );
		this->m_pWString = new WCHAR [ size + 1 ];
		lstrcpyW( this->m_pWString, cString );
	}
	return *this;
}

/****************************/
/*! \fn TString TString::operator +( const char * string )
    \brief Symbol Overload - TString and char string concatenation

    \param cString Character string to concatenate

    \return Returns the new TString object
*/
/****************************/

TString TString::operator +( const char * cString ) {

	if ( cString != NULL ) {
		TString newTString((UINT)(lstrlen( this->m_pString )+lstrlen( cString )));

		lstrcpy( newTString.m_pString, this->m_pString );
		lstrcat( newTString.m_pString, cString );
		return newTString;
	}
	return *this; // return an object not a ref or pointer to object, so object is copied on return.
}

/****************************/
/*! \fn TString TString::operator +( const char chr )
    \brief Symbol Overload - TString and char concatenation

    \param chr Character to concatenate

    \return Returns the new TString object
*/
/****************************/

TString TString::operator +( const char chr ) {

	size_t size = lstrlen( this->m_pString );
	TString newTString((UINT)(size+2));

	lstrcpy( newTString.m_pString, this->m_pString );
	newTString.m_pString[size] = chr;
	newTString.m_pString[size+1] = 0;

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

	if ( tString.m_pString ) {
		TString newTString((UINT)(lstrlen( this->m_pString )+lstrlen( tString.m_pString )));

		lstrcpy( newTString.m_pString, this->m_pString );
		lstrcat( newTString.m_pString, tString.m_pString );
		return newTString;
	}
	return *this;
}

/****************************/
/*! \fn TString TString::operator -( const char * string )
    \brief Symbol Overload - remove char string from TString

    \param cString Character string to remove

    \return Returns the new TString object
*/
/****************************/

TString TString::operator -( const char * cString ) {

	if ( cString != NULL ) {
		TString newTString(*this);
		newTString.i_remove(cString);
		return newTString;
	}
	return *this;
}

TString TString::operator -( const char chr) {

	if ( chr == 0 ) return *this;

	TString newTString(*this);
	char str[2];
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
/*! \fn void TString::operator +=( const char * string )
    \brief Symbol Overload - TString and char string concatenation

    \param cString Character string to concatenate
*/
/****************************/

TString & TString::operator +=( const char * cString ) {

	if ( cString != NULL ) {
		char * temp = new char [ lstrlen( this->m_pString ) + lstrlen( cString ) + 1 ];
		lstrcpy( temp, this->m_pString );
		lstrcat( temp, cString );
		this->deleteString();
		this->m_pString = temp;
	}
	return *this;
}

/****************************/
/*! \fn void TString::operator +=( const char chr )
    \brief Symbol Overload - TString and char concatenation

    \param chr Character to concatenate
*/
/****************************/

TString & TString::operator +=( const char chr ) {

	if ( this->m_pString != NULL ) {
		int len = lstrlen( this->m_pString );
		char * temp = new char [ len + 2 ];
		lstrcpy( temp, this->m_pString );
		temp[len] = chr;
		temp[len+1] = 0;
		this->deleteString();
		this->m_pString = temp;
	}
	else {
		this->m_pString = new char[2];
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

	if ( tString.m_pString ) {
		char * temp = new char [ lstrlen( this->m_pString ) + lstrlen( tString.m_pString ) + 1 ];
		lstrcpy( temp, this->m_pString );
		lstrcat( temp, tString.m_pString );
		this->deleteString();
		this->m_pString = temp;
	}
	return *this;
}

TString & TString::operator +=( const WCHAR &chr ) {

	if ( this->m_pWString != NULL ) {
		int len = lstrlenW( this->m_pWString );
		WCHAR * temp = new WCHAR [ len + 2 ];
		lstrcpyW( temp, this->m_pWString );
		temp[len] = chr;
		temp[len+1] = 0;
		this->deleteString();
		this->m_pWString = temp;
	}
	else {
		this->m_pWString = new WCHAR[2];
		this->m_pWString[0] = chr;
		this->m_pWString[1] = 0;
	}
	return *this;
}

/****************************/
/*! \fn void TString::operator -=( const char * string )
\brief Symbol Overload - remove char string from TString

\param cString Character string to remove
*/
/****************************/

TString & TString::operator -=( const char * cString ) {

	if ( cString != NULL )
		this->i_remove(cString);

	return *this;
}

TString & TString::operator -=( const char chr ) {

	if ( chr != 0 ) {
		char str[2];
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
/*! \fn void TString::operator ==( const char * string )
    \brief Symbol Overload - TString and char string comparison

    \param cString Character string to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator ==( const char * cString ) const {

	if ( cString && this->m_pString ) {
		if ( lstrcmp( this->m_pString, cString ) == 0)
			return true;
	}
	return false;
}

/****************************/
/*! \fn void TString::operator ==( const char chr )
    \brief Symbol Overload - TString and char comparison

    \param chr Character to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator ==( const char chr ) const {

	if ( this->m_pString ) {

		char temp[2];
		temp[0] = chr;
		temp[1] = 0;

		if ( lstrcmp( this->m_pString, temp ) == 0)
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
		if ( lstrcmp( this->m_pString, tString.m_pString ) == 0)
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
/*! \fn void TString::operator !=( const char * cString )
    \brief Symbol Overload - TString and char string comparison

    \param cString Character string to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator !=( const char * cString ) const {

	if ( cString && this->m_pString ) {

		if ( lstrcmp( this->m_pString, cString ) != 0 )
			return true;
	}
	return false;
}

/****************************/
/*! \fn void TString::operator !=( const char chr )
    \brief Symbol Overload - TString and char comparison

    \param chr Character to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator !=( const char chr ) const {

	if ( this->m_pString != NULL ) {

		char temp[2];
		temp[0] = chr;
		temp[1] = 0;

		if ( lstrcmp( this->m_pString, temp ) != 0 )
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

		if ( lstrcmp( this->m_pString, tString.m_pString ) != 0 )
			return true;
	}
	return false;
}

/****************************/
/*! \fn void TString::operator >( const char * cString )
    \brief Symbol Overload - TString and char string comparison

    \param cString Character string to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator >( const char * cString ) const {

	if ( cString != NULL && this->m_pString != NULL ) {
		if ( lstrcmp( this->m_pString, cString ) > 0 )
			return true;
	}
	return false;
}

/****************************/
/*! \fn void TString::operator >( const char chr )
    \brief Symbol Overload - TString and char comparison

    \param chr Character to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator >( const char chr ) const {

	if ( this->m_pString != NULL ) {

		char temp[2];
		temp[0] = chr;
		temp[1] = 0;

		if ( lstrcmp( this->m_pString, temp ) > 0 )
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

		if ( lstrcmp( this->m_pString, tString.m_pString ) > 0 )
			return true;
	}
	return false;
}

/****************************/
/*! \fn void TString::operator >=( const char * cString )
    \brief Symbol Overload - TString and char string comparison

    \param cString Character string to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator >=( const char * cString ) const {

	if ( cString != NULL && this->m_pString != NULL ) {

		if ( lstrcmp( this->m_pString, cString ) >= 0 )
			return true;
	}
	return false;
}

/****************************/
/*! \fn void TString::operator >=( const char chr )
    \brief Symbol Overload - TString and char comparison

    \param chr Character to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator >=( const char chr ) const {

	if ( this->m_pString != NULL ) {

		char temp[2];
		temp[0] = chr;
		temp[1] = 0;

		if ( lstrcmp( this->m_pString, temp ) >= 0 )
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

		if ( lstrcmp( this->m_pString, tString.m_pString ) >= 0 )
			return true;
	}
	return false;
}

/****************************/
/*! \fn void TString::operator <( const char * cString )
    \brief Symbol Overload - TString and char string comparison

    \param cString Character string to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator <( const char * cString ) const {

	if ( cString != NULL && this->m_pString != NULL ) {

		if ( lstrcmp( this->m_pString, cString ) < 0 )
			return true;
	}
	return false;
}

/****************************/
/*! \fn void TString::operator <( const char chr )
    \brief Symbol Overload - TString and char comparison

    \param chr Character to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator <( const char chr ) const {

	if ( this->m_pString != NULL ) {

		char temp[2];
		temp[0] = chr;
		temp[1] = 0;

		if ( lstrcmp( this->m_pString, temp ) < 0 )
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

		if ( lstrcmp( this->m_pString, tString.m_pString ) < 0 )
			return true;
	}
	return false;
}

/****************************/
/*! \fn void TString::operator <=( const char * cString )
    \brief Symbol Overload - TString and char string comparison

    \param cString Character string to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator <=( const char * cString ) const {

	if ( cString != NULL && this->m_pString != NULL ) {

		if ( lstrcmp( this->m_pString, cString ) <= 0 )
			return true;
	}
	return false;
}

/****************************/
/*! \fn void TString::operator <=( const char chr )
    \brief Symbol Overload - TString and char comparison

    \param chr Character to compare

    \return Returns \b true or \b false based on the comparison

    \note Comparison si based on a regular string comparison
    \note If the string in the TString object is empty, will return \b false
*/
/****************************/

bool TString::operator <=( const char chr ) const {

	if ( this->m_pString != NULL ) {

		char temp[2];
		temp[0] = chr;
		temp[1] = 0;

		if ( lstrcmp( this->m_pString, temp ) <= 0 )
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

		if ( lstrcmp( this->m_pString, tString.m_pString ) <= 0 )
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

	if ( N < 0 || N == 1 )
		return *this;

	if ( N == 0 )
		return "";

	TString tmp((UINT)(lstrlen( this->m_pString ) *N));

	for (int i = 0; i < N; i++ )
		lstrcat( tmp.m_pString, this->m_pString );

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

	if ( N == 0 ) {
		this->deleteString( );
		return *this;
	}

	// if string is null just return.
	if (( N == 1 ) || (this->m_pString == NULL))
		return *this;

	char *temp = this->m_pString;
	this->m_pString = new char [(lstrlen( temp )*N)+1];
	this->m_pString[0] = 0;

	for (int i = 0; i < N; i++)
		lstrcat( this->m_pString, temp );

	this->deleteWString();
	delete [] temp;
	return *this;
}

/****************************/
/*! \fn char & TString::operator []( long int N )
    \brief Symbol Overload - Array Subscript

    \param N Character index

    \return Returns the Nth char of string

    \note > Index starts at \b 0 \n
          > Index can be negative, \b -1 being last character
*/
/****************************/

char & TString::operator []( long int N ) const {

	static char chr = '\0';

	if ( this->m_pString ) {
		long int size = (long int) lstrlen( this->m_pString );
		if (N < 0) N += size;

		if ( N > size - 1 || N < 0 ) 
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
		return lstrlen(this->m_pString);

	return 0;
}

size_t TString::wlen( ) const {

	if ( this->m_pWString == NULL ) // force conversion.
		const_cast<TString *>(this)->to_wchr();

	if ( this->m_pWString )
		return lstrlenW( this->m_pWString );

	return 0;
}

/****************************/
/*! \fn int TString::find( const char * substring, int N )
    \brief Function to find position or number of occurences of a substring in the string

    \param substring Substring to search
    \param N Nth substring to search (N = \b 0 > Total number of matches)

    \return > Number of occurrences (N = 0)\n
            > Starting position of \b substring \n
            > \b -1 if function fails or no substring was found
*/
/****************************/

int TString::find( const char * substring, const int N ) const {

	if ( substring && this->m_pString ) {

		char * temp, * temp2 = this->m_pString;

		int i = 0;
		int subl = lstrlen( substring ); // Ook
		while ( ( temp = strstr( temp2, substring ) ) != NULL ) {
			i++;
			//if ( N != 0 && i == N )
			if ( i == N ) // i is never zero
				return (int) (temp - this->m_pString);
			temp2 = (temp + subl); // Ook
		}
		if (N == 0)
			return i;
	}
	return -1;
}

/****************************/
/*! \fn int TString::find( const char chr, int N )
    \brief Function to find position or number of occurrences of a char in the string
*/
/****************************/
int TString::find(const char chr, const int N) const {
	int c = 0;
	size_t len = this->len();

	for (size_t i = 0; i < len; i++) {
		// found a match, increase counter
		if (this->m_pString[i] == chr)
			c++;

		// if we've reached the Nth match we want, return the position
		if ((N > 0) && (c == N))
			return i;
	}

	// return number of results
	if (N == 0)
		return c;

	// -1 if no matches
	return -1;
}


/****************************/
/*! \fn TString TString::findtok(char * cToken, int N, char * sepChars)
    \brief Returns the index of a specific token

	 \param cToken the token to find
    \param N the Nth match
    \param sepChars separating character

    \return index of found token

    \note > Index starts at \b 1 \n
*/
/****************************/
unsigned int TString::findtok(const char * cToken, const int N, const char * sepChars) const {
	unsigned int count = 0;

	for (unsigned int i = 1; i <= this->numtok(sepChars); i++) {
		if (this->gettok(i, sepChars) == cToken) {
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

	if ( this->m_pString ) {

		int size = (int) lstrlen( this->m_pString );

		if ( N < 0 ) N += size;

		if ( N > size - 1 || N < 0 )
			return newTString;

		if ( N + M > size )
			M = size - N;

		delete [] newTString.m_pString; // change by Ook
		newTString.m_pString = new char[M+1];
		newTString.m_pString[0] = 0;

		char * temp = this->m_pString;
		temp += N;

		lstrcpyn( newTString.m_pString, temp, M + 1 );
		newTString.m_pString[M] = 0;
	}
	return newTString;
}

TString TString::wsub( int N, int M ) const {

	TString newTString;

	if ( this->m_pWString ) {

		int size = (int) lstrlenW( this->m_pWString );

		if ( N < 0 ) N += size;

		if ( N > size - 1 || N < 0 )
			return newTString;

		if ( N + M > size )
			M = size - N;

		delete [] newTString.m_pWString; // change by Ook
		newTString.m_pWString = new WCHAR[M+1];
		newTString.m_pWString[0] = 0;

		WCHAR * temp = this->m_pWString;
		temp += N;

		lstrcpynW( newTString.m_pWString, temp, M + 1 );
		newTString.m_pWString[M] = 0;
	}
	return newTString;
}

/*
internal remove function, called by the other remove()/-/-= functions.
Ook
*/

int TString::i_remove(const char *subString)
{
	// no str to remove or no str to remove from
	if ((subString == NULL) || (this->m_pString == NULL)) return 0;
	char *sub, *p = this->m_pString;
	int cnt = 0, subl = lstrlen(subString), ol = lstrlen(p);
	// if length of sub string or old string is 0 return 0
	// may change this.
	if ((subl == 0) || (ol == 0)) return 0;
	// see if we have any matches & how many.
	while ((sub = strstr(p,subString)) != NULL) {
		cnt++;
		p = sub + subl;
	}
	// make final string if we have any matches.
	if (cnt > 0) {
		p = this->m_pString;
		char *out = new char [ (ol - (cnt * subl)) +1 ]; // allocate new string.
		out[0] = 0;
		while ((sub = strstr(p,subString)) != NULL) {
			::strncat(out,p,(sub - p)); // copy bit before substring. if any.
			p = sub + subl; // update pointer to skip substring.
		}
		lstrcat(out,p); // append the end text, if any.
		this->deleteString(); // delete old string
		this->m_pString = out; // save new one.
	}
	return cnt;
}

/*
	internal replace function, called by the other replace() functions.
	Ook
*/
int TString::i_replace(const char *subString, const char *rString)
{
	if ((subString == NULL) || (rString == NULL) || (this->m_pString == NULL)) return 0;
	char *sub, *p = this->m_pString;
	int cnt = 0, subl = lstrlen(subString), repl = lstrlen(rString), ol = lstrlen(p);
	// if length of sub string or old string is 0 return 0
	// may change this.
	if ((subl == 0) || (ol == 0)) return 0;
	// see if we have any matches & how many.
	while ((sub = strstr(p,subString)) != NULL) {
		cnt++;
		p = sub + subl;
	}
	// make final string if we have any matches.
	if (cnt > 0) {
		p = this->m_pString;
		char *out = new char [ (cnt * repl) + (ol - (cnt * subl)) +1 ]; // allocate new string.
		out[0] = 0;
		while ((sub = strstr(p,subString)) != NULL) {
			::strncat(out,p,(sub - p)); // copy bit before substring. if any.
			lstrcat(out,rString); // append new replacement string.
			p = sub + subl; // update pointer to skip substring.
		}
		lstrcat(out,p); // append the end text, if any.
		this->deleteString(); // delete old string
		this->m_pString = out; // save new one.
	}
	return cnt;
}

/****************************/
/*! \fn int TString::replace( const char * substring, const char * replace )
    \brief blah!
*/
/****************************/

int TString::replace( const char * subString, const char * rString ) {
	return this->i_replace(subString,rString);
}

/****************************/
/*! \fn int TString::replace( const char * substring, const char rchr )
    \brief blah!
*/
/****************************/

int TString::replace( const char * subString, const char rchr ) {

	char tmp[2];
	tmp[0] = rchr;
	tmp[1] = 0;
	return this->i_replace(subString,tmp);
}

/****************************/
/*! \fn int TString::replace( const char chr, const char * replace )
    \brief blah!
*/
/****************************/

int TString::replace( const char chr, const char * rString ) {

	char tmp[2];
	tmp[0] = chr;
	tmp[1] = 0;
	return this->i_replace(tmp,rString);
}

/****************************/
/*! \fn int TString::replace( const char chr, const char rchr )
    \brief blah!
*/
/****************************/

int TString::replace( const char chr, const char rchr ) {

	int cnt = 0;
	char *p = this->m_pString;
	if (p == NULL) return 0;
	while (*p) {
		if (*p == chr) {
			*p = rchr;
			cnt++;
		}
		p++;
	}
	return cnt;
}

/*!
 * \brief blah
 *
 * blah
 */

TString TString::gettok( int N, const char * sepChars ) const {

	if ( sepChars == NULL || this->m_pString == NULL )
		return *this;

	int nToks = this->numtok( sepChars );

	if ( N > nToks )
		return "";

	if ( N < 0 ) {

		N += ( nToks + 1 );
		if ( N < 1 )
			return "";
	}

	TString token(*this);

	char * p_cStart = this->m_pString, * p_cEnd = this->m_pString;
	long iCount = 0;
	int sepl = lstrlen( sepChars ); // Ook

	while ( ( p_cEnd = strstr( p_cStart, sepChars ) ) != NULL ) {

		iCount++;

		if ( iCount == N ) {

			int len = p_cEnd - p_cStart;
			delete [] token.m_pString; // change by Ook
			token.m_pString = new char [len + 1];
			token.m_pString[len] = 0;
			lstrcpyn( token.m_pString, p_cStart, len + 1 );

			break;
		}
		p_cStart = p_cEnd + sepl; // Ook
	}

	if ( iCount == N - 1 ) {

		p_cEnd = this->m_pString + lstrlen( this->m_pString );
		int len = p_cEnd - p_cStart;

		delete [] token.m_pString; // change by Ook
		token.m_pString = new char [len + 1];
		token.m_pString[len] = 0;
		lstrcpyn( token.m_pString, p_cStart, len + 1 );    
	}
	else if ( p_cEnd == NULL )
		return "";

	return token;
}

/*!
 * \brief blah
 *
 * blah
 */

TString TString::gettok( int N, int M, const char * sepChars ) const {

	if ( sepChars == NULL || this->m_pString == NULL )
		return *this;

	int nToks = this->numtok( sepChars );

	if ( N == M )
		return this->gettok( N, sepChars );

	if ( M <= N && M != -1 )
		return "";

	if ( M > nToks - 1 )
		M = -1;

	if ( N > nToks )
		return "";

	if ( N < 0 ) {

		N += ( nToks + 1 );
		if ( N < 1 )
			return "";
	}

	char * p_cStart = this->m_pString, * p_cEnd;
	char * p_cFirst = NULL, * p_cLast = NULL;
	long iCount = 0;
	int sepl = lstrlen( sepChars ); // Ook

	while ( ( p_cEnd = strstr( p_cStart, sepChars ) ) != NULL ) {

		iCount++;

		if ( iCount == N ) {

			p_cFirst = p_cStart;

			if ( M == -1 )
				break;
		}

		if ( iCount == M ) {

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


/*!
 * \brief blah
 *
 * blah
 */

unsigned int TString::numtok( const char * sepChars ) const {

	if ( sepChars == NULL || this->m_pString == NULL)
		return 0;

	if (lstrlen(this->m_pString) == 0)
		return 0;

	char * p_cStart = this->m_pString, * p_cEnd;
	unsigned int iCount = 0;
	const unsigned int sepl = lstrlen( sepChars ); // Ook

	while ( ( p_cEnd = strstr( p_cStart, sepChars ) ) != NULL ) {

		iCount++;
		p_cStart = p_cEnd + sepl; // Ook
	}
	return iCount + 1;
}

/*!
 * \brief blah
 *
 * blah
 */

void TString::deltok( const int N, const char * sepChars ) {

	if ( sepChars == NULL || this->m_pString == NULL )
		return;

	if ( N > this->numtok( sepChars ) || N < 1 )
		return;

	if ( N == 1 && this->numtok( sepChars ) == 1 ) {

		this->deleteString();
		return;
	}

	char * p_cStart = this->m_pString, * p_cEnd;
	long int i = 0;
	int sepl = lstrlen( sepChars ); // Ook

	while ( ( p_cEnd = strstr( p_cStart, sepChars ) ) != NULL ) {

		i++;

		if ( i == N )
			break;

		p_cStart = p_cEnd + sepl; // Ook
	}

	// delete the first token
	if ( p_cStart == this->m_pString ) {

		p_cEnd++;

		char * pNew = new char[ lstrlen( p_cEnd ) + 1 ];
		lstrcpy( pNew, p_cEnd );
		this->deleteString();
		this->m_pString = pNew;
	}
	// last token
	else if ( p_cEnd == NULL ) {

		p_cStart--;
		*p_cStart = 0;

		char * pNew = new char[ lstrlen( this->m_pString ) + 1 ];
		lstrcpy( pNew, this->m_pString );
		this->deleteString();
		this->m_pString = pNew;
	}
	// inbound token
	else {

		*p_cStart = 0;
		p_cEnd++;

		char * pNew = new char[ lstrlen( this->m_pString ) + lstrlen( p_cEnd ) + 1 ];
		lstrcpy( pNew, this->m_pString );
		lstrcat( pNew, p_cEnd );

		this->deleteString();
		this->m_pString = pNew;
	}
}
/*!
 * \brief blah
 *
 * blah
 */

void TString::instok( const char * cToken, const int N, const char * sepChars ) {

	if ( cToken == NULL || sepChars == NULL || this->m_pString == NULL )
		return;

	if ( N < 1 )
		return;

	char * p_cStart = this->m_pString, * p_cEnd;
	long int i = 1;
	int sepl = lstrlen( sepChars ); // Ook

	while ( ( p_cEnd = strstr( p_cStart, sepChars ) ) != NULL ) {

		if ( i == N )
			break;

		i++;

		p_cStart = p_cEnd + sepl; // Ook
	}

	char * pNew = new char[ lstrlen( cToken ) + lstrlen( this->m_pString ) + lstrlen( sepChars ) + 1 ];

	// delete the first token
	if ( p_cStart == this->m_pString ) {

		lstrcpy( pNew, cToken );
		lstrcat( pNew, sepChars );
		lstrcat( pNew, this->m_pString );
		this->deleteString();
		this->m_pString = pNew;
	}
	else if ( p_cEnd == NULL ) {

		if ( i == N ) {

			p_cStart -= sepl; // Ook
			*p_cStart = 0;
			p_cStart += sepl; // Ook

			lstrcpy( pNew, this->m_pString );
			lstrcat( pNew, sepChars );
			lstrcat( pNew, cToken );
			lstrcat( pNew, sepChars );
			lstrcat( pNew, p_cStart );
			this->deleteString();
			this->m_pString = pNew;

		}
		else {

			lstrcpy( pNew, this->m_pString );
			lstrcat( pNew, sepChars );
			lstrcat( pNew, cToken );
			this->deleteString();
			this->m_pString = pNew;
		}
	}
	else {
		p_cStart -= sepl; // Ook
		*p_cStart = 0;
		p_cStart += sepl; // Ook

		lstrcpy( pNew, this->m_pString );
		lstrcat( pNew, sepChars );
		lstrcat( pNew, cToken );
		lstrcat( pNew, sepChars );
		lstrcat( pNew, p_cStart );
		this->deleteString();
		this->m_pString = pNew;
	}
}

/*!
 * \brief blah
 * added by Ook
 * blah
 */
void TString::addtok( const char * cToken, const char * sepChars ) {

	if ( cToken == NULL || sepChars == NULL || this->m_pString == NULL )
		return;

	char * pNew;
	int mp_len = lstrlen(this->m_pString);
	if (mp_len) {
		pNew = new char[ lstrlen( cToken ) + mp_len + lstrlen( sepChars ) + 1 ];
		lstrcpy( pNew, this->m_pString );
		lstrcat( pNew, sepChars );
		lstrcat( pNew, cToken );
	}
	else {
		pNew = new char[ lstrlen( cToken ) + 1 ];
		lstrcpy( pNew, cToken );
	}
	this->deleteString();
	this->m_pString = pNew;
}
//void TString::addtok( const TString &cToken, const char * sepChars ) {
//	this->addtok(cToken.to_chr(), sepChars);
//}

bool TString::istok(const char * cToken, const char * sepChars ) const {

	if ( sepChars == NULL || this->m_pString == NULL )
		return false;

	char * p_cStart = this->m_pString, * p_cEnd;
	int sepl = lstrlen( sepChars );

	while ( ( p_cEnd = strstr( p_cStart, sepChars ) ) != NULL ) {
		int l = (int)(p_cEnd - p_cStart);
		if (l > 0) {
			if (strncmp(cToken,p_cStart,l) == 0) return true;
		}
		p_cStart = p_cEnd + sepl;
	}
	if (lstrcmp(cToken,p_cStart) == 0) return true;

	return false;
}

/*!
 * \brief blah
 *
 * blah
 */

void TString::puttok( const char * cToken, int N, const char * sepChars ) {

	if ( cToken == NULL || sepChars == NULL || this->m_pString == NULL )
		return;

	char * p_cStart = this->m_pString, * p_cEnd;
	long int i = 0;
	int sepl = lstrlen( sepChars ); // Ook

	while ( ( p_cEnd = strstr( p_cStart, sepChars ) ) != NULL ) {

		i++;

		if ( i == N )
			break;

		p_cStart = p_cEnd + sepl; // Ook
	}

	// delete the first token
	if ( p_cStart == this->m_pString ) {

		char * pNew = new char[ lstrlen( cToken ) + lstrlen( p_cEnd ) + 1 ];

		lstrcpy( pNew, cToken );
		lstrcat( pNew, p_cEnd );
		this->deleteString();
		this->m_pString = pNew;
	}
	// last token
	else if ( p_cEnd == NULL ) {

		*p_cStart = 0;

		char * pNew = new char[ lstrlen( cToken ) + lstrlen( this->m_pString ) + 1 ];

		lstrcpy( pNew, this->m_pString );
		lstrcat( pNew, cToken );
		this->deleteString();
		this->m_pString = pNew;
	}
	// inbound token
	else {

		*p_cStart = 0;

		char * pNew = new char[ lstrlen( this->m_pString ) + lstrlen( cToken ) + lstrlen( p_cEnd ) + 1 ];

		lstrcpy( pNew, this->m_pString );
		lstrcat( pNew, cToken );
		lstrcat( pNew, p_cEnd );
		this->deleteString();
		this->m_pString = pNew;    
	}
}

void TString::remtok(const char * cToken, int N, const char * sepChars) {
	int tokennr = findtok(cToken, N, sepChars);
	if (tokennr > 0) deltok(tokennr);
}
/*!
 * \brief blah
 *
 * blah
 */
TString &TString::trim() {
	char *start = this->m_pString;
	char *end = this->m_pString + lstrlen(this->m_pString);

	if ((start == NULL) || (end == NULL))
		return *this;

	// Trim from start
	while (start != end && *start == 32)
		start++;

	// Trim from end
	while (end != start && *(--end) == 32);

	size_t new_len = end - start +1;

	char *temp = new char[new_len +1];
	temp[new_len] = 0;

	lstrcpyn(temp, start, new_len +1);
	
	this->deleteString();
	this->m_pString = temp;

	return *this;
}

// pos must be within length of string, n can be < 0
TString TString::mid(const int pos, int n) const
{
	TString tmp;
	int l = (int)lstrlen(this->m_pString);
	if ((n == 0) || (l == 0) || (pos > l) || (pos < 0)) return tmp;
	if (n < 0) n = l - pos + n;
	else if ((pos + n) > l) n = l - pos;
	if (n < 1) return tmp;
	n++;
	char *p = new char[n];
	lstrcpyn(p,&this->m_pString[pos],n);
	tmp.deleteString();
	tmp.m_pString = p;
	return tmp;
}
// if n > string length its truncated, n can be < 0
TString TString::left(int n) const
{
	TString tmp;
	int l = (int)lstrlen(this->m_pString);
	if ((l == 0) || (n == 0)) return tmp;
	if (n < 0) n = l + n;
	if (n > l) n = l;
	n++;
	char *p = new char[n];
	lstrcpyn(p,this->m_pString,n);
	tmp.deleteString();
	tmp.m_pString = p;
	return tmp;
}
// if n > string length its truncated, n can be < 0
TString TString::right(int n) const
{
	TString tmp;
	int l = (int)lstrlen(this->m_pString);
	if ((n == 0) || (l == 0)) return tmp;
	if (n > l) n = l;
	int start = l-n, len = n+1;
	if (n < 0) {
		start = abs(n);
		if (start > l) return tmp;
		len = (l - start) + 1;
	}
	char *p = new char[len];
	lstrcpy(p,&this->m_pString[start]);
	tmp.deleteString();
	tmp.m_pString = p;
	return tmp;
}
// Ook - match() function taken from aircdll.dll by Tabo source.
/* taken from the hybrid-5.3p7 source */
unsigned char TString::tolowertab[] = { 0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, ' ', '!', '"', '#', '$', '%', '&', 0x27, '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|',  '}', '~', '_',  '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|',  '}', '~', 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
unsigned char TString::touppertab[] = { 0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, ' ', '!', '"', '#', '$', '%', '&', 0x27, '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', 0x5f, '`', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
inline int TString::rfc_tolower(const int c)
{
  return tolowertab[(unsigned char)(c)];
}
inline int TString::rfc_toupper(const int c)
{
  return touppertab[(unsigned char)(c)];
}
/*
 * The next function is based on eggdrop's wild_match (by robey pointer)
 * I added an option to perform a case sensitive match
 */

#define QUOTE '\\'
#define WILDS '*'
#define WILDP '%'
#define WILDQ '?'
/* #define WILDT '~' */

#define MATCH (match+saved+sofar)
#define NOMATCH 0

/*
 * EGGDROP:   wild_match_per(char *m, char *n)
 * IrcII:     wild_match(char *m, char *n)
 *
 * Features:  Forward, case-insensitive, ?, *, %, ~(optional)
 * Best use:  Generic string matching, such as in IrcII-esque bindings
 */
int TString::match (register char *m, register char *n, const bool cs /* case sensitive */)
{
	char *ma = m, *lsm = 0, *lsn = 0, *lpm = 0, *lpn = 0;
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
			while ((*m == WILDT) || (*m == ' '));	/*  for each space or ~ */
			sofar += space;				/* Each counts as exact */
			while (*n == ' ') {
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
				while ((m > ma) && (*m == '?'));	/* For first non-? char */
				if ((m > ma) ? ((*m == '*') && (m[-1] != QUOTE)) : (*m == '*'))
					return MATCH;	/* nonquoted * = match */
				break;
			case WILDP:
				while (*(++m) == WILDP);	/* Zap redundant %s */
				if (*m != WILDS) {		/* Don't both if next=* */
					if (*n != ' ') {	/* WILDS can't match ' ' */
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
					m++;			/* Zap redundant wilds */
				while ((*m == WILDS) || (*m == WILDP));
				lsm = m;
				lsn = n;
				lpm = 0;		/* Save '*' fallback spot */
				match += (saved + sofar);	/* Save tally count */
				saved = sofar = 0;
				continue;		/* Done with '*' */
			case WILDQ:
				m++;
				n++;
				continue;		/* Match one char */
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
		if (lpm) {			/* Try to fallback on '%' */
			n = ++lpn;
			m = lpm;
			sofar = 0;		/* Restore position */
			if ((*n | 32) == 32)
				lpm = 0;		/* Can't match 0 or ' ' */
			continue;			/* Next char, please */
		}
		if (lsm) {			/* Try to fallback on '*' */
			n = ++lsn;
			m = lsm;			/* Restore position */
			/* Used to test for (!*n) here but it wasn't necessary so it's gone */
			saved = sofar = 0;
			continue;			/* Next char, please */
		}
		return NOMATCH;		/* No fallbacks=No match */
	}
	while ((*m == WILDS) || (*m == WILDP))
		m++;			/* Zap leftover %s & *s */
	return (*m) ? NOMATCH : MATCH;	/* End of both = match */
}
TString TString::wildtok( char * wildString, int N, const char * sepChars ) const
{
	int cnt = 1, m = 0;

	if ( sepChars == NULL || this->m_pString == NULL )
		return "";

	int nToks = this->numtok( sepChars );

	if ( N > nToks )
		return "";

	for (TString tmp(this->gettok(cnt++,sepChars)); tmp != ""; tmp = this->gettok(cnt++,sepChars))
	{
		if (match(wildString,tmp.to_chr(),false)) {
			m++;
			if (m == N) return tmp;
		}
	}
	return "";
}
int TString::nwildtok( char * wildString, const char * sepChars ) const
{
	int cnt = 1, m = 0;

	if ( sepChars == NULL || this->m_pString == NULL )
		return 0;

	for (TString tmp(this->gettok(cnt++,sepChars)); tmp != ""; tmp = this->gettok(cnt++,sepChars))
	{
		if (match(wildString,tmp.to_chr(),false))
			m++;
	}
	return m;
}
int TString::tsprintf(const char *fmt, ...)
{
	va_list args;
	va_start( args, fmt );
	int cnt = tvprintf(fmt, &args);
	va_end( args );
	return cnt;
}

int TString::tvprintf(const char *fmt, va_list * args)
{
	int cnt = _vscprintf(fmt, *args);
	if (cnt > 0) {
		char *txt = new char[++cnt];
		if (txt != NULL) {
			// warning C4996: 'vsprintf' was declared deprecated
			// http://forums.microsoft.com/MSDN/ShowPost.aspx?PostID=10254&SiteID=1
			vsprintf_s(txt, cnt, fmt, *args );
		}
		else
			cnt = 0;
		this->deleteString();
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
bool TString::iswm(char *a) const
{
	return match(a, this->m_pString,false)?true:false;
}

/*
 * iswmcs(*mask*)
 *    returns TRUE if *mask* matches string
 *    else it returns FALSE
 *    case sensitive
 */
bool TString::iswmcs(char *a) const
{
	return match(a, this->m_pString,true)?true:false;
}
//void TString::strncpy(const char *cString, const int n)
//{
//	this->deleteString();
//	this->m_pString = new char[n+1];
//	lstrcpyn(this->m_pString,cString,n);
//}
//void TString::strncpy(const TString &tString, const int n)
//{
//	this->deleteString();
//	this->m_pString = new char[n+1];
//	lstrcpyn(this->m_pString,tString.to_chr(),n);
//}
//void TString::strncat(const char *cString, const int n)
//{
//	char *tmp = new char[n + lstrlen(this->m_pString) + 1];
//	lstrcpy(tmp,this->m_pString);
//	::strncat(tmp,cString,n);
//	this->deleteString();
//	this->m_pString = tmp;
//}
//void TString::strncat(const TString &tString, const int n)
//{
//	char *tmp = new char[n + lstrlen(this->m_pString) + 1];
//	lstrcpy(tmp,this->m_pString);
//	::strncat(tmp,tString.to_chr(),n);
//	this->deleteString();
//	this->m_pString = tmp;
//}
WCHAR *TString::to_wchr(bool tryutf8)
{
	if (this->m_pWString != NULL)
		return this->m_pWString;

	if (this->m_pString == NULL)
		return NULL;

	//int widelen = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,this->m_pString,-1, NULL, 0);
	//this->m_pWString = new WCHAR[widelen+1];
	//MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,this->m_pString,-1, this->m_pWString, widelen);

	// this method also allows UTF8 decoding.
	//int widelen = MultiByteToWideChar(CP_UTF8,0,this->m_pString,-1, NULL, 0);
	//this->m_pWString = new WCHAR[widelen+1];
	//MultiByteToWideChar(CP_UTF8,0,this->m_pString,-1, this->m_pWString, widelen);

	// try UTF8 encoded first, but error on invalid chars.
	//int widelen = MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,this->m_pString,-1, NULL, 0);
	//if (widelen == 0) {
	//	// zero result, error maybe?
	//	if (GetLastError() == ERROR_NO_UNICODE_TRANSLATION) {
	//		// invalid chars, assume its NOT a utf8 string then, try CP_ACP
	//		widelen = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,this->m_pString,-1, NULL, 0);
	//		if (widelen != 0) {
	//			this->m_pWString = new WCHAR[widelen+1];
	//			MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,this->m_pString,-1, this->m_pWString, widelen);
	//		}
	//	}
	//	// if no error, assume zero length string.
	//}
	//else {
	//	this->m_pWString = new WCHAR[widelen+1];
	//	MultiByteToWideChar(CP_UTF8,0,this->m_pString,-1, this->m_pWString, widelen);
	//}
	// If utf8 input expected try & do a utf8->utf16 convert.
	int widelen;
	if (tryutf8) {
		// try UTF8 encoded first, but error on invalid chars.
		widelen = MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,this->m_pString,-1, NULL, 0);
		if (widelen == 0) {
			// zero result, error maybe?
			if (GetLastError() == ERROR_NO_UNICODE_TRANSLATION) {
				// invalid chars, assume its NOT a utf8 string then, try ascii->utf16
				widelen = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,this->m_pString,-1, NULL, 0);
				if (widelen != 0) {
					this->m_pWString = new WCHAR[widelen+1];
					MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,this->m_pString,-1, this->m_pWString, widelen);
				}
			}
			// if no error, assume zero length string.
		}
		else {
			this->m_pWString = new WCHAR[widelen+1];
			MultiByteToWideChar(CP_UTF8,0,this->m_pString,-1, this->m_pWString, widelen);
		}
	}
	else {
		// otherwise just do an ascii->utf16
		widelen = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,this->m_pString,-1, NULL, 0);
		if (widelen != 0) {
			this->m_pWString = new WCHAR[widelen+1];
			MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,this->m_pString,-1, this->m_pWString, widelen);
		}
	}
	//int utf8len = MultiByteToWideChar(CP_UTF8,0,this->m_pString,-1, NULL, 0);
	//int widelen = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,this->m_pString,-1, NULL, 0);
	//if (utf8len < widelen) {
	//	this->m_pWString = new WCHAR[widelen+utf8len+1];
	//	WCHAR *utf8buf = new WCHAR[widelen+1];
	//	WCHAR *widebuf = new WCHAR[widelen+1];

	//	MultiByteToWideChar(CP_UTF8,0,this->m_pString,-1, utf8buf, utf8len);
	//	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,this->m_pString,-1, widebuf, widelen);

	//	for (int i = 0; i < widelen; i++)
	//	{
	//		if (utf8buf[i] == widebuf[i])
	//			this->m_pWString[i] = utf8buf[i];
	//		else {
	//		}
	//	}
	//	delete [] widebuf;
	//	delete [] utf8buf;
	//}
	//else
	//{
	//	// otherwise just do an ascii->utf16
	//	if (widelen != 0) {
	//		this->m_pWString = new WCHAR[widelen+1];
	//		MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,this->m_pString,-1, this->m_pWString, widelen);
	//	}
	//}
	return this->m_pWString;
}
//int wildcmp(const char *wild, const char *string) {
//	// Written by Jack Handy - jakkhandy@hotmail.com
//	const char *cp = NULL, *mp = NULL;
//
//	while ((*string) && (*wild != '*')) {
//		if ((*wild != *string) && (*wild != '?')) {
//			return 0;
//		}
//		wild++;
//		string++;
//	}
//
//	while (*string) {
//		if (*wild == '*') {
//			if (!*++wild) {
//				return 1;
//			}
//			mp = wild;
//			cp = string+1;
//		} else if ((*wild == *string) || (*wild == '?')) {
//			wild++;
//			string++;
//		} else {
//			wild = mp;
//			string = cp++;
//		}
//	}
//
//	while (*wild == '*') {
//		wild++;
//	}
//	return !*wild;
//}
/*!
 * \brief blah
 *
 * blah
 */
TString &TString::strip() {
	TCHAR *start = this->m_pString;
	TCHAR *end = this->m_pString + lstrlen(this->m_pString);

	if ((start == NULL) || (end == NULL))
		return *this;

	// Trim from start
	while (start != end && *start == TEXT(' '))
		start++;

	// Trim from end
	while (end != start && *(--end) == TEXT(' '));

	size_t new_len = end - start +1;

	TCHAR *temp = new TCHAR[new_len +1];
	temp[new_len] = 0;

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
				if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9') {
					++pos;

					if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9')
						pos++;

					// maybe a background color
					if (wtxt[pos+1] == L',') {
						++pos;

						if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9') {
							pos++;

							if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9')
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

	this->deleteString();
	this->m_pString = temp;

	return *this;
}
