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

  unsigned int i = 1, numtok = styles.numtok( " " );
  *Styles = SS_ETCHEDHORZ;

  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "vertical" ) {
      *Styles &= ~SS_ETCHEDHORZ;
      *Styles |= SS_ETCHEDVERT;
    }
		//else if (styles.gettok(i , " ") == "nowrap")
		//	*Styles |= SS_LEFTNOWORDWRAP;
		//else if (styles.gettok(i, " ") == "center")
		//	*Styles |= SS_CENTER;
		//else if (styles.gettok(i, " ") == "right")
		//	*Styles |= SS_RIGHT;
		//else if (styles.gettok(i, " ") == "noprefix")
		//	*Styles |= SS_NOPREFIX;
		//else if (styles.gettok(i, " ") == "endellipsis")
		//	*Styles |= SS_ENDELLIPSIS;
		//else if (styles.gettok(i, " ") == "pathellipsis")
		//	*Styles |= SS_PATHELLIPSIS;
		//else if ( styles.gettok( i , " " ) == "alpha" )
		//	this->m_bAlphaBlend = true;

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

//  int numtok = input.numtok( " " );
  //if ( input.gettok( 3, " " ) == "text" ) {

		//lstrcpy(szReturnValue, this->m_sText.to_chr() );
  //  return;
  //}
	if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
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
  this->parseSwitchFlags( input.gettok( 3, " " ), &flags );

//  int numtok = input.numtok( " " );

	//xdid -t [NAME] [ID] [SWITCH]
	//if (flags.switch_flags[19]) {
	//	this->m_sText = input.gettok(4, -1, " ");
	//	this->m_sText.trim();

	//	// redraw if transparent
	//	if (this->isExStyle(WS_EX_TRANSPARENT)) {
	//		RECT r;
	//		POINT pt;
	//		int w, h;

	//		GetWindowRect(this->m_Hwnd, &r);
	//		pt.x = r.left;
	//		pt.y = r.top;
	//		w = r.right - r.left;
	//		h = r.bottom - r.top;
	//		ScreenToClient(GetParent(this->m_Hwnd), &pt);
	//		r.left = pt.x;
	//		r.top = pt.y;
	//		r.right = r.left + w;
	//		r.bottom = r.top + h;
	//		InvalidateRect(GetParent(this->m_Hwnd), &r, TRUE);
	//		this->redrawWindow();
	//	}
	//}
 // else
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
		//case WM_PAINT:
		//	{
		//		//if (!this->m_bAlphaBlend)
		//		//	break;
  //      PAINTSTRUCT ps;
  //      HDC hdc;

  //      hdc = BeginPaint( this->m_Hwnd, &ps );

		//		LRESULT res = 0L;
		//		bParsed = TRUE;

		//		RECT rcClient, rc;

		//		// get controls client area
		//		GetClientRect( this->m_Hwnd, &rcClient );

		//		// Setup alpha blend if any.
		//		LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

		//		// fill background.
		//		//DcxControl::DrawCtrlBackground(hdc,this,&rcClient);

		//		//res = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );
		//		rc = rcClient;
		//		int half = rc.bottom = rc.bottom / 2;
		//		DrawEdge(hdc, &rc, EDGE_ETCHED, BF_BOTTOM);

		//		// draw text if any.
		//		if (this->m_sText.len() > 0) {
		//			SetBkMode(hdc, TRANSPARENT);
		//			UINT style = DT_LEFT;
		//			if (this->isStyle(SS_CENTER))
		//				style |= DT_CENTER;
		//			if (this->isStyle(SS_RIGHT)) {
		//				style &= ~DT_LEFT;
		//				style |= DT_RIGHT;
		//			}
		//			if (this->isStyle(SS_ENDELLIPSIS))
		//				style |= DT_END_ELLIPSIS;
		//			if (this->isStyle(SS_PATHELLIPSIS))
		//				style |= DT_PATH_ELLIPSIS;
		//			if (this->isStyle(SS_NOPREFIX))
		//				style |= DT_NOPREFIX;
		//			if (this->isStyle(SS_LEFTNOWORDWRAP))
		//				style |= DT_SINGLELINE; // ?? same ??
		//			DrawText(hdc, this->m_sText.to_chr(), this->m_sText.len(), &rc, DT_CALCRECT);
		//			int w = rc.right - rc.left;
		//			int h = rc.bottom - rc.top;
		//			rc.top = half - h/2;
		//			DrawText(hdc, this->m_sText.to_chr(), this->m_sText.len(), &rc, style);
		//		}

		//		this->FinishAlphaBlend(ai);

		//		EndPaint( this->m_Hwnd, &ps );
		//		return res;
		//	}
		//	break;

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