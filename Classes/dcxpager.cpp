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

DcxPager::DcxPager(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
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

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	//Pager_SetButtonSize(m_Hwnd,15);
	//Pager_SetBkColor(m_Hwnd,0);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxPager::~DcxPager() noexcept
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

void DcxPager::toXml(TiXmlElement* const xml) const
{
	if (!xml || !m_Hwnd)
		return;

	__super::toXml(xml);

	//const auto pd = this->getParentDialog();
	//if (!pd)
	//	return;

	xml->SetAttribute("styles", getStyles().c_str());
	// no cla allowed in pager control...
	//xml->SetAttribute("nocla", 1);
	xml->SetAttribute("bordersize", Pager_GetBorder(m_Hwnd));
	xml->SetAttribute("buttonsize", Pager_GetButtonSize(m_Hwnd));
	setColourAttribute(xml, "bgcolour", Pager_GetBkColor(m_Hwnd));
	xml->SetAttribute("pos", Pager_GetPos(m_Hwnd));

	auto pChild = Dcx::dcxGetProp<DcxControl*>(this->m_ChildHWND, TEXT("dcx_cthis"));
	if (!pChild)
		return;

	const Dcx::dcxWindowRect rc(this->m_ChildHWND, m_Hwnd);

	if (auto xctrl = pChild->toXml(); xctrl)
	{
		xctrl->SetAttribute("x", rc.left);
		xctrl->SetAttribute("y", rc.top);
		if (!xctrl->Attribute("height"))
			xctrl->SetAttribute("height", rc.Height());
		if (!xctrl->Attribute("width"))
			xctrl->SetAttribute("width", rc.Width());

		xml->LinkEndChild(xctrl);
	}
}

void DcxPager::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	const auto pd = this->getParentDialog();
	if (!pd)
		return;

	__super::fromXml(xDcxml, xThis);

	if (const auto tmp = queryColourAttribute(xThis, "bgcolour"); tmp != CLR_INVALID)
		Pager_SetBkColor(m_Hwnd, tmp);

	Pager_SetBorder(m_Hwnd, queryIntAttribute(xThis, "bordersize"));
	Pager_SetButtonSize(m_Hwnd, queryIntAttribute(xThis, "buttonsize"));

	if (auto xCtrl = xThis->FirstChildElement("control"); xCtrl)
	{
		const auto iX = queryIntAttribute(xCtrl, "x");
		const auto iY = queryIntAttribute(xCtrl, "y");
		const auto iWidth = queryIntAttribute(xCtrl, "width");
		const auto iHeight = queryIntAttribute(xCtrl, "height");
		TString tsID(queryAttribute(xCtrl, "id"));
		auto szType = queryAttribute(xCtrl, "type");
		auto szStyles = queryAttribute(xCtrl, "styles");

		// ID is NOT a number!
		if (tsID.empty()) // no id, generate one.
			tsID.addtok(getParentDialog()->getUniqueID());

		TString tsInput;
		_ts_sprintf(tsInput, TEXT("% % % % % % %"), tsID, szType, iX, iY, iWidth, iHeight, szStyles);
		if (auto ctrl = pd->addControl(tsInput, 1,
			DcxAllowControls::ALLOW_TOOLBAR |
			DcxAllowControls::ALLOW_REBAR |
			DcxAllowControls::ALLOW_PANEL |
			DcxAllowControls::ALLOW_TAB |
			DcxAllowControls::ALLOW_BOX |
			DcxAllowControls::ALLOW_IMAGE |
			DcxAllowControls::ALLOW_PAGER |
			DcxAllowControls::ALLOW_DOCK,
			m_Hwnd); ctrl)
		{
			ctrl->fromXml(xThis, xCtrl);

			ctrl->addStyle(WindowStyle::CCS_NoResize);
			this->setChild(ctrl->getHwnd());
		}
	}

	Pager_SetPos(m_Hwnd, queryIntAttribute(xThis, "pos")); // set after any control is added.
}

TiXmlElement* DcxPager::toXml(void) const
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

dcxWindowStyles DcxPager::parseControlStyles(const TString& tsStyles)
{
	auto ws = parseGeneralControlStyles(tsStyles);

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"horizontal"_hash:
			ws.m_Styles |= PGS_HORZ;
			break;
		case L"autoscroll"_hash:
			ws.m_Styles |= PGS_AUTOSCROLL;
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

void DcxPager::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
	case L"colour"_hash:
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

void DcxPager::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -b [NAME] [ID] [SWITCH] [W]
	if (flags[TEXT('b')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		this->setBorderSize(input.getnexttok().to_int());	// tok 4
	}
	// xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	else if (flags[TEXT('c')])
	{
		if (numtok < 9)
			throw DcxExceptions::dcxInvalidArguments();

		if (IsWindow(this->m_ChildHWND))
			throw Dcx::dcxException("Child Control already exists");

		if (const auto pd = this->getParentDialog(); pd)
		{
			const auto p_Control = pd->addControl(input, 4,
				DcxAllowControls::ALLOW_TOOLBAR |
				DcxAllowControls::ALLOW_REBAR |
				DcxAllowControls::ALLOW_PANEL |
				DcxAllowControls::ALLOW_TAB |
				DcxAllowControls::ALLOW_BOX |
				DcxAllowControls::ALLOW_IMAGE |
				DcxAllowControls::ALLOW_PAGER |
				DcxAllowControls::ALLOW_DOCK,
				m_Hwnd);

			p_Control->addStyle(WindowStyle::CCS_NoResize);
			this->setChild(p_Control->getHwnd());
		}
	}
	// xdid -d [NAME] [ID] [SWITCH] [ID]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto pd = this->getParentDialog();
		if (!pd)
			throw DcxExceptions::dcxInvalidCommand();

		const auto tsID(input.getnexttok());		// tok 4
		const auto ID = pd->NameToID(tsID);

		if (!pd->isIDValid(ID))
			throw Dcx::dcxException(TEXT("Unknown control with ID %(%) (dialog %)"), tsID, ID - mIRC_ID_OFFSET, pd->getName());

		const auto p_Control = pd->getControlByID(ID);
		// Ook: no ref count check for dialog or window? needs checked

		if (!p_Control)
			throw Dcx::dcxException(TEXT("Unable to get control with ID %(%) (dialog %)"), tsID, ID - mIRC_ID_OFFSET, pd->getName());

		if (const auto dct = p_Control->getControlType(); (dct == DcxControlTypes::DIALOG || dct == DcxControlTypes::WINDOW))
			delete p_Control;
		else {
			if (p_Control->getRefCount() != 0)
				throw Dcx::dcxException(TEXT("Can't delete control with ID \"%\" when it is inside it's own event (dialog %)"), p_Control->getUserID(), pd->getName());

			auto cHwnd = p_Control->getHwnd();
			pd->deleteControl(p_Control); // remove from internal list!
			DestroyWindow(cHwnd);
		}

		this->m_ChildHWND = nullptr;
	}
	// xdid -s [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('s')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		this->setButtonSize(input.getlasttoks().to_<LONG>());	// tok 4, -1
	}
	// xdid -t [NAME] [ID] [SWITCH] [COLOR]
	else if (flags[TEXT('t')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		this->setBkColor(input.getlasttoks().to_<COLORREF>());	// tok 4, -1
	}
	// xdid -z [NAME] [ID] [SWITCH]
	else if (flags[TEXT('z')])
	{
		if (numtok < 3)
			throw DcxExceptions::dcxInvalidArguments();

		this->reCalcSize();
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

void DcxPager::setChild(const HWND child) noexcept
{
	this->m_ChildHWND = child;
	Pager_SetChild(m_Hwnd, child);
}

void DcxPager::setBkColor(const COLORREF c) noexcept
{
	Pager_SetBkColor(m_Hwnd, c);
}

void DcxPager::setBorderSize(const int bSize) noexcept
{
	Pager_SetBorder(m_Hwnd, bSize);
}

void DcxPager::setButtonSize(const int bSize) noexcept
{
	Pager_SetButtonSize(m_Hwnd, bSize);
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
LRESULT DcxPager::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept
{
	switch (uMsg)
	{
	case WM_NOTIFY:
	{

		dcxlParam(LPNMHDR, hdr);

		if (!hdr)
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

			const auto* const cthis = Dcx::dcxGetProp<DcxControl*>(this->m_ChildHWND, TEXT("dcx_cthis"));
			if (!cthis)
				break;

			if (const auto bSize = Pager_GetButtonSize(m_Hwnd); (cthis->getControlType() == DcxControlTypes::TOOLBAR))
			{
				SIZE sz{};
				SendMessage(m_ChildHWND, TB_GETMAXSIZE, 0, reinterpret_cast<LPARAM>(&sz));
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
		default:
			break;
		}
	}
	break;
	default:
		break;
	}
	return 0L;
}

LRESULT DcxPager::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
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
			{
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
#pragma warning(push)
#pragma warning(disable: 26454)	//: warning C26454 : Arithmetic overflow : '-' operation produces a negative unsigned result at compile time(io.5).
				if ((c_this->getControlType() == DcxControlTypes::TOOLBAR) && (hdr->code == TBN_DELETINGBUTTON))
				{
					// handle toolbar button delete.
					reCalcSize();
				}
#pragma warning(pop)
			}
		}
	}
	break;

	case WM_HSCROLL:
	case WM_VSCROLL:
	case WM_COMMAND:
	{
		if (IsWindow(to_hwnd(lParam)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(to_hwnd(lParam), TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_DELETEITEM:
	{
		dcxlParam(LPDELETEITEMSTRUCT, idata);
		if ((idata) && (IsWindow(idata->hwndItem)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(idata->hwndItem, TEXT("dcx_cthis")); c_this)
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
		dcxlParam(LPDRAWITEMSTRUCT, idata);

		if ((idata) && (IsWindow(idata->hwndItem)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(idata->hwndItem, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_SIZE:
	{
		HandleChildControlSize();

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

LRESULT DcxPager::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
