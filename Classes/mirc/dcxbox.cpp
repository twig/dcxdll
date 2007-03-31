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

#include "../layout/layoutcellfixed.h"
#include "../layout/layoutcellfill.h"
#include "../layout/layoutcellpane.h"

extern BOOL XPPlus;

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxBox::DcxBox( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->m_TitleButton = NULL;
	this->_hTheme = NULL;

	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CONTROLPARENT,
		DCX_BOXCLASS,
		NULL,
		Styles | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);

	if ( bNoTheme )
		dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->m_pLayoutManager = new LayoutManager( this->m_Hwnd );

	this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );

	if (this->m_iBoxStyles & BOXS_CHECK) {
		this->m_TitleButton = CreateWindowEx(
			ExStyles,
			"BUTTON",
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX,
			CW_USEDEFAULT,CW_USEDEFAULT,11,10,
			this->m_Hwnd,
			(HMENU) ID,
			GetModuleHandle(NULL), 
			NULL);
	}
	else if (this->m_iBoxStyles & BOXS_RADIO) {
		this->m_TitleButton = CreateWindowEx(
		ExStyles,
		"BUTTON",
		NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON,
		CW_USEDEFAULT,CW_USEDEFAULT,11,10,
		this->m_Hwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);
	}
	if (IsWindow(this->m_TitleButton)) {
		if ( bNoTheme )
			dcxSetWindowTheme( this->m_TitleButton , L" ", L" " );
		if (!(Styles & WS_DISABLED))
			SendMessage(this->m_TitleButton,BM_SETCHECK,BST_CHECKED,0L);
	}
	if (XPPlus)
		this->_hTheme = OpenThemeDataUx(this->m_Hwnd,L"BUTTON");
}

/*!
 * \brief blah
 *
 * blah
 */

DcxBox::~DcxBox( ) {

  if ( this->m_pLayoutManager != NULL )
    delete this->m_pLayoutManager;
	if (this->_hTheme)
		CloseThemeDataUx(this->_hTheme);
  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxBox::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	unsigned int i = 1, numtok = styles.numtok( );
	this->m_iBoxStyles = 0;
	//*Styles = BS_NOTIFY;

	while ( i <= numtok ) {

		if (styles.gettok( i ) == "right")
			this->m_iBoxStyles |= BOXS_RIGHT;
		else if (styles.gettok( i ) == "center")
			this->m_iBoxStyles |= BOXS_CENTER;
		else if (styles.gettok( i ) == "bottom")
			this->m_iBoxStyles |= BOXS_BOTTOM;
		else if (styles.gettok( i ) == "none")
			this->m_iBoxStyles |= BOXS_NONE;
		else if (styles.gettok( i ) == "rounded")
			this->m_iBoxStyles |= BOXS_ROUNDED;
		else if (styles.gettok( i ) == "check") {
			this->m_iBoxStyles &= ~BOXS_RADIO;
			this->m_iBoxStyles |= BOXS_CHECK;
		}
		else if (styles.gettok( i ) == "radio") {
			this->m_iBoxStyles &= ~BOXS_CHECK;
			this->m_iBoxStyles |= BOXS_RADIO;
		}
		else if (styles.gettok( i ) == "transparent")
			*ExStyles |= WS_EX_TRANSPARENT;
		else if ( styles.gettok( i ) == "alpha" )
			this->m_bAlphaBlend = true;
		else if (( styles.gettok( i ) == "shadow" ))
			this->m_bShadowText = true;
		else if (( styles.gettok( i ) == "noformat" ))
			this->m_bCtrlCodeText = false;

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

//  int numtok = input.numtok( );

	TString prop(input.gettok( 3 ));

  // [NAME] [ID] [PROP]
  if ( prop == "text" ) {

    GetWindowText( this->m_Hwnd, szReturnValue, 900 );
    return;
  }
  else if ( prop == "inbox" ) {

    RECT rc, rcText;
    GetClientRect( this->m_Hwnd, &rc );

    int n = GetWindowTextLength( this->m_Hwnd );

    InflateRect( &rc, -2, -2 );
    if ( n > 0 ) {

      HDC hdc = GetDC( this->m_Hwnd );
      HFONT oldFont = NULL;

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
  else if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
    return;
  
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
  this->parseSwitchFlags( input.gettok( 3 ), &flags );

  int numtok = input.numtok( );

  // xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
  if ( flags.switch_flags[2] && numtok > 8 ) {

    UINT ID = mIRC_ID_OFFSET + (UINT)input.gettok( 4 ).to_int( );

    if ( ID > mIRC_ID_OFFSET - 1 && 
      !IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
      this->m_pParentDialog->getControlByID( ID ) == NULL ) 
    {
			DcxControl * p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,input,5,-1,this->m_Hwnd);

			if ( p_Control != NULL ) {
				this->m_pParentDialog->addControl( p_Control );
				this->redrawWindow( );
			}
		}
		else {
			TString error;
			error.sprintf("Control with ID \"%d\" already exists", ID - mIRC_ID_OFFSET );
			this->showError(NULL,"-c", error.to_chr());
			//DCXError("/xdid -c",error.to_chr() );
		}
	}
  // xdid -d [NAME] [ID] [SWITCH] [ID]
  else if ( flags.switch_flags[3] && numtok > 3 ) {

    UINT ID = mIRC_ID_OFFSET + input.gettok( 4 ).to_int( );
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
			}
      else {

        TString error;
        error.sprintf("Can't delete control with ID \"%d\" when it is inside it's own event (dialog %s)", 
                  p_Control->getUserID( ), this->m_pParentDialog->getName( ).to_chr( ) );
				this->showError(NULL,"-d", error.to_chr());
				//DCXError("/xdid -d",error.to_chr() );
      }
    }
    else {

      TString error;
      error.sprintf("Unknown control with ID \"%d\" (dialog %s)", 
                ID - mIRC_ID_OFFSET, this->m_pParentDialog->getName( ).to_chr( ) );
			this->showError(NULL,"-d", error.to_chr());
			//DCXError("/xdid -d",error.to_chr() );
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

		if ( input.gettok( 4 ) == "update" ) {
			if ( this->m_pLayoutManager != NULL ) {
				RECT rc;
				GetClientRect( this->m_Hwnd, &rc );
				this->m_pLayoutManager->updateLayout( rc );
				this->redrawWindow();
			}
		}
		else if (input.gettok( 4 ) == "clear") {
			if (this->m_pLayoutManager != NULL)
				delete this->m_pLayoutManager;
			this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);
			//this->redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
    else if ( numtok > 8 ) {

      TString com(input.gettok( 1, TSTAB ).gettok( 4 ));
      com.trim( );
      TString path(input.gettok( 1, TSTAB ).gettok( 5, -1 ));
      path.trim( );
      TString p2(input.gettok( 2, TSTAB ));
      p2.trim( );

      UINT flags = this->parseLayoutFlags( p2.gettok( 1 ) );
      UINT ID = p2.gettok( 2 ).to_int( );
      UINT WGT = p2.gettok( 3 ).to_int( );
      UINT W = p2.gettok( 4 ).to_int( );
      UINT H = p2.gettok( 5 ).to_int( );

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
          if ( flags & LAYOUTID ) {
            if ( cHwnd != NULL && IsWindow( cHwnd ) )
              p_Cell = new LayoutCellFill( cHwnd );
            else {
							TString error;
              error.sprintf("Cell Fill -> Invalid ID : %d", ID );
							this->showError(NULL,"-l", error.to_chr());
							//DCXError("/xdid -l", error.to_chr() );
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
                error.sprintf("Cell Fixed -> Invalid ID : %d", ID );
								this->showError(NULL,"-l", error.to_chr());
								//DCXError("/xdid -l", error.to_chr() );
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
                error.sprintf("Cell Fixed -> Invalid ID : %d", ID );
								this->showError(NULL,"-l", error.to_chr());
								//DCXError("/xdid -l",error.to_chr() );
                return;
              }
            }
          } //else
        } // else if ( flags & LAYOUTFIXED )
        else {
					this->showError(NULL,"-l", "Unknown Cell Type");
          //DCXError("/xdid -l","Unknown Cell Type" );
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
              error.sprintf("Invalid item path: %s", path.to_chr( ) );
							this->showError(NULL,"-l", error.to_chr());
							//DCXError("/xdid -l",error.to_chr() );
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

    TString text(input.gettok( 4, -1 ));
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
BOOL CALLBACK EnumBoxChildren(HWND hwnd,LPDCXENUM de)
{
	//LPDCXENUM de = (LPDCXENUM)lParam;
	if ((de->mChildHwnd != hwnd) && (GetParent(hwnd) == de->mBox))
		EnableWindow(hwnd,de->mState);

	return TRUE;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxBox::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
 // switch( uMsg ) {
 //   case WM_COMMAND:
 //     {
 //       switch ( HIWORD( wParam ) ) {
 //         case BN_CLICKED:
 //           {
	//						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
	//              this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID( ) );
 //           }
 //           break;
 //       }
	//		}
	//}
	return 0L;
}

void DcxBox::FillBkg(const HDC hdc, const LPRECT rc, const HBRUSH hBrush)
{
	if (!this->isExStyle(WS_EX_TRANSPARENT)) {
		// paint the background
		FillRect(hdc, rc, hBrush);
	}
}

LRESULT DcxBox::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
  switch( uMsg ) {

    case WM_HELP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
					this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_NOTIFY:
      {
        LPNMHDR hdr = (LPNMHDR) lParam;

        if (!hdr)
          break;

				if (IsWindow(hdr->hwndFrom)) {
					DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
      }
      break;

    case WM_COMMAND:
			{
				if (((HWND)lParam != NULL) && ((HWND)lParam == this->m_TitleButton) ) {
					switch ( HIWORD( wParam ) )
					{
						case BN_CLICKED:
						{
							BOOL state = (SendMessage(this->m_TitleButton,BM_GETCHECK,0,0) == BST_CHECKED);
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								char ret[10];

								this->callAliasEx(ret, "%s,%d,%d", "checkchange", this->getUserID(), state);

								if (lstrcmp("nochange", ret) == 0)
									return 0L;
							}

							DCXENUM de;
							de.mChildHwnd = this->m_TitleButton;
							de.mBox = this->m_Hwnd;
							de.mState = state;

							EnumChildWindows(this->m_Hwnd,(WNDENUMPROC)EnumBoxChildren,(LPARAM)&de);
							break;
						}
					} // end switch
					break;
				}
			}
    case WM_HSCROLL:
    case WM_VSCROLL:
      {
				if (IsWindow((HWND) lParam)) {
					DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
      }
      break;

		case WM_COMPAREITEM:
			{
				LPCOMPAREITEMSTRUCT idata = (LPCOMPAREITEMSTRUCT)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
			}
			break;

    case WM_DELETEITEM:
      {
				DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
      }
      break;

    case WM_MEASUREITEM:
      {
				HWND cHwnd = GetDlgItem(this->m_Hwnd, wParam);
				if (IsWindow(cHwnd)) {
					DcxControl *c_this = (DcxControl *) GetProp(cHwnd,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
      }
      break;

    case WM_DRAWITEM:
      {
				DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
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

				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_SIZE)
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

					bParsed = TRUE;
					LRESULT lRes = CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, wParam, lParam);

					if ( clrText != -1 )
						SetTextColor( (HDC) wParam, clrText );

					if ( clrBackText != -1 )
						SetBkColor( (HDC) wParam, clrBackText );

					if (p_Control->isExStyle(WS_EX_TRANSPARENT)) {
						// when transparent set as no bkg brush & default transparent drawing.
						SetBkMode((HDC) wParam, TRANSPARENT);
						hBackBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
					}

					if ( hBackBrush != NULL )
						lRes = (LRESULT) hBackBrush;

					return lRes;
				}
			}
			break;

		case WM_ENABLE: {
			this->redrawWindow();
			break;
		}


		case WM_ERASEBKGND:
		{
			//if (this->isExStyle(WS_EX_TRANSPARENT)) {
			//	bParsed = TRUE;
			//	return TRUE;
			//}
			//HDC hdc = (HDC)wParam;
			//RECT rc;
			//GetClientRect(this->m_Hwnd, &rc);
			// paint the background
			//if (this->_hTheme != NULL) {
			//	int _iStateId = (IsWindowEnabled(this->m_Hwnd) ? GBS_NORMAL : GBS_DISABLED);
			//	if (IsThemeBackgroundPartiallyTransparentUx(this->_hTheme, BP_GROUPBOX, _iStateId))
			//		DrawThemeParentBackgroundUx(this->m_Hwnd, hdc, &rc);
			//}
			//if (this->m_bAlphaBlend)
			//if (this->m_bAlphaBlend || this->isExStyle(WS_EX_TRANSPARENT))
				this->DrawParentsBackground((HDC) wParam);
			//RECT rect;
			//GetClientRect( this->m_Hwnd, &rect );
			//DcxControl::DrawCtrlBackground((HDC) wParam,this,&rect);
			bParsed = TRUE;
			return TRUE;
		}
		break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(this->m_Hwnd, &ps);
				RECT rc, rc2, rcText, rcText2;
				//int n = GetWindowTextLength(this->m_Hwnd);
				TString wtext;
				int n = TGetWindowText(this->m_Hwnd, wtext);
				HBRUSH hBrush = GetSysColorBrush(COLOR_3DFACE);

				GetClientRect(this->m_Hwnd, &rc);
				CopyRect(&rc2, &rc);

				// Setup alpha blend if any.
				LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

				// paint the background
				//if (dcxIsThemeActive())
				//	DrawThemeParentBackgroundUx(this->m_Hwnd, hdc, &rc2);
				//else
				//	this->FillBkg(hdc, &rc2, hBrush);
				if (!this->isExStyle(WS_EX_TRANSPARENT)) {
					// set up brush colors
					if (this->m_hBackBrush != NULL)
						hBrush = this->m_hBackBrush;
				}

				// if no border, dont bother
				if (this->m_iBoxStyles & BOXS_NONE) {
					this->FillBkg(hdc, &rc2, hBrush);
					EndPaint(this->m_Hwnd, &ps);

					bParsed = TRUE;
					return 0L;
				}

				SetBkMode(hdc, TRANSPARENT);

				// no text, no box!
				if (!n) {
					if (this->m_iBoxStyles & BOXS_ROUNDED) {
						HRGN m_Region = CreateRoundRectRgn(rc2.left, rc2.top, rc2.right, rc2.bottom, 10, 10);
						if (!this->isExStyle(WS_EX_TRANSPARENT))
							FillRgn(hdc,m_Region,hBrush);
						FrameRgn(hdc,m_Region,(HBRUSH)GetStockObject(DC_PEN),1,1);
						DeleteObject(m_Region);
					}
					else {
						this->FillBkg(hdc, &rc2, hBrush);
						//DrawEdge(hdc, &rc2, EDGE_RAISED, BF_TOPLEFT | BF_BOTTOMRIGHT);
						DrawEdge(hdc, &rc2, EDGE_ETCHED, BF_RECT);
					}
					if (IsWindow(this->m_TitleButton))
						SetWindowPos(this->m_TitleButton,NULL,rc2.left,rc2.top,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
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

					//char *text = new char[n +2];
					//GetWindowText(this->m_Hwnd, text, n +1);

					//TString wtext(text);
					//delete [] text;

					if (this->m_bCtrlCodeText)
						calcStrippedRect(hdc, wtext, 0, &rcText, false);
					else
						DrawText(hdc, wtext.to_chr(), n, &rcText, DT_CALCRECT);

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

					if (IsWindow(this->m_TitleButton))
					{
						RECT bSZ;
						GetWindowRect(this->m_TitleButton,&bSZ);
						bSZ.bottom = (bSZ.right - bSZ.left);
						SetWindowPos(this->m_TitleButton,NULL,rcText2.left,rcText2.top,bSZ.bottom,(rcText2.bottom - rcText2.top),SWP_NOZORDER|SWP_NOACTIVATE);
						rcText.left += bSZ.bottom;
						rcText.right += bSZ.bottom;
						rcText2.left += bSZ.bottom;
						rcText2.right += bSZ.bottom;
					}

					if (this->isExStyle(WS_EX_TRANSPARENT))
						ExcludeClipRect(hdc, rcText2.left, rcText2.top, rcText2.right, rcText2.bottom);

					// draw the border
					if (this->m_iBoxStyles & BOXS_ROUNDED) {
						HRGN m_Region = CreateRoundRectRgn(rc2.left, rc2.top, rc2.right, rc2.bottom, 10, 10);
						if (!this->isExStyle(WS_EX_TRANSPARENT))
							FillRgn(hdc,m_Region,hBrush);
						FrameRgn(hdc,m_Region,(HBRUSH)GetStockObject(BLACK_BRUSH),1,1);
						DeleteObject(m_Region);
					}
					else {
						this->FillBkg(hdc, &rc2, hBrush);
						DrawEdge(hdc, &rc2, EDGE_ETCHED, BF_RECT);
					}

					if (!this->isExStyle(WS_EX_TRANSPARENT)) {
						HRGN m_Region = CreateRectRgn(rc2.left,rc2.top,rc2.right,rc2.bottom);
						if (m_Region)
							SelectClipRgn(hdc,m_Region);
						FillRect(hdc, &rcText2, hBrush);
						//this->FillBkg(hdc, &rcText2, hBrush);
						if (m_Region) {
							SelectClipRgn(hdc,NULL);
							DeleteObject(m_Region);
						}
					}
					else
						SelectClipRgn(hdc,NULL);
					//else
					//	IntersectClipRect(hdc, rcText2.left, rcText2.top, rcText2.right, rcText2.bottom);

					// draw the text
					if (!this->m_bCtrlCodeText) {
						if (this->m_bShadowText)
							dcxDrawShadowText(hdc,wtext.to_wchr(), n,&rcText, DT_END_ELLIPSIS | DT_LEFT, this->m_clrText, 0, 5, 5);
						else
							DrawText(hdc, wtext.to_chr(), n, &rcText, DT_LEFT | DT_END_ELLIPSIS);
					}
					else
						mIRC_DrawText(hdc, wtext, &rcText, DT_LEFT | DT_END_ELLIPSIS, this->m_bShadowText);
				}

				this->FinishAlphaBlend(ai);

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
					if (GetCursor() != this->m_hCursor)
						SetCursor( this->m_hCursor );
					bParsed = TRUE;
					return TRUE;
				}
			}
			break;

		case WM_LBUTTONDOWN:
		{
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				this->callAliasEx(NULL, "%s,%d", "lbdown", this->getUserID());
			break;
		}

		case WM_LBUTTONUP:
		{
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
				this->callAliasEx(NULL, "%s,%d", "lbup", this->getUserID());
				this->callAliasEx(NULL, "%s,%d", "sclick", this->getUserID());
			}
			break;
		}

		case WM_LBUTTONDBLCLK:
		{
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				this->callAliasEx(NULL, "%s,%d", "dclick", this->getUserID());
			break;
		}

		case WM_RBUTTONDOWN:
		{
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				this->callAliasEx(NULL, "%s,%d", "rclick", this->getUserID());
			break;
		}
		case WM_THEMECHANGED:
			{
				if (this->_hTheme != NULL) {
					CloseThemeDataUx(this->_hTheme);
					this->_hTheme = OpenThemeDataUx(this->m_Hwnd,L"BUTTON");
				}
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