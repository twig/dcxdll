/*!
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/userex/functions/openthemedata.asp
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
PFNISTHEMEACTIVE IsThemeActiveUx = NULL; //!< blah
PFNOPENTHEMEDATA OpenThemeDataUx = NULL;
PFNCLOSETHEMEDATA CloseThemeDataUx = NULL;
PFNDRAWTHEMEBACKGROUND DrawThemeBackgroundUx = NULL;
PFNGETTHEMEBACKGROUNDCONTENTRECT GetThemeBackgroundContentRectUx = NULL;
PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT IsThemeBackgroundPartiallyTransparentUx = NULL;
PFNDRAWTHEMEPARENTBACKGROUND DrawThemeParentBackgroundUx = NULL;
PFNDRAWTHEMETEXT DrawThemeTextUx = NULL;

HMODULE UXModule = NULL;             //!< UxTheme.dll Module Handle
BOOL XPPlus = FALSE;                 //!< Is OS WinXP+ ?

//FILE * logFile;

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

// UltraDock stuff
// mIRC components HWND
extern HWND treeb_hwnd, sb_hwnd, tb_hwnd, mdi_hwnd, lb_hwnd;
// switchbar position
extern int swb_pos;
// indicate if MDI is maxed out
extern BOOL MDIismax;
extern VectorOfDocks v_docks;
bool dcxSignal;

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
void WINAPI LoadDll(LOADINFO * load) {
	int cnt = 0;

	// If mIRC V6.2+ then try & create our own unique mapfile.
	// damn mIRC reports as 6.2 instead of 6.20!
	// meaning mirc v6.17 appears to be a higher version.
	if ((HIWORD(load->mVersion) == 2) && (LOWORD(load->mVersion) == 6)) {
		TString map_name;
		cnt = 1;
		mIRCLink.m_hFileMap = NULL;

		while ((mIRCLink.m_hFileMap == NULL) && (cnt < 256)) {
			// create mapfile name.
			map_name.sprintf("mIRC%d",cnt);
			// create mapfile.
			mIRCLink.m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,0,PAGE_READWRITE,0,4096,map_name.to_chr());

			// if create failed, fall back on old method.
			if ((mIRCLink.m_hFileMap == NULL) || (mIRCLink.m_hFileMap == INVALID_HANDLE_VALUE)) {
				cnt = 0;
				break;
			}

			// if mapfile already exists then close & try another name.
			if (GetLastError() == ERROR_ALREADY_EXISTS) {
				CloseHandle(mIRCLink.m_hFileMap);
				mIRCLink.m_hFileMap = NULL;
			}
			else
				break;

			cnt++;
		}

		if (cnt == 256)
			cnt = 0;
	}

	mIRCLink.m_map_cnt = cnt; // set mapfile counter for SendMessage()'s

	// use old method for < mirc 6.2 or when new method fails.
	if (cnt == 0) {
		mIRCLink.m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, 4096, "mIRC");
	}

	mIRCLink.m_pData = (LPSTR) MapViewOfFile(mIRCLink.m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	mIRCLink.m_mIRCHWND = load->mHwnd;

	// Initializing OLE Support
	//CoInitialize( NULL );

	// Initializing OLE Support
	OleInitialize(NULL);

	//get IClassFactory* for WebBrowser
	CoGetClassObject(CLSID_WebBrowser, CLSCTX_INPROC_SERVER, 0, IID_IClassFactory, (void**) &g_pClassFactory);
	//6BF52A52-394A-11D3-B153-00C04F79FAA6

	// RichEdit DLL Loading
	LoadLibrary("RICHED20.DLL");

	// UXModule Loading
	UXModule = LoadLibrary("UXTHEME.DLL");

	if (UXModule) {
		SetWindowThemeUx = (PFNSETTHEME) GetProcAddress(UXModule, "SetWindowTheme");
		IsThemeActiveUx = (PFNISTHEMEACTIVE) GetProcAddress(UXModule, "IsThemeActive");
		OpenThemeDataUx = (PFNOPENTHEMEDATA) GetProcAddress(UXModule, "OpenThemeData");
		CloseThemeDataUx = (PFNCLOSETHEMEDATA) GetProcAddress(UXModule, "CloseThemeData");
		DrawThemeBackgroundUx = (PFNDRAWTHEMEBACKGROUND) GetProcAddress(UXModule, "DrawThemeBackground");
		GetThemeBackgroundContentRectUx = (PFNGETTHEMEBACKGROUNDCONTENTRECT) GetProcAddress(UXModule, "GetThemeBackgroundContentRect");
		IsThemeBackgroundPartiallyTransparentUx = (PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT) GetProcAddress(UXModule, "IsThemeBackgroundPartiallyTransparent");
		DrawThemeParentBackgroundUx = (PFNDRAWTHEMEPARENTBACKGROUND) GetProcAddress(UXModule, "DrawThemeParentBackground");
		DrawThemeTextUx = (PFNDRAWTHEMETEXT) GetProcAddress(UXModule, "DrawThemeText");

		if (SetWindowThemeUx && IsThemeActiveUx && OpenThemeDataUx && CloseThemeDataUx &&
			DrawThemeBackgroundUx && GetThemeBackgroundContentRectUx && IsThemeBackgroundPartiallyTransparentUx &&
			DrawThemeParentBackgroundUx && DrawThemeTextUx)
			XPPlus = TRUE;
		else {
			FreeLibrary(UXModule);
			UXModule = NULL;
			XPPlus = FALSE;
			mIRCError("DCX: There was a problem loading IsThemedXP");
		}
	}
	else
		XPPlus = FALSE;
	// Load Control definitions
	INITCOMMONCONTROLSEX icex;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_TREEVIEW_CLASSES | ICC_BAR_CLASSES | ICC_PROGRESS_CLASS | ICC_LISTVIEW_CLASSES | 
		ICC_USEREX_CLASSES | ICC_COOL_CLASSES | ICC_STANDARD_CLASSES | ICC_UPDOWN_CLASS | ICC_DATE_CLASSES | 
		ICC_TAB_CLASSES | ICC_INTERNET_CLASSES | ICC_PAGESCROLLER_CLASS;
	InitCommonControlsEx( &icex ); 

	WNDCLASSEX wc;
	ZeroMemory((void*)&wc , sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);

	// Custom ProgressBar
	GetClassInfoEx(NULL, PROGRESS_CLASS, &wc);
	wc.lpszClassName = DCX_PROGRESSBARCLASS;
	RegisterClassEx(&wc);

	// Custom TreeView
	GetClassInfoEx(NULL, WC_TREEVIEW, &wc);
	wc.lpszClassName = DCX_TREEVIEWCLASS;
	RegisterClassEx(&wc);

	// Custom Toolbar
	GetClassInfoEx(NULL, TOOLBARCLASSNAME, &wc);
	wc.lpszClassName = DCX_TOOLBARCLASS;
	RegisterClassEx(&wc);

	// Custom StatusBar
	GetClassInfoEx(NULL, STATUSCLASSNAME, &wc);
	wc.lpszClassName = DCX_STATUSBARCLASS;
	RegisterClassEx(&wc);

	// Custom ListView
	GetClassInfoEx(NULL, WC_LISTVIEW, &wc);
	wc.lpszClassName = DCX_LISTVIEWCLASS;
	RegisterClassEx(&wc);

	// Custom ComboEx
	GetClassInfoEx(NULL, WC_COMBOBOXEX, &wc);
	wc.lpszClassName = DCX_COMBOEXCLASS;
	RegisterClassEx(&wc);

	// Custom TrackBar
	GetClassInfoEx(NULL, TRACKBAR_CLASS, &wc);
	wc.lpszClassName = DCX_TRACKBARCLASS;
	RegisterClassEx(&wc);

	// Custom RichEdit
	GetClassInfoEx(NULL, "RichEdit20A", &wc);
	wc.lpszClassName = DCX_RICHEDITCLASS;
	RegisterClassEx(&wc);

	// Custom RebarCtrl
	GetClassInfoEx(NULL, REBARCLASSNAME, &wc);
	wc.lpszClassName = DCX_REBARCTRLCLASS;
	RegisterClassEx(&wc);

	// Custom Color Combo
	GetClassInfoEx(NULL, "COMBOBOX", &wc);
	wc.lpszClassName = DCX_COLORCOMBOCLASS;
	RegisterClassEx(&wc);

	// Custom TabCtrl
	GetClassInfoEx(NULL, WC_TABCONTROL, &wc);
	wc.lpszClassName = DCX_TABCTRLCLASS;
	RegisterClassEx(&wc);

	// Custom UpDown
	GetClassInfoEx(NULL, UPDOWN_CLASS, &wc);
	wc.lpszClassName = DCX_UPDOWNCLASS;
	RegisterClassEx(&wc);

	// Custom IppAddress
	GetClassInfoEx(NULL, WC_IPADDRESS, &wc);
	wc.lpszClassName = DCX_IPADDRESSCLASS;
	RegisterClassEx(&wc);

	// Init Divider Control
	//InitDivider( GetModuleHandle( NULL ) );

	// Custom Divider
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = 0;
	wc.lpfnWndProc    = DividerWndProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;
	wc.hInstance      = GetModuleHandle(NULL);
	wc.hIcon          = NULL;
	wc.hCursor        = NULL;
	wc.hbrBackground  = (HBRUSH) (COLOR_3DFACE +1);
	wc.lpszMenuName   = 0;
	wc.lpszClassName  = DCX_DIVIDERCLASS;
	wc.hIconSm        = NULL;
	RegisterClassEx(&wc);

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
	GetClassInfoEx(NULL, "#32770", &wc); // NB: using this class causes tooltips in toolbar children to not show
	wc.lpszClassName = DCX_PANELCLASS;
	RegisterClassEx(&wc);

	// Custom Box
	GetClassInfoEx(NULL, "#32770", &wc); // NB: using this class causes tooltips in toolbar children to not show
	wc.lpszClassName = DCX_BOXCLASS;
	RegisterClassEx(&wc);

	// Custom Button
	GetClassInfoEx(NULL, "BUTTON", &wc);
	wc.lpszClassName = DCX_BUTTONCLASS;
	RegisterClassEx(&wc);

	// Custom Calendar
	GetClassInfoEx(NULL, MONTHCAL_CLASS, &wc);
	wc.lpszClassName = DCX_CALENDARCLASS;
	RegisterClassEx(&wc);

	// Custom Pager
	GetClassInfoEx(NULL, WC_PAGESCROLLER, &wc);
	wc.lpszClassName = DCX_PAGERCLASS;
	RegisterClassEx(&wc);


	/***** XPopup Stuff *****/
	g_OldmIRCWindowProc = (WNDPROC) SetWindowLong(mIRCLink.m_mIRCHWND, GWL_WNDPROC, (LONG) mIRCSubClassWinProc);

	WNDCLASS wcpop;
	ZeroMemory(&wcpop, sizeof(WNDCLASS));
	wcpop.hInstance = GetModuleHandle(NULL);
	wcpop.lpszClassName = XPOPUPMENUCLASS;
	wcpop.lpfnWndProc = XPopupMenu::XPopupWinProc;
	RegisterClass(&wcpop);

	mhMenuOwner = CreateWindow(XPOPUPMENUCLASS, NULL, 0, 0, 0, 0, 0, 0, 0, GetModuleHandle(NULL), 0);

	g_mIRCPopupMenu = new XPopupMenu(NULL);
	g_mIRCMenuBar = new XPopupMenu(GetMenu(mIRCLink.m_mIRCHWND));

	InitUltraDock();
	dcxSignal = false;
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
int WINAPI UnloadDll(int timeout) {
	// DLL unloaded because mIRC exits or /dll -u used
	if (timeout == 0) {
		//mIRCError("Unloading DCX DLL");
		CloseUltraDock(); // UnDock All.

		Dialogs.closeDialogs();

		UnregisterClass(DCX_PROGRESSBARCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_TREEVIEWCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_TOOLBARCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_STATUSBARCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_LISTVIEWCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_COMBOEXCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_TRACKBARCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_RICHEDITCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_REBARCTRLCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_COLORCOMBOCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_UPDOWNCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_BUTTONCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_DIVIDERCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_PANELCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_TABCTRLCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_CALENDARCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_BOXCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_PAGERCLASS, GetModuleHandle(NULL));

		// Class Factory of Web Control
		if (g_pClassFactory != NULL)
			g_pClassFactory->Release();

		// Terminating OLE Support
		OleUninitialize();



		/***** XPopup Stuff *****/
		SetWindowLong(mIRCLink.m_mIRCHWND, GWL_WNDPROC, (LONG) g_OldmIRCWindowProc);

		g_XPopupMenuManager.clearMenus();

		delete g_mIRCPopupMenu;
		g_mIRCMenuBar->cleanMenu(GetMenu(mIRCLink.m_mIRCHWND));
		delete g_mIRCMenuBar;

		if (mhMenuOwner != NULL)
			DestroyWindow(mhMenuOwner);

		UnregisterClass(XPOPUPMENUCLASS, GetModuleHandle(NULL));

		UnmapViewOfFile(mIRCLink.m_pData);
		CloseHandle(mIRCLink.m_hFileMap);

		return 1;
	}
	// keep DLL in memory
	else
		return 0;
}

/*!
* \brief DCX DLL Version Function
*/
mIRC(Version) {
	wsprintf(data,
		"DCX (XPopup) DLL %d.%d.%d %s by ClickHeRe, twig*, and Ook ©2006 - http://dcx.scriptsdb.org",
		DLL_VERSION, DLL_SUBVERSION, DLL_BUILD, DLL_STATE);
	return 3;
}

/*!
* \brief Check if windows is themed
*/
mIRC(IsThemedXP) {
	wsprintf(data, "%s", (dcxIsThemeActive() ? "$true" : "$false"));
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

	TString com;
	char res[40];

	// check if the alias exists
	com.sprintf("$isalias(%s)", d.gettok(2, " ").to_chr());
	mIRCeval(com.to_chr(), res);

	if (lstrcmp(res, "$false") == 0) {
		wsprintf(data, "D_ERROR Mark : No such alias : %s", d.gettok(2, " ").to_chr());
		mIRCError(data);
		return 3;
	}

	// check if valid dialog
	HWND mHwnd = GetHwndFromString(d.gettok(1, " "));

	if (IsWindow(mHwnd) == FALSE) {
		wsprintf(data, "D_ERROR Mark : Invalid Dialog Window : %s", d.gettok(1, " ").to_chr());
		mIRCError(data);
		return 3;
	}

	if (Dialogs.getDialogByHandle(mHwnd) != NULL) {
		wsprintf(data, "D_ERROR Mark: Window Already Marked : %s", d.gettok(1, " ").to_chr());
		mIRCError(data);
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

	if (d.numtok(" ") < 1) {
		TString error;
		error.sprintf("D_ERROR GetSysColor: Invalid arguments");
		mIRCError(error.to_chr());
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
		ret("D_ERROR GetSystemColor: Invalid parameter specified");

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
	if (d.numtok("\t") != 3) {
		mIRCError("D_ERROR OpenDialog: Invalid parameters");
		ret("");
	}

	ret(FileDialog(d, "OPEN", mWnd).to_chr());
}

// SaveDialog (styles) [TAB] (file) [TAB] (filter)
mIRC(SaveDialog) {
	TString d(data);
	d.trim();

	// count number of tab tokens
	if (d.numtok("\t") != 3) {
		mIRCError("D_ERROR SaveDialog: Invalid parameters");
		ret("");
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
	DWORD style = CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST | CF_LIMITSIZE;
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

	for (int i = 1; i <= input.numtok("\t"); i++) {
		TString option(input.gettok(i, "\t"));
		int numtok = 0;

		option.trim();
		numtok = option.numtok(" ");

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
			TString flag(option.gettok(2, " "));
			int c = flag.len();
			int i = 0;

			while (i < c) {
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

	// check that at least some fonts are showing
	if (!(style & CF_PRINTERFONTS) && !(style & CF_SCREENFONTS))
		style |= CF_SCREENFONTS;

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

	if (d.numtok(" ") < 3) {
		mIRCError("/xdid invalid arguments");
		data[0] = 0;
		return 3;
	}

	DcxDialog * p_Dialog = Dialogs.getDialogByName(d.gettok(1, " "));

	if (p_Dialog == NULL) {
		TString error;
		error.sprintf("/xdid unknown dialog \"%s\": see Mark command", d.gettok(1, " ").to_chr());
		mIRCError(error.to_chr());
		data[0] = 0;
		return 3;
	}

	TString IDs = d.gettok(2, " "), d2;
	DcxControl * p_Control;
	int i = 1, n = IDs.numtok(",");

	// Multiple IDs
	if (n > 1) {
		while (i <= n) {
			p_Control = p_Dialog->getControlByID((UINT) atoi(IDs.gettok(i, ",").to_chr()) + mIRC_ID_OFFSET);

			if (p_Control == NULL) {
				TString error;
				error.sprintf("/xdid invalid ID : %s (dialog : %s)", IDs.gettok(i, ",").to_chr(), d.gettok(1, " ").to_chr());
				mIRCError(error.to_chr());
				data[0] = 0;
				return 3;
			}

			d2 = d.gettok(1, " ") + " " + IDs.gettok(i, ",") + " " + d.gettok(3, -1, " ");

			p_Control->parseCommandRequest(d2);
			i++;
		}
	}
	//Single ID
	else {
		p_Control = p_Dialog->getControlByID((UINT) atoi(d.gettok(2, " ").to_chr()) + mIRC_ID_OFFSET);

		if (p_Control == NULL) {
			TString error;
			error.sprintf("/xdid invalid ID : %s (dialog : %s)", d.gettok(2, " ").to_chr(), d.gettok(1, " ").to_chr());
			mIRCError(error.to_chr());
			data[0] = 0;
			return 3;
		}

		p_Control->parseCommandRequest(d);
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
mIRC(_xdid) {
	TString d(data);
	d.trim();

	data[0] = 0;

	if (d.numtok(" ") < 3) {
		mIRCError("$ $+ xdid invalid arguments");
		return 3;
	}

	DcxDialog * p_Dialog = Dialogs.getDialogByName(d.gettok(1, " "));

	if (p_Dialog == NULL) {
		TString error;
		error.sprintf("$ $+ xdid unknown dialog \"%s\": see Mark command", d.gettok(1, " ").to_chr());
		mIRCError(error.to_chr());
		return 3;
	}

	DcxControl * p_Control = p_Dialog->getControlByID((UINT) atoi(d.gettok(2, " ").to_chr()) + mIRC_ID_OFFSET);

	if (p_Control == NULL) {
		TString error;
		error.sprintf("$ $+ xdid invalid ID : %s (dialog %s)", d.gettok(2, " ").to_chr(), d.gettok(1, " ").to_chr());
		mIRCError(error.to_chr());
		return 3;
	}

	p_Control->parseInfoRequest(d, data);
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
		TString val;
		RECT rc;

		GetWindowRect(hTaskbar, &rc);
		val.sprintf("%d %d %d %d", rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
		ret(val.to_chr());
	}

	ret("D_ERROR GetTaskbarPos: could not find taskbar");
}


/*!
* \brief DCX DLL /xdialog Function
*
* mIRC /xdialog -switch dialog (options) interface
*
* Argument \b data contains -> [NAME] [SWITCH] (OPTIONS)
*/
mIRC(xdialog) {
	TString d(data);
	d.trim();

	data[0] = 0;

	if (d.numtok(" ") < 2) {
		TString error;
		error.sprintf("/xdialog invalid arguments ( dialog %s)", d.gettok(1, " ").to_chr());
		mIRCError(error.to_chr());
		return 3;
	}

	DcxDialog * p_Dialog = Dialogs.getDialogByName(d.gettok(1, " "));

	if (p_Dialog == NULL) {
		TString error;
		error.sprintf("/xdialog unknown dialog \"%s\": see Mark command", d.gettok(1, " ").to_chr());
		mIRCError(error.to_chr());
		return 3;
	}

	p_Dialog->parseCommandRequest(d);
	return 3;
}

/*!
* \brief DCX DLL $xdialog Function
*
* mIRC $xdialog(dialog, options).prop interface
*
* Argument \b data contains -> [NAME] [SWITCH] (OPTIONS)
*/
mIRC(_xdialog) {
	TString d(data);
	d.trim();

	// reset mIRC data
	data[0] = 0;

	if (d.numtok(" ") < 2) {
		TString error;
		error.sprintf("$ $+ xdialog invalid arguments ( dialog %s)", d.gettok(1, " ").to_chr());
		mIRCError(error.to_chr());
		return 3;
	}

	DcxDialog *p_Dialog = Dialogs.getDialogByName(d.gettok(1, " "));

	if (p_Dialog == NULL) {
		if (d.gettok(2, " ") != "ismarked") {
			TString error;
			error.sprintf("$ $+ xdialog unknown dialog \"%s\": see Mark command", d.gettok(1, " ").to_chr());
			mIRCError(error.to_chr());
			return 3;
		}
		else
			ret("$false");
	}

	p_Dialog->parseInfoRequest(d, data);
	return 3;
}

/***** XPopup Stuff *****/
/*!
* \brief blah
*
* blah
*/
LRESULT CALLBACK mIRCSubClassWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
    case WM_SIZE:
      {
				mIRCSignalDCX("size mIRC %d %d %d", mHwnd, LOWORD(lParam), HIWORD(lParam));
      }
      break;

		case WM_PARENTNOTIFY:
			{
				if (LOWORD(wParam) == WM_DESTROY)
					UltraUnDock((HWND)lParam);
			}
			break;

		//case WM_CONTEXTMENU:
		case WM_INITMENUPOPUP:
		{
			if (HIWORD(lParam) == FALSE) {
				// let mIRC populate the menus dynamically
				LRESULT lRes = CallWindowProc(g_OldmIRCWindowProc, mHwnd, uMsg, wParam, lParam);

				if (isMenuBarMenu(GetMenu(mHwnd), (HMENU) wParam)) {
					isMenuBar = TRUE;

					if (bIsActiveMircMenubarPopup == TRUE)
						g_mIRCMenuBar->convertMenu((HMENU) wParam, TRUE);
				}
				else {
					isMenuBar = FALSE;

					if (bIsActiveMircPopup == TRUE)
						g_mIRCPopupMenu->convertMenu((HMENU) wParam, FALSE);
				}

				isSysMenu = FALSE;
				return lRes;
			}
			else
				isSysMenu = TRUE;

			break;
		}

		case WM_EXITMENULOOP:
		{
			if ((isMenuBar == FALSE) && (bIsActiveMircPopup == TRUE))
				g_mIRCPopupMenu->clearAllMenuItems();

			//return 0L;
			break;
		}

		case WM_UNINITMENUPOPUP:
		{
			if ((isMenuBar == TRUE) && (isSysMenu == FALSE) && (bIsActiveMircMenubarPopup == TRUE))
				g_mIRCMenuBar->deleteAllItemData((HMENU) wParam);

			//return 0L;
			break;
		}

		case WM_MEASUREITEM:
		{
			LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;

			if (lpmis->CtlType == ODT_MENU) {
				XPopupMenuItem *p_Item = (XPopupMenuItem*) lpmis->itemData;

				if (p_Item != NULL) {
					SIZE size = p_Item->getItemSize(mHwnd);

					lpmis->itemWidth = size.cx;
					lpmis->itemHeight = size.cy;
					return TRUE; 
				}
			}

			break;
		}

		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;

			if (lpdis->CtlType == ODT_MENU) {
				XPopupMenuItem *p_Item = (XPopupMenuItem*) lpdis->itemData;

				if (p_Item != NULL) {
					p_Item->DrawItem(lpdis);
					return TRUE; 
				}
			}

			break;
		}

		case WM_CLOSE:
		{
			SetWindowLong(mIRCLink.m_mIRCHWND, GWL_WNDPROC, (LONG) g_OldmIRCWindowProc);
			PostMessage(mHwnd, uMsg, 0, 0);
			return 0L;
			break;
		}

		default:
			break;
	}

	return CallWindowProc(g_OldmIRCWindowProc, mHwnd, uMsg, wParam, lParam);
}

DcxDialogCollection dcxDialogs() {
	return Dialogs;
}

BOOL isMenuBarMenu(HMENU hMenu, HMENU hMatch) {
	HMENU hTemp;
	int i = 0, n = GetMenuItemCount(hMenu);

	while (i < n) {
		if ((hTemp = GetSubMenu(hMenu, i)) != NULL) {
			if (hTemp == hMatch)
				return TRUE;

			if (isMenuBarMenu(hTemp, hMatch) == TRUE)
				return TRUE;
		}

		++i;
	}

	return FALSE;
}

/*
 * /dcx MsgBox [STYLES] [TAB] [TITLE] [TAB] [MSG]
 * http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/windowing/dialogboxes/dialogboxreference/dialogboxfunctions/messagebox.asp
 */
mIRC(MsgBox) {
	TString d(data);
	d.trim();

	if (d.numtok("\t") < 3) {
		ret("D_ERROR MessageBox: invalid parameters");
	}

	DWORD   style     = MB_DEFBUTTON1;
	TString strStyles = d.gettok(1, "\t");
	TString strTitle  = d.gettok(2, "\t");
	TString strMsg    = d.gettok(3, -1, "\t");
	int     n         = strStyles.numtok(" ");
	HWND    owner     = aWnd;

	strTitle.trim();
	strMsg.trim();
	strStyles.trim();

	for (int i = 1; i <= n; i++) {
//		MB_ABORTRETRYIGNORE
//		MB_CANCELTRYCONTINUE && isXP()

		if (strStyles.gettok(i, " ") == "ok")
			style |= MB_OK;
		else if (strStyles.gettok(i, " ") == "okcancel")
			style |= MB_OKCANCEL;
		else if (strStyles.gettok(i, " ") == "retrycancel")
			style |= MB_RETRYCANCEL;
		else if (strStyles.gettok(i, " ") == "yesno")
			style |= MB_YESNO;
		else if (strStyles.gettok(i, " ") == "yesnocancel")
			style |= MB_YESNOCANCEL;
		else if (strStyles.gettok(i, " ") == "exclamation")
			style |= MB_ICONEXCLAMATION;
		else if (strStyles.gettok(i, " ") == "warning")
			style |= MB_ICONWARNING;
		else if (strStyles.gettok(i, " ") == "information")
			style |= MB_ICONINFORMATION;
		else if (strStyles.gettok(i, " ") == "asterisk")
			style |= MB_ICONASTERISK;
		else if (strStyles.gettok(i, " ") == "question")
			style |= MB_ICONQUESTION;
		else if (strStyles.gettok(i, " ") == "stop")
			style |= MB_ICONSTOP;
		else if (strStyles.gettok(i, " ") == "error")
			style |= MB_ICONERROR;
		else if (strStyles.gettok(i, " ") == "hand")
			style |= MB_ICONHAND;
		//else if (strStyles.gettok(i, " ") == "help")
		//	style |= MB_HELP;
		else if (strStyles.gettok(i, " ") == "defbutton2")
			style |= MB_DEFBUTTON2;
		else if (strStyles.gettok(i, " ") == "defbutton3")
			style |= MB_DEFBUTTON3;
		else if (strStyles.gettok(i, " ") == "defbutton4")
			style |= MB_DEFBUTTON4;
		else if (strStyles.gettok(i, " ") == "modal")
			style |= MB_APPLMODAL;
		else if (strStyles.gettok(i, " ") == "sysmodal")
			style |= MB_SYSTEMMODAL;
		else if (strStyles.gettok(i, " ") == "taskmodal")
			style |= MB_TASKMODAL;
		else if (strStyles.gettok(i, " ") == "right")
			style |= MB_RIGHT;
		else if (strStyles.gettok(i, " ") == "rtl")
			style |= MB_RTLREADING;
		else if (strStyles.gettok(i, " ") == "foreground")
			style |= MB_SETFOREGROUND;
		else if (strStyles.gettok(i, " ") == "topmost")
			style |= MB_TOPMOST;
		else if (strStyles.gettok(i, " ") == "owner")
			owner = FindOwner(strStyles, mWnd);
	}

	// if task modal, send in null to block app
	if (style & MB_TASKMODAL) {
		owner = NULL;
	}

	switch (MessageBox(owner, strMsg.to_chr(), strTitle.to_chr(), style)) {
		case IDABORT:
			ret("abort");
			break;
		case IDCANCEL:
			ret("cancel");
			break;
		case IDCONTINUE:
			ret("continue");
			break;
		case IDIGNORE:
			ret("ignore");
			break;
		case IDNO:
			ret("no");
			break;
		case IDOK:
			ret("ok");
			break;
		case IDRETRY:
			ret("retry");
			break;
		case IDTRYAGAIN:
			ret("tryagain");
			break;
		case IDYES:
			ret("yes");
			break;
		default:
			ret("");
			break;
	}
}

/*!
* \brief XPopup DLL /xpop Function
*
* mIRC /xpop -switch menu path [TAB] optional data
*
* Argument \b data contains -> [MENU] [SWITCH] [PATH] [TAB] [OPTION]
*/
mIRC(xpop) {
	TString d(data);
	d.trim();

	if (d.numtok(" ") < 3) {
		mIRCError("/ $+ xpop invalid arguments");
		data[0] = 0;
		return 3;
	}

	if ((d.gettok(1, " ") == "mirc") || (d.gettok(1, " ") == "mircbar")) {
		mIRCError("/ $+ xpop invalid menu name : mirc or mircbar menus don't have access to this feature.");
		data[0] = 0;
		return 3;
	}

	XPopupMenu *p_Menu = g_XPopupMenuManager.getMenuByName(d.gettok(1, " "));

	if (p_Menu == NULL) {
		TString error;
		error.sprintf("/ $+ xpop unknown menu \"%s\": see /xpopup -c command", d.gettok(1, " ").to_chr());
		mIRCError(error.to_chr());
		data[0] = 0;
		return 3;
	}

	p_Menu->parseXPopCommand(d);
	return 3;
}

/*!
* \brief XPopup DLL $xpop Function
*
* mIRC $xpop(menu, path, options).prop interface
*
* Argument \b data contains -> [MENU] [PROP] [PATH] [TAB] [OPTION]
*/
mIRC(_xpop) {
	TString d(data);
	d.trim();

	if (d.numtok(" ") < 3) {
		mIRCError("$ $+ xpop invalid arguments");
		data[0] = 0;
		return 3;
	}

	if ((d.gettok(1, " ") == "mirc") || (d.gettok(1, " ") == "mircbar")) {
		mIRCError("$ $+ xpop invalid menu name : mirc or mircbar menus don't have access to this feature.");
		data[0] = 0;
		return 3;
	}

	XPopupMenu *p_Menu = g_XPopupMenuManager.getMenuByName(d.gettok(1, " "));

	if (p_Menu == NULL) {
		TString error;
		error.sprintf("$ $+ xpop unknown menu \"%s\": see /xpopup -c command", d.gettok(1, " ").to_chr());
		mIRCError(error.to_chr());
		data[0] = 0;
		return 3;
	}

	p_Menu->parseXPopIdentifier(d, data);
	return 3;
}

/*!
* \brief XPopup DLL /xpopup Function
*
* mIRC /xpopup -switch menu (options)
*
* Argument \b data contains -> [MENU] [SWITCH] (OPTIONS)
*/
mIRC(xpopup) {
	TString d(data);
	d.trim();

	if (d.numtok(" ") < 2) {
		mIRCError("/ $+ xpopup invalid arguments");
		data[0] = 0;
		return 3;
	}

	g_XPopupMenuManager.parseXPopupCommand(d);
	return 3;
}

/*!
* \brief XPopup DLL $xpopup Function
*
* mIRC $xpopup(menu, options).prop interface
*
* Argument \b data contains -> [MENU] [PROP] (OPTIONS)
*/
mIRC(_xpopup) {
	TString d(data);
	d.trim();

	if (d.numtok(" ") < 2) {
		mIRCError("$ $+ xpopup invalid arguments");
		data[0] = 0;
		return 3;
	}

	g_XPopupMenuManager.parseXPopupIdentifier(d, data);
	return 3;
}

/*!
* \brief XPopup DLL /mpopup Function
*
* mIRC /mpopup menuname [0|1]
*
* Argument \b data contains -> [MENU] [0|1]
*/
mIRC(mpopup) {
	TString d(data);
	d.trim();

	if (d.numtok(" ") < 2) {
		mIRCError("/ $+ mpopup invalid arguments");
		data[0] = 0;
		return 3;
	}

	if (d.gettok(1, " ") == "mirc") {
		if (d.gettok(2, " ") == "1")
			bIsActiveMircPopup = TRUE;
		else
			bIsActiveMircPopup = FALSE;
	}
	else if (d.gettok(1, " ") == "mircbar") {
		if (d.gettok(2, " ") == "1")
			bIsActiveMircMenubarPopup = TRUE;
		else {
			bIsActiveMircMenubarPopup = FALSE;
			g_mIRCMenuBar->cleanMenu(GetMenu(mIRCLink.m_mIRCHWND));
		}
	}

	data[0] = 0;
	return 3;
}
// <1|0>
mIRC(xSignal) {
	TString d(data);
	d.trim();
	if ((BOOL)d.to_num())
		dcxSignal = true;
	else
		dcxSignal = false;
	data[0] = 0;
	return 1;
}
// <hwnd>
mIRC(xNoTheme) {
	TString d(data);
	d.trim();
	if (!XPPlus)
		mIRCError("This function is for XP+ ONLY");
	HWND hwnd = (HWND)d.to_num();
	if (IsWindow(hwnd)) {
		if (dcxSetWindowTheme(hwnd,L" ",L" ") != S_OK) {
			mIRCError("Unable to set theme");
		}
		return 1;
	}
	mIRCError("Invalid HWND");
	return 1;
}