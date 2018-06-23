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

// XMenuBar stuff
HMENU g_OriginalMenuBar = nullptr;
//XPopupMenu *g_mIRCScriptMenu = nullptr; // Wrapper for the mIRC scriptable menu.

// XPopup Stuff

SIGNALSWITCH dcxSignal;
/*
	* DllMain()
	* function added to insure that only a single instance of dcx.dll is loaded.
*/

//HANDLE hDcxMutex = nullptr;

namespace {
	HANDLE getMutex() noexcept
	{
		static HANDLE hDcxMutex = nullptr;

		if (hDcxMutex)
			return hDcxMutex;

		TCHAR mutex[128];
		// add pid of mIRC.exe to name so mutex is specific to this instance of mIRC.
		// GetModuleHandle(nullptr) was returning a consistant result.
		_ts_snprintf(&mutex[0], std::extent_v<decltype(mutex)>, TEXT("DCX_LOADED%lx"), GetCurrentProcessId()); // NB: calls user32.dll, is this ok? See warnings in DllMain() docs.
																														   // Enforce only one instance of dcx.dll loaded at a time.
		hDcxMutex = CreateMutex(nullptr, TRUE, &mutex[0]);

		if ((hDcxMutex == nullptr) || (GetLastError() == ERROR_ALREADY_EXISTS))
			return nullptr;

		return hDcxMutex;
	}
}
/*! \brief DllMain function is used to ensure that only one copy of DCX is loaded at a time.
 *
 */
BOOL WINAPI DllMain(
					HINSTANCE hinstDLL,  // handle to DLL module
					DWORD fdwReason,     // reason for calling function
					LPVOID lpReserved ) noexcept  // reserved
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		//TCHAR mutex[128];
		//// Initialize once for each new process.
		//// Return FALSE to fail DLL load.
		//DisableThreadLibraryCalls(hinstDLL);
		//// add pid of mIRC.exe to name so mutex is specific to this instance of mIRC.
		//// GetModuleHandle(nullptr) was returning a consistant result.
		//_ts_snprintf(&mutex[0], gsl::narrow_cast<int>(Dcx::countof(mutex)), TEXT("DCX_LOADED%lx"), GetCurrentProcessId()); // NB: calls user32.dll, is this ok? See warnings in DllMain() docs.
		//
		//// Enforce only one instance of dcx.dll loaded at a time.
		//hDcxMutex = CreateMutex(nullptr, TRUE, &mutex[0]);
		//if (hDcxMutex == nullptr)
		//	return FALSE;
		//else if (GetLastError() == ERROR_ALREADY_EXISTS)
		//{
		//	//ReleaseMutex(hDcxMutex);
		//	//CloseHandle(hDcxMutex);
		//	return FALSE;
		//}

		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		DisableThreadLibraryCalls(hinstDLL);

		if (getMutex() == nullptr)
			return FALSE;
	}
	break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		// should never be called when using DisableThreadLibraryCalls()
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.

		//if (hDcxMutex != nullptr)
		//{
		//	ReleaseMutex(hDcxMutex);
		//	CloseHandle(hDcxMutex);
		//	hDcxMutex = nullptr;
		//}

		if (const HANDLE hMutex = getMutex(); hMutex != nullptr)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
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
_INTEL_DLL_ void WINAPI LoadDll(mIRCLinker::LOADINFO * load)
{
	try {
		Dcx::load(load);

		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Initialising UltraDock..."));
		InitUltraDock();
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("LoadDLL"), TEXT("error: %S"), e.what());
		Dcx::error(TEXT("LoadDLL"), TEXT("error: %"), e.what());
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
_INTEL_DLL_ int WINAPI UnloadDll(int timeout)
{
	// DLL unloaded because mIRC exits or /dll -u used
	if (timeout != 1)
	{
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
#if DCX_DEBUG_OUTPUT
#if __has_include(<ColourString.h>)
#include <ColourString.h>
#endif
mIRC(Version)
{
	try {
		TString tmp(TEXT(" This is a token string "));

		//mIRCLinker::execex(TEXT("/echo -a \" This is a token string \" :: \"%s\""), tmp.to_chr());
		mIRCLinker::echo(TEXT("\" This is a token string \" :: \"%\""), tmp);


		{	// test trim() function
			tmp.trim();
			Ensures(tmp == TEXT("This is a token string"_ts));

			mIRCLinker::echo(TEXT("Success: trim() - all tests passed"));
		}
		
		{
			Dcx::dcxNumber<int> iNum{ -10 };

			int signednum(iNum);
			UINT unsignednum(iNum);

			Ensures(signednum == -10);
			Ensures(unsignednum == (UINT)-10);
			
			signednum = 0;
			unsignednum = 0;

			iNum = -5;

			signednum = iNum;
			unsignednum = iNum;

			Ensures(signednum == -5);
			Ensures(unsignednum == (UINT)-5);

			iNum = 14U;

			signednum = iNum;
			unsignednum = iNum;

			Ensures(signednum == 14);
			Ensures(unsignednum == 14U);
			Ensures(signednum == (int)iNum);
			Ensures(unsignednum == (UINT)iNum);

			iNum = -1;
			signednum = iNum;
			unsignednum = iNum;

			//mIRCLinker::execex(TEXT("/echo -s Success: %u :: %d"), unsignednum, signednum);
			mIRCLinker::echo(TEXT("Success: % :: %"), unsignednum, signednum);
		}

		{	// test refString
			const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> rData(data);
			Ensures(rData.data() != nullptr);
			Ensures(rData.size() == MIRC_BUFFER_SIZE_CCH);

			rData.clear();
			Ensures(rData.empty());

			rData = tmp.to_chr();
			Ensures(!rData.empty());
			Ensures(rData == tmp.to_chr());
			Ensures(rData.length() == tmp.length());

			rData += TEXT("other");
			Ensures(rData != tmp.to_chr());
			Ensures(rData.length() == tmp.length() + 5U);

			rData += TEXT('a');
			Ensures(rData.length() == tmp.length() + 6U);

			mIRCLinker::echo(TEXT("Success: refString - all tests passed"));
		}

		{	// test simpleString
			stString<MIRC_BUFFER_SIZE_CCH> rData(data);
			Ensures(rData.data() != nullptr);
			Ensures(rData.size() == MIRC_BUFFER_SIZE_CCH);

			rData.clear();
			Ensures(rData.empty());

			rData = tmp.to_chr();
			Ensures(!rData.empty());
			Ensures(rData == tmp.to_chr());
			Ensures(rData.length() == tmp.length());

			rData += TEXT("other");
			Ensures(rData != tmp.to_chr());
			Ensures(rData.length() == tmp.length() + 5U);

			rData += TEXT('a');
			Ensures(rData.length() == tmp.length() + 6U);

			mIRCLinker::echo(TEXT("Success: simpleString - all tests passed"));
		}

		{	// test numtok() function
			// numtok()
			Ensures(tmp.numtok() == 5);
			// numtok(" ")
			Ensures(tmp.numtok(TEXT(" ")) == 5);
			// numtok(string)
			Ensures(tmp.numtok(TEXT("is")) == 3);
			// numtok(string)
			Ensures(tmp.numtok(TEXT("token")) == 2);

			mIRCLinker::echo(TEXT("Success: numtok() - all tests passed"));
		}

		{	// test addtok() function
			// addtok(number)
			tmp.addtok(100);	// "This is a token string 100"
			Ensures(tmp == TEXT("This is a token string 100"_ts));

			// addtok(string)
			tmp.addtok(TEXT("chars"));	// "This is a token string 100 chars"
			Ensures(tmp == TEXT("This is a token string 100 chars"_ts));

			// addtok(character)
			tmp.addtok(TEXT('q'));	// "This is a token string 100 chars q"
			Ensures(tmp == TEXT("This is a token string 100 chars q"_ts));

			// addtok(character,character)
			tmp.addtok(TEXT('b'), TEXT('n'));
			Ensures(tmp == TEXT("This is a token string 100 chars qnb"_ts));

			tmp.addtok(0);	// "This is a token string 100 chars qnb 0"
			Ensures(tmp == TEXT("This is a token string 100 chars qnb 0"_ts));

			tmp.addtok(L"");	// "This is a token string 100 chars qnb 0 "
			Ensures(tmp == TEXT("This is a token string 100 chars qnb 0 "_ts));

			// deltok(end of string)
			tmp.deltok(tmp.numtok());	// "This is a token string 100 chars qnb 0"
			Ensures(tmp == TEXT("This is a token string 100 chars qnb 0"_ts));

			mIRCLinker::echo(TEXT("Success: addtok() - all tests passed"));
		}

		{	// test instok()
			// instok(string,middle of token string)
			tmp.instok(TEXT("m_inserted"), 5); // "This is a token m_inserted string 100 chars qnb 0"
			Ensures(tmp == TEXT("This is a token m_inserted string 100 chars qnb 0"_ts));

			// instok(string,start of token string)
			tmp.instok(TEXT("s_inserted"), 1); // "s_inserted This is a token m_inserted string 100 chars qnb 0"
			Ensures(tmp == TEXT("s_inserted This is a token m_inserted string 100 chars qnb 0"_ts));

			// instok(string,end of token string)
			tmp.instok(TEXT("e_inserted"), tmp.numtok() + 1);	// "s_inserted This is a token m_inserted string 100 chars qnb 0 e_inserted"
			Ensures(tmp == TEXT("s_inserted This is a token m_inserted string 100 chars qnb 0 e_inserted"_ts));

			mIRCLinker::echo(TEXT("Success: instok() - all tests passed"));
		}

		{	// test deltok()
			// deltok(middle of token string)
			tmp.deltok(2);	// "s_inserted is a token m_inserted string 100 chars qnb 0 e_inserted"
			Ensures(tmp == TEXT("s_inserted is a token m_inserted string 100 chars qnb 0 e_inserted"_ts));

			// deltok(start of string)
			tmp.deltok(1);	// "is a token m_inserted string 100 chars qnb 0 e_inserted"
			Ensures(tmp == TEXT("is a token m_inserted string 100 chars qnb 0 e_inserted"_ts));

			// deltok(end of string)
			tmp.deltok(tmp.numtok());	// "is a token m_inserted string 100 chars qnb 0"
			Ensures(tmp == TEXT("is a token m_inserted string 100 chars qnb 0"_ts));

			// deltok(end of string)
			tmp.deltok(tmp.numtok());	// "is a token m_inserted string 100 chars qnb"
			Ensures(tmp == TEXT("is a token m_inserted string 100 chars qnb"_ts));

			mIRCLinker::echo(TEXT("Success: deltok() - all tests passed"));
		}

		{	// test puttok()
			// puttok(string,middle of token string)
			tmp.puttok(TEXT("put"), 4);	// "is a token put string 100 chars qnb"
			Ensures(tmp == TEXT("is a token put string 100 chars qnb"_ts));

			// puttok(string,start of token string)
			tmp.puttok(TEXT("putter"), 1);	// "putter a token put string 100 chars qnb"
			Ensures(tmp == TEXT("putter a token put string 100 chars qnb"_ts));

			// puttok(string,last token in string)
			tmp.puttok(TEXT("putted"), tmp.numtok());	// "putter a token put string 100 chars putted"
			Ensures(tmp == TEXT("putter a token put string 100 chars putted"_ts));

			// puttok(string,end of token string)
			tmp.puttok(TEXT("e_put"), tmp.numtok() +1);	// "putter a token put string 100 chars putted e_put"
			Ensures(tmp == TEXT("putter a token put string 100 chars putted e_put"_ts));

			mIRCLinker::echo(TEXT("Success: puttok() - all tests passed"));
		}

		{	// test gettok()
			TString tsTestCrash;

			// crash condition....
			tsTestCrash = tmp.gettok(2);
			tsTestCrash = tmp.gettok(2, -1);
			Ensures(tsTestCrash == TEXT("a token put string 100 chars putted e_put"_ts));

			// gettok(num,same num) get single token from middle of string
			tsTestCrash = tmp.gettok(2, 2);
			Ensures(tsTestCrash == TEXT("a"_ts));

			// gettok(N, N+1) get 2 tokens from middle of string
			tsTestCrash = tmp.gettok(2, 3);
			Ensures(tsTestCrash == TEXT("a token"_ts));

			// gettok(N,numtok()) get all tokens from N onwards, untill very last token
			tsTestCrash = tmp.gettok(2, static_cast<int>(tmp.numtok()));
			Ensures(tsTestCrash == TEXT("a token put string 100 chars putted e_put"_ts));

			// gettok(N,-1) get whole string starting at the Nth token
			tsTestCrash = tmp.gettok(2, -1);
			Ensures(tsTestCrash == TEXT("a token put string 100 chars putted e_put"_ts));

			mIRCLinker::echo(TEXT("Success: gettok() - all tests passed"));
		}

		TString tok;

		{	// test number handling
			auto tsNum(tmp.gettok(6));	// "putter a token put string 100 chars putted e_put" == 100
			auto t = 0;
			Ensures(tsNum == TEXT("100"_ts));

			// append "3.14" to string, floats are always zero padded so becomes 3.140000
			tsNum += 3.14;
			Ensures(tsNum == TEXT("1003.140000"_ts));

			// append a zero from a var
			tsNum += t;
			Ensures(tsNum == TEXT("1003.1400000"_ts));

			// convert string to an integer, causes 1003.1400000 to become 1003
			t = tsNum.to_<int>();
			Ensures(t == 1003);

			// post increment token count, tok is set to first token
			tok = tsNum++;
			Ensures(tok == TEXT("1003.1400000"_ts));

			// post increment token count, tok is set to second (non-existant) token, so returns blank
			tok = tsNum++;
			Ensures(tok == TEXT(""_ts));

			// reset tsNum to new contents
			tsNum = TEXT("blah1 blah2 blah3");	// "blah1 blah2 blah3"
			Ensures(tsNum == TEXT("blah1 blah2 blah3"_ts));

			// set tsNum to second token & get it
			tsNum++;
			tok = *tsNum;	// "blah2"
			Ensures(tsNum == TEXT("blah1 blah2 blah3"_ts));
			Ensures(tok == TEXT("blah2"_ts));

			mIRCLinker::echo(TEXT("Success: number handling - all tests passed"));
		}

		{	// test iterator's
			//for (auto itStart = tmp.begin(), itEnd = tmp.end(); itStart != itEnd; ++itStart)
			//{
			//	mIRCLinker::execex(TEXT("/echo -a test for: %s"), (*itStart).to_chr());
			//}

			const auto nToks = tmp.numtok();
			auto nIter = decltype(nToks){0};

			// begin()/end() based loop
			auto itStart = tmp.begin(), itEnd = tmp.end();
			while (itStart != itEnd)
			{
				const TString tsTmp(*itStart);
				++nIter;
				Ensures(nIter == (size_t)itStart);
				++itStart;
			}
			Ensures((nToks == nIter));
			Ensures((nToks == tmp.numtok()));

			nIter = 0U;

			// ranged for loop
			for (auto x : tmp)
			{
				++nIter;
			}
			Ensures((nToks == nIter));
			Ensures((nToks == tmp.numtok()));

			mIRCLinker::echo(TEXT("Success: iterator - all tests passed"));
		}

		{	// test join()
			tok.join(tmp, TEXT('!'));
			Ensures(tok == TEXT("blah2!putter!a!token!put!string!100!chars!putted!e_put"_ts));

			mIRCLinker::echo(TEXT("Success: join() - all tests passed"));
		}

		{	// test parse_string()
			const auto t = Dcx::parse_string<int>("200");
			Ensures(t == 200);

			mIRCLinker::echo(TEXT("Success: parse_string() - all tests passed"));
		}

#if __has_include(<ColourString.h>)
		{
			const TCHAR ctrlk = TEXT('\u0003');
			std::basic_string<TCHAR> str(TEXT("test "));
			str += ctrlk;
			str += TEXT("04red");
			str += ctrlk;
			str += TEXT("\u0002 bold\u0002 ");	// space between \x02 & bold required, as it sees \x02 as \x02b otherwise
			str += TEXT(" \x1Funderline\x1F ");
			str += TEXT(" \x1Ditalic\x1D ");
			str += TEXT(" \x16reverse\x16 ");
			str += TEXT("\xF end");
			Ensures(str == L"test \u000304red\u0003\u0002 bold\u0002  \x1Funderline\x1F  \x1Ditalic\x1D  \x16reverse\x16 \xF end");

			ColourString<TCHAR> cc(str);
			Ensures(cc == str);

			ColourString<TCHAR> cc2(str);
			Ensures(cc == cc2);

			cc2 += TEXT(" some extra text");
			str += TEXT(" some extra text");
			//Ensures(cc2 == str);

			Ensures(cc != cc2);

			ColourString<TCHAR> cc3{ cc, TEXT(" "), cc2 };
			//Ensures(cc3 == L"test \u000304red\u0003\u0002 bold\u0002  \x1Funderline\x1F  \x1Ditalic\x1D  \x16reverse\x16 \xF end test \u000304red\u0003\u0002 bold\u0002  \x1Funderline\x1F  \x1Ditalic\x1D  \x16reverse\x16 \xF end some extra text");

			ColourString<char> cc4("test here");
			Ensures(cc4 == "test here");

			Ensures(cc2 != cc3);
			//Ensures(cc4 != cc2);

			//mIRCLinker::execex(TEXT("/echo -a test7: rtf -> %s"), cc2.ToRtf().c_str());

			//tsprintf(tok, TEXT("convert%, %, %, %, %, %"), 6, tsNum, t, tmp, str.c_str(), cc.ToString().c_str());
			//mIRCLinker::execex(TEXT("/echo -a convert6: %s"), tok.to_chr());
			//tsprintf(str, TEXT("convert%, %, %, %, %"), std::to_wstring(7), tsNum.to_chr(), std::to_wstring(t), tmp.to_chr(), cc.ToString().c_str());
			//mIRCLinker::execex(TEXT("/echo -a convert7: %s"), str.c_str());
			//tok.clear();
			//tsprintf(tok, "convert%, %, %, %, %, %", 8, tsNum, t, tmp, str.c_str(), cc.ToString().c_str());
			//mIRCLinker::execex(TEXT("/echo -a convert8: %s"), tok.to_chr());

			mIRCLinker::echo(TEXT("Success: ColourString - all tests passed"));
		}
#endif

		{	// test std::to_string() overload for std::wstring -> std::string
			std::wstring str(TEXT("test"));
			auto str2 = std::to_string(str);
			Ensures(str2 == "test");

			mIRCLinker::echo(TEXT("Success: to_string() - all tests passed"));
		}

		{	// test replace()
			tok.replace(TEXT("token"), TEXT("freddy"));
			Ensures(tok == TEXT("blah2!putter!a!freddy!put!string!100!chars!putted!e_put"_ts));

			mIRCLinker::echo(TEXT("Success: replace() - all tests passed"));
		}

		//{
		//	tok = _ts_strstr("this is a sample text with some extra added", "sample");
		//	mIRCLinker::execex(TEXT("/echo -a _ts_strstr: %s"), tok.to_chr());
		//}

		{	// test istok() - "blah2!putter!a!freddy!put!string!100!chars!putted!e_put"
			// istok(string,character) - string NOT present
			bool bRes(tok.istok(TEXT("m_inserted"), TEXT('!')));
			Ensures(bRes == false);

			// istok(string,character) - string present
			bRes = tok.istok(TEXT("string"), TEXT('!'));
			Ensures(bRes == true);

			// istok(string,string) - string present
			bRes = tok.istok(TEXT("ter!a!freddy!"), TEXT("put"));
			Ensures(bRes == true);

			// istok(string,string) - string NOT present
			bRes = tok.istok(TEXT("ter!a!token!"), TEXT("put"));
			Ensures(bRes == false);

			mIRCLinker::echo(TEXT("Success: istok() - all tests passed"));
		}

		{	// test reptok() - "blah2!putter!a!freddy!put!string!100!chars!putted!e_put"
			tok.reptok(TEXT("string"), TEXT("blobby"), 1, TEXT('!'));
			Ensures(tok == TEXT("blah2!putter!a!freddy!put!blobby!100!chars!putted!e_put"_ts));

			tok.reptok(TEXT("e_put"), TEXT("blobby"), 1, TEXT('!'));
			Ensures(tok == TEXT("blah2!putter!a!freddy!put!blobby!100!chars!putted!blobby"_ts));

			tok.reptok(TEXT("blobby"), TEXT("mister"), 2, TEXT('!'));
			Ensures(tok == TEXT("blah2!putter!a!freddy!put!blobby!100!chars!putted!mister"_ts));

			mIRCLinker::echo(TEXT("Success: reptok() - all tests passed"));
		}

		{	// test sorttok() - "blah2!putter!a!freddy!put!blobby!100!chars!putted!mister"
			tok += TEXT("!101!102!200!4!5");
			Ensures(tok == TEXT("blah2!putter!a!freddy!put!blobby!100!chars!putted!mister!101!102!200!4!5"_ts));

			tok.sorttok(TEXT(""), TEXT("!"));	// alpha sort
			Ensures(tok == TEXT("100!101!102!200!4!5!a!blah2!blobby!chars!freddy!mister!put!putted!putter"_ts));

			tok.sorttok(TEXT("r"), TEXT("!"));	// reverse alpha sort
			Ensures(tok == TEXT("putter!putted!put!mister!freddy!chars!blobby!blah2!a!5!4!200!102!101!100"_ts));

			tok.sorttok(TEXT("a"), TEXT("!"));	// alphanumeric sort
			Ensures(tok == TEXT("4!5!100!101!102!200!a!blah2!blobby!chars!freddy!mister!put!putted!putter"_ts));

			tok.sorttok(TEXT("ar"), TEXT("!"));	// reverse alphanumeric
			Ensures(tok == TEXT("putter!putted!put!mister!freddy!chars!blobby!blah2!a!200!102!101!100!5!4"_ts));

			tok.sorttok(TEXT("n"), TEXT("!"));	// numeric sort
			Ensures(tok == TEXT("putter!putted!put!mister!freddy!chars!blobby!blah2!a!4!5!100!101!102!200"_ts));

			tok.sorttok(TEXT("nr"), TEXT("!"));	// reverse numeric sort
			Ensures(tok == TEXT("200!102!101!100!5!4!putter!putted!put!mister!freddy!chars!blobby!blah2!a"_ts));

			tok.sorttok(TEXT("c"), TEXT("!"));	// channel prefix sort
			Ensures(tok == TEXT("102!101!100!200!4!5!a!blobby!blah2!chars!freddy!mister!putter!putted!put"_ts));

			tok.sorttok(TEXT("cr"), TEXT("!"));	// reverse channel prefix sort
			Ensures(tok == TEXT("putter!putted!put!mister!freddy!chars!blobby!blah2!a!5!4!200!102!101!100"_ts));

			mIRCLinker::echo(TEXT("Success: sorttok() - all tests passed"));
		}

		{	// test wildtok()/nwildtok() - "putter!putted!put!mister!freddy!chars!blobby!blah2!a!5!4!200!102!101!100"
			Ensures(tok.wildtok(TEXT("put*"), 2, TEXT("!")) == TEXT("putted"_ts));
			Ensures(tok.nwildtok(TEXT("put*"), TEXT("!")) == 3);

			mIRCLinker::echo(TEXT("Success: wildtok() - all tests passed"));
		}

		{	// test matchtok() - "putter!putted!put!mister!freddy!chars!blobby!blah2!a!5!4!200!102!101!100"
			Ensures(tok.matchtok(TEXT("put"), 1, TEXT("!")) == TEXT("putter"_ts));
			Ensures(tok.matchtok(TEXT("put"), 3, TEXT("!")) == TEXT("put"_ts));

			mIRCLinker::echo(TEXT("Success: matchtok() - all tests passed"));
		}

		{	// test iswm()/iswmcs() - "putter!putted!put!mister!freddy!chars!blobby!blah2!a!5!4!200!102!101!100"
			Ensures(tok.iswm(TEXT("*put*")));
			Ensures(tok.iswmcs(TEXT("*put*")));
			Ensures(!tok.iswmcs(TEXT("*PUT*")));
			mIRCLinker::echo(TEXT("Success: iswm(cs)() - all tests passed"));
		}

		{
			// test constructors
			const TCHAR *ctmp = TEXT("test");
			auto test1 = TString(TEXT("test"));
			Ensures(test1 == TEXT("test"_ts));

			auto test2 = L"test"_ts;
			Ensures(test2 == TEXT("test"_ts));

			auto test3 = TString("test");
			Ensures(test3 == TEXT("test"_ts));

			auto test4 = TString(test1);
			Ensures(test4 == TEXT("test"_ts));

			auto test5 = "test"_ts;
			Ensures(test5 == TEXT("test"_ts));

			auto test6 = TString(ctmp);
			Ensures(test6 == TEXT("test"_ts));

			Ensures(_ts_isEmpty(test6) == false);

			Ensures(_ts_isEmpty(ctmp) == false);

			mIRCLinker::echo(TEXT("Success: Constructors - all tests passed"));
		}

		{	// test getfirsttok()/getnexttok() - "putter!putted!put!mister!freddy!chars!blobby!blah2!a!5!4!200!102!101!100"
			auto toktest = tok.gettok(2, TEXT('!'));
			Ensures(toktest == TEXT("putted"_ts));

			toktest = tok.gettok(3, -1, TEXT('!'));
			Ensures(toktest == TEXT("put!mister!freddy!chars!blobby!blah2!a!5!4!200!102!101!100"_ts));

			toktest = tok.getfirsttok(1, TEXT('!'));
			Ensures(toktest == TEXT("putter"_ts));

			toktest = tok.getnexttok(TEXT('!'));
			Ensures(toktest == TEXT("putted"_ts));

			mIRCLinker::echo(TEXT("Success: getfirsttok()/getnexttok() - all tests passed"));
		}

		{	// test _ts_strlen()
			auto itest = _ts_strlen(TEXT("12"));
			Ensures(itest == 2);

			itest = _ts_strlen(TEXT('2'));
			Ensures(itest == 1);

			itest = _ts_strlen(tok);
			Ensures(itest == 72);

			mIRCLinker::echo(TEXT("Success: _ts_strlen() - all tests passed"));
		}


		//{	// test range iteration
		//	auto rng = Dcx::range(10U, 20U);
		//	for (auto rItStart = rng.begin(), rItEnd = rng.end(); rItStart != rItEnd; ++rItStart)
		//	{
		//		mIRCLinker::execex(TEXT("/echo -a range: %u"), *rItStart);
		//	}
		//}

		{	// test expanding a TString object beyond the internal buffer
			TString tsExp(TEXT("expanding text"));	// len = 14
			Ensures(tsExp.len() == 14);

			tsExp *= 20;							// len = 14*20 = 280
			Ensures(tsExp.len() == 280);

			mIRCLinker::echo(TEXT("Success: internal buffer overflow - all tests passed"));
		}

		{	// test remove()
			tok.remove(TEXT('!'));
			Ensures(tok == TEXT("putterputtedputmisterfreddycharsblobbyblah2a54200102101100"_ts));

			mIRCLinker::echo(TEXT("Success: remove() - all tests passed"));
		}

		{	// test _ts_strcpyn()
			const char chr_test[128] = { 0 };
			char chr_buf[128] = { 0 };

			ts_strcpyn(&chr_buf[0], &chr_test[0], std::extent_v<decltype(chr_buf)>);

			Ensures(ts_strcmp(&chr_test[0], &chr_buf[0]) == 0);

			mIRCLinker::echo(TEXT("Success: _ts_strcpyn() - all tests passed"));
		}

		{	// test numeric_cast()
			tok = TEXT("123");
			UINT iTest = Dcx::numeric_cast<UINT>(tok);
			Ensures(iTest == 123);
			
			iTest = Dcx::numeric_cast<UINT>("400");
			Ensures(iTest == 400);

			const auto fTest = Dcx::numeric_cast<float>("3.14");
			Ensures(fTest == float(3.14));

			const auto dTest = Dcx::numeric_cast<double>("3.14");
			Ensures(dTest == double(3.14));

			mIRCLinker::echo(TEXT("Success: numeric_cast() - all tests passed"));
		}

		// hashes should be the same...
		{	// test string hashes
			constexpr const auto hash1 = const_hash("123");
			const auto hash2 = dcx_hash("123");
			constexpr const auto hash3 = "123"_crc32;
			constexpr const auto hash4 = "123"_hash;
			constexpr const auto hash5 = L"123"_hash;
			const auto hash6 = dcx_hash(L"123");

			static_assert(hash3 == hash4, "hash 3, & 4 failed");
			Ensures(hash2 == hash3);
			Ensures(hash2 == 0x884863D2);

			Ensures(hash6 == hash5);
			Ensures(hash6 == 0xD0AD20B1);

			static_assert(hash1 == 0xAF0A1B9E, "hash 1 failed");

			mIRCLinker::echo(TEXT("Success: string hashing - all tests passed"));
		}
		mIRCLinker::echo(TEXT("Success: RGB % lightens to %"), RGB(184, 199, 146), XPopupMenuItem::LightenColor(200, RGB(184, 199, 146)));

		throw Dcx::dcxException(TEXT("No such Exception"));
		//throw Dcx::dcxException("No such Exception: % :: %", iTest, tok);
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("Version"), TEXT("error: %S"), e.what());
		Dcx::error(TEXT("Version"), TEXT("error: %"), e.what());
	}

#ifdef DCX_DEV_BUILD
	if (mIRCLinker::isUnicode())
	{
		_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH,
			TEXT("DCX (XPopup) DLL %s %s%d UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2005 Compiled on " __DATE__ " " __TIME__),
			DLL_VERSION, DLL_STATE, DLL_DEV_BUILD);
	}
	else {
		_ts_snprintf((char *)data, MIRC_BUFFER_SIZE_CCH,
			"DCX (XPopup) DLL %S %S%d UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2005 Compiled on " __DATE__ " " __TIME__,
			DLL_VERSION, DLL_STATE, DLL_DEV_BUILD);
	}
#else
	if (mIRCLinker::isUnicode())
	{
		_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH,
			TEXT("DCX (XPopup) DLL %s %s UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2005 Compiled on " __DATE__ " " __TIME__),
			DLL_VERSION, DLL_STATE);
	}
	else {
		_ts_snprintf((char *)data, MIRC_BUFFER_SIZE_CCH,
			"DCX (XPopup) DLL %S %S UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2005 Compiled on " __DATE__ " " __TIME__,
			DLL_VERSION, DLL_STATE);
	}
#endif
	return 3;
}
#else
mIRC(Version)
{
#ifdef DCX_DEV_BUILD
	if (mIRCLinker::isUnicode())
	{
		_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH,
			TEXT("DCX (XPopup) DLL %s %s%d UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2005 Compiled on " __DATE__ " " __TIME__),
			DLL_VERSION, DLL_STATE, DLL_DEV_BUILD);
	}
	else {
		_ts_snprintf((char *)data, MIRC_BUFFER_SIZE_CCH,
			"DCX (XPopup) DLL %S %S%d UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2005 Compiled on " __DATE__ " " __TIME__,
			DLL_VERSION, DLL_STATE, DLL_DEV_BUILD);
	}
#else
	if (mIRCLinker::isUnicode())
	{
		_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH,
			TEXT("DCX (XPopup) DLL %s %s UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2005 Compiled on " __DATE__ " " __TIME__),
			DLL_VERSION, DLL_STATE);
	}
	else {
		_ts_snprintf((char *)data, MIRC_BUFFER_SIZE_CCH,
			"DCX (XPopup) DLL %S %S UTF by ClickHeRe, twig*, Ook, andy and Mpdreamz  ©2005 Compiled on " __DATE__ " " __TIME__,
			DLL_VERSION, DLL_STATE);
	}
#endif
	return 3;
}
#endif

/*!
* \brief DCX DLL is DcxDirectShow supported?
*/
mIRC(IsUsingDirectX)
{
	static TString dxData;
	if (Dcx::isDX9Installed())
	{
		ret(dxData.to_chr());
	}
	else if (Dcx::initDirectX(data, MIRC_BUFFER_SIZE_CCH))
	{
		dxData = data;
		return 3;
	}
	ret(TEXT("$false"));
}

/*!
* \brief DCX DLL is GDI+ supported?
*/
mIRC(IsUsingGDI) noexcept
{
	//ret((Dcx::GDIModule.isUseable() ? TEXT("$true") : TEXT("$false")));
	ret(dcx_truefalse(Dcx::GDIModule.isUseable()));
}

/*!
* \brief Check if it's safe to unload DLL
*/
mIRC(IsUnloadSafe) noexcept
{
	//ret((Dcx::isUnloadSafe() ? TEXT("$true") : TEXT("$false")));
	ret(dcx_truefalse(Dcx::isUnloadSafe()));
}

/*!
* \brief Check if windows is themed
*/
mIRC(IsThemedXP) noexcept
{
	//ret((Dcx::UXModule.dcxIsThemeActive() ? TEXT("$true") : TEXT("$false")));
	ret(dcx_truefalse(Dcx::UXModule.dcxIsThemeActive() != FALSE));
}

/*!
* \brief DCX DLL Mark Function
*
* Argument \b data contains -> [NAME] [ALIAS]
*/

// Mark [NAME] [ALIAS]
mIRC(Mark)
{
	__assume(data != nullptr);

	TString d(data);

	data[0] = 0;

	const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> szReturnValue(data);

	try {
		d.trim();

		if (d.numtok() < 2)
			throw Dcx::dcxException("[NAME] [ALIAS]");

		const TString tsDialog(d.getfirsttok(1));
		const TString tsCallback(d.getnexttok());
		Dcx::mark(tsDialog, tsCallback);

		szReturnValue = TEXT("D_OK Mark: Dialog Marked");
		return 3;
	}
	catch (const std::exception &e)
	{
		Dcx::error(TEXT("/dcx Mark"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("/dcx Mark"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("$!dcx(Mark,[name],[alias])"));
	mIRCLinker::echo(TEXT("/dcx Mark [name] [alias]"));
	mIRCLinker::echo(TEXT("[name] = the name of the dialog to mark"));
	mIRCLinker::echo(TEXT("[alias] = the callback alias to use."));
	return 0;
}

/*!
* \brief DCX DLL GetSystemColor Function
*
* Argument \b data contains -> [ATTRIB] (not implemented yet, [DEFAULT])
*/
// GetSystemColor [ATTRIBUTE]
mIRC(GetSystemColor)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 1)
			throw Dcx::dcxException("Invalid Arguments");

		int col = 0;
		switch (std::hash<TString>{}(d.gettok(1)))
		{
		case TEXT("COLOR_3DDKSHADOW"_hash):
			col = COLOR_3DDKSHADOW;
			break;
		case TEXT("COLOR_3DFACE"_hash):
			col = COLOR_3DFACE;
			break;
		case TEXT("COLOR_3DHIGHLIGHT"_hash):
			col = COLOR_3DHIGHLIGHT;
			break;
		case TEXT("COLOR_3DHILIGHT"_hash):
			col = COLOR_3DHILIGHT;
			break;
		case TEXT("COLOR_3DLIGHT"_hash):
			col = COLOR_3DLIGHT;
			break;
		case TEXT("COLOR_3DSHADOW"_hash):
			col = COLOR_3DSHADOW;
			break;
		case TEXT("COLOR_ACTIVEBORDER"_hash):
			col = COLOR_ACTIVEBORDER;
			break;
		case TEXT("COLOR_ACTIVECAPTION"_hash):
			col = COLOR_ACTIVECAPTION;
			break;
		case TEXT("COLOR_APPWORKSPACE"_hash):
			col = COLOR_APPWORKSPACE;
			break;
		case TEXT("COLOR_BACKGROUND"_hash):
			col = COLOR_BACKGROUND;
			break;
		case TEXT("COLOR_BTNFACE"_hash):
			col = COLOR_BTNFACE;
			break;
		case TEXT("COLOR_BTNHIGHLIGHT"_hash):
			col = COLOR_BTNHIGHLIGHT;
			break;
		case TEXT("COLOR_BTNSHADOW"_hash):
			col = COLOR_BTNSHADOW;
			break;
		case TEXT("COLOR_BTNTEXT"_hash):
			col = COLOR_BTNTEXT;
			break;
		case TEXT("COLOR_CAPTIONTEXT"_hash):
			col = COLOR_CAPTIONTEXT;
			break;
		case TEXT("COLOR_DESKTOP"_hash):
			col = COLOR_DESKTOP;
			break;
		case TEXT("COLOR_GRADIENTACTIVECAPTION"_hash):
			col = COLOR_GRADIENTACTIVECAPTION;
			break;
		case TEXT("COLOR_GRADIENTINACTIVECAPTION"_hash):
			col = COLOR_GRADIENTINACTIVECAPTION;
			break;
		case TEXT("COLOR_GRAYTEXT"_hash):
			col = COLOR_GRAYTEXT;
			break;
		case TEXT("COLOR_HIGHLIGHT"_hash):
			col = COLOR_HIGHLIGHT;
			break;
		case TEXT("COLOR_HIGHLIGHTTEXT"_hash):
			col = COLOR_HIGHLIGHTTEXT;
			break;
		case TEXT("COLOR_HOTLIGHT"_hash):
			col = COLOR_HOTLIGHT;
			break;
		case TEXT("COLOR_INACTIVEBORDER"_hash):
			col = COLOR_INACTIVEBORDER;
			break;
		case TEXT("COLOR_INACTIVECAPTION"_hash):
			col = COLOR_INACTIVECAPTION;
			break;
		case TEXT("COLOR_INACTIVECAPTIONTEXT"_hash):
			col = COLOR_INACTIVECAPTIONTEXT;
			break;
		case TEXT("COLOR_INFOBK"_hash):
			col = COLOR_INFOBK;
			break;
		case TEXT("COLOR_INFOTEXT"_hash):
			col = COLOR_INFOTEXT;
			break;
		case TEXT("COLOR_MENU"_hash):
			col = COLOR_MENU;
			break;
		case TEXT("COLOR_MENUHILIGHT"_hash):
			col = COLOR_MENUHILIGHT;
			break;
		case TEXT("COLOR_MENUBAR"_hash):
			col = COLOR_MENUBAR;
			break;
		case TEXT("COLOR_MENUTEXT"_hash):
			col = COLOR_MENUTEXT;
			break;
		case TEXT("COLOR_SCROLLBAR"_hash):
			col = COLOR_SCROLLBAR;
			break;
		case TEXT("COLOR_WINDOW"_hash):
			col = COLOR_WINDOW;
			break;
		case TEXT("COLOR_WINDOWFRAME"_hash):
			col = COLOR_WINDOWFRAME;
			break;
		case TEXT("COLOR_WINDOWTEXT"_hash):
			col = COLOR_WINDOWTEXT;
			break;
		case TEXT("COLOR_GLASS"_hash):
			col = -1;
			break;
		default:
			throw Dcx::dcxException("Invalid parameter specified");
		}

		if (col == -1)
		{
			RGBQUAD clr{};
			if (BOOL bOpaque = FALSE; FAILED(Dcx::VistaModule.dcxDwmGetColorizationColor((DWORD *)&clr, &bOpaque)))
				throw Dcx::dcxException("Unable to get glass colour.");
		
			_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), RGB(clr.rgbRed, clr.rgbGreen, clr.rgbBlue));
		}
		else {
			// max of 8 digits, 9 for null terminator
			_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), GetSysColor(col));
		}
		return 3;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("$!dcx(GetSystemColor)"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("$!dcx(GetSystemColor)"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("$!dcx(GetSystemColor)"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("$!dcx(GetSystemColor)"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("$!dcx(GetSystemColor,[attribute])"));
	mIRCLinker::echo(TEXT("[attribute] = the name of the colour to get. (see docs for list)"));
	return 0;
}


/*!
* \brief DCX DLL /xdid Function
*
* mIRC /xdid -switch dialog ID (options) interface
*
* Argument \b data contains -> [NAME] [ID] [SWITCH] (OPTIONS)
*/
mIRC(xdid)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 3)
			throw Dcx::dcxException("Invalid Arguments");

		const auto tsDname(d.getfirsttok(1));
		const auto *const p_Dialog = Dcx::Dialogs.getDialogByName(tsDname);

		if (p_Dialog == nullptr)
			throw Dcx::dcxException(TEXT("Unknown dialog \"%\": see Mark command"), tsDname);

		const auto IDs(d.getnexttok());			// tok 2
		const auto tsArgs(d.getlasttoks());		// tok 3, -1
		const auto n = IDs.numtok(TSCOMMACHAR);
		const auto HandleIDRange = [=](const TString &tsID) {
			UINT id_start = 0, id_end = 0;
			if (tsID.numtok(TEXT('-')) == 2)
			{
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

			for (auto id = id_start; id <= id_end; ++id)
			{
				const auto p_Control = p_Dialog->getControlByID(id + mIRC_ID_OFFSET);

				if (p_Control == nullptr)
					throw Dcx::dcxException(TEXT("Unable to find control: % (dialog : %)"), id, tsDname);

				TString d2(tsDname);
				d2.addtok(id);
				d2.addtok(tsArgs);

				p_Control->parseCommandRequest(d2);
			}
		};

		// Multiple IDs id,id,id,id-id,id-id
		if (n > 1)
		{
			for (auto itStart = IDs.begin(TSCOMMACHAR), itEnd = IDs.end(); itStart != itEnd; ++itStart)
			{
				//UINT id_start = 0, id_end = 0;
				//const TString tsID(*itStart);
				//if (tsID.numtok(TEXT('-')) == 2)
				//{
				//	id_start = tsID.getfirsttok(1, TEXT('-')).to_<UINT>();
				//	id_end = tsID.getnexttok(TEXT('-')).to_<UINT>();
				//}
				//else
				//	id_start = id_end = p_Dialog->NameToUserID(tsID);
				//
				//if (id_end < id_start)
				//	throw Dcx::dcxException(TEXT("Invalid ID : % (dialog : %)"), id_end, tsDname);
				//if (id_start == 0)
				//	throw Dcx::dcxException(TEXT("Invalid ID : % (dialog : %)"), id_start, tsDname);
				//if (id_end == 0)
				//	throw Dcx::dcxException(TEXT("Invalid ID : % (dialog : %)"), id_end, tsDname);
				//
				//for (auto id = id_start; id <= id_end; ++id)
				//{
				//	const auto p_Control = p_Dialog->getControlByID(id + mIRC_ID_OFFSET);
				//
				//	if (p_Control == nullptr)
				//		throw Dcx::dcxException(TEXT("Unable to find control: % (dialog : %)"), id, tsDname);
				//
				//	TString d2(tsDname);
				//	d2.addtok(id);
				//	d2.addtok(tsArgs);
				//
				//	p_Control->parseCommandRequest(d2);
				//}

				HandleIDRange(*itStart);
			}
		}
		//Single ID or single id-id
		else {
			//UINT id_start = 0, id_end = 0;
			//
			//if (IDs.numtok(TEXT('-')) == 2)
			//{
			//	id_start = IDs.getfirsttok(1, TEXT('-')).to_<UINT>();
			//	id_end = IDs.getnexttok(TEXT('-')).to_<UINT>();
			//}
			//else
			//	id_start = id_end = p_Dialog->NameToUserID(IDs);
			//
			//if (id_end < id_start)
			//	throw Dcx::dcxException(TEXT("Invalid ID : % (dialog : %)"), id_end, tsDname);
			//if (id_start == 0)
			//	throw Dcx::dcxException(TEXT("Invalid ID : % (dialog : %)"), id_start, tsDname);
			//if (id_end == 0)
			//	throw Dcx::dcxException(TEXT("Invalid ID : % (dialog : %)"), id_end, tsDname);
			//
			//for (auto id = id_start; id <= id_end; ++id)
			//{
			//	const auto p_Control = p_Dialog->getControlByID(id + mIRC_ID_OFFSET);
			//
			//	if (p_Control == nullptr)
			//		throw Dcx::dcxException(TEXT("Unable to find control: % (dialog : %)"), id, tsDname);
			//
			//	p_Control->parseCommandRequest(d);
			//}

			HandleIDRange(IDs);
		}
		return 1;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("/xdid"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("/xdid"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/xdid"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("/xdid"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("/xdid -[switch] [dialog] [ID] [options]"));
	mIRCLinker::echo(TEXT("[switch] = depends on control type"));
	mIRCLinker::echo(TEXT("[dialog] = the dialog containing the control"));
	mIRCLinker::echo(TEXT("[ID] = the ID of the control"));
	mIRCLinker::echo(TEXT("[options] = depends on the control type & switch used"));
	return 0;
}

/*!
* \brief DCX DLL $xdid Function
*
* mIRC $xdid(dialog, ID, options).prop interface
*
* Argument \b data contains -> [NAME] [ID] [PROP] (OPTIONS)
*/
mIRC(_xdid)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 3)
			throw Dcx::dcxException("Invalid Arguments");

		const auto tsDname(d.getfirsttok(1));
		const auto tsID(d.getnexttok());

		const auto *const p_Dialog = Dcx::Dialogs.getDialogByName(tsDname);

		if (p_Dialog == nullptr)
			throw Dcx::dcxException(TEXT("Unknown dialog \"%\": see Mark command"), tsDname);

		if (!p_Dialog->isNamedId(tsID))
			throw Dcx::dcxException(TEXT("Invalid ID : % (dialog %)"), tsID, tsDname);

		const auto *const p_Control = p_Dialog->getControlByID(p_Dialog->NameToID(tsID));

		if (p_Control == nullptr)
			throw Dcx::dcxException(TEXT("Unable to find control: % (dialog %)"), tsID, tsDname);

		p_Control->parseInfoRequest(d, refString<TCHAR, MIRC_BUFFER_SIZE_CCH>(data));
		return 3;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("$!xdid"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("$!xdid"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("$!xdid"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("$!xdid"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("$!xdid([dialog],[ID](,options)).prop"));
	mIRCLinker::echo(TEXT("[dialog] = the dialog containing the control"));
	mIRCLinker::echo(TEXT("[ID] = the ID of the control to affect"));
	mIRCLinker::echo(TEXT("(options) = depends on the prop used"));
	mIRCLinker::echo(TEXT("[prop] = depends on the control type"));
	return 0;
}

/*!
* \brief DCX DLL $dcx(GetTaskbarPos) Function
*
* $dcx(GetTaskbarPos) interface
*
*/
mIRC(GetTaskbarPos)
{
	data[0] = 0;

	try {
		const auto hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), nullptr);

		if (!IsWindow(hTaskbar))
			throw Dcx::dcxException("Could not find taskbar");

#if DCX_USE_WRAPPERS
		Dcx::dcxWindowRect rc(hTaskbar);

		_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.Width(), rc.Height());
#else
		RECT rc{};

		if (!GetWindowRect(hTaskbar, &rc))
			throw Dcx::dcxException("Unable to get window rect");

		_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
#endif
		return 3;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("$!dcx(GetTaskbarPos)"), TEXT("error: %S"), e.what());
		Dcx::error(TEXT("$!dcx(GetTaskbarPos)"), TEXT("error: %"), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("$!dcx(GetTaskbarPos)"), TEXT("error: Unknown Exception"));
	}
	mIRCLinker::echo(TEXT("$!dcx(GetTaskBarPos)"));
	mIRCLinker::echo(TEXT("no args - returns the x y w h of the TaskBar"));
	return 0;
}

/*!
* \brief DCX DLL /xdialog Function
*
* mIRC /xdialog -switch dialog (options) interface
*
* Argument \b data contains -> [NAME] [SWITCH] (OPTIONS)
*/
mIRC(xdialog)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		const auto tsDname(d.gettok(1));

		if (d.numtok() < 2)
			throw Dcx::dcxException(TEXT("Invalid arguments ( dialog %)"), tsDname);

		const auto p_Dialog = Dcx::Dialogs.getDialogByName(tsDname);

		if (p_Dialog == nullptr)
			throw Dcx::dcxException(TEXT("Unknown dialog \"%\": see Mark command"), tsDname);

		p_Dialog->parseCommandRequest(d);
		return 1;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("/xdialog"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("/xdialog"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/xdialog"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("/xdialog"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("/xdialog -[switch] [dialog] [options]"));
	mIRCLinker::echo(TEXT("[switch] = a,b,c,d,E,f,g,h,j,l,m,n,P,q,r,R,s,S,t,T,U,V,w,x,z"));
	mIRCLinker::echo(TEXT("[dialog] = the dialog to affect"));
	mIRCLinker::echo(TEXT("[options] = depends on the switch used"));
	return 0;
}

/*!
* \brief DCX DLL $xdialog Function
*
* mIRC $xdialog(dialog, options).prop interface
*
* Argument \b data contains -> [NAME] [SWITCH] (OPTIONS)
*/
mIRC(_xdialog)
{
	TString d(data);

	// reset mIRC data
	data[0] = 0;

	try {
		d.trim();

		const auto tsDname(d.getfirsttok(1));

		if (d.numtok() < 2)
			throw Dcx::dcxException(TEXT("Invalid arguments ( dialog %)"), tsDname);

		const auto *const p_Dialog = Dcx::Dialogs.getDialogByName(tsDname);

		if (p_Dialog == nullptr)
		{
			if (d.getnexttok() != TEXT("ismarked"))
				throw Dcx::dcxException(TEXT("Unknown dialog \"%\": see Mark command"), tsDname);

			ret(TEXT("$false"));
		}

		p_Dialog->parseInfoRequest(d, refString<TCHAR, MIRC_BUFFER_SIZE_CCH>(data));
		return 3;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("$!xdialog"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("$!xdialog"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("$!xdialog"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("$!xdialog"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("$!xdialog([dialog],[options]).prop"));
	mIRCLinker::echo(TEXT("[dialog] = the dialog to affect"));
	mIRCLinker::echo(TEXT("[options] = depends on the prop used"));
	mIRCLinker::echo(TEXT("[prop] = isid,nextid,id,ismenu,ismarked,parent,mouseid,focusid,mouse,key,alias,zlayer,zlayercurrent,visible,glasscolor"));
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
mIRC(xpop)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 3)
			throw Dcx::dcxException("Invalid Arguments");

		const auto tsMenu(d.gettok(1));
		const auto uHash = std::hash<TString>{}(tsMenu);

		if ((uHash == TEXT("mirc"_hash)) || (uHash == TEXT("mircbar"_hash)))
			throw Dcx::dcxException("Invalid menu name : mirc or mircbar menus don't have access to this feature.");

		const auto p_Menu = Dcx::XPopups.getMenuByHash(uHash, false);

		if (p_Menu == nullptr)
			throw Dcx::dcxException(TEXT("Unknown menu \"%\": see /xpopup -c command"), tsMenu);

		p_Menu->parseXPopCommand(d);
		return 1;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("/xpop"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("/xpop"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/xpop"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("/xpop"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("/xpop -[switch] [menu] [path] [tab] [option]"));
	mIRCLinker::echo(TEXT("[switch] = a,c,d,f,i,s,t"));
	mIRCLinker::echo(TEXT("[menu] = name of the menu to affect"));
	mIRCLinker::echo(TEXT("[path] = path to item"));
	mIRCLinker::echo(TEXT("[option] = depends on switch used"));
	return 0;
}

/*!
* \brief XPopup DLL $xpop Function
*
* mIRC $xpop(menu, path, options).prop interface
*
* Argument \b data contains -> [MENU] [PROP] [PATH] [TAB] [OPTION]
*/
mIRC(_xpop)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 3)
			throw Dcx::dcxException("Invalid Arguments");

		const auto tsMenu(d.gettok(1));
		const auto uHash = std::hash<TString>{}(tsMenu);

		if ((uHash == TEXT("mirc"_hash)) || (uHash == TEXT("mircbar"_hash)))
			throw Dcx::dcxException("Invalid menu name : mirc or mircbar menus don't have access to this feature.");

		const auto *const p_Menu = Dcx::XPopups.getMenuByHash(uHash, false);

		if (p_Menu == nullptr)
			throw Dcx::dcxException(TEXT("Unknown menu \"%\": see /xpopup -c command"), tsMenu);

		p_Menu->parseXPopIdentifier(d, mIRCResultString(data));
		return 3;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("$!xpop"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("$!xpop"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("$!xpop"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("$!xpop"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("$!xpop([menu],[path],[options]).prop"));
	mIRCLinker::echo(TEXT("[menu] = name of the menu to get information on"));
	mIRCLinker::echo(TEXT("[path] = path to menu item"));
	mIRCLinker::echo(TEXT("[options] = depends on prop used"));
	mIRCLinker::echo(TEXT("[prop] = num,text,icon,checked,enabled,submenu"));
	return 0;
}

/*!
* \brief XPopup DLL /xpopup Function
*
* mIRC /xpopup -switch menu (options)
*
* Argument \b data contains -> [MENU] [SWITCH] (OPTIONS)
*/
mIRC(xpopup)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 2)
			throw Dcx::dcxException("Invalid Arguments");

		Dcx::XPopups.parseCommand(d);
		return 1;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("/xpopup"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		//Dcx::error(__FUNCTIONW__, TEXT("\"%\" error: %"), d, e.what());
		Dcx::error(TEXT("/xpopup"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/xpopup"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("/xpopup"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("/xpopup -[switch] [menu] (options)"));
	mIRCLinker::echo(TEXT("[switch] = b,c,d,i,j,l,m,M,p,s,t,x, or R"));
	mIRCLinker::echo(TEXT("[menu] = name of the menu to affect"));
	mIRCLinker::echo(TEXT("(options) = depends on switch used"));
	return 0;
}

/*!
* \brief XPopup DLL $xpopup Function
*
* mIRC $xpopup(menu, options).prop interface
*
* Argument \b data contains -> [MENU] [PROP] (OPTIONS)
*/
mIRC(_xpopup)
{
	TString d(data);

	data[0] = 0;

	try {
		//d.trim();
		//
		//if (d.numtok() < 2)
		//	throw Dcx::dcxException("Invalid Arguments");
		//
		//Dcx::XPopups.parseIdentifier(d, mIRCResultString(data));
		//return 3;

		d.trim();

		if (d.numtok() < 2)
			throw Dcx::dcxException("Invalid Arguments");

		mIRCResultString refData(data);

		refData = Dcx::XPopups.parseIdentifier(d);

		return 3;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("$!xpopup"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("$!xpopup"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("$!xpopup"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("$!xpopup"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("$!xpopup([menu],[options]).prop"));
	mIRCLinker::echo(TEXT("[menu] = name of the menu to get information on"));
	mIRCLinker::echo(TEXT("[options] = depends on prop used"));
	mIRCLinker::echo(TEXT("[prop] = ismenu,menuname,menubar,style,exstyle,colors,color,isrounded,alpha,marked"));
	return 0;
}

/*!
* \brief XMenuBar DLL /xmenubar Function
*
* mIRC /xmenubar -switch (options)
*/
mIRC(xmenubar)
{
	TString d(data);

	data[0] = 0;

	try {
		Dcx::XMenubar.parseXMenuBarCommand(d.trim());
		return 1;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("/xmenubar"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("/xmenubar"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/xmenubar"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("/xmenubar"), TEXT("\"%\" error: Unknown Exception"), d);
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
mIRC(_xmenubar)
{
	TString d(data);

	data[0] = 0;

	try {
		Dcx::XMenubar.parseXMenuBarInfo(d.trim(), mIRCResultString(data));
		return 3;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("$!xmenubar"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("$!xmenubar"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("$!xmenubar"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("$!xmenubar"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("$!xmenubar([options]).prop"));
	mIRCLinker::echo(TEXT("[prop] == menu"));
	mIRCLinker::echo(TEXT("[options] == 0 or a number to get the Nth menu name"));
	return 0;
}

/*!
* \brief XPopup DLL /mpopup Function
*
* mIRC /mpopup menuname [0|1]
*
* Argument \b data contains -> [MENU] [0|1]
*/
mIRC(mpopup)
{
	TString d(data);

	data[0] = 0;

	try {
		return Dcx::XPopups.parseMPopup(d.trim());
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("/mpopup"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("/mpopup"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/mpopup"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("/mpopup"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("/mpopup [MENU] [1|0]"));
	mIRCLinker::echo(TEXT("[MENU] = mirc or mircbar"));
	mIRCLinker::echo(TEXT("[1|0] = Enable/Disable"));
	return 0;
}

// /dcx xSignal [1|0] (+FLAGS)
mIRC(xSignal)
{
	TString d(data);

	data[0] = 0;

	try {
		// determine state
		const auto bState = (d.trim().getfirsttok(1).to_int() > 0);
		auto flags(d.getnexttok());

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
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("/dcx xSignal"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("/dcx xSignal"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/dcx xSignal"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("/dcx xSignal"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("/dcx xSignal [1|0] (+FLAGS)"));
	mIRCLinker::echo(TEXT("[1|0] = Enable/Disable signals specified in (+FLAGS)"));
	mIRCLinker::echo(TEXT("(+FLAGS) = Any combination of d = xdock,s = xstatusbar,t = xtray (no flags == all flags)"));
	return 0;
}

// /dcx WindowProps [HWND] [+FLAGS] (ARGS)
mIRC(WindowProps)
{
	const TString input(data);

	data[0] = TEXT('\0');

	try {
		const auto numtok = input.numtok();

		if (numtok < 2)
			throw Dcx::dcxException("Insuffient parameters");

		auto hwnd = (HWND)input.getfirsttok(1).to_<DWORD>();	// tok 1

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
		if (xflags[TEXT('T')])
		{
			if (Dcx::UXModule.isUseable())
			{
				if (Dcx::UXModule.dcxSetWindowTheme(hwnd, TEXT(" "), TEXT(" ")) != S_OK)
					throw Dcx::dcxException("Unable to set theme");
			}
		}
		// set hwnd's title icon
		// +i [INDEX] [FILENAME]
		if (xflags[TEXT('i')])
		{
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
		else if (xflags[TEXT('t')])
		{
			TString txt;

			if (xflags[TEXT('i')])
			{
				if (input.numtok(TSTABCHAR) > 1)
					txt = input.gettok(2, -1, TSTABCHAR);
			}
			else if (numtok > 2)
				txt = input.getlasttoks();	// tok 3, -1

			SetWindowText(hwnd, txt.trim().to_chr());
		}
		// RMB click hwnd at pos.
		// +r [X] [Y]
		else if (xflags[TEXT('r')])
		{
			const auto x = input.getnexttok().to_<UINT>();	// tok 3
			const auto y = input.getnexttok().to_<UINT>();	// tok 4
			const LPARAM parm = MAKELONG(x, y);
			SendMessage(hwnd, WM_RBUTTONDOWN, MK_RBUTTON, parm);
			PostMessage(hwnd, WM_RBUTTONUP, MK_RBUTTON, parm); // MUST be a PostMessage or the dll hangs untill the menu is closed.
		}
		// Add Vista+ glass effect to window.
		// +v [top] [left] [bottom] [right]
		else if (xflags[TEXT('v')])
		{
			MARGINS margin;
			margin.cyTopHeight = input.getnexttok().to_<int>();		// tok 3
			margin.cxLeftWidth = input.getnexttok().to_<int>();		// tok 4
			margin.cyBottomHeight = input.getnexttok().to_<int>();	// tok 5
			margin.cxRightWidth = input.getnexttok().to_<int>();	// tok 6
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
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("/dcx WindowProps"), TEXT("\"%s\" error: %S"), input.to_chr(), e.what());
		Dcx::error(TEXT("/dcx WindowProps"), TEXT("\"%\" error: %"), input, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/dcx WindowProps"), TEXT("\"%s\" error: Unknown Exception"), input.to_chr());
		Dcx::error(TEXT("/dcx WindowProps"), TEXT("\"%\" error: Unknown Exception"), input);
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
mIRC(ActiveWindow)
{
	TString input(data);

	data[0] = 0;

	try {
		input.trim();

		const auto numtok = input.numtok();

		if (numtok < 1)
			throw Dcx::dcxException("Insufficient parameters");

		const auto hwnd = GetForegroundWindow();

		if (!IsWindow(hwnd))
			throw Dcx::dcxException("Unable to determine active window");

		WINDOWINFO wi{};
		wi.cbSize = sizeof(WINDOWINFO);

		if (!GetWindowInfo(hwnd, &wi))
			throw Dcx::dcxException("Unable to get window information");

		switch (std::hash<TString>{}(input.gettok(1)))
		{
		case TEXT("hwnd"_hash):		// handle
			_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), (DWORD)hwnd);	// don't use %p as this gives a hex result.
			break;
		case TEXT("x"_hash):		// left
			_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), wi.rcWindow.left);
			break;
		case TEXT("y"_hash):		// top
			_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), wi.rcWindow.top);
			break;
		case TEXT("w"_hash):		// width
			_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), wi.rcWindow.right - wi.rcWindow.left);
			break;
		case TEXT("h"_hash):		// height
			_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), wi.rcWindow.bottom - wi.rcWindow.top);
			break;
		case TEXT("caption"_hash):	// title text
			GetWindowText(hwnd, data, MIRC_BUFFER_SIZE_CCH);
			break;
		default:					// otherwise
			throw Dcx::dcxException("Invalid parameters");
		}
		return 3;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("$!dcx(ActiveWindow)"), TEXT("\"%s\" error: %S"), input.to_chr(), e.what());
		Dcx::error(TEXT("$!dcx(ActiveWindow)"), TEXT("\"%\" error: %"), input, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("$!dcx(ActiveWindow)"), TEXT("\"%s\" error: Unknown Exception"), input.to_chr());
		Dcx::error(TEXT("$!dcx(ActiveWindow)"), TEXT("\"%\" error: Unknown Exception"), input);
	}
	mIRCLinker::echo(TEXT("$!dcx(ActiveWindow, [property])"));
	mIRCLinker::echo(TEXT("[property] = x,y,w,h,caption,hwnd"));
	return 0;
}

/*! \brief /dcx GhostDrag 0-255
 *
 */
mIRC(GhostDrag)
{
	TString input(data);

	data[0] = 0;

	try {
		input.trim();

		if (input.empty())
			throw Dcx::dcxException("Invalid parameters");

		// [0-255] enable or (255 == disable) ghost drag for main mIRC window.
		const auto alpha = static_cast<std::byte>(input.gettok(1).to_<UINT>() & 0xFF);

		if (!Dcx::setGhostDrag(alpha))
			throw Dcx::dcxException("Invalid alpha value");

		return 1;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("/dcx GhostDrag"), TEXT("\"%s\" error: %S"), input.to_chr(), e.what());
		Dcx::error(TEXT("/dcx GhostDrag"), TEXT("\"%\" error: %"), input, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/dcx GhostDrag"), TEXT("\"%s\" error: Unknown Exception"), input.to_chr());
		Dcx::error(TEXT("/dcx GhostDrag"), TEXT("\"%\" error: Unknown Exception"), input);
	}
	mIRCLinker::echo(TEXT("/dcx GhostDrag [alpha]"));
	mIRCLinker::echo(TEXT("[alpha] = 0-255 - The level of alpha blend to use."));
	return 0;
}

/*!
* \brief DCX DLL /dcx SetSystemCursors Function
*
* mIRC /dcx SetSystemCursors [cursor id] (filename)
*
* Argument \b data contains -> [CURSOR ID] (filename)
*/
mIRC(SetSystemCursors)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.empty())
			throw Dcx::dcxException("Invalid Arguments");

		//const auto tsCursor(d.getfirsttok(1));
		//auto tsFilename(d.getlasttoks());
		//const auto systemCursorId = Dcx::parseSystemCursorType(tsCursor);

		const auto systemCursorId = Dcx::parseSystemCursorType(std::hash<TString>{}(d.getfirsttok(1)));
		auto tsFilename(d.getlasttoks());

		if (!IsFile(tsFilename))
			throw Dcx::dcxException("Unable to open file");

		auto hCursor = (HCURSOR)LoadImage(nullptr, tsFilename.to_chr(), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);

		if (hCursor == nullptr)
			throw Dcx::dcxException("Unable to load cursor");

		if (!SetSystemCursor(hCursor, systemCursorId))
			throw Dcx::dcxException("Unable to set system cursor");

		return 1;
	}
	catch (const std::exception &e)
	{
		Dcx::error(TEXT("/dcx SetSystemCursors"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/dcx SetSystemCursors"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("/dcx SetSystemCursors"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	mIRCLinker::echo(TEXT("/dcx SetSystemCursors [cursor id] [filename]"));
	mIRCLinker::echo(TEXT("[cursor id] = appstarting,normal,cross,hand,ibeam,no,sizeall,sizenesw,sizens,sizenwse,sizewe,wait"));
	mIRCLinker::echo(TEXT("[filename] = The path to a .cur or .ani file."));
	return 0;
}

/*!
* \brief DCX DLL /dcx SetmIRCCursors Function
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

		if (!tsFilename.empty() && !IsFile(tsFilename))
			throw Dcx::dcxException("Unable to open file");

		for (auto itStart = tsCursor.begin(TSCOMMACHAR), itEnd = tsCursor.end(); itStart != itEnd; ++itStart)
		{
			const auto uAreaHash(std::hash<TString>{}(*itStart));
			
			if (const auto AreaId = Dcx::parseAreaType(uAreaHash); AreaId > 0)
			{
				if (tsFilename.empty())
				{
					// empty filename == remove custom cursor, if any.
					Dcx::deleteAreaCursor(AreaId);
				}
				else
					Dcx::setAreaCursor(Dcx::dcxLoadCursorFromFile(tsFilename), AreaId);
			}
			else {
				const auto CursorId = Dcx::parseCursorType(uAreaHash);
				if (CursorId == nullptr)
					throw Dcx::dcxException("Unknown cursor type");

				if (auto hSystemCursor = Dcx::dcxLoadCursorFromResource(CursorId); tsFilename.empty())
				{
					// empty filename == remove custom cursor, if any.

					Dcx::deleteCursor(hSystemCursor);
				}
				else
					Dcx::setCursor(hSystemCursor, Dcx::dcxLoadCursorFromFile(tsFilename));
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
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("/dcx SetmIRCCursors"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("/dcx SetmIRCCursors"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/dcx SetmIRCCursors"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("/dcx SetmIRCCursors"), TEXT("\"%\" error: Unknown Exception"), d);
	}

	mIRCLinker::echo(TEXT("/dcx SetmIRCCursors [type(,type2,...)] (filename)"));
	mIRCLinker::echo(TEXT("[type] = appstarting,arrow,cross,hand,help,ibeam,no,sizeall,sizenesw,sizens,sizenwse,sizewe,uparrow,wait,client,caption,sysmenu,size,menu,vscroll,help,hscroll,min,max,left,right,top,topleft,topright,bottom,bottomleft,bottomright,border,close"));
	mIRCLinker::echo(TEXT("(filename) = optional, if empty custom cursor is removed, otherwise the path to a .cur or .ani file."));
	return 0;
}

/*!
* \brief DCX DLL /dcx SetDCXSettings Function
*
* mIRC /dcx SetDCXSettings [option] [option args]
*
* Argument \b data contains -> ...
*/

mIRC(SetDCXSettings)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.empty())
			throw Dcx::dcxException("Invalid Arguments");

		const auto tsOpt(d.getfirsttok(1));

		switch (std::hash<TString>{}(tsOpt))
		{
		case L"StaticColours"_hash:
		{
			Dcx::setting_bStaticColours = (d.getnexttok().to_int() > 0);
			break;
		}
		case L"UpdateColours"_hash:
		{
			const auto btmp = Dcx::setting_bStaticColours;
		
			Dcx::setting_bStaticColours = false;
			getmIRCPalette();
		
			Dcx::setting_bStaticColours = btmp;
			break;
		}
		default:
			throw Dcx::dcxException("Invalid Option");
		}
		return 1;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("/dcx SetDCXSettings"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("/dcx SetDCXSettings"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/dcx SetDCXSettings"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("/dcx SetDCXSettings"), TEXT("\"%\" error: Unknown Exception"), d);
	}

	mIRCLinker::echo(TEXT("/dcx SetDCXSettings [option] (option args)"));
	mIRCLinker::echo(TEXT("[option] = StaticColours,UpdateColours"));
	mIRCLinker::echo(TEXT("(option args) = optional, args contents depends on the option used."));
	return 0;
}
