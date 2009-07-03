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
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxToolBar::DcxToolBar( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles )
: DcxControl( ID, p_Dialog )
, m_hItemFont(NULL)
, m_hOldItemFont(NULL)
, m_bOverrideTheme(false)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles,
		DCX_TOOLBARCLASS,
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

	if ( ExStyles != 0 )
		SendMessage( this->m_Hwnd, TB_SETEXTENDEDSTYLE, (WPARAM) 0, (LPARAM) ExStyles );

	SendMessage( this->m_Hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), (LPARAM) 0 );
	this->m_ToolTipHWND = (HWND)SendMessage( this->m_Hwnd, TB_GETTOOLTIPS, NULL, NULL);
	if (styles.istok("balloon") && this->m_ToolTipHWND != NULL) {
		SetWindowLong(this->m_ToolTipHWND,GWL_STYLE,GetWindowLong(this->m_ToolTipHWND,GWL_STYLE) | TTS_BALLOON);
	}
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
	unsigned int i = 1, numtok = styles.numtok( );

	while ( i <= numtok ) {

		if ( styles.gettok( i ) == "flat" )
			*Styles |= TBSTYLE_FLAT;
		else if ( styles.gettok( i ) == "tooltips" )
			*Styles |= TBSTYLE_TOOLTIPS;
		else if ( styles.gettok( i ) == "transparent" )
			*Styles |= TBSTYLE_TRANSPARENT;
		else if ( styles.gettok( i ) == "nodivider" )
			*Styles |= CCS_NODIVIDER;
		else if ( styles.gettok( i ) == "top" )
			*Styles |= CCS_TOP;
		else if ( styles.gettok( i ) == "bottom" )
			*Styles |= CCS_BOTTOM;
		else if ( styles.gettok( i ) == "left" )
			*Styles |= CCS_LEFT;
		else if ( styles.gettok( i ) == "right" )
			*Styles |= CCS_RIGHT;
		//else if ( styles.gettok( i ) == "noresize" ) 
		//  *Styles |= CCS_NORESIZE;
		//else if ( styles.gettok( i ) == "noparentalign" ) 
		//  *Styles |= CCS_NOPARENTALIGN ;
		else if ( styles.gettok( i ) == "noauto" )
			*Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
		else if ( styles.gettok( i ) == "adjustable" )
			*Styles |= CCS_ADJUSTABLE;
		else if ( styles.gettok( i ) == "list" )
			*Styles |= TBSTYLE_LIST;
		else if ( styles.gettok( i ) == "wrap" )
			*Styles |= TBSTYLE_WRAPABLE;
		else if ( styles.gettok( i ) == "arrows" )
			*ExStyles |= TBSTYLE_EX_DRAWDDARROWS;
		else if ( styles.gettok( i ) == "override" )
			this->m_bOverrideTheme = true;

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

	int numtok = input.numtok( );
	TString prop(input.gettok( 3 ));

	// [NAME] [ID] [PROP]
	if ( prop == "num" ) {
		wsprintf( szReturnValue, "%d", this->getButtonCount( ) );
		return;
	}
	// [NAME] [ID] [PROP]
	else if (prop == "mouseitem") {
		long lResult = SendMessage(this->m_Hwnd, TB_GETHOTITEM, NULL, NULL);

		wsprintf(szReturnValue, "%d", lResult);
		return;
	}
	// [NAME] [ID] [PROP]
	else if ( prop == "text" && numtok > 3 ) {

		int nButton = input.gettok( 4 ).to_int( ) - 1;

		if ( nButton > -1 && nButton < this->getButtonCount( ) ) {

			// This way fails to give the correct result after buttons have been removed.
			//this->getButtonText( this->getIndexToCommand( nButton ), szReturnValue ); // possible overflow, needs fixing at some point.
			TBBUTTONINFO tbbi;
			ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
			tbbi.cbSize = sizeof( TBBUTTONINFO );
			tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;
			this->getButtonInfo( nButton, &tbbi );

			LPDCXTBBUTTON lpdcxtbb = (LPDCXTBBUTTON) tbbi.lParam;

			if ( lpdcxtbb != NULL )
				lstrcpyn( szReturnValue, lpdcxtbb->bText.to_chr( ), 900 );
			return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == "icon" && numtok > 3 ) {

		int iButton = input.gettok( 4 ).to_int( ) - 1;

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
	else if ( prop == "state" && numtok > 3 ) {

		int iButton = input.gettok( 4 ).to_int( ) - 1;

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
	else if ( prop == "tooltip" && numtok > 3 ) {

		int iButton = input.gettok( 4 ).to_int( ) - 1;

		if ( iButton > -1 && iButton < this->getButtonCount( ) ) {

			TBBUTTONINFO tbbi;
			ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
			tbbi.cbSize = sizeof( TBBUTTONINFO );
			tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;
			this->getButtonInfo( iButton, &tbbi );

			LPDCXTBBUTTON lpdcxtbb = (LPDCXTBBUTTON) tbbi.lParam;

			if ( lpdcxtbb != NULL )
				lstrcpyn( szReturnValue, lpdcxtbb->tsTipText.to_chr( ), 900 );
			return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if (prop == "dropdownpoint" && numtok > 3) {
		RECT rc;
		TString coordinates;

		ZeroMemory(&rc, sizeof(RECT));
		int iButton = input.gettok(4).to_int() -1;

		// out of range;
		if ((iButton < 0) || (iButton >= this->getButtonCount())) {
			this->showError("dropdownpoint", NULL, "Index out of range.");
			return;
		}

		this->getItemRect(iButton, &rc);
		MapWindowPoints(this->m_Hwnd, NULL, (LPPOINT)&rc, 2);
		coordinates.sprintf("%d %d %d %d", rc.left, rc.bottom, rc.right, rc.top);

		lstrcpy(szReturnValue, coordinates.to_chr());
		return;
	}
	else if (this->parseGlobalInfoRequest(input, szReturnValue))
		return;

	szReturnValue[0] = 0;
}

/*!
* \brief blah
*
* blah
*/

void DcxToolBar::parseCommandRequest( TString & input ) {
	XSwitchFlags flags(input.gettok(3));
	int numtok = input.numtok( );

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags['r']) {
		this->resetContent();
	}

	// xdid -a [NAME] [ID] [SWITCH] [N] [+FLAGS] [WIDTH] [#ICON] [COLOR] (Button Text) [TAB] Tooltip Text
	if ( flags['a'] && numtok > 4 ) {

		int nPos = input.gettok( 4 ).to_int( ) - 1;

		if ( nPos == -1 )
			nPos += this->getButtonCount( ) + 1;

		TString flags(input.gettok( 5 ));
		int width = input.gettok( 6 ).to_int( );
		int icon = input.gettok( 7 ).to_int( ) - 1;
		COLORREF clrText = (COLORREF) input.gettok( 8 ).to_num( );
		int numtok = input.gettok( 1, TSTAB ).numtok( );

		TBBUTTON tbb;
		ZeroMemory( &tbb, sizeof( TBBUTTON ) );

		tbb.fsState = parseButtonStateFlags( flags );
		tbb.idCommand = this->getFreeButtonID( );
		UINT buttonStyles = parseButtonStyleFlags( flags );
		tbb.fsStyle = buttonStyles;

		if (( icon == -1 ) || (numtok < 7))
			tbb.iBitmap = I_IMAGENONE;
		else
			tbb.iBitmap = icon;

		TString itemtext;

		if ( numtok > 8 ) {

			itemtext = input.gettok( 1, TSTAB ).gettok( 9, -1 ).trim();

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

		if ( input.numtok( TSTAB ) > 1 )
			lpdcxtbb->tsTipText = input.gettok( 2, -1, TSTAB ).trim();

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
		lpdcxtbb->clrBtnFace = -1;
		lpdcxtbb->clrBtnHighlight = -1;
		lpdcxtbb->clrHighlightHotTrack = -1;
		lpdcxtbb->clrMark = -1;
		lpdcxtbb->clrTextHighlight = -1;
		lpdcxtbb->iTextBkgMode = TRANSPARENT;
		lpdcxtbb->iTextHighlightBkgMode = TRANSPARENT;

		tbb.dwData = (LPARAM) lpdcxtbb;
		lpdcxtbb->bText = itemtext;
		tbb.iString = (INT_PTR)lpdcxtbb->bText.to_chr();

		// insert button
		this->insertButton( nPos, &tbb );
		if (nPos == 1)
			SendMessage(this->m_Hwnd, TB_CHANGEBITMAP, nPos, MAKELPARAM(I_IMAGENONE,0));

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
	// xdid -c [NAME] [ID] [SWITCH] [N] [+FLAGS] [RGB] [+REMOVEFLAGS]
	else if ( flags['c'] && numtok > 5 ) {

		int nButton = input.gettok( 4 ).to_int( ) - 1;
		UINT buttonStyles = parseButtonStyleFlags( input.gettok( 5 ) );
		UINT removeButtonStyles = parseButtonStyleFlags( input.gettok( 7 ) );
		COLORREF clrColor = (COLORREF)input.gettok( 6 ).to_num( );

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
			tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;

			if ( this->getButtonInfo( nButton, &tbbi ) != -1 ) {

				LPDCXTBBUTTON lpdcxtbb = (LPDCXTBBUTTON) tbbi.lParam;

				if ( buttonStyles & BTNS_UNDERLINE )
					lpdcxtbb->bUline = TRUE;

				if ( buttonStyles & BTNS_BOLD )
					lpdcxtbb->bBold = TRUE;

				if ( buttonStyles & BTNS_COLOR )
					lpdcxtbb->clrText = clrColor;

				if ( buttonStyles & BTNS_HIGHLIGHT_TXTCOLOR )
					lpdcxtbb->clrTextHighlight = clrColor;

				if ( buttonStyles & BTNS_MARK_BKGCOLOR )
					lpdcxtbb->clrMark = clrColor;

				if ( buttonStyles & BTNS_BTNCOLOR )
					lpdcxtbb->clrBtnFace = clrColor;

				if ( buttonStyles & BTNS_HIGHLIGHT_BTNCOLOR )
					lpdcxtbb->clrBtnHighlight = clrColor;

				if ( buttonStyles & BTNS_HOTTRACK_BTNCOLOR )
					lpdcxtbb->clrHighlightHotTrack = clrColor;

				if ( removeButtonStyles & BTNS_UNDERLINE )
					lpdcxtbb->bUline = FALSE;

				if ( removeButtonStyles & BTNS_BOLD )
					lpdcxtbb->bBold = FALSE;

				if ( removeButtonStyles & BTNS_COLOR )
					lpdcxtbb->clrText = -1;

				if ( removeButtonStyles & BTNS_HIGHLIGHT_TXTCOLOR )
					lpdcxtbb->clrTextHighlight = -1;

				if ( removeButtonStyles & BTNS_MARK_BKGCOLOR )
					lpdcxtbb->clrMark = -1;

				if ( removeButtonStyles & BTNS_BTNCOLOR )
					lpdcxtbb->clrBtnFace = -1;

				if ( removeButtonStyles & BTNS_HIGHLIGHT_BTNCOLOR )
					lpdcxtbb->clrBtnHighlight = -1;

				if ( removeButtonStyles & BTNS_HOTTRACK_BTNCOLOR )
					lpdcxtbb->clrHighlightHotTrack = -1;

				this->redrawWindow( );
			}
		}
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if ( flags['d'] && numtok > 3 ) {
		int nButton = input.gettok( 4 ).to_int( ) - 1;

		if (nButton > -1)
			this->deleteButton(nButton);
	}
	// xdid -i [NAME] [ID] [SWITCH] [N] [IMAGE]
	else if ( flags['i'] && numtok > 4 ) {

		int nButton = input.gettok( 4 ).to_int( ) - 1;
		int iImage = input.gettok( 5 ).to_int( ) - 1;

		TBBUTTONINFO tbbi;
		ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
		tbbi.cbSize = sizeof( TBBUTTONINFO );
		tbbi.dwMask = TBIF_IMAGE | TBIF_BYINDEX;
		tbbi.iImage = iImage;

		this->setButtonInfo( nButton /*this->getIndexToCommand( nButton )*/, &tbbi );
	}
	// xdid -j [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if ( flags['j'] && numtok > 4 ) {

		int nMin = input.gettok( 4 ).to_int( );
		int nMax = input.gettok( 4 ).to_int( );

		this->setButtonWidth( nMin, nMax );
	}
	// xdid -l [NAME] [ID] [SWITCH] [SIZE]
	else if (flags['l'] && numtok > 3) {
		HIMAGELIST himl;

		int size = input.gettok( 4 ).to_int();

		// these are destroyed after being removed from toolbar by setImageList() function.
		//ImageList_Destroy(this->getImageList(TB_IML_NORMAL));
		//ImageList_Destroy(this->getImageList(TB_IML_DISABLE));
		//ImageList_Destroy(this->getImageList(TB_IML_HOT));

		if (size != 32 && size != 24)
			size = 16;

		himl = this->createImageList(size);
		this->setImageList(himl, TB_IML_NORMAL);

		himl = this->createImageList(size);
		this->setImageList(himl, TB_IML_DISABLE);

		himl = this->createImageList(size);
		this->setImageList(himl, TB_IML_HOT);

		this->redrawWindow();
	}
	// xdid -m [NAME] [ID] [SWITCH] [1|0]
	else if ( flags['m'] && numtok > 3 ) {

		if ( input.gettok( 4 ) == "1" ) {

			this->m_bAutoStretch = TRUE;
			this->autoStretchButtons( );
		}
		else
			this->m_bAutoStretch = FALSE;
	}
	// xdid -q [NAME] [ID] [SWITCH] [N] (TIPTEXT)
	else if (flags['q'] && numtok > 3) {
		int nButton = input.gettok( 4 ).to_int() -1;

		if (nButton > -1 && nButton < this->getButtonCount()) {
			//int idButton = this->getIndexToCommand(nButton);
			TBBUTTONINFO tbbi;

			ZeroMemory(&tbbi, sizeof(TBBUTTONINFO));
			tbbi.cbSize = sizeof(TBBUTTONINFO);
			tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;

			if (this->getButtonInfo(nButton, &tbbi) > -1) {
				LPDCXTBBUTTON lpdcxtbb = (LPDCXTBBUTTON) tbbi.lParam;

				if (numtok > 4)	// has a new tooltip
					lpdcxtbb->tsTipText = input.gettok(5, -1);
				else					// no tooltip
					lpdcxtbb->tsTipText = "";
			}
		}
	}
	// This is to avoid an invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags['r']) {
	}
	// xdid -t [NAME] [ID] [SWITCH] [N] [+FLAGS]
	else if ( flags['t'] && numtok > 4 ) {

		int nButton = input.gettok( 4 ).to_int( ) - 1;
		UINT fStates = parseButtonStateFlags( input.gettok( 5 ) );

		int idButton = this->getIndexToCommand( nButton );

		if ( idButton > 0 )
			this->setButtonState( idButton, fStates );
	}
	// xdid -u [NAME] [ID] [SWITCH] [DX] [DY]
	else if ( flags['u'] && numtok > 4 ) {

		int dxButton = input.gettok( 4 ).to_int( );
		int dyButton = input.gettok( 5 ).to_int( );

		this->setButtonSize( dxButton, dyButton );
	}
	// xdid -v [NAME] [ID] [SWITCH] [N] (TEXT)
	else if ( flags['v'] && numtok > 3 ) {

		int nButton = input.gettok( 4 ).to_int( ) - 1;
		if ( nButton > -1 && nButton < this->getButtonCount( ) ) {
			int nIndex = this->getIndexToCommand(nButton);
			TBBUTTONINFO tbbi;

			ZeroMemory(&tbbi, sizeof(TBBUTTONINFO));
			tbbi.cbSize = sizeof(TBBUTTONINFO);
			tbbi.dwMask = TBIF_LPARAM;
			if (this->getButtonInfo(nIndex, &tbbi) > -1) {
				LPDCXTBBUTTON lpdcxtbb = (LPDCXTBBUTTON) tbbi.lParam;
				if ( numtok > 4 )
					lpdcxtbb->bText = input.gettok( 5, -1 );
				else
					lpdcxtbb->bText = "";
				ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
				tbbi.cbSize = sizeof( TBBUTTONINFO );
				tbbi.dwMask = TBIF_TEXT;
				tbbi.pszText = lpdcxtbb->bText.to_chr();
				this->setButtonInfo(nIndex, &tbbi);
			}
		}
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags['w'] && numtok > 5) {
		UINT iFlags = this->parseImageListFlags(input.gettok( 4 ));

		if (input.gettok( 4 )[0] != '+') {
			this->showError(NULL, "-w", "Invalid Flags");
			return;
		}

		HIMAGELIST himl;
		HICON icon = NULL;
		int index = input.gettok( 5 ).to_int();
		TString filename(input.gettok(6, -1));

		himl = this->getImageList(TB_IML_NORMAL);

		// load the icon
		if (himl != NULL) {
			int cx, cy;
			ImageList_GetIconSize(himl, &cx, &cy);

			if (cx > 16)
				icon = dcxLoadIcon(index, filename, true, input.gettok( 4 ));
			else
				icon = dcxLoadIcon(index, filename, false, input.gettok( 4 ));

			// Grayscale the icon
			//if ((iFlags & TB_ICO_GREY) && icon)
			//	icon = CreateGrayscaleIcon(icon);
		}
		else
			this->showError(NULL, "-w", "Unable to get Normal Image List");

		// if there is an icon to process
		if (icon) {
			// NORMAL IML
			if (iFlags & TB_IML_NORMAL) {
				himl = this->getImageList(TB_IML_NORMAL);

				if (himl != NULL)
					ImageList_AddIcon(himl, icon);
			}

			// DISABLED IML
			if (iFlags & TB_IML_DISABLE) {
				himl = this->getImageList(TB_IML_DISABLE);

				if (himl != NULL)
					ImageList_AddIcon(himl, icon);
			}

			// HOT IML
			if (iFlags & TB_IML_HOT) {
				himl = this->getImageList(TB_IML_HOT);

				if (himl != NULL)
					ImageList_AddIcon(himl, icon);
			}

			DestroyIcon(icon);
		}
		else
			this->showErrorEx(NULL, "-w", "Icon Failed To Load: %s", filename.to_chr());
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
* \brief blah
*
* blah
*/

UINT DcxToolBar::parseButtonStateFlags( const TString & flags ) {

	INT i = 1, len = (INT)flags.len( ), iFlags = TBSTATE_ENABLED;

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

UINT DcxToolBar::parseButtonStyleFlags( const TString & flags ) {

	INT i = 1, len = (INT)flags.len( ), iFlags = TBSTYLE_BUTTON; //BTNS_BUTTON;

	// no +sign, missing params
	if ( flags[0] != '+' ) 
		return iFlags;

	while ( i < len ) {

		switch (flags[i])
		{
		case 'a':
			iFlags |= TBSTYLE_AUTOSIZE; //BTNS_AUTOSIZE;
			break;
		case 'b':
			iFlags |= BTNS_BOLD;
			break;
		case 'B':
			iFlags |= BTNS_BTNCOLOR;
			break;
		case 'c':
			iFlags |= BTNS_COLOR;
			break;
		case 'g':
			iFlags |= TBSTYLE_GROUP; //BTNS_GROUP;
			break;
		case 'h':
			iFlags |= BTNS_HIGHLIGHT_TXTCOLOR;
			break;
		case 'H':
			iFlags |= BTNS_HIGHLIGHT_BTNCOLOR;
			break;
		case 'k':
			iFlags |= TBSTYLE_CHECK; //BTNS_CHECK;
			break;
		case 'l':
			iFlags |= BTNS_WIDTH;
			break;
		case 'm':
			iFlags |= BTNS_MARK_BKGCOLOR;
			break;
		case 'T':
			iFlags |= BTNS_HOTTRACK_BTNCOLOR;
			break;
		case 'u':
			iFlags |= BTNS_UNDERLINE;
			break;
		case 'v':
			iFlags |= TBSTYLE_DROPDOWN; //BTNS_DROPDOWN;
			break;
		case 'x':
			iFlags |= BTNS_TBKGCOLOR;
			break;
		case 'z':
			iFlags |= BTNS_TTXTCOLOR;
			break;
		}

		++i;
	}
	return iFlags;
}

/*!
* \brief blah
*
* blah
*/
UINT DcxToolBar::parseImageListFlags( const TString &flags) {
	INT i = 1, len = (INT)flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+')
		return iFlags;

	while (i < len) {
		if (flags[i] == 'n')
			iFlags |= TB_IML_NORMAL;
		else if (flags[i] == 'd')
			iFlags |= TB_IML_DISABLE;
		else if (flags[i] == 'h')
			iFlags |= TB_IML_HOT;
		else if (flags[i] == 'g')
			iFlags |= TB_ICO_GREY;

		++i;
	}

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxToolBar::getImageList( const int iImageList ) {

	if ( iImageList == TB_IML_NORMAL )
		return (HIMAGELIST) SendMessage( this->m_Hwnd, TB_GETIMAGELIST, (WPARAM) 0, (LPARAM) 0);
	else if ( iImageList == TB_IML_DISABLE )
		return (HIMAGELIST) SendMessage( this->m_Hwnd, TB_GETDISABLEDIMAGELIST, (WPARAM) 0, (LPARAM) 0);
	else if ( iImageList == TB_IML_HOT )
		return (HIMAGELIST) SendMessage( this->m_Hwnd, TB_GETHOTIMAGELIST, (WPARAM) 0, (LPARAM) 0);
	return NULL;
}

/*!
* \brief blah
*
* blah
*/

void DcxToolBar::setImageList(HIMAGELIST himl, const int iImageList) {
	HIMAGELIST himlOld = NULL;

	if (iImageList == TB_IML_NORMAL)
		himlOld = (HIMAGELIST) SendMessage(this->m_Hwnd, TB_SETIMAGELIST, (WPARAM) NULL, (LPARAM) himl);
	else if (iImageList == TB_IML_DISABLE)
		himlOld = (HIMAGELIST) SendMessage(this->m_Hwnd, TB_SETDISABLEDIMAGELIST, (WPARAM) NULL, (LPARAM) himl);
	else if (iImageList == TB_IML_HOT)
		himlOld = (HIMAGELIST) SendMessage(this->m_Hwnd, TB_SETHOTIMAGELIST, (WPARAM) NULL, (LPARAM) himl);

	if (himlOld)
		ImageList_Destroy(himlOld);
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxToolBar::createImageList( const UINT iSize ) {

	return ImageList_Create(iSize, iSize, ILC_COLOR32 | ILC_MASK, 1, 0);
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

void DcxToolBar::autoPosition( const int width, const int height ) {

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

LRESULT DcxToolBar::insertButton( const int nPos, const LPTBBUTTON lpbb ) {
	return SendMessage( this->m_Hwnd, TB_INSERTBUTTON, (WPARAM) nPos, (LPARAM) lpbb );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::hitTest( const LPPOINT lpt ) const {
	return SendMessage( this->m_Hwnd, TB_HITTEST, (WPARAM) 0, (LPARAM) lpt );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getItemRect( const int iButton, LPRECT lprc ) const {
	return SendMessage( this->m_Hwnd, TB_GETITEMRECT, (WPARAM) iButton, (LPARAM) lprc );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getButtonCount( ) const {
	return SendMessage( this->m_Hwnd, TB_BUTTONCOUNT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::setButtonWidth( const int cxMin, const int cxMax ) {
	return SendMessage( this->m_Hwnd, TB_SETBUTTONWIDTH, (WPARAM) 0, (LPARAM) MAKELONG ( cxMin, cxMax ) );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::deleteButton(const int iButton) {
	return (LRESULT) SendMessage( this->m_Hwnd, TB_DELETEBUTTON, (WPARAM) iButton, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::setButtonInfo( const int idButton, LPTBBUTTONINFO lpbi ) {
	return SendMessage( this->m_Hwnd, TB_SETBUTTONINFO, (WPARAM) idButton, (LPARAM) lpbi );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getButtonInfo( const int idButton, LPTBBUTTONINFO lpbi ) const {
	return SendMessage(this->m_Hwnd, TB_GETBUTTONINFO, (WPARAM) idButton, (LPARAM) lpbi);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getTooltips( ) const {
	return SendMessage( this->m_Hwnd, TB_GETTOOLTIPS, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getButtonText( const int idButton, LPSTR lpszText ) const {
	return (LRESULT) SendMessage( this->m_Hwnd, TB_GETBUTTONTEXT, (WPARAM) idButton, (LPARAM) lpszText );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getButton( const int iButton, LPTBBUTTON lpb ) const {
	return SendMessage( this->m_Hwnd, TB_GETBUTTON, (WPARAM) iButton, (LPARAM) lpb );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::getCommandToIndex( const int idButton ) const {
	return SendMessage( this->m_Hwnd, TB_COMMANDTOINDEX, (WPARAM) idButton, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::moveButton( const int iButtonFrom, const int iButtonTo ) {
	return SendMessage( this->m_Hwnd, TB_MOVEBUTTON, (WPARAM) iButtonFrom, (LPARAM) iButtonTo );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::markButton( const int iButton, const bool fHighlight ) {
	return SendMessage( this->m_Hwnd, TB_MARKBUTTON, (WPARAM) iButton, (LPARAM) MAKELONG ( fHighlight, 0 ) );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::setButtonState( const int idButton, const UINT fState ) {
	return SendMessage( this->m_Hwnd, TB_SETSTATE, (WPARAM) idButton, (LPARAM) MAKELONG ( fState, 0 ) );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxToolBar::setButtonSize( const int dxButton, const int dyButton ) {
	return SendMessage( this->m_Hwnd, TB_SETBUTTONSIZE, (WPARAM) 0, (LPARAM) MAKELONG (dxButton, dyButton) );
}


/*!
* \brief blah
*
* blah
*/

int DcxToolBar::getFreeButtonID( ) const {

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

int DcxToolBar::getIndexToCommand(const int iIndex) const {
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
LRESULT DcxToolBar::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {
		case WM_COMMAND:
			{
				int iButton = (int) wParam;

				if ( iButton > -1 ) {
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
						this->execAliasEx("%s,%d,%d", "sclick", this->getUserID( ), this->getCommandToIndex( iButton ) + 1 );
				}

				bParsed = TRUE;
			}
			break;

		case WM_NOTIFY:
			{
				LPNMHDR hdr = (LPNMHDR) lParam;
				if (!hdr)
					break;

				switch( hdr->code ) {
				case NM_RCLICK:
					{
						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
							LPNMMOUSE lpnm = (LPNMMOUSE) lParam;
							POINT pt = lpnm->pt;
							int iButton = (int) this->hitTest( &pt );

							if ( iButton > -1 ) {
								RECT rc;
								this->getItemRect( iButton, &rc );
								MapWindowRect(this->m_Hwnd, NULL, &rc);
								this->execAliasEx("%s,%d,%d,%d,%d,%d,%d", "rclick", this->getUserID( ), iButton+1, rc.left, rc.bottom, rc.right, rc.top );
								//POINT pt2 = pt;
								//pt.x = rc.left; 
								//pt.y = rc.bottom;
								//pt2.x = rc.left;
								//pt2.y = rc.top;
								//ClientToScreen( this->m_Hwnd, &pt );
								//ClientToScreen( this->m_Hwnd, &pt2 );
								//this->execAliasEx("%s,%d,%d,%d,%d,%d,%d", "rclick", 
								//	this->getUserID( ), iButton+1, pt.x, pt.y, pt2.x, pt2.y );
							}
						}
						bParsed = TRUE;
						return TRUE;
					}
					break;

				case TBN_DROPDOWN:
					{
						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
							LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR) lParam;
							int iButton = lpnmtb->iItem -1;

							if ( iButton > -1 ) {
								RECT rc;
								this->getItemRect( iButton, &rc );
								MapWindowRect(this->m_Hwnd, NULL, &rc);
								this->execAliasEx("%s,%d,%d,%d,%d,%d,%d", "dropdown", this->getUserID( ), iButton+1, rc.left, rc.bottom, rc.right, rc.top );
								//POINT pt2 = pt;
								//pt.x = rc.left;
								//pt.y = rc.bottom;
								//pt2.x = rc.left;
								//pt2.y = rc.top;
								//ClientToScreen( this->m_Hwnd, &pt );
								//ClientToScreen( this->m_Hwnd, &pt2 );
								//this->execAliasEx("%s,%d,%d,%d,%d,%d,%d", "dropdown",
								//	this->getUserID( ), iButton+1, pt.x, pt.y, pt2.x, pt2.y );
							}
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

								DWORD dFlags = (CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT);

								if ( lpdtbb->clrBtnFace != -1 )
									lpntbcd->clrBtnFace = lpdtbb->clrBtnFace;
								if ( lpdtbb->clrBtnHighlight != -1 )
									lpntbcd->clrBtnHighlight = lpdtbb->clrBtnHighlight;
								if ( lpdtbb->clrHighlightHotTrack != -1 ) {
									lpntbcd->clrHighlightHotTrack = lpdtbb->clrHighlightHotTrack;
									dFlags |= TBCDRF_HILITEHOTTRACK;
								}
								if ( lpdtbb->clrMark != -1 )
									lpntbcd->clrMark = lpdtbb->clrMark;
								if ( lpdtbb->clrText != -1 )
									lpntbcd->clrText = lpdtbb->clrText;
								if ( lpdtbb->clrTextHighlight != -1 )
									lpntbcd->clrTextHighlight = lpdtbb->clrTextHighlight;

								HFONT hFont = GetWindowFont( this->m_Hwnd );

								LOGFONT lf;
								GetObject( hFont, sizeof(LOGFONT), &lf );
								if ( lpdtbb->bBold )
									lf.lfWeight |= FW_BOLD;
								else
									lf.lfWeight &= ~FW_BOLD;
								if ( lpdtbb->bUline )
									lf.lfUnderline = TRUE;
								else
									lf.lfUnderline = FALSE;

								this->m_hItemFont = CreateFontIndirect( &lf );
								if (this->m_hItemFont != NULL)
									this->m_hOldItemFont = SelectFont( lpntbcd->nmcd.hdc, this->m_hItemFont );

#if defined(DCX_USE_WINSDK)
								// allows custom colours even when control is themed. (Vista Only)
								if (this->m_bOverrideTheme)
									dFlags |= TBCDRF_USECDCOLORS;
#endif
								return dFlags;
							}

						case CDDS_ITEMPOSTPAINT:
							//return CDRF_DODEFAULT;
							{
								if (this->m_hOldItemFont != NULL) {
									SelectFont( lpntbcd->nmcd.hdc, this->m_hOldItemFont);
									this->m_hOldItemFont = NULL;
								}
								if (this->m_hItemFont != NULL) {
									DeleteFont(this->m_hItemFont);
									this->m_hItemFont = NULL;
								}
								return CDRF_DODEFAULT;
							}

						default:
							return CDRF_DODEFAULT;
						}
					}
					break;

				case TBN_GETINFOTIP:
					{
						LPNMTBGETINFOTIP tcgit = (LPNMTBGETINFOTIP) lParam;
						if ( tcgit != NULL ) {

							LPDCXTBBUTTON lpdtbb = (LPDCXTBBUTTON) tcgit->lParam;

							if (( lpdtbb != NULL ) && (tcgit->pszText != NULL))
								lstrcpyn(tcgit->pszText,lpdtbb->tsTipText.to_chr(), tcgit->cchTextMax);
						}
						bParsed = TRUE;
					}
					break;
					//case TTN_GETDISPINFO:
					//	{
					//		LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam;

					//		idButton = lpttt->hdr.idFrom;
					//	}
					//	break;

				case TBN_DELETINGBUTTON:
					{
						LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR) lParam;
						TBBUTTON tbb;
						ZeroMemory( &tbb, sizeof(TBBUTTON) );

						this->getButton( this->getCommandToIndex(lpnmtb->iItem), &tbb );
						if ( tbb.dwData != NULL ) {
							delete (LPDCXTBBUTTON) tbb.dwData;
						}
						bParsed = TRUE;
					}
					break;
					// NM_CHAR code all works, but do we want it?
					//case NM_CHAR:
					//	{
					//		if (this->m_pParentDialog->getEventMask() & DCX_EVENT_EDIT) {
					//			LPNMCHAR lpnmc = (LPNMCHAR) lParam;
					//			this->execAliasEx("%s,%d,%d,%d", "keyup", this->getUserID(), lpnmc->ch, lpnmc->dwItemPrev);
					//		}
					//		bParsed = TRUE;
					//	}
					//	break;

			} // switch
		}
		break;
	}
	return 0L;
}

LRESULT DcxToolBar::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

		case WM_CONTEXTMENU:
		case WM_LBUTTONUP:
			break;

		case WM_SIZE:
			{
				if ( this->m_bAutoStretch )
					this->autoStretchButtons( );        
			}
			break;

		case WM_PAINT:
			{
				if (!this->m_bAlphaBlend)
					break;
				PAINTSTRUCT ps;
				HDC hdc;

				hdc = BeginPaint( this->m_Hwnd, &ps );

				LRESULT res = 0L;
				bParsed = TRUE;

				// Setup alpha blend if any.
				LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

				res = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

				this->FinishAlphaBlend(ai);

				EndPaint( this->m_Hwnd, &ps );
				return res;
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