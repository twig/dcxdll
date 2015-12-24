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

DcxToolBar::DcxToolBar(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
	, m_hItemFont(nullptr)
	, m_hOldItemFont(nullptr)
	, m_bOverrideTheme(false)
{
	//We need to divide ExStyles and tbExStyles here because the first is used for
	//global transparent style and the second for arrows style
	LONG Styles = 0, ExStyles = 0, ExStylesTb = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &ExStylesTb, &bNoTheme);

	this->m_Hwnd = CreateWindowEx(
		ExStyles,
		DCX_TOOLBARCLASS,
		nullptr,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(this->m_Hwnd))
		throw std::runtime_error("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(this->m_Hwnd, L" ", L" ");

	if (ExStylesTb != 0)
		SendMessage(this->m_Hwnd, TB_SETEXTENDEDSTYLE, (WPARAM)0, (LPARAM)ExStylesTb);

	SendMessage(this->m_Hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), (LPARAM)0);
	this->m_ToolTipHWND = (HWND)SendMessage(this->m_Hwnd, TB_GETTOOLTIPS, NULL, NULL);
	if (styles.istok(TEXT("balloon")) && this->m_ToolTipHWND != nullptr) {
		SetWindowLong(this->m_ToolTipHWND, GWL_STYLE, GetWindowStyle(this->m_ToolTipHWND) | TTS_BALLOON);
	}
	//SendMessage( this->m_Hwnd, TB_SETPARENT, (WPARAM)mParentHwnd, NULL);

	this->autoSize();
	this->m_bAutoStretch = FALSE;

	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
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
void DcxToolBar::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {
	LONG ExStylesTb;
	parseControlStyles(styles, Styles, ExStyles, &ExStylesTb, bNoTheme);
}

/*!
 * \brief blah
 *
 * blah
 */
void DcxToolBar::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, LONG * ExStylesTb, BOOL * bNoTheme )
{
#if TSTRING_PARTS
	//*Styles |= CCS_ADJUSTABLE;
	for (const auto &tsStyle: styles)
	{
		if ( tsStyle == TEXT("flat") )
			*Styles |= TBSTYLE_FLAT;
		else if ( tsStyle == TEXT("tooltips") )
			*Styles |= TBSTYLE_TOOLTIPS;
		else if ( tsStyle == TEXT("transparent") )
			*Styles |= TBSTYLE_TRANSPARENT;
		else if ( tsStyle == TEXT("nodivider") )
			*Styles |= CCS_NODIVIDER;
		else if ( tsStyle == TEXT("top") )
			*Styles |= CCS_TOP;
		else if ( tsStyle == TEXT("bottom") )
			*Styles |= CCS_BOTTOM;
		else if ( tsStyle == TEXT("left") )
			*Styles |= CCS_LEFT;
		else if ( tsStyle == TEXT("right") )
			*Styles |= CCS_RIGHT;
		//else if ( tsStyle == TEXT("noresize") ) 
		//  *Styles |= CCS_NORESIZE;
		//else if ( tsStyle == TEXT("noparentalign") ) 
		//  *Styles |= CCS_NOPARENTALIGN ;
		else if ( tsStyle == TEXT("noauto") )
			*Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
		else if ( tsStyle == TEXT("adjustable") )
			*Styles |= CCS_ADJUSTABLE;
		else if ( tsStyle == TEXT("list") )
			*Styles |= TBSTYLE_LIST;
		else if ( tsStyle == TEXT("wrap") )
			*Styles |= TBSTYLE_WRAPABLE;
		else if ( tsStyle == TEXT("arrows") )
			*ExStylesTb |= TBSTYLE_EX_DRAWDDARROWS;
		else if ( tsStyle == TEXT("override") )
			this->m_bOverrideTheme = true;
	}
#else
	//*Styles |= CCS_ADJUSTABLE;
	for (auto tsStyle(styles.getfirsttok(1)); !tsStyle.empty(); tsStyle = styles.getnexttok())
	{
		if ( tsStyle == TEXT("flat") )
			*Styles |= TBSTYLE_FLAT;
		else if ( tsStyle == TEXT("tooltips") )
			*Styles |= TBSTYLE_TOOLTIPS;
		else if ( tsStyle == TEXT("transparent") )
			*Styles |= TBSTYLE_TRANSPARENT;
		else if ( tsStyle == TEXT("nodivider") )
			*Styles |= CCS_NODIVIDER;
		else if ( tsStyle == TEXT("top") )
			*Styles |= CCS_TOP;
		else if ( tsStyle == TEXT("bottom") )
			*Styles |= CCS_BOTTOM;
		else if ( tsStyle == TEXT("left") )
			*Styles |= CCS_LEFT;
		else if ( tsStyle == TEXT("right") )
			*Styles |= CCS_RIGHT;
		//else if ( tsStyle == TEXT("noresize") ) 
		//  *Styles |= CCS_NORESIZE;
		//else if ( tsStyle == TEXT("noparentalign") ) 
		//  *Styles |= CCS_NOPARENTALIGN ;
		else if ( tsStyle == TEXT("noauto") )
			*Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
		else if ( tsStyle == TEXT("adjustable") )
			*Styles |= CCS_ADJUSTABLE;
		else if ( tsStyle == TEXT("list") )
			*Styles |= TBSTYLE_LIST;
		else if ( tsStyle == TEXT("wrap") )
			*Styles |= TBSTYLE_WRAPABLE;
		else if ( tsStyle == TEXT("arrows") )
			*ExStylesTb |= TBSTYLE_EX_DRAWDDARROWS;
		else if ( tsStyle == TEXT("override") )
			this->m_bOverrideTheme = true;
	}
#endif
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

void DcxToolBar::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const {

	const auto numtok = input.numtok();
	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP]
	if ( prop == TEXT("num") ) {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getButtonCount( ) );
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("mouseitem")) {
		const auto lResult = SendMessage(this->m_Hwnd, TB_GETHOTITEM, NULL, NULL);

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), lResult);
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("text") && numtok > 3 ) {

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			throw std::invalid_argument("Out of Range");

		// This way fails to give the correct result after buttons have been removed.
		//this->getButtonText( this->getIndexToCommand( nButton ), szReturnValue ); // possible overflow, needs fixing at some point.
		TBBUTTONINFO tbbi;
		ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
		tbbi.cbSize = sizeof( TBBUTTONINFO );
		tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;
		this->getButtonInfo( iButton, &tbbi );

		auto lpdcxtbb = reinterpret_cast<LPDCXTBBUTTON>(tbbi.lParam);

		if ( lpdcxtbb != nullptr )
			dcx_strcpyn( szReturnValue, lpdcxtbb->bText.to_chr( ), MIRC_BUFFER_SIZE_CCH );
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("icon") && numtok > 3 ) {

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			throw std::invalid_argument("Out of Range");

		TBBUTTONINFO tbbi;
		ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
		tbbi.cbSize = sizeof( TBBUTTONINFO );
		tbbi.dwMask = TBIF_IMAGE | TBIF_BYINDEX;
		this->getButtonInfo( iButton, &tbbi );

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), tbbi.iImage + 1 );
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("state") && numtok > 3 ) {

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			throw std::invalid_argument("Out of Range");

		TBBUTTONINFO tbbi;
		ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
		tbbi.cbSize = sizeof( TBBUTTONINFO );
		tbbi.dwMask = TBIF_STATE | TBIF_BYINDEX;
		this->getButtonInfo( iButton, &tbbi );

		if (lstrcpyn(szReturnValue, TEXT("+"), MIRC_BUFFER_SIZE_CCH) != nullptr)
		{
			if (!dcx_testflag(tbbi.fsState, TBSTATE_ENABLED))
				lstrcat(szReturnValue, TEXT("d"));

			if (dcx_testflag(tbbi.fsState, TBSTATE_INDETERMINATE))
				lstrcat(szReturnValue, TEXT("i"));

			if (dcx_testflag(tbbi.fsState, TBSTATE_HIDDEN))
				lstrcat(szReturnValue, TEXT("h"));

			if (dcx_testflag(tbbi.fsState, TBSTATE_PRESSED))
				lstrcat(szReturnValue, TEXT("p"));

			if (dcx_testflag(tbbi.fsState, TBSTATE_CHECKED))
				lstrcat(szReturnValue, TEXT("x"));

			if (dcx_testflag(tbbi.fsState, TBSTATE_WRAP))
				lstrcat(szReturnValue, TEXT("w"));
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("tooltip") && numtok > 3 ) {

		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		if (iButton < 0 && iButton >= this->getButtonCount())
			throw std::invalid_argument("Out of Range");

		TBBUTTONINFO tbbi;
		ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
		tbbi.cbSize = sizeof( TBBUTTONINFO );
		tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;
		this->getButtonInfo( iButton, &tbbi );

		auto lpdcxtbb = reinterpret_cast<LPDCXTBBUTTON>(tbbi.lParam);

		if ( lpdcxtbb != nullptr )
			dcx_strcpyn( szReturnValue, lpdcxtbb->tsTipText.to_chr( ), MIRC_BUFFER_SIZE_CCH );
	}
	// [NAME] [ID] [PROP] [N]
	else if (prop == TEXT("dropdownpoint") && numtok > 3) {
		RECT rc;

		ZeroMemory(&rc, sizeof(RECT));
		const auto iButton = input.getnexttok().to_int() - 1;	// tok 4

		// out of range;
		if (iButton < 0 && iButton >= this->getButtonCount())
			throw std::invalid_argument("Out of Range");

		this->getItemRect(iButton, &rc);
		MapWindowPoints(this->m_Hwnd, nullptr, (LPPOINT)&rc, 2);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.bottom, rc.right, rc.top);
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
* \brief blah
*
* blah
*/

void DcxToolBar::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		this->resetContent();

	// xdid -a [NAME] [ID] [SWITCH] [N] [+FLAGS] [WIDTH] [#ICON] [COLOR] (Button Text) [TAB] Tooltip Text
	if ( flags[TEXT('a')] && numtok > 4 ) {

		auto nPos = input.getnexttok().to_int() - 1;	// tok 4

		if ( nPos == -1 )
			nPos += this->getButtonCount( ) + 1;

		const auto tsFlags(input.getnexttok());							// tok 5
		const auto width = input.getnexttok().to_int();					// tok 6
		const auto icon = input.getnexttok().to_int() - 1;					// tok 7
		const auto clrText = (COLORREF)input.getnexttok().to_num();	// tok 8
		const auto iNumtok = input.gettok(1, TSTAB).numtok();

		TBBUTTON tbb;
		ZeroMemory( &tbb, sizeof( TBBUTTON ) );

		tbb.fsState = parseButtonStateFlags( tsFlags );
		tbb.idCommand = this->getFreeButtonID( );
		auto buttonStyles = parseButtonStyleFlags(tsFlags);
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
		auto lpdcxtbb = new DCXTBBUTTON;

		if ( input.numtok( TSTAB ) > 1 )
			lpdcxtbb->tsTipText = input.gettok( 2, -1, TSTAB ).trim();

		if (dcx_testflag(buttonStyles, BTNS_UNDERLINE))
			lpdcxtbb->bUline = TRUE;
		else
			lpdcxtbb->bUline = FALSE;

		if (dcx_testflag(buttonStyles, BTNS_BOLD))
			lpdcxtbb->bBold = TRUE;
		else
			lpdcxtbb->bBold = FALSE;

		if (dcx_testflag(buttonStyles, BTNS_COLOR))
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
		if ( dcx_testflag(buttonStyles,BTNS_WIDTH ))
		{
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

		const auto iButton = input.getnexttok( ).to_int( ) - 1;							// tok 4
		const auto buttonStyles = parseButtonStyleFlags(input.getnexttok());			// tok 5
		const auto clrColor = (COLORREF)input.getnexttok().to_num();				// tok 6
		const auto removeButtonStyles = parseButtonStyleFlags(input.getnexttok());	// tok 7

		if (iButton == -1 && this->m_ToolTipHWND != nullptr) {
			if (dcx_testflag(buttonStyles, BTNS_TBKGCOLOR))
				SendMessage(this->m_ToolTipHWND,TTM_SETTIPBKCOLOR, (WPARAM)clrColor, NULL);
			else if (dcx_testflag(buttonStyles, BTNS_TTXTCOLOR))
				SendMessage(this->m_ToolTipHWND,TTM_SETTIPTEXTCOLOR, (WPARAM)clrColor, NULL);
		}
		else {

			if (iButton < 0 && iButton >= this->getButtonCount())
				throw std::out_of_range("Out of Range");

			TBBUTTONINFO tbbi;
			ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
			tbbi.cbSize = sizeof( TBBUTTONINFO );
			tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;

			if (this->getButtonInfo(iButton, &tbbi) == -1)
				throw std::runtime_error("Unable to get button info");

			auto lpdcxtbb = reinterpret_cast<LPDCXTBBUTTON>(tbbi.lParam);

			if ( dcx_testflag(buttonStyles, BTNS_UNDERLINE ))
				lpdcxtbb->bUline = TRUE;

			if (dcx_testflag(buttonStyles, BTNS_BOLD))
				lpdcxtbb->bBold = TRUE;

			if (dcx_testflag(buttonStyles, BTNS_COLOR))
				lpdcxtbb->clrText = clrColor;

			if (dcx_testflag(buttonStyles, BTNS_HIGHLIGHT_TXTCOLOR))
				lpdcxtbb->clrTextHighlight = clrColor;

			if (dcx_testflag(buttonStyles, BTNS_MARK_BKGCOLOR))
				lpdcxtbb->clrMark = clrColor;

			if (dcx_testflag(buttonStyles, BTNS_BTNCOLOR))
				lpdcxtbb->clrBtnFace = clrColor;

			if (dcx_testflag(buttonStyles, BTNS_HIGHLIGHT_BTNCOLOR))
				lpdcxtbb->clrBtnHighlight = clrColor;

			if (dcx_testflag(buttonStyles, BTNS_HOTTRACK_BTNCOLOR))
				lpdcxtbb->clrHighlightHotTrack = clrColor;

			if (dcx_testflag(removeButtonStyles, BTNS_UNDERLINE))
				lpdcxtbb->bUline = FALSE;

			if (dcx_testflag(removeButtonStyles, BTNS_BOLD))
				lpdcxtbb->bBold = FALSE;

			if (dcx_testflag(removeButtonStyles, BTNS_COLOR))
				lpdcxtbb->clrText = CLR_INVALID;

			if (dcx_testflag(removeButtonStyles, BTNS_HIGHLIGHT_TXTCOLOR))
				lpdcxtbb->clrTextHighlight = CLR_INVALID;

			if (dcx_testflag(removeButtonStyles, BTNS_MARK_BKGCOLOR))
				lpdcxtbb->clrMark = CLR_INVALID;

			if (dcx_testflag(removeButtonStyles, BTNS_BTNCOLOR))
				lpdcxtbb->clrBtnFace = CLR_INVALID;

			if (dcx_testflag(removeButtonStyles, BTNS_HIGHLIGHT_BTNCOLOR))
				lpdcxtbb->clrBtnHighlight = CLR_INVALID;

			if (dcx_testflag(removeButtonStyles, BTNS_HOTTRACK_BTNCOLOR))
				lpdcxtbb->clrHighlightHotTrack = CLR_INVALID;

			this->redrawWindow( );
		}
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('d')] && numtok > 3 ) {
		const auto nButton = input.getnexttok().to_int() - 1;	// tok 4

		if (nButton > -1)
			this->deleteButton(nButton);
	}
	// xdid -i [NAME] [ID] [SWITCH] [N] [IMAGE]
	else if ( flags[TEXT('i')] && numtok > 4 ) {

		const auto nButton = input.getnexttok().to_int() - 1;	// tok 4
		auto iImage = input.getnexttok().to_int() - 1;			// tok 5

		if (iImage < 0)
			iImage = I_IMAGENONE;

		TBBUTTONINFO tbbi;
		ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
		tbbi.cbSize = sizeof( TBBUTTONINFO );
		tbbi.dwMask = TBIF_IMAGE | TBIF_BYINDEX;
		tbbi.iImage = iImage;

		this->setButtonInfo( nButton /*this->getIndexToCommand( nButton )*/, &tbbi );
	}
	// xdid -j -> [NAME] [ID] -j [MIN] [MAX]
	else if ( flags[TEXT('j')] && numtok > 4 ) {

		const auto nMin = input.getnexttok().to_int();	// tok 4
		const auto nMax = input.getnexttok().to_int();	// tok 5	// was tok 4 should be 5

		this->setButtonWidth( nMin, nMax );
	}
	// xdid -l [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('l')] && numtok > 3) {
		HIMAGELIST himl;

		auto size = input.getnexttok().to_int();	// tok 4

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

		if ( input.getnexttok( ) == TEXT('1') ) {	// tok 4

			this->m_bAutoStretch = TRUE;
			this->autoStretchButtons( );
		}
		else
			this->m_bAutoStretch = FALSE;
	}
	// xdid -q [NAME] [ID] [SWITCH] [N] (TIPTEXT)
	else if (flags[TEXT('q')] && numtok > 3) {
		const auto nButton = input.getnexttok().to_int() - 1;	// tok 4

		if (nButton < 0 && nButton >= this->getButtonCount())
			throw std::out_of_range("Out of Range");

		//int idButton = this->getIndexToCommand(nButton);
		TBBUTTONINFO tbbi;

		ZeroMemory(&tbbi, sizeof(TBBUTTONINFO));
		tbbi.cbSize = sizeof(TBBUTTONINFO);
		tbbi.dwMask = TBIF_LPARAM | TBIF_BYINDEX;

		if (this->getButtonInfo(nButton, &tbbi) < 0)
			throw std::runtime_error("Unable to get button info");

		auto lpdcxtbb = reinterpret_cast<LPDCXTBBUTTON>(tbbi.lParam);

		if (numtok > 4)	// has a new tooltip
			lpdcxtbb->tsTipText = input.getlasttoks();	// tok 5, -1
		else					// no tooltip
			lpdcxtbb->tsTipText.clear();	// = TEXT("");
	}
	// This is to avoid an invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
	}
	// xdid -t [NAME] [ID] [SWITCH] [N] [+FLAGS]
	else if ( flags[TEXT('t')] && numtok > 4 ) {

		const auto nButton = input.getnexttok().to_int() - 1;				// tok 4
		const auto fStates = parseButtonStateFlags(input.getnexttok());	// tok 5

		const auto idButton = this->getIndexToCommand(nButton);

		if ( idButton > 0 )
			this->setButtonState( idButton, fStates );
	}
	// xdid -u [NAME] [ID] [SWITCH] [DX] [DY]
	else if ( flags[TEXT('u')] && numtok > 4 ) {

		const auto dxButton = input.getnexttok().to_int();		// tok 4
		const auto dyButton = input.getnexttok().to_int();		// tok 5

		this->setButtonSize( dxButton, dyButton );
	}
	// xdid -v [NAME] [ID] [SWITCH] [N] (TEXT)
	else if ( flags[TEXT('v')] && numtok > 3 ) {

		const auto nButton = input.getnexttok().to_int() - 1;		// tok 4

		if (nButton < 0 && nButton >= this->getButtonCount())
			throw std::out_of_range("Out of Range");
		
		const auto nIndex = this->getIndexToCommand(nButton);
		TBBUTTONINFO tbbi;

		ZeroMemory(&tbbi, sizeof(TBBUTTONINFO));
		tbbi.cbSize = sizeof(TBBUTTONINFO);
		tbbi.dwMask = TBIF_LPARAM;
		
		if (this->getButtonInfo(nIndex, &tbbi) < 0)
			throw std::runtime_error("Unable to get button info");
		
		auto lpdcxtbb = reinterpret_cast<LPDCXTBBUTTON>(tbbi.lParam);
		if (numtok > 4)
			lpdcxtbb->bText = input.getlasttoks();	// tok 5, -1
		else
			lpdcxtbb->bText.clear();	// = TEXT("");

		ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
		tbbi.cbSize = sizeof( TBBUTTONINFO );
		tbbi.dwMask = TBIF_TEXT;
		tbbi.pszText = lpdcxtbb->bText.to_chr();
		this->setButtonInfo(nIndex, &tbbi);
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')] && numtok > 5) {
		const auto tsFlags(input.getnexttok( ));	// tok 4
		const auto iFlags = this->parseImageListFlags(tsFlags);

		if (tsFlags[0] != TEXT('+'))
			throw std::invalid_argument("Invalid Flags");

		const auto index = input.getnexttok().to_int();	// tok 5
		auto filename(input.getlasttoks());			// tok 6, -1

		auto himl = this->getImageList(TB_IML_NORMAL);

		// load the icon
		if (himl == nullptr)
			throw std::runtime_error("Unable to get Normal Image List");

		int cx, cy;
		if (!ImageList_GetIconSize(himl, &cx, &cy))
			throw std::runtime_error("Unable to get Icon Size");

		//auto icon = dcxLoadIcon(index, filename, (cx > 16), tsFlags);
		//
		//// if there is an icon to process
		//if (icon == nullptr)
		//	throw std::runtime_error(Dcx::dcxGetFormattedString(TEXT("Icon Failed To Load: %s"), filename.to_chr()));
		//
		//// NORMAL IML
		//if (dcx_testflag(iFlags, TB_IML_NORMAL)) {
		//	himl = this->getImageList(TB_IML_NORMAL);
		//
		//	if (himl != nullptr)
		//		ImageList_AddIcon(himl, icon);
		//}
		//
		//// DISABLED IML
		//if (dcx_testflag(iFlags, TB_IML_DISABLE)) {
		//	himl = this->getImageList(TB_IML_DISABLE);
		//
		//	if (himl != nullptr)
		//		ImageList_AddIcon(himl, icon);
		//}
		//
		//// HOT IML
		//if (dcx_testflag(iFlags, TB_IML_HOT)) {
		//	himl = this->getImageList(TB_IML_HOT);
		//
		//	if (himl != nullptr)
		//		ImageList_AddIcon(himl, icon);
		//}
		//
		//DestroyIcon(icon);

		Dcx::dcxIcon icon(index, filename, (cx > 16), tsFlags);

		// NORMAL IML
		if (dcx_testflag(iFlags, TB_IML_NORMAL)) {
			himl = this->getImageList(TB_IML_NORMAL);

			if (himl != nullptr)
				ImageList_AddIcon(himl, icon);
		}

		// DISABLED IML
		if (dcx_testflag(iFlags, TB_IML_DISABLE)) {
			himl = this->getImageList(TB_IML_DISABLE);

			if (himl != nullptr)
				ImageList_AddIcon(himl, icon);
		}

		// HOT IML
		if (dcx_testflag(iFlags, TB_IML_HOT)) {
			himl = this->getImageList(TB_IML_HOT);

			if (himl != nullptr)
				ImageList_AddIcon(himl, icon);
		}
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

	const XSwitchFlags xflags(flags);
	BYTE iFlags = TBSTATE_ENABLED;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if ( xflags[TEXT('d')] )
		iFlags &= ~TBSTATE_ENABLED;
	if ( xflags[TEXT('h')] )
		iFlags |= TBSTATE_HIDDEN;
	if ( xflags[TEXT('i')] )
		iFlags |= TBSTATE_INDETERMINATE;
	if ( xflags[TEXT('p')] )
		iFlags |= TBSTATE_PRESSED;
	if ( xflags[TEXT('x')] )
		iFlags |= TBSTATE_CHECKED;
	if ( xflags[TEXT('w')] )
		iFlags |= TBSTATE_WRAP;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

UINT DcxToolBar::parseButtonStyleFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = BTNS_BUTTON;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if (xflags[TEXT('a')])
		iFlags |= BTNS_AUTOSIZE;
	if (xflags[TEXT('b')])
		iFlags |= BTNS_BOLD;
	if (xflags[TEXT('B')])
		iFlags |= BTNS_BTNCOLOR;
	if (xflags[TEXT('c')])
		iFlags |= BTNS_COLOR;
	if (xflags[TEXT('g')])
		iFlags |= BTNS_GROUP;
	if (xflags[TEXT('h')])
		iFlags |= BTNS_HIGHLIGHT_TXTCOLOR;
	if (xflags[TEXT('H')])
		iFlags |= BTNS_HIGHLIGHT_BTNCOLOR;
	if (xflags[TEXT('k')])
		iFlags |= BTNS_CHECK;
	if (xflags[TEXT('l')])
		iFlags |= BTNS_WIDTH;
	if (xflags[TEXT('m')])
		iFlags |= BTNS_MARK_BKGCOLOR;
	if (xflags[TEXT('T')])
		iFlags |= BTNS_HOTTRACK_BTNCOLOR;
	if (xflags[TEXT('u')])
		iFlags |= BTNS_UNDERLINE;
	if (xflags[TEXT('v')])
		iFlags |= BTNS_DROPDOWN;
	if (xflags[TEXT('x')])
		iFlags |= BTNS_TBKGCOLOR;
	if (xflags[TEXT('z')])
		iFlags |= BTNS_TTXTCOLOR;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/
UINT DcxToolBar::parseImageListFlags( const TString &flags) {
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('n')])
		iFlags |= TB_IML_NORMAL;
	if (xflags[TEXT('d')])
		iFlags |= TB_IML_DISABLE;
	if (xflags[TEXT('h')])
		iFlags |= TB_IML_HOT;
	if (xflags[TEXT('g')])
		iFlags |= TB_ICO_GREY;

	return iFlags;
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxToolBar::getImageList( const dcxToolBar_ImageLists iImageList ) const
{
	if ( iImageList == TB_IML_NORMAL )
		return (HIMAGELIST) SendMessage( this->m_Hwnd, TB_GETIMAGELIST, (WPARAM) 0, (LPARAM) 0);
	else if ( iImageList == TB_IML_DISABLE )
		return (HIMAGELIST) SendMessage( this->m_Hwnd, TB_GETDISABLEDIMAGELIST, (WPARAM) 0, (LPARAM) 0);
	else if ( iImageList == TB_IML_HOT )
		return (HIMAGELIST) SendMessage( this->m_Hwnd, TB_GETHOTIMAGELIST, (WPARAM) 0, (LPARAM) 0);
	return nullptr;
}

/*!
* \brief blah
*
* blah
*/

void DcxToolBar::setImageList(HIMAGELIST himl, const dcxToolBar_ImageLists iImageList) {
	HIMAGELIST himlOld = nullptr;

	if (iImageList == TB_IML_NORMAL)
		himlOld = (HIMAGELIST) SendMessage(this->m_Hwnd, TB_SETIMAGELIST, (WPARAM) nullptr, (LPARAM) himl);
	else if (iImageList == TB_IML_DISABLE)
		himlOld = (HIMAGELIST) SendMessage(this->m_Hwnd, TB_SETDISABLEDIMAGELIST, (WPARAM) nullptr, (LPARAM) himl);
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

	auto nButtons = this->getButtonCount( );

	while ( nButtons-- )
		this->deleteButton( nButtons );
}

/*!
* \brief blah
*
* blah
*/

void DcxToolBar::autoStretchButtons( ) {

	const auto nButtons = this->getButtonCount();
	auto nTotalButtons = nButtons;

	RECT rc, rcItem;

	if (!GetClientRect(this->m_Hwnd, &rc))
		return;

	auto leftwidth = rc.right - rc.left;

	TBBUTTON tbb;
	ZeroMemory( &tbb, sizeof( TBBUTTON ) );

	// Find the button width
	for (auto i = decltype(nButtons){0}; i < nButtons; i++)
	{
		this->getButton( i, &tbb );

		if ( dcx_testflag(tbb.fsStyle, BTNS_SEP) ) {
			this->getItemRect( i, &rcItem );
			leftwidth -= rcItem.right - rcItem.left;
			--nTotalButtons;
		}
	}

	// Apply the new width
	TBBUTTONINFO tbbi;
	ZeroMemory( &tbbi, sizeof( TBBUTTONINFO ) );
	tbbi.cbSize = sizeof( TBBUTTONINFO );
	tbbi.dwMask = TBIF_SIZE;
	tbbi.cx = (WORD)( (double) leftwidth / (double) nTotalButtons );

	// Make sure the size is positive
	if ( tbbi.cx > 0 ) {

		for (auto i = decltype(nButtons){0}; i < nButtons; i++ )
		{
			this->getButton( i, &tbb );

			if (dcx_testflag(tbb.fsStyle, BTNS_SEP))
				continue;

			this->setButtonInfo( this->getIndexToCommand( i ), &tbbi );
		}
	}
}

void DcxToolBar::autoPosition( const int width, const int height ) {

	if ( !this->isStyle( CCS_NOPARENTALIGN | CCS_NORESIZE ) )
		SendMessage( this->m_Hwnd, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
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

	int i = 0, iID;
	do {
		iID = this->getCommandToIndex( ++i );
	} while (iID != -1);
	return i;
}

/*!
* \brief blah
*
* blah
*/

int DcxToolBar::getIndexToCommand(const int iIndex) const {
	const auto nButtons = this->getButtonCount();

	for (auto i = decltype(nButtons){1}; i <= nButtons; i++)
	{
		if (this->getCommandToIndex(i) == iIndex)
			return i;
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
				auto iButton = static_cast<int>(wParam);

				if ( iButton > -1 ) {
					if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
						this->execAliasEx(TEXT("%s,%d,%d"), TEXT("sclick"), this->getUserID( ), this->getCommandToIndex( iButton ) + 1 );
				}

				bParsed = TRUE;
			}
			break;

		case WM_NOTIFY:
			{
				dcxlParam(LPNMHDR, hdr);

				if (hdr == nullptr)
					break;

				switch( hdr->code ) {
				case NM_RCLICK:
					{
						if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
						{
							dcxlParam(LPNMMOUSE, lpnm);

							auto pt = lpnm->pt;
							const auto iButton = this->hitTest(&pt);

							if ( iButton > -1 ) {
								RECT rc;
								this->getItemRect( iButton, &rc );
								MapWindowRect(this->m_Hwnd, nullptr, &rc);
								this->execAliasEx(TEXT("%s,%d,%d,%d,%d,%d,%d"), TEXT("rclick"), this->getUserID( ), iButton+1, rc.left, rc.bottom, rc.right, rc.top );
							}
						}
						bParsed = TRUE;
						return TRUE;
					}
					break;

				case TBN_DROPDOWN:
					{
						if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
						{
							dcxlParam(LPNMTOOLBAR, lpnmtb);

							const auto iButton = lpnmtb->iItem - 1;

							if ( iButton > -1 ) {
								RECT rc;
								this->getItemRect( iButton, &rc );
								MapWindowRect(this->m_Hwnd, nullptr, &rc);
								this->execAliasEx(TEXT("%s,%d,%d,%d,%d,%d,%d"), TEXT("dropdown"), this->getUserID( ), iButton+1, rc.left, rc.bottom, rc.right, rc.top );
							}
						}
						bParsed = TRUE;
						return TBDDRET_DEFAULT;
					}
					break;

				case NM_CUSTOMDRAW:
					{
						dcxlParam(LPNMTBCUSTOMDRAW, lpntbcd);

						bParsed = TRUE;
						switch( lpntbcd->nmcd.dwDrawStage ) {

						case CDDS_PREPAINT:
							return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);

						case CDDS_ITEMPREPAINT:
							{

								auto lpdtbb = reinterpret_cast<LPDCXTBBUTTON>(lpntbcd->nmcd.lItemlParam);

								if ( lpdtbb == nullptr )
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

								auto hFont = GetWindowFont(this->m_Hwnd);

								if (hFont != nullptr)
								{
									LOGFONT lf;
									if (GetObject(hFont, sizeof(LOGFONT), &lf) != 0)
									{
										if (lpdtbb->bBold)
											lf.lfWeight |= FW_BOLD;
										else
											lf.lfWeight &= ~FW_BOLD;
										if (lpdtbb->bUline)
											lf.lfUnderline = TRUE;
										else
											lf.lfUnderline = FALSE;

										this->m_hItemFont = CreateFontIndirect(&lf);
										if (this->m_hItemFont != nullptr)
											this->m_hOldItemFont = SelectFont(lpntbcd->nmcd.hdc, this->m_hItemFont);
									}
								}
								// allows custom colours even when control is themed. (Vista Only)
								if (this->m_bOverrideTheme)
									dFlags |= TBCDRF_USECDCOLORS;

								return dFlags;
							}

						case CDDS_ITEMPOSTPAINT:
							//return CDRF_DODEFAULT;
							{
								if (this->m_hOldItemFont != nullptr) {
									SelectFont( lpntbcd->nmcd.hdc, this->m_hOldItemFont);
									this->m_hOldItemFont = nullptr;
								}
								if (this->m_hItemFont != nullptr) {
									DeleteFont(this->m_hItemFont);
									this->m_hItemFont = nullptr;
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
						dcxlParam(LPNMTBGETINFOTIP, tcgit);

						if ( tcgit != nullptr ) {

							auto lpdtbb = reinterpret_cast<LPDCXTBBUTTON>(tcgit->lParam);

							if (( lpdtbb != nullptr ) && (tcgit->pszText != nullptr))
								dcx_strcpyn(tcgit->pszText,lpdtbb->tsTipText.to_chr(), tcgit->cchTextMax);
						}
						bParsed = TRUE;
					}
					break;

					//case TTN_GETDISPINFO:
					//	{
					//		LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam;
					//
					//		idButton = lpttt->hdr.idFrom;
					//	}
					//	break;

				case TBN_DELETINGBUTTON:
					{
						dcxlParam(LPNMTOOLBAR, lpnmtb);

						TBBUTTON tbb;
						ZeroMemory( &tbb, sizeof(TBBUTTON) );

						this->getButton( this->getCommandToIndex(lpnmtb->iItem), &tbb );

						auto lpBut = reinterpret_cast<LPDCXTBBUTTON>(tbb.dwData);

						delete lpBut;

						bParsed = TRUE;
					}
					break;

					// NM_CHAR code all works, but do we want it?
					//case NM_CHAR:
					//	{
					//		if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT)) {
					//			auto lpnmc = (LPNMCHAR) lParam;
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
				auto hdc = BeginPaint(this->m_Hwnd, &ps);

				LRESULT res = 0L;
				bParsed = TRUE;

				// Setup alpha blend if any.
				auto ai = this->SetupAlphaBlend(&hdc);

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
