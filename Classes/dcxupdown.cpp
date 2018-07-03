/*!
 * \file dcxupdown.cpp
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
#include "Classes/dcxupdown.h"
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

DcxUpDown::DcxUpDown(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_UPDOWNCLASS,
		ws.m_Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	if (styles.istok(TEXT("tooltips")))
	{
		if (!IsWindow(p_Dialog->getToolTip()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		this->m_ToolTipHWND = p_Dialog->getToolTip();
		AddToolTipToolInfo(this->m_ToolTipHWND, m_Hwnd);
	}
	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxUpDown::~DcxUpDown()
{
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxUpDown::parseControlStyles(const TString & tsStyles)
{
	auto ws = parseGeneralControlStyles(tsStyles);

	ws.m_Styles |= UDS_ALIGNRIGHT;

	for (const auto tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"left"_hash:
		{
			ws.m_Styles &= ~UDS_ALIGNRIGHT;
			ws.m_Styles |= UDS_ALIGNLEFT;
		}
		break;
		case L"arrowkeys"_hash:
			ws.m_Styles |= UDS_ARROWKEYS;
			break;
		case L"horz"_hash:
			ws.m_Styles |= UDS_HORZ;
			break;
		case L"hottrack"_hash:
			ws.m_Styles |= UDS_HOTTRACK;
			break;
		case L"nothousands"_hash:
			ws.m_Styles |= UDS_NOTHOUSANDS;
			break;
		case L"buddyint"_hash:
			ws.m_Styles |= UDS_SETBUDDYINT;
			break;
		case L"wrap"_hash:
			ws.m_Styles |= UDS_WRAP;
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

void DcxUpDown::parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"value"_hash:
	{
		BOOL bError = FALSE;

		_ts_snprintf(szReturnValue, TEXT("%d"), getPos32(&bError));
	}
	break;
	// [NAME] [ID] [PROP]
	case L"range"_hash:
	{
		const auto[iMin, iMax] = getRange32();

		_ts_snprintf(szReturnValue, TEXT("%d %d"), iMin, iMax);
	}
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

void DcxUpDown::parseCommandRequest(const TString & input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -c [NAME] [ID] [SWITCH] [BUDDYID]
	if (flags[TEXT('c')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const auto tsID(input.getnexttok());
		const auto ID = getParentDialog()->NameToID(tsID);

		const auto *const p_Control = getParentDialog()->getControlByID(ID);	// tok 4

		if (p_Control == nullptr)
			throw Dcx::dcxException("Unable to get control");

		// Text notifications
		if (const auto &cType(p_Control->getControlType()); cType == DcxControlTypes::TEXT || cType == DcxControlTypes::EDIT)
			setBuddy(p_Control->getHwnd());

		//auto p_Control = getParentDialog()->getControlByID(input.getnexttok().to_<UINT>() + mIRC_ID_OFFSET);	// tok 4
		//
		//if (p_Control == nullptr)
		//	throw Dcx::dcxException("Unable to get control");
		//
		////TCHAR ClassName[256];
		////GetClassName( p_Control->getHwnd( ), ClassName, Dcx::countof(ClassName) );
		////
		////// Text notifications
		////if ( lstrcmpi( TEXT("STATIC"), ClassName ) == 0 || lstrcmpi( TEXT("EDIT"), ClassName ) == 0 )
		////	this->setBuddy( p_Control->getHwnd( ) );
		//
		////stString<256> sClassName;
		////GetClassName(p_Control->getHwnd(), sClassName, sClassName.size());
		////
		////// Text notifications
		////if (sClassName == TEXT("STATIC") || sClassName == TEXT("EDIT"))
		////	setBuddy(p_Control->getHwnd());
		//
		//const auto &cType(p_Control->getControlType());
		//// Text notifications
		//if (cType == DcxControlTypes::TEXT || cType == DcxControlTypes::EDIT)
		//	setBuddy(p_Control->getHwnd());

	}
	// xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags[TEXT('r')])
	{
		if (numtok < 5)
			throw Dcx::dcxException("Insufficient parameters");

		const auto iMin = input.getnexttok().to_int();	// tok 4
		const auto iMax = input.getnexttok().to_int();	// tok 5

		setRange32(iMin, iMax);
	}
	// xdid -t [NAME] [ID] [SWITCH] [BASE]
	else if (flags[TEXT('t')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const auto nBase = input.getnexttok().to_int();	// tok 4

		setBase(nBase);
	}
	// xdid -v [NAME] [ID] [SWITCH] [POS]
	else if (flags[TEXT('v')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const auto nPos = input.getnexttok().to_int();	// tok 4

		setPos32(nPos);
	}
	else
		parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::setBuddy(const HWND mHwnd) noexcept
{
	return SendMessage(m_Hwnd, UDM_SETBUDDY, (WPARAM)mHwnd, (LPARAM)0);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::setRange32(const int iLow, const int iHigh) noexcept
{
	return SendMessage(m_Hwnd, UDM_SETRANGE32, gsl::narrow_cast<WPARAM>(iLow), gsl::narrow_cast<LPARAM>(iHigh));
}

/*!
 * \brief blah
 *
 * blah
 */

std::pair<int, int> DcxUpDown::getRange32() const noexcept
{
	int iMin = 0, iMax = 0;
	SendMessage(m_Hwnd, UDM_GETRANGE32, (WPARAM)&iMin, (LPARAM)&iMax);
	return{ iMin,iMax };
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::setBase(const int iBase) noexcept
{
	return SendMessage(m_Hwnd, UDM_SETBASE, gsl::narrow_cast<WPARAM>(iBase), (LPARAM)0);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::getBase() const noexcept
{
	return SendMessage(m_Hwnd, UDM_GETBASE, (WPARAM)0, (LPARAM)0);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::setPos32(const INT nPos) noexcept
{
	return SendMessage(m_Hwnd, UDM_SETPOS32, (WPARAM)0, (LPARAM)nPos);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::getPos32(const LPBOOL pfError) const noexcept
{
	return SendMessage(m_Hwnd, UDM_GETPOS32, 0, (LPARAM)pfError);
}

const TString DcxUpDown::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, UDS_ALIGNLEFT))
		styles.addtok(TEXT("left"));
	if (dcx_testflag(Styles, UDS_HORZ))
		styles.addtok(TEXT("horz"));
	if (dcx_testflag(Styles, UDS_HOTTRACK))
		styles.addtok(TEXT("hottrack"));
	if (dcx_testflag(Styles, UDS_NOTHOUSANDS))
		styles.addtok(TEXT("nothousands"));
	if (dcx_testflag(Styles, UDS_SETBUDDYINT))
		styles.addtok(TEXT("buddyint"));
	if (dcx_testflag(Styles, UDS_WRAP))
		styles.addtok(TEXT("wrap"));
	return styles;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxUpDown::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed)
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
		case UDN_DELTAPOS:
		{
			if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("sclick,%u"), getUserID());
			bParsed = TRUE;
		}
		break;
		}
	}
	break;
	}
	return 0L;
}

LRESULT DcxUpDown::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed)
{
	switch (uMsg)
	{
	case WM_LBUTTONUP:
		break;

	case WM_DESTROY:
	{
		delete this;
		bParsed = TRUE;
	}
	break;

	default:
		return CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}

WNDPROC DcxUpDown::m_hDefaultClassProc = nullptr;

LRESULT DcxUpDown::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc != nullptr)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
