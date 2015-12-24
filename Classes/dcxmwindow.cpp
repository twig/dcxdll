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
#include "defines.h"
#include "Classes/dcxmwindow.h"
#include "Classes/dcxdialog.h"

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

DcxMWindow::DcxMWindow(const HWND cHwnd, const HWND pHwnd, const UINT ID, DcxDialog *const p_Dialog, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	this->m_Hwnd = cHwnd;
	this->m_OrigParentHwnd = GetParent(this->m_Hwnd);

	this->m_OrigStyles = this->removeStyle(WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);

	this->m_OrigExStyles = this->removeExStyle(WS_EX_CONTROLPARENT | WS_EX_MDICHILD);

	this->addStyle(WS_CHILD);

	SetParent(this->m_Hwnd, pHwnd);
	SetWindowPos(this->m_Hwnd, nullptr, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, NULL);
	ShowWindow(this->m_Hwnd, SW_SHOWNOACTIVATE);
	UpdateWindow(this->m_Hwnd);

	this->m_OrigID = SetWindowLong(this->m_Hwnd, GWL_ID, ID);

	this->registreDefaultWindowProc();
	SetProp(this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief Destructor
 *
 * blah
 */

DcxMWindow::~DcxMWindow( ) {

	auto parent = GetParent(this->m_Hwnd);
	if ( parent == this->m_OrigParentHwnd && this->m_OrigParentHwnd != this->m_pParentDialog->getHwnd())
		return;

	this->unregistreDefaultWindowProc( );

	auto bHide = (IsWindowVisible(this->m_Hwnd) != FALSE);
	if ( !bHide )
		ShowWindow( this->m_Hwnd, SW_HIDE );

	SetWindowLong( this->m_Hwnd, GWL_ID, this->m_OrigID );
	//SetParent( this->m_Hwnd, this->m_OrigParentHwnd );
	//this->removeStyle(WS_CHILDWINDOW);
	//SetParent( this->m_Hwnd, nullptr );
	if (parent == this->m_OrigParentHwnd) // handles oddness where orig parent == current when it shouldnt, maybe due to init event docking.
		parent = GetParent(parent);
	else
		parent = this->m_OrigParentHwnd;

	SetParent( this->m_Hwnd, parent );
	this->setStyle( this->m_OrigStyles );
	this->setExStyle( this->m_OrigExStyles );

	SetWindowPos( this->m_Hwnd, nullptr, 30, 30, 0, 0, SWP_NOSIZE|SWP_FRAMECHANGED );
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

void DcxMWindow::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	//  auto numtok = input.numtok( );

	// [NAME] [ID] [PROP]
	if (input.gettok(3) == TEXT("wname")) {
		dcx_strcpyn(szReturnValue, this->m_OrigName.to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxMWindow::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.gettok(3));

	this->parseGlobalCommandRequest( input, flags );
}

void DcxMWindow::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme) {
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
LRESULT DcxMWindow::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxMWindow::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

		//case WM_GETDLGCODE:
		//	{
		//		bParsed = TRUE;
		//		return DLGC_WANTALLKEYS;
		//	}
		//	break;

		case WM_DESTROY:
			{
				delete this;
				bParsed = TRUE;
			}
			break;

		default:
			return this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return 0L;
}
