/*!
http://www.codeguru.com/cpp/controls/controls/dateselectioncontrolsetc/article.php/c2229/
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/sysinfo/base/converting_a_time_t_value_to_a_file_time.asp
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/monthcal/structures/nmdaystate.asp

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

#include "dcxcalendar.h"
#include "dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxCalendar::DcxCalendar( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles | WS_EX_CLIENTEDGE, 
    DCX_CALENDARCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle( NULL ), 
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxCalendar::DcxCalendar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles | WS_EX_CLIENTEDGE, 
    DCX_CALENDARCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxCalendar::~DcxCalendar( ) {

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxCalendar::parseControlStyles(TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) {
	unsigned int i = 1, numtok = styles.numtok(" ");

	while (i <= numtok) {
		if (styles.gettok(i , " ") == "multi")
			*Styles |= MCS_MULTISELECT;
		else if (styles.gettok(i , " ") == "notoday")
			*Styles |= MCS_NOTODAY;
		else if (styles.gettok(i , " ") == "notodaycircle")
			*Styles |= MCS_NOTODAYCIRCLE;
		else if (styles.gettok(i , " ") == "weeknum")
			*Styles |= MCS_WEEKNUMBERS;
		else if (styles.gettok(i , " ") == "daystate")
			*Styles |= MCS_DAYSTATE;

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

void DcxCalendar::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( " " );

  // [NAME] [ID] [PROP]
  if ( input.gettok( 3, " " ) == "text" ) {

  }
  else if ( this->parseGlobalInfoRequest( input, szReturnValue ) ) {

    return;
  }
  
  szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxCalendar::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );
//MonthCal_SetMaxSelCount(this->m_Hwnd, 7);
  // set colors
  // set cur sel
  // set day state
  // set sel range
  // set today

//  int numtok = input.numtok( " " );

  //xdid -t [NAME] [ID] [SWITCH]
  if ( flags.switch_flags[19] ) {
    TString text = input.gettok( 4, -1, " " );
    text.trim( );
    SetWindowText( this->m_Hwnd, text.to_chr( ) );
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
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			switch(hdr->code) {
				case MCN_GETDAYSTATE: {
					LPNMDAYSTATE lpNMDayState = (LPNMDAYSTATE) lParam;
					MONTHDAYSTATE mds[12];

					int iMax = lpNMDayState->cDayState;
					char eval[100];

					for (int i = 0; i < iMax; i++) {
						// daystate ctrlid startdate
						this->callAliasEx(eval, "%s,%d,%d", "daystate", this->getUserID(),
							SystemTimeToMircTime(lpNMDayState->stStart));
						mds[i] = (MONTHDAYSTATE) 0;

						TString strDays(eval);
						strDays.trim();

						for (int x = 1; x <= strDays.numtok(","); x++) {
							TString tok = strDays.gettok(x, ",");
							tok.trim();
							BOLDDAY(mds[i], tok.to_int());
						}

						// increment the month so we get a proper offset
						lpNMDayState->stStart.wMonth++;

						if (lpNMDayState->stStart.wMonth > 12) {
							lpNMDayState->stStart.wMonth = 1;
							lpNMDayState->stStart.wYear++;
						}
					}

					lpNMDayState->prgDayState = mds;
					bParsed = TRUE;
					return FALSE;
					break;
				}

				case MCN_SELCHANGE: {
					this->callAliasEx(NULL, "%s,%d", "selchange", this->getUserID());
					break;
				}
				case MCN_SELECT: {
					// specific code to handle multiselect dates
					if (this->isStyle(MCS_MULTISELECT)) {
						// get the selected range
						SYSTEMTIME selrange[2];

						MonthCal_GetSelRange(this->m_Hwnd, selrange);

						// send event to callback
						this->callAliasEx(NULL, "%s,%d,%d,%d", "select", this->getUserID(),
							SystemTimeToMircTime(selrange[0]),
							SystemTimeToMircTime(selrange[1]));
					}
					// code to handle single selected dates
					else {
						SYSTEMTIME st;
						MonthCal_GetCurSel(this->m_Hwnd, &st);

						// send event to callback
						this->callAliasEx(NULL, "%s,%d,%d", "select", this->getUserID(), SystemTimeToMircTime(st));
					}

					break;
				}
				case NM_RELEASEDCAPTURE: {
					this->callAliasEx(NULL, "%s,%d", "sclick", this->getUserID());
					break;
				}
				default: {
					//mIRCError("ELSE");
					break;
				}
			} // end switch
		}
	}
	return 0L;
}

LRESULT DcxCalendar::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) {
	switch (uMsg) {
		case WM_HELP: {
			this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
			break;
		}

		//case WM_GETDLGCODE:
		//{
		//	return DLGC_WANTARROWS;
		//	break;
		//}

		case WM_MOUSEMOVE: {
			this->m_pParentDialog->setMouseControl(this->getUserID());
			break;
		}

		case WM_SETFOCUS: {
			this->m_pParentDialog->setFocusControl(this->getUserID());
			break;
		}

		case WM_SETCURSOR: {
			if (LOWORD(lParam) == HTCLIENT && (HWND) wParam == this->m_Hwnd && this->m_hCursor != NULL) {
				SetCursor( this->m_hCursor );
				bParsed = TRUE;
				return TRUE;
			}
			break;
		}

		case WM_DESTROY: {
			delete this;
			bParsed = TRUE;
			break;
		}

		default:
			break;
	}

	return 0L;
}

long DcxCalendar::SystemTimeToMircTime(SYSTEMTIME st) {
	char ret[100];

	TString months[12] = {
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

	wsprintf(ret, "$ctime(%d %s %d)",
		st.wDay,
		months[st.wMonth -1],
		st.wYear);

	mIRCeval(ret, ret);
	return atoi(ret);
}
