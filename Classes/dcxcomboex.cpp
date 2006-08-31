/*!
 * \file dcxcomboex.cpp
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

#include "dcxcomboex.h"
#include "dcxdialog.h"

void DcxComboEx::ConstructComboEx( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles )
{
  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles, 
    DCX_COMBOEXCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | CBS_AUTOHSCROLL | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SendMessage( this->m_Hwnd , CBEM_SETWINDOWTHEME, 0, (LPARAM)L" " );
  //if ( bNoTheme )
  //  SetWindowTheme( this->m_Hwnd , L" ", L" " );

  this->m_EditHwnd = (HWND) this->getEditControl( );

  if ( IsWindow( this->m_EditHwnd ) ) {
		//if ( bNoTheme )
		//	SetWindowTheme( this->m_EditHwnd , L" ", L" " );

    LPDCXCOMBOEXEDIT lpce = new DCXCOMBOEXEDIT;

    lpce->cHwnd = this->m_Hwnd;
    lpce->pHwnd = mParentHwnd;

    SetWindowLong( this->m_EditHwnd, GWL_STYLE, GetWindowLong( this->m_EditHwnd, GWL_STYLE ) | ES_AUTOHSCROLL );

    lpce->OldProc = (WNDPROC) SetWindowLong( this->m_EditHwnd, GWL_WNDPROC, (LONG) DcxComboEx::ComboExEditProc );
    SetWindowLong( this->m_EditHwnd, GWL_USERDATA, (LONG) lpce );
  }

	//HWND combo = (HWND)SendMessage(this->m_Hwnd,CBEM_GETCOMBOCONTROL,0,0);
	//if (IsWindow(combo) && bNoTheme)
	//	SetWindowTheme( combo , L" ", L" " );

	//if (p_Dialog->getToolTip() != NULL) {
	//	if (styles.istok("tooltips"," ")) {

	//		this->m_ToolTipHWND = p_Dialog->getToolTip();

	//		AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
	//		AddToolTipToolInfo(this->m_ToolTipHWND, this->m_EditHwnd);
	//	}
	//}

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );

  DragAcceptFiles(this->m_Hwnd, TRUE);

	// fix bug with disabled creation
	// todo: fix this properly
	if (Styles & WS_DISABLED) {
		EnableWindow(this->m_Hwnd, TRUE);
		EnableWindow(this->m_Hwnd, FALSE);
	}
}
/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxComboEx::DcxComboEx( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{
	this->ConstructComboEx(ID,p_Dialog,p_Dialog->getHwnd(),rc,styles);
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

DcxComboEx::DcxComboEx( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{
	this->ConstructComboEx(ID,p_Dialog,mParentHwnd,rc,styles);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxComboEx::~DcxComboEx( ) {

  ImageList_Destroy( this->getImageList( ) );
  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxComboEx::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  //*ExStyles |= CBES_EX_NOSIZELIMIT;
  unsigned int i = 1, numtok = styles.numtok( " " );

  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "simple" )
      *Styles |= CBS_SIMPLE;
    else if ( styles.gettok( i , " " ) == "dropdown" )
      *Styles |= CBS_DROPDOWNLIST;
    else if ( styles.gettok( i , " " ) == "dropedit" )
      *Styles |= CBS_DROPDOWN;

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

void DcxComboEx::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );

  // [NAME] [ID] [PROP] [N]
  if ( input.gettok( 3, " " ) == "text" && numtok > 3 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nItem > -1 ) {
      
      COMBOBOXEXITEM cbi;
      ZeroMemory( &cbi, sizeof( COMBOBOXEXITEM ) );

      cbi.mask = CBEIF_TEXT;
      cbi.iItem = nItem;
      cbi.pszText = szReturnValue;
      cbi.cchTextMax = 900;

      this->getItem( &cbi );
      return;
    }
    else if ( nItem == -1 && ( this->isStyle( CBS_DROPDOWN ) || this->isStyle( CBS_SIMPLE ) ) ) {
      
      GetWindowText( (HWND) this->getEditControl( ), szReturnValue, 900 );
      return;
    }
  }
  // [NAME] [ID] [PROP]
  else if ( input.gettok( 3, " " ) == "seltext" ) {

    int nItem = this->getCurSel( );

    if ( nItem > -1 ) {

      COMBOBOXEXITEM cbi;
      ZeroMemory( &cbi, sizeof( COMBOBOXEXITEM ) );

      cbi.mask = CBEIF_TEXT;
      cbi.iItem = nItem;
      cbi.pszText = szReturnValue;
      cbi.cchTextMax = 900;

      this->getItem( &cbi );
      return;
    }
  }
  // [NAME] [ID] [PROP]
  else if ( input.gettok( 3, " " ) == "sel" ) {

    int nItem = this->getCurSel( );
    
    if ( nItem > -1 ) {

      wsprintf( szReturnValue, "%d", nItem + 1 );
      return;
    }
  }
  // [NAME] [ID] [PROP]
  else if ( input.gettok( 3, " " ) == "num" ) {

    wsprintf( szReturnValue, "%d", this->getCount( ) );
    return;
  }
  // [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N]
  else if ( input.gettok( 3, " " ) == "find" && numtok > 5 ) {

    TString matchtext = input.gettok( 2, "\t" );
    matchtext.trim( );
    TString params = input.gettok( 3, "\t" );
    params.trim( );

    if ( matchtext.len( ) > 0 ) {

      UINT SearchType;

      if ( params.gettok( 1, " " ) == "R" )
        SearchType = CBEXSEARCH_R;
      else
        SearchType = CBEXSEARCH_W;

      int N = (int) atoi( params.gettok( 2, " " ).to_chr( ) );

      // count total
      if ( N == 0 ) {

        int nItems = this->getCount( ), i = 0, count = 0;

        while ( i < nItems ) {

          if ( this->matchItemText( i, &matchtext, SearchType ) )
            count++;

          i++;
        }

        wsprintf( szReturnValue, "%d", count );
        return;
      }
      // find Nth matching
      else {

        int nItems = this->getCount( ), i = 0, count = 0;

        while ( i < nItems ) {

          if ( this->matchItemText( i, &matchtext, SearchType ) )
            count++;

          if ( count == N ) {

            wsprintf( szReturnValue, "%d", i + 1 );
            return;
          }

          i++;
        }
      } // else
    }
    return;
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

void DcxComboEx::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  // xdid -a [NAME] [ID] [SWITCH] [N] [INDENT] [ICON] [STATE] [OVERLAY] Item Text
  if ( flags.switch_flags[0] && numtok > 8 ) {

    int nPos = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    int indent = atoi( input.gettok( 5, " " ).to_chr( ) );
    int icon = atoi( input.gettok( 6, " " ).to_chr( ) ) - 1;
    int state = atoi( input.gettok( 7, " " ).to_chr( ) ) - 1;
    //int overlay = atoi( input.gettok( 8, " " ).to_chr( ) ) - 1;
    TString itemtext = input.gettok( 9, -1, " " );

    COMBOBOXEXITEM cbi;
    ZeroMemory( &cbi, sizeof( COMBOBOXEXITEM ) );

    cbi.mask = CBEIF_TEXT | CBEIF_INDENT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
    cbi.iIndent = indent;
    cbi.iImage = icon;
    cbi.iSelectedImage = state;
    //cbi.iOverlay = overlay;
    cbi.pszText = itemtext.to_chr( );
    cbi.iItem = nPos;

    this->insertItem( &cbi );

  }
  // xdid -c [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[2] && numtok > 3 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

	 if ( nItem > -1 ) {
      this->setCurSel( nItem );
	 }
  }
  // xdid -d [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[3] && numtok > 3 ) {
    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

	 if (nItem > -1 && nItem < this->getCount())
      this->deleteItem( nItem );

	 if (!this->getCount())
		 this->redrawWindow();
  }
  // xdid -r [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[17] ) {

    this->resetContent( );
  }
  // xdid -u [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[20] ) {

    this->setCurSel( -1 );
  }
  // xdid -w [NAME] [ID] [SWITCH] [INDEX] [FILENAME]
  else if ( flags.switch_flags[22] && numtok > 4 ) {

    HIMAGELIST himl;
    HICON icon;
    int index;

    if ( ( himl = this->getImageList( ) ) == NULL ) {

      himl = this->createImageList( );

      if ( himl )
        this->setImageList( himl );
    }

    index = atoi( input.gettok( 4, " ").to_chr( ) );
    TString filename = input.gettok( 5, -1, " " );
    ExtractIconEx( filename.to_chr( ), index, 0, &icon, 1 );
    ImageList_AddIcon( himl, icon );
    DestroyIcon( icon );
  }
  // xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
  else if ( flags.switch_flags[24] ) {

    ImageList_Destroy( this->getImageList( ) );
    this->setImageList( NULL );
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

HIMAGELIST DcxComboEx::getImageList(  ) {

  return (HIMAGELIST) SendMessage( this->m_Hwnd, CBEM_GETIMAGELIST, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxComboEx::setImageList( HIMAGELIST himl ) {

  SendMessage( this->m_Hwnd, CBEM_SETIMAGELIST, (WPARAM) 0, (LPARAM) himl );
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxComboEx::createImageList( ) {

  return ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxComboEx::matchItemText( int nItem, TString * search, UINT SearchType ) {

  char res[10];
  char itemtext[900];
  char com[1000];

  COMBOBOXEXITEM cbi;
  ZeroMemory( &cbi, sizeof( COMBOBOXEXITEM ) );

  cbi.mask = CBEIF_TEXT;
  cbi.iItem = nItem;
  cbi.pszText = itemtext;
  cbi.cchTextMax = 900;

  this->getItem( &cbi );

  // Regex Search
  if ( SearchType == CBEXSEARCH_R )
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

LRESULT DcxComboEx::insertItem( PCOMBOBOXEXITEM lpcCBItem ) {
  return SendMessage( this->m_Hwnd, CBEM_INSERTITEM, (WPARAM) 0, (LPARAM) lpcCBItem );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxComboEx::getItem( PCOMBOBOXEXITEM lpcCBItem ) {
  return SendMessage( this->m_Hwnd, CBEM_GETITEM, (WPARAM) 0, (LPARAM) lpcCBItem );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxComboEx::getEditControl( ) {
  return SendMessage( this->m_Hwnd, CBEM_GETEDITCONTROL, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxComboEx::deleteItem( int iIndex ) {
  return SendMessage( this->m_Hwnd, CBEM_DELETEITEM, (WPARAM) iIndex, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxComboEx::setCurSel( int iIndex ) {
  return SendMessage( this->m_Hwnd, CB_SETCURSEL, (WPARAM) iIndex, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxComboEx::getCurSel( ) {
  return SendMessage( this->m_Hwnd, CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxComboEx::getLBText( int iIndex, LPSTR lps ) {
  return SendMessage( this->m_Hwnd, CB_GETLBTEXT, (WPARAM) iIndex, (LPARAM) lps );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxComboEx::resetContent( ) {
  return SendMessage( this->m_Hwnd, CB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxComboEx::getCount( ) {
  return SendMessage( this->m_Hwnd, CB_GETCOUNT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxComboEx::limitText( int iLimit ) {
  return SendMessage( this->m_Hwnd, CB_LIMITTEXT, (WPARAM) iLimit, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxComboEx::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
  switch( uMsg ) {
    case WM_COMMAND:
      {
        switch( HIWORD( wParam ) ) {

          case CBN_DBLCLK:
            {
              this->callAliasEx( NULL, "%s,%d,%d", "dclick", this->getUserID( ), this->getCurSel( ) + 1 );
              bParsed = TRUE;
              return TRUE;
            }
            break;

          case CBN_SELENDOK:
            {
              this->callAliasEx( NULL, "%s,%d,%d", "sclick", this->getUserID( ), this->getCurSel( ) + 1 );
              char itemtext[500];
              COMBOBOXEXITEM cbex;
              ZeroMemory( &cbex, sizeof( COMBOBOXEXITEM ) );
              cbex.mask = CBEIF_TEXT;
              cbex.pszText = itemtext;
              cbex.cchTextMax = 500;
              cbex.iItem = this->getCurSel( );
              this->getItem( &cbex );
              SetWindowText( this->m_EditHwnd, itemtext );
              bParsed = TRUE;
              return TRUE;
            }
            break;

          case CBN_EDITCHANGE:
            {
              this->callAliasEx( NULL, "%s,%d", "edit", this->getUserID( ) );
              bParsed = TRUE;
              return TRUE;
            }
            break;          
        } // switch
        bParsed = TRUE;
      }
      break;
	}
	return 0L;
}

LRESULT DcxComboEx::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
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

    case WM_SETCURSOR:
      {
        if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd && this->m_hCursor != NULL ) {

          SetCursor( this->m_hCursor );
          bParsed = TRUE;
          return TRUE;
        }
      }
      break;
	case WM_DROPFILES:
	{
		HDROP files = (HDROP) wParam;
		char filename[500];
      int count = DragQueryFile(files, 0xFFFFFFFF,  filename, 500);

		if (count) {
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

		DragFinish(files);
		break;
	}
		case WM_MOUSEACTIVATE:
			{
				//mIRCError( "WM_MOUSEACTIVATE" );
				switch (HIWORD(lParam))
				{
				case WM_RBUTTONDOWN:
					{
						//mIRCError( "ComboEx WM_RBUTTONDOWN" );
						// NB: rclick doesnt change selection!
						this->callAliasEx( NULL, "%s,%d,%d", "rclick", this->getUserID( ), this->getCurSel( ) + 1 );
					}
					break;
				}
				//TODO: Add `ownmenu` setting or similar to stop default edit menu & replace with own.
				// this could be done with most if not all controls.
				//bParsed = TRUE;
				//return MA_ACTIVATE;
			}
			break;
    case WM_NCDESTROY:
      {
        delete this;
        bParsed = TRUE;
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

LRESULT CALLBACK DcxComboEx::ComboExEditProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

  LPDCXCOMBOEXEDIT lpce = (LPDCXCOMBOEXEDIT) GetWindowLong( mHwnd, GWL_USERDATA );
	if (lpce == NULL)	return DefWindowProc( mHwnd, uMsg, wParam, lParam );
  //mIRCError( "DcxComboEx::ComboExEditProc" );
  switch( uMsg ) {

    case WM_GETDLGCODE:
      return DLGC_WANTALLKEYS;

    case WM_KEYDOWN:
      {
        if ( wParam == VK_RETURN ) {

          DcxControl * pthis = (DcxControl *) GetProp( lpce->cHwnd, "dcx_cthis" );

          if ( pthis != NULL ) {
            pthis->callAliasEx( NULL, "%s,%d", "return", pthis->getUserID( ) );
          }
        }
      }
      break;
		//case WM_NOTIFY:
		//	{
  //      LPNMHDR hdr = (LPNMHDR) lParam;
  //      if (!hdr)
  //        break;

  //      switch( hdr->code ) {
		//		case TTN_GETDISPINFO:
		//			{
	 //         DcxControl * pthis = (DcxControl *) GetProp( lpce->cHwnd, "dcx_cthis" );
	 //         if ( pthis != NULL ) {
		//					LPNMTTDISPINFO di = (LPNMTTDISPINFO)lParam;
		//					di->lpszText = "test";
		//					di->hinst = NULL;
		//				}
		//				return 0L;
		//			}
		//			break;
		//		case TTN_LINKCLICK:
		//			{
	 //         DcxControl * pthis = (DcxControl *) GetProp( lpce->cHwnd, "dcx_cthis" );
	 //         if ( pthis != NULL ) {
		//					pthis->callAliasEx( NULL, "%s,%d", "tooltiplink", pthis->getUserID( ) );
		//				}
		//				return 0L;
		//			}
		//			break;
		//		}
		//	}
		//	break;

    case WM_NCDESTROY:
      {
        
        WNDPROC OldProc = lpce->OldProc;
        if ( lpce )
          delete lpce;

        return CallWindowProc( OldProc, mHwnd, uMsg, wParam, lParam );
      }
      break;
  }
  return CallWindowProc( lpce->OldProc, mHwnd, uMsg, wParam, lParam );
}