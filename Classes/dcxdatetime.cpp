/*!
Documentation
http://msdn2.microsoft.com/en-us/library/bb761727.aspx

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
#include "defines.h"
#include "Classes/dcxdatetime.h"
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

DcxDateTime::DcxDateTime(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString &styles)
	: DcxControl(ID, p_Dialog)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	m_Hwnd = CreateWindowEx(
		static_cast<DWORD>(ExStyles) | WS_EX_CLIENTEDGE,
		DCX_DATETIMECLASS,
		nullptr,
		WS_CHILD | static_cast<DWORD>(Styles),
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	this->setControlFont((HFONT)GetStockObject(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */
DcxDateTime::~DcxDateTime() {
	this->unregistreDefaultWindowProc();
}

void DcxDateTime::toXml(TiXmlElement *const xml) const
{
	char buf[64];
	SYSTEMTIME st{};

	//ZeroMemory(&st, sizeof(SYSTEMTIME));

	DateTime_GetSystemtime(m_Hwnd, &st);
	wnsprintfA(buf, static_cast<int>(Dcx::countof(buf)), "%ld", SystemTimeToMircTime(&st));
	__super::toXml(xml);
	xml->SetAttribute("caption", buf);
	xml->SetAttribute("styles", getStyles().c_str());

	return;
}

TiXmlElement * DcxDateTime::toXml(void) const
{
	auto xml = __super::toXml();

	char buf[64];
	SYSTEMTIME st{};

	DateTime_GetSystemtime(m_Hwnd, &st);
	wnsprintfA(buf, static_cast<int>(Dcx::countof(buf)), "%ld", SystemTimeToMircTime(&st));

	xml->SetAttribute("caption", buf);
	xml->SetAttribute("styles", getStyles().c_str());

	return xml;
}

const TString DcxDateTime::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, DTS_LONGDATEFORMAT))
		styles.addtok(TEXT("longdateformat"));
	if (dcx_testflag(Styles, DTS_SHORTDATEFORMAT))
		styles.addtok(TEXT("shortdateformat"));
	if (dcx_testflag(Styles, DTS_SHORTDATECENTURYFORMAT))
		styles.addtok(TEXT("shortdatecenturyformat"));
	if (dcx_testflag(Styles, DTS_TIMEFORMAT))
		styles.addtok(TEXT("timeformat"));
	if (dcx_testflag(Styles, DTS_RIGHTALIGN))
		styles.addtok(TEXT("right"));
	if (dcx_testflag(Styles, DTS_SHOWNONE))
		styles.addtok(TEXT("shownone"));
	if (dcx_testflag(Styles, DTS_UPDOWN))
		styles.addtok(TEXT("updown"));

	return styles;
}

/*!
 * \brief blah
 *
 * blah
 */
void DcxDateTime::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
{
	for (const auto &tsStyle: styles)
	{
#if DCX_USE_HASHING
		switch (std::hash<TString>{}(tsStyle))
		{
			case L"longdateformat"_hash:
				*Styles |= DTS_LONGDATEFORMAT;
				break;
			case L"shortdateformat"_hash:
				*Styles |= DTS_SHORTDATEFORMAT;
				break;
			case L"shortdatecenturyformat"_hash:
				*Styles |= DTS_SHORTDATECENTURYFORMAT;
				break;
			case L"timeformat"_hash:
				*Styles |= DTS_TIMEFORMAT;
				break;
			case L"right"_hash:
				*Styles |= DTS_RIGHTALIGN;
				break;
			case L"shownone"_hash:
				*Styles |= DTS_SHOWNONE;
				break;
			case L"updown"_hash:
				*Styles |= DTS_UPDOWN;
			default:
				break;
		}
#else
		if (tsStyle == TEXT("longdateformat"))
			*Styles |= DTS_LONGDATEFORMAT;
		else if (tsStyle == TEXT("shortdateformat"))
			*Styles |= DTS_SHORTDATEFORMAT;
		else if (tsStyle == TEXT("shortdatecenturyformat"))
			*Styles |= DTS_SHORTDATECENTURYFORMAT;
		else if (tsStyle == TEXT("timeformat"))
			*Styles |= DTS_TIMEFORMAT;
		else if (tsStyle == TEXT("right"))
			*Styles |= DTS_RIGHTALIGN;
		else if (tsStyle == TEXT("shownone"))
			*Styles |= DTS_SHOWNONE;
		else if (tsStyle == TEXT("updown"))
			*Styles |= DTS_UPDOWN;
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
void DcxDateTime::parseInfoRequest( const TString &input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP]
	if (prop == TEXT("range")) {
		SYSTEMTIME st[2];
		TString dmin(TEXT("nolimit"));
		TString dmax(TEXT("nolimit"));

		ZeroMemory(st, sizeof(SYSTEMTIME) *2);

		const auto val = DateTime_GetRange(m_Hwnd, st);

		if (dcx_testflag(val, GDTR_MIN))
			dmin.tsprintf(TEXT("%ld"), SystemTimeToMircTime(&(st[0])));

		if (dcx_testflag(val, GDTR_MAX))
			dmax.tsprintf(TEXT("%ld"), SystemTimeToMircTime(&(st[1])));

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%s %s"), dmin.to_chr(), dmax.to_chr()); // going to be within 900 limit anyway.
	}
	else if (prop == TEXT("value")) {
		SYSTEMTIME st;

		ZeroMemory(&st, sizeof(SYSTEMTIME));

		DateTime_GetSystemtime(m_Hwnd, &st);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ld"), SystemTimeToMircTime(&st));
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */
// TODO: find a way to change state of checkbox /xdid -c
void DcxDateTime::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));

	const auto numtok = input.numtok();

	// xdid -f [NAME] [ID] [SWITCH] (FORMAT)
	if (flags[TEXT('f')]) {
		if (numtok > 3) {
			const auto format(input.getlasttoks());	// tok 4, -1
			DateTime_SetFormat(m_Hwnd, format.to_chr());
		}
		else
			DateTime_SetFormat(m_Hwnd, nullptr);
	}
	//xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags[TEXT('r')] && numtok > 4) {
		DWORD dflags = 0;
		SYSTEMTIME range[2];

		ZeroMemory(range, sizeof(SYSTEMTIME) *2);

		const auto tsMin(input++);	// tok 4
		const auto tsMax(input++);	// tok 5

		if (tsMin != TEXT("nolimit")) {
			const auto min = tsMin.to_<long>();
			range[0] = MircTimeToSystemTime(min);
			dflags |= GDTR_MIN;
		}

		if (tsMax != TEXT("nolimit")) {
			const auto max = tsMax.to_<long>();
			range[1] = MircTimeToSystemTime(max);
			dflags |= GDTR_MAX;
		}

		DateTime_SetRange(m_Hwnd, dflags, range);
	}
	//xdid -t [NAME] [ID] [SWITCH] [TIMESTAMP]
	else if (flags[TEXT('t')] && numtok > 3) {
		const auto ts(input++);	// tok 4

		if (ts == TEXT("reset")) {
			if (isStyle(DTS_SHOWNONE))
				DateTime_SetSystemtime(m_Hwnd, GDT_NONE, nullptr);
		}
		else {
			const auto mircTime = ts.to_<long>();
			const auto sysTime = MircTimeToSystemTime(mircTime);

			DateTime_SetSystemtime(m_Hwnd, GDT_VALID, &sysTime);
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
	switch (uMsg) {
		case WM_NOTIFY: {
			dcxlParam(LPNMHDR, hdr);

			if (hdr == nullptr)
				break;

			switch(hdr->code) {
				case DTN_CLOSEUP: {
					this->execAliasEx(TEXT("closed,%d"), getUserID());
					break;
				}

				case DTN_DROPDOWN:
				{
					/*
					TCHAR cBuff[256];
					TString buff(cBuff);

					this->callAliasEx(buff.to_chr(), TEXT("%s,%d"), TEXT("open"), this->getUserID());

					if (!buff.empty()) {
						int i = 1;
						int numtok = buff.numtok();
						TString style;
						UINT styles;
						bool foundValid = false;

						while (i <= numtok) {
							style = buff.gettok(i);

							if (style == TEXT("multi")) {
								styles |= MCS_MULTISELECT;
								foundValid = true;
							}
							else if (style == TEXT("notoday")) {
								styles |= MCS_NOTODAY;
								foundValid = true;
							}
							else if (style == TEXT("notodaycircle")) {
								styles |= MCS_NOTODAYCIRCLE;
								foundValid = true;
							}
							else if (style == TEXT("weeknum")) {
								styles |= MCS_WEEKNUMBERS;
								foundValid = true;
							}
							else if (style == TEXT("daystate")) {
								styles |= MCS_DAYSTATE;
								foundValid = true;
							}

							i++;
						}

						if (foundValid) {
							HWND cal = DateTime_GetMonthCal(m_Hwnd);
							SetWindowLong(cal, GWL_STYLE, styles);
						}
					}

					mIRCDebug(TEXT("returned calendar styles: %s"), buff.to_chr());
					*/

					// TODO: allow for calendar customisation. see DTN_DROPDOWN http://msdn2.microsoft.com/en-us/library/bb761739.aspx
					execAliasEx(TEXT("open,%d"), getUserID());
					break;
				}

				case DTN_DATETIMECHANGE: {
					dcxlParam(LPNMDATETIMECHANGE, dtc);

					if (dtc->dwFlags == GDT_NONE)
						execAliasEx(TEXT("change,%d,none"), getUserID());
					else
						execAliasEx(TEXT("change,%d,%d"), getUserID(), SystemTimeToMircTime(&(dtc->st)));

					return 0L;
				}
			} // end switch
		}
	}

	return 0L;
}

LRESULT DcxDateTime::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) {
	switch (uMsg) {

		case WM_DESTROY:
			delete this;
			bParsed = TRUE;
			break;

		default:
			return this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return 0L;
}
