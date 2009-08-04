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

DcxListView::DcxListView( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles )
: DcxControl( ID, p_Dialog )
, m_bDrag(false)
, m_hItemFont(NULL)
, m_hOldItemFont(NULL)
, m_OrigEditProc(NULL)
, m_iSelectedItem(0)
, m_iSelectedSubItem(0)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles,
		DCX_LISTVIEWCLASS,
		NULL,
		WS_CHILD | (Styles & ~WS_CLIPCHILDREN), // can't be ws_clipchildren as this causes render bug when progressbar items are selected.
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw "Unable To Create Window";

	if ( bNoTheme )
		Dcx::XPPlusModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	SendMessage( this->m_Hwnd, CCM_SETVERSION, (WPARAM) 5, (LPARAM) 0 );

	this->parseListviewExStyles( styles, &ExStyles);

	ListView_SetExtendedListViewStyleEx( this->m_Hwnd, ExStyles, ExStyles);

	this->m_ToolTipHWND = ListView_GetToolTips(this->m_Hwnd);

	if (this->m_ToolTipHWND != NULL) {
		if (styles.istok("balloon"))
			SetWindowLong(this->m_ToolTipHWND,GWL_STYLE,GetWindowStyle(this->m_ToolTipHWND) | TTS_BALLOON);
		//if (styles.istok("tooltips")) {
		//	this->m_ToolTipHWND = p_Dialog->getToolTip();
		//	AddToolTipToolInfo(this->m_ToolTipHWND,this->m_Hwnd);
		//}
	}

	this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );

	DragAcceptFiles(this->m_Hwnd, TRUE);
}

/*!
* \brief blah
*
* blah
*/

DcxListView::~DcxListView( ) {

	ListView_DeleteAllItems( this->m_Hwnd );

	ImageList_Destroy( this->getImageList( LVSIL_NORMAL ) );
	ImageList_Destroy( this->getImageList( LVSIL_SMALL ) );
	ImageList_Destroy( this->getImageList( LVSIL_STATE ) );

	/*
	for (int i = 0; i < (int) m_lvpbars.size(); i++) {
	DestroyWindow(m_lvpbars[i].pbar->getHwnd());
	//delete m_lvpbars[i].pbar;
	}

	m_lvpbars.clear();
	*/

	this->unregistreDefaultWindowProc( );
}

TString DcxListView::getStyles(void) {
	TString styles(__super::getStyles());
	DWORD Styles;
	DWORD ExStyles;
	Styles = GetWindowStyle(this->m_Hwnd);
	ExStyles = ListView_GetExtendedListViewStyle(this->m_Hwnd);
	if (Styles & LVS_REPORT)
		styles.addtok("report");
	else if (Styles & LVS_LIST)
		styles.addtok("list");
	if (Styles & LVS_ICON)
		styles.addtok("icon");
	if (Styles & LVS_SMALLICON)
		styles.addtok("smallicon");
	if (Styles & LVS_NOCOLUMNHEADER)
		styles.addtok("noheader");
	if (Styles & LVS_ALIGNLEFT)
		styles.addtok("alignleft");
	if (Styles & LVS_ALIGNTOP)
		styles.addtok("aligntop");
	if (Styles & LVS_AUTOARRANGE)
		styles.addtok("autoarrange");
	if (Styles & LVS_NOLABELWRAP)
		styles.addtok("nolabelwrap");
	if (Styles & LVS_SHOWSELALWAYS)
		styles.addtok("showsel");
	if (Styles & LVS_SINGLESEL)
		styles.addtok("singlesel");
	if (Styles & LVS_EDITLABELS)
		styles.addtok("editlabel");
	if (Styles & LVS_SORTASCENDING)
		styles.addtok("sortasc");
	if (Styles & LVS_SORTDESCENDING)
		styles.addtok("sortdesc");
	if (Styles & LVS_NOSCROLL)
		styles.addtok("noscroll");
	if (Styles & LVS_NOSORTHEADER)
		styles.addtok("noheadersort");
	if (ExStyles & LVS_EX_GRIDLINES)
		styles.addtok("grid");
	if (ExStyles & LVS_EX_BORDERSELECT)
		styles.addtok("borderselect");
	if (ExStyles & LVS_EX_FLATSB)
		styles.addtok("flatsb");
	if (ExStyles & LVS_EX_FULLROWSELECT)
		styles.addtok("fullrow");
	if (ExStyles & LVS_EX_CHECKBOXES)
		styles.addtok("checkbox");
	if (ExStyles & LVS_EX_HEADERDRAGDROP)
		styles.addtok("headerdrag");
	if (ExStyles & LVS_EX_TRACKSELECT)
		styles.addtok("hottrack");
	if (ExStyles & LVS_EX_ONECLICKACTIVATE)
		styles.addtok("oneclick");
	if (ExStyles & LVS_EX_TWOCLICKACTIVATE)
		styles.addtok("twoclick");
	if (ExStyles & LVS_EX_UNDERLINEHOT)
		styles.addtok("underlinehot");
	if (ExStyles & LVS_EX_UNDERLINECOLD)
		styles.addtok("underlinecold");
	if (ExStyles & LVS_EX_SUBITEMIMAGES)
		styles.addtok("subitemimage");
	if ((ExStyles & LVS_EX_LABELTIP) && (ExStyles & LVS_EX_INFOTIP))
		styles.addtok("tooltip");
	return styles;
}

/*!
* \brief blah
*
* blah
*/

void DcxListView::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	//*ExStyles |= LVS_EX_SUBITEMIMAGES;
	//*Styles |= LVS_SINGLESEL;

	*ExStyles = WS_EX_CLIENTEDGE;

	unsigned int i = 1, numtok = styles.numtok( );

	while ( i <= numtok ) {

		if ( styles.gettok( i ) == "report" ) 
			*Styles |= LVS_REPORT;
		else if ( styles.gettok( i ) == "icon" ) 
			*Styles |= LVS_ICON;
		else if ( styles.gettok( i ) == "smallicon" ) 
			*Styles |= LVS_SMALLICON;
		else if ( styles.gettok( i ) == "list" ) 
			*Styles |= LVS_LIST;
		else if ( styles.gettok( i ) == "noheader" ) 
			*Styles |= LVS_NOCOLUMNHEADER;
		else if ( styles.gettok( i ) == "alignleft" ) 
			*Styles |= LVS_ALIGNLEFT;
		else if ( styles.gettok( i ) == "aligntop" ) 
			*Styles |= LVS_ALIGNTOP;
		else if ( styles.gettok( i ) == "autoarrange" ) 
			*Styles |= LVS_AUTOARRANGE;
		else if ( styles.gettok( i ) == "nolabelwrap" ) 
			*Styles |= LVS_NOLABELWRAP;
		else if ( styles.gettok( i ) == "showsel" ) 
			*Styles |= LVS_SHOWSELALWAYS;
		else if ( styles.gettok( i ) == "singlesel" ) 
			*Styles |= LVS_SINGLESEL;
		else if ( styles.gettok( i ) == "editlabel" ) 
			*Styles |= LVS_EDITLABELS;
		else if ( styles.gettok( i ) == "sortasc" ) 
			*Styles |= LVS_SORTASCENDING;
		else if ( styles.gettok( i ) == "sortdesc" ) 
			*Styles |= LVS_SORTDESCENDING;
		else if ( styles.gettok( i ) == "noscroll" ) 
			*Styles |= LVS_NOSCROLL;
		else if ( styles.gettok( i ) == "noheadersort" ) 
			*Styles |= LVS_NOSORTHEADER;

		i++;
	}
	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

void DcxListView::parseListviewExStyles( const TString & styles, LONG * ExStyles )
{
	//*ExStyles = 0;
	//if (!Dcx::XPPlusModule.isUseable())
	*ExStyles = LVS_EX_DOUBLEBUFFER;

	unsigned int i = 1, numtok = styles.numtok( );

	while ( i <= numtok )
	{
		if ( styles.gettok( i ) == "grid" ) 
			*ExStyles |= LVS_EX_GRIDLINES;
		else if ( styles.gettok( i ) == "borderselect" ) 
			*ExStyles |= LVS_EX_BORDERSELECT;
		else if ( styles.gettok( i ) == "flatsb" ) 
			*ExStyles |= LVS_EX_FLATSB;
		else if ( styles.gettok( i ) == "fullrow" ) 
			*ExStyles |= LVS_EX_FULLROWSELECT;
		else if ( styles.gettok( i ) == "checkbox" ) 
			*ExStyles |= LVS_EX_CHECKBOXES;
		else if ( styles.gettok( i ) == "headerdrag" ) 
			*ExStyles |= LVS_EX_HEADERDRAGDROP;
		else if ( styles.gettok( i ) == "hottrack" ) 
			*ExStyles |= LVS_EX_TRACKSELECT;
		else if ( styles.gettok( i ) == "oneclick" ) 
			*ExStyles |= LVS_EX_ONECLICKACTIVATE;
		else if ( styles.gettok( i ) == "twoclick" ) 
			*ExStyles |= LVS_EX_TWOCLICKACTIVATE;
		else if ( styles.gettok( i ) == "underlinehot" ) 
			*ExStyles |= LVS_EX_UNDERLINEHOT;
		else if ( styles.gettok( i ) == "underlinecold" ) 
			*ExStyles |= LVS_EX_UNDERLINECOLD;
		else if ( styles.gettok( i ) == "subitemimage" ) 
			*ExStyles |= LVS_EX_SUBITEMIMAGES;
		else if ( styles.gettok( i ) == "tooltip" )
			*ExStyles |= LVS_EX_LABELTIP | LVS_EX_INFOTIP;

		i++;
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

void DcxListView::parseInfoRequest(TString &input, char *szReturnValue) {
	int numtok = input.numtok( );

	TString prop(input.gettok( 3 ));
	
	// [NAME] [ID] [PROP] [N] [NSUB]
	if (prop == "columns") {
		// if its a report listview and it has headers
		if (!this->isStyle(LVS_NOCOLUMNHEADER) &&
			(this->isListViewStyle(LVS_REPORT))) {
				HWND hHeader = (HWND) ListView_GetHeader(this->m_Hwnd);

				if (!hHeader) {
					this->showError("columns",NULL,"could not find header");
					return;
				}

				wsprintf(szReturnValue, "%d", (int) Header_GetItemCount(hHeader));
		}
		else
			lstrcpy(szReturnValue, "0");

		return;
	}
	// [NAME] [ID] [PROP] (N)
	else if (prop == "columnorder") {
		if ((!this->isListViewStyle(LVS_REPORT)) || (this->isStyle(LVS_NOCOLUMNHEADER))) {
			return;
		}

		// if its a report listview and it has headers
		int count = this->getColumnCount();
		int *val = new int[count];
		int col = (numtok > 3 ? input.gettok(3).to_int() -1 : -1);

		// invalid column
		if ((col < -1) || (col >= count)) {
			delete [] val;
			return;
		}

		ListView_GetColumnOrderArray(this->m_Hwnd, count, val);

		// increase each value by 1 for easy user indexing
		for (int i = 0; i < count; i++)
			val[i]++;

		// get specific column
		if (col > -1) {
			wsprintf(szReturnValue, "%s", val[col]);
			delete [] val;
			return;
		}

		// collect all values
		TString buff;

		for (int i = 0; i < count; i++)
			buff.sprintf("%s %d", buff.to_chr(), val[i]);

		wsprintf(szReturnValue, "%s", buff.trim().to_chr());
		delete [] val;
		return;
	}
	// [NAME] [ID] [PROP] [N] (NSUB)
	else if (prop == "text" && numtok > 3) {
		int nItem = input.gettok(4).to_int() - 1;
		int nSubItem = 0;
		
		if (numtok > 4)
			nSubItem = input.gettok(5).to_int() -1;

		if ((nItem > -1) && (nSubItem > -1) && (nItem < ListView_GetItemCount(this->m_Hwnd))) {
			ListView_GetItemText( this->m_Hwnd, nItem, nSubItem, szReturnValue, 900 );
			return;
		}
	}
	// [NAME] [ID] [PROP] [N] [NSUB]
	else if ( prop == "icon" && numtok > 4 ) {

		int nItem = input.gettok( 4 ).to_int( ) - 1;
		int nSubItem = input.gettok(5).to_int() -1;

		if ((nItem > -1) && (nSubItem > -1) &&
			(nItem < ListView_GetItemCount(this->m_Hwnd)) &&
			(nSubItem < this->getColumnCount())) {

			LVITEM lvi;
			lvi.mask = LVIF_IMAGE;
			lvi.iItem = nItem;
			lvi.iSubItem = nSubItem;

			ListView_GetItem( this->m_Hwnd, &lvi );
			wsprintf( szReturnValue, "%d", lvi.iImage + 1 );
			return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if (prop == "selected" && (numtok > 3)) {
		int nItem = input.gettok(4).to_int();

		// In range
		if ((nItem > 0) && (nItem < ListView_GetItemCount(this->m_Hwnd))) {
			BOOL selected = ListView_GetItemState(this->m_Hwnd, nItem -1, LVIS_SELECTED) & LVIS_SELECTED;
			wsprintf(szReturnValue, "%s", (selected ? "$true" : "$false"));
			return;
		}
	}
	// [NAME] [ID] [PROP] (NSUB)
	else if ( prop == "seltext") {
		int nItem = ListView_GetNextItem( this->m_Hwnd, -1, LVIS_SELECTED );
		int nSubItem = 0;
		
		if (numtok > 3)
			nSubItem = input.gettok(4).to_int() -1;

		if ((nItem > -1) && (nSubItem > -1)) {
			ListView_GetItemText( this->m_Hwnd, nItem, nSubItem, szReturnValue, 900 );
			return;
		}
	}
	// [NAME] [ID] [PROP] (N)
	else if (prop == "sel") {
		if (this->isStyle(LVS_SINGLESEL)) {
			int nItem = ListView_GetNextItem(this->m_Hwnd, -1, LVIS_SELECTED);

			if (nItem > -1) {
				wsprintf(szReturnValue, "%d", nItem + 1);
				return;
			}
		}
		// multi select
		else {
			int nSelItems = ListView_GetSelectedCount(this->m_Hwnd);
			int nItem = -1;

			// if we want a specific index
			if (numtok > 3) {
				int n = input.gettok( 4 ).to_int();

				// sel index out of bounds
				if (n > nSelItems)
					return;

				// return total count of selected files
				if (n == 0) {
					wsprintf(szReturnValue, "%d", nSelItems);
					return;
				}

				while ((nItem = ListView_GetNextItem(this->m_Hwnd, nItem, LVIS_SELECTED)) != -1) {
					n--;

					// reached the index we want to return
					if (n == 0) {
						wsprintf(szReturnValue, "%d", nItem +1);
						return;
					}
				}

				// should not be here, but return to force blank value
				return;
			}

			// otherwise we want a list of indexes (comma seperated)
			if (nSelItems > 0) {
				TString list;
				int i = 1;

				while ((nItem = ListView_GetNextItem(this->m_Hwnd, nItem, LVIS_SELECTED)) != -1) {
					if (i == 1)
						list.sprintf("%d", nItem +1);
					else
						list.sprintf("%s,%d", list.to_chr(), nItem +1);

					i++;
				}

				lstrcpyn(szReturnValue, list.to_chr(), 900);
				return;
			}
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == "selnum" ) {

		wsprintf( szReturnValue, "%d", ListView_GetSelectedCount( this->m_Hwnd ) );
		return;
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == "state" && numtok > 3 ) {

		int nItem = input.gettok( 4 ).to_int( ) - 1;

		if ( nItem > -1 && nItem < ListView_GetItemCount( this->m_Hwnd ) ) {

			if ( this->isListViewStyle( LVS_REPORT ) ) {

				int state = ListView_GetItemState( this->m_Hwnd, nItem, LVIS_STATEIMAGEMASK );

				if ( state == 8192 )
					lstrcpy( szReturnValue, "2" );
				else if ( state == 4096 )
					lstrcpy( szReturnValue, "1" );
				else
					lstrcpy( szReturnValue, "0" );
			}
			else
				wsprintf( szReturnValue, "%d", ListView_GetItemState( this->m_Hwnd, nItem, LVIS_STATEIMAGEMASK ) );
		}
		return;
	}
	// [NAME] [ID] [PROP]
	else if ( prop == "num" ) {

		wsprintf( szReturnValue, "%d", ListView_GetItemCount( this->m_Hwnd ) );
		return;
	}
	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [COLUMN] [N]
	else if ( prop == "find" && numtok > 6 ) {

		TString matchtext(input.gettok(2, TSTAB).trim());
		TString params(input.gettok(3, TSTAB).trim());

		if ( matchtext.len( ) > 0 ) {

			UINT SearchType;
			TString searchMode(params.gettok(1));

			if (searchMode == "R")
				SearchType = LVSEARCH_R;
			else if (searchMode == "W")
				SearchType = LVSEARCH_W;
			else
				SearchType = LVSEARCH_E;

			int nColumn = params.gettok( 2 ).to_int( ) - 1;
			int N = params.gettok( 3 ).to_int( );

			// count total
			if ( N == 0 ) {

				int nItems = ListView_GetItemCount( this->m_Hwnd );
				int nColumns = this->getColumnCount( );
				int count = 0;

				// Search all columns
				if ( nColumn == -1 ) {

					for ( int i = 0; i < nItems; i++ ) {
						for ( int k = 0; k < nColumns; k++ ) {

							if ( this->matchItemText( i, k, &matchtext, SearchType ) )
								count++;
						}
					}
				}
				// Particular Column
				else {

					if ( nColumn < -1 || nColumn >= nColumns )
						return;

					for ( int i = 0; i < nItems; i++ ) {

						if ( this->matchItemText( i, nColumn, &matchtext, SearchType ) )
							count++;
					}
				}

				wsprintf( szReturnValue, "%d", count );
				return;
			}
			// find Nth matching
			else {

				int nItems = ListView_GetItemCount( this->m_Hwnd );
				int nColumns = this->getColumnCount( );
				int count = 0;

				// Search all columns
				if ( nColumn == -1 ) {

					for ( int i = 0; i < nItems; i++ ) {

						for ( int k = 0; k < nColumns; k++ ) {

							if ( this->matchItemText( i, k, &matchtext, SearchType ) )
								count++;

							// found Nth matching
							if ( count == N ) {

								wsprintf( szReturnValue, "%d %d", i + 1, k + 1 );
								return;
							}
						}
					}
				}
				// Particular Column
				else {

					if ( nColumn < -1 || nColumn >= nColumns )
						return;

					for ( int i = 0; i < nItems; i++ ) {

						if ( this->matchItemText( i, nColumn, &matchtext, SearchType ) )
							count++;

						// found Nth matching
						if ( count == N ) {

							wsprintf( szReturnValue, "%d %d", i + 1, nColumn + 1 );
							return;
						}
					}
				} //else
			} // else
		}
		return;
	}
	// [NAME] [ID] [PROP]
	else if ( prop == "tbitem" ) {

		if ( this->isStyle( LVS_REPORT ) || this->isStyle( LVS_LIST ) )
			wsprintf( szReturnValue, "%d %d", this->getTopIndex( ) + 1, this->getBottomIndex( ) + 1 );

		return;
	}
	else if ( prop == "mouseitem" ) {

		LVHITTESTINFO lvh;
		GetCursorPos( &lvh.pt );
		MapWindowPoints(NULL, this->m_Hwnd, &lvh.pt, 1 );
		ListView_SubItemHitTest( this->m_Hwnd, &lvh );

		if ( lvh.flags & LVHT_ONITEM )
			wsprintf( szReturnValue, "%d %d", lvh.iItem + 1, lvh.iSubItem +1 );
		else
			lstrcpy( szReturnValue, "-1 -1");

		return;
	}
	// [NAME] [ID] [PROP] (N)
	else if (prop == "hwidth") {
		int nColumn = -1;

		if (numtok > 3)
			nColumn = input.gettok(4).to_int() - 1;

		// return all columns
		if (nColumn == -1) {
			TString buff;
			int count = this->getColumnCount();

			for (int i = 0; i < count; i++) {
				buff.sprintf("%s %d", buff.to_chr(), ListView_GetColumnWidth(this->m_Hwnd, i));
			}

			wsprintf(szReturnValue, "%s", buff.trim().to_chr());
			return;
		}
		else if (nColumn > -1 && nColumn < this->getColumnCount()) {
			wsprintf(szReturnValue, "%d", ListView_GetColumnWidth(this->m_Hwnd, nColumn));
			return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == "htext" && numtok > 3 ) {

		int nColumn = input.gettok( 4 ).to_int( ) - 1;

		if ( nColumn > -1 && nColumn < this->getColumnCount( ) ) {

			LVCOLUMN lvc;
			ZeroMemory( &lvc, sizeof( LVCOLUMN ) );
			lvc.mask = LVCF_TEXT;
			lvc.cchTextMax = 900;
			lvc.pszText = szReturnValue;

			if ( ListView_GetColumn( this->m_Hwnd, nColumn, &lvc ) )
				return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == "hicon" && numtok > 3 ) {

		int nColumn = input.gettok( 4 ).to_int( ) - 1;

		if ( nColumn > -1 && nColumn < this->getColumnCount( ) ) {

			LVCOLUMN lvc;
			ZeroMemory( &lvc, sizeof( LVCOLUMN ) );
			lvc.mask = LVCF_IMAGE;

			if ( ListView_GetColumn( this->m_Hwnd, nColumn, &lvc ) ) {

				wsprintf( szReturnValue, "%d", lvc.iImage + 1 );
				return;
			}
		}
	}
	// [NAME] [ID] [PROP] [GID]
	else if ( prop == "gtext" && numtok > 3 ) {

		int GID = input.gettok( 4 ).to_int( );

		WCHAR wstr[901];

		LVGROUP lvg;
		ZeroMemory( &lvg, sizeof( LVGROUP ) );
		lvg.cbSize = sizeof( LVGROUP );
		lvg.mask = LVGF_HEADER;
		lvg.cchHeader = 900;
		lvg.pszHeader = wstr;

		if ( Dcx::XPPlusModule.isUseable( ) && ListView_GetGroupInfo( this->m_Hwnd, GID, &lvg ) != -1 ) {
			WideCharToMultiByte( CP_ACP, 0, wstr, -1, szReturnValue, 900, NULL, NULL );
			//int n = WideCharToMultiByte( CP_ACP, 0, wstr, lstrlenW( wstr ) + 1, szReturnValue, 900, NULL, NULL );
			//TString error;
			//error.sprintf("Chars %d", n );
			//mIRCError( error.to_chr() );
			//mIRCError( szReturnValue );
			return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == "genabled" ) {

		if ( Dcx::XPPlusModule.isUseable( ) && ListView_IsGroupViewEnabled( this->m_Hwnd ) )
			lstrcpy( szReturnValue, "$true" );
		else
			lstrcpy( szReturnValue, "$false" );

		return;
	}
	// [NAME] [ID] [PROP] [N] [NSUB] [PBARPROP] [PARAM]
	else if ((prop == "pbar") && (numtok > 5)) {
		/*
		int nItem = input.gettok( 4 ).to_int() -1;
		int nSubItem = input.gettok( 5 ).to_int();

		DCXLVPBAR* pbarCell = this->getPbar(nItem, nSubItem);

		if (pbarCell) {
		TString cmd = input.gettok( 1 ) + " " + input.gettok( 2 ) + " " + input.gettok( 6 );

		if (numtok > 6)
		cmd = cmd + " " + input.gettok(7, -1);

		pbarCell->pbar->parseInfoRequest(cmd, szReturnValue);
		}
		*/
		int nItem = input.gettok( 4 ).to_int() -1;
		int nSubItem = input.gettok( 5 ).to_int();

		if (nItem > -1 && nItem < ListView_GetItemCount(this->m_Hwnd)) {
			LVITEM lvi;

			ZeroMemory(&lvi, sizeof(LVITEM));
			lvi.mask = LVIF_PARAM;
			lvi.iItem = nItem;

			ListView_GetItem(this->m_Hwnd, &lvi);

			LPDCXLVITEM lvdcx = (LPDCXLVITEM) lvi.lParam;

			if (!lvdcx->pbar || lvdcx->iPbarCol != nSubItem)
				return;

			TString cmd = input.gettok( 1 ) + " " + input.gettok( 2 ) + " " + input.gettok(6, -1);
			lvdcx->pbar->parseInfoRequest(cmd, szReturnValue);

			//ListView_GetItemText(this->m_Hwnd, nItem, nSubItem, szReturnValue, 900);
			return;
		}

		return;
	}
#ifndef DCX_USE_WINSDK
	// [NAME] [ID] [PROP] [N]
	else if ( prop == "gnum" ) {
		if ( Dcx::XPPlusModule.isUseable( ) && ListView_IsGroupViewEnabled( this->m_Hwnd ) )
		{
			//int g = 0, gcount = 0;
			//while (g < 256) { if (ListView_HasGroup(this->m_Hwnd, g++)) gcount++; }
			int gcount = 0;
			for (int g = 0; g < 256; g++) { if (ListView_HasGroup(this->m_Hwnd, g)) gcount++; }
			wsprintf(szReturnValue, "%d", gcount);
			return;
		}
		else
			this->showError("gnum", NULL, "GroupView Not Enabled, Only On XP+");
	}
#else
	// [NAME] [ID] [PROP] [N]
	else if ( prop == "gnum" ) {
		if ( Dcx::XPPlusModule.isUseable( ) && ListView_IsGroupViewEnabled( this->m_Hwnd ) )
		{
			if (Dcx::VistaModule.isUseable())
				wsprintf(szReturnValue, "%d", ListView_GetGroupCount(this->m_Hwnd));
			else {
				int gcount = 0;
				for (int g = 0; g < 256; g++) { if (ListView_HasGroup(this->m_Hwnd, g)) gcount++; }
				//int g = 0, gcount = 0;
				//while (g < 256) { if (ListView_HasGroup(this->m_Hwnd, g++)) gcount++; }
				wsprintf(szReturnValue, "%d", gcount);
			}
			return;
		}
		else
			this->showError("gnum", NULL, "GroupView Not Enabled, Only On XP+");
	}
#endif
	else if ( prop == "gid" ) {
		if ( Dcx::XPPlusModule.isUseable( ) && ListView_IsGroupViewEnabled( this->m_Hwnd ) )
		{
			int iIndex = input.gettok(4).to_int() -1;

			if (iIndex > -1 && iIndex < ListView_GetItemCount(this->m_Hwnd))
			{
				LVITEM lvi;
				ZeroMemory(&lvi, sizeof(LVITEM));
				lvi.iItem = iIndex;
				lvi.mask = LVIF_GROUPID;

				if (ListView_GetItem(this->m_Hwnd, &lvi))
				{
					wsprintf(szReturnValue, "%d", lvi.iGroupId); // group id can be -2 (Not In group), -1 (groupcallback, should never be), 0+ groupid
					return;
				}
				else // Unable to find group info
					this->showError("gid", NULL, "Unable to get Group ID");
			}
			else
				this->showErrorEx("gid", NULL, "Invalid Item: %d", iIndex);
		}
		else
			this->showError("gid", NULL, "GroupView Not Enabled, Only On XP+");
	}
	// [NAME] [ID] [PROP] [ROW] [COL]
	else if (prop == "markeditem") {
		int nRow = input.gettok(4).to_int();
		int nCol = input.gettok(5).to_int();

		// 1-based indexes.
		if ((nRow < 1) || (nRow > ListView_GetItemCount(this->m_Hwnd))) {
			this->showErrorEx("markeditem", NULL, "Invalid item index %d", nRow);
			return;
		}

		if ((nCol < 1) || (nCol > this->getColumnCount())) {
			this->showErrorEx("markeditem", NULL, "Invalid column index %d", nCol);
			return;
		}

		// Convert to 0-index
		nRow--;
		nCol--;

		LVITEM lvi;
		lvi.mask = LVIF_PARAM;
		lvi.iItem = nRow;
		lvi.iSubItem = nCol;

		ListView_GetItem(this->m_Hwnd, &lvi);
		wsprintf(szReturnValue, "%s", ((LPDCXLVITEM) lvi.lParam)->tsMark.to_chr());
		return;
	}
	else if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
		return;

	szReturnValue[0] = 0;
}

void DcxListView::autoSize(const int nColumn, const TString &flags)
{
	this->autoSize(nColumn, this->parseHeaderFlags2(flags));
}

void DcxListView::autoSize(const int nColumn, const int iFlags)
{
	if (iFlags == -3) {
		int n = 0;
		ListView_SetColumnWidth(this->m_Hwnd, nColumn, LVSCW_AUTOSIZE);
		n = ListView_GetColumnWidth(this->m_Hwnd, nColumn);
		ListView_SetColumnWidth(this->m_Hwnd, nColumn, LVSCW_AUTOSIZE_USEHEADER);
		n = max(ListView_GetColumnWidth(this->m_Hwnd, nColumn),n);
		ListView_SetColumnWidth(this->m_Hwnd, nColumn, n);
	}
	else if (iFlags & LVSCW_AUTOSIZE)
		ListView_SetColumnWidth(this->m_Hwnd, nColumn, LVSCW_AUTOSIZE);
	else if (iFlags & LVSCW_AUTOSIZE_USEHEADER)
		ListView_SetColumnWidth(this->m_Hwnd, nColumn, LVSCW_AUTOSIZE_USEHEADER);
}

/*!
* \brief blah
*
* blah
*/

void DcxListView::parseCommandRequest(TString &input) {
	XSwitchFlags flags(input.gettok(3));

	int numtok = input.numtok( );

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags['r']) {
		ListView_DeleteAllItems(this->m_Hwnd);
	}

	//xdid -a [NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
	if (flags['a'] && numtok > 12) {
		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));

		TString data(input.gettok(1, TSTAB).gettok(4, -1).trim());
		int nPos = data.gettok( 1 ).to_int() -1;

		if (nPos < 0)
			nPos = ListView_GetItemCount(this->m_Hwnd);

		UINT stateFlags = this->parseItemFlags(data.gettok( 3 ));

		if (stateFlags & LVIS_XML) {
			// load all item data from an xml file.
			TString filename(data.gettok( 11, -1));
			this->xmlLoadListview(nPos, data.gettok( 10 ), filename);
			return;
		}

		if (stateFlags & LVIS_HASHTABLE) {
			// load all data from a mIRC hashtable.
			this->xLoadListview(nPos, data, "$hget(%s)", "$hget(%s,0).item", "$hget(%s,%d)", "$hget(%s,%s)");
			return;
		}

		if (stateFlags & LVIS_WINDOW) {
			// load all data from a mIRC @window.
			//this->xLoadListview(nPos, data.gettok( 10 ), data.gettok( 11 ), data.gettok( 12, -1 ), "$window(%s)", "$line(%s,0)", "$line(%s,%d)", NULL);
			this->xLoadListview(nPos, data, "$window(%s)", "$line(%s,0)", "$line(%s,%d)", NULL);
			return;
		}

		if (stateFlags & LVIS_CONTROL) {
			// load all data from another dcx control.
			this->ctrlLoadListview(nPos, data);
			return;
		}
		massSetItem(nPos, input);
	}
	// xdid -A [NAME] [ID] [SWITCH] [ROW] [COL] [+FLAGS] [INFO]
	else if (flags['A']) {
		int n = input.numtok();

		if (n < 7) {
			this->showErrorEx(NULL, "-A", "Insufficient parameters");
			return;
		}

		int nRow = input.gettok(4).to_int();
		int nCol = input.gettok(5).to_int();

		// We're currently checking 1-based indexes.
		if ((nRow < 1) || (nRow > ListView_GetItemCount(this->m_Hwnd))) {
			this->showErrorEx(NULL, "-A", "Invalid row index %d.", nRow);
			return;
		}

		if ((nCol < 1) || (nCol > this->getColumnCount())) {
			this->showErrorEx(NULL, "-A", "Invalid column index %d.", nCol);
			return;
		}

		// Convert to 0-based index.
		nRow--;
		nCol--;

		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));

		lvi.mask = LVIF_PARAM;
		lvi.iItem = nRow;
		lvi.iSubItem = nCol;

		// Couldnt retrieve info
		if (!ListView_GetItem(this->m_Hwnd, &lvi)) {
			this->showError(NULL, "-A", "Unable to get item.");
			return;
		}

		LPDCXLVITEM lviDcx = (LPDCXLVITEM) lvi.lParam;

		TString flag(input.gettok(6));
		TString info(input.gettok(7, -1));

		if (flag.find('M', 1) > 0)
			lviDcx->tsMark = info;
		else
			this->showErrorEx(NULL, "-A", "Unknown flags %s", flag.to_chr());
		
		return;
	}
	// xdid -B [NAME] [ID] [SWITCH] [N]
	else if (flags['B'] && numtok > 3) {
		int nItem = (int)input.gettok( 4 ).to_num() -1;

		// check if item supplied was 0 (now -1), last item in list
		if (nItem == -1) {
			nItem = ListView_GetItemCount(this->m_Hwnd) -1;

			if (nItem < 0) {
				this->showError(NULL,"-B", "Invalid Item: No Items in list");
				return;
			}
		}
		if (nItem < 0) {
			this->showError(NULL,"-B", "Invalid Item");
			return;
		}

		if (GetFocus() != this->m_Hwnd)
			SetFocus(this->m_Hwnd);
		ListView_EditLabel(this->m_Hwnd, nItem);
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags['c'] && numtok > 3) {
		int nItemCnt = ListView_GetItemCount(this->m_Hwnd);
		if (nItemCnt < 1) {
			this->showError(NULL, "-c", "Invalid Item: No Items in list");
			return;
		}
		if (this->isStyle(LVS_SINGLESEL)) {
			int nItem = (int)input.gettok( 4 ).to_num() -1;

			if (nItem == -1)
				nItem = nItemCnt -1;

			if (nItem > -1)
				ListView_SetItemState(this->m_Hwnd, nItem, LVIS_SELECTED, LVIS_SELECTED);
		}
		else {
			TString Ns(input.gettok( 4 ));
			int i = 1;
			int n = Ns.numtok(TSCOMMA);
			int nItems = nItemCnt--;

			while (i <= n) {
				int nItem = Ns.gettok(i, TSCOMMA).to_int() -1;

				if (nItem == -1)
					nItem = nItemCnt;

				if (nItem > -1 && nItem < nItems)
					ListView_SetItemState(this->m_Hwnd, nItem, LVIS_SELECTED, LVIS_SELECTED);

				i++;
			}
		}
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags['d'] && (numtok > 3)) {
		int nItem = (int)input.gettok( 4 ).to_num() -1;

		// check if item supplied was 0 (now -1), last item in list.
		if (nItem == -1) {
			nItem = ListView_GetItemCount(this->m_Hwnd) -1;

			if (nItem < 0) {
				this->showError(NULL,"-d", "Invalid Item: No Items in list");
				return;
			}
		}
		if (nItem < 0) {
			this->showError(NULL,"-d", "Invalid Item");
			return;
		}

		ListView_DeleteItem(this->m_Hwnd, nItem);
	}
	// xdid -g [NAME] [ID] [SWITCH] [+FLAGS] [X] [Y] (FILENAME) ([tab] watermark filename)
	else if (flags['g'] && numtok > 5) {
		LVBKIMAGE lvbki;
		ZeroMemory(&lvbki, sizeof(LVBKIMAGE));
		TString filename;
		//TString watermarkfile;

		lvbki.ulFlags = this->parseImageFlags(input.gettok( 4 ));
		lvbki.xOffsetPercent = (int)input.gettok( 5 ).to_num();
		lvbki.yOffsetPercent = (int)input.gettok( 6 ).to_num();

		if (numtok > 6) {
			filename = input.gettok(7, -1).trim();
			//filename = input.gettok(7, -1).gettok(1,TSTAB);

			//if (lvbki.ulFlags & LVBKIF_TYPE_WATERMARK) {
			//	watermarkfile = input.gettok(7,-1).gettok(2,-1,TSTAB).trim();
			//	lvbki.hbm = dcxLoadBitmap(lvbki.hbm, watermarkfile);
			//}
			lvbki.pszImage = filename.to_chr();
			lvbki.ulFlags |= LVBKIF_SOURCE_URL;
		}

		ListView_SetBkImage(this->m_Hwnd, &lvbki);
	}
	// xdid -i [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if (flags['i'] && numtok > 4) {
		UINT iColorFlags = this->parseColorFlags(input.gettok( 4 ));
		COLORREF clrColor = (COLORREF)input.gettok( 5 ).to_num();

		if (iColorFlags & LVCS_TEXT)
			ListView_SetTextColor(this->m_Hwnd, clrColor);

		if (iColorFlags & LVCS_BKG) {
			if (input.gettok( 5 ) == "none")
				ListView_SetBkColor(this->m_Hwnd, CLR_NONE);
			else
				ListView_SetBkColor(this->m_Hwnd, clrColor);
		}

		if (iColorFlags & LVCS_BKGTEXT) {
			if (input.gettok( 5 ) == "none")
				ListView_SetTextBkColor(this->m_Hwnd, CLR_NONE);
			else
				ListView_SetTextBkColor(this->m_Hwnd, clrColor);
		}

		if (iColorFlags & LVCS_OUTLINE)
			ListView_SetOutlineColor(this->m_Hwnd, clrColor);

		this->redrawWindow();
	}
	// xdid -j [NAME] [ID] [SWITCH] [ROW] [COL] [FLAGS] ([COLOUR] (BGCOLOUR))
	else if (flags['j'] && numtok > 5) {
		int nItem = input.gettok( 4 ).to_int() -1;
		int nCol = input.gettok( 5 ).to_int() -1;
		COLORREF clrText = (COLORREF)input.gettok( 7 ).to_num();
		COLORREF clrBack = (COLORREF)input.gettok( 8 ).to_num();

		if (nItem == -1) {
			nItem = ListView_GetItemCount(this->m_Hwnd) -1;

			if (nItem < 0) {
				this->showError(NULL,"-j", "Invalid Item: No Items in list");
				return;
			}
		}

		// invalid info
		if ((nItem < 0) || (nCol < 0) || (nCol >= this->getColumnCount())) {
			this->showError(NULL,"-j", "Invalid Item");
			return;
		}

		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));

		lvi.mask = LVIF_PARAM | LVIF_STATE;
		lvi.iItem = nItem;
		lvi.iSubItem = nCol;

		// couldnt retrieve info
		if (!ListView_GetItem(this->m_Hwnd, &lvi)) {
			this->showError(NULL,"-j", "Unable to get Item.");
			return;
		}

		UINT lviflags = this->parseItemFlags(input.gettok( 6 ));
		LPDCXLVITEM lviDcx = (LPDCXLVITEM) lvi.lParam;

		if (lviDcx != NULL) {
			if ((UINT)nCol < lviDcx->vInfo.size()) {
				lviDcx->vInfo[nCol]->m_dFlags = lviflags;
				if (lviflags & LVIS_COLOR)
					lviDcx->vInfo[nCol]->m_cText = clrText;
				else
					lviDcx->vInfo[nCol]->m_cText = CLR_INVALID;

				if (lviflags & LVIS_BGCOLOR)
					lviDcx->vInfo[nCol]->m_cBg = clrBack;
				else
					lviDcx->vInfo[nCol]->m_cBg = CLR_INVALID;

				ListView_SetItemState(this->m_Hwnd, nItem, lviflags, LVIS_DROPHILITED|LVIS_FOCUSED|LVIS_SELECTED|LVIS_CUT);
			}
			else
				this->showError(NULL, "-j", "No Render Information for SubItem, More subitems than columns?");
		}
		else
			this->showError(NULL, "-j", "No DCX Item Information, somethings very wrong");
	}
	// xdid -k [NAME] [ID] [SWITCH] [STATE] [N]
	else if (flags['k'] && numtok > 4) {
		int state = (int)input.gettok( 4 ).to_num();
		int nItem = (int)input.gettok( 5 ).to_num() -1;

		// check if item supplied was 0 (now -1), last item in list.
		if (nItem == -1) {
			nItem = ListView_GetItemCount(this->m_Hwnd) -1;

			if (nItem < 0) {
				this->showError(NULL,"-k", "Invalid Item: No Items in list");
				return;
			}
		}
		if (nItem < 0) {
			this->showError(NULL,"-k", "Invalid Item");
			return;
		}

		ListView_SetItemState(this->m_Hwnd, nItem, INDEXTOSTATEIMAGEMASK(state), LVIS_STATEIMAGEMASK);
	}
	// xdid -l [NAME] [ID] [SWITCH] [N] [M] [ICON] (OVERLAY)
	else if (flags['l'] && numtok > 5) {
		int nItem    = input.gettok(4).to_int() -1;
		int nSubItem = input.gettok(5).to_int() -1;
		int nIcon    = input.gettok(6).to_int() -1;
		int nOverlay = (numtok > 6 ? input.gettok(7).to_int() : -1);

		// check if item supplied was 0 (now -1), last item in list.
		if (nItem == -1) {
			nItem = ListView_GetItemCount(this->m_Hwnd) -1;

			if (nItem < 0) {
				this->showError(NULL,"-l", "Invalid Item: No Items in list");
				return;
			}
		}

		// invalid item
		if ((nItem < 0) || (nSubItem < 0) || (nSubItem >= this->getColumnCount())) {
			this->showError(NULL,"-l", "Invalid Item");
			return;
		}

		/*
			nIcon = 0 (use no icon)
			nIcon = -1 (ignore value, just change overlay)

			overlay = 0 (no icon)
		*/
		// no icon to change
		if ((nIcon < -1) && (nOverlay < 0))
			return;

		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));

		lvi.iItem = nItem;
		lvi.iSubItem = nSubItem;

		// theres an icon to change
		if (nIcon > -2) {
			lvi.mask |= LVIF_IMAGE;
			lvi.iImage = nIcon;
		}

		if (nOverlay > -1) {
			lvi.mask |= LVIF_STATE;
			lvi.stateMask |= LVIS_OVERLAYMASK;
			lvi.state |= INDEXTOOVERLAYMASK(nOverlay);
		}

		ListView_SetItem(this->m_Hwnd, &lvi);
	}
	// xdid -m [NAME] [ID] [SWITCH] [0|1]
	else if (flags['m'] && numtok > 3) {
		if (input.gettok( 4 ) == "1")
			ListView_EnableGroupView(this->m_Hwnd, TRUE);
		else
			ListView_EnableGroupView(this->m_Hwnd, FALSE);
	}
	// xdid -n [NAME] [ID] [SWITCH] [N] [+FLAGS] (WIDTH)
	else if (flags['n'] && numtok > 4) {
		TString tsFlags(input.gettok(5));

		// manually set width
		if (tsFlags.find('m', 0)) {
			TString widths(input.gettok(6, -1));
			int count = this->getColumnCount();

			if (widths.numtok() < count) {
				this->showError(NULL, "-n", "Insufficient number of widths specified for +m flag");
				return;
			}

			for (int i = 1; i <= count; i++)
				ListView_SetColumnWidth(this->m_Hwnd, i -1, widths.gettok(i).to_int());

			return;
		}

		int nColumn = input.gettok(4).to_int() -1;
		UINT iFlags = this->parseHeaderFlags2(tsFlags);

		if (nColumn > -1 && nColumn < this->getColumnCount()) {
			if (iFlags == -3) { // +s
				int n = 0;
				ListView_SetColumnWidth(this->m_Hwnd, nColumn, LVSCW_AUTOSIZE);
				n = ListView_GetColumnWidth(this->m_Hwnd, nColumn);
				ListView_SetColumnWidth(this->m_Hwnd, nColumn, LVSCW_AUTOSIZE_USEHEADER);
				n = max(ListView_GetColumnWidth(this->m_Hwnd, nColumn),n);
				ListView_SetColumnWidth(this->m_Hwnd, nColumn, n);
			}
			else if (iFlags & LVSCW_AUTOSIZE) // +a
				ListView_SetColumnWidth(this->m_Hwnd, nColumn, LVSCW_AUTOSIZE);
			else if (iFlags & LVSCW_AUTOSIZE_USEHEADER) // +h
				ListView_SetColumnWidth(this->m_Hwnd, nColumn, LVSCW_AUTOSIZE_USEHEADER);
			else { // fixed width
				if (numtok > 5)
					ListView_SetColumnWidth(this->m_Hwnd, nColumn, input.gettok( 6 ).to_int());
				else
					this->showError(NULL, "-n", "No width specified");
			}
		}
	}
	// xdid -o [NAME] [ID] [SWITCH] [ORDER ...]
	else if (flags['o'] && numtok > 3) {
		TString ids(input.gettok(4, -1).trim());
		int count = this->getColumnCount();
		int *indexes;

		// Basic check first
		if (ids.numtok() != count) {
			this->showError(NULL, "-o", "Incorrect number of indexes.");
			return;
		}

		indexes = new int[count];

		for (int i = 0; i < count; i++)
		{

			int tmp = ids.gettok(i +1).to_int();

			if ((tmp == 0) || // invalid character
				(tmp < 0) ||  // negative
				(tmp > count)) // out of array bounds
			{
				this->showErrorEx(NULL, "-o", "Invalid index %d.", tmp);
				delete [] indexes;
				return;
			}

			indexes[i] = tmp -1;
		}

		ListView_SetColumnOrderArray(this->m_Hwnd, count, indexes);
		delete [] indexes;
	}
	// xdid -q [NAME] [ID] [SWITCH] [N] [+FLAGS] [GID] [Group Text]
	else if (flags['q'] && numtok > 6) {
		int index = (int)input.gettok( 4 ).to_num() -1;
		UINT iFlags = this->parseGroupFlags(input.gettok( 5 ));
		int gid = (int)input.gettok( 6 ).to_num();

		if (Dcx::XPPlusModule.isUseable() && index > -1 && gid > 0) {
			if (ListView_IsGroupViewEnabled(this->m_Hwnd)) {
				if (!ListView_HasGroup(this->m_Hwnd, gid)) {
					TString text(input.gettok(7, -1));

					LVGROUP lvg;
					ZeroMemory(&lvg, sizeof(LVGROUP));
					lvg.cbSize = sizeof(LVGROUP);
					lvg.mask = LVGF_ALIGN | LVGF_HEADER | LVGF_GROUPID;

					//LPWSTR wstr = new WCHAR[text.len() + 1];
					//MultiByteToWideChar(CP_ACP, 0, text.to_chr(), text.len() +1, wstr, text.len() +1);
					LPWSTR wstr = text.to_wchr(this->m_bUseUTF8); // can this buffer be deleted? or is it needed by the control? requires testing.

					lvg.iGroupId = gid;
					lvg.pszHeader = wstr;
					lvg.uAlign = iFlags;

					ListView_InsertGroup(this->m_Hwnd, index, &lvg);
				}
				else
					this->showErrorEx(NULL,"-q", "Group already exists: %d", gid);
			}
			else
				this->showError(NULL,"-q", "Can't add to a group when Group View is not enabled.");
		}
	}
	// xdid -r [NAME] [ID] [SWITCH]
	// Note: This is here to prevent an message
	else if (flags['r']) {
		//ListView_DeleteAllItems(this->m_Hwnd);
	}
	// xdid -t [NAME] [ID] [SWITCH] [+FLAGS] [#ICON] [WIDTH] (Header text) [{TAB} [+FLAGS] [#ICON] [WIDTH] Header text {TAB} ... ]
	else if (flags['t'] && numtok > 5) {
		int nCol = this->getColumnCount();

		if (nCol > 0) {
			while (--nCol > 0)
				ListView_DeleteColumn(this->m_Hwnd, nCol);
		}

		LVCOLUMN lvc;
		ZeroMemory(&lvc, sizeof(LVCOLUMN));
		int nColumn = 0;
		TString data(input.gettok(1, TSTAB).gettok(4, -1).trim());

		int icon  = data.gettok( 2 ).to_int() -1;
		int width = data.gettok( 3 ).to_int();

		TString itemtext;
		if (data.numtok( ) > 3)
			itemtext = data.gettok(4, -1);

		lvc.iImage = -1;
		lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvc.cx = width;
		lvc.fmt = this->parseHeaderFlags(data.gettok( 1 ));
		lvc.iSubItem = 0;
		lvc.pszText = itemtext.to_chr();

		if (icon > -1) {
			lvc.mask |= LVCF_IMAGE;
			lvc.iImage = icon;
		}

		if (this->getColumnCount() > 0)
			ListView_SetColumn(this->m_Hwnd, nColumn, &lvc);
		else
			ListView_InsertColumn(this->m_Hwnd, nColumn, &lvc);
		/*
		*	These flags do NOT make the columns auto size as text is added
		* They auto size the columns to match the text already present (none).
		*/
		this->autoSize(nColumn,data.gettok( 1 ));

		int tabs = input.numtok(TSTAB);

		if (tabs > 1 ) {
			int i = 2;

			while (i <= tabs) {
				nColumn++;

				data = input.gettok(i, TSTAB).trim();

				icon  = data.gettok( 2 ).to_int() -1;
				width = data.gettok( 3 ).to_int();
				itemtext = "";

				if (data.numtok( ) > 3)
					itemtext = data.gettok(4, -1);

				lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
				lvc.cx = width;
				lvc.fmt = this->parseHeaderFlags(data.gettok( 1 ));
				lvc.iImage = -1;
				lvc.iSubItem = 0;
				lvc.pszText = itemtext.to_chr();

				if (icon > -1) {
					lvc.mask |= LVCF_IMAGE;
					lvc.iImage = icon;
				}

				ListView_InsertColumn(this->m_Hwnd, nColumn, &lvc);

				this->autoSize(nColumn,data.gettok( 1 ));

				i++;
			}
		}
	}
	// xdid -u [NAME] [ID] [SWITCH]
	else if (flags['u']) {
		ListView_SetItemState(this->m_Hwnd, -1, 0, LVIS_SELECTED);
	}
	// xdid -v [NAME] [ID] [SWITCH] [N] [M] (ItemText)
	else if (flags['v'] && numtok > 4) {
		int nItem = input.gettok(4).to_int() - 1;
		int nSubItem = input.gettok(5).to_int() -1;

		// check if item supplied was 0 (now -1), last item in list.
		if (nItem == -1) {
			nItem = ListView_GetItemCount(this->m_Hwnd) -1;

			if (nItem < 0) {
				this->showError(NULL,"-v", "Invalid Item: No Items in list");
				return;
			}
		}
		if (nItem < 0) {
			this->showError(NULL,"-v", "Invalid Item");
			return;
		}

		TString itemtext(input.gettok(6, -1).trim());

		LVITEM lvi;
		LPDCXLVITEM lpdcxlvi;

		ZeroMemory(&lvi, sizeof(LVITEM));

		lvi.mask = LVIF_PARAM;
		lvi.iItem = nItem;
		//lvi.iSubItem = nSubItem;

		ListView_GetItem(this->m_Hwnd, &lvi);
		lpdcxlvi = (LPDCXLVITEM) lvi.lParam;

		if (lpdcxlvi && lpdcxlvi->pbar && lpdcxlvi->iPbarCol == nSubItem) {
			itemtext = input.gettok( 1 ) + " " + input.gettok( 2 ) + " " + itemtext;
			lpdcxlvi->pbar->parseCommandRequest(itemtext);
		}
		else if (nItem > -1 && nSubItem > -1 && nSubItem <= this->getColumnCount()) {
			ListView_SetItemText(this->m_Hwnd, nItem, nSubItem, itemtext.to_chr());
		}
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags['w'] && numtok > 5) {
		TString tflags(input.gettok( 4 ));
		UINT iFlags = this->parseIconFlagOptions(tflags);
		HIMAGELIST himl;
		HICON icon = NULL;
		int index = input.gettok( 5 ).to_int();
		TString filename(input.gettok(6, -1));
		int overlayindex = 0;

		// determine overlay index
		if (tflags.find('o',0)) {
			// overlay id offset
			int io = tflags.find('o',1) +1;
			overlayindex = tflags.mid(io, (tflags.len() - io)).to_int();

			if (overlayindex < 1 || overlayindex > 15) {
				this->showError(NULL, "-w", "Overlay index out of range (1 -> 15)");
				overlayindex = 0;
			}
		}

		

		// load both normal and small icons
		if (iFlags & LVSIL_SMALL) {
			// load normal icon
			if ((himl = this->initImageList(LVSIL_NORMAL)) == NULL) {
				this->showError(NULL, "-w", "Unable to create normal image list");
				return;
			}

			if (index < 0) {
				if (!AddFileIcons(himl, filename, true, -1)) {
					this->showErrorEx(NULL, "-w", "Unable to Add %s's Icons", filename.to_chr());
					return;
				}
			}
			else {
				icon = dcxLoadIcon(index, filename, true, tflags);

				if (icon == NULL) {
					this->showError(NULL, "-w", "Unable to load normal icon");
					return;
				}

				int i = ImageList_AddIcon(himl, icon);
				if (overlayindex > 0)
					ImageList_SetOverlayImage(himl, i, overlayindex);

				DestroyIcon(icon);
			}

			// load small icon
			if ((himl = this->initImageList(LVSIL_SMALL)) == NULL) {
				this->showError(NULL, "-w", "Unable to create small image list");
				if (icon != NULL)
					DestroyIcon(icon);
				return;
			}

			if (index < 0) {
				if (!AddFileIcons(himl, filename, false, -1)) {
					this->showErrorEx(NULL, "-w", "Unable to Add %s's Icons", filename.to_chr());
					return;
				}
			}
			else {
				icon = dcxLoadIcon(index, filename, false, tflags);

				if (icon == NULL) {
					this->showError(NULL, "-w", "Unable to load small icon");
					return;
				}

				int i = ImageList_AddIcon(himl, icon);

				if (overlayindex > 0)
					ImageList_SetOverlayImage(himl, i, overlayindex);

				DestroyIcon(icon);
			}
		}

		// state icon
		if (iFlags & LVSIL_STATE) {
			if ((himl = this->initImageList(LVSIL_STATE)) == NULL) {
				this->showError(NULL, "-w", "Unable to create state image list");
				if (icon != NULL)
					DestroyIcon(icon);
				return;
			}

			if (index < 0) {
				if (!AddFileIcons(himl, filename, false, -1)) {
					this->showErrorEx(NULL, "-w", "Unable to Add %s's Icons", filename.to_chr());
					return;
				}
			}
			else {
				icon = dcxLoadIcon(index, filename, false, tflags);

				if (icon == NULL) {
					this->showError(NULL, "-w", "Unable to load state icon");
					return;
				}

				ImageList_AddIcon(himl, icon);

				DestroyIcon(icon);
			}
		}
	}
	// xdid -W [NAME] [ID] [SWITCH] [STYLE]
	else if (flags['W'] && numtok > 3) {
		static const TString poslist("report icon smallicon list");
		TString style(input.gettok(4));
		int index = poslist.findtok(style.to_chr(), 1);
		UINT mode;

		switch (index) {
			case 1: // report
				mode = LVS_REPORT; break;

			case 2: // icon
				mode = LVS_ICON; break;

			case 3: // smallicon
				mode = LVS_SMALLICON; break;

			case 4: // list
				mode = LVS_LIST; break;

			default:
				this->showError(NULL, "-W", "Unknown style");
				return;
		}

		DWORD dwOldStyle = GetWindowStyle(this->m_Hwnd);
		dwOldStyle &= ~LVS_TYPEMASK; // Remove any of the flags indicating current styles
		dwOldStyle |= mode; // Specify the style we want to switch to
		SetWindowLong(this->m_Hwnd, GWL_STYLE, dwOldStyle);
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if (flags['y'] && numtok > 3) {
		UINT iFlags = this->parseIconFlagOptions(input.gettok( 4 ));
		HIMAGELIST himl;

		if (iFlags & LVSIL_SMALL) {
			if ((himl = this->getImageList(LVSIL_SMALL)) != NULL) {
				ImageList_Destroy(himl);
				this->setImageList(NULL, LVSIL_SMALL);
			}

			if ((himl = this->getImageList(LVSIL_NORMAL)) != NULL) {
				ImageList_Destroy(himl);
				this->setImageList(NULL, LVSIL_NORMAL);
			}
		}

		if (iFlags & LVSIL_STATE) {
			if ((himl = this->getImageList(LVSIL_STATE)) != NULL) {
				ImageList_Destroy(himl);
			}
		}
	}
	// xdid -z [NAME] [ID] [SWITCH] [+FLAGS] [N] (ALIAS)
	else if (flags['z'] && numtok > 4) {
		DCXLVSORT lvsort;
		int nColumn = input.gettok( 5 ).to_int() -1;

		lvsort.m_Hwnd = this->m_Hwnd;
		lvsort.iSortFlags = this->parseSortFlags(input.gettok( 4 ));
		lvsort.nColumn = nColumn;

		if (nColumn < 0 || nColumn >= this->getColumnCount())
			return;

		if (lvsort.iSortFlags & LVSS_CUSTOM && numtok < 6)
			return;
		else
			lvsort.tsCustomAlias = input.gettok( 6 );

		ListView_SortItemsEx(this->m_Hwnd, DcxListView::sortItemsEx, &lvsort);
	}
	// xdid -T [NAME] [ID] [SWITCH] [nItem] [nSubItem] (ToolTipText)
	// atm this only seems works for subitem 0. Mainly due to the callback LVN_GETINFOTIP only being sent for sub 0.
	else if (flags['T'] && numtok > 4) {
		input.trim();
		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));

		lvi.iItem = input.gettok(4).to_int() -1;
		lvi.iSubItem = input.gettok(5).to_int();
		lvi.mask = LVIF_PARAM;

		// check if item supplied was 0 (now -1), last item in list.
		if (lvi.iItem == -1) {
			lvi.iItem = ListView_GetItemCount(this->m_Hwnd) -1;

			if (lvi.iItem < 0) {
				this->showError(NULL,"-T", "Invalid Item: No Items in list");
				return;
			}
		}

		if ((lvi.iItem < 0) || (lvi.iSubItem < 0)) {
			this->showErrorEx(NULL, "-T", "Invalid Item: %d Subitem: %d", lvi.iItem +1, lvi.iSubItem);
			return;
		}

		if (ListView_GetItem(this->m_Hwnd, &lvi)) {
			LPDCXLVITEM lpmylvi = (LPDCXLVITEM) lvi.lParam;

			if (lpmylvi != NULL) {
				lpmylvi->tsTipText = (numtok > 5 ? input.gettok(6, -1) : "");
				LVSETINFOTIP it;
				ZeroMemory(&it, sizeof(it));
				it.cbSize = sizeof(it);
				it.iItem = lvi.iItem;
				it.iSubItem = lvi.iSubItem;
				it.pszText = lpmylvi->tsTipText.to_wchr(this->m_bUseUTF8);
				ListView_SetInfoTip(this->m_Hwnd,&it);
			}
		}
		else
			this->showErrorEx(NULL, "-T", "Unable To Get Item: %d Subitem: %d", lvi.iItem +1, lvi.iSubItem);
	}
	// xdid -Z [NAME] [ID] [SWITCH] [%]
	else if (flags['Z'] && numtok > 3) {
		// only works for this one so far
		if (!this->isStyle(LVS_REPORT))
			return;

		int pos = input.gettok(4).to_int();
		int count = ListView_GetItemCount(this->m_Hwnd);
		int height;
		RECT rc;

		// no items - no need to view
		if (count == 0)
			return;

		// check boundaries
		if ((pos < 0) || (pos > 100))
			return;

		// subtract the number of visible items
		count -= ListView_GetCountPerPage(this->m_Hwnd);

		// get height of a single item
		ListView_GetItemRect(this->m_Hwnd, 0, &rc, LVIR_BOUNDS);
		height = count * (rc.bottom - rc.top);

		pos = round((float) height * (float) pos / (float) 100.0);

		ListView_EnsureVisible(this->m_Hwnd, 0, FALSE);
		ListView_Scroll(this->m_Hwnd, 0, pos);
	}
	// xdid -V [NAME] [ID] [SWITCH] [nItem]
	else if (flags['V'] && numtok > 3) {
		int nItem = input.gettok( 4 ).to_int() -1;

		if (nItem > -1)
			ListView_EnsureVisible(this->m_Hwnd, nItem, FALSE);
	}
	// xdid -S [NAME] [ID] [+FLAGS] [N1] [N2] [ARGS]
	else if (flags['S'] && numtok > 5) {
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
		int count = ListView_GetItemCount(this->m_Hwnd);
		TString tsFlags(input.gettok( 3 ).trim());
		int iN1 = input.gettok( 4 ).to_int();
		int iN2 = input.gettok( 5 ).to_int();
		TString tsArgs(input.gettok( 6, -1 ).trim());

		if ((tsFlags[0] != '+') || (tsFlags.len() < 2)) {
			// no flags specified.
			this->showError(NULL,"-S", "Invalid Flags: No Flags Specified.");
			return;
		}
		// make sure N1-N2 are within the range of items in listview.
		// adjust iN2 if its < 0, so its an offset from the last item.
		if (iN2 < 0) iN2 = count + iN2;
		if ((iN1 < 1) || (iN1 > count) || (iN2 < 0) || (iN2 < iN1)) {
			this->showError(NULL,"-S", "Invalid Range: N1-N2 Must be in range of items in listview");
			return;
		}
		if (iN2 == 0) iN2 = count;

		switch (tsFlags[1])
		{
		case 'c':
			{
				char res[256];
				// check window exists
				Dcx::mIRC.evalex(res, 256, "$window(%s)", tsArgs.to_chr());
				// if not exit
				if (tsArgs != res) {
					this->showErrorEx(NULL, "-S", "Invalid window: %s", tsArgs.to_chr());
					return;
				}
				//
			}
			break;
		case 'f':
			break;
		case 'h':
			break;
		case 'x':
			break;
		default:
			this->showErrorEx(NULL,"-S", "Invalid Flags: %s", tsFlags.to_chr());
			return;
		}
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*
Initialises an image list.
*/
HIMAGELIST DcxListView::initImageList(const int iImageList) {
	HIMAGELIST himl = this->getImageList(iImageList);

	if (himl != NULL)
		return himl;

	himl = this->createImageList(iImageList == LVSIL_NORMAL ? TRUE : FALSE);

	if (himl != NULL)
		this->setImageList(himl, iImageList);

	return himl;
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxListView::getImageList( const int iImageList ) {

	return ListView_GetImageList( this->m_Hwnd, iImageList );
}

/*!
* \brief blah
*
* blah
*/

void DcxListView::setImageList(HIMAGELIST himl, const int iImageList) {
	HIMAGELIST o = ListView_SetImageList(this->m_Hwnd, himl, iImageList);
	if (o != NULL && o != himl)
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

	UINT i = 1, len = (INT)flags.len( ), iFlags = 0;

	// no +sign, missing params
	if ( flags[0] != '+' ) 
		return iFlags;

	while ( i < len ) {
		if ( flags[i] == 'n' )
			iFlags |= LVSIL_SMALL;
		else if ( flags[i] == 's' )
			iFlags |= LVSIL_STATE;

		++i;
	}
	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseItemFlags(const TString & flags) {
	INT i = 1, len = (INT)flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+')
		return iFlags;

	while (i < len) {
		if (flags[i] == 'b')
			iFlags |= LVIS_BOLD;
		else if (flags[i] == 'c')
			iFlags |= LVIS_COLOR;
		else if (flags[i] == 'd')
			iFlags |= LVIS_DROPHILITED;
		else if (flags[i] == 'f')
			iFlags |= LVIS_FOCUSED;
		else if (flags[i] == 'i')
			iFlags |= LVIS_ITALIC;
		else if (flags[i] == 'k')
			iFlags |= LVIS_BGCOLOR;
		else if (flags[i] == 's')
			iFlags |= LVIS_SELECTED;
		else if (flags[i] == 't')
			iFlags |= LVIS_CUT;
		else if (flags[i] == 'u')
			iFlags |= LVIS_UNDERLINE;
		else if (flags[i] == 'p')
			iFlags |= LVIS_PBAR;
		else if (flags[i] == 'H')
			iFlags |= LVIS_HASHITEM;
		else if (flags[i] == 'n')
			iFlags |= LVIS_HASHNUMBER;
		else if (flags[i] == 'x')
			iFlags |= LVIS_XML;
		else if (flags[i] == 'w')
			iFlags |= LVIS_HASHTABLE;
		else if (flags[i] == 'y')
			iFlags |= LVIS_WINDOW;
		else if (flags[i] == 'z')
			iFlags |= LVIS_CONTROL;

		++i;
	}

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseMassItemFlags(const TString & flags) {
	INT i = 1, len = (INT)flags.len();
	UINT iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+')
		return iFlags;

	while (i < len) {
		if (flags[i] == 'a')
			iFlags |= LVIMF_ALLINFO;
		else if (flags[i] == 'A')
			iFlags |= LVIMF_ADDALL;
		else if (flags[i] == 'n')
			iFlags |= LVIMF_NUMERIC;
		else if (flags[i] == 'i')
			iFlags |= LVIMF_NAMED;

		++i;
	}

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseHeaderFlags( const TString & flags ) {

	INT i = 1, len = (INT)flags.len( ), iFlags = 0;

	// no +sign, missing params
	if ( flags[0] != '+' )
		return iFlags;

	while ( i < len ) {

		if ( flags[i] == 'l' )
			iFlags |= LVCFMT_LEFT;
		else if ( flags[i] == 'c' )
			iFlags |= LVCFMT_CENTER;
		else if ( flags[i] == 'r' )
			iFlags |= LVCFMT_RIGHT;
		else if ( flags[i] == 'b' )
			iFlags |= LVCFMT_BITMAP_ON_RIGHT;

		++i;
	}
	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

INT DcxListView::parseHeaderFlags2( const TString & flags ) {

	INT i = 1, len = (INT)flags.len( ), iFlags = 0;

	// no +sign, missing params
	if ( flags[0] != '+' ) 
		return iFlags;

	while ( i < len ) {

		if ( flags[i] == 'a' ) // auto size
			iFlags = LVSCW_AUTOSIZE;
		else if ( flags[i] == 'h' ) // header size
			iFlags = LVSCW_AUTOSIZE_USEHEADER;
		else if ( flags[i] == 'm' ) // max size (max of auto & header)
			iFlags = -3;

		++i;
	}
	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseSortFlags( const TString & flags ) {

	INT i = 1, len = (INT)flags.len( ), iFlags = 0;

	// no +sign, missing params
	if ( flags[0] != '+' ) 
		return iFlags;

	while ( i < len ) {

		if ( flags[i] == 'a' )
			iFlags |= LVSS_ASC;
		else if ( flags[i] == 'c' )
			iFlags |= LVSS_CUSTOM;
		else if ( flags[i] == 'd' )
			iFlags |= LVSS_DESC;
		else if ( flags[i] == 'n' )
			iFlags |= LVSS_NUMERIC;
		else if ( flags[i] == 's' )
			iFlags |= LVSS_CASE;
		else if ( flags[i] == 't' )
			iFlags |= LVSS_ALPHA;

		++i;
	}
	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseGroupFlags( const TString & flags ) {

	INT i = 1, len = (INT)flags.len( ), iFlags = 0;

	// no +sign, missing params
	if ( flags[0] != '+' ) 
		return iFlags;

	while ( i < len ) {

		if ( flags[i] == 'c' )
			iFlags |= LVGA_HEADER_CENTER;
		else if ( flags[i] == 'l' )
			iFlags |= LVGA_HEADER_LEFT;
		else if ( flags[i] == 'r' )
			iFlags |= LVGA_HEADER_RIGHT;

		++i;
	}
	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseColorFlags( const TString & flags ) {

	INT i = 1, len = (INT)flags.len( ), iFlags = 0;

	// no +sign, missing params
	if ( flags[0] != '+' ) 
		return iFlags;

	while ( i < len ) {

		if ( flags[i] == 'b' )
			iFlags |= LVCS_BKG;
		else if ( flags[i] == 'k' )
			iFlags |= LVCS_BKGTEXT;
		else if ( flags[i] == 'o' )
			iFlags |= LVCS_OUTLINE;
		else if ( flags[i] == 't' )
			iFlags |= LVCS_TEXT;

		++i;
	}
	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxListView::parseImageFlags( const TString & flags ) {

	INT i = 1, len = (INT)flags.len( ), iFlags = 0;

	// no +sign, missing params
	if ( flags[0] != '+' ) 
		return iFlags;

	while ( i < len ) {

		if ( flags[i] == 'n' )
			iFlags |= LVBKIF_STYLE_NORMAL;
		else if ( flags[i] == 'r' )
			iFlags |= LVBKIF_SOURCE_NONE;
		else if ( flags[i] == 't' )
			iFlags |= LVBKIF_STYLE_TILE;
		//else if ( flags[i] == 'w' )
		//	iFlags |= LVBKIF_TYPE_WATERMARK;
		//else if ( flags[i] == 'a' )
		//	iFlags |= LVBKIF_TYPE_WATERMARK|LVBKIF_FLAG_ALPHABLEND;

		++i;
	}
	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

BOOL DcxListView::isListViewStyle( const long dwView ) const {

	long dwStyle = GetWindowStyle( this->m_Hwnd );
	if ( ( dwStyle & LVS_TYPEMASK ) == dwView )
		return TRUE;

	return FALSE;
}

/*!
* \brief blah
*
* blah
*/

BOOL DcxListView::matchItemText( const int nItem, const int nSubItem, const TString * search, const UINT SearchType ) {
	char itemtext[900];
	ListView_GetItemText( this->m_Hwnd, nItem, nSubItem, itemtext, 900 );

	switch (SearchType) {
		case LVSEARCH_R:
			return isRegexMatch(itemtext, search->to_chr());
		case LVSEARCH_W:
			return TString(itemtext).iswm(search->to_chr());
		case LVSEARCH_E:
			return (lstrcmp(search->to_chr(), itemtext) == 0); // must be a zero check not a !
	}

	return FALSE;
}

/*!
* \brief blah
*
* blah
*/

int DcxListView::getColumnCount( ) const {

	if (ListView_GetView(this->m_Hwnd) != LVS_REPORT)	// Colums info is only available in report view.
		return 1; // when not in report view fake a single column.

	LVCOLUMN lvc;
	ZeroMemory( &lvc, sizeof(LVCOLUMN) );
	lvc.mask = LVCF_WIDTH;

	int i = 0;
	while ( ListView_GetColumn( this->m_Hwnd, i, &lvc ) != FALSE )
		i++;

	return i;
}

/*!
* \brief blah
*
* blah
*/

int DcxListView::getTopIndex( ) const {

	if ( ListView_GetItemCount( this->m_Hwnd) > 0 )
		return ListView_GetTopIndex( this->m_Hwnd );
	else
		return -1;
}

/*!
* \brief blah
*
* blah
*/

int DcxListView::getBottomIndex( ) const {

	int nBottomIndex = ListView_GetTopIndex( this->m_Hwnd ) + ListView_GetCountPerPage( this->m_Hwnd ) - 1;
	if ( nBottomIndex > ( ListView_GetItemCount( this->m_Hwnd ) - 1 ) )
		nBottomIndex = ListView_GetItemCount( this->m_Hwnd ) - 1;

	return nBottomIndex;
}

/*!
* \brief blah
*
* blah
*/

int CALLBACK DcxListView::sortItemsEx( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort ) {

	LPDCXLVSORT plvsort = (LPDCXLVSORT) lParamSort;
	char itemtext1[900];
	char itemtext2[900];

	ListView_GetItemText( plvsort->m_Hwnd, lParam1, plvsort->nColumn, itemtext1, 900 );
	ListView_GetItemText( plvsort->m_Hwnd, lParam2, plvsort->nColumn, itemtext2, 900 );

	// CUSTOM Sort
	if ( plvsort->iSortFlags & LVSS_CUSTOM ) {
		char res[20];

		Dcx::mIRC.evalex( res, 20, "$%s(%s,%s)", plvsort->tsCustomAlias.to_chr( ), itemtext1, itemtext2 );

		int ires = atoi(res);

		if (ires < -1)
			ires = -1;
		else if (ires > 1)
			ires = 1;

		if ( plvsort->iSortFlags & LVSS_DESC )
			return ires;
		else {

			if (ires == -1)
				return 1;
			else if (ires == 1)
				return -1;
		}
	}
	// NUMERIC Sort
	else if ( plvsort->iSortFlags & LVSS_NUMERIC ) {

		int i1 = atoi( itemtext1 );
		int i2 = atoi( itemtext2 );

		if ( plvsort->iSortFlags & LVSS_DESC ) {

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

		if ( plvsort->iSortFlags & LVSS_DESC ) {
			if ( plvsort->iSortFlags & LVSS_CASE )
				return -lstrcmp( itemtext1, itemtext2 );
			else
				return -lstrcmpi( itemtext1, itemtext2 );
		}
		else {
			if ( plvsort->iSortFlags & LVSS_CASE )
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
LRESULT DcxListView::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {
		case WM_NOTIFY:
			{
				LPNMHDR hdr = (LPNMHDR) lParam;

				if (!hdr)
					break;

				switch( hdr->code ) {

					case NM_CLICK:
						{
							bParsed = TRUE;

							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								LVHITTESTINFO lvh;
								LPNMITEMACTIVATE nmia = (LPNMITEMACTIVATE)lParam;
								lvh.pt = nmia->ptAction;
								ListView_SubItemHitTest( this->m_Hwnd, &lvh );
								DWORD lvexstyles = ListView_GetExtendedListViewStyle( this->m_Hwnd );

								if ( ( lvh.flags & LVHT_ONITEMSTATEICON ) && ( lvexstyles & LVS_EX_CHECKBOXES ) && !( lvh.flags & LVHT_ONITEMICON ) && !( lvh.flags & LVHT_ONITEMLABEL ) ) 
								{
									//TODO: int state = ListView_GetCheckState(this->m_Hwnd, lvh.iItem);
									this->execAliasEx("%s,%d,%d,%d", "stateclick", this->getUserID( ), lvh.iItem + 1, lvh.iSubItem +1);
								}
								else if ( lvh.flags & LVHT_ONITEM )
									this->execAliasEx("%s,%d,%d,%d", "sclick", this->getUserID( ), lvh.iItem + 1, lvh.iSubItem +1);
								else if (lvh.flags & LVHT_NOWHERE)
									this->execAliasEx("%s,%d", "sclick", this->getUserID());

#if !defined(NDEBUG) || defined(DCX_DEV_BUILD)
								if (!(lvexstyles & LVS_EX_FULLROWSELECT))
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
#endif
							}
						}
						break;

					case NM_DBLCLK:
						{
							bParsed = TRUE;

							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								LVHITTESTINFO lvh;
								LPNMITEMACTIVATE nmia = (LPNMITEMACTIVATE)lParam;
								lvh.pt = nmia->ptAction;
								ListView_SubItemHitTest( this->m_Hwnd, &lvh );

								if ( lvh.flags & LVHT_ONITEM )
									this->execAliasEx("%s,%d,%d,%d", "dclick", this->getUserID( ), lvh.iItem +1, lvh.iSubItem +1);
								else
									this->execAliasEx("%s,%d", "dclick", this->getUserID());
							}
						}
						break;

					case NM_RCLICK:
						{
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								LVHITTESTINFO lvh;
								LPNMITEMACTIVATE nmia = (LPNMITEMACTIVATE)lParam;
								lvh.pt = nmia->ptAction;
								ListView_SubItemHitTest( this->m_Hwnd, &lvh );

								if ( lvh.flags & LVHT_ONITEM )
									this->execAliasEx("%s,%d,%d,%d", "rclick", this->getUserID( ), lvh.iItem +1, lvh.iSubItem +1);
								else
									this->execAliasEx("%s,%d", "rclick", this->getUserID());
							}
							bParsed = TRUE;
						}
						break;

					case NM_RDBLCLK:
						{
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								LVHITTESTINFO lvh;
								LPNMITEMACTIVATE nmia = (LPNMITEMACTIVATE)lParam;
								lvh.pt = nmia->ptAction;
								ListView_SubItemHitTest( this->m_Hwnd, &lvh );

								if ( lvh.flags & LVHT_ONITEM )
									this->execAliasEx("%s,%d,%d,%d", "rdclick", this->getUserID( ), lvh.iItem +1, lvh.iSubItem +1);
								else
									this->execAliasEx("%s,%d", "rdclick", this->getUserID());
							}
							bParsed = TRUE;
						}
						break;

					case NM_HOVER:
						{
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								LVHITTESTINFO lvh;
								GetCursorPos( &lvh.pt );
								MapWindowPoints(NULL, this->m_Hwnd, &lvh.pt, 1 );
								ListView_SubItemHitTest( this->m_Hwnd, &lvh );

								if ( lvh.flags & LVHT_ONITEM )
									this->execAliasEx("%s,%d,%d,%d", "hover", this->getUserID( ), lvh.iItem + 1, lvh.iSubItem +1);
								else
									this->execAliasEx("%s,%d", "hover", this->getUserID());
							}
							bParsed = TRUE;
						}
						break;

					case LVN_BEGINLABELEDIT:
						{

							bParsed = TRUE;
							LPNMLVDISPINFO lplvdi = (LPNMLVDISPINFO) lParam;

							ListView_SetItemState( this->m_Hwnd, lplvdi->item.iItem, LVIS_SELECTED, LVIS_SELECTED );

							HWND edit_hwnd = ListView_GetEditControl( this->m_Hwnd );

							this->m_OrigEditProc = (WNDPROC) SetWindowLongPtr( edit_hwnd, GWLP_WNDPROC, (LONG_PTR) DcxListView::EditLabelProc );
							SetProp( edit_hwnd, "dcx_pthis", (HANDLE) this );

							char ret[256];
							this->evalAliasEx(ret, 255, "%s,%d,%d,%d", "labelbegin", this->getUserID(), lplvdi->item.iItem +1, lplvdi->item.iSubItem +1);

							if ( !lstrcmp( "noedit", ret ) )
								return TRUE;
						}
						break;

					case LVN_ENDLABELEDIT:
						{

							bParsed = TRUE;

							LPNMLVDISPINFO lplvdi = (LPNMLVDISPINFO) lParam;
							if ( lplvdi->item.pszText == NULL ) {

								this->execAliasEx("%s,%d", "labelcancel", this->getUserID( ) );
							}
							else {
								char ret[256];
								this->evalAliasEx( ret, 255, "%s,%d,%s", "labelend", this->getUserID( ), lplvdi->item.pszText );

								if ( !lstrcmp( "noedit", ret ) )
									return FALSE;

								return TRUE;
							}
							return TRUE;
						}
						break;

					case NM_CUSTOMDRAW:
						{
							LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW) lParam;
							bParsed = TRUE;

							switch( lplvcd->nmcd.dwDrawStage ) {
								case CDDS_PREPAINT:
									return ( CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT );

								case CDDS_ITEMPREPAINT:
									return CDRF_NOTIFYSUBITEMDRAW;

								case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
									{
										LPDCXLVITEM lpdcxlvi = (LPDCXLVITEM) lplvcd->nmcd.lItemlParam;

										if ( lpdcxlvi == NULL )
											return CDRF_DODEFAULT;

										if ((UINT)lplvcd->iSubItem >= lpdcxlvi->vInfo.size())
											return CDRF_DODEFAULT;

										LPDCXLVRENDERINFO ri = lpdcxlvi->vInfo[lplvcd->iSubItem];
										if ( ri->m_cText != CLR_INVALID )
											lplvcd->clrText = ri->m_cText;

										if ( ri->m_cBg != CLR_INVALID )
											lplvcd->clrTextBk = ri->m_cBg;
										//if (lplvcd->nmcd.uItemState & CDIS_SELECTED)
										//if (lplvcd->nmcd.uItemState & CDIS_FOCUS)
										//	lplvcd->clrTextBk = RGB(255,0,0);
										//else if ( ri->m_cBg != -1 )
										//	lplvcd->clrTextBk = ri->m_cBg;
										//lplvcd->clrFace = RGB(0,255,0);

										if (ri->m_dFlags & LVIS_UNDERLINE || ri->m_dFlags & LVIS_BOLD || ri->m_dFlags & LVIS_ITALIC) {
											HFONT hFont = GetWindowFont(this->m_Hwnd);
											LOGFONT lf;

											GetObject(hFont, sizeof(LOGFONT), &lf);

											if (ri->m_dFlags & LVIS_BOLD)
												lf.lfWeight |= FW_BOLD;
											if (ri->m_dFlags & LVIS_UNDERLINE)
												lf.lfUnderline = true;
											if (ri->m_dFlags & LVIS_ITALIC)
												lf.lfItalic = true;

											this->m_hItemFont = CreateFontIndirect( &lf );
											if (this->m_hItemFont != NULL)
												this->m_hOldItemFont = SelectFont( lplvcd->nmcd.hdc, this->m_hItemFont );
										}

										// NB: CDRF_NOTIFYPOSTPAINT required to get the post paint message.
										return ( CDRF_NEWFONT|CDRF_NOTIFYPOSTPAINT );

										//RECT rcText, rcIcon, rcBounds;
										//TString tsText((UINT)1024);
										//UINT style = DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS;
										//COLORREF clrText = CLR_INVALID;
										//LVITEM lvi;
										//ZeroMemory(&lvi, sizeof(LVITEM));
										//HIMAGELIST himl = this->getImageList(LVSIL_SMALL);

										//lvi.mask = LVIF_IMAGE|LVIF_STATE|LVIF_TEXT;
										//lvi.pszText = tsText.to_chr();
										//lvi.cchTextMax = 1023;
										//lvi.iItem = lplvcd->nmcd.dwItemSpec;
										//lvi.iSubItem = lplvcd->iSubItem;

										//ListView_GetItem(this->m_Hwnd, &lvi);

										//ListView_GetSubItemRect(this->m_Hwnd, lplvcd->nmcd.dwItemSpec, lplvcd->iSubItem, LVIR_BOUNDS, &rcBounds);
										//ListView_GetSubItemRect(this->m_Hwnd, lplvcd->nmcd.dwItemSpec, lplvcd->iSubItem, LVIR_ICON, &rcIcon);

										//rcText = rcBounds;
										//rcText.left = rcIcon.right + 1;

										//DrawTextW(lplvcd->nmcd.hdc, tsText.to_wchr(this->m_bUseUTF8), tsText.wlen(), &rcText, style | DT_CALCRECT);

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

										//if ((himl != NULL) && (lvi.iImage > -1)) {
										//	ImageList_Draw(himl, lvi.iImage, lplvcd->nmcd.hdc, rcIcon.left, rcIcon.top, ILD_TRANSPARENT);
										//}

										//if (!this->m_bCtrlCodeText) {
										//	if (this->m_bShadowText)
										//		dcxDrawShadowText(lplvcd->nmcd.hdc, tsText.to_wchr(this->m_bUseUTF8), tsText.wlen(), &rcText, style, lplvcd->clrText, 0, 5, 5);
										//	else
										//		DrawTextW(lplvcd->nmcd.hdc, tsText.to_wchr(this->m_bUseUTF8), tsText.wlen(), &rcText, style);
										//}
										//else
										//	mIRC_DrawText(lplvcd->nmcd.hdc, tsText, &rcText, style, this->m_bShadowText, this->m_bUseUTF8);

										//if (clrText != CLR_INVALID)
										//	SetTextColor(lplvcd->nmcd.hdc, clrText);

										//// NB: CDRF_NOTIFYPOSTPAINT required to get the post paint message.
										//return ( CDRF_NEWFONT|CDRF_NOTIFYPOSTPAINT|CDRF_SKIPDEFAULT );
									}
									break;

								case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
									{
										if (this->m_hOldItemFont != NULL) {
											SelectFont( lplvcd->nmcd.hdc, this->m_hOldItemFont);
											this->m_hOldItemFont = NULL;
										}
										if (this->m_hItemFont != NULL) {
											DeleteFont(this->m_hItemFont);
											this->m_hItemFont = NULL;
										}
										return CDRF_DODEFAULT;
									}

								case CDDS_POSTPAINT:
									// update the pbar positions
									//this->ScrollPbars((int) lplvcd->nmcd.dwItemSpec);
									this->UpdateScrollPbars();
									return CDRF_DODEFAULT;

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
							LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW) lParam;
							LPDCXLVITEM lpdcxlvi = (LPDCXLVITEM) lpnmlv->lParam;

							if (lpdcxlvi != NULL) {
								if (lpdcxlvi->pbar != NULL)
									DestroyWindow(lpdcxlvi->pbar->getHwnd());

								VectorOfRenderInfo::iterator itStart = lpdcxlvi->vInfo.begin();
								VectorOfRenderInfo::iterator itEnd = lpdcxlvi->vInfo.end();

								while (itStart != itEnd) {
									if (*itStart != NULL)
										delete (LPDCXLVRENDERINFO)*itStart;
									itStart++;
								}
								lpdcxlvi->vInfo.clear();

								delete lpdcxlvi;
							}
							bParsed = TRUE; // message has been handled.
						}
						break;

					// TODO: twig: erm? unfinished? its undocumented
					case LVN_BEGINDRAG:
						{
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_DRAG)
								this->execAliasEx("%s,%d", "begindrag", this->getUserID( ) );
						}
						break;

						//case LVN_ENDSCROLL:
						// {
						//	 if (this->isExStyle(LVS_EX_GRIDLINES)) {
						//		 mIRCError("scroll");
						//		 //this->redrawWindow();
						//	 }
						//	 break;
						// }
						//		 4294967296-max
					case LVN_KEYDOWN:
						{
							LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN) lParam; 
							WORD wVKey = pnkd->wVKey;
							char cb[15];

							this->evalAliasEx(cb, 14, "%s,%d,%d", "keydown", this->getUserID( ), wVKey);

							// space to change checkbox state
							if ((wVKey == 32) &&
								(ListView_GetExtendedListViewStyle(this->m_Hwnd) & LVS_EX_CHECKBOXES)) {

								// stop it from allowing user to change checkstate by pressing spacebar
								if (!lstrcmp("nospace", cb)) {
									bParsed = TRUE;
									return TRUE;
								}

								int index = ListView_GetNextItem(this->m_Hwnd, -1, LVNI_FOCUSED);

								// TODO: twig: change this if we get multiple checkbox columns working
								this->evalAliasEx(cb, 14, "%s,%d,%d,%d", "stateclick", this->getUserID(), index +1, 1);
							}
						}
						break;
						//case LVN_CHANGING: // 4294967196
						//case LVN_CHANGED: // 4294967195
						//	NM_FIRST
						//	LVN_FIRST
					case LVN_ITEMCHANGED:
						{
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
								if (pnmv->iItem == -1)
									break;

								if (pnmv->uChanged & LVIF_STATE) {
									if ((pnmv->uNewState & LVIS_SELECTED) && !(pnmv->uOldState & LVIS_SELECTED))
										this->execAliasEx("%s,%d,%d,%d", "selected", this->getUserID( ), pnmv->iItem +1, pnmv->iSubItem +1);
									else if (!(pnmv->uNewState & LVIS_SELECTED) && (pnmv->uOldState & LVIS_SELECTED))
										this->execAliasEx("%s,%d,%d,%d", "deselected", this->getUserID( ), pnmv->iItem +1, pnmv->iSubItem +1);
								}
							}
						}
						break;
					case LVN_GETINFOTIP:
						{
							LPNMLVGETINFOTIP pGetInfoTip = (LPNMLVGETINFOTIP)lParam;
							LVITEM lvi = { 0 };
							lvi.iItem = pGetInfoTip->iItem;
							lvi.iSubItem = pGetInfoTip->iSubItem;
							lvi.mask = LVIF_PARAM;
							// Get item information.
							if (ListView_GetItem(this->m_Hwnd, &lvi)) {
								LPDCXLVITEM lpdcxlvi = (LPDCXLVITEM)lvi.lParam;
								// return tooltip text, if any.
								if (lpdcxlvi->tsTipText.len() > 0)
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
			//			DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,"dcx_cthis");
			//			if (c_this != NULL) {
			//				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			//			}
			//		}
			//	}
			//	break;

			//  case WM_DELETEITEM:
			//    {
			//		DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
			//		if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
			//			DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
			//			if (c_this != NULL) {
			//				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			//			}
			//		}
			//    }
			//    break;

			//  case WM_MEASUREITEM:
			//    {
			//		HWND cHwnd = GetDlgItem(this->m_Hwnd, wParam);
			//		if (IsWindow(cHwnd)) {
			//			DcxControl *c_this = (DcxControl *) GetProp(cHwnd,"dcx_cthis");
			//			if (c_this != NULL) {
			//				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			//			}
			//		}
			//    }
			//    break;

			//  case WM_DRAWITEM:
			//    {
			//		DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
			//		if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
			//			DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
			//			if (c_this != NULL) {
			//				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			//			}
			//		}
			//    }
			//   break;

		case WM_NOTIFY: 
			{

				LPNMHDR hdr = (LPNMHDR) lParam;

				if (!hdr)
					break;

				//if (this->m_Hwnd != hdr->hwndFrom) {
				//	if (IsWindow(hdr->hwndFrom)) {
				//		DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,"dcx_cthis");
				//		if (c_this != NULL) {
				//			lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				//		}
				//	}
				//}
				//if (!bParsed) {
				switch( hdr->code ) {
				case NM_RCLICK:
					{
						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
							char ClassName[257];

							GetClassName( hdr->hwndFrom, ClassName, 256 );

							if ( lstrcmpi( ClassName, "SysHeader32" ) == 0 ) {

								HDHITTESTINFO hdti;
								GetCursorPos( &hdti.pt );
								MapWindowPoints(NULL, hdr->hwndFrom, &hdti.pt, 1 );
								if ( SendMessage( hdr->hwndFrom, HDM_HITTEST, (WPARAM) 0, (LPARAM) &hdti ) != -1 )
									this->execAliasEx("%s,%d,%d", "hrclick", this->getUserID( ), hdti.iItem + 1 );
							}
						}
						bParsed = TRUE;
					}
					break;
				case HDN_BEGINTRACKW:
				case HDN_BEGINTRACK:
					{
						bParsed = TRUE;

						LPNMHEADER pHeader = (LPNMHEADER) lParam;

						char ret[256];
						this->evalAliasEx( ret, 255, "%s,%d,%d", "trackbegin", this->getUserID(), pHeader->iItem +1);

						if (!lstrcmp("notrack", ret))
							return TRUE;
					}
					break;

				case HDN_ITEMCLICKW:
				case HDN_ITEMCLICK:
					{
						bParsed = TRUE;

						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
							LPNMHEADER lphdr = (LPNMHEADER) lParam;
							this->execAliasEx("%s,%d,%d", "hsclick", this->getUserID( ), lphdr->iItem + 1 );
						}
					}
					break;

				case HDN_ITEMDBLCLICKW:
				case HDN_ITEMDBLCLICK:
					{
						bParsed = TRUE;

						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
							LPNMHEADER lphdr = (LPNMHEADER) lParam;
							this->execAliasEx("%s,%d,%d", "hdclick", this->getUserID( ), lphdr->iItem + 1 );
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
					//					if (dci != NULL)
					//						di->lpszText = dci->tsTipText.to_chr();
					//				}
					//			}
					//		}
					//		//di->lpszText = this->m_tsToolTip.to_chr();
					//		di->hinst = NULL;
					//		bParsed = TRUE;
					//	}
					//	break;
					//case TTN_LINKCLICK:
					//	{
					//		bParsed = TRUE;
					//		this->execAliasEx("%s,%d", "tooltiplink", this->getUserID( ) );
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
				//	DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,"dcx_cthis");
				//	if (c_this != NULL) {
				//		lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				//	}
				//	else {
				if (LOWORD(wParam) == SB_ENDSCROLL)
					this->execAliasEx("%s,%d", "scrollend", this->getUserID());

				if (ListView_GetExtendedListViewStyle(this->m_Hwnd) & LVS_EX_GRIDLINES)
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
				HDC hdc;

				hdc = BeginPaint( this->m_Hwnd, &ps );

				bParsed = TRUE;

				// Setup alpha blend if any.
				LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

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

	DcxListView * pthis = (DcxListView *) GetProp( mHwnd, "dcx_pthis" );

	switch( uMsg ) {

		case WM_GETDLGCODE:
			return DLGC_WANTALLKEYS;

		case WM_DESTROY:
			{
				RemoveProp( mHwnd, "dcx_pthis" );
				SetWindowLongPtr( mHwnd, GWLP_WNDPROC, (LONG_PTR) pthis->m_OrigEditProc );
			}
			break;

	}
	return CallWindowProc( pthis->m_OrigEditProc, mHwnd, uMsg, wParam, lParam );
}


DcxControl* DcxListView::CreatePbar(LPLVITEM lvi, const TString &styles) {
	// can only create progress for an existing item
	if (!lvi || !lvi->lParam)
		return NULL;

	LPDCXLVITEM lpdcxlvi = (LPDCXLVITEM) lvi->lParam;

	if (lpdcxlvi->pbar)
		return NULL;

	RECT rItem;

	// initial rect for pbar
	if (lvi->iSubItem == 0)
		ListView_GetItemRect(this->m_Hwnd, lvi->iItem, &rItem, LVIR_LABEL);
	else
		ListView_GetSubItemRect(this->m_Hwnd, lvi->iItem, lvi->iSubItem, LVIR_LABEL, &rItem);

	lpdcxlvi->iPbarCol = lvi->iSubItem;
	// controls within a listview have a problem in that they cant set an item height,
	// so they all appear very small, & dont look very good.
	//UINT ID = mIRC_ID_OFFSET + styles.gettok( 1 ).to_int();
	//if ( ID > mIRC_ID_OFFSET - 1 && 
	//	!IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
	//	this->m_pParentDialog->getControlByID( ID ) == NULL ) 
	//{
	//	TString ctrl_args;
	//	ctrl_args.sprintf("%s %d %d %d %d %s", styles.gettok(2).to_chr(), rItem.left, rItem.top, (rItem.right - rItem.left), (rItem.bottom - rItem.top), styles.gettok(3,-1).to_chr());
	//	lpdcxlvi->pbar = DcxControl::controlFactory(this->m_pParentDialog,ID,ctrl_args,1,-1,this->m_Hwnd);
	//}
	try {
		lpdcxlvi->pbar = (DcxControl *)new DcxProgressBar(this->getID(), this->m_pParentDialog, this->m_Hwnd, &rItem, const_cast<TString &>(styles));
	}
	catch ( char *err ) {
		this->showErrorEx(NULL, "-a", "Unable To Create ProgressBar: %s", err);
	}
	return lpdcxlvi->pbar;
}


void DcxListView::UpdateScrollPbars(void) {
	for (int row = 0; row < ListView_GetItemCount(this->m_Hwnd); row++) {
		this->ScrollPbars(row);
	}
}

// BUG: when listview has horiz scrollbars pbar will be moved oddly when listview is scrolled horiz.
//			pbars are positioned relative to visible area of control & as such arn't scrolled.
void DcxListView::ScrollPbars(const int row) {
	LPLVITEM lvi = new LVITEM;

	ZeroMemory(lvi, sizeof(LVITEM));

	for (int col = 0; col < this->getColumnCount(); col++) {
		lvi->iItem = row;
		lvi->iSubItem = col;
		lvi->mask = LVIF_PARAM;

		ListView_GetItem(this->m_Hwnd, lvi);

		if (!lvi->lParam)
			continue;

		LPDCXLVITEM lpdcxlvi = (LPDCXLVITEM) lvi->lParam;

		if (!lpdcxlvi->pbar)
			continue;

		// isnt the right column to move it to
		if (lpdcxlvi->iPbarCol != col) {
			continue;
		}

		RECT rItem;

		// hide it if its scrolled off visible range
		if ((lvi->iItem < this->getTopIndex()) || (lvi->iItem > this->getBottomIndex() +1)) {
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
		GetWindowRect(lpdcxlvi->pbar->getHwnd(), &rcWin);
		MapWindowRect(NULL, this->m_Hwnd, &rcWin);
		if (!EqualRect(&rcWin, &rItem)) {
			MoveWindow(lpdcxlvi->pbar->getHwnd(),
				rItem.left, rItem.top, (rItem.right - rItem.left), (rItem.bottom - rItem.top),
				FALSE);
			InvalidateRect(lpdcxlvi->pbar->getHwnd(),NULL,TRUE);
		}
		break;
	}

	delete lvi;
}

/*
	xmlLoadListview()
	Loads items into a listview control from a dcxml file.
*/
bool DcxListView::xmlLoadListview(const int nPos, const TString &name, TString &filename)
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

	TiXmlElement *xElm = xRoot->FirstChildElement("listview_data");
	if (xElm == NULL) {
		this->showError(NULL, "-a", "Unable To Find 'listview_data' element");
		return false;
	}

	xElm = xElm->FirstChildElement(name.to_chr());
	if (xElm == NULL) {
		this->showErrorEx(NULL, "-a", "Unable To Find Dataset: %s", name.to_chr());
		return false;
	}

	this->setRedraw(FALSE);

	int i = 0, nItem = nPos;
	const char *attr = NULL;
	LVITEM lvi;

	for (TiXmlElement *xNode = xElm->FirstChildElement("lvitem"); xNode != NULL; xNode = xNode->NextSiblingElement("lvitem"))
	{
		LPDCXLVITEM lpmylvi = new DCXLVITEM;

		lpmylvi->iPbarCol = 0;
		lpmylvi->pbar = NULL;
		lpmylvi->vInfo.clear();

		xmlSetItem(nItem, 0, xNode, &lvi, lpmylvi);

		// Items state icon.
		int stateicon = -1;
		attr = xNode->Attribute("stateicon",&i);
		if (attr != NULL && i > -1)
			stateicon = i;

		// Items overlay icon.
		int overlayicon = 0;
		attr = xNode->Attribute("overlayicon",&i);
		if (attr != NULL && i > 0)
			overlayicon = i;

		lvi.iItem = ListView_InsertItem(this->m_Hwnd, &lvi);

		if (lvi.iItem == -1) {
			//delete lpmylvi;
			//delete ri;
			this->showError(NULL,"-a", "Unable to add item");
			this->setRedraw(TRUE);
			return false;
		}

		if (stateicon > -1)
			ListView_SetItemState(this->m_Hwnd, lvi.iItem, INDEXTOSTATEIMAGEMASK(stateicon), LVIS_STATEIMAGEMASK);

		// overlay is 1-based index, max 15 overlay icons
		if (overlayicon > 0 && overlayicon < 16)
			ListView_SetItemState(this->m_Hwnd, lvi.iItem, INDEXTOOVERLAYMASK(overlayicon), LVIS_OVERLAYMASK);

		// Items checked state (if LVS_EX_CHECKBOXES style used)
		attr = xNode->Attribute("checked",&i);
		if (attr != NULL && i > 0 && (ListView_GetExtendedListViewStyle(this->m_Hwnd) & LVS_EX_CHECKBOXES)) // items are always added in `unchecked` state
			ListView_SetCheckState(this->m_Hwnd, lvi.iItem, TRUE);

		// autosize the column
		attr = xNode->Attribute("autosize",&i);
		if (attr != NULL && i > 0)
			this->autoSize(0,LVSCW_AUTOSIZE);
		else {
			attr = xNode->Attribute("autosizeheader",&i);
			if (attr != NULL && i > 0)
				this->autoSize(0,LVSCW_AUTOSIZE_USEHEADER);
			else {
				attr = xNode->Attribute("autosizemax",&i);
				if (attr != NULL && i > 0)
					this->autoSize(0,-3);
			}
		}
		// add items tooltip
		attr = xNode->Attribute("tooltip");
		if (attr != NULL) {
			TString cmd;
			cmd.sprintf("0 0 -T %d 0 %s", lvi.iItem +1, attr);
			this->parseCommandRequest(cmd);
		}
		// add subitems
		int nSubItem = 1;
		for (TiXmlElement *xSubNode = xNode->FirstChildElement("lvsubitem"); xSubNode != NULL; xSubNode = xSubNode->NextSiblingElement("lvsubitem"))
		{
			xmlSetItem(nItem, nSubItem, xSubNode, &lvi, lpmylvi);

			// SubItems overlay icon.
			attr = xSubNode->Attribute("overlayicon",&i);
			if (attr != NULL && i > 0) {
				lvi.mask |= LVIF_STATE;
				lvi.state |= INDEXTOOVERLAYMASK(i);
				lvi.stateMask |= LVIS_OVERLAYMASK;
			}
			if (ListView_SetItem(this->m_Hwnd, &lvi))
			{
				attr = xNode->Attribute("autosize",&i);
				if (attr != NULL && i > 0)
					this->autoSize(nSubItem,LVSCW_AUTOSIZE);
				else {
					attr = xNode->Attribute("autosizeheader",&i);
					if (attr != NULL && i > 0)
						this->autoSize(nSubItem,LVSCW_AUTOSIZE_USEHEADER);
					else {
						attr = xNode->Attribute("autosizemax",&i);
						if (attr != NULL && i > 0)
							this->autoSize(nSubItem,-3);
					}
				}
			}
			nSubItem++;
		}
		nItem++;
	}

	this->setRedraw(TRUE);
	//InvalidateRect(this->m_Hwnd, NULL, FALSE);
	//UpdateWindow(this->m_Hwnd);
	this->redrawWindow();
	return true;
}

void DcxListView::xmlSetItem(const int nItem, const int nSubItem, TiXmlElement *xNode, LPLVITEMA lvi, LPDCXLVITEM lpmylvi)
{
	LPDCXLVRENDERINFO ri = new DCXLVRENDERINFO;
	const char *attr = NULL;
	int i = 0;

	ZeroMemory(lvi, sizeof(LVITEM));
	ZeroMemory(ri, sizeof(DCXLVRENDERINFO));

	lvi->iItem = nItem;
	lvi->iSubItem = nSubItem;
	if (nSubItem == 0) {
		lvi->mask = LVIF_PARAM|LVIF_STATE;
		lvi->lParam = (LPARAM) lpmylvi;
	}

	// Is Item text in Bold?
	attr = xNode->Attribute("textbold",&i);
	if (i > 0)
		ri->m_dFlags |= LVIS_BOLD;

	// Is Item text Underlined?
	attr = xNode->Attribute("textunderline",&i);
	if (i > 0)
		ri->m_dFlags |= LVIS_UNDERLINE;

	// Items text colour.
	attr = xNode->Attribute("textcolor",&i);
	if (attr != NULL && i > -1) {
		ri->m_cText = (COLORREF)i;
		ri->m_dFlags |= LVIS_COLOR;
	}
	else
		ri->m_cText = CLR_INVALID;

	// Items background colour.
	attr = xNode->Attribute("backgroundcolor",&i);
	if (attr != NULL && i > -1) {
		ri->m_cBg = (COLORREF)i;
		ri->m_dFlags |= LVIS_BGCOLOR;
	}
	else
		ri->m_cBg = CLR_INVALID;

	lpmylvi->vInfo.push_back(ri);

	// Items icon.
	lvi->mask |= LVIF_IMAGE; // moved here to turn off icon when none is wanted.
	attr = xNode->Attribute("icon",&i);
	if (attr != NULL && i > 0)
		lvi->iImage = i -1;
	else
		lvi->iImage = -1;

	// Items icon.
	if (Dcx::XPPlusModule.isUseable() && ListView_IsGroupViewEnabled(this->m_Hwnd)) {
		attr = xNode->Attribute("group",&i);
		if (attr != NULL && i > -1 && ListView_HasGroup(this->m_Hwnd, i)) {
			lvi->iGroupId = i;
			lvi->mask |= LVIF_GROUPID;
		}
		else
			lvi->iGroupId = -1;
	}

	// Items background colour.
	attr = xNode->Attribute("indent",&i);
	if (attr != NULL && i > -1) {
		lvi->iIndent = i;
		lvi->mask |= LVIF_INDENT;
	}
	else
		lvi->iIndent = -1;

	// Items Text.
	attr = xNode->Attribute("text");
	if (attr != NULL) {
		lvi->mask |= LVIF_TEXT;
		lvi->pszText = (LPSTR)attr;
	}

	lvi->state = ri->m_dFlags;
	lvi->stateMask = (LVIS_FOCUSED|LVIS_SELECTED|LVIS_CUT|LVIS_DROPHILITED); // only alter the controls flags, ignore our custom ones.
}

//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
bool DcxListView::ctrlLoadListview(const int nPos, const TString &tsData)
{
	//DcxDialog * p_Dialog = Dcx::Dialogs.getDialogByName(dialogname);
	//if (p_Dialog == NULL) {
	//	this->showErrorEx(NULL, "-a", "Invalid dialog name: %s Only DCX dialogs are supported.", dialogname.to_chr());
	//	return false;
	//}
	//DcxControl * p_Control = p_Dialog->getControlByID((UINT) ctrl_ID + mIRC_ID_OFFSET);

	//if (p_Control == NULL) {
	//	this->showErrorEx(NULL, "-a", "Invalid control id: %d Only DCX controls are supported.", ctrl_ID);
	//	return false;
	//}
	return false;
}
//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
//tsData = [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] [+flags] [window/table] [item]
bool DcxListView::xLoadListview(const int nPos, const TString &tsData, const char *sTest, const char *sCount, const char *sGet, const char *sGetNamed)
{
	char res[1024];	// used to store the data returned by mIRC.
	TString tsflags(tsData.gettok( 10 ));
	TString tsName(tsData.gettok( 11 ));
	TString tsItem(tsData.gettok( 12, -1 ));

	// check table/window exists
	Dcx::mIRC.evalex(res, 1024, sTest, tsName.to_chr());
	// if not exit
	if (lstrcmp(tsName.to_chr(), res) != 0) {
		this->showErrorEx(NULL, "-a", "Invalid hashtable/window: %s", tsName.to_chr());
		return false;
	}
	// get the total number of items in the table.
	Dcx::mIRC.evalex(res, 1024, sCount, tsName.to_chr());
	UINT iTotal = atoi(res);
	// if no items then exit.
	if (iTotal == 0)
		return false;

	// convert the flags string to a bin mask
	UINT iFlags = DcxListView::parseMassItemFlags(tsflags);
	int iStart = 0, iEnd = 0;	// the first & last items in the hash table to add.
	TString input;	// the input string thats sent to the add item function.
					// The string end up having alot of space holders (0's), these are needed so the function can work with a normal xdid -a too.

	if (iFlags & LVIMF_NAMED) {
		if (sGetNamed == NULL) {
			this->showError(NULL, "-a", "Invalid flag used, +i is for hashtable items only");
			return false;
		}
		// add a single named item
		Dcx::mIRC.evalex(res, 1024, sGetNamed, tsName.to_chr(), tsItem.to_chr());
		if (iFlags & LVIMF_ALLINFO)
			// add items data from [INDENT] onwards is taken from hashtable, including subitems.
			//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
			//   0     0     0      0   read from hashtable->
			//input.sprintf("0 0 0 0 %s",res);
		{
			input = "0 0 0 0 ";
			input += res;
		}
		else
			// only the item text is taken from the hashtable.
			//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
			//   0     0     0      0      0       +       0        0         0           0        0        0		read from hashtable->
			//input.sprintf("0 0 0 0 0 + 0 0 0 0 0 0 %s", res);
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
	else if (iFlags & LVIMF_ADDALL) {
		// add all items from N onwards inclusive
		iStart = tsItem.to_int();	// first item to add
		iEnd = iTotal;			// last item to add is the last item in table.
	}
	else if (iFlags & LVIMF_NUMERIC) {
		// numeric range supplied.
		iStart = tsItem.gettok( 1, TSCOMMA).to_int();	// first item in hash table to be added taken from range.
		iEnd = tsItem.gettok( 2, TSCOMMA).to_int();	// last item in hash table to be added taken from range.
		if (iEnd < 0) iEnd = iTotal + iEnd;		// if iEnd is a negative number then make iEnd the last item in table + iEnd
	}
	// iStart & iEnd MUST be 1 or greater. (zero gives a hashtable/window item count.)
	if ((iStart < 1) || (iEnd < 1)) {
		this->showErrorEx(NULL, "-a", "Invalid numeric supplied: %s", tsItem.to_chr());
		return false;
	}

	this->setRedraw(FALSE); // disable redraw while adding lots of items.

	int nItem = nPos;	// tmp var use to update the item pos for each item added.

	while (iStart <= iEnd) {
		// get items data
		Dcx::mIRC.evalex(res, 1024, sGet, tsName.to_chr(), iStart);
		if (iFlags & LVIMF_ALLINFO)
			// add items data from [INDENT] onwards is taken from hashtable, including subitems.
			//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
			//   0     0     0      0   read from hashtable->
			//input.sprintf("0 0 0 0 %s",res);
		{
			input = "0 0 0 0 ";
			input += res;
		}
		else
			// only the item text is taken from the hashtable.
			//[NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text ...
			//   0     0     0      0      0       +       0        0         0           0        0        0		read from hashtable->
			//input.sprintf("0 0 0 0 0 + 0 0 0 0 0 0 %s", res);
		{
			parseText2Item(res, input, tsData);
		}
		massSetItem(nItem++, input);

		iStart++;
	}

	this->setRedraw(TRUE); // re-enable redraw when finished adding items.

	return true;
}

/*
	*	Used by xdid -a to parse data & add items & subitems
	*	Used by (hash/win/ctrl)LoadListView() functions to parse data & add items & subitems
*/

void DcxListView::massSetItem(const int nPos, const TString &input)
{
	TString data(input.gettok(1, TSTAB).gettok(4, -1).trim());

	int indent = data.gettok( 2 ).to_int();
	UINT stateFlags = this->parseItemFlags(data.gettok( 3 ));
	int icon = data.gettok( 4 ).to_int() -1;
	int state = data.gettok( 5 ).to_int();
	int overlay = data.gettok( 6 ).to_int( );
	int group = data.gettok( 7 ).to_int();
	COLORREF clrText = (COLORREF)data.gettok( 8 ).to_num();
	COLORREF clrBack = (COLORREF)data.gettok( 9 ).to_num();
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));

	LPDCXLVITEM lpmylvi = new DCXLVITEM;

	if (lpmylvi == NULL)
		return;

	lpmylvi->iPbarCol = 0;
	lpmylvi->pbar = NULL;
	lpmylvi->vInfo.clear();

	LPDCXLVRENDERINFO ri = new DCXLVRENDERINFO;

	if (ri == NULL) {
		delete lpmylvi;
		return;
	}

	// setup colum zero
	ri->m_dFlags = stateFlags;
	if (stateFlags & LVIS_COLOR)
		ri->m_cText = clrText;
	else
		ri->m_cText = CLR_INVALID;

	if (stateFlags & LVIS_BGCOLOR)
		ri->m_cBg = clrBack;
	else
		ri->m_cBg = CLR_INVALID;

	lpmylvi->vInfo.push_back(ri);

	TString itemtext;
	if (data.numtok( ) > 9) {
		itemtext = data.gettok(10, -1);

		char res[1024];
		if (stateFlags & LVIS_HASHITEM) {
			Dcx::mIRC.evalex(res, 1024, "$hget(%s,%s)", itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
			itemtext = res;
		}
		else if (stateFlags & LVIS_HASHNUMBER) {
			Dcx::mIRC.evalex(res, 1024,  "$hget(%s,%s).data", itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
			itemtext = res;
		}
	}

	lvi.iItem = nPos;
	lvi.iImage = -1;
	lvi.state = (stateFlags & 0xFFFF); // mask out higher number flags. These flags cause the add to fail & arnt needed here anyway.
	lvi.stateMask = (LVIS_FOCUSED|LVIS_SELECTED|LVIS_CUT|LVIS_DROPHILITED); // only alter the controls flags, ignore our custom ones.
	lvi.iSubItem = 0;
	lvi.lParam = (LPARAM) lpmylvi;
	lvi.mask = LVIF_PARAM|LVIF_STATE;

	if (icon > -1)
		lvi.iImage = icon;
	lvi.mask |= LVIF_IMAGE; // when this is a set & iImage is -1 the parent does the drawing (& in this case does nothing so no icon)
							// this fixes the icon being drawn even when no icon set.
							// leave this separate for now untill it's been well tested.
	
	// LVS_REPORT view
	if (this->isListViewStyle(LVS_REPORT)) {

		if (Dcx::XPPlusModule.isUseable() && group > 0) {
			if (ListView_IsGroupViewEnabled(this->m_Hwnd)) {
				if (ListView_HasGroup(this->m_Hwnd, group)) {
					lvi.iGroupId = group;
					lvi.mask |= LVIF_GROUPID;
				}
				else
					this->showErrorEx(NULL,"-a", "Invalid Group specified: %d", group);
			}
			else
				this->showError(NULL,"-a", "Can't add to a group when Group View is not enabled.");
		}

		if (indent > 0) {
			lvi.mask |= LVIF_INDENT;
			lvi.iIndent = indent;
		}

		// set text in case of pbar
		if (!(stateFlags & LVIS_PBAR)) {
			lvi.mask |= LVIF_TEXT;
			lvi.pszText = itemtext.to_chr();
		}

		lvi.iItem = ListView_InsertItem(this->m_Hwnd, &lvi);
		//BOOL result = FALSE;

		if (lvi.iItem == -1) {
			delete lpmylvi;
			delete ri;
			this->showError(NULL,"-a", "Unable to add item");
			return;
		}

		// create pbar for first column
		if (stateFlags & LVIS_PBAR)
			CreatePbar(&lvi, itemtext.gettok(1, -1));

		// subitems
		int tabs;

		if ((tabs = input.numtok(TSTAB)) > 1) {
			int i = 2;

			// ADD check for num columns
			while (i <= tabs) {
				data = input.gettok(i, TSTAB).trim();

				if (data.numtok() < 5) {
					this->showError(NULL,"-a", "Invalid subitem syntax");
					break;
				}

				stateFlags = parseItemFlags(data.gettok( 1 ));
				clrText = (COLORREF)data.gettok(4).to_num();
				clrBack = (COLORREF)data.gettok(5).to_num();

				// setup colum #
				ri = new DCXLVRENDERINFO;

				if (ri == NULL)
					break;

				ri->m_dFlags = stateFlags;

				if (stateFlags & LVIS_COLOR)
					ri->m_cText = clrText;
				else
					ri->m_cText = CLR_INVALID;

				if (stateFlags & LVIS_BGCOLOR)
					ri->m_cBg = clrBack;
				else
					ri->m_cBg = CLR_INVALID;

				lpmylvi->vInfo.push_back(ri);

				lvi.iSubItem = i -1;
				lvi.mask = LVIF_TEXT;

				// icon
				icon = data.gettok( 2 ).to_int() -1;

				lvi.mask |= LVIF_IMAGE; // moved here to turn off icon when none is wanted.
				if (icon > -1)
					lvi.iImage = icon;
				else
					lvi.iImage = -1;

				// overlay
				if ((overlay = data.gettok(3).to_int()) > 0) {
					lvi.mask |= LVIF_STATE;
					lvi.state |= INDEXTOOVERLAYMASK(overlay);
					lvi.stateMask |= LVIS_OVERLAYMASK;
				}
				else
					lvi.state |= INDEXTOOVERLAYMASK(0);

				itemtext = "";
				if (data.numtok() > 5) {
					itemtext = data.gettok(6, -1);

					char res[1024];
					if ((stateFlags & LVIS_HASHITEM) && (itemtext.numtok() == 2)) {
						Dcx::mIRC.evalex(res, 1024, "$hget(%s,%s)", itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
						itemtext = res;
					}
					else if ((stateFlags & LVIS_HASHNUMBER) && (itemtext.numtok() == 2)) {
						Dcx::mIRC.evalex(res, 1024,  "$hget(%s,%s).data", itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
						itemtext = res;
					}
				}
				// create pbar for subitem
				if (stateFlags & LVIS_PBAR) {
					CreatePbar(&lvi, itemtext);
					itemtext = "";
				}

				lvi.pszText = itemtext.to_chr();
				ListView_SetItem(this->m_Hwnd, &lvi);

				this->autoSize(i -1,data.gettok( 1 ));
				i++;
			}
		}

		if (state > -1)
			ListView_SetItemState(this->m_Hwnd, lvi.iItem, INDEXTOSTATEIMAGEMASK(state), LVIS_STATEIMAGEMASK);

		// overlay is 1-based index, max 15 overlay icons
		if (overlay > 0 && overlay < 16)
			ListView_SetItemState(this->m_Hwnd, lvi.iItem, INDEXTOOVERLAYMASK(overlay), LVIS_OVERLAYMASK);

		this->autoSize(0,input.gettok( 6 ));
	}
	// LVS_ICON | LVS_SMALLICON | LVS_LIST views
	else {

		lvi.mask |= LVIF_TEXT;
		lvi.pszText = itemtext.to_chr();
		lvi.iItem = ListView_InsertItem(this->m_Hwnd, &lvi);

		if (lvi.iItem == -1) {
			this->showError(NULL,"-a", "Unable to add item");
			return;
		}

		if (state > -1)
			ListView_SetItemState(this->m_Hwnd, lvi.iItem, INDEXTOSTATEIMAGEMASK(state), LVIS_STATEIMAGEMASK);

		// overlay is 1-based index
		if (overlay > 0 && overlay < 16)
			ListView_SetItemState(this->m_Hwnd, lvi.iItem, INDEXTOOVERLAYMASK(overlay), LVIS_OVERLAYMASK);
	}
}

void DcxListView::parseText2Item(const TString &tsTxt, TString &tsItem, const TString &tsData)
{
	// first part of item, fill in blanks with 0's
	//tsItem = "0 0 0 0 0 + 0 0 0 0 0 0 ";
	//// add item text.
	//tsItem += tsTxt.gettok(1, TSTAB);
	//// add all subitems
	//int tok = 2, ntok = tsTxt.numtok(TSTAB);
	//while (tok <= ntok) {
	//	//[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text
	//	// again fill in blanks with 0's
	//	tsItem.addtok("+ 0 0 0 0 ",TSTAB);
	//	// add subitems text.
	//	tsItem += tsTxt.gettok(tok, TSTAB);
	//	tok++;
	//}
	tsItem = "0 0 0 0 ";
	tsItem += tsData.gettok( 2, 10); // copy flags & icon etc.. from /xdid -a line.
	// add item text.
	tsItem.addtok(tsTxt.gettok(1, TSTAB).to_chr());
	// add all subitems
	int tok = 2, ntok = tsTxt.numtok(TSTAB);
	while (tok <= ntok) {
		//[+FLAGS] [#ICON] [#OVERLAY] [COLOR] [BGCOLOR] Item Text
		// again fill in blanks with 0's
		tsItem.addtok("+ 0 0 0 0 ",TSTAB); // subitems are added without flags, not going to change this.
		// add subitems text.
		tsItem += tsTxt.gettok(tok, TSTAB);
		tok++;
	}
}

