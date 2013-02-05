/*!
 * \file dcxcolorcombo.cpp
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
#include "Classes/dcxcolorcombo.h"
#include "Classes/dcxtrackbar.h"
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

DcxColorCombo::DcxColorCombo( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles,
		DCX_COLORCOMBOCLASS,
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
		Dcx::UXModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxColorCombo::~DcxColorCombo( ) {

	this->resetContent( );
	this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxColorCombo::parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme) {
	*Styles |= CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED;

	this->parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxColorCombo::parseInfoRequest( TString & input, char * szReturnValue ) {

	const UINT numtok = input.numtok( );

	const TString prop(input.gettok( 3 ));

	// [NAME] [ID] [PROP]
	if ( prop == "num" ) {

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, "%d", this->getCount( ) );
		return;
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == "color" && numtok > 3 ) {

		const int nItem = input.gettok( 4 ).to_int( ) - 1;

		if ( nItem > -1 && nItem < this->getCount( ) ) {

			LPDCXCCOMBOITEM lpdcxcci = (LPDCXCCOMBOITEM) this->getItemData( nItem );

			if ( lpdcxcci != NULL ) {

				wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, "%d", lpdcxcci->clrItem );
				return;
			}
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == "sel" ) {

		const int nItem = (int)this->getCurSel( );

		if ( nItem != CB_ERR ) {
			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, "%d", nItem + 1 );
			return;
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
void DcxColorCombo::parseCommandRequest(TString &input) {
	const XSwitchFlags flags(input.gettok(3));
	const UINT numtok = input.numtok( );

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags['r']) {
		this->resetContent();
	}

	// xdid -a [NAME] [ID] [SWITCH] [N] [RGB]
	if (flags['a'] && numtok > 4) {
		int nItem = input.gettok(4).to_int() -1;
		const COLORREF clrItem = (COLORREF)input.gettok( 5 ).to_num();

		if (nItem >= this->getCount())
			nItem = -1;

		if (nItem > -2) {
			LPDCXCCOMBOITEM lpdcxcci = new DCXCCOMBOITEM;

			if (lpdcxcci == NULL) {
				this->showError(NULL, TEXT("-a"), TEXT("Unable to Allocate Memory"));
				return;
			}

			lpdcxcci->clrItem = clrItem;
			//lpmycci->itemtext = "";
			this->insertItem(nItem, (LPARAM) lpdcxcci);
		}
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags['c'] && numtok > 3) {
		const int nItem = input.gettok(4).to_int() -1;

		if ((nItem > -2) && (nItem < this->getCount()))
			this->setCurSel(nItem);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags['d'] && numtok > 3) {
		const int nItem = (int)input.gettok( 4 ).to_num() -1;

		if (nItem > -1 && nItem < this->getCount()) {
			this->deleteItem(nItem);
		}
	}
	// xdid -m [NAME] [ID] [SWITCH]
	else if (flags['m']) {
		this->setmIRCPalette();
	}
	// xdid -o [NAME] [ID] [SWITCH] [N] [RGB]
	else if (flags['o'] && numtok > 4) {
		const int nItem = input.gettok( 4 ).to_int() -1;
		const COLORREF clrItem = (COLORREF)input.gettok( 5 ).to_num();

		if (nItem > -1 && nItem < this->getCount()) {
			LPDCXCCOMBOITEM lpdcxcci = (LPDCXCCOMBOITEM) this->getItemData(nItem);

			if (lpdcxcci != NULL)
				lpdcxcci->clrItem = clrItem;
		}
	}
	// This is to avoid invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags['r']) {
		//this->resetContent();
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxColorCombo::setmIRCPalette( ) {

	static const char com[] = "$color(0) $color(1) $color(2) $color(3) $color(4) $color(5) $color(6) $color(7) $color(8) $color(9) $color(10) $color(11) $color(12) $color(13) $color(14) $color(15)";
	TString cols;
	Dcx::mIRC.tsEval( cols, com );

	LPDCXCCOMBOITEM lpdcxcci;

	const UINT len = cols.numtok( );

	for (UINT i = 1; i <= len; i++ )
	{
		lpdcxcci = new DCXCCOMBOITEM;

		if (lpdcxcci != NULL) {
			lpdcxcci->clrItem = (COLORREF) cols.gettok( i ).to_num( );
			//lpmycci->itemtext = "";
			this->insertItem( -1, (LPARAM) lpdcxcci );
		}
	}
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::insertItem(  const int nPos, const LPARAM lParam ) {
  return SendMessage( this->m_Hwnd, CB_INSERTSTRING, (WPARAM) nPos, lParam );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::getCount( ) const {
  return SendMessage( this->m_Hwnd, CB_GETCOUNT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::setCurSel( const int nPos ) {
  return SendMessage( this->m_Hwnd, CB_SETCURSEL, (WPARAM) nPos, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::getItemData( const int nItem ) const {
  return SendMessage( this->m_Hwnd, CB_GETITEMDATA, (WPARAM) nItem, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::getCurSel( ) const {
  return SendMessage( this->m_Hwnd, CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::deleteItem( const int nItem ) {
  return SendMessage( this->m_Hwnd, CB_DELETESTRING, (WPARAM) nItem, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxColorCombo::resetContent( ) {
  return SendMessage( this->m_Hwnd, CB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0 );
}


/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxColorCombo::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {
	case WM_COMMAND:
		{
			if ( HIWORD( wParam ) == CBN_SELENDOK)
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->execAliasEx("%s,%d,%d", "sclick", this->getUserID( ), this->getCurSel( ) + 1 );
				bParsed = TRUE;
				return 0L;
			}
		}
		break;
	case WM_DELETEITEM:
		{
			PDELETEITEMSTRUCT delis = (PDELETEITEMSTRUCT) lParam;
			LPDCXCCOMBOITEM lpdcxcci = (LPDCXCCOMBOITEM) delis->itemData;

			if ( lpdcxcci != NULL )
				delete lpdcxcci;

			bParsed = TRUE;
			return TRUE;
		}
		break;

	case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;

			if ( lpdis != NULL && lpdis->itemID != -1 ) {

				LPDCXCCOMBOITEM lpdcxcci = (LPDCXCCOMBOITEM) lpdis->itemData;

				if ( lpdcxcci != NULL ) {

					/*
					if ( lpdis->itemState & ODS_SELECTED ) {
					hPen = CreatePen( PS_SOLID, 2, RGB(0,0,0) );
					mIRCError( "ODS_SELECTED" );
					}
					else
					*/
					HPEN hPen = CreatePen( PS_SOLID, 1, RGB(0,0,0) );

					const HPEN oldPen = SelectPen( lpdis->hDC, hPen );

					RECT rcItem = lpdis->rcItem;

					// draw selection indicator
					if (lpdis->itemState & ODS_COMBOBOXEDIT)
						SetBkColor(lpdis->hDC, GetSysColor(COLOR_WINDOW));
					else if (lpdis->itemState & ODS_SELECTED)
						SetBkColor(lpdis->hDC, GetSysColor(COLOR_MENUHILIGHT));
					else
						SetBkColor(lpdis->hDC, GetSysColor(COLOR_WINDOW));
					ExtTextOut( lpdis->hDC, rcItem.left, rcItem.top, ETO_CLIPPED | ETO_OPAQUE, &rcItem, "", NULL, NULL );

					InflateRect( &rcItem, -4, -2 );

					SetBkColor( lpdis->hDC, lpdcxcci->clrItem );

					ExtTextOut( lpdis->hDC, rcItem.left, rcItem.top, ETO_CLIPPED | ETO_OPAQUE, &rcItem, "", NULL, NULL );

					MoveToEx( lpdis->hDC, rcItem.left, rcItem.top, NULL );
					LineTo( lpdis->hDC, rcItem.right, rcItem.top );

					MoveToEx( lpdis->hDC, rcItem.right, rcItem.top, NULL );
					LineTo( lpdis->hDC, rcItem.right, rcItem.bottom );

					MoveToEx( lpdis->hDC, rcItem.right, rcItem.bottom, NULL );
					LineTo( lpdis->hDC, rcItem.left, rcItem.bottom );

					MoveToEx( lpdis->hDC, rcItem.left, rcItem.bottom, NULL );
					LineTo( lpdis->hDC, rcItem.left, rcItem.top );

					SelectPen( lpdis->hDC, oldPen );
					DeletePen( hPen );
				}
			}
			bParsed = TRUE;
			return TRUE;
		}
		break;
	case WM_MEASUREITEM:
		{
			LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;

			if (lpmis != NULL)
				lpmis->itemHeight = 16; 
			bParsed = TRUE;
			return TRUE;
		}
		break;
	}
	return 0L;
}

LRESULT DcxColorCombo::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

	case WM_LBUTTONUP:
		{
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				this->execAliasEx("%s,%d", "lbup", this->getUserID( ) );
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
