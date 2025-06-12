/*!
 * \file dcxcheck.cpp
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
#include "Classes/mirc/dcxcheck.h"
#include "Classes/dcxdialog.h"
#include "Dcx.h"

 /*!
  * \brief Constructor
  *
  * \param ID Control ID
  * \param p_Dialog Parent DcxDialog Object
  * \param mParentHwnd Parent Window Handle
  * \param rc Window Rectangle
  * \param styles Window Style Tokenized List
  */
DcxCheck::DcxCheck(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog, DcxControlTypes::CHECK)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_CHECKCLASS,
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

	if (styles.istok(TEXT("tooltips")))
	{
		if (!IsWindow(p_Dialog->getToolTipHWND()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		setToolTipHWND(p_Dialog->getToolTipHWND());
		AddToolTipToolInfo(getToolTipHWND(), m_Hwnd);
	}

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

DcxCheck::~DcxCheck()
{
}

void DcxCheck::toXml(TiXmlElement* const xml) const
{
	if (!xml || !m_Hwnd)
		return;

	__super::toXml(xml);

	const TString wtext(TGetWindowText(m_Hwnd));
	xml->SetAttribute("caption", wtext.c_str());
	xml->SetAttribute("styles", getStyles().c_str());
	xml->SetAttribute("state", Button_GetCheck(m_Hwnd));

	{
		TiXmlElement xColours("colours");

		setColourAttribute(&xColours, "checkbg", this->m_Colours.m_clrBackground);
		setColourAttribute(&xColours, "checkframe", this->m_Colours.m_clrFrame);
		setColourAttribute(&xColours, "checktick", this->m_Colours.m_clrTick);
		setColourAttribute(&xColours, "checkdisabledbg", this->m_Colours.m_clrDisabledBackground);
		setColourAttribute(&xColours, "checkdisabledframe", this->m_Colours.m_clrDisabledFrame);
		setColourAttribute(&xColours, "checkdisabledtick", this->m_Colours.m_clrDisabledTick);
		setColourAttribute(&xColours, "checkhotbg", this->m_Colours.m_clrHotBackground);
		setColourAttribute(&xColours, "checkhotframe", this->m_Colours.m_clrHotFrame);
		setColourAttribute(&xColours, "checkhottick", this->m_Colours.m_clrHotTick);

		xml->InsertEndChild(xColours);
	}
}

TiXmlElement* DcxCheck::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

std::unique_ptr<TiXmlElement> DcxCheck::toXml(int blah) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml;
}

void DcxCheck::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
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
	if (auto xColours = xThis->FirstChildElement("colours"); xColours)
	{
		if (const auto tmp = queryColourAttribute(xColours, "checkbg"); tmp != CLR_INVALID)
			this->m_Colours.m_clrBackground = tmp;
		if (const auto tmp = queryColourAttribute(xColours, "checkframe"); tmp != CLR_INVALID)
			this->m_Colours.m_clrFrame = tmp;
		if (const auto tmp = queryColourAttribute(xColours, "checktick"); tmp != CLR_INVALID)
			this->m_Colours.m_clrTick = tmp;
		if (const auto tmp = queryColourAttribute(xColours, "checkdisabledbg"); tmp != CLR_INVALID)
			this->m_Colours.m_clrDisabledBackground = tmp;
		if (const auto tmp = queryColourAttribute(xColours, "checkdisabledframe"); tmp != CLR_INVALID)
			this->m_Colours.m_clrDisabledFrame = tmp;
		if (const auto tmp = queryColourAttribute(xColours, "checkdisabledtick"); tmp != CLR_INVALID)
			this->m_Colours.m_clrDisabledTick = tmp;
		if (const auto tmp = queryColourAttribute(xColours, "checkhotbg"); tmp != CLR_INVALID)
			this->m_Colours.m_clrHotBackground = tmp;
		if (const auto tmp = queryColourAttribute(xColours, "checkhotframe"); tmp != CLR_INVALID)
			this->m_Colours.m_clrHotFrame = tmp;
		if (const auto tmp = queryColourAttribute(xColours, "checkhottick"); tmp != CLR_INVALID)
			this->m_Colours.m_clrHotTick = tmp;
	}
}

const TString DcxCheck::getStyles(void) const
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
	if (dcx_testflag(Styles, BS_AUTO3STATE))
		styles.addtok(TEXT("3state"));
	if (dcx_testflag(Styles, BS_VCENTER))
		styles.addtok(TEXT("vcenter"));
	if (m_bCustom)
		styles.addtok(TEXT("custom"));
	return styles;
}

dcxWindowStyles DcxCheck::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	ws.m_Styles |= BS_AUTOCHECKBOX;

	for (const auto& tsStyle : tsStyles)
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
		case L"3state"_hash:
		{
			ws.m_Styles &= gsl::narrow_cast<DWORD>(~BS_AUTOCHECKBOX);
			ws.m_Styles |= BS_AUTO3STATE;
		}
		break;
		case L"custom"_hash:
			m_bCustom = true;
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
TString DcxCheck::parseInfoRequest(const TString& input) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"text"_hash:
		return TGetWindowText(m_Hwnd);
		// [NAME] [ID] [PROP]
	case L"state"_hash:
	{
		TString tsRes(TEXT("0"));

		if (const auto iCheck = Button_GetCheck(m_Hwnd); dcx_testflag(iCheck, BST_INDETERMINATE))
			tsRes[0] = TEXT('2');
		else if (dcx_testflag(iCheck, BST_CHECKED))
			tsRes[0] = TEXT('1');

		return tsRes;
	}
	break;
	default:
		return parseGlobalInfoRequest(input);
		break;
	}
}

void DcxCheck::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	szReturnValue = parseInfoRequest(input).to_chr();
}

void DcxCheck::parseCommandRequest(const TString& input)
{
	const auto numtok = input.numtok();

	//xdid -c [NAME] [ID] [SWITCH]
	if (const XSwitchFlags flags(input.getfirsttok(3)); flags[TEXT('c')])
	{
		static_assert(CheckFreeCommand(TEXT('c')), "Command in use!");

		// xdid -cu
		if (flags[TEXT('u')])
			Button_SetCheck(m_Hwnd, BST_INDETERMINATE);
		else
			Button_SetCheck(m_Hwnd, BST_CHECKED);
	}
	//xdid -n [NAME] [ID] [SWITCH] [+FLAGS] [+MASK] (ARGS)
	else if (flags[TEXT('n')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xFlags(input.getnexttok());
		const XSwitchFlags xMask(input.getnexttok());
		const TString tsArgs(input.getlasttoks());

		if (!xFlags[L'+'] || !xMask[L'+'])
			throw DcxExceptions::dcxInvalidFlag();

		// rounded checkbox
		if (xFlags[L'r'])
		{
			this->m_bRoundedCheckBox = xMask[L'r'];
		}

		// margins
		if (xFlags[L'm'])
		{
			if (xMask[L'm'])
			{
				// ARGS = left top right text
				// no bottom is set
				if (tsArgs.numtok() != 4)
					throw DcxExceptions::dcxInvalidArguments();

				if (const auto tmp = tsArgs.getfirsttok(1).to_int(); tmp >= 0)
					m_CheckMargins.cxLeftWidth = tmp;
				if (const auto tmp = tsArgs.getnexttokas<int>(); tmp >= 0)
					m_CheckMargins.cyTopHeight = tmp;
				if (const auto tmp = tsArgs.getnexttokas<int>(); tmp >= 0)
					m_CheckMargins.cxRightWidth = tmp;
				if (const auto tmp = tsArgs.getnexttokas<int>(); tmp >= 0)
					m_CheckMargins.cyBottomHeight = tmp;
			}
			else {
				const MARGINS tmp{};
				m_CheckMargins = tmp;
			}
			this->InvalidateAndUpdate(nullptr);
		}
	}
	//xdid -t [NAME] [ID] [SWITCH] ItemText
	//else if (flags[TEXT('t')])
	else if (IsThisCommand<TEXT('t')>(flags))
	{
		Button_SetText(m_Hwnd, input.getlasttoks().trim().to_chr());	// tok 4, -1
	}
	//xdid -u [NAME] [ID] [SWITCH]
	else if (flags[TEXT('u')])
	{
		static_assert(CheckFreeCommand(TEXT('u')), "Command in use!");

		Button_SetCheck(m_Hwnd, BST_UNCHECKED);
	}
	// xdid -C [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if (flags[TEXT('C')])
	{
		// this is an overload of the global command.
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		// first do standard colours
		this->parseGlobalCommandRequest(input, flags);

		// now do checkbox specific ones.
		const auto iFlags = this->parseColorFlags(input.getfirsttok(4));
		const auto clrColor = input.getnexttokas<COLORREF>();	// tok 5

		if (dcx_testflag(iFlags, DcxColourFlags::CHECKBOXDISABLED))
		{
			// checkbox is disabled...
			if (dcx_testflag(iFlags, DcxColourFlags::CHECKBOXBGCOLOR))
				m_Colours.m_clrDisabledBackground = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::CHECKBOXFRAMECOLOR))
				m_Colours.m_clrDisabledFrame = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::CHECKBOXTICKCOLOR))
				m_Colours.m_clrDisabledTick = clrColor;
		}
		else if (dcx_testflag(iFlags, DcxColourFlags::CHECKBOXHOT))
		{
			// checkbox is NOT disabled & is hot
			if (dcx_testflag(iFlags, DcxColourFlags::CHECKBOXBGCOLOR))
				m_Colours.m_clrHotBackground = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::CHECKBOXFRAMECOLOR))
				m_Colours.m_clrHotFrame = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::CHECKBOXTICKCOLOR))
				m_Colours.m_clrHotTick = clrColor;
		}
		else {
			// checkbox is NOT disabled & is NOT hot
			if (dcx_testflag(iFlags, DcxColourFlags::CHECKBOXBGCOLOR))
				m_Colours.m_clrBackground = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::CHECKBOXFRAMECOLOR))
				m_Colours.m_clrFrame = clrColor;

			if (dcx_testflag(iFlags, DcxColourFlags::CHECKBOXTICKCOLOR))
				m_Colours.m_clrTick = clrColor;
		}
		if (m_Colours.m_clrHotFrame == CLR_INVALID)
			m_Colours.m_clrHotFrame = GetSysColor(COLOR_HOTLIGHT);
		if (m_Colours.m_clrHotTick == CLR_INVALID)
			m_Colours.m_clrHotTick = GetSysColor(COLOR_HOTLIGHT);
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

LRESULT DcxCheck::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		switch (Dcx::dcxHIWORD(wParam))
		{
			// catch this so we can use $xdid(checkbox).state in sclick callback
		case BN_CLICKED:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
				//execAliasEx(TEXT("sclick,%u"), getUserID());

				// /.timer repetitions delay alias dialog event id
				//mIRCLinker::execex(TEXT("/.timer 1 0 %s %s sclick %u"),
				//	this->getParentDialog()->getAliasName().to_chr(),
				//	this->getParentDialog()->getName().to_chr(),
				//	getUserID());
				mIRCLinker::exec(TEXT("/.timer 1 0 % % sclick %"),
					this->getParentDialog()->getAliasName(),
					this->getParentDialog()->getName(),
					getUserID());
			}
			InvalidateRect(m_Hwnd, nullptr, FALSE);
			break;
		}
		default:
			break;
		}

		break;
	}

	//case WM_NOTIFY:
	//{
	//	dcxlParam(LPNMHDR, hdr);
	//
	//	if (!hdr)
	//		break;
	//
	//	switch (hdr->code)
	//	{
	//	default:
	//		break;
	//	case NM_CUSTOMDRAW:
	//	{
	//		dcxlParam(LPNMCUSTOMDRAW, lpcd);
	//		bParsed = TRUE;
	//
	//		if ((!lpcd) || (!m_bCustom))
	//			return CDRF_DODEFAULT;
	//
	//		switch (lpcd->dwDrawStage)
	//		{
	//		case CDDS_PREERASE:
	//			//{
	//			//	return (CDRF_SKIPDEFAULT);
	//			//}
	//			//break;
	//			//case CDDS_POSTERASE:
	//			//{
	//			//	return CDRF_DODEFAULT;
	//			//}
	//			//break;
	//		case CDDS_PREPAINT:
	//		{
	//			// do all drawing here
	//			const clrCheckBox cols = m_Colours;
	//
	//			RECT rc{ lpcd->rc };
	//			RECT rcCheck{};
	//			// square (in future get system value for this)
	//			constexpr int iCheckSize = 16;
	//
	//			rcCheck.left = rc.left /*+ 1*/;
	//			rcCheck.right = rcCheck.left + iCheckSize;
	//			// center checkbox vertically in control
	//			rcCheck.top = ((rc.bottom - rc.top) / 2) - (iCheckSize / 2);
	//			rcCheck.bottom = rcCheck.top + iCheckSize;
	//
	//			//HDC hdcPaint = lpcd->hdc;
	//
	//			HDC hdcPaint{};
	//			const auto hPaintBuf = CreateHDCBufferNoCopy(lpcd->hdc, &hdcPaint);
	//			if (!hPaintBuf || !hdcPaint)
	//				hdcPaint = lpcd->hdc;
	//
	//			if (this->IsThemed())
	//			{
	//				auto hTheme = DcxUXModule::dcxGetWindowTheme(m_Hwnd);
	//				if (!hTheme)
	//					break;
	//
	//				int iState{};
	//				const bool bChecked = (Button_GetCheck(m_Hwnd) == BST_CHECKED);
	//				if (dcx_testflag(lpcd->uItemState, CDIS_DISABLED))
	//				{
	//					if (bChecked)
	//						iState = CHECKBOXSTATES::CBS_CHECKEDDISABLED;
	//					else
	//						iState = CHECKBOXSTATES::CBS_UNCHECKEDDISABLED;
	//				}
	//				else if (dcx_testflag(lpcd->uItemState, CDIS_SELECTED))
	//				{
	//					if (bChecked)
	//						iState = CHECKBOXSTATES::CBS_CHECKEDPRESSED;
	//					else
	//						iState = CHECKBOXSTATES::CBS_UNCHECKEDPRESSED;
	//				}
	//				else if (dcx_testflag(lpcd->uItemState, CDIS_HOT))
	//				{
	//					if (bChecked)
	//						iState = CHECKBOXSTATES::CBS_CHECKEDHOT;
	//					else
	//						iState = CHECKBOXSTATES::CBS_UNCHECKEDHOT;
	//				}
	//				else {
	//					if (bChecked)
	//						iState = CHECKBOXSTATES::CBS_CHECKEDNORMAL;
	//					else
	//						iState = CHECKBOXSTATES::CBS_UNCHECKEDNORMAL;
	//				}
	//
	//				if (DcxUXModule::dcxIsThemeBackgroundPartiallyTransparent(hTheme, BUTTONPARTS::BP_CHECKBOX, iState))
	//					DcxUXModule::dcxDrawThemeParentBackground(m_Hwnd, hdcPaint, &rc);
	//
	//				DcxUXModule::dcxDrawThemeBackground(hTheme, hdcPaint, BUTTONPARTS::BP_CHECKBOX, iState, &rcCheck, &rc);
	//				if (const auto tsText(TGetWindowText(m_Hwnd)); !tsText.empty())
	//				{
	//					// move left edge of rect past the checkbox
	//					rc.left = rcCheck.right + 1;
	//
	//					if (rc.right > rc.left)
	//						DcxUXModule::dcxDrawThemeText(hTheme, hdcPaint, BUTTONPARTS::BP_CHECKBOX, iState, tsText.to_wchr(), tsText.len(), DT_LEFT | DT_SINGLELINE | DT_VCENTER, 0, &rc);
	//				}
	//			}
	//			else {
	//				if (this->isExStyle(WindowExStyle::Transparent))
	//					this->DrawParentsBackground(hdcPaint, std::addressof(rc));
	//				else
	//					DcxControl::DrawCtrlBackground(hdcPaint, this, std::addressof(rc));
	//
	//				dcxDrawCheckBox(hdcPaint, std::addressof(rcCheck), std::addressof(cols), lpcd->uItemState, (Button_GetCheck(m_Hwnd) == BST_CHECKED), this->m_bRoundedCheckBox);
	//
	//				// move left edge of rect past the checkbox
	//				rc.left = rcCheck.right + 1;
	//
	//				// check theres still room for the text.
	//				if (rc.right > rc.left)
	//				{
	//					if (const auto tsText(TGetWindowText(m_Hwnd)); !tsText.empty())
	//					{
	//						//ctrlDrawText(lpcd->hdc, tsText, std::addressof(rc), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	//
	//						// get height of text. NB: DT_VCENTER messes things up for some reason...
	//						RECT rcText{};
	//						calcTextRect(hdcPaint, tsText, std::addressof(rcText), DT_LEFT | DT_SINGLELINE /*| DT_VCENTER*/);
	//
	//						// set right hand limit to the size of the control.
	//						rcText.right = rc.right;
	//
	//						// offset text to right of checkbox, & vertically centered.
	//						OffsetRect(&rcText, rc.left, ((rc.bottom - rc.top) / 2) - ((rcText.bottom - rcText.top) / 2));
	//
	//						// finally draw text.
	//						ctrlDrawText(hdcPaint, tsText, std::addressof(rcText), DT_LEFT | DT_SINGLELINE /*| DT_VCENTER*/);
	//					}
	//				}
	//				if (dcx_testflag(lpcd->uItemState, CDIS_FOCUS))
	//					DrawFocusRect(hdcPaint, std::addressof(rc));
	//			}
	//
	//			if (hPaintBuf)
	//				DeleteHDCBufferNoCopy(hPaintBuf);
	//
	//			return CDRF_SKIPDEFAULT;
	//		}
	//		break;
	//
	//		default:
	//			return CDRF_DODEFAULT;
	//		}
	//	}
	//	break;
	//
	//	}
	//}
	//break;

	default:
		break;
	}

	return 0L;
}

LRESULT DcxCheck::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
	{
		if (const auto pDialog = getParentDialog(); pDialog)
			pDialog->setMouseControl(getUserID());

		m_bHover = false;

		if (!m_bTracking)
			m_bTracking = TrackMouseEvents(TME_LEAVE | TME_HOVER);
		if (!m_bHot)
		{
			m_bHot = true;
			InvalidateRect(m_Hwnd, nullptr, FALSE);
		}
	}
	break;

	case WM_MOUSEHOVER:
	{
		if (!m_bHover && m_bTracking)
		{
			m_bHover = true;
			InvalidateRect(m_Hwnd, nullptr, FALSE);
		}
	}
	break;

	case WM_MOUSELEAVE:
	{
		if (m_bTracking)
		{
			m_bHover = false;
			m_bTracking = false;
			m_bHot = false;
			m_bSelected = false;
			InvalidateRect(m_Hwnd, nullptr, FALSE);
		}
	}
	break;

	case WM_LBUTTONDOWN:
	{
		if (!m_bSelected)
		{
			m_bSelected = true;
			InvalidateRect(m_Hwnd, nullptr, FALSE);
		}
		if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("lbdown,%u"), getUserID());
	}
	break;

	case WM_LBUTTONUP:
	{
		m_bSelected = false;
		if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("lbup,%u"), getUserID());
	}
	break;

	case WM_ENABLE:
	{
		InvalidateRect(m_Hwnd, nullptr, FALSE);
		bParsed = TRUE;
	}
	break;

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
		if (GetUpdateRect(m_Hwnd, nullptr, FALSE))
		{
			PAINTSTRUCT ps{};
			auto hdc = BeginPaint(m_Hwnd, &ps);
			Auto(EndPaint(m_Hwnd, &ps));

			this->DrawClientArea(hdc, uMsg, lParam);
		}
		else if (wParam)
			this->DrawClientArea(reinterpret_cast<HDC>(wParam), uMsg, lParam);
		else {
			auto hdc = GetDC(m_Hwnd);
			//auto hdc = GetDCEx(m_Hwnd, nullptr, DCX_CLIPCHILDREN|DCX_CLIPSIBLINGS); // <- Ook: needs testing
			Auto(ReleaseDC(m_Hwnd, hdc));

			this->DrawClientArea(hdc, uMsg, lParam);

			//ValidateRect(m_Hwnd, nullptr);
		}
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

void DcxCheck::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	//CleanUpParentCache();

	// Setup alpha blend if any.
	const auto ai = SetupAlphaBlend(&hdc, true);
	Auto(FinishAlphaBlend(ai));

	//if (!this->IsThemed() || !DcxUXModule::dcxIsThemeActive())
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
	//		if (this->m_bCustom)
	//		{
	//			CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
	//		}
	//		else {
	//			const auto bWasTransp = this->isExStyle(WindowExStyle::Transparent);
	//
	//			// fill background.
	//			if (bWasTransp)
	//			{
	//				if (!this->IsAlphaBlend())
	//					this->DrawParentsBackground(hdc, &rcClient);
	//			}
	//			else
	//				DcxControl::DrawCtrlBackground(hdc, this, &rcClient);
	//
	//			// This is a workaround to allow our background to be seen under the control.
	//			if (!bWasTransp)
	//				this->addExStyle(WindowExStyle::Transparent);
	//
	//			CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
	//
	//			if (!bWasTransp)
	//				this->removeExStyle(WindowExStyle::Transparent);
	//		}
	//	}
	//}
	//else
	//	CallDefaultClassProc(WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdc),/* PRF_NONCLIENT |*/ PRF_CLIENT | PRF_CHILDREN);

	//// do all drawing here
	//const clrCheckBox cols = m_Colours;
	//
	//RECT rc{};
	//GetClientRect(m_Hwnd, &rc);
	//
	//RECT rcCheck{};
	//// square (in future get system value for this)
	//constexpr int iCheckSize = 16;
	//
	//rcCheck.left = rc.left /*+ 1*/;
	//rcCheck.right = rcCheck.left + iCheckSize;
	//// center checkbox vertically in control
	//rcCheck.top = ((rc.bottom - rc.top) / 2) - (iCheckSize / 2);
	//rcCheck.bottom = rcCheck.top + iCheckSize;
	//
	////HDC hdcPaint = hdc;
	//
	//HDC hdcPaint{};
	//const auto hPaintBuf = CreateHDCBufferNoCopy(hdc, &hdcPaint);
	//Auto(DeleteHDCBufferNoCopy(hPaintBuf));
	//
	//if (!hPaintBuf || !hdcPaint)
	//	hdcPaint = hdc;
	//
	//const auto hFontOld = Dcx::dcxSelectObject<HFONT>(hdcPaint, getControlFont());
	//Auto(Dcx::dcxSelectObject<HFONT>(hdcPaint, hFontOld));
	//
	//const bool bDisabled = !IsWindowEnabled(m_Hwnd);
	//const bool bChecked = (Button_GetCheck(m_Hwnd) == BST_CHECKED);
	//const bool bSelected = m_bSelected;
	//const bool bFocus = (GetFocus() == m_Hwnd);
	//const bool bHot = m_bHot;
	//
	//if (this->IsThemed())
	//{
	//	auto hTheme = DcxUXModule::dcxGetWindowTheme(m_Hwnd);
	//	if (!hTheme)
	//	{
	//		CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
	//		return;
	//	}
	//
	//	int iState = CHECKBOXSTATES::CBS_UNCHECKEDNORMAL;
	//
	//	if (bDisabled)
	//	{
	//		if (bChecked)
	//			iState = CHECKBOXSTATES::CBS_CHECKEDDISABLED;
	//		else
	//			iState = CHECKBOXSTATES::CBS_UNCHECKEDDISABLED;
	//	}
	//	else if (bSelected)
	//	{
	//		if (bChecked)
	//			iState = CHECKBOXSTATES::CBS_CHECKEDPRESSED;
	//		else
	//			iState = CHECKBOXSTATES::CBS_UNCHECKEDPRESSED;
	//	}
	//	else if (bHot)
	//	{
	//		if (bChecked)
	//			iState = CHECKBOXSTATES::CBS_CHECKEDHOT;
	//		else
	//			iState = CHECKBOXSTATES::CBS_UNCHECKEDHOT;
	//	}
	//	else if (bChecked)
	//		iState = CHECKBOXSTATES::CBS_CHECKEDNORMAL;
	//
	//	if (DcxUXModule::dcxIsThemeBackgroundPartiallyTransparent(hTheme, BUTTONPARTS::BP_CHECKBOX, iState))
	//		DcxUXModule::dcxDrawThemeParentBackground(m_Hwnd, hdcPaint, &rc);
	//
	//	if (!this->isExStyle(WindowExStyle::Transparent))
	//		DcxControl::DrawCtrlBackground(hdcPaint, this, &rc);
	//
	//	DcxUXModule::dcxDrawThemeBackground(hTheme, hdcPaint, BUTTONPARTS::BP_CHECKBOX, iState, &rcCheck, nullptr);
	//	if (const auto tsText(TGetWindowText(m_Hwnd)); !tsText.empty())
	//	{
	//		// move left edge of rect past the checkbox
	//		rc.left = rcCheck.right + 1;
	//
	//		if (rc.right > rc.left)
	//		{
	//			if (this->m_TextOptions.m_bNoCtrlCodes)
	//			{
	//				DTTOPTS dtt{};
	//				dtt.dwSize = sizeof(DTTOPTS);
	//				if (this->m_TextOptions.m_clrText != CLR_INVALID)
	//				{
	//					dtt.crText = this->m_TextOptions.m_clrText;
	//					dtt.dwFlags |= DTT_TEXTCOLOR;
	//				}
	//
	//				DcxUXModule::dcxDrawThemeTextEx(hTheme, hdcPaint, BUTTONPARTS::BP_CHECKBOX, iState, tsText.to_wchr(), tsText.len(), DT_LEFT | DT_SINGLELINE | DT_VCENTER, &rc, &dtt);
	//			}
	//			else
	//				ctrlDrawText(hdcPaint, tsText, std::addressof(rc), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	//		}
	//	}
	//}
	//else {
	//	if (this->isExStyle(WindowExStyle::Transparent))
	//		this->DrawParentsBackground(hdcPaint, std::addressof(rc));
	//	else
	//		DcxControl::DrawCtrlBackground(hdcPaint, this, std::addressof(rc));
	//
	//	DWORD dState{};
	//
	//	if (bDisabled)
	//		dState |= CDIS_DISABLED;
	//	if (bSelected)
	//		dState |= CDIS_SELECTED;
	//	if (bHot)
	//		dState |= CDIS_HOT;
	//	if (bChecked)
	//		dState |= CDIS_CHECKED;
	//	if (bFocus)
	//		dState |= CDIS_FOCUS;
	//
	//	dcxDrawCheckBox(hdcPaint, std::addressof(rcCheck), std::addressof(cols), dState, bChecked, this->m_bRoundedCheckBox);
	//
	//	// move left edge of rect past the checkbox
	//	rc.left = rcCheck.right + 1;
	//
	//	// check theres still room for the text.
	//	if (rc.right > rc.left)
	//	{
	//		if (const auto tsText(TGetWindowText(m_Hwnd)); !tsText.empty())
	//		{
	//			//ctrlDrawText(hdcPaint, tsText, std::addressof(rc), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	//
	//			// get height of text. NB: DT_VCENTER messes things up for some reason...
	//			RECT rcText{};
	//			calcTextRect(hdcPaint, tsText, std::addressof(rcText), DT_LEFT | DT_SINGLELINE /*| DT_VCENTER*/);
	//
	//			// set right hand limit to the size of the control.
	//			rcText.right = rc.right;
	//
	//			// offset text to right of checkbox, & vertically centered.
	//			OffsetRect(&rcText, rc.left, ((rc.bottom - rc.top) / 2) - ((rcText.bottom - rcText.top) / 2));
	//
	//			// finally draw text.
	//			ctrlDrawText(hdcPaint, tsText, std::addressof(rcText), DT_LEFT | DT_SINGLELINE /*| DT_VCENTER*/);
	//		}
	//	}
	//}

	dcxCheckData cd;

	if (!GetClientRect(m_Hwnd, &cd.m_rc))
		return;

	if (IsRectEmpty(&cd.m_rc))
		return;

	//const auto hPaintBuf = CreateHDCBufferOptions(hdc, &cd.m_hdcPaint, true, false, false, 255);
	//Auto(DeleteHDCBufferOptions(hPaintBuf, true));

	//if (!hPaintBuf || !cd.m_hdcPaint)
	cd.m_hdcPaint = hdc;
	//cd.m_rcCheck = cd.m_rc;
	//cd.m_rcText = cd.m_rc;

	const auto hFontOld = Dcx::dcxSelectObject<HFONT>(cd.m_hdcPaint, getControlFont());
	Auto(Dcx::dcxSelectObject<HFONT>(cd.m_hdcPaint, hFontOld));

	if (this->IsThemed())
		cd.m_hTheme = DcxUXModule::dcxGetWindowTheme(m_Hwnd);

	cd.m_ButtonState = Button_GetCheck(m_Hwnd);
	cd.m_WinStyle = dcxGetWindowStyle(m_Hwnd);

	cd.m_bDisabled = !IsWindowEnabled(m_Hwnd);
	cd.m_bChecked = (cd.m_ButtonState == BST_CHECKED);
	cd.m_bSelected = m_bSelected;
	cd.m_bIndeterminate = (cd.m_ButtonState == BST_INDETERMINATE);
	cd.m_bFocus = (GetFocus() == m_Hwnd);
	cd.m_bHot = m_bHot;
	cd.m_b3State = (dcx_testflag(cd.m_WinStyle, BS_AUTO3STATE) || dcx_testflag(cd.m_WinStyle, BS_3STATE));
	cd.m_bRightJustify = dcx_testflag(cd.m_WinStyle, BS_RIGHT);
	cd.m_bHCenter = dcx_testflag(cd.m_WinStyle, BS_CENTER);
	cd.m_bVCenter = dcx_testflag(cd.m_WinStyle, BS_VCENTER);
	cd.m_bRightButton = dcx_testflag(cd.m_WinStyle, BS_RIGHTBUTTON);

	cd.m_DrawFrameState = gsl::narrow_cast<DWORD>((cd.m_b3State ? DFCS_BUTTON3STATE : DFCS_BUTTONCHECK));

	cd.m_iState = CHECKBOXSTATES::CBS_UNCHECKEDNORMAL;

	cd.m_tsText = TGetWindowText(m_Hwnd);

	if (cd.m_bDisabled)
	{
		if (cd.m_bChecked)
			cd.m_iState = CHECKBOXSTATES::CBS_CHECKEDDISABLED;
		else if (cd.m_b3State && cd.m_bIndeterminate)
			cd.m_iState = CHECKBOXSTATES::CBS_MIXEDDISABLED;
		else
			cd.m_iState = CHECKBOXSTATES::CBS_UNCHECKEDDISABLED;
	}
	else if (cd.m_bSelected)
	{
		if (cd.m_bChecked)
			cd.m_iState = CHECKBOXSTATES::CBS_CHECKEDPRESSED;
		else if (cd.m_b3State && cd.m_bIndeterminate)
			cd.m_iState = CHECKBOXSTATES::CBS_MIXEDPRESSED;
		else
			cd.m_iState = CHECKBOXSTATES::CBS_UNCHECKEDPRESSED;
	}
	else if (cd.m_bHot)
	{
		if (cd.m_bChecked)
			cd.m_iState = CHECKBOXSTATES::CBS_CHECKEDHOT;
		else if (cd.m_b3State && cd.m_bIndeterminate)
			cd.m_iState = CHECKBOXSTATES::CBS_MIXEDHOT;
		else
			cd.m_iState = CHECKBOXSTATES::CBS_UNCHECKEDHOT;
	}
	else if (cd.m_bChecked)
		cd.m_iState = CHECKBOXSTATES::CBS_CHECKEDNORMAL;
	else if (cd.m_b3State && cd.m_bIndeterminate)
		cd.m_iState = CHECKBOXSTATES::CBS_MIXEDNORMAL;

	switch (cd.m_iState)
	{
	case CHECKBOXSTATES::CBS_CHECKEDDISABLED:
	{
		cd.m_DrawState |= CDIS_CHECKED;
		cd.m_DrawFrameState |= DFCS_CHECKED;
	}
	[[fallthrough]];
	case CHECKBOXSTATES::CBS_UNCHECKEDDISABLED:
	{
		cd.m_DrawState |= CDIS_DISABLED;
		cd.m_DrawFrameState |= DFCS_INACTIVE;
	}
	break;
	case CHECKBOXSTATES::CBS_CHECKEDHOT:
	{
		cd.m_DrawState |= CDIS_CHECKED;
		cd.m_DrawFrameState |= DFCS_CHECKED;
	}
	[[fallthrough]];
	case CHECKBOXSTATES::CBS_UNCHECKEDHOT:
	{
		cd.m_DrawState |= CDIS_HOT;
		cd.m_DrawFrameState |= DFCS_HOT;
	}
	break;
	case CHECKBOXSTATES::CBS_CHECKEDPRESSED:
	{
		cd.m_DrawState |= CDIS_CHECKED;
		cd.m_DrawFrameState |= DFCS_CHECKED;
	}
	[[fallthrough]];
	case CHECKBOXSTATES::CBS_UNCHECKEDPRESSED:
	{
		cd.m_DrawState |= CDIS_SELECTED;
		cd.m_DrawFrameState |= DFCS_PUSHED;
	}
	break;
	case CHECKBOXSTATES::CBS_CHECKEDNORMAL:
	{
		cd.m_DrawState |= CDIS_CHECKED;
		cd.m_DrawFrameState |= DFCS_CHECKED;
	}
	[[fallthrough]];
	case CHECKBOXSTATES::CBS_UNCHECKEDNORMAL:
	default:
		break;
	}
	if (cd.m_bFocus)
		cd.m_DrawState |= CDIS_FOCUS;

	cd.m_szCheckSize = DcxUXModule::dcxGetCheckBoxSize(cd.m_hTheme, m_Hwnd, cd.m_hdcPaint, &cd.m_rc);

	// make sure checkbox & text are in correct order.
	if (cd.m_bRightButton)
	{
		cd.m_rcCheck.left = cd.m_rc.right - cd.m_szCheckSize.cx;
		cd.m_rcCheck.right = cd.m_rc.right;
		cd.m_rcCheck.bottom = cd.m_szCheckSize.cy;

		cd.m_rcText = cd.m_rc;
		cd.m_rcText.right = cd.m_rcCheck.left - 1;
	}
	else {
		cd.m_rcCheck.right = cd.m_szCheckSize.cx;
		cd.m_rcCheck.bottom = cd.m_szCheckSize.cy;

		cd.m_rcText = cd.m_rc;
		cd.m_rcText.left = cd.m_rcCheck.right + 1;
	}

	// set vert positioning. (for checkbox, text, & focus)
	if (cd.m_bVCenter)
	{
		// center checkbox vertically in control
		OffsetRect(&cd.m_rcCheck, 0, (((cd.m_rc.bottom - cd.m_rc.top) / 2) - (cd.m_szCheckSize.cy / 2)));

		// center text vertically in control
		//cd.m_textFlags |= DT_VCENTER;	// Ook: doesnt seem to center correctly, needs looked at.

		RECT rcTmp{};
		calcTextRect(cd.m_hdcPaint, cd.m_tsText, std::addressof(rcTmp), cd.m_textFlags);

		cd.m_rcText.top = (((cd.m_rc.bottom - cd.m_rc.top) / 2) - ((rcTmp.bottom - rcTmp.top) / 2));
		cd.m_rcText.bottom = cd.m_rcText.top + (rcTmp.bottom - rcTmp.top);
	}

	OffsetRect(&cd.m_rcCheck, 0, m_CheckMargins.cyTopHeight);

	if (cd.m_bRightButton)
	{
		OffsetRect(&cd.m_rcCheck, -m_CheckMargins.cxRightWidth, 0);
		cd.m_rcText.right -= m_CheckMargins.cxRightWidth + m_CheckMargins.cxLeftWidth;
	}
	else {
		OffsetRect(&cd.m_rcCheck, m_CheckMargins.cxLeftWidth, 0);
		cd.m_rcText.left += m_CheckMargins.cxRightWidth + m_CheckMargins.cxLeftWidth;
	}

	// set horiz positioning. (just for text & focus)
	if (cd.m_bHCenter)
	{
		cd.m_textFlags |= DT_CENTER;

		if (cd.m_bRightButton)
		{
			cd.m_rcFocus = cd.m_rcText;
			cd.m_rcText.left += m_CheckMargins.cyBottomHeight;
			cd.m_rcText.right -= (m_CheckMargins.cyBottomHeight + m_CheckMargins.cyBottomHeight);
		}
		else {
			cd.m_rcFocus = cd.m_rcText;
			OffsetRect(&cd.m_rcText, m_CheckMargins.cyBottomHeight, 0);
			cd.m_rcFocus.right += (m_CheckMargins.cyBottomHeight + m_CheckMargins.cyBottomHeight);
		}
	}
	else {
		if (cd.m_bRightButton)
		{
			cd.m_rcFocus = cd.m_rcText;
			cd.m_rcText.left += m_CheckMargins.cyBottomHeight;
			cd.m_rcText.right -= (m_CheckMargins.cyBottomHeight + m_CheckMargins.cyBottomHeight);
		}
		else {
			cd.m_rcFocus = cd.m_rcText;
			OffsetRect(&cd.m_rcText, m_CheckMargins.cyBottomHeight, 0);
			cd.m_rcFocus.right += (m_CheckMargins.cyBottomHeight + m_CheckMargins.cyBottomHeight);
		}
	}

	if (cd.m_bRightJustify)
	{
		cd.m_textFlags |= DT_RIGHT;
		cd.m_textFlags &= ~DT_CENTER;
	}

	// make sure right edge of focus rect stays in view.
	if (cd.m_rcFocus.right > cd.m_rc.right)
		cd.m_rcFocus.right = cd.m_rc.right;

	ctrlDrawBackground(cd);
	ctrlDrawCheckBox(cd);
	ctrlDrawCheckText(cd);

	if (cd.m_bFocus)
		DrawFocusRect(cd.m_hdcPaint, std::addressof(cd.m_rcFocus));
}

void DcxCheck::ctrlDrawBackground(dcxCheckData& cd)
{
	if (!cd.m_hdcPaint || IsRectEmpty(&cd.m_rc) || (cd.m_rc.right <= cd.m_rc.left))
		return;

	if (cd.m_hTheme)
	{
		if (!this->isExStyle(WindowExStyle::Transparent))
		{
			if (m_bCustom)
				DcxControl::DrawCtrlBackground(cd.m_hdcPaint, this, &cd.m_rc);
			else
				DcxControl::DrawCtrlBackground(cd.m_hdcPaint, this, &cd.m_rc, cd.m_hTheme, BUTTONPARTS::BP_CHECKBOX, cd.m_iState);
		}
	}
	else {
		if (this->isExStyle(WindowExStyle::Transparent))
			this->DrawParentsBackground(cd.m_hdcPaint, &cd.m_rc);
		else
			DcxControl::DrawCtrlBackground(cd.m_hdcPaint, this, &cd.m_rc);
	}
}

void DcxCheck::ctrlDrawCheckBox(dcxCheckData& cd) noexcept
{
	if (!cd.m_hdcPaint || IsRectEmpty(&cd.m_rcCheck) || (cd.m_rcCheck.right <= cd.m_rcCheck.left))
		return;

	if (this->m_bCustom)
		dcxDrawCheckBox(cd.m_hdcPaint, std::addressof(cd.m_rcCheck), std::addressof(m_Colours), cd.m_DrawState, (dcx_testflag(cd.m_DrawState, CDIS_CHECKED)), this->m_bRoundedCheckBox);
	else if (cd.m_hTheme)
		DcxUXModule::dcxDrawThemeBackground(cd.m_hTheme, cd.m_hdcPaint, BUTTONPARTS::BP_CHECKBOX, cd.m_iState, &cd.m_rcCheck, nullptr);
	else
		DrawFrameControl(cd.m_hdcPaint, &cd.m_rcCheck, DFC_BUTTON, cd.m_DrawFrameState);
}

void DcxCheck::ctrlDrawCheckText(dcxCheckData& cd)
{
	if (!cd.m_hdcPaint || IsRectEmpty(&cd.m_rc) || (cd.m_rc.right <= cd.m_rc.left))
		return;

	if (cd.m_tsText.empty())
		return;

	//RECT rcBase{ cd.m_rcText };
	//rcBase.left += m_CheckMargins.cyBottomHeight;

	if (!this->m_TextOptions.m_bUseNewStyle)
	{
		// not using newstyle text, so try themed first.
		if (cd.m_hTheme)
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

				DcxUXModule::dcxDrawThemeTextEx(cd.m_hTheme, cd.m_hdcPaint, BUTTONPARTS::BP_CHECKBOX, cd.m_iState, cd.m_tsText.to_wchr(), cd.m_tsText.len(), cd.m_textFlags, std::addressof(cd.m_rcText), &dtt);
				return;
			}
		}
	}
	ctrlDrawText(cd.m_hdcPaint, cd.m_tsText, std::addressof(cd.m_rcText), cd.m_textFlags);

	//// get height of text. NB: DT_VCENTER messes things up for some reason...
	//calcTextRect(cd.m_hdcPaint, cd.m_tsText, std::addressof(cd.m_rcText), cd.m_textFlags);

	//// set right hand limit to the size of the control.
	//cd.m_rcText.right = rcBase.right;

	//// offset text to right of checkbox, & vertically centered.
	//OffsetRect(std::addressof(cd.m_rcText), rcBase.left, ((rcBase.bottom - rcBase.top) / 2) - ((cd.m_rcText.bottom - cd.m_rcText.top) / 2));

	//// finally draw text.
	//ctrlDrawText(cd.m_hdcPaint, cd.m_tsText, std::addressof(cd.m_rcText), cd.m_textFlags);
}

LRESULT DcxCheck::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
