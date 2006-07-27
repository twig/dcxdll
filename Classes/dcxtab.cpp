/*!
 * \file dcxtab.cpp
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

#include "dcxtab.h"
#include "dcxdialog.h"

//#include "dcxprogressbar.h"
//#include "dcxtrackbar.h"
//#include "dcxcolorcombo.h"
//#include "dcxcomboex.h"
//#include "dcxstatusbar.h"
//#include "dcxtoolbar.h"
//#include "dcxbutton.h"
#include "dcxtreeview.h"
#include "dcxlistview.h"
#include "dcxrebar.h"
#include "dcxrichedit.h"
#include "dcxwebctrl.h"

#include "dcxpanel.h"
#include "dcxdivider.h"

#include "mirc/dcxedit.h"
#include "mirc/dcxlist.h"
#include "mirc/dcximage.h"

#include "dcxmwindow.h"
#include "dcxmdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxTab::DcxTab( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles, 
    DCX_TABCTRLCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle( NULL ), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  /*
  HWND hHwndTip = TabCtrl_GetToolTips( this->m_Hwnd );
  if ( IsWindow( hHwndTip ) ) {

    char data[500];
    wsprintf( data, "Activating Tooltips TT HWND %x", hHwndTip );
    mIRCError( data );

    TOOLINFO ti;
    ZeroMemory( &ti, sizeof( TOOLINFO ) );
    ti.cbSize = sizeof( TOOLINFO );
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.hwnd = p_Dialog->getHwnd( );
    ti.uId = (UINT) this->m_Hwnd;
    ti.lpszText = LPSTR_TEXTCALLBACK;
    SendMessage( hHwndTip, TTM_ADDTOOL, (WPARAM) 0, (LPARAM) &ti );
  }
  */

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

DcxTab::DcxTab( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles, 
    DCX_TABCTRLCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  /*
  HWND hHwndTip = TabCtrl_GetToolTips( this->m_Hwnd );
  if ( IsWindow( hHwndTip ) ) {

    TOOLINFO ti;
    ZeroMemory( &ti, sizeof( TOOLINFO ) );
    ti.cbSize = sizeof( TOOLINFO );
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.hwnd = mParentHwnd;
    ti.uId = (UINT) this->m_Hwnd;
    ti.lpszText = LPSTR_TEXTCALLBACK;
    SendMessage( hHwndTip, TTM_ADDTOOL, (WPARAM) 0, (LPARAM) &ti );
  }
  */

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxTab::~DcxTab( ) {

  ImageList_Destroy( this->getImageList( ) );

  int n = 0, nItems = TabCtrl_GetItemCount( this->m_Hwnd );
  while ( n < nItems ) {

    this->deleteLParamInfo( n );
    ++n;
  }

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTab::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  unsigned int i = 1, numtok = styles.numtok( " " );

	//*ExStyles = WS_EX_CONTROLPARENT;

  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "vertical" )
      *Styles |= TCS_VERTICAL;
    else if ( styles.gettok( i , " " ) == "bottom" )
      *Styles |= TCS_BOTTOM;
    else if ( styles.gettok( i , " " ) == "right" )
      *Styles |= TCS_RIGHT;
    else if ( styles.gettok( i , " " ) == "fixedwidth" )
      *Styles |= TCS_FIXEDWIDTH;
    else if ( styles.gettok( i , " " ) == "buttons" )
      *Styles |= TCS_BUTTONS;
    else if ( styles.gettok( i , " " ) == "flat" )
      *Styles |= TCS_FLATBUTTONS;
    else if ( styles.gettok( i , " " ) == "hot" )
      *Styles |= TCS_HOTTRACK;
    else if ( styles.gettok( i , " " ) == "multiline" )
      *Styles |= TCS_MULTILINE;
    else if ( styles.gettok( i , " " ) == "rightjustify" )
      *Styles |= TCS_RIGHTJUSTIFY;
    else if ( styles.gettok( i , " " ) == "scrollopposite" )
      *Styles |= TCS_SCROLLOPPOSITE;
    //else if ( styles.gettok( i , " " ) == "tooltips" )
      //*Styles |= TCS_TOOLTIPS;
    else if ( styles.gettok( i , " " ) == "flatseps" )
      *ExStyles |= TCS_EX_FLATSEPARATORS;

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

void DcxTab::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );

  if ( input.gettok( 3, " " ) == "text" && numtok > 3 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nItem > -1 && nItem < TabCtrl_GetItemCount( this->m_Hwnd ) ) {
      
      TCITEM tci;
      ZeroMemory( &tci, sizeof( TCITEM ) );

      tci.mask = TCIF_TEXT;
      tci.pszText = szReturnValue;
      tci.cchTextMax = 900;

      TabCtrl_GetItem( this->m_Hwnd, nItem, &tci );
      return;
    }
  }
  else if ( input.gettok( 3, " " ) == "num" ) {

    wsprintf( szReturnValue, "%d", TabCtrl_GetItemCount( this->m_Hwnd ) );
    return;
  }
  // [NAME] [ID] [PROP] [N]
  else if ( input.gettok( 3, " " ) == "icon" && numtok > 3 ) {

    int iTab = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( iTab > -1 && iTab < TabCtrl_GetItemCount( this->m_Hwnd ) ) {

      TCITEM tci;
      ZeroMemory( &tci, sizeof( TCITEM ) );

      tci.mask = TCIF_IMAGE;

      TabCtrl_GetItem( this->m_Hwnd, iTab, &tci );

      wsprintf( szReturnValue, "%d", tci.iImage + 1 );
      return;
    }
  }
  else if ( input.gettok( 3, " " ) == "sel" ) {

    int nItem = TabCtrl_GetCurSel( this->m_Hwnd );

    if ( nItem > -1 && nItem < TabCtrl_GetItemCount( this->m_Hwnd ) ) {

      wsprintf( szReturnValue, "%d", nItem + 1 );
      return;
    }
  }
  else if ( input.gettok( 3, " " ) == "seltext" ) {

    int nItem = TabCtrl_GetCurSel( this->m_Hwnd );

    if ( nItem > -1 && nItem < TabCtrl_GetItemCount( this->m_Hwnd ) ) {
      
      TCITEM tci;
      ZeroMemory( &tci, sizeof( TCITEM ) );

      tci.mask = TCIF_TEXT;
      tci.pszText = szReturnValue;
      tci.cchTextMax = 900;

      TabCtrl_GetItem( this->m_Hwnd, nItem, &tci );
      return;
    }
  }
  if ( input.gettok( 3, " " ) == "childid" && numtok > 3 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nItem > -1 && nItem < TabCtrl_GetItemCount( this->m_Hwnd ) ) {
      
      TCITEM tci;
      ZeroMemory( &tci, sizeof( TCITEM ) );

      tci.mask = TCIF_PARAM;
      TabCtrl_GetItem( this->m_Hwnd, nItem, &tci );

      LPDCXTCITEM lpdtci = (LPDCXTCITEM) tci.lParam;

      DcxControl * c = this->m_pParentDialog->getControlByHWND( lpdtci->mChildHwnd );
      if ( c != NULL ) 
        wsprintf( szReturnValue, "%d", c->getUserID( ) );

      return;
    }
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

void DcxTab::parseCommandRequest( TString & input ) {
  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  // xdid -a [NAME] [ID] [SWITCH] [N] [ICON] [TEXT][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)[TAB](TOOLTIP)
  if ( flags.switch_flags[0] && numtok > 4 ) {
    TCITEM tci;
    ZeroMemory( &tci, sizeof( TCITEM ) );
    tci.mask = TCIF_IMAGE | TCIF_PARAM;

    TString data = input.gettok( 1, "\t" );
    data.trim( );

    TString control_data = "";
    if ( input.numtok( "\t" ) > 1 ) {
      control_data = input.gettok( 2, "\t" );
      control_data.trim( );
    }

    TString tooltip = "";
    if ( input.numtok( "\t" ) > 2 ) {
      tooltip = input.gettok( 3, "\t" );
      tooltip.trim( );
    }

    int nIndex = atoi( data.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nIndex == -1 )
      nIndex += TabCtrl_GetItemCount( this->m_Hwnd ) + 1;

	 tci.iImage = atoi( data.gettok( 5, " " ).to_chr( ) ) - 1;

    // Extra params
    LPDCXTCITEM lpdtci = new DCXTCITEM;
    lpdtci->tsTipText = tooltip;
    tci.lParam = (LPARAM) lpdtci;

    // Itemtext
    TString itemtext;
    if ( data.numtok( " " ) > 5 ) {
      itemtext = data.gettok( 6, -1, " " );
      tci.mask |= TCIF_TEXT;
      tci.pszText = itemtext.to_chr( );
    }

    if ( control_data.numtok( " " ) > 5 ) {
      UINT ID = mIRC_ID_OFFSET + atoi( control_data.gettok( 1, " " ).to_chr( ) );

      if ( ID > mIRC_ID_OFFSET - 1 && 
        !IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
        this->m_pParentDialog->getControlByID( ID ) == NULL ) 
      {
        TString type = control_data.gettok( 2, " " );
        RECT rc;

        rc.left = atol( control_data.gettok( 3, " " ).to_chr( ) );
        rc.top = atol( control_data.gettok( 4, " " ).to_chr( ) );
        rc.right = rc.left + atol( control_data.gettok( 5, " " ).to_chr( ) );
        rc.bottom = rc.top + atol( control_data.gettok( 6, " " ).to_chr( ) );

        DcxControl * p_Control = NULL;

        TString styles = "";
        if ( control_data.numtok( " " ) > 6 ) {
          styles = control_data.gettok( 7, -1, " " );
        }

        /*
        if ( type == "trackbar" ) {
        p_Control = new DcxTrackBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles  );
        }
        else if ( type == "pbar" ) {
        p_Control = new DcxProgressBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "comboex" ) {
        p_Control = new DcxComboEx( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "toolbar" ) {
        p_Control = new DcxToolBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        p_Control->addStyle( CCS_NOPARENTALIGN | CCS_NORESIZE );
        }
        else if ( type == "statusbar" ) {
        p_Control = new DcxStatusBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        p_Control->addStyle( CCS_NOPARENTALIGN | CCS_NORESIZE );
        }
        else if ( type == "colorcombo" ) {
        p_Control = new DcxColorCombo( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "button" ) {
        p_Control = new DcxButton( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        */

        if ( type == "treeview" ) {
          p_Control = new DcxTreeView( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "listview" ) {
          p_Control = new DcxListView( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "richedit" ) {
          p_Control = new DcxRichEdit( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "divider" ) {
          p_Control = new DcxDivider( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "panel" ) {
          p_Control = new DcxPanel( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "tab" ) {
          p_Control = new DcxTab( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "rebar" ) {
          p_Control = new DcxReBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "webctrl" ) {
          p_Control = new DcxWebControl( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "edit" ) {
          p_Control = new DcxEdit( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "image" ) {
          p_Control = new DcxImage( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "list" ) {
          p_Control = new DcxList( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }

        if ( p_Control != NULL ) {
          lpdtci->mChildHwnd = p_Control->getHwnd( );
          this->m_pParentDialog->addControl( p_Control );
        }
      }
      else {
        char error[500];
        wsprintf( error, "/xdid -a : Control with ID \"%d\" already exists", ID - mIRC_ID_OFFSET );
        mIRCError( error );
      }
    }

    TabCtrl_InsertItem( this->m_Hwnd, nIndex, &tci );
    this->activateSelectedTab( );
  }
  // xdid -c [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[2] && numtok > 3 ) {
    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nItem > -1 && nItem < TabCtrl_GetItemCount( this->m_Hwnd ) ) {
      TabCtrl_SetCurSel( this->m_Hwnd, nItem );
      this->activateSelectedTab( );
    }
  }
  // xdid -d [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[3] && numtok > 3 ) {
	  int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

	  // if a valid item to delete
	  if ( nItem > -1 && nItem < TabCtrl_GetItemCount( this->m_Hwnd ) ) {
		  int curSel = TabCtrl_GetCurSel(this->m_Hwnd);
		  TCITEM tci;
		  ZeroMemory( &tci, sizeof( TCITEM ) );

		  tci.mask = TCIF_PARAM;

		  if (TabCtrl_GetItem(this->m_Hwnd, nItem, &tci)) {
			  LPDCXTCITEM lpdtci = (LPDCXTCITEM) tci.lParam;

			  if ( lpdtci != NULL && lpdtci->mChildHwnd != NULL && IsWindow( lpdtci->mChildHwnd ) ) {
				  DestroyWindow( lpdtci->mChildHwnd );
				  delete lpdtci;
			  }
		  }

		  TabCtrl_DeleteItem( this->m_Hwnd, nItem );

		  // select the next tab item if its the current one
		  if (curSel == nItem) {
			  if (nItem < TabCtrl_GetItemCount(this->m_Hwnd))
				  TabCtrl_SetCurSel(this->m_Hwnd, nItem);
			  else
				  TabCtrl_SetCurSel(this->m_Hwnd, nItem -1);

			  this->activateSelectedTab( );
		  }
	  }
  }
  // xdid -l [NAME] [ID] [SWITCH] [N] [ICON]
  else if ( flags.switch_flags[11] && numtok > 4 ) {
    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    int nIcon = atoi( input.gettok( 5, " " ).to_chr( ) ) - 1;

    if ( nItem > -1 && nItem < TabCtrl_GetItemCount( this->m_Hwnd ) ) {
     TCITEM tci;
     ZeroMemory( &tci, sizeof( TCITEM ) );
     tci.mask = TCIF_IMAGE;
     tci.iImage = nIcon;

     TabCtrl_SetItem( this->m_Hwnd, nItem, &tci );
    }
  }
  // xdid -m [NAME] [ID] [SWITCH] [X] [Y]
  else if ( flags.switch_flags[12] && numtok > 4 ) {

    int X = atoi( input.gettok( 4, " " ).to_chr( ) );
    int Y = atoi( input.gettok( 5, " " ).to_chr( ) );

    TabCtrl_SetItemSize( this->m_Hwnd, X, Y );
  }
  // xdid -r [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[17] ) {

    int n = 0, nItems = TabCtrl_GetItemCount( this->m_Hwnd );
    while ( n < nItems ) {

      TCITEM tci;
      ZeroMemory( &tci, sizeof( TCITEM ) );

      tci.mask = TCIF_PARAM;

      if ( TabCtrl_GetItem( this->m_Hwnd, n, &tci ) ) {

        LPDCXTCITEM lpdtci = (LPDCXTCITEM) tci.lParam;
        if ( lpdtci != NULL && lpdtci->mChildHwnd != NULL && IsWindow( lpdtci->mChildHwnd ) ) {
          DestroyWindow( lpdtci->mChildHwnd );
          delete lpdtci;
        }
      }
      ++n;
    }

    TabCtrl_DeleteAllItems( this->m_Hwnd );
  }
  // xdid -t [NAME] [ID] [SWITCH] [N] (text)
  else if ( flags.switch_flags[19] && numtok > 3 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nItem > -1 && nItem < TabCtrl_GetItemCount( this->m_Hwnd ) ) {

      TString itemtext = "";

      TCITEM tci;
      ZeroMemory( &tci, sizeof( TCITEM ) );
      tci.mask = TCIF_TEXT;


      if ( numtok > 4 ) {
        itemtext = input.gettok( 5, -1, " " );
        itemtext.trim( );
      }

      tci.pszText = itemtext.to_chr( );

      TabCtrl_SetItem( this->m_Hwnd, nItem, &tci );
    }
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

HIMAGELIST DcxTab::getImageList(  ) {

  return TabCtrl_GetImageList( this->m_Hwnd );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTab::setImageList( HIMAGELIST himl ) {

  TabCtrl_SetImageList( this->m_Hwnd, himl );
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxTab::createImageList( ) {

  return ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTab::deleteLParamInfo( int nItem ) {

  TCITEM tci;
  ZeroMemory( &tci, sizeof( TCITEM ) );

  tci.mask = TCIF_PARAM;

  if ( TabCtrl_GetItem( this->m_Hwnd, nItem, &tci ) ) {

    LPDCXTCITEM lpdtci = (LPDCXTCITEM) tci.lParam;

    if ( lpdtci != NULL )
      delete lpdtci;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTab::activateSelectedTab( ) {

  int nTab = TabCtrl_GetItemCount( this->m_Hwnd );
  int nSel = TabCtrl_GetCurSel( this->m_Hwnd );

  if ( nTab > 0 ) {

    RECT tabrect, rc;
    GetWindowRect( this->m_Hwnd, &tabrect );
    TabCtrl_AdjustRect( this->m_Hwnd, FALSE, &tabrect );
    GetWindowRect( this->m_Hwnd, &rc );

    OffsetRect( &tabrect, -rc.left, -rc.top );

    /*
    char data[500];
    wsprintf( data, "WRECT %d %d %d %d - ARECT %d %d %d %d", 
      rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top,
      tabrect.left, tabrect.top, tabrect.right-tabrect.left, tabrect.bottom-tabrect.top );

    mIRCError( data );
    */

    TCITEM tci;
    ZeroMemory( &tci, sizeof( TCITEM ) );
    tci.mask = TCIF_PARAM;

    HDWP hdwp = BeginDeferWindowPos( 0 );
    while ( nTab-- ) {

      TabCtrl_GetItem( this->m_Hwnd, nTab, &tci );
      LPDCXTCITEM lpdtci = (LPDCXTCITEM) tci.lParam;

      if ( lpdtci->mChildHwnd != NULL && IsWindow( lpdtci->mChildHwnd ) ) {

        if ( nTab == nSel ) {

          hdwp = DeferWindowPos( hdwp, lpdtci->mChildHwnd, NULL, 
            tabrect.left, tabrect.top, tabrect.right-tabrect.left, tabrect.bottom-tabrect.top, 
            SWP_SHOWWINDOW | SWP_NOZORDER );
        }
        else {
          hdwp = DeferWindowPos( hdwp, lpdtci->mChildHwnd, NULL, 0, 0, 0, 0, 
            SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER );
        }
      }
    }
    EndDeferWindowPos( hdwp );
  }
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTab::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	//mIRCDebug("tab: %d", uMsg);
  switch( uMsg ) {

    case WM_HELP:
      {
        char ret[256];
        this->callAliasEx( ret, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_NOTIFY : 
      {

        //mIRCError( "Tab WM_NOTIFY" );
        LPNMHDR hdr = (LPNMHDR) lParam;

        if (!hdr)
          break;

        char ClassName[256];

        if ( IsWindow( hdr->hwndFrom ) && GetClassName( hdr->hwndFrom, ClassName, 256 ) != 0 ) {

          switch( hdr->code ) {

            case NM_CLICK:
              {
                //mIRCError( "Tab WM_NOTIFY - NM_CLICK" );
                if ( lstrcmp( DCX_STATUSBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            case NM_DBLCLK:
              {
                //mIRCError( "Tab WM_NOTIFY - NM_DBLCLK" );
                if ( lstrcmp( DCX_STATUSBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            case NM_RCLICK:
              {
                //mIRCError( "Dialog WM_NOTIFY - NM_RCLICK" );
                if ( lstrcmp( DCX_STATUSBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TABCTRLCLASS, ClassName ) == 0) {

                  // Not a forward to this window, comes from a child window
                  if ( this->m_Hwnd != hdr->hwndFrom ) {
                    bParsed = TRUE;
                    return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                  }
                  else {

                    TCHITTESTINFO tchi;
                    tchi.flags = TCHT_ONITEM;
                    GetCursorPos( &tchi.pt );
                    ScreenToClient( this->m_Hwnd, &tchi.pt );

                    int tab = TabCtrl_HitTest( this->m_Hwnd, &tchi );
                    int stab = TabCtrl_GetCurSel( this->m_Hwnd );

                    if ( tab != -1 && tab == stab ) {

                      char ret[256];
                      this->callAliasEx( ret, "%s,%d,%d", "rclick", this->getUserID( ), tab+1 );
                    }
                    bParsed = TRUE;
                  }
                }
              }
              break;

            case NM_CUSTOMDRAW:
              {
                if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              } // NM_CUSTOMDRAW
              break;

            // TreeView Related Notifications
            case TVN_GETINFOTIP:
            case TVN_ITEMEXPANDED:
            case TVN_BEGINLABELEDIT:
            case TVN_ENDLABELEDIT:
            case TVN_DELETEITEM:
              {
                if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            // Toolbar stuff
            case TBN_DELETINGBUTTON:
            case TBN_DROPDOWN:
            case TBN_GETINFOTIP:
              {
                if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            // Listview stuff
            case LVN_BEGINLABELEDIT:
            case LVN_ENDLABELEDIT:
            case LVN_DELETEITEM:
              {
                if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            case HDN_BEGINTRACK:
            case HDN_ITEMCLICK:
            case HDN_ITEMDBLCLICK:
              {
                GetClassName( GetParent( hdr->hwndFrom ), ClassName, 256 );
                if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            case RBN_ENDDRAG:
            case RBN_HEIGHTCHANGE:
            case RBN_DELETINGBAND:
              {
                if ( lstrcmp( DCX_REBARCTRLCLASS, ClassName ) == 0 ) {

                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

              /*
            case TTN_GETDISPINFO: 
              {
                mIRCError( "Tooltip requested" );

                if ( lstrcmp( DCX_TABCTRLCLASS, ClassName ) == 0) {

                  // Not a forward to this window, comes from a child window
                  if ( this->m_Hwnd != hdr->hwndFrom ) {
                    bParsed = TRUE;
                    return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                  }
                  else {
                    mIRCError( "Tooltip requested" );
                    bParsed = TRUE;
                  }
                }
              }
              break;
              */

            // Tab Stuff
            case TCN_SELCHANGE:
              {
                if ( lstrcmp( DCX_TABCTRLCLASS, ClassName ) == 0) {

                  // Not a forward to this window, comes from a child window
                  if ( this->m_Hwnd != hdr->hwndFrom ) {
                    bParsed = TRUE;
                    return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                  }
                  else {
                   
                    int tab = TabCtrl_GetCurSel( this->m_Hwnd );

                    if ( tab != -1 ) {

                      char ret[256];
                      this->callAliasEx( ret, "%s,%d,%d", "sclick", this->getUserID( ), tab+1 );
                    }
                    this->activateSelectedTab( );
                    bParsed = TRUE;
                  }
                }
              }
              break;
          }
        }
      }
      break;

    case WM_HSCROLL: 
      {
        char ClassName[256];
        if ( IsWindow( (HWND) lParam ) && GetClassName( (HWND) lParam, ClassName, 256 ) != 0 ) {

          if ( lstrcmp( DCX_TRACKBARCLASS, ClassName ) == 0 ) {
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          }
        }
      }
      break;

    case WM_VSCROLL: 
      {
        char ClassName[256];
        if ( IsWindow( (HWND) lParam ) && GetClassName( (HWND) lParam, ClassName, 256 ) != 0 ) {

          if ( lstrcmp( DCX_TRACKBARCLASS, ClassName ) == 0 ) {
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          }
        }
      }
      break;

    case WM_DELETEITEM:
      {
        //mIRCError( "Tab WM_DELETEITEM" );

        char ClassName[256];
				DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem)) && (GetClassName( idata->hwndItem, ClassName, 256 ) != 0)) {
          if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {
            //mIRCError( "DCX_COLORCOMBOCLASS WM_DELETEITEM" );
            bParsed = TRUE;
            return SendMessage( idata->hwndItem, uMsg, wParam, lParam );
          }
				}
        //char ClassName[256];
        //HWND cHwnd = GetDlgItem( this->m_Hwnd, wParam );
        //if ( IsWindow( cHwnd ) && GetClassName( cHwnd, ClassName, 256 ) != 0) {

        //  if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {

        //    //mIRCError( "DCX_COLORCOMBOCLASS WM_DELETEITEM" );
        //    bParsed = TRUE;
        //    return SendMessage( cHwnd, uMsg, wParam, lParam );
        //  }
        //}
      }
      break;

    case WM_MEASUREITEM:
      {
        //mIRCError( "Tab WM_MEASUREITEM" );
				if (ctrl_MeasureItem(this->m_Hwnd, wParam, lParam)) return TRUE;
        //char ClassName[256];
        //HWND cHwnd = GetDlgItem( this->m_Hwnd, wParam );
        //if ( IsWindow( cHwnd ) && GetClassName( cHwnd, ClassName, 256 ) != 0 ) {

        //  if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {

        //    //mIRCError( "DCX_COLORCOMBOCLASS WM_MEASUREITEM" );

        //    LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;

        //    if ( lpmis != NULL )
        //      lpmis->itemHeight = 16; 

        //    return TRUE;
        //  }
        //}
      }
      break;

    case WM_DRAWITEM:
      {
        //mIRCError( "Tab WM_DRAWITEM" );

        char ClassName[256];
				DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem)) && (GetClassName( idata->hwndItem, ClassName, 256 ) != 0)) {
          if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {
            //mIRCError( "DCX_COLORCOMBOCLASS WM_DRAWITEM" );
            bParsed = TRUE;
            return SendMessage( idata->hwndItem, uMsg, wParam, lParam );
          }
				}
        //char ClassName[256];
        //HWND cHwnd = GetDlgItem( this->m_Hwnd, wParam );
        //if ( IsWindow( cHwnd ) && GetClassName( cHwnd, ClassName, 256 ) != 0) {

        //  if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {

        //    //mIRCError( "DCX_COLORCOMBOCLASS WM_DRAWITEM" );
        //    bParsed = TRUE;
        //    return SendMessage( cHwnd, uMsg, wParam, lParam );
        //  }
        //}
      }
     break;

    case WM_COMMAND:
      {
        //mIRCError( "Tab WM_COMMAND" );
        char ClassName[256];

				if ( IsWindow( (HWND) lParam ) && GetClassName( (HWND) lParam, ClassName, 256 ) != 0 ) {

          // ComboEx Notifications
          if ( lstrcmp( DCX_COMBOEXCLASS, ClassName ) == 0 ) {
            bParsed = TRUE;
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          }
          // ColorCombo notifications
          else if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {
            bParsed = TRUE;
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          }
          // Edit notifications
					else if ( lstrcmpi( "EDIT", ClassName ) == 0 ) {
            bParsed = TRUE;
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
					}
          // List notifications
          else if ( lstrcmpi( "LISTBOX", ClassName ) == 0 ) {
            bParsed = TRUE;
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          }
        }
      }
      break;

    case WM_SIZE:
      {
        this->activateSelectedTab( );
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

		case WM_GETDLGCODE:
			{
				//mIRCError("Tab WM_GETDLGCODE");
				bParsed = TRUE;
				return 0L; //DLGC_STATIC;
			}
			break;
		//case WM_ACTIVATE:
		//	{
		//		switch (wParam)
		//		{
		//		case WA_ACTIVE:
		//			{
		//				mIRCError("Tab Activate");
		//			}
		//			break;
		//		case WA_CLICKACTIVE:
		//			{
		//				mIRCError("Tab Click Activate");
		//			}
		//			break;
		//		case WA_INACTIVE:
		//			{
		//				mIRCError("Tab De-Activate");
		//			}
		//			break;
		//		}
		//		bParsed = TRUE;
		//		return 0L;
		//	}
		//	break;

    case WM_DESTROY:
      {
        //mIRCError( "WM_DESTROY" );
        delete this;
        bParsed = TRUE;
      }
      break;

    default:
      break;
  }

  return 0L;
}