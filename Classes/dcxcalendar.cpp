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
	, m_MonthDayStates()
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	m_Hwnd = CreateWindowEx(
		gsl::narrow_cast<DWORD>(ExStyles) | WS_EX_CLIENTEDGE,
		DCX_CALENDARCLASS,
		nullptr,
		WS_CHILD | gsl::narrow_cast<DWORD>(Styles),
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
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
	xml->SetAttribute("styles", getStyles().c_str());
}

TiXmlElement * DcxCalendar::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

const TString DcxCalendar::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);

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
	long start = 0, end = 0;

	if (isStyle(MCS_MULTISELECT)) {
		SYSTEMTIME st[2];
		ZeroMemory(&st[0], sizeof(SYSTEMTIME) *2);
		MonthCal_GetSelRange(m_Hwnd, st);

		start = SystemTimeToMircTime(&(st[0]));
		end = SystemTimeToMircTime(&(st[1]));
	}
	else {
		SYSTEMTIME st;

		ZeroMemory(&st, sizeof(SYSTEMTIME));
		MonthCal_GetCurSel(m_Hwnd, &st);

		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		st.wMilliseconds = 0;

		start = SystemTimeToMircTime(&st);
		end = start;
	}

	//TCHAR buf[128] = { 0 };

	//wnsprintf(buf, Dcx::countof(buf), TEXT("%ld %ld"), start, end);
	//return buf;

	stString<128> buf;

	//wnsprintf(buf, gsl::narrow_cast<int>(buf.size()), TEXT("%ld %ld"), start, end);
	_ts_snprintf(buf, TEXT("%ld %ld"), start, end);
	return buf.data();
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
#if DCX_USE_HASHING
		switch (std::hash<TString>{}(tsStyle))
		{
			case L"multi"_hash:
				*Styles |= MCS_MULTISELECT;
				break;
			case L"notoday"_hash:
				*Styles |= MCS_NOTODAY;
				break;
			case L"notodaycircle"_hash:
				*Styles |= MCS_NOTODAYCIRCLE;
				break;
			case L"weeknum"_hash:
				*Styles |= MCS_WEEKNUMBERS;
				break;
			case L"daystate"_hash:
				*Styles |= MCS_DAYSTATE;
			default:
				break;
		}
#else
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
#endif
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

void DcxCalendar::parseInfoRequest(const TString &input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
#if DCX_USE_HASHING
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"value"_hash:
		szReturnValue = getValue().to_chr();
		break;
	case L"range"_hash:
	{
		SYSTEMTIME st[2];
		TString dmin(TEXT("nolimit"));
		TString dmax(TEXT("nolimit"));

		ZeroMemory(&st[0], sizeof(SYSTEMTIME) * 2);

		const auto val = MonthCal_GetRange(m_Hwnd, st);

		if (dcx_testflag(val, GDTR_MIN))
			dmin.tsprintf(TEXT("%ld"), SystemTimeToMircTime(&(st[0])));

		if (dcx_testflag(val, GDTR_MAX))
			dmax.tsprintf(TEXT("%ld"), SystemTimeToMircTime(&(st[1])));

		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%s %s"), dmin.to_chr(), dmax.to_chr()); // going to be within 900 limit anyway.
		_ts_snprintf(szReturnValue, TEXT("%s %s"), dmin.to_chr(), dmax.to_chr()); // going to be within 900 limit anyway.
	}
	break;
	case L"today"_hash:
	{
		SYSTEMTIME st = { 0 };

		MonthCal_GetToday(m_Hwnd, &st);
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ld"), SystemTimeToMircTime(&st));
		_ts_snprintf(szReturnValue, TEXT("%ld"), SystemTimeToMircTime(&st));
	}
	break;
	case L"selcount"_hash:
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), MonthCal_GetMaxSelCount(m_Hwnd));
		_ts_snprintf(szReturnValue, TEXT("%u"), MonthCal_GetMaxSelCount(m_Hwnd));
		break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
		break;
	}
#else
	// [NAME] [ID] [PROP]
	if (const auto prop(input.getfirsttok(3)); prop == TEXT("value")) {
		szReturnValue = getValue().to_chr();
	}
	else if (prop == TEXT("range")) {
		SYSTEMTIME st[2];
		TString dmin(TEXT("nolimit"));
		TString dmax(TEXT("nolimit"));

		ZeroMemory(st, sizeof(SYSTEMTIME) *2);

		const auto val = MonthCal_GetRange(m_Hwnd, st);

		if (dcx_testflag(val, GDTR_MIN))
			dmin.tsprintf(TEXT("%ld"), SystemTimeToMircTime(&(st[0])));

		if (dcx_testflag(val, GDTR_MAX))
			dmax.tsprintf(TEXT("%ld"), SystemTimeToMircTime(&(st[1])));

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%s %s"), dmin.to_chr(), dmax.to_chr()); // going to be within 900 limit anyway.
	}
	else if (prop == TEXT("today")) {
		SYSTEMTIME st;

		ZeroMemory(&st, sizeof(SYSTEMTIME));

		MonthCal_GetToday(m_Hwnd, &st);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ld"), SystemTimeToMircTime(&st));
	}
	else if (prop == TEXT("selcount")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), MonthCal_GetMaxSelCount(m_Hwnd));
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
#endif
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
			MonthCal_SetColor(m_Hwnd, MCSC_BACKGROUND, static_cast<LPARAM>(col));

		// Set the background color displayed within the month.
		if (xflags[TEXT('g')])
			MonthCal_SetColor(m_Hwnd, MCSC_MONTHBK, static_cast<LPARAM>(col));

		// Set the color used to display text within a month.
		if (xflags[TEXT('t')])
			MonthCal_SetColor(m_Hwnd, MCSC_TEXT, static_cast<LPARAM>(col));

		// Set the background color displayed in the calendar's title and selection color.
		if (xflags[TEXT('i')])
			MonthCal_SetColor(m_Hwnd, MCSC_TITLEBK, static_cast<LPARAM>(col));

		// Set the color used to display text within the calendar's title.
		if (xflags[TEXT('a')])
			MonthCal_SetColor(m_Hwnd, MCSC_TITLETEXT, static_cast<LPARAM>(col));

		// Set the color used to display header day and trailing day text. Header and trailing days are the days from the previous and following months that appear on the current month calendar.
		if (xflags[TEXT('r')])
			MonthCal_SetColor(m_Hwnd, MCSC_TRAILINGTEXT, static_cast<LPARAM>(col));
	}
	//xdid -m [NAME] [ID] [SWITCH] [MAX]
	else if (flags[TEXT('m')] && numtok > 3) {
		const auto max = input.getnexttok().to_int();	// tok 4

		MonthCal_SetMaxSelCount(m_Hwnd, max);
	}
	//xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags[TEXT('r')] && numtok > 4) {
		DWORD dflags = 0;
		SYSTEMTIME range[2];

		ZeroMemory(&range[0], sizeof(SYSTEMTIME) *2);

		if (const auto tsMin(input.getnexttok()); tsMin != TEXT("nolimit")) {
			range[0] = MircTimeToSystemTime(tsMin.to_<long>());
			dflags |= GDTR_MIN;
		}

		if (const auto tsMax(input.getnexttok()); tsMax != TEXT("nolimit")) {
			range[1] = MircTimeToSystemTime(tsMax.to_<long>());
			dflags |= GDTR_MAX;
		}

		MonthCal_SetRange(m_Hwnd, dflags, range);
	}
	//xdid -s [NAME] [ID] [SWITCH] [MIN] (MAX)
	else if (flags[TEXT('s')] && numtok > 3) {
		const auto min = input.getnexttok().to_<long>();	// tok 4
		SYSTEMTIME range[2];

		ZeroMemory(&range[0], sizeof(SYSTEMTIME) *2);
		range[0] = MircTimeToSystemTime(min);

		if (isStyle(MCS_MULTISELECT)) {
			// if only one date specified, select the same date
			if (numtok < 5)
				range[1] = range[0];
			else {
				const auto max = input.getnexttok().to_<long>();	// tok 5
				range[1] = MircTimeToSystemTime(max);
			}

			MonthCal_SetSelRange(m_Hwnd, range);
		}
		else
			MonthCal_SetCurSel(m_Hwnd, &(range[0]));
	}
	//xdid -t [NAME] [ID] [SWITCH] [TIMESTAMP]
	else if (flags[TEXT('t')] && numtok > 3) {
		const auto mircTime = input.getnexttok().to_<long>();	// tok 4
		const auto sysTime = MircTimeToSystemTime(mircTime);

		MonthCal_SetToday(m_Hwnd, &sysTime);
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
					//TCHAR eval[100] = { 0 };
					//
					//for (auto i = decltype(iMax){0}; i < iMax; i++) {
					//	// daystate ctrlid startdate
					//	evalAliasEx(&eval[0], Dcx::countof(eval), TEXT("daystate,%u,%d"), getUserID(), SystemTimeToMircTime(&(lpNMDayState->stStart)));
					//	m_MonthDayStates[i] = (MONTHDAYSTATE) 0;
					//	// Ook: this needs looked at, see if we can remove eval or strDays, cant need both...
					//	TString strDays(eval);
					//
					//	strDays.trim();
					//
					//	for (auto itStart = strDays.begin(TSCOMMACHAR), itEnd = strDays.end(); itStart != itEnd; ++itStart)
					//		BOLDDAY(m_MonthDayStates[i], (*itStart).to_int());
					//
					//	// increment the month so we get a proper offset
					//	lpNMDayState->stStart.wMonth++;
					//
					//	if (lpNMDayState->stStart.wMonth > 12) {
					//		lpNMDayState->stStart.wMonth = 1;
					//		lpNMDayState->stStart.wYear++;
					//	}
					//}

					for (auto i = decltype(iMax){0}; i < iMax; i++) {
						// daystate ctrlid startdate
						const TString strDays(m_pParentDialog->evalAliasT(TEXT("daystate,%,%"), getUserID(), SystemTimeToMircTime(&(lpNMDayState->stStart))).second.trim());
						m_MonthDayStates[i] = static_cast<MONTHDAYSTATE>(0);

						for (auto itStart = strDays.begin(TSCOMMACHAR), itEnd = strDays.end(); itStart != itEnd; ++itStart)
							BOLDDAY(m_MonthDayStates[i], (*itStart).to_int());

						// increment the month so we get a proper offset
						lpNMDayState->stStart.wMonth++;

						if (lpNMDayState->stStart.wMonth > 12) {
							lpNMDayState->stStart.wMonth = 1;
							lpNMDayState->stStart.wYear++;
						}
					}

					lpNMDayState->prgDayState = &m_MonthDayStates[0];
					bParsed = TRUE;
					return FALSE;
				}

				case MCN_SELCHANGE: {
					execAliasEx(TEXT("selchange,%u"), getUserID());
					break;
				}
				case MCN_SELECT: {
					// specific code to handle multiselect dates
					if (isStyle(MCS_MULTISELECT)) {
						// get the selected range
						SYSTEMTIME selrange[2] = { 0 };

						MonthCal_GetSelRange(m_Hwnd, selrange);

						// send event to callback
						execAliasEx(TEXT("select,%u,%d,%d"), getUserID(),
							SystemTimeToMircTime(&(selrange[0])),
							SystemTimeToMircTime(&(selrange[1])));
					}
					// code to handle single selected dates
					else {
						SYSTEMTIME st = { 0 };

						//ZeroMemory(&st, sizeof(SYSTEMTIME));
						MonthCal_GetCurSel(m_Hwnd, &st);

						st.wHour = 0;
						st.wMinute = 0;
						st.wSecond = 0;
						st.wMilliseconds = 0;

						// send event to callback
						execAliasEx(TEXT("select,%u,%d"), getUserID(), SystemTimeToMircTime(&st));
					}

					break;
				}
				case NM_RELEASEDCAPTURE: {
					if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
						execAliasEx(TEXT("sclick,%u"), getUserID());
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
			if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("lbup,%u"), getUserID( ) );
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
		return CommonMessage( uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}
