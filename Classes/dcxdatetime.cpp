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

#include "dcxdatetime.h"
#include "dcxdialog.h"

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
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw "Unable To Create Window";

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
	//int numtok = input.numtok();

	TString prop(input.gettok( 3 ));

	// [NAME] [ID] [PROP]
	if (prop == "range") {
		SYSTEMTIME st[2];
		TString min;
		TString max;
		DWORD val;

		ZeroMemory(st, sizeof(SYSTEMTIME) *2);

		val = DateTime_GetRange(this->m_Hwnd, st);

		if (val & GDTR_MIN)
			min.sprintf("%ld", SystemTimeToMircTime(&(st[0])));
		else
			min = "nolimit";

		if (val & GDTR_MAX)
			max.sprintf("%ld", SystemTimeToMircTime(&(st[1])));
		else
			max = "nolimit";

		wsprintf(szReturnValue, "%s %s", min.to_chr(), max.to_chr()); // going to be within 900 limit anyway.
		return;
	}
	else if (prop == "value") {
		SYSTEMTIME st;

		ZeroMemory(&st, sizeof(SYSTEMTIME));

		DateTime_GetSystemtime(this->m_Hwnd, &st);
		wsprintf(szReturnValue, "%ld", SystemTimeToMircTime(&st));
		return;
	}
	else if (this->parseGlobalInfoRequest(input, szReturnValue)) {
		return;
	}

	szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */
// TODO: find a way to change state of checkbox /xdid -c
void DcxDateTime::parseCommandRequest(TString &input) {
	XSwitchFlags flags(input.gettok(3));

	int numtok = input.numtok();

	// xdid -f [NAME] [ID] [SWITCH] (FORMAT)
	if (flags['f']) {
		if (numtok > 3) {
			TString format(input.gettok(4, -1));
			DateTime_SetFormat(this->m_Hwnd, format.to_chr());
		}
		else {
			DateTime_SetFormat(this->m_Hwnd, NULL);
			return;
		}
	}
	//xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags['r'] && numtok > 4) {
		DWORD dflags = 0;
		SYSTEMTIME range[2];

		ZeroMemory(range, sizeof(SYSTEMTIME) *2);

		if (input.gettok(4) != "nolimit") {
			long min = (long) input.gettok(4).to_num();
			range[0] = MircTimeToSystemTime(min);
			dflags |= GDTR_MIN;
		}

		if (input.gettok(5) != "nolimit") {
			long max = (long) input.gettok(5).to_num();
			range[1] = MircTimeToSystemTime(max);
			dflags |= GDTR_MAX;
		}

		DateTime_SetRange(this->m_Hwnd, dflags, range);
	}
	//xdid -t [NAME] [ID] [SWITCH] [TIMESTAMP]
	else if (flags['t'] && numtok > 3) {
		TString ts(input.gettok(4));

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
	switch (uMsg) {
		case WM_NOTIFY: {
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			switch(hdr->code) {
				case DTN_CLOSEUP: {
					this->callAliasEx(NULL, "%s,%d", "closed", this->getUserID());
					break;
				}

				case DTN_DROPDOWN:
				{
					/*
					char cBuff[256];
					TString buff(cBuff);

					this->callAliasEx(buff.to_chr(), "%s,%d", "open", this->getUserID());

					if (buff.len() > 0) {
						int i = 1;
						int numtok = buff.numtok();
						TString style;
						UINT styles;
						bool foundValid = false;

						while (i <= numtok) {
							style = buff.gettok(i);

							if (style == "multi") {
								styles |= MCS_MULTISELECT;
								foundValid = true;
							}
							else if (style == "notoday") {
								styles |= MCS_NOTODAY;
								foundValid = true;
							}
							else if (style == "notodaycircle") {
								styles |= MCS_NOTODAYCIRCLE;
								foundValid = true;
							}
							else if (style == "weeknum") {
								styles |= MCS_WEEKNUMBERS;
								foundValid = true;
							}
							else if (style == "daystate") {
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

					mIRCDebug("returned calendar styles: %s", buff.to_chr());
					*/

					// TODO: allow for calendar customisation. see DTN_DROPDOWN http://msdn2.microsoft.com/en-us/library/bb761739.aspx
					this->callAliasEx(NULL, "%s,%d", "open", this->getUserID());
					break;
				}

				case DTN_DATETIMECHANGE: {
					LPNMDATETIMECHANGE dtc = (LPNMDATETIMECHANGE) lParam;

					if (dtc->dwFlags == GDT_NONE)
						this->callAliasEx(NULL, "%s,%d,%s", "change", this->getUserID(), "none");
					else
						this->callAliasEx(NULL, "%s,%d,%d", "change", this->getUserID(), SystemTimeToMircTime(&(dtc->st)));

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
