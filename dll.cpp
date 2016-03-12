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
	try {
		TString tmp(TEXT("This is a token string    "));

		tmp.trim();

		tmp.addtok(100);	// "This is a token string 100"
		mIRCLinker::execex(TEXT("/echo -a addtok(100): %s"), tmp.to_chr());

		tmp.addtok(TEXT("chars"));	// "This is a token string 100 chars"
		mIRCLinker::execex(TEXT("/echo -a addtok(chars): %s"), tmp.to_chr());

		tmp.instok(TEXT("m_inserted"), 5); // "This is a token m_inserted string 100 chars"
		mIRCLinker::execex(TEXT("/echo -a instok(m_inserted,5): %s"), tmp.to_chr());

		tmp.instok(TEXT("s_inserted"), 1); // "s_inserted This is a token m_inserted string 100 chars"
		mIRCLinker::execex(TEXT("/echo -a instok(s_inserted,1): %s"), tmp.to_chr());

		tmp.instok(TEXT("e_inserted"), 10);	// "s_inserted This is a token m_inserted string 100 chars e_inserted"
		mIRCLinker::execex(TEXT("/echo -a instok(e_inserted,10): %s"), tmp.to_chr());

		tmp.deltok(2);	// "s_inserted is a token m_inserted string 100 chars e_inserted"
		mIRCLinker::execex(TEXT("/echo -a deltok(2): %s"), tmp.to_chr());

		tmp.deltok(1);	// "is a token m_inserted string 100 chars e_inserted"
		mIRCLinker::execex(TEXT("/echo -a deltok(1): %s"), tmp.to_chr());

		tmp.deltok(8);	// "is a token m_inserted string 100 chars"
		mIRCLinker::execex(TEXT("/echo -a deltok(8): %s"), tmp.to_chr());

		tmp.puttok(TEXT("put"), 4);	// "is a token put string 100 chars"
		mIRCLinker::execex(TEXT("/echo -a puttok(put,4): %s"), tmp.to_chr());

		tmp.puttok(TEXT("putter"), 1);	// "putter a token put string 100 chars"
		mIRCLinker::execex(TEXT("/echo -a puttok(putter,1): %s"), tmp.to_chr());

		tmp.puttok(TEXT("putted"), 7);	// "putter a token put string 100 putted"
		mIRCLinker::execex(TEXT("/echo -a puttok(putted,7): %s"), tmp.to_chr());

		tmp.puttok(TEXT("e_put"), 8);	// "putter a token put string 100 putted e_put"
		mIRCLinker::execex(TEXT("/echo -a puttok(e_putt,8): %s"), tmp.to_chr());

		TString tsTestCrash;
		tsTestCrash = tmp.gettok(2);
		tsTestCrash = tmp.gettok(2, -1);

		mIRCLinker::execex(TEXT("/echo -a gettok(2,2): %s"), tmp.gettok(2, 2).to_chr()); // "a"
		mIRCLinker::execex(TEXT("/echo -a gettok(2,3): %s"), tmp.gettok(2, 3).to_chr()); // "a token"
		mIRCLinker::execex(TEXT("/echo -a gettok(2,numtok()): %s"), tmp.gettok(2, static_cast<int>(tmp.numtok())).to_chr()); // "a token put string 100 putted e_put"
		mIRCLinker::execex(TEXT("/echo -a gettok(2,-1): %s"), tmp.gettok(2, -1).to_chr()); // "a token put string 100 putted e_put"
		auto tsNum(tmp.gettok(6));	// "putter a token put string 100 putted e_put" == 100
		auto t = 0;
		//tsNum >> t;
		//tsNum << " text " << t << TEXT(" some morer ");

		tsNum += 3.14;
		tsNum += t;
		t = tsNum.to_<int>();

		mIRCLinker::execex(TEXT("/echo -a tsNum: %s :tsNum.to_<int>(): %d"), tsNum.to_chr(), t); // "1003.140000"

		auto tok = tsNum++;
		mIRCLinker::execex(TEXT("/echo -a tsNum++: %s"), tok.to_chr()); // "1003.140000"

		tok = tsNum++;
		mIRCLinker::execex(TEXT("/echo -a tsNum++: %s"), tok.to_chr()); // ""

		tsNum = TEXT("blah1 blah2 blah3");
		tsNum++;
		tok = *tsNum;
		mIRCLinker::execex(TEXT("/echo -a tsNum: %s"), tsNum.to_chr()); // ""
		mIRCLinker::execex(TEXT("/echo -a *tsNum: %s"), tok.to_chr()); // ""

		//for (auto itStart = tmp.begin(), itEnd = tmp.end(); itStart != itEnd; ++itStart)
		//{
		//	mIRCLinker::execex(TEXT("/echo -a test for: %s"), (*itStart).to_chr());
		//}

		auto itStart = tmp.begin(), itEnd = tmp.end(); 
		while (itStart != itEnd)
		{
			const TString tsTmp(*itStart);
			mIRCLinker::execex(TEXT("/echo -a iterate tmp: %u :: %s"), (size_t)itStart, tsTmp.to_chr());
			++itStart;
		}
		tok.join(tmp, TEXT("!"));
		mIRCLinker::execex(TEXT("/echo -a tok.join(tmp,!): %s"), tok.to_chr());

		t = Dcx::parse_string<int>("200");
		mIRCLinker::execex(TEXT("/echo -a parse_string<int>(200): %d"), t); // 100

		tok.addtok(TEXT('b'), TEXT('n'));

		mIRCLinker::execex(TEXT("/echo -a addtok(b,n): %s"), tok.to_chr());

		{
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
			mIRCLinker::execex(TEXT("/echo -a cc.ToString().c_str(): %s"), cc.ToString().c_str());
			ColourString<TCHAR> cc2(str);
			mIRCLinker::execex(TEXT("/echo -a cc2.ToString().c_str(): %s"), cc2.ToString().c_str());
			if (cc == cc2)
				mIRCLinker::exec(TEXT("/echo -a cc == cc2"));
			else
				mIRCLinker::exec(TEXT("/echo -a cc != cc2"));

			cc2 += TEXT(" some extra text");

			mIRCLinker::execex(TEXT("/echo -a cc.ToString().c_str(): %s"), cc.ToString().c_str());
			mIRCLinker::execex(TEXT("/echo -a cc2.ToString().c_str(): %s"), cc2.ToString().c_str());

			if (cc == cc2)
				mIRCLinker::exec(TEXT("/echo -a cc == cc2"));
			else
				mIRCLinker::exec(TEXT("/echo -a cc != cc2"));

			ColourString<TCHAR> cc3{ cc, TEXT(" "), cc2 };
			mIRCLinker::execex(TEXT("/echo -a ColourString<TCHAR> cc3{ cc, , cc2 }: %s"), cc3.ToString().c_str());

			ColourString<char> cc4("test here");
			mIRCLinker::execex(TEXT("/echo -a ColourString<char> cc4(test here): %S"), cc4.ToString().c_str());

			if (cc3 != cc2)
				mIRCLinker::exec(TEXT("/echo -a cc3 != cc2"));
			else
				mIRCLinker::exec(TEXT("/echo -a cc3 == cc2"));

			//mIRCLinker::execex(TEXT("/echo -a test7: rtf -> %s"), cc2.ToRtf().c_str());

			//tsprintf(tok, TEXT("convert%, %, %, %, %, %"), 6, tsNum, t, tmp, str.c_str(), cc.ToString().c_str());
			//mIRCLinker::execex(TEXT("/echo -a convert6: %s"), tok.to_chr());
			//tsprintf(str, TEXT("convert%, %, %, %, %"), std::to_wstring(7), tsNum.to_chr(), std::to_wstring(t), tmp.to_chr(), cc.ToString().c_str());
			//mIRCLinker::execex(TEXT("/echo -a convert7: %s"), str.c_str());
			//tok.clear();
			//tsprintf(tok, "convert%, %, %, %, %, %", 8, tsNum, t, tmp, str.c_str(), cc.ToString().c_str());
			//mIRCLinker::execex(TEXT("/echo -a convert8: %s"), tok.to_chr());

			auto str2 = std::to_string(str);
		}

		//tok = _ts_strstr("this is a sample text with some extra added", "sample");

		//mIRCLinker::execex(TEXT("/echo -a my_strstr: %s"), tok.to_chr());

		if (tok.istok(TEXT("m_inserted"),TEXT("!")))
			mIRCLinker::execex(TEXT("/echo -a istok(m_inserted,!): yes - %s"), tok.to_chr());
		else
			mIRCLinker::execex(TEXT("/echo -a istok(m_inserted,!): no - %s"), tok.to_chr());

		if (tok.istok(TEXT("string"), TEXT("!")))
			mIRCLinker::execex(TEXT("/echo -a istok(string,!): yes - %s"), tok.to_chr());
		else
			mIRCLinker::execex(TEXT("/echo -a istok(string,!): no - %s"), tok.to_chr());

		tok.reptok(TEXT("string"), TEXT("blobby"), 1, TEXT("!"));
		mIRCLinker::execex(TEXT("/echo -a reptok(string,blobby,1,!): %s"), tok.to_chr());
		tok.reptok(TEXT("e_putnb"), TEXT("blobby"), 1, TEXT("!"));
		mIRCLinker::execex(TEXT("/echo -a reptok(e_putnb,blobby,1,!): %s"), tok.to_chr());
		tok.reptok(TEXT("blobby"), TEXT("mister"), 2, TEXT("!"));
		mIRCLinker::execex(TEXT("/echo -a reptok(blobby,mister,2,!): %s"), tok.to_chr());

		tok += TEXT("!101!102!200!4!5");

		tok.sorttok(TEXT(""), TEXT("!"));	// alpha sort
		mIRCLinker::execex(TEXT("/echo -a sorttok(): %s"), tok.to_chr());
		tok.sorttok(TEXT("r"), TEXT("!"));	// reverse alpha sort
		mIRCLinker::execex(TEXT("/echo -a sorttok(r): %s"), tok.to_chr());
		tok.sorttok(TEXT("a"), TEXT("!"));	// alphanumeric sort
		mIRCLinker::execex(TEXT("/echo -a sorttok(a): %s"), tok.to_chr());
		tok.sorttok(TEXT("ar"), TEXT("!"));	// reverse alphanumeric
		mIRCLinker::execex(TEXT("/echo -a sorttok(ar): %s"), tok.to_chr());
		tok.sorttok(TEXT("n"), TEXT("!"));	// numeric sort
		mIRCLinker::execex(TEXT("/echo -a sorttok(n): %s"), tok.to_chr());
		tok.sorttok(TEXT("nr"), TEXT("!"));	// reverse numeric sort
		mIRCLinker::execex(TEXT("/echo -a sorttok(nr): %s"), tok.to_chr());
		tok.sorttok(TEXT("c"), TEXT("!"));	// channel prefix sort
		mIRCLinker::execex(TEXT("/echo -a sorttok(c): %s"), tok.to_chr());
		tok.sorttok(TEXT("cr"), TEXT("!"));	// reverse channel prefix sort
		mIRCLinker::execex(TEXT("/echo -a sorttok(cr): %s"), tok.to_chr());

		mIRCLinker::execex(TEXT("/echo -a wildtok(put*,2,!): %s"), tok.wildtok(TEXT("put*"), 2, TEXT("!")).to_chr());
		mIRCLinker::execex(TEXT("/echo -a nwildtok(put*,!): %u"), tok.nwildtok(TEXT("put*"), TEXT("!")));

		mIRCLinker::execex(TEXT("/echo -a matchtok(put,1,!): %s"), tok.matchtok(TEXT("put"), 1, TEXT("!")).to_chr());
		mIRCLinker::execex(TEXT("/echo -a matchtok(put,3,!): %s"), tok.matchtok(TEXT("put"), 3, TEXT("!")).to_chr());

		if (tok.iswm(TEXT("*put*")))
			mIRCLinker::exec(TEXT("/echo -a tok.iswm(*put*) == true"));
		else
			mIRCLinker::exec(TEXT("/echo -a tok.iswm(*put*) == false"));

		if (tok.iswmcs(TEXT("*put*")))
			mIRCLinker::exec(TEXT("/echo -a tok.iswmcs(*put*) == true"));
		else
			mIRCLinker::exec(TEXT("/echo -a tok.iswmcs(*put*) == false"));

		{
			const TCHAR *ctmp = TEXT("test");
			mIRCLinker::execex(TEXT("/echo -a tmp: %s"), ctmp);
			auto test1 = TString(TEXT("test"));
			mIRCLinker::execex(TEXT("/echo -a test1: %s"), test1.to_chr());
			auto test2 = L"test"_ts;
			mIRCLinker::execex(TEXT("/echo -a test2: %s"), test2.to_chr());
			auto test3 = TString("test");
			mIRCLinker::execex(TEXT("/echo -a test3: %s"), test3.to_chr());
			auto test4 = TString(test1);
			mIRCLinker::execex(TEXT("/echo -a test4: %s"), test4.to_chr());
			auto test5 = "test"_ts;
			mIRCLinker::execex(TEXT("/echo -a test5: %s"), test5.to_chr());
			auto test6 = TString(ctmp);
			mIRCLinker::execex(TEXT("/echo -a test6: %s"), test6.to_chr());
			if (_ts_isEmpty(test6))
				mIRCLinker::exec(TEXT("/echo -a test6: true"));
			else
				mIRCLinker::exec(TEXT("/echo -a test6: false"));
			if (_ts_isEmpty(ctmp))
				mIRCLinker::exec(TEXT("/echo -a test8: true"));
			else
				mIRCLinker::exec(TEXT("/echo -a test8: false"));
			//if (_ts_isEmpty(str))
			//	mIRCLinker::exec(TEXT("/echo -a test9: true"));
			//else
			//	mIRCLinker::exec(TEXT("/echo -a test9: false"));
			//if (_ts_isEmpty(cc))
			//	mIRCLinker::exec(TEXT("/echo -a test10: true"));
			//else
			//	mIRCLinker::exec(TEXT("/echo -a test10: false"));
		}

		{
			auto toktest = tok.gettok(2,TEXT('!'));
			mIRCLinker::execex(TEXT("/echo -a toktest1: %s"), toktest.to_chr());
			toktest = tok.gettok(3, -1, TEXT('!'));
			mIRCLinker::execex(TEXT("/echo -a toktest2: %s"), toktest.to_chr());
			toktest = tok.getfirsttok(1, TEXT('!'));
			mIRCLinker::execex(TEXT("/echo -a toktest3: %s"), toktest.to_chr());
			toktest = tok.getnexttok(TEXT('!'));
			mIRCLinker::execex(TEXT("/echo -a toktest4: %s"), toktest.to_chr());
		}

		// test exception code
		tok = TEXT("123");
		UINT iTest = Dcx::numeric_cast<UINT>(tok);
		//iTest = Dcx::numeric_cast<UINT>("400");

		//{
		//	auto rng = Dcx::range(10U, 20U);
		//	for (auto rItStart = rng.begin(), rItEnd = rng.end(); rItStart != rItEnd; ++rItStart)
		//	{
		//		mIRCLinker::execex(TEXT("/echo -a range: %u"), *rItStart);
		//	}
		//}

		// test expanding a TString object beyond the internal buffer
		{
			TString tsExp(TEXT("expanding text")); // len = 14
			tsExp *= 20;
			mIRCLinker::execex(TEXT("/echo -a expand: %u :: %s"), tsExp.len(), tsExp.to_chr());
		}
		//throw Dcx::dcxException(TEXT("No such Exception"));
		throw Dcx::dcxException("No such Exception: % :: %", iTest, tok);
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("Version"), TEXT("error: %S"), e.what());
	}
#endif

#ifdef DCX_DEV_BUILD
	if (mIRCLinker::isUnicode())
	{
		wnsprintf(data, MIRC_BUFFER_SIZE_CCH,
			TEXT("DCX (XPopup) DLL %s %s%d UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2006-2016"),
			DLL_VERSION, DLL_STATE, DLL_DEV_BUILD);
	}
	else {
		wnsprintfA((char *)data, MIRC_BUFFER_SIZE_CCH,
			"DCX (XPopup) DLL %S %S%d UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2006-2016",
			DLL_VERSION, DLL_STATE, DLL_DEV_BUILD);
	}
#else
	if (mIRCLinker::isUnicode())
	{
		wnsprintf(data, MIRC_BUFFER_SIZE_CCH,
			TEXT("DCX (XPopup) DLL %s %s UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2006-2016"),
			DLL_VERSION, DLL_STATE);
	}
	else {
		wnsprintfA((char *)data, MIRC_BUFFER_SIZE_CCH,
			"DCX (XPopup) DLL %S %S UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2006-2016",
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
			throw Dcx::dcxException("[NAME] [ALIAS]");

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
			throw Dcx::dcxException("Invalid Arguments");

		int col;
		const auto coltype(d.gettok(1));

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
			throw Dcx::dcxException("Invalid parameter specified");

		if (col == -1) {
			RGBQUAD clr = { 0 };
			BOOL bOpaque = FALSE;
			if (FAILED(Dcx::VistaModule.dcxDwmGetColorizationColor((DWORD *)&clr, &bOpaque)))
				throw Dcx::dcxException("Unable to get glass colour.");
		
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
			throw Dcx::dcxException("Invalid Arguments");

		const auto tsDname(d.getfirsttok(1));
		auto p_Dialog = Dcx::Dialogs.getDialogByName(tsDname);

		if (p_Dialog == nullptr)
			throw Dcx::dcxException(TEXT("Unknown dialog \"%\": see Mark command"), tsDname);

		const auto IDs(d.getnexttok());			// tok 2
		const auto tsArgs(d.getlasttoks());		// tok 3, -1
		TString d2;
		const auto n = IDs.numtok(TSCOMMACHAR);

		// Multiple IDs id,id,id,id-id,id-id
		if (n > 1) {
			for (auto itStart = IDs.begin(TSCOMMA), itEnd = IDs.end(); itStart != itEnd; ++itStart)
			{
				UINT id_start = 0, id_end = 0;
				const TString tsID(*itStart);
				if (tsID.numtok(TEXT('-')) == 2) {
					id_start = tsID.getfirsttok(1, TEXT('-')).to_<UINT>();
					id_end = tsID.getnexttok(TEXT('-')).to_<UINT>();
				}
				else
					id_start = id_end = p_Dialog->NameToUserID(tsID);

				if (id_end < id_start)
					throw Dcx::dcxException(TEXT("Invalid ID : % (dialog : %)"), id_end, tsDname);
				if (id_start == 0)
					throw Dcx::dcxException(TEXT("Invalid ID : % (dialog : %)"), id_start, tsDname);
				if (id_end == 0)
					throw Dcx::dcxException(TEXT("Invalid ID : % (dialog : %)"), id_end, tsDname);

				for (auto id = id_start; id <= id_end; id++) {
					auto p_Control = p_Dialog->getControlByID(id + mIRC_ID_OFFSET);

					if (p_Control == nullptr)
						throw Dcx::dcxException(TEXT("Unable to find control: % (dialog : %)"), id, tsDname);
					d2 = tsDname;
					d2.addtok(id);
					d2.addtok(tsArgs);

					p_Control->parseCommandRequest(d2);
				}
			}
		}
		//Single ID or single id-id
		else {
			UINT id_start = 0, id_end = 0;

			if (IDs.numtok(TEXT('-')) == 2) {
				id_start = IDs.getfirsttok(1, TEXT('-')).to_<UINT>();
				id_end = IDs.getnexttok(TEXT('-')).to_<UINT>();
			}
			else
				id_start = id_end = p_Dialog->NameToUserID(IDs);

			if (id_end < id_start)
				throw Dcx::dcxException(TEXT("Invalid ID : % (dialog : %)"), id_end, tsDname);
			if (id_start == 0)
				throw Dcx::dcxException(TEXT("Invalid ID : % (dialog : %)"), id_start, tsDname);
			if (id_end == 0)
				throw Dcx::dcxException(TEXT("Invalid ID : % (dialog : %)"), id_end, tsDname);

			for (auto id = id_start; id <= id_end; id++) {
				auto p_Control = p_Dialog->getControlByID(id + mIRC_ID_OFFSET);

				if (p_Control == nullptr)
					throw Dcx::dcxException(TEXT("Unable to find control: % (dialog : %)"), id, tsDname);

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
			throw Dcx::dcxException("Invalid Arguments");

		const auto tsDname(d.getfirsttok(1));
		const auto tsID(d.getnexttok());

		auto p_Dialog = Dcx::Dialogs.getDialogByName(tsDname);

		if (p_Dialog == nullptr)
			throw Dcx::dcxException(TEXT("Unknown dialog \"%\": see Mark command"), tsDname);

		if (!p_Dialog->isNamedId(tsID))
			throw Dcx::dcxException(TEXT("Invalid ID : % (dialog %)"), tsID, tsDname);

		const auto p_Control = p_Dialog->getControlByID(p_Dialog->NameToID(tsID));

		if (p_Control == nullptr)
			throw Dcx::dcxException(TEXT("Unable to find control: % (dialog %)"), tsID, tsDname);

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
		auto hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), nullptr);

		if (!IsWindow(hTaskbar))
			throw Dcx::dcxException("could not find taskbar");
		
		RECT rc;

		if (!GetWindowRect(hTaskbar, &rc))
			throw Dcx::dcxException("Unable to get window rect");

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

		const auto tsDname(d.gettok(1));

		if (d.numtok() < 2)
			throw Dcx::dcxException(TEXT("Invalid arguments ( dialog %)"), tsDname);

		auto p_Dialog = Dcx::Dialogs.getDialogByName(tsDname);

		if (p_Dialog == nullptr)
			throw Dcx::dcxException(TEXT("Unknown dialog \"%\": see Mark command"), tsDname);

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

		const auto tsDname(d.getfirsttok(1));

		if (d.numtok() < 2)
			throw Dcx::dcxException(TEXT("Invalid arguments ( dialog %)"), tsDname);

		const auto p_Dialog = Dcx::Dialogs.getDialogByName(tsDname);

		if (p_Dialog == nullptr) {
			if (d.getnexttok() != TEXT("ismarked"))
				throw Dcx::dcxException(TEXT("Unknown dialog \"%\": see Mark command"), tsDname);

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
			throw Dcx::dcxException("Invalid Arguments");

		const auto tsMenu(d.gettok(1));

		if ((tsMenu == TEXT("mirc")) || (tsMenu == TEXT("mircbar")))
			throw Dcx::dcxException("Invalid menu name : mirc or mircbar menus don't have access to this feature.");

		auto p_Menu = Dcx::XPopups.getMenuByName(tsMenu, false);

		if (p_Menu == nullptr)
			throw Dcx::dcxException(TEXT("Unknown menu \"%\": see /xpopup -c command"), tsMenu);

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
			throw Dcx::dcxException("Invalid Arguments");

		const auto tsMenu(d.gettok(1));

		if ((tsMenu == TEXT("mirc")) || (tsMenu == TEXT("mircbar")))
			throw Dcx::dcxException("Invalid menu name : mirc or mircbar menus don't have access to this feature.");

		const auto p_Menu = Dcx::XPopups.getMenuByName(tsMenu, false);

		if (p_Menu == nullptr)
			throw Dcx::dcxException(TEXT("Unknown menu \"%\": see /xpopup -c command"), tsMenu);

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
			throw Dcx::dcxException("Invalid Arguments");

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
			throw Dcx::dcxException("Invalid Arguments");

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
	mIRCLinker::echo(TEXT("/xmenubar -[switch] (options)"));
	mIRCLinker::echo(TEXT("[switch] = M,a,d,g,l,r, or s"));
	mIRCLinker::echo(TEXT("(options) = depends on switch used"));
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
	mIRCLinker::echo(TEXT("$!xmenubar([options]).prop"));
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
	mIRCLinker::echo(TEXT("/mpopup [MENU] [1|0]"));
	mIRCLinker::echo(TEXT("[MENU] = mirc or mircbar"));
	mIRCLinker::echo(TEXT("[1|0] = Enable/Disable"));
	return 0;
}

// /dcx xSignal [1|0] (+FLAGS)
mIRC(xSignal) {
	TString d(data);

	data[0] = 0;

	try {
		// determine state
		const auto bState = (d.getfirsttok(1).to_int() > 0);
		auto flags(d.trim().getnexttok());

		// if no flags specified, set all states
		if (flags.empty())
			flags = TEXT("+dst");

		const XSwitchFlags xflags(flags);

		if (!xflags[TEXT('+')])
			throw Dcx::dcxException("flags must start with a '+'");

		if (xflags[TEXT('d')])
			dcxSignal.xdock = bState;
		else if (xflags[TEXT('s')])
			dcxSignal.xstatusbar = bState;
		else if (xflags[TEXT('t')])
			dcxSignal.xtray = bState;
		else
			throw Dcx::dcxException("Unknown flag specified.");

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
	mIRCLinker::echo(TEXT("/dcx xSignal [1|0] (+FLAGS)"));
	mIRCLinker::echo(TEXT("[1|0] = Enable/Disable signals specified in (+FLAGS)"));
	mIRCLinker::echo(TEXT("(+FLAGS) = Any combination of d = xdock,s = xstatusbar,t = xtray (no flags == all flags)"));
	return 0;
}

// /dcx WindowProps [HWND] [+FLAGS] (ARGS)
mIRC(WindowProps) {
	const TString input(data);

	data[0] = TEXT('\0');

	try {
		const auto numtok = input.numtok();

		if (numtok < 2)
			throw Dcx::dcxException("Insuffient parameters");

		auto hwnd = (HWND)input.getfirsttok(1).to_int();	// tok 1

		if (!IsWindow(hwnd))
			throw Dcx::dcxException("Invalid window");

		const auto flags(input.getnexttok().trim());	// tok 2
		const XSwitchFlags xflags(flags);

		if (!xflags[TEXT('+')] || (flags.len() < 2))
			throw Dcx::dcxException("No Flags Found");

		if (!xflags[TEXT('T')] && !xflags[TEXT('i')] && !xflags[TEXT('t')] && !xflags[TEXT('r')] && !xflags[TEXT('v')])
			throw Dcx::dcxException("Unknown Flags");

		// set hwnd NoTheme
		// +T
		if (xflags[TEXT('T')]) {
			if (Dcx::UXModule.isUseable()) {
				if (Dcx::UXModule.dcxSetWindowTheme(hwnd, TEXT(" "), TEXT(" ")) != S_OK)
					throw Dcx::dcxException("Unable to set theme");
			}
		}
		// set hwnd's title icon
		// +i [INDEX] [FILENAME]
		if (xflags[TEXT('i')]) {
			// invalid args
			if (numtok < 3)
				throw Dcx::dcxException("Invalid Args");

			const auto index = input.getnexttok().to_int();	// tok 3
			//auto filename(input.gettok(1, TSTAB).gettok(4, -1).trim());
			auto filename(input.getlasttoks().trim());

			ChangeHwndIcon(hwnd, flags, index, filename);
		}
		// set hwnd title text
		// +t [TEXT]
		else if (xflags[TEXT('t')]) {
			TString txt;

			if (xflags[TEXT('i')]) {
				if (input.numtok(TSTABCHAR) > 1)
					txt = input.gettok(2, -1, TSTAB);
			}
			else if (numtok > 2)
				txt = input.getlasttoks();	// tok 3, -1

			SetWindowText(hwnd, txt.trim().to_chr());
		}
		// RMB click hwnd at pos.
		// +r [X] [Y]
		else if (xflags[TEXT('r')]) {
			const auto x = input.getnexttok().to_<UINT>();	// tok 3
			const auto y = input.getnexttok().to_<UINT>();	// tok 4
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
	mIRCLinker::echo(TEXT("/dcx WindowProps [HWND] [+FLAGS] (ARGS)"));
	mIRCLinker::echo(TEXT("[HWND] = HWND of window to alter"));
	mIRCLinker::echo(TEXT("[+FLAGS] = Flags that set what to alter (T = NoTheme, t = Title Text, i = Title Icon, r = RMB click, v = add glass effect)"));
	mIRCLinker::echo(TEXT("(ARGS) = Args dependent on flags selected (see docs)"));
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

		const auto numtok = input.numtok();

		if (numtok < 1)
			throw Dcx::dcxException("Insufficient parameters");

		auto hwnd = GetForegroundWindow();

		if (!IsWindow(hwnd))
			throw Dcx::dcxException("Unable to determine active window");

		const auto prop(input.gettok(1));
		WINDOWINFO wi;

		ZeroMemory(&wi, sizeof(WINDOWINFO));
		if (!GetWindowInfo(hwnd, &wi))
			throw Dcx::dcxException("Unable to get window information");

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
			throw Dcx::dcxException("Invalid parameters");

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
	mIRCLinker::echo(TEXT("$!dcx(ActiveWindow, [property])"));
	mIRCLinker::echo(TEXT("[property] = x,y,w,h,caption,hwnd"));
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
			throw Dcx::dcxException("Invalid parameters");

		// [0-255] enable or (255 == disable) ghost drag for main mIRC window.
		const auto alpha = (BYTE)(input.gettok(1).to_int() & 0xFF);

		if (!Dcx::setGhostDrag(alpha))
			throw Dcx::dcxException("Invalid alpha value");

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
	mIRCLinker::echo(TEXT("/dcx GhostDrag [alpha]"));
	mIRCLinker::echo(TEXT("[alpha] = 0-255 - The level of alpha blend to use."));
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
			throw Dcx::dcxException("Invalid Arguments");

		const auto tsCursor(d.getfirsttok(1));
		auto tsFilename(d.getlasttoks());
		const auto systemCursorId = Dcx::parseSystemCursorType(tsCursor);

		if (!IsFile(tsFilename))
			throw Dcx::dcxException("Unable to open file");

		auto hCursor = (HCURSOR)LoadImage(nullptr, tsFilename.to_chr(), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);

		if (hCursor == nullptr)
			throw Dcx::dcxException("Unable to load cursor");

		if (!SetSystemCursor(hCursor, systemCursorId))
			throw Dcx::dcxException("Unable to set system cursor");

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
	mIRCLinker::echo(TEXT("/dcx SetSystemCursors [cursor id] [filename]"));
	mIRCLinker::echo(TEXT("[cursor id] = appstarting,normal,cross,hand,ibeam,no,sizeall,sizenesw,sizens,sizenwse,sizewe,wait"));
	mIRCLinker::echo(TEXT("[filename] = The path to a .cur or .ani file."));
	return 0;
}

/*!
* \brief XPopup DLL /dcx SetmIRCCursors Function
*
* mIRC /dcx SetmIRCCursors [area id] (filename)
*
* Argument \b data contains -> [area ID,...] (filename)
*/

mIRC(SetmIRCCursors)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.empty())
			throw Dcx::dcxException("Invalid Arguments");

		const auto tsCursor(d.getfirsttok(1));
		auto tsFilename(d.getlasttoks());
		for (auto itStart = tsCursor.begin(TSCOMMA), itEnd = tsCursor.end(); itStart != itEnd; ++itStart)
		{
			const TString tsArea(*itStart);
			const auto AreaId = Dcx::parseAreaType(tsArea);
			if (AreaId > 0)
			{
				if (tsFilename.empty())
				{
					// empty filename == remove custom cursor, if any.
					Dcx::deleteAreaCursor(AreaId);
				}
				else {
					if (!IsFile(tsFilename))
						throw Dcx::dcxException("Unable to open file");

					Dcx::setAreaCursor(Dcx::dcxLoadCursorFromFile(tsFilename), AreaId);
				}
			}
			else {
				const auto CursorId = Dcx::parseCursorType(tsArea);
				if (CursorId == nullptr)
					throw Dcx::dcxException("Unknown cursor type");

				auto hSystemCursor = Dcx::dcxLoadCursorFromResource(CursorId);

				if (tsFilename.empty())
				{
					// empty filename == remove custom cursor, if any.

					Dcx::deleteCursor(hSystemCursor);
				}
				else {
					if (!IsFile(tsFilename))
						throw Dcx::dcxException("Unable to open file");

					Dcx::setCursor(hSystemCursor, Dcx::dcxLoadCursorFromFile(tsFilename));
				}
			}
		}
		return 1;

		//const auto AreaId = Dcx::parseAreaType(tsCursor);
		//
		//if (tsFilename.empty())
		//{
		//	// empty filename == remove custom cursor, if any.
		//	Dcx::deleteCursor(AreaId);
		//}
		//else {
		//	if (!IsFile(tsFilename))
		//		throw Dcx::dcxException("Unable to open file");
		//
		//	auto hCursor = Dcx::dcxLoadCursorFromFile(tsFilename);
		//	Dcx::Cursor_Data cd;
		//	cd.m_bNoDestroy = false;
		//	cd.m_hCursor = hCursor;
		//
		//	Dcx::setCursor(cd, AreaId);
		//}
		//return 1;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/dcx SetmIRCCursors"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/dcx SetmIRCCursors"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	//mIRCLinker::echo(TEXT("/dcx SetmIRCCursors [area(,area2,...)] (filename)"));
	//mIRCLinker::echo(TEXT("[area] = client,nowhere,caption,sysmenu,size,menu,vscroll,help,hscroll,min,max,left,right,top,topleft,topright,bottom,bottomleft,bottomright,border,close"));
	//mIRCLinker::echo(TEXT("(filename) = optional, if empty custom cursor is removed, otherwise the path to a .cur or .ani file."));

	//mIRCLinker::echo(TEXT("/dcx SetmIRCCursors [type(,type2,...)] (filename)"));
	//mIRCLinker::echo(TEXT("[type] = appstarting,arrow,cross,hand,help,ibeam,no,sizeall,sizenesw,sizens,sizenwse,sizewe,uparrow,wait"));
	//mIRCLinker::echo(TEXT("(filename) = optional, if empty custom cursor is removed, otherwise the path to a .cur or .ani file."));

	mIRCLinker::echo(TEXT("/dcx SetmIRCCursors [type(,type2,...)] (filename)"));
	mIRCLinker::echo(TEXT("[type] = appstarting,arrow,cross,hand,help,ibeam,no,sizeall,sizenesw,sizens,sizenwse,sizewe,uparrow,wait,client,caption,sysmenu,size,menu,vscroll,help,hscroll,min,max,left,right,top,topleft,topright,bottom,bottomleft,bottomright,border,close"));
	mIRCLinker::echo(TEXT("(filename) = optional, if empty custom cursor is removed, otherwise the path to a .cur or .ani file."));
	return 0;
}
