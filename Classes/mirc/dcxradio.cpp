/*!
 * \file dcxradio.cpp
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
#include "Classes/mirc/dcxradio.h"
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

DcxRadio::DcxRadio(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_BUTTONCLASS,
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

	if (p_Dialog->getToolTipHWND())
	{
		if (styles.istok(TEXT("tooltips")))
		{
			setToolTipHWND(p_Dialog->getToolTipHWND());
			if (!IsWindow(getToolTipHWND()))
				throw Dcx::dcxException("Unable to get ToolTips window");

			AddToolTipToolInfo(getToolTipHWND(), m_Hwnd);
		}
	}
	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxRadio::~DcxRadio() noexcept
{
}

const TString DcxRadio::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, BS_RIGHT))
		styles.addtok(TEXT("rjustify"));
	if (dcx_testflag(Styles, BS_CENTER))
		styles.addtok(TEXT("center"));
	if (dcx_testflag(Styles, BS_LEFT))
		styles.addtok(TEXT("ljustify"));
	if (dcx_testflag(Styles, BS_RIGHTBUTTON))
		styles.addtok(TEXT("right"));
	if (dcx_testflag(Styles, BS_PUSHLIKE))
		styles.addtok(TEXT("pushlike"));

	return styles;

}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxRadio::parseControlStyles(const TString & tsStyles)
{
	dcxWindowStyles ws;

	ws.m_Styles |= BS_AUTORADIOBUTTON;

	for (const auto &tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"rjustify"_hash:
			ws.m_Styles |= BS_RIGHT;
			break;
		case L"center"_hash:
			ws.m_Styles |= BS_CENTER;
			break;
		case L"ljustify"_hash:
			ws.m_Styles |= BS_LEFT;
			break;
		case L"right"_hash:
			ws.m_Styles |= BS_RIGHTBUTTON;
			break;
		case L"pushlike"_hash:
			ws.m_Styles |= BS_PUSHLIKE;
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

TString DcxRadio::parseInfoRequest(const TString& input) const
{
	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP]
	if (prop == TEXT("text"))
		return TGetWindowText(m_Hwnd);
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("state"))
	{
		if (dcx_testflag(Button_GetCheck(m_Hwnd), BST_CHECKED))
			return TEXT("1");

		return TEXT("0");
	}

	return parseGlobalInfoRequest(input);
}

void DcxRadio::parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	//const auto prop(input.getfirsttok(3));

	//// [NAME] [ID] [PROP]
	//if (prop == TEXT("text"))
	//{
	//	GetWindowText(m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	//}
	//// [NAME] [ID] [PROP]
	//else if (prop == TEXT("state"))
	//{
	//	dcx_ConChar(dcx_testflag(Button_GetCheck(m_Hwnd), BST_CHECKED), szReturnValue);
	//}
	//else
	//	this->parseGlobalInfoRequest(input, szReturnValue);

	szReturnValue = parseInfoRequest(input).to_chr();
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxRadio::parseCommandRequest(const TString & input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	//xdid -c [NAME] [ID] [SWITCH]
	if (flags[TEXT('c')])
	{
		Button_SetCheck(m_Hwnd, BST_CHECKED);
	}
	//xdid -t [NAME] [ID] [SWITCH] [TEXT]
	else if (flags[TEXT('t')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		Button_SetText(m_Hwnd, input.getlasttoks().trim().to_chr());	// tok 4, -1
	}
	//xdid -u [NAME] [ID] [SWITCH]
	else if (flags[TEXT('u')])
	{
		Button_SetCheck(m_Hwnd, BST_UNCHECKED);
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxRadio::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) noexcept
{
	return 0L;
}

LRESULT DcxRadio::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed)
{
	switch (uMsg)
	{
	case WM_ERASEBKGND:
	{
		bParsed = TRUE;
		return TRUE;
	}
	break;

	case WM_PRINTCLIENT:
	{
		this->DrawClientArea(reinterpret_cast<HDC>(wParam), uMsg, lParam);
		bParsed = TRUE;
	}
	break;

	case WM_PAINT:
	{
		bParsed = TRUE;
		PAINTSTRUCT ps{};

		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		this->DrawClientArea(hdc, uMsg, lParam);
	}
	break;

	case WM_DESTROY:
	{
		delete this;
		bParsed = TRUE;
	}
	break;

	case WM_LBUTTONUP:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
		{
			const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);
			bParsed = TRUE;

			execAliasEx(TEXT("lbup,%u"), getUserID());
			execAliasEx(TEXT("sclick,%u"), getUserID());
			return lRes;
		}
	}
	break;

	default:
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}

void DcxRadio::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	// Setup alpha blend if any.
	auto ai = SetupAlphaBlend(&hdc);
	Auto(FinishAlphaBlend(ai));

	if (!IsThemed() || !Dcx::UXModule.dcxIsThemeActive())
	{
		// get controls client area
		if (RECT rcClient{}; GetClientRect(m_Hwnd, &rcClient))
		{
			if (const auto clr = getBackColor(); clr != CLR_INVALID)
				SetBkColor(hdc, clr);

			if (const auto clr = getTextColor(); clr != CLR_INVALID)
				SetTextColor(hdc, clr);

			const auto bWasTransp = this->isExStyle(WindowExStyle::Transparent);

			// fill background.
			if (bWasTransp)
			{
				if (!IsAlphaBlend())
					this->DrawParentsBackground(hdc, &rcClient);
			}
			else
				DcxControl::DrawCtrlBackground(hdc, this, &rcClient);

			if (!bWasTransp)
				this->addExStyle(WindowExStyle::Transparent);

			CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);

			if (!bWasTransp)
				this->removeExStyle(WindowExStyle::Transparent);
		}
	}
	else
		CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
}

void DcxRadio::toXml(TiXmlElement *const xml) const
{
	if (!xml || !m_Hwnd)
		return;

	__super::toXml(xml);

	const TString wtext(TGetWindowText(m_Hwnd));
	xml->SetAttribute("caption", wtext.c_str());
	xml->SetAttribute("styles", getStyles().c_str());
	xml->SetAttribute("state", Button_GetCheck(m_Hwnd));
}

TiXmlElement * DcxRadio::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxRadio::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	if (const auto tmp = queryAttribute(xThis, "caption"); !_ts_isEmpty(tmp))
	{
		const TString tsText(tmp);
		SetWindowText(m_Hwnd, tsText.to_chr());
	}
	if (const auto tmp = queryIntAttribute(xThis, "state"); tmp)
		Button_SetCheck(m_Hwnd, tmp);
}

LRESULT DcxRadio::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
