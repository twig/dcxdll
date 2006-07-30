/*!
 * \file dcxpanel.cpp
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

#include "dcxpanel.h"
#include "dcxdialog.h"

#include "dcxprogressbar.h"
#include "dcxtrackbar.h"
#include "dcxcolorcombo.h"
#include "dcxcomboex.h"
#include "dcxstatusbar.h"
#include "dcxtreeview.h"
#include "dcxtoolbar.h"
#include "dcxlistview.h"
#include "dcxbutton.h"
#include "dcxrichedit.h"
#include "dcxrebar.h"
#include "dcxipaddress.h"
#include "dcxupdown.h"
#include "dcxwebctrl.h"
#include "dcxcalendar.h"

#include "dcxdivider.h"
#include "dcxtab.h"

#include "dcxmwindow.h"
#include "dcxmdialog.h"

#include "mirc/dcxline.h"
#include "mirc/dcxbox.h"
#include "mirc/dcxradio.h"
#include "mirc/dcxcheck.h"
#include "mirc/dcxtext.h"
#include "mirc/dcxedit.h"
#include "mirc/dcxscroll.h"
#include "mirc/dcxlist.h"
#include "mirc/dcxlink.h"
#include "mirc/dcximage.h"

#include "layout/layoutcellfixed.h"
#include "layout/layoutcellfill.h"
#include "layout/layoutcellpane.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxPanel::DcxPanel( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles, 
    DCX_PANELCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle( NULL ), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  this->m_pLayoutManager = new LayoutManager( this->m_Hwnd );

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

DcxPanel::DcxPanel( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles, 
    DCX_PANELCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  this->m_pLayoutManager = new LayoutManager( this->m_Hwnd );

  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxPanel::~DcxPanel( ) {

  if ( this->m_pLayoutManager != NULL )
    delete this->m_pLayoutManager;

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPanel::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  unsigned int i = 1, numtok = styles.numtok( " " );

  *ExStyles = WS_EX_CONTROLPARENT;

  /*
  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "notheme" )
      *bNoTheme = TRUE;

    i++;
  }
  */
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

void DcxPanel::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );

  if ( this->parseGlobalInfoRequest( input, szReturnValue ) ) {

    return;
  }
  
  szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPanel::parseCommandRequest( TString & input ) {

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
          char expression[200];
          wsprintf( expression, "$window(%s).hwnd", styles.gettok( 1, " " ).to_chr( ) );
          mIRCeval( expression, windowHwnd );

          HWND winHwnd = (HWND) atoi( windowHwnd );

          if ( IsWindow( winHwnd ) && this->m_pParentDialog->getControlByHWND( winHwnd ) == NULL ) {

            p_Control = new DcxMWindow( winHwnd, this->m_Hwnd, ID, this->m_pParentDialog, &rc, styles );
          }
        }
      }
      else if ( type == "dialog" ) {

        if ( styles.numtok( " " ) > 0 ) {

          char windowHwnd[30];
          char expression[200];
          wsprintf( expression, "$dialog(%s).hwnd", styles.gettok( 1, " " ).to_chr( ) );
          mIRCeval( expression, windowHwnd );

          HWND winHwnd = (HWND) atoi( windowHwnd );

          if ( IsWindow( winHwnd ) && this->m_pParentDialog->getControlByHWND( winHwnd ) == NULL ) {

            p_Control = new DcxMDialog( winHwnd, this->m_Hwnd, ID, this->m_pParentDialog, &rc, styles );
          }
        }
      }

      if ( p_Control != NULL ) {

        this->m_pParentDialog->addControl( p_Control );

        this->redrawWindow( );
      }
    }
    else {

      char error[500];
      wsprintf( error, "/xdid -c : Control with ID \"%d\" already exists", ID - mIRC_ID_OFFSET );
      mIRCError( error );
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
      else if ( p_Control->getRefCount( ) == 0 )
        DestroyWindow( cHwnd );
      else {

        char error[500];
        wsprintf( error, "Can't delete control with ID \"%d\" when it is inside it's own event (dialog %s)", 
                  p_Control->getUserID( ), this->m_pParentDialog->getName( ).to_chr( ) );
        mIRCError( error );
      }
    }
    else {

      char error[500];
      wsprintf( error, "/ $+ xdialog -d : Unknown control with ID \"%d\" (dialog %s)", 
                ID - mIRC_ID_OFFSET, this->m_pParentDialog->getName( ).to_chr( ) );
      mIRCError( error );
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
              char error[500];
                wsprintf( error, "/xdid -l : Cell Fill -> Invalid ID : %d", ID );
                mIRCError( error );
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
                char error[500];
                wsprintf( error, "/xdid -l : Cell Fixed -> Invalid ID : %d", ID );
                mIRCError( error );
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
                char error[500];
                wsprintf( error, "/xdid -l : Cell Fixed -> Invalid ID : %d", ID );
                mIRCError( error );
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

              char error[500];
              wsprintf( error, "/xdid -l : Invalid item path: %s", path.to_chr( ) );
              mIRCError( error );
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

        if ( p_GetCell == NULL ) {

          char error[500];
          wsprintf( error, "/xdid -l : Invalid item path: %s", path.to_chr( ) );
          mIRCError( error );
          return;
        }
        else {

          RECT rc;
          SetRect( &rc, ID, WGT, W, H );
          p_GetCell->setBorder( rc );
        }
      } // else if ( com == "space" )
    } // if ( numtok > 7 )
  }
	// xdid -t [NAME] [ID] [SWITCH] [TEXT]
	else if (flags.switch_flags[19] && numtok > 3) {
		SetWindowText(this->m_Hwnd, input.gettok(4, -1, " ").to_chr());
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

UINT DcxPanel::parseLayoutFlags( TString & flags ) {

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

LRESULT DcxPanel::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	//mIRCDebug("panel: %d", uMsg);
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
				//if (hdr->hwndFrom == this->m_Hwnd)
				//	return 0L;

        char ClassName[256];

        if ( IsWindow( hdr->hwndFrom ) && GetClassName( hdr->hwndFrom, ClassName, 256 ) != 0 ) {

          //char error[500];
          //wsprintf( error, "%d == %d == %d -> %X %X", hdr->code, HDN_ITEMCLICKA, HDN_ITEMCLICKW, hdr->hwndFrom, GetParent( hdr->hwndFrom ) );
          //mIRCError( error );

          switch( hdr->code ) {

            case NM_CLICK:
              {
                //mIRCError( "Rebar WM_NOTIFY - NM_CLICK" );
                if ( lstrcmp( DCX_STATUSBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                //else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 ) {
                //  bParsed = TRUE;
                //  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                //}
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            case NM_DBLCLK:
              {
                //mIRCError( "Dialog WM_NOTIFY - NM_DBLCLK" );
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
                //mIRCError( "Toolbar Tooltips Here!-1" );
                if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  //mIRCError( "Toolbar Tooltips Here!" );
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

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

            // wierd problem here, all the other controls get this correct except this one, go figure!
            case HDN_BEGINTRACK:
            case HDN_BEGINTRACKW:
            case HDN_ITEMCLICK:
            case HDN_ITEMCLICKW:
            case HDN_ITEMDBLCLICK:
            case HDN_ITEMDBLCLICKW:
              {
                //mIRCError( "Control WM_NOTIFY - HDN_XXX" );

                GetClassName( GetParent( hdr->hwndFrom ), ClassName, 256 );
                if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {

                  //mIRCError( "Control WM_NOTIFY - HDN_XXX 2" );

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
        //mIRCError( "Rebar WM_DELETEITEM" );

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
        //mIRCError( "Panel WM_MEASUREITEM" );
				if (ctrl_MeasureItem(this->m_Hwnd, wParam, lParam)) return TRUE;
      }
      break;

    case WM_DRAWITEM:
      {
        //mIRCError( "Panel WM_DRAWITEM" );

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
        //mIRCError( "Panel WM_COMMAND" );
				//if ((HWND)lParam == this->m_Hwnd)
				//	return 0L;

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
          else if ( lstrcmp( DCX_BUTTONCLASS, ClassName ) == 0 ) {

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

    case WM_CTLCOLORDLG:
      {
        bParsed = TRUE;
        return (INT_PTR) this->getBackClrBrush( );
      }
      break;

      /*
    case WM_ERASEBKGND: 
      {

        if ( this->m_hBackBrush != NULL ) {

          bParsed = TRUE;

          HDC hdc = GetDC( this->m_Hwnd );

          HBRUSH hBrush;

          if ( this->m_hBackBrush != NULL )
            hBrush = this->m_hBackBrush;
          else
            hBrush = GetSysColorBrush( COLOR_3DFACE );

          RECT rect;
          GetWindowRect( this->m_Hwnd, &rect );
          OffsetRect( &rect, -rect.left, -rect.top );
          FillRect( hdc, &rect, hBrush );

          ReleaseDC( this->m_Hwnd, hdc );

          return TRUE;
        }
      }
      break;
      */

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

    case WM_SETCURSOR:
      {
        if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd ) {

          if ( this->m_hCursor != NULL ) {

            SetCursor( this->m_hCursor );
          }
          else {

            HCURSOR hCursor = LoadCursor( NULL, IDC_ARROW );
            SetCursor( hCursor );
          }
          bParsed = TRUE;
          return TRUE;
        }
      }
      break;

    case WM_MOUSEMOVE:
      {
        this->m_pParentDialog->setMouseControl( this->getUserID( ) );
      }
      break;

    case WM_LBUTTONDOWN:
      {
        this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID( ) );
        this->callAliasEx( NULL, "%s,%d", "lbdown", this->getUserID( ) );
      }
      break;

    case WM_LBUTTONUP:
      {
        this->callAliasEx( NULL, "%s,%d", "lbup", this->getUserID( ) );
      }
      break;

    case WM_LBUTTONDBLCLK:
      {
        this->callAliasEx( NULL, "%s,%d", "dclick", this->getUserID( ) );
        this->callAliasEx( NULL, "%s,%d", "lbdblclk", this->getUserID( ) );
      }
      break;

    case WM_SETFOCUS:
      {
        this->m_pParentDialog->setFocusControl( this->getUserID( ) );
      }
      break;

		//case WM_GETDLGCODE:
		//{
		//	//mIRCError("Panel WM_GETDLGCODE");
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