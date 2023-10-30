/*!
 * \file dcxscroll.cpp
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
#include "Classes/mirc/dcxscroll.h"
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

DcxScroll::DcxScroll(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_SCROLLBARCLASS,
		ws.m_Styles | WindowStyle::Child,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	const SCROLLINFO si{ sizeof(SCROLLINFO), SIF_POS | SIF_RANGE, 0, 100, 0U, 0, 0 };

	SetScrollInfo(m_Hwnd, SB_CTL, &si, TRUE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxScroll::~DcxScroll( ) noexcept
{
}

dcxWindowStyles DcxScroll::parseControlStyles(const TString & tsStyles)
{
	dcxWindowStyles ws;

	if (tsStyles.istok(TEXT("vertical")))
		ws.m_Styles |= SBS_VERT;

	return parseGeneralControlStyles(tsStyles, ws);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

TString DcxScroll::parseInfoRequest(const TString& input) const
{
	return TString();
}

void DcxScroll::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"value"_hash:
	{
		SCROLLINFO si{ sizeof(SCROLLINFO), SIF_POS, 0, 0, 0U, 0, 0 };

		if (!GetScrollInfo(m_Hwnd, SB_CTL, &si))
			throw Dcx::dcxException("Unable to get scroll info");

		_ts_snprintf(szReturnValue, TEXT("%d"), si.nPos);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"range"_hash:
	{
		SCROLLINFO si{ sizeof(SCROLLINFO), SIF_RANGE, 0, 0, 0U, 0, 0 };

		if (!GetScrollInfo(m_Hwnd, SB_CTL, &si))
			throw Dcx::dcxException("Unable to get scroll info");

		_ts_snprintf(szReturnValue, TEXT("%d %d"), si.nMin, si.nMax);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"line"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), this->m_nLine);
		break;
		// [NAME] [ID] [PROP]
	case L"page"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), this->m_nPage);
		break;
	default:
		this->parseGlobalInfoRequest(input, szReturnValue);
		break;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxScroll::parseCommandRequest( const TString & input )
{
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	//xdid -l [NAME] [ID] [SWITCH] [N]
	if ( flags[TEXT('l')] )
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nLine = input.getnexttok().to_int();	// tok 4

		if ( nLine > 0 )
			this->m_nLine = nLine;
	}
	//xdid -m [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('m')] )
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nPage = input.getnexttok().to_int();	// tok 4

		if ( nPage > 0 )
			this->m_nPage = nPage;
	}
	//xdid -r [NAME] [ID] [SWITCH] [L] [R]
	else if ( flags[TEXT('r')] )
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto L = input.getnexttok().to_int();	// tok 4
		const auto R = input.getnexttok().to_int();	// tok 5

		const SCROLLINFO si{ sizeof(SCROLLINFO),SIF_RANGE,L,R,0U,0,0 };

		SetScrollInfo( m_Hwnd, SB_CTL, &si, TRUE );
	}
	//xdid -v [NAME] [ID] [SWITCH] [VALUE]
	else if ( flags[TEXT('v')] )
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto pos = input.getnexttok().to_int();	// tok 4

		const SCROLLINFO si{ sizeof(SCROLLINFO),SIF_POS,0,0,0U,pos,0 };

		SetScrollInfo( m_Hwnd, SB_CTL, &si, TRUE );
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

const TString DcxScroll::getStyles(void) const
{
	auto tsStyles(__super::getStyles());

	if (const auto Styles = dcxGetWindowStyle(m_Hwnd); dcx_testflag(Styles, SBS_VERT))
		tsStyles.addtok(TEXT("vertical"));

	return tsStyles;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxScroll::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed)
{
	//switch (uMsg)
	//{
	//case WM_HSCROLL:
	//{
	//	SCROLLINFO si{};
	//	si.cbSize = sizeof(SCROLLINFO);
	//	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
	//	if (!GetScrollInfo(m_Hwnd, SB_CTL, &si))
	//		break;
	//
	//	switch (LOWORD(wParam))
	//	{
	//	case SB_TOP:
	//	{
	//		si.nPos = si.nMin;
	//		this->execAliasEx(TEXT("top,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
	//
	//	case SB_BOTTOM:
	//	{
	//		si.nPos = si.nMax;
	//		this->execAliasEx(TEXT("bottom,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
	//
	//	//case SB_ENDTRACK:
	//	//  CallAliasEx( p_Dialog, ret, TEXT("%s,%d,%d"), TEXT("sclick"), 
	//	//               this->getUserID( ), p_DcxTrackBar->getPos( ) );
	//	//  break;
	//
	//	case SB_PAGEUP:
	//	{
	//		if ((si.nPos - this->m_nPage) >= si.nMin)
	//			si.nPos -= this->m_nPage;
	//		else
	//			si.nPos = si.nMin;
	//
	//		this->execAliasEx(TEXT("pageup,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
	//
	//	case SB_PAGEDOWN:
	//	{
	//		if (si.nPos + this->m_nPage <= si.nMax)
	//			si.nPos += this->m_nPage;
	//		else
	//			si.nPos = si.nMax;
	//
	//		this->execAliasEx(TEXT("pagedown,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
	//
	//	case SB_LINEUP:
	//	{
	//		if (si.nPos - this->m_nLine >= si.nMin)
	//			si.nPos -= this->m_nLine;
	//		else
	//			si.nPos = si.nMin;
	//
	//		this->execAliasEx(TEXT("lineup,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
	//
	//	case SB_LINEDOWN:
	//	{
	//		if (si.nPos + this->m_nLine <= si.nMax)
	//			si.nPos += this->m_nLine;
	//		else
	//			si.nPos = si.nMax;
	//
	//		this->execAliasEx(TEXT("linedown,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
	//
	//	case SB_THUMBPOSITION:
	//	{
	//		this->execAliasEx(TEXT("trackend,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
	//
	//	case SB_THUMBTRACK:
	//	{
	//		si.nPos = si.nTrackPos;
	//		this->execAliasEx(TEXT("tracking,%u,%d"), getUserID(), si.nTrackPos);
	//		break;
	//	}
	//	}
	//
	//	bParsed = TRUE;
	//	si.fMask = SIF_POS;
	//	SetScrollInfo(m_Hwnd, SB_CTL, &si, TRUE);
	//	break;
	//}
	//
	//case WM_VSCROLL:
	//{
	//	SCROLLINFO si;
	//	si.cbSize = sizeof(SCROLLINFO);
	//	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
	//	if (!GetScrollInfo(m_Hwnd, SB_CTL, &si))
	//		break;
//
	//	switch (LOWORD(wParam))
	//	{
	//	case SB_TOP:
	//	{
	//		si.nPos = si.nMin;
	//		this->execAliasEx(TEXT("top,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
//
	//	case SB_BOTTOM:
	//	{
	//		si.nPos = si.nMax;
	//		this->execAliasEx(TEXT("bottom,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
//
	//	//case SB_ENDTRACK:
	//	//  CallAliasEx( p_Dialog, ret, TEXT("%s,%d,%d"), TEXT("sclick"), 
	//	//               this->getUserID( ), p_DcxTrackBar->getPos( ) );
	//	//  break;
//
	//	case SB_PAGEUP:
	//	{
	//		if (si.nPos - this->m_nPage >= si.nMin)
	//			si.nPos -= this->m_nPage;
	//		else
	//			si.nPos = si.nMin;
//
	//		this->execAliasEx(TEXT("pageup,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
//
	//	case SB_PAGEDOWN:
	//	{
	//		if (si.nPos + this->m_nPage <= si.nMax)
	//			si.nPos += this->m_nPage;
	//		else
	//			si.nPos = si.nMax;
//
	//		this->execAliasEx(TEXT("pagedown,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
//
	//	case SB_LINEUP:
	//	{
	//		if (si.nPos - this->m_nLine >= si.nMin)
	//			si.nPos -= this->m_nLine;
	//		else
	//			si.nPos = si.nMin;
//
	//		this->execAliasEx(TEXT("lineup,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
//
	//	case SB_LINEDOWN:
	//	{
	//		if (si.nPos + this->m_nLine <= si.nMax)
	//			si.nPos += this->m_nLine;
	//		else
	//			si.nPos = si.nMax;
//
	//		this->execAliasEx(TEXT("linedown,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
//
	//	case SB_THUMBPOSITION:
	//	{
	//		this->execAliasEx(TEXT("trackend,%u,%d"), getUserID(), si.nPos);
	//		break;
	//	}
//
	//	case SB_THUMBTRACK:
	//	{
	//		si.nPos = si.nTrackPos;
	//		this->execAliasEx(TEXT("tracking,%u,%d"), getUserID(), si.nTrackPos);
	//		break;
	//	}
	//	}
//
	//	bParsed = TRUE;
	//	si.fMask = SIF_POS;
	//	SetScrollInfo(m_Hwnd, SB_CTL, &si, TRUE);
	//	break;
	//}
	//}

	switch (const auto HandleScroll = [=]() {
		SCROLLINFO si{};
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
		if (!GetScrollInfo(m_Hwnd, SB_CTL, &si))
			return FALSE;

		switch (Dcx::dcxLOWORD(wParam))
		{
		case SB_TOP:
		{
			si.nPos = si.nMin;
			this->execAliasEx(TEXT("top,%u,%d"), getUserID(), si.nPos);
			break;
		}

		case SB_BOTTOM:
		{
			si.nPos = si.nMax;
			this->execAliasEx(TEXT("bottom,%u,%d"), getUserID(), si.nPos);
			break;
		}

		//case SB_ENDTRACK:
		//  CallAliasEx( p_Dialog, ret, TEXT("%s,%d,%d"), TEXT("sclick"), 
		//               this->getUserID( ), p_DcxTrackBar->getPos( ) );
		//  break;

		case SB_PAGEUP:
		{
			if ((si.nPos - this->m_nPage) >= si.nMin)
				si.nPos -= this->m_nPage;
			else
				si.nPos = si.nMin;

			this->execAliasEx(TEXT("pageup,%u,%d"), getUserID(), si.nPos);
			break;
		}

		case SB_PAGEDOWN:
		{
			if (si.nPos + this->m_nPage <= si.nMax)
				si.nPos += this->m_nPage;
			else
				si.nPos = si.nMax;

			this->execAliasEx(TEXT("pagedown,%u,%d"), getUserID(), si.nPos);
			break;
		}

		case SB_LINEUP:
		{
			if (si.nPos - this->m_nLine >= si.nMin)
				si.nPos -= this->m_nLine;
			else
				si.nPos = si.nMin;

			this->execAliasEx(TEXT("lineup,%u,%d"), getUserID(), si.nPos);
			break;
		}

		case SB_LINEDOWN:
		{
			if (si.nPos + this->m_nLine <= si.nMax)
				si.nPos += this->m_nLine;
			else
				si.nPos = si.nMax;

			this->execAliasEx(TEXT("linedown,%u,%d"), getUserID(), si.nPos);
			break;
		}

		case SB_THUMBPOSITION:
		{
			this->execAliasEx(TEXT("trackend,%u,%d"), getUserID(), si.nPos);
			break;
		}

		case SB_THUMBTRACK:
		{
			si.nPos = si.nTrackPos;
			this->execAliasEx(TEXT("tracking,%u,%d"), getUserID(), si.nTrackPos);
			break;
		}
		default:
			break;
		}

		si.fMask = SIF_POS;
		SetScrollInfo(m_Hwnd, SB_CTL, &si, TRUE);
		return TRUE;
	}; uMsg)
	{
	case WM_HSCROLL:
	case WM_VSCROLL:
		bParsed = HandleScroll();
		break;
	default:
		break;
	}

	return 0L;
}

LRESULT DcxScroll::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed)
{
	switch (uMsg)
	{
	case WM_PAINT:
	{
		if (!this->IsAlphaBlend())
			break;

		PAINTSTRUCT ps{};

		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		bParsed = TRUE;

		// Setup alpha blend if any.
		const auto ai = this->SetupAlphaBlend(&hdc);
		Auto(this->FinishAlphaBlend(ai));

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

void DcxScroll::toXml(TiXmlElement *const xml) const
{
	if (!xml)
		return;

	__super::toXml(xml);

	//const TString wtext(TGetWindowText(m_Hwnd));
	//xml->SetAttribute("caption", wtext.c_str());
	xml->SetAttribute("styles", getStyles().c_str());
	xml->SetAttribute("linesize", this->m_nLine);
	xml->SetAttribute("pagesize", this->m_nPage);

	// save range
	// save value
	{
		SCROLLINFO si{ sizeof(SCROLLINFO),SIF_RANGE | SIF_POS,0,0,0U,0,0 };

		GetScrollInfo(m_Hwnd, SB_CTL, &si);

		xml->SetAttribute("value", si.nPos);
		xml->SetAttribute("min", si.nMin);
		xml->SetAttribute("max", si.nMax);
}
}

TiXmlElement * DcxScroll::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

LRESULT DcxScroll::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
