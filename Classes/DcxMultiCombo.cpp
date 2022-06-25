#include "defines.h"
#include "DcxMultiCombo.h"

DcxMultiCombo::DcxMultiCombo(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
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

	if (!IsWindow(m_Hwnd))
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

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

		if (dcx_testflag(pDialog->getEventMask(), DCX_EVENT_CLICK))
		{
			switch (Dcx::dcxHIWORD(wParam))
			{
			case MCON_EDITCHANGE:
			{
				bParsed = TRUE;
				if (dcx_testflag(pDialog->getEventMask(), DCX_EVENT_EDIT))
					this->execAliasEx(TEXT("edit,%u"), getUserID());
			}
			break;

			case MCON_SELCHANGE:
			{
				bParsed = TRUE;

				if (dcx_testflag(pDialog->getEventMask(), DCX_EVENT_CLICK))
				{
					const auto nItem = getCurSel();

					this->execAliasEx(TEXT("sclick,%u,%d"), getUserID(), nItem + 1);
				}
			}
			break;

			case MCON_DBLCLK:
			{
				bParsed = TRUE;

				if (dcx_testflag(pDialog->getEventMask(), DCX_EVENT_CLICK))
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

			const auto pDialog = getParentDialog();
			if (!pDialog)
				break;

			if (dcx_testflag(pDialog->getEventMask(), DCX_EVENT_SIZE))
			{
				TString tsBuf(128u);
				evalAliasEx(tsBuf.to_chr(), tsBuf.capacity_cch(), TEXT("measureitem,%u,%u,%u"), getUserID(), lpmis->itemWidth, lpmis->itemHeight);

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

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

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

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

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

		auto clr = input.getnexttok().to_<COLORREF>();
		auto tsItemText(input.getlasttoks());

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
			item.m_tsItemText = tsItemText;

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
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('c')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

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
			SendMessage(m_Hwnd, MC_WM_ADDCHILD, 0, (LPARAM)p_Control->getHwnd());
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if ((nItem < 0) || (nItem >= this->getCount()))
			throw DcxExceptions::dcxInvalidItem();

		SendMessage(m_Hwnd, MC_WM_DELETEITEM, nItem, 0);
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
		if (const auto iState = input.getnexttok().to_<int>(); iState > 0)
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
		getmIRCPalette(&plt[0], std::size(plt), false);
		SendMessage(m_Hwnd, MC_WM_FILLCOLOURS, (WPARAM)std::size(plt), (LPARAM)&plt[0]);
	}
	// xdid -o [NAME] [ID] [SWITCH] [N] [RGB] (TEXT)
	else if (flags[TEXT('o')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if ((nItem < 0) || (nItem >= this->getCount()))
			throw DcxExceptions::dcxInvalidItem();

		const auto clr = input.getnexttok().to_<COLORREF>();	// tok 5
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

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		// nItem == -1, means add to the end.
		if ((nItem < -1) || (nItem >= this->getCount()))
			throw DcxExceptions::dcxInvalidItem();

		const auto clr = input.getnexttok().to_<COLORREF>();	// tok 5
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

void DcxMultiCombo::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	const TString wtext(TGetWindowText(m_Hwnd));
	xml->SetAttribute("caption", wtext.c_str());
	xml->SetAttribute("styles", getStyles().c_str());
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
