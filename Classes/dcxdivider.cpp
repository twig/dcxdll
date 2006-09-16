/*!
 * \file dcxdivider.cpp
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

#include "dcxdivider.h"
#include "dcxdialog.h"

//#include "dcxprogressbar.h"
//#include "dcxtrackbar.h"
//#include "dcxcolorcombo.h"
//#include "dcxcomboex.h"
//#include "dcxstatusbar.h"
//#include "dcxtoolbar.h"
//#include "dcxbutton.h"
#include "dcxtreeview.h"
#include "dcxlistview.h"
#include "dcxrebar.h"
#include "dcxrichedit.h"
#include "dcxwebctrl.h"

#include "dcxpanel.h"
#include "dcxtab.h"

#include "mirc/dcxedit.h"
#include "mirc/dcxlist.h"
#include "mirc/dcximage.h"

#include "dcxmwindow.h"
#include "dcxmdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxDivider::DcxDivider( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles | WS_EX_CONTROLPARENT, 
    DCX_DIVIDERCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle( NULL ), 
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

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

DcxDivider::DcxDivider( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles | WS_EX_CONTROLPARENT, 
    DCX_DIVIDERCLASS, 
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

DcxDivider::~DcxDivider( ) {

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDivider::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  unsigned int i = 1, numtok = styles.numtok( " " );
  *Styles |= DVS_HORZ;

  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "vertical" )
      *Styles |= DVS_VERT;

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

void DcxDivider::parseInfoRequest( TString & input, char * szReturnValue ) {

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

void DcxDivider::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  // xdid -l|r [NAME] [ID] [SWITCH] [MIN] [IDEAL][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
  if ( ( flags.switch_flags[11] || flags.switch_flags[17] )&& numtok > 9 ) {

    DVPANEINFO dvpi;
    ZeroMemory( &dvpi, sizeof( DVPANEINFO ) );
    dvpi.cbSize = sizeof( DVPANEINFO );

    TString data = input.gettok( 1, "\t" );
    data.trim( );

    TString control_data = "";
    if ( input.numtok( "\t" ) > 1 ) {

      control_data = input.gettok( 2, "\t" );
      control_data.trim( );
    }

    dvpi.fMask = DVPIM_CHILD | DVPIM_MIN | DVPIM_IDEAL;
    dvpi.cxMin = data.gettok( 4, " " ).to_int( );
    dvpi.cxIdeal = data.gettok( 5, " " ).to_int( );

    if ( control_data.numtok( " " ) > 5 ) {

      UINT ID = mIRC_ID_OFFSET + control_data.gettok( 1, " " ).to_int( );

      if ( ID > mIRC_ID_OFFSET - 1 && 
        !IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
        this->m_pParentDialog->getControlByID( ID ) == NULL ) 
      {

        TString type = control_data.gettok( 2, " " );
        RECT rc;

        rc.left = (LONG)control_data.gettok( 3, " " ).to_num( );
        rc.top = (LONG)control_data.gettok( 4, " " ).to_num( );
        rc.right = rc.left + (LONG)control_data.gettok( 5, " " ).to_num( );
        rc.bottom = rc.top + (LONG)control_data.gettok( 6, " " ).to_num( );

        DcxControl * p_Control = NULL;

        TString styles = "";
        if ( control_data.numtok( " " ) > 6 ) {
          styles = control_data.gettok( 7, -1, " " );
        }

        /*
        if ( type == "trackbar" ) {
          p_Control = new DcxTrackBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles  );
        }
        else if ( type == "pbar" ) {
          p_Control = new DcxProgressBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "comboex" ) {
          p_Control = new DcxComboEx( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "toolbar" ) {
          p_Control = new DcxToolBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
          p_Control->addStyle( CCS_NOPARENTALIGN | CCS_NORESIZE );
        }
        else if ( type == "statusbar" ) {
          p_Control = new DcxStatusBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
          p_Control->addStyle( CCS_NOPARENTALIGN | CCS_NORESIZE );
        }
        else if ( type == "colorcombo" ) {
          p_Control = new DcxColorCombo( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "button" ) {
          p_Control = new DcxButton( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        */

        if ( type == "treeview" ) {
          p_Control = new DcxTreeView( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "listview" ) {
          p_Control = new DcxListView( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "richedit" ) {
          p_Control = new DcxRichEdit( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "divider" ) {
          p_Control = new DcxDivider( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "panel" ) {
          p_Control = new DcxPanel( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "tab" ) {
          p_Control = new DcxTab( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "rebar" ) {
          p_Control = new DcxReBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "webctrl" ) {
          p_Control = new DcxWebControl( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "edit" ) {
          p_Control = new DcxEdit( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "image" ) {
          p_Control = new DcxImage( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "list" ) {
          p_Control = new DcxList( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }

        if ( p_Control != NULL ) {

          this->m_pParentDialog->addControl( p_Control );

          dvpi.hChild = p_Control->getHwnd( );

          if ( flags.switch_flags[11] )
            this->setPane( DVF_PANELEFT, &dvpi );
          else if ( flags.switch_flags[17] )
            this->setPane( DVF_PANERIGHT, &dvpi );
					//if (!this->isExStyle(WS_EX_CONTROLPARENT)) {
					//	if (p_Control->isStyle(WS_TABSTOP)) this->addExStyle(WS_EX_CONTROLPARENT);
					//}

          this->redrawWindow( );
        }
      }
      else {

        TString error;
        error.sprintf("/xdid -l|r : Control with ID \"%d\" already exists", ID - mIRC_ID_OFFSET );
				mIRCError( error.to_chr() );
      }
    }
  }
  // xdid -v [NAME] [ID] [SWITCH] [POS]
  else if ( flags.switch_flags[21] && numtok > 3 ) {

    int iPos = atoi( input.gettok( 4, " " ).to_chr( ) );
    this->setDivPos( iPos );
  }
  else {
    this->parseGlobalCommandRequest( input, flags );
  }
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxDivider::setPane( UINT iPaneId, LPDVPANEINFO lpdvpi ) {
  return SendMessage( this->m_Hwnd, DV_SETPANE, (WPARAM) iPaneId, (LPARAM) lpdvpi );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxDivider::setDivPos( UINT iDivPos ) {
  return SendMessage( this->m_Hwnd, DV_SETDIVPOS, (WPARAM) 0, (LPARAM) iDivPos );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxDivider::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
//  switch( uMsg ) {
//	}
	return 0L;
}

LRESULT DcxDivider::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
  switch( uMsg ) {

    case WM_HELP:
      {
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