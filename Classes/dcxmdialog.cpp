/*!
 * \file dcxmdialog.cpp
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
#include "Classes/dcxmdialog.h"
#include "Classes/dcxdialog.h"

 /*!
  * \brief Constructor
  *
  * \param cHwnd Attached dialog Handle
  * \param pHwnd Parent Window
  * \param ID Control ID
  * \param p_Dialog Parent DcxDialog Object
  * \param rc Window Rectangle
  * \param styles Window Style Tokenized List
  */

DcxMDialog::DcxMDialog(const HWND cHwnd, const HWND pHwnd, const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	parseControlStyles(styles);

	m_Hwnd = cHwnd;
	this->m_OrigParentHwnd = GetParent(m_Hwnd);

	this->m_OrigStyles = this->removeStyle(WindowStyle::Caption | DS_FIXEDSYS | DS_SETFONT | DS_3DLOOK | DS_MODALFRAME | WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);

	this->m_OrigExStyles = this->setExStyle(WindowExStyle::ControlParent);

	this->addStyle(WindowStyle::Child);

	SetParent(m_Hwnd, pHwnd);
	SetWindowPos(m_Hwnd, nullptr, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0);
	ShowWindow(m_Hwnd, SW_SHOWNOACTIVATE);
	UpdateWindow(m_Hwnd);

	this->m_OrigID = dcxSetWindowID(m_Hwnd, ID);

	SetProp(this->m_Hwnd, TEXT("dcx_cthis"), this);
}

/*!
 * \brief Destructor
 *
 * blah
 */

DcxMDialog::~DcxMDialog() noexcept
{
	auto parent = GetParent(m_Hwnd);
	if ((parent == this->m_OrigParentHwnd) && (this->m_OrigParentHwnd != this->getParentDialog()->getHwnd()))
		return;

	if (!m_DeleteByDestroy)
	{ // all this isn't needed if control is deleted because of closing the dialog
		const auto bHide = (IsWindowVisible(m_Hwnd) != FALSE);
		if (!bHide)
			ShowWindow(m_Hwnd, SW_HIDE);

		dcxSetWindowID(m_Hwnd, m_OrigID);

		//this->removeStyle(WS_CHILD);
		//this->addStyle(WS_POPUP);
		//SetParent( m_Hwnd, nullptr );

		if (parent == this->m_OrigParentHwnd) // handles oddness where orig parent == current when it shouldnt, maybe due to init event docking.
			parent = GetParent(parent);
		else
			parent = this->m_OrigParentHwnd;

		SetParent(m_Hwnd, parent);
		//SetParent( m_Hwnd, this->m_OrigParentHwnd);
		this->setStyle(this->m_OrigStyles);
		this->setExStyle(this->m_OrigExStyles);

		SetWindowPos(m_Hwnd, nullptr, 30, 30, 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED);
		this->redrawWindow();

		if (!bHide)
			ShowWindow(m_Hwnd, SW_SHOW);
	}
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxMDialog::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	// [NAME] [ID] [PROP]
	if (input.gettok(3) == TEXT("dname"))
		szReturnValue = m_OrigName.to_chr();
	else
		parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxMDialog::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));

	this->parseGlobalCommandRequest(input, flags);
}

dcxWindowStyles DcxMDialog::parseControlStyles(const TString& tsStyles)
{
	m_OrigName = tsStyles;

	return{ };
}

/*!
 * \brief Subclassed Window Procedure
 *
 * \param uMsg Window Procedure Message ID
 * \param wParam Window Procedure WPARAM
 * \param lParam Window Procedure LPARAM
 * \param bParsed Indicates if subclassed procedure parsed the message
 */
LRESULT DcxMDialog::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept
{
	return 0L;
}

LRESULT DcxMDialog::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{

	switch (uMsg)
	{
	case WM_DESTROY:
	{
		auto wnd = this->m_hDefaultClassProc;
		auto mHwnd = m_Hwnd;
		m_DeleteByDestroy = true;
		delete this;
		ShowWindow(mHwnd, SW_HIDE);
		CallWindowProc(wnd, mHwnd, uMsg, wParam, lParam);
		bParsed = TRUE;
	}
	break;

	default:
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}
	return 0L;
}

LRESULT DcxMDialog::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
