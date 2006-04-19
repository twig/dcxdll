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


/*!
* \brief Shows CommonDialog for Open/Save
*
* Shows and returns the file selected
*
* \return > TString "" if cancelled
*         > TString Path+Filename
*/
TString FileDialog(TString data, TString method, HWND pWnd) {
	DWORD style = OFN_EXPLORER;
	OPENFILENAME ofn;
	char szFilename[900];

	// seperate the tokenz
	TString styles(data.gettok(1, "	"));
	TString file(data.gettok(2, "	"));
	TString filter(data.gettok(3, "	"));

	styles.trim();
	file.trim();
	filter.trim();

	// format the filter into the format WinAPI wants, with double NULL TERMINATOR at end
	if (filter != "")
		filter = "All Files (*.*)|*.*";

	filter += '\0';
	filter.replace('|', '\0');

	// set up the OFN struct
	ZeroMemory(&ofn, sizeof(ofn));
	wsprintf(szFilename, "%s", file.to_chr());

	ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
	ofn.hwndOwner = pWnd;
	ofn.lpstrFilter = filter.to_chr();
	ofn.lpstrFile = szFilename;
	ofn.nMaxFile = 900;
	ofn.lpstrDefExt = "";

	for (int i = 1; i <= styles.numtok(" "); i++) {
		if (styles.gettok(i, " ") == "multisel")
			style |= OFN_ALLOWMULTISELECT;
		else if (styles.gettok(i, " ") == "createprompt")
			style |= OFN_CREATEPROMPT;
		// FIXME: explorer style resizable on default, cant get rid of that shit
		else if (styles.gettok(i, " ") == "enablesizing")
			style |= OFN_ENABLESIZING;
		else if (styles.gettok(i, " ") == "filemustexist")
			style |= OFN_FILEMUSTEXIST; // (open)
		else if (styles.gettok(i, " ") == "showhidden")
			style |= OFN_FORCESHOWHIDDEN; // 2k/xp
		else if (styles.gettok(i, " ") == "noreadonly")
			style |= OFN_HIDEREADONLY;
		else if (styles.gettok(i, " ") == "nochangedir")
			style |= OFN_NOCHANGEDIR; // (save)
		else if (styles.gettok(i, " ") == "getshortcuts")
			style |= OFN_NODEREFERENCELINKS;
		else if (styles.gettok(i, " ") == "nonetwork")
			style |= OFN_NONETWORKBUTTON;
		else if (styles.gettok(i, " ") == "novalidate")
			style |= OFN_NOVALIDATE;
		else if (styles.gettok(i, " ") == "norecent")
			style |= OFN_DONTADDTORECENT; // 2k/xp
		else if (styles.gettok(i, " ") == "overwriteprompt")
			style |= OFN_OVERWRITEPROMPT; // save
		else if (styles.gettok(i, " ") == "pathmustexist")
			style |= OFN_PATHMUSTEXIST;
		else if (styles.gettok(i, " ") == "owner")
			ofn.hwndOwner = FindOwner(styles, pWnd);
	}

	ofn.Flags = style;

	if (method == "OPEN" && GetOpenFileName(&ofn)) {
		TString str("");

		// if there are multiple files selected
		if (style & OFN_ALLOWMULTISELECT) {
			char *p = szFilename; 

			// process the file name at p since its null terminated
			while (*p != '\0') { 
				if (str != "")
					str += "|";

				str += p;
				p += strlen(p)+1;
			} 
		}
		else	// copy the string directly
			str = szFilename;

		return str;
	}
	else if (method == "SAVE" && GetSaveFileName(&ofn)) {
		return TString(szFilename);
	}

	return TString("");
}

/*!
* \brief Finds an owner of a dialog, used with styles
*
* Returns the owner HWND
*
*/
HWND FindOwner(TString data, HWND defaultWnd) {
	int i = data.findtok("owner", 1, " ");

	// 'owner' token not found in data
	if (!i)
		return defaultWnd;

	// if there is a token after 'owner'
	if (i < data.numtok(" ")) {
		// if it is a number (HWND) passed
		HWND wnd = (HWND) atoi(data.gettok(i +1, " ").to_chr());

		if (wnd)
			return wnd;

		// try to retrieve dialog hwnd from name
		char com[100];
		char res[10];

		wsprintf(com, "$dialog(%s).hwnd", data.gettok(i +1, " ").to_chr());
		mIRCeval(com, res);
		wnd = (HWND) atoi(res);

		if (wnd)
			return wnd;
	}

	return defaultWnd;
}




/*!
* \brief Copies string to the clipboard
*
* Returns TRUE if successful
*
* http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/dataexchange/clipboard/usingtheclipboard.asp
*/
BOOL CopyToClipboard(HWND owner, TString str) {
	if (!OpenClipboard(owner)) {
		mIRCError("CopyToClipboard: couldnt open clipboard");
		return FALSE;
	}

	int cbsize = (strlen(str.to_chr()) +1) * sizeof(TCHAR);
	EmptyClipboard();
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, cbsize);

	if (hglbCopy == NULL) {
		CloseClipboard();
		mIRCError("CopyToClipboard: couldnt open global memory");
		return FALSE;
	}

	char *strCopy = (char *) GlobalLock(hglbCopy);

	// original code, limited and doesnt copy large chunks
	//wsprintf(strCopy, "%s", str.to_chr());

	// demo code from msdn, copies everything
	memcpy(strCopy, str.to_chr(), cbsize);
	strCopy[cbsize] = (TCHAR) 0;    // null character

	GlobalUnlock(hglbCopy);
	SetClipboardData(CF_TEXT, hglbCopy);
	CloseClipboard();

	return TRUE;
}