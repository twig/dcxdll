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
#include "defines.h"
#include "Classes/dcxtoolbar.h"
#include "Classes/dcxdialog.h"

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
	//We need to divide ExStyles and tbExStyles here because the first is used for
	//global transparent style and the second for arrows style
	LONG Styles = 0, ExStyles = 0, ExStylesTb = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &ExStylesTb, &bNoTheme );

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
		throw TEXT("Unable To Create Window");

	if ( bNoTheme )
		Dcx::XPPlusModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	if ( ExStylesTb != 0 )
		SendMessage( this->m_Hwnd, TB_SETEXTENDEDSTYLE, (WPARAM) 0, (LPARAM) ExStylesTb );

	SendMessage( this->m_Hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), (LPARAM) 0 );
	this->m_ToolTipHWND = (HWND)SendMessage( this->m_Hwnd, TB_GETTOOLTIPS, NULL, NULL);
	if (styles.istok(TEXT("balloon")) && this->m_ToolTipHWND != NULL) {
		SetWindowLong(this->m_ToolTipHWND,GWL_STYLE,GetWindowStyle(this->m_ToolTipHWND) | TTS_BALLOON);
	}
	//SendMessage( this->m_Hwnd, TB_SETPARENT, (WPARAM)mParentHwnd, NULL);

	this->autoSize( );
	this->m_bAutoStretch = FALSE;

	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
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
	LONG ExStylesTb;
	parseControlStyles(styles, Styles, ExStyles, &ExStylesTb, bNoTheme);
}

/*!
 * \brief blah
 *
 * blah
 */
void DcxToolBar::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, LONG * ExStylesTb, BOOL * bNoTheme ) {

	//*Styles |= CCS_ADJUSTABLE;
	unsigned int i = 1, numtok = styles.numtok( );

	while ( i <= numtok ) {

		if ( styles.gettok( i ) == TEXT("flat") )
			*Styles |= TBSTYLE_FLAT;
		else if ( styles.gettok( i ) == TEXT("tooltips") )
			*Styles |= TBSTYLE_TOOLTIPS;
		else if ( styles.gettok( i ) == TEXT("transparent") )
			*Styles |= TBSTYLE_TRANSPARENT;
		else if ( styles.gettok( i ) == TEXT("nodivider") )
			*Styles |= CCS_NODIVIDER;
		else if ( styles.gettok( i ) == TEXT("top") )
			*Styles |= CCS_TOP;
		else if ( styles.gettok( i ) == TEXT("bottom") )
			*Styles |= CCS_BOTTOM;
		else if ( styles.gettok( i ) == TEXT("left") )
			*Styles |= CCS_LEFT;
		else if ( styles.gettok( i ) == TEXT("right") )
			*Styles |= CCS_RIGHT;
		//else if ( styles.gettok( i ) == TEXT("noresize") ) 
		//  *Styles |= CCS_NORESIZE;
		//else if ( styles.gettok( i ) == TEXT("noparentalign") ) 
		//  *Styles |= CCS_NOPARENTALIGN ;
		else if ( styles.gettok( i ) == TEXT("noauto") )
			*Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
		else if ( styles.gettok( i ) == TEXT("adjustable") )
			*Styles |= CCS_ADJUSTABLE;
		else if ( styles.gettok( i ) == TEXT("list") )
			*Styles |= TBSTYLE_LIST;
		else if ( styles.gettok( i ) == TEXT("wrap") )
			*Styles |= TBSTYLE_WRAPABLE;
		else if ( styles.gettok( i ) == TEXT("arrows") )
			*ExStylesTb |= TBSTYLE_EX_DRAWDDARROWS;
		else if ( styles.gettok( i ) == TEXT("override") )
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

void DcxToolBar::parseInfoRequest( TString & input, PTCHAR szReturnValue ) {

	int numtok = input.numtok( );
	TString prop(input.gettok( 3 ));

	// [NAME] [ID] [PROP]
	if ( prop == TEXT("num") ) {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getButtonCount( ) );
		return;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("mouseitem")) {
		long lResult = SendMessage(this->m_Hwnd, TB_GETHOTITEM, NULL, NULL);

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), lResult);
		return;
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("text") && numtok > 3 ) {

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
				lstrcpyn( szReturnValue, lpdcxtbb->bText.to_chr( ), MIRC_BUFFER_SIZE_CCH );
			return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("icon") && numtok > 3 ) {

		int iButton = input.gettok( 4 ).to_int( ) - 1;

		if ( iButton > -1 && iButton < this->getButtonCount( ) ) {

			TBBUTTONINFO tbbi;
			ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
			tbbi.cbSize = sizeof( TBBUTTONINFO );
			tbbi.dwMask = TBIF_IMAGE | TBIF_BYINDEX;
			this->getButtonInfo( iButton, &tbbi );

			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), tbbi.iImage + 1 );
			return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("state") && numtok > 3 ) {

		int iButton = input.gettok( 4 ).to_int( ) - 1;

		if ( iButton > -1 && iButton < this->getButtonCount( ) ) {

			TBBUTTONINFO tbbi;
			ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
			tbbi.cbSize = sizeof( TBBUTTONINFO );
			tbbi.dwMask = TBIF_STATE | TBIF_BYINDEX;
			this->getButtonInfo( iButton, &tbbi );

			lstrcpyn( szReturnValue, TEXT("+"), MIRC_BUFFER_SIZE_CCH );

			if ( !( tbbi.fsState & TBSTATE_ENABLED ) )
				lstrcat( szReturnValue,TEXT("d") );

			if ( tbbi.fsState & TBSTATE_INDETERMINATE )
				lstrcat( szReturnValue,TEXT("i") );

			if ( tbbi.fsState & TBSTATE_HIDDEN )
				lstrcat( szReturnValue,TEXT("h") );

			if ( tbbi.fsState & TBSTATE_PRESSED )
				lstrcat( szReturnValue,TEXT("p") );

			if ( tbbi.fsState & TBSTATE_CHECKED )
				lstrcat( szReturnValue,TEXT("x") );

			if ( tbbi.fsState & TBSTATE_WRAP )
				lstrcat( szReturnValue,TEXT("w") );

			return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("tooltip") && numtok > 3 ) {

		int iButton = input.gettok( 4 ).to_int( ) - 1;

		if ( iButton > -1 && iButton < this->getButtonCount( ) ) {

			TBBUTTONINFO tbbi;
			ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
			tbbi.cbSize = sizeof( TBBUTTONINFO );
			tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;
			this->getButtonInfo( iButton, &tbbi );

			LPDCXTBBUTTON lpdcxtbb = (LPDCXTBBUTTON) tbbi.lParam;

			if ( lpdcxtbb != NULL )
				lstrcpyn( szReturnValue, lpdcxtbb->tsTipText.to_chr( ), MIRC_BUFFER_SIZE_CCH );
			return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if (prop == TEXT("dropdownpoint") && numtok > 3) {
		RECT rc;

		ZeroMemory(&rc, sizeof(RECT));
		int iButton = input.gettok(4).to_int() -1;

		// out of range;
		if ((iButton < 0) || (iButton >= this->getButtonCount())) {
			this->showError(TEXT("dropdownpoint"), NULL, TEXT("Index out of range."));
			return;
		}

		this->getItemRect(iButton, &rc);
		MapWindowPoints(this->m_Hwnd, NULL, (LPPOINT)&rc, 2);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.bottom, rc.right, rc.top);
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
	if (flags[TEXT('r')]) {
		this->resetContent();
	}

	// xdid -a [NAME] [ID] [SWITCH] [N] [+FLAGS] [WIDTH] [#ICON] [COLOR] (Button Text) [TAB] Tooltip Text
	if ( flags[TEXT('a')] && numtok > 4 ) {

		int nPos = input.gettok( 4 ).to_int( ) - 1;

		if ( nPos == -1 )
			nPos += this->getButtonCount( ) + 1;

		TString tsFlags(input.gettok( 5 ));
		int width = input.gettok( 6 ).to_int( );
		int icon = input.gettok( 7 ).to_int( ) - 1;
		COLORREF clrText = (COLORREF) input.gettok( 8 ).to_num( );
		int iNumtok = input.gettok( 1, TSTAB ).numtok( );

		TBBUTTON tbb;
		ZeroMemory( &tbb, sizeof( TBBUTTON ) );

		tbb.fsState = parseButtonStateFlags( tsFlags );
		tbb.idCommand = this->getFreeButtonID( );
		UINT buttonStyles = parseButtonStyleFlags( tsFlags );
		tbb.fsStyle = (BYTE)(buttonStyles & 0xFF);

		if (( icon == -1 ) || (iNumtok < 7))
			tbb.iBitmap = I_IMAGENONE;
		else
			tbb.iBitmap = icon;

		TString itemtext;

		if ( iNumtok > 8 ) {

			itemtext = input.gettok( 1, TSTAB ).gettok( 9, -1 ).trim();

			if ( itemtext == TEXT('-') ) {

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

		lpdcxtbb->tsTipText = TEXT("");

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
			lpdcxtbb->clrText = CLR_INVALID;
		lpdcxtbb->clrBtnFace = CLR_INVALID;
		lpdcxtbb->clrBtnHighlight = CLR_INVALID;
		lpdcxtbb->clrHighlightHotTrack = CLR_INVALID;
		lpdcxtbb->clrMark = CLR_INVALID;
		lpdcxtbb->clrTextHighlight = CLR_INVALID;
		lpdcxtbb->iTextBkgMode = TRANSPARENT;
		lpdcxtbb->iTextHighlightBkgMode = TRANSPARENT;

		tbb.dwData = (LPARAM) lpdcxtbb;
		lpdcxtbb->bText = itemtext;
		tbb.iString = (INT_PTR)lpdcxtbb->bText.to_chr();

		// insert button
		this->insertButton( nPos, &tbb );
		//if (nPos == 1) // commented out for possible fix to http://dcx.scriptsdb.org/forum/showthread.php?tid=121 http://dcx.scriptsdb.org/bug/index.php?do=details&task_id=749
		//	SendMessage(this->m_Hwnd, TB_CHANGEBITMAP, nPos, MAKELPARAM(I_IMAGENONE,0)); // why was this added?

		// set width of button
		if ( buttonStyles & BTNS_WIDTH ) {

			TBBUTTONINFO tbbi;
			ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
			tbbi.cbSize = sizeof( TBBUTTONINFO );
			tbbi.dwMask = 0 ;

			tbbi.dwMask |= TBIF_SIZE;
			tbbi.cx = (WORD)width;
			this->setButtonInfo( tbb.idCommand, &tbbi );
		}

		this->autoSize( );
		if ( this->m_bAutoStretch )
			this->autoStretchButtons( );
	}
	// xdid -c [NAME] [ID] [SWITCH] [N] [+FLAGS] [RGB] [+REMOVEFLAGS]
	else if ( flags[TEXT('c')] && numtok > 5 ) {

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
					lpdcxtbb->clrText = CLR_INVALID;

				if ( removeButtonStyles & BTNS_HIGHLIGHT_TXTCOLOR )
					lpdcxtbb->clrTextHighlight = CLR_INVALID;

				if ( removeButtonStyles & BTNS_MARK_BKGCOLOR )
					lpdcxtbb->clrMark = CLR_INVALID;

				if ( removeButtonStyles & BTNS_BTNCOLOR )
					lpdcxtbb->clrBtnFace = CLR_INVALID;

				if ( removeButtonStyles & BTNS_HIGHLIGHT_BTNCOLOR )
					lpdcxtbb->clrBtnHighlight = CLR_INVALID;

				if ( removeButtonStyles & BTNS_HOTTRACK_BTNCOLOR )
					lpdcxtbb->clrHighlightHotTrack = CLR_INVALID;

				this->redrawWindow( );
			}
		}
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('d')] && numtok > 3 ) {
		int nButton = input.gettok( 4 ).to_int( ) - 1;

		if (nButton > -1)
			this->deleteButton(nButton);
	}
	// xdid -i [NAME] [ID] [SWITCH] [N] [IMAGE]
	else if ( flags[TEXT('i')] && numtok > 4 ) {

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
	else if ( flags[TEXT('j')] && numtok > 4 ) {

		int nMin = input.gettok( 4 ).to_int( );
		int nMax = input.gettok( 4 ).to_int( );

		this->setButtonWidth( nMin, nMax );
	}
	// xdid -l [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('l')] && numtok > 3) {
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
	else if ( flags[TEXT('m')] && numtok > 3 ) {

		if ( input.gettok( 4 ) == TEXT('1') ) {

			this->m_bAutoStretch = TRUE;
			this->autoStretchButtons( );
		}
		else
			this->m_bAutoStretch = FALSE;
	}
	// xdid -q [NAME] [ID] [SWITCH] [N] (TIPTEXT)
	else if (flags[TEXT('q')] && numtok > 3) {
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
					lpdcxtbb->tsTipText = TEXT("");
			}
		}
	}
	// This is to avoid an invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
	}
	// xdid -t [NAME] [ID] [SWITCH] [N] [+FLAGS]
	else if ( flags[TEXT('t')] && numtok > 4 ) {

		int nButton = input.gettok( 4 ).to_int( ) - 1;
		UINT fStates = parseButtonStateFlags( input.gettok( 5 ) );

		int idButton = this->getIndexToCommand( nButton );

		if ( idButton > 0 )
			this->setButtonState( idButton, fStates );
	}
	// xdid -u [NAME] [ID] [SWITCH] [DX] [DY]
	else if ( flags[TEXT('u')] && numtok > 4 ) {

		int dxButton = input.gettok( 4 ).to_int( );
		int dyButton = input.gettok( 5 ).to_int( );

		this->setButtonSize( dxButton, dyButton );
	}
	// xdid -v [NAME] [ID] [SWITCH] [N] (TEXT)
	else if ( flags[TEXT('v')] && numtok > 3 ) {

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
					lpdcxtbb->bText = TEXT("");
				ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
				tbbi.cbSize = sizeof( TBBUTTONINFO );
				tbbi.dwMask = TBIF_TEXT;
				tbbi.pszText = lpdcxtbb->bText.to_chr();
				this->setButtonInfo(nIndex, &tbbi);
			}
		}
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')] && numtok > 5) {
		UINT iFlags = this->parseImageListFlags(input.gettok( 4 ));

		if (input.gettok( 4 )[0] != TEXT('+')) {
			this->showError(NULL, TEXT("-w"), TEXT("Invalid Flags"));
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
			this->showError(NULL, TEXT("-w"), TEXT("Unable to get Normal Image List"));

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
			this->showErrorEx(NULL, TEXT("-w"), TEXT("Icon Failed To Load: %s"), filename.to_chr());
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
* \brief blah
*
* blah
*/

BYTE DcxToolBar::parseButtonStateFlags( const TString & flags ) {

	INT i = 1, len = (INT)flags.len( );
	BYTE iFlags = TBSTATE_ENABLED;

	// no +sign, missing params
	if ( flags[0] != TEXT('+') ) 
		return iFlags;

	while ( i < len ) {

		if ( flags[i] == TEXT('d') )
			iFlags &= ~TBSTATE_ENABLED;
		else if ( flags[i] == TEXT('h') )
			iFlags |= TBSTATE_HIDDEN;
		else if ( flags[i] == TEXT('i') )
			iFlags |= TBSTATE_INDETERMINATE;
		else if ( flags[i] == TEXT('p') )
			iFlags |= TBSTATE_PRESSED;
		else if ( flags[i] == TEXT('x') )
			iFlags |= TBSTATE_CHECKED;
		else if ( flags[i] == TEXT('w') )
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
	if ( flags[0] != TEXT('+') ) 
		return iFlags;

	while ( i < len ) {

		switch (flags[i])
		{
		case TEXT('a'):
			iFlags |= TBSTYLE_AUTOSIZE; //BTNS_AUTOSIZE;
			break;
		case TEXT('b'):
			iFlags |= BTNS_BOLD;
			break;
		case TEXT('B'):
			iFlags |= BTNS_BTNCOLOR;
			break;
		case TEXT('c'):
			iFlags |= BTNS_COLOR;
			break;
		case TEXT('g'):
			iFlags |= TBSTYLE_GROUP; //BTNS_GROUP;
			break;
		case TEXT('h'):
			iFlags |= BTNS_HIGHLIGHT_TXTCOLOR;
			break;
		case TEXT('H'):
			iFlags |= BTNS_HIGHLIGHT_BTNCOLOR;
			break;
		case TEXT('k'):
			iFlags |= TBSTYLE_CHECK; //BTNS_CHECK;
			break;
		case TEXT('l'):
			iFlags |= BTNS_WIDTH;
			break;
		case TEXT('m'):
			iFlags |= BTNS_MARK_BKGCOLOR;
			break;
		case TEXT('T'):
			iFlags |= BTNS_HOTTRACK_BTNCOLOR;
			break;
		case TEXT('u'):
			iFlags |= BTNS_UNDERLINE;
			break;
		case TEXT('v'):
			iFlags |= TBSTYLE_DROPDOWN; //BTNS_DROPDOWN;
			break;
		case TEXT('x'):
			iFlags |= BTNS_TBKGCOLOR;
			break;
		case TEXT('z'):
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
	if (flags[0] != TEXT('+'))
		return iFlags;

	while (i < len) {
		if (flags[i] == TEXT('n'))
			iFlags |= TB_IML_NORMAL;
		else if (flags[i] == TEXT('d'))
			iFlags |= TB_IML_DISABLE;
		else if (flags[i] == TEXT('h'))
			iFlags |= TB_IML_HOT;
		else if (flags[i] == TEXT('g'))
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

			//mIRCError( TEXT("Separator") );
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
	tbbi.cx = (WORD)( (double) leftwidth / (double) nTotalButtons );

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

	mIRCError( TEXT("this->isStyle( CCS_NOPARENTALIGN ) && !this->isStyle( CCS_NORESIZE )") );

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

	mIRCError( TEXT("!this->isStyle( CCS_NOPARENTALIGN )") );
	SendMessage( this->m_Hwnd, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
	}
	*/
	/*

	if ( !this->isStyle( CCS_NOPARENTALIGN | CCS_NORESIZE ) ) {

	mIRCError( TEXT("No alignement styles") );
	}
	else if ( this->isStyle( CCS_NOPARENTALIGN ) ) {

	mIRCError( TEXT("Alignement NOPARENT styles") );
	}
	else if ( this->isStyle( CCS_NORESIZE ) ) {

	mIRCError( TEXT("Alignement NORESIZE styles") );

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
						this->execAliasEx(TEXT("%s,%d,%d"), TEXT("sclick"), this->getUserID( ), this->getCommandToIndex( iButton ) + 1 );
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
								this->execAliasEx(TEXT("%s,%d,%d,%d,%d,%d,%d"), TEXT("rclick"), this->getUserID( ), iButton+1, rc.left, rc.bottom, rc.right, rc.top );
								//POINT pt2 = pt;
								//pt.x = rc.left; 
								//pt.y = rc.bottom;
								//pt2.x = rc.left;
								//pt2.y = rc.top;
								//ClientToScreen( this->m_Hwnd, &pt );
								//ClientToScreen( this->m_Hwnd, &pt2 );
								//this->execAliasEx(TEXT("%s,%d,%d,%d,%d,%d,%d"), TEXT("rclick"), 
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
								this->execAliasEx(TEXT("%s,%d,%d,%d,%d,%d,%d"), TEXT("dropdown"), this->getUserID( ), iButton+1, rc.left, rc.bottom, rc.right, rc.top );
								//POINT pt2 = pt;
								//pt.x = rc.left;
								//pt.y = rc.bottom;
								//pt2.x = rc.left;
								//pt2.y = rc.top;
								//ClientToScreen( this->m_Hwnd, &pt );
								//ClientToScreen( this->m_Hwnd, &pt2 );
								//this->execAliasEx(TEXT("%s,%d,%d,%d,%d,%d,%d"), TEXT("dropdown"),
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

								if ( lpdtbb->clrBtnFace != CLR_INVALID )
									lpntbcd->clrBtnFace = lpdtbb->clrBtnFace;
								if ( lpdtbb->clrBtnHighlight != CLR_INVALID )
									lpntbcd->clrBtnHighlight = lpdtbb->clrBtnHighlight;
								if ( lpdtbb->clrHighlightHotTrack != CLR_INVALID ) {
									lpntbcd->clrHighlightHotTrack = lpdtbb->clrHighlightHotTrack;
									dFlags |= TBCDRF_HILITEHOTTRACK;
								}
								if ( lpdtbb->clrMark != CLR_INVALID )
									lpntbcd->clrMark = lpdtbb->clrMark;
								if ( lpdtbb->clrText != CLR_INVALID )
									lpntbcd->clrText = lpdtbb->clrText;
								if ( lpdtbb->clrTextHighlight != CLR_INVALID )
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
					//			this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("keyup"), this->getUserID(), lpnmc->ch, lpnmc->dwItemPrev);
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
