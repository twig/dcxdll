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
DcxDivider::DcxDivider(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
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

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		DcxUXModule::dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);
}

DcxDivider::~DcxDivider() noexcept
{
}

const TString DcxDivider::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, DVS_VERT))
		styles.addtok(TEXT("vertical"));

	return styles;
}

dcxWindowStyles DcxDivider::parseControlStyles(const TString& tsStyles)
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
void DcxDivider::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	if (!m_Hwnd)
		return;

	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case TEXT("position"_hash):
	{
		_ts_snprintf(szReturnValue, TEXT("%u"), getDivPos());
	}
	break;

	case TEXT("isvertical"_hash):
		_ts_snprintf(szReturnValue, TEXT("%u"), (dcxGetWindowStyle(m_Hwnd) & DVS_VERT));
		break;

	case TEXT("barcolours"_hash):
	case TEXT("barcolors"_hash):
	{
		const auto dbc = getBarColours();
		_ts_snprintf(szReturnValue, TEXT("%u %u %u %u"), dbc.clrBar, dbc.clrSelBarFg, dbc.clrSelBarBg, dbc.clrBarHover);
	}
	break;

	case TEXT("barwidth"_hash):
	{
		_ts_snprintf(szReturnValue, TEXT("%u"), getBarWidth());
	}
	break;

	case TEXT("barstate"_hash):
	{
		const auto mask = SendMessage(m_Hwnd, DV_GETBARSTATE, 0, 0);
		if (dcx_testflag(mask, DVBS_DRAGGING))
			szReturnValue += TEXT("dragging ");
		if (dcx_testflag(mask, DVBS_LOCKED))
			szReturnValue += TEXT("locked ");
	}
	break;

	default:
		this->parseGlobalInfoRequest(input, szReturnValue);
		break;
	}
}

void DcxDivider::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));

	const auto numtok = input.numtok();

	// xdid -l|r [NAME] [ID] [SWITCH] [MIN] [IDEAL][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if (flags[TEXT('l')] || flags[TEXT('r')])
	{
		if (numtok < 10)
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
			throw DcxExceptions::dcxInvalidArguments();

		//const auto ID = mIRC_ID_OFFSET + control_data.gettok(1).to_<UINT>();
		const TString tsID(control_data.gettok(1));
		const auto pParent = getParentDialog();
		if (!pParent)
			throw Dcx::dcxException(TEXT("Unable to get controls parent dialog"));

		const auto ID = pParent->NameToID(tsID);

		if (pParent->isIDValid(ID) || ID)
			throw Dcx::dcxException(TEXT("Control with ID %(%) already exists"), tsID, ID - mIRC_ID_OFFSET);

		try {
			dvpi.hChild = pParent->addControl(control_data, 1, DcxAllowControls::ALLOW_ALLBUTDOCK, m_Hwnd)->getHwnd();

			if (flags[TEXT('l')])
				setPane(DVF_PANELEFT, &dvpi);
			else if (flags[TEXT('r')])
				setPane(DVF_PANERIGHT, &dvpi);

			redrawWindow();
		}
		catch (const std::exception& e)
		{
			showError(nullptr, TEXT("-l|r"), TEXT("Unable To Create Control %(%) (%)"), tsID, ID - mIRC_ID_OFFSET, e.what());
			throw;
		}
	}
	// xdid -v [NAME] [ID] [SWITCH] [POS]
	else if (flags[TEXT('v')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		if (!setDivPos(input.getnexttok().to_<UINT>()))	// tok 4
			throw Dcx::dcxException("Divider position must be between bounds.");
	}
	// xdid -W [NAME] [ID] [SWITCH] [WIDTH]
	else if (flags[TEXT('W')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		if (!setBarWidth(input.getnexttok().to_<UINT>()))	// tok 4
			throw DcxExceptions::dcxInvalidArguments();
	}
	// xdid -Q [NAME] [ID] [SWITCH] [COLOUR] [SELECTED COLOUR] [SELECTED BKG COLOUR] [HOVER COLOUR]
	else if (flags[TEXT('Q')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		DVBARCOLORS dbc;
		dbc.clrBar = input.getnexttok().to_<COLORREF>();
		if (numtok > 4)
			dbc.clrSelBarFg = input.getnexttok().to_<COLORREF>();
		if (numtok > 5)
			dbc.clrSelBarBg = input.getnexttok().to_<COLORREF>();
		if (numtok > 6)
			dbc.clrBarHover = input.getnexttok().to_<COLORREF>();

		if (!setBarColor(dbc))
			throw DcxExceptions::dcxInvalidArguments();
	}
	// xdid -L [NAME] [ID] [SWITCH] [LOCK STATE]
	else if (flags[TEXT('L')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const BOOL bState = ((input.getnexttok().to_<int>() > 0) ? TRUE : FALSE);

		SendMessage(m_Hwnd, DV_LOCKBAR, bState, 0L);
	}
	else
		parseGlobalCommandRequest(input, flags);
}

LRESULT DcxDivider::setPane(const UINT iPaneId, const LPDVPANEINFO lpdvpi) noexcept
{
	return SendMessage(m_Hwnd, DV_SETPANE, gsl::narrow_cast<WPARAM>(iPaneId), reinterpret_cast<LPARAM>(lpdvpi));
}

LRESULT DcxDivider::setDivPos(const UINT iDivPos) noexcept
{
	return SendMessage(m_Hwnd, DV_SETDIVPOS, 0U, gsl::narrow_cast<LPARAM>(iDivPos));
}

BOOL DcxDivider::setBarColor(COLORREF clrUnselected, COLORREF clrSelected) noexcept
{
	DVBARCOLORS dbc;
	dbc.clrBar = clrUnselected;
	dbc.clrSelBarFg = clrSelected;
	return setBarColor(dbc);
}

BOOL DcxDivider::setBarColor(const DVBARCOLORS& dbc) noexcept
{
	return gsl::narrow_cast<BOOL>(SendMessage(m_Hwnd, DV_SETBARCOLOR, 0, reinterpret_cast<LPARAM>(&dbc)));
}

BOOL DcxDivider::setBarWidth(UINT nWidth) noexcept
{
	return gsl::narrow_cast<BOOL>(SendMessage(m_Hwnd, DV_SETBARWIDTH, 0U, gsl::narrow_cast<LPARAM>(nWidth)));
}

UINT DcxDivider::getDivPos() const noexcept
{
	BOOL bVert{};
	UINT iPos{};
	if (m_Hwnd)
		SendMessage(m_Hwnd, DV_GETDIVPOS, reinterpret_cast<WPARAM>(&bVert), reinterpret_cast<LPARAM>(&iPos));
	return iPos;
}

UINT DcxDivider::getBarWidth() const noexcept
{
	UINT iPos{};
	if (m_Hwnd)
		SendMessage(m_Hwnd, DV_GETBARWIDTH, 0U, reinterpret_cast<LPARAM>(&iPos));
	return iPos;
}

DVBARCOLORS DcxDivider::getBarColours() const noexcept
{
	//COLORREF clr{ CLR_INVALID };
	//COLORREF selclr{ CLR_INVALID };
	//if (m_Hwnd)
	//	SendMessage(m_Hwnd, DV_GETBARCOLOR, reinterpret_cast<WPARAM>(&clr), reinterpret_cast<LPARAM>(&selclr));
	//return { clr, selclr };

	DVBARCOLORS dbc;
	if (m_Hwnd)
		SendMessage(m_Hwnd, DV_GETBARCOLOR, 0, reinterpret_cast<LPARAM>(&dbc));
	return dbc;
}

void DcxDivider::toXml(TiXmlElement* const xml) const
{
	if (!xml)
		return;

	const auto pParent = this->getParentDialog();
	if (!pParent)
		return;

	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());

	xml->SetAttribute("pos", getDivPos());
	xml->SetAttribute("barwidth", getBarWidth());
	{
		const auto dbc = getBarColours();
		setColourAttribute(xml, "barcolour", dbc.clrBar);
		setColourAttribute(xml, "barselectedcolour", dbc.clrSelBarFg);
		setColourAttribute(xml, "barselectedbgcolour", dbc.clrSelBarBg);
		setColourAttribute(xml, "barhovercolour", dbc.clrBarHover);
	}

	{
		// left of divider
		DVPANEINFO left;
		Divider_GetChildControl(m_Hwnd, DVF_PANELEFT, &left);

		TiXmlElement xLeft("item");
		xLeft.SetAttribute("min", left.cxMin);
		xLeft.SetAttribute("ideal", left.cxIdeal);
		xLeft.SetAttribute("side", "left");

		if (left.hChild)
		{
			if (const auto* const dcxcleft = pParent->getControlByHWND(left.hChild); dcxcleft)
				xLeft.LinkEndChild(dcxcleft->toXml());
		}

		xml->InsertEndChild(xLeft);
	}
	{
		// right of divider
		DVPANEINFO right;
		Divider_GetChildControl(m_Hwnd, DVF_PANERIGHT, &right);

		TiXmlElement xRight("item");
		xRight.SetAttribute("min", right.cxMin);
		xRight.SetAttribute("ideal", right.cxIdeal);
		xRight.SetAttribute("side", "right");

		if (right.hChild)
		{
			if (const auto* const dcxcright = pParent->getControlByHWND(right.hChild); dcxcright)
				xRight.LinkEndChild(dcxcright->toXml());
		}

		xml->InsertEndChild(xRight);
	}
}

TiXmlElement* DcxDivider::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxDivider::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	const auto pd = this->getParentDialog();
	if (!pd)
		return;

	__super::fromXml(xDcxml, xThis);

	if (const auto iPos = queryIntAttribute(xThis, "pos", 100); !setDivPos(iPos))
		throw Dcx::dcxException("Divider position must be between bounds.");

	setBarWidth(queryIntAttribute(xThis, "barwidth", 2));

	//setBarColor(queryColourAttribute(xThis, "barcolour"), queryColourAttribute(xThis, "barselectedcolour"));

	{
		DVBARCOLORS dbc;
		dbc.clrBar = queryColourAttribute(xThis, "barcolour");
		dbc.clrSelBarFg = queryColourAttribute(xThis, "barselectedcolour");
		dbc.clrSelBarBg = queryColourAttribute(xThis, "barselectedbgcolour");
		dbc.clrBarHover = queryColourAttribute(xThis, "barhovercolour");
		setBarColor(dbc);
	}

	for (auto xItem = xThis->FirstChildElement("item"); xItem; xItem = xItem->NextSiblingElement())
	{
		const auto szSide = queryAttribute(xItem, "side");
		if (_ts_isEmpty(szSide)) // no side, invalid item
			throw DcxExceptions::dcxInvalidItem();

		DVPANEINFO dvpi;
		dvpi.cbSize = sizeof(DVPANEINFO);
		dvpi.fMask = DVPIM_MIN | DVPIM_IDEAL;
		dvpi.cxIdeal = queryIntAttribute(xItem, "ideal");
		dvpi.cxMin = queryIntAttribute(xItem, "min");

		if (auto xCtrl = xItem->FirstChildElement("control"); xCtrl)
		{
			// control.
			dvpi.fMask |= DVPIM_CHILD;

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
			if (auto ctrl = pd->addControl(tsInput, 1, DcxAllowControls::ALLOW_ALLBUTDOCK, m_Hwnd); ctrl)
			{
				ctrl->fromXml(xThis, xCtrl);

				dvpi.hChild = ctrl->getHwnd();

				if (_ts_strcmp(szSide, "left") == 0)
					setPane(DVF_PANELEFT, &dvpi);
				else
					setPane(DVF_PANERIGHT, &dvpi);
			}

		}
	}
}

LRESULT DcxDivider::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept
{
	return 0L;
}

LRESULT DcxDivider::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
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
		this->CallDefaultClassProc(uMsg, wParam, lParam);

		delete this;
		bParsed = TRUE;
	}
	break;

	case DV_CHANGEPOS:
	{
		if (dcx_testflag(getEventMask(), DCX_EVENT_DRAG))
		{
			dcxlParam(LPPOINT, pt);

			const auto phase = gsl::narrow_cast<int>(wParam);

			constexpr TCHAR szdrag_begin[] = TEXT("dragbegin");
			constexpr TCHAR szdrag[] = TEXT("drag");
			constexpr TCHAR szdrag_finish[] = TEXT("dragfinish");
			if (!this->execAliasEx(TEXT("%s,%u,%d,%d"), (phase == DVNM_DRAG_START ? &szdrag_begin[0] : (phase == DVNM_DRAG_END ? &szdrag_finish[0] : &szdrag[0])), this->getUserID(), pt->x, pt->y))
			{
				bParsed = TRUE;
				return TRUE;
			}
		}
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
