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
//void dcxInfoError(const char *ctrl, const char *functn, const char* dlg, const int ctrlid, const char *msg) {
//	TString err;
//
//	err.sprintf("D_ERROR %s(%s, %d).%s: %s", ctrl, dlg, ctrlid, functn, msg);
//	mIRCError(err.to_chr());
//}

/*!
* \brief Converts mIRC long time to C++ SYSTEMTIME object.
*/
SYSTEMTIME MircTimeToSystemTime(const long mircTime) {
	TString str;
	SYSTEMTIME st;

	ZeroMemory(&st, sizeof(SYSTEMTIME));

	Dcx::mIRC.tsEvalex( str, "$asctime(%ld, d m yyyy)", mircTime);

	st.wDay = (WORD)str.gettok(1).to_int();
	st.wMonth = (WORD)str.gettok(2).to_int();
	st.wYear = (WORD)str.gettok(3).to_int();

	return st;
}

long SystemTimeToMircTime(const LPSYSTEMTIME pst) {
	if (pst->wMonth == 0) {
		Dcx::error("SystemTimeToMircTime", "invalid SYSTEMTIME parameter.");
		return 0;
	}

	char ret[100];

	static const char *months[12] = {
		"January",
		"Feburary",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December"
	};

	Dcx::mIRC.evalex(ret, 100, "$ctime(%d:%d:%d %d %s %d)",
		pst->wHour,
		pst->wMinute,
		pst->wSecond,
		pst->wDay,
		months[pst->wMonth -1],
		pst->wYear);

	return atol(ret);
}
