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

XPopupMenuItem::XPopupMenuItem( XPopupMenu * Parent, const bool bSep, ULONG_PTR dwDataBackup ) noexcept
: m_pXParentMenu( Parent ), m_bSep( bSep ), m_nIcon(-1), m_bSubMenu(false), m_dwItemDataBackup(dwDataBackup), m_bBigBitmap(false), m_bReserved(false)
{
}

/*!
 * \brief Constructor
 *
 * blah
 */

XPopupMenuItem::XPopupMenuItem( XPopupMenu * Parent, const TString &tsItemText, const int nIcon, const bool bSubMenu, ULONG_PTR dwDataBackup ) 
: m_pXParentMenu( Parent ), m_tsItemText( tsItemText ), m_nIcon( nIcon ), m_bSubMenu( bSubMenu ), m_bSep( false ), m_dwItemDataBackup(dwDataBackup), m_bBigBitmap(false), m_bReserved(false)
{
	m_tsItemText.trim();
}

/*!
 * \brief Destructor
 *
 * blah
 */

XPopupMenuItem::~XPopupMenuItem( ) noexcept
{

}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::setSubMenu( const bool bSubMenu ) noexcept
{
	this->m_bSubMenu = bSubMenu;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::setItemText( const TString & tsItemText )
{
	if ( !this->m_bSep )
		this->m_tsItemText = tsItemText;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::setItemIcon( const int nIcon ) noexcept
{
	if ( !this->m_bSep )
		this->m_nIcon = nIcon;
}

/*!
 * \brief blah
 *
 * blah
 */

const TString &XPopupMenuItem::getItemText( ) const noexcept
{
	return this->m_tsItemText;
}

/*!
 * \brief blah
 *
 * blah
 */

const int &XPopupMenuItem::getItemIcon(  ) const noexcept
{
	return this->m_nIcon;
}

/*!
 * \brief blah
 *
 * blah
 */

SIZE XPopupMenuItem::getItemSize( const HWND mHwnd )
{
	if (this->m_bSep)
		return{ XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD, XPMI_SEPHEIGHT };

	if (const auto typeHash = m_pXParentMenu->getNameHash(); ((typeHash == TEXT("mirc"_hash)) || (typeHash == TEXT("mircbar"_hash)) || (typeHash == TEXT("dialog"_hash))))
	{
		if (m_tsItemText.numtok(TEXT('\v')) > 1)
		{
			m_nIcon = m_tsItemText.getfirsttok(1, TEXT('\v')).to_int() - 1;		// tok 1, TEXT("\v")
			m_tsItemText = m_tsItemText.getnexttok(TEXT('\v')).trim();			// tok 2, TEXT("\v")
		}
	}
	else
		mIRCLinker::eval(m_tsItemText, m_tsItemText);

	//check if the first char is $chr(12), if so then the text is utf8 (this is kept for compatability with old script only)
	if (m_tsItemText[0] == 12)
	{
		// remove $chr(12) from text and trim whitespaces
		m_tsItemText = m_tsItemText.right(-1).trim();
	}

	SIZE size{ XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD, XPMI_HEIGHT };

	if (const auto hdc = GetDC(mHwnd); hdc != nullptr)
	{
		Auto(ReleaseDC(mHwnd, hdc));

		RECT rc{};
		TString tsStripped(m_tsItemText);
		tsStripped.strip();
		DrawText(hdc, tsStripped.to_chr(), gsl::narrow_cast<int>(tsStripped.len()), &rc, DT_CALCRECT | DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		size.cx += std::max<long>((rc.right - rc.left), XPMI_MINSTRING);

		//// Odd error in size returned by GetTextExtentPoint32() when dealing with utf text, length is cut short for some reason...
		//TString tsStripped(m_tsItemText);
		//tsStripped.strip();
		//tsStripped.Normalize();
		//GetTextExtentPoint32(hdc, tsStripped.to_chr(), tsStripped.len(), &size);
		//size.cx += std::max<long>(size.cx, XPMI_MINSTRING);
	}

	return size;
}
	
/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItem( const LPDRAWITEMSTRUCT lpdis )
{
	const auto lpcol = this->m_pXParentMenu->getColors( );
	const auto iItemStyle = this->m_pXParentMenu->getItemStyle( );
	const auto bGrayed = dcx_testflag(lpdis->itemState, ODS_GRAYED);
	const auto bSelected = dcx_testflag(lpdis->itemState, ODS_SELECTED);
	const auto bChecked = dcx_testflag(lpdis->itemState, ODS_CHECKED);

	//// playing around with menu transparency
	//const BYTE alpha = this->m_pXParentMenu->IsAlpha();
	//
	//// If alpha == 255 then menu is fully opaque so no need to change to layered.
	//if (alpha < 255) {
	//	HWND hMenuWnd = WindowFromDC(lpdis->hDC);
	//
	//	if (IsWindow(hMenuWnd)) {
	//		DWORD dwStyle = GetWindowExStyle(hMenuWnd);
	//
	//		if (!(dwStyle & WS_EX_LAYERED)) {
	//			SetWindowLong(hMenuWnd, GWL_EXSTYLE, dwStyle | WS_EX_LAYERED);
	//			SetLayeredWindowAttributes(hMenuWnd, 0, (BYTE)alpha, LWA_ALPHA); // 0xCC = 80% Opaque
	//			//RedrawWindow(hMenuWnd, nullptr, nullptr, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_UPDATENOW|RDW_INVALIDATE);
	//			// NB: Menus on XP will not show as transparent straight away when a transition effect is used when displaying the menu.
	//			// This can't be fixed at this time, live with it.
	//			// NB: Menus on Vista/Win7 also suffer from this.
	//		}
	//	}
	//}

//#if DCX_DEBUG_OUTPUT && 0
//	{
//		// test code to draw transparent background but have the text be opaque
//		auto hScreenHDC = GetDC(nullptr);
//		Auto(ReleaseDC(nullptr, hScreenHDC));
//		
//		HWND hMenuWnd = WindowFromDC(lpdis->hDC);
//
//		RECT rc = lpdis->rcItem;
//
//		MapWindowRect(hMenuWnd, nullptr, &rc);
//		BitBlt(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom, nullptr, rc.left, rc.top, SRCCOPY);
//	}
//#endif

	// All Items
	this->DrawItemBackground( lpdis, lpcol);
	this->DrawItemBox( lpdis, lpcol );

	// alrdy done in getsize
	//if (const auto &typeHash(m_pXParentMenu->getNameHash()); ((typeHash == TEXT("mircbar"_hash)) || (typeHash == TEXT("dialog"_hash))))
	//{
	//	if (m_tsItemText.numtok(TEXT('\v')) > 1)
	//	{
	//		m_nIcon = m_tsItemText.getfirsttok(1, TEXT('\v')).to_int() - 1;
	//		m_tsItemText = m_tsItemText.getnexttok(TEXT('\v')).trim();
	//	}
	//}

	// Item is selected
	if (this->m_pXParentMenu->getStyle() != XPopupMenu::XPMS_BUTTON)
	{
		if (bSelected)
		{
			if (bGrayed)
			{
				// only draw selection box if XPS_DISABLEDSEL
				if (dcx_testflag(iItemStyle, XPS_DISABLEDSEL))
					this->DrawItemSelection(lpdis, lpcol, true, this->m_pXParentMenu->IsRounded());
			}
			else
				this->DrawItemSelection( lpdis, lpcol, false, this->m_pXParentMenu->IsRounded() );
		}
	}

	// Separator
	if ( this->m_bSep )
		this->DrawItemSeparator( lpdis, lpcol );
	// Regular Item
	else {
		if (bChecked)
			this->DrawItemCheckBox(lpdis, lpcol, bGrayed);

		this->DrawItemText(lpdis, lpcol, bGrayed);

		if ( !bChecked || this->m_nIcon > -1 )
			this->DrawItemIcon(lpdis, lpcol, iItemStyle, bSelected, bGrayed);
		if ( this->m_bSubMenu )
			this->DrawItemSubArrow(lpdis, lpcol, bGrayed);
	}
}

/*!
 * \brief Draws the background of the actual item where text resides
 *
 * blah
 */

void XPopupMenuItem::DrawItemBackground(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol)
{
	switch (this->m_pXParentMenu->getStyle())
	{
	case XPopupMenu::XPMS_ICY:
		this->DrawGradient(lpdis->hDC, &lpdis->rcItem, lpcol->m_clrLightBox, lpcol->m_clrBox, true);
		break;

	case XPopupMenu::XPMS_ICY_REV:
		this->DrawGradient(lpdis->hDC, &lpdis->rcItem, lpcol->m_clrBox, lpcol->m_clrLightBox, true);
		break;

	case XPopupMenu::XPMS_GRADE:
		this->DrawGradient(lpdis->hDC, &lpdis->rcItem, lpcol->m_clrBox, lpcol->m_clrLightBox, false);
		break;

	case XPopupMenu::XPMS_GRADE_REV:
		this->DrawGradient(lpdis->hDC, &lpdis->rcItem, lpcol->m_clrLightBox, lpcol->m_clrBox, false);
		break;

	case XPopupMenu::XPMS_CUSTOM:
	case XPopupMenu::XPMS_CUSTOMBIG:
	{
		if (this->DrawMenuBitmap(lpdis, (this->m_pXParentMenu->getStyle() == XPopupMenu::XPMS_CUSTOMBIG), this->m_pXParentMenu->getBackBitmap()))
			break;
	}
	// fall through when unable to draw bitmap (no bitmap or error)
	[[fallthrough]];
	case XPopupMenu::XPMS_OFFICEXP:
	case XPopupMenu::XPMS_NORMAL:
	case XPopupMenu::XPMS_OFFICE2003_REV:
	case XPopupMenu::XPMS_OFFICE2003:
	case XPopupMenu::XPMS_BUTTON:
	case XPopupMenu::XPMS_VERTICAL:
	case XPopupMenu::XPMS_VERTICAL_REV:
	default:
	{
		if (const auto hBrush = CreateSolidBrush(lpcol->m_clrBack); hBrush != nullptr)
		{
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

void XPopupMenuItem::DrawItemBox(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol)
{
	switch (this->m_pXParentMenu->getStyle())
	{
	case XPopupMenu::XPMS_OFFICE2003_REV:
	{
		const RECT rc{ XPMI_BOXLPAD, lpdis->rcItem.top, XPMI_BOXLPAD + XPMI_BOXWIDTH, lpdis->rcItem.bottom };
		XPopupMenuItem::DrawGradient(lpdis->hDC, &rc, lpcol->m_clrBox, lpcol->m_clrLightBox);
		break;
	}

	case XPopupMenu::XPMS_OFFICEXP:
	{
		const RECT rc{ XPMI_BOXLPAD, lpdis->rcItem.top, XPMI_BOXLPAD + XPMI_BOXWIDTH, lpdis->rcItem.bottom };
		
		if (const auto hBrush = CreateSolidBrush(lpcol->m_clrBox); hBrush != nullptr)
		{
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
	case XPopupMenu::XPMS_CUSTOMBIG:
	case XPopupMenu::XPMS_NORMAL:
		break;

	case XPopupMenu::XPMS_BUTTON:
	{
		if (this->m_bSep)
			break;

		const RECT rc = lpdis->rcItem;

		UINT uiStyle = DFCS_BUTTONPUSH | DFCS_ADJUSTRECT;
		const auto bGrayed = dcx_testflag(lpdis->itemState, ODS_GRAYED);
		const auto bSelected = dcx_testflag(lpdis->itemState, ODS_SELECTED);

		if (bSelected)
			uiStyle |= DFCS_PUSHED;
		if (bGrayed)
			uiStyle |= DFCS_INACTIVE;

		DrawFrameControl(lpdis->hDC, &lpdis->rcItem, DFC_BUTTON, uiStyle);
		if (bSelected)
			this->DrawItemSelection(lpdis, lpcol, bGrayed, false);

		lpdis->rcItem = rc;
	}
	break;

	case XPopupMenu::XPMS_OFFICE2003:
	default:
	{
		const RECT rc{ XPMI_BOXLPAD, lpdis->rcItem.top, XPMI_BOXLPAD + XPMI_BOXWIDTH, lpdis->rcItem.bottom };

		XPopupMenuItem::DrawGradient(lpdis->hDC, &rc, lpcol->m_clrLightBox, lpcol->m_clrBox);
		break;
	}
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItemSelection( const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol, const bool bDis, const bool bRounded ) noexcept
{

	//if (dcx_testflag(lpdis->itemState, ODS_HOTLIGHT))
	//{
	//	DrawFrameControl(lpdis->hDC,&lpdis->rcItem,DFC_BUTTON,(bDis)?(DFCS_BUTTONPUSH|DFCS_INACTIVE):(DFCS_BUTTONPUSH|DFCS_HOT));
	//	return;
	//}

	const auto hPen = CreatePen( PS_SOLID, 1, lpcol->m_clrSelectionBorder );

	if (hPen == nullptr)
		return;
	Auto(DeletePen(hPen));

	const auto hOldPen = SelectPen( lpdis->hDC, hPen );
	Auto(SelectPen(lpdis->hDC, hOldPen));

	
	if (const auto hBrush = CreateSolidBrush(bDis ? lpcol->m_clrDisabledSelection : lpcol->m_clrSelection); hBrush != nullptr)
	{
		Auto(DeleteBrush(hBrush));

		const auto hOldBrush = SelectBrush( lpdis->hDC, hBrush );
		Auto(SelectBrush(lpdis->hDC, hOldBrush));

		const RECT rc = lpdis->rcItem;

		if (bRounded)
			RoundRect( lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom, 10, 10 );
		else
			Rectangle( lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom );
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItemCheckBox( const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol, const bool bDis ) noexcept
{
	const auto hBrush = CreateSolidBrush(bDis ? lpcol->m_clrDisabledCheckBox : lpcol->m_clrCheckBox);
	Auto(DeleteBrush(hBrush));

	const auto hPenBorder = CreatePen(PS_SOLID, 1, lpcol->m_clrSelectionBorder);
	Auto(DeletePen(hPenBorder));

	const auto hPenText = CreatePen(PS_SOLID, 1, bDis ? lpcol->m_clrDisabledText : lpcol->m_clrText);
	Auto(DeletePen(hPenText));

	if ((hBrush == nullptr) || (hPenBorder == nullptr) || (hPenText == nullptr))
		return;

	const auto hOldBrush = SelectBrush(lpdis->hDC, hBrush);
	Auto(SelectBrush(lpdis->hDC, hOldBrush));

	const auto hOldPen = SelectPen(lpdis->hDC, hPenBorder);
	Auto(SelectPen(lpdis->hDC, hOldPen));

	RECT rc = lpdis->rcItem;

	InflateRect( &rc, 0, -1 );
	rc.left += 1;
	rc.right = rc.left + rc.bottom - rc.top;

	//RoundRect( lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom, 5, 5 );
	Rectangle( lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom );

	const auto hOldPenTxt = SelectPen( lpdis->hDC, hPenText );
	Auto(SelectPen(lpdis->hDC, hOldPenTxt));

	const auto x = (rc.right + rc.left) / 2 - 3;
	const auto y = (rc.bottom + rc.top) / 2 - 3;

	MoveToEx( lpdis->hDC, x, y+2, nullptr );
	LineTo( lpdis->hDC, x, y+5 );
	MoveToEx( lpdis->hDC, x+1, y+3, nullptr );
	LineTo( lpdis->hDC, x+1, y+6 );
	MoveToEx( lpdis->hDC, x+2, y+4, nullptr );
	LineTo( lpdis->hDC, x+2, y+7 );
	MoveToEx( lpdis->hDC, x+3, y+3, nullptr );
	LineTo( lpdis->hDC, x+3, y+6 );
	MoveToEx( lpdis->hDC, x+4, y+2, nullptr );
	LineTo( lpdis->hDC, x+4, y+5 );
	MoveToEx( lpdis->hDC, x+5, y+1, nullptr );
	LineTo( lpdis->hDC, x+5, y+4 );
	MoveToEx( lpdis->hDC, x+6, y, nullptr );
	LineTo( lpdis->hDC, x+6, y+3 );
}

/*!
 * \brief blah
 *
 * blah
 */
void XPopupMenuItem::DrawItemText( const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol, const bool bDis )
{
	const auto oldClr = SetTextColor(lpdis->hDC, (bDis ? lpcol->m_clrDisabledText : ((dcx_testflag(lpdis->itemState, ODS_SELECTED)) ? lpcol->m_clrSelectedText : lpcol->m_clrText)));
	Auto(SetTextColor(lpdis->hDC, oldClr));

	const auto oldBkg = SetBkMode(lpdis->hDC, TRANSPARENT);
	Auto(SetBkMode(lpdis->hDC, oldBkg));

	RECT rc = lpdis->rcItem;
	rc.left += XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD;

	//TString txt;
	////check if the first char is $chr(12), if so then the text is utf8 (this is kept for compatability with old script only)
	//if ( this->m_tsItemText[0] == 12)
	//{
	//	// remove $chr(12) from text and trim whitespaces
	//	txt = this->m_tsItemText.right(-1).trim();
	//}
	//else // not utf8 so copy
	//	txt = this->m_tsItemText;
	//
	//if ( txt.numtok( TSTABCHAR) > 1 )
	//{
	//	const auto lefttext(txt.getfirsttok(1, TSTABCHAR).trim());
	//	const auto righttext(txt.getnexttok(TSTABCHAR).trim());
	//
	//	//DrawTextEx( lpdis->hDC, lefttext.to_chr( ), lefttext.len( ), &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, nullptr );
	//	mIRC_DrawText( lpdis->hDC, lefttext, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, false);
	//
	//	if (!righttext.empty()) 
	//	{
	//		rc.right -= 15;
	//		//DrawTextEx( lpdis->hDC, (TCHAR *)righttext.to_chr( ), righttext.len( ), &rc, DT_RIGHT | DT_SINGLELINE | DT_VCENTER, nullptr );
	//		mIRC_DrawText( lpdis->hDC, righttext, &rc, DT_RIGHT | DT_SINGLELINE | DT_VCENTER, false);
	//	}
	//}
	//else {
	//	//DrawTextEx( lpdis->hDC, this->m_tsItemText.to_chr( ), this->m_tsItemText.len( ), &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, nullptr );
	//	mIRC_DrawText( lpdis->hDC, txt, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, false);
	//}

	if (m_tsItemText.numtok(TSTABCHAR) > 1)
	{
		const auto lefttext(m_tsItemText.getfirsttok(1, TSTABCHAR).trim());
		const auto righttext(m_tsItemText.getnexttok(TSTABCHAR).trim());

		//DrawTextEx( lpdis->hDC, lefttext.to_chr( ), lefttext.len( ), &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, nullptr );
		mIRC_DrawText(lpdis->hDC, lefttext, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, false);

		if (!righttext.empty())
		{
			rc.right -= 15;
			//DrawTextEx( lpdis->hDC, (TCHAR *)righttext.to_chr( ), righttext.len( ), &rc, DT_RIGHT | DT_SINGLELINE | DT_VCENTER, nullptr );
			mIRC_DrawText(lpdis->hDC, righttext, &rc, DT_RIGHT | DT_SINGLELINE | DT_VCENTER, false);
		}
	}
	else {
		//DrawTextEx( lpdis->hDC, this->m_tsItemText.to_chr( ), this->m_tsItemText.len( ), &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, nullptr );
		mIRC_DrawText(lpdis->hDC, m_tsItemText, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, false);
	}
}
/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItemIcon( const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol, const UINT iExStyles, const bool bSel, const bool bDis ) noexcept
{
	if (const auto himl = this->m_pXParentMenu->getImageList(); (himl != nullptr && this->m_nIcon > -1 && this->m_nIcon < ImageList_GetImageCount( himl ) ))
	{
		constexpr auto x = (XPMI_BOXLPAD + XPMI_BOXLPAD + XPMI_BOXWIDTH - XPMI_ICONSIZE) / 2;
		const auto y = (lpdis->rcItem.top + lpdis->rcItem.bottom - XPMI_ICONSIZE) / 2;

		// Selected Item
		if ( bSel )
		{
			// Disabled
			if ( bDis )
				ImageList_DrawEx( himl, this->m_nIcon, lpdis->hDC, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT | ILD_BLEND50 );
			else {

				if (dcx_testflag(iExStyles, XPS_ICON3DSHADOW))
				{
					ImageList_DrawEx( himl, this->m_nIcon, lpdis->hDC, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT | ILD_BLEND25 );
					ImageList_DrawEx( himl, this->m_nIcon, lpdis->hDC, x - 1, y - 1, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT );

				}
				else if (dcx_testflag(iExStyles, XPS_ICON3D))
					ImageList_DrawEx( himl, this->m_nIcon, lpdis->hDC, x - 1, y - 1, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT );
				else
					ImageList_DrawEx( himl, this->m_nIcon, lpdis->hDC, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT );
			}
		}
		// Not selected
		else {

			if ( bDis )
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

void XPopupMenuItem::DrawItemSubArrow( const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol, const bool bDis ) noexcept
{

//#ifdef DCX_USE_GDIPLUS
//	const int x = lpdis->rcItem.right - 9;
//	const int y = ( lpdis->rcItem.bottom + lpdis->rcItem.top ) / 2 - 5;
//
//	if (!Dcx::GDIModule.isUseable())
//	{
//		auto hPen = CreatePen(PS_SOLID, 1, bDis ? lpcol->m_clrDisabledText : lpcol->m_clrText);
//
//		if (hPen == nullptr)
//			return;
//		Auto(DeletePen(hPen));
//
//		const auto hOldPen = SelectPen(lpdis->hDC, hPen);
//		Auto(SelectPen(lpdis->hDC, hOldPen));
//
//		MoveToEx(lpdis->hDC, x, y, nullptr);
//		LineTo(lpdis->hDC, x + 1, y);
//		MoveToEx(lpdis->hDC, x, y + 1, nullptr);
//		LineTo(lpdis->hDC, x + 2, y + 1);
//		MoveToEx(lpdis->hDC, x, y + 2, nullptr);
//		LineTo(lpdis->hDC, x + 3, y + 2);
//		MoveToEx(lpdis->hDC, x, y + 3, nullptr);
//		LineTo(lpdis->hDC, x + 4, y + 3);
//		MoveToEx(lpdis->hDC, x, y + 4, nullptr);
//		LineTo(lpdis->hDC, x + 5, y + 4);
//		MoveToEx(lpdis->hDC, x, y + 5, nullptr);
//		LineTo(lpdis->hDC, x + 4, y + 5);
//		MoveToEx(lpdis->hDC, x, y + 6, nullptr);
//		LineTo(lpdis->hDC, x + 3, y + 6);
//		MoveToEx(lpdis->hDC, x, y + 7, nullptr);
//		LineTo(lpdis->hDC, x + 2, y + 7);
//		MoveToEx(lpdis->hDC, x, y + 8, nullptr);
//		LineTo(lpdis->hDC, x + 1, y + 8);
//	}
//	else {
//		Gdiplus::Graphics gfx( lpdis->hDC );
//		const COLORREF clrShape = bDis?lpcol->m_clrDisabledText:lpcol->m_clrText;
//		RECT rc{ x,y,x + 5,y + 8 };
//
//		gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias8x8);
//		//gfx.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
//		gfx.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
//		gfx.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
//		gfx.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
//		Gdiplus::SolidBrush blackBrush(Gdiplus::Color(128, GetRValue(clrShape), GetGValue(clrShape), GetBValue(clrShape)));
//		// Create an array of Point objects that define the polygon.
//		Gdiplus::Point points[3];
//		points[0].X = rc.left;
//		points[0].Y = rc.top;
//		points[1].X = rc.right;
//		points[1].Y = (rc.top + ((rc.bottom - rc.top)/2));
//		points[2].X = rc.left;
//		points[2].Y = rc.bottom;
//		// Fill the polygon.
//		gfx.FillPolygon(&blackBrush, &points[0], 3);
//	}
//	ExcludeClipRect( lpdis->hDC, lpdis->rcItem.right - 11, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom );
//#else

	const auto hPen = CreatePen(PS_SOLID, 1, bDis ? lpcol->m_clrDisabledText : lpcol->m_clrText);

	if (hPen == nullptr)
		return;
	Auto(DeletePen(hPen));

	const auto hOldPen = SelectPen(lpdis->hDC, hPen);
	Auto(SelectPen(lpdis->hDC, hOldPen));

	const auto x = lpdis->rcItem.right - 9;
	const auto y = (lpdis->rcItem.bottom + lpdis->rcItem.top) / 2 - 5;

	MoveToEx( lpdis->hDC, x, y, nullptr );
	LineTo( lpdis->hDC, x+1, y );
	MoveToEx( lpdis->hDC, x, y+1, nullptr );
	LineTo( lpdis->hDC, x+2, y+1 );
	MoveToEx( lpdis->hDC, x, y+2, nullptr );
	LineTo( lpdis->hDC, x+3, y+2 );
	MoveToEx( lpdis->hDC, x, y+3, nullptr );
	LineTo( lpdis->hDC, x+4, y+3 );
	MoveToEx( lpdis->hDC, x, y+4, nullptr );
	LineTo( lpdis->hDC, x+5, y+4 );
	MoveToEx( lpdis->hDC, x, y+5, nullptr );
	LineTo( lpdis->hDC, x+4, y+5 );
	MoveToEx( lpdis->hDC, x, y+6, nullptr );
	LineTo( lpdis->hDC, x+3, y+6 );
	MoveToEx( lpdis->hDC, x, y+7, nullptr );
	LineTo( lpdis->hDC, x+2, y+7 );
	MoveToEx( lpdis->hDC, x, y+8, nullptr );
	LineTo( lpdis->hDC, x+1, y+8 );

	ExcludeClipRect( lpdis->hDC, lpdis->rcItem.right - 11, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom );
//#endif
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawItemSeparator( const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol ) noexcept
{
	switch (this->m_pXParentMenu->getStyle())
	{
	case XPopupMenu::XPMS_ICY:
	case XPopupMenu::XPMS_ICY_REV:
	case XPopupMenu::XPMS_NORMAL:
	case XPopupMenu::XPMS_BUTTON:
	{
		const auto hPen = CreatePen(PS_SOLID, 1, lpcol->m_clrSeparatorLine);

		if (hPen == nullptr)
			break;
		Auto(DeletePen(hPen));

		const auto x1 = lpdis->rcItem.left;
		const auto x2 = lpdis->rcItem.right;
		const auto y = (lpdis->rcItem.bottom + lpdis->rcItem.top) / 2;

		const auto oldPen = SelectPen(lpdis->hDC, hPen);
		Auto(SelectPen(lpdis->hDC, oldPen));

		MoveToEx(lpdis->hDC, x1, y, nullptr);
		LineTo(lpdis->hDC, x2, y);
	}
	break;

	case XPopupMenu::XPMS_OFFICEXP:
	case XPopupMenu::XPMS_OFFICE2003_REV:
	case XPopupMenu::XPMS_OFFICE2003:
	case XPopupMenu::XPMS_GRADE:
	case XPopupMenu::XPMS_GRADE_REV:
	case XPopupMenu::XPMS_CUSTOM:
	case XPopupMenu::XPMS_CUSTOMBIG:
	case XPopupMenu::XPMS_VERTICAL:
	case XPopupMenu::XPMS_VERTICAL_REV:
	default:
	{
		const auto hPen = CreatePen(PS_SOLID, 1, lpcol->m_clrSeparatorLine);

		if (hPen == nullptr)
			break;
		Auto(DeletePen(hPen));

		const auto x1 = XPMI_BOXLPAD + XPMI_BOXWIDTH + XPMI_BOXRPAD;
		const auto x2 = lpdis->rcItem.right;
		const auto y = (lpdis->rcItem.bottom + lpdis->rcItem.top) / 2;

		const auto oldPen = SelectPen(lpdis->hDC, hPen);
		Auto(SelectPen(lpdis->hDC, oldPen));

		MoveToEx(lpdis->hDC, x1, y, nullptr);
		LineTo(lpdis->hDC, x2, y);
	}
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuItem::DrawGradient( const HDC hdc, const RECT *const lprc, const COLORREF clrStart, const COLORREF clrEnd, const bool bHorz ) noexcept
{
	const auto StartRed = GetRValue(clrStart);
	const auto StartGreen = GetGValue((clrStart & 0xFFFF));
	const auto StartBlue = GetBValue(clrStart);

	const auto EndRed = GetRValue(clrEnd);
	const auto EndGreen = GetGValue((clrEnd & 0xFFFF));
	const auto EndBlue = GetBValue(clrEnd);

	//TRIVERTEX        vert[2] = { 0 };
	//
	//vert [0] .x      = lprc->left;
	//vert [0] .y      = lprc->top;
	//vert [0] .Red    = (COLOR16)((StartRed & 0xff) << 8);
	//vert [0] .Green  = (COLOR16)((StartGreen & 0xff) << 8);
	//vert [0] .Blue   = (COLOR16)((StartBlue & 0xff) << 8);
	//vert [0] .Alpha  = 0xff00;
	//
	//vert [1] .x      = lprc->right;
	//vert [1] .y      = lprc->bottom;
	//vert [1] .Red    = (COLOR16)((EndRed & 0xff) << 8);
	//vert [1] .Green  = (COLOR16)((EndGreen & 0xff) << 8);
	//vert [1] .Blue   = (COLOR16)((EndBlue & 0xff) << 8);
	//vert [1] .Alpha  = 0xff00;
	////vert [1] .Alpha  = 0x0C00;

	TRIVERTEX vert[2] = {
		{ lprc->left, lprc->top, gsl::narrow_cast<COLOR16>((StartRed & 0xff) << 8), gsl::narrow_cast<COLOR16>((StartGreen & 0xff) << 8), gsl::narrow_cast<COLOR16>((StartBlue & 0xff) << 8), 0xff00 },
		{ lprc->right, lprc->bottom, gsl::narrow_cast<COLOR16>((EndRed & 0xff) << 8), gsl::narrow_cast<COLOR16>((EndGreen & 0xff) << 8), gsl::narrow_cast<COLOR16>((EndBlue & 0xff) << 8), 0xff00 }
	};

	GRADIENT_RECT    gRect{ 0,1 };

	const ULONG gMode = (bHorz) ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H;

	if (!GradientFill(hdc,&vert[0],std::extent_v<decltype(vert)>,&gRect,1,gMode))
	{
		// if GradientFill fails do our own method.
		constexpr auto dy = 2;
		const auto n = ( bHorz ) ? lprc->bottom - lprc->top - dy : lprc->right - lprc->left - dy;

		RECT rc{};

		for (auto dn = decltype(n){0}; dn <= n; dn += dy)
		{
			//const auto Red = gsl::narrow_cast<BYTE>(MulDiv(gsl::narrow_cast<int>(EndRed) - StartRed, dn, n) + StartRed);
			//const auto Green = gsl::narrow_cast<BYTE>(MulDiv(gsl::narrow_cast<int>(EndGreen) - StartGreen, dn, n) + StartGreen);
			//const auto Blue = gsl::narrow_cast<BYTE>(MulDiv(gsl::narrow_cast<int>(EndBlue) - StartBlue, dn, n) + StartBlue);

			const auto Red = gsl::narrow_cast<BYTE>(Dcx::dcxMulDiv32(gsl::narrow_cast<int>(EndRed) - StartRed, dn, n) + StartRed);
			const auto Green = gsl::narrow_cast<BYTE>(Dcx::dcxMulDiv32(gsl::narrow_cast<int>(EndGreen) - StartGreen, dn, n) + StartGreen);
			const auto Blue = gsl::narrow_cast<BYTE>(Dcx::dcxMulDiv32(gsl::narrow_cast<int>(EndBlue) - StartBlue, dn, n) + StartBlue);

			if ( bHorz )
				SetRect( &rc, lprc->left, lprc->top + dn, lprc->right , lprc->top + dn + dy );
			else
				SetRect( &rc, lprc->left + dn, lprc->top, lprc->left + dn + dy, lprc->bottom );

			if (const auto hBrush = CreateSolidBrush(RGB(Red, Green, Blue)); hBrush != nullptr)
			{
				FillRect( hdc, &rc, hBrush );
				DeleteBrush( hBrush );
			}
		}
	}
}

void XPopupMenuItem::DrawVerticalBar(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol, const bool bReversed)
{
	// Working code: Calculates height of complete menu, and draws gradient to fill. Samples taken off complete gradient when needed.
	///*
	// Get height of all menu items
			//int menuH = 0;
			//int i = 0;
	// GetMenuItemRect() calls here cause submenus to fails to render on first opening for some reason.
			//while (GetMenuItemRect(mIRCLinker::getHWND(), (HMENU) lpdis->hwndItem, i, &rc) != FALSE) {
			//	menuH += (rc.bottom - rc.top);
			//	i++;
			//}
			//RECT rcIntersect;
			//RECT rcBar;

			//// Fix to remove black line at the bottom
			//menuH++;
	// using bitmap height like this fixes that.
	BITMAP bm{};
	if (GetObject((HBITMAP)GetCurrentObject(lpdis->hDC, OBJ_BITMAP), sizeof(BITMAP), &bm) == 0)
		return;

	//const auto menuH = bm.bmHeight;

	//RECT rcIntersect{};
	//RECT rcBar{};

	//// get the size of the bar on the left
	//SetRect(&rcBar, XPMI_BOXLPAD, 0, XPMI_BOXLPAD + XPMI_BOXWIDTH, menuH);

	//// get the rect of the box which will draw JUST the box (prevents redraw over items already done)
	//SetRect(&rcIntersect, rcBar.left, lpdis->rcItem.top, rcBar.right, lpdis->rcItem.bottom);

	// get the size of the bar on the left
	RECT rcBar{ XPMI_BOXLPAD, 0, (XPMI_BOXLPAD + XPMI_BOXWIDTH), bm.bmHeight };

	// get the rect of the box which will draw JUST the box (prevents redraw over items already done)
	RECT rcIntersect{ XPMI_BOXLPAD, lpdis->rcItem.top, (XPMI_BOXLPAD + XPMI_BOXWIDTH), lpdis->rcItem.bottom };

#if DCX_USE_WRAPPERS
	// set up a buffer to draw the whole gradient bar
	const Dcx::dcxHDCBuffer hdcBuffer(lpdis->hDC, rcBar);

	// draw the gradient into the buffer
	if (bReversed)
		XPopupMenuItem::DrawGradient(hdcBuffer, &rcBar, lpcol->m_clrBox, lpcol->m_clrLightBox, true);
	else
		XPopupMenuItem::DrawGradient(hdcBuffer, &rcBar, lpcol->m_clrLightBox, lpcol->m_clrBox, true);

	// copy the box we want from the whole gradient bar
	BitBlt(lpdis->hDC, rcIntersect.left, rcIntersect.top, rcIntersect.right - rcIntersect.left, rcIntersect.bottom - rcIntersect.top, hdcBuffer, rcIntersect.left, rcIntersect.top, SRCCOPY);
#else
	// set up a buffer to draw the whole gradient bar
	auto hdcBuffer = CreateHDCBuffer(lpdis->hDC, &rcBar);

	if (hdcBuffer != nullptr) {
		Auto(DeleteHDCBuffer(hdcBuffer));

		// draw the gradient into the buffer
		if (bReversed)
			XPopupMenuItem::DrawGradient(*hdcBuffer, &rcBar, lpcol->m_clrBox, lpcol->m_clrLightBox, TRUE);
		else
			XPopupMenuItem::DrawGradient(*hdcBuffer, &rcBar, lpcol->m_clrLightBox, lpcol->m_clrBox, TRUE);

		// copy the box we want from the whole gradient bar
		BitBlt(lpdis->hDC, rcIntersect.left, rcIntersect.top, rcIntersect.right - rcIntersect.left, rcIntersect.bottom - rcIntersect.top, *hdcBuffer, rcIntersect.left, rcIntersect.top, SRCCOPY);
	}
	else {
		// buffer create failed, try unbuffered.
		if (bReversed)
			XPopupMenuItem::DrawGradient(lpdis->hDC, &rcIntersect, lpcol->m_clrBox, lpcol->m_clrLightBox, TRUE);
		else
			XPopupMenuItem::DrawGradient(lpdis->hDC, &rcIntersect, lpcol->m_clrLightBox, lpcol->m_clrBox, TRUE);
	}
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

COLORREF XPopupMenuItem::LightenColor( const UINT iScale, const COLORREF clrColor ) noexcept
{
	//const auto nScale = gsl::narrow_cast<int>(iScale);
	//const auto R = MulDiv(255 - GetRValue(clrColor), nScale, 255) + GetRValue(clrColor);
	//const auto G = MulDiv(255 - GetGValue((clrColor & 0xFFFF)), nScale, 255) + GetGValue((clrColor & 0xFFFF));
	//const auto B = MulDiv(255 - GetBValue(clrColor), nScale, 255) + GetBValue(clrColor);
	//
	//return RGB( R, G, B ); 

	const auto nScale = gsl::narrow_cast<int>(iScale);
	const auto R = Dcx::dcxMulDiv32(255 - GetRValue(clrColor), nScale, 255) + GetRValue(clrColor);
	const auto G = Dcx::dcxMulDiv32(255 - GetGValue((clrColor & 0xFFFF)), nScale, 255) + GetGValue((clrColor & 0xFFFF));
	const auto B = Dcx::dcxMulDiv32(255 - GetBValue(clrColor), nScale, 255) + GetBValue(clrColor);

	return RGB(R, G, B);
}

/*!
* \brief blah
*
* blah
*/

COLORREF XPopupMenuItem::DarkenColor( const UINT iScale, const COLORREF clrColor ) noexcept
{
	//const auto nScale = gsl::narrow_cast<int>(iScale);
	//const auto R = MulDiv(GetRValue(clrColor), (255 - nScale), 255);
	//const auto G = MulDiv(GetGValue((clrColor & 0xFFFF)), (255 - nScale), 255);
	//const auto B = MulDiv(GetBValue(clrColor), (255 - nScale), 255);
	//
	//return RGB( R, G, B ); 

	const auto nScale = gsl::narrow_cast<int>(iScale);
	const auto R = Dcx::dcxMulDiv32(GetRValue(clrColor), (255 - nScale), 255);
	const auto G = Dcx::dcxMulDiv32(GetGValue((clrColor & 0xFFFF)), (255 - nScale), 255);
	const auto B = Dcx::dcxMulDiv32(GetBValue(clrColor), (255 - nScale), 255);

	return RGB(R, G, B);
}

bool XPopupMenuItem::DrawMenuBitmap(const LPDRAWITEMSTRUCT lpdis, const bool bBigImage, const HBITMAP bmImage)
{
#if DCX_USE_WRAPPERS
	if ((lpdis == nullptr) || (bmImage == nullptr))
		return false;

	if (!bBigImage)
	{
		const Dcx::dcxHDCBitmapResource hdcbmp(lpdis->hDC, bmImage);

		if (BITMAP bmp{}; GetObject(bmImage, sizeof(BITMAP), &bmp) != 0)
		{
			const int oldMode = SetStretchBltMode(lpdis->hDC, STRETCH_HALFTONE);
			SetBrushOrgEx(lpdis->hDC, 0, 0, nullptr);
			StretchBlt(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right - lpdis->rcItem.left, lpdis->rcItem.bottom - lpdis->rcItem.top, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			SetStretchBltMode(lpdis->hDC, oldMode);
		}
	}
	else {
		BITMAP bm{};
		if (GetObject(static_cast<HBITMAP>(GetCurrentObject(lpdis->hDC, OBJ_BITMAP)), sizeof(BITMAP), &bm) == 0)
			return false;

		//RECT rcIntersect;
		//RECT rcBar;

		//// get the size of the whole menu.
		//if (SetRect(&rcBar, 0, 0, bm.bmWidth, bm.bmHeight) == FALSE)
		//	return false;

		//// get the rect of the box which will draw JUST the box (prevents redraw over items already done)
		//if (SetRect(&rcIntersect, rcBar.left, lpdis->rcItem.top, rcBar.right, lpdis->rcItem.bottom) == FALSE)
		//	return false;

		// get the size of the whole menu.
		RECT rcBar{ 0, 0, bm.bmWidth, bm.bmHeight };

		// get the rect of the box which will draw JUST the box (prevents redraw over items already done)
		RECT rcIntersect{ 0, lpdis->rcItem.top, rcBar.right, lpdis->rcItem.bottom };

		// set up a buffer to draw the whole whole menus background.
		const Dcx::dcxHDCBuffer hdcBuffer(lpdis->hDC, rcBar);

		// draw into the buffer
		const Dcx::dcxHDCBitmapResource hdcbmp(lpdis->hDC, bmImage);

		if (GetObject(bmImage, sizeof(BITMAP), &bm) != 0)
		{
			SetStretchBltMode(hdcBuffer, STRETCH_HALFTONE);
			SetBrushOrgEx(hdcBuffer, 0, 0, nullptr);
			StretchBlt(hdcBuffer, rcBar.left, rcBar.top, rcBar.right - rcBar.left, rcBar.bottom - rcBar.top, hdcbmp, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

			// test code to alpha blend the image (works)
			//auto p_Item = reinterpret_cast<XPopupMenuItem *>(lpdis->itemData);
			//auto hBrush = CreateSolidBrush(p_Item->m_pXParentMenu->getColor(XPopupMenu::MenuColours::XPMC_BACKGROUND));
			//if (hBrush != nullptr) {
			//	FillRect(hdcBuffer, &rcBar, hBrush);
			//	DeleteBrush(hBrush);
			//	// NB: atm we simply silently fail when we can't make a brush. (so drawing errors will occur when create fails)
			//}
			//BLENDFUNCTION blend{ AC_SRC_OVER, 0, 0x7f, 0 };
			//AlphaBlend(hdcBuffer, rcBar.left, rcBar.top, rcBar.right - rcBar.left, rcBar.bottom - rcBar.top, hdcbmp, 0, 0, bm.bmWidth, bm.bmHeight, blend);
		}
		// copy the box we want from the whole gradient bar
		BitBlt(lpdis->hDC, rcIntersect.left, rcIntersect.top, rcIntersect.right - rcIntersect.left, rcIntersect.bottom - rcIntersect.top, hdcBuffer, rcIntersect.left, rcIntersect.top, SRCCOPY);
	}
	return true;
#else
	if ((lpdis == nullptr) || (bmImage == nullptr))
		return false;

	if (!bBigImage)
	{
		const auto hdcbmp = CreateCompatibleDC(lpdis->hDC);

		if (hdcbmp == nullptr)
			return false;
		Auto(DeleteDC(hdcbmp));

		if (BITMAP bmp{}; GetObject(bmImage, sizeof(BITMAP), &bmp) != 0)
		{
			const auto hOldBm = SelectBitmap(hdcbmp, bmImage);
			Auto(SelectBitmap(hdcbmp, hOldBm));

			const auto oldMode = SetStretchBltMode(lpdis->hDC, STRETCH_HALFTONE);
			SetBrushOrgEx(lpdis->hDC, 0, 0, nullptr);
			StretchBlt(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right - lpdis->rcItem.left, lpdis->rcItem.bottom - lpdis->rcItem.top, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			SetStretchBltMode(lpdis->hDC, oldMode);
		}
	}
	else {
		BITMAP bm{};
		if (GetObject((HBITMAP)GetCurrentObject(lpdis->hDC, OBJ_BITMAP), sizeof(BITMAP), &bm) == 0)
			return false;

		//RECT rcIntersect;
		//RECT rcBar;

		//// get the size of the whole menu.
		//if (SetRect(&rcBar, 0, 0, bm.bmWidth, bm.bmHeight) == FALSE)
		//	return false;

		//// get the rect of the box which will draw JUST the box (prevents redraw over items already done)
		//if (SetRect(&rcIntersect, rcBar.left, lpdis->rcItem.top, rcBar.right, lpdis->rcItem.bottom) == FALSE)
		//	return false;

		// get the size of the whole menu.
		RECT rcBar{ 0, 0, bm.bmWidth, bm.bmHeight };

		// get the rect of the box which will draw JUST the box (prevents redraw over items already done)
		RECT rcIntersect{ 0, lpdis->rcItem.top, rcBar.right, lpdis->rcItem.bottom };

		// set up a buffer to draw the whole whole menus background.
		const auto hdcBuffer = CreateHDCBuffer(lpdis->hDC, &rcBar);

		if (hdcBuffer == nullptr)
			return false;
		Auto(DeleteHDCBuffer(hdcBuffer));

		// draw into the buffer
		auto hdcbmp = CreateCompatibleDC(lpdis->hDC);
		if (hdcbmp != nullptr)
		{
			Auto(DeleteDC(hdcbmp));

			BITMAP bmp{};

			if (GetObject(bmImage, sizeof(BITMAP), &bmp) != 0)
			{
				const auto hOldBm = SelectBitmap(hdcbmp, bmImage);
				Auto(SelectBitmap(hdcbmp, hOldBm));

				SetStretchBltMode(*hdcBuffer, STRETCH_HALFTONE);
				SetBrushOrgEx(*hdcBuffer, 0, 0, nullptr);

				StretchBlt(*hdcBuffer, rcBar.left, rcBar.top, rcBar.right - rcBar.left, rcBar.bottom - rcBar.top, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			}

			// copy the box we want from the whole gradient bar
			BitBlt(lpdis->hDC, rcIntersect.left, rcIntersect.top, rcIntersect.right - rcIntersect.left, rcIntersect.bottom - rcIntersect.top, *hdcBuffer, rcIntersect.left, rcIntersect.top, SRCCOPY);
		}
	}
	return true;
#endif
}
