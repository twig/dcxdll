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

//#ifdef DEBUG
//	if (m_bGridLayout)
//	{
//		COMBOBOXINFO cbi{ sizeof(COMBOBOXINFO) };
//		SendMessage(m_Hwnd, CB_GETCOMBOBOXINFO, 0, (LPARAM)&cbi);
//		if (cbi.hwndList)
//		{
//			//auto data = GetWindowLongPtr(m_Hwnd, 18); // 21
//
//			SetProp(cbi.hwndList, TEXT("dcxListProc"), this);
//			this->m_hListProc = SubclassWindow(cbi.hwndList, DcxColorCombo::ListProc);
//
//			this->m_hGrid = dcxCreateWindow(
//				dcxGetWindowExStyle(cbi.hwndList) & ~(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE | WS_EX_TOOLWINDOW | WS_EX_WINDOWEDGE),
//				TEXT("ComboLBox") /*WC_LISTBOX*/,
//				//(dcxGetWindowStyle(cbi.hwndList) & ~(WS_BORDER|WS_DLGFRAME)) | WindowStyle::Child | WS_VISIBLE | LBS_MULTICOLUMN | LBS_OWNERDRAWFIXED | /*LBS_NODATA |*/ LBS_NOTIFY,
//				(dcxGetWindowStyle(cbi.hwndList) & ~(WS_BORDER | WS_DLGFRAME /*| LBS_COMBOBOX*/)) | LBS_MULTICOLUMN /*| LBS_NODATA*/,
//				rc,
//				cbi.hwndList,
//				ID + 1,
//				nullptr);
//			if (m_hGrid)
//			{
//				Dcx::UXModule.dcxSetWindowTheme(m_hGrid, L" ", L" ");
//
//				SetWindowFont(m_hGrid, GetWindowFont(m_Hwnd), FALSE);
//				//SetWindowFont(m_hGrid, Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
//
//				//((PCBOX)GetWindowLongPtr(m_Hwnd, 0))->pww = (PWW)m_hGrid;
//			}
//
//			GetWindowLongPtr(m_Hwnd, GWLP_USERDATA);
//		}
//	}
//#endif

	setNoThemed(ws.m_NoTheme);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxColorCombo::~DcxColorCombo() noexcept
{
	this->resetContent();
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxColorCombo::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	ws.m_Styles |= CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
//#ifdef DEBUG
//		case L"grid"_hash:
//			m_bGridLayout = true;
//			break;
//#endif
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

/*!
 * \brief blah
 *
 * blah
 */
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
			//if (const auto item = new DCXCCOMBOITEM(clrItem); this->insertItem(nItem, item) < 0)
			//	delete item;

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

/*!
 * \brief blah
 *
 * blah
 */

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

/*!
 * \brief blah
 *
 * blah
 */

int DcxColorCombo::insertItem(const int nPos, const DCXCCOMBOITEM* dci) noexcept
{
	return ComboBox_InsertString(m_Hwnd, nPos, dci);
	//return SendMessage(m_Hwnd, CB_INSERTSTRING, gsl::narrow_cast<WPARAM>(nPos), (LPARAM)dci);
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxColorCombo::getCount() const noexcept
{
	return ComboBox_GetCount(m_Hwnd);
	//return SendMessage(m_Hwnd, CB_GETCOUNT, (WPARAM)0U, (LPARAM)0U);
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxColorCombo::setCurSel(const int nPos) noexcept
{
	return ComboBox_SetCurSel(m_Hwnd, nPos);
	//return SendMessage(m_Hwnd, CB_SETCURSEL, gsl::narrow_cast<WPARAM>(nPos), (LPARAM)0U);
}

/*!
 * \brief blah
 *
 * blah
 */

LPDCXCCOMBOITEM DcxColorCombo::getItemData(const int nItem) const noexcept
{
	return reinterpret_cast<LPDCXCCOMBOITEM>(ComboBox_GetItemData(m_Hwnd, nItem));
	//return reinterpret_cast<LPDCXCCOMBOITEM>(SendMessage( m_Hwnd, CB_GETITEMDATA, gsl::narrow_cast<WPARAM>(nItem), (LPARAM) 0U ));
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxColorCombo::getCurSel() const noexcept
{
	return ComboBox_GetCurSel(m_Hwnd);
	//return SendMessage( m_Hwnd, CB_GETCURSEL, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxColorCombo::deleteItem(const int nItem) noexcept
{
	return ComboBox_DeleteString(m_Hwnd, nItem);
	//return SendMessage(m_Hwnd, CB_DELETESTRING, gsl::narrow_cast<WPARAM>(nItem), (LPARAM)0U);
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxColorCombo::resetContent() noexcept
{
	return ComboBox_ResetContent(m_Hwnd);
	//return SendMessage(m_Hwnd, CB_RESETCONTENT, (WPARAM)0U, (LPARAM)0U);
}

BOOL DcxColorCombo::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	//const auto lpdcxcci = ListProcData->getItemData(lpdis->itemID);
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


/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxColorCombo::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		if (Dcx::dcxHIWORD(wParam) == CBN_SELENDOK)
		{
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("sclick,%u,%d"), getUserID(), getCurSel() + 1);
			bParsed = TRUE;
			return 0L;
		}

		//if (m_bGridLayout)
		//{
		//	if (Dcx::dcxHIWORD(wParam) == CBN_CLOSEUP)
		//	{
		//		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
		//			execAliasEx(TEXT("sclick,%u,%d"), getUserID(), getCurSel() + 1);
		//		bParsed = TRUE;
		//		return 0L;
		//	}
		//	//else if (Dcx::dcxHIWORD(wParam) == CBN_DROPDOWN)
		//	//{
		//	//	bParsed = TRUE;
		//	//	return 1L;
		//	//}
		//}
		//else {
		//	if (Dcx::dcxHIWORD(wParam) == CBN_SELENDOK)
		//	{
		//		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
		//			execAliasEx(TEXT("sclick,%u,%d"), getUserID(), getCurSel() + 1);
		//		bParsed = TRUE;
		//		return 0L;
		//	}
		//}
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

//#ifdef DEBUG
//		// dont bother drawing anything except the editbox when in grid style.
//		if (m_bGridLayout && !dcx_testflag(lpdis->itemState, ODS_COMBOBOXEDIT))
//		{
//			bParsed = TRUE;
//			return TRUE;
//		}
//#endif

		bParsed = TRUE;

		return DrawItem(lpdis);
	}
	break;

	case WM_MEASUREITEM:
	{
		dcxlParam(LPMEASUREITEMSTRUCT, lpmis);

		if (!lpmis)
			break;

//#ifdef DEBUG
//		if (m_bGridLayout)
//		{
//			lpmis->itemHeight = DCX_COLORCOMBO_ITEM_HEIGHT;
//			//if (lpmis->itemID != 1)
//			lpmis->itemWidth = DCX_COLORCOMBO_ITEM_HEIGHT * 2;
//		}
//		else
//			lpmis->itemHeight = DCX_COLORCOMBO_ITEM_HEIGHT;
//#else
//		lpmis->itemHeight = DCX_COLORCOMBO_ITEM_HEIGHT;
//#endif

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
		//case WM_CREATE:
		//{
		//	bParsed = TRUE;
		//	
		//	const auto lRes = this->CallDefaultClassProc(uMsg, wParam, lParam);
		//	if (m_bGridLayout)
		//	{
		//		COMBOBOXINFO cbi{};
		//		cbi.cbSize = sizeof(COMBOBOXINFO);
		//		SendMessage(m_Hwnd, CB_GETCOMBOBOXINFO, 0, (LPARAM)&cbi);
		//		if (auto data = (PCBOX)GetWindowLongPtr(m_Hwnd, 0); data)
		//		{
		//			HWND listwin = (HWND)data->pww;
		//			if (IsWindow(listwin))
		//			{
		//				const auto styles = dcxGetWindowStyle(listwin) | LBS_MULTICOLUMN;
		//				const auto exstyles = dcxGetWindowExStyle(listwin);
		//				RECT rc{};
		//				GetWindowRect(listwin, &rc);
		//				const auto list_id = GetWindowID(listwin);
		//				const auto parent = GetParent(listwin);
		//				//TCHAR buf[128]{};
		//				//GetClassName(listwin, &buf[0], std::size(buf));
		//				SetParent(listwin, nullptr);
		//	
		//				DestroyWindow(listwin);
		//				data->pww = nullptr;
		//	
		//				data->pww = (PWW)dcxCreateWindow(exstyles, TEXT("ComboLBox") /*WC_LISTBOX*/, styles, &rc, parent, list_id);
		//			}
		//		}
		//	
		//		//COMBOBOXINFO cbi{};
		//		//cbi.cbSize = sizeof(COMBOBOXINFO);
		//		//SendMessage(m_Hwnd, CB_GETCOMBOBOXINFO, 0, (LPARAM)&cbi);
		//		//if (cbi.hwndList)
		//		//{
		//		//	auto styles = dcxGetWindowStyle(cbi.hwndList);
		//		//	auto exstyles = dcxGetWindowExStyle(cbi.hwndList);
		//		//	DestroyWindow(cbi.hwndList);
		//		//}
		//	}
		//	
		//	return lRes;
		//}
		//case WM_MEASUREITEM:
		//{
		//	if (m_bGridLayout)
		//	{
		//		dcxlParam(LPMEASUREITEMSTRUCT, lpmis);
		//		if (!lpmis)
		//			break;
		//		bParsed = TRUE;
		//		lpmis->itemHeight = DCX_COLORCOMBO_ITEM_HEIGHT;
		//		lpmis->itemWidth = DCX_COLORCOMBO_ITEM_HEIGHT * 2;
		//		return TRUE;
		//	}
		//}
		//break;

	case WM_LBUTTONUP:
	{
		if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
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

//#ifdef DEBUG
//LRESULT CALLBACK DcxColorCombo::ListProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	auto ListProcData = Dcx::dcxGetProp<DcxColorCombo*>(mHwnd, TEXT("dcxListProc"));
//	if (!ListProcData)
//		return DefWindowProc(mHwnd, uMsg, wParam, lParam);
//
//	switch (uMsg)
//	{
//		//case LB_INSERTSTRING:
//		//case LB_ADDSTRING:
//		//case LB_DELETESTRING:
//		//case LB_RESETCONTENT:
//		//case LB_SETSEL:
//		//case LB_SETCURSEL:
//		//case LB_ITEMFROMPOINT:
//		//case LB_SETLOCALE:
//		//case LB_SETTABSTOPS:
//		//case LB_SETTOPINDEX:
//		//case LB_SETCOLUMNWIDTH:
//		//case LB_SELITEMRANGE:
//		//case LB_SELITEMRANGEEX:
//		//case LB_SELECTSTRING:
//		//case LB_SETCARETINDEX:
//		//case WM_MOUSEMOVE:
//		//{
//		//	if (ListProcData->m_hGrid)
//		//	{
//		//		auto lRes = CallWindowProc(ListProcData->m_hListProc, mHwnd, uMsg, wParam, lParam);
//		//		return SendMessage(ListProcData->m_hGrid, uMsg, wParam, lParam);
//		//	}
//		//}
//		//break;
//
//	case WM_WINDOWPOSCHANGING:
//	{
//		dcxlParam(LPWINDOWPOS, wp);
//		wp->cx = 300;
//		wp->cy = 200;
//		return 0L;
//	}
//	break;
//
//	case WM_WINDOWPOSCHANGED:
//	{
//		if (ListProcData->m_hGrid)
//		{
//			dcxlParam(LPWINDOWPOS, wp);
//			SetWindowPos(ListProcData->m_hGrid, HWND_TOP, 0, 0, wp->cx, wp->cy, SWP_SHOWWINDOW);
//		}
//	}
//	break;
//
//	case WM_MEASUREITEM:
//	{
//		dcxlParam(LPMEASUREITEMSTRUCT, lpmis);
//
//		if (!lpmis)
//			break;
//
//		lpmis->itemHeight = DCX_COLORCOMBO_ITEM_HEIGHT;
//		lpmis->itemWidth = DCX_COLORCOMBO_ITEM_HEIGHT * 2;
//
//		return TRUE;
//	}
//	break;
//
//	//case WM_DELETEITEM:
//	//{
//	//	return TRUE;
//	//}
//	//break;
//
//	case WM_DRAWITEM:
//	{
//		dcxlParam(LPDRAWITEMSTRUCT, lpdis);
//
//		if (!lpdis || lpdis->itemID == -1)
//			break;
//
//		//const auto lpdcxcci = ListProcData->getItemData(lpdis->itemID);
//		const auto lpdcxcci = reinterpret_cast<LPDCXCCOMBOITEM>(lpdis->itemData);
//
//		if (!lpdcxcci)
//			break;
//
//		return ListProcData->DrawItem(lpdis);
//	}
//	break;
//
//	case WM_LBUTTONUP:
//	case WM_CTLCOLOR:
//	case WM_CTLCOLORBTN:
//	case WM_CTLCOLORDLG:
//	case WM_CTLCOLOREDIT:
//	case WM_CTLCOLORLISTBOX:
//	case WM_CTLCOLORMSGBOX:
//	case WM_CTLCOLORSCROLLBAR:
//	case WM_CTLCOLORSTATIC:
//	case WM_NOTIFY:
//	case WM_COMMAND:
//	{
//		return SendMessage(GetParent(mHwnd), uMsg, wParam, lParam);
//	}
//	break;
//
//	case WM_NCDESTROY:
//	{
//		RemoveProp(mHwnd, TEXT("dcxListProc"));
//	}
//	break;
//
//	//case LBCB_STARTTRACK:
//	//case LBCB_ENDTRACK:
//	case WM_SHOWWINDOW:
//	case WM_NCPAINT:
//	case WM_PAINT:
//	case WM_DESTROY:
//		break;
//
//	default:
//		if (ListProcData->m_hGrid)
//			return SendMessage(ListProcData->m_hGrid, uMsg, wParam, lParam);
//		break;
//	}
//	return CallWindowProc(ListProcData->m_hListProc, mHwnd, uMsg, wParam, lParam);
//}
//#endif

LRESULT DcxColorCombo::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
