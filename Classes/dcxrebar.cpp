/*!
 * \file dcxrebar.cpp
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
#include "Classes/dcxrebar.h"
#include "Classes/dcxdialog.h"
#include "Dcx.h"



/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxReBar::DcxReBar( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, const TString & styles ) 
: DcxControl( ID, p_Dialog ) 
, m_iClickedBand(-1)
, m_iRowLimit(0)
, m_iWidth(0)
, m_iHeight(0)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CONTROLPARENT,
		DCX_REBARCTRLCLASS,
		NULL,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw TEXT("Unable To Create Window");

	if ( bNoTheme ) {
		//SendMessage( this->m_Hwnd, RB_SETWINDOWTHEME, NULL, L" ");
		Dcx::UXModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );
	}
	{
		this->setImageList( this->createImageList() );
		//REBARINFO rbi;
		//rbi.cbSize = sizeof(REBARINFO);
		//rbi.fMask = 0;
		//rbi.himl = NULL;
		//this->setBarInfo( &rbi );
	}
	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
}

/*!
 * \brief Destructor
 *
 * blah
 */

DcxReBar::~DcxReBar( ) {

  this->resetContents( );

  ImageList_Destroy( this->getImageList( ) );

  this->unregistreDefaultWindowProc( );
}

TString DcxReBar::getStyles(void) const
{
	TString styles(__super::getStyles());
	const DWORD Styles = GetWindowStyle(this->m_Hwnd);
	if (Styles & RBS_BANDBORDERS)
		styles.addtok(TEXT("borders"));
	if (Styles & RBS_DBLCLKTOGGLE)
		styles.addtok(TEXT("dblclktoggle"));
	if (Styles & RBS_FIXEDORDER)
		styles.addtok(TEXT("fixedorder"));
	if (Styles & RBS_VARHEIGHT)
		styles.addtok(TEXT("varheight"));
	if (Styles & RBS_TOOLTIPS)
		styles.addtok(TEXT("tooltips"));
	if (Styles & RBS_VERTICALGRIPPER)
		styles.addtok(TEXT("verticalgrip"));
	if (Styles & CCS_VERT)
		styles.addtok(TEXT("vertical"));
	if (Styles & CCS_RIGHT)
		styles.addtok(TEXT("right"));
	if (Styles & CCS_BOTTOM)
		styles.addtok(TEXT("bottom"));
	if (Styles & CCS_NORESIZE)
		styles.addtok(TEXT("noresize"));
	if (Styles & CCS_NOPARENTALIGN)
		styles.addtok(TEXT("noparentalign"));
	if (Styles & (CCS_NOPARENTALIGN | CCS_NORESIZE))
		styles.addtok(TEXT("noauto"));
	return styles;

}


void DcxReBar::toXml(TiXmlElement * xml) const {
	__super::toXml(xml);
	const int count = this->getBandCount( );
	if (count > 0) {
		for (int i = 0; i < count; i++) {
			const DcxControl * c = this->getControl(i);
			if (c != NULL) {
				TiXmlElement * subs = new TiXmlElement("control");
				c->toXml(subs);
				xml->LinkEndChild(subs);
			}
		}
	}
}

DcxControl * DcxReBar::getControl(const int index) const {

    if ( index > -1 && index < this->getBandCount( ) ) {
      
      REBARBANDINFO rbBand;
      ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
      rbBand.cbSize = sizeof( REBARBANDINFO );
      rbBand.fMask = RBBIM_CHILD;

      if ( this->getBandInfo( index, &rbBand ) != 0 )
        return this->m_pParentDialog->getControlByHWND( rbBand.hwndChild );

    }
	return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxReBar::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	*Styles |= RBS_AUTOSIZE;

	//*ExStyles |= WS_EX_CONTROLPARENT;

	for (TString tsStyle(styles.getfirsttok( 1 )); tsStyle != TEXT(""); tsStyle = styles.getnexttok( ))
	{
		if ( tsStyle == TEXT("borders") ) 
			*Styles |= RBS_BANDBORDERS;
		else if ( tsStyle == TEXT("dblclktoggle") ) 
			*Styles |= RBS_DBLCLKTOGGLE;
		else if ( tsStyle == TEXT("fixedorder") ) 
			*Styles |= RBS_FIXEDORDER;
		else if ( tsStyle == TEXT("varheight") ) 
			*Styles |= RBS_VARHEIGHT;
		else if ( tsStyle == TEXT("tooltips") ) 
			*Styles |= RBS_TOOLTIPS;
		else if ( tsStyle == TEXT("verticalgrip") ) 
			*Styles |= RBS_VERTICALGRIPPER;
		else if ( tsStyle == TEXT("vertical") ) 
			*Styles |= CCS_VERT;
		else if ( tsStyle == TEXT("right") ) 
			*Styles |= CCS_RIGHT;
		else if ( tsStyle == TEXT("bottom") ) 
			*Styles |= CCS_BOTTOM;
		else if ( tsStyle == TEXT("noresize") ) 
			*Styles |= CCS_NORESIZE;
		else if ( tsStyle == TEXT("noparentalign") ) 
			*Styles |= CCS_NOPARENTALIGN ;
		else if ( tsStyle == TEXT("noauto") )
			*Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
	}
	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxReBar::getImageList( ) const {

  REBARINFO ri;
  ZeroMemory(&ri, sizeof(REBARINFO));
  ri.cbSize = sizeof(REBARINFO);
  ri.fMask = RBIM_IMAGELIST;
  this->getBarInfo( &ri );
  return ri.himl;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxReBar::setImageList( HIMAGELIST himl ) {

  REBARINFO ri;
  ZeroMemory( &ri, sizeof(REBARINFO) );
  ri.cbSize = sizeof(REBARINFO);
  ri.himl = himl;
  ri.fMask = RBIM_IMAGELIST;
  this->setBarInfo( &ri );
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxReBar::createImageList( ) {
  return ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxReBar::parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const
{
	const UINT numtok = input.numtok( );

	const TString prop(input.getfirsttok( 3 ));

	if ( prop == TEXT("num") ) {

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getBandCount( ) );
		return;
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("text") && numtok > 3 ) {

		const int nIndex = input.getnexttok( ).to_int( ) - 1;	// tok 4

		if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {

			REBARBANDINFO rbBand;
			ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
			rbBand.cbSize = sizeof( REBARBANDINFO );
			rbBand.fMask = RBBIM_TEXT;
			rbBand.cch = MIRC_BUFFER_SIZE_CCH;
			rbBand.lpText = szReturnValue;
			if ( this->getBandInfo( nIndex, &rbBand ) != 0 )
				return;
		}
	}
	else if ( prop == TEXT("childid") && numtok > 3 ) {

		const int nItem = input.getnexttok( ).to_int( ) - 1; // tok 4
		const DcxControl * c = this->getControl(nItem);
		if ( c != NULL )
			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), c->getUserID( ) );

		return;
	}
	// $xdid([NAME], [ID], [N]).[PROP]
	else if (prop == TEXT("markeditem")) {
		REBARBANDINFO rbi;
		LPDCXRBBAND pdcxrbb;
		int n = input.getnexttok( ).to_int();	// tok 4

		if (n < 1 || n > this->getBandCount()) {
			this->showErrorEx(TEXT("markeditem"), NULL, TEXT("Invalid band index: %d"), n);
			return;
		}

		// Zero based index
		n--;

		ZeroMemory(&rbi, sizeof(REBARBANDINFO));
		rbi.cbSize = sizeof(REBARBANDINFO);
		rbi.fMask = RBBIM_LPARAM;
		this->getBandInfo(n, &rbi);
		pdcxrbb = (LPDCXRBBAND) rbi.lParam;

		lstrcpyn(szReturnValue, pdcxrbb->tsMarkText.to_chr(), MIRC_BUFFER_SIZE_CCH);
		return;
	}
	else if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
		return;

	szReturnValue[0] = 0;
}
/*!
 * \brief blah
 *
 * \param input [NAME] [SWITCH] [ID] (OPTIONS)
 */

void DcxReBar::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok(3));
	const UINT numtok = input.numtok();

	// xdid -a [NAME] [ID] [SWITCH] [N] [+FLAGS] [CX] [CY] [WIDTH] [ICON] [COLOR] [Item Text][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)[TAB]Tooltip
	if ( flags[TEXT('a')] && numtok > 9 ) {

		REBARBANDINFO rbBand;
		ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
#ifdef DCX_USE_WINSDK
		if (Dcx::VistaModule.isUseable()) // NB: when rbBand.cbSize is set to the Vista size on XP the insertband will FAIL!! fucking MS!
			rbBand.cbSize = sizeof( REBARBANDINFO );
		else
			rbBand.cbSize = REBARBANDINFO_V6_SIZE;
#else
		rbBand.cbSize = sizeof( REBARBANDINFO );
#endif
		rbBand.fMask = RBBIM_STYLE | RBBIM_LPARAM;

		const TString data(input.getfirsttok(1, TSTAB).trim());
		TString control_data;
		TString tooltip;
		const UINT nToks = input.numtok(TSTAB);

		if (nToks > 1) {
			control_data = input.getnexttok( TSTAB).trim();		// tok 2

			if (nToks > 2)
				tooltip = input.getnexttok( TSTAB).trim();		// tok 3
		}
		int nIndex = data.getfirsttok( 4 ).to_int( ) - 1;
		rbBand.fStyle = this->parseBandStyleFlags( data.getnexttok( ) );	// tok 5
		const int cx = data.getnexttok( ).to_int( );						// tok 6
		const int cy = data.getnexttok( ).to_int( );						// tok 7
		const int width = data.getnexttok( ).to_int( );						// tok 8
		const int nIcon = data.getnexttok( ).to_int( ) - 1;					// tok 9
		const COLORREF clrText = (COLORREF)data.getnexttok( ).to_num( );	// tok 10

		if (nIndex < -1)
			nIndex = -1;

		rbBand.cxMinChild = cx;
		rbBand.cyMinChild = cy;
		rbBand.cx = width;
		rbBand.cyIntegral = 1;
		rbBand.cyChild = cy;

		TString itemtext;
		if ( data.numtok( ) > 10 ) {
			itemtext = data.gettok(11, -1).trim();
			rbBand.fMask |= RBBIM_TEXT;
			rbBand.lpText = itemtext.to_chr( );
			//rbBand.cch = itemtext.len();
		}

		// Tooltip Handling
		LPDCXRBBAND lpdcxrbb = new DCXRBBAND;

		if (lpdcxrbb == NULL) {
			this->showError(NULL, TEXT("-a"), TEXT("Unable to Allocate Memory"));
			return;
		}

		if ( rbBand.fStyle & RBBS_UNDERLINE )
			lpdcxrbb->bUline = TRUE;
		else
			lpdcxrbb->bUline = FALSE;

		if ( rbBand.fStyle & RBBS_BOLD )
			lpdcxrbb->bBold = TRUE;
		else
			lpdcxrbb->bBold = FALSE;

		if ( rbBand.fStyle & RBBS_COLOR )
			lpdcxrbb->clrText = clrText;
		else
			lpdcxrbb->clrText = CLR_INVALID;

		if ( nIcon > -1 ) {
			rbBand.iImage = nIcon;
			rbBand.fMask |= RBBIM_IMAGE;
		}

		rbBand.lParam = (LPARAM) lpdcxrbb;

		DcxControl * p_Control = NULL;
		if ( control_data.numtok( ) > 5 ) {
			UINT ID = mIRC_ID_OFFSET + control_data.gettok( 1 ).to_int( );

			if ( ID > mIRC_ID_OFFSET - 1 && 
				!IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
				this->m_pParentDialog->getControlByID( ID ) == NULL ) 
			{
				try {
					p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,control_data,2,
						CTLF_ALLOW_TRACKBAR |
						CTLF_ALLOW_PBAR |
						CTLF_ALLOW_COMBOEX |
						CTLF_ALLOW_TOOLBAR |
						CTLF_ALLOW_STATUSBAR |
						CTLF_ALLOW_TREEVIEW |
						CTLF_ALLOW_LISTVIEW |
						CTLF_ALLOW_COLORCOMBO |
						CTLF_ALLOW_BUTTON |
						CTLF_ALLOW_RICHEDIT |
						CTLF_ALLOW_DIVIDER |
						CTLF_ALLOW_PANEL |
						CTLF_ALLOW_TAB
						,this->m_Hwnd);
					if ( p_Control != NULL ) {
						if ((p_Control->getType() == TEXT("statusbar")) || (p_Control->getType() == TEXT("toolbar")))
							p_Control->addStyle( CCS_NOPARENTALIGN | CCS_NORESIZE );
						this->m_pParentDialog->addControl( p_Control );
						rbBand.fMask |= RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_ID;
						rbBand.hwndChild = p_Control->getHwnd( );
						//rbBand.cxMinChild = cx;
						//rbBand.cyMinChild = cy;
						//rbBand.cx = width;
						//rbBand.cyIntegral = 1;
						//rbBand.cyChild = cy;
						rbBand.wID = ID;
					}
				}
				catch ( TCHAR *err ) {
					this->showErrorEx(NULL, TEXT("-a"), TEXT("Unable To Create Control: %d (%s)"), ID - mIRC_ID_OFFSET, err );
					delete lpdcxrbb;
					return;
				}
			}
			else {
				this->showErrorEx(NULL, TEXT("-a"), TEXT("Control with ID \"%d\" already exists"), ID - mIRC_ID_OFFSET );
				delete lpdcxrbb;
				return;
			}
		}

		//LRESULT lRes;
		//rbBand.cbSize = sizeof(REBARBANDINFO);
		//rbBand.fMask = RBBIM_LPARAM|RBBIM_TEXT;
		//rbBand.cch = itemtext.len();
		//rbBand.lpText = itemtext.to_chr();
		////this->insertBand( nIndex, &rbBand );
		//lRes = SendMessage(this->m_Hwnd, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
		//mIRCDebug(TEXT("res: %ld"), lRes);
		if (this->insertBand( nIndex, &rbBand ) == 0L) { // 0L means failed.
			this->showError(NULL, TEXT("-a"), TEXT("Unable To Add Band"));
			if (p_Control != NULL) {
				this->m_pParentDialog->deleteControl(p_Control);
				DestroyWindow(rbBand.hwndChild);
			}
			delete lpdcxrbb;
		}
	}
	// xdid -A [NAME] [ID] [SWITCH] [N] (TEXT)
	else if (flags[TEXT('A')] && numtok > 3) {
		REBARBANDINFO rbi;
		LPDCXRBBAND pdcxrbb;
		int n = input.getnexttok( ).to_int();	// tok 4

		if (n < 1 || n > this->getBandCount()) {
			this->showErrorEx(NULL, TEXT("-A"), TEXT("Invalid band index: %d"), n);
			return;
		}

		// Zero based index
		n--;

		ZeroMemory(&rbi, sizeof(REBARBANDINFO));
		rbi.cbSize = sizeof(REBARBANDINFO);
		rbi.fMask = RBBIM_LPARAM;
		this->getBandInfo(n, &rbi);
		pdcxrbb = (LPDCXRBBAND) rbi.lParam;
		pdcxrbb->tsMarkText = (numtok > 4 ? input.gettok(5, -1) : TEXT(""));
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('d')] && numtok > 3 ) {

		const int nIndex = input.getnexttok( ).to_int( ) - 1;	// tok 4

		if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {

			this->deleteBand( nIndex );
		}
	}
	// xdid -i [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('i')] && numtok > 3 ) {

		const int nIndex = input.getnexttok( ).to_int( ) - 1;	// tok 4

		if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {

			this->showBand( nIndex, FALSE );
		}
	}
	// xdid -j [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('j')] && numtok > 3 ) {

		const int nIndex = input.getnexttok( ).to_int( ) - 1;	// tok 4

		if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {

			this->showBand( nIndex, TRUE );
		}
	}
	// xdid -k [NAME] [ID] [SWITCH] [N] [ICON]
	else if ( flags[TEXT('k')] && numtok > 4 ) {

		REBARBANDINFO rbBand;
		ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
		rbBand.cbSize = sizeof( REBARBANDINFO );
		rbBand.fMask = RBBIM_IMAGE;

		const int nIndex = input.getnexttok( ).to_int( ) - 1;	// tok 4
		const int nIcon = input.getnexttok( ).to_int( ) - 1;	// tok 5
		if ( nIndex > -1 && nIndex < this->getBandCount( ) && nIcon > -2 ) {

			rbBand.iImage = nIcon;
			this->setBandInfo( nIndex, &rbBand );
		}
	}
	// xdid -l -> [NAME] [ID] -l [N|ALL]
	else if ( flags[TEXT('l')] && numtok > 3 ) {

		REBARBANDINFO rbBand;
		ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
		rbBand.cbSize = sizeof( REBARBANDINFO );
		rbBand.fMask = RBBIM_STYLE;

		const int nItems = this->getBandCount( );
		const TString tsItem(input.getnexttok( ));	// tok 4

		if ( tsItem == TEXT("all") ) {

			for (int i = 0; i < nItems; i++ )
			{
				if ( this->getBandInfo( i, &rbBand ) != 0 ) {

					rbBand.fStyle |= RBBS_NOGRIPPER;
					this->setBandInfo( i, &rbBand );
				}
			}
		}
		else {

			const int nIndex = tsItem.to_int( ) - 1;

			if ( nIndex > -1 && nIndex < nItems && this->getBandInfo( nIndex, &rbBand ) != 0 ) {

				rbBand.fStyle |= RBBS_NOGRIPPER;
				this->setBandInfo( nIndex, &rbBand );
			}
		}
	}
	// xdid -m [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('m')] && numtok > 3 ) {

		const int nIndex = input.getnexttok( ).to_int( ) - 1;	// tok 4

		if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {

			this->maxBand( nIndex, FALSE );
		}
	}
	// xdid -n [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('n')] && numtok > 3 ) {

		const int nIndex = input.getnexttok( ).to_int( ) - 1;	// tok 4

		if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {

			this->minBand( nIndex, FALSE );
		}
	}
	// xdid -q [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('q')] && numtok > 3 ) {

		const int nRows = input.getnexttok( ).to_int( );	// tok 4

		if ( nRows > -1 ) {

			this->m_iRowLimit = nRows;
		}
	}
	// xdid -t [NAME] [ID] [SWITCH] [N] [TEXT]
	else if ( flags[TEXT('t')] && numtok > 3 ) {

		REBARBANDINFO rbBand;
		ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
		rbBand.cbSize = sizeof( REBARBANDINFO );
		rbBand.fMask = RBBIM_TEXT;

		const int nIndex = input.getnexttok( ).to_int( ) - 1;	// tok 4
		if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {

			TString itemtext;
			if ( numtok > 4 )
				itemtext = input.gettok(5, -1).trim();

			rbBand.lpText = itemtext.to_chr( );
			this->setBandInfo( nIndex, &rbBand );
		}
	}
	// xdid -u [NAME] [ID] [SWITCH] [N|ALL]
	else if ( flags[TEXT('u')] && numtok > 3 ) {

		REBARBANDINFO rbBand;
		ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
		rbBand.cbSize = sizeof( REBARBANDINFO );
		rbBand.fMask = RBBIM_STYLE;

		const int nItems = this->getBandCount( );
		const TString tsItem(input.getnexttok( ));	// tok 4

		if ( tsItem == TEXT("all") ) {

			for (int i = 0; i < nItems; i++ )
			{
				if ( this->getBandInfo( i, &rbBand ) != 0 ) {

					rbBand.fStyle &= ~RBBS_NOGRIPPER;
					this->setBandInfo( i, &rbBand );
				}
			}
		}
		else {

			const int nIndex = tsItem.to_int( ) - 1;

			if ( nIndex > -1 && nIndex < nItems && this->getBandInfo( nIndex, &rbBand ) != 0 ) {

				rbBand.fStyle &= ~RBBS_NOGRIPPER;
				this->setBandInfo( nIndex, &rbBand );
			}
		}
	}
	// xdid -v [NAME] [ID] [SWITCH] [NFrom] [NTo]
	else if ( flags[TEXT('v')] && numtok > 4 ) {

		const int nIndexFrom = input.getnexttok( ).to_int( ) - 1;	// tok 4
		const int nIndexTo = input.getnexttok( ).to_int( ) - 1;		// tok 4
		const int nItems = this->getBandCount( );

		if ( nIndexFrom > -1 && nIndexFrom < nItems && nIndexTo > -1 && nIndexTo < nItems ) {

			this->moveBand( nIndexFrom, nIndexTo );
		}
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')] && numtok > 5) {
		const TString flag(input.getnexttok( ));		// tok 4
		const int index = input.getnexttok( ).to_int();	// tok 5
		TString filename(input.gettok( 6, -1 ));

		HIMAGELIST himl = this->getImageList();

		if (himl == NULL) {
			himl = this->createImageList();

			if (himl != NULL)
				this->setImageList(himl);
		}

		if (himl != NULL) {
			HICON icon = dcxLoadIcon(index, filename, false, flag);

			if (icon != NULL) {
				ImageList_AddIcon(himl, icon);
				DestroyIcon(icon);
			}
		}
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if ( flags[TEXT('y')] ) {

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

void DcxReBar::resetContents( ) {

	int nItems = this->getBandCount( );

	while ( nItems-- )
		this->deleteBand( nItems );
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxReBar::parseBandStyleFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if ( xflags[TEXT('b')] )
		iFlags |= RBBS_BOLD;
	if ( xflags[TEXT('c')] )
		iFlags |= RBBS_COLOR;
	if ( xflags[TEXT('e')] )
		iFlags |= RBBS_CHILDEDGE;
	if ( xflags[TEXT('f')] )
		iFlags |= RBBS_FIXEDSIZE;
	if ( xflags[TEXT('g')] )
		iFlags |= RBBS_GRIPPERALWAYS;
	if ( xflags[TEXT('h')] )
		iFlags |= RBBS_HIDDEN;
	if ( xflags[TEXT('n')] )
		iFlags |= RBBS_NOGRIPPER;
	if ( xflags[TEXT('o')] )
		iFlags |= RBBS_USECHEVRON;
	if ( xflags[TEXT('u')] )
		iFlags |= RBBS_UNDERLINE;
	if ( xflags[TEXT('v')] )
		iFlags |= RBBS_VARIABLEHEIGHT;
	if ( xflags[TEXT('w')] )
		iFlags |= RBBS_BREAK;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::insertBand(const int uIndex, LPREBARBANDINFO lprbbi ) {
  return SendMessage(this->m_Hwnd, RB_INSERTBAND, (WPARAM) uIndex, (LPARAM) lprbbi ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::deleteBand( const UINT uIndex ) {
  return SendMessage( this->m_Hwnd, RB_DELETEBAND, (WPARAM) uIndex, (LPARAM) 0 ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getBandInfo( const UINT uBand, LPREBARBANDINFO lprbbi ) const {
  return SendMessage( this->m_Hwnd, RB_GETBANDINFO, (WPARAM) uBand, (LPARAM) lprbbi ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setBandInfo( const UINT uBand, LPREBARBANDINFO lprbbi ) {
  return SendMessage( this->m_Hwnd, RB_SETBANDINFO, (WPARAM) uBand, (LPARAM) lprbbi ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setBarInfo( LPREBARINFO lprbi ) {
  return SendMessage( this->m_Hwnd, RB_SETBARINFO, (WPARAM) 0, (LPARAM) lprbi ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getBarInfo( LPREBARINFO lprbi ) const {
  return SendMessage( this->m_Hwnd, RB_GETBARINFO, (WPARAM) 0, (LPARAM) lprbi ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getRowCount( ) const {
  return SendMessage( this->m_Hwnd, RB_GETROWCOUNT, (WPARAM) 0, (LPARAM) 0 ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::hitTest( LPRBHITTESTINFO lprbht ) {
  return SendMessage( this->m_Hwnd, RB_HITTEST, (WPARAM) 0, (LPARAM) lprbht ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getToolTips( ) const {
  return SendMessage( this->m_Hwnd, RB_GETTOOLTIPS, (WPARAM) 0, (LPARAM) 0 ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setToolTips( const HWND hwndToolTip ) {
  return SendMessage( this->m_Hwnd, RB_SETTOOLTIPS, (WPARAM) hwndToolTip, (LPARAM) 0 ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getIDToIndex( const UINT uBandID ) const {
  return SendMessage( this->m_Hwnd, RB_IDTOINDEX, (WPARAM) uBandID, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getBandCount( ) const {
  return SendMessage( this->m_Hwnd, RB_GETBANDCOUNT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setReDraw( const BOOL uState ) {
  return SendMessage( this->m_Hwnd, WM_SETREDRAW, (WPARAM) uState, (LPARAM) uState );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::showBand( const UINT uBand, const BOOL fShow ) {
  return SendMessage( this->m_Hwnd, RB_SHOWBAND, (WPARAM) uBand, (LPARAM) fShow );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::moveBand( const UINT iFrom, const UINT iTo ) {
  return SendMessage( this->m_Hwnd, RB_MOVEBAND, (WPARAM) iFrom, (LPARAM) iTo );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::maxBand( const UINT uBand, const BOOL fIdeal ) {
  return SendMessage( this->m_Hwnd, RB_MAXIMIZEBAND, (WPARAM) uBand, (LPARAM) fIdeal );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::minBand( const UINT uBand, const BOOL fIdeal ) {
  return SendMessage( this->m_Hwnd, RB_MINIMIZEBAND, (WPARAM) uBand, (LPARAM) fIdeal );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxReBar::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {

	case WM_NOTIFY:
		{
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			switch( hdr->code ) {
			case NM_CUSTOMDRAW:
				{
					LPNMCUSTOMDRAW lpncd = (LPNMCUSTOMDRAW) lParam;

					bParsed = TRUE;

					switch( lpncd->dwDrawStage ) {
					case CDDS_PREPAINT:
						return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);
					case CDDS_ITEMPREPAINT:
						{
							LPDCXRBBAND lpdcxrbb = (LPDCXRBBAND) lpncd->lItemlParam;

							if ( lpdcxrbb == NULL )
								return CDRF_DODEFAULT;

							if ( lpdcxrbb->clrText != -1 )
								SetTextColor( lpncd->hdc, lpdcxrbb->clrText );
						}
						return (CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT);
					case CDDS_ITEMPOSTPAINT:
						return CDRF_DODEFAULT;
					default:
						return CDRF_DODEFAULT;
					}
				} // NM_CUSTOMDRAW
				break;

			case RBN_HEIGHTCHANGE:
				{
					bParsed = TRUE;
					RECT rc;
					GetWindowRect( this->m_Hwnd, &rc );
					const UINT width = rc.right - rc.left;
					const UINT height = rc.bottom - rc.top;

					if ( this->m_iWidth != width || this->m_iHeight != height ) {

						this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("change"), this->getUserID( ),
							width, height );

						this->m_iWidth = width;
						this->m_iHeight = height;
					}
				}
				break;

			case RBN_ENDDRAG:
				{
					bParsed = TRUE;
					this->redrawWindow( );
				}
				break;

			case RBN_DELETINGBAND:
				{
					bParsed = TRUE;

					LPNMREBAR lpnmrb = (LPNMREBAR) lParam;

					REBARBANDINFO rbBand;
					ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
					rbBand.cbSize = sizeof( REBARBANDINFO );
					rbBand.fMask = RBBIM_CHILD;

					if ( this->getBandInfo( lpnmrb->uBand, &rbBand ) != 0 ) {

						if ( IsWindow( rbBand.hwndChild ) ) {

							DcxControl * p_delControl = this->m_pParentDialog->getControlByHWND( rbBand.hwndChild );
							this->m_pParentDialog->deleteControl( p_delControl );
						}

						LPDCXRBBAND lpdcxrbb = (LPDCXRBBAND) lpnmrb->lParam;
						if ( lpdcxrbb != NULL )
							delete lpdcxrbb;
					}
				}
				break;
			} // switch
		}
		break;
	}

	return 0L;
}

LRESULT DcxReBar::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
	switch( uMsg ) {

	case WM_NOTIFY:
		{
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			if (IsWindow(hdr->hwndFrom)) {
				DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,TEXT("dcx_cthis"));
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
				DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,TEXT("dcx_cthis"));
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
				DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
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
				DcxControl *c_this = (DcxControl *) GetProp(cHwnd,TEXT("dcx_cthis"));
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
				DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
				if (c_this != NULL) {
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
		}
		break;

	case WM_LBUTTONUP:
		{
			RBHITTESTINFO rbhi;
			GetCursorPos( &rbhi.pt );
			MapWindowPoints(NULL, this->m_Hwnd, &rbhi.pt, 1 );
			const int band = this->hitTest( &rbhi );

			if ( ( rbhi.flags & RBHT_GRABBER || rbhi.flags & RBHT_CAPTION ) && band != -1 ) {

				this->m_iClickedBand = band;

				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("sclick"), this->getUserID( ), band + 1 );
			}
		}
		break;

	case WM_CONTEXTMENU:
		{
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
				RBHITTESTINFO rbhi;
				GetCursorPos( &rbhi.pt );
				MapWindowPoints(NULL, this->m_Hwnd, &rbhi.pt, 1 );
				const int band = this->hitTest( &rbhi );

				if ( band != -1 )
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("rclick"), this->getUserID( ), band + 1 );
			}
		}
		break;

	case WM_SIZE:
		{
			InvalidateRect( this->m_Hwnd, NULL, TRUE );
		}
		break;

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
