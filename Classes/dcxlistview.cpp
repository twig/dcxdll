/*!
* \file dcxlistview.cpp
* \brief blah
*
* blah
*
* \author David Legault ( clickhere at scriptsdb dot org )
* \version 1.1
*
* \b Revisions
*	1.1
*		Added Visual Studio 2005 specific code. Ook
*
*
* Note:
* Do not use this->getExStyle(), it will not return the correct value.
* For the listview, use ListView_GetExtendedListViewStyle() instead.
*
* © ScriptsDB.org - 2006
*/
#include "defines.h"
#include "Classes/dcxlistview.h"
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
DcxListView::DcxListView(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog, DcxControlTypes::LISTVIEW)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_LISTVIEWCLASS,
		(ws.m_Styles & ~WS_CLIPCHILDREN) | WS_CHILD, // can't be ws_clipchildren as this causes render bug when progressbar items are selected.
		//ws.m_Styles | WS_CHILD, // can't be ws_clipchildren as this causes render bug when progressbar items are selected.
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		DcxUXModule::dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	SendMessage(m_Hwnd, CCM_SETVERSION, COMCTL32_VERSION, 0);

	const auto lvExStyles = parseListviewExStyles(styles);

	Dcx::dcxListView_SetExtendedListViewStyleEx(m_Hwnd, gsl::narrow_cast<WPARAM>(lvExStyles), gsl::narrow_cast<LPARAM>(lvExStyles));

	setToolTipHWND(Dcx::dcxListView_GetToolTips(m_Hwnd));

	if (getToolTipHWND())
	{
		if (styles.istok(TEXT("balloon")))
			AddStyles(getToolTipHWND(), GWL_STYLE, TTS_BALLOON);

		//if (styles.istok(TEXT("tooltips"))) {
		//	this->m_ToolTipHWND = p_Dialog->getToolTip();
		//	AddToolTipToolInfo(this->m_ToolTipHWND,m_Hwnd);
		//}
	}

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);

	DragAcceptFiles(m_Hwnd, TRUE);
}

DcxListView::~DcxListView() noexcept
{
	if (m_Hwnd)
		Dcx::dcxListView_DeleteAllItems(m_Hwnd);

	for (const auto& a : this->m_vWidths)
		delete a;
	this->m_vWidths.clear();

	//ImageList_Destroy(getImageList(LVSIL_NORMAL));
	//ImageList_Destroy(getImageList(LVSIL_SMALL));
	//ImageList_Destroy(getImageList(LVSIL_STATE));
	//ImageList_Destroy(getImageList(LVSIL_FOOTER));
	//ImageList_Destroy(getImageList(LVSIL_GROUPHEADER));
}

const TString DcxListView::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);
	const auto ExStyles = Dcx::dcxListView_GetExtendedListViewStyle(m_Hwnd);

	if (dcx_testflag(Styles, LVS_REPORT))
		styles.addtok(TEXT("report"));
	else if (dcx_testflag(Styles, LVS_LIST))
		styles.addtok(TEXT("list"));
	if (dcx_testflag(Styles, LVS_ICON))	// LVS_ICON == zero, so never true?!?!
		styles.addtok(TEXT("icon"));
	if (dcx_testflag(Styles, LVS_SMALLICON))
		styles.addtok(TEXT("smallicon"));
	if (dcx_testflag(Styles, LVS_NOCOLUMNHEADER))
		styles.addtok(TEXT("noheader"));
	if (dcx_testflag(Styles, LVS_ALIGNLEFT))
		styles.addtok(TEXT("alignleft"));
	if (dcx_testflag(Styles, LVS_ALIGNTOP))	// same as LVS_ICON ?!?!?!
		styles.addtok(TEXT("aligntop"));
	if (dcx_testflag(Styles, LVS_AUTOARRANGE))
		styles.addtok(TEXT("autoarrange"));
	if (dcx_testflag(Styles, LVS_NOLABELWRAP))
		styles.addtok(TEXT("nolabelwrap"));
	if (dcx_testflag(Styles, LVS_SHOWSELALWAYS))
		styles.addtok(TEXT("showsel"));
	if (dcx_testflag(Styles, LVS_SINGLESEL))
		styles.addtok(TEXT("singlesel"));
	if (dcx_testflag(Styles, LVS_EDITLABELS))
		styles.addtok(TEXT("editlabel"));
	if (dcx_testflag(Styles, LVS_SORTASCENDING))
		styles.addtok(TEXT("sortasc"));
	if (dcx_testflag(Styles, LVS_SORTDESCENDING))
		styles.addtok(TEXT("sortdesc"));
	if (dcx_testflag(Styles, LVS_NOSCROLL))
		styles.addtok(TEXT("noscroll"));
	if (dcx_testflag(Styles, LVS_NOSORTHEADER))
		styles.addtok(TEXT("noheadersort"));
	if (dcx_testflag(ExStyles, LVS_EX_GRIDLINES))
		styles.addtok(TEXT("grid"));
	if (dcx_testflag(ExStyles, LVS_EX_BORDERSELECT))
		styles.addtok(TEXT("borderselect"));
	if (dcx_testflag(ExStyles, LVS_EX_FLATSB))
		styles.addtok(TEXT("flatsb"));
	if (dcx_testflag(ExStyles, LVS_EX_FULLROWSELECT))
		styles.addtok(TEXT("fullrow"));
	if (dcx_testflag(ExStyles, LVS_EX_CHECKBOXES))
		styles.addtok(TEXT("checkbox"));
	if (dcx_testflag(ExStyles, LVS_EX_HEADERDRAGDROP))
		styles.addtok(TEXT("headerdrag"));
	if (dcx_testflag(ExStyles, LVS_EX_TRACKSELECT))
		styles.addtok(TEXT("hottrack"));
	if (dcx_testflag(ExStyles, LVS_EX_ONECLICKACTIVATE))
		styles.addtok(TEXT("oneclick"));
	if (dcx_testflag(ExStyles, LVS_EX_TWOCLICKACTIVATE))
		styles.addtok(TEXT("twoclick"));
	if (dcx_testflag(ExStyles, LVS_EX_UNDERLINEHOT))
		styles.addtok(TEXT("underlinehot"));
	if (dcx_testflag(ExStyles, LVS_EX_UNDERLINECOLD))
		styles.addtok(TEXT("underlinecold"));
	if (dcx_testflag(ExStyles, LVS_EX_SUBITEMIMAGES))
		styles.addtok(TEXT("subitemimage"));
	if ((dcx_testflag(ExStyles, LVS_EX_LABELTIP)) && (dcx_testflag(ExStyles, LVS_EX_INFOTIP)))
		styles.addtok(TEXT("tooltip"));
	if (dcx_testflag(ExStyles, LVS_EX_TRANSPARENTBKGND))
		styles.addtok(TEXT("transparentbkg"));
	if (dcx_testflag(ExStyles, LVS_EX_TRANSPARENTSHADOWTEXT))
		styles.addtok(TEXT("shadowtext"));
	if (dcx_testflag(ExStyles, LVS_EX_AUTOSIZECOLUMNS))
		styles.addtok(TEXT("autosize"));
	if (dcx_testflag(ExStyles, LVS_EX_HEADERINALLVIEWS))
		styles.addtok(TEXT("headeralways"));
	if (dcx_testflag(ExStyles, LVS_EX_HIDELABELS))
		styles.addtok(TEXT("hidelabels"));
	if (dcx_testflag(ExStyles, LVS_EX_AUTOCHECKSELECT))
		styles.addtok(TEXT("autocheck"));
	if (dcx_testflag(ExStyles, LVS_EX_COLUMNSNAPPOINTS))
		styles.addtok(TEXT("columnsnap"));
	if (dcx_testflag(ExStyles, LVS_EX_JUSTIFYCOLUMNS))
		styles.addtok(TEXT("columnjustify"));
	if (dcx_testflag(ExStyles, LVS_EX_COLUMNOVERFLOW))
		styles.addtok(TEXT("columnoverflow"));
	if (dcx_testflag(ExStyles, LVS_EX_SNAPTOGRID))
		styles.addtok(TEXT("snaptogrid"));
	if (dcx_testflag(ExStyles, LVS_EX_AUTOAUTOARRANGE))
		styles.addtok(TEXT("autoautoarrange"));
	if (m_bAllowDrag)
		styles.addtok(TEXT("drag"));
	if (m_bCustomGroups)
		styles.addtok(TEXT("groups"));

	return styles;
}

dcxWindowStyles DcxListView::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	ws.m_ExStyles |= WS_EX_CLIENTEDGE;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"report"_hash:
			ws.m_Styles |= LVS_REPORT;
			break;
		case L"icon"_hash:
			ws.m_Styles |= LVS_ICON;
			break;
		case L"smallicon"_hash:
			ws.m_Styles |= LVS_SMALLICON;
			break;
		case L"list"_hash:
			ws.m_Styles |= LVS_LIST;
			break;
		case L"noheader"_hash:
			ws.m_Styles |= LVS_NOCOLUMNHEADER;
			break;
		case L"alignleft"_hash:
			ws.m_Styles |= LVS_ALIGNLEFT;
			break;
		case L"aligntop"_hash:
			ws.m_Styles |= LVS_ALIGNTOP;
			break;
		case L"autoarrage"_hash:
			ws.m_Styles |= LVS_AUTOARRANGE;
			break;
		case L"nolabelwrap"_hash:
			ws.m_Styles |= LVS_NOLABELWRAP;
			break;
		case L"showsel"_hash:
			ws.m_Styles |= LVS_SHOWSELALWAYS;
			break;
		case L"singlesel"_hash:
			ws.m_Styles |= LVS_SINGLESEL;
			break;
		case L"editlabel"_hash:
			ws.m_Styles |= LVS_EDITLABELS;
			break;
		case L"sortasc"_hash:
			ws.m_Styles |= LVS_SORTASCENDING;
			break;
		case L"sortdesc"_hash:
			ws.m_Styles |= LVS_SORTDESCENDING;
			break;
		case L"noscoll"_hash:
			ws.m_Styles |= LVS_NOSCROLL;
			break;
		case L"noheadersort"_hash:
			ws.m_Styles |= LVS_NOSORTHEADER;
			break;
		case L"drag"_hash:
			m_bAllowDrag = true;
			break;
		case L"groups"_hash:
			m_bCustomGroups = true;
			break;
		case L"subitemsel"_hash:
			m_bSubItemSelect = true;
			break;
		default:
			break;
		}
	}

	return parseGeneralControlStyles(tsStyles, ws);
}

const WindowExStyle DcxListView::parseListviewExStyles(const TString& styles) noexcept
{
	WindowExStyle ExStyles(WindowExStyle::None);

	ExStyles |= LVS_EX_DOUBLEBUFFER;

	try {
		for (const auto& tsStyle : styles)
		{
			switch (std::hash<TString>{}(tsStyle))
			{
			case L"grid"_hash:
				ExStyles |= LVS_EX_GRIDLINES;
				break;
			case L"borderselect"_hash:
				ExStyles |= LVS_EX_BORDERSELECT;
				break;
			case L"flatsb"_hash:
				ExStyles |= LVS_EX_FLATSB;
				break;
			case L"fullrow"_hash:
				ExStyles |= LVS_EX_FULLROWSELECT;
				break;
			case L"checkbox"_hash:
				ExStyles |= LVS_EX_CHECKBOXES;
				break;
			case L"headerdrag"_hash:
				ExStyles |= LVS_EX_HEADERDRAGDROP;
				break;
			case L"hottrack"_hash:
				ExStyles |= LVS_EX_TRACKSELECT;
				break;
			case L"oneclick"_hash:
				ExStyles |= LVS_EX_ONECLICKACTIVATE;
				break;
			case L"twoclick"_hash:
				ExStyles |= LVS_EX_TWOCLICKACTIVATE;
				break;
			case L"underlinehot"_hash:
				ExStyles |= LVS_EX_UNDERLINEHOT;
				break;
			case L"underlinecold"_hash:
				ExStyles |= LVS_EX_UNDERLINECOLD;
				break;
			case L"subitemimage"_hash:
				ExStyles |= LVS_EX_SUBITEMIMAGES;
				break;
			case L"tooltip"_hash:
				ExStyles |= LVS_EX_LABELTIP | LVS_EX_INFOTIP;
				break;
			case L"transparentbkg"_hash:
				ExStyles |= LVS_EX_TRANSPARENTBKGND;
				break;
			case L"shadowtext"_hash:
				ExStyles |= LVS_EX_TRANSPARENTSHADOWTEXT;
				break;
			case L"autosize"_hash:
				ExStyles |= LVS_EX_AUTOSIZECOLUMNS;
				break;
			case L"headeralways"_hash:
				ExStyles |= LVS_EX_HEADERINALLVIEWS;
				break;
			case L"hidelabels"_hash:
				ExStyles |= LVS_EX_HIDELABELS;
				break;
			case L"autocheck"_hash:
				ExStyles |= LVS_EX_AUTOCHECKSELECT;
				break;
			case L"columnsnap"_hash:
				ExStyles |= LVS_EX_COLUMNSNAPPOINTS;
				break;
			case L"columnjustify"_hash:
				ExStyles |= LVS_EX_JUSTIFYCOLUMNS;
				break;
			case L"columnoverflow"_hash:
				ExStyles |= LVS_EX_COLUMNOVERFLOW;
				//LVN_COLUMNOVERFLOWCLICK
				// needs LVS_EX_HEADERINALLVIEWS
				break;
			case L"snaptogrid"_hash:
				ExStyles |= LVS_EX_SNAPTOGRID;
				break;
			case L"autoautoarrange"_hash:
				ExStyles |= LVS_EX_AUTOAUTOARRANGE;
				break;
			default:
				break;
			}
		}
	}
	catch (...) {}

	return ExStyles;
}

/*!
* \brief $xdid Parsing Function
*
* \param input [NAME] [ID] [PROP] (OPTIONS)
* \param szReturnValue mIRC Data Container
*
* \return > void
*/
void DcxListView::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	const auto numtok = input.numtok();

	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP] [N] [NSUB]
	case L"columns"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), this->getColumnCount());
		break;
		// [NAME] [ID] [PROP] (N)
	case L"columnorder"_hash:
	{
		//// if its a report listview and it has headers
		//const auto& count = this->getColumnCount();
		//const auto col = (numtok > 3 ? input++.to_int() - 1 : -1);	// tok 4
		//
		//// invalid column
		//if ((col < -1) || (col >= count) || (count <= 0))
		//	throw DcxExceptions::dcxOutOfRange();
		//
		//auto val = std::make_unique<int[]>(gsl::narrow_cast<size_t>(count));
		//
		//Dcx::dcxListView_GetColumnOrderArray(m_Hwnd, count, val.get());
		//
		//// increase each value by 1 for easy user indexing
		//for (auto i = decltype(count){0}; i < count; ++i)
		//	gsl::at(val, gsl::narrow_cast<size_t>(i))++;
		//
		//// get specific column
		//if (col > -1)
		//{
		//	_ts_snprintf(szReturnValue, TEXT("%d"), gsl::at(val, gsl::narrow_cast<size_t>(col)));
		//	return;
		//}
		//
		//// collect all values
		//TString buff(gsl::narrow_cast<TString::size_type>(count * 32));
		//
		//for (auto i = decltype(count){0}; i < count; ++i)
		//	buff.addtok(gsl::at(val, gsl::narrow_cast<size_t>(i)));
		//
		//szReturnValue = buff.trim().to_chr();

		// if its a report listview and it has headers
		const auto& count = this->getColumnCount();
		const auto col = (numtok > 3 ? input++.to_int() - 1 : -1);	// tok 4

		// invalid column
		if ((col < -1) || (col >= count) || (count <= 0))
			throw DcxExceptions::dcxOutOfRange();

		VectorOfInts val(gsl::narrow_cast<size_t>(count));

		Dcx::dcxListView_GetColumnOrderArray(m_Hwnd, count, val.data());

		// increase each value by 1 for easy user indexing
		for (auto& v : val)
			++v;

		// get specific column
		if (col > -1)
		{
			_ts_snprintf(szReturnValue, TEXT("%d"), gsl::at(val, gsl::narrow_cast<size_t>(col)));
			return;
		}

		// collect all values
		TString buff(gsl::narrow_cast<TString::size_type>(count * 32));

		for (const auto& v : val)
			buff.addtok(v);

		szReturnValue = buff.trim().to_chr();
	}
	break;
	// [NAME] [ID] [PROP] [N] (NSUB)
	case L"text"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input++.to_int() - 1;		// tok 4
		auto nSubItem = 0;

		if (numtok > 4)
			nSubItem = input++.to_int() - 1;			// tok 5
		if ((nItem < 0) || (nSubItem < 0) || (nItem >= Dcx::dcxListView_GetItemCount(m_Hwnd)))
			throw DcxExceptions::dcxOutOfRange();

		Dcx::dcxListView_GetItemText(m_Hwnd, nItem, nSubItem, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	break;
	// [NAME] [ID] [PROP] [N] (NSUB)
	case L"textlen"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input++.to_int() - 1;		// tok 4
		auto nSubItem = 0;

		if (numtok > 4)
			nSubItem = input++.to_int() - 1;			// tok 5
		if ((nItem < 0) || (nSubItem < 0) || (nItem >= Dcx::dcxListView_GetItemCount(m_Hwnd)))
			throw DcxExceptions::dcxOutOfRange();

		const auto len = Dcx::dcxListView_GetItemTextLength(m_Hwnd, nItem, nSubItem);
		_ts_snprintf(szReturnValue, TEXT("%zu"), len);
	}
	break;
	// [NAME] [ID] [PROP] [N] [NSUB]
	case L"icon"_hash:
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input++.to_int() - 1;	// tok 4
		const auto nSubItem = input++.to_int() - 1;	// tok 5

		if ((nItem < 0) || (nSubItem < 0) || (nItem >= Dcx::dcxListView_GetItemCount(m_Hwnd)) || (nSubItem >= this->getColumnCount()))
			throw DcxExceptions::dcxOutOfRange();

		LVITEM lvi{};
		lvi.mask = LVIF_IMAGE;
		lvi.iItem = nItem;
		lvi.iSubItem = nSubItem;

		Dcx::dcxListView_GetItem(m_Hwnd, &lvi);
		_ts_snprintf(szReturnValue, TEXT("%d"), lvi.iImage + 1);
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"selected"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input++.to_int() - 1;	// tok 4
		// In range
		if ((nItem < 0) || (nItem >= Dcx::dcxListView_GetItemCount(m_Hwnd)))
			throw DcxExceptions::dcxOutOfRange();

		szReturnValue = dcx_truefalse(dcx_testflag(Dcx::dcxListView_GetItemState(m_Hwnd, nItem, LVIS_SELECTED), LVIS_SELECTED));
	}
	break;
	// [NAME] [ID] [PROP] (NSUB)
	case L"seltext"_hash:
	{
		auto nSubItem = 0;

		if (numtok > 3)
			nSubItem = input++.to_int() - 1;	// tok 4

		if (const auto nItem = Dcx::dcxListView_GetNextItem(m_Hwnd, -1, LVIS_SELECTED); ((nItem > -1) && (nSubItem > -1)))
			Dcx::dcxListView_GetItemText(m_Hwnd, nItem, nSubItem, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	break;
	// [NAME] [ID] [PROP] (N)
	case L"sel"_hash:
	{
		if (this->isStyle(WindowStyle::LVS_SingleSelect))
		{
			const auto nItem = Dcx::dcxListView_GetNextItem(m_Hwnd, -1, LVIS_SELECTED);

			if (nItem > -1)
				_ts_snprintf(szReturnValue, TEXT("%d"), nItem + 1);
		}
		// multi select
		else {
			const auto nSelItems = Dcx::dcxListView_GetSelectedCount(m_Hwnd);
			auto nItem = -1;

			// if we want a specific index
			if (numtok > 3)
			{
				const auto n = input++.to_<UINT>();	// tok 4
				// sel index out of bounds
				if (n > nSelItems)
					return;

				// return total count of selected items
				if (n == 0)
				{
					_ts_snprintf(szReturnValue, TEXT("%u"), nSelItems);
					return;
				}

				for (auto i = decltype(n){1}; i <= n; ++i)	// find the Nth selected item.
					nItem = Dcx::dcxListView_GetNextItem(m_Hwnd, nItem, LVIS_SELECTED);

				if (nItem != -1)
					_ts_snprintf(szReturnValue, TEXT("%d"), nItem + 1);
			}

			// otherwise we want a list of indexes (comma seperated)
			else if (nSelItems > 0)
			{
				TString list;

				while ((nItem = Dcx::dcxListView_GetNextItem(m_Hwnd, nItem, LVIS_SELECTED)) != -1)
					list.addtok((nItem + 1), TSCOMMACHAR);

				szReturnValue = list.to_chr();
			}
		}
	}
	break;
	// [NAME] [ID] [PROP]
	case L"selnum"_hash:
		_ts_snprintf(szReturnValue, TEXT("%u"), Dcx::dcxListView_GetSelectedCount(m_Hwnd));
		break;
		// [NAME] [ID] [PROP] [N]
	case L"state"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input++.to_int() - 1;	// tok 4
		if ((nItem < 0) || (nItem >= Dcx::dcxListView_GetItemCount(m_Hwnd)))
			throw DcxExceptions::dcxOutOfRange();

		if (this->isListViewStyle(to_WindowStyle(LVS_REPORT)))
		{
			const auto state = Dcx::dcxListView_GetItemState(m_Hwnd, nItem, LVIS_STATEIMAGEMASK);

			if (state == 8192)
				szReturnValue = TEXT('2');
			else
			{
				if (state == 4096)
					szReturnValue = TEXT('1');
				else
					szReturnValue = TEXT('0');
			}
		}
		else
			_ts_snprintf(szReturnValue, TEXT("%u"), Dcx::dcxListView_GetItemState(m_Hwnd, nItem, LVIS_STATEIMAGEMASK));
	}
	break;
	// [NAME] [ID] [PROP]
	case L"num"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), Dcx::dcxListView_GetItemCount(m_Hwnd));
		break;
		// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [COLUMN] [N]
	case L"find"_hash:
	{
		if (numtok < 7)
			throw DcxExceptions::dcxInvalidArguments();

		if (const auto matchtext(input.getfirsttok(2, TSTABCHAR).trim()); !matchtext.empty())
		{
			const auto params(input.getnexttok(TSTABCHAR).trim());			// tok 3

			if (params.numtok() != 3)
				throw DcxExceptions::dcxInvalidArguments();

			const auto SearchType = CharToSearchType(params++[0]);
			const auto nColumn = params++.to_int() - 1;	// tok 2
			const auto N = params++.to_int();			// tok 3
			//const auto bCallback = (params++.to_int() > 0);

			const auto nItems = Dcx::dcxListView_GetItemCount(m_Hwnd);
			const auto nColumns = this->getColumnCount();
			auto count = decltype(N){0};

			const dcxSearchData srch_data(matchtext, SearchType);

			// count total
			if (N == 0)
			{
				// Search all columns
				if (nColumn == -1)
				{
					for (auto i = decltype(nItems){0}; i < nItems; ++i)
					{
						for (auto k = decltype(nColumns){0}; k < nColumns; ++k)
						{
							if (this->matchItemText(i, k, srch_data))
								++count;
						}
					}
				}
				// Particular Column
				else {
					if (nColumn < -1 || nColumn >= nColumns)
						throw DcxExceptions::dcxOutOfRange();

					for (auto i = decltype(nItems){0}; i < nItems; ++i)
					{
						if (this->matchItemText(i, nColumn, srch_data))
							++count;
					}
				}

				_ts_snprintf(szReturnValue, TEXT("%d"), count);
			} // if ( N == 0 )
			  // find Nth matching
			else {
				// Search all columns
				if (nColumn == -1)
				{
					for (auto i = decltype(nItems){0}; i < nItems; ++i)
					{
						for (auto k = decltype(nColumns){0}; k < nColumns; ++k)
						{
							if (this->matchItemText(i, k, srch_data))
								++count;

							// found Nth matching
							if (count == N)
							{
								_ts_snprintf(szReturnValue, TEXT("%d %d"), i + 1, k + 1);
								return;
							}
						}
					}
				}
				// Particular Column
				else {

					if (nColumn < -1 || nColumn >= nColumns)
						throw DcxExceptions::dcxOutOfRange();

					for (auto i = decltype(nItems){0}; i < nItems; ++i)
					{
						if (this->matchItemText(i, nColumn, srch_data))
							++count;

						// found Nth matching
						if (count == N)
						{
							_ts_snprintf(szReturnValue, TEXT("%d %d"), i + 1, nColumn + 1);
							return;
						}
					}
				} //else
			} // else
		} // if ( !matchtext.empty() )
		else
			throw Dcx::dcxException("Match Text Missing");	// Ook: allow searching for empty items???
	}
	break;
	// [NAME] [ID] [PROP]
	case L"tbitem"_hash:
	{
		// Only gives usefull results when in 'report' or 'list' views & groups are disabled.
		// 
		//if (this->isStyle(LVS_REPORT) || this->isStyle(LVS_LIST))	// applies to all views??
		_ts_snprintf(szReturnValue, TEXT("%d %d"), this->getTopIndex() + 1, this->getBottomIndex() + 1);
	}
	break;
	case L"mouseitem"_hash:
	{
		if (LVHITTESTINFO lvh{}; GetCursorPos(&lvh.pt))
		{
			MapWindowPoints(nullptr, m_Hwnd, &lvh.pt, 1);
			Dcx::dcxListView_SubItemHitTest(m_Hwnd, &lvh);

			if ((lvh.flags & LVHT_ONITEM) != 0)
				_ts_snprintf(szReturnValue, TEXT("%d %d"), lvh.iItem + 1, lvh.iSubItem + 1);
			else
				szReturnValue = TEXT("-1 -1");
		}
	}
	break;
	// [NAME] [ID] [PROP] (N)
	case L"hwidth"_hash:
	{
		auto nColumn = -1;

		if (numtok > 3)
			nColumn = input.getnexttokas<int>() - 1;	// tok 4

		const auto count = this->getColumnCount();

		// return all columns
		if (nColumn == -1)
		{
			TString buff(gsl::narrow_cast<TString::size_type>(count) * 32);

			for (auto i = decltype(count){0}; i < count; i++)
				buff.addtok(Dcx::dcxListView_GetColumnWidth(m_Hwnd, i));

			szReturnValue = buff.trim().to_chr();
		}
		else {
			if (nColumn < 0 || nColumn >= count)
				throw DcxExceptions::dcxOutOfRange();

			_ts_snprintf(szReturnValue, TEXT("%d"), Dcx::dcxListView_GetColumnWidth(m_Hwnd, nColumn));
		}
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"htext"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nColumn = input.getnexttokas<int>() - 1;	// tok 4

		if (nColumn < 0 || nColumn >= this->getColumnCount())
			throw DcxExceptions::dcxOutOfRange();

		LVCOLUMN lvc{};
		lvc.mask = LVCF_TEXT;
		lvc.cchTextMax = gsl::narrow_cast<int>(szReturnValue.size());
		lvc.pszText = szReturnValue;

		Dcx::dcxListView_GetColumn(m_Hwnd, nColumn, &lvc);
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"hicon"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nColumn = input.getnexttokas<int>() - 1;	// tok 4

		if (nColumn < 0 || nColumn >= this->getColumnCount())
			throw DcxExceptions::dcxOutOfRange();

		LVCOLUMN lvc{};
		lvc.mask = LVCF_IMAGE;

		if (Dcx::dcxListView_GetColumn(m_Hwnd, nColumn, &lvc))
			_ts_snprintf(szReturnValue, TEXT("%d"), lvc.iImage + 1);
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"hstate"_hash:
	{
		if (numtok != 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nCol = (input.getnexttokas<int>() - 1);	// tok 4

		if (nCol < 0 || nCol >= this->getColumnCount())
			throw DcxExceptions::dcxOutOfRange();

		HDITEM hdr{};
		hdr.mask = HDI_FORMAT;

		if (const auto h = Dcx::dcxListView_GetHeader(m_Hwnd); !IsWindow(h))
			throw Dcx::dcxException("Unable to get Header Window");
		else if (!Dcx::dcxHeader_GetItem(h, nCol, &hdr))
			throw Dcx::dcxException("Unable to get Header Info");

		TString tsRes;

		if (dcx_testflag(hdr.fmt, HDF_SORTDOWN))
			tsRes.addtok(TEXT("sortdown"));
		if (dcx_testflag(hdr.fmt, HDF_SORTUP))
			tsRes.addtok(TEXT("sortup"));
		if (dcx_testflag(hdr.fmt, HDF_CHECKBOX))
			tsRes.addtok(TEXT("checkbox"));
		if (dcx_testflag(hdr.fmt, HDF_CHECKED))
			tsRes.addtok(TEXT("checked"));
		if (dcx_testflag(hdr.fmt, HDF_SPLITBUTTON))
			tsRes.addtok(TEXT("dropdown"));

		szReturnValue = tsRes.to_chr();
	}
	break;
	// [NAME] [ID] [PROP] [GID]
	case L"gtext"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto GID = input.getnexttokas<UINT>();	// tok 4

		auto wstr = std::make_unique<WCHAR[]>(MIRC_BUFFER_SIZE_CCH + 1);
		gsl::at(wstr, 0) = TEXT('\0');

		if (LVGROUP lvg{ sizeof(LVGROUP), LVGF_HEADER, wstr.get(), MIRC_BUFFER_SIZE_CCH }; Dcx::dcxListView_GetGroupInfo(m_Hwnd, GID, &lvg) != -1)
			szReturnValue = lvg.pszHeader;
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"genabled"_hash:
		szReturnValue = dcx_truefalse(Dcx::dcxListView_IsGroupViewEnabled(m_Hwnd) != FALSE);
		break;
		// [NAME] [ID] [PROP] [N] [NSUB] [PBARPROP] [PARAM]
	case L"pbar"_hash:
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttokas<int>() - 1;	// tok 4
		const auto nSubItem = input.getnexttokas<int>() - 1;	// tok 5

		if (nItem < 0 || nSubItem < 0 || nItem >= Dcx::dcxListView_GetItemCount(m_Hwnd))
			throw DcxExceptions::dcxOutOfRange();

		LVITEM lvi{ LVIF_PARAM, nItem };

		if (!Dcx::dcxListView_GetItem(m_Hwnd, &lvi))
			throw Dcx::dcxException("Unable to get Item");

		const auto lvdcx = reinterpret_cast<LPDCXLVITEM>(lvi.lParam);

		if (!lvdcx || !lvdcx->pbar || lvdcx->iPbarCol != nSubItem)
			throw Dcx::dcxException("No Progessbar Here");

		//const TString cmd(input.gettok( 1 ) + TEXT(" ") + input.gettok( 2 ) + TEXT(" ") + input.getlasttoks());	// tok 6, -1
		//const TString cmd(this->getParentDialog()->getName() + TEXT(" ") + input.gettok(2) + TEXT(" ") + input.getlasttoks());	// tok 6, -1
		//const TString cmd{ this->getParentDialog()->getName(), TEXT(" "_ts), input.gettok(2), TEXT(" "_ts), input.getlasttoks() };	// tok 6, -1
		TString cmd(this->getParentDialog()->getName());
		cmd.addtok(input.gettok(2));		// tok 2
		cmd.addtok(input.getlasttoks());	// tok 6, -1

		lvdcx->pbar->parseInfoRequest(cmd, szReturnValue);
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"gnum"_hash:
	{
		if (Dcx::VersInfo.isVista())
			_ts_snprintf(szReturnValue, TEXT("%d"), Dcx::dcxListView_GetGroupCount(m_Hwnd));
		else {
			auto gcount = 0U;
			for (auto g = 0U; g < 256U; g++)
				if (Dcx::dcxListView_HasGroup(m_Hwnd, gsl::narrow_cast<int>(g)))
					++gcount;

			_ts_snprintf(szReturnValue, TEXT("%u"), gcount);
		}
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"gid"_hash:
	{
		const auto iIndex = input.getnexttokas<int>() - 1;	// tok 4

		if (iIndex < 0 || iIndex >= Dcx::dcxListView_GetItemCount(m_Hwnd))
			throw Dcx::dcxException(TEXT("Invalid Item: %"), iIndex);

		LVITEM lvi{ LVIF_GROUPID,iIndex };

		if (!Dcx::dcxListView_GetItem(m_Hwnd, &lvi))
			throw Dcx::dcxException("Unable to get Group ID");

		_ts_snprintf(szReturnValue, TEXT("%d"), lvi.iGroupId); // group id can be -2 (Not In group), -1 (groupcallback, should never be), 0+ groupid
	}
	break;
	// [NAME] [ID] [PROP] [ROW] [COL]
	case L"markeditem"_hash:
	{
		auto nRow = input.getnexttokas<int>();	// tok 4
		auto nCol = input.getnexttokas<int>();	// tok 5

		// 1-based indexes.
		if ((nRow < 1) || (nRow > Dcx::dcxListView_GetItemCount(m_Hwnd)))
			throw Dcx::dcxException(TEXT("Invalid item index %"), nRow);

		if ((nCol < 1) || (nCol > this->getColumnCount()))
			throw Dcx::dcxException(TEXT("Invalid column index %"), nCol);

		// Convert to 0-index
		nRow--;
		nCol--;

		LVITEM lvi{};
		lvi.mask = LVIF_PARAM;
		lvi.iItem = nRow;
		lvi.iSubItem = nCol;

		if (!Dcx::dcxListView_GetItem(m_Hwnd, &lvi))
			throw Dcx::dcxException(TEXT("Unable to get item: % %"), nRow, nCol);

		szReturnValue = (reinterpret_cast<LPDCXLVITEM>(lvi.lParam))->tsMark.to_chr();
	}
	break;
	// [NAME] [ID] [PROP]
	case L"emptytext"_hash:
	{
		//if (Dcx::VistaModule.isUseable())
		//	Dcx::dcxListView_GetEmptyText(m_Hwnd, szReturnValue.data(), MIRC_BUFFER_SIZE_CCH);

		szReturnValue = TGetWindowText(m_Hwnd).to_chr();
	}
	break;
	// [NAME] [ID] [PROP] [GID]
	case L"gstate"_hash:
	{
		if (numtok != 4)
			throw DcxExceptions::dcxInvalidArguments();

		TString tsFlags('+');

		if (Dcx::VersInfo.isVista())
		{
			const auto gid = input.getnexttokas<int>();	// tok 4
			constexpr UINT iMask = LVGS_COLLAPSIBLE | LVGS_HIDDEN | LVGS_NOHEADER | LVGS_COLLAPSED | LVGS_SELECTED;

			const auto iState = Dcx::dcxListView_GetGroupState(m_Hwnd, gid, iMask);

			if (dcx_testflag(iState, LVGS_COLLAPSIBLE))
				tsFlags += TEXT('C');
			if (dcx_testflag(iState, LVGS_HIDDEN))
				tsFlags += TEXT('H');
			if (dcx_testflag(iState, LVGS_NOHEADER))
				tsFlags += TEXT('N');
			if (dcx_testflag(iState, LVGS_COLLAPSED))
				tsFlags += TEXT('O');
			if (dcx_testflag(iState, LVGS_SELECTED))
				tsFlags += TEXT('S');
		}
		szReturnValue = tsFlags.to_chr();
	}
	break;
	// [NAME] [ID] [PROP] [flags]
	case L"icons"_hash:
	{
		if (numtok != 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iFlags = parseIconFlagOptions(input.getnexttok());	// tok 4

		UINT iCount{};
		if (dcx_testflag(iFlags, LVSIL_SMALL))
		{
			if (auto himl = getImageList(LVSIL_NORMAL); himl)
				iCount += ImageList_GetImageCount(himl);
		}
		if (dcx_testflag(iFlags, LVSIL_STATE))
		{
			if (auto himl = getImageList(LVSIL_STATE); himl)
				iCount += ImageList_GetImageCount(himl);
		}
		if (dcx_testflag(iFlags, LVSIL_FOOTER))
		{
			if (auto himl = getImageList(LVSIL_FOOTER); himl)
				iCount += ImageList_GetImageCount(himl);
		}
		_ts_snprintf(szReturnValue, TEXT("%u"), iCount);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"margin"_hash:
	{
		RECT rc{};
		if (Dcx::dcxListView_GetViewMargin(m_Hwnd, &rc))
			_ts_snprintf(szReturnValue, TEXT("%d %d %d %d"), rc.left, rc.right, rc.top, rc.bottom);
		else
			szReturnValue = TEXT("-1 -1 -1 -1");
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
}

void DcxListView::autoSize(const int nColumn, const TString& flags)
{
	this->autoSize(nColumn, this->parseHeaderFlags2(flags));
}

void DcxListView::autoSize(const int nColumn, const int iFlags, const int iWidth) noexcept
{
	if (!m_Hwnd)
		return;

	if (dcx_testflag(iFlags, DCX_LV_COLUMNF_FIXED))
		Dcx::dcxListView_SetColumnWidth(m_Hwnd, nColumn, iWidth);
	else if (dcx_testflag(iFlags, DCX_LV_COLUMNF_AUTOMAX))
	{
		Dcx::dcxListView_SetColumnWidth(m_Hwnd, nColumn, LVSCW_AUTOSIZE);
		auto n = Dcx::dcxListView_GetColumnWidth(m_Hwnd, nColumn);
		Dcx::dcxListView_SetColumnWidth(m_Hwnd, nColumn, LVSCW_AUTOSIZE_USEHEADER);
		n = std::max(Dcx::dcxListView_GetColumnWidth(m_Hwnd, nColumn), n);
		Dcx::dcxListView_SetColumnWidth(m_Hwnd, nColumn, n);
	}
	else if (dcx_testflag(iFlags, DCX_LV_COLUMNF_AUTO))
		Dcx::dcxListView_SetColumnWidth(m_Hwnd, nColumn, LVSCW_AUTOSIZE);
	else if (dcx_testflag(iFlags, DCX_LV_COLUMNF_AUTOHEADER))
		Dcx::dcxListView_SetColumnWidth(m_Hwnd, nColumn, LVSCW_AUTOSIZE_USEHEADER);
	//else if (dcx_testflag(iFlags, DCX_LV_COLUMNF_PERCENT))
	//	Dcx::dcxListView_SetColumnWidth(m_Hwnd, nColumn, 0);
}

void DcxListView::HandleDragDrop(int x, int y) noexcept
{
	if (!m_Hwnd)
		return;

	Dcx::dcxListView_ClearInsertMark(m_Hwnd);

	// Determine the dropped item
	const int iItem = Dcx::dcxListView_GetItemAtPos(m_Hwnd, x, y);
	// Out of the ListView?
	// Not in an item?
	if (iItem == -1)
		return;

	// Dropped item is selected?
	//if (Dcx::dcxListView_GetItemState(m_Hwnd, iItem, LVIS_SELECTED) == LVIS_SELECTED)
	//	return;

	// Rearrange the items
	for (int iPos = Dcx::dcxListView_GetNextItem(m_Hwnd, -1, LVNI_SELECTED); (iPos != -1); iPos = Dcx::dcxListView_GetNextItem(m_Hwnd, -1, LVNI_SELECTED))
		this->MoveItem(iPos, iItem, true);

	//if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_DRAG))
	//	execAliasEx(TEXT("enddrag,%u"), getUserID()); // allow blocking the drag?
}

void DcxListView::HandleDragMove(int x, int y) noexcept
{
	if (!m_Hwnd)
		return;

	// Determine the dropped item
	const int iItem = Dcx::dcxListView_GetItemAtPos(m_Hwnd, x, y);
	if (iItem == -1)
		return;

	LVINSERTMARK lvim{};
	lvim.cbSize = sizeof(LVINSERTMARK);
	lvim.iItem = iItem;

	Dcx::dcxListView_SetInsertMark(m_Hwnd, &lvim);
}

void DcxListView::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));

	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		Dcx::dcxListView_DeleteAllItems(m_Hwnd);

	//xdid -a [NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
	//xdid -a -> [NAME] [ID] -a [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
	if (flags[TEXT('a')])
	{
		Command_a(input);
	}
	// xdid -A [NAME] [ID] [SWITCH] [ROW] [COL] [+FLAGS] [INFO]
	else if (flags[TEXT('A')])
	{
		static_assert(CheckFreeCommand(TEXT('A')), "Command in use!");

		if (numtok < 7)
			throw DcxExceptions::dcxInvalidArguments();

		auto nRow = input++.to_int();	// tok 4
		auto nCol = input++.to_int();	// tok 5

		// We're currently checking 1-based indexes.
		if ((nRow < 1) || (nRow > Dcx::dcxListView_GetItemCount(m_Hwnd)))
			throw Dcx::dcxException(TEXT("Invalid row index %."), nRow);

		if ((nCol < 1) || (nCol > this->getColumnCount()))
			throw Dcx::dcxException(TEXT("Invalid column index %."), nCol);

		// Convert to 0-based index.
		nRow--;
		nCol--;

		//LVITEM lvi{ LVIF_PARAM, nRow, nCol, 0U, 0U, nullptr, 0, 0, 0U, 0, 0, 0U, nullptr, nullptr, 0 };
		LVITEM lvi{ LVIF_PARAM, nRow, nCol };

		// Couldnt retrieve info
		if (!Dcx::dcxListView_GetItem(m_Hwnd, &lvi))
			throw Dcx::dcxException("Unable to get item.");

		const auto lviDcx = reinterpret_cast<LPDCXLVITEM>(lvi.lParam);

		if (!lviDcx)
			throw Dcx::dcxException("Unable to Retrieve Item Data");

		const XSwitchFlags xflag(input++);	// tok 6
		const auto info(input++);		// tok 7, -1

		if (!xflag[TEXT('+')])
			throw DcxExceptions::dcxInvalidFlag();

		if (!xflag[TEXT('M')])	// mark info
			throw Dcx::dcxException(TEXT("Unknown flags %"), input.gettok(6));

		lviDcx->tsMark = info;
	}
	// xdid -B [NAME] [ID] [N]
	// xdid -B -> [NAME] [ID] -B [N]
	else if (flags[TEXT('B')])
	{
		static_assert(CheckFreeCommand(TEXT('B')), "Command in use!");

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = StringToItemNumber(input.getnexttok());	// tok 4

		if (nItem < 0)
			throw DcxExceptions::dcxInvalidItem();

		if (GetFocus() != m_Hwnd)
			SetFocus(m_Hwnd);

		Dcx::dcxListView_EditLabel(m_Hwnd, nItem);
	}
	// xdid -c [NAME] [ID] [N,N2,N3-N4...]
	// xdid -c -> [NAME] [ID] -c [N,N2,N3-N4...]
	else if (flags[TEXT('c')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItemCnt = Dcx::dcxListView_GetItemCount(m_Hwnd);
		if (nItemCnt < 1)
			throw Dcx::dcxException("Invalid Item: No Items in list");

		if (this->isStyle(WindowStyle::LVS_SingleSelect))
		{
			auto nItem = input.getnexttokas<int>() - 1;	// tok 4

			if (nItem == -1)
				nItem = nItemCnt - 1;

			if (nItem > -1)
				Dcx::dcxListView_SetItemState(m_Hwnd, nItem, LVIS_SELECTED, LVIS_SELECTED);
		}
		else {
			{
				const auto Ns(input.getnexttok());	// tok 4
				const auto itEnd = Ns.end();
				for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
				{
					const auto tsLine(*itStart);
					const auto r = Dcx::getItemRange2(tsLine, nItemCnt);

					if ((r.b < 0) || (r.e < 0) || (r.b > r.e))
						throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

					for (const auto nItem : r)
						Dcx::dcxListView_SetItemState(m_Hwnd, nItem, LVIS_SELECTED, LVIS_SELECTED);
				}
			}
		}
	}
	// xdid -d [NAME] [ID] [SWITCH] [N,N2,N3-N4...]
	// xdid -d [NAME] [ID] -d [N,N2,N3-N4...]
	// xdid -d [NAME] [ID] -d [N] [+flags] [subitem] [match text]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		auto Ns(input.getnexttok());	// tok 4
		const XSwitchFlags xFlags(input.getnexttok());	// tok 5

		if (xFlags[TEXT('+')])
		{
			// have flags, so its a match text delete
			const auto nSubItem = input.getnexttokas<int>();

			const dcxSearchData srch_data(input.getnexttok(), FlagsToSearchType(xFlags));

			// NB: item count changes on every delete.
			for (auto nPos = Ns.to_int(); nPos < Dcx::dcxListView_GetItemCount(m_Hwnd); ++nPos)
			{
				if (this->matchItemText(nPos, nSubItem, srch_data))
					Dcx::dcxListView_DeleteItem(m_Hwnd, nPos--);		// NB: we do nPos-- here as a lines just been removed so we have to check the same nPos again
			}
		}
		else {
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
				const auto nItemCnt = Dcx::dcxListView_GetItemCount(m_Hwnd);

				const auto [iStart, iEnd] = Dcx::getItemRange(tsLine, nItemCnt);

				if ((iStart < 0) || (iEnd < iStart) || (iStart >= nItemCnt) || (iEnd >= nItemCnt))
					throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

				// delete from highest number to lowest
				for (auto nItem = iEnd; nItem >= iStart; --nItem)
					Dcx::dcxListView_DeleteItem(m_Hwnd, nItem);
			}
		}
	}
	// xdid -g [NAME] [ID] [SWITCH] [+FLAGS] [X] [Y] (FILENAME) ([tab] watermark filename)
	else if (flags[TEXT('g')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		LVBKIMAGE lvbki{};
		TString filename;

		lvbki.ulFlags = this->parseImageFlags(input++);	// tok 4
		lvbki.xOffsetPercent = input++.to_int();	// tok 5
		lvbki.yOffsetPercent = input++.to_int();	// tok 6

		if (numtok > 6)
		{
			filename = input.getlasttoks().trim();				// tok 7, -1
			// make sure path exists & path is complete.
			if (!IsFile(filename))
			{
				// if not a file path check if its a url.
				if (!PathIsURL(filename.to_chr()))
					throw Dcx::dcxException(TEXT("Unable To Access File: %"), filename);
			}

			//GetFullPathName(filename.to_chr(), MAX_PATH, iconPath, nullptr);
			//
			//filename = input.gettok(7, -1).gettok(1,TSTAB);
			//
			//if (lvbki.ulFlags & LVBKIF_TYPE_WATERMARK) {
			//	TString watermarkfile(input.gettok(7,-1).gettok(2,-1,TSTAB).trim());
			//	lvbki.hbm = dcxLoadBitmap(lvbki.hbm, watermarkfile);
			//}

			lvbki.pszImage = filename.to_chr();
			lvbki.ulFlags |= LVBKIF_SOURCE_URL;
		}

		Dcx::dcxListView_SetBkImage(m_Hwnd, &lvbki);
	}
	// xdid -i [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if (flags[TEXT('i')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iColorFlags = this->parseColorFlags(input++);	// tok 4
		const auto tsClr(input++);	// tok 5
		const auto clrColor = (tsClr == TEXT("none")) ? CLR_NONE : tsClr.to_<COLORREF>();

		if (dcx_testflag(iColorFlags, LVCS_TEXT))
			Dcx::dcxListView_SetTextColor(m_Hwnd, clrColor);

		if (dcx_testflag(iColorFlags, LVCS_BKG))
			Dcx::dcxListView_SetBkColor(m_Hwnd, clrColor);

		if (dcx_testflag(iColorFlags, LVCS_BKGTEXT))
			Dcx::dcxListView_SetTextBkColor(m_Hwnd, clrColor);

		if (dcx_testflag(iColorFlags, LVCS_OUTLINE))
			Dcx::dcxListView_SetOutlineColor(m_Hwnd, clrColor);

		this->redrawWindow();
	}
	// xdid -j [NAME] [ID] [ROW] [COL] [FLAGS] ([COLOUR] (BGCOLOUR))
	// [NAME] [ID] -j [ROW] [COL] [FLAGS] ([COLOUR] (BGCOLOUR))
	else if (flags[TEXT('j')])
	{
		static_assert(CheckFreeCommand(TEXT('j')), "Command in use!");

		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = StringToItemNumber(input++);	// tok 4
		const auto nCol = input++.to_int() - 1;					// tok 5
		const auto lviflags = this->parseItemFlags(input++);	// tok 6
		const auto clrText = input++.to_<COLORREF>();	// tok 7
		const auto clrBack = input++.to_<COLORREF>();	// tok 8

		// invalid info
		if ((nItem < 0) || (nCol < 0) || (nCol >= this->getColumnCount()))
			throw DcxExceptions::dcxInvalidItem();

		LVITEM lvi{ LVIF_PARAM | LVIF_STATE, nItem, nCol };

		// couldn't retrieve info
		if (!Dcx::dcxListView_GetItem(m_Hwnd, &lvi))
			throw Dcx::dcxException("Unable to get Item.");

		const auto lviDcx = reinterpret_cast<LPDCXLVITEM>(lvi.lParam);

		if (!lviDcx)
			throw Dcx::dcxException("No DCX Item Information, somethings very wrong");

		if (gsl::narrow_cast<UINT>(nCol) >= lviDcx->vInfo.size())
			throw Dcx::dcxException("No Render Information for SubItem, More subitems than columns?");

		auto& ri = gsl::at(lviDcx->vInfo, gsl::narrow_cast<UINT>(nCol));

		ri.m_dFlags = lviflags;
		if (dcx_testflag(lviflags, LVIS_COLOR))
			ri.m_cText = clrText;
		else
			ri.m_cText = CLR_INVALID;

		if (dcx_testflag(lviflags, LVIS_BGCOLOR))
			ri.m_cBg = clrBack;
		else
			ri.m_cBg = CLR_INVALID;

		Dcx::dcxListView_SetItemState(m_Hwnd, nItem, lviflags, LVIS_DROPHILITED | LVIS_FOCUSED | LVIS_SELECTED | LVIS_CUT);
		Dcx::dcxListView_Update(m_Hwnd, nItem);
	}
	// xdid -k [NAME] [ID] [SWITCH] [STATE] [N,N2,N3-N4...]
	// xdid -k -> [NAME] [ID] -k [STATE] [N,N2,N3-N4...]
	else if (flags[TEXT('k')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto state = input.getnexttokas<int>();	// tok 4
		const auto Ns(input.getnexttok());				// tok 5
		const auto nItemCnt = Dcx::dcxListView_GetItemCount(m_Hwnd);

		const auto itEnd = Ns.end();
		for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
		{
			const auto tsLine(*itStart);

			const auto r = Dcx::getItemRange2(tsLine, nItemCnt);

			if ((r.b < 0) || (r.e < 0) || (r.b > r.e))
				throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

			for (auto nItem : r)
				Dcx::dcxListView_SetItemState(m_Hwnd, nItem, INDEXTOSTATEIMAGEMASK(gsl::narrow_cast<UINT>(state)), LVIS_STATEIMAGEMASK);
		}
	}
	// xdid -l [NAME] [ID] [SWITCH] [N,N2,N3-N4...] [M,M2,M3-M4...] [ICON] (OVERLAY)
	else if (flags[TEXT('l')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsItems(input.getnexttok());									// tok 4
		const auto tsSubItems(input.getnexttok());								// tok 5
		const auto nIcon = input.getnexttokas<int>() - 1;						// tok 6
		const auto nOverlay = (numtok > 6 ? input.getnexttokas<int>() : -1);	// tok 7

		// no icon to change
		if ((nIcon < -1) && (nOverlay < 0))
			return;	// not an error, just do nothing.

		// get total items
		const auto nItemCnt = Dcx::dcxListView_GetItemCount(m_Hwnd);
		// get total subitems
		const auto nSubItemCnt = this->getColumnCount();

		// iterate through all item ranges supplied
		const auto itEnd = tsItems.end();
		for (auto itStart = tsItems.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
		{
			const auto tsLine(*itStart);

			const auto ItemRange = Dcx::getItemRange2(tsLine, nItemCnt);

			if ((ItemRange.b < 0) || (ItemRange.e < 0) || (ItemRange.b > ItemRange.e))
				throw Dcx::dcxException(TEXT("Invalid Item Range %."), tsLine);

			// iterate through this range
			for (auto nItem : ItemRange)
			{
				LVITEM lvi{};

				lvi.iItem = nItem;

				// iterate through all subitem ranges supplied (must be done for each item)
				const auto itSubEnd = tsItems.end();
				for (auto itSubStart = tsSubItems.begin(TSCOMMACHAR); itSubStart != itSubEnd; ++itSubStart)
				{
					const auto tsSubLine(*itSubStart);
					const auto SubItemRange = Dcx::getItemRange2(tsSubLine, nSubItemCnt);

					if ((SubItemRange.b < 0) || (SubItemRange.e < 0) || (SubItemRange.b > SubItemRange.e))
						throw Dcx::dcxException(TEXT("Invalid SubItem Range %."), tsSubLine);

					// iterate through this subitem range.
					for (auto nSubItem : SubItemRange)
					{
						// invalid item
						if ((nItem < 0) || (nSubItem < 0) || (nSubItem >= nSubItemCnt))
							throw DcxExceptions::dcxInvalidItem();

						lvi.iSubItem = nSubItem;

						// theres an icon to change
						if (nIcon > -2)
						{
							lvi.mask = LVIF_IMAGE;
							lvi.iImage = nIcon;
						}

						if (nOverlay > -1)
						{
							lvi.mask |= LVIF_STATE;
							lvi.stateMask = LVIS_OVERLAYMASK;
							lvi.state = gsl::narrow_cast<UINT>(INDEXTOOVERLAYMASK(nOverlay));
						}

						// finally set the items icons
						Dcx::dcxListView_SetItem(m_Hwnd, &lvi);
					}
				}
			}
		}
	}
	// xdid -m [NAME] [ID] [SWITCH] [0|1]
	else if (flags[TEXT('m')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		Dcx::dcxListView_EnableGroupView(m_Hwnd, (input.getnexttok() == TEXT('1')));	// tok 4
	}
	// xdid -n [NAME] [ID] [SWITCH] [N] [+FLAGS] (WIDTH) ...
	else if (flags[TEXT('n')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		auto tsColumn = input.getnexttok(); // tok 4
		const XSwitchFlags xflags(input.getnexttok());		// tok 5
		const UINT iTotal = gsl::narrow_cast<UINT>(this->getColumnCount());

		this->setRedraw(FALSE);	// disable redraw while setting sizes
		Auto(this->setRedraw(TRUE));	// auto re-enable drawing

		// manually set widths for all specified columns
		if (xflags[TEXT('m')])
		{
			// xdid -n dname id -1 +m width1 width2 ...

			if ((numtok - 6) < iTotal)
				throw Dcx::dcxException("Insufficient number of widths specified for +m flag");

			for (auto i = decltype(iTotal){1}; i <= iTotal; ++i)
				Dcx::dcxListView_SetColumnWidth(m_Hwnd, i - 1, input.getnexttokas<int>());	// tok 6+
		}
		else if (xflags[TEXT('d')])
		{
			// dynamic widths...
			if ((numtok - 6) < iTotal)
				throw Dcx::dcxException("Insufficient number of widths specified for +d flag");

			const auto nColumn = (tsColumn.to_int() - 1);	// tok 4

			if ((nColumn < 0) || (gsl::narrow_cast<UINT>(nColumn) >= iTotal))
				throw Dcx::dcxException(TEXT("Invalid column specified: %"), nColumn + 1);

			const auto iFlags = this->parseHeaderFlags2(xflags);
			const auto iWidth = input.getnexttokas<int>();	// tok 6

			for (const auto& a : this->m_vWidths)
			{
				if (a->m_iColumn == nColumn)
				{
					// column info found
					a->m_dFlags = gsl::narrow_cast<DWORD>(iFlags);
					a->m_iSize = iWidth;
					break;
				}
			}
			this->autoSize(nColumn, iFlags, iWidth);
		}
		else {
			const auto iFlags = this->parseHeaderFlags2(xflags);
			const auto iWidth = input.getnexttokas<int>();	// tok 6

			if ((iFlags == 0) && (numtok < 6))
				throw Dcx::dcxException("No width specified");

			const auto HandleColumn = [=](const TString& tsColumns) {
				const auto r = Dcx::getItemRange2(tsColumn, this->getColumnCount());

				if ((r.b < 0) || (r.e < r.b))
					throw DcxExceptions::dcxOutOfRange();

				for (auto nColumn : r)
				{
					this->autoSize(nColumn, iFlags, iWidth);
				}
			};
			if (tsColumn.numtok(TSCOMMACHAR) > 1)
			{
				// column == 1,2,3-4....
				const auto itEnd = tsColumn.end();
				for (auto itStart = tsColumn.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
				{
					HandleColumn(*itStart);
				}
			}
			else {
				if (tsColumn == TEXT('0'))
				{
					// column = 0 == set all columns to single width
					for (UINT n{}; n < iTotal; ++n)
						this->autoSize(n, iFlags, iWidth);
				}
				else
					// column = 3-4
					HandleColumn(tsColumn);
			}
		}
	}
	// xdid -o [NAME] [ID] [SWITCH] [ORDER ...]
	else if (flags[TEXT('o')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto ids(input.getlasttoks().trim());	// tok 4, -1
		const UINT count = gsl::narrow_cast<UINT>(this->getColumnCount());

		// Basic check first
		if (ids.numtok() != count)
			throw Dcx::dcxException("Incorrect number of indexes.");

		const auto indexes = std::make_unique<int[]>(count);

		UINT i = 0;
		for (const auto& id : ids)
		{
			const auto tmp = id.to_int();

			if ((tmp == 0) || // invalid character
				(tmp < 0) ||  // negative
				(gsl::narrow_cast<UINT>(tmp) > count)) // out of array bounds
				throw Dcx::dcxException(TEXT("Invalid index %."), tmp);

			gsl::at(indexes, i++) = tmp - 1;
		}

		Dcx::dcxListView_SetColumnOrderArray(m_Hwnd, count, indexes.get());
	}
	// xdid -O [NAME] [ID] [FLAGS] [N,N2,N3-N4...] [M]
	// Move item or range of items...
	// [NAME] [ID] -O [FLAGS] [N,N2,N3-N4...] [M]
	else if (flags[TEXT('O')])
	{
		static_assert(CheckFreeCommand(TEXT('O')), "Command in use!");

		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xFlags(input.getnexttok());
		const auto tsItems(input.getnexttok());
		const auto nDestItem = StringToItemNumber(input++);

		// get total items
		const auto nItemCnt = Dcx::dcxListView_GetItemCount(m_Hwnd);

		if (!xFlags[L'+'])
			throw DcxExceptions::dcxInvalidFlag();

		// invalid info (allow moving to one item beyond last)
		if ((nDestItem < 0) || (nDestItem > nItemCnt))
			throw DcxExceptions::dcxInvalidItem();

		// iterate through all item ranges supplied
		const auto itEnd = tsItems.end();
		for (auto itStart = tsItems.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
		{
			const auto tsLine(*itStart);

			const auto ItemRange = Dcx::getItemRange2(tsLine, nItemCnt);

			if ((ItemRange.b < 0) || (ItemRange.e < 0) || (ItemRange.b > ItemRange.e))
				throw Dcx::dcxException(TEXT("Invalid Item Range %."), tsLine);

			// iterate through this range
			for (auto nItem : ItemRange)
			{
				MoveItem(nItem, nDestItem, xFlags[L's']);
			}
		}

	}
	// xdid -q [NAME] [ID] [SWITCH] [N] [+FLAGS] [GID] [Group Text]
	// xdid -q [NAME] [ID] [SWITCH] [N] [+FLAGS] [GID] [Group Text] ([tab] Group column2 text)...
	else if (flags[TEXT('q')])
	{
		if (numtok < 7)
			throw DcxExceptions::dcxInvalidArguments();

		addGroup(input);
	}
	// xdid -Q [NAME] [ID] [SWITCH] [Add|Move|Del|Setup] ....
	// xdid -Q [NAME] [ID] [SWITCH] Add [N] [+FLAGS] [GID] [Group Text] ([tab] Group column2 text)...
	// xdid -Q [NAME] [ID] [SWITCH] Move [GID] [N]
	// xdid -Q [NAME] [ID] [SWITCH] Del [GID]
	// xdid -Q [NAME] [ID] [SWITCH] Setup [GID,GID2,GID3-GIDn] [+FLAGS]
	else if (flags[TEXT('Q')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsCmd(input++);										// tok 4

		switch (std::hash<TString>()(tsCmd))
		{
			// xdid -Q [NAME] [ID] [SWITCH] Add [N] [+FLAGS] [GID] [Group Text] ([tab] Group column2 text)...
		case TEXT("Add"_hash):
		case TEXT("add"_hash):
		{
			if (numtok < 8)
				throw DcxExceptions::dcxInvalidArguments();

			addGroup(input);
			break;
		}
		// xdid -Q [NAME] [ID] [SWITCH] Move [GID] [N]
		case TEXT("Move"_hash):
		case TEXT("move"_hash):
		{
			if (numtok < 6)
				throw DcxExceptions::dcxInvalidArguments();

			const auto gid = input++.to_int();								// tok 5
			const auto index = input++.to_int() - 1;						// tok 6

			if (!Dcx::dcxListView_HasGroup(m_Hwnd, gid))
				throw Dcx::dcxException(TEXT("Group doesn't exist: %"), gid);

			Dcx::dcxListView_MoveGroup(m_Hwnd, gid, index);
			break;
		}
		// xdid -Q [NAME] [ID] [SWITCH] Del [GID]
		case TEXT("Del"_hash):
		case TEXT("del"_hash):
		{
			if (numtok < 5)
				throw DcxExceptions::dcxInvalidArguments();

			const auto gid = input++.to_int();								// tok 5

			if (gid == -1)
				Dcx::dcxListView_RemoveAllGroups(m_Hwnd);
			else {
				if (!Dcx::dcxListView_HasGroup(m_Hwnd, gid))
					throw Dcx::dcxException(TEXT("Group doesn't exist: %"), gid);

				Dcx::dcxListView_RemoveGroup(m_Hwnd, gid);
			}
			break;
		}
		// xdid -Q [NAME] [ID] [SWITCH] Setup [GID,GID2,GID3-GIDn] [+FLAGS] [+MASK] (ARGS)
		case TEXT("Setup"_hash):
		case TEXT("setup"_hash):
		{
			if (numtok < 6)
				throw DcxExceptions::dcxInvalidArguments();

			const auto tsGID = input.getnexttok();
			const auto tsFlags(input.getnexttok());
			const auto tsMask(input.getnexttok());
			const auto tsArgs(input.getlasttoks()); // flags dependant args, if any

			const auto iFlags = this->parseGroupFlags(tsFlags);
			const auto iState = this->parseGroupState(tsFlags);

			const auto iFlagsMask = this->parseGroupFlags(tsMask);
			const auto iStateMask = this->parseGroupState(tsMask);

			LVGROUP lvg{};
			lvg.cbSize = sizeof(LVGROUP);
			lvg.mask = LVGF_STATE | LVGF_ALIGN;

			const auto gid_count = Dcx::dcxListView_GetGroupCount(m_Hwnd);
			const auto itEnd = tsGID.end();
			for (auto itStart = tsGID.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
			{
				const auto tsLine(*itStart);
				const auto r = Dcx::make_range(tsLine, gid_count);

				if ((r.b < 0) || (r.e < 0) || (r.b > r.e))
					throw DcxExceptions::dcxInvalidArguments();

				for (const auto nGID : r)
				{
					// setup each specified group.
					if (!Dcx::dcxListView_HasGroup(m_Hwnd, nGID))
						throw Dcx::dcxException(TEXT("Group doesn't exist: %"), nGID);
					lvg.iGroupId = nGID;
					Dcx::dcxListView_GetGroupInfo(m_Hwnd, nGID, &lvg);

					lvg.stateMask = iStateMask;
					lvg.state = iState;

					if (iFlagsMask != 0)
					{
						if (const UINT uAlign{ (iFlags & iFlagsMask) }; uAlign != lvg.uAlign)
							lvg.uAlign = uAlign;
					}

					Dcx::dcxListView_SetGroupInfo(m_Hwnd, nGID, &lvg);
				}
			}
		}
		break;
		default:
			throw DcxExceptions::dcxInvalidCommand();
			break;
		}
	}
	// xdid -r [NAME] [ID] [SWITCH]
	// Note: This is here to prevent a message
	else if (flags[TEXT('r')])
	{
		//ListView_DeleteAllItems(m_Hwnd);
	}
	// xdid -t [NAME] [ID] [SWITCH] [+FLAGS] [#ICON] [WIDTH] (Header text) [{TAB} [+FLAGS] [#ICON] [WIDTH] Header text {TAB} ... ]
	else if (flags[TEXT('t')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		this->DeleteColumns(-1);

		auto nColumn = 0;
		for (auto tsIt = input.begin(TSTABCHAR); tsIt; ++tsIt)
		{
			auto data(*tsIt);
			data.trim();
			const auto tsflags(data.getfirsttok((nColumn == 0) ? 4 : 1));	// tok 1
			const auto icon = data.getnexttokas<int>() - 1;					// tok 2
			const auto width = data.getnexttokas<int>();					// tok 3

		TString itemtext;
		if (data.numtok() > 3)
				itemtext = data.getlasttoks();								// tok 4, -1

		this->addColumn(nColumn, -1, tsflags, icon, width, itemtext);

				++nColumn;
			}
		}
	// xdid -u [NAME] [ID] [SWITCH]
	else if (flags[TEXT('u')])
	{
		Dcx::dcxListView_SetItemState(m_Hwnd, -1, 0, LVIS_SELECTED);
		Dcx::dcxListView_SetSubItemListState(m_Hwnd, m_SubItemsSelected, 0, LVIS_SELECTED);
		m_SubItemsSelected.clear();
	}
	// xdid -v [NAME] [ID] [SWITCH] [N] [M] (ItemText)
	else if (flags[TEXT('v')])
	{
		Command_v(input);
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tflags(input.getnexttok());	// tok 4
		const auto tsIndex(input.getnexttok());	// tok 5
		auto filename(input.getlasttoks());		// tok 6, -1

		this->loadIcon(tflags, tsIndex, filename);
	}
	// xdid -W [NAME] [ID] [SWITCH] [VIEW|nochange] (CONTROL EXTENDED STYLES)
	else if (flags[TEXT('W')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		DWORD mode = MAXDWORD;

		switch (std::hash<TString>{}(input.getnexttok()))
		{
		case TEXT("report"_hash):
			mode = LV_VIEW_DETAILS;
			break;
		case TEXT("icon"_hash):
			mode = LV_VIEW_ICON;
			break;
		case TEXT("smallicon"_hash):
			mode = LV_VIEW_SMALLICON;
			break;
		case TEXT("list"_hash):
			mode = LV_VIEW_LIST;
			break;
		case TEXT("tile"_hash):
			mode = LV_VIEW_TILE;
			break;
		case TEXT("nochange"_hash):
		default:
			mode = MAXDWORD;
			break;
		}

		if (mode != MAXDWORD)
			Dcx::dcxListView_SetView(m_Hwnd, mode);

		if (const auto tsStyles = input.getnexttok(); !tsStyles.empty())
		{
			//const auto styles = parseControlStyles(tsStyles);
			const auto lvExStyles = parseListviewExStyles(tsStyles);

			const auto currentExStyles = Dcx::dcxListView_GetExtendedListViewStyle(m_Hwnd);
			Dcx::dcxListView_SetExtendedListViewStyleEx(m_Hwnd, gsl::narrow_cast<WPARAM>(lvExStyles), gsl::narrow_cast<LPARAM>(~currentExStyles));
		}
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if (flags[TEXT('y')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iFlags = this->parseIconFlagOptions(input.getnexttok());	// tok 4

		if (dcx_testflag(iFlags, LVSIL_SMALL))
		{
			if (const auto himl = this->getImageList(LVSIL_SMALL); himl)
			{
				this->setImageList(nullptr, LVSIL_SMALL);
				ImageList_Destroy(himl);
			}

			if (const auto himl = this->getImageList(LVSIL_NORMAL); himl)
			{
				this->setImageList(nullptr, LVSIL_NORMAL);
				ImageList_Destroy(himl);
			}
		}

		if (dcx_testflag(iFlags, LVSIL_STATE))
		{
			if (const auto himl = this->getImageList(LVSIL_STATE); himl)
			{
				this->setImageList(nullptr, LVSIL_STATE);
				ImageList_Destroy(himl);
			}
		}
		if (dcx_testflag(iFlags, LVSIL_FOOTER))
		{
			if (const auto himl = this->getImageList(LVSIL_FOOTER); himl)
			{
				this->setImageList(nullptr, LVSIL_FOOTER);
				ImageList_Destroy(himl);
			}
		}
	}
	// xdid -z [NAME] [ID] [SWITCH] [+FLAGS] [N] (ALIAS)
	else if (flags[TEXT('z')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto lvsort = std::make_unique<DCXLVSORT>();	// too big for stack, use heap.

		lvsort->iSortFlags = this->parseSortFlags(input.getnexttok());	// tok 4
		const auto nColumn = input.getnexttokas<int>() - 1;	// tok 5

		lvsort->m_Hwnd = m_Hwnd;
		lvsort->nColumn = nColumn;

		if (nColumn < 0 || nColumn >= this->getColumnCount())
			throw DcxExceptions::dcxInvalidArguments();

		if (dcx_testflag(lvsort->iSortFlags, LVSS_CUSTOM) && numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		lvsort->tsCustomAlias = input.getnexttok();		// tok 6

		Dcx::dcxListView_SortItemsEx(m_Hwnd, DcxListView::sortItemsEx, lvsort.get());
	}
	// xdid -T [NAME] [ID] [SWITCH] [nItem] [nSubItem] (ToolTipText)
	// atm this only seems works for subitem 0. Mainly due to the callback LVN_GETINFOTIP only being sent for sub 0.
	// This overrides the Global T command for tooltips
	else if (flags[TEXT('T')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		LVITEM lvi{ LVIF_PARAM, input.getnexttokas<int>() - 1, input.getnexttokas<int>() };

		// check if item supplied was 0 (now -1), last item in list.
		if (lvi.iItem == -1)
		{
			lvi.iItem = Dcx::dcxListView_GetItemCount(m_Hwnd) - 1;

			if (lvi.iItem < 0)
				throw Dcx::dcxException("Invalid Item: No Items in list");
		}

		if ((lvi.iItem < 0) || (lvi.iSubItem < 0))
			throw Dcx::dcxException(TEXT("Invalid Item: % Subitem: %"), lvi.iItem + 1, lvi.iSubItem);

		if (!Dcx::dcxListView_GetItem(m_Hwnd, &lvi))
			throw Dcx::dcxException(TEXT("Unable To Get Item: % Subitem: %"), lvi.iItem + 1, lvi.iSubItem);

		const auto lpmylvi = reinterpret_cast<LPDCXLVITEM>(lvi.lParam);

		if (!lpmylvi)
			throw Dcx::dcxException("Unable to get DCX Item Information, something very wrong!");

		lpmylvi->tsTipText = (numtok > 5 ? input.getlasttoks() : TEXT(""));	// tok 6, -1

		LVSETINFOTIP it{ sizeof(LVSETINFOTIP), 0, lpmylvi->tsTipText.to_chr(), lvi.iItem, lvi.iSubItem };

		Dcx::dcxListView_SetInfoTip(m_Hwnd, &it);
	}
	// xdid -Z [NAME] [ID] [SWITCH] [%]
	else if (flags[TEXT('Z')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		// only works for this one so far
		//if (!this->isStyle(LVS_REPORT))
		//	return;

		auto pos = input.getnexttokas<int>();	// tok 4
		auto count = Dcx::dcxListView_GetItemCount(m_Hwnd);

		// no items - no need to view
		// check boundaries
		if ((count == 0) || (pos < 0) || (pos > 100))
			return;	// no error, just do nothing.

		// subtract the number of visible items
		count -= Dcx::dcxListView_GetCountPerPage(m_Hwnd);

		RECT rc{};
		// get height of a single item
		Dcx::dcxListView_GetItemRect(m_Hwnd, 0, &rc, LVIR_BOUNDS);
		const auto height = count * (rc.bottom - rc.top);

		//pos = std::lroundf(gsl::narrow_cast<float>(height) * gsl::narrow_cast<float>(pos) / gsl::narrow_cast<float>(100.0));
		pos = dcx_round(gsl::narrow_cast<float>(height) * gsl::narrow_cast<float>(pos) / 100.0f);
		//pos = Dcx::dcxRound<int>(height * pos / 100.0);

		Dcx::dcxListView_EnsureVisible(m_Hwnd, 0, FALSE);
		Dcx::dcxListView_Scroll(m_Hwnd, 0, pos);
	}
	// xdid -V [NAME] [ID] [SWITCH] [nItem] (subitem)
	else if (flags[TEXT('V')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		if (const auto nItem = StringToItemNumber(input.getnexttok()); nItem > -1)
		{
			Dcx::dcxListView_EnsureVisible(m_Hwnd, nItem, FALSE);

			if (const auto nSub = input.getnexttokas<int>(); nSub)
				Dcx::dcxListView_EnsureSubItemVisible(m_Hwnd, nItem, nSub);
		}
	}
	// xdid -S [NAME] [ID] [+FLAGS] [N1] [N2] [ARGS]
	else if (flags[TEXT('S')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		// [N1] [N2] are the item range to save.
		//   0 < [N1] <= total items in listview.
		//   if [N2] is zero then all items from N1 onward are saved.
		//   if [N2] is the same as [N1] it's ignored.
		//   if [N2] is a negative number then its taken as [N1] to [total number of items in list + [N2]]
		//[+FLAGS] decides what [ARGS] is.
		// +f save to file [filename] (data is appended to an existing file or a new file is created)
		// +h save to hashtable [table] (table must exist)
		// +x save to xml [dataset_name filename]
		// +c save to custom @window [@window] (data is appended to the bottom of the window, window must exist)
		const auto count = Dcx::dcxListView_GetItemCount(m_Hwnd);
		const auto tsFlags(input.getnexttok().trim());		// tok 4
		const auto iN1 = input.getnexttokas<int>() - 1;	// tok 5 adjusted from 1-based to be zero based
		auto iN2 = input.getnexttokas<int>() - 1;			// tok 6 adjusted from 1-based to be zero based
		const auto tsArgs(input.getlasttoks().trim());		// tok 7, -1

		if ((tsFlags[0] != TEXT('+')) || (tsFlags.len() < 2))
			// no flags specified.
			throw DcxExceptions::dcxInvalidFlag();

		// make sure N1-N2 are within the range of items in listview.
		// adjust iN2 if its < 0, so its an offset from the last item.
		if (iN2 == -1)
			iN2 += count;
		else if (iN2 < -1)
			iN2 += count;

		if ((iN1 < 0) || (iN1 >= count) || (iN2 < 0) || (iN2 < iN1))
			throw Dcx::dcxException("Invalid Range: N1-N2 Must be in range of items in listview");

		switch (tsFlags[1])
		{
		case TEXT('c'):
			xSaveListview(iN1, iN2, tsArgs, TEXT("$window(%)"), TEXT("/echo % %"));
			break;
		case TEXT('f'):
			xSaveListview(iN1, iN2, tsArgs, TEXT("$isfile(%)"), TEXT("/write -m1 % %"));
			break;
		case TEXT('h'):
			xSaveListview(iN1, iN2, tsArgs, TEXT("$hget(%)"), TEXT("/hadd % %"));
			break;
		case TEXT('x'):
		{
			//xSaveListview(iN1, iN2, tsArgs, TEXT("$window(%)"), TEXT("echo % %"));
			const TString tsDataset(tsArgs.getfirsttok(1));
			TString tsFilename(tsArgs.getlasttoks());
			xmlSaveListview(iN1, iN2, tsDataset, tsFilename);
		}
		break;
		default:
			throw DcxExceptions::dcxInvalidFlag();
		}
	}
	// xdid -H [NAME] [ID] [COL] [+FLAGS] [ARGS]
	// xdid -H [NAME] [ID] -H [COL|COL1-COL2|COL1,COL2|COL1,COL2-COL3 etc..] [+FLAGS] [ARGS]
	else if (flags[TEXT('H')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsCols(input.getnexttok());			// tok 4
		const XSwitchFlags xflag(input.getnexttok());		// tok 5
		const auto info(input.getlasttoks());			// tok 6, -1

		if (!xflag[TEXT('+')])
			throw DcxExceptions::dcxInvalidFlag();

		auto h = Dcx::dcxListView_GetHeader(m_Hwnd);
		if (!IsWindow(h))
			throw Dcx::dcxException("Unable to get Header Window");

		{
			const auto col_count = this->getColumnCount();
			const auto itEnd = tsCols.end();
			for (auto itStart = tsCols.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
			{
				const auto col(*itStart);
				const auto r = Dcx::getItemRange2(col, col_count);

				if ((r.b < 0) || (r.e < 0) || (r.b >= col_count) || (r.e > col_count))
					throw Dcx::dcxException(TEXT("Invalid column index %."), col);

				for (auto nCol : r)
				{
					if (!xflag['s'])	// change header style
						throw Dcx::dcxException(TEXT("Unknown flags %"), input.gettok(5));

					setHeaderStyle(h, nCol, info);
				}
			}
		}
	}
	// xdid -G [NAME] [ID] [SWITCH] [GID,GID2,GID3-GIDn] [+MASK] [+STATES]
	else if (flags[TEXT('G')])
	{
		if (numtok != 6)
			throw DcxExceptions::dcxInvalidArguments();

		if (!Dcx::VersInfo.isVista())
			throw Dcx::dcxException("This Command is Vista+ Only!");

		const auto tsGID = input.getnexttok();							// tok 4
		const auto iMask = this->parseGroupState(input.getnexttok());	// tok 5
		const auto iState = this->parseGroupState(input.getnexttok());	// tok 6

		const auto gid_count = Dcx::dcxListView_GetGroupCount(m_Hwnd);
		const auto itEnd = tsGID.end();
		for (auto itStart = tsGID.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
		{
			const auto tsLine(*itStart);
			const auto r = Dcx::make_range(tsLine, gid_count);

			if ((r.b < 0) || (r.e < 0) || (r.b > r.e))
				throw DcxExceptions::dcxInvalidArguments();

			for (auto nGID : r)
			{
				if (!Dcx::dcxListView_HasGroup(m_Hwnd, nGID))
					throw Dcx::dcxException(TEXT("Group doesn't exist: %"), nGID);

				Dcx::dcxListView_SetGroupState(m_Hwnd, nGID, iMask, iState);
			}
		}
	}
	// xdid -N [NAME] [ID] [SWITCH] [+FLAGS] [ARGS]
	// xdid -N [NAME] [ID] [SWITCH] [+m] [LEFT] [RIGHT] [TOP] [BOTTOM]
	// xdid -N [NAME] [ID] [SWITCH] [+Lc] [COLOUR BKG]
	// xdid -N [NAME] [ID] [SWITCH] [+Lb] [COLOUR BORDER]
	// xdid -N [NAME] [ID] [SWITCH] [+Li] [COLOUR TEXT]
	// xdid -N [NAME] [ID] [SWITCH] [+Lt] [TEXT]
	// xdid -N [NAME] [ID] [SWITCH] [+LC] [control]
	else if (flags[TEXT('N')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xFlags(input.getnexttok());
		const auto tsArgs(input.getlasttoks());

		if (xFlags[TEXT('m')])
		{
			// set margin sizes (solo flag, cant be combined with others)
			// [ARGS] = [LEFT] [RIGHT] [TOP] [BOTTOM]
			if (RECT rc{}; Dcx::dcxListView_GetViewMargin(m_Hwnd, &rc))
			{
				if (const auto ileft = tsArgs.getfirsttok(1).to_<long>(); ileft >= 0)
					rc.left = ileft;
				if (const auto iright = tsArgs.getnexttokas<long>(); iright >= 0)
					rc.right = iright;
				if (const auto itop = tsArgs.getnexttokas<long>(); itop >= 0)
					rc.top = itop;
				if (const auto ibottom = tsArgs.getnexttokas<long>(); ibottom >= 0)
					rc.bottom = ibottom;

				// header height is included in the top margin listview returns
				// but musnt be included in the top value we set.
				// we need to adjust top value to remove it before setting.
				if (auto hHdr = Dcx::dcxListView_GetHeader(m_Hwnd); hHdr)
				{
					if (IsWindowVisible(hHdr))
					{
						const Dcx::dcxWindowRect rcHdr(hHdr);

						rc.top -= rcHdr.Height();
						rc.top = std::max(0L, rc.top);
					}
				}
				Dcx::dcxListView_SetViewMargin(m_Hwnd, &rc);
			}
		}
		else {
			auto parseMargin = [](_In_ const XSwitchFlags& xFlag, _In_ const TString& tsArg, _Inout_ dcxListViewMarginSideData& mData) {
				if (xFlag[TEXT('c')])
				{
					// colour bkg
					mData.m_clrBkg = tsArg.to_<COLORREF>();
				}
				if (xFlag[TEXT('b')])
				{
					// colour border
					mData.m_clrBorder = tsArg.to_<COLORREF>();
				}
				if (xFlag[TEXT('i')])
				{
					// colour text
					mData.m_clrTxt = tsArg.to_<COLORREF>();
				}
				if (xFlag[TEXT('t')])
				{
					// text
					mData.m_Text = tsArg;
				}
				if (xFlag[TEXT('C')])
				{
					// control
				}
			};
			if (xFlags[TEXT('L')])
			{
				// left margin options
				parseMargin(xFlags, tsArgs, m_MarginData.m_Left);
			}
			else if (xFlags[TEXT('R')])
			{
				// right margin options
				parseMargin(xFlags, tsArgs, m_MarginData.m_Right);
			}
			else if (xFlags[TEXT('T')])
			{
				// top margin options
				parseMargin(xFlags, tsArgs, m_MarginData.m_Top);
			}
			else if (xFlags[TEXT('B')])
			{
				// bottom margin options
				parseMargin(xFlags, tsArgs, m_MarginData.m_Bottom);
			}
			else
				throw DcxExceptions::dcxInvalidFlag();
		}
	}
	else
		this->parseGlobalCommandRequest(input, flags); // bCefFhJMpURsTxz
}

void DcxListView::setHeaderStyle(HWND h, const int nCol, const TString& info)
{
	HDITEM hdr{};
	hdr.mask = HDI_FORMAT;

	if (!Dcx::dcxHeader_GetItem(h, nCol, &hdr))
		throw Dcx::dcxException(TEXT("Unable to get item: %"), nCol + 1);

	for (const auto& tmp : info)
	{
		switch (std::hash<TString>{}(tmp))
		{
		case TEXT("sortdown"_hash):	// sortdown
		{
			hdr.fmt &= ~HDF_SORTUP;
			hdr.fmt |= HDF_SORTDOWN;
		}
		break;
		case TEXT("sortup"_hash):	// sortup
		{
			hdr.fmt &= ~HDF_SORTDOWN;
			hdr.fmt |= HDF_SORTUP;
		}
		break;
		case TEXT("checkbox"_hash):	// checkbox
		{
			hdr.fmt |= HDF_CHECKBOX;
		}
		break;
		case TEXT("checked"_hash):	// checked
		{
			hdr.fmt |= HDF_CHECKED;
		}
		break;
		case TEXT("nocheckbox"_hash):	// nocheckbox
		{
			hdr.fmt &= ~(HDF_CHECKBOX | HDF_CHECKED);
		}
		break;
		case TEXT("unchecked"_hash):	// unchecked
		{
			hdr.fmt &= ~HDF_CHECKED;
		}
		break;
		case TEXT("nosort"_hash):	// nosort
		{
			hdr.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
		}
		break;
		default:	//error
			// silently fail on invalid styles
			break;
		}
	}

	Dcx::dcxHeader_SetItem(h, nCol, &hdr);
}

void DcxListView::addColumn(int nColumn, int iOrder, const TString& tsFlags, int iIcon, int iWidth, const TString& tsText)
{
	TString itemtext(tsText);

	LVCOLUMN lvc{};

	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = gsl::narrow_cast<int>(this->parseHeaderFlags(tsFlags));
	lvc.cx = iWidth;
	lvc.pszText = itemtext.to_chr();
	lvc.iSubItem = 0;
	lvc.iImage = I_IMAGENONE;
	if (iOrder >= 0)
	{
		lvc.mask |= LVCF_ORDER;
		lvc.iOrder = iOrder;
	}
	if (iIcon > -1)
	{
		lvc.mask |= LVCF_IMAGE;
		lvc.iImage = iIcon;
	}

	// column count always zero here, since we just deleted them all
	if (Dcx::dcxListView_InsertColumn(m_Hwnd, nColumn, &lvc) == -1)
		throw Dcx::dcxException(TEXT("Unable to add column: %"), nColumn);

	//++this->m_iColumnCount;
	this->m_iColumnCount = -1; // force a recount

	/*
	*	These flags do NOT make the columns auto size as text is added
	* They auto size the columns to match the text already present (none).
	*/
	this->autoSize(nColumn, tsFlags);
}

/*
Initializes an image list.
*/
gsl::strict_not_null<HIMAGELIST> DcxListView::initImageList(const int iImageList)
{
	if (auto himl = getImageList(iImageList); himl)
		return gsl::make_strict_not_null(himl);

	auto himl = createImageList((iImageList == LVSIL_NORMAL));

	if (!himl)
		throw Dcx::dcxException("Unable to create image list");

	setImageList(himl, iImageList);

	return gsl::make_strict_not_null(himl);
}

GSL_SUPPRESS(lifetime.4)
HIMAGELIST DcxListView::getImageList(const int iImageList) const noexcept
{
	return Dcx::dcxListView_GetImageList(m_Hwnd, iImageList);
}

void DcxListView::setImageList(const HIMAGELIST himl, const int iImageList) noexcept
{
	if (auto o = Dcx::dcxListView_SetImageList(m_Hwnd, himl, iImageList); (o && o != himl))
		ImageList_Destroy(o);
}

UINT DcxListView::parseIconFlagOptions(const TString& flags)
{
	UINT iFlags = 0;

	const XSwitchFlags xflags(flags);

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	if (xflags[TEXT('n')])
		iFlags |= LVSIL_SMALL;
	if (xflags[TEXT('s')])
		iFlags |= LVSIL_STATE;
	if (xflags[TEXT('f')])
		iFlags |= LVSIL_FOOTER;
	if (xflags[TEXT('g')])
		iFlags |= LVSIL_GROUPHEADER;
	return iFlags;
}

UINT DcxListView::parseItemFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	if (xflags[TEXT('b')])
		iFlags |= LVIS_BOLD;
	if (xflags[TEXT('c')])
		iFlags |= LVIS_COLOR;
	if (xflags[TEXT('C')])
		iFlags |= LVIS_CENTERICON;
	if (xflags[TEXT('d')])
		iFlags |= LVIS_DROPHILITED;
	if (xflags[TEXT('f')])
		iFlags |= LVIS_FOCUSED;
	if (xflags[TEXT('H')])
		iFlags |= LVIS_HASHITEM;
	if (xflags[TEXT('i')])
		iFlags |= LVIS_ITALIC;
	if (xflags[TEXT('k')])
		iFlags |= LVIS_BGCOLOR;
	if (xflags[TEXT('n')])
		iFlags |= LVIS_HASHNUMBER;
	if (xflags[TEXT('p')])
		iFlags |= LVIS_PBAR;
	if (xflags[TEXT('u')])
		iFlags |= LVIS_UNDERLINE;
	if (xflags[TEXT('s')])
		iFlags |= LVIS_SELECTED;
	if (xflags[TEXT('t')])
		iFlags |= LVIS_CUT;
	if (xflags[TEXT('w')])
		iFlags |= LVIS_HASHTABLE;
	if (xflags[TEXT('x')])
		iFlags |= LVIS_XML;
	if (xflags[TEXT('y')])
		iFlags |= LVIS_WINDOW;
	if (xflags[TEXT('z')])
		iFlags |= LVIS_CONTROL;

	return iFlags;
}

TString DcxListView::parseItemFlags(UINT uFlags)
{
	TString tsFlags(L'+');
	if (dcx_testflag(uFlags, LVIS_BOLD))
		tsFlags += TEXT('b');
	if (dcx_testflag(uFlags, LVIS_COLOR))
		tsFlags += TEXT('c');
	if (dcx_testflag(uFlags, LVIS_CENTERICON))
		tsFlags += TEXT('C');
	if (dcx_testflag(uFlags, LVIS_DROPHILITED))
		tsFlags += TEXT('d');
	if (dcx_testflag(uFlags, LVIS_FOCUSED))
		tsFlags += TEXT('f');
	if (dcx_testflag(uFlags, LVIS_HASHITEM))
		tsFlags += TEXT('H');
	if (dcx_testflag(uFlags, LVIS_ITALIC))
		tsFlags += TEXT('i');
	if (dcx_testflag(uFlags, LVIS_BGCOLOR))
		tsFlags += TEXT('k');
	if (dcx_testflag(uFlags, LVIS_HASHNUMBER))
		tsFlags += TEXT('n');
	if (dcx_testflag(uFlags, LVIS_PBAR))
		tsFlags += TEXT('p');
	if (dcx_testflag(uFlags, LVIS_UNDERLINE))
		tsFlags += TEXT('u');
	if (dcx_testflag(uFlags, LVIS_SELECTED))
		tsFlags += TEXT('s');
	if (dcx_testflag(uFlags, LVIS_CUT))
		tsFlags += TEXT('t');
	if (dcx_testflag(uFlags, LVIS_HASHTABLE))
		tsFlags += TEXT('w');
	if (dcx_testflag(uFlags, LVIS_XML))
		tsFlags += TEXT('x');
	if (dcx_testflag(uFlags, LVIS_WINDOW))
		tsFlags += TEXT('y');
	if (dcx_testflag(uFlags, LVIS_CONTROL))
		tsFlags += TEXT('z');

	return tsFlags;
}

UINT DcxListView::parseMassItemFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	if (xflags[TEXT('a')])
		iFlags |= LVIMF_ALLINFO;
	if (xflags[TEXT('A')])
		iFlags |= LVIMF_ADDALL;
	if (xflags[TEXT('n')])
		iFlags |= LVIMF_NUMERIC;
	if (xflags[TEXT('i')])
		iFlags |= LVIMF_NAMED;

	return iFlags;
}

// used flags bcflr
UINT DcxListView::parseHeaderFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	// 'a' use by flags2
	if (xflags[TEXT('b')])
		iFlags |= LVCFMT_BITMAP_ON_RIGHT;
	if (xflags[TEXT('c')])
		iFlags |= LVCFMT_CENTER;
	if (xflags[TEXT('d')])
		iFlags |= LVCFMT_SPLITBUTTON;
	if (xflags[TEXT('e')])
		iFlags |= HDF_SORTDOWN;
	if (xflags[TEXT('f')])
		iFlags |= LVCFMT_FIXED_WIDTH;
	// 'F' use by flags2
	if (xflags[TEXT('g')])
		iFlags |= HDF_SORTUP;
	// 'h' use by flags2
	if (xflags[TEXT('l')])
		iFlags |= LVCFMT_LEFT;
	// 'p' use by flags2
	if (xflags[TEXT('q')])
		iFlags |= LVCFMT_FIXED_RATIO;
	if (xflags[TEXT('r')])
		iFlags |= LVCFMT_RIGHT;
	// 's' use by flags2

	return iFlags;
}

INT DcxListView::parseHeaderFlags2(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	return parseHeaderFlags2(xflags);
}

INT DcxListView::parseHeaderFlags2(const XSwitchFlags& xflags)
{
	int iFlags{};

	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	if (xflags[TEXT('a')]) // auto size
		iFlags |= DCX_LV_COLUMNF_AUTO;
	// 'b' use by flags
	// 'c' use by flags
	// 'd' use by flags
	// 'e' use by flags
	// 'f' use by flags
	if (xflags[TEXT('F')]) // fixed width.
		iFlags |= DCX_LV_COLUMNF_FIXED;
	// 'g' use by flags
	if (xflags[TEXT('h')]) // header size
		iFlags |= DCX_LV_COLUMNF_AUTOHEADER;
	// 'l' use by flags
	// 'p' use by flags
	if (xflags[TEXT('P')]) // percentage
		iFlags |= DCX_LV_COLUMNF_PERCENT;
	// 'q' use by flags
	// 'r' use by flags
	// 's' use by flags
	if (xflags[TEXT('S')]) // max size (max of auto & header)
		iFlags |= DCX_LV_COLUMNF_AUTOMAX;

	return iFlags;
}

UINT DcxListView::parseSortFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags{};

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	if (xflags[TEXT('a')])
		iFlags |= LVSS_ASC;
	if (xflags[TEXT('c')])
		iFlags |= LVSS_CUSTOM;
	if (xflags[TEXT('d')])
		iFlags |= LVSS_DESC;
	if (xflags[TEXT('n')])
		iFlags |= LVSS_NUMERIC;
	if (xflags[TEXT('s')])
		iFlags |= LVSS_CASE;
	if (xflags[TEXT('t')])
		iFlags |= LVSS_ALPHA;
	if (xflags[TEXT('i')])
		iFlags |= LVSS_IRC;

	return iFlags;
}

UINT DcxListView::parseGroupFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags{};

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	if (xflags[TEXT('c')])
		iFlags |= LVGA_HEADER_CENTER;
	if (xflags[TEXT('l')])
		iFlags |= LVGA_HEADER_LEFT;
	if (xflags[TEXT('r')])
		iFlags |= LVGA_HEADER_RIGHT;

	return iFlags;
}

UINT DcxListView::parseGroupState(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags{};

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	if (xflags[TEXT('C')])
		iFlags |= LVGS_COLLAPSIBLE;
	if (xflags[TEXT('H')])
		iFlags |= LVGS_HIDDEN;
	if (xflags[TEXT('N')])
		iFlags |= LVGS_NOHEADER;
	if (xflags[TEXT('O')])
		iFlags |= LVGS_COLLAPSED;
	if (xflags[TEXT('S')])
		iFlags |= LVGS_SELECTED;
	if (xflags[TEXT('s')])
		iFlags |= LVGS_SUBSETED;
	//if (xflags[TEXT('L')])
	//	iFlags |= LVGS_SUBSETLINKFOCUSED;

	return iFlags;
}

UINT DcxListView::parseColorFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags{};

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	if (xflags[TEXT('b')])
		iFlags |= LVCS_BKG;
	if (xflags[TEXT('k')])
		iFlags |= LVCS_BKGTEXT;
	if (xflags[TEXT('o')])
		iFlags |= LVCS_OUTLINE;
	if (xflags[TEXT('t')])
		iFlags |= LVCS_TEXT;

	return iFlags;
}

UINT DcxListView::parseImageFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags{};

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	if (xflags[TEXT('n')])
		iFlags |= LVBKIF_STYLE_NORMAL;
	if (xflags[TEXT('r')])
		iFlags |= LVBKIF_SOURCE_NONE;
	if (xflags[TEXT('t')])
		iFlags |= LVBKIF_STYLE_TILE;
	if (xflags[TEXT('w')])
		iFlags |= LVBKIF_TYPE_WATERMARK;
	if (xflags[TEXT('a')])
		iFlags |= LVBKIF_TYPE_WATERMARK | LVBKIF_FLAG_ALPHABLEND;

	return iFlags;
}

bool DcxListView::isListViewStyle(const WindowStyle dwView) const noexcept
{
	return ((dcxGetWindowStyle(m_Hwnd) & LVS_TYPEMASK) == dwView);
}

GSL_SUPPRESS(bounds.4)
GSL_SUPPRESS(con.4)
GSL_SUPPRESS(r.5)
bool DcxListView::matchItemText(const int nItem, const int nSubItem, const TString& search, const DcxSearchTypes& SearchType) const
{
	const dcxSearchData srch_data(search, SearchType);

	return matchItemText(nItem, nSubItem, srch_data);
}

bool DcxListView::matchItemText(const int nItem, const int nSubItem, const dcxSearchData& srch_data) const
{
	//auto itemtext = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);
	//gsl::at(itemtext, 0) = TEXT('\0');
	//Dcx::dcxListView_GetItemText(m_Hwnd, nItem, nSubItem, itemtext.get(), MIRC_BUFFER_SIZE_CCH);
	//return DcxSearchHelper::matchItemText(itemtext.get(), srch_data);

	const auto tsItemText(Dcx::dcxListView_GetItemText(m_Hwnd, nItem, nSubItem));

	return DcxSearchHelper::matchItemText(tsItemText.to_chr(), srch_data);
}

const int& DcxListView::getColumnCount() const noexcept
{
	//if (m_iColumnCount < 0)
	{
		if (auto hHeader = Dcx::dcxListView_GetHeader(m_Hwnd); hHeader)
			m_iColumnCount = Dcx::dcxHeader_GetItemCount(hHeader);
		else
		{
			LVCOLUMN lvc{};
			lvc.mask = LVCF_WIDTH;

			auto i = 0;
			while (Dcx::dcxListView_GetColumn(m_Hwnd, i, &lvc) != FALSE)
				++i;

			m_iColumnCount = i;
		}
	}
	return m_iColumnCount;
}

void DcxListView::loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc)
{
	auto filename(tsSrc);
	auto index = tsIndex.to_int();
	const auto iFlags = parseIconFlagOptions(tsFlags);
	auto overlayindex = 0;

	// determine overlay index
	if (tsFlags.find(TEXT('o'), 0))
	{
		// overlay id offset
		const auto io = tsFlags.find(TEXT('o'), 1) + 1;
		overlayindex = tsFlags.mid(io, gsl::narrow_cast<int>(tsFlags.len() - io)).to_int();

		if (overlayindex < 1 || overlayindex > 15)
			throw Dcx::dcxException("Overlay index out of range (1 -> 15)");
	}

	// load both normal and small icons
	if (dcx_testflag(iFlags, LVSIL_SMALL))
	{
		// load normal icon
		if (const auto himl = this->initImageList(LVSIL_NORMAL); index < 0)
		{
			AddFileIcons(himl, filename, true, -1);
		}
		else {
#if DCX_USE_WRAPPERS
			const Dcx::dcxIconResource icon(index, filename, true, tsFlags);

			if (const auto i = ImageList_AddIcon(himl, icon.get()); overlayindex > 0)
				ImageList_SetOverlayImage(himl, i, overlayindex);
#else
			const HICON icon = dcxLoadIcon(index, filename, true, tflags);

			if (!icon)
				throw Dcx::dcxException("Unable to load normal icon");

			if (const int i = ImageList_AddIcon(himl, icon); overlayindex > 0)
				ImageList_SetOverlayImage(himl, i, overlayindex);

			DestroyIcon(icon);
#endif
		}

		// load small icon
		if (const auto himl = this->initImageList(LVSIL_SMALL); index < 0)
		{
			AddFileIcons(himl, filename, false, -1);
		}
		else {
#if DCX_USE_WRAPPERS
			const Dcx::dcxIconResource icon(index, filename, false, tsFlags);

			if (const auto i = ImageList_AddIcon(himl, icon.get()); overlayindex > 0)
				ImageList_SetOverlayImage(himl, i, overlayindex);
#else
			const HICON icon = dcxLoadIcon(index, filename, false, tsFlags);

			if (!icon)
				throw Dcx::dcxException("Unable to load small icon");

			if (const int i = ImageList_AddIcon(himl, icon); overlayindex > 0)
				ImageList_SetOverlayImage(himl, i, overlayindex);

			DestroyIcon(icon);
#endif
		}
	}

	// state icon
	if (dcx_testflag(iFlags, LVSIL_STATE))
	{
		if (const auto himl = this->initImageList(LVSIL_STATE); index < 0)
		{
			AddFileIcons(himl, filename, false, -1);
		}
		else {
#if DCX_USE_WRAPPERS
			const Dcx::dcxIconResource icon(index, filename, false, tsFlags);

			ImageList_AddIcon(himl, icon.get());
#else
			const HICON icon = dcxLoadIcon(index, filename, false, tsFlags);

			if (!icon)
				throw Dcx::dcxException("Unable to load state icon");

			ImageList_AddIcon(himl, icon);

			DestroyIcon(icon);
#endif
	}
}

	// footer icons
	if (dcx_testflag(iFlags, LVSIL_FOOTER))
	{
		if (const auto himl = this->initImageList(LVSIL_FOOTER); index < 0)
		{
			AddFileIcons(himl, filename, false, -1);
		}
		else {
#if DCX_USE_WRAPPERS
			const Dcx::dcxIconResource icon(index, filename, false, tsFlags);

			ImageList_AddIcon(himl, icon.get());
#else
			const HICON icon = dcxLoadIcon(index, filename, false, tflags);

			if (!icon)
				throw Dcx::dcxException("Unable to load footer icon");

			ImageList_AddIcon(himl, icon);

			DestroyIcon(icon);
#endif
	}
	}

	// group header icons
	if (dcx_testflag(iFlags, LVSIL_GROUPHEADER))
	{
		if (const auto himl = this->initImageList(LVSIL_GROUPHEADER); index < 0)
		{
			AddFileIcons(himl, filename, false, -1);
		}
		else {
#if DCX_USE_WRAPPERS
			const Dcx::dcxIconResource icon(index, filename, false, tsFlags);

			ImageList_AddIcon(himl, icon.get());
#else
			const HICON icon = dcxLoadIcon(index, filename, false, tflags);

			if (!icon)
				throw Dcx::dcxException("Unable to load footer icon");

			ImageList_AddIcon(himl, icon);

			DestroyIcon(icon);
#endif
	}
	}
}

int DcxListView::getTopIndex() const noexcept
{
	if (Dcx::dcxListView_GetItemCount(m_Hwnd) > 0)
		return Dcx::dcxListView_GetTopIndex(m_Hwnd);

	return -1;
}

int DcxListView::getBottomIndex() const noexcept
{
	auto nBottomIndex = (Dcx::dcxListView_GetTopIndex(m_Hwnd) + Dcx::dcxListView_GetCountPerPage(m_Hwnd));

	if (const auto nCount = Dcx::dcxListView_GetItemCount(m_Hwnd); (nBottomIndex > nCount))
		nBottomIndex = nCount;

	return --nBottomIndex;
}

int CALLBACK DcxListView::sortItemsEx(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	auto plvsort = reinterpret_cast<LPDCXLVSORT>(lParamSort);
	if (!plvsort)
		return 0;

	plvsort->itemtext1[0] = TEXT('\0');
	plvsort->itemtext2[0] = TEXT('\0');

	GSL_SUPPRESS(bounds.3) Dcx::dcxListView_GetItemText(plvsort->m_Hwnd, gsl::narrow_cast<int>(lParam1), plvsort->nColumn, &plvsort->itemtext1[0], gsl::narrow_cast<int>(std::size(plvsort->itemtext1)));
	GSL_SUPPRESS(bounds.3) Dcx::dcxListView_GetItemText(plvsort->m_Hwnd, gsl::narrow_cast<int>(lParam2), plvsort->nColumn, &plvsort->itemtext2[0], gsl::narrow_cast<int>(std::size(plvsort->itemtext2)));

	// CUSTOM Sort
	if (dcx_testflag(plvsort->iSortFlags, LVSS_CUSTOM))
	{
		if (plvsort->tsCustomAlias.empty())
			return 0;

		stString<20> sRes;

		// testing new sort call... new ver doesnt pass item text directly via alias, but instead sets a %var with the text & passes the %var name to the alias.
		// Should solve some item name issues.
		//mIRCLinker::execex(TEXT("/!set -nu1 %%dcx_1sort%d %s"), plvsort->itemtext1, plvsort->itemtext1 );
		//mIRCLinker::execex(TEXT("/!set -nu1 %%dcx_2sort%d %s"), plvsort->itemtext2, plvsort->itemtext2 );
		//mIRCLinker::evalex( sRes, static_cast<int>(sRes.size()), TEXT("$%s(%%dcx_1sort%d,%%dcx_2sort%d)"), plvsort->tsCustomAlias.to_chr( ), plvsort->itemtext1, plvsort->itemtext2 );
		////
		////mIRCLinker::evalex( sRes, Dcx::countof(sRes), TEXT("$%s(%s,%s)"), plvsort->tsCustomAlias.to_chr( ), itemtext1, itemtext2 );

		mIRCLinker::exec(TEXT("/!set -nu1 \\%dcx_1sort% %"), (LONG_PTR)&plvsort->itemtext1[0], &plvsort->itemtext1[0]);
		mIRCLinker::exec(TEXT("/!set -nu1 \\%dcx_2sort% %"), (LONG_PTR)&plvsort->itemtext2[0], &plvsort->itemtext2[0]);
		mIRCLinker::eval(sRes, TEXT("$%(\\%dcx_1sort%,\\%dcx_2sort%)"), plvsort->tsCustomAlias, (LONG_PTR)&plvsort->itemtext1[0], (LONG_PTR)&plvsort->itemtext2[0]);

		auto ires = _ts_atoi(sRes.data());

		if (ires < -1)
			ires = -1;
		else if (ires > 1)
			ires = 1;

		if (dcx_testflag(plvsort->iSortFlags, LVSS_DESC))
			return ires;
		else {

			if (ires == -1)
				return 1;
			else if (ires == 1)
				return -1;
		}
	}
	// NUMERIC Sort
	else if (dcx_testflag(plvsort->iSortFlags, LVSS_NUMERIC))
	{
		const auto i1 = _ts_atoi(&plvsort->itemtext1[0]);
		const auto i2 = _ts_atoi(&plvsort->itemtext2[0]);

		if (dcx_testflag(plvsort->iSortFlags, LVSS_DESC))
		{
			if (i1 < i2)
				return 1;
			else if (i1 > i2)
				return -1;
		}
		else {

			if (i1 < i2)
				return -1;
			else if (i1 > i2)
				return 1;
		}
	}
	// IRC Sort
	else if (dcx_testflag(plvsort->iSortFlags, LVSS_IRC))
	{
		const auto r1 = GetRank(plvsort->itemtext1[0]);
		const auto r2 = GetRank(plvsort->itemtext2[0]);

		if (dcx_testflag(plvsort->iSortFlags, LVSS_DESC))
		{
			if (r1 > r2)
				return -1;
			if (r1 < r2)
				return 1;
			if (dcx_testflag(plvsort->iSortFlags, LVSS_CASE))
				return -_ts_strcmp(&plvsort->itemtext1[0], &plvsort->itemtext2[0]);
			else
				return -_ts_stricmp(&plvsort->itemtext1[0], &plvsort->itemtext2[0]);
		}
		else {
			if (r1 > r2)
				return 1;
			if (r1 < r2)
				return -1;
			if (dcx_testflag(plvsort->iSortFlags, LVSS_CASE))
				return _ts_strcmp(&plvsort->itemtext1[0], &plvsort->itemtext2[0]);
			else
				return _ts_stricmp(&plvsort->itemtext1[0], &plvsort->itemtext2[0]);
		}
	}
	// Default ALPHA Sort
	else {

		if (dcx_testflag(plvsort->iSortFlags, LVSS_DESC))
		{
			if (dcx_testflag(plvsort->iSortFlags, LVSS_CASE))
				return -_ts_strcmp(&plvsort->itemtext1[0], &plvsort->itemtext2[0]);
			else
				return -_ts_stricmp(&plvsort->itemtext1[0], &plvsort->itemtext2[0]);
		}
		else {
			if (dcx_testflag(plvsort->iSortFlags, LVSS_CASE))
				return _ts_strcmp(&plvsort->itemtext1[0], &plvsort->itemtext2[0]);
			else
				return _ts_stricmp(&plvsort->itemtext1[0], &plvsort->itemtext2[0]);
		}
	}

	return 0;
}

GSL_SUPPRESS(con.4)
LRESULT DcxListView::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
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
			bParsed = TRUE;

			dcxlParam(LPNMITEMACTIVATE, nmia);
			LVHITTESTINFO lvh{ nmia->ptAction, 0U,0,0,0 };

			Dcx::dcxListView_SubItemHitTest(m_Hwnd, &lvh);

			const auto lvexstyles = Dcx::dcxListView_GetExtendedListViewStyle(m_Hwnd);

			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
				if (dcx_testflag(lvh.flags, LVHT_ONITEMSTATEICON) && dcx_testflag(lvexstyles, LVS_EX_CHECKBOXES) && !dcx_testflag(lvh.flags, LVHT_ONITEMICON) && !dcx_testflag(lvh.flags, LVHT_ONITEMLABEL))
				{
					//TODO: int state = ListView_GetCheckState(m_Hwnd, lvh.iItem);
					execAliasEx(TEXT("stateclick,%u,%d,%d"), getUserID(), lvh.iItem + 1, lvh.iSubItem + 1);
				}
				else if ((lvh.flags & LVHT_ONITEM) != 0)
					execAliasEx(TEXT("sclick,%u,%d,%d"), getUserID(), lvh.iItem + 1, lvh.iSubItem + 1);
				else if (dcx_testflag(lvh.flags, LVHT_NOWHERE))
					execAliasEx(TEXT("sclick,%u"), getUserID());
			}

			if (!dcx_testflag(lvexstyles, LVS_EX_FULLROWSELECT) && m_bSubItemSelect)
			{ // make subitem show as selected.

				if (dcx_testflag(dcxGetWindowStyle(m_Hwnd), WindowStyle::LVS_SingleSelect) || (!dcx_testflag(nmia->uKeyFlags, LVKF_CONTROL) && !dcx_testflag(nmia->uKeyFlags, LVKF_SHIFT)))
				{
					Dcx::dcxListView_SetItemState(m_Hwnd, -1, 0, LVIS_SELECTED);
					Dcx::dcxListView_SetSubItemListState(m_Hwnd, m_SubItemsSelected, 0, LVIS_SELECTED);
					m_SubItemsSelected.clear();
				}

				// select new
				Dcx::dcxListView_SetSubItemState(m_Hwnd, lvh.iItem, lvh.iSubItem, LVIS_SELECTED, LVIS_SELECTED);

				m_SubItemsSelected.emplace_back(lvh.iItem, lvh.iSubItem);
			}
		}
		break;

		case NM_DBLCLK:
		{
			bParsed = TRUE;

			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
				dcxlParam(LPNMITEMACTIVATE, nmia);
				LVHITTESTINFO lvh{ nmia->ptAction, 0U,0,0,0 };

				if (Dcx::dcxListView_SubItemHitTest(m_Hwnd, &lvh) == -1)
					execAliasEx(TEXT("dclick,%u"), getUserID());
				else
					execAliasEx(TEXT("dclick,%u,%d,%d"), getUserID(), lvh.iItem + 1, lvh.iSubItem + 1);
			}
		}
		break;

		case NM_RCLICK:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
				dcxlParam(LPNMITEMACTIVATE, nmia);
				LVHITTESTINFO lvh{ nmia->ptAction, 0U,0,0,0 };

				if (Dcx::dcxListView_SubItemHitTest(m_Hwnd, &lvh) == -1)
					execAliasEx(TEXT("rclick,%u"), getUserID());
				else
					execAliasEx(TEXT("rclick,%u,%d,%d"), getUserID(), lvh.iItem + 1, lvh.iSubItem + 1);
			}
			bParsed = TRUE;
			return TRUE;
		}
		break;

		case NM_RDBLCLK:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
				dcxlParam(LPNMITEMACTIVATE, nmia);
				LVHITTESTINFO lvh{ nmia->ptAction, 0U,0,0,0 };

				if (Dcx::dcxListView_SubItemHitTest(m_Hwnd, &lvh) == -1)
					execAliasEx(TEXT("rdclick,%u"), getUserID());
				else
					execAliasEx(TEXT("rdclick,%u,%d,%d"), getUserID(), lvh.iItem + 1, lvh.iSubItem + 1);
			}
			bParsed = TRUE;
		}
		break;

		case NM_HOVER:
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
				if (LVHITTESTINFO lvh{ Dcx::dcxListView_CursorHitTest(m_Hwnd) }; lvh.flags & LVHT_ONITEM)
					execAliasEx(TEXT("hover,%u,%d,%d"), getUserID(), lvh.iItem + 1, lvh.iSubItem + 1);
				else
					execAliasEx(TEXT("hover,%u"), getUserID());
			}
			bParsed = TRUE;
		}
		break;

		case LVN_BEGINLABELEDIT:
		{
			bParsed = TRUE;
			dcxlParam(const LPNMLVDISPINFO, lplvdi);

			if (!lplvdi)
				break;

			Dcx::dcxListView_SetItemState(m_Hwnd, lplvdi->item.iItem, LVIS_SELECTED, LVIS_SELECTED);

			if (auto edit_hwnd = Dcx::dcxListView_GetEditControl(m_Hwnd); edit_hwnd)
			{
				m_OrigEditProc = SubclassWindow(edit_hwnd, DcxListView::EditLabelProc);
				Dcx::dcxSetProp(edit_hwnd, TEXT("dcx_pthis"), this);
			}

			const stString<256> sRet;
			evalAliasEx(sRet, gsl::narrow_cast<int>(sRet.size()), TEXT("labelbegin,%u,%d,%d"), getUserID(), lplvdi->item.iItem + 1, lplvdi->item.iSubItem + 1);
			//evalAlias(sRet, gsl::narrow_cast<int>(sRet.size()), TEXT("labelbegin,%,%,%"), getUserID(), lplvdi->item.iItem + 1, lplvdi->item.iSubItem + 1);

			if (sRet == TEXT("noedit"))
				return TRUE;

		}
		break;

		case LVN_ENDLABELEDIT:
		{
			bParsed = TRUE;

			dcxlParam(const LPNMLVDISPINFO, lplvdi);

			if (!lplvdi)
				break;

			if (!lplvdi->item.pszText)
				execAliasEx(TEXT("labelcancel,%u,%d,%d"), getUserID(), lplvdi->item.iItem + 1, lplvdi->item.iSubItem + 1);
			else {
				const stString<256> sRet;
				evalAliasEx(sRet, gsl::narrow_cast<int>(sRet.size()), TEXT("labelend,%u,%d,%d,%s"), getUserID(), lplvdi->item.iItem + 1, lplvdi->item.iSubItem + 1, lplvdi->item.pszText);
				//evalAlias(sRet, gsl::narrow_cast<int>(sRet.size()), TEXT("labelend,%,%"), getUserID(), lplvdi->item.pszText);

				if (sRet == TEXT("noedit"))
					return FALSE;
			}
			return TRUE;
		}
		break;

		case LVN_COLUMNOVERFLOWCLICK:
		{
			dcxlParam(LPNMLISTVIEW, lpml);
			//dcxlParam(LPNMITEMACTIVATE, lpml);

			bParsed = TRUE;

			if (!lpml)
				break;

			execAliasEx(TEXT("columnoverflow,%u,%d,%d"), getUserID(), lpml->iItem, lpml->iSubItem);
		}
		break;

		case NM_CUSTOMDRAW:
		{
			dcxlParam(LPNMLVCUSTOMDRAW, lplvcd);

			bParsed = TRUE;

			if (!lplvcd)
				return CDRF_DODEFAULT;

			if (lplvcd->dwItemType == LVCDI_GROUP)
				return DrawGroup(lplvcd);
			else
				return DrawItem(lplvcd);
		}
		break;

		case LVN_DELETEALLITEMS:
		{
			bParsed = TRUE;
			return FALSE; // make sure we get an LVN_DELETEITEM for each item.
		}
		break;

		case LVN_DELETEITEM:
		{
			dcxlParam(LPNMLISTVIEW, lpnmlv);

			if (!lpnmlv)
				break;

			if (auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lpnmlv->lParam); lpdcxlvi)
			{
				if (lpdcxlvi->pbar)
					DestroyWindow(lpdcxlvi->pbar->getHwnd());

				//for (auto& x : lpdcxlvi->vInfo)
				//	delete x;
				//
				//lpdcxlvi->vInfo.clear();

				delete lpdcxlvi;
			}
			bParsed = TRUE; // message has been handled.
		}
		break;

		// handle dragging items...
		case LVN_BEGINDRAG:
		{
			if (!m_bAllowDrag)
				break;

			if (dcx_testflag(getEventMask(), DCX_EVENT_DRAG))
				execAliasEx(TEXT("begindrag,%u"), getUserID()); // allow blocking the drag?

			// You can set your customized cursor here

			POINT p{ 8,8 };
			IMAGEINFO imf{};
			LONG iHeight{};

			//DragDetect();

			// Ok, now we create a drag-image for all selected items
			bool bFirst = true;
			//int iPos = Dcx::dcxListView_GetNextItem(m_Hwnd, -1, LVNI_SELECTED);
			//while (iPos != -1)
			//{
			//	if (bFirst)
			//	{
			//		// For the first selected item,
			//		// we simply create a single-line drag image
			//		Dcx::m_hDragImage = Dcx::dcxListView_CreateDragImage(m_Hwnd, iPos, &p);
			//		if (!Dcx::m_hDragImage)
			//			break;
			//
			//		ImageList_GetImageInfo(Dcx::m_hDragImage, 0, &imf);
			//		iHeight = imf.rcImage.bottom;
			//		bFirst = false;
			//	}
			//	else {
			//		// For the rest selected items,
			//		// we create a single-line drag image, then
			//		// append it to the bottom of the complete drag image
			//		if (auto hOneImageList = Dcx::dcxListView_CreateDragImage(m_Hwnd, iPos, &p); hOneImageList)
			//		{
			//			if (auto hTempImageList = ImageList_Merge(Dcx::m_hDragImage, 0, hOneImageList, 0, 0, iHeight); hTempImageList)
			//			{
			//				ImageList_Destroy(Dcx::m_hDragImage);
			//				Dcx::m_hDragImage = hTempImageList;
			//				if (!Dcx::m_hDragImage)
			//					break;
			//			}
			//			ImageList_Destroy(hOneImageList);
			//		}
			//		ImageList_GetImageInfo(Dcx::m_hDragImage, 0, &imf);
			//		iHeight = imf.rcImage.bottom;
			//	}
			//	iPos = Dcx::dcxListView_GetNextItem(m_Hwnd, iPos, LVNI_SELECTED);
			//}

			for (auto iPos = Dcx::dcxListView_GetNextItem(m_Hwnd, -1, LVNI_SELECTED); (iPos != -1); iPos = Dcx::dcxListView_GetNextItem(m_Hwnd, iPos, LVNI_SELECTED))
			{
				if (bFirst)
				{
					// For the first selected item,
					// we simply create a single-line drag image
					Dcx::m_hDragImage = Dcx::dcxListView_CreateDragImage(m_Hwnd, iPos, &p);
					if (!Dcx::m_hDragImage)
						break;

					bFirst = false;
				}
				else {
					if (!Dcx::m_hDragImage)
						break;

					// For the rest selected items,
					// we create a single-line drag image, then
					// append it to the bottom of the complete drag image
					if (auto hOneImageList = Dcx::dcxListView_CreateDragImage(m_Hwnd, iPos, &p); hOneImageList)
					{
						if (auto hTempImageList = ImageList_Merge(Dcx::m_hDragImage, 0, hOneImageList, 0, 0, iHeight); hTempImageList)
						{
							ImageList_Destroy(Dcx::m_hDragImage);
							Dcx::m_hDragImage = hTempImageList;
							if (!Dcx::m_hDragImage)
								break;
						}
						ImageList_Destroy(hOneImageList);
					}
				}
				if (Dcx::m_hDragImage)
					ImageList_GetImageInfo(Dcx::m_hDragImage, 0, &imf);
				iHeight = imf.rcImage.bottom;
			}

			if (Dcx::m_hDragImage)
			{
				// Now we can initialize then start the drag action
				ImageList_BeginDrag(Dcx::m_hDragImage, 0, 0, 0);

				POINT pt = ((NM_LISTVIEW*)((LPNMHDR)lParam))->ptAction;
				MapWindowPoints(m_Hwnd, nullptr, &pt, 1);

				ImageList_DragEnter(GetDesktopWindow(), pt.x, pt.y);

				Dcx::m_bShowingDragImage = true;
				Dcx::m_pDragSourceCtrl = this;

				// Don't forget to capture the mouse
				if (mIRCLinker::m_mIRCHWND)
					SetCapture(mIRCLinker::m_mIRCHWND);
			}
		}
		break;

		//case LVN_ENDSCROLL:
		// {
		//	 if (this->isExStyle(LVS_EX_GRIDLINES)) {
		//		 mIRCError(TEXT("scroll"));
		//		 //this->redrawWindow();
		//	 }
		//	 break;
		// }
		//		 4294967296-max

		case LVN_KEYDOWN:
		{
			const auto pnkd = reinterpret_cast<LPNMLVKEYDOWN>(lParam);
			if (!pnkd)
				break;

			const auto wVKey = pnkd->wVKey;
			const stString<15> cb;

			evalAliasEx(cb, gsl::narrow_cast<int>(cb.size()), TEXT("keydown,%u,%d"), getUserID(), wVKey);

			// space to change checkbox state
			if ((wVKey == 32) && dcx_testflag(Dcx::dcxListView_GetExtendedListViewStyle(m_Hwnd), LVS_EX_CHECKBOXES))
			{
				// stop it from allowing user to change checkstate by pressing spacebar
				if (cb == TEXT("nospace"))
				{
					bParsed = TRUE;
					return TRUE;
				}

				const auto index = Dcx::dcxListView_GetNextItem(m_Hwnd, -1, LVNI_FOCUSED);

				// TODO: twig: change this if we get multiple checkbox columns working
				evalAliasEx(cb, gsl::narrow_cast<int>(cb.size()), TEXT("stateclick,%u,%d,1"), getUserID(), index + 1);
			}
		}
		break;

		//case LVN_CHANGING: // 4294967196
		//case LVN_CHANGED: // 4294967195
		//	NM_FIRST
		//	LVN_FIRST

		case LVN_ITEMCHANGED:	// no return value
		{
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
				const auto pnmv = reinterpret_cast<const LPNMLISTVIEW>(lParam);
				if (!pnmv || pnmv->iItem == -1)
					break;

				if (dcx_testflag(pnmv->uChanged, LVIF_STATE))
				{
					//if (dcx_testflag(pnmv->uNewState, LVIS_SELECTED) && !dcx_testflag(pnmv->uOldState, LVIS_SELECTED))
					//	execAliasEx(TEXT("selected,%u,%d,%d"), getUserID( ), pnmv->iItem +1, pnmv->iSubItem +1);
					//else if (!dcx_testflag(pnmv->uNewState, LVIS_SELECTED) && dcx_testflag(pnmv->uOldState, LVIS_SELECTED))
					//	execAliasEx(TEXT("deselected,%u,%d,%d"), getUserID( ), pnmv->iItem +1, pnmv->iSubItem +1);

					if (dcx_testflag(pnmv->uNewState, LVIS_SELECTED))
					{
						if (!dcx_testflag(pnmv->uOldState, LVIS_SELECTED))
							execAliasEx(TEXT("selected,%u,%d,%d"), getUserID(), pnmv->iItem + 1, pnmv->iSubItem + 1);
					}
					else
					{
						if (dcx_testflag(pnmv->uOldState, LVIS_SELECTED))
							execAliasEx(TEXT("deselected,%u,%d,%d"), getUserID(), pnmv->iItem + 1, pnmv->iSubItem + 1);
					}
				}
				//bParsed = TRUE;
			}
		}
		break;

		// same as dclick
	//case LVN_ITEMACTIVATE:	// MUST return zero
	//{
	//	if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK)) {
	//		const dcxlParam(LPNMITEMACTIVATE, na);
	//		if (na->iItem == -1)
	//			break;
//
					//		execAliasEx(TEXT("activated,%u,%d,%d"), getUserID(), na->iItem + 1, na->iSubItem + 1);
					//	}
					//}
					//break;

		case LVN_GETINFOTIP:
		{
			const auto pGetInfoTip = reinterpret_cast<const LPNMLVGETINFOTIP>(lParam);
			if (!pGetInfoTip)
				break;

			// Get item information.
			if (LVITEM lvi{ LVIF_PARAM, pGetInfoTip->iItem, pGetInfoTip->iSubItem }; Dcx::dcxListView_GetItem(m_Hwnd, &lvi))
			{
				// return tooltip text, if any.
				if (auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lvi.lParam); !lpdcxlvi->tsTipText.empty())
					pGetInfoTip->pszText = lpdcxlvi->tsTipText.to_chr();
				bParsed = TRUE;
			}
		}
		break;

		case LVN_GETEMPTYMARKUP:
		{
			const auto pnmMarkup = reinterpret_cast<NMLVEMPTYMARKUP*>(lParam);
			if (!pnmMarkup)
				break;

			if (const TString szBuf(TGetWindowText(m_Hwnd)); !szBuf.empty())
			{
				pnmMarkup->dwFlags = EMF_CENTERED;

				_ts_strcpyn(&pnmMarkup->szMarkup[0], szBuf.to_chr(), std::size(pnmMarkup->szMarkup));

				bParsed = TRUE;
				return TRUE;
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

LRESULT DcxListView::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_CONTEXTMENU:
		break;

	case WM_COMMAND:
	{
		if (IsWindow(to_hwnd(lParam)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(lParam, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_COMPAREITEM:
	{
		if (dcxlParam(LPCOMPAREITEMSTRUCT, idata); ((idata) && (IsWindow(idata->hwndItem))))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(lParam, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_DELETEITEM:
	{
		if (dcxlParam(LPDELETEITEMSTRUCT, idata); ((idata) && (IsWindow(idata->hwndItem))))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(lParam, TEXT("dcx_cthis")); c_this)
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
		if (dcxlParam(LPDRAWITEMSTRUCT, idata); ((idata) && (IsWindow(idata->hwndItem))))
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
		if (!bParsed)
		{
			switch (hdr->code)
			{
			case NM_RCLICK:
			{
				if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
				{
					if (Dcx::dcxListView_GetHeader(m_Hwnd) == hdr->hwndFrom)
					{
						if (HDHITTESTINFO hdti{}; GetCursorPos(&hdti.pt))
						{
							MapWindowPoints(nullptr, hdr->hwndFrom, &hdti.pt, 1);
							if (Dcx::dcxHeader_HitTest(hdr->hwndFrom, &hdti) != -1)
								execAliasEx(TEXT("hrclick,%u,%d"), getUserID(), hdti.iItem + 1);
						}
					}
				}
				bParsed = TRUE;
				return TRUE;
			}
			break;
			case HDN_BEGINTRACK:
			{
				bParsed = TRUE;

				dcxlParam(LPNMHEADER, pHeader);
				if (!pHeader)
					break;

				//TCHAR ret[256];
				//evalAliasEx( ret, Dcx::countof(ret), TEXT("trackbegin,%u,%d"), getUserID(), pHeader->iItem +1);
				//
				//if (ts_strcmp(TEXT("notrack"), ret) == 0)
				//	return TRUE;

				const stString<256> sRet;
				evalAliasEx(sRet, gsl::narrow_cast<int>(sRet.size()), TEXT("trackbegin,%u,%d"), getUserID(), pHeader->iItem + 1);

				if (sRet == TEXT("notrack"))
					return TRUE;
			}
			break;

			case HDN_ITEMCLICK:
			{
				bParsed = TRUE;

				if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
				{
					dcxlParam(LPNMHEADER, lphdr);

					switch (lphdr->iButton)
					{
					case 0: // left click
					{
						// code allows changing the sort up/down display by clicking on a header
						HDITEM tmphdr{};
						tmphdr.mask = HDI_FORMAT;
						if (Dcx::dcxHeader_GetItem(lphdr->hdr.hwndFrom, lphdr->iItem, &tmphdr))
						{
							if (dcx_testflag(tmphdr.fmt, HDF_SORTDOWN))
							{
								tmphdr.fmt &= ~HDF_SORTDOWN;
								tmphdr.fmt |= HDF_SORTUP;
							}
							else if (dcx_testflag(tmphdr.fmt, HDF_SORTUP))
							{
								tmphdr.fmt &= ~HDF_SORTUP;
								tmphdr.fmt |= HDF_SORTDOWN;
							}
							else if ((tmphdr.fmt & (HDF_IMAGE | HDF_BITMAP)) == 0)
								tmphdr.fmt |= HDF_SORTDOWN;
							if ((tmphdr.fmt & (HDF_SORTUP | HDF_SORTDOWN)) > 0)
								Dcx::dcxHeader_SetItem(lphdr->hdr.hwndFrom, lphdr->iItem, &tmphdr);
							execAliasEx(TEXT("hsclick,%u,%d,%d"), getUserID(), lphdr->iItem + 1, tmphdr.fmt);
						}
						else
							execAliasEx(TEXT("hsclick,%u,%d"), getUserID(), lphdr->iItem + 1);
					}
					break;
					case 1: // right click (never triggers)
						execAliasEx(TEXT("hrclick,%u,%d"), getUserID(), lphdr->iItem + 1);
						break;
					case 2: // middle click (never triggers)
						execAliasEx(TEXT("hmclick,%u,%d"), getUserID(), lphdr->iItem + 1);
						break;
					default:
						break;
					}
				}
			}
			break;

			case HDN_ITEMDBLCLICK:
			{
				bParsed = TRUE;

				if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
				{
					dcxlParam(LPNMHEADER, lphdr);
					if (!lphdr)
						break;

					execAliasEx(TEXT("hdclick,%u,%d"), getUserID(), lphdr->iItem + 1);
				}
			}
			break;
			case HDN_DROPDOWN:
			{
				bParsed = TRUE;

				if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
				{
					dcxlParam(LPNMHEADER, lphdr);
					if (!lphdr)
						break;

					execAliasEx(TEXT("hdropdown,%u,%d,%d"), getUserID(), lphdr->iItem + 1, lphdr->iButton);
				}
			}
			break;
			// LVN_GETTOOLTIP/TTN_GETDISPINFO/TTN_LINKCLICK fail....
			//case LVN_GETINFOTIP:
			//	{
			//	}
			//	break;
			//case TTN_GETDISPINFO:
			//	{
			//		LPNMTTDISPINFO di = (LPNMTTDISPINFO)lParam;
			//		LVHITTESTINFO hti;
			//		GetCursorPos( &hti.pt );
			//		ScreenToClient( m_Hwnd, &hti.pt );
			//		ZeroMemory(&hti,sizeof(LVHITTESTINFO));
			//		hti.flags = LVHT_ONITEM;
			//		if (ListView_SubItemHitTest(m_Hwnd,&hti) != -1) {
			//			if (hti.flags & LVHT_ONITEM) {
			//				LVITEM lvi;
			//				ZeroMemory(&lvi,sizeof(LVITEM));
			//				lvi.mask = LVIF_PARAM;
			//				lvi.iItem = hti.iItem;
			//				lvi.iSubItem = hti.iSubItem;
			//				if (ListView_GetItem(m_Hwnd,&lvi)) {
			//					LPDCXLVITEM dci = (LPDCXLVITEM) lvi.lParam;
			//					if (dci != nullptr)
			//						di->lpszText = dci->tsTipText.to_chr();
			//				}
			//			}
			//		}
			//		//di->lpszText = this->m_tsToolTip.to_chr();
			//		di->hinst = nullptr;
			//		bParsed = TRUE;
			//	}
			//	break;
			//case TTN_LINKCLICK:
			//	{
			//		bParsed = TRUE;
			//		this->execAliasEx(TEXT("%s,%d"), TEXT("tooltiplink"), this->getUserID( ) );
			//	}
			//	break;
			default:
				break;

			} // switch
		}
	}
	break;

	//case WM_MOUSEMOVE:
	//{
	//	lRes = CommonMessage(uMsg, wParam, lParam, bParsed);
	//}
	//break;

	case WM_HSCROLL:
	case WM_VSCROLL:
	{
		if (IsWindow(to_hwnd(lParam)))
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(lParam, TEXT("dcx_cthis")); c_this)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			else {

				if (Dcx::dcxLOWORD(wParam) == SB_ENDSCROLL)
					execAliasEx(TEXT("scrollend,%u"), getUserID());

				//switch (LOWORD(wParam))
				//{
				//case SB_ENDSCROLL:
				//	execAliasEx(TEXT("scrollend,%u"), getUserID());
				//case SB_PAGEUP:
				//case SB_PAGEDOWN:
				//	UpdateScrollPbars();
				//	break;
				//	//default:
				//	   // int iTop = getTopIndex() - 1;
				//	   // int iBottom = getBottomIndex() + 1;
				//	   // if (iTop >= 0)
				//		  //  ScrollPbars(iTop);
				//	   // if ((iBottom >= 0) && (iTop != iBottom))
				//		  //  ScrollPbars(iBottom);
				//}

				if (dcx_testflag(Dcx::dcxListView_GetExtendedListViewStyle(m_Hwnd), LVS_EX_GRIDLINES))
					redrawWindow();
			}
		}
		DrawMargin();
		break;
	}

	case WM_PAINT:
	{
		//if (!IsAlphaBlend())
		//	break;
		//
		//PAINTSTRUCT ps{};
		//auto hdc = BeginPaint(m_Hwnd, &ps);
		//Auto(EndPaint(m_Hwnd, &ps));
		//
		//bParsed = TRUE;
		//
		//// Setup alpha blend if any.
		//auto ai = SetupAlphaBlend(&hdc);
		//Auto(FinishAlphaBlend(ai));
		//
		//lRes = CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);

		//if (IsControlCodeTextEnabled())
		//{
		//	// only allow this rendering if control code text is enabled atm (may change)
		//	if (Dcx::dcxListView_GetItemCount(m_Hwnd) <= 0)
		//	{
		//		if (GetWindowTextLength(m_Hwnd) > 0)
		//		{
		//			if (const TString tsBuf(TGetWindowText(m_Hwnd)); !tsBuf.empty())
		//			{
		//				bParsed = TRUE;
		//
		//				if (!wParam)
		//				{
		//					PAINTSTRUCT ps{};
		//					auto hdc = BeginPaint(m_Hwnd, &ps);
		//					Auto(EndPaint(m_Hwnd, &ps));
		//
		//					DrawEmpty(hdc, tsBuf);
		//				}
		//				else {
		//					DrawEmpty(reinterpret_cast<HDC>(wParam), tsBuf);
		//				}
		//			}
		//		}
		//
		//	}
		//}
		//
		//if (!bParsed && IsAlphaBlend())
		//{
		//	bParsed = TRUE;
		//
		//	if (!wParam)
		//	{
		//		PAINTSTRUCT ps{};
		//		auto hdc = BeginPaint(m_Hwnd, &ps);
		//		Auto(EndPaint(m_Hwnd, &ps));
		//
		//		{
		//			// Setup alpha blend if any.
		//			auto ai = SetupAlphaBlend(&hdc);
		//			Auto(FinishAlphaBlend(ai));
		//
		//			lRes = CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
		//
		//			DrawMargin(hdc);
		//		}
		//	}
		//	else {
		//		auto hdc = reinterpret_cast<HDC>(wParam);
		//
		//		// Setup alpha blend if any.
		//		auto ai = SetupAlphaBlend(&hdc);
		//		Auto(FinishAlphaBlend(ai));
		//
		//		lRes = CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
		//
		//		DrawMargin(hdc);
		//	}
		//}

		bParsed = TRUE;
		if (!wParam)
		{
			//PAINTSTRUCT ps{};
			//auto hdc = BeginPaint(m_Hwnd, &ps);
			//Auto(EndPaint(m_Hwnd, &ps));
			//
			//lRes = CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
			//
			//DrawClientArea(hdc);

			lRes = CallDefaultClassProc(uMsg, wParam, lParam);

			DrawClientArea();
		}
		else {
			lRes = CallDefaultClassProc(uMsg, wParam, lParam);

			DrawClientArea(reinterpret_cast<HDC>(wParam));
		}
	}
	break;

	case WM_ERASEBKGND:
	{
		lRes = CallDefaultClassProc(uMsg, wParam, lParam);

		DrawMargin(reinterpret_cast<HDC>(wParam));

		bParsed = TRUE;

		// trying to fix flicker when drawing margin, partial? success
		//auto hdc = reinterpret_cast<HDC>(wParam);
		//auto ai = SetupAlphaBlend(&hdc, true);
		//Auto(FinishAlphaBlend(ai));
		//
		//lRes = CallDefaultClassProc(uMsg, (WPARAM)hdc, lParam);
		//
		//DrawMargin(hdc);
		//
		//bParsed = TRUE;
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
		lRes = CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}

LRESULT CALLBACK DcxListView::EditLabelProc(gsl::not_null<HWND> mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto* const pthis = Dcx::dcxGetProp<DcxListView*>(mHwnd.get(), TEXT("dcx_pthis"));

	if (!pthis)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);
	if (!pthis->m_OrigEditProc)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	switch (uMsg)
	{
	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;

	case WM_DESTROY:
	{
		RemoveProp(mHwnd, TEXT("dcx_pthis"));
		SubclassWindow(mHwnd, pthis->m_OrigEditProc);
	}
	break;
	default:
		break;
	}
	return CallWindowProc(pthis->m_OrigEditProc, mHwnd, uMsg, wParam, lParam);
}

DcxControl* DcxListView::CreatePbar(LPLVITEM lvi, const TString& styles)
{
	// can only create progress for an existing item
	if (!lvi || lvi->lParam == 0)
		return nullptr;

	const auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lvi->lParam);

	// check if control already exists.
	if (lpdcxlvi->pbar)
		return nullptr;

	RECT rItem{};

	// initial rect for pbar
	if (lvi->iSubItem == 0)
		Dcx::dcxListView_GetItemRect(m_Hwnd, lvi->iItem, &rItem, LVIR_LABEL);
	else
		Dcx::dcxListView_GetSubItemRect(m_Hwnd, lvi->iItem, lvi->iSubItem, LVIR_LABEL, &rItem);

	lpdcxlvi->iPbarCol = lvi->iSubItem;
	// controls within a listview have a problem in that they cant set an item height,
	// so they all appear very small, & dont look very good. (this can maybe be solved within NM_CUSTOMDRAW prepaint stage)
	const auto tsID(styles.getfirsttok(1));
	//const auto ID = mIRC_ID_OFFSET + tsID.to_<UINT>();

	//if (!this->getParentDialog()->isIDValid(ID, true))
	//	throw Dcx::dcxException(TEXT("Control with ID \"%\" already exists"), ID - mIRC_ID_OFFSET);

	try {
		// this method allows named id's and the possibility of adding other control types...
		TString ctrl_args;
		// pbar/ipaddress/button/image/panel only version
		//<id> <type> <x y w h> <styles>
		const TString tsType(styles.getnexttok());	// tok 2
		//ctrl_args.tsprintf(TEXT("%s %s %d %d %d %d %s"), tsID.to_chr(), tsType.to_chr(), rItem.left, rItem.top, (rItem.right - rItem.left), (rItem.bottom - rItem.top), styles.getlasttoks().to_chr());	// tok 3-
		_ts_sprintf(ctrl_args, TEXT("% % % % % % %"), tsID, tsType, rItem.left, rItem.top, (rItem.right - rItem.left), (rItem.bottom - rItem.top), styles.getlasttoks());	// tok 3-
		lpdcxlvi->pbar = getParentDialog()->addControl(ctrl_args, 1, DcxAllowControls::ALLOW_PBAR | DcxAllowControls::ALLOW_IPADDRESS | DcxAllowControls::ALLOW_BUTTON | DcxAllowControls::ALLOW_IMAGE | DcxAllowControls::ALLOW_PANEL, m_Hwnd);

		//// pbar only version
		////<id> pbar <x y w h> <styles>
		//ctrl_args.tsprintf(TEXT("%s pbar %d %d %d %d %s"), tsID.to_chr(), rItem.left, rItem.top, (rItem.right - rItem.left), (rItem.bottom - rItem.top), styles.getlasttoks().to_chr());
		//lpdcxlvi->pbar = this->getParentDialog()->addControl(ctrl_args,1,DcxAllowControls::ALLOW_PBAR,m_Hwnd);

		////lpdcxlvi->pbar = new DcxProgressBar(this->getID(), this->m_pParentDialog, m_Hwnd, &rItem, styles);
		//lpdcxlvi->pbar = new DcxProgressBar(ID, this->m_pParentDialog, m_Hwnd, &rItem, styles.getlasttoks());
		//this->getParentDialog()->addControl(lpdcxlvi->pbar);

		m_bHasPBars = true;

		return lpdcxlvi->pbar;
	}
	catch (const std::exception& e)
	{
		showError(nullptr, TEXT("CreatePbar()"), TEXT("Unable To Create Control % (%)"), tsID, e.what());
		throw;
	}
}


void DcxListView::UpdateScrollPbars()
{
	if (!m_bHasPBars || !m_Hwnd)
		return;

	const auto nCount = Dcx::dcxListView_GetItemCount(m_Hwnd);
	const auto nCols = getColumnCount();
	const auto iTop = getTopIndex();
	const auto iBottom = getBottomIndex() + 1;

	auto lvi = std::make_unique<LVITEM>();

	ZeroMemory(lvi.get(), sizeof(LVITEM));

	for (auto row = decltype(nCount){0}; row < nCount; ++row)
		ScrollPbars(row, nCols, iTop, iBottom, lvi.get());
}

// BUG: when listview has horiz scrollbars pbar will be moved oddly when listview is scrolled horiz.
//			pbars are positioned relative to visible area of control & as such arn't scrolled.
void DcxListView::ScrollPbars(const int row, const int nCols, const int iTop, const int iBottom, LPLVITEM lvi) noexcept
{
	for (auto col = decltype(nCols){0}; col < nCols; ++col)
	{
		lvi->iItem = row;
		lvi->iSubItem = col;
		lvi->mask = LVIF_PARAM;

		if (!Dcx::dcxListView_GetItem(m_Hwnd, lvi))
			continue;

		const auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lvi->lParam);

		if (!lpdcxlvi)
			continue;

		if (!lpdcxlvi->pbar)
			continue;

		// isnt the right column to move it to
		if (lpdcxlvi->iPbarCol != col)
			continue;

		// hide it if its scrolled off visible range
		//if (!ListView_IsItemVisible(m_Hwnd, lvi->iItem)) {
		//if ((lvi->iItem < iTop) || (lvi->iItem > iBottom))
		//{
		//	ShowWindow(lpdcxlvi->pbar->getHwnd(), SW_HIDE);
		//	break;
		//}
		//else
		//	ShowWindow(lpdcxlvi->pbar->getHwnd(), SW_SHOW);

		if (Dcx::dcxListView_IsItemVisible(m_Hwnd, lvi->iItem))
			ShowWindow(lpdcxlvi->pbar->getHwnd(), SW_SHOW);

		RECT rItem{};

		// get coordinates to move to
		if (col == 0)
			Dcx::dcxListView_GetItemRect(m_Hwnd, lvi->iItem, &rItem, LVIR_LABEL);
		else
			Dcx::dcxListView_GetSubItemRect(m_Hwnd, lvi->iItem, lvi->iSubItem, LVIR_LABEL, &rItem);

		// show borders correctly
		//rItem.bottom--;
		//++rItem.top;
		//++rItem.left;
		//rItem.right--;

		// Ook: testing a workaround for controls being drawn over headers
		if (auto hHeader = Dcx::dcxListView_GetHeader(m_Hwnd); IsWindowVisible(hHeader))
		{
			if (RECT rcClient{}, rcHeader{}; (GetClientRect(m_Hwnd, &rcClient) && GetWindowRectParent(hHeader, &rcHeader)))
			{
				rcClient.top += (rcHeader.bottom - rcHeader.top);
				if (rItem.top <= rcClient.top)
				{
					ShowWindow(lpdcxlvi->pbar->getHwnd(), SW_HIDE);
					break;
				}
			}
		}

		//RECT rcWin;
		//if (GetWindowRect(lpdcxlvi->pbar->getHwnd(), &rcWin))
		//{
		//	MapWindowRect(nullptr, m_Hwnd, &rcWin);
		//	if (!EqualRect(&rcWin, &rItem)) {
		//		MoveWindow(lpdcxlvi->pbar->getHwnd(),
		//			rItem.left, rItem.top, (rItem.right - rItem.left), (rItem.bottom - rItem.top),
		//			FALSE);
		//		InvalidateRect(lpdcxlvi->pbar->getHwnd(), nullptr, TRUE);
		//	}
		//}

		if (RECT rcWin{}; GetWindowRectParent(lpdcxlvi->pbar->getHwnd(), &rcWin))
		{
			if (!EqualRect(&rcWin, &rItem))
			{
				MoveWindow(lpdcxlvi->pbar->getHwnd(),
					rItem.left, rItem.top, (rItem.right - rItem.left), (rItem.bottom - rItem.top),
					FALSE);
				InvalidateRect(lpdcxlvi->pbar->getHwnd(), nullptr, TRUE);
			}
		}
		break;
	}
}

/*
	xmlLoadListview()
	Loads items into a listview control from a dcxml file.
*/
bool DcxListView::xmlLoadListview(const int nPos, const TString& dataset, TString& filename)
{
	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("Unable To Access File: %"), filename);

	TiXmlDocument doc(filename.c_str());
	doc.SetCondenseWhiteSpace(false);

	const auto xmlFile = doc.LoadFile();
	if (!xmlFile)
		throw Dcx::dcxException(TEXT("Not an XML File: %"), filename);

	const auto* const xRoot = doc.FirstChildElement("dcxml");
	if (!xRoot)
		throw Dcx::dcxException("Unable Find 'dcxml' root");

	const auto* xElm = xRoot->FirstChildElement("listview_data");
	if (!xElm)
		throw Dcx::dcxException("Unable To Find 'listview_data' element");

	xElm = xElm->FirstChildElement(dataset.c_str());
	if (!xElm)
		throw Dcx::dcxException(TEXT("Unable To Find Dataset: %"), dataset);

	Auto(this->redrawWindow());

	return xmlLoadListview(nPos, xElm);
}

bool DcxListView::xmlLoadListview(const int nPos, const TiXmlElement* xElm)
{
	//TString tsBuf((UINT)MIRC_BUFFER_SIZE_CCH);
	//int nItem{ nPos };
	//
	//if (nPos < 1)
	//	nItem = Dcx::dcxListView_GetItemCount(m_Hwnd);
	//
	//for (const auto* xNode = xElm->FirstChildElement("item"); xNode; xNode = xNode->NextSiblingElement("item"))
	//{
	//	TString tsText(queryAttribute(xNode, "text"));
	//	const TiXmlElement *xCtrl{};
	//
	//	if (xCtrl = xNode->FirstChildElement("control"); xCtrl)
	//	{
	//		const auto iX = queryIntAttribute(xCtrl, "x");
	//		const auto iY = queryIntAttribute(xCtrl, "y");
	//		const auto iWidth = queryIntAttribute(xCtrl, "width");
	//		const auto iHeight = queryIntAttribute(xCtrl, "height");
	//		TString tsID(queryAttribute(xCtrl, "id"));
	//		auto szType = queryAttribute(xCtrl, "type");
	//		auto szStyles = queryAttribute(xCtrl, "styles");
	//
	//		// ID is NOT a number!
	//		if (tsID.empty()) // no id, generate one.
	//			tsID.addtok(getParentDialog()->getUniqueID());
	//
	//		_ts_sprintf(tsText, TEXT("% % % % % % %"), tsID, szType, iX, iY, iWidth, iHeight, szStyles);
	//	}
	//	_ts_sprintf(tsBuf, L"dname id -a % % % % % % % % % %",
	//		nItem,
	//		queryIntAttribute(xNode, "indent"),
	//		queryAttribute(xNode, "flags","+"),
	//		queryIntAttribute(xNode, "icon"),
	//		queryIntAttribute(xNode, "stateicon", -1),
	//		queryIntAttribute(xNode, "overlayicon"),
	//		queryIntAttribute(xNode, "groupid"),
	//		queryColourAttribute(xNode, "textcolour"),
	//		queryColourAttribute(xNode, "bgcolour"),
	//		tsText);
	//
	//	for (const auto* xSubNode = xNode->FirstChildElement("subitem"); xSubNode; xSubNode = xSubNode->NextSiblingElement("subitem"))
	//	{
	//		TString tsSub;
	//		TString tsSubText(queryAttribute(xSubNode, "text"));
	//		if (xCtrl = xSubNode->FirstChildElement("control"); xCtrl)
	//		{
	//			const auto iX = queryIntAttribute(xCtrl, "x");
	//			const auto iY = queryIntAttribute(xCtrl, "y");
	//			const auto iWidth = queryIntAttribute(xCtrl, "width");
	//			const auto iHeight = queryIntAttribute(xCtrl, "height");
	//			TString tsID(queryAttribute(xCtrl, "id"));
	//			auto szType = queryAttribute(xCtrl, "type");
	//			auto szStyles = queryAttribute(xCtrl, "styles");
	//
	//			// ID is NOT a number!
	//			if (tsID.empty()) // no id, generate one.
	//				tsID.addtok(getParentDialog()->getUniqueID());
	//
	//			_ts_sprintf(tsSubText, TEXT("% % % % % % %"), tsID, szType, iX, iY, iWidth, iHeight, szStyles);
	//		}
	//		_ts_sprintf(tsSub, L"\t% % % % % %",
	//			queryAttribute(xSubNode, "flags", "+"),
	//			queryIntAttribute(xSubNode, "icon"),
	//			queryIntAttribute(xSubNode, "overlayicon"),
	//			queryColourAttribute(xSubNode, "textcolour"),
	//			queryColourAttribute(xSubNode, "bgcolour"),
	//			tsSubText);
	//
	//		tsBuf += tsSub;
	//	}
	//	//xdid -a [NAME] [ID] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
	//	//[N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
	//	if (!tsBuf.empty())
	//	{
	//		Command_a(tsBuf);
	//		++nItem;
	//	}
	//}
	//return true;

	TString tsBuf(gsl::narrow_cast<TString::size_type>(MIRC_BUFFER_SIZE_CCH));
	int nItem{ nPos };

	if (nPos < 1)
		nItem = Dcx::dcxListView_GetItemCount(m_Hwnd);

	if (nItem == 0)
		nItem = 1;

	for (const auto* xNode = xElm->FirstChildElement("item"); xNode; xNode = xNode->NextSiblingElement("item"))
	{
		TString tsText(queryAttribute(xNode, "text"));
		const TiXmlElement* xCtrl{};

		if (auto xtmpCtrl = xNode->FirstChildElement("control"); xtmpCtrl)
		{
			const auto iX = queryIntAttribute(xtmpCtrl, "x");
			const auto iY = queryIntAttribute(xtmpCtrl, "y");
			const auto iWidth = queryIntAttribute(xtmpCtrl, "width");
			const auto iHeight = queryIntAttribute(xtmpCtrl, "height");
			TString tsID(queryAttribute(xtmpCtrl, "id"));
			auto szType = queryAttribute(xtmpCtrl, "type");
			auto szStyles = queryAttribute(xtmpCtrl, "styles");

			// ID is NOT a number!
			if (tsID.empty()) // no id, generate one.
				tsID.addtok(getParentDialog()->getUniqueID());

			_ts_sprintf(tsText, TEXT("% % % % % % %"), tsID, szType, iX, iY, iWidth, iHeight, szStyles);

			xCtrl = xtmpCtrl;
		}
		_ts_sprintf(tsBuf, L"dname id -a % % % % % % % % % %",
			nItem,
			queryIntAttribute(xNode, "indent"),
			queryAttribute(xNode, "flags", "+"),
			queryIntAttribute(xNode, "icon"),
			queryIntAttribute(xNode, "stateicon", -1),
			queryIntAttribute(xNode, "overlayicon"),
			queryIntAttribute(xNode, "group"),
			queryColourAttribute(xNode, "textcolour"),
			queryColourAttribute(xNode, "bgcolour"),
			tsText);

		for (const auto* xSubNode = xNode->FirstChildElement("subitem"); xSubNode; xSubNode = xSubNode->NextSiblingElement("subitem"))
		{
			TString tsSub;
			TString tsSubText(queryAttribute(xSubNode, "text"));
			if (auto xtmpCtrl = xSubNode->FirstChildElement("control"); xtmpCtrl)
			{
				const auto iX = queryIntAttribute(xtmpCtrl, "x");
				const auto iY = queryIntAttribute(xtmpCtrl, "y");
				const auto iWidth = queryIntAttribute(xtmpCtrl, "width");
				const auto iHeight = queryIntAttribute(xtmpCtrl, "height");
				TString tsID(queryAttribute(xtmpCtrl, "id"));
				auto szType = queryAttribute(xtmpCtrl, "type");
				auto szStyles = queryAttribute(xtmpCtrl, "styles");

				// ID is NOT a number!
				if (tsID.empty()) // no id, generate one.
					tsID.addtok(getParentDialog()->getUniqueID());

				_ts_sprintf(tsSubText, TEXT("% % % % % % %"), tsID, szType, iX, iY, iWidth, iHeight, szStyles);

				xCtrl = xtmpCtrl;
			}
			_ts_sprintf(tsSub, L"\t% % % % % %",
				queryAttribute(xSubNode, "flags", "+"),
				queryIntAttribute(xSubNode, "icon"),
				queryIntAttribute(xSubNode, "overlayicon"),
				queryColourAttribute(xSubNode, "textcolour"),
				queryColourAttribute(xSubNode, "bgcolour"),
				tsSubText);

			tsBuf += tsSub;
		}
		//xdid -a [NAME] [ID] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
		//[N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
		if (!tsBuf.empty())
		{
			Command_a(tsBuf);

			if (xCtrl)
			{
				LVITEM lvi{};
				lvi.mask = LVIF_PARAM;
				lvi.iItem = nItem - 1;

				Dcx::dcxListView_GetItem(m_Hwnd, &lvi);

				if (LPDCXLVITEM lpmylvi = reinterpret_cast<LPDCXLVITEM>(lvi.lParam); lpmylvi)
				{
					if (lpmylvi->pbar)
						lpmylvi->pbar->fromXml(xElm, xCtrl);
				}
			}
			++nItem;
		}
	}
	return true;
}

bool DcxListView::xmlSaveListview(const int nPosStart, int iPosEnd, const TString& dataset, TString& filename) const
{
	TiXmlDocument doc(filename.c_str());
	doc.SetCondenseWhiteSpace(false);

	if (IsFile(filename))
	{
		if (!doc.LoadFile())
			throw Dcx::dcxException(TEXT("Not an XML File: %"), filename);
	}
	// first get or create dcxml item
	auto xRoot = doc.FirstChildElement("dcxml");
	if (!xRoot)
	{
		xRoot = dynamic_cast<TiXmlElement*>(doc.InsertEndChild(TiXmlElement("dcxml")));

		if (!xRoot)
			throw Dcx::dcxException("Unable To Add Root <dcxml>");
	}
	// get or create listview_data item
	auto xData = xRoot->FirstChildElement("listview_data");
	if (!xData)
	{
		xData = dynamic_cast<TiXmlElement*>(xRoot->InsertEndChild(TiXmlElement("listview_data")));
		if (!xData)
			throw Dcx::dcxException("Unable to add <listview_data> item");
	}
	// get or create dataset item
	auto xDataset = xRoot->FirstChildElement(dataset.c_str());
	if (!xDataset)
	{
		xDataset = dynamic_cast<TiXmlElement*>(xRoot->InsertEndChild(TiXmlElement(dataset.c_str())));
		if (!xDataset)
			throw Dcx::dcxException("Unable to add <dataset> item");
	}
	xDataset->Clear();

	// save current setup
	xmlSaveListview(nPosStart, iPosEnd, xDataset);

	// save to file.
	return doc.SaveFile();
}

bool DcxListView::xmlSaveListview(const int nPosStart, int iPosEnd, TiXmlElement* xElm) const
{
	if (!m_Hwnd || !xElm)
		return false;

	xElm->SetAttribute("version", DCXML_DIALOG_VERSION);

	const auto nTotal = Dcx::dcxListView_GetItemCount(m_Hwnd);
	if (nTotal == 0)
		return true;

	const auto nCols = this->getColumnCount();

	if ((iPosEnd >= 0) && (iPosEnd < nPosStart))
		iPosEnd = nPosStart;

	if ((iPosEnd < 0) || (iPosEnd > nTotal))
		iPosEnd = nTotal;

	for (int n = nPosStart; n < iPosEnd; ++n)
	{
		if (auto xml = ItemToXml(n, nCols); xml)
			xElm->LinkEndChild(xml);
	}
	return true;
}

//[N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] +flags dialog id (N|N1,N2)
bool DcxListView::ctrlLoadListview(const int nPos, const TString& tsData) noexcept
{
	//auto tsFlags(tsData.gettok( 10 ));
	//auto dialogname(tsData.gettok( 11 ).trim());
	//auto ctrl_ID = tsData.gettok( 12 ).to_int();
	//auto tsItem(tsData.gettok( 13 ).trim());
	//
	//if (tsFlags[0] != TEXT('+')) {
	//	showErrorEx(nullptr, TEXT("-a"), TEXT("Invalid flags specified, missing +: %s Only DCX dialogs are supported."), dialogname.to_chr());
	//	return false;
	//}
	//auto p_Dialog = Dcx::Dialogs.getDialogByName(dialogname);
	//if (p_Dialog == nullptr) {
	//	showErrorEx(nullptr, TEXT("-a"), TEXT("Invalid dialog name: %s Only DCX dialogs are supported."), dialogname.to_chr());
	//	return false;
	//}
	//auto p_Control = p_Dialog->getControlByID((UINT) ctrl_ID + mIRC_ID_OFFSET);
	//
	//if (p_Control == nullptr) {
	//	showErrorEx(nullptr, TEXT("-a"), TEXT("Invalid control id: %d Only DCX controls are supported."), ctrl_ID);
	//	return false;
	//}
	////mIRCLinker::evalex();
	return false;
}

//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
//tsData = [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] [+flags] [window/table] [item]
bool DcxListView::xLoadListview(const int nPos, const TString& tsData, const TCHAR* sTest, const TCHAR* sCount, const TCHAR* sGet, const TCHAR* sGetNamed)
{
	TString tsRes;	// used to store the data returned by mIRC.
	const auto tsflags(tsData.getfirsttok(10));		// tok 10
	const auto tsName(tsData.getnexttok());			// tok 11
	const auto tsItem(tsData.getlasttoks());		// tok 12, -1

	// check table/window exists
	//mIRCLinker::tsEvalex(tsRes, sTest, tsName.to_chr());
	mIRCLinker::eval(tsRes, sTest, tsName);

	// if not exit
	if (tsName != tsRes)
		throw Dcx::dcxException(TEXT("Invalid hashtable/window: %"), tsName);

	// get the total number of items in the table.
	//mIRCLinker::tsEvalex(tsRes, sCount, tsName.to_chr());
	mIRCLinker::eval(tsRes, sCount, tsName);
	const auto iTotal = tsRes.to_<UINT>();
	// if no items then exit.
	if (iTotal == 0)
		return false;

	// convert the flags string to a bin mask
	const auto iFlags = DcxListView::parseMassItemFlags(tsflags);
	auto iStart = 0, iEnd = 0;	// the first & last items in the hash table to add.
	TString input;	// the input string thats sent to the add item function.
	// The string end up having alot of space holders (0's), these are needed so the function can work with a normal xdid -a too.

	if (dcx_testflag(iFlags, LVIMF_NAMED))
	{
		if (!sGetNamed)
			throw Dcx::dcxException("Invalid flag used, +i is for hashtable items only");

		// add a single named item
		//mIRCLinker::tsEvalex(tsRes, sGetNamed, tsName.to_chr(), tsItem.to_chr());
		mIRCLinker::eval(tsRes, sGetNamed, tsName, tsItem);
		if (dcx_testflag(iFlags, LVIMF_ALLINFO))
			// add items data from [INDENT] onwards is taken from hashtable, including subitems.
			//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
			//   0     0     0      0   read from hashtable->
			//input.sprintf(TEXT("0 0 0 0 %s"),tsRes);
		{
			input = TEXT("0 0 0 0 ");
			input += tsRes;
		}
		else
			// only the item text is taken from the hashtable.
			//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
			//   0     0     0      0      0       +       0        0         0           0        0        0		read from hashtable->
			//input.sprintf(TEXT("0 0 0 0 0 + 0 0 0 0 0 0 %s"), tsRes);
		{
			parseText2Item(tsRes, input, tsData);
		}
		// add this item
		massSetItem(nPos, input);
		return true;
	}
	else if (iFlags == 0)
	{
		// no flags, just add text to nPos with no extras
		iStart = iEnd = tsItem.to_int();
		// first & last item are the same.
	}
	else if (dcx_testflag(iFlags, LVIMF_ADDALL))
	{
		// add all items from N onwards inclusive
		iStart = tsItem.to_int();	// first item to add
		iEnd = gsl::narrow_cast<int>(iTotal);			// last item to add is the last item in table.
	}
	else if (dcx_testflag(iFlags, LVIMF_NUMERIC))
	{
		// numeric range supplied.
		iStart = tsItem.getfirsttok(1, TSCOMMACHAR).to_int();	// first item in hash table to be added taken from range.
		iEnd = tsItem.getnexttok(TSCOMMACHAR).to_int();	// last item in hash table to be added taken from range.	tok 2
		if (iEnd < 0)
			iEnd = gsl::narrow_cast<int>(iTotal + iEnd);		// if iEnd is a negative number then make iEnd the last item in table + iEnd
	}
	// iStart & iEnd MUST be 1 or greater. (zero gives a hashtable/window item count.)
	if ((iStart < 1) || (iEnd < 1))
		throw Dcx::dcxException(TEXT("Invalid numeric supplied: %"), tsItem);

	this->setRedraw(FALSE); // disable redraw while adding lots of items.

	Auto(this->setRedraw(TRUE));

	//int nItem = nPos;	// tmp var use to update the item pos for each item added.

	for (auto nItem = nPos; iStart <= iEnd; ++iStart)
	{
		// get items data
		//mIRCLinker::tsEvalex(tsRes, sGet, tsName.to_chr(), iStart);
		mIRCLinker::eval(tsRes, sGet, tsName, iStart);
		if (dcx_testflag(iFlags, LVIMF_ALLINFO))
			// add items data from [INDENT] onwards is taken from hashtable, including subitems.
			//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
			//   0     0     0      0   read from hashtable->
			//input.sprintf(TEXT("0 0 0 0 %s"),tsRes);
		{
			input = TEXT("0 0 0 0 ");
			input += tsRes;
		}
		else
			// only the item text is taken from the hashtable.
			//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
			//   0     0     0      0      0       +       0        0         0           0        0        0		read from hashtable->
			//input.sprintf(TEXT("0 0 0 0 0 + 0 0 0 0 0 0 %s"), tsRes);
		{
			parseText2Item(tsRes, input, tsData);
		}
		massSetItem(nItem++, input);
	}

	return true;
}

/*
	*	Used by xdid -a to parse data & add items & subitems
	*	Used by (hash/win/ctrl)LoadListView() functions to parse data & add items & subitems
*/

void DcxListView::massSetItem(const int nPos, const TString& input)
{
	auto data(input.gettok(1, TSTABCHAR).gettok(4, -1).trim());

	const auto indent = data.getfirsttok(2).to_<int>();			// tok 2
	auto stateFlags = this->parseItemFlags(data++);				// tok 3
	auto icon = data++.to_<int>() - 1;							// tok 4
	const auto state = data++.to_<int>();						// tok 5
	auto overlay = data++.to_<int>();							// tok 6
	const auto group = data++.to_<int>();						// tok 7
	//auto clrText = data.getnexttokas<COLORREF>();			// tok 8
	//auto clrBack = data.getnexttokas<COLORREF>();			// tok 9
	auto clrText = data++.to_<COLORREF>();						// tok 8
	auto clrBack = data++.to_<COLORREF>();						// tok 9

	if (Dcx::dcxListView_GetItemCount(m_Hwnd) <= 0)
		InvalidateRect(m_Hwnd, nullptr, TRUE);

	auto lpmylvi = std::make_unique<DCXLVITEM>();
	{
		//auto ri = std::make_unique<DCXLVRENDERINFO>();
		DCXLVRENDERINFO ri{};

		lpmylvi->iPbarCol = 0;
		lpmylvi->pbar = nullptr;
		lpmylvi->vInfo.clear();

		// setup colum zero
		ri.m_dFlags = stateFlags;
		if (dcx_testflag(stateFlags, LVIS_COLOR))
			ri.m_cText = clrText;

		if (dcx_testflag(stateFlags, LVIS_BGCOLOR))
			ri.m_cBg = clrBack;

		//lpmylvi->vInfo.push_back(ri.release());
		//lpmylvi->vInfo.push_back(ri);
		lpmylvi->vInfo.emplace_back(ri);
	}
	TString itemtext;
	if (data.numtok() > 9U)
	{
		itemtext = data.getlasttoks();		// tok 10, -1

		//if (dcx_testflag(stateFlags, LVIS_HASHITEM))
		//	mIRCLinker::tsEvalex(itemtext, TEXT("$hget(%s,%s)"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
		//else if (dcx_testflag(stateFlags, LVIS_HASHNUMBER))
		//	mIRCLinker::tsEvalex(itemtext,  TEXT("$hget(%s,%s).data"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
		if (dcx_testflag(stateFlags, LVIS_HASHITEM))
			mIRCLinker::eval(itemtext, TEXT("$hget(%,%)"), itemtext.getfirsttok(1), itemtext.getnexttok());
		else if (dcx_testflag(stateFlags, LVIS_HASHNUMBER))
			mIRCLinker::eval(itemtext, TEXT("$hget(%,%).data"), itemtext.getfirsttok(1), itemtext.getnexttok());
	}

	LVITEM lvi{};

	lvi.mask = LVIF_PARAM | LVIF_STATE | LVIF_GROUPID | LVIF_IMAGE;
	lvi.iItem = nPos;
	lvi.iSubItem = 0;
	lvi.state = (stateFlags & 0xFFFF); // mask out higher number flags. These flags cause the add to fail & arnt needed here anyway.
	lvi.stateMask = (LVIS_FOCUSED | LVIS_SELECTED | LVIS_CUT | LVIS_DROPHILITED); // only alter the controls flags, ignore our custom ones.
	lvi.iImage = I_IMAGECALLBACK; // NB: using I_IMAGENONE causes a gap to be left for the icon for some reason. Using I_IMAGECALLBACK doesn't do this.
	//lvi.lParam = reinterpret_cast<LPARAM>(lpmylvi.get());
	lvi.lParam = Dcx::numeric_cast<LPARAM>(lpmylvi.get());
	lvi.iGroupId = I_GROUPIDNONE;	// set item as belonging to NO group by default.
	// otherwise if group flag isnt set I_GROUPIDCALLBACK is assumed.

	if (icon > -1)
		lvi.iImage = icon;

	// LVIF_IMAGE; // when this is a set & iImage is -1 the parent does the drawing (& in this case does nothing so no icon)
					// this fixes the icon being drawn even when no icon set.
					// leave this separate for now untill it's been well tested.

	// groups not reportview only.
	if (group > 0)
	{
		if (!Dcx::dcxListView_HasGroup(m_Hwnd, group))
			throw Dcx::dcxException(TEXT("Invalid Group specified: %"), group);

		lvi.iGroupId = group;
	}

	if (indent > 0)
	{
		lvi.mask |= LVIF_INDENT;
		lvi.iIndent = indent;
	}

	// set text in case of pbar
	if (!dcx_testflag(stateFlags, LVIS_PBAR))
	{
		lvi.mask |= LVIF_TEXT;
		lvi.pszText = itemtext.to_chr();
	}

	lvi.iItem = Dcx::dcxListView_InsertItem(m_Hwnd, &lvi);

	if (lvi.iItem == -1)
		throw Dcx::dcxException("Unable to add item");

	auto tmp_lpmylvi = lpmylvi.release();

	// create pbar for first column
	if (dcx_testflag(stateFlags, LVIS_PBAR))
		CreatePbar(&lvi, itemtext);	// tok 1, -1

	// subitems
	const auto tabs = input.numtok(TSTABCHAR);

	if (tabs > 1)
	{
		// ADD check for num columns
		for (auto i = decltype(tabs){2}; i <= tabs; ++i)
		{
			data = input.gettok(Dcx::numeric_cast<int>(i), TSTABCHAR).trim();
			const auto nToks = data.numtok();

			if (nToks < 5)
				throw Dcx::dcxException("Invalid subitem syntax");

			stateFlags = parseItemFlags(data++);				// tok 1
			icon = data++.to_<int>() - 1;						// tok 2
			overlay = data++.to_<int>();						// tok 3
			clrText = data++.to_<COLORREF>();					// tok 4
			clrBack = data++.to_<COLORREF>();					// tok 5

			// setup colum #
			{
				//auto ri = std::make_unique<DCXLVRENDERINFO>();
				DCXLVRENDERINFO ri{};

				ri.m_dFlags = stateFlags;

				ri.m_cText = (dcx_testflag(stateFlags, LVIS_COLOR) ? clrText : CLR_INVALID);

				ri.m_cBg = (dcx_testflag(stateFlags, LVIS_BGCOLOR) ? clrBack : CLR_INVALID);

				//tmp_lpmylvi->vInfo.push_back(ri.release());
				//tmp_lpmylvi->vInfo.push_back(ri);
				tmp_lpmylvi->vInfo.emplace_back(ri);
			}
			lvi.iSubItem = Dcx::numeric_cast<int>(i) - 1;
			lvi.mask = LVIF_TEXT | LVIF_IMAGE;

			// icon
			//lvi.mask |= LVIF_IMAGE; // moved here to turn off icon when none is wanted.
			if (icon > -1)
				lvi.iImage = icon;
			else
				lvi.iImage = I_IMAGECALLBACK; // NB: using I_IMAGENONE causes a gap to be left for the icon for some reason. Using I_IMAGECALLBACK doesn't do this.

			// overlay
			if (overlay > 0)
			{
				lvi.mask |= LVIF_STATE;
				lvi.state |= INDEXTOOVERLAYMASK(overlay);
				lvi.stateMask |= LVIS_OVERLAYMASK;
			}
			else
				lvi.state |= INDEXTOOVERLAYMASK(0);

			itemtext.clear();	// = TEXT("");
			if (nToks > 5)
			{
				itemtext = data.getlasttoks();	// tok 6, -1

				//if ((dcx_testflag(stateFlags, LVIS_HASHITEM)) && (itemtext.numtok() == 2))
				//	mIRCLinker::tsEvalex(itemtext, TEXT("$hget(%s,%s)"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
				//else if ((dcx_testflag(stateFlags, LVIS_HASHNUMBER)) && (itemtext.numtok() == 2))
				//	mIRCLinker::tsEvalex(itemtext,  TEXT("$hget(%s,%s).data"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
				if ((dcx_testflag(stateFlags, LVIS_HASHITEM)) && (itemtext.numtok() == 2))
					mIRCLinker::eval(itemtext, TEXT("$hget(%,%)"), itemtext.getfirsttok(1), itemtext.getnexttok());
				else if ((dcx_testflag(stateFlags, LVIS_HASHNUMBER)) && (itemtext.numtok() == 2))
					mIRCLinker::eval(itemtext, TEXT("$hget(%,%).data"), itemtext.getfirsttok(1), itemtext.getnexttok());
			}
			// create pbar for subitem
			if (dcx_testflag(stateFlags, LVIS_PBAR))
			{
				CreatePbar(&lvi, itemtext);
				itemtext.clear();	// = TEXT("");
			}

			lvi.pszText = itemtext.to_chr();
			Dcx::dcxListView_SetItem(m_Hwnd, &lvi);

			this->autoSize(gsl::narrow_cast<int>(i - 1), data.gettok(1));
		}
	}

	if (state > -1)
		Dcx::dcxListView_SetItemState(m_Hwnd, lvi.iItem, gsl::narrow_cast<UINT>(INDEXTOSTATEIMAGEMASK(state)), LVIS_STATEIMAGEMASK);

	// overlay is 1-based index, max 15 overlay icons
	if (overlay > 0 && overlay < 16)
		Dcx::dcxListView_SetItemState(m_Hwnd, lvi.iItem, gsl::narrow_cast<UINT>(INDEXTOOVERLAYMASK(overlay)), LVIS_OVERLAYMASK);

	this->autoSize(0, input.gettok(6));
}

void DcxListView::parseText2Item(const TString& tsTxt, TString& tsItem, const TString& tsData)
{
	// first part of item, fill in blanks with 0's
	//tsItem = TEXT("0 0 0 0 0 + 0 0 0 0 0 0 ");
	//// add item text.
	//tsItem += tsTxt.gettok(1, TSTAB);
	//// add all subitems
	//int tok = 2, ntok = tsTxt.numtok(TSTAB);
	//while (tok <= ntok) {
	//	//[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text
	//	// again fill in blanks with 0's
	//	tsItem.addtok(TEXT("+ 0 0 0 0 "),TSTAB);
	//	// add subitems text.
	//	tsItem += tsTxt.gettok(tok, TSTAB);
	//	tok++;
	//}

	tsItem = TEXT("0 0 0 0 ");
	tsItem += tsData.gettok(2, 9); // copy flags & icon etc.. from /xdid -a line.
	// add item text.
	tsItem.addtok(tsTxt.getfirsttok(1, TSTABCHAR));
	// add all subitems

	for (auto tsTok(tsTxt.getnexttok(TSTABCHAR)); !tsTok.empty(); tsTok = tsTxt.getnexttok(TSTABCHAR))
	{
		//[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text
		// again fill in blanks with 0's
		tsItem.addtok(TEXT("+ 0 0 0 0 "), TSTABCHAR); // subitems are added without flags, not going to change this.
		// add subitems text.
		tsItem += tsTok;
	}
}

//void DcxListView::getItemRange(const TString &tsItems, const int nItemCnt, int *iStart_range, int *iEnd_range)
//{
////	int iStart, iEnd;
////	if (tsItems.numtok(TEXT('-')) == 2) {
////		iStart = tsItems.getfirsttok(1, TEXT('-')).to_int() -1;
////		iEnd = tsItems.getnexttok(TEXT('-')).to_int() -1;
////
////		if (iEnd == -1)	// special case
////			iEnd = nItemCnt -1;
////	}
////	else {
////		iEnd = tsItems.to_int() -1;
////
////		if (iEnd == -1)	// special case
////			iStart = iEnd = nItemCnt -1;
////		else
////			iStart = iEnd;
////	}
////	*iStart_range = iStart;
////	*iEnd_range = iEnd;
//
//	std::tie(*iStart_range, *iEnd_range) = getItemRange(tsItems, nItemCnt);
//}
//
//std::pair<int, int> DcxListView::getItemRange(const TString &tsItems, const int nItemCnt)
//{
//	int iStart, iEnd;
//	if (tsItems.numtok(TEXT('-')) == 2) {
//		iStart = tsItems.getfirsttok(1, TEXT('-')).to_int() - 1;
//		iEnd = tsItems.getnexttok(TEXT('-')).to_int() - 1;
//
//		if (iEnd == -1)	// special case
//			iEnd = nItemCnt - 1;
//	}
//	else {
//		iEnd = tsItems.to_int() - 1;
//
//		if (iEnd == -1)	// special case
//			iStart = iEnd = nItemCnt - 1;
//		else
//			iStart = iEnd;
//	}
//	return std::make_pair(iStart, iEnd);
//}

void DcxListView::DeleteColumns(const int nColumn) noexcept
{
	if (nColumn == -1)
	{
		// delete all columns
		if (auto nCol = this->getColumnCount(); nCol > 0)
		{
			while (--nCol > 0)
				Dcx::dcxListView_DeleteColumn(m_Hwnd, nCol);
		}
		this->m_iColumnCount = 0;
	}
	else {
		// delete specific column
		Dcx::dcxListView_DeleteColumn(m_Hwnd, nColumn);
		this->m_iColumnCount--;
	}
}

TString DcxListView::ItemToString(int nItem, int iColumns) const
{
	TString res;

	auto sTextBuffer = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);

	LVITEM lvitem{};

	gsl::at(sTextBuffer, 0) = TEXT('\0');

	lvitem.mask = LVIF_GROUPID | LVIF_IMAGE | LVIF_INDENT | LVIF_STATE | LVIF_TEXT | LVIF_PARAM;
	lvitem.iItem = nItem;
	lvitem.iSubItem = 0;
	lvitem.pszText = sTextBuffer.get();
	lvitem.cchTextMax = MIRC_BUFFER_SIZE_CCH;

	if (Dcx::dcxListView_GetItem(m_Hwnd, &lvitem))
	{
		// [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text{ TAB }[+FLAGS][#ICON][#OVERLAY][COLOR][BGCOLOR] Item Text ...
		res.addtok(lvitem.iIndent);		// INDENT
		res.addtok(TEXT('+'));			// +flags
		COLORREF bgclr = CLR_NONE;
		COLORREF fgclr = CLR_NONE;
		LPDCXLVITEM lpmylvi = reinterpret_cast<LPDCXLVITEM>(lvitem.lParam);
		if (lpmylvi)
		{
			if (!lpmylvi->vInfo.empty())
			{
				//if (auto ri = gsl::at(lpmylvi->vInfo, 0); ri)
				{
					const auto& ri = gsl::at(lpmylvi->vInfo, 0);

					bgclr = ri.m_cBg;
					fgclr = ri.m_cText;

					res += parseItemFlags(ri.m_dFlags);

					//if (dcx_testflag(ri.m_dFlags, LVIS_COLOR))
					//	res += TEXT('c');
					//if (dcx_testflag(ri.m_dFlags, LVIS_BGCOLOR))
					//	res += TEXT('k');
					//if (dcx_testflag(ri.m_dFlags, LVIS_BOLD))
					//	res += TEXT('b');
					//if (dcx_testflag(ri.m_dFlags, LVIS_CENTERICON))
					//	res += TEXT('C');
					//if (dcx_testflag(ri.m_dFlags, LVIS_ITALIC))
					//	res += TEXT('i');
					//if (dcx_testflag(ri.m_dFlags, LVIS_UNDERLINE))
					//	res += TEXT('u');
				}
			}
		}

		res.addtok(lvitem.iImage + 1);	// ICON
		res.addtok(lvitem.state);		// state
		res.addtok(TEXT('0'));			// overlay
		res.addtok(lvitem.iGroupId);	// group

		res.addtok(fgclr);			// colour
		res.addtok(bgclr);			// bgcolour

		if (lvitem.pszText[0] != TEXT('\0'))
			res.addtok(lvitem.pszText);	// item text

		for (auto nSubItem = 1; nSubItem < iColumns; ++nSubItem)
		{
			gsl::at(sTextBuffer, 0) = TEXT('\0');

			lvitem.iSubItem = nSubItem;
			lvitem.cchTextMax = MIRC_BUFFER_SIZE_CCH;
			lvitem.pszText = sTextBuffer.get();
			lvitem.mask = LVIF_IMAGE | LVIF_TEXT;

			if (Dcx::dcxListView_GetItem(m_Hwnd, &lvitem))
			{
				// { TAB }[+FLAGS][#ICON][#OVERLAY][COLOR][BGCOLOR] Item Text ...
				res += TEXT('\t');
				res.addtok(TEXT('+'));			// +flags

				bgclr = CLR_NONE;
				fgclr = CLR_NONE;
				if (lpmylvi)
				{
					if (nSubItem < gsl::narrow_cast<int>(std::size(lpmylvi->vInfo)))
					{
						//if (auto ri = gsl::at(lpmylvi->vInfo, gsl::narrow_cast<size_t>(nSubItem)); ri)
						{
							const auto& ri = gsl::at(lpmylvi->vInfo, gsl::narrow_cast<size_t>(nSubItem));
							bgclr = ri.m_cBg;
							fgclr = ri.m_cText;

							res += parseItemFlags(ri.m_dFlags);
						}
					}
				}

				res.addtok(lvitem.iImage + 1);	// ICON
				res.addtok(TEXT('0'));			// overlay

				res.addtok(fgclr);			// colour
				res.addtok(bgclr);			// bgcolour

				if (lvitem.pszText[0] != TEXT('\0'))
					res.addtok(lvitem.pszText);	// item text
			}
		}
	}
	return res;
}

TiXmlElement* DcxListView::ItemToXml(int nItem, int iColumns) const
{
	auto xItem = std::make_unique<TiXmlElement>("item");
	auto sTextBuffer = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);

	LVITEM lvitem{};

	gsl::at(sTextBuffer, 0) = TEXT('\0');

	lvitem.mask = LVIF_GROUPID | LVIF_IMAGE | LVIF_INDENT | LVIF_STATE | LVIF_TEXT | LVIF_PARAM;
	lvitem.iItem = nItem;
	lvitem.iSubItem = 0;
	lvitem.pszText = sTextBuffer.get();
	lvitem.cchTextMax = MIRC_BUFFER_SIZE_CCH;
	lvitem.stateMask = UINT_MAX;

	if (Dcx::dcxListView_GetItem(m_Hwnd, &lvitem))
	{
		// [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text{ TAB }[+FLAGS][#ICON][#OVERLAY][COLOR][BGCOLOR] Item Text ...
		xItem->SetAttribute("indent", lvitem.iIndent);		// INDENT
		LPDCXLVITEM lpmylvi = reinterpret_cast<LPDCXLVITEM>(lvitem.lParam);
		{
			TString tsFlags(TEXT('+'));			// +flags
			if (lpmylvi)
			{
				if (lpmylvi->pbar && lpmylvi->iPbarCol == 0)
				{
					// save pbar info
					xItem->LinkEndChild(lpmylvi->pbar->toXml());
				}
				if (!lpmylvi->vInfo.empty())
				{
					//if (auto ri = gsl::at(lpmylvi->vInfo, 0); ri)
					{
						const auto& ri = gsl::at(lpmylvi->vInfo, 0);

						if (ri.m_cText != CLR_INVALID)
							setColourAttribute(xItem.get(), "textcolour", ri.m_cText);	// colour
						if (ri.m_cBg != CLR_INVALID)
							setColourAttribute(xItem.get(), "bgcolour", ri.m_cBg);		// bgcolour

						tsFlags = parseItemFlags(ri.m_dFlags);

						//if (dcx_testflag(ri.m_dFlags, LVIS_COLOR))
						//	tsFlags += TEXT('c');
						//if (dcx_testflag(ri.m_dFlags, LVIS_BGCOLOR))
						//	tsFlags += TEXT('k');
						//if (dcx_testflag(ri.m_dFlags, LVIS_BOLD))
						//	tsFlags += TEXT('b');
						//if (dcx_testflag(ri.m_dFlags, LVIS_CENTERICON))
						//	tsFlags += TEXT('C');
						//if (dcx_testflag(ri.m_dFlags, LVIS_ITALIC))
						//	tsFlags += TEXT('i');
						//if (dcx_testflag(ri.m_dFlags, LVIS_UNDERLINE))
						//	tsFlags += TEXT('u');
					}
				}
			}
			xItem->SetAttribute("flags", tsFlags.c_str());	// flags
		}

		if (lvitem.iImage >= 0)
			xItem->SetAttribute("icon", lvitem.iImage + 1);	// ICON
		if (const auto iState = (lvitem.state & LVIS_STATEIMAGEMASK) >> 12; iState > 0)
			xItem->SetAttribute("stateicon", iState);	// state
		if (const auto iOverlay = (lvitem.state & LVIS_OVERLAYMASK) >> 8; iOverlay > 0)
			xItem->SetAttribute("overlayicon", iOverlay);			// overlay 1-based
		if (lvitem.iGroupId >= 0)
			xItem->SetAttribute("group", lvitem.iGroupId);	// group

		if (lvitem.pszText[0] != TEXT('\0'))
			xItem->SetAttribute("text", TString(lvitem.pszText).c_str());	// item text

		for (auto nSubItem = 1; nSubItem < iColumns; ++nSubItem)
		{
			gsl::at(sTextBuffer, 0) = TEXT('\0');

			TiXmlElement xSubItem("subitem");

			lvitem.iSubItem = nSubItem;
			lvitem.cchTextMax = MIRC_BUFFER_SIZE_CCH;
			lvitem.pszText = sTextBuffer.get();
			lvitem.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_STATE;

			if (Dcx::dcxListView_GetItem(m_Hwnd, &lvitem))
			{
				// { TAB }[+FLAGS][#ICON][#OVERLAY][COLOR][BGCOLOR] Item Text ...
				if (lvitem.iImage >= 0)
					xSubItem.SetAttribute("icon", lvitem.iImage + 1);	// ICON
				xSubItem.SetAttribute("overlayicon", (lvitem.state & LVIS_OVERLAYMASK) >> 8);			// overlay

				{
					TString tsFlags(TEXT('+'));			// +flags

					if (lpmylvi)
					{
						if (lpmylvi->pbar && lpmylvi->iPbarCol == nSubItem)
						{
							// save pbar info
							xSubItem.LinkEndChild(lpmylvi->pbar->toXml());
						}
						if (nSubItem < gsl::narrow_cast<int>(std::size(lpmylvi->vInfo)))
						{
							//if (auto ri = gsl::at(lpmylvi->vInfo, gsl::narrow_cast<size_t>(nSubItem)); ri)
							{
								const auto& ri = gsl::at(lpmylvi->vInfo, gsl::narrow_cast<size_t>(nSubItem));
								if (ri.m_cBg != CLR_INVALID)
									setColourAttribute(&xSubItem, "bgcolour", ri.m_cBg);		// bgcolour
								if (ri.m_cText != CLR_INVALID)
									setColourAttribute(&xSubItem, "textcolour", ri.m_cText);	// colour

								tsFlags = parseItemFlags(ri.m_dFlags);

								//if (dcx_testflag(ri.m_dFlags, LVIS_COLOR))
								//	tsFlags += TEXT('c');
								//if (dcx_testflag(ri.m_dFlags, LVIS_BGCOLOR))
								//	tsFlags += TEXT('k');
								//if (dcx_testflag(ri.m_dFlags, LVIS_BOLD))
								//	tsFlags += TEXT('b');
								//if (dcx_testflag(ri.m_dFlags, LVIS_CENTERICON))
								//	tsFlags += TEXT('C');
								//if (dcx_testflag(ri.m_dFlags, LVIS_ITALIC))
								//	tsFlags += TEXT('i');
								//if (dcx_testflag(ri.m_dFlags, LVIS_UNDERLINE))
								//	tsFlags += TEXT('u');
							}
						}
					}
					xSubItem.SetAttribute("flags", tsFlags.c_str());				// +flags
				}
				if (lvitem.pszText[0] != TEXT('\0'))
					xSubItem.SetAttribute("text", TString(lvitem.pszText).c_str());	// item text
			}

			xItem->InsertEndChild(xSubItem);
		}
	}
	return xItem.release();
}

bool DcxListView::xSaveListview(const int nStartPos, const int nEndPos, const TString& tsData, const TCHAR* sTestCommand, const TCHAR* sStoreCommand)
{
	if (nStartPos > nEndPos)
		return false;

	TString res;
	// check store exists
	mIRCLinker::eval(res, sTestCommand, tsData);

	// if not exit
	if (res.empty())
		throw Dcx::dcxException(TEXT("Invalid store: %"), tsData);

	if (const auto iCount = Dcx::dcxListView_GetItemCount(m_Hwnd); nEndPos >= iCount)
		return false;

	const auto iColumns = getColumnCount();

	for (auto nItem = nStartPos; nItem <= nEndPos; ++nItem)
	{
		res = ItemToString(nItem, iColumns);

		if (!res.empty())
			mIRCLinker::exec(sStoreCommand, tsData, res);
	}
	return true;
}

void DcxListView::DrawEmpty(HDC hdc, const TString& tsBuf)
{
	if ((!m_Hwnd) || (!hdc))
		return;

	// Setup alpha blend if any.
	auto ai = SetupAlphaBlend(&hdc);
	Auto(FinishAlphaBlend(ai));

	if (RECT rc{}; GetClientRect(m_Hwnd, &rc))
	{

		if (auto hdr_hwnd = Dcx::dcxListView_GetHeader(m_Hwnd); hdr_hwnd)
		{
			if (RECT rcH{};	GetWindowRect(hdr_hwnd, &rcH))
			{
				MapWindowRect(nullptr, m_Hwnd, &rcH);
				rc.top += rcH.bottom;
			}
		}

		{
			//const COLORREF clrText = ListView_GetTextColor(m_Hwnd);
			//const COLORREF clrTextBk = ListView_GetTextBkColor(m_Hwnd);
			const COLORREF clrText = (m_TextOptions.m_clrText == CLR_INVALID) ? GetSysColor(COLOR_WINDOWTEXT) : m_TextOptions.m_clrText;
			const COLORREF clrTextBk = (m_TextOptions.m_clrTextBackground == CLR_INVALID) ? GetSysColor(COLOR_WINDOW) : m_TextOptions.m_clrTextBackground;

			SetTextColor(hdc, clrText);
			SetBkColor(hdc, clrTextBk);

			{
				const COLORREF clrBk = (m_clrBackground == CLR_INVALID) ? Dcx::dcxListView_GetBkColor(m_Hwnd) : m_clrBackground;
				Dcx::FillRectColour(hdc, &rc, clrBk);
			}
		}

		rc.top += 10;

		if (m_hFont)
			Dcx::dcxSelectObject(hdc, m_hFont);

		//if (IsControlCodeTextDisabled())
		//	DrawTextW(hdc, tsBuf.to_wchr(), -1, &rc, DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);
		//else
		//	mIRC_DrawText(hdc, tsBuf, &rc, DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP, false);

		this->ctrlDrawText(hdc, tsBuf, &rc, DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);

		DrawMargin(hdc);
	}
}

void DcxListView::DrawMargin(HDC hdc) noexcept
{
	// test code for putting stuff in magins...

	if ((!m_Hwnd) || (!hdc) || (!m_hFont))
		return;

	RECT vrc{};
	if (!Dcx::dcxListView_GetViewMargin(m_Hwnd, &vrc))
		return;

	if (vrc.left < 1)
		return;

	RECT rc{ getListRect() };

	const auto olClr = SetTextColor(hdc, m_MarginData.m_Left.m_clrTxt);
	Auto(SetTextColor(hdc, olClr));

	const auto hPrevFont = Dcx::dcxSelectObject(hdc, m_hFont);
	Auto(Dcx::dcxSelectObject(hdc, hPrevFont));

	//if (!dcxDrawTranslucentRect(hdc, &rc, m_MarginData.m_Left.m_clrBkg, m_MarginData.m_Left.m_clrBorder, false))
	dcxDrawRect(hdc, &rc, m_MarginData.m_Left.m_clrBkg, m_MarginData.m_Left.m_clrBorder, false);

	rc.bottom -= 5;
	rc.top += 5;

	if (m_MarginData.m_Left.m_Ctrl)
	{
		// size child control.
		SetWindowPos(m_MarginData.m_Left.m_Ctrl->getHwnd(), nullptr, rc.left, rc.top, (rc.right - rc.left), (rc.bottom - rc.top), SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	else if (!m_MarginData.m_Left.m_Text.empty())
	{
		DrawRotatedText(m_MarginData.m_Left.m_Text, &rc, hdc, 90);
		//DrawRotatedText(m_MarginData.m_Left.m_Text, &rc, hdc, 90, true, 90);
		//ctrlDrawText(hdc, m_MarginData.m_Left.m_Text, &rc, DT_SINGLELINE);
	}
}

void DcxListView::DrawMargin() noexcept
{
	if (!m_Hwnd)
		return;

	//if (HDC hdc = GetDC(m_Hwnd); hdc)
	//{
	//	Auto(ReleaseDC(m_Hwnd, hdc));
	//	DrawMargin(hdc);
	//}

	DrawMargin(wil::GetDC(m_Hwnd).get());
}

void DcxListView::DrawClientArea(HDC hdc)
{
	if ((!hdc) || (!m_Hwnd))
		return;

	// Setup alpha blend if any.
	auto ai = SetupAlphaBlend(&hdc);
	Auto(FinishAlphaBlend(ai));

	if (!IsControlCodeTextDisabled())
	{
		// only allow this rendering if control code text is enabled atm (may change)
		if (Dcx::dcxListView_GetItemCount(m_Hwnd) <= 0)
		{
			if (GetWindowTextLength(m_Hwnd) > 0)
			{
				if (const TString tsBuf(TGetWindowText(m_Hwnd)); !tsBuf.empty())
				{
					DrawEmpty(hdc, tsBuf);
					return;
				}
			}
		}
	}

	//CallDefaultClassProc(WM_PAINT, reinterpret_cast<WPARAM>(hdc), 0);
	DrawMargin(hdc);
}

void DcxListView::DrawClientArea()
{
	//if (HDC hdc = GetDC(m_Hwnd); hdc)
	//{
	//	Auto(ReleaseDC(m_Hwnd, hdc));
	//	DrawClientArea(hdc);
	//}

	DrawClientArea(wil::GetDC(m_Hwnd).get());
}

LRESULT DcxListView::DrawItem(LPNMLVCUSTOMDRAW lplvcd)
{
	if (!lplvcd)
		return CDRF_DODEFAULT;

	switch (lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		return (CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT);

	case CDDS_ITEMPREPAINT:
		return CDRF_NOTIFYSUBITEMDRAW;

	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
	{
		auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lplvcd->nmcd.lItemlParam);

		if (!lpdcxlvi)
			return CDRF_DODEFAULT;

		if ((lpdcxlvi->pbar) && (lplvcd->iSubItem == lpdcxlvi->iPbarCol))
			return CDRF_SKIPDEFAULT;

		//if ((lpdcxlvi->pbar) && (lplvcd->iSubItem == lpdcxlvi->iPbarCol))
		//{
		//	//DrawControl(lplvcd->nmcd.hdc, lpdcxlvi->pbar->getHwnd());
		//	SendMessage(lpdcxlvi->pbar->getHwnd(), WM_PRINT, (WPARAM)lplvcd->nmcd.hdc, gsl::narrow_cast<LPARAM>(PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN));
		//	return CDRF_SKIPDEFAULT;
		//}

		if ((gsl::narrow_cast<UINT>(lplvcd->iSubItem) >= lpdcxlvi->vInfo.size()) || (lplvcd->iSubItem < 0))
			return CDRF_DODEFAULT;

		const auto& ri = gsl::at(lpdcxlvi->vInfo, gsl::narrow_cast<UINT>(lplvcd->iSubItem));
		if (ri.m_cText != CLR_INVALID)
			lplvcd->clrText = ri.m_cText;
		if (ri.m_cBg != CLR_INVALID)
			lplvcd->clrTextBk = ri.m_cBg;

		if (dcx_testflag(ri.m_dFlags, LVIS_UNDERLINE) || dcx_testflag(ri.m_dFlags, LVIS_BOLD) || dcx_testflag(ri.m_dFlags, LVIS_ITALIC))
		{
			if (auto [code, lf] = Dcx::dcxGetObject<LOGFONT>(GetWindowFont(m_Hwnd)); code != 0)
			{
				if (dcx_testflag(ri.m_dFlags, LVIS_BOLD))
					lf.lfWeight |= FW_BOLD;
				if (dcx_testflag(ri.m_dFlags, LVIS_UNDERLINE))
					lf.lfUnderline = 1;
				if (dcx_testflag(ri.m_dFlags, LVIS_ITALIC))
					lf.lfItalic = 1;

				this->m_hItemFont = CreateFontIndirect(&lf);
				if (this->m_hItemFont)
					this->m_hOldItemFont = Dcx::dcxSelectObject(lplvcd->nmcd.hdc, this->m_hItemFont);
			}
		}
		if (dcx_testflag(ri.m_dFlags, LVIS_CENTERICON))
		{
			// test code for centering an image in an item when it has no text
			const stString<MIRC_BUFFER_SIZE_CCH> sBuf;

			LVITEM lvi{ LVIF_IMAGE | LVIF_TEXT | LVIF_STATE, gsl::narrow_cast<int>(lplvcd->nmcd.dwItemSpec), lplvcd->iSubItem, 0U, LVIS_SELECTED | LVIS_FOCUSED, sBuf, gsl::narrow_cast<int>(sBuf.size()), 0,0,0,0,0U, nullptr, nullptr, 0 };

			if (Dcx::dcxListView_GetItem(m_Hwnd, &lvi))
			{
				if ((lvi.iImage > I_IMAGECALLBACK) && sBuf.empty())
				{
					// no text, but has image, so center image in item
					if (auto himl = getImageList(LVSIL_SMALL); himl)
					{
						if (RECT rcBounds{}; Dcx::dcxListView_GetSubItemRect(m_Hwnd, gsl::narrow_cast<int>(lplvcd->nmcd.dwItemSpec), lplvcd->iSubItem, LVIR_BOUNDS, &rcBounds))
						{
							UINT iDrawFlags = ILD_NORMAL | ILD_TRANSPARENT;

							if (dcx_testflag(lvi.state, LVIS_SELECTED))
							{
								const auto exStyles = Dcx::dcxListView_GetExtendedListViewStyle(m_Hwnd);

								if (!dcx_testflag(exStyles, LVS_EX_BORDERSELECT))
								{
									// fill background with selected colour. Only if not border select mode
									iDrawFlags |= ILD_BLEND50;	// blend icon image with select colour
									if (getParentDialog()->isDialogActive())
									{
										if (getParentDialog()->getFocusControl() == getUserID())
											FillRect(lplvcd->nmcd.hdc, &rcBounds, GetSysColorBrush(COLOR_HIGHLIGHT));
										else if (isStyle(WindowStyle::LVS_ShowSelAlways)) // item greyed...
											FillRect(lplvcd->nmcd.hdc, &rcBounds, GetSysColorBrush(COLOR_MENU));
									}
									else if (isStyle(WindowStyle::LVS_ShowSelAlways))
										FillRect(lplvcd->nmcd.hdc, &rcBounds, GetSysColorBrush(COLOR_MENU));
								}
								if (!dcx_testflag(exStyles, LVS_EX_FULLROWSELECT))
								{
									// draw focus rect around selected item. Only if not fullrow select
									if (dcx_testflag(lvi.state, LVIS_FOCUSED))
										DrawFocusRect(lplvcd->nmcd.hdc, &rcBounds);
								}
							}
							else {
								if (ri.m_cBg != CLR_INVALID)
									Dcx::FillRectColour(lplvcd->nmcd.hdc, &rcBounds, ri.m_cBg);
							}
							if (int iSizeX = 0, iSizeY = 0; ImageList_GetIconSize(himl, &iSizeX, &iSizeY))
							{
								if (ImageList_Draw(himl, lvi.iImage, lplvcd->nmcd.hdc, rcBounds.left + ((rcBounds.right - rcBounds.left) / 2) - (iSizeX / 2), rcBounds.top, iDrawFlags))
									return (CDRF_SKIPDEFAULT | CDRF_NOTIFYPOSTPAINT);
							}
						}
					}
				}
			}
		}
		// NB: CDRF_NOTIFYPOSTPAINT required to get the post paint message.
		return (CDRF_NEWFONT | CDRF_NOTIFYPOSTPAINT);
	}
	break;

	case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
	{
		const auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lplvcd->nmcd.lItemlParam);

		if (!lpdcxlvi)
			return CDRF_DODEFAULT;

		if (!lpdcxlvi->pbar)
		{
			if (m_hOldItemFont)
			{
				Dcx::dcxSelectObject(lplvcd->nmcd.hdc, m_hOldItemFont);
				m_hOldItemFont = nullptr;
			}
			if (m_hItemFont)
			{
				DeleteObject(m_hItemFont);
				m_hItemFont = nullptr;
			}
		}
		//else if (lpdcxlvi->iPbarCol == lplvcd->iSubItem) {
		//
		//	//this->isListViewStyle(LVS_REPORT)
		//	auto pbarHwnd = lpdcxlvi->pbar->getHwnd();
		//
		//	ShowWindow(pbarHwnd, SW_SHOW);
		//
		//	RECT rItem, rcWin;
		//
		//	CopyRect(&rItem, &lplvcd->nmcd.rc);
		//
		//	// show borders correctly
		//	rItem.bottom--;
		//	rItem.top++;
		//	rItem.left++;
		//	rItem.right--;
		//
		//	GetWindowRect(pbarHwnd, &rcWin);
		//	MapWindowRect(nullptr, m_Hwnd, &rcWin);
		//	if (!EqualRect(&rcWin, &rItem)) {
		//		MoveWindow(pbarHwnd, rItem.left, rItem.top, (rItem.right - rItem.left), (rItem.bottom - rItem.top), FALSE);
		//		InvalidateRect(pbarHwnd, nullptr, TRUE);
		//	}
		//}
		return CDRF_DODEFAULT;
	}

	case CDDS_POSTPAINT:
	{
		// update the pbar positions
		//ScrollPbars((int)lplvcd->nmcd.dwItemSpec);
		// NB: This method updates all progress bars each time ANY item is drawn, to do this it itterates ALL items!! This is VERY slow!
		UpdateScrollPbars();

#ifndef NDEBUG
		// test code for putting stuff in magins...
		//DrawMargin(lplvcd->nmcd.hdc);
#endif
	}
	[[fallthrough]];
	default:
		break;
	}
	return CDRF_DODEFAULT;
}

void DcxListView::DrawGroupHeaderText(HDC hdc, HTHEME hTheme, int iStateId, LPCRECT rc, const TString& tsText, UINT uTextFlags, UINT uAlign, bool bCustomText, int iCol)
{
	RECT rcText{ *rc };
	RECT rcRgn{ rcText };

	if (const TString tsHeader(tsText.gettok(iCol + 1, TSTABCHAR)); !tsHeader.empty())
	{
		// calc the min rect for the text
		if (bCustomText)
			calcTextRect(hdc, tsHeader, &rcRgn, uTextFlags);
		else
			DcxUXModule::dcxGetThemeTextExtent(hTheme, hdc, LISTVIEWPARTS::LVP_GROUPHEADER, iStateId, tsHeader.to_chr(), -1, uTextFlags, &rcText, &rcRgn);

		// offset rect for group settings.
		if (dcx_testflag(uAlign, LVGA_HEADER_CENTER))
		{
			OffsetRect(&rcRgn, ((rcText.right - rcText.left) / 2) - ((rcRgn.right - rcRgn.left) / 2), 0);
			rcRgn.right += 5;
		}
		else if (dcx_testflag(uAlign, LVGA_HEADER_RIGHT))
		{
			OffsetRect(&rcRgn, (rcText.right - rcText.left) - (rcRgn.right - rcRgn.left), 0);
			rcRgn.left -= 5;
		}
		if (rcRgn.left < rcText.left)
			rcRgn.left = rcText.left;
		if (rcRgn.right > rcText.right)
			rcRgn.right = rcText.right;

		// draw text
		if (bCustomText)
			ctrlDrawText(hdc, tsHeader, &rcRgn, uTextFlags);
		else
			DcxUXModule::dcxDrawThemeText(hTheme, hdc, LISTVIEWPARTS::LVP_GROUPHEADER, iStateId, tsHeader.to_chr(), -1, uTextFlags, 0, &rcRgn);

		// exclude text rect from line (drawn bellow)
		ExcludeClipRect(hdc, rcRgn.left, rcRgn.top, rcRgn.right, rcRgn.bottom);
	}
}

LRESULT DcxListView::DrawGroup(LPNMLVCUSTOMDRAW lplvcd)
{
	if ((!m_Hwnd) || (!lplvcd) || (!m_bCustomGroups))
		return CDRF_DODEFAULT;

	switch (lplvcd->nmcd.dwDrawStage)
	{
	default:
		return CDRF_DODEFAULT;

	case CDDS_PREPAINT:
	{
		// get group text if any.
		const TString tsBuf(getGroupHeader(gsl::narrow_cast<int>(lplvcd->nmcd.dwItemSpec)));

		// bail out if no tab seperated text
		if (tsBuf.numtok(TSTABCHAR) == 1)
			return CDRF_DODEFAULT;

		// open theme
		GSL_SUPPRESS(r.3) GSL_SUPPRESS(lifetime.1) auto hTheme = DcxUXModule::dcxOpenThemeData(m_Hwnd, L"ListView;ListViewStyle");
		if (!hTheme)
			return CDRF_DODEFAULT;

		Auto(GSL_SUPPRESS(lifetime.1) DcxUXModule::dcxCloseThemeData(hTheme));

		const int iStateId = getGroupDrawState();

		// draw selection rect if any.
		DrawGroupSelectionRect(hTheme, lplvcd->nmcd.hdc, &lplvcd->rcText, iStateId);

		// draw header bkg image if any
		if (auto himl = getImageList(LVSIL_GROUPHEADER); himl)
		{
			ImageList_Draw(himl, 1, lplvcd->nmcd.hdc, lplvcd->rcText.left, lplvcd->rcText.top, ILD_NORMAL | ILD_TRANSPARENT);
			//ImageList_DrawEx(himl, 0, lplvcd->nmcd.hdc, lplvcd->rcText.left, lplvcd->rcText.top, (lplvcd->rcText.right - lplvcd->rcText.left), (lplvcd->rcText.bottom - lplvcd->rcText.top), CLR_NONE, CLR_NONE, ILD_NORMAL);
		}

		// setup text flags
		constexpr UINT iTextFlags = DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER;

		const bool bCustomText = (!this->IsControlCodeTextDisabled() || !this->IsThemed());

		if (bCustomText)
		{
			SetTextColor(lplvcd->nmcd.hdc, this->getThemeGroupTextColour(hTheme, iStateId));

			if (HFONT hFont = this->getThemeGroupFont(hTheme, iStateId, lplvcd->nmcd.hdc); hFont)
			{
				this->m_hItemFont = hFont;
				this->m_hOldItemFont = Dcx::dcxSelectObject(lplvcd->nmcd.hdc, hFont);
			}
		}

		// multiple columns only supported on report (details) view.
		if (Dcx::dcxListView_GetView(m_Hwnd) == LV_VIEW_DETAILS)
		{
			int iLeft = lplvcd->rcText.left;
			const auto iColTotal = this->getColumnCount();
			for (int iCol{}; iCol < iColTotal; ++iCol)
			{
				auto iWidth = Dcx::dcxListView_GetColumnWidth(m_Hwnd, iCol);
				if (iCol == 0)
					iWidth -= iLeft;

				const RECT rcText{ iLeft, lplvcd->rcText.top, iLeft + iWidth, lplvcd->rcText.bottom };
				//rcText.left = iLeft;
				//rcText.top = lplvcd->rcText.top;
				//rcText.right = iLeft + iWidth;
				//rcText.bottom = lplvcd->rcText.bottom;

				DrawGroupHeaderText(lplvcd->nmcd.hdc, hTheme, iStateId, &rcText, tsBuf, iTextFlags, lplvcd->uAlign, bCustomText, iCol);

				iLeft += iWidth;
			}
		}
		else
			DrawGroupHeaderText(lplvcd->nmcd.hdc, hTheme, iStateId, &lplvcd->rcText, tsBuf, iTextFlags, lplvcd->uAlign, bCustomText, 0);

		// draw collapse/expand button.
		if (IsGroupCollapsible(gsl::narrow_cast<int>(lplvcd->nmcd.dwItemSpec)))
		{
			const RECT rcButton{ GetHeaderButtonRect(&lplvcd->rcText) };
			//RECT rcButton{ lplvcd->rcText };
			//DcxUXModule::dcxGetThemeRect(hTheme, LISTVIEWPARTS::LVP_COLLAPSEBUTTON, LVCB_NORMAL, TMT_RECT, &rcButton);

			//RECT rcHDR{}, rcLABEL{};
			//Dcx::dcxListView_GetGroupRect(m_Hwnd, lplvcd->nmcd.dwItemSpec, LVGGR_HEADER, &rcHDR);
			//Dcx::dcxListView_GetGroupRect(m_Hwnd, lplvcd->nmcd.dwItemSpec, LVGGR_LABEL, &rcLABEL);

			const Dcx::dcxCursorPos pt(m_Hwnd);

			int iButtonStateId{ LVEB_NORMAL };
			//const RECT rcButton{ GetHeaderButtonRect(hTheme, lplvcd->nmcd.hdc, iButtonStateId, &lplvcd->rcText) };
			//WORD wHitCode{};
			//DcxUXModule::dcxHitTestThemeBackground(hTheme, lplvcd->nmcd.hdc, LISTVIEWPARTS::LVP_COLLAPSEBUTTON, iButtonStateId, HTTB_BACKGROUNDSEG, &lplvcd->rcText, nullptr, pt, &wHitCode);

			if (PtInRect(&rcButton, pt))
				iButtonStateId = LVEB_HOVER;

			if (IsGroupCollapsed(gsl::narrow_cast<int>(lplvcd->nmcd.dwItemSpec)))
				DcxUXModule::dcxDrawThemeBackground(hTheme, lplvcd->nmcd.hdc, LISTVIEWPARTS::LVP_EXPANDBUTTON, iButtonStateId, &rcButton, nullptr);
			else
				DcxUXModule::dcxDrawThemeBackground(hTheme, lplvcd->nmcd.hdc, LISTVIEWPARTS::LVP_COLLAPSEBUTTON, iButtonStateId, &rcButton, nullptr);

			// exclude button rect from line (drawn bellow)
			ExcludeClipRect(lplvcd->nmcd.hdc, rcButton.left, rcButton.top, rcButton.right, rcButton.bottom);
		}

		{
			// calc line size.
			RECT rcLine = lplvcd->rcText;
			rcLine.left += 5;
			rcLine.right -= 5;
			rcLine.top += ((rcLine.bottom - rcLine.top) / 2) - 1;
			rcLine.bottom = rcLine.top + 1;

			// draw line through group. (excluding text areas)
			DcxUXModule::dcxDrawThemeBackground(hTheme, lplvcd->nmcd.hdc, LISTVIEWPARTS::LVP_GROUPHEADERLINE, iStateId, &rcLine, nullptr);
		}

		return CDRF_SKIPDEFAULT;
	}
	break;

	case CDDS_POSTPAINT:
	{
		if (m_hOldItemFont)
		{
			Dcx::dcxSelectObject(lplvcd->nmcd.hdc, m_hOldItemFont);
			m_hOldItemFont = nullptr;
		}
		if (m_hItemFont)
		{
			DeleteObject(m_hItemFont);
			m_hItemFont = nullptr;
		}
	}
	break;
	}
	return CDRF_DODEFAULT;
}

int DcxListView::getGroupDrawState() noexcept
{
	// get mouse pos
	const Dcx::dcxCursorPos pos(m_Hwnd);

	// find where that is relative to the group
	LVHITTESTINFO lvhti{};
	lvhti.pt.x = pos.x;
	lvhti.pt.y = pos.y;

	Dcx::dcxListView_HitTestEx(m_Hwnd, &lvhti);

	// check if mouse pointer is over group area
	if (dcx_testflag(lvhti.flags, LVHT_EX_GROUP_HEADER))
		return GROUPHEADERSTATES::LVGH_OPENHOT;

	return GROUPHEADERSTATES::LVGH_OPEN;
}

TString DcxListView::getGroupHeader(int gid)
{
	TString tsBuf(gsl::narrow_cast<TString::size_type>(mIRCLinker::c_mIRC_Buffer_Size_cch));

	LVGROUP gInfo{};
	gInfo.cbSize = sizeof(LVGROUP);

	// check if group HAS a header.
	gInfo.mask = LVGF_STATE;
	gInfo.stateMask = LVGS_NOHEADER;

	if (Dcx::dcxListView_GetGroupInfo(m_Hwnd, gid, &gInfo) == -1)
		return tsBuf;

	if (dcx_testflag(gInfo.state, LVGS_NOHEADER))
		return tsBuf;

	gInfo.mask = LVGF_HEADER;
	gInfo.stateMask = 0;
	gInfo.state = 0;
	gInfo.pszHeader = tsBuf.to_chr();
	gInfo.cchHeader = gsl::narrow_cast<int>(tsBuf.capacity_cch());

	if (Dcx::dcxListView_GetGroupInfo(m_Hwnd, gid, &gInfo) == -1)
		return tsBuf;

	return tsBuf;
}

void DcxListView::addGroup(const TString& tsInput)
{
	const auto index = tsInput.getnexttokas<int>() - 1;
	const auto tsFlags(tsInput.getnexttok());
	const auto gid = tsInput.getnexttokas<int>();
	auto text(tsInput.getlasttoks());

	this->addGroup(index, tsFlags, gid, text);
}

void DcxListView::addGroup(int index, const TString& tsFlags, int gid, TString& tsText)
{
	if (index < 0 || gid <= 0)
		throw DcxExceptions::dcxInvalidArguments();

	if (Dcx::dcxListView_HasGroup(m_Hwnd, gid))
		throw Dcx::dcxException(TEXT("Group already exists: %"), gid);

	const auto iFlags = this->parseGroupFlags(tsFlags);
	const auto iState = this->parseGroupState(tsFlags);

	LVGROUP lvg{};
	lvg.cbSize = sizeof(LVGROUP);
	lvg.mask = LVGF_ALIGN | LVGF_HEADER | LVGF_GROUPID | LVGF_STATE /* | LVGF_TITLEIMAGE | */ /*LVGF_EXTENDEDIMAGE*/;

	//lvg.iTitleImage = 0;
	//lvg.iExtendedImage = 0;

	lvg.pszHeader = tsText.to_chr();
	lvg.iGroupId = gid;

	lvg.stateMask = iState;
	lvg.state = iState;
	lvg.uAlign = iFlags;

	Dcx::dcxListView_InsertGroup(m_Hwnd, index, std::addressof(lvg));
}

void DcxListView::DrawGroupSelectionRect(HTHEME hTheme, HDC hdc, LPCRECT rc, int iStateId) noexcept
{
	// NB: Non themed groups dont show a `hot` rect
	if (hTheme)
	{
		if (DcxUXModule::dcxDrawThemeBackground(hTheme, hdc, LISTVIEWPARTS::LVP_GROUPHEADER, iStateId, rc, nullptr) != S_OK)
		{
			// if themed drawing fails try our own draw routine
			constexpr COLORREF clrBkg = RGB(185, 229, 242);
			constexpr COLORREF clrBorder = RGB(94, 131, 191);
			dcxDrawTranslucentRect(hdc, rc, clrBkg, clrBorder, false);
		}
	}
}

void DcxListView::CopyItem(int iSrc, int iDest)
{
	// check for same item.
	if (iSrc == iDest)
		return;

	TCHAR szBuf[MIRC_BUFFER_SIZE_CCH]{};
	LVITEM lvi{};
	lvi.iItem = iSrc;
	lvi.iSubItem = 0;
	lvi.cchTextMax = MIRC_BUFFER_SIZE_CCH;
	lvi.pszText = &szBuf[0];
	lvi.stateMask = 0; // ~gsl::narrow_cast<UINT>(LVIS_SELECTED); // dont want selected state
	lvi.mask = /*LVIF_STATE |*/ LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM | LVIF_TEXT | LVIF_GROUPID | LVIF_COLUMNS;

	// Rearrange the items
	if (Dcx::dcxListView_GetItem(m_Hwnd, &lvi))
	{
		// First, copy one item
		lvi.iItem = iDest;

		// copy item data too (needs subctrl fixed)
		auto item_data = new DCXLVITEM(*((LPDCXLVITEM)lvi.lParam));
		item_data->pbar = nullptr;
		lvi.lParam = (LPARAM)item_data;

		// Insert the main item
		const int iRet = Dcx::dcxListView_InsertItem(m_Hwnd, &lvi);
		if (iRet <= iSrc)
			iSrc++;

		// Set the subitem text
		for (int i = 1; i < this->getColumnCount(); i++)
		{
			//Dcx::dcxListView_GetItemText(m_Hwnd, iSrc, i, &szBuf[0], std::size(szBuf));
			//Dcx::dcxListView_SetItemText(m_Hwnd, iRet, i, &szBuf[0]);

			lvi.mask = LVIF_TEXT | LVIF_IMAGE;
			lvi.iItem = iSrc;
			lvi.iSubItem = i;
			lvi.pszText = &szBuf[0];
			lvi.cchTextMax = gsl::narrow_cast<int>(std::size(szBuf));

			Dcx::dcxListView_GetItem(m_Hwnd, &lvi);

			lvi.iItem = iRet;
			Dcx::dcxListView_SetItem(m_Hwnd, &lvi);
		}
	}
}

void DcxListView::MoveItem(int iSrc, int iDest, bool bPreseveState) noexcept
{
	// check for same item.
	if ((!m_Hwnd) || (iSrc == iDest))
		return;

	const auto oldEvents = this->m_dEventMask;
	this->m_dEventMask = 0;
	Auto(this->m_dEventMask = oldEvents);

	TCHAR szBuf[MIRC_BUFFER_SIZE_CCH]{};
	LVITEM lvi{};
	lvi.iItem = iSrc;
	lvi.iSubItem = 0;
	lvi.cchTextMax = MIRC_BUFFER_SIZE_CCH;
	lvi.pszText = &szBuf[0];
	lvi.stateMask = UINT_MAX;
	lvi.mask = LVIF_STATE | LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM | LVIF_TEXT | LVIF_GROUPID | LVIF_COLUMNS;

	// Get source item details
	if (Dcx::dcxListView_GetItem(m_Hwnd, &lvi))
	{
		// change to dest pos
		lvi.iItem = iDest;

		// dont want selected state
		//const auto bSel = dcx_testflag(lvi.state, LVIS_SELECTED);

		lvi.state &= ~gsl::narrow_cast<UINT>(LVIS_SELECTED);
		lvi.stateMask &= ~gsl::narrow_cast<UINT>(LVIS_SELECTED);

		// Insert the main item
		const int iRet = Dcx::dcxListView_InsertItem(m_Hwnd, &lvi);
		if (iRet == -1)
			return;

		if (iRet <= iSrc)
			iSrc++;

		// Set the subitem text & image
		for (int i = 1; i < this->getColumnCount(); ++i)
		{
			lvi.mask = LVIF_TEXT | LVIF_IMAGE;
			lvi.iItem = iSrc;
			lvi.iSubItem = i;
			lvi.pszText = &szBuf[0];
			lvi.cchTextMax = gsl::narrow_cast<int>(std::size(szBuf));

			if (Dcx::dcxListView_GetItem(m_Hwnd, &lvi))
			{
				lvi.iItem = iRet;
				Dcx::dcxListView_SetItem(m_Hwnd, &lvi);
			}
		}

		// always seems to set the check state as checked.
		//if (dcx_testflag(Dcx::dcxListView_GetExtendedListViewStyle(m_Hwnd), LVS_EX_CHECKBOXES))
		//	Dcx::dcxListView_SetCheckState(m_Hwnd, iRet, Dcx::dcxListView_GetCheckState(m_Hwnd, iSrc));

		// setting select state cause lockup for some reason
		//if (bPreseveState && bSel)
		//	Dcx::dcxListView_SetItemState(m_Hwnd, iRet, LVIS_SELECTED, LVIS_SELECTED);

		// need to remove PARAM before doing delete (as we are still using this data in the new moved item)
		lvi.iItem = iSrc;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_PARAM;
		lvi.lParam = 0;
		Dcx::dcxListView_SetItem(m_Hwnd, &lvi);

		// Delete from original position
		Dcx::dcxListView_DeleteItem(m_Hwnd, iSrc);
	}
}

RECT DcxListView::getListRect() const noexcept
{
	//RECT rcClient{};
	//GetClientRect(m_Hwnd, &rcClient);
	//
	//if (auto hHeader = Dcx::dcxListView_GetHeader(m_Hwnd); IsWindowVisible(hHeader))
	//{
	//	if (RECT rcHeader{}; GetWindowRectParent(hHeader, &rcHeader))
	//	{
	//		rcClient.top += (rcHeader.bottom - rcHeader.top);
	//	}
	//}
	//return rcClient;

	if (!m_Hwnd)
		return {};

	RECT rcClient{};
	if (!GetClientRect(m_Hwnd, &rcClient))
		return {};

	RECT rcView{};
	if (!Dcx::dcxListView_GetViewRect(m_Hwnd, &rcView))
		return {};

	RECT mOffsets{};
	if (!Dcx::dcxListView_GetViewMargin(m_Hwnd, &mOffsets))
		return {};

	RECT rcMargin{};
	rcMargin.right = rcView.left;
	rcMargin.left = rcView.left - mOffsets.left;

	if (auto hHeader = Dcx::dcxListView_GetHeader(m_Hwnd); hHeader && IsWindowVisible(hHeader))
	{
		if (RECT rcHeader{}; GetWindowRectParent(hHeader, &rcHeader))
			rcMargin.top = rcClient.top + (rcHeader.bottom - rcHeader.top) + 1;
	}

	rcMargin.bottom = rcClient.bottom;

	return rcMargin;
}

//xdid -a [NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
//xdid -a -> [NAME] [ID] -a [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
void DcxListView::Command_a(const TString& input)
{
	if (!m_Hwnd)
		return;

	const auto numtok = input.numtok();

	if (numtok < 13)
		throw DcxExceptions::dcxInvalidArguments();

	const auto data(input.gettok(1, TSTABCHAR).gettok(4, -1).trim());
	auto nPos = data.gettok(1).to_int() - 1;

	if (nPos < 0)
		nPos = Dcx::dcxListView_GetItemCount(m_Hwnd);

	const auto stateFlags = this->parseItemFlags(data.gettok(3));

	if (dcx_testflag(stateFlags, LVIS_XML))
	{
		// load all item data from an xml file.
		const auto tsName(data.getfirsttok(10));	// tok 10
		auto filename(data.getlasttoks());			// tok 11, -1
		this->xmlLoadListview(nPos, tsName, filename);
		return;
	}

	if (dcx_testflag(stateFlags, LVIS_HASHTABLE))
	{
		// load all data from a mIRC hashtable.
		//this->xLoadListview(nPos, data, TEXT("$hget(%s)"), TEXT("$hget(%s,0).item"), TEXT("$hget(%s,%d)"), TEXT("$hget(%s,%s)"));
		this->xLoadListview(nPos, data, TEXT("$hget(%)"), TEXT("$hget(%,0).item"), TEXT("$hget(%,%)"), TEXT("$hget(%,%)"));
		return;
	}

	if (dcx_testflag(stateFlags, LVIS_WINDOW))
	{
		// load all data from a mIRC @window.
		//this->xLoadListview(nPos, data, TEXT("$window(%s)"), TEXT("$line(%s,0)"), TEXT("$line(%s,%d)"), nullptr);
		this->xLoadListview(nPos, data, TEXT("$window(%)"), TEXT("$line(%,0)"), TEXT("$line(%,%)"), nullptr);
		return;
	}

	if (dcx_testflag(stateFlags, LVIS_CONTROL))
	{
		// load all data from another dcx control.
		this->ctrlLoadListview(nPos, data);
		return;
	}

	massSetItem(nPos, input);
}

/// <summary>
/// 
/// </summary>
/// <param name="input"></param>
void DcxListView::Command_v(const TString& input)
{
	if (!m_Hwnd)
		return;

	const auto numtok = input.numtok();

	if (numtok < 5)
		throw DcxExceptions::dcxInvalidArguments();

	const auto tsItems(input.getnexttok());		// tok 4
	const auto tsSubItems(input.getnexttok());	// tok 5
	auto itemtext(input.getlasttoks().trim());	// tok 6, -1

	if (tsItems == TEXT("-1"))
	{
		// special case -1 supplied as item number, sets text for empty listview
		Dcx::dcxListView_SetEmptyText(m_Hwnd, itemtext.to_chr());	// tok 6, -1
		// if subitem set, redraw
		if (tsSubItems == TEXT('2'))
			redrawWindow();
		return;
	}

	// get total items
	const auto nItemCnt = Dcx::dcxListView_GetItemCount(m_Hwnd);
	// get total subitems
	const auto nSubItemCnt = this->getColumnCount();

	// iterate through all item ranges supplied
	const auto itEnd = tsItems.end();
	for (auto itStart = tsItems.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
	{
		const auto tsLine(*itStart);

		const auto ItemRange = Dcx::getItemRange2(tsLine, nItemCnt);

		if ((ItemRange.b < 0) || (ItemRange.e < 0) || (ItemRange.b > ItemRange.e))
			throw Dcx::dcxException(TEXT("Invalid Item Range %."), tsLine);

		// iterate through this range
		for (auto nItem : ItemRange)
		{
			LVITEM lvi{ LVIF_PARAM, nItem };
			Dcx::dcxListView_GetItem(m_Hwnd, &lvi);

			// iterate through all subitem ranges supplied (must be done for each item)
			const auto itSubEnd = tsItems.end();
			for (auto itSubStart = tsSubItems.begin(TSCOMMACHAR); itSubStart != itSubEnd; ++itSubStart)
			{
				const auto tsSubLine(*itSubStart);
				const auto SubItemRange = Dcx::getItemRange2(tsSubLine, nSubItemCnt);

				if ((SubItemRange.b < 0) || (SubItemRange.e < 0) || (SubItemRange.b > SubItemRange.e))
					throw Dcx::dcxException(TEXT("Invalid SubItem Range %."), tsSubLine);

				// iterate through this subitem range.
				for (auto nSubItem : SubItemRange)
				{
					// invalid item
					if ((nItem < 0) || (nSubItem < 0) || (nSubItem >= nSubItemCnt))
						throw DcxExceptions::dcxInvalidItem();

					if (const auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lvi.lParam); (lpdcxlvi && lpdcxlvi->pbar && lpdcxlvi->iPbarCol == nSubItem))
					{
						itemtext = input.getfirsttok(1) + TEXT(' ') + input.getnexttok() + TEXT(' ') + itemtext;
						lpdcxlvi->pbar->parseCommandRequest(itemtext);
					}
					else {
						if ((nItem < 0) || (nSubItem < 0) || (nSubItem >= this->getColumnCount()))
							throw DcxExceptions::dcxInvalidItem();

						Dcx::dcxListView_SetItemText(m_Hwnd, nItem, nSubItem, itemtext.to_chr());
					}
				}
			}
		}
	}
}

void DcxListView::toXml(TiXmlElement* const xml) const
{
	if (!xml || !m_Hwnd)
		return;

	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());

	// empty text
	if (const TString tsText(TGetWindowText(m_Hwnd)); !tsText.empty())
		xml->SetAttribute("emptytext", tsText.c_str());

	// group view
	if (Dcx::dcxListView_IsGroupViewEnabled(m_Hwnd))
		xml->SetAttribute("groups", "1");

	// icons...
	{
		if (auto himl = this->getImageList(LVSIL_NORMAL); himl)
			xmlSaveImageList(himl, xml, L"+nB"_ts);
		if (auto himl = this->getImageList(LVSIL_STATE); himl)
			xmlSaveImageList(himl, xml, L"+sB"_ts);
		if (auto himl = this->getImageList(LVSIL_FOOTER); himl)
			xmlSaveImageList(himl, xml, L"+fB"_ts);
		if (auto himl = this->getImageList(LVSIL_GROUPHEADER); himl)
			xmlSaveImageList(himl, xml, L"+gB"_ts);
	}
	// save column info
	{
		const auto count = this->getColumnCount();
		TCHAR szBuffer[MIRC_BUFFER_SIZE_CCH]{};
		LVCOLUMN lc{};
		lc.mask = LVCF_TEXT | LVCF_FMT | LVCF_IMAGE | LVCF_WIDTH | LVCF_ORDER;

		for (int iCol{}; iCol < count; ++iCol)
		{
			lc.cchTextMax = MIRC_BUFFER_SIZE_CCH;
			lc.pszText = &szBuffer[0];

			if (Dcx::dcxListView_GetColumn(m_Hwnd, iCol, &lc))
			{
				TiXmlElement xColumn("column");

				xColumn.SetAttribute("id", iCol + 1);
				{
					const TString tsBuffer(lc.pszText);
					xColumn.SetAttribute("text", tsBuffer.c_str());
				}
				xColumn.SetAttribute("image", lc.iImage);
				xColumn.SetAttribute("width", lc.cx);
				xColumn.SetAttribute("order", lc.iOrder);

				// setup format in flag form
				{
					TString tsFlags(L"+");
					if (dcx_testflag(lc.fmt, LVCFMT_BITMAP_ON_RIGHT))
						tsFlags += L"b";
					if (dcx_testflag(lc.fmt, LVCFMT_CENTER))
						tsFlags += L"c";
					if (dcx_testflag(lc.fmt, LVCFMT_SPLITBUTTON))
						tsFlags += L"d";
					if (dcx_testflag(lc.fmt, HDF_SORTDOWN))
						tsFlags += L"e";
					if (dcx_testflag(lc.fmt, LVCFMT_FIXED_WIDTH))
						tsFlags += L"f";
					if (dcx_testflag(lc.fmt, HDF_SORTUP))
						tsFlags += L"g";
					if (dcx_testflag(lc.fmt, LVCFMT_LEFT))
						tsFlags += L"l";
					if (dcx_testflag(lc.fmt, LVCFMT_FIXED_RATIO))
						tsFlags += L"q";
					if (dcx_testflag(lc.fmt, LVCFMT_RIGHT))
						tsFlags += L"r";

					xColumn.SetAttribute("flags", tsFlags.c_str());
				}
				xml->InsertEndChild(xColumn);
			}
		}
	}
	// save group info
	{
		const auto count = Dcx::dcxListView_GetGroupCount(m_Hwnd);
		TCHAR szBuffer[MIRC_BUFFER_SIZE_CCH]{};
		LVGROUP lvg{};

		for (int i{}; i < count; ++i)
		{
			lvg.cbSize = sizeof(lvg);
			lvg.pszHeader = &szBuffer[0];
			lvg.cchHeader = gsl::narrow_cast<int>(std::size(szBuffer));
			lvg.mask = LVGF_HEADER | LVGF_GROUPID | LVGF_ALIGN | LVGF_STATE;
			lvg.stateMask = LVGS_NORMAL | LVGS_COLLAPSED | LVGS_HIDDEN | LVGS_NOHEADER | LVGS_COLLAPSIBLE | LVGS_SUBSETED | LVGS_SELECTED;

			if (Dcx::dcxListView_GetGroupInfoByIndex(m_Hwnd, i, &lvg))
			{
				TiXmlElement xGroup("group");

				xGroup.SetAttribute("id", lvg.iGroupId);
				if (!dcx_testflag(lvg.state, LVGS_NOHEADER))
				{
					const TString tsBuffer(lvg.pszHeader);
					xGroup.SetAttribute("text", tsBuffer.c_str());
				}
				//xGroup.SetAttribute("image", lvg.iTitleImage);
				//xGroup.SetAttribute("extendedimage", lvg.iExtendedImage);

				// setup format in flag form
				{
					TString tsFlags(L"+");
					if (dcx_testflag(lvg.uAlign, LVGA_HEADER_CENTER))
						tsFlags += L"c";
					if (dcx_testflag(lvg.uAlign, LVGA_HEADER_LEFT))
						tsFlags += L"l";
					if (dcx_testflag(lvg.uAlign, LVGA_HEADER_RIGHT))
						tsFlags += L"r";

					if (dcx_testflag(lvg.state, LVGS_COLLAPSIBLE))
						tsFlags += L"C";
					if (dcx_testflag(lvg.state, LVGS_HIDDEN))
						tsFlags += L"H";
					if (dcx_testflag(lvg.state, LVGS_NOHEADER))
						tsFlags += L"N";
					if (dcx_testflag(lvg.state, LVGS_COLLAPSED))
						tsFlags += L"O";
					if (dcx_testflag(lvg.state, LVGS_SELECTED))
						tsFlags += L"S";
					if (dcx_testflag(lvg.state, LVGS_SUBSETED))
						tsFlags += L"s";

					xGroup.SetAttribute("flags", tsFlags.c_str());
				}
				xml->InsertEndChild(xGroup);
			}
		}
	}

	xmlSaveListview(0, -1, xml);
}

TiXmlElement* DcxListView::toXml() const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

std::unique_ptr<TiXmlElement> DcxListView::toXml(int blah) const
{
	UNREFERENCED_PARAMETER(blah);
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml;
}

void DcxListView::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	if (const TString tsText(queryAttribute(xThis, "emptytext")); !tsText.empty())
	{
		Dcx::dcxListView_SetEmptyText(m_Hwnd, tsText.to_chr());
	}

	{
		// load column data

		int nCnt{};
		for (auto xElm = xThis->FirstChildElement("column"); xElm; xElm = xElm->NextSiblingElement("column"))
		{
			const TString tsFlags(queryAttribute(xElm, "flags", "+"));
			const TString tsText(queryAttribute(xElm, "text"));
			const auto iIcon = queryIntAttribute(xElm, "icon");
			const auto iWidth = queryIntAttribute(xElm, "width");
			const auto iOrder = queryIntAttribute(xElm, "order", -1);

			this->addColumn(nCnt, iOrder, tsFlags, iIcon, iWidth, tsText);

			++nCnt;
		}
	}
	{
		// load group data
		int iIndex{};
		for (auto xElm = xThis->FirstChildElement("group"); xElm; xElm = xElm->NextSiblingElement("group"))
		{
			//const auto iIndex = queryIntAttribute(xElm, "index");
			const TString tsFlags(queryAttribute(xElm, "flags", "+"));
			const auto iGID = queryIntAttribute(xElm, "id");
			TString tsText(queryAttribute(xElm, "text"));
			//index +flags gid text
			this->addGroup(iIndex, tsFlags, iGID, tsText);

			++iIndex;
		}
	}

	if (queryIntAttribute(xThis, "groups"))
	{
		Dcx::dcxListView_EnableGroupView(m_Hwnd, true);
	}

	xmlLoadListview(0, xThis);
}

LRESULT DcxListView::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
