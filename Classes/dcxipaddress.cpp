/*!
 * \file dcxipaddress.cpp
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
#include "Classes/dcxipaddress.h"
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

DcxIpAddress::DcxIpAddress(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	m_Hwnd = CreateWindowEx(
		gsl::narrow_cast<DWORD>(ExStyles),
		DCX_IPADDRESSCLASS,
		nullptr,
		WS_CHILD | gsl::narrow_cast<DWORD>(Styles),
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);

	if (styles.istok(TEXT("tooltips"))) {
		if (!IsWindow(p_Dialog->getToolTip()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		this->m_ToolTipHWND = p_Dialog->getToolTip();
		AddToolTipToolInfo(this->m_ToolTipHWND, m_Hwnd);
	}

	// fix bug with disabled creation
	// todo: fix this properly
	if (dcx_testflag(Styles, WS_DISABLED)) {
		EnableWindow(m_Hwnd, TRUE);
		EnableWindow(m_Hwnd, FALSE);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

DcxIpAddress::~DcxIpAddress( ) {

	this->unregistreDefaultWindowProc( );
}

void DcxIpAddress::toXml(TiXmlElement *const xml) const
{
	char buf[128];
	this->AddressToString(&buf[0], Dcx::countof(buf));

	__super::toXml(xml);

	xml->SetAttribute("caption", &buf[0]);
}

TiXmlElement * DcxIpAddress::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxIpAddress::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
{
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

void DcxIpAddress::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	// [NAME] [ID] [PROP]
	if (input.gettok(3) == TEXT("ip")) {

		//DWORD ip;
		//this->getAddress(&ip);
		//
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d.%d.%d.%d"), FIRST_IPADDRESS(ip),
		//	SECOND_IPADDRESS(ip),
		//	THIRD_IPADDRESS(ip),
		//	FOURTH_IPADDRESS(ip));

		this->AddressToString(szReturnValue.data(), szReturnValue.size());
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxIpAddress::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));

	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		this->clearAddress();

	// xdid -a [NAME] [ID] [SWITCH] IP.IP.IP.IP
	if (flags[TEXT('a')] && numtok > 3) {
		const auto IP(input.getnexttok().strip());	// tok 4

		if (IP.numtok(TEXT('.')) != 4)
			throw Dcx::dcxException(TEXT("Invalid Address: %"), IP);

		this->setAddress(IP.to_addr());
	}
	// xdid -g [NAME] [ID] [SWITCH] [N] [MIN] [MAX]
	else if (flags[TEXT('g')] && numtok > 5) {
		const auto nField = input.getnexttok().to_int() - 1;				// tok 4
		const auto min = (BYTE)(input.getnexttok().to_int() & 0xFF);	// tok 5
		const auto max = (BYTE)(input.getnexttok().to_int() & 0xFF);	// tok 6

		if (nField < 0 || nField > 3)
			throw Dcx::dcxException("Out of Range");

		this->setRange(nField, min, max);
	}
	// xdid -j [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('j')] && numtok > 3) {
		const auto nField = input.getnexttok().to_int() - 1;	// tok 4

		if (nField < 0 || nField > 3)
			throw Dcx::dcxException("Out of Range");
		
		this->setFocus(nField);
	}
	// This is to avoid invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
		//this->clearAddress();
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::setRange( const int nField, const BYTE iMin, const BYTE iMax ) {
	return SendMessage( m_Hwnd, IPM_SETRANGE, (WPARAM) nField, (LPARAM) MAKEIPRANGE( iMin, iMax ) );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::setFocus( const int nField ) {
	return SendMessage( m_Hwnd, IPM_SETFOCUS, (WPARAM) nField, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::setAddress( const DWORD dwIpAddress ) {
	return SendMessage( m_Hwnd, IPM_SETADDRESS, (WPARAM) 0, (LPARAM) dwIpAddress );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::getAddress( LPDWORD lpdwIpAddress ) const {
	return SendMessage( m_Hwnd, IPM_GETADDRESS, (WPARAM) 0, (LPARAM) lpdwIpAddress );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::clearAddress( ) {
	return SendMessage( m_Hwnd, IPM_CLEARADDRESS, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxIpAddress::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {
	case WM_NOTIFY:
		{
			dcxlParam(LPNMHDR, hdr);

			if (hdr == nullptr)
				break;

			if ( hdr->code == IPN_FIELDCHANGED )
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT))
					this->execAliasEx(TEXT("edit,%d"), getUserID( ) );
				bParsed = TRUE;
			}
		}
		break;
	}
	return 0L;
}

LRESULT DcxIpAddress::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch (uMsg) {

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_CONTEXTMENU:
	case WM_SETCURSOR:
		break;

	case WM_MOUSEACTIVATE:
	{
		if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {
			switch (HIWORD(lParam))
			{
			case WM_LBUTTONUP:
			{
				execAliasEx(TEXT("sclick,%d"), getUserID());
			}
			break;
			case WM_RBUTTONUP:
			{
				execAliasEx(TEXT("rclick,%d"), getUserID());
			}
			break;
			}
		}
		bParsed = TRUE;
		return MA_NOACTIVATE;
	}
	break;
	//case WM_SIZE:
	//	{
	//		this->redrawWindow();
	//	}
	//	break;
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
