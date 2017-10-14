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
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxColorCombo::DcxColorCombo(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
{
	const auto[bNoTheme, Styles, ExStyles] = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ExStyles,
		DCX_COLORCOMBOCLASS,
		Styles | WindowStyle::Child,
		rc,
		mParentHwnd,
		ID);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxColorCombo::~DcxColorCombo()
{
	this->resetContent();
	this->unregistreDefaultWindowProc();
}

/*!
 * \brief blah
 *
 * blah
 */

//void DcxColorCombo::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
//{
//	*Styles |= CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED;
//
//	this->parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
//}

std::tuple<NoTheme, WindowStyle, WindowExStyle> DcxColorCombo::parseControlStyles(const TString & tsStyles)
{
	WindowStyle Styles(WindowStyle::None);
	WindowExStyle ExStyles(WindowExStyle::None);

	Styles |= CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED;

	return parseGeneralControlStyles(tsStyles, Styles, ExStyles);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxColorCombo::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"num"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), getCount());
		break;
		// [NAME] [ID] [PROP] [N]
	case L"color"_hash:
	{
		if (input.numtok() < 4)
			throw Dcx::dcxException("Invalid number of argumnets");

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem < 0 || nItem >= getCount())
			throw Dcx::dcxException("Invalid Item");

		auto lpdcxcci = getItemData(nItem);

		if (lpdcxcci == nullptr)
			throw Dcx::dcxException("Unable to get item data");

		_ts_snprintf(szReturnValue, TEXT("%u"), lpdcxcci->clrItem);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"sel"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), gsl::narrow_cast<int>(getCurSel()) + 1);
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
void DcxColorCombo::parseCommandRequest( const TString &input)
{
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		this->resetContent();

	// xdid -a [NAME] [ID] [SWITCH] [N] [RGB]
	if (flags[TEXT('a')] && numtok > 4)
	{
		auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem >= this->getCount())
			nItem = -1;

		if (const auto clrItem = input.getnexttok().to_<COLORREF>(); nItem > -2) {
			this->insertItem(nItem, new DCXCCOMBOITEM(clrItem));
		}
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('c')] && numtok > 3)
	{
		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if ((nItem < -1) || (nItem >= this->getCount()))
			throw Dcx::dcxException("Item out of range");
	
		this->setCurSel(nItem);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')] && numtok > 3)
	{
		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if ((nItem < -1) || (nItem >= this->getCount()))
			throw Dcx::dcxException("Item out of range");
		
		this->deleteItem(nItem);
	}
	// xdid -m [NAME] [ID] [SWITCH]
	else if (flags[TEXT('m')])
	{
		this->setmIRCPalette();
	}
	// xdid -o [NAME] [ID] [SWITCH] [N] [RGB]
	else if (flags[TEXT('o')] && numtok > 4)
	{
		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if ((nItem < -1) || (nItem >= this->getCount()))
			throw Dcx::dcxException("Item out of range");

		if (auto lpdcxcci = this->getItemData(nItem); lpdcxcci != nullptr)
			lpdcxcci->clrItem = input.getnexttok().to_<COLORREF>();	// tok 5
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

void DcxColorCombo::setmIRCPalette( )
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
	getmIRCPalette(); // get mIRC palette

	for (const auto &col : staticPalette)
	{
		this->insertItem(-1, new DCXCCOMBOITEM(col));
	}
	//Dcx::dcxPalette cPalette(); // mIRC palette
	//
	//for (const auto &col : cPalette)
	//{
	//	this->insertItem(-1, new DCXCCOMBOITEM(col));
	//}
#else
	COLORREF cPalette[mIRC_PALETTE_SIZE] = { CLR_INVALID }; // mIRC palette

	getmIRCPalette(&cPalette[0], Dcx::countof(cPalette)); // get mIRC palette

	for (const auto &col: cPalette)
	{
		this->insertItem( -1, new DCXCCOMBOITEM(col));
	}
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxColorCombo::insertItem(const int nPos, const DCXCCOMBOITEM *dci)
{
	return ComboBox_InsertString(m_Hwnd, nPos, dci);
	//return SendMessage(m_Hwnd, CB_INSERTSTRING, gsl::narrow_cast<WPARAM>(nPos), lParam);
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

int DcxColorCombo::getCurSel( ) const
{
	return ComboBox_GetCurSel(m_Hwnd);
	//return SendMessage( m_Hwnd, CB_GETCURSEL, (WPARAM) 0U, (LPARAM) 0U );
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxColorCombo::deleteItem(const int nItem)
{
	return ComboBox_DeleteString(m_Hwnd, nItem);
	//return SendMessage(m_Hwnd, CB_DELETESTRING, gsl::narrow_cast<WPARAM>(nItem), (LPARAM)0U);
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxColorCombo::resetContent()
{
	return ComboBox_ResetContent(m_Hwnd);
	//return SendMessage(m_Hwnd, CB_RESETCONTENT, (WPARAM)0U, (LPARAM)0U);
}


/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxColorCombo::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		if (HIWORD(wParam) == CBN_SELENDOK)
		{
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("sclick,%u,%d"), getUserID(), getCurSel() + 1);
			bParsed = TRUE;
			return 0L;
		}
	}
	break;
	case WM_DELETEITEM:
	{
		dcxlParam(PDELETEITEMSTRUCT, delis);

		if (delis == nullptr)
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

		if (lpdis == nullptr || lpdis->itemID == -1)
			break;

		const auto lpdcxcci = reinterpret_cast<LPDCXCCOMBOITEM>(lpdis->itemData);

		if (lpdcxcci == nullptr)
			break;

		const auto hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		Auto(DeletePen(hPen));

		const auto oldPen = SelectPen(lpdis->hDC, hPen);
		Auto(SelectPen(lpdis->hDC, oldPen));

		auto rcItem = lpdis->rcItem;

		// draw selection indicator
		if (dcx_testflag(lpdis->itemState, ODS_COMBOBOXEDIT))
			SetBkColor(lpdis->hDC, GetSysColor(COLOR_WINDOW));
		else if (dcx_testflag(lpdis->itemState, ODS_SELECTED))
			SetBkColor(lpdis->hDC, GetSysColor(COLOR_MENUHILIGHT));
		else
			SetBkColor(lpdis->hDC, GetSysColor(COLOR_WINDOW));
		ExtTextOut(lpdis->hDC, rcItem.left, rcItem.top, ETO_CLIPPED | ETO_OPAQUE, &rcItem, TEXT(""), NULL, nullptr);

		InflateRect(&rcItem, -4, -2);

		SetBkColor(lpdis->hDC, lpdcxcci->clrItem);

		ExtTextOut(lpdis->hDC, rcItem.left, rcItem.top, ETO_CLIPPED | ETO_OPAQUE, &rcItem, TEXT(""), NULL, nullptr);

		MoveToEx(lpdis->hDC, rcItem.left, rcItem.top, nullptr);
		LineTo(lpdis->hDC, rcItem.right, rcItem.top);

		MoveToEx(lpdis->hDC, rcItem.right, rcItem.top, nullptr);
		LineTo(lpdis->hDC, rcItem.right, rcItem.bottom);

		MoveToEx(lpdis->hDC, rcItem.right, rcItem.bottom, nullptr);
		LineTo(lpdis->hDC, rcItem.left, rcItem.bottom);

		MoveToEx(lpdis->hDC, rcItem.left, rcItem.bottom, nullptr);
		LineTo(lpdis->hDC, rcItem.left, rcItem.top);

		bParsed = TRUE;
		return TRUE;
	}
	break;
	case WM_MEASUREITEM:
	{
		dcxlParam(LPMEASUREITEMSTRUCT, lpmis);

		if (lpmis == nullptr)
			break;

		lpmis->itemHeight = 16;

		bParsed = TRUE;
		return TRUE;
	}
	break;
	}
	return 0L;
}

LRESULT DcxColorCombo::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed)
{
	switch (uMsg)
	{
	case WM_LBUTTONUP:
	{
		if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
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
