/*!
http://www.codeproject.com/miscctrl/transparentslider.asp
 * \file dcxtrackbar.cpp
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
#include "Classes/dcxtrackbar.h"
#include "Classes/dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxTrackBar::DcxTrackBar(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog),
	m_bUpdatingTooltip(false),
	m_colTransparent(CLR_INVALID)
{
	this->m_hbmp[TBBMP_BACK] = nullptr;
	this->m_hbmp[TBBMP_THUMB] = nullptr;
	this->m_hbmp[TBBMP_THUMBDRAG] = nullptr;
	this->m_hbmp[TBBMP_CHANNEL] = nullptr;

	const auto[bNoTheme, Styles, ExStyles] = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ExStyles,
		DCX_TRACKBARCLASS,
		Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	// Keep track of the tooltip
	if (dcx_testflag(Styles, TBS_TOOLTIPS))
	{
		auto tooltip = (HWND)SendMessage(m_Hwnd, TBM_GETTOOLTIPS, NULL, NULL);

		if (tooltip != nullptr)
			this->m_ToolTipHWND = tooltip;
	}

	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxTrackBar::~DcxTrackBar( )
{
	if (this->m_hbmp[TBBMP_BACK] != nullptr)
		DeleteBitmap(this->m_hbmp[TBBMP_BACK]);
	if (this->m_hbmp[TBBMP_THUMB] != nullptr)
		DeleteBitmap(this->m_hbmp[TBBMP_THUMB]);
	if (this->m_hbmp[TBBMP_THUMBDRAG] != nullptr)
		DeleteBitmap(this->m_hbmp[TBBMP_THUMBDRAG]);
	if (this->m_hbmp[TBBMP_CHANNEL] != nullptr)
		DeleteBitmap(this->m_hbmp[TBBMP_CHANNEL]);

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

//void DcxTrackBar::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
//{
//	*Styles |= TBS_FIXEDLENGTH;
//
//	for (const auto &tsStyle: styles)
//	{
//#if DCX_USE_HASHING
//		switch (std::hash<TString>{}(tsStyle))
//		{
//			case L"autoticks"_hash:
//				*Styles |= TBS_AUTOTICKS;
//				break;
//			case L"both"_hash:
//				*Styles |= TBS_BOTH;
//				break;
//			case L"top"_hash:
//				*Styles |= TBS_TOP;		// == TBS_LEFT == 4
//				break;
//			case L"bottom"_hash:
//				*Styles |= TBS_BOTTOM;	// == TBS_RIGHT == 0, so never set.... should just remove TBS_RIGHT/TOP ?
//				break;
//			case L"left"_hash:
//				*Styles |= TBS_LEFT;	// == TBS_TOP == 4
//				break;
//			case L"right"_hash:
//				*Styles |= TBS_RIGHT;	// == TBS_BOTTOM == 0
//				break;
//			case L"select"_hash:
//				*Styles |= TBS_ENABLESELRANGE;
//				break;
//			case L"vertical"_hash:
//				*Styles |= TBS_VERT;
//				break;
//			case L"nothumb"_hash:
//				*Styles |= TBS_NOTHUMB;
//				break;
//			case L"noticks"_hash:
//				*Styles |= TBS_NOTICKS;
//				break;
//			case L"reversed"_hash:
//				*Styles |= TBS_REVERSED;
//				break;
//			case L"downisleft"_hash:
//				*Styles |= TBS_DOWNISLEFT;
//				break;
//			case L"tooltips"_hash:
//				*Styles |= TBS_TOOLTIPS;
//				break;
//			case L"transparentbkg"_hash:
//				*Styles |= TBS_TRANSPARENTBKGND;
//			default:
//				break;
//		}
//#else
//		if (tsStyle == TEXT("autoticks"))
//			*Styles |= TBS_AUTOTICKS;
//		else if ( tsStyle == TEXT("both") ) 
//			*Styles |= TBS_BOTH;
//		else if ( tsStyle == TEXT("top") ) 
//			*Styles |= TBS_TOP;		// == TBS_LEFT == 4
//		else if ( tsStyle == TEXT("bottom") ) 
//			*Styles |= TBS_BOTTOM;	// == TBS_RIGHT == 0, so never set.... should just remove TBS_RIGHT/TOP ?
//		else if ( tsStyle == TEXT("left") ) 
//			*Styles |= TBS_LEFT;	// == TBS_TOP == 4
//		else if ( tsStyle == TEXT("right") ) 
//			*Styles |= TBS_RIGHT;	// == TBS_BOTTOM == 0
//		else if ( tsStyle == TEXT("select") ) 
//			*Styles |= TBS_ENABLESELRANGE;
//		else if ( tsStyle == TEXT("vertical") ) 
//			*Styles |= TBS_VERT;
//		else if ( tsStyle == TEXT("nothumb") ) 
//			*Styles |= TBS_NOTHUMB;
//		else if ( tsStyle == TEXT("noticks") ) 
//			*Styles |= TBS_NOTICKS;
//		else if ( tsStyle == TEXT("reversed") ) 
//			*Styles |= TBS_REVERSED;
//		else if ( tsStyle == TEXT("downisleft") ) 
//			*Styles |= TBS_DOWNISLEFT;
//		else if ( tsStyle == TEXT("tooltips") ) 
//			*Styles |= TBS_TOOLTIPS;
//		else if ( tsStyle == TEXT("transparentbkg") )
//			*Styles |= TBS_TRANSPARENTBKGND;
//#endif
//	}
//
//	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
//}

std::tuple<NoTheme, WindowStyle, WindowExStyle> DcxTrackBar::parseControlStyles(const TString & tsStyles)
{
	auto[bNoTheme, Styles, ExStyles] = parseGeneralControlStyles(tsStyles);

	Styles |= TBS_FIXEDLENGTH;

	for (const auto &tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"autoticks"_hash:
			Styles |= TBS_AUTOTICKS;
			break;
		case L"both"_hash:
			Styles |= TBS_BOTH;
			break;
		case L"top"_hash:
			Styles |= TBS_TOP;		// == TBS_LEFT == 4
			break;
		case L"bottom"_hash:
			Styles |= TBS_BOTTOM;	// == TBS_RIGHT == 0, so never set.... should just remove TBS_RIGHT/TOP ?
			break;
		case L"left"_hash:
			Styles |= TBS_LEFT;	// == TBS_TOP == 4
			break;
		case L"right"_hash:
			Styles |= TBS_RIGHT;	// == TBS_BOTTOM == 0
			break;
		case L"select"_hash:
			Styles |= TBS_ENABLESELRANGE;
			break;
		case L"vertical"_hash:
			Styles |= TBS_VERT;
			break;
		case L"nothumb"_hash:
			Styles |= TBS_NOTHUMB;
			break;
		case L"noticks"_hash:
			Styles |= TBS_NOTICKS;
			break;
		case L"reversed"_hash:
			Styles |= TBS_REVERSED;
			break;
		case L"downisleft"_hash:
			Styles |= TBS_DOWNISLEFT;
			break;
		case L"tooltips"_hash:
			Styles |= TBS_TOOLTIPS;
			break;
		case L"transparentbkg"_hash:
			Styles |= TBS_TRANSPARENTBKGND;
		default:
			break;
		}
	}

	return { bNoTheme, Styles, ExStyles };
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxTrackBar::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
	case L"value"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), this->getPos());
		break;
	case L"range"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d %d"), this->getRangeMin(), this->getRangeMax());
		break;
	case L"line"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), this->getLineSize());
		break;
	case L"page"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), this->getPageSize());
		break;
	case L"selrange"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d %d"), this->getSelStart(), this->getSelEnd());
		break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
}
/*!
* \brief blah
 *
 * \param input [NAME] [SWITCH] [ID] (OPTIONS)
 */

void DcxTrackBar::parseCommandRequest( const TString & input )
{
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -c [NAME] [ID] [SWITCH] [VALUE]
	if ( flags[TEXT('c')] && numtok > 3 )
	{
		const auto lPosition = input.getnexttok().to_<long>();	// tok 4
		this->setTic( lPosition );
	}
	// xdid -g [NAME] [ID] [SWITCH] [FLAGS] [FILE]
	else if (flags[TEXT('g')] && numtok > 4)
	{
		const auto tflags = parseImageFlags(input.getnexttok());	// tok 4
		auto filename(input.getlasttoks().trim());				// tok 5, -1

		// background
		if (dcx_testflag(tflags, TBCS_BACK))
			this->m_hbmp[TBBMP_BACK] = dcxLoadBitmap(this->m_hbmp[TBBMP_BACK], filename);
		// thumb
		if (dcx_testflag(tflags, TBCS_THUMB))
			this->m_hbmp[TBBMP_THUMB] = dcxLoadBitmap(this->m_hbmp[TBBMP_THUMB], filename);
		// thumb hover
		if (dcx_testflag(tflags, TBCS_THUMBDRAG))
			this->m_hbmp[TBBMP_THUMBDRAG] = dcxLoadBitmap(this->m_hbmp[TBBMP_THUMBDRAG], filename);
		// channel
		if (dcx_testflag(tflags, TBCS_CHANNEL))
			this->m_hbmp[TBBMP_CHANNEL] = dcxLoadBitmap(this->m_hbmp[TBBMP_CHANNEL], filename);

		// these dont seem to work so dont bother calling it
		//this->redrawWindow();
		//InvalidateRect(m_Hwnd, nullptr, TRUE);
	}
	// xdid -j [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if ( flags[TEXT('j')] && numtok > 4 )
	{
		const auto iMin = input.getnexttok().to_<long>();	// tok 4
		const auto iMax = input.getnexttok().to_<long>();	// tok 5
		this->setSel( iMin, iMax );
	}
	// xdid -l [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('l')] && numtok > 3 )
	{
		const auto lLineSize = input.getnexttok().to_<long>();	// tok 4
		this->setLineSize( lLineSize );
	}
	// xdid -m [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('m')] && numtok > 3 )
	{
		const auto lPageSize = input.getnexttok().to_<long>();	// tok 4
		this->setPageSize( lPageSize );
	}
	// xdid -n [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('n')] && numtok > 3 )
	{
		const auto iTicFreq = input.getnexttok().to_<long>();	// tok 4
		this->setTicFreq( iTicFreq );
	}
	// xdid -q [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('q')] )
	{
		this->clearTics( );
	}
	// xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if ( flags[TEXT('r')] && numtok > 4 )
	{
		const auto lMinRange = input.getnexttok().to_<long>();	// tok 4
		const auto lMaxRange = input.getnexttok().to_<long>();	// tok 5

		this->setRangeMin( lMinRange );
		this->setRangeMax( lMaxRange );
	}
	// xdid -o [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('o')] && numtok > 3)
	{
		m_colTransparent = input.getnexttok().to_<COLORREF>();	// tok 4
		this->redrawWindow();
	}
	// xdid -t [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('t')] && numtok > 3 )
	{
		const auto value(input.getnexttok());	// tok 4

		if ( value == TEXT("left") )
			this->setTipSide( TBTS_LEFT );
		else if ( value == TEXT("right") )
			this->setTipSide( TBTS_RIGHT );
		else if ( value == TEXT("bottom") )
			this->setTipSide( TBTS_BOTTOM );
		else
			this->setTipSide( TBTS_TOP );
	}
	// xdid -u [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('u')] && numtok > 3 )
	{
		const auto lLength = input.getnexttok().to_<UINT>();	// tok 4

		this->setThumbLength( lLength );
	}
	// xdid -v [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('v')] && numtok > 3 )
	{
		const auto lPosition = input.getnexttok().to_<long>();	// tok 4

		this->setPos( lPosition );
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setRangeMin( const LONG iLowLim )
{
	return SendMessage( m_Hwnd, TBM_SETRANGEMIN, (WPARAM) TRUE, (LPARAM) iLowLim );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getRangeMin(  ) const
{
	return SendMessage( m_Hwnd, TBM_GETRANGEMIN, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setRangeMax( const LONG iHighLim )
{
	return SendMessage( m_Hwnd, TBM_SETRANGEMAX, (WPARAM) TRUE, (LPARAM) iHighLim );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getRangeMax(  ) const
{
	return SendMessage( m_Hwnd, TBM_GETRANGEMAX, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setRange( const LONG iLowLim, const LONG iHighLim )
{
	return SendMessage( m_Hwnd, TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG ( iLowLim, iHighLim ) );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setPos( const LONG lPosition )
{
	return SendMessage( m_Hwnd, TBM_SETPOS, (WPARAM) TRUE, (LPARAM) lPosition );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getPos(  ) const
{
	return SendMessage( m_Hwnd, TBM_GETPOS, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

void DcxTrackBar::setTic( const LONG lPosition )
{
	SendMessage( m_Hwnd, TBM_SETTIC, (WPARAM) 0U, (LPARAM) lPosition );
}

/*!
* \brief blah
*
* blah
*/

void DcxTrackBar::setTicFreq( const LONG wFreq )
{
	SendMessage( m_Hwnd, TBM_SETTICFREQ, (WPARAM) wFreq, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::clearTics( )
{
	return SendMessage( m_Hwnd, TBM_CLEARTICS, (WPARAM) TRUE, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setTipSide( const int fLocation )
{
	return SendMessage( m_Hwnd, TBM_SETTIPSIDE, (WPARAM) fLocation, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setPageSize( const LONG lPageSize )
{
	return SendMessage( m_Hwnd, TBM_SETPAGESIZE, (WPARAM) 0U, (LPARAM) lPageSize );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getPageSize(  ) const
{
	return SendMessage( m_Hwnd, TBM_GETPAGESIZE, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setLineSize( const LONG lLineSize )
{
	return SendMessage( m_Hwnd, TBM_SETLINESIZE, (WPARAM) 0U, (LPARAM) lLineSize );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getLineSize(  ) const
{
	return SendMessage( m_Hwnd, TBM_GETLINESIZE, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setThumbLength( const UINT iLength )
{
	return SendMessage( m_Hwnd, TBM_SETTHUMBLENGTH, (WPARAM) iLength, (LPARAM) 0U );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setSel( const LONG iLowLim, const LONG iHighLim )
{
	return SendMessage( m_Hwnd, TBM_SETSEL, (WPARAM) TRUE, (LPARAM) MAKELONG( iLowLim, iHighLim ) );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getSelStart(  ) const
{
	return SendMessage( m_Hwnd, TBM_GETSELSTART, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getSelEnd(  ) const
{
	return SendMessage( m_Hwnd, TBM_GETSELEND, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxTrackBar::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
{
	switch (uMsg)
	{
	case WM_VSCROLL:
	case WM_HSCROLL:
	{
		switch (LOWORD(wParam)) 
		{
		case TB_TOP:
			this->execAliasEx(TEXT("%s,%d,%d"), TEXT("top"), this->getUserID(), this->getPos());
			break;

		case TB_BOTTOM:
			this->execAliasEx(TEXT("%s,%d,%d"), TEXT("bottom"), this->getUserID(), this->getPos());
			break;

		case TB_PAGEUP:
			this->execAliasEx(TEXT("%s,%d,%d"), TEXT("pageup"), this->getUserID(), this->getPos());
			break;

		case TB_PAGEDOWN:
			this->execAliasEx(TEXT("%s,%d,%d"), TEXT("pagedown"), this->getUserID(), this->getPos());
			break;

		case TB_LINEUP:
			this->execAliasEx(TEXT("%s,%d,%d"), TEXT("lineup"), this->getUserID(), this->getPos());
			break;

		case TB_LINEDOWN:
			this->execAliasEx(TEXT("%s,%d,%d"), TEXT("linedown"), this->getUserID(), this->getPos());
			break;

		case TB_THUMBPOSITION:
			this->execAliasEx(TEXT("%s,%d,%d"), TEXT("trackend"), this->getUserID(), this->getPos());
			break;

		case SB_THUMBTRACK:
			this->execAliasEx(TEXT("%s,%d,%d"), TEXT("tracking"), this->getUserID(), this->getPos());
			break;
		}

		break;
	}

	case WM_NOTIFY: 
	{
		dcxlParam(LPNMHDR, hdr);

		if (hdr == nullptr)
			break;

		switch (hdr->code) 
		{
		case NM_CUSTOMDRAW: 
		{
			// if (no items to draw)
			if (
				(this->m_hbmp[TBBMP_BACK] == nullptr) &&
				(this->m_hbmp[TBBMP_THUMB] == nullptr) &&
				(this->m_hbmp[TBBMP_THUMBDRAG] == nullptr) &&
				(this->m_hbmp[TBBMP_CHANNEL] == nullptr)
				)
				break;

			dcxlParam(LPNMCUSTOMDRAW, nmcd);
			if (nmcd == nullptr)
				break;

			auto hdc = nmcd->hdc;
			if (hdc == nullptr)
				break;

			bParsed = TRUE;

			switch (nmcd->dwDrawStage) 
			{
			case CDDS_PREPAINT:
				return CDRF_NOTIFYITEMDRAW/* | CDRF_NOTIFYPOSTPAINT*/;

			case CDDS_ITEMPREPAINT:
			{
				// try to make it draw the tics, doesnt work =(
				if (nmcd->dwItemSpec == TBCD_TICS)
					return CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT;

				// channel that the trackbar control's thumb marker slides along
				if (nmcd->dwItemSpec == TBCD_CHANNEL)
				{
					// draw the background here
					DrawTrackBarPart(hdc, TBBMP_BACK);

					if (DrawTrackBarPart(hdc, TBBMP_CHANNEL, &nmcd->rc))
						return /*CDRF_NOTIFYPOSTPAINT | */CDRF_SKIPDEFAULT;

					//								return CDRF_NOTIFYPOSTPAINT | CDRF_SKIPDEFAULT;
					return CDRF_DODEFAULT;// | CDRF_NOTIFYPOSTPAINT;
				}
				// trackbar control's thumb marker. This is the portion of the control
				// that the user moves.  For the pre-item-paint of the thumb, we draw 
				// everything completely here, during item pre-paint, and then tell
				// the control to skip default painting and NOT to notify 
				// us during post-paint.
				if (nmcd->dwItemSpec == TBCD_THUMB)
				{
					auto iPartId = TBBMP_THUMB;

					// if thumb is selected/focussed, switch brushes
					if (dcx_testflag(nmcd->uItemState, CDIS_SELECTED))
						iPartId = TBBMP_THUMBDRAG;

					if (DrawTrackBarPart(hdc, iPartId, &nmcd->rc))
						return CDRF_SKIPDEFAULT;
				}

				// default!
				return CDRF_DODEFAULT;
			} // item prepaint

			case CDDS_ITEMPOSTPAINT:
				return CDRF_SKIPDEFAULT;
				// item postpaint

			default:
				return CDRF_DODEFAULT;
			} // end drawstage

			//break; // unreachable
		} // end NM_CUSTOMDRAW

		} // end SWITCH
	} // end notify
	}
	return 0L;
}

LRESULT DcxTrackBar::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) 
{
	switch (uMsg)
	{
		//case WM_ERASEBKGND:
		//	{
		//		if (this->isExStyle(WS_EX_TRANSPARENT))
		//			this->DrawParentsBackground((HDC)wParam);
		//		else {
		//			RECT rect;
		//			GetClientRect( m_Hwnd, &rect );
		//			DcxControl::DrawCtrlBackground((HDC) wParam,this,&rect);
		//		}
		//		bParsed = TRUE;
		//		return TRUE;
		//	}
		//	break;

	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (hdr == nullptr)
			break;

		switch (hdr->code)
		{
			// Show tooltip
		case TTN_SHOW:
			// Check if its currently being updated.
			if (this->m_bUpdatingTooltip)
				break;

			TString buff(80U);

			evalAliasEx(buff.to_chr(), gsl::narrow_cast<int>(buff.capacity()), TEXT("showtip,%u,%d"), getUserID(), getPos());

			if (!buff.empty())
			{
				TOOLINFO ti{};

				ti.cbSize = sizeof(TOOLINFO);
				ti.hinst = GetModuleHandle(nullptr);
				ti.hwnd = m_Hwnd;
				ti.uId = (UINT_PTR)m_Hwnd;
				ti.lpszText = buff.to_chr();

				this->m_bUpdatingTooltip = true;
				SendMessage(this->m_ToolTipHWND, TTM_UPDATETIPTEXT, NULL, (LPARAM)&ti);
				this->m_bUpdatingTooltip = false;
			}
			break; // case TTN_SHOW
		}
		break; // case WM_NOTIFY
	}

	case WM_PAINT:
	{
		if (!this->m_bAlphaBlend)
			break;

		PAINTSTRUCT ps{};
		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		bParsed = TRUE;

		// Setup alpha blend if any.
		auto ai = this->SetupAlphaBlend(&hdc);
		Auto(this->FinishAlphaBlend(ai));

		//if (this->isExStyle(WS_EX_TRANSPARENT))
		//	this->DrawParentsBackground(hdc);
		//else
		//	DcxControl::DrawCtrlBackground(hdc,this,&ps.rcPaint);

		return CallDefaultProc(m_Hwnd, uMsg, (WPARAM)hdc, lParam);
	}
	break;

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


UINT DcxTrackBar::parseImageFlags(const TString &flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('b')])
		iFlags |= TBCS_BACK;
	if (xflags[TEXT('c')])
		iFlags |= TBCS_CHANNEL;
	if (xflags[TEXT('d')])
		iFlags |= TBCS_THUMBDRAG;
	if (xflags[TEXT('t')])
		iFlags |= TBCS_THUMB;

	return iFlags;
}

bool DcxTrackBar::DrawTrackBarPart(HDC hdc, const TrackBarParts iPartId, const RECT *const rc)
{
	if (m_hbmp[iPartId] == nullptr)
		return false;

	RECT rect{};
	if (rc == nullptr)
	{
		if (!GetClientRect(m_Hwnd, &rect))
			return false;
	}
	else
		CopyRect(&rect, rc);

	BITMAP bmp{};
	if (GetObject(m_hbmp[iPartId], sizeof(BITMAP), &bmp) == 0)
		return false;

#if DCX_USE_WRAPPERS
	Dcx::dcxHDCBitmapResource hdcbmp(hdc, m_hbmp[iPartId]);
#else
	auto hdcbmp = CreateCompatibleDC(hdc);

	if (hdcbmp == nullptr)
		return false;
	Auto(DeleteDC(hdcbmp));

	auto oldBM = SelectBitmap(hdcbmp, m_hbmp[iPartId]);
	Auto(SelectBitmap(hdcbmp, oldBM));
#endif

	TransparentBlt(hdc,
		rect.left, rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top, hdcbmp,
		0, 0, bmp.bmWidth, bmp.bmHeight, m_colTransparent);

	return true;
}

const TString DcxTrackBar::getStyles(void) const
{
	auto tsStyles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, TBS_AUTOTICKS))
		tsStyles.addtok(TEXT("autoticks"));
	if (dcx_testflag(Styles, TBS_VERT))
	{
		tsStyles.addtok(TEXT("vertical"));
		if (dcx_testflag(Styles, TBS_TOP))	// 0x04
			tsStyles.addtok(TEXT("top"));
		if (dcx_testflag(Styles, TBS_BOTTOM))	// 0x00
			tsStyles.addtok(TEXT("bottom"));
	}
	else {
		if (dcx_testflag(Styles, TBS_LEFT))		// 0x04
			tsStyles.addtok(TEXT("left"));
		if (dcx_testflag(Styles, TBS_RIGHT))	// 0x00
			tsStyles.addtok(TEXT("right"));
	}
	if (dcx_testflag(Styles, TBS_BOTH))
		tsStyles.addtok(TEXT("both"));

	if (dcx_testflag(Styles, TBS_ENABLESELRANGE))
		tsStyles.addtok(TEXT("select"));
	if (dcx_testflag(Styles, TBS_NOTHUMB))
		tsStyles.addtok(TEXT("nothumb"));
	if (dcx_testflag(Styles, TBS_REVERSED))
		tsStyles.addtok(TEXT("reversed"));
	if (dcx_testflag(Styles, TBS_DOWNISLEFT))
		tsStyles.addtok(TEXT("downisleft"));
	if (dcx_testflag(Styles, TBS_TOOLTIPS))
		tsStyles.addtok(TEXT("tooltips"));
	if (dcx_testflag(Styles, TBS_TRANSPARENTBKGND))
		tsStyles.addtok(TEXT("transparentbkg"));
	if (dcx_testflag(Styles, TBS_NOTICKS))
		tsStyles.addtok(TEXT("noticks"));

	return tsStyles;
}

void DcxTrackBar::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());
}

TiXmlElement * DcxTrackBar::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}
