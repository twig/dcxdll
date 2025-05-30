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
 * � ScriptsDB.org - 2006
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
DcxCalendar::DcxCalendar(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog, DcxControlTypes::CALENDAR)
	, m_MonthDayStates()
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WindowExStyle::ClientEdge,
		DCX_CALENDARCLASS,
		ws.m_Styles | WindowStyle::Child,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		DcxUXModule::dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxCalendar::~DcxCalendar() noexcept
{
}

void DcxCalendar::toXml(TiXmlElement* const xml) const
{
	if (!xml)
		return;

	__super::toXml(xml);

	{
		const TString wtext(TGetWindowText(m_Hwnd));
		xml->SetAttribute("caption", wtext.c_str());
	}
	xml->SetAttribute("styles", getStyles().c_str());
	xml->SetAttribute("value", getValue().c_str());
	xml->SetAttribute("maxselcount", MonthCal_GetMaxSelCount(m_Hwnd));
	//xml->SetAttribute("range", MonthCal_GetMaxSelCount(m_Hwnd));

	{
		TiXmlElement xColours("calcolours");

		xColours.SetAttribute("background", gsl::narrow_cast<COLORREF>(MonthCal_GetColor(m_Hwnd, MCSC_BACKGROUND)));
		xColours.SetAttribute("monthbk", gsl::narrow_cast<COLORREF>(MonthCal_GetColor(m_Hwnd, MCSC_MONTHBK)));
		xColours.SetAttribute("text", gsl::narrow_cast<COLORREF>(MonthCal_GetColor(m_Hwnd, MCSC_TEXT)));
		xColours.SetAttribute("titlebk", gsl::narrow_cast<COLORREF>(MonthCal_GetColor(m_Hwnd, MCSC_TITLEBK)));
		xColours.SetAttribute("titletext", gsl::narrow_cast<COLORREF>(MonthCal_GetColor(m_Hwnd, MCSC_TITLETEXT)));
		xColours.SetAttribute("trailingtext", gsl::narrow_cast<COLORREF>(MonthCal_GetColor(m_Hwnd, MCSC_TRAILINGTEXT)));

		xml->InsertEndChild(xColours);
	}


}

TiXmlElement* DcxCalendar::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxCalendar::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

}

const TString DcxCalendar::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

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
	long start{}, end{};

	if (isStyle(WindowStyle::MCS_MultiSelect))
	{
		SYSTEMTIME st[2]{};

		MonthCal_GetSelRange(m_Hwnd, &st[0]);

		start = SystemTimeToMircTime(&(st[0]));
		end = SystemTimeToMircTime(&(st[1]));
	}
	else {
		//SYSTEMTIME st{};
		//
		//MonthCal_GetCurSel(m_Hwnd, &st);
		//
		//st.wHour = 0;
		//st.wMinute = 0;
		//st.wSecond = 0;
		//st.wMilliseconds = 0;
		//
		//start = SystemTimeToMircTime(&st);
		//end = start;

		start = end = getCurrentCTime();
	}

	stString<128> buf;

	_ts_snprintf(buf, TEXT("%ld %ld"), start, end);
	return TString(buf.data());
}

long DcxCalendar::getCurrentCTime() const
{
	if (!m_Hwnd)
		return 0;

	SYSTEMTIME st{};

	if (MonthCal_GetCurSel(m_Hwnd, &st) == 0)	// always fails if control has multiselect style.
		return 0;

	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
	st.wMilliseconds = 0;

	return SystemTimeToMircTime(&st);
}

dcxWindowStyles DcxCalendar::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"multi"_hash:
			ws.m_Styles |= MCS_MULTISELECT;
			break;
		case L"notoday"_hash:
			ws.m_Styles |= MCS_NOTODAY;
			break;
		case L"notodaycircle"_hash:
			ws.m_Styles |= MCS_NOTODAYCIRCLE;
			break;
		case L"weeknum"_hash:
			ws.m_Styles |= MCS_WEEKNUMBERS;
			break;
		case L"daystate"_hash:
			ws.m_Styles |= MCS_DAYSTATE;
			break;
		default:
			break;
		}
	}

	return parseGeneralControlStyles(tsStyles, ws);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxCalendar::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"value"_hash:
		szReturnValue = getValue().to_chr();
		break;
	case L"text"_hash:
	{
		GetWindowText(m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	break;
	case L"range"_hash:
	{
		SYSTEMTIME st[2]{};
		TString dmin(TEXT("nolimit"_ts));
		TString dmax(TEXT("nolimit"_ts));

		const auto val = MonthCal_GetRange(m_Hwnd, st);

		if (dcx_testflag(val, GDTR_MIN))
			dmin.tsprintf(TEXT("%ld"), SystemTimeToMircTime(&(st[0])));

		if (dcx_testflag(val, GDTR_MAX))
			dmax.tsprintf(TEXT("%ld"), SystemTimeToMircTime(&(st[1])));

		_ts_snprintf(szReturnValue, TEXT("%s %s"), dmin.to_chr(), dmax.to_chr()); // going to be within 900 limit anyway.
	}
	break;
	case L"today"_hash:
	{
		SYSTEMTIME st{};

		MonthCal_GetToday(m_Hwnd, &st);
		_ts_snprintf(szReturnValue, TEXT("%ld"), SystemTimeToMircTime(&st));
	}
	break;
	case L"selcount"_hash:
		_ts_snprintf(szReturnValue, TEXT("%u"), MonthCal_GetMaxSelCount(m_Hwnd));
		break;

	case L"border"_hash:
		_ts_snprintf(szReturnValue, TEXT("%u"), MonthCal_GetCalendarBorder(m_Hwnd));
		break;

	case L"count"_hash:
		_ts_snprintf(szReturnValue, TEXT("%u"), MonthCal_GetCalendarCount(m_Hwnd));
		break;

	case L"view"_hash:

		//	MCMV_MONTH Monthly view.	zero
		//	MCMV_YEAR Annual view.		one
		//	MCMV_DECADE Decade view.	two
		//	MCMV_CENTURY Century view.	three

		_ts_snprintf(szReturnValue, TEXT("%u"), MonthCal_GetCurrentView(m_Hwnd));
		break;

	case L"sel"_hash:
		_ts_snprintf(szReturnValue, TEXT("%u"), getCurrentCTime());
		break;

	case L"firstdayofweek"_hash:
	{
		auto iFirst = MonthCal_GetFirstDayOfWeek(m_Hwnd);
		TString tsDay(TEXT("Monday"));
		switch (Dcx::dcxLOWORD(iFirst))
		{
		case 0:
			tsDay = TEXT("Monday");
			break;
		case 1:
			tsDay = TEXT("Tuesday");
			break;
		case 2:
			tsDay = TEXT("Wednesday");
			break;
		case 3:
			tsDay = TEXT("Thursday");
			break;
		case 4:
			tsDay = TEXT("Friday");
			break;
		case 5:
			tsDay = TEXT("Saturday");
			break;
		case 6:
			tsDay = TEXT("Sunday");
			break;
		default:
			break;
		}
		_ts_snprintf(szReturnValue, TEXT("%u %s"), Dcx::dcxHIWORD(iFirst), tsDay.to_chr());
	}
	break;

	case TEXT("calcolour"_hash):
	case TEXT("calcolor"_hash):
	{
		if (const auto hashType = std::hash<TString>()(input.getnexttok()); hashType == L"all"_hash)
		{
			_ts_snprintf(szReturnValue, TEXT("%ld %ld %ld %ld %ld %ld"),
				MonthCal_GetColor(m_Hwnd, MCSC_BACKGROUND),
				MonthCal_GetColor(m_Hwnd, MCSC_MONTHBK),
				MonthCal_GetColor(m_Hwnd, MCSC_TEXT),
				MonthCal_GetColor(m_Hwnd, MCSC_TITLEBK),
				MonthCal_GetColor(m_Hwnd, MCSC_TITLETEXT),
				MonthCal_GetColor(m_Hwnd, MCSC_TRAILINGTEXT));
		}
		else {
			int iType{ MCSC_BACKGROUND };

			switch (hashType)
			{
			default:
			case TEXT("background"_hash):
				iType = MCSC_BACKGROUND;
				break;
			case TEXT("monthbk"_hash):
				iType = MCSC_MONTHBK;
				break;
			case TEXT("text"_hash):
				iType = MCSC_TEXT;
				break;
			case TEXT("titlebk"_hash):
				iType = MCSC_TITLEBK;
				break;
			case TEXT("titletext"_hash):
				iType = MCSC_TITLETEXT;
				break;
			case TEXT("trailingtext"_hash):
				iType = MCSC_TRAILINGTEXT;
				break;
			}

			const auto clr = gsl::narrow_cast<COLORREF>(MonthCal_GetColor(m_Hwnd, iType));

			_ts_snprintf(szReturnValue, TEXT("%ld"), clr);
		}
	}
	break;

	default:
		parseGlobalInfoRequest(input, szReturnValue);
		break;
	}
}

/*!
 * \brief blah
 *
 * blah
 */
void DcxCalendar::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));

	//SetDayState

	const auto numtok = input.numtok();

	// xdid -k [NAME] [ID] [SWITCH] [+FLAGS] [$RGB]
	// xdid -k [NAME] [ID] [SWITCH] [+A] [$RGB] [$RGB] [$RGB] [$RGB] [$RGB] [$RGB]
	if (flags[TEXT('k')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xflags(input.getnexttok());	// tok 4
		const auto col = input.getnexttok().to_<COLORREF>();	// tok 5

		// Set ALL colours at once
		if (xflags[TEXT('A')])
		{
			MonthCal_SetColor(m_Hwnd, MCSC_BACKGROUND, col);
			MonthCal_SetColor(m_Hwnd, MCSC_MONTHBK, input.getnexttokas<COLORREF>());
			MonthCal_SetColor(m_Hwnd, MCSC_TEXT, input.getnexttokas<COLORREF>());
			MonthCal_SetColor(m_Hwnd, MCSC_TITLEBK, input.getnexttokas<COLORREF>());
			MonthCal_SetColor(m_Hwnd, MCSC_TITLETEXT, input.getnexttokas<COLORREF>());
			MonthCal_SetColor(m_Hwnd, MCSC_TRAILINGTEXT, input.getnexttokas<COLORREF>());

			return;
		}

		// Set the background color displayed between months.
		if (xflags[TEXT('b')])
			MonthCal_SetColor(m_Hwnd, MCSC_BACKGROUND, gsl::narrow_cast<LPARAM>(col));

		// Set the background color displayed within the month.
		if (xflags[TEXT('g')])
			MonthCal_SetColor(m_Hwnd, MCSC_MONTHBK, gsl::narrow_cast<LPARAM>(col));

		// Set the color used to display text within a month.
		if (xflags[TEXT('t')])
			MonthCal_SetColor(m_Hwnd, MCSC_TEXT, gsl::narrow_cast<LPARAM>(col));

		// Set the background color displayed in the calendar's title and selection color.
		if (xflags[TEXT('i')])
			MonthCal_SetColor(m_Hwnd, MCSC_TITLEBK, gsl::narrow_cast<LPARAM>(col));

		// Set the color used to display text within the calendar's title.
		if (xflags[TEXT('a')])
			MonthCal_SetColor(m_Hwnd, MCSC_TITLETEXT, gsl::narrow_cast<LPARAM>(col));

		// Set the color used to display header day and trailing day text. Header and trailing days are the days from the previous and following months that appear on the current month calendar.
		if (xflags[TEXT('r')])
			MonthCal_SetColor(m_Hwnd, MCSC_TRAILINGTEXT, gsl::narrow_cast<LPARAM>(col));
	}
	//xdid -m [NAME] [ID] [SWITCH] [MAX]
	else if (flags[TEXT('m')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto max = input.getnexttok().to_int();	// tok 4

		MonthCal_SetMaxSelCount(m_Hwnd, max);
	}
	//xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags[TEXT('r')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		DWORD dflags{};
		SYSTEMTIME range[2]{};

		if (const auto tsMin(input.getnexttok()); tsMin != TEXT("nolimit"))
		{
			range[0] = MircTimeToSystemTime(tsMin.to_<long>());
			dflags |= GDTR_MIN;
		}

		if (const auto tsMax(input.getnexttok()); tsMax != TEXT("nolimit"))
		{
			range[1] = MircTimeToSystemTime(tsMax.to_<long>());
			dflags |= GDTR_MAX;
		}

		MonthCal_SetRange(m_Hwnd, dflags, range);
	}
	//xdid -s [NAME] [ID] [SWITCH] [MIN] (MAX)
	else if (flags[TEXT('s')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto min = input.getnexttok().to_<long>();	// tok 4
		SYSTEMTIME range[2]{};

		range[0] = MircTimeToSystemTime(min);

		if (isStyle(WindowStyle::MCS_MultiSelect))
		{
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
	else if (flags[TEXT('t')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto mircTime = input.getnexttok().to_<long>();	// tok 4
		const auto sysTime = MircTimeToSystemTime(mircTime);

		MonthCal_SetToday(m_Hwnd, &sysTime);
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxCalendar::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	const auto pDialog = getParentDialog();
	if (!pDialog)
		return 0L;	// something went very wrong...

	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (!hdr)
			break;

		switch (hdr->code)
		{
		case MCN_GETDAYSTATE:
		{
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

			GSL_SUPPRESS(bounds.3)
				for (auto i = decltype(iMax){0}; i < iMax; ++i)
			{
				// daystate ctrlid startdate
				const TString strDays(pDialog->evalAliasT(TEXT("daystate,%,%"), getUserID(), SystemTimeToMircTime(&(lpNMDayState->stStart))).second.trim());
				gsl::at(m_MonthDayStates, i) = 0;

				{
					const auto itEnd = strDays.end();
					for (auto itStart = strDays.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
						BOLDDAY(gsl::at(m_MonthDayStates, i), (*itStart).to_int());
				}

				// increment the month so we get a proper offset
				++lpNMDayState->stStart.wMonth;

				if (lpNMDayState->stStart.wMonth > 12)
				{
					lpNMDayState->stStart.wMonth = 1;
					++lpNMDayState->stStart.wYear;
				}
			}

			lpNMDayState->prgDayState = &m_MonthDayStates[0];
			bParsed = TRUE;
			return FALSE;
		}

		case MCN_SELCHANGE:
		{
			execAliasEx(TEXT("selchange,%u"), getUserID());
			break;
		}
		case MCN_SELECT:
		{
			// specific code to handle multiselect dates
			if (isStyle(WindowStyle::MCS_MultiSelect))
			{
				// get the selected range
				SYSTEMTIME selrange[2]{};

				MonthCal_GetSelRange(m_Hwnd, selrange);

				// send event to callback
				execAliasEx(TEXT("select,%u,%d,%d"), getUserID(),
					SystemTimeToMircTime(&(selrange[0])),
					SystemTimeToMircTime(&(selrange[1])));
			}
			// code to handle single selected dates
			else {
				SYSTEMTIME st{};

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
		case NM_RELEASEDCAPTURE:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("sclick,%u"), getUserID());
			break;
		}
		default:
			break;
		} // end switch
	}
	break;
	default:
		break;
	}
	return 0L;
}

LRESULT DcxCalendar::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	//const auto pDialog = getParentDialog();
	//if (!pDialog)
	//	return 0L;	// something went very wrong...

	switch (uMsg)
	{
	case WM_LBUTTONUP:
	{
		if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("lbup,%u"), getUserID());
	}
	break;

	//case WM_GETDLGCODE:
	//{
	//	return DLGC_WANTARROWS;
	//	break;
	//}

	case WM_DESTROY:
	{
		this->CallDefaultClassProc(uMsg, wParam, lParam);

		delete this;
		bParsed = TRUE;
		break;
	}

	default:
		return CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}

LRESULT DcxCalendar::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
