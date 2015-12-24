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

DcxListView::DcxListView(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
	//, m_bDrag(false)
	, m_hItemFont(nullptr)
	, m_hOldItemFont(nullptr)
	, m_OrigEditProc(nullptr)
	, m_iSelectedItem(0)
	, m_iSelectedSubItem(0)
	, m_bHasPBars(false)
	, m_iColumnCount(-1)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	this->m_Hwnd = CreateWindowEx(
		(DWORD)ExStyles,
		DCX_LISTVIEWCLASS,
		nullptr,
		(DWORD)(WS_CHILD | (Styles & ~WS_CLIPCHILDREN)), // can't be ws_clipchildren as this causes render bug when progressbar items are selected.
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(this->m_Hwnd))
		throw std::runtime_error("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(this->m_Hwnd, L" ", L" ");

	SendMessage(this->m_Hwnd, CCM_SETVERSION, (WPARAM)6, (LPARAM)0);

	this->parseListviewExStyles(styles, &ExStyles);

	ListView_SetExtendedListViewStyleEx(this->m_Hwnd, (WPARAM)ExStyles, ExStyles);

	this->m_ToolTipHWND = ListView_GetToolTips(this->m_Hwnd);

	if (this->m_ToolTipHWND != nullptr) {
		if (styles.istok(TEXT("balloon")))
			SetWindowLong(this->m_ToolTipHWND, GWL_STYLE, (LONG)(GetWindowStyle(this->m_ToolTipHWND) | TTS_BALLOON));
		//if (styles.istok(TEXT("tooltips"))) {
		//	this->m_ToolTipHWND = p_Dialog->getToolTip();
		//	AddToolTipToolInfo(this->m_ToolTipHWND,this->m_Hwnd);
		//}
	}

	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);

	DragAcceptFiles(this->m_Hwnd, TRUE);
}

/*!
* \brief blah
*
* blah
*/

DcxListView::~DcxListView() {

	ListView_DeleteAllItems(this->m_Hwnd);

	ImageList_Destroy(this->getImageList(LVSIL_NORMAL));
	ImageList_Destroy(this->getImageList(LVSIL_SMALL));
	ImageList_Destroy(this->getImageList(LVSIL_STATE));

	this->unregistreDefaultWindowProc();
}

const TString DcxListView::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(this->m_Hwnd);
	const auto ExStyles = ListView_GetExtendedListViewStyle(this->m_Hwnd);

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

void DcxListView::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	*ExStyles = WS_EX_CLIENTEDGE;

#if TSTRING_PARTS
	for (const auto &tsStyle : styles)
	{
		if (tsStyle == TEXT("report"))
			*Styles |= LVS_REPORT;
		else if ( tsStyle == TEXT("icon") )
			*Styles |= LVS_ICON;
		else if ( tsStyle == TEXT("smallicon") )
			*Styles |= LVS_SMALLICON;
		else if ( tsStyle == TEXT("list") )
			*Styles |= LVS_LIST;
		else if ( tsStyle == TEXT("noheader") )
			*Styles |= LVS_NOCOLUMNHEADER;
		else if ( tsStyle == TEXT("alignleft") )
			*Styles |= LVS_ALIGNLEFT;
		else if ( tsStyle == TEXT("aligntop") )
			*Styles |= LVS_ALIGNTOP;
		else if ( tsStyle == TEXT("autoarrange") )
			*Styles |= LVS_AUTOARRANGE;
		else if ( tsStyle == TEXT("nolabelwrap") )
			*Styles |= LVS_NOLABELWRAP;
		else if ( tsStyle == TEXT("showsel") )
			*Styles |= LVS_SHOWSELALWAYS;
		else if ( tsStyle == TEXT("singlesel") )
			*Styles |= LVS_SINGLESEL;
		else if ( tsStyle == TEXT("editlabel") )
			*Styles |= LVS_EDITLABELS;
		else if ( tsStyle == TEXT("sortasc") )
			*Styles |= LVS_SORTASCENDING;
		else if ( tsStyle == TEXT("sortdesc") )
			*Styles |= LVS_SORTDESCENDING;
		else if ( tsStyle == TEXT("noscroll") )
			*Styles |= LVS_NOSCROLL;
		else if ( tsStyle == TEXT("noheadersort") )
			*Styles |= LVS_NOSORTHEADER;
	}
#else
	for (auto tsStyle(styles.getfirsttok(1)); !tsStyle.empty(); tsStyle = styles.getnexttok())
	{
		if ( tsStyle == TEXT("report") )
			*Styles |= LVS_REPORT;
		else if ( tsStyle == TEXT("icon") )
			*Styles |= LVS_ICON;
		else if ( tsStyle == TEXT("smallicon") )
			*Styles |= LVS_SMALLICON;
		else if ( tsStyle == TEXT("list") )
			*Styles |= LVS_LIST;
		else if ( tsStyle == TEXT("noheader") )
			*Styles |= LVS_NOCOLUMNHEADER;
		else if ( tsStyle == TEXT("alignleft") )
			*Styles |= LVS_ALIGNLEFT;
		else if ( tsStyle == TEXT("aligntop") )
			*Styles |= LVS_ALIGNTOP;
		else if ( tsStyle == TEXT("autoarrange") )
			*Styles |= LVS_AUTOARRANGE;
		else if ( tsStyle == TEXT("nolabelwrap") )
			*Styles |= LVS_NOLABELWRAP;
		else if ( tsStyle == TEXT("showsel") )
			*Styles |= LVS_SHOWSELALWAYS;
		else if ( tsStyle == TEXT("singlesel") )
			*Styles |= LVS_SINGLESEL;
		else if ( tsStyle == TEXT("editlabel") )
			*Styles |= LVS_EDITLABELS;
		else if ( tsStyle == TEXT("sortasc") )
			*Styles |= LVS_SORTASCENDING;
		else if ( tsStyle == TEXT("sortdesc") )
			*Styles |= LVS_SORTDESCENDING;
		else if ( tsStyle == TEXT("noscroll") )
			*Styles |= LVS_NOSCROLL;
		else if ( tsStyle == TEXT("noheadersort") )
			*Styles |= LVS_NOSORTHEADER;
	}
#endif

	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

void DcxListView::parseListviewExStyles( const TString & styles, LONG * ExStyles )
{
	*ExStyles = LVS_EX_DOUBLEBUFFER;

#if TSTRING_PARTS
	for (const auto &tsStyle : styles)
	{
		if (tsStyle == TEXT("grid"))
			*ExStyles |= LVS_EX_GRIDLINES;
		else if ( tsStyle == TEXT("borderselect") ) 
			*ExStyles |= LVS_EX_BORDERSELECT;
		else if ( tsStyle == TEXT("flatsb") ) 
			*ExStyles |= LVS_EX_FLATSB;
		else if ( tsStyle == TEXT("fullrow") ) 
			*ExStyles |= LVS_EX_FULLROWSELECT;
		else if ( tsStyle == TEXT("checkbox") ) 
			*ExStyles |= LVS_EX_CHECKBOXES;
		else if ( tsStyle == TEXT("headerdrag") ) 
			*ExStyles |= LVS_EX_HEADERDRAGDROP;
		else if ( tsStyle == TEXT("hottrack") ) 
			*ExStyles |= LVS_EX_TRACKSELECT;
		else if ( tsStyle == TEXT("oneclick") ) 
			*ExStyles |= LVS_EX_ONECLICKACTIVATE;
		else if ( tsStyle == TEXT("twoclick") ) 
			*ExStyles |= LVS_EX_TWOCLICKACTIVATE;
		else if ( tsStyle == TEXT("underlinehot") ) 
			*ExStyles |= LVS_EX_UNDERLINEHOT;
		else if ( tsStyle == TEXT("underlinecold") ) 
			*ExStyles |= LVS_EX_UNDERLINECOLD;
		else if ( tsStyle == TEXT("subitemimage") ) 
			*ExStyles |= LVS_EX_SUBITEMIMAGES;
		else if ( tsStyle == TEXT("tooltip") )
			*ExStyles |= LVS_EX_LABELTIP | LVS_EX_INFOTIP;
		else if ( tsStyle == TEXT("transparentbkg") )
			*ExStyles |= LVS_EX_TRANSPARENTBKGND;
		else if ( tsStyle == TEXT("shadowtext") )
			*ExStyles |= LVS_EX_TRANSPARENTSHADOWTEXT;
		else if ( tsStyle == TEXT("autosize") )
			*ExStyles |= LVS_EX_AUTOSIZECOLUMNS;
		else if ( tsStyle == TEXT("headeralways") )
			*ExStyles |= LVS_EX_HEADERINALLVIEWS;
		else if ( tsStyle == TEXT("hidelabels") )
			*ExStyles |= LVS_EX_HIDELABELS;
		else if (tsStyle == TEXT("autocheck"))
			*ExStyles |= LVS_EX_AUTOCHECKSELECT;
		// LVS_EX_COLUMNSNAPPOINTS LVS_EX_JUSTIFYCOLUMNS LVS_EX_SNAPTOGRID LVS_EX_AUTOAUTOARRANGE
	}
#else
	for (auto tsStyle(styles.getfirsttok(1)); !tsStyle.empty(); tsStyle = styles.getnexttok())
	{
		if ( tsStyle == TEXT("grid") ) 
			*ExStyles |= LVS_EX_GRIDLINES;
		else if ( tsStyle == TEXT("borderselect") ) 
			*ExStyles |= LVS_EX_BORDERSELECT;
		else if ( tsStyle == TEXT("flatsb") ) 
			*ExStyles |= LVS_EX_FLATSB;
		else if ( tsStyle == TEXT("fullrow") ) 
			*ExStyles |= LVS_EX_FULLROWSELECT;
		else if ( tsStyle == TEXT("checkbox") ) 
			*ExStyles |= LVS_EX_CHECKBOXES;
		else if ( tsStyle == TEXT("headerdrag") ) 
			*ExStyles |= LVS_EX_HEADERDRAGDROP;
		else if ( tsStyle == TEXT("hottrack") ) 
			*ExStyles |= LVS_EX_TRACKSELECT;
		else if ( tsStyle == TEXT("oneclick") ) 
			*ExStyles |= LVS_EX_ONECLICKACTIVATE;
		else if ( tsStyle == TEXT("twoclick") ) 
			*ExStyles |= LVS_EX_TWOCLICKACTIVATE;
		else if ( tsStyle == TEXT("underlinehot") ) 
			*ExStyles |= LVS_EX_UNDERLINEHOT;
		else if ( tsStyle == TEXT("underlinecold") ) 
			*ExStyles |= LVS_EX_UNDERLINECOLD;
		else if ( tsStyle == TEXT("subitemimage") ) 
			*ExStyles |= LVS_EX_SUBITEMIMAGES;
		else if ( tsStyle == TEXT("tooltip") )
			*ExStyles |= LVS_EX_LABELTIP | LVS_EX_INFOTIP;
		else if ( tsStyle == TEXT("transparentbkg") )
			*ExStyles |= LVS_EX_TRANSPARENTBKGND;
		else if ( tsStyle == TEXT("shadowtext") )
			*ExStyles |= LVS_EX_TRANSPARENTSHADOWTEXT;
		else if ( tsStyle == TEXT("autosize") )
			*ExStyles |= LVS_EX_AUTOSIZECOLUMNS;
		else if ( tsStyle == TEXT("headeralways") )
			*ExStyles |= LVS_EX_HEADERINALLVIEWS;
		else if ( tsStyle == TEXT("hidelabels") )
			*ExStyles |= LVS_EX_HIDELABELS;
		else if (tsStyle == TEXT("autocheck"))
			*ExStyles |= LVS_EX_AUTOCHECKSELECT;
		// LVS_EX_COLUMNSNAPPOINTS LVS_EX_JUSTIFYCOLUMNS LVS_EX_SNAPTOGRID LVS_EX_AUTOAUTOARRANGE
	}
#endif
}

/*!
* \brief $xdid Parsing Function
*
* \param input [NAME] [ID] [PROP] (OPTIONS)
* \param szReturnValue mIRC Data Container
*
* \return > void
*/

void DcxListView::parseInfoRequest( const TString &input, PTCHAR szReturnValue) const
{
	const auto numtok = input.numtok();

	const auto prop(input.getfirsttok(3));
	
	// [NAME] [ID] [PROP] [N] [NSUB]
	if (prop == TEXT("columns")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getColumnCount());
	}
	// [NAME] [ID] [PROP] (N)
	else if (prop == TEXT("columnorder")) {
		// if its a report listview and it has headers
		const auto count = this->getColumnCount();
#if TSTRING_TESTCODE
		const auto col = (numtok > 3 ? input++.to_int() - 1 : -1);	// tok 4
#else
		const auto col = (numtok > 3 ? input.getnexttok( ).to_int() -1 : -1);	// tok 4
#endif

		// invalid column
		if ((col < -1) || (col >= count) || (count <= 0))
			throw std::invalid_argument("Out of Range");

		auto val = std::make_unique<int[]>((UINT)count);

		ListView_GetColumnOrderArray(this->m_Hwnd, count, val.get());

		// increase each value by 1 for easy user indexing
		for (auto i = decltype(count){0}; i < count; i++)
			val[i]++;

		// get specific column
		if (col > -1) {
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), val[col]);
			return;
		}

		// collect all values
		TString buff((UINT)(count * 32));

		for (auto i = decltype(count){0}; i < count; i++)
			buff.addtok(val[i]);

		dcx_strcpyn(szReturnValue, buff.trim().to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP] [N] (NSUB)
	else if (prop == TEXT("text") && numtok > 3) {
#if TSTRING_TESTCODE
		const auto nItem = input++.to_int() - 1;		// tok 4
#else
		const auto nItem = input.getnexttok( ).to_int() - 1;		// tok 4
#endif
		auto nSubItem = 0;
		
		if (numtok > 4)
#if TSTRING_TESTCODE
			nSubItem = input++.to_int() - 1;			// tok 5
#else
			nSubItem = input.getnexttok( ).to_int() -1;			// tok 5
#endif
		if ((nItem < 0) || (nSubItem < 0) || (nItem >= ListView_GetItemCount(this->m_Hwnd)))
			throw std::invalid_argument("Out of Range");
		
		ListView_GetItemText(this->m_Hwnd, nItem, nSubItem, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP] [N] [NSUB]
	else if ( prop == TEXT("icon") && numtok > 4 ) {

#if TSTRING_TESTCODE
		const auto nItem = input++.to_int() - 1;	// tok 4
		const auto nSubItem = input++.to_int() - 1;	// tok 5
#else
		const auto nItem = input.getnexttok( ).to_int( ) - 1;	// tok 4
		const auto nSubItem = input.getnexttok( ).to_int() -1;	// tok 5
#endif

		if ((nItem < 0) || (nSubItem < 0) || (nItem >= ListView_GetItemCount(this->m_Hwnd)) || (nSubItem >= this->getColumnCount()))
			throw std::invalid_argument("Out of Range");

		LVITEM lvi;
		lvi.mask = LVIF_IMAGE;
		lvi.iItem = nItem;
		lvi.iSubItem = nSubItem;

		ListView_GetItem( this->m_Hwnd, &lvi );
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), lvi.iImage + 1 );
	}
	// [NAME] [ID] [PROP] [N]
	else if (prop == TEXT("selected") && (numtok > 3)) {
#if TSTRING_TESTCODE
		const auto nItem = input++.to_int() - 1;	// tok 4
#else
		const auto nItem = input.getnexttok( ).to_int() - 1;	// tok 4
#endif
		// In range
		if ((nItem < 0) || (nItem >= ListView_GetItemCount(this->m_Hwnd)))
			throw std::invalid_argument("Out of Range");

		const auto selected = dcx_testflag(ListView_GetItemState(this->m_Hwnd, nItem, LVIS_SELECTED), LVIS_SELECTED);
		dcx_Con(selected, szReturnValue);
	}
	// [NAME] [ID] [PROP] (NSUB)
	else if ( prop == TEXT("seltext")) {
		const auto nItem = ListView_GetNextItem(this->m_Hwnd, -1, LVIS_SELECTED);
		auto nSubItem = 0;
		
		if (numtok > 3)
#if TSTRING_TESTCODE
			nSubItem = input++.to_int() - 1;	// tok 4
#else
			nSubItem = input.getnexttok( ).to_int() -1;	// tok 4
#endif

		if ((nItem > -1) && (nSubItem > -1))
			ListView_GetItemText( this->m_Hwnd, nItem, nSubItem, szReturnValue, MIRC_BUFFER_SIZE_CCH );
	}
	// [NAME] [ID] [PROP] (N)
	else if (prop == TEXT("sel")) {
		if (this->isStyle(LVS_SINGLESEL)) {
			const auto nItem = ListView_GetNextItem(this->m_Hwnd, -1, LVIS_SELECTED);

			if (nItem > -1)
				wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), nItem + 1);
		}
		// multi select
		else {
			const auto nSelItems = ListView_GetSelectedCount(this->m_Hwnd);
			auto nItem = -1;

			// if we want a specific index
			if (numtok > 3) {
#if TSTRING_TESTCODE
				const auto n = input++.to_<UINT>();	// tok 4
#else
				const auto n = (UINT)input.getnexttok( ).to_int();	// tok 4
#endif
				// sel index out of bounds
				if (n > nSelItems)
					return;

				// return total count of selected files
				if (n == 0) {
					wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), nSelItems);
					return;
				}

				for (auto i = decltype(n){1}; i <= n; i++)	// find the Nth selected item.
					nItem = ListView_GetNextItem(this->m_Hwnd, nItem, LVIS_SELECTED);

				if (nItem != -1)
					wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), nItem +1);
			}

			// otherwise we want a list of indexes (comma seperated)
			else if (nSelItems > 0) {
				TString list;

				while ((nItem = ListView_GetNextItem(this->m_Hwnd, nItem, LVIS_SELECTED)) != -1)
					list.addtok((nItem + 1), TSCOMMA);

				dcx_strcpyn(szReturnValue, list.to_chr(), MIRC_BUFFER_SIZE_CCH);
			}
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("selnum") ) {

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), ListView_GetSelectedCount( this->m_Hwnd ) );
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("state") && numtok > 3 ) {

#if TSTRING_TESTCODE
		const auto nItem = input++.to_int() - 1;	// tok 4
#else
		const auto nItem = input.getnexttok( ).to_int( ) - 1;	// tok 4
#endif
		if ((nItem < 0) || (nItem >= ListView_GetItemCount(this->m_Hwnd)))
			throw std::invalid_argument("Out of Range");

		if ( this->isListViewStyle( LVS_REPORT ) ) {

			const auto state = ListView_GetItemState(this->m_Hwnd, nItem, LVIS_STATEIMAGEMASK);

			if (state == 8192) {
				szReturnValue[0] = TEXT('2');
				szReturnValue[1] = TEXT('\0');
			}
			else
				dcx_ConChar(state == 4096, szReturnValue);
		}
		else
			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), ListView_GetItemState( this->m_Hwnd, nItem, LVIS_STATEIMAGEMASK ) );
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("num") ) {

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), ListView_GetItemCount( this->m_Hwnd ) );
	}
	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [COLUMN] [N]
	else if ( prop == TEXT("find") && numtok > 6 ) {

		const auto matchtext(input.getfirsttok(2, TSTAB).trim());
		const auto params(input.getnexttok(TSTAB).trim());			// tok 3

		if ( !matchtext.empty() ) {

			ListView_SearchTypes SearchType = LVSEARCH_E;
#if TSTRING_TESTCODE
			const auto searchMode(params++[0]);
#else
			const auto searchMode(params.getfirsttok( 1 )[0]);
#endif
			if (searchMode == TEXT('R'))
				SearchType = LVSEARCH_R;
			else if (searchMode == TEXT('W'))
				SearchType = LVSEARCH_W;

#if TSTRING_TESTCODE
			const auto nColumn = params++.to_int() - 1;	// tok 2
			const auto N = params++.to_int();			// tok 3
#else
			const auto nColumn = params.getnexttok( ).to_int( ) - 1;	// tok 2
			const auto N = params.getnexttok( ).to_int( );			// tok 3
#endif
			const auto nItems = ListView_GetItemCount(this->m_Hwnd);
			const auto nColumns = this->getColumnCount();
			auto count = 0;

			// count total
			if ( N == 0 ) {
				// Search all columns
				if ( nColumn == -1 ) {

					for (auto i = decltype(nItems){0}; i < nItems; i++) {
						for (auto k = decltype(nColumns){0}; k < nColumns; k++) {

							if ( this->matchItemText( i, k, &matchtext, SearchType ) )
								count++;
						}
					}
				}
				// Particular Column
				else {

					if (nColumn < -1 || nColumn >= nColumns)
						throw std::invalid_argument("Out of Range");

					for (auto i = decltype(nItems){0}; i < nItems; i++) {

						if ( this->matchItemText( i, nColumn, &matchtext, SearchType ) )
							count++;
					}
				}

				wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), count );
			} // if ( N == 0 )
			// find Nth matching
			else {
				// Search all columns
				if ( nColumn == -1 ) {

					for (auto i = decltype(nItems){0}; i < nItems; i++) {

						for (auto k = decltype(nColumns){0}; k < nColumns; k++) {

							if ( this->matchItemText( i, k, &matchtext, SearchType ) )
								count++;

							// found Nth matching
							if ( count == N )
								wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), i + 1, k + 1 );
						}
					}
				}
				// Particular Column
				else {

					if ( nColumn < -1 || nColumn >= nColumns )
						throw std::invalid_argument("Out of Range");

					for (auto i = decltype(nItems){0}; i < nItems; i++) {

						if ( this->matchItemText( i, nColumn, &matchtext, SearchType ) )
							count++;

						// found Nth matching
						if ( count == N )
							wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), i + 1, nColumn + 1 );
					}
				} //else
			} // else
		} // if ( !matchtext.empty() )
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("tbitem") ) {

		if ( this->isStyle( LVS_REPORT ) || this->isStyle( LVS_LIST ) )
			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), this->getTopIndex( ) + 1, this->getBottomIndex( ) + 1 );
	}
	else if ( prop == TEXT("mouseitem") ) {

		LVHITTESTINFO lvh;
		if (GetCursorPos(&lvh.pt))
		{
			MapWindowPoints(nullptr, this->m_Hwnd, &lvh.pt, 1);
			ListView_SubItemHitTest(this->m_Hwnd, &lvh);

			if (dcx_testflag(lvh.flags, LVHT_ONITEM))
				wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), lvh.iItem + 1, lvh.iSubItem + 1);
			else
				dcx_strcpyn(szReturnValue, TEXT("-1 -1"), MIRC_BUFFER_SIZE_CCH);
		}
	}
	// [NAME] [ID] [PROP] (N)
	else if (prop == TEXT("hwidth")) {
		auto nColumn = -1;

		if (numtok > 3)
			nColumn = input.getnexttok( ).to_int() - 1;	// tok 4

		const auto count = this->getColumnCount();

		// return all columns
		if (nColumn == -1) {
			TString buff((UINT)(count *32));

			for (auto i = decltype(count){0}; i < count; i++)
				buff.addtok( ListView_GetColumnWidth(this->m_Hwnd, i) );

			dcx_strcpyn(szReturnValue, buff.trim().to_chr(), MIRC_BUFFER_SIZE_CCH);
		}
		else {
			if (nColumn < 0 || nColumn >= count)
				throw std::invalid_argument("Column Out Of Range");

			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), ListView_GetColumnWidth(this->m_Hwnd, nColumn));
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("htext") && numtok > 3 ) {

		const auto nColumn = input.getnexttok( ).to_int( ) - 1;	// tok 4

		if ( nColumn < 0 || nColumn >= this->getColumnCount( ) )
			throw std::invalid_argument("Column Out Of Range");

		LVCOLUMN lvc;
		ZeroMemory( &lvc, sizeof( LVCOLUMN ) );
		lvc.mask = LVCF_TEXT;
		lvc.cchTextMax = MIRC_BUFFER_SIZE_CCH;
		lvc.pszText = szReturnValue;

		ListView_GetColumn(this->m_Hwnd, nColumn, &lvc);
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("hicon") && numtok > 3 ) {

		const auto nColumn = input.getnexttok().to_int() - 1;	// tok 4

		if (nColumn < 0 || nColumn >= this->getColumnCount())
			throw std::invalid_argument("Column Out Of Range");

		LVCOLUMN lvc;
		ZeroMemory( &lvc, sizeof( LVCOLUMN ) );
		lvc.mask = LVCF_IMAGE;

		if ( ListView_GetColumn( this->m_Hwnd, nColumn, &lvc ) )
			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), lvc.iImage + 1 );
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("hstate") && numtok == 4 ) {

		auto h = ListView_GetHeader(this->m_Hwnd);
		if (!IsWindow(h))
			throw std::runtime_error("Unable to get Header Window");

		const auto nCol = (input.getnexttok().to_int() - 1);	// tok 4

		if (nCol < 0 || nCol >= this->getColumnCount())
			throw std::invalid_argument("Column Out Of Range");

		TString tsRes;
		HDITEM hdr = {0};
		hdr.mask = HDI_FORMAT;

		if (!Header_GetItem(h, nCol, &hdr))
			throw std::runtime_error("Unable to get Header Info");

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

		dcx_strcpyn(szReturnValue, tsRes.to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP] [GID]
	else if ( prop == TEXT("gtext") && numtok > 3 ) {

		const auto GID = input.getnexttok().to_int();	// tok 4

		auto wstr = std::make_unique<WCHAR[]>(MIRC_BUFFER_SIZE_CCH + 1);
		wstr[0] = TEXT('\0');

		LVGROUP lvg;
		ZeroMemory( &lvg, sizeof( LVGROUP ) );
		lvg.cbSize = sizeof( LVGROUP );
		lvg.mask = LVGF_HEADER;
		lvg.cchHeader = MIRC_BUFFER_SIZE_CCH;
		lvg.pszHeader = wstr.get();

		if ( ListView_GetGroupInfo( this->m_Hwnd, GID, &lvg ) != -1 )
			dcx_strcpyn(szReturnValue, lvg.pszHeader, MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("genabled") ) {

		dcx_Con(ListView_IsGroupViewEnabled(this->m_Hwnd), szReturnValue);
	}
	// [NAME] [ID] [PROP] [N] [NSUB] [PBARPROP] [PARAM]
	else if ((prop == TEXT("pbar")) && (numtok > 5)) {
		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4
		const auto nSubItem = input.getnexttok().to_int() - 1;	// tok 5

		if (nItem < 0 || nSubItem < 0 || nItem >= ListView_GetItemCount(this->m_Hwnd))
			throw std::invalid_argument("Out of Range");

		LVITEM lvi;

		ZeroMemory(&lvi, sizeof(LVITEM));
		lvi.mask = LVIF_PARAM;
		lvi.iItem = nItem;

		if (!ListView_GetItem(this->m_Hwnd, &lvi))
			throw std::runtime_error("Unable to get Item");

		auto lvdcx = reinterpret_cast<LPDCXLVITEM>(lvi.lParam);

		if (lvdcx == nullptr || lvdcx->pbar == nullptr || lvdcx->iPbarCol != nSubItem)
			throw std::invalid_argument("No Progessbar Here");

		//const TString cmd(input.gettok( 1 ) + TEXT(" ") + input.gettok( 2 ) + TEXT(" ") + input.getlasttoks());	// tok 6, -1
		//const TString cmd(this->m_pParentDialog->getName() + TEXT(" ") + input.gettok(2) + TEXT(" ") + input.getlasttoks());	// tok 6, -1
		const TString cmd{ this->m_pParentDialog->getName(), TEXT(" "), input.gettok(2), TEXT(" "), input.getlasttoks() };	// tok 6, -1
		lvdcx->pbar->parseInfoRequest(cmd, szReturnValue);
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("gnum") ) {
		if (Dcx::VistaModule.isVista())
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), ListView_GetGroupCount(this->m_Hwnd));
		else {
			int gcount = 0;
			for (auto g = 0; g < 256; g++) { if (ListView_HasGroup(this->m_Hwnd, (WPARAM)g)) gcount++; }
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), gcount);
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("gid") ) {
		const auto iIndex = input.getnexttok().to_int() - 1;	// tok 4

		if (iIndex < 0 || iIndex >= ListView_GetItemCount(this->m_Hwnd))
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid Item: %d"), iIndex));

		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));
		lvi.iItem = iIndex;
		lvi.mask = LVIF_GROUPID;

		if (!ListView_GetItem(this->m_Hwnd, &lvi))
			throw std::runtime_error("Unable to get Group ID");

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), lvi.iGroupId); // group id can be -2 (Not In group), -1 (groupcallback, should never be), 0+ groupid
	}
	// [NAME] [ID] [PROP] [ROW] [COL]
	else if (prop == TEXT("markeditem")) {
		auto nRow = input.getnexttok().to_int();	// tok 4
		auto nCol = input.getnexttok().to_int();	// tok 5

		// 1-based indexes.
		if ((nRow < 1) || (nRow > ListView_GetItemCount(this->m_Hwnd)))
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid item index %d"), nRow));

		if ((nCol < 1) || (nCol > this->getColumnCount()))
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid column index %d"), nCol));

		// Convert to 0-index
		nRow--;
		nCol--;

		LVITEM lvi;
		lvi.mask = LVIF_PARAM;
		lvi.iItem = nRow;
		lvi.iSubItem = nCol;

		if (!ListView_GetItem(this->m_Hwnd, &lvi))
			throw std::runtime_error(Dcx::dcxGetFormattedString(TEXT("Unable to get item: %d %d"), nRow, nCol));
		
		dcx_strcpyn(szReturnValue, ((LPDCXLVITEM)lvi.lParam)->tsMark.to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("emptytext")) {
		if (Dcx::VistaModule.isUseable())
			ListView_GetEmptyText(this->m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP] [GID]
	else if ( prop == TEXT("gstate") && numtok == 4 ) {

		TString tsFlags('+');

		if (Dcx::VistaModule.isVista()) {
			const auto gid = input.getnexttok().to_int();	// tok 4
			const UINT iMask = LVGS_COLLAPSIBLE|LVGS_HIDDEN|LVGS_NOHEADER|LVGS_COLLAPSED|LVGS_SELECTED;

			const auto iState = ListView_GetGroupState(this->m_Hwnd, gid, iMask);

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
		dcx_strcpyn(szReturnValue, tsFlags.to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP] [flags]
	else if (prop == TEXT("icons") && numtok == 4) {
		const auto iFlags = parseIconFlagOptions(input.getnexttok());	// tok 4

		UINT iCount = 0;
		if (dcx_testflag(iFlags, LVSIL_SMALL)) {
			auto himl = getImageList(LVSIL_NORMAL);
			if (himl != nullptr)
				iCount += ImageList_GetImageCount(himl);
		}
		if (dcx_testflag(iFlags, LVSIL_STATE)) {
			auto himl = getImageList(LVSIL_STATE);
			if (himl != nullptr)
				iCount += ImageList_GetImageCount(himl);
		}
		wsprintf(szReturnValue, TEXT("%u"), iCount);
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

void DcxListView::autoSize(const int nColumn, const TString &flags)
{
	this->autoSize(nColumn, this->parseHeaderFlags2(flags));
}

void DcxListView::autoSize(const int nColumn, const int iFlags, const int iWidth)
{
	if (dcx_testflag(iFlags, DCX_LV_COLUMNF_FIXED))
		ListView_SetColumnWidth(this->m_Hwnd, nColumn, iWidth);
	else if (dcx_testflag(iFlags, DCX_LV_COLUMNF_AUTOMAX)) {
		int n = 0;
		ListView_SetColumnWidth(this->m_Hwnd, nColumn, LVSCW_AUTOSIZE);
		n = ListView_GetColumnWidth(this->m_Hwnd, nColumn);
		ListView_SetColumnWidth(this->m_Hwnd, nColumn, LVSCW_AUTOSIZE_USEHEADER);
		n = max(ListView_GetColumnWidth(this->m_Hwnd, nColumn),n);
		ListView_SetColumnWidth(this->m_Hwnd, nColumn, n);
	}
	else if (dcx_testflag(iFlags, DCX_LV_COLUMNF_AUTO))
		ListView_SetColumnWidth(this->m_Hwnd, nColumn, LVSCW_AUTOSIZE);
	else if (dcx_testflag(iFlags, DCX_LV_COLUMNF_AUTOHEADER))
		ListView_SetColumnWidth(this->m_Hwnd, nColumn, LVSCW_AUTOSIZE_USEHEADER);
	//else if (dcx_testflag(iFlags, DCX_LV_COLUMNF_PERCENT))
	//	ListView_SetColumnWidth(this->m_Hwnd, nColumn, 0);
}

/*!
* \brief blah
*
* blah
*/

void DcxListView::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));

	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		ListView_DeleteAllItems(this->m_Hwnd);

	//xdid -a [NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
	//xdid -a -> [NAME] [ID] -a [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
	if (flags[TEXT('a')] && numtok > 12) {
		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));

		const auto data(input.gettok(1, TSTAB).gettok(4, -1).trim());
		auto nPos = data.gettok(1).to_int() - 1;

		if (nPos < 0)
			nPos = ListView_GetItemCount(this->m_Hwnd);

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
			this->xLoadListview(nPos, data, TEXT("$hget(%s)"), TEXT("$hget(%s,0).item"), TEXT("$hget(%s,%d)"), TEXT("$hget(%s,%s)"));
			return;
		}

		if (dcx_testflag(stateFlags, LVIS_WINDOW))
		{
			// load all data from a mIRC @window.
			this->xLoadListview(nPos, data, TEXT("$window(%s)"), TEXT("$line(%s,0)"), TEXT("$line(%s,%d)"), nullptr);
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
	else if (flags[TEXT('A')]) {
		if (numtok < 7)
			throw std::invalid_argument("Insufficient parameters");

#if TSTRING_TESTCODE

		auto nRow = input++.to_int();	// tok 4
		auto nCol = input++.to_int();	// tok 5
#else
		auto nRow = input.getnexttok( ).to_int();	// tok 4
		auto nCol = input.getnexttok( ).to_int();	// tok 5
#endif
		// We're currently checking 1-based indexes.
		if ((nRow < 1) || (nRow > ListView_GetItemCount(this->m_Hwnd)))
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid row index %d."), nRow));

		if ((nCol < 1) || (nCol > this->getColumnCount()))
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid column index %d."), nCol));

		// Convert to 0-based index.
		nRow--;
		nCol--;

		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));

		lvi.mask = LVIF_PARAM;
		lvi.iItem = nRow;
		lvi.iSubItem = nCol;

		// Couldnt retrieve info
		if (!ListView_GetItem(this->m_Hwnd, &lvi))
			throw std::runtime_error("Unable to get item.");

		auto lviDcx = reinterpret_cast<LPDCXLVITEM>(lvi.lParam);

		if (lviDcx == nullptr) 
			throw std::runtime_error("Unable to Retrieve Item Data");
		
#if TSTRING_TESTCODE
		const XSwitchFlags xflag(input++);	// tok 6
		const auto info(input++);		// tok 7, -1
#else
		const XSwitchFlags xflag(input.getnexttok());	// tok 6
		const auto info(input.getlasttoks());		// tok 7, -1
#endif
		if (!xflag[TEXT('+')])
			throw std::invalid_argument("Missing '+' in flags");

		if (!xflag[TEXT('M')])	// mark info
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unknown flags %s"), input.gettok(6).to_chr()));
	
		lviDcx->tsMark = info;
	}
	// xdid -B [NAME] [ID] [N]
	// xdid -B -> [NAME] [ID] -B [N]
	else if (flags[TEXT('B')] && numtok > 3) {
		auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		// check if item supplied was 0 (now -1), last item in list
		if (nItem == -1) {
			nItem = ListView_GetItemCount(this->m_Hwnd) -1;

			if (nItem < 0)
				throw std::invalid_argument("Invalid Item: No Items in list");
		}
		if (nItem < 0)
			throw std::invalid_argument("Invalid Item");

		if (GetFocus() != this->m_Hwnd)
			SetFocus(this->m_Hwnd);

		ListView_EditLabel(this->m_Hwnd, nItem);
	}
	// xdid -c [NAME] [ID] [N]
	// xdid -c -> [NAME] [ID] -c [N]
	else if (flags[TEXT('c')] && numtok > 3) {
		auto nItemCnt = ListView_GetItemCount(this->m_Hwnd);
		if (nItemCnt < 1)
			throw std::invalid_argument("Invalid Item: No Items in list");

		if (this->isStyle(LVS_SINGLESEL)) {
			auto nItem = input.getnexttok().to_int() - 1;	// tok 4

			if (nItem == -1)
				nItem = nItemCnt -1;

			if (nItem > -1)
				ListView_SetItemState(this->m_Hwnd, nItem, LVIS_SELECTED, LVIS_SELECTED);
		}
		else {
#if TSTRING_PARTS
			const auto Ns(input.getnexttok());	// tok 4
#if TSTRING_ITERATOR
			for (auto itStart = Ns.begin(TSCOMMA), itEnd = Ns.end(); itStart != itEnd; ++itStart)
			{
				const auto tsLine(*itStart);
				auto iStart = 0, iEnd = 0;
				this->getItemRange(tsLine, nItemCnt, &iStart, &iEnd);
				if ( (iStart < 0) || (iEnd < 0) || (iStart >= nItemCnt) || (iEnd >= nItemCnt) )
					throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid index %s."), tsLine.to_chr()));

				for (auto nItem = iStart; nItem <= iEnd; nItem++)
					ListView_SetItemState(this->m_Hwnd, nItem, LVIS_SELECTED, LVIS_SELECTED);
			}
#else
			Ns.split(TSCOMMA);
			for (const auto &tsLine: Ns) {
				auto iStart = 0, iEnd = 0;
				this->getItemRange(tsLine, nItemCnt, &iStart, &iEnd);
				if ( (iStart < 0) || (iEnd < 0) || (iStart >= nItemCnt) || (iEnd >= nItemCnt) )
					throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid index %s."), tsLine.to_chr()));

				for (auto nItem = iStart; nItem <= iEnd; nItem++)
					ListView_SetItemState(this->m_Hwnd, nItem, LVIS_SELECTED, LVIS_SELECTED);
			}
#endif
#else
			const auto Ns(input.getnexttok( ));	// tok 4
			for (auto tsLine(Ns.getfirsttok(1, TSCOMMA)); !tsLine.empty(); tsLine = Ns.getnexttok(TSCOMMA)) {
				auto iStart = 0, iEnd = 0;
				this->getItemRange(tsLine, nItemCnt, &iStart, &iEnd);
				if ( (iStart < 0) || (iEnd < 0) || (iStart >= nItemCnt) || (iEnd >= nItemCnt) )
					throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid index %s."), tsLine.to_chr()));

				for (auto nItem = iStart; nItem <= iEnd; nItem++)
					ListView_SetItemState(this->m_Hwnd, nItem, LVIS_SELECTED, LVIS_SELECTED);
			}
#endif
		}
	}
	// xdid -d [NAME] [ID] [SWITCH] [N,N2,N3-N4...]
	// xdid -d [NAME] [ID] -d [N,N2,N3-N4...]
	// xdid -d [NAME] [ID] -d [N] [+flags] [subitem] [match text]
	else if (flags[TEXT('d')] && (numtok > 3)) {
		const auto Ns(input.getnexttok());	// tok 4
		const XSwitchFlags xFlags(input.getnexttok());	// tok 5

		if (xFlags[TEXT('+')]) {
			// have flags, so its a match text delete
			const auto nSubItem = input.getnexttok().to_int();
			const auto tsMatchText(input.getnexttok());
			auto SearchType = LVSEARCH_E;	// plain text exact match delete
			const auto nItems = ListView_GetItemCount(this->m_Hwnd);

			if (xFlags[TEXT('w')])
				SearchType = LVSEARCH_W;	// wildcard delete
			else if (xFlags[TEXT('r')])
				SearchType = LVSEARCH_R;	// regex delete

			for (auto nPos = Ns.to_int(); nPos < nItems; nPos++) {

				if (this->matchItemText(nPos, nSubItem, &tsMatchText, SearchType))
					ListView_DeleteItem(this->m_Hwnd, nPos--);		// NB: we do nPos-- here as a lines just been removed so we have to check the same nPos again
			}
		}
		else {
#if TSTRING_PARTS
#if TSTRING_ITERATOR
			for (auto itStart = Ns.begin(TSCOMMA), itEnd = Ns.end(); itStart != itEnd; ++itStart)
			{
				auto iStart = 0, iEnd = 0;
				const auto tsLine(*itStart);
				const auto nItemCnt = ListView_GetItemCount(this->m_Hwnd);

				this->getItemRange(tsLine, nItemCnt, &iStart, &iEnd);

				if ((iStart < 0) || (iEnd < iStart) || (iStart >= nItemCnt) || (iEnd >= nItemCnt))
					throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid index %s."), tsLine.to_chr()));

				for (auto nItem = iStart; nItem <= iEnd; nItem++)
					ListView_DeleteItem(this->m_Hwnd, nItem);
			}
#else
			Ns.split(TSCOMMA);

			for (const auto &tsLine: Ns)
			{
				auto iStart = 0, iEnd = 0;
				const auto nItemCnt = ListView_GetItemCount(this->m_Hwnd);

				this->getItemRange(tsLine, nItemCnt, &iStart, &iEnd);

				if ((iStart < 0) || (iEnd < iStart) || (iStart >= nItemCnt) || (iEnd >= nItemCnt))
					throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid index %s."), tsLine.to_chr()));

				for (auto nItem = iStart; nItem <= iEnd; nItem++)
					ListView_DeleteItem(this->m_Hwnd, nItem);
			}
#endif
#else
			for (auto tsLine(Ns.getfirsttok(1, TSCOMMA)); !tsLine.empty(); tsLine = Ns.getnexttok(TSCOMMA)) {
				auto iStart = 0, iEnd = 0;
				const auto nItemCnt = ListView_GetItemCount(this->m_Hwnd);
				this->getItemRange(tsLine, nItemCnt, &iStart, &iEnd);
				if ((iStart < 0) || (iEnd < iStart) || (iStart >= nItemCnt) || (iEnd >= nItemCnt))
					throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid index %s."), tsLine.to_chr()));

				for (auto nItem = iStart; nItem <= iEnd; nItem++)
					ListView_DeleteItem(this->m_Hwnd, nItem);
			}
#endif
		}
	}
	// xdid -g [NAME] [ID] [SWITCH] [+FLAGS] [X] [Y] (FILENAME) ([tab] watermark filename)
	else if (flags[TEXT('g')] && numtok > 5) {
		LVBKIMAGE lvbki;
		ZeroMemory(&lvbki, sizeof(LVBKIMAGE));
		TString filename;

#if TSTRING_TESTCODE
		lvbki.ulFlags = this->parseImageFlags(input++);	// tok 4
		lvbki.xOffsetPercent = input++.to_int();	// tok 5
		lvbki.yOffsetPercent = input++.to_int();	// tok 6
#else
		lvbki.ulFlags = this->parseImageFlags(input.getnexttok( ));	// tok 4
		lvbki.xOffsetPercent = input.getnexttok( ).to_int();	// tok 5
		lvbki.yOffsetPercent = input.getnexttok( ).to_int();	// tok 6
#endif
		if (numtok > 6) {
			filename = input.getlasttoks().trim();				// tok 7, -1
			// make sure path exists & path is complete.
			if (!IsFile(filename)) {
				// if not a file path check if its a url.
				if (!PathIsURL(filename.to_chr()))
					throw std::runtime_error(Dcx::dcxGetFormattedString(TEXT("Unable To Access File: %s"), filename.to_chr()));
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

		ListView_SetBkImage(this->m_Hwnd, &lvbki);
	}
	// xdid -i [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if (flags[TEXT('i')] && numtok > 4) {
#if TSTRING_TESTCODE
		const auto iColorFlags = this->parseColorFlags(input++);	// tok 4
		const auto tsClr(input++);	// tok 5
		const auto clrColor = tsClr.to_<COLORREF>();
#else
		const auto iColorFlags = this->parseColorFlags(input.getnexttok( ));	// tok 4
		const auto tsClr(input.getnexttok( ));	// tok 5
		const auto clrColor = (COLORREF)tsClr.to_num();
#endif

		if (dcx_testflag(iColorFlags, LVCS_TEXT))
			ListView_SetTextColor(this->m_Hwnd, clrColor);

		if (dcx_testflag(iColorFlags, LVCS_BKG)) {
			if (tsClr == TEXT("none"))
				ListView_SetBkColor(this->m_Hwnd, CLR_NONE);
			else
				ListView_SetBkColor(this->m_Hwnd, clrColor);
		}

		if (dcx_testflag(iColorFlags, LVCS_BKGTEXT)) {
			if (tsClr == TEXT("none"))
				ListView_SetTextBkColor(this->m_Hwnd, CLR_NONE);
			else
				ListView_SetTextBkColor(this->m_Hwnd, clrColor);
		}

		if (dcx_testflag(iColorFlags, LVCS_OUTLINE))
			ListView_SetOutlineColor(this->m_Hwnd, clrColor);

		this->redrawWindow();
	}
	// xdid -j [NAME] [ID] [ROW] [COL] [FLAGS] ([COLOUR] (BGCOLOUR))
	// [NAME] [ID] -j [ROW] [COL] [FLAGS] ([COLOUR] (BGCOLOUR))
	else if (flags[TEXT('j')] && numtok > 5) {
#if TSTRING_TESTCODE
		auto nItem = input++.to_int() - 1;						// tok 4
		const auto nCol = input++.to_int() - 1;					// tok 5
		const auto lviflags = this->parseItemFlags(input++);	// tok 6
		const auto clrText = input++.to_<COLORREF>();	// tok 7
		const auto clrBack = input++.to_<COLORREF>();	// tok 8
#else
		auto nItem = input.getnexttok( ).to_int() -1;						// tok 4
		const auto nCol = input.getnexttok( ).to_int() -1;					// tok 5
		const auto lviflags = this->parseItemFlags(input.getnexttok( ));	// tok 6
		const auto clrText = (COLORREF)input.getnexttok( ).to_num();	// tok 7
		const auto clrBack = (COLORREF)input.getnexttok( ).to_num();	// tok 8
#endif

		if (nItem == -1) {
			nItem = ListView_GetItemCount(this->m_Hwnd) -1;

			if (nItem < 0)
				throw std::invalid_argument("Invalid Item: No Items in list");
		}

		// invalid info
		if ((nItem < 0) || (nCol < 0) || (nCol >= this->getColumnCount()))
			throw std::invalid_argument("Invalid Item");

		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));

		lvi.mask = LVIF_PARAM | LVIF_STATE;
		lvi.iItem = nItem;
		lvi.iSubItem = nCol;

		// couldn't retrieve info
		if (!ListView_GetItem(this->m_Hwnd, &lvi))
			throw std::runtime_error("Unable to get Item.");

		auto lviDcx = reinterpret_cast<LPDCXLVITEM>(lvi.lParam);

		if (lviDcx == nullptr)
			throw std::runtime_error("No DCX Item Information, somethings very wrong");

		if ((UINT)nCol >= lviDcx->vInfo.size())
			throw std::runtime_error("No Render Information for SubItem, More subitems than columns?");

		auto ri = lviDcx->vInfo[(UINT)nCol];

		ri->m_dFlags = lviflags;
		if (dcx_testflag(lviflags, LVIS_COLOR))
			ri->m_cText = clrText;
		else
			ri->m_cText = CLR_INVALID;

		if (dcx_testflag(lviflags, LVIS_BGCOLOR))
			ri->m_cBg = clrBack;
		else
			ri->m_cBg = CLR_INVALID;

		ListView_SetItemState(this->m_Hwnd, nItem, lviflags, LVIS_DROPHILITED | LVIS_FOCUSED | LVIS_SELECTED | LVIS_CUT);
	}
	// xdid -k [NAME] [ID] [SWITCH] [STATE] [N]
	// xdid -k -> [NAME] [ID] -k [STATE] [N]
	else if (flags[TEXT('k')] && numtok > 4) {
		const auto state = input.getnexttok().to_int();	// tok 4
		const auto Ns(input.getnexttok());	// tok 5
		const auto nItemCnt = ListView_GetItemCount(this->m_Hwnd);

#if TSTRING_PARTS
#if TSTRING_ITERATOR
		for (auto itStart = Ns.begin(TSCOMMA), itEnd = Ns.end(); itStart != itEnd; ++itStart)
		{
			auto iStart = 0, iEnd = 0;
			const auto tsLine(*itStart);
			this->getItemRange(tsLine, nItemCnt, &iStart, &iEnd);

			if ( (iStart < 0) || (iEnd < 0) || (iStart >= nItemCnt) || (iEnd >= nItemCnt) )
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid index %s."), tsLine.to_chr()));

			for (auto nItem = iStart; nItem <= iEnd; nItem++)
				ListView_SetItemState(this->m_Hwnd, nItem, (UINT)INDEXTOSTATEIMAGEMASK(state), LVIS_STATEIMAGEMASK);
		}
#else
		Ns.split(TSCOMMA);

		for (const auto &tsLine: Ns)
		{
			auto iStart = 0, iEnd = 0;
			this->getItemRange(tsLine, nItemCnt, &iStart, &iEnd);

			if ( (iStart < 0) || (iEnd < 0) || (iStart >= nItemCnt) || (iEnd >= nItemCnt) )
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid index %s."), tsLine.to_chr()));

			for (auto nItem = iStart; nItem <= iEnd; nItem++)
				ListView_SetItemState(this->m_Hwnd, nItem, INDEXTOSTATEIMAGEMASK(state), LVIS_STATEIMAGEMASK);
		}
#endif
#else
		for (auto tsLine(Ns.getfirsttok(1, TSCOMMA)); !tsLine.empty(); tsLine = Ns.getnexttok(TSCOMMA)) {
			auto iStart = 0, iEnd = 0;
			this->getItemRange(tsLine, nItemCnt, &iStart, &iEnd);

			if ( (iStart < 0) || (iEnd < 0) || (iStart >= nItemCnt) || (iEnd >= nItemCnt) )
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid index %s."), tsLine.to_chr()));

			for (auto nItem = iStart; nItem <= iEnd; nItem++)
				ListView_SetItemState(this->m_Hwnd, nItem, INDEXTOSTATEIMAGEMASK(state), LVIS_STATEIMAGEMASK);
		}
#endif
	}
	// xdid -l [NAME] [ID] [SWITCH] [N] [M] [ICON] (OVERLAY)
	else if (flags[TEXT('l')] && numtok > 5) {
		auto nItem = input.getnexttok().to_int() - 1;							// tok 4
		const auto nSubItem = input.getnexttok().to_int() - 1;					// tok 5
		const auto nIcon = input.getnexttok().to_int() - 1;					// tok 6
		const auto nOverlay = (numtok > 6 ? input.getnexttok().to_int() : -1);	// tok 7

		// check if item supplied was 0 (now -1), last item in list.
		if (nItem == -1) {
			nItem = ListView_GetItemCount(this->m_Hwnd) -1;

			if (nItem < 0)
				throw std::invalid_argument("Invalid Item: No Items in list");
		}

		// invalid item
		if ((nItem < 0) || (nSubItem < 0) || (nSubItem >= this->getColumnCount()))
			throw std::invalid_argument("Invalid Item");

		/*
			nIcon = 0 (use no icon)
			nIcon = -1 (ignore value, just change overlay)

			overlay = 0 (no icon)
		*/
		// no icon to change
		if ((nIcon < -1) && (nOverlay < 0))
			return;	// not an error, just do nothing.

		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));

		lvi.iItem = nItem;
		lvi.iSubItem = nSubItem;

		// theres an icon to change
		if (nIcon > -2) {
			lvi.mask = LVIF_IMAGE;
			lvi.iImage = nIcon;
		}

		if (nOverlay > -1) {
			lvi.mask |= LVIF_STATE;
			lvi.stateMask = LVIS_OVERLAYMASK;
			lvi.state = (UINT)INDEXTOOVERLAYMASK(nOverlay);
		}

		ListView_SetItem(this->m_Hwnd, &lvi);
	}
	// xdid -m [NAME] [ID] [SWITCH] [0|1]
	else if (flags[TEXT('m')] && numtok > 3) {
		ListView_EnableGroupView(this->m_Hwnd, (input.getnexttok() == TEXT('1')));	// tok 4
	}
	// xdid -n [NAME] [ID] [SWITCH] [N] [+FLAGS] (WIDTH) ...
	else if (flags[TEXT('n')] && numtok > 4) {
		auto nColumn = (input.getnexttok().to_int() - 1);	// tok 4
		const XSwitchFlags xflags(input.getnexttok( ));		// tok 5
		const UINT iTotal = (UINT)this->getColumnCount();

		this->setRedraw(FALSE);	// disable redraw while setting sizes
		Auto(this->setRedraw(TRUE));	// auto re-enable drawing

		// manually set widths for all specified columns
		if (xflags[TEXT('m')]) {
			// xdid -n dname id -1 +m width1 width2 ...

			if ((numtok - 6) < iTotal)
				throw std::invalid_argument("Insufficient number of widths specified for +m flag");

			for (auto i = decltype(iTotal){1}; i <= iTotal; i++)
				ListView_SetColumnWidth(this->m_Hwnd, i - 1, input.getnexttok().to_int());	// tok 6+
		}
		else if (xflags[TEXT('d')]) {
			// dynamic widths...
			if ((numtok - 6) < iTotal)
				throw std::invalid_argument("Insufficient number of widths specified for +d flag");

			if ((nColumn < 0) || ((UINT)nColumn >= iTotal))
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid column specified: %d"), nColumn +1));

			const auto iFlags = this->parseHeaderFlags2(xflags);
			const auto iWidth = input.getnexttok().to_int();	// tok 6

			for (auto &a : this->m_vWidths)
			{
				if (a->m_iColumn == nColumn) {
					// column info found
					a->m_dFlags = (DWORD)iFlags;
					a->m_iSize = iWidth;
					break;
				}
			}
			this->autoSize(nColumn, iFlags, iWidth);
		}
		else {
			const auto iFlags = this->parseHeaderFlags2(xflags);
			const auto iWidth = input.getnexttok().to_int();	// tok 6
			UINT iCount;

			if ((iFlags == 0) && (numtok < 6))
				throw std::invalid_argument("No width specified");

			if (nColumn > -1 && (UINT)nColumn < iTotal)	// set width for a single specific column
				iCount = 1;			// set a single column to a set width:			/xdid -n dname id column + width
			else {
				iCount = iTotal;	// set all columns to a single width:			/xdid -n dname id -1 + width
				nColumn = 0;		// or set all columns to an auto sized width:	/xdid -n dname id -1 +ahsFp
			}

			for (auto n = decltype(iCount){0}; n < iCount; n++)
			{
				this->autoSize(nColumn, iFlags, iWidth);
				nColumn++;
			}
		}
	}
	// xdid -o [NAME] [ID] [SWITCH] [ORDER ...]
	else if (flags[TEXT('o')] && numtok > 3) {
		const auto ids(input.getlasttoks().trim());	// tok 4, -1
		const UINT count = (UINT)this->getColumnCount();

		// Basic check first
		if (ids.numtok() != count)
			throw std::invalid_argument("Incorrect number of indexes.");

		auto indexes = std::make_unique<int[]>(count);

#if TSTRING_PARTS
		UINT i = 0;
		for (const auto &id: ids)
		{
			const auto tmp = id.to_int();

			if ((tmp == 0) || // invalid character
				(tmp < 0) ||  // negative
				((UINT)tmp > count)) // out of array bounds
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid index %d."), tmp));

			indexes[i++] = tmp -1;
		}
#else
		ids.getfirsttok(0);

		for (UINT i = 0; i < count; i++)
		{
			const int tmp = ids.getnexttok().to_int();	// tok i+1

			if ((tmp == 0) || // invalid character
				(tmp < 0) ||  // negative
				((UINT)tmp > count)) // out of array bounds
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid index %d."), tmp));

			indexes[i] = tmp -1;
		}
#endif

		ListView_SetColumnOrderArray(this->m_Hwnd, count, indexes.get());
	}
	// xdid -q [NAME] [ID] [SWITCH] [N] [+FLAGS] [GID] [Group Text]
	else if (flags[TEXT('q')] && numtok > 6) {
#if TSTRING_TESTCODE
		const auto index = input++.to_int() - 1;						// tok 4
		const auto tsflags(input++);									// tok 5
		const auto gid = input++.to_int();								// tok 6
		const auto iFlags = this->parseGroupFlags(tsflags);
#else
		const auto index = input.getnexttok( ).to_int() -1;				// tok 4
		const auto tsflags(input.getnexttok( ));						// tok 5
		const auto gid = input.getnexttok( ).to_int();					// tok 6
		const auto iFlags = this->parseGroupFlags(tsflags);
#endif

		if (index < 0 || gid <= 0)
			throw std::invalid_argument("Invalid Arguments");

		if (ListView_HasGroup(this->m_Hwnd, (WPARAM)gid))
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Group already exists: %d"), gid));

		const auto text(input.getlasttoks());				// tok 7, -1
		const auto iState = this->parseGroupState(tsflags);

		LVGROUP lvg;
		ZeroMemory(&lvg, sizeof(LVGROUP));
		lvg.cbSize = sizeof(LVGROUP);
		lvg.mask = LVGF_ALIGN | LVGF_HEADER | LVGF_GROUPID | LVGF_STATE;

		lvg.iGroupId = gid;
		lvg.pszHeader = const_cast<TCHAR *>(text.to_chr());
		lvg.uAlign = iFlags;

		lvg.stateMask = iState;
		lvg.state = iState;

		ListView_InsertGroup(this->m_Hwnd, index, &lvg);
	}
	// xdid -r [NAME] [ID] [SWITCH]
	// Note: This is here to prevent an message
	else if (flags[TEXT('r')]) {
		//ListView_DeleteAllItems(this->m_Hwnd);
	}
	// xdid -t [NAME] [ID] [SWITCH] [+FLAGS] [#ICON] [WIDTH] (Header text) [{TAB} [+FLAGS] [#ICON] [WIDTH] Header text {TAB} ... ]
	else if (flags[TEXT('t')] && numtok > 5) {

		this->DeleteColumns(-1);

		LVCOLUMN lvc;
		ZeroMemory(&lvc, sizeof(LVCOLUMN));
		auto nColumn = 0;
		auto data(input.gettok(1, TSTAB).gettok(4, -1).trim());
#if TSTRING_TESTCODE
		auto tsflags(data++);					// tok 1
		auto icon = data++.to_<int>() - 1;		// tok 2
		auto width = data++.to_<int>();			// tok 3
#else
		auto tsflags(data.getfirsttok( 1 ));			// tok 1
		auto icon  = data.getnexttok( ).to_int() -1;	// tok 2
		auto width = data.getnexttok( ).to_int();		// tok 3
#endif

		TString itemtext;
		if (data.numtok( ) > 3)
			itemtext = data.getlasttoks();				// tok 4, -1

		lvc.iImage = I_IMAGENONE;
		lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvc.cx = width;
		lvc.fmt = (int)this->parseHeaderFlags(tsflags);
		lvc.iSubItem = 0;
		lvc.pszText = itemtext.to_chr();

		if (icon > -1) {
			lvc.mask |= LVCF_IMAGE;
			lvc.iImage = icon;
		}

		// column count always zero here, since we just deleted them all
		if (ListView_InsertColumn(this->m_Hwnd, nColumn, &lvc) == -1)
			throw std::runtime_error(Dcx::dcxGetFormattedString(TEXT("Unable to add column: %d"), nColumn));

		this->m_iColumnCount++;

		/*
		*	These flags do NOT make the columns auto size as text is added
		* They auto size the columns to match the text already present (none).
		*/
		this->autoSize(nColumn,tsflags);

		const auto tabs = input.numtok(TSTAB);

		if (tabs > 1 ) {
			for (auto i = decltype(tabs){2}; i <= tabs; i++)
			{
				nColumn++;

				data = input.gettok((int)i, TSTAB).trim();

#if TSTRING_TESTCODE
				tsflags = data++;
				icon = data++.to_int() - 1;	// tok 2
				width = data++.to_int();	// tok 3
#else
				tsflags = data.getfirsttok( 1 );
				icon  = data.getnexttok( ).to_int() -1;	// tok 2
				width = data.getnexttok( ).to_int();	// tok 3
#endif
				itemtext.clear();	// = TEXT("");

				if (data.numtok( ) > 3)
					itemtext = data.getlasttoks();		// tok 4, -1

				lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
				lvc.cx = width;
				lvc.fmt = (int)this->parseHeaderFlags(tsflags);
				lvc.iImage = I_IMAGENONE;
				lvc.iSubItem = 0;
				lvc.pszText = itemtext.to_chr();

				if (icon > -1) {
					lvc.mask |= LVCF_IMAGE;
					lvc.iImage = icon;
				}

				if (ListView_InsertColumn(this->m_Hwnd, nColumn, &lvc) == -1)
					throw std::runtime_error(Dcx::dcxGetFormattedString(TEXT("Unable to add column: %d"), nColumn));

				this->m_iColumnCount++;

				this->autoSize(nColumn, tsflags);
			}
		}
	}
	// xdid -u [NAME] [ID] [SWITCH]
	else if (flags[TEXT('u')]) {
		ListView_SetItemState(this->m_Hwnd, -1, 0, LVIS_SELECTED);
	}
	// xdid -v [NAME] [ID] [SWITCH] [N] [M] (ItemText)
	else if (flags[TEXT('v')] && numtok > 4) {
		auto nItem = input.getnexttok().to_int() - 1;			// tok 4
		const auto nSubItem = input.getnexttok().to_int() - 1;	// tok 5

		// check if item supplied was 0 (now -1), last item in list.
		if (nItem == -1) {
			nItem = ListView_GetItemCount(this->m_Hwnd) -1;

			if (nItem < 0)
				throw std::invalid_argument("Invalid Item: No Items in list");
		}
		if (nItem == -2) {
			// special case -1  (now -2) supplied as item number, sets text for empty listview
			SetWindowText(this->m_Hwnd,input.getlasttoks().trim().to_chr());	// doesnt work, needs looked at.	tok 6, -1
			return;
		}
		if (nItem < 0)
			throw std::invalid_argument("Invalid Item");

		auto itemtext(input.getlasttoks().trim());	// tok 6, -1

		LVITEM lvi;

		ZeroMemory(&lvi, sizeof(LVITEM));

		lvi.mask = LVIF_PARAM;
		lvi.iItem = nItem;
		//lvi.iSubItem = nSubItem;

		ListView_GetItem(this->m_Hwnd, &lvi);
		auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lvi.lParam);

		if (lpdcxlvi != nullptr && lpdcxlvi->pbar != nullptr && lpdcxlvi->iPbarCol == nSubItem) {
			itemtext = input.gettok( 1 ) + TEXT(" ") + input.gettok( 2 ) + TEXT(" ") + itemtext;
			lpdcxlvi->pbar->parseCommandRequest(itemtext);
		}
		else {
			if ((nItem < 0) || (nSubItem < 0) || (nSubItem >= this->getColumnCount()))
				throw std::invalid_argument("Invalid Item");

			ListView_SetItemText(this->m_Hwnd, nItem, nSubItem, itemtext.to_chr());
		}
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')] && numtok > 5) {
		const auto tflags(input.getnexttok());				// tok 4
		const auto index = input.getnexttok().to_int();			// tok 5
		auto filename(input.getlasttoks());					// tok 6, -1
		const auto iFlags = this->parseIconFlagOptions(tflags);
		auto overlayindex = 0;

		// determine overlay index
		if (tflags.find(TEXT('o'),0)) {
			// overlay id offset
			const auto io = tflags.find(TEXT('o'), 1) + 1;
			overlayindex = tflags.mid(io, (int)(tflags.len() - io)).to_int();

			if (overlayindex < 1 || overlayindex > 15)
				throw std::invalid_argument("Overlay index out of range (1 -> 15)");
		}

		// load both normal and small icons
		if (dcx_testflag(iFlags, LVSIL_SMALL)) {
			// load normal icon
			auto himl = this->initImageList(LVSIL_NORMAL);
			if (himl == nullptr)
				throw std::runtime_error("Unable to create normal image list");

			if (index < 0) {
				if (!AddFileIcons(himl, filename, true, -1))
					throw std::runtime_error(Dcx::dcxGetFormattedString(TEXT("Unable to Add %s's Icons"), filename.to_chr()));
			}
			else {
				//HICON icon = dcxLoadIcon(index, filename, true, tflags);
				//
				//if (icon == nullptr)
				//	throw std::runtime_error("Unable to load normal icon");
				//
				//const int i = ImageList_AddIcon(himl, icon);
				//if (overlayindex > 0)
				//	ImageList_SetOverlayImage(himl, i, overlayindex);
				//
				//DestroyIcon(icon);

				Dcx::dcxIcon icon(index, filename, true, tflags);

				const auto i = ImageList_AddIcon(himl, icon);
				if (overlayindex > 0)
					ImageList_SetOverlayImage(himl, i, overlayindex);
			}

			// load small icon
			himl = this->initImageList(LVSIL_SMALL);
			if (himl == nullptr)
				throw std::runtime_error("Unable to create small image list");

			if (index < 0) {
				if (!AddFileIcons(himl, filename, false, -1))
					throw std::runtime_error(Dcx::dcxGetFormattedString(TEXT("Unable to Add %s's Icons"), filename.to_chr()));
			}
			else {
				//HICON icon = dcxLoadIcon(index, filename, false, tflags);
				//
				//if (icon == nullptr)
				//	throw std::runtime_error("Unable to load small icon");
				//
				//const int i = ImageList_AddIcon(himl, icon);
				//
				//if (overlayindex > 0)
				//	ImageList_SetOverlayImage(himl, i, overlayindex);
				//
				//DestroyIcon(icon);

				Dcx::dcxIcon icon(index, filename, false, tflags);

				const auto i = ImageList_AddIcon(himl, icon);

				if (overlayindex > 0)
					ImageList_SetOverlayImage(himl, i, overlayindex);
			}
		}

		// state icon
		if (dcx_testflag(iFlags, LVSIL_STATE)) {
			auto himl = this->initImageList(LVSIL_STATE);
			if (himl == nullptr)
				throw std::runtime_error("Unable to create state image list");

			if (index < 0) {
				if (!AddFileIcons(himl, filename, false, -1))
					throw std::runtime_error(Dcx::dcxGetFormattedString(TEXT("Unable to Add %s's Icons"), filename.to_chr()));
			}
			else {
				//HICON icon = dcxLoadIcon(index, filename, false, tflags);
				//
				//if (icon == nullptr)
				//	throw std::runtime_error("Unable to load state icon");
				//
				//ImageList_AddIcon(himl, icon);
				//
				//DestroyIcon(icon);

				Dcx::dcxIcon icon(index, filename, false, tflags);

				ImageList_AddIcon(himl, icon);
			}
		}
	}
	// xdid -W [NAME] [ID] [SWITCH] [STYLE]
	else if (flags[TEXT('W')] && numtok > 3) {
		static const TString poslist(TEXT("report icon smallicon list tile"));
		static const UINT lv_styles[5] = { LV_VIEW_DETAILS, LV_VIEW_ICON, LV_VIEW_SMALLICON, LV_VIEW_LIST, LV_VIEW_TILE };
		const auto style(input.getnexttok());	// tok 4
#if TSTRING_TEMPLATES
		const auto index = poslist.findtok(style, 1);
#else
		const auto index = poslist.findtok(style.to_chr(), 1);
#endif
		const UINT mode = lv_styles[index -1];

		ListView_SetView(this->m_Hwnd, mode);
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if (flags[TEXT('y')] && numtok > 3) {
		const auto iFlags = this->parseIconFlagOptions(input.getnexttok());	// tok 4
		HIMAGELIST himl;

		if (dcx_testflag(iFlags, LVSIL_SMALL)) {
			himl = this->getImageList(LVSIL_SMALL);
			if (himl != nullptr) {
				this->setImageList(nullptr, LVSIL_SMALL);
				ImageList_Destroy(himl);
			}

			himl = this->getImageList(LVSIL_NORMAL);
			if (himl != nullptr) {
				this->setImageList(nullptr, LVSIL_NORMAL);
				ImageList_Destroy(himl);
			}
		}

		if (dcx_testflag(iFlags, LVSIL_STATE)) {
			himl = this->getImageList(LVSIL_STATE);
			if (himl != nullptr) {
				this->setImageList(nullptr, LVSIL_STATE);
				ImageList_Destroy(himl);
			}
		}
	}
	// xdid -z [NAME] [ID] [SWITCH] [+FLAGS] [N] (ALIAS)
	else if (flags[TEXT('z')] && numtok > 4) {
		auto lvsort = std::make_unique<DCXLVSORT>();	// too big for stack, use heap.

		lvsort->iSortFlags = this->parseSortFlags(input.getnexttok( ));	// tok 4
		const auto nColumn = input.getnexttok().to_int() - 1;	// tok 5

		lvsort->m_Hwnd = this->m_Hwnd;
		lvsort->nColumn = nColumn;

		if (nColumn < 0 || nColumn >= this->getColumnCount())
			throw std::invalid_argument("Invalid Arguments");

		if (dcx_testflag(lvsort->iSortFlags, LVSS_CUSTOM) && numtok < 6)
			throw std::invalid_argument("Invalid Arguments for Flags");

		lvsort->tsCustomAlias = input.getnexttok( );		// tok 6

		ListView_SortItemsEx(this->m_Hwnd, DcxListView::sortItemsEx, lvsort.get());
	}
	// xdid -T [NAME] [ID] [SWITCH] [nItem] [nSubItem] (ToolTipText)
	// atm this only seems works for subitem 0. Mainly due to the callback LVN_GETINFOTIP only being sent for sub 0.
	else if (flags[TEXT('T')] && numtok > 4) {
		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));

		lvi.iItem = input.getnexttok( ).to_int() -1;	// tok 4
		lvi.iSubItem = input.getnexttok( ).to_int();	// tok 5
		lvi.mask = LVIF_PARAM;

		// check if item supplied was 0 (now -1), last item in list.
		if (lvi.iItem == -1) {
			lvi.iItem = ListView_GetItemCount(this->m_Hwnd) -1;

			if (lvi.iItem < 0)
				throw std::invalid_argument("Invalid Item: No Items in list");
		}

		if ((lvi.iItem < 0) || (lvi.iSubItem < 0))
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid Item: %d Subitem: %d"), lvi.iItem +1, lvi.iSubItem));

		if (!ListView_GetItem(this->m_Hwnd, &lvi))
			throw std::runtime_error(Dcx::dcxGetFormattedString(TEXT("Unable To Get Item: %d Subitem: %d"), lvi.iItem + 1, lvi.iSubItem));

		auto lpmylvi = reinterpret_cast<LPDCXLVITEM>(lvi.lParam);

		if (lpmylvi == nullptr)
			throw std::runtime_error("Unable to get DCX Item Information, something very wrong!");

		lpmylvi->tsTipText = (numtok > 5 ? input.getlasttoks() : TEXT(""));	// tok 6, -1
		LVSETINFOTIP it;
		ZeroMemory(&it, sizeof(it));
		it.cbSize = sizeof(it);
		it.iItem = lvi.iItem;
		it.iSubItem = lvi.iSubItem;
		it.pszText = lpmylvi->tsTipText.to_chr();

		ListView_SetInfoTip(this->m_Hwnd,&it);
	}
	// xdid -Z [NAME] [ID] [SWITCH] [%]
	else if (flags[TEXT('Z')] && numtok > 3) {
		// only works for this one so far
		//if (!this->isStyle(LVS_REPORT))
		//	return;

		auto pos = input.getnexttok().to_int();	// tok 4
		auto count = ListView_GetItemCount(this->m_Hwnd);
		RECT rc;

		// no items - no need to view
		// check boundaries
		if ((count == 0) || (pos < 0) || (pos > 100))
			return;	// no error, just do nothing.

		// subtract the number of visible items
		count -= ListView_GetCountPerPage(this->m_Hwnd);

		// get height of a single item
		ListView_GetItemRect(this->m_Hwnd, 0, &rc, LVIR_BOUNDS);
		const auto height = count * (rc.bottom - rc.top);

		pos = dcx_round((float) height * (float) pos / (float) 100.0);

		ListView_EnsureVisible(this->m_Hwnd, 0, FALSE);
		ListView_Scroll(this->m_Hwnd, 0, pos);
		//int nItem = round((float)count / (float)100.0 * (float)pos);
		//ListView_EnsureVisible(this->m_Hwnd, 0, FALSE);
		//ListView_EnsureVisible(this->m_Hwnd, --nItem, FALSE);
	}
	// xdid -V [NAME] [ID] [SWITCH] [nItem]
	else if (flags[TEXT('V')] && numtok > 3) {
		const auto nItem = input.getnexttok().to_int() - 1;	// tok 4

		if (nItem > -1)
			ListView_EnsureVisible(this->m_Hwnd, nItem, FALSE);
	}
	// xdid -S [NAME] [ID] [+FLAGS] [N1] [N2] [ARGS]
	else if (flags[TEXT('S')] && numtok > 5) {
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
		const auto count = ListView_GetItemCount(this->m_Hwnd);
		const auto tsFlags(input.getnexttok().trim());		// tok 4
		const auto iN1 = input.getnexttok().to_int();			// tok 5
		auto iN2 = input.getnexttok().to_int();					// tok 6
		const auto tsArgs(input.getlasttoks().trim());		// tok 7, -1

		if ((tsFlags[0] != TEXT('+')) || (tsFlags.len() < 2))
			// no flags specified.
			throw std::invalid_argument("Invalid Flags: No Flags Specified.");

		// make sure N1-N2 are within the range of items in listview.
		// adjust iN2 if its < 0, so its an offset from the last item.
		if (iN2 == 0) iN2 = count;
		else if (iN2 < 0) iN2 = count + iN2;
		if ((iN1 < 1) || (iN1 > count) || (iN2 < 0) || (iN2 < iN1))
			throw std::invalid_argument("Invalid Range: N1-N2 Must be in range of items in listview");

		switch (tsFlags[1])
		{
		case TEXT('c'):
			{
				TString res;
				// check window exists
				mIRCLinker::tsEvalex(res, TEXT("$window(%s)"), tsArgs.to_chr());
				// if not exit
				if (tsArgs != res)
					throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid window: %s"), tsArgs.to_chr()));
				//
			}
			break;
		case TEXT('f'):
			break;
		case TEXT('h'):
			break;
		case TEXT('x'):
			break;
		default:
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid Flags: %s"), tsFlags.to_chr()));
		}
	}
	// xdid -H [NAME] [ID] [COL] [+FLAGS] [ARGS]
	// xdid -H [NAME] [ID] -H [COL|COL1-COL2|COL1,COL2|COL1,COL2-COL3 etc..] [+FLAGS] [ARGS]
	else if (flags[TEXT('H')]) {
		if (numtok < 4)
			throw std::invalid_argument("Insufficient parameters");

		const auto tsCols(input.getnexttok());			// tok 4
		const XSwitchFlags xflag(input.getnexttok( ));		// tok 5
		const auto info(input.getlasttoks());			// tok 6, -1

		if (!xflag[TEXT('+')])
			throw std::invalid_argument("Missing '+' in flags");

		auto h = ListView_GetHeader(this->m_Hwnd);
		if (!IsWindow(h))
			throw std::runtime_error("Unable to get Header Window");

		const auto col_count = this->getColumnCount();

#if TSTRING_PARTS
#if TSTRING_ITERATOR
		for (auto itStart = tsCols.begin(TSCOMMA), itEnd = tsCols.end(); itStart != itEnd; ++itStart)
		{
			auto col_start = 0, col_end = 0;
			const auto col(*itStart);
			this->getItemRange(col, col_count, &col_start, &col_end);

			if ( (col_start < 0) || (col_end < 0) || (col_start >= col_count) || (col_end >= col_count) )
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid column index %s."), col.to_chr()));

			for (auto nCol = col_start; nCol <= col_end; nCol++) {
				if (!xflag['s'])	// change header style
					throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unknown flags %s"), input.gettok(5).to_chr()));

				this->setHeaderStyle(h, nCol, info);
			}
		}
#else
		tsCols.split(TSCOMMA);
		for (const auto &col: tsCols) {
			auto col_start = 0, col_end = 0;
			this->getItemRange(col, col_count, &col_start, &col_end);

			if ( (col_start < 0) || (col_end < 0) || (col_start >= col_count) || (col_end >= col_count) )
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid column index %s."), col.to_chr()));

			for (auto nCol = col_start; nCol <= col_end; nCol++) {
				if (!xflag['s'])	// change header style
					throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unknown flags %s"), input.gettok(5).to_chr()));

				this->setHeaderStyle(h, nCol, info);
			}
		}
#endif
#else
		for (auto col(tsCols.getfirsttok(1, TSCOMMA)); !col.empty(); col = tsCols.getnexttok(TSCOMMA)) {
			int col_start = 0, col_end = 0;
			this->getItemRange(col, col_count, &col_start, &col_end);

			if ( (col_start < 0) || (col_end < 0) || (col_start >= col_count) || (col_end >= col_count) )
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid column index %s."), col.to_chr()));

			for (int nCol = col_start; nCol <= col_end; nCol++) {
				if (!xflag['s'])	// change header style
					throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unknown flags %s"), input.gettok(5).to_chr()));

				this->setHeaderStyle(h, nCol, info);
			}
		}
#endif

	}
	// xdid -G [NAME] [ID] [SWITCH] [GID] [+MASK] [+STATES]
	else if (flags[TEXT('G')] && numtok == 6) {
		const auto gid = input.getnexttok().to_int();

		if (!ListView_HasGroup(this->m_Hwnd, (WPARAM)gid))
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Group doesn't exist: %d"), gid));

		if (!Dcx::VistaModule.isVista())
			throw std::runtime_error("This Command is Vista+ Only!");

		const auto iMask = this->parseGroupState(input.getnexttok());	// tok 5
		const auto iState = this->parseGroupState(input.getnexttok());	// tok 6

		ListView_SetGroupState(this->m_Hwnd, gid, iMask, iState);
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

void DcxListView::setHeaderStyle(HWND h, const int nCol, const TString &info)
{
	HDITEM hdr = {0};
	hdr.mask = HDI_FORMAT;

	if (!Header_GetItem(h, nCol, &hdr))
		throw std::runtime_error(Dcx::dcxGetFormattedString(TEXT("Unable to get item: %d"), nCol + 1));
	
	static const TString header_styles(TEXT("sortdown sortup checkbox checked nocheckbox unchecked nosort"));

#if TSTRING_PARTS
	for (const auto &tmp: info)
	{
#if TSTRING_TEMPLATES
		switch (header_styles.findtok(tmp, 1))
#else
		switch (header_styles.findtok(tmp.to_chr(), 1))
#endif
		{
		case 1:	// sortdown
		{
			hdr.fmt &= ~HDF_SORTUP;
			hdr.fmt |= HDF_SORTDOWN;
		}
		break;
		case 2:	// sortup
		{
			hdr.fmt &= ~HDF_SORTDOWN;
			hdr.fmt |= HDF_SORTUP;
		}
		break;
		case 3:	// checkbox
		{
			hdr.fmt |= HDF_CHECKBOX;
		}
		break;
		case 4:	// checked
		{
			hdr.fmt |= HDF_CHECKED;
		}
		break;
		case 5:	// nocheckbox
		{
			hdr.fmt &= ~(HDF_CHECKBOX | HDF_CHECKED);
		}
		break;
		case 6:	// unchecked
		{
			hdr.fmt &= ~HDF_CHECKED;
		}
		break;
		case 7:	// nosort
		{
			hdr.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
		}
		break;
		case 0:		//error
		default:	//error
			// silently fail on invalid styles
			break;
		}
	}
#else
	for (auto tmp(info.getfirsttok(1)); !tmp.empty(); tmp = info.getnexttok())
	{
#if TSTRING_TEMPLATES
		switch (header_styles.findtok(tmp, 1))
#else
		switch (header_styles.findtok(tmp.to_chr(), 1))
#endif
		{
		case 1:	// sortdown
			{
				hdr.fmt &= ~HDF_SORTUP;
				hdr.fmt |= HDF_SORTDOWN;
			}
			break;
		case 2:	// sortup
			{
				hdr.fmt &= ~HDF_SORTDOWN;
				hdr.fmt |= HDF_SORTUP;
			}
			break;
		case 3:	// checkbox
			{
				hdr.fmt |= HDF_CHECKBOX;
			}
			break;
		case 4:	// checked
			{
				hdr.fmt |= HDF_CHECKED;
			}
			break;
		case 5:	// nocheckbox
			{
				hdr.fmt &= ~(HDF_CHECKBOX|HDF_CHECKED);
			}
			break;
		case 6:	// unchecked
			{
				hdr.fmt &= ~HDF_CHECKED;
			}
			break;
		case 7:	// nosort
			{
				hdr.fmt &= ~(HDF_SORTUP|HDF_SORTDOWN);
			}
			break;
		case 0:		//error
		default:	//error
			break;
		}
	}
#endif
	Header_SetItem(h, nCol, &hdr);
}

/*
Initializes an image list.
*/
HIMAGELIST DcxListView::initImageList(const int iImageList) {
	auto himl = this->getImageList(iImageList);

	if (himl != nullptr)
		return himl;

	himl = this->createImageList(iImageList == LVSIL_NORMAL ? TRUE : FALSE);

	if (himl != nullptr)
		this->setImageList(himl, iImageList);

	return himl;
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxListView::getImageList( const int iImageList ) const
{
	return ListView_GetImageList( this->m_Hwnd, iImageList );
}

/*!
* \brief blah
*
* blah
*/

void DcxListView::setImageList(const HIMAGELIST himl, const int iImageList) {
	auto o = ListView_SetImageList(this->m_Hwnd, himl, iImageList);
	if (o != nullptr && o != himl)
		ImageList_Destroy(o);
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxListView::createImageList( const BOOL bIcons ) {

	HIMAGELIST himl;
	// Big Image List
	if ( bIcons )
		himl = ImageList_Create( 32, 32, ILC_COLOR32|ILC_MASK, 1, 0 );
	else
		himl = ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );

	ImageList_SetBkColor( himl, RGB(255,255,255) );
	return himl;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseIconFlagOptions( const TString & flags ) {

	UINT iFlags = 0;

	const XSwitchFlags xflags(flags);

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if ( xflags[TEXT('n')] )
		iFlags |= LVSIL_SMALL;
	if ( xflags[TEXT('s')] )
		iFlags |= LVSIL_STATE;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseItemFlags(const TString & flags) {
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

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

UINT DcxListView::parseMassItemFlags(const TString & flags) {
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

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
UINT DcxListView::parseHeaderFlags( const TString & flags ) {
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	if (!xflags[TEXT('+')])
		return 0;

	// 'a' use by flags2
	if ( xflags[TEXT('b')] )
		iFlags |= LVCFMT_BITMAP_ON_RIGHT;
	if ( xflags[TEXT('c')] )
		iFlags |= LVCFMT_CENTER;
	// 'd' use by flags2
	if ( xflags[TEXT('f')] )
		iFlags |= LVCFMT_FIXED_WIDTH;
	// 'F' use by flags2
	// 'h' use by flags2
	if ( xflags[TEXT('l')] )
		iFlags |= LVCFMT_LEFT;
	if ( xflags[TEXT('r')] )
		iFlags |= LVCFMT_RIGHT;
	if ( xflags[TEXT('q')] )
		iFlags |= LVCFMT_FIXED_RATIO;
	// 's' use by flags2
	if ( xflags[TEXT('d')] )
		iFlags |= LVCFMT_SPLITBUTTON;
	if ( xflags[TEXT('e')] )
		iFlags |= HDF_SORTDOWN;
	if ( xflags[TEXT('g')] )
		iFlags |= HDF_SORTUP;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

INT DcxListView::parseHeaderFlags2( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	return parseHeaderFlags2(xflags);
}
INT DcxListView::parseHeaderFlags2( const XSwitchFlags & xflags ) {

	int iFlags = 0;

	if (!xflags[TEXT('+')])
		return 0;

	if (xflags[TEXT('a')]) // auto size
		iFlags |= DCX_LV_COLUMNF_AUTO;
	if (xflags[TEXT('F')]) // fixed width.
		iFlags |= DCX_LV_COLUMNF_FIXED;
	if (xflags[TEXT('h')]) // header size
		iFlags |= DCX_LV_COLUMNF_AUTOHEADER;
	if (xflags[TEXT('p')]) // percentage
		iFlags |= DCX_LV_COLUMNF_PERCENT;
	if (xflags[TEXT('s')]) // max size (max of auto & header)
		iFlags |= DCX_LV_COLUMNF_AUTOMAX;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseSortFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if ( xflags[TEXT('a')] )
		iFlags |= LVSS_ASC;
	if ( xflags[TEXT('c')] )
		iFlags |= LVSS_CUSTOM;
	if ( xflags[TEXT('d')] )
		iFlags |= LVSS_DESC;
	if ( xflags[TEXT('n')] )
		iFlags |= LVSS_NUMERIC;
	if ( xflags[TEXT('s')] )
		iFlags |= LVSS_CASE;
	if ( xflags[TEXT('t')] )
		iFlags |= LVSS_ALPHA;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseGroupFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if ( xflags[TEXT('c')] )
		iFlags |= LVGA_HEADER_CENTER;
	if ( xflags[TEXT('l')] )
		iFlags |= LVGA_HEADER_LEFT;
	if ( xflags[TEXT('r')] )
		iFlags |= LVGA_HEADER_RIGHT;

	return iFlags;
}
UINT DcxListView::parseGroupState( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if ( xflags[TEXT('C')] )
		iFlags |= LVGS_COLLAPSIBLE;
	if ( xflags[TEXT('H')] )
		iFlags |= LVGS_HIDDEN;
	if ( xflags[TEXT('N')] )
		iFlags |= LVGS_NOHEADER;
	if ( xflags[TEXT('O')] )
		iFlags |= LVGS_COLLAPSED;
	if ( xflags[TEXT('S')] )
		iFlags |= LVGS_SELECTED;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseColorFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if ( xflags[TEXT('b')] )
		iFlags |= LVCS_BKG;
	if ( xflags[TEXT('k')] )
		iFlags |= LVCS_BKGTEXT;
	if ( xflags[TEXT('o')] )
		iFlags |= LVCS_OUTLINE;
	if ( xflags[TEXT('t')] )
		iFlags |= LVCS_TEXT;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseImageFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if ( xflags[TEXT('n')] )
		iFlags |= LVBKIF_STYLE_NORMAL;
	if ( xflags[TEXT('r')] )
		iFlags |= LVBKIF_SOURCE_NONE;
	if ( xflags[TEXT('t')] )
		iFlags |= LVBKIF_STYLE_TILE;
	if ( xflags[TEXT('w')] )
		iFlags |= LVBKIF_TYPE_WATERMARK;
	if ( xflags[TEXT('a')] )
		iFlags |= LVBKIF_TYPE_WATERMARK|LVBKIF_FLAG_ALPHABLEND;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

bool DcxListView::isListViewStyle( const DWORD dwView ) const {

	return ((GetWindowStyle(this->m_Hwnd) & LVS_TYPEMASK) == dwView);
}

/*!
* \brief blah
*
* blah
*/

bool DcxListView::matchItemText(const int nItem, const int nSubItem, const TString * search, const ListView_SearchTypes SearchType) const
{
	TCHAR itemtext[MIRC_BUFFER_SIZE_CCH];
	itemtext[0] = TEXT('\0');

	ListView_GetItemText( this->m_Hwnd, nItem, nSubItem, itemtext, MIRC_BUFFER_SIZE_CCH );

	switch (SearchType) {
		case LVSEARCH_R:
			return isRegexMatch(itemtext, search->to_chr());
		case LVSEARCH_W:
			return TString(itemtext).iswm(search->to_chr());
		case LVSEARCH_E:
			return (lstrcmp(search->to_chr(), itemtext) == 0); // must be a zero check not a !
	}

	return false;
}

/*!
* \brief blah
*
* blah
*/

const int &DcxListView::getColumnCount( ) const
{
	if (m_iColumnCount >= 0)
		return m_iColumnCount;
	else {
		LVCOLUMN lvc;
		ZeroMemory(&lvc, sizeof(LVCOLUMN));
		lvc.mask = LVCF_WIDTH;

		auto i = 0;
		while (ListView_GetColumn(this->m_Hwnd, i, &lvc) != FALSE)
			i++;

		m_iColumnCount = i;

		return m_iColumnCount;
	}
}

/*!
* \brief blah
*
* blah
*/

int DcxListView::getTopIndex( ) const {

	if ( ListView_GetItemCount( this->m_Hwnd) > 0 )
		return ListView_GetTopIndex( this->m_Hwnd );

	return -1;
}

/*!
* \brief blah
*
* blah
*/

int DcxListView::getBottomIndex( ) const {

	auto nBottomIndex = (ListView_GetTopIndex(this->m_Hwnd) + ListView_GetCountPerPage(this->m_Hwnd));
	const auto nCount = ListView_GetItemCount(this->m_Hwnd);
	if (nBottomIndex > nCount)
		nBottomIndex = nCount;

	return --nBottomIndex;
}

/*!
* \brief blah
*
* blah
*/

int CALLBACK DcxListView::sortItemsEx( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort ) {

	auto plvsort = reinterpret_cast<LPDCXLVSORT>(lParamSort);
	plvsort->itemtext1[0] = TEXT('\0');
	plvsort->itemtext2[0] = TEXT('\0');

	ListView_GetItemText( plvsort->m_Hwnd, lParam1, plvsort->nColumn, plvsort->itemtext1, MIRC_BUFFER_SIZE_CCH );
	ListView_GetItemText( plvsort->m_Hwnd, lParam2, plvsort->nColumn, plvsort->itemtext2, MIRC_BUFFER_SIZE_CCH );

	// CUSTOM Sort
	if (dcx_testflag(plvsort->iSortFlags, LVSS_CUSTOM ))
	{
		if (plvsort->tsCustomAlias.empty())
			return 0;

		TCHAR res[20];

		// testing new sort call... new ver doesnt pass item text directly via alias, but instead sets a %var with the text & passes the %var name to the alias.
		// Should solve some item name issues.
		mIRCLinker::execex(TEXT("/!set -nu1 %%dcx_1sort%d %s"), plvsort->itemtext1, plvsort->itemtext1 );
		mIRCLinker::execex(TEXT("/!set -nu1 %%dcx_2sort%d %s"), plvsort->itemtext2, plvsort->itemtext2 );
		mIRCLinker::evalex( res, 20, TEXT("$%s(%%dcx_1sort%d,%%dcx_2sort%d)"), plvsort->tsCustomAlias.to_chr( ), plvsort->itemtext1, plvsort->itemtext2 );
		//
		//mIRCLinker::evalex( res, 20, TEXT("$%s(%s,%s)"), plvsort->tsCustomAlias.to_chr( ), itemtext1, itemtext2 );

		auto ires = dcx_atoi(res);

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
	else if (dcx_testflag(plvsort->iSortFlags, LVSS_NUMERIC)) {
		const auto i1 = dcx_atoi(plvsort->itemtext1);
		const auto i2 = dcx_atoi(plvsort->itemtext2);

		if (dcx_testflag(plvsort->iSortFlags, LVSS_DESC)) {

			if ( i1 < i2 )
				return 1;
			else if ( i1 > i2 )
				return -1;
		}
		else {

			if ( i1 < i2 )
				return -1;
			else if ( i1 > i2 )
				return 1;
		}
	}
	// Default ALPHA Sort
	else {

		if (dcx_testflag(plvsort->iSortFlags, LVSS_DESC)) {
			if (dcx_testflag(plvsort->iSortFlags, LVSS_CASE))
				return -lstrcmp( plvsort->itemtext1, plvsort->itemtext2 );
			else
				return -lstrcmpi( plvsort->itemtext1, plvsort->itemtext2 );
		}
		else {
			if (dcx_testflag(plvsort->iSortFlags, LVSS_CASE))
				return lstrcmp( plvsort->itemtext1, plvsort->itemtext2 );
			else
				return lstrcmpi( plvsort->itemtext1, plvsort->itemtext2 );
		}
	}

	return 0;
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxListView::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {
		case WM_NOTIFY:
			{
				dcxlParam(LPNMHDR, hdr);

				if (hdr == nullptr)
					break;

				switch( hdr->code ) {

					case NM_CLICK:
						{
							bParsed = TRUE;

							LVHITTESTINFO lvh = { 0 };
							
							dcxlParam(LPNMITEMACTIVATE, nmia);
							lvh.pt = nmia->ptAction;

							ListView_SubItemHitTest(this->m_Hwnd, &lvh);

							const auto lvexstyles = ListView_GetExtendedListViewStyle(this->m_Hwnd);

							if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {

								if (dcx_testflag(lvh.flags, LVHT_ONITEMSTATEICON) && dcx_testflag(lvexstyles, LVS_EX_CHECKBOXES) && !dcx_testflag(lvh.flags, LVHT_ONITEMICON) && !dcx_testflag(lvh.flags, LVHT_ONITEMLABEL))
								{
									//TODO: int state = ListView_GetCheckState(this->m_Hwnd, lvh.iItem);
									this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("stateclick"), this->getUserID(), lvh.iItem + 1, lvh.iSubItem + 1);
								}
								else if (dcx_testflag(lvh.flags, LVHT_ONITEM))
									this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("sclick"), this->getUserID(), lvh.iItem + 1, lvh.iSubItem + 1);
								else if (dcx_testflag(lvh.flags, LVHT_NOWHERE))
									this->execAliasEx(TEXT("%s,%d"), TEXT("sclick"), this->getUserID());
							}
//#if !defined(NDEBUG) || defined(DCX_DEV_BUILD)
							if (!dcx_testflag(lvexstyles, LVS_EX_FULLROWSELECT))
							{ // make subitem show as selected. TEST CODE!!!!
								LVITEM lvi = { 0 };
								// deselect previous
								lvi.iItem = this->m_iSelectedItem;
								lvi.iSubItem = this->m_iSelectedSubItem;
								lvi.mask = LVIF_STATE;
								lvi.state = 0;
								lvi.stateMask = LVIS_SELECTED;
								ListView_SetItem(this->m_Hwnd, &lvi);
								// select new
								this->m_iSelectedItem = lvh.iItem;
								this->m_iSelectedSubItem = lvh.iSubItem;
								lvi.iItem = lvh.iItem;
								lvi.iSubItem = lvh.iSubItem;
								lvi.mask = LVIF_STATE;
								lvi.state = LVIS_SELECTED;
								lvi.stateMask = LVIS_SELECTED;
								ListView_SetItem(this->m_Hwnd, &lvi);
							}
//#endif
						}
						break;

					case NM_DBLCLK:
						{
							bParsed = TRUE;

							if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {
								LVHITTESTINFO lvh;

								dcxlParam(LPNMITEMACTIVATE, nmia);
								lvh.pt = nmia->ptAction;

								ListView_SubItemHitTest( this->m_Hwnd, &lvh );

								if (dcx_testflag(lvh.flags, LVHT_ONITEM))
									this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("dclick"), this->getUserID( ), lvh.iItem +1, lvh.iSubItem +1);
								else
									this->execAliasEx(TEXT("%s,%d"), TEXT("dclick"), this->getUserID());
							}
						}
						break;

					case NM_RCLICK:
						{
							if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {
								LVHITTESTINFO lvh;

								dcxlParam(LPNMITEMACTIVATE, nmia);
								lvh.pt = nmia->ptAction;

								ListView_SubItemHitTest( this->m_Hwnd, &lvh );

								if (dcx_testflag(lvh.flags, LVHT_ONITEM))
									this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("rclick"), this->getUserID( ), lvh.iItem +1, lvh.iSubItem +1);
								else
									this->execAliasEx(TEXT("%s,%d"), TEXT("rclick"), this->getUserID());
							}
							bParsed = TRUE;
						}
						break;

					case NM_RDBLCLK:
						{
							if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {
								LVHITTESTINFO lvh;

								dcxlParam(LPNMITEMACTIVATE, nmia);
								lvh.pt = nmia->ptAction;

								ListView_SubItemHitTest( this->m_Hwnd, &lvh );

								if (dcx_testflag(lvh.flags, LVHT_ONITEM))
									this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("rdclick"), this->getUserID( ), lvh.iItem +1, lvh.iSubItem +1);
								else
									this->execAliasEx(TEXT("%s,%d"), TEXT("rdclick"), this->getUserID());
							}
							bParsed = TRUE;
						}
						break;

					case NM_HOVER:
						{
							if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {
								LVHITTESTINFO lvh;
								if (GetCursorPos(&lvh.pt))
								{
									MapWindowPoints(nullptr, this->m_Hwnd, &lvh.pt, 1);
									ListView_SubItemHitTest(this->m_Hwnd, &lvh);

									if (dcx_testflag(lvh.flags, LVHT_ONITEM))
										this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("hover"), this->getUserID(), lvh.iItem + 1, lvh.iSubItem + 1);
									else
										this->execAliasEx(TEXT("%s,%d"), TEXT("hover"), this->getUserID());
								}
							}
							bParsed = TRUE;
						}
						break;

					case LVN_BEGINLABELEDIT:
						{
							bParsed = TRUE;
							dcxlParam(const LPNMLVDISPINFO, lplvdi);

							ListView_SetItemState( this->m_Hwnd, lplvdi->item.iItem, LVIS_SELECTED, LVIS_SELECTED );

							auto edit_hwnd = ListView_GetEditControl(this->m_Hwnd);

							this->m_OrigEditProc = SubclassWindow( edit_hwnd, DcxListView::EditLabelProc );
							SetProp( edit_hwnd, TEXT("dcx_pthis"), (HANDLE) this );

							TCHAR ret[256];
							this->evalAliasEx(ret, 255, TEXT("%s,%d,%d,%d"), TEXT("labelbegin"), this->getUserID(), lplvdi->item.iItem +1, lplvdi->item.iSubItem +1);

							if ( lstrcmp( TEXT("noedit"), ret ) == 0)
								return TRUE;
						}
						break;

					case LVN_ENDLABELEDIT:
						{
							bParsed = TRUE;

							dcxlParam(const LPNMLVDISPINFO, lplvdi);

							if ( lplvdi->item.pszText == nullptr )
								this->execAliasEx(TEXT("%s,%d"), TEXT("labelcancel"), this->getUserID( ) );
							else {
								TCHAR ret[256];
								this->evalAliasEx( ret, 255, TEXT("%s,%d,%s"), TEXT("labelend"), this->getUserID( ), lplvdi->item.pszText );

								if ( lstrcmp( TEXT("noedit"), ret ) == 0)
									return FALSE;
							}
							return TRUE;
						}
						break;

					case NM_CUSTOMDRAW:
						{
							const auto lplvcd = reinterpret_cast<const LPNMLVCUSTOMDRAW>(lParam);
							bParsed = TRUE;

							if (lplvcd == nullptr)
								return CDRF_DODEFAULT;

							//if (!ListView_IsItemVisible(this->m_Hwnd, lplvcd->nmcd.dwItemSpec))
							//	return CDRF_DODEFAULT;

							switch( lplvcd->nmcd.dwDrawStage ) {
								case CDDS_PREPAINT:
									return ( CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT );

								case CDDS_ITEMPREPAINT:
									return CDRF_NOTIFYSUBITEMDRAW;

								case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
									{
										auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lplvcd->nmcd.lItemlParam);

										if ( lpdcxlvi == nullptr )
											return CDRF_DODEFAULT;

										if ((lpdcxlvi->pbar != nullptr) && (lplvcd->iSubItem == lpdcxlvi->iPbarCol))
											return CDRF_SKIPDEFAULT;

										if (((UINT)lplvcd->iSubItem >= lpdcxlvi->vInfo.size()) || (lplvcd->iSubItem < 0))
											return CDRF_DODEFAULT;

										auto ri = lpdcxlvi->vInfo[(UINT)lplvcd->iSubItem];
										if (ri->m_cText != CLR_INVALID)
											lplvcd->clrText = ri->m_cText;

										if (ri->m_cBg != CLR_INVALID)
											lplvcd->clrTextBk = ri->m_cBg;
										//if (lplvcd->nmcd.uItemState & CDIS_SELECTED)
										//if (lplvcd->nmcd.uItemState & CDIS_FOCUS)
										//	lplvcd->clrTextBk = RGB(255,0,0);
										//else if ( ri->m_cBg != -1 )
										//	lplvcd->clrTextBk = ri->m_cBg;
										//lplvcd->clrFace = RGB(0,255,0);

										if (dcx_testflag(ri->m_dFlags, LVIS_UNDERLINE) || dcx_testflag(ri->m_dFlags, LVIS_BOLD) || dcx_testflag(ri->m_dFlags,LVIS_ITALIC))
										{
											const auto hFont = GetWindowFont(this->m_Hwnd);
											LOGFONT lf;

											if (GetObject(hFont, sizeof(LOGFONT), &lf) != 0)
											{
												if (dcx_testflag(ri->m_dFlags, LVIS_BOLD))
													lf.lfWeight |= FW_BOLD;
												if (dcx_testflag(ri->m_dFlags, LVIS_UNDERLINE))
													lf.lfUnderline = 1;
												if (dcx_testflag(ri->m_dFlags, LVIS_ITALIC))
													lf.lfItalic = 1;

												this->m_hItemFont = CreateFontIndirect(&lf);
												if (this->m_hItemFont != nullptr)
													this->m_hOldItemFont = SelectFont(lplvcd->nmcd.hdc, this->m_hItemFont);
											}
										}
//#if DCX_DEBUG_OUTPUT
										if (dcx_testflag(ri->m_dFlags,LVIS_CENTERICON))
										{
											// test code for centering an image in an item when it has no text
											LVITEM lvi = { 0 };
											TCHAR buf[MIRC_BUFFER_SIZE_CCH];
											buf[0] = TEXT('\0');

											lvi.pszText = buf;
											lvi.cchTextMax = MIRC_BUFFER_SIZE_CCH;
											lvi.iItem = (int)lplvcd->nmcd.dwItemSpec;
											lvi.iSubItem = lplvcd->iSubItem;
											lvi.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_STATE;
											lvi.stateMask = LVIS_SELECTED|LVIS_FOCUSED;

											if (ListView_GetItem(this->m_Hwnd, &lvi)) {
												//if ((lvi.iImage > I_IMAGECALLBACK) && (lstrlen(buf) <= 0))
												if ((lvi.iImage > I_IMAGECALLBACK) && (buf[0] == TEXT('\0')))
												{
													// no text, but has image, so center image in item
													auto himl = getImageList(LVSIL_SMALL);
													if (himl != nullptr)
													{
														RECT rcBounds;
														if (ListView_GetSubItemRect(this->m_Hwnd, lplvcd->nmcd.dwItemSpec, lplvcd->iSubItem, LVIR_BOUNDS, &rcBounds))
														{
															UINT iDrawFlags = ILD_NORMAL | ILD_TRANSPARENT;

															if (dcx_testflag(lvi.state, LVIS_SELECTED))
															//if (ListView_GetItemState(this->m_Hwnd, lplvcd->nmcd.dwItemSpec, LVIS_SELECTED) == LVIS_SELECTED)
																//if (dcx_testflag(lplvcd->nmcd.uItemState, CDIS_SELECTED))
															{
																const auto exStyles = ListView_GetExtendedListViewStyle(this->m_Hwnd);

																if (!dcx_testflag(exStyles, LVS_EX_BORDERSELECT))
																{
																	// fill background with selected colour. Only if not border select mode
																	iDrawFlags |= ILD_BLEND50;	// blend icon image with select colour
																	if (this->m_pParentDialog->isDialogActive())
																	{
																		if (this->m_pParentDialog->getFocusControl() == this->getUserID())
																			FillRect(lplvcd->nmcd.hdc, &rcBounds, GetSysColorBrush(COLOR_HIGHLIGHT));
																		else if (this->isStyle(LVS_SHOWSELALWAYS)) // item greyed...
																			FillRect(lplvcd->nmcd.hdc, &rcBounds, GetSysColorBrush(COLOR_MENU));
																	}
																	else if (this->isStyle(LVS_SHOWSELALWAYS))
																		FillRect(lplvcd->nmcd.hdc, &rcBounds, GetSysColorBrush(COLOR_MENU));
																}
																if (!dcx_testflag(exStyles,LVS_EX_FULLROWSELECT))
																{
																	// draw focus rect around selected item. Only if not fullrow select
																	if (dcx_testflag(lvi.state,LVIS_FOCUSED))
																		DrawFocusRect(lplvcd->nmcd.hdc, &rcBounds);
																}
															}
															else {
																if (ri->m_cBg != CLR_INVALID)
																{
																	auto hBrush = CreateSolidBrush(ri->m_cBg);
																	if (hBrush != nullptr)
																	{
																		FillRect(lplvcd->nmcd.hdc, &rcBounds, hBrush);
																		DeleteBrush(hBrush);
																	}
																}
															}
															int iSizeX, iSizeY;
															if (ImageList_GetIconSize(himl, &iSizeX, &iSizeY))
															{
																if (ImageList_Draw(himl, lvi.iImage, lplvcd->nmcd.hdc, rcBounds.left + ((rcBounds.right - rcBounds.left) / 2) - (iSizeX / 2), rcBounds.top, iDrawFlags))
																	return (CDRF_SKIPDEFAULT | CDRF_NOTIFYPOSTPAINT);
															}
														}
													}
												}
											}
										}
//#endif

										// NB: CDRF_NOTIFYPOSTPAINT required to get the post paint message.
										return ( CDRF_NEWFONT|CDRF_NOTIFYPOSTPAINT );

										//RECT rcText, rcIcon, rcBounds;
										//TString tsText((UINT)1024);
										//UINT style = DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS;
										//COLORREF clrText = CLR_INVALID;
										//LVITEM lvi;
										//ZeroMemory(&lvi, sizeof(LVITEM));
										//HIMAGELIST himl = this->getImageList(LVSIL_SMALL);
										//
										//lvi.mask = LVIF_IMAGE|LVIF_STATE|LVIF_TEXT;
										//lvi.pszText = tsText.to_chr();
										//lvi.cchTextMax = 1023;
										//lvi.iItem = lplvcd->nmcd.dwItemSpec;
										//lvi.iSubItem = lplvcd->iSubItem;
										//
										//ListView_GetItem(this->m_Hwnd, &lvi);
										//
										//ListView_GetSubItemRect(this->m_Hwnd, lplvcd->nmcd.dwItemSpec, lplvcd->iSubItem, LVIR_BOUNDS, &rcBounds);
										//ListView_GetSubItemRect(this->m_Hwnd, lplvcd->nmcd.dwItemSpec, lplvcd->iSubItem, LVIR_ICON, &rcIcon);
										//
										//rcText = rcBounds;
										//rcText.left = rcIcon.right + 1;
										//
										//DrawTextW(lplvcd->nmcd.hdc, tsText.to_wchr(), tsText.len(), &rcText, style | DT_CALCRECT);
										//
										//if (lplvcd->nmcd.uItemState & CDIS_SELECTED) {
										//	// fill background with selected colour.
										//	FillRect(lplvcd->nmcd.hdc, &rcBounds, GetSysColorBrush(COLOR_HIGHLIGHT));
										//	// draw focus rect around selected item.
										//	DrawFocusRect(lplvcd->nmcd.hdc, &rcBounds);
										//	// set selected text colour.
										//	clrText = SetTextColor(lplvcd->nmcd.hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
										//}
										//else {
										//	// set text colour.
										//	clrText = SetTextColor(lplvcd->nmcd.hdc, lplvcd->clrText);
										//}
										//
										//if ((himl != nullptr) && (lvi.iImage > -1)) {
										//	ImageList_Draw(himl, lvi.iImage, lplvcd->nmcd.hdc, rcIcon.left, rcIcon.top, ILD_TRANSPARENT);
										//}
										//
										//if (!this->m_bCtrlCodeText) {
										//	if (this->m_bShadowText)
										//		dcxDrawShadowText(lplvcd->nmcd.hdc, tsText.to_wchr(), tsText.len(), &rcText, style, lplvcd->clrText, 0, 5, 5);
										//	else
										//		DrawTextW(lplvcd->nmcd.hdc, tsText.to_wchr(), tsText.len(), &rcText, style);
										//}
										//else
										//	mIRC_DrawText(lplvcd->nmcd.hdc, tsText, &rcText, style, this->m_bShadowText);
										//
										//if (clrText != CLR_INVALID)
										//	SetTextColor(lplvcd->nmcd.hdc, clrText);
										//
										//// NB: CDRF_NOTIFYPOSTPAINT required to get the post paint message.
										//return ( CDRF_NEWFONT|CDRF_NOTIFYPOSTPAINT|CDRF_SKIPDEFAULT );
									}
									break;

								case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
									{
										auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lplvcd->nmcd.lItemlParam);

										if (lpdcxlvi == nullptr)
											return CDRF_DODEFAULT;

										if (lpdcxlvi->pbar == nullptr) {
											if (this->m_hOldItemFont != nullptr) {
												SelectFont(lplvcd->nmcd.hdc, this->m_hOldItemFont);
												this->m_hOldItemFont = nullptr;
											}
											if (this->m_hItemFont != nullptr) {
												DeleteFont(this->m_hItemFont);
												this->m_hItemFont = nullptr;
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
										//	MapWindowRect(nullptr, this->m_Hwnd, &rcWin);
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
										//this->ScrollPbars((int)lplvcd->nmcd.dwItemSpec);
										// NB: This method updates all progress bars each time ANY item is drawn, to do this it itterates ALL items!! This is VERY slow!
										this->UpdateScrollPbars();
									}
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

							auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lpnmlv->lParam);

							if (lpdcxlvi != nullptr) {
								if (lpdcxlvi->pbar != nullptr)
									DestroyWindow(lpdcxlvi->pbar->getHwnd());

								for (auto &x : lpdcxlvi->vInfo)
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
							if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_DRAG))
								this->execAliasEx(TEXT("%s,%d"), TEXT("begindrag"), this->getUserID( ) );
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
							const auto wVKey = pnkd->wVKey;
							TCHAR cb[15];

							this->evalAliasEx(cb, 14, TEXT("%s,%d,%d"), TEXT("keydown"), this->getUserID( ), wVKey);

							// space to change checkbox state
							if ((wVKey == 32) && dcx_testflag(ListView_GetExtendedListViewStyle(this->m_Hwnd), LVS_EX_CHECKBOXES)) {

								// stop it from allowing user to change checkstate by pressing spacebar
								if (lstrcmp(TEXT("nospace"), cb) == 0) {
									bParsed = TRUE;
									return TRUE;
								}

								const auto index = ListView_GetNextItem(this->m_Hwnd, -1, LVNI_FOCUSED);

								// TODO: twig: change this if we get multiple checkbox columns working
								this->evalAliasEx(cb, 14, TEXT("%s,%d,%d,%d"), TEXT("stateclick"), this->getUserID(), index +1, 1);
							}
						}
						break;
						//case LVN_CHANGING: // 4294967196
						//case LVN_CHANGED: // 4294967195
						//	NM_FIRST
						//	LVN_FIRST
					case LVN_ITEMCHANGED:
						{
							if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {
								const auto pnmv = reinterpret_cast<const LPNMLISTVIEW>(lParam);
								if (pnmv->iItem == -1)
									break;

								if (dcx_testflag(pnmv->uChanged, LVIF_STATE))
								{
									if (dcx_testflag(pnmv->uNewState, LVIS_SELECTED) && !dcx_testflag(pnmv->uOldState, LVIS_SELECTED))
										this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("selected"), this->getUserID( ), pnmv->iItem +1, pnmv->iSubItem +1);
									else if (!dcx_testflag(pnmv->uNewState, LVIS_SELECTED) && dcx_testflag(pnmv->uOldState, LVIS_SELECTED))
										this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("deselected"), this->getUserID( ), pnmv->iItem +1, pnmv->iSubItem +1);
								}
							}
						}
						break;
					case LVN_GETINFOTIP:
						{
							const auto pGetInfoTip = reinterpret_cast<const LPNMLVGETINFOTIP>(lParam);
							LVITEM lvi = { 0 };
							lvi.iItem = pGetInfoTip->iItem;
							lvi.iSubItem = pGetInfoTip->iSubItem;
							lvi.mask = LVIF_PARAM;
							// Get item information.
							if (ListView_GetItem(this->m_Hwnd, &lvi)) {
								auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lvi.lParam);
								// return tooltip text, if any.
								if (!lpdcxlvi->tsTipText.empty())
									pGetInfoTip->pszText = lpdcxlvi->tsTipText.to_chr();
								bParsed = TRUE;
							}
						}
						break;
				} // switch
			}
			break;
	}
	return 0L;
}

LRESULT DcxListView::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
	switch( uMsg ) {

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_CONTEXTMENU:
			break;

			//case WM_COMMAND:
			//	{
			//		if (IsWindow((HWND) lParam)) {
			//			DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,TEXT("dcx_cthis"));
			//			if (c_this != nullptr) {
			//				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			//			}
			//		}
			//	}
			//	break;
			//
			//  case WM_DELETEITEM:
			//    {
			//		DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
			//		if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
			//			DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
			//			if (c_this != nullptr) {
			//				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			//			}
			//		}
			//    }
			//    break;
			//
			//  case WM_MEASUREITEM:
			//    {
			//		HWND cHwnd = GetDlgItem(this->m_Hwnd, wParam);
			//		if (IsWindow(cHwnd)) {
			//			DcxControl *c_this = (DcxControl *) GetProp(cHwnd,TEXT("dcx_cthis"));
			//			if (c_this != nullptr) {
			//				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			//			}
			//		}
			//    }
			//    break;
			//
			//  case WM_DRAWITEM:
			//    {
			//		DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
			//		if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
			//			DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
			//			if (c_this != nullptr) {
			//				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			//			}
			//		}
			//    }
			//   break;

		case WM_NOTIFY: 
			{

				dcxlParam(LPNMHDR, hdr);

				if (hdr == nullptr)
					break;

				//if (this->m_Hwnd != hdr->hwndFrom) {
				//	if (IsWindow(hdr->hwndFrom)) {
				//		DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,TEXT("dcx_cthis"));
				//		if (c_this != nullptr) {
				//			lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				//		}
				//	}
				//}
				//if (!bParsed) {
				switch( hdr->code ) {
				case NM_RCLICK:
					{
						if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {
							TCHAR ClassName[257];

							if (GetClassName(hdr->hwndFrom, ClassName, 256) != 0)
							{
								if (lstrcmpi(ClassName, TEXT("SysHeader32")) == 0) {

									HDHITTESTINFO hdti;
									if (GetCursorPos(&hdti.pt))
									{
										MapWindowPoints(nullptr, hdr->hwndFrom, &hdti.pt, 1);
										if (SendMessage(hdr->hwndFrom, HDM_HITTEST, (WPARAM)0, (LPARAM)&hdti) != -1)
											this->execAliasEx(TEXT("%s,%d,%d"), TEXT("hrclick"), this->getUserID(), hdti.iItem + 1);
									}
								}
							}
						}
						bParsed = TRUE;
					}
					break;
				case HDN_BEGINTRACK:
					{
						bParsed = TRUE;

						const dcxlParam(LPNMHEADER, pHeader);

						TCHAR ret[256];
						this->evalAliasEx( ret, 255, TEXT("%s,%d,%d"), TEXT("trackbegin"), this->getUserID(), pHeader->iItem +1);

						if (lstrcmp(TEXT("notrack"), ret) == 0)
							return TRUE;
					}
					break;

				case HDN_ITEMCLICK:
					{
						bParsed = TRUE;

						if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {
							const dcxlParam(LPNMHEADER, lphdr);

							switch (lphdr->iButton)
							{
							case 0: // left click
								{
									// commented code allows changing the sort up/down display by clicking on a header
									//HDITEM hdr = {0};
									//hdr.mask = HDI_FORMAT;
									//if (Header_GetItem(lphdr->hdr.hwndFrom,lphdr->iItem, &hdr)) {
									//	if (dcx_testflag(hdr.fmt, HDF_SORTDOWN)) {
									//		hdr.fmt &= ~HDF_SORTDOWN;
									//		hdr.fmt |= HDF_SORTUP;
									//	}
									//	else if (dcx_testflag(hdr.fmt, HDF_SORTUP)) {
									//		hdr.fmt &= ~HDF_SORTUP;
									//		hdr.fmt |= HDF_SORTDOWN;
									//	}
									//	else if ((hdr.fmt & (HDF_IMAGE|HDF_BITMAP)) == 0)
									//		hdr.fmt |= HDF_SORTDOWN;
									//	if ((hdr.fmt & (HDF_SORTUP|HDF_SORTDOWN)) > 0)
									//		Header_SetItem(lphdr->hdr.hwndFrom, lphdr->iItem, &hdr);
									//}
									this->execAliasEx(TEXT("%s,%d,%d"), TEXT("hsclick"), this->getUserID( ), lphdr->iItem + 1 );
								}
								break;
							case 1: // right click (never triggers)
								this->execAliasEx(TEXT("%s,%d,%d"), TEXT("hrclick"), this->getUserID( ), lphdr->iItem + 1 );
								break;
							case 2: // middle click (never triggers)
								this->execAliasEx(TEXT("%s,%d,%d"), TEXT("hmclick"), this->getUserID( ), lphdr->iItem + 1 );
								break;
							}
						}
					}
					break;

				case HDN_ITEMDBLCLICK:
					{
						bParsed = TRUE;

						if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {
							const dcxlParam(LPNMHEADER, lphdr);

							this->execAliasEx(TEXT("%s,%d,%d"), TEXT("hdclick"), this->getUserID( ), lphdr->iItem + 1 );
						}
					}
					break;
				case HDN_DROPDOWN:
					{
						bParsed = TRUE;

						if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {
							const dcxlParam(LPNMHEADER, lphdr);

							this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("hdropdown"), this->getUserID( ), lphdr->iItem + 1, lphdr->iButton );
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
					//		ScreenToClient( this->m_Hwnd, &hti.pt );
					//		ZeroMemory(&hti,sizeof(LVHITTESTINFO));
					//		hti.flags = LVHT_ONITEM;
					//		if (ListView_SubItemHitTest(this->m_Hwnd,&hti) != -1) {
					//			if (hti.flags & LVHT_ONITEM) {
					//				LVITEM lvi;
					//				ZeroMemory(&lvi,sizeof(LVITEM));
					//				lvi.mask = LVIF_PARAM;
					//				lvi.iItem = hti.iItem;
					//				lvi.iSubItem = hti.iSubItem;
					//				if (ListView_GetItem(this->m_Hwnd,&lvi)) {
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
				} // switch
				//}
			}
			break;

		case WM_HSCROLL:
		case WM_VSCROLL:
			{
				//if (IsWindow((HWND) lParam)) {
				//	DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,TEXT("dcx_cthis"));
				//	if (c_this != nullptr) {
				//		lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				//	}
				//	else {

			   if (LOWORD(wParam) == SB_ENDSCROLL)
				   this->execAliasEx(TEXT("%s,%d"), TEXT("scrollend"), this->getUserID());

			   //UINT sbAction = LOWORD(wParam);
			   //
			   //switch (sbAction) {
			   //case SB_ENDSCROLL:
				  // this->execAliasEx(TEXT("%s,%d"), TEXT("scrollend"), this->getUserID());
			   //case SB_PAGEUP:
			   //case SB_PAGEDOWN:
				  // this->UpdateScrollPbars();
				  // break;
			   ////default:
				  //// int iTop = this->getTopIndex() - 1;
				  //// int iBottom = this->getBottomIndex() + 1;
				  //// if (iTop >= 0)
					 ////  ScrollPbars(iTop);
				  //// if ((iBottom >= 0) && (iTop != iBottom))
					 ////  ScrollPbars(iBottom);
			   //}
				if (dcx_testflag(ListView_GetExtendedListViewStyle(this->m_Hwnd),LVS_EX_GRIDLINES))
					this->redrawWindow();

				//	}
				//}
				break;
			}

		case WM_PAINT:
			{
				if (!this->m_bAlphaBlend)
					break;

				PAINTSTRUCT ps;
				auto hdc = BeginPaint(this->m_Hwnd, &ps);

				bParsed = TRUE;

				// Setup alpha blend if any.
				auto ai = this->SetupAlphaBlend(&hdc);

				lRes = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

				this->FinishAlphaBlend(ai);

				EndPaint( this->m_Hwnd, &ps );
			}
			break;

		case WM_DESTROY:
			{
				delete this;
				bParsed = TRUE;
			}
			break;

		default:
			lRes = this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return lRes;
}

/*!
* \brief blah
*
* blah
*/

LRESULT CALLBACK DcxListView::EditLabelProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

	auto pthis = static_cast<DcxListView *>(GetProp(mHwnd, TEXT("dcx_pthis")));

	if (pthis == nullptr)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	switch( uMsg ) {

		case WM_GETDLGCODE:
			return DLGC_WANTALLKEYS;

		case WM_DESTROY:
			{
				RemoveProp( mHwnd, TEXT("dcx_pthis") );
				SubclassWindow( mHwnd, pthis->m_OrigEditProc );
			}
			break;

	}
	return CallWindowProc( pthis->m_OrigEditProc, mHwnd, uMsg, wParam, lParam );
}


DcxControl* DcxListView::CreatePbar(LPLVITEM lvi, const TString &styles) {
	// can only create progress for an existing item
	if (lvi == nullptr || lvi->lParam == NULL)
		return nullptr;

	auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lvi->lParam);

	if (lpdcxlvi->pbar)
		return nullptr;

	RECT rItem;

	// initial rect for pbar
	if (lvi->iSubItem == 0)
		ListView_GetItemRect(this->m_Hwnd, lvi->iItem, &rItem, LVIR_LABEL);
	else
		ListView_GetSubItemRect(this->m_Hwnd, lvi->iItem, lvi->iSubItem, LVIR_LABEL, &rItem);

	lpdcxlvi->iPbarCol = lvi->iSubItem;
	// controls within a listview have a problem in that they cant set an item height,
	// so they all appear very small, & dont look very good. (this can maybe be solved within NM_CUSTOMDRAW prepaint stage)
	const auto ID = mIRC_ID_OFFSET + (UINT)styles.getfirsttok(1).to_int();

	if (!this->m_pParentDialog->isIDValid(ID, true))
		throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Control with ID \"%d\" already exists"), ID - mIRC_ID_OFFSET));

	try {
		//TString ctrl_args;
		//ctrl_args.tsprintf(TEXT("%s %d %d %d %d %s"), styles.gettok(2).to_chr(), rItem.left, rItem.top, (rItem.right - rItem.left), (rItem.bottom - rItem.top), styles.gettok(3,-1).to_chr());
		//lpdcxlvi->pbar = DcxControl::controlFactory(this->m_pParentDialog,ID,ctrl_args,1,-1,this->m_Hwnd);
	
		//lpdcxlvi->pbar = new DcxProgressBar(this->getID(), this->m_pParentDialog, this->m_Hwnd, &rItem, styles);
		lpdcxlvi->pbar = new DcxProgressBar(ID, this->m_pParentDialog, this->m_Hwnd, &rItem, styles.getlasttoks());
		this->m_pParentDialog->addControl(lpdcxlvi->pbar);

		this->m_bHasPBars = true;

		return lpdcxlvi->pbar;
	}
	catch (std::exception &e) {
		this->showErrorEx(nullptr, TEXT("CreatePbar()"), TEXT("Unable To Create Control %d (%S)"), ID - mIRC_ID_OFFSET, e.what());
		throw;
	}
}


void DcxListView::UpdateScrollPbars(void) {

	if (!this->m_bHasPBars)
		return;

	const auto nCount = ListView_GetItemCount(this->m_Hwnd);
	const auto nCols = this->getColumnCount();
	const auto iTop = this->getTopIndex();
	const auto iBottom = this->getBottomIndex() + 1;

	//LPLVITEM lvi = new LVITEM;
	auto lvi = std::make_unique<LVITEM>();

	ZeroMemory(lvi.get(), sizeof(LVITEM));

	for (auto row = decltype(nCount){0}; row < nCount; row++) {
		this->ScrollPbars(row, nCols, iTop, iBottom, lvi.get());
	}
	// both for loops produce the same result, its a question of style, which is best?
	//for (auto row = decltype(nCount){0}; row < nCount; row++) {
	//	this->ScrollPbars(row, nCols, iTop, iBottom, lvi.get());
	//}
}

// BUG: when listview has horiz scrollbars pbar will be moved oddly when listview is scrolled horiz.
//			pbars are positioned relative to visible area of control & as such arn't scrolled.
void DcxListView::ScrollPbars(const int row, const int nCols, const int iTop, const int iBottom, LPLVITEM lvi) {

	for (auto col = decltype(nCols){0}; col < nCols; col++) {
		lvi->iItem = row;
		lvi->iSubItem = col;
		lvi->mask = LVIF_PARAM;

		if (!ListView_GetItem(this->m_Hwnd, lvi))
			continue;

		auto lpdcxlvi = reinterpret_cast<LPDCXLVITEM>(lvi->lParam);

		if (lpdcxlvi == nullptr)
			continue;

		if (lpdcxlvi->pbar == nullptr)
			continue;

		// isnt the right column to move it to
		if (lpdcxlvi->iPbarCol != col)
			continue;

		RECT rItem;

		// hide it if its scrolled off visible range
		//if (!ListView_IsItemVisible(this->m_Hwnd, lvi->iItem)) {
		if ((lvi->iItem < iTop) || (lvi->iItem > iBottom)) {
			ShowWindow(lpdcxlvi->pbar->getHwnd(), SW_HIDE);
			break;
		}
		else
			ShowWindow(lpdcxlvi->pbar->getHwnd(), SW_SHOW);

		// get coordinates to move to
		if (col == 0)
			ListView_GetItemRect(this->m_Hwnd, lvi->iItem, &rItem, LVIR_LABEL);
		else
			ListView_GetSubItemRect(this->m_Hwnd, lvi->iItem, lvi->iSubItem, LVIR_LABEL, &rItem);

		// show borders correctly
		rItem.bottom--;
		rItem.top++;
		rItem.left++;
		rItem.right--;

		RECT rcWin;
		if (GetWindowRect(lpdcxlvi->pbar->getHwnd(), &rcWin))
		{
			MapWindowRect(nullptr, this->m_Hwnd, &rcWin);
			if (!EqualRect(&rcWin, &rItem)) {
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
bool DcxListView::xmlLoadListview(const int nPos, const TString &name, TString &filename)
{
	if (!IsFile(filename))
		throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unable To Access File: %s"), filename.to_chr()));

	TiXmlDocument doc(filename.c_str());
	doc.SetCondenseWhiteSpace(false);
	TString tsBuf;

	const auto xmlFile = doc.LoadFile();
	if (!xmlFile)
		throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Not an XML File: %s"), filename.to_chr()));

	const auto *const xRoot = doc.FirstChildElement("dcxml");
	if (xRoot == nullptr)
		throw std::runtime_error("Unable Find 'dcxml' root");

	const auto *xElm = xRoot->FirstChildElement("listview_data");
	if (xElm == nullptr)
		throw std::runtime_error("Unable To Find 'listview_data' element");

	xElm = xElm->FirstChildElement(name.c_str());
	if (xElm == nullptr)
		throw std::runtime_error(Dcx::dcxGetFormattedString(TEXT("Unable To Find Dataset: %s"), name.to_chr()));

	this->setRedraw(FALSE);

	Auto(this->redrawWindow());
	Auto(this->setRedraw(TRUE));

	auto i = 0, nItem = nPos;
	LVITEM lvi;

	for (const auto *xNode = xElm->FirstChildElement("lvitem"); xNode != nullptr; xNode = xNode->NextSiblingElement("lvitem"))
	{
		auto lpmylvi = new DCXLVITEM;
		//auto lpmylvi = std::make_unique<DCXLVITEM>();

		lpmylvi->iPbarCol = 0;
		lpmylvi->pbar = nullptr;
		lpmylvi->vInfo.clear();

		xmlSetItem(nItem, 0, xNode, &lvi, lpmylvi, tsBuf);

		// Items state icon.
		auto stateicon = -1;
		const auto *attr = xNode->Attribute("stateicon", &i);
		if (attr != nullptr && i > -1)
			stateicon = i;

		// Items overlay icon.
		auto overlayicon = 0;
		attr = xNode->Attribute("overlayicon",&i);
		if (attr != nullptr && i > 0)
			overlayicon = i;

		lvi.iItem = ListView_InsertItem(this->m_Hwnd, &lvi);

		if (lvi.iItem == -1)
		{
			delete lpmylvi;
			throw std::runtime_error("Unable to add item");
		}

		if (stateicon > -1)
			ListView_SetItemState(this->m_Hwnd, lvi.iItem, (UINT)INDEXTOSTATEIMAGEMASK(stateicon), LVIS_STATEIMAGEMASK);

		// overlay is 1-based index, max 15 overlay icons
		if (overlayicon > 0 && overlayicon < 16)
			ListView_SetItemState(this->m_Hwnd, lvi.iItem, (UINT)INDEXTOOVERLAYMASK(overlayicon), LVIS_OVERLAYMASK);

		// Items checked state (if LVS_EX_CHECKBOXES style used)
		//attr = xNode->Attribute("checked",&i);
		//if (attr != nullptr && i > 0 && (ListView_GetExtendedListViewStyle(this->m_Hwnd) & LVS_EX_CHECKBOXES)) // items are always added in `unchecked` state
		//	ListView_SetCheckState(this->m_Hwnd, lvi.iItem, TRUE);
		i = queryIntAttribute(xNode, "checked");
		if (i > 0 && (ListView_GetExtendedListViewStyle(this->m_Hwnd) & LVS_EX_CHECKBOXES)) // items are always added in `unchecked` state
			ListView_SetCheckState(this->m_Hwnd, lvi.iItem, TRUE);

		// autosize the column
		attr = xNode->Attribute("autosize",&i);
		if (attr != nullptr && i > 0)
			this->autoSize(0, DCX_LV_COLUMNF_AUTO);				//LVSCW_AUTOSIZE
		else {
			attr = xNode->Attribute("autosizeheader",&i);
			if (attr != nullptr && i > 0)
				this->autoSize(0, DCX_LV_COLUMNF_AUTOHEADER);	//LVSCW_AUTOSIZE_USEHEADER
			else {
				attr = xNode->Attribute("autosizemax",&i);
				if (attr != nullptr && i > 0)
					this->autoSize(0, DCX_LV_COLUMNF_AUTOMAX);	//LVSCW_AUTOSIZE_MAX
			}
		}
		// add items tooltip
		attr = xNode->Attribute("tooltip");
		if (attr != nullptr) {
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
			TString cmd;
			cmd.tsprintf(TEXT("0 0 -T %d 0 %S"), lvi.iItem +1, attr);
			this->parseCommandRequest(cmd);
		}
		// add subitems
		auto nSubItem = 1;
		for (const auto *xSubNode = xNode->FirstChildElement("lvsubitem"); xSubNode != nullptr; xSubNode = xSubNode->NextSiblingElement("lvsubitem"))
		{
			xmlSetItem(nItem, nSubItem, xSubNode, &lvi, lpmylvi, tsBuf);

			// SubItems overlay icon.
			attr = xSubNode->Attribute("overlayicon",&i);
			if (attr != nullptr && i > 0) {
				lvi.mask |= LVIF_STATE;
				lvi.state |= INDEXTOOVERLAYMASK(i);
				lvi.stateMask |= LVIS_OVERLAYMASK;
			}
			if (ListView_SetItem(this->m_Hwnd, &lvi))
			{
				attr = xNode->Attribute("autosize",&i);
				if (attr != nullptr && i > 0)
					this->autoSize(nSubItem, DCX_LV_COLUMNF_AUTO);				// LVSCW_AUTOSIZE
				else {
					attr = xNode->Attribute("autosizeheader",&i);
					if (attr != nullptr && i > 0)
						this->autoSize(nSubItem, DCX_LV_COLUMNF_AUTOHEADER);	// LVSCW_AUTOSIZE_USEHEADER
					else {
						attr = xNode->Attribute("autosizemax",&i);
						if (attr != nullptr && i > 0)
							this->autoSize(nSubItem, DCX_LV_COLUMNF_AUTOMAX);	// LVSCW_AUTOSIZE_MAX
					}
				}
			}
			nSubItem++;
		}
		nItem++;
	}

	return true;
}
void DcxListView::xmlSetItem(const int nItem, const int nSubItem, const TiXmlElement *xNode, LPLVITEM lvi, LPDCXLVITEM lpmylvi, TString &tsBuf)
{
	if ((xNode == nullptr) || (lvi == nullptr) || (lpmylvi == nullptr))
		return;

	ZeroMemory(lvi, sizeof(LVITEM));

	auto i = 0;
	auto ri = new DCXLVRENDERINFO;

	ZeroMemory(ri, sizeof(DCXLVRENDERINFO));

	lvi->iItem = nItem;
	lvi->iSubItem = nSubItem;
	if (nSubItem == 0) {
		lvi->mask = LVIF_PARAM | LVIF_STATE;
		lvi->lParam = (LPARAM)lpmylvi;
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
	if (attr != nullptr && i > -1) {
		ri->m_cText = (COLORREF)i;
		ri->m_dFlags |= LVIS_COLOR;
	}
	else
		ri->m_cText = CLR_INVALID;

	// Items background colour.
	attr = xNode->Attribute("backgroundcolor", &i);
	if (attr != nullptr && i > -1) {
		ri->m_cBg = (COLORREF)i;
		ri->m_dFlags |= LVIS_BGCOLOR;
	}
	else
		ri->m_cBg = CLR_INVALID;

	lpmylvi->vInfo.push_back(ri);

	// Items icon.
	lvi->mask |= LVIF_IMAGE; // moved here to turn off icon when none is wanted.
	attr = xNode->Attribute("icon",&i);
	if (attr != nullptr && i > 0)
		lvi->iImage = i -1;
	else
		lvi->iImage = I_IMAGECALLBACK; // NB: using I_IMAGENONE cause a gap to be left for the icon for some reason. Using I_IMAGECALLBACK doesn't do this.

	// Items icon.
	attr = xNode->Attribute("group",&i);
	lvi->mask |= LVIF_GROUPID;
	if (attr != nullptr && i > -1 && ListView_HasGroup(this->m_Hwnd, (WPARAM)i))
		lvi->iGroupId = i;
	else
		lvi->iGroupId = I_GROUPIDNONE;

	// Items indent.
	attr = xNode->Attribute("indent",&i);
	if (attr != nullptr && i > -1) {
		lvi->iIndent = i;
		lvi->mask |= LVIF_INDENT;
	}
	else
		lvi->iIndent = 0;

	// Items Text.
	attr = xNode->Attribute("text");
	if (attr != nullptr) {
		lvi->mask |= LVIF_TEXT;
		tsBuf = attr;
		lvi->pszText = tsBuf.to_chr();
	}

	lvi->state = ri->m_dFlags;
	lvi->stateMask = (LVIS_FOCUSED|LVIS_SELECTED|LVIS_CUT|LVIS_DROPHILITED); // only alter the controls flags, ignore our custom ones.
}

//[N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] +flags dialog id (N|N1,N2)
bool DcxListView::ctrlLoadListview(const int nPos, const TString &tsData)
{
	//auto tsFlags(tsData.gettok( 10 ));
	//auto dialogname(tsData.gettok( 11 ).trim());
	//auto ctrl_ID = tsData.gettok( 12 ).to_int();
	//auto tsItem(tsData.gettok( 13 ).trim());
	//
	//if (tsFlags[0] != TEXT('+')) {
	//	this->showErrorEx(nullptr, TEXT("-a"), TEXT("Invalid flags specified, missing +: %s Only DCX dialogs are supported."), dialogname.to_chr());
	//	return false;
	//}
	//auto p_Dialog = Dcx::Dialogs.getDialogByName(dialogname);
	//if (p_Dialog == nullptr) {
	//	this->showErrorEx(nullptr, TEXT("-a"), TEXT("Invalid dialog name: %s Only DCX dialogs are supported."), dialogname.to_chr());
	//	return false;
	//}
	//auto p_Control = p_Dialog->getControlByID((UINT) ctrl_ID + mIRC_ID_OFFSET);
	//
	//if (p_Control == nullptr) {
	//	this->showErrorEx(nullptr, TEXT("-a"), TEXT("Invalid control id: %d Only DCX controls are supported."), ctrl_ID);
	//	return false;
	//}
	////mIRCLinker::evalex();
	return false;
}
//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
//tsData = [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] [+flags] [window/table] [item]
bool DcxListView::xLoadListview(const int nPos, const TString &tsData, const TCHAR *sTest, const TCHAR *sCount, const TCHAR *sGet, const TCHAR *sGetNamed)
{
	TString res;	// used to store the data returned by mIRC.
	const auto tsflags(tsData.getfirsttok(10));
	const auto tsName(tsData.getnexttok());			// tok 11
	const auto tsItem(tsData.getlasttoks());			// tok 12, -1

	// check table/window exists
	mIRCLinker::tsEvalex(res, sTest, tsName.to_chr());
	// if not exit
	if (tsName != res)
		throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid hashtable/window: %s"), tsName.to_chr()));

	// get the total number of items in the table.
	mIRCLinker::tsEvalex(res, sCount, tsName.to_chr());
#if TSTRING_TEMPLATES
	const auto iTotal = res.to_<UINT>();
#else
	const UINT iTotal = res.to_int();
#endif
	// if no items then exit.
	if (iTotal == 0)
		return false;

	// convert the flags string to a bin mask
	const auto iFlags = DcxListView::parseMassItemFlags(tsflags);
	auto iStart = 0, iEnd = 0;	// the first & last items in the hash table to add.
	TString input;	// the input string thats sent to the add item function.
					// The string end up having alot of space holders (0's), these are needed so the function can work with a normal xdid -a too.

	if (dcx_testflag(iFlags, LVIMF_NAMED)) {
		if (sGetNamed == nullptr)
			throw std::invalid_argument("Invalid flag used, +i is for hashtable items only");

		// add a single named item
		mIRCLinker::tsEvalex(res, sGetNamed, tsName.to_chr(), tsItem.to_chr());
		if (dcx_testflag(iFlags, LVIMF_ALLINFO))
			// add items data from [INDENT] onwards is taken from hashtable, including subitems.
			//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
			//   0     0     0      0   read from hashtable->
			//input.sprintf(TEXT("0 0 0 0 %s"),res);
		{
			input = TEXT("0 0 0 0 ");
			input += res;
		}
		else
			// only the item text is taken from the hashtable.
			//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
			//   0     0     0      0      0       +       0        0         0           0        0        0		read from hashtable->
			//input.sprintf(TEXT("0 0 0 0 0 + 0 0 0 0 0 0 %s"), res);
		{
			parseText2Item(res, input, tsData);
		}
		// add this item
		massSetItem(nPos, input);
		return true;
	}
	else if (iFlags == 0) {
		// no flags, just add text to nPos with no extras
		iStart = iEnd = tsItem.to_int();
		// first & last item are the same.
	}
	else if (dcx_testflag(iFlags, LVIMF_ADDALL)) {
		// add all items from N onwards inclusive
		iStart = tsItem.to_int();	// first item to add
		iEnd = (int)iTotal;			// last item to add is the last item in table.
	}
	else if (dcx_testflag(iFlags, LVIMF_NUMERIC)) {
		// numeric range supplied.
		iStart = tsItem.getfirsttok( 1, TSCOMMA).to_int();	// first item in hash table to be added taken from range.
		iEnd = tsItem.getnexttok( TSCOMMA).to_int();	// last item in hash table to be added taken from range.	tok 2
		if (iEnd < 0)
			iEnd = (int)(iTotal + iEnd);		// if iEnd is a negative number then make iEnd the last item in table + iEnd
	}
	// iStart & iEnd MUST be 1 or greater. (zero gives a hashtable/window item count.)
	if ((iStart < 1) || (iEnd < 1))
		throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid numeric supplied: %s"), tsItem.to_chr()));

	this->setRedraw(FALSE); // disable redraw while adding lots of items.

	Auto(this->setRedraw(TRUE));

	//int nItem = nPos;	// tmp var use to update the item pos for each item added.

	for (auto nItem = nPos; iStart <= iEnd; iStart++)
	{
		// get items data
		mIRCLinker::tsEvalex(res, sGet, tsName.to_chr(), iStart);
		if (dcx_testflag(iFlags, LVIMF_ALLINFO))
			// add items data from [INDENT] onwards is taken from hashtable, including subitems.
			//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
			//   0     0     0      0   read from hashtable->
			//input.sprintf(TEXT("0 0 0 0 %s"),res);
		{
			input = TEXT("0 0 0 0 ");
			input += res;
		}
		else
			// only the item text is taken from the hashtable.
			//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
			//   0     0     0      0      0       +       0        0         0           0        0        0		read from hashtable->
			//input.sprintf(TEXT("0 0 0 0 0 + 0 0 0 0 0 0 %s"), res);
		{
			parseText2Item(res, input, tsData);
		}
		massSetItem(nItem++, input);
	}

	return true;
}

/*
	*	Used by xdid -a to parse data & add items & subitems
	*	Used by (hash/win/ctrl)LoadListView() functions to parse data & add items & subitems
*/

void DcxListView::massSetItem(const int nPos, const TString &input)
{
	auto data(input.gettok(1, TSTAB).gettok(4, -1).trim());

#if TSTRING_TESTCODE
	const auto indent = data.getfirsttok(2).to_int();			// tok 2
	auto stateFlags = this->parseItemFlags(data++);				// tok 3
	auto icon = data++.to_<int>() - 1;							// tok 4
	const auto state = data++.to_<int>();						// tok 5
	auto overlay = data++.to_<int>();							// tok 6
	const auto group = data++.to_<int>();						// tok 7
	auto clrText = data.getnexttok().to_<COLORREF>();			// tok 8
	auto clrBack = data.getnexttok().to_<COLORREF>();			// tok 9
#else
	const auto indent = data.getfirsttok( 2 ).to_int();			// tok 2
	auto stateFlags = this->parseItemFlags(data.getnexttok( ));	// tok 3
	auto icon = data.getnexttok( ).to_int() -1;					// tok 4
	const auto state = data.getnexttok( ).to_int();				// tok 5
	auto overlay = data.getnexttok( ).to_int( );				// tok 6
	const auto group = data.getnexttok( ).to_int();				// tok 7
	auto clrText = (COLORREF)data.getnexttok( ).to_num();		// tok 8
	auto clrBack = (COLORREF)data.getnexttok( ).to_num();		// tok 9
#endif

	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));

	//LPDCXLVITEM lpmylvi = new DCXLVITEM;
	//LPDCXLVRENDERINFO ri = new DCXLVRENDERINFO;

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
	if (data.numtok( ) > 9) {
		itemtext = data.getlasttoks();		// tok 10, -1

		if (dcx_testflag(stateFlags, LVIS_HASHITEM))
			mIRCLinker::tsEvalex(itemtext, TEXT("$hget(%s,%s)"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
		else if (dcx_testflag(stateFlags, LVIS_HASHNUMBER))
			mIRCLinker::tsEvalex(itemtext,  TEXT("$hget(%s,%s).data"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
	}

	lvi.iItem = nPos;
	lvi.iImage = I_IMAGECALLBACK; // NB: using I_IMAGENONE causes a gap to be left for the icon for some reason. Using I_IMAGECALLBACK doesn't do this.
	lvi.state = (stateFlags & 0xFFFF); // mask out higher number flags. These flags cause the add to fail & arnt needed here anyway.
	lvi.stateMask = (LVIS_FOCUSED|LVIS_SELECTED|LVIS_CUT|LVIS_DROPHILITED); // only alter the controls flags, ignore our custom ones.
	lvi.iSubItem = 0;
	lvi.lParam = (LPARAM)lpmylvi.get();
	lvi.mask = LVIF_PARAM | LVIF_STATE;

	if (icon > -1)
		lvi.iImage = icon;

	lvi.mask |= LVIF_IMAGE; // when this is a set & iImage is -1 the parent does the drawing (& in this case does nothing so no icon)
							// this fixes the icon being drawn even when no icon set.
							// leave this separate for now untill it's been well tested.

	// groups not reportview only.
	lvi.iGroupId = I_GROUPIDNONE;	// set item as belonging to NO group by default.
	lvi.mask |= LVIF_GROUPID;		// otherwise if group flag isnt set I_GROUPIDCALLBACK is assumed.
	if (group > 0) {
		if (!ListView_HasGroup(this->m_Hwnd, (WPARAM)group))
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid Group specified: %d"), group));
		
		lvi.iGroupId = group;
	}

	if (indent > 0) {
		lvi.mask |= LVIF_INDENT;
		lvi.iIndent = indent;
	}

	// set text in case of pbar
	if (!dcx_testflag(stateFlags, LVIS_PBAR)) {
		lvi.mask |= LVIF_TEXT;
		lvi.pszText = itemtext.to_chr();
	}

	lvi.iItem = ListView_InsertItem(this->m_Hwnd, &lvi);

	if (lvi.iItem == -1)
		throw std::runtime_error("Unable to add item");

	auto tmp_lpmylvi = lpmylvi.release();

	// create pbar for first column
	if (dcx_testflag(stateFlags, LVIS_PBAR))
		CreatePbar(&lvi, itemtext);	// tok 1, -1

	// subitems
	const auto tabs = input.numtok(TSTAB);

	if (tabs > 1) {
		// ADD check for num columns
		for (auto i = decltype(tabs){2}; i <= tabs; i++)
		{
			data = input.gettok((int)i, TSTAB).trim();
			const auto nToks = data.numtok();

			if (nToks < 5)
				throw std::invalid_argument("Invalid subitem syntax");

#if TSTRING_TESTCODE
			stateFlags = parseItemFlags(data++);				// tok 1
			icon = data++.to_int() - 1;							// tok 2
			overlay = data++.to_<int>();						// tok 3
			clrText = data++.to_<COLORREF>();					// tok 4
			clrBack = data++.to_<COLORREF>();					// tok 5
#else
			stateFlags = parseItemFlags(data.getfirsttok( 1 ));	// tok 1
			icon = data.getnexttok( ).to_int() -1;				// tok 2
			overlay = data.getnexttok( ).to_int();				// tok 3
			clrText = (COLORREF)data.getnexttok( ).to_num();	// tok 4
			clrBack = (COLORREF)data.getnexttok().to_num();		// tok 5
#endif

			// setup colum #
			{
				auto ri = std::make_unique<DCXLVRENDERINFO>();

				ri->m_dFlags = stateFlags;

				if (dcx_testflag(stateFlags, LVIS_COLOR))
					ri->m_cText = clrText;
				else
					ri->m_cText = CLR_INVALID;

				if (dcx_testflag(stateFlags, LVIS_BGCOLOR))
					ri->m_cBg = clrBack;
				else
					ri->m_cBg = CLR_INVALID;

				tmp_lpmylvi->vInfo.push_back(ri.release());
			}
			lvi.iSubItem = (int)(i -1);
			lvi.mask = LVIF_TEXT;

			// icon
			lvi.mask |= LVIF_IMAGE; // moved here to turn off icon when none is wanted.
			if (icon > -1)
				lvi.iImage = icon;
			else
				lvi.iImage = I_IMAGECALLBACK; // NB: using I_IMAGENONE causes a gap to be left for the icon for some reason. Using I_IMAGECALLBACK doesn't do this.

			// overlay
			if (overlay > 0) {
				lvi.mask |= LVIF_STATE;
				lvi.state |= INDEXTOOVERLAYMASK(overlay);
				lvi.stateMask |= LVIS_OVERLAYMASK;
			}
			else
				lvi.state |= INDEXTOOVERLAYMASK(0);

			itemtext.clear();	// = TEXT("");
			if (nToks > 5) {
				itemtext = data.getlasttoks();	// tok 6, -1

				if ((dcx_testflag(stateFlags, LVIS_HASHITEM)) && (itemtext.numtok() == 2))
					mIRCLinker::tsEvalex(itemtext, TEXT("$hget(%s,%s)"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
				else if ((dcx_testflag(stateFlags, LVIS_HASHNUMBER)) && (itemtext.numtok() == 2))
					mIRCLinker::tsEvalex(itemtext,  TEXT("$hget(%s,%s).data"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
			}
			// create pbar for subitem
			if (dcx_testflag(stateFlags, LVIS_PBAR)) {
				CreatePbar(&lvi, itemtext);
				itemtext.clear();	// = TEXT("");
			}

			lvi.pszText = itemtext.to_chr();
			ListView_SetItem(this->m_Hwnd, &lvi);

			this->autoSize((int)(i -1),data.gettok( 1 ));
		}
	}

	if (state > -1)
		ListView_SetItemState(this->m_Hwnd, lvi.iItem, (UINT)INDEXTOSTATEIMAGEMASK(state), LVIS_STATEIMAGEMASK);

	// overlay is 1-based index, max 15 overlay icons
	if (overlay > 0 && overlay < 16)
		ListView_SetItemState(this->m_Hwnd, lvi.iItem, (UINT)INDEXTOOVERLAYMASK(overlay), LVIS_OVERLAYMASK);

	this->autoSize(0,input.gettok( 6 ));
}

void DcxListView::parseText2Item(const TString &tsTxt, TString &tsItem, const TString &tsData)
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
	tsItem += tsData.gettok( 2, 9); // copy flags & icon etc.. from /xdid -a line.
	// add item text.
#if TSTRING_TEMPLATES
	tsItem.addtok(tsTxt.getfirsttok(1, TSTAB));
#else
	tsItem.addtok(tsTxt.getfirsttok(1, TSTAB).to_chr());
#endif
	// add all subitems

	for (auto tsTok(tsTxt.getnexttok(TSTAB)); !tsTok.empty(); tsTok = tsTxt.getnexttok(TSTAB))
	{
		//[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text
		// again fill in blanks with 0's
		tsItem.addtok(TEXT("+ 0 0 0 0 "), TSTAB); // subitems are added without flags, not going to change this.
		// add subitems text.
		tsItem += tsTok;
	}
}

void DcxListView::getItemRange(const TString &tsItems, const int nItemCnt, int *iStart_range, int *iEnd_range)
{
	int iStart, iEnd;
	if (tsItems.numtok(TEXT("-")) == 2) {
		iStart = tsItems.getfirsttok(1, TEXT("-")).to_int() -1;
		iEnd = tsItems.getnexttok(TEXT("-")).to_int() -1;

		if (iEnd == -1)	// special case
			iEnd = nItemCnt -1;
	}
	else {
		iEnd = tsItems.to_int() -1;

		if (iEnd == -1)	// special case
			iStart = iEnd = nItemCnt -1;
		else
			iStart = iEnd;
	}
	*iStart_range = iStart;
	*iEnd_range = iEnd;
}

void DcxListView::DeleteColumns(const int nColumn)
{
	if (nColumn == -1) {
		// delete all columns
		auto nCol = this->getColumnCount();

		if (nCol > 0) {
			while (--nCol > 0)
				ListView_DeleteColumn(this->m_Hwnd, nCol);
		}
		this->m_iColumnCount = 0;
	}
	else {
		// delete specific column
		ListView_DeleteColumn(this->m_Hwnd, nColumn);
		this->m_iColumnCount--;
	}
}
