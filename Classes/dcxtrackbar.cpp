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

DcxTrackBar::DcxTrackBar(const UINT ID, DcxDialog* const p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_TRACKBARCLASS,
		ws.m_Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	// Keep track of the tooltip
	if (dcx_testflag(ws.m_Styles, TBS_TOOLTIPS))
	{
		if (const auto tooltip = reinterpret_cast<HWND>(SendMessage(m_Hwnd, TBM_GETTOOLTIPS, NULL, NULL)); tooltip)
			this->m_ToolTipHWND = tooltip;
	}

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxTrackBar::~DcxTrackBar() noexcept
{
	for (const auto& x : m_hbmp)
	{
		if (x)
			DeleteBitmap(x);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxTrackBar::parseControlStyles(const TString& tsStyles)
{
	auto ws = parseGeneralControlStyles(tsStyles);

	ws.m_Styles |= TBS_FIXEDLENGTH;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"autoticks"_hash:
			ws.m_Styles |= TBS_AUTOTICKS;
			break;
		case L"both"_hash:
			ws.m_Styles |= TBS_BOTH;
			break;
		case L"top"_hash:
			ws.m_Styles |= TBS_TOP;		// == TBS_LEFT == 4
			break;
		case L"bottom"_hash:
			ws.m_Styles |= TBS_BOTTOM;	// == TBS_RIGHT == 0, so never set.... should just remove TBS_RIGHT/TOP ?
			break;
		case L"left"_hash:
			ws.m_Styles |= TBS_LEFT;	// == TBS_TOP == 4
			break;
		case L"right"_hash:
			ws.m_Styles |= TBS_RIGHT;	// == TBS_BOTTOM == 0
			break;
		case L"select"_hash:
			ws.m_Styles |= TBS_ENABLESELRANGE;
			break;
		case L"vertical"_hash:
			ws.m_Styles |= TBS_VERT;
			break;
		case L"nothumb"_hash:
			ws.m_Styles |= TBS_NOTHUMB;
			break;
		case L"noticks"_hash:
			ws.m_Styles |= TBS_NOTICKS;
			break;
		case L"reversed"_hash:
			ws.m_Styles |= TBS_REVERSED;
			break;
		case L"downisleft"_hash:
			ws.m_Styles |= TBS_DOWNISLEFT;
			break;
		case L"tooltips"_hash:
			ws.m_Styles |= TBS_TOOLTIPS;
			break;
		case L"transparentbkg"_hash:
			ws.m_Styles |= TBS_TRANSPARENTBKGND;
			break;
		default:
			break;
		}
	}

	return ws;
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxTrackBar::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
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

void DcxTrackBar::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -c [NAME] [ID] [SWITCH] [VALUE]
	if (flags[TEXT('c')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const auto lPosition = input.getnexttok().to_<long>();	// tok 4
		this->setTic(lPosition);
	}
	// xdid -g [NAME] [ID] [SWITCH] [FLAGS] [FILE]
	else if (flags[TEXT('g')])
	{
		if (numtok < 5)
			throw Dcx::dcxException("Insufficient parameters");

		const auto tflags = parseImageFlags(input.getnexttok());	// tok 4
		auto filename(input.getlasttoks().trim());				// tok 5, -1

		// background
		if (dcx_testflag(tflags, TBCS_BACK))
			gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_BACK)) = dcxLoadBitmap(gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_BACK)), filename);
		// thumb
		if (dcx_testflag(tflags, TBCS_THUMB))
			gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMB)) = dcxLoadBitmap(gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMB)), filename);
		// thumb hover
		if (dcx_testflag(tflags, TBCS_THUMBDRAG))
			gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMBDRAG)) = dcxLoadBitmap(gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMBDRAG)), filename);
		// channel
		if (dcx_testflag(tflags, TBCS_CHANNEL))
			gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_CHANNEL)) = dcxLoadBitmap(gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_CHANNEL)), filename);

		// these dont seem to work so dont bother calling it
		//this->redrawWindow();
		//InvalidateRect(m_Hwnd, nullptr, TRUE);
	}
	// xdid -j [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags[TEXT('j')])
	{
		if (numtok < 5)
			throw Dcx::dcxException("Insufficient parameters");

		const auto iMin = input.getnexttok().to_<long>();	// tok 4
		const auto iMax = input.getnexttok().to_<long>();	// tok 5
		this->setSel(iMin, iMax);
	}
	// xdid -l [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('l')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const auto lLineSize = input.getnexttok().to_<long>();	// tok 4
		this->setLineSize(lLineSize);
	}
	// xdid -m [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('m')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const auto lPageSize = input.getnexttok().to_<long>();	// tok 4
		this->setPageSize(lPageSize);
	}
	// xdid -n [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('n')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const auto iTicFreq = input.getnexttok().to_<long>();	// tok 4
		this->setTicFreq(iTicFreq);
	}
	// xdid -q [NAME] [ID] [SWITCH]
	else if (flags[TEXT('q')])
	{
		this->clearTics();
	}
	// xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags[TEXT('r')])
	{
		if (numtok < 5)
			throw Dcx::dcxException("Insufficient parameters");

		const auto lMinRange = input.getnexttok().to_<long>();	// tok 4
		const auto lMaxRange = input.getnexttok().to_<long>();	// tok 5

		this->setRangeMin(lMinRange);
		this->setRangeMax(lMaxRange);
	}
	// xdid -o [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('o')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		m_colTransparent = input.getnexttok().to_<COLORREF>();	// tok 4
		this->redrawWindow();
	}
	// xdid -t [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('t')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		switch (std::hash<TString>{}(input.getnexttok()))
		{
		case TEXT("left"_hash):
			this->setTipSide(TBTS_LEFT);
			break;
		case TEXT("right"_hash):
			this->setTipSide(TBTS_RIGHT);
			break;
		case TEXT("bottom"_hash):
			this->setTipSide(TBTS_BOTTOM);
			break;
		default:
			this->setTipSide(TBTS_TOP);
			break;
		}
	}
	// xdid -u [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('u')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const auto lLength = input.getnexttok().to_<UINT>();	// tok 4

		this->setThumbLength(lLength);
	}
	// xdid -v [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('v')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const auto lPosition = input.getnexttok().to_<long>();	// tok 4

		this->setPos(lPosition);
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setRangeMin(const LONG iLowLim) noexcept
{
	return SendMessage(m_Hwnd, TBM_SETRANGEMIN, 1U, gsl::narrow_cast<LPARAM>(iLowLim));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getRangeMin() const noexcept
{
	return SendMessage(m_Hwnd, TBM_GETRANGEMIN, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setRangeMax(const LONG iHighLim) noexcept
{
	return SendMessage(m_Hwnd, TBM_SETRANGEMAX, 1U, gsl::narrow_cast<LPARAM>(iHighLim));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getRangeMax() const noexcept
{
	return SendMessage(m_Hwnd, TBM_GETRANGEMAX, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setRange(const LONG iLowLim, const LONG iHighLim) noexcept
{
	//return SendMessage(m_Hwnd, TBM_SETRANGE, 1U, MAKELPARAM(iLowLim, iHighLim));
	return SendMessage(m_Hwnd, TBM_SETRANGE, 1U, Dcx::dcxMAKELPARAM(iLowLim, iHighLim));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setPos(const LONG lPosition) noexcept
{
	return SendMessage(m_Hwnd, TBM_SETPOS, 1U, gsl::narrow_cast<LPARAM>(lPosition));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getPos() const noexcept
{
	return SendMessage(m_Hwnd, TBM_GETPOS, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/

void DcxTrackBar::setTic(const LONG lPosition) noexcept
{
	SendMessage(m_Hwnd, TBM_SETTIC, 0U, gsl::narrow_cast<LPARAM>(lPosition));
}

/*!
* \brief blah
*
* blah
*/

void DcxTrackBar::setTicFreq(const LONG wFreq) noexcept
{
	SendMessage(m_Hwnd, TBM_SETTICFREQ, gsl::narrow_cast<WPARAM>(wFreq), 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::clearTics() noexcept
{
	return SendMessage(m_Hwnd, TBM_CLEARTICS, 1U, 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setTipSide(const int fLocation) noexcept
{
	return SendMessage(m_Hwnd, TBM_SETTIPSIDE, gsl::narrow_cast<WPARAM>(fLocation), 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setPageSize(const LONG lPageSize) noexcept
{
	return SendMessage(m_Hwnd, TBM_SETPAGESIZE, 0U, gsl::narrow_cast<LPARAM>(lPageSize));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getPageSize() const noexcept
{
	return SendMessage(m_Hwnd, TBM_GETPAGESIZE, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setLineSize(const LONG lLineSize) noexcept
{
	return SendMessage(m_Hwnd, TBM_SETLINESIZE, 0U, gsl::narrow_cast<LPARAM>(lLineSize));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getLineSize() const noexcept
{
	return SendMessage(m_Hwnd, TBM_GETLINESIZE, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setThumbLength(const UINT iLength) noexcept
{
	return SendMessage(m_Hwnd, TBM_SETTHUMBLENGTH, gsl::narrow_cast<WPARAM>(iLength), 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::setSel(const LONG iLowLim, const LONG iHighLim) noexcept
{
	//return SendMessage(m_Hwnd, TBM_SETSEL, 1U, MAKELPARAM(iLowLim, iHighLim));
	return SendMessage(m_Hwnd, TBM_SETSEL, 1U, Dcx::dcxMAKELPARAM(iLowLim, iHighLim));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getSelStart() const noexcept
{
	return SendMessage(m_Hwnd, TBM_GETSELSTART, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxTrackBar::getSelEnd() const noexcept
{
	return SendMessage(m_Hwnd, TBM_GETSELEND, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxTrackBar::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_VSCROLL:
	case WM_HSCROLL:
	{
		switch (Dcx::dcxLOWORD(wParam))
		{
		case TB_TOP:
			this->execAliasEx(TEXT("top,%u,%d"), getUserID(), this->getPos());
			break;

		case TB_BOTTOM:
			this->execAliasEx(TEXT("bottom,%u,%d"), getUserID(), this->getPos());
			break;

		case TB_PAGEUP:
			this->execAliasEx(TEXT("pageup,%u,%d"), getUserID(), this->getPos());
			break;

		case TB_PAGEDOWN:
			this->execAliasEx(TEXT("pagedown,%u,%d"), getUserID(), this->getPos());
			break;

		case TB_LINEUP:
			this->execAliasEx(TEXT("lineup,%u,%d"), getUserID(), this->getPos());
			break;

		case TB_LINEDOWN:
			this->execAliasEx(TEXT("linedown,%u,%d"), getUserID(), this->getPos());
			break;

		case TB_THUMBPOSITION:
			this->execAliasEx(TEXT("trackend,%u,%d"), getUserID(), this->getPos());
			break;

		case SB_THUMBTRACK:
			this->execAliasEx(TEXT("tracking,%u,%d"), getUserID(), this->getPos());
			break;

		default:
			break;
		}

		break;
	}

	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (!hdr)
			break;

		switch (hdr->code)
		{
		case NM_CUSTOMDRAW:
		{
			// if (no items to draw)
			if (
				(!gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_BACK))) &&
				(!gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMB))) &&
				(!gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMBDRAG))) &&
				(!gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_CHANNEL)))
				)
				break;

			dcxlParam(LPNMCUSTOMDRAW, nmcd);
			if (!nmcd)
				break;

			auto hdc = nmcd->hdc;
			if (!hdc)
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
					DrawTrackBarPart(hdc, TrackBarParts::TBBMP_BACK);

					if (DrawTrackBarPart(hdc, TrackBarParts::TBBMP_CHANNEL, &nmcd->rc))
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
					auto iPartId = TrackBarParts::TBBMP_THUMB;

					// if thumb is selected/focussed, switch brushes
					if (dcx_testflag(nmcd->uItemState, CDIS_SELECTED))
						iPartId = TrackBarParts::TBBMP_THUMBDRAG;

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

		default:
			break;
		} // end SWITCH
	} // end notify
	break;
	default:
		break;
	}
	return 0L;
}

LRESULT DcxTrackBar::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
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

		if (!hdr)
			break;

		switch (hdr->code)
		{
			// Show tooltip
		case TTN_SHOW:
		{
			// Check if its currently being updated.
			if (this->m_bUpdatingTooltip)
				break;

			//TString buff(80U);
			//evalAliasEx(buff.to_chr(), gsl::narrow_cast<int>(buff.capacity()), TEXT("showtip,%u,%d"), getUserID(), getPos());
			//
			//if (!buff.empty())
			//{
			//	TOOLINFO ti{};
			//
			//	ti.cbSize = sizeof(TOOLINFO);
			//	ti.hinst = GetModuleHandle(nullptr);
			//	ti.hwnd = m_Hwnd;
			//	ti.uId = reinterpret_cast<UINT_PTR>(m_Hwnd);
			//	ti.lpszText = buff.to_chr();
			//
			//	this->m_bUpdatingTooltip = true;
			//	SendMessage(this->m_ToolTipHWND, TTM_UPDATETIPTEXT, NULL, reinterpret_cast<LPARAM>(&ti));
			//	this->m_bUpdatingTooltip = false;
			//}

			const stString<80> buff;

			evalAliasEx(buff.data(), gsl::narrow_cast<int>(buff.size()), TEXT("showtip,%u,%d"), getUserID(), getPos());

			if (!buff.empty())
			{
				TOOLINFO ti{};

				ti.cbSize = sizeof(TOOLINFO);
				ti.hinst = GetModuleHandle(nullptr);
				ti.hwnd = m_Hwnd;
				ti.uId = reinterpret_cast<UINT_PTR>(m_Hwnd);
				ti.lpszText = buff.data();

				this->m_bUpdatingTooltip = true;
				SendMessage(this->m_ToolTipHWND, TTM_UPDATETIPTEXT, NULL, reinterpret_cast<LPARAM>(&ti));
				this->m_bUpdatingTooltip = false;
			}
		}
		break; // case TTN_SHOW
		default:
			break;
		}
		break; // case WM_NOTIFY
	}

	case WM_PAINT:
	{
		if (!this->IsAlphaBlend())
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

		return CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
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


UINT DcxTrackBar::parseImageFlags(const TString& flags) noexcept
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

bool DcxTrackBar::DrawTrackBarPart(HDC hdc, const TrackBarParts iPartId, const RECT* const rc)
{
	if (!gsl::at(m_hbmp, gsl::narrow_cast<UINT>(iPartId)))
		return false;

	RECT rect{};
	if (!rc)
	{
		if (!GetClientRect(m_Hwnd, &rect))
			return false;
	}
	else
		CopyRect(&rect, rc);

	BITMAP bmp{};
	if (GetObject(gsl::at(m_hbmp, gsl::narrow_cast<UINT>(iPartId)), sizeof(BITMAP), &bmp) == 0)
		return false;

#if DCX_USE_WRAPPERS
	Dcx::dcxHDCBitmapResource hdcbmp(hdc, gsl::at(m_hbmp, gsl::narrow_cast<UINT>(iPartId)));
#else
	auto hdcbmp = CreateCompatibleDC(hdc);

	if (!hdcbmp)
		return false;
	Auto(DeleteDC(hdcbmp));

	auto oldBM = Dcx::dcxSelectObject(hdcbmp, gsl::at(m_hbmp, gsl::narrow_cast<UINT>(iPartId)));
	Auto(Dcx::dcxSelectObject(hdcbmp, oldBM));
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

void DcxTrackBar::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());
}

TiXmlElement* DcxTrackBar::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

LRESULT DcxTrackBar::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
