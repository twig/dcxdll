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
#include "Dcx.h"
#include "mIRCLinker.h"
#include "DcxUXModule.h"
#include "DcxGDIModule.h"
#include "DcxDWMModule.h"
#include "classes/dcxdialogcollection.h"
#include "classes/dcxdialog.h"
#include "classes/dcxcontrol.h"

#include "classes/custom/divider.h"
#include "classes/custom/dcxtrayicon.h"

#include "classes/xpopup/xmenubar.h"

#include "classes/xpopup/xpopupmenumanager.h"
#include "classes/xpopup/xpopupmenu.h"

#include <exdisp.h>
#include <mshtml.h>
#include <exdispid.h>

// Windows 2000+ Pointers
PFNGETWINDOWINFO GetWindowInfoUx = NULL;
PFNANIMATEWINDOW AnimateWindowUx = NULL;
PFNINSENDMESSAGEEX InSendMessageExUx = NULL;
PFNFLASHWINDOWEX FlashWindowExUx = NULL;

// XP+ function pointers
// Others
PFNUPDATELAYEREDWINDOW UpdateLayeredWindowUx = NULL;
PFNSETLAYEREDWINDOWATTRIBUTES SetLayeredWindowAttributesUx = NULL;
PFNDRAWSHADOWTEXT DrawShadowTextUx = NULL;
PFNPICKICONDLG PickIconDlgUx = NULL;
PFNGETFULLPATHNAMEW GetFullPathNameWUx = NULL;

//FILE * logFile;

//IClassFactory * g_pClassFactory = NULL; //!< Web Control Factory


// Tray Icon stuff
DcxTrayIcon *trayIcons = NULL; // tray icon manager


// XMenuBar stuff
HMENU g_OriginalMenuBar = NULL;
XPopupMenu *g_mIRCScriptMenu = NULL; // Wrapper for the mIRC scriptable menu.


// XPopup Stuff

SIGNALSWITCH dcxSignal;
/*
	* DllMain()
	* function added to insure that only a single instance of dcx.dll is loaded.
*/

HANDLE hDcxMutex = NULL;
//extern BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved );

BOOL WINAPI DllMain(
					HINSTANCE hinstDLL,  // handle to DLL module
					DWORD fdwReason,     // reason for calling function
					LPVOID lpReserved )  // reserved
{
	// Perform actions based on the reason for calling.
	switch( fdwReason ) 
	{
	case DLL_PROCESS_ATTACH:
		{
			char mutex[128];
			// Initialize once for each new process.
			// Return FALSE to fail DLL load.
			DisableThreadLibraryCalls(hinstDLL);
			// add pid of mIRC.exe to name so mutex is specific to this instance of mIRC.
			// GetModuleHandle(NULL) was returning a consistant result.
			wsprintf(mutex,"DCX_LOADED%lx", GetCurrentProcessId()); // NB: calls user32.dll, is this ok? See warnings in DllMain() docs.

			// Enforce only one instance of dcx.dll loaded at a time.
			hDcxMutex = CreateMutex(NULL, TRUE, mutex); // Windows 2000:  Do not create a named synchronization object in DllMain because the system will then load an additional DLL. This restriction does not apply to subsequent versions of Windows.
			if (hDcxMutex == NULL) return FALSE;		// TODO: solve this issue or are we going to make the dll XP+ only now?
			else if (GetLastError() == ERROR_ALREADY_EXISTS) {
				//ReleaseMutex(hDcxMutex);
				//CloseHandle(hDcxMutex);
				return FALSE;
			}
		}
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		// should never be called when using DisableThreadLibraryCalls()
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		if (hDcxMutex != NULL) {
			ReleaseMutex(hDcxMutex);
			CloseHandle(hDcxMutex);
			hDcxMutex = NULL;
		}
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
	UNREFERENCED_PARAMETER(lpReserved);
}

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
_INTEL_DLL_ void WINAPI LoadDll(LOADINFO * load) {
	Dcx::load(load);

	DCX_DEBUG(Dcx::debug,"LoadDLL", "Initialising UltraDock...");
	InitUltraDock();
//#ifndef NDEBUG
//	InitCustomDock();
//#endif
}

/*!
* \brief mIRC DLL UnLoad Function
*
* This function is called when the DLL is unloaded.
*
* It uninitializes all the stuff DCX set up and frees up memory fragments.
*
* \param timeout Unload trigger indicator (0 = /dll -u (or on pre mIRC 6.30, exit) - 1 = timeout unload after 10 min - 2 = exit on mIRC 6.30+)
*/
_INTEL_DLL_ int WINAPI UnloadDll(int timeout) {
	// DLL unloaded because mIRC exits or /dll -u used
	if (timeout != 1) {
		Dcx::unload();


		/***** TrayIcon Stuff *****/
		if (trayIcons != NULL) {
			delete trayIcons;
			trayIcons = NULL;
		}

		/***** XMenuBar Stuff *****/
		//g_XMenuBar.resetMenuBar();

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
#ifdef DCX_DEV_BUILD
	wsprintf(data,
		"DCX (XPopup) DLL %d.%d.%d %s%d by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2006-2009",
		DLL_VERSION, DLL_SUBVERSION, DLL_BUILD, DLL_STATE, DLL_DEV_BUILD);
#else
	wsprintf(data,
		"DCX (XPopup) DLL %d.%d.%d %s by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2006-2009",
		DLL_VERSION, DLL_SUBVERSION, DLL_BUILD, DLL_STATE);
#endif
	return 3;
}

/*!
* \brief DCX DLL is DcxDirectShow supported?
*/
static TString dxData;
mIRC(IsUsingDirectX) {
	if (Dcx::isDX9Installed()) {
		lstrcpy(data, dxData.to_chr());
		return 3;
	}
	else if (Dcx::initDirectX(data, 900)) {
		dxData = data;
		return 3;
	}
	ret("$false");
//#ifdef DCX_USE_DXSDK
//	DWORD dx_ver;
//	if (GetDXVersion(&dx_ver, data, 900) == S_OK) {
//		if (dx_ver < 0x00090000)
//			mIRCLink.m_bDX9Installed = false;
//		else
//			mIRCLink.m_bDX9Installed = true;
//		if (dx_ver < 1)
//			ret("$false");
//	}
//	return 3;
//#else
//	ret("$false");
//#endif // DCX_USE_DXSDK
}

/*!
* \brief DCX DLL is GDI+ supported?
*/
mIRC(IsUsingGDI) {
	ret((Dcx::GDIModule.isUseable() ? "$true" : "$false"));
}

/*!
* \brief Check if it's safe to unload DLL
*/
mIRC(IsUnloadSafe) {
	ret((Dcx::isUnloadSave() ? "$true" : "$false"));
}

/*!
* \brief Check if windows is themed
*/
mIRC(IsThemedXP) {
	ret((Dcx::XPPlusModule.dcxIsThemeActive() ? "$true" : "$false"));
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

	if (d.numtok() < 2) 
	{
		Dcx::error("Mark", "[NAME] [ALIAS]");
		ret(Dcx::getLastError());
	}

	return Dcx::mark(data, d.gettok(1), d.gettok(2));
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

	if (d.numtok() < 1) {
		Dcx::error("$!dcx(GetSysColor)","Invalid arguments");
		return 0;
	}

	int col;
	TString coltype(d.gettok(1));

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
	wsprintf(data, "%d", GetSysColor(col));
	return 3;
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

	data[0] = 0;

	if (d.numtok( ) < 3) {
		Dcx::error("/xdid","Invalid arguments");
		return 0;
	}

	DcxDialog * p_Dialog = Dcx::Dialogs.getDialogByName(d.gettok( 1 ));

	if (p_Dialog == NULL) {
		Dcx::errorex("/xdid", "Unknown dialog \"%s\": see Mark command", d.gettok(1).to_chr());
		return 0;
	}

	TString IDs(d.gettok( 2 )), d2;
	DcxControl * p_Control;
	int n = IDs.numtok(TSCOMMA);

	// Multiple IDs id,id,id,id-id,id-id
	if (n > 1) {
		for (int i = 1; i <= n; i++) {
			TString tsID(IDs.gettok(i, TSCOMMA));
			UINT id_start = 0, id_end = 0;
			if (tsID.numtok("-") == 2) {
				id_start = tsID.gettok(1, "-").to_int();
				id_end = tsID.gettok(2, "-").to_int();
			}
			else
				id_start = id_end = tsID.to_int();

			for (UINT id = id_start; id <= id_end; id++) {
				p_Control = p_Dialog->getControlByID(id + mIRC_ID_OFFSET);

				if (p_Control == NULL) {
					p_Dialog->showErrorEx(NULL,d.gettok( 3 ).to_chr(), "(xdid) Invalid ID : %ld (dialog : %s)", id, d.gettok( 1 ).to_chr());
					return 0;
				}

				d2.sprintf("%s %ld %s",d.gettok( 1 ).to_chr(), id, d.gettok(3, -1).to_chr());

				p_Control->parseCommandRequest(d2);
			}
		}
	}
	//Single ID or single id-id
	else {
		UINT id_start = 0, id_end = 0;
		if (IDs.numtok("-") == 2) {
			id_start = IDs.gettok(1, "-").to_int();
			id_end = IDs.gettok(2, "-").to_int();
		}
		else
			id_start = id_end = IDs.to_int();

		for (UINT id = id_start; id <= id_end; id++) {
			p_Control = p_Dialog->getControlByID(id + mIRC_ID_OFFSET);

			if (p_Control == NULL) {
				p_Dialog->showErrorEx(NULL,d.gettok( 3 ).to_chr(), "(xdid) Invalid ID : %ld (dialog : %s)", id, d.gettok( 1 ).to_chr());
				return 0;
			}

			p_Control->parseCommandRequest(d);
		}
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

	if (d.numtok( ) < 3) {
		Dcx::error("$!xdid()","Invalid arguments");
		return 0;
	}

	DcxDialog * p_Dialog = Dcx::Dialogs.getDialogByName(d.gettok( 1 ));

	if (p_Dialog == NULL) {
		Dcx::errorex("$!xdid()", "Unknown dialog \"%s\": see Mark command", d.gettok(1).to_chr());
		return 0;
	}

	DcxControl * p_Control = p_Dialog->getControlByID((UINT) d.gettok( 2 ).to_int() + mIRC_ID_OFFSET);

	if (p_Control == NULL) {
		p_Dialog->showErrorEx(d.gettok( 3 ).to_chr(), NULL, "Invalid ID : %s (dialog %s)", d.gettok( 2 ).to_chr(), d.gettok( 1 ).to_chr());
		return 0;
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
		RECT rc;

		GetWindowRect(hTaskbar, &rc);
		wsprintf(data, "%d %d %d %d", rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
		return 3;
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

	if (d.numtok( ) < 2) {
		Dcx::errorex("/xdialog", "Invalid arguments ( dialog %s)", d.gettok(1).to_chr());
		return 0;
	}

	DcxDialog * p_Dialog = Dcx::Dialogs.getDialogByName(d.gettok( 1 ));

	if (p_Dialog == NULL) {
		Dcx::errorex("/xdialog", "Unknown dialog \"%s\": see Mark command", d.gettok(1).to_chr());
		return 0;
	}

	p_Dialog->parseCommandRequest(d);
	return 1;
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

	if (d.numtok( ) < 2) {
		Dcx::errorex("$!xdialog()", "Invalid arguments ( dialog %s)", d.gettok(1).to_chr());
		return 0;
	}

	DcxDialog *p_Dialog = Dcx::Dialogs.getDialogByName(d.gettok( 1 ));

	if (p_Dialog == NULL) {
		if (d.gettok( 2 ) != "ismarked") {
			Dcx::errorex("$!xdialog()", "Unknown dialog \"%s\": see Mark command", d.gettok(1).to_chr());
			return 0;
		}
		else
			ret("$false");
	}

	p_Dialog->parseInfoRequest(d, data);
	return 3;
}

/***** XPopup Stuff 

*!
* \brief XPopup DLL /xpop Function
*
* mIRC /xpop -switch menu path [TAB] optional data
*
* Argument \b data contains -> [MENU] [SWITCH] [PATH] [TAB] [OPTION]
*/
mIRC(xpop) {
	TString d(data);
	d.trim();

	data[0] = 0;

	if (d.numtok( ) < 3) {
		Dcx::error("/xpop","Invalid arguments");
		return 0;
	}

	if ((d.gettok(1) == "mirc") || (d.gettok(1) == "mircbar")) {
		Dcx::error("/xpop","Invalid menu name : mirc or mircbar menus don't have access to this feature.");
		return 0;
	}

	XPopupMenu *p_Menu = Dcx::XPopups.getMenuByName(d.gettok(1), FALSE);

	if (p_Menu == NULL) {
		Dcx::errorex("/xpop", "Unknown menu \"%s\": see /xpopup -c command", d.gettok(1).to_chr());
		return 0;
	}

	p_Menu->parseXPopCommand(d);
	return 1;
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

	data[0] = 0;

	if (d.numtok( ) < 3) {
		Dcx::error("$!xpop()","Invalid arguments");
		return 0;
	}

	if ((d.gettok( 1 ) == "mirc") || (d.gettok( 1 ) == "mircbar")) {
		Dcx::error("$!xpop()","Invalid menu name : mirc or mircbar menus don't have access to this feature.");
		return 0;
	}

	XPopupMenu *p_Menu = Dcx::XPopups.getMenuByName(d.gettok(1), FALSE);

	if (p_Menu == NULL) {
		Dcx::errorex("$!xpop()", "Unknown menu \"%s\": see /xpopup -c command", d.gettok(1).to_chr());
		return 0;
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

	data[0] = 0;

	if (d.numtok( ) < 2) {
		Dcx::error("/xpopup","Invalid arguments");
		return 0;
	}

	Dcx::XPopups.parseCommand(d);
	return 1;
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

	data[0] = 0;

	if (d.numtok( ) < 2) {
		Dcx::error("$!xpopup()","Invalid arguments");
		return 0;
	}

	Dcx::XPopups.parseIdentifier(d, data);
	return 3;
}

/*!
* \brief XMenuBar DLL /xmenubar Function
*
* mIRC /xmenubar -switch (options)
*/
mIRC(xmenubar) {
	TString d(data);

	data[0] = 0;
	Dcx::XMenubar.parseXMenuBarCommand(d.trim());

	return 1;
}

/*!
* \brief XMenuBar DLL $xmenubar Function
*
* mIRC $xmenubar(options).prop interface
*/
mIRC(_xmenubar) {
	TString d(data);

	data[0] = 0;
	Dcx::XMenubar.parseXMenuBarInfo(d.trim(), data);

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

	data[0] = 0;

	return Dcx::XPopups.parseMPopup(d.trim());
}
// /dcx xSignal [1|0] (+FLAGS)
mIRC(xSignal) {
	TString d(data);
	TString flags;
	bool state;

	d.trim();

	// flags specified
	if (d.numtok() > 1)
		flags = d.gettok(2);
	// if no flags specified, set all states
	else
		flags = "+dst";

	// determine state
	if (d.to_num() > 0)
		state = true;
	else
		state = false;

	// start from 1 because we expect it to be the '+' symbol
	for (int i = 1; i < (int) flags.len(); i++) {
		switch (flags[i]){
			case 'd':
				dcxSignal.xdock = state;
				break;

			case 's':
				dcxSignal.xstatusbar = state;
				break;

			case 't':
				dcxSignal.xtray = state;
				break;

			default:
				Dcx::errorex("/dcx xSignal", "Unknown flag '%c' specified.", flags[i]);
		}
	}

	data[0] = 0;
	return 1;
}

// /dcx WindowProps [HWND] [+FLAGS] (ARGS)
mIRC(WindowProps) {
	TString input(data);
	int numtok = input.numtok( );

	if (numtok < 2) {
		Dcx::error("/dcx WindowProps", "Insuffient parameters");
		return 0;
	}

	HWND hwnd = (HWND) input.gettok(1).to_int();

	if (!IsWindow(hwnd)) {
		Dcx::error("/dcx WindowProps", "Invalid window");
		return 0;
	}

	TString flags(input.gettok( 2 ).trim());

	if ((flags[0] != '+') || (flags.len() < 2)) {
		Dcx::error("/dcx WindowProps","No Flags Found");
		return 0;
	}

	if ((flags.find('T', 0) == 0) && (flags.find('i', 0) == 0) && (flags.find('t', 0) == 0) && (flags.find('r', 0) == 0)) {
		Dcx::error("/dcx WindowProps","Unknown Flags");
		return 0;
	}

	// set hwnd NoTheme
	// +T
	if (flags.find('T', 0)) {
		if (Dcx::XPPlusModule.isUseable()) {
			if (Dcx::XPPlusModule.dcxSetWindowTheme(hwnd,L" ",L" ") != S_OK)
				Dcx::error("/dcx WindowProps", "Unable to set theme");
		}
	}
	// set hwnd's title icon
	// +i [INDEX] [FILENAME]
	if (flags.find('i', 0)) {
		if (numtok < 3) {
			// invalid args
			Dcx::error("/dcx WindowProps", "Invalid Args");
			return 0;
		}
		int index = input.gettok( 3 ).to_int();
		TString filename(input.gettok(1,TSTAB).gettok(4, -1).trim());

		if (!ChangeHwndIcon(hwnd,flags,index,filename))
			return 0;
	}
	// set hwnd title text
	// +t [TEXT]
	if (flags.find('t', 0)) { 
		TString txt;
		
		if (flags.find('i', 0)) {
			if (input.numtok(TSTAB) > 1)
				txt = input.gettok(2,-1,TSTAB);
		}
		else if (numtok > 2) {
			txt = input.gettok(3, -1);
		}

		SetWindowText(hwnd, txt.trim().to_chr());
	}
	// RMB click hwnd at pos.
	// +r [X] [Y]
	if (flags.find('r', 0)) {
		UINT x = (UINT)input.gettok( 3 ).to_num();
		UINT y = (UINT)input.gettok( 4 ).to_num();
		LPARAM parm = MAKELONG(x,y);
		SendMessage(hwnd,WM_RBUTTONDOWN,MK_RBUTTON,parm);
		PostMessage(hwnd,WM_RBUTTONUP,MK_RBUTTON,parm); // MUST be a PostMessage or the dll hangs untill the menu is closed.
	}

	return 1;
}

// $dcx(ActiveWindow, property)
mIRC(ActiveWindow) {
	if (GetWindowInfoUx == NULL) {
		Dcx::error("$!dcx(ActiveWindow)", "Function Unsupported By Current OS");
		return 0;
	}

	TString input(data);
	input.trim();

	data[0] = 0;

	int numtok = input.numtok();

	if (numtok < 1) {
		Dcx::error("$!dcx(ActiveWindow)", "Insufficient parameters");
		return 0;
	}

	HWND hwnd = GetForegroundWindow();

	if (!IsWindow(hwnd)) {
		Dcx::error("$!dcx(ActiveWindow)", "Unable to determine active window");
		return 0;
	}

	TString prop(input.gettok(1));
	WINDOWINFO wi;

	ZeroMemory(&wi, sizeof(WINDOWINFO));
	GetWindowInfoUx(hwnd, &wi);

	if (prop == "hwnd")         // handle
		wsprintf(data, "%d", hwnd);
	else if (prop == "x")       // left
		wsprintf(data, "%d", wi.rcWindow.left);
	else if (prop == "y")       // top
		wsprintf(data, "%d", wi.rcWindow.top);
	else if (prop == "w")       // width
		wsprintf(data, "%d", wi.rcWindow.right - wi.rcWindow.left);
	else if (prop == "h")       // height
		wsprintf(data, "%d", wi.rcWindow.bottom - wi.rcWindow.top);
	else if (prop == "caption") // title text
		GetWindowText(hwnd, data, 900);
	else {                      // otherwise
		Dcx::error("$!dcx(ActiveWindow)", "Invalid parameters");
		return 0;
	}

	return 3;
}

mIRC(GhostDrag) {
	TString input(data);
	input.trim();

	if (input == "")
	{
		Dcx::error("/dcx GhostDrag", "Invalid parameters");
		return 0;
	}

	// [0-255] enable or (255 == disable) ghost drag for main mIRC window.
	BYTE alpha = (BYTE)(input.gettok(1).to_int() & 0xFF);

	if (!Dcx::setGhostDrag(alpha))
	{
		Dcx::error("/dcx GhostDrag", "Invalid alpha value");
		return 0;
	}

	return 1;
}
