/*!
 * \file dcxline.cpp
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

#include "dcxline.h"
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

DcxLine::DcxLine( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
, m_bVertical(false)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles | WS_EX_TRANSPARENT,
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

	this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxLine::~DcxLine( ) {

	this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxLine::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	unsigned int i = 1, numtok = styles.numtok( );

	while ( i <= numtok ) {

		if ( styles.gettok( i ) == "vertical" )
			this->m_bVertical = true;
		else if (styles.gettok(i ) == "nowrap")
			*Styles |= SS_LEFTNOWORDWRAP;
		else if (styles.gettok(i) == "center")
			*Styles |= SS_CENTER;
		else if (styles.gettok(i) == "right")
			*Styles |= SS_RIGHT;
		else if (styles.gettok(i) == "noprefix")
			*Styles |= SS_NOPREFIX;
		else if (styles.gettok(i) == "endellipsis")
			*Styles |= SS_ENDELLIPSIS;
		else if (styles.gettok(i) == "pathellipsis")
			*Styles |= SS_PATHELLIPSIS;
		else if ( styles.gettok( i ) == "alpha" )
			this->m_bAlphaBlend = true;
		else if (( styles.gettok( i ) == "shadow" ))
			this->m_bShadowText = true;
		else if (( styles.gettok( i ) == "noformat" ))
			this->m_bCtrlCodeText = false;

		i++;
	}
	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxLine::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( );
	if ( input.gettok( 3 ) == "text" ) {
		lstrcpyn(szReturnValue, this->m_sText.to_chr(), 900);
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

void DcxLine::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( input.gettok( 3 ), &flags );

//  int numtok = input.numtok( );

	//xdid -t [NAME] [ID] [SWITCH] [TEXT]
	if (flags.switch_flags[19]) {
		this->m_sText = input.gettok(4, -1);
		this->m_sText.trim();

		if (this->m_bVertical) {
			HFONT hFont = (HFONT)this->getFont(), hVFont = NULL;
			if (hFont == NULL)
				hFont = GetStockFont(DEFAULT_GUI_FONT);
			LOGFONT lf;
			ZeroMemory(&lf, sizeof(LOGFONT));
			GetObject(hFont, sizeof(LOGFONT), &lf);
			lf.lfEscapement = 900;
			lf.lfOrientation = 900;
			hVFont = CreateFontIndirect(&lf);
			if (hVFont != NULL)
				this->setControlFont(hVFont, FALSE);
		}
		// redraw if transparent
		if (this->isExStyle(WS_EX_TRANSPARENT)) {
			RECT r;
			POINT pt;
			int w, h;

			GetWindowRect(this->m_Hwnd, &r);
			pt.x = r.left;
			pt.y = r.top;
			w = r.right - r.left;
			h = r.bottom - r.top;
			ScreenToClient(GetParent(this->m_Hwnd), &pt);
			r.left = pt.x;
			r.top = pt.y;
			r.right = r.left + w;
			r.bottom = r.top + h;
			InvalidateRect(GetParent(this->m_Hwnd), &r, TRUE);
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
LRESULT DcxLine::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxLine::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
	        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
				bParsed = TRUE;
				return TRUE;
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
					if (GetCursor() != this->m_hCursor)
						SetCursor( this->m_hCursor );
          bParsed = TRUE;
          return TRUE;
        }
      }
      break;
		case WM_ERASEBKGND:
			{
				//this->DrawParentsBackground((HDC) wParam);
				bParsed = TRUE;
				return TRUE;
			}
			break;
		case WM_PAINT:
			{
				//if (!this->m_bAlphaBlend)
				//	break;
				PAINTSTRUCT ps;
				HDC hdc;

				hdc = BeginPaint( this->m_Hwnd, &ps );

				LRESULT res = 0L;
				bParsed = TRUE;

				RECT rcClient, rcLine, rcText;

				// get controls client area
				GetClientRect( this->m_Hwnd, &rcClient );

				// Setup alpha blend if any.
				LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

				// fill background.
				//DcxControl::DrawCtrlBackground(hdc,this,&rcClient);

				//res = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );
				rcLine = rcClient;
				rcText = rcClient;

				// draw text if any.
				if (this->m_sText.len() > 0) {
					if (this->m_hFont != NULL)
						SelectObject(hdc, this->m_hFont);

					if (this->m_clrText != -1)
						SetTextColor(hdc, this->m_clrText);
					else
						SetTextColor(hdc, GetSysColor(
							IsWindowEnabled(this->m_Hwnd) ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT)
						);
					UINT style = 0;
					if (this->isStyle(SS_ENDELLIPSIS))
						style |= DT_END_ELLIPSIS;
					if (this->isStyle(SS_PATHELLIPSIS))
						style |= DT_PATH_ELLIPSIS;
					if (this->isStyle(SS_NOPREFIX))
						style |= DT_NOPREFIX;
					if (this->isStyle(SS_LEFTNOWORDWRAP))
						style |= DT_SINGLELINE;
					if (this->m_bVertical) {
						style |= DT_CENTER;
						SIZE sz;
						SetBkMode(hdc, TRANSPARENT);
						GetTextExtentPoint32(hdc,this->m_sText.to_chr(),this->m_sText.len(), &sz);
						rcText.bottom = rcText.top + sz.cx;
						rcText.right = rcText.left + sz.cy;
						if (this->isStyle(SS_CENTER))
							OffsetRect(&rcText,((rcClient.right - rcClient.left)/2) - ((rcText.right - rcText.left)/2),((rcClient.bottom - rcClient.top)/2) - ((rcText.bottom - rcText.top)/2));
						else if (this->isStyle(SS_RIGHT))
							OffsetRect(&rcText,((rcClient.right - rcClient.left)/2) - ((rcText.right - rcText.left)/2),rcClient.bottom - (rcText.bottom - rcText.top));
						TextOut(hdc,rcText.left, rcText.bottom, this->m_sText.to_chr(), this->m_sText.len());
					}
					else {
						style |= DT_LEFT|DT_VCENTER;
						if (this->m_bCtrlCodeText)
							calcStrippedRect(hdc, this->m_sText, style, &rcText, false);
						else
							DrawText(hdc, this->m_sText.to_chr(), this->m_sText.len(), &rcText, DT_CALCRECT | style);
						if (this->isStyle(SS_CENTER))
							OffsetRect(&rcText,((rcClient.right - rcClient.left)/2) - ((rcText.right - rcText.left)/2),0);
						else if (this->isStyle(SS_RIGHT))
							OffsetRect(&rcText,rcClient.right - (rcText.right - rcText.left),0);

						// draw the text
						if (!this->m_bCtrlCodeText) {
							SetBkMode(hdc, TRANSPARENT);
							if (this->m_bShadowText)
								dcxDrawShadowText(hdc,this->m_sText.to_wchr(), this->m_sText.len(),&rcText, style, this->m_clrText, 0, 5, 5);
							else
								DrawText(hdc, this->m_sText.to_chr(), this->m_sText.len(), &rcText, style);
						}
						else
							mIRC_DrawText(hdc, this->m_sText, &rcText, style, this->m_bShadowText);
					}
					ExcludeClipRect(hdc,rcText.left, rcText.top, rcText.right, rcText.bottom);
				}
				if (this->m_bVertical) {
					rcLine.left = rcLine.left + ((rcLine.right - rcLine.left) / 2);
					DrawEdge(hdc, &rcLine, EDGE_ETCHED, BF_LEFT);
				}
				else {
					rcLine.bottom = rcLine.bottom / 2;
					DrawEdge(hdc, &rcLine, EDGE_ETCHED, BF_BOTTOM);
				}
				this->FinishAlphaBlend(ai);

				EndPaint( this->m_Hwnd, &ps );
				return res;
			}
			break;

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