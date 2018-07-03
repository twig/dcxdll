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
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(	
		ws.m_ExStyles | WS_EX_CONTROLPARENT,
		DCX_PANELCLASS, 
		ws.m_Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (ws.m_NoTheme )
		Dcx::UXModule.dcxSetWindowTheme( m_Hwnd , L" ", L" " );

	this->m_pLayoutManager = std::make_unique<LayoutManager>(m_Hwnd);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxPanel::~DcxPanel( )
{
}

void DcxPanel::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);

	m_pLayoutManager->getRoot()->toXml(xml);
}

TiXmlElement * DcxPanel::toXml(void) const
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
dcxWindowStyles DcxPanel::parseControlStyles(const TString & tsStyles)
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
void DcxPanel::parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPanel::parseCommandRequest( const TString & input )
{
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if ( flags[TEXT('c')] )
	{
		if (numtok < 9)
			throw Dcx::dcxException("Insufficient parameters");

		getParentDialog()->addControl(input, 4, CTLF_ALLOW_ALL, m_Hwnd);
		redrawWindow();
	}
	// xdid -d [NAME] [ID] [SWITCH] [ID]
	else if ( flags[TEXT('d')] )
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const auto ID = mIRC_ID_OFFSET + input.getnexttok().to_<UINT>();	// tok 4

		if (!this->getParentDialog()->isIDValid(ID))
			throw Dcx::dcxException(TEXT("Unknown control with ID \"%\" (dialog %)"), ID - mIRC_ID_OFFSET, this->getParentDialog()->getName());

		const auto p_Control = this->getParentDialog()->getControlByID(ID);
		if (p_Control == nullptr)
			throw Dcx::dcxException(TEXT("Unable to get control with ID \"%\" (dialog %)"), ID - mIRC_ID_OFFSET, this->getParentDialog()->getName());

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
	else if ( flags[TEXT('l')] )
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		if (m_pLayoutManager == nullptr)
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
			//delete m_pLayoutManager;
			//m_pLayoutManager = new LayoutManager(m_Hwnd);
			////redrawWindow(); // dont redraw here, leave that for an `update` cmd

			//m_pLayoutManager.reset(new LayoutManager(m_Hwnd));
			m_pLayoutManager = std::make_unique<LayoutManager>(m_Hwnd);
		}
		break;
		default:
		{
			if (numtok < 9)
				throw Dcx::dcxException("Insufficient parameters");

			m_pLayoutManager->AddCell(input, 4);
		}

		}
	}
	// xdid -t [NAME] [ID] [SWITCH] [TEXT]
	else if (flags[TEXT('t')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

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
LRESULT DcxPanel::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) noexcept
{
	return 0L;
}

LRESULT DcxPanel::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed)
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
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
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

	case WM_COMPAREITEM:
	{
		dcxlParam(LPCOMPAREITEMSTRUCT, idata);

		if ((idata != nullptr) && (IsWindow(idata->hwndItem)))
		{
			if (const auto c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis"))); c_this != nullptr)
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
		if (const auto cHwnd = GetDlgItem(m_Hwnd, static_cast<int>(wParam)); IsWindow(cHwnd))
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
		if (lParam != NULL)
		{
			dcxlParam(LPWINDOWPOS, wp);

			if (this->m_pLayoutManager != nullptr)
			{
				RECT rc{ 0, 0, wp->cx, wp->cy };

				//if (this->m_pLayoutManager->updateLayout( rc ))
				//	this->redrawWindow( );
				this->m_pLayoutManager->updateLayout(rc);
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
		if (this->isExStyle(WindowExStyle::Transparent))
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

WNDPROC DcxPanel::m_hDefaultClassProc = nullptr;

LRESULT DcxPanel::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc != nullptr)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
