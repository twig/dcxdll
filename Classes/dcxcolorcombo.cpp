/*!
 * \file dcxcolorcombo.cpp
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
#include "Classes/dcxcolorcombo.h"
#include "Classes/dcxtrackbar.h"
#include "Classes/dcxdialog.h"
#include "Dcx.h"

 /*!
  * \brief Constructor
  *
  * \param ID - Control ID
  * \param p_Dialog - Parent DcxDialog Object
  * \param mParentHwnd - Parent Window Handle
  * \param rc - Window Rectangle
  * \param styles - Window Style Tokenized List
  */
DcxColorCombo::DcxColorCombo(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_COLORCOMBOCLASS,
		ws.m_Styles | WindowStyle::Child,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);

	setNoThemed(ws.m_NoTheme);
}

DcxColorCombo::~DcxColorCombo() noexcept
{
	this->resetContent();
}

dcxWindowStyles DcxColorCombo::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	ws.m_Styles |= CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"shownumbers"_hash:
			m_bShowNumbers = true;
			break;

		default:
			break;
		}
	}

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

void DcxColorCombo::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
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

		if (nItem < 0 || nItem >= getCount())
			throw DcxExceptions::dcxInvalidItem();

		const auto lpdcxcci = getItemData(nItem);

		if (!lpdcxcci)
			throw Dcx::dcxException("Unable to get item data");

		_ts_snprintf(szReturnValue, TEXT("%u"), lpdcxcci->clrItem);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"sel"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), getCurSel() + 1);
		break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
		break;
	}
}

void DcxColorCombo::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		this->resetContent();

	// xdid -a [NAME] [ID] [SWITCH] [N] [RGB] (Text)
	if (flags[TEXT('a')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem >= this->getCount())
			nItem = -1;

		if (const auto clrItem = input.getnexttok().to_<COLORREF>(); nItem > -2)
		{
			const auto item = new DCXCCOMBOITEM(clrItem);

			// if optional text supplied, set it.
			if (TString txt(input.getlasttoks()); !txt.trim().empty())
				item->tsItemText = txt;

			if (this->insertItem(nItem, item) < 0)
				delete item;
		}
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('c')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if ((nItem < -1) || (nItem >= this->getCount()))
			throw DcxExceptions::dcxInvalidItem();

		this->setCurSel(nItem);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if ((nItem < -1) || (nItem >= this->getCount()))
			throw DcxExceptions::dcxInvalidItem();

		this->deleteItem(nItem);
	}
	// xdid -m [NAME] [ID] [SWITCH]
	else if (flags[TEXT('m')])
	{
		this->setmIRCPalette();
	}
	// xdid -o [NAME] [ID] [SWITCH] [N] [RGB]
	else if (flags[TEXT('o')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if ((nItem < -1) || (nItem >= this->getCount()))
			throw DcxExceptions::dcxInvalidItem();

		if (const auto lpdcxcci = this->getItemData(nItem); lpdcxcci)
			lpdcxcci->clrItem = input.getnexttok().to_<COLORREF>();	// tok 5
	}
	// xdid -t [NAME] [ID] [SWITCH] [N] [Text]
	else if (flags[TEXT('t')])
	{
		// set an items text (text can be blank to clear it)
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if ((nItem < -1) || (nItem >= this->getCount()))
			throw DcxExceptions::dcxInvalidItem();

		if (const auto lpdcxcci = this->getItemData(nItem); lpdcxcci)
			lpdcxcci->tsItemText = input.getlasttoks().trim();	// tok 5
	}
	// This is to avoid invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
	{
		//this->resetContent();
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

void DcxColorCombo::setmIRCPalette()
{
	//static const TCHAR com[] = TEXT("$color(0) $color(1) $color(2) $color(3) $color(4) $color(5) $color(6) $color(7) $color(8) $color(9) $color(10) $color(11) $color(12) $color(13) $color(14) $color(15)");
	//TString cols;
	////mIRCLinker::tsEval( cols, &com[0] );
	//mIRCLinker::eval(cols, &com[0]);
	//
	//for (const auto &col: cols)
	//{
	//	this->insertItem(-1, new DCXCCOMBOITEM(col.to_<COLORREF>()));
	//}

#if DCX_USE_WRAPPERS
	getmIRCPalette(false); // get mIRC palette

	for (const auto& col : staticPalette)
	{
		if (const auto item = new DCXCCOMBOITEM(col); this->insertItem(-1, item) < 0)
			delete item;
	}

#else
	//COLORREF cPalette[mIRC_PALETTE_SIZE] = { CLR_INVALID }; // mIRC palette
	//
	//getmIRCPalette(&cPalette[0], std::extent_v<decltype(cPalette)>); // get mIRC palette
	//
	//for (const auto &col: cPalette)
	//{
	//	this->insertItem( -1, new DCXCCOMBOITEM(col));
	//}

	COLORREF cPalette[mIRC_PALETTE_SIZE] = { CLR_INVALID }; // mIRC palette

	getmIRCPalette(cPalette); // get mIRC palette

	for (const auto& col : cPalette)
	{
		this->insertItem(-1, new DCXCCOMBOITEM(col));
	}
#endif
}

int DcxColorCombo::insertItem(const int nPos, const DCXCCOMBOITEM* dci) noexcept
{
	if (!m_Hwnd)
		return -1;

	return ComboBox_InsertString(m_Hwnd, nPos, dci);
}

int DcxColorCombo::getCount() const noexcept
{
	if (!m_Hwnd)
		return -1;

	return ComboBox_GetCount(m_Hwnd);
}

int DcxColorCombo::setCurSel(const int nPos) noexcept
{
	if (!m_Hwnd)
		return -1;

	return ComboBox_SetCurSel(m_Hwnd, nPos);
}

LPDCXCCOMBOITEM DcxColorCombo::getItemData(const int nItem) const noexcept
{
	if (!m_Hwnd)
		return nullptr;

	return reinterpret_cast<LPDCXCCOMBOITEM>(ComboBox_GetItemData(m_Hwnd, nItem));
}

int DcxColorCombo::getCurSel() const noexcept
{
	if (!m_Hwnd)
		return -1;

	return ComboBox_GetCurSel(m_Hwnd);
}

int DcxColorCombo::deleteItem(const int nItem) noexcept
{
	if (!m_Hwnd)
		return -1;

	return ComboBox_DeleteString(m_Hwnd, nItem);
}

int DcxColorCombo::resetContent() noexcept
{
	if (!m_Hwnd)
		return -1;

	return ComboBox_ResetContent(m_Hwnd);
}

BOOL DcxColorCombo::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	if (!lpdis || !lpdis->hDC)
		return FALSE;

	const auto lpdcxcci = reinterpret_cast<LPDCXCCOMBOITEM>(lpdis->itemData);

	if (!lpdcxcci)
		return FALSE;

	const auto savedDC = SaveDC(lpdis->hDC);
	Auto(RestoreDC(lpdis->hDC, savedDC));

	const auto hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	Auto(DeleteObject(hPen));

	const auto oldPen = Dcx::dcxSelectObject<HPEN>(lpdis->hDC, hPen);
	Auto(Dcx::dcxSelectObject<HPEN>(lpdis->hDC, oldPen));

	auto rcItem = lpdis->rcItem;

	// draw selection indicator
	if (dcx_testflag(lpdis->itemState, ODS_COMBOBOXEDIT))
		SetBkColor(lpdis->hDC, GetSysColor(COLOR_WINDOW));
	else if (dcx_testflag(lpdis->itemState, ODS_SELECTED))
		SetBkColor(lpdis->hDC, GetSysColor(COLOR_MENUHILIGHT));
	else
		SetBkColor(lpdis->hDC, GetSysColor(COLOR_WINDOW));
	ExtTextOut(lpdis->hDC, rcItem.left, rcItem.top, ETO_CLIPPED | ETO_OPAQUE, &rcItem, TEXT(""), 0, nullptr);

	InflateRect(&rcItem, -4, -2);

	SetBkColor(lpdis->hDC, lpdcxcci->clrItem);

	ExtTextOut(lpdis->hDC, rcItem.left, rcItem.top, ETO_CLIPPED | ETO_OPAQUE, &rcItem, TEXT(""), 0, nullptr);

	TString txt(lpdcxcci->tsItemText);

	if (this->m_bShowNumbers)
		txt.addtok(lpdis->itemID);

	if (!txt.empty())
	{
		// set text colour so it will contrast nicely with the item colour.
		if (lpdcxcci->clrText == CLR_INVALID)
			lpdcxcci->clrText = GetContrastColour(lpdcxcci->clrItem);
		SetTextColor(lpdis->hDC, lpdcxcci->clrText);

		this->ctrlDrawText(lpdis->hDC, txt, &rcItem, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}
	MoveToEx(lpdis->hDC, rcItem.left, rcItem.top, nullptr);
	LineTo(lpdis->hDC, rcItem.right, rcItem.top);

	MoveToEx(lpdis->hDC, rcItem.right, rcItem.top, nullptr);
	LineTo(lpdis->hDC, rcItem.right, rcItem.bottom);

	MoveToEx(lpdis->hDC, rcItem.right, rcItem.bottom, nullptr);
	LineTo(lpdis->hDC, rcItem.left, rcItem.bottom);

	MoveToEx(lpdis->hDC, rcItem.left, rcItem.bottom, nullptr);
	LineTo(lpdis->hDC, rcItem.left, rcItem.top);

	return TRUE;
}

void DcxColorCombo::toXml(TiXmlElement* const xml) const
{
	if (!xml || !m_Hwnd)
		return;

	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());

	xml->SetAttribute("sel", getCurSel());

	{
		RECT rc{};
		SendMessage(m_Hwnd, CB_GETDROPPEDCONTROLRECT, 0, reinterpret_cast<LPARAM>(&rc));

		xml->SetAttribute("height", (rc.bottom - rc.top));
	}
	const auto iCount = this->getCount();
	for (int nItem{}; nItem < iCount; ++nItem)
	{
		if (const auto data = this->getItemData(nItem); data)
		{
			TiXmlElement xItem("item");

			xItem.SetAttribute("text", data->tsItemText.c_str());
			setColourAttribute(&xItem, "textcolour", data->clrText);
			setColourAttribute(&xItem, "bgcolour", data->clrItem);

			xml->InsertEndChild(xItem);
		}
	}
}

TiXmlElement* DcxColorCombo::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxColorCombo::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	for (auto xItem = xThis->FirstChildElement("item"); xItem; xItem = xItem->NextSiblingElement("item"))
	{
		const TString tsText(queryAttribute(xItem, "text"));
		const auto clrTxt = queryColourAttribute(xItem, "textcolour");
		const auto clrItem = queryColourAttribute(xItem, "bgcolour");

		const auto item = new DCXCCOMBOITEM(clrItem);

		// if optional text supplied, set it.
		item->tsItemText = tsText;
		item->clrText = clrTxt;
		item->clrItem = clrItem;

		if (this->insertItem(-1, item) < 0)
			delete item;
	}
	if (const auto iSel = queryIntAttribute(xThis, "sel"); iSel > 0)
	{
		this->setCurSel(iSel);
	}
}

const TString DcxColorCombo::getStyles(void) const
{
	auto tsStyles(__super::getStyles());
	//const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (this->m_bShowNumbers)
		tsStyles.addtok(TEXT("shownumbers"));

	return tsStyles;
}

LRESULT DcxColorCombo::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		if (Dcx::dcxHIWORD(wParam) == CBN_SELENDOK)
		{
			if (dcx_testflag(this->getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("sclick,%u,%d"), getUserID(), getCurSel() + 1);
			bParsed = TRUE;
			return 0L;
		}
	}
	break;
	case WM_DELETEITEM:
	{
		dcxlParam(PDELETEITEMSTRUCT, delis);

		if (!delis)
			break;

		auto lpdcxcci = reinterpret_cast<LPDCXCCOMBOITEM>(delis->itemData);

		delete lpdcxcci;

		bParsed = TRUE;
		return TRUE;
	}
	break;

	case WM_DRAWITEM:
	{
		dcxlParam(LPDRAWITEMSTRUCT, lpdis);

		if (!lpdis || lpdis->itemID == -1 || wParam == 0)
			break;

		const auto lpdcxcci = reinterpret_cast<LPDCXCCOMBOITEM>(lpdis->itemData);

		if (!lpdcxcci)
			break;

		bParsed = TRUE;

		return DrawItem(lpdis);
	}
	break;

	case WM_MEASUREITEM:
	{
		dcxlParam(LPMEASUREITEMSTRUCT, lpmis);

		if (!lpmis)
			break;

		lpmis->itemHeight = DCX_COLORCOMBO_ITEM_HEIGHT;
		bParsed = TRUE;
		return TRUE;
	}
	break;

	default:
		break;
	}
	return 0L;
}

LRESULT DcxColorCombo::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	//const WindowMessages emsg = gsl::narrow_cast<WindowMessages>(uMsg);

	switch (uMsg)
	{
	case WM_LBUTTONUP:
	{
		if (dcx_testflag(this->getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("lbup,%u"), getUserID());
	}
	break;
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

LRESULT DcxColorCombo::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
