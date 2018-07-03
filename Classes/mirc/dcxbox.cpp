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

DcxBox::DcxBox(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
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

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	// remove all borders
	removeStyle(WindowStyle::Border | WS_DLGFRAME);
	removeExStyle(WindowExStyle::ClientEdge | WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE);

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	m_pLayoutManager = std::make_unique<LayoutManager>(m_Hwnd);

	setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);

	//if (dcx_testflag(m_iBoxStyles, BOXS_CHECK) || dcx_testflag(m_iBoxStyles, BOXS_RADIO))
	//{
	//	WindowStyle ButtonStyles(WindowStyle::Child | WS_VISIBLE | WS_CLIPSIBLINGS);
	//	
	//	if (dcx_testflag(m_iBoxStyles, BOXS_CHECK))
	//		ButtonStyles |= BS_AUTOCHECKBOX;
	//	else if (dcx_testflag(m_iBoxStyles, BOXS_RADIO))
	//		ButtonStyles |= BS_AUTORADIOBUTTON;
	//	
	//	m_TitleButton = CreateWindowEx(
	//		gsl::narrow_cast<DWORD>(ExStyles),
	//		WC_BUTTON,
	//		nullptr,
	//		gsl::narrow_cast<DWORD>(ButtonStyles),
	//		CW_USEDEFAULT, CW_USEDEFAULT, 11, 10,
	//		m_Hwnd,
	//		(HMENU)ID,
	//		GetModuleHandle(nullptr),
	//		nullptr);
	//	
	//	if (!IsWindow(m_TitleButton))
	//		throw Dcx::dcxException("Unable To Create Check Window");
	//	
	//	if (bNoTheme)
	//		Dcx::UXModule.dcxSetWindowTheme(m_TitleButton, L" ", L" ");
	//	if (!dcx_testflag(Styles, WS_DISABLED))
	//		SendMessage(m_TitleButton, BM_SETCHECK, BST_CHECKED, 0L);
	//}

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
	if (_hTheme != nullptr)
		Dcx::UXModule.dcxCloseThemeData(_hTheme);
}

/*!
* \brief blah
*
* blah
*/

//void DcxBox::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
//{
//	m_iBoxStyles = 0;
//
//	for (const auto &tsStyle : styles)
//	{
//		switch (std::hash<TString>{}(tsStyle))
//		{
//			case L"right"_hash:
//				m_iBoxStyles |= BOXS_RIGHT;
//				break;
//			case L"center"_hash:
//				m_iBoxStyles |= BOXS_CENTER;
//				break;
//			case L"bottom"_hash:
//				m_iBoxStyles |= BOXS_BOTTOM;
//				break;
//			case L"none"_hash:
//				m_iBoxStyles |= BOXS_NONE;
//				break;
//			case L"rounded"_hash:
//				m_iBoxStyles |= BOXS_ROUNDED;
//				break;
//			case L"check"_hash: {
//					m_iBoxStyles &= ~BOXS_RADIO;
//					m_iBoxStyles |= BOXS_CHECK;
//				}
//				break;
//			case L"radio"_hash: {
//					m_iBoxStyles &= ~BOXS_CHECK;
//					m_iBoxStyles |= BOXS_RADIO;
//				}
//				break;
//			case L"transparent"_hash:
//				*ExStyles |= WS_EX_TRANSPARENT;
//			default:
//				break;
//		}
//	}
//
//	parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
//}

dcxWindowStyles DcxBox::parseControlStyles(const TString & tsStyles)
{
	m_iBoxStyles = 0;

	auto ws = parseGeneralControlStyles(tsStyles);

	for (const auto &tsStyle : tsStyles)
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

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxBox::parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"text"_hash:
		GetWindowText(m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH);
		break;
	case L"inbox"_hash:
	{
		RECT rc{};
		if (!GetClientRect(m_Hwnd, &rc))
			throw Dcx::dcxException("Unable to get client rect!");

		InflateRect(&rc, -2, -2);
		if (GetWindowTextLength(m_Hwnd) > 0)
		{
			const auto hdc = GetDC(m_Hwnd);
			if (hdc == nullptr)
				throw Dcx::dcxException("Unable to get windows DC");

			Auto(ReleaseDC(m_Hwnd, hdc));

			HFONT oldFont = nullptr;
			RECT rcText = rc;

			if (const auto f = getControlFont(); f != nullptr)
				oldFont = SelectFont(hdc, f);

			const TString text(TGetWindowText(m_Hwnd));
			DrawText(hdc, text.to_chr(), gsl::narrow_cast<int>(text.len()), &rcText, DT_CALCRECT);

			if (oldFont != nullptr)
				SelectFont(hdc, oldFont);

			if (const auto h = rcText.bottom - rcText.top; dcx_testflag(m_iBoxStyles, BOXS_BOTTOM))
				rc.bottom -= (h + 2);
			else
				rc.top += (h - 2);
		}

		_ts_snprintf(szReturnValue, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
		break;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxBox::parseCommandRequest(const TString & input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if (flags[TEXT('c')])
	{
		if (numtok < 9)
			throw Dcx::dcxException("Insufficient parameters");

		getParentDialog()->addControl(input, 4, CTLF_ALLOW_ALL, m_Hwnd);
		redrawWindow();
	}
	// xdid -d [NAME] [ID] [SWITCH] [ID]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const auto tsID(input.getnexttok());	// tok 4
		const auto ID = getParentDialog()->NameToID(tsID);

		if (!getParentDialog()->isIDValid(ID))
			throw Dcx::dcxException(TEXT("Unknown control with ID \"%\" (dialog %)"), ID - mIRC_ID_OFFSET, getParentDialog()->getName());

		const auto p_Control = getParentDialog()->getControlByID(ID);

		if (p_Control == nullptr)
			throw Dcx::dcxException("Unable to get control");

		if (const auto dct = p_Control->getControlType(); (dct == DcxControlTypes::DIALOG || dct == DcxControlTypes::WINDOW))
			delete p_Control;
		else {
			if (p_Control->getRefCount() != 0)
				throw Dcx::dcxException(TEXT("Can't delete control with ID \"%\" when it is inside it's own event (dialog %)"), p_Control->getUserID(), this->getParentDialog()->getName());

			auto cHwnd = p_Control->getHwnd();
			getParentDialog()->deleteControl(p_Control); // remove from internal list!
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
			throw Dcx::dcxException("Insufficient parameters");

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
			//delete m_pLayoutManager;
			//m_pLayoutManager = new LayoutManager(m_Hwnd);
			////redrawWindow(); // dont redraw here, leave that for an `update` cmd

			//m_pLayoutManager.reset(new LayoutManager(m_Hwnd));
			m_pLayoutManager = std::make_unique<LayoutManager>(m_Hwnd);
		}
		break;
		default:
			if (numtok > 8)
				m_pLayoutManager->AddCell(input, 4);
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

BOOL CALLBACK DcxBox::EnumBoxChildren(HWND hwnd, const DCXENUM *const de) noexcept
{
	if ((de->mChildHwnd != hwnd) && (GetParent(hwnd) == de->mBox))
		EnableWindow(hwnd, de->mState);

	return TRUE;
}

void DcxBox::toXml(TiXmlElement *const xml) const
{
	const TString wtext(TGetWindowText(m_Hwnd));
	__super::toXml(xml);
	xml->SetAttribute("caption", wtext.c_str());
	xml->SetAttribute("styles", getStyles().c_str());

	if (m_pLayoutManager)
		m_pLayoutManager->getRoot()->toXml(xml);
}

TiXmlElement * DcxBox::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
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
LRESULT DcxBox::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) noexcept
{
	return 0L;
}

LRESULT DcxBox::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed)
{
	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (hdr == nullptr)
			break;

		if (IsWindow(hdr->hwndFrom))
		{
			if (const auto c_this = reinterpret_cast<DcxControl *>(GetProp(hdr->hwndFrom, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	//	case WM_COMMAND:
	//	{
	//		HWND cmdHwnd = (HWND)lParam;
	//
	//		if (IsWindow(cmdHwnd))
	//		{
	//			if (cmdHwnd == m_TitleButton)
	//			{
	//				switch (HIWORD(wParam))
	//				{
	//				case BN_CLICKED:
	//				{
	//					const auto state = (SendMessage(m_TitleButton, BM_GETCHECK, 0, 0) == BST_CHECKED);
	//					if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
	//					{
	//						stString<10> sRet;
	//
	//						evalAliasEx(sRet, gsl::narrow_cast<int>(sRet.size()), TEXT("checkchange,%u,%d"), getUserID(), state);
	//
	//						if (sRet == TEXT("nochange"))
	//							return 0L;
	//					}
	//
	//					const DCXENUM de{ m_TitleButton,m_Hwnd,state };
	//
	//#pragma warning(push)
	//#pragma warning(disable: 4191)
	//
	//					EnumChildWindows(m_Hwnd, (WNDENUMPROC)DcxBox::EnumBoxChildren, (LPARAM)&de);
	//
	//#pragma warning(pop)
	//					break;
	//				}
	//				} // end switch
	//				break;
	//			}
	//			else {
	//				if (auto c_this = reinterpret_cast<DcxControl *>(GetProp(cmdHwnd, TEXT("dcx_cthis"))); c_this != nullptr)
	//					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
	//			}
	//		}
	//	}
	//	break;

		//case WM_LBUTTONUP:
		//{
		//	if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
		//	{
		//		execAliasEx(TEXT("lbup,%u"), getUserID());
		//		execAliasEx(TEXT("sclick,%u"), getUserID());
		//	}
		//}
		//break;

	case WM_HSCROLL:
	case WM_VSCROLL:
	case WM_COMMAND:
	{
		if (lParam == 0L)
			break;

		if (IsWindow((HWND)lParam))
		{
			if (const auto c_this = reinterpret_cast<DcxControl *>(GetProp((HWND)lParam, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_COMPAREITEM:
	{
		if (dcxlParam(LPCOMPAREITEMSTRUCT, idata); ((idata != nullptr) && (IsWindow(idata->hwndItem))))
		{
			if (const auto c_this = reinterpret_cast<DcxControl *>(GetProp((HWND)lParam, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_DELETEITEM:
	{
		if (dcxlParam(LPDELETEITEMSTRUCT, idata); ((idata != nullptr) && (IsWindow(idata->hwndItem))))
		{
			if (const auto c_this = reinterpret_cast<DcxControl *>(GetProp((HWND)lParam, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_MEASUREITEM:
	{
		if (const auto cHwnd = GetDlgItem(m_Hwnd, gsl::narrow_cast<int>(wParam)); IsWindow(cHwnd))
		{
			if (const auto c_this = reinterpret_cast<DcxControl *>(GetProp(cHwnd, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_DRAWITEM:
	{
		if (dcxlParam(LPDRAWITEMSTRUCT, idata); ((idata != nullptr) && (IsWindow(idata->hwndItem))))
		{
			if (const auto c_this = reinterpret_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_SIZE:
	{
		HandleChildrenSize();

		//if (m_pLayoutManager != nullptr)
		//{
		//	RECT rc{ 0, 0, LOWORD( lParam ), HIWORD( lParam ) };
		//	if (m_pLayoutManager->updateLayout( rc ))
		//		redrawWindow( );
		//}
	}
	break;
	case WM_WINDOWPOSCHANGING:
	{
		if (lParam == 0L)
			break;

		if (m_pLayoutManager != nullptr)
		{
			dcxlParam(LPWINDOWPOS, wp);

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
		EraseBackground((HDC)wParam);
		bParsed = TRUE;
		return TRUE;
	}
	break;

	case WM_PRINTCLIENT:
	{
		DrawClientArea((HDC)wParam);
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
		if (_hTheme != nullptr)
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

	default:
		lRes = CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}

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
		if (m_pLayoutManager != nullptr)
			m_pLayoutManager->updateLayout(rc);
	}
}

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

		//if (IsWindow(m_TitleButton))
		//	SetWindowPos(m_TitleButton,nullptr,rc2.left,rc2.top,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	}
	// draw text
	else {
		// prepare for appearance
		if (const auto f = getControlFont(); f != nullptr)
			SelectFont(hdc, f);

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

		//if (IsWindow(this->m_TitleButton))
		//{
		//	if (RECT bSZ{}; GetWindowRect(this->m_TitleButton, &bSZ))
		//	{
		//		bSZ.bottom = (bSZ.right - bSZ.left);
		//		SetWindowPos(this->m_TitleButton, nullptr, rcText2.left, rcText2.top, bSZ.bottom, (rcText2.bottom - rcText2.top), SWP_NOZORDER | SWP_NOACTIVATE);
		//		rcText.left += bSZ.bottom;
		//		rcText.right += bSZ.bottom;
		//		rcText2.left += bSZ.bottom;
		//		rcText2.right += bSZ.bottom;
		//	}
		//}

		if (dcx_testflag(m_iBoxStyles, BOXS_CHECK))
		{
			const int halfheight = ((rcText2.bottom - rcText2.top) / 2);
			const int dy = halfheight / 2;
			const int iSize = std::max<int>(18, halfheight);
			RECT rcCheck{ rcText2.left, rcText2.top + dy, rcText2.left + iSize, rcText2.top + iSize };

			const auto bEnabled = IsWindowEnabled(m_Hwnd);

			UINT uStyles = DFCS_BUTTONCHECK | DFCS_TRANSPARENT;

			if (m_bTitleChecked)
				uStyles |= DFCS_CHECKED;

			if (!bEnabled)
				uStyles |= DFCS_INACTIVE;

			DrawFrameControl(hdc, &rcCheck, DFC_BUTTON, uStyles);

			rcText.left += rcCheck.bottom;
			rcText.right += rcCheck.bottom;
			//rcText2.left += rcCheck.bottom;
			//rcText2.right += rcCheck.bottom;
		}

		// draw the border
		DrawBorder(hdc, rc2);

		SelectClipRgn(hdc, nullptr);

		// draw the text
		this->ctrlDrawText(hdc, wtext, &rcText, DT_LEFT | DT_END_ELLIPSIS | DT_SINGLELINE);
	}
}

WNDPROC DcxBox::m_hDefaultClassProc = nullptr;

LRESULT DcxBox::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc != nullptr)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}

void DcxBox::DrawBorder(HDC hdc, RECT & rc) noexcept
{
	if (dcx_testflag(m_iBoxStyles, BOXS_ROUNDED))
	{
		if (const auto m_Region = CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 10, 10); m_Region != nullptr)
		{
			Auto(DeleteRgn(m_Region));

			SelectClipRgn(hdc, m_Region);
			DcxControl::DrawCtrlBackground(hdc, this, &rc);
			SelectClipRgn(hdc, nullptr);

			const auto hBorderBrush = m_hBorderBrush != nullptr ? m_hBorderBrush : GetStockBrush(BLACK_BRUSH);

			FrameRgn(hdc, m_Region, hBorderBrush, 1, 1);
		}
	}
	else {
		DcxControl::DrawCtrlBackground(hdc, this, &rc);
		DrawEdge(hdc, &rc, EDGE_ETCHED, BF_RECT);
	}
}
