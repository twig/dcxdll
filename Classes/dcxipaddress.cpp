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
 * � ScriptsDB.org - 2006
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

DcxIpAddress::DcxIpAddress(const UINT ID, DcxDialog* const p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_IPADDRESSCLASS,
		ws.m_Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsWindow(m_Hwnd))
		//throw Dcx::dcxException("Unable To Create Window");
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);

	if (styles.istok(TEXT("tooltips")))
	{
		if (!IsWindow(p_Dialog->getToolTip()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		setToolTipHWND(p_Dialog->getToolTip());
		AddToolTipToolInfo(getToolTipHWND(), m_Hwnd);
	}

	// fix bug with disabled creation
	// todo: fix this properly
	if (dcx_testflag(ws.m_Styles, WS_DISABLED))
	{
		EnableWindow(m_Hwnd, TRUE);
		EnableWindow(m_Hwnd, FALSE);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

DcxIpAddress::~DcxIpAddress() noexcept
{
}

void DcxIpAddress::toXml(TiXmlElement* const xml) const
{
	//char buf[128]{};
	//this->AddressToString(&buf[0], Dcx::countof(buf));
	//__super::toXml(xml);
	//xml->SetAttribute("caption", &buf[0]);

	const TString buf(AddressToString<TString>());

	__super::toXml(xml);

	xml->SetAttribute("caption", buf.c_str());
}

TiXmlElement* DcxIpAddress::toXml(void) const
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

 //void DcxIpAddress::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
 //{
 //	this->parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
 //}

dcxWindowStyles DcxIpAddress::parseControlStyles(const TString& tsStyles)
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

void DcxIpAddress::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	// [NAME] [ID] [PROP]
	if (input.gettok(3) == TEXT("ip"))
	{
		//this->AddressToString(szReturnValue.data(), szReturnValue.size());
		szReturnValue = AddressToString<TString>();
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxIpAddress::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));

	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		this->clearAddress();

	// xdid -a [NAME] [ID] [SWITCH] IP.IP.IP.IP
	if (flags[TEXT('a')])
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto IP(input.getnexttok().strip());	// tok 4

		if (IP.numtok(TEXT('.')) != 4)
			throw Dcx::dcxException(TEXT("Invalid Address: %"), IP);

		this->setAddress(IP.to_addr());
	}
	// xdid -g [NAME] [ID] [SWITCH] [N] [MIN] [MAX]
	else if (flags[TEXT('g')])
	{
		if (numtok < 6)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nField = input.getnexttok().to_int() - 1;				// tok 4
		const auto min = gsl::narrow_cast<BYTE>(input.getnexttok().to_int() & 0xFF);	// tok 5
		const auto max = gsl::narrow_cast<BYTE>(input.getnexttok().to_int() & 0xFF);	// tok 6

		if (nField < 0 || nField > 3)
			//throw Dcx::dcxException("Out of Range");
			throw DcxExceptions::dcxOutOfRange();

		this->setRange(nField, min, max);
	}
	// xdid -j [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('j')])
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nField = input.getnexttok().to_int() - 1;	// tok 4

		if (nField < 0 || nField > 3)
			//throw Dcx::dcxException("Out of Range");
			throw DcxExceptions::dcxOutOfRange();

		this->setFocus(nField);
	}
	// This is to avoid invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
	{
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

LRESULT DcxIpAddress::setRange(const int nField, const BYTE iMin, const BYTE iMax) noexcept
{
	return SendMessage(m_Hwnd, IPM_SETRANGE, gsl::narrow_cast<WPARAM>(nField), gsl::narrow_cast<LPARAM>(MAKEIPRANGE(iMin, iMax)));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::setFocus(const int nField) noexcept
{
	return SendMessage(m_Hwnd, IPM_SETFOCUS, gsl::narrow_cast<WPARAM>(nField), 0);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::setAddress(const DWORD dwIpAddress) noexcept
{
	return SendMessage(m_Hwnd, IPM_SETADDRESS, 0U, gsl::narrow_cast<LPARAM>(dwIpAddress));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::getAddress(const LPDWORD lpdwIpAddress) const noexcept
{
	return SendMessage(m_Hwnd, IPM_GETADDRESS, 0U, reinterpret_cast<LPARAM>(lpdwIpAddress));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::clearAddress() noexcept
{
	return SendMessage(m_Hwnd, IPM_CLEARADDRESS, 0U, 0);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxIpAddress::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (!hdr)
			break;
#pragma warning(push)
#pragma warning(disable: 26454)	//: warning C26454 : Arithmetic overflow : '-' operation produces a negative unsigned result at compile time(io.5).

		if (hdr->code == IPN_FIELDCHANGED)
		{
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
				this->execAliasEx(TEXT("edit,%u"), getUserID());
			bParsed = TRUE;
		}
#pragma warning(pop)
	}
	break;
	default:
		break;
	}
	return 0L;
}

LRESULT DcxIpAddress::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_CONTEXTMENU:
	case WM_SETCURSOR:
		break;

	case WM_MOUSEACTIVATE:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
		{
			switch (Dcx::dcxHIWORD(lParam))
			{
			case WM_LBUTTONUP:
			{
				execAliasEx(TEXT("sclick,%u"), getUserID());
			}
			break;
			case WM_RBUTTONUP:
			{
				execAliasEx(TEXT("rclick,%u"), getUserID());
			}
			break;
			default:
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

LRESULT DcxIpAddress::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
