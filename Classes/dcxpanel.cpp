/*!
 * \file dcxpanel.cpp
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

#include "dcxpanel.h"
#include "dcxdialog.h"

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
#include "dcxpager.h"

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

//DcxPanel::DcxPanel( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
//: DcxControl( ID, p_Dialog ) 
//{
//
//  LONG Styles = 0, ExStyles = 0;
//  BOOL bNoTheme = FALSE;
//  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );
//
//  this->m_Hwnd = CreateWindowEx(	
//    ExStyles | WS_EX_CONTROLPARENT,
//    DCX_PANELCLASS, 
//    NULL,
//    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
//    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
//    p_Dialog->getHwnd( ),
//    (HMENU) ID,
//    GetModuleHandle( NULL ), 
//    NULL);
//
//  if ( bNoTheme )
//    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );
//
//  this->m_pLayoutManager = new LayoutManager( this->m_Hwnd );
//
//  this->registreDefaultWindowProc( );
//  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
//}
//
/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxPanel::DcxPanel( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles | WS_EX_CONTROLPARENT, 
    DCX_PANELCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

  this->m_pLayoutManager = new LayoutManager( this->m_Hwnd );

  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxPanel::~DcxPanel( ) {

  if ( this->m_pLayoutManager != NULL )
    delete this->m_pLayoutManager;

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPanel::parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme) {
	//unsigned int i = 1, numtok = styles.numtok(" ");

  //*ExStyles = WS_EX_CONTROLPARENT;

  /*
  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "notheme" )
      *bNoTheme = TRUE;

    i++;
  }
  */

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

void DcxPanel::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( " " );

  if ( this->parseGlobalInfoRequest( input, szReturnValue ) ) {

    return;
  }
  
  szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPanel::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  // xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
  if ( flags.switch_flags[2] && numtok > 8 ) {

    UINT ID = mIRC_ID_OFFSET + input.gettok( 4, " " ).to_int( );

    if ( ID > mIRC_ID_OFFSET - 1 && 
      !IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
      this->m_pParentDialog->getControlByID( ID ) == NULL ) 
    {
			DcxControl * p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,input,5,-1,this->m_Hwnd);

			if ( p_Control != NULL ) {
				this->m_pParentDialog->addControl( p_Control );
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
      if ( p_Control->getType( ) == "dialog" || p_Control->getType( ) == "window" )
        delete p_Control;
			else if ( p_Control->getRefCount( ) == 0 ) {
				this->m_pParentDialog->deleteControl( p_Control ); // remove from internal list!
        DestroyWindow( cHwnd );
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
  /*
  //xdid -l [NAME] [ID] [SWITCH] [OPTIONS]

  [OPTIONS] :

  root [TAB]+flpiw [ID] [WEIGHT] [W] [H]
  add PATH[TAB]+flpiw [ID] [WEIGHT] [W] [H]
  space PATH[TAB]+ [L] [T] [R] [B]
  */
  else if ( flags.switch_flags[11] && numtok > 3 ) {

    if ( input.gettok( 4, " " ) == "update" ) {

      if ( this->m_pLayoutManager != NULL ) {

        RECT rc;
        GetClientRect( this->m_Hwnd, &rc );
        this->m_pLayoutManager->updateLayout( rc );
				this->redrawWindow();
      }
    }
		else if (input.gettok(4, " ") == "clear") {
			if (this->m_pLayoutManager != NULL)
				delete this->m_pLayoutManager;
			this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);
			//this->redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
    else if ( numtok > 8 ) {
      TString com(input.gettok( 1, "\t" ).gettok( 4, " " ));
      com.trim( );
      TString path(input.gettok( 1, "\t" ).gettok( 5, -1, " " ));
      path.trim( );
      TString p2(input.gettok( 2, "\t" ));
      p2.trim( );

      UINT flags = this->parseLayoutFlags( p2.gettok( 1, " " ) );
      UINT ID = p2.gettok( 2, " " ).to_int( );
      UINT WGT = p2.gettok( 3, " " ).to_int( );
      UINT W = p2.gettok( 4, " " ).to_int( );
      UINT H = p2.gettok( 5, " " ).to_int( );

      if ( com ==  "root" || com == "cell" ) {

        HWND cHwnd = GetDlgItem( this->m_Hwnd, mIRC_ID_OFFSET + ID );

        LayoutCell * p_Cell = NULL;

        // LayoutCellPane
        if ( flags & LAYOUTPANE ) {

          if ( flags & LAYOUTHORZ )
            p_Cell = new LayoutCellPane( LayoutCellPane::HORZ );
          else
            p_Cell = new LayoutCellPane( LayoutCellPane::VERT );
        } // if ( flags & LAYOUTPANE )
        // LayoutFill Cell
        else if ( flags & LAYOUTFILL ) {
          if ( flags & LAYOUTID ) {
            if ( cHwnd != NULL && IsWindow( cHwnd ) )
              p_Cell = new LayoutCellFill( cHwnd );
            else {
              TString error;
              error.sprintf("Cell Fill -> Invalid ID : %d", ID );
							DCXError("/xdid -l", error.to_chr() );
              return;
            }
          }
          else {
            p_Cell = new LayoutCellFill( );
          }
        } // else if ( flags & LAYOUTFILL )
        // LayoutCellFixed
        else if ( flags & LAYOUTFIXED ) {

          LayoutCellFixed::FixedType type;

          if ( flags & LAYOUTVERT && flags & LAYOUTHORZ )
            type = LayoutCellFixed::BOTH;
          else if ( flags & LAYOUTVERT )
            type = LayoutCellFixed::HEIGHT;
          else
            type = LayoutCellFixed::WIDTH;

          // Defined Rectangle
          if ( flags & LAYOUTDIM ) {

            RECT rc;
            SetRect( &rc, 0, 0, W, H );

            if ( flags & LAYOUTID ) {

              if ( cHwnd != NULL && IsWindow( cHwnd ) )
                p_Cell = new LayoutCellFixed( cHwnd, rc, type );
              else {
                TString error;
                error.sprintf("Cell Fixed -> Invalid ID : %d", ID );
								DCXError("/xdid -l", error.to_chr() );
                return;
              }
            }
            else
              p_Cell = new LayoutCellFixed( rc, type );

          }
          // No defined Rectangle
          else {

            if ( flags & LAYOUTID ) {

              if ( cHwnd != NULL && IsWindow( cHwnd ) )
                p_Cell = new LayoutCellFixed( cHwnd, type );
              else {
                TString error;
                error.sprintf("Cell Fixed -> Invalid ID : %d", ID );
								DCXError("/xdid -l", error.to_chr() );
                return;
              }
            }
          } //else
        } // else if ( flags & LAYOUTFIXED )
        else {
          DCXError("/xdid -l", "Unknown Cell Type" );
          return;
        }

        if ( com == "root" ) {

          if ( p_Cell != NULL )
            this->m_pLayoutManager->setRoot( p_Cell );
  
        } // if ( com == "root" )
        else if ( com == "cell" ) {

          if ( p_Cell != NULL ) {

            LayoutCell * p_GetCell;

            if ( path == "root" )
              p_GetCell = this->m_pLayoutManager->getRoot( );
            else
              p_GetCell = this->m_pLayoutManager->getCell( path );

            if ( p_GetCell == NULL ) {

              TString error;
              error.sprintf("Invalid item path: %s", path.to_chr( ) );
							DCXError("/xdid -l", error.to_chr() );
              return;
            }

            if ( p_GetCell->getType( ) == LayoutCell::PANE ) {

              LayoutCellPane * p_PaneCell = (LayoutCellPane *) p_GetCell;
              p_PaneCell->addChild( p_Cell, WGT );
            }
          }
        } // else if ( com == "cell" )
      } // if ( com ==  "root" || com == "cell" )
      else if ( com ==  "space" ) {

        LayoutCell * p_GetCell;

        if ( path == "root" )
          p_GetCell = this->m_pLayoutManager->getRoot( );
        else
          p_GetCell = this->m_pLayoutManager->getCell( path );

        if ( p_GetCell == NULL ) {

          TString error;
          error.sprintf("Invalid item path: %s", path.to_chr( ) );
					DCXError("/xdid -l", error.to_chr() );
          return;
        }
        else {

          RECT rc;
          SetRect( &rc, ID, WGT, W, H );
          p_GetCell->setBorder( rc );
        }
      } // else if ( com == "space" )
    } // if ( numtok > 7 )
  }
	// xdid -t [NAME] [ID] [SWITCH] [TEXT]
	else if (flags.switch_flags[19] && numtok > 3) {
		SetWindowText(this->m_Hwnd, input.gettok(4, -1, " ").to_chr());
	}
  else
    this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxPanel::parseLayoutFlags( TString & flags ) {

  INT i = 1, len = flags.len( );
  UINT iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if ( flags[i] == 'f' )
      iFlags |= LAYOUTFIXED;
    else if ( flags[i] == 'h' )
      iFlags |= LAYOUTHORZ;
    else if ( flags[i] == 'i' )
      iFlags |= LAYOUTID;
    else if ( flags[i] == 'l' )
      iFlags |= LAYOUTFILL ;
    else if ( flags[i] == 'p' )
      iFlags |= LAYOUTPANE;
    else if ( flags[i] == 'v' )
      iFlags |= LAYOUTVERT;
    else if ( flags[i] == 'w' )
      iFlags |= LAYOUTDIM;

    ++i;
  }

  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxPanel::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxPanel::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

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

        RECT rc;
        SetRect( &rc, 0, 0, LOWORD( lParam ), HIWORD( lParam ) );
        this->m_pLayoutManager->updateLayout( rc );
        this->redrawWindow( );
      }
      break;

    case WM_CTLCOLORDLG:
      {
        bParsed = TRUE;
        return (INT_PTR) this->getBackClrBrush( );
      }
      break;

      /*
    case WM_ERASEBKGND: 
      {

        if ( this->m_hBackBrush != NULL ) {

          bParsed = TRUE;

          HDC hdc = GetDC( this->m_Hwnd );

          HBRUSH hBrush;

          if ( this->m_hBackBrush != NULL )
            hBrush = this->m_hBackBrush;
          else
            hBrush = GetSysColorBrush( COLOR_3DFACE );

          RECT rect;
          GetWindowRect( this->m_Hwnd, &rect );
          OffsetRect( &rect, -rect.left, -rect.top );
          FillRect( hdc, &rect, hBrush );

          ReleaseDC( this->m_Hwnd, hdc );

          return TRUE;
        }
      }
      break;
      */

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
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx( NULL, "%s,%d", "lbdown", this->getUserID( ) );
      }
      break;

    case WM_LBUTTONUP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
	        this->callAliasEx( NULL, "%s,%d", "lbup", this->getUserID( ) );
					this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID( ) );
				}
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

  return lRes;
}