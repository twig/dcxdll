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
    WS_CHILD | WS_VISIBLE | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

  SendMessage( this->m_Hwnd, CCM_SETVERSION, (WPARAM) 5, (LPARAM) 0 );

  if ( ExStyles & TVS_CHECKBOXES )
    this->addStyle( TVS_CHECKBOXES );

	this->m_ToolTipHWND = (HWND)TreeView_GetToolTips(this->m_Hwnd);
	if (styles.istok("balloon") && this->m_ToolTipHWND != NULL) {
		SetWindowLong(this->m_ToolTipHWND,GWL_STYLE,GetWindowLong(this->m_ToolTipHWND,GWL_STYLE) | TTS_BALLOON);
	}

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
		else if ( styles.gettok( i ) == "alpha" )
			this->m_bAlphaBlend = true;

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

void DcxTreeView::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( );
	TString prop(input.gettok( 3 ));

  // [NAME] [ID] [PROP] [PATH]
  if ( prop == "text" && numtok > 3 ) {

    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hAfter = TVI_ROOT;

    if ( this->parsePath( &input.gettok( 4, -1 ), &hParent, &hAfter ) ) {

      if ( this->correctTargetItem( &hParent, &hAfter ) ) {

        this->getItemText( &hAfter, szReturnValue, 900 );
        return;
      }
    }
  }
  // [NAME] [ID] [PROP] [PATH]
	else if ( prop == "icon" && numtok > 3 ) {
		HTREEITEM hParent = TVI_ROOT;
		HTREEITEM hAfter = TVI_ROOT;

		if ( this->parsePath( &input.gettok( 4, -1 ), &hParent, &hAfter ) ) {
			if ( this->correctTargetItem( &hParent, &hAfter ) ) {
				TVITEMEX tvi; 
				tvi.hItem = hAfter;
				tvi.mask = TVIF_IMAGE | TVIF_HANDLE;

				TreeView_GetItem( this->m_Hwnd, &tvi );
				wsprintf( szReturnValue, "%d", (tvi.iImage > 10000 ? -2 : tvi.iImage ) + 1 );
				return;
			}
		}
	}
  // [NAME] [ID] [PROP] [PATH]
  else if ( prop == "tooltip" && numtok > 3 ) {

    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hAfter = TVI_ROOT;

    if ( this->parsePath( &input.gettok( 4, -1 ), &hParent, &hAfter ) ) {

      if ( this->correctTargetItem( &hParent, &hAfter ) ) {

        TVITEMEX tvi;
        tvi.hItem = hAfter;
        tvi.mask = TVIF_HANDLE | TVIF_PARAM;

        TreeView_GetItem( this->m_Hwnd, &tvi );
        LPDCXTVITEM lpdcxtvi = (LPDCXTVITEM) tvi.lParam;

        if ( lpdcxtvi != NULL )
          lstrcpyn( szReturnValue, lpdcxtvi->tsTipText.to_chr( ), 900 );

        return;
      }
    }
  }
  // [NAME] [ID] [PROP]
  else if ( prop == "seltext" ) {

    HTREEITEM hItem = TreeView_GetSelection( this->m_Hwnd );
    this->getItemText( &hItem, szReturnValue, 900 );
    return;
  }
  // [NAME] [ID] [PROP]
  else if ( prop == "selpath" ) {

    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hItem = TreeView_GetSelection( this->m_Hwnd );
    VectorOfInts v;

    if ( this->getPath( &v, &hParent, &hItem ) ) {

      std::string path = this->getPathFromVector( &v );
      lstrcpyn( szReturnValue, path.c_str( ), 900 );
      return;
    }
  }
  // [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N] [SUBPATH]
  else if ( prop == "find" && numtok > 5 ) {

    TString matchtext(input.gettok( 2, TSTAB ));
    matchtext.trim( );
    TString params(input.gettok( 3, TSTAB ));
    params.trim( );

    if ( matchtext.len( ) > 0 ) {

      UINT SearchType;

      if ( params.gettok( 1 ) == "R" )
        SearchType = TVSEARCH_R;
      else
        SearchType = TVSEARCH_W;

      int N = params.gettok( 2 ).to_int( );
      int NC = 0;

      HTREEITEM hParent = TVI_ROOT;

      if ( params.numtok( ) > 2 ) {

        HTREEITEM hStart = TVI_ROOT;
        if ( this->parsePath( &params.gettok( 3, -1 ), &hStart, &hParent ) ) {

          if ( !this->correctTargetItem( &hStart, &hParent ) )
            hParent = TVI_ROOT;
        }
        else
          hParent = TVI_ROOT;
      }

      HTREEITEM hItem;
      if ( this->findItemText( &hParent, &hItem, &matchtext, N, &NC, SearchType ) ) {

        VectorOfInts v;
        hParent = TVI_ROOT;

        if ( this->getPath( &v, &hParent, &hItem ) ) {

          std::string path = this->getPathFromVector( &v );
          lstrcpyn( szReturnValue, path.c_str( ), 900 );
        }
      }
      else if ( N == 0 ) {

        wsprintf( szReturnValue, "%d", NC );
      }
    }
    return;
  }
  // [NAME] [ID] [PROP] [PATH]
  else if ( prop == "state" && numtok > 3 ) {

    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hAfter = TVI_ROOT;

    if ( this->parsePath( &input.gettok( 4, -1 ), &hParent, &hAfter ) ) {

      if ( this->correctTargetItem( &hParent, &hAfter ) ) {

        if ( this->isStyle( TVS_CHECKBOXES ) ) {

          int state = TreeView_GetCheckState( this->m_Hwnd, hAfter );

          if ( state == 1 )
            lstrcpy( szReturnValue, "2" );
          else if ( state == 0 )
            lstrcpy( szReturnValue, "1" );
          else
            lstrcpy( szReturnValue, "0" );
          return;
        }
        else {

          wsprintf( szReturnValue, "%d", TreeView_GetItemState( this->m_Hwnd, hAfter, TVIS_STATEIMAGEMASK ) );
          return;
        }
      }
    }
  }
  // [NAME] [ID] [PROP] [PATH]
  else if ( prop == "num" && numtok > 3 ) {

    TString path(input.gettok( 4, -1 ));

    if ( path == "root" ) {

      HTREEITEM hParent = TVI_ROOT;
      wsprintf( szReturnValue, "%d", this->getItemCount( &hParent ) );
      return;
    }
    else {

      HTREEITEM hParent = TVI_ROOT;
      HTREEITEM hAfter = TVI_ROOT;

      if ( this->parsePath( &path, &hParent, &hAfter ) ) {

        if ( this->correctTargetItem( &hParent, &hAfter ) ) {

          wsprintf( szReturnValue, "%d", this->getItemCount( &hAfter ) );
          return;
        }
      }
    }
  }
  // [NAME] [ID] [PROP] [PATH]
  else if ( prop == "expand" && numtok > 3 ) {

    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hAfter = TVI_ROOT;

    if ( this->parsePath( &input.gettok( 4, -1 ), &hParent, &hAfter ) ) {

      if ( this->correctTargetItem( &hParent, &hAfter ) ) {

        if ( TreeView_GetItemState( this->m_Hwnd, hAfter, TVIS_EXPANDED ) & TVIS_EXPANDED )
          lstrcpy( szReturnValue, "1" );
        else
          lstrcpy( szReturnValue, "0" );
        return;       
      }
    }
  }
  else if ( prop == "mouseitem" ) {

		TVHITTESTINFO tvh;
		GetCursorPos( &tvh.pt );
		ScreenToClient( this->m_Hwnd, &tvh.pt );
		TreeView_HitTest( this->m_Hwnd, &tvh );

		if ( tvh.flags & TVHT_ONITEM ) {

			VectorOfInts numPath;
			HTREEITEM hStart = TVI_ROOT;
			this->getPath( &numPath, &hStart, &tvh.hItem );
			std::string path = this->getPathFromVector( &numPath );

			lstrcpyn( szReturnValue, path.c_str( ), 900 );
		}
		else
			lstrcpy( szReturnValue, "0" );

		return;
	}
	else if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
		return;

	szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( input.gettok( 3 ), &flags );

  int numtok = input.numtok( );

  // xdid -r [NAME] [ID] [SWITCH]
  if (flags.switch_flags[17]) {
    TreeView_DeleteAllItems(this->m_Hwnd);
  }

	// xdid -a [NAME] [ID] [SWITCH] N N N ... N[TAB][+FLAGS] [#ICON] [#SICON] [#OVERLAY] [#STATE] [#INTEGRAL] [COLOR] [BKGCOLOR] Text[TAB]Tooltip Text
	if (flags.switch_flags[0]) {
		int n = input.numtok(TSTAB);

		if (n > 1) {
			TString path(input.gettok(1, TSTAB).gettok(4, -1));
			path.trim();
			TString data(input.gettok(2, TSTAB));
			data.trim();
			TString tooltip;

			if (n > 2) {
				tooltip = input.gettok(3, -1, TSTAB);
				tooltip.trim();
			}

			if (data.numtok( ) > 8)
				this->insertItem(&path, &data, &tooltip);
		}
	}
	// xdid -B [NAME] [ID] [SWITCH] N N N
	else if (flags.switch_cap_flags[1] && numtok > 3) {
		HTREEITEM hParent = TVI_ROOT;
		HTREEITEM hAfter = TVI_ROOT;

		if (this->parsePath(&input.gettok(4, -1), &hParent, &hAfter)) {
			if (this->correctTargetItem(&hParent, &hAfter))
				TreeView_EnsureVisible(this->m_Hwnd, hAfter); // make sure selected item is visible.
				TreeView_EditLabel(this->m_Hwnd, hAfter);
		}
	}
  // xdid -c [NAME] [ID] [SWITCH] N N N
  else if ( flags.switch_flags[2] && numtok > 3 ) {

    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hAfter = TVI_ROOT;

    if ( this->parsePath( &input.gettok( 4, -1 ), &hParent, &hAfter ) ) {

			if ( this->correctTargetItem( &hParent, &hAfter ) ) {
				TreeView_EnsureVisible(this->m_Hwnd, hAfter); // make sure selected item is visible.
        TreeView_SelectItem( this->m_Hwnd, hAfter );
			}
    }
  }
  // xdid -d [NAME] [ID] [SWITCH] N N N
  else if ( flags.switch_flags[3] && numtok > 3 ) {

    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hAfter = TVI_ROOT;

    if ( this->parsePath( &input.gettok( 4, -1 ), &hParent, &hAfter ) ) {

      if ( this->correctTargetItem( &hParent, &hAfter ) )
        TreeView_DeleteItem( this->m_Hwnd, hAfter );
    }
  }
  // xdid -g [NAME] [ID] [SWITCH] [HEIGHT]
  else if ( flags.switch_flags[6] && numtok > 3 ) {

    int iHeight = input.gettok( 4 ).to_int( );

    if ( iHeight > -2 ) {

      TreeView_SetItemHeight( this->m_Hwnd, iHeight );
    }
  }
	// xdid -i [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if ( flags.switch_flags[8] && numtok > 4 ) {
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
  // xdid -j [NAME] [ID] [SWITCH] [+FLAGS] [N N N] [TAB] [ICON] [SICON]
  else if ( flags.switch_flags[9] && numtok > 5 ) {

    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hAfter = TVI_ROOT;

    TString path(input.gettok( 1, TSTAB ).gettok( 4, -1 ));
    path.trim( );

    if ( input.numtok( TSTAB ) > 1 ) {

      TString icons(input.gettok( 2, TSTAB ));
      icons.trim( );

      if ( this->parsePath( &path, &hParent, &hAfter ) ) {

        if ( this->correctTargetItem( &hParent, &hAfter ) ) {

          int nIcon = icons.gettok( 1 ).to_int( ) - 1;
          int sIcon = icons.gettok( 2 ).to_int( ) - 1;

			 // quickfix so it doesnt display an image
			 if (nIcon == -1)
					nIcon = 10000;
			 if (sIcon == -1)
				 sIcon = 10000;

          if ( nIcon > -1 && sIcon > -1 ) {
            TVITEMEX tvi; 

            tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_HANDLE;
            tvi.hItem = hAfter;
            tvi.iImage = nIcon;
            tvi.iSelectedImage = sIcon;
            TreeView_SetItem( this->m_Hwnd, &tvi );
          }
        }
      }
    }
  }
  // xdid -k [NAME] [ID] [SWITCH] [STATE] N N N
  else if ( flags.switch_flags[10] && numtok > 4 ) {

    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hAfter = TVI_ROOT;

    if ( this->parsePath( &input.gettok( 5, -1 ), &hParent, &hAfter ) ) {

      UINT state = input.gettok( 4 ).to_int( );

      if ( !this->correctTargetItem( &hParent, &hAfter ) )
        return;

      TreeView_SetItemState( this->m_Hwnd, hAfter, INDEXTOSTATEIMAGEMASK( state ), TVIS_STATEIMAGEMASK );
    }
  }
  // xdid -l [NAME] [ID] [SWITCH] [SIZE]
  else if ( flags.switch_flags[11] && numtok > 3 ) {

    int size = input.gettok( 4 ).to_int( );

    if ( size != 32 && size != 24 )
      size = 16;

    this->m_iIconSize = size;
  }
  // xdid -m [NAME] [ID] [SWITCH] N N N{TAB}N N N
  else if ( flags.switch_flags[12] && numtok > 3 && input.numtok( TSTAB ) > 1 ) {

    TString pathFrom(input.gettok( 1, TSTAB ).gettok( 4, -1 ));
    pathFrom.trim( );
    TString pathTo(input.gettok( 2, TSTAB ));
    pathTo.trim( );

    HTREEITEM hParentFrom = TVI_ROOT;
    HTREEITEM hAfterFrom = TVI_ROOT;
    HTREEITEM hParentTo = TVI_ROOT;
    HTREEITEM hAfterTo = TVI_ROOT;

    HTREEITEM hNewItem;

		if ( !this->parsePath( &pathFrom, &hParentFrom, &hAfterFrom ) ) {
			this->showErrorEx(NULL,"-m", "Unable to parse path: %s", pathFrom.to_chr());
      return;
		}
		if ( !this->correctTargetItem( &hParentFrom, &hAfterFrom ) ) {
			this->showError(NULL,"-m", "Correction Failed.");
      return;
		}
		if ( !this->parsePath( &pathTo, &hParentTo, &hAfterTo ) ) {
			this->showErrorEx(NULL,"-m", "Unable to parse path: %s", pathTo.to_chr());
      return;
		}

    hNewItem = this->cloneItem( &hAfterFrom, &hParentTo, &hAfterTo );

    if ( hNewItem != NULL )
      this->copyAllItems( &hAfterFrom, &hNewItem );

    TreeView_DeleteItem( this->m_Hwnd, hAfterFrom );
  }
  // xdid -n [NAME] [ID] [SWITCH] N N N{TAB}N N N
  else if ( flags.switch_flags[13] && numtok > 3 && input.numtok( TSTAB ) > 1 ) {

    TString pathFrom(input.gettok( 1, TSTAB ).gettok( 4, -1 ));
    pathFrom.trim( );
    TString pathTo(input.gettok( 2, TSTAB ));
    pathTo.trim( );

    HTREEITEM hParentFrom = TVI_ROOT;
    HTREEITEM hAfterFrom = TVI_ROOT;
    HTREEITEM hParentTo = TVI_ROOT;
    HTREEITEM hAfterTo = TVI_ROOT;

    HTREEITEM hNewItem;

    if ( !this->parsePath( &pathFrom, &hParentFrom, &hAfterFrom ) )
      return;
    if ( !this->correctTargetItem( &hParentFrom, &hAfterFrom ) )
      return;
    if ( !this->parsePath( &pathTo, &hParentTo, &hAfterTo ) )
      return;

    hNewItem = this->cloneItem( &hAfterFrom, &hParentTo, &hAfterTo );

    if ( hNewItem != NULL )
      this->copyAllItems( &hAfterFrom, &hNewItem );
  }
  // xdid -o [NAME] [ID] [SWITCH] N N N [TAB] (Tooltip Text)
  else if ( flags.switch_flags[14] && numtok > 3 ) {

    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hAfter = TVI_ROOT;

    TString path(input.gettok( 1, TSTAB ).gettok( 4, -1 ));
    path.trim( );

    TString tiptext;
    if ( input.numtok( TSTAB ) > 1 ) {

      tiptext = input.gettok( 2, TSTAB );
      tiptext.trim( );
    }

    if ( this->parsePath( &path, &hParent, &hAfter ) ) {

      if ( this->correctTargetItem( &hParent, &hAfter ) ) {

        TVITEMEX tvi; 

        tvi.hItem = hAfter;
        tvi.mask = TVIF_HANDLE | TVIF_PARAM ; 

        if ( TreeView_GetItem( this->m_Hwnd, &tvi ) ) {

          LPDCXTVITEM lpdcxtvitem = (LPDCXTVITEM) tvi.lParam;

          if ( lpdcxtvitem != NULL )
            lpdcxtvitem->tsTipText = tiptext;
 
        }
      }
    }
  }
  // xdid -Q [NAME] [ID] [SWITCH] [+FLAGS] [COLOR] N N N
  else if ( flags.switch_cap_flags[16] && numtok > 5 ) {
    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hAfter = TVI_ROOT;

    if ( this->parsePath( &input.gettok( 6, -1 ), &hParent, &hAfter ) ) {
      COLORREF clrText = (COLORREF) input.gettok( 5 ).to_num( );

      if ( !this->correctTargetItem( &hParent, &hAfter ) )
        return;

			TVITEMEX tvi; 

      tvi.hItem = hAfter;
      tvi.mask = TVIF_HANDLE | TVIF_PARAM ; 

      if ( TreeView_GetItem( this->m_Hwnd, &tvi ) ) {
        LPDCXTVITEM lpdcxtvitem = (LPDCXTVITEM) tvi.lParam;

        if ( lpdcxtvitem != NULL ) {
          int iFlags = this->parseItemFlags( input.gettok( 4 ) );

          if ( iFlags & TVIS_UNDERLINE )
            lpdcxtvitem->bUline = TRUE;
          else
            lpdcxtvitem->bUline = FALSE;

          if ( iFlags & TVIS_BOLD )
            lpdcxtvitem->bBold = TRUE;
          else
            lpdcxtvitem->bBold = FALSE;

					if (iFlags & TVIS_ITALIC)
						lpdcxtvitem->bItalic = TRUE;
					else
						lpdcxtvitem->bItalic = FALSE;

          if ( iFlags & TVIS_COLOR )
            lpdcxtvitem->clrText = clrText;
          else
            lpdcxtvitem->clrText = (COLORREF)-1;

          this->redrawWindow( );
        }
      }
    }
  }
  // xdid -r [NAME] [ID] [SWITCH]
  else if (flags.switch_flags[17]) {
  }
  // xdid -t [NAME] [ID] [SWITCH] [+FLAGS] N N N
  else if ( flags.switch_flags[19] && numtok > 4 ) {

    UINT iFlags = this->parseToggleFlags( input.gettok( 4 ) );

    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hAfter = TVI_ROOT;

    if ( input.gettok( 5, -1 ) == "root" ) {

      if ( iFlags & TVIE_EXPALL )
        this->expandAllItems( &hAfter, TVE_EXPAND );
      else if ( iFlags & TVIE_COLALL )
        this->expandAllItems( &hAfter, TVE_COLLAPSE );
    }
    else if ( this->parsePath( &input.gettok( 5, -1 ), &hParent, &hAfter ) ) {

      if ( !this->correctTargetItem( &hParent, &hAfter ) )
        return;

      if ( iFlags & TVIE_EXP )
        TreeView_Expand( this->m_Hwnd, hAfter, TVE_EXPAND );
      else if ( iFlags & TVIE_EXPPART )
        TreeView_Expand( this->m_Hwnd, hAfter, TVE_EXPAND | TVE_EXPANDPARTIAL );
      else if ( iFlags & TVIE_COL )
        TreeView_Expand( this->m_Hwnd, hAfter, TVE_COLLAPSE );
      else if ( iFlags & TVIE_COLRES )
        TreeView_Expand( this->m_Hwnd, hAfter, TVE_COLLAPSE | TVE_COLLAPSERESET );
      else if ( iFlags & TVIE_TOGGLE )
        TreeView_Expand( this->m_Hwnd, hAfter, TVE_TOGGLE );
    }
  }
  // xdid -u [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[20] ) {
    
    TreeView_SelectItem( this->m_Hwnd, NULL );
  }
  // xdid -v [NAME] [ID] [SWITCH] N N N [TAB] (Item Text)
  else if ( flags.switch_flags[21] && numtok > 3 ) {

    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hAfter = TVI_ROOT;

    TString path(input.gettok( 1, TSTAB ).gettok( 4, -1 ));
    path.trim( );

    TString itemtext;
    if ( input.numtok( TSTAB ) > 1 ) {
      
      itemtext = input.gettok( 2, TSTAB );
      itemtext.trim( );
    }

    if ( this->parsePath( &path, &hParent, &hAfter ) ) {

      if ( this->correctTargetItem( &hParent, &hAfter ) ) {

        TVITEMEX tvi;

        tvi.mask = TVIF_TEXT | TVIF_HANDLE;
        tvi.hItem = hAfter;
        tvi.pszText = itemtext.to_chr( );
        TreeView_SetItem( this->m_Hwnd, &tvi );
      }
    }
  }
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags.switch_flags[22] && numtok > 5) {
		UINT iFlags = this->parseIconFlagOptions( input.gettok( 4 ) );

		HIMAGELIST himl;
		HICON icon = NULL;

		int index = input.gettok( 5 ).to_int();
		TString filename(input.gettok(6, -1));

		if (this->m_iIconSize > 16)
			icon = dcxLoadIcon(index, filename, TRUE, input.gettok( 4 ));
		else	
			icon = dcxLoadIcon(index, filename, FALSE, input.gettok( 4 ));

		if (iFlags & TVIT_NORMAL) {
			if ((himl = this->getImageList(TVSIL_NORMAL)) == NULL) {
				himl = this->createImageList();

				if (himl)
					this->setImageList(himl, TVSIL_NORMAL);
			}

			ImageList_AddIcon(himl, icon);
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
  else if ( flags.switch_flags[24] && numtok > 3 ) {

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
  else if ( flags.switch_flags[25] && numtok > 4 ) {

    DCXTVSORT dtvs;
    ZeroMemory( &dtvs, sizeof(DCXTVSORT) );
    dtvs.iSortFlags = this->parseSortFlags( input.gettok( 4 ) );
    dtvs.pthis = this;

    TString path(input.gettok( 1, TSTAB ).gettok( 5, -1 ));
    path.trim( );

    if ( input.numtok( TSTAB ) > 1 ) {
      dtvs.tsCustomAlias = input.gettok( 2, TSTAB );
      dtvs.tsCustomAlias.trim( );
    }

    HTREEITEM hParent = TVI_ROOT;
    HTREEITEM hAfter = TVI_ROOT;

    TVSORTCB tvs;
    ZeroMemory( &tvs, sizeof(TVSORTCB) );
    tvs.lpfnCompare = DcxTreeView::sortItemsEx;
    tvs.lParam = (LPARAM) &dtvs;

    if ( path == "root" ) {

      tvs.hParent = TVI_ROOT;

      if ( dtvs.iSortFlags & TVSS_ALL )
        TreeView_SortChildrenCB( this->m_Hwnd, &tvs, TRUE );
      else if ( dtvs.iSortFlags & TVSS_SINGLE )
        TreeView_SortChildrenCB( this->m_Hwnd, &tvs, FALSE );
    }
    else if ( this->parsePath( &path, &hParent, &hAfter ) ) {

      if ( this->correctTargetItem( &hParent, &hAfter ) ) {

        tvs.hParent = hAfter;

        if ( dtvs.iSortFlags & TVSS_ALL )
          TreeView_SortChildrenCB( this->m_Hwnd, &tvs, TRUE );
        else if ( dtvs.iSortFlags & TVSS_SINGLE )
          TreeView_SortChildrenCB( this->m_Hwnd, &tvs, FALSE );
      }
    }
  }
	// xdid -G [NAME] [ID] [SWITCH] [+FLAGS] [X] [Y] (FILENAME)
	else if (flags.switch_cap_flags[6] && numtok > 6) {
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
			else {
				if (!this->m_bTransparent)
					this->setExStyle(WS_EX_TRANSPARENT);
			}
		}
		this->redrawWindow();
	}
  else
    this->parseGlobalCommandRequest( input, flags );
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
HTREEITEM DcxTreeView::insertItem(const TString * path, const TString * data, const TString * Tooltip) {

	HTREEITEM hParent = TVI_ROOT;
	HTREEITEM hAfter = TVI_ROOT;
	HTREEITEM hItem = NULL;

	TVITEMEX tvi;
	TVINSERTSTRUCT tvins;

	ZeroMemory(&tvins, sizeof(TVINSERTSTRUCT));
	ZeroMemory(&tvi, sizeof(TVITEMEX));

	tvi.mask = TVIF_TEXT | TVIF_STATE | TVIF_INTEGRAL | TVIF_PARAM | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	int iFlags			= this->parseItemFlags(data->gettok( 1 ));
	int icon				= data->gettok( 2 ).to_int() -1;
	int sicon			= data->gettok( 3 ).to_int() -1;
	int overlay			= data->gettok( 4 ).to_int();
	int state			= data->gettok( 5 ).to_int();
	int integral		= data->gettok( 6 ).to_int() +1;
	COLORREF clrText	= (COLORREF) data->gettok( 7 ).to_num();
	COLORREF clrBkg = (COLORREF) data->gettok( 8 ).to_num();

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

	// path
	this->parsePath(path, &hParent, &hAfter);

	// text
	TString itemtext(data->gettok(9, -1));

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

	if (state > -1)
		TreeView_SetItemState(this->m_Hwnd, hItem, INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK);
	if (overlay > -1)
		TreeView_SetItemState(this->m_Hwnd, hItem, INDEXTOSTATEIMAGEMASK(overlay), TVIS_OVERLAYMASK);

	return hItem;
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
		mIRCevalEX( res, "$%s(%s,%s)", ptvsort->tsCustomAlias.to_chr( ), itemtext1, itemtext2 );

    if ( ptvsort->iSortFlags & TVSS_DESC ) {

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
BOOL DcxTreeView::parsePath( const TString * path, HTREEITEM * hParent, HTREEITEM * hInsertAfter, const int depth ) {

  int n = path->numtok( ), i = 1;
  int k = path->gettok( depth ).to_int( );
  HTREEITEM hPreviousItem = TVI_FIRST, hCurrentItem;

  hCurrentItem = TreeView_GetChild( this->m_Hwnd, *hParent );

	if ( hCurrentItem == NULL ) {
		if ( k == -1 ) {
			*hInsertAfter = TVI_LAST;
			return TRUE;
		}
		else
			return FALSE;
	}

	do {
		if ( i == k ) {

			if ( depth == n ) {

				*hInsertAfter = hPreviousItem;
				return TRUE;
			}
			else {

				*hParent = hCurrentItem;
				*hInsertAfter = TVI_FIRST;
	      return this->parsePath( path, hParent, hInsertAfter, depth +1);
			}
		}
		i++;
		hPreviousItem = hCurrentItem;

	} while ( ( hCurrentItem = TreeView_GetNextSibling( this->m_Hwnd, hCurrentItem ) ) != NULL );

  if ( k == -1 ) {

    if ( depth == n ) {

      *hInsertAfter = TVI_LAST;
      return TRUE;
    }
    else {

			*hParent = hPreviousItem;
      *hInsertAfter = TVI_FIRST;
      return this->parsePath( path, hParent, hInsertAfter, depth +1);
    }
  }
	else if ((depth == n) && (i == k))
		*hInsertAfter = hPreviousItem;
  return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxTreeView::correctTargetItem( HTREEITEM * hParent, HTREEITEM * hInsertAfter ) {

  if ( *hInsertAfter == TVI_FIRST ) {
    *hInsertAfter = TreeView_GetChild( this->m_Hwnd, *hParent );
  }
  else if ( *hInsertAfter == TVI_LAST ) {

    HTREEITEM hItem = TreeView_GetChild( this->m_Hwnd, *hParent );

    if ( hItem == NULL )
      return FALSE;

    do {
      *hInsertAfter = hItem;
    } while ( ( hItem = TreeView_GetNextSibling( this->m_Hwnd, *hInsertAfter ) ) != NULL );

  }
  else if ( *hInsertAfter == TVI_ROOT ) {
    return FALSE;
  }
  else {

    if ( ( *hInsertAfter = TreeView_GetNextSibling( this->m_Hwnd, *hInsertAfter ) ) == NULL )
      return FALSE;
  }

  return TRUE;
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxTreeView::getPath( VectorOfInts * numPath, HTREEITEM * hStart, HTREEITEM *hItemToFind, int depth ) {

  int i = 0;
  HTREEITEM hCurrentItem;

  if ( ( hCurrentItem = TreeView_GetChild( this->m_Hwnd, *hStart ) ) == NULL )
    return FALSE;

  do {

    i++;

    if ( hCurrentItem == *hItemToFind ) {

      numPath->push_back( i );
      return TRUE;
    }
    else if ( this->getPath( numPath, &hCurrentItem, hItemToFind, depth + 1 ) ) {

      numPath->push_back( i );
      return TRUE;
    }

  } while ( ( hCurrentItem = TreeView_GetNextSibling( this->m_Hwnd, hCurrentItem ) ) != NULL );

  return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */

std::string DcxTreeView::getPathFromVector( VectorOfInts * numPath ) {

 if ( numPath->empty( ) )
   return std::string( "" );

 std::string path = "";

 VectorOfInts::reverse_iterator itStart = numPath->rbegin( );
 VectorOfInts::reverse_iterator itEnd = numPath->rend( );

 char num[5];

 while ( itStart != itEnd ) {

#ifdef VS2005
   _itoa( *itStart, num, 10 );
#else
   itoa( *itStart, num, 10 );
#endif
   path += num;
   path += ' ';

   itStart++;
 }
 return path;
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

/*!
 * \brief blah
 *
 * blah
 */

int DcxTreeView::getItemCount( HTREEITEM * hParent ) const {

  int i = 0;
  HTREEITEM hItem;

  if ( ( hItem = TreeView_GetChild( this->m_Hwnd, *hParent ) ) != NULL )
    ++i;

  do {

    ++i;
  } while ( ( hItem = TreeView_GetNextSibling( this->m_Hwnd, hItem ) ) != NULL );

  return i;
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxTreeView::matchItemText( HTREEITEM * hItem, const TString * search, const UINT SearchType ) {

	char itemtext[900];
	this->getItemText(hItem, itemtext, 900);
	if (SearchType == TVSEARCH_R) {
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
  //char res[10];
  //char itemtext[900];
  //char com[1000];

  //this->getItemText( hItem, itemtext, 900 );
  //// Regex Search
  //if ( SearchType == TVSEARCH_R )
  //  wsprintf( com, "$regex(%s,%s)", itemtext, search->to_chr( ) );
  //// Wildcard Search
  //else
  //  wsprintf( com, "$iif(%s iswm %s,1,0)", search->to_chr( ), itemtext );

  //mIRCeval(com, res);

  //if ( !lstrcmp( res, "1" ) )
  //  return TRUE;

  return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxTreeView::findItemText( HTREEITEM * hStart, HTREEITEM * hItem, const TString * search, const int N, int * NC, const UINT SearchType ) {

  HTREEITEM hCurrentItem;

  if ( ( hCurrentItem = TreeView_GetChild( this->m_Hwnd, *hStart ) ) == NULL )
    return FALSE;

  do {

    if ( this->matchItemText( &hCurrentItem, search, SearchType ) )
      (*NC)++;

    if ( N != 0 && *NC == N ) {
      *hItem = hCurrentItem;
      return TRUE;
    }

    if ( this->findItemText( &hCurrentItem, hItem, search, N, NC, SearchType ) )
      return TRUE;

  } while ( ( hCurrentItem = TreeView_GetNextSibling( this->m_Hwnd, hCurrentItem ) ) != NULL );

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
  switch( uMsg ) {
    case WM_NOTIFY : 
      {

        LPNMHDR hdr = (LPNMHDR) lParam;

        if (!hdr)
          break;

        switch( hdr->code ) {

          case NM_CLICK:
            {
//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/treeview/reflist.asp
              TVHITTESTINFO tvh;
              GetCursorPos( &tvh.pt );
              ScreenToClient( this->m_Hwnd, &tvh.pt );
              TreeView_HitTest( this->m_Hwnd, &tvh );

              if ( tvh.flags & TVHT_ONITEMBUTTON ) {

                VectorOfInts numPath;
                HTREEITEM hStart = TVI_ROOT;
                this->getPath( &numPath, &hStart, &tvh.hItem );
                std::string path = this->getPathFromVector( &numPath );

								if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					        this->callAliasEx( NULL, "%s,%d,%s", "buttonclick", this->getUserID( ), path.c_str( ) );
              }
               //&& this->isStyle( TVS_CHECKBOXES )
              else if ( ( tvh.flags & TVHT_ONITEMSTATEICON ) ) {

                VectorOfInts numPath;
                HTREEITEM hStart = TVI_ROOT;
                this->getPath( &numPath, &hStart, &tvh.hItem );
                std::string path = this->getPathFromVector( &numPath );

                if ( this->isStyle( TVS_CHECKBOXES ) ) {

                  int state = TreeView_GetCheckState( this->m_Hwnd, tvh.hItem );

									if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
						        this->callAliasEx( NULL, "%s,%d,%d,%s", "stateclick", this->getUserID( ), 
							        state == 0 ? 2 : 1 , path.c_str( ) );
                }
                else {
									if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
	                  this->callAliasEx( NULL, "%s,%d,%d,%s", "stateclick", this->getUserID( ), 
		                  TreeView_GetItemState( this->m_Hwnd, tvh.hItem, TVIS_STATEIMAGEMASK ) , path.c_str( ) );
                }
              }
              //|| ( ( tvh.flags & TVHT_ONITEMRIGHT ) && this->isStyle( TVS_FULLROWSELECT ) ) )
              else if ( tvh.flags & TVHT_ONITEM ) {

                VectorOfInts numPath;
                HTREEITEM hStart = TVI_ROOT;
                this->getPath( &numPath, &hStart, &tvh.hItem );
                std::string path = this->getPathFromVector( &numPath );

                TreeView_SelectItem( this->m_Hwnd, tvh.hItem );
                
								if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					        this->callAliasEx( NULL, "%s,%d,%s", "sclick", this->getUserID( ), path.c_str( ) );
              }
							// single click not on item
							else if ((tvh.flags & TVHT_NOWHERE) || (tvh.flags & TVHT_ONITEMRIGHT)) {
								if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
									this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID());
							}
              bParsed = TRUE;
            }
            break;

          case NM_DBLCLK:
            {
              TVHITTESTINFO tvh;
              GetCursorPos( &tvh.pt );
              ScreenToClient( this->m_Hwnd, &tvh.pt );
              TreeView_HitTest( this->m_Hwnd, &tvh );

              //|| ( ( tvh.flags & TVHT_ONITEMRIGHT ) && this->isStyle( TVS_FULLROWSELECT ) ) )
              if ( tvh.flags & TVHT_ONITEM ) {

                VectorOfInts numPath;
                HTREEITEM hStart = TVI_ROOT;
                this->getPath( &numPath, &hStart, &tvh.hItem );
                std::string path = this->getPathFromVector( &numPath );

                TreeView_SelectItem( this->m_Hwnd, tvh.hItem );

								if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					        this->callAliasEx( NULL, "%s,%d,%s", "dclick", this->getUserID( ), path.c_str( ) );
              }
              bParsed = TRUE;
            }
            break;

          case NM_RCLICK:
            {
              TVHITTESTINFO tvh;
              GetCursorPos( &tvh.pt );
              ScreenToClient( this->m_Hwnd, &tvh.pt );
              TreeView_HitTest( this->m_Hwnd, &tvh );

              //|| ( ( tvh.flags & TVHT_ONITEMRIGHT ) && this->isStyle( TVS_FULLROWSELECT ) ) )
              if ( tvh.flags & TVHT_ONITEM ) {

                VectorOfInts numPath;
                HTREEITEM hStart = TVI_ROOT;
                this->getPath( &numPath, &hStart, &tvh.hItem );
                std::string path = this->getPathFromVector( &numPath );

                TreeView_SelectItem( this->m_Hwnd, tvh.hItem );

								if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					        this->callAliasEx( NULL, "%s,%d,%s", "rclick", this->getUserID( ), path.c_str( ) );
              }
						  else {
								if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
								  this->callAliasEx( NULL, "%s,%d", "rclick", this->getUserID());
						  }
              bParsed = TRUE;
            }
            break;

          case TVN_GETINFOTIP:
            {
              LPNMTVGETINFOTIP tcgit = (LPNMTVGETINFOTIP) lParam;
              LPDCXTVITEM lpdcxtvi = (LPDCXTVITEM) tcgit->lParam;
              tcgit->pszText = lpdcxtvi->tsTipText.to_chr( );
              tcgit->cchTextMax = lpdcxtvi->tsTipText.len( );
              bParsed = TRUE;
            }
            break;

          case TVN_ITEMEXPANDED:
            {
              bParsed = TRUE;
              LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW) lParam;

              if ( lpnmtv->action & TVE_COLLAPSE ) {

                VectorOfInts numPath;
                HTREEITEM hStart = TVI_ROOT;
                this->getPath( &numPath, &hStart, &lpnmtv->itemNew.hItem );
                std::string path = this->getPathFromVector( &numPath );

                this->callAliasEx( NULL, "%s,%d,%s", "collapse", this->getUserID( ), path.c_str( ) );
              }
              else if ( lpnmtv->action & TVE_EXPAND ) {

                VectorOfInts numPath;
                HTREEITEM hStart = TVI_ROOT;
                this->getPath( &numPath, &hStart, &lpnmtv->itemNew.hItem );
                std::string path = this->getPathFromVector( &numPath );

                this->callAliasEx( NULL, "%s,%d,%s", "expand", this->getUserID( ), path.c_str( ) );
              }
            }
            break;

          case TVN_BEGINLABELEDIT:
            {
              bParsed = TRUE;
              LPNMTVDISPINFO lptvdi = (LPNMTVDISPINFO) lParam;

              TreeView_SelectItem( this->m_Hwnd,lptvdi->item.hItem );

              HWND edit_hwnd = TreeView_GetEditControl( this->m_Hwnd );

              this->m_OrigEditProc = (WNDPROC) SetWindowLong( edit_hwnd, GWL_WNDPROC, (LONG) DcxTreeView::EditLabelProc );
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

              if ( lptvdi->item.pszText == NULL ) {

                this->callAliasEx( NULL, "%s,%d", "labelcancel", this->getUserID( ) );
              }
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

									//if (this->m_pImage != NULL && this->isExStyle(WS_EX_TRANSPARENT)) {
									//	lpntvcd->clrTextBk = CLR_NONE;
									//	//SetBkMode(lpntvcd->nmcd.hdc, TRANSPARENT);
									//}
									//else {
										// draw unselected background color
										if ((lpdcxtvi->clrBkg != -1) && (!(lpntvcd->nmcd.uItemState & CDIS_SELECTED)))
											lpntvcd->clrTextBk = lpdcxtvi->clrBkg;
										else if ((this->m_colSelection != -1) && (lpntvcd->nmcd.uItemState & CDIS_SELECTED))
										{
											lpntvcd->clrTextBk = this->m_colSelection;
										}
									//}
									//if ( lpdcxtvi->bUline || lpdcxtvi->bBold) {
									HFONT hFont = (HFONT) SendMessage( this->m_Hwnd, WM_GETFONT, 0, 0 );

									LOGFONT lf;
									GetObject( hFont, sizeof(LOGFONT), &lf );

									if (lpdcxtvi->bBold)
										lf.lfWeight |= FW_BOLD;
									if (lpdcxtvi->bUline)
										lf.lfUnderline = true;
									if (lpdcxtvi->bItalic)
										lf.lfItalic = true;

									HFONT hFontNew = CreateFontIndirect( &lf );
									//HFONT hOldFont = (HFONT) SelectObject( lpntvcd->nmcd.hdc, hFontNew );
									SelectObject(lpntvcd->nmcd.hdc, hFontNew);

									DeleteObject( hFontNew );
									//}
								}
								return ( CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT );

                case CDDS_ITEMPOSTPAINT:
                  return CDRF_DODEFAULT;

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
        SetWindowLong( mHwnd, GWL_WNDPROC, (LONG) pthis->m_OrigEditProc );
      }
      break;

  }
  return CallWindowProc( pthis->m_OrigEditProc, mHwnd, uMsg, wParam, lParam );
}

void DcxTreeView::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	// Setup alpha blend if any.
	LPALPHAINFO ai = this->SetupAlphaBlend(&hdc, true);

	if (mIRCLink.m_bUseGDIPlus && this->m_pImage != NULL)
		DrawGDIPlusImage(hdc);

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
		this->showError(NULL,"LoadGDIPlusImage", "Unable to open file");
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
