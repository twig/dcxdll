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

	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	this->m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CONTROLPARENT,
		DCX_BOXCLASS,
		nullptr,
		Styles | WS_CHILD,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(this->m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	// remove all borders
	this->removeStyle(WS_BORDER | WS_DLGFRAME);
	this->removeExStyle(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE);

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(this->m_Hwnd, L" ", L" ");

	this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);

	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);

	if (dcx_testflag(this->m_iBoxStyles, BOXS_CHECK)) {
		this->m_TitleButton = CreateWindowEx(
			ExStyles,
			TEXT("BUTTON"),
			nullptr,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX,
			CW_USEDEFAULT, CW_USEDEFAULT, 11, 10,
			this->m_Hwnd,
			(HMENU)ID,
			GetModuleHandle(nullptr),
			nullptr);
	}
	else if (dcx_testflag(this->m_iBoxStyles, BOXS_RADIO)) {
		this->m_TitleButton = CreateWindowEx(
			ExStyles,
			TEXT("BUTTON"),
			nullptr,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON,
			CW_USEDEFAULT, CW_USEDEFAULT, 11, 10,
			this->m_Hwnd,
			(HMENU)ID,
			GetModuleHandle(nullptr),
			nullptr);
	}
	if (IsWindow(this->m_TitleButton)) {
		if (bNoTheme)
			Dcx::UXModule.dcxSetWindowTheme(this->m_TitleButton, L" ", L" ");
		if (!dcx_testflag(Styles, WS_DISABLED))
			SendMessage(this->m_TitleButton, BM_SETCHECK, BST_CHECKED, 0L);
	}
	if (Dcx::UXModule.isUseable())
		this->_hTheme = Dcx::UXModule.dcxOpenThemeData(this->m_Hwnd, L"BUTTON");
}

/*!
* \brief blah
*
* blah
*/

DcxBox::~DcxBox() {

	delete this->m_pLayoutManager;

	if (this->_hTheme)
		Dcx::UXModule.dcxCloseThemeData(this->_hTheme);
	this->unregistreDefaultWindowProc();
}

/*!
* \brief blah
*
* blah
*/

void DcxBox::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	this->m_iBoxStyles = 0;

	for (const auto &a : styles)
	{
#if DCX_SWITCH_OBJ
		Switch(a)
			.Case(TEXT("right"), [this] { m_iBoxStyles |= BOXS_RIGHT; }).Break()
			.Case(TEXT("center"), [this] { m_iBoxStyles |= BOXS_CENTER; }).Break()
			.Case(TEXT("bottom"), [this] { m_iBoxStyles |= BOXS_BOTTOM; }).Break()
			.Case(TEXT("none"), [this] { m_iBoxStyles |= BOXS_NONE; }).Break()
			.Case(TEXT("rounded"), [this] { m_iBoxStyles |= BOXS_ROUNDED; }).Break()
			.Case(TEXT("check"), [this] { m_iBoxStyles &= ~BOXS_RADIO; m_iBoxStyles |= BOXS_CHECK; }).Break()
			.Case(TEXT("radio"), [this] { m_iBoxStyles &= ~BOXS_CHECK; m_iBoxStyles |= BOXS_RADIO; }).Break()
			.Case(TEXT("transparent"), [ExStyles] { *ExStyles |= WS_EX_TRANSPARENT; }).Break();
#else
		if (a == TEXT("right"))
			this->m_iBoxStyles |= BOXS_RIGHT;
		else if (a == TEXT("center"))
			this->m_iBoxStyles |= BOXS_CENTER;
		else if (a == TEXT("bottom"))
			this->m_iBoxStyles |= BOXS_BOTTOM;
		else if (a == TEXT("none"))
			this->m_iBoxStyles |= BOXS_NONE;
		else if (a == TEXT("rounded"))
			this->m_iBoxStyles |= BOXS_ROUNDED;
		else if (a == TEXT("check")) {
			this->m_iBoxStyles &= ~BOXS_RADIO;
			this->m_iBoxStyles |= BOXS_CHECK;
		}
		else if (a == TEXT("radio")) {
			this->m_iBoxStyles &= ~BOXS_CHECK;
			this->m_iBoxStyles |= BOXS_RADIO;
		}
		else if (a == TEXT("transparent"))
			*ExStyles |= WS_EX_TRANSPARENT;
#endif
	}

	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

#ifndef DCX_SWITCH_OBJ
void DcxBox::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	//  auto numtok = input.numtok( );

	const auto prop(input.getfirsttok( 3 ));

	// [NAME] [ID] [PROP]
	if ( prop == TEXT("text") ) {

		GetWindowText( this->m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH );
	}
	else if ( prop == TEXT("inbox") ) {

		RECT rc;
		if (!GetClientRect(this->m_Hwnd, &rc))
			throw Dcx::dcxException("Unable to get client rect!");

		InflateRect( &rc, -2, -2 );
		if ( GetWindowTextLength( this->m_Hwnd ) > 0 )
		{
			auto hdc = GetDC( this->m_Hwnd );
			if (hdc != nullptr)
			{
				HFONT oldFont = nullptr;
				RECT rcText = rc;

				if (this->m_hFont != nullptr)
					oldFont = SelectFont(hdc, this->m_hFont);

				TString text;
				TGetWindowText(this->m_Hwnd, text);
				DrawText(hdc, text.to_chr(), text.len(), &rcText, DT_CALCRECT);

				if (oldFont != nullptr)
					SelectFont(hdc, oldFont);

				ReleaseDC(this->m_Hwnd, hdc);

				//const auto w = rcText.right - rcText.left;
				const auto h = rcText.bottom - rcText.top;

				if (dcx_testflag(this->m_iBoxStyles, BOXS_BOTTOM))
					rc.bottom -= (h + 2);
				else
					rc.top += (h - 2);
			}
		}

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top );
	}
	else
		this->parseGlobalInfoRequest( input, szReturnValue );
}
#else
void DcxBox::parseInfoRequest(const TString & input, PTCHAR szReturnValue) const
{
	szReturnValue[0] = 0;

	Switch(input.getfirsttok(3))
		.Case(TEXT("text"), [this, szReturnValue] { GetWindowText(m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH); }).Break()
		.Case(TEXT("inbox"), [this, szReturnValue] {
			RECT rc;
			if (!GetClientRect(this->m_Hwnd, &rc))
				throw Dcx::dcxException("Unable to get client rect!");

			InflateRect(&rc, -2, -2);
			if (GetWindowTextLength(this->m_Hwnd) > 0)
			{
				auto hdc = GetDC(this->m_Hwnd);
				if (hdc == nullptr)
					throw Dcx::dcxException("Unable to get windows DC");

				HFONT oldFont = nullptr;
				RECT rcText = rc;

				if (this->m_hFont != nullptr)
					oldFont = SelectFont(hdc, this->m_hFont);

				TString text;
				TGetWindowText(this->m_Hwnd, text);
				DrawText(hdc, text.to_chr(), text.len(), &rcText, DT_CALCRECT);

				if (oldFont != nullptr)
					SelectFont(hdc, oldFont);

				ReleaseDC(this->m_Hwnd, hdc);

				//const auto w = rcText.right - rcText.left;
				const auto h = rcText.bottom - rcText.top;

				if (dcx_testflag(this->m_iBoxStyles, BOXS_BOTTOM))
					rc.bottom -= (h + 2);
				else
					rc.top += (h - 2);
			}

			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	}).Break()
		.Default([this, &input, szReturnValue] { this->parseGlobalInfoRequest(input, szReturnValue); });
}
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

		//const auto ID = mIRC_ID_OFFSET + (UINT)input.getnexttok().to_int();	// tok 4
		//
		//if (!this->m_pParentDialog->isIDValid(ID, true))
		//	throw Dcx::dcxException(TEXT("Control with ID \"%\" already exists"), ID - mIRC_ID_OFFSET);
		//
		//try {
		//	//throw Dcx::dcxException("test");
		//
		//	this->m_pParentDialog->addControl(DcxControl::controlFactory(this->m_pParentDialog, ID, input, 5, CTLF_ALLOW_ALL, this->m_Hwnd));
		//	this->redrawWindow( );
		//}
		//catch (std::exception &e)
		//{
		//	this->showErrorEx(nullptr, TEXT("-c"), TEXT("Unable To Create Control %d (%S)"), ID - mIRC_ID_OFFSET, e.what());
		//	throw;
		//}

		this->m_pParentDialog->addControl(input, 4, CTLF_ALLOW_ALL, this->m_Hwnd);
		this->redrawWindow();
	}
	// xdid -d [NAME] [ID] [SWITCH] [ID]
	else if ( flags[TEXT('d')] && numtok > 3 ) {

		const auto ID = mIRC_ID_OFFSET + (UINT)input.getnexttok().to_int();	// tok 4

		if (!this->m_pParentDialog->isIDValid(ID))
			throw Dcx::dcxException(TEXT("Unknown control with ID \"%\" (dialog %)"), ID - mIRC_ID_OFFSET, this->m_pParentDialog->getName());

		auto p_Control = this->m_pParentDialog->getControlByID(ID);

		if (p_Control == nullptr)
			throw Dcx::dcxException("Unable to get control");

		const auto dct = p_Control->getControlType();

		if (dct == DcxControlTypes::DIALOG || dct == DcxControlTypes::WINDOW)
			delete p_Control;
		else {
			if (p_Control->getRefCount() != 0)
				throw Dcx::dcxException(TEXT("Can't delete control with ID \"%\" when it is inside it's own event (dialog %)"), p_Control->getUserID(), this->m_pParentDialog->getName());

			auto cHwnd = p_Control->getHwnd();
			this->m_pParentDialog->deleteControl(p_Control); // remove from internal list!
			DestroyWindow(cHwnd);
		}

		//if (p_Control->getType() == TEXT("dialog") || p_Control->getType() == TEXT("window"))
		//	delete p_Control;
		//else {
		//	if (p_Control->getRefCount() != 0)
		//		throw Dcx::dcxException(TEXT("Can't delete control with ID \"%\" when it is inside it's own event (dialog %)"), p_Control->getUserID(), this->m_pParentDialog->getName());
		//
		//	auto cHwnd = p_Control->getHwnd();
		//	this->m_pParentDialog->deleteControl(p_Control); // remove from internal list!
		//	DestroyWindow(cHwnd);
		//}
	}
	/*
	//xdid -l [NAME] [ID] [SWITCH] [OPTIONS]

	[OPTIONS] :

	root [TAB]+flpiw [ID] [WEIGHT] [W] [H]
	add PATH[TAB]+flpiw [ID] [WEIGHT] [W] [H]
	space PATH[TAB]+ [L] [T] [R] [B]
	*/
	else if ( flags[TEXT('l')] && numtok > 3 ) {

		const auto tsCmd(input.getnexttok());	// tok 4

		if ( tsCmd == TEXT("update") ) {
			if ( this->m_pLayoutManager != nullptr ) {
				RECT rc;
				if (!GetClientRect( this->m_Hwnd, &rc ))
					throw Dcx::dcxException("Unable to get client rect!");
				
				this->m_pLayoutManager->updateLayout(rc);

				this->redrawWindow();
			}
		}
		else if (tsCmd == TEXT("clear")) {
			delete this->m_pLayoutManager;
			this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);
			//this->redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
		else if ( numtok > 8 )
			this->m_pLayoutManager->AddCell(input, 4);
	}
	//xdid -t [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('t')] ) {
		SetWindowText(this->m_Hwnd, input.getlasttoks().trim().to_chr());	// tok 4, -1
		this->redrawWindow( );
	}
	else
		this->parseGlobalCommandRequest( input, flags );
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

void DcxBox::toXml(TiXmlElement *const xml) const {
	TString wtext;
	TGetWindowText(this->m_Hwnd, wtext);
	__super::toXml(xml);
	xml->SetAttribute("caption", wtext.c_str());
	this->m_pLayoutManager->getRoot()->toXml(xml);
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
				if (((HWND)lParam != nullptr) && ((HWND)lParam == this->m_TitleButton) ) {
					switch ( HIWORD( wParam ) )
					{
						case BN_CLICKED:
						{
							const auto state = (SendMessage(this->m_TitleButton, BM_GETCHECK, 0, 0) == BST_CHECKED);
							if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {
								TCHAR ret[10];

								this->evalAliasEx(ret, 9, TEXT("%s,%d,%d"), TEXT("checkchange"), this->getUserID(), state);

								if (lstrcmp(TEXT("nochange"), ret) == 0)
									return 0L;
							}

							DCXENUM de;
							de.mChildHwnd = this->m_TitleButton;
							de.mBox = this->m_Hwnd;
							de.mState = state;

							EnumChildWindows(this->m_Hwnd,(WNDENUMPROC)DcxBox::EnumBoxChildren,(LPARAM)&de);
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
				auto cHwnd = GetDlgItem(this->m_Hwnd, wParam);
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

				while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_REBARCTRLCLASS, nullptr ) ) != nullptr ) {

					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_STATUSBARCLASS, nullptr ) ) != nullptr ) {

					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_TOOLBARCLASS, nullptr ) ) != nullptr ) {

					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_SIZE))
					this->execAliasEx(TEXT("%s,%d"), TEXT("sizing"), this->getUserID( ) );

				//if (this->m_pLayoutManager != nullptr) {
				//	RECT rc;
				//	SetRect( &rc, 0, 0, LOWORD( lParam ), HIWORD( lParam ) );
				//	if (this->m_pLayoutManager->updateLayout( rc ))
				//		this->redrawWindow( );
				//}
			}
			break;
		case WM_WINDOWPOSCHANGING:
			{
				if (lParam == 0L)
					break;

				if (this->m_pLayoutManager != nullptr) {
					dcxlParam(LPWINDOWPOS, wp);

					RECT rc;
					SetRect( &rc, 0, 0, wp->cx, wp->cy );
					this->m_pLayoutManager->updateLayout( rc );
				}
			}
			break;

		case WM_ENABLE: {
			this->redrawWindow();
			break;
		}


		case WM_ERASEBKGND:
		{
			this->EraseBackground((HDC)wParam);
			bParsed = TRUE;
			return TRUE;
		}
		break;

		case WM_PRINTCLIENT:
			this->DrawClientArea((HDC)wParam);
			bParsed = TRUE;
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps;

				auto hdc = BeginPaint(this->m_Hwnd, &ps);

				this->DrawClientArea(hdc);

				EndPaint(this->m_Hwnd, &ps);

				bParsed = TRUE;
				return 0L;
			}
			break;

		case WM_THEMECHANGED:
			{
				if (this->_hTheme != nullptr) {
					Dcx::UXModule.dcxCloseThemeData(this->_hTheme);
					this->_hTheme = Dcx::UXModule.dcxOpenThemeData(this->m_Hwnd,L"BUTTON");
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
			lRes = this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return lRes;
}
void DcxBox::EraseBackground(HDC hdc)
{
	RECT rc;
	if (GetClientRect(this->m_Hwnd, &rc))
	{
		// fill background.
		if (this->isExStyle(WS_EX_TRANSPARENT) || this->m_pParentDialog->isExStyle(WS_EX_COMPOSITED))
			this->DrawParentsBackground(hdc, &rc);
		else // normal bkg
			DcxControl::DrawCtrlBackground(hdc, this, &rc);
		// Update CLA if any.
		if (this->m_pLayoutManager != nullptr)
			this->m_pLayoutManager->updateLayout(rc);
	}
}

void DcxBox::DrawClientArea(HDC hdc)
{
	RECT rc, rc2, rcText, rcText2;
	TString wtext;
	const auto n = TGetWindowText(this->m_Hwnd, wtext);

	if (!GetClientRect(this->m_Hwnd, &rc))
		return;

	// Setup alpha blend if any.
	auto ai = this->SetupAlphaBlend(&hdc, true);

	// if no border, dont bother
	if (dcx_testflag(this->m_iBoxStyles, BOXS_NONE)) {
		DcxControl::DrawCtrlBackground(hdc, this, &rc);
		this->FinishAlphaBlend(ai);
		return;
	}

	CopyRect(&rc2, &rc);

	SetBkMode(hdc, TRANSPARENT);

	// no text, no box!
	if (n == 0) {
		if (dcx_testflag(this->m_iBoxStyles, BOXS_ROUNDED)) {
			auto m_Region = CreateRoundRectRgn(rc2.left, rc2.top, rc2.right, rc2.bottom, 10, 10);
			if (m_Region != nullptr) {
				SelectClipRgn(hdc,m_Region);
				DcxControl::DrawCtrlBackground(hdc, this, &rc2);
				SelectClipRgn(hdc,nullptr);

				auto hBorderBrush = this->m_hBorderBrush;
				if (hBorderBrush == nullptr)
					hBorderBrush = GetStockBrush(BLACK_BRUSH);

				FrameRgn(hdc,m_Region,hBorderBrush,1,1);
				DeleteRgn(m_Region);
			}
		}
		else {
			DcxControl::DrawCtrlBackground(hdc, this, &rc2);
			DrawEdge(hdc, &rc2, EDGE_ETCHED, BF_RECT);
		}
		if (IsWindow(this->m_TitleButton))
			SetWindowPos(this->m_TitleButton,nullptr,rc2.left,rc2.top,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	}
	// draw text
	else {
		// prepare for appearance
		if (this->m_hFont != nullptr)
			SelectFont(hdc, this->m_hFont);

		if (this->m_clrText != CLR_INVALID)
			SetTextColor(hdc, this->m_clrText);
		else
			SetTextColor(hdc, GetSysColor(IsWindowEnabled(this->m_Hwnd) ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));

		CopyRect(&rcText, &rc); // MUST initialize rect first!.

		this->calcTextRect(hdc, wtext, &rcText, DT_LEFT | DT_END_ELLIPSIS |DT_SINGLELINE);
		if (this->m_bShadowText) {
			rcText.bottom = min((rcText.bottom +6), rc.bottom);
			rcText.right = min((rcText.right +6), rc.right);
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
				SelectClipRgn(hdc,m_Region);
				DcxControl::DrawCtrlBackground(hdc, this, &rc2);
				SelectClipRgn(hdc,nullptr);
				ExcludeClipRect(hdc, rcText2.left, rcText2.top, rcText2.right, rcText2.bottom);

				auto hBorderBrush = this->m_hBorderBrush;
				if (hBorderBrush == nullptr)
					hBorderBrush = GetStockBrush(BLACK_BRUSH);

				FrameRgn(hdc,m_Region,hBorderBrush,1,1);
				DeleteRgn(m_Region);
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

	this->FinishAlphaBlend(ai);
}
