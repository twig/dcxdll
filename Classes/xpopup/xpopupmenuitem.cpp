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

#include "xpopupmenuitem.h"
#include "xpopupmenu.h"

/*!
 * \brief Constructor
 *
 * blah
 */

XPopupMenuItem::XPopupMenuItem( XPopupMenu * Parent, const BOOL bSep )
: m_pXParentMenu( Parent ), m_bSep( bSep ), m_nIcon(-1), m_bSubMenu(FALSE) {
}

/*!
 * \brief Constructor
 *
 * blah
 */

XPopupMenuItem::XPopupMenuItem( XPopupMenu * Parent, const TString &tsItemText, const int nIcon, const BOOL bSubMenu ) 
: m_pXParentMenu( Parent ), m_tsItemText( tsItemText), m_nIcon( nIcon ), m_bSubMenu( bSubMenu ), m_bSep( FALSE ) {
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

    if ( this->m_pXParentMenu->getName( ) == "mirc" || this->m_pXParentMenu->getName( ) == "mircbar" ) {
      if ( this->m_tsItemText.numtok( "\v" ) > 1 ) {
        this->m_nIcon = this->m_tsItemText.gettok( 1, "\v").to_int( ) - 1;
        this->m_tsItemText = this->m_tsItemText.gettok( 2, "\v" );
        this->m_tsItemText.trim( );
      }

      GetTextExtentPoint32( hdc, this->m_tsItemText.to_chr( ), this->m_tsItemText.len( ), &size );
    }
    else {
      char res[900];
      mIRCeval( this->m_tsItemText.to_chr( ), res );
      this->m_tsItemText = res;
      GetTextExtentPoint32( hdc, res, lstrlen( res ), &size );
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

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItem( const LPDRAWITEMSTRUCT lpdis ) {

  LPXPMENUCOLORS lpcol = this->m_pXParentMenu->getColors( );
  UINT iItemStyle = this->m_pXParentMenu->getItemStyle( );

	// playing around with menu transparency
	if (SetLayeredWindowAttributesUx != NULL) {
		UINT alpha = this->m_pXParentMenu->IsAlpha();

		// we use the zero value to indicate NO alpha, rather than fully transparent.
		if (alpha <= 255) {
			HWND hMenuWnd = WindowFromDC(lpdis->hDC);
			if (IsWindow(hMenuWnd)) {
				DWORD dwStyle = GetWindowLong(hMenuWnd, GWL_EXSTYLE);
				if (!(dwStyle & WS_EX_LAYERED)) {
					SetWindowLong(hMenuWnd, GWL_EXSTYLE, dwStyle | WS_EX_LAYERED);
					SetLayeredWindowAttributesUx(hMenuWnd, 0, alpha, LWA_ALPHA); // 0xCC = 80% Opaque
				}
			}
		}
	}
  // All Items
  this->DrawItemBackground( lpdis, lpcol );
  this->DrawItemBox( lpdis, lpcol );

  if ( this->m_pXParentMenu->getName( ) == "mircbar" ) {

    if ( this->m_tsItemText.numtok( "\v" ) > 1 ) {

      this->m_nIcon = this->m_tsItemText.gettok( 1, "\v").to_int( ) - 1;
      this->m_tsItemText = this->m_tsItemText.gettok( 2, "\v" );
      this->m_tsItemText.trim( );
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

void XPopupMenuItem::DrawItemBackground( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol ) {

   switch ( this->m_pXParentMenu->getStyle( ) ) {

    case XPopupMenu::XPMS_ICY:
      {
        this->DrawGradient( lpdis->hDC, &lpdis->rcItem, LightenColor( 200, lpcol->m_clrBox ), lpcol->m_clrBox, TRUE );
      }
      break;

    case XPopupMenu::XPMS_ICY_REV:
      {
        this->DrawGradient( lpdis->hDC, &lpdis->rcItem, lpcol->m_clrBox, LightenColor( 200, lpcol->m_clrBox ), TRUE );
      }
      break;

    case XPopupMenu::XPMS_GRADE:
      {
        //lpdis->rcItem.right+=1;
        this->DrawGradient( lpdis->hDC, &lpdis->rcItem, lpcol->m_clrBox, LightenColor( 200, lpcol->m_clrBox ), FALSE );
      }
      break;

    case XPopupMenu::XPMS_GRADE_REV:
      {
        //lpdis->rcItem.right+=1;
        this->DrawGradient( lpdis->hDC, &lpdis->rcItem, LightenColor( 200, lpcol->m_clrBox ), lpcol->m_clrBox, FALSE );
      }
      break;

     case XPopupMenu::XPMS_CUSTOM:
      {
        HBITMAP hBitmap = this->m_pXParentMenu->getBackBitmap( );

        if ( hBitmap != NULL ) {

          HDC hdcbmp = CreateCompatibleDC( lpdis->hDC );
					if (hdcbmp != NULL) {
						BITMAP bmp;

						GetObject( hBitmap, sizeof( BITMAP ), &bmp );
						HBITMAP hOldBm = (HBITMAP)SelectObject( hdcbmp, hBitmap );
						StretchBlt( lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right - lpdis->rcItem.left, 
							lpdis->rcItem.bottom - lpdis->rcItem.top, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY );

						SelectObject( hdcbmp, hOldBm );
						DeleteDC( hdcbmp );
					}
          break;
        }
      }

    case XPopupMenu::XPMS_OFFICEXP:
    case XPopupMenu::XPMS_NORMAL:
    case XPopupMenu::XPMS_OFFICE2003_REV:
    case XPopupMenu::XPMS_OFFICE2003:
    default:
      {
        HBRUSH hBrush = CreateSolidBrush( lpcol->m_clrBack );
        FillRect( lpdis->hDC, &lpdis->rcItem, hBrush );
        DeleteObject( hBrush );
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
			FillRect(lpdis->hDC, &rc, hBrush);
			DeleteObject(hBrush);
			break;
		}

		case XPopupMenu::XPMS_VERTICAL_REV:
		{
			DrawVerticalBar(lpdis, lpcol, TRUE);
			break;
		}

		case XPopupMenu::XPMS_VERTICAL:
		{
			DrawVerticalBar(lpdis, lpcol, FALSE);
			break;
		}

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
	HBRUSH hBrush = (HBRUSH) CreateSolidBrush( bDis?lpcol->m_clrDisabledSelection:lpcol->m_clrSelection );
	HPEN hOldPen = (HPEN) SelectObject( lpdis->hDC, hPen );
	HBRUSH hOldBrush = (HBRUSH) SelectObject( lpdis->hDC, hBrush );

	RECT rc;
	CopyRect( &rc, &lpdis->rcItem );
	if (bRounded)
		RoundRect( lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom, 10, 10 );
	else
		Rectangle( lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom );

	DeleteObject( SelectObject( lpdis->hDC, hOldPen ) );
	DeleteObject( SelectObject( lpdis->hDC, hOldBrush ) );
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItemCheckBox( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const BOOL bDis ) {

  HPEN hPen = CreatePen( PS_SOLID, 1, lpcol->m_clrSelectionBorder );
  HBRUSH hBrush = (HBRUSH) CreateSolidBrush( bDis?lpcol->m_clrDisabledCheckBox:lpcol->m_clrCheckBox );
  HPEN hOldPen = (HPEN) SelectObject( lpdis->hDC, hPen );
  HBRUSH hOldBrush = (HBRUSH) SelectObject( lpdis->hDC, hBrush );

  RECT rc;
  CopyRect( &rc, &lpdis->rcItem );
  InflateRect( &rc, 0, -1 );
  rc.left += 1;
  rc.right = rc.left + rc.bottom - rc.top;

  Rectangle( lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom );

  DeleteObject( SelectObject( lpdis->hDC, hOldPen ) );

  hPen = CreatePen( PS_SOLID, 1, bDis?lpcol->m_clrDisabledText:lpcol->m_clrText );
  hOldPen = (HPEN)SelectObject( lpdis->hDC, hPen );

  int x = ( rc.right + rc.left ) / 2 - 3;
  int y = ( rc.bottom + rc.top ) / 2 - 3;
	
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
	
  DeleteObject( SelectObject( lpdis->hDC, hOldPen ) );
  DeleteObject( SelectObject( lpdis->hDC, hOldBrush ) );
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItemText( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const BOOL bDis ) {

	SetTextColor( lpdis->hDC, (bDis?lpcol->m_clrDisabledText:((lpdis->itemState & ODS_SELECTED)?lpcol->m_clrSelectedText:lpcol->m_clrText)) );

  SetBkMode( lpdis->hDC, TRANSPARENT );

  RECT rc;
  CopyRect( &rc, &lpdis->rcItem );
  rc.left += XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD;

  if ( this->m_tsItemText.numtok( TSTAB ) > 1 ) {

    TString lefttext(this->m_tsItemText.gettok( 1, TSTAB ));
    lefttext.trim( );
    TString righttext(this->m_tsItemText.gettok( 2, TSTAB ));
    righttext.trim( );

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
		mIRC_DrawText( lpdis->hDC, this->m_tsItemText, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, false);
  }
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItemIcon( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const UINT iExStyles, const BOOL bSel, const BOOL bDis ) {

  HIMAGELIST himl = this->m_pXParentMenu->getImageList( );

  int x = ( XPMI_BOXLPAD + XPMI_BOXLPAD + XPMI_BOXWIDTH - XPMI_ICONSIZE ) / 2;
  int y = ( lpdis->rcItem.top + lpdis->rcItem.bottom - XPMI_ICONSIZE ) / 2;

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
  HPEN hOldPen = (HPEN) SelectObject( lpdis->hDC, hPen );

  int x = lpdis->rcItem.right - 9;
  int y = ( lpdis->rcItem.bottom + lpdis->rcItem.top ) / 2 - 5;

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
  
  SelectObject( lpdis->hDC, hOldPen );
  DeleteObject( hPen );
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

        int x1 = lpdis->rcItem.left;
        int x2 = lpdis->rcItem.right;
        int y = ( lpdis->rcItem.bottom + lpdis->rcItem.top) / 2;

        HPEN oldPen = (HPEN) SelectObject( lpdis->hDC, hPen) ;
        MoveToEx( lpdis->hDC, x1, y, NULL );
        LineTo( lpdis->hDC, x2, y );
        SelectObject( lpdis->hDC, oldPen );
        DeleteObject(hPen);
      }
      break;

    case XPopupMenu::XPMS_OFFICEXP:
    case XPopupMenu::XPMS_OFFICE2003_REV:
    case XPopupMenu::XPMS_OFFICE2003:
    default:
      {

        HPEN hPen = CreatePen( PS_SOLID, 1, lpcol->m_clrSeparatorLine );

        int x1 = XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD;
        int x2 = lpdis->rcItem.right;
        int y = ( lpdis->rcItem.bottom + lpdis->rcItem.top) / 2;

        HPEN oldPen = (HPEN) SelectObject( lpdis->hDC, hPen) ;
        MoveToEx( lpdis->hDC, x1, y, NULL );
        LineTo( lpdis->hDC, x2, y );
        SelectObject( lpdis->hDC, oldPen );
        DeleteObject(hPen);
      }
   }
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawGradient( const HDC hdc, const LPRECT lprc, const COLORREF clrStart, const COLORREF clrEnd, const BOOL bHorz ) {

  BYTE StartRed   = GetRValue( clrStart );
  BYTE StartGreen = GetGValue( clrStart );
  BYTE StartBlue  = GetBValue( clrStart );

  BYTE EndRed    = GetRValue( clrEnd );
  BYTE EndGreen  = GetGValue( clrEnd );
  BYTE EndBlue   = GetBValue( clrEnd );

  int n;
  int dy = 2;

  if ( bHorz == TRUE )
    n = lprc->bottom - lprc->top - dy;
  else
    n = lprc->right - lprc->left;

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
    FillRect( hdc, &rc, hBrush );
    DeleteObject( hBrush );
  }
}


void XPopupMenuItem::DrawVerticalBar(const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const BOOLEAN bReversed) {
	RECT rcIntersect;
	RECT rcClip;
	RECT rcBar;

	// get the size of the bar on the left
	GetClipBox(lpdis->hDC, &rcClip);
	CopyRect(&rcBar, &rcClip);

	rcBar.left = XPMI_BOXLPAD;
	rcBar.right = XPMI_BOXLPAD + XPMI_BOXWIDTH;
	rcBar.bottom += 1; // prevent the black line at the bottom

	// get the rect of the box which will draw the box
	SetRect(&rcIntersect, rcBar.left, lpdis->rcItem.top, rcBar.right, lpdis->rcItem.bottom);

	// set up a buffer to draw the whole gradient bar
	HDC hdcBuffer = CreateCompatibleDC(lpdis->hDC);

	if (hdcBuffer != NULL) {
		HBITMAP hbmp = CreateCompatibleBitmap(lpdis->hDC, rcBar.right - rcBar.left, rcBar.bottom - rcBar.top);

		if (hbmp != NULL) {
			HBITMAP hbmpOld = (HBITMAP) SelectObject(hdcBuffer, hbmp);

			// draw the gradient into the buffer
			if (bReversed)
				DrawGradient(hdcBuffer, &rcBar, lpcol->m_clrBox, LightenColor(200, lpcol->m_clrBox), TRUE);
			else
				DrawGradient(hdcBuffer, &rcBar, LightenColor(200, lpcol->m_clrBox), lpcol->m_clrBox, TRUE);

			// copy the box we want from the whole gradient bar
			BitBlt(lpdis->hDC, rcIntersect.left, rcIntersect.top, rcIntersect.right - rcIntersect.left, rcIntersect.bottom - rcIntersect.top, hdcBuffer, rcIntersect.left, rcIntersect.top, SRCCOPY);

			// clean up the memory object
			DeleteObject(	SelectObject(hdcBuffer, hbmpOld) );
		}
		DeleteDC(hdcBuffer);
	}
}



/*!
 * \brief blah
 *
 * blah
 */

COLORREF XPopupMenuItem::LightenColor( const unsigned int iScale, const COLORREF clrColor ) { 

  long R = MulDiv( 255 - GetRValue( clrColor ), iScale, 255 ) + GetRValue( clrColor );
  long G = MulDiv( 255 - GetGValue( clrColor ), iScale, 255 ) + GetGValue( clrColor );
  long B = MulDiv( 255 - GetBValue( clrColor ), iScale, 255 ) + GetBValue( clrColor );

  return RGB( R, G, B ); 
}

/*!
 * \brief blah
 *
 * blah
 */

COLORREF XPopupMenuItem::DarkenColor( const unsigned int iScale, const COLORREF clrColor )
{ 
  long R = MulDiv( GetRValue( clrColor ), ( 255 - iScale ), 255 );
  long G = MulDiv( GetGValue( clrColor ), ( 255 - iScale ), 255 );
  long B = MulDiv( GetBValue( clrColor ), ( 255 - iScale ), 255 );

  return RGB( R, G, B ); 
}