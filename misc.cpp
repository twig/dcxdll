/*!
* \file misc.cpp
* \brief Misc Library File
*
* This file contains the main DCX dll routines.
*
* \author David Legault ( clickhere at scriptsdb dot org )
* \version 1.0
*
* \b Revisions
*
* © ScriptsDB.org - 2006
*/

#include "defines.h"

extern mIRCDLL mIRCLink; //!< blah

extern PFNSETTHEME SetWindowThemeUx;  //!< blah
extern BOOL XPPlus;                   //!< Is OS WinXP+ ?


/*!
* \brief Rounding function
*/

int round( float x ) { 
	if ( x - (float)(int) x > 0.5 ) 
		return (int) x + 1; 
	else 
		return (int) x;
}

/*!
* \brief blah
*
* blah
*/

void mIRCSignal( const char * data ) {
	/*
	logFile = fopen( "dcxlog.txt", "a" );

	if ( logFile ) {
	fwrite( data, sizeof( char ), strlen( data ), logFile );
	fclose( logFile );
	}
	*/

	wsprintf( mIRCLink.m_pData, "//.signal -n DCX %s", data );
	SendMessage( mIRCLink.m_mIRCHWND, WM_USER + 200, 0, 0 );
}

/*!
* \brief blah
*
* blah
*/

void mIRCError( const char * data ) {
	/*
	logFile = fopen( "dcxlog.txt", "a" );

	if ( logFile ) {
	fwrite( data, sizeof( char ), strlen( data ), logFile );
	fclose( logFile );
	}
	*/
	wsprintf( mIRCLink.m_pData, "//echo -s %s", data );
	SendMessage( mIRCLink.m_mIRCHWND, WM_USER + 200, 0, 0 );
}

/*!
* \brief mIRC $identifier evaluation function
*
* blah
*/

void mIRCeval( const char * data, char * res ) {
	lstrcpy( mIRCLink.m_pData, data );
	SendMessage( mIRCLink.m_mIRCHWND, WM_USER + 201, 0, 0 );
	lstrcpy( res, mIRCLink.m_pData );
}

/*!
* \brief mIRC /command function
*
* blah
*/

void mIRCcom( const char * data ) {
	lstrcpy( mIRCLink.m_pData, data );
	SendMessage( mIRCLink.m_mIRCHWND, WM_USER + 200, 0, 0 );
}

/*!
* \brief Read File Contents
*
* blah
*/

char * readFile( const char * filename ) {
	//Ouvrir le fichier, read en mode binaire
	FILE * file;

	// Open file in binary mode and read
	if ( ( file = fopen( filename, "rb" ) ) == NULL ) return NULL;

	// Seek End of file
	if ( fseek( file, 0, SEEK_END ) ) return NULL;

	// Read pointer location, because pointer is at the end, results into file size
	unsigned long size = ftell( file );

	// Get back to file beginning
	if ( fseek( file, 0, SEEK_SET ) ) return NULL;

	// make data container for file contents
	char * fileContents = new char[ size + 1 ];

	// Null terminate the string
	fileContents[ size ] = 0;

	// read the file, fails, destroy memory and return NULL
	if ( fread( fileContents, 1, size, file ) != size ) {
		delete fileContents;
		return NULL;
	}

	// close file
	fclose( file );
	file = NULL;

	// return memory block containing file data
	return fileContents;
}

/*!
* \brief Windows Theme Setting Function
*
* Used to remove theme on controls
*/

HRESULT SetWindowTheme( HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList ) {
	if ( XPPlus )
		return SetWindowThemeUx( hwnd, L" ", L" " );
	else
		return 0;
}

/*!
* \brief Windows XP function
*
* Returns whether the OS version is XP+ or not
*
* \return > \b true if OS is XP+ \n
*         > \b false otherwise
*/
BOOL isXP() {
	return XPPlus;
}