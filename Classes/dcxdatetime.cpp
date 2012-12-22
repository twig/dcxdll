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

DcxDateTime::DcxDateTime(UINT ID, DcxDialog *p_Dialog, HWND mParentHwnd, RECT *rc, const TString &styles)
: DcxControl(ID, p_Dialog) 
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	this->m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CLIENTEDGE,
		DCX_DATETIMECLASS,
		NULL,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw TEXT("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(this->m_Hwnd , L" ", L" ");

	this->setControlFont((HFONT) GetStockObject(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */
DcxDateTime::~DcxDateTime() {
	this->unregistreDefaultWindowProc();
}

void DcxDateTime::toXml(TiXmlElement * xml) const
{
	char buf[64];
	SYSTEMTIME st;

	ZeroMemory(&st, sizeof(SYSTEMTIME));

	DateTime_GetSystemtime(this->m_Hwnd, &st);
	wnsprintfA(buf, 64, "%ld", SystemTimeToMircTime(&st));
	__super::toXml(xml);
	xml->SetAttribute("caption", buf);
	return;
}

TString DcxDateTime::getStyles(void) const
{
	TString styles(__super::getStyles());
	const DWORD Styles = GetWindowStyle(this->m_Hwnd);

	if (Styles & DTS_LONGDATEFORMAT)
		styles.addtok(TEXT("longdateformat"));
	if (Styles & DTS_SHORTDATEFORMAT)
		styles.addtok(TEXT("shortdateformat"));
	if (Styles & DTS_SHORTDATECENTURYFORMAT)
		styles.addtok(TEXT("shortdatecenturyformat"));
	if (Styles & DTS_TIMEFORMAT)
		styles.addtok(TEXT("timeformat"));
	if (Styles & DTS_RIGHTALIGN)
		styles.addtok(TEXT("right"));
	if (Styles & DTS_SHOWNONE)
		styles.addtok(TEXT("shownone"));
	if (Styles & DTS_UPDOWN)
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
	//const UINT numtok = styles.numtok();

	//for (UINT i = 1; i <= numtok; i++)
	//{
	//	if (styles.gettok(i) == TEXT("longdateformat"))
	//		*Styles |= DTS_LONGDATEFORMAT;
	//	else if (styles.gettok(i) == TEXT("shortdateformat"))
	//		*Styles |= DTS_SHORTDATEFORMAT;
	//	else if (styles.gettok(i) == TEXT("shortdatecenturyformat"))
	//		*Styles |= DTS_SHORTDATECENTURYFORMAT;
	//	else if (styles.gettok(i) == TEXT("timeformat"))
	//		*Styles |= DTS_TIMEFORMAT;
	//	else if (styles.gettok(i) == TEXT("right"))
	//		*Styles |= DTS_RIGHTALIGN;
	//	else if (styles.gettok(i) == TEXT("shownone"))
	//		*Styles |= DTS_SHOWNONE;
	//	else if (styles.gettok(i) == TEXT("updown"))
	//		*Styles |= DTS_UPDOWN;
	//}
	for (TString tsStyle(styles.getfirsttok( 1 )); tsStyle != ""; tsStyle = styles.getnexttok( ))
	{
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
void DcxDateTime::parseInfoRequest( const TString &input, PTCHAR szReturnValue) const {

	const TString prop(input.getfirsttok( 3 ));

	// [NAME] [ID] [PROP]
	if (prop == TEXT("range")) {
		SYSTEMTIME st[2];
		TString dmin(TEXT("nolimit"));
		TString dmax(TEXT("nolimit"));

		ZeroMemory(st, sizeof(SYSTEMTIME) *2);

		const DWORD val = DateTime_GetRange(this->m_Hwnd, st);

		if (val & GDTR_MIN)
			dmin.tsprintf(TEXT("%ld"), SystemTimeToMircTime(&(st[0])));

		if (val & GDTR_MAX)
			dmax.tsprintf(TEXT("%ld"), SystemTimeToMircTime(&(st[1])));

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%s %s"), dmin.to_chr(), dmax.to_chr()); // going to be within 900 limit anyway.
		return;
	}
	else if (prop == TEXT("value")) {
		SYSTEMTIME st;

		ZeroMemory(&st, sizeof(SYSTEMTIME));

		DateTime_GetSystemtime(this->m_Hwnd, &st);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ld"), SystemTimeToMircTime(&st));
		return;
	}
	else if (this->parseGlobalInfoRequest(input, szReturnValue))
		return;

	szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */
// TODO: find a way to change state of checkbox /xdid -c
void DcxDateTime::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));

	const UINT numtok = input.numtok();

	// xdid -f [NAME] [ID] [SWITCH] (FORMAT)
	if (flags[TEXT('f')]) {
		if (numtok > 3) {
			const TString format(input.gettok(4, -1));
			DateTime_SetFormat(this->m_Hwnd, format.to_chr());
		}
		else {
			DateTime_SetFormat(this->m_Hwnd, NULL);
			return;
		}
	}
	//xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags[TEXT('r')] && numtok > 4) {
		DWORD dflags = 0;
		SYSTEMTIME range[2];

		ZeroMemory(range, sizeof(SYSTEMTIME) *2);

		const TString tsMin(input.getnexttok( ));	// tok 4
		const TString tsMax(input.getnexttok( ));	// tok 5

		if (tsMin != TEXT("nolimit")) {
			const long min = (long) tsMin.to_num();
			range[0] = MircTimeToSystemTime(min);
			dflags |= GDTR_MIN;
		}

		if (tsMax != TEXT("nolimit")) {
			const long max = (long) tsMax.to_num();
			range[1] = MircTimeToSystemTime(max);
			dflags |= GDTR_MAX;
		}

		DateTime_SetRange(this->m_Hwnd, dflags, range);
	}
	//xdid -t [NAME] [ID] [SWITCH] [TIMESTAMP]
	else if (flags[TEXT('t')] && numtok > 3) {
		const TString ts(input.getnexttok( ));	// tok 4

		if (ts == TEXT("reset")) {
			if (isStyle(DTS_SHOWNONE))
				DateTime_SetSystemtime(this->m_Hwnd, GDT_NONE, NULL);
		}
		else {
			const long mircTime = (long) ts.to_num();
			const SYSTEMTIME sysTime = MircTimeToSystemTime(mircTime);

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
	switch (uMsg) {
		case WM_NOTIFY: {
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			switch(hdr->code) {
				case DTN_CLOSEUP: {
					this->execAliasEx(TEXT("%s,%d"), TEXT("closed"), this->getUserID());
					break;
				}

				case DTN_DROPDOWN:
				{
					/*
					TCHAR cBuff[256];
					TString buff(cBuff);

					this->callAliasEx(buff.to_chr(), TEXT("%s,%d"), TEXT("open"), this->getUserID());

					if (buff.len() > 0) {
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
							HWND cal = DateTime_GetMonthCal(this->m_Hwnd);
							SetWindowLong(cal, GWL_STYLE, styles);
						}
					}

					mIRCDebug(TEXT("returned calendar styles: %s"), buff.to_chr());
					*/

					// TODO: allow for calendar customisation. see DTN_DROPDOWN http://msdn2.microsoft.com/en-us/library/bb761739.aspx
					this->execAliasEx(TEXT("%s,%d"), TEXT("open"), this->getUserID());
					break;
				}

				case DTN_DATETIMECHANGE: {
					LPNMDATETIMECHANGE dtc = (LPNMDATETIMECHANGE) lParam;

					if (dtc->dwFlags == GDT_NONE)
						this->execAliasEx(TEXT("%s,%d,%s"), TEXT("change"), this->getUserID(), TEXT("none"));
					else
						this->execAliasEx(TEXT("%s,%d,%d"), TEXT("change"), this->getUserID(), SystemTimeToMircTime(&(dtc->st)));

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
