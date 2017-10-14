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
	parseControlStyles(styles);

	m_Hwnd = cHwnd;
	this->m_OrigParentHwnd = GetParent(m_Hwnd);

	this->m_OrigStyles = this->removeStyle(WindowStyle::Caption | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);

	this->m_OrigExStyles = this->removeExStyle(WindowExStyle::ControlParent | WS_EX_MDICHILD);

	this->addStyle(WindowStyle::Child);

	SetParent(m_Hwnd, pHwnd);
	SetWindowPos(m_Hwnd, nullptr, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, NULL);
	ShowWindow(m_Hwnd, SW_SHOWNOACTIVATE);
	UpdateWindow(m_Hwnd);

	this->m_OrigID = dcxSetWindowID(m_Hwnd, ID);

	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief Destructor
 *
 * blah
 */

DcxMWindow::~DcxMWindow( )
{
	auto parent = GetParent(m_Hwnd);
	if ( parent == this->m_OrigParentHwnd && this->m_OrigParentHwnd != this->m_pParentDialog->getHwnd())
		return;

	this->unregistreDefaultWindowProc( );

	auto bHide = (IsWindowVisible(m_Hwnd) != FALSE);
	if ( !bHide )
		ShowWindow( m_Hwnd, SW_HIDE );

	dcxSetWindowID( m_Hwnd, m_OrigID );

	//SetParent( m_Hwnd, this->m_OrigParentHwnd );
	//this->removeStyle(WS_CHILDWINDOW);
	//SetParent( m_Hwnd, nullptr );

	if (parent == this->m_OrigParentHwnd) // handles oddness where orig parent == current when it shouldnt, maybe due to init event docking.
		parent = GetParent(parent);
	else
		parent = this->m_OrigParentHwnd;

	SetParent( m_Hwnd, parent );
	this->setStyle( this->m_OrigStyles );
	this->setExStyle( this->m_OrigExStyles );

	SetWindowPos( m_Hwnd, nullptr, 30, 30, 0, 0, SWP_NOSIZE|SWP_FRAMECHANGED );
	UpdateWindow( m_Hwnd );

	if ( !bHide )
		ShowWindow( m_Hwnd, SW_SHOW );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxMWindow::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	//  auto numtok = input.numtok( );

	// [NAME] [ID] [PROP]
	if (input.gettok(3) == TEXT("wname"))
		szReturnValue = m_OrigName.to_chr();
	else
		parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxMWindow::parseCommandRequest( const TString & input )
{
	const XSwitchFlags flags(input.gettok(3));

	this->parseGlobalCommandRequest( input, flags );
}

//void DcxMWindow::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
//{
//	this->m_OrigName = styles;
//}

std::tuple<NoTheme, WindowStyle, WindowExStyle> DcxMWindow::parseControlStyles(const TString & tsStyles)
{
	m_OrigName = tsStyles;

	return{ false, WindowStyle::None, WindowExStyle::None };
}

/*!
 * \brief Subclassed Window Procedure
 *
 * \param uMsg Window Procedure Message ID
 * \param wParam Window Procedure WPARAM
 * \param lParam Window Procedure LPARAM
 * \param bParsed Indicates if subclassed procedure parsed the message
 */
LRESULT DcxMWindow::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
{
	return 0L;
}

LRESULT DcxMWindow::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed)
{
	switch (uMsg)
	{
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
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}
