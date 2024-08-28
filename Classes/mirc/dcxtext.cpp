/*!
 * \file dcxtext.cpp
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
#include "Classes/mirc/dcxtext.h"
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

DcxText::DcxText(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_TEXTCLASS,
		ws.m_Styles | WindowStyle::Child,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	// remove all borders
	removeStyle(WindowStyle::Border | WS_DLGFRAME);
	removeExStyle(WindowExStyle::ClientEdge | WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE);

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	setTextColor(GetSysColor(COLOR_WINDOWTEXT));

	setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);

	if (styles.istok(TEXT("tooltips")))
	{
		if (!IsWindow(p_Dialog->getToolTipHWND()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		setToolTipHWND(p_Dialog->getToolTipHWND());
		AddToolTipToolInfo(getToolTipHWND(), m_Hwnd);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

DcxText::~DcxText() noexcept
{
}

dcxWindowStyles DcxText::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	ws.m_Styles |= SS_NOTIFY;
	this->m_uiStyle = DT_LEFT;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"nowrap"_hash:
			m_uiStyle |= DT_SINGLELINE;
			break;
		case L"center"_hash:
			m_uiStyle |= DT_CENTER;
			break;
		case L"right"_hash:
			m_uiStyle |= DT_RIGHT;
			break;
		case L"noprefix"_hash:
			m_uiStyle |= DT_NOPREFIX;
			break;
		case L"endellipsis"_hash:
			m_uiStyle |= DT_END_ELLIPSIS;
			break;
		case L"pathellipsis"_hash:
			m_uiStyle |= DT_PATH_ELLIPSIS;
			break;
		case L"doublebuffer"_hash:
			m_bDoubleBuffer = true;
			break;
		default:
			break;
		}
	}

	if (!dcx_testflag(this->m_uiStyle, DT_SINGLELINE))
		this->m_uiStyle |= DT_WORDBREAK;

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
TString DcxText::parseInfoRequest(const TString& input) const
{
	return TString();
}
void DcxText::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	// [NAME] [ID] [PROP]
	if (input.gettok(3) == TEXT("text"))
	{
		GetWindowText(m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxText::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
	{
		this->m_tsText.clear();	// = TEXT("");
		SetWindowText(m_Hwnd, TEXT(""));
	}

	// xdid -a [NAME] [ID] [SPACE 0|1] [TEXT]
	if (flags[TEXT('a')])
	{
		if (numtok < 3)
			throw DcxExceptions::dcxInvalidArguments();

		if (input.getnexttok().to_int() == 1)	// tok 4
			this->m_tsText += TEXT(' ');

		this->m_tsText += input.getlasttoks();	// tok 5, -1
		SetWindowText(m_Hwnd, this->m_tsText.to_chr());

		// redraw if transparent
		if (this->isExStyle(WindowExStyle::Transparent))
		{
			this->InvalidateParentRect(m_Hwnd);
			this->redrawWindow();
		}
	}
	// This is to avoid invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
	{
	}
	//xdid -t [NAME] [ID] [SWITCH] [TEXT]
	else if (flags[TEXT('t')])
	{
		this->m_tsText = input.getlasttoks();	// tok 4, -1
		SetWindowText(m_Hwnd, this->m_tsText.to_chr());

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
LRESULT DcxText::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept
{
	return 0L;
}

LRESULT DcxText::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_ERASEBKGND:
	{
		if (this->isExStyle(WindowExStyle::Transparent))
		{
			bParsed = TRUE;
			return TRUE;
		}
		break;
	}

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

	case WM_ENABLE:
	{ // fixes bug with redraw when text control is enabled/disabled & formatted text is being used.
		bParsed = TRUE;
		InvalidateRect(m_Hwnd, nullptr, FALSE);
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

void DcxText::DrawClientArea(HDC hdc)
{
	if (!hdc)
		return;

	RECT r{};

	if (!GetClientRect(m_Hwnd, &r))
		return;

	// Setup alpha blend if any.
	auto ai = SetupAlphaBlend(&hdc, m_bDoubleBuffer);
	Auto(FinishAlphaBlend(ai));

	if (!hdc)
		return;

	//DcxControl::DrawCtrlBackground(hdc, this, &r);
	//
	//HFONT oldFont = nullptr;
	//COLORREF oldClr = CLR_INVALID;
	//COLORREF oldBkgClr = CLR_INVALID;
	//
	//// check if font is valid & set it.
	//if (const auto hFont = this->getControlFont(); hFont)
	//	oldFont = Dcx::dcxSelectObject<HFONT>(hdc, hFont);
	//
	//// check if control is enabled.
	//if (IsWindowEnabled(m_Hwnd))
	//{
	//	if (const auto clr = this->getTextColor(); clr != CLR_INVALID)
	//		oldClr = SetTextColor(hdc, clr);
	//	if (const auto clr = this->getBackColor(); clr != CLR_INVALID)
	//		oldBkgClr = SetBkColor(hdc, clr);
	//}
	//else { // disabled controls colouring
	//	oldClr = SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
	//	oldBkgClr = SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
	//}
	//
	//const TString wtext(TGetWindowText(m_Hwnd));
	//this->ctrlDrawText(hdc, wtext, &r, this->m_uiStyle);
	//
	//if (oldBkgClr != CLR_INVALID)
	//	SetBkColor(hdc, oldBkgClr);
	//if (oldClr != CLR_INVALID)
	//	SetTextColor(hdc, oldClr);
	//if (oldFont)
	//	Dcx::dcxSelectObject<HFONT>(hdc, oldFont);

	const auto saved = SaveDC(hdc);
	Auto(RestoreDC(hdc, saved));

	DcxControl::DrawCtrlBackground(hdc, this, &r);

	// check if font is valid & set it.
	if (const auto hFont = this->getControlFont(); hFont)
		Dcx::dcxSelectObject<HFONT>(hdc, hFont);

	// check if control is enabled.
	if (IsWindowEnabled(m_Hwnd))
	{
		if (const auto clr = this->getTextColor(); clr != CLR_INVALID)
			SetTextColor(hdc, clr);
		if (const auto clr = this->getBackColor(); clr != CLR_INVALID)
			SetBkColor(hdc, clr);
	}
	else { // disabled controls colouring
		SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
	}

	const TString wtext(TGetWindowText(m_Hwnd));
	this->ctrlDrawText(hdc, wtext, &r, this->m_uiStyle);
}

const TString DcxText::getStyles(void) const
{
	auto tsStyles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, DT_SINGLELINE))
		tsStyles.addtok(TEXT("nowrap"));
	if (dcx_testflag(Styles, DT_CENTER))
		tsStyles.addtok(TEXT("center"));
	if (dcx_testflag(Styles, DT_RIGHT))
		tsStyles.addtok(TEXT("right"));
	if (dcx_testflag(Styles, DT_NOPREFIX))
		tsStyles.addtok(TEXT("noprefix"));
	if (dcx_testflag(Styles, DT_END_ELLIPSIS))
		tsStyles.addtok(TEXT("endellipsis"));
	if (dcx_testflag(Styles, DT_PATH_ELLIPSIS))
		tsStyles.addtok(TEXT("pathellipsis"));
	if (m_bDoubleBuffer)
		tsStyles.addtok(TEXT("doublebuffer"));

	return tsStyles;
}

void DcxText::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	this->m_tsText = xThis->GetText();
	SetWindowText(m_Hwnd, this->m_tsText.to_chr());
}

void DcxText::toXml(TiXmlElement* const xml) const
{
	if (!xml)
		return;

	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());

	xml->LinkEndChild(new TiXmlText(this->m_tsText.c_str()));
}

TiXmlElement* DcxText::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

LRESULT DcxText::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
