/*!
 * \file tstring.h
 * \brief String and Token Management Class
 *
 * This experimental library was built to reproduce easy to use string containers managing effectively 
 * comparisons and token manipulations as done in the mIRC scripting language.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.5
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
 *
 * © ScriptsDB.org - 2005
 */

#ifndef _TSTRING_H_
#define _TSTRING_H_

//#include <stdio.h>
#include <stdlib.h>
//#include <string.h>

/*!
 * \brief String and Token Management Class
 */
class TString {

private:

  void deleteString( );
  int _replace(const char *subString, const char *rString); // Ook

public:

  char * m_pString; //!< String buffer

  TString( );
  TString( const char * cString );
  TString( const char chr );
  TString( const TString & tString );
  
  //! Destructor
  ~TString( );

  /*! \fn char * to_chr( )
      \brief Returns the string as a C char string
  */
  char * to_chr( ) { return this->m_pString; };

  // Operator Overloads
  void operator =( const TString & tString );
  void operator =( const char * cString );
  void operator =( const char chr );

  TString operator +( const char * cString );
  TString operator +( const char chr );
  TString operator +( const TString & tString );

  void operator +=( const char * cString );
  void operator +=( const char chr );
  void operator +=( const TString & tString );

  bool operator ==( const int iNull );
  bool operator ==( const char * cString );
  bool operator ==( const char chr );
  bool operator ==( const TString & tString );

  bool operator !=( const int iNull );
  bool operator !=( const char * cString );
  bool operator !=( const char chr );
  bool operator !=( const TString & tString );

  bool operator >( const char * cString );
  bool operator >( const char chr );
  bool operator >( const TString & tString );

  bool operator >=( const char * cString );
  bool operator >=( const char chr );
  bool operator >=( const TString & tString );

  bool operator <( const char * cString );
  bool operator <( const char chr );
  bool operator <( const TString & tString );

  bool operator <=( const char * cString );
  bool operator <=( const char chr );
  bool operator <=( const TString & tString );

  TString operator *( const int N );
  void operator *=( const int N );

  char & operator []( long int N );

  // General String Lib
  size_t len( );

  int find( const char * substring, int N );
  int find( const char chr, int N );

  TString sub( int N, int M );

  void trim( );
  
  int replace( const char * subString, const char * rString );
  int replace( const char * subString, const char rchr );
  int replace( const char chr, const char * rString );
  int replace( const char chr, const char rchr );

  // Token Lib
  void addtok( char * cToken, char * sepChars );
  void deltok( int N, char * sepChars );
  int findtok( char * cToken, int N, char * sepChars );
  TString gettok( int N, char * sepChars );
  TString gettok( int N, int M, char * sepChars );
  void instok( char * cToken, int N, char * sepChars );
  bool istok( char * cToken, char * sepChars );
  TString matchtok( char * mString, int N, char * sepChars );
  int numtok( char * sepChars );
  void puttok( char * cToken, int N, char * sepChars );
  void remtok( char * cToken, int N, char * sepChars );
  void reptok( char * cToken, char * newToken, int N, char * sepChars );
  void sorttok( char * sortOptions, char * sepChars );
  // added by Ook
  TString wildtok( char * wildString, int N, char * sepChars );
  int nwildtok( char * wildString, char * sepChars );
  TString mid(int pos, int n);
  TString left(int n);
  TString right(int n);
  //
};

#endif // TSTRING_H_