/*!
TODO: monthchange event
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/monthcal/structures/nmdaystate.asp

MCHT_TITLEBTNNEXT
    The given point was over the button at the top right corner of the control. If the user clicks here, the month calendar will scroll its display to the next month or set of months.
MCHT_TITLEBTNPREV
    The given point was over the button at the top left corner of the control. If the user clicks here, the month calendar will scroll its display to the previous month or set of months.

 * \file dcxcalendar.cpp
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
#include "defines.h"
#include "Classes/dcxcalendar.h"
#include "Classes/dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxCalendar::DcxCalendar(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	m_MonthDayStates[0] = 0;
	m_MonthDayStates[1] = 0;
	m_MonthDayStates[2] = 0;
	m_MonthDayStates[3] = 0;
	m_MonthDayStates[4] = 0;
	m_MonthDayStates[5] = 0;
	m_MonthDayStates[6] = 0;
	m_MonthDayStates[7] = 0;
	m_MonthDayStates[8] = 0;
	m_MonthDayStates[9] = 0;
	m_MonthDayStates[10] = 0;
	m_MonthDayStates[11] = 0;

	this->m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CLIENTEDGE,
		DCX_CALENDARCLASS,
		nullptr,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(this->m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(this->m_Hwnd, L" ", L" ");

	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxCalendar::~DcxCalendar() {

	this->unregistreDefaultWindowProc();
}

void DcxCalendar::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);
	xml->SetAttribute("caption", getValue().c_str());
}

const TString DcxCalendar::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(this->m_Hwnd);

	if (dcx_testflag(Styles, MCS_MULTISELECT))
		styles.addtok(TEXT("multi"));
	if (dcx_testflag(Styles, MCS_NOTODAY))
		styles.addtok(TEXT("notoday"));
	if (dcx_testflag(Styles, MCS_NOTODAYCIRCLE))
		styles.addtok(TEXT("notodaycircle"));
	if (dcx_testflag(Styles, MCS_WEEKNUMBERS))
		styles.addtok(TEXT("weeknum"));
	if (dcx_testflag(Styles, MCS_DAYSTATE))
		styles.addtok(TEXT("daystate"));

	return styles;
}

const TString DcxCalendar::getValue(void) const
{
	long start, end;
	TCHAR buf[128];

	if (isStyle(MCS_MULTISELECT)) {
		SYSTEMTIME st[2];
		ZeroMemory(st, sizeof(SYSTEMTIME) *2);
		MonthCal_GetSelRange(this->m_Hwnd, st);

		start = SystemTimeToMircTime(&(st[0]));
		end = SystemTimeToMircTime(&(st[1]));
	}
	else {
		SYSTEMTIME st;

		ZeroMemory(&st, sizeof(SYSTEMTIME));
		MonthCal_GetCurSel(this->m_Hwnd, &st);

		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		st.wMilliseconds = 0;

		start = SystemTimeToMircTime(&st);
		end = start;
	}

	wnsprintf(buf, 128, TEXT("%ld %ld"), start, end);
	return buf;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxCalendar::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme)
{
	for (const auto &tsStyle: styles)
	{
		if (tsStyle == TEXT("multi"))
			*Styles |= MCS_MULTISELECT;
		else if (tsStyle == TEXT("notoday"))
			*Styles |= MCS_NOTODAY;
		else if (tsStyle == TEXT("notodaycircle"))
			*Styles |= MCS_NOTODAYCIRCLE;
		else if (tsStyle == TEXT("weeknum"))
			*Styles |= MCS_WEEKNUMBERS;
		else if (tsStyle == TEXT("daystate"))
			*Styles |= MCS_DAYSTATE;
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

void DcxCalendar::parseInfoRequest(const TString &input, PTCHAR szReturnValue) const
{
	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP]
	if (prop == TEXT("value")) {
		dcx_strcpyn(szReturnValue, this->getValue().to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	else if (prop == TEXT("range")) {
		SYSTEMTIME st[2];
		TString dmin(TEXT("nolimit"));
		TString dmax(TEXT("nolimit"));

		ZeroMemory(st, sizeof(SYSTEMTIME) *2);

		const auto val = MonthCal_GetRange(this->m_Hwnd, st);

		if (dcx_testflag(val, GDTR_MIN))
			dmin.tsprintf(TEXT("%ld"), SystemTimeToMircTime(&(st[0])));

		if (dcx_testflag(val, GDTR_MAX))
			dmax.tsprintf(TEXT("%ld"), SystemTimeToMircTime(&(st[1])));

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%s %s"), dmin.to_chr(), dmax.to_chr()); // going to be within 900 limit anyway.
	}
	else if (prop == TEXT("today")) {
		SYSTEMTIME st;

		ZeroMemory(&st, sizeof(SYSTEMTIME));

		MonthCal_GetToday(this->m_Hwnd, &st);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ld"), SystemTimeToMircTime(&st));
	}
	else if (prop == TEXT("selcount")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), MonthCal_GetMaxSelCount(this->m_Hwnd));
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */
void DcxCalendar::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));

//SetDayState

	const auto numtok = input.numtok();

	// xdid -k [NAME] [ID] [SWITCH] [+FLAGS] [$RGB]
	if (flags[TEXT('k')] && numtok > 4) {
		const XSwitchFlags xflags(input.getnexttok( ));	// tok 4
		const auto col = input.getnexttok().to_<COLORREF>();	// tok 5

		// Set the background color displayed between months.
		if (xflags[TEXT('b')])
			MonthCal_SetColor(this->m_Hwnd, MCSC_BACKGROUND, col);

		// Set the background color displayed within the month.
		if (xflags[TEXT('g')])
			MonthCal_SetColor(this->m_Hwnd, MCSC_MONTHBK, col);

		// Set the color used to display text within a month.
		if (xflags[TEXT('t')])
			MonthCal_SetColor(this->m_Hwnd, MCSC_TEXT, col);

		// Set the background color displayed in the calendar's title and selection color.
		if (xflags[TEXT('i')])
			MonthCal_SetColor(this->m_Hwnd, MCSC_TITLEBK, col);

		// Set the color used to display text within the calendar's title.
		if (xflags[TEXT('a')])
			MonthCal_SetColor(this->m_Hwnd, MCSC_TITLETEXT, col);

		// Set the color used to display header day and trailing day text. Header and trailing days are the days from the previous and following months that appear on the current month calendar.
		if (xflags[TEXT('r')])
			MonthCal_SetColor(this->m_Hwnd, MCSC_TRAILINGTEXT, col);
	}
	//xdid -m [NAME] [ID] [SWITCH] [MAX]
	else if (flags[TEXT('m')] && numtok > 3) {
		const auto max = input.getnexttok().to_int();	// tok 4

		MonthCal_SetMaxSelCount(this->m_Hwnd, max);
	}
	//xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags[TEXT('r')] && numtok > 4) {
		DWORD dflags = 0;
		SYSTEMTIME range[2];

		ZeroMemory(range, sizeof(SYSTEMTIME) *2);

		const auto tsMin(input.getnexttok());	// tok 4
		const auto tsMax(input.getnexttok());	// tok 5

		if (tsMin != TEXT("nolimit")) {
			range[0] = MircTimeToSystemTime((long) tsMin.to_num());
			dflags |= GDTR_MIN;
		}

		if (tsMax != TEXT("nolimit")) {
			range[1] = MircTimeToSystemTime((long) tsMax.to_num());
			dflags |= GDTR_MAX;
		}

		MonthCal_SetRange(this->m_Hwnd, dflags, range);
	}
	//xdid -s [NAME] [ID] [SWITCH] [MIN] (MAX)
	else if (flags[TEXT('s')] && numtok > 3) {
		const auto min = input.getnexttok().to_<long>();	// tok 4
		SYSTEMTIME range[2];

		ZeroMemory(range, sizeof(SYSTEMTIME) *2);
		range[0] = MircTimeToSystemTime(min);

		if (isStyle(MCS_MULTISELECT)) {
			// if only one date specified, select the same date
			if (numtok < 5)
				range[1] = range[0];
			else {
				const auto max = input.getnexttok().to_<long>();	// tok 5
				range[1] = MircTimeToSystemTime(max);
			}

			MonthCal_SetSelRange(this->m_Hwnd, range);
		}
		else
			MonthCal_SetCurSel(this->m_Hwnd, &(range[0]));
	}
	//xdid -t [NAME] [ID] [SWITCH] [TIMESTAMP]
	else if (flags[TEXT('t')] && numtok > 3) {
		const auto mircTime = input.getnexttok().to_<long>();	// tok 4
		const auto sysTime = MircTimeToSystemTime(mircTime);

		MonthCal_SetToday(this->m_Hwnd, &sysTime);
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxCalendar::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) {
	switch (uMsg) {
		case WM_NOTIFY: {
			dcxlParam(LPNMHDR, hdr);

			if (hdr == nullptr)
				break;

			switch(hdr->code) {
				case MCN_GETDAYSTATE: {
					dcxlParam(LPNMDAYSTATE, lpNMDayState);

					const auto iMax = lpNMDayState->cDayState;
					TCHAR eval[100];

					for (auto i = decltype(iMax){0}; i < iMax; i++) {
						// daystate ctrlid startdate
						this->evalAliasEx(eval, 100, TEXT("%s,%d,%d"), TEXT("daystate"), this->getUserID(), SystemTimeToMircTime(&(lpNMDayState->stStart)));
						m_MonthDayStates[i] = (MONTHDAYSTATE) 0;

						TString strDays(eval);

						strDays.trim();

						for (auto itStart = strDays.begin(TSCOMMA), itEnd = strDays.end(); itStart != itEnd; ++itStart)
							BOLDDAY(m_MonthDayStates[i], (*itStart).to_int());

						// increment the month so we get a proper offset
						lpNMDayState->stStart.wMonth++;

						if (lpNMDayState->stStart.wMonth > 12) {
							lpNMDayState->stStart.wMonth = 1;
							lpNMDayState->stStart.wYear++;
						}
					}

					lpNMDayState->prgDayState = m_MonthDayStates;
					bParsed = TRUE;
					return FALSE;
				}

				case MCN_SELCHANGE: {
					this->execAliasEx(TEXT("%s,%d"), TEXT("selchange"), this->getUserID());
					break;
				}
				case MCN_SELECT: {
					// specific code to handle multiselect dates
					if (this->isStyle(MCS_MULTISELECT)) {
						// get the selected range
						SYSTEMTIME selrange[2];

						MonthCal_GetSelRange(this->m_Hwnd, selrange);

						// send event to callback
						this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("select"), this->getUserID(),
							SystemTimeToMircTime(&(selrange[0])),
							SystemTimeToMircTime(&(selrange[1])));
					}
					// code to handle single selected dates
					else {
						SYSTEMTIME st;

						ZeroMemory(&st, sizeof(SYSTEMTIME));
						MonthCal_GetCurSel(this->m_Hwnd, &st);

						st.wHour = 0;
						st.wMinute = 0;
						st.wSecond = 0;
						st.wMilliseconds = 0;

						// send event to callback
						this->execAliasEx(TEXT("%s,%d,%d"), TEXT("select"), this->getUserID(), SystemTimeToMircTime(&st));
					}

					break;
				}
				case NM_RELEASEDCAPTURE: {
					if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
						this->execAliasEx(TEXT("%s,%d"), TEXT("sclick"), this->getUserID());
					break;
				}
				default:
					break;
			} // end switch
		}
	}
	return 0L;
}

LRESULT DcxCalendar::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) {
	switch (uMsg) {

	case WM_LBUTTONUP:
		{
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				this->execAliasEx(TEXT("%s,%d"), TEXT("lbup"), this->getUserID( ) );
		}
		break;

		//case WM_GETDLGCODE:
		//{
		//	return DLGC_WANTARROWS;
		//	break;
		//}

	case WM_DESTROY:
		{
			delete this;
			bParsed = TRUE;
			break;
		}

	default:
		return this->CommonMessage( uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}
