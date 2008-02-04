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

#include "dcxtreeview.h"
#include "dcxdialog.h"

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
, m_colSelection(-1)
, m_hOldItemFont(NULL)
, m_hItemFont(NULL)
#ifdef DCX_USE_GDIPLUS
, m_pImage(NULL)
, m_bResizeImage(false)
, m_bTileImage(false)
, m_iXOffset(0)
, m_iYOffset(0)
, m_bTransparent(false)
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
		throw "Unable To Create Window";

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

  SendMessage( this->m_Hwnd, CCM_SETVERSION, (WPARAM) 5, (LPARAM) 0 );

  if ( ExStyles & TVS_CHECKBOXES )
    this->addStyle( TVS_CHECKBOXES );

	this->m_ToolTipHWND = (HWND)TreeView_GetToolTips(this->m_Hwnd);
	if (styles.istok("balloon") && this->m_ToolTipHWND != NULL) {
		SetWindowLong(this->m_ToolTipHWND,GWL_STYLE,GetWindowLong(this->m_ToolTipHWND,GWL_STYLE) | TTS_BALLOON);
	}

#ifdef DCX_USE_WINSDK
	if (mIRCLink.m_bVista) {
		ExStyles = TreeView_GetExtendedStyle(this->m_Hwnd);
		parseTreeViewExStyles( styles, &ExStyles);
		TreeView_SetExtendedStyle(this->m_Hwnd, ExStyles, ExStyles);
	}
#endif

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );

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

void DcxTreeView::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	*Styles |= TVS_INFOTIP;

	unsigned int i = 1, numtok = styles.numtok( );

	while ( i <= numtok ) {

		if ( styles.gettok( i ) == "haslines" ) 
			*Styles |= TVS_HASLINES;
		else if ( styles.gettok( i ) == "hasbuttons" ) 
			*Styles |= TVS_HASBUTTONS;
		else if ( styles.gettok( i ) == "linesatroot" ) 
			*Styles |= TVS_LINESATROOT;
		else if ( styles.gettok( i ) == "showsel" ) 
			*Styles |= TVS_SHOWSELALWAYS;
		else if ( styles.gettok( i ) == "editlabel" ) 
			*Styles |= TVS_EDITLABELS;
		else if ( styles.gettok( i ) == "nohscroll" ) 
			*Styles |= TVS_NOHSCROLL;
		else if ( styles.gettok( i ) == "notooltips" ) 
			*Styles |= TVS_NOTOOLTIPS;
		else if ( styles.gettok( i ) == "noscroll" ) 
			*Styles |= TVS_NOSCROLL;
		else if ( styles.gettok( i ) == "fullrow" ) 
			*Styles |= TVS_FULLROWSELECT;
		else if ( styles.gettok( i ) == "singleexpand" ) 
			*Styles |= TVS_SINGLEEXPAND;
		else if ( styles.gettok( i ) == "checkbox" ) 
			*ExStyles |= TVS_CHECKBOXES;

		i++;
	}
	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

#ifdef DCX_USE_WINSDK
void DcxTreeView::parseTreeViewExStyles( const TString &styles, LONG * ExStyles ) {

	// Vista+ ONLY!
  unsigned int i = 1, numtok = styles.numtok( );

  while ( i <= numtok ) {

    if ( styles.gettok( i ) == "fadebuttons" )
      *ExStyles |= TVS_EX_FADEINOUTEXPANDOS;
    else if ( styles.gettok( i ) == "doublebuffer" )
      *ExStyles |= TVS_EX_DOUBLEBUFFER;
    //else if ( styles.gettok( i ) == "multi" )
    //  *ExStyles |= TVS_EX_MULTISELECT; // Style NOT to be used (unsupported by commctrl)
    else if ( styles.gettok( i ) == "noident" )
      *ExStyles |= TVS_EX_NOINDENTSTATE;
    else if ( styles.gettok( i ) == "richtooltip" )
      *ExStyles |= TVS_EX_RICHTOOLTIP;
    else if ( styles.gettok( i ) == "autohscroll" )
      *ExStyles |= TVS_EX_AUTOHSCROLL;

    i++;
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

void DcxTreeView::parseInfoRequest(TString &input, char *szReturnValue) {
	int numtok = input.numtok();
	TString prop(input.gettok(3));

	// [NAME] [ID] [PROP] [PATH]
	if (prop == "text" && numtok > 3) {
		TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx("text", NULL, "Unable to parse path: %s", path.to_chr());
			return;
		}

		this->getItemText(&item, szReturnValue, 900);
		return;
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == "icon" && numtok > 3) {
		TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx("icon", NULL, "Unable to parse path: %s", path.to_chr());
			return;
		}

		TVITEMEX tvi; 
		tvi.hItem = item;
		tvi.mask = TVIF_IMAGE | TVIF_HANDLE;

		TreeView_GetItem(this->m_Hwnd, &tvi);
		wsprintf(szReturnValue, "%d", (tvi.iImage > 10000 ? -2 : tvi.iImage) +1);
		return;
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == "tooltip" && numtok > 3) {
		TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx("tooltip", NULL, "Unable to parse path: %s", path.to_chr());
			return;
		}

		TVITEMEX tvi;
		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;

		TreeView_GetItem(this->m_Hwnd, &tvi);
		LPDCXTVITEM lpdcxtvi = (LPDCXTVITEM) tvi.lParam;

		if (lpdcxtvi != NULL)
			lstrcpyn(szReturnValue, lpdcxtvi->tsTipText.to_chr(), 900);

		return;
	}
	// [NAME] [ID] [PROP]
	else if (prop == "seltext") {
		HTREEITEM item = TreeView_GetSelection(this->m_Hwnd);

		this->getItemText(&item, szReturnValue, 900);
		return;
	}
	// [NAME] [ID] [PROP]
	else if ( prop == "selpath" ) {
		HTREEITEM hItem = TreeView_GetSelection(this->m_Hwnd);
		TString path = this->getPathFromItem(&hItem);

		lstrcpyn(szReturnValue, path.to_chr(), 900);
		return;
	}
	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N] [SUBPATH]
	else if (prop == "find" && numtok > 5) {
		TString matchtext(input.gettok(2, TSTAB).trim());
		TString params(input.gettok(3, TSTAB).trim());

		if (matchtext.len() < 1) {
			this->showErrorEx("find", NULL, "No matchtext specified.");
			return;
		}

		UINT searchType;
		TString searchMode = params.gettok(1);
		HTREEITEM startingPoint = TVI_ROOT;
		HTREEITEM result;

		if (searchMode == "R")
			searchType = TVSEARCH_R;
		else if (searchMode == "W")
			searchType = TVSEARCH_W;
		else
			searchType = TVSEARCH_E;

		int n = params.gettok(2).to_int();
		int matchCount = 0;

		if (params.numtok() > 2) {
			TString path(params.gettok(3, -1));

			startingPoint = this->parsePath(&path);

			if (startingPoint == NULL) {
				this->showErrorEx("find", NULL, "Unable to parse path: %s", path.to_chr());
				return;
			}
		}

		if (this->findItemText(&startingPoint, &result, &matchtext, n, &matchCount, searchType)) {
			TString path = this->getPathFromItem(&result);
			lstrcpyn(szReturnValue, path.to_chr(), 900);
		}
		else if (n == 0)
			wsprintf(szReturnValue, "%d", matchCount);

		return;
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == "state" && numtok > 3) {
		TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx("state", NULL, "Unable to parse path: %s", path.to_chr());
			return;
		}

		if (this->isStyle(TVS_CHECKBOXES)) {
			int state = TreeView_GetCheckState(this->m_Hwnd, item);

			if (state == 1)
				lstrcpy(szReturnValue, "2");
			else if (state == 0)
				lstrcpy(szReturnValue, "1");
			else
				lstrcpy(szReturnValue, "0");
			return;
		}
		else {
			wsprintf(szReturnValue, "%d", TreeView_GetItemState(this->m_Hwnd, item, TVIS_STATEIMAGEMASK));
			return;
		}
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == "num" && numtok > 3) {
		TString path(input.gettok(4, -1).trim());
		HTREEITEM item;

		if (path == "root") {
			item = TVI_ROOT;
			wsprintf(szReturnValue, "%d", this->getChildCount(&item));
			return;
		}

	    item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx("num", NULL, "Unable to parse path: %s", path.to_chr());
			return;
		}

        wsprintf(szReturnValue, "%d", this->getChildCount(&item));
        return;
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == "expand" && numtok > 3) {
		TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx("expand", NULL, "Unable to parse path: %s", path.to_chr());
			return;
		}

		if (TreeView_GetItemState(this->m_Hwnd, item, TVIS_EXPANDED) & TVIS_EXPANDED)
			lstrcpy(szReturnValue, "1");
		else
			lstrcpy(szReturnValue, "0");

		return;
	}
	// [NAME] [ID] [PROP]
	else if (prop == "mouseitem") {
		TVHITTESTINFO tvh;
		GetCursorPos(&tvh.pt);
		ScreenToClient( this->m_Hwnd, &tvh.pt );
		TreeView_HitTest( this->m_Hwnd, &tvh );

		if ( tvh.flags & TVHT_ONITEM ) {
			TString path = this->getPathFromItem(&tvh.hItem);

			lstrcpyn(szReturnValue, path.to_chr(), 900);
		}
		else
			lstrcpy(szReturnValue, "0");

		return;
	}
	// [NAME] [ID] [PROP] [PATH]
	else if (prop == "markeditem" && numtok > 3) {
		TString path(input.gettok(4, -1).trim());
		HTREEITEM item;

		item = parsePath(&path);

		if (item == NULL) {
			this->showErrorEx("markeditem", NULL, "Invalid Path: %s", path.to_chr());
			return;
		}

		TVITEMEX tvi;

		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;

		if (!TreeView_GetItem(this->m_Hwnd, &tvi)) {
			this->showErrorEx(NULL, "-A", "Unable to retrieve item: %s", path.to_chr());
			return;
		}

		LPDCXTVITEM lpdcxtvitem = (LPDCXTVITEM) tvi.lParam;
		wsprintf(szReturnValue, "%s", lpdcxtvitem->tsMark.to_chr());
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

void DcxTreeView::parseCommandRequest( TString & input ) {
	XSwitchFlags flags(input.gettok(3));
	int numtok = input.numtok( );

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags['r']) {
		TreeView_DeleteAllItems(this->m_Hwnd);
	}

	// xdid -a [NAME] [ID] [SWITCH] N N N ... N[TAB][+FLAGS] [#ICON] [#SICON] [#OVERLAY] [#STATE] [#INTEGRAL] [COLOR] [BKGCOLOR] Text[TAB]Tooltip Text
	if (flags['a']) {
		int n = input.numtok(TSTAB);

		if (n > 1) {
			TString path(input.gettok(1, TSTAB).gettok(4, -1).trim());
			TString data(input.gettok(2, TSTAB).trim());
			TString tooltip;

			if (n > 2)
				tooltip = input.gettok(3, -1, TSTAB).trim();

			if (data.numtok( ) > 8)
				this->insertItem(&path, &data, &tooltip);
		}
	}
	// xdid -A [NAME] [ID] [SWITCH] N N N ... N[TAB][+FLAGS] [INFO]
	else if (flags['A']) {
		int n = input.numtok(TSTAB);

		if (n < 2) {
			this->showErrorEx(NULL, "-A", "Insufficient parameters");
			return;
		}

		TString path(input.gettok(1, TSTAB).trim());
		TString data(input.gettok(2, TSTAB).trim());
		HTREEITEM item;

		n = path.numtok();

		if (n < 4) {
			this->showErrorEx(NULL, "-A", "Insufficient parameters (path)");
			return;
		}

		path = path.gettok(4, -1);
		n = data.numtok();

		if (n < 2) {
			this->showErrorEx(NULL, "-A", "Insufficient parameters (args)");
			return;
		}

		item = parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, "-A", "Invalid Path: %s", path.to_chr());
			return;
		}

		TString flag(data.gettok(1));
		TString info(data.gettok(2, -1));

		if (flag.find('M', 1) > 0) {
			TVITEMEX tvi; 

			tvi.hItem = item;
			tvi.mask = TVIF_HANDLE | TVIF_PARAM ; 

			if (!TreeView_GetItem(this->m_Hwnd, &tvi)) {
				this->showErrorEx(NULL, "-A", "Unable to retrieve item: %s", path.to_chr());
				return;
			}

			LPDCXTVITEM lpdcxtvitem = (LPDCXTVITEM) tvi.lParam;

			lpdcxtvitem->tsMark = info;
			TreeView_SetItem(this->m_Hwnd, &tvi);
		}
		else {
			this->showErrorEx(NULL, "-A", "Unknown flags %s", flag.to_chr());
			return;
		}
	}
	// xdid -B [NAME] [ID] [SWITCH] N N N
	else if (flags['B'] && numtok > 3) {
		TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);
		
		if (item == NULL) {
			this->showErrorEx(NULL, "-B", "Invalid Path: %s", path.to_chr());
			return;
		}

		TreeView_EnsureVisible(this->m_Hwnd, item); // make sure selected item is visible.
		TreeView_EditLabel(this->m_Hwnd, item);
	}
	// xdid -c [NAME] [ID] [SWITCH] N N N
	else if (flags['c'] && numtok > 3) {
		TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, "-c", "Invalid Path: %s", path.to_chr());
			return;
		}

		TreeView_EnsureVisible(this->m_Hwnd, item); // make sure selected item is visible.
		TreeView_SelectItem(this->m_Hwnd, item);
	}
	// xdid -d [NAME] [ID] [SWITCH] N N N
	else if (flags['d'] && numtok > 3) {
		TString path(input.gettok(4, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, "-d", "Invalid Path: %s", path.to_chr());
			return;
		}

		TreeView_DeleteItem(this->m_Hwnd, item);
	}
	// xdid -g [NAME] [ID] [SWITCH] [HEIGHT]
	else if (flags['g'] && numtok > 3) {
		int iHeight = input.gettok(4).to_int();

		if (iHeight > -2)
			TreeView_SetItemHeight(this->m_Hwnd, iHeight);
	}
	// xdid -i [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if ( flags['i'] && numtok > 4 ) {
		UINT iFlags = this->parseColorFlags(input.gettok( 4 ));

		COLORREF clr = (COLORREF) input.gettok( 5 ).to_num( );

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
	else if (flags['j'] && numtok > 5) {
		HTREEITEM item;
		TString path(input.gettok(1, TSTAB).gettok(4, -1).trim());

		// Invalid parameters, missing icons segment.
		if (input.numtok(TSTAB) < 2) {
			this->showErrorEx(NULL, "-j", "Invalid parameters.");
			return;
		}

		TString icons(input.gettok(2, TSTAB).trim());

		// Invalid parameters, missing icon args.
		if (icons.numtok() < 2) {
			this->showErrorEx(NULL, "-j", "Invalid parameters.");
			return;
		}

		item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, "-j", "Invalid Path: %s", path.to_chr());
			return;
		}

		int nIcon = icons.gettok(1).to_int() -1;
		int sIcon = icons.gettok(2).to_int() -1;
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
			int oIcon = icons.gettok(3).to_int();

			// overlay is 1-based index
			if (oIcon > -1)
				TreeView_SetItemState(this->m_Hwnd, tvi.hItem, INDEXTOOVERLAYMASK(oIcon), TVIS_OVERLAYMASK);

			// if ignoring both nIcon and sIcon
			if ((nIcon == -2) && (sIcon == -2))
				return;
		}

		// normal icon
		if (nIcon > -2) {
			tvi.mask |= TVIF_IMAGE;

			// quickfix so it doesnt display an image
			// http://dcx.scriptsdb.org/bug/?do=details&id=350
			if (nIcon == -1)
				nIcon = 10000;

			tvi.iImage = nIcon;
		}

		// selected icon
		if (sIcon > -2) {
			tvi.mask |= TVIF_SELECTEDIMAGE;

			// quickfix so it doesnt display an image
			// http://dcx.scriptsdb.org/bug/?do=details&id=350
			if (sIcon == -1)
				sIcon = 10000;

			tvi.iSelectedImage = sIcon;
		}

		TreeView_SetItem(this->m_Hwnd, &tvi);
	}
	// xdid -k [NAME] [ID] [SWITCH] [STATE] N N N
	else if (flags['k'] && numtok > 4) {
		TString path(input.gettok(5, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, "-k", "Invalid Path: %s", path.to_chr());
			return;
		}

		UINT state = input.gettok(4).to_int();

		TreeView_SetItemState(this->m_Hwnd, item, INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK);
	}
	// xdid -l [NAME] [ID] [SWITCH] [SIZE]
	else if ( flags['l'] && numtok > 3 ) {
		int size = input.gettok( 4 ).to_int( );

		if ( size != 32 && size != 24 )
			size = 16;

		this->m_iIconSize = size;
	}
	// xdid -m [NAME] [ID] [SWITCH] N N N{TAB}N N N
	else if (flags['m'] && numtok > 3 && input.numtok(TSTAB) > 1) {
		TString pathFrom(input.gettok(1, TSTAB).gettok(4, -1).trim());
		TString pathTo(input.gettok(2, TSTAB).trim());

		HTREEITEM item = this->parsePath(&pathFrom);
		HTREEITEM hParentTo = TVI_ROOT;
		HTREEITEM hAfterTo = TVI_ROOT;
		HTREEITEM hNewItem;

		if (item == NULL) {
			this->showErrorEx(NULL, "-m", "Unable to parse FROM path: %s", pathFrom.to_chr());
			return;
		}

		if (this->parsePath(&pathTo, &hParentTo, &hAfterTo) == NULL) {
			this->showErrorEx(NULL, "-m", "Unable to parse TO path: %s", pathTo.to_chr());
			return;
		}

		// Check if we're moving parent into a child branch
		if ((hParentTo != TVI_FIRST) && (hParentTo != TVI_LAST)) {
			HTREEITEM tmp = hParentTo;

			// Trace back all the way to root
			while (tmp != TVI_ROOT) {
				if (tmp == item) {
					this->showErrorEx(NULL, "-m", "Cannot move parent node (%s) to child node (%s).", pathFrom.to_chr(), pathTo.to_chr());
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
			this->showError(NULL, "-m", "Unable to move item.");
	}
	// xdid -n [NAME] [ID] [SWITCH] N N N{TAB}N N N
	else if (flags['n'] && numtok > 3 && input.numtok(TSTAB) > 1) {
		TString pathFrom(input.gettok(1, TSTAB).gettok(4, -1));
		TString pathTo(input.gettok(2, TSTAB));
		pathFrom.trim();
		pathTo.trim();

		HTREEITEM item;
		HTREEITEM hParentTo = TVI_ROOT;
		HTREEITEM hAfterTo = TVI_ROOT;
		HTREEITEM hNewItem;

		item = this->parsePath(&pathFrom);

		// Check source item.
		if (item == NULL) {
			this->showErrorEx(NULL, "-n", "Unable to parse FROM path: %s", pathFrom.to_chr());
			return;
		}

		// Check destination.
		if (this->parsePath(&pathTo, &hParentTo, &hAfterTo) == NULL) {
			this->showErrorEx(NULL, "-n", "Unable to parse TO path: %s", pathFrom.to_chr());
			return;
		}

		// Check if we're copying source into a child branch
		if ((hParentTo != TVI_FIRST) && (hParentTo != TVI_LAST) && (hParentTo != TVI_ROOT)) {
			HTREEITEM tmp = hParentTo;

			// Trace back all the way to root
			while (tmp != NULL) {
				if (tmp == item) {
					this->showErrorEx(NULL, "-n", "Cannot copy parent node (%s) to child node (%s).", pathFrom.to_chr(), pathTo.to_chr());
					return;
				}

				tmp = TreeView_GetParent(this->m_Hwnd, tmp);
			}
		}

		hNewItem = this->cloneItem(&item, &hParentTo, &hAfterTo);

		if (hNewItem != NULL)
			this->copyAllItems(&item, &hNewItem);
		else
			this->showErrorEx(NULL, "-n", "Unable to copy items.");
	}
	// xdid -o [NAME] [ID] [SWITCH] N N N [TAB] (Tooltip Text)
	else if (flags['o'] && numtok > 3) {
		TString path(input.gettok(1, TSTAB).gettok(4, -1).trim());
		HTREEITEM item;
		TString tiptext;
		
		if (input.numtok(TSTAB) > 1)
			tiptext = input.gettok(2, TSTAB).trim();

		item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, "-o", "Unable to parse path: %s", path.to_chr());
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
	else if ( flags['Q'] && numtok > 5 ) {
		TString path(input.gettok(6, -1));
		HTREEITEM item = this->parsePath(&path);
		COLORREF clrText = (COLORREF) input.gettok(5).to_num();

		if (item == NULL) {
			this->showErrorEx(NULL, "-Q", "Unable to parse path: %s", path.to_chr());
			return;
		}

		TVITEMEX tvi; 

		tvi.hItem = item;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM ; 

		if (!TreeView_GetItem(this->m_Hwnd, &tvi)) {
			this->showErrorEx(NULL, "-Q", "Unable to retrieve item: %s", path.to_chr());
			return;
		}

		LPDCXTVITEM lpdcxtvitem = (LPDCXTVITEM) tvi.lParam;

		if (lpdcxtvitem == NULL ) {
			this->showErrorEx(NULL, "-Q", "Unable to retrieve tag: %s", path.to_chr());
			return;
		}

		int iFlags = this->parseItemFlags(input.gettok(4));

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
			lpdcxtvitem->clrText = (COLORREF) -1;

		this->redrawWindow();
	}
	// This is to avoid an invalid error message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags['r']) {
	}
	// xdid -t [NAME] [ID] [SWITCH] [+FLAGS] N N N
	else if (flags['t'] && numtok > 4) {
		UINT iFlags = this->parseToggleFlags(input.gettok(4));

		if (input.gettok(5, -1) == "root") {
			HTREEITEM hStart = TVI_ROOT;

			if (iFlags & TVIE_EXPALL)
				this->expandAllItems(&hStart, TVE_EXPAND);
			else if (iFlags & TVIE_COLALL)
				this->expandAllItems(&hStart, TVE_COLLAPSE);

			return;
		}

		TString path(input.gettok(5, -1));
		HTREEITEM item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, "-t", "Unable to parse path: %s", path.to_chr());
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
	else if (flags['u']) {
		TreeView_SelectItem(this->m_Hwnd, NULL);
	}
	// xdid -v [NAME] [ID] [SWITCH] N N N [TAB] (Item Text)
	else if (flags['v'] && numtok > 3) {
		TString path(input.gettok(1, TSTAB).gettok(4, -1).trim());
		HTREEITEM item;
		TString itemtext;

		if (input.numtok(TSTAB) > 1)
			itemtext = input.gettok(2, TSTAB).trim();

		item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, "-v", "Unable to parse path: %s", path.to_chr());
			return;
		}

		TVITEMEX tvi;

		tvi.mask = TVIF_TEXT | TVIF_HANDLE;
		tvi.hItem = item;
		tvi.pszText = itemtext.to_chr();
		TreeView_SetItem(this->m_Hwnd, &tvi);
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags['w'] && numtok > 5) {
		TString flags(input.gettok(4));
		UINT iFlags = this->parseIconFlagOptions(flags);

		HIMAGELIST himl;
		HICON icon = NULL;

		int index = input.gettok( 5 ).to_int();
		TString filename(input.gettok(6, -1));

		if (this->m_iIconSize > 16)
			icon = dcxLoadIcon(index, filename, TRUE, flags);
		else
			icon = dcxLoadIcon(index, filename, FALSE, flags);

		if (iFlags & TVIT_NORMAL) {
			if ((himl = this->getImageList(TVSIL_NORMAL)) == NULL) {
				himl = this->createImageList();

				if (himl)
					this->setImageList(himl, TVSIL_NORMAL);
			}

			int i = ImageList_AddIcon(himl, icon);

			if (flags.find('o',0)) {
				// overlay image
				int io = flags.find('o',1) +1;
				int o = flags.mid(io, (flags.len() - io)).to_int();

				if (o < 1 || o > 15) {
					this->showError(NULL, "-w", "Overlay index out of range (1 -> 15)");
					o = 0;
				}

				if (o > 0)
					ImageList_SetOverlayImage(himl, i, o);
			}
		}

		if (iFlags & TVIT_STATE) {
			if ((himl = this->getImageList(TVSIL_STATE)) == NULL) {
				himl = this->createImageList();

				if (himl)
					this->setImageList(himl, TVSIL_STATE);
			}

			ImageList_AddIcon(himl, icon);
		}
		if (icon != NULL)
			DestroyIcon(icon);
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if ( flags['y'] && numtok > 3 ) {
		UINT iFlags = this->parseIconFlagOptions( input.gettok( 4 ) );

		HIMAGELIST himl;

		if ( iFlags & TVIT_NORMAL ) {

			if ( ( himl = this->getImageList( TVSIL_NORMAL ) ) != NULL ) {
				ImageList_Destroy( himl );
				this->setImageList( NULL, TVSIL_NORMAL );
			}
		}

		if ( iFlags & TVIT_STATE ) {

			if ( ( himl = this->getImageList( TVSIL_STATE ) ) != NULL ) {

				ImageList_Destroy( himl );
				this->setImageList( NULL, TVSIL_STATE );
			}
		}
	}
	// xdid -z [NAME] [ID] [SWITCH] [+FLAGS] N N N [TAB] [ALIAS]
	else if (flags['z'] && numtok > 4) {
		TString path(input.gettok(1, TSTAB).gettok(5, -1).trim());
		DCXTVSORT dtvs;
		TVSORTCB tvs;

		dtvs.iSortFlags = 0;
		dtvs.pthis = NULL;
		//ZeroMemory(&dtvs, sizeof(DCXTVSORT)); // zeromem's an object.
		dtvs.iSortFlags = this->parseSortFlags(input.gettok(4));
		dtvs.pthis = this;

		if (input.numtok(TSTAB) > 1)
			dtvs.tsCustomAlias = input.gettok(2, TSTAB).trim();

		ZeroMemory( &tvs, sizeof(TVSORTCB) );
		tvs.lpfnCompare = DcxTreeView::sortItemsEx;
		tvs.lParam = (LPARAM) &dtvs;

		if (path == "root")
			tvs.hParent = TVI_ROOT;
		else {

			HTREEITEM item = this->parsePath(&path);

			if (item == NULL) {
				this->showErrorEx(NULL, "-z", "Unable to parse path: %s", path.to_chr());
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
	else if (flags['G'] && numtok > 6) {
		TString flag(input.gettok( 4 ));
		this->m_iXOffset = input.gettok( 5 ).to_int();
		this->m_iYOffset = input.gettok( 6 ).to_int();
		TString filename(input.gettok( 7, -1));
		if (!this->m_bTransparent)
			this->removeExStyle(WS_EX_TRANSPARENT);
		if (filename != "none") {
			if (!mIRCLink.m_bUseGDIPlus)
				this->showError(NULL,"-G","GDI+ Not Supported On This Machine");
			else if (!LoadGDIPlusImage(flag, filename))
				this->showErrorEx(NULL,"-G","Unable to load Image: %s", filename.to_chr());
			else if (!this->m_bTransparent)
				this->setExStyle(WS_EX_TRANSPARENT);
		}
		this->redrawWindow();
	}
	// xdid -S [NAME] [ID] [SWITCH] [N (N...)][TAB][+FLAGS] [NAME] [FILENAME]
	else if (flags['S'] && numtok > 5) {
		if (input.numtok(TSTAB) != 2) {
			this->showError(NULL, "-S", "Invalid Command Syntax.");
			return;
		}

		TString fileData(input.gettok(2, TSTAB));

		if (fileData.numtok() < 3) {
			this->showError(NULL, "-S", "Invalid Command Syntax.");
			return;
		}

		HTREEITEM item;
		TString path(input.gettok(1, TSTAB).gettok(4, -1).trim());
		//TString flags(fileData.gettok(1));
		TString name(fileData.gettok(2).trim());
		TString filename(fileData.gettok(3, -1).trim());

		if (name.len() < 1) {
			this->showError(NULL, "-S", "Invalid dataset");
			return;
		}
		if (path.len() < 1) {
			this->showError(NULL, "-S", "Invalid path");
			return;
		}

		item = this->parsePath(&path);

		if (item == NULL) {
			this->showErrorEx(NULL, "-S", "Unable to parse path: %s", path.to_chr());
			return;
		}

		if (!this->xmlSaveTree(item, name, filename))
			this->showErrorEx(NULL, "-S", "Unable To Save Data To: <%s> %s", name.to_chr(), filename.to_chr());
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

	tvi.mask = TVIF_TEXT | TVIF_STATE | TVIF_INTEGRAL | TVIF_PARAM /*| TVIF_HANDLE*/ | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

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
		this->xmlLoadTree(hAfter, hParent, itemtext.gettok( 1 ), itemtext.gettok( 2, -1));
		return;
	}

	// parse DCX parameters
	LPDCXTVITEM lpmytvi = new DCXTVITEM;

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
		lpmytvi->clrText = (COLORREF)-1;

	if (iFlags & TVIS_BKG)
		lpmytvi->clrBkg = clrBkg;
	else
		lpmytvi->clrBkg = (COLORREF)-1;

	{
		char res[1024];
		if ((iFlags & TVIS_HASHITEM) && (itemtext.numtok() == 2)) {
			mIRCevalEX(res, 1024, "$hget(%s,%s)", itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
			itemtext = res;
		}
		else if ((iFlags & TVIS_HASHNUMBER) && (itemtext.numtok() == 2)) {
			mIRCevalEX(res, 1024,  "$hget(%s,%s).data", itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
			itemtext = res;
		}
	}

	tvi.pszText = itemtext.to_chr();
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);

	// icons
	// http://dcx.scriptsdb.org/bug/?do=details&id=350
	tvi.iImage = 10000;
	tvi.iSelectedImage = 10000;

	if (icon > -1) {
		tvi.iImage = icon;
		//tvi.mask |= TVIF_IMAGE;
	}

	if (sicon > -1) {
		tvi.iSelectedImage = sicon;
		//tvi.mask |= TVIF_SELECTEDIMAGE;
	}

	iFlags &= ~TVIS_DCXMASK; // exclude DCX flags, they were messing up state & overlay icon settings, found when saved data didnt match what was set.

	//tvi.hItem = hAfter;
	tvi.state = iFlags;
	tvi.stateMask = iFlags;
	tvi.iIntegral = integral;
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
	UINT i = 1, len = flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+') 
		return iFlags;

	while (i < len) {
		if (flags[i] == 'n')
			iFlags |= TVIT_NORMAL;
		else if (flags[i] == 's')
			iFlags |= TVIT_STATE;

		++i;
	}

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseItemFlags(const TString &flags) {
	INT i = 1, len = flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+') 
		return iFlags;

	while (i < len) {
		if (flags[i] == 'b')
			iFlags |= TVIS_BOLD;
		else if (flags[i] == 'c')
			iFlags |= TVIS_COLOR;
		else if (flags[i] == 'e')
			iFlags |= TVIS_EXPANDED;
		else if (flags[i] == 'i')
			iFlags |= TVIS_ITALIC;
		else if (flags[i] == 's')
			iFlags |= TVIS_SELECTED;
		else if (flags[i] == 'u')
			iFlags |= TVIS_UNDERLINE;
		else if (flags[i] == 'g')
			iFlags |= TVIS_BKG;
		else if (flags[i] == 'H')
			iFlags |= TVIS_HASHITEM;
		else if (flags[i] == 'n')
			iFlags |= TVIS_HASHNUMBER;
		else if (flags[i] == 'x')
			iFlags |= TVIS_XML;

		++i;
	}

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseSortFlags( const TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if ( flags[i] == 'a' )
      iFlags |= TVSS_ASC;
    else if ( flags[i] == 'b' )
      iFlags |= TVSS_SINGLE;
    else if ( flags[i] == 'c' )
      iFlags |= TVSS_CUSTOM;
    else if ( flags[i] == 'd' )
      iFlags |= TVSS_DESC;
    else if ( flags[i] == 'n' )
      iFlags |= TVSS_NUMERIC;
    else if ( flags[i] == 'r' )
      iFlags |= TVSS_ALL;
    else if ( flags[i] == 's' )
      iFlags |= TVSS_CASE;
    else if ( flags[i] == 't' )
      iFlags |= TVSS_ALPHA;

    ++i;
  }
  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseColorFlags( const TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if (flags[i] == 'b')
      iFlags |= TVCOLOR_B;
    else if (flags[i] == 'l')
      iFlags |= TVCOLOR_L;
	 else if (flags[i] == 's')
      iFlags |= TVCOLOR_S;
    else if (flags[i] == 't')
      iFlags |= TVCOLOR_T;

    ++i;
  }
  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseToggleFlags( const TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if ( flags[i] == 'a' )
      iFlags |= TVIE_EXPALL;
    else if ( flags[i] == 'c' )
      iFlags |= TVIE_COL;
    else if ( flags[i] == 'e' )
      iFlags |= TVIE_EXP;
    else if ( flags[i] == 'p' )
      iFlags |= TVIE_EXPPART;
    else if ( flags[i] == 'r' )
      iFlags |= TVIE_COLRES;
    else if ( flags[i] == 't' )
      iFlags |= TVIE_TOGGLE;
    else if ( flags[i] == 'z' )
      iFlags |= TVIE_COLALL;

    ++i;
  }
  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

int CALLBACK DcxTreeView::sortItemsEx( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort ) {

	LPDCXTVSORT ptvsort = (LPDCXTVSORT) lParamSort;
	char itemtext1[900];
	char itemtext2[900];

	LPDCXTVITEM lptvi1 = (LPDCXTVITEM) lParam1;
	LPDCXTVITEM lptvi2 = (LPDCXTVITEM) lParam2;

	ptvsort->pthis->getItemText( &lptvi1->hHandle, itemtext1, 900 );
	ptvsort->pthis->getItemText( &lptvi2->hHandle, itemtext2, 900 );

	// CUSTOM Sort
	if ( ptvsort->iSortFlags & TVSS_CUSTOM ) {

		char res[20];
		mIRCevalEX( res, 20, "$%s(%s,%s)", ptvsort->tsCustomAlias.to_chr( ), itemtext1, itemtext2 );

		int ires = atoi(res);

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

		int i1 = atoi( itemtext1 );
		int i2 = atoi( itemtext2 );

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
TString DcxTreeView::getPathFromItem(HTREEITEM *item) {
	HTREEITEM parent = *item;
	HTREEITEM current;
	TString result;
	VectorOfInts vec;
	int i;

	// Loop until we reach the topmost node of the tree.
	do {
		i = 1;
		current = parent;

		// Count each previous node before the node containing our item.
		while (current = TreeView_GetPrevSibling(this->m_Hwnd, current)) {
			i++;
		}

		vec.push_back(i);
	} while ((parent = TreeView_GetParent(this->m_Hwnd, parent)) != NULL);

	// Construct the string containing the path backwards, as we traced it backwards.
	VectorOfInts::reverse_iterator itStart = vec.rbegin( );
	VectorOfInts::reverse_iterator itEnd   = vec.rend( );

	while (itStart != itEnd) {
		if (result == "")
			result.sprintf("%d", *itStart);
		else
			result.sprintf("%s %d", result.to_chr(), *itStart);

		itStart++;
	}

	// Trim to ensure clean path.
	result.trim();

	return result;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::getItemText( HTREEITEM * hItem, char * szBuffer, const int cchTextMax ) const {

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
	char itemtext[900];
	this->getItemText(hItem, itemtext, 900);

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
  char itemtext[1000];
  lstrcpy( itemtext, "\0" );
  TVITEMEX tvi; 

  tvi.hItem = *hItem;
  tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_INTEGRAL | TVIF_PARAM | TVIF_HANDLE;
  tvi.cchTextMax = 1000;
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
					ScreenToClient(this->m_Hwnd, &tvh.pt);
					TreeView_HitTest(this->m_Hwnd, &tvh);

					if (tvh.flags & TVHT_ONITEMBUTTON) {
						TString path = this->getPathFromItem(&tvh.hItem);

						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
							this->callAliasEx(NULL, "%s,%d,%s", "buttonclick", this->getUserID(), path.to_chr());
					}
					//&& this->isStyle( TVS_CHECKBOXES )
					else if ((tvh.flags & TVHT_ONITEMSTATEICON)) {
						TString path = this->getPathFromItem(&tvh.hItem);

						if (this->isStyle(TVS_CHECKBOXES)) {
							int state = TreeView_GetCheckState(this->m_Hwnd, tvh.hItem);

							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								this->callAliasEx(NULL, "%s,%d,%d,%s", "stateclick", this->getUserID(),
									state == 0 ? 2 : 1 , path.to_chr());
							}
						}
						else {
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								this->callAliasEx(NULL, "%s,%d,%d,%s", "stateclick", this->getUserID(), 
									TreeView_GetItemState(this->m_Hwnd, tvh.hItem, TVIS_STATEIMAGEMASK), path.to_chr());
							}
						}
					}
					//|| ( ( tvh.flags & TVHT_ONITEMRIGHT ) && this->isStyle( TVS_FULLROWSELECT ) ) )
					else if (tvh.flags & TVHT_ONITEM) {
						TString path = this->getPathFromItem(&tvh.hItem);

						TreeView_SelectItem(this->m_Hwnd, tvh.hItem);

						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
							this->callAliasEx(NULL, "%s,%d,%s", "sclick", this->getUserID(), path.to_chr());
					}
					// single click not on item
					else if ((tvh.flags & TVHT_NOWHERE) || (tvh.flags & TVHT_ONITEMRIGHT)) {
						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
							this->callAliasEx(NULL, "%s,%d", "sclick", this->getUserID());
					}

					bParsed = TRUE;
					break;
				}

			case NM_DBLCLK:
				{
					TVHITTESTINFO tvh;

					GetCursorPos(&tvh.pt);
					ScreenToClient(this->m_Hwnd, &tvh.pt);
					TreeView_HitTest(this->m_Hwnd, &tvh);

					//|| ( ( tvh.flags & TVHT_ONITEMRIGHT ) && this->isStyle( TVS_FULLROWSELECT ) ) )
					if (tvh.flags & TVHT_ONITEM) {
						TString path = this->getPathFromItem(&tvh.hItem);

						TreeView_SelectItem(this->m_Hwnd, tvh.hItem);

						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
							this->callAliasEx(NULL, "%s,%d,%s", "dclick", this->getUserID(), path.to_chr());
					}

					bParsed = TRUE;
					break;
				}

			case NM_RCLICK:
				{
					TVHITTESTINFO tvh;

					GetCursorPos(&tvh.pt);
					ScreenToClient(this->m_Hwnd, &tvh.pt);
					TreeView_HitTest(this->m_Hwnd, &tvh);

					//|| ( ( tvh.flags & TVHT_ONITEMRIGHT ) && this->isStyle( TVS_FULLROWSELECT ) ) )
					if (tvh.flags & TVHT_ONITEM) {
						TString path = this->getPathFromItem(&tvh.hItem);

						TreeView_SelectItem(this->m_Hwnd, tvh.hItem);

						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
							this->callAliasEx(NULL, "%s,%d,%s", "rclick", this->getUserID(), path.to_chr());
					}
					else if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
						this->callAliasEx( NULL, "%s,%d", "rclick", this->getUserID());

					bParsed = TRUE;
				}
				break;

			case TVN_SELCHANGED:
			{
				LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW) lParam;

				if (lpnmtv != NULL) {
					TString path = this->getPathFromItem(&lpnmtv->itemNew.hItem);
					this->callAliasEx(NULL, "%s,%d,%s", "selchange", this->getUserID(), path.to_chr());
				}

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

					if (lpnmtv->action & TVE_COLLAPSE) {
						TString path(this->getPathFromItem(&lpnmtv->itemNew.hItem));
						this->callAliasEx(NULL, "%s,%d,%s", "collapse", this->getUserID(), path.to_chr());
					}
					else if (lpnmtv->action & TVE_EXPAND) {
						TString path(this->getPathFromItem(&lpnmtv->itemNew.hItem));
						this->callAliasEx(NULL, "%s,%d,%s", "expand", this->getUserID(), path.to_chr());
					}

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
					SetProp( edit_hwnd, "dcx_pthis", (HANDLE) this );

					char ret[256];
					this->callAliasEx( ret, "%s,%d", "labelbegin", this->getUserID( ) );

					if ( !lstrcmp( "noedit", ret ) )
						return TRUE;

					return FALSE;
				}
				break;
			case TVN_ENDLABELEDIT:
				{
					bParsed = TRUE;

					LPNMTVDISPINFO lptvdi = (LPNMTVDISPINFO) lParam;

					if ( lptvdi->item.pszText == NULL )
						this->callAliasEx( NULL, "%s,%d", "labelcancel", this->getUserID( ) );
					else {
						char ret[256];
						this->callAliasEx( ret, "%s,%d,%s", "labelend", this->getUserID( ), lptvdi->item.pszText );

						if ( !lstrcmp( "noedit", ret ) )
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

							bool bSelected = (lpntvcd->nmcd.uItemState & CDIS_SELECTED);

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
							//char buf[900];
							//ZeroMemory(&tvitem,sizeof(tvitem));
							//tvitem.hItem = (HTREEITEM)lpntvcd->nmcd.dwItemSpec;
							//tvitem.mask = TVIF_TEXT;
							//tvitem.pszText = buf;
							//tvitem.cchTextMax = 900;
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
							BOOL state = TreeView_GetCheckState(this->m_Hwnd, htvi);
							//this->callAliasEx( NULL, "%s,%d,%d,%d", "stateclick", this->getUserID( ), (state ? 0 : 1), TreeView_MapHTREEITEMToAccID(this->m_Hwnd, htvi) );
							this->callAliasEx( NULL, "%s,%d,%d,%s", "stateclick", this->getUserID( ), (state ? 0 : 1), this->getPathFromItem(&htvi).to_chr() );
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

	DcxTreeView * pthis = (DcxTreeView *) GetProp( mHwnd, "dcx_pthis" );

	switch( uMsg ) {

	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;

	case WM_DESTROY:
		{
			RemoveProp( mHwnd, "dcx_pthis" );
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
	if (mIRCLink.m_bUseGDIPlus && this->m_pImage != NULL)
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
		this->showErrorEx(NULL,"LoadGDIPlusImage", "Unable to Access File: %s", filename.to_chr());
		return false;
	}
	this->m_pImage = new Image(filename.to_wchr(),TRUE);

	// couldnt allocate image object.
	if (this->m_pImage == NULL) {
		this->showError(NULL,"LoadGDIPlusImage", "Couldn't allocate image object.");
		return false;
	}
	// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
	Status status = this->m_pImage->GetLastStatus();
	if (status != Ok) {
		this->showError(NULL,"LoadGDIPlusImage", GetLastStatusStr(status));
		PreloadData();
		return false;
	}

	if (flags.find('h',0)) { // High Quality
		this->m_CQuality = CompositingQualityHighQuality;
		this->m_IMode = InterpolationModeHighQualityBicubic;
	}
	else {
		this->m_CQuality = CompositingQualityDefault;
		this->m_IMode = InterpolationModeDefault;
	}

	if (flags.find('b',0)) // Blend Image
		this->m_CMode = CompositingModeSourceOver;
	else
		this->m_CMode = CompositingModeSourceCopy;

	if (flags.find('a',0)) // Anti-Aliased
		this->m_SMode = SmoothingModeAntiAlias;
	else
		this->m_SMode = SmoothingModeDefault;

	if (flags.find('t',0)) // Tile
		this->m_bTileImage = true;
	else
		this->m_bTileImage = false;

	if (flags.find('r',0)) // Tile
		this->m_bResizeImage = true;
	else
		this->m_bResizeImage = false;

	return true;
}
void DcxTreeView::DrawGDIPlusImage(HDC hdc)
{
	RECT rc;
	GetClientRect(this->m_Hwnd, &rc);

	int w = (rc.right - rc.left), h = (rc.bottom - rc.top), x = rc.left, y = rc.top;
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
		this->showErrorEx(NULL, NULL, "Unable To Access File: %s", filename.to_chr());
		return false;
	}

	TiXmlDocument doc(filename.to_chr());
	doc.SetCondenseWhiteSpace(false);

	bool xmlFile = doc.LoadFile();
	if (!xmlFile) {
		this->showErrorEx(NULL, "-a", "Not an XML File: %s", filename.to_chr());
		return false;
	}

	TiXmlElement *xRoot = doc.FirstChildElement("dcxml");
	if (xRoot == NULL) {
		this->showError(NULL, "-a", "Unable Find 'dcxml' root");
		return false;
	}

	TiXmlElement *xElm = xRoot->FirstChildElement("treeview_data");
	if (xElm == NULL) {
		this->showError(NULL, "-a", "Unable To Find 'treeview_data' element");
		return false;
	}

	xElm = xElm->FirstChildElement(name.to_chr());
	if (xElm == NULL) {
		this->showErrorEx(NULL, "-a", "Unable To Find Dataset: %s", name.to_chr());
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
	TiXmlDocument doc(filename.to_chr());
	doc.SetCondenseWhiteSpace(false);

	if (IsFile(filename)) {
		bool xmlFile = doc.LoadFile();
		if (!xmlFile) {
			this->showErrorEx(NULL, "-S", "Not an XML File: %s", filename.to_chr());
			return false;
		}
	}
	TiXmlElement *xRoot = doc.FirstChildElement("dcxml");
	if (xRoot == NULL) {
		xRoot = (TiXmlElement *)doc.InsertEndChild(TiXmlElement("dcxml"));

		if (xRoot == NULL) {
			this->showErrorEx(NULL, "-S", "Unable To Add Root <dcxml>");
			return false;
		}
	}

	TiXmlElement *xElm = xRoot->FirstChildElement("treeview_data");
	if (xElm == NULL) {
		xElm = (TiXmlElement *)xRoot->InsertEndChild(TiXmlElement("treeview_data"));
		if (xElm == NULL)
			return false;
	}

	xElm = xElm->FirstChildElement(name.to_chr());
	if (xElm == NULL) {
		xElm = (TiXmlElement *)xElm->InsertEndChild(TiXmlElement(name.to_chr()));
		if (xElm == NULL)
			return false;
	}

	xElm->Clear();

	if ( hFromItem == TVI_ROOT)
		hFromItem = TreeView_GetRoot(this->m_Hwnd);

	PTCHAR buf = new TCHAR[1024];
	if (buf != NULL) {
		if (!this->xmlGetItems(hFromItem, xElm, buf)) {
			this->showErrorEx(NULL, "-S", "Unable To Add Items to XML");
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
		tvi.cchTextMax = 1024;
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
			xChild.SetAttribute("text", tvi.pszText);
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
				xChild.SetAttribute("tooltip", lpmytvi->tsTipText.to_chr());
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
TiXmlElement *DcxTreeView::xmlInsertItems(HTREEITEM hParent, HTREEITEM &hInsertAfter, TiXmlElement *xElm)
{
	TVINSERTSTRUCT tvins;
	int i = 0;
	const char *attr = NULL;
	TiXmlElement *xRes = NULL;

	for (TiXmlElement *xNode = xElm->FirstChildElement("tvitem"); xNode != NULL; xNode = xNode->NextSiblingElement("tvitem")) {
		ZeroMemory(&tvins, sizeof(tvins));
		tvins.hInsertAfter = hInsertAfter;
		tvins.hParent = hParent;
		LPDCXTVITEM lpmytvi = new DCXTVITEM;

		lpmytvi->hHandle = NULL;

#ifdef DCX_USE_WINSDK
		tvins.itemex.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE;
#else
		tvins.itemex.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
#endif
		tvins.itemex.lParam = (LPARAM)lpmytvi;

		// Items Icon.
		attr = xNode->Attribute("image",&i);
		if (attr != NULL && i > 0)
			tvins.itemex.iImage = i -1;
		else
			tvins.itemex.iImage = 10000;
		// Items Selected state icon.
		attr = xNode->Attribute("selectedimage",&i);
		if (attr != NULL && i > 0)
			tvins.itemex.iSelectedImage = i -1;
		else
			tvins.itemex.iSelectedImage = tvins.itemex.iImage;
		// Items expanded state icon.
#ifdef DCX_USE_WINSDK
		attr = xNode->Attribute("expandedimage",&i);
		if (attr != NULL && i > 0)
			tvins.itemex.iExpandedImage = i -1;
		else
			tvins.itemex.iExpandedImage = 10000;
#endif
		// Items height integral.
		attr = xNode->Attribute("integral",&i);
		if (attr != NULL && i > 0) {
			tvins.itemex.iIntegral = i;
			tvins.itemex.mask |= TVIF_INTEGRAL;
		}
		// Items selected
		attr = xNode->Attribute("selected",&i);
		if (attr != NULL && i > 0) {
			tvins.itemex.state |= TVIS_SELECTED;
			tvins.itemex.stateMask |= TVIS_SELECTED;
		}
		// Items tooltip
		attr = xNode->Attribute("tooltip");
		if (attr != NULL) {
			lpmytvi->tsTipText = attr;
		}
		// Items text colour.
		attr = xNode->Attribute("textcolor",&i);
		if (attr != NULL && i > -1) {
			lpmytvi->clrText = (COLORREF)i;
		}
		else
			lpmytvi->clrText = CLR_INVALID;
		// Items background colour.
		attr = xNode->Attribute("backgroundcolor",&i);
		if (attr != NULL && i > -1) {
			lpmytvi->clrBkg = (COLORREF)i;
		}
		else
			lpmytvi->clrBkg = CLR_INVALID;
		// Is Item text in Bold?
		attr = xNode->Attribute("textbold",&i);
		if (i > 0) {
			lpmytvi->bBold = TRUE;
			tvins.itemex.state |= TVIS_BOLD;
			tvins.itemex.stateMask |= TVIS_BOLD;
		}
		else
			lpmytvi->bBold = FALSE;
		// Is item text in italics?
		attr = xNode->Attribute("textitalic",&i);
		lpmytvi->bItalic = (i > 0);
		// Is item text underlined?
		attr = xNode->Attribute("textunderline",&i);
		lpmytvi->bUline = (i > 0);
		// Items Text.
		attr = xNode->Attribute("text");
		if (attr != NULL) {
			tvins.itemex.cchTextMax = lstrlen(attr);
			tvins.itemex.pszText = (LPSTR)attr;
			tvins.itemex.mask |= TVIF_TEXT;
		}
		hInsertAfter = TreeView_InsertItem(this->m_Hwnd, &tvins);
		if (hInsertAfter == NULL) {
			delete lpmytvi;
			this->showError(NULL, "-a", "Unable To Add XML Item To TreeView");
			return NULL;
		}
		lpmytvi->hHandle = hInsertAfter;
		// Items state icon.
		attr = xNode->Attribute("state",&i);
		if (attr != NULL && i > -1 && i < 5) // zero means no state icon anyway.
			TreeView_SetItemState(this->m_Hwnd, hInsertAfter, INDEXTOSTATEIMAGEMASK(i), TVIS_STATEIMAGEMASK);
		// Items overlay icon.
		// overlay is 1-based index
		attr = xNode->Attribute("overlay",&i);
		if (attr != NULL && i > 0 && i < 16)
			TreeView_SetItemState(this->m_Hwnd, hInsertAfter, INDEXTOOVERLAYMASK(i), TVIS_OVERLAYMASK);

		if (xNode->FirstChild("tvitem") != NULL) {
			// item has children.
			xNode = this->xmlInsertItems(hInsertAfter, hInsertAfter, xNode);
		}
		xRes = xNode;
	}
	return xRes;
}

/*
 * Retrieves an item from the path specified.
 *
 * If the path is invalid, it will return NULL.
 * If hParent and hInsertAt parameters are filled, it is assumed we are searching for a place to insert the item
 */
HTREEITEM DcxTreeView::parsePath(const TString *path, HTREEITEM *hParent, HTREEITEM *hInsertAt) {
	HTREEITEM current = TVI_ROOT;
	HTREEITEM foundSoFar = current;
	int count = path->numtok();
	int dir;
	int i;                             // iterator for counting the Nth directory
	int k = 1;                         // iterator for path directories
	bool bFillLocation = ((hParent != NULL) && (hInsertAt != NULL));

	// Invalid path - no directives.
	if (count == 0)
		return NULL;

	// Iterate through each directive in the path string
	while (k <= count) {
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

			if ((bFillLocation) && (++i == dir))
				*hInsertAt = current;
		}

		// Couldnt find specified path.
		if (current == NULL)
			return NULL;
		// Inch forward on next item.
		else
			foundSoFar = current;

		k++;
	}

	return foundSoFar;
}
