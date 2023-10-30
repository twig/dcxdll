/*!
 * \file dcxline.cpp
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
#include "Classes/mirc/dcxline.h"
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

DcxLine::DcxLine(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WindowExStyle::Transparent,
		DCX_LINECLASS,
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

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxLine::~DcxLine() noexcept
{
}

const TString DcxLine::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);
	if (this->m_bVertical)
		styles.addtok(TEXT("vertical"));
	if (this->m_bSimple)
		styles.addtok(TEXT("simple"));
	if (dcx_testflag(Styles, SS_LEFTNOWORDWRAP))
		styles.addtok(TEXT("nowrap"));
	if (dcx_testflag(Styles, SS_CENTER))
		styles.addtok(TEXT("center"));
	if (dcx_testflag(Styles, SS_RIGHT))
		styles.addtok(TEXT("right"));
	if (dcx_testflag(Styles, SS_NOPREFIX))
		styles.addtok(TEXT("noprefix"));
	if (dcx_testflag(Styles, SS_ENDELLIPSIS))
		styles.addtok(TEXT("endellipsis"));
	if (dcx_testflag(Styles, SS_PATHELLIPSIS))
		styles.addtok(TEXT("pathellipsis"));
	return styles;

}

void DcxLine::toXml(TiXmlElement* const xml) const
{
	if (!xml)
		return;

	__super::toXml(xml);

	TString styles(getStyles());
	styles.remtok(TEXT("transparent"), 1); // line always has transparent style (why?)
	if (!styles.empty())
		xml->SetAttribute("styles", styles.c_str());
	else
		xml->RemoveAttribute("styles");
	if (!this->m_sText.empty())
		xml->SetAttribute("caption", this->m_sText.c_str());
}

TiXmlElement* DcxLine::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxLine::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	this->m_sText = queryAttribute(xThis, "caption");
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxLine::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"simple"_hash:
			m_bSimple = true;
			break;
		case L"vertical"_hash:
			m_bVertical = true;
			break;
		case L"nowrap"_hash:
			ws.m_Styles |= SS_LEFTNOWORDWRAP;
			break;
		case L"center"_hash:
			ws.m_Styles |= SS_CENTER;
			break;
		case L"right"_hash:
			ws.m_Styles |= SS_RIGHT;
			break;
		case L"noprefix"_hash:
			ws.m_Styles |= SS_NOPREFIX;
			break;
		case L"endellipsis"_hash:
			ws.m_Styles |= SS_ENDELLIPSIS;
			break;
		case L"pathellipsis"_hash:
			ws.m_Styles |= SS_PATHELLIPSIS;
			break;
		default:
			break;
		}
	}

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

TString DcxLine::parseInfoRequest(const TString& input) const
{
	return TString();
}

void DcxLine::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	if (input.gettok(3) == TEXT("text"))
		szReturnValue = m_sText.to_chr();
	else
		parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxLine::parseCommandRequest(const TString& input)
{
	//xdid -t [NAME] [ID] [SWITCH] [TEXT]
	if (const XSwitchFlags flags(input.getfirsttok(3)); flags[TEXT('t')])
	{
		this->m_sText = input.getlasttoks().trim();	// tok 4, -1

		// redraw if transparent
		if (this->isExStyle(WindowExStyle::Transparent))
		{
			this->InvalidateParentRect(m_Hwnd);

			this->redrawWindow();
		}
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxLine::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept
{
	return 0L;
}

LRESULT DcxLine::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_ERASEBKGND:
	{
		//this->DrawParentsBackground((HDC) wParam);
		bParsed = TRUE;
		return TRUE;
	}
	break;
	case WM_PRINTCLIENT:
	{
		this->DrawClientArea(reinterpret_cast<HDC>(wParam));
		bParsed = TRUE;
	}
	break;
	case WM_PAINT:
	{
		bParsed = TRUE;
		PAINTSTRUCT ps{};

		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		this->DrawClientArea(hdc);
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

void DcxLine::DrawClientArea(HDC hdc)
{
	RECT rcClient{};

	// get controls client area
	if (!GetClientRect(m_Hwnd, &rcClient))
		return;

	// Setup alpha blend if any.
	auto ai = SetupAlphaBlend(&hdc);
	Auto(FinishAlphaBlend(ai));

	// fill background.
	//DcxControl::DrawCtrlBackground(hdc,this,&rcClient);

	//res = CallWindowProc( this->m_DefaultWindowProc, m_Hwnd, uMsg, (WPARAM) hdc, lParam );

	if (this->m_bSimple)
	{
		// make sure colours are set.
		COLORREF clr = getBackColor();
		if (clr == CLR_INVALID)
			clr = GetSysColor(COLOR_BTNFACE);

		Dcx::FillRectColour(hdc, &rcClient, clr);
	}

	RECT rcLine = rcClient, rcText = rcClient;

	// draw text if any.
	if (!m_sText.empty())
	{
		HFONT oldhFont{ nullptr };
		if (const auto f = getControlFont(); f)
			oldhFont = Dcx::dcxSelectObject<HFONT>(hdc, f);

		if (const auto clr = getTextColor(); clr != CLR_INVALID)
			SetTextColor(hdc, clr);
		else
			SetTextColor(hdc, GetSysColor(IsWindowEnabled(m_Hwnd) ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));

		UINT style{};
		if (this->isStyle(WindowStyle::SS_EndEllipsis))
			style |= DT_END_ELLIPSIS;
		if (this->isStyle(WindowStyle::SS_PathEllipsis))
			style |= DT_PATH_ELLIPSIS;
		if (this->isStyle(WindowStyle::SS_NoPrefix))
			style |= DT_NOPREFIX;
		if (this->isStyle(WindowStyle::SS_LeftNoWordWrap))
			style |= DT_SINGLELINE;

		if (this->m_bVertical)
		{
			// new working ver that does the same as the orig but using the current font.
			SIZE sz{};
			const auto oMode = SetBkMode(hdc, TRANSPARENT);
			Auto(SetBkMode(hdc, oMode));

			GetTextExtentPoint32(hdc, this->m_sText.to_chr(), gsl::narrow_cast<int>(this->m_sText.len()), &sz);

			rcText.bottom = rcText.top + sz.cx;
			rcText.right = rcText.left + sz.cy;

			if (this->isStyle(WindowStyle::SS_Center))
				OffsetRect(&rcText, ((rcClient.right - rcClient.left) / 2) - ((rcText.right - rcText.left) / 2), ((rcClient.bottom - rcClient.top) / 2) - ((rcText.bottom - rcText.top) / 2));
			else if (this->isStyle(WindowStyle::SS_Right))
				OffsetRect(&rcText, ((rcClient.right - rcClient.left) / 2) - ((rcText.right - rcText.left) / 2), rcClient.bottom - (rcText.bottom - rcText.top));

			DrawRotatedText(this->m_sText, &rcText, hdc, 90, true, 90);

			// test ver that uses a diff routine entierly to draw vertical text
			//int oMode = SetBkMode(hdc, TRANSPARENT);
			//UINT dvStyle = DV_BOTTOM|DV_HCENTER;
			//DrawVertText(hdc, this->m_sText.to_chr(), this->m_sText.len(), &rcText, dvStyle | DV_CALCRECT);
			//if (this->isStyle(SS_CENTER))
			//	OffsetRect(&rcText,((rcClient.right - rcClient.left)/2) - ((rcText.right - rcText.left)/2),((rcClient.bottom - rcClient.top)/2) - ((rcText.bottom - rcText.top)/2));
			//else if (this->isStyle(SS_RIGHT))
			//	OffsetRect(&rcText,((rcClient.right - rcClient.left)/2) - ((rcText.right - rcText.left)/2),rcClient.bottom - (rcText.bottom - rcText.top));
			//DrawVertText(hdc, this->m_sText.to_chr(), this->m_sText.len(), &rcText, dvStyle);
			//SetBkMode(hdc, oMode);
		}
		else {
			style |= DT_LEFT | DT_VCENTER;

			this->calcTextRect(hdc, this->m_sText, &rcText, style);
			if (this->isStyle(WindowStyle::SS_Center))
				OffsetRect(&rcText, ((rcClient.right - rcClient.left) / 2) - ((rcText.right - rcText.left) / 2), 0);
			else if (this->isStyle(WindowStyle::SS_Right))
				OffsetRect(&rcText, rcClient.right - (rcText.right - rcText.left), 0);

			// draw the text
			this->ctrlDrawText(hdc, this->m_sText, &rcText, style);
		}
		if (oldhFont)
			Dcx::dcxSelectObject<HFONT>(hdc, oldhFont);

		ExcludeClipRect(hdc, rcText.left, rcText.top, rcText.right, rcText.bottom);
	}

	if (this->m_bVertical)
	{
		//rcLine.left = rcLine.left + ((rcLine.right - rcLine.left) / 2);
		rcLine.left = (rcLine.right / 2);
		//rcLine.left = rcLine.left + std::max(1, gsl::narrow_cast<int>((rcLine.right - rcLine.left) / 2));
		DrawEdge(hdc, &rcLine, EDGE_ETCHED, BF_LEFT);
	}
	else {
		// max() needed to fix 1px high lines not being shown
		rcLine.bottom = std::max(1, gsl::narrow_cast<int>(rcLine.bottom / 2));
		DrawEdge(hdc, &rcLine, EDGE_ETCHED, BF_BOTTOM);
	}
}

LRESULT DcxLine::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
