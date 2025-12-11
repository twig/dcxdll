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
	: DcxControl(ID, p_Dialog, DcxControlTypes::RADIO)
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
		DcxUXModule::dcxSetWindowTheme(m_Hwnd, L" ", L" ");

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
		case L"vcenter"_hash:
			ws.m_Styles |= BS_VCENTER;
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

	case WM_ENABLE:
	{
		bParsed = TRUE;
		setRedraw(FALSE);
		const auto lRes = this->CallDefaultClassProc(uMsg, wParam, lParam);
		setRedraw(TRUE);
		this->DrawClientArea();
		return lRes;
	}

	case WM_DESTROY:
	{
		this->CallDefaultClassProc(uMsg, wParam, lParam);

		delete this;
		bParsed = TRUE;
	}
	break;

	case WM_LBUTTONUP:
	{
		if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
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

void DcxRadio::DrawClientAreaThemed(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	//CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);

	dcxRadioData rd;

	rd.m_hTheme = DcxUXModule::dcxOpenThemeData(m_Hwnd, VSCLASS_BUTTON);
	//rd.m_hTheme = DcxUXModule::dcxGetWindowTheme(m_Hwnd);
	if (!rd.m_hTheme)
	{
		DrawClientAreaNoTheme(hdc, uMsg, lParam);
		return;
	}
	Auto(DcxUXModule::dcxCloseThemeData(rd.m_hTheme));

	if (!GetClientRect(m_Hwnd, &rd.m_rc))
		return;

	if (IsRectEmpty(&rd.m_rc))
		return;

	rd.m_hdcPaint = hdc;

	rd.m_WinStyle = dcxGetWindowStyle(m_Hwnd);
	rd.m_iThemeState = RADIOBUTTONSTATES::RBS_UNCHECKEDNORMAL;
	rd.m_ButtonState = Button_GetState(m_Hwnd);
	rd.m_bHot = dcx_testflag(rd.m_ButtonState, BST_HOT);
	rd.m_bPressed = dcx_testflag(rd.m_ButtonState, BST_PUSHED);
	rd.m_bChecked = dcx_testflag(rd.m_ButtonState, BST_CHECKED); /*(Button_GetCheck(m_Hwnd) == BST_CHECKED);*/ /*(dcx_testflag(rd.m_ButtonState, BST_CHECKED) && !dcx_testflag(rd.m_ButtonState, BST_INDETERMINATE));*/
	rd.m_bFocus = (GetFocus() == m_Hwnd);
	rd.m_bRightJustify = dcx_testflag(rd.m_WinStyle, BS_RIGHT); //BST_DROPDOWNPUSHED = 0x400 BST_HOT = 0x200
	rd.m_bHCenter = dcx_testflag(rd.m_WinStyle, BS_CENTER);
	rd.m_bVCenter = dcx_testflag(rd.m_WinStyle, BS_VCENTER);
	rd.m_bRightButton = dcx_testflag(rd.m_WinStyle, BS_RIGHTBUTTON);

	rd.m_tsText = TGetWindowText(m_Hwnd);

	if (!IsWindowEnabled(m_Hwnd))
	{
		if (rd.m_bChecked)
			rd.m_iThemeState = RADIOBUTTONSTATES::RBS_CHECKEDDISABLED;
		else
			rd.m_iThemeState = RADIOBUTTONSTATES::RBS_UNCHECKEDDISABLED;
	}
	else {
		if (rd.m_bChecked)
		{
			if (rd.m_bPressed)
				rd.m_iThemeState = RADIOBUTTONSTATES::RBS_CHECKEDPRESSED;
			else if (rd.m_bHot)
				rd.m_iThemeState = RADIOBUTTONSTATES::RBS_CHECKEDHOT;
			else
				rd.m_iThemeState = RADIOBUTTONSTATES::RBS_CHECKEDNORMAL;
		}
		else {
			if (rd.m_bPressed)
				rd.m_iThemeState = RADIOBUTTONSTATES::RBS_UNCHECKEDPRESSED;
			else if (rd.m_bHot)
				rd.m_iThemeState = RADIOBUTTONSTATES::RBS_UNCHECKEDHOT;
		}
	}
	//if (DcxUXModule::dcxIsThemeBackgroundPartiallyTransparent(rd.m_hTheme, BUTTONPARTS::BP_RADIOBUTTON, rd.m_iThemeState))
	//	DcxUXModule::dcxDrawThemeParentBackground(m_Hwnd, rd.m_hdcPaint, &rd.m_rc);

	if (this->isExStyle(WindowExStyle::Transparent))
		this->DrawParentsBackground(rd.m_hdcPaint, &rd.m_rc);
	else
		DcxControl::DrawCtrlBackground(rd.m_hdcPaint, this, &rd.m_rc, rd.m_hTheme, BUTTONPARTS::BP_RADIOBUTTON, rd.m_iThemeState);

	const auto hFontOld = Dcx::dcxSelectObject<HFONT>(rd.m_hdcPaint, getControlFont());
	Auto(Dcx::dcxSelectObject<HFONT>(rd.m_hdcPaint, hFontOld));

	rd.m_szRadioSize = DcxUXModule::dcxGetCheckBoxSize(rd.m_hTheme, m_Hwnd, rd.m_hdcPaint, &rd.m_rc);

	// make sure radio & text are in correct order.
	if (rd.m_bRightButton)
	{
		rd.m_rcRadio.left = rd.m_rc.right - rd.m_szRadioSize.cx;
		rd.m_rcRadio.right = rd.m_rc.right;
		rd.m_rcRadio.bottom = rd.m_szRadioSize.cy;

		rd.m_rcText = rd.m_rc;
		rd.m_rcText.right = rd.m_rcRadio.left - 1;
	}
	else {
		rd.m_rcRadio.right = rd.m_szRadioSize.cx;
		rd.m_rcRadio.bottom = rd.m_szRadioSize.cy;

		rd.m_rcText = rd.m_rc;
		rd.m_rcText.left = rd.m_rcRadio.right + 1;
	}

	// set vert positioning. (for checkbox, text, & focus)
	if (rd.m_bVCenter)
	{
		// center checkbox vertically in control
		OffsetRect(&rd.m_rcRadio, 0, (((rd.m_rc.bottom - rd.m_rc.top) / 2) - (rd.m_szRadioSize.cy / 2)));

		// center text vertically in control
		//cd.m_textFlags |= DT_VCENTER;	// Ook: doesnt seem to center correctly, needs looked at.

		RECT rcTmp{};
		calcTextRect(rd.m_hdcPaint, rd.m_tsText, std::addressof(rcTmp), rd.m_textFlags);

		rd.m_rcText.top = (((rd.m_rc.bottom - rd.m_rc.top) / 2) - ((rcTmp.bottom - rcTmp.top) / 2));
		rd.m_rcText.bottom = rd.m_rcText.top + (rcTmp.bottom - rcTmp.top);
	}

	// apply the set offsets
	//OffsetRect(&cd.m_rcCheck, 0, m_CheckMargins.cyTopHeight);
	//
	//if (cd.m_bRightButton)
	//{
	//	OffsetRect(&cd.m_rcCheck, -m_CheckMargins.cxRightWidth, 0);
	//	cd.m_rcText.right -= m_CheckMargins.cxRightWidth + m_CheckMargins.cxLeftWidth;
	//}
	//else {
	//	OffsetRect(&cd.m_rcCheck, m_CheckMargins.cxLeftWidth, 0);
	//	cd.m_rcText.left += m_CheckMargins.cxRightWidth + m_CheckMargins.cxLeftWidth;
	//}

	// set horiz positioning. (just for text & focus)
	if (rd.m_bHCenter)
	{
		//center text within text area
		rd.m_textFlags |= DT_CENTER;
		rd.m_textFlags &= ~DT_RIGHT;
	}
	// weirdly BS_RIGHT seems to be set whenever we set BS_CENTER
	else if (rd.m_bRightJustify)
	{
		//text to right of text area
		rd.m_textFlags |= DT_RIGHT;
		rd.m_textFlags &= ~DT_CENTER;
	}

	rd.m_rcFocus = rd.m_rcText;

	//if (rd.m_bRightButton)
	//{
	//	rd.m_rcText.left += m_CheckMargins.cyBottomHeight;
	//	rd.m_rcText.right -= (m_CheckMargins.cyBottomHeight + m_CheckMargins.cyBottomHeight);
	//}
	//else {
	//	OffsetRect(&rd.m_rcText, m_CheckMargins.cyBottomHeight, 0);
	//	rd.m_rcFocus.right += (m_CheckMargins.cyBottomHeight + m_CheckMargins.cyBottomHeight);
	//}

	// make sure edge of focus rect stays in view.
	if (rd.m_rcFocus.right > rd.m_rc.right)
		rd.m_rcFocus.right = rd.m_rc.right;
	if (rd.m_rcFocus.left < rd.m_rc.left)
		rd.m_rcFocus.left = rd.m_rc.left;

	DcxUXModule::dcxDrawThemeBackground(rd.m_hTheme, rd.m_hdcPaint, BUTTONPARTS::BP_RADIOBUTTON, rd.m_iThemeState, &rd.m_rcRadio, nullptr);

	if (rd.m_tsText.empty())
		return;

	if (!this->m_TextOptions.m_bUseNewStyle)
	{
		// not using newstyle text, so try themed first.
		if (rd.m_hTheme)
		{
			// only used themed if no ctrl codes being used.
			if (this->IsControlCodeTextDisabled())
			{
				DTTOPTS dtt{};
				dtt.dwSize = sizeof(DTTOPTS);
				if (this->isExStyle(WindowExStyle::Composited))
				{
					dtt.dwFlags |= DTT_COMPOSITED;
				}
				if (this->m_TextOptions.m_clrText != CLR_INVALID)
				{
					dtt.crText = this->m_TextOptions.m_clrText;
					dtt.dwFlags |= DTT_TEXTCOLOR;
				}
				//if (!IsWindowEnabled(m_Hwnd))
				//	tsText.strip();

				DcxUXModule::dcxDrawThemeTextEx(rd.m_hTheme, rd.m_hdcPaint, BUTTONPARTS::BP_RADIOBUTTON, rd.m_iThemeState, rd.m_tsText.to_wchr(), gsl::narrow_cast<int>(rd.m_tsText.len()), rd.m_textFlags, std::addressof(rd.m_rcText), &dtt);
				return;
			}
		}
	}
	ctrlDrawText(rd.m_hdcPaint, rd.m_tsText, std::addressof(rd.m_rcText), rd.m_textFlags);

	if (rd.m_bFocus)
	{
		//CreatePen(PS_SOLID)
		DrawFocusRect(rd.m_hdcPaint, std::addressof(rd.m_rcRadio));
	}
}

void DcxRadio::DrawClientAreaNoTheme(HDC hdc, const UINT uMsg, LPARAM lParam)
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

void DcxRadio::DrawClientArea()
{
	if (HDC hdc = GetDC(m_Hwnd); hdc)
	{
		Auto(ReleaseDC(m_Hwnd, hdc));

		DrawClientArea(hdc, WM_PAINT, 0);
	}
}

void DcxRadio::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	//CleanUpParentCache();

	// Setup alpha blend if any.
	auto ai = SetupAlphaBlend(&hdc);
	Auto(FinishAlphaBlend(ai));

	//if (!IsThemed() || !DcxUXModule::dcxIsThemeActive())
	//{
	//	// get controls client area
	//	if (RECT rcClient{}; GetClientRect(m_Hwnd, &rcClient))
	//	{
	//		if (const auto clr = getBackColor(); clr != CLR_INVALID)
	//			SetBkColor(hdc, clr);
	//
	//		if (const auto clr = getTextColor(); clr != CLR_INVALID)
	//			SetTextColor(hdc, clr);
	//
	//		const auto bWasTransp = this->isExStyle(WindowExStyle::Transparent);
	//
	//		// fill background.
	//		if (bWasTransp)
	//		{
	//			if (!IsAlphaBlend())
	//				this->DrawParentsBackground(hdc, &rcClient);
	//		}
	//		else
	//			DcxControl::DrawCtrlBackground(hdc, this, &rcClient);
	//
	//		if (!bWasTransp)
	//			this->addExStyle(WindowExStyle::Transparent);
	//
	//		CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
	//
	//		if (!bWasTransp)
	//			this->removeExStyle(WindowExStyle::Transparent);
	//	}
	//}
	//else
	//	CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);

	if (IsThemed() && DcxUXModule::dcxIsThemeActive())
		DrawClientAreaThemed(hdc, uMsg, lParam);
	else
		DrawClientAreaNoTheme(hdc, uMsg, lParam);
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
