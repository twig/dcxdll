/*!
* \file dcxcomboex.cpp
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
#include "Classes/dcxcomboex.h"
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

DcxComboEx::DcxComboEx(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_COMBOEXCLASS,
		ws.m_Styles | WindowStyle::Child | CBS_AUTOHSCROLL,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsWindow(m_Hwnd))
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
	{
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");
		//SendMessage( m_Hwnd, CBEM_SETWINDOWTHEME, 0, (LPARAM)(LPCWSTR)L" "); // do this instead?
	}

	setNoThemed(ws.m_NoTheme);

	this->m_EditHwnd = getEditControl();

	if (IsWindow(this->m_EditHwnd))
	{
		if (ws.m_NoTheme)
			Dcx::UXModule.dcxSetWindowTheme(this->m_EditHwnd, L" ", L" ");

		this->m_exEdit.cHwnd = m_Hwnd;
		this->m_exEdit.pHwnd = mParentHwnd;

		//SetWindowLong( this->m_EditHwnd, GWL_STYLE, GetWindowLong( this->m_EditHwnd, GWL_STYLE ));// | ES_AUTOHSCROLL );
		this->m_exEdit.OldProc = SubclassWindow(this->m_EditHwnd, DcxComboEx::ComboExEditProc);
		SetWindowLongPtr(this->m_EditHwnd, GWLP_USERDATA, reinterpret_cast<LONG>(&this->m_exEdit));
	}

	this->m_hComboHwnd = reinterpret_cast<HWND>(SendMessage(m_Hwnd, CBEM_GETCOMBOCONTROL, 0, 0));
	if (IsWindow(this->m_hComboHwnd))
	{
		if (ws.m_NoTheme)
			Dcx::UXModule.dcxSetWindowTheme(this->m_hComboHwnd, L" ", L" ");

		COMBOBOXINFO cbi{};
		cbi.cbSize = sizeof(cbi);
		if (!GetComboBoxInfo(this->m_hComboHwnd, &cbi))
			throw Dcx::dcxException("Unable to get Combo Box Info");

		if (styles.istok(TEXT("sort")))
		{ // doesnt work atm. CBS_SORT
			if (IsWindow(cbi.hwndList))
				AddStyles(cbi.hwndList, GWL_STYLE, LBS_SORT);
		}
		if (styles.istok(TEXT("hscroll")))
		{
			if (IsWindow(cbi.hwndList))
				AddStyles(cbi.hwndList, GWL_STYLE, WS_HSCROLL);
		}
	}

	//if (p_Dialog->getToolTip() != nullptr) {
	//	if (styles.istok(TEXT("tooltips"))) {
	//		this->m_ToolTipHWND = p_Dialog->getToolTip();
	//		AddToolTipToolInfo(this->m_ToolTipHWND, m_Hwnd);
	//		AddToolTipToolInfo(this->m_ToolTipHWND, this->m_EditHwnd);
	//	}
	//}

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);

	DragAcceptFiles(m_Hwnd, TRUE);

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

DcxComboEx::~DcxComboEx() noexcept
{
	ImageList_Destroy(this->getImageList());
}

dcxWindowStyles DcxComboEx::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	//ws.m_ExStyles |= CBES_EX_NOSIZELIMIT;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"simple"_hash:
			ws.m_Styles |= CBS_SIMPLE;
			break;
		case L"dropdown"_hash:
			ws.m_Styles |= CBS_DROPDOWNLIST;
			break;
		case L"dropedit"_hash:
			ws.m_Styles |= CBS_DROPDOWN;
			break;
		case L"sort"_hash:
			ws.m_Styles |= CBS_SORT;
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

void DcxComboEx::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	const auto numtok = input.numtok();

	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP] [N]
	case L"text"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		if (const auto nItem = input.getnexttok().to_int() - 1; nItem > -1)
		{
			COMBOBOXEXITEM cbi{ CBEIF_TEXT, nItem, szReturnValue, MIRC_BUFFER_SIZE_CCH };

			getItem(&cbi);
		}
		else {
			if (nItem != -1 || (!this->isStyle(WindowStyle::CBS_DropDown) && !this->isStyle(WindowStyle::CBS_Simple)))
				throw DcxExceptions::dcxInvalidItem();

			//szReturnValue = m_tsSelected;

			if (IsWindow(m_EditHwnd))
				GetWindowText(m_EditHwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH);
		}
	}
	break;
	// [NAME] [ID] [PROP]
	case L"seltext"_hash:
	{
		const auto nItem = getCurSel();

		if (nItem < 0)
			throw DcxExceptions::dcxInvalidItem();

		COMBOBOXEXITEM cbi{ CBEIF_TEXT,nItem,szReturnValue,MIRC_BUFFER_SIZE_CCH,0,0,0,0,0 };

		getItem(&cbi);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"sel"_hash:
	{
		const auto nItem = getCurSel();

		if (nItem < 0)
			throw DcxExceptions::dcxInvalidItem();

		_ts_snprintf(szReturnValue, TEXT("%d"), nItem + 1);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"num"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), getCount());
		break;
		// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N]
	case L"find"_hash:
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		if (const auto matchtext(input.getfirsttok(2, TSTABCHAR).trim()); !matchtext.empty())
		{
			const auto params(input.getnexttok(TSTABCHAR).trim());	// tok 3
			const auto SearchType = CharToSearchType(params++[0]);

			const auto N = params++.to_<UINT>();	// tok 2

			const auto nItems = this->getCount();

			const dcxSearchData srch_data(matchtext, SearchType);

			// count total
			if (auto count = decltype(N){0}; N == 0)
			{
				for (auto i = decltype(nItems){0}; i < nItems; ++i)
				{
					if (this->matchItemText(i, srch_data))
						++count;
				}

				_ts_snprintf(szReturnValue, TEXT("%u"), count);
			}
			// find Nth matching
			else {
				for (auto i = decltype(nItems){0}; i < nItems; ++i)
				{
					if (this->matchItemText(i, srch_data))
						++count;

					if (count == N)
					{
						_ts_snprintf(szReturnValue, TEXT("%d"), i + 1);
						return;
					}
				}
			} // else
		}
	}
	break;
	// [NAME] [ID] [PROP] [ROW]
	case L"markeditem"_hash:
	{
		if (numtok != 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		COMBOBOXEXITEM cbi{ CBEIF_LPARAM, nItem };

		getItem(&cbi);

		auto mycbi = reinterpret_cast<LPDCXCBITEM>(cbi.lParam);

		if (!mycbi)
			throw Dcx::dcxException("Unable to get DCX Item");

		szReturnValue = mycbi->tsMark.to_chr();
	}
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

void DcxComboEx::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));

	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		this->resetContent();

	// xdid -a [NAME] [ID] [SWITCH] [N] [INDENT] [ICON] [STATE] [OVERLAY] Item Text
	// [NAME] [ID] -a [N] [INDENT] [ICON] [STATE] [OVERLAY] Item Text
	if (flags[TEXT('a')])
	{
		if (numtok < 8)
			throw DcxExceptions::dcxInvalidArguments();

#if TSTRING_TESTCODE
		const auto nPos = input.getnexttokas<int>() - 1;			// tok 4
		const auto indent = input.getnexttokas<int>();		// tok 5
		const auto icon = input.getnexttokas<int>() - 1;	// tok 6
		const auto state = input.getnexttokas<int>() - 1;	// tok 7
		const auto overlay = input.getnexttokas<int>();		// tok 8		(never used, here for spacing only atm)
#else
		const auto nPos = input.getnexttok().to_int() - 1;		// tok 4
		const auto indent = input.getnexttok().to_int();	// tok 5
		const auto icon = input.getnexttok().to_int() - 1;	// tok 6
		const auto state = input.getnexttok().to_int() - 1;	// tok 7
		const auto overlay = input.getnexttok().to_int();	// tok 8		(never used, here for spacing only atm)
#endif
		const auto itemtext(input.getlasttoks());			// tok 9, -1

		if (nPos < -1)
		{
			if (IsWindow(this->m_EditHwnd))
				SetWindowText(this->m_EditHwnd, itemtext.to_chr());
		}
		else {
			if (COMBOBOXEXITEM cbi{ (CBEIF_TEXT | CBEIF_INDENT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM),nPos,const_cast<TCHAR*>(itemtext.to_chr()),0,icon,state,overlay,indent, reinterpret_cast<LPARAM>(new DCXCBITEM) }; this->insertItem(&cbi) < 0)
			{
				delete reinterpret_cast<DCXCBITEM*>(cbi.lParam);
				throw Dcx::dcxException("Unable to add item.");
			}
			// Now update the horizontal scroller
			if (const auto combo = reinterpret_cast<HWND>(SendMessage(m_Hwnd, CBEM_GETCOMBOCONTROL, 0, 0)); IsWindow(combo))
			{
				// Get Font sizes (best way i can find atm, if you know something better then please let me know)

				//int nMaxStrlen = itemtext.len();
				//const int nHorizExtent = (int)SendMessage( combo, CB_GETHORIZONTALEXTENT, 0, 0 );
				//
				//HDC hdc = GetDC( m_Hwnd );
				//TEXTMETRIC tm;
				//HFONT hFont = this->getFont();
				//
				//HFONT hOldFont = SelectFont(hdc, hFont);
				//
				//GetTextMetrics(hdc, &tm);
				//
				//SelectFont(hdc, hOldFont);
				//
				//ReleaseDC( m_Hwnd, hdc);
				//
				//// Multiply max str len by font average width + 1
				//nMaxStrlen *= (tm.tmAveCharWidth + tm.tmOverhang);
				//// Add 2 * chars as spacer.
				//nMaxStrlen += (tm.tmAveCharWidth * 2);
				//
				//if (nMaxStrlen > nHorizExtent)
				//	SendMessage(combo, CB_SETHORIZONTALEXTENT, nMaxStrlen, 0);

				if (const auto hdc = GetDC(m_Hwnd); hdc)
				{
					Auto(ReleaseDC(m_Hwnd, hdc));

					const HFONT hFont = this->getFont();
					HFONT hOldFont = nullptr;

					if (hFont)
						hOldFont = Dcx::dcxSelectObject<HFONT>(hdc, hFont);

					if (SIZE sz{}; GetTextExtentPoint32(hdc, itemtext.to_chr(), gsl::narrow_cast<int>(itemtext.len()), &sz))
					{
						if (sz.cx > gsl::narrow_cast<long>(SendMessage(combo, CB_GETHORIZONTALEXTENT, 0, 0)))
							SendMessage(combo, CB_SETHORIZONTALEXTENT, gsl::narrow_cast<WPARAM>(sz.cx), 0);
					}

					if (hFont)
						Dcx::dcxSelectObject<HFONT>(hdc, hOldFont);
				}
			}
		}
	}
	// xdid -A [NAME] [ID] [ROW] [+FLAGS] [INFO]
	// [NAME] [ID] -A [ROW] [+FLAGS] [INFO]
	else if (flags[TEXT('A')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		auto nRow = input.getnexttok().to_int();	// tok 4

		// We're currently checking 1-based indexes.
		if ((nRow < 1) || (nRow > this->getCount()))
			throw Dcx::dcxException(TEXT("Invalid row index %."), nRow);

		// Convert to 0-based index.
		nRow--;

		COMBOBOXEXITEM cbei{ CBEIF_LPARAM, nRow };

		// Couldn't retrieve info
		if (!this->getItem(&cbei))
			throw Dcx::dcxException("Unable to get item.");

		auto cbiDcx = reinterpret_cast<LPDCXCBITEM>(cbei.lParam);

		if (!cbiDcx)
			throw Dcx::dcxException("Unable to get Item Info");

		const XSwitchFlags xflags(input.getnexttok());	// tok 5
		const auto info(input.getlasttoks());		// tok 6, -1

		if (!xflags[TEXT('M')])
			throw Dcx::dcxException(TEXT("Unknown flags %"), input.gettok(5));

		cbiDcx->tsMark = info;
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('c')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem > -1)
			this->setCurSel(nItem);
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

			//const auto SearchType = FlagsToSearchType(xFlags);
			//for (auto nPos = Ns.to_int(); nPos < nItems; ++nPos)
			//{
			//	if (this->matchItemText(nPos, tsMatchText, SearchType))
			//		this->deleteItem(nPos--);		// NB: we do nPos-- here as a lines just been removed so we have to check the same nPos again
			//}

			const dcxSearchData srch_data(tsMatchText, FlagsToSearchType(xFlags));
			for (auto nPos = Ns.to_int(); nPos < nItems; ++nPos)
			{
				if (this->matchItemText(nPos, srch_data))
					this->deleteItem(nPos--);		// NB: we do nPos-- here as a lines just been removed so we have to check the same nPos again
			}
		}
		else {
			{
				const auto itEnd = Ns.end();
				for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
				{
					const TString tsLine(*itStart);

					const auto [iStart, iEnd] = getItemRange(tsLine, nItems);

					if ((iStart < 0) || (iEnd < 0) || (iStart >= nItems) || (iEnd >= nItems))
						throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

					for (auto nPos = iStart; nPos <= iEnd; ++nPos)
						this->deleteItem(nPos);
				}
			}
		}

		if (!this->getCount())
			this->redrawWindow();
	}
	// xdid -l [NAME] [ID] [SWITCH] [ON|OFF]
	else if (flags[TEXT('l')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const BOOL enabled = (input.getnexttok().to_int() > 0);	// tok 4
		if (auto hEdit = this->getEditControl(); hEdit)
			SendMessage(hEdit, EM_SETREADONLY, gsl::narrow_cast<WPARAM>(enabled), 0);
	}
	// This is to avoid invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
	{
		//this->resetContent();
	}
	// xdid -u [NAME] [ID] [SWITCH]
	else if (flags[TEXT('u')])
	{
		this->setCurSel(-1);
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		auto himl = this->getImageList();

		if (!himl)
		{
			himl = this->createImageList();

			if (himl)
				this->setImageList(himl);
		}

		if (himl)
		{
			const auto flag(input++);				// tok 4
			const auto index = input++.to_<int>();	// tok 5
			auto filename(input++);					// tok 6, -1
#if DCX_USE_WRAPPERS
			const Dcx::dcxIconResource icon(index, filename, false, flag);

			ImageList_AddIcon(himl, icon.get());
#else
			if (auto icon = dcxLoadIcon(index, filename, false, flag); icon)
			{
				ImageList_AddIcon(himl, icon);
				DestroyIcon(icon);
			}
#endif
	}
}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if (flags[TEXT('y')])
	{
		ImageList_Destroy(this->getImageList());
		this->setImageList(nullptr);
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxComboEx::getImageList() const noexcept
{
	return reinterpret_cast<HIMAGELIST>(SendMessage(m_Hwnd, CBEM_GETIMAGELIST, 0U, 0));
}

/*!
* \brief blah
*
* blah
*/

void DcxComboEx::setImageList(const HIMAGELIST himl) noexcept
{
	SendMessage(m_Hwnd, CBEM_SETIMAGELIST, 0U, reinterpret_cast<LPARAM>(himl));
}

bool DcxComboEx::matchItemText(const int nItem, const TString& search, const DcxSearchTypes& SearchType) const
{
	auto itemtext = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);

	COMBOBOXEXITEM cbi{ CBEIF_TEXT, nItem, itemtext.get(), MIRC_BUFFER_SIZE_CCH };

	getItem(&cbi);

	return DcxListHelper::matchItemText(cbi.pszText, search, SearchType);
}

bool DcxComboEx::matchItemText(const int nItem, const dcxSearchData &srch_data) const
{
	auto itemtext = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);

	COMBOBOXEXITEM cbi{ CBEIF_TEXT, nItem, itemtext.get(), MIRC_BUFFER_SIZE_CCH };

	getItem(&cbi);

	return DcxListHelper::matchItemText(cbi.pszText, srch_data);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::insertItem(const PCOMBOBOXEXITEM lpcCBItem) noexcept
{
	return SendMessage(m_Hwnd, CBEM_INSERTITEM, 0U, reinterpret_cast<LPARAM>(lpcCBItem));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getItem(const PCOMBOBOXEXITEM lpcCBItem) const noexcept
{
	return SendMessage(m_Hwnd, CBEM_GETITEM, 0U, reinterpret_cast<LPARAM>(lpcCBItem));
}

/*!
* \brief blah
*
* blah
*/

HWND DcxComboEx::getEditControl() const noexcept
{
	return reinterpret_cast<HWND>(SendMessage(m_Hwnd, CBEM_GETEDITCONTROL, 0U, 0));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::deleteItem(const int iIndex) noexcept
{
	return SendMessage(m_Hwnd, CBEM_DELETEITEM, gsl::narrow_cast<WPARAM>(iIndex), 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::setCurSel(const int iIndex) noexcept
{
	return SendMessage(m_Hwnd, CB_SETCURSEL, gsl::narrow_cast<WPARAM>(iIndex), 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getCurSel() const noexcept
{
	return SendMessage(m_Hwnd, CB_GETCURSEL, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getLBText(const int iIndex, LPSTR lps) noexcept
{
	return SendMessage(m_Hwnd, CB_GETLBTEXT, gsl::narrow_cast<WPARAM>(iIndex), reinterpret_cast<LPARAM>(lps));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::resetContent() noexcept
{
	return SendMessage(m_Hwnd, CB_RESETCONTENT, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getCount() const noexcept
{
	return SendMessage(m_Hwnd, CB_GETCOUNT, 0U, 0);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::limitText(const int iLimit) noexcept
{
	return SendMessage(m_Hwnd, CB_LIMITTEXT, gsl::narrow_cast<WPARAM>(iLimit), 0);
}

const TString DcxComboEx::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, CBS_SIMPLE))
		styles.addtok(TEXT("simple"));
	if (dcx_testflag(Styles, CBS_DROPDOWNLIST))
		styles.addtok(TEXT("dropdown"));
	if (dcx_testflag(Styles, CBS_DROPDOWN))
		styles.addtok(TEXT("dropedit"));

	return styles;
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxComboEx::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (Dcx::dcxHIWORD(wParam))
		{
		case CBN_DBLCLK:
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("dclick,%u,%d"), getUserID(), getCurSel() + 1);
			bParsed = TRUE;
			return TRUE;
			break;
		case CBN_SELENDOK:
		{
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("sclick,%u,%d"), getUserID(), getCurSel() + 1);

			// Ook: This code seems completely unneeded.
			//auto itemtext = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);
			//
			//itemtext[0] = TEXT('\0');
			//
			//COMBOBOXEXITEM cbex{ CBEIF_TEXT, getCurSel(), itemtext.get(), MIRC_BUFFER_SIZE_CCH };
			//
			//getItem(&cbex);
			//
			//if (IsWindow(m_EditHwnd))
			//	SetWindowText(m_EditHwnd, itemtext.get());
			//m_tsSelected = itemtext;

			bParsed = TRUE;
			return TRUE;
		}
		break;
		case CBN_EDITCHANGE:
		{
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
				execAliasEx(TEXT("edit,%u"), getUserID());

			bParsed = TRUE;
			return TRUE;
		}
		break;
		default:
			break;
		} // switch
		bParsed = TRUE;
		break;
	case WM_NOTIFY:
		switch (dcxlParam(LPNMHDR, nhmdr); nhmdr->code)
		{
		case CBEN_ENDEDIT:
		{
			dcxlParam(LPNMCBEENDEDIT, endedit);

			if (!endedit)
				break;

			if (endedit->iWhy == CBENF_RETURN)
			{
				if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
					execAliasEx(TEXT("return,%u"), getUserID());
				bParsed = TRUE;
			}
		}
		break;
		case CBEN_DELETEITEM:
		{
			dcxlParam(PNMCOMBOBOXEX, lpcb);

			if (!lpcb)
				break;

			auto lpdcxcbi = reinterpret_cast<LPDCXCBITEM>(lpcb->ceItem.lParam);
			lpcb->ceItem.lParam = 0;

			delete lpdcxcbi;

			bParsed = TRUE; // message has been handled
		}
		break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return 0L;
}

LRESULT DcxComboEx::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_LBUTTONUP:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			execAliasEx(TEXT("lbup,%u"), getUserID());
	}
	break;
	case WM_LBUTTONDBLCLK:
		[[fallthrough]];
	case WM_CONTEXTMENU:
		break;

	case WM_MOUSEACTIVATE:
	{
		switch (Dcx::dcxHIWORD(lParam))
		{
		case WM_RBUTTONDOWN:
		{
			// NB: rclick doesnt change selection!
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
				execAliasEx(TEXT("rclick,%u,%d"), getUserID(), getCurSel() + 1);
		}
		break;
		default:
			break;
		}
		//TODO: Add `ownmenu` setting or similar to stop default edit menu & replace with own.
		// this could be done with most if not all controls.
		//bParsed = TRUE;
		//return MA_ACTIVATE;
	}
	break;

	case WM_NCDESTROY:
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

/*!
* \brief blah
*
* blah
*/

LRESULT CALLBACK DcxComboEx::ComboExEditProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpce = reinterpret_cast<LPDCXCOMBOEXEDIT>(GetWindowLongPtr(mHwnd, GWLP_USERDATA));
	if (!lpce)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	switch (uMsg)
	{
		// This message added to allow the control to return a msg when 'return' is pressed.
	case WM_GETDLGCODE:
		//return DLGC_WANTALLKEYS | CallWindowProc(lpce->OldProc, mHwnd, uMsg, wParam, lParam);
		if (wParam == VK_RETURN)
			return DLGC_WANTALLKEYS | CallWindowProc(lpce->OldProc, mHwnd, uMsg, wParam, lParam);
		break;

		//case WM_CHAR:
		//{
		//	if (wParam == VK_TAB)
		//		return DefWindowProc(mHwnd, uMsg, wParam, lParam);
			//
		//	if (wParam == VK_RETURN)
		//	{
		//		DcxControl* pthis = Dcx::dcxGetProp<DcxControl*>(lpce->cHwnd, TEXT("dcx_cthis"));
	//
		//		if (pthis)
		//		{
		//			if (dcx_testflag(pthis->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
		//				pthis->execAliasEx(TEXT("return,%u"), pthis->getUserID());
		//			return 0;
		//		}
		//	}
		//}
		//break;
		//case WM_KEYDOWN:
		//{
		//	if (wParam == VK_TAB)
		//		return DefWindowProc(mHwnd, uMsg, wParam, lParam);
	//
		//	if (wParam == VK_RETURN)
		//	{
		//		DcxControl* pthis = Dcx::dcxGetProp<DcxControl*>(lpce->cHwnd, TEXT("dcx_cthis"));
	//
		//		if (pthis)
		//		{
		//			if (dcx_testflag(pthis->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
		//				pthis->execAliasEx(TEXT("return,%u"), pthis->getUserID());
		//			return 0;
		//		}
		//	}
		//}
		//break;

		//case WM_NOTIFY:
		//	{
		//      LPNMHDR hdr = (LPNMHDR) lParam;
		//      if (!hdr)
		//        break;
	//
		//      switch( hdr->code ) {
		//		case TTN_GETDISPINFO:
		//			{
		//         DcxControl * pthis = (DcxControl *) GetProp( lpce->cHwnd, TEXT("dcx_cthis") );
		//         if ( pthis != nullptr ) {
		//					LPNMTTDISPINFO di = (LPNMTTDISPINFO)lParam;
		//					di->lpszText = TEXT("test");
		//					di->hinst = nullptr;
		//				}
		//				return 0L;
		//			}
		//			break;
		//		case TTN_LINKCLICK:
		//			{
		//         DcxControl * pthis = (DcxControl *) GetProp( lpce->cHwnd, TEXT("dcx_cthis") );
		//         if ( pthis != nullptr ) {
		//					pthis->callAliasEx( nullptr, TEXT("%s,%d"), TEXT("tooltiplink"), pthis->getUserID( ) );
		//				}
		//				return 0L;
		//			}
		//			break;
		//		}
		//	}
		//	break;

	case LB_GETITEMRECT:
	{
		// This fixes the lockup on clicking the comboex editbox. (why?)
		// this is not a real fix, but a workaround.
		// NB: when we dont replace the editbox wndproc this crash doesnt happen.
		// mIRC sends this message to it's child windows, no idea why
		return LB_ERR;
	}
	break;
	default:
		break;
	}
	return CallWindowProc(lpce->OldProc, mHwnd, uMsg, wParam, lParam);
}

void DcxComboEx::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	//const TString wtext(TGetWindowText(m_Hwnd));
	//xml->SetAttribute("caption", wtext.c_str());
	xml->SetAttribute("styles", getStyles().c_str());

	//TODO: add saving of comboex items
}

TiXmlElement* DcxComboEx::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

LRESULT DcxComboEx::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
