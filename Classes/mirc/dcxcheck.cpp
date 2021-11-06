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

DcxCheck::DcxCheck(const UINT ID, DcxDialog* const p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
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

	if (!IsWindow(m_Hwnd))
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	if (styles.istok(TEXT("tooltips")))
	{
		if (!IsWindow(p_Dialog->getToolTip()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		setToolTipHWND(p_Dialog->getToolTip());
		AddToolTipToolInfo(getToolTipHWND(), m_Hwnd);
	}

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxCheck::~DcxCheck()
{
}


void DcxCheck::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	const TString wtext(TGetWindowText(m_Hwnd));
	xml->SetAttribute("caption", wtext.c_str());
	xml->SetAttribute("styles", getStyles().c_str());
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

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxCheck::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	ws.m_Styles |= BS_AUTOCHECKBOX;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle.to_chr()))
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

/*!
 * \brief blah
 *
 * blah
 */

void DcxCheck::parseCommandRequest(const TString& input)
{
	//xdid -c [NAME] [ID] [SWITCH]
	if (const XSwitchFlags flags(input.getfirsttok(3)); flags[TEXT('c')])
	{
		// xdid -cu
		if (flags[TEXT('u')])
			Button_SetCheck(m_Hwnd, BST_INDETERMINATE);
		else
			Button_SetCheck(m_Hwnd, BST_CHECKED);
	}
	//xdid -t [NAME] [ID] [SWITCH] ItemText
	else if (flags[TEXT('t')])
	{
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
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
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

			break;
		}
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
		default:
			break;
		case NM_CUSTOMDRAW:
		{
			dcxlParam(LPNMCUSTOMDRAW, lpcd);
			bParsed = TRUE;

			if ((!lpcd) || (!m_bCustom))
				return CDRF_DODEFAULT;

			switch (lpcd->dwDrawStage)
			{
			case CDDS_PREERASE:
				//{
				//	return (CDRF_SKIPDEFAULT);
				//}
				//break;
				//case CDDS_POSTERASE:
				//{
				//	return CDRF_DODEFAULT;
				//}
				//break;
			case CDDS_PREPAINT:
			{
				// do all drawing here
				clrCheckBox cols;
				cols.m_clrHotFrame = GetSysColor(COLOR_HOTLIGHT);
				cols.m_clrHotTick = GetSysColor(COLOR_HOTLIGHT);

				RECT rc{ lpcd->rc };
				RECT rcCheck{};
				// square
				constexpr int iCheckSize = 16;

				rcCheck.left = rc.left + 1;
				rcCheck.right = rcCheck.left + iCheckSize;
				// center checkbox vertically in control
				rcCheck.top = ((rc.bottom - rc.top) / 2) - (iCheckSize / 2);
				rcCheck.bottom = rcCheck.top + iCheckSize;

				DcxControl::DrawCtrlBackground(lpcd->hdc, this, std::addressof(rc));

				dcxDrawCheckBox(lpcd->hdc, std::addressof(rcCheck), std::addressof(cols), lpcd->uItemState, (Button_GetCheck(m_Hwnd) == BST_CHECKED), false);

				// move left edge of rect past the checkbox
				rc.left = rcCheck.right + 1;

				// check theres still room for the text.
				if (rc.right > rc.left)
				{
					if (const auto tsText(TGetWindowText(m_Hwnd)); !tsText.empty())
					{
						//ctrlDrawText(lpcd->hdc, tsText, std::addressof(rc), DT_LEFT | DT_SINGLELINE | DT_VCENTER);

						// get height of text. NB: DT_VCENTER messes things up for some reason...
						RECT rcText{};
						calcTextRect(lpcd->hdc, tsText, std::addressof(rcText), DT_LEFT | DT_SINGLELINE /*| DT_VCENTER*/);

						// set right hand limit to the size of the control.
						rcText.right = rc.right;

						// offset text to right of checkbox, & vertically centered.
						OffsetRect(&rcText, rc.left, ((rc.bottom - rc.top)/2) - ((rcText.bottom - rcText.top)/2));

						// finally draw text.
						ctrlDrawText(lpcd->hdc, tsText, std::addressof(rcText), DT_LEFT | DT_SINGLELINE /*| DT_VCENTER*/);
					}
				}
				return CDRF_SKIPDEFAULT;
			}
			break;

			default:
				return CDRF_DODEFAULT;
			}
		}
		break;

		}
	}
	break;

	default:
		break;
	}

	return 0L;
}

LRESULT DcxCheck::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_LBUTTONUP:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("lbup,%u"), getUserID());
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
		else {
			auto hdc = GetDC(m_Hwnd);
			//auto hdc = GetDCEx(m_Hwnd, nullptr, DCX_CLIPCHILDREN|DCX_CLIPSIBLINGS); // <- Ook: needs testing
			Auto(ReleaseDC(m_Hwnd, hdc));

			this->DrawClientArea(hdc, uMsg, lParam);
		}
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

void DcxCheck::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	// Setup alpha blend if any.
	const auto ai = SetupAlphaBlend(&hdc);
	Auto(FinishAlphaBlend(ai));

	if (!this->IsThemed() || !Dcx::UXModule.dcxIsThemeActive())
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
				if (!this->IsAlphaBlend())
					this->DrawParentsBackground(hdc, &rcClient);
			}
			else
				DcxControl::DrawCtrlBackground(hdc, this, &rcClient);

			// This is a workaround to allow our background to be seen under the control.
			if (!bWasTransp)
				this->addExStyle(WindowExStyle::Transparent);

			CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);

			if (!bWasTransp)
				this->removeExStyle(WindowExStyle::Transparent);
		}
	}
	else
		CallDefaultClassProc(WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdc), PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN);
}

LRESULT DcxCheck::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
