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
DcxListView::DcxListView(const UINT ID, DcxDialog* const p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
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

	if (!IsWindow(m_Hwnd))
		//throw Dcx::dcxException("Unable To Create Window");
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

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

/*!
* \brief blah
*
* blah
*/

DcxListView::~DcxListView() noexcept
{
	Dcx::dcxListView_DeleteAllItems(m_Hwnd);

	ImageList_Destroy(getImageList(LVSIL_NORMAL));
	ImageList_Destroy(getImageList(LVSIL_SMALL));
	ImageList_Destroy(getImageList(LVSIL_STATE));
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

	return styles;
}

/*!
* \brief blah
*
* blah
*/

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
			default:
				break;
			}
			// LVS_EX_COLUMNSNAPPOINTS LVS_EX_JUSTIFYCOLUMNS LVS_EX_SNAPTOGRID LVS_EX_AUTOAUTOARRANGE
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
		// if its a report listview and it has headers
		const auto count = this->getColumnCount();
		const auto col = (numtok > 3 ? input++.to_int() - 1 : -1);	// tok 4

																	// invalid column
		if ((col < -1) || (col >= count) || (count <= 0))
			//throw Dcx::dcxException("Out of Range");
			throw DcxExceptions::dcxOutOfRange();

		auto val = std::make_unique<int[]>(gsl::narrow_cast<UINT>(count));

		Dcx::dcxListView_GetColumnOrderArray(m_Hwnd, count, val.get());

		// increase each value by 1 for easy user indexing
		for (auto i = decltype(count){0}; i < count; ++i)
			gsl::at(val, gsl::narrow_cast<size_t>(i))++;

		// get specific column
		if (col > -1)
		{
			_ts_snprintf(szReturnValue, TEXT("%d"), gsl::at(val, gsl::narrow_cast<size_t>(col)));
			return;
		}

		// collect all values
		TString buff(gsl::narrow_cast<UINT>(count * 32));

		for (auto i = decltype(count){0}; i < count; ++i)
			buff.addtok(gsl::at(val, gsl::narrow_cast<size_t>(i)));

		szReturnValue = buff.trim().to_chr();
	}
	break;
	// [NAME] [ID] [PROP] [N] (NSUB)
	case L"text"_hash:
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input++.to_int() - 1;		// tok 4
		auto nSubItem = 0;

		if (numtok > 4)
			nSubItem = input++.to_int() - 1;			// tok 5
		if ((nItem < 0) || (nSubItem < 0) || (nItem >= Dcx::dcxListView_GetItemCount(m_Hwnd)))
			//throw Dcx::dcxException("Out of Range");
			throw DcxExceptions::dcxOutOfRange();

		Dcx::dcxListView_GetItemText(m_Hwnd, nItem, nSubItem, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	break;
	// [NAME] [ID] [PROP] [N] [NSUB]
	case L"icon"_hash:
	{
		if (numtok < 5)
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input++.to_int() - 1;	// tok 4
		const auto nSubItem = input++.to_int() - 1;	// tok 5

		if ((nItem < 0) || (nSubItem < 0) || (nItem >= Dcx::dcxListView_GetItemCount(m_Hwnd)) || (nSubItem >= this->getColumnCount()))
			//throw Dcx::dcxException("Out of Range");
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
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input++.to_int() - 1;	// tok 4
													// In range
		if ((nItem < 0) || (nItem >= Dcx::dcxListView_GetItemCount(m_Hwnd)))
			//throw Dcx::dcxException("Out of Range");
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

				// return total count of selected files
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

				szReturnValue = list;
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
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input++.to_int() - 1;	// tok 4
		if ((nItem < 0) || (nItem >= Dcx::dcxListView_GetItemCount(m_Hwnd)))
			//throw Dcx::dcxException("Out of Range");
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
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		if (const auto matchtext(input.getfirsttok(2, TSTABCHAR).trim()); !matchtext.empty())
		{
			const auto params(input.getnexttok(TSTABCHAR).trim());			// tok 3

			if (params.numtok() != 3)
				//throw Dcx::dcxException("Invalid number of arguments");
				throw DcxExceptions::dcxInvalidArguments();

			const auto SearchType = CharToSearchType(params++[0]);
			const auto nColumn = params++.to_int() - 1;	// tok 2
			const auto N = params++.to_int();			// tok 3
			const auto nItems = Dcx::dcxListView_GetItemCount(m_Hwnd);
			const auto nColumns = this->getColumnCount();
			auto count = decltype(N){0};

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
							if (this->matchItemText(i, k, matchtext, SearchType))
								++count;
						}
					}
				}
				// Particular Column
				else {
					if (nColumn < -1 || nColumn >= nColumns)
						//throw Dcx::dcxException("Out of Range");
						throw DcxExceptions::dcxOutOfRange();

					for (auto i = decltype(nItems){0}; i < nItems; ++i)
					{
						if (this->matchItemText(i, nColumn, matchtext, SearchType))
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
							if (this->matchItemText(i, k, matchtext, SearchType))
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
						//throw Dcx::dcxException("Out of Range");
						throw DcxExceptions::dcxOutOfRange();

					for (auto i = decltype(nItems){0}; i < nItems; ++i)
					{
						if (this->matchItemText(i, nColumn, matchtext, SearchType))
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
			nColumn = input.getnexttok().to_int() - 1;	// tok 4

		const auto count = this->getColumnCount();

		// return all columns
		if (nColumn == -1)
		{
			TString buff(gsl::narrow_cast<UINT>(count * 32));

			for (auto i = decltype(count){0}; i < count; i++)
				buff.addtok(Dcx::dcxListView_GetColumnWidth(m_Hwnd, i));

			szReturnValue = buff.trim();
		}
		else {
			if (nColumn < 0 || nColumn >= count)
				//throw Dcx::dcxException("Out of Range");
				throw DcxExceptions::dcxOutOfRange();

			_ts_snprintf(szReturnValue, TEXT("%d"), Dcx::dcxListView_GetColumnWidth(m_Hwnd, nColumn));
		}
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"htext"_hash:
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nColumn = input.getnexttok().to_int() - 1;	// tok 4

		if (nColumn < 0 || nColumn >= this->getColumnCount())
			//throw Dcx::dcxException("Out of Range");
			throw DcxExceptions::dcxOutOfRange();

		LVCOLUMN lvc{};
		lvc.mask = LVCF_TEXT;
		lvc.cchTextMax = szReturnValue.size();
		lvc.pszText = szReturnValue;

		Dcx::dcxListView_GetColumn(m_Hwnd, nColumn, &lvc);
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"hicon"_hash:
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nColumn = input.getnexttok().to_int() - 1;	// tok 4

		if (nColumn < 0 || nColumn >= this->getColumnCount())
			//throw Dcx::dcxException("Out of Range");
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
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nCol = (input.getnexttok().to_int() - 1);	// tok 4

		if (nCol < 0 || nCol >= this->getColumnCount())
			//throw Dcx::dcxException("Out of Range");
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

		szReturnValue = tsRes;
	}
	break;
	// [NAME] [ID] [PROP] [GID]
	case L"gtext"_hash:
	{
		if (numtok < 4)
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		const auto GID = input.getnexttok().to_<UINT>();	// tok 4

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
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4
		const auto nSubItem = input.getnexttok().to_int() - 1;	// tok 5

		if (nItem < 0 || nSubItem < 0 || nItem >= Dcx::dcxListView_GetItemCount(m_Hwnd))
			//throw Dcx::dcxException("Out of Range");
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
		if (Dcx::VistaModule.isVista())
			_ts_snprintf(szReturnValue, TEXT("%d"), Dcx::dcxListView_GetGroupCount(m_Hwnd));
		else {
			auto gcount = 0U;
			for (auto g = 0U; g < 256U; g++)
				if (Dcx::dcxListView_HasGroup(m_Hwnd, gsl::narrow_cast<WPARAM>(g)))
					++gcount;

			_ts_snprintf(szReturnValue, TEXT("%u"), gcount);
		}
	}
	break;
	// [NAME] [ID] [PROP] [N]
	case L"gid"_hash:
	{
		const auto iIndex = input.getnexttok().to_int() - 1;	// tok 4

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
		auto nRow = input.getnexttok().to_int();	// tok 4
		auto nCol = input.getnexttok().to_int();	// tok 5

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

		szReturnValue = (reinterpret_cast<LPDCXLVITEM>(lvi.lParam))->tsMark;
	}
	break;
	// [NAME] [ID] [PROP]
	case L"emptytext"_hash:
	{
		//if (Dcx::VistaModule.isUseable())
		//	Dcx::dcxListView_GetEmptyText(m_Hwnd, szReturnValue.data(), MIRC_BUFFER_SIZE_CCH);

		szReturnValue = TGetWindowText(m_Hwnd);
	}
	break;
	// [NAME] [ID] [PROP] [GID]
	case L"gstate"_hash:
	{
		if (numtok != 4)
			//throw Dcx::dcxException("Invalid number of arguments");
			throw DcxExceptions::dcxInvalidArguments();

		TString tsFlags('+');

		if (Dcx::VistaModule.isVista())
		{
			const auto gid = input.getnexttok().to_int();	// tok 4
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
		szReturnValue = tsFlags;
	}
	break;
	// [NAME] [ID] [PROP] [flags]
	case L"icons"_hash:
	{
		if (numtok != 4)
			//throw Dcx::dcxException("Invalid number of arguments");
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
		_ts_snprintf(szReturnValue, TEXT("%u"), iCount);
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

/*!
* \brief blah
*
* blah
*/

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
		if (numtok < 13)
			throw DcxExceptions::dcxInvalidArguments();

		LVITEM lvi{};

		const auto data(input.gettok(1, TSTABCHAR).gettok(4, -1).trim());
		auto nPos = data.gettok(1).to_int() - 1;

		if (nPos < 0)
			nPos = Dcx::dcxListView_GetItemCount(m_Hwnd);

		const auto stateFlags = this->parseItemFlags(data.gettok(3));

		if (dcx_testflag(stateFlags, LVIS_XML))
		{
			// load all item data from an xml file.
			const auto tsName(data.getfirsttok(10));		// tok 10
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
	// xdid -A [NAME] [ID] [SWITCH] [ROW] [COL] [+FLAGS] [INFO]
	else if (flags[TEXT('A')])
	{
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

		//LVITEM lvi = { 0 };
		////ZeroMemory(&lvi, sizeof(LVITEM));
		//
		//lvi.mask = LVIF_PARAM;
		//lvi.iItem = nRow;
		//lvi.iSubItem = nCol;

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
			//throw Dcx::dcxException("Missing '+' in flags");
			throw DcxExceptions::dcxInvalidFlag();

		if (!xflag[TEXT('M')])	// mark info
			throw Dcx::dcxException(TEXT("Unknown flags %"), input.gettok(6));

		lviDcx->tsMark = info;
	}
	// xdid -B [NAME] [ID] [N]
	// xdid -B -> [NAME] [ID] -B [N]
	else if (flags[TEXT('B')])
	{
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
			auto nItem = input.getnexttok().to_int() - 1;	// tok 4

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

					const auto [iStart, iEnd] = getItemRange2(tsLine, nItemCnt);	// uses structured binding...

					if ((iStart < 0) || (iEnd < 0) || (iStart >= nItemCnt) || (iEnd >= nItemCnt))
						throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

					for (auto nItem = iStart; nItem <= iEnd; nItem++)
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

		const auto Ns(input.getnexttok());	// tok 4
		const XSwitchFlags xFlags(input.getnexttok());	// tok 5

		if (xFlags[TEXT('+')])
		{
			// have flags, so its a match text delete
			const auto nSubItem = input.getnexttok().to_int();
			const auto tsMatchText(input.getnexttok());
			const auto SearchType = FlagsToSearchType(xFlags);

			// NB: item count changes on every delete.
			for (auto nPos = Ns.to_int(); nPos < Dcx::dcxListView_GetItemCount(m_Hwnd); ++nPos)
			{
				if (this->matchItemText(nPos, nSubItem, tsMatchText, SearchType))
					Dcx::dcxListView_DeleteItem(m_Hwnd, nPos--);		// NB: we do nPos-- here as a lines just been removed so we have to check the same nPos again
			}
		}
		else {
			const auto itEnd = Ns.end();
			for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
			{
				const auto tsLine(*itStart);
				const auto nItemCnt = Dcx::dcxListView_GetItemCount(m_Hwnd);

				const auto [iStart, iEnd] = getItemRange2(tsLine, nItemCnt);	// uses structured binding...

				if ((iStart < 0) || (iEnd < iStart) || (iStart >= nItemCnt) || (iEnd >= nItemCnt))
					throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

				for (auto nItem = iStart; nItem <= iEnd; ++nItem)
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

		ListView_SetBkImage(m_Hwnd, &lvbki);
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
			ListView_SetTextColor(m_Hwnd, clrColor);

		if (dcx_testflag(iColorFlags, LVCS_BKG))
			ListView_SetBkColor(m_Hwnd, clrColor);

		if (dcx_testflag(iColorFlags, LVCS_BKGTEXT))
			ListView_SetTextBkColor(m_Hwnd, clrColor);

		if (dcx_testflag(iColorFlags, LVCS_OUTLINE))
			ListView_SetOutlineColor(m_Hwnd, clrColor);

		this->redrawWindow();
	}
	// xdid -j [NAME] [ID] [ROW] [COL] [FLAGS] ([COLOUR] (BGCOLOUR))
	// [NAME] [ID] -j [ROW] [COL] [FLAGS] ([COLOUR] (BGCOLOUR))
	else if (flags[TEXT('j')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = StringToItemNumber(input++);	// tok 4

		//auto nItem = input++.to_int() - 1;						// tok 4
		//if (nItem == -1)
		//{
		//	nItem = Dcx::dcxListView_GetItemCount(m_Hwnd) - 1;
		//
		//	if (nItem < 0)
		//		throw Dcx::dcxException("Invalid Item: No Items in list");
		//}

		const auto nCol = input++.to_int() - 1;					// tok 5
		const auto lviflags = this->parseItemFlags(input++);	// tok 6
		const auto clrText = input++.to_<COLORREF>();	// tok 7
		const auto clrBack = input++.to_<COLORREF>();	// tok 8


		// invalid info
		if ((nItem < 0) || (nCol < 0) || (nCol >= this->getColumnCount()))
			throw DcxExceptions::dcxInvalidItem();

		//LVITEM lvi{};
		//
		//lvi.mask = LVIF_PARAM | LVIF_STATE;
		//lvi.iItem = nItem;
		//lvi.iSubItem = nCol;

		LVITEM lvi{ LVIF_PARAM | LVIF_STATE, nItem, nCol };

		// couldn't retrieve info
		if (!Dcx::dcxListView_GetItem(m_Hwnd, &lvi))
			throw Dcx::dcxException("Unable to get Item.");

		const auto lviDcx = reinterpret_cast<LPDCXLVITEM>(lvi.lParam);

		if (!lviDcx)
			throw Dcx::dcxException("No DCX Item Information, somethings very wrong");

		if (gsl::narrow_cast<UINT>(nCol) >= lviDcx->vInfo.size())
			throw Dcx::dcxException("No Render Information for SubItem, More subitems than columns?");

		const auto ri = gsl::at(lviDcx->vInfo, gsl::narrow_cast<UINT>(nCol));

		ri->m_dFlags = lviflags;
		if (dcx_testflag(lviflags, LVIS_COLOR))
			ri->m_cText = clrText;
		else
			ri->m_cText = CLR_INVALID;

		if (dcx_testflag(lviflags, LVIS_BGCOLOR))
			ri->m_cBg = clrBack;
		else
			ri->m_cBg = CLR_INVALID;

		Dcx::dcxListView_SetItemState(m_Hwnd, nItem, lviflags, LVIS_DROPHILITED | LVIS_FOCUSED | LVIS_SELECTED | LVIS_CUT);
	}
	// xdid -k [NAME] [ID] [SWITCH] [STATE] [N]
	// xdid -k -> [NAME] [ID] -k [STATE] [N]
	else if (flags[TEXT('k')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto state = input.getnexttok().to_int();	// tok 4
		const auto Ns(input.getnexttok());	// tok 5
		const auto nItemCnt = Dcx::dcxListView_GetItemCount(m_Hwnd);

		const auto itEnd = Ns.end();
		for (auto itStart = Ns.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
		{
			const auto tsLine(*itStart);

			const auto [iStart, iEnd] = getItemRange2(tsLine, nItemCnt);

			if ((iStart < 0) || (iEnd < 0) || (iStart >= nItemCnt) || (iEnd >= nItemCnt))
				throw Dcx::dcxException(TEXT("Invalid index %."), tsLine);

			for (auto nItem = iStart; nItem <= iEnd; ++nItem)
				Dcx::dcxListView_SetItemState(m_Hwnd, nItem, INDEXTOSTATEIMAGEMASK(gsl::narrow_cast<UINT>(state)), LVIS_STATEIMAGEMASK);
		}
	}
	// xdid -l [NAME] [ID] [SWITCH] [N] [M] [ICON] (OVERLAY)
	else if (flags[TEXT('l')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = StringToItemNumber(input.getnexttok());	// tok 4

		//auto nItem = input.getnexttok().to_int() - 1;							// tok 4
		//// check if item supplied was 0 (now -1), last item in list.
		//if (nItem == -1)
		//{
		//	nItem = Dcx::dcxListView_GetItemCount(m_Hwnd) - 1;
		//
		//	if (nItem < 0)
		//		throw Dcx::dcxException("Invalid Item: No Items in list");
		//}

		const auto nSubItem = input.getnexttok().to_int() - 1;					// tok 5
		const auto nIcon = input.getnexttok().to_int() - 1;					// tok 6
		const auto nOverlay = (numtok > 6 ? input.getnexttok().to_int() : -1);	// tok 7

		// invalid item
		if ((nItem < 0) || (nSubItem < 0) || (nSubItem >= this->getColumnCount()))
			throw DcxExceptions::dcxInvalidItem();

		/*
			nIcon = 0 (use no icon)
			nIcon = -1 (ignore value, just change overlay)

			overlay = 0 (no icon)
		*/
		// no icon to change
		if ((nIcon < -1) && (nOverlay < 0))
			return;	// not an error, just do nothing.

		LVITEM lvi{};

		lvi.iItem = nItem;
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

		Dcx::dcxListView_SetItem(m_Hwnd, &lvi);
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

		auto nColumn = (input.getnexttok().to_int() - 1);	// tok 4
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
				Dcx::dcxListView_SetColumnWidth(m_Hwnd, i - 1, input.getnexttok().to_int());	// tok 6+
		}
		else if (xflags[TEXT('d')])
		{
			// dynamic widths...
			if ((numtok - 6) < iTotal)
				throw Dcx::dcxException("Insufficient number of widths specified for +d flag");

			if ((nColumn < 0) || (gsl::narrow_cast<UINT>(nColumn) >= iTotal))
				throw Dcx::dcxException(TEXT("Invalid column specified: %"), nColumn + 1);

			const auto iFlags = this->parseHeaderFlags2(xflags);
			const auto iWidth = input.getnexttok().to_int();	// tok 6

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
			const auto iWidth = input.getnexttok().to_int();	// tok 6
			UINT iCount = 0;

			if ((iFlags == 0) && (numtok < 6))
				throw Dcx::dcxException("No width specified");

			if (nColumn > -1 && gsl::narrow_cast<UINT>(nColumn) < iTotal)	// set width for a single specific column
				iCount = 1;			// set a single column to a set width:			/xdid -n dname id column + width
			else {
				iCount = iTotal;	// set all columns to a single width:			/xdid -n dname id -1 + width
				nColumn = 0;		// or set all columns to an auto sized width:	/xdid -n dname id -1 +ahsFp
			}

			for (auto n = decltype(iCount){0}; n < iCount; ++n)
			{
				this->autoSize(nColumn, iFlags, iWidth);
				++nColumn;
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

			//std::clamp(gsl::narrow_cast<UINT>(tmp), 0U, count);

			gsl::at(indexes, i++) = tmp - 1;
		}

		Dcx::dcxListView_SetColumnOrderArray(m_Hwnd, count, indexes.get());
	}
	// xdid -q [NAME] [ID] [SWITCH] [N] [+FLAGS] [GID] [Group Text]
	else if (flags[TEXT('q')])
	{
		if (numtok < 7)
			throw DcxExceptions::dcxInvalidArguments();

		const auto index = input++.to_int() - 1;						// tok 4
		const auto tsflags(input++);									// tok 5
		const auto gid = input++.to_int();								// tok 6
		const auto iFlags = this->parseGroupFlags(tsflags);

		if (index < 0 || gid <= 0)
			throw DcxExceptions::dcxInvalidArguments();

		if (Dcx::dcxListView_HasGroup(m_Hwnd, gsl::narrow_cast<WPARAM>(gid)))
			throw Dcx::dcxException(TEXT("Group already exists: %"), gid);

		auto text(input.getlasttoks());				// tok 7, -1
		const auto iState = this->parseGroupState(tsflags);

		LVGROUP lvg{};
		lvg.cbSize = sizeof(LVGROUP);
		lvg.mask = LVGF_ALIGN | LVGF_HEADER | LVGF_GROUPID | LVGF_STATE;

		lvg.pszHeader = text.to_chr();
		lvg.iGroupId = gid;

		lvg.stateMask = iState;
		lvg.state = iState;
		lvg.uAlign = iFlags;

		ListView_InsertGroup(m_Hwnd, index, &lvg);
	}
	// xdid -Q [NAME] [ID] [SWITCH] [Add|Move|Del] ....
	// xdid -Q [NAME] [ID] [SWITCH] Add [N] [+FLAGS] [GID] [Group Text]
	// xdid -Q [NAME] [ID] [SWITCH] Move [GID] [N]
	// xdid -Q [NAME] [ID] [SWITCH] Del [GID]
	else if (flags[TEXT('Q')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsCmd(input++);										// tok 4

		switch (std::hash<TString>()(tsCmd))
		{
		case TEXT("Add"_hash):
		{
			if (numtok < 8)
				throw DcxExceptions::dcxInvalidArguments();

			const auto index = input++.to_int() - 1;						// tok 5
			const auto tsflags(input++);									// tok 6
			const auto gid = input++.to_int();								// tok 7
			const auto iFlags = this->parseGroupFlags(tsflags);

			if (index < 0 || gid <= 0)
				throw DcxExceptions::dcxInvalidArguments();

			if (Dcx::dcxListView_HasGroup(m_Hwnd, gsl::narrow_cast<WPARAM>(gid)))
				throw Dcx::dcxException(TEXT("Group already exists: %"), gid);

			auto text(input.getlasttoks());				// tok 8, -1
			const auto iState = this->parseGroupState(tsflags);

			LVGROUP lvg{};
			lvg.cbSize = sizeof(LVGROUP);
			lvg.mask = LVGF_ALIGN | LVGF_HEADER | LVGF_GROUPID | LVGF_STATE;

			lvg.pszHeader = text.to_chr();
			lvg.iGroupId = gid;

			lvg.stateMask = iState;
			lvg.state = iState;
			lvg.uAlign = iFlags;

			Dcx::dcxListView_InsertGroup(m_Hwnd, index, std::addressof(lvg));
			break;
		}
		case TEXT("Move"_hash):
		{
			if (numtok < 6)
				throw DcxExceptions::dcxInvalidArguments();

			const auto gid = input++.to_int();								// tok 5
			const auto index = input++.to_int() - 1;						// tok 6

			if (!Dcx::dcxListView_HasGroup(m_Hwnd, gsl::narrow_cast<WPARAM>(gid)))
				throw Dcx::dcxException(TEXT("Group doesn't exist: %"), gid);

			Dcx::dcxListView_MoveGroup(m_Hwnd, gid, index);
			break;
		}
		case TEXT("Del"_hash):
		{
			if (numtok < 5)
				throw DcxExceptions::dcxInvalidArguments();

			const auto gid = input++.to_int();								// tok 5

			if (gid == -1)
				Dcx::dcxListView_RemoveAllGroups(m_Hwnd);
			else {
				if (!Dcx::dcxListView_HasGroup(m_Hwnd, gsl::narrow_cast<WPARAM>(gid)))
					throw Dcx::dcxException(TEXT("Group doesn't exist: %"), gid);

				Dcx::dcxListView_RemoveGroup(m_Hwnd, gid);
			}
			break;
		}
		default:
			throw DcxExceptions::dcxInvalidCommand();
			break;
		}
	}
	// xdid -r [NAME] [ID] [SWITCH]
	// Note: This is here to prevent an message
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
		auto data(input.gettok(1, TSTAB).gettok(4, -1).trim());
		auto tsflags(data++);					// tok 1
		auto icon = data++.to_<int>() - 1;		// tok 2
		auto width = data++.to_<int>();			// tok 3

		TString itemtext;
		if (data.numtok() > 3)
			itemtext = data.getlasttoks();				// tok 4, -1

		LVCOLUMN lvc{};

		lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvc.fmt = gsl::narrow_cast<int>(this->parseHeaderFlags(tsflags));
		lvc.cx = width;
		lvc.pszText = itemtext.to_chr();
		lvc.iSubItem = 0;
		lvc.iImage = I_IMAGENONE;

		if (icon > -1)
		{
			lvc.mask |= LVCF_IMAGE;
			lvc.iImage = icon;
		}

		// column count always zero here, since we just deleted them all
		if (Dcx::dcxListView_InsertColumn(m_Hwnd, nColumn, &lvc) == -1)
			throw Dcx::dcxException(TEXT("Unable to add column: %"), nColumn);

		++this->m_iColumnCount;

		/*
		*	These flags do NOT make the columns auto size as text is added
		* They auto size the columns to match the text already present (none).
		*/
		this->autoSize(nColumn, tsflags);

		if (const auto tabs = input.numtok(TSTABCHAR); tabs > 1)
		{
			for (auto i = decltype(tabs){2}; i <= tabs; ++i)
			{
				++nColumn;

				data = input.gettok(gsl::narrow_cast<int>(i), TSTABCHAR).trim();

				tsflags = data++;
				icon = data++.to_int() - 1;	// tok 2
				width = data++.to_int();	// tok 3
				itemtext.clear();	// = TEXT("");

				if (data.numtok() > 3)
					itemtext = data.getlasttoks();		// tok 4, -1

				lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
				lvc.cx = width;
				lvc.fmt = gsl::narrow_cast<int>(this->parseHeaderFlags(tsflags));
				lvc.iImage = I_IMAGENONE;
				lvc.iSubItem = 0;
				lvc.pszText = itemtext.to_chr();

				if (icon > -1)
				{
					lvc.mask |= LVCF_IMAGE;
					lvc.iImage = icon;
				}

				if (Dcx::dcxListView_InsertColumn(m_Hwnd, nColumn, &lvc) == -1)
					throw Dcx::dcxException(TEXT("Unable to add column: %"), nColumn);

				++this->m_iColumnCount;

				this->autoSize(nColumn, tsflags);
			}
		}
		//redrawWindow();
	}
	// xdid -u [NAME] [ID] [SWITCH]
	else if (flags[TEXT('u')])
	{
		Dcx::dcxListView_SetItemState(m_Hwnd, -1, 0, LVIS_SELECTED);
	}
	// xdid -v [NAME] [ID] [SWITCH] [N] [M] (ItemText)
	else if (flags[TEXT('v')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nItem = StringToItemNumber(input.getnexttok());

		const auto nSubItem = input.getnexttok().to_int() - 1;	// tok 5

		if (nItem == -2)
		{
			// special case -1  (now -2) supplied as item number, sets text for empty listview
			Dcx::dcxListView_SetEmptyText(m_Hwnd, input.getlasttoks().trim().to_chr());	// tok 6, -1
			// if subitem set, redraw
			if (nSubItem)
				redrawWindow();
			return;
		}
		if (nItem < 0)
			throw DcxExceptions::dcxInvalidItem();

		auto itemtext(input.getlasttoks().trim());	// tok 6, -1

		//LVITEM lvi{};
		//
		//lvi.mask = LVIF_PARAM;
		//lvi.iItem = nItem;
		////lvi.iSubItem = nSubItem;
		//
		//ListView_GetItem(m_Hwnd, &lvi);
		//
		//
		//if (auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lvi.lParam); (lpdcxlvi != nullptr && lpdcxlvi->pbar != nullptr && lpdcxlvi->iPbarCol == nSubItem))
		//{
		//	itemtext = input.getfirsttok(1) + TEXT(' ') + input.getnexttok() + TEXT(' ') + itemtext;
		//	lpdcxlvi->pbar->parseCommandRequest(itemtext);
		//}
		//else {
		//	if ((nItem < 0) || (nSubItem < 0) || (nSubItem >= this->getColumnCount()))
		//		throw Dcx::dcxException("Invalid Item");
		//
		//	ListView_SetItemText(m_Hwnd, nItem, nSubItem, itemtext.to_chr());
		//}

		if (LVITEM lvi{ LVIF_PARAM, nItem }; Dcx::dcxListView_GetItem(m_Hwnd, &lvi))
		{
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
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tflags(input.getnexttok());				// tok 4
		const auto index = input.getnexttok().to_int();		// tok 5
		auto filename(input.getlasttoks());					// tok 6, -1
		const auto iFlags = parseIconFlagOptions(tflags);
		auto overlayindex = 0;

		// determine overlay index
		if (tflags.find(TEXT('o'), 0))
		{
			// overlay id offset
			const auto io = tflags.find(TEXT('o'), 1) + 1;
			overlayindex = tflags.mid(io, gsl::narrow_cast<int>(tflags.len() - io)).to_int();

			if (overlayindex < 1 || overlayindex > 15)
				throw Dcx::dcxException("Overlay index out of range (1 -> 15)");
		}

		// load both normal and small icons
		if (dcx_testflag(iFlags, LVSIL_SMALL))
		{
			// load normal icon
			if (auto himl = this->initImageList(LVSIL_NORMAL); index < 0)
			{
				AddFileIcons(himl, filename, true, -1);
			}
			else {
#if DCX_USE_WRAPPERS
				const Dcx::dcxIconResource icon(index, filename, true, tflags);

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
			if (auto himl = this->initImageList(LVSIL_SMALL); index < 0)
			{
				AddFileIcons(himl, filename, false, -1);
			}
			else {
#if DCX_USE_WRAPPERS
				const Dcx::dcxIconResource icon(index, filename, false, tflags);

				if (const auto i = ImageList_AddIcon(himl, icon.get()); overlayindex > 0)
					ImageList_SetOverlayImage(himl, i, overlayindex);
#else
				const HICON icon = dcxLoadIcon(index, filename, false, tflags);

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
			if (auto himl = this->initImageList(LVSIL_STATE); index < 0)
			{
				AddFileIcons(himl, filename, false, -1);
			}
			else {
#if DCX_USE_WRAPPERS
				const Dcx::dcxIconResource icon(index, filename, false, tflags);

				ImageList_AddIcon(himl, icon.get());
#else
				const HICON icon = dcxLoadIcon(index, filename, false, tflags);

				if (!icon)
					throw Dcx::dcxException("Unable to load state icon");

				ImageList_AddIcon(himl, icon);

				DestroyIcon(icon);
#endif
			}
		}
	}
	// xdid -W [NAME] [ID] [SWITCH] [STYLE]
	else if (flags[TEXT('W')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		auto mode = LV_VIEW_DETAILS;

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
		default:
			break;
		}

		Dcx::dcxListView_SetView(m_Hwnd, mode);
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
	}
	// xdid -z [NAME] [ID] [SWITCH] [+FLAGS] [N] (ALIAS)
	else if (flags[TEXT('z')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto lvsort = std::make_unique<DCXLVSORT>();	// too big for stack, use heap.

		lvsort->iSortFlags = this->parseSortFlags(input.getnexttok());	// tok 4
		const auto nColumn = input.getnexttok().to_int() - 1;	// tok 5

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
	else if (flags[TEXT('T')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		//LVITEM lvi{};
		//
		//lvi.mask = LVIF_PARAM;
		//lvi.iItem = input.getnexttok( ).to_int() -1;	// tok 4
		//lvi.iSubItem = input.getnexttok( ).to_int();	// tok 5

		LVITEM lvi{ LVIF_PARAM, input.getnexttok().to_int() - 1, input.getnexttok().to_int() };

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

		//LVSETINFOTIP it{};
		//it.cbSize = sizeof(it);
		//it.dwFlags = 0;
		//it.pszText = lpmylvi->tsTipText.to_chr();
		//it.iItem = lvi.iItem;
		//it.iSubItem = lvi.iSubItem;

		LVSETINFOTIP it{ sizeof(LVSETINFOTIP), 0, lpmylvi->tsTipText.to_chr(), lvi.iItem,lvi.iSubItem };

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

		auto pos = input.getnexttok().to_int();	// tok 4
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
		pos = dcx_round(gsl::narrow_cast<float>(height) * gsl::narrow_cast<float>(pos) / gsl::narrow_cast<float>(100.0));
		//pos = Dcx::dcxRound<int>(height * pos / 100.0);

		Dcx::dcxListView_EnsureVisible(m_Hwnd, 0, FALSE);
		Dcx::dcxListView_Scroll(m_Hwnd, 0, pos);
	}
	// xdid -V [NAME] [ID] [SWITCH] [nItem]
	else if (flags[TEXT('V')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		if (const auto nItem = input.getnexttok().to_int() - 1; nItem > -1)
			Dcx::dcxListView_EnsureVisible(m_Hwnd, nItem, FALSE);
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
		const auto iN1 = input.getnexttok().to_int();			// tok 5
		auto iN2 = input.getnexttok().to_int();					// tok 6
		const auto tsArgs(input.getlasttoks().trim());		// tok 7, -1

		if ((tsFlags[0] != TEXT('+')) || (tsFlags.len() < 2))
			// no flags specified.
			throw DcxExceptions::dcxInvalidFlag();

		// make sure N1-N2 are within the range of items in listview.
		// adjust iN2 if its < 0, so its an offset from the last item.
		if (iN2 == 0)
			iN2 = count;
		else if (iN2 < 0)
			iN2 += count;

		if ((iN1 < 1) || (iN1 > count) || (iN2 < 0) || (iN2 < iN1))
			throw Dcx::dcxException("Invalid Range: N1-N2 Must be in range of items in listview");

		//switch (tsFlags[1])
		//{
		//case TEXT('c'):
		//	xSaveListview(iN1, iN2, tsArgs, TEXT("$window(%s)"), TEXT("/echo %s %s"));
		//	break;
		//case TEXT('f'):
		//	//xSaveListview(iN1, iN2, tsArgs, TEXT("$window(%s)"), TEXT("echo %s %s"));
		//	break;
		//case TEXT('h'):
		//	xSaveListview(iN1, iN2, tsArgs, TEXT("$hget(%s)"), TEXT("/hadd %s %s"));
		//	break;
		//case TEXT('x'):
		//	//xSaveListview(iN1, iN2, tsArgs, TEXT("$window(%s)"), TEXT("echo %s %s"));
		//	break;
		//default:
		//	throw Dcx::dcxException(TEXT("Invalid Flags: %"), tsFlags);
		//}

		switch (tsFlags[1])
		{
		case TEXT('c'):
			xSaveListview(iN1, iN2, tsArgs, TEXT("$window(%)"), TEXT("/echo % %"));
			break;
		case TEXT('f'):
			//xSaveListview(iN1, iN2, tsArgs, TEXT("$window(%)"), TEXT("echo % %"));
			break;
		case TEXT('h'):
			xSaveListview(iN1, iN2, tsArgs, TEXT("$hget(%)"), TEXT("/hadd % %"));
			break;
		case TEXT('x'):
			//xSaveListview(iN1, iN2, tsArgs, TEXT("$window(%)"), TEXT("echo % %"));
			break;
		default:
			//throw Dcx::dcxException(TEXT("Invalid Flags: %"), tsFlags);
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
			//throw Dcx::dcxException("Missing '+' in flags");
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

				const auto [col_start, col_end] = getItemRange2(col, col_count);

				if ((col_start < 0) || (col_end < 0) || (col_start >= col_count) || (col_end >= col_count))
					throw Dcx::dcxException(TEXT("Invalid column index %."), col);

				for (auto nCol = col_start; nCol <= col_end; ++nCol)
				{
					if (!xflag['s'])	// change header style
						throw Dcx::dcxException(TEXT("Unknown flags %"), input.gettok(5));

					setHeaderStyle(h, nCol, info);
				}
			}
		}
	}
	// xdid -G [NAME] [ID] [SWITCH] [GID] [+MASK] [+STATES]
	else if (flags[TEXT('G')])
	{
		if (numtok != 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto gid = input.getnexttok().to_int();

		if (!Dcx::dcxListView_HasGroup(m_Hwnd, gsl::narrow_cast<WPARAM>(gid)))
			throw Dcx::dcxException(TEXT("Group doesn't exist: %"), gid);

		if (!Dcx::VistaModule.isVista())
			throw Dcx::dcxException("This Command is Vista+ Only!");

		const auto iMask = this->parseGroupState(input.getnexttok());	// tok 5
		const auto iState = this->parseGroupState(input.getnexttok());	// tok 6

		Dcx::dcxListView_SetGroupState(m_Hwnd, gid, iMask, iState);
	}
	else
		this->parseGlobalCommandRequest(input, flags);
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

/*
Initializes an image list.
*/
HIMAGELIST DcxListView::initImageList(const int iImageList)
{
	if (auto himl = getImageList(iImageList); himl)
		return himl;

	auto himl = createImageList((iImageList == LVSIL_NORMAL));

	if (!himl)
		throw Dcx::dcxException("Unable to create image list");

	setImageList(himl, iImageList);

	return himl;
}

/*!
* \brief blah
*
* blah
*/

GSL_SUPPRESS(lifetime.4)
HIMAGELIST DcxListView::getImageList(const int iImageList) const noexcept
{
	return ListView_GetImageList(m_Hwnd, iImageList);
}

/*!
* \brief blah
*
* blah
*/

void DcxListView::setImageList(const HIMAGELIST himl, const int iImageList) noexcept
{
	if (auto o = ListView_SetImageList(m_Hwnd, himl, iImageList); (o && o != himl))
		ImageList_Destroy(o);
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseIconFlagOptions(const TString& flags)
{
	UINT iFlags = 0;

	const XSwitchFlags xflags(flags);

	// no +sign, missing params
	//if (!xflags[TEXT('+')])
	//	return iFlags;
	if (!xflags[TEXT('+')])
		//throw Dcx::dcxException("Invalid Icon flags used, + missing!");
		throw DcxExceptions::dcxInvalidFlag();

	if (xflags[TEXT('n')])
		iFlags |= LVSIL_SMALL;
	if (xflags[TEXT('s')])
		iFlags |= LVSIL_STATE;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseItemFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	//if (!xflags[TEXT('+')])
	//	return iFlags;
	if (!xflags[TEXT('+')])
		//throw Dcx::dcxException("Invalid Item flags used, + missing!");
		throw DcxExceptions::dcxInvalidFlag();

	if (xflags[TEXT('b')])
		iFlags |= LVIS_BOLD;
	if (xflags[TEXT('c')])
		iFlags |= LVIS_COLOR;
	if (xflags[TEXT('d')])
		iFlags |= LVIS_DROPHILITED;
	if (xflags[TEXT('f')])
		iFlags |= LVIS_FOCUSED;
	if (xflags[TEXT('i')])
		iFlags |= LVIS_ITALIC;
	if (xflags[TEXT('k')])
		iFlags |= LVIS_BGCOLOR;
	if (xflags[TEXT('s')])
		iFlags |= LVIS_SELECTED;
	if (xflags[TEXT('t')])
		iFlags |= LVIS_CUT;
	if (xflags[TEXT('u')])
		iFlags |= LVIS_UNDERLINE;
	if (xflags[TEXT('p')])
		iFlags |= LVIS_PBAR;
	if (xflags[TEXT('H')])
		iFlags |= LVIS_HASHITEM;
	if (xflags[TEXT('n')])
		iFlags |= LVIS_HASHNUMBER;
	if (xflags[TEXT('x')])
		iFlags |= LVIS_XML;
	if (xflags[TEXT('w')])
		iFlags |= LVIS_HASHTABLE;
	if (xflags[TEXT('y')])
		iFlags |= LVIS_WINDOW;
	if (xflags[TEXT('z')])
		iFlags |= LVIS_CONTROL;
	if (xflags[TEXT('C')])
		iFlags |= LVIS_CENTERICON;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseMassItemFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	//if (!xflags[TEXT('+')])
	//	return iFlags;
	if (!xflags[TEXT('+')])
		//throw Dcx::dcxException("Invalid Mass Item flags used, + missing!");
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

/*!
* \brief blah
*
* blah
*/
// used flags bcflr
UINT DcxListView::parseHeaderFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	//if (!xflags[TEXT('+')])
	//	return 0;
	if (!xflags[TEXT('+')])
		//throw Dcx::dcxException("Invalid Header flags used, + missing!");
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

/*!
* \brief blah
*
* blah
*/

INT DcxListView::parseHeaderFlags2(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	return parseHeaderFlags2(xflags);
}

INT DcxListView::parseHeaderFlags2(const XSwitchFlags& xflags)
{
	int iFlags{};

	//if (!xflags[TEXT('+')])
	//	return 0;
	if (!xflags[TEXT('+')])
		//throw Dcx::dcxException("Invalid Header flags used, + missing!");
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
	if (xflags[TEXT('p')]) // percentage
		iFlags |= DCX_LV_COLUMNF_PERCENT;
	// 'q' use by flags
	// 'r' use by flags
	if (xflags[TEXT('s')]) // max size (max of auto & header)
		iFlags |= DCX_LV_COLUMNF_AUTOMAX;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseSortFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags{};

	// no +sign, missing params
	//if (!xflags[TEXT('+')])
	//	return iFlags;
	if (!xflags[TEXT('+')])
		//throw Dcx::dcxException("Invalid Sort flags used, + missing!");
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

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseGroupFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags{};

	// no +sign, missing params
	//if (!xflags[TEXT('+')])
	//	return iFlags;
	if (!xflags[TEXT('+')])
		//throw Dcx::dcxException("Invalid Group flags used, + missing!");
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
	//if (!xflags[TEXT('+')])
	//	return iFlags;
	if (!xflags[TEXT('+')])
		//throw Dcx::dcxException("Invalid Group State used, + missing!");
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

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseColorFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags{};

	// no +sign, missing params
	//if (!xflags[TEXT('+')])
	//	return iFlags;
	if (!xflags[TEXT('+')])
		//throw Dcx::dcxException("Invalid Color flags used, + missing!");
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

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseImageFlags(const TString& flags)
{
	const XSwitchFlags xflags(flags);
	UINT iFlags{};

	// no +sign, missing params
	//if (!xflags[TEXT('+')])
	//	return iFlags;
	if (!xflags[TEXT('+')])
		//throw Dcx::dcxException("Invalid Image flags used, + missing!");
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

/*!
* \brief blah
*
* blah
*/

bool DcxListView::isListViewStyle(const WindowStyle dwView) const noexcept
{
	return ((dcxGetWindowStyle(m_Hwnd) & LVS_TYPEMASK) == dwView);
}

GSL_SUPPRESS(bounds.4)
GSL_SUPPRESS(con.4)
GSL_SUPPRESS(r.5)
bool DcxListView::matchItemText(const int nItem, const int nSubItem, const TString& search, const DcxSearchTypes& SearchType) const
{
	auto itemtext = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);
	gsl::at(itemtext, 0) = TEXT('\0');

	Dcx::dcxListView_GetItemText(m_Hwnd, nItem, nSubItem, itemtext.get(), MIRC_BUFFER_SIZE_CCH);

	return DcxListHelper::matchItemText(itemtext.get(), search, SearchType);
}

/*!
* \brief blah
*
* blah
*/

const int& DcxListView::getColumnCount() const noexcept
{
	if (m_iColumnCount < 0)
	{
		LVCOLUMN lvc{};
		lvc.mask = LVCF_WIDTH;

		auto i = 0;
		while (Dcx::dcxListView_GetColumn(m_Hwnd, i, &lvc) != FALSE)
			++i;

		m_iColumnCount = i;
	}
	return m_iColumnCount;
}

/*!
* \brief blah
*
* blah
*/

int DcxListView::getTopIndex() const noexcept
{
	if (Dcx::dcxListView_GetItemCount(m_Hwnd) > 0)
		return Dcx::dcxListView_GetTopIndex(m_Hwnd);

	return -1;
}

/*!
* \brief blah
*
* blah
*/

int DcxListView::getBottomIndex() const noexcept
{
	auto nBottomIndex = (Dcx::dcxListView_GetTopIndex(m_Hwnd) + Dcx::dcxListView_GetCountPerPage(m_Hwnd));

	if (const auto nCount = Dcx::dcxListView_GetItemCount(m_Hwnd); (nBottomIndex > nCount))
		nBottomIndex = nCount;

	return --nBottomIndex;
}

/*!
* \brief blah
*
* blah
*/

int CALLBACK DcxListView::sortItemsEx(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	auto plvsort = reinterpret_cast<LPDCXLVSORT>(lParamSort);
	if (!plvsort)
		return 0;

	plvsort->itemtext1[0] = TEXT('\0');
	plvsort->itemtext2[0] = TEXT('\0');

	GSL_SUPPRESS(bounds.3) Dcx::dcxListView_GetItemText(plvsort->m_Hwnd, lParam1, plvsort->nColumn, &plvsort->itemtext1[0], gsl::narrow_cast<int>(std::size(plvsort->itemtext1)));
	GSL_SUPPRESS(bounds.3) Dcx::dcxListView_GetItemText(plvsort->m_Hwnd, lParam2, plvsort->nColumn, &plvsort->itemtext2[0], gsl::narrow_cast<int>(std::size(plvsort->itemtext2)));

	// CUSTOM Sort
	if (dcx_testflag(plvsort->iSortFlags, LVSS_CUSTOM))
	{
		if (plvsort->tsCustomAlias.empty())
			return 0;

		//TCHAR sRes[20];
		stString<20> sRes;

		// testing new sort call... new ver doesnt pass item text directly via alias, but instead sets a %var with the text & passes the %var name to the alias.
		// Should solve some item name issues.
		//mIRCLinker::execex(TEXT("/!set -nu1 %%dcx_1sort%d %s"), plvsort->itemtext1, plvsort->itemtext1 );
		//mIRCLinker::execex(TEXT("/!set -nu1 %%dcx_2sort%d %s"), plvsort->itemtext2, plvsort->itemtext2 );
		//mIRCLinker::evalex( sRes, static_cast<int>(sRes.size()), TEXT("$%s(%%dcx_1sort%d,%%dcx_2sort%d)"), plvsort->tsCustomAlias.to_chr( ), plvsort->itemtext1, plvsort->itemtext2 );
		////
		////mIRCLinker::evalex( sRes, Dcx::countof(sRes), TEXT("$%s(%s,%s)"), plvsort->tsCustomAlias.to_chr( ), itemtext1, itemtext2 );

		mIRCLinker::exec(TEXT("/!set -nu1 \\%dcx_1sort% %"), &plvsort->itemtext1[0], &plvsort->itemtext1[0]);
		mIRCLinker::exec(TEXT("/!set -nu1 \\%dcx_2sort% %"), &plvsort->itemtext2[0], &plvsort->itemtext2[0]);
		mIRCLinker::eval(sRes, TEXT("$%(\\%dcx_1sort%,\\%dcx_2sort%)"), plvsort->tsCustomAlias, &plvsort->itemtext1[0], &plvsort->itemtext2[0]);

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

/*!
* \brief blah
*
* blah
*/
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

			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
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
			//#if !defined(NDEBUG) || defined(DCX_DEV_BUILD)
			if (!dcx_testflag(lvexstyles, LVS_EX_FULLROWSELECT))
			{ // make subitem show as selected. TEST CODE!!!!

				LVITEM lvi{ LVIF_STATE, m_iSelectedItem, m_iSelectedSubItem, 0, LVIS_SELECTED, nullptr, 0, 0, 0, 0, 0, 0, nullptr, nullptr, 0 };

				// deselect previous
				Dcx::dcxListView_SetItem(m_Hwnd, &lvi);

				// select new
				m_iSelectedItem = lvh.iItem;
				m_iSelectedSubItem = lvh.iSubItem;
				lvi.iItem = lvh.iItem;
				lvi.iSubItem = lvh.iSubItem;
				lvi.mask = LVIF_STATE;
				lvi.state = LVIS_SELECTED;
				lvi.stateMask = LVIS_SELECTED;

				Dcx::dcxListView_SetItem(m_Hwnd, &lvi);
			}
			//#endif
		}
		break;

		case NM_DBLCLK:
		{
			bParsed = TRUE;

			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
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
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
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
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
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
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			{
				if (LVHITTESTINFO lvh{}; GetCursorPos(&lvh.pt))
				{
					MapWindowPoints(nullptr, m_Hwnd, &lvh.pt, 1);

					if (Dcx::dcxListView_SubItemHitTest(m_Hwnd, &lvh) == -1)
						execAliasEx(TEXT("hover,%u"), getUserID());
					else
						execAliasEx(TEXT("hover,%u,%d,%d"), getUserID(), lvh.iItem + 1, lvh.iSubItem + 1);
				}
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

			auto edit_hwnd = Dcx::dcxListView_GetEditControl(m_Hwnd);

			m_OrigEditProc = SubclassWindow(edit_hwnd, DcxListView::EditLabelProc);
			SetProp(edit_hwnd, TEXT("dcx_pthis"), this);

			//TCHAR ret[256];
			//evalAliasEx(ret, Dcx::countof(ret), TEXT("labelbegin,%u,%d,%d"), getUserID(), lplvdi->item.iItem +1, lplvdi->item.iSubItem +1);
			//
			//if ( ts_strcmp( TEXT("noedit"), ret ) == 0)
			//	return TRUE;

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
				execAliasEx(TEXT("labelcancel,%u"), getUserID());
			else {
				//TCHAR ret[256];
				//evalAliasEx( ret, Dcx::countof(ret), TEXT("labelend,%u,%s"), getUserID( ), lplvdi->item.pszText );
				//
				//if ( ts_strcmp( TEXT("noedit"), ret ) == 0)
				//	return FALSE;

				const stString<256> sRet;
				evalAliasEx(sRet, gsl::narrow_cast<int>(sRet.size()), TEXT("labelend,%u,%s"), getUserID(), lplvdi->item.pszText);
				//evalAlias(sRet, gsl::narrow_cast<int>(sRet.size()), TEXT("labelend,%,%"), getUserID(), lplvdi->item.pszText);

				if (sRet == TEXT("noedit"))
					return FALSE;
			}
			return TRUE;
		}
		break;

		case NM_CUSTOMDRAW:
		{
			const auto lplvcd = reinterpret_cast<const LPNMLVCUSTOMDRAW>(lParam);
			bParsed = TRUE;

			if (!lplvcd)
				return CDRF_DODEFAULT;

			//if (!ListView_IsItemVisible(m_Hwnd, lplvcd->nmcd.dwItemSpec))
			//	return CDRF_DODEFAULT;

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

				if ((gsl::narrow_cast<UINT>(lplvcd->iSubItem) >= lpdcxlvi->vInfo.size()) || (lplvcd->iSubItem < 0))
					return CDRF_DODEFAULT;

				const auto ri = gsl::at(lpdcxlvi->vInfo, gsl::narrow_cast<UINT>(lplvcd->iSubItem));
				if (ri->m_cText != CLR_INVALID)
					lplvcd->clrText = ri->m_cText;
				if (ri->m_cBg != CLR_INVALID)
					lplvcd->clrTextBk = ri->m_cBg;

				//if (ri->m_cText != CLR_INVALID)
				//{
				//	ri->m_cOrigText = lplvcd->clrText;
				//	lplvcd->clrText = ri->m_cText;
				//}
				//if (ri->m_cBg != CLR_INVALID)
				//{
				//	ri->m_cOrigBg = lplvcd->clrTextBk;
				//	lplvcd->clrTextBk = ri->m_cBg;
				//}

				if (dcx_testflag(ri->m_dFlags, LVIS_UNDERLINE) || dcx_testflag(ri->m_dFlags, LVIS_BOLD) || dcx_testflag(ri->m_dFlags, LVIS_ITALIC))
				{
					//if (LOGFONT lf{}; GetObject(GetWindowFont(m_Hwnd), sizeof(LOGFONT), &lf) != 0)
					//{
					//	if (dcx_testflag(ri->m_dFlags, LVIS_BOLD))
					//		lf.lfWeight |= FW_BOLD;
					//	if (dcx_testflag(ri->m_dFlags, LVIS_UNDERLINE))
					//		lf.lfUnderline = 1;
					//	if (dcx_testflag(ri->m_dFlags, LVIS_ITALIC))
					//		lf.lfItalic = 1;
					//
					//	this->m_hItemFont = CreateFontIndirect(&lf);
					//	if (this->m_hItemFont != nullptr)
					//		this->m_hOldItemFont = SelectFont(lplvcd->nmcd.hdc, this->m_hItemFont);
					//}

					if (auto [code, lf] = Dcx::dcxGetObject<LOGFONT>(GetWindowFont(m_Hwnd)); code != 0)
					{
						if (dcx_testflag(ri->m_dFlags, LVIS_BOLD))
							lf.lfWeight |= FW_BOLD;
						if (dcx_testflag(ri->m_dFlags, LVIS_UNDERLINE))
							lf.lfUnderline = 1;
						if (dcx_testflag(ri->m_dFlags, LVIS_ITALIC))
							lf.lfItalic = 1;

						this->m_hItemFont = CreateFontIndirect(&lf);
						if (this->m_hItemFont)
							this->m_hOldItemFont = Dcx::dcxSelectObject(lplvcd->nmcd.hdc, this->m_hItemFont);
					}
				}
				if (dcx_testflag(ri->m_dFlags, LVIS_CENTERICON))
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
								if (RECT rcBounds{}; Dcx::dcxListView_GetSubItemRect(m_Hwnd, lplvcd->nmcd.dwItemSpec, lplvcd->iSubItem, LVIR_BOUNDS, &rcBounds))
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
										if (ri->m_cBg != CLR_INVALID)
											Dcx::FillRectColour(lplvcd->nmcd.hdc, &rcBounds, ri->m_cBg);
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

				//const auto ri = lpdcxlvi->vInfo[gsl::narrow_cast<UINT>(lplvcd->iSubItem)];
				//if (ri->m_cText != CLR_INVALID)
				//{
				//	lplvcd->clrText = ri->m_cOrigText;
				//	SetTextColor(lplvcd->nmcd.hdc, ri->m_cOrigText);
				//}
				//if (ri->m_cBg != CLR_INVALID)
				//{
				//	lplvcd->clrTextBk = ri->m_cOrigBg;
				//	SetBkColor(lplvcd->nmcd.hdc, ri->m_cOrigBg);
				//}

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
			}
			[[fallthrough]];
			default:
				return CDRF_DODEFAULT;
			}
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

				for (auto& x : lpdcxlvi->vInfo)
					delete x;

				lpdcxlvi->vInfo.clear();

				delete lpdcxlvi;
			}
			bParsed = TRUE; // message has been handled.
		}
		break;

		// TODO: twig: erm? unfinished? its undocumented
		case LVN_BEGINDRAG:
		{
			if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_DRAG))
				execAliasEx(TEXT("begindrag,%u"), getUserID());
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

			//TCHAR cb[15];
			//
			//evalAliasEx(cb, Dcx::countof(cb), TEXT("keydown,%u,%d"), getUserID( ), wVKey);
			//
			//// space to change checkbox state
			//if ((wVKey == 32) && dcx_testflag(ListView_GetExtendedListViewStyle(m_Hwnd), LVS_EX_CHECKBOXES)) {
			//
			//	// stop it from allowing user to change checkstate by pressing spacebar
			//	if (ts_strcmp(TEXT("nospace"), cb) == 0) {
			//		bParsed = TRUE;
			//		return TRUE;
			//	}
			//
			//	const auto index = ListView_GetNextItem(m_Hwnd, -1, LVNI_FOCUSED);
			//
			//	// TODO: twig: change this if we get multiple checkbox columns working
			//	evalAliasEx(cb, Dcx::countof(cb), TEXT("stateclick,%u,%d,1"), getUserID(), index +1);
			//}

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
			if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
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

			//LVITEM lvi = { 0 };
			//lvi.mask = LVIF_PARAM;
			//lvi.iItem = pGetInfoTip->iItem;
			//lvi.iSubItem = pGetInfoTip->iSubItem;
			//// Get item information.
			//if (ListView_GetItem(m_Hwnd, &lvi))
			//{
			//	// return tooltip text, if any.
			//	if (auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lvi.lParam); !lpdcxlvi->tsTipText.empty())
			//		pGetInfoTip->pszText = lpdcxlvi->tsTipText.to_chr();
			//	bParsed = TRUE;
			//}

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
		if (IsWindow(reinterpret_cast<HWND>(lParam)))
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
				if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
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

				if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
				{
					dcxlParam(LPNMHEADER, lphdr);

					switch (lphdr->iButton)
					{
					case 0: // left click
					{
						// commented code allows changing the sort up/down display by clicking on a header
#if DCX_DEBUG_OUTPUT
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
#endif
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

				if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
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

				if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
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

	case WM_HSCROLL:
	case WM_VSCROLL:
	{
		if (IsWindow(reinterpret_cast<HWND>(lParam)))
		{
			//if (const auto c_this = static_cast<DcxControl*>(GetProp(reinterpret_cast<HWND>(lParam), TEXT("dcx_cthis"))); c_this)
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

		if (IsControlCodeTextEnabled())
		{
			// only allow this rendering if control code text is enabled atm (may change)
			if (Dcx::dcxListView_GetItemCount(m_Hwnd) <= 0)
			{
				if (GetWindowTextLength(m_Hwnd) > 0)
				{
					if (const TString tsBuf(TGetWindowText(m_Hwnd)); !tsBuf.empty())
					{
						bParsed = TRUE;

						PAINTSTRUCT ps{};
						auto hdc = BeginPaint(m_Hwnd, &ps);
						Auto(EndPaint(m_Hwnd, &ps));

						// Setup alpha blend if any.
						auto ai = SetupAlphaBlend(&hdc);
						Auto(FinishAlphaBlend(ai));

						//lRes = CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);

						if (RECT rc{}; GetClientRect(m_Hwnd, &rc))
						{

							if (auto hdr_hwnd = Dcx::dcxListView_GetHeader(m_Hwnd); hdr_hwnd)
							{
								if (RECT rcH{};	GetWindowRect(hdr_hwnd, &rcH))
								{
									MapWindowRect(nullptr, m_Hwnd, &rcH);
									rc.top += rcH.bottom;
								}

								//const Dcx::dcxWindowRect rcH(hdr_hwnd, m_Hwnd);
								//rc.top += rcH.bottom;
							}

							{
								//const COLORREF clrText = ListView_GetTextColor(m_Hwnd);
								//const COLORREF clrTextBk = ListView_GetTextBkColor(m_Hwnd);
								const COLORREF clrText = (m_clrText == CLR_INVALID) ? GetSysColor(COLOR_WINDOWTEXT) : m_clrText;
								const COLORREF clrTextBk = (m_clrBackText == CLR_INVALID) ? GetSysColor(COLOR_WINDOW) : m_clrBackText;

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

							if (IsControlCodeTextEnabled())
								mIRC_DrawText(hdc, tsBuf, &rc, DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP, false);
							else
								DrawText(hdc, tsBuf.to_chr(), -1, &rc, DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);
						}
					}
				}

			}
		}

		if (!bParsed && IsAlphaBlend())
		{
			PAINTSTRUCT ps{};
			auto hdc = BeginPaint(m_Hwnd, &ps);
			Auto(EndPaint(m_Hwnd, &ps));

			bParsed = TRUE;

			// Setup alpha blend if any.
			auto ai = SetupAlphaBlend(&hdc);
			Auto(FinishAlphaBlend(ai));

			lRes = CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
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
		lRes = CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}

/*!
* \brief blah
*
* blah
*/

LRESULT CALLBACK DcxListView::EditLabelProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	//const auto* const pthis = static_cast<DcxListView*>(GetProp(mHwnd, TEXT("dcx_pthis")));
	const auto* const pthis = Dcx::dcxGetProp<DcxListView*>(mHwnd, TEXT("dcx_pthis"));

	if (!pthis)
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
	if (!lvi || lvi->lParam == NULL)
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
	if (!m_bHasPBars)
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
void DcxListView::ScrollPbars(const int row, const int nCols, const int iTop, const int iBottom, LPLVITEM lvi)
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

		if (ListView_IsItemVisible(m_Hwnd, lvi->iItem))
		{
			ShowWindow(lpdcxlvi->pbar->getHwnd(), SW_SHOW);
		}

		RECT rItem{};

		// get coordinates to move to
		if (col == 0)
			Dcx::dcxListView_GetItemRect(m_Hwnd, lvi->iItem, &rItem, LVIR_LABEL);
		else
			Dcx::dcxListView_GetSubItemRect(m_Hwnd, lvi->iItem, lvi->iSubItem, LVIR_LABEL, &rItem);

		// show borders correctly
		rItem.bottom--;
		++rItem.top;
		++rItem.left;
		rItem.right--;

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
bool DcxListView::xmlLoadListview(const int nPos, const TString& name, TString& filename)
{
	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("Unable To Access File: %"), filename);

	TiXmlDocument doc(filename.c_str());
	doc.SetCondenseWhiteSpace(false);
	TString tsBuf;

	const auto xmlFile = doc.LoadFile();
	if (!xmlFile)
		throw Dcx::dcxException(TEXT("Not an XML File: %"), filename);

	const auto* const xRoot = doc.FirstChildElement("dcxml");
	if (!xRoot)
		throw Dcx::dcxException("Unable Find 'dcxml' root");

	const auto* xElm = xRoot->FirstChildElement("listview_data");
	if (!xElm)
		throw Dcx::dcxException("Unable To Find 'listview_data' element");

	xElm = xElm->FirstChildElement(name.c_str());
	if (!xElm)
		throw Dcx::dcxException(TEXT("Unable To Find Dataset: %"), name);

	this->setRedraw(FALSE);

	Auto(this->redrawWindow());
	Auto(this->setRedraw(TRUE));

	auto i = 0, nItem = nPos;
	LVITEM lvi{};

	for (const auto* xNode = xElm->FirstChildElement("lvitem"); xNode; xNode = xNode->NextSiblingElement("lvitem"))
	{
		gsl::owner<LPDCXLVITEM> lpmylvi = new DCXLVITEM;
		//auto lpmylvi = std::make_unique<DCXLVITEM>();

		lpmylvi->iPbarCol = 0;
		lpmylvi->pbar = nullptr;
		lpmylvi->vInfo.clear();

		xmlSetItem(nItem, 0, xNode, &lvi, lpmylvi, tsBuf);

		// Items state icon.
		auto stateicon = -1;
		const auto* attr = xNode->Attribute("stateicon", &i);
		if (attr && i > -1)
			stateicon = i;

		// Items overlay icon.
		auto overlayicon = 0;
		attr = xNode->Attribute("overlayicon", &i);
		if (attr && i > 0)
			overlayicon = i;

		lvi.iItem = Dcx::dcxListView_InsertItem(m_Hwnd, &lvi);

		if (lvi.iItem == -1)
		{
			delete lpmylvi;
			throw Dcx::dcxException("Unable to add item");
		}

		if (stateicon > -1)
			Dcx::dcxListView_SetItemState(m_Hwnd, lvi.iItem, gsl::narrow_cast<UINT>(INDEXTOSTATEIMAGEMASK(stateicon)), LVIS_STATEIMAGEMASK);

		// overlay is 1-based index, max 15 overlay icons
		if (overlayicon > 0 && overlayicon < 16)
			Dcx::dcxListView_SetItemState(m_Hwnd, lvi.iItem, gsl::narrow_cast<UINT>(INDEXTOOVERLAYMASK(overlayicon)), LVIS_OVERLAYMASK);

		// Items checked state (if LVS_EX_CHECKBOXES style used)
		//attr = xNode->Attribute("checked",&i);
		//if (attr != nullptr && i > 0 && (ListView_GetExtendedListViewStyle(m_Hwnd) & LVS_EX_CHECKBOXES)) // items are always added in `unchecked` state
		//	ListView_SetCheckState(m_Hwnd, lvi.iItem, TRUE);
		i = queryIntAttribute(xNode, "checked");
		if (i > 0 && (Dcx::dcxListView_GetExtendedListViewStyle(m_Hwnd) & LVS_EX_CHECKBOXES)) // items are always added in `unchecked` state
			Dcx::dcxListView_SetCheckState(m_Hwnd, lvi.iItem, TRUE);

		// autosize the column
		attr = xNode->Attribute("autosize", &i);
		if (attr && i > 0)
			this->autoSize(0, DCX_LV_COLUMNF_AUTO);				//LVSCW_AUTOSIZE
		else {
			attr = xNode->Attribute("autosizeheader", &i);
			if (attr && i > 0)
				this->autoSize(0, DCX_LV_COLUMNF_AUTOHEADER);	//LVSCW_AUTOSIZE_USEHEADER
			else {
				attr = xNode->Attribute("autosizemax", &i);
				if (attr && i > 0)
					this->autoSize(0, DCX_LV_COLUMNF_AUTOMAX);	//LVSCW_AUTOSIZE_MAX
			}
		}
		// add items tooltip
		attr = xNode->Attribute("tooltip");
		if (attr)
		{
			// this version works fine, but lots of +='s
			//TString cmd(TEXT("0 0 -T "));
			//cmd += lvi.iItem + 1;
			//cmd += TEXT(" 0 ");
			//cmd += attr;
			//this->parseCommandRequest(cmd);

			// this version fails as we dont have a constructor for numbers
			//TString cmd{ TEXT("0 0 -T "), lvi.iItem + 1, TEXT(" 0 "), attr };
			//this->parseCommandRequest(cmd);

			// this version works fine, but uses formatted printing
			//TString cmd;
			//cmd.tsprintf(TEXT("0 0 -T %d 0 %S"), lvi.iItem +1, attr);
			//this->parseCommandRequest(cmd);

			TString cmd;
			parseCommandRequest(_ts_sprintf(cmd, TEXT("0 0 -T % 0 %"), (lvi.iItem + 1), attr));
		}
		// add subitems
		auto nSubItem = 1;
		for (const auto* xSubNode = xNode->FirstChildElement("lvsubitem"); xSubNode; xSubNode = xSubNode->NextSiblingElement("lvsubitem"))
		{
			xmlSetItem(nItem, nSubItem, xSubNode, &lvi, lpmylvi, tsBuf);

			// SubItems overlay icon.
			attr = xSubNode->Attribute("overlayicon", &i);
			if (attr && i > 0)
			{
				lvi.mask |= LVIF_STATE;
				lvi.state |= INDEXTOOVERLAYMASK(i);
				lvi.stateMask |= LVIS_OVERLAYMASK;
			}
			if (Dcx::dcxListView_SetItem(m_Hwnd, &lvi))
			{
				attr = xNode->Attribute("autosize", &i);
				if (attr && i > 0)
					this->autoSize(nSubItem, DCX_LV_COLUMNF_AUTO);				// LVSCW_AUTOSIZE
				else {
					attr = xNode->Attribute("autosizeheader", &i);
					if (attr && i > 0)
						this->autoSize(nSubItem, DCX_LV_COLUMNF_AUTOHEADER);	// LVSCW_AUTOSIZE_USEHEADER
					else {
						attr = xNode->Attribute("autosizemax", &i);
						if (attr && i > 0)
							this->autoSize(nSubItem, DCX_LV_COLUMNF_AUTOMAX);	// LVSCW_AUTOSIZE_MAX
					}
				}
			}
			++nSubItem;
		}
		++nItem;
	}

	return true;
}

void DcxListView::xmlSetItem(const int nItem, const int nSubItem, const TiXmlElement* xNode, LPLVITEM lvi, LPDCXLVITEM lpmylvi, TString& tsBuf)
{
	if ((!xNode) || (!lvi) || (!lpmylvi))
		return;

	ZeroMemory(lvi, sizeof(LVITEM));

	auto i = 0;

	{
		//auto ri = new DCXLVRENDERINFO;
		//ZeroMemory(ri, sizeof(DCXLVRENDERINFO));

		auto ri = std::make_unique<DCXLVRENDERINFO>();

		lvi->iItem = nItem;
		lvi->iSubItem = nSubItem;
		if (nSubItem == 0)
		{
			lvi->mask = LVIF_PARAM | LVIF_STATE;
			lvi->lParam = reinterpret_cast<LPARAM>(lpmylvi);
		}

		// Is Item text in Bold?
		auto attr = xNode->Attribute("textbold", &i);
		if (i > 0)
			ri->m_dFlags |= LVIS_BOLD;

		// Is Item text Underlined?
		attr = xNode->Attribute("textunderline", &i);
		if (i > 0)
			ri->m_dFlags |= LVIS_UNDERLINE;

		// Items text colour.
		attr = xNode->Attribute("textcolor", &i);
		if (attr && i > -1)
		{
			ri->m_cText = gsl::narrow_cast<COLORREF>(i);
			ri->m_dFlags |= LVIS_COLOR;
		}
		else
			ri->m_cText = CLR_INVALID;

		// Items background colour.
		attr = xNode->Attribute("backgroundcolor", &i);
		if (attr && i > -1)
		{
			ri->m_cBg = gsl::narrow_cast<COLORREF>(i);
			ri->m_dFlags |= LVIS_BGCOLOR;
		}
		else
			ri->m_cBg = CLR_INVALID;

		lvi->state = ri->m_dFlags;
		lvi->stateMask = (LVIS_FOCUSED | LVIS_SELECTED | LVIS_CUT | LVIS_DROPHILITED); // only alter the controls flags, ignore our custom ones.

		lpmylvi->vInfo.push_back(ri.release());
	}

	// Items icon.
	lvi->mask |= LVIF_IMAGE; // moved here to turn off icon when none is wanted.
	auto attr = xNode->Attribute("icon", &i);
	if (attr && i > 0)
		lvi->iImage = i - 1;
	else
		lvi->iImage = I_IMAGECALLBACK; // NB: using I_IMAGENONE cause a gap to be left for the icon for some reason. Using I_IMAGECALLBACK doesn't do this.

	// Items icon.
	attr = xNode->Attribute("group", &i);
	lvi->mask |= LVIF_GROUPID;
	if (attr && i > -1 && Dcx::dcxListView_HasGroup(m_Hwnd, gsl::narrow_cast<WPARAM>(i)))
		lvi->iGroupId = i;
	else
		lvi->iGroupId = I_GROUPIDNONE;

	// Items indent.
	attr = xNode->Attribute("indent", &i);
	if (attr && i > -1)
	{
		lvi->iIndent = i;
		lvi->mask |= LVIF_INDENT;
	}
	else
		lvi->iIndent = 0;

	// Items Text.
	attr = xNode->Attribute("text");
	if (attr)
	{
		lvi->mask |= LVIF_TEXT;
		tsBuf = attr;
		lvi->pszText = tsBuf.to_chr();
	}
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
	//auto clrText = data.getnexttok().to_<COLORREF>();			// tok 8
	//auto clrBack = data.getnexttok().to_<COLORREF>();			// tok 9
	auto clrText = data++.to_<COLORREF>();						// tok 8
	auto clrBack = data++.to_<COLORREF>();						// tok 9

	if (Dcx::dcxListView_GetItemCount(m_Hwnd) <= 0)
		InvalidateRect(m_Hwnd, nullptr, TRUE);

	auto lpmylvi = std::make_unique<DCXLVITEM>();
	{
		auto ri = std::make_unique<DCXLVRENDERINFO>();

		lpmylvi->iPbarCol = 0;
		lpmylvi->pbar = nullptr;
		lpmylvi->vInfo.clear();

		// setup colum zero
		ri->m_dFlags = stateFlags;
		if (dcx_testflag(stateFlags, LVIS_COLOR))
			ri->m_cText = clrText;
		else
			ri->m_cText = CLR_INVALID;

		if (dcx_testflag(stateFlags, LVIS_BGCOLOR))
			ri->m_cBg = clrBack;
		else
			ri->m_cBg = CLR_INVALID;

		lpmylvi->vInfo.push_back(ri.release());
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
		if (!Dcx::dcxListView_HasGroup(m_Hwnd, Dcx::numeric_cast<WPARAM>(group)))
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
				auto ri = std::make_unique<DCXLVRENDERINFO>();

				ri->m_dFlags = stateFlags;

				ri->m_cText = (dcx_testflag(stateFlags, LVIS_COLOR) ? clrText : CLR_INVALID);

				ri->m_cBg = (dcx_testflag(stateFlags, LVIS_BGCOLOR) ? clrBack : CLR_INVALID);

				tmp_lpmylvi->vInfo.push_back(ri.release());
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

TString DcxListView::ItemToString(int nItem, int iColumns)
{
	TString res;

	auto sTextBuffer = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);

	LVITEM lvitem{};

	gsl::at(sTextBuffer, 0) = TEXT('\0');

	lvitem.mask = LVIF_GROUPID | LVIF_IMAGE | LVIF_INDENT | LVIF_STATE | LVIF_TEXT;
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
				if (auto ri = gsl::at(lpmylvi->vInfo, 0); ri)
				{
					bgclr = ri->m_cBg;
					fgclr = ri->m_cText;
					//auto stateFlags = this->parseItemFlags(data++);
					if (dcx_testflag(ri->m_dFlags, LVIS_COLOR))
						res += TEXT('c');
					if (dcx_testflag(ri->m_dFlags, LVIS_BGCOLOR))
						res += TEXT('k');
					if (dcx_testflag(ri->m_dFlags, LVIS_BOLD))
						res += TEXT('b');
					if (dcx_testflag(ri->m_dFlags, LVIS_CENTERICON))
						res += TEXT('C');
					if (dcx_testflag(ri->m_dFlags, LVIS_ITALIC))
						res += TEXT('i');
					if (dcx_testflag(ri->m_dFlags, LVIS_UNDERLINE))
						res += TEXT('u');
				}
			}
		}

		res.addtok(lvitem.iImage);		// ICON
		res.addtok(lvitem.state);		// state
		res.addtok(TEXT('0'));			// overlay
		res.addtok(lvitem.iGroupId);	// group

		if (fgclr == CLR_NONE)
			res.addtok(TEXT("-1"));		// colour
		else
			res.addtok(fgclr);			// colour
		if (bgclr == CLR_NONE)
			res.addtok(TEXT("-1"));		// bgcolour
		else
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
				res.addtok(lvitem.iImage);		// ICON
				res.addtok(TEXT('0'));			// overlay

				bgclr = CLR_NONE;
				fgclr = CLR_NONE;
				if (lpmylvi)
				{
					if (nSubItem < gsl::narrow_cast<int>(std::size(lpmylvi->vInfo)))
					{
						if (auto ri = gsl::at(lpmylvi->vInfo, gsl::narrow_cast<size_t>(nSubItem)); ri)
						{
							bgclr = ri->m_cBg;
							fgclr = ri->m_cText;
						}
					}
				}

				if (fgclr == CLR_NONE)
					res.addtok(TEXT("-1"));		// colour
				else
					res.addtok(fgclr);			// colour
				if (bgclr == CLR_NONE)
					res.addtok(TEXT("-1"));		// bgcolour
				else
					res.addtok(bgclr);			// bgcolour

				if (lvitem.pszText[0] != TEXT('\0'))
					res.addtok(lvitem.pszText);	// item text
			}
		}
	}
	return res;
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

void DcxListView::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());
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

LRESULT DcxListView::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
