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

//FILE * logFile;

//IClassFactory * g_pClassFactory = nullptr; //!< Web Control Factory


// Tray Icon stuff
DcxTrayIcon *trayIcons = nullptr; // tray icon manager


// XMenuBar stuff
HMENU g_OriginalMenuBar = nullptr;
XPopupMenu *g_mIRCScriptMenu = nullptr; // Wrapper for the mIRC scriptable menu.


// XPopup Stuff

SIGNALSWITCH dcxSignal;
/*
	* DllMain()
	* function added to insure that only a single instance of dcx.dll is loaded.
*/

HANDLE hDcxMutex = nullptr;
//extern BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved );

/*! \brief DllMain function is used to ensure that only one copy of DCX is loaded at a time.
 *
 */
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
			TCHAR mutex[128];
			// Initialize once for each new process.
			// Return FALSE to fail DLL load.
			DisableThreadLibraryCalls(hinstDLL);
			// add pid of mIRC.exe to name so mutex is specific to this instance of mIRC.
			// GetModuleHandle(nullptr) was returning a consistant result.
			wnsprintf(mutex, 128, TEXT("DCX_LOADED%lx"), GetCurrentProcessId()); // NB: calls user32.dll, is this ok? See warnings in DllMain() docs.

			// Enforce only one instance of dcx.dll loaded at a time.
			hDcxMutex = CreateMutex(nullptr, TRUE, mutex); // Windows 2000:  Do not create a named synchronization object in DllMain because the system will then load an additional DLL. This restriction does not apply to subsequent versions of Windows.
			if (hDcxMutex == nullptr) return FALSE;		// TODO: solve this issue or are we going to make the dll XP+ only now?
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
		if (hDcxMutex != nullptr) {
			ReleaseMutex(hDcxMutex);
			CloseHandle(hDcxMutex);
			hDcxMutex = nullptr;
		}
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
#ifndef __INTEL_COMPILER
	UNREFERENCED_PARAMETER(lpReserved);
#endif
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
	try {
		Dcx::load(load);

		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Initialising UltraDock..."));
		InitUltraDock();
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("LoadDLL"), TEXT("error: %S"), e.what());
	}
	catch (...)
	{
		Dcx::error(TEXT("LoadDLL"), TEXT("error: Unknown Exception"));
	}
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

		return 1;
	}
	// keep DLL in memory
	else
		return 0;
}

/*!
* \brief DCX DLL Version Function
*/
#include <ColourString.h>
mIRC(Version) {
#if DCX_DEBUG_OUTPUT
	TString tmp(TEXT("This is a token string    "));

	tmp.trim();

	tmp.addtok(100);	// "This is a token string 100"
	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.to_chr());

	tmp.addtok(TEXT("chars"));	// "This is a token string 100 chars"
	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.to_chr());

	tmp.instok(TEXT("m_inserted"),5); // "This is a token m_inserted string 100 chars"
	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.to_chr());

	tmp.instok(TEXT("s_inserted"), 1); // "s_inserted This is a token m_inserted string 100 chars"
	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.to_chr());

	tmp.instok(TEXT("e_inserted"), 10);	// "s_inserted This is a token m_inserted string 100 chars e_inserted"
	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.to_chr());

	tmp.deltok(2);	// "s_inserted is a token m_inserted string 100 chars e_inserted"
	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.to_chr());

	tmp.deltok(1);	// "is a token m_inserted string 100 chars e_inserted"
	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.to_chr());

	tmp.deltok(8);	// "is a token m_inserted string 100 chars"
	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.to_chr());

	tmp.puttok(TEXT("put"), 4);	// "is a token put string 100 chars"
	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.to_chr());

	tmp.puttok(TEXT("putter"), 1);	// "putter a token put string 100 chars"
	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.to_chr());

	tmp.puttok(TEXT("putted!"), 8);	// "putter a token put string 100 putted!"
	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.to_chr());

	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.gettok(2,2).to_chr()); // "a"
	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.gettok(2, 3).to_chr()); // "a token"
	mIRCLinker::execex(TEXT("/echo -a test: %s"), tmp.gettok(2, -1).to_chr()); // "a token put string 100 putted!"
	TString tsNum(tmp.gettok(6));
	int t = 0;
	tsNum >> t;
	tsNum << " text " << t << TEXT(" some morer ");
	tsNum.append_number(3.14);

	mIRCLinker::execex(TEXT("/echo -a convert: %s :: %d"), tsNum.to_chr(), t); // "a token put string 100 putted!"

	TString tok = tsNum++;
	mIRCLinker::execex(TEXT("/echo -a convert2: %s"), tok.to_chr()); // "a token put string 100 putted!"

	tok = tsNum++;
	mIRCLinker::execex(TEXT("/echo -a convert3: %s"), tok.to_chr()); // "a token put string 100 putted!"

	for (const auto &x : tmp)
	{
		mIRCLinker::execex(TEXT("/echo -a test for: %s"), x.to_chr());
	}

	TCHAR ctrlk = TEXT('\x03');
	std::basic_string<TCHAR> str(TEXT("test "));
	str += ctrlk;
	str += TEXT("04red");
	str += ctrlk;
	str += TEXT("\x02 bold\x02 ");	// space between \x02 & bold required, as it sees \x02 as \x02b otherwise
	str += TEXT(" \x1Funderline\x1F ");
	str += TEXT(" \x1Ditalic\x1D ");
	str += TEXT(" \x16reverse\x16 ");
	str += TEXT("\xF end");
	ColourString<TCHAR> cc(str);
	mIRCLinker::execex(TEXT("/echo -a test1: %s"), cc.ToString().c_str());
	ColourString<TCHAR> cc2(str);
	mIRCLinker::execex(TEXT("/echo -a test2: %s"), cc2.ToString().c_str());
	if (cc == cc2)
		mIRCLinker::execex(TEXT("/echo -a test3: The same '%s'"), cc2.ToString().c_str());
	else
		mIRCLinker::execex(TEXT("/echo -a test3: NOT The same '%s'"), cc2.ToString().c_str());

	cc2 += TEXT(" some extra text");
	if (cc == cc2)
		mIRCLinker::execex(TEXT("/echo -a test4: The same '%s'"), cc2.ToString().c_str());
	else
		mIRCLinker::execex(TEXT("/echo -a test4: NOT The same '%s'"), cc2.ToString().c_str());

	ColourString<TCHAR> cc3{ cc, TEXT(" "), cc2 };
	mIRCLinker::execex(TEXT("/echo -a initilizer list test: %s"), cc3.ToString().c_str());

	ColourString<char> cc4("test here");
	mIRCLinker::execex(TEXT("/echo -a char test5: %S"), cc4.ToString().c_str());

	if (cc != cc2)
		mIRCLinker::execex(TEXT("/echo -a test6: NOT The same '%s'"), cc2.ToString().c_str());
	else
		mIRCLinker::execex(TEXT("/echo -a test6: The same '%s'"), cc2.ToString().c_str());

	//mIRCLinker::execex(TEXT("/echo -a test7: rtf -> %s"), cc2.ToRtf().c_str());

#endif

#ifdef DCX_DEV_BUILD
	if (mIRCLinker::isUnicode())
	{
		wnsprintf(data, MIRC_BUFFER_SIZE_CCH,
			TEXT("DCX (XPopup) DLL %s %s%d UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2006-2014"),
			DLL_VERSION, DLL_STATE, DLL_DEV_BUILD);
	}
	else {
		wnsprintfA((char *)data, MIRC_BUFFER_SIZE_CCH,
			"DCX (XPopup) DLL %S %S%d UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2006-2014",
			DLL_VERSION, DLL_STATE, DLL_DEV_BUILD);
	}
#else
	if (mIRCLinker::isUnicode())
	{
		wnsprintf(data, MIRC_BUFFER_SIZE_CCH,
			TEXT("DCX (XPopup) DLL %s %s UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2006-2014"),
			DLL_VERSION, DLL_STATE);
	}
	else {
		wnsprintfA((char *)data, MIRC_BUFFER_SIZE_CCH,
			"DCX (XPopup) DLL %S %S UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2006-2014",
			DLL_VERSION, DLL_STATE);
	}
#endif
	return 3;
}

/*!
* \brief DCX DLL is DcxDirectShow supported?
*/
static TString dxData;
mIRC(IsUsingDirectX) {
	if (Dcx::isDX9Installed()) {
		ret(dxData.to_chr());
	}
	else if (Dcx::initDirectX(data, MIRC_BUFFER_SIZE_CCH)) {
		dxData = data;
		return 3;
	}
	ret(TEXT("$false"));
}

/*!
* \brief DCX DLL is GDI+ supported?
*/
mIRC(IsUsingGDI) {
	ret((Dcx::GDIModule.isUseable() ? TEXT("$true") : TEXT("$false")));
}

/*!
* \brief Check if it's safe to unload DLL
*/
mIRC(IsUnloadSafe) {
	ret((Dcx::isUnloadSafe() ? TEXT("$true") : TEXT("$false")));
}

/*!
* \brief Check if windows is themed
*/
mIRC(IsThemedXP) {
	ret((Dcx::UXModule.dcxIsThemeActive() ? TEXT("$true") : TEXT("$false")));
}

/*!
* \brief DCX DLL Mark Function
*
* Argument \b data contains -> [NAME] [ALIAS]
*/

// Mark [NAME] [ALIAS]
mIRC(Mark) {
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 2)
			throw std::invalid_argument("[NAME] [ALIAS]");

		return Dcx::mark(data, d.gettok(1), d.gettok(2));
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/dcx Mark"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/dcx Mark"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	ret(Dcx::getLastError());
}

/*!
* \brief DCX DLL GetSystemColor Function
*
* Argument \b data contains -> [ATTRIB] (not implemented yet, [DEFAULT])
*/
// GetSystemColor [ATTRIBUTE]
mIRC(GetSystemColor) {
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 1)
			throw std::invalid_argument("Invalid Arguments");

		int col;
		const TString coltype(d.gettok(1));

		if (coltype == TEXT("COLOR_3DDKSHADOW")) { col = COLOR_3DDKSHADOW; }
		else if (coltype == TEXT("COLOR_3DFACE")) { col = COLOR_3DFACE; }
		else if (coltype == TEXT("COLOR_3DHIGHLIGHT")) { col = COLOR_3DHIGHLIGHT; }
		else if (coltype == TEXT("COLOR_3DHILIGHT")) { col = COLOR_3DHILIGHT; }
		else if (coltype == TEXT("COLOR_3DLIGHT")) { col = COLOR_3DLIGHT; }
		else if (coltype == TEXT("COLOR_3DSHADOW")) { col = COLOR_3DSHADOW; }
		else if (coltype == TEXT("COLOR_ACTIVEBORDER")) { col = COLOR_ACTIVEBORDER; }
		else if (coltype == TEXT("COLOR_ACTIVECAPTION")) { col = COLOR_ACTIVECAPTION; }
		else if (coltype == TEXT("COLOR_APPWORKSPACE")) { col = COLOR_APPWORKSPACE; }
		else if (coltype == TEXT("COLOR_BACKGROUND")) { col = COLOR_BACKGROUND; }
		else if (coltype == TEXT("COLOR_BTNFACE")) { col = COLOR_BTNFACE; }
		else if (coltype == TEXT("COLOR_BTNHIGHLIGHT")) { col = COLOR_BTNHIGHLIGHT; }
		else if (coltype == TEXT("COLOR_BTNSHADOW")) { col = COLOR_BTNSHADOW; }
		else if (coltype == TEXT("COLOR_BTNTEXT")) { col = COLOR_BTNTEXT; }
		else if (coltype == TEXT("COLOR_CAPTIONTEXT")) { col = COLOR_CAPTIONTEXT; }
		else if (coltype == TEXT("COLOR_DESKTOP")) { col = COLOR_DESKTOP; }
		else if (coltype == TEXT("COLOR_GRADIENTACTIVECAPTION")) { col = COLOR_GRADIENTACTIVECAPTION; }
		else if (coltype == TEXT("COLOR_GRADIENTINACTIVECAPTION")) { col = COLOR_GRADIENTINACTIVECAPTION; }
		else if (coltype == TEXT("COLOR_GRAYTEXT")) { col = COLOR_GRAYTEXT; }
		else if (coltype == TEXT("COLOR_HIGHLIGHT")) { col = COLOR_HIGHLIGHT; }
		else if (coltype == TEXT("COLOR_HIGHLIGHTTEXT")) { col = COLOR_HIGHLIGHTTEXT; }
		else if (coltype == TEXT("COLOR_HOTLIGHT")) { col = COLOR_HOTLIGHT; }
		else if (coltype == TEXT("COLOR_INACTIVEBORDER")) { col = COLOR_INACTIVEBORDER; }
		else if (coltype == TEXT("COLOR_INACTIVECAPTION")) { col = COLOR_INACTIVECAPTION; }
		else if (coltype == TEXT("COLOR_INACTIVECAPTIONTEXT")) { col = COLOR_INACTIVECAPTIONTEXT; }
		else if (coltype == TEXT("COLOR_INFOBK")) { col = COLOR_INFOBK; }
		else if (coltype == TEXT("COLOR_INFOTEXT")) { col = COLOR_INFOTEXT; }
		else if (coltype == TEXT("COLOR_MENU")) { col = COLOR_MENU; }
		else if (coltype == TEXT("COLOR_MENUHILIGHT")) { col = COLOR_MENUHILIGHT; }
		else if (coltype == TEXT("COLOR_MENUBAR")) { col = COLOR_MENUBAR; }
		else if (coltype == TEXT("COLOR_MENUTEXT")) { col = COLOR_MENUTEXT; }
		else if (coltype == TEXT("COLOR_SCROLLBAR")) { col = COLOR_SCROLLBAR; }
		else if (coltype == TEXT("COLOR_WINDOW")) { col = COLOR_WINDOW; }
		else if (coltype == TEXT("COLOR_WINDOWFRAME")) { col = COLOR_WINDOWFRAME; }
		else if (coltype == TEXT("COLOR_WINDOWTEXT")) { col = COLOR_WINDOWTEXT; }
		else if (coltype == TEXT("COLOR_GLASS")) { col = -1; }
		else
			throw std::invalid_argument("Invalid parameter specified");

		if (col == -1) {
			RGBQUAD clr = { 0 };
			BOOL bOpaque = FALSE;
			if (FAILED(Dcx::VistaModule.dcxDwmGetColorizationColor((DWORD *)&clr, &bOpaque)))
				throw std::runtime_error("Unable to get glass colour.");
		
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), RGB(clr.rgbRed, clr.rgbGreen, clr.rgbBlue));
		}
		else {
			// max of 8 digits, 9 for null terminator
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), GetSysColor(col));
		}
		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!dcx(GetSystemColor)"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!dcx(GetSystemColor)"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
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

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 3)
			throw std::invalid_argument("Invalid Arguments");

		const TString tsDname(d.getfirsttok(1));
		DcxDialog * p_Dialog = Dcx::Dialogs.getDialogByName(tsDname);

		if (p_Dialog == nullptr)
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unknown dialog \"%s\": see Mark command"), tsDname.to_chr()));

		const TString IDs(d.getnexttok());			// tok 2
		const TString tsArgs(d.getlasttoks());		// tok 3, -1
		TString d2;
		const int n = IDs.numtok(TSCOMMA);

		// Multiple IDs id,id,id,id-id,id-id
		if (n > 1) {

			IDs.getfirsttok(0, TSCOMMA);

			for (int i = 1; i <= n; i++) {
				const TString tsID(IDs.getnexttok(TSCOMMA));	// tok i
				UINT id_start = 0, id_end = 0;
				if (tsID.numtok(TEXT("-")) == 2) {
					id_start = tsID.getfirsttok(1, TEXT("-")).to_int();
					id_end = tsID.getnexttok(TEXT("-")).to_int();
				}
				else
					id_start = id_end = tsID.to_int();

				if (id_end < id_start)
					throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid ID : %ld (dialog : %s)"), id_end, tsDname.to_chr()));

				for (UINT id = id_start; id <= id_end; id++) {
					DcxControl * p_Control = p_Dialog->getControlByID(id + mIRC_ID_OFFSET);

					if (p_Control == nullptr)
						throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid ID : %ld (dialog : %s)"), id, tsDname.to_chr()));

					d2.tsprintf(TEXT("%s %ld %s"), tsDname.to_chr(), id, tsArgs.to_chr());

					p_Control->parseCommandRequest(d2);
				}
			}
		}
		//Single ID or single id-id
		else {
			UINT id_start = 0, id_end = 0;
			if (IDs.numtok(TEXT("-")) == 2) {
				id_start = IDs.getfirsttok(1, TEXT("-")).to_int();
				id_end = IDs.getnexttok(TEXT("-")).to_int();
			}
			else
				id_start = id_end = IDs.to_int();

			for (UINT id = id_start; id <= id_end; id++) {
				DcxControl * p_Control = p_Dialog->getControlByID(id + mIRC_ID_OFFSET);

				if (p_Control == nullptr)
					throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid ID : %ld (dialog : %s)"), id, tsDname.to_chr()));

				p_Control->parseCommandRequest(d);
			}
		}
		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/xdid"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/xdid"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
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

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 3)
			throw std::invalid_argument("Invalid Arguments");

		const TString tsDname(d.getfirsttok(1));
		const TString tsID(d.getnexttok());

		DcxDialog * p_Dialog = Dcx::Dialogs.getDialogByName(tsDname);

		if (p_Dialog == nullptr)
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unknown dialog \"%s\": see Mark command"), tsDname.to_chr()));

		const DcxControl * p_Control = p_Dialog->getControlByID((UINT)tsID.to_int() + mIRC_ID_OFFSET);

		if (p_Control == nullptr)
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid ID : %s (dialog %s)"), tsID.to_chr(), tsDname.to_chr()));

		p_Control->parseInfoRequest(d, data);
		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!xdid"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!xdid"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
}

/*!
* \brief DCX DLL $dcx(GetTaskbarPos) Function
*
* $dcx(GetTaskbarPos) interface
*
*/
mIRC(GetTaskbarPos) {
	try {
		HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), nullptr);

		if (!IsWindow(hTaskbar))
			throw std::runtime_error("could not find taskbar");
		
		RECT rc;

		if (!GetWindowRect(hTaskbar, &rc))
			throw std::runtime_error("Unable to get window rect");

		wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!dcx(GetTaskbarPos)"), TEXT("error: %S"), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("$!dcx(GetTaskbarPos)"), TEXT("error: Unknown Exception"));
	}
	return 0;
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

	data[0] = 0;

	try {
		d.trim();

		const TString tsDname(d.gettok(1));

		if (d.numtok() < 2)
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid arguments ( dialog %s)"), tsDname.to_chr()));

		DcxDialog * p_Dialog = Dcx::Dialogs.getDialogByName(tsDname);

		if (p_Dialog == nullptr)
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unknown dialog \"%s\": see Mark command"), tsDname.to_chr()));

		p_Dialog->parseCommandRequest(d);
		return 1;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/xdialog"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/xdialog"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
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

	// reset mIRC data
	data[0] = 0;

	try {
		d.trim();

		const TString tsDname(d.getfirsttok(1));

		if (d.numtok() < 2)
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid arguments ( dialog %s)"), tsDname.to_chr()));

		const DcxDialog *p_Dialog = Dcx::Dialogs.getDialogByName(tsDname);

		if (p_Dialog == nullptr) {
			if (d.getnexttok() != TEXT("ismarked"))
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unknown dialog \"%s\": see Mark command"), tsDname.to_chr()));

			ret(TEXT("$false"));
		}

		p_Dialog->parseInfoRequest(d, data);
		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!xdialog"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!xdialog"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
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

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 3)
			throw std::invalid_argument("Invalid Arguments");

		const TString tsMenu(d.gettok(1));

		if ((tsMenu == TEXT("mirc")) || (tsMenu == TEXT("mircbar")))
			throw std::invalid_argument("Invalid menu name : mirc or mircbar menus don't have access to this feature.");

		XPopupMenu *p_Menu = Dcx::XPopups.getMenuByName(tsMenu, FALSE);

		if (p_Menu == nullptr)
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unknown menu \"%s\": see /xpopup -c command"), tsMenu.to_chr()));

		p_Menu->parseXPopCommand(d);
		return 1;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/xpop"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/xpop"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
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

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 3)
			throw std::invalid_argument("Invalid Arguments");

		const TString tsMenu(d.gettok(1));

		if ((tsMenu == TEXT("mirc")) || (tsMenu == TEXT("mircbar")))
			throw std::invalid_argument("Invalid menu name : mirc or mircbar menus don't have access to this feature.");

		const XPopupMenu *p_Menu = Dcx::XPopups.getMenuByName(tsMenu, FALSE);

		if (p_Menu == nullptr)
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unknown menu \"%s\": see /xpopup -c command"), tsMenu.to_chr()));

		p_Menu->parseXPopIdentifier(d, data);
		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!xpop"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!xpop"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
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

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 2)
			throw std::invalid_argument("Invalid Arguments");

		Dcx::XPopups.parseCommand(d);
		return 1;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/xpopup"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/xpopup"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
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

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 2)
			throw std::invalid_argument("Invalid Arguments");

		Dcx::XPopups.parseIdentifier(d, data);
		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!xpopup"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!xpopup"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
}

/*!
* \brief XMenuBar DLL /xmenubar Function
*
* mIRC /xmenubar -switch (options)
*/
mIRC(xmenubar) {
	TString d(data);

	data[0] = 0;

	try {
		Dcx::XMenubar.parseXMenuBarCommand(d.trim());
		return 1;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/xmenubar"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/xmenubar"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
}

/*!
* \brief XMenuBar DLL $xmenubar Function
*
* mIRC $xmenubar(options).prop interface
*/
mIRC(_xmenubar) {
	TString d(data);

	data[0] = 0;

	try {
		Dcx::XMenubar.parseXMenuBarInfo(d.trim(), data);
		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!xmenubar"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!xmenubar"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
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

	try {
		return Dcx::XPopups.parseMPopup(d.trim());
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/mpopup"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/mpopup"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
}

// /dcx xSignal [1|0] (+FLAGS)
mIRC(xSignal) {
	TString d(data);

	data[0] = 0;

	try {
		// determine state
		const bool state = (d.getfirsttok(1).to_int() > 0);
		TString flags(d.trim().getnexttok());

		// if no flags specified, set all states
		if (flags.empty())
			flags = TEXT("+dst");

		const XSwitchFlags xflags(flags);

		if (!xflags[TEXT('+')])
			throw std::invalid_argument("flags must start with a '+'");

		if (xflags[TEXT('d')])
			dcxSignal.xdock = state;
		else if (xflags[TEXT('s')])
			dcxSignal.xstatusbar = state;
		else if (xflags[TEXT('t')])
			dcxSignal.xtray = state;
		else
			throw std::invalid_argument("Unknown flag specified.");

		return 1;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/dcx xSignal"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/dcx xSignal"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
}

// /dcx WindowProps [HWND] [+FLAGS] (ARGS)
mIRC(WindowProps) {
	const TString input(data);

	data[0] = TEXT('\0');

	try {
		const UINT numtok = input.numtok();

		if (numtok < 2)
			throw std::invalid_argument("Insuffient parameters");

		HWND hwnd = (HWND)input.getfirsttok(1).to_int();	// tok 1

		if (!IsWindow(hwnd))
			throw std::invalid_argument("Invalid window");

		const TString flags(input.getnexttok().trim());	// tok 2
		const XSwitchFlags xflags(flags);

		if (!xflags[TEXT('+')] || (flags.len() < 2))
			throw std::invalid_argument("No Flags Found");

		if (!xflags[TEXT('T')] && !xflags[TEXT('i')] && !xflags[TEXT('t')] && !xflags[TEXT('r')] && !xflags[TEXT('v')])
			throw std::invalid_argument("Unknown Flags");

		// set hwnd NoTheme
		// +T
		if (xflags[TEXT('T')]) {
			if (Dcx::UXModule.isUseable()) {
				if (Dcx::UXModule.dcxSetWindowTheme(hwnd, TEXT(" "), TEXT(" ")) != S_OK)
					throw std::runtime_error("Unable to set theme");
			}
		}
		// set hwnd's title icon
		// +i [INDEX] [FILENAME]
		if (xflags[TEXT('i')]) {
			// invalid args
			if (numtok < 3)
				throw std::invalid_argument("Invalid Args");

			const int index = input.getnexttok().to_int();	// tok 3
			TString filename(input.gettok(1, TSTAB).gettok(4, -1).trim());

			if (!ChangeHwndIcon(hwnd, flags, index, filename))
				throw std::runtime_error("Unable to set icon");
		}
		// set hwnd title text
		// +t [TEXT]
		else if (xflags[TEXT('t')]) {
			TString txt;

			if (xflags[TEXT('i')]) {
				if (input.numtok(TSTAB) > 1)
					txt = input.gettok(2, -1, TSTAB);
			}
			else if (numtok > 2)
				txt = input.getlasttoks();	// tok 3, -1

			SetWindowText(hwnd, txt.trim().to_chr());
		}
		// RMB click hwnd at pos.
		// +r [X] [Y]
		else if (xflags[TEXT('r')]) {
			const UINT x = (UINT)input.getnexttok().to_num();	// tok 3
			const UINT y = (UINT)input.getnexttok().to_num();	// tok 4
			const LPARAM parm = MAKELONG(x, y);
			SendMessage(hwnd, WM_RBUTTONDOWN, MK_RBUTTON, parm);
			PostMessage(hwnd, WM_RBUTTONUP, MK_RBUTTON, parm); // MUST be a PostMessage or the dll hangs untill the menu is closed.
		}
		// Add Vista+ glass effect to window.
		// +v [top] [left] [bottom] [right]
		else if (xflags[TEXT('v')]) {
			MARGINS margin;
			margin.cyTopHeight = (INT)input.getnexttok().to_num();		// tok 3
			margin.cxLeftWidth = (INT)input.getnexttok().to_num();		// tok 4
			margin.cyBottomHeight = (INT)input.getnexttok().to_num();	// tok 5
			margin.cxRightWidth = (INT)input.getnexttok().to_num();	// tok 6
			AddStyles(hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
			//RGBQUAD clr = {0};
			//BOOL bOpaque = FALSE;
			//Dcx::VistaModule.dcxDwmGetColorizationColor((DWORD *)&clr, &bOpaque);
			//HDC vhdc = GetDC(hwnd);
			//if (vhdc != nullptr) {
			//	SetBkColor(vhdc,RGB(clr.rgbRed,clr.rgbGreen,clr.rgbBlue));
			//	ReleaseDC(hwnd,vhdc);
			//}
			//SetLayeredWindowAttributes(hwnd, RGB(clr.rgbRed,clr.rgbGreen,clr.rgbBlue), 0, LWA_COLORKEY);
			Dcx::VistaModule.dcxDwmExtendFrameIntoClientArea(hwnd, &margin);
			RedrawWindow(hwnd, nullptr, nullptr, RDW_INTERNALPAINT | RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_UPDATENOW);
		}
		return 1;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/dcx WindowProps"), TEXT("\"%s\" error: %S"), input.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/dcx WindowProps"), TEXT("\"%s\" error: Unknown Exception"), input.to_chr());
	}
	return 0;
}

/*! \brief $dcx(ActiveWindow, property)
 *
 * $dcx(ActiveWindow, x)
 * $dcx(ActiveWindow, y)
 * $dcx(ActiveWindow, w)
 * $dcx(ActiveWindow, h)
 * $dcx(ActiveWindow, caption)
 * $dcx(ActiveWindow, hwnd)
 */
mIRC(ActiveWindow) {
	TString input(data);

	data[0] = 0;

	try {
		input.trim();

		const UINT numtok = input.numtok();

		if (numtok < 1)
			throw std::invalid_argument("Insufficient parameters");

		HWND hwnd = GetForegroundWindow();

		if (!IsWindow(hwnd))
			throw std::invalid_argument("Unable to determine active window");

		const TString prop(input.gettok(1));
		WINDOWINFO wi;

		ZeroMemory(&wi, sizeof(WINDOWINFO));
		GetWindowInfo(hwnd, &wi);

		if (prop == TEXT("hwnd"))         // handle
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), (DWORD)hwnd);	// don't use %p is this gives a hex result.
		else if (prop == TEXT("x"))       // left
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), wi.rcWindow.left);
		else if (prop == TEXT("y"))       // top
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), wi.rcWindow.top);
		else if (prop == TEXT("w"))       // width
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), wi.rcWindow.right - wi.rcWindow.left);
		else if (prop == TEXT("h"))       // height
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), wi.rcWindow.bottom - wi.rcWindow.top);
		else if (prop == TEXT("caption")) // title text
			GetWindowText(hwnd, data, MIRC_BUFFER_SIZE_CCH);
		else                      // otherwise
			throw std::invalid_argument("Invalid parameters");

		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!dcx(ActiveWindow)"), TEXT("\"%s\" error: %S"), input.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!dcx(ActiveWindow)"), TEXT("\"%s\" error: Unknown Exception"), input.to_chr());
	}
	return 0;
}

/*! \brief /dcx GhostDrag 0-255
 *
 */
mIRC(GhostDrag) {
	TString input(data);

	data[0] = 0;

	try {
		input.trim();

		if (input.empty())
			throw std::invalid_argument("Invalid parameters");

		// [0-255] enable or (255 == disable) ghost drag for main mIRC window.
		const BYTE alpha = (BYTE)(input.gettok(1).to_int() & 0xFF);

		if (!Dcx::setGhostDrag(alpha))
			throw std::invalid_argument("Invalid alpha value");

		return 1;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/dcx GhostDrag"), TEXT("\"%s\" error: %S"), input.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/dcx GhostDrag"), TEXT("\"%s\" error: Unknown Exception"), input.to_chr());
	}
	return 0;
}

/*!
* \brief XPopup DLL /dcx SetSystemCursors Function
*
* mIRC /dcx SetSystemCursors [cursor id] (filename)
*
* Argument \b data contains -> [CURSOR ID] (filename)
*/
mIRC(SetSystemCursors) {
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.empty())
			throw std::invalid_argument("Invalid Arguments");

		const TString tsCursor(d.getfirsttok(1));
		TString tsFilename(d.getnexttok());
		const DWORD systemCursorId = Dcx::parseSystemCursorType(tsCursor);

		if (!IsFile(tsFilename))
			throw std::invalid_argument("Unable to open file");

		HCURSOR hCursor = (HCURSOR)LoadImage(nullptr, tsFilename.to_chr(), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);

		if (hCursor == nullptr)
			throw std::runtime_error("Unable to load cursor");

		if (!SetSystemCursor(hCursor, systemCursorId))
			throw std::runtime_error("Unable to set system cursor");

		return 1;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/dcx SetSystemCursors"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/dcx SetSystemCursors"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
}
