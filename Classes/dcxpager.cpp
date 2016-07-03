/*
	* Pager Control
*/
#include "defines.h"
#include "Classes/dcxpager.h"
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

DcxPager::DcxPager(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles )
: DcxControl( ID, p_Dialog )
, m_ChildHWND(nullptr)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CONTROLPARENT,
		DCX_PAGERCLASS,
		nullptr,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( m_Hwnd , L" ", L" " );

	//Pager_SetButtonSize(m_Hwnd,15);
	//Pager_SetBkColor(m_Hwnd,0);

	this->registreDefaultWindowProc( );
	SetProp( m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxPager::~DcxPager( ) {

  this->unregistreDefaultWindowProc( );
}

const TString DcxPager::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, PGS_HORZ))
		styles.addtok(TEXT("horizontal"));
	if (dcx_testflag(Styles, PGS_AUTOSCROLL))
		styles.addtok(TEXT("autoscroll"));

	return styles;
}

void DcxPager::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);
	const auto child = this->m_pParentDialog->getControlByHWND(this->m_ChildHWND);
	if (child != nullptr)
		xml->LinkEndChild(child->toXml());
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPager::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
{
	for (const auto &tsStyle: styles)
	{
		if (tsStyle == TEXT("horizontal"))
			*Styles |= PGS_HORZ;
		else if (tsStyle == TEXT("autoscroll"))
			*Styles |= PGS_AUTOSCROLL;
	}

	this->parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxPager::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	//  auto numtok = input.numtok( );
	const auto prop(input.gettok( 3 ));

	if (prop == TEXT("color"))
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), Pager_GetBkColor(m_Hwnd) );
	else if ( prop == TEXT("bsize"))
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), Pager_GetButtonSize(m_Hwnd) );
	else if ( prop == TEXT("border"))
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), Pager_GetBorder(m_Hwnd) );
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPager::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -b [NAME] [ID] [SWITCH] [W]
	if ( flags[TEXT('b')] && numtok > 3 ) {
		this->setBorderSize(input.getnexttok( ).to_int());	// tok 4
	}
	// xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	else if ( flags[TEXT('c')] && numtok > 8 ) {

		if (IsWindow(this->m_ChildHWND))
			throw Dcx::dcxException("Child Control already exists");

		auto p_Control = this->m_pParentDialog->addControl(input, 4,
			CTLF_ALLOW_TOOLBAR |
			CTLF_ALLOW_REBAR |
			CTLF_ALLOW_PANEL |
			CTLF_ALLOW_TAB |
			CTLF_ALLOW_BOX |
			CTLF_ALLOW_IMAGE |
			CTLF_ALLOW_PAGER |
			CTLF_ALLOW_DOCK
			, m_Hwnd);

		p_Control->addStyle(CCS_NORESIZE);
		this->setChild(p_Control->getHwnd());
	}
	// xdid -d [NAME] [ID] [SWITCH] [ID]
	else if ( flags[TEXT('d')] && numtok > 3 ) {
		const auto tsID(input.getnexttok( ));		// tok 4
		const auto ID = this->m_pParentDialog->NameToID(tsID);

		if ( !this->m_pParentDialog->isIDValid(ID) )
			throw Dcx::dcxException(TEXT("Unknown control with ID \"%\" (dialog %)"), tsID, this->m_pParentDialog->getName());

		auto p_Control = this->m_pParentDialog->getControlByID(ID);
		// Ook: no ref count check for dialog or window? needs checked

		if (p_Control == nullptr)
			throw Dcx::dcxException(TEXT("Unable to get control with ID \"%\" (dialog %)"), tsID, this->m_pParentDialog->getName());

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

		this->m_ChildHWND = nullptr;
	}
	// xdid -s [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('s')] && numtok > 3) {
		this->setButtonSize(input.getlasttoks().to_<LONG>());	// tok 4, -1
	}
	// xdid -t [NAME] [ID] [SWITCH] [COLOR]
	else if (flags[TEXT('t')] && numtok > 3) {
		this->setBkColor(input.getlasttoks().to_<COLORREF>());	// tok 4, -1
	}
	// xdid -z [NAME] [ID] [SWITCH]
	else if (flags[TEXT('z')] && numtok > 2) {
		this->reCalcSize();
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}
void DcxPager::setChild(const HWND child)
{
	this->m_ChildHWND = child;
	Pager_SetChild(m_Hwnd,child);
}
void DcxPager::setBkColor(const COLORREF c)
{
	Pager_SetBkColor(m_Hwnd,c);
}
void DcxPager::setBorderSize(const int bSize)
{
	Pager_SetBorder(m_Hwnd,bSize);
}
void DcxPager::setButtonSize(const int bSize)
{
	Pager_SetButtonSize(m_Hwnd,bSize);
}
void DcxPager::reCalcSize(void) const
{
	Pager_RecalcSize(m_Hwnd);
}
/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxPager::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) {
	switch (uMsg) {
	case WM_NOTIFY:
	{

		dcxlParam(LPNMHDR, hdr);

		if (hdr == nullptr)
			break;

		switch (hdr->code) {
		case PGN_CALCSIZE:
		{
			bParsed = TRUE;
			dcxlParam(LPNMPGCALCSIZE, lpnmcs);

			//GetWindowRect(this->m_ChildHWND,&rc);
			//if (lpnmcs->dwFlag == PGF_CALCHEIGHT)
			//	lpnmcs->iHeight = (rc.bottom - rc.top);
			//else
			//	lpnmcs->iWidth = (rc.right - rc.left);

			auto cthis = static_cast<DcxControl *>(GetProp(this->m_ChildHWND, TEXT("dcx_cthis")));
			if (cthis == nullptr)
				break;

			const auto bSize = Pager_GetButtonSize(m_Hwnd);
			//if (cthis->getType() == TEXT("toolbar")) {
			if (cthis->getControlType() == DcxControlTypes::TOOLBAR) {
				SIZE sz = { 0 };
				SendMessage(m_ChildHWND, TB_GETMAXSIZE, 0, (LPARAM)&sz);
				if (lpnmcs->dwFlag == PGF_CALCHEIGHT)
					lpnmcs->iHeight = sz.cy + bSize;
				else
					lpnmcs->iWidth = sz.cx + bSize;
			}
			else {
				RECT rc = { 0 };
				if (!GetWindowRect(m_ChildHWND, &rc))
					break;

				if (lpnmcs->dwFlag == PGF_CALCHEIGHT)
					lpnmcs->iHeight = (rc.bottom - rc.top) + bSize;
				else
					lpnmcs->iWidth = (rc.right - rc.left) + bSize;
			}
		}
		break;
		}
	}
	break;
	}
	return 0L;
}

LRESULT DcxPager::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

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
			switch( hdr->code ) {
			case TBN_DELETINGBUTTON:
				{ // handle toolbar button delete.
					this->reCalcSize();
				}
				break;
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
			auto cHwnd = GetDlgItem(m_Hwnd, wParam);
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

			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_SIZE))
				this->execAliasEx(TEXT("%s,%d"), TEXT("sizing"), this->getUserID( ) );

			this->reCalcSize();
			this->redrawWindow( );
		}
		break;

	case WM_SETFOCUS:
		{
			this->m_pParentDialog->setFocusControl( this->getUserID( ) );
			this->reCalcSize();
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
