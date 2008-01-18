/*!
 * \file dcxstatusbar.cpp
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

#include "dcxstatusbar.h"
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

DcxStatusBar::DcxStatusBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
, m_hImageList(NULL)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CONTROLPARENT,
		DCX_STATUSBARCLASS,
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

	this->m_vParts.clear();
	ZeroMemory(this->m_iDynamicParts, sizeof(this->m_iDynamicParts));
	ZeroMemory(this->m_iFixedParts, sizeof(this->m_iFixedParts));

	if ((rc->bottom - rc->top) > 0)
		SendMessage(this->m_Hwnd,SB_SETMINHEIGHT,rc->bottom - rc->top,0);

	this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxStatusBar::~DcxStatusBar( ) {

	VectorOfParts::iterator itStart = this->m_vParts.begin();
	VectorOfParts::iterator itEnd = this->m_vParts.end();

	while (itStart != itEnd) {
		if (*itStart != NULL) {
			delete *itStart;
		}
		itStart++;
	}
	this->cleanPartIcons( );
	ImageList_Destroy( this->getImageList( ) );

	this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	unsigned int i = 1, numtok = styles.numtok( );

	while ( i <= numtok ) {

		if ( styles.gettok( i ) == "grip" )
			*Styles |= SBARS_SIZEGRIP;
		else if ( styles.gettok( i ) == "tooltips" )
			*Styles |= SBARS_TOOLTIPS;
		else if ( styles.gettok( i ) == "nodivider" )
			*Styles |= CCS_NODIVIDER;
		else if ( styles.gettok( i ) == "top" ) {
			*Styles |= CCS_TOP;
			*Styles &= ~SBARS_SIZEGRIP; // size grip doesn't work for left or top styles.
		}
		else if ( styles.gettok( i ) == "noresize" )
			*Styles |= CCS_NORESIZE;
		else if ( styles.gettok( i ) == "noparentalign" )
			*Styles |= CCS_NOPARENTALIGN ;
		else if ( styles.gettok( i ) == "noauto" )
			*Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
		//else if ( styles.gettok( i ) == "left" )
		//{ // NB: left & right styles don't render the parts vertically.
		//	*Styles |= CCS_LEFT;
		//	*Styles &= ~SBARS_SIZEGRIP;
		//}
		//else if ( styles.gettok( i ) == "right" )
		//	*Styles |= CCS_RIGHT;

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

void DcxStatusBar::parseInfoRequest( TString & input, char * szReturnValue ) {

	int numtok = input.numtok( );
	TString prop(input.gettok( 3 ));

	// [NAME] [ID] [PROP] [N]
	if ( prop == "text" && numtok > 3 ) {

		int iPart = input.gettok( 4 ).to_int( ) -1, nParts = this->getParts( 256, 0 );

		if ( iPart > -1 && iPart < nParts ) {

			this->getText( iPart, szReturnValue ); // possible overflow, needs fixed at some point.
			return;
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == "parts" ) {

		INT parts[256];
		int nParts = this->getParts( 256, 0 );

		this->getParts( 256, parts );

		int i = 0;
		szReturnValue[0] = 0;
		char d[10];

		while ( i < nParts ) {

			wsprintf( d, "%d", parts[i] );

			if ( i != 0 )
				lstrcat( szReturnValue, " " );
			lstrcat( szReturnValue, d );

			i++;
		}
		return;
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == "tooltip" && numtok > 3 ) {

		int iPart = input.gettok( 4 ).to_int( ), nParts = this->getParts( 256, 0 );

		if ( iPart > -1 && iPart < nParts ) {

			this->getTipText( iPart, 900, szReturnValue );
			return;
		}
	}
	else if ( this->parseGlobalInfoRequest( input, szReturnValue ) ) {

		return;
	}

	szReturnValue[0] = 0;
}

void DcxStatusBar::deletePartInfo(const int iPart)
{
	if (HIWORD( this->getTextLength( iPart ) ) & SBT_OWNERDRAW) {
		LPSB_PARTINFO pPart = (LPSB_PARTINFO)this->getText(iPart, NULL);
		VectorOfParts::iterator itStart = this->m_vParts.begin();
		VectorOfParts::iterator itEnd = this->m_vParts.end();

		while (itStart != itEnd) {
			if (*itStart != NULL && ((LPSB_PARTINFO)*itStart) == pPart) {
				this->setText( iPart, SBT_OWNERDRAW, NULL);
				delete pPart->m_Child;
				delete pPart;
				this->m_vParts.erase(itStart);
				return;
			}
			itStart++;
		}
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::parseCommandRequest( TString & input ) {
	XSwitchFlags flags(input.gettok(3));

	int numtok = input.numtok( );

	// xdid -k [NAME] [ID] [SWITCH] [COLOR]
	if (flags['k'] && numtok > 3) {
		int col = input.gettok( 4 ).to_int();

		if (col < 0)
			this->setBkColor((COLORREF) CLR_DEFAULT);
		else
			this->setBkColor((COLORREF) col);
	}
	// xdid -l [NAME] [ID] [SWITCH] [POS [POS POS ...]]
	else if ( flags['l'] && numtok > 3 ) {

		int nParts = numtok - 3;
		INT parts[256];

		int i = 0, c = 0, t = 0;
		TString p;
		while ( i < nParts ) {

			if (c >= 100) {
				this->showError(NULL, "-l", "Can't Allocate Over 100% of Statusbar!");
				return;
			}

			p = input.gettok( i+4 );
			t = p.to_int();

			if (p.right(1) == "%") {
				this->m_iDynamicParts[i] = t;
				c += t;
			}
			else
				this->m_iFixedParts[i] = t;

			parts[i] = t;
			i++;
		}
		this->setParts( nParts, parts );
		this->updateParts();
	}
	// xdid -t [NAME] [ID] [SWITCH] N [+FLAGS] [#ICON] [Cell Text][TAB]Tooltip Text
	// xdid -t [NAME] [ID] [SWITCH] N [+c] [#ICON] [CID] [CTRL] [X] [Y] [W] [H] (OPTIONS)
	else if ( flags['t'] && numtok > 5 ) {

		int nPos = input.gettok( 4 ).to_int( ) - 1;
		TString flag(input.gettok( 5 ));
		int icon = input.gettok( 6 ).to_int( ) - 1;

		if ( nPos < 0 || nPos >= this->getParts( 256, 0 ) ) {
			this->showError(NULL, "-t", "Invalid Part");
			return;
		}

		TString itemtext;
		TString tooltip;

		this->deletePartInfo(nPos); // delete custom info if any.
		DestroyIcon( (HICON) this->getIcon( nPos ) ); // delete any icon for this part.
		this->setIcon( nPos, NULL ); // set as no icon.

		UINT iFlags = this->parseItemFlags( flag );

		if ( input.gettok( 1, TSTAB ).numtok( ) > 6 ) {
		  
			itemtext = input.gettok( 1, TSTAB ).gettok( 7, -1 );
			itemtext.trim( );
		}

		if ( input.numtok( TSTAB ) > 1 ) {
		  
			tooltip = input.gettok( 2, TSTAB );
			tooltip.trim( );
		}

		if (iFlags & SBT_OWNERDRAW) {
			LPSB_PARTINFO pPart = new SB_PARTINFO;
			ZeroMemory(pPart,sizeof(SB_PARTINFO));
			pPart->m_iIcon = icon;
			if (flag.find('f',0)) { // mIRC formatted text
				pPart->m_Text = itemtext;
				this->setTipText( nPos, tooltip.to_chr( ) );
			}
			else { // child control
				UINT ID = mIRC_ID_OFFSET + (UINT)itemtext.gettok( 1 ).to_int( );

				if ( ID > mIRC_ID_OFFSET - 1 && 
					!IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
					this->m_pParentDialog->getControlByID( ID ) == NULL )
				{
					try {
						DcxControl * p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,itemtext,2,
										CTLF_ALLOW_PBAR|CTLF_ALLOW_TRACKBAR|CTLF_ALLOW_COMBOEX|
										 CTLF_ALLOW_STATUSBAR|CTLF_ALLOW_TOOLBAR|
										 CTLF_ALLOW_TREEVIEW|CTLF_ALLOW_LISTVIEW|CTLF_ALLOW_REBAR|
										 CTLF_ALLOW_BUTTON|CTLF_ALLOW_EDIT|
										 CTLF_ALLOW_UPDOWN| CTLF_ALLOW_IPADDRESS|CTLF_ALLOW_WEBCTRL|
										 CTLF_ALLOW_CALANDER|CTLF_ALLOW_DIVIDER|CTLF_ALLOW_PANEL|
										 CTLF_ALLOW_TAB|CTLF_ALLOW_LINE|CTLF_ALLOW_BOX|CTLF_ALLOW_RADIO|
										 CTLF_ALLOW_CHECK|CTLF_ALLOW_TEXT|CTLF_ALLOW_SCROLL|CTLF_ALLOW_LIST|
										 CTLF_ALLOW_LINK|CTLF_ALLOW_IMAGE|CTLF_ALLOW_PAGER|CTLF_ALLOW_DATETIME|
										 CTLF_ALLOW_STACKER|CTLF_ALLOW_DIRECTSHOW,this->m_Hwnd);
						//DcxControl * p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,itemtext,2,-1,this->m_Hwnd);
						// problems with colorcombo/richedit, causes odd gfx glitches & dialog slow down.
						if ( p_Control != NULL ) {
							this->m_pParentDialog->addControl( p_Control );
							pPart->m_Child = p_Control;
							ShowWindow(p_Control->getHwnd(),SW_HIDE); // hide control untill a WM_DRAWITEM is recieved.
							SendMessage(this->m_Hwnd,WM_SIZE,0,0);
						}
					}
					catch ( char *err ) {
						this->showErrorEx(NULL, "-t", "Unable To Create Control %d (%s)", ID - mIRC_ID_OFFSET, err);
						delete pPart;
						return;
					}
				}
				else {
					this->showErrorEx(NULL, "-t", "Control with ID \"%d\" already exists", ID - mIRC_ID_OFFSET );
					delete pPart;
					return;
				}
			}
			this->setPartInfo( nPos, iFlags, pPart );
		}
		else {

			if ( icon != -1 )
				this->setIcon( nPos, ImageList_GetIcon( this->getImageList( ), icon, ILD_TRANSPARENT ) );

			this->setText( nPos, iFlags, itemtext.to_chr( ) );
			this->setTipText( nPos, tooltip.to_chr( ) );
		}
	}
	// xdid -v [NAME] [ID] [SWITCH] [N] (TEXT)
	else if ( flags['v'] && numtok > 3 ) {

		int nPos = input.gettok( 4 ).to_int( ) - 1;

		if ( nPos > -1 && nPos < this->getParts( 256, 0 ) ) {

			TString itemtext;
			if ( numtok > 4 )
				itemtext = input.gettok( 5, -1 );

			if (HIWORD( this->getTextLength( nPos ) ) & SBT_OWNERDRAW) {
				LPSB_PARTINFO pPart = (LPSB_PARTINFO)this->getText(nPos, NULL);
				pPart->m_Text = itemtext;
				this->setPartInfo( nPos, SBT_OWNERDRAW, pPart );
			}
			else {
				char text[900];
				this->setText( nPos, HIWORD( this->getText( nPos, text ) ), itemtext.to_chr( ) );
			}
		}
		else
			this->showError(NULL, "-v", "Invalid Part");
	}
	// xdid -w [NAME] [ID] [SWITCH] [FLAGS] [INDEX] [FILENAME]
	else if (flags['w'] && numtok > 5) {
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
	else if ( flags['y'] ) {

		ImageList_Destroy( this->getImageList( ) );
		this->setImageList(NULL);
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxStatusBar::getImageList( ) {

  return this->m_hImageList;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::setImageList( HIMAGELIST himl ) {

  this->m_hImageList = himl;
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxStatusBar::createImageList( ) {

  return ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxStatusBar::parseItemFlags( TString & flags ) {

	INT i = 1, len = flags.len( ), iFlags = 0;

	// no +sign, missing params
	if ( flags[0] != '+' ) 
		return iFlags;

	while ( i < len ) {
		switch(flags[i])
		{
		case 'n':
			iFlags |= SBT_NOBORDERS;
			break;
		case 'p':
			iFlags |= SBT_POPOUT;
			break;
		case 'c':
			iFlags |= SBT_OWNERDRAW;
		case 'f':
			iFlags |= SBT_OWNERDRAW;
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

void DcxStatusBar::cleanPartIcons( ) {

  int n = 0;
  while ( n < 256 ) {

    DestroyIcon( (HICON) this->getIcon( n ) );
    n++;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setParts( const int nParts, const LPINT aWidths ) {
  return SendMessage( this->m_Hwnd, SB_SETPARTS, (WPARAM) nParts, (LPARAM) aWidths );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getParts( const int nParts, LPINT aWidths ) const {
  return SendMessage( this->m_Hwnd, SB_GETPARTS, (WPARAM) nParts, (LPARAM) aWidths );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getBorders( LPINT aWidths ) const {
  return SendMessage( this->m_Hwnd, SB_GETBORDERS, (WPARAM) 0, (LPARAM) aWidths );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setBkColor( const COLORREF clrBk ) {
  return SendMessage( this->m_Hwnd, SB_SETBKCOLOR, (WPARAM) 0, (LPARAM) clrBk );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setText( const int iPart, const int Style, const LPSTR lpstr ) {
  return SendMessage( this->m_Hwnd, SB_SETTEXT, (WPARAM) iPart | Style, (LPARAM) lpstr );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setPartInfo( const int iPart, const int Style, const LPSB_PARTINFO pPart) {
  return SendMessage( this->m_Hwnd, SB_SETTEXT, (WPARAM) iPart | (Style | SBT_OWNERDRAW), (LPARAM) pPart );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getText( const int iPart, LPSTR lpstr ) const {
  return SendMessage( this->m_Hwnd, SB_GETTEXT, (WPARAM) iPart, (LPARAM) lpstr );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getTextLength( const int iPart ) const {
  return SendMessage( this->m_Hwnd, SB_GETTEXTLENGTH, (WPARAM) iPart, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setTipText( const int iPart, const LPSTR lpstr ) {
  return SendMessage( this->m_Hwnd, SB_SETTIPTEXT, (WPARAM) iPart, (LPARAM) lpstr );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getTipText( const int iPart, const int nSize, LPSTR lpstr ) const {
  return SendMessage( this->m_Hwnd, SB_GETTIPTEXT, (WPARAM) MAKEWPARAM (iPart, nSize), (LPARAM) lpstr );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getRect( const int iPart, LPRECT lprc ) const {
  return SendMessage( this->m_Hwnd, SB_GETRECT, (WPARAM) iPart, (LPARAM) lprc );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setIcon( const int iPart, const HICON hIcon ) {
  return SendMessage( this->m_Hwnd, SB_SETICON, (WPARAM) iPart, (LPARAM) hIcon );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getIcon( const int iPart ) const {
  return SendMessage( this->m_Hwnd, SB_GETICON, (WPARAM) iPart, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxStatusBar::hitTest( const POINT & pt ) const {

  RECT rc;
  int n = 0;
  //int tx = 0;
  int nParts = this->getParts( 256, 0 );

  while ( n < nParts ) {

    this->getRect( n, &rc );
    if ( PtInRect( &rc, pt ) )
      return n;

    n++;
  }
  return -1;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxStatusBar::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {
	case WM_NOTIFY:
		{
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			switch( hdr->code ) {

			case NM_CLICK:
				{
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
						POINT pt;
						GetCursorPos( &pt );
						ScreenToClient( this->m_Hwnd, &pt );
						int cell = this->hitTest( pt );

						if ( cell != -1 )
							this->callAliasEx( NULL, "%s,%d,%d", "sclick", this->getUserID( ), cell + 1 );
					}
					bParsed = TRUE;
				}
				break;

			case NM_RCLICK:
				{
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
						POINT pt;
						GetCursorPos( &pt );
						ScreenToClient( this->m_Hwnd, &pt );
						int cell = this->hitTest( pt );

						if ( cell != -1 )
							this->callAliasEx( NULL, "%s,%d,%d", "rclick", this->getUserID( ), cell + 1 );
					}
					bParsed = TRUE;
				}
				break;

			case NM_DBLCLK: 
				{
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
						POINT pt;
						GetCursorPos( &pt );
						ScreenToClient( this->m_Hwnd, &pt );
						int cell = this->hitTest( pt );

						if ( cell != -1 )
							this->callAliasEx( NULL, "%s,%d,%d", "dclick", this->getUserID( ), cell + 1 );
					}
					bParsed = TRUE;
				}
				break;

			} // switch
		}
		break;
	case WM_DRAWITEM: // support for ownerdraw statusbar. NB: NO Delete Item msg.
		{
			LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;
			if (this->m_Hwnd == lpDrawItem->hwndItem) {
				bParsed = TRUE;
				LPSB_PARTINFO pPart = (LPSB_PARTINFO)lpDrawItem->itemData;
				if (pPart != NULL) {
					RECT rc = lpDrawItem->rcItem;
					if (pPart->m_iIcon > -1) {
						IMAGEINFO ii;
						ImageList_GetImageInfo(this->m_hImageList, pPart->m_iIcon, &ii);
						ImageList_Draw(this->m_hImageList, pPart->m_iIcon, lpDrawItem->hDC, rc.left, rc.top + ((rc.bottom - rc.top) - (ii.rcImage.bottom - ii.rcImage.top)) / 2, ILD_TRANSPARENT);
						rc.left += (ii.rcImage.right - ii.rcImage.left);
					}
					if (pPart->m_Text.len() > 0)
						mIRC_DrawText(lpDrawItem->hDC, pPart->m_Text, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE, false, this->m_bUseUTF8);
					else if (pPart->m_Child != NULL) {
						SetWindowPos(pPart->m_Child->getHwnd(), NULL, rc.left, rc.top,
							(rc.right - rc.left), (rc.bottom - rc.top), SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
					}
					return TRUE;
				}
			}
		}
		break;
	}
	return 0L;
}

LRESULT DcxStatusBar::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
	switch( uMsg ) {
		case WM_CONTEXTMENU:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
			break;

		case WM_HSCROLL:
		case WM_VSCROLL:
		case WM_COMMAND:
			{
				if (IsWindow((HWND) lParam)) {
					DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,"dcx_cthis");
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_COMPAREITEM:
			{
				LPCOMPAREITEMSTRUCT idata = (LPCOMPAREITEMSTRUCT)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DELETEITEM:
			{
				DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_MEASUREITEM:
			{
				HWND cHwnd = GetDlgItem(this->m_Hwnd, wParam);
				if (IsWindow(cHwnd)) {
					DcxControl *c_this = (DcxControl *) GetProp(cHwnd,"dcx_cthis");
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DRAWITEM:
			{
				DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_NOTIFY: 
			{
				LPNMHDR hdr = (LPNMHDR) lParam;

				if (!hdr)
					break;

				if (this->m_Hwnd != hdr->hwndFrom) {
					if (IsWindow(hdr->hwndFrom)) {
						DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,"dcx_cthis");
						if (c_this != NULL)
							lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
			}
			break;

		case WM_SIZE:
			{
				this->updateParts();
			}
			break;

			//case WM_PAINT:
			//	{
			//		PAINTSTRUCT ps;
			//		HDC hdc;

			//		hdc = BeginPaint( this->m_Hwnd, &ps );

			//		bParsed = TRUE;

			//		// Setup alpha blend if any.
			//		LPALPHAINFO ai = this->SetupAlphaBlend(&hdc, true);

			//		DcxControl::DrawCtrlBackground( hdc, this);

			//		lRes = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

			//		this->FinishAlphaBlend(ai);

			//		EndPaint( this->m_Hwnd, &ps );
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

void DcxStatusBar::updateParts(void) {
	int nParts = this->getParts(0,NULL);

	if (nParts <= 0)
		return;

	RECT rcClient;
	int *pParts = new int[nParts];
	//int borders[3];
	int w, pw = 0;

	GetClientRect(this->m_Hwnd, &rcClient);
	this->getParts(nParts, pParts);

	//this->getBorders(borders);

	w = (rcClient.right - rcClient.left); // - (2 * borders[1]);

	for (int i = 0; i < nParts; i++) {
		if (this->m_iDynamicParts[i] != 0)
			pw = (w / 100) * this->m_iDynamicParts[i];
		else
			pw = this->m_iFixedParts[i];
		if (i == 0)
			pParts[i] = pw;
		else {
			if (pw == -1)
				pParts[i] = -1;
			else
				pParts[i] = (pParts[i-1] + pw);
		}
	}

	this->setParts(nParts, pParts);
	delete [] pParts;
}
