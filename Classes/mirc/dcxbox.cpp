/*!
 * \file dcxbox.cpp
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
#include "dcxbox.h"
#include "Dcx.h"
#include "Classes/dcxdialog.h"

#include "Classes/layout/layoutcellfixed.h"
#include "Classes/layout/layoutcellfill.h"
#include "Classes/layout/layoutcellpane.h"

 /*!
  * \brief Constructor
  *
  * \param ID Control ID
  * \param p_Dialog Parent DcxDialog Object
  * \param mParentHwnd Parent Window Handle
  * \param rc Window Rectangle
  * \param styles Window Style Tokenized List
  */

DcxBox::DcxBox(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WindowExStyle::ControlParent,
		DCX_BOXCLASS,
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

	m_pLayoutManager = std::make_unique<LayoutManager>(m_Hwnd);

	setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);

	if (Dcx::UXModule.isUseable())
		_hTheme = Dcx::UXModule.dcxOpenThemeData(m_Hwnd, VSCLASS_BUTTON);
}

/*!
* \brief blah
*
* blah
*/

DcxBox::~DcxBox()
{
	if (_hTheme)
		Dcx::UXModule.dcxCloseThemeData(_hTheme);
}

/// <summary>
/// Parse a string into the controls styles.
/// </summary>
/// <param name="tsStyles"></param>
/// <returns></returns>
dcxWindowStyles DcxBox::parseControlStyles(const TString& tsStyles)
{
	m_iBoxStyles = 0;

	auto ws = parseGeneralControlStyles(tsStyles);

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"right"_hash:
			m_iBoxStyles |= BOXS_RIGHT;
			break;
		case L"center"_hash:
			m_iBoxStyles |= BOXS_CENTER;
			break;
		case L"bottom"_hash:
			m_iBoxStyles |= BOXS_BOTTOM;
			break;
		case L"none"_hash:
			m_iBoxStyles |= BOXS_NONE;
			break;
		case L"rounded"_hash:
			m_iBoxStyles |= BOXS_ROUNDED;
			break;
		case L"check"_hash:
		{
			m_iBoxStyles &= ~BOXS_RADIO;
			m_iBoxStyles |= BOXS_CHECK;
		}
		break;
		case L"radio"_hash:
		{
			m_iBoxStyles &= ~BOXS_CHECK;
			m_iBoxStyles |= BOXS_RADIO;
		}
		break;
		default:
			break;
		}
	}

	return ws;
}

TString DcxBox::parseInfoRequest(const TString& input) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"text"_hash:
		return TGetWindowText(m_Hwnd);
	case L"inbox"_hash:
	{
		RECT rc{};
		if (!GetClientRect(m_Hwnd, &rc))
			throw Dcx::dcxException("Unable to get client rect!");

		InflateRect(&rc, -2, -2);
		if (GetWindowTextLength(m_Hwnd) > 0)
		{
			const auto hdc = GetDC(m_Hwnd);
			if (!hdc)
				throw Dcx::dcxException("Unable to get windows DC");

			Auto(ReleaseDC(m_Hwnd, hdc));

			HFONT oldFont = nullptr;
			RECT rcText = rc;

			if (const auto f = getControlFont(); f)
				oldFont = Dcx::dcxSelectObject<HFONT>(hdc, f);

			const TString text(TGetWindowText(m_Hwnd));
			DrawText(hdc, text.to_chr(), gsl::narrow_cast<int>(text.len()), &rcText, DT_CALCRECT); // we only want the height, so calling DrawText() with DT_CALCRECT is fine.

			if (oldFont)
				Dcx::dcxSelectObject<HFONT>(hdc, oldFont);

			if (const auto h = rcText.bottom - rcText.top; dcx_testflag(m_iBoxStyles, BOXS_BOTTOM))
				rc.bottom -= (h + 2);
			else
				rc.top += (h - 2);
		}

		TString tsRes;
		tsRes.tsprintf(TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
		return tsRes;
	}
	break;
	default:
		return parseGlobalInfoRequest(input);
		break;
	}
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */
void DcxBox::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	szReturnValue = parseInfoRequest(input).to_chr();
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxBox::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if (flags[TEXT('c')])
	{
		if (numtok < 9)
			throw DcxExceptions::dcxInvalidArguments();

		getParentDialog()->addControl(input, 4, DcxAllowControls::ALLOW_ALL, m_Hwnd);
		redrawWindow();
	}
	// xdid -d [NAME] [ID] [SWITCH] [ID]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto pDialog = getParentDialog();
		const auto tsID(input.getnexttok());	// tok 4
		const auto ID = pDialog->NameToID(tsID);

		if (!pDialog->isIDValid(ID))
			throw Dcx::dcxException(TEXT("Unknown control with ID %(%) (dialog %)"), tsID, ID - mIRC_ID_OFFSET, pDialog->getName());

		const auto p_Control = pDialog->getControlByID(ID);

		if (!p_Control)
			throw Dcx::dcxException(TEXT("Unable to get control with ID %(%) (dialog %)"), tsID, ID - mIRC_ID_OFFSET, pDialog->getName());

		if (const auto dct = p_Control->getControlType(); (dct == DcxControlTypes::DIALOG || dct == DcxControlTypes::WINDOW))
			delete p_Control;
		else {
			if (p_Control->getRefCount() != 0)
				throw Dcx::dcxException(TEXT("Can't delete control with ID \"%\" when it is inside it's own event (dialog %)"), p_Control->getUserID(), pDialog->getName());

			auto cHwnd = p_Control->getHwnd();
			pDialog->deleteControl(p_Control); // remove from internal list!
			DestroyWindow(cHwnd);
		}
	}
	/*
	//xdid -l [NAME] [ID] [SWITCH] [OPTIONS]

	[OPTIONS] :

	root [TAB]+flpiw [ID] [WEIGHT] [W] [H]
	add PATH[TAB]+flpiw [ID] [WEIGHT] [W] [H]
	space PATH[TAB]+ [L] [T] [R] [B]
	*/
	else if (flags[TEXT('l')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		if (m_pLayoutManager == nullptr)
			throw Dcx::dcxException("No LayoutManager available");

		switch (std::hash<TString>{}(input.getnexttok()))	// tok 4
		{
		case L"update"_hash:
		{
			RECT rc{};
			if (!GetClientRect(m_Hwnd, &rc))
				throw Dcx::dcxException("Unable to get client rect!");

			m_pLayoutManager->updateLayout(rc);

			redrawWindow();
		}
		break;
		case L"clear"_hash:
		{
			// dont redraw here, leave that for an `update` cmd

			m_pLayoutManager = std::make_unique<LayoutManager>(m_Hwnd);
		}
		break;
		default:
			if (numtok > 8)
				m_pLayoutManager->AddCell(input, 4, this->getParentDialog());
		}
	}
	//xdid -t [NAME] [ID] [SWITCH]
	else if (flags[TEXT('t')])
	{
		SetWindowText(m_Hwnd, input.getlasttoks().trim().to_chr());	// tok 4, -1
		redrawWindow();
	}
	else
		parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL CALLBACK DcxBox::EnumBoxChildren(HWND hwnd, const DCXENUM* const de) noexcept
{
	if (de && hwnd && (de->mChildHwnd != hwnd) && (GetParent(hwnd) == de->mBox))
		EnableWindow(hwnd, de->mState);

	return TRUE;
}

void DcxBox::toXml(TiXmlElement* const xml) const
{
	if (!xml)
		return;

	const TString wtext(TGetWindowText(m_Hwnd));
	__super::toXml(xml);
	xml->SetAttribute("caption", wtext.c_str());
	xml->SetAttribute("styles", getStyles().c_str());

	if (m_pLayoutManager)
	{
		if (const auto rt = m_pLayoutManager->getRoot(); rt)
		{
			rt->toXml(xml);
			return;
		}
	}
	for (auto hChild = GetWindow(m_Hwnd, GW_CHILD); hChild; hChild = GetWindow(hChild, GW_HWNDNEXT))
	{
		auto pthis = Dcx::dcxGetProp<DcxControl*>(hChild, TEXT("dcx_cthis"));
		if (!pthis)
			return;

		const Dcx::dcxWindowRect rc(hChild, m_Hwnd);

		if (auto xctrl = pthis->toXml(); xctrl)
		{
			xctrl->SetAttribute("weight", 1);
			xctrl->SetAttribute("x", rc.left);
			xctrl->SetAttribute("y", rc.top);
			xctrl->SetAttribute("height", rc.Height());
			xctrl->SetAttribute("width", rc.Width());

			xml->LinkEndChild(xctrl);
		}
	}
}

GSL_SUPPRESS(lifetime.4)
TiXmlElement* DcxBox::toXml() const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

std::unique_ptr<TiXmlElement> DcxBox::toXml(int blah) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml;
}

const TString DcxBox::getStyles(void) const
{
	auto result(__super::getStyles());

	if (dcx_testflag(this->m_iBoxStyles, BOXS_RIGHT))
		result.addtok(TEXT("right"));
	if (dcx_testflag(this->m_iBoxStyles, BOXS_CENTER))
		result.addtok(TEXT("center"));
	if (dcx_testflag(this->m_iBoxStyles, BOXS_BOTTOM))
		result.addtok(TEXT("bottom"));
	if (dcx_testflag(this->m_iBoxStyles, BOXS_NONE))
		result.addtok(TEXT("none"));
	if (dcx_testflag(this->m_iBoxStyles, BOXS_ROUNDED))
		result.addtok(TEXT("rounded"));
	if (dcx_testflag(this->m_iBoxStyles, BOXS_CHECK))
		result.addtok(TEXT("check"));
	else if (dcx_testflag(this->m_iBoxStyles, BOXS_RADIO))
		result.addtok(TEXT("radio"));

	return result;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxBox::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept
{
	return 0L;
}

LRESULT DcxBox::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (!hdr)
			break;

		if (IsWindow(hdr->hwndFrom))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(hdr->hwndFrom, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_HSCROLL:
	case WM_VSCROLL:
	case WM_COMMAND:
	{
		if (lParam == 0L)
			break;

		if (IsWindow(reinterpret_cast<HWND>(lParam)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(lParam, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_COMPAREITEM:
	{
		if (dcxlParam(LPCOMPAREITEMSTRUCT, idata); ((idata) && (IsWindow(idata->hwndItem))))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(lParam, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_DELETEITEM:
	{
		if (dcxlParam(LPDELETEITEMSTRUCT, idata); ((idata) && (IsWindow(idata->hwndItem))))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(lParam, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_MEASUREITEM:
	{
		if (const auto cHwnd = GetDlgItem(m_Hwnd, gsl::narrow_cast<int>(wParam)); IsWindow(cHwnd))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(cHwnd, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_DRAWITEM:
	{
		if (dcxlParam(LPDRAWITEMSTRUCT, idata); ((idata) && (IsWindow(idata->hwndItem))))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(idata->hwndItem, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_SIZE:
	{
		HandleChildControlSize();

		//if (m_pLayoutManager)
		//{
		//	RECT rc{ 0, 0, LOWORD( lParam ), HIWORD( lParam ) };
		//	if (m_pLayoutManager->updateLayout( rc ))
		//		redrawWindow( );
		//}
	}
	break;

	case WM_WINDOWPOSCHANGING:
	{
		dcxlParam(LPWINDOWPOS, wp);

		if (!wp)
			break;

		if (m_pLayoutManager && !dcx_testflag(wp->flags, SWP_HIDEWINDOW))
		{
			RECT rc{ 0, 0, wp->cx, wp->cy };
			m_pLayoutManager->updateLayout(rc);
		}
	}
	break;

	case WM_ENABLE:
	{
		redrawWindow();
	}
	break;

	case WM_ERASEBKGND:
	{
		EraseBackground(reinterpret_cast<HDC>(wParam));
		bParsed = TRUE;
		return TRUE;
	}
	break;

	case WM_PRINTCLIENT:
	{
		DrawClientArea(reinterpret_cast<HDC>(wParam));
		bParsed = TRUE;
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps{};

		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		DrawClientArea(hdc);

		bParsed = TRUE;
		return 0L;
	}
	break;

	case WM_THEMECHANGED:
	{
		if (_hTheme)
		{
			Dcx::UXModule.dcxCloseThemeData(_hTheme);
			_hTheme = Dcx::UXModule.dcxOpenThemeData(m_Hwnd, VSCLASS_BUTTON);
		}
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
		if (dcx_testflag(m_iBoxStyles, BOXS_CHECK) || dcx_testflag(m_iBoxStyles, BOXS_RADIO))
		{
#if DCX_USE_WRAPPERS
			if (const Dcx::dcxCursorPos pt(m_Hwnd); pt)
			{
				if (PtInRect(&m_rcCheck, pt))
				{
					// clicked inside button rect
					m_bTitleChecked = m_bTitleChecked ? false : true;
					if (HDC hdc = GetDC(m_Hwnd); hdc)
					{
						DrawCheckButton(hdc, &m_rcCheck);

						ReleaseDC(m_Hwnd, hdc);
					}
					if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
					{
						const stString<10> sRet;

						evalAliasEx(sRet, gsl::narrow_cast<int>(sRet.size()), TEXT("checkchange,%u,%d"), getUserID(), m_bTitleChecked);

						if (sRet == TEXT("nochange"))
							return 0L;
					}

					const DCXENUM de{ nullptr,m_Hwnd,m_bTitleChecked };

#pragma warning(push)
#pragma warning(disable: 4191)

					EnumChildWindows(m_Hwnd, (WNDENUMPROC)DcxBox::EnumBoxChildren, (LPARAM)&de);

#pragma warning(pop)
					// stop further proccessing of message.
					return 0L;
				}
			}
#else
			if (POINT pt{}; GetCursorPos(&pt))
			{
				if (MapWindowPoints(nullptr, m_Hwnd, &pt, 1))
				{
					if (PtInRect(&m_rcCheck, pt))
					{
						// clicked inside button rect
						m_bTitleChecked = m_bTitleChecked ? false : true;
						if (HDC hdc = GetDC(m_Hwnd); hdc)
						{
							DrawCheckButton(hdc, &m_rcCheck);

							ReleaseDC(m_Hwnd, hdc);
						}
						if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
						{
							const stString<10> sRet;

							evalAliasEx(sRet, gsl::narrow_cast<int>(sRet.size()), TEXT("checkchange,%u,%d"), getUserID(), m_bTitleChecked);

							if (sRet == TEXT("nochange"))
								return 0L;
						}

						const DCXENUM de{ nullptr,m_Hwnd,m_bTitleChecked };

#pragma warning(push)
#pragma warning(disable: 4191)

						EnumChildWindows(m_Hwnd, (WNDENUMPROC)DcxBox::EnumBoxChildren, (LPARAM)&de);

#pragma warning(pop)
						// stop further proccessing of message.
						return 0L;
					}
				}
			}
#endif
		}
	}
	[[fallthrough]];
	default:
		lRes = CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}

/// <summary>
/// Draw controls background.
/// </summary>
/// <param name="hdc"></param>
void DcxBox::EraseBackground(HDC hdc)
{
	if (RECT rc{}; GetClientRect(m_Hwnd, &rc))
	{
		// fill background.
		if (isExStyle(WindowExStyle::Transparent) || getParentDialog()->isExStyle(WindowExStyle::Composited))
			DrawParentsBackground(hdc, &rc);
		else // normal bkg
			DcxControl::DrawCtrlBackground(hdc, this, &rc);
		// Update CLA if any.
		if (m_pLayoutManager)
			m_pLayoutManager->updateLayout(rc);
	}
}

/// <summary>
/// Draw control.
/// </summary>
/// <param name="hdc"></param>
void DcxBox::DrawClientArea(HDC hdc)
{
	RECT rc{}, rc2{}, rcText{}, rcText2{};

	if (!GetClientRect(m_Hwnd, &rc))
		return;

	// Setup alpha blend if any.
	const auto ai = SetupAlphaBlend(&hdc, true);
	Auto(FinishAlphaBlend(ai));

	// if no border, dont bother
	if (dcx_testflag(m_iBoxStyles, BOXS_NONE))
	{
		DcxControl::DrawCtrlBackground(hdc, this, &rc);
		return;
	}

	CopyRect(&rc2, &rc);

	SetBkMode(hdc, TRANSPARENT);

	// no text, no box!
	if (const TString wtext(TGetWindowText(m_Hwnd)); wtext.empty())
	{
		DrawBorder(hdc, rc2);
	}
	// draw text
	else {
		// prepare for appearance
		if (const auto f = getControlFont(); f)
			Dcx::dcxSelectObject<HFONT>(hdc, f);

		if (m_clrText != CLR_INVALID)
			SetTextColor(hdc, m_clrText);
		else
			SetTextColor(hdc, GetSysColor(IsWindowEnabled(m_Hwnd) ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));

		CopyRect(&rcText, &rc); // MUST initialize rect first!.

		calcTextRect(hdc, wtext, &rcText, DT_LEFT | DT_END_ELLIPSIS | DT_SINGLELINE);
		if (IsShadowTextEnabled())
		{
			rcText.bottom = std::min((rcText.bottom + 6), rc.bottom);
			rcText.right = std::min((rcText.right + 6), rc.right);
		}

		const auto w = rcText.right - rcText.left;
		const auto h = rcText.bottom - rcText.top;

		// shift border and text locations
		// text at bottom?
		if (dcx_testflag(this->m_iBoxStyles, BOXS_BOTTOM))
		{
			rcText.top = rc2.bottom - h;
			rc2.bottom -= h / 2;
		}
		// text at top
		else {
			rcText.top = rc2.top;
			rc2.top += h / 2;
		}

		// text location
		rcText.bottom = rcText.top + h;

		// align text horizontally
		if (const auto bw = (rc.right - rc.left - (2 * DCX_BOXTEXTSPACING)); w > bw)
		{
			rcText.left = rc.left + DCX_BOXTEXTSPACING;
			rcText.right = rc.right - DCX_BOXTEXTSPACING;
		}
		else {
			if (dcx_testflag(this->m_iBoxStyles, BOXS_RIGHT))
				rcText.left = rc.right - DCX_BOXTEXTSPACING - w;
			else if (dcx_testflag(this->m_iBoxStyles, BOXS_CENTER))
				rcText.left = (rc.left + rc.right - w) / 2;
			else
				rcText.left = rc.left + DCX_BOXTEXTSPACING;

			rcText.right = rcText.left + w;
		}

		// clear some space for the text
		CopyRect(&rcText2, &rcText);
		InflateRect(&rcText2, 3, 0);

		RECT rcClip = rcText2;

		if (dcx_testflag(m_iBoxStyles, BOXS_CHECK) || dcx_testflag(m_iBoxStyles, BOXS_RADIO))
		{
			const int halfheight = ((rcText2.bottom - rcText2.top) / 2);
			const int dy = halfheight / 2;
			const int iSize = std::max<int>(18, halfheight);
			const RECT rcCheck{ rcText2.left, rcText2.top + dy, rcText2.left + iSize, rcText2.top + iSize };

			OffsetRect(&rcText, (rcCheck.right - rcCheck.left), 0);

			m_rcCheck = rcCheck;

			rcClip.right += (rcCheck.right - rcCheck.left);

			// draw the border
			DrawBorder(hdc, rc2, &rcClip);

			// draw check/radio box
			DrawCheckButton(hdc, &m_rcCheck);
		}
		else
			// draw the border
			DrawBorder(hdc, rc2, &rcClip);

		// draw the text
		this->ctrlDrawText(hdc, wtext, &rcText, DT_LEFT | DT_END_ELLIPSIS | DT_SINGLELINE);
	}
}

/// <summary>
/// The class defaulty WindowProc
/// </summary>
/// <param name="uMsg"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
LRESULT DcxBox::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}

/// <summary>
/// Draw the control border.
/// </summary>
/// <param name="hdc"></param>
/// <param name="rc"></param>
/// <param name="rcClip"></param>
/// <returns></returns>
void DcxBox::DrawBorder(HDC hdc, RECT& rc, const LPRECT rcClip) noexcept
{
	if (dcx_testflag(m_iBoxStyles, BOXS_ROUNDED))
	{
		if (const auto m_Region = CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 10, 10); m_Region)
		{
			Auto(DeleteObject(m_Region));

			SelectClipRgn(hdc, m_Region);
			DcxControl::DrawCtrlBackground(hdc, this, &rc);

			if (HRGN hClipRgn = CreateClipRegion(rcClip); hClipRgn)
			{
				Auto(DeleteRgn(hClipRgn));

				CombineRgn(hClipRgn, hClipRgn, m_Region, RGN_AND);

				SelectClipRgn(hdc, hClipRgn);

				const auto hBorderBrush = (m_hBorderBrush) ? m_hBorderBrush : Dcx::dcxGetStockObject<HBRUSH>(BLACK_BRUSH);

				FrameRgn(hdc, m_Region, hBorderBrush, 1, 1);

				SelectClipRgn(hdc, nullptr);
			}
			else {
				SelectClipRgn(hdc, nullptr);

				const auto hBorderBrush = (m_hBorderBrush) ? m_hBorderBrush : Dcx::dcxGetStockObject<HBRUSH>(BLACK_BRUSH);

				FrameRgn(hdc, m_Region, hBorderBrush, 1, 1);
			}
		}
	}
	else {
		DcxControl::DrawCtrlBackground(hdc, this, &rc);

		if (HRGN hClipRgn = CreateClipRegion(rcClip); hClipRgn)
		{
			Auto(DeleteRgn(hClipRgn));

			SelectClipRgn(hdc, hClipRgn);

			DrawEdge(hdc, &rc, EDGE_ETCHED, BF_RECT);

			SelectClipRgn(hdc, nullptr);
		}
		else
			DrawEdge(hdc, &rc, EDGE_ETCHED, BF_RECT);
	}
}

/// <summary>
/// Create a clip region for the control.
/// </summary>
/// <param name="rcClip"></param>
/// <returns></returns>
HRGN DcxBox::CreateClipRegion(LPCRECT rcClip) noexcept
{
	if (!rcClip)
		return nullptr;

	if (HRGN hClipRgn = CreateRectRgn(rcClip->left, rcClip->top, rcClip->right, rcClip->bottom); hClipRgn)
	{
		if (RECT rcWin{}; GetClientRect(m_Hwnd, &rcWin))
		{
			if (auto hWinRgn = CreateRectRgn(rcWin.left, rcWin.top, rcWin.right, rcWin.bottom); hWinRgn)
			{
				Auto(DeleteRgn(hWinRgn));

				CombineRgn(hClipRgn, hClipRgn, hWinRgn, RGN_XOR);
			}
		}
		return hClipRgn;
	}
	return nullptr;
}

/// <summary>
/// Draw the controls check/radio box.
/// </summary>
/// <param name="hdc"></param>
/// <param name="rcCheck"></param>
/// <returns></returns>
void DcxBox::DrawCheckButton(HDC hdc, LPRECT rcCheck) noexcept
{
	const auto bEnabled = IsWindowEnabled(m_Hwnd);

	UINT uStyles = DFCS_TRANSPARENT;

	if (dcx_testflag(m_iBoxStyles, BOXS_RADIO))
		uStyles |= DFCS_BUTTONRADIO;
	else
		uStyles |= DFCS_BUTTONCHECK;

	if (m_bTitleChecked)
		uStyles |= DFCS_CHECKED;

	if (!bEnabled)
		uStyles |= DFCS_INACTIVE;

	DrawFrameControl(hdc, rcCheck, DFC_BUTTON, uStyles);
}