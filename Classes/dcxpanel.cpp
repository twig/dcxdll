/*!
 * \file dcxpanel.cpp
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
#include "Classes/dcxpanel.h"
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

DcxPanel::DcxPanel(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles )
: DcxControl( ID, p_Dialog ) 
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	m_Hwnd = CreateWindowEx(	
		static_cast<DWORD>(ExStyles) | WS_EX_CONTROLPARENT, 
		DCX_PANELCLASS, 
		nullptr,
		WS_CHILD | static_cast<DWORD>(Styles),
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(nullptr), 
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( m_Hwnd , L" ", L" " );

	this->m_pLayoutManager = new LayoutManager( m_Hwnd );

	this->registreDefaultWindowProc( );
	SetProp( m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxPanel::~DcxPanel( ) {

	delete this->m_pLayoutManager;

	this->unregistreDefaultWindowProc( );
}

void DcxPanel::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);

	m_pLayoutManager->getRoot()->toXml(xml);
}

TiXmlElement * DcxPanel::toXml(void) const
{
	auto xml = __super::toXml();

	m_pLayoutManager->getRoot()->toXml(xml);

	return xml;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPanel::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
{
	parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxPanel::parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPanel::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if ( flags[TEXT('c')] && numtok > 8 ) {

		//const auto ID = mIRC_ID_OFFSET + (UINT)input.getnexttok().to_int();	// tok 4
		//
		//if (!this->m_pParentDialog->isIDValid(ID, true))
		//	throw Dcx::dcxException(TEXT("Control with ID \"%\" already exists"), ID - mIRC_ID_OFFSET);
	//
		//try {
		//	m_pParentDialog->addControl(DcxControl::controlFactory(m_pParentDialog, ID, input, 5, CTLF_ALLOW_ALL, m_Hwnd));
		//}
		//catch (const std::exception &e ) {
		//	showErrorEx(nullptr, TEXT("-c"), TEXT("Unable To Create Control %d (%S)"), ID - mIRC_ID_OFFSET, e.what());
		//	throw;
		//}
		//this->redrawWindow();

		m_pParentDialog->addControl(input, 4, CTLF_ALLOW_ALL, m_Hwnd);
		redrawWindow();
	}
	// xdid -d [NAME] [ID] [SWITCH] [ID]
	else if ( flags[TEXT('d')] && numtok > 3 ) {

		const auto ID = mIRC_ID_OFFSET + input.getnexttok().to_<UINT>();	// tok 4

		if (!this->m_pParentDialog->isIDValid(ID))
			throw Dcx::dcxException(TEXT("Unknown control with ID \"%\" (dialog %)"), ID - mIRC_ID_OFFSET, this->m_pParentDialog->getName());

		auto p_Control = this->m_pParentDialog->getControlByID(ID);
		if (p_Control == nullptr)
			throw Dcx::dcxException(TEXT("Unable to get control with ID \"%\" (dialog %)"), ID - mIRC_ID_OFFSET, this->m_pParentDialog->getName());

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
		{
			if (numtok <= 8)
				throw Dcx::dcxException("Invalid Args: Not enough arguments");
			
			m_pLayoutManager->AddCell(input, 4);
		}

		}
#else
		if (m_pLayoutManager == nullptr)
			throw Dcx::dcxException("No LayoutManager available");

		const auto tsCmd(input.getnexttok());	// tok 4

		if (tsCmd == TEXT("update")) {
			RECT rc;
			if (!GetClientRect(m_Hwnd, &rc))
				throw Dcx::dcxException("Unable to get client rect!");

			m_pLayoutManager->updateLayout(rc);

			redrawWindow();
		}
		else if (tsCmd == TEXT("clear")) {
			delete m_pLayoutManager;
			m_pLayoutManager = new LayoutManager(m_Hwnd);
			//redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
		else
		{
			if (numtok <= 8)
				throw Dcx::dcxException("Invalid Args: Not enough arguments");

			m_pLayoutManager->AddCell(input, 4);
		}
#endif
	}
	// xdid -t [NAME] [ID] [SWITCH] [TEXT]
	else if (flags[TEXT('t')] && numtok > 3) {
		SetWindowText(m_Hwnd, input.getlasttoks().to_chr());	// tok 4, -1
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxPanel::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxPanel::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
	switch( uMsg ) {

		case WM_NOTIFY : 
			{
				dcxlParam(LPNMHDR, hdr);

				if (hdr == nullptr)
					break;

				if (IsWindow(hdr->hwndFrom)) {
					auto c_this = static_cast<DcxControl *>(GetProp(hdr->hwndFrom, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_HSCROLL:
		case WM_VSCROLL:
		case WM_COMMAND:
			{
				if (IsWindow((HWND) lParam)) {
					auto c_this = static_cast<DcxControl *>(GetProp((HWND)lParam, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_COMPAREITEM:
			{
				dcxlParam(LPCOMPAREITEMSTRUCT, idata);

				if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
					auto c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DELETEITEM:
			{
				dcxlParam(LPDELETEITEMSTRUCT, idata);

				if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
					auto c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_MEASUREITEM:
			{
				auto cHwnd = GetDlgItem(m_Hwnd, static_cast<int>(wParam));
				if (IsWindow(cHwnd)) {
					auto c_this = static_cast<DcxControl *>(GetProp(cHwnd, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DRAWITEM:
			{
				dcxlParam(LPDRAWITEMSTRUCT, idata);

				if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
					auto c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_SIZE:
			{
					//bParsed = TRUE;
					//lRes = CallWindowProc(this->m_DefaultWindowProc, m_Hwnd, uMsg, wParam, lParam);

				HWND bars = nullptr;

				while ( ( bars = FindWindowEx( m_Hwnd, bars, DCX_REBARCTRLCLASS, nullptr ) ) != nullptr ) {
					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				while ( ( bars = FindWindowEx( m_Hwnd, bars, DCX_STATUSBARCLASS, nullptr ) ) != nullptr ) {
					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				//while ((bars = FindWindowEx(m_Hwnd, bars, DCX_PANELCLASS, nullptr)) != nullptr) {
				//	SendMessage(bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0);
				//}

				while ( ( bars = FindWindowEx( m_Hwnd, bars, DCX_TOOLBARCLASS, nullptr ) ) != nullptr ) {
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
				if (lParam != NULL) {
					dcxlParam(LPWINDOWPOS, wp);

					if (this->m_pLayoutManager != nullptr) {
						RECT rc;
						SetRect( &rc, 0, 0, wp->cx, wp->cy );
						//if (this->m_pLayoutManager->updateLayout( rc ))
						//	this->redrawWindow( );
						this->m_pLayoutManager->updateLayout( rc );
					}
				}
			}
			break;

		//case WM_EXITSIZEMOVE:
		//	{
		//		this->redrawWindow();
		//	}
		//	break;

		case WM_ERASEBKGND:
			{
				// fill background.
				if (this->isExStyle(WS_EX_TRANSPARENT))
					this->DrawParentsBackground((HDC)wParam);
				else // normal bkg
					DcxControl::DrawCtrlBackground((HDC)wParam, this);
				bParsed = TRUE;
				return TRUE;
			}
			break;

		case WM_PRINTCLIENT:
			{
				dcxwParam(HDC, hdc);

				bParsed = TRUE;

				// Setup alpha blend if any.
				const auto ai = this->SetupAlphaBlend(&hdc);

				{ // simply fill with bkg
					if (this->isExStyle(WS_EX_TRANSPARENT)) {
						if (!this->m_bAlphaBlend)
							this->DrawParentsBackground(hdc);
					}
					else
						DcxControl::DrawCtrlBackground(hdc,this);
				}

				this->FinishAlphaBlend(ai);
			}
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps;

				auto hdc = BeginPaint(m_Hwnd, &ps);

				bParsed = TRUE;

				// Setup alpha blend if any.
				auto ai = this->SetupAlphaBlend(&hdc);

				{ // simply fill with bkg
					if (this->isExStyle(WS_EX_TRANSPARENT)) {
						if (!this->m_bAlphaBlend)
							this->DrawParentsBackground(hdc);
					}
					else
						DcxControl::DrawCtrlBackground(hdc,this);
				}

				this->FinishAlphaBlend(ai);

				EndPaint( m_Hwnd, &ps );
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
