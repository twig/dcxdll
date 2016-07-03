/*!
 * \file dcxwindow.cpp
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
#include "dcxwindow.h"

/*!
 * \brief Constructor
 *
 * \param mHwnd Window Handle
 * \param mID Window ID
 */

DcxWindow::DcxWindow( const HWND mHwnd, const UINT mID )
	: m_Hwnd( mHwnd ), m_ID( mID )
{
	if (IDC_map.empty()) {
		IDC_map[TEXT("appstarting")] = IDC_APPSTARTING;
		IDC_map[TEXT("arrow")] = IDC_ARROW;
		IDC_map[TEXT("cross")] = IDC_CROSS;
		IDC_map[TEXT("hand")] = IDC_HAND;
		IDC_map[TEXT("help")] = IDC_HELP;
		IDC_map[TEXT("ibeam")] = IDC_IBEAM;
		IDC_map[TEXT("no")] = IDC_NO;
		IDC_map[TEXT("sizeall")] = IDC_SIZEALL;
		IDC_map[TEXT("sizenesw")] = IDC_SIZENESW;
		IDC_map[TEXT("sizens")] = IDC_SIZENS;
		IDC_map[TEXT("sizenwse")] = IDC_SIZENWSE;
		IDC_map[TEXT("sizewe")] = IDC_SIZEWE;
		IDC_map[TEXT("uparrow")] = IDC_UPARROW;
		IDC_map[TEXT("wait")] = IDC_WAIT;
	}
}

/*!
 * \brief Constructor
 *
 * \param mID Window ID
 */

DcxWindow::DcxWindow( const UINT mID )
: DcxWindow(nullptr, mID)
{
}

/*!
 * \brief Destructor
 *
 * Destructor
 */

DcxWindow::~DcxWindow( ) {


}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxWindow::isStyle( const LONG Styles ) const noexcept {

	//if ( GetWindowStyle( m_Hwnd ) & Styles )
	//	return TRUE;
	//
	//return FALSE;
	return dcx_testflag(GetWindowStyle(m_Hwnd), (DWORD)Styles);	// this makes sure ALL flags match not just some.
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::removeStyle( const LONG Styles ) {

	auto winStyles = GetWindowStyle(m_Hwnd);
	return SetWindowLongPtr( m_Hwnd, GWL_STYLE, winStyles &= ~Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::addStyle( const LONG Styles ) {

	auto winStyles = GetWindowStyle(m_Hwnd);
	return SetWindowLongPtr( m_Hwnd, GWL_STYLE, winStyles |= Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::setStyle( const LONG Styles ) {

	return SetWindowLongPtr( m_Hwnd, GWL_STYLE, Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxWindow::isExStyle( const LONG Styles ) const noexcept {

	//if ( GetWindowExStyle( m_Hwnd ) & Styles )
	//	return TRUE;
	//
	//return FALSE;
	return dcx_testflag(GetWindowExStyle(m_Hwnd), (DWORD)Styles);
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::removeExStyle( const LONG Styles ) {

	auto winStyles = GetWindowExStyle(m_Hwnd);
	return SetWindowLongPtr( m_Hwnd, GWL_EXSTYLE, winStyles &= ~Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::addExStyle( const LONG Styles ) {

	auto winStyles = GetWindowExStyle(m_Hwnd);
	return SetWindowLongPtr( m_Hwnd, GWL_EXSTYLE, winStyles |= Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::setExStyle( const LONG Styles ) {

	return SetWindowLongPtr( m_Hwnd, GWL_EXSTYLE, Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

const UINT &DcxWindow::getID( ) const noexcept {

	return m_ID;
}

/*!
 * \brief blah
 *
 * blah
 */

const HWND &DcxWindow::getHwnd( ) const noexcept {

	return m_Hwnd;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxWindow::redrawWindow( ) {

	RedrawWindow( m_Hwnd, nullptr, nullptr, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE/*|RDW_FRAME|RDW_UPDATENOW*/ );
}

/*
 * Redraw Window using a buffered method to avoid flicker of children etc..
 * Vista Style has issues with this, as it relies on the WM_PAINT, anyway, its not needed for Vista style.
 * Composited windows already have buffered draw.
*/
// TODO: this changes the border style of children upon resize of dialog.
void DcxWindow::redrawBufferedWindow( ) {

	if (this->isExStyle(WS_EX_COMPOSITED)) {
		this->redrawWindow();
		return;
	}

	auto hdc = GetWindowDC(m_Hwnd);

	if (hdc == nullptr)
		return;
	Auto(ReleaseDC(m_Hwnd, hdc));

	RECT rc;
	if (GetWindowRect(m_Hwnd, &rc))
	{
#if DCX_USE_WRAPPERS
		Dcx::dcxHDCBuffer hBuffer(hdc, rc);

		SendMessage(m_Hwnd, WM_PRINT, (WPARAM)(HDC)hBuffer, PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE | PRF_ERASEBKGND);

		BitBlt(hdc, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), hBuffer, 0, 0, SRCCOPY);
#else
		auto hBuffer = CreateHDCBuffer(hdc, &rc);

		if (hBuffer != nullptr) {
			Auto(DeleteHDCBuffer(hBuffer));

			SendMessage(m_Hwnd, WM_PRINT, (WPARAM)*hBuffer, PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE | PRF_ERASEBKGND);

			BitBlt(hdc, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), *hBuffer, 0, 0, SRCCOPY);
		}
#endif
	}
	ValidateRect(m_Hwnd, nullptr);
}

/*!
 * \brief blah
 *
 * blah
 */
std::map<TString, PTCHAR> DcxWindow::IDC_map;

PTCHAR DcxWindow::parseCursorType( const TString & cursor )
{
	auto got = IDC_map.find(cursor);

	if (got != IDC_map.end())
		return got->second;
	return nullptr;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxWindow::parseCursorFlags(const TString &flags) {
	UINT iFlags = 0;
	const XSwitchFlags xflags(flags);

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('f')])	// load file
		iFlags |= DCCS_FROMFILE;
	if (xflags[TEXT('r')])	// load resource
		iFlags |= DCCS_FROMRESSOURCE;

	return iFlags;
}

UINT DcxWindow::parseCursorArea(const TString & flags)
{
	UINT iFlags = 0;
	const XSwitchFlags xflags(flags);

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	// can't use +f or +r
	if (xflags[TEXT('c')])	// client area
		iFlags = HTCLIENT;
	if (xflags[TEXT('m')])	// menu area
		iFlags = HTMENU;
	if (xflags[TEXT('s')])	// sysmenu area
		iFlags = HTSYSMENU;
	if (xflags[TEXT('c')])	// caption area
		iFlags = HTCAPTION;
	if (xflags[TEXT('S')])	// size button area
		iFlags = HTSIZE;
	if (xflags[TEXT('h')])	// horiz scroll area
		iFlags = HTHSCROLL;
	if (xflags[TEXT('v')])	// vert scroll area
		iFlags = HTVSCROLL;
	if (xflags[TEXT('M')])	// min button area
		iFlags = HTMINBUTTON;
	if (xflags[TEXT('x')])	// max button area
		iFlags = HTMAXBUTTON;
	if (xflags[TEXT('C')])	// close button area
		iFlags = HTCLOSE;
	if (xflags[TEXT('n')])	// nowhere area
		iFlags = HTNOWHERE;
	if (xflags[TEXT('H')])	// help button area
		iFlags = HTHELP;
	if (xflags[TEXT('l')])	// left area
		iFlags = HTLEFT;
	if (xflags[TEXT('R')])	// right area
		iFlags = HTRIGHT;
	if (xflags[TEXT('t')])	// top area
		iFlags = HTTOP;
	if (xflags[TEXT('b')])	// bottom area
		iFlags = HTBOTTOM;

	if (xflags[TEXT('L')])	// topleft area
		iFlags = HTTOPLEFT;
	if (xflags[TEXT('Q')])	// topright area
		iFlags = HTTOPRIGHT;
	if (xflags[TEXT('T')])	// bottomleft area
		iFlags = HTBOTTOMLEFT;
	if (xflags[TEXT('B')])	// bottomright area
		iFlags = HTBOTTOMRIGHT;
	if (xflags[TEXT('X')])	// border area
		iFlags = HTBORDER;

	return iFlags;
}

HIMAGELIST DcxWindow::createImageList(const bool bBigIcons)
{
	HIMAGELIST himl = nullptr;
	auto sz = (bBigIcons ? 32 : 16);

	himl = ImageList_Create(sz, sz, ILC_COLOR32 | ILC_MASK, 1, 0);

	if (himl != nullptr)
		ImageList_SetBkColor(himl, RGB(255, 255, 255));

	return himl;
}
