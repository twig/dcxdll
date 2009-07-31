/*!
 * \file dcxmdialog.cpp
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
#include "Classes/dcxmdialog.h"
#include "Classes/dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param cHwnd Attached dialog Handle
 * \param pHwnd Parent Window
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxMDialog::DcxMDialog( HWND cHwnd, HWND pHwnd, UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles )
: DcxControl( ID, p_Dialog )
{
  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->m_DeleteByDestroy = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = cHwnd;
  this->m_OrigParentHwnd = GetParent( this->m_Hwnd );

  this->m_OrigStyles = this->removeStyle( WS_CAPTION|DS_FIXEDSYS|DS_SETFONT|DS_3DLOOK|DS_MODALFRAME|
    WS_POPUP|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME );

  this->m_OrigExStyles = this->setExStyle( WS_EX_CONTROLPARENT );

  this->addStyle( WS_CHILD );

  SetParent( this->m_Hwnd, pHwnd );
  SetWindowPos( this->m_Hwnd, NULL, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, NULL );
  ShowWindow( this->m_Hwnd, SW_SHOWNOACTIVATE );
  UpdateWindow( this->m_Hwnd );

  this->m_OrigID = SetWindowLong( this->m_Hwnd, GWL_ID, ID );

  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief Destructor
 *
 * blah
 */

DcxMDialog::~DcxMDialog( ) {
	HWND parent = GetParent( this->m_Hwnd );
	if ( parent == this->m_OrigParentHwnd && this->m_OrigParentHwnd != this->m_pParentDialog->getHwnd())
		return;

	this->unregistreDefaultWindowProc( );
	if (!this->m_DeleteByDestroy) { // all this isn't needed if control is deleted because of closing the dialog

		BOOL bHide = IsWindowVisible( this->m_Hwnd );
		if ( !bHide )
			ShowWindow( this->m_Hwnd, SW_HIDE );

		SetWindowLong( this->m_Hwnd, GWL_ID, this->m_OrigID );
		//this->removeStyle(WS_CHILD);
		//this->addStyle(WS_POPUP);
		//SetParent( this->m_Hwnd, NULL );
		if (parent == this->m_OrigParentHwnd) // handles oddness where orig parent == current when it shouldnt, maybe due to init event docking.
			parent = GetParent(parent);
		else
			parent = this->m_OrigParentHwnd;

		SetParent( this->m_Hwnd, parent );
		//SetParent( this->m_Hwnd, this->m_OrigParentHwnd);
		this->setStyle( this->m_OrigStyles);
		this->setExStyle( this->m_OrigExStyles );

		SetWindowPos( this->m_Hwnd, NULL, 30, 30, 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED);
		this->redrawWindow( );

		if ( !bHide )
			ShowWindow( this->m_Hwnd, SW_SHOW );
	}
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxMDialog::parseInfoRequest(TString &input, char *szReturnValue) {
//	int numtok = input.numtok( );

	// [NAME] [ID] [PROP]
	if (input.gettok( 3 ) == "dname") {
		lstrcpyn(szReturnValue, this->m_OrigName.to_chr(), 900);
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

void DcxMDialog::parseCommandRequest( TString & input ) {
	XSwitchFlags flags(input.gettok(3));

  //int numtok = input.numtok( );

	this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxMDialog::parseControlStyles(TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) {
	this->m_OrigName = styles;
}

/*!
 * \brief Subclassed Window Procedure
 *
 * \param uMsg Window Procedure Message ID
 * \param wParam Window Procedure WPARAM
 * \param lParam Window Procedure LPARAM
 * \param bParsed Indicates if subclassed procedure parsed the message
 */
LRESULT DcxMDialog::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxMDialog::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_DESTROY:
      {
				WNDPROC wnd = this->m_DefaultWindowProc;
				HWND mHwnd = this->m_Hwnd;
				this->m_DeleteByDestroy = TRUE;
        delete this;
				ShowWindow(mHwnd, SW_HIDE);
				CallWindowProc(wnd, mHwnd, uMsg, wParam, lParam);
        bParsed = TRUE;
      }
      break;

    default:
			return this->CommonMessage( uMsg, wParam, lParam, bParsed);
      break;
  }
  return 0L;
}
