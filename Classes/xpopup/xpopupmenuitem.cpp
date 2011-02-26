/*!
 * \file xpopupmenuitem.cpp
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
#include "Classes/xpopup/xpopupmenuitem.h"
#include "Classes/xpopup/xpopupmenu.h"
#include "Dcx.h"

/*!
 * \brief Constructor
 *
 * blah
 */

XPopupMenuItem::XPopupMenuItem( XPopupMenu * Parent, const BOOL bSep, ULONG_PTR dwDataBackup )
: m_pXParentMenu( Parent ), m_bSep( bSep ), m_nIcon(-1), m_bSubMenu(FALSE), m_dwItemDataBackup(dwDataBackup) {
}

/*!
 * \brief Constructor
 *
 * blah
 */

XPopupMenuItem::XPopupMenuItem( XPopupMenu * Parent, const TString &tsItemText, const int nIcon, const BOOL bSubMenu, ULONG_PTR dwDataBackup ) 
: m_pXParentMenu( Parent ), m_tsItemText( tsItemText ), m_nIcon( nIcon ), m_bSubMenu( bSubMenu ), m_bSep( FALSE ), m_dwItemDataBackup(dwDataBackup)
{
	this->m_tsItemText.trim();
}

/*!
 * \brief Destructor
 *
 * blah
 */

XPopupMenuItem::~XPopupMenuItem( ) {

}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::setSubMenu( const BOOL bSubMenu ) {

	this->m_bSubMenu = bSubMenu;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::setItemText( const TString & tsItemText ) {

	if ( this->m_bSep == FALSE )
		this->m_tsItemText = tsItemText;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::setItemIcon( const int nIcon ) {

	if ( this->m_bSep == FALSE )
		this->m_nIcon = nIcon;
}

/*!
 * \brief blah
 *
 * blah
 */

const TString * XPopupMenuItem::getItemText( ) const {

	return &this->m_tsItemText;
}

/*!
 * \brief blah
 *
 * blah
 */

int XPopupMenuItem::getItemIcon(  ) const {

	return this->m_nIcon;
}

/*!
 * \brief blah
 *
 * blah
 */

SIZE XPopupMenuItem::getItemSize( const HWND mHwnd ) {

	SIZE size;
	size.cx = 0;
	size.cy = 0;

	if ( this->m_bSep == FALSE ) {

		HDC hdc = GetDC( mHwnd );

		const TString tsType(this->m_pXParentMenu->getName( ));
		if ( (tsType == TEXT("mirc")) || (tsType == TEXT("mircbar")) || (tsType == TEXT("dialog")) ) {
			if ( this->m_tsItemText.numtok( TEXT("\v") ) > 1 ) {
				this->m_nIcon = this->m_tsItemText.gettok( 1, TEXT("\v")).to_int( ) - 1;
				this->m_tsItemText = this->m_tsItemText.gettok(2, TEXT("\v")).trim();
			}

			GetTextExtentPoint32( hdc, this->m_tsItemText.to_chr( ), this->m_tsItemText.len( ), &size );
		}
		else {
			TString tsRes;
			Dcx::mIRC.tsEval( tsRes, this->m_tsItemText.to_chr( ) );
			this->m_tsItemText = tsRes;
			GetTextExtentPoint32( hdc, tsRes.to_chr(), tsRes.len(), &size );
		}

		ReleaseDC( mHwnd, hdc );
	}

	size.cx += XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD;
	if ( this->m_bSep == TRUE )
		size.cy = 3;
	else
		size.cy = XPMI_HEIGHT;

	return size;
}
	
ULONG_PTR XPopupMenuItem::getItemDataBackup() {
	return m_dwItemDataBackup;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItem( const LPDRAWITEMSTRUCT lpdis ) {

	const LPXPMENUCOLORS lpcol = this->m_pXParentMenu->getColors( );
	const UINT iItemStyle = this->m_pXParentMenu->getItemStyle( );

	// playing around with menu transparency
	const BYTE alpha = this->m_pXParentMenu->IsAlpha();

	// If alpha == 255 then menu is fully opaque so no need to change to layered.
	if (alpha < 255) {
		HWND hMenuWnd = WindowFromDC(lpdis->hDC);

		if (IsWindow(hMenuWnd)) {
			DWORD dwStyle = GetWindowExStyle(hMenuWnd);

			if (!(dwStyle & WS_EX_LAYERED)) {
				SetWindowLong(hMenuWnd, GWL_EXSTYLE, dwStyle | WS_EX_LAYERED);
				SetLayeredWindowAttributes(hMenuWnd, 0, (BYTE)alpha, LWA_ALPHA); // 0xCC = 80% Opaque
				//RedrawWindow(hMenuWnd, NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_UPDATENOW|RDW_INVALIDATE);
				// NB: Menus on XP will not show as transparent straight away when a transition effect is used when displaying the menu.
				// This can't be fixed at this time, live with it.
				// NB: Menus on Vista/Win7 also suffer from this.
			}
		}
	}
	// All Items
	this->DrawItemBackground( lpdis, lpcol );
	this->DrawItemBox( lpdis, lpcol );

	const TString tsType(this->m_pXParentMenu->getName( ));
	if (( tsType == TEXT("mircbar") ) || ( tsType == TEXT("dialog"))) {

		if ( this->m_tsItemText.numtok( TEXT("\v") ) > 1 ) {

			this->m_nIcon = this->m_tsItemText.gettok( 1, TEXT("\v")).to_int( ) - 1;
			this->m_tsItemText = this->m_tsItemText.gettok( 2, TEXT("\v") ).trim();
		}
	}

	// Item is selected
	if ( lpdis->itemState & ODS_SELECTED ) {

		if ( lpdis->itemState & ODS_GRAYED ) {

			if ( iItemStyle & XPS_DISABLEDSEL )
				this->DrawItemSelection( lpdis, lpcol, TRUE, this->m_pXParentMenu->IsRounded() );
		}
		else
			this->DrawItemSelection( lpdis, lpcol, FALSE, this->m_pXParentMenu->IsRounded() );
	}

	if ( lpdis->itemState & ODS_CHECKED && this->m_bSep == FALSE )
		this->DrawItemCheckBox( lpdis, lpcol, lpdis->itemState & ODS_GRAYED?TRUE:FALSE );

	// Separator
	if ( this->m_bSep == TRUE )
		this->DrawItemSeparator( lpdis, lpcol );
	// Regular Item
	else {
		this->DrawItemText( lpdis, lpcol, lpdis->itemState & ODS_GRAYED?TRUE:FALSE );

		if ( !( lpdis->itemState & ODS_CHECKED ) || this->m_nIcon > -1 ) {
			this->DrawItemIcon( lpdis, lpcol, iItemStyle, lpdis->itemState & ODS_SELECTED?TRUE:FALSE, 
				lpdis->itemState & ODS_GRAYED?TRUE:FALSE );
		}
		if ( this->m_bSubMenu == TRUE )
			this->DrawItemSubArrow( lpdis, lpcol, lpdis->itemState & ODS_GRAYED?TRUE:FALSE );
	}
}

/*!
 * \brief Draws the background of the actual item where text resides
 *
 * blah
 */

void XPopupMenuItem::DrawItemBackground(const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol) {
	switch (this->m_pXParentMenu->getStyle()) {
		case XPopupMenu::XPMS_ICY:
			this->DrawGradient(lpdis->hDC, &lpdis->rcItem, LightenColor(200, lpcol->m_clrBox), lpcol->m_clrBox, TRUE);
			break;

		case XPopupMenu::XPMS_ICY_REV:
			this->DrawGradient(lpdis->hDC, &lpdis->rcItem, lpcol->m_clrBox, LightenColor(200, lpcol->m_clrBox), TRUE);
			break;

		case XPopupMenu::XPMS_GRADE:
			{
				// For some reason this call to DrawGradient will not fill up the last pixel on the right without this fix.
				//lpdis->rcItem.right++;
				this->DrawGradient(lpdis->hDC, &lpdis->rcItem, lpcol->m_clrBox, LightenColor(200, lpcol->m_clrBox), FALSE);
				//lpdis->rcItem.right--;
			}
			break;

		case XPopupMenu::XPMS_GRADE_REV:
			{
				// For some reason this call to DrawGradient will not fill up the last pixel on the right without this fix.
				//lpdis->rcItem.right++;
				this->DrawGradient(lpdis->hDC, &lpdis->rcItem, LightenColor(200, lpcol->m_clrBox), lpcol->m_clrBox, FALSE);
				//lpdis->rcItem.right--;
			}
			break;

		case XPopupMenu::XPMS_CUSTOM:
			{
				//HBITMAP hBitmap = this->m_pXParentMenu->getBackBitmap();

				//if (hBitmap != NULL) {
				//	HDC hdcbmp = CreateCompatibleDC(lpdis->hDC);
				//	if (hdcbmp != NULL) {
				//		BITMAP bmp;

				//		GetObject(hBitmap, sizeof(BITMAP), &bmp);
				//		HBITMAP hOldBm = SelectBitmap(hdcbmp, hBitmap);
				//		StretchBlt(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right - lpdis->rcItem.left, 
				//			lpdis->rcItem.bottom - lpdis->rcItem.top, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

				//		SelectBitmap(hdcbmp, hOldBm);
				//		DeleteDC(hdcbmp);
				//	}
				//}
				//else {
				//	// when no bitmap is supplied do colour fill as with standard menu styles.
				//	// fixes bug: http://dcx.scriptsdb.org/bug/index.php?do=details&task_id=723
				//	HBRUSH hBrush = CreateSolidBrush(lpcol->m_clrBack);

				//	if (hBrush != NULL) {
				//		FillRect(lpdis->hDC, &lpdis->rcItem, hBrush);
				//		DeleteBrush(hBrush);
				//		// NB: atm we simply silently fail when we can't make a brush. (so drawing errors will occur when create fails)
				//	}
				//}
				HBITMAP hBitmap = this->m_pXParentMenu->getBackBitmap();

				if (hBitmap != NULL) {
					HDC hdcbmp = CreateCompatibleDC(lpdis->hDC);
					if (hdcbmp != NULL) {
						BITMAP bmp;

						GetObject(hBitmap, sizeof(BITMAP), &bmp);
						HBITMAP hOldBm = SelectBitmap(hdcbmp, hBitmap);
						StretchBlt(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right - lpdis->rcItem.left, 
							lpdis->rcItem.bottom - lpdis->rcItem.top, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

						SelectBitmap(hdcbmp, hOldBm);
						DeleteDC(hdcbmp);
						break;
					}
				}
			}
			// fall through when unable to draw bitmap (no bitmap or error)
		case XPopupMenu::XPMS_OFFICEXP:
		case XPopupMenu::XPMS_NORMAL:
		case XPopupMenu::XPMS_OFFICE2003_REV:
		case XPopupMenu::XPMS_OFFICE2003:
		default:
			{
				HBRUSH hBrush = CreateSolidBrush(lpcol->m_clrBack);

				if (hBrush != NULL) {
					FillRect(lpdis->hDC, &lpdis->rcItem, hBrush);
					DeleteBrush(hBrush);
					// NB: atm we simply silently fail when we can't make a brush. (so drawing errors will occur when create fails)
				}
			}
			break;
	}
}

/*!
 * \brief Draws the area on the left side of the menu for checks/radios/icons
 *
 * blah
 */

void XPopupMenuItem::DrawItemBox(const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol) {

	switch (this->m_pXParentMenu->getStyle()) {
		case XPopupMenu::XPMS_OFFICE2003_REV:
		{
			RECT rc;
			SetRect(&rc, XPMI_BOXLPAD, lpdis->rcItem.top, XPMI_BOXLPAD + XPMI_BOXWIDTH, lpdis->rcItem.bottom);
			XPopupMenuItem::DrawGradient(lpdis->hDC, &rc, lpcol->m_clrBox, LightenColor(200, lpcol->m_clrBox));
			break;
		}

		case XPopupMenu::XPMS_OFFICEXP:
		{
			RECT rc;
			SetRect(&rc, XPMI_BOXLPAD, lpdis->rcItem.top, XPMI_BOXLPAD + XPMI_BOXWIDTH, lpdis->rcItem.bottom);
			HBRUSH hBrush = CreateSolidBrush(lpcol->m_clrBox);
			if (hBrush != NULL) {
				FillRect(lpdis->hDC, &rc, hBrush);
				DeleteBrush(hBrush);
			}
			break;
		}

		
		case XPopupMenu::XPMS_VERTICAL:
		case XPopupMenu::XPMS_VERTICAL_REV:
			XPopupMenuItem::DrawVerticalBar(lpdis, lpcol, (this->m_pXParentMenu->getStyle() == XPopupMenu::XPMS_VERTICAL_REV));
			break;

		case XPopupMenu::XPMS_ICY:
		case XPopupMenu::XPMS_ICY_REV:
		case XPopupMenu::XPMS_GRADE:
		case XPopupMenu::XPMS_GRADE_REV:
		case XPopupMenu::XPMS_CUSTOM:
		case XPopupMenu::XPMS_NORMAL:
			break;

		case XPopupMenu::XPMS_OFFICE2003:
		default:
		{
			RECT rc;
			SetRect(&rc, XPMI_BOXLPAD, lpdis->rcItem.top, XPMI_BOXLPAD + XPMI_BOXWIDTH, lpdis->rcItem.bottom);

			XPopupMenuItem::DrawGradient(lpdis->hDC, &rc, LightenColor(200, lpcol->m_clrBox), lpcol->m_clrBox);
			break;
		}
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItemSelection( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const BOOL bDis, const BOOL bRounded ) {

	HPEN hPen = CreatePen( PS_SOLID, 1, lpcol->m_clrSelectionBorder );

	if (hPen == NULL)
		return;

	HPEN hOldPen = SelectPen( lpdis->hDC, hPen );

	HBRUSH hBrush = CreateSolidBrush( bDis?lpcol->m_clrDisabledSelection:lpcol->m_clrSelection );
	if (hBrush != NULL) {
		HBRUSH hOldBrush = SelectBrush( lpdis->hDC, hBrush );

		RECT rc;
		CopyRect( &rc, &lpdis->rcItem );

		if (bRounded)
			RoundRect( lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom, 10, 10 );
		else
			Rectangle( lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom );
		DeleteBrush( SelectBrush( lpdis->hDC, hOldBrush ) );
	}
	DeletePen( SelectPen( lpdis->hDC, hOldPen ) );
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItemCheckBox( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const BOOL bDis ) {

	HBRUSH hBrush = CreateSolidBrush( bDis?lpcol->m_clrDisabledCheckBox:lpcol->m_clrCheckBox );
	HPEN hPenBorder = CreatePen( PS_SOLID, 1, lpcol->m_clrSelectionBorder );
	HPEN hPenText = CreatePen( PS_SOLID, 1, bDis?lpcol->m_clrDisabledText:lpcol->m_clrText );

	if ((hBrush == NULL) || (hPenBorder == NULL) || (hPenText == NULL)) {
		if (hPenText != NULL)
			DeletePen(hPenText);
		if (hPenBorder != NULL)
			DeletePen(hPenBorder);
		if (hBrush != NULL)
			DeleteBrush(hBrush);
		return;
	}

	HBRUSH hOldBrush = SelectBrush( lpdis->hDC, hBrush );
	HPEN hOldPen = SelectPen( lpdis->hDC, hPenBorder );

	RECT rc;
	CopyRect( &rc, &lpdis->rcItem );
	InflateRect( &rc, 0, -1 );
	rc.left += 1;
	rc.right = rc.left + rc.bottom - rc.top;

	Rectangle( lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom );

	SelectPen( lpdis->hDC, hPenText );

	const int x = ( rc.right + rc.left ) / 2 - 3;
	const int y = ( rc.bottom + rc.top ) / 2 - 3;

	MoveToEx( lpdis->hDC, x, y+2, NULL );
	LineTo( lpdis->hDC, x, y+5 );
	MoveToEx( lpdis->hDC, x+1, y+3, NULL );
	LineTo( lpdis->hDC, x+1, y+6 );
	MoveToEx( lpdis->hDC, x+2, y+4, NULL );
	LineTo( lpdis->hDC, x+2, y+7 );
	MoveToEx( lpdis->hDC, x+3, y+3, NULL );
	LineTo( lpdis->hDC, x+3, y+6 );
	MoveToEx( lpdis->hDC, x+4, y+2, NULL );
	LineTo( lpdis->hDC, x+4, y+5 );
	MoveToEx( lpdis->hDC, x+5, y+1, NULL );
	LineTo( lpdis->hDC, x+5, y+4 );
	MoveToEx( lpdis->hDC, x+6, y, NULL );
	LineTo( lpdis->hDC, x+6, y+3 );

	DeletePen( SelectPen( lpdis->hDC, hOldPen ) );
	DeletePen( hPenBorder );
	DeleteBrush( SelectBrush( lpdis->hDC, hOldBrush ) );
}

/*!
 * \brief blah
 *
 * blah
 */
void XPopupMenuItem::DrawItemText( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const BOOL bDis ) {

	COLORREF oldClr = SetTextColor( lpdis->hDC, (bDis?lpcol->m_clrDisabledText:((lpdis->itemState & ODS_SELECTED)?lpcol->m_clrSelectedText:lpcol->m_clrText)) );

	const int oldBkg = SetBkMode( lpdis->hDC, TRANSPARENT );

	RECT rc;
	CopyRect( &rc, &lpdis->rcItem );
	rc.left += XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD;

	TString txt;
	//check if the first char is $chr(12), if so then the text is utf8
	if ( this->m_tsItemText[0] == 12) {
		// remove $chr(12) from text and trim whitespaces
		txt = this->m_tsItemText.right(-1).trim();
	}
	else // not utf8 so copy
		txt = this->m_tsItemText;

	if ( txt.numtok( TSTAB ) > 1 ) {

		TString lefttext(txt.gettok(1, TSTAB).trim());
		TString righttext(txt.gettok(2, TSTAB).trim());

		//DrawTextEx( lpdis->hDC, lefttext.to_chr( ), lefttext.len( ), &rc, 
		//  DT_LEFT | DT_SINGLELINE | DT_VCENTER, NULL );
		mIRC_DrawText( lpdis->hDC, lefttext, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, false);

		if ( righttext.len( ) > 0 ) {

			rc.right -= 15;
			//DrawTextEx( lpdis->hDC, righttext.to_chr( ), righttext.len( ), &rc, 
			//  DT_RIGHT | DT_SINGLELINE | DT_VCENTER, NULL );
			mIRC_DrawText( lpdis->hDC, righttext, &rc, DT_RIGHT | DT_SINGLELINE | DT_VCENTER, false);
		}
	}
	else {
		//DrawTextEx( lpdis->hDC, this->m_tsItemText.to_chr( ), this->m_tsItemText.len( ), &rc, 
		//  DT_LEFT | DT_SINGLELINE | DT_VCENTER, NULL );
		mIRC_DrawText( lpdis->hDC, txt, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, false);
	}
	SetBkMode( lpdis->hDC, oldBkg );
	SetTextColor( lpdis->hDC, oldClr );
}
/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItemIcon( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const UINT iExStyles, const BOOL bSel, const BOOL bDis ) {

	HIMAGELIST himl = this->m_pXParentMenu->getImageList( );

	const int x = ( XPMI_BOXLPAD + XPMI_BOXLPAD + XPMI_BOXWIDTH - XPMI_ICONSIZE ) / 2;
	const int y = ( lpdis->rcItem.top + lpdis->rcItem.bottom - XPMI_ICONSIZE ) / 2;

	if ( himl != NULL && this->m_nIcon > -1 && this->m_nIcon < ImageList_GetImageCount( himl ) ) {

		// Selected Item
		if ( bSel == TRUE ) {

			// Disabled
			if ( bDis == TRUE )
				ImageList_DrawEx( himl, this->m_nIcon, lpdis->hDC, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT | ILD_BLEND50 );
			else {

				if ( iExStyles & XPS_ICON3DSHADOW ) {

					ImageList_DrawEx( himl, this->m_nIcon, lpdis->hDC, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT | ILD_BLEND25 );
					ImageList_DrawEx( himl, this->m_nIcon, lpdis->hDC, x - 1, y - 1, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT );

				}
				else if ( iExStyles & XPS_ICON3D )
					ImageList_DrawEx( himl, this->m_nIcon, lpdis->hDC, x - 1, y - 1, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT );
				else
					ImageList_DrawEx( himl, this->m_nIcon, lpdis->hDC, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT );
			}
		}
		// Not selected
		else {

			if ( bDis == TRUE )
				ImageList_DrawEx( himl, this->m_nIcon, lpdis->hDC, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT | ILD_BLEND50 );
			else
				ImageList_DrawEx( himl, this->m_nIcon, lpdis->hDC, x, y, 0, 0, CLR_NONE, RGB(0,0,0), ILD_TRANSPARENT );
		}
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItemSubArrow( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const BOOL bDis ) {

	HPEN hPen = CreatePen( PS_SOLID, 1, bDis?lpcol->m_clrDisabledText:lpcol->m_clrText );

	if (hPen == NULL)
		return;

	HPEN hOldPen = SelectPen( lpdis->hDC, hPen );

	const int x = lpdis->rcItem.right - 9;
	const int y = ( lpdis->rcItem.bottom + lpdis->rcItem.top ) / 2 - 5;

	MoveToEx( lpdis->hDC, x, y, NULL );
	LineTo( lpdis->hDC, x+1, y );
	MoveToEx( lpdis->hDC, x, y+1, NULL );
	LineTo( lpdis->hDC, x+2, y+1 );
	MoveToEx( lpdis->hDC, x, y+2, NULL );
	LineTo( lpdis->hDC, x+3, y+2 );
	MoveToEx( lpdis->hDC, x, y+3, NULL );
	LineTo( lpdis->hDC, x+4, y+3 );
	MoveToEx( lpdis->hDC, x, y+4, NULL );
	LineTo( lpdis->hDC, x+5, y+4 );
	MoveToEx( lpdis->hDC, x, y+5, NULL );
	LineTo( lpdis->hDC, x+4, y+5 );
	MoveToEx( lpdis->hDC, x, y+6, NULL );
	LineTo( lpdis->hDC, x+3, y+6 );
	MoveToEx( lpdis->hDC, x, y+7, NULL );
	LineTo( lpdis->hDC, x+2, y+7 );
	MoveToEx( lpdis->hDC, x, y+8, NULL );
	LineTo( lpdis->hDC, x+1, y+8 );

	ExcludeClipRect( lpdis->hDC, lpdis->rcItem.right - 10, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom );

	SelectPen( lpdis->hDC, hOldPen );
	DeletePen( hPen );
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItemSeparator( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol ) {

	switch ( this->m_pXParentMenu->getStyle( ) ) {

	case XPopupMenu::XPMS_ICY:
	case XPopupMenu::XPMS_ICY_REV:
	case XPopupMenu::XPMS_NORMAL:
		{

			HPEN hPen = CreatePen( PS_SOLID, 1, lpcol->m_clrSeparatorLine );

			if (hPen != NULL) {
				const int x1 = lpdis->rcItem.left;
				const int x2 = lpdis->rcItem.right;
				const int y = ( lpdis->rcItem.bottom + lpdis->rcItem.top) / 2;

				HPEN oldPen = SelectPen( lpdis->hDC, hPen) ;
				MoveToEx( lpdis->hDC, x1, y, NULL );
				LineTo( lpdis->hDC, x2, y );
				SelectPen( lpdis->hDC, oldPen );
				DeletePen(hPen);
			}
		}
		break;

	case XPopupMenu::XPMS_OFFICEXP:
	case XPopupMenu::XPMS_OFFICE2003_REV:
	case XPopupMenu::XPMS_OFFICE2003:
	default:
		{

			HPEN hPen = CreatePen( PS_SOLID, 1, lpcol->m_clrSeparatorLine );

			if (hPen != NULL) {
				const int x1 = XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD;
				const int x2 = lpdis->rcItem.right;
				const int y = ( lpdis->rcItem.bottom + lpdis->rcItem.top) / 2;

				HPEN oldPen = SelectPen( lpdis->hDC, hPen) ;
				MoveToEx( lpdis->hDC, x1, y, NULL );
				LineTo( lpdis->hDC, x2, y );
				SelectPen( lpdis->hDC, oldPen );
				DeletePen(hPen);
			}
		}
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawGradient( const HDC hdc, const LPRECT lprc, const COLORREF clrStart, const COLORREF clrEnd, const BOOL bHorz ) {

	const BYTE StartRed   = GetRValue( clrStart );
	const BYTE StartGreen = GetGValue( (clrStart & 0xFFFF) );
	const BYTE StartBlue  = GetBValue( clrStart );

	const BYTE EndRed    = GetRValue( clrEnd );
	const BYTE EndGreen  = GetGValue( (clrEnd & 0xFFFF) );
	const BYTE EndBlue   = GetBValue( clrEnd );

	TRIVERTEX        vert[2];
	GRADIENT_RECT    gRect;
	ULONG gMode = GRADIENT_FILL_RECT_H;

	vert [0] .x      = lprc->left;
	vert [0] .y      = lprc->top;
	vert [0] .Red    = (COLOR16)((StartRed & 0xff) << 8);
	vert [0] .Green  = (COLOR16)((StartGreen & 0xff) << 8);
	vert [0] .Blue   = (COLOR16)((StartBlue & 0xff) << 8);
	vert [0] .Alpha  = 0xff00;

	vert [1] .x      = lprc->right;
	vert [1] .y      = lprc->bottom;
	vert [1] .Red    = (COLOR16)((EndRed & 0xff) << 8);
	vert [1] .Green  = (COLOR16)((EndGreen & 0xff) << 8);
	vert [1] .Blue   = (COLOR16)((EndBlue & 0xff) << 8);
	vert [1] .Alpha  = 0xff00;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;

	if (bHorz)
		gMode = GRADIENT_FILL_RECT_V;

	if (!GradientFill(hdc,vert,2,&gRect,1,gMode)) {
		// if GradientFill fails do our own method.
		int n;
		const int dy = 2;

		if ( bHorz == TRUE )
			n = lprc->bottom - lprc->top - dy;
		else
			n = lprc->right - lprc->left - dy;

		RECT rc;
		HBRUSH hBrush;

		for ( int dn = 0; dn <= n; dn += dy ) {

			BYTE Red = (BYTE)( MulDiv( int( EndRed ) - StartRed , dn, n ) + StartRed );
			BYTE Green = (BYTE)( MulDiv( int( EndGreen ) - StartGreen, dn, n ) + StartGreen );
			BYTE Blue = (BYTE)( MulDiv( int( EndBlue ) - StartBlue, dn, n ) + StartBlue );

			if ( bHorz == TRUE )
				SetRect( &rc, lprc->left, lprc->top + dn, lprc->right , lprc->top + dn + dy );
			else
				SetRect( &rc, lprc->left + dn, lprc->top, lprc->left + dn + dy, lprc->bottom );

			hBrush = CreateSolidBrush( RGB( Red, Green, Blue ) );
			if (hBrush != NULL) {
				FillRect( hdc, &rc, hBrush );
				DeleteBrush( hBrush );
			}
		}
	}
}


void XPopupMenuItem::DrawVerticalBar(const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const BOOLEAN bReversed) {
	// Working code: Calculates height of complete menu, and draws gradient to fill. Samples taken off complete gradient when needed.
	///*
	// Get height of all menu items
			//int menuH = 0;
			//int i = 0;
	// GetMenuItemRect() calls here cause submenus to fails to render on first opening for some reason.
			//while (GetMenuItemRect(Dcx::mIRC.getHWND(), (HMENU) lpdis->hwndItem, i, &rc) != FALSE) {
			//	menuH += (rc.bottom - rc.top);
			//	i++;
			//}
			//RECT rcIntersect;
			//RECT rcBar;

			//// Fix to remove black line at the bottom
			//menuH++;
	// using bitmap height like this fixes that.
	BITMAP bm;
	GetObject((HBITMAP)GetCurrentObject(lpdis->hDC, OBJ_BITMAP), sizeof(BITMAP), &bm);
	int menuH = bm.bmHeight;

	RECT rcIntersect;
	RECT rcBar;

	// get the size of the bar on the left
	SetRect(&rcBar, XPMI_BOXLPAD, 0, XPMI_BOXLPAD + XPMI_BOXWIDTH, menuH);

	// get the rect of the box which will draw JUST the box (prevents redraw over items already done)
	SetRect(&rcIntersect, rcBar.left, lpdis->rcItem.top, rcBar.right, lpdis->rcItem.bottom);

	// set up a buffer to draw the whole gradient bar
	HDC *hdcBuffer = CreateHDCBuffer(lpdis->hDC, &rcBar);

	if (hdcBuffer != NULL) {

		// draw the gradient into the buffer
		if (bReversed)
			XPopupMenuItem::DrawGradient(*hdcBuffer, &rcBar, lpcol->m_clrBox, LightenColor(200, lpcol->m_clrBox), TRUE);
		else
			XPopupMenuItem::DrawGradient(*hdcBuffer, &rcBar, LightenColor(200, lpcol->m_clrBox), lpcol->m_clrBox, TRUE);

		// copy the box we want from the whole gradient bar
		BitBlt(lpdis->hDC, rcIntersect.left, rcIntersect.top, rcIntersect.right - rcIntersect.left, rcIntersect.bottom - rcIntersect.top, *hdcBuffer, rcIntersect.left, rcIntersect.top, SRCCOPY);

		DeleteHDCBuffer(hdcBuffer);
	}
	else {
		// buffer create failed, try unbuffered.
		if (bReversed)
			XPopupMenuItem::DrawGradient(lpdis->hDC, &rcIntersect, lpcol->m_clrBox, LightenColor(200, lpcol->m_clrBox), TRUE);
		else
			XPopupMenuItem::DrawGradient(lpdis->hDC, &rcIntersect, LightenColor(200, lpcol->m_clrBox), lpcol->m_clrBox, TRUE);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

COLORREF XPopupMenuItem::LightenColor( const unsigned int iScale, const COLORREF clrColor )
{
	const long R = MulDiv( 255 - GetRValue( clrColor ), iScale, 255 ) + GetRValue( clrColor );
	const long G = MulDiv( 255 - GetGValue( (clrColor & 0xFFFF) ), iScale, 255 ) + GetGValue( (clrColor & 0xFFFF) );
	const long B = MulDiv( 255 - GetBValue( clrColor ), iScale, 255 ) + GetBValue( clrColor );

	return RGB( R, G, B ); 
}

/*!
* \brief blah
*
* blah
*/

COLORREF XPopupMenuItem::DarkenColor( const unsigned int iScale, const COLORREF clrColor )
{
	const long R = MulDiv( GetRValue( clrColor ), ( 255 - iScale ), 255 );
	const long G = MulDiv( GetGValue( clrColor ), ( 255 - iScale ), 255 );
	const long B = MulDiv( GetBValue( clrColor ), ( 255 - iScale ), 255 );

	return RGB( R, G, B ); 
}
