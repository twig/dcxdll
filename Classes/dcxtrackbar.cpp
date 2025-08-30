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
DcxTrackBar::DcxTrackBar(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog, DcxControlTypes::TRACKBAR)
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

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		DcxUXModule::dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	// Keep track of the tooltip
	if (dcx_testflag(ws.m_Styles, TBS_TOOLTIPS))
	{
		if (const auto tooltip = to_hwnd(SendMessage(m_Hwnd, TBM_GETTOOLTIPS, 0, 0)); tooltip)
			this->setToolTipHWND(tooltip);
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
	for (auto& x : m_hbmp)
		x.reset();
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
		static_assert(CheckFreeCommand(TEXT('c')), "Command in use!");

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto lPosition = input.getnexttokas<long>();	// tok 4
		this->setTic(lPosition);
	}
	// xdid -g [NAME] [ID] [SWITCH] [+FLAGS] [FILE]
	else if (flags[TEXT('g')])
	{
		static_assert(CheckFreeCommand(TEXT('g')), "Command in use!");

		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tflags = parseImageFlags(input.getnexttok());	// tok 4
		auto filename(input.getlasttoks().trim());				// tok 5, -1

		// background
		if (dcx_testflag(tflags, TBCS_BACK))
		{
			gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_BACK)).m_hBitmap = dcxLoadBitmap(gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_BACK)).m_hBitmap, filename);
			gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_BACK)).m_tsFilename = filename;
		}
		// thumb
		if (dcx_testflag(tflags, TBCS_THUMB))
		{
			gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMB)).m_hBitmap = dcxLoadBitmap(gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMB)).m_hBitmap, filename);
			gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMB)).m_tsFilename = filename;
		}
		// thumb hover
		if (dcx_testflag(tflags, TBCS_THUMBDRAG))
		{
			gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMBDRAG)).m_hBitmap = dcxLoadBitmap(gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMBDRAG)).m_hBitmap, filename);
			gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMBDRAG)).m_tsFilename = filename;
		}
		// channel
		if (dcx_testflag(tflags, TBCS_CHANNEL))
		{
			gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_CHANNEL)).m_hBitmap = dcxLoadBitmap(gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_CHANNEL)).m_hBitmap, filename);
			gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_CHANNEL)).m_tsFilename = filename;
		}

		// these dont seem to work so dont bother calling it
		//this->redrawWindow();
		//InvalidateRect(m_Hwnd, nullptr, TRUE);
	}
	// xdid -j [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags[TEXT('j')])
	{
		static_assert(CheckFreeCommand(TEXT('j')), "Command in use!");

		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iMin = input.getnexttokas<long>();	// tok 4
		const auto iMax = input.getnexttokas<long>();	// tok 5
		this->setSel(iMin, iMax);
	}
	// xdid -l [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('l')])
	{
		static_assert(CheckFreeCommand(TEXT('l')), "Command in use!");

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto lLineSize = input.getnexttokas<long>();	// tok 4
		this->setLineSize(lLineSize);
	}
	// xdid -m [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('m')])
	{
		static_assert(CheckFreeCommand(TEXT('m')), "Command in use!");

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto lPageSize = input.getnexttokas<long>();	// tok 4
		this->setPageSize(lPageSize);
	}
	// xdid -n [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('n')])
	{
		static_assert(CheckFreeCommand(TEXT('n')), "Command in use!");

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iTicFreq = input.getnexttokas<long>();	// tok 4
		this->setTicFreq(iTicFreq);
	}
	// xdid -q [NAME] [ID] [SWITCH]
	else if (flags[TEXT('q')])
	{
		static_assert(CheckFreeCommand(TEXT('q')), "Command in use!");

		this->clearTics();
	}
	// xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags[TEXT('r')])
	{
		static_assert(CheckFreeCommand(TEXT('r')), "Command in use!");

		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto lMinRange = input.getnexttokas<long>();	// tok 4
		const auto lMaxRange = input.getnexttokas<long>();	// tok 5

		this->setRangeMin(lMinRange);
		this->setRangeMax(lMaxRange);
	}
	// xdid -o [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('o')])
	{
		static_assert(CheckFreeCommand(TEXT('o')), "Command in use!");

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		m_colTransparent = input.getnexttokas<COLORREF>();	// tok 4
		this->redrawWindow();
	}
	// xdid -t [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('t')])
	{
		static_assert(CheckFreeCommand(TEXT('t')), "Command in use!");

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

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
		static_assert(CheckFreeCommand(TEXT('u')), "Command in use!");

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto lLength = input.getnexttokas<UINT>();	// tok 4

		this->setThumbLength(lLength);
	}
	// xdid -v [NAME] [ID] [SWITCH] [VALUE]
	else if (flags[TEXT('v')])
	{
		static_assert(CheckFreeCommand(TEXT('v')), "Command in use!");

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto lPosition = input.getnexttokas<long>();	// tok 4

		this->setPos(lPosition);
	}
	// xdid -B [NAME] [ID] [SWITCH] [BUDDYID] [LEFT/RIGHT]
	else if (flags[TEXT('B')])
	{
		static_assert(CheckFreeCommand(TEXT('B')), "Command in use!");

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsID(input.getnexttok());
		const auto tsSide(input.getnexttok());

		const auto ID = getParentDialog()->NameToID(tsID);

		const auto* const p_Control = getParentDialog()->getControlByID(ID);	// tok 4

		if (!p_Control)
			throw Dcx::dcxException("Unable to get control");

		// Text notifications
		if (const auto & cType(p_Control->getControlType()); cType == DcxControlTypes::TEXT || cType == DcxControlTypes::EDIT)
			setBuddy(p_Control->getHwnd(), (tsSide == TEXT("left")));
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

HWND DcxTrackBar::setBuddy(const HWND mHwnd, bool bLeft) noexcept
{
	return to_hwnd(SendMessage(m_Hwnd, TBM_SETBUDDY, bLeft, reinterpret_cast<LPARAM>(mHwnd)));
}

HWND DcxTrackBar::getBuddy(bool bLeft) const noexcept
{
	return to_hwnd(SendMessage(m_Hwnd, TBM_GETBUDDY, bLeft, 0));
}

/*!
* \brief blah
*
* blah
*/

void DcxTrackBar::setRangeMin(const LONG iLowLim) noexcept
{
	SendMessage(m_Hwnd, TBM_SETRANGEMIN, 1U, gsl::narrow_cast<LPARAM>(iLowLim));
}

/*!
* \brief blah
*
* blah
*/

int DcxTrackBar::getRangeMin() const noexcept
{
	return gsl::narrow_cast<int>(SendMessage(m_Hwnd, TBM_GETRANGEMIN, 0U, 0));
}

/*!
* \brief blah
*
* blah
*/

void DcxTrackBar::setRangeMax(const LONG iHighLim) noexcept
{
	SendMessage(m_Hwnd, TBM_SETRANGEMAX, 1U, gsl::narrow_cast<LPARAM>(iHighLim));
}

/*!
* \brief blah
*
* blah
*/

int DcxTrackBar::getRangeMax() const noexcept
{
	return gsl::narrow_cast<int>(SendMessage(m_Hwnd, TBM_GETRANGEMAX, 0U, 0));
}

/*!
* \brief blah
*
* blah
*/

void DcxTrackBar::setRange(const LONG iLowLim, const LONG iHighLim) noexcept
{
	SendMessage(m_Hwnd, TBM_SETRANGE, 1U, Dcx::dcxMAKELPARAM(iLowLim, iHighLim));
}

/*!
* \brief blah
*
* blah
*/

void DcxTrackBar::setPos(const LONG lPosition) noexcept
{
	SendMessage(m_Hwnd, TBM_SETPOS, 1U, gsl::narrow_cast<LPARAM>(lPosition));
}

/*!
* \brief blah
*
* blah
*/

int DcxTrackBar::getPos() const noexcept
{
	return gsl::narrow_cast<int>(SendMessage(m_Hwnd, TBM_GETPOS, 0U, 0));
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
	m_iTickFreq = std::max(wFreq, 1L);
}

/*!
* \brief blah
*
* blah
*/

void DcxTrackBar::clearTics() noexcept
{
	SendMessage(m_Hwnd, TBM_CLEARTICS, 1U, 0);
}

/*!
* \brief blah
*
* blah
*/

int DcxTrackBar::setTipSide(const int fLocation) noexcept
{
	return gsl::narrow_cast<int>(SendMessage(m_Hwnd, TBM_SETTIPSIDE, gsl::narrow_cast<WPARAM>(fLocation), 0));
}

int DcxTrackBar::getTipSide() const noexcept
{
	auto notthis = const_cast<DcxTrackBar*>(this);
	const auto oldvalue = notthis->setTipSide(TBTS_LEFT);
	notthis->setTipSide(oldvalue);
	return oldvalue;
}

/*!
* \brief blah
*
* blah
*/

uint32_t DcxTrackBar::setPageSize(const LONG lPageSize) noexcept
{
	return gsl::narrow_cast<uint32_t>(SendMessage(m_Hwnd, TBM_SETPAGESIZE, 0U, gsl::narrow_cast<LPARAM>(lPageSize)));
}

/*!
* \brief blah
*
* blah
*/

uint32_t DcxTrackBar::getPageSize() const noexcept
{
	return gsl::narrow_cast<uint32_t>(SendMessage(m_Hwnd, TBM_GETPAGESIZE, 0U, 0));
}

/*!
* \brief blah
*
* blah
*/

uint32_t DcxTrackBar::setLineSize(const LONG lLineSize) noexcept
{
	return gsl::narrow_cast<uint32_t>(SendMessage(m_Hwnd, TBM_SETLINESIZE, 0U, gsl::narrow_cast<LPARAM>(lLineSize)));
}

/*!
* \brief blah
*
* blah
*/

uint32_t DcxTrackBar::getLineSize() const noexcept
{
	return gsl::narrow_cast<uint32_t>(SendMessage(m_Hwnd, TBM_GETLINESIZE, 0U, 0));
}

/*!
* \brief blah
*
* blah
*/

void DcxTrackBar::setThumbLength(const UINT iLength) noexcept
{
	SendMessage(m_Hwnd, TBM_SETTHUMBLENGTH, gsl::narrow_cast<WPARAM>(iLength), 0);
}

UINT DcxTrackBar::getThumbLength() const noexcept
{
	return gsl::narrow_cast<UINT>(SendMessage(m_Hwnd, TBM_GETTHUMBLENGTH, 0, 0));
}

/*!
* \brief blah
*
* blah
*/

void DcxTrackBar::setSel(const LONG iLowLim, const LONG iHighLim) noexcept
{
	// this works fine, but is limited to a 16bit range
	//SendMessage(m_Hwnd, TBM_SETSEL, 1U, Dcx::dcxMAKELPARAM(iLowLim, iHighLim));

	// this allows a 32bit range.
	SendMessage(m_Hwnd, TBM_SETSELSTART, 0U, iLowLim);
	SendMessage(m_Hwnd, TBM_SETSELEND, 1U, iHighLim);
}

/*!
* \brief blah
*
* blah
*/

uint32_t DcxTrackBar::getSelStart() const noexcept
{
	return gsl::narrow_cast<uint32_t>(SendMessage(m_Hwnd, TBM_GETSELSTART, 0U, 0));
}

/*!
* \brief blah
*
* blah
*/

uint32_t DcxTrackBar::getSelEnd() const noexcept
{
	return gsl::narrow_cast<uint32_t>(SendMessage(m_Hwnd, TBM_GETSELEND, 0U, 0));
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
				(!gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_BACK)).m_hBitmap) &&
				(!gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMB)).m_hBitmap) &&
				(!gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_THUMBDRAG)).m_hBitmap) &&
				(!gsl::at(this->m_hbmp, gsl::narrow_cast<UINT>(TrackBarParts::TBBMP_CHANNEL)).m_hBitmap)
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
			//	SendMessage(this->m_ToolTipHWND, TTM_UPDATETIPTEXT, 0, reinterpret_cast<LPARAM>(&ti));
			//	this->m_bUpdatingTooltip = false;
			//}

			const stString<80> buff;

			evalAliasEx(buff.data(), gsl::narrow_cast<int>(buff.size()), TEXT("showtip,%u,%d"), getUserID(), getPos());

			if (!buff.empty())
			{
				//TOOLINFO ti{};
				//ti.cbSize = sizeof(TOOLINFO);
				//ti.hinst = GetModuleHandle(nullptr);
				//ti.hwnd = m_Hwnd;
				//ti.uId = reinterpret_cast<UINT_PTR>(m_Hwnd);
				//ti.lpszText = buff.data();
				//this->m_bUpdatingTooltip = true;
				//SendMessage(this->getToolTipHWND(), TTM_UPDATETIPTEXT, 0, reinterpret_cast<LPARAM>(&ti));
				//this->m_bUpdatingTooltip = false;

				TOOLINFO ti{};
				ti.cbSize = sizeof(TOOLINFO);
				ti.hinst = GetModuleHandle(nullptr);
				ti.hwnd = m_Hwnd;
				ti.uId = reinterpret_cast<UINT_PTR>(m_Hwnd);
				ti.lpszText = buff.data();
				this->m_bUpdatingTooltip = true;
				Dcx::dcxToolTip_UpdateTipText(this->getToolTipHWND(), &ti);
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
		this->CallDefaultClassProc(uMsg, wParam, lParam);

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
	if (!hdc || !m_Hwnd || !gsl::at(m_hbmp, gsl::narrow_cast<UINT>(iPartId)).m_hBitmap)
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
	if (GetObject(gsl::at(m_hbmp, gsl::narrow_cast<UINT>(iPartId)).m_hBitmap, sizeof(BITMAP), &bmp) == 0)
		return false;

#if DCX_USE_WRAPPERS
	Dcx::dcxHDCBitmapResource hdcbmp(hdc, gsl::at(m_hbmp, gsl::narrow_cast<UINT>(iPartId)).m_hBitmap);
#else
	auto hdcbmp = CreateCompatibleDC(hdc);

	if (!hdcbmp)
		return false;
	Auto(DeleteDC(hdcbmp));

	auto oldBM = Dcx::dcxSelectObject(hdcbmp, gsl::at(m_hbmp, gsl::narrow_cast<UINT>(iPartId)).m_hBitmap);
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

	// pos min max linesize pagesize tickfreq

	xml->SetAttribute("pos", this->getPos());
	xml->SetAttribute("min", this->getRangeMin());
	xml->SetAttribute("max", this->getRangeMax());
	xml->SetAttribute("linesize", this->getLineSize());
	xml->SetAttribute("pagesize", this->getPageSize());
	xml->SetAttribute("tickfreq", this->getTickFreq());
	if (m_colTransparent != CLR_INVALID)
		setColourAttribute(xml, "transparentcolour", m_colTransparent);
	xml->SetAttribute("tipside", this->getTipSide());
	xml->SetAttribute("thumblength", this->getThumbLength());
	if (auto hLeft = this->getBuddy(true); hLeft)
	{
		if (auto pthis = Dcx::dcxGetProp<DcxControl*>(hLeft, TEXT("dcx_cthis")); pthis)
		{
			xml->SetAttribute("buddyleft", pthis->getUserID());
		}
	}
	if (auto hRight = this->getBuddy(false); hRight)
	{
		if (auto pthis = Dcx::dcxGetProp<DcxControl*>(hRight, TEXT("dcx_cthis")); pthis)
		{
			xml->SetAttribute("buddyright", pthis->getUserID());
		}
	}

	// images
	{
		TiXmlElement xImages("images");
		if (m_hbmp[0].m_hBitmap)
			xImages.SetAttribute("backsrc", m_hbmp[0].m_tsFilename.c_str());
		if (m_hbmp[1].m_hBitmap)
			xImages.SetAttribute("thumbsrc", m_hbmp[1].m_tsFilename.c_str());
		if (m_hbmp[2].m_hBitmap)
			xImages.SetAttribute("dragsrc", m_hbmp[2].m_tsFilename.c_str());
		if (m_hbmp[3].m_hBitmap)
			xImages.SetAttribute("channelsrc", m_hbmp[3].m_tsFilename.c_str());

		if (m_hbmp[0].m_hBitmap || m_hbmp[1].m_hBitmap || m_hbmp[2].m_hBitmap || m_hbmp[3].m_hBitmap)
			xml->InsertEndChild(xImages);
	}

}

TiXmlElement* DcxTrackBar::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxTrackBar::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	this->setRangeMin(queryIntAttribute(xThis, "min"));
	this->setRangeMax(queryIntAttribute(xThis, "max", 100));	// default 0-100
	this->setLineSize(queryIntAttribute(xThis, "linesize", 1));	// default is one
	this->setPageSize(queryIntAttribute(xThis, "pagesize", this->getPageSize()));
	this->setTicFreq(queryIntAttribute(xThis, "tickfreq", 1));	// default is one
	if (auto clr = queryColourAttribute(xThis, "transparentcolour"); clr != CLR_INVALID)
		this->m_colTransparent = clr;
	this->setTipSide(queryIntAttribute(xThis, "tipside", TBTS_LEFT));
	this->setThumbLength(queryIntAttribute(xThis, "thumblength", 10));	// only affects TBS_FIXEDLENGTH style controls

	{
		const auto pd = this->getParentDialog();
		if (!pd)
			throw DcxExceptions::dcxInvalidCommand();

		if (const TString buddyid(queryAttribute(xThis, "buddyleft")); !buddyid.empty())
		{
			if (auto ctrl = pd->getControlByNamedID(buddyid); ctrl)
			{
				this->setBuddy(ctrl->getHwnd(), true);
			}
		}
		if (const TString buddyid(queryAttribute(xThis, "buddyright")); !buddyid.empty())
		{
			if (auto ctrl = pd->getControlByNamedID(buddyid); ctrl)
			{
				this->setBuddy(ctrl->getHwnd(), false);
			}
		}
	}

	// set pos last so we dont get a range error, & in future may update buddy
	this->setPos(queryIntAttribute(xThis, "pos"));

	if (auto xImages = xThis->FirstChildElement("images"); xImages)
	{
		this->m_hbmp[0].m_tsFilename = queryAttribute(xImages, "backsrc");
		if (!this->m_hbmp[0].m_tsFilename.empty())
			m_hbmp[0].m_hBitmap = dcxLoadBitmap(m_hbmp[0].m_hBitmap, this->m_hbmp[0].m_tsFilename);
		this->m_hbmp[1].m_tsFilename = queryAttribute(xImages, "thumbsrc");
		if (!this->m_hbmp[1].m_tsFilename.empty())
			m_hbmp[1].m_hBitmap = dcxLoadBitmap(m_hbmp[1].m_hBitmap, this->m_hbmp[1].m_tsFilename);
		this->m_hbmp[2].m_tsFilename = queryAttribute(xImages, "dragsrc");
		if (!this->m_hbmp[2].m_tsFilename.empty())
			m_hbmp[2].m_hBitmap = dcxLoadBitmap(m_hbmp[2].m_hBitmap, this->m_hbmp[2].m_tsFilename);
		this->m_hbmp[3].m_tsFilename = queryAttribute(xImages, "channelsrc");
		if (!this->m_hbmp[3].m_tsFilename.empty())
			m_hbmp[3].m_hBitmap = dcxLoadBitmap(m_hbmp[3].m_hBitmap, this->m_hbmp[3].m_tsFilename);
	}

}

LRESULT DcxTrackBar::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
