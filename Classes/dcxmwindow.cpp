/*!
 * \file dcxmwindow.cpp
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

#include "dcxmwindow.h"
#include "dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param cHwnd Attached \@window Handle
 * \param pHwnd Parent Window
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxMWindow::DcxMWindow( const HWND cHwnd, const HWND pHwnd, const UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) : DcxControl( ID, p_Dialog ) {

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = cHwnd;
  this->m_OrigParentHwnd = GetParent( this->m_Hwnd );

  this->m_OrigStyles = this->removeStyle( WS_CAPTION|DS_FIXEDSYS|DS_SETFONT|DS_MODALFRAME|WS_POPUP|
    WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME );

  this->m_OrigExStyles = this->removeExStyle( WS_EX_CONTROLPARENT|WS_EX_MDICHILD );

  this->addStyle( WS_CHILD );

  SetParent( this->m_Hwnd, pHwnd );
  SetWindowPos( this->m_Hwnd, NULL, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, NULL );
  ShowWindow( this->m_Hwnd, SW_SHOWNOACTIVATE );
  UpdateWindow( this->m_Hwnd);

  this->m_OrigID = SetWindowLong( this->m_Hwnd, GWL_ID, ID );

  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief Destructor
 *
 * blah
 */

DcxMWindow::~DcxMWindow( ) {

	HWND parent = GetParent( this->m_Hwnd );
	if ( parent == this->m_OrigParentHwnd && this->m_OrigParentHwnd != this->m_pParentDialog->getHwnd())
		return;

	this->unregistreDefaultWindowProc( );

	BOOL bHide = IsWindowVisible( this->m_Hwnd );
	if ( !bHide )
		ShowWindow( this->m_Hwnd, SW_HIDE );
	
	SetWindowLong( this->m_Hwnd, GWL_ID, this->m_OrigID );
	//SetParent( this->m_Hwnd, this->m_OrigParentHwnd );
	//this->removeStyle(WS_CHILDWINDOW);
	//SetParent( this->m_Hwnd, NULL );
	if (parent == this->m_OrigParentHwnd) // handles oddness where orig parent == current when it shouldnt, maybe due to init event docking.
		parent = GetParent(parent);
	else
		parent = this->m_OrigParentHwnd;

	SetParent( this->m_Hwnd, parent );
	this->setStyle( this->m_OrigStyles );
	this->setExStyle( this->m_OrigExStyles );

	SetWindowPos( this->m_Hwnd, NULL, 30, 30, 0, 0, SWP_NOSIZE|SWP_FRAMECHANGED );
	UpdateWindow( this->m_Hwnd );

	if ( !bHide )
		ShowWindow( this->m_Hwnd, SW_SHOW );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxMWindow::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( );

  if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
    return;
  
  szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxMWindow::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( input.gettok( 3 ), &flags );

//  int numtok = input.numtok( );

  this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief Subclassed Window Procedure
 *
 * \param uMsg Window Procedure Message ID
 * \param wParam Window Procedure WPARAM
 * \param lParam Window Procedure LPARAM
 * \param bParsed Indicates if subclassed procedure parsed the message
 */
LRESULT DcxMWindow::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxMWindow::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

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