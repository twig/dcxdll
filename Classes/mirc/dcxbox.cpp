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
	, m_TitleButton(nullptr)
	, _hTheme(nullptr)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;

	parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	m_Hwnd = CreateWindowEx(
		static_cast<DWORD>(ExStyles) | WS_EX_CONTROLPARENT,
		DCX_BOXCLASS,
		nullptr,
		static_cast<DWORD>(Styles) | WS_CHILD,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	// remove all borders
	removeStyle(WS_BORDER | WS_DLGFRAME);
	removeExStyle(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE);

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	m_pLayoutManager = new LayoutManager(m_Hwnd);

	setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);

	if (dcx_testflag(m_iBoxStyles, BOXS_CHECK) || dcx_testflag(m_iBoxStyles, BOXS_RADIO))
	{
		Styles = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;

		if (dcx_testflag(m_iBoxStyles, BOXS_CHECK))
			Styles |= BS_AUTOCHECKBOX;
		else if (dcx_testflag(m_iBoxStyles, BOXS_RADIO))
			Styles |= BS_AUTORADIOBUTTON;

		m_TitleButton = CreateWindowEx(
			static_cast<DWORD>(ExStyles),
			TEXT("BUTTON"),
			nullptr,
			static_cast<DWORD>(Styles),
			CW_USEDEFAULT, CW_USEDEFAULT, 11, 10,
			m_Hwnd,
			(HMENU)ID,
			GetModuleHandle(nullptr),
			nullptr);

		if (IsWindow(m_TitleButton)) {
			if (bNoTheme)
				Dcx::UXModule.dcxSetWindowTheme(m_TitleButton, L" ", L" ");
			if (!dcx_testflag(Styles, WS_DISABLED))
				SendMessage(m_TitleButton, BM_SETCHECK, BST_CHECKED, 0L);
		}
	}
	if (Dcx::UXModule.isUseable())
		_hTheme = Dcx::UXModule.dcxOpenThemeData(m_Hwnd, L"BUTTON");
}

/*!
* \brief blah
*
* blah
*/

DcxBox::~DcxBox() {

	delete m_pLayoutManager;

	if (_hTheme != nullptr)
		Dcx::UXModule.dcxCloseThemeData(_hTheme);
	unregistreDefaultWindowProc();
}

/*!
* \brief blah
*
* blah
*/

void DcxBox::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	m_iBoxStyles = 0;

	for (const auto &tsStyle : styles)
	{
#if DCX_USE_HASHING
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
			case L"check"_hash: {
					m_iBoxStyles &= ~BOXS_RADIO;
					m_iBoxStyles |= BOXS_CHECK;
				}
				break;
			case L"radio"_hash: {
					m_iBoxStyles &= ~BOXS_CHECK;
					m_iBoxStyles |= BOXS_RADIO;
				}
				break;
			case L"transparent"_hash:
				*ExStyles |= WS_EX_TRANSPARENT;
			default:
				break;
		}
#else
#if DCX_SWITCH_OBJ
		Switch(tsStyle)
			.Case(TEXT("right"), [this] { m_iBoxStyles |= BOXS_RIGHT; }).Break()
			.Case(TEXT("center"), [this] { m_iBoxStyles |= BOXS_CENTER; }).Break()
			.Case(TEXT("bottom"), [this] { m_iBoxStyles |= BOXS_BOTTOM; }).Break()
			.Case(TEXT("none"), [this] { m_iBoxStyles |= BOXS_NONE; }).Break()
			.Case(TEXT("rounded"), [this] { m_iBoxStyles |= BOXS_ROUNDED; }).Break()
			.Case(TEXT("check"), [this] { m_iBoxStyles &= ~BOXS_RADIO; m_iBoxStyles |= BOXS_CHECK; }).Break()
			.Case(TEXT("radio"), [this] { m_iBoxStyles &= ~BOXS_CHECK; m_iBoxStyles |= BOXS_RADIO; }).Break()
			.Case(TEXT("transparent"), [ExStyles] { *ExStyles |= WS_EX_TRANSPARENT; }).Break();
#else
		if (tsStyle == TEXT("right"))
			m_iBoxStyles |= BOXS_RIGHT;
		else if (tsStyle == TEXT("center"))
			m_iBoxStyles |= BOXS_CENTER;
		else if (tsStyle == TEXT("bottom"))
			m_iBoxStyles |= BOXS_BOTTOM;
		else if (tsStyle == TEXT("none"))
			m_iBoxStyles |= BOXS_NONE;
		else if (tsStyle == TEXT("rounded"))
			m_iBoxStyles |= BOXS_ROUNDED;
		else if (tsStyle == TEXT("check")) {
			m_iBoxStyles &= ~BOXS_RADIO;
			m_iBoxStyles |= BOXS_CHECK;
		}
		else if (tsStyle == TEXT("radio")) {
			m_iBoxStyles &= ~BOXS_CHECK;
			m_iBoxStyles |= BOXS_RADIO;
		}
		else if (tsStyle == TEXT("transparent"))
			*ExStyles |= WS_EX_TRANSPARENT;
#endif
#endif
	}

	parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

#if DCX_USE_HASHING
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
		RECT rc;
		if (!GetClientRect(m_Hwnd, &rc))
			throw Dcx::dcxException("Unable to get client rect!");

		InflateRect(&rc, -2, -2);
		if (GetWindowTextLength(m_Hwnd) > 0)
		{
			auto hdc = GetDC(m_Hwnd);
			if (hdc == nullptr)
				throw Dcx::dcxException("Unable to get windows DC");

			Auto(ReleaseDC(m_Hwnd, hdc));

			HFONT oldFont = nullptr;
			RECT rcText = rc;

			if (m_hFont != nullptr)
				oldFont = SelectFont(hdc, m_hFont);

			TString text;
			TGetWindowText(m_Hwnd, text);
			DrawText(hdc, text.to_chr(), static_cast<int>(text.len()), &rcText, DT_CALCRECT);

			if (oldFont != nullptr)
				SelectFont(hdc, oldFont);

			//const auto w = rcText.right - rcText.left;
			const auto h = rcText.bottom - rcText.top;

			if (dcx_testflag(m_iBoxStyles, BOXS_BOTTOM))
				rc.bottom -= (h + 2);
			else
				rc.top += (h - 2);
		}

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
		break;
	}
}
#else
#ifndef DCX_SWITCH_OBJ
void DcxBox::parseInfoRequest( const TString & input, refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	const auto prop(input.getfirsttok( 3 ));

	// [NAME] [ID] [PROP]
	if ( prop == TEXT("text") ) {

		GetWindowText( m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH );
	}
	else if ( prop == TEXT("inbox") ) {

		RECT rc;
		if (!GetClientRect(m_Hwnd, &rc))
			throw Dcx::dcxException("Unable to get client rect!");

		InflateRect(&rc, -2, -2);
		if (GetWindowTextLength(m_Hwnd) > 0)
		{
			auto hdc = GetDC(m_Hwnd);
			if (hdc == nullptr)
				throw Dcx::dcxException("Unable to get windows DC");

			Auto(ReleaseDC(m_Hwnd, hdc));

			HFONT oldFont = nullptr;
			RECT rcText = rc;

			if (m_hFont != nullptr)
				oldFont = SelectFont(hdc, m_hFont);

			TString text;
			TGetWindowText(m_Hwnd, text);
			DrawText(hdc, text.to_chr(), static_cast<int>(text.len()), &rcText, DT_CALCRECT);

			if (oldFont != nullptr)
				SelectFont(hdc, oldFont);

			//const auto w = rcText.right - rcText.left;
			const auto h = rcText.bottom - rcText.top;

			if (dcx_testflag(m_iBoxStyles, BOXS_BOTTOM))
				rc.bottom -= (h + 2);
			else
				rc.top += (h - 2);
		}

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	}
	else
		parseGlobalInfoRequest( input, szReturnValue );
}
#else
void DcxBox::parseInfoRequest(const TString & input, refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	Switch(input.getfirsttok(3))
		.Case(L"text"_ts), [this, szReturnValue] { GetWindowText(m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH); }).Break()
		.Case(L"inbox"_ts), [this, szReturnValue] {
			RECT rc;
			if (!GetClientRect(m_Hwnd, &rc))
				throw Dcx::dcxException("Unable to get client rect!");

			InflateRect(&rc, -2, -2);
			if (GetWindowTextLength(m_Hwnd) > 0)
			{
				auto hdc = GetDC(m_Hwnd);
				if (hdc == nullptr)
					throw Dcx::dcxException("Unable to get windows DC");

				Auto(ReleaseDC(m_Hwnd, hdc));

				HFONT oldFont = nullptr;
				RECT rcText = rc;

				if (m_hFont != nullptr)
					oldFont = SelectFont(hdc, m_hFont);

				TString text;
				TGetWindowText(m_Hwnd, text);
				DrawText(hdc, text.to_chr(), static_cast<int>(text.len()), &rcText, DT_CALCRECT);

				if (oldFont != nullptr)
					SelectFont(hdc, oldFont);

				//const auto w = rcText.right - rcText.left;
				const auto h = rcText.bottom - rcText.top;

				if (dcx_testflag(m_iBoxStyles, BOXS_BOTTOM))
					rc.bottom -= (h + 2);
				else
					rc.top += (h - 2);
			}

			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	}).Break()
		.Default([this, &input, szReturnValue] { parseGlobalInfoRequest(input, szReturnValue); });
}
#endif
#endif

/*!
 * \brief blah
 *
 * blah
 */

void DcxBox::parseCommandRequest( const TString & input ) {

	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if ( flags[TEXT('c')] && numtok > 8 ) {

		m_pParentDialog->addControl(input, 4, CTLF_ALLOW_ALL, m_Hwnd);
		redrawWindow();
	}
	// xdid -d [NAME] [ID] [SWITCH] [ID]
	else if ( flags[TEXT('d')] && numtok > 3 ) {

		const auto tsID(input.getnexttok());	// tok 4
		const auto ID = m_pParentDialog->NameToID(tsID);

		if (!m_pParentDialog->isIDValid(ID))
			throw Dcx::dcxException(TEXT("Unknown control with ID \"%\" (dialog %)"), ID - mIRC_ID_OFFSET, m_pParentDialog->getName());

		auto p_Control = m_pParentDialog->getControlByID(ID);

		if (p_Control == nullptr)
			throw Dcx::dcxException("Unable to get control");

		const auto dct = p_Control->getControlType();

		if (dct == DcxControlTypes::DIALOG || dct == DcxControlTypes::WINDOW)
			delete p_Control;
		else {
			if (p_Control->getRefCount() != 0)
				throw Dcx::dcxException(TEXT("Can't delete control with ID \"%\" when it is inside it's own event (dialog %)"), p_Control->getUserID(), this->m_pParentDialog->getName());

			auto cHwnd = p_Control->getHwnd();
			m_pParentDialog->deleteControl(p_Control); // remove from internal list!
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
	else if ( flags[TEXT('l')] && numtok > 3 ) {

#if DCX_USE_HASHING
		if (m_pLayoutManager == nullptr)
			throw Dcx::dcxException("No LayoutManager available");

		switch (std::hash<TString>{}(input.getnexttok()))	// tok 4
		{
		case L"update"_hash:
		{
			RECT rc;
			if (!GetClientRect(m_Hwnd, &rc))
				throw Dcx::dcxException("Unable to get client rect!");

			m_pLayoutManager->updateLayout(rc);

			redrawWindow();
		}
		break;
		case L"clear"_hash:
		{
			delete m_pLayoutManager;
			m_pLayoutManager = new LayoutManager(m_Hwnd);
			//redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
		break;
		default:
			if (numtok > 8)
				m_pLayoutManager->AddCell(input, 4);
		}
#else
		if (m_pLayoutManager == nullptr)
			throw Dcx::dcxException("No LayoutManager available");

		const auto tsCmd(input.getnexttok());	// tok 4

		if ( tsCmd == TEXT("update") ) {
			RECT rc;
			if (!GetClientRect( m_Hwnd, &rc ))
				throw Dcx::dcxException("Unable to get client rect!");
				
			m_pLayoutManager->updateLayout(rc);

			redrawWindow();
		}
		else if (tsCmd == TEXT("clear")) {
			delete m_pLayoutManager;
			m_pLayoutManager = new LayoutManager(m_Hwnd);
			//redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
		else if ( numtok > 8 )
			m_pLayoutManager->AddCell(input, 4);
#endif
	}
	//xdid -t [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('t')] ) {
		SetWindowText(m_Hwnd, input.getlasttoks().trim().to_chr());	// tok 4, -1
		redrawWindow( );
	}
	else
		parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL CALLBACK DcxBox::EnumBoxChildren(HWND hwnd,const DCXENUM *const de)
{
	if ((de->mChildHwnd != hwnd) && (GetParent(hwnd) == de->mBox))
		EnableWindow(hwnd,de->mState);

	return TRUE;
}

void DcxBox::toXml(TiXmlElement *const xml) const
{
	TString wtext;
	TGetWindowText(m_Hwnd, wtext);
	__super::toXml(xml);
	xml->SetAttribute("caption", wtext.c_str());
	xml->SetAttribute("styles", getStyles().c_str());

	if (m_pLayoutManager != nullptr)
		m_pLayoutManager->getRoot()->toXml(xml);
}

TiXmlElement * DcxBox::toXml(void) const
{
	auto xml = __super::toXml();

	TString wtext;
	TGetWindowText(m_Hwnd, wtext);
	xml->SetAttribute("caption", wtext.c_str());
	xml->SetAttribute("styles", getStyles().c_str());

	if (m_pLayoutManager != nullptr)
		m_pLayoutManager->getRoot()->toXml(xml);

	return xml;
}

const TString DcxBox::getStyles(void) const {
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
LRESULT DcxBox::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxBox::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
	switch( uMsg ) {

	case WM_NOTIFY:
		{
			dcxlParam(LPNMHDR, hdr);

			if (hdr == nullptr)
				break;

			if (IsWindow(hdr->hwndFrom)) {
				auto c_this = reinterpret_cast<DcxControl *>(GetProp(hdr->hwndFrom, TEXT("dcx_cthis")));
				if (c_this != nullptr)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

    case WM_COMMAND:
			{
				if (((HWND)lParam != nullptr) && ((HWND)lParam == m_TitleButton) ) {
					switch ( HIWORD( wParam ) )
					{
						case BN_CLICKED:
						{
							const auto state = (SendMessage(m_TitleButton, BM_GETCHECK, 0, 0) == BST_CHECKED);
							if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {
								//TCHAR ret[10];
								//
								//evalAliasEx(ret, Dcx::countof(ret), TEXT("checkchange,%u,%d"), getUserID(), state);
								//
								//if (ts_strcmp(TEXT("nochange"), ret) == 0)
								//	return 0L;

								stString<10> sRet;

								evalAliasEx(sRet, static_cast<int>(sRet.size()), TEXT("checkchange,%u,%d"), getUserID(), state);

								if (sRet == TEXT("nochange"))
									return 0L;
							}

							DCXENUM de{ m_TitleButton,m_Hwnd,state };

#pragma warning(push)
#pragma warning(disable: 4191)

							EnumChildWindows(m_Hwnd,(WNDENUMPROC)DcxBox::EnumBoxChildren,(LPARAM)&de);

#pragma warning(pop)
							break;
						}
					} // end switch
					break;
				}
			}
		case WM_HSCROLL:
		case WM_VSCROLL:
			{
				if (lParam == 0L)
					break;

				if (IsWindow((HWND) lParam)) {
					auto c_this = reinterpret_cast<DcxControl *>(GetProp((HWND)lParam, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_COMPAREITEM:
			{
				dcxlParam(LPCOMPAREITEMSTRUCT, idata);

				if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
					auto c_this = reinterpret_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DELETEITEM:
			{
				dcxlParam(LPDELETEITEMSTRUCT, idata);
				if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
					auto c_this = reinterpret_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_MEASUREITEM:
			{
				auto cHwnd = GetDlgItem(m_Hwnd, static_cast<int>(wParam));
				if (IsWindow(cHwnd)) {
					auto c_this = reinterpret_cast<DcxControl *>(GetProp(cHwnd, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DRAWITEM:
			{
				dcxlParam(LPDRAWITEMSTRUCT, idata);

				if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
					auto c_this = reinterpret_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_SIZE:
			{

				HWND bars = nullptr;

				while ( ( bars = FindWindowEx( m_Hwnd, bars, DCX_REBARCTRLCLASS, nullptr ) ) != nullptr ) {

					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				while ( ( bars = FindWindowEx( m_Hwnd, bars, DCX_STATUSBARCLASS, nullptr ) ) != nullptr ) {

					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				while ( ( bars = FindWindowEx( m_Hwnd, bars, DCX_TOOLBARCLASS, nullptr ) ) != nullptr ) {

					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_SIZE))
					execAliasEx(TEXT("sizing,%u"), getUserID( ) );

				//if (m_pLayoutManager != nullptr) {
				//	RECT rc;
				//	SetRect( &rc, 0, 0, LOWORD( lParam ), HIWORD( lParam ) );
				//	if (m_pLayoutManager->updateLayout( rc ))
				//		redrawWindow( );
				//}
			}
			break;
		case WM_WINDOWPOSCHANGING:
			{
				if (lParam == 0L)
					break;

				if (m_pLayoutManager != nullptr) {
					dcxlParam(LPWINDOWPOS, wp);

					RECT rc;
					SetRect( &rc, 0, 0, wp->cx, wp->cy );
					m_pLayoutManager->updateLayout( rc );
				}
			}
			break;

		case WM_ENABLE: {
			redrawWindow();
			break;
		}


		case WM_ERASEBKGND:
		{
			EraseBackground((HDC)wParam);
			bParsed = TRUE;
			return TRUE;
		}
		break;

		case WM_PRINTCLIENT:
			DrawClientArea((HDC)wParam);
			bParsed = TRUE;
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps;

				auto hdc = BeginPaint(m_Hwnd, &ps);

				DrawClientArea(hdc);

				EndPaint(m_Hwnd, &ps);

				bParsed = TRUE;
				return 0L;
			}
			break;

		case WM_THEMECHANGED:
			{
				if (_hTheme != nullptr) {
					Dcx::UXModule.dcxCloseThemeData(_hTheme);
					_hTheme = Dcx::UXModule.dcxOpenThemeData(m_Hwnd,L"BUTTON");
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
			lRes = CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return lRes;
}
void DcxBox::EraseBackground(HDC hdc)
{
	RECT rc;
	if (GetClientRect(m_Hwnd, &rc))
	{
		// fill background.
		if (isExStyle(WS_EX_TRANSPARENT) || m_pParentDialog->isExStyle(WS_EX_COMPOSITED))
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
	RECT rc = { 0 }, rc2 = { 0 }, rcText = { 0 }, rcText2 = { 0 };
	TString wtext;
	const auto n = TGetWindowText(m_Hwnd, wtext);

	if (!GetClientRect(m_Hwnd, &rc))
		return;

	// Setup alpha blend if any.
	auto ai = SetupAlphaBlend(&hdc, true);
	Auto(FinishAlphaBlend(ai));

	// if no border, dont bother
	if (dcx_testflag(m_iBoxStyles, BOXS_NONE)) {
		DcxControl::DrawCtrlBackground(hdc, this, &rc);
		//this->FinishAlphaBlend(ai);
		return;
	}

	CopyRect(&rc2, &rc);

	SetBkMode(hdc, TRANSPARENT);

	// no text, no box!
	if (n == 0) {
		if (dcx_testflag(m_iBoxStyles, BOXS_ROUNDED)) {
			auto m_Region = CreateRoundRectRgn(rc2.left, rc2.top, rc2.right, rc2.bottom, 10, 10);
			if (m_Region != nullptr) {
				Auto(DeleteRgn(m_Region));

				SelectClipRgn(hdc,m_Region);
				DcxControl::DrawCtrlBackground(hdc, this, &rc2);
				SelectClipRgn(hdc,nullptr);

				auto hBorderBrush = m_hBorderBrush;
				if (hBorderBrush == nullptr)
					hBorderBrush = GetStockBrush(BLACK_BRUSH);

				FrameRgn(hdc,m_Region,hBorderBrush,1,1);
			}
		}
		else {
			DcxControl::DrawCtrlBackground(hdc, this, &rc2);
			DrawEdge(hdc, &rc2, EDGE_ETCHED, BF_RECT);
		}
		if (IsWindow(m_TitleButton))
			SetWindowPos(m_TitleButton,nullptr,rc2.left,rc2.top,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	}
	// draw text
	else {
		// prepare for appearance
		if (m_hFont != nullptr)
			SelectFont(hdc, m_hFont);

		if (m_clrText != CLR_INVALID)
			SetTextColor(hdc, m_clrText);
		else
			SetTextColor(hdc, GetSysColor(IsWindowEnabled(m_Hwnd) ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));

		CopyRect(&rcText, &rc); // MUST initialize rect first!.

		calcTextRect(hdc, wtext, &rcText, DT_LEFT | DT_END_ELLIPSIS |DT_SINGLELINE);
		if (m_bShadowText) {
			rcText.bottom = std::min((rcText.bottom +6), rc.bottom);
			rcText.right = std::min((rcText.right +6), rc.right);
		}

		const auto w = rcText.right - rcText.left;
		const auto h = rcText.bottom - rcText.top;

		// shift border and text locations
		// text at bottom?
		if (dcx_testflag(this->m_iBoxStyles, BOXS_BOTTOM)) {
			rcText.top = rc2.bottom - h;
			rc2.bottom -= h/2;
		}
		// text at top
		else {
			rcText.top = rc2.top;
			rc2.top += h/2;
		}

		// text location
		rcText.bottom = rcText.top + h;

		// align text horizontally
		const auto bw = rc.right - rc.left - (2 * DCX_BOXTEXTSPACING);

		if (w > bw) {
			rcText.left = rc.left + DCX_BOXTEXTSPACING;
			rcText.right = rc.right - DCX_BOXTEXTSPACING;
		}
		else {
			if (dcx_testflag(this->m_iBoxStyles, BOXS_RIGHT))
				rcText.left = rc.right - DCX_BOXTEXTSPACING - w;
			else if (dcx_testflag(this->m_iBoxStyles, BOXS_CENTER))
				rcText.left = (rc.left + rc.right - w) /2;
			else
				rcText.left = rc.left + DCX_BOXTEXTSPACING;

			rcText.right = rcText.left + w;
		}

		// clear some space for the text
		CopyRect(&rcText2, &rcText);
		InflateRect(&rcText2, 3, 0);

		if (IsWindow(this->m_TitleButton))
		{
			RECT bSZ;
			if (GetWindowRect(this->m_TitleButton, &bSZ))
			{
				bSZ.bottom = (bSZ.right - bSZ.left);
				SetWindowPos(this->m_TitleButton, nullptr, rcText2.left, rcText2.top, bSZ.bottom, (rcText2.bottom - rcText2.top), SWP_NOZORDER | SWP_NOACTIVATE);
				rcText.left += bSZ.bottom;
				rcText.right += bSZ.bottom;
				rcText2.left += bSZ.bottom;
				rcText2.right += bSZ.bottom;
			}
		}

		// draw the border
		if (dcx_testflag(this->m_iBoxStyles, BOXS_ROUNDED)) {
			auto m_Region = CreateRoundRectRgn(rc2.left, rc2.top, rc2.right, rc2.bottom, 10, 10);
			if (m_Region != nullptr) {
				Auto(DeleteRgn(m_Region));

				SelectClipRgn(hdc,m_Region);
				DcxControl::DrawCtrlBackground(hdc, this, &rc2);
				SelectClipRgn(hdc,nullptr);
				ExcludeClipRect(hdc, rcText2.left, rcText2.top, rcText2.right, rcText2.bottom);

				auto hBorderBrush = this->m_hBorderBrush;
				if (hBorderBrush == nullptr)
					hBorderBrush = GetStockBrush(BLACK_BRUSH);

				FrameRgn(hdc,m_Region,hBorderBrush,1,1);
			}
		}
		else {
			DcxControl::DrawCtrlBackground(hdc, this, &rc2);
			ExcludeClipRect(hdc, rcText2.left, rcText2.top, rcText2.right, rcText2.bottom);
			DrawEdge(hdc, &rc2, EDGE_ETCHED, BF_RECT);
		}
		SelectClipRgn(hdc,nullptr);

		// draw the text
		this->ctrlDrawText(hdc, wtext, &rcText, DT_LEFT | DT_END_ELLIPSIS |DT_SINGLELINE);
	}

	//this->FinishAlphaBlend(ai);
}
