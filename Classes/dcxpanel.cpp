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
 * � ScriptsDB.org - 2006
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
DcxPanel::DcxPanel(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog, DcxControlTypes::PANEL)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WS_EX_CONTROLPARENT,
		DCX_PANELCLASS,
		ws.m_Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		DcxUXModule::dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	this->m_pLayoutManager = std::make_unique<LayoutManager>(m_Hwnd);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxPanel::~DcxPanel() noexcept
{
}

void DcxPanel::toXml(TiXmlElement* const xml) const
{
	if (!xml || !m_Hwnd)
		return;

	__super::toXml(xml);

	if (m_pLayoutManager)
	{
		if (const auto rt = m_pLayoutManager->getRoot(); rt)
		{
			// NB: root cell can be other than pane, needs looked at more, should we just disallow this?
			if (rt->getType() != LayoutCell::CellType::PANE)
				xml->LinkEndChild(rt->toXml());
			else
				rt->toXml(xml);
			return;
		}
	}
	xml->SetAttribute("nocla", 1);
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
			if (!xctrl->Attribute("height"))
				xctrl->SetAttribute("height", rc.Height());
			if (!xctrl->Attribute("width"))
				xctrl->SetAttribute("width", rc.Width());

			xml->LinkEndChild(xctrl);
		}
	}
}

TiXmlElement* DcxPanel::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxPanel::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	const auto pd = getParentDialog();
	if (!pd)
		return;

	__super::fromXml(xDcxml, xThis);

	if (!xThis->Attribute("nocla"))
	{
		const auto szCascade = queryAttribute(xThis, "cascade", "v");
		const auto szMargin = queryAttribute(xThis, "margin", "0 0 0 0");

		TString tsInput;
		_ts_sprintf(tsInput, L"% % -l root \t +p% 0 0 0 0", pd->getName(), this->getUserID(), szCascade);
		this->parseCommandRequest(tsInput);
		_ts_sprintf(tsInput, L"% % -l space root \t + %", pd->getName(), this->getUserID(), szMargin);
		this->parseCommandRequest(tsInput);
	}

	// tsPath = the cla path to this element
	// xParent = this elements xml parent. dialog, pane, or control
	xmlParseElements(L"root"_ts, xThis, nullptr);
}

//void DcxPanel::xmlParseElements(const TString& tsPath, const TiXmlElement* xParent)
//{
//	if (!xParent || tsPath.empty())
//		return;
//
//	const auto pd = getParentDialog();
//	if (!pd)
//		return;
//
//	TString tsCurrentPath(tsPath);
//
//	int iCLA{ 1 };
//
//	// parse all child elements
//	for (auto xElement = xParent->FirstChildElement(); xElement; xElement = xElement->NextSiblingElement())
//	{
//		switch (std::hash<const char*>()(xElement->Value()))
//		{
//		case "pane"_hash:
//		{
//			// its a pane
//			TString tsTok;
//			tsTok.addtok(iCLA);
//			tsCurrentPath.puttok(tsTok, tsCurrentPath.numtok());
//
//			xmlAddPane(tsPath, tsCurrentPath, xElement);
//
//			++iCLA;
//		}
//		break;
//		case "control"_hash:
//		{
//			xmlAddControl(tsPath, tsCurrentPath, xParent, xElement);
//		}
//		break;
//		case "calltemplate"_hash:
//		{
//			xmlCallTemplate(tsCurrentPath, xElement);
//		}
//		break;
//		default:
//			break;
//		}
//	}
//}

//void DcxPanel::xmlAddPane(const TString& tsParentPath, const TString& tsCurrentPath, const TiXmlElement* xElement)
//{
//	if (!xElement || tsParentPath.empty())
//		return;
//
//	const auto pd = getParentDialog();
//	if (!pd)
//		return;
//
//	// cascade margin weight
//	auto szCascade = queryAttribute(xElement, "cascade");
//	auto szMargin = queryAttribute(xElement, "margin", "0 0 0 0");
//	const auto iWidth = queryIntAttribute(xElement, "width");
//	const auto iHeight = queryIntAttribute(xElement, "height");
//	const auto iWeight = queryIntAttribute(xElement, "weight", 1);
//
//	// xdid -l dname id root $chr(9) +p id weight w h
//	// name switch options
//	TString tsInput;
//	_ts_sprintf(tsInput, L"% % -l cell % \t +p% 0 % % %", pd->getName(), this->getUserID(), tsParentPath, szCascade, iWeight, iWidth, iHeight);
//	this->parseCommandRequest(tsInput);
//	_ts_sprintf(tsInput, L"% % -l space % \t + %", pd->getName(), this->getUserID(), tsCurrentPath, szMargin);
//	this->parseCommandRequest(tsInput);
//
//	xmlParseElements(tsCurrentPath, xElement);
//}

//bool DcxPanel::xmlAddControl(const TString& tsParentPath, const TString& tsCurrentPath, const TiXmlElement* xParent, const TiXmlElement* xCtrl)
//{
//	if (!xParent || !xCtrl || tsParentPath.empty())
//		return false;
//	const auto pd = getParentDialog();
//	if (!pd)
//		return false;
//
//	auto szX = queryAttribute(xCtrl, "x", "0");
//	auto szY = queryAttribute(xCtrl, "y", "0");
//	const auto iWidth = queryIntAttribute(xCtrl, "width");
//	const auto iHeight = queryIntAttribute(xCtrl, "height");
//	auto szID = queryAttribute(xCtrl, "id");
//	auto szType = queryAttribute(xCtrl, "type");
//	auto szStyles = queryAttribute(xCtrl, "styles");
//
//	// ID is NOT a number!
//	if (_ts_isEmpty(szID)) // needs looked at, think dcxml generates an id.
//		throw DcxExceptions::dcxInvalidItem();
//
//	// fixed position control, no cla
//	// xdid -c dname id [newid] [type] [x] [y] [width] [height] [styles...]
//	TString tsInput;
//	_ts_sprintf(tsInput, TEXT("% % % % % % %"), szID, szType, szX, szY, iWidth, iHeight, szStyles);
//	if (auto ctrl = pd->addControl(tsInput, 1, DcxAllowControls::ALLOW_ALL, m_Hwnd); ctrl)
//	{
//		ctrl->fromXml(xParent, xCtrl);
//
//		// x & y makes this a fixed control, not cla
//		if (!xCtrl->Attribute("x") && !xCtrl->Attribute("y") && !xParent->Attribute("nocla"))
//		{
//			// assume its cla now.
//			const auto iWeight = queryIntAttribute(xCtrl, "weight", 1);
//			TString tsFlags("i"); // id included
//			if (xCtrl->Attribute("width"))
//			{
//				tsFlags += L'f'; // fixed size
//				if (xCtrl->Attribute("height"))
//					tsFlags += L'w'; // both
//				else
//					tsFlags += L'h'; // horizontal
//
//			}
//			else if (xCtrl->Attribute("height"))
//				tsFlags += L"fv"; // fixed vertical
//			else
//				tsFlags += L'l'; // fill
//
//			{
//				_ts_sprintf(tsInput, L"% % -l cell % \t +% % % % %", pd->getName(), this->getUserID(), tsParentPath, tsFlags, szID, iWeight, iWidth, iHeight);
//				this->parseCommandRequest(tsInput);
//				return true;
//			}
//		}
//	}
//	return false;
//}

/*!
 * \brief blah
 *
 * blah
 */
dcxWindowStyles DcxPanel::parseControlStyles(const TString& tsStyles)
{
	return parseGeneralControlStyles(tsStyles);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */
void DcxPanel::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPanel::parseCommandRequest(const TString& input)
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

		const TString tsID(input.getnexttok());
		const auto ID = getParentDialog()->NameToID(tsID);

		if (!this->getParentDialog()->isIDValid(ID))
			throw Dcx::dcxException(TEXT("Unknown control with ID %(%) (dialog %)"), tsID, ID - mIRC_ID_OFFSET, this->getParentDialog()->getName());

		const auto p_Control = this->getParentDialog()->getControlByID(ID);
		if (!p_Control)
			throw Dcx::dcxException(TEXT("Unable to get control with ID %(%) (dialog %)"), tsID, ID - mIRC_ID_OFFSET, this->getParentDialog()->getName());

		if (const auto dct = p_Control->getControlType(); (dct == DcxControlTypes::DIALOG || dct == DcxControlTypes::WINDOW))
			delete p_Control;
		else {
			if (p_Control->getRefCount() != 0)
				throw Dcx::dcxException(TEXT("Can't delete control with ID \"%\" when it is inside it's own event (dialog %)"), p_Control->getUserID(), this->getParentDialog()->getName());

			const auto cHwnd = p_Control->getHwnd();
			this->getParentDialog()->deleteControl(p_Control); // remove from internal list!
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

		if (!m_pLayoutManager)
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
			m_pLayoutManager = std::make_unique<LayoutManager>(m_Hwnd);
		}
		break;
		default:
		{
			if (numtok < 9)
				throw DcxExceptions::dcxInvalidArguments();

			m_pLayoutManager->AddCell(input, 4, this->getParentDialog());
		}

		}
	}
	// xdid -t [NAME] [ID] [SWITCH] [TEXT]
	else if (flags[TEXT('t')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		SetWindowTextW(m_Hwnd, input.getlasttoks().to_wchr());	// tok 4, -1
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxPanel::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept
{
	return 0L;
}

LRESULT DcxPanel::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
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
		if (IsWindow(to_hwnd(lParam)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(to_hwnd(lParam), TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_COMPAREITEM:
	{
		dcxlParam(LPCOMPAREITEMSTRUCT, idata);

		if ((idata) && (IsWindow(idata->hwndItem)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(idata->hwndItem, TEXT("dcx_cthis")); c_this)
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

		//if (this->m_pLayoutManager)
		//{
		//	RECT rc;
		//	SetRect( &rc, 0, 0, LOWORD( lParam ), HIWORD( lParam ) );
		//	if (this->m_pLayoutManager->updateLayout( rc ))
		//		this->redrawWindow( );
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

			//if (this->m_pLayoutManager->updateLayout( rc ))
			//	this->redrawWindow( );
			m_pLayoutManager->updateLayout(rc);
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
		if (this->isExStyle(WindowExStyle::Transparent))
			this->DrawParentsBackground(reinterpret_cast<HDC>(wParam));
		else // normal bkg
			DcxControl::DrawCtrlBackground(reinterpret_cast<HDC>(wParam), this);

		// Update CLA if any.
		{
			if (m_pLayoutManager)
				if (RECT rc{}; GetClientRect(m_Hwnd, &rc))
					m_pLayoutManager->updateLayout(rc);
		}

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
		Auto(this->FinishAlphaBlend(ai));

		{ // simply fill with bkg
			if (this->isExStyle(WindowExStyle::Transparent))
			{
				if (!this->IsAlphaBlend())
					this->DrawParentsBackground(hdc);
			}
			else
				DcxControl::DrawCtrlBackground(hdc, this);
		}
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps{};

		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		bParsed = TRUE;

		{
			// Setup alpha blend if any.
			auto ai = this->SetupAlphaBlend(&hdc);
			Auto(this->FinishAlphaBlend(ai));

			{ // simply fill with bkg
				if (this->isExStyle(WindowExStyle::Transparent))
				{
					if (!this->IsAlphaBlend())
						this->DrawParentsBackground(hdc);
				}
				else
					DcxControl::DrawCtrlBackground(hdc, this);
			}
		}
	}
	break;

	case WM_DESTROY:
	{
		this->CallDefaultClassProc(uMsg, wParam, lParam);

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

LRESULT DcxPanel::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
