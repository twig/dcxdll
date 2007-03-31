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
 * © ScriptsDB.org - 2006
 */

#include "dcxlistview.h"
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

DcxListView::DcxListView( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles )
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles,
    DCX_LISTVIEWCLASS,
    NULL,
    WS_CHILD | WS_VISIBLE | (Styles & ~WS_CLIPCHILDREN), // can't be ws_clipchildren as this causes render bug when progressbar items are selected.
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL),
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

  SendMessage( this->m_Hwnd, CCM_SETVERSION, (WPARAM) 5, (LPARAM) 0 );

	this->parseListviewExStyles( styles, &ExStyles);

  ListView_SetExtendedListViewStyleEx( this->m_Hwnd, ExStyles, ExStyles);

	this->m_ToolTipHWND = ListView_GetToolTips(this->m_Hwnd);

	if (this->m_ToolTipHWND != NULL) {
		if (styles.istok("balloon"))
			SetWindowLong(this->m_ToolTipHWND,GWL_STYLE,GetWindowLong(this->m_ToolTipHWND,GWL_STYLE) | TTS_BALLOON);
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
		else if ( styles.gettok( i ) == "alpha" )
			this->m_bAlphaBlend = true;

    i++;
  }
  this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

void DcxListView::parseListviewExStyles( TString & styles, LONG * ExStyles )
{
	//*ExStyles = 0;
	//if (!isXP())
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
				//DCXError("$ $+ xdid(listview).columns", "could not find header");
				return;
			}

			wsprintf(szReturnValue, "%d", (int) Header_GetItemCount(hHeader));
		}
		else
			lstrcpy(szReturnValue, "0");

		return;
	}
  // [NAME] [ID] [PROP] [N] [NSUB]
  else if ( prop == "text" && numtok > 4 ) {

    int nItem = input.gettok( 4 ).to_int( ) - 1;
    int nSubItem = input.gettok( 5 ).to_int( );

    if ( nItem > -1 && nItem < ListView_GetItemCount( this->m_Hwnd ) ) {
      ListView_GetItemText( this->m_Hwnd, nItem, nSubItem, szReturnValue, 900 );
      return;
    }
  }
  // [NAME] [ID] [PROP] [N] [NSUB]
  else if ( prop == "icon" && numtok > 4 ) {

    int nItem = input.gettok( 4 ).to_int( ) - 1;
    int nSubItem = input.gettok( 5 ).to_int( );

    if ( nItem > -1 && nItem < ListView_GetItemCount( this->m_Hwnd ) ) {

      LVITEM lvi;
      lvi.mask = LVIF_IMAGE;
      lvi.iItem = nItem;
      lvi.iSubItem = nSubItem;

      ListView_GetItem( this->m_Hwnd, &lvi );
      wsprintf( szReturnValue, "%d", lvi.iImage + 1 );
      return;
    }
  }
  // [NAME] [ID] [PROP] [NSUB]
  else if ( prop == "seltext" && numtok > 3 ) {

    int nItem = ListView_GetNextItem( this->m_Hwnd, -1, LVIS_SELECTED );
    int nSubItem = input.gettok( 4 ).to_int( );

    if ( nItem > -1 ) {

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
				char line[5];
				int i = 1;

				while ((nItem = ListView_GetNextItem(this->m_Hwnd, nItem, LVIS_SELECTED)) != -1) {
#ifdef VS2005
					_itoa(nItem + 1, line, 10);
#else
					itoa(nItem + 1, line, 10);
#endif          
					list += line;

					if (i != nSelItems)
						list += ',';
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

    TString matchtext(input.gettok( 2, TSTAB ));
    matchtext.trim( );
    TString params(input.gettok( 3, TSTAB ));
    params.trim( );

    if ( matchtext.len( ) > 0 ) {

      UINT SearchType;

      if ( params.gettok( 1 ) == "R" )
        SearchType = LVSEARCH_R;
      else
        SearchType = LVSEARCH_W;

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
    ScreenToClient( this->m_Hwnd, &lvh.pt );
    ListView_HitTest( this->m_Hwnd, &lvh );

    if ( lvh.flags & LVHT_ONITEM )
      wsprintf( szReturnValue, "%d", lvh.iItem + 1 );
    else
      wsprintf( szReturnValue, "%d", -1 );
      
    return;
  }
  // [NAME] [ID] [PROP] [N]
  else if ( prop == "hwidth" && numtok > 3 ) {

    int nColumn = input.gettok( 4 ).to_int( ) - 1;

    if ( nColumn > -1 && nColumn < this->getColumnCount( ) ) {

      wsprintf( szReturnValue, "%d", ListView_GetColumnWidth( this->m_Hwnd, nColumn ) );
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
    
    if ( isXP( ) && ListView_GetGroupInfo( this->m_Hwnd, GID, &lvg ) != -1 ) {
      int n = WideCharToMultiByte( CP_ACP, 0, wstr, lstrlenW( wstr ) + 1, szReturnValue, 900, NULL, NULL );
      TString error;
      error.sprintf("Chars %d", n );
			mIRCError( error.to_chr() );
      mIRCError( szReturnValue );
      return;
    }
  }
  // [NAME] [ID] [PROP] [N]
  else if ( prop == "genabled" ) {

    if ( isXP( ) && ListView_IsGroupViewEnabled( this->m_Hwnd ) )
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
		//int nSubItem = input.gettok( 5 ).to_int();

		if (nItem > -1 && nItem < ListView_GetItemCount(this->m_Hwnd)) {
			LVITEM lvi;

			ZeroMemory(&lvi, sizeof(LVITEM));
			lvi.mask = LVIF_PARAM;

			ListView_GetItem(this->m_Hwnd, &lvi);

			LPDCXLVITEM lvdcx = (LPDCXLVITEM) lvi.lParam;

			if (!lvdcx->pbar)
				return;

			TString cmd = input.gettok( 1 ) + " " + input.gettok( 2 ) + " " + input.gettok(6, -1);
			lvdcx->pbar->parseInfoRequest(cmd, szReturnValue);

			//ListView_GetItemText(this->m_Hwnd, nItem, nSubItem, szReturnValue, 900);
			return;
		}

		return;
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == "gnum" ) {
	}
	else if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
		return;

  szReturnValue[0] = 0;
}

void DcxListView::autoSize(const int nColumn, TString &flags)
{
	UINT iFlags = this->parseHeaderFlags2(flags);

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
	XSwitchFlags flags;

	ZeroMemory((void*) &flags, sizeof(XSwitchFlags));
	this->parseSwitchFlags(input.gettok( 3 ), &flags);

	int numtok = input.numtok( );

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags.switch_flags[17]) {
		ListView_DeleteAllItems(this->m_Hwnd);
	}

	//xdid -a [NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] [COLOR] [BGCOLOR] Item Text ...
	if (flags.switch_flags[0] && numtok > 12) {
		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));

		TString data(input.gettok(1, TSTAB).gettok(4, -1));
		data.trim();
		int nPos = (int)data.gettok( 1 ).to_num() -1;

		if (nPos < 0)
			nPos = ListView_GetItemCount(this->m_Hwnd);

		int indent = (int)data.gettok( 2 ).to_num();
		UINT stateFlags = this->parseItemFlags(data.gettok( 3 ));
		int icon = (int)data.gettok( 4 ).to_num() -1;
		int state = (int)data.gettok( 5 ).to_num();
		//int overlay = data.gettok( 6 ).to_int( );
		int group = (int)data.gettok( 7 ).to_num();
		COLORREF clrText = (COLORREF)data.gettok( 8 ).to_num();
		COLORREF clrBack = (COLORREF)data.gettok( 9 ).to_num();

		LPDCXLVITEM lpmylvi = new DCXLVITEM;
		ZeroMemory(lpmylvi, sizeof(DCXLVITEM));
		lpmylvi->vInfo.clear();

		LPDCXLVRENDERINFO ri = new DCXLVRENDERINFO;

		// setup colum zero
		ri->m_dFlags = stateFlags;
		if (stateFlags & LVIS_COLOR)
			ri->m_cText = clrText;
		else
			ri->m_cText = -1;

		if (stateFlags & LVIS_BGCOLOR)
			ri->m_cBg = clrBack;
		else
			ri->m_cBg = -1;

		lpmylvi->vInfo.push_back(ri);

		TString itemtext;
		if (data.numtok( ) > 9)
			itemtext = data.gettok(10, -1);

		// LVS_REPORT view
		if (this->isListViewStyle(LVS_REPORT)) {

			lvi.mask = LVIF_TEXT | LVIF_INDENT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE;
			lvi.iItem = nPos;
			lvi.iImage = -1;
			lvi.state = stateFlags;
			lvi.iSubItem = 0;
			lvi.iIndent = indent;

			if (isXP() && group > 0) {
				lvi.iGroupId = group;
				lvi.mask |= LVIF_GROUPID;
			}

			if (icon > -1)
				lvi.iImage = icon;

			// set text in case of pbar
			if (stateFlags & LVIS_PBAR)
				lvi.pszText = "";
			else
				lvi.pszText = itemtext.to_chr();

			lvi.lParam = (LPARAM) lpmylvi;
			lvi.iItem = ListView_InsertItem(this->m_Hwnd, &lvi);
			if (lvi.iItem == -1) {
				this->showError(NULL,"-a", "Unable to add item");
				//DCXError("/xdid -a","Unable to add item");
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
					data = input.gettok(i, TSTAB);
					data.trim();

					if (data.numtok() < 4) {
						this->showError(NULL,"-a", "Invalid subitem syntax");
						//DCXError("/xdid -a", "Invalid subitem syntax");
						break;
					}
					stateFlags = parseItemFlags(data.gettok( 1 ));
					clrText = (COLORREF)data.gettok( 3 ).to_num();
					clrBack = (COLORREF)data.gettok( 4 ).to_num();

					// setup colum #
					ri = new DCXLVRENDERINFO;
					ri->m_dFlags = stateFlags;
					if (stateFlags & LVIS_COLOR)
						ri->m_cText = clrText;
					else
						ri->m_cText = -1;

					if (stateFlags & LVIS_BGCOLOR)
						ri->m_cBg = clrBack;
					else
						ri->m_cBg = -1;
					lpmylvi->vInfo.push_back(ri);

					lvi.iSubItem = i -1;
					lvi.mask = LVIF_TEXT | LVIF_IMAGE;

					// icon
					icon = (int)data.gettok( 2 ).to_num() -1;

					if (icon > -1)
						lvi.iImage = icon;
					else
						lvi.iImage = -1;

					itemtext = "";
					if (data.numtok( ) > 4)
						itemtext = data.gettok(5, -1);

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
			// if ( overlay > -1 )
			//ListView_SetItemState(hwnd, lvi.iItem, INDEXTOSTATEIMAGEMASK(overlay), LVIS_OVERLAYMASK);
			this->autoSize(0,input.gettok( 6 ));
		}
		// LVS_ICON | LVS_SMALLICON | LVS_LIST views
		else {

			lvi.iItem = nPos;
			lvi.iImage = -1;
			lvi.state = stateFlags;
			lvi.iSubItem = 0;
			lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;

			if (icon > -1) {
				lvi.iImage = icon;
				lvi.mask |= LVIF_IMAGE;
			}

			lvi.pszText = itemtext.to_chr();
			lvi.lParam = (LPARAM) lpmylvi;
			lvi.iItem = ListView_InsertItem(this->m_Hwnd, &lvi);

			if (lvi.iItem == -1) {
				this->showError(NULL,"-a", "Unable to add item");
				//DCXError("/xdid -a","Unable to add item");
				return;
			}

			if (state > -1)
				ListView_SetItemState(this->m_Hwnd, lvi.iItem, INDEXTOSTATEIMAGEMASK(state), LVIS_STATEIMAGEMASK);
			//ListView_SetItemState(hwnd, lvi.iItem, INDEXTOSTATEIMAGEMASK(overlay), LVIS_OVERLAYMASK);
		}
	}
	// xdid -B [NAME] [ID] [SWITCH] [N]
	else if (flags.switch_cap_flags[1] && numtok > 3) {
		int nItem = (int)input.gettok( 4 ).to_num() -1;

		if (nItem > -1)
			ListView_EditLabel(this->m_Hwnd, nItem);
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags.switch_flags[2] && numtok > 3) {
		if (this->isStyle(LVS_SINGLESEL)) {
			int nItem = (int)input.gettok( 4 ).to_num() -1;

			if (nItem > -1)
				ListView_SetItemState(this->m_Hwnd, nItem, LVIS_SELECTED, LVIS_SELECTED);
		}
		else {
			TString Ns(input.gettok( 4 ));
			int i = 1;
			int n = Ns.numtok(TSCOMMA);
			int nItems = ListView_GetItemCount(this->m_Hwnd);

			while (i <= n) {
				int nItem = Ns.gettok(i, TSCOMMA).to_int() -1;

				if (nItem > -1 && nItem < nItems)
					ListView_SetItemState(this->m_Hwnd, nItem, LVIS_SELECTED, LVIS_SELECTED);

				i++;
			}
		}
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags.switch_flags[3] && (numtok > 3)) {
		int nItem = (int)input.gettok( 4 ).to_num() -1;

		if (nItem > -1)
			ListView_DeleteItem(this->m_Hwnd, nItem);
		/*
		// delete pbars if in row
		for (int i = (int) m_lvpbars.size() -1; i >= 0 ; i--) {
		DCXLVPBAR* pbarCell = &(m_lvpbars[i]);

		if (pbarCell->row == nItem) {
		DestroyWindow(pbarCell->pbar->getHwnd());
		//delete pbarCell->pbar;

		m_lvpbars.erase(m_lvpbars.begin() + i, m_lvpbars.begin() + i +1);
		}
		}

		// shift the pbars below it to keep alignment correct
		for (int i = 0; i < (int) m_lvpbars.size(); i++) {
		DCXLVPBAR* pbarCell = &(m_lvpbars[i]);

		if (pbarCell->row > nItem) {
		pbarCell->row--;
		}
		}

		ResizePbars();
		*/
	}
	// xdid -g [NAME] [ID] [SWITCH] [+FLAGS] [X] [Y] (FILENAME) ([tab] watermark filename)
	else if (flags.switch_flags[6] && numtok > 5) {
		LVBKIMAGE lvbki;
		ZeroMemory(&lvbki, sizeof(LVBKIMAGE));
		TString filename;
		//TString watermarkfile;

		lvbki.ulFlags = this->parseImageFlags(input.gettok( 4 ));
		lvbki.xOffsetPercent = (int)input.gettok( 5 ).to_num();
		lvbki.yOffsetPercent = (int)input.gettok( 6 ).to_num();

		if (numtok > 6) {
			filename = input.gettok(7, -1);
			//filename = input.gettok(7, -1).gettok(1,TSTAB);
			filename.trim();
			//if (lvbki.ulFlags & LVBKIF_TYPE_WATERMARK) {
			//	watermarkfile = input.gettok(7,-1).gettok(2,-1,TSTAB);
			//	watermarkfile.trim();
			//	lvbki.hbm = dcxLoadBitmap(lvbki.hbm, watermarkfile);
			//}
			lvbki.pszImage = filename.to_chr();
			lvbki.ulFlags |= LVBKIF_SOURCE_URL;
		}

		ListView_SetBkImage(this->m_Hwnd, &lvbki);
	}
	// xdid -i [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if (flags.switch_flags[8] && numtok > 4) {
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
	else if (flags.switch_flags[9] && numtok > 5) {
		int nItem = input.gettok( 4 ).to_int() -1;
		int nCol = input.gettok( 5 ).to_int() -1;
		COLORREF clrText = (COLORREF)input.gettok( 7 ).to_num();
		COLORREF clrBack = (COLORREF)input.gettok( 8 ).to_num();

		// invalid info
		if ((nItem < 0) || (nCol < 0)) {
			this->showError(NULL,"-j", "Invalid Item");
			//DCXError("/xdid -j","Invalid Item");
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
			//DCXError("/xdid -j","Unable to get Item.");
			return;
		}

		UINT flags = this->parseItemFlags(input.gettok( 6 ));
		LPDCXLVITEM lviDcx = (LPDCXLVITEM) lvi.lParam;

		if (lviDcx != NULL) {
			if ((UINT)nCol < lviDcx->vInfo.size()) {
				lviDcx->vInfo[nCol]->m_dFlags = flags;
				if (flags & LVIS_COLOR)
					lviDcx->vInfo[nCol]->m_cText = clrText;
				else
					lviDcx->vInfo[nCol]->m_cText = -1;

				if (flags & LVIS_BGCOLOR)
					lviDcx->vInfo[nCol]->m_cBg = clrBack;
				else
					lviDcx->vInfo[nCol]->m_cBg = -1;

				ListView_SetItemState(this->m_Hwnd, nItem, flags, 0xFFFFFF);
			}
			else
				this->showError(NULL, "-j", "No Render Information for SubItem, More subitems than columns?");
				//DCXError("/xdid -j","No Render Information for SubItem, More subitems than columns?");
		}
		else
			this->showError(NULL, "-j", "No DCX Item Information, somethings very wrong");
			//DCXError("/xdid -j","No DCX Item Information, somethings very wrong");
	}
	// xdid -k [NAME] [ID] [SWITCH] [STATE] [N]
	else if (flags.switch_flags[10] && numtok > 4) {
		int state = (int)input.gettok( 4 ).to_num();
		int nItem = (int)input.gettok( 5 ).to_num() -1;

		ListView_SetItemState(this->m_Hwnd, nItem, INDEXTOSTATEIMAGEMASK(state), LVIS_STATEIMAGEMASK);
	}
	// xdid -l [NAME] [ID] [SWITCH] [N] [M] [ICON]
	else if (flags.switch_flags[11] && numtok > 5) {
		int nItem    = (int)input.gettok( 4 ).to_num() -1;
		int nSubItem = (int)input.gettok( 5 ).to_num();
		int nIcon    = (int)input.gettok( 6 ).to_num() -1;

		if (nItem > -1 && nSubItem > -1 && nSubItem <= this->getColumnCount() && nIcon > -2) {
			LVITEM lvi;

			lvi.mask = LVIF_IMAGE;
			lvi.iItem = nItem;
			lvi.iSubItem = nSubItem;
			lvi.iImage = nIcon;
			ListView_SetItem(this->m_Hwnd, &lvi);
		}
	}
	// xdid -m [NAME] [ID] [SWITCH] [0|1]
	else if (flags.switch_flags[12] && numtok > 3) {
		if (input.gettok( 4 ) == "1")
			ListView_EnableGroupView(this->m_Hwnd, TRUE);
		else
			ListView_EnableGroupView(this->m_Hwnd, FALSE);
	}
	// xdid -n [NAME] [ID] [SWITCH] [N] [+FLAGS] (WIDTH)
	else if (flags.switch_flags[13] && numtok > 4) {
		int nColumn = (int)input.gettok( 4 ).to_num();
		UINT iFlags = this->parseHeaderFlags2(input.gettok( 5 ));

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
					//DCXError("/xdid -n","No width specified");
			}
		}
	}
	// xdid -q [NAME] [ID] [SWITCH] [N] [+FLAGS] [GID] [Group Text]
	else if (flags.switch_flags[16] && numtok > 6) {
		int index = (int)input.gettok( 4 ).to_num() -1;
		UINT iFlags = this->parseGroupFlags(input.gettok( 5 ));
		int gid = (int)input.gettok( 6 ).to_num();

		if (isXP() && index > -1 && gid > 0) {
			TString text(input.gettok(7, -1));

			LVGROUP lvg;
			ZeroMemory(&lvg, sizeof(LVGROUP));
			lvg.cbSize = sizeof(LVGROUP);
			lvg.mask = LVGF_ALIGN | LVGF_HEADER | LVGF_GROUPID;

			LPWSTR wstr = new WCHAR[text.len() + 1];
			MultiByteToWideChar(CP_ACP, 0, text.to_chr(), text.len() +1, wstr, text.len() +1);
			//LPWSTR wstr = text.to_wchr(); // can this buffer be deleted? or is it needed by the control? requires testing.

			lvg.iGroupId = gid;
			lvg.pszHeader = wstr;
			lvg.uAlign = iFlags;

			ListView_InsertGroup(this->m_Hwnd, index, &lvg);
		}
	}
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags.switch_flags[17]) {
		//ListView_DeleteAllItems(this->m_Hwnd);
	}
	// xdid -t [NAME] [ID] [SWITCH] [+FLAGS] [#ICON] [WIDTH] (Header text) [{TAB} [+FLAGS] [#ICON] [WIDTH] Header text {TAB} ... ]
	else if (flags.switch_flags[19] && numtok > 5) {
		int nCol = this->getColumnCount();

		if (nCol > 0) {
			while (--nCol > 0)
				ListView_DeleteColumn(this->m_Hwnd, nCol);
		}

		LVCOLUMN lvc;
		ZeroMemory(&lvc, sizeof(LVCOLUMN));
		int nColumn = 0;
		TString data(input.gettok(1, TSTAB ).gettok(4, -1));
		data.trim();

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

				data = input.gettok(i, TSTAB);
				data.trim();

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
	else if (flags.switch_flags[20]) {
		ListView_SetItemState(this->m_Hwnd, -1, 0, LVIS_SELECTED);
	}
	// xdid -v [NAME] [ID] [SWITCH] [N] [M] (ItemText)
	else if (flags.switch_flags[21] && numtok > 4) {
		int nItem = input.gettok( 4 ).to_int() - 1;
		int nSubItem = input.gettok( 5 ).to_int();

		if (nItem > -1) {
			TString itemtext(input.gettok(6, -1));
			itemtext.trim();

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
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags.switch_flags[22] && numtok > 5) {
		TString tflags(input.gettok( 4 ));
		UINT iFlags = this->parseIconFlagOptions(tflags);
		HIMAGELIST himl;
		HICON icon;
		int index = input.gettok( 5 ).to_int();
		TString filename(input.gettok(6, -1));

		if (iFlags & LVSIL_SMALL) {

			icon = dcxLoadIcon(index, filename, true, tflags);

			if (icon != NULL) {
				if ((himl = this->getImageList(LVSIL_NORMAL)) == NULL) {
					himl = this->createImageList(TRUE);

					if (himl != NULL)
						this->setImageList(himl, LVSIL_NORMAL);
				}
				if (himl != NULL)
					ImageList_AddIcon(himl, icon);
				DestroyIcon(icon);
			}
			else {
				this->showError(NULL, "-w", "Unable to Load Icon");
				//DCXError("/xdid -w", "Unable to Load Icon");
				return;
			}

			icon = dcxLoadIcon(index, filename, false, tflags);

			if (icon != NULL) {
				if ((himl = this->getImageList(LVSIL_SMALL)) == NULL) {
					himl = this->createImageList(FALSE);

					if (himl)
						this->setImageList(himl, LVSIL_SMALL);
				}
				if (himl != NULL)
					ImageList_AddIcon(himl, icon);
				DestroyIcon(icon);
			}
		}

		if (iFlags & LVSIL_STATE) {

			icon = dcxLoadIcon(index, filename, false, tflags);

			if (icon != NULL) {
				if ((himl = this->getImageList(LVSIL_STATE)) == NULL) {
					himl = this->createImageList(FALSE);

					if (himl)
						this->setImageList(himl, LVSIL_STATE);
				}
				if (himl != NULL)
					ImageList_AddIcon(himl, icon);
				DestroyIcon(icon);
			}
		}
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if (flags.switch_flags[24] && numtok > 3) {
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
	else if (flags.switch_flags[25] && numtok > 4) {
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
	else if (flags.switch_cap_flags[19] && numtok > 4) {
		input.trim();
		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));

		lvi.iItem = input.gettok( 4 ).to_int();
		lvi.iSubItem = input.gettok( 5 ).to_int();
		lvi.mask = LVIF_PARAM;

		if (ListView_GetItem(this->m_Hwnd,&lvi)) {
			LPDCXLVITEM lpmylvi = (LPDCXLVITEM) lvi.lParam;

			if (lpmylvi != NULL) {
				lpmylvi->tsTipText = (numtok > 3 ? input.gettok(6, -1) : "");
				LVSETINFOTIP it;
				ZeroMemory(&it, sizeof(it));
				it.cbSize = sizeof(it);
				it.iItem = lvi.iItem;
				it.iSubItem = lvi.iSubItem;
				it.pszText = lpmylvi->tsTipText.to_wchr();
				ListView_SetInfoTip(this->m_Hwnd,&it);
			}
		}
	}
	// xdid -Z [NAME] [ID] [SWITCH] [relative pixels]
	else if (flags.switch_cap_flags[25] && numtok > 3) {
		int pos = input.gettok( 4 ).to_int( );
		// this is a temp fix untill we find out why WM_VSCROLL isn't working.
		ListView_Scroll(this->m_Hwnd, 0, pos);
	}
	// xdid -V [NAME] [ID] [SWITCH] [nItem]
	else if (flags.switch_cap_flags[21] && numtok > 3) {
		int nItem = input.gettok( 4 ).to_int() -1;

		if (nItem > -1)
			ListView_EnsureVisible(this->m_Hwnd, nItem, FALSE);
	}
	else
		this->parseGlobalCommandRequest(input, flags);
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

UINT DcxListView::parseIconFlagOptions( TString & flags ) {

  UINT i = 1, len = flags.len( ), iFlags = 0;

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

UINT DcxListView::parseItemFlags(TString & flags) {
	INT i = 1, len = flags.len(), iFlags = 0;

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

		++i;
	}

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxListView::parseHeaderFlags( TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

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

UINT DcxListView::parseHeaderFlags2( TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

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

UINT DcxListView::parseSortFlags( TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

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

UINT DcxListView::parseGroupFlags( TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

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

UINT DcxListView::parseColorFlags( TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

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

UINT DcxListView::parseImageFlags( TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

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

  long dwStyle = GetWindowLong( this->m_Hwnd, GWL_STYLE ); 
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
	if (SearchType == LVSEARCH_R) {
		TString com;
		char res[10];
		//com.sprintf("$regex(%s,%s)", itemtext, search->to_chr( ) );
		com.sprintf("/set -nu1 %%dcx_text %s", itemtext );
		mIRCcom(com.to_chr());
		com.sprintf("/set -nu1 %%dcx_regex %s", search->to_chr( ) );
		mIRCcom(com.to_chr());
		com = "$regex(%dcx_text,%dcx_regex)";
		mIRCeval( com.to_chr(), res );
		if ( !lstrcmp( res, "1" ) )
			return TRUE;
	}
	else {
		TString text(itemtext);
		return text.iswm(search->to_chr());
	}
 // char res[10];
 // char itemtext[900];
 // TString com;

 // ListView_GetItemText( this->m_Hwnd, nItem, nSubItem, itemtext, 900 );
 // // Regex Search
 // if ( SearchType == LVSEARCH_R )
 //   com.sprintf("$regex(%s,%s)", itemtext, search->to_chr( ) );
 // // Wildcard Search
 // else
 //   com.sprintf("$iif(%s iswm %s,1,0)", search->to_chr( ), itemtext );

	//mIRCeval( com.to_chr(), res );

 // if ( !lstrcmp( res, "1" ) )
 //     return TRUE;

	return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxListView::getColumnCount( ) const {

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
		TString com;
		char res[20];

    com.sprintf("$%s(%s,%s)", plvsort->tsCustomAlias.to_chr( ), itemtext1, itemtext2 );
		mIRCeval( com.to_chr(), res );

    if ( plvsort->iSortFlags & LVSS_DESC ) {

      if ( lstrcmp( res, "-1" ) )
        return -1;
      else if ( lstrcmp( res, "1" ) )
        return 1;
    }
    else {

      if ( lstrcmp( res, "-1" ) )
        return 1;
      else if ( lstrcmp( res, "1" ) )
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
    case WM_NOTIFY : 
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
								//GetCursorPos( &lvh.pt );
								//ScreenToClient( this->m_Hwnd, &lvh.pt );
								ListView_SubItemHitTest( this->m_Hwnd, &lvh );

								if ( ( lvh.flags & LVHT_ONITEMSTATEICON ) &&
									( ListView_GetExtendedListViewStyle( this->m_Hwnd ) & LVS_EX_CHECKBOXES ) &&
									!( lvh.flags & LVHT_ONITEMICON ) &&
									!( lvh.flags & LVHT_ONITEMLABEL ) ) 
								{
							//TODO: int state = ListView_GetCheckState(this->m_Hwnd, lvh.iItem);
									this->callAliasEx( NULL, "%s,%d,%d,%d", "stateclick", this->getUserID( ), lvh.iItem + 1, lvh.iSubItem );
								}
								else if ( lvh.flags & LVHT_ONITEM )
									this->callAliasEx( NULL, "%s,%d,%d,%d", "sclick", this->getUserID( ), lvh.iItem + 1, lvh.iSubItem );
								else if (lvh.flags & LVHT_NOWHERE)
									this->callAliasEx(NULL, "%s,%d", "sclick", this->getUserID());
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
								//GetCursorPos( &lvh.pt );
								//ScreenToClient( this->m_Hwnd, &lvh.pt );
								ListView_SubItemHitTest( this->m_Hwnd, &lvh );

								if ( lvh.flags & LVHT_ONITEM )
									this->callAliasEx( NULL, "%s,%d,%d,%d", "dclick", this->getUserID( ), lvh.iItem + 1, lvh.iSubItem );
								else
									this->callAliasEx( NULL, "%s,%d", "dclick", this->getUserID());
							}
            }
            break;

          case NM_RCLICK:
            {
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								LVHITTESTINFO lvh;
								LPNMITEMACTIVATE nmia = (LPNMITEMACTIVATE)lParam;
								lvh.pt = nmia->ptAction;
								//GetCursorPos( &lvh.pt );
								//ScreenToClient( this->m_Hwnd, &lvh.pt );
								ListView_SubItemHitTest( this->m_Hwnd, &lvh );

								if ( lvh.flags & LVHT_ONITEM )
									this->callAliasEx( NULL, "%s,%d,%d,%d", "rclick", this->getUserID( ), lvh.iItem + 1, lvh.iSubItem );
								else
									this->callAliasEx( NULL, "%s,%d", "rclick", this->getUserID());
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
									this->callAliasEx( NULL, "%s,%d,%d,%d", "rdclick", this->getUserID( ), lvh.iItem + 1, lvh.iSubItem );
								else
									this->callAliasEx( NULL, "%s,%d", "rdclick", this->getUserID());
							}
							bParsed = TRUE;
            }
            break;

          case NM_HOVER:
            {
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								LVHITTESTINFO lvh;
								GetCursorPos( &lvh.pt );
								ScreenToClient( this->m_Hwnd, &lvh.pt );
								ListView_SubItemHitTest( this->m_Hwnd, &lvh );

								if ( lvh.flags & LVHT_ONITEM )
									this->callAliasEx( NULL, "%s,%d,%d,%d", "hover", this->getUserID( ), lvh.iItem + 1, lvh.iSubItem );
								else
									this->callAliasEx( NULL, "%s,%d", "hover", this->getUserID());
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

              this->m_OrigEditProc = (WNDPROC) SetWindowLong( edit_hwnd, GWL_WNDPROC, (LONG) DcxListView::EditLabelProc );
              SetProp( edit_hwnd, "dcx_pthis", (HANDLE) this );

              char ret[256];
              this->callAliasEx( ret, "%s,%d", "labelbegin", this->getUserID( ) );

              if ( !lstrcmp( "noedit", ret ) )
                return TRUE;
            }
            break;

          case LVN_ENDLABELEDIT:
            {

              bParsed = TRUE;

              LPNMLVDISPINFO lplvdi = (LPNMLVDISPINFO) lParam;

              if ( lplvdi->item.pszText == NULL ) {

                this->callAliasEx( NULL, "%s,%d", "labelcancel", this->getUserID( ) );
              }
              else {
	              char ret[256];
                this->callAliasEx( ret, "%s,%d,%s", "labelend", this->getUserID( ), lplvdi->item.pszText );

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
	                  return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;

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
										if ( ri->m_cText != -1 )
											lplvcd->clrText = ri->m_cText;

										if ( ri->m_cBg != -1 )
											lplvcd->clrTextBk = ri->m_cBg;

										//DrawFrameControl(lplvcd->nmcd.hdc,&lplvcd->nmcd.rc,DFC_BUTTON,DFCS_BUTTONCHECK|DFCS_CHECKED);
										//return CDRF_SKIPDEFAULT;

										if (ri->m_dFlags & LVIS_UNDERLINE || ri->m_dFlags & LVIS_BOLD || ri->m_dFlags & LVIS_ITALIC) {
											HFONT hFont = (HFONT) SendMessage(this->m_Hwnd, WM_GETFONT, 0, 0);
											LOGFONT lf;

											GetObject(hFont, sizeof(LOGFONT), &lf);

											if (ri->m_dFlags & LVIS_BOLD)
												lf.lfWeight |= FW_BOLD;
											if (ri->m_dFlags & LVIS_UNDERLINE)
												lf.lfUnderline = true;
											if (ri->m_dFlags & LVIS_ITALIC)
												lf.lfItalic = true;

											HFONT hFontNew = CreateFontIndirect( &lf );
											//HFONT hOldFont = (HFONT) SelectObject( lplvcd->nmcd.hdc, hFontNew );
											SelectObject(lplvcd->nmcd.hdc, hFontNew);

											DeleteObject(hFontNew);
										}
										return ( CDRF_NEWFONT );
									}
									break;

								case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
									return CDRF_DODEFAULT;

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


          case LVN_BEGINDRAG:
            {
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_DRAG)
	              this->callAliasEx( NULL, "%s,%d", "begindrag", this->getUserID( ) );
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
        } // switch
      }
      break;
	}
	return 0L;
}

LRESULT DcxListView::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
  switch( uMsg ) {

    case WM_HELP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
	        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
      }
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
										ScreenToClient( hdr->hwndFrom, &hdti.pt );
										if ( SendMessage( hdr->hwndFrom, HDM_HITTEST, (WPARAM) 0, (LPARAM) &hdti ) != -1 )
											this->callAliasEx( NULL, "%s,%d,%d", "hrclick", this->getUserID( ), hdti.iItem + 1 );
									}
								}
								bParsed = TRUE;
							}
							break;
						case HDN_BEGINTRACKW:
						case HDN_BEGINTRACK:
							{
								bParsed = TRUE;

								char ret[256];
								this->callAliasEx( ret, "%s,%d", "trackbegin", this->getUserID( ) );

								if ( !lstrcmp( "notrack", ret ) )
									return TRUE;
							}
							break;

						case HDN_ITEMCLICKW:
						case HDN_ITEMCLICK:
							{
								bParsed = TRUE;

								if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
									LPNMHEADER lphdr = (LPNMHEADER) lParam;
									this->callAliasEx( NULL, "%s,%d,%d", "hsclick", this->getUserID( ), lphdr->iItem + 1 );
								}
							}
							break;

						case HDN_ITEMDBLCLICKW:
						case HDN_ITEMDBLCLICK:
							{
								bParsed = TRUE;

								if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
									LPNMHEADER lphdr = (LPNMHEADER) lParam;
									this->callAliasEx( NULL, "%s,%d,%d", "hdclick", this->getUserID( ), lphdr->iItem + 1 );
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
						//		this->callAliasEx( NULL, "%s,%d", "tooltiplink", this->getUserID( ) );
						//	}
						//	break;
					} // switch
				//}
      }
      break;

		case WM_HSCROLL:
			{
				//if (IsWindow((HWND) lParam)) {
				//	DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,"dcx_cthis");
				//	if (c_this != NULL) {
				//		lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				//	}
				//	else {
						if (this->isExStyle(LVS_EX_GRIDLINES)) {
							this->redrawWindow();
						}
				//	}
				//}
				break;
			}

		case WM_VSCROLL:
		{
			//if (IsWindow((HWND) lParam)) {
			//	DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,"dcx_cthis");
			//	if (c_this != NULL) {
			//		lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			//	}
			//	else {
					if (LOWORD(wParam) == SB_ENDSCROLL)
						this->callAliasEx(NULL, "%s,%d", "scrollend", this->getUserID());

					if (this->isExStyle(LVS_EX_GRIDLINES))
						this->redrawWindow();
			//	}
			//}
			break;
		}

    case WM_MOUSEMOVE:
      {
        this->m_pParentDialog->setMouseControl( this->getUserID( ) );
      }
      break;

    case WM_SETFOCUS:
      {
        this->m_pParentDialog->setFocusControl( this->getUserID( ) );
      }
      break;

		case WM_DROPFILES:
			{
				HDROP files = (HDROP) wParam;
				char filename[500];
				int count = DragQueryFile(files, 0xFFFFFFFF,  filename, 500);

				if (count) {
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_DRAG) {
						char ret[20];

						this->callAliasEx(ret, "%s,%d,%d", "dragbegin", this->getUserID(), count);

						// cancel drag drop event
						if (lstrcmpi(ret, "cancel") == 0) {
							DragFinish(files);
							return 0L;
						}

						// for each file, send callback message
						for (int i = 0; i < count; i++) {
							if (DragQueryFile(files, i, filename, 500))
								this->callAliasEx(ret, "%s,%d,%s", "dragfile", this->getUserID(), filename);
						}

						this->callAliasEx(ret, "%s,%d", "dragfinish", this->getUserID());
					}
				}

				DragFinish(files);
				break;
			}
		case WM_SETCURSOR:
			{
				if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd && this->m_hCursor != NULL ) {
					if (GetCursor() != this->m_hCursor)
						SetCursor( this->m_hCursor );
					bParsed = TRUE;
					return TRUE;
				}
			}
			break;
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
        SetWindowLong( mHwnd, GWL_WNDPROC, (LONG) pthis->m_OrigEditProc );
      }
      break;

  }
  return CallWindowProc( pthis->m_OrigEditProc, mHwnd, uMsg, wParam, lParam );
}


DcxControl* DcxListView::CreatePbar(LPLVITEM lvi, TString &styles) {
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
	lpdcxlvi->pbar = (DcxControl *)new DcxProgressBar(this->getID(), this->m_pParentDialog, this->m_Hwnd, &rItem, styles);
	return lpdcxlvi->pbar;
}


void DcxListView::UpdateScrollPbars() {
	for (int row = 0; row < ListView_GetItemCount(this->m_Hwnd); row++) {
		this->ScrollPbars(row);
	}
}

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

		MoveWindow(lpdcxlvi->pbar->getHwnd(),
			rItem.left, rItem.top, (rItem.right - rItem.left), (rItem.bottom - rItem.top),
			TRUE);

		//lpdcxlvi->pbar->redrawWindow();
		//this->redrawWindow();
		break;
	}

	delete lvi;
}