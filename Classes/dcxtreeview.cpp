/*!
 * \file dcxtreeview.cpp
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
 * © ScriptsDB.org - 2006
 */
#include "defines.h"
#include "Classes/dcxtreeview.h"
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

DcxTreeView::DcxTreeView( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles )
: DcxControl( ID, p_Dialog )
, m_iIconSize(16)
, m_colSelection(CLR_INVALID)
, m_hOldItemFont(nullptr)
, m_hItemFont(nullptr)
#ifdef DCX_USE_GDIPLUS
, m_pImage(nullptr)
, m_bResizeImage(false)
, m_bTileImage(false)
, m_iXOffset(0)
, m_iYOffset(0)
, m_bTransparent(false)
, m_bDestroying(false)
#endif
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	m_Hwnd = CreateWindowEx(	
		ExStyles | WS_EX_CLIENTEDGE,
		DCX_TREEVIEWCLASS,
		nullptr,
		WS_CHILD | Styles, 
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(nullptr), 
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( m_Hwnd , L" ", L" " );

	SendMessage( m_Hwnd, CCM_SETVERSION, (WPARAM) 6, (LPARAM) 0 );

	if ( ExStyles & TVS_CHECKBOXES )
		this->addStyle( TVS_CHECKBOXES );

	this->m_ToolTipHWND = (HWND)TreeView_GetToolTips(m_Hwnd);
	if (styles.istok(TEXT("balloon")) && this->m_ToolTipHWND != nullptr) {
		SetWindowLong(this->m_ToolTipHWND,GWL_STYLE,GetWindowStyle(this->m_ToolTipHWND) | TTS_BALLOON);
	}

	if (Dcx::VistaModule.isUseable()) {
		ExStyles = TreeView_GetExtendedStyle(m_Hwnd);
		parseTreeViewExStyles( styles, &ExStyles);
		TreeView_SetExtendedStyle(m_Hwnd, ExStyles, ExStyles);
	}

	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );

	DragAcceptFiles(m_Hwnd, TRUE);
	m_bTransparent = isExStyle(WS_EX_TRANSPARENT);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxTreeView::~DcxTreeView( ) {
	// make sure that there are no sensless events called when deleting all items
	m_bDestroying = true;
	// clear all items
	TreeView_DeleteAllItems(m_Hwnd);

	ImageList_Destroy(this->getImageList(TVSIL_NORMAL));
	ImageList_Destroy(this->getImageList(TVSIL_STATE));

	PreloadData();

	this->unregistreDefaultWindowProc();
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	*Styles |= TVS_INFOTIP;

	for (const auto &tsStyle: styles)
	{
#if DCX_USE_HASHING
		switch (std::hash<TString>{}(tsStyle))
		{
			case L"haslines"_hash:
				*Styles |= TVS_HASLINES;
				break;
			case L"hasbuttons"_hash:
				*Styles |= TVS_HASBUTTONS;
				break;
			case L"linesatroot"_hash:
				*Styles |= TVS_LINESATROOT;
				break;
			case L"showsel"_hash:
				*Styles |= TVS_SHOWSELALWAYS;
				break;
			case L"editlabel"_hash:
				*Styles |= TVS_EDITLABELS;
				break;
			case L"nohscroll"_hash:
				*Styles |= TVS_NOHSCROLL;
				break;
			case L"notooltips"_hash:
				*Styles |= TVS_NOTOOLTIPS;
				break;
			case L"noscroll"_hash:
				*Styles |= TVS_NOSCROLL;
				break;
			case L"fullrow"_hash:
				*Styles |= TVS_FULLROWSELECT;
				break;
			case L"singleexpand"_hash:
				*Styles |= TVS_SINGLEEXPAND;
				break;
			case L"checkbox"_hash:
				*ExStyles |= TVS_CHECKBOXES;
			default:
				break;
		}
#else
		if (tsStyle == TEXT("haslines"))
			*Styles |= TVS_HASLINES;
		else if ( tsStyle == TEXT("hasbuttons") ) 
			*Styles |= TVS_HASBUTTONS;
		else if ( tsStyle == TEXT("linesatroot") ) 
			*Styles |= TVS_LINESATROOT;
		else if ( tsStyle == TEXT("showsel") ) 
			*Styles |= TVS_SHOWSELALWAYS;
		else if ( tsStyle == TEXT("editlabel") ) 
			*Styles |= TVS_EDITLABELS;
		else if ( tsStyle == TEXT("nohscroll") ) 
			*Styles |= TVS_NOHSCROLL;
		else if ( tsStyle == TEXT("notooltips") ) 
			*Styles |= TVS_NOTOOLTIPS;
		else if ( tsStyle == TEXT("noscroll") ) 
			*Styles |= TVS_NOSCROLL;
		else if ( tsStyle == TEXT("fullrow") ) 
			*Styles |= TVS_FULLROWSELECT;
		else if ( tsStyle == TEXT("singleexpand") ) 
			*Styles |= TVS_SINGLEEXPAND;
		else if ( tsStyle == TEXT("checkbox") ) 
			*ExStyles |= TVS_CHECKBOXES;
#endif
	}

	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

void DcxTreeView::parseTreeViewExStyles( const TString &styles, LONG * ExStyles )
{
	// Vista+ ONLY!
	for (const auto &tsStyle : styles)
	{
#if DCX_USE_HASHING
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"fadebuttons"_hash:
			*ExStyles |= TVS_EX_FADEINOUTEXPANDOS;
			break;
		case L"doublebuffer"_hash:
			*ExStyles |= TVS_EX_DOUBLEBUFFER;
			break;
			//case L"multi"_hash:
			//  *ExStyles |= TVS_EX_MULTISELECT; // Style NOT to be used (unsupported by commctrl)
			//	break;
		case L"noident"_hash:
			*ExStyles |= TVS_EX_NOINDENTSTATE;
			break;
		case L"richtooltip"_hash:
			*ExStyles |= TVS_EX_RICHTOOLTIP;
			break;
		case L"autohscroll"_hash:
			*ExStyles |= TVS_EX_AUTOHSCROLL;
			break;
		default:
			break;
		}
#else
		if ( tsStyle == TEXT("fadebuttons") )
			*ExStyles |= TVS_EX_FADEINOUTEXPANDOS;
		else if ( tsStyle == TEXT("doublebuffer") )
			*ExStyles |= TVS_EX_DOUBLEBUFFER;
		//else if ( tsStyle == TEXT("multi") )
		//  *ExStyles |= TVS_EX_MULTISELECT; // Style NOT to be used (unsupported by commctrl)
		else if ( tsStyle == TEXT("noident") )
			*ExStyles |= TVS_EX_NOINDENTSTATE;
		else if ( tsStyle == TEXT("richtooltip") )
			*ExStyles |= TVS_EX_RICHTOOLTIP;
		else if ( tsStyle == TEXT("autohscroll") )
			*ExStyles |= TVS_EX_AUTOHSCROLL;
#endif
	}
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxTreeView::parseInfoRequest( const TString &input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
#if DCX_USE_HASHING
	const auto numtok = input.numtok();
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP] [PATH]
	case L"text"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		this->getItemText(&item, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	break;
	// [NAME] [ID] [PROP] [PATH]
	case L"icon"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		TVITEMEX tvi;
		tvi.hItem = item;
		tvi.mask = TVIF_IMAGE | TVIF_HANDLE;

		TreeView_GetItem(m_Hwnd, &tvi);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), (tvi.iImage > 10000 ? -2 : tvi.iImage) + 1);
	}
	break;
	// [NAME] [ID] [PROP] [PATH]
	case L"tooltip"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		TVITEMEX tvi;
		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;

		TreeView_GetItem(m_Hwnd, &tvi);
		auto lpdcxtvi = reinterpret_cast<LPDCXTVITEM>(tvi.lParam);

		if (lpdcxtvi != nullptr)
			szReturnValue = lpdcxtvi->tsTipText.to_chr();
	}
	break;
	// [NAME] [ID] [PROP]
	case L"seltext"_hash:
	{
		auto hItem = TreeView_GetSelection(m_Hwnd);

		this->getItemText(&hItem, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"selpath"_hash:
	{
		auto hItem = TreeView_GetSelection(m_Hwnd);
		const auto path(this->getPathFromItem(&hItem));

		szReturnValue = path.to_chr();
	}
	break;
	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N] [SUBPATH]
	case L"find"_hash:
	{
		if (numtok < 6)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto matchtext(input.getfirsttok(2, TSTABCHAR).trim());
		const auto params(input.getnexttok(TSTABCHAR).trim());		// tok 3

		if (matchtext.empty())
			throw Dcx::dcxException("No matchtext specified.");

		auto searchType = DcxSearchTypes::SEARCH_E;
		//const auto searchMode(params.getfirsttok(1));	// tok 1
		const auto searchMode(params++[0]);	// tok 1
		HTREEITEM startingPoint = TVI_ROOT;

		if (searchMode == TEXT('R'))
			searchType = DcxSearchTypes::SEARCH_R;
		else if (searchMode == TEXT('W'))
			searchType = DcxSearchTypes::SEARCH_W;

		const auto n = params++.to_int();	// tok 2
		auto matchCount = 0;

		if (params.numtok() > 2) {
			const auto path(params.getlasttoks());	// tok 3, -1

			startingPoint = this->parsePath(&path);

			if (startingPoint == nullptr)
				throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);
		}

		HTREEITEM result = nullptr;
		if (findItemText(&startingPoint, &result, matchtext, n, matchCount, searchType))
			szReturnValue = getPathFromItem(&result).to_chr();
		else if (n == 0)
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), matchCount);
	}
	break;
	// [NAME] [ID] [PROP] [PATH]
	case L"state"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		if (this->isStyle(TVS_CHECKBOXES)) {
			switch (TreeView_GetCheckState(m_Hwnd, item))
			{
			case 1:
				szReturnValue = TEXT('2');
				break;
			case 0:
				szReturnValue = TEXT('1');
				break;
			default:
				szReturnValue = TEXT('0');
				break;
			}
		}
		else
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), TreeView_GetItemState(m_Hwnd, item, TVIS_STATEIMAGEMASK));
	}
	break;
	// [NAME] [ID] [PROP] [PATH]
	case L"num"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto path(input.getlasttoks().trim());	// tok 4, -1
		HTREEITEM item = TVI_ROOT;

		if (path == TEXT("root")) {
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), getChildCount(&item));
			return;
		}

		item = parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getChildCount(&item));
	}
	break;
	// [NAME] [ID] [PROP] [PATH]
	case L"expand"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		dcx_ConChar(TreeView_GetItemState(m_Hwnd, item, TVIS_EXPANDED) & TVIS_EXPANDED, szReturnValue);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"mouseitem"_hash:
	{
		TVHITTESTINFO tvh = { 0 };

		if (!GetCursorPos(&tvh.pt))
			throw Dcx::dcxException("Unable to get cursor position");

		MapWindowPoints(nullptr, m_Hwnd, &tvh.pt, 1);
		TreeView_HitTest(m_Hwnd, &tvh);

		if (dcx_testflag(tvh.flags, TVHT_ONITEM))
			szReturnValue = getPathFromItem(&tvh.hItem).to_chr();
		else
			szReturnValue = TEXT('0');
	}
	break;
	// [NAME] [ID] [PROP] [PATH]
	case L"markeditem"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto path(input.getlasttoks().trim());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		TVITEMEX tvi = { 0 };

		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;

		if (!TreeView_GetItem(m_Hwnd, &tvi))
			throw Dcx::dcxException(TEXT("Unable to retrieve item: %"), path);

		auto lpdcxtvitem = reinterpret_cast<LPDCXTVITEM>(tvi.lParam);
		szReturnValue = lpdcxtvitem->tsMark.to_chr();
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
#else
	const auto numtok = input.numtok();
	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP] [PATH]
	if (prop == TEXT("text") && numtok > 3) {
		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		this->getItemText(&item, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == TEXT("icon") && numtok > 3) {
		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		TVITEMEX tvi; 
		tvi.hItem = item;
		tvi.mask = TVIF_IMAGE | TVIF_HANDLE;

		TreeView_GetItem(m_Hwnd, &tvi);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), (tvi.iImage > 10000 ? -2 : tvi.iImage) +1);
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == TEXT("tooltip") && numtok > 3) {
		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		TVITEMEX tvi;
		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;

		TreeView_GetItem(m_Hwnd, &tvi);
		auto lpdcxtvi = reinterpret_cast<LPDCXTVITEM>(tvi.lParam);

		if (lpdcxtvi != nullptr)
			szReturnValue = lpdcxtvi->tsTipText.to_chr();
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("seltext")) {
		auto item = TreeView_GetSelection(m_Hwnd);

		this->getItemText(&item, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("selpath") ) {
		auto hItem = TreeView_GetSelection(m_Hwnd);
		const auto path(this->getPathFromItem(&hItem));

		szReturnValue = path.to_chr();
	}
	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N] [SUBPATH]
	else if (prop == TEXT("find") && numtok > 5) {
		const auto matchtext(input.getfirsttok(2, TSTABCHAR).trim());
		const auto params(input.getnexttok(TSTABCHAR).trim());		// tok 3

		if (matchtext.empty())
			throw Dcx::dcxException("No matchtext specified.");

		auto searchType = DcxSearchTypes::SEARCH_E;
		//const auto searchMode(params.getfirsttok(1));	// tok 1
		const auto searchMode(params++[0]);	// tok 1
		HTREEITEM startingPoint = TVI_ROOT;

		if (searchMode == TEXT('R'))
			searchType = DcxSearchTypes::SEARCH_R;
		else if (searchMode == TEXT('W'))
			searchType = DcxSearchTypes::SEARCH_W;

		const auto n = params++.to_int();	// tok 2
		auto matchCount = 0;

		if (params.numtok() > 2) {
			const auto path(params.getlasttoks());	// tok 3, -1

			startingPoint = this->parsePath(&path);

			if (startingPoint == nullptr)
				throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);
		}

		HTREEITEM result = nullptr;
		if (findItemText(&startingPoint, &result, matchtext, n, matchCount, searchType))
			szReturnValue = getPathFromItem(&result).to_chr();
		else if (n == 0)
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), matchCount);
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == TEXT("state") && numtok > 3) {
		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		if (this->isStyle(TVS_CHECKBOXES)) {
			const auto state = TreeView_GetCheckState(m_Hwnd, item);

			if (state == 1)
				szReturnValue = TEXT('2');
			else
				dcx_ConChar(state == 0, szReturnValue);
		}
		else
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), TreeView_GetItemState(m_Hwnd, item, TVIS_STATEIMAGEMASK));
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == TEXT("num") && numtok > 3) {
		const auto path(input.getlasttoks().trim());	// tok 4, -1
		HTREEITEM item = TVI_ROOT;

		if (path == TEXT("root")) {
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), getChildCount(&item));
			return;
		}

		item = parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getChildCount(&item));
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == TEXT("expand") && numtok > 3) {
		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		dcx_ConChar(TreeView_GetItemState(m_Hwnd, item, TVIS_EXPANDED) & TVIS_EXPANDED, szReturnValue);
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("mouseitem")) {
		TVHITTESTINFO tvh = { 0 };

		if (!GetCursorPos(&tvh.pt))
			throw Dcx::dcxException("Unable to get cursor position");

		MapWindowPoints(nullptr, m_Hwnd, &tvh.pt, 1 );
		TreeView_HitTest( m_Hwnd, &tvh );

		if (dcx_testflag(tvh.flags, TVHT_ONITEM))
			szReturnValue = getPathFromItem(&tvh.hItem).to_chr();
		else
			szReturnValue = TEXT('0');
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == TEXT("markeditem") && numtok > 3) {
		const auto path(input.getlasttoks().trim());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		TVITEMEX tvi = { 0 };

		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;

		if (!TreeView_GetItem(m_Hwnd, &tvi))
			throw Dcx::dcxException(TEXT("Unable to retrieve item: %"), path);

		auto lpdcxtvitem = reinterpret_cast<LPDCXTVITEM>(tvi.lParam);
		szReturnValue = lpdcxtvitem->tsMark.to_chr();
	}
	else
		parseGlobalInfoRequest(input, szReturnValue);
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')]) {
		// If the window style for a tree-view control contains TVS_SCROLL and all items are deleted,
		// new items are not displayed until the window styles are reset.
		// The following code shows one way to ensure that items are always displayed.
		const auto styles = GetWindowStyle(m_Hwnd);
		TreeView_DeleteAllItems(m_Hwnd);
		SetWindowLong(m_Hwnd, GWL_STYLE, styles);
	}

	// xdid -a [NAME] [ID] [SWITCH] N N N ... N[TAB][+FLAGS] [#ICON] [#SICON] [#OVERLAY] [#STATE] [#INTEGRAL] [COLOR] [BKGCOLOR] Text[TAB]Tooltip Text
	if (flags[TEXT('a')]) {
		const auto n = input.numtok(TSTABCHAR);

		if (n > 1) {
			const auto path(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());	// tok 1
			const auto data(input.getnexttok(TSTABCHAR).trim());	// tok 2
			TString tooltip;

			if (n > 2)
				tooltip = input.getlasttoks().trim();	// tok 3, -1, TSTAB

			if (data.numtok( ) > 8)
				this->insertItem(&path, &data, &tooltip);
		}
	}
	// xdid -A [NAME] [ID] [SWITCH] N N N ... N[TAB][+FLAGS] [INFO]
	else if (flags[TEXT('A')]) {

		if (input.numtok(TSTABCHAR) < 2)
			throw Dcx::dcxException("Insufficient parameters");

		auto path(input.getfirsttok(1, TSTABCHAR).trim());		// tok 1
		const auto data(input.getnexttok(TSTABCHAR).trim());	// tok 2

		if (path.numtok() < 4)
			throw Dcx::dcxException("Insufficient parameters (path)");

		if (data.numtok() < 2)
			throw Dcx::dcxException("Insufficient parameters (args)");

		path = path.gettok(4, -1);

		HTREEITEM item = parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		const XSwitchFlags xflag(data.getfirsttok(1));	// tok 1
		const auto info(data.getlasttoks());		// tok 2, -1

		if (!xflag[TEXT('M')])
			throw Dcx::dcxException("Unknown flags");
		
		TVITEMEX tvi = { 0 };

		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;

		if (!TreeView_GetItem(m_Hwnd, &tvi))
			throw Dcx::dcxException(TEXT("Unable to retrieve item: %"), path);

		auto lpdcxtvitem = reinterpret_cast<LPDCXTVITEM>(tvi.lParam);

		lpdcxtvitem->tsMark = info;
		TreeView_SetItem(m_Hwnd, &tvi);
	}
	// xdid -B [NAME] [ID] [SWITCH] N N N
	else if (flags[TEXT('B')] && numtok > 3) {
		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(&path);
		
		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		TreeView_EnsureVisible(m_Hwnd, item); // make sure selected item is visible.
		TreeView_EditLabel(m_Hwnd, item);
	}
	// xdid -c [NAME] [ID] [SWITCH] N N N
	else if (flags[TEXT('c')] && numtok > 3) {
		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		TreeView_EnsureVisible(m_Hwnd, item); // make sure selected item is visible.
		TreeView_SelectItem(m_Hwnd, item);
	}
	// xdid -d [NAME] [ID] [SWITCH] N N N
	else if (flags[TEXT('d')] && numtok > 3) {
		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		TreeView_DeleteItem(m_Hwnd, item);
	}
	// xdid -g [NAME] [ID] [SWITCH] [HEIGHT]
	else if (flags[TEXT('g')] && numtok > 3) {
		const auto iHeight = input.getnexttok().to_int();	// tok 4

		if (iHeight > -2)
			TreeView_SetItemHeight(m_Hwnd, iHeight);
	}
	// xdid -i [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if ( flags[TEXT('i')] && numtok > 4 ) {
		const auto iFlags = this->parseColorFlags(input.getnexttok());	// tok 4

		const auto clr = input.getnexttok().to_<COLORREF>();	// tok 5
		if (dcx_testflag(iFlags,TVCOLOR_B))
			TreeView_SetBkColor( m_Hwnd, clr );

		if (dcx_testflag(iFlags,TVCOLOR_L))
			TreeView_SetLineColor( m_Hwnd, clr );

		if (dcx_testflag(iFlags,TVCOLOR_T))
			TreeView_SetTextColor( m_Hwnd, clr );

		if (dcx_testflag(iFlags,TVCOLOR_S))
			this->m_colSelection = clr;

		this->redrawWindow();
	}
	// xdid -j [NAME] [ID] [SWITCH] [+FLAGS] [N N N] [TAB] [ICON] [SICON] (OVERLAY)
	else if (flags[TEXT('j')] && numtok > 5) {
		const auto path(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());

		// Invalid parameters, missing icons segment.
		if (input.numtok(TSTABCHAR) < 2)
			throw Dcx::dcxException("Invalid parameters.");

		const auto icons(input.getnexttok(TSTABCHAR).trim());	// tok 2

		// Invalid parameters, missing icon args.
		if (icons.numtok() < 2)
			throw Dcx::dcxException("Invalid parameters.");

		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		auto nIcon = icons.getfirsttok(1).to_int() - 1;
		auto sIcon = icons.getnexttok().to_int() - 1;	// tok 2
		TVITEMEX tvi = { 0 };

		tvi.mask = TVIF_HANDLE;
		tvi.hItem = item;

		/*
		nIcon/sIcon values (actual values)
		-2 = ignore icon
		-1 = no icon
		0+ = valid icon
		*/

		// overlay
		if (icons.numtok() > 2) {
			const auto oIcon = icons.getnexttok().to_int();	// tok 3

			// overlay is 1-based index
			if (oIcon > -1)
				TreeView_SetItemState(m_Hwnd, tvi.hItem, INDEXTOOVERLAYMASK(oIcon), TVIS_OVERLAYMASK);

		}

		// if ignoring both nIcon and sIcon (if its -2 or less)
		if ((nIcon < -1) && (sIcon < -1))
			return;

		// normal icon
		if (nIcon > -2) {
			tvi.mask |= TVIF_IMAGE;

			// quickfix so it doesnt display an image
			// http://dcx.scriptsdb.org/bug/?do=details&id=350
			if (nIcon == -1)
				nIcon = I_IMAGENONE; //10000;

			tvi.iImage = nIcon;
		}

		// selected icon
		if (sIcon > -2) {
			tvi.mask |= TVIF_SELECTEDIMAGE;

			// quickfix so it doesnt display an image
			// http://dcx.scriptsdb.org/bug/?do=details&id=350
			if (sIcon == -1)
				sIcon = I_IMAGENONE; //10000;

			tvi.iSelectedImage = sIcon;
		}

		TreeView_SetItem(m_Hwnd, &tvi);
	}
	// xdid -k [NAME] [ID] [SWITCH] [STATE] N N N
	else if (flags[TEXT('k')] && numtok > 4) {
		const auto state = input.getnexttok().to_int();	// tok 4
		const auto path(input.getlasttoks());		// tok 5, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		TreeView_SetItemState(m_Hwnd, item, INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK);
	}
	// xdid -l [NAME] [ID] [SWITCH] [SIZE]
	else if ( flags[TEXT('l')] && numtok > 3 ) {
		auto size = input.getnexttok().to_int();	// tok 4

		if ( size != 32 && size != 24 )
			size = 16;

		this->m_iIconSize = size;
	}
	// xdid -m [NAME] [ID] [SWITCH] N N N{TAB}N N N
	else if (flags[TEXT('m')] && numtok > 3 && input.numtok(TSTABCHAR) > 1) {
		const auto pathFrom(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());
		const auto pathTo(input.getnexttok(TSTABCHAR).trim());	// tok 2

		auto item = this->copyAllItems(pathFrom, pathTo);

		TreeView_DeleteItem(m_Hwnd, item);
	}
	// xdid -n [NAME] [ID] [SWITCH] N N N{TAB}N N N
	else if (flags[TEXT('n')] && numtok > 3 && input.numtok(TSTABCHAR) > 1) {
		const auto pathFrom(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());
		const auto pathTo(input.getnexttok(TSTABCHAR).trim());	// tok 2

		this->copyAllItems(pathFrom, pathTo);
	}
	// xdid -o [NAME] [ID] [SWITCH] N N N [TAB] (Tooltip Text)
	else if (flags[TEXT('o')] && numtok > 3) {
		const auto path(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());
		TString tiptext;
		
		if (input.numtok(TSTABCHAR) > 1)
			tiptext = input.getnexttok( TSTABCHAR).trim();	// tok 2

		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		TVITEMEX tvi; 

		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM ; 

		if (TreeView_GetItem(m_Hwnd, &tvi)) {
			auto lpdcxtvitem = reinterpret_cast<LPDCXTVITEM>(tvi.lParam);

			if (lpdcxtvitem != nullptr)
				lpdcxtvitem->tsTipText = tiptext;
		}
	}
	// xdid -Q [NAME] [ID] [SWITCH] [+FLAGS] [COLOR] N N N
	else if ( flags[TEXT('Q')] && numtok > 5 ) {
		const auto iFlags = this->parseItemFlags(input.getnexttok());		// tok 4
		const auto clrText = input.getnexttok().to_<COLORREF>();	// tok 5
		const auto path(input.getlasttoks());							// tok 6, -1
		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		TVITEMEX tvi; 

		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM ; 

		if (!TreeView_GetItem(m_Hwnd, &tvi))
			throw Dcx::dcxException(TEXT("Unable to retrieve item: %"), path);

		auto lpdcxtvitem = reinterpret_cast<LPDCXTVITEM>(tvi.lParam);

		if (lpdcxtvitem == nullptr )
			throw Dcx::dcxException(TEXT("Unable to retrieve tag: %"), path);

		lpdcxtvitem->bUline = dcx_testflag(iFlags, TVIS_UNDERLINE);

		lpdcxtvitem->bBold = dcx_testflag(iFlags, TVIS_BOLD);

		lpdcxtvitem->bItalic = dcx_testflag(iFlags, TVIS_ITALIC);

		if (dcx_testflag(iFlags, TVIS_COLOR))
			lpdcxtvitem->clrText = clrText;
		else
			lpdcxtvitem->clrText = CLR_INVALID;

		redrawWindow();
	}
	// This is to avoid an invalid error message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
	}
	// xdid -t [NAME] [ID] [SWITCH] [+FLAGS] N N N
	else if (flags[TEXT('t')] && numtok > 4) {
		const auto iFlags = this->parseToggleFlags(input.getnexttok());	// tok 4
		const auto path(input.getlasttoks());							// tok 5, -1

		if (path == TEXT("root")) {
			HTREEITEM hStart = TVI_ROOT;

			if (dcx_testflag(iFlags,TVIE_EXPALL))
				this->expandAllItems(&hStart, TVE_EXPAND);
			else if (dcx_testflag(iFlags, TVIE_COLALL))
				this->expandAllItems(&hStart, TVE_COLLAPSE);

			return;
		}

		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		if (dcx_testflag(iFlags, TVIE_EXP))
			TreeView_Expand(m_Hwnd, item, TVE_EXPAND);
		else if (dcx_testflag(iFlags, TVIE_EXPPART))
			TreeView_Expand(m_Hwnd, item, TVE_EXPAND | TVE_EXPANDPARTIAL);
		else if (dcx_testflag(iFlags, TVIE_COL))
			TreeView_Expand(m_Hwnd, item, TVE_COLLAPSE);
		else if (dcx_testflag(iFlags, TVIE_COLRES))
			TreeView_Expand(m_Hwnd, item, TVE_COLLAPSE | TVE_COLLAPSERESET);
		else if (dcx_testflag(iFlags, TVIE_TOGGLE))
			TreeView_Expand(m_Hwnd, item, TVE_TOGGLE);
	}
	// xdid -u [NAME] [ID] [SWITCH]
	else if (flags[TEXT('u')]) {
		TreeView_SelectItem(m_Hwnd, nullptr);
	}
	// xdid -v [NAME] [ID] [SWITCH] N N N [TAB] (Item Text)
	else if (flags[TEXT('v')] && numtok > 3) {
		const auto path(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());
		TString itemtext;

		if (input.numtok(TSTABCHAR) > 1)
			itemtext = input.getnexttok( TSTABCHAR).trim();	// tok 2

		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		TVITEMEX tvi;

		tvi.mask = TVIF_TEXT | TVIF_HANDLE;
		tvi.hItem = item;
		tvi.pszText = itemtext.to_chr();
		TreeView_SetItem(m_Hwnd, &tvi);
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')] && numtok > 5) {
		const auto tsFlags(input.getnexttok());	// tok 4
		const auto iFlags = this->parseIconFlagOptions(tsFlags);

		HICON icon = nullptr;

		const auto index = input.getnexttok().to_int();	// tok 5
		auto filename(input.getlasttoks());			// tok 6, -1
		const auto bLarge = (this->m_iIconSize > 16);

		if (index >= 0)
			icon = dcxLoadIcon(index, filename, bLarge, tsFlags);

		if (dcx_testflag(iFlags,TVIT_NORMAL))
		{
			auto himl = this->getImageList(TVSIL_NORMAL);
			if (himl == nullptr) {
				himl = createImageList();

				if (himl != nullptr)
					this->setImageList(himl, TVSIL_NORMAL);
			}

			if (himl != nullptr) {
				if (index < 0) {
					if (!AddFileIcons(himl, filename, bLarge, -1))
						throw Dcx::dcxException(TEXT("Unable to Add %'s Icons"), filename);
				}
				else {
					const auto i = ImageList_AddIcon(himl, icon);

					if (tsFlags.find(TEXT('o'),0)) {
						// overlay image
						const auto io = tsFlags.find(TEXT('o'), 1) + 1;
						auto o = tsFlags.mid(io, (tsFlags.len() - io)).to_int();

						if (o < 1 || o > 15)
							throw Dcx::dcxException("Overlay index out of range (1 -> 15)");

						if (o > 0)
							ImageList_SetOverlayImage(himl, i, o);
					}
				}
			}
		}

		if (dcx_testflag(iFlags,TVIT_STATE))
		{
			auto himl = this->getImageList(TVSIL_STATE);
			if (himl == nullptr) {
				himl = this->createImageList();

				if (himl != nullptr)
					this->setImageList(himl, TVSIL_STATE);
			}

			if (himl != nullptr) {
				if (index < 0) {
					if (!AddFileIcons(himl, filename, bLarge, -1))
						throw Dcx::dcxException(TEXT("Unable to Add %'s Icons"), filename);
				}
				else
					ImageList_AddIcon(himl, icon);
			}
		}
		if (icon != nullptr)
			DestroyIcon(icon);
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if ( flags[TEXT('y')] && numtok > 3 ) {
		const auto iFlags = this->parseIconFlagOptions(input.getnexttok());	// tok 4

		if (dcx_testflag(iFlags, TVIT_NORMAL))
		{
			auto himl = this->getImageList( TVSIL_NORMAL );
			if ( himl != nullptr ) {
				ImageList_Destroy( himl );
				this->setImageList( nullptr, TVSIL_NORMAL );
			}
		}

		if (dcx_testflag(iFlags, TVIT_STATE))
		{
			auto himl = this->getImageList( TVSIL_STATE );
			if ( himl != nullptr ) {

				ImageList_Destroy( himl );
				this->setImageList( nullptr, TVSIL_STATE );
			}
		}
	}
	// xdid -z [NAME] [ID] [SWITCH] [+FLAGS] N N N [TAB] [ALIAS]
	else if (flags[TEXT('z')] && numtok > 4) {
		//LPDCXTVSORT dtvs = new DCXTVSORT;	// too big for stack, use heap.
		auto dtvs = std::make_unique<DCXTVSORT>();
		TVSORTCB tvs = { 0 };

		//dtvs->pthis = nullptr;
		dtvs->iSortFlags = this->parseSortFlags(input.getnexttok());	// tok 4
		dtvs->pthis = this;

		const auto path(input.getfirsttok(1, TSTABCHAR).gettok(5, -1).trim());	// tok 1, TSTAB

		if (input.numtok(TSTABCHAR) > 1)
			dtvs->tsCustomAlias = input.getnexttok(TSTABCHAR).trim();	// tok 2, TSTAB

		//ZeroMemory( &tvs, sizeof(TVSORTCB) );
		tvs.lpfnCompare = DcxTreeView::sortItemsEx;
		tvs.lParam = (LPARAM) dtvs.get();

		if (path == TEXT("root"))
			tvs.hParent = TVI_ROOT;
		else {

			auto item = this->parsePath(&path);

			if (item == nullptr)
				throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

			tvs.hParent = item;
		}
		if (dcx_testflag(dtvs->iSortFlags,TVSS_ALL))
			TreeView_SortChildrenCB(m_Hwnd, &tvs, TRUE); // NB: doesnt recurse!! This is a OS problem.
		else if (dcx_testflag(dtvs->iSortFlags,TVSS_SINGLE))
			TreeView_SortChildrenCB(m_Hwnd, &tvs, FALSE);
	}
	// xdid -G [NAME] [ID] [SWITCH] [+FLAGS] [X] [Y] (FILENAME)
	else if (flags[TEXT('G')] && numtok > 6) {
		const auto flag(input.getnexttok());			// tok 4
		this->m_iXOffset = input.getnexttok( ).to_int();	// tok 5
		this->m_iYOffset = input.getnexttok( ).to_int();	// tok 6
		auto filename(input.getlasttoks());				// tok 7, -1

		if (!this->m_bTransparent)
			this->removeExStyle(WS_EX_TRANSPARENT);
		if (filename != TEXT("none")) {
			if (!Dcx::GDIModule.isUseable())
				throw Dcx::dcxException("GDI+ Not Supported On This Machine");
			
			if (!LoadGDIPlusImage(flag, filename))
				throw Dcx::dcxException(TEXT("Unable to load Image: %"), filename);
			
			if (!this->m_bTransparent)
				this->setExStyle(WS_EX_TRANSPARENT);
		}
		this->redrawWindow();
	}
	// xdid -S [NAME] [ID] [SWITCH] [N (N...)][TAB][+FLAGS] [NAME] [FILENAME]
	else if (flags[TEXT('S')] && numtok > 5) {
		if (input.numtok(TSTABCHAR) != 2)
			throw Dcx::dcxException("Invalid Command Syntax.");

		const auto path(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());	// tok 1, TSTAB
		const auto fileData(input.getnexttok(TSTABCHAR));						// tok 2, TSTAB

		if (fileData.numtok() < 3)
			throw Dcx::dcxException("Invalid Command Syntax.");

		const auto name(fileData.getfirsttok(2).trim());		// tok 2
		auto filename(fileData.getlasttoks().trim());		// tok 3, -1

		if (name.empty())
			throw Dcx::dcxException("Invalid dataset");

		if (path.empty())
			throw Dcx::dcxException("Invalid path");

		auto item = this->parsePath(&path);

		if (item == nullptr)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		if (!this->xmlSaveTree(item, name, filename))
			throw Dcx::dcxException(TEXT("Unable To Save Data To: <%> %"), name, filename);
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxTreeView::getImageList( const int type ) const {

  return TreeView_GetImageList( m_Hwnd, type );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::setImageList( HIMAGELIST himl, const int type ) {
	auto o = TreeView_SetImageList(m_Hwnd, himl, type);
	if (o != nullptr && o != himl) // don't destroy if nullptr or the same list as just added.
		ImageList_Destroy(o);
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxTreeView::createImageList( ) {

  return ImageList_Create( m_iIconSize, m_iIconSize, ILC_COLOR32|ILC_MASK, 1, 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

//HTREEITEM DcxTreeView::insertItem( ) {
void DcxTreeView::insertItem(const TString * path, const TString * data, const TString * Tooltip) {

	HTREEITEM hParent = TVI_ROOT;
	HTREEITEM hAfter = TVI_ROOT;
	HTREEITEM hItem = nullptr;

	TVITEMEX tvi = { 0 };
	TVINSERTSTRUCT tvins = { 0 };

	//ZeroMemory(&tvins, sizeof(TVINSERTSTRUCT));
	//ZeroMemory(&tvi, sizeof(TVITEMEX));

	//tvi.mask = TVIF_TEXT | TVIF_STATE | TVIF_INTEGRAL | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvi.mask = TVIF_STATE | TVIF_PARAM;

	auto iFlags = this->parseItemFlags(data->getfirsttok(1));	// tok 1
	const auto icon = data->getnexttok().to_int() - 1;			// tok 2
	const auto sicon = data->getnexttok().to_int() - 1;			// tok 3
	const auto overlay = data->getnexttok().to_int();				// tok 4
	const auto state = data->getnexttok().to_int();				// tok 5
	const auto integral = data->getnexttok().to_int() + 1;			// tok 6
	const auto clrText = data->getnexttok().to_<COLORREF>();	// tok 7
	const auto clrBkg = data->getnexttok().to_<COLORREF>();	// tok 8

	// text
	auto itemtext(data->getlasttoks());							// tok 9, -1

	// path
	if (this->parsePath(path, &hParent, &hAfter) == nullptr)
		return;

	if (dcx_testflag(iFlags,TVIS_XML)) {
		const auto tsName(itemtext.getfirsttok(1));		// tok 1
		auto filename(itemtext.getlasttoks());			// tok 2, -1
		xmlLoadTree(hAfter, hParent, tsName, filename);
		return;
	}

	// parse DCX parameters
	//LPDCXTVITEM lpmytvi = new DCXTVITEM;
	auto lpmytvi = std::make_unique<DCXTVITEM>();

	lpmytvi->tsTipText = *Tooltip;

	lpmytvi->bUline = dcx_testflag(iFlags, TVIS_UNDERLINE);

	lpmytvi->bBold = dcx_testflag(iFlags, TVIS_BOLD);

	lpmytvi->bItalic = dcx_testflag(iFlags, TVIS_ITALIC);

	if (dcx_testflag(iFlags, TVIS_COLOR))
		lpmytvi->clrText = clrText;
	else
		lpmytvi->clrText = CLR_INVALID;

	if (dcx_testflag(iFlags, TVIS_BKG))
		lpmytvi->clrBkg = clrBkg;
	else
		lpmytvi->clrBkg = CLR_INVALID;

	{
		if ((dcx_testflag(iFlags, TVIS_HASHITEM)) && (itemtext.numtok() == 2))
			mIRCLinker::tsEvalex(itemtext, TEXT("$hget(%s,%s)"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
		else if ((dcx_testflag(iFlags, TVIS_HASHNUMBER)) && (itemtext.numtok() == 2))
			mIRCLinker::tsEvalex(itemtext,  TEXT("$hget(%s,%s).data"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
	}

	if (!itemtext.empty()) {
		tvi.pszText = itemtext.to_chr();
		tvi.cchTextMax = itemtext.len();
		tvi.mask |= TVIF_TEXT;
	}
	// icons
	// http://dcx.scriptsdb.org/bug/?do=details&id=350
	tvi.iImage = I_IMAGENONE;
	tvi.iSelectedImage = I_IMAGENONE;

	tvi.mask |= TVIF_IMAGE;
	if (icon > -1)
		tvi.iImage = icon;

	tvi.mask |= TVIF_SELECTEDIMAGE;
	if (sicon > -1)
		tvi.iSelectedImage = sicon;

	if (integral > 1) {
		tvi.iIntegral = integral;
		tvi.mask |= TVIF_INTEGRAL;
	}

	iFlags &= ~TVIS_DCXMASK; // exclude DCX flags, they were messing up state & overlay icon settings, found when saved data didnt match what was set.

	//tvi.hItem = hAfter;
	tvi.state = iFlags;
	tvi.stateMask = iFlags;
	tvi.lParam = (LPARAM) lpmytvi.get();

	tvins.itemex = tvi;
	tvins.hInsertAfter = hAfter;
	tvins.hParent = hParent;

	hItem = TreeView_InsertItem(m_Hwnd, &tvins);
	if (hItem == nullptr)
		throw Dcx::dcxException("Unable to add Item");

	lpmytvi->hHandle = hItem;
	lpmytvi.release();

	if (state > -1 && state < 5) // zero is no state image.
		TreeView_SetItemState(m_Hwnd, hItem, INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK);

	// overlay is 1-based index
	if (overlay > 0 && overlay < 16)
		TreeView_SetItemState(m_Hwnd, hItem, INDEXTOOVERLAYMASK(overlay), TVIS_OVERLAYMASK);

	return;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseIconFlagOptions(const TString &flags) {
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')]) 
		return iFlags;

	if (xflags[TEXT('n')])
		iFlags |= TVIT_NORMAL;
	if (xflags[TEXT('s')])
		iFlags |= TVIT_STATE;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseItemFlags(const TString &flags) {
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')]) 
		return iFlags;

	if (xflags[TEXT('b')])
		iFlags |= TVIS_BOLD;
	if (xflags[TEXT('c')])
		iFlags |= TVIS_COLOR;
	if (xflags[TEXT('e')])
		iFlags |= TVIS_EXPANDED;
	if (xflags[TEXT('i')])
		iFlags |= TVIS_ITALIC;
	if (xflags[TEXT('s')])
		iFlags |= TVIS_SELECTED;
	if (xflags[TEXT('u')])
		iFlags |= TVIS_UNDERLINE;
	if (xflags[TEXT('g')])
		iFlags |= TVIS_BKG;
	if (xflags[TEXT('H')])
		iFlags |= TVIS_HASHITEM;
	if (xflags[TEXT('n')])
		iFlags |= TVIS_HASHNUMBER;
	if (xflags[TEXT('x')])
		iFlags |= TVIS_XML;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseSortFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if ( xflags[TEXT('a')] )
		iFlags |= TVSS_ASC;
	if ( xflags[TEXT('b')] )
		iFlags |= TVSS_SINGLE;
	if ( xflags[TEXT('c')] )
		iFlags |= TVSS_CUSTOM;
	if ( xflags[TEXT('d')] )
		iFlags |= TVSS_DESC;
	if ( xflags[TEXT('n')] )
		iFlags |= TVSS_NUMERIC;
	if ( xflags[TEXT('r')] )
		iFlags |= TVSS_ALL;
	if ( xflags[TEXT('s')] )
		iFlags |= TVSS_CASE;
	if ( xflags[TEXT('t')] )
		iFlags |= TVSS_ALPHA;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseColorFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if (xflags[TEXT('b')])
		iFlags |= TVCOLOR_B;
	if (xflags[TEXT('l')])
		iFlags |= TVCOLOR_L;
	if (xflags[TEXT('s')])
		iFlags |= TVCOLOR_S;
	if (xflags[TEXT('t')])
		iFlags |= TVCOLOR_T;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseToggleFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if ( xflags[TEXT('a')] )
		iFlags |= TVIE_EXPALL;
	if ( xflags[TEXT('c')] )
		iFlags |= TVIE_COL;
	if ( xflags[TEXT('e')] )
		iFlags |= TVIE_EXP;
	if ( xflags[TEXT('p')] )
		iFlags |= TVIE_EXPPART;
	if ( xflags[TEXT('r')] )
		iFlags |= TVIE_COLRES;
	if ( xflags[TEXT('t')] )
		iFlags |= TVIE_TOGGLE;
	if ( xflags[TEXT('z')] )
		iFlags |= TVIE_COLALL;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

int CALLBACK DcxTreeView::sortItemsEx( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort ) {

	auto ptvsort = reinterpret_cast<LPDCXTVSORT>(lParamSort);
	auto lptvi1 = reinterpret_cast<LPDCXTVITEM>(lParam1);
	auto lptvi2 = reinterpret_cast<LPDCXTVITEM>(lParam2);

	if ((ptvsort == nullptr) || (lptvi1 == nullptr) || (lptvi2 == nullptr))
		return 0;

	ptvsort->pthis->getItemText( &lptvi1->hHandle, ptvsort->itemtext1, MIRC_BUFFER_SIZE_CCH );
	ptvsort->pthis->getItemText( &lptvi2->hHandle, ptvsort->itemtext2, MIRC_BUFFER_SIZE_CCH );

	// CUSTOM Sort
	if (dcx_testflag(ptvsort->iSortFlags, TVSS_CUSTOM)) {
		if (ptvsort->tsCustomAlias.empty())
			return 0;

		//TCHAR sRes[20];
		stString<20> sRes;
		mIRCLinker::evalex( sRes, Dcx::countof(sRes), TEXT("$%s(%s,%s)"), ptvsort->tsCustomAlias.to_chr( ), ptvsort->itemtext1, ptvsort->itemtext2 );

		auto ires = dcx_atoi(sRes);

		if (ires < -1)
			ires = -1;
		else if (ires > 1)
			ires = 1;

		if (dcx_testflag(ptvsort->iSortFlags, TVSS_DESC))
			return ires;
		else {

			if (ires == -1)
				return 1;
			else if (ires == 1)
				return -1;
		}
	}
	// NUMERIC Sort
	else if (dcx_testflag(ptvsort->iSortFlags, TVSS_NUMERIC)) {

		const auto i1 = dcx_atoi(ptvsort->itemtext1);
		const auto i2 = dcx_atoi(ptvsort->itemtext2);

		if (dcx_testflag(ptvsort->iSortFlags, TVSS_DESC)) {

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

		if (dcx_testflag(ptvsort->iSortFlags, TVSS_DESC)) {
			if (dcx_testflag(ptvsort->iSortFlags, TVSS_CASE))
				return -lstrcmp( ptvsort->itemtext1, ptvsort->itemtext2 );

			return -lstrcmpi( ptvsort->itemtext1, ptvsort->itemtext2 );
		}
		else {
			if (dcx_testflag(ptvsort->iSortFlags, TVSS_CASE))
				return lstrcmp( ptvsort->itemtext1, ptvsort->itemtext2 );

			return lstrcmpi( ptvsort->itemtext1, ptvsort->itemtext2 );
		}
	}

	return 0;
}

/*!
 * \brief blah
 *
 * blah
 */
TString DcxTreeView::getPathFromItem(HTREEITEM *item) const {
	HTREEITEM parent = *item;
	HTREEITEM current = nullptr;
	TString result;
	VectorOfInts vec;

	// Loop until we reach the topmost node of the tree.
	do {
		int i = 1;
		current = parent;

		// Count each previous node before the node containing our item.
		while ((current = TreeView_GetPrevSibling(m_Hwnd, current)) != nullptr) {
			i++;
		}

		vec.push_back(i);
	} while ((parent = TreeView_GetParent(m_Hwnd, parent)) != nullptr);

	// Construct the string containing the path backwards, as we traced it backwards.

//	// NB: error path reversed... when addtok used, must use instok() which doesnt support adding numerics (template version of instok() now does support numerics)
//	for (const auto &x: vec) {
//		result.instok(*itStart, 1);
//	}
//	return result.trim();

	// reverse iterator allows us to use addtok()
	//auto itStart = vec.rbegin();
	//auto itEnd = vec.rend();
	//
	//while (itStart != itEnd) {
	//	result.addtok(*itStart);
	//	++itStart;
	//}

	for (auto itStart = vec.rbegin(), itEnd = vec.rend(); itStart != itEnd; ++itStart)
		result.addtok(*itStart);

	// Trim to ensure clean path.
	return result.trim();
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::getItemText( HTREEITEM * hItem, TCHAR * szBuffer, const int cchTextMax ) const {

	TVITEMEX tvi;

	tvi.hItem = *hItem;
	tvi.mask = TVIF_TEXT | TVIF_HANDLE;
	tvi.cchTextMax = cchTextMax;
	tvi.pszText = szBuffer;

	if (!TreeView_GetItem(m_Hwnd, &tvi))
		szBuffer[0] = 0;
}

/*
 * Returns the number of children a node has.
 */
int DcxTreeView::getChildCount(HTREEITEM *hParent) const {
	auto i = 0;
	auto hItem = TreeView_GetChild(m_Hwnd, *hParent);

	// No children
	if (hItem == nullptr)
		return 0;

	// Child found
	++i;

	// Iterate through sibling nodes until reaching the end
	while ((hItem = TreeView_GetNextSibling(m_Hwnd, hItem)) != nullptr)
		++i;

	return i;
}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxTreeView::matchItemText(HTREEITEM *hItem, const TString &search, const DcxSearchTypes &SearchType) const {
	auto itemtext = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);
	itemtext[0] = TEXT('\0');

	getItemText(hItem, itemtext.get(), MIRC_BUFFER_SIZE_CCH);

	switch (SearchType) {
		case DcxSearchTypes::SEARCH_R:
			return isRegexMatch(itemtext.get(), search.to_chr());
		case DcxSearchTypes::SEARCH_W:
			return TString(itemtext).iswm(search);
		case DcxSearchTypes::SEARCH_E:
			return (lstrcmp(search.to_chr(), itemtext.get()) == 0);
	}

	return false;
}

/*!
 * \brief blah
 *
 * blah
 */
bool DcxTreeView::findItemText(HTREEITEM *hStart, HTREEITEM *result, const TString &queryText, const int n, int &matchCount, const DcxSearchTypes &searchType) const {
	HTREEITEM item;

	// Check if it should search child nodes
	if ((item = TreeView_GetChild(m_Hwnd, *hStart)) == nullptr)
		return false;

	do {
		if (this->matchItemText(&item, queryText, searchType))
			matchCount++;

		if (n != 0 && matchCount == n) {
			*result = item;
			return true;
		}

		if (this->findItemText(&item, result, queryText, n, matchCount, searchType))
			return true;

	} while ((item = TreeView_GetNextSibling(m_Hwnd, item)) != nullptr);

	return false;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::expandAllItems( HTREEITEM * hStart, const UINT expandOption ) {

  HTREEITEM hCurrentItem;

  if ( ( hCurrentItem = TreeView_GetChild( m_Hwnd, *hStart) ) == nullptr )
    return;

  do {

    this->expandAllItems( &hCurrentItem, expandOption );
    TreeView_Expand( m_Hwnd, hCurrentItem, expandOption );

  } while ( ( hCurrentItem = TreeView_GetNextSibling( m_Hwnd, hCurrentItem ) ) != nullptr );
}

/*!
 * \brief blah
 *
 * blah
 */

HTREEITEM DcxTreeView::cloneItem( HTREEITEM * hItem, HTREEITEM * hParentTo, HTREEITEM * hAfterTo ) {

	// Move the root node
	TCHAR itemtext[MIRC_BUFFER_SIZE_CCH];
	itemtext[0] = 0;
	TVITEMEX tvi;

	tvi.hItem = *hItem;
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_INTEGRAL | TVIF_PARAM | TVIF_HANDLE;
	tvi.cchTextMax = MIRC_BUFFER_SIZE_CCH;
	tvi.pszText = itemtext;

	if (!TreeView_GetItem(m_Hwnd, &tvi))
		return nullptr;

	TVINSERTSTRUCT tvin = { 0 };

	auto lpdcxtvitem = reinterpret_cast<LPDCXTVITEM>(tvi.lParam);
	auto lpdcxtvitem2 = new DCXTVITEM;

	*lpdcxtvitem2 = *lpdcxtvitem;

	tvi.hItem = 0;
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_INTEGRAL | TVIF_PARAM;
	tvi.lParam = (LPARAM)lpdcxtvitem2;

	tvin.hParent = *hParentTo;
	tvin.hInsertAfter = *hAfterTo;
	tvin.itemex = tvi;

	auto item = TreeView_InsertItem(m_Hwnd, &tvin);
	if (item == nullptr)
		delete lpdcxtvitem2;

	return item;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::copyAllItems( HTREEITEM *hItem, HTREEITEM * hParentTo ) {

  HTREEITEM hCurrentItem = nullptr, hNewItem = nullptr;
  HTREEITEM hAfterTo = TVI_LAST;

  if ( ( hCurrentItem = TreeView_GetChild( m_Hwnd, *hItem ) ) == nullptr )
    return;

  do {

    hNewItem = cloneItem( &hCurrentItem, hParentTo, &hAfterTo );
    if ( hNewItem != nullptr )
      copyAllItems( &hCurrentItem, &hNewItem );

  } while ( ( hCurrentItem = TreeView_GetNextSibling( m_Hwnd, hCurrentItem ) ) != nullptr );
}

HTREEITEM DcxTreeView::copyAllItems(const TString &pathFrom, const TString &pathTo)
{
	auto item = this->parsePath(&pathFrom);
	HTREEITEM hParentTo = TVI_ROOT;
	HTREEITEM hAfterTo = TVI_ROOT;

	if (item == nullptr)
		throw Dcx::dcxException(TEXT("Invalid FROM Path: %"), pathFrom);

	if (this->parsePath(&pathTo, &hParentTo, &hAfterTo) == nullptr)
		throw Dcx::dcxException(TEXT("Invalid TO Path: %"), pathTo);

	// Check if we're moving parent into a child branch
	if ((hParentTo != TVI_FIRST) && (hParentTo != TVI_LAST)) {
		auto tmp = hParentTo;

		// Trace back all the way to root
		while (tmp != nullptr && tmp != TVI_ROOT) {
			if (tmp == item)
				throw Dcx::dcxException(TEXT("Cannot copy parent node (%) to child node (%)."), pathFrom, pathTo);

			tmp = TreeView_GetParent(m_Hwnd, tmp);
		}
	}

	auto hNewItem = this->cloneItem(&item, &hParentTo, &hAfterTo);

	if (hNewItem == nullptr)
		throw Dcx::dcxException("Unable to move item.");

	this->copyAllItems(&item, &hNewItem);
	return item;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxTreeView::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch(uMsg) {
		case WM_NOTIFY : 
		{
			dcxlParam(LPNMHDR, hdr);

			if (hdr == nullptr)
				break;

			switch (hdr->code) {
			case NM_CLICK:
				{
				//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/treeview/reflist.asp
					TVHITTESTINFO tvh;
					if (!GetCursorPos(&tvh.pt))
						break;

					MapWindowPoints(nullptr, m_Hwnd, &tvh.pt, 1);
					TreeView_HitTest(m_Hwnd, &tvh);

					if (dcx_testflag(tvh.flags,TVHT_ONITEMBUTTON))
					{
						if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
							execAliasEx(TEXT("buttonclick,%u,%s"), getUserID(), getPathFromItem(&tvh.hItem).to_chr());
					}
					else if (dcx_testflag(tvh.flags, TVHT_ONITEMSTATEICON)) {
						if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
						{
							const auto path(getPathFromItem(&tvh.hItem));
							if (isStyle(TVS_CHECKBOXES))
								execAliasEx(TEXT("stateclick,%u,%u,%s"), getUserID(), (TreeView_GetCheckState(m_Hwnd, tvh.hItem) == 0 ? 2U : 1U) , path.to_chr());
							else
								execAliasEx(TEXT("stateclick,%u,%u,%s"), getUserID(), TreeView_GetItemState(m_Hwnd, tvh.hItem, TVIS_STATEIMAGEMASK), path.to_chr());
						}
					}
					else if (tvh.flags & TVHT_ONITEM) // dont use dcx_testflag() here as TVHT_ONITEM is a combination of several hit types.
					{
						//TreeView_SelectItem(m_Hwnd, tvh.hItem);	// why is this here? selections shows up fine without it

						if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
							execAliasEx(TEXT("sclick,%u,%s"), getUserID(), getPathFromItem(&tvh.hItem).to_chr());
					}
					// single click not on item
					else if (dcx_testflag(tvh.flags, TVHT_NOWHERE) || dcx_testflag(tvh.flags, TVHT_ONITEMRIGHT)) {
						if (dcx_testflag(m_pParentDialog->getEventMask(),DCX_EVENT_CLICK))
							execAliasEx(TEXT("sclick,%u"), getUserID());
					}

					bParsed = TRUE;
					break;
				}

			case NM_DBLCLK:
				{
					TVHITTESTINFO tvh;

					if (!GetCursorPos(&tvh.pt))
						break;

					MapWindowPoints(nullptr, m_Hwnd, &tvh.pt, 1);
					TreeView_HitTest(m_Hwnd, &tvh);

					//|| ( (dcx_testflag(tvh.flags, TVHT_ONITEMRIGHT)) && this->isStyle( TVS_FULLROWSELECT ) ) )
					if (dcx_testflag(tvh.flags, TVHT_ONITEM)) {
						TreeView_SelectItem(m_Hwnd, tvh.hItem);

						if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
							execAliasEx(TEXT("dclick,%u,%s"), getUserID(), getPathFromItem(&tvh.hItem).to_chr());
					}

					bParsed = TRUE;
					break;
				}

			case NM_RCLICK:
				{
					TVHITTESTINFO tvh;

					if (!GetCursorPos(&tvh.pt))
						break;

					MapWindowPoints(nullptr, m_Hwnd, &tvh.pt, 1);
					TreeView_HitTest(m_Hwnd, &tvh);

					//|| ( (dcx_testflag(tvh.flags, TVHT_ONITEMRIGHT)) && this->isStyle( TVS_FULLROWSELECT ) ) )
					if (dcx_testflag(tvh.flags,TVHT_ONITEM))
					{
						TreeView_SelectItem(m_Hwnd, tvh.hItem);

						if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
							execAliasEx(TEXT("rclick,%u,%s"), getUserID(), getPathFromItem(&tvh.hItem).to_chr());
					}
					else if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
						execAliasEx(TEXT("rclick,%u"), getUserID());

					bParsed = TRUE;
					return TRUE; // stop rclick being passed down to parent controls.
				}
				break;

			case TVN_SELCHANGED:
			{
				dcxlParam(LPNMTREEVIEW, lpnmtv);

				if (lpnmtv != nullptr && !m_bDestroying)
					execAliasEx(TEXT("selchange,%u,%s"), getUserID(), getPathFromItem(&lpnmtv->itemNew.hItem).to_chr());

				bParsed = TRUE;
			}
			break;

			case TVN_GETINFOTIP:
				{
					dcxlParam(LPNMTVGETINFOTIP, tcgit);

					if (tcgit != nullptr) {
						auto lpdcxtvi = reinterpret_cast<LPDCXTVITEM>(tcgit->lParam);
						if (lpdcxtvi != nullptr) {
							tcgit->pszText = lpdcxtvi->tsTipText.to_chr( );
							tcgit->cchTextMax = lpdcxtvi->tsTipText.len( );
						}
					}
					bParsed = TRUE;
				}
				break;

			case TVN_ITEMEXPANDING:
				{
					// disables redraw to stop flicker with bkg image.
					if (isExStyle(WS_EX_TRANSPARENT))
						setRedraw(FALSE);
				}
				break;

			case TVN_ITEMEXPANDED:
				{
					dcxlParam(LPNMTREEVIEW, lpnmtv);

					if (dcx_testflag(lpnmtv->action,TVE_COLLAPSE))
						execAliasEx(TEXT("collapse,%u,%s"), getUserID(), getPathFromItem(&lpnmtv->itemNew.hItem).to_chr());
					else if (dcx_testflag(lpnmtv->action,TVE_EXPAND))
						execAliasEx(TEXT("expand,%u,%s"), getUserID(), getPathFromItem(&lpnmtv->itemNew.hItem).to_chr());

					// re-enables redraw & updates.
					if (isExStyle(WS_EX_TRANSPARENT)) {
						setRedraw(TRUE);
						//InvalidateRect(m_Hwnd, nullptr, FALSE);
						//UpdateWindow(m_Hwnd);
						//if (m_pParentDialog->IsVistaStyle())
						redrawWindow();
						//else
						//	redrawBufferedWindow();
					}

					bParsed = TRUE;
				}
				break;

			case TVN_BEGINLABELEDIT:
				{
					bParsed = TRUE;
					dcxlParam(LPNMTVDISPINFO, lptvdi);

					TreeView_SelectItem( m_Hwnd,lptvdi->item.hItem );

					auto edit_hwnd = TreeView_GetEditControl(m_Hwnd);

					m_OrigEditProc = SubclassWindow( edit_hwnd, DcxTreeView::EditLabelProc );
					SetProp( edit_hwnd, TEXT("dcx_pthis"), (HANDLE) this );

					//TCHAR ret[256];
					//evalAliasEx( ret, Dcx::countof(ret), TEXT("labelbegin,%u"), getUserID( ) );
					//
					//return (lstrcmp(TEXT("noedit"), ret) == 0);

					//stString<256> sRet;
					//evalAliasEx( sRet, Dcx::countof(sRet), TEXT("labelbegin,%u"), getUserID( ) );
					//
					//return (sRet == TEXT("noedit"));

					return (m_pParentDialog->evalAliasT(TEXT("labelbegin,%"), getUserID()).second == TEXT("noedit"));
				}
				break;
			case TVN_ENDLABELEDIT:
				{
					bParsed = TRUE;

					dcxlParam(LPNMTVDISPINFO, lptvdi);

					if ( lptvdi->item.pszText == nullptr )
						execAliasEx(TEXT("labelcancel,%u"), getUserID( ) );
					else {
						//TCHAR ret[256];
						//evalAliasEx( ret, Dcx::countof(ret), TEXT("labelend,%u,%s"), getUserID( ), lptvdi->item.pszText );
						//
						//return (lstrcmp(TEXT("noedit"), ret) == 0);

						//stString<256> sRet;
						//evalAliasEx( sRet, Dcx::countof(sRet), TEXT("labelend,%u,%s"), getUserID( ), lptvdi->item.pszText );
						//
						//return (sRet == TEXT("noedit"));

						return (m_pParentDialog->evalAliasT(TEXT("labelend,%,%"), getUserID(), lptvdi->item.pszText).second == TEXT("noedit"));
					}
					return TRUE;
				}
				break;

			case NM_CUSTOMDRAW:
				{
					dcxlParam(LPNMTVCUSTOMDRAW, lpntvcd);

					bParsed = TRUE;

					switch( lpntvcd->nmcd.dwDrawStage ) {

					case CDDS_PREPAINT:
						return ( CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW );

					case CDDS_ITEMPREPAINT:
						{
							auto lpdcxtvi = reinterpret_cast<LPDCXTVITEM>(lpntvcd->nmcd.lItemlParam);

							if ( lpdcxtvi == nullptr )
								return CDRF_DODEFAULT;

							if ( lpdcxtvi->clrText != CLR_INVALID )
								lpntvcd->clrText = lpdcxtvi->clrText;

							const auto bSelected = (dcx_testflag(lpntvcd->nmcd.uItemState, CDIS_SELECTED));

							// draw unselected background color
							//if (this->isExStyle(WS_EX_TRANSPARENT) && !bSelected){
							//	lpntvcd->clrTextBk = CLR_NONE;
							//	SetBkMode(lpntvcd->nmcd.hdc, TRANSPARENT);
							//}
							//else if ((lpdcxtvi->clrBkg != -1) && !bSelected)
							if ((lpdcxtvi->clrBkg != CLR_INVALID) && !bSelected)
								lpntvcd->clrTextBk = lpdcxtvi->clrBkg;
							else if ((m_colSelection != CLR_INVALID) && bSelected)
								lpntvcd->clrTextBk = m_colSelection;

							if ( lpdcxtvi->bUline || lpdcxtvi->bBold || lpdcxtvi->bItalic) {
								auto hFont = GetWindowFont(m_Hwnd);

								LOGFONT lf;
								if (GetObject(hFont, sizeof(LOGFONT), &lf) != 0)
								{
									if (lpdcxtvi->bBold)
										lf.lfWeight |= FW_BOLD;
									if (lpdcxtvi->bUline)
										lf.lfUnderline = TRUE;
									if (lpdcxtvi->bItalic)
										lf.lfItalic = TRUE;

									m_hItemFont = CreateFontIndirect(&lf);
									if (m_hItemFont != nullptr)
										m_hOldItemFont = SelectFont(lpntvcd->nmcd.hdc, m_hItemFont);
								}
							}

							//TVITEMEX tvitem;
							//TCHAR buf[MIRC_BUFFER_SIZE_CCH];
							//ZeroMemory(&tvitem,sizeof(tvitem));
							//tvitem.hItem = (HTREEITEM)lpntvcd->nmcd.dwItemSpec;
							//tvitem.mask = TVIF_TEXT;
							//tvitem.pszText = buf;
							//tvitem.cchTextMax = MIRC_BUFFER_SIZE_CCH;
							//TreeView_GetItem(m_Hwnd, &tvitem);
							//TString tsItem(buf);
							//RECT rcTxt = lpntvcd->nmcd.rc;
							//if (!this->m_bCtrlCodeText) {
							//	if (bSelected && this->m_bShadowText) // could cause problems with pre-XP as this is commctrl v6+
							//		dcxDrawShadowText(lpntvcd->nmcd.hdc,tsItem.to_wchr(this->m_bUseUTF8), tsItem.wlen(),&rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, lpntvcd->clrText, 0, 5, 5);
							//	else
							//		DrawTextW( lpntvcd->nmcd.hdc, tsItem.to_wchr( ), tsItem.wlen( ), &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE );
							//}
							//else
							//	mIRC_DrawText(lpntvcd->nmcd.hdc, tsItem, &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, ((bSelected && this->m_bShadowText) ? true : false));
						}
						//return ( CDRF_NOTIFYPOSTPAINT | CDRF_SKIPDEFAULT );
						return ( CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT );

					case CDDS_ITEMPOSTPAINT:
						{
							if (m_hOldItemFont != nullptr) {
								SelectFont( lpntvcd->nmcd.hdc, m_hOldItemFont);
								m_hOldItemFont = nullptr;
							}
							if (m_hItemFont != nullptr) {
								DeleteFont(m_hItemFont);
								m_hItemFont = nullptr;
							}
							return CDRF_DODEFAULT;
						}

					default:
						return CDRF_DODEFAULT;
					}
				}
				break;

			case TVN_DELETEITEM:
				{
					dcxlParam(LPNMTREEVIEW, lpnmtv);
					if (lpnmtv == nullptr)
						break;

					auto lpdcxtvi = reinterpret_cast<LPDCXTVITEM>(lpnmtv->itemOld.lParam);

					delete lpdcxtvi;
				}
				break;
			case TVN_KEYDOWN:
				{
					dcxlParam(LPNMTVKEYDOWN, ptvkd);

					if (ptvkd->wVKey == VK_SPACE)
					{
						auto htvi = TreeView_GetSelection(m_Hwnd);
						if (htvi != nullptr) {
							const auto state = TreeView_GetCheckState(m_Hwnd, htvi);
							//this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("stateclick"), this->getUserID( ), (state ? 0 : 1), TreeView_MapHTREEITEMToAccID(m_Hwnd, htvi) );
							this->execAliasEx(TEXT("stateclick,%u,%d,%s"), getUserID( ), (state ? 0 : 1), getPathFromItem(&htvi).to_chr() );
						}
					}
				}
				break;
			//case TVN_ITEMCHANGED: // vista only :/
			//	{
			//		NMTVITEMCHANGE  *pnm = (NMTVITEMCHANGE *)lParam;
			//	}
			//	break;

			} // switch
		}
		break;
	}
	return 0L;
}

LRESULT DcxTreeView::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

		case WM_CONTEXTMENU:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
			break;

		case WM_PRINTCLIENT:
			{
				this->DrawClientArea((HDC)wParam, uMsg, lParam);
				bParsed = TRUE;
			}
			break;
		case WM_PAINT:
			{
				bParsed = TRUE;
				PAINTSTRUCT ps;

				auto hdc = BeginPaint(m_Hwnd, &ps);

				DrawClientArea(hdc, uMsg, lParam);

				EndPaint( m_Hwnd, &ps );
			}
			break;
		case WM_DESTROY:
			{
				delete this;
				bParsed = TRUE;
			}
			break;

		default:
			return CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return 0L;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT CALLBACK DcxTreeView::EditLabelProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

	auto pthis = reinterpret_cast<DcxTreeView *>(GetProp(mHwnd, TEXT("dcx_pthis")));

	switch( uMsg ) {

	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;

	case WM_DESTROY:
		{
			RemoveProp( mHwnd, TEXT("dcx_pthis") );
			if ((WNDPROC)GetWindowLongPtr(mHwnd, GWLP_WNDPROC) == DcxTreeView::EditLabelProc)
				SubclassWindow( mHwnd, pthis->m_OrigEditProc );
		}
		break;
	}
	return CallWindowProc( pthis->m_OrigEditProc, mHwnd, uMsg, wParam, lParam );
}

void DcxTreeView::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	// Setup alpha blend if any. Double Buffer is needed to stop flicker when a bkg image is used.
	auto ai = SetupAlphaBlend(&hdc, true);
	Auto(FinishAlphaBlend(ai));

#ifdef DCX_USE_GDIPLUS
	if (Dcx::GDIModule.isUseable() && m_pImage != nullptr)
		DrawGDIPlusImage(hdc);
#endif

	//CallWindowProc( m_DefaultWindowProc, m_Hwnd, uMsg, (WPARAM) hdc, lParam );
	CallDefaultProc(m_Hwnd, uMsg, (WPARAM)hdc, lParam);
}

// clears existing image and icon data and sets pointers to null
void DcxTreeView::PreloadData() {
#ifdef DCX_USE_GDIPLUS
	delete m_pImage;
	m_pImage = nullptr;
#endif
}

#ifdef DCX_USE_GDIPLUS
bool DcxTreeView::LoadGDIPlusImage(const TString &flags, TString &filename) {
	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("LoadGDIPlusImage() - Unable to Access File: %"), filename);

	m_pImage = new Gdiplus::Image(filename.to_chr(),TRUE);

	// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
	const auto status = m_pImage->GetLastStatus();
	if (status != Gdiplus::Status::Ok) {
		PreloadData();
		throw Dcx::dcxException(TEXT("LoadGDIPlusImage() - %"), GetLastStatusStr(status));
	}
	const XSwitchFlags xflags(flags);

	if (xflags[TEXT('h')]) { // High Quality
		this->m_CQuality = Gdiplus::CompositingQualityHighQuality;
		this->m_IMode = Gdiplus::InterpolationModeHighQualityBicubic;
	}
	else {
		this->m_CQuality = Gdiplus::CompositingQualityDefault;
		this->m_IMode = Gdiplus::InterpolationModeDefault;
	}

	if (xflags[TEXT('b')]) // Blend Image
		this->m_CMode = Gdiplus::CompositingModeSourceOver;
	else
		this->m_CMode = Gdiplus::CompositingModeSourceCopy;

	if (xflags[TEXT('a')]) // Anti-Aliased
		this->m_SMode = Gdiplus::SmoothingModeAntiAlias;
	else
		this->m_SMode = Gdiplus::SmoothingModeDefault;

	if (xflags[TEXT('t')]) // Tile
		this->m_bTileImage = true;
	else
		this->m_bTileImage = false;

	if (xflags[TEXT('r')]) // Tile
		this->m_bResizeImage = true;
	else
		this->m_bResizeImage = false;

	return true;
}

void DcxTreeView::DrawGDIPlusImage(HDC hdc)
{
	RECT rc;
	if (!GetClientRect(m_Hwnd, &rc))
		return;

	const auto w = (rc.right - rc.left), h = (rc.bottom - rc.top), x = rc.left, y = rc.top;
	if (m_bTransparent) {
		if (!m_bAlphaBlend)
			DrawParentsBackground( hdc, &rc);
	}
	else {
		auto hBrush = getBackClrBrush();
		if (hBrush == nullptr) {
			hBrush = CreateSolidBrush(GetBkColor(hdc));
			if (hBrush != nullptr) {
				FillRect(hdc, &rc, hBrush);
				DeleteBrush(hBrush);
			}
		}
		else
			FillRect( hdc, &rc, hBrush );
	}
	Gdiplus::Graphics grphx( hdc );

	grphx.SetCompositingQuality(m_CQuality);
	grphx.SetCompositingMode(m_CMode);
	grphx.SetSmoothingMode(m_SMode);

	if (((m_pImage->GetWidth() == 1) || (m_pImage->GetHeight() == 1)) && m_bResizeImage) {
		// This fixes a GDI+ bug when resizing 1 px images
		// http://www.devnewsgroups.net/group/microsoft.public.dotnet.framework.windowsforms/topic11515.aspx
		grphx.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
		grphx.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
	}
	else
		grphx.SetInterpolationMode(m_IMode);

	if (m_bTileImage) {
		Gdiplus::ImageAttributes imAtt;
		imAtt.SetWrapMode(Gdiplus::WrapModeTile);

		grphx.DrawImage(m_pImage,
			Gdiplus::Rect(x + m_iXOffset, y + m_iYOffset, w, h),  // dest rect
			0, 0, w, h,       // source rect
			Gdiplus::UnitPixel,
			&imAtt);
	}
	else if (m_bResizeImage)
		grphx.DrawImage( m_pImage, m_iXOffset, m_iYOffset, w, h );
	else
		grphx.DrawImage( m_pImage, m_iXOffset, m_iYOffset);
}
#endif
/*
	xmlLoadTree()
	Loads items into a treeview control from a dcxml file.
*/
HTREEITEM DcxTreeView::xmlLoadTree(HTREEITEM hInsertAfter, HTREEITEM hParent, const TString &name, TString &filename)
{
	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("xmlLoadTree() - Unable To Access File: %"), filename);

	TiXmlDocument doc(filename.c_str());
	doc.SetCondenseWhiteSpace(false);

	auto xmlFile = doc.LoadFile();
	if (!xmlFile)
		throw Dcx::dcxException(TEXT("xmlLoadTree() - Not an XML File: %"), filename);

	const auto xRoot = doc.FirstChildElement("dcxml");
	if (xRoot == nullptr)
		throw Dcx::dcxException("xmlLoadTree() - Unable Find 'dcxml' root");

	const auto *xElm = xRoot->FirstChildElement("treeview_data");
	if (xElm == nullptr)
		throw Dcx::dcxException("xmlLoadTree() - Unable To Find 'treeview_data' element");

	xElm = xElm->FirstChildElement(name.c_str());
	if (xElm == nullptr)
		throw Dcx::dcxException(TEXT("xmlLoadTree() - Unable To Find Dataset: %"), name);

	if ( hInsertAfter == TVI_ROOT)
		hInsertAfter = TreeView_GetRoot(m_Hwnd);

	this->setRedraw(FALSE);
	this->xmlInsertItems(hParent, hInsertAfter, xElm);
	this->setRedraw(TRUE);
	//InvalidateRect(m_Hwnd, nullptr, FALSE);
	//UpdateWindow(m_Hwnd);
	this->redrawWindow();
	return hInsertAfter;
}

/*
	xmlSaveTree()
	Saves Items from a treeview control to a dcxml file.
	NB: Removes any existing items from the named dataset.
*/
bool DcxTreeView::xmlSaveTree(HTREEITEM hFromItem, const TString &name, TString &filename)
{
	TiXmlDocument doc(filename.c_str());
	doc.SetCondenseWhiteSpace(false);

	if (IsFile(filename)) {
		bool xmlFile = doc.LoadFile();
		if (!xmlFile)
			throw Dcx::dcxException(TEXT("xmlSaveTree() - Not an XML File: %"), filename);
	}
	auto xRoot = doc.FirstChildElement("dcxml");
	if (xRoot == nullptr) {
		xRoot = reinterpret_cast<TiXmlElement *>(doc.InsertEndChild(TiXmlElement("dcxml")));

		if (xRoot == nullptr)
			throw Dcx::dcxException("xmlSaveTree() - Unable To Add Root <dcxml>");
	}

	auto xData = xRoot->FirstChildElement("treeview_data");
	if (xData == nullptr) {
		xData = reinterpret_cast<TiXmlElement *>(xRoot->InsertEndChild(TiXmlElement("treeview_data")));
		if (xData == nullptr)
			throw Dcx::dcxException("xmlSaveTree() - Unable to add <treeview_data> item");
	}

	auto xElm = xData->FirstChildElement(name.c_str());
	if (xElm == nullptr) {
		xElm = reinterpret_cast<TiXmlElement *>(xData->InsertEndChild(TiXmlElement(name.c_str())));
		if (xElm == nullptr)
			throw Dcx::dcxException("xmlSaveTree() - Unable to add named item");
	}

	xElm->Clear();

	if ( hFromItem == TVI_ROOT)
		hFromItem = TreeView_GetRoot(m_Hwnd);

	auto buf = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);

	if (!this->xmlGetItems(hFromItem, xElm, buf.get()))
		throw Dcx::dcxException("xmlSaveTree() - Unable To Add Items to XML");

	doc.SaveFile();

	return true;
}

/*
	xmlGetItems()
	Recursive function that loops through all treeview items & adds them to the xml data.
*/
bool DcxTreeView::xmlGetItems(const HTREEITEM hFirstSibling, TiXmlElement *xElm, TCHAR *buf)
{
	if (hFirstSibling == nullptr)
		return false;

	TVITEMEX tvi = { 0 };
	TiXmlElement *xTmp = nullptr;
	bool bRes = true;
	for (auto hSib = hFirstSibling; hSib != nullptr; hSib = TreeView_GetNextSibling(m_Hwnd, hSib)) {
		ZeroMemory(&tvi,sizeof(tvi));
		tvi.hItem = hSib;
		tvi.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE | TVIF_INTEGRAL | TVIF_STATE | TVIF_TEXT | TVIF_CHILDREN;
		tvi.cchTextMax = MIRC_BUFFER_SIZE_CCH;
		tvi.pszText = buf;

		if (!TreeView_GetItem(m_Hwnd, &tvi))
			break;

		auto lpmytvi = reinterpret_cast<LPDCXTVITEM>(tvi.lParam);
		if (lpmytvi == nullptr) {
			bRes = false;
			break;
		}
		{
			TiXmlElement xChild("tvitem");
			xChild.SetAttribute("text", TString(tvi.pszText).c_str());
			if (tvi.iImage > -1 && tvi.iImage != 10000)
				xChild.SetAttribute("image", tvi.iImage +1);
			if (tvi.iSelectedImage > -1 && tvi.iSelectedImage != 10000)
				xChild.SetAttribute("selectedimage", tvi.iSelectedImage +1);
			if (tvi.iExpandedImage > 0 && tvi.iExpandedImage != 10000)
				xChild.SetAttribute("expandedimage", tvi.iExpandedImage);
			if (tvi.iIntegral > 0)
				xChild.SetAttribute("itegral", tvi.iIntegral);
			if (dcx_testflag(tvi.stateMask, TVIS_SELECTED) && dcx_testflag(tvi.state, TVIS_SELECTED))
				xChild.SetAttribute("selected", 1);
			if (!lpmytvi->tsTipText.empty())
				xChild.SetAttribute("tooltip", lpmytvi->tsTipText.c_str());
			if (lpmytvi->bBold)
				xChild.SetAttribute("textbold", 1);
			if (lpmytvi->bItalic)
				xChild.SetAttribute("textitalic", 1);
			if (lpmytvi->bUline)
				xChild.SetAttribute("textunderline", 1);
			if (lpmytvi->clrBkg != CLR_INVALID)
				xChild.SetAttribute("backgroundcolor", lpmytvi->clrBkg);
			if (lpmytvi->clrText != CLR_INVALID)
				xChild.SetAttribute("textcolor", lpmytvi->clrText);
			auto i = (((tvi.state & TVIS_OVERLAYMASK) >> 8) & 0xFF);
			if (i > 0 && i < 16) // zero means no overlay, so don't save
				xChild.SetAttribute("overlay", i);
			i = (((tvi.state & TVIS_STATEIMAGEMASK) >> 12) & 0xFF);
			if (i > 0 && i < 5)
				xChild.SetAttribute("state", i); // zero means no state image so don't save
			xTmp = xElm->InsertEndChild(xChild)->ToElement();
			if (xTmp == nullptr) {
				bRes = false;
				break;
			}
		}
		if (tvi.cChildren > 0 && xTmp != nullptr)
			bRes = this->xmlGetItems(TreeView_GetChild(m_Hwnd, hSib), xTmp, buf);
	}
	return bRes;
}

/*
	xmlInsertItems()
	Recursive function that inserts items into a treeview control from the xml data.
*/
const TiXmlElement *DcxTreeView::xmlInsertItems(HTREEITEM hParent, HTREEITEM &hInsertAfter, const TiXmlElement *xElm)
{
	TVINSERTSTRUCT tvins = { 0 };
	const char *attr = nullptr;
	const TiXmlElement *xRes = xElm;

	for (const auto *xNode = xElm->FirstChildElement("tvitem"); xNode != nullptr; xNode = xNode->NextSiblingElement("tvitem")) {
		ZeroMemory(&tvins, sizeof(tvins));
		tvins.hInsertAfter = hInsertAfter;
		tvins.hParent = hParent;
		{ // scope to limit usage of lpmytvi
			//auto lpmytvi = new DCXTVITEM;
			auto lpmytvi = std::make_unique<DCXTVITEM>();

			lpmytvi->hHandle = nullptr;

			tvins.itemex.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE;
			tvins.itemex.lParam = (LPARAM)lpmytvi.get();

			// Items Icon.
			tvins.itemex.iImage = queryIntAttribute(xNode, "image", I_IMAGECALLBACK) - 1;	// NB: I_IMAGECALLBACK is change to I_IMAGENONE by the -1

			// Items Selected state icon.
			tvins.itemex.iSelectedImage = queryIntAttribute(xNode, "selectedimage", tvins.itemex.iImage + 1) - 1;

			// Items expanded state icon.
			tvins.itemex.iExpandedImage = queryIntAttribute(xNode, "expandedimage", I_IMAGECALLBACK) - 1;	// NB: I_IMAGECALLBACK is change to I_IMAGENONE by the -1

			// Items height integral.
			tvins.itemex.iIntegral = queryIntAttribute(xNode, "integral");
			if (tvins.itemex.iIntegral > 0)
				tvins.itemex.mask |= TVIF_INTEGRAL;

			// Items selected
			if (queryIntAttribute(xNode, "selected") > 0) {
				tvins.itemex.state |= TVIS_SELECTED;
				tvins.itemex.stateMask |= TVIS_SELECTED;
			}

			// Items tooltip
			attr = xNode->Attribute("tooltip");
			if (attr != nullptr)
				lpmytvi->tsTipText = attr;

			// Items text colour.
			lpmytvi->clrText = static_cast<COLORREF>(queryIntAttribute(xNode, "textcolor", CLR_INVALID));

			// Items background colour.
			lpmytvi->clrBkg = static_cast<COLORREF>(queryIntAttribute(xNode, "backgroundcolor", CLR_INVALID));

			// Is Item text in Bold?
			if (queryIntAttribute(xNode, "textbold") > 0) {
				lpmytvi->bBold = true;
				tvins.itemex.state |= TVIS_BOLD;
				tvins.itemex.stateMask |= TVIS_BOLD;
			}
			else
				lpmytvi->bBold = false;

			// Is item text in italics?
			lpmytvi->bItalic = (queryIntAttribute(xNode, "textitalic") > 0);

			// Is item text underlined?
			lpmytvi->bUline = (queryIntAttribute(xNode, "textunderline") > 0);

			// Items Text.
			TString tsAttr(xNode->Attribute("text")); // MUST be before/outside if()
			if (!tsAttr.empty()) {
				tvins.itemex.cchTextMax = tsAttr.len();
				tvins.itemex.pszText = tsAttr.to_chr();
				tvins.itemex.mask |= TVIF_TEXT;
			}
			hInsertAfter = TreeView_InsertItem(m_Hwnd, &tvins);
			if (hInsertAfter == nullptr)
				throw Dcx::dcxException("xmlInsertItems() - Unable To Add XML Item To TreeView");

			lpmytvi->hHandle = hInsertAfter;
			lpmytvi.release(); // <- release mem so its not deleted.
		}
		// Items state icon.
		auto i = queryIntAttribute(xNode, "state");
		if (i < 5) // zero means no state icon anyway.
			TreeView_SetItemState(m_Hwnd, hInsertAfter, INDEXTOSTATEIMAGEMASK(i), TVIS_STATEIMAGEMASK);

		// Items overlay icon.
		// overlay is 1-based index
		i = queryIntAttribute(xNode, "overlay");
		if (i > 0 && i < 16)
			TreeView_SetItemState(m_Hwnd, hInsertAfter, INDEXTOOVERLAYMASK(i), TVIS_OVERLAYMASK);

		if (xNode->FirstChild("tvitem") != nullptr) {
			// item has children. NB: DON'T update xNode to the result of this call as this stops subsequent items being added.
			this->xmlInsertItems(hInsertAfter, hInsertAfter, xNode);
		}
		xRes = xNode;
	}
	return xRes;
}

const TString DcxTreeView::getStyles(void) const {
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);
	const auto ExStyles = GetWindowExStyle(m_Hwnd);

	if (dcx_testflag(Styles, TVS_HASLINES))
		styles.addtok(TEXT("haslines"));
	if (dcx_testflag(Styles, TVS_HASBUTTONS))
		styles.addtok(TEXT("hasbuttons"));
	if (dcx_testflag(Styles, TVS_LINESATROOT))
		styles.addtok(TEXT("linesatroot"));
	if (dcx_testflag(Styles, TVS_SHOWSELALWAYS))
		styles.addtok(TEXT("showsel"));
	if (dcx_testflag(Styles, TVS_EDITLABELS))
		styles.addtok(TEXT("editlabel"));
	if (dcx_testflag(Styles, TVS_NOHSCROLL))
		styles.addtok(TEXT("nohscroll"));
	if (dcx_testflag(Styles, TVS_FULLROWSELECT))
		styles.addtok(TEXT("fullrow"));
	if (dcx_testflag(Styles, TVS_SINGLEEXPAND))
		styles.addtok(TEXT("singleexpand"));
	if (dcx_testflag(ExStyles, TVS_CHECKBOXES))
		styles.addtok(TEXT("checkbox"));
	return styles;
}

/*
 * Retrieves an item from the path specified.
 *
 * If the path is invalid, it will return nullptr.
 * If hParent and hInsertAt parameters are filled, it is assumed we are searching for a place to insert the item
 */
HTREEITEM DcxTreeView::parsePath(const TString *path, HTREEITEM *hParent, HTREEITEM *hInsertAt) const {
	HTREEITEM current = TVI_ROOT;
	auto foundSoFar = current;
	const auto count = path->numtok();
	//int k = 1;                         // iterator for path directories
	const auto bFillLocation = ((hParent != nullptr) && (hInsertAt != nullptr));

	// Invalid path - no directives.
	if (count == 0)
		return nullptr;

	// Iterate through each directive in the path string
	for (auto k = decltype(count){1}; k <= count; k++)
	{
		// Convert the path directory.
		const auto dir = path->gettok(k).to_int();

		// Keep track of the current parent.
		if (bFillLocation)
			*hParent = current;

		current = TreeView_GetChild(m_Hwnd, current);

		// Adding first child item, with the 2nd last directive as the parent node
		if ((current == nullptr) && (getChildCount(&foundSoFar) == 0) && bFillLocation && (k == count)) {
			*hParent = foundSoFar;
			*hInsertAt = TVI_FIRST;
			return foundSoFar;
		}

		// Return first child
		if ((dir == 1)) {
			if (bFillLocation) {
				*hInsertAt = current;
			}
		}
		// Insert at very start.
		// Not used unless for insertion or moving.
		else if (dir == 0) {
			if (bFillLocation) {
				*hParent = foundSoFar;
				*hInsertAt = TVI_FIRST;
			}
		}
		// Last item in current branch (Handles negative numbers as last item)
		// else if (dir == -1)
		else if (dir < 0) {
			HTREEITEM tmp;

			while ((tmp = TreeView_GetNextSibling(m_Hwnd, current)) != nullptr) {
				current = tmp;
			}

			if (bFillLocation)
				*hInsertAt = TVI_LAST;
		}
		// Search for Nth child.
		else {
			//i = 1;
			//
			//while ((current = TreeView_GetNextSibling(m_Hwnd, current)) != nullptr) {
			//	if (bFillLocation)
			//		*hInsertAt = current;
			//
			//	i++;
			//
			//	// Found the branch we want
			//	if (i == dir)
			//		break;
			//}
			// this version allows using a path thats `last item + 1`

			auto i = 1;                             // iterator for counting the Nth directory
			HTREEITEM tmp;

			while ((tmp = TreeView_GetNextSibling(m_Hwnd, current)) != nullptr) {
				current = tmp;
				if (bFillLocation)
					*hInsertAt = current;
				if (++i == dir)
					break;
			}
			
			// using last item + 1 in a path only works wenn fill location is true, and we are at the last level
			//if ((bFillLocation) && (++i == dir) && (k == count))
			//	*hInsertAt = current;
			// else we need to be sure that the last item wasn't null
			//else 
			//	current = tmp;
		}

		// Couldnt find specified path.
		if (current == nullptr)
			return nullptr;

		// Inch forward on next item.
		foundSoFar = current;
	}

	return foundSoFar;
}
