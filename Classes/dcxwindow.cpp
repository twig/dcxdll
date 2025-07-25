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
 * � ScriptsDB.org - 2006
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
	: m_Hwnd(mHwnd), m_ID(mID)
{
	if (!m_hZeroRgn)
		m_hZeroRgn = CreateRectRgn(0, 0, 0, 0);

	m_uDPI = Dcx::DpiModule.dcxGetDpiForWindow(m_Hwnd);
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

DcxWindow::~DcxWindow() noexcept
{
	//if (m_hZeroRgn)
	//	DeleteRgn(m_hZeroRgn);
}

bool DcxWindow::isStyle(const WindowStyle Styles) const noexcept
{
	return dcx_testflag(dcxGetWindowStyle(m_Hwnd), Styles);	// this makes sure ALL flags match not just some.
}

WindowStyle DcxWindow::removeStyle(const WindowStyle Styles) noexcept
{
	const auto winStyles = dcxGetWindowStyle(m_Hwnd);
	return dcxSetWindowStyle(m_Hwnd, winStyles & ~Styles);
}

WindowStyle DcxWindow::addStyle(const WindowStyle Styles) noexcept
{
	const auto winStyles = dcxGetWindowStyle(m_Hwnd);
	return dcxSetWindowStyle(m_Hwnd, winStyles | Styles);
}

WindowStyle DcxWindow::setStyle(const WindowStyle Styles) noexcept
{
	return dcxSetWindowStyle(m_Hwnd, Styles);
}

bool DcxWindow::isExStyle(const WindowExStyle Styles) const noexcept
{
	return dcx_testflag(dcxGetWindowExStyle(m_Hwnd), Styles);
}

WindowExStyle DcxWindow::removeExStyle(const WindowExStyle Styles) noexcept
{
	const auto winStyles = dcxGetWindowExStyle(m_Hwnd);
	return dcxSetWindowExStyle(m_Hwnd, winStyles & ~Styles);
}

WindowExStyle DcxWindow::addExStyle(const WindowExStyle Styles) noexcept
{
	const auto winStyles = dcxGetWindowExStyle(m_Hwnd);
	return dcxSetWindowExStyle(m_Hwnd, winStyles | Styles);
}

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

const UINT& DcxWindow::getID() const noexcept
{
	return m_ID;
}

void DcxWindow::OpenToolTip() const noexcept
{
	if (!m_HwndTooltip)
		return;

	Dcx::dcxToolTip_PopUp(m_HwndTooltip);
}

const HWND& DcxWindow::getHwnd() const noexcept
{
	return m_Hwnd;
}

void DcxWindow::CloseToolTip() const noexcept
{
	if (!m_HwndTooltip)
		return;

	//TOOLINFO ti{};
	//ti.cbSize = sizeof(TOOLINFO);
	//if (Dcx::dcxToolTip_GetCurrentTool(m_HwndTooltip, &ti))
	//	Dcx::dcxToolTip_TrackActivate(m_HwndTooltip, FALSE, &ti);

	Dcx::dcxToolTip_Pop(m_HwndTooltip);
}

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

	BitBlt(hdc, 0, 0, rc.Width(), rc.Height(), hBuffer.get(), 0, 0, SRCCOPY);
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

	BitBlt(hdc, 0, 0, rc.Width(), rc.Height(), hBuffer.get(), 0, 0, SRCCOPY);
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

void DcxWindow::InvalidateAndUpdate(_In_opt_ LPCRECT prc, _In_ bool bErase) noexcept
{
	if (!m_Hwnd)
		return;

	InvalidateRect(m_Hwnd, prc, bErase);
	UpdateWindow(m_Hwnd);
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

	//auto hDWP = BeginDeferWindowPos(1);
	//if (dcx_testflag(sz, SizingTypes::ReBar))
	//	for (auto bars = FindWindowEx(m_Hwnd, nullptr, DCX_REBARCTRLCLASS, nullptr); bars; bars = FindWindowEx(m_Hwnd, bars, DCX_REBARCTRLCLASS, nullptr))
	//		hDWP = DeferWindowPos(hDWP, bars, nullptr, 0,0,0,0, SWP_NOZORDER| SWP_NOSENDCHANGING| SWP_NOOWNERZORDER| SWP_NOMOVE| SWP_NOACTIVATE);

	//if (dcx_testflag(sz, SizingTypes::Status))
	//	for (auto bars = FindWindowEx(m_Hwnd, nullptr, DCX_STATUSBARCLASS, nullptr); bars; bars = FindWindowEx(m_Hwnd, bars, DCX_STATUSBARCLASS, nullptr))
	//		hDWP = DeferWindowPos(hDWP, bars, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSENDCHANGING | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	//if (dcx_testflag(sz, SizingTypes::Panel))
	//	for (auto bars = FindWindowEx(m_Hwnd, nullptr, DCX_PANELCLASS, nullptr); bars; bars = FindWindowEx(m_Hwnd, bars, DCX_PANELCLASS, nullptr))
	//		hDWP = DeferWindowPos(hDWP, bars, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSENDCHANGING | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	//if (dcx_testflag(sz, SizingTypes::Toolbar))
	//	for (auto bars = FindWindowEx(m_Hwnd, nullptr, DCX_TOOLBARCLASS, nullptr); bars; bars = FindWindowEx(m_Hwnd, bars, DCX_TOOLBARCLASS, nullptr))
	//		hDWP = DeferWindowPos(hDWP, bars, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSENDCHANGING | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	//EndDeferWindowPos(hDWP);
}

void DcxWindow::setEventMask(const TString& tspFlags, const TString& tsnFlags)
{
	const XSwitchFlags xpFlags(tspFlags);
	const XSwitchFlags xnFlags(tsnFlags);

	if (!xpFlags[TEXT('+')] || !xnFlags[TEXT('-')])
		throw DcxExceptions::dcxInvalidFlag();

	auto mask = m_dEventMask;

	if (xpFlags[TEXT('c')])
		mask |= DCX_EVENT_CLICK;
	if (xpFlags[TEXT('d')])
		mask |= DCX_EVENT_DRAG;
	if (xpFlags[TEXT('e')])
		mask |= DCX_EVENT_EDIT;
	if (xpFlags[TEXT('f')])
		mask |= DCX_EVENT_FOCUS;
	if (xpFlags[TEXT('h')])
		mask |= DCX_EVENT_HELP;
	if (xpFlags[TEXT('m')])
		mask |= DCX_EVENT_MOUSE;
	if (xpFlags[TEXT('s')])
		mask |= DCX_EVENT_SIZE;
	if (xpFlags[TEXT('t')])
		mask |= DCX_EVENT_THEME;
	if (xpFlags[TEXT('C')])
		mask |= DCX_EVENT_CLOSE;
	if (xpFlags[TEXT('M')])
		mask |= DCX_EVENT_MOVE;

	if (xnFlags[TEXT('c')])
		mask &= ~DCX_EVENT_CLICK;
	if (xnFlags[TEXT('d')])
		mask &= ~DCX_EVENT_DRAG;
	if (xnFlags[TEXT('e')])
		mask &= ~DCX_EVENT_EDIT;
	if (xnFlags[TEXT('f')])
		mask &= ~DCX_EVENT_FOCUS;
	if (xnFlags[TEXT('h')])
		mask &= ~DCX_EVENT_HELP;
	if (xnFlags[TEXT('m')])
		mask &= ~DCX_EVENT_MOUSE;
	if (xnFlags[TEXT('s')])
		mask &= ~DCX_EVENT_SIZE;
	if (xnFlags[TEXT('t')])
		mask &= ~DCX_EVENT_THEME;
	if (xnFlags[TEXT('C')])
		mask &= ~DCX_EVENT_CLOSE;
	if (xnFlags[TEXT('M')])
		mask &= ~DCX_EVENT_MOVE;

	m_dEventMask = mask;
}

bool DcxWindow::TrackMouseEvents(DWORD events, UINT uTime) const noexcept
{
	//TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT),events,m_Hwnd,HOVER_DEFAULT };

	//UINT uTime{ HOVER_DEFAULT };
	//SystemParametersInfoW(SPI_GETMOUSEHOVERTIME, 0, &uTime, 0);

	TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT),events,m_Hwnd,uTime };

	return _TrackMouseEvent(&tme) != FALSE;
}

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

TString DcxWindow::getBorderStyles() const
{
	TString tsResult;
	if (!m_Hwnd)
		return tsResult;

	const auto Styles = dcxGetWindowStyle(m_Hwnd);
	const auto ExStyles = dcxGetWindowExStyle(m_Hwnd);

	if (dcx_testflag(Styles, WS_BORDER))
		tsResult += TEXT('b');
	if (dcx_testflag(ExStyles, WS_EX_CLIENTEDGE))
		tsResult += TEXT('c');
	if (dcx_testflag(Styles, WS_DLGFRAME))
		tsResult += TEXT('d');
	if (dcx_testflag(ExStyles, WS_EX_DLGMODALFRAME))
		tsResult += TEXT('f');
	if (dcx_testflag(ExStyles, WS_EX_CONTEXTHELP))
		tsResult += TEXT('h');
	if (dcx_testflag(Styles, WS_MAXIMIZEBOX))
		tsResult += TEXT('m');
	if (dcx_testflag(Styles, WS_MINIMIZEBOX))
		tsResult += TEXT('n');
	if (dcx_testflag(ExStyles, WS_EX_TOOLWINDOW))
		tsResult += TEXT('o');
	if (dcx_testflag(ExStyles, WS_EX_STATICEDGE))
		tsResult += TEXT('s');
	if (dcx_testflag(Styles, WS_CAPTION))	// combination of WS_BORDER | WS_DLGFRAME
		tsResult += TEXT('t');
	if (dcx_testflag(ExStyles, WS_EX_WINDOWEDGE))
		tsResult += TEXT('w');
	if (dcx_testflag(Styles, WS_SYSMENU))
		tsResult += TEXT('y');
	if (dcx_testflag(Styles, WS_SIZEBOX))
		tsResult += TEXT('z');
	if (dcx_testflag(ExStyles, WS_EX_COMPOSITED))
		tsResult += TEXT('x');
	if (dcx_testflag(ExStyles, WS_EX_LAYERED))
		tsResult += TEXT('v');

	return tsResult;
}
