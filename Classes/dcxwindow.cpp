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

DcxWindow::DcxWindow(const HWND mHwnd, const UINT mID) noexcept
	: m_Hwnd(mHwnd), m_ID(mID), m_hZeroRgn(CreateRectRgn(0, 0, 0, 0))
{
}

/*!
 * \brief Constructor
 *
 * \param mID Window ID
 */

DcxWindow::DcxWindow(const UINT mID) noexcept
	: DcxWindow(nullptr, mID)
{
}

/*!
 * \brief Destructor
 *
 * Destructor
 */

DcxWindow::~DcxWindow() noexcept
{
	if (m_hZeroRgn)
		DeleteRgn(m_hZeroRgn);
}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxWindow::isStyle(const WindowStyle Styles) const noexcept
{
	return dcx_testflag(dcxGetWindowStyle(m_Hwnd), Styles);	// this makes sure ALL flags match not just some.
}

/*!
 * \brief blah
 *
 * blah
 */

WindowStyle DcxWindow::removeStyle(const WindowStyle Styles) noexcept
{
	const auto winStyles = dcxGetWindowStyle(m_Hwnd);
	return dcxSetWindowStyle(m_Hwnd, winStyles & ~Styles);
}

/*!
 * \brief blah
 *
 * blah
 */

WindowStyle DcxWindow::addStyle(const WindowStyle Styles) noexcept
{
	const auto winStyles = dcxGetWindowStyle(m_Hwnd);
	return dcxSetWindowStyle(m_Hwnd, winStyles | Styles);
}

/*!
 * \brief blah
 *
 * blah
 */

WindowStyle DcxWindow::setStyle(const WindowStyle Styles) noexcept
{
	return dcxSetWindowStyle(m_Hwnd, Styles);
}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxWindow::isExStyle(const WindowExStyle Styles) const noexcept
{
	return dcx_testflag(dcxGetWindowExStyle(m_Hwnd), Styles);
}

/*!
 * \brief blah
 *
 * blah
 */

WindowExStyle DcxWindow::removeExStyle(const WindowExStyle Styles) noexcept
{
	const auto winStyles = dcxGetWindowExStyle(m_Hwnd);
	return dcxSetWindowExStyle(m_Hwnd, winStyles & ~Styles);
}

/*!
 * \brief blah
 *
 * blah
 */

WindowExStyle DcxWindow::addExStyle(const WindowExStyle Styles) noexcept
{
	const auto winStyles = dcxGetWindowExStyle(m_Hwnd);
	return dcxSetWindowExStyle(m_Hwnd, winStyles | Styles);
}

/*!
 * \brief blah
 *
 * blah
 */

WindowExStyle DcxWindow::setExStyle(const WindowExStyle Styles) noexcept
{
	return dcxSetWindowExStyle(m_Hwnd, Styles);
}

dcxWindowStyles DcxWindow::parseBorderStyles(const TString& tsFlags) noexcept
{
	const XSwitchFlags xflags(tsFlags);
	WindowStyle Styles(WindowStyle::None);
	WindowExStyle ExStyles(WindowExStyle::None);

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return { Styles, ExStyles };

	if (xflags[TEXT('b')])
		Styles |= WS_BORDER;
	if (xflags[TEXT('c')])
		ExStyles |= WS_EX_CLIENTEDGE;
	if (xflags[TEXT('d')])
		Styles |= WS_DLGFRAME;
	if (xflags[TEXT('f')])
		ExStyles |= WS_EX_DLGMODALFRAME;
	if (xflags[TEXT('s')])
		ExStyles |= WS_EX_STATICEDGE;
	if (xflags[TEXT('w')])
		ExStyles |= WS_EX_WINDOWEDGE;

	return { Styles, ExStyles };
}

/*!
 * \brief blah
 *
 * blah
 */

const UINT& DcxWindow::getID() const noexcept
{
	return m_ID;
}

/*!
 * \brief blah
 *
 * blah
 */

const HWND& DcxWindow::getHwnd() const noexcept
{
	return m_Hwnd;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxWindow::redrawWindow() const noexcept
{
	RedrawWindow(m_Hwnd, nullptr, nullptr, RDW_INTERNALPAINT | RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_ERASE/*|RDW_FRAME|RDW_UPDATENOW*/);
}

/*
 * Redraw Window using a buffered method to avoid flicker of children etc..
 * Vista Style has issues with this, as it relies on the WM_PAINT, anyway, its not needed for Vista style.
 * Composited windows already have buffered draw.
*/
// TODO: this changes the border style of children upon resize of dialog.
void DcxWindow::redrawBufferedWindow()
{
	if (this->isExStyle(WindowExStyle::Composited))
	{
		this->redrawWindow();
		return;
	}

	const auto hdc = GetWindowDC(m_Hwnd);

	if (!hdc)
		return;
	Auto(ReleaseDC(m_Hwnd, hdc));

#if DCX_USE_WRAPPERS
	const Dcx::dcxWindowRect rc(m_Hwnd);

	const Dcx::dcxHDCBuffer hBuffer(hdc, rc);

	SendMessage(m_Hwnd, WM_PRINT, reinterpret_cast<WPARAM>(hBuffer.get()), PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE | PRF_ERASEBKGND);

	BitBlt(hdc, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), hBuffer.get(), 0, 0, SRCCOPY);
#else
	if (RECT rc{}; GetWindowRect(m_Hwnd, &rc))
	{
		if (const auto hBuffer = CreateHDCBuffer(hdc, &rc); hBuffer)
		{
			Auto(DeleteHDCBuffer(hBuffer));

			SendMessage(m_Hwnd, WM_PRINT, (WPARAM)* hBuffer, PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE | PRF_ERASEBKGND);

			BitBlt(hdc, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), *hBuffer, 0, 0, SRCCOPY);
		}
	}
#endif
	ValidateRect(m_Hwnd, nullptr);
}

void DcxWindow::redrawBufferedWindowClient()
{
	if (this->isExStyle(WindowExStyle::Composited))
	{
		this->redrawWindow();
		return;
	}

	const auto hdc = GetDC(m_Hwnd);

	if (!hdc)
		return;
	Auto(ReleaseDC(m_Hwnd, hdc));

#if DCX_USE_WRAPPERS
	const Dcx::dcxClientRect rc(m_Hwnd);

	const Dcx::dcxHDCBuffer hBuffer(hdc, rc);

	SendMessage(m_Hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hBuffer.get()), PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE | PRF_ERASEBKGND);

	BitBlt(hdc, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), hBuffer.get(), 0, 0, SRCCOPY);
#else
	if (RECT rc{}; GetClientRect(m_Hwnd, &rc))
	{
		if (const auto hBuffer = CreateHDCBuffer(hdc, &rc); hBuffer)
		{
			Auto(DeleteHDCBuffer(hBuffer));

			SendMessage(m_Hwnd, WM_PRINTCLIENT, (WPARAM)*hBuffer, PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE | PRF_ERASEBKGND);

			BitBlt(hdc, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), *hBuffer, 0, 0, SRCCOPY);
		}
	}
#endif
	ValidateRect(m_Hwnd, nullptr);
}

void DcxWindow::HandleChildSizing(SizingTypes sz) const noexcept
{
	if (dcx_testflag(sz, SizingTypes::ReBar))
		for (auto bars = FindWindowEx(m_Hwnd, nullptr, DCX_REBARCTRLCLASS, nullptr); bars; bars = FindWindowEx(m_Hwnd, bars, DCX_REBARCTRLCLASS, nullptr))
			SendMessage(bars, WM_SIZE, 0U, 0);

	if (dcx_testflag(sz, SizingTypes::Status))
		for (auto bars = FindWindowEx(m_Hwnd, nullptr, DCX_STATUSBARCLASS, nullptr); bars; bars = FindWindowEx(m_Hwnd, bars, DCX_STATUSBARCLASS, nullptr))
			SendMessage(bars, WM_SIZE, 0U, 0);

	if (dcx_testflag(sz, SizingTypes::Panel))
		for (auto bars = FindWindowEx(m_Hwnd, nullptr, DCX_PANELCLASS, nullptr); bars; bars = FindWindowEx(m_Hwnd, bars, DCX_PANELCLASS, nullptr))
			SendMessage(bars, WM_SIZE, 0U, 0);

	if (dcx_testflag(sz, SizingTypes::Toolbar))
		for (auto bars = FindWindowEx(m_Hwnd, nullptr, DCX_TOOLBARCLASS, nullptr); bars; bars = FindWindowEx(m_Hwnd, bars, DCX_TOOLBARCLASS, nullptr))
			SendMessage(bars, WM_SIZE, 0U, 0);
}

/*!
 * \brief blah
 *
 * blah
 */

PTCHAR DcxWindow::parseCursorType(const TString& cursor)
{
	const static std::map<std::hash<TString>::result_type, PTCHAR> IDC_map{
		{ TEXT("appstarting"_hash), IDC_APPSTARTING },
		{ TEXT("arrow"_hash), IDC_ARROW },
		{ TEXT("cross"_hash), IDC_CROSS },
		{ TEXT("hand"_hash), IDC_HAND },
		{ TEXT("help"_hash), IDC_HELP },
		{ TEXT("ibeam"_hash), IDC_IBEAM },
		{ TEXT("no"_hash), IDC_NO },
		{ TEXT("sizeall"_hash), IDC_SIZEALL },
		{ TEXT("sizenesw"_hash), IDC_SIZENESW },
		{ TEXT("sizens"_hash), IDC_SIZENS },
		{ TEXT("sizenwse"_hash), IDC_SIZENWSE },
		{ TEXT("sizewe"_hash), IDC_SIZEWE },
		{ TEXT("uparrow"_hash), IDC_UPARROW },
		{ TEXT("wait"_hash), IDC_WAIT }
	};

	if (const auto got = IDC_map.find(std::hash<TString>{}(cursor)); got != IDC_map.end())
		return got->second;
	return nullptr;
}

/*!
 * \brief blah
 *
 * blah
 */

DcxResourceFlags DcxWindow::parseCursorFlags(const TString& flags) noexcept
{
	DcxResourceFlags iFlags{ DcxResourceFlags::None };
	const XSwitchFlags xflags(flags);

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('f')])	// load file
		iFlags = DcxResourceFlags::FROMFILE;
	else if (xflags[TEXT('r')])	// load resource
		iFlags = DcxResourceFlags::FROMRESSOURCE;

	return iFlags;
}

UINT DcxWindow::parseCursorArea(const TString& flags) noexcept
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
	if (xflags[TEXT('a')])	// caption area
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

HIMAGELIST DcxWindow::createImageList(const bool bBigIcons) noexcept
{
	const auto sz = (bBigIcons ? DcxIconSizes::LargeIcon : DcxIconSizes::SmallIcon);

	auto himl = ImageList_Create(gsl::narrow_cast<int>(sz), gsl::narrow_cast<int>(sz), ILC_COLOR32 | ILC_MASK, 1, 0);

	if (himl)
		ImageList_SetBkColor(himl, RGB(255, 255, 255));

	return himl;
}

//LRESULT DcxWindow::CallDefaultProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
//{
//	if (!m_hDefaultWindowProc)
//		return DefWindowProc(mHwnd, uMsg, wParam, lParam);
//
//	return CallWindowProc(m_hDefaultWindowProc, mHwnd, uMsg, wParam, lParam);
//}
