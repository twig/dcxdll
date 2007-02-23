/*!
 * \file dcxtext.cpp
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

#include "dcxtext.h"
#include "../dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxText::DcxText( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles, 
		"STATIC", 
		NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL), 
		NULL);

	if ( bNoTheme )
		dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	if (p_Dialog->getToolTip() != NULL) {
		if (styles.istok("tooltips")) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
	}
	this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxText::~DcxText( ) {

	this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxText::parseControlStyles(TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) {
	unsigned int i = 1, numtok = styles.numtok( );
	*Styles |= SS_NOTIFY;

	while (i <= numtok) {
		if (styles.gettok( i ) == "nowrap")
			*Styles |= SS_LEFTNOWORDWRAP;
		else if (styles.gettok( i ) == "center")
			*Styles |= SS_CENTER;
		else if (styles.gettok( i ) == "right")
			*Styles |= SS_RIGHT;
		else if (styles.gettok( i ) == "noprefix")
			*Styles |= SS_NOPREFIX;
		else if (styles.gettok( i ) == "endellipsis")
			*Styles |= SS_ENDELLIPSIS;
		else if (styles.gettok( i ) == "pathellipsis")
			*Styles |= SS_PATHELLIPSIS;
		else if (styles.gettok( i ) == "transparent")
			*ExStyles |= WS_EX_TRANSPARENT;
		else if ( styles.gettok( i ) == "alpha" )
			this->m_bAlphaBlend = true;
		else if (( styles.gettok( i ) == "shadow" ))
			this->m_bShadowText = true;
		else if (( styles.gettok( i ) == "noformat" ))
			this->m_bCtrlCodeText = false;

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

void DcxText::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( );

  // [NAME] [ID] [PROP]
  if ( input.gettok( 3 ) == "text" ) {

    GetWindowText( this->m_Hwnd, szReturnValue, 900 );
    return;
  }
  else if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
    return;
  
  szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxText::parseCommandRequest(TString &input) {
	XSwitchFlags flags;
	ZeroMemory((void*) &flags, sizeof(XSwitchFlags));
	this->parseSwitchFlags(input.gettok( 3 ), &flags);

//  int numtok = input.numtok( );

	//xdid -t [NAME] [ID] [SWITCH]
	if (flags.switch_flags[19]) {
		TString text(input.gettok(4, -1));
		text.trim();
		SetWindowText(this->m_Hwnd, text.to_chr());

		// redraw if transparent
		if (this->isExStyle(WS_EX_TRANSPARENT)) {
			RECT r;
			HWND hParent = GetParent(this->m_Hwnd);

			GetWindowRect(this->m_Hwnd, &r);

			MapWindowPoints(NULL, hParent, (LPPOINT)&r, 2); // maps all 4 points & handles RTL
			InvalidateRect(hParent, &r, TRUE);
			this->redrawWindow();
		}
	}
  else
    this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxText::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
  switch( uMsg ) {
    case WM_COMMAND:
      {
        switch ( HIWORD( wParam ) ) {

          case STN_CLICKED:
            {
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				        this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID( ) );
            }
            break;

          case STN_DBLCLK:
            {
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				        this->callAliasEx( NULL, "%s,%d", "dclick", this->getUserID( ) );
            }
            break;
        }
      }
      break;
	}
	return 0L;
}

LRESULT DcxText::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
	        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
      }
      break;

		case WM_NOTIFY:
			{
        LPNMHDR hdr = (LPNMHDR) lParam;
        if (!hdr)
          break;

        switch( hdr->code ) {
				case TTN_GETDISPINFO:
					{
						LPNMTTDISPINFO di = (LPNMTTDISPINFO)lParam;
						di->lpszText = this->m_tsToolTip.to_chr();
						di->hinst = NULL;
						bParsed = TRUE;
					}
					break;
				case TTN_LINKCLICK:
					{
						bParsed = TRUE;
						this->callAliasEx( NULL, "%s,%d", "tooltiplink", this->getUserID( ) );
					}
					break;
				}
			}
			break;

    case WM_CONTEXTMENU:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
	        this->callAliasEx( NULL, "%s,%d", "rclick", this->getUserID( ) );
      }
      break;

    case WM_MOUSEMOVE:
      {
        this->m_pParentDialog->setMouseControl( this->getUserID( ) );
      }
      break;

    case WM_SETFOCUS:
      {
        this->m_pParentDialog->setFocusControl( this->getUserID( ) );
      }
      break;

    case WM_SETCURSOR:
      {
        if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd && this->m_hCursor != NULL ) {

          SetCursor( this->m_hCursor );
          bParsed = TRUE;
          return TRUE;
        }
      }
      break;

		case WM_ERASEBKGND:
		{
			if (this->isExStyle(WS_EX_TRANSPARENT)) {
				bParsed = TRUE;
				return TRUE;
			}

			break;
		}

		case WM_PAINT:
		{
			//if (!this->isExStyle(WS_EX_TRANSPARENT) && !this->m_bAlphaBlend)
			//	break;

			bParsed = TRUE;
			LRESULT res = 0L;

			PAINTSTRUCT ps;
			RECT r;
			HDC hdc = BeginPaint(this->m_Hwnd, &ps);

			// Setup alpha blend if any.
			LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

			//if (this->isExStyle(WS_EX_TRANSPARENT)) {
				int nText = GetWindowTextLength(this->m_Hwnd);
				char *text = new char[nText +1];
				GetWindowText(this->m_Hwnd, text, nText +1);
				TString wtext(text);
				delete [] text;

				GetClientRect(this->m_Hwnd, &r);

				SelectObject(hdc, this->m_hFont);
				SetTextColor(hdc, this->m_clrText);

				UINT style = DT_LEFT;
				if (this->isStyle(SS_CENTER))
					style = DT_CENTER;
				if (this->isStyle(SS_RIGHT))
					style = DT_RIGHT;
				if (this->isStyle(SS_ENDELLIPSIS))
					style |= DT_END_ELLIPSIS;
				if (this->isStyle(SS_PATHELLIPSIS))
					style |= DT_PATH_ELLIPSIS;
				if (this->isStyle(SS_NOPREFIX))
					style |= DT_NOPREFIX;
				if (this->isStyle(SS_LEFTNOWORDWRAP))
					style |= DT_SINGLELINE;
 				else
					style |= DT_WORDBREAK; // changed for autowrap between words

				if (!this->m_bCtrlCodeText) {
					SetBkMode(hdc, TRANSPARENT);
					if (this->m_bShadowText)
						dcxDrawShadowText(hdc, wtext.to_wchr(), nText, &r, style, this->m_clrText, 0, 5, 5);
					else
						DrawText(hdc, wtext.to_chr(), nText, &r, style);
				}
				else
					mIRC_DrawText(hdc, wtext, &r, style, this->m_bShadowText);

				res = TRUE;
			//}
			//else
			//	res = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

			this->FinishAlphaBlend(ai);

			EndPaint(this->m_Hwnd, &ps);
			return res;
		}

    case WM_DESTROY:
      {
        delete this;
        bParsed = TRUE;
      }
      break;

    default:
      break;
  }

  return 0L;
}