/*!
Documentation
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/datetime/reflist.asp

 * \file dcxDateTime.cpp
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2006
 */

#include "dcxdatetime.h"
#include "dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

//DcxDateTime::DcxDateTime( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
//: DcxControl( ID, p_Dialog ) 
//{
//
//  LONG Styles = 0, ExStyles = 0;
//  BOOL bNoTheme = FALSE;
//  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );
//
//  this->m_Hwnd = CreateWindowEx(	
//    ExStyles | WS_EX_CLIENTEDGE, 
//    DCX_DATETIMECLASS, 
//    NULL,
//    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
//    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
//    p_Dialog->getHwnd( ),
//    (HMENU) ID,
//    GetModuleHandle( NULL ), 
//    NULL);
//
//  if ( bNoTheme )
//    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );
//
//  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
//  this->registreDefaultWindowProc( );
//  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
//}
//
/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxDateTime::DcxDateTime(UINT ID, DcxDialog *p_Dialog, HWND mParentHwnd, RECT *rc, TString &styles)
: DcxControl(ID, p_Dialog) 
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	this->m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CLIENTEDGE,
		DCX_DATETIMECLASS,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);

	if (bNoTheme)
		dcxSetWindowTheme(this->m_Hwnd , L" ", L" ");

	this->setControlFont((HFONT) GetStockObject(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(this->m_Hwnd, "dcx_cthis", (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */
DcxDateTime::~DcxDateTime() {
	this->unregistreDefaultWindowProc();
}

/*!
 * \brief blah
 *
 * blah
 */
void DcxDateTime::parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme) {
	unsigned int i = 1, numtok = styles.numtok();

	while (i <= numtok) {
		if (styles.gettok(i) == "longdateformat")
			*Styles |= DTS_LONGDATEFORMAT;
		else if (styles.gettok(i) == "shortdateformat")
			*Styles |= DTS_SHORTDATEFORMAT;
		else if (styles.gettok(i) == "shortdatecenturyformat")
			*Styles |= DTS_SHORTDATECENTURYFORMAT;

		else if (styles.gettok(i) == "timeformat")
			*Styles |= DTS_TIMEFORMAT;

		else if (styles.gettok(i) == "right")
			*Styles |= DTS_RIGHTALIGN;
		else if (styles.gettok(i) == "shownone")
			*Styles |= DTS_SHOWNONE;
		else if (styles.gettok(i) == "updown")
			*Styles |= DTS_UPDOWN;

		i++;
	}

	this->parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */
void DcxDateTime::parseInfoRequest(TString &input, char *szReturnValue) {
//			DateTime_GetRange
//			DateTime_GetSystemtime

	//int numtok = input.numtok();

	//// [NAME] [ID] [PROP]
	//if (input.gettok(3) == "selected") {
	//	long start;
	//	long end;

	//	if (isStyle(MCS_MULTISELECT)) {
	//		SYSTEMTIME st[2];

	//		ZeroMemory(st, sizeof(SYSTEMTIME) *2);

	//		MonthCal_GetSelRange(this->m_Hwnd, st);
	//		start = SystemTimeToMircTime(&(st[0]));
	//		end = SystemTimeToMircTime(&(st[1]));
	//	}
	//	else {
	//		SYSTEMTIME st;

	//		MonthCal_GetCurSel(this->m_Hwnd, &st);
	//		start = SystemTimeToMircTime(&st);
	//		end = start;
	//	}

	//	wsprintf(szReturnValue, "%ld %ld", start, end);
	//	return;
	//}
	//else if (input.gettok(3) == "range") {
	//	SYSTEMTIME st[2];
	//	TString min;
	//	TString max;
	//	DWORD val;

	//	ZeroMemory(st, sizeof(SYSTEMTIME) *2);

	//	val = MonthCal_GetRange(this->m_Hwnd, st);

	//	if (val & GDTR_MIN)
	//		min.sprintf("%ld", SystemTimeToMircTime(&(st[0])));
	//	else
	//		min = "nolimit";

	//	if (val & GDTR_MAX)
	//		max.sprintf("%ld", SystemTimeToMircTime(&(st[1])));
	//	else
	//		max = "nolimit";

	//	wsprintf(szReturnValue, "%s %s", min, max);
	//	return;
	//}
	//else if (input.gettok(3) == "today") {
	//	SYSTEMTIME st;

	//	ZeroMemory(&st, sizeof(SYSTEMTIME));

	//	MonthCal_GetToday(this->m_Hwnd, &st);
	//	wsprintf(szReturnValue, "%ld", SystemTimeToMircTime(&st));
	//	return;
	//}
	//else if (input.gettok(3) == "selcount") {
	//	wsprintf(szReturnValue, "%d", MonthCal_GetMaxSelCount(this->m_Hwnd));
	//	return;
	//}
	//else
	if (this->parseGlobalInfoRequest(input, szReturnValue)) {
		return;
	}

	szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */
void DcxDateTime::parseCommandRequest(TString &input) {
	XSwitchFlags flags;
	ZeroMemory((void*) &flags, sizeof(XSwitchFlags));
	this->parseSwitchFlags(input.gettok(3), &flags);

	int numtok = input.numtok();

	// xdid -f [NAME] [ID] [SWITCH] (FORMAT)
	if (flags.switch_flags[5]) {
		if (numtok > 3) {
			TString format = input.gettok(4, -1);
			DateTime_SetFormat(this->m_Hwnd, format.to_chr());
		}
		else {
			DateTime_SetFormat(this->m_Hwnd, NULL);
			return;
		}
	}
	//xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags.switch_flags[17] && numtok > 4) {
		DWORD flags = 0;
		SYSTEMTIME range[2];

		ZeroMemory(range, sizeof(SYSTEMTIME) *2);

		if (input.gettok(4) != "nolimit") {
			long min = (long) input.gettok(4).to_num();
			range[0] = MircTimeToSystemTime(min);
			flags |= GDTR_MIN;
		}

		if (input.gettok(5) != "nolimit") {
			long max = (long) input.gettok(5).to_num();
			range[1] = MircTimeToSystemTime(max);
			flags |= GDTR_MAX;
		}

		DateTime_SetRange(this->m_Hwnd, flags, range);
	}
	//xdid -t [NAME] [ID] [SWITCH] [TIMESTAMP]
	else if (flags.switch_flags[19] && numtok > 3) {
		TString ts = input.gettok(4);

		if (ts == "reset") {
			if (isStyle(DTS_SHOWNONE)) {
				DateTime_SetSystemtime(this->m_Hwnd, GDT_NONE, NULL);
			}
		}
		else {
			SYSTEMTIME sysTime;
			long mircTime = (long) ts.to_num();

			sysTime = MircTimeToSystemTime(mircTime);
			DateTime_SetSystemtime(this->m_Hwnd, GDT_VALID, &sysTime);
		}
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxDateTime::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) {
	//switch (uMsg) {
	//	case WM_NOTIFY: {
	//		LPNMHDR hdr = (LPNMHDR) lParam;

	//		if (!hdr)
	//			break;

	//		switch(hdr->code) {
	//			case MCN_GETDAYSTATE: {
	//				LPNMDAYSTATE lpNMDayState = (LPNMDAYSTATE) lParam;
	//				MONTHDAYSTATE mds[12];

	//				int iMax = lpNMDayState->cDayState;
	//				char eval[100];

	//				for (int i = 0; i < iMax; i++) {
	//					// daystate ctrlid startdate
	//					this->callAliasEx(eval, "%s,%d,%d", "daystate", this->getUserID(),
	//						SystemTimeToMircTime(&(lpNMDayState->stStart)));
	//					mds[i] = (MONTHDAYSTATE) 0;

	//					TString strDays(eval);
	//					strDays.trim();

	//					for (int x = 1; x <= strDays.numtok(","); x++) {
	//						TString tok = strDays.gettok(x);
	//						tok.trim();
	//						BOLDDAY(mds[i], tok.to_int());
	//					}

	//					// increment the month so we get a proper offset
	//					lpNMDayState->stStart.wMonth++;

	//					if (lpNMDayState->stStart.wMonth > 12) {
	//						lpNMDayState->stStart.wMonth = 1;
	//						lpNMDayState->stStart.wYear++;
	//					}
	//				}

	//				lpNMDayState->prgDayState = mds;
	//				bParsed = TRUE;
	//				return FALSE;
	//				break;
	//			}

	//			case MCN_SELCHANGE: {
	//				this->callAliasEx(NULL, "%s,%d", "selchange", this->getUserID());
	//				break;
	//			}
	//			case MCN_SELECT: {
	//				// specific code to handle multiselect dates
	//				if (this->isStyle(MCS_MULTISELECT)) {
	//					// get the selected range
	//					SYSTEMTIME selrange[2];

	//					MonthCal_GetSelRange(this->m_Hwnd, selrange);

	//					// send event to callback
	//					this->callAliasEx(NULL, "%s,%d,%d,%d", "select", this->getUserID(),
	//						SystemTimeToMircTime(&(selrange[0])),
	//						SystemTimeToMircTime(&(selrange[1])));
	//				}
	//				// code to handle single selected dates
	//				else {
	//					SYSTEMTIME st;
	//					MonthCal_GetCurSel(this->m_Hwnd, &st);

	//					// send event to callback
	//					this->callAliasEx(NULL, "%s,%d,%d", "select", this->getUserID(), SystemTimeToMircTime(&st));
	//				}

	//				break;
	//			}
	//			case NM_RELEASEDCAPTURE: {
	//				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
	//					this->callAliasEx(NULL, "%s,%d", "sclick", this->getUserID());
	//				break;
	//			}
	//			default: {
	//				//mIRCError("ELSE");
	//				break;
	//			}
	//		} // end switch
	//	}
	//}
	return 0L;
}

LRESULT DcxDateTime::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) {
	switch (uMsg) {
		case WM_HELP:
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
				this->callAliasEx(NULL, "%s,%d", "help", this->getUserID());

			break;

		case WM_MOUSEMOVE:
			this->m_pParentDialog->setMouseControl(this->getUserID());
			break;

		case WM_SETFOCUS:
			this->m_pParentDialog->setFocusControl(this->getUserID());
			break;

		case WM_SETCURSOR:
			if (LOWORD(lParam) == HTCLIENT && (HWND) wParam == this->m_Hwnd && this->m_hCursor != NULL) {
				SetCursor(this->m_hCursor);
				bParsed = TRUE;
				return TRUE;
			}

			break;

		case WM_DESTROY:
			delete this;
			bParsed = TRUE;
			break;

		default:
			break;
	}

	return 0L;
}