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

DcxCalendar::DcxCalendar(const UINT ID, DcxDialog* const p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
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

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

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
	__super::toXml(xml);

	xml->SetAttribute("caption", getValue().c_str());
	xml->SetAttribute("styles", getStyles().c_str());
}

TiXmlElement* DcxCalendar::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
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

		MonthCal_GetSelRange(m_Hwnd, st);

		start = SystemTimeToMircTime(&(st[0]));
		end = SystemTimeToMircTime(&(st[1]));
	}
	else {
		SYSTEMTIME st{};

		MonthCal_GetCurSel(m_Hwnd, &st);

		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		st.wMilliseconds = 0;

		start = SystemTimeToMircTime(&st);
		end = start;
	}

	stString<128> buf;

	_ts_snprintf(buf, TEXT("%ld %ld"), start, end);
	return buf.data();
}

/*!
 * \brief blah
 *
 * blah
 */

 //void DcxCalendar::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme)
 //{
 //	for (const auto &tsStyle: styles)
 //	{
 //#if DCX_USE_HASHING
 //		switch (std::hash<TString>{}(tsStyle))
 //		{
 //			case L"multi"_hash:
 //				*Styles |= MCS_MULTISELECT;
 //				break;
 //			case L"notoday"_hash:
 //				*Styles |= MCS_NOTODAY;
 //				break;
 //			case L"notodaycircle"_hash:
 //				*Styles |= MCS_NOTODAYCIRCLE;
 //				break;
 //			case L"weeknum"_hash:
 //				*Styles |= MCS_WEEKNUMBERS;
 //				break;
 //			case L"daystate"_hash:
 //				*Styles |= MCS_DAYSTATE;
 //			default:
 //				break;
 //		}
 //#else
 //		if (tsStyle == TEXT("multi"))
 //			*Styles |= MCS_MULTISELECT;
 //		else if (tsStyle == TEXT("notoday"))
 //			*Styles |= MCS_NOTODAY;
 //		else if (tsStyle == TEXT("notodaycircle"))
 //			*Styles |= MCS_NOTODAYCIRCLE;
 //		else if (tsStyle == TEXT("weeknum"))
 //			*Styles |= MCS_WEEKNUMBERS;
 //		else if (tsStyle == TEXT("daystate"))
 //			*Styles |= MCS_DAYSTATE;
 //#endif
 //	}
 //
 //	this->parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
 //}

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
	case L"range"_hash:
	{
		SYSTEMTIME st[2]{};
		TString dmin(TEXT("nolimit"));
		TString dmax(TEXT("nolimit"));

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
	if (flags[TEXT('k')])
	{
		if (numtok < 5)
			throw Dcx::dcxException("Insufficient parameters");

		const XSwitchFlags xflags(input.getnexttok());	// tok 4
		const auto col = input.getnexttok().to_<COLORREF>();	// tok 5

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
			throw Dcx::dcxException("Insufficient parameters");

		const auto max = input.getnexttok().to_int();	// tok 4

		MonthCal_SetMaxSelCount(m_Hwnd, max);
	}
	//xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags[TEXT('r')])
	{
		if (numtok < 5)
			throw Dcx::dcxException("Insufficient parameters");

		DWORD dflags{};
		SYSTEMTIME range[2]{};

		//ZeroMemory(&range[0], sizeof(SYSTEMTIME) *2);

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
			throw Dcx::dcxException("Insufficient parameters");

		const auto min = input.getnexttok().to_<long>();	// tok 4
		SYSTEMTIME range[2]{};

		//ZeroMemory(&range[0], sizeof(SYSTEMTIME) * 2);
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
			throw Dcx::dcxException("Insufficient parameters");

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
				const TString strDays(getParentDialog()->evalAliasT(TEXT("daystate,%,%"), getUserID(), SystemTimeToMircTime(&(lpNMDayState->stStart))).second.trim());
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
			if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
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
	switch (uMsg)
	{
	case WM_LBUTTONUP:
	{
		if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
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
