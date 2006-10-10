/*
	* Pager Control
*/

#include "dcxpager.h"
#include "dcxdialog.h"

#include "dcxpanel.h"
#include "dcxprogressbar.h"
#include "dcxtrackbar.h"
#include "dcxcolorcombo.h"
#include "dcxcomboex.h"
#include "dcxstatusbar.h"
#include "dcxtreeview.h"
#include "dcxtoolbar.h"
#include "dcxlistview.h"
#include "dcxbutton.h"
#include "dcxrichedit.h"
#include "dcxrebar.h"
#include "dcxipaddress.h"
#include "dcxupdown.h"
#include "dcxwebctrl.h"
#include "dcxcalendar.h"

#include "dcxdivider.h"
#include "dcxtab.h"

#include "dcxmwindow.h"
#include "dcxmdialog.h"

#include "mirc/dcxline.h"
#include "mirc/dcxbox.h"
#include "mirc/dcxradio.h"
#include "mirc/dcxcheck.h"
#include "mirc/dcxtext.h"
#include "mirc/dcxedit.h"
#include "mirc/dcxscroll.h"
#include "mirc/dcxlist.h"
#include "mirc/dcxlink.h"
#include "mirc/dcximage.h"

#include "layout/layoutcellfixed.h"
#include "layout/layoutcellfill.h"
#include "layout/layoutcellpane.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxPager::DcxPager( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles | WS_EX_CONTROLPARENT,
    DCX_PAGERCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    //WS_CHILD | WS_VISIBLE | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle( NULL ), 
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->m_ChildHWND = NULL;
	//GetStockObject();
	//Pager_SetButtonSize(this->m_Hwnd,15);
	//Pager_SetBkColor(this->m_Hwnd,0);

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

DcxPager::DcxPager( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles | WS_EX_CONTROLPARENT, 
    DCX_PAGERCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    //WS_CHILD | WS_VISIBLE | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->m_ChildHWND = NULL;
	//Pager_SetButtonSize(this->m_Hwnd,15);
	//Pager_SetBkColor(this->m_Hwnd,0);

  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxPager::~DcxPager( ) {

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPager::parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme) {
	unsigned int i = 1, numtok = styles.numtok(" ");

  while ( i <= numtok ) {

		if (styles.gettok( i, " ") == "horizontal")
			*Styles |= PGS_HORZ;
		else if (styles.gettok( i, " ") == "autoscroll")
			*Styles |= PGS_AUTOSCROLL;

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

void DcxPager::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( " " );
  if ( input.gettok( 3, " " ) == "color") {
    wsprintf( szReturnValue, "%d", Pager_GetBkColor(this->m_Hwnd) );
    return;
	}
  else if ( input.gettok( 3, " " ) == "bsize") {
    wsprintf( szReturnValue, "%d", Pager_GetButtonSize(this->m_Hwnd) );
    return;
	}
  else if ( input.gettok( 3, " " ) == "border") {
    wsprintf( szReturnValue, "%d", Pager_GetBorder(this->m_Hwnd) );
    return;
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

void DcxPager::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );
  // xdid -b [NAME] [ID] [SWITCH] [W]
  if ( flags.switch_flags[1] && numtok > 3 ) {
		this->setBorderSize((int)input.gettok(4," ").to_num());
	}
  // xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
  else if ( flags.switch_flags[2] && numtok > 8 ) {

		if (IsWindow(this->m_ChildHWND)) {
			DCXError( "/xdid -c","Child Control already exists" );
			return;
		}
    UINT ID = mIRC_ID_OFFSET + (UINT)input.gettok( 4, " " ).to_int( );

    if ( ID > mIRC_ID_OFFSET - 1 && 
      !IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
      this->m_pParentDialog->getControlByID( ID ) == NULL ) 
    {
				DcxControl * p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,input,5,
					CTLF_ALLOW_TOOLBAR |
					CTLF_ALLOW_REBAR |
					CTLF_ALLOW_PANEL |
					CTLF_ALLOW_TAB |
					CTLF_ALLOW_BOX |
					CTLF_ALLOW_IMAGE |
					CTLF_ALLOW_PAGER |
					CTLF_ALLOW_DOCK
					,this->m_Hwnd);

      if ( p_Control != NULL ) {
        this->m_pParentDialog->addControl( p_Control );
				p_Control->addStyle(CCS_NORESIZE);
				this->setChild(p_Control->getHwnd());

        this->redrawWindow( );
      }
    }
    else {
      TString error;
      error.sprintf("Control with ID \"%d\" already exists", ID - mIRC_ID_OFFSET );
			DCXError("/xdid -c", error.to_chr() );
    }
  }
  // xdid -d [NAME] [ID] [SWITCH] [ID]
  else if ( flags.switch_flags[3] && numtok > 3 ) {

    UINT ID = mIRC_ID_OFFSET + input.gettok( 4, " " ).to_int( );
    DcxControl * p_Control;
    
    if ( IsWindow( GetDlgItem( this->m_Hwnd, ID ) ) && 
      ID > mIRC_ID_OFFSET - 1 && ( p_Control = this->m_pParentDialog->getControlByID( ID ) ) != NULL ) 
    {

      HWND cHwnd = p_Control->getHwnd( );
			if ( p_Control->getType( ) == "dialog" || p_Control->getType( ) == "window" ) {
        delete p_Control;
				this->m_ChildHWND = NULL;
			}
			else if ( p_Control->getRefCount( ) == 0 ) {
				this->m_pParentDialog->deleteControl( p_Control ); // remove from internal list!
        DestroyWindow( cHwnd );
				this->m_ChildHWND = NULL;
			}
      else {
        TString error;
        error.sprintf("Can't delete control with ID \"%d\" when it is inside it's own event (dialog %s)", 
                  p_Control->getUserID( ), this->m_pParentDialog->getName( ).to_chr( ) );
				DCXError("/xdid -d", error.to_chr() );
      }
    }
    else {
      TString error;
      error.sprintf("Unknown control with ID \"%d\" (dialog %s)", 
                ID - mIRC_ID_OFFSET, this->m_pParentDialog->getName( ).to_chr( ) );
			DCXError("/xdid -d", error.to_chr() );
    }
  }
	// xdid -s [NAME] [ID] [SWITCH] [SIZE]
	else if (flags.switch_flags[18] && numtok > 3) {
		this->setButtonSize((LONG)input.gettok(4, -1, " ").to_num());
	}
	// xdid -t [NAME] [ID] [SWITCH] [COLOR]
	else if (flags.switch_flags[19] && numtok > 3) {
		this->setBkColor((COLORREF)input.gettok(4, -1, " ").to_num());
	}
	// xdid -z [NAME] [ID] [SWITCH]
	else if (flags.switch_flags[25] && numtok > 2) {
		this->reCalcSize();
	}
  else {
    this->parseGlobalCommandRequest( input, flags );
  }
}
void DcxPager::setChild(const HWND child)
{
	this->m_ChildHWND = child;
	Pager_SetChild(this->m_Hwnd,child);
}
void DcxPager::setBkColor(const COLORREF c)
{
	Pager_SetBkColor(this->m_Hwnd,c);
}
void DcxPager::setBorderSize(const int bSize)
{
	Pager_SetBorder(this->m_Hwnd,bSize);
}
void DcxPager::setButtonSize(const int bSize)
{
	Pager_SetButtonSize(this->m_Hwnd,bSize);
}
void DcxPager::reCalcSize(void) const
{
	Pager_RecalcSize(this->m_Hwnd);
}
/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxPager::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {
    case WM_NOTIFY : 
      {

        LPNMHDR hdr = (LPNMHDR) lParam;

        if (!hdr)
          break;

				switch( hdr->code ) {
				case PGN_CALCSIZE:
					{
						bParsed = TRUE;
						LPNMPGCALCSIZE lpnmcs = (LPNMPGCALCSIZE) lParam;
						RECT rc;
						ZeroMemory(&rc,sizeof(RECT));
						//GetWindowRect(this->m_ChildHWND,&rc);
						//if (lpnmcs->dwFlag == PGF_CALCHEIGHT)
						//	lpnmcs->iHeight = (rc.bottom - rc.top);
						//else
						//	lpnmcs->iWidth = (rc.right - rc.left);
						DcxControl *cthis = (DcxControl *)GetProp(this->m_ChildHWND,"dcx_cthis");
						if (cthis != NULL) {
							int bSize = Pager_GetButtonSize(this->m_Hwnd);
							if (cthis->getType() == "toolbar") {
								SIZE sz;
								SendMessage(this->m_ChildHWND, TB_GETMAXSIZE, 0, (LPARAM)&sz);
								if (lpnmcs->dwFlag == PGF_CALCHEIGHT)
									lpnmcs->iHeight = sz.cy+bSize;
								else
									lpnmcs->iWidth = sz.cx+bSize;
							}
							else {
								GetWindowRect(this->m_ChildHWND,&rc);
								if (lpnmcs->dwFlag == PGF_CALCHEIGHT)
									lpnmcs->iHeight = (rc.bottom - rc.top)+bSize;
								else
									lpnmcs->iWidth = (rc.right - rc.left)+bSize;
							}
						}
					}
					break;
				}
      }
      break;
	}
	return 0L;
}

LRESULT DcxPager::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
  switch( uMsg ) {

    case WM_HELP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
	        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_NOTIFY : 
      {

        LPNMHDR hdr = (LPNMHDR) lParam;

        if (!hdr)
          break;
				if (IsWindow(hdr->hwndFrom)) {
					DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
				switch( hdr->code ) {
				case TBN_DELETINGBUTTON:
					{ // handle toolbar button delete.
						this->reCalcSize();
					}
					break;
				}
      }
      break;

    case WM_HSCROLL:
    case WM_VSCROLL:
    case WM_COMMAND:
      {
				if (IsWindow((HWND) lParam)) {
					DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
      }
      break;

    case WM_DELETEITEM:
      {
				DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
      }
      break;

    case WM_MEASUREITEM:
      {
				HWND cHwnd = GetDlgItem(this->m_Hwnd, wParam);
				if (IsWindow(cHwnd)) {
					DcxControl *c_this = (DcxControl *) GetProp(cHwnd,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
      }
      break;

    case WM_DRAWITEM:
      {
				DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
      }
			break;

    case WM_SIZE:
      {
        HWND bars = NULL;

        while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_REBARCTRLCLASS, NULL ) ) != NULL ) {

          SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
        }

        while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_STATUSBARCLASS, NULL ) ) != NULL ) {

          SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
        }

        while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_TOOLBARCLASS, NULL ) ) != NULL ) {

          SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
        }

				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_SIZE)
	        this->callAliasEx( NULL, "%s,%d", "sizing", this->getUserID( ) );
				this->reCalcSize();
        this->redrawWindow( );
      }
      break;

    case WM_CTLCOLORDLG:
      {
        bParsed = TRUE;
        return (INT_PTR) this->getBackClrBrush( );
      }
      break;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
      {

        DcxControl * p_Control = this->m_pParentDialog->getControlByHWND( (HWND) lParam );

        if ( p_Control != NULL ) {

          COLORREF clrText = p_Control->getTextColor( );
          COLORREF clrBackText = p_Control->getBackColor( );
          HBRUSH hBackBrush = p_Control->getBackClrBrush( );

          if ( clrText != -1 )
            SetTextColor( (HDC) wParam, clrText );
 
          if ( clrBackText != -1 )
            SetBkColor( (HDC) wParam, clrBackText );

          if ( hBackBrush != NULL ) {

            bParsed = TRUE;
            return (LRESULT) hBackBrush;
          }

        }
      }
      break;

    case WM_SETCURSOR:
      {
        if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd ) {

          if ( this->m_hCursor != NULL ) {

            SetCursor( this->m_hCursor );
          }
          else {

            HCURSOR hCursor = LoadCursor( NULL, IDC_ARROW );
            SetCursor( hCursor );
          }
          bParsed = TRUE;
          return TRUE;
        }
      }
      break;

    case WM_MOUSEMOVE:
      {
        this->m_pParentDialog->setMouseControl( this->getUserID( ) );
      }
      break;

    case WM_LBUTTONDOWN:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
					this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID( ) );
					this->callAliasEx( NULL, "%s,%d", "lbdown", this->getUserID( ) );
				}
      }
      break;

    case WM_LBUTTONUP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
	        this->callAliasEx( NULL, "%s,%d", "lbup", this->getUserID( ) );
      }
      break;

    case WM_LBUTTONDBLCLK:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
					this->callAliasEx( NULL, "%s,%d", "dclick", this->getUserID( ) );
					this->callAliasEx( NULL, "%s,%d", "lbdblclk", this->getUserID( ) );
				}
      }
      break;

    case WM_SETFOCUS:
      {
        this->m_pParentDialog->setFocusControl( this->getUserID( ) );
				this->reCalcSize();
      }
      break;
   // case WM_PAINT:
   //   {
   //     PAINTSTRUCT ps; 
   //     HDC hdc;
			//	LRESULT res = 0L;

   //     hdc = BeginPaint( this->m_Hwnd, &ps );
			//	// Center icon in client rectangle
			//	int cxIcon = GetSystemMetrics(SM_CXICON);
			//	int cyIcon = GetSystemMetrics(SM_CYICON);
			//	RECT rc;
			//	GetClientRect(this->m_Hwnd,&rc);
			//	int x = ((rc.right - rc.left) - cxIcon + 1) / 2;
			//	int y = ((rc.bottom - rc.top) - cyIcon + 1) / 2;

			//	// Draw the icon
			//	//DrawIcon(hdc, x, y, m_hIcon);
   //     EndPaint( this->m_Hwnd, &ps ); 

   //     return res;
			//}
			//break;
    case WM_DESTROY:
      {
        delete this;
        bParsed = TRUE;
      }
      break;

    default:
      break;
  }

  return lRes;
}
