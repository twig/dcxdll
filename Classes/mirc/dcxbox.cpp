/*!
 * \file dcxbox.cpp
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

#include "dcxbox.h"
#include "../dcxdialog.h"

#include "../dcxprogressbar.h"
#include "../dcxtrackbar.h"
#include "../dcxcolorcombo.h"
#include "../dcxcomboex.h"
#include "../dcxstatusbar.h"
#include "../dcxtreeview.h"
#include "../dcxtoolbar.h"
#include "../dcxlistview.h"
#include "../dcxbutton.h"
#include "../dcxrichedit.h"
#include "../dcxrebar.h"
#include "../dcxipaddress.h"
#include "../dcxupdown.h"
#include "../dcxwebctrl.h"
#include "../dcxcalendar.h"

#include "../dcxpanel.h"
#include "../dcxdivider.h"
#include "../dcxtab.h"

#include "../dcxmwindow.h"
#include "../dcxmdialog.h"

#include "dcxline.h"
#include "dcxradio.h"
#include "dcxcheck.h"
#include "dcxtext.h"
#include "dcxedit.h"
#include "dcxscroll.h"
#include "dcxlist.h"
#include "dcxlink.h"
#include "dcximage.h"

#include "../layout/layoutcellfixed.h"
#include "../layout/layoutcellfill.h"
#include "../layout/layoutcellpane.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxBox::DcxBox( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
	  ExStyles, 
    DCX_BOXCLASS, //"BUTTON", 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle( NULL ), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  this->m_pLayoutManager = new LayoutManager( this->m_Hwnd );

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

DcxBox::DcxBox( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles, 
    DCX_BOXCLASS, //"BUTTON", 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  this->m_pLayoutManager = new LayoutManager( this->m_Hwnd );

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxBox::~DcxBox( ) {

  if ( this->m_pLayoutManager != NULL )
    delete this->m_pLayoutManager;

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxBox::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  unsigned int i = 1, numtok = styles.numtok( " " );
  this->m_iBoxStyles = 0;
  
	//*ExStyles = WS_EX_CONTROLPARENT; removed by Ook

  while ( i <= numtok ) {

    if (styles.gettok(i , " ") == "right")
      this->m_iBoxStyles |= BOXS_RIGHT;
    else if (styles.gettok(i , " ") == "center")
      this->m_iBoxStyles |= BOXS_CENTER;
    else if (styles.gettok(i , " ") == "bottom")
      this->m_iBoxStyles |= BOXS_BOTTOM;
	 else if (styles.gettok(i , " ") == "none")
      this->m_iBoxStyles |= BOXS_NONE;
	 else if (styles.gettok(i , " ") == "rounded")
      this->m_iBoxStyles |= BOXS_ROUNDED;
		else if (styles.gettok(i , " ") == "transparent")
			*ExStyles |= WS_EX_TRANSPARENT;

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

void DcxBox::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( " " );

  // [NAME] [ID] [PROP]
  if ( input.gettok( 3, " " ) == "text" ) {

    GetWindowText( this->m_Hwnd, szReturnValue, 900 );
    return;
  }
  else if ( input.gettok( 3, " " ) == "inbox" ) {

    RECT rc, rcText;
    GetClientRect( this->m_Hwnd, &rc );

    int n = GetWindowTextLength( this->m_Hwnd );

    InflateRect( &rc, -2, -2 );
    if ( n > 0 ) {

      HDC hdc = GetDC( this->m_Hwnd );
      HFONT oldFont;

      if (this->m_hFont != NULL)
        oldFont = (HFONT) SelectObject(hdc, this->m_hFont);

      char * text = new char[n+2];
      GetWindowText( this->m_Hwnd, text, n+1 );
      DrawText( hdc, text, n, &rcText, DT_CALCRECT );

		if (this->m_hFont != NULL)
			SelectObject(hdc, oldFont);

      ReleaseDC( this->m_Hwnd, hdc );

      //int w = rcText.right - rcText.left;
      int h = rcText.bottom - rcText.top;

      if ( this->m_iBoxStyles & BOXS_BOTTOM ) {
        rc.bottom = rc.bottom - h + 2;
      }
      else {
        rc.top = rc.top + h - 2;
      }
    }

    wsprintf( szReturnValue, "%d %d %d %d", rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top );
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

void DcxBox::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  // xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
  if ( flags.switch_flags[2] && numtok > 8 ) {

    UINT ID = mIRC_ID_OFFSET + atoi( input.gettok( 4, " " ).to_chr( ) );

    if ( ID > mIRC_ID_OFFSET - 1 && 
      !IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
      this->m_pParentDialog->getControlByID( ID ) == NULL ) 
    {

      TString type = input.gettok( 5, " " );
      RECT rc;

      rc.left = atol( input.gettok( 6, " " ).to_chr( ) );
      rc.top = atol( input.gettok( 7, " " ).to_chr( ) );
      rc.right = rc.left + atol( input.gettok( 8, " " ).to_chr( ) );
      rc.bottom = rc.top + atol( input.gettok( 9, " " ).to_chr( ) );

      DcxControl * p_Control = NULL;

      TString styles = "";
      if ( numtok > 9 ) {
        styles = input.gettok( 10, -1, " " );
      }

      if ( type == "trackbar" ) {
        p_Control = new DcxTrackBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "pbar" ) {
        p_Control = new DcxProgressBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "comboex" ) {
        p_Control = new DcxComboEx( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "toolbar" ) {
        p_Control = new DcxToolBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "statusbar" ) {
        p_Control = new DcxStatusBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "treeview" ) {
        p_Control = new DcxTreeView( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "listview" ) {
        p_Control = new DcxListView( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "colorcombo" ) {
        p_Control = new DcxColorCombo( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "button" ) {
        p_Control = new DcxButton( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "richedit" ) {
        p_Control = new DcxRichEdit( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "rebar" ) {
        p_Control = new DcxReBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
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
      else if ( type == "ipaddress" ) {
        p_Control = new DcxIpAddress( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "updown" ) {
        p_Control = new DcxUpDown( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "webctrl" ) {
        p_Control = new DcxWebControl( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "calendar" ) {
        p_Control = new DcxCalendar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "line" ) {
        p_Control = new DcxLine( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "box" ) {
        p_Control = new DcxBox( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "radio" ) {
        p_Control = new DcxRadio( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "check" ) {
        p_Control = new DcxCheck( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "text" ) {
        p_Control = new DcxText( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "edit" ) {
        p_Control = new DcxEdit( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "scroll" ) {
        p_Control = new DcxScroll( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "list" ) {
        p_Control = new DcxList( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "link" ) {
        p_Control = new DcxLink( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "image" ) {
        p_Control = new DcxImage( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
      }
      else if ( type == "window" ) {

        if ( styles.numtok( " " ) > 0 ) {

          char windowHwnd[30];
          TString expression;
          expression.sprintf("$window(%s).hwnd", styles.gettok( 1, " " ).to_chr( ) );
					mIRCeval( expression.to_chr(), windowHwnd );

          HWND winHwnd = (HWND) atoi( windowHwnd );

          if ( IsWindow( winHwnd ) && this->m_pParentDialog->getControlByHWND( winHwnd ) == NULL ) {

            p_Control = new DcxMWindow( winHwnd, this->m_Hwnd, ID, this->m_pParentDialog, &rc, styles );
          }
        }
      }
      else if ( type == "dialog" ) {

        if ( styles.numtok( " " ) > 0 ) {

          char windowHwnd[30];
          TString expression;
          expression.sprintf("$dialog(%s).hwnd", styles.gettok( 1, " " ).to_chr( ) );
					mIRCeval( expression.to_chr(), windowHwnd );

          HWND winHwnd = (HWND) atoi( windowHwnd );

          if ( IsWindow( winHwnd ) && this->m_pParentDialog->getControlByHWND( winHwnd ) == NULL ) {

            p_Control = new DcxMDialog( winHwnd, this->m_Hwnd, ID, this->m_pParentDialog, &rc, styles );
          }
        }
      }

      if ( p_Control != NULL ) {

        this->m_pParentDialog->addControl( p_Control );
				if (!this->isExStyle(WS_EX_CONTROLPARENT)) {
					if (p_Control->isStyle(WS_TABSTOP)) this->addExStyle(WS_EX_CONTROLPARENT);
				}

        this->redrawWindow( );
      }
    }
    else {
      TString error;
      error.sprintf("/xdid -c : Control with ID \"%d\" already exists", ID - mIRC_ID_OFFSET );
			mIRCError( error.to_chr() );
    }
  }
  // xdid -d [NAME] [ID] [SWITCH] [ID]
  else if ( flags.switch_flags[3] && numtok > 3 ) {

    UINT ID = mIRC_ID_OFFSET + atoi( input.gettok( 4, " " ).to_chr( ) );
    DcxControl * p_Control;
    
    if ( IsWindow( GetDlgItem( this->m_Hwnd, ID ) ) && 
      ID > mIRC_ID_OFFSET - 1 && ( p_Control = this->m_pParentDialog->getControlByID( ID ) ) != NULL ) 
    {

      HWND cHwnd = p_Control->getHwnd( );
      if ( p_Control->getType( ) == "dialog" || p_Control->getType( ) == "window" )
        delete p_Control;
			else if ( p_Control->getRefCount( ) == 0 ) {
				this->m_pParentDialog->deleteControl( p_Control ); // remove from internal list!
        DestroyWindow( cHwnd );
				if (GetWindow(this->m_Hwnd,GW_CHILD) == NULL) { // if no children remove style
					if (this->isExStyle(WS_EX_CONTROLPARENT)) {
						this->removeExStyle(WS_EX_CONTROLPARENT);
					}
				}
			}
      else {

        TString error;
        error.sprintf("Can't delete control with ID \"%d\" when it is inside it's own event (dialog %s)", 
                  p_Control->getUserID( ), this->m_pParentDialog->getName( ).to_chr( ) );
				mIRCError( error.to_chr() );
      }
    }
    else {

      TString error;
      error.sprintf("/ $+ xdialog -d : Unknown control with ID \"%d\" (dialog %s)", 
                ID - mIRC_ID_OFFSET, this->m_pParentDialog->getName( ).to_chr( ) );
			mIRCError( error.to_chr() );
    }
  }
  /*
  //xdid -l [NAME] [ID] [SWITCH] [OPTIONS]

  [OPTIONS] :

  root [TAB]+flpiw [ID] [WEIGHT] [W] [H]
  add PATH[TAB]+flpiw [ID] [WEIGHT] [W] [H]
  space PATH[TAB]+ [L] [T] [R] [B]
  */
  else if ( flags.switch_flags[11] && numtok > 3 ) {

    if ( input.gettok( 4, " " ) == "update" ) {

      if ( this->m_pLayoutManager != NULL ) {

        RECT rc;
        GetClientRect( this->m_Hwnd, &rc );
        this->m_pLayoutManager->updateLayout( rc );
      }
    }
    else if ( numtok > 8 ) {

      TString com = input.gettok( 1, "\t" ).gettok( 4, " " );
      com.trim( );
      TString path = input.gettok( 1, "\t" ).gettok( 5, -1, " " );
      path.trim( );
      TString p2 = input.gettok( 2, "\t" );
      p2.trim( );

      UINT flags = this->parseLayoutFlags( p2.gettok( 1, " " ) );
      UINT ID = atoi( p2.gettok( 2, " " ).to_chr( ) );
      UINT WGT = atoi( p2.gettok( 3, " " ).to_chr( ) );
      UINT W = atoi( p2.gettok( 4, " " ).to_chr( ) );
      UINT H = atoi( p2.gettok( 5, " " ).to_chr( ) );

      if ( com ==  "root" || com == "cell" ) {

        HWND cHwnd = GetDlgItem( this->m_Hwnd, mIRC_ID_OFFSET + ID );

        LayoutCell * p_Cell = NULL;

        // LayoutCellPane
        if ( flags & LAYOUTPANE ) {

          if ( flags & LAYOUTHORZ )
            p_Cell = new LayoutCellPane( LayoutCellPane::HORZ );
          else
            p_Cell = new LayoutCellPane( LayoutCellPane::VERT );
        } // if ( flags & LAYOUTPANE )
        // LayoutFill Cell
        else if ( flags & LAYOUTFILL ) {

          //mIRCError( "LayoutFill" );

          if ( flags & LAYOUTID ) {

            //mIRCError( "LayoutFillID" );

            if ( cHwnd != NULL && IsWindow( cHwnd ) )
              p_Cell = new LayoutCellFill( cHwnd );
            else {
							TString error;
              error.sprintf("/xdid -l : Cell Fill -> Invalid ID : %d", ID );
							mIRCError( error.to_chr() );
              return;
            }
          }
          else {
            p_Cell = new LayoutCellFill( );
          }
        } // else if ( flags & LAYOUTFILL )
        // LayoutCellFixed
        else if ( flags & LAYOUTFIXED ) {

          LayoutCellFixed::FixedType type;

          if ( flags & LAYOUTVERT && flags & LAYOUTHORZ )
            type = LayoutCellFixed::BOTH;
          else if ( flags & LAYOUTVERT )
            type = LayoutCellFixed::HEIGHT;
          else
            type = LayoutCellFixed::WIDTH;

          // Defined Rectangle
          if ( flags & LAYOUTDIM ) {

            RECT rc;
            SetRect( &rc, 0, 0, W, H );

            if ( flags & LAYOUTID ) {

              if ( cHwnd != NULL && IsWindow( cHwnd ) )
                p_Cell = new LayoutCellFixed( cHwnd, rc, type );
              else {
                TString error;
                error.sprintf("/xdid -l : Cell Fixed -> Invalid ID : %d", ID );
								mIRCError( error.to_chr() );
                return;
              }
            }
            else
              p_Cell = new LayoutCellFixed( rc, type );

          }
          // No defined Rectangle
          else {

            if ( flags & LAYOUTID ) {

              if ( cHwnd != NULL && IsWindow( cHwnd ) )
                p_Cell = new LayoutCellFixed( cHwnd, type );
              else {
                TString error;
                error.sprintf("/xdid -l : Cell Fixed -> Invalid ID : %d", ID );
								mIRCError( error.to_chr() );
                return;
              }
            }
          } //else
        } // else if ( flags & LAYOUTFIXED )
        else {
          mIRCError( "/xdid -l : Unknown Cell Type" );
          return;
        }

        if ( com == "root" ) {

          if ( p_Cell != NULL )
            this->m_pLayoutManager->setRoot( p_Cell );
  
        } // if ( com == "root" )
        else if ( com == "cell" ) {

          if ( p_Cell != NULL ) {

            LayoutCell * p_GetCell;

            if ( path == "root" )
              p_GetCell = this->m_pLayoutManager->getRoot( );
            else
              p_GetCell = this->m_pLayoutManager->getCell( path );

            if ( p_GetCell == NULL ) {
              TString error;
              error.sprintf("/xdid -l : Invalid item path: %s", path.to_chr( ) );
							mIRCError( error.to_chr() );
              return;
            }
            
            if ( p_GetCell->getType( ) == LayoutCell::PANE ) {

              LayoutCellPane * p_PaneCell = (LayoutCellPane *) p_GetCell;
              p_PaneCell->addChild( p_Cell, WGT );
            }
          }
        } // else if ( com == "cell" )
      } // if ( com ==  "root" || com == "cell" )
      else if ( com ==  "space" ) {

        LayoutCell * p_GetCell;

        if ( path == "root" )
          p_GetCell = this->m_pLayoutManager->getRoot( );
        else
          p_GetCell = this->m_pLayoutManager->getCell( path );

        if ( p_GetCell != NULL ) {
          
          RECT rc;
          SetRect( &rc, ID, WGT, W, H );
          p_GetCell->setBorder( rc );
        }
      } // else if ( com == "space" )
    } // if ( numtok > 7 )
  }
  //xdid -t [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[19] ) {

    TString text = input.gettok( 4, -1, " " );
    text.trim( );
    SetWindowText( this->m_Hwnd, text.to_chr( ) );
    this->redrawWindow( );
  }
  else
    this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxBox::parseLayoutFlags( TString & flags ) {

  INT i = 1, len = flags.len( );
  UINT iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if ( flags[i] == 'f' )
      iFlags |= LAYOUTFIXED;
    else if ( flags[i] == 'h' )
      iFlags |= LAYOUTHORZ;
    else if ( flags[i] == 'i' )
      iFlags |= LAYOUTID;
    else if ( flags[i] == 'l' )
      iFlags |= LAYOUTFILL ;
    else if ( flags[i] == 'p' )
      iFlags |= LAYOUTPANE;
    else if ( flags[i] == 'v' )
      iFlags |= LAYOUTVERT;
    else if ( flags[i] == 'w' )
      iFlags |= LAYOUTDIM;

    ++i;
  }

  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxBox::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_NOTIFY : 
      {

        LPNMHDR hdr = (LPNMHDR) lParam;

        if (!hdr)
          break;

        char ClassName[256];

        if ( IsWindow( hdr->hwndFrom ) && GetClassName( hdr->hwndFrom, ClassName, 256 ) != 0 ) {

          switch( hdr->code ) {
            case NM_CLICK:
              {
                //mIRCError( "Box WM_NOTIFY - NM_CLICK" );
                if ( lstrcmp( DCX_STATUSBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                /*
                else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                */
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            case NM_DBLCLK:
              {
                //mIRCError( "Box WM_NOTIFY - NM_DBLCLK" );
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
                //mIRCError( "Box WM_NOTIFY - NM_RCLICK" );
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
                else if ( lstrcmp( DCX_TABCTRLCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
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

            case LVN_BEGINLABELEDIT:
            case LVN_ENDLABELEDIT:
            case LVN_DELETEITEM:
						case LVN_GETINFOTIP:
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
                  return SendMessage( GetParent( hdr->hwndFrom ), uMsg, wParam, lParam );
                }
              }
              break;

            case TCN_SELCHANGE:
              {
                if ( lstrcmp( DCX_TABCTRLCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
              }
              break;

            case IPN_FIELDCHANGED:
              {
                if ( lstrcmp( DCX_IPADDRESSCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
              }
              break;

            case UDN_DELTAPOS:
              {
                if ( lstrcmp( DCX_UPDOWNCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
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
          else if ( lstrcmpi( "SCROLLBAR", ClassName ) == 0 ) {
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
          else if ( lstrcmpi( "SCROLLBAR", ClassName ) == 0 ) {

            SendMessage( (HWND) lParam, uMsg, wParam, lParam );
            return DefWindowProc( this->m_Hwnd, uMsg, wParam, lParam );
          }
        }
      }
      break;

    case WM_DELETEITEM:
      {
        //mIRCError( "Box WM_DELETEITEM" );

        char ClassName[256];
				DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem)) && (GetClassName( idata->hwndItem, ClassName, 256 ) != 0)) {
          if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {
            //mIRCError( "DCX_COLORCOMBOCLASS WM_DELETEITEM" );
            bParsed = TRUE;
            return SendMessage( idata->hwndItem, uMsg, wParam, lParam );
          }
				}
      }
      break;

    case WM_MEASUREITEM:
      {
        //mIRCError( "Box WM_MEASUREITEM" );

				if (ctrl_MeasureItem(this->m_Hwnd, wParam, lParam)) return TRUE;
      }
      break;

    case WM_DRAWITEM:
      {
        //mIRCError( "Box WM_DRAWITEM" );

        char ClassName[256];
				DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem)) && (GetClassName( idata->hwndItem, ClassName, 256 ) != 0)) {
          if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {
            //mIRCError( "DCX_COLORCOMBOCLASS WM_DRAWITEM" );
            bParsed = TRUE;
            return SendMessage( idata->hwndItem, uMsg, wParam, lParam );
          }
				}
      }
     break;

    case WM_COMMAND:
      {

        // If it's this control
        if ( (HWND) lParam == this->m_Hwnd ) {

          switch ( HIWORD( wParam ) ) {

            case BN_CLICKED:
              {
                //mIRCError( "Control BN_CLICKED" );
                this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID( ) );
              }
              break;
          }
        }
        else {
          //mIRCError( "Rebar WM_COMMAND" );
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
            // Button notifications
            if ( lstrcmp( DCX_BUTTONCLASS, ClassName ) == 0 ) {

              bParsed = TRUE;
              return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
            }
            // Box/Radio/Check notifications
            else if ( lstrcmpi( "BUTTON", ClassName ) == 0 ) {

              bParsed = TRUE;
              return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
            }
            // Text notifications
            else if ( lstrcmpi( "STATIC", ClassName ) == 0 ) {

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
            // Toolbar notifications
            else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 ) {

              bParsed = TRUE;
              return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
            }
            // IP Address notifications
            /*
            else if ( lstrcmp( DCX_IPADDRESSCLASS, ClassName ) == 0 ) {

            bParsed = TRUE;
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
            }
            */
          }
        }
      }
      break;

    case WM_SIZE:
      {

        HWND bars = NULL;

        while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_REBARCTRLCLASS, NULL ) ) != NULL ) {

          SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
        }

        while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_STATUSBARCLASS, NULL ) ) != NULL ) {

          SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
        }

        while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_TOOLBARCLASS, NULL ) ) != NULL ) {

          SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
        }

        this->callAliasEx( NULL, "%s,%d", "sizing", this->getUserID( ) );

        RECT rc;
        SetRect( &rc, 0, 0, LOWORD( lParam ), HIWORD( lParam ) );
        this->m_pLayoutManager->updateLayout( rc );
        this->redrawWindow( );
      }
      break;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
      {

        DcxControl * p_Control = this->m_pParentDialog->getControlByHWND( (HWND) lParam );

        if ( p_Control != NULL ) {

          COLORREF clrText = p_Control->getTextColor( );
          COLORREF clrBackText = p_Control->getBackColor( );
          HBRUSH hBackBrush = p_Control->getBackClrBrush( );

          if ( clrText != -1 )
            SetTextColor( (HDC) wParam, clrText );
 
          if ( clrBackText != -1 )
            SetBkColor( (HDC) wParam, clrBackText );

          if ( hBackBrush != NULL ) {

            bParsed = TRUE;
            return (LRESULT) hBackBrush;
          }

        }
      }
      break;

		case WM_ENABLE: {
			this->redrawWindow();
			break;
		}


		case WM_ERASEBKGND:
		{
			if (this->isExStyle(WS_EX_TRANSPARENT)) {
				bParsed = TRUE;
				return TRUE;
			}
		}

		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(this->m_Hwnd, &ps);
				RECT rc, rc2, rcText, rcText2;
				int n = GetWindowTextLength(this->m_Hwnd);
				HBRUSH hBrush;

				GetClientRect(this->m_Hwnd, &rc);
				CopyRect(&rc2, &rc);

				if (!this->isExStyle(WS_EX_TRANSPARENT)) {
					// set up brush colors
					if (this->m_hBackBrush != NULL)
						hBrush = this->m_hBackBrush;
					else
						hBrush = GetSysColorBrush(COLOR_3DFACE);

					// paint the background
					FillRect(hdc, &rc2, hBrush);
				}
				else {
					hBrush = GetSysColorBrush(COLOR_3DFACE);
				}

				// if no border, dont bother
				if (this->m_iBoxStyles & BOXS_BOTTOM) {
					EndPaint(this->m_Hwnd, &ps);

					bParsed = TRUE;
					return 0L;
				}

				SetBkMode(hdc, TRANSPARENT);

				// no text, no box!
				if (!n) {
					if (this->m_iBoxStyles & BOXS_ROUNDED) {
						HBRUSH OldhBrush = (HBRUSH) SelectObject(hdc,hBrush);
						RoundRect(hdc, rc2.left, rc2.top, rc2.right, rc2.bottom, 10, 10);
						SelectObject(hdc,OldhBrush);
					}
					else {
						//if (!this->isExStyle(WS_EX_TRANSPARENT)) 
						//	FillRect(hdc, &rc2, hBrush);
						DrawEdge(hdc, &rc2, EDGE_RAISED, BF_TOPLEFT | BF_BOTTOMRIGHT);
					}
				}
				// draw text
				else {
					// prepare for appearance
					if (this->m_hFont != NULL)
						SelectObject(hdc, this->m_hFont);

					if (this->m_clrText != -1)
						SetTextColor(hdc, this->m_clrText);
					else
						SetTextColor(hdc, GetSysColor(
							IsWindowEnabled(this->m_Hwnd) ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT)
						);

					char *text = new char[n +2];
					GetWindowText(this->m_Hwnd, text, n +1);

					DrawText(hdc, text, n, &rcText, DT_CALCRECT);
					int w = rcText.right - rcText.left;
					int h = rcText.bottom - rcText.top;

					// shift border and text locations
					// text at bottom?
					if (this->m_iBoxStyles & BOXS_BOTTOM) {
						rcText.top = rc2.bottom - h;
						rc2.bottom -= h/2;
					}
					// text at top
					else {
						rcText.top = rc2.top;
						rc2.top += h/2;
					}

					// text location
					rcText.bottom = rcText.top + h;

					// align text horizontally
					int bw = rc.right - rc.left - (2 * DCX_BOXTEXTSPACING);

					if (w > bw) {
						rcText.left = rc.left + DCX_BOXTEXTSPACING;
						rcText.right = rc.right - DCX_BOXTEXTSPACING;
					}
					else {
						if (this->m_iBoxStyles & BOXS_RIGHT)
							rcText.left = rc.right - DCX_BOXTEXTSPACING - w;
						else if (this->m_iBoxStyles & BOXS_CENTER)
							rcText.left = (rc.left + rc.right - w) /2;
						else
							rcText.left = rc.left + DCX_BOXTEXTSPACING;

						rcText.right = rcText.left + w;
					}


					// clear some space for the text
					CopyRect(&rcText2, &rcText);
					InflateRect(&rcText2, 3, 0);

					//if (this->isExStyle(WS_EX_TRANSPARENT))
					//	ExcludeClipRect(hdc, rcText2.left, rcText2.top, rcText2.right, rcText2.bottom);

					// draw the border
					if (this->m_iBoxStyles & BOXS_ROUNDED) {
						//DrawRoundRect(hdc,&rc2,10,10);
						HBRUSH OldhBrush = (HBRUSH) SelectObject(hdc,hBrush);
						RoundRect(hdc, rc2.left, rc2.top, rc2.right, rc2.bottom, 10, 10);
						SelectObject(hdc,OldhBrush);
					}
					else {
						//if (!this->isExStyle(WS_EX_TRANSPARENT)) 
						//	FillRect(hdc, &rc2, hBrush);
						DrawEdge(hdc, &rc2, EDGE_ETCHED, BF_RECT);
					}

					if (!this->isExStyle(WS_EX_TRANSPARENT))
						FillRect(hdc, &rcText2, hBrush);
					//else
					//	IntersectClipRect(hdc, rcText2.left, rcText2.top, rcText2.right, rcText2.bottom);

					// draw the text
					DrawText(hdc, text, n, &rcText, DT_LEFT | DT_END_ELLIPSIS);

					delete [] text;
				}

				EndPaint(this->m_Hwnd, &ps);

				bParsed = TRUE;
				return 0L;
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

		case WM_LBUTTONDOWN:
		{
			this->callAliasEx(NULL, "%s,%d", "lbdown", this->getUserID());
			this->callAliasEx(NULL, "%s,%d", "sclick", this->getUserID());
			break;
		}

		case WM_LBUTTONUP:
		{
			this->callAliasEx(NULL, "%s,%d", "lbup", this->getUserID());
			break;
		}

		case WM_LBUTTONDBLCLK:
		{
			this->callAliasEx(NULL, "%s,%d", "dclick", this->getUserID());
			break;
		}

		case WM_RBUTTONDOWN:
		{
			this->callAliasEx(NULL, "%s,%d", "rclick", this->getUserID());
			break;
		}
		//case WM_GETDLGCODE:
		//{
		//	//mIRCError("Box WM_GETDLGCODE");
  //    bParsed = TRUE;
		//	return 0L; //DLGC_STATIC;
		//}
		//break;

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