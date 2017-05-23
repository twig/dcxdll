/*!
 * \file dcxdivider.cpp
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
#include "Classes/dcxdivider.h"
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

DcxDivider::DcxDivider(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	m_Hwnd = CreateWindowEx(
		static_cast<DWORD>(ExStyles) | WS_EX_CONTROLPARENT,
		DCX_DIVIDERCLASS,
		nullptr,
		WS_CHILD | static_cast<DWORD>(Styles),
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDivider::~DcxDivider() {

	this->unregistreDefaultWindowProc();
}

/*!
 * \brief blah
 *
 * blah
 */

const TString DcxDivider::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, DVS_VERT))
		styles.addtok(TEXT("vertical"));

	return styles;
}

void DcxDivider::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	*Styles |= DVS_HORZ;

	//for (auto tsStyle(styles.getfirsttok(1)); !tsStyle.empty(); tsStyle = styles.getnexttok())
	//{
	//	if ( tsStyle == TEXT("vertical") )
	//		*Styles |= DVS_VERT;
	//}

	if (styles.istok(TEXT("vertical")))
		*Styles |= DVS_VERT;

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

void DcxDivider::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP]
	if (prop == TEXT("position")) {
		auto iDivPos = 0;

		SendMessage(m_Hwnd, DV_GETDIVPOS, (WPARAM) NULL, (LPARAM) &iDivPos);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), iDivPos);
	}
	else if (prop == TEXT("isvertical"))
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), (GetWindowStyle(m_Hwnd) & DVS_VERT));
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDivider::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));

	const auto numtok = input.numtok();

	// xdid -l|r [NAME] [ID] [SWITCH] [MIN] [IDEAL][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if ( ( flags[TEXT('l')] || flags[TEXT('r')] ) && numtok > 9 ) {

		DVPANEINFO dvpi;
		ZeroMemory( &dvpi, sizeof( DVPANEINFO ) );
		dvpi.cbSize = sizeof( DVPANEINFO );

		const auto data(input.getfirsttok(1, TSTABCHAR).trim());
		TString control_data;

		if ( input.numtok( TSTABCHAR) > 1 )
			control_data = input.getnexttok( TSTABCHAR).trim();	// tok 2

		dvpi.fMask = DVPIM_CHILD | DVPIM_MIN | DVPIM_IDEAL;
		dvpi.cxMin = data.getfirsttok( 4 ).to_<UINT>( );
		dvpi.cxIdeal = data.getnexttok( ).to_<UINT>( );	// tok 5

		if (control_data.numtok() < 6)
			throw Dcx::dcxException("Insufficient Parameters");

		const auto ID = mIRC_ID_OFFSET + control_data.gettok(1).to_<UINT>();

		if (!m_pParentDialog->isIDValid(ID, true))
			throw Dcx::dcxException(TEXT("Control with ID \"%\" already exists"), ID - mIRC_ID_OFFSET);

		try {
			//auto p_Control = DcxControl::controlFactory(m_pParentDialog, ID, control_data, 2, CTLF_ALLOW_ALLBUTDOCK, m_Hwnd); // <- never NULL

			//m_pParentDialog->addControl( p_Control );

			auto p_Control = m_pParentDialog->addControl(control_data, 1, CTLF_ALLOW_ALLBUTDOCK, m_Hwnd);

			dvpi.hChild = p_Control->getHwnd( );

			if ( flags[TEXT('l')] )
				setPane( DVF_PANELEFT, &dvpi );
			else if ( flags[TEXT('r')] )
				setPane( DVF_PANERIGHT, &dvpi );

			redrawWindow( );
		}
		catch (const std::exception &e) {
			//showErrorEx(nullptr, TEXT("-c"), TEXT("Unable To Create Control %d (%S)"), ID - mIRC_ID_OFFSET, e.what());
			showError(nullptr, TEXT("-c"), TEXT("Unable To Create Control % (%)"), ID - mIRC_ID_OFFSET, e.what());
			throw;
		}
	}
	// xdid -v [NAME] [ID] [SWITCH] [POS]
	else if (flags[TEXT('v')] && numtok > 3) {
		if (!setDivPos(input.getnexttok( ).to_<UINT>()))	// tok 4
			throw Dcx::dcxException("Divider position must be between bounds.");
	}
	else
		parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxDivider::setPane( const UINT iPaneId, LPDVPANEINFO lpdvpi ) {
  return SendMessage( m_Hwnd, DV_SETPANE, (WPARAM) iPaneId, (LPARAM) lpdvpi );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxDivider::setDivPos( const UINT iDivPos ) {
  return SendMessage( m_Hwnd, DV_SETDIVPOS, (WPARAM) 0U, (LPARAM) iDivPos );
}

void DcxDivider::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());

	DVPANEINFO left;
	DVPANEINFO right;
	Divider_GetChildControl(m_Hwnd, DVF_PANELEFT, &left);
	Divider_GetChildControl(m_Hwnd, DVF_PANELEFT, &right);
	if (left.hChild != nullptr) {
		auto dcxcleft = this->m_pParentDialog->getControlByHWND(left.hChild);
		if (dcxcleft != nullptr)
			xml->LinkEndChild(dcxcleft->toXml());
		else
			xml->LinkEndChild(new TiXmlElement("control"));
	}
	else
		xml->LinkEndChild(new TiXmlElement("control"));
	if (right.hChild != nullptr) {
		auto dcxcright = this->m_pParentDialog->getControlByHWND(right.hChild);
		if (dcxcright != nullptr)
			xml->LinkEndChild(dcxcright->toXml());
		else
			xml->LinkEndChild(new TiXmlElement("control"));
	}
	else
		xml->LinkEndChild(new TiXmlElement("control"));
}

TiXmlElement * DcxDivider::toXml(void) const
{
	auto xml = __super::toXml();

	xml->SetAttribute("styles", getStyles().c_str());

	DVPANEINFO left;
	DVPANEINFO right;
	Divider_GetChildControl(m_Hwnd, DVF_PANELEFT, &left);
	Divider_GetChildControl(m_Hwnd, DVF_PANELEFT, &right);
	if (left.hChild != nullptr) {
		auto dcxcleft = this->m_pParentDialog->getControlByHWND(left.hChild);
		if (dcxcleft != nullptr)
			xml->LinkEndChild(dcxcleft->toXml());
		else
			xml->LinkEndChild(new TiXmlElement("control"));
	}
	else
		xml->LinkEndChild(new TiXmlElement("control"));
	if (right.hChild != nullptr) {
		auto dcxcright = this->m_pParentDialog->getControlByHWND(right.hChild);
		if (dcxcright != nullptr)
			xml->LinkEndChild(dcxcright->toXml());
		else
			xml->LinkEndChild(new TiXmlElement("control"));
	}
	else
		xml->LinkEndChild(new TiXmlElement("control"));

	return xml;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxDivider::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxDivider::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

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

	case WM_DESTROY:
		{
			delete this;
			bParsed = TRUE;
		}
		break;

	case DV_CHANGEPOS:
		{
			const auto phase = static_cast<int>(wParam);
			const auto pt = reinterpret_cast<LPPOINT>(lParam);

			this->execAliasEx(TEXT("%s,%d,%d,%d"), (phase == DVNM_DRAG_START ? TEXT("dragbegin") : (phase == DVNM_DRAG_END ? TEXT("dragfinish") : TEXT("drag"))), this->getUserID(), pt->x, pt->y);
		}
		break;

	default:
		lRes = this->CommonMessage( uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}
