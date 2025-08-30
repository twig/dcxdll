#include "defines.h"
#include "DcxMultiCombo.h"

DcxMultiCombo::DcxMultiCombo(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog, DcxControlTypes::MULTICOMBO)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WS_EX_CONTROLPARENT /*| WS_EX_CLIENTEDGE*/,
		DCX_MULTICOMBOCLASS,
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

DcxMultiCombo::~DcxMultiCombo() noexcept
{
}

LRESULT DcxMultiCombo::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
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

	default:
		lRes = this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}

LRESULT DcxMultiCombo::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		const auto pDialog = getParentDialog();
		if (!pDialog)
			break;

		if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
		{
			switch (Dcx::dcxHIWORD(wParam))
			{
			case MCON_EDITCHANGE:
			{
				bParsed = TRUE;
				if (dcx_testflag(getEventMask(), DCX_EVENT_EDIT))
					this->execAliasEx(TEXT("edit,%u"), getUserID());
			}
			break;

			case MCON_SELCHANGE:
			{
				bParsed = TRUE;

				if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
				{
					const auto nItem = getCurSel();

					this->execAliasEx(TEXT("sclick,%u,%d"), getUserID(), nItem + 1);
				}
			}
			break;

			case MCON_DBLCLK:
			{
				bParsed = TRUE;

				if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
				{
					const auto nItem = getCurSel();

					this->execAliasEx(TEXT("dclick,%u,%d"), getUserID(), nItem + 1);
				}
			}
			break;
			default:
				break;
			} // switch ( HIWORD( wParam ) )
		}
	}
	break;

	case WM_MEASUREITEM:
	{
		dcxlParam(LPMEASUREITEMSTRUCT, lpmis);

		if ((lpmis->CtlType == MCOMBO_TYPE) && (lpmis->itemID == MC_ID_DROP))
		{
			//lpMeasureItem->itemHeight = 160;
			//lpMeasureItem->itemWidth = 200;

			//const auto pDialog = getParentDialog();
			//if (!pDialog)
			//	break;

			if (dcx_testflag(getEventMask(), DCX_EVENT_SIZE))
			{
				TString tsBuf(gsl::narrow_cast<TString::size_type>(128u));
				evalAliasEx(tsBuf.to_chr(), gsl::narrow_cast<int>(tsBuf.capacity_cch()), TEXT("measureitem,%u,%u,%u"), getUserID(), lpmis->itemWidth, lpmis->itemHeight);

				{
					const UINT width = tsBuf.getfirsttok(1).to_<UINT>();
					const UINT height = tsBuf.getlasttoks().to_<UINT>();

					lpmis->itemHeight = std::max(height, 20u);
					lpmis->itemWidth = std::max(width, 20u);
				}
			}
			bParsed = TRUE;
			return TRUE;
		}
	}
	break;
	default:
		break;
	}
	return 0L;
}

void DcxMultiCombo::parseInfoRequest(const TString& input, const refString<TCHAR, 4100>& szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"num"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), getCount());
		break;
		// [NAME] [ID] [PROP] [N]
	case L"colour"_hash:
	case L"color"_hash:
	{
		if (input.numtok() < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttokas<int>() - 1;	// tok 4

		if (nItem < -1 || nItem >= getCount())
			throw DcxExceptions::dcxInvalidItem();

		if (getCurStyle() != MCS_COLOUR)
			throw DcxExceptions::dcxInvalidArguments();

		const auto lpdcxcci = getColourItem(nItem);

		_ts_snprintf(szReturnValue, TEXT("%u"), lpdcxcci.m_clrItem);
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"text"_hash:
	{
		if (input.numtok() < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttokas<int>() - 1;	// tok 4

		if (nItem < -1 || nItem >= getCount())
			throw DcxExceptions::dcxInvalidItem();

		switch (const auto dStyle = getCurStyle(); dStyle)
		{
		default:
			break;
		case MCS_COLOUR:
		{
			const auto lpdcxcci = getColourItem(nItem);

			szReturnValue = lpdcxcci.m_tsItemText.to_chr();
		}
		break;

		case MCS_LISTBOX:
		{
			const auto lpdcxcci = getListBoxItem(nItem);

			szReturnValue = lpdcxcci.m_tsItemText.to_chr();
		}
		break;
		}
	}
	break;

	// [NAME] [ID] [PROP]
	case L"sel"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), getCurSel() + 1);
		break;

		// [NAME] [ID] [PROP]
	case L"seltext"_hash:
	{
		switch (const auto dStyle = getCurStyle(); dStyle)
		{
		default:
			break;
		case MCS_COLOUR:
		{
			const auto nItem = getCurSel();
			const auto lpdcxcci = getColourItem(nItem);

			szReturnValue = lpdcxcci.m_tsItemText.to_chr();
		}
		break;

		case MCS_LISTBOX:
		{
			const auto nItem = getCurSel();
			const auto lpdcxcci = getListBoxItem(nItem);

			szReturnValue = lpdcxcci.m_tsItemText.to_chr();
		}
		break;
		}
	}
	break;

	// [NAME] [ID] [PROP]
	case L"style"_hash:
		// -1 = error, 1 = dropped, 0 = not dropped.
		_ts_snprintf(szReturnValue, TEXT("%u"), getCurStyle());
		break;

		// [NAME] [ID] [PROP]
	case L"dropstate"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), getDropState());
		break;

	default:
		parseGlobalInfoRequest(input, szReturnValue);
		break;
	}
}

void DcxMultiCombo::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		this->resetContent();

	// xdid -a [NAME] [ID] [SWITCH] [RGB] (text)
	if (flags[TEXT('a')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		auto clr = input.getnexttokas<COLORREF>();
		auto tsItemText(input.getlasttoks());

		// add item to colour/listbox/listview/treeview dropdown
		addItem(clr, tsItemText);
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('c')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttokas<int>() - 1;	// tok 4

		if ((nItem < 0) || (nItem >= this->getCount()))
			throw DcxExceptions::dcxInvalidItem();

		this->setCurSel(nItem);
	}
	// xdid -C [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	else if (flags[TEXT('C')])
	{
		if (numtok < 9)
			throw DcxExceptions::dcxInvalidArguments();

		// control has a single child control, adding a new one over an existing one will remove & destroy the old one first.
		if (const auto p_Control = getParentDialog()->addControl(input, 4, DcxAllowControls::ALLOW_ALL, m_Hwnd); p_Control)
			SendMessage(m_Hwnd, MC_WM_ADDCHILD, 0, reinterpret_cast<LPARAM>(p_Control->getHwnd()));
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	//xdid -d -> [NAME] [ID] -d [N](,[N],[N1]-N2],...)
	//xdid -d -> [NAME] [ID] -d [N] [+flags] [match text]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto Ns(input.getnexttok());			// tok 4
		const XSwitchFlags xFlags(input.getnexttok());	// tok 5
		const auto nItems = this->getCount();

		if (xFlags[TEXT('+')])
		{
			// have flags, so its a match text delete
			const auto tsMatchText(input.getnexttok());

			const dcxSearchData srch_data(tsMatchText, FlagsToSearchType(xFlags));
			for (auto nPos = Ns.to_int(); nPos < nItems; ++nPos)
			{
				if (this->matchItemText(nPos, srch_data))
					SendMessage(m_Hwnd, MC_WM_DELETEITEM, nPos--, 0); // NB: we do nPos-- here as a lines just been removed so we have to check the same nPos again
			}
		}
		else {
			const auto itEnd = Ns.end();
			for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
			{
				const TString tsLine(*itStart);

				const auto [iStart, iEnd] = Dcx::getItemRange(tsLine, nItems);

				if ((iStart < 0) || (iEnd < 0) || (iStart >= nItems) || (iEnd >= nItems))
					throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

				for (auto nPos = iStart; nPos <= iEnd; ++nPos)
					SendMessage(m_Hwnd, MC_WM_DELETEITEM, nPos, 0);
			}
		}
	}
	// xdid -D [NAME] [ID] [SWITCH]
	else if (flags[TEXT('D')])
	{
		// no args, doesnt matter if child exists or not.
		SendMessage(m_Hwnd, MC_WM_REMCHILD, 0, 0);
	}
	// xdid -s [NAME] [ID] [SWITCH] [STATE]
	else if (flags[TEXT('s')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		// show/hide drop window
		if (const auto iState = input.getnexttokas<int>(); iState > 0)
			SendMessage(m_Hwnd, MC_WM_SHOWDROP, 1, 0); // show drop window
		else
			SendMessage(m_Hwnd, MC_WM_SHOWDROP, 0, 0); // hide drop window
	}
	// xdid -u [NAME] [ID] [SWITCH]
	else if (flags[TEXT('u')])
	{
		this->setCurSel(-1);
	}
	// xdid -t [NAME] [ID] [SWITCH] (TEXT)
	else if (flags[TEXT('t')])
	{
		const TString tsText(input.getlasttoks());
		SetWindowText(m_Hwnd, tsText.to_chr());
	}
	// xdid -m [NAME] [ID] [SWITCH]
	else if (flags[TEXT('m')])
	{
		COLORREF plt[mIRC_PALETTE_SIZE]{ CLR_INVALID };
		getmIRCPalette(&plt[0], gsl::narrow_cast<UINT>(std::size(plt)), false);
		SendMessage(m_Hwnd, MC_WM_FILLCOLOURS, gsl::narrow_cast<WPARAM>(std::size(plt)), reinterpret_cast<LPARAM>(&plt[0]));
	}
	// xdid -o [NAME] [ID] [SWITCH] [N] [RGB] (TEXT)
	else if (flags[TEXT('o')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttokas<int>() - 1;	// tok 4

		if ((nItem < 0) || (nItem >= this->getCount()))
			throw DcxExceptions::dcxInvalidItem();

		const auto clr = input.getnexttokas<COLORREF>();	// tok 5
		const auto tsText(input.getlasttoks());	// tok 5

		switch (const auto dStyle = getCurStyle(); dStyle)
		{
		default:
			break;

		case MCS_LISTBOX:
		case MCS_COLOUR:
		{
			MCOMBO_ITEM data;
			data.m_clrItem = clr;
			data.m_tsItemText = tsText;

			SendMessage(m_Hwnd, MC_WM_SETITEM, nItem, (LPARAM)&data);
		}
		break;

		}
	}
	// xdid -i [NAME] [ID] [SWITCH] [N] [RGB] (TEXT)
	else if (flags[TEXT('i')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttokas<int>() - 1;	// tok 4

		// nItem == -1, means add to the end.
		if ((nItem < -1) || (nItem >= this->getCount()))
			throw DcxExceptions::dcxInvalidItem();

		const auto clr = input.getnexttokas<COLORREF>();	// tok 5
		const auto tsText(input.getlasttoks());	// tok 5

		switch (const auto dStyle = getCurStyle(); dStyle)
		{
		default:
			break;

		case MCS_LISTBOX:
		case MCS_COLOUR:
		{
			MCOMBO_ITEM data;
			data.m_clrItem = clr;
			data.m_tsItemText = tsText;

			SendMessage(m_Hwnd, MC_WM_INSERTITEM, nItem, (LPARAM)&data);
		}
		break;

		}
	}
	// This is to avoid invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
	{
		//this->resetContent();
	}
	else
		parseGlobalCommandRequest(input, flags);
}

void DcxMultiCombo::addItem(COLORREF clr, const TString& tsText)
{
	// add item to colour/listbox/listview/treeview dropdown
	switch (const auto dStyle = getCurStyle(); dStyle)
	{
	default:
		break;

	case MCS_LISTBOX:
	case MCS_COLOUR:
	{
		MCOMBO_ITEM item;
		item.m_clrItem = clr;
		item.m_tsItemText = tsText;
		item.m_Type = dStyle;

		SendMessage(m_Hwnd, MC_WM_ADDITEM, 0, (LPARAM)&item);
	}
	break;

	//case MCS_LISTVIEW:
	//{
	//	TString tsText(input.getlasttoks().trim());
	//	MCOMBO_LISTVIEWITEM item;
	//	item.m_Item.mask = LVIF_TEXT | LVIF_GROUPID;
	//	item.m_Item.pszText = tsText.to_chr();
	//	//item.m_Item.cchTextMax = tsText.len();
	//	item.m_Item.iGroupId = I_GROUPIDNONE;
	//
	//	SendMessage(m_Hwnd, MC_WM_ADDITEM, 0, (LPARAM)&item);
	//}
	//break;
	//
	//case MCS_TREEVIEW:
	//{
	//	TString tsText(input.getlasttoks().trim());
	//	MCOMBO_TREEVIEWITEM item;
	//	item.m_Item.mask = TVIF_TEXT;
	//	item.m_Item.pszText = tsText.to_chr();
	//	item.m_Item.hItem = TVI_ROOT;
	//
	//	SendMessage(m_Hwnd, MC_WM_ADDITEM, 0, (LPARAM)&item);
	//}
	//break;
	}
}

bool DcxMultiCombo::matchItemText(const int nItem, const dcxSearchData& srch_data) const
{
	const MCOMBO_ITEM res = getListBoxItem(nItem);

	return DcxSearchHelper::matchItemText(res.m_tsItemText.to_chr(), srch_data);
}

void DcxMultiCombo::toXml(TiXmlElement* const xml) const
{
	if (!xml || !m_Hwnd)
		return;

	__super::toXml(xml);

	const TString wtext(TGetWindowText(m_Hwnd));
	xml->SetAttribute("caption", wtext.c_str());
	xml->SetAttribute("styles", getStyles().c_str());

	// Ook: unfinished!

	// no icons...

	TiXmlElement xDrop("drop");

	{
		RECT rcDrop{};
		SendMessage(m_Hwnd, MC_WM_GETDROPRECT, 0, reinterpret_cast<LPARAM>(&rcDrop));

		xDrop.SetAttribute("height", (rcDrop.bottom - rcDrop.top));
		xDrop.SetAttribute("width", (rcDrop.right - rcDrop.left));
	}

	switch (const auto iStyle = this->getCurStyle(); iStyle)
	{
	case MCS_CUSTOM:
	{
		if (auto ctrl = getChild(); ctrl)
		{
			TiXmlElement xChild("control");

			ctrl->toXml(&xChild);

			RECT rcDrop{};
			SendMessage(m_Hwnd, MC_WM_GETDROPRECT, 0, reinterpret_cast<LPARAM>(&rcDrop));

			if (!xChild.Attribute("height"))
				xChild.SetAttribute("height", (rcDrop.bottom - rcDrop.top));
			if (!xChild.Attribute("width"))
				xChild.SetAttribute("width", (rcDrop.right - rcDrop.left));

			xDrop.InsertEndChild(xChild);
		}
	}
	break;
	case MCS_COLOUR:
		[[fallthrough]];
	case MCS_LISTBOX:
	{
		const int nCnt = this->getCount();

		if (nCnt < 1)
			return;

		for (int nItem{}; nItem < nCnt; ++nItem)
		{
			auto ItemData = this->getListBoxItem(nItem); // does the same for both styles.

			TiXmlElement xChild("item");

			xChild.SetAttribute("text", ItemData.m_tsItemText.c_str());
			if (ItemData.m_clrItem != CLR_INVALID)
				setColourAttribute(&xChild, "textbgcolour", ItemData.m_clrItem);
			//if (ItemData.m_clrText != CLR_INVALID)
			//	xChild.SetAttribute("textcolour", ItemData.m_clrText);

			xDrop.InsertEndChild(xChild);
		}

	}
	break;
	default:
		break;
	}

	xml->InsertEndChild(xDrop);
}

TiXmlElement* DcxMultiCombo::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

std::unique_ptr<TiXmlElement> DcxMultiCombo::toXml(int blah) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml;
}

void DcxMultiCombo::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	// Ook: unfinished!
	const TString wtext(queryAttribute(xThis, "caption"));
	SetWindowText(m_Hwnd, wtext.to_chr());

	if (auto xDrop = xThis->FirstChildElement("drop"); xDrop)
	{
		if (auto xCtrl = xDrop->FirstChildElement("control"); xCtrl)
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

			TString tsInput;
			_ts_sprintf(tsInput, TEXT("% % % % % % %"), tsID, szType, iX, iY, iWidth, iHeight, szStyles);
			if (auto ctrl = getParentDialog()->addControl(tsInput, 1, DcxAllowControls::ALLOW_ALLBUTDOCK, m_Hwnd); ctrl)
			{
				ctrl->fromXml(xThis, xCtrl);

				SendMessage(m_Hwnd, MC_WM_ADDCHILD, 0, reinterpret_cast<LPARAM>(ctrl->getHwnd()));
			}
		}
		else {
			//const auto iWidth = queryIntAttribute(xDrop, "width");
			//const auto iHeight = queryIntAttribute(xDrop, "height");

			for (auto xItem = xDrop->FirstChildElement("item"); xItem; xItem = xItem->NextSiblingElement("item"))
			{
				//clr txt bg
				const TString tsText(queryAttribute(xItem, "text"));
				COLORREF clrBgText{ CLR_INVALID };

				if (auto tmp = queryColourAttribute(xItem, "textbgcolour"); tmp != CLR_INVALID)
					clrBgText = tmp;

				addItem(clrBgText, tsText);
			}
		}
	}
}

const TString DcxMultiCombo::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto style = this->getCurStyle();

	if (style == MCS_COLOUR)
		styles.addtok(TEXT("colourgrid"));
	else if (style == MCS_LISTBOX)
		styles.addtok(TEXT("listbox"));
	return styles;
}

dcxWindowStyles DcxMultiCombo::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"colorgrid"_hash:
		case L"colourgrid"_hash:
			//ws.m_Styles &= ~(MCS_LISTBOX|MCS_LISTVIEW|MCS_TREEVIEW);
			ws.m_Styles &= ~(MCS_LISTBOX);
			ws.m_Styles |= MCS_COLOUR;
			break;
		case L"listbox"_hash:
			//ws.m_Styles &= ~(MCS_COLOUR | MCS_LISTVIEW | MCS_TREEVIEW);
			ws.m_Styles &= ~(MCS_COLOUR);
			ws.m_Styles |= MCS_LISTBOX;
			break;
			//case L"listview"_hash:
			//	ws.m_Styles &= ~(MCS_LISTBOX | MCS_COLOUR | MCS_TREEVIEW);
			//	ws.m_Styles |= MCS_LISTVIEW;
			//	break;
			//case L"treeview"_hash:
			//	ws.m_Styles &= ~(MCS_LISTBOX | MCS_LISTVIEW | MCS_COLOUR);
			//	ws.m_Styles |= MCS_TREEVIEW;
			//	break;

		default:
			break;
		}
	}

	return parseGeneralControlStyles(tsStyles, ws);
}

LRESULT DcxMultiCombo::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
