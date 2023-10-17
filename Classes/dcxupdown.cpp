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

DcxUpDown::DcxUpDown(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
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

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	if (styles.istok(TEXT("tooltips")))
	{
		if (!IsWindow(p_Dialog->getToolTipHWND()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		setToolTipHWND(p_Dialog->getToolTipHWND());
		AddToolTipToolInfo(getToolTipHWND(), m_Hwnd);
	}
	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxUpDown::~DcxUpDown() noexcept
{
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxUpDown::parseControlStyles(const TString& tsStyles)
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
void DcxUpDown::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
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
		const auto [iMin, iMax] = getRange32();

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

void DcxUpDown::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -c [NAME] [ID] [SWITCH] [BUDDYID]
	if (flags[TEXT('c')])
	{
		static_assert(CheckFreeCommand(TEXT('c')), "Command in use!");

		if (numtok < 4)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsID(input.getnexttok());
		const auto ID = getParentDialog()->NameToID(tsID);

		const auto* const p_Control = getParentDialog()->getControlByID(ID);	// tok 4

		if (!p_Control)
			throw Dcx::dcxException("Unable to get control");

		// Text notifications
		if (const auto & cType(p_Control->getControlType()); cType == DcxControlTypes::TEXT || cType == DcxControlTypes::EDIT)
			setBuddy(p_Control->getHwnd());
	}
	// xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if (flags[TEXT('r')])
	{
		static_assert(CheckFreeCommand(TEXT('r')), "Command in use!");

		if (numtok < 5)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto iMin = input.getnexttok().to_int();	// tok 4
		const auto iMax = input.getnexttok().to_int();	// tok 5

		setRange32(iMin, iMax);
	}
	// xdid -t [NAME] [ID] [SWITCH] [BASE]
	else if (flags[TEXT('t')])
	{
		static_assert(CheckFreeCommand(TEXT('t')), "Command in use!");

		if (numtok < 4)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nBase = input.getnexttok().to_int();	// tok 4

		setBase(nBase);
	}
	// xdid -v [NAME] [ID] [SWITCH] [POS]
	else if (flags[TEXT('v')])
	{
		static_assert(CheckFreeCommand(TEXT('v')), "Command in use!");

		if (numtok < 4)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

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

DcxControl* DcxUpDown::getBuddy() const noexcept
{
	if (auto hwnd = reinterpret_cast<HWND>(SendMessage(m_Hwnd, UDM_GETBUDDY, 0, 0)); hwnd)
	{
		if (auto pd = getParentDialog(); pd)
			return pd->getControlByHWND(hwnd);
	}
	return nullptr;
}

LRESULT DcxUpDown::setBuddy(const HWND mHwnd) noexcept
{
	return SendMessage(m_Hwnd, UDM_SETBUDDY, reinterpret_cast<WPARAM>(mHwnd), 0);
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
	int iMin{}, iMax{};
	SendMessage(m_Hwnd, UDM_GETRANGE32, reinterpret_cast<WPARAM>(&iMin), reinterpret_cast<LPARAM>(&iMax));
	return{ iMin,iMax };
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::setBase(const int iBase) noexcept
{
	return SendMessage(m_Hwnd, UDM_SETBASE, gsl::narrow_cast<WPARAM>(iBase), 0);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::getBase() const noexcept
{
	return SendMessage(m_Hwnd, UDM_GETBASE, 0U, 0);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::setPos32(const INT nPos) noexcept
{
	return SendMessage(m_Hwnd, UDM_SETPOS32, 0U, gsl::narrow_cast<LPARAM>(nPos));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::getPos32(const LPBOOL pfError) const noexcept
{
	return SendMessage(m_Hwnd, UDM_GETPOS32, 0U, reinterpret_cast<LPARAM>(pfError));
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

void DcxUpDown::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	auto pd = getParentDialog();
	if (!pd)
		return;

	if (auto ctrl = getBuddy(); ctrl)
		xml->SetAttribute("buddyid", pd->IDToName(ctrl->getID()).c_str());
}

TiXmlElement* DcxUpDown::toXml(void) const
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
LRESULT DcxUpDown::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
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
		case UDN_DELTAPOS:
		{
			if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("sclick,%u"), getUserID());
			bParsed = TRUE;
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

LRESULT DcxUpDown::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
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

LRESULT DcxUpDown::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
