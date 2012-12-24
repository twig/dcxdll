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

DcxTreeView::DcxTreeView( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles )
: DcxControl( ID, p_Dialog )
, m_iIconSize(16)
, m_colSelection(CLR_INVALID)
, m_hOldItemFont(NULL)
, m_hItemFont(NULL)
#ifdef DCX_USE_GDIPLUS
, m_pImage(NULL)
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

	this->m_Hwnd = CreateWindowEx(	
		ExStyles | WS_EX_CLIENTEDGE,
		DCX_TREEVIEWCLASS,
		NULL,
		WS_CHILD | Styles, 
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL), 
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw TEXT("Unable To Create Window");

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	SendMessage( this->m_Hwnd, CCM_SETVERSION, (WPARAM) 6, (LPARAM) 0 );

	if ( ExStyles & TVS_CHECKBOXES )
		this->addStyle( TVS_CHECKBOXES );

	this->m_ToolTipHWND = (HWND)TreeView_GetToolTips(this->m_Hwnd);
	if (styles.istok(TEXT("balloon")) && this->m_ToolTipHWND != NULL) {
		SetWindowLong(this->m_ToolTipHWND,GWL_STYLE,GetWindowStyle(this->m_ToolTipHWND) | TTS_BALLOON);
	}

#ifdef DCX_USE_WINSDK
	if (Dcx::VistaModule.isUseable()) {
		ExStyles = TreeView_GetExtendedStyle(this->m_Hwnd);
		parseTreeViewExStyles( styles, &ExStyles);
		TreeView_SetExtendedStyle(this->m_Hwnd, ExStyles, ExStyles);
	}
#endif

	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );

	DragAcceptFiles(this->m_Hwnd, TRUE);
	if (this->isExStyle(WS_EX_TRANSPARENT))
		this->m_bTransparent = true;
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
  TreeView_DeleteAllItems( this->m_Hwnd );

  ImageList_Destroy( this->getImageList( TVSIL_NORMAL ) );
  ImageList_Destroy( this->getImageList( TVSIL_STATE ) );

	PreloadData();

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	*Styles |= TVS_INFOTIP;

	for (TString tsStyle(styles.getfirsttok( 1 )); tsStyle != TEXT(""); tsStyle = styles.getnexttok( ))
	{
		if ( tsStyle == TEXT("haslines") ) 
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
	}
	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

#ifdef DCX_USE_WINSDK
void DcxTreeView::parseTreeViewExStyles( const TString &styles, LONG * ExStyles ) {

	// Vista+ ONLY!
	for (TString tsStyle(styles.getfirsttok( 1 )); tsStyle != TEXT(""); tsStyle = styles.getnexttok( ))
	{
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
	}
}
#endif

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxTreeView::parseInfoRequest( const TString &input, TCHAR *szReturnValue) const
{
	const UINT numtok = input.numtok();
	const TString prop(input.getfirsttok( 3 ));

	// [NAME] [ID] [PROP] [PATH]
	if (prop == TEXT("text") && numtok > 3) {
		const TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(TEXT("text"), NULL, TEXT("Unable to parse path: %s"), path.to_chr());
			return;
		}

		this->getItemText(&item, szReturnValue, MIRC_BUFFER_SIZE_CCH);
		return;
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == TEXT("icon") && numtok > 3) {
		const TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(TEXT("icon"), NULL, TEXT("Unable to parse path: %s"), path.to_chr());
			return;
		}

		TVITEMEX tvi; 
		tvi.hItem = item;
		tvi.mask = TVIF_IMAGE | TVIF_HANDLE;

		TreeView_GetItem(this->m_Hwnd, &tvi);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), (tvi.iImage > 10000 ? -2 : tvi.iImage) +1);
		return;
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == TEXT("tooltip") && numtok > 3) {
		const TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(TEXT("tooltip"), NULL, TEXT("Unable to parse path: %s"), path.to_chr());
			return;
		}

		TVITEMEX tvi;
		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;

		TreeView_GetItem(this->m_Hwnd, &tvi);
		LPDCXTVITEM lpdcxtvi = (LPDCXTVITEM) tvi.lParam;

		if (lpdcxtvi != NULL)
			lstrcpyn(szReturnValue, lpdcxtvi->tsTipText.to_chr(), MIRC_BUFFER_SIZE_CCH);

		return;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("seltext")) {
		HTREEITEM item = TreeView_GetSelection(this->m_Hwnd);

		this->getItemText(&item, szReturnValue, MIRC_BUFFER_SIZE_CCH);
		return;
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("selpath") ) {
		HTREEITEM hItem = TreeView_GetSelection(this->m_Hwnd);
		const TString path(this->getPathFromItem(&hItem));

		lstrcpyn(szReturnValue, path.to_chr(), MIRC_BUFFER_SIZE_CCH);
		return;
	}
	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N] [SUBPATH]
	else if (prop == TEXT("find") && numtok > 5) {
		const TString matchtext(input.getfirsttok(2, TSTAB).trim());
		const TString params(input.getnexttok( TSTAB ).trim());		// tok 3

		if (matchtext.len() < 1) {
			this->showErrorEx(TEXT("find"), NULL, TEXT("No matchtext specified."));
			return;
		}

		UINT searchType;
		const TString searchMode(params.getfirsttok( 1 ));
		HTREEITEM startingPoint = TVI_ROOT;
		HTREEITEM result;

		if (searchMode == TEXT('R'))
			searchType = TVSEARCH_R;
		else if (searchMode == TEXT('W'))
			searchType = TVSEARCH_W;
		else
			searchType = TVSEARCH_E;

		const int n = params.getnexttok( ).to_int();	// tok 2
		int matchCount = 0;

		if (params.numtok() > 2) {
			const TString path(params.gettok(3, -1));

			startingPoint = this->parsePath(&path);

			if (startingPoint == NULL) {
				this->showErrorEx(TEXT("find"), NULL, TEXT("Unable to parse path: %s"), path.to_chr());
				return;
			}
		}

		if (this->findItemText(&startingPoint, &result, &matchtext, n, &matchCount, searchType)) {
			const TString path(this->getPathFromItem(&result));
			lstrcpyn(szReturnValue, path.to_chr(), MIRC_BUFFER_SIZE_CCH);
		}
		else if (n == 0)
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), matchCount);

		return;
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == TEXT("state") && numtok > 3) {
		const TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(TEXT("state"), NULL, TEXT("Unable to parse path: %s"), path.to_chr());
			return;
		}

		if (this->isStyle(TVS_CHECKBOXES)) {
			const int state = TreeView_GetCheckState(this->m_Hwnd, item);

			if (state == 1)
				lstrcpyn(szReturnValue, TEXT("2"), MIRC_BUFFER_SIZE_CCH);
			else if (state == 0)
				lstrcpyn(szReturnValue, TEXT("1"), MIRC_BUFFER_SIZE_CCH);
			else
				lstrcpyn(szReturnValue, TEXT("0"), MIRC_BUFFER_SIZE_CCH);
			return;
		}
		else {
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), TreeView_GetItemState(this->m_Hwnd, item, TVIS_STATEIMAGEMASK));
			return;
		}
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == TEXT("num") && numtok > 3) {
		const TString path(input.gettok(4, -1).trim());
		HTREEITEM item;

		if (path == TEXT("root")) {
			item = TVI_ROOT;
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getChildCount(&item));
			return;
		}

		item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(TEXT("num"), NULL, TEXT("Unable to parse path: %s"), path.to_chr());
			return;
		}

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getChildCount(&item));
		return;
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == TEXT("expand") && numtok > 3) {
		const TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(TEXT("expand"), NULL, TEXT("Unable to parse path: %s"), path.to_chr());
			return;
		}

		if (TreeView_GetItemState(this->m_Hwnd, item, TVIS_EXPANDED) & TVIS_EXPANDED)
			lstrcpyn(szReturnValue, TEXT("1"), MIRC_BUFFER_SIZE_CCH);
		else
			lstrcpyn(szReturnValue, TEXT("0"), MIRC_BUFFER_SIZE_CCH);

		return;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("mouseitem")) {
		TVHITTESTINFO tvh;
		GetCursorPos(&tvh.pt);
		MapWindowPoints(NULL, this->m_Hwnd, &tvh.pt, 1 );
		TreeView_HitTest( this->m_Hwnd, &tvh );

		if ( tvh.flags & TVHT_ONITEM )
			lstrcpyn(szReturnValue, this->getPathFromItem(&tvh.hItem).to_chr(), MIRC_BUFFER_SIZE_CCH);
		else
			lstrcpyn(szReturnValue, TEXT("0"), MIRC_BUFFER_SIZE_CCH);

		return;
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == TEXT("markeditem") && numtok > 3) {
		const TString path(input.gettok(4, -1).trim());
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(TEXT("markeditem"), NULL, TEXT("Invalid Path: %s"), path.to_chr());
			return;
		}

		TVITEMEX tvi;

		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;

		if (!TreeView_GetItem(this->m_Hwnd, &tvi)) {
			this->showErrorEx(NULL, TEXT("-A"), TEXT("Unable to retrieve item: %s"), path.to_chr());
			return;
		}

		LPDCXTVITEM lpdcxtvitem = (LPDCXTVITEM) tvi.lParam;
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%s"), lpdcxtvitem->tsMark.to_chr());
		lstrcpyn(szReturnValue, lpdcxtvitem->tsMark.to_chr(), MIRC_BUFFER_SIZE_CCH);
		return;
	}
	else if (this->parseGlobalInfoRequest(input, szReturnValue))
		return;

	szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const UINT numtok = input.numtok( );

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')]) {
		// If the window style for a tree-view control contains TVS_SCROLL and all items are deleted,
		// new items are not displayed until the window styles are reset.
		// The following code shows one way to ensure that items are always displayed.
		const DWORD styles = GetWindowLong(this->m_Hwnd, GWL_STYLE);
		TreeView_DeleteAllItems(this->m_Hwnd);
		SetWindowLong(this->m_Hwnd, GWL_STYLE, styles);
	}

	// xdid -a [NAME] [ID] [SWITCH] N N N ... N[TAB][+FLAGS] [#ICON] [#SICON] [#OVERLAY] [#STATE] [#INTEGRAL] [COLOR] [BKGCOLOR] Text[TAB]Tooltip Text
	if (flags[TEXT('a')]) {
		const int n = input.numtok(TSTAB);

		if (n > 1) {
			const TString path(input.getfirsttok(1, TSTAB).gettok(4, -1).trim());
			const TString data(input.getnexttok( TSTAB).trim());	// tok 2
			TString tooltip;

			if (n > 2)
				tooltip = input.gettok(3, -1, TSTAB).trim();

			if (data.numtok( ) > 8)
				this->insertItem(&path, &data, &tooltip);
		}
	}
	// xdid -A [NAME] [ID] [SWITCH] N N N ... N[TAB][+FLAGS] [INFO]
	else if (flags[TEXT('A')]) {

		if (input.numtok(TSTAB) < 2) {
			this->showErrorEx(NULL, TEXT("-A"), TEXT("Insufficient parameters"));
			return;
		}

		TString path(input.getfirsttok(1, TSTAB).trim());
		const TString data(input.getnexttok( TSTAB).trim());	// tok 2
		HTREEITEM item;

		if (path.numtok() < 4) {
			this->showErrorEx(NULL, TEXT("-A"), TEXT("Insufficient parameters (path)"));
			return;
		}

		path = path.gettok(4, -1);

		if (data.numtok() < 2) {
			this->showErrorEx(NULL, TEXT("-A"), TEXT("Insufficient parameters (args)"));
			return;
		}

		item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, TEXT("-A"), TEXT("Invalid Path: %s"), path.to_chr());
			return;
		}

		const TString flag(data.gettok(1));
		const TString info(data.gettok(2, -1));

		if (flag.find(TEXT('M'), 1) > 0) {
			TVITEMEX tvi; 

			tvi.hItem = item;
			tvi.mask = TVIF_HANDLE | TVIF_PARAM ; 

			if (!TreeView_GetItem(this->m_Hwnd, &tvi)) {
				this->showErrorEx(NULL, TEXT("-A"), TEXT("Unable to retrieve item: %s"), path.to_chr());
				return;
			}

			LPDCXTVITEM lpdcxtvitem = (LPDCXTVITEM) tvi.lParam;

			lpdcxtvitem->tsMark = info;
			TreeView_SetItem(this->m_Hwnd, &tvi);
		}
		else {
			this->showErrorEx(NULL, TEXT("-A"), TEXT("Unknown flags %s"), flag.to_chr());
			return;
		}
	}
	// xdid -B [NAME] [ID] [SWITCH] N N N
	else if (flags[TEXT('B')] && numtok > 3) {
		const TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);
		
		if (item == NULL) {
			this->showErrorEx(NULL, TEXT("-B"), TEXT("Invalid Path: %s"), path.to_chr());
			return;
		}

		TreeView_EnsureVisible(this->m_Hwnd, item); // make sure selected item is visible.
		TreeView_EditLabel(this->m_Hwnd, item);
	}
	// xdid -c [NAME] [ID] [SWITCH] N N N
	else if (flags[TEXT('c')] && numtok > 3) {
		const TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, TEXT("-c"), TEXT("Invalid Path: %s"), path.to_chr());
			return;
		}

		TreeView_EnsureVisible(this->m_Hwnd, item); // make sure selected item is visible.
		TreeView_SelectItem(this->m_Hwnd, item);
	}
	// xdid -d [NAME] [ID] [SWITCH] N N N
	else if (flags[TEXT('d')] && numtok > 3) {
		const TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, TEXT("-d"), TEXT("Invalid Path: %s"), path.to_chr());
			return;
		}

		TreeView_DeleteItem(this->m_Hwnd, item);
	}
	// xdid -g [NAME] [ID] [SWITCH] [HEIGHT]
	else if (flags[TEXT('g')] && numtok > 3) {
		const int iHeight = input.getnexttok( ).to_int();	// tok 4

		if (iHeight > -2)
			TreeView_SetItemHeight(this->m_Hwnd, iHeight);
	}
	// xdid -i [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if ( flags[TEXT('i')] && numtok > 4 ) {
		const UINT iFlags = this->parseColorFlags(input.getnexttok( ));	// tok 4

		const COLORREF clr = (COLORREF) input.getnexttok( ).to_num( );	// tok 5

		if (iFlags & TVCOLOR_B)
			TreeView_SetBkColor( this->m_Hwnd, clr );

		if (iFlags & TVCOLOR_L)
			TreeView_SetLineColor( this->m_Hwnd, clr );

		if (iFlags & TVCOLOR_T)
			TreeView_SetTextColor( this->m_Hwnd, clr );

		if (iFlags & TVCOLOR_S)
			this->m_colSelection = clr;

		this->redrawWindow();
	}
	// xdid -j [NAME] [ID] [SWITCH] [+FLAGS] [N N N] [TAB] [ICON] [SICON] (OVERLAY)
	else if (flags[TEXT('j')] && numtok > 5) {
		HTREEITEM item;
		const TString path(input.getfirsttok(1, TSTAB).gettok(4, -1).trim());

		// Invalid parameters, missing icons segment.
		if (input.numtok(TSTAB) < 2) {
			this->showErrorEx(NULL, TEXT("-j"), TEXT("Invalid parameters."));
			return;
		}

		const TString icons(input.getnexttok( TSTAB).trim());	// tok 2

		// Invalid parameters, missing icon args.
		if (icons.numtok() < 2) {
			this->showErrorEx(NULL, TEXT("-j"), TEXT("Invalid parameters."));
			return;
		}

		item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, TEXT("-j"), TEXT("Invalid Path: %s"), path.to_chr());
			return;
		}

		int nIcon = icons.getfirsttok( 1 ).to_int() -1;
		int sIcon = icons.getnexttok( ).to_int() -1;	// tok 2
		TVITEMEX tvi;

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
			const int oIcon = icons.getnexttok( ).to_int();	// tok 3

			// overlay is 1-based index
			if (oIcon > -1)
				TreeView_SetItemState(this->m_Hwnd, tvi.hItem, INDEXTOOVERLAYMASK(oIcon), TVIS_OVERLAYMASK);

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

		TreeView_SetItem(this->m_Hwnd, &tvi);
	}
	// xdid -k [NAME] [ID] [SWITCH] [STATE] N N N
	else if (flags[TEXT('k')] && numtok > 4) {
		const TString path(input.gettok(5, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, TEXT("-k"), TEXT("Invalid Path: %s"), path.to_chr());
			return;
		}

		const UINT state = input.getnexttok( ).to_int();	// tok 4

		TreeView_SetItemState(this->m_Hwnd, item, INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK);
	}
	// xdid -l [NAME] [ID] [SWITCH] [SIZE]
	else if ( flags[TEXT('l')] && numtok > 3 ) {
		UINT size = input.getnexttok( ).to_int( );	// tok 4

		if ( size != 32 && size != 24 )
			size = 16;

		this->m_iIconSize = size;
	}
	// xdid -m [NAME] [ID] [SWITCH] N N N{TAB}N N N
	else if (flags[TEXT('m')] && numtok > 3 && input.numtok(TSTAB) > 1) {
		const TString pathFrom(input.getfirsttok(1, TSTAB).gettok(4, -1).trim());
		const TString pathTo(input.getnexttok( TSTAB).trim());	// tok 2

		HTREEITEM item = this->parsePath(&pathFrom);
		HTREEITEM hParentTo = TVI_ROOT;
		HTREEITEM hAfterTo = TVI_ROOT;
		HTREEITEM hNewItem;

		if (item == NULL) {
			this->showErrorEx(NULL, TEXT("-m"), TEXT("Unable to parse FROM path: %s"), pathFrom.to_chr());
			return;
		}

		if (this->parsePath(&pathTo, &hParentTo, &hAfterTo) == NULL) {
			this->showErrorEx(NULL, TEXT("-m"), TEXT("Unable to parse TO path: %s"), pathTo.to_chr());
			return;
		}

		// Check if we're moving parent into a child branch
		if ((hParentTo != TVI_FIRST) && (hParentTo != TVI_LAST)) {
			HTREEITEM tmp = hParentTo;

			// Trace back all the way to root
			while (tmp != NULL && tmp != TVI_ROOT) {
				if (tmp == item) {
					this->showErrorEx(NULL, TEXT("-m"), TEXT("Cannot move parent node (%s) to child node (%s)."), pathFrom.to_chr(), pathTo.to_chr());
					return;
				}

				tmp = TreeView_GetParent(this->m_Hwnd, tmp);
			}
		}

		hNewItem = this->cloneItem(&item, &hParentTo, &hAfterTo);

		if (hNewItem != NULL) {
			this->copyAllItems(&item, &hNewItem);
			TreeView_DeleteItem(this->m_Hwnd, item);
		}
		else
			this->showError(NULL, TEXT("-m"), TEXT("Unable to move item."));
	}
	// xdid -n [NAME] [ID] [SWITCH] N N N{TAB}N N N
	else if (flags[TEXT('n')] && numtok > 3 && input.numtok(TSTAB) > 1) {
		const TString pathFrom(input.getfirsttok(1, TSTAB).gettok(4, -1).trim());
		const TString pathTo(input.getnexttok( TSTAB).trim());	// tok 2

		HTREEITEM item;
		HTREEITEM hParentTo = TVI_ROOT;
		HTREEITEM hAfterTo = TVI_ROOT;
		HTREEITEM hNewItem;

		item = this->parsePath(&pathFrom);

		// Check source item.
		if (item == NULL) {
			this->showErrorEx(NULL, TEXT("-n"), TEXT("Unable to parse FROM path: %s"), pathFrom.to_chr());
			return;
		}

		// Check destination.
		if (this->parsePath(&pathTo, &hParentTo, &hAfterTo) == NULL) {
			this->showErrorEx(NULL, TEXT("-n"), TEXT("Unable to parse TO path: %s"), pathFrom.to_chr());
			return;
		}

		// Check if we're copying source into a child branch
		if ((hParentTo != TVI_FIRST) && (hParentTo != TVI_LAST) && (hParentTo != TVI_ROOT)) {
			HTREEITEM tmp = hParentTo;

			// Trace back all the way to root
			while (tmp != NULL) {
				if (tmp == item) {
					this->showErrorEx(NULL, TEXT("-n"), TEXT("Cannot copy parent node (%s) to child node (%s)."), pathFrom.to_chr(), pathTo.to_chr());
					return;
				}

				tmp = TreeView_GetParent(this->m_Hwnd, tmp);
			}
		}

		hNewItem = this->cloneItem(&item, &hParentTo, &hAfterTo);

		if (hNewItem != NULL)
			this->copyAllItems(&item, &hNewItem);
		else
			this->showErrorEx(NULL, TEXT("-n"), TEXT("Unable to copy items."));
	}
	// xdid -o [NAME] [ID] [SWITCH] N N N [TAB] (Tooltip Text)
	else if (flags[TEXT('o')] && numtok > 3) {
		const TString path(input.getfirsttok(1, TSTAB).gettok(4, -1).trim());
		TString tiptext;
		
		if (input.numtok(TSTAB) > 1)
			tiptext = input.getnexttok( TSTAB).trim();	// tok 2

		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, TEXT("-o"), TEXT("Unable to parse path: %s"), path.to_chr());
			return;
		}

		TVITEMEX tvi; 

		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM ; 

		if (TreeView_GetItem(this->m_Hwnd, &tvi)) {
			LPDCXTVITEM lpdcxtvitem = (LPDCXTVITEM) tvi.lParam;

			if (lpdcxtvitem != NULL)
				lpdcxtvitem->tsTipText = tiptext;
		}
	}
	// xdid -Q [NAME] [ID] [SWITCH] [+FLAGS] [COLOR] N N N
	else if ( flags[TEXT('Q')] && numtok > 5 ) {
		const int iFlags = this->parseItemFlags(input.getnexttok( ));		// tok 4
		const COLORREF clrText = (COLORREF) input.getnexttok( ).to_num();	// tok 5
		const TString path(input.gettok(6, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, TEXT("-Q"), TEXT("Unable to parse path: %s"), path.to_chr());
			return;
		}

		TVITEMEX tvi; 

		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM ; 

		if (!TreeView_GetItem(this->m_Hwnd, &tvi)) {
			this->showErrorEx(NULL, TEXT("-Q"), TEXT("Unable to retrieve item: %s"), path.to_chr());
			return;
		}

		LPDCXTVITEM lpdcxtvitem = (LPDCXTVITEM) tvi.lParam;

		if (lpdcxtvitem == NULL ) {
			this->showErrorEx(NULL, TEXT("-Q"), TEXT("Unable to retrieve tag: %s"), path.to_chr());
			return;
		}

		if (iFlags & TVIS_UNDERLINE)
			lpdcxtvitem->bUline = TRUE;
		else
			lpdcxtvitem->bUline = FALSE;

		if (iFlags & TVIS_BOLD)
			lpdcxtvitem->bBold = TRUE;
		else
			lpdcxtvitem->bBold = FALSE;

		if (iFlags & TVIS_ITALIC)
			lpdcxtvitem->bItalic = TRUE;
		else
			lpdcxtvitem->bItalic = FALSE;

		if (iFlags & TVIS_COLOR)
			lpdcxtvitem->clrText = clrText;
		else
			lpdcxtvitem->clrText = CLR_INVALID;

		this->redrawWindow();
	}
	// This is to avoid an invalid error message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
	}
	// xdid -t [NAME] [ID] [SWITCH] [+FLAGS] N N N
	else if (flags[TEXT('t')] && numtok > 4) {
		const UINT iFlags = this->parseToggleFlags(input.getnexttok( ));	// tok 4
		const TString path(input.gettok(5, -1));

		if (path == TEXT("root")) {
			HTREEITEM hStart = TVI_ROOT;

			if (iFlags & TVIE_EXPALL)
				this->expandAllItems(&hStart, TVE_EXPAND);
			else if (iFlags & TVIE_COLALL)
				this->expandAllItems(&hStart, TVE_COLLAPSE);

			return;
		}

		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, TEXT("-t"), TEXT("Unable to parse path: %s"), path.to_chr());
			return;
		}

		if (iFlags & TVIE_EXP)
			TreeView_Expand(this->m_Hwnd, item, TVE_EXPAND);
		else if (iFlags & TVIE_EXPPART)
			TreeView_Expand(this->m_Hwnd, item, TVE_EXPAND | TVE_EXPANDPARTIAL);
		else if (iFlags & TVIE_COL)
			TreeView_Expand(this->m_Hwnd, item, TVE_COLLAPSE);
		else if (iFlags & TVIE_COLRES)
			TreeView_Expand(this->m_Hwnd, item, TVE_COLLAPSE | TVE_COLLAPSERESET);
		else if (iFlags & TVIE_TOGGLE)
			TreeView_Expand(this->m_Hwnd, item, TVE_TOGGLE);
	}
	// xdid -u [NAME] [ID] [SWITCH]
	else if (flags[TEXT('u')]) {
		TreeView_SelectItem(this->m_Hwnd, NULL);
	}
	// xdid -v [NAME] [ID] [SWITCH] N N N [TAB] (Item Text)
	else if (flags[TEXT('v')] && numtok > 3) {
		const TString path(input.getfirsttok(1, TSTAB).gettok(4, -1).trim());
		TString itemtext;

		if (input.numtok(TSTAB) > 1)
			itemtext = input.getnexttok( TSTAB).trim();	// tok 2

		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, TEXT("-v"), TEXT("Unable to parse path: %s"), path.to_chr());
			return;
		}

		TVITEMEX tvi;

		tvi.mask = TVIF_TEXT | TVIF_HANDLE;
		tvi.hItem = item;
		tvi.pszText = itemtext.to_chr();
		TreeView_SetItem(this->m_Hwnd, &tvi);
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')] && numtok > 5) {
		const TString tsFlags(input.getnexttok( ));	// tok 4
		const UINT iFlags = this->parseIconFlagOptions(tsFlags);

		HICON icon = NULL;

		const int index = input.getnexttok( ).to_int();	// tok 5
		TString filename(input.gettok(6, -1));
		const bool bLarge = (this->m_iIconSize > 16);

		if (index >= 0) {
			icon = dcxLoadIcon(index, filename, bLarge, tsFlags);

			if (icon == NULL) {
				this->showError(NULL, TEXT("-w"), TEXT("Unable to load icon"));
				return;
			}
		}
		if (iFlags & TVIT_NORMAL) {
			HIMAGELIST himl = this->getImageList(TVSIL_NORMAL);
			if (himl == NULL) {
				himl = this->createImageList();

				if (himl != NULL)
					this->setImageList(himl, TVSIL_NORMAL);
			}

			if (index < 0) {
				if (!AddFileIcons(himl, filename, bLarge, -1)) {
					this->showErrorEx(NULL, TEXT("-w"), TEXT("Unable to Add %s's Icons"), filename.to_chr());
					return;
				}
			}
			else {
				const int i = ImageList_AddIcon(himl, icon);

				if (tsFlags.find(TEXT('o'),0)) {
					// overlay image
					const int io = tsFlags.find(TEXT('o'),1) +1;
					int o = tsFlags.mid(io, (tsFlags.len() - io)).to_int();

					if (o < 1 || o > 15) {
						this->showError(NULL, TEXT("-w"), TEXT("Overlay index out of range (1 -> 15)"));
						o = 0;
					}

					if (o > 0)
						ImageList_SetOverlayImage(himl, i, o);
				}
			}
		}

		if (iFlags & TVIT_STATE) {
			HIMAGELIST himl = this->getImageList(TVSIL_STATE);
			if (himl == NULL) {
				himl = this->createImageList();

				if (himl != NULL)
					this->setImageList(himl, TVSIL_STATE);
			}

			if (index < 0) {
				if (!AddFileIcons(himl, filename, bLarge, -1)) {
					this->showErrorEx(NULL, TEXT("-w"), TEXT("Unable to Add %s's Icons"), filename.to_chr());
					return;
				}
			}
			else
				ImageList_AddIcon(himl, icon);
		}
		DestroyIcon(icon);
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if ( flags[TEXT('y')] && numtok > 3 ) {
		const UINT iFlags = this->parseIconFlagOptions( input.getnexttok( ) );	// tok 4

		HIMAGELIST himl;

		if ( iFlags & TVIT_NORMAL ) {
			himl = this->getImageList( TVSIL_NORMAL );
			if ( himl != NULL ) {
				ImageList_Destroy( himl );
				this->setImageList( NULL, TVSIL_NORMAL );
			}
		}

		if ( iFlags & TVIT_STATE ) {
			himl = this->getImageList( TVSIL_STATE );
			if ( himl != NULL ) {

				ImageList_Destroy( himl );
				this->setImageList( NULL, TVSIL_STATE );
			}
		}
	}
	// xdid -z [NAME] [ID] [SWITCH] [+FLAGS] N N N [TAB] [ALIAS]
	else if (flags[TEXT('z')] && numtok > 4) {
		const TString path(input.gettok(1, TSTAB).gettok(5, -1).trim());
		DCXTVSORT dtvs;
		TVSORTCB tvs;

		dtvs.pthis = NULL;
		dtvs.iSortFlags = this->parseSortFlags(input.gettok(4));
		dtvs.pthis = this;

		if (input.numtok(TSTAB) > 1)
			dtvs.tsCustomAlias = input.gettok(2, TSTAB).trim();

		ZeroMemory( &tvs, sizeof(TVSORTCB) );
		tvs.lpfnCompare = DcxTreeView::sortItemsEx;
		tvs.lParam = (LPARAM) &dtvs;

		if (path == TEXT("root"))
			tvs.hParent = TVI_ROOT;
		else {

			HTREEITEM item = this->parsePath(&path);

			if (item == NULL) {
				this->showErrorEx(NULL, TEXT("-z"), TEXT("Unable to parse path: %s"), path.to_chr());
				return;
			}

			tvs.hParent = item;
		}
		if (dtvs.iSortFlags & TVSS_ALL)
			TreeView_SortChildrenCB(this->m_Hwnd, &tvs, TRUE); // NB: doesnt recurse!! This is a OS problem.
		else if (dtvs.iSortFlags & TVSS_SINGLE)
			TreeView_SortChildrenCB(this->m_Hwnd, &tvs, FALSE);
	}
	// xdid -G [NAME] [ID] [SWITCH] [+FLAGS] [X] [Y] (FILENAME)
	else if (flags[TEXT('G')] && numtok > 6) {
		const TString flag(input.getnexttok( ));			// tok 4
		this->m_iXOffset = input.getnexttok( ).to_int();	// tok 5
		this->m_iYOffset = input.getnexttok( ).to_int();	// tok 6
		TString filename(input.gettok( 7, -1));
		if (!this->m_bTransparent)
			this->removeExStyle(WS_EX_TRANSPARENT);
		if (filename != TEXT("none")) {
			if (!Dcx::GDIModule.isUseable())
				this->showError(NULL,TEXT("-G"),TEXT("GDI+ Not Supported On This Machine"));
			else if (!LoadGDIPlusImage(flag, filename))
				this->showErrorEx(NULL,TEXT("-G"),TEXT("Unable to load Image: %s"), filename.to_chr());
			else if (!this->m_bTransparent)
				this->setExStyle(WS_EX_TRANSPARENT);
		}
		this->redrawWindow();
	}
	// xdid -S [NAME] [ID] [SWITCH] [N (N...)][TAB][+FLAGS] [NAME] [FILENAME]
	else if (flags[TEXT('S')] && numtok > 5) {
		if (input.numtok(TSTAB) != 2) {
			this->showError(NULL, TEXT("-S"), TEXT("Invalid Command Syntax."));
			return;
		}

		const TString fileData(input.gettok(2, TSTAB));

		if (fileData.numtok() < 3) {
			this->showError(NULL, TEXT("-S"), TEXT("Invalid Command Syntax."));
			return;
		}

		const TString path(input.gettok(1, TSTAB).gettok(4, -1).trim());
		//TString flags(fileData.gettok(1));
		const TString name(fileData.gettok(2).trim());
		TString filename(fileData.gettok(3, -1).trim());

		if (name.len() < 1) {
			this->showError(NULL, TEXT("-S"), TEXT("Invalid dataset"));
			return;
		}
		if (path.len() < 1) {
			this->showError(NULL, TEXT("-S"), TEXT("Invalid path"));
			return;
		}

		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, TEXT("-S"), TEXT("Unable to parse path: %s"), path.to_chr());
			return;
		}

		if (!this->xmlSaveTree(item, name, filename))
			this->showErrorEx(NULL, TEXT("-S"), TEXT("Unable To Save Data To: <%s> %s"), name.to_chr(), filename.to_chr());
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxTreeView::getImageList( const int type ) {

  return (HIMAGELIST) TreeView_GetImageList( this->m_Hwnd, type );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::setImageList( HIMAGELIST himl, const int type ) {
	HIMAGELIST o = TreeView_SetImageList( this->m_Hwnd, himl, type );
	if (o != NULL && o != himl) // don't destroy if NULL or the same list as just added.
		ImageList_Destroy(o);
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxTreeView::createImageList( ) {

  return ImageList_Create( this->m_iIconSize, this->m_iIconSize, ILC_COLOR32|ILC_MASK, 1, 0 );
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
	HTREEITEM hItem = NULL;

	TVITEMEX tvi;
	TVINSERTSTRUCT tvins;

	ZeroMemory(&tvins, sizeof(TVINSERTSTRUCT));
	ZeroMemory(&tvi, sizeof(TVITEMEX));

	//tvi.mask = TVIF_TEXT | TVIF_STATE | TVIF_INTEGRAL | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvi.mask = TVIF_STATE | TVIF_PARAM;

	int iFlags			= this->parseItemFlags(data->gettok( 1 ));
	int icon			= data->gettok( 2 ).to_int() -1;
	int sicon			= data->gettok( 3 ).to_int() -1;
	int overlay			= data->gettok( 4 ).to_int();
	int state			= data->gettok( 5 ).to_int();
	int integral		= data->gettok( 6 ).to_int() +1;
	COLORREF clrText	= (COLORREF) data->gettok( 7 ).to_num();
	COLORREF clrBkg		= (COLORREF) data->gettok( 8 ).to_num();

	// text
	TString itemtext(data->gettok(9, -1));

	// path
	if (this->parsePath(path, &hParent, &hAfter) == NULL)
		return;

	if (iFlags & TVIS_XML) {
		TString filename(itemtext.gettok( 2, -1));
		this->xmlLoadTree(hAfter, hParent, itemtext.gettok( 1 ), filename);
		return;
	}

	// parse DCX parameters
	LPDCXTVITEM lpmytvi = new DCXTVITEM;

	if (lpmytvi == NULL) {
		this->showError(NULL, NULL, TEXT("Unable to Allocate Memory"));
		return;
	}

	lpmytvi->tsTipText = *Tooltip;

	if (iFlags & TVIS_UNDERLINE)
		lpmytvi->bUline = TRUE;
	else
		lpmytvi->bUline = FALSE;

	if (iFlags & TVIS_BOLD)
		lpmytvi->bBold = TRUE;
	else
		lpmytvi->bBold = FALSE;

	if (iFlags & TVIS_ITALIC)
		lpmytvi->bItalic = TRUE;
	else
		lpmytvi->bItalic = FALSE;

	if (iFlags & TVIS_COLOR)
		lpmytvi->clrText = clrText;
	else
		lpmytvi->clrText = CLR_INVALID;

	if (iFlags & TVIS_BKG)
		lpmytvi->clrBkg = clrBkg;
	else
		lpmytvi->clrBkg = CLR_INVALID;

	{
		TString tsRes;
		if ((iFlags & TVIS_HASHITEM) && (itemtext.numtok() == 2)) {
			Dcx::mIRC.tsEvalex(tsRes, TEXT("$hget(%s,%s)"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
			itemtext = tsRes;
		}
		else if ((iFlags & TVIS_HASHNUMBER) && (itemtext.numtok() == 2)) {
			Dcx::mIRC.tsEvalex(tsRes,  TEXT("$hget(%s,%s).data"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
			itemtext = tsRes;
		}
	}

	if (itemtext.len() > 0) {
		tvi.pszText = itemtext.to_chr();
		tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);
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
	tvi.lParam = (LPARAM) lpmytvi;

	tvins.itemex = tvi;
	tvins.hInsertAfter = hAfter;
	tvins.hParent = hParent;

	hItem = TreeView_InsertItem(this->m_Hwnd, &tvins);
	lpmytvi->hHandle = hItem;

	if (state > -1 && state < 5) // zero is no state image.
		TreeView_SetItemState(this->m_Hwnd, hItem, INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK);

	// overlay is 1-based index
	if (overlay > 0 && overlay < 16)
		TreeView_SetItemState(this->m_Hwnd, hItem, INDEXTOOVERLAYMASK(overlay), TVIS_OVERLAYMASK);

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

	LPDCXTVSORT ptvsort = (LPDCXTVSORT) lParamSort;
	TCHAR itemtext1[MIRC_BUFFER_SIZE_CCH];
	TCHAR itemtext2[MIRC_BUFFER_SIZE_CCH];

	LPDCXTVITEM lptvi1 = (LPDCXTVITEM) lParam1;
	LPDCXTVITEM lptvi2 = (LPDCXTVITEM) lParam2;

	ptvsort->pthis->getItemText( &lptvi1->hHandle, itemtext1, MIRC_BUFFER_SIZE_CCH );
	ptvsort->pthis->getItemText( &lptvi2->hHandle, itemtext2, MIRC_BUFFER_SIZE_CCH );

	// CUSTOM Sort
	if ( ptvsort->iSortFlags & TVSS_CUSTOM ) {

		TCHAR res[20];
		Dcx::mIRC.evalex( res, 20, TEXT("$%s(%s,%s)"), ptvsort->tsCustomAlias.to_chr( ), itemtext1, itemtext2 );

		int ires = dcx_atoi(res);

		if (ires < -1)
			ires = -1;
		else if (ires > 1)
			ires = 1;

		if ( ptvsort->iSortFlags & TVSS_DESC )
			return ires;
		else {

			if (ires == -1)
				return 1;
			else if (ires == 1)
				return -1;
		}
	}
	// NUMERIC Sort
	else if ( ptvsort->iSortFlags & TVSS_NUMERIC ) {

		const int i1 = dcx_atoi( itemtext1 );
		const int i2 = dcx_atoi( itemtext2 );

		if ( ptvsort->iSortFlags & TVSS_DESC ) {

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

		if ( ptvsort->iSortFlags & TVSS_DESC ) {
			if ( ptvsort->iSortFlags & TVSS_CASE )
				return -lstrcmp( itemtext1, itemtext2 );
			else
				return -lstrcmpi( itemtext1, itemtext2 );
		}
		else {
			if ( ptvsort->iSortFlags & TVSS_CASE )
				return lstrcmp( itemtext1, itemtext2 );
			else
				return lstrcmpi( itemtext1, itemtext2 );
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
	HTREEITEM current;
	TString result;
	VectorOfInts vec;

	// Loop until we reach the topmost node of the tree.
	do {
		int i = 1;
		current = parent;

		// Count each previous node before the node containing our item.
		while ((current = TreeView_GetPrevSibling(this->m_Hwnd, current)) != NULL) {
			i++;
		}

		vec.push_back(i);
	} while ((parent = TreeView_GetParent(this->m_Hwnd, parent)) != NULL);

	// Construct the string containing the path backwards, as we traced it backwards.
	VectorOfInts::reverse_iterator itStart = vec.rbegin( );
	VectorOfInts::reverse_iterator itEnd   = vec.rend( );

	while (itStart != itEnd) {
		if (result == TEXT(""))
			result.tsprintf(TEXT("%d"), *itStart);
		else
			result.tsprintf(TEXT("%s %d"), result.to_chr(), *itStart);

		itStart++;
	}

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

  if ( !TreeView_GetItem( this->m_Hwnd, &tvi ) )
    szBuffer[0] = 0;
}

/*
 * Returns the number of children a node has.
 */
int DcxTreeView::getChildCount(HTREEITEM *hParent) const {
	int i = 0;
	HTREEITEM hItem;

	hItem = TreeView_GetChild(this->m_Hwnd, *hParent);

	// Child found
	if (hItem != NULL)
		++i;
	// No children
	else
		return 0;

	// Iterate through sibling nodes until reaching the end
	while ((hItem = TreeView_GetNextSibling(this->m_Hwnd, hItem)) != NULL)
		++i;

	return i;
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxTreeView::matchItemText(HTREEITEM *hItem, const TString *search, const UINT SearchType) const {
	TCHAR itemtext[MIRC_BUFFER_SIZE_CCH];
	this->getItemText(hItem, itemtext, MIRC_BUFFER_SIZE_CCH);

	switch (SearchType) {
		case TVSEARCH_R:
			return isRegexMatch(itemtext, search->to_chr());
		case TVSEARCH_W:
			return TString(itemtext).iswm(search->to_chr());
		case TVSEARCH_E:
			return (!lstrcmp(search->to_chr(), itemtext));
	}

	return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */
BOOL DcxTreeView::findItemText(HTREEITEM *hStart, HTREEITEM *result, const TString *queryText, const int n, int *matchCount, const UINT searchType) const {
	HTREEITEM item;

	// Check if it should search child nodes
	if ((item = TreeView_GetChild(this->m_Hwnd, *hStart)) == NULL)
		return FALSE;

	do {
		if (this->matchItemText(&item, queryText, searchType))
			(*matchCount)++;

		if (n != 0 && *matchCount == n) {
			*result = item;
			return TRUE;
		}

		if (this->findItemText(&item, result, queryText, n, matchCount, searchType))
			return TRUE;

	} while ((item = TreeView_GetNextSibling(this->m_Hwnd, item)) != NULL);

	return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::expandAllItems( HTREEITEM * hStart, const UINT expandOption ) {

  HTREEITEM hCurrentItem;

  if ( ( hCurrentItem = TreeView_GetChild( this->m_Hwnd, *hStart) ) == NULL )
    return;

  do {

    this->expandAllItems( &hCurrentItem, expandOption );
    TreeView_Expand( this->m_Hwnd, hCurrentItem, expandOption );

  } while ( ( hCurrentItem = TreeView_GetNextSibling( this->m_Hwnd, hCurrentItem ) ) != NULL );
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

  if ( !TreeView_GetItem( this->m_Hwnd, &tvi ) )
    return NULL;

  TVINSERTSTRUCT tvin;

  LPDCXTVITEM lpdcxtvitem = (LPDCXTVITEM) tvi.lParam;
  LPDCXTVITEM lpdcxtvitem2 = new DCXTVITEM;

  *lpdcxtvitem2 = *lpdcxtvitem;

  tvi.hItem = 0;
  tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_INTEGRAL | TVIF_PARAM;
  tvi.lParam = (LPARAM) lpdcxtvitem2;

  tvin.hParent = *hParentTo;
  tvin.hInsertAfter = *hAfterTo;
  tvin.itemex = tvi;

  return TreeView_InsertItem( this->m_Hwnd, &tvin );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::copyAllItems( HTREEITEM *hItem, HTREEITEM * hParentTo ) {

  HTREEITEM hCurrentItem, hNewItem;
  HTREEITEM hAfterTo = TVI_LAST;

  if ( ( hCurrentItem = TreeView_GetChild( this->m_Hwnd, *hItem ) ) == NULL )
    return;

  do {

    hNewItem = this->cloneItem( &hCurrentItem, hParentTo, &hAfterTo );
    if ( hNewItem != NULL )
      this->copyAllItems( &hCurrentItem, &hNewItem );

  } while ( ( hCurrentItem = TreeView_GetNextSibling( this->m_Hwnd, hCurrentItem ) ) != NULL );
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
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			switch (hdr->code) {
			case NM_CLICK:
				{
				//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/treeview/reflist.asp
					TVHITTESTINFO tvh;
					GetCursorPos(&tvh.pt);
					MapWindowPoints(NULL, this->m_Hwnd, &tvh.pt, 1);
					TreeView_HitTest(this->m_Hwnd, &tvh);

					if (tvh.flags & TVHT_ONITEMBUTTON) {
						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
							this->execAliasEx(TEXT("%s,%d,%s"), TEXT("buttonclick"), this->getUserID(), this->getPathFromItem(&tvh.hItem).to_chr());
					}
					//&& this->isStyle( TVS_CHECKBOXES )
					else if ((tvh.flags & TVHT_ONITEMSTATEICON)) {
						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
							const TString path(this->getPathFromItem(&tvh.hItem));
							if (this->isStyle(TVS_CHECKBOXES))
								this->execAliasEx(TEXT("%s,%d,%d,%s"), TEXT("stateclick"), this->getUserID(), (TreeView_GetCheckState(this->m_Hwnd, tvh.hItem) == 0 ? 2 : 1) , path.to_chr());
							else if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
								this->execAliasEx(TEXT("%s,%d,%d,%s"), TEXT("stateclick"), this->getUserID(), TreeView_GetItemState(this->m_Hwnd, tvh.hItem, TVIS_STATEIMAGEMASK), path.to_chr());
						}
					}
					//|| ( ( tvh.flags & TVHT_ONITEMRIGHT ) && this->isStyle( TVS_FULLROWSELECT ) ) )
					else if (tvh.flags & TVHT_ONITEM) {
						TreeView_SelectItem(this->m_Hwnd, tvh.hItem);

						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
							this->execAliasEx(TEXT("%s,%d,%s"), TEXT("sclick"), this->getUserID(), this->getPathFromItem(&tvh.hItem).to_chr());
					}
					// single click not on item
					else if ((tvh.flags & TVHT_NOWHERE) || (tvh.flags & TVHT_ONITEMRIGHT)) {
						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
							this->execAliasEx(TEXT("%s,%d"), TEXT("sclick"), this->getUserID());
					}

					bParsed = TRUE;
					break;
				}

			case NM_DBLCLK:
				{
					TVHITTESTINFO tvh;

					GetCursorPos(&tvh.pt);
					MapWindowPoints(NULL, this->m_Hwnd, &tvh.pt, 1);
					TreeView_HitTest(this->m_Hwnd, &tvh);

					//|| ( ( tvh.flags & TVHT_ONITEMRIGHT ) && this->isStyle( TVS_FULLROWSELECT ) ) )
					if (tvh.flags & TVHT_ONITEM) {
						TreeView_SelectItem(this->m_Hwnd, tvh.hItem);

						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
							this->execAliasEx(TEXT("%s,%d,%s"), TEXT("dclick"), this->getUserID(), this->getPathFromItem(&tvh.hItem).to_chr());
					}

					bParsed = TRUE;
					break;
				}

			case NM_RCLICK:
				{
					TVHITTESTINFO tvh;

					GetCursorPos(&tvh.pt);
					MapWindowPoints(NULL, this->m_Hwnd, &tvh.pt, 1);
					TreeView_HitTest(this->m_Hwnd, &tvh);

					//|| ( ( tvh.flags & TVHT_ONITEMRIGHT ) && this->isStyle( TVS_FULLROWSELECT ) ) )
					if (tvh.flags & TVHT_ONITEM) {
						TreeView_SelectItem(this->m_Hwnd, tvh.hItem);

						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
							this->execAliasEx(TEXT("%s,%d,%s"), TEXT("rclick"), this->getUserID(), this->getPathFromItem(&tvh.hItem).to_chr());
					}
					else if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
						this->execAliasEx(TEXT("%s,%d"), TEXT("rclick"), this->getUserID());

					bParsed = TRUE;
					return TRUE; // stop rclick being passed down to parent controls.
				}
				break;

			case TVN_SELCHANGED:
			{
				LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW) lParam;

				if (lpnmtv != NULL && !m_bDestroying)
					this->execAliasEx(TEXT("%s,%d,%s"), TEXT("selchange"), this->getUserID(), this->getPathFromItem(&lpnmtv->itemNew.hItem).to_chr());

				bParsed = TRUE;
			}
			break;

			case TVN_GETINFOTIP:
				{
					LPNMTVGETINFOTIP tcgit = (LPNMTVGETINFOTIP) lParam;
					if (tcgit != NULL) {
						LPDCXTVITEM lpdcxtvi = (LPDCXTVITEM) tcgit->lParam;
						if (lpdcxtvi != NULL) {
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
					if (this->isExStyle(WS_EX_TRANSPARENT))
						this->setRedraw(FALSE);
				}
				break;

			case TVN_ITEMEXPANDED:
				{
					LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW) lParam;

					if (lpnmtv->action & TVE_COLLAPSE)
						this->execAliasEx(TEXT("%s,%d,%s"), TEXT("collapse"), this->getUserID(), this->getPathFromItem(&lpnmtv->itemNew.hItem).to_chr());
					else if (lpnmtv->action & TVE_EXPAND)
						this->execAliasEx(TEXT("%s,%d,%s"), TEXT("expand"), this->getUserID(), this->getPathFromItem(&lpnmtv->itemNew.hItem).to_chr());

					// re-enables redraw & updates.
					if (this->isExStyle(WS_EX_TRANSPARENT)) {
						this->setRedraw(TRUE);
						//InvalidateRect(this->m_Hwnd, NULL, FALSE);
						//UpdateWindow(this->m_Hwnd);
						//if (this->m_pParentDialog->IsVistaStyle())
						this->redrawWindow();
						//else
						//	this->redrawBufferedWindow();
					}

					bParsed = TRUE;
				}
				break;

			case TVN_BEGINLABELEDIT:
				{
					bParsed = TRUE;
					LPNMTVDISPINFO lptvdi = (LPNMTVDISPINFO) lParam;

					TreeView_SelectItem( this->m_Hwnd,lptvdi->item.hItem );

					HWND edit_hwnd = TreeView_GetEditControl( this->m_Hwnd );

					this->m_OrigEditProc = SubclassWindow( edit_hwnd, DcxTreeView::EditLabelProc );
					SetProp( edit_hwnd, TEXT("dcx_pthis"), (HANDLE) this );

					TCHAR ret[256];
					this->evalAliasEx( ret, 255, TEXT("%s,%d"), TEXT("labelbegin"), this->getUserID( ) );

					if ( !lstrcmp( TEXT("noedit"), ret ) )
						return TRUE;

					return FALSE;
				}
				break;
			case TVN_ENDLABELEDIT:
				{
					bParsed = TRUE;

					LPNMTVDISPINFO lptvdi = (LPNMTVDISPINFO) lParam;

					if ( lptvdi->item.pszText == NULL )
						this->execAliasEx(TEXT("%s,%d"), TEXT("labelcancel"), this->getUserID( ) );
					else {
						TCHAR ret[256];
						this->evalAliasEx( ret, 255, TEXT("%s,%d,%s"), TEXT("labelend"), this->getUserID( ), lptvdi->item.pszText );

						if ( !lstrcmp( TEXT("noedit"), ret ) )
							return FALSE;
					}
					return TRUE;
				}
				break;

			case NM_CUSTOMDRAW:
				{
					LPNMTVCUSTOMDRAW lpntvcd = (LPNMTVCUSTOMDRAW) lParam;
					bParsed = TRUE;

					switch( lpntvcd->nmcd.dwDrawStage ) {

					case CDDS_PREPAINT:
						return ( CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW );

					case CDDS_ITEMPREPAINT:
						{
							LPDCXTVITEM lpdcxtvi = (LPDCXTVITEM) lpntvcd->nmcd.lItemlParam;

							if ( lpdcxtvi == NULL )
								return CDRF_DODEFAULT;

							if ( lpdcxtvi->clrText != -1 )
								lpntvcd->clrText = lpdcxtvi->clrText;

							const bool bSelected = (lpntvcd->nmcd.uItemState & CDIS_SELECTED);

							// draw unselected background color
							//if (this->isExStyle(WS_EX_TRANSPARENT) && !bSelected){
							//	lpntvcd->clrTextBk = CLR_NONE;
							//	SetBkMode(lpntvcd->nmcd.hdc, TRANSPARENT);
							//}
							//else if ((lpdcxtvi->clrBkg != -1) && !bSelected)
							if ((lpdcxtvi->clrBkg != -1) && !bSelected)
								lpntvcd->clrTextBk = lpdcxtvi->clrBkg;
							else if ((this->m_colSelection != -1) && bSelected)
								lpntvcd->clrTextBk = this->m_colSelection;

							if ( lpdcxtvi->bUline || lpdcxtvi->bBold || lpdcxtvi->bItalic) {
								HFONT hFont = GetWindowFont( this->m_Hwnd );

								LOGFONT lf;
								GetObject( hFont, sizeof(LOGFONT), &lf );

								if (lpdcxtvi->bBold)
									lf.lfWeight |= FW_BOLD;
								if (lpdcxtvi->bUline)
									lf.lfUnderline = true;
								if (lpdcxtvi->bItalic)
									lf.lfItalic = true;

								this->m_hItemFont = CreateFontIndirect( &lf );
								if (this->m_hItemFont != NULL)
									this->m_hOldItemFont = SelectFont( lpntvcd->nmcd.hdc, this->m_hItemFont );
							}
							//TVITEMEX tvitem;
							//TCHAR buf[MIRC_BUFFER_SIZE_CCH];
							//ZeroMemory(&tvitem,sizeof(tvitem));
							//tvitem.hItem = (HTREEITEM)lpntvcd->nmcd.dwItemSpec;
							//tvitem.mask = TVIF_TEXT;
							//tvitem.pszText = buf;
							//tvitem.cchTextMax = MIRC_BUFFER_SIZE_CCH;
							//TreeView_GetItem(this->m_Hwnd, &tvitem);
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
							if (this->m_hOldItemFont != NULL) {
								SelectFont( lpntvcd->nmcd.hdc, this->m_hOldItemFont);
								this->m_hOldItemFont = NULL;
							}
							if (this->m_hItemFont != NULL) {
								DeleteFont(this->m_hItemFont);
								this->m_hItemFont = NULL;
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
					LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW) lParam;
					LPDCXTVITEM lpdcxtvi = (LPDCXTVITEM) lpnmtv->itemOld.lParam;

					if ( lpdcxtvi != NULL ) 
						delete lpdcxtvi;
				}
				break;
			case TVN_KEYDOWN:
				{
					LPNMTVKEYDOWN ptvkd = (LPNMTVKEYDOWN) lParam;

					if (ptvkd->wVKey == VK_SPACE)
					{
						HTREEITEM htvi = TreeView_GetSelection(this->m_Hwnd);
						if (htvi != NULL) {
							const BOOL state = TreeView_GetCheckState(this->m_Hwnd, htvi);
							//this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("stateclick"), this->getUserID( ), (state ? 0 : 1), TreeView_MapHTREEITEMToAccID(this->m_Hwnd, htvi) );
							this->execAliasEx(TEXT("%s,%d,%d,%s"), TEXT("stateclick"), this->getUserID( ), (state ? 0 : 1), this->getPathFromItem(&htvi).to_chr() );
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
				HDC hdc;

				hdc = BeginPaint( this->m_Hwnd, &ps );

				this->DrawClientArea(hdc, uMsg, lParam);

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
			return this->CommonMessage( uMsg, wParam, lParam, bParsed);
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

	DcxTreeView * pthis = (DcxTreeView *) GetProp( mHwnd, TEXT("dcx_pthis") );

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

void DcxTreeView::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	// Setup alpha blend if any. Double Buffer is needed to stop flicker when a bkg image is used.
	LPALPHAINFO ai = this->SetupAlphaBlend(&hdc, true);

#ifdef DCX_USE_GDIPLUS
	if (Dcx::GDIModule.isUseable() && this->m_pImage != NULL)
		DrawGDIPlusImage(hdc);
#endif

	CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

	this->FinishAlphaBlend(ai);
}

// clears existing image and icon data and sets pointers to null
void DcxTreeView::PreloadData() {
#ifdef DCX_USE_GDIPLUS
	if (this->m_pImage != NULL) {
		delete this->m_pImage;
		this->m_pImage = NULL;
	}
#endif
}

#ifdef DCX_USE_GDIPLUS
bool DcxTreeView::LoadGDIPlusImage(const TString &flags, TString &filename) {
	if (!IsFile(filename)) {
		this->showErrorEx(NULL,TEXT("LoadGDIPlusImage"), TEXT("Unable to Access File: %s"), filename.to_chr());
		return false;
	}
	this->m_pImage = new Image(filename.to_chr(),TRUE);

	// couldnt allocate image object.
	if (this->m_pImage == NULL) {
		this->showError(NULL,TEXT("LoadGDIPlusImage"), TEXT("Couldn't allocate image object."));
		return false;
	}
	// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
	Status status = this->m_pImage->GetLastStatus();
	if (status != Ok) {
		this->showError(NULL,TEXT("LoadGDIPlusImage"), GetLastStatusStr(status));
		PreloadData();
		return false;
	}
	const XSwitchFlags xflags(flags);

	if (xflags[TEXT('h')]) { // High Quality
		this->m_CQuality = CompositingQualityHighQuality;
		this->m_IMode = InterpolationModeHighQualityBicubic;
	}
	else {
		this->m_CQuality = CompositingQualityDefault;
		this->m_IMode = InterpolationModeDefault;
	}

	if (xflags[TEXT('b')]) // Blend Image
		this->m_CMode = CompositingModeSourceOver;
	else
		this->m_CMode = CompositingModeSourceCopy;

	if (xflags[TEXT('a')]) // Anti-Aliased
		this->m_SMode = SmoothingModeAntiAlias;
	else
		this->m_SMode = SmoothingModeDefault;

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
	GetClientRect(this->m_Hwnd, &rc);

	const int w = (rc.right - rc.left), h = (rc.bottom - rc.top), x = rc.left, y = rc.top;
	if (this->m_bTransparent) {
		if (!this->m_bAlphaBlend)
			this->DrawParentsBackground( hdc, &rc);
	}
	else {
		HBRUSH hBrush = this->getBackClrBrush();
		if (hBrush == NULL) {
			hBrush = CreateSolidBrush(GetBkColor(hdc));
			FillRect( hdc, &rc, hBrush );
			DeleteBrush(hBrush);
		}
		else
			FillRect( hdc, &rc, hBrush );
	}
	Graphics grphx( hdc );

	grphx.SetCompositingQuality(this->m_CQuality);
	grphx.SetCompositingMode(this->m_CMode);
	grphx.SetSmoothingMode(this->m_SMode);

	if (((this->m_pImage->GetWidth() == 1) || (this->m_pImage->GetHeight() == 1)) && this->m_bResizeImage) {
		// This fixes a GDI+ bug when resizing 1 px images
		// http://www.devnewsgroups.net/group/microsoft.public.dotnet.framework.windowsforms/topic11515.aspx
		grphx.SetInterpolationMode(InterpolationModeNearestNeighbor);
		grphx.SetPixelOffsetMode(PixelOffsetModeHalf);
	}
	else
		grphx.SetInterpolationMode(this->m_IMode);

	if (this->m_bTileImage) {
		ImageAttributes imAtt;
		imAtt.SetWrapMode(WrapModeTile);

		grphx.DrawImage(this->m_pImage,
			Rect(x + this->m_iXOffset, y + this->m_iYOffset, w, h),  // dest rect
			0, 0, w, h,       // source rect
			UnitPixel,
			&imAtt);
	}
	else if (this->m_bResizeImage)
		grphx.DrawImage( this->m_pImage, this->m_iXOffset, this->m_iYOffset, w, h );
	else
		grphx.DrawImage( this->m_pImage, this->m_iXOffset, this->m_iYOffset);
}
#endif
/*
	xmlLoadTree()
	Loads items into a treeview control from a dcxml file.
*/
HTREEITEM DcxTreeView::xmlLoadTree(HTREEITEM hInsertAfter, HTREEITEM hParent, const TString &name, TString &filename)
{
	if (!IsFile(filename)) {
		this->showErrorEx(NULL, NULL, TEXT("Unable To Access File: %s"), filename.to_chr());
		return false;
	}

	TiXmlDocument doc(filename.c_str());
	doc.SetCondenseWhiteSpace(false);

	bool xmlFile = doc.LoadFile();
	if (!xmlFile) {
		this->showErrorEx(NULL, TEXT("-a"), TEXT("Not an XML File: %s"), filename.to_chr());
		return false;
	}

	const TiXmlElement *xRoot = doc.FirstChildElement("dcxml");
	if (xRoot == NULL) {
		this->showError(NULL, TEXT("-a"), TEXT("Unable Find 'dcxml' root"));
		return false;
	}

	const TiXmlElement *xElm = xRoot->FirstChildElement("treeview_data");
	if (xElm == NULL) {
		this->showError(NULL, TEXT("-a"), TEXT("Unable To Find 'treeview_data' element"));
		return false;
	}

	xElm = xElm->FirstChildElement(name.c_str());
	if (xElm == NULL) {
		this->showErrorEx(NULL, TEXT("-a"), TEXT("Unable To Find Dataset: %s"), name.to_chr());
		return false;
	}

	if ( hInsertAfter == TVI_ROOT)
		hInsertAfter = TreeView_GetRoot(this->m_Hwnd);

	this->setRedraw(FALSE);
	this->xmlInsertItems(hParent, hInsertAfter, xElm);
	this->setRedraw(TRUE);
	//InvalidateRect(this->m_Hwnd, NULL, FALSE);
	//UpdateWindow(this->m_Hwnd);
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
		if (!xmlFile) {
			this->showErrorEx(NULL, TEXT("-S"), TEXT("Not an XML File: %s"), filename.to_chr());
			return false;
		}
	}
	TiXmlElement *xRoot = doc.FirstChildElement("dcxml");
	if (xRoot == NULL) {
		xRoot = (TiXmlElement *)doc.InsertEndChild(TiXmlElement("dcxml"));

		if (xRoot == NULL) {
			this->showErrorEx(NULL, TEXT("-S"), TEXT("Unable To Add Root <dcxml>"));
			return false;
		}
	}

	TiXmlElement *xData = xRoot->FirstChildElement("treeview_data");
	if (xData == NULL) {
		xData = (TiXmlElement *)xRoot->InsertEndChild(TiXmlElement("treeview_data"));
		if (xData == NULL)
			return false;
	}

	TiXmlElement *xElm = xData->FirstChildElement(name.c_str());
	if (xElm == NULL) {
		xElm = (TiXmlElement *)xData->InsertEndChild(TiXmlElement(name.c_str()));
		if (xElm == NULL)
			return false;
	}

	xElm->Clear();

	if ( hFromItem == TVI_ROOT)
		hFromItem = TreeView_GetRoot(this->m_Hwnd);

	PTCHAR buf = new TCHAR[MIRC_BUFFER_SIZE_CCH];
	if (buf != NULL) {
		if (!this->xmlGetItems(hFromItem, xElm, buf)) {
			this->showErrorEx(NULL, TEXT("-S"), TEXT("Unable To Add Items to XML"));
			return false;
		}
		else
			doc.SaveFile();
		delete [] buf;
	}
	return true;
}

/*
	xmlGetItems()
	Recursive function that loops through all treeview items & adds them to the xml data.
*/
bool DcxTreeView::xmlGetItems(HTREEITEM hFirstSibling, TiXmlElement *xElm, TCHAR *buf)
{
	if (hFirstSibling == NULL)
		return false;

	TVITEMEX tvi;
	TiXmlElement *xTmp = NULL;
	bool bRes = true;
	for (HTREEITEM hSib = hFirstSibling; hSib != NULL; hSib = TreeView_GetNextSibling(this->m_Hwnd, hSib)) {
		ZeroMemory(&tvi,sizeof(tvi));
		tvi.hItem = hSib;
#ifdef DCX_USE_WINSDK
		tvi.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE | TVIF_INTEGRAL | TVIF_STATE | TVIF_TEXT | TVIF_CHILDREN;
#else
		tvi.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_INTEGRAL | TVIF_STATE | TVIF_TEXT | TVIF_CHILDREN;
#endif
		tvi.cchTextMax = MIRC_BUFFER_SIZE_CCH;
		tvi.pszText = buf;

		if (!TreeView_GetItem(this->m_Hwnd, &tvi))
			break;

		LPDCXTVITEM lpmytvi = (LPDCXTVITEM)tvi.lParam;
		if (lpmytvi == NULL) {
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
#ifdef DCX_USE_WINSDK
			if (tvi.iExpandedImage > 0 && tvi.iExpandedImage != 10000)
				xChild.SetAttribute("expandedimage", tvi.iExpandedImage);
#endif
			if (tvi.iIntegral > 0)
				xChild.SetAttribute("itegral", tvi.iIntegral);
			if (tvi.stateMask & TVIS_SELECTED && tvi.state & TVIS_SELECTED)
				xChild.SetAttribute("selected", 1);
			if (lpmytvi->tsTipText.len())
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
			UINT i = (tvi.state & TVIS_OVERLAYMASK) >> 8;
			if (i > 0 && i < 16) // zero means no overlay, so don't save
				xChild.SetAttribute("overlay", i);
			i = (tvi.state & TVIS_STATEIMAGEMASK) >> 12;
			if (i > 0 && i < 5)
				xChild.SetAttribute("state", i); // zero means no state image so don't save
			xTmp = xElm->InsertEndChild(xChild)->ToElement();
			if (xTmp == NULL) {
				bRes = false;
				break;
			}
		}
		if (tvi.cChildren > 0 && xTmp != NULL)
			bRes = this->xmlGetItems(TreeView_GetChild(this->m_Hwnd, hSib), xTmp, buf);
	}
	return bRes;
}

/*
	xmlInsertItems()
	Recursive function that inserts items into a treeview control from the xml data.
*/
const TiXmlElement *DcxTreeView::xmlInsertItems(HTREEITEM hParent, HTREEITEM &hInsertAfter, const TiXmlElement *xElm)
{
	TVINSERTSTRUCT tvins;
	int i = 0;
	const char *attr = NULL;
	const TiXmlElement *xRes = xElm;

	for (const TiXmlElement *xNode = xElm->FirstChildElement("tvitem"); xNode != NULL; xNode = xNode->NextSiblingElement("tvitem")) {
		ZeroMemory(&tvins, sizeof(tvins));
		tvins.hInsertAfter = hInsertAfter;
		tvins.hParent = hParent;
		LPDCXTVITEM lpmytvi = new DCXTVITEM;

		if (lpmytvi == NULL)
			return xRes;

		lpmytvi->hHandle = NULL;

#ifdef DCX_USE_WINSDK
		tvins.itemex.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE;
#else
		tvins.itemex.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
#endif
		tvins.itemex.lParam = (LPARAM)lpmytvi;

//		// Items Icon.
//		attr = xNode->Attribute("image",&i);
//		if (attr != NULL && i > 0)
//			tvins.itemex.iImage = i -1;
//		else
//			tvins.itemex.iImage = 10000;
//
//		// Items Selected state icon.
//		attr = xNode->Attribute("selectedimage",&i);
//		if (attr != NULL && i > 0)
//			tvins.itemex.iSelectedImage = i -1;
//		else
//			tvins.itemex.iSelectedImage = tvins.itemex.iImage;
//		// Items expanded state icon.
//#ifdef DCX_USE_WINSDK
//		attr = xNode->Attribute("expandedimage",&i);
//		if (attr != NULL && i > 0)
//			tvins.itemex.iExpandedImage = i -1;
//		else
//			tvins.itemex.iExpandedImage = 10000;
//#endif
//		// Items height integral.
//		attr = xNode->Attribute("integral",&i);
//		if (attr != NULL && i > 0) {
//			tvins.itemex.iIntegral = i;
//			tvins.itemex.mask |= TVIF_INTEGRAL;
//		}
//		// Items selected
//		attr = xNode->Attribute("selected",&i);
//		if (attr != NULL && i > 0) {
//			tvins.itemex.state |= TVIS_SELECTED;
//			tvins.itemex.stateMask |= TVIS_SELECTED;
//		}
//		// Items tooltip
//		attr = xNode->Attribute("tooltip");
//		if (attr != NULL) {
//			lpmytvi->tsTipText = attr;
//		}
//		// Items text colour.
//		attr = xNode->Attribute("textcolor",&i);
//		if (attr != NULL && i > -1) {
//			lpmytvi->clrText = (COLORREF)i;
//		}
//		else
//			lpmytvi->clrText = CLR_INVALID;
//		// Items background colour.
//		attr = xNode->Attribute("backgroundcolor",&i);
//		if (attr != NULL && i > -1) {
//			lpmytvi->clrBkg = (COLORREF)i;
//		}
//		else
//			lpmytvi->clrBkg = CLR_INVALID;
//		// Is Item text in Bold?
//		attr = xNode->Attribute("textbold",&i);
//		if (i > 0) {
//			lpmytvi->bBold = TRUE;
//			tvins.itemex.state |= TVIS_BOLD;
//			tvins.itemex.stateMask |= TVIS_BOLD;
//		}
//		else
//			lpmytvi->bBold = FALSE;
//		// Is item text in italics?
//		attr = xNode->Attribute("textitalic",&i);
//		lpmytvi->bItalic = (i > 0);
//		// Is item text underlined?
//		attr = xNode->Attribute("textunderline",&i);
//		lpmytvi->bUline = (i > 0);
//		// Items Text.
//		attr = xNode->Attribute("text");
//		TString tsAttr(attr);
//		if (attr != NULL) {
//			tvins.itemex.cchTextMax = tsAttr.len();
//			tvins.itemex.pszText = tsAttr.to_chr();
//			tvins.itemex.mask |= TVIF_TEXT;
//		}
//		hInsertAfter = TreeView_InsertItem(this->m_Hwnd, &tvins);
//		if (hInsertAfter == NULL) {
//			delete lpmytvi;
//			this->showError(NULL, TEXT("-a"), TEXT("Unable To Add XML Item To TreeView"));
//			return NULL;
//		}
//		lpmytvi->hHandle = hInsertAfter;
//		// Items state icon.
//		attr = xNode->Attribute("state",&i);
//		if (attr != NULL && i > -1 && i < 5) // zero means no state icon anyway.
//			TreeView_SetItemState(this->m_Hwnd, hInsertAfter, INDEXTOSTATEIMAGEMASK(i), TVIS_STATEIMAGEMASK);
//		// Items overlay icon.
//		// overlay is 1-based index
//		attr = xNode->Attribute("overlay",&i);
//		if (attr != NULL && i > 0 && i < 16)
//			TreeView_SetItemState(this->m_Hwnd, hInsertAfter, INDEXTOOVERLAYMASK(i), TVIS_OVERLAYMASK);

		// Items Icon.
		tvins.itemex.iImage = this->queryIntAttribute(xNode, "image", I_IMAGECALLBACK) -1;	// NB: I_IMAGECALLBACK is change to I_IMAGENONE by the -1

		// Items Selected state icon.
		tvins.itemex.iSelectedImage = this->queryIntAttribute(xNode, "selectedimage", tvins.itemex.iImage +1) -1;

		// Items expanded state icon.
#ifdef DCX_USE_WINSDK
		tvins.itemex.iExpandedImage = this->queryIntAttribute(xNode, "expandedimage", I_IMAGECALLBACK) -1;	// NB: I_IMAGECALLBACK is change to I_IMAGENONE by the -1
#endif
		// Items height integral.
		tvins.itemex.iIntegral = this->queryIntAttribute(xNode, "integral");
		if (tvins.itemex.iIntegral > 0)
			tvins.itemex.mask |= TVIF_INTEGRAL;

		// Items selected
		if (this->queryIntAttribute(xNode, "selected") > 0) {
			tvins.itemex.state |= TVIS_SELECTED;
			tvins.itemex.stateMask |= TVIS_SELECTED;
		}

		// Items tooltip
		attr = xNode->Attribute("tooltip");
		if (attr != NULL) {
			lpmytvi->tsTipText = attr;
		}

		// Items text colour.
		lpmytvi->clrText = (COLORREF)this->queryIntAttribute(xNode, "textcolor", CLR_INVALID);

		// Items background colour.
		lpmytvi->clrBkg = (COLORREF)this->queryIntAttribute(xNode, "backgroundcolor", CLR_INVALID);

		// Is Item text in Bold?
		if (this->queryIntAttribute(xNode, "textbold") > 0) {
			lpmytvi->bBold = TRUE;
			tvins.itemex.state |= TVIS_BOLD;
			tvins.itemex.stateMask |= TVIS_BOLD;
		}
		else
			lpmytvi->bBold = FALSE;

		// Is item text in italics?
		lpmytvi->bItalic = (this->queryIntAttribute(xNode, "textitalic") > 0);

		// Is item text underlined?
		lpmytvi->bUline = (this->queryIntAttribute(xNode, "textunderline") > 0);

		// Items Text.
		attr = xNode->Attribute("text");
		TString tsAttr(attr); // MUST be before/outside if()
		if (attr != NULL) {
			tvins.itemex.cchTextMax = tsAttr.len();
			tvins.itemex.pszText = tsAttr.to_chr();
			tvins.itemex.mask |= TVIF_TEXT;
		}
		hInsertAfter = TreeView_InsertItem(this->m_Hwnd, &tvins);
		if (hInsertAfter == NULL) {
			delete lpmytvi;
			this->showError(NULL, TEXT("-a"), TEXT("Unable To Add XML Item To TreeView"));
			return NULL;
		}
		lpmytvi->hHandle = hInsertAfter;

		// Items state icon.
		i = this->queryIntAttribute(xNode, "state");
		if (i < 5) // zero means no state icon anyway.
			TreeView_SetItemState(this->m_Hwnd, hInsertAfter, INDEXTOSTATEIMAGEMASK(i), TVIS_STATEIMAGEMASK);

		// Items overlay icon.
		// overlay is 1-based index
		i = this->queryIntAttribute(xNode, "overlay");
		if (i > 0 && i < 16)
			TreeView_SetItemState(this->m_Hwnd, hInsertAfter, INDEXTOOVERLAYMASK(i), TVIS_OVERLAYMASK);

		if (xNode->FirstChild("tvitem") != NULL) {
			// item has children. NB: DON'T update xNode to the result of this call as this stops subsequent items being added.
			this->xmlInsertItems(hInsertAfter, hInsertAfter, xNode);
		}
		xRes = xNode;
	}
	return xRes;
}

TString DcxTreeView::getStyles(void) {
	TString styles(__super::getStyles());
	const DWORD Styles = GetWindowStyle(this->m_Hwnd);
	const DWORD ExStyles = GetWindowExStyle(this->m_Hwnd);

	if (Styles & TVS_HASLINES)
		styles.addtok(TEXT("haslines"));
	if (Styles & TVS_HASBUTTONS)
		styles.addtok(TEXT("hasbuttons"));
	if (Styles & TVS_LINESATROOT)
		styles.addtok(TEXT("linesatroot"));
	if (Styles & TVS_SHOWSELALWAYS)
		styles.addtok(TEXT("showsel"));
	if (Styles & TVS_EDITLABELS)
		styles.addtok(TEXT("editlabel"));
	if (Styles & TVS_NOHSCROLL)
		styles.addtok(TEXT("nohscroll"));
	if (Styles & TVS_FULLROWSELECT)
		styles.addtok(TEXT("fullrow"));
	if (Styles & TVS_SINGLEEXPAND)
		styles.addtok(TEXT("singleexpand"));
	if (ExStyles & TVS_CHECKBOXES)
		styles.addtok(TEXT("checkbox"));
	return styles;
}

/*
 * Retrieves an item from the path specified.
 *
 * If the path is invalid, it will return NULL.
 * If hParent and hInsertAt parameters are filled, it is assumed we are searching for a place to insert the item
 */
HTREEITEM DcxTreeView::parsePath(const TString *path, HTREEITEM *hParent, HTREEITEM *hInsertAt) const {
	HTREEITEM current = TVI_ROOT;
	HTREEITEM foundSoFar = current;
	const int count = path->numtok();
	int dir;
	int i;                             // iterator for counting the Nth directory
	//int k = 1;                         // iterator for path directories
	const bool bFillLocation = ((hParent != NULL) && (hInsertAt != NULL));

	// Invalid path - no directives.
	if (count == 0)
		return NULL;

	// Iterate through each directive in the path string
	for (int k = 1; k <= count; k++)
	{
		// Convert the path directory.
		dir = path->gettok(k).to_int();

		// Keep track of the current parent.
		if (bFillLocation)
			*hParent = current;

		current = TreeView_GetChild(this->m_Hwnd, current);

		// Adding first child item, with the 2nd last directive as the parent node
		if ((current == NULL) && (getChildCount(&foundSoFar) == 0) && bFillLocation && (k == count)) {
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

			while ((tmp = TreeView_GetNextSibling(this->m_Hwnd, current)) != NULL) {
				current = tmp;
			}

			if (bFillLocation)
				*hInsertAt = TVI_LAST;
		}
		// Search for Nth child.
		else {
			//i = 1;

			//while ((current = TreeView_GetNextSibling(this->m_Hwnd, current)) != NULL) {
			//	if (bFillLocation)
			//		*hInsertAt = current;

			//	i++;

			//	// Found the branch we want
			//	if (i == dir)
			//		break;
			//}
			// this version allows using a path thats `last item + 1`
			i = 1;
			HTREEITEM tmp;

			while ((tmp = TreeView_GetNextSibling(this->m_Hwnd, current)) != NULL) {
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
		if (current == NULL)
			return NULL;
		// Inch forward on next item.
		else
			foundSoFar = current;
	}

	return foundSoFar;
}
int DcxTreeView::queryIntAttribute(const TiXmlElement *element,const char *attribute,const int defaultValue)
{
	int integer = defaultValue;
	if (element->QueryIntAttribute(attribute,&integer) == TIXML_SUCCESS) {
		if (integer < 0)
			integer = defaultValue;
	}
	return integer;
}
