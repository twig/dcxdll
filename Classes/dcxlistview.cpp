/*!
 * \file dcxlistview.cpp
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

#include "dcxlistview.h"
#include "dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxListView::DcxListView( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    WS_EX_CLIENTEDGE, 
    DCX_LISTVIEWCLASS,
    NULL,
    WS_CHILD | WS_VISIBLE | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  SendMessage( this->m_Hwnd, CCM_SETVERSION, (WPARAM) 5, (LPARAM) 0 );

  ListView_SetExtendedListViewStyleEx( this->m_Hwnd, ExStyles, ExStyles );

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

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
    WS_EX_CLIENTEDGE, 
    DCX_LISTVIEWCLASS,
    NULL,
    WS_CHILD | WS_VISIBLE | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  SendMessage( this->m_Hwnd, CCM_SETVERSION, (WPARAM) 5, (LPARAM) 0 );

  ListView_SetExtendedListViewStyleEx( this->m_Hwnd, ExStyles, ExStyles );

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
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

  unsigned int i = 1, numtok = styles.numtok( " " );

  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "report" ) 
      *Styles |= LVS_REPORT;
    else if ( styles.gettok( i , " " ) == "icon" ) 
      *Styles |= LVS_ICON;
    else if ( styles.gettok( i , " " ) == "smallicon" ) 
      *Styles |= LVS_SMALLICON;
    else if ( styles.gettok( i , " " ) == "list" ) 
      *Styles |= LVS_LIST;
    else if ( styles.gettok( i , " " ) == "noheader" ) 
      *Styles |= LVS_NOCOLUMNHEADER;
    else if ( styles.gettok( i , " " ) == "grid" ) 
      *ExStyles |= LVS_EX_GRIDLINES;
    else if ( styles.gettok( i , " " ) == "borderselect" ) 
      *ExStyles |= LVS_EX_BORDERSELECT;
    else if ( styles.gettok( i , " " ) == "flatsb" ) 
      *ExStyles |= LVS_EX_FLATSB;
    else if ( styles.gettok( i , " " ) == "fullrow" ) 
      *ExStyles |= LVS_EX_FULLROWSELECT;
    else if ( styles.gettok( i , " " ) == "checkbox" ) 
      *ExStyles |= LVS_EX_CHECKBOXES;
    else if ( styles.gettok( i , " " ) == "headerdrag" ) 
      *ExStyles |= LVS_EX_HEADERDRAGDROP;
    else if ( styles.gettok( i , " " ) == "hottrack" ) 
      *ExStyles |= LVS_EX_TRACKSELECT;
    else if ( styles.gettok( i , " " ) == "oneclick" ) 
      *ExStyles |= LVS_EX_ONECLICKACTIVATE;
    else if ( styles.gettok( i , " " ) == "twoclick" ) 
      *ExStyles |= LVS_EX_TWOCLICKACTIVATE;
    else if ( styles.gettok( i , " " ) == "underlinehot" ) 
      *ExStyles |= LVS_EX_UNDERLINEHOT;
    else if ( styles.gettok( i , " " ) == "underlinecold" ) 
      *ExStyles |= LVS_EX_UNDERLINECOLD;
    else if ( styles.gettok( i , " " ) == "alignleft" ) 
      *Styles |= LVS_ALIGNLEFT;
    else if ( styles.gettok( i , " " ) == "aligntop" ) 
      *Styles |= LVS_ALIGNTOP;
    else if ( styles.gettok( i , " " ) == "autoarrange" ) 
      *Styles |= LVS_AUTOARRANGE;
    else if ( styles.gettok( i , " " ) == "nolabelwrap" ) 
      *Styles |= LVS_NOLABELWRAP;
    else if ( styles.gettok( i , " " ) == "showsel" ) 
      *Styles |= LVS_SHOWSELALWAYS;
    else if ( styles.gettok( i , " " ) == "singlesel" ) 
      *Styles |= LVS_SINGLESEL;
    else if ( styles.gettok( i , " " ) == "editlabel" ) 
      *Styles |= LVS_EDITLABELS;
    else if ( styles.gettok( i , " " ) == "sortasc" ) 
      *Styles |= LVS_SORTASCENDING;
    else if ( styles.gettok( i , " " ) == "sortdesc" ) 
      *Styles |= LVS_SORTDESCENDING;
    else if ( styles.gettok( i , " " ) == "noscroll" ) 
      *Styles |= LVS_NOSCROLL;
    else if ( styles.gettok( i , " " ) == "subitemimage" ) 
      *ExStyles |= LVS_EX_SUBITEMIMAGES;
    else if ( styles.gettok( i , " " ) == "tooltip" ) 
      *ExStyles |= LVS_EX_LABELTIP | LVS_EX_INFOTIP;

    i++;
  }
  this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxListView::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );

  // [NAME] [ID] [PROP] [N] [NSUB]
  if ( input.gettok( 3, " " ) == "text" && numtok > 4 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    int nSubItem = atoi( input.gettok( 5, " " ).to_chr( ) );

    if ( nItem > -1 && nItem < ListView_GetItemCount( this->m_Hwnd ) ) {
      ListView_GetItemText( this->m_Hwnd, nItem, nSubItem, szReturnValue, 900 );
      return;
    }
  }
  // [NAME] [ID] [PROP] [N] [NSUB]
  else if ( input.gettok( 3, " " ) == "icon" && numtok > 4 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    int nSubItem = atoi( input.gettok( 5, " " ).to_chr( ) );

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
  else if ( input.gettok( 3, " " ) == "seltext" && numtok > 3 ) {

    int nItem = ListView_GetNextItem( this->m_Hwnd, -1, LVIS_SELECTED );
    int nSubItem = atoi( input.gettok( 4, " " ).to_chr( ) );

    if ( nItem > -1 ) {

      ListView_GetItemText( this->m_Hwnd, nItem, nSubItem, szReturnValue, 900 );
      return;
    }
  }
  // [NAME] [ID] [PROP]
  else if ( input.gettok( 3, " " ) == "sel" ) {

    if ( this->isStyle( LVS_SINGLESEL ) ) {
      int nItem = ListView_GetNextItem( this->m_Hwnd, -1, LVIS_SELECTED );
      if ( nItem > -1 ) {
        wsprintf( szReturnValue, "%d", nItem + 1 );
        return;
      }
    }
    else {

      int nSelItems = ListView_GetSelectedCount( this->m_Hwnd );
      if ( nSelItems > 0 ) {

        TString list = "";
        char line[5];

        int nItem = -1;
        int i = 1;
        while ( ( nItem = ListView_GetNextItem( this->m_Hwnd, nItem, LVIS_SELECTED ) ) != -1 ) {

          itoa( nItem + 1, line, 10 );
          
          list += line;
          if ( i != nSelItems )
            list += ',';
        }

        lstrcpy( szReturnValue, list.to_chr( ) );
        return;
      }
    }
  }
  // [NAME] [ID] [PROP]
  else if ( input.gettok( 3, " " ) == "selnum" ) {

    wsprintf( szReturnValue, "%d", ListView_GetSelectedCount( this->m_Hwnd ) );
    return;
  }
  // [NAME] [ID] [PROP] [N]
  else if ( input.gettok( 3, " " ) == "state" && numtok > 3 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

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
  else if ( input.gettok( 3, " " ) == "num" ) {

    wsprintf( szReturnValue, "%d", ListView_GetItemCount( this->m_Hwnd ) );
    return;
  }
  // [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [COLUMN] [N]
  else if ( input.gettok( 3, " " ) == "find" && numtok > 6 ) {

    TString matchtext = input.gettok( 2, "\t" );
    matchtext.trim( );
    TString params = input.gettok( 3, "\t" );
    params.trim( );

    if ( matchtext.len( ) > 0 ) {

      UINT SearchType;

      if ( params.gettok( 1, " " ) == "R" )
        SearchType = LVSEARCH_R;
      else
        SearchType = LVSEARCH_W;

      int nColumn = (int) atoi( params.gettok( 2, " " ).to_chr( ) ) - 1;
      int N = (int) atoi( params.gettok( 3, " " ).to_chr( ) );

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
  else if ( input.gettok( 3, " " ) == "tbitem" ) {

    if ( this->isStyle( LVS_REPORT ) || this->isStyle( LVS_LIST ) )
      wsprintf( szReturnValue, "%d %d", this->getTopIndex( ) + 1, this->getBottomIndex( ) + 1 );
      
    return;
  }
  else if ( input.gettok( 3, " " ) == "mouseitem" ) {

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
  else if ( input.gettok( 3, " " ) == "hwidth" && numtok > 3 ) {

    int nColumn = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nColumn > -1 && nColumn < this->getColumnCount( ) ) {

      wsprintf( szReturnValue, "%d", ListView_GetColumnWidth( this->m_Hwnd, nColumn ) );
      return;
    }
  }
  // [NAME] [ID] [PROP] [N]
  else if ( input.gettok( 3, " " ) == "htext" && numtok > 3 ) {

    int nColumn = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

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
  else if ( input.gettok( 3, " " ) == "hicon" && numtok > 3 ) {

    int nColumn = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

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
  else if ( input.gettok( 3, " " ) == "gtext" && numtok > 3 ) {

    int GID = atoi( input.gettok( 4, " " ).to_chr( ) );

    WCHAR wstr[901];

    LVGROUP lvg;
    ZeroMemory( &lvg, sizeof( LVGROUP ) );
    lvg.cbSize = sizeof( LVGROUP );
    lvg.mask = LVGF_HEADER;
    lvg.cchHeader = 900;
    lvg.pszHeader = wstr;
    
    if ( isXP( ) && ListView_GetGroupInfo( this->m_Hwnd, GID, &lvg ) != -1 ) {

      mIRCError( "Got Information!" );
      int n = WideCharToMultiByte( CP_ACP, 0, wstr, lstrlenW( wstr ) + 1, szReturnValue, 900, NULL, NULL );
      char error[500];
      wsprintf( error, "Chars %d", n );
      mIRCError( error );
      mIRCError( szReturnValue );
      return;
    }
  }
  // [NAME] [ID] [PROP] [N]
  else if ( input.gettok( 3, " " ) == "genabled" ) {

    if ( isXP( ) && ListView_IsGroupViewEnabled( this->m_Hwnd ) )
      lstrcpy( szReturnValue, "$true" );
    else
      lstrcpy( szReturnValue, "$false" );

    return;
  }
  // [NAME] [ID] [PROP] [N]
  else if ( input.gettok( 3, " " ) == "gnum" ) {


  }
  else if ( this->parseGlobalInfoRequest( input, szReturnValue ) ) {

    return;
  }

  szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxListView::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  //xdid -a [NAME] [ID] [SWITCH] [N] [INDENT] [+FLAGS] [#ICON] [#STATE] [#OVERLAY] [#GROUPID] [COLOR] [BGCOLOR] Item Text {TAB}[+FLAGS] [#ICON] Item Text ...
  if ( flags.switch_flags[0] && numtok > 12 ) {

    LVITEM lvi;
    ZeroMemory( &lvi, sizeof( LVITEM ) );

    TString data = input.gettok( 1, "\t" ).gettok( 4, -1, " " );
    data.trim( );

    // LVS_REPORT view
    if ( this->isListViewStyle( LVS_REPORT ) ) {

      int nPos = atoi( data.gettok( 1, " " ).to_chr( ) ) - 1;

      if ( nPos == -1 )
        nPos += ListView_GetItemCount( this->m_Hwnd ) + 1;

      int indent = atoi( data.gettok( 2, " " ).to_chr( ) );
      UINT stateFlags = this->parseItemFlags( data.gettok( 3, " " ) );
      int icon = atoi( data.gettok( 4, " " ).to_chr( ) ) - 1;
      int state = atoi( data.gettok( 5, " " ).to_chr( ) );
      int overlay = atoi( data.gettok( 6, " " ).to_chr( ) );
      int group = atoi( data.gettok( 7, " " ).to_chr( ) );
      COLORREF clrText = atol( data.gettok( 8, " " ).to_chr( ) );
      COLORREF clrBack = atol( data.gettok( 9, " " ).to_chr( ) );

      TString itemtext = "";

      if ( data.numtok( " " ) > 9 )
        itemtext = data.gettok( 10, -1, " " );

      lvi.mask = LVIF_TEXT | LVIF_INDENT | LVIF_PARAM | LVIF_IMAGE;
      lvi.iItem = nPos;
      lvi.iImage = -1;
      lvi.state = stateFlags;
      lvi.iSubItem = 0;
      lvi.iIndent = indent;

      if ( isXP( ) && group > 0 ) {

        lvi.iGroupId = group;
        lvi.mask |= LVIF_GROUPID;
      }

      if ( icon > -1 )
        lvi.iImage = icon;

      lvi.pszText = itemtext.to_chr( );

      LPDCXLVITEM lpmylvi = new DCXLVITEM;

      if ( stateFlags & LVIS_UNDERLINE )
        lpmylvi->bUline = TRUE;
      else
        lpmylvi->bUline = FALSE;

      if ( stateFlags & LVIS_BOLD )
        lpmylvi->bBold = TRUE;
      else
        lpmylvi->bBold = FALSE;

      if ( stateFlags & LVIS_COLOR )
        lpmylvi->clrText = clrText;
      else
        lpmylvi->clrText = -1;

      if ( stateFlags & LVIS_BGCOLOR )
        lpmylvi->clrBack = clrBack;
      else
        lpmylvi->clrBack = -1;

      lvi.lParam = (LPARAM) lpmylvi;

      lvi.iItem = ListView_InsertItem( this->m_Hwnd, &lvi );

      int tabs;
      if ( ( tabs = input.numtok( "\t" ) ) > 1 ) {

        int i = 2;

        // ADD check for num columns
        while ( i <= tabs ) {

          data = input.gettok( i, "\t" );
          data.trim( );

          lvi.iSubItem = i - 1;
          lvi.mask = LVIF_TEXT | LVIF_IMAGE;
          lvi.iImage = -1;

          icon = atoi( data.gettok( 2, " " ).to_chr( ) ) - 1;

          if ( icon > -1 )
            lvi.iImage = icon;

          itemtext = "";
          if ( data.numtok( " " ) > 2 )
            itemtext = data.gettok( 3, -1, " " );

          lvi.pszText = itemtext.to_chr( );
          ListView_SetItem( this->m_Hwnd, &lvi );

          i++;
        }
      }

      if ( state > -1 )
        ListView_SetItemState( this->m_Hwnd, lvi.iItem, INDEXTOSTATEIMAGEMASK( state ), LVIS_STATEIMAGEMASK );
      // if ( overlay > -1 )
        //ListView_SetItemState(hwnd, lvi.iItem, INDEXTOSTATEIMAGEMASK(overlay), LVIS_OVERLAYMASK);
    }
    // LVS_ICON | LVS_SMALLICON | LVS_LIST views
    else {

      int nPos = atoi( data.gettok( 1, " " ).to_chr( ) ) - 1;

      if ( nPos == -1 )
        nPos += ListView_GetItemCount( this->m_Hwnd ) + 1;

      int indent = atoi( data.gettok( 2, " " ).to_chr( ) );
      UINT stateFlags = this->parseItemFlags( data.gettok( 3, " " ) );
      int icon = atoi( data.gettok( 4, " " ).to_chr( ) ) - 1;
      int state = atoi( data.gettok( 5, " " ).to_chr( ) );
      int overlay = atoi( data.gettok( 6, " " ).to_chr( ) );
      int group = atoi( data.gettok( 7, " " ).to_chr( ) );

      TString itemtext;

      if ( data.numtok( " " ) > 9 )
        itemtext = data.gettok( 10, -1, " " );

      lvi.iItem = nPos;
      lvi.iImage = -1;
      lvi.state = stateFlags;
      lvi.iSubItem = 0;
      lvi.mask = LVIF_TEXT | LVIF_STATE;

      if ( icon > -1 ) {
        lvi.iImage = icon;
        lvi.mask |= LVIF_IMAGE;
      }

      lvi.pszText = itemtext.to_chr( );

      lvi.iItem = ListView_InsertItem( this->m_Hwnd, &lvi );

      if ( state > -1 )
        ListView_SetItemState( this->m_Hwnd, lvi.iItem, INDEXTOSTATEIMAGEMASK( state ), LVIS_STATEIMAGEMASK );
      //ListView_SetItemState(hwnd, lvi.iItem, INDEXTOSTATEIMAGEMASK(overlay), LVIS_OVERLAYMASK);
    }
  }
  // xdid -c [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[2] && numtok > 3 ) {

    if ( this->isStyle( LVS_SINGLESEL ) ) {

      int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
      if ( nItem > -1 )
        ListView_SetItemState( this->m_Hwnd, nItem, LVIS_SELECTED, LVIS_SELECTED );
    }
    else {

      TString Ns = input.gettok( 4, " " );
      
      int i = 1, n = Ns.numtok( "," ), nItems = ListView_GetItemCount( this->m_Hwnd );

      while ( i <= n ) {

        int nItem = atoi( Ns.gettok( i, "," ).to_chr( ) ) - 1;

        if ( nItem > -1 && nItem < nItems )
          ListView_SetItemState( this->m_Hwnd, nItem, LVIS_SELECTED, LVIS_SELECTED );

        i++;
      }
    }
  }
  // xdid -d [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[3] && numtok > 3 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nItem > -1 )
      ListView_DeleteItem( this->m_Hwnd, nItem );

  }
  // xdid -g [NAME] [ID] [SWITCH] [+FLAGS] [X] [Y] (FILENAME)
  else if ( flags.switch_flags[6] && numtok > 5 ) {

    LVBKIMAGE lvbki;
    ZeroMemory( &lvbki, sizeof( LVBKIMAGE ) );

    lvbki.ulFlags = this->parseImageFlags( input.gettok( 4, " " ) );
    lvbki.xOffsetPercent = atoi( input.gettok( 5, " " ).to_chr( ) );
    lvbki.yOffsetPercent = atoi( input.gettok( 6, " " ).to_chr( ) );
    TString filename;

    if ( numtok > 6 ) {
      filename = input.gettok( 7, -1, " " );
      filename.trim( );
      lvbki.pszImage = filename.to_chr( );
      lvbki.ulFlags |= LVBKIF_SOURCE_URL;
    }

    ListView_SetBkImage( this->m_Hwnd, &lvbki );
  }
  // xdid -i [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
  else if ( flags.switch_flags[8] && numtok > 4 ) {

    UINT iColorFlags = this->parseColorFlags( input.gettok( 4, " " ) );
    COLORREF clrColor = atol( input.gettok( 5, " " ).to_chr( ) );

    if ( iColorFlags & LVCS_TEXT )
      ListView_SetTextColor( this->m_Hwnd, clrColor );
    if ( iColorFlags & LVCS_BKG ) {
      if ( input.gettok( 5, " " ) == "none" )
        ListView_SetBkColor( this->m_Hwnd, CLR_NONE );
      else
        ListView_SetBkColor( this->m_Hwnd, clrColor );
    }
    if ( iColorFlags & LVCS_BKGTEXT ) {
      if ( input.gettok( 5, " " ) == "none" )
        ListView_SetTextBkColor( this->m_Hwnd, CLR_NONE );
      else
        ListView_SetTextBkColor( this->m_Hwnd, clrColor );
    }
    if ( iColorFlags & LVCS_OUTLINE )
      ListView_SetOutlineColor( this->m_Hwnd, clrColor );

    this->redrawWindow( );
  }
  // xdid -k [NAME] [ID] [SWITCH] [STATE] [N]
  else if ( flags.switch_flags[10] && numtok > 4 ) {

    int state = atoi( input.gettok( 4, " " ).to_chr( ) );
    int nItem = atoi( input.gettok( 5, " " ).to_chr( ) ) - 1;

    ListView_SetItemState( this->m_Hwnd, nItem, INDEXTOSTATEIMAGEMASK( state ), LVIS_STATEIMAGEMASK );
  }
  // xdid -l [NAME] [ID] [SWITCH] [N] [M] [ICON]
  else if ( flags.switch_flags[11] && numtok > 5 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    int nSubItem = atoi( input.gettok( 5, " " ).to_chr( ) );
    int nIcon = atoi( input.gettok( 6, " " ).to_chr( ) ) - 1;

    if ( nItem > -1 && nSubItem > -1 && nSubItem <= this->getColumnCount( ) && nIcon > -2 ) {

      LVITEM lvi;

      lvi.mask = LVIF_IMAGE;
      lvi.iItem = nItem;
      lvi.iSubItem = nSubItem;
      lvi.iImage = nIcon;
      ListView_SetItem( this->m_Hwnd, &lvi );
    }
  }
  // xdid -m [NAME] [ID] [SWITCH] [0|1]
  else if ( flags.switch_flags[12] && numtok > 3 ) {

    if ( input.gettok( 4, " " ) == "1" )
      ListView_EnableGroupView( this->m_Hwnd, TRUE );
    else
      ListView_EnableGroupView( this->m_Hwnd, FALSE );
  }
  // xdid -n [NAME] [ID] [SWITCH] [N] [+FLAGS] [WIDTH]
  else if ( flags.switch_flags[13] && numtok > 5 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    UINT iFlags = this->parseHeaderFlags2( input.gettok( 5, " " ) );
    int width = atoi( input.gettok( 6, " " ).to_chr( ) );

    if ( nItem > -1 && nItem < this->getColumnCount( ) ) {

      if ( iFlags & LVSCW_AUTOSIZE )
        ListView_SetColumnWidth( this->m_Hwnd, nItem, LVSCW_AUTOSIZE );
      else if ( iFlags & LVSCW_AUTOSIZE_USEHEADER )
        ListView_SetColumnWidth( this->m_Hwnd, nItem, LVSCW_AUTOSIZE_USEHEADER );
      else
        ListView_SetColumnWidth( this->m_Hwnd, nItem, width );
    }
  }
  // xdid -q [NAME] [ID] [SWITCH] [N] [+FLAGS] [GID] [Group Text]
  else if ( flags.switch_flags[16] && numtok > 6 ) {

    int index = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    UINT iFlags = this->parseGroupFlags( input.gettok( 5, " " ) );
    int gid = atoi( input.gettok( 6, " " ).to_chr( ) );

    if ( isXP( ) && index > -1 && gid > 0 ) {

      TString text = input.gettok( 7, -1, " " );

      LVGROUP lvg;
      ZeroMemory( &lvg, sizeof( LVGROUP ) );
      lvg.cbSize = sizeof( LVGROUP );
      lvg.mask = LVGF_ALIGN | LVGF_HEADER | LVGF_GROUPID;

      LPWSTR wstr = new WCHAR[text.len( ) + 1];
      MultiByteToWideChar( CP_ACP, 0, text.to_chr( ), text.len( ) + 1, 
                           wstr, text.len( ) + 1 );

      lvg.iGroupId = gid;
      lvg.pszHeader = wstr;
      lvg.uAlign = iFlags;

      ListView_InsertGroup( this->m_Hwnd, index, &lvg );
    }
  }
  // xdid -r [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[17] ) {

    ListView_DeleteAllItems( this->m_Hwnd );
  }
  // xdid -t [NAME] [ID] [SWITCH] [+FLAGS] [#ICON] [WIDTH] (Header text) [{TAB} [+FLAGS] [#ICON] [WIDTH] Header text {TAB} ... ]
  else if ( flags.switch_flags[19] && numtok > 5 ) {

    int nCol;
    if ( ( nCol = this->getColumnCount( ) ) > 0 ) {
      while ( --nCol > 0 )
        ListView_DeleteColumn( this->m_Hwnd, nCol );
    }

    LVCOLUMN lvc;
    ZeroMemory( &lvc, sizeof( LVCOLUMN ) );
    int nColumn = 0;
    TString data = input.gettok( 1, "\t" ).gettok( 4, -1, " " );
    data.trim( );

    int icon = atoi( data.gettok( 2, " " ).to_chr( ) ) - 1;
    int width = atoi( data.gettok( 3, " " ).to_chr( ) );

    TString itemtext;
    if ( data.numtok( " " ) > 3 )
      itemtext = data.gettok( 4, -1, " " );

    lvc.iImage = -1;
    lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
    lvc.cx = width;
    lvc.fmt = this->parseHeaderFlags( data.gettok( 1, " " ) ) ;
    lvc.iSubItem = 0;
    lvc.pszText = itemtext.to_chr( );

    UINT iFlags2 = this->parseHeaderFlags2( data.gettok( 1, " " ) );

    if ( icon > -1 ) {

      lvc.mask |= LVCF_IMAGE;
      lvc.iImage = icon;
    }

    if ( this->getColumnCount( ) > 0 )
      ListView_SetColumn( this->m_Hwnd, nColumn, &lvc );
    else
      ListView_InsertColumn( this->m_Hwnd, nColumn, &lvc );

    if ( iFlags2 & LVSCW_AUTOSIZE )
      ListView_SetColumnWidth( this->m_Hwnd, nColumn, LVSCW_AUTOSIZE );
    else if ( iFlags2 & LVSCW_AUTOSIZE_USEHEADER )
      ListView_SetColumnWidth( this->m_Hwnd, nColumn, LVSCW_AUTOSIZE_USEHEADER );
    
    int tabs;
    if ( ( tabs = input.numtok( "\t" ) ) > 1 ) {

      int i = 2;
      while ( i <= tabs ) {

        nColumn++;
        data = input.gettok( i, "\t" );
        data.trim( );

        icon = atoi( data.gettok( 2, " " ).to_chr( ) ) - 1;
        width = atoi( data.gettok( 3, " " ).to_chr( ) );

        itemtext = "";
        if ( data.numtok( " " ) > 3 )
          itemtext = data.gettok( 4, -1, " " );

        lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
        lvc.cx = width;
        lvc.fmt = this->parseHeaderFlags( data.gettok( 1, " " ) );
        lvc.iImage = -1;
        lvc.iSubItem = 0;
        lvc.pszText = itemtext.to_chr( );

        iFlags2 = this->parseHeaderFlags2( data.gettok( 1, " " ) );

        if ( icon > -1 ) {

          lvc.mask |= LVCF_IMAGE;
          lvc.iImage = icon;
        }

        ListView_InsertColumn( this->m_Hwnd, nColumn, &lvc );

        if ( iFlags2 & LVSCW_AUTOSIZE )
          ListView_SetColumnWidth( this->m_Hwnd, nColumn, LVSCW_AUTOSIZE );
        else if ( iFlags2 & LVSCW_AUTOSIZE_USEHEADER )
          ListView_SetColumnWidth( this->m_Hwnd, nColumn, LVSCW_AUTOSIZE_USEHEADER );

        i++;
      }
    }
  }
  // xdid -u [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[20] ) {

    ListView_SetItemState( this->m_Hwnd, -1, 0, LVIS_SELECTED );
  }
  // xdid -v [NAME] [ID] [SWITCH] [N] [M] (ItemText)
  else if ( flags.switch_flags[21] && numtok > 4 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    int nSubItem = atoi( input.gettok( 5, " " ).to_chr( ) );
    TString itemtext = input.gettok( 6, -1 , " " );
    itemtext.trim( );
    
    if ( nItem > -1 && nSubItem > -1 && nSubItem <= this->getColumnCount( ) ) {

      ListView_SetItemText( this->m_Hwnd, nItem, nSubItem, itemtext.to_chr( ) );
    }
  }
  // xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
  else if ( flags.switch_flags[22] && numtok > 5 ) {

    UINT iFlags = this->parseIconFlagOptions( input.gettok( 4, " " ) );

    HIMAGELIST himl;
    HICON icon;
    int index;
    TString filename;

    if ( iFlags & LVSIL_SMALL ) {

      if ( ( himl = this->getImageList( LVSIL_NORMAL ) ) == NULL ) {

        himl = this->createImageList( TRUE );

        if ( himl )
          this->setImageList( himl, LVSIL_NORMAL );
      }

      index = atoi( input.gettok( 5, " ").to_chr( ) );
      filename = input.gettok( 6, -1, " " );
      ExtractIconEx( filename.to_chr( ), index, &icon, 0, 1 );
      ImageList_AddIcon( himl, icon );
      DestroyIcon( icon );

      if ( ( himl = this->getImageList( LVSIL_SMALL ) ) == NULL ) {

        himl = this->createImageList( FALSE );

        if ( himl )
          this->setImageList( himl, LVSIL_SMALL );
      }

      index = atoi( input.gettok( 5, " ").to_chr( ) );
      filename = input.gettok( 6, -1, " " );
      ExtractIconEx( filename.to_chr( ), index, 0, &icon, 1 );
      ImageList_AddIcon( himl, icon );
      DestroyIcon( icon );
    }

    if ( iFlags & LVSIL_STATE ) {

      if ( ( himl = this->getImageList( LVSIL_STATE ) ) == NULL ) {

        himl = this->createImageList( FALSE );

        if ( himl )
          this->setImageList( himl, LVSIL_STATE );
      }

      index = atoi( input.gettok( 5, " ").to_chr( ) );
      filename = input.gettok( 6, -1, " " );
      ExtractIconEx( filename.to_chr( ), index, 0, &icon, 1 );
      ImageList_AddIcon( himl, icon );
      DestroyIcon( icon );
    }
  }
  // xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
  else if ( flags.switch_flags[24] && numtok > 3 ) {

    UINT iFlags = this->parseIconFlagOptions( input.gettok( 4, " " ) );

    HIMAGELIST himl;

    if ( iFlags & LVSIL_SMALL ) {

      if ( ( himl = this->getImageList( LVSIL_SMALL ) ) != NULL ) {

        ImageList_Destroy( himl );
        this->setImageList( NULL, LVSIL_SMALL );
      }

      if ( ( himl = this->getImageList( LVSIL_NORMAL ) ) != NULL ) {

        ImageList_Destroy( himl );
        this->setImageList( NULL, LVSIL_NORMAL );
      }
    }

    if ( iFlags & LVSIL_STATE ) {

      if ( ( himl = this->getImageList( LVSIL_STATE ) ) != NULL ) {

        ImageList_Destroy( himl );
      }
    }
  }
   // xdid -z [NAME] [ID] [SWITCH] [+FLAGS] [N] (ALIAS)
  else if ( flags.switch_flags[25] && numtok > 4 ) {

    DCXLVSORT lvsort;
    lvsort.m_Hwnd = this->m_Hwnd;
    lvsort.iSortFlags = this->parseSortFlags( input.gettok( 4, " " ) );
    int nColumn = atoi( input.gettok( 5, " " ).to_chr( ) ) - 1;
    lvsort.nColumn = nColumn;

    if ( nColumn < 0 || nColumn >= this->getColumnCount( ) )
      return;

      if ( lvsort.iSortFlags & LVSS_CUSTOM && numtok < 6 )
        return;
      else
        lvsort.tsCustomAlias = input.gettok( 6, " " );

    ListView_SortItemsEx( this->m_Hwnd, DcxListView::sortItemsEx, &lvsort );
  }
  else {
    this->parseGlobalCommandRequest( input, flags );
  }
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxListView::getImageList( int iImageList ) {

  return ListView_GetImageList( this->m_Hwnd, iImageList );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxListView::setImageList( HIMAGELIST himl, int iImageList ) {

  ListView_SetImageList( this->m_Hwnd, himl, iImageList );
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxListView::createImageList( BOOL bIcons ) {

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

UINT DcxListView::parseItemFlags( TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if ( flags[i] == 'b' )
      iFlags |= LVIS_BOLD;
    else if ( flags[i] == 'c' )
      iFlags |= LVIS_COLOR;
    else if ( flags[i] == 'd' )
      iFlags |= LVIS_DROPHILITED;
    else if ( flags[i] == 'f' )
      iFlags |= LVIS_FOCUSED;
    else if ( flags[i] == 'k' )
      iFlags |= LVIS_BGCOLOR;
    else if ( flags[i] == 's' )
      iFlags |= LVIS_SELECTED;
    else if ( flags[i] == 't' )
      iFlags |= LVIS_CUT;
    else if ( flags[i] == 'u' )
      iFlags |= LVIS_UNDERLINE;

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

    if ( flags[i] == 'a' )
      iFlags |= LVSCW_AUTOSIZE;
    else if ( flags[i] == 'h' )
      iFlags |= LVSCW_AUTOSIZE_USEHEADER;

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

    ++i;
  }
  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxListView::isListViewStyle( long dwView ) {

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

BOOL DcxListView::matchItemText( int nItem, int nSubItem, TString * search, UINT SearchType ) {

  char res[10];
  char itemtext[900];
  char com[1000];

  ListView_GetItemText( this->m_Hwnd, nItem, nSubItem, itemtext, 900 );
  // Regex Search
  if ( SearchType == LVSEARCH_R )
    wsprintf( com, "$regex(%s,%s)", itemtext, search->to_chr( ) );
  // Wildcard Search
  else
    wsprintf( com, "$iif(%s iswm %s,1,0)", search->to_chr( ), itemtext );

  mIRCeval( com, res );

  if ( !lstrcmp( res, "1" ) )
      return TRUE;

  return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxListView::getColumnCount( ) {

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

int DcxListView::getTopIndex( ) {

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

int DcxListView::getBottomIndex( ) {

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
  char com[900];
  char res[20];
  char itemtext1[900];
  char itemtext2[900];

  ListView_GetItemText( plvsort->m_Hwnd, lParam1, plvsort->nColumn, itemtext1, 900 );
  ListView_GetItemText( plvsort->m_Hwnd, lParam2, plvsort->nColumn, itemtext2, 900 );

  // CUSTOM Sort
  if ( plvsort->iSortFlags & LVSS_CUSTOM ) {

    wsprintf( com, "$%s(%s,%s)", plvsort->tsCustomAlias.to_chr( ), itemtext1, itemtext2 );
    mIRCeval( com, res );

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

LRESULT DcxListView::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
        char ret[256];
        this->callAliasEx( ret, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_NOTIFY : 
      {

        LPNMHDR hdr = (LPNMHDR) lParam;

        if (!hdr)
          break;

        switch( hdr->code ) {

          case NM_CLICK:
            {
              //mIRCError( "Control WM_NOTIFY - NM_CLICK" );
              bParsed = TRUE;

              LVHITTESTINFO lvh;
              GetCursorPos( &lvh.pt );
              ScreenToClient( this->m_Hwnd, &lvh.pt );
              ListView_HitTest( this->m_Hwnd, &lvh );
              char ret[256];

              if ( ( lvh.flags & LVHT_ONITEMSTATEICON ) &&
                ( ListView_GetExtendedListViewStyle( this->m_Hwnd ) & LVS_EX_CHECKBOXES ) &&
                !( lvh.flags & LVHT_ONITEMICON ) &&
                !( lvh.flags & LVHT_ONITEMLABEL ) ) 
              {
                this->callAliasEx( ret, "%s,%d,%d", "stateclick", this->getUserID( ), lvh.iItem + 1 );
              }
              else if ( lvh.flags & LVHT_ONITEM )
                this->callAliasEx( ret, "%s,%d,%d", "sclick", this->getUserID( ), lvh.iItem + 1 );
            }
            break;

          case NM_DBLCLK:
            {
              //mIRCError( "Control WM_NOTIFY - NM_DBLCLK" );
              bParsed = TRUE;

              LVHITTESTINFO lvh;
              GetCursorPos( &lvh.pt );
              ScreenToClient( this->m_Hwnd, &lvh.pt );
              ListView_HitTest( this->m_Hwnd, &lvh );
              char ret[256];

              if ( lvh.flags & LVHT_ONITEM )
                this->callAliasEx( ret, "%s,%d,%d", "dclick", this->getUserID( ), lvh.iItem + 1 );
            }
            break;

          case NM_RCLICK:
            {
              char ClassName[257];

              GetClassName( hdr->hwndFrom, ClassName, 256 );

              if ( lstrcmpi( ClassName, "SysHeader32" ) == 0 ) {

                HDHITTESTINFO hdti;
                GetCursorPos( &hdti.pt );
                ScreenToClient( hdr->hwndFrom, &hdti.pt );
                if ( SendMessage( hdr->hwndFrom, HDM_HITTEST, (WPARAM) 0, (LPARAM) &hdti ) != -1 ) {

                  char ret[256];
                  this->callAliasEx( ret, "%s,%d,%d", "hrclick", this->getUserID( ), hdti.iItem + 1 );
                }
              }
              else {
                //mIRCError( "Control WM_NOTIFY - NM_RCLICK" );

                LVHITTESTINFO lvh;
                GetCursorPos( &lvh.pt );
                ScreenToClient( this->m_Hwnd, &lvh.pt );
                ListView_HitTest( this->m_Hwnd, &lvh );
                char ret[256];

                if ( lvh.flags & LVHT_ONITEM )
                  this->callAliasEx( ret, "%s,%d,%d", "rclick", this->getUserID( ), lvh.iItem + 1 );
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

              if ( !lstrcmp( "noedit", ret ) ) {
                return TRUE;
              }
            }
            break;

          case LVN_ENDLABELEDIT:
            {

              bParsed = TRUE;

              char ret[256];
              LPNMLVDISPINFO lplvdi = (LPNMLVDISPINFO) lParam;

              if ( lplvdi->item.pszText == NULL ) {

                this->callAliasEx( ret, "%s,%d", "labelcancel", this->getUserID( ) );
              }
              else {

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

              //mIRCSignal( "NM_CUSTOMDRAW" );
              LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW) lParam;
              bParsed = TRUE;

              switch( lplvcd->nmcd.dwDrawStage ) {

                case CDDS_PREPAINT:
                  return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYSUBITEMDRAW;

                case CDDS_ITEMPREPAINT:
                  //mIRCError( "CDRF_NOTIFYSUBITEMDRAW" );
                  return CDRF_NOTIFYSUBITEMDRAW;

                case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
                  {
                    //mIRCError( "CDDS_ITEMPREPAINT CDDS_SUBITEM" );

                    /*
                    if ( lplvcd->iSubItem == 1  ) {

                      char data[500];
                      wsprintf( data, "GIVEN RECT %d %d %d %d - %d %d", lplvcd->nmcd.rc.left, 
                        lplvcd->nmcd.rc.top, lplvcd->nmcd.rc.right, lplvcd->nmcd.rc.bottom, 
                        lplvcd->nmcd.rc.right - lplvcd->nmcd.rc.left,
                        lplvcd->nmcd.rc.bottom - lplvcd->nmcd.rc.top);

                      mIRCError( data );

                      RECT rc;
                      ListView_GetItemRect( this->m_Hwnd, lplvcd->nmcd.dwItemSpec, &rc, LVIR_BOUNDS );
                      lplvcd->nmcd.rc.top = rc.top;
                      lplvcd->nmcd.rc.bottom = rc.bottom;

                      wsprintf( data, "ITEM RECT %d %d %d %d - %d %d", rc.left, 
                        rc.top, rc.right, rc.bottom, rc.right - rc.left, rc.bottom - rc.top );

                      mIRCError( data );

                      SetBkColor( lplvcd->nmcd.hdc, RGB(255,255,255) );

                      ExtTextOut( lplvcd->nmcd.hdc, lplvcd->nmcd.rc.left, lplvcd->nmcd.rc.top, 
                        ETO_CLIPPED | ETO_OPAQUE, &lplvcd->nmcd.rc, "", NULL, NULL);

                      SetBkColor( lplvcd->nmcd.hdc, RGB(128,0,255) );

                      //lplvcd->nmcd.rc.right -= 20;

                      InflateRect( &lplvcd->nmcd.rc, 0, -1 );

                      ExtTextOut( lplvcd->nmcd.hdc, lplvcd->nmcd.rc.left, lplvcd->nmcd.rc.top, 
                        ETO_CLIPPED | ETO_OPAQUE, &lplvcd->nmcd.rc, "", NULL, NULL);

                      mIRCError( "SUBITEM 1" );
                      return CDRF_SKIPDEFAULT;
                    }
                    */

                    LPDCXLVITEM lpdcxlvi = (LPDCXLVITEM) lplvcd->nmcd.lItemlParam;

                    if ( lpdcxlvi == NULL )
                      return CDRF_DODEFAULT;

                    if ( lpdcxlvi->clrText != -1 )
                      lplvcd->clrText = lpdcxlvi->clrText;

                    if ( lpdcxlvi->clrBack != -1 )
                      lplvcd->clrTextBk = lpdcxlvi->clrBack;

                    if ( lpdcxlvi->bUline || lpdcxlvi->bBold ) {

                      HFONT hFont = (HFONT) SendMessage( this->m_Hwnd, WM_GETFONT, 0, 0 );

                      LOGFONT lf;
                      GetObject( hFont, sizeof(LOGFONT), &lf );

                      if ( lpdcxlvi->bBold )
                        lf.lfWeight |= FW_BOLD;
                      if ( lpdcxlvi->bUline )
                        lf.lfUnderline = true;

                      HFONT hFontNew = CreateFontIndirect( &lf );
                      HFONT hOldFont = (HFONT) SelectObject( lplvcd->nmcd.hdc, hFontNew );

                      DeleteObject( hFontNew );
                    }
                  }
                  return ( CDRF_NEWFONT );

                case CDDS_ITEMPOSTPAINT:
                  return CDRF_DODEFAULT;

                default:
                  return CDRF_DODEFAULT;
              }
            }
            break;

          case LVN_DELETEITEM: 
            {

              //mIRCError( "Control WM_NOTIFY - LVN_DELETEITEM" );

              LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW) lParam;
              LPDCXLVITEM lpdcxlvi = (LPDCXLVITEM) lpnmlv->lParam;

              if ( lpdcxlvi != NULL )
                delete lpdcxlvi;
            }
            break;

          case HDN_BEGINTRACKW:
          case HDN_BEGINTRACK:
            {

              //mIRCError( "Control WM_NOTIFY - HDN_BEGINTRACK" );
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
              //mIRCError( "Control WM_NOTIFY - HDN_ITEMCLICK" );
              bParsed = TRUE;

              LPNMHEADER lphdr = (LPNMHEADER) lParam; 

              char ret[256];
              this->callAliasEx( ret, "%s,%d,%d", "hsclick", this->getUserID( ), lphdr->iItem + 1 );
            }
            break;

          case HDN_ITEMDBLCLICKW:
          case HDN_ITEMDBLCLICK:
            {
              //mIRCError( "Control WM_NOTIFY - HDN_ITEMDBLCLICK" );
              bParsed = TRUE;

              LPNMHEADER lphdr = (LPNMHEADER) lParam; 

              char ret[256];
              this->callAliasEx( ret, "%s,%d,%d", "hdclick", this->getUserID( ), lphdr->iItem + 1 );
            }
            break;

          case LVN_BEGINDRAG:
            {

              char ret[256];
              this->callAliasEx( ret, "%s,%d", "begindrag", this->getUserID( ) );
            }
            break;

        } // switch
      }
      break;

    case WM_VSCROLL:
      {

        if ( LOWORD( wParam ) == SB_ENDSCROLL ) {

          char ret[256];
          this->callAliasEx( ret, "%s,%d", "scrollend", this->getUserID( ) );
        }
      }
      break;

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

    case WM_DESTROY:
      {
        //mIRCError( "WM_DESTROY" );
        delete this;
        bParsed = TRUE;
      }
      break;

    case WM_SETCURSOR:
      {
        if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd && this->m_hCursor != NULL ) {

          SetCursor( this->m_hCursor );
          bParsed = TRUE;
          return TRUE;
        }
      }
      break;

    default:
      break;
  }

  return 0L;
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