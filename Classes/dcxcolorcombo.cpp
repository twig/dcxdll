/*!
 * \file dcxcolorcombo.cpp
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

#include "dcxcolorcombo.h"
#include "dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxColorCombo::DcxColorCombo( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles,
    DCX_COLORCOMBOCLASS,
    NULL,
    WS_CHILD | WS_VISIBLE | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

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

DcxColorCombo::DcxColorCombo( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles,
    DCX_COLORCOMBOCLASS,
    NULL,
    WS_CHILD | WS_VISIBLE | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxColorCombo::~DcxColorCombo( ) {

  this->resetContent( );
  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxColorCombo::parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme) {
	*Styles |= CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED; 
	//unsigned int i = 1, numtok = styles.numtok( " " );

  /*
  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "notheme" )
      *bNoTheme = TRUE;
    
    i++;
  }
  */

	this->parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxColorCombo::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );

   // [NAME] [ID] [PROP]
  if ( input.gettok( 3, " " ) == "num" ) {

    wsprintf( szReturnValue, "%d", this->getCount( ) );
    return;
  }
   // [NAME] [ID] [PROP] [N]
  else if ( input.gettok( 3, " " ) == "color" && numtok > 3 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nItem > -1 && nItem < this->getCount( ) ) {

      LPDCXCCOMBOITEM lpdcxcci = (LPDCXCCOMBOITEM) this->getItemData( nItem );

      if ( lpdcxcci != NULL ) {

        wsprintf( szReturnValue, "%d", lpdcxcci->clrItem );
        return;
      }
    }
  }
   // [NAME] [ID] [PROP]
  else if ( input.gettok( 3, " " ) == "sel" ) {

    int nItem;
    if ( ( nItem = this->getCurSel( ) ) != CB_ERR ) {

      wsprintf( szReturnValue, "%d", nItem + 1 );
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

void DcxColorCombo::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  // xdid -a [NAME] [ID] [SWITCH] [N] [RGB]
  if ( flags.switch_flags[0] && numtok > 4 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    COLORREF clrItem = atol( input.gettok( 5, " " ).to_chr( ) );

    if ( nItem > -2 && nItem < this->getCount( ) ) {

      LPDCXCCOMBOITEM lpdcxcci = new DCXCCOMBOITEM;
      lpdcxcci->clrItem = clrItem;
      //lpmycci->itemtext = "";
      this->insertItem( nItem, (LPARAM) lpdcxcci );
    }
  }
  // xdid -c [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[2] && numtok > 3 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nItem > -1 && nItem < this->getCount( ) ) {

      this->setCurSel( nItem );
    }
  }
  // xdid -d [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[3] && numtok > 3 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nItem > -1 && nItem < this->getCount( ) ) {

      this->deleteItem( nItem );
    }
  }
  // xdid -m [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[12] ) {

    this->setmIRCPalette( );
  }
  // xdid -o [NAME] [ID] [SWITCH] [N] [RGB]
  else if ( flags.switch_flags[14] && numtok > 4 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    COLORREF clrItem = atol( input.gettok( 5, " " ).to_chr( ) );

    if ( nItem > -1 && nItem < this->getCount( ) ) {

      LPDCXCCOMBOITEM lpdcxcci = (LPDCXCCOMBOITEM) this->getItemData( nItem );

      if ( lpdcxcci != NULL )
        lpdcxcci->clrItem = clrItem;
    }
  }
  // xdid -r [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[17] ) {

    this->resetContent( );
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

void DcxColorCombo::setmIRCPalette( ) {

  char com[500];
  char colors[500];
  lstrcpy( com,"$color(0) $color(1) $color(2) $color(3) $color(4) $color(5) $color(6) $color(7) $color(8) $color(9) $color(10) $color(11) $color(12) $color(13) $color(14) $color(15)");
  mIRCeval( com, colors );

  TString cols = colors;
  LPDCXCCOMBOITEM lpdcxcci;

  int i = 1, len = cols.numtok( " " );
  while ( i <= len ) {

    lpdcxcci = new DCXCCOMBOITEM;
    lpdcxcci->clrItem = (COLORREF) atol( cols.gettok( i, " " ).to_chr( ) );
    //lpmycci->itemtext = "";
    this->insertItem( -1, (LPARAM) lpdcxcci );

    i++;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::insertItem(  int nPos, LPARAM lParam ) {
  return SendMessage( this->m_Hwnd, CB_INSERTSTRING, (WPARAM) nPos, lParam );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::getCount( ) {
  return SendMessage( this->m_Hwnd, CB_GETCOUNT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::setCurSel( int nPos ) {
  return SendMessage( this->m_Hwnd, CB_SETCURSEL, (WPARAM) nPos, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::getItemData( int nItem ) {
  return SendMessage( this->m_Hwnd, CB_GETITEMDATA, (WPARAM) nItem, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::getCurSel( ) {
  return SendMessage( this->m_Hwnd, CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::deleteItem( int nItem ) {
  return SendMessage( this->m_Hwnd, CB_DELETESTRING, (WPARAM) nItem, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::resetContent( ) {
  return SendMessage( this->m_Hwnd, CB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0 );
}


/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxColorCombo::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
  switch( uMsg ) {
    case WM_COMMAND:
      {
        switch ( HIWORD( wParam ) ) {

          case CBN_SELENDOK:
            {
              this->callAliasEx( NULL, "%s,%d,%d", "sclick", this->getUserID( ), this->getCurSel( ) + 1 );
              bParsed = TRUE;
              return 0L;
            }
            break;
        }
      }
      break;
	}
	return 0L;
}

LRESULT DcxColorCombo::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_DELETEITEM:
      {
        //mIRCError( "Control WM_DELETEITEM" );

        PDELETEITEMSTRUCT delis = (PDELETEITEMSTRUCT) lParam;
        LPDCXCCOMBOITEM lpdcxcci = (LPDCXCCOMBOITEM) delis->itemData;

        if ( lpdcxcci != NULL )
          delete lpdcxcci;

        bParsed = TRUE;
        return TRUE;
      }
      break;

    case WM_DRAWITEM:
      {
        //mIRCError( "Control WM_DRAWITEM" );

        LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;

        if ( lpdis != NULL && lpdis->itemID != -1 ) {

          LPDCXCCOMBOITEM lpdcxcci = (LPDCXCCOMBOITEM) lpdis->itemData;

          if ( lpdcxcci != NULL ) {

            HPEN hPen;

            /*
            if ( lpdis->itemState & ODS_SELECTED ) {
              hPen = CreatePen( PS_SOLID, 2, RGB(0,0,0) );
              mIRCError( "ODS_SELECTED" );
            }
            else
              */
            hPen = CreatePen( PS_SOLID, 1, RGB(0,0,0) );

            HPEN oldPen = (HPEN) SelectObject( lpdis->hDC, hPen );

            RECT rcItem = lpdis->rcItem;
            InflateRect( &rcItem, -4, -2 );

            SetBkColor( lpdis->hDC, lpdcxcci->clrItem );

            ExtTextOut( lpdis->hDC, rcItem.left, rcItem.top, ETO_CLIPPED | ETO_OPAQUE, &rcItem, "", NULL, NULL );

            MoveToEx( lpdis->hDC, rcItem.left, rcItem.top, NULL );
            LineTo( lpdis->hDC, rcItem.right, rcItem.top );

            MoveToEx( lpdis->hDC, rcItem.right, rcItem.top, NULL );
            LineTo( lpdis->hDC, rcItem.right, rcItem.bottom );

            MoveToEx( lpdis->hDC, rcItem.right, rcItem.bottom, NULL );
            LineTo( lpdis->hDC, rcItem.left, rcItem.bottom );

            MoveToEx( lpdis->hDC, rcItem.left, rcItem.bottom, NULL );
            LineTo( lpdis->hDC, rcItem.left, rcItem.top );

            SelectObject( lpdis->hDC, oldPen );
            DeleteObject( hPen );
          }
        }
        bParsed = TRUE;
        return TRUE;
      }
      break;

    case WM_MOUSEMOVE:
      {
        this->m_pParentDialog->setMouseControl( this->getUserID( ) );
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

    case WM_SETFOCUS:
      {
        this->m_pParentDialog->setFocusControl( this->getUserID( ) );
      }
      break;

    default:
      break;
  }

  return 0L;
}