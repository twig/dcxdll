/*!
 * \file dcxdialog.cpp
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

#include "../defines.h"
#include "dcxdialog.h"
#include "dcxdialogcollection.h"
#include "dcxcontrol.h"

#include "dcxtoolbar.h"
#include "dcxrichedit.h"

#include "layout/layoutcellfixed.h"
#include "layout/layoutcellfill.h"
#include "layout/layoutcellpane.h"

extern DcxDialogCollection Dialogs;

extern mIRCDLL mIRCLink;

/*!
 * \brief Constructor
 *
 * \param mHwnd Dialog Window Handle
 * \param tsName Dialog Name
 * \param tsAliasName Dialog Callback alias Name
 */

DcxDialog::DcxDialog( HWND mHwnd, TString & tsName, TString & tsAliasName )
: DcxWindow( mHwnd, 0 )
, m_uStyleBg(DBS_BKGNORMAL)
{
  this->m_tsName = tsName;
  this->m_tsAliasName = tsAliasName;
  this->m_hBackBrush = NULL;
  this->m_bitmapBg = NULL;
  this->m_colTransparentBg = RGB(255,0,255);

  this->m_hCursor = NULL;
  this->m_bCursorFromFile = FALSE;

  this->addStyle( WS_CLIPCHILDREN );

  this->m_hOldWindowProc = (WNDPROC) SetWindowLong( this->m_Hwnd, GWL_WNDPROC, (LONG) DcxDialog::WindowProc );

  this->m_pLayoutManager = new LayoutManager( this->m_Hwnd );

  this->m_MouseID = 0;
  this->m_FocusID = 0;

  SetProp( this->m_Hwnd, "dcx_this", (HANDLE) this );

  DragAcceptFiles(this->m_Hwnd, TRUE);
}

/*!
 * \brief Destructor
 *
 * blah
 */

DcxDialog::~DcxDialog() {
	if (this->m_pLayoutManager != NULL)
		delete this->m_pLayoutManager;

	if (this->m_bitmapBg)
		DeleteObject(m_bitmapBg);

	RemoveProp(this->m_Hwnd, "dcx_this");
}

/*!
 * \brief blah
 *
 * blah
 */
TString DcxDialog::getName( ) {

  return this->m_tsName;
}

/*!
 * \brief blah
 *
 * blah
 */

TString DcxDialog::getAliasName( ) {

  return this->m_tsAliasName;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::addControl( DcxControl * p_Control ) {

  this->m_vpControls.push_back( p_Control );
  p_Control->redrawWindow( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::deleteControl( DcxControl * p_Control ) {
  VectorOfControlPtrs::iterator itStart = this->m_vpControls.begin( );
  VectorOfControlPtrs::iterator itEnd = this->m_vpControls.end( );

  while ( itStart != itEnd ) {
    if ( *itStart == p_Control && *itStart != NULL ) {
      this->m_vpControls.erase( itStart );
      return;
    }

    itStart++;
  }
}

void DcxDialog::deleteAllControls() {
  /*
  char error[500];
  wsprintf( error, "%d Controls", this->m_vpControls.size( ) );
  mIRCError( error );

  VectorOfControlPtrs::iterator it;
  DcxControl * p_Control;

  
  while ( ( p_Control = this->m_vpControls.pop_back( ) ) != NULL ) {

    wsprintf( error, "ID %d %d", p_Control->getUserID( ), this->m_vpControls.size( ) );
    mIRCError( error );

    if ( p_Control != NULL ) {

      if ( p_Control->getType( ) == "window" || p_Control->getType( ) == "dialog" )
        delete p_Control;
      else
        DestroyWindow( p_Control->getHwnd( ) );
    }
  }
  this->m_vpControls.clear( );
  

  wsprintf( error, "%d Controls", this->m_vpControls.size( ) );
  mIRCError( error );
  */
}

/*!
 * \brief blah
 *
 * blah
 */

DcxControl * DcxDialog::getControlByID( UINT ID ) {
  VectorOfControlPtrs::iterator itStart = this->m_vpControls.begin( );
  VectorOfControlPtrs::iterator itEnd = this->m_vpControls.end( );

  while ( itStart != itEnd ) {
    if ( (*itStart)->getID( ) == ID )
      return *itStart;

    itStart++;
  }

  return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

DcxControl * DcxDialog::getControlByHWND( HWND mHwnd ) {
  VectorOfControlPtrs::iterator itStart = this->m_vpControls.begin( );
  VectorOfControlPtrs::iterator itEnd = this->m_vpControls.end( );

  while ( itStart != itEnd ) {
    if ( (*itStart)->getHwnd( ) == mHwnd )
      return *itStart;

    itStart++;
  }

  return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::parseCommandRequest(TString & input) {
  XSwitchFlags flags;

  ZeroMemory((void*)&flags, sizeof(XSwitchFlags));
  this->parseSwitchFlags( &input.gettok(2, " "), &flags);

  int numtok = input.numtok( " " );

  // xdialog -a [NAME] [SWITCH] [+FLAGS] [DURATION]
  if (flags.switch_flags[0] && numtok > 3) {
    AnimateWindow( this->m_Hwnd, 
                   atoi( input.gettok( 4, " " ).to_chr( ) ), 
                   getAnimateStyles( input.gettok( 3, " " ) ) );

    if ( IsWindowVisible( this->m_Hwnd ) )
      RedrawWindow( this->m_Hwnd, NULL, NULL, 
                    RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE );

  }
  // xdialog -b [NAME] [SWITCH] [+FLAGS]
  else if ( flags.switch_flags[1] && numtok > 2 ) {

    this->removeStyle( WS_BORDER|WS_DLGFRAME|WS_MAXIMIZEBOX|WS_MINIMIZEBOX|
      WS_SYSMENU|WS_SIZEBOX|WS_CAPTION );

    this->removeExStyle( WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME|WS_EX_CONTEXTHELP|
      WS_EX_TOOLWINDOW|WS_EX_STATICEDGE|WS_EX_WINDOWEDGE );

    LONG Styles = 0, ExStyles = 0;
    this->parseBorderStyles( input.gettok( 3, " " ), &Styles, &ExStyles );

    this->addStyle( Styles );
    this->addExStyle( ExStyles );

    SetWindowPos( this->m_Hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );
    InvalidateRect( this->m_Hwnd, NULL, TRUE );
    SendMessage( this->m_Hwnd, WM_NCPAINT, (WPARAM) 1, (LPARAM) 0 );
  }
  // xdialog -c [NAME] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
  else if ( flags.switch_flags[2] && numtok > 7 ) {

    UINT ID = mIRC_ID_OFFSET + atoi( input.gettok( 3, " " ).to_chr( ) );

    if ( IsWindow( GetDlgItem( this->m_Hwnd, ID ) ) == FALSE && 
      ID > mIRC_ID_OFFSET - 1 && this->getControlByID( ID ) == NULL )
    {

      DcxControl * p_Control = DcxControl::controlFactory( this, ID, input );
      if ( p_Control != NULL )
        this->addControl( p_Control );

    }
    else {

      char error[500];
      wsprintf( error, "/xdialog -c : Control with ID \"%d\" already exists", ID - mIRC_ID_OFFSET );
      mIRCError( error );
    }
  }
	// xdialog -d [NAME] [SWITCH] [ID]
	else if (flags.switch_flags[3] && numtok > 2) {
		/*
		if ( input.gettok( 3, " " ) == "*" ) { 

		this->deleteAllControls( );
		}
		else {
		*/

		UINT ID = mIRC_ID_OFFSET + atoi(input.gettok(3, " ").to_chr());
		DcxControl * p_Control;

		if (IsWindow(GetDlgItem(this->m_Hwnd, ID)) && 
			(ID > mIRC_ID_OFFSET - 1) && (p_Control = this->getControlByID(ID)) != NULL)
		{
			HWND cHwnd = p_Control->getHwnd();
			UINT cid = p_Control->getUserID();

			// fix up focus id
			if (cid == this->m_FocusID) {
				HWND h = GetNextDlgTabItem(this->m_Hwnd, cHwnd, FALSE);

				// control exists and is not the last one
				if (h && (h != cHwnd))
					this->setFocusControl(GetDlgCtrlID(h) - mIRC_ID_OFFSET);
				else
					this->setFocusControl(0);
			}

			if ((p_Control->getType() == "dialog") || (p_Control->getType() == "window"))
				delete p_Control;
			else if (p_Control->getRefCount() == 0)
				DestroyWindow(cHwnd);
			else
				mIRCDebug("Can't delete control with ID \"%d\" when it is inside it's own event (dialog %s)", p_Control->getUserID(), this->m_tsName.to_chr());
		}
		// unknown control
		else
			mIRCDebug("/ $+ xdialog -d : Unknown control with ID \"%d\" (dialog %s)", ID - mIRC_ID_OFFSET, this->m_tsName.to_chr());
	}
  // xdid -f [NAME] [SWITCH] [+FLAGS] [COUNT] [TIMEOUT]
  else if ( flags.switch_flags[5] && numtok > 4 ) {
    UINT iFlags = this->parseFlashFlags( input.gettok( 3, " " ) );
    INT iCount = atoi( input.gettok( 4, " " ).to_chr( ) );
    DWORD dwTimeout = atol( input.gettok( 5, " " ).to_chr( ) );

    FLASHWINFO fli;
    ZeroMemory( &fli, sizeof( FLASHWINFO ) );
    fli.cbSize = sizeof( FLASHWINFO );
    fli.dwFlags = iFlags;
    fli.hwnd = this->m_Hwnd;
    fli.uCount = iCount;
    fli.dwTimeout = dwTimeout;

    FlashWindowEx( & fli );
  }
  // xdid -g [NAME] [SWITCH] [+FLAGS] [COLOR|FILENAME]
	else if (flags.switch_flags[6] && numtok > 3) {
		this->m_uStyleBg = this->parseBkgFlags(input.gettok(3, " "));

		if (this->m_uStyleBg & DBS_BKGCOLOR) {
			COLORREF clrColor = atol(input.gettok(4, " ").to_chr());

			if (this->m_hBackBrush != NULL) {
				DeleteObject(this->m_hBackBrush);
				this->m_hBackBrush = NULL;
			}

			if (clrColor != -1)
				this->m_hBackBrush = CreateSolidBrush(clrColor);
		}
		else if (this->m_uStyleBg & DBS_BKGBITMAP) {
			if (this->m_bitmapBg) {
				DeleteObject(this->m_bitmapBg);
				this->m_bitmapBg = NULL;
			}

			TString filename = input.gettok(4, -1, " ");
			filename.trim();

			if (filename != "none")
				this->m_bitmapBg = LoadBitmap(this->m_bitmapBg, filename);
		}

		InvalidateRect(this->m_Hwnd, NULL, TRUE);
	}
  // xdid -j [NAME] [SWITCH]
  else if ( flags.switch_flags[9] ) {

    this->redrawWindow( );
  }
  /*
  //xdialog -l [NAME] [SWITCH] [OPTIONS]

  [OPTIONS] :

  root [TAB]+flpiw [ID] [WEIGHT] [W] [H]
  add PATH[TAB]+flpiw [ID] [WEIGHT] [W] [H]
  space PATH[TAB]+ [L] [T] [R] [B]
  */
  else if ( flags.switch_flags[11] && numtok > 2 ) {

    if ( input.gettok( 3, " " ) == "update" ) {

      if ( this->m_pLayoutManager != NULL ) {

        RECT rc;
        GetClientRect( this->m_Hwnd, &rc );
        this->m_pLayoutManager->updateLayout( rc );
      }
    }
    else if ( numtok > 7 ) {

      TString com = input.gettok( 1, "\t" ).gettok( 3, " " );
      com.trim( );
      TString path = input.gettok( 1, "\t" ).gettok( 4, -1, " " );
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
                wsprintf( error, "/xdialog -l : Cell Fill -> Invalid ID : %d", ID );
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
                wsprintf( error, "/xdialog -l : Cell Fixed -> Invalid ID : %d", ID );
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
                wsprintf( error, "/xdialog -l : Cell Fixed -> Invalid ID : %d", ID );
                mIRCError( error );
                return;
              }
            }
          } //else
        } // else if ( flags & LAYOUTFIXED )
        else {
          mIRCError( "/xdialog -l : Unknown Cell Type" );
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
              wsprintf( error, "/xdialog -l : Invalid item path: %s", path.to_chr( ) );
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

        if ( p_GetCell != NULL ) {
          
          RECT rc;
          SetRect( &rc, ID, WGT, W, H );
          p_GetCell->setBorder( rc );
        }
      } // else if ( com == "space" )
    } // if ( numtok > 7 )
  }
  // xdialog -q [NAME] [SWITCH] [+FLAGS] [CURSOR|FILENAME]
  else if ( flags.switch_flags[16] && numtok > 3 ) {

    if ( this->m_bCursorFromFile ) {
      DeleteObject( this->m_hCursor );
      this->m_hCursor = NULL;
      this->m_bCursorFromFile = FALSE;
    }
    else
      this->m_hCursor = NULL;

    UINT iFlags = this->parseCursorFlags( input.gettok( 3, " " ) );

    if ( iFlags & DCCS_FROMRESSOURCE )
      this->m_hCursor = LoadCursor( NULL, this->parseCursorType( input.gettok( 4, " " ) ) );
    else if ( iFlags & DCCS_FROMFILE ) {

      this->m_hCursor = LoadCursorFromFile( input.gettok( 4, -1, " " ).to_chr( ) );
      this->m_bCursorFromFile = TRUE;
    }
  }
  // xdialog -x [NAME] [SWITCH]
  else if ( flags.switch_flags[23] ) {

    DestroyWindow( this->m_Hwnd );
  }

  // xdialog -h [NAME] [SWITCH]
  if ( flags.switch_flags[7] ) {

    ShowWindow( this->m_Hwnd, SW_HIDE );
  }
  // xdialog -m [NAME] [SWITCH]
  if ( flags.switch_flags[12] ) {

    ShowWindow( this->m_Hwnd, SW_MAXIMIZE );
  }
  // xdialog -n [NAME] [SWITCH]
  if ( flags.switch_flags[13] ) {

    ShowWindow( this->m_Hwnd, SW_MINIMIZE );
  }
  // xdialog -r [NAME] [SWITCH]
  if ( flags.switch_flags[17] ) {

    ShowWindow( this->m_Hwnd, SW_RESTORE );
  }
  // xdialog -s [NAME] [SWITCH]
  if ( flags.switch_flags[18] ) {

    ShowWindow( this->m_Hwnd, SW_SHOW );
  }
  // xdialog -t [NAME] [SWITCH] [COLOR]
	if (flags.switch_flags[19] && numtok > 2) {
		this->m_colTransparentBg = atoi(input.gettok(3, " ").to_chr());
		this->redrawWindow();
	}
  /*
  else {
    char error[500];
    wsprintf( error, "Invalid /xdialog switch : %s : or number of arguments", input.gettok( 2, " " ).to_chr( ) );
    mIRCError( error );
  }
  */
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::parseBorderStyles( TString & flags, LONG * Styles, LONG * ExStyles ) {
  INT i = 1, len = flags.len( );

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return;

  while ( i < len ) {
    if ( flags[i] == 'b' )
      *Styles |= WS_BORDER;
    else if ( flags[i] == 'c' )
      *ExStyles |= WS_EX_CLIENTEDGE;
    else if ( flags[i] == 'd' )
      *Styles |= WS_DLGFRAME ;
    else if ( flags[i] == 'f' )
      *ExStyles |= WS_EX_DLGMODALFRAME;
    else if ( flags[i] == 'h' )
      *ExStyles |= WS_EX_CONTEXTHELP;
    else if ( flags[i] == 'm' )
      *Styles |= WS_MAXIMIZEBOX;
    else if ( flags[i] == 'n' )
      *Styles |= WS_MINIMIZEBOX;
    else if ( flags[i] == 'o' )
      *ExStyles |= WS_EX_TOOLWINDOW;
    else if ( flags[i] == 's' )
      *ExStyles |= WS_EX_STATICEDGE;
    else if ( flags[i] == 't' )
      *Styles |= WS_CAPTION;
    else if ( flags[i] == 'w' )
      *ExStyles |= WS_EX_WINDOWEDGE;
    else if ( flags[i] == 'y' )
      *Styles |= WS_SYSMENU;
    else if ( flags[i] == 'z' )
      *Styles |= WS_SIZEBOX;

    ++i;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

DWORD DcxDialog::getAnimateStyles( TString & flags ) {
  DWORD Styles = 0;

  int i = 1, len = flags.len( );
  while ( i < len ) {
    if ( flags[i] == 's' )
      Styles |= AW_SLIDE;
    else if ( flags[i] == 'h' )
      Styles |= AW_HIDE;
    else if ( flags[i] == 'a' )
      Styles |= AW_ACTIVATE;
    else if ( flags[i] == 'b' )
      Styles |= AW_BLEND;
    else if ( flags[i] == 'v' )
      Styles |= AW_VER_POSITIVE;
    else if ( flags[i] == 'u' )
      Styles |= AW_VER_NEGATIVE;
    else if ( flags[i] == 'c' )
      Styles |= AW_CENTER;
    else if ( flags[i] == 'o' )
      Styles |= AW_HOR_POSITIVE;
    else if ( flags[i] == 'n' )
      Styles |= AW_HOR_NEGATIVE;

    i++;
  }

  return Styles;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxDialog::parseLayoutFlags( TString & flags ) {
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

UINT DcxDialog::parseBkgFlags(TString & flags) {
	INT i = 1, len = flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+') 
		return iFlags;

	while (i < len) {
		if (flags[i] == 'b')
			iFlags |= DBS_BKGCOLOR;
		else if (flags[i] == 'i')
			iFlags |= DBS_BKGBITMAP;

		else if (flags[i] == 'n')
			iFlags |= DBS_BKGNORMAL;
		else if (flags[i] == 't')
			iFlags |= DBS_BKGTILE;
		else if (flags[i] == 's')
			iFlags |= DBS_BKGSTRETCH;

		else if (flags[i] == 'c')
			iFlags |= DBS_BKGCENTER;
		else if (flags[i] == 'v')
			iFlags |= DBS_BKGVCENTER;
		else if (flags[i] == 'r')
			iFlags |= DBS_BKGRIGHT;
		else if (flags[i] == 'o')
			iFlags |= DBS_BKGBOTTOM;

		i++;
	}

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxDialog::parseFlashFlags( TString & flags ) {
	INT i = 1, len = flags.len( ), iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {
    if ( flags[i] == 'a' )
      iFlags |= FLASHW_ALL;
    else if ( flags[i] == 'c' )
      iFlags |= FLASHW_CAPTION;
    else if ( flags[i] == 'f' )
      iFlags |= FLASHW_TIMERNOFG;
    else if ( flags[i] == 'r' )
      iFlags |= FLASHW_TRAY;
    else if ( flags[i] == 's' )
      iFlags |= FLASHW_STOP;
    else if ( flags[i] == 't' )
      iFlags |= FLASHW_TIMER;

    ++i;
  }

  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxDialog::parseCursorFlags( TString & flags ) {
  INT i = 1, len = flags.len( );
  UINT iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {
    if ( flags[i] == 'f' )
      iFlags |= DCCS_FROMFILE;
    else if ( flags[i] == 'r' )
      iFlags |= DCCS_FROMRESSOURCE;

    ++i;
  }

  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

LPSTR DcxDialog::parseCursorType( TString & cursor ) {
  if ( cursor == "appstarting" )
    return IDC_APPSTARTING;
  else if ( cursor == "arrow" )
    return IDC_ARROW;
  else if ( cursor == "cross" )
    return IDC_CROSS;
  else if ( cursor == "hand" )
    return IDC_HAND;
  else if ( cursor == "help" )
    return IDC_HELP;
  else if ( cursor == "ibeam" )
    return IDC_IBEAM;
  else if ( cursor == "no" )
    return IDC_NO;
  else if ( cursor == "sizeall" )
    return IDC_SIZEALL;
  else if ( cursor == "sizenesw" )
    return IDC_SIZENESW;
  else if ( cursor == "sizens" )
    return IDC_SIZENS;
  else if ( cursor == "sizenwse" )
    return IDC_SIZENWSE;
  else if ( cursor == "sizewe" )
    return IDC_SIZEWE;
  else if ( cursor == "uparrow" )
    return IDC_UPARROW;
  else if ( cursor == "wait" )
    return IDC_WAIT;

  return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::parseInfoRequest( TString & input, char * szReturnValue ) {
  int numtok = input.numtok( " " );

  // [NAME] [PROP] [ID]
  if ( input.gettok( 2, " " ) == "isid" && numtok > 2 ) {
    int nID = atoi( input.gettok( 3, " " ).to_chr( ) );

    if ( IsWindow( GetDlgItem( this->m_Hwnd, nID + mIRC_ID_OFFSET ) ) || 
      this->getControlByID( nID + mIRC_ID_OFFSET ) != NULL ) 
    {
      lstrcpy( szReturnValue, "$true" );
    }
    else
      lstrcpy( szReturnValue, "$false" );

    return;
  }
  // [NAME] [PROP]
  else if ( input.gettok( 2, " " ) == "nextid" ) {

    int nID = 6001;
    while ( IsWindow( GetDlgItem( this->m_Hwnd, nID ) ) || 
      this->getControlByID( nID ) != NULL ) 
    {
      nID++;
    }

    wsprintf( szReturnValue, "%d", nID - mIRC_ID_OFFSET );
    return;
  }
  // [NAME] [PROP] [N]
  if ( input.gettok( 2, " " ) == "id" && numtok > 2 ) {

    int N = atoi( input.gettok( 3, " " ).to_chr( ) ) - 1;

    if ( N == -1 ) {

      wsprintf( szReturnValue, "%d", this->m_vpControls.size( ) );
    }
    else if ( N > -1 && N < (int) this->m_vpControls.size( ) ) {

      wsprintf( szReturnValue, "%d", this->m_vpControls[N]->getUserID( ) );
    }

    return;
  }
  // [NAME] [PROP]
  else if ( input.gettok( 2, " " ) == "ismenu" ) {

    if ( GetMenu( this->m_Hwnd ) != NULL )
      lstrcpy( szReturnValue, "$true" );
    else
      lstrcpy( szReturnValue, "$false" );

    return;
  }
  else if ( input.gettok( 2, " " ) == "ismarked" ) {
	if (Dialogs.getDialogByHandle( this->m_Hwnd ) != NULL) {
      lstrcpy( szReturnValue, "$true" );
	}
    else
      lstrcpy( szReturnValue, "$false" );
    return;
  }
  // [NAME] [PROP]
  else if ( input.gettok( 2, " " ) == "mouseid" ) {

    wsprintf( szReturnValue, "%d", this->m_MouseID );
    return;
  }
  // [NAME] [PROP]
  else if ( input.gettok( 2, " " ) == "focusid" ) {

    wsprintf( szReturnValue, "%d", this->m_FocusID );
    return;
  }
  // [NAME] [PROP]
  else if ( input.gettok( 2, " " ) == "mouse" ) {

    POINT pt;
    GetCursorPos( &pt );
    ScreenToClient( this->m_Hwnd, &pt );
    wsprintf( szReturnValue, "%d %d", pt.x, pt.y );
    return;
  }
  // [NAME] [PROP]
  else if ( input.gettok( 2, " " ) == "key" ) {

    UINT iKeyState = 0;
    if ( GetAsyncKeyState( VK_LBUTTON ) < 0 )
      iKeyState |= 1;
    if ( GetAsyncKeyState( VK_RBUTTON ) < 0 )
      iKeyState |= 2;
    if ( GetAsyncKeyState( VK_MBUTTON ) < 0 )
      iKeyState |= 4;
    if ( GetAsyncKeyState( VK_LSHIFT ) < 0 )
      iKeyState |= 8;
    if ( GetAsyncKeyState( VK_LCONTROL ) < 0 )
      iKeyState |= 16;
    if ( GetAsyncKeyState( VK_LMENU ) < 0 )
      iKeyState |= 32;
    if ( GetAsyncKeyState( VK_RSHIFT ) < 0 )
      iKeyState |= 64;
    if ( GetAsyncKeyState( VK_RCONTROL ) < 0 )
      iKeyState |= 128;
    if ( GetAsyncKeyState( VK_RMENU ) < 0 )
      iKeyState |= 256;
    if ( GetAsyncKeyState( VK_LEFT ) < 0 )
      iKeyState |= 512;
    if ( GetAsyncKeyState( VK_UP ) < 0 )
      iKeyState |= 1024;
    if ( GetAsyncKeyState( VK_RIGHT ) < 0 )
      iKeyState |= 2048;
    if ( GetAsyncKeyState( VK_DOWN ) < 0 )
      iKeyState |= 4096;
    if ( GetAsyncKeyState( VK_CAPITAL ) < 0 )
      iKeyState |= 8192;

    wsprintf( szReturnValue, "%d", iKeyState );
    return;
  }
  else {
	  //char error[500];
	  //wsprintf( error, "Invalid $ $+ xdialog property : %s : or number of arguments", input.gettok( 2, " " ).to_chr( ) );
	  //mIRCError( error );
		//TString error;
		//error.sprintf("Invalid $ $+ xdialog property : %s : or number of arguments", input.gettok( 2, " " ).to_chr( ) );
		//mIRCError( error.to_chr() );
		mIRCDebug("Invalid $xdialog property : %s : or number of arguments", input.gettok(2, " ").to_chr());
  }

  szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxDialog::callAliasEx( char * szReturn, const char * szFormat, ... ) {

  va_list args;
  va_start( args, szFormat );
  char parms[2048];
  vsprintf( parms, szFormat, args );
  wsprintf( mIRCLink.m_pData, "$%s(%s,%s)", 
    this->getAliasName( ).to_chr( ), 
    this->getName( ).to_chr( ),
    parms );

	SendMessage( mIRCLink.m_mIRCHWND, WM_USER + 201, 0, mIRCLink.m_map_cnt );
  //MessageBox(NULL, mData, NULL, MB_OK);
  //Signal(mData);
  if ( szReturn )
    lstrcpy( szReturn, mIRCLink.m_pData );
  //MessageBox(NULL, szReturn, NULL, MB_OK);
  va_end( args );
  if ( !lstrcmp( mIRCLink.m_pData, "$false" ) )
    return FALSE;

  return TRUE;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::updateLayout( RECT & rc ) {

  if ( this->m_pLayoutManager != NULL )
    this->m_pLayoutManager->updateLayout( rc );
}

/*!
 * \brief blah
 *
 * blah
 */

HBRUSH DcxDialog::getBackClrBrush( ) {

  return this->m_hBackBrush;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::setMouseControl( UINT mUID ) {
  if ( mUID != this->m_MouseID ) {
    this->callAliasEx( NULL, "%s,%d", "mouseleave", this->m_MouseID );
    this->callAliasEx( NULL, "%s,%d", "mouseenter", mUID );
    this->m_MouseID = mUID;
  }
  else {
    this->callAliasEx( NULL, "%s,%d", "mouse", mUID );
  }
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialog::setFocusControl( UINT mUID ) {
  if ( mUID != this->m_FocusID ) {
    this->callAliasEx( NULL, "%s,%d", "focusout", this->m_FocusID );
    this->callAliasEx( NULL, "%s,%d", "focus", mUID );
    this->m_FocusID = mUID;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT WINAPI DcxDialog::WindowProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	DcxDialog *p_this = (DcxDialog *) GetProp(mHwnd, "dcx_this");

	// sanity check for prop existing.
	if (p_this == NULL)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	bool fBlocked = (InSendMessageEx(NULL) & (ISMEX_REPLIED|ISMEX_SEND)) == ISMEX_SEND;

	// If Message is blocking just call old win proc
	if (fBlocked) {
		return CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
	}

	switch( uMsg ) {
		case WM_THEMECHANGED:
		{
			char ret[256];
			p_this->callAliasEx(ret, "%s,%d", "themechanged", 0);
			break;
		}
    case WM_NOTIFY : 
      {
        //mIRCError( "Dialog WM_NOTIFY" );
        LPNMHDR hdr = (LPNMHDR) lParam;

        if (!hdr)
          break;

        char ClassName[256];

        if ( IsWindow( hdr->hwndFrom ) && GetClassName( hdr->hwndFrom, ClassName, 256 ) != 0 ) {

          switch( hdr->code ) {
            case NM_CLICK:
              {
                //mIRCError( "Dialog WM_NOTIFY - NM_CLICK" );
                if ( lstrcmp( DCX_STATUSBARCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                else if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                //else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 )
                  //return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
              }
              break;

            case NM_DBLCLK:
              {
                //mIRCError( "Dialog WM_NOTIFY - NM_DBLCLK" );
                if ( lstrcmp( DCX_STATUSBARCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                else if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
              }
              break;

            case NM_RCLICK:
              {
                //mIRCError( "Dialog WM_NOTIFY - NM_RCLICK" );
                if ( lstrcmp( DCX_STATUSBARCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                else if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                else if ( lstrcmp( DCX_TABCTRLCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
              }
              break;

            case NM_CUSTOMDRAW:
              {
                if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                else if ( lstrcmp( DCX_REBARCTRLCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
								else if (lstrcmp(DCX_TRACKBARCLASS, ClassName) == 0)
									return SendMessage(hdr->hwndFrom, uMsg, wParam, lParam);
              }
              break;

              // TreeView Related Notifications
            case TVN_GETINFOTIP:
            case TVN_ITEMEXPANDED:
            case TVN_BEGINLABELEDIT:
            case TVN_ENDLABELEDIT:
            case TVN_DELETEITEM:
              {
                if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
              }
              break;

            case TBN_DELETINGBUTTON:
            case TBN_DROPDOWN:
            case TBN_GETINFOTIP:
              {
                if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
              }
              break;

            case LVN_BEGINLABELEDIT:
            case LVN_ENDLABELEDIT:
            case LVN_DELETEITEM:
            case LVN_BEGINDRAG:
				case LVN_ENDSCROLL:
              {
                if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
              }
              break;

            case HDN_BEGINTRACK:
            case HDN_ITEMCLICK:
            case HDN_ITEMDBLCLICK:
              {
                GetClassName( GetParent( hdr->hwndFrom ), ClassName, 256 );
                if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 )
                  return SendMessage( GetParent( hdr->hwndFrom ), uMsg, wParam, lParam );
              }
              break;

            case RBN_ENDDRAG:
            case RBN_HEIGHTCHANGE:
            case RBN_DELETINGBAND:
              {
                if ( lstrcmp( DCX_REBARCTRLCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
              }
              break;

              /*
            case TTN_GETDISPINFO: 
              {

                char data[500];
                wsprintf( data, "TTN_GETDISPINFO %x %d", hdr->hwndFrom, hdr->idFrom );
                mIRCError( data );

                TOOLINFO ti;
                ZeroMemory( &ti, sizeof( TOOLINFO ) );
                ti.cbSize = sizeof( TOOLINFO );
                ti.uFlags = TTF_IDISHWND;
                ti.hwnd = p_this->getHwnd( );

                if ( SendMessage( hdr->hwndFrom, TTM_GETTOOLINFO, (WPARAM) 0, (LPARAM) &ti ) )
                  mIRCError( "Got Infotip" );

                GetClassName( GetParent( (HWND) hdr->idFrom ), ClassName, 256 );
                if ( lstrcmp( DCX_TABCTRLCLASS, ClassName ) == 0 ) {

                  return SendMessage( (HWND) hdr->idFrom, uMsg, wParam, lParam );
                }
              }
              break;
              */

            case TCN_SELCHANGE:
              {
                if ( lstrcmp( DCX_TABCTRLCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
              }
              break;
            case IPN_FIELDCHANGED:
              {
                //mIRCError( "Tab WM_NOTIFY - IPN_FIELDCHANGED" );
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

				// richedit notifications
				case EN_SELCHANGE:
				case EN_LINK:
				{
					if (lstrcmp(DCX_RICHEDITCLASS, ClassName) == 0)
						return SendMessage(hdr->hwndFrom, uMsg, wParam, lParam);

					break;
				}

				case MCN_GETDAYSTATE:
				case MCN_SELCHANGE:
				case MCN_SELECT:
				case NM_RELEASEDCAPTURE:
				{
					if (lstrcmp(DCX_CALENDARCLASS, ClassName) == 0)
						return SendMessage(hdr->hwndFrom, uMsg, wParam, lParam);

					break;
				}
          } // switch

        } // if classname
      }
      break;

    case WM_HSCROLL: 
      {
        char ClassName[256];
        if ( IsWindow( (HWND) lParam ) && GetClassName( (HWND) lParam, ClassName, 256 ) != 0 ) {

          if ( lstrcmp( DCX_TRACKBARCLASS, ClassName ) == 0 )
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          else if ( lstrcmpi( "SCROLLBAR", ClassName ) == 0 )
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
        }
      }
      break;

    case WM_VSCROLL: 
      {
        char ClassName[256];
        if ( IsWindow( (HWND) lParam ) && GetClassName( (HWND) lParam, ClassName, 256 ) != 0 ) {

          if ( lstrcmp( DCX_TRACKBARCLASS, ClassName ) == 0 )
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          else if ( lstrcmpi( "SCROLLBAR", ClassName ) == 0 )
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
        }
      }
      break;

		case WM_DELETEITEM:
		{
			//mIRCError( "Dialog WM_DELETEITEM" );
			char ClassName[256];
			DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;

			if ((idata != NULL) && (IsWindow(idata->hwndItem)) && (GetClassName( idata->hwndItem, ClassName, 256 ) != 0)) {
				if (lstrcmp(DCX_COLORCOMBOCLASS, ClassName) == 0) {
					//mIRCError( "DCX_COLORCOMBOCLASS WM_DELETEITEM" );
					return SendMessage( idata->hwndItem, uMsg, wParam, lParam );
				}
			}
		}
		break;

    case WM_MEASUREITEM:
      {
        //mIRCError( "Dialog WM_MEASUREITEM" );
				if (ctrl_MeasureItem(mHwnd, wParam, lParam)) return TRUE;
      }
      break;

		case WM_DRAWITEM:
		{
			//mIRCError( "Dialog WM_DRAWITEM" );
			char ClassName[256];
			DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;

			if ((idata != NULL) && (IsWindow(idata->hwndItem)) && (GetClassName( idata->hwndItem, ClassName, 256 ) != 0)) {
				if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {
					//mIRCError( "DCX_COLORCOMBOCLASS WM_DRAWITEM" );
					return SendMessage( idata->hwndItem, uMsg, wParam, lParam );
				}
			}
		}
		break;

    case WM_COMMAND:
      {
        //mIRCError( "Dialog WM_COMMAND" );
        char ClassName[256];

        if ( HIWORD( wParam ) == 0 && LOWORD( wParam ) == 2 && lParam == NULL ) {
          char ret[256];
          p_this->callAliasEx( ret, "%s,%d", "close", 0 );

          if ( lstrcmp( "noclose", ret ) == 0 )
            return 0L;
        }
        else if ( IsWindow( (HWND) lParam ) && GetClassName( (HWND) lParam, ClassName, 256 ) != 0 ) {

          // ComboEx Notifications
          if ( lstrcmp( DCX_COMBOEXCLASS, ClassName ) == 0 )
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          // ColorCombo notifications
          else if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 )
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          // Button notifications
          else if ( lstrcmp( DCX_BUTTONCLASS, ClassName ) == 0 )
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          // Box/Radio/Check notifications
          else if ( lstrcmpi( "BUTTON", ClassName ) == 0 && p_this->getControlByHWND( (HWND) lParam ) != NULL )
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          // Text/Image notifications
          else if ( lstrcmpi( "STATIC", ClassName ) == 0 && p_this->getControlByHWND( (HWND) lParam ) != NULL )
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          // Edit notifications
          else if ( lstrcmpi( "EDIT", ClassName ) == 0 && p_this->getControlByHWND( (HWND) lParam ) != NULL )
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          // List notifications
          else if ( lstrcmpi( "LISTBOX", ClassName ) == 0 && p_this->getControlByHWND( (HWND) lParam ) != NULL )
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          // Toolbar notifications
          else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 )
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          // IP Address notifications
          //else if ( lstrcmp( DCX_IPADDRESSCLASS, ClassName ) == 0 )
            //return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
        }
      }
      break;

    case WM_SYSCOMMAND:
      {

        switch( wParam & 0xFFF0 ) {

          case SC_MOVE:
            {
              if ( p_this != NULL ) {

                char ret[256];
                p_this->callAliasEx( ret, "%s,%d", "beginmove", 0 );

                if ( lstrcmp( "nomove", ret ) == 0 )
                  return 0L;

                p_this->m_bInMoving = true;
                
                return DefWindowProc( mHwnd, uMsg, wParam, lParam );
              }
            }
            break;

          case SC_CLOSE:
            {
              if ( p_this != NULL ) {

                char ret[256];
                p_this->callAliasEx( ret, "%s,%d", "close", 0 );

                if ( lstrcmp( "noclose", ret ) == 0 )
                  return 0L;
              }
            }
            break;

          case SC_MINIMIZE:
            {
              if ( p_this != NULL ) {

                char ret[256];
                p_this->callAliasEx( ret, "%s,%d", "min", 0 );

                if ( lstrcmp( "stop", ret ) == 0 )
                  return 0L;
                
                return DefWindowProc( mHwnd, uMsg, wParam, lParam );
              }
            }
            break;

          case SC_MAXIMIZE:
            {
              if ( p_this != NULL ) {

                char ret[256];
                p_this->callAliasEx( ret, "%s,%d", "max", 0 );

                if ( lstrcmp( "stop", ret ) == 0 )
                  return 0L;
                
                return DefWindowProc( mHwnd, uMsg, wParam, lParam );
              }
            }
            break;

          case SC_RESTORE:
            {
              if ( p_this != NULL ) {

                p_this->callAliasEx( NULL, "%s,%d", "restore", 0 );
                
                return DefWindowProc( mHwnd, uMsg, wParam, lParam );
              }
            }
            break;

          case SC_SIZE:
            {

              if ( p_this != NULL ) {

                char ret[256];
                p_this->callAliasEx( ret, "%s,%d", "beginsize", 0 );

                if ( lstrcmp( "nosize", ret ) == 0 )
                  return 0L;

                p_this->m_bInSizing = true;

                return DefWindowProc( mHwnd, uMsg, wParam, lParam );
              }
            }
            break;

        } // switch
      }
      break;

    case WM_EXITSIZEMOVE:
      {

        if ( p_this != NULL ) {

          if ( p_this->m_bInSizing )
            p_this->callAliasEx( NULL, "%s,%d", "endsize", 0 );
          else if ( p_this->m_bInMoving )
            p_this->callAliasEx( NULL, "%s,%d", "endmove", 0 );

          p_this->m_bInMoving = false;
          p_this->m_bInSizing = false;
        }
      }
      break;

    case WM_MOVING:
      {

        if ( p_this != NULL ) {
          p_this->callAliasEx( NULL, "%s,%d", "moving", 0 );
        }
      }
      break;

    case WM_SIZE:
      {
        HWND bars = NULL;

        while ( ( bars = FindWindowEx( mHwnd, bars, DCX_REBARCTRLCLASS, NULL ) ) != NULL ) {

          SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
        }

        while ( ( bars = FindWindowEx( mHwnd, bars, DCX_STATUSBARCLASS, NULL ) ) != NULL ) {

          SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
        }

        while ( ( bars = FindWindowEx( mHwnd, bars, DCX_TOOLBARCLASS, NULL ) ) != NULL ) {

          DcxToolBar * t = (DcxToolBar *) p_this->getControlByHWND( bars );
          t->autoPosition( LOWORD( lParam ), HIWORD( lParam ) );
          //SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) lParam );
        }
        
        if ( p_this != NULL ) {

          char ret[256];
          p_this->callAliasEx( ret, "%s,%d,%d,%d", "sizing", 0, LOWORD( lParam ), HIWORD( lParam ) );

          RECT rc;
          SetRect( &rc, 0, 0, LOWORD( lParam ), HIWORD( lParam ) );
          p_this->updateLayout( rc );
					p_this->redrawWindow();

          if ( lstrcmp( "nosize", ret ) == 0 )
            return 0L;
          
        }
      }
      break;

		case WM_ERASEBKGND:
		{
			if (mHwnd != p_this->getHwnd())
				break;

			HDC hdc = (HDC) wParam;
			RECT rwnd;

			GetClientRect(p_this->getHwnd(), &rwnd);

			// background color
			if (p_this->getBackClrBrush())
				FillRect(hdc, &rwnd, p_this->getBackClrBrush());
			else
				FillRect(hdc, &rwnd, GetSysColorBrush(COLOR_3DFACE));

			// draw bitmap
			if (p_this->m_bitmapBg) {
				HDC hdcbmp = CreateCompatibleDC(hdc);
				BITMAP bmp;

				GetObject(p_this->m_bitmapBg, sizeof(BITMAP), &bmp);
				SelectObject(hdcbmp, p_this->m_bitmapBg);

				int x = 0;
				int y = 0;
				int w = rwnd.right - rwnd.left;
				int h = rwnd.bottom - rwnd.top;

				// stretch
				if (p_this->m_uStyleBg & DBS_BKGSTRETCH) {
					//BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcbmp, 0, 0, SRCCOPY);
					TransparentBlt(hdc, x, y, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, p_this->m_colTransparentBg);
				}
				// tile
				else if (p_this->m_uStyleBg & DBS_BKGTILE) {
					for (y = 0; y < h; y += bmp.bmHeight) {
						for (x = 0; x < w; x += bmp.bmWidth) {
							//BitBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hdcbmp, 0, 0, SRCCOPY);
							TransparentBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, p_this->m_colTransparentBg);
						}
					}
				}
				// normal
				//else if (p_this->m_uStyleBg & DBS_BKGNORMAL) {
				else {
					// align horizontally
					if (p_this->m_uStyleBg & DBS_BKGCENTER)
						x = (w - bmp.bmWidth) / 2;
					else if (p_this->m_uStyleBg & DBS_BKGRIGHT)
						x = w - bmp.bmWidth;

					// align vertically
					if (p_this->m_uStyleBg & DBS_BKGVCENTER)
						y = (h - bmp.bmHeight) / 2;
					else if (p_this->m_uStyleBg & DBS_BKGBOTTOM)
						y = h - bmp.bmHeight;

					TransparentBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, p_this->m_colTransparentBg);
				}

				DeleteDC(hdcbmp);
			}

			return TRUE;
		}

    case WM_CTLCOLORDLG:
      {
			//mIRCDebug("color dialog");
        return (INT_PTR) p_this->getBackClrBrush();
      }
      break;

    case WM_CTLCOLORBTN:
      {

        DcxControl * p_Control = p_this->getControlByHWND( (HWND) lParam );

        if ( p_Control != NULL ) {

          COLORREF clrText = p_Control->getTextColor( );
          //COLORREF clrBackText = p_Control->getBackColor( );
          //HBRUSH hBackBrush = p_Control->getBackClrBrush( );

          /*
          
          char data[500];
          wsprintf( data, "%d - %x - %d %d %x", p_Control->getUserID( ), lParam, clrText, clrBackText, hBackBrush );
          mIRCError( data );

          */

          if ( clrText != -1 )
            SetTextColor( (HDC) wParam, clrText );

          //if ( clrBackText != -1 )
            //SetBkColor( (HDC) wParam, clrBackText );

          SetBkMode( (HDC) wParam, TRANSPARENT );

          //if ( hBackBrush != NULL )
            //return (LRESULT) hBackBrush;

          return CallWindowProc( p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam );
        }
      }
      break;

    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
      {

        DcxControl * p_Control = p_this->getControlByHWND( (HWND) lParam );

        if ( p_Control != NULL ) {

          COLORREF clrText = p_Control->getTextColor( );
          COLORREF clrBackText = p_Control->getBackColor( );
          HBRUSH hBackBrush = p_Control->getBackClrBrush( );

				// let static controls shine through
//				SetBkMode((HDC) wParam, TRANSPARENT);
//				return (LRESULT) GetStockBrush(HOLLOW_BRUSH);

          /*
          char data[500];
          wsprintf( data, "%d - %x - %d %d %x", p_Control->getUserID( ), lParam, clrText, clrBackText, hBackBrush );
          mIRCError( data );
          */
//http://www.ddj.com/dept/windows/184416395
//http://www.codeguru.com/Cpp/misc/samples/article.php/c1469#download
          if ( clrText != -1 )
            SetTextColor( (HDC) wParam, clrText );

          if ( clrBackText != -1 )
            SetBkColor( (HDC) wParam, clrBackText );
 
          if ( hBackBrush != NULL )
            return (LRESULT) hBackBrush;

        }
      }
      break;

    case WM_MOUSEMOVE:
      {
        p_this->setMouseControl( 0 );
      }
      break;

    case WM_LBUTTONDOWN:
      {
	      //mIRCError( "Dialog WM_LBUTTONDOWN" );
        p_this->callAliasEx( NULL, "%s,%d", "sclick", 0 );
        p_this->callAliasEx( NULL, "%s,%d", "lbdown", 0 );
      }
      break;

    case WM_LBUTTONUP:
      {
        p_this->callAliasEx( NULL, "%s,%d", "lbup", 0 );
      }
      break;

    case WM_LBUTTONDBLCLK:
      {
        p_this->callAliasEx( NULL, "%s,%d", "dclick", 0 );
        p_this->callAliasEx( NULL, "%s,%d", "lbdblclk", 0 );
      }
      break;

    case WM_RBUTTONDOWN:
      {
        p_this->callAliasEx( NULL, "%s,%d", "rclick", 0 );
        p_this->callAliasEx( NULL, "%s,%d", "rbdown", 0 );
      }
      break;

    case WM_RBUTTONUP:
      {
        p_this->callAliasEx( NULL, "%s,%d", "rbup", 0 );
      }
      break;

    case WM_RBUTTONDBLCLK:
      {
        p_this->callAliasEx( NULL, "%s,%d", "rbdblclk", 0 );
      }
      break;

    case WM_MBUTTONDOWN:
      {
        p_this->callAliasEx( NULL, "%s,%d", "mbdown", 0 );
      }
      break;

    case WM_MBUTTONUP:
      {
        p_this->callAliasEx( NULL, "%s,%d", "mbup", 0 );
      }
      break;
    
    case WM_MBUTTONDBLCLK:
      {
        p_this->callAliasEx( NULL, "%s,%d", "mbdblclk", 0 );
      }
      break;

      /*
    case WM_MOUSEWHEEL:
      {
        p_this->callAliasEx( NULL, "%s,%d,%d", "mwheel", 0, HIWORD( wParam ) );
      }
      break;
      */

    case WM_INITMENUPOPUP:
      {
        //mIRCError( "WM_INITMENUPOPUP" );
      }
      break;

    case WM_SETCURSOR:
      {
        if ( LOWORD( lParam ) == HTCLIENT && 
            (HWND) wParam == p_this->getHwnd( ) && 
            p_this->getCursor( ) != NULL ) 
        {
          SetCursor( p_this->getCursor( ) );
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

			p_this->callAliasEx(ret, "%s,%d,%d", "dragbegin", 0, count);

			// cancel drag drop event
			if (lstrcmpi(ret, "cancel") == 0) {
				DragFinish(files);
				return 0L;
			}

			// for each file, send callback message
			for (int i = 0; i < count; i++) {
				if (DragQueryFile(files, i, filename, 500))
					p_this->callAliasEx(ret, "%s,%d,%s", "dragfile", 0, filename);
			}

			p_this->callAliasEx(ret, "%s,%d", "dragfinish", 0);
		}

		DragFinish(files);
		break;
	}
			//case WM_GETDLGCODE:
			//{
			//	//mIRCError("Dialog WM_GETDLGCODE");
			//	return 0L;
			//}
			//break;
		case WM_ACTIVATE:
			{
				char ret[20];
				switch (wParam)
				{
				case WA_ACTIVE:
				case WA_CLICKACTIVE:
					{
						p_this->callAliasEx(ret, "%s,%d", "activate", 0);
					}
					break;
				case WA_INACTIVE:
					{
						p_this->callAliasEx(ret, "%s,%d", "deactivate", 0);
					}
					break;
				}
				// Activate bug: If we return 0 here its fixed, If we comment out CallWindowProc() below, its fixed.
				//							If on the other hand we do neither of these the code enters a WM_GETDLGCODE loop
				//							where this message is constantly sent to the button & tab controls (in tab_prob example)
				return 0L;
			}
			break;
		//case WM_NEXTDLGCTL:
		//	{
  //      mIRCError( "WM_NEXTDLGCTL" );
		//	}
		//	break;
    case WM_NCDESTROY: 
      {
        //mIRCError( "WM_NCDESTROY" );
        
        if ( p_this != NULL ) {

          LRESULT res = CallWindowProc( p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam );
          Dialogs.deleteDialog( p_this );
          delete p_this;
          return res;
        }
      }
      break;

    default:
      break;
  }

  /*
  LRESULT lRes;
  char data[500];

  if ( p_this != NULL ) {
    lRes = CallWindowProc( p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam );
    wsprintf( data, "DIALOG RETURN %d %x", lRes, lRes );
    mIRCError( data );
    return lRes;
  }

  lRes = DefWindowProc( mHwnd, uMsg, wParam, lParam );
  wsprintf( data, "DIALOG RETURN %d", lRes );
  mIRCError( data );
  return lRes;
  */

	if (p_this != NULL) {
		return CallWindowProc(p_this->m_hOldWindowProc, mHwnd, uMsg, wParam, lParam);
	}

	return DefWindowProc( mHwnd, uMsg, wParam, lParam );
}