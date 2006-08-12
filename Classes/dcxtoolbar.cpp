/*!
http://www.codeguru.com/cpp/controls/toolbar/article.php/c5487/
grey icons

* \file dcxtoolbar.cpp
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

#include "dcxtoolbar.h"
#include "dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxToolBar::DcxToolBar( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    0,
    DCX_TOOLBARCLASS,
    NULL,
    WS_CHILD | WS_VISIBLE | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  if ( ExStyles != 0 )
    SendMessage( this->m_Hwnd, TB_SETEXTENDEDSTYLE, (WPARAM) 0, (LPARAM) ExStyles );

  SendMessage( this->m_Hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), (LPARAM) 0 );

  this->autoSize( );
  this->m_bAutoStretch = FALSE;

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

DcxToolBar::DcxToolBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles )
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    0,
    DCX_TOOLBARCLASS,
    NULL,
    WS_CHILD | WS_VISIBLE | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  if ( ExStyles != 0 )
    SendMessage( this->m_Hwnd, TB_SETEXTENDEDSTYLE, (WPARAM) 0, (LPARAM) ExStyles );

  SendMessage( this->m_Hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), (LPARAM) 0 );
	this->m_ToolTipHWND = (HWND)SendMessage( this->m_Hwnd, TB_GETTOOLTIPS, NULL, NULL);
	//SendMessage( this->m_Hwnd, TB_SETPARENT, (WPARAM)mParentHwnd, NULL);

  this->autoSize( );
  this->m_bAutoStretch = FALSE;

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxToolBar::~DcxToolBar( ) {

  this->resetContent( );

  ImageList_Destroy( this->getImageList( TB_IML_NORMAL ) );
  ImageList_Destroy( this->getImageList( TB_IML_DISABLE ) );
  ImageList_Destroy( this->getImageList( TB_IML_HOT ) );

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxToolBar::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  //*Styles |= CCS_ADJUSTABLE;
  unsigned int i = 1, numtok = styles.numtok( " " );

  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "flat" ) 
      *Styles |= TBSTYLE_FLAT;
    else if ( styles.gettok( i , " " ) == "tooltips" ) 
      *Styles |= TBSTYLE_TOOLTIPS;
    else if ( styles.gettok( i , " " ) == "transparent" ) 
      *Styles |= TBSTYLE_TRANSPARENT;
    else if ( styles.gettok( i , " " ) == "nodivider" ) 
      *Styles |= CCS_NODIVIDER;
    else if ( styles.gettok( i , " " ) == "top" ) 
      *Styles |= CCS_TOP;
    else if ( styles.gettok( i , " " ) == "bottom" ) 
      *Styles |= CCS_BOTTOM;
    else if ( styles.gettok( i , " " ) == "left" ) 
      *Styles |= CCS_LEFT;
    else if ( styles.gettok( i , " " ) == "right" ) 
      *Styles |= CCS_RIGHT;
    //else if ( styles.gettok( i , " " ) == "noresize" ) 
      //*Styles |= CCS_NORESIZE;
    //else if ( styles.gettok( i , " " ) == "noparentalign" ) 
      //*Styles |= CCS_NOPARENTALIGN ;
    else if ( styles.gettok( i , " " ) == "noauto" )
      *Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
    else if ( styles.gettok( i , " " ) == "adjustable" ) 
      *Styles |= CCS_ADJUSTABLE;
    else if ( styles.gettok( i , " " ) == "list" ) 
      *Styles |= TBSTYLE_LIST;
    else if ( styles.gettok( i , " " ) == "wrap" ) 
      *Styles |= TBSTYLE_WRAPABLE;
    else if ( styles.gettok( i , " " ) == "arrows" ) 
      *ExStyles |= TBSTYLE_EX_DRAWDDARROWS;

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

void DcxToolBar::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );

  // [NAME] [ID] [PROP]
  if ( input.gettok( 3, " " ) == "num" ) {
    wsprintf( szReturnValue, "%d", this->getButtonCount( ) );
    return;
  }
	// [NAME] [ID] [PROP]
	else if (input.gettok(3, " ") == "mouseitem") {
		long lResult = SendMessage(this->m_Hwnd, TB_GETHOTITEM, NULL, NULL);

		wsprintf(szReturnValue, "%d", lResult);
		return;
	}
  // [NAME] [ID] [PROP]
  else if ( input.gettok( 3, " " ) == "text" && numtok > 3 ) {

    int nButton = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nButton > -1 && nButton < this->getButtonCount( ) ) {

      this->getButtonText( this->getIndexToCommand( nButton ), szReturnValue );
      return;
    }
  }
  // [NAME] [ID] [PROP] [N]
  else if ( input.gettok( 3, " " ) == "icon" && numtok > 3 ) {

    int iButton = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( iButton > -1 && iButton < this->getButtonCount( ) ) {

      TBBUTTONINFO tbbi;
      ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
      tbbi.cbSize = sizeof( TBBUTTONINFO );
      tbbi.dwMask = TBIF_IMAGE | TBIF_BYINDEX;
      this->getButtonInfo( iButton, &tbbi );

      wsprintf( szReturnValue, "%d", tbbi.iImage + 1 );
      return;
    }
  }
  // [NAME] [ID] [PROP] [N]
  else if ( input.gettok( 3, " " ) == "state" && numtok > 3 ) {

    int iButton = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( iButton > -1 && iButton < this->getButtonCount( ) ) {

      TBBUTTONINFO tbbi;
      ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
      tbbi.cbSize = sizeof( TBBUTTONINFO );
      tbbi.dwMask = TBIF_STATE | TBIF_BYINDEX;
      this->getButtonInfo( iButton, &tbbi );

      lstrcpy( szReturnValue, "+" );

      if ( !( tbbi.fsState & TBSTATE_ENABLED ) )
        lstrcat( szReturnValue,"d" );

      if ( tbbi.fsState & TBSTATE_INDETERMINATE )
        lstrcat( szReturnValue,"i" );

      if ( tbbi.fsState & TBSTATE_HIDDEN )
        lstrcat( szReturnValue,"h" );

      if ( tbbi.fsState & TBSTATE_PRESSED )
        lstrcat( szReturnValue,"p" );

      if ( tbbi.fsState & TBSTATE_CHECKED )
        lstrcat( szReturnValue,"x" );

      if ( tbbi.fsState & TBSTATE_WRAP )
        lstrcat( szReturnValue,"w" );

      return;
    }
  }
  // [NAME] [ID] [PROP] [N]
  else if ( input.gettok( 3, " " ) == "tooltip" && numtok > 3 ) {

    int iButton = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( iButton > -1 && iButton < this->getButtonCount( ) ) {

      TBBUTTONINFO tbbi;
      ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
      tbbi.cbSize = sizeof( TBBUTTONINFO );
      tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;
      this->getButtonInfo( iButton, &tbbi );

      LPDCXTBBUTTON lpdcxtbb = (LPDCXTBBUTTON) tbbi.lParam;

      if ( lpdcxtbb != NULL ) {

        lstrcpy( szReturnValue, lpdcxtbb->tsTipText.to_chr( ) );
      }
      return;
    }
  }
	// [NAME] [ID] [PROP]
	else if (input.gettok(3, " ") == "tooltipbgcolor") {
		DWORD cref = 0;

		if (this->m_ToolTipHWND != NULL)
			cref = (DWORD) SendMessage(this->m_ToolTipHWND,TTM_GETTIPBKCOLOR, NULL, NULL);

		wsprintf(szReturnValue, "%ld", cref);
		return;
	}
	// [NAME] [ID] [PROP]
	else if (input.gettok(3, " ") == "tooltiptextcolor") {
		DWORD cref = 0;

		if (this->m_ToolTipHWND != NULL)
			cref = (DWORD) SendMessage(this->m_ToolTipHWND, TTM_GETTIPTEXTCOLOR, NULL, NULL);

		wsprintf(szReturnValue, "%ld", cref);
		return;
	}
	else if (this->parseGlobalInfoRequest(input, szReturnValue)) {
		return;
	}

	szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxToolBar::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  // xdid -a [NAME] [ID] [SWITCH] [N] [+FLAGS] [WIDTH] [#ICON] [COLOR] [Button Text][TAB]Tooltip Text
  if ( flags.switch_flags[0] && numtok > 4 ) {

    int nPos = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nPos == -1 )
      nPos += this->getButtonCount( ) + 1;

    TString flags = input.gettok( 5, " " );
    int width = atoi( input.gettok( 6, " " ).to_chr( ) );
    int icon = atoi( input.gettok( 7, " " ).to_chr( ) ) - 1;
    COLORREF clrText = (COLORREF) atol( input.gettok( 8, " " ).to_chr( ) );

    TBBUTTON tbb;
    ZeroMemory( &tbb, sizeof( TBBUTTON ) );

    tbb.fsState = parseButtonStateFlags( flags );
    tbb.idCommand = this->getFreeButtonID( );
    UINT buttonStyles = parseButtonStyleFlags( flags );
    tbb.fsStyle = buttonStyles;

    if ( icon == -1 )
      tbb.iBitmap = I_IMAGENONE;
    else
      tbb.iBitmap = icon;

    TString itemtext;

    if ( input.gettok( 1, "\t" ).numtok( " " ) > 8 ) {
      
      itemtext = input.gettok( 1, "\t" ).gettok( 9, -1, " " );
      itemtext.trim( );

      if ( itemtext == "-" ) {
        
        tbb.fsStyle = BTNS_SEP;
        //tbb.fsState = TBSTATE_ENABLED;
        tbb.iBitmap = icon;
        tbb.dwData = 0;
        tbb.iString = 0;

        this->insertButton( nPos, &tbb );
        this->autoSize( );
        this->redrawWindow( );
        return;
      }
    }

    // Tooltip Handling
    LPDCXTBBUTTON lpdcxtbb = new DCXTBBUTTON;

    lpdcxtbb->tsTipText = "";

    if ( input.numtok( "\t" ) > 1 ) {
      lpdcxtbb->tsTipText = input.gettok( 2, "\t" );
      lpdcxtbb->tsTipText.trim( );
    }
    
    if ( buttonStyles & BTNS_UNDERLINE )
      lpdcxtbb->bUline = TRUE;
    else
      lpdcxtbb->bUline = FALSE;

    if ( buttonStyles & BTNS_BOLD )
      lpdcxtbb->bBold = TRUE;
    else
      lpdcxtbb->bBold = FALSE;

    if ( buttonStyles & BTNS_COLOR )
      lpdcxtbb->clrText = clrText;
    else
      lpdcxtbb->clrText = -1;

    tbb.dwData = (LPARAM) lpdcxtbb;
		lpdcxtbb->bText = itemtext;
		tbb.iString = (INT_PTR)lpdcxtbb->bText.to_chr();

    // insert button
    this->insertButton( nPos, &tbb );

    // set width of button
    if ( buttonStyles & BTNS_WIDTH ) {

      TBBUTTONINFO tbbi;
      ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
      tbbi.cbSize = sizeof( TBBUTTONINFO );
      tbbi.dwMask = 0 ;

      tbbi.dwMask |= TBIF_SIZE;
      tbbi.cx = width;
      this->setButtonInfo( tbb.idCommand, &tbbi );
    }

    this->autoSize( );
    if ( this->m_bAutoStretch )
      this->autoStretchButtons( );
  }
  // xdid -c [NAME] [ID] [SWITCH] [N] [+FLAGS] [RGB]
  else if ( flags.switch_flags[2] && numtok > 5 ) {

    int nButton = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    UINT buttonStyles = parseButtonStyleFlags( input.gettok( 5, " " ) );
    COLORREF clrColor = atol( input.gettok( 6, " " ).to_chr( ) );
		if (nButton == -1 && this->m_ToolTipHWND != NULL) {
			if (buttonStyles & BTNS_TBKGCOLOR)
				SendMessage(this->m_ToolTipHWND,TTM_SETTIPBKCOLOR, (WPARAM)clrColor, NULL);
			else if (buttonStyles & BTNS_TTXTCOLOR)
				SendMessage(this->m_ToolTipHWND,TTM_SETTIPTEXTCOLOR, (WPARAM)clrColor, NULL);
		}
    else if ( nButton > -1 && nButton < this->getButtonCount( ) ) {

      TBBUTTONINFO tbbi;
      ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
      tbbi.cbSize = sizeof( TBBUTTONINFO );
      tbbi.dwMask = TBIF_LPARAM;

      if ( this->getButtonInfo( this->getIndexToCommand( nButton ), &tbbi ) ) {

        LPDCXTBBUTTON lpdcxtbb = (LPDCXTBBUTTON) tbbi.lParam;

        if ( buttonStyles & BTNS_UNDERLINE )
          lpdcxtbb->bUline = TRUE;
        else
          lpdcxtbb->bUline = FALSE;

        if ( buttonStyles & BTNS_BOLD )
          lpdcxtbb->bBold = TRUE;
        else
          lpdcxtbb->bBold = FALSE;

        if ( buttonStyles & BTNS_COLOR )
          lpdcxtbb->clrText = clrColor;
        else
          lpdcxtbb->clrText = -1;

        this->redrawWindow( );
      }
    }
  }
  // xdid -d [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[3] && numtok > 3 ) {
    int nButton = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if (nButton > -1)
		 if (this->deleteButton(nButton)) {
//			mIRCError("success");
		 }
		 else {
//mIRCError("huh?");
		 }
  }
  // xdid -i [NAME] [ID] [SWITCH] [N] [IMAGE]
  else if ( flags.switch_flags[8] && numtok > 4 ) {

    int nButton = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    int iImage = atoi( input.gettok( 5, " " ).to_chr( ) ) - 1;

    TBBUTTONINFO tbbi;
    ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
    tbbi.cbSize = sizeof( TBBUTTONINFO );
    tbbi.dwMask = TBIF_IMAGE;
    tbbi.iImage = iImage;

    this->setButtonInfo( this->getIndexToCommand( nButton ), &tbbi );
  }
  // xdid -j [NAME] [ID] [SWITCH] [MIN] [MAX]
  else if ( flags.switch_flags[9] && numtok > 4 ) {

    int nMin = atoi( input.gettok( 4, " " ).to_chr( ) );
    int nMax = atoi( input.gettok( 4, " " ).to_chr( ) );

    this->setButtonWidth( nMin, nMax );
  }
  // xdid -l [NAME] [ID] [SWITCH] [SIZE]
  else if ( flags.switch_flags[11] && numtok > 3 ) {

    HIMAGELIST himl;

    int size = atoi( input.gettok( 4, " " ).to_chr( ) );

    ImageList_Destroy( this->getImageList( TB_IML_NORMAL ) );
    ImageList_Destroy( this->getImageList( TB_IML_DISABLE ) );
    ImageList_Destroy( this->getImageList( TB_IML_HOT ) );

    if ( size != 32 && size != 24 )
      size = 16;

    //TString error;
    
    himl = this->createImageList( size );
    this->setImageList( himl, TB_IML_NORMAL );
    //error.sprintf("Image List Normal %X", himl );
    //mIRCError( error.to_chr() );
    himl = this->createImageList( size );
    this->setImageList( himl, TB_IML_DISABLE );
    //error.sprintf("Image List Disabled %X", himl );
    //mIRCError( error.to_chr() );
    himl = this->createImageList( size );
    this->setImageList( himl, TB_IML_HOT );
    //error.sprintf("Image List Hot %X", himl );
    //mIRCError( error.to_chr() );

    this->redrawWindow( );
  }
  // xdid -m [NAME] [ID] [SWITCH] [1|0]
  else if ( flags.switch_flags[12] && numtok > 3 ) {

    if ( input.gettok( 4, " " ) == "1" ) {

      this->m_bAutoStretch = TRUE;
      this->autoStretchButtons( );
    }
    else
      this->m_bAutoStretch = FALSE;
  }
  // xdid -q [NAME] [ID] [SWITCH] [N] (TIPTEXT)
	else if (flags.switch_flags[16] && numtok > 3) {
		int nButton = atoi(input.gettok(4, " ").to_chr()) -1;

		if (nButton > -1 && nButton < this->getButtonCount()) {
			int nIndex = this->getIndexToCommand(nButton) -1;
			TBBUTTONINFO tbbi;

			ZeroMemory(&tbbi, sizeof(TBBUTTONINFO));
			tbbi.cbSize = sizeof(TBBUTTONINFO);
			tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;

			if (this->getButtonInfo(nIndex, &tbbi) > -1) {
				LPDCXTBBUTTON lpdcxtbb = (LPDCXTBBUTTON) tbbi.lParam;

				if (numtok > 4)	// has a new tooltip
					lpdcxtbb->tsTipText = input.gettok(5, -1, " ");
				else					// no tooltip
					lpdcxtbb->tsTipText = "";
			}
		}
	}
  // xdid -r [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[17] ) {

    this->resetContent( );
  }
  // xdid -t [NAME] [ID] [SWITCH] [N] [+FLAGS]
  else if ( flags.switch_flags[19] && numtok > 4 ) {

    int nButton = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    UINT fStates = parseButtonStateFlags( input.gettok( 5, " " ) );

    int idButton = this->getIndexToCommand( nButton );

    if ( idButton > 0 )
      this->setButtonState( idButton, fStates );
  }
  // xdid -u [NAME] [ID] [SWITCH] [DX] [DY]
  else if ( flags.switch_flags[20] && numtok > 4 ) {

    int dxButton = atoi( input.gettok( 4, " " ).to_chr( ) );
    int dyButton = atoi( input.gettok( 5, " " ).to_chr( ) );

    this->setButtonSize( dxButton, dyButton );
  }
  // xdid -v [NAME] [ID] [SWITCH] [N] (TEXT)
  else if ( flags.switch_flags[21] && numtok > 3 ) {

    int nButton = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
		//mIRCDebug("but: %d cnt: %d", nButton, this->getButtonCount( ));
    if ( nButton > -1 && nButton < this->getButtonCount( ) ) {
			int nIndex = this->getIndexToCommand(nButton);
			//mIRCDebug("index: %d", nIndex);
			TBBUTTONINFO tbbi;

			ZeroMemory(&tbbi, sizeof(TBBUTTONINFO));
			tbbi.cbSize = sizeof(TBBUTTONINFO);
			tbbi.dwMask = TBIF_LPARAM;
			if (this->getButtonInfo(nIndex, &tbbi) > -1) {
				//mIRCError("Got Info");
				LPDCXTBBUTTON lpdcxtbb = (LPDCXTBBUTTON) tbbi.lParam;
				if ( numtok > 4 )
					lpdcxtbb->bText = input.gettok( 5, -1, " " );
				else
					lpdcxtbb->bText = "";
				ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
				tbbi.cbSize = sizeof( TBBUTTONINFO );
				tbbi.dwMask = TBIF_TEXT;
				tbbi.pszText = lpdcxtbb->bText.to_chr();
				this->setButtonInfo(nIndex, &tbbi);
			}
      //TString itemtext = "";
      //if ( numtok > 4 )
      //  itemtext = input.gettok( 5, -1, " " );

      //TBBUTTONINFO tbbi;
      //ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
      //tbbi.cbSize = sizeof( TBBUTTONINFO );
      //tbbi.dwMask = TBIF_TEXT;
      //tbbi.pszText = itemtext.to_chr( );
      //this->setButtonInfo( this->getIndexToCommand( nButton ), &tbbi );
    }
  }
  // xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
  else if ( flags.switch_flags[22] && numtok > 5 ) {

    UINT iFlags = this->parseImageListFlags( input.gettok( 4, " " ) );

    HIMAGELIST himl;
    HICON icon;
    int index = atoi( input.gettok( 5, " ").to_chr( ) );
    TString filename = input.gettok( 6, -1, " " );

    // NORMAL IML
    if ( iFlags & TB_IML_NORMAL ) {

      //mIRCError( "Adding Normal Icon" );
      himl = this->getImageList( TB_IML_NORMAL );

      if ( himl != NULL ) {

        //TString error;
        //error.sprintf("Normal Icons : %d %X", ImageList_GetImageCount( himl ), himl );
        //mIRCError( error.to_chr() );

        int cx, cy;
        ImageList_GetIconSize( himl, &cx, &cy );

        if ( cx == 32 || cx == 24 )
          ExtractIconEx( filename.to_chr( ), index, &icon, 0, 1 );
        else
          ExtractIconEx( filename.to_chr( ), index, 0, &icon, 1 );

        ImageList_AddIcon( himl, icon );
        DestroyIcon( icon );

        //error.sprintf("Normal Icons : %d %X", ImageList_GetImageCount( himl ), himl );
        //mIRCError( error.to_chr() );
      }
    }
    // DISABLED IML
    if ( iFlags & TB_IML_DISABLE ) {

      //mIRCError( "Adding Disabled Icon" );
      himl = this->getImageList( TB_IML_DISABLE );

      if ( himl != NULL ) {

        //TString error;
        //error.sprintf("Disabled Icons : %d %X", ImageList_GetImageCount( himl ), himl );
        //mIRCError( error.to_chr() );

        int cx, cy;
        ImageList_GetIconSize( himl, &cx, &cy );

        if ( cx == 32 || cx == 24 )
          ExtractIconEx( filename.to_chr( ), index, &icon, 0, 1 );
        else
          ExtractIconEx( filename.to_chr( ), index, 0, &icon, 1 );

        ImageList_AddIcon( himl, icon );
        DestroyIcon( icon );

        //error.sprintf("Disabled Icons : %d %X", ImageList_GetImageCount( himl ), himl );
        //mIRCError( error.to_chr() );
      }
    }
    // HOT IML
    if ( iFlags & TB_IML_HOT ) {

      //mIRCError( "Adding Hot Icon" );
      himl = this->getImageList( TB_IML_HOT );

      if ( himl != NULL ) {

        //TString error;
        //error.sprintf("Hot Icons : %d %X", ImageList_GetImageCount( himl ), himl );
        //mIRCError( error.to_chr() );

        int cx, cy;
        ImageList_GetIconSize( himl, &cx, &cy );

        if ( cx == 32 || cx == 24 )
          ExtractIconEx( filename.to_chr( ), index, &icon, 0, 1 );
        else
          ExtractIconEx( filename.to_chr( ), index, 0, &icon, 1 );

        ImageList_AddIcon( himl, icon );
        DestroyIcon( icon );

        //error.sprintf("Hot Icons : %d %X", ImageList_GetImageCount( himl ), himl );
        //mIRCError( error.to_chr() );
      }
    }
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

UINT DcxToolBar::parseButtonStateFlags( TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = TBSTATE_ENABLED;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if ( flags[i] == 'd' )
      iFlags &= ~TBSTATE_ENABLED;
    else if ( flags[i] == 'h' )
      iFlags |= TBSTATE_HIDDEN;
    else if ( flags[i] == 'i' )
      iFlags |= TBSTATE_INDETERMINATE;
    else if ( flags[i] == 'p' )
      iFlags |= TBSTATE_PRESSED;
    else if ( flags[i] == 'x' )
      iFlags |= TBSTATE_CHECKED;
    else if ( flags[i] == 'w' )
      iFlags |= TBSTATE_WRAP;

    ++i;
  }
  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxToolBar::parseButtonStyleFlags( TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = BTNS_BUTTON;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if ( flags[i] == 'a' )
      iFlags |= BTNS_AUTOSIZE;
    else if ( flags[i] == 'k' )
      iFlags |= BTNS_CHECK;
    else if ( flags[i] == 'g' )
      iFlags |= BTNS_GROUP;
    else if ( flags[i] == 'v' )
      iFlags |= BTNS_DROPDOWN;
    else if ( flags[i] == 'l' )
      iFlags |= BTNS_WIDTH;
    else if ( flags[i] == 'b' )
      iFlags |= BTNS_BOLD;
    else if ( flags[i] == 'c' )
      iFlags |= BTNS_COLOR;
    else if ( flags[i] == 'u' )
      iFlags |= BTNS_UNDERLINE;
    else if ( flags[i] == 'x' )
      iFlags |= BTNS_TBKGCOLOR;
    else if ( flags[i] == 'z' )
      iFlags |= BTNS_TTXTCOLOR;

    ++i;
  }
  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxToolBar::parseImageListFlags( TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if ( flags[i] == 'n' )
      iFlags |= TB_IML_NORMAL;
    else if ( flags[i] == 'd' )
      iFlags |= TB_IML_DISABLE;
    else if ( flags[i] == 'h' )
      iFlags |= TB_IML_HOT;

    ++i;
  }
  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxToolBar::getImageList( int iImageList ) {

  if ( iImageList == TB_IML_NORMAL ) {
    return (HIMAGELIST) SendMessage( this->m_Hwnd, TB_GETIMAGELIST, (WPARAM) 0, (LPARAM) 0);
  }
  else if ( iImageList == TB_IML_DISABLE ) {
    return (HIMAGELIST) SendMessage( this->m_Hwnd, TB_GETDISABLEDIMAGELIST, (WPARAM) 0, (LPARAM) 0);
  }
  else if ( iImageList == TB_IML_HOT ) {
    return (HIMAGELIST) SendMessage( this->m_Hwnd, TB_GETHOTIMAGELIST, (WPARAM) 0, (LPARAM) 0);
  }
  return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxToolBar::setImageList( HIMAGELIST himl, int iImageList ) {

  if ( iImageList == TB_IML_NORMAL ) {
    SendMessage( this->m_Hwnd, TB_SETIMAGELIST, (WPARAM) 0, (LPARAM) himl );
  }
  else if ( iImageList == TB_IML_DISABLE ) {
    SendMessage( this->m_Hwnd, TB_SETDISABLEDIMAGELIST, (WPARAM) 0, (LPARAM) himl );
  }
  else if ( iImageList == TB_IML_HOT ) {
    SendMessage( this->m_Hwnd, TB_SETHOTIMAGELIST, (WPARAM) 0, (LPARAM) himl );
  }
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxToolBar::createImageList( UINT iSize ) {

  return ImageList_Create( iSize, iSize, ILC_COLOR32|ILC_MASK, 1, 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxToolBar::resetContent( ) {

  int nButtons = this->getButtonCount( );

  while ( nButtons-- )
    this->deleteButton( nButtons );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxToolBar::autoStretchButtons( ) {

  int nButtons = this->getButtonCount( );
  int nTotalButtons = nButtons;

  RECT rc, rcItem;
  int leftwidth;

  GetClientRect( this->m_Hwnd, &rc );
  leftwidth = rc.right - rc.left;

  TBBUTTON tbb;
  ZeroMemory( &tbb, sizeof( TBBUTTON ) );

  // Find the button width
  int i = 0;

  while ( i < nButtons ) {

    this->getButton( i, &tbb );

    if ( tbb.fsStyle & BTNS_SEP ) {

      //mIRCError( "Separator" );
      this->getItemRect( i, &rcItem );
      leftwidth -= rcItem.right - rcItem.left;
      --nTotalButtons;
    }
    ++i;
  }

  // Apply the new width
  TBBUTTONINFO tbbi;
  ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
  tbbi.cbSize = sizeof( TBBUTTONINFO );
  tbbi.dwMask = TBIF_SIZE;
  tbbi.cx = (int)( (double) leftwidth / (double) nTotalButtons );

  // Make sure the size is positive
  if ( tbbi.cx > 0 ) {
    i = 0;

    while ( i < nButtons ) {

      this->getButton( i, &tbb );

      if ( tbb.fsStyle & BTNS_SEP ) {
        ++i;
        continue;
      }

      this->setButtonInfo( this->getIndexToCommand( i ), &tbbi );

      ++i;
    }
  }
}

void DcxToolBar::autoPosition( int width, int height ) {

  if ( !this->isStyle( CCS_NOPARENTALIGN | CCS_NORESIZE ) ) {

    SendMessage( this->m_Hwnd, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
  }
  /*
  
  if ( this->isStyle( CCS_NOPARENTALIGN ) && !this->isStyle( CCS_NORESIZE ) ) {

    mIRCError( "this->isStyle( CCS_NOPARENTALIGN ) && !this->isStyle( CCS_NORESIZE )" );

    RECT rc, rc2;
    GetWindowRect( this->m_Hwnd, &rc );
    GetWindowRect( GetParent( this->m_Hwnd ), &rc2 );
    OffsetRect( &rc, -rc2.left, -rc2.top );

    if ( this->isStyle( CCS_VERT ) ) {

      SetWindowPos( this->m_Hwnd, NULL, rc.left, rc.top, rc.right - rc.left, (rc2.bottom - rc2.top) - rc.top, SWP_NOZORDER );

    }
    else {

      SetWindowPos( this->m_Hwnd, NULL, rc.left, rc.top, (rc2.right - rc2.left) - rc.left, rc.bottom - rc.top, SWP_NOZORDER );
    }
  }
  else if ( !this->isStyle( CCS_NOPARENTALIGN ) ) {

    mIRCError( "!this->isStyle( CCS_NOPARENTALIGN )" );
    SendMessage( this->m_Hwnd, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
  }
  */
  /*

  if ( !this->isStyle( CCS_NOPARENTALIGN | CCS_NORESIZE ) ) {

    mIRCError( "No alignement styles" );
  }
  else if ( this->isStyle( CCS_NOPARENTALIGN ) ) {

    mIRCError( "Alignement NOPARENT styles" );
  }
  else if ( this->isStyle( CCS_NORESIZE ) ) {

    mIRCError( "Alignement NORESIZE styles" );
    
  }
  */
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::autoSize( ) {
  return SendMessage( this->m_Hwnd, TB_AUTOSIZE, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::insertButton( int nPos, LPTBBUTTON lpbb ) {
  return SendMessage( this->m_Hwnd, TB_INSERTBUTTON, (WPARAM) nPos, (LPARAM) lpbb );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::hitTest( LPPOINT lpt ) {
  return SendMessage( this->m_Hwnd, TB_HITTEST, (WPARAM) 0, (LPARAM) lpt );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::getItemRect( int iButton, LPRECT lprc ) {
  return SendMessage( this->m_Hwnd, TB_GETITEMRECT, (WPARAM) iButton, (LPARAM) lprc );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::getButtonCount( ) {
  return SendMessage( this->m_Hwnd, TB_BUTTONCOUNT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::setButtonWidth( int cxMin, int cxMax ) {
  return SendMessage( this->m_Hwnd, TB_SETBUTTONWIDTH, (WPARAM) 0, (LPARAM) MAKELONG ( cxMin, cxMax ) );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::deleteButton(int iButton) {
  return (LRESULT) SendMessage( this->m_Hwnd, TB_DELETEBUTTON, (WPARAM) iButton, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::setButtonInfo( int idButton, LPTBBUTTONINFO lpbi ) {
  return SendMessage( this->m_Hwnd, TB_SETBUTTONINFO, (WPARAM) idButton, (LPARAM) lpbi );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::getButtonInfo( int idButton, LPTBBUTTONINFO lpbi ) {
	return SendMessage(this->m_Hwnd, TB_GETBUTTONINFO, (WPARAM) idButton, (LPARAM) lpbi);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::getTooltips( ) {
  return SendMessage( this->m_Hwnd, TB_GETTOOLTIPS, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::getButtonText( int idButton, LPSTR lpszText ) {
  return (LRESULT) SendMessage( this->m_Hwnd, TB_GETBUTTONTEXT, (WPARAM) idButton, (LPARAM) lpszText );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::getButton( int iButton, LPTBBUTTON lpb ) {
  return SendMessage( this->m_Hwnd, TB_GETBUTTON, (WPARAM) iButton, (LPARAM) lpb );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::getCommandToIndex( int idButton ) {
  return SendMessage( this->m_Hwnd, TB_COMMANDTOINDEX, (WPARAM) idButton, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::moveButton( int iButtonFrom, int iButtonTo ) {
  return SendMessage( this->m_Hwnd, TB_MOVEBUTTON, (WPARAM) iButtonFrom, (LPARAM) iButtonTo );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::markButton( int iButton, bool fHighlight ) {
  return SendMessage( this->m_Hwnd, TB_MARKBUTTON, (WPARAM) iButton, (LPARAM) MAKELONG ( fHighlight, 0 ) );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::setButtonState( int idButton, UINT fState ) {
  return SendMessage( this->m_Hwnd, TB_SETSTATE, (WPARAM) idButton, (LPARAM) MAKELONG ( fState, 0 ) );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::setButtonSize( int dxButton, int dyButton ) {
  return SendMessage( this->m_Hwnd, TB_SETBUTTONSIZE, (WPARAM) 0, (LPARAM) MAKELONG (dxButton, dyButton) );
}


/*!
 * \brief blah
 *
 * blah
 */

int DcxToolBar::getFreeButtonID( ) {

  int i = 1;
  int iID = this->getCommandToIndex( i );
  while ( iID != -1 ) {
    i++;
    iID = this->getCommandToIndex( i );
  }
  return i;
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxToolBar::getIndexToCommand(int iIndex) {
	int i = 1, nButtons = this->getButtonCount();

	while (i <= nButtons) {
		if (this->getCommandToIndex(i) == iIndex)
			return i;

		i++;
	}

	return 0;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxToolBar::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_COMMAND:
      {

        //int iButton = (int) this->hitTest( &pt );

        int iButton = (int) wParam;
        
        if ( iButton > -1 ) {
          /*
          RECT rc;
          this->getItemRect( this->getCommandToIndex( iButton ), &rc );
          POINT pt2 = pt;
          pt.x = rc.left; 
          pt.y = rc.bottom;
          pt2.x = rc.left;
          pt2.y = rc.top;
          ClientToScreen( this->m_Hwnd, &pt );
          ClientToScreen( this->m_Hwnd, &pt2 );
          
          this->callAliasEx( NULL, "%s,%d,%d,%d,%d,%d,%d", "sclick", 
            this->getUserID( ), iButton+1, pt.x, pt.y, pt2.x, pt2.y );
            */
          this->callAliasEx( NULL, "%s,%d,%d", "sclick", 
            this->getUserID( ), this->getCommandToIndex( iButton ) + 1 );
        }

        bParsed = TRUE;
      }
      break;

    case WM_NOTIFY : 
      {
        //mIRCError( "Control WM_NOTIFY" );

        LPNMHDR hdr = (LPNMHDR) lParam;
        if (!hdr)
          break;

        switch( hdr->code ) {

          /*
          case NM_CLICK:
            {

              
              LPNMMOUSE lpnm = (LPNMMOUSE) lParam;
              POINT pt = lpnm->pt;
              int iButton = (int) this->hitTest( &pt );

              if ( iButton > -1 ) {

                char ret[256];
                RECT rc;
                this->getItemRect( iButton, &rc );
                POINT pt2 = pt;
                pt.x = rc.left; 
                pt.y = rc.bottom;
                pt2.x = rc.left;
                pt2.y = rc.top;
                ClientToScreen( this->m_Hwnd, &pt );
                ClientToScreen( this->m_Hwnd, &pt2 );
                this->callAliasEx( ret, "%s,%d,%d,%d,%d,%d,%d", "sclick", 
                  this->getUserID( ), iButton+1, pt.x, pt.y, pt2.x, pt2.y );
              }

              bParsed = TRUE;
            }
            break;
            */

          case NM_RCLICK:
            {

              LPNMMOUSE lpnm = (LPNMMOUSE) lParam;
              POINT pt = lpnm->pt;
              int iButton = (int) this->hitTest( &pt );

              if ( iButton > -1 ) {
                RECT rc;
                this->getItemRect( iButton, &rc );
                POINT pt2 = pt;
                pt.x = rc.left; 
                pt.y = rc.bottom;
                pt2.x = rc.left;
                pt2.y = rc.top;
                ClientToScreen( this->m_Hwnd, &pt );
                ClientToScreen( this->m_Hwnd, &pt2 );
                this->callAliasEx( NULL, "%s,%d,%d,%d,%d,%d,%d", "rclick", 
                  this->getUserID( ), iButton+1, pt.x, pt.y, pt2.x, pt2.y );
              }

              bParsed = TRUE;
            }
            break;

           case TBN_DROPDOWN:
            {

              LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR) lParam;
              POINT pt;
              GetCursorPos( &pt );
              ScreenToClient( this->m_Hwnd, &pt );
              int iButton = (int) this->hitTest( &pt );

              if ( iButton > -1 ) {
                RECT rc;
                this->getItemRect( iButton, &rc );
                POINT pt2 = pt;
                pt.x = rc.left; 
                pt.y = rc.bottom;
                pt2.x = rc.left;
                pt2.y = rc.top;
                ClientToScreen( this->m_Hwnd, &pt );
                ClientToScreen( this->m_Hwnd, &pt2 );
                this->callAliasEx( NULL, "%s,%d,%d,%d,%d,%d,%d", "dropdown", 
                  this->getUserID( ), iButton+1, pt.x, pt.y, pt2.x, pt2.y );
              }

              bParsed = TRUE;
              return TBDDRET_DEFAULT;
            }
	          break;

          case NM_CUSTOMDRAW:
            {
              LPNMTBCUSTOMDRAW lpntbcd = (LPNMTBCUSTOMDRAW) lParam;

              bParsed = TRUE;
              switch( lpntbcd->nmcd.dwDrawStage ) {

                case CDDS_PREPAINT:
                  return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);

                case CDDS_ITEMPREPAINT:
                  {
                    
                    LPDCXTBBUTTON lpdtbb = (LPDCXTBBUTTON) lpntbcd->nmcd.lItemlParam;

										if ( lpdtbb == NULL )
                      return CDRF_DODEFAULT;

//mIRCError("getting tt");


                    if ( lpdtbb->clrText != -1 )
                      lpntbcd->clrText = lpdtbb->clrText;

                    HFONT hFont = (HFONT) SendMessage( this->m_Hwnd, WM_GETFONT, 0, 0 );

//mIRCError("getting hfont");

                    LOGFONT lf;
                    GetObject( hFont, sizeof(LOGFONT), &lf );
//mIRCError("getting logfont");
                    if ( lpdtbb->bBold )
                      lf.lfWeight |= FW_BOLD;
                    else
                      lf.lfWeight &= ~FW_BOLD;
//mIRCError("getting uline");
                    if ( lpdtbb->bUline )
                      lf.lfUnderline = TRUE;
                    else
                      lf.lfUnderline = FALSE;
//mIRCError("getting new font");
                    HFONT hFontNew = CreateFontIndirect( &lf );
                    HFONT hOldFont = (HFONT) SelectObject( lpntbcd->nmcd.hdc, hFontNew );
//mIRCError("getting blah");
                    DeleteObject(hFontNew);
//mIRCError("getting delete");
                  }
//mIRCError("getting ret");
                  return (CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT);

                case CDDS_ITEMPOSTPAINT:
                  return CDRF_DODEFAULT;

                default:
                  return CDRF_DODEFAULT;
              }
            }
            break;

          case TBN_GETINFOTIP:
            {
              //mIRCError( "Control WM_NOTIFY - TBN_GETINFOTIP" );

              LPNMTBGETINFOTIP tcgit = (LPNMTBGETINFOTIP) lParam;
              if ( tcgit != NULL ) {

                //mIRCError( "Toolbar Tooltips Here2!" );
                LPDCXTBBUTTON lpdtbb = (LPDCXTBBUTTON) tcgit->lParam;

                if ( lpdtbb != NULL ) {

                  //mIRCError( "Toolbar Tooltips Here3!" );
									//mIRCDebug("tip: %s", lpdtbb->tsTipText.to_chr( ));
									if (tcgit->pszText != NULL) {
										lstrcpyn(tcgit->pszText,lpdtbb->tsTipText.to_chr(), tcgit->cchTextMax);
									}
                }
					 //mIRCError("ugh!!!!");
              }
				  //mIRCError("parsed");
              bParsed = TRUE;
            }
            break;

          case TBN_DELETINGBUTTON:
            {
              //mIRCError( "Control WM_NOTIFY - TBN_DELETINGBUTTON" );

              LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR) lParam;
              TBBUTTON tbb;
              ZeroMemory( &tbb, sizeof(TBBUTTON) );

              this->getButton( lpnmtb->iItem -1, &tbb );
							if ( tbb.dwData != NULL ) {
                delete (LPDCXTBBUTTON) tbb.dwData;
							}

              bParsed = TRUE;
            }
            break;

        } // switch
      }
      break;

    case WM_SIZE:
      {
        
        if ( this->m_bAutoStretch )
          this->autoStretchButtons( );        
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