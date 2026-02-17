/*!
 * \file dcxrebar.cpp
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
#include "Classes/dcxrebar.h"
#include "Classes/dcxdialog.h"
#include "Dcx.h"

 /*!
  * \brief Constructor
  *
  * \param ID Control ID
  * \param p_Dialog Parent DcxDialog Object
  * \param mParentHwnd Parent Window Handle
  * \param rc Window Rectangle
  * \param styles Window Style Tokenized List
  */
DcxReBar::DcxReBar(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog, DcxControlTypes::REBAR)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WS_EX_CONTROLPARENT,
		DCX_REBARCTRLCLASS,
		ws.m_Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
	{
		//SendMessage( m_Hwnd, RB_SETWINDOWTHEME, 0, L" ");
		DcxUXModule::dcxSetWindowTheme(m_Hwnd, L" ", L" ");
	}

	setNoThemed(ws.m_NoTheme);

	this->setImageList(this->createImageList());

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
}

/*!
 * \brief Destructor
 *
 * blah
 */
DcxReBar::~DcxReBar() noexcept
{
	this->resetContents();

	auto himl = this->getImageList();
	setImageList(nullptr);
	if (himl)
		ImageList_Destroy(himl);
}

const TString DcxReBar::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, RBS_BANDBORDERS))
		styles.addtok(TEXT("borders"));
	if (dcx_testflag(Styles, RBS_DBLCLKTOGGLE))
		styles.addtok(TEXT("dblclktoggle"));
	if (dcx_testflag(Styles, RBS_FIXEDORDER))
		styles.addtok(TEXT("fixedorder"));
	if (dcx_testflag(Styles, RBS_VARHEIGHT))
		styles.addtok(TEXT("varheight"));
	if (dcx_testflag(Styles, RBS_TOOLTIPS))
		styles.addtok(TEXT("tooltips"));
	if (dcx_testflag(Styles, RBS_VERTICALGRIPPER))
		styles.addtok(TEXT("verticalgrip"));
	if (dcx_testflag(Styles, CCS_VERT))
		styles.addtok(TEXT("vertical"));
	if (dcx_testflag(Styles, CCS_RIGHT))
		styles.addtok(TEXT("right"));
	if (dcx_testflag(Styles, CCS_BOTTOM))
		styles.addtok(TEXT("bottom"));
	if (dcx_testflag(Styles, CCS_NORESIZE))
		styles.addtok(TEXT("noresize"));
	if (dcx_testflag(Styles, CCS_NOPARENTALIGN))
		styles.addtok(TEXT("noparentalign"));
	if (dcx_testflag(Styles, (CCS_NOPARENTALIGN | CCS_NORESIZE)))
		styles.addtok(TEXT("noauto"));

	return styles;
}

void DcxReBar::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());
	xml->SetAttribute("nocla", "1");

	// icons...
	if (auto himl = this->getImageList(); himl)
		xmlSaveImageList(himl, xml, L"+B"_ts);

	if (const auto count = this->getBandCount(); count > 0)
	{
		TCHAR szBuf[MIRC_BUFFER_SIZE_CCH]{};
		REBARBANDINFO rbbi{};
		if (Dcx::VersInfo.isVista()) // NB: when rbBand.cbSize is set to the Vista size on XP the insertband will FAIL!! fucking MS!
			rbbi.cbSize = sizeof(REBARBANDINFO);
		else
			GSL_SUPPRESS(es.47) rbbi.cbSize = REBARBANDINFO_V6_SIZE;
		rbbi.fMask = RBBIM_TEXT | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_IMAGE | RBBIM_COLORS | RBBIM_STYLE | RBBIM_LPARAM;

		for (auto i = decltype(count){0}; i < count; ++i)
		{
			rbbi.lpText = &szBuf[0];
			rbbi.cch = MIRC_BUFFER_SIZE_CCH;

			this->getBandInfo(i, &rbbi);

			auto lpdrbb = reinterpret_cast<DCXRBBAND*>(rbbi.lParam);

			TiXmlElement xItem("item");
			{
				TString tsText(rbbi.lpText);
				xItem.SetAttribute("text", tsText.c_str());
				xItem.SetAttribute("icon", rbbi.iImage);
				xItem.SetAttribute("fgcolour", rbbi.clrFore);
				xItem.SetAttribute("bgcolour", rbbi.clrBack);
				xItem.SetAttribute("width", rbbi.cx);
				xItem.SetAttribute("minwidth", rbbi.cxMinChild);
				xItem.SetAttribute("minheight", rbbi.cyMinChild);

				if (lpdrbb)
				{
					if (lpdrbb->clrText != CLR_INVALID)
						xItem.SetAttribute("textcolour", lpdrbb->clrText);
					if (lpdrbb->bBold)
						xItem.SetAttribute("bold", "1");
					if (lpdrbb->bUline)
						xItem.SetAttribute("underline", "1");
					if (!lpdrbb->tsTipText.empty())
						xItem.SetAttribute("tooltip", lpdrbb->tsTipText.c_str());
					if (!lpdrbb->tsMarkText.empty())
						xItem.SetAttribute("mark", lpdrbb->tsMarkText.c_str());
				}

				xItem.SetAttribute("flags", parseBandStyleFlags(rbbi.fStyle).c_str());
			}
			if (const auto* const c = this->getControl(i); c)
			{
				//const Dcx::dcxWindowRect rc(rbbi.hwndChild, m_Hwnd);

				TiXmlElement xSub("control");

				c->toXml(&xSub);

				//xSub.SetAttribute("x", rc.left);
				//xSub.SetAttribute("y", rc.top);
				//xSub.SetAttribute("height", rc.Height());
				//xSub.SetAttribute("width", rc.Width());

				xItem.InsertEndChild(xSub);
			}
			xml->InsertEndChild(xItem);
		}
	}
}

TiXmlElement* DcxReBar::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxReBar::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	{
		int iIndex{};

		for (auto xItem = xThis->FirstChildElement("item"); xItem; xItem = xItem->NextSiblingElement("item"))
		{
			TString tsText(queryAttribute(xItem, "text"));
			const auto nIcon = queryIntAttribute(xItem, "icon");
			const auto clrText = gsl::narrow_cast<COLORREF>(queryIntAttribute(xItem, "textcolour", CLR_INVALID));
			//queryIntAttribute(xItem, "fgcolour");
			//queryIntAttribute(xItem, "bgcolour");
			const auto width = queryIntAttribute(xItem, "width");
			const auto minwidth = queryIntAttribute(xItem, "minwidth");
			const auto minheight = queryIntAttribute(xItem, "minheight");
			const auto tsFlags(queryAttribute(xItem, "flags", "+"));
			const auto tsTooltip(queryAttribute(xItem, "tooltip"));

			TString control_data;
			auto xCtrl = xItem->FirstChildElement("control");
			if (xCtrl)
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

				_ts_sprintf(control_data, TEXT("% % % % % % %"), tsID, szType, iX, iY, iWidth, iHeight, szStyles);
			}

			this->addBand(iIndex, minwidth, minheight, width, nIcon, clrText, tsFlags, tsText, control_data, tsTooltip);

			if (xCtrl)
			{
				if (auto ctrl = this->getControl(iIndex); ctrl)
				{
					ctrl->fromXml(xDcxml, xCtrl);
				}
			}

			++iIndex;
		}
	}
}

DcxControl* DcxReBar::getControl(const int index) const noexcept
{
	if (index > -1 && index < this->getBandCount())
	{
		REBARBANDINFO rbBand{ sizeof(REBARBANDINFO), RBBIM_CHILD };

		if (this->getBandInfo(gsl::narrow_cast<UINT>(index), &rbBand) != 0)
			return this->getParentDialog()->getControlByHWND(rbBand.hwndChild);

	}
	return nullptr;
}

dcxWindowStyles DcxReBar::parseControlStyles(const TString& tsStyles)
{
	auto ws = parseGeneralControlStyles(tsStyles);

	ws.m_Styles |= RBS_AUTOSIZE;

	//ExStyles |= WS_EX_CONTROLPARENT;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"borders"_hash:
			ws.m_Styles |= RBS_BANDBORDERS;
			break;
		case L"dblclktoggle"_hash:
			ws.m_Styles |= RBS_DBLCLKTOGGLE;
			break;
		case L"fixedorder"_hash:
			ws.m_Styles |= RBS_FIXEDORDER;
			break;
		case L"varheight"_hash:
			ws.m_Styles |= RBS_VARHEIGHT;
			break;
		case L"tooltips"_hash:
			ws.m_Styles |= RBS_TOOLTIPS;
			break;
		case L"verticalgrip"_hash:
			ws.m_Styles |= RBS_VERTICALGRIPPER;
			break;
		case L"vertical"_hash:
			ws.m_Styles |= CCS_VERT;
			break;
		case L"right"_hash:
			ws.m_Styles |= CCS_RIGHT;
			break;
		case L"bottom"_hash:
			ws.m_Styles |= CCS_BOTTOM;
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
		default:
			break;
		}
	}

	return ws;
}

HIMAGELIST DcxReBar::getImageList() const noexcept
{
	REBARINFO ri{ sizeof(REBARINFO),RBIM_IMAGELIST };

	getBarInfo(&ri);

	return ri.himl;
}

void DcxReBar::setImageList(HIMAGELIST himl) noexcept
{
	REBARINFO ri{ sizeof(REBARINFO), RBIM_IMAGELIST, himl };

	setBarInfo(&ri);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxReBar::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	const auto numtok = input.numtok();

	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
	case L"num"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), this->getBandCount());
		break;
	// [NAME] [ID] [PROP] [N]
	case L"text"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nIndex = input.getnexttokas<int>() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_TEXT;
		rbBand.cch = MIRC_BUFFER_SIZE_CCH;
		rbBand.lpText = szReturnValue;

		getBandInfo(gsl::narrow_cast<UINT>(nIndex), &rbBand);
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"childid"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto n = input.getnexttokas<int>() - 1; // tok 4

		if (n < 0 || n >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		if (const auto* const c = this->getControl(n); c)
			_ts_snprintf(szReturnValue, TEXT("%u"), c->getUserID());
	}
	break;
	// $xdid([NAME], [ID], [N]).[PROP]
	case L"markeditem"_hash:
	{
		const auto n = input.getnexttokas<int>() - 1;	// tok 4

		if (n < 0 || n >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		//REBARBANDINFO rbi{};
		//rbi.cbSize = sizeof(REBARBANDINFO);
		//rbi.fMask = RBBIM_LPARAM;
		//getBandInfo(gsl::narrow_cast<UINT>(n), &rbi);
		//if (auto pdcxrbb = reinterpret_cast<LPDCXRBBAND>(rbi.lParam); pdcxrbb)
		//	szReturnValue = pdcxrbb->tsMarkText.to_chr();

		if (const auto pdcxrbb = reinterpret_cast<LPDCXRBBAND>(getBandInfo(gsl::narrow_cast<UINT>(n), RBBIM_LPARAM).lParam); pdcxrbb)
			szReturnValue = pdcxrbb->tsMarkText.to_chr();
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"width"_hash:
	{
		const auto n = input.getnexttokas<int>() - 1;	// tok 4

		if (n < 0 || n >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		//REBARBANDINFO rbi{};
		//rbi.cbSize = sizeof(REBARBANDINFO);
		//rbi.fMask = RBBIM_SIZE;
		//getBandInfo(gsl::narrow_cast<UINT>(n), &rbi);
		//_ts_snprintf(szReturnValue, TEXT("%u"), rbi.cx);

		_ts_snprintf(szReturnValue, TEXT("%u"), getBandInfo(gsl::narrow_cast<UINT>(n), RBBIM_SIZE).cx);
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"ideal"_hash:
	{
		const auto n = input.getnexttokas<int>() - 1;	// tok 4

		if (n < 0 || n >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		_ts_snprintf(szReturnValue, TEXT("%u"), getBandInfo(gsl::narrow_cast<UINT>(n), RBBIM_IDEALSIZE).cxIdeal);
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"maximized"_hash:
	{
		const auto n = input.getnexttokas<int>() - 1;	// tok 4

		if (n < 0 || n >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		const auto rbi = getBandInfo(gsl::narrow_cast<UINT>(n), RBBIM_IDEALSIZE | RBBIM_SIZE);
		szReturnValue = dcx_truefalse((rbi.cx == rbi.cxIdeal));

		//auto bRes{ false };
		//if (auto rbb = reinterpret_cast<DCXRBBAND*>(this->getBandInfo(gsl::narrow_cast<UINT>(n), RBBIM_LPARAM).lParam); rbb)
		//{
		//	bRes = rbb->bMaximized;
		//}
		//szReturnValue = dcx_truefalse(bRes);
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"image"_hash:
	{
		const auto n = input.getnexttokas<int>() - 1;	// tok 4

		if (n < 0 || n >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		_ts_snprintf(szReturnValue, TEXT("%u"), getBandInfo(gsl::narrow_cast<UINT>(n), RBBIM_IMAGE).iImage);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"rows"_hash:
	{
		_ts_snprintf(szReturnValue, TEXT("%u"), getRowCount());
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"rowheight"_hash:
	{
		// NB: n is a band number, not a row number. returns the height of the row containing the specified band.
		const auto n = input.getnexttokas<int>() - 1;	// tok 4

		if (n < 0 || n >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		_ts_snprintf(szReturnValue, TEXT("%u"), SendMessage(m_Hwnd, RB_GETROWHEIGHT, n, 0));
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
}

/*!
 * \brief blah
 *
 * \param input [NAME] [SWITCH] [ID] (OPTIONS)
 */

void DcxReBar::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -a [NAME] [ID] [SWITCH] [N] [+FLAGS] [CX] [CY] [WIDTH] [ICON] [COLOR] (Item Text)[TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)[TAB]Tooltip
	if (flags[TEXT('a')])
	{
		if (numtok < 10)
			throw DcxExceptions::dcxInvalidArguments();

		const auto data(input.getfirsttok(1, TSTABCHAR).trim());
		TString control_data, tooltip;
		const auto nToks = input.numtok(TSTABCHAR);

		if (nToks > 1)
		{
			control_data = input.getnexttok(TSTABCHAR).trim();			// tok 2

			if (nToks > 2)
				tooltip = input.getnexttok(TSTABCHAR).trim();			// tok 3
		}
		const auto nIndex = data.getfirsttok(4).to_<int>() - 1;			// tok 4
		const auto tsFlags = data.getnexttok();							// tok 5
		const auto cx = data.getnexttokas<UINT>();					// tok 6
		const auto cy = data.getnexttokas<UINT>();					// tok 7
		const auto width = data.getnexttokas<UINT>();				// tok 8
		const auto nIcon = data.getnexttokas<int>() - 1;			// tok 9
		const auto clrText = data.getnexttokas<COLORREF>();			// tok 10
		TString itemtext;
		if (data.numtok() > 10)
			itemtext = data.getlasttoks().trim();						// tok 11, -1

		addBand(nIndex, cx, cy, width, nIcon, clrText, tsFlags, itemtext, control_data, tooltip);
	}
	// xdid -A [NAME] [ID] [SWITCH] [N] (TEXT)
	// xdid -A [NAME] [ID] [SWITCH] [N,N2,N3-N4...] (TEXT)
	else if (flags[TEXT('A')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto Ns(input.getnexttok());	// tok 4

		const auto nItemCnt = this->getBandCount();
		const auto itEnd = Ns.end();
		REBARBANDINFO rbi{};
		rbi.cbSize = sizeof(REBARBANDINFO);
		rbi.fMask = RBBIM_LPARAM;

		for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
		{
			const auto tsLine(*itStart);

			const auto r = getItemRange2(tsLine, nItemCnt);

			if ((r.b < 0) || (r.e < 0) || (r.b > r.e))
				throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

			for (auto nItem : r)
			{
				if (this->getBandInfo(gsl::narrow_cast<UINT>(nItem), &rbi) != 0)
				{
					if (auto pdcxrbb = reinterpret_cast<LPDCXRBBAND>(rbi.lParam); pdcxrbb)
						pdcxrbb->tsMarkText = (numtok > 4 ? input.getlasttoks() : TEXT(""));	// tok 5, -1
				}
			}
		}
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	// xdid -d [NAME] [ID] [SWITCH] [N,N2,N3-N4...]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		auto Ns(input.getnexttok());	// tok 4

		// reverse sort the token list so we start at the end.
		{
			TString::SortOptions srt;
			srt.bNumeric = true;
			srt.bReverse = true;

			Ns.sorttok(srt, TSCOMMA);
		}

		const auto itEnd = Ns.end();
		for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
		{
			const auto tsLine(*itStart);
			const auto nItemCnt = this->getBandCount();

			const auto [iStart, iEnd] = getItemRange(tsLine, nItemCnt);

			if ((iStart < 0) || (iEnd < iStart) || (iStart >= nItemCnt) || (iEnd >= nItemCnt))
				throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

			// delete from highest number to lowest
			for (auto nItem = iEnd; nItem >= iStart; --nItem)
				this->deleteBand(nItem);
		}
	}
	// xdid -i [NAME] [ID] [SWITCH] [N]
	// xdid -i [NAME] [ID] [SWITCH] [N,N2,N3-N4...]
	else if (flags[TEXT('i')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto Ns(input.getnexttok());	// tok 4

		const auto nItemCnt = this->getBandCount();
		const auto itEnd = Ns.end();

		for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
		{
			const auto tsLine(*itStart);

			const auto r = getItemRange2(tsLine, nItemCnt);

			if ((r.b < 0) || (r.e < 0) || (r.b > r.e))
				throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

			for (auto nItem : r)
				this->showBand(gsl::narrow_cast<UINT>(nItem), FALSE);
		}
	}
	// xdid -j [NAME] [ID] [SWITCH] [N]
	// xdid -j [NAME] [ID] [SWITCH] [N,N2,N3-N4...]
	else if (flags[TEXT('j')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto Ns(input.getnexttok());	// tok 4

		const auto nItemCnt = this->getBandCount();
		const auto itEnd = Ns.end();

		for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
		{
			const auto tsLine(*itStart);

			const auto r = getItemRange2(tsLine, nItemCnt);

			if ((r.b < 0) || (r.e < 0) || (r.b > r.e))
				throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

			for (auto nItem : r)
				this->showBand(gsl::narrow_cast<UINT>(nItem), TRUE);
		}
	}
	// xdid -k [NAME] [ID] [SWITCH] [N] [ICON]
	// xdid -k [NAME] [ID] [SWITCH] [N,N2,N3-N4...] [ICON]
	else if (flags[TEXT('k')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto Ns(input.getnexttok());	// tok 4
		const auto nIcon = input.getnexttokas<int>() - 1;	// tok 5

		const auto nItemCnt = this->getBandCount();
		const auto itEnd = Ns.end();

		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_IMAGE;
		rbBand.iImage = nIcon;

		for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
		{
			const auto tsLine(*itStart);

			const auto r = getItemRange2(tsLine, nItemCnt);

			if ((r.b < 0) || (r.e < 0) || (r.b > r.e))
				throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

			for (auto nItem : r)
				this->setBandInfo(nItem, &rbBand);
		}
	}
	// xdid -l -> [NAME] [ID] -l [N|ALL]
	// xdid -l -> [NAME] [ID] -l [N,N2,N3-N4...]
	else if (flags[TEXT('l')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_STYLE;

		const auto nItems = this->getBandCount();
		const auto tsItem(input.getnexttok());	// tok 4

		if (tsItem == TEXT("all"))
		{
			for (auto i = decltype(nItems){0}; i < nItems; ++i)
			{
				if (this->getBandInfo(i, &rbBand) != 0)
				{
					rbBand.fStyle |= RBBS_NOGRIPPER;
					this->setBandInfo(i, &rbBand);
				}
			}
		}
		else {
			const auto itEnd = tsItem.end();

			for (auto itStart = tsItem.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
			{
				const auto tsLine(*itStart);

				const auto r = getItemRange2(tsLine, nItems);

				if ((r.b < 0) || (r.e < 0) || (r.b > r.e))
					throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

				for (auto nIndex : r)
				{
					if (nIndex < 0 || nIndex >= nItems || this->getBandInfo(nIndex, &rbBand) == 0)
						throw DcxExceptions::dcxInvalidItem();

					rbBand.fStyle |= RBBS_NOGRIPPER;
					this->setBandInfo(nIndex, &rbBand);
				}
			}
		}
	}
	// xdid -m [NAME] [ID] [SWITCH] [N] (IDEAL)
	else if (flags[TEXT('m')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nIndex = input.getnexttokas<int>() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		const auto bIdeal = (input.getnexttokas<int>() > 0);

		this->maxBand(nIndex, bIdeal);
	}
	// xdid -n [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('n')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nIndex = input.getnexttokas<int>() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw DcxExceptions::dcxInvalidItem();

		this->minBand(nIndex);
	}
	// xdid -q [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('q')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nRows = input.getnexttokas<int>();	// tok 4

		if (nRows > -1)
			this->m_iRowLimit = nRows;
	}
	// xdid -t [NAME] [ID] [SWITCH] [N] (TEXT)
	// xdid -t [NAME] [ID] [SWITCH] [N,N2,N3-N4...] (TEXT)
	else if (flags[TEXT('t')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto Ns(input.getnexttok());	// tok 4
		TString itemtext;
		if (numtok > 4)
			itemtext = input.getlasttoks().trim();	// tok 5, -1

		const auto nItemCnt = this->getBandCount();
		const auto itEnd = Ns.end();

		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_TEXT;
		rbBand.lpText = itemtext.to_chr();

		for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
		{
			const auto tsLine(*itStart);

			const auto r = getItemRange2(tsLine, nItemCnt);

			if ((r.b < 0) || (r.e < 0) || (r.b > r.e))
				throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

			for (auto nItem : r)
				this->setBandInfo(nItem, &rbBand);
		}
	}
	// xdid -u [NAME] [ID] [SWITCH] [N|ALL]
	// xdid -u [NAME] [ID] [SWITCH] [N,N2,N3-N4...]
	else if (flags[TEXT('u')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_STYLE;

		const auto nItems = this->getBandCount();
		const auto tsItem(input.getnexttok());	// tok 4

		if (tsItem == TEXT("all"))
		{
			for (auto i = decltype(nItems){0}; i < nItems; ++i)
			{
				if (this->getBandInfo(i, &rbBand) != 0)
				{
					rbBand.fStyle &= ~RBBS_NOGRIPPER;
					this->setBandInfo(i, &rbBand);
				}
			}
		}
		else {
			const auto itEnd = tsItem.end();

			for (auto itStart = tsItem.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
			{
				const auto tsLine(*itStart);

				const auto r = getItemRange2(tsLine, nItems);

				if ((r.b < 0) || (r.e < 0) || (r.b > r.e))
					throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

				for (auto nIndex : r)
				{
					if (nIndex < 0 || nIndex >= nItems || this->getBandInfo(nIndex, &rbBand) == 0)
						throw DcxExceptions::dcxInvalidItem();

					rbBand.fStyle &= ~RBBS_NOGRIPPER;
					this->setBandInfo(nIndex, &rbBand);
				}
			}
		}
	}
	// xdid -v [NAME] [ID] [SWITCH] [NFrom] [NTo]
	else if (flags[TEXT('v')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nIndexFrom = input.getnexttokas<int>() - 1;	// tok 4
		const auto nIndexTo = input.getnexttokas<int>() - 1;		// tok 4
		const auto nItems = this->getBandCount();

		if (nIndexFrom < 0 || nIndexFrom >= nItems || nIndexTo < 0 || nIndexTo >= nItems)
			throw DcxExceptions::dcxInvalidItem();

		if (this->moveBand(gsl::narrow_cast<UINT>(nIndexFrom), gsl::narrow_cast<UINT>(nIndexTo)) == 0)
			throw Dcx::dcxException("Move failed");

		SendMessage(m_Hwnd, WM_SIZE, 0, 0); // NB: redraw does nothing here, needs a size to force the update.
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [N,N2,N3-N4...] [FILENAME] (TODO: allows loading a range of icons into the control)
	else if (flags[TEXT('w')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto flag(input.getnexttok());		// tok 4
		const auto tsRanges(input.getnexttok());	// tok 5
		auto filename(input.getlasttoks());			// tok 6, -1

		this->loadIcon(flag, tsRanges, filename);
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if (flags[TEXT('y')])
	{
		auto himl = this->getImageList();
		setImageList(nullptr);
		if (himl)
			ImageList_Destroy(himl);
	}
	// xdid -W [NAME] [ID] [SWITCH] [N|ALL] [WIDTH]
	// xdid -W [NAME] [ID] [SWITCH] [N,N2,N3-N4...|all] [WIDTH]
	else if (flags[TEXT('W')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_SIZE;

		const auto nItems = this->getBandCount();
		const auto tsItem(input.getnexttok());	// tok 4
		rbBand.cx = input.getnexttokas<UINT>();

		if (tsItem == TEXT("all"))
		{
			for (auto i = decltype(nItems){0}; i < nItems; ++i)
			{
				if (this->getBandInfo(i, &rbBand) != 0)
					this->setBandInfo(i, &rbBand);
			}
		}
		else {
			const auto itEnd = tsItem.end();

			for (auto itStart = tsItem.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
			{
				const auto tsLine(*itStart);

				const auto r = getItemRange2(tsLine, nItems);

				if ((r.b < 0) || (r.e < 0) || (r.b > r.e))
					throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

				for (auto nIndex : r)
				{
					if (nIndex < 0 || nIndex >= nItems || this->getBandInfo(nIndex, &rbBand) == 0)
						throw DcxExceptions::dcxInvalidItem();

					this->setBandInfo(nIndex, &rbBand);
				}
			}
		}
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

void DcxReBar::resetContents() noexcept
{
	auto nItems = gsl::narrow_cast<UINT>(this->getBandCount());

	while (nItems--)
		this->deleteBand(nItems);
}

UINT DcxReBar::parseBandStyleFlags(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('b')])
		iFlags |= RBBS_BOLD;
	if (xflags[TEXT('c')])
		iFlags |= RBBS_COLOR;
	if (xflags[TEXT('e')])
		iFlags |= RBBS_CHILDEDGE;
	if (xflags[TEXT('f')])
		iFlags |= RBBS_FIXEDSIZE;
	if (xflags[TEXT('g')])
		iFlags |= RBBS_GRIPPERALWAYS;
	if (xflags[TEXT('h')])
		iFlags |= RBBS_HIDDEN;
	if (xflags[TEXT('n')])
		iFlags |= RBBS_NOGRIPPER;
	if (xflags[TEXT('o')])
		iFlags |= RBBS_USECHEVRON;
	if (xflags[TEXT('u')])
		iFlags |= RBBS_UNDERLINE;
	if (xflags[TEXT('v')])
		iFlags |= RBBS_VARIABLEHEIGHT;
	if (xflags[TEXT('w')])
		iFlags |= RBBS_BREAK;

	return iFlags;
}

TString DcxReBar::parseBandStyleFlags(UINT flags) noexcept
{
	TString tsFlags(L"+");

	if (dcx_testflag(flags, RBBS_BOLD))
		tsFlags += L'b';
	if (dcx_testflag(flags, RBBS_COLOR))
		tsFlags += L'c';
	if (dcx_testflag(flags, RBBS_CHILDEDGE))
		tsFlags += L'e';
	if (dcx_testflag(flags, RBBS_FIXEDSIZE))
		tsFlags += L'f';
	if (dcx_testflag(flags, RBBS_GRIPPERALWAYS))
		tsFlags += L'g';
	if (dcx_testflag(flags, RBBS_HIDDEN))
		tsFlags += L'h';
	if (dcx_testflag(flags, RBBS_NOGRIPPER))
		tsFlags += L'n';
	if (dcx_testflag(flags, RBBS_USECHEVRON))
		tsFlags += L'o';
	if (dcx_testflag(flags, RBBS_UNDERLINE))
		tsFlags += L'u';
	if (dcx_testflag(flags, RBBS_VARIABLEHEIGHT))
		tsFlags += L'v';
	if (dcx_testflag(flags, RBBS_BREAK))
		tsFlags += L'w';

	return tsFlags;
}

void DcxReBar::addBand(int nIndex, UINT cx, UINT cy, UINT width, int nIcon, COLORREF clrText, const TString& tsFlags, const TString& tsText, const TString& tsControl_data, const TString& tsTooltip)
{
	if (cx > width)
		throw Dcx::dcxException("Invalid width: width is less than min width");

	REBARBANDINFO rbBand{};
	if (Dcx::DwmModule.isUseable()) // NB: when rbBand.cbSize is set to the Vista size on XP the insertband will FAIL!! fucking MS!
		rbBand.cbSize = sizeof(REBARBANDINFO);
	else
		GSL_SUPPRESS(es.47) rbBand.cbSize = REBARBANDINFO_V6_SIZE;

	rbBand.fMask = RBBIM_STYLE | RBBIM_LPARAM;

	rbBand.fStyle = parseBandStyleFlags(tsFlags);

	if (nIndex < -1)
		nIndex = -1;

	rbBand.cxMinChild = cx;
	rbBand.cyMinChild = cy;
	rbBand.cx = width;
	rbBand.cyIntegral = 1;
	rbBand.cyChild = cy;

	rbBand.cxIdeal = width;

	if (!tsText.empty())
	{
		rbBand.fMask |= RBBIM_TEXT;
		rbBand.lpText = const_cast<TCHAR*>(tsText.to_chr());
	}

	// Tooltip Handling
	auto lpdcxrbb = std::make_unique<DCXRBBAND>();

	lpdcxrbb->bUline = dcx_testflag(rbBand.fStyle, RBBS_UNDERLINE);

	lpdcxrbb->bBold = dcx_testflag(rbBand.fStyle, RBBS_BOLD);

	if (dcx_testflag(rbBand.fStyle, RBBS_COLOR))
		lpdcxrbb->clrText = clrText;
	else
		lpdcxrbb->clrText = CLR_INVALID;

	if (nIcon > -1)
	{
		rbBand.iImage = nIcon;
		rbBand.fMask |= RBBIM_IMAGE;
	}

	rbBand.lParam = reinterpret_cast<LPARAM>(lpdcxrbb.get());

	DcxControl* p_Control{ nullptr };
	if (tsControl_data.numtok() > 5)
	{
		p_Control = this->getParentDialog()->addControl(tsControl_data, 1,
			DcxAllowControls::ALLOW_TRACKBAR |
			DcxAllowControls::ALLOW_PBAR |
			DcxAllowControls::ALLOW_COMBOEX |
			DcxAllowControls::ALLOW_TOOLBAR |
			DcxAllowControls::ALLOW_STATUSBAR |
			DcxAllowControls::ALLOW_TREEVIEW |
			DcxAllowControls::ALLOW_LISTVIEW |
			DcxAllowControls::ALLOW_COLORCOMBO |
			DcxAllowControls::ALLOW_BUTTON |
			DcxAllowControls::ALLOW_RICHEDIT |
			DcxAllowControls::ALLOW_DIVIDER |
			DcxAllowControls::ALLOW_PANEL |
			DcxAllowControls::ALLOW_TAB |
			DcxAllowControls::ALLOW_IMAGE |
			DcxAllowControls::ALLOW_EDIT |
			DcxAllowControls::ALLOW_CHECK |
			DcxAllowControls::ALLOW_RADIO |
			DcxAllowControls::ALLOW_TEXT |
			DcxAllowControls::ALLOW_MULTICOMBO, m_Hwnd);

		if (const auto dct = p_Control->getControlType(); ((dct == DcxControlTypes::STATUSBAR) || (dct == DcxControlTypes::TOOLBAR)))
			p_Control->addStyle(WindowStyle::CCS_NoParentAlign | CCS_NORESIZE);

		rbBand.fMask |= RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_ID | RBBIM_IDEALSIZE;
		rbBand.hwndChild = p_Control->getHwnd();
		rbBand.wID = p_Control->getID();
	}

	if (this->insertBand(nIndex, &rbBand) == 0L)
	{ // 0L means failed.
		this->getParentDialog()->deleteControl(p_Control);
		if (rbBand.hwndChild)
			DestroyWindow(rbBand.hwndChild);

		throw Dcx::dcxException("Unable To Add Band");
	}
	lpdcxrbb.release();
}

bool DcxReBar::insertBand(const int uIndex, const LPREBARBANDINFO lprbbi) noexcept
{
	return !!SendMessage(m_Hwnd, RB_INSERTBAND, gsl::narrow_cast<WPARAM>(uIndex), reinterpret_cast<LPARAM>(lprbbi));
}

bool DcxReBar::deleteBand(const UINT uIndex) noexcept
{
	return !!SendMessage(m_Hwnd, RB_DELETEBAND, gsl::narrow_cast<WPARAM>(uIndex), gsl::narrow_cast<LPARAM>(0));
}

bool DcxReBar::getBandInfo(const UINT uBand, const LPREBARBANDINFO lprbbi) const noexcept
{
	return !!SendMessage(m_Hwnd, RB_GETBANDINFO, gsl::narrow_cast<WPARAM>(uBand), reinterpret_cast<LPARAM>(lprbbi));
}

REBARBANDINFO DcxReBar::getBandInfo(const UINT uBand, const UINT uMask) const noexcept
{
	REBARBANDINFO rbi{};
	rbi.cbSize = sizeof(REBARBANDINFO);
	rbi.fMask = uMask;

	getBandInfo(uBand, &rbi);

	return rbi;
}

bool DcxReBar::setBandInfo(const UINT uBand, const LPREBARBANDINFO lprbbi) noexcept
{
	return !!SendMessage(m_Hwnd, RB_SETBANDINFO, gsl::narrow_cast<WPARAM>(uBand), reinterpret_cast<LPARAM>(lprbbi));
}

bool DcxReBar::setBarInfo(const LPREBARINFO lprbi) noexcept
{
	return !!SendMessage(m_Hwnd, RB_SETBARINFO, gsl::narrow_cast<WPARAM>(0), reinterpret_cast<LPARAM>(lprbi));
}

bool DcxReBar::getBarInfo(const LPREBARINFO lprbi) const noexcept
{
	return !!SendMessage(m_Hwnd, RB_GETBARINFO, gsl::narrow_cast<WPARAM>(0), reinterpret_cast<LPARAM>(lprbi));
}

int DcxReBar::getRowCount() const noexcept
{
	return gsl::narrow_cast<int>(SendMessage(m_Hwnd, RB_GETROWCOUNT, gsl::narrow_cast<WPARAM>(0), gsl::narrow_cast<LPARAM>(0)));
}

int DcxReBar::hitTest(const LPRBHITTESTINFO lprbht) const noexcept
{
	return gsl::narrow_cast<int>(SendMessage(m_Hwnd, RB_HITTEST, gsl::narrow_cast<WPARAM>(0), reinterpret_cast<LPARAM>(lprbht)));
}

HWND DcxReBar::getToolTips() const noexcept
{
	return to_hwnd(SendMessage(m_Hwnd, RB_GETTOOLTIPS, gsl::narrow_cast<WPARAM>(0), gsl::narrow_cast<LPARAM>(0)));
}

void DcxReBar::setToolTips(const HWND hwndToolTip) noexcept
{
	SendMessage(m_Hwnd, RB_SETTOOLTIPS, reinterpret_cast<WPARAM>(hwndToolTip), gsl::narrow_cast<LPARAM>(0));
}

int DcxReBar::getIDToIndex(const UINT uBandID) const noexcept
{
	return gsl::narrow_cast<int>(SendMessage(m_Hwnd, RB_IDTOINDEX, gsl::narrow_cast<WPARAM>(uBandID), gsl::narrow_cast<LPARAM>(0)));
}

int DcxReBar::getBandCount() const noexcept
{
	return gsl::narrow_cast<int>(SendMessage(m_Hwnd, RB_GETBANDCOUNT, gsl::narrow_cast<WPARAM>(0), gsl::narrow_cast<LPARAM>(0)));
}

bool DcxReBar::showBand(const UINT uBand, const BOOL fShow) noexcept
{
	return !!SendMessage(m_Hwnd, RB_SHOWBAND, gsl::narrow_cast<WPARAM>(uBand), gsl::narrow_cast<LPARAM>(fShow));
}

bool DcxReBar::moveBand(const UINT iFrom, const UINT iTo) noexcept
{
	return !!SendMessage(m_Hwnd, RB_MOVEBAND, gsl::narrow_cast<WPARAM>(iFrom), gsl::narrow_cast<LPARAM>(iTo));
}

void DcxReBar::maxBand(const UINT uBand, const BOOL fIdeal) noexcept
{
	SendMessage(m_Hwnd, RB_MAXIMIZEBAND, gsl::narrow_cast<WPARAM>(uBand), gsl::narrow_cast<LPARAM>(fIdeal));
}

void DcxReBar::minBand(const UINT uBand) noexcept
{
	SendMessage(m_Hwnd, RB_MINIMIZEBAND, gsl::narrow_cast<WPARAM>(uBand), 0);
}

void DcxReBar::loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc)
{
	auto himl = this->getImageList();
	if (!himl)
	{
		himl = this->createImageList();

		if (himl)
			this->setImageList(himl);
	}

	if (!himl)
		throw Dcx::dcxException("Unable to get imagelist");

	auto filename(tsSrc);

	Dcx::dcxLoadIconRange(himl, filename, false, tsFlags, tsIndex);
}

LRESULT DcxReBar::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
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
		case NM_CUSTOMDRAW:
		{
			dcxlParam(LPNMCUSTOMDRAW, lpncd);

			bParsed = TRUE;

			switch (lpncd->dwDrawStage)
			{
			case CDDS_PREPAINT:
				return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);
			case CDDS_ITEMPREPAINT:
			{
				const auto lpdcxrbb = reinterpret_cast<LPDCXRBBAND>(lpncd->lItemlParam);

				if (!lpdcxrbb)
					return CDRF_DODEFAULT;

				if (lpdcxrbb->clrText != CLR_INVALID)
					SetTextColor(lpncd->hdc, lpdcxrbb->clrText);
			}
			return (CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT);
			case CDDS_ITEMPOSTPAINT:
				return CDRF_DODEFAULT;
			default:
				return CDRF_DODEFAULT;
			}
		} // NM_CUSTOMDRAW
		break;

		case RBN_HEIGHTCHANGE:
		{
			bParsed = TRUE;

			if (RECT rc{}; GetWindowRect(m_Hwnd, &rc))
			{
				if (const UINT width = gsl::narrow_cast<UINT>(rc.right - rc.left), height = gsl::narrow_cast<UINT>(rc.bottom - rc.top); (m_iWidth != width || m_iHeight != height))
				{
					execAliasEx(TEXT("change,%u,%d,%d"), getUserID(), width, height);

					m_iWidth = width;
					m_iHeight = height;
				}
			}
		}
		break;

		case RBN_ENDDRAG:
		{
			bParsed = TRUE;
			redrawWindow();
		}
		break;

		case RBN_DELETINGBAND:
		{
			bParsed = TRUE;

			dcxlParam(LPNMREBAR, lpnmrb);

			if (!lpnmrb)
				break;

			REBARBANDINFO rbBand{};
			rbBand.cbSize = sizeof(REBARBANDINFO);
			rbBand.fMask = RBBIM_CHILD;

			if (getBandInfo(lpnmrb->uBand, &rbBand) != 0)
			{
				if (IsWindow(rbBand.hwndChild))
					getParentDialog()->deleteControl(getParentDialog()->getControlByHWND(rbBand.hwndChild));

				auto lpdcxrbb = reinterpret_cast<LPDCXRBBAND>(lpnmrb->lParam);
				delete lpdcxrbb;
			}
		}
		break;
		default:
			break;
		} // switch
	}
	break;
	default:
		break;
	}

	return 0L;
}

LRESULT DcxReBar::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		if (dcxlParam(LPNMHDR, hdr); (hdr) && IsWindow(hdr->hwndFrom))
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

	case WM_LBUTTONUP:
	{
		if (RBHITTESTINFO rbhi{}; GetCursorPos(&rbhi.pt))
		{
			MapWindowPoints(nullptr, m_Hwnd, &rbhi.pt, 1);

			if (const auto band = hitTest(&rbhi); (dcx_testflag(rbhi.flags, RBHT_GRABBER) || dcx_testflag(rbhi.flags, RBHT_CAPTION)) && band != -1)
			{
				m_iClickedBand = band;

				if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
					this->execAliasEx(TEXT("sclick,%u,%d"), getUserID(), band + 1);
			}
		}
	}
	break;

	case WM_CONTEXTMENU:
	{
		if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
		{
			if (RBHITTESTINFO rbhi{}; GetCursorPos(&rbhi.pt))
			{
				MapWindowPoints(nullptr, m_Hwnd, &rbhi.pt, 1);

				if (const auto band = this->hitTest(&rbhi); band != -1)
					this->execAliasEx(TEXT("rclick,%u,%d"), getUserID(), band + 1);
			}
		}
	}
	break;

	case WM_SIZE:
	{
		InvalidateRect(m_Hwnd, nullptr, TRUE);
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

LRESULT DcxReBar::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
