/*! 
 * \file TString.cpp
 * \brief String and Token Management Class Functions
 *
 * This experimental library was built to reproduce easy to use string containers managing effectively 
 * comparisons and token manipulations as done in the mIRC scripting language.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.6
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
 *
 * © ScriptsDB.org - 2005
 */

#include "TString.h"

#include <windows.h>

/*! 
 * \brief Default Constructor
 */
/****************************/

TString::TString( ) {

  this->m_pString = new char[1];
  //this->m_pString = 0;
  this->m_pString[0] = 0; // change by Ook
}

/****************************/
/*! \fn TString::TString( const char * cString )
    \brief Constructor input from C char string

    \param cString Character string to initialize string to
*/
/****************************/

TString::TString( const char * cString ) {

  this->m_pString = NULL;

  if ( cString != NULL ) {

    int size = lstrlen( cString );
    this->m_pString = new char [ size + 1 ];
    lstrcpy( this->m_pString, cString );
  }
  else {
    this->m_pString = new char[1];
    //this->m_pString = 0;
    this->m_pString[0] = 0; // change by Ook
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
  this->m_pString[0] = chr;
  this->m_pString[1] = 0;
}

/****************************/
/*! \fn TString::TString( const TString & tString )
    \brief Copy Constructor

    \param tString TString object to copy
*/
/****************************/

TString::TString( const TString & tString ) {

  if ( tString.m_pString != NULL ) {

    size_t size = lstrlen( tString.m_pString );
    this->m_pString = new char [size+1];
    lstrcpy( this->m_pString, tString.m_pString );
  }
  else {
    this->m_pString = new char[1];
    //this->m_pString = 0;
    this->m_pString[0] = 0; // change By Ook
  }
}

TString::~TString( ) {

  //printf( "Destructor: %x\n", this->m_pString );
  this->deleteString( );
}

/*!
 * \brief Deletes the allocated char buffer
 */
void TString::deleteString( ) {

  if ( this->m_pString != NULL ) {

    delete []this->m_pString; 
    this->m_pString = NULL;
  }
}

/****************************/
void TString::operator =( const TString & tString ) {

  //printf( "Equality operator\n" );

  this->deleteString( );

  if ( tString.m_pString != NULL ) {

    size_t size = lstrlen( tString.m_pString );
    this->m_pString = new char [ size + 1 ];
    lstrcpy( this->m_pString, tString.m_pString );
  }
}

/****************************/
/*! \fn void TString::operator=( const char * string )
    \brief Symbol Overload - TString assignement from C char string

    \param cString Character string to assign string to
*/
/****************************/
void TString::operator =( const char * cString ) {

  this->deleteString( );

  if ( cString != NULL ) {

    size_t size = lstrlen( cString );
    this->m_pString = new char [ size + 1 ];
    lstrcpy( this->m_pString, cString );
  }
}

/****************************/
/*! \fn void TString::operator=( const char chr )
    \brief Symbol Overload - TString assignement from C char

    \param chr Character to assign string to
*/
/****************************/
void TString::operator =( const char chr ) {

  this->deleteString( );
  this->m_pString = new char[2];
  this->m_pString[0] = chr;
  this->m_pString[1] = 0;
}

/****************************/
/*! \fn TString TString::operator +( const char * string )
    \brief Symbol Overload - TString and char string concatenation

    \param cString Character string to concatenate

    \return Returns the new TString object
*/
/****************************/

TString TString::operator +( const char * cString ) {

  TString newTString;

  if ( cString != NULL ) {
	delete [] newTString.m_pString; // change by Ook
    newTString.m_pString = new char [lstrlen( this->m_pString )+lstrlen( cString )+1];
    lstrcpy( newTString.m_pString, this->m_pString );
    lstrcat( newTString.m_pString, cString );
  }
  else
    newTString = *this;

  return newTString;
}

/****************************/
/*! \fn TString TString::operator +( const char chr )
    \brief Symbol Overload - TString and char concatenation

    \param chr Character to concatenant

    \return Returns the new TString object
*/
/****************************/

TString TString::operator +( const char chr ) {

  TString newTString;

  size_t size = lstrlen( this->m_pString );
	delete [] newTString.m_pString; // change by Ook
  newTString.m_pString = new char [ size + 2 ];
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

  TString newTString;

  if ( tString.m_pString ) {

	delete [] newTString.m_pString; // change by Ook
    newTString.m_pString = new char [lstrlen( this->m_pString )+lstrlen( tString.m_pString )+1];
    lstrcpy( newTString.m_pString, this->m_pString );
    lstrcat( newTString.m_pString, tString.m_pString );
  }
  else
    newTString = *this;
  
  return newTString;
}

/****************************/
/*! \fn void TString::operator +=( const char * string )
    \brief Symbol Overload - TString and char string concatenation

    \param cString Character string to concatenate
*/
/****************************/

void TString::operator +=( const char * cString ) {

  if ( cString != NULL ) {
    //int len = lstrlen( this->m_pString );
    //char * temp = new char[ len + 1 ];
    //lstrcpy( temp, this->m_pString );
    //this->deleteString( );
    //this->m_pString = new char [ len + lstrlen( cString ) + 1 ];
    //lstrcpy( this->m_pString, temp );
    //lstrcat( this->m_pString, cString );
    //delete []temp;
	char * temp = new char [ lstrlen( this->m_pString ) + lstrlen( cString ) + 1 ];
    lstrcpy( temp, this->m_pString );
    lstrcat( temp, cString );
	delete [] this->m_pString;
	this->m_pString = temp;
  }
}

/****************************/
/*! \fn void TString::operator +=( const char chr )
    \brief Symbol Overload - TString and char concatenation

    \param chr Character to concatenate
*/
/****************************/

void TString::operator +=( const char chr ) {

  if ( this->m_pString != NULL ) {
    //int len = lstrlen( this->m_pString );
    //char * temp = new char[ len + 1 ];
    //lstrcpy( temp, this->m_pString );
    //this->deleteString( );
    //this->m_pString = new char [ len + 2 ];
    //lstrcpy( this->m_pString, temp );
    //this->m_pString[ len ] = chr;
    //this->m_pString[ len + 1 ] = 0;
    //delete []temp;
	int len = lstrlen( this->m_pString );
	char * temp = new char [ len + 2 ];
    lstrcpy( temp, this->m_pString );
	temp[len] = chr;
	temp[len+1] = 0;
	delete [] this->m_pString;
	this->m_pString = temp;
  }
  else {
    this->m_pString = new char[2];
    this->m_pString[0] = chr;
    this->m_pString[1] = 0;
  }
}

/****************************/
/*! \fn void TString::operator +=( const TString & tString )
    \brief Symbol Overload - TString and TString concatenation

    \param tString TString object to concatenate
*/
/****************************/

void TString::operator +=( const TString & tString ) {

  if ( tString.m_pString ) {
    //int len = lstrlen( this->m_pString );
    //char * temp = new char[ len + 1 ];
    //lstrcpy( temp, this->m_pString );
    //this->deleteString( );
    //this->m_pString = new char [ len + lstrlen( tString.m_pString ) + 1 ];
    //lstrcpy( this->m_pString, temp );
    //lstrcat( this->m_pString, tString.m_pString );
    //delete []temp;
	char * temp = new char [ lstrlen( this->m_pString ) + lstrlen( tString.m_pString ) + 1 ];
    lstrcpy( temp, this->m_pString );
    lstrcat( temp, tString.m_pString );
	delete [] this->m_pString;
	this->m_pString = temp;
  }
}

/****************************/
/*! \fn bool TString::operator ==( const int iNull )
    \brief Symbol Overload - TString and NULL comparison

    \param iNull Value of comparison (usually 0 for NULL)

    \return Returns \b true or \b false based on the comparison
*/

bool TString::operator ==( const int iNull ) {

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

bool TString::operator ==( const char * cString ) {

  if ( cString && this->m_pString ) {

    if ( !lstrcmp( this->m_pString, cString ) )
      return true;
    else 
      return false;
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

bool TString::operator ==( const char chr ) {

  if ( this->m_pString ) {

    char temp[2];
    temp[0] = chr;
    temp[1] = 0;

    if ( !lstrcmp( this->m_pString, temp ) )
      return true;
    else 
      return false;
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

bool TString::operator ==( const TString & tString ) {

  if ( this->m_pString && tString.m_pString ) {

    if ( !lstrcmp( this->m_pString, tString.m_pString ) )
      return true;
    else
      return false;
  }
  return false;
}

/****************************/
/*! \fn bool TString::operator !=( const int iNull )
    \brief Symbol Overload - TString and NULL comparison

    \param iNull Value of comparison (usually 0 for NULL)

    \return Returns \b true or \b false based on the comparison
*/

bool TString::operator !=( const int iNull ) {

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

bool TString::operator !=( const char * cString ) {

  if ( cString && this->m_pString ) {

    if ( !lstrcmp( this->m_pString, cString ) )
      return false;
    else 
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

bool TString::operator !=( const char chr ) {

  if ( this->m_pString != NULL ) {

    char temp[2];
    temp[0] = chr;
    temp[1] = 0;

    if ( !lstrcmp( this->m_pString, temp ) )
      return false;
    else 
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

bool TString::operator !=( const TString & tString ) {

  if ( this->m_pString != NULL && tString.m_pString != NULL ) {

    if ( !lstrcmp( this->m_pString, tString.m_pString ) )
      return false;
    else
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

bool TString::operator >( const char * cString ) {

  if ( cString != NULL && this->m_pString != NULL ) {

    if ( lstrcmp( this->m_pString, cString ) > 0 )
      return true;
    else 
      return false;
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

bool TString::operator >( const char chr ) {

  if ( this->m_pString != NULL ) {

    char temp[2];
    temp[0] = chr;
    temp[1] = 0;

    if ( lstrcmp( this->m_pString, temp ) > 0 )
      return true;
    else 
      return false;
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

bool TString::operator >( const TString & tString ) {

  if ( this->m_pString != NULL && tString.m_pString != NULL ) {

    if ( lstrcmp( this->m_pString, tString.m_pString ) > 0 )
      return true;
    else
      return false;
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

bool TString::operator >=( const char * cString ) {

  if ( cString != NULL && this->m_pString != NULL ) {

    if ( lstrcmp( this->m_pString, cString ) >= 0 )
      return true;
    else 
      return false;
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

bool TString::operator >=( const char chr ) {

  if ( this->m_pString != NULL ) {

    char temp[2];
    temp[0] = chr;
    temp[1] = 0;

    if ( lstrcmp( this->m_pString, temp ) >= 0 )
      return true;
    else 
      return false;
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

bool TString::operator >=( const TString & tString ) {

  if ( this->m_pString != NULL && tString.m_pString != NULL ) {

    if ( lstrcmp( this->m_pString, tString.m_pString ) >= 0 )
      return true;
    else
      return false;
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

bool TString::operator <( const char * cString ) {

  if ( cString != NULL && this->m_pString != NULL ) {

    if ( lstrcmp( this->m_pString, cString ) < 0 )
      return true;
    else 
      return false;
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

bool TString::operator <( const char chr ) {

  if ( this->m_pString != NULL ) {

    char temp[2];
    temp[0] = chr;
    temp[1] = 0;

    if ( lstrcmp( this->m_pString, temp ) < 0 )
      return true;
    else 
      return false;
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

bool TString::operator <( const TString & tString ) {

  if ( this->m_pString != NULL && tString.m_pString != NULL ) {

    if ( lstrcmp( this->m_pString, tString.m_pString ) < 0 )
      return true;
    else
      return false;
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

bool TString::operator <=( const char * cString ) {

  if ( cString != NULL && this->m_pString != NULL ) {

    if ( lstrcmp( this->m_pString, cString ) <= 0 )
      return true;
    else 
      return false;
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

bool TString::operator <=( const char chr ) {

  if ( this->m_pString != NULL ) {

    char temp[2];
    temp[0] = chr;
    temp[1] = 0;

    if ( lstrcmp( this->m_pString, temp ) <= 0 )
      return true;
    else 
      return false;
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

bool TString::operator <=( const TString & tString ) {

  if ( this->m_pString != NULL && tString.m_pString != NULL ) {

    if ( lstrcmp( this->m_pString, tString.m_pString ) <= 0 )
      return true;
    else
      return false;
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

TString TString::operator *( const int N ) {

  TString newTString;

  if ( N == 0 )
    return newTString;

  if ( N < 0 || N == 1 )
    return *this;

  size_t size = lstrlen( this->m_pString );
	delete [] newTString.m_pString; // change by Ook
  newTString.m_pString = new char [size*N+1];
  newTString.m_pString[0] = 0;

  int i = 0;
  while ( i < N ) {

    lstrcat( newTString.m_pString, this->m_pString );
    i++;
  }
  return newTString;
}

/****************************/
/*! \fn void TString::operator *=( const int N )
    \brief Symbol Overload - TString string Repetition

    \param N Number of repetitions

    \note Using \b 0 for N makes the string being empty
*/
/****************************/

void TString::operator *=( const int N ) {

  if ( N == 0 ) {
    this->deleteString( );
    return;
  }

  if ( N == 1 )
    return;

  //size_t size = lstrlen( this->m_pString );
  //char * temp = new char [size+1];
  //lstrcpy( temp, this->m_pString );

  //this->deleteString( );

  //this->m_pString = new char [size*N+1];
  //this->m_pString[0] = 0;

  //int i = 0;
  //while ( i < N ) {

  //  lstrcat( this->m_pString, temp );
  //  i++;
  //}
  //delete []temp;
  if (this->m_pString == NULL) // if string is null just return.
	  return;
  char *temp = this->m_pString;
  this->m_pString = new char [(lstrlen( temp )*N)+1];
  this->m_pString[0] = 0;

  int i = 0;
  while ( i < N ) {
    lstrcat( this->m_pString, temp );
    i++;
  }
  delete [] temp;
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

char & TString::operator []( long int N ) {

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

size_t TString::len( ) {

  if ( this->m_pString )
    return lstrlen( this->m_pString );

  return 0;
}

/****************************/
/*! \fn int TString::find( const char * substring, int N )
    \brief Function to find position or number of occurences of a substring in the string

    \param substring Substring to search
    \param N Nth substring to search (N = \b 0 > Total number of matches)

    \return > Number of occurences (N = 0)\n
            > Starting position of \b substring \n
            > \b -1 if function fails or no substring was found
*/
/****************************/

int TString::find( const char * substring, int N ) {

  if ( substring && this->m_pString ) {

    char * temp, * temp2 = this->m_pString;

    int i = 0;
	int subl = lstrlen( substring ); // Ook
    while ( ( temp = strstr( temp2, substring ) ) != NULL ) {

      i++;

      if ( N != 0 && i == N ) {

        return (int) (temp - this->m_pString);
      }

      //temp2 = (temp + lstrlen( substring ));
      temp2 = (temp + subl); // Ook
    }
    if (N == 0)
      return i;
  }
  return -1;
}

/****************************/
/*! \fn int TString::find( const char chr, int N )
    \brief Function to find position or number of occurences of a char in the string
*/
/****************************/

int TString::find( const char chr, int N ) {


  return -1;
}


/****************************/
/*! \fn TString TString::findtok(char * cToken, int N, char * sepChars)
    \brief Returns the index of a specific token

	 \param cToken the token to find
    \param N the Nth match
    \param sepChars seperating character

    \return index of found token

    \note > Index starts at \b 1 \n
*/
/****************************/
int TString::findtok(char * cToken, int N, char * sepChars) {
	int count = 0;
	TString str(cToken);

	for (int i = 1; i <= this->numtok(sepChars); i++) {
		if (this->gettok(i, sepChars) == str) {
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

TString TString::sub( int N, int M ) {

  TString newTString;

  if ( this->m_pString ) {

    int size = (int) lstrlen( this->m_pString );

    if ( N < 0 ) N += (long int) size;

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

/*
	internal replace function, called by the other replace() functions.
	Ook
*/
int TString::_replace(const char *subString, const char *rString)
{
	if ((subString == NULL) || (rString == NULL) || (this->m_pString == NULL)) return 0;
	char *sub = NULL, *p = this->m_pString;
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
		char *out = NULL;
		p = this->m_pString;
		out = new char [ (cnt * repl) + (ol - (cnt * subl)) +1 ]; // allocate new string.
		out[0] = 0;
		while ((sub = strstr(p,subString)) != NULL) {
			strncat(out,p,(sub - p)); // copy bit before substring. if any.
			lstrcat(out,rString); // append new replacement string.
			p = sub + subl; // update pointer to skip substring.
		}
		lstrcat(out,p); // append the end text, if any.
		delete [] this->m_pString; // delete old string
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
	return this->_replace(subString,rString);
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
  return this->_replace(subString,tmp);
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
	return this->_replace(tmp,rString);
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

TString TString::gettok( int N, char * sepChars ) {

  //printf( "gettok\n" );
  TString token = *this;
  //token = this->m_pString;

  if ( sepChars == NULL || this->m_pString == NULL )
    return token;

  int nToks = this->numtok( sepChars );

  if ( N > nToks ) {

    token = "";
    return token;
  }

  if ( N < 0 ) {

    N += ( nToks + 1 );
    if ( N < 1 ) {

      token = "";
      return token;
    }
  }

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
    //p_cStart = p_cEnd + lstrlen( sepChars );
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
  else if ( p_cEnd == NULL ) {
    token = "";
  }

  return token;
}

/*!
 * \brief blah
 *
 * blah
 */

TString TString::gettok( int N, int M, char * sepChars ) {

  //printf( "gettok\n" );
  TString token = *this;
  //token = this->m_pString;

  if ( sepChars == NULL || this->m_pString == NULL )
    return token;

  int nToks = this->numtok( sepChars );

  if ( N == M )
    return this->gettok( N, sepChars );

  if ( M <= N && M != -1 ) {

    token = "";
    return token;
  }

  if ( M > nToks - 1 )
    M = -1;

  if ( N > nToks ) {

    token = "";
    return token;
  }

  if ( N < 0 ) {

    N += ( nToks + 1 );
    if ( N < 1 ) {

      token = "";
      return token;
    }
  }

  char * p_cStart = this->m_pString, * p_cEnd = this->m_pString;
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

    //p_cStart = p_cEnd + lstrlen( sepChars );
    p_cStart = p_cEnd + sepl;
  }

  if ( M == -1 ) {

    if ( iCount == nToks - 1 )
      p_cFirst = p_cStart;

    token = p_cFirst;
  }
  else {

    int len =  p_cLast - p_cFirst + 1;

    if ( token.m_pString != NULL )
      token.deleteString( );

    token.m_pString = new char[ len + 1 ];
    lstrcpyn( token.m_pString, p_cFirst, len + 1 );
  }

  return token;
}


/*!
 * \brief blah
 *
 * blah
 */

int TString::numtok( char * sepChars ) {

  if ( sepChars == NULL || this->m_pString == NULL )
    return 0;

  char * p_cStart = this->m_pString, * p_cEnd = this->m_pString;
  int iCount = 0;
	int sepl = lstrlen( sepChars ); // Ook

  while ( ( p_cEnd = strstr( p_cStart, sepChars ) ) != NULL ) {

    iCount++;
    //p_cStart = p_cEnd + lstrlen( sepChars );
    p_cStart = p_cEnd + sepl; // Ook
  }
  return iCount + 1;
}

/*!
 * \brief blah
 *
 * blah
 */

void TString::deltok( int N, char * sepChars ) {

  if ( sepChars == NULL || this->m_pString == NULL )
    return;

  if ( N > this->numtok( sepChars ) || N < 1 )
    return;

  if ( N == 1 && this->numtok( sepChars ) == 1 ) {

    delete [] this->m_pString;
    this->m_pString = NULL;
    return;
  }

  char * p_cStart = this->m_pString, * p_cEnd = this->m_pString;
  long int i = 0;
	int sepl = lstrlen( sepChars ); // Ook

  while ( ( p_cEnd = strstr( p_cStart, sepChars ) ) != NULL ) {

    i++;

    if ( i == N )
      break;

    //p_cStart = p_cEnd + lstrlen( sepChars );
    p_cStart = p_cEnd + sepl; // Ook
  }

  // delete the first token
  if ( p_cStart == this->m_pString ) {

    p_cEnd++;

    char * pNew = new char[ lstrlen( p_cEnd ) + 1 ];
    lstrcpy( pNew, p_cEnd );
    delete [] this->m_pString;
    this->m_pString = pNew;
  }
  // last token
  else if ( p_cEnd == NULL ) {

    p_cStart--;
    *p_cStart = 0;

    char * pNew = new char[ lstrlen( this->m_pString ) + 1 ];
    lstrcpy( pNew, this->m_pString );
    delete [] this->m_pString;
    this->m_pString = pNew;
  }
  // inbound token
  else {

    *p_cStart = 0;
    p_cEnd++;

    char * pNew = new char[ lstrlen( this->m_pString ) + lstrlen( p_cEnd ) + 1 ];
    lstrcpy( pNew, this->m_pString );
    lstrcat( pNew, p_cEnd );

    delete [] this->m_pString;
    this->m_pString = pNew;
  }
}
/*!
 * \brief blah
 *
 * blah
 */

void TString::instok( char * cToken, int N, char * sepChars ) {

  if ( cToken == NULL || sepChars == NULL || this->m_pString == NULL )
    return;

  if ( N < 1 )
    return;

  char * p_cStart = this->m_pString, * p_cEnd = this->m_pString;
  long int i = 1;
	int sepl = lstrlen( sepChars ); // Ook

  while ( ( p_cEnd = strstr( p_cStart, sepChars ) ) != NULL ) {

    if ( i == N )
      break;

    i++;

    //p_cStart = p_cEnd + lstrlen( sepChars );
    p_cStart = p_cEnd + sepl; // Ook
  }

  char * pNew = new char[ lstrlen( cToken ) + lstrlen( this->m_pString ) + lstrlen( sepChars ) + 1 ];

  // delete the first token
  if ( p_cStart == this->m_pString ) {

    lstrcpy( pNew, cToken );
    lstrcat( pNew, sepChars );
    lstrcat( pNew, this->m_pString );
    delete [] this->m_pString;
    this->m_pString = pNew;
  }
  else if ( p_cEnd == NULL ) {

    if ( i == N ) {

      //p_cStart -= lstrlen( sepChars );
      p_cStart -= sepl; // Ook
      *p_cStart = 0;
      //p_cStart += lstrlen( sepChars );
      p_cStart += sepl; // Ook

      lstrcpy( pNew, this->m_pString );
      lstrcat( pNew, sepChars );
      lstrcat( pNew, cToken );
      lstrcat( pNew, sepChars );
      lstrcat( pNew, p_cStart );
      delete [] this->m_pString;
      this->m_pString = pNew;

    }
    else {

      lstrcpy( pNew, this->m_pString );
      lstrcat( pNew, sepChars );
      lstrcat( pNew, cToken );
      delete [] this->m_pString;
      this->m_pString = pNew;
    }
  }
  else {

    //p_cStart -= lstrlen( sepChars );
    p_cStart -= sepl; // Ook
    *p_cStart = 0;
    //p_cStart += lstrlen( sepChars );
    p_cStart += sepl; // Ook

    lstrcpy( pNew, this->m_pString );
    lstrcat( pNew, sepChars );
    lstrcat( pNew, cToken );
    lstrcat( pNew, sepChars );
    lstrcat( pNew, p_cStart );
    delete [] this->m_pString;
    this->m_pString = pNew;
  }
}

/*!
 * \brief blah
 * added by Ook
 * blah
 */
void TString::addtok( char * cToken, char * sepChars ) {

  if ( cToken == NULL || sepChars == NULL || this->m_pString == NULL )
    return;
  char * pNew = NULL;
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
  delete [] this->m_pString;
  this->m_pString = pNew;
}

bool TString::istok(char * cToken, char * sepChars ) {

  if ( sepChars == NULL || this->m_pString == NULL )
    return false;

  char * p_cStart = this->m_pString, * p_cEnd = this->m_pString;
	int l = 0;
	int sepl = lstrlen( sepChars );

  while ( ( p_cEnd = strstr( p_cStart, sepChars ) ) != NULL ) {
		l = (int)(p_cEnd - p_cStart);
		if (l > 0) {
			if (strncmp(cToken,p_cStart,l) == 0) return true;
		}
    p_cStart = p_cEnd + sepl;
  }
	if (strcmp(cToken,p_cStart) == 0) return true;

  return false;
}

/*!
 * \brief blah
 *
 * blah
 */

void TString::puttok( char * cToken, int N, char * sepChars ) {

  if ( cToken == NULL || sepChars == NULL || this->m_pString == NULL )
    return;

  char * p_cStart = this->m_pString, * p_cEnd = this->m_pString;
  long int i = 0;
	int sepl = lstrlen( sepChars ); // Ook

  while ( ( p_cEnd = strstr( p_cStart, sepChars ) ) != NULL ) {

    i++;

    if ( i == N )
      break;

    //p_cStart = p_cEnd + lstrlen( sepChars );
    p_cStart = p_cEnd + sepl; // Ook
  }

  // delete the first token
  if ( p_cStart == this->m_pString ) {

    //p_cEnd++;

    char * pNew = new char[ lstrlen( cToken ) + lstrlen( p_cEnd ) + 1 ];

    lstrcpy( pNew, cToken );
    lstrcat( pNew, p_cEnd );
    delete [] this->m_pString;
    this->m_pString = pNew;
  }
  // last token
  else if ( p_cEnd == NULL ) {

    //p_cStart--;
    *p_cStart = 0;

    char * pNew = new char[ lstrlen( cToken ) + lstrlen( this->m_pString ) + 1 ];

    lstrcpy( pNew, this->m_pString );
    lstrcat( pNew, cToken );
    delete [] this->m_pString;
    this->m_pString = pNew;
  }
  // inbound token
  else {

    *p_cStart = 0;
    //p_cEnd++;

    char * pNew = new char[ lstrlen( this->m_pString ) + lstrlen( cToken ) + lstrlen( p_cEnd ) + 1 ];

    lstrcpy( pNew, this->m_pString );
    lstrcat( pNew, cToken );
    lstrcat( pNew, p_cEnd );
    delete [] this->m_pString;
    this->m_pString = pNew;    
  }
}

/*!
 * \brief blah
 *
 * blah
 */

void TString::trim( ) {

  char * start = this->m_pString;
  char * end = this->m_pString + lstrlen( this->m_pString );

  while ( start != end && *start == 32 )
    start++;

  while ( end != start && *(--end) == 32 );

  size_t new_len = end - start + 1;

  char * temp = new char[new_len+1];
  temp[new_len] = 0;

  lstrcpyn( temp, start, new_len + 1 );

  if ( this->m_pString )
    delete this->m_pString;

  this->m_pString = temp;

}
// pos must be within length of string, n can be < 0
TString TString::mid(int pos, int n)
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
TString TString::left(int n)
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
TString TString::right(int n)
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
static unsigned char tolowertab[] = { 0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, ' ', '!', '"', '#', '$', '%', '&', 0x27, '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|',  '}', '~', '_',  '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|',  '}', '~', 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
static unsigned char touppertab[] = { 0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, ' ', '!', '"', '#', '$', '%', '&', 0x27, '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', 0x5f, '`', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
static inline int rfc_tolower(int c)
{
  return tolowertab[(unsigned char)(c)];
}
static inline int rfc_toupper(int c)
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
int match (register char *m, register char *n, bool cs /* case sensitive */)
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
TString TString::wildtok( char * wildString, int N, char * sepChars )
{
	TString tmp;
	int cnt = 1, m = 0;

	if ( sepChars == NULL || this->m_pString == NULL )
		return tmp;

	int nToks = this->numtok( sepChars );

	if ( N > nToks ) {
		tmp = "";
		return tmp;
	}
	tmp = this->gettok(cnt++,sepChars);
	while (tmp != "") {
		if (match(wildString,tmp.to_chr(),false)) {
			m++;
			if (m == N) return tmp;
		}
		tmp = this->gettok(cnt++,sepChars);
	}
	tmp = "";
	return tmp;
}
int TString::nwildtok( char * wildString, char * sepChars )
{
	TString tmp;
	int cnt = 1, m = 0;

	if ( sepChars == NULL || this->m_pString == NULL )
		return 0;

	tmp = this->gettok(cnt++,sepChars);
	while (tmp != "")
	{
		if (match(wildString,tmp.to_chr(),false)) {
			m++;
		}
		tmp = this->gettok(cnt++,sepChars);
	}
	return m;
}
int TString::sprintf(const char *fmt, ...)
{
  va_list args;
  va_start( args, fmt );
	int cnt = _vscprintf(fmt, args);
	char *txt = new char[cnt +1];
  vsprintf(txt, fmt, args );
	delete [] this->m_pString;
	this->m_pString = txt;
	va_end( args );
	return cnt;
}