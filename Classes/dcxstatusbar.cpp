/*!
 * \file dcxstatusbar.cpp
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
#include "Classes/dcxstatusbar.h"
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
DcxStatusBar::DcxStatusBar(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog, DcxControlTypes::STATUSBAR)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WS_EX_CONTROLPARENT,
		DCX_STATUSBARCLASS,
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

	//ZeroMemory(&m_iDynamicParts[0], sizeof(m_iDynamicParts));
	//ZeroMemory(&m_iFixedParts[0], sizeof(m_iFixedParts));

	if (const auto h = rc->bottom - rc->top; h > 0)
		Dcx::dcxStatusBar_SetMinHeight(m_Hwnd, h);

	setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxStatusBar::~DcxStatusBar() noexcept
{
	for (const auto pPart : m_vParts)
		delete pPart;

	cleanPartIcons();
	ImageList_Destroy(getImageList());
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxStatusBar::parseControlStyles(const TString& tsStyles)
{
	auto ws = parseGeneralControlStyles(tsStyles);

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"grip"_hash:
			ws.m_Styles |= SBARS_SIZEGRIP;
			break;
		case L"tooltips"_hash:
			ws.m_Styles |= SBARS_TOOLTIPS;
			break;
		case L"nodivider"_hash:
			ws.m_Styles |= CCS_NODIVIDER;
			break;
		case L"top"_hash:
		{
			ws.m_Styles |= CCS_TOP;
			ws.m_Styles &= ~SBARS_SIZEGRIP; // size grip doesn't work for left or top styles.
		}
		break;
		case L"noresize"_hash:
			ws.m_Styles |= CCS_NORESIZE;
			break;
		case L"noparentalign"_hash:
			ws.m_Styles |= CCS_NOPARENTALIGN;
			break;
		case L"noauto"_hash:
			ws.m_Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
			break;
			//case L"left"_hash:
			//{ // NB: left & right styles don't render the parts vertically.
			//	Styles |= CCS_LEFT;
			//	Styles &= ~SBARS_SIZEGRIP;
			//}
			//break;
			//case L"right"_hash:
			//	Styles |= CCS_RIGHT;
			//	break;
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

void DcxStatusBar::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	const auto numtok = input.numtok();

	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP] [N]
	case L"text"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const int iPart = input.getnexttokas<int>() - 1, nParts = getParts(DCX_STATUSBAR_MAX_PARTS, nullptr);	// tok 4

		if (iPart < 0 && iPart >= nParts)
			throw Dcx::dcxException("Invalid Part");

		if (const auto iFlags = getPartFlags(iPart); dcx_testflag(iFlags, SBT_OWNERDRAW))
		{
			if (auto pPart = reinterpret_cast<LPSB_PARTINFOX>(getText(iPart, nullptr)); pPart)
				szReturnValue = pPart->m_xText.to_chr();
		}
		else {
			const auto len = this->getTextLength(iPart);
			auto text = std::make_unique<WCHAR[]>(len + 1U);
			getText(iPart, text.get());
			szReturnValue = text.get();
		}
	}
	break;
	// [NAME] [ID] [PROP]
	case L"parts"_hash:
	{
		auto parts = std::make_unique<INT[]>(DCX_STATUSBAR_MAX_PARTS);

		const auto nParts = this->getParts(DCX_STATUSBAR_MAX_PARTS, nullptr);

		getParts(DCX_STATUSBAR_MAX_PARTS, parts.get());

		TString tmp(gsl::narrow_cast<TString::size_type>(32U * nParts));

		for (auto i = decltype(nParts){0}; i < nParts; ++i)
			tmp.addtok(gsl::at(parts, i));

		szReturnValue = tmp.to_chr();
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"tooltip"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const int iPart = input.getnexttokas<int>(), nParts = getParts(DCX_STATUSBAR_MAX_PARTS, nullptr);	// tok 4

		if (iPart < 0 && iPart >= nParts)
			throw Dcx::dcxException("Invalid Part");

		getTipText(iPart, MIRC_BUFFER_SIZE_CCH, szReturnValue);
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
}

void DcxStatusBar::deletePartInfo(const int iPart) noexcept
{
	if (!dcx_testflag(Dcx::dcxHIWORD(getTextLength(iPart)), SBT_OWNERDRAW))
		return;

	auto pPart = reinterpret_cast<LPSB_PARTINFOX>(getText(iPart, nullptr));

	if (!pPart)
		return;

	//for (auto itStart = m_vParts.begin(), itEnd = m_vParts.end(); itStart != itEnd; ++itStart)
	//{
	//	if (*itStart == pPart) {
	//		this->setText( iPart, SBT_OWNERDRAW, nullptr);
	//		delete pPart->m_xChild;
	//		delete pPart;
	//		m_vParts.erase(itStart);
	//		return;
	//	}
	//}

	if (Dcx::eraseIfFound(m_vParts, pPart))
	{
		setText(iPart, SBT_OWNERDRAW, nullptr);
		delete pPart->m_xChild;
		delete pPart;
	}
}

void DcxStatusBar::loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc)
{
	auto filename(tsSrc);

	auto himl = getImageList();

	if (!himl)
	{
		himl = createImageList();

		if (himl)
			setImageList(himl);
	}

	if (!himl)
		throw Dcx::dcxException("Unable to get imagelist");

	Dcx::dcxLoadIconRange(himl, filename, false, tsFlags, tsIndex);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));

	const auto numtok = input.numtok();

	// xdid -k [NAME] [ID] [SWITCH] [COLOR]
	if (flags[TEXT('k')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto col = input.getnexttokas<COLORREF>();	// tok 4
		if (col == CLR_INVALID)
			this->setBkColor(CLR_DEFAULT);
		else
			this->setBkColor(col);
	}
	// xdid -l -> [NAME] [ID] -l [POS [POS POS ...]]
	else if (flags[TEXT('l')])
	{
		//if (numtok < 4)
		//	throw DcxExceptions::dcxInvalidArguments();
		//
		//const auto nParts = numtok - 3;
		//auto parts = std::make_unique<INT[]>(DCX_STATUSBAR_MAX_PARTS);
		//
		//auto c = 0U;
		//TString p;
		//for (auto i = decltype(nParts){0}; i < nParts; ++i)
		//{
		//	if (c >= 100)
		//		throw Dcx::dcxException("Can't Allocate Over 100% of Statusbar!");
		//
		//	p = input.getnexttok();	// tok i+4
		//	const auto t = p.to_int();
		//
		//	if (p.right(1) == TEXT('%'))
		//	{
		//		gsl::at(m_iDynamicParts, i) = t;
		//		c += t;
		//	}
		//	else
		//		gsl::at(m_iFixedParts, i) = t;
		//
		//	gsl::at(parts, i) = t;
		//}
		//setParts(nParts, parts.get());
		//updateParts();

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		setPartsPositions(input.getlasttoks());
	}
	// xdid -t [NAME] [ID] [SWITCH] N [+FLAGS] [#ICON] [Cell Text][TAB]Tooltip Text
	// xdid -t [NAME] [ID] [SWITCH] N [+c] [#ICON] [CID] [CTRL] [X] [Y] [W] [H] (OPTIONS)
	// xdid -t [NAME] [ID] [SWITCH] N [+f] [#ICON] (TEXT)[TAB]Tooltip Text
	else if (flags[TEXT('t')])
	{
		//if (numtok < 6)
		//	throw DcxExceptions::dcxInvalidArguments();
		//
		//const auto nPos = input.getnexttokas<int>() - 1;	// tok 4
		//const auto flag(input.getnexttok());			// tok 5
		//const auto icon = input.getnexttokas<int>() - 1;	// tok 6
		//const auto tsTabOne(input.getfirsttok(1, TSTABCHAR).trim());
		//const auto tsTabTwo(input.getlasttoks().trim());
		//
		//if (nPos < 0 || nPos >= this->getParts(DCX_STATUSBAR_MAX_PARTS, nullptr))
		//	throw Dcx::dcxException("Invalid Part");
		//
		//TString itemtext;
		//TString tooltip;
		//
		//deletePartInfo(nPos); // delete custom info if any.
		//DestroyIcon(getIcon(nPos)); // delete any icon for this part.
		//setIcon(nPos, nullptr); // set as no icon.
		//
		//const auto iFlags = parseItemFlags(flag);
		//
		//if (tsTabOne.numtok() > 6)
		//	itemtext = tsTabOne.gettok(7, -1).trim();
		//
		//if (!tsTabTwo.empty())
		//	tooltip = tsTabTwo;
		//
		//if (dcx_testflag(iFlags, SBT_OWNERDRAW))
		//{
		//	auto pPart = std::make_unique<SB_PARTINFOX>();
		//
		//	pPart->m_xChild = nullptr;
		//	pPart->m_xiIcon = icon;
		//	if (flag.find(TEXT('f'), 0))
		//	{ // mIRC formatted text
		//		pPart->m_xText = itemtext;
		//		setTipText(nPos, tooltip.to_chr());
		//	}
		//	else { // child control
		//		// this is split like this to avoid a compiler bug that caused the compiler to use >30GB ram & still fail.
		//		constexpr auto eAllowedControls1 = DcxAllowControls::ALLOW_PBAR | DcxAllowControls::ALLOW_TRACKBAR | DcxAllowControls::ALLOW_COMBOEX | DcxAllowControls::ALLOW_STATUSBAR | DcxAllowControls::ALLOW_TOOLBAR | DcxAllowControls::ALLOW_MULTICOMBO;
		//		constexpr auto eAllowedControls2 = DcxAllowControls::ALLOW_TREEVIEW | DcxAllowControls::ALLOW_LISTVIEW | DcxAllowControls::ALLOW_REBAR | DcxAllowControls::ALLOW_BUTTON | DcxAllowControls::ALLOW_EDIT;
		//		constexpr auto eAllowedControls3 = DcxAllowControls::ALLOW_UPDOWN | DcxAllowControls::ALLOW_IPADDRESS | DcxAllowControls::ALLOW_WEBCTRL | DcxAllowControls::ALLOW_CALANDER | DcxAllowControls::ALLOW_DIVIDER | DcxAllowControls::ALLOW_PANEL;
		//		constexpr auto eAllowedControls4 = DcxAllowControls::ALLOW_TAB | DcxAllowControls::ALLOW_LINE | DcxAllowControls::ALLOW_BOX | DcxAllowControls::ALLOW_RADIO | DcxAllowControls::ALLOW_CHECK | DcxAllowControls::ALLOW_TEXT | DcxAllowControls::ALLOW_SCROLL | DcxAllowControls::ALLOW_LIST;
		//		constexpr auto eAllowedControls5 = DcxAllowControls::ALLOW_LINK | DcxAllowControls::ALLOW_IMAGE | DcxAllowControls::ALLOW_PAGER | DcxAllowControls::ALLOW_DATETIME | DcxAllowControls::ALLOW_STACKER | DcxAllowControls::ALLOW_DIRECTSHOW;
		//		constexpr auto eAllowedControls = eAllowedControls1 | eAllowedControls2 | eAllowedControls3 | eAllowedControls4 | eAllowedControls5;
		//
		//		auto p_Control = getParentDialog()->addControl(itemtext, 1,
		//			eAllowedControls,
		//			m_Hwnd);
		//
		//		/*DcxAllowControls::ALLOW_COLORCOMBO|*/
		//		/*DcxAllowControls::ALLOW_RICHEDIT|*/
		//
		//		pPart->m_xChild = p_Control;
		//		ShowWindow(p_Control->getHwnd(), SW_HIDE); // hide control untill a WM_DRAWITEM is recieved.
		//		SendMessage(m_Hwnd, WM_SIZE, 0, 0);
		//	}
		//	setPartInfo(nPos, iFlags, pPart.release());
		//}
		//else {
		//
		//	if (icon != -1)
		//		setIcon(nPos, ImageList_GetIcon(getImageList(), icon, ILD_TRANSPARENT));
		//
		//	setText(nPos, iFlags, itemtext.to_chr());
		//	setTipText(nPos, tooltip.to_chr());
		//}


		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nPos = input.getnexttokas<int>() - 1;	// tok 4
		const auto tsFlags(input.getnexttok());			// tok 5
		const auto iIcon = input.getnexttokas<int>() - 1;	// tok 6
		const auto tsTabOne(input.getfirsttok(1, TSTABCHAR).trim());
		const auto tsTooltip(input.getlasttoks().trim());

		TString tsText;

		if (tsTabOne.numtok() > 6)
			tsText = tsTabOne.gettok(7, -1).trim();

		setPartContents(nPos, tsFlags, iIcon, tsText, tsTooltip);
	}
	// xdid -v [NAME] [ID] [SWITCH] [N] (TEXT)
	else if (flags[TEXT('v')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nPos = input.getnexttokas<int>() - 1;	// tok 4

		if (nPos < 0 || nPos >= this->getParts(DCX_STATUSBAR_MAX_PARTS, nullptr))
			throw Dcx::dcxException("Invalid Part");

		TString itemtext;
		if (numtok > 4)
			itemtext = input.getlasttoks();	// tok 5, -1

		if (dcx_testflag(Dcx::dcxHIWORD(getTextLength(nPos)), SBT_OWNERDRAW))
		{
			if (auto pPart = reinterpret_cast<LPSB_PARTINFOX>(getText(nPos, nullptr)); pPart)
			{
				pPart->m_xText = itemtext;
				setPartInfo(nPos, SBT_OWNERDRAW, pPart);
			}
		}
		else {
			auto text = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);
			setText(nPos, Dcx::dcxHIWORD(getText(nPos, text.get())), itemtext.to_chr());
		}
	}
	// xdid -w [NAME] [ID] [SWITCH] [FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')])
	{
		//		if (numtok < 6)
		//			throw DcxExceptions::dcxInvalidArguments();
		//
		//		const auto flag(input.getnexttok());			// tok 4
		//		const auto index = input.getnexttokas<int>();		// tok 5
		//		auto filename(input.getlasttoks());				// tok 6, -1
		//
		//		auto himl = getImageList();
		//
		//		if (!himl)
		//		{
		//			himl = createImageList();
		//
		//			if (himl)
		//				setImageList(himl);
		//		}
		//
		//		if (!himl)
		//			throw Dcx::dcxException("Unable to get imagelist");
		//
		//#if DCX_USE_WRAPPERS
		//		const Dcx::dcxIconResource icon(index, filename, false, flag);
		//
		//		ImageList_AddIcon(himl, icon.get());
		//#else
		//		HICON icon = dcxLoadIcon(index, filename, false, flag);
		//
		//		if (!icon)
		//			throw Dcx::dcxException("Unable to load icon");
		//
		//		ImageList_AddIcon(himl, icon);
		//		DestroyIcon(icon);
		//#endif

		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto flag(input.getnexttok());	// tok 4
		const auto tsIndex(input.getnexttok());	// tok 5
		auto filename(input.getlasttoks());		// tok 6, -1

		this->loadIcon(flag, tsIndex, filename);
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if (flags[TEXT('y')])
	{
		ImageList_Destroy(getImageList());
		setImageList(nullptr);
	}
	else
		parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxStatusBar::getImageList() const noexcept
{
	return m_hImageList;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::setImageList(const HIMAGELIST himl) noexcept
{
	m_hImageList = himl;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxStatusBar::parseItemFlags(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('n')])
		iFlags |= SBT_NOBORDERS;
	if (xflags[TEXT('p')])
		iFlags |= SBT_POPOUT;
	if (xflags[TEXT('c')])
		iFlags |= SBT_OWNERDRAW;
	if (xflags[TEXT('f')])
		iFlags |= SBT_OWNERDRAW;

	return iFlags;
}

TString DcxStatusBar::getItemFlags(int iPos) const noexcept
{
	const UINT nFlags = this->getPartFlags(iPos);

	TString tsFlags(L"+");

	if (dcx_testflag(nFlags, SBT_OWNERDRAW))
	{
		if (const auto pInfo = this->getPartInfo(iPos); pInfo)
		{
			if (pInfo->m_xChild)
				tsFlags += L'c';
			else
				tsFlags += L'f';
		}
	}
	if (dcx_testflag(nFlags, SBT_NOBORDERS))
		tsFlags += L'n';
	if (dcx_testflag(nFlags, SBT_POPOUT))
		tsFlags += L'p';

	return tsFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::cleanPartIcons() noexcept
{
	for (auto n = 0; n < DCX_STATUSBAR_MAX_PARTS; ++n)
	{
		if (auto hIcon = getIcon(n); hIcon)
			DestroyIcon(hIcon);
	}
}

void DcxStatusBar::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());
	if (const auto tmp = getBkColor(); tmp != CLR_INVALID)
		setColourAttribute(xml, "bgcolour", tmp);

	// icons...
	if (auto himl = this->getImageList(); himl)
		xmlSaveImageList(himl, xml, L"+B"_ts);

	const auto nParts = getParts(DCX_STATUSBAR_MAX_PARTS, nullptr);

	{
		TString tsCells;
		for (auto n = decltype(nParts){0}; n < nParts; ++n)
		{
			// add item to cell list
			if (m_iDynamicParts[n])
			{
				// perc #%
				tsCells.addtok(m_iDynamicParts[n]);
				tsCells += L'%';
			}
			else if (m_iFixedParts[n])
			{
				// fixed #
				tsCells.addtok(m_iFixedParts[n]);
			}

			// save item
			TiXmlElement xItem("item");

			xItem.SetAttribute("index", n);

			if (const TString tsFlags(this->getItemFlags(n)); !tsFlags.empty())
				xItem.SetAttribute("flags", tsFlags.c_str());

			if (const auto pInfo = this->getPartInfo(n); pInfo)
			{
				if (pInfo->m_xiIcon >= 0)
					xItem.SetAttribute("icon", pInfo->m_xiIcon + 1);
				if (!pInfo->m_xText.empty())
					xItem.SetAttribute("text", pInfo->m_xText.c_str());
				if (pInfo->m_xChild)
				{
					// save control here
					xItem.LinkEndChild(pInfo->m_xChild->toXml());
				}
			}
			else {
				const TString tsText(this->getText(n));
				if (!tsText.empty())
					xItem.SetAttribute("text", tsText.c_str());
			}
			{
				const TString tsTooltip(this->getTipText(n));
				if (!tsTooltip.empty())
					xItem.SetAttribute("tooltip", tsTooltip.c_str());
			}

			xml->InsertEndChild(xItem);
		}
		if (!tsCells.empty())
			xml->SetAttribute("cells", tsCells.c_str());
	}
}

TiXmlElement* DcxStatusBar::toXml() const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxStatusBar::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	if (const auto tmp = queryColourAttribute(xThis, "bgcolour"); tmp != CLR_INVALID)
		setBkColor(tmp);

	// setup positions
	{
		const TString tsPositions(queryAttribute(xThis, "cells"));
		setPartsPositions(tsPositions);
	}

	for (auto xItem = xThis->FirstChildElement("item"); xItem; xItem = xItem->NextSiblingElement("item"))
	{
		// each item can be a control or text
		const auto nPos = queryIntAttribute(xItem, "index");
		const auto iIcon = queryIntAttribute(xItem, "icon") - 1;
		const TString tsFlags(queryAttribute(xItem, "flags"));
		TString tsText(queryAttribute(xItem, "text"));
		const TString tsTooltip(queryAttribute(xItem, "tooltip"));

		if (tsFlags.find(TEXT('c'), 0))
		{
			// look for control data
			if (auto xCtrl = xItem->FirstChildElement("control"); xCtrl)
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

				_ts_sprintf(tsText, TEXT("% % % % % % %"), tsID, szType, iX, iY, iWidth, iHeight, szStyles);
				setPartContents(nPos, tsFlags, iIcon, tsText, tsTooltip);

				if (auto pInfo = this->getPartInfo(nPos); pInfo)
				{
					if (pInfo->m_xChild)
						pInfo->m_xChild->fromXml(xThis, xCtrl);
				}
			}
		}
		else
			setPartContents(nPos, tsFlags, iIcon, tsText, tsTooltip);

	}

}

const TString DcxStatusBar::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, SBARS_SIZEGRIP))
		styles.addtok(TEXT("grip"));
	if (dcx_testflag(Styles, SBARS_TOOLTIPS))
		styles.addtok(TEXT("tooltips"));
	if (dcx_testflag(Styles, CCS_NODIVIDER))
		styles.addtok(TEXT("nodivider"));
	if (dcx_testflag(Styles, CCS_TOP))
		styles.addtok(TEXT("top"));
	if (dcx_testflag(Styles, CCS_NORESIZE))
		styles.addtok(TEXT("noresize"));
	if (dcx_testflag(Styles, CCS_NOPARENTALIGN))
		if (dcx_testflag(Styles, CCS_NORESIZE))
			styles.addtok(TEXT("noauto"));
		else
			styles.addtok(TEXT("noparentalign"));
	return styles;
}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxStatusBar::setParts(const int nParts, const LPINT aWidths) noexcept
{
	return Dcx::dcxStatusBar_SetParts(m_Hwnd, nParts, aWidths);
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxStatusBar::getParts(const int nParts, LPINT aWidths) const noexcept
{
	return Dcx::dcxStatusBar_GetParts(m_Hwnd, nParts, aWidths);
}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxStatusBar::getBorders(LPINT aWidths) const noexcept
{
	return Dcx::dcxStatusBar_GetBorders(m_Hwnd, aWidths);
}

void DcxStatusBar::setPartsPositions(const TString& tsPositions)
{
	const auto nParts = tsPositions.numtok();
	if (nParts >= DCX_STATUSBAR_MAX_PARTS)
		return;

	auto parts = std::make_unique<INT[]>(DCX_STATUSBAR_MAX_PARTS);

	auto c = 0U;
	int i{};

	for (auto p : tsPositions)
	{
		if (c >= 100)
			throw Dcx::dcxException("Can't Allocate Over 100% of Statusbar!");

		const auto t = p.to_int();

		if (p.right(1) == TEXT('%'))
		{
			gsl::at(m_iDynamicParts, i) = t;
			c += t;
		}
		else
			gsl::at(m_iFixedParts, i) = t;

		gsl::at(parts, i) = t;

		++i;
	}
	setParts(gsl::narrow_cast<int>(nParts), parts.get());
	updateParts();
}

void DcxStatusBar::setPartContents(int nPos, const TString& tsFlags, int nIcon, const TString& tsText, const TString& tsTooltip)
{
	if (nPos < 0 || nPos >= this->getParts(DCX_STATUSBAR_MAX_PARTS, nullptr))
		throw Dcx::dcxException("Invalid Part");

	deletePartInfo(nPos); // delete custom info if any.
	DestroyIcon(getIcon(nPos)); // delete any icon for this part.
	setIcon(nPos, nullptr); // set as no icon.

	const auto iFlags = parseItemFlags(tsFlags);

	if (dcx_testflag(iFlags, SBT_OWNERDRAW))
	{
		auto pPart = std::make_unique<SB_PARTINFOX>();

		pPart->m_xChild = nullptr;
		pPart->m_xiIcon = nIcon;
		if (tsFlags.find(TEXT('f'), 0))
		{ // mIRC formatted text
			pPart->m_xText = tsText;
			setTipText(nPos, tsTooltip.to_chr());
		}
		else { // child control
			// this is split like this to avoid a compiler bug that caused the compiler to use >30GB ram & still fail.
			constexpr auto eAllowedControls1 = DcxAllowControls::ALLOW_PBAR | DcxAllowControls::ALLOW_TRACKBAR | DcxAllowControls::ALLOW_COMBOEX | DcxAllowControls::ALLOW_STATUSBAR | DcxAllowControls::ALLOW_TOOLBAR | DcxAllowControls::ALLOW_MULTICOMBO;
			constexpr auto eAllowedControls2 = DcxAllowControls::ALLOW_TREEVIEW | DcxAllowControls::ALLOW_LISTVIEW | DcxAllowControls::ALLOW_REBAR | DcxAllowControls::ALLOW_BUTTON | DcxAllowControls::ALLOW_EDIT;
			constexpr auto eAllowedControls3 = DcxAllowControls::ALLOW_UPDOWN | DcxAllowControls::ALLOW_IPADDRESS | DcxAllowControls::ALLOW_WEBCTRL | DcxAllowControls::ALLOW_CALANDER | DcxAllowControls::ALLOW_DIVIDER | DcxAllowControls::ALLOW_PANEL;
			constexpr auto eAllowedControls4 = DcxAllowControls::ALLOW_TAB | DcxAllowControls::ALLOW_LINE | DcxAllowControls::ALLOW_BOX | DcxAllowControls::ALLOW_RADIO | DcxAllowControls::ALLOW_CHECK | DcxAllowControls::ALLOW_TEXT | DcxAllowControls::ALLOW_SCROLL | DcxAllowControls::ALLOW_LIST;
			constexpr auto eAllowedControls5 = DcxAllowControls::ALLOW_LINK | DcxAllowControls::ALLOW_IMAGE | DcxAllowControls::ALLOW_PAGER | DcxAllowControls::ALLOW_DATETIME | DcxAllowControls::ALLOW_STACKER | DcxAllowControls::ALLOW_DIRECTSHOW | DcxAllowControls::ALLOW_WEB2CTRL;
			constexpr auto eAllowedControls = eAllowedControls1 | eAllowedControls2 | eAllowedControls3 | eAllowedControls4 | eAllowedControls5;

			auto p_Control = getParentDialog()->addControl(tsText, 1,
				eAllowedControls,
				m_Hwnd);

			/*DcxAllowControls::ALLOW_COLORCOMBO|*/
			/*DcxAllowControls::ALLOW_RICHEDIT|*/

			pPart->m_xChild = p_Control;
			ShowWindow(p_Control->getHwnd(), SW_HIDE); // hide control untill a WM_DRAWITEM is recieved.
			SendMessage(m_Hwnd, WM_SIZE, 0, 0);
		}
		setPartInfo(nPos, iFlags, pPart.release());
	}
	else {

		if (nIcon != -1)
			setIcon(nPos, ImageList_GetIcon(getImageList(), nIcon, ILD_TRANSPARENT));

		setText(nPos, iFlags, tsText.to_chr());
		setTipText(nPos, tsTooltip.to_chr());
	}
}

_NODISCARD DcxStatusBar::sbBorders DcxStatusBar::getBorders() const noexcept
{
	sbBorders aWidths{};

	Dcx::dcxStatusBar_GetBorders(m_Hwnd, reinterpret_cast<LPINT>(&aWidths));

	return aWidths;
}

/*!
 * \brief blah
 *
 * blah
 */

COLORREF DcxStatusBar::setBkColor(const COLORREF clrBk) noexcept
{
	return Dcx::dcxStatusBar_SetBkColor(m_Hwnd, clrBk);
}

COLORREF DcxStatusBar::getBkColor() const noexcept
{
	auto notthis = const_cast<DcxStatusBar*>(this);
	const auto oldClr = notthis->setBkColor(CLR_DEFAULT);
	notthis->setBkColor(oldClr);
	return oldClr;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setText(const int iPart, const int Style, const LPCTCH lpstr) noexcept
{
	return Dcx::dcxStatusBar_SetText(m_Hwnd, iPart, Style, lpstr);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setPartInfo(const int iPart, const int Style, gsl::owner<const LPSB_PARTINFOX> pPart) noexcept
{
	return Dcx::dcxStatusBar_SetPartInfo(m_Hwnd, iPart, Style, pPart);
}

LPSB_PARTINFOX DcxStatusBar::getPartInfo(const int iPart) const noexcept
{
	//if (dcx_testflag(this->getPartFlags(iPart), SBT_OWNERDRAW))
	//	return reinterpret_cast<LPSB_PARTINFOX>(SendMessage(m_Hwnd, SB_GETTEXT, gsl::narrow_cast<WPARAM>(iPart), 0));
	//return nullptr;

	return Dcx::dcxStatusBar_GetPartInfo<LPSB_PARTINFOX>(m_Hwnd, iPart);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getText(const int iPart, PTCHAR lpstr) const noexcept
{
	return Dcx::dcxStatusBar_GetText(m_Hwnd, iPart, lpstr);
}

TString DcxStatusBar::getText(const int iPart) const
{
	return Dcx::dcxStatusBar_GetText(m_Hwnd, iPart);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getTextLength(const int iPart) const noexcept
{
	return Dcx::dcxStatusBar_GetTextLength(m_Hwnd, iPart);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::setTipText(const int iPart, const LPCTCH lpstr) noexcept
{
	Dcx::dcxStatusBar_SetTipText(m_Hwnd, iPart, lpstr);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::getTipText(const int iPart, const int nSize, PTCHAR lpstr) const noexcept
{
	Dcx::dcxStatusBar_GetTipText(m_Hwnd, iPart, nSize, lpstr);
}

TString DcxStatusBar::getTipText(const int iPart) const
{
	return Dcx::dcxStatusBar_GetTipText(m_Hwnd, iPart);
}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxStatusBar::getRect(const int iPart, gsl::not_null<LPRECT> lprc) const noexcept
{
	return Dcx::dcxStatusBar_GetRect(m_Hwnd, iPart, lprc);
}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxStatusBar::setIcon(const int iPart, const HICON hIcon) noexcept
{
	return Dcx::dcxStatusBar_SetIcon(m_Hwnd, iPart, hIcon);
}

/*!
 * \brief blah
 *
 * blah
 */

HICON DcxStatusBar::getIcon(const int iPart) const noexcept
{
	return Dcx::dcxStatusBar_GetIcon(m_Hwnd, iPart);
}

/*!
* \brief blah
*
* blah
*/

UINT DcxStatusBar::getPartFlags(const int iPart) const noexcept
{
	return Dcx::dcxStatusBar_GetPartFlags(m_Hwnd, iPart);
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxStatusBar::hitTest(const POINT& pt) const noexcept
{
	const auto nParts = getParts(DCX_STATUSBAR_MAX_PARTS, nullptr);

	for (auto n = decltype(nParts){0}; n < nParts; ++n)
	{
		if (RECT rc{}; getRect(n, gsl::not_null(&rc)))
			if (PtInRect(&rc, pt))
				return n;
	}
	return -1;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxStatusBar::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
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
		case NM_CLICK:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
#if DCX_USE_WRAPPERS
				if (const Dcx::dcxCursorPos pt(m_Hwnd); pt)
					if (const auto cell = this->hitTest(pt); cell != -1)
						this->execAliasEx(TEXT("sclick,%u,%d"), getUserID(), cell + 1);
#else
				if (POINT pt{}; GetCursorPos(&pt))
				{
					MapWindowPoints(nullptr, m_Hwnd, &pt, 1);

					if (const auto cell = this->hitTest(pt); cell != -1)
						this->execAliasEx(TEXT("sclick,%u,%d"), getUserID(), cell + 1);
				}
#endif
			}
			bParsed = TRUE;
		}
		break;

		case NM_RCLICK:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
#if DCX_USE_WRAPPERS
				if (const Dcx::dcxCursorPos pt(m_Hwnd); pt)
					if (const auto cell = this->hitTest(pt); cell != -1)
						this->execAliasEx(TEXT("rclick,%u,%d"), getUserID(), cell + 1);
#else
				if (POINT pt{}; GetCursorPos(&pt))
				{
					MapWindowPoints(nullptr, m_Hwnd, &pt, 1);

					if (const auto cell = this->hitTest(pt); cell != -1)
						this->execAliasEx(TEXT("rclick,%u,%d"), getUserID(), cell + 1);
				}
#endif
			}
			bParsed = TRUE;
		}
		break;

		case NM_DBLCLK:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
#if DCX_USE_WRAPPERS
				if (const Dcx::dcxCursorPos pt(m_Hwnd); pt)
					if (const auto cell = this->hitTest(pt); cell != -1)
						this->execAliasEx(TEXT("dclick,%u,%d"), getUserID(), cell + 1);
#else
				if (POINT pt{}; GetCursorPos(&pt))
				{
					MapWindowPoints(nullptr, m_Hwnd, &pt, 1);

					if (const auto cell = this->hitTest(pt); cell != -1)
						this->execAliasEx(TEXT("dclick,%u,%d"), getUserID(), cell + 1);
				}
#endif
			}
			bParsed = TRUE;
		}
		break;
		default:
			break;
		} // switch
	}
	break;
	case WM_DRAWITEM: // support for ownerdraw statusbar. NB: NO Delete Item msg.
	{
		if (dcxlParam(LPDRAWITEMSTRUCT, lpDrawItem); m_Hwnd == lpDrawItem->hwndItem)
		{
			bParsed = TRUE;

			if (const auto* const pPart = reinterpret_cast<LPSB_PARTINFOX>(lpDrawItem->itemData); pPart)
			{
				auto rc = lpDrawItem->rcItem;

				if (pPart->m_xiIcon > -1)
				{
					if (IMAGEINFO ii{}; ImageList_GetImageInfo(this->m_hImageList, pPart->m_xiIcon, &ii))
					{
						if (ImageList_Draw(this->m_hImageList, pPart->m_xiIcon, lpDrawItem->hDC, rc.left, rc.top + ((rc.bottom - rc.top) - (ii.rcImage.bottom - ii.rcImage.top)) / 2, ILD_TRANSPARENT))
							rc.left += (ii.rcImage.right - ii.rcImage.left) + 5;
					}
				}
				if (!pPart->m_xText.empty())
					mIRC_DrawText(lpDrawItem->hDC, pPart->m_xText, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE, false);
				else if (pPart->m_xChild)
					SetWindowPos(pPart->m_xChild->getHwnd(), nullptr, rc.left, rc.top, (rc.right - rc.left), (rc.bottom - rc.top), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);

				return TRUE;
			}
		}
	}
	break;
	default:
		break;
	}
	return 0L;
}

LRESULT DcxStatusBar::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_CONTEXTMENU:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONUP:
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
		if (dcxlParam(LPCOMPAREITEMSTRUCT, idata); (idata) && (IsWindow(idata->hwndItem)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(idata->hwndItem, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_DELETEITEM:
	{
		if (dcxlParam(LPDELETEITEMSTRUCT, idata); (idata) && (IsWindow(idata->hwndItem)))
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
		if (dcxlParam(LPDRAWITEMSTRUCT, idata); (idata) && (IsWindow(idata->hwndItem)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(idata->hwndItem, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (!hdr)
			break;

		if (m_Hwnd != hdr->hwndFrom)
		{
			if (IsWindow(hdr->hwndFrom))
			{
				if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(hdr->hwndFrom, TEXT("dcx_cthis")); c_this)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
	}
	break;

	case WM_SIZE:
	{
		this->updateParts();
	}
	break;

	//case WM_PAINT:
	//	{
	//		PAINTSTRUCT ps;
	//		HDC hdc;
	//
	//		hdc = BeginPaint( m_Hwnd, &ps );
	//
	//		bParsed = TRUE;
	//
	//		// Setup alpha blend if any.
	//		LPALPHAINFO ai = this->SetupAlphaBlend(&hdc, true);
	//
	//		DcxControl::DrawCtrlBackground( hdc, this);
	//
	//		lRes = CallWindowProc( this->m_DefaultWindowProc, m_Hwnd, uMsg, (WPARAM) hdc, lParam );
	//
	//		this->FinishAlphaBlend(ai);
	//
	//		EndPaint( m_Hwnd, &ps );
	//	}
	//	break;

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

void DcxStatusBar::updateParts(void)
{
	const auto nParts = this->getParts(0, nullptr);

	if (nParts <= 0)
		return;

	RECT rcClient{};
	if (!GetClientRect(m_Hwnd, &rcClient))
		return;

	auto pParts = std::make_unique<int[]>(nParts);
	//int borders[3];

	getParts(nParts, pParts.get());

	//getBorders(borders);

	const auto w = (rcClient.right - rcClient.left); // - (2 * borders[1]);

	for (auto i = decltype(nParts){0}; i < nParts; ++i)
	{
		int pw = gsl::at(m_iFixedParts, i);

		if (gsl::at(m_iDynamicParts, i) != 0)
			pw = (w / 100) * gsl::at(m_iDynamicParts, i);

		if (i == 0)
			gsl::at(pParts, i) = pw;
		else {
			if (pw == -1)
				gsl::at(pParts, i) = -1;
			else
				gsl::at(pParts, i) = (gsl::at(pParts, i - 1) + pw);
		}
	}

	setParts(nParts, pParts.get());
}

LRESULT DcxStatusBar::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
