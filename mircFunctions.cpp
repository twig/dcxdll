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
* © ScriptsDB.org - 2007
*/

#include "defines.h"

extern bool dcxSignal;

/*!
* \brief Sends a signal to mIRC.
*
* This method allows for multiple parameters.
*/
void mIRCSignalDCX(const char *szFormat, ...) {
	if (!dcxSignal)
		return;
	va_list args;
	va_start(args, szFormat);

	char msg[2048];
	vsprintf(msg, szFormat, args);
	mIRCSignal(msg);
	va_end(args);
}

/*!
* \brief Sends a signal to mIRC.
*/
void mIRCSignal(const char *data) {
	/*
	logFile = fopen( "dcxlog.txt", "a" );

	if ( logFile ) {
	fwrite( data, sizeof( char ), strlen( data ), logFile );
	fclose( logFile );
	}
	*/

	wsprintf(mIRCLink.m_pData, "//.signal -n DCX %s", data);
	SendMessage(mIRCLink.m_mIRCHWND, WM_USER + 200, 0, mIRCLink.m_map_cnt);
}

/*!
 * \brief Sends a debug message to mIRC (without formatting).
 *
 * This method allows for multiple parameters.
 */
void mIRCDebug(const char *szFormat, ...) {
	va_list args;
	va_start(args, szFormat);

	char msg[2048];
	vsprintf(msg, szFormat, args);
	mIRCError(msg);
	va_end(args);
}

/*!
* \brief Displays output text to the mIRC status window.
*/
void mIRCError(const char *data) {
	wsprintf(mIRCLink.m_pData, "//echo -s %s", data);
	SendMessage(mIRCLink.m_mIRCHWND, WM_USER + 200, 0, mIRCLink.m_map_cnt);
}

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
 * \brief Sends an error message to mIRC.
 */
void DCXError(const char *cmd, const char *msg)
{
	TString err;

	err.sprintf("D_ERROR %s (%s)", cmd, msg);
	mIRCError(err.to_chr());
}

/*!
 * \brief Sends a debug message to mIRC (with formatting).
 *
 * This method allows for multiple parameters.
 */
void DCXDebug(const char *cmd, const char *msg)
{
	TString err;

	err.sprintf("D_DEBUG %s (%s)", cmd, msg);
	mIRCError(err.to_chr());
}

/*!
 * \brief Requests mIRC $identifiers to be evaluated.
 *
 * Allow sufficient characters to be returned.
 */
void mIRCeval(const char *data, char *res) {
	lstrcpy(mIRCLink.m_pData, data);
	SendMessage(mIRCLink.m_mIRCHWND, WM_USER + 201, 0, mIRCLink.m_map_cnt);
	lstrcpy(res, mIRCLink.m_pData);
}

/*!
/*!
 * \brief Requests mIRC $identifiers to be evaluated.
 *
 * Allow sufficient characters to be returned.
 * Requests mIRC to perform command using vsprintf.
*/
void mIRCevalEX(char *res, const char *szFormat, ...) {
	va_list args;
	va_start(args, szFormat);

	char msg[2048];
	vsprintf(msg, szFormat, args);
	mIRCeval(msg, res);
	va_end(args);
}

/*!
* \brief Requests mIRC to perform command.
*/
void mIRCcom(const char *data) {
	lstrcpy(mIRCLink.m_pData, data);
	SendMessage(mIRCLink.m_mIRCHWND, WM_USER + 200, 0, mIRCLink.m_map_cnt);
}
/*!
* \brief Requests mIRC to perform command using vsprintf.
*/
void mIRCcomEX(const char *szFormat, ...) {
	va_list args;
	va_start(args, szFormat);

	char msg[2048];
	vsprintf(msg, szFormat, args);
	mIRCcom(msg);
	va_end(args);
}

/*!
* \brief Converts mIRC long time to C++ SYSTEMTIME object.
*/
SYSTEMTIME MircTimeToSystemTime(const long mircTime) {
	char eval[100];
	SYSTEMTIME st;

	ZeroMemory(&st, sizeof(SYSTEMTIME));

	mIRCevalEX( eval, "$asctime(%ld, d m yyyy)", mircTime);

	TString str(eval);

	st.wDay = str.gettok(1).to_int();
	st.wMonth = str.gettok(2).to_int();
	st.wYear = str.gettok(3).to_int();

	return st;
}

long SystemTimeToMircTime(const LPSYSTEMTIME pst) {
	if (pst->wMonth == 0) {
		DCXError("SystemTimeToMircTime", "invalid SYSTEMTIME parameter.");
		return 0;
	}

	char ret[100];

	static const TString months[12] = {
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

	wsprintf(ret, "$ctime(%d:%d:%d %d %s %d)",
		pst->wHour,
		pst->wMinute,
		pst->wSecond,
		pst->wDay,
		months[pst->wMonth -1].to_chr(),
		pst->wYear);

	mIRCeval(ret, ret);
	return atol(ret);
}