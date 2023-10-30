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

DcxPanel::DcxPanel(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
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
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

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

		SetWindowText(m_Hwnd, input.getlasttoks().to_chr());	// tok 4, -1
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
		if (IsWindow(reinterpret_cast<HWND>(lParam)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(reinterpret_cast<HWND>(lParam), TEXT("dcx_cthis")); c_this)
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
