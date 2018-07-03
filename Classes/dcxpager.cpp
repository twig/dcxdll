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
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WS_EX_CONTROLPARENT,
		DCX_PAGERCLASS,
		ws.m_Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (ws.m_NoTheme )
		Dcx::UXModule.dcxSetWindowTheme( m_Hwnd , L" ", L" " );

	//Pager_SetButtonSize(m_Hwnd,15);
	//Pager_SetBkColor(m_Hwnd,0);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxPager::~DcxPager( )
{
}

const TString DcxPager::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, PGS_HORZ))
		styles.addtok(TEXT("horizontal"));
	if (dcx_testflag(Styles, PGS_AUTOSCROLL))
		styles.addtok(TEXT("autoscroll"));

	return styles;
}

void DcxPager::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());

	const auto *const child = this->getParentDialog()->getControlByHWND(this->m_ChildHWND);
	if (child != nullptr)
		xml->LinkEndChild(child->toXml());
}

TiXmlElement * DcxPager::toXml(void) const
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

dcxWindowStyles DcxPager::parseControlStyles(const TString & tsStyles)
{
	auto ws = parseGeneralControlStyles(tsStyles);

	for (const auto &tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"horizontal"_hash:
			ws.m_Styles |= PGS_HORZ;
			break;
		case L"autoscroll"_hash:
			ws.m_Styles |= PGS_AUTOSCROLL;
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

void DcxPager::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
	case L"color"_hash:
		_ts_snprintf(szReturnValue, TEXT("%u"), Pager_GetBkColor(m_Hwnd));
		break;
	case L"bsize"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), Pager_GetButtonSize(m_Hwnd));
		break;
	case L"border"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), Pager_GetBorder(m_Hwnd));
		break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPager::parseCommandRequest( const TString & input )
{
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -b [NAME] [ID] [SWITCH] [W]
	if ( flags[TEXT('b')] )
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		this->setBorderSize(input.getnexttok( ).to_int());	// tok 4
	}
	// xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	else if ( flags[TEXT('c')] )
	{
		if (numtok < 9)
			throw Dcx::dcxException("Insufficient parameters");

		if (IsWindow(this->m_ChildHWND))
			throw Dcx::dcxException("Child Control already exists");

		const auto p_Control = this->getParentDialog()->addControl(input, 4,
			CTLF_ALLOW_TOOLBAR |
			CTLF_ALLOW_REBAR |
			CTLF_ALLOW_PANEL |
			CTLF_ALLOW_TAB |
			CTLF_ALLOW_BOX |
			CTLF_ALLOW_IMAGE |
			CTLF_ALLOW_PAGER |
			CTLF_ALLOW_DOCK
			, m_Hwnd);

		p_Control->addStyle(WindowStyle::CCS_NoResize);
		this->setChild(p_Control->getHwnd());
	}
	// xdid -d [NAME] [ID] [SWITCH] [ID]
	else if ( flags[TEXT('d')] )
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const auto tsID(input.getnexttok( ));		// tok 4
		const auto ID = this->getParentDialog()->NameToID(tsID);

		if ( !this->getParentDialog()->isIDValid(ID) )
			throw Dcx::dcxException(TEXT("Unknown control with ID \"%\" (dialog %)"), tsID, this->getParentDialog()->getName());

		const auto p_Control = this->getParentDialog()->getControlByID(ID);
		// Ook: no ref count check for dialog or window? needs checked

		if (p_Control == nullptr)
			throw Dcx::dcxException(TEXT("Unable to get control with ID \"%\" (dialog %)"), tsID, this->getParentDialog()->getName());

		if (const auto dct = p_Control->getControlType(); (dct == DcxControlTypes::DIALOG || dct == DcxControlTypes::WINDOW))
			delete p_Control;
		else {
			if (p_Control->getRefCount() != 0)
				throw Dcx::dcxException(TEXT("Can't delete control with ID \"%\" when it is inside it's own event (dialog %)"), p_Control->getUserID(), this->getParentDialog()->getName());

			auto cHwnd = p_Control->getHwnd();
			this->getParentDialog()->deleteControl(p_Control); // remove from internal list!
			DestroyWindow(cHwnd);
		}

		this->m_ChildHWND = nullptr;
	}
	// xdid -s [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('s')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		this->setButtonSize(input.getlasttoks().to_<LONG>());	// tok 4, -1
	}
	// xdid -t [NAME] [ID] [SWITCH] [COLOR]
	else if (flags[TEXT('t')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		this->setBkColor(input.getlasttoks().to_<COLORREF>());	// tok 4, -1
	}
	// xdid -z [NAME] [ID] [SWITCH]
	else if (flags[TEXT('z')])
	{
		if (numtok < 3)
			throw Dcx::dcxException("Insufficient parameters");

		this->reCalcSize();
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

void DcxPager::setChild(const HWND child) noexcept
{
	this->m_ChildHWND = child;
	Pager_SetChild(m_Hwnd,child);
}

void DcxPager::setBkColor(const COLORREF c) noexcept
{
	Pager_SetBkColor(m_Hwnd,c);
}

void DcxPager::setBorderSize(const int bSize) noexcept
{
	Pager_SetBorder(m_Hwnd,bSize);
}

void DcxPager::setButtonSize(const int bSize) noexcept
{
	Pager_SetButtonSize(m_Hwnd,bSize);
}

void DcxPager::reCalcSize(void) const noexcept
{
	Pager_RecalcSize(m_Hwnd);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxPager::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) noexcept
{
	switch (uMsg)
	{
	case WM_NOTIFY:
	{

		dcxlParam(LPNMHDR, hdr);

		if (hdr == nullptr)
			break;

		switch (hdr->code)
		{
		case PGN_CALCSIZE:
		{
			bParsed = TRUE;
			dcxlParam(LPNMPGCALCSIZE, lpnmcs);

			//GetWindowRect(this->m_ChildHWND,&rc);
			//if (lpnmcs->dwFlag == PGF_CALCHEIGHT)
			//	lpnmcs->iHeight = (rc.bottom - rc.top);
			//else
			//	lpnmcs->iWidth = (rc.right - rc.left);

			const auto *const cthis = static_cast<DcxControl *>(GetProp(this->m_ChildHWND, TEXT("dcx_cthis")));
			if (cthis == nullptr)
				break;

			if (const auto bSize = Pager_GetButtonSize(m_Hwnd); (cthis->getControlType() == DcxControlTypes::TOOLBAR))
			{
				SIZE sz{};
				SendMessage(m_ChildHWND, TB_GETMAXSIZE, 0, (LPARAM)&sz);
				if (lpnmcs->dwFlag == PGF_CALCHEIGHT)
					lpnmcs->iHeight = sz.cy + bSize;
				else
					lpnmcs->iWidth = sz.cx + bSize;
			}
			else if (RECT rc{}; GetWindowRect(m_ChildHWND, &rc))
			{
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

LRESULT DcxPager::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed)
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
			if (const auto c_this = static_cast<DcxControl *>(GetProp(hdr->hwndFrom, TEXT("dcx_cthis"))); c_this != nullptr)
			{
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				if ((c_this->getControlType() == DcxControlTypes::TOOLBAR) && (hdr->code == TBN_DELETINGBUTTON))
				{
					// handle toolbar button delete.
					reCalcSize();
				}
			}
		}
	}
	break;

	case WM_HSCROLL:
	case WM_VSCROLL:
	case WM_COMMAND:
	{
		if (IsWindow((HWND)lParam))
		{
			if (const auto c_this = static_cast<DcxControl *>(GetProp((HWND)lParam, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_DELETEITEM:
	{
		dcxlParam(LPDELETEITEMSTRUCT, idata);
		if ((idata != nullptr) && (IsWindow(idata->hwndItem)))
		{
			if (const auto c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_MEASUREITEM:
	{
		if (const auto cHwnd = GetDlgItem(m_Hwnd, gsl::narrow_cast<int>(wParam)); IsWindow(cHwnd))
		{
			if (const auto c_this = static_cast<DcxControl *>(GetProp(cHwnd, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_DRAWITEM:
	{
		dcxlParam(LPDRAWITEMSTRUCT, idata);

		if ((idata != nullptr) && (IsWindow(idata->hwndItem)))
		{
			if (const auto c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_SIZE:
	{
		HandleChildrenSize();

		reCalcSize();
		redrawWindow();
	}
	break;

	case WM_SETFOCUS:
	{
		this->getParentDialog()->setFocusControl(this->getUserID());
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
		lRes = this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}

WNDPROC DcxPager::m_hDefaultClassProc = nullptr;

LRESULT DcxPager::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc != nullptr)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
