/*!
* \file mircFunctions.cpp
* \brief Methods which will print out information to mIRC windows.
*
* This file contains the informative (debug/error/status) information for DCX dll routines.
*
* \author William Nguyen ( twig at genscripts dot net )
* \version 1.0
*
* \b Revisions
*
* © ScriptsDB.org - 2007-2008
*/

#include "defines.h"
#include "Dcx.h"
#include "mIRCLinker.h"



/*!
* \brief Displays an error message for the control when using $xdid().prop
*/
//void dcxInfoError(const TCHAR *ctrl, const TCHAR *functn, const TCHAR* dlg, const int ctrlid, const TCHAR *msg) {
//	TString err;
//
//	err.sprintf(TEXT("D_ERROR %s(%s, %d).%s: %s"), ctrl, dlg, ctrlid, functn, msg);
//	mIRCError(err.to_chr());
//}

/*!
* \brief Converts mIRC long time to C++ SYSTEMTIME object.
*/
SYSTEMTIME MircTimeToSystemTime(const long mircTime) {
	TString str;
	SYSTEMTIME st;

	ZeroMemory(&st, sizeof(SYSTEMTIME));

	mIRCLinker::tsEvalex(str, TEXT("$asctime(%ld, d m yyyy hh nn ss)"), mircTime);

	st.wDay = (WORD)str.getfirsttok(1).to_int();
	st.wMonth = (WORD)str.getnexttok().to_int();
	st.wYear = (WORD)str.getnexttok().to_int();
	st.wHour = (WORD)str.getnexttok().to_int();
	st.wMinute = (WORD)str.getnexttok().to_int();
	st.wSecond = (WORD)str.getnexttok().to_int();

	return st;
}

long SystemTimeToMircTime(const LPSYSTEMTIME pst) {
	if (pst->wMonth == 0) {
		Dcx::error(TEXT("SystemTimeToMircTime"), TEXT("invalid SYSTEMTIME parameter."));
		return 0;
	}

	//TCHAR sRet[100];
	stString<100U> sRet;

	static const TCHAR *months[12] = {
		TEXT("January"),
		TEXT("Feburary"),
		TEXT("March"),
		TEXT("April"),
		TEXT("May"),
		TEXT("June"),
		TEXT("July"),
		TEXT("August"),
		TEXT("September"),
		TEXT("October"),
		TEXT("November"),
		TEXT("December")
	};

	mIRCLinker::evalex(sRet, static_cast<int>(Dcx::countof(sRet)), TEXT("$ctime(%u:%u:%u %u %s %u)"),
		pst->wHour,
		pst->wMinute,
		pst->wSecond,
		pst->wDay,
		months[pst->wMonth -1],
		pst->wYear);

	return dcx_atoi(sRet.data());
}
