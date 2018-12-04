/*!
 * \file dcxcontrol.cpp
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
#include "Dcx.h"

#include "Classes/dcxcontrol.h"
#include "Classes/dcxdialog.h"
#include "Classes/dcxprogressbar.h"
#include "Classes/dcxtrackbar.h"
#include "Classes/dcxtrackbar.h"
#include "Classes/dcxcomboex.h"
#include "Classes/dcxcolorcombo.h"
#include "Classes/dcxstatusbar.h"
#include "Classes/dcxtreeview.h"
#include "Classes/dcxtoolbar.h"
#include "Classes/dcxlistview.h"
#include "Classes/dcxrebar.h"
#include "Classes/dcxbutton.h"
#include "Classes/dcxrichedit.h"
#include "Classes/dcxupdown.h"
#include "Classes/dcxipaddress.h"
#include "Classes/dcxwebctrl.h"
#include "Classes/dcxcalendar.h"
#include "Classes/dcxdatetime.h"
#include "Classes/dcxpager.h"
#include "Classes/dcxdivider.h"
#include "Classes/dcxpanel.h"
#include "Classes/dcxtab.h"
#include "Classes/mirc/dcxbox.h"
#include "Classes/mirc/dcxline.h"
#include "Classes/mirc/dcxline.h"
#include "Classes/mirc/dcxradio.h"
#include "Classes/mirc/dcxcheck.h"
#include "Classes/mirc/dcxtext.h"
#include "Classes/mirc/dcxedit.h"
#include "Classes/mirc/dcxscroll.h"
#include "Classes/mirc/dcxlist.h"
#include "Classes/mirc/dcxlink.h"
#include "Classes/mirc/dcximage.h"
#include "Classes/dcxmwindow.h"
#include "Classes/dcxmdialog.h"
#include "Classes/dcxstacker.h"

#ifdef DCX_USE_DXSDK
#include "Classes/dcxdirectshow.h"
#endif // DCX_USE_DXSDK

bool DcxControl::m_bInitialized = false;

/*!
 * \brief Constructor
 *
 * \param mID Control ID
 * \param p_Dialog Parent DcxDialog Object
 */

DcxControl::DcxControl(const UINT mID, DcxDialog *const p_Dialog)
	: DcxWindow(mID)
	, m_pParentDialog(p_Dialog)
	, m_UserID(mID - mIRC_ID_OFFSET)
	, m_dEventMask(p_Dialog->getEventMask())	// inherit the parent dialogs event mask
	//, m_ToolTipHWND(p_Dialog->getToolTip())
{
	if (!m_bInitialized)
		throw Dcx::dcxException("DCX Controls NOT Initialized!");
}

/*!
 * \brief Destructor
 *
 * Control Interface Destructor.
 *
 * Frees global control resources.
 */

DcxControl::~DcxControl( )
{
	RemoveProp( m_Hwnd, TEXT("dcx_cthis") );

	// Reverse to old font
	setControlFont(nullptr, FALSE);

	// Delete background brush
	if (m_hBackBrush != nullptr)
	{
		DeleteBrush( m_hBackBrush );
		m_hBackBrush = nullptr;
	}
	// Delete border brush
	if (m_hBorderBrush != nullptr)
	{
		DeleteBrush( m_hBorderBrush );
		m_hBorderBrush = nullptr;
	}

	// check if we need to destroy the cursor (do not destroy if same cursor as parent dialog, parent will destroy this for us)
	if (m_bCursorFromFile && m_hCursor != nullptr && m_hCursor != getParentDialog()->getCursor())
	{
		DestroyCursor( m_hCursor );
		m_hCursor = nullptr;
	}

	getParentDialog()->deleteControl( this );
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxControl::parseGeneralControlStyles(const TString & styles, dcxWindowStyles &ws)
{
	ws.m_Styles |= WS_CLIPCHILDREN | WS_VISIBLE;

	for (const auto &tsStyle : styles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"notheme"_hash:
			ws.m_NoTheme = true;
			break;
		case L"tabstop"_hash:
			ws.m_Styles |= WS_TABSTOP;
			break;
		case L"group"_hash:
			ws.m_Styles |= WS_GROUP;
			break;
		case L"disabled"_hash:
			ws.m_Styles |= WS_DISABLED;
			break;
		case L"transparent"_hash:
			ws.m_ExStyles |= WS_EX_TRANSPARENT;
			break;
		case L"hidden"_hash:
			ws.m_Styles &= ~WS_VISIBLE;
			break;
		case L"alpha"_hash:
			m_bAlphaBlend = true;
			break;
		case L"shadow"_hash:
			m_bShadowText = true;
			break;
		case L"noformat"_hash:
			m_bCtrlCodeText = false;
			break;
		case L"hgradient"_hash:
			m_bGradientFill = true;
			break;
		case L"vgradient"_hash:
		{
			m_bGradientFill = true;
			m_bGradientVertical = true;
		}
		break;
		default:
			break;
		}
	}
	return ws;
}

dcxWindowStyles DcxControl::parseGeneralControlStyles(const TString & styles)
{
	//dcxWindowStyles ws;
	//
	//ws.m_Styles |= WS_CLIPCHILDREN | WS_VISIBLE;
	//
	//for (const auto &tsStyle : styles)
	//{
	//	switch (std::hash<TString>{}(tsStyle))
	//	{
	//	case L"notheme"_hash:
	//		ws.m_NoTheme = true;
	//		break;
	//	case L"tabstop"_hash:
	//		ws.m_Styles |= WS_TABSTOP;
	//		break;
	//	case L"group"_hash:
	//		ws.m_Styles |= WS_GROUP;
	//		break;
	//	case L"disabled"_hash:
	//		ws.m_Styles |= WS_DISABLED;
	//		break;
	//	case L"transparent"_hash:
	//		ws.m_ExStyles |= WS_EX_TRANSPARENT;
	//		break;
	//	case L"hidden"_hash:
	//		ws.m_Styles &= static_cast<DWORD>(~WS_VISIBLE);
	//		break;
	//	case L"alpha"_hash:
	//		m_bAlphaBlend = true;
	//		break;
	//	case L"shadow"_hash:
	//		m_bShadowText = true;
	//		break;
	//	case L"noformat"_hash:
	//		m_bCtrlCodeText = false;
	//		break;
	//	case L"hgradient"_hash:
	//		m_bGradientFill = true;
	//		break;
	//	case L"vgradient"_hash:
	//	{
	//		m_bGradientFill = true;
	//		m_bGradientVertical = true;
	//	}
	//	break;
	//	default:
	//		break;
	//	}
	//}
	//return ws;

	dcxWindowStyles ws;

	return parseGeneralControlStyles(styles, ws);
}

/*!
 * \brief blah
 *
 * NB: Possible buffer overwrite condition when returned data is longer than allocated szReturn
 */

bool DcxControl::evalAliasEx( TCHAR *const szReturn, const int maxlen, const TCHAR *const szFormat, ... )
{
	TString parms;
	va_list args = nullptr;

	va_start( args, szFormat );
	parms.tvprintf(szFormat, args );
	va_end( args );

	return getParentDialog()->evalAlias(refString<TCHAR, MIRC_BUFFER_SIZE_CCH>(szReturn), maxlen, parms.to_chr());
	//return getParentDialog()->evalAlias(szReturn, maxlen, parms.to_chr());
}

bool DcxControl::execAliasEx( const TCHAR *const szFormat, ... )
{
	TString parms;
	va_list args = nullptr;

	va_start( args, szFormat );
	parms.tvprintf(szFormat, args);
	va_end( args );

	return getParentDialog()->execAlias(parms.to_chr());
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::parseGlobalCommandRequest( const TString & input, const XSwitchFlags & flags ) {
	const auto numtok = input.numtok();

	// xdid -f [NAME] [ID] [SWITCH] [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
	if ( flags[TEXT('f')] && numtok > 3 )
	{
		if (LOGFONT lf{ }; ParseCommandToLogfont(input.gettok(4, -1), &lf))
			setControlFont(CreateFontIndirect(&lf), FALSE);

		redrawWindow( );
	}
	// xdid -p [NAME] [ID] [SWITCH] [X] [Y] [W] [H]
	else if ( flags[TEXT('p')] && numtok > 6 )
	{
		const auto x = input.getfirsttok(4).to_int();
		const auto y = input.getnexttok().to_int();	// tok 5
		const auto w = input.getnexttok().to_int();	// tok 6
		const auto h = input.getnexttok().to_int();	// tok 7

		MoveWindow( m_Hwnd, x, y, w, h, TRUE );
		//this->InvalidateParentRect( m_Hwnd);
		//InvalidateRect( GetParent( m_Hwnd ), nullptr, TRUE );
		//this->redrawWindow( );
		//this->getParentDialog()->redrawBufferedWindow();
		//SendMessage( m_Hwnd, WM_NCPAINT, (WPARAM) 1, (LPARAM) 0 );
	}
	// xdid -x [NAME] [ID] [SWITCH] [+FLAGS]
	else if ( flags[TEXT('x')] && numtok > 3 )
	{
		this->removeStyle(WindowStyle::Border | WS_DLGFRAME);
		this->removeExStyle(WindowExStyle::ClientEdge | WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE);

		const auto[Styles, ExStyles] = parseBorderStyles( input.gettok( 4 ) );

		this->addStyle( Styles );
		this->addExStyle( ExStyles );

		SetWindowPos(m_Hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		InvalidateRect(m_Hwnd, nullptr, TRUE);
		SendMessage( m_Hwnd, WM_NCPAINT, (WPARAM) 1, (LPARAM) 0 );
	}
	// xdid -C [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if ( flags[TEXT('C')] && numtok > 4 )
	{
		const auto iFlags = this->parseColorFlags(input.getfirsttok(4));
		const auto clrColor = input.getnexttok().to_<COLORREF>();	// tok 5

		if (dcx_testflag(iFlags, DCC_BKGCOLOR))
		{
			if (this->m_hBackBrush != nullptr)
			{
				DeleteBrush( this->m_hBackBrush );
				this->m_hBackBrush = nullptr;
				this->m_clrBackground = CLR_INVALID;
			}

			if ( clrColor != CLR_INVALID )
			{
				this->m_hBackBrush = CreateSolidBrush( clrColor );
				this->m_clrBackground = clrColor;
			}
		}

		if (dcx_testflag(iFlags, DCC_TEXTCOLOR))
			this->m_clrText = clrColor;

		if (dcx_testflag(iFlags, DCC_TEXTBKGCOLOR))
			this->m_clrBackText = clrColor;

		if (dcx_testflag(iFlags, DCC_BORDERCOLOR))
		{
			if (this->m_hBorderBrush != nullptr)
			{
				DeleteBrush( this->m_hBorderBrush );
				this->m_hBorderBrush = nullptr;
			}

			if ( clrColor != CLR_INVALID )
				this->m_hBorderBrush = CreateSolidBrush( clrColor );
		}

		if (dcx_testflag(iFlags, DCC_GRADSTARTCOLOR))
			this->m_clrStartGradient = clrColor;

		if (dcx_testflag(iFlags, DCC_GRADENDCOLOR))
			this->m_clrEndGradient = clrColor;

		// force a control redraw
		this->redrawWindow( );
	}
	// xdid -F [NAME] [ID] [SWITCH]
	else if (flags[TEXT('F')])
		SetFocus(m_Hwnd);
	// xdid -J [NAME] [ID] [SWITCH] [+FLAGS] [CURSOR|FILENAME]
	// xdid -J DNAME ID [+FLAGS] [CURSOR|FILENAME]
	else if ( flags[TEXT('J')] && numtok > 4 )
	{
		const auto iFlags = this->parseCursorFlags( input.getfirsttok( 4 ) );
		auto filename(input.getlasttoks());
		const auto CursorType = this->parseCursorType(filename);

		// if previous cursor was the dialogs cursor, just set as blank
		if (m_hCursor == getParentDialog()->getCursor())
			m_hCursor = nullptr;

		this->m_hCursor = Dcx::dcxLoadCursor(iFlags, CursorType, this->m_bCursorFromFile, this->m_hCursor, filename);
	}
	// xdid -M [NAME] [ID] [SWITCH] [MARK INFO]
	else if (flags[TEXT('M')])
	{
		TString info;

		if (numtok > 3)
			info = input.gettok(4, -1).trim();

		this->m_tsMark = info;
	}
	// xdid -Z [NAME] [ID] [SWITCH] [%]
	else if ( flags[TEXT('Z')] && numtok > 3 )
	{
		const auto perc = (BYTE)(input.getfirsttok(4).to_int() & 0xFF);

		if (perc > 100)
			throw Dcx::dcxException("Invalid Percentage");

		int min = 0, max = 0;
		if (!GetScrollRange(m_Hwnd, SB_VERT, &min, &max))
			throw Dcx::dcxException("Unable to get scrollbar min/max");

		//scrollbar is defined and has visible range
		if (min != 0 || max != 0)
		{
			const auto pos = dcx_round((float)(max - min) * (float)perc / (float) 100.0);
			//const auto pos = Dcx::dcxRound<int>((max - min) * perc / 100.0);

			//SCROLLINFO si;
			//ZeroMemory(&si, sizeof (SCROLLINFO));
			//si.cbSize = sizeof(SCROLLINFO);
			//si.fMask = SIF_POS;

			SCROLLINFO si{ sizeof(SCROLLINFO), SIF_POS,0,0,0,0,0 };

			if (!GetScrollInfo(m_Hwnd, SB_VERT, &si))
				throw Dcx::dcxException("Unable to get scroll info");

			si.nPos = pos;
			SetScrollInfo(m_Hwnd, SB_VERT, &si, TRUE);
			SendMessage(m_Hwnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, si.nPos), NULL);
		}
	}

	// xdid -b [NAME] [ID]
	else if ( flags[TEXT('b')] )
		EnableWindow( m_Hwnd, FALSE );
	// xdid -e [NAME] [ID]
	else if ( flags[TEXT('e')] )
		EnableWindow( m_Hwnd, TRUE );
	// xdid -h [NAME] [ID] [SWITCH] (+FLAGS) (DURATION)
	else if (flags[TEXT('h')])
	{
		if (numtok > 4)
			AnimateWindow(m_Hwnd,
				input.gettok(5).to_dword(),
				static_cast<DWORD>((AW_HIDE | DcxDialog::getAnimateStyles(input.gettok(4))) & ~AW_ACTIVATE));
		else
			ShowWindow(m_Hwnd, SW_HIDE);

		// shouldnt need to update layout on a hidden window....
		//RECT rc;
		//if (GetClientRect(this->getParentDialog()->getHwnd(), &rc))
		//{
		//	if (this->getParentDialog()->updateLayout(rc))
		//		this->getParentDialog()->redrawWindow(); // why do we need the redraw?
		//}
	}
	// xdid -s [NAME] [ID] [SWITCH] (+FLAGS) (DURATION)
	else if ( flags[TEXT('s')] )
	{
		if (numtok > 4)
		{
			AnimateWindow(m_Hwnd,
				input.gettok(5).to_dword(),
				static_cast<DWORD>((AW_ACTIVATE | DcxDialog::getAnimateStyles(input.gettok( 4 ))) & ~AW_HIDE));
		}
		else
			ShowWindow(m_Hwnd, SW_SHOW);

		if (RECT rc{}; GetClientRect(this->getParentDialog()->getHwnd(), &rc))
		{
			if (this->getParentDialog()->updateLayout(rc))
				this->getParentDialog()->redrawWindow(); // why do we need the redraw?
		}
	}
	// xdid -U [NAME] [ID]
	else if (flags[TEXT('U')])
	{
		// Box Double click Bug: the GetNextDlgtabItem() function never returns & seems to just loop forever.
		// from functions doc:
		//	If the search for the next control with the WS_TABSTOP
		//	style encounters a window with the WS_EX_CONTROLPARENT style,
		//	the system recursively searches the window's children.
		//
		//HWND hNextCtrl = GetNextDlgTabItem(this->getParentDialog()->getHwnd(), m_Hwnd, FALSE);
        //
		//if (hNextCtrl && (hNextCtrl != m_Hwnd))
		//	SendMessage(this->getParentDialog()->getHwnd(), WM_NEXTDLGCTL, (WPARAM) hNextCtrl, TRUE);
		////::PostMessage(this->getParentDialog()->getHwnd(), WM_NEXTDLGCTL, NULL, FALSE);
		//else
		//	SetFocus(nullptr);

		// Just remove focus from all controls
		SetFocus(nullptr);
	}
	// xdid -T [NAME] [ID] [SWITCH] (ToolTipText)
	else if (flags[TEXT('T')] && numtok > 2)
		this->m_tsToolTip = (numtok > 3 ? input.gettok(4, -1).trim() : TEXT(""));
	// xdid -R [NAME] [ID] [SWITCH] [FLAG] [ARGS]
	else if (flags[TEXT('R')] && numtok > 3)
	{
		const XSwitchFlags xflags(input.getfirsttok( 4 ));

		if (!xflags[TEXT('+')])
			throw Dcx::dcxException("Invalid Flag");

		HRGN m_Region = nullptr;
		auto RegionMode = 0;
		auto noRegion = false;

#if DCX_USE_WRAPPERS
		const Dcx::dcxWindowRect rc(m_Hwnd);
#else
		RECT rc = { 0 };
		if (!GetWindowRect(m_Hwnd, &rc))
			throw Dcx::dcxException("Unable to get window rect!");
#endif

		if (xflags[TEXT('o')])
			RegionMode = RGN_OR;
		else if (xflags[TEXT('a')])
			RegionMode = RGN_AND;
		else if (xflags[TEXT('i')])
			RegionMode = RGN_DIFF;
		else if (xflags[TEXT('x')])
			RegionMode = RGN_XOR;

		if (xflags[TEXT('f')]) // image file - [COLOR] [FILE]
		{
			if (numtok < 6)
				throw Dcx::dcxException("Invalid Arguments");

			const auto tCol = input.getnexttok().to_<COLORREF>();		// tok 5
			auto filename(input.getlasttoks());							// tok 6, -1

			auto bitmapRgn = dcxLoadBitmap(nullptr, filename);

			if (bitmapRgn == nullptr)
				throw Dcx::dcxException("Unable To Load Image file.");
			Auto(DeleteBitmap(bitmapRgn));

			if (xflags[TEXT('R')]) // now resize image to match control.
				bitmapRgn = resizeBitmap(bitmapRgn, &rc);
			m_Region = BitmapRegion(bitmapRgn, tCol, (tCol != CLR_INVALID));
		}
		else if (xflags[TEXT('r')]) // rounded rect - radius args (optional)
		{
			const auto radius = (numtok > 4) ? input.getnexttok( ).to_int() : 20;	// tok 5

			m_Region = CreateRoundRectRgn(0,0,rc.right - rc.left,rc.bottom - rc.top, radius, radius);
		}
		else if (xflags[TEXT('c')]) // circle - radius arg (optional)
		{
			if (numtok > 4)
			{
				auto radius = input.getnexttok( ).to_int();	// tok 5
				if (radius < 1)
					radius = 100; // handle cases where arg isnt a number or is a negative.
				const auto cx = ((rc.right - rc.left) / 2);
				const auto cy = ((rc.bottom - rc.top) / 2);
				m_Region = CreateEllipticRgn(cx-radius,cy-radius,cx+radius,cy+radius);
			}
			else
				m_Region = CreateEllipticRgn(0,0,rc.right - rc.left,rc.bottom - rc.top);
		}
		else if (xflags[TEXT('p')]) // polygon
		{
			// u need at least 3 points for a shape
			if (numtok < 7)
				throw Dcx::dcxException("Invalid Arguments");

			const auto strPoints(input.getlasttoks());	// tok 5, -1
			const auto tPoints = strPoints.numtok();

			if (tPoints < 3)
				throw Dcx::dcxException("Invalid Points: At least 3 points required.");

			auto pnts = std::make_unique<POINT[]>(tPoints);
			UINT cnt = 0;

			for (const auto &strPoint: strPoints)
			{
				pnts[cnt].x = strPoint.getfirsttok(1, TSCOMMACHAR).to_<LONG>();
				pnts[cnt].y = strPoint.getnexttok(TSCOMMACHAR).to_<LONG>();	// tok 2
				++cnt;
			}

			m_Region = CreatePolygonRgn(pnts.get(), gsl::narrow_cast<int>(tPoints), WINDING);
		}
		else if (xflags[TEXT('b')])
		{ // alpha [1|0] [level]
			noRegion = true;
			if (numtok != 6)
				throw Dcx::dcxException("Invalid Args");

			m_bAlphaBlend = (input.getnexttok().to_int() > 0);	// tok 5

			const auto alpha = gsl::narrow_cast<BYTE>(input.getnexttok().to_int() & 0xFF);	// tok 6

			if (alpha == 255U)
				m_bAlphaBlend = false;
			m_iAlphaLevel = alpha;
		}
		else {
			if (!xflags[TEXT('n')]) // none, no args
				throw Dcx::dcxException("Invalid Flag");
			
			noRegion = true;
			SetWindowRgn(m_Hwnd, nullptr, FALSE);	// redraw at end
		}

		if (!noRegion)
		{
			if (m_Region == nullptr)
				throw Dcx::dcxException("Unable to create region.");

			if (RegionMode != 0)
			{
				if (auto wrgn = CreateRectRgn(0, 0, 0, 0); wrgn != nullptr)
				{
					Auto(DeleteObject(wrgn));
					if (GetWindowRgn(m_Hwnd,wrgn) != ERROR)
						CombineRgn(m_Region,m_Region,wrgn,RegionMode);
				}
			}
			SetWindowRgn(m_Hwnd,m_Region,FALSE);	// redraw at end
		}
		redrawWindow();
	}
	// invalid command
	else
		throw Dcx::dcxException("Invalid Command");
}

HBITMAP DcxControl::resizeBitmap(HBITMAP srcBM, const RECT *const rc) noexcept
{
#if DCX_USE_WRAPPERS && 0

	if ((srcBM == nullptr) || (rc == nullptr))
		return nullptr;

	// set result to original bitmap incase resize fails at some point.
	auto hRes = srcBM;

	// get source bitmap info.
	BITMAP bm;
	if (GetObject(srcBM, sizeof(bm), &bm) == 0)
		return hRes;

	Dcx::dcxHDCBitmapResource srcDC(nullptr, srcBM);

	auto w = (rc->right - rc->left), h = (rc->bottom - rc->top);

	// create dest bitmap.
	Dcx::dcxHDCBitmap2Resource destDC(srcDC.get(), w, h);

	// resize bitmap, render into dest hdc/bitmap
	if (StretchBlt(destDC, 0, 0, w, h, srcDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY)) {
		GdiFlush();
		// set the return bitmap if resize worked.
		DeleteBitmap(hRes);
		hRes = destDC;
	}
	return hRes;
#else
	// set result to original bitmap incase resize fails at some point.
	auto hRes = srcBM;

	if ((srcBM == nullptr) || (rc == nullptr))
		return nullptr;

	// get source bitmap info.
	BITMAP bm{};
	if (GetObject(srcBM, sizeof(bm), &bm) == 0)
		return hRes;

	// create temp hdc to render from
	const auto srcDC = CreateCompatibleDC(nullptr);
	if (srcDC == nullptr)
		return hRes;

	// delete source hdc
	Auto(DeleteDC(srcDC));

	// select bitmap into new hdc
	const auto oldSrcBm = SelectBitmap(srcDC, srcBM);

	// set source hdc back to orig bitmap.
	Auto(SelectBitmap(srcDC, oldSrcBm));

	// create temp dest hdc to render into.
	const auto destDC = CreateCompatibleDC(srcDC);

	if (destDC == nullptr)
		return hRes;

	// delete dest hdc
	Auto(DeleteDC(destDC));

	const auto w = (rc->right - rc->left), h = (rc->bottom - rc->top);

	// create dest bitmap.
	auto newBM = CreateCompatibleBitmap(srcDC, w, h);

	if (newBM == nullptr)
		return hRes;

	Auto(DeleteBitmap(newBM));

	// select dest bitmap into dest hdc
	const auto oldDestBm = SelectBitmap(destDC, newBM);

	// set dest hdc back to orig bitmap.
	Auto(SelectBitmap(destDC, oldDestBm));

	// resize bitmap, render into dest hdc/bitmap
	if (StretchBlt(destDC, 0, 0, w, h, srcDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY))
	{
		GdiFlush();
		// set the return bitmap if resize worked.
		std::swap(hRes, newBM);
	}
	return hRes;
#endif
}

const static std::map<std::hash<TString>::result_type, DcxControlTypes> dcxTypesMap{
	{TEXT("box"_hash), DcxControlTypes::BOX},
	{TEXT("check"_hash), DcxControlTypes::CHECK},
	{TEXT("edit"_hash), DcxControlTypes::EDIT},
	{TEXT("image"_hash), DcxControlTypes::IMAGE},
	{TEXT("line"_hash), DcxControlTypes::LINE},
	{TEXT("link"_hash), DcxControlTypes::LINK},
	{TEXT("list"_hash), DcxControlTypes::LIST},
	{TEXT("radio"_hash), DcxControlTypes::RADIO},
	{TEXT("scroll"_hash), DcxControlTypes::SCROLL},
	{TEXT("text"_hash), DcxControlTypes::TEXT},
	{TEXT("button"_hash), DcxControlTypes::BUTTON},
	{TEXT("calendar"_hash), DcxControlTypes::CALENDAR},
	{TEXT("colorcombo"_hash), DcxControlTypes::COLORCOMBO},
	{TEXT("comboex"_hash), DcxControlTypes::COMBOEX},
	{TEXT("datetime"_hash), DcxControlTypes::DATETIME},
	{TEXT("directshow"_hash), DcxControlTypes::DIRECTSHOW},
	{TEXT("divider"_hash), DcxControlTypes::DIVIDER},
	{TEXT("ipaddress"_hash), DcxControlTypes::IPADDRESS},
	{TEXT("listview"_hash), DcxControlTypes::LISTVIEW},
	{TEXT("dialog"_hash), DcxControlTypes::DIALOG},
	{TEXT("window"_hash), DcxControlTypes::WINDOW},
	{TEXT("pager"_hash), DcxControlTypes::PAGER},
	{TEXT("panel"_hash), DcxControlTypes::PANEL},
	{TEXT("pbar"_hash), DcxControlTypes::PROGRESSBAR},
	{TEXT("rebar"_hash), DcxControlTypes::REBAR},
	{TEXT("richedit"_hash), DcxControlTypes::RICHEDIT},
	{TEXT("stacker"_hash), DcxControlTypes::STACKER},
	{TEXT("statusbar"_hash), DcxControlTypes::STATUSBAR},
	{TEXT("tab"_hash), DcxControlTypes::TABB},
	{TEXT("toolbar"_hash), DcxControlTypes::TOOLBAR},
	{TEXT("trackbar"_hash), DcxControlTypes::TRACKBAR},
	{TEXT("treeview"_hash), DcxControlTypes::TREEVIEW},
	{TEXT("updown"_hash), DcxControlTypes::UPDOWN},
	{TEXT("webctrl"_hash), DcxControlTypes::WEBCTRL}
};

DcxControlTypes DcxControl::TSTypeToControlType(const TString & t)
{
	//static std::map<TString, DcxControlTypes> dcxTypesMap;
	//
	//if (dcxTypesMap.empty())
	//{
	//	dcxTypesMap[TEXT("box")] = DcxControlTypes::BOX;
	//	dcxTypesMap[TEXT("check")] = DcxControlTypes::CHECK;
	//	dcxTypesMap[TEXT("edit")] = DcxControlTypes::EDIT;
	//	dcxTypesMap[TEXT("image")] = DcxControlTypes::IMAGE;
	//	dcxTypesMap[TEXT("line")] = DcxControlTypes::LINE;
	//	dcxTypesMap[TEXT("link")] = DcxControlTypes::LINK;
	//	dcxTypesMap[TEXT("list")] = DcxControlTypes::LIST;
	//	dcxTypesMap[TEXT("radio")] = DcxControlTypes::RADIO;
	//	dcxTypesMap[TEXT("scroll")] = DcxControlTypes::SCROLL;
	//	dcxTypesMap[TEXT("text")] = DcxControlTypes::TEXT;
	//	dcxTypesMap[TEXT("button")] = DcxControlTypes::BUTTON;
	//	dcxTypesMap[TEXT("calendar")] = DcxControlTypes::CALENDAR;
	//	dcxTypesMap[TEXT("colorcombo")] = DcxControlTypes::COLORCOMBO;
	//	dcxTypesMap[TEXT("comboex")] = DcxControlTypes::COMBOEX;
	//	dcxTypesMap[TEXT("datetime")] = DcxControlTypes::DATETIME;
	//	dcxTypesMap[TEXT("directshow")] = DcxControlTypes::DIRECTSHOW;
	//	dcxTypesMap[TEXT("divider")] = DcxControlTypes::DIVIDER;
	//	dcxTypesMap[TEXT("ipaddress")] = DcxControlTypes::IPADDRESS;
	//	dcxTypesMap[TEXT("listview")] = DcxControlTypes::LISTVIEW;
	//	dcxTypesMap[TEXT("dialog")] = DcxControlTypes::DIALOG;
	//	dcxTypesMap[TEXT("window")] = DcxControlTypes::WINDOW;
	//	dcxTypesMap[TEXT("pager")] = DcxControlTypes::PAGER;
	//	dcxTypesMap[TEXT("panel")] = DcxControlTypes::PANEL;
	//	dcxTypesMap[TEXT("pbar")] = DcxControlTypes::PROGRESSBAR;
	//	dcxTypesMap[TEXT("rebar")] = DcxControlTypes::REBAR;
	//	dcxTypesMap[TEXT("richedit")] = DcxControlTypes::RICHEDIT;
	//	dcxTypesMap[TEXT("stacker")] = DcxControlTypes::STACKER;
	//	dcxTypesMap[TEXT("statusbar")] = DcxControlTypes::STATUSBAR;
	//	dcxTypesMap[TEXT("tab")] = DcxControlTypes::TABB;
	//	dcxTypesMap[TEXT("toolbar")] = DcxControlTypes::TOOLBAR;
	//	dcxTypesMap[TEXT("trackbar")] = DcxControlTypes::TRACKBAR;
	//	dcxTypesMap[TEXT("treeview")] = DcxControlTypes::TREEVIEW;
	//	dcxTypesMap[TEXT("updown")] = DcxControlTypes::UPDOWN;
	//	dcxTypesMap[TEXT("webctrl")] = DcxControlTypes::WEBCTRL;
	//}

	if (const auto got = dcxTypesMap.find(std::hash<TString>()(t)); got != dcxTypesMap.end())
		return got->second;
	return DcxControlTypes::UNKNOWN;
}

/*!
 * \brief blah
 *
 * blah
 */

const UINT DcxControl::parseColorFlags( const TString & flags ) noexcept
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] )
		return iFlags;

	if ( xflags[TEXT('b')] )
		iFlags |= DCC_BKGCOLOR;
	if ( xflags[TEXT('k')] )
		iFlags |= DCC_TEXTBKGCOLOR;
	if ( xflags[TEXT('t')] )
		iFlags |= DCC_TEXTCOLOR;
	if ( xflags[TEXT('r')] )
		iFlags |= DCC_BORDERCOLOR;
	if ( xflags[TEXT('g')] )
		iFlags |= DCC_GRADSTARTCOLOR;
	if ( xflags[TEXT('G')] )
		iFlags |= DCC_GRADENDCOLOR;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

std::pair<WindowStyle,WindowExStyle> DcxControl::parseBorderStyles(const TString & flags) noexcept
{
	const XSwitchFlags xflags(flags);
	WindowStyle Styles(WindowStyle::None);
	WindowExStyle ExStyles(WindowExStyle::None);

	// no +sign, missing params
	if ( !xflags[TEXT('+')] )
		return { Styles, ExStyles };

	if ( xflags[TEXT('b')] )
		Styles |= WS_BORDER;
	if ( xflags[TEXT('c')] )
		ExStyles |= WS_EX_CLIENTEDGE;
	if ( xflags[TEXT('d')] )
		Styles |= WS_DLGFRAME ;
	if ( xflags[TEXT('f')] )
		ExStyles |= WS_EX_DLGMODALFRAME;
	if ( xflags[TEXT('s')] )
		ExStyles |= WS_EX_STATICEDGE;
	if ( xflags[TEXT('w')] )
		ExStyles |= WS_EX_WINDOWEDGE;

	return { Styles, ExStyles };
}

/*!
 * \brief Get Information about a control.
 *
 * blah
 */
bool DcxControl::parseGlobalInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
	case L"hwnd"_hash:
	{
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), (DWORD)m_Hwnd);	// can't use %p as this gives a hex result.
		_ts_snprintf(szReturnValue, TEXT("%lu"), (DWORD)m_Hwnd);	// can't use %p as this gives a hex result.
		return true;
	}
	break;
	case L"visible"_hash:
	{
		szReturnValue = dcx_truefalse((IsWindowVisible(m_Hwnd) != FALSE));
		return true;
	}
	break;
	case L"enabled"_hash:
	{
		szReturnValue = dcx_truefalse((IsWindowEnabled(m_Hwnd) != FALSE));
		return true;
	}
	break;
	case L"pos"_hash:
	{
		const auto rc = getWindowPosition();
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
		_ts_snprintf(szReturnValue, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
		return true;
	}
	break;
	case L"dpos"_hash:
	{
#if DCX_USE_WRAPPERS
		const Dcx::dcxWindowRect rc(m_Hwnd, getParentDialog()->getHwnd());

		_ts_snprintf(szReturnValue, TEXT("%d %d %d %d"), rc.left, rc.top, rc.Width(), rc.Height());
#else
		if (RECT rc{}; GetWindowRect(m_Hwnd, &rc))
		{
			MapWindowRect(nullptr, getParentDialog()->getHwnd(), &rc);
		
			//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
			_ts_snprintf(szReturnValue, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
			return true;
		}
#endif
		return true;
	}
	break;
	case L"mark"_hash:
	{
		szReturnValue = m_tsMark.to_chr();
		return true;
	}
	break;
	case L"mouse"_hash:
	{
#if DCX_USE_WRAPPERS
		Dcx::dcxCursorPos pt(m_Hwnd);

		_ts_snprintf(szReturnValue, TEXT("%d %d"), pt.x, pt.y);

		return true;
#else
		if (POINT pt{}; GetCursorPos(&pt))
		{
			MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
			//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), pt.x, pt.y);
			_ts_snprintf(szReturnValue, TEXT("%d %d"), pt.x, pt.y);
			return true;
		}
#endif
	}
	break;
	case L"pid"_hash:
	{
#if DCX_USE_WRAPPERS
		auto hParent = GetParent(m_Hwnd);

		if (const Dcx::dcxClassName sClassname(hParent); sClassname == TEXT("#32770"))
			szReturnValue = TEXT('0');
		else
			_ts_snprintf(szReturnValue, TEXT("%u"), getParentDialog()->getControlByHWND(hParent)->getUserID());

		return true;
#else
		stString<257> sClassname;
		
		auto hParent = GetParent(m_Hwnd);
		GetClassName(hParent, sClassname, gsl::narrow_cast<int>(sClassname.size()));
		
		if (sClassname == TEXT("#32770"))
			szReturnValue = TEXT('0');
		else
			//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), getParentDialog()->getControlByHWND(hParent)->getUserID());
			_ts_snprintf(szReturnValue, TEXT("%u"), getParentDialog()->getControlByHWND(hParent)->getUserID());
		
		return true;
#endif
	}
	break;
	case L"type"_hash:
	{
		szReturnValue = getType().to_chr();
		return true;
	}
	break;
	case L"styles"_hash:
	{
		szReturnValue = getStyles().to_chr();
		return true;
	}
	break;
	case L"font"_hash:
	{
		auto hFontControl = getControlFont();

		if (hFontControl == nullptr)
			hFontControl = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

		if (hFontControl != nullptr)
		{
			if (LOGFONT lfCurrent{}; GetObject(hFontControl, sizeof(LOGFONT), &lfCurrent) != 0)
			{
				szReturnValue = ParseLogfontToCommand(&lfCurrent).to_chr();
				return true;
			}
		}
	}
	break;
	// [NAME] [ID] [PROP]
	case L"tooltipbgcolor"_hash:
	{
		COLORREF cref = CLR_INVALID;

		if (m_ToolTipHWND != nullptr)
			cref = gsl::narrow_cast<COLORREF>(SendMessage(m_ToolTipHWND, TTM_GETTIPBKCOLOR, NULL, NULL));

		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), cref);
		_ts_snprintf(szReturnValue, TEXT("%lu"), cref);
		return true;
	}
	break;
	// [NAME] [ID] [PROP]
	case L"tooltiptextcolor"_hash:
	{
		COLORREF cref = CLR_INVALID;

		if (m_ToolTipHWND != nullptr)
			cref = gsl::narrow_cast<COLORREF>(SendMessage(m_ToolTipHWND, TTM_GETTIPTEXTCOLOR, NULL, NULL));

		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), cref);
		_ts_snprintf(szReturnValue, TEXT("%lu"), cref);
		return true;
	}
	// [NAME] [ID] [PROP]
	case L"alpha"_hash:
	{
		szReturnValue = dcx_truefalse(m_bAlphaBlend);
		return true;
	}
	break;
	// [NAME] [ID] [PROP]
	case L"textcolor"_hash:
	{
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_clrText);
		_ts_snprintf(szReturnValue, TEXT("%u"), m_clrText);
		return true;
	}
	break;
	// [NAME] [ID] [PROP]
	case L"textbgcolor"_hash:
	{
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_clrBackText);
		_ts_snprintf(szReturnValue, TEXT("%u"), m_clrBackText);
		return true;
	}
	break;
	// [NAME] [ID] [PROP]
	case L"bgcolor"_hash:
	{
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_clrBackground);
		_ts_snprintf(szReturnValue, TEXT("%u"), m_clrBackground);
		return true;
	}
	default:
		throw Dcx::dcxException("Invalid property or number of arguments");
		break;
	}
	return false;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT CALLBACK DcxControl::WindowProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//auto pthis = static_cast<DcxControl*>(GetProp(mHwnd, TEXT("dcx_cthis")));

	DcxControl *pthis = nullptr;

	if (uMsg == WM_NCCREATE)
	{
		const CREATESTRUCT *cs = reinterpret_cast<CREATESTRUCT *>(lParam);

		// no params, fail...
		if ((cs == nullptr) || (cs->lpCreateParams == nullptr))
			return FALSE; // cause CreateWindowEx() to fail.

						  // get control object
		pthis = reinterpret_cast<DcxControl *>(cs->lpCreateParams);
		// set this control objects hwnd.
		pthis->m_Hwnd = mHwnd;

		// save control object to this hwnd
		if (!SetProp(mHwnd, TEXT("dcx_cthis"), pthis))
			return FALSE;	// SetProp() failed, cause CreateWindowEx() to fail.
	}
	else
		pthis = static_cast<DcxControl*>(GetProp(mHwnd, TEXT("dcx_cthis")));

	// sanity check, see that prop exists.
	if (pthis == nullptr)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	if (uMsg == WM_PAINT && pthis->getParentDialog()->IsVistaStyle())
	{
		ValidateRect(mHwnd, nullptr);

		if (RECT rcUpdate{}; GetWindowRect(mHwnd, &rcUpdate))
			pthis->getParentDialog()->UpdateVistaStyle(&rcUpdate);

		return 0L;
	}

	if (const auto fBlocked = (InSendMessageEx(nullptr) & (ISMEX_REPLIED | ISMEX_SEND)) == ISMEX_SEND; !fBlocked)
	{
		// If Message is blocking just call old win proc
		BOOL bParsed = FALSE;

		if ((uMsg != WM_DESTROY) && (uMsg != WM_NCDESTROY))
			pthis->incRef( );

		const auto lrRes = pthis->PostMessage(uMsg, wParam, lParam, bParsed);

		if ((uMsg != WM_DESTROY) && (uMsg != WM_NCDESTROY))
			pthis->decRef();

		if (bParsed)
			return lrRes;
	}

	return pthis->CallDefaultClassProc(uMsg, wParam, lParam);
}

/*!
 * \brief blah
 *
 * blah
 *
 * Input [NAME] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
 */

DcxControl * DcxControl::controlFactory(DcxDialog *const p_Dialog, const UINT mID, const TString & tsInput, const UINT offset, const UINT64 mask, HWND hParent)
{
	const auto type(tsInput.getfirsttok(gsl::narrow_cast<int>(offset)));

	RECT rc{};

	rc.left = tsInput.getnexttok().to_<LONG>();
	rc.top = tsInput.getnexttok().to_<LONG>();
	rc.right = rc.left + tsInput.getnexttok().to_<LONG>();
	rc.bottom = rc.top + tsInput.getnexttok().to_<LONG>();

	const auto styles(tsInput.getlasttoks());

	if (hParent == nullptr)
		hParent = p_Dialog->getHwnd();

//	const auto dct = DcxControl::TSTypeToControlType(type);
//
//	if ((dct == DcxControlTypes::PROGRESSBAR) && (dcx_testflag(mask, CTLF_ALLOW_PBAR)))
//		return new DcxProgressBar(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::TRACKBAR) && (dcx_testflag(mask, CTLF_ALLOW_TRACKBAR)))
//		return new DcxTrackBar(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::COMBOEX) && (dcx_testflag(mask, CTLF_ALLOW_COMBOEX)))
//		return new DcxComboEx(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::COLORCOMBO) && (dcx_testflag(mask, CTLF_ALLOW_COLORCOMBO)))
//		return new DcxColorCombo(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::STATUSBAR) && (dcx_testflag(mask, CTLF_ALLOW_STATUSBAR)))
//		return new DcxStatusBar(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::TOOLBAR) && (dcx_testflag(mask, CTLF_ALLOW_TOOLBAR)))
//		return new DcxToolBar(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::TREEVIEW) && (dcx_testflag(mask, CTLF_ALLOW_TREEVIEW)))
//		return new DcxTreeView(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::LISTVIEW) && (dcx_testflag(mask, CTLF_ALLOW_LISTVIEW)))
//		return new DcxListView(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::REBAR) && (dcx_testflag(mask, CTLF_ALLOW_REBAR)))
//		return new DcxReBar(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::BUTTON) && (dcx_testflag(mask, CTLF_ALLOW_BUTTON)))
//		return new DcxButton(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::RICHEDIT) && (dcx_testflag(mask, CTLF_ALLOW_RICHEDIT)))
//		return new DcxRichEdit(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::UPDOWN) && (dcx_testflag(mask, CTLF_ALLOW_UPDOWN)))
//		return new DcxUpDown(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::IPADDRESS) && (dcx_testflag(mask, CTLF_ALLOW_IPADDRESS)))
//		return new DcxIpAddress(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::WEBCTRL) && (dcx_testflag(mask, CTLF_ALLOW_WEBCTRL)))
//		return new DcxWebControl(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::CALENDAR) && (dcx_testflag(mask, CTLF_ALLOW_CALANDER)))
//		return new DcxCalendar(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::DATETIME) && (dcx_testflag(mask, CTLF_ALLOW_CALANDER)))
//		return new DcxDateTime(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::DIVIDER) && (dcx_testflag(mask, CTLF_ALLOW_DIVIDER)))
//		return new DcxDivider(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::PANEL) && (dcx_testflag(mask, CTLF_ALLOW_PANEL)))
//		return new DcxPanel(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::TABB) && (dcx_testflag(mask, CTLF_ALLOW_TAB)))
//		return new DcxTab(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::LINE) && (dcx_testflag(mask, CTLF_ALLOW_LINE)))
//		return new DcxLine(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::BOX) && (dcx_testflag(mask, CTLF_ALLOW_BOX)))
//		return new DcxBox(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::RADIO) && (dcx_testflag(mask, CTLF_ALLOW_RADIO)))
//		return new DcxRadio(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::CHECK) && (dcx_testflag(mask, CTLF_ALLOW_CHECK)))
//		return new DcxCheck(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::TEXT) && (dcx_testflag(mask, CTLF_ALLOW_TEXT)))
//		return new DcxText(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::EDIT) && (dcx_testflag(mask, CTLF_ALLOW_EDIT)))
//		return new DcxEdit(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::SCROLL) && (dcx_testflag(mask, CTLF_ALLOW_SCROLL)))
//		return new DcxScroll(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::LIST) && (dcx_testflag(mask, CTLF_ALLOW_LIST)))
//		return new DcxList(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::LINK) && (dcx_testflag(mask, CTLF_ALLOW_LINK)))
//		return new DcxLink(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::IMAGE) && (dcx_testflag(mask, CTLF_ALLOW_IMAGE)))
//		return new DcxImage(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::PAGER) && (dcx_testflag(mask, CTLF_ALLOW_PAGER)))
//		return new DcxPager(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::STACKER) && (dcx_testflag(mask, CTLF_ALLOW_STACKER)))
//		return new DcxStacker(mID, p_Dialog, hParent, &rc, styles);
//#ifdef DCX_USE_DXSDK
//
//	else if ((dct == DcxControlTypes::DIRECTSHOW) && (dcx_testflag(mask, CTLF_ALLOW_DIRECTSHOW)))
//		return new DcxDirectshow(mID, p_Dialog, hParent, &rc, styles);
//
//#endif // DCX_USE_DXSDK
//
//	else if ((dct == DcxControlTypes::WINDOW) && (dcx_testflag(mask, CTLF_ALLOW_DOCK))) {
//		if (styles.empty())
//			throw Dcx::dcxException("No window name");
//
//		const auto tsWin(styles.getfirsttok(1));
//
//		// this helps stop '@' being passed as $window(@).hwnd == $window(-2).hwnd & usually causes a crash.
//		if (tsWin.len() < 2)
//			throw Dcx::dcxException(TEXT("No such window: %s"), tsWin);
//
//		auto winHwnd = (HWND)tsWin.to_num();
//		if (!IsWindow(winHwnd)) {
//			TCHAR windowHwnd[30];
//
//			mIRCLinker::evalex(windowHwnd, 30, TEXT("$window(%s).hwnd"), tsWin.to_chr());
//
//			winHwnd = (HWND)dcx_atoi(windowHwnd);
//		}
//
//		if (!IsWindow(winHwnd))
//			throw Dcx::dcxException(TEXT("No such window: %s"), tsWin);
//
//		if (p_Dialog->getControlByHWND(winHwnd) != nullptr)
//			throw Dcx::dcxException("Window already a DCX Control");
//
//		return new DcxMWindow(winHwnd, hParent, mID, p_Dialog, &rc, styles);
//	}
//	else if ((dct == DcxControlTypes::DIALOG) && (dcx_testflag(mask, CTLF_ALLOW_DOCK))) {
//		if (styles.empty())
//			throw Dcx::dcxException("No dialog name");
//
//		const auto tsDname(styles.getfirsttok(1));
//		auto winHwnd = GetHwndFromString(tsDname);
//
//		if (IsWindow(winHwnd))
//			throw Dcx::dcxException(TEXT("No such dialog: %"), tsDname);
//
//		if (p_Dialog->getControlByHWND(winHwnd) != nullptr)
//			Dcx::dcxException(TEXT("Control already exists : %"), tsDname);
//
//		auto newDialog = new DcxMDialog(winHwnd, hParent, mID, p_Dialog, &rc, styles);
//		auto dlg = Dcx::Dialogs.getDialogByHandle(winHwnd);
//
//		// if its a dcx marked dialog, mark the parent name
//		if (dlg != nullptr)
//			dlg->setParentName(p_Dialog->getName());
//
//		return newDialog;
//	}
	
	switch (DcxControl::TSTypeToControlType(type))
	{
	case DcxControlTypes::PROGRESSBAR:
		if (dcx_testflag(mask, CTLF_ALLOW_PBAR))
			return new DcxProgressBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TRACKBAR:
		if (dcx_testflag(mask, CTLF_ALLOW_TRACKBAR))
			return new DcxTrackBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::COMBOEX:
		if (dcx_testflag(mask, CTLF_ALLOW_COMBOEX))
			return new DcxComboEx(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::COLORCOMBO:
		if (dcx_testflag(mask, CTLF_ALLOW_COLORCOMBO))
			return new DcxColorCombo(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::STATUSBAR:
		if (dcx_testflag(mask, CTLF_ALLOW_STATUSBAR))
			return new DcxStatusBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TOOLBAR:
		if (dcx_testflag(mask, CTLF_ALLOW_TOOLBAR))
			return new DcxToolBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TREEVIEW:
		if (dcx_testflag(mask, CTLF_ALLOW_TREEVIEW))
			return new DcxTreeView(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::LISTVIEW:
		if (dcx_testflag(mask, CTLF_ALLOW_LISTVIEW))
			return new DcxListView(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::REBAR:
		if (dcx_testflag(mask, CTLF_ALLOW_REBAR))
			return new DcxReBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::BUTTON:
		if (dcx_testflag(mask, CTLF_ALLOW_BUTTON))
			return new DcxButton(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::RICHEDIT:
		if (dcx_testflag(mask, CTLF_ALLOW_RICHEDIT))
			return new DcxRichEdit(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::UPDOWN:
		if (dcx_testflag(mask, CTLF_ALLOW_UPDOWN))
			return new DcxUpDown(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::IPADDRESS:
		if (dcx_testflag(mask, CTLF_ALLOW_IPADDRESS))
			return new DcxIpAddress(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::WEBCTRL:
		if (dcx_testflag(mask, CTLF_ALLOW_WEBCTRL))
			return new DcxWebControl(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::CALENDAR:
		if (dcx_testflag(mask, CTLF_ALLOW_CALANDER))
			return new DcxCalendar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::DATETIME:
		if (dcx_testflag(mask, CTLF_ALLOW_CALANDER))
			return new DcxDateTime(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::DIVIDER:
		if (dcx_testflag(mask, CTLF_ALLOW_DIVIDER))
			return new DcxDivider(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::PANEL:
		if (dcx_testflag(mask, CTLF_ALLOW_PANEL))
			return new DcxPanel(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TABB:
		if (dcx_testflag(mask, CTLF_ALLOW_TAB))
			return new DcxTab(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::LINE:
		if (dcx_testflag(mask, CTLF_ALLOW_LINE))
			return new DcxLine(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::BOX:
		if (dcx_testflag(mask, CTLF_ALLOW_BOX))
			return new DcxBox(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::RADIO:
		if (dcx_testflag(mask, CTLF_ALLOW_RADIO))
			return new DcxRadio(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::CHECK:
		if (dcx_testflag(mask, CTLF_ALLOW_CHECK))
			return new DcxCheck(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TEXT:
		if (dcx_testflag(mask, CTLF_ALLOW_TEXT))
			return new DcxText(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::EDIT:
		if (dcx_testflag(mask, CTLF_ALLOW_EDIT))
			return new DcxEdit(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::SCROLL:
		if (dcx_testflag(mask, CTLF_ALLOW_SCROLL))
			return new DcxScroll(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::LIST:
		if (dcx_testflag(mask, CTLF_ALLOW_LIST))
			return new DcxList(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::LINK:
		if (dcx_testflag(mask, CTLF_ALLOW_LINK))
			return new DcxLink(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::IMAGE:
		if (dcx_testflag(mask, CTLF_ALLOW_IMAGE))
			return new DcxImage(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::PAGER:
		if (dcx_testflag(mask, CTLF_ALLOW_PAGER))
			return new DcxPager(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::STACKER:
		if (dcx_testflag(mask, CTLF_ALLOW_STACKER))
			return new DcxStacker(mID, p_Dialog, hParent, &rc, styles);
		break;
#ifdef DCX_USE_DXSDK

	case DcxControlTypes::DIRECTSHOW:
		if (dcx_testflag(mask, CTLF_ALLOW_DIRECTSHOW))
			return new DcxDirectshow(mID, p_Dialog, hParent, &rc, styles);
		break;

#endif // DCX_USE_DXSDK

	case DcxControlTypes::WINDOW:
		if (dcx_testflag(mask, CTLF_ALLOW_DOCK))
		{
			if (styles.empty())
				throw Dcx::dcxException("No window name");

			const auto tsWin(styles.getfirsttok(1));

			// this helps stop '@' being passed as $window(@).hwnd == $window(-2).hwnd & usually causes a crash.
			if (tsWin.len() < 2)
				throw Dcx::dcxException(TEXT("No such window: %"), tsWin);

			auto winHwnd = (HWND)tsWin.to_<DWORD>();
			if (!IsWindow(winHwnd))
			{
				//stString<30> windowHwnd;
				//
				//mIRCLinker::evalex(windowHwnd, static_cast<const int>(windowHwnd.size()), TEXT("$window(%s).hwnd"), tsWin.to_chr());
				//
				//winHwnd = (HWND)dcx_atoi(windowHwnd.data());

				stString<30> windowHwnd;

				mIRCLinker::eval(windowHwnd, TEXT("$window(%).hwnd"), tsWin);

				winHwnd = (HWND)dcx_atoi(windowHwnd.data());
			}

			if (!IsWindow(winHwnd))
				throw Dcx::dcxException(TEXT("No such window: %"), tsWin);

			if (p_Dialog->getControlByHWND(winHwnd) != nullptr)
				throw Dcx::dcxException("Window already a DCX Control");

			return new DcxMWindow(winHwnd, hParent, mID, p_Dialog, &rc, styles);
		}
		break;
	case DcxControlTypes::DIALOG:
		if (dcx_testflag(mask, CTLF_ALLOW_DOCK))
		{
			if (styles.empty())
				throw Dcx::dcxException("No dialog name");

			const auto tsDname(styles.getfirsttok(1));
			const auto winHwnd = GetHwndFromString(tsDname);

			if (!IsWindow(winHwnd))
				throw Dcx::dcxException(TEXT("No such dialog: %"), tsDname);

			if (p_Dialog->getControlByHWND(winHwnd) != nullptr)
				throw Dcx::dcxException(TEXT("Control already exists : %"), tsDname);

			//auto newDialog = new DcxMDialog(winHwnd, hParent, mID, p_Dialog, &rc, styles);
			auto newDialog = std::make_unique<DcxMDialog>(winHwnd, hParent, mID, p_Dialog, &rc, styles);

			// if its a dcx marked dialog, mark the parent name
			if (auto dlg = Dcx::Dialogs.getDialogByHandle(winHwnd); dlg != nullptr)
				dlg->setParentName(p_Dialog->getName());

			return newDialog.release();
		}
		break;
	case DcxControlTypes::UNKNOWN:
		throw Dcx::dcxException(TEXT("Unknown Control Type: %"), type);
		break;
	}
	throw Dcx::dcxException(TEXT("Control Type not supported: %"), type);
	return nullptr;	// never gets executed, but compilers complain about its absense...
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxControl::setFont( const HFONT hFont, const BOOL fRedraw ) noexcept
{
  return SendMessage( m_Hwnd, WM_SETFONT, (WPARAM) hFont, MAKELPARAM(fRedraw, 0) );
}

/*!
 * \brief blah
 *
 * blah
 */

HFONT DcxControl::getFont( ) const noexcept
{
  return GetWindowFont( m_Hwnd );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxControl::setRedraw( const BOOL fView ) noexcept
{
  return SendMessage( m_Hwnd, WM_SETREDRAW, gsl::narrow_cast<WPARAM>(fView), (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::setControlFont( const HFONT hFont, const BOOL fRedraw ) noexcept
{
	if (auto hControlFont = this->getFont(); hControlFont != GetStockObject(DEFAULT_GUI_FONT))
	{
		if (hControlFont != nullptr)
		{
			DeleteFont(hControlFont);
			this->m_hFont = nullptr;
		}
		else if (this->m_hFont != nullptr)
		{
			DeleteFont(this->m_hFont);
			this->m_hFont = nullptr;
		}
	}

	this->setFont(hFont, fRedraw);
	this->m_hFont = hFont;
}

/*!
 * \brief blah
 *
 * blah
 */

 /*!
 * \brief blah
 *
 * blah
 */

 /*!
 * \brief blah
 *
 * blah
 */

const RECT DcxControl::getWindowPosition(void) const noexcept
{
#if DCX_USE_WRAPPERS
	const Dcx::dcxWindowRect rc(m_Hwnd, GetParent(m_Hwnd));
	return rc.CopyRect();
	//return{ rc.left, rc.top, rc.right, rc.bottom };
	//return rc;
#else
	RECT rc;
	SetRectEmpty(&rc);

	GetWindowRectParent(m_Hwnd, &rc);
	return rc;
#endif
}

void DcxControl::updateParentCtrl(void) noexcept
{
	// find the host control, if any.
	this->m_pParentHWND = GetParent(m_Hwnd);
}

void DcxControl::DrawCtrlBackground(const HDC hdc, const DcxControl *const p_this, const RECT *const rwnd, HTHEME hTheme, const int iPartId, const int iStateId) noexcept
{
	if ((hdc == nullptr) || (p_this == nullptr))
		return;

	// fill background.
	if (!p_this->isExStyle(WindowExStyle::Transparent))
	{
		RECT rc{};
		if (rwnd == nullptr)
		{
			if (!GetClientRect(p_this->getHwnd(), &rc))
				return;
		}
		else
			CopyRect(&rc, rwnd);

		if (!IsWindowEnabled(p_this->m_Hwnd))
		{// use disabled colouring when windows disabled.
			if (hTheme != nullptr && p_this->IsThemed() && Dcx::UXModule.dcxIsThemeActive())
			{
				if (Dcx::UXModule.dcxIsThemeBackgroundPartiallyTransparent(hTheme, iPartId, iStateId))
					Dcx::UXModule.dcxDrawThemeParentBackground(p_this->m_Hwnd, hdc, &rc);
				Dcx::UXModule.dcxDrawThemeBackground(hTheme, hdc, iPartId, iStateId, &rc, nullptr);
			}
			else
				FillRect( hdc, &rc, GetSysColorBrush(COLOR_3DFACE) );
		}
		else if (p_this->m_bGradientFill)
		{
			const auto clrStart = (p_this->getStartGradientColor() != CLR_INVALID) ? p_this->getStartGradientColor() : GetSysColor(COLOR_3DFACE);
			const auto clrEnd = (p_this->getEndGradientColor() != CLR_INVALID) ? p_this->getEndGradientColor() : GetSysColor(COLOR_GRADIENTACTIVECAPTION);

			XPopupMenuItem::DrawGradient( hdc, &rc, clrStart, clrEnd, p_this->m_bGradientVertical);
		}
		else {
			auto hBrush = p_this->getBackClrBrush();
			if (hBrush == nullptr)
			{
				if (hTheme != nullptr && p_this->IsThemed() && Dcx::UXModule.dcxIsThemeActive())
				{
					if (Dcx::UXModule.dcxIsThemeBackgroundPartiallyTransparent(hTheme, iPartId, iStateId))
						Dcx::UXModule.dcxDrawThemeParentBackground(p_this->m_Hwnd, hdc, &rc);
					Dcx::UXModule.dcxDrawThemeBackground(hTheme, hdc, iPartId, iStateId, &rc, nullptr);
					return;
				}
				else
					hBrush = GetSysColorBrush(COLOR_3DFACE);
			}
			if (hBrush != nullptr)
				FillRect( hdc, &rc, hBrush );
		}
	}
}

void DcxControl::DrawControl(HDC hDC, HWND hwnd)
{
#if DCX_USE_WRAPPERS
	// if window matches this one, don't draw (loop condition)
	if (hwnd == m_Hwnd)
		return;

	// if window isn't visible, don't draw.
	if (!IsWindowVisible(hwnd))
		return;

	// if window is within a background paint of it's own, don't draw. (loop condition)
	if (const auto *const p_ctrl = this->getParentDialog()->getControlByHWND(hwnd); (p_ctrl != nullptr && p_ctrl->m_bInPrint))
		return;

	const Dcx::dcxWindowRect rc(hwnd, GetParent(hwnd));

	// if window isn't within the client area of the control who's background we are drawing, don't draw.
	if (!RectVisible(hDC, &rc))
		return;

	const auto w = rc.Width(), h = rc.Height();

	const Dcx::dcxBitmapResource hBitmap(hDC, w, h);

	const Dcx::dcxHDCBitmapResource hdcMemory(hDC, hBitmap.get());

	::SendMessage(hwnd, WM_ERASEBKGND, (WPARAM)(HDC)hdcMemory.get(), 1L); // HACK: using 1L instead of NULL as a workaround for stacker.
	::SendMessage(hwnd, WM_PRINT, (WPARAM)(HDC)hdcMemory.get(), (LPARAM)PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE /*| PRF_ERASEBKGND*/);

	BitBlt(hDC, rc.left, rc.top, w, h, hdcMemory.get(), 0, 0, SRCCOPY);
#else
	// if window matches this one, don't draw (loop condition)
	if (hwnd == m_Hwnd)
		return;
	
	// if window isnTEXT('t visible, don')t draw.
	if (!IsWindowVisible(hwnd))
		return;
	
	// if window is within a background paint of it's own, don't draw. (loop condition)
	if (const auto *const p_ctrl = this->getParentDialog()->getControlByHWND(hwnd); (p_ctrl != nullptr && p_ctrl->m_bInPrint))
		return;
	
	//RECT rc;
	//if (!GetWindowRect(hwnd, &rc))
	//	return;
	//
	//MapWindowRect(nullptr, GetParent(hwnd), &rc);

	RECT rc{};
	if (!GetWindowRectParent(hwnd, &rc))
		return;

	// if window isn't within the client area of the control who's background we are drawing, don't draw.
	if (!RectVisible(hDC, &rc))
		return;
	
	auto hdcMemory = ::CreateCompatibleDC(hDC);
	
	if (hdcMemory == nullptr)
		return;
	
	Auto(DeleteDC(hdcMemory));
	
	auto w = (rc.right - rc.left), h = (rc.bottom - rc.top);
	auto hBitmap = ::CreateCompatibleBitmap(hDC, w, h);
	
	if (hBitmap == nullptr)
		return;
	
	Auto(DeleteBitmap(hBitmap));
	
	const auto hbmpOld = SelectBitmap(hdcMemory, hBitmap);
	
	Auto(SelectBitmap(hdcMemory, hbmpOld));
	
	::SendMessage(hwnd, WM_ERASEBKGND, (WPARAM)hdcMemory, 1L); // HACK: using 1L instead of NULL as a workaround for stacker.
	::SendMessage( hwnd, WM_PRINT, (WPARAM)hdcMemory, (LPARAM)PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE /*| PRF_ERASEBKGND*/);
	
	BitBlt( hDC, rc.left, rc.top, w, h, hdcMemory, 0, 0, SRCCOPY);
#endif

}

void DcxControl::DrawParentsBackground(const HDC hdc, const RECT *const rcBounds, const HWND dHwnd)
{
	// fill in parent bg
	RECT rcClient{};
	auto hwnd = m_Hwnd;

	if (dHwnd != nullptr)
		hwnd = dHwnd;

	if (rcBounds == nullptr)
	{
		if (!GetClientRect(hwnd, &rcClient)) // get controls client area
			return;
	}
	else
		rcClient = *rcBounds;

	// if themes are active use them.
//	if (dcxIsThemeActive() && !this->m_bNoTheme) {
//#ifdef DCX_USE_WINSDK
//		if (DrawThemeParentBackgroundExUx != nullptr)
//			DrawThemeParentBackgroundExUx(hwnd, hdc, 0, &rcClient); // Vista only, does basicly the same as below.
//		else
//#endif
//			DrawThemeParentBackgroundUx(hwnd, hdc, &rcClient); // XP+
//		return;
//	}

	/*
		The following code draws the parents background & client area,
		followed by all child controls covered by this one.
	*/
	updateParentCtrl(); // find the host control, if any.

	//If in Vista mode
	if (getParentDialog()->IsVistaStyle())
	{
		// Check if the hdc to render too is the main hdc, if so bkg is already drawn so just return
		if (hdc == getParentDialog()->GetVistaHDC())
			return;
		// Check if parent is dialog.
		if (m_pParentHWND == getParentDialog()->getHwnd())
		{
			// When in vista mode dialog has already been drawn
			// So just grab image from windows DC.
			auto hdcParent = getParentDialog()->GetVistaHDC();
			auto rcWin = rcClient;
			getParentDialog()->MapVistaRect(hwnd, &rcWin);
			BitBlt( hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
				hdcParent, rcWin.left, rcWin.top, SRCCOPY);
			return;
		}
	}
	if (getParentDialog()->isExStyle(WindowExStyle::Composited))
	{
		// When in composited mode underling controls have already been drawn
		// So just grab image from windows DC.

		if (auto hdcParent = GetDC(m_pParentHWND); hdcParent != nullptr)
		{
			Auto(ReleaseDC(m_pParentHWND, hdcParent));
		
			auto rcWin = rcClient;
			MapWindowRect(hwnd, m_pParentHWND, &rcWin);
		
			BitBlt(hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
				hdcParent, rcWin.left, rcWin.top, SRCCOPY);
		}
		return;
	}

	//// make a new HDC for background rendering
	//HDC hdcbkg = CreateCompatibleDC( hdc );
	//if (hdcbkg != nullptr) {
	//	//RECT rcParentWin, rcParentClient, rcWin;
	//	RECT rcParentWin, rcWin;
	//	// get parent windows client area.
	//	GetClientRect(this->m_pParentHWND,&rcParentWin);
	//	// make a bitmap for rendering to.
	//	HBITMAP memBM = CreateCompatibleBitmap ( hdc, rcParentWin.right - rcParentWin.left, rcParentWin.bottom - rcParentWin.top );
	//	if (memBM != nullptr) {
	//		// get this controls x & y pos within its parent.
	//		rcWin = rcClient;
	//		MapWindowPoints(hwnd,this->m_pParentHWND, (LPPOINT)&rcWin, 2);
	//		// associate bitmap with HDC
	//		HBITMAP oldBM = (HBITMAP)SelectObject ( hdcbkg, memBM );
	//		HRGN clipRgn = CreateRectRgnIndirect(&rcWin); // clip parents drawing to this controls rect.
	//		if (clipRgn != nullptr) {
	//			SelectClipRgn(hdcbkg, clipRgn);
	//			DeleteObject(clipRgn);
	//		}
	//		// Sending WM_ERASEBKGND followed by WM_PRINTCLIENT emulates the method used by DrawThemeParentBackgroundEx() on vista.
	//		this->m_bInPrint = true; // this helps prevent long drawing loops
	//		// fill in the parents image
	//		::SendMessage(this->m_pParentHWND, WM_ERASEBKGND, (WPARAM)hdcbkg,1L); // HACK: using 1L instead of nullptr as a workaround for stacker.
	//		::SendMessage(this->m_pParentHWND, WM_PRINTCLIENT, (WPARAM)hdcbkg,PRF_CLIENT);
	//		// now draw all child controls within area of this control.
	//		// NB: AVOID EnumChildWindows()
	//		HWND child = GetWindow(m_Hwnd, GW_HWNDPREV);
	//		while (child != nullptr) {
	//			this->DrawControl(hdcbkg, child);
	//			child = GetWindow(child, GW_HWNDPREV);
	//		}
	//		this->m_bInPrint = false;
	//		// draw background to main hdc
	//		BitBlt( hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
	//			hdcbkg, rcWin.left, rcWin.top, SRCCOPY);
	//		DeleteObject(SelectObject( hdcbkg, oldBM ));
	//	}
	//	DeleteDC( hdcbkg );
	//}
	// make a new HDC for background rendering

	RECT rcParentWin{}, rcWin{};
	if (!GetClientRect(m_pParentHWND, &rcParentWin))
		return;

#if DCX_USE_WRAPPERS
	const Dcx::dcxHDCBuffer hdcbkg(hdc, rcParentWin);

	// get this controls x & y pos within its parent.
	rcWin = rcClient;
	MapWindowRect(hwnd, m_pParentHWND, &rcWin);
	
	if (auto clipRgn = CreateRectRgnIndirect(&rcWin); /* clip parents drawing to this controls rect. */ clipRgn != nullptr)
	{
		SelectClipRgn(hdcbkg.get(), clipRgn);
		DeleteRgn(clipRgn);
	}

	// Sending WM_ERASEBKGND followed by WM_PRINTCLIENT emulates the method used by DrawThemeParentBackgroundEx() on vista.
	m_bInPrint = true; // this helps prevent long drawing loops
	// fill in the parents image
	::SendMessage(m_pParentHWND, WM_ERASEBKGND, (WPARAM)(HDC)hdcbkg.get(), 1L); // HACK: using 1L instead of NULL as a workaround for stacker.
	::SendMessage(m_pParentHWND, WM_PRINTCLIENT, (WPARAM)(HDC)hdcbkg.get(), PRF_CLIENT);

	for (auto hChild = GetWindow(m_Hwnd, GW_HWNDPREV); hChild != nullptr; hChild = GetWindow(hChild, GW_HWNDPREV))
		DrawControl(hdcbkg.get(), hChild);

	m_bInPrint = false;
	// draw background to main hdc
	BitBlt(hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top), hdcbkg.get(), rcWin.left, rcWin.top, SRCCOPY);
#else
	auto hdcbkg = CreateHDCBuffer(hdc, &rcParentWin);
	if (hdcbkg == nullptr)
		return;
	Auto(DeleteHDCBuffer(hdcbkg));

	// get this controls x & y pos within its parent.
	rcWin = rcClient;
	MapWindowRect(hwnd, this->m_pParentHWND, &rcWin);

	if (auto clipRgn = CreateRectRgnIndirect(&rcWin); /* clip parents drawing to this controls rect. */ clipRgn != nullptr)
	{
		SelectClipRgn(*hdcbkg, clipRgn);
		DeleteRgn(clipRgn);
	}

	// Sending WM_ERASEBKGND followed by WM_PRINTCLIENT emulates the method used by DrawThemeParentBackgroundEx() on vista.
	this->m_bInPrint = true; // this helps prevent long drawing loops
							 // fill in the parents image
	::SendMessage(this->m_pParentHWND, WM_ERASEBKGND, (WPARAM)*hdcbkg, 1L); // HACK: using 1L instead of NULL as a workaround for stacker.
	::SendMessage(this->m_pParentHWND, WM_PRINTCLIENT, (WPARAM)*hdcbkg, PRF_CLIENT);

	//::SendMessage(this->m_pParentHWND, WM_PRINT, (WPARAM)*hdcbkg,PRF_CLIENT|PRF_ERASEBKGND);
	// now draw all child controls within area of this control.
	// NB: AVOID EnumChildWindows()
	//HWND child = GetWindow(m_Hwnd, GW_HWNDPREV);
	//while (child != nullptr) {
	//	this->DrawControl(*hdcbkg, child);
	//	child = GetWindow(child, GW_HWNDPREV);
	//}

	for (auto hChild = GetWindow(m_Hwnd, GW_HWNDPREV); hChild != nullptr; hChild = GetWindow(hChild, GW_HWNDPREV))
		this->DrawControl(*hdcbkg, hChild);

	this->m_bInPrint = false;
	// draw background to main hdc
	BitBlt(hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
		*hdcbkg, rcWin.left, rcWin.top, SRCCOPY);
#endif
}

LPALPHAINFO DcxControl::SetupAlphaBlend(HDC *hdc, const bool DoubleBuffer)
{
	if ((hdc == nullptr) || (*hdc == nullptr) || (!this->IsAlphaBlend() && !DoubleBuffer))
		return nullptr;

	//auto ai = new ALPHAINFO(m_Hwnd);
	auto ai = std::make_unique<ALPHAINFO>(m_Hwnd);

	//ZeroMemory(ai.get(),sizeof(ALPHAINFO));
	/*
		1: draw parents bg to temp hdc
		2: copy bg to temp hdcbkg for later alpha
		3: draw button to temp hdc, over parents bg
		4: alpha blend temp hdc to hdc
	*/
	//// get windows client rect
	//if (!GetClientRect(m_Hwnd, &ai->ai_rcClient))
	//{
	//	//delete ai;
	//	return nullptr;
	//}
	//// get window rect
	//if (!GetWindowRect(m_Hwnd, &ai->ai_rcWin))
	//{
	//	//delete ai;
	//	return nullptr;
	//}

	if (Dcx::UXModule.IsBufferedPaintSupported())
	{
		BP_PAINTPARAMS paintParams{ sizeof(BP_PAINTPARAMS),BPPF_ERASE, nullptr, nullptr };
		ai->ai_bf.AlphaFormat = AC_SRC_OVER;
		ai->ai_bf.SourceConstantAlpha = this->m_iAlphaLevel; // 0x7f half of 0xff = 50% transparency
		if (this->IsAlphaBlend())
			paintParams.pBlendFunction = &ai->ai_bf;

		ai->ai_Buffer = Dcx::UXModule.dcxBeginBufferedPaint(*hdc, &ai->ai_rcClient, BPBF_COMPATIBLEBITMAP, &paintParams, &ai->ai_hdc);
		if (ai->ai_Buffer != nullptr)
		{
			this->DrawParentsBackground(ai->ai_hdc, &ai->ai_rcClient);
			BitBlt(*hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
			*hdc = ai->ai_hdc;
			return ai.release();
		}
	}
	// if vista method failed, fall through to our own method.

	//{
	//	// create a new HDC for alpha blending.
	//	ai->ai_hdc = CreateCompatibleDC( *hdc );
	//	ai->ai_bkg = nullptr;
	//	if (ai->ai_hdc != nullptr) {
	//		//GetClientRect(m_Hwnd,&ai->ai_rcWin);
	//		GetWindowRect(m_Hwnd,&ai->ai_rcWin);
	//		// create a bitmap to render to
	//		ai->ai_bitmap = CreateCompatibleBitmap ( *hdc, ai->ai_rcWin.right - ai->ai_rcWin.left, ai->ai_rcWin.bottom - ai->ai_rcWin.top );
	//		if (ai->ai_bitmap != nullptr) {
	//			GetClientRect(m_Hwnd,&ai->ai_rcClient);
	//			// associate bitmap with hdc
	//			ai->ai_oldBM = (HBITMAP)SelectObject ( ai->ai_hdc, ai->ai_bitmap );
	//			// fill in parent bg
	//			this->DrawParentsBackground(ai->ai_hdc, &ai->ai_rcClient);
	//			// If alpha blending, make a background bitmap & fill it.
	//			if (this->IsAlphaBlend()) {
	//				// avoid doing the whole background rendering again by simply copying the one we just did.
	//				HDC hdcbkg = CreateCompatibleDC( *hdc );
	//				if (hdcbkg != nullptr) {
	//					ai->ai_bkg = CreateCompatibleBitmap ( *hdc, ai->ai_rcWin.right - ai->ai_rcWin.left, ai->ai_rcWin.bottom - ai->ai_rcWin.top );
	//					if (ai->ai_bkg != nullptr) {
	//						HBITMAP oldBM = (HBITMAP)SelectObject ( hdcbkg, ai->ai_bkg );
	//
	//						BitBlt( hdcbkg, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
	//
	//						SelectObject(hdcbkg, oldBM);
	//					}
	//					DeleteDC(hdcbkg);
	//				}
	//			}
	//			ai->ai_Oldhdc = *hdc;
	//			*hdc = ai->ai_hdc;
	//		}
	//	}
	//}
	// create a new HDC Buffer for alpha blending.

	ai->ai_hdcBuffer = CreateHDCBuffer(*hdc, &ai->ai_rcWin);
	ai->ai_bkg = nullptr;
	if (ai->ai_hdcBuffer != nullptr)
	{
		// assign hdc for easy refrence & compat with previous code.
		ai->ai_hdc = *ai->ai_hdcBuffer;
		// fill in parent bg
		DrawParentsBackground(ai->ai_hdc, &ai->ai_rcClient);
		// If alpha blending, make a background bitmap & fill it.
		if (IsAlphaBlend())
		{
			// avoid doing the whole background rendering again by simply copying the one we just did.
			if (auto hdcbkg = CreateCompatibleDC(*hdc); hdcbkg != nullptr)
			{
				Auto(DeleteDC(hdcbkg));

				ai->ai_bkg = CreateCompatibleBitmap(*hdc, ai->ai_rcWin.right - ai->ai_rcWin.left, ai->ai_rcWin.bottom - ai->ai_rcWin.top);
				if (ai->ai_bkg != nullptr)
				{
					auto oldBM = SelectBitmap(hdcbkg, ai->ai_bkg);
					Auto(SelectBitmap(hdcbkg, oldBM));

					BitBlt(hdcbkg, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
				}
			}

			//Dcx::dcxHDCBitmap2Resource hdcbkg(*hdc, (ai->ai_rcWin.right - ai->ai_rcWin.left),(ai->ai_rcWin.bottom - ai->ai_rcWin.top));
			//BitBlt(hdcbkg, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
		}
		ai->ai_Oldhdc = *hdc;
		*hdc = ai->ai_hdc;
	}
	return ai.release();
}

void DcxControl::FinishAlphaBlend(LPALPHAINFO ai) noexcept
{
	if (ai == nullptr)
		return;
	Auto(delete ai);

	if (ai->ai_Buffer != nullptr)
	{
		Dcx::UXModule.dcxEndBufferedPaint(ai->ai_Buffer, TRUE);
		return;
	}
	//// if we can't do Vista method, try do our own
	//if (ai->ai_hdc != nullptr) {
	//	if (ai->ai_bitmap != nullptr) {
	//		int w = (ai->ai_rcClient.right - ai->ai_rcClient.left), h = (ai->ai_rcClient.bottom - ai->ai_rcClient.top);
	//		if (this->IsAlphaBlend()) {
	//			if (ai->ai_bkg != nullptr) {
	//				// create a new HDC for alpha blending. (doing things this way avoids any flicker)
	//				HDC hdcbkg = CreateCompatibleDC( ai->ai_Oldhdc);
	//				if (hdcbkg != nullptr) {
	//					// associate bitmap with hdc
	//					HBITMAP oldBM = (HBITMAP)SelectObject ( hdcbkg, ai->ai_bkg );
	//					// alpha blend finished button with parents background
	//					BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)this->m_iAlphaLevel, 0 }; // 0x7f half of 0xff = 50% transparency
	//					AlphaBlend(hdcbkg,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, w, h,bf);
	//					// draw final image to windows hdc.
	//					BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,hdcbkg,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
	//
	//					SelectObject( hdcbkg, oldBM);
	//					DeleteDC(hdcbkg);
	//				}
	//				DeleteBitmap(ai->ai_bkg);
	//			}
	//		}
	//		else
	//			BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
	//
	//		SelectObject (ai->ai_hdc, ai->ai_oldBM );
	//		DeleteObject(ai->ai_bitmap);
	//	}
	//	DeleteDC( ai->ai_hdc );
	//}
	//delete ai;

	// if we can't do Vista method, try do our own
	if (ai->ai_hdcBuffer != nullptr)
	{
		Auto(DeleteHDCBuffer(ai->ai_hdcBuffer));

		if (const auto w = (ai->ai_rcClient.right - ai->ai_rcClient.left), h = (ai->ai_rcClient.bottom - ai->ai_rcClient.top); this->IsAlphaBlend())
		{
			if (ai->ai_bkg != nullptr)
			{
				Auto(DeleteBitmap(ai->ai_bkg));

				// create a new HDC for alpha blending. (doing things this way avoids any flicker)
				if (const auto hdcbkg = CreateCompatibleDC(ai->ai_Oldhdc); hdcbkg != nullptr)
				{
					Auto(DeleteDC(hdcbkg));

					// associate bitmap with hdc
					const auto oldBM = SelectBitmap(hdcbkg, ai->ai_bkg);
					Auto(SelectBitmap(hdcbkg, oldBM));

					// alpha blend finished button with parents background
					ai->ai_bf.AlphaFormat = AC_SRC_OVER;
					ai->ai_bf.SourceConstantAlpha = this->m_iAlphaLevel; // 0x7f half of 0xff = 50% transparency
					AlphaBlend(hdcbkg,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, w, h, ai->ai_bf);
					// draw final image to windows hdc.
					BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,hdcbkg,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
				}
			}
		}
		else
			BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
	}
}

void DcxControl::showError(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const err) const
{
	if (getParentDialog() == nullptr)
		return;

	if (getParentDialog()->IsVerbose())
	{
		TString res;
		if (prop != nullptr)
			_ts_sprintf(res, TEXT("D_IERROR %(%, %).%: %"), getType(), getParentDialog()->getName(), getUserID(), prop, err);
		else
			_ts_sprintf(res, TEXT("D_CERROR (%) xdid % % %: %"), getType(), cmd, getParentDialog()->getName(), getUserID(), err);
		Dcx::error(cmd, res.to_chr());
	}

	if (!getParentDialog()->getAliasName().empty())
		const_cast<DcxControl *>(this)->execAliasEx(TEXT("error,%u,%s,%s,%s,%s"), getUserID(), getType().to_chr(), (prop != nullptr ? prop : TEXT("none")), (cmd != nullptr ? cmd : TEXT("none")), err);
}

LRESULT DcxControl::CommonMessage( const UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
{
	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_HELP:
	{
		if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_HELP))
			execAliasEx(TEXT("help,%u"), getUserID());
		bParsed = TRUE;
		lRes = TRUE;
	}
	break;

	case WM_SETCURSOR:
	{
		if ((LOWORD(lParam) == HTCLIENT) && ((HWND)wParam == m_Hwnd) && (m_hCursor != nullptr))
		{
			if (GetCursor() != m_hCursor)
				SetCursor(m_hCursor);
			bParsed = TRUE;
			return TRUE;
		}
	}
	break;

	case WM_MOUSEMOVE:
	{
		getParentDialog()->setMouseControl(getUserID());
	}
	break;

	case WM_SETFOCUS:
	{
		getParentDialog()->setFocusControl(getUserID());
	}
	break;

	case WM_CTLCOLORDLG:
	{
		bParsed = TRUE;
		return (INT_PTR)getBackClrBrush();
	}
	break;

	case WM_CTLCOLORBTN:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
	{
		if (const auto *const p_Control = this->getParentDialog()->getControlByHWND((HWND)lParam); p_Control != nullptr)
		{
			const auto clrText = p_Control->getTextColor();
			const auto clrBackText = p_Control->getBackColor();
			auto hBackBrush = p_Control->getBackClrBrush();

			bParsed = TRUE;

			//lRes = p_Control->CallDefaultClassProc(uMsg, wParam, lParam);	// error, parent window handles this, NOT the child control
			lRes = CallDefaultClassProc(uMsg, wParam, lParam);

			if (clrText != CLR_INVALID)
				SetTextColor((HDC)wParam, clrText);

			if (clrBackText != CLR_INVALID)
				SetBkColor((HDC)wParam, clrBackText);

			if (p_Control->isExStyle(WindowExStyle::Transparent))
			{
				// when transparent set as no bkg brush & default transparent drawing.
				SetBkMode((HDC)wParam, TRANSPARENT);
				hBackBrush = static_cast<HBRUSH>(GetStockObject(HOLLOW_BRUSH));
			}

			if (hBackBrush != nullptr)
				lRes = (LRESULT)hBackBrush;
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("lbdown,%u"), getUserID());
	}
	break;

	case WM_LBUTTONUP:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
		{
			execAliasEx(TEXT("lbup,%u"), getUserID());
			execAliasEx(TEXT("sclick,%u"), getUserID());
		}
	}
	break;

	case WM_LBUTTONDBLCLK:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
		{
			execAliasEx(TEXT("dclick,%u"), getUserID());
			execAliasEx(TEXT("lbdblclk,%u"), getUserID());
		}
	}
	break;

	case WM_RBUTTONDOWN:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("rbdown,%u"), getUserID());
	}
	break;

	case WM_RBUTTONUP:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("rbup,%u"), getUserID());
	}
	break;

	case WM_RBUTTONDBLCLK:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("rdclick,%u"), getUserID());
	}
	break;

	case WM_CONTEXTMENU:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
		{
			if (execAliasEx(TEXT("rclick,%u"), getUserID()))
				lRes = CallDefaultClassProc(uMsg, wParam, lParam);	// this allows the display of local context menus (like copy/paste with edit control)
			// default menu is NOT displayed when rclick event returns $false
			// this allows the display of custom menus...
		}
		bParsed = TRUE; // stops event being passed down to parent controls
	}
	break;
	case WM_DROPFILES:
	{
		dcxwParam(HDROP, files);

		//TCHAR filename[500];
		//const auto count = DragQueryFile(files, 0xFFFFFFFF, filename, Dcx::countof(filename));
		//
		//if (count > 0) {
		//	if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_DRAG)) {
		//		TCHAR ret[20];
		//
		//		evalAliasEx(ret, Dcx::countof(ret), TEXT("dragbegin,%u,%u"), getUserID(), count);
		//
		//		// cancel drag drop event
		//		if (lstrcmpi(ret, TEXT("cancel")) == 0) {
		//			DragFinish(files);
		//			return 0L;
		//		}
		//
		//		// for each file, send callback message
		//		for (auto i = decltype(count){0}; i < count; i++) {
		//			if (DragQueryFile(files, i, filename, Dcx::countof(filename)))
		//				execAliasEx(TEXT("dragfile,%u,%s"), getUserID(), filename);
		//		}
		//
		//		execAliasEx(TEXT("dragfinish,%u"), getUserID());
		//	}
		//}
		//
		//DragFinish(files);

		const stString<500> sFilename;

		if (const auto count = DragQueryFile(files, 0xFFFFFFFF, sFilename, sFilename.size()); count > 0)
		{
			if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_DRAG))
			{
				const stString<20> sRet;

				evalAliasEx(sRet, gsl::narrow_cast<int>(sRet.size()), TEXT("dragbegin,%u,%u"), getUserID(), count);

				// cancel drag drop event
				if (sRet == TEXT("cancel"))
				{
					DragFinish(files);
					return 0L;
				}

				// for each file, send callback message
				for (auto i = decltype(count){0}; i < count; ++i)
				{
					if (DragQueryFile(files, i, sFilename, sFilename.size()))
						execAliasEx(TEXT("dragfile,%u,%s"), getUserID(), sFilename);
				}

				execAliasEx(TEXT("dragfinish,%u"), getUserID());
			}
		}

		DragFinish(files);
		break;
	}
	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (hdr == nullptr)
			break;

		switch (hdr->code)
		{
		case TTN_GETDISPINFO:
		{
			if (!this->m_tsToolTip.empty())
			{
				dcxlParam(LPNMTTDISPINFO, di);

				di->lpszText = this->m_tsToolTip.to_chr();
				di->hinst = nullptr;
				bParsed = TRUE;
			}
		}
		break;
		case TTN_LINKCLICK:
		{
			bParsed = TRUE;
			execAliasEx(TEXT("tooltiplink,%u"), getUserID());
		}
		break;
		}
	}
	break;
	// Default WM_PRINTCLIENT method that handles alpha for us.
	// This Message is required for AnimateWindow() to work (also used by new transparency/alpha code)
	case WM_PRINTCLIENT:
	{
		dcxwParam(HDC, hdc);

		bParsed = TRUE;

		// Setup alpha blend if any.
		const auto ai = SetupAlphaBlend(&hdc);
		Auto(FinishAlphaBlend(ai));

		lRes = CallDefaultClassProc(uMsg, (WPARAM)hdc, lParam);
	}
	break;
	case WM_PRINT:
	{
		if (m_bInPrint) // avoid a drawing loop.
			bParsed = TRUE;
	}
	break;

	// redraw the control if the theme has changed
	case WM_THEMECHANGED:
		redrawWindow();
		break;

	case WM_CLOSE:
	{
		if (GetKeyState(VK_ESCAPE) != 0) // don't allow the window to close if escape is pressed. Needs looking into for a better method.
			bParsed = TRUE;
	}
	break;
	case WM_KEYDOWN:
	{
		if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
		{
			if (wParam == VK_RETURN)
				execAliasEx(TEXT("return,%u"), getUserID());

			//if ((this->m_bIgnoreRepeat) && (lParam & 0x40000000)) // ignore repeats
			// break;

			execAliasEx(TEXT("keydown,%u,%d"), getUserID(), wParam);
		}
		break;
	}
	case WM_KEYUP:
	{
		if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
			execAliasEx(TEXT("keyup,%u,%d"), getUserID(), wParam);
		break;
	}

	//case WM_WINDOWPOSCHANGING:
	//{
	//	if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_MOVE)) {
	//		WINDOWPOS *wp = (WINDOWPOS *) lParam;
//
		//		// break if nomove & nosize specified, since thats all we care about.
		//		if ((dcx_testflag(wp->flags, SWP_NOMOVE)) && (dcx_testflag(wp->flags, SWP_NOSIZE)))
		//			break;
//
		//		TCHAR ret[256];
//
		//		this->callAliasEx(ret, "changing,%d,%d,%d,%d,%d,%d", this->getUserID(), (dcx_testflag(wp->flags, 3)),wp->x, wp->y, wp->cx, wp->cy);
//
		//		if (wp != nullptr) {
		//			if (ts_strcmp(TEXT("nosize"), ret) == 0)
		//				wp->flags |= SWP_NOSIZE;
		//			else if (ts_strcmp(TEXT("nomove"), ret) == 0)
		//				wp->flags |= SWP_NOMOVE;
		//			else if (ts_strcmp(TEXT("nochange"), ret) == 0)
		//				wp->flags |= SWP_NOSIZE | SWP_NOMOVE;
		//		}
		//	}
		//	break;
		//}
	}
	return lRes;
}

void DcxControl::HandleChildrenSize()
{
	HWND bars = nullptr;

	while ((bars = FindWindowEx(m_Hwnd, bars, DCX_REBARCTRLCLASS, nullptr)) != nullptr)
	{
		SendMessage(bars, WM_SIZE, (WPARAM)0, (LPARAM)0);
	}

	while ((bars = FindWindowEx(m_Hwnd, bars, DCX_STATUSBARCLASS, nullptr)) != nullptr)
	{
		SendMessage(bars, WM_SIZE, (WPARAM)0, (LPARAM)0);
	}

	while ((bars = FindWindowEx(m_Hwnd, bars, DCX_TOOLBARCLASS, nullptr)) != nullptr)
	{
		SendMessage(bars, WM_SIZE, (WPARAM)0, (LPARAM)0);
	}

	if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_SIZE))
		execAliasEx(TEXT("sizing,%u"), getUserID());
}

// Invalidate controls area in parent.
void DcxControl::InvalidateParentRect(HWND hwnd)
{
#if DCX_USE_WRAPPERS
	const auto parent = GetParent(hwnd);
	const Dcx::dcxWindowRect rc(hwnd, parent);

	InvalidateRect(parent, &rc, TRUE);
#else
	RECT rc{};
	auto parent = GetParent(hwnd);

	if (!GetWindowRectParent(hwnd, &rc))
		return;

	InvalidateRect(parent, &rc, TRUE);
#endif
}

void DcxControl::calcTextRect(HDC hdc, const TString &txt, LPRECT rc, const UINT style)
{
	auto t(txt);
	if (this->IsControlCodeTextEnabled())
		t.strip();
	if (this->IsShadowTextEnabled())
		dcxDrawShadowText(hdc, t.to_chr(), t.len(), rc, style | DT_CALCRECT, this->m_clrText, 0,5,5);
	else
		DrawText(hdc, t.to_chr(), gsl::narrow_cast<int>(t.len()), rc, style | DT_CALCRECT);
}

void DcxControl::ctrlDrawText(HDC hdc, const TString &txt, const LPRECT rc, const UINT style)
{
	const auto oldClr = SetTextColor(hdc, m_clrText);
	Auto(SetTextColor(hdc, oldClr));

	if (!this->IsControlCodeTextEnabled())
	{
		const auto oldBkgMode = SetBkMode(hdc, TRANSPARENT);
		Auto(SetBkMode(hdc, oldBkgMode));

		if (this->IsShadowTextEnabled())
			dcxDrawShadowText(hdc, txt.to_chr(), txt.len(), rc, style, this->m_clrText, 0, 5, 5);
		else
			DrawText(hdc, txt.to_chr(), gsl::narrow_cast<int>(txt.len()), rc, style);
	}
	else
		mIRC_DrawText(hdc, txt, rc, style, this->IsShadowTextEnabled());
}

const TString DcxControl::getStyles(void) const
{
	TString result;
	const auto exStyles = dcxGetWindowExStyle(m_Hwnd);
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (Dcx::UXModule.dcxGetWindowTheme(m_Hwnd) == nullptr)
		result = TEXT("notheme");
	if ( dcx_testflag(Styles,WS_TABSTOP ) )
		result.addtok(TEXT("tabstop"));
	if ( dcx_testflag(Styles,WS_GROUP ) )
		result.addtok(TEXT("group"));
	if ( dcx_testflag(Styles,WS_DISABLED ) )
		result.addtok(TEXT("disabled"));
	if ( dcx_testflag(exStyles,WS_EX_TRANSPARENT ))
		result.addtok(TEXT("transparent"));
	if ( !dcx_testflag(Styles,WS_VISIBLE ))
		result.addtok(TEXT("hidden"));
	if ( this->IsAlphaBlend() )
		result.addtok(TEXT("alpha"));
	if ( this->IsShadowTextEnabled() )
		result.addtok(TEXT("shadow"));
	if ( !this->IsControlCodeTextEnabled() ) 
		result.addtok(TEXT("noformat"));
	if ( this->IsGradientFillEnabled() )
	{
		if ( this->IsGradientFillVertical() )
			result.addtok(TEXT("vgradient"));
		else 
			result.addtok(TEXT("hgradient"));
	}
	result.addtok(TEXT("utf8"));
	return result;
}

const TString DcxControl::getBorderStyles(void) const
{
	TString bstyles;
	const auto exStyles = dcxGetWindowExStyle(m_Hwnd);
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles,WS_BORDER))
		bstyles += TEXT('b');
	if (dcx_testflag(exStyles,WS_EX_CLIENTEDGE))
		bstyles += TEXT('c');
	if (dcx_testflag(Styles,WS_DLGFRAME))
		bstyles += TEXT('d');
	if (dcx_testflag(exStyles,WS_EX_DLGMODALFRAME))
		bstyles += TEXT('f');
	if (dcx_testflag(exStyles,WS_EX_STATICEDGE))
		bstyles += TEXT('s');
	if (dcx_testflag(exStyles,WS_EX_WINDOWEDGE))
		bstyles += TEXT('w');
	return bstyles;
}

void DcxControl::toXml(TiXmlElement *const xml) const
{
	const auto styles(getStyles());

	xml->SetAttribute("id", gsl::narrow_cast<int>(getUserID()));
	xml->SetAttribute("type", getType().c_str());
	if (!styles.empty())
		xml->SetAttribute("styles", styles.c_str());
}

TiXmlElement * DcxControl::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

// Convert a number into the closest icon size
DcxIconSizes DcxControl::NumToIconSize(const int &num) noexcept
{
	// if size is > max allowed, return max
	if (num >= gsl::narrow_cast<int>(DcxIconSizes::MaxSize))
		return DcxIconSizes::MaxSize;
	// we now know that size is < max allowed, so if its > smallest size, return medium
	if (num > gsl::narrow_cast<int>(DcxIconSizes::SmallIcon))
		return DcxIconSizes::MediumIcon;
	// otherwise return small
	return DcxIconSizes::SmallIcon;
}

void DcxControl::InitializeDcxControls(void)
{
	if (m_bInitialized)
		return;

//	WNDCLASSEX wc;
//	ZeroMemory((void*)&wc, sizeof(WNDCLASSEX));
//	wc.cbSize = sizeof(WNDCLASSEX);
//	
//#define dcxRegisterClass(szClass, szDcxClass, pClassObj) { \
//	if (GetClassInfoEx(nullptr, (TCHAR *)(szClass), &wc) != 0) { \
//		wc.lpszClassName = (TCHAR *)(szDcxClass); \
//		pClassObj::m_hDefaultClassProc = wc.lpfnWndProc; \
//		wc.lpfnWndProc = DcxControl::WindowProc; \
//		wc.hInstance = GetModuleHandle(nullptr); \
//		wc.style &= ~CS_GLOBALCLASS; \
//		RegisterClassEx(&wc); \
//	} \
//};
//
	//// Custom ProgressBar
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ProgressBar..."));
	//dcxRegisterClass(PROGRESS_CLASS, DCX_PROGRESSBARCLASS, DcxProgressBar);
	//
	//// Custom TreeView
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering TreeView..."));
	//dcxRegisterClass(WC_TREEVIEW, DCX_TREEVIEWCLASS, DcxTreeView);
	//
	//// Custom Toolbar
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ToolBar..."));
	//dcxRegisterClass(TOOLBARCLASSNAME, DCX_TOOLBARCLASS, DcxToolBar);
	//
	//// Custom StatusBar
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering StatusBar..."));
	//dcxRegisterClass(STATUSCLASSNAME, DCX_STATUSBARCLASS, DcxStatusBar);
	//
	//// Custom ListView
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Listview..."));
	//dcxRegisterClass(WC_LISTVIEW, DCX_LISTVIEWCLASS, DcxListView);
	//
	//// Custom ComboEx
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ComboEx..."));
	//dcxRegisterClass(WC_COMBOBOXEX, DCX_COMBOEXCLASS, DcxComboEx);
	//
	//// Custom TrackBar
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering TrackBar..."));
	//dcxRegisterClass(TRACKBAR_CLASS, DCX_TRACKBARCLASS, DcxTrackBar);
	//
	//// Custom RichEdit
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering RichEdit..."));
	//dcxRegisterClass(RICHEDIT_CLASS, DCX_RICHEDITCLASS, DcxRichEdit);
	//
	//// Custom RebarCtrl
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ReBar..."));
	//dcxRegisterClass(REBARCLASSNAME, DCX_REBARCTRLCLASS, DcxReBar);
	//
	//// Custom Color Combo
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ComboBox..."));
	//dcxRegisterClass(WC_COMBOBOX, DCX_COLORCOMBOCLASS, DcxColorCombo);
	//
	//// Custom TabCtrl
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Tab..."));
	//dcxRegisterClass(WC_TABCONTROL, DCX_TABCTRLCLASS, DcxTab);
	//
	//// Custom UpDown
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering UpDown..."));
	//dcxRegisterClass(UPDOWN_CLASS, DCX_UPDOWNCLASS, DcxUpDown);
	//
	//// Custom IpAddress
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering IpAddress..."));
	//dcxRegisterClass(WC_IPADDRESS, DCX_IPADDRESSCLASS, DcxIpAddress);
	//
	//// Custom Button
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Button..."));
	//dcxRegisterClass(WC_BUTTON, DCX_BUTTONCLASS, DcxButton);
	//
	//// Custom Check
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Check..."));
	//dcxRegisterClass(WC_BUTTON, DCX_CHECKCLASS, DcxCheck);
	//
	//// Custom Radio
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Radio..."));
	//dcxRegisterClass(WC_BUTTON, DCX_RADIOCLASS, DcxRadio);
	//
	//// Custom Calendar
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Calendar..."));
	//dcxRegisterClass(MONTHCAL_CLASS, DCX_CALENDARCLASS, DcxCalendar);
	//
	//// Custom DateTime
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering DateTime..."));
	//dcxRegisterClass(DATETIMEPICK_CLASS, DCX_DATETIMECLASS, DcxDateTime);
	//
	//// Custom Pager
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Pager..."));
	//dcxRegisterClass(WC_PAGESCROLLER, DCX_PAGERCLASS, DcxPager);
	//
	//// Custom Image
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Image..."));
	//dcxRegisterClass(WC_STATIC, DCX_IMAGECLASS, DcxImage);
	//
	//// Custom Edit
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Edit..."));
	//dcxRegisterClass(WC_EDIT, DCX_EDITCLASS, DcxEdit);
		//
	//// Custom Divider
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Divider..."));
	//wc.cbSize = sizeof(WNDCLASSEX);
	//wc.style = 0;
	//DcxDivider::m_hDefaultClassProc = DividerWndProc;
	//wc.lpfnWndProc = DcxControl::WindowProc;
	//wc.cbClsExtra = 0;
	//wc.cbWndExtra = 0;
	//wc.hInstance = GetModuleHandle(nullptr);
	//wc.hIcon = nullptr;
	//wc.hCursor = nullptr;
	//wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
	//wc.lpszMenuName = 0;
	//wc.lpszClassName = DCX_DIVIDERCLASS;
	//wc.hIconSm = nullptr;
	//RegisterClassEx(&wc);
//
	//// Custom Panel
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Panel..."));
	//wc.cbSize = sizeof(WNDCLASSEX);
	//wc.style = CS_PARENTDC;
	//DcxPanel::m_hDefaultClassProc = DefWindowProc;
	//wc.lpfnWndProc = DcxControl::WindowProc;
	//wc.cbClsExtra = 0;
	//wc.cbWndExtra = 0;
	//wc.hInstance = GetModuleHandle(nullptr);
	//wc.hIcon = nullptr;
	//wc.hCursor = LoadCursor(GetModuleHandle(nullptr), IDC_ARROW);
	//wc.hbrBackground = nullptr; //(HBRUSH) (COLOR_3DFACE +1);
	//wc.lpszMenuName = 0;
	//wc.lpszClassName = DCX_PANELCLASS;
	//wc.hIconSm = nullptr;
	//RegisterClassEx(&wc);
//
	//// Custom Box
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Box..."));
	//wc.cbSize = sizeof(WNDCLASSEX);
	//wc.style = CS_PARENTDC;
	//DcxBox::m_hDefaultClassProc = DefWindowProc;
	//wc.lpfnWndProc = DcxControl::WindowProc;
	//wc.cbClsExtra = 0;
	//wc.cbWndExtra = 0;
	//wc.hInstance = GetModuleHandle(nullptr);
	//wc.hIcon = nullptr;
	//wc.hCursor = LoadCursor(GetModuleHandle(nullptr), IDC_ARROW);
	//wc.hbrBackground = nullptr; //(HBRUSH) (COLOR_3DFACE +1);
	//wc.lpszMenuName = 0;
	//wc.lpszClassName = DCX_BOXCLASS;
	//wc.hIconSm = nullptr;
	//RegisterClassEx(&wc);
//
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Web..."));
	//dcxRegisterClass(WC_STATIC, DCX_WEBCLASS, DcxWebControl);
//
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Line..."));
	//dcxRegisterClass(WC_STATIC, DCX_LINECLASS, DcxLine);
//
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Link..."));
	//dcxRegisterClass(WC_STATIC, DCX_LINKCLASS, DcxLink);
	////dcxRegisterClass(WC_LINK, DCX_LINKCLASS, DcxLink);
//
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering List..."));
	//dcxRegisterClass(WC_LISTBOX, DCX_LISTCLASS, DcxList);
//
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Scroll..."));
	//dcxRegisterClass(WC_SCROLLBAR, DCX_SCROLLCLASS, DcxScroll);
//
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Text..."));
	//dcxRegisterClass(WC_STATIC, DCX_TEXTCLASS, DcxText);
//
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering DirectShow..."));
	//dcxRegisterClass(WC_STATIC, DCX_DIRECTSHOWCLASS, DcxDirectshow);
//
	//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Stacker..."));
	//dcxRegisterClass(WC_LISTBOX, DCX_STACKERCLASS, DcxStacker);
//
	//m_bInitialized = true;
//
	//return;

	// Custom ProgressBar
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ProgressBar..."));
	dcxRegisterClass<DcxProgressBar>(PROGRESS_CLASS, DCX_PROGRESSBARCLASS);

	// Custom TreeView
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering TreeView..."));
	dcxRegisterClass<DcxTreeView>(WC_TREEVIEW, DCX_TREEVIEWCLASS);

	// Custom Toolbar
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ToolBar..."));
	dcxRegisterClass<DcxToolBar>(TOOLBARCLASSNAME, DCX_TOOLBARCLASS);

	// Custom StatusBar
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering StatusBar..."));
	dcxRegisterClass<DcxStatusBar>(STATUSCLASSNAME, DCX_STATUSBARCLASS);

	// Custom ListView
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Listview..."));
	dcxRegisterClass<DcxListView>(WC_LISTVIEW, DCX_LISTVIEWCLASS);

	// Custom ComboEx
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ComboEx..."));
	dcxRegisterClass<DcxComboEx>(WC_COMBOBOXEX, DCX_COMBOEXCLASS);

	// Custom TrackBar
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering TrackBar..."));
	dcxRegisterClass<DcxTrackBar>(TRACKBAR_CLASS, DCX_TRACKBARCLASS);

	// Custom RichEdit
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering RichEdit..."));
	dcxRegisterClass<DcxRichEdit>(RICHEDIT_CLASS, DCX_RICHEDITCLASS);

	// Custom RebarCtrl
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ReBar..."));
	dcxRegisterClass<DcxReBar>(REBARCLASSNAME, DCX_REBARCTRLCLASS);

	// Custom Color Combo
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ComboBox..."));
	dcxRegisterClass<DcxColorCombo>(WC_COMBOBOX, DCX_COLORCOMBOCLASS);

	// Custom TabCtrl
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Tab..."));
	dcxRegisterClass<DcxTab>(WC_TABCONTROL, DCX_TABCTRLCLASS);

	// Custom UpDown
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering UpDown..."));
	dcxRegisterClass<DcxUpDown>(UPDOWN_CLASS, DCX_UPDOWNCLASS);

	// Custom IpAddress
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering IpAddress..."));
	dcxRegisterClass<DcxIpAddress>(WC_IPADDRESS, DCX_IPADDRESSCLASS);

	// Custom Button
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Button..."));
	dcxRegisterClass<DcxButton>(WC_BUTTON, DCX_BUTTONCLASS);

	// Custom Check
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Check..."));
	dcxRegisterClass<DcxCheck>(WC_BUTTON, DCX_CHECKCLASS);

	// Custom Radio
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Radio..."));
	dcxRegisterClass<DcxRadio>(WC_BUTTON, DCX_RADIOCLASS);

	// Custom Calendar
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Calendar..."));
	dcxRegisterClass<DcxCalendar>(MONTHCAL_CLASS, DCX_CALENDARCLASS);

	// Custom DateTime
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering DateTime..."));
	dcxRegisterClass<DcxDateTime>(DATETIMEPICK_CLASS, DCX_DATETIMECLASS);

	// Custom Pager
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Pager..."));
	dcxRegisterClass<DcxPager>(WC_PAGESCROLLER, DCX_PAGERCLASS);

	// Custom Image
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Image..."));
	dcxRegisterClass<DcxImage>(WC_STATIC, DCX_IMAGECLASS);

	// Custom Edit
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Edit..."));
	dcxRegisterClass<DcxEdit>(WC_EDIT, DCX_EDITCLASS);

	{
		const auto hHandle = GetModuleHandle(nullptr);

		// Custom Divider
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Divider..."));

		{
			const WNDCLASSEX wc{ sizeof(WNDCLASSEX),0, DcxControl::WindowProc, 0,0,hHandle,nullptr,nullptr,(HBRUSH)(COLOR_3DFACE + 1),0,DCX_DIVIDERCLASS,nullptr };
			DcxDivider::m_hDefaultClassProc = DividerWndProc;
			RegisterClassEx(&wc);
		}

		// Custom Panel
		{
			DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Panel..."));

			const WNDCLASSEX wc{ sizeof(WNDCLASSEX), CS_PARENTDC, DcxControl::WindowProc, 0, 0, hHandle, nullptr, LoadCursor(hHandle, IDC_ARROW), nullptr, 0, DCX_PANELCLASS, nullptr };
			DcxPanel::m_hDefaultClassProc = DefWindowProc;
			RegisterClassEx(&wc);
		}

		// Custom Box
		{
			DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Box..."));
			const WNDCLASSEX wc{ sizeof(WNDCLASSEX), CS_PARENTDC, DcxControl::WindowProc, 0, 0, hHandle, nullptr, LoadCursor(hHandle, IDC_ARROW), nullptr, 0, DCX_BOXCLASS, nullptr };
			DcxBox::m_hDefaultClassProc = DefWindowProc;
			RegisterClassEx(&wc);
		}
	}

	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Web..."));
	dcxRegisterClass<DcxWebControl>(WC_STATIC, DCX_WEBCLASS);

	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Line..."));
	dcxRegisterClass<DcxLine>(WC_STATIC, DCX_LINECLASS);

	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Link..."));
	dcxRegisterClass<DcxLink>(WC_STATIC, DCX_LINKCLASS);
	//dcxRegisterClass<DcxLink>(WC_LINK, DCX_LINKCLASS);

	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering List..."));
	dcxRegisterClass<DcxList>(WC_LISTBOX, DCX_LISTCLASS);

	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Scroll..."));
	dcxRegisterClass<DcxScroll>(WC_SCROLLBAR, DCX_SCROLLCLASS);

	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Text..."));
	dcxRegisterClass<DcxText>(WC_STATIC, DCX_TEXTCLASS);

	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering DirectShow..."));
	dcxRegisterClass<DcxDirectshow>(WC_STATIC, DCX_DIRECTSHOWCLASS);

	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Stacker..."));
	dcxRegisterClass<DcxStacker>(WC_LISTBOX, DCX_STACKERCLASS);

	m_bInitialized = true;

	return;
}

void DcxControl::UnInitializeDcxControls() noexcept
{
	m_bInitialized = false;

	UnregisterClass(DCX_PROGRESSBARCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_TREEVIEWCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_TOOLBARCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_STATUSBARCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_LISTVIEWCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_COMBOEXCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_TRACKBARCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_RICHEDITCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_REBARCTRLCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_COLORCOMBOCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_TABCTRLCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_UPDOWNCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_IPADDRESSCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_BUTTONCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_CHECKCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_RADIOCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_CALENDARCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_DATETIMECLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_PAGERCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_IMAGECLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_EDITCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_DIVIDERCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_PANELCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_BOXCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_WEBCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_LINECLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_LINKCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_LISTCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_SCROLLCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_TEXTCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_DIRECTSHOWCLASS, GetModuleHandle(nullptr));
	UnregisterClass(DCX_STACKERCLASS, GetModuleHandle(nullptr));
}
