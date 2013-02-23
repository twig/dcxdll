/*!
 * \file dcxstacker.cpp
 * \brief blah
 *
 * blah
 *
 * \author Ook
 * \version 1.0
 *
 * \b Revisions
 *	1.0
 *
 * © ScriptsDB.org - 2006
 */
#include "defines.h"
#include "Classes/dcxstacker.h"
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

DcxStacker::DcxStacker( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, const TString & styles )
: DcxControl( ID, p_Dialog )
, m_hActive(NULL)
, m_dStyles(0)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CONTROLPARENT,
		TEXT("ListBox"),
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
		Dcx::UXModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	SendMessage( this->m_Hwnd, CCM_SETVERSION, (WPARAM) 6, (LPARAM) 0 );

	//this->m_hBackBrush = GetSysColorBrush(COLOR_3DFACE);

	this->m_vImageList.clear();

	if (p_Dialog->getToolTip() != NULL) {
		if (styles.istok(TEXT("tooltips"))) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
	}

	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxStacker::~DcxStacker( ) {

	this->clearImageList();
	this->unregistreDefaultWindowProc( );
}

void DcxStacker::clearImageList(void)
{
#ifdef DCX_USE_GDIPLUS
	VectorOfImages::iterator itStart = this->m_vImageList.begin();
	VectorOfImages::iterator itEnd = this->m_vImageList.end();

	while (itStart != itEnd) {
		if (*itStart != NULL)
			delete (Image *)*itStart;
		itStart++;
	}
	this->m_vImageList.clear();
#endif
}

void DcxStacker::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{

	*Styles |= LBS_OWNERDRAWVARIABLE|LBS_NOTIFY;
	this->m_dStyles = STACKERS_COLLAPSE;

	for (TString tsStyle(styles.getfirsttok( 1 )); tsStyle != TEXT(""); tsStyle = styles.getnexttok( ))
	{
		if ( tsStyle == TEXT("vscroll") )
			*Styles |= WS_VSCROLL;
		else if ( tsStyle == TEXT("gradient") )
			this->m_dStyles |= STACKERS_GRAD;
		else if ( tsStyle == TEXT("arrows") )
			this->m_dStyles |= STACKERS_ARROW;
		else if ( tsStyle == TEXT("nocollapse") )
			this->m_dStyles &= ~STACKERS_COLLAPSE;
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

void DcxStacker::parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const {

	const UINT numtok = input.numtok( );
	const TString prop(input.getfirsttok( 3 ));

	// [NAME] [ID] [PROP] [N]
	if ( prop == TEXT("text") && numtok > 3 ) {
		const int nSel = input.getnexttok( ).to_int( ) - 1;	// tok 4

		if ( nSel > -1 && nSel < ListBox_GetCount( this->m_Hwnd ) ) {
			LPDCXSITEM sitem = this->getItem(nSel);
			if (sitem != NULL && sitem != (LPDCXSITEM)LB_ERR)
				lstrcpyn(szReturnValue,sitem->tsCaption.to_chr(), MIRC_BUFFER_SIZE_CCH);
			return;
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("num") ) {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), ListBox_GetCount( this->m_Hwnd ) );
		return;
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("sel") ) {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), ListBox_GetCurSel( this->m_Hwnd ) + 1 );
		return;
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("haschild") && numtok > 3 ) {
		const int nSel = input.getnexttok( ).to_int( ) - 1;	// tok 4

		lstrcpyn(szReturnValue,TEXT("$false"), MIRC_BUFFER_SIZE_CCH);

		if ( nSel > -1 && nSel < ListBox_GetCount( this->m_Hwnd ) ) {
			LPDCXSITEM sitem = this->getItem(nSel);
			if (sitem != NULL && sitem != (LPDCXSITEM)LB_ERR) {
				if (sitem->pChild != NULL)
					lstrcpyn(szReturnValue,TEXT("$true"), MIRC_BUFFER_SIZE_CCH);
			}
			return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("childid") && numtok > 3 ) {
		const int nSel = input.getnexttok( ).to_int( ) - 1;	// tok 4

		lstrcpyn(szReturnValue,TEXT("0"), MIRC_BUFFER_SIZE_CCH);

		if ( nSel > -1 && nSel < ListBox_GetCount( this->m_Hwnd ) ) {
			LPDCXSITEM sitem = this->getItem(nSel);
			if (sitem != NULL && sitem != (LPDCXSITEM)LB_ERR) {
				if (sitem->pChild != NULL)
					wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"),sitem->pChild->getUserID());
			}
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

void DcxStacker::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));

	const UINT numtok = input.numtok( );

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')]) {
		SendMessage(this->m_Hwnd, LB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0);
	}

	//xdid -a -> [NAME] [ID] -a [N] [+FLAGS] [IMAGE] [SIMAGE] [COLOR] [BGCOLOR] Item Text [TAB] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if (flags[TEXT('a')] && numtok > 9) {

		LPDCXSITEM sitem = new DCXSITEM;

		if (sitem == NULL) {
			this->showError(NULL, TEXT("-a"), TEXT("Error adding item to control: No Memory"));
			return;
		}
		const TString item(input.getfirsttok(1,TSTAB).trim());
		const TString ctrl(input.getnexttok(TSTAB).trim());			// tok 2

		int nPos = item.getfirsttok( 4 ).to_int( ) - 1;
		const TString flag(item.getnexttok( ).trim());				// tok 5	?? flag never used ??
		sitem->iItemImg = item.getnexttok( ).to_int() -1;			// tok 6
		sitem->iSelectedItemImg = item.getnexttok( ).to_int() -1;	// tok 7
		sitem->clrText = (COLORREF)item.getnexttok( ).to_num();		// tok 8
		sitem->clrBack = (COLORREF)item.getnexttok( ).to_num();		// tok 9
		sitem->pChild = NULL;
		sitem->hFont = NULL;
		sitem->tsCaption = item.gettok(10,-1);

		if ( nPos < 0 )
			nPos = ListBox_GetCount( this->m_Hwnd );
		if (nPos == LB_ERR)
			nPos = 0;

		if (ctrl.len() > 0) {
			const UINT ID = mIRC_ID_OFFSET + (UINT)ctrl.gettok( 1 ).to_int( );

			if ( (ID > mIRC_ID_OFFSET - 1) && !IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && (this->m_pParentDialog->getControlByID( ID ) == NULL) )
			{
				try {
					DcxControl * p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,ctrl,2,CTLF_ALLOW_ALL,this->m_Hwnd);

					if ( p_Control != NULL ) {
						this->m_pParentDialog->addControl( p_Control );
						sitem->pChild = p_Control;
						ShowWindow(p_Control->getHwnd(),SW_HIDE);
						this->redrawWindow( );
					}
				}
				catch ( TCHAR *err ) {
					this->showErrorEx(NULL, TEXT("-a"), TEXT("Unable To Create Control %d (%s)"), ID - mIRC_ID_OFFSET, err);
					delete sitem;
					return;
				}
			}
			else {
				this->showErrorEx(NULL, TEXT("-a"), TEXT("Control with ID \"%d\" already exists"), ID - mIRC_ID_OFFSET );
				delete sitem;
				return;
			}
		}
		//ListBox_InsertString(this->m_Hwnd, nPos, (LPARAM)sitem);
		if (SendMessage(this->m_Hwnd,LB_INSERTSTRING,nPos,(LPARAM)sitem) < 0) {
			delete sitem;
			this->showError(NULL, TEXT("-a"), TEXT("Error adding item to control"));
			return;
		}
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('c')] && numtok > 3) {
		const int nPos = input.getnexttok( ).to_int( ) - 1;		// tok 4

		if ( nPos > -1 && nPos < ListBox_GetCount( this->m_Hwnd ) )
			SendMessage(this->m_Hwnd,LB_SETCURSEL,nPos,NULL);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')] && (numtok > 3)) {
		const int nPos = input.getnexttok( ).to_int( ) - 1;	// tok 4

		if ( nPos > -1 && nPos < ListBox_GetCount( this->m_Hwnd ) )
			ListBox_DeleteString( this->m_Hwnd, nPos );
	}
	// This is to avoid an invalid flag message.
	//xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
	}
	//xdid -u [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('u')] ) {
		ListBox_SetCurSel( this->m_Hwnd, -1 );
	}
	// xdid -T [NAME] [ID] [SWITCH] [N] (ToolTipText)
	else if (flags[TEXT('T')] && numtok > 3) {
		const int nPos = input.getnexttok( ).to_int( ) - 1;	// tok 4

		if ( nPos > -1 && nPos < ListBox_GetCount( this->m_Hwnd ) ) {
			LPDCXSITEM sitem = this->getItem(nPos);
			if (sitem != NULL && sitem != (LPDCXSITEM)LB_ERR) {
				sitem->tsTipText = (numtok > 4 ? input.gettok(5, -1).trim() : TEXT(""));
			}
		}
	}
	//xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [FILE]
	else if ( flags[TEXT('w')] && (numtok > 4)) {
#ifdef DCX_USE_GDIPLUS

		const TString flag(input.getnexttok( ));		// tok 4
		TString filename(input.getnexttok( ).trim());	// tok 5

		if (!IsFile(filename)) {
			this->showErrorEx(NULL, TEXT("-w"), TEXT("Unable to Access File: %s"), filename.to_chr());
			return;
		}
		this->m_vImageList.push_back(new Image(filename.to_chr()));
#endif
	}
	//xdid -y [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('y')] ) {
		this->clearImageList();
		this->redrawWindow();
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

int DcxStacker::getItemID(void) const {
	POINT pt;
	GetCursorPos( &pt );
	MapWindowPoints(NULL, this->m_Hwnd, &pt, 1);
	return (int)(LOWORD((DWORD)SendMessage(this->m_Hwnd,LB_ITEMFROMPOINT,NULL,MAKELPARAM(pt.x,pt.y))) +1);
}

int DcxStacker::getSelItemID(void) const {
	return (int)(LOWORD((DWORD)SendMessage(this->m_Hwnd,LB_GETCURSEL,NULL,NULL)) +1);
}

DWORD DcxStacker::getItemCount(void) const {
	return (DWORD)ListBox_GetCount(this->m_Hwnd);
}

LPDCXSITEM DcxStacker::getItem(const int nPos) const {
	return (LPDCXSITEM)SendMessage(this->m_Hwnd,LB_GETITEMDATA,nPos,NULL);
}

LPDCXSITEM DcxStacker::getHotItem(void) const {
	return (LPDCXSITEM)SendMessage(this->m_Hwnd,LB_GETITEMDATA,this->getItemID()-1,NULL);
}

void DcxStacker::getItemRect(const int nPos, LPRECT rc) const {
	SendMessage(this->m_Hwnd,LB_GETITEMRECT,(WPARAM)nPos,(LPARAM)rc);
}

TString DcxStacker::getStyles(void) const {
	TString styles(__super::getStyles());
	const DWORD Styles = GetWindowStyle(this->m_Hwnd);
	if (Styles & WS_VSCROLL)
		styles.addtok(TEXT("vscroll"));
	if (this->m_dStyles & STACKERS_GRAD)
		styles.addtok(TEXT("gradient"));
	if (this->m_dStyles & STACKERS_ARROW)
		styles.addtok(TEXT("arrows"));
	if (~this->m_dStyles & STACKERS_COLLAPSE)
		styles.addtok(TEXT("nocollapse"));
	return styles;
}

void DcxStacker::DrawAliasedTriangle(const HDC hdc, const LPRECT rc, const COLORREF clrShape)
{
#ifdef DCX_USE_GDIPLUS
	if (!Dcx::GDIModule.isUseable() || hdc == NULL || rc == NULL)
		return;

	Graphics gfx( hdc );

	gfx.SetSmoothingMode(SmoothingModeAntiAlias);
	gfx.SetCompositingMode(CompositingModeSourceOver);
	//gfx.SetCompositingQuality
	SolidBrush blackBrush(Color(128, GetRValue(clrShape), GetGValue(clrShape), GetBValue(clrShape)));
	// Create an array of Point objects that define the polygon.
	//Point point1(rc->left, rc->top);
	//Point point2(rc->right, rc->top);
	//Point point3(rc->left + (rc->right - rc->left)/2, rc->bottom);
	//Point points[3] = {point1, point2, point3};
	Point points[3];
	points[0].X = rc->left;
	points[0].Y = rc->top;
	points[1].X = rc->right;
	points[1].Y = rc->top;
	points[2].X = (rc->left + (rc->right - rc->left)/2);
	points[2].Y = rc->bottom;
	// Fill the polygon.
	gfx.FillPolygon(&blackBrush, points, 3);
#endif
}

void DcxStacker::DrawItemImage(const HDC hdc, Image *img, const LPRECT rc)
{
#ifdef DCX_USE_GDIPLUS
	if (!Dcx::GDIModule.isUseable() || img == NULL || rc == NULL || hdc == NULL)
		return;

	Graphics grphx( hdc );
	grphx.SetSmoothingMode(SmoothingModeAntiAlias);
	grphx.SetCompositingMode(CompositingModeSourceOver);
	const int w = (rc->right - rc->left), h = (rc->bottom - rc->top);
	if ((img->GetWidth() == 1) || (img->GetHeight() == 1)) {
		// This fixes a GDI+ bug when resizing 1 px images
		// http://www.devnewsgroups.net/group/microsoft.public.dotnet.framework.windowsforms/topic11515.aspx
		grphx.SetInterpolationMode(InterpolationModeNearestNeighbor);
		grphx.SetPixelOffsetMode(PixelOffsetModeHalf);
	}
	grphx.DrawImage( img, rc->left, rc->top, w, h );
#endif
}

void DcxStacker::DrawSItem(const LPDRAWITEMSTRUCT idata)
{
	if (idata == NULL || idata->itemID == -1)
		return;
	LPDCXSITEM sitem = (LPDCXSITEM)idata->itemData;
	if (sitem == NULL)
		return;
	RECT rcWin;
	GetClientRect(idata->hwndItem, &rcWin);
	// Create temp HDC as drawing buffer.
	// if temp HDC or its bitmap fails to create, use supplied HDC without buffer.
	HDC *hBuffer = CreateHDCBuffer(idata->hDC, &rcWin);
	HDC memDC;
	if (hBuffer != NULL)
		memDC = *hBuffer;
	else
		memDC = idata->hDC;

	HFONT hFont = sitem->hFont;
	LOGFONT lf;
	RECT rcText = idata->rcItem;
	int h = MIN_STACK_HEIGHT, button_base = 0;
	bool Redraw = false;

	if (hFont == NULL)
		hFont = GetWindowFont(idata->hwndItem);

	GetObject(hFont, sizeof(LOGFONT), &lf);

	h = max(h,lf.lfHeight * 20);
	rcText.bottom = rcText.top + h;
	//rcText.right -= 1;
	if (rcText.bottom > idata->rcItem.bottom)
		rcText.bottom = idata->rcItem.bottom;
	button_base = rcText.bottom;

	// draw button for this item.
	UINT style = DFCS_BUTTONPUSH|DFCS_ADJUSTRECT;
	if (idata->itemState & ODS_DISABLED)
		style |= DFCS_INACTIVE;
	if (idata->itemState & ODS_SELECTED)
		style |= DFCS_PUSHED;
	//if (idata->itemState & ODS_HOTLIGHT)
	//	style |= DFCS_HOT;

	DrawFrameControl(memDC,&rcText,DFC_BUTTON,style);
	rcText.right -= GetSystemMetrics(SM_CXEDGE); // move in right side past border

	// fill background colour if any.
	if (this->m_dStyles & STACKERS_GRAD) {
		COLORREF clrStart = this->m_clrStartGradient;
		COLORREF clrEnd = this->m_clrEndGradient;

		if (clrEnd == CLR_INVALID)
			clrEnd = GetSysColor(COLOR_GRADIENTACTIVECAPTION);
		if (clrStart == CLR_INVALID)
			clrStart = sitem->clrBack;

		if (clrStart == CLR_INVALID)
			clrStart = GetSysColor(COLOR_BTNFACE);

		XPopupMenuItem::DrawGradient( memDC, &rcText, clrStart, clrEnd, FALSE);
	}
	else if (sitem->clrBack != CLR_INVALID) {
		SetBkColor(memDC,sitem->clrBack);
		ExtTextOut(memDC, rcText.left, rcText.top, ETO_CLIPPED | ETO_OPAQUE, &rcText, TEXT(""), NULL, NULL );
	}

	// draw GDI+ image if any, we draw the image after the  colour fill to allow for alpha in pics.
	if (idata->itemState & ODS_SELECTED && sitem->iSelectedItemImg > -1 && sitem->iSelectedItemImg < (int)this->m_vImageList.size()) {
		this->DrawItemImage(memDC, this->m_vImageList[sitem->iSelectedItemImg], &rcText);
	}
	else if (sitem->iItemImg > -1 && sitem->iItemImg < (int)this->m_vImageList.size()) {
		this->DrawItemImage(memDC, this->m_vImageList[sitem->iItemImg], &rcText);
	}

	// draw text if any
	if (sitem->tsCaption.len()) {
		HFONT oldFont = SelectFont(memDC,hFont);
		// get text colour.
		COLORREF clrText = sitem->clrText, oldClr = CLR_INVALID;
		if (clrText == CLR_INVALID)
			clrText = GetSysColor(COLOR_BTNTEXT);
		// draw the text
		if (clrText != CLR_INVALID)
			oldClr = SetTextColor(memDC,clrText);

		this->ctrlDrawText(memDC, sitem->tsCaption, &rcText, DT_CENTER | DT_END_ELLIPSIS);

		if (oldClr != CLR_INVALID)
			SetTextColor(memDC,oldClr);
		//COLORREF clrText = sitem->clrText;
		//if (clrText == CLR_INVALID)
		//	clrText = GetSysColor(COLOR_BTNTEXT);
		// draw the text
		//if (!this->m_bCtrlCodeText) {
		//	SetBkMode(memDC,TRANSPARENT);
		//	if (this->m_bShadowText)
		//		dcxDrawShadowText(memDC,sitem->tsCaption.to_chr(), sitem->tsCaption.len(),&rcText, DT_END_ELLIPSIS | DT_CENTER, clrText, 0, 5, 5);
		//	else {
		//		if (clrText != CLR_INVALID)
		//			SetTextColor(memDC,clrText);
		//		DrawTextW(memDC, sitem->tsCaption.to_chr(), sitem->tsCaption.len(), &rcText, DT_CENTER | DT_END_ELLIPSIS);
		//	}
		//}
		//else
		//	mIRC_DrawText(memDC, sitem->tsCaption, &rcText, DT_CENTER | DT_END_ELLIPSIS, this->m_bShadowText);

		SelectFont(memDC,oldFont);
	}
	// draw arrows if wanted.
	if (this->m_dStyles & STACKERS_ARROW) {
		RECT rcArrow = rcText;
		rcArrow.left = rcArrow.right - 10;
		rcArrow.top += 3;
		rcArrow.bottom -= 3;
		DrawAliasedTriangle(memDC,&rcArrow,0);
	}
	if (idata->hDC != memDC) { // if using temp buffer HDC, render completed item to main HDC & cleanup buffer.
		BitBlt(idata->hDC,idata->rcItem.left, idata->rcItem.top, (idata->rcItem.right - idata->rcItem.left), h, memDC, idata->rcItem.left, idata->rcItem.top, SRCCOPY);
		DeleteHDCBuffer(hBuffer);
	}

	// position child control if any.
	if (sitem->pChild != NULL) {
		HWND sWnd = sitem->pChild->getHwnd();
		if (IsWindow(sWnd)) {
			if (idata->itemState & ODS_SELECTED) {
				RECT rc;
				GetWindowRect(sWnd,&rc);
				SetWindowPos(sWnd,NULL,idata->rcItem.left, button_base,(idata->rcItem.right - idata->rcItem.left),(rc.bottom - rc.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);
				h += (rc.bottom - rc.top);
				if (sWnd != this->m_hActive) {
					if (IsWindow(this->m_hActive))
						Redraw = true;
				}
				this->m_hActive = sWnd;
			}
			else
				SetWindowPos(sWnd,NULL,0,0,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW);
		}
	}
	else if (IsWindow(this->m_hActive) && idata->itemState & ODS_SELECTED) {
		this->m_hActive = NULL;
		Redraw = true;
	}
	if (h != (idata->rcItem.bottom - idata->rcItem.top)) {
		SendMessage(idata->hwndItem,LB_SETITEMHEIGHT,idata->itemID,h);
		//MEASUREITEMSTRUCT mi;
		//SCROLLINFO sc;
		//UINT scroll_height = 0, cnt, items = this->getItemCount(), minitemheight = -1;
		//BOOL bParsed = FALSE;
		//cnt = items;
		//mi.CtlID = this->getID();
		//mi.CtlType = ODT_LISTBOX;
		//while (cnt > 0) {
		//	mi.itemID = cnt-1;
		//	mi.itemData = (ULONG_PTR)this->getItem(cnt-1);
		//	this->ParentMessage(WM_MEASUREITEM, (WPARAM)mi.CtlID, (LPARAM)&mi, bParsed);
		//	if (bParsed) {
		//		if (mi.itemHeight < minitemheight)
		//			minitemheight = mi.itemHeight;
		//		scroll_height += mi.itemHeight;
		//		bParsed = FALSE;
		//	}
		//	cnt--;
		//}
		//if (minitemheight == 0)
		//	minitemheight = 1;

		//ZeroMemory(&sc, sizeof(sc));
		//sc.cbSize = sizeof(sc);
		//sc.fMask = SIF_RANGE;
		//sc.nMin = 0;
		//sc.nMax = items + (h / minitemheight) -1;
		//SetScrollInfo(this->m_Hwnd, SB_VERT, &sc, FALSE);
		Redraw = true;
	}
	if (Redraw)
		this->redrawWindow();
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxStacker::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_COMMAND:
		{
			bParsed = TRUE;
			switch (HIWORD(wParam))
			{
			case LBN_DBLCLK:
				{
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
						this->execAliasEx(TEXT("%s,%d,%d"), TEXT("dclick"), this->getUserID( ), this->getSelItemID());
				}
				break;
			case LBN_SELCHANGE:
				{
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
						this->execAliasEx(TEXT("%s,%d,%d"), TEXT("sclick"), this->getUserID( ), this->getSelItemID());
				}
				break;
			}
		}
		break;
	case WM_COMPAREITEM:
		{
			bParsed = TRUE;
			//LPCOMPAREITEMSTRUCT idata = (LPCOMPAREITEMSTRUCT)lParam;
		}
		break;

	case WM_DELETEITEM:
		{
			DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
			LPDCXSITEM sitem = (LPDCXSITEM)idata->itemData;
			if (sitem == NULL)
				break;
			bParsed = TRUE;
			lRes = TRUE;
			if (sitem->pChild != NULL) {
				if ( sitem->pChild->getType( ) == TEXT("dialog") || sitem->pChild->getType( ) == TEXT("window") )
					delete sitem->pChild;
				else { //if ( sitem->pChild->getRefCount( ) == 0 ) {
					this->m_pParentDialog->deleteControl( sitem->pChild ); // remove from internal list!
					if (IsWindow(sitem->pChild->getHwnd()))
						DestroyWindow( sitem->pChild->getHwnd() );
				}
			}
			delete sitem;
		}
		break;

	case WM_MEASUREITEM:
		{
			LPMEASUREITEMSTRUCT idata = (LPMEASUREITEMSTRUCT) lParam;
			LPDCXSITEM sitem = (LPDCXSITEM)idata->itemData;
			if (sitem == NULL)
				break;
			bParsed = TRUE;
			lRes = TRUE;
			HFONT hFont = sitem->hFont;
			LOGFONT lf;
			RECT rcText;
			GetClientRect(this->m_Hwnd,&rcText);
			int h = MIN_STACK_HEIGHT, w = (rcText.right - rcText.left);

			if (hFont == NULL)
				hFont = GetWindowFont(this->m_Hwnd);

			GetObject(hFont, sizeof(LOGFONT), &lf);

			h = max(h,lf.lfHeight * 20);

			if (sitem->pChild != NULL) {
				HWND win = sitem->pChild->getHwnd();
				if (IsWindow(win) && IsWindowVisible(win)) {
					RECT rc;
					GetWindowRect(win,&rc);
					h += (rc.bottom - rc.top);
					//w = max(w,(rc.right - rc.left));
				}
			}
			idata->itemHeight = h;
			idata->itemWidth = w;
		}
		break;

	case WM_DRAWITEM:
		{
			bParsed = TRUE;
			lRes = TRUE;
			DrawSItem((LPDRAWITEMSTRUCT)lParam);
		}
		break;
	}
	return lRes;
}

LRESULT DcxStacker::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
	switch( uMsg )
	{
	case WM_GETDLGCODE:
		{
			bParsed = TRUE;
			return DLGC_WANTALLKEYS;
		}
		break;

	case WM_HSCROLL:
	case WM_VSCROLL:
		//{
		//	if (((HWND) lParam == NULL) && this->m_bAlphaBlend) {
		//		bParsed = TRUE;
		//		//SendMessage(this->m_Hwnd,WM_SETREDRAW,FALSE,NULL);
		//		LRESULT res = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, wParam, lParam );
		//		//SendMessage(this->m_Hwnd,WM_SETREDRAW,TRUE,NULL);
		//		//InvalidateRect(this->m_Hwnd, NULL, FALSE);
		//		//this->redrawWindow();
		//		//RECT rc;
		//		//HWND hParent = GetParent(this->m_Hwnd);
		//		//GetWindowRect(this->m_Hwnd, &rc);
		//		//MapWindowPoints(NULL, hParent, (LPPOINT)&rc, 2);
		//		//InvalidateRect(hParent, &rc, FALSE);
		//		//RedrawWindow(this->m_Hwnd,0,0,RDW_INVALIDATE|RDW_ALLCHILDREN|RDW_NOERASE);
		//		//RedrawWindow(this->m_Hwnd,0,0,RDW_INTERNALPAINT|RDW_FRAME|RDW_INVALIDATE|RDW_ALLCHILDREN);
		//		return res;
		//	}
		//}

	case WM_COMMAND:
		{
			if (IsWindow((HWND) lParam)) {
				DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,TEXT("dcx_cthis"));
				if (c_this != NULL)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_COMPAREITEM:
		{
			LPCOMPAREITEMSTRUCT idata = (LPCOMPAREITEMSTRUCT)lParam;
			if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
				DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
				if (c_this != NULL)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_DELETEITEM:
		{
			DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
			if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
				DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
				if (c_this != NULL)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_MEASUREITEM:
		{
			HWND cHwnd = GetDlgItem(this->m_Hwnd, wParam);
			if (IsWindow(cHwnd)) {
				DcxControl *c_this = (DcxControl *) GetProp(cHwnd,TEXT("dcx_cthis"));
				if (c_this != NULL)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
			if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
				DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
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

			if (hdr->hwndFrom == this->m_ToolTipHWND) {
				switch( hdr->code )
				{
					case TTN_GETDISPINFO:
						{
							LPNMTTDISPINFO di = (LPNMTTDISPINFO)lParam;
							LPDCXSITEM sitem = this->getHotItem();
							if (sitem != NULL && sitem != (LPDCXSITEM)LB_ERR) {
								di->lpszText = sitem->tsTipText.to_chr();
								di->hinst = NULL;
							}
							bParsed = TRUE;
						}
						break;
					case TTN_LINKCLICK:
						{
							bParsed = TRUE;
							this->execAliasEx(TEXT("%s,%d,%d"), TEXT("tooltiplink"), this->getUserID( ), this->getItemID() );
						}
						break;
				}
			}
			else if (this->m_Hwnd != hdr->hwndFrom)
			{
				if (IsWindow(hdr->hwndFrom)) {
					DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,TEXT("dcx_cthis"));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
		}
		break;

	case WM_LBUTTONDBLCLK:
		break;

	case WM_LBUTTONUP:
		{
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				this->execAliasEx(TEXT("%s,%d,%d"), TEXT("lbup"), this->getUserID( ), this->getItemID());
		}
		break;

	case WM_ENABLE:
		{
			this->redrawWindow();
		}
		break;

	case WM_ERASEBKGND:
		{
			bParsed = TRUE;
			// fill background.
			DWORD lastID = this->getItemCount();
			if (lastID == LB_ERR)
				break;
			lastID--;
			if ((lastID == 0) || (lParam != NULL))
				this->DrawParentsBackground((HDC)wParam);
			else {
				RECT rcItem, rcClient;
				GetClientRect(this->m_Hwnd,&rcClient);
				this->getItemRect(lastID, &rcItem);
				if (rcItem.bottom < rcClient.bottom) {
					rcClient.top = rcItem.bottom;
					this->DrawParentsBackground((HDC) wParam, &rcClient);
				}
				//lastID = this->getSelItemID();
				//if (lastID != LB_ERR) {
				//	this->getItemRect(lastID, &rcItem);
				//	this->DrawParentsBackground((HDC) wParam, &rcItem);
				//}
			}
			return TRUE;
		}
		break;

	case WM_PAINT:
		{
			if (!this->m_bAlphaBlend)
				break;
			PAINTSTRUCT ps;
			HDC hdc;

			hdc = BeginPaint( this->m_Hwnd, &ps );

			bParsed = TRUE;

			// Setup alpha blend if any.
			LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

			lRes = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

			this->FinishAlphaBlend(ai);

			EndPaint( this->m_Hwnd, &ps );
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
