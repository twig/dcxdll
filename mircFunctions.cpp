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
* \brief Converts mIRC long time to C++ SYSTEMTIME object.
*/
SYSTEMTIME MircTimeToSystemTime(const long mircTime)
{
	//TString str;
	//SYSTEMTIME st = { 0 };
	//
	////mIRCLinker::tsEvalex(str, TEXT("$asctime(%ld, d m yyyy hh nn ss)"), mircTime);
	//mIRCLinker::eval(str, TEXT("$asctime(%,d m yyyy hh nn ss)"), mircTime);
	//
	//st.wDay = str.getfirsttok(1).to_<WORD>();
	//st.wMonth = str.getnexttok().to_<WORD>();
	//st.wYear = str.getnexttok().to_<WORD>();
	//st.wHour = str.getnexttok().to_<WORD>();
	//st.wMinute = str.getnexttok().to_<WORD>();
	//st.wSecond = str.getnexttok().to_<WORD>();
	//
	//return st;

	TString str;

	mIRCLinker::eval(str, TEXT("$asctime(%,yyyy m d hh nn ss)"), mircTime);

	return { str.getfirsttok(1).to_<WORD>(), str.getnexttok().to_<WORD>(), 0, str.getnexttok().to_<WORD>(), str.getnexttok().to_<WORD>(), str.getnexttok().to_<WORD>(), str.getnexttok().to_<WORD>(), 0 };
}

long SystemTimeToMircTime(const LPSYSTEMTIME pst)
{
	if ((pst == nullptr) || (pst->wMonth == 0))
	{
		Dcx::error(TEXT("SystemTimeToMircTime"), TEXT("invalid SYSTEMTIME parameter."));
		return 0;
	}

	//TCHAR sRet[100];
	//stString<100U> sRet;

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

	//mIRCLinker::evalex(sRet, static_cast<int>(Dcx::countof(sRet)), TEXT("$ctime(%u:%u:%u %u %s %u)"),
	//	pst->wHour,
	//	pst->wMinute,
	//	pst->wSecond,
	//	pst->wDay,
	//	months[pst->wMonth -1],
	//	pst->wYear);

	//mIRCLinker::eval(sRet, TEXT("$ctime(%:%:% % % %)"),
	//	pst->wHour,
	//	pst->wMinute,
	//	pst->wSecond,
	//	pst->wDay,
	//	months[pst->wMonth - 1],
	//	pst->wYear);
	//
	//return dcx_atoi(sRet.data());

	

	//if (const auto[bOk, iNum] = mIRCLinker::uEval<long>(TEXT("$ctime(%:%:% % % %)"),
	//	pst->wHour,
	//	pst->wMinute,
	//	pst->wSecond,
	//	pst->wDay,
	//	months[pst->wMonth - 1],
	//	pst->wYear); bOk)
	//	return iNum;

	if (const auto iNum = mIRCLinker::uEval<long>(TEXT("$ctime(%:%:% % % %)"),
		pst->wHour,
		pst->wMinute,
		pst->wSecond,
		pst->wDay,
		months[pst->wMonth - 1],
		pst->wYear); iNum.has_value())
		return *iNum;

	Dcx::error(TEXT("SystemTimeToMircTime"), TEXT("Unable to get time."));
	return 0;
}
