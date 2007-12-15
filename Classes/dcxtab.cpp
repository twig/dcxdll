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
: DcxControl(ID, p_Dialog)
, m_bClosable(false)
, m_bGradient(false)
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles | WS_EX_CONTROLPARENT, 
    DCX_TABCTRLCLASS, 
    NULL,
    WS_CHILD | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

	if (!IsWindow(this->m_Hwnd))
		throw "Unable To Create Window";

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

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
	//if (p_Dialog->getToolTip() != NULL) {
	//	if (styles.istok("tooltips")) {
	//		this->m_ToolTipHWND = p_Dialog->getToolTip();
	//		TabCtrl_SetToolTips(this->m_Hwnd,this->m_ToolTipHWND);
	//		//AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
	//	}
	//}

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

  unsigned int i = 1, numtok = styles.numtok( );

	//*ExStyles = WS_EX_CONTROLPARENT;

  while ( i <= numtok ) {

    if ( styles.gettok( i ) == "vertical" )
      *Styles |= TCS_VERTICAL;
    else if ( styles.gettok( i ) == "bottom" )
      *Styles |= TCS_BOTTOM;
    else if ( styles.gettok( i ) == "right" )
      *Styles |= TCS_RIGHT;
    else if ( styles.gettok( i ) == "fixedwidth" )
      *Styles |= TCS_FIXEDWIDTH;
    else if ( styles.gettok( i ) == "buttons" )
      *Styles |= TCS_BUTTONS;
    else if ( styles.gettok( i ) == "flat" )
      *Styles |= TCS_FLATBUTTONS;
    else if ( styles.gettok( i ) == "hot" )
      *Styles |= TCS_HOTTRACK;
    else if ( styles.gettok( i ) == "multiline" )
      *Styles |= TCS_MULTILINE;
    else if ( styles.gettok( i ) == "rightjustify" )
      *Styles |= TCS_RIGHTJUSTIFY;
    else if ( styles.gettok( i ) == "scrollopposite" )
      *Styles |= TCS_SCROLLOPPOSITE;
    //else if ( styles.gettok( i ) == "tooltips" )
    //  *Styles |= TCS_TOOLTIPS;
    else if ( styles.gettok( i ) == "flatseps" )
      *ExStyles |= TCS_EX_FLATSEPARATORS;
    else if (styles.gettok(i) == "closable") {
       this->m_bClosable = true;
       *Styles |= TCS_OWNERDRAWFIXED;
    }
		else if ( styles.gettok( i ) == "alpha" )
			this->m_bAlphaBlend = true;
		else if ( styles.gettok( i ) == "gradient" )
			this->m_bGradient = true;

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

  int numtok = input.numtok( );
	TString prop(input.gettok( 3 ));

  if ( prop == "text" && numtok > 3 ) {

    int nItem = input.gettok( 4 ).to_int( ) - 1;

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
  else if ( prop == "num" ) {

    wsprintf( szReturnValue, "%d", TabCtrl_GetItemCount( this->m_Hwnd ) );
    return;
  }
  // [NAME] [ID] [PROP] [N]
  else if ( prop == "icon" && numtok > 3 ) {

    int iTab = input.gettok( 4 ).to_int( ) - 1;

    if ( iTab > -1 && iTab < TabCtrl_GetItemCount( this->m_Hwnd ) ) {

      TCITEM tci;
      ZeroMemory( &tci, sizeof( TCITEM ) );

      tci.mask = TCIF_IMAGE;

      TabCtrl_GetItem( this->m_Hwnd, iTab, &tci );

      wsprintf( szReturnValue, "%d", tci.iImage + 1 );
      return;
    }
  }
  else if ( prop == "sel" ) {

    int nItem = TabCtrl_GetCurSel( this->m_Hwnd );

    if ( nItem > -1 && nItem < TabCtrl_GetItemCount( this->m_Hwnd ) ) {

      wsprintf( szReturnValue, "%d", nItem + 1 );
      return;
    }
  }
  else if ( prop == "seltext" ) {

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
  else if ( prop == "childid" && numtok > 3 ) {

    int nItem = input.gettok( 4 ).to_int( ) - 1;

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
	// [NAME] [ID] [PROP]
	else if (prop == "mouseitem") {
		TCHITTESTINFO tchi;

		tchi.flags = TCHT_ONITEM;
		GetCursorPos(&tchi.pt);
		ScreenToClient(this->m_Hwnd, &tchi.pt);

		int tab = TabCtrl_HitTest(this->m_Hwnd, &tchi);

		wsprintf(szReturnValue, "%d", tab +1);
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

void DcxTab::parseCommandRequest( TString & input ) {
  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  parseSwitchFlags(input.gettok(3), &flags);

  int numtok = input.numtok( );

  // xdid -r [NAME] [ID] [SWITCH]
	if (flags.switch_flags[17]) {
		int n = 0;
		TCITEM tci;
		int nItems = TabCtrl_GetItemCount(this->m_Hwnd);

		while (n < nItems) {
			ZeroMemory(&tci, sizeof(TCITEM));

			tci.mask = TCIF_PARAM;

			if (TabCtrl_GetItem(this->m_Hwnd, n, &tci)) {
				LPDCXTCITEM lpdtci = (LPDCXTCITEM) tci.lParam;

				if (lpdtci != NULL && lpdtci->mChildHwnd != NULL && IsWindow(lpdtci->mChildHwnd)) {
					DestroyWindow(lpdtci->mChildHwnd);
					delete lpdtci;
				}
			}

			++n;
		}

		TabCtrl_DeleteAllItems(this->m_Hwnd);
	}

  // xdid -a [NAME] [ID] [SWITCH] [N] [ICON] [TEXT][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)[TAB](TOOLTIP)
  if ( flags.switch_flags[0] && numtok > 4 ) {
    TCITEM tci;
    ZeroMemory( &tci, sizeof( TCITEM ) );
    tci.mask = TCIF_IMAGE | TCIF_PARAM;

    TString data(input.gettok( 1, TSTAB ));
    data.trim( );

    TString control_data;
    if ( input.numtok( TSTAB ) > 1 ) {
      control_data = input.gettok( 2, TSTAB );
      control_data.trim( );
    }

    TString tooltip;
    if ( input.numtok( TSTAB ) > 2 ) {
      tooltip = input.gettok( 3, -1, TSTAB );
      tooltip.trim( );
    }

    int nIndex = data.gettok( 4 ).to_int( ) - 1;

    if ( nIndex == -1 )
      nIndex += TabCtrl_GetItemCount( this->m_Hwnd ) + 1;

	 tci.iImage = data.gettok( 5 ).to_int( ) - 1;

    // Extra params
    LPDCXTCITEM lpdtci = new DCXTCITEM;
    lpdtci->tsTipText = tooltip;
    tci.lParam = (LPARAM) lpdtci;

    // Itemtext
    TString itemtext;
    if ( data.numtok( ) > 5 ) {
      itemtext = data.gettok( 6, -1 );
      tci.mask |= TCIF_TEXT;

      if (this->m_bClosable)
         itemtext += "   ";

      tci.pszText = itemtext.to_chr( );
    }

    if ( control_data.numtok( ) > 5 ) {
      UINT ID = mIRC_ID_OFFSET + (UINT)control_data.gettok( 1 ).to_int( );

      if ( ID > mIRC_ID_OFFSET - 1 && 
        !IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
        this->m_pParentDialog->getControlByID( ID ) == NULL ) 
      {
				try {
					DcxControl * p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,control_data,2,
						CTLF_ALLOW_TREEVIEW |
						CTLF_ALLOW_LISTVIEW |
						CTLF_ALLOW_RICHEDIT |
						CTLF_ALLOW_DIVIDER |
						CTLF_ALLOW_PANEL |
						CTLF_ALLOW_TAB |
						CTLF_ALLOW_REBAR |
						CTLF_ALLOW_WEBCTRL |
						CTLF_ALLOW_EDIT |
						CTLF_ALLOW_IMAGE |
						CTLF_ALLOW_LIST
						,this->m_Hwnd);

					if ( p_Control != NULL ) {
						lpdtci->mChildHwnd = p_Control->getHwnd( );
						this->m_pParentDialog->addControl( p_Control );
					}
				}
				catch ( char *err ) {
					this->showErrorEx(NULL, "-a", "Unable To Create Control %d (%s)", ID - mIRC_ID_OFFSET, err);
				}
      }
      else
				this->showErrorEx(NULL, "-a", "Control with ID \"%d\" already exists", ID - mIRC_ID_OFFSET );
    }

    TabCtrl_InsertItem( this->m_Hwnd, nIndex, &tci );
    this->activateSelectedTab( );
  }
  // xdid -c [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[2] && numtok > 3 ) {
    int nItem = input.gettok( 4 ).to_int( ) - 1;

    if ( nItem > -1 && nItem < TabCtrl_GetItemCount( this->m_Hwnd ) ) {
      TabCtrl_SetCurSel( this->m_Hwnd, nItem );
      this->activateSelectedTab( );
    }
  }
  // xdid -d [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[3] && numtok > 3 ) {
	  int nItem = input.gettok( 4 ).to_int( ) - 1;

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
    int nItem = input.gettok( 4 ).to_int( ) - 1;
    int nIcon = input.gettok( 5 ).to_int( ) - 1;

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

    int X = input.gettok( 4 ).to_int( );
    int Y = input.gettok( 5 ).to_int( );

    TabCtrl_SetItemSize( this->m_Hwnd, X, Y );
  }
  // xdid -r [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[17] ) {
  }
  // xdid -t [NAME] [ID] [SWITCH] [N] (text)
  else if ( flags.switch_flags[19] && numtok > 3 ) {

    int nItem = input.gettok( 4 ).to_int( ) - 1;

    if ( nItem > -1 && nItem < TabCtrl_GetItemCount( this->m_Hwnd ) ) {

      TString itemtext;

      TCITEM tci;
      ZeroMemory( &tci, sizeof( TCITEM ) );
      tci.mask = TCIF_TEXT;


      if ( numtok > 4 ) {
        itemtext = input.gettok( 5, -1 );
        itemtext.trim( );
      }

      tci.pszText = itemtext.to_chr( );

      TabCtrl_SetItem( this->m_Hwnd, nItem, &tci );
    }
  }

   // xdid -v [DNAME] [ID] [SWITCH] [N] [POS]
   else if (flags.switch_flags[21] && numtok > 4) {
      int nItem = input.gettok(4).to_int();
      int pos = input.gettok(5).to_int();
      BOOL adjustDelete = FALSE;

      if (nItem == pos)
         return;
      else if ((nItem < 1) || (nItem > TabCtrl_GetItemCount(this->m_Hwnd)))
         return;
      else if ((pos < 1) || (pos > TabCtrl_GetItemCount(this->m_Hwnd)))
         return;

      // does the nItem index get shifted after we insert
      if (nItem > pos)
         adjustDelete = TRUE;

      // decrement coz of 0-index
      nItem--;

      // get the item we're moving
      char* text = new char[900];
      TCITEM tci;
      ZeroMemory(&tci, sizeof(TCITEM));
  
      tci.pszText = text;
      tci.cchTextMax = 900;
      tci.mask = TCIF_IMAGE | TCIF_PARAM | TCIF_TEXT | TCIF_STATE;

      TabCtrl_GetItem(this->m_Hwnd, nItem, &tci);

      // insert it into the new position
      TabCtrl_InsertItem(this->m_Hwnd, pos, &tci);

      // remove the old tab item
      TabCtrl_DeleteItem(this->m_Hwnd, (adjustDelete ? nItem +1 : nItem));
   }

	// xdid -w [NAME] [ID] [SWITCH] [FLAGS] [INDEX] [FILENAME]
	else if (flags.switch_flags[22] && numtok > 5) {
		HIMAGELIST himl;
		HICON icon;
		TString flag(input.gettok( 4 ));
		int index = input.gettok( 5 ).to_int();
		TString filename(input.gettok(6, -1));

		if ((himl = this->getImageList()) == NULL) {
			himl = this->createImageList();

			if (himl)
				this->setImageList(himl);
		}

		icon = dcxLoadIcon(index, filename, FALSE, flag);

		//if (flag.find('g', 0))
		//	icon = CreateGrayscaleIcon(icon);

		ImageList_AddIcon(himl, icon);
		DestroyIcon(icon);
	}
  // xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
  else if ( flags.switch_flags[24] ) {

    ImageList_Destroy( this->getImageList( ) );
  }
  else
    this->parseGlobalCommandRequest( input, flags );
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

void DcxTab::deleteLParamInfo( const int nItem ) {

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
						SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOOWNERZORDER );
				}
				else {
					hdwp = DeferWindowPos( hdwp, lpdtci->mChildHwnd, NULL, 0, 0, 0, 0, 
						SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOOWNERZORDER );
				}
			}
		}
		EndDeferWindowPos( hdwp );
	}
}

void DcxTab::GetCloseButtonRect(const RECT& rcItem, RECT& rcCloseButton)
{
	// ----------
	//rcCloseButton.top = rcItem.top + 2;
	//rcCloseButton.bottom = rcCloseButton.top + (m_iiCloseButton.rcImage.bottom - m_iiCloseButton.rcImage.top);
	//rcCloseButton.right = rcItem.right - 2;
	//rcCloseButton.left = rcCloseButton.right - (m_iiCloseButton.rcImage.right - m_iiCloseButton.rcImage.left);
	// ----------
	rcCloseButton.top = rcItem.top + 2;
	rcCloseButton.bottom = rcCloseButton.top + (16);
	rcCloseButton.right = rcItem.right - 2;
	rcCloseButton.left = rcCloseButton.right - (16);
	// ----------
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxTab::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) {
	switch (uMsg) {
		case WM_NOTIFY : 
			{
				LPNMHDR hdr = (LPNMHDR) lParam;

				if (!hdr)
					break;

				switch (hdr->code) {
					case NM_RCLICK:
					{
						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
							TCHITTESTINFO tchi;

							tchi.flags = TCHT_ONITEM;
							GetCursorPos(&tchi.pt);
							ScreenToClient(this->m_Hwnd, &tchi.pt);

							int tab = TabCtrl_HitTest(this->m_Hwnd, &tchi);
							int stab = TabCtrl_GetCurSel(this->m_Hwnd);

							if (tab != -1)
								this->callAliasEx(NULL, "%s,%d,%d", "rclick", this->getUserID(), tab +1);
						}

						bParsed = TRUE;
						break;
					}

					case NM_CLICK:
						{
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								int tab = TabCtrl_GetCurFocus(this->m_Hwnd);
								//int tab = TabCtrl_GetCurSel(this->m_Hwnd);

								if (tab != -1) {
									if (this->m_bClosable) {
										RECT rcCloseButton, rc;
										POINT pt;
										GetCursorPos(&pt);
										MapWindowPoints(NULL,this->m_Hwnd, &pt, 1);
										TabCtrl_GetItemRect(this->m_Hwnd, tab, &rc);
										GetCloseButtonRect(rc, rcCloseButton);

										if (PtInRect(&rcCloseButton, pt)) {
											this->callAliasEx(NULL, "%s,%d,%d", "closetab", this->getUserID(), tab +1);
											break;
										}
									}
									this->callAliasEx(NULL, "%s,%d,%d", "sclick", this->getUserID(), tab +1);
								}
							}
						}
					// fall through.
					case TCN_SELCHANGE:
						{
							this->activateSelectedTab();
							bParsed = TRUE;
						}
						break;
				}
				break;
			}

		// Original source based on code from eMule 0.47 source code available at http://www.emule-project.net
		case WM_DRAWITEM:
			{
				if (!m_bClosable)
					break;

				DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;

				if ((idata == NULL) || (!IsWindow(idata->hwndItem)))
					break;

				//DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem, "dcx_cthis");

				//if (c_this == NULL)
				//	break;

				RECT rect;
				int nTabIndex = idata->itemID;

				if (nTabIndex < 0)
					break;

				CopyRect(&rect, &idata->rcItem);

				// if themes are active use them.
				// call default WndProc(), DrawThemeParentBackgroundUx() is only temporary
				DcxControl::DrawCtrlBackground(idata->hDC, this, &rect);
				//DrawThemeParentBackgroundUx(this->m_Hwnd, idata->hDC, &rect);
				//CopyRect(&rect, &idata->rcItem);
				if (this->m_bGradient) {
					if (this->m_clrBackText == -1)
						// Gives a nice silver/gray gradient
						XPopupMenuItem::DrawGradient(idata->hDC, &rect, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNFACE), TRUE);
					else
						XPopupMenuItem::DrawGradient(idata->hDC, &rect, GetSysColor(COLOR_BTNHIGHLIGHT), this->m_clrBackText, TRUE);
				}
				rect.left += 1+ GetSystemMetrics(SM_CXEDGE); // move in past border.

				// TODO: (twig) Ook can u take a look at this plz? string stuff isnt my forte
				TCHAR szLabel[900];
				TC_ITEM tci;

				tci.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_STATE;
				tci.pszText = szLabel;
				tci.cchTextMax = 900;
				tci.dwStateMask = TCIS_HIGHLIGHTED;

				if (!TabCtrl_GetItem(this->getHwnd(), nTabIndex, &tci)) {
					mIRCDebug("DcxTab: invalid item");
					break;
				}

				TString label(szLabel);

				// fill the rect so it appears to "merge" with the tab page content
				//if (!dcxIsThemeActive())
				//FillRect(idata->hDC, &rect, GetSysColorBrush(COLOR_BTNFACE));

				// set transparent so text background isnt annoying
				int iOldBkMode = SetBkMode(idata->hDC, TRANSPARENT);

				// Draw icon on left side if the item has an icon
				if (tci.iImage != -1) {
					ImageList_DrawEx( this->getImageList(), tci.iImage, idata->hDC, rect.left, rect.top, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT );
					IMAGEINFO ii;
					ImageList_GetImageInfo( this->getImageList(), tci.iImage, &ii);
					rect.left += (ii.rcImage.right - ii.rcImage.left);
				}
				// Draw 'Close button' at right side
				if (m_bClosable) {
					RECT rcCloseButton;
					GetCloseButtonRect(rect, rcCloseButton);
					// Draw systems close button ? or do you want a custom close button?
					DrawFrameControl(idata->hDC, &rcCloseButton, DFC_CAPTION, DFCS_CAPTIONCLOSE | DFCS_FLAT | DFCS_TRANSPARENT);
					//MoveToEx( idata->hDC, rcCloseButton.left, rcCloseButton.top, NULL );
					//LineTo( idata->hDC, rcCloseButton.right, rcCloseButton.bottom );
					//MoveToEx( idata->hDC, rcCloseButton.right, rcCloseButton.top, NULL );
					//LineTo( idata->hDC, rcCloseButton.left, rcCloseButton.bottom );

					rect.right = rcCloseButton.left - 2;
				}
				COLORREF crOldColor;

				if (tci.dwState & TCIS_HIGHLIGHTED)
					crOldColor = SetTextColor(idata->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));

				rect.top += 1+ GetSystemMetrics(SM_CYEDGE); //4;
				//DrawText(idata->hDC, label.to_chr(), label.len(), &rect, DT_SINGLELINE | DT_TOP | DT_NOPREFIX);
				// allow mirc formatted text.
				mIRC_DrawText(idata->hDC, label, &rect, DT_SINGLELINE | DT_TOP | DT_NOPREFIX, false);

				if (tci.dwState & TCIS_HIGHLIGHTED)
					SetTextColor(idata->hDC, crOldColor);

				SetBkMode(idata->hDC, iOldBkMode);
				break;
			}
	}

	return 0L;
}

LRESULT DcxTab::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
  switch( uMsg ) {

		case WM_CONTEXTMENU:
		case WM_LBUTTONUP:
			break;

    case WM_NOTIFY : 
      {
        LPNMHDR hdr = (LPNMHDR) lParam;

        if (!hdr)
          break;

				//if (hdr->hwndFrom == this->m_ToolTipHWND) {
				//	switch(hdr->code) {
				//	case TTN_GETDISPINFO:
				//		{
				//			LPNMTTDISPINFO di = (LPNMTTDISPINFO)lParam;
				//			di->lpszText = this->m_tsToolTip.to_chr();
				//			di->hinst = NULL;
				//			bParsed = TRUE;
				//		}
				//		break;
				//	case TTN_LINKCLICK:
				//		{
				//			bParsed = TRUE;
				//			this->callAliasEx(NULL, "%s,%d", "tooltiplink", this->getUserID());
				//		}
				//		break;
				//	default:
				//		break;
				//	}
				//}
				if (IsWindow(hdr->hwndFrom)) {
					DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,"dcx_cthis");
					if (c_this != NULL) {
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
      }
      break;

    case WM_HSCROLL: 
    case WM_VSCROLL: 
    case WM_COMMAND:
      {
				if (IsWindow((HWND) lParam)) {
					DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,"dcx_cthis");
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

    case WM_SIZE:
      {
        this->activateSelectedTab( );
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_SIZE)
	        this->callAliasEx( NULL, "%s,%d", "sizing", this->getUserID( ) );
      }
      break;

		case WM_ERASEBKGND:
			{
				if (this->isExStyle(WS_EX_TRANSPARENT))
					this->DrawParentsBackground((HDC)wParam);
				else {
					RECT rect;
					GetClientRect( this->m_Hwnd, &rect );
					DcxControl::DrawCtrlBackground((HDC) wParam,this,&rect);
				}
				bParsed = TRUE;
				return TRUE;
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

		//case WM_CLOSE:
		//	{
		//		if (GetKeyState(VK_ESCAPE) != 0) // don't allow the window to close if escape is pressed. Needs looking into for a better method.
		//			bParsed = TRUE;
		//	}
		//	break;
    case WM_DESTROY:
      {
        delete this;
        bParsed = TRUE;
      }
      break;

    default:
			lRes = this->CommonMessage( uMsg, wParam, lParam, bParsed);
      break;
  }

  return lRes;
}
