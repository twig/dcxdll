/*!
* \file dll.cpp
* \brief Main DLL File
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
#include "classes/dcxdialogcollection.h"
#include "classes/dcxdialog.h"
#include "classes/dcxcontrol.h"

#include "classes/custom/divider.h"

#include "classes/xpopup/xpopupmenumanager.h"
#include "classes/xpopup/xpopupmenu.h"

#include <exdisp.h>
#include <mshtml.h>
#include <exdispid.h>

// DCX Stuff
DcxDialogCollection Dialogs; //!< blah

mIRCDLL mIRCLink; //!< blah

PFNSETTHEME SetWindowThemeUx = NULL; //!< blah
HMODULE UXModule = NULL;             //!< UxTheme.dll Module Handle
BOOL XPPlus = FALSE;                 //!< Is OS WinXP+ ?

FILE * logFile;

//GdiplusStartupInput gdiplusStartupInput; // GDI+ Startup Input Param
//ULONG_PTR           gdiplusToken;        // GDI+ Token Param

IClassFactory * g_pClassFactory; //!< Web Control Factory



// XPopup Stuff
XPopupMenuManager g_XPopupMenuManager; //!< Global XPopupMenu Manager

XPopupMenu * g_mIRCPopupMenu;
XPopupMenu * g_mIRCMenuBar;

BOOL isMenuBar;
BOOL isSysMenu;

BOOL bIsActiveMircPopup = FALSE;
BOOL bIsActiveMircMenubarPopup = FALSE;

HWND mhMenuOwner; //!< Menu Owner Window Which Processes WM_ Menu Messages

WNDPROC g_OldmIRCWindowProc;

/*!
* \brief mIRC DLL Load Function
*
* This function is called when the DLL is loaded.
*
* It initializes all what the DLL needs and links mIRC received information to the mIRCDLL \b mIRCLink
* data structure to be used later in various functions in the DLL.
*
* \param load mIRC Load Structure Pointer
*/

void WINAPI LoadDll( LOADINFO * load ) {
	mIRCLink.m_hFileMap = CreateFileMapping( INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, 4096, "mIRC" );     
	mIRCLink.m_pData = (LPSTR) MapViewOfFile( mIRCLink.m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0 );
	mIRCLink.m_mIRCHWND = load->mHwnd;

	// Initializing OLE Support
	//CoInitialize( NULL );

	// Initializing OLE Support
	OleInitialize( NULL );

	//get IClassFactory* for WebBrowser
	CoGetClassObject( CLSID_WebBrowser, CLSCTX_INPROC_SERVER, 0, IID_IClassFactory, (void**) &g_pClassFactory );
	//6BF52A52-394A-11D3-B153-00C04F79FAA6

	// RichEdit DLL Loading
	LoadLibrary( "RICHED20.DLL" );

	// UXModule Loading
	UXModule = LoadLibrary( "UXTHEME.DLL" );

	if( UXModule ) {
		SetWindowThemeUx = (PFNSETTHEME) GetProcAddress( UXModule, "SetWindowTheme" );

		if ( SetWindowThemeUx )
			XPPlus = TRUE;
		else {
			FreeLibrary( UXModule );
			UXModule = NULL;
			XPPlus = FALSE;
		}
	}

	// Initialize GDI+.
	//GdiplusStartup( &gdiplusToken, &gdiplusStartupInput, NULL );

	// Load Control definitions
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof( INITCOMMONCONTROLSEX );
	icex.dwICC  = ICC_TREEVIEW_CLASSES | ICC_BAR_CLASSES | ICC_PROGRESS_CLASS | ICC_LISTVIEW_CLASSES | 
		ICC_USEREX_CLASSES | ICC_COOL_CLASSES | ICC_STANDARD_CLASSES | ICC_UPDOWN_CLASS | ICC_DATE_CLASSES | 
		ICC_TAB_CLASSES | ICC_INTERNET_CLASSES;
	InitCommonControlsEx( &icex ); 

	WNDCLASSEX wc;
	ZeroMemory( (void*)&wc , sizeof(WNDCLASSEX) );
	wc.cbSize = sizeof( WNDCLASSEX );

	// Custom ProgressBar
	GetClassInfoEx( NULL, PROGRESS_CLASS, &wc );
	wc.lpszClassName = DCX_PROGRESSBARCLASS;
	RegisterClassEx( &wc );

	// Custom TreeView
	GetClassInfoEx( NULL, WC_TREEVIEW, &wc );
	wc.lpszClassName = DCX_TREEVIEWCLASS;
	RegisterClassEx( &wc );

	// Custom Toolbar
	GetClassInfoEx( NULL, TOOLBARCLASSNAME, &wc );
	wc.lpszClassName = DCX_TOOLBARCLASS;
	RegisterClassEx( &wc );

	// Custom StatusBar
	GetClassInfoEx( NULL, STATUSCLASSNAME, &wc );
	wc.lpszClassName = DCX_STATUSBARCLASS;
	RegisterClassEx( &wc );

	// Custom ListView
	GetClassInfoEx( NULL, WC_LISTVIEW, &wc );
	wc.lpszClassName = DCX_LISTVIEWCLASS;
	RegisterClassEx( &wc );

	// Custom ComboEx
	GetClassInfoEx( NULL, WC_COMBOBOXEX, &wc );
	wc.lpszClassName = DCX_COMBOEXCLASS;
	RegisterClassEx( &wc );

	// Custom TrackBar
	GetClassInfoEx( NULL, TRACKBAR_CLASS, &wc );
	wc.lpszClassName = DCX_TRACKBARCLASS;
	RegisterClassEx( &wc );

	// Custom RichEdit
	GetClassInfoEx( NULL, "RichEdit20A", &wc );
	wc.lpszClassName = DCX_RICHEDITCLASS;
	RegisterClassEx( &wc );

	// Custom RebarCtrl
	GetClassInfoEx( NULL, REBARCLASSNAME, &wc );
	wc.lpszClassName = DCX_REBARCTRLCLASS;
	RegisterClassEx( &wc );

	// Custom Color Combo
	GetClassInfoEx( NULL, "COMBOBOX", &wc );
	wc.lpszClassName = DCX_COLORCOMBOCLASS;
	RegisterClassEx( &wc );

	// Custom TabCtrl
	GetClassInfoEx( NULL, WC_TABCONTROL, &wc );
	wc.lpszClassName = DCX_TABCTRLCLASS;
	RegisterClassEx(&wc);

	// Custom UpDown
	GetClassInfoEx( NULL, UPDOWN_CLASS, &wc );
	wc.lpszClassName = DCX_UPDOWNCLASS;
	RegisterClassEx( &wc );

	// Custom IppAddress
	GetClassInfoEx( NULL, WC_IPADDRESS, &wc );
	wc.lpszClassName = DCX_IPADDRESSCLASS;
	RegisterClassEx( &wc );

	// Init Divider Control
	//InitDivider( GetModuleHandle( NULL ) );

	// Custom Divider
	wc.cbSize		= sizeof( WNDCLASSEX );
	wc.style			= 0;
	wc.lpfnWndProc	= DividerWndProc;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	= GetModuleHandle( NULL );
	wc.hIcon			= NULL;
	wc.hCursor		= NULL;
	wc.hbrBackground	= (HBRUSH)(COLOR_3DFACE+1);
	wc.lpszMenuName	= 0;
	wc.lpszClassName	= DCX_DIVIDERCLASS;
	wc.hIconSm			= NULL;
	RegisterClassEx( &wc );

	//GetClassInfoEx( NULL, DIVIDERCLASSNAME, &wc );
	//wc.lpszClassName = ;
	//RegisterClassEx( &wc );

	/*
	// Custom Panel
	wc.cbSize			    = sizeof( WNDCLASSEX );
	wc.style		  	  = 0;
	wc.lpfnWndProc	  = DefWindowProc;
	wc.cbClsExtra		  = 0;
	wc.cbWndExtra		  = 0;
	wc.hInstance		  = GetModuleHandle( NULL );
	wc.hIcon			    = LoadCursor( GetModuleHandle( NULL ), IDC_ARROW );
	wc.hCursor			  = NULL;
	wc.hbrBackground	= NULL; //(HBRUSH)(COLOR_3DFACE+1);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= DCX_PANELCLASS;
	wc.hIconSm			  = NULL;
	RegisterClassEx( &wc );
	*/

	// Custom Panel
	GetClassInfoEx( NULL, "#32770", &wc );
	wc.lpszClassName = DCX_PANELCLASS;
	RegisterClassEx( &wc );

	// Custom Box
	GetClassInfoEx( NULL, "#32770", &wc );
	wc.lpszClassName = DCX_BOXCLASS;
	RegisterClassEx( &wc );

	// Custom Button
	GetClassInfoEx( NULL, "BUTTON", &wc );
	wc.lpszClassName = DCX_BUTTONCLASS;
	RegisterClassEx( &wc );

	// Custom Calendar
	GetClassInfoEx( NULL, MONTHCAL_CLASS, &wc );
	wc.lpszClassName = DCX_CALENDARCLASS;
	RegisterClassEx( &wc );



	/***** XPopup Stuff *****/
	g_OldmIRCWindowProc = (WNDPROC) SetWindowLong( mIRCLink.m_mIRCHWND, GWL_WNDPROC, (LONG) mIRCSubClassWinProc );

	WNDCLASS wcpop;
	ZeroMemory( &wcpop, sizeof( WNDCLASS ) );
	wcpop.hInstance = GetModuleHandle( NULL );
	wcpop.lpszClassName = XPOPUPMENUCLASS;
	wcpop.lpfnWndProc = XPopupMenu::XPopupWinProc;
	RegisterClass( &wcpop );

	mhMenuOwner = CreateWindow( XPOPUPMENUCLASS, NULL, 0, 0, 0, 0, 0, 0, 0, GetModuleHandle( NULL ), 0 );

	g_mIRCPopupMenu = new XPopupMenu( NULL );
	g_mIRCMenuBar = new XPopupMenu( GetMenu( mIRCLink.m_mIRCHWND ) );
}

/*!
* \brief mIRC DLL UnLoad Function
*
* This function is called when the DLL is unloaded.
*
* It uninitializes all the stuff DCX set up and frees up memory fragments.
*
* \param timeout Unload trigger indicator (0 = timeout unload after 10 min - 1 = exit or /dll -u)
*/

int WINAPI UnloadDll( int timeout ) {
	// DLL unloaded because mIRC exits or /dll -u used
	if ( timeout == 0 ) {
		//mIRCError( "Unloading DCX DLL" );
		Dialogs.closeDialogs( );

		UnregisterClass( DCX_PROGRESSBARCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_TREEVIEWCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_TOOLBARCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_STATUSBARCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_LISTVIEWCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_COMBOEXCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_TRACKBARCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_RICHEDITCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_REBARCTRLCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_COLORCOMBOCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_UPDOWNCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_BUTTONCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_DIVIDERCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_PANELCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_TABCTRLCLASS, GetModuleHandle(NULL) );
		UnregisterClass( DCX_CALENDARCLASS, GetModuleHandle(NULL) );

		UnregisterClass( DCX_BOXCLASS, GetModuleHandle(NULL) );

		// Class Factory of Web Control
		if ( g_pClassFactory != NULL )
			g_pClassFactory->Release( );

		// Terminating OLE Support
		OleUninitialize( );

		// GDI+ Shutdown
		//GdiplusShutdown( gdiplusToken );



		/***** XPopup Stuff *****/
		SetWindowLong( mIRCLink.m_mIRCHWND, GWL_WNDPROC, (LONG) g_OldmIRCWindowProc );

		g_XPopupMenuManager.clearMenus( );

		delete g_mIRCPopupMenu;
		g_mIRCMenuBar->cleanMenu( GetMenu( mIRCLink.m_mIRCHWND ) );
		delete g_mIRCMenuBar;

		if ( mhMenuOwner != NULL )
			DestroyWindow( mhMenuOwner );

		UnregisterClass( XPOPUPMENUCLASS, GetModuleHandle( NULL ) );

		UnmapViewOfFile( mIRCLink.m_pData );
		CloseHandle( mIRCLink.m_hFileMap );

		return 1;
	}
	// keep DLL in memory
	else 
		return 0;
}

/*!
* \brief DCX DLL Version Function
*/

mIRC( Version ) {
	wsprintf(data,
		"DCX (XPopup) DLL %d.%d.%d %s by ClickHeRe and twig* ©2006 - http://dcx.scriptsdb.org",
		DLL_VERSION, DLL_SUBVERSION, DLL_BUILD, DLL_STATE);
	return 3;
}

/*!
* \brief DCX DLL Mark Function
*
* Argument \b data contains -> [NAME] [ALIAS]
*/

// Mark [NAME] [ALIAS]
mIRC(Mark) {
	TString d(data);
	d.trim();

	if (d.numtok(" ") < 2)
		ret("D_ERROR Mark: [NAME] [ALIAS]");

	char com[100];
	char res[20];
	wsprintf(com, "$dialog(%s).hwnd", d.gettok(1, " ").to_chr());
	mIRCeval(com, res );

	HWND mHwnd = (HWND) atoi(res);

	if (IsWindow(mHwnd) == FALSE) {
		wsprintf(data, "D_ERROR Mark : Invalid Dialog Window : %s", d.gettok(1, " ").to_chr());
		return 3;
	}

	if (Dialogs.getDialogByHandle(mHwnd) != NULL) {
		wsprintf(data, "D_ERROR Mark: Window Already Marked : %s", d.gettok(1, " ").to_chr());
		return 3;
	}

	Dialogs.markDialog(mHwnd, d.gettok(1, " "), d.gettok(2, " "));
	ret("D_OK Mark: Dialog Marked");
}

/*!
* \brief DCX DLL GetSystemColor Function
*
* Argument \b data contains -> [ATTRIB] (not implemented yet, [DEFAULT])
*/

// GetSystemColor [ATTRIBUTE]
mIRC(GetSystemColor) {
	TString d(data);
	d.trim();

	//mIRCSignal( d.to_chr( ) );

	if (d.numtok(" ") < 1) {
		char error[900];
		wsprintf(error, "dcx(GetSysColor): invalid arguments");
		mIRCError(error);
		return 0;
	}

	int col;
	TString coltype = d.gettok(1, " ");

	if      (coltype == "COLOR_3DDKSHADOW"		) { col = COLOR_3DDKSHADOW; }
	else if (coltype == "COLOR_3DFACE"			) { col = COLOR_3DFACE; }
	else if (coltype == "COLOR_3DHIGHLIGHT"	) { col = COLOR_3DHIGHLIGHT; }
	else if (coltype == "COLOR_3DHILIGHT"		) { col = COLOR_3DHILIGHT; }
	else if (coltype == "COLOR_3DLIGHT"			) { col = COLOR_3DLIGHT; }
	else if (coltype == "COLOR_3DSHADOW"		) { col = COLOR_3DSHADOW; }
	else if (coltype == "COLOR_ACTIVEBORDER"	) { col = COLOR_ACTIVEBORDER; }
	else if (coltype == "COLOR_ACTIVECAPTION"	) { col = COLOR_ACTIVECAPTION; }
	else if (coltype == "COLOR_APPWORKSPACE"	) { col = COLOR_APPWORKSPACE; }
	else if (coltype == "COLOR_BACKGROUND"		) { col = COLOR_BACKGROUND; }
	else if (coltype == "COLOR_BTNFACE"			) { col = COLOR_BTNFACE; }
	else if (coltype == "COLOR_BTNHIGHLIGHT"	) { col = COLOR_BTNHIGHLIGHT; }
	else if (coltype == "COLOR_BTNSHADOW"		) { col = COLOR_BTNSHADOW; }
	else if (coltype == "COLOR_BTNTEXT"			) { col = COLOR_BTNTEXT; }
	else if (coltype == "COLOR_CAPTIONTEXT"	) { col = COLOR_CAPTIONTEXT; }
	else if (coltype == "COLOR_DESKTOP"			) { col = COLOR_DESKTOP; }
	else if (coltype == "COLOR_GRADIENTACTIVECAPTION") { col = COLOR_GRADIENTACTIVECAPTION; }
	else if (coltype == "COLOR_GRADIENTINACTIVECAPTION") { col = COLOR_GRADIENTINACTIVECAPTION; }
	else if (coltype == "COLOR_GRAYTEXT"		) { col = COLOR_GRAYTEXT; }
	else if (coltype == "COLOR_HIGHLIGHT"		) { col = COLOR_HIGHLIGHT; }
	else if (coltype == "COLOR_HIGHLIGHTTEXT"	) { col = COLOR_HIGHLIGHTTEXT; }
	else if (coltype == "COLOR_HOTLIGHT"		) { col = COLOR_HOTLIGHT; }
	else if (coltype == "COLOR_INACTIVEBORDER") { col = COLOR_INACTIVEBORDER; }
	else if (coltype == "COLOR_INACTIVECAPTION") { col = COLOR_INACTIVECAPTION; }
	else if (coltype == "COLOR_INACTIVECAPTIONTEXT") { col = COLOR_INACTIVECAPTIONTEXT; }
	else if (coltype == "COLOR_INFOBK"			) { col = COLOR_INFOBK; }
	else if (coltype == "COLOR_INFOTEXT"		) { col = COLOR_INFOTEXT; }
	else if (coltype == "COLOR_MENU"				) { col = COLOR_MENU; }
	else if (coltype == "COLOR_MENUHILIGHT"	) { col = COLOR_MENUHILIGHT; }
	else if (coltype == "COLOR_MENUBAR"			) { col = COLOR_MENUBAR; }
	else if (coltype == "COLOR_MENUTEXT"		) { col = COLOR_MENUTEXT; }
	else if (coltype == "COLOR_SCROLLBAR"		) { col = COLOR_SCROLLBAR; }
	else if (coltype == "COLOR_WINDOW"			) { col = COLOR_WINDOW; }
	else if (coltype == "COLOR_WINDOWFRAME"	) { col = COLOR_WINDOWFRAME; }
	else if (coltype == "COLOR_WINDOWTEXT"		) { col = COLOR_WINDOWTEXT; }
	else
		ret("$dcx(GetSystemColor): Invalid parameter specified");

	// max of 8 digits, 9 for null terminator
	char val[9];
	wsprintf(val, "%d", GetSysColor(col));
	ret(val);
}


/*!
* \brief DCX DLL ColorDialog Function
*
* Argument \b data contains -> (DEFAULT) [STYLES]
*/

// ColorDialog (DEFAULT) [STYLES]
mIRC(ColorDialog) {
	TString d(data);
	d.trim();

	CHOOSECOLOR	cc;
	static COLORREF clr[16];
	COLORREF		sel = atoi(d.gettok(1, " ").to_chr());
	DWORD			styles = CC_RGBINIT;
	ZeroMemory(&cc, sizeof(CHOOSECOLOR));

	// initial settings
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = mWnd;

	if (d.numtok(" ") > 1) {
		for (int i = 1; i <= d.numtok(" "); i++) {
			if (d.gettok(i, " ") == "anycolor")
				styles |= CC_ANYCOLOR;
			else if (d.gettok(i, " ") == "fullopen")
				styles |= CC_FULLOPEN;
			else if (d.gettok(i, " ") == "nofullopen")
				styles |= CC_PREVENTFULLOPEN;
			else if (d.gettok(i, " ") == "solidonly")
				styles |= CC_SOLIDCOLOR;
			else if (d.gettok(i, " ") == "owner")
				cc.hwndOwner = FindOwner(d, mWnd);
		}
	}

	cc.rgbResult = (COLORREF) sel;
	cc.Flags = styles;
	cc.lpCustColors = clr;

	if (ChooseColor(&cc)) {
		wsprintf(data, "%d", cc.rgbResult);
		ret(data);
	}
	else
		ret("-1");
}


/*!
* \brief DCX DLL OpenDialog|SaveDialog Function
*
* Argument \b data contains -> (styles) [TAB] (file) [TAB] (filter)
*
* http://www.winprog.org/tutorial/app_two.html
* http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/userinput/commondialogboxlibrary/commondialogboxreference/commondialogboxstructures/openfilename.asp
*/

// OpenDialog (styles) [TAB] (file) [TAB] (filter)
mIRC(OpenDialog) {
	TString d(data);
	d.trim();

	// count number of tab tokens
	if (d.numtok("	") != 3) {
		ret("$dcx(OpenDialog): Invalid parameters");
	}

	ret(FileDialog(d, "OPEN", mWnd).to_chr());
}

// SaveDialog (styles) [TAB] (file) [TAB] (filter)
mIRC(SaveDialog) {
	TString d(data);
	d.trim();

	// count number of tab tokens
	if (d.numtok("	") != 3) {
		ret("$dcx(SaveDialog): Invalid parameters");
	}

	ret(FileDialog(d, "SAVE", mWnd).to_chr());
}


/*!
* \brief Shows CommonDialog for Selecting Fonts
*
* Shows and returns the file selected
*
* \return > TString "" if cancelled
*         > TString +flags charset size fontname
*/
mIRC(FontDialog) {
	DWORD style = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_FORCEFONTEXIST | CF_LIMITSIZE;
	CHOOSEFONT cf;
	LOGFONT lf;

	// seperate the tokens (by tabs)
	TString input(data);
	input.trim();

	// set up the LF structure
	ZeroMemory(&lf, sizeof(LOGFONT));
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);

	// set up the CF struct
	ZeroMemory(&cf, sizeof(CHOOSEFONT));
	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = mWnd;
	cf.lpLogFont = &lf;
	cf.nSizeMin = 8;
	cf.nSizeMax = 72;

	for (int i = 1; i <= input.numtok("	"); i++) {
		TString option(input.gettok(i, "	"));
		int numtok = 0;

		option.trim();
		numtok = option.numtok(" ");
		//mIRCDebug("parsing option: %s", option.to_chr());

/*
default +flags(ibsua) charset size fontname
flags +etc
color rgb
minmaxsize min max (Ranges from 8 to 72, if "D" is used 5 to 30)
owner hwnd

+flag to use mirc colors
palette col...16...col

//clear | echo -a $dcx(FontDialog, hello asd $chr(9) flags +abcdef $chr(9) color 255 $chr(9) owner dcxtest_1146984371 $chr(9) default + default 10 Verdana) | /udcx

http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/userinput/commondialogboxlibrary/commondialogboxreference/commondialogboxstructures/choosefont.asp
*/

		// flags +
		if (option.gettok(1, " ") == "flags" && numtok > 1) {
			//style |= OFN_ALLOWMULTISELECT;
			//mIRCDebug("flags: %s (todo)", option.gettok(2, " ").to_chr());

			TString flag(option.gettok(2, " "));
			//mIRCError(flag.to_chr());
			int c = flag.len();
			int i = 0;

			while (i < c) {
				//mIRCDebug("checking flag %c", flag[i]);

				if (flag[i] == 'a')
					style |= CF_NOFACESEL;
				else if (flag[i] == 'b')
					style |= CF_SCRIPTSONLY;
				else if (flag[i] == 'c')
					style |= CF_SCALABLEONLY;// (Scalable fonts include vector fonts, scalable printer fonts, TrueType fonts, and fonts scaled by other technologies.)
				else if (flag[i] == 'e')
					style |= CF_EFFECTS;
				else if (flag[i] == 'f')
					style |= CF_FORCEFONTEXIST;
				else if (flag[i] == 'h')
					style |= CF_NOSCRIPTSEL;
				else if (flag[i] == 'i')
					style |= CF_NOSIMULATIONS;
				else if (flag[i] == 'm')
					style |= CF_SELECTSCRIPT;
				else if (flag[i] == 'n')
					style |= CF_PRINTERFONTS;
				else if (flag[i] == 'p')
					style |= CF_FIXEDPITCHONLY;
				else if (flag[i] == 'r')
					style |= CF_NOVERTFONTS;
				else if (flag[i] == 's')
					style |= CF_SCREENFONTS;
				else if (flag[i] == 't')
					style |= CF_TTONLY;
				else if (flag[i] == 'v')
					style |= CF_NOVECTORFONTS;
				else if (flag[i] == 'w')
					style |= CF_WYSIWYG;
				else if (flag[i] == 'y')
					style |= CF_NOSTYLESEL;
				else if (flag[i] == 'z')
					style |= CF_NOSIZESEL;

				i++;
			}
		}
		// defaults +flags(ibsua) charset size fontname
		else if (option.gettok(1, " ") == "default" && numtok > 4)
			ParseCommandToLogfont(option.gettok(2, -1, " "), &lf);
		// color rgb
		else if (option.gettok(1, " ") == "color" && numtok > 1)
			cf.rgbColors = (COLORREF) atoi(option.gettok(2, " ").to_chr());
		// minmaxsize min max
		else if (option.gettok(1, " ") == "minmaxsize" && numtok > 2) {
			cf.nSizeMin = atoi(option.gettok(2, " ").to_chr());
			cf.nSizeMax = atoi(option.gettok(3, " ").to_chr());
		}
		// owner
		else if (option.gettok(1, " ") == "owner" && numtok > 1)
			cf.hwndOwner = FindOwner(option, mWnd);
	}

	cf.Flags = style;
	cf.iPointSize = lf.lfHeight * 10;

	// show the dialog
	if (ChooseFont(&cf)) {
		char str[900];
		TString fntflags(ParseLogfontToCommand(&lf));

		// color flags font info
		wsprintf(str, "%d %s", cf.rgbColors, fntflags.to_chr());
		ret(str);
	}
	else
		ret("");
}



/*!
* \brief DCX DLL /xdid Function
*
* mIRC /xdid -switch dialog ID (options) interface
*
* Argument \b data contains -> [NAME] [ID] [SWITCH] (OPTIONS)
*/

mIRC(xdid) {
	TString d(data);
	d.trim();

	//mIRCError( d.to_chr( ) );

	if (d.numtok(" ") < 3) {
		mIRCError("/xdid invalid arguments");
		data[0] = 0;
		return 3;
	}

	//mIRCError( d.gettok( 1, " " ).to_chr( ) );

	DcxDialog * p_Dialog = Dialogs.getDialogByName( d.gettok( 1, " " ) );

	if (p_Dialog == NULL) {
		char error[200];
		wsprintf( error, "/xdid unknown dialog \"%s\": see Mark command", d.gettok( 1, " " ).to_chr( ) );
		mIRCError( error );
		data[0] = 0;
		return 3;
	}

	TString IDs = d.gettok(2, " "), d2;
	DcxControl * p_Control;
	int i = 1, n = IDs.numtok(",");

	// Multiple IDs
	if (n > 1) {
		while (i <= n) {
			p_Control = p_Dialog->getControlByID( (UINT) atoi( IDs.gettok( i, "," ).to_chr( ) ) + mIRC_ID_OFFSET );

			//mIRCError( p_Dialog->getName( ).to_chr( ) );

			if (p_Control == NULL) {
				char error[200];
				wsprintf( error, "/xdid invalid ID : %s (dialog : %s)", IDs.gettok( i, "," ).to_chr( ), d.gettok( 1, " " ).to_chr( ) );
				mIRCError( error );
				data[0] = 0;
				return 3;
			}

			d2 = d.gettok(1, " ") + " " + IDs.gettok(i, ",") + " " + d.gettok(3, -1, " ");

			//mIRCError( d2.to_chr( ) );

			p_Control->parseCommandRequest(d2);
			i++;
		}
	}
	//Single ID
	else {
		p_Control = p_Dialog->getControlByID( (UINT) atoi( d.gettok( 2, " " ).to_chr( ) ) + mIRC_ID_OFFSET );

		if ( p_Control == NULL ) {
			char error[200];
			wsprintf( error, "/xdid invalid ID : %s (dialog : %s)", d.gettok(2, " ").to_chr( ), d.gettok( 1, " " ).to_chr());
			mIRCError( error );
			data[0] = 0;
			return 3;
		}

		p_Control->parseCommandRequest( d );
	}

	return 3;
}

/*!
* \brief DCX DLL $xdid Function
*
* mIRC $xdid(dialog, ID, options).prop interface
*
* Argument \b data contains -> [NAME] [ID] [PROP] (OPTIONS)
*/

mIRC( _xdid ) {
	TString d( data );
	d.trim( );

	data[0] = 0;

	if ( d.numtok( " " ) < 3 ) {
		mIRCError( "$ $+ xdid invalid arguments" );
		return 3;
	}

	DcxDialog * p_Dialog = Dialogs.getDialogByName( d.gettok( 1, " " ) );

	if ( p_Dialog == NULL ) {
		char error[200];
		wsprintf( error, "$ $+ xdid unknown dialog \"%s\": see Mark command", d.gettok( 1, " " ).to_chr( ) );
		mIRCError( error );
		return 3;
	}

	DcxControl * p_Control = p_Dialog->getControlByID( (UINT) atoi( d.gettok( 2, " " ).to_chr( ) ) + mIRC_ID_OFFSET );

	if ( p_Control == NULL ) {
		char error[200];
		wsprintf( error, "$ $+ xdid invalid ID : %s (dialog %s)", d.gettok( 2, " " ).to_chr( ), d.gettok( 1, " " ).to_chr( ) );
		mIRCError( error );
		return 3;
	}

	p_Control->parseInfoRequest( d, data );
	return 3;
}


/*!
* \brief DCX DLL $dcx(GetTaskbarPos) Function
*
* $dcx(GetTaskbarPos) interface
*
*/

mIRC(GetTaskbarPos) {
	HWND hTaskbar = FindWindow("Shell_TrayWnd", NULL);
	
	if (hTaskbar) {
		char *val = new char[100];
		RECT rc;

		GetWindowRect(hTaskbar, &rc);
		wsprintf(val, "%d %d %d %d", rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
		ret(val);
	}

	ret("D_ERROR: GetTaskbarPos");
}


/*!
* \brief DCX DLL /xdialog Function
*
* mIRC /xdialog -switch dialog (options) interface
*
* Argument \b data contains -> [NAME] [SWITCH] (OPTIONS)
*/

mIRC( xdialog ) {
	TString d( data );
	d.trim( );

	data[0] = 0;

	//mIRCSignal( d.to_chr( ) );

	if ( d.numtok( " " ) < 2 ) {
		char error[900];
		wsprintf( error, "/xdialog invalid arguments ( dialog %s)", d.gettok( 1, " " ).to_chr( ) );
		mIRCError( error );
		return 3;
	}

	DcxDialog * p_Dialog = Dialogs.getDialogByName( d.gettok( 1, " " ) );

	if ( p_Dialog == NULL ) {
		char error[200];
		wsprintf( error, "/xdialog unknown dialog \"%s\": see Mark command", d.gettok( 1, " " ).to_chr( ) );
		mIRCError( error );
		return 3;
	}

	p_Dialog->parseCommandRequest( d );
	return 3;
}

/*!
* \brief DCX DLL $xdialog Function
*
* mIRC $xdialog(dialog, options).prop interface
*
* Argument \b data contains -> [NAME] [SWITCH] (OPTIONS)
*/

mIRC( _xdialog ) {
	TString d( data );
	d.trim( );

	// reset mIRC data
	data[0] = 0;

	if ( d.numtok( " " ) < 2 ) {
		char error[900];
		wsprintf( error, "$ $+ xdialog invalid arguments ( dialog %s)", d.gettok( 1, " " ).to_chr( ) );
		mIRCError( error );
		return 3;
	}

	DcxDialog * p_Dialog = Dialogs.getDialogByName( d.gettok( 1, " " ) );

	if ( p_Dialog == NULL ) {
		char error[200];
		wsprintf( error, "$ $+ xdialog unknown dialog \"%s\": see Mark command", d.gettok( 1, " " ).to_chr( ) );
		mIRCError( error );
		return 3;
	}

	p_Dialog->parseInfoRequest( d, data );
	return 3;
}



/***** XPopup Stuff *****/
/*!
* \brief blah
*
* blah
*/

LRESULT CALLBACK mIRCSubClassWinProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	switch ( uMsg ) {
	case WM_INITMENUPOPUP:
		{
			if ( HIWORD( lParam ) == FALSE ) {
				// let mIRC populate the menus dynamically
				LRESULT lRes = CallWindowProc( g_OldmIRCWindowProc, mHwnd, uMsg, wParam, lParam );

				if ( isMenuBarMenu( GetMenu( mHwnd ), (HMENU) wParam ) ) {
					//mIRCError( "Part of the mIRC Menubar" );
					isMenuBar = TRUE;
					if ( bIsActiveMircMenubarPopup == TRUE )
						g_mIRCMenuBar->convertMenu( (HMENU) wParam, TRUE );
				}
				else {
					//mIRCError( "Not Part of the mIRC Menubar" );
					isMenuBar = FALSE;
					if ( bIsActiveMircPopup == TRUE )
						g_mIRCPopupMenu->convertMenu( (HMENU) wParam, FALSE );
				}

				isSysMenu = FALSE;
				return lRes;
			}
			else
				isSysMenu = TRUE;
		}
		break;

	case WM_EXITMENULOOP:
		{
			//mIRCError( "mIRC WM_EXITMENULOOP" );
			if ( isMenuBar == FALSE && bIsActiveMircPopup == TRUE )
				g_mIRCPopupMenu->clearAllMenuItems( );

			//return 0L;
		}
		break;

	case WM_UNINITMENUPOPUP:
		{
			//mIRCError( "mIRC WM_UNINITMENUPOPUP" );
			if ( isMenuBar == TRUE && isSysMenu == FALSE && bIsActiveMircMenubarPopup == TRUE )
				g_mIRCMenuBar->deleteAllItemData( (HMENU) wParam );

			//return 0L;
		}
		break;

	case WM_MEASUREITEM:
		{
			LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;
			if ( lpmis->CtlType == ODT_MENU ) {
				XPopupMenuItem * p_Item = (XPopupMenuItem *) lpmis->itemData;

				if ( p_Item != NULL ) {
					SIZE size = p_Item->getItemSize( mHwnd );
					lpmis->itemWidth = size.cx;
					lpmis->itemHeight = size.cy;
					return TRUE; 
				}
			}
		}
		break;

	case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
			if ( lpdis->CtlType == ODT_MENU ) {
				XPopupMenuItem * p_Item = (XPopupMenuItem *) lpdis->itemData;

				if ( p_Item != NULL ) {
					p_Item->DrawItem( lpdis );
					return TRUE; 
				}
			}
		}
		break;

	case WM_CLOSE:
		{
			SetWindowLong( mIRCLink.m_mIRCHWND, GWL_WNDPROC, (LONG) g_OldmIRCWindowProc );
			PostMessage( mHwnd, uMsg, 0, 0 );
			return 0L;
		}
		break;

	default:
		break;
	}

	return CallWindowProc( g_OldmIRCWindowProc, mHwnd, uMsg, wParam, lParam );
}

BOOL isMenuBarMenu( HMENU hMenu, HMENU hMatch ) {
	HMENU hTemp;
	int i = 0, n = GetMenuItemCount( hMenu );
	while ( i < n ) {
		if ( ( hTemp = GetSubMenu( hMenu, i ) ) != NULL ) {
			if ( hTemp == hMatch )
				return TRUE;

			if ( isMenuBarMenu( hTemp, hMatch ) == TRUE )
				return TRUE;
		}
		++i;
	}
	return FALSE;
}

/*!
* \brief XPopup DLL /xpop Function
*
* mIRC /xpop -switch menu path [TAB] optional data
*
* Argument \b data contains -> [MENU] [SWITCH] [PATH] [TAB] [OPTION]
*/

mIRC( xpop ) {
	TString d( data );
	d.trim( );

	//mIRCError( d.to_chr( ) );

	if ( d.numtok( " " ) < 3 ) {
		mIRCError( "/ $+ xpop invalid arguments" );
		data[0] = 0;
		return 3;
	}

	if ( d.gettok( 1, " " ) == "mirc" || d.gettok( 1, " " ) == "mircbar" ) {
		mIRCError( "/ $+ xpop invalid menu name : mirc or mircbar menus don't have acces to this feature." );
		data[0] = 0;
		return 3;
	}

	XPopupMenu * p_Menu = g_XPopupMenuManager.getMenuByName( d.gettok( 1, " " ) );
	//mIRCError( d.gettok( 1, " " ).to_chr( ) );

	if ( p_Menu == NULL ) {
		char error[200];
		wsprintf( error, "/ $+ xpop unknown menu \"%s\": see /xpopup -c command", d.gettok( 1, " " ).to_chr( ) );
		mIRCError( error );
		data[0] = 0;
		return 3;
	}

	p_Menu->parseXPopCommand( d );
	return 3;
}

/*!
* \brief XPopup DLL $xpop Function
*
* mIRC $xpop(menu, path, options).prop interface
*
* Argument \b data contains -> [MENU] [PROP] [PATH] [TAB] [OPTION]
*/

mIRC( _xpop ) {
	TString d( data );
	d.trim( );

	if ( d.numtok( " " ) < 3 ) {
		mIRCError( "$ $+ xpop invalid arguments" );
		data[0] = 0;
		return 3;
	}

	if ( d.gettok( 1, " " ) == "mirc" || d.gettok( 1, " " ) == "mircbar" ) {
		mIRCError( "$ $+ xpop invalid menu name : mirc or mircbar menus don't have acces to this feature." );
		data[0] = 0;
		return 3;
	}

	XPopupMenu * p_Menu = g_XPopupMenuManager.getMenuByName( d.gettok( 1, " " ) );
	//mIRCError( d.gettok( 1, " " ).to_chr( ) );

	if ( p_Menu == NULL ) {
		char error[200];
		wsprintf( error, "$ $+ xpop unknown menu \"%s\": see /xpopup -c command", d.gettok( 1, " " ).to_chr( ) );
		mIRCError( error );
		data[0] = 0;
		return 3;
	}

	p_Menu->parseXPopIdentifier( d, data );
	return 3;
}

/*!
* \brief XPopup DLL /xpopup Function
*
* mIRC /xpopup -switch menu (options)
*
* Argument \b data contains -> [MENU] [SWITCH] (OPTIONS)
*/

mIRC( xpopup ) {
	TString d( data );
	d.trim( );

	if ( d.numtok( " " ) < 2 ) {
		mIRCError( "/ $+ xpopup invalid arguments" );
		data[0] = 0;
		return 3;
	}

	g_XPopupMenuManager.parseXPopupCommand( d );
	return 3;
}

/*!
* \brief XPopup DLL $xpopup Function
*
* mIRC $xpopup(menu, options).prop interface
*
* Argument \b data contains -> [MENU] [PROP] (OPTIONS)
*/

mIRC( _xpopup ) {
	TString d( data );
	d.trim( );

	if ( d.numtok( " " ) < 2 ) {
		mIRCError( "$ $+ xpopup invalid arguments" );
		data[0] = 0;
		return 3;
	}

	g_XPopupMenuManager.parseXPopupIdentifier( d, data );
	return 3;
}

/*!
* \brief XPopup DLL /mpopup Function
*
* mIRC /mpopup menuname [0|1]
*
* Argument \b data contains -> [MENU] [0|1]
*/

mIRC( mpopup ) {
	TString d( data );
	d.trim( );

	if ( d.numtok( " " ) < 2 ) {
		mIRCError( "/ $+ mpopup invalid arguments" );
		data[0] = 0;
		return 3;
	}

	//BOOL isActiveMircPopup;
	//BOOL isActiveMircMenubarPopup;

	if ( d.gettok( 1, " " ) == "mirc" ) {
		if ( d.gettok( 2, " " ) == "1" )
			bIsActiveMircPopup = TRUE;
		else
			bIsActiveMircPopup = FALSE;
	}
	else if ( d.gettok( 1, " " ) == "mircbar" ) {
		if ( d.gettok( 2, " " ) == "1" )
			bIsActiveMircMenubarPopup = TRUE;
		else {
			bIsActiveMircMenubarPopup = FALSE;
			g_mIRCMenuBar->cleanMenu( GetMenu( mIRCLink.m_mIRCHWND ) );
		}
	}

	data[0] = 0;
	return 3;
}