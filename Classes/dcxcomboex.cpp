/*!
* \file dcxcomboex.cpp
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
#include "Classes/dcxcomboex.h"
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

DcxComboEx::DcxComboEx( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles,
		DCX_COMBOEXCLASS,
		NULL,
		WS_CHILD | CBS_AUTOHSCROLL | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw "Unable To Create Window";

	if ( bNoTheme ) {
		Dcx::XPPlusModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );
		//SendMessage( this->m_Hwnd, CBEM_SETWINDOWTHEME, NULL, (LPARAM)(LPCWSTR)L" "); // do this instead?
	}

	this->m_EditHwnd = (HWND) this->getEditControl( );

	if ( IsWindow( this->m_EditHwnd ) ) {
		if ( bNoTheme )
			Dcx::XPPlusModule.dcxSetWindowTheme( this->m_EditHwnd , L" ", L" " );

		LPDCXCOMBOEXEDIT lpce = new DCXCOMBOEXEDIT;

		lpce->cHwnd = this->m_Hwnd;
		lpce->pHwnd = mParentHwnd;

		//SetWindowLong( this->m_EditHwnd, GWL_STYLE, GetWindowLong( this->m_EditHwnd, GWL_STYLE ));// | ES_AUTOHSCROLL );
		lpce->OldProc = SubclassWindow( this->m_EditHwnd, DcxComboEx::ComboExEditProc );
		SetWindowLongPtr( this->m_EditHwnd, GWLP_USERDATA, (LONG) lpce );
	}

	HWND combo = (HWND)SendMessage(this->m_Hwnd,CBEM_GETCOMBOCONTROL,0,0);
	if (IsWindow(combo)) {
		if (bNoTheme)
			Dcx::XPPlusModule.dcxSetWindowTheme( combo , L" ", L" " );

		COMBOBOXINFO cbi = { 0 };
		cbi.cbSize = sizeof(cbi);
		GetComboBoxInfo(combo, &cbi);

		if (styles.istok("sort")) { // doesnt work atm.
			if (IsWindow(cbi.hwndList)) {
				AddStyles(cbi.hwndList, GWL_STYLE, LBS_SORT);
			}
		}
		if (styles.istok("hscroll")) {
			//if (IsWindow(cbi.hwndCombo))
			//AddStyles(cbi.hwndCombo, GWL_STYLE, WS_HSCROLL);
			if (IsWindow(cbi.hwndList))
				AddStyles(cbi.hwndList, GWL_STYLE, WS_HSCROLL);
		}
	}
	//if (p_Dialog->getToolTip() != NULL) {
	//	if (styles.istok("tooltips")) {
	//		this->m_ToolTipHWND = p_Dialog->getToolTip();
	//		AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
	//		AddToolTipToolInfo(this->m_ToolTipHWND, this->m_EditHwnd);
	//	}
	//}

	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );

	DragAcceptFiles(this->m_Hwnd, TRUE);

	// fix bug with disabled creation
	// todo: fix this properly
	if (Styles & WS_DISABLED) {
		EnableWindow(this->m_Hwnd, TRUE);
		EnableWindow(this->m_Hwnd, FALSE);
	}
}

/*!
* \brief blah
*
* blah
*/

DcxComboEx::~DcxComboEx( ) {

	ImageList_Destroy( this->getImageList( ) );

	this->unregistreDefaultWindowProc( );
}

/*!
* \brief blah
*
* blah
*/

void DcxComboEx::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	//*ExStyles |= CBES_EX_NOSIZELIMIT;
	unsigned int i = 1, numtok = styles.numtok( );

	while ( i <= numtok ) {

		if ( styles.gettok( i ) == "simple" )
			*Styles |= CBS_SIMPLE;
		else if ( styles.gettok( i ) == "dropdown" )
			*Styles |= CBS_DROPDOWNLIST;
		else if ( styles.gettok( i ) == "dropedit" )
			*Styles |= CBS_DROPDOWN;

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

void DcxComboEx::parseInfoRequest( TString & input, char * szReturnValue ) {

	int numtok = input.numtok( );

	TString prop(input.gettok( 3 ));

	// [NAME] [ID] [PROP] [N]
	if ( prop == "text" && numtok > 3 ) {

		int nItem = input.gettok( 4 ).to_int( ) - 1;

		if ( nItem > -1 ) {

			COMBOBOXEXITEM cbi;
			ZeroMemory( &cbi, sizeof( COMBOBOXEXITEM ) );

			cbi.mask = CBEIF_TEXT;
			cbi.iItem = nItem;
			cbi.pszText = szReturnValue;
			cbi.cchTextMax = MIRC_BUFFER_SIZE_CCH;

			this->getItem( &cbi );
			return;
		}
		else if ( nItem == -1 && ( this->isStyle( CBS_DROPDOWN ) || this->isStyle( CBS_SIMPLE ) ) ) {

			GetWindowText( (HWND) this->getEditControl( ), szReturnValue, MIRC_BUFFER_SIZE_CCH );
			return;
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == "seltext" ) {

		int nItem = this->getCurSel( );

		if ( nItem > -1 ) {

			COMBOBOXEXITEM cbi;
			ZeroMemory( &cbi, sizeof( COMBOBOXEXITEM ) );

			cbi.mask = CBEIF_TEXT;
			cbi.iItem = nItem;
			cbi.pszText = szReturnValue;
			cbi.cchTextMax = MIRC_BUFFER_SIZE_CCH;

			this->getItem( &cbi );
			return;
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == "sel" ) {

		int nItem = this->getCurSel( );

		if ( nItem > -1 ) {

			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, "%d", nItem + 1 );
			return;
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == "num" ) {

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, "%d", this->getCount( ) );
		return;
	}
	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N]
	else if ( prop == "find" && numtok > 5 ) {

		TString matchtext(input.gettok(2, TSTAB).trim());
		TString params(input.gettok(3, TSTAB).trim());

		if ( matchtext.len( ) > 0 ) {

			UINT SearchType;

			if ( params.gettok( 1 ) == "R" )
				SearchType = CBEXSEARCH_R;
			else
				SearchType = CBEXSEARCH_W;

			int N = params.gettok( 2 ).to_int( );

			// count total
			if ( N == 0 ) {

				int nItems = this->getCount( ), i = 0, count = 0;

				while ( i < nItems ) {

					if ( this->matchItemText( i, &matchtext, SearchType ) )
						count++;

					i++;
				}

				wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, "%d", count );
				return;
			}
			// find Nth matching
			else {

				int nItems = this->getCount( ), i = 0, count = 0;

				while ( i < nItems ) {

					if ( this->matchItemText( i, &matchtext, SearchType ) )
						count++;

					if ( count == N ) {

						wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, "%d", i + 1 );
						return;
					}

					i++;
				}
			} // else
		}
		return;
	}
	// [NAME] [ID] [PROP] [ROW]
	else if ( prop == "markeditem" && numtok == 3 ) {

		int nItem = input.gettok( 4 ).to_int( ) - 1;

		if ( nItem > -1 && nItem < (int)m_vItemDataList.size()) {
			lstrcpyn( szReturnValue,  m_vItemDataList.at( nItem ).tsMark.to_chr(), MIRC_BUFFER_SIZE_CCH );
		}
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

void DcxComboEx::parseCommandRequest(TString &input) {
	XSwitchFlags flags(input.gettok(3));

	int numtok = input.numtok( );

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags['r']) {
		this->resetContent();
	}

	// xdid -a [NAME] [ID] [SWITCH] [N] [INDENT] [ICON] [STATE] [OVERLAY] Item Text
	if (flags['a'] && numtok > 8) {
		int nPos   = input.gettok( 4 ).to_int() -1;
		int indent = input.gettok( 5 ).to_int();
		int icon   = input.gettok( 6 ).to_int() -1;
		int state  = input.gettok( 7 ).to_int() -1;
		//int overlay = input.gettok( 8 ).to_int() - 1;
		TString itemtext(input.gettok( 9, -1));

		if (nPos == -2) {
			if (IsWindow(this->m_EditHwnd))
				SetWindowText(this->m_EditHwnd, itemtext.to_chr());

			//SendMessage(this->m_EditHwnd, WM_SETTEXT,0, (LPARAM)itemtext.to_chr());
		}
		else {
			COMBOBOXEXITEM cbi;

			ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));

			cbi.mask = CBEIF_TEXT | CBEIF_INDENT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
			cbi.iIndent = indent;
			cbi.iImage = icon;
			cbi.iSelectedImage = state;
			//cbi.iOverlay = overlay;
			cbi.pszText = itemtext.to_chr();
			cbi.iItem = nPos;

			DCXCBITEM mycbi;

			nPos = this->insertItem(&cbi);
			this->m_vItemDataList.insert(this->m_vItemDataList.begin() + nPos, (const DCXCBITEM)mycbi);

			// Now update the horizontal scroller
			HWND combo = (HWND)SendMessage(this->m_Hwnd, CBEM_GETCOMBOCONTROL, NULL, NULL);

			if (IsWindow(combo)) {
				// Get Font sizes (best way i can find atm, if you know something better then please let me know)
				int nMaxStrlen = itemtext.len();
				int nHorizExtent = (int)SendMessage( combo, CB_GETHORIZONTALEXTENT, NULL, NULL );
				HDC hdc = GetDC( this->m_Hwnd );
				TEXTMETRIC tm;
				HFONT hFont = this->getFont();

				HFONT hOldFont = SelectFont(hdc, hFont);

				GetTextMetrics(hdc, &tm);

				SelectFont(hdc, hOldFont);

				ReleaseDC( this->m_Hwnd, hdc);

				// Multiply max str len by font average width + 1
				nMaxStrlen *= (tm.tmAveCharWidth + tm.tmOverhang);
				// Add 2 * chars as spacer.
				nMaxStrlen += (tm.tmAveCharWidth * 2);

				if (nMaxStrlen > nHorizExtent)
					SendMessage( combo, CB_SETHORIZONTALEXTENT, nMaxStrlen, NULL);
			}
		}
	}
	// xdid -A [NAME] [ID] [ROW] [+FLAGS] [INFO]
	else if (flags['A']) {
		int n = input.numtok();

		if (n < 5) {
			this->showErrorEx(NULL, "-A", "Insufficient parameters");
			return;
		}

		int nRow = input.gettok(3).to_int();

		// We're currently checking 1-based indexes.
		if ((nRow < 1) || (nRow > this->getCount())) {
			this->showErrorEx(NULL, "-A", "Invalid row index %d.", nRow);
			return;
		}

		// Convert to 0-based index.
		nRow--;

		COMBOBOXEXITEM cbei;
		ZeroMemory(&cbei, sizeof(COMBOBOXEXITEM));

		cbei.mask = CBEIF_LPARAM;
		cbei.iItem = nRow;

		// Couldnt retrieve info
		if (!SendMessage(this->m_Hwnd, CBEM_GETITEM, NULL, (LPARAM)&cbei)) {
			this->showError(NULL, "-A", "Unable to get item.");
			return;
		}

		LPDCXCBITEM cbiDcx = (LPDCXCBITEM) cbei.lParam;

		TString flag(input.gettok(6));
		TString info(input.gettok(7, -1));

		if (flag.find('M', 1) > 0)
			cbiDcx->tsMark = info;
		else
			this->showErrorEx(NULL, "-A", "Unknown flags %s", flag.to_chr());

		return;
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags['c'] && numtok > 3) {
		int nItem = input.gettok( 4 ).to_int() -1;

		if (nItem > -1) {
			this->setCurSel(nItem);
		}
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags['d'] && numtok > 3) {
		int nItem = input.gettok( 4 ).to_int() -1;

		if (nItem > -1 && nItem < this->getCount())
			this->deleteItem(nItem);

		if (!this->getCount())
			this->redrawWindow();
	}
	// This is to avoid invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags['r']) {
		//this->resetContent();
	}
	// xdid -u [NAME] [ID] [SWITCH]
	else if (flags['u']) {
		this->setCurSel(-1);
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags['w'] && numtok > 5) {
		HIMAGELIST himl;
		TString flag(input.gettok( 4 ));
		int index = input.gettok( 5 ).to_int();;
		TString filename(input.gettok(6, -1));

		if ((himl = this->getImageList()) == NULL) {
			himl = this->createImageList();

			if (himl)
				this->setImageList(himl);
		}

		if (himl != NULL) {
			HICON icon = dcxLoadIcon(index, filename, false, flag);

			//if (flag.find('g', 0))
			//	icon = CreateGrayscaleIcon(icon);

			ImageList_AddIcon(himl, icon);
			DestroyIcon(icon);
		}
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if (flags['y']) {
		ImageList_Destroy(this->getImageList());
		this->setImageList(NULL);
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxComboEx::getImageList(  ) {

	return (HIMAGELIST) SendMessage( this->m_Hwnd, CBEM_GETIMAGELIST, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

void DcxComboEx::setImageList( HIMAGELIST himl ) {

	SendMessage( this->m_Hwnd, CBEM_SETIMAGELIST, (WPARAM) 0, (LPARAM) himl );
}

/*!
* \brief blah
*
* blah
*/

HIMAGELIST DcxComboEx::createImageList( ) {

	return ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
}

/*!
* \brief blah
*
* blah
*/

BOOL DcxComboEx::matchItemText( const int nItem, const TString * search, const UINT SearchType ) {

	char itemtext[MIRC_BUFFER_SIZE_CCH];

	COMBOBOXEXITEM cbi;
	ZeroMemory( &cbi, sizeof( COMBOBOXEXITEM ) );

	cbi.mask = CBEIF_TEXT;
	cbi.iItem = nItem;
	cbi.pszText = itemtext;
	cbi.cchTextMax = MIRC_BUFFER_SIZE_CCH;

	this->getItem( &cbi );

	if ( SearchType == CBEXSEARCH_R )
		return isRegexMatch(itemtext, search->to_chr());

	return TString(itemtext).iswm(search->to_chr());
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::insertItem( const PCOMBOBOXEXITEM lpcCBItem ) {
	return SendMessage( this->m_Hwnd, CBEM_INSERTITEM, (WPARAM) 0, (LPARAM) lpcCBItem );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getItem( PCOMBOBOXEXITEM lpcCBItem ) const {
	return SendMessage( this->m_Hwnd, CBEM_GETITEM, (WPARAM) 0, (LPARAM) lpcCBItem );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getEditControl( ) const {
	return SendMessage( this->m_Hwnd, CBEM_GETEDITCONTROL, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::deleteItem( const int iIndex ) {
	return SendMessage( this->m_Hwnd, CBEM_DELETEITEM, (WPARAM) iIndex, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::setCurSel( const int iIndex ) {
	return SendMessage( this->m_Hwnd, CB_SETCURSEL, (WPARAM) iIndex, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getCurSel( ) const {
	return SendMessage( this->m_Hwnd, CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getLBText( const int iIndex, LPSTR lps ) {
	return SendMessage( this->m_Hwnd, CB_GETLBTEXT, (WPARAM) iIndex, (LPARAM) lps );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::resetContent( ) {
	return SendMessage( this->m_Hwnd, CB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::getCount( ) const {
	return SendMessage( this->m_Hwnd, CB_GETCOUNT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxComboEx::limitText( const int iLimit ) {
	return SendMessage( this->m_Hwnd, CB_LIMITTEXT, (WPARAM) iLimit, (LPARAM) 0 );
}

TString DcxComboEx::getStyles(void) {
	TString styles(__super::getStyles());
	DWORD Styles;
	Styles = GetWindowStyle(this->m_Hwnd);
	if (Styles & CBS_SIMPLE)
		styles.addtok("simple", " ");
	if (Styles & CBS_DROPDOWNLIST) 
		styles.addtok("dropdown", " ");
	if (Styles & CBS_DROPDOWN) 
		styles.addtok("dropedit", " ");
	return styles;
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxComboEx::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch ( uMsg ) 
	{
	case WM_COMMAND: 
		switch ( HIWORD( wParam ) )
		{
		case CBN_DBLCLK:
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				this->execAliasEx("%s,%d,%d", "dclick", this->getUserID( ), this->getCurSel( ) + 1 );
			bParsed = TRUE;
			return TRUE;
			break;

		case CBN_SELENDOK:
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				this->execAliasEx("%s,%d,%d", "sclick", this->getUserID( ), this->getCurSel( ) + 1 );
			char itemtext[500];
			COMBOBOXEXITEM cbex;
			ZeroMemory( &cbex, sizeof( COMBOBOXEXITEM ) );
			cbex.mask = CBEIF_TEXT;
			cbex.pszText = itemtext;
			cbex.cchTextMax = 500;
			cbex.iItem = this->getCurSel( );
			this->getItem( &cbex );
			SetWindowText( this->m_EditHwnd, itemtext );
			bParsed = TRUE;
			return TRUE;
			break;
		case CBN_EDITCHANGE:
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_EDIT)
				this->execAliasEx("%s,%d", "edit", this->getUserID( ) );
			bParsed = TRUE;
			return TRUE;
			break;
		} // switch
		bParsed = TRUE;
		break;
	case WM_NOTIFY:
		NMHDR * nhmdr;
		nhmdr = (LPNMHDR)lParam;
		switch (nhmdr->code) 
		{
		case CBEN_ENDEDIT:
			NMCBEENDEDIT * endedit;
			endedit = (LPNMCBEENDEDIT) lParam;
			if (endedit->iWhy == CBENF_RETURN) {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_EDIT)
					this->execAliasEx("%s,%d", "return", this->getUserID( ) );
			}
			break;
		case CBEN_DELETEITEM:
			PNMCOMBOBOXEX lpcb = (PNMCOMBOBOXEX ) lParam;
			LPDCXCBITEM lpdcxcbi = (LPDCXCBITEM) lpcb->ceItem.lParam;

			if (lpdcxcbi != NULL) {
				delete lpdcxcbi;
			}
			bParsed = TRUE; // message has been handled
			break;
		}
		break;
	}
	return 0L;
}

LRESULT DcxComboEx::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {
	case WM_LBUTTONUP:
		{
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				this->execAliasEx("%s,%d", "lbup", this->getUserID( ) );
		}
		break;
	case WM_LBUTTONDBLCLK:
	case WM_CONTEXTMENU:
		break;

	case WM_MOUSEACTIVATE:
		{
			switch (HIWORD(lParam))
			{
			case WM_RBUTTONDOWN:
				{
					// NB: rclick doesnt change selection!
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
						this->execAliasEx("%s,%d,%d", "rclick", this->getUserID( ), this->getCurSel( ) + 1 );
				}
				break;
			}
			//TODO: Add `ownmenu` setting or similar to stop default edit menu & replace with own.
			// this could be done with most if not all controls.
			//bParsed = TRUE;
			//return MA_ACTIVATE;
		}
		break;
	case WM_NCDESTROY:
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

/*!
* \brief blah
*
* blah
*/

LRESULT CALLBACK DcxComboEx::ComboExEditProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

	LPDCXCOMBOEXEDIT lpce = (LPDCXCOMBOEXEDIT) GetWindowLongPtr( mHwnd, GWLP_USERDATA );
	if (lpce == NULL)	return DefWindowProc( mHwnd, uMsg, wParam, lParam );
	//mIRCError( "DcxComboEx::ComboExEditProc" );
	switch( uMsg ) {
		/*
		case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS | CallWindowProc( lpce->OldProc, mHwnd, uMsg, wParam, lParam );
		break;

		case WM_KEYDOWN:
		{
		if ( wParam == VK_RETURN ) {

		DcxControl * pthis = (DcxControl *) GetProp( lpce->cHwnd, "dcx_cthis" );

		if ( pthis != NULL ) {
		pthis->callAliasEx( NULL, "%s,%d", "return", pthis->getUserID( ) );
		}
		}
		}
		break;
		//case WM_NOTIFY:
		//	{
		//      LPNMHDR hdr = (LPNMHDR) lParam;
		//      if (!hdr)
		//        break;

		//      switch( hdr->code ) {
		//		case TTN_GETDISPINFO:
		//			{
		//         DcxControl * pthis = (DcxControl *) GetProp( lpce->cHwnd, "dcx_cthis" );
		//         if ( pthis != NULL ) {
		//					LPNMTTDISPINFO di = (LPNMTTDISPINFO)lParam;
		//					di->lpszText = "test";
		//					di->hinst = NULL;
		//				}
		//				return 0L;
		//			}
		//			break;
		//		case TTN_LINKCLICK:
		//			{
		//         DcxControl * pthis = (DcxControl *) GetProp( lpce->cHwnd, "dcx_cthis" );
		//         if ( pthis != NULL ) {
		//					pthis->callAliasEx( NULL, "%s,%d", "tooltiplink", pthis->getUserID( ) );
		//				}
		//				return 0L;
		//			}
		//			break;
		//		}
		//	}
		//	break;*/
		case LB_GETITEMRECT:
		{
		// This fixes the lockup on clicking the comboex editbox. (why?)
		// this is not a real fix, but a workaround.
		// NB: when we dont replace the editbox wndproc this crash doesnt happen.
		// mIRC sends this message to it's child windows, no idea why
		return LB_ERR;
		}
		break;

	case WM_NCDESTROY:
		{

			WNDPROC OldProc = lpce->OldProc;
			if ( lpce )
				delete lpce;

			return CallWindowProc( OldProc, mHwnd, uMsg, wParam, lParam );
		}
		break;
	}
	return CallWindowProc( lpce->OldProc, mHwnd, uMsg, wParam, lParam );
}
