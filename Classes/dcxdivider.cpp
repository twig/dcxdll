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
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WS_EX_CONTROLPARENT,
		DCX_DIVIDERCLASS,
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

	setNoThemed(ws.m_NoTheme);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDivider::~DcxDivider() noexcept
{
}

/*!
 * \brief blah
 *
 * blah
 */

const TString DcxDivider::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, DVS_VERT))
		styles.addtok(TEXT("vertical"));

	return styles;
}

dcxWindowStyles DcxDivider::parseControlStyles(const TString & tsStyles)
{
	dcxWindowStyles ws;

	ws.m_Styles |= DVS_HORZ;

	if (tsStyles.istok(TEXT("vertical")))
		ws.m_Styles |= DVS_VERT;

	return parseGeneralControlStyles(tsStyles, ws);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */
void DcxDivider::parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case TEXT("position"_hash):
	{
		auto iDivPos = 0;

		SendMessage(m_Hwnd, DV_GETDIVPOS, 0U, reinterpret_cast<LPARAM>(&iDivPos));
		_ts_snprintf(szReturnValue, TEXT("%d"), iDivPos);
	}
	break;

	case TEXT("isvertical"_hash):
		_ts_snprintf(szReturnValue, TEXT("%u"), (dcxGetWindowStyle(m_Hwnd) & DVS_VERT));
		break;

	default:
		this->parseGlobalInfoRequest(input, szReturnValue);
		break;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDivider::parseCommandRequest(const TString & input)
{
	const XSwitchFlags flags(input.getfirsttok(3));

	const auto numtok = input.numtok();

	// xdid -l|r [NAME] [ID] [SWITCH] [MIN] [IDEAL][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if (flags[TEXT('l')] || flags[TEXT('r')])
	{
		if (numtok < 10)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		DVPANEINFO dvpi;

		const auto data(input.getfirsttok(1, TSTABCHAR).trim());
		TString control_data;

		if (input.numtok(TSTABCHAR) > 1)
			control_data = input.getnexttok(TSTABCHAR).trim();	// tok 2

		dvpi.fMask = DVPIM_CHILD | DVPIM_MIN | DVPIM_IDEAL;
		dvpi.cxMin = data.getfirsttok(4).to_<UINT>();
		dvpi.cxIdeal = data.getnexttok().to_<UINT>();	// tok 5

		if (control_data.numtok() < 6)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		//const auto ID = mIRC_ID_OFFSET + control_data.gettok(1).to_<UINT>();
		const TString tsID(control_data.gettok(1));
		const auto ID = getParentDialog()->NameToID(tsID);

		if (getParentDialog()->isIDValid(ID) || ID)
			throw Dcx::dcxException(TEXT("Control with ID %(%) already exists"), tsID, ID - mIRC_ID_OFFSET);

		try {
			dvpi.hChild = getParentDialog()->addControl(control_data, 1, DcxAllowControls::ALLOW_ALLBUTDOCK, m_Hwnd)->getHwnd();

			if (flags[TEXT('l')])
				setPane(DVF_PANELEFT, &dvpi);
			else if (flags[TEXT('r')])
				setPane(DVF_PANERIGHT, &dvpi);

			redrawWindow();
		}
		catch (const std::exception &e)
		{
			showError(nullptr, TEXT("-c"), TEXT("Unable To Create Control %(%) (%)"), tsID, ID - mIRC_ID_OFFSET, e.what());
			throw;
		}
	}
	// xdid -v [NAME] [ID] [SWITCH] [POS]
	else if (flags[TEXT('v')])
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Insufficient parameters");
			throw DcxExceptions::dcxInvalidArguments();

		if (!setDivPos(input.getnexttok().to_<UINT>()))	// tok 4
			throw Dcx::dcxException("Divider position must be between bounds.");
	}
	else
		parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxDivider::setPane(const UINT iPaneId, const LPDVPANEINFO lpdvpi) noexcept
{
	return SendMessage(m_Hwnd, DV_SETPANE, gsl::narrow_cast<WPARAM>(iPaneId), reinterpret_cast<LPARAM>(lpdvpi));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxDivider::setDivPos(const UINT iDivPos) noexcept
{
	return SendMessage(m_Hwnd, DV_SETDIVPOS, 0U, gsl::narrow_cast<LPARAM>(iDivPos));
}

void DcxDivider::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());

	DVPANEINFO left;
	DVPANEINFO right;
	Divider_GetChildControl(m_Hwnd, DVF_PANELEFT, &left);
	Divider_GetChildControl(m_Hwnd, DVF_PANERIGHT, &right);
	if (left.hChild)
	{
		if (const auto *const dcxcleft = this->getParentDialog()->getControlByHWND(left.hChild); dcxcleft)
			xml->LinkEndChild(dcxcleft->toXml());
		else
			xml->LinkEndChild(new TiXmlElement("control"));
	}
	else
		xml->LinkEndChild(new TiXmlElement("control"));
	if (right.hChild)
	{
		if (const auto *const dcxcright = this->getParentDialog()->getControlByHWND(right.hChild); dcxcright)
			xml->LinkEndChild(dcxcright->toXml());
		else
			xml->LinkEndChild(new TiXmlElement("control"));
	}
	else
		xml->LinkEndChild(new TiXmlElement("control"));
}

TiXmlElement * DcxDivider::toXml(void) const
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
LRESULT DcxDivider::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) noexcept
{
	return 0L;
}

LRESULT DcxDivider::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed)
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
			if (const auto c_this = Dcx::dcxGetProp<DcxControl *>(hdr->hwndFrom, TEXT("dcx_cthis")); c_this)
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
		if (auto cHwnd = GetDlgItem(m_Hwnd, gsl::narrow_cast<int>(wParam)); IsWindow(cHwnd))
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

	case WM_DESTROY:
	{
		delete this;
		bParsed = TRUE;
	}
	break;

	case DV_CHANGEPOS:
	{
		const auto phase = gsl::narrow_cast<int>(wParam);
		const auto pt = reinterpret_cast<LPPOINT>(lParam);

		constexpr TCHAR szdrag_begin[] = TEXT("dragbegin");
		constexpr TCHAR szdrag[] = TEXT("drag");
		constexpr TCHAR szdrag_finish[] = TEXT("dragfinish");
		this->execAliasEx(TEXT("%s,%u,%d,%d"), (phase == DVNM_DRAG_START ? &szdrag_begin[0] : (phase == DVNM_DRAG_END ? &szdrag_finish[0] : &szdrag[0])), this->getUserID(), pt->x, pt->y);
	}
	break;

	default:
		lRes = this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}

LRESULT DcxDivider::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
