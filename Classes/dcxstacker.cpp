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

DcxStacker::DcxStacker( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles )
	: DcxControl(ID, p_Dialog)
	, m_hActive(nullptr)
	, m_dStyles(0)
#ifdef DCX_USE_GDIPLUS
	, m_vImageList()
#endif
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CONTROLPARENT,
		TEXT("ListBox"),
		nullptr,
		WS_CHILD | Styles, 
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(nullptr), 
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( m_Hwnd , L" ", L" " );

	SendMessage( m_Hwnd, CCM_SETVERSION, (WPARAM) 6, (LPARAM) 0 );

	//this->m_hBackBrush = GetSysColorBrush(COLOR_3DFACE);

	if (p_Dialog->getToolTip() != nullptr) {
		if (styles.istok(TEXT("tooltips"))) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, m_Hwnd);
		}
	}

	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
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
	for (auto &x: this->m_vImageList)
		delete x;

	this->m_vImageList.clear();
#endif
}

void DcxStacker::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{

	*Styles |= LBS_OWNERDRAWVARIABLE|LBS_NOTIFY;
	this->m_dStyles = STACKERS_COLLAPSE;

	for (const auto &tsStyle: styles)
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

void DcxStacker::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const {

	const auto numtok = input.numtok();
	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP] [N]
	if ( prop == TEXT("text") && numtok > 3 ) {
		const auto nSel = input.getnexttok().to_int() - 1;	// tok 4

		if (nSel < 0 && nSel >= ListBox_GetCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		auto sitem = this->getItem(nSel);
		if (sitem == nullptr || sitem == (LPDCXSITEM)LB_ERR)
			throw Dcx::dcxException("Unable to get Item");

		dcx_strcpyn(szReturnValue,sitem->tsCaption.to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("num") ) {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), ListBox_GetCount( m_Hwnd ) );
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("sel") ) {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), ListBox_GetCurSel( m_Hwnd ) + 1 );
	}
	// [NAME] [ID] [PROP] [N]
	else if ( prop == TEXT("haschild") && numtok > 3 ) {
		//const auto nSel = input.getnexttok().to_int() - 1;	// tok 4

		//dcx_strcpyn(szReturnValue,TEXT("$false"), MIRC_BUFFER_SIZE_CCH);

		//if (nSel < 0 && nSel >= ListBox_GetCount(m_Hwnd))
		//	throw Dcx::dcxException("Invalid Item");

		//auto sitem = this->getItem(nSel);
		//if (sitem == nullptr || sitem == (LPDCXSITEM)LB_ERR)
		//	throw Dcx::dcxException("Unable to get Item");
		//
		//if (sitem->pChild != nullptr)
		//	dcx_strcpyn(szReturnValue,TEXT("$true"), MIRC_BUFFER_SIZE_CCH);

		const auto nSel = input.getnexttok().to_int() - 1;	// tok 4

		if (nSel < 0 && nSel >= ListBox_GetCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		auto sitem = this->getItem(nSel);
		if (sitem == nullptr || sitem == (LPDCXSITEM)LB_ERR)
			throw Dcx::dcxException("Unable to get Item");

		dcx_strcpyn(szReturnValue, dcx_truefalse(sitem->pChild != nullptr), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP] [N]
	else if (prop == TEXT("childid") && numtok > 3) {
		const auto nSel = input.getnexttok().to_int() - 1;	// tok 4

		if (nSel < 0 && nSel >= ListBox_GetCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");

		auto sitem = this->getItem(nSel);
		if (sitem == nullptr || sitem == (LPDCXSITEM)LB_ERR)
			throw Dcx::dcxException("Unable to get Item");

		if (sitem->pChild != nullptr)
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), sitem->pChild->getUserID());
		else
			dcx_strcpyn(szReturnValue, TEXT("0"), MIRC_BUFFER_SIZE_CCH);
	}
	else
		parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStacker::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));

	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
		SendMessage(m_Hwnd, LB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0);

	//xdid -a -> [NAME] [ID] -a [N] [+FLAGS] [IMAGE] [SIMAGE] [COLOR] [BGCOLOR] Item Text [TAB] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if (flags[TEXT('a')] && numtok > 9) {

		auto sitem = std::make_unique<DCXSITEM>();

		const auto item(input.getfirsttok(1, TSTABCHAR).trim());		// tok 1, TSTAB
		const auto ctrl(input.getnexttok(TSTABCHAR).trim());			// tok 2, TSTAB

		auto nPos = item.getfirsttok(4).to_int() - 1;				// tok 4
		const auto flag(item.getnexttok().trim());				// tok 5	?? flag never used ??

		sitem->iItemImg = item.getnexttok( ).to_int() -1;			// tok 6
		sitem->iSelectedItemImg = item.getnexttok( ).to_int() -1;	// tok 7
		sitem->clrText = item.getnexttok().to_<COLORREF>();		// tok 8
		sitem->clrBack = item.getnexttok().to_<COLORREF>();		// tok 9
		sitem->pChild = nullptr;
		sitem->hFont = nullptr;
		sitem->tsCaption = item.getlasttoks();						// tok 10, -1

		if ( nPos < 0 )
			nPos = ListBox_GetCount( m_Hwnd );
		if (nPos == LB_ERR)
			nPos = 0;

		if (!ctrl.empty()) {

			auto p_Control = this->m_pParentDialog->addControl(ctrl, 1, CTLF_ALLOW_ALL, m_Hwnd);
			sitem->pChild = p_Control;
			ShowWindow(p_Control->getHwnd(),SW_HIDE);
			this->redrawWindow( );

			//const auto ID = mIRC_ID_OFFSET + (UINT)ctrl.gettok( 1 ).to_int( );
			//
			//if (!this->m_pParentDialog->isIDValid(ID, true))
			//	throw Dcx::dcxException(TEXT("Control with ID \"%\" already exists"), ID - mIRC_ID_OFFSET);
			//
			//try {
			//	auto p_Control = DcxControl::controlFactory(this->m_pParentDialog, ID, ctrl, 2, CTLF_ALLOW_ALL, m_Hwnd);
			//
			//	this->m_pParentDialog->addControl( p_Control );
			//	sitem->pChild = p_Control;
			//	ShowWindow(p_Control->getHwnd(),SW_HIDE);
			//	this->redrawWindow( );
			//}
			//catch (std::exception &e) {
			//	this->showErrorEx(nullptr, TEXT("-a"), TEXT("Unable To Create Control %d (%S)"), ID - mIRC_ID_OFFSET, e.what());
			//	throw;
			//}
		}
		//ListBox_InsertString(m_Hwnd, nPos, (LPARAM)sitem);
		if (SendMessage(m_Hwnd,LB_INSERTSTRING,nPos,(LPARAM)sitem.get()) < 0)
			throw Dcx::dcxException("Error adding item to control");

		sitem.release();
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('c')] && numtok > 3) {
		const auto nPos = input.getnexttok().to_int() - 1;		// tok 4

		if (nPos < 0 && nPos >= ListBox_GetCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");
		
		SendMessage(m_Hwnd, LB_SETCURSEL, nPos, NULL);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')] && (numtok > 3)) {
		const auto nPos = input.getnexttok().to_int() - 1;	// tok 4

		if (nPos < 0 && nPos >= ListBox_GetCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");
		
		ListBox_DeleteString(m_Hwnd, nPos);
	}
	// This is to avoid an invalid flag message.
	//xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
	}
	//xdid -u [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('u')] ) {
		ListBox_SetCurSel( m_Hwnd, -1 );
	}
	// xdid -T [NAME] [ID] [SWITCH] [N] (ToolTipText)
	else if (flags[TEXT('T')] && numtok > 3) {
		const auto nPos = input.getnexttok().to_int() - 1;	// tok 4

		if (nPos < 0 && nPos >= ListBox_GetCount(m_Hwnd))
			throw Dcx::dcxException("Invalid Item");
		
		auto sitem = this->getItem(nPos);
		if (sitem == nullptr || sitem == (LPDCXSITEM)LB_ERR)
			throw Dcx::dcxException("Unable to get Item");

		sitem->tsTipText = (numtok > 4 ? input.getlasttoks().trim() : TEXT(""));	// tok 5, -1
	}
	//xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [FILE]
	else if ( flags[TEXT('w')] && (numtok > 4)) {
#ifdef DCX_USE_GDIPLUS

		const auto flag(input.getnexttok());		// tok 4
		auto filename(input.getnexttok().trim());	// tok 5

		if (!IsFile(filename))
			throw Dcx::dcxException(TEXT("Unable to Access File: %"), filename);

		m_vImageList.push_back(new Gdiplus::Image(filename.to_wchr()));
#endif
	}
	//xdid -y [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('y')] ) {
		clearImageList();
		redrawWindow();
	}
	else
		parseGlobalCommandRequest(input, flags);
}

int DcxStacker::getItemID(void) const {
	POINT pt;
	if (!GetCursorPos(&pt))
		return -1;

	MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
	return (int)(LOWORD((DWORD)SendMessage(m_Hwnd,LB_ITEMFROMPOINT,NULL,MAKELPARAM(pt.x,pt.y))) +1);
}

int DcxStacker::getSelItemID(void) const {
	return (int)(LOWORD((DWORD)SendMessage(m_Hwnd,LB_GETCURSEL,NULL,NULL)) +1);
}

DWORD DcxStacker::getItemCount(void) const {
	return (DWORD)ListBox_GetCount(m_Hwnd);
}

LPDCXSITEM DcxStacker::getItem(const int nPos) const {
	return (LPDCXSITEM)SendMessage(m_Hwnd,LB_GETITEMDATA,nPos,NULL);
}

LPDCXSITEM DcxStacker::getHotItem(void) const {
	return (LPDCXSITEM)SendMessage(m_Hwnd,LB_GETITEMDATA,this->getItemID()-1,NULL);
}

void DcxStacker::getItemRect(const int nPos, LPRECT rc) const {
	SendMessage(m_Hwnd,LB_GETITEMRECT,(WPARAM)nPos,(LPARAM)rc);
}

const TString DcxStacker::getStyles(void) const {
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, WS_VSCROLL))
		styles.addtok(TEXT("vscroll"));
	if (dcx_testflag(this->m_dStyles, STACKERS_GRAD))
		styles.addtok(TEXT("gradient"));
	if (dcx_testflag(this->m_dStyles, STACKERS_ARROW))
		styles.addtok(TEXT("arrows"));
	if (!dcx_testflag(this->m_dStyles, STACKERS_COLLAPSE))
		styles.addtok(TEXT("nocollapse"));
	return styles;
}

void DcxStacker::DrawAliasedTriangle(const HDC hdc, const LPRECT rc, const COLORREF clrShape)
{
#ifdef DCX_USE_GDIPLUS
	if (!Dcx::GDIModule.isUseable() || hdc == nullptr || rc == nullptr)
		return;

	Gdiplus::Graphics gfx( hdc );

	gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	gfx.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
	//gfx.SetCompositingQuality
	Gdiplus::SolidBrush blackBrush(Gdiplus::Color(128, GetRValue(clrShape), GetGValue(clrShape), GetBValue(clrShape)));
	// Create an array of Point objects that define the polygon.
	//Point point1(rc->left, rc->top);
	//Point point2(rc->right, rc->top);
	//Point point3(rc->left + (rc->right - rc->left)/2, rc->bottom);
	//Point points[3] = {point1, point2, point3};
	Gdiplus::Point points[3];
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

void DcxStacker::DrawItemImage(const HDC hdc, Gdiplus::Image *const img, const LPRECT rc)
{
#ifdef DCX_USE_GDIPLUS
	if (!Dcx::GDIModule.isUseable() || img == nullptr || rc == nullptr || hdc == nullptr)
		return;

	Gdiplus::Graphics grphx( hdc );
	grphx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	grphx.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
	const int w = (rc->right - rc->left), h = (rc->bottom - rc->top);
	if ((img->GetWidth() == 1) || (img->GetHeight() == 1)) {
		// This fixes a GDI+ bug when resizing 1 px images
		// http://www.devnewsgroups.net/group/microsoft.public.dotnet.framework.windowsforms/topic11515.aspx
		grphx.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
		grphx.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
	}
	grphx.DrawImage( img, rc->left, rc->top, w, h );
#endif
}

void DcxStacker::DrawSItem(const LPDRAWITEMSTRUCT idata)
{
	if (idata == nullptr || idata->itemID == -1)
		return;

	auto sitem = reinterpret_cast<LPDCXSITEM>(idata->itemData);
	if (sitem == nullptr)
		return;

	RECT rcWin;
	if (!GetClientRect(idata->hwndItem, &rcWin))
		return;

	auto hFont = sitem->hFont;

	if (hFont == nullptr)
		hFont = GetWindowFont(idata->hwndItem);

	LOGFONT lf = { 0 };
	if (GetObject(hFont, sizeof(LOGFONT), &lf) == 0)
		return;

	// Create temp HDC as drawing buffer.
	// if temp HDC or its bitmap fails to create, use supplied HDC without buffer.
	auto hBuffer = CreateHDCBuffer(idata->hDC, &rcWin);

	HDC memDC = idata->hDC;

	if (hBuffer != nullptr)
		memDC = *hBuffer;

	auto rcText = idata->rcItem;
	auto h = MIN_STACK_HEIGHT, button_base = 0;
	auto Redraw = false;

	h = max(h,lf.lfHeight * 20);
	rcText.bottom = rcText.top + h;
	//rcText.right -= 1;
	if (rcText.bottom > idata->rcItem.bottom)
		rcText.bottom = idata->rcItem.bottom;
	button_base = rcText.bottom;

	// draw button for this item.
	UINT style = DFCS_BUTTONPUSH|DFCS_ADJUSTRECT;
	if (dcx_testflag(idata->itemState, ODS_DISABLED))
		style |= DFCS_INACTIVE;
	if (dcx_testflag(idata->itemState, ODS_SELECTED))
		style |= DFCS_PUSHED;
	//if (dcx_testflag(idata->itemState, ODS_HOTLIGHT))
	//	style |= DFCS_HOT;

	DrawFrameControl(memDC,&rcText,DFC_BUTTON,style);
	rcText.right -= GetSystemMetrics(SM_CXEDGE); // move in right side past border

	// fill background colour if any.
	if (dcx_testflag(this->m_dStyles, STACKERS_GRAD)) {
		auto clrStart = this->m_clrStartGradient;
		auto clrEnd = this->m_clrEndGradient;

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
		ExtTextOut(memDC, rcText.left, rcText.top, ETO_CLIPPED | ETO_OPAQUE, &rcText, TEXT(""), NULL, nullptr );
	}
	
	// draw GDI+ image if any, we draw the image after the  colour fill to allow for alpha in pics.
	if (dcx_testflag(idata->itemState, ODS_SELECTED) && sitem->iSelectedItemImg > -1 && sitem->iSelectedItemImg < static_cast<int>(this->m_vImageList.size()))
		DrawItemImage(memDC, m_vImageList[sitem->iSelectedItemImg], &rcText);
	else if (sitem->iItemImg > -1 && sitem->iItemImg < static_cast<int>(this->m_vImageList.size()))
		DrawItemImage(memDC, m_vImageList[sitem->iItemImg], &rcText);

	// draw text if any
	if (!sitem->tsCaption.empty()) {
		auto oldFont = SelectFont(memDC, hFont);
		Auto(SelectFont(memDC, oldFont));

		// get text colour.
		auto clrText = sitem->clrText, oldClr = (COLORREF)CLR_INVALID;
		if (clrText == CLR_INVALID)
			clrText = GetSysColor(COLOR_BTNTEXT);
		// draw the text
		if (clrText != CLR_INVALID)
			oldClr = SetTextColor(memDC,clrText);

		ctrlDrawText(memDC, sitem->tsCaption, &rcText, DT_CENTER | DT_END_ELLIPSIS);

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

	}
	// draw arrows if wanted.
	if (dcx_testflag(this->m_dStyles, STACKERS_ARROW))
	{
		auto rcArrow = rcText;
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
	if (sitem->pChild != nullptr) {
		auto sWnd = sitem->pChild->getHwnd();
		if (IsWindow(sWnd)) {
			if (dcx_testflag(idata->itemState, ODS_SELECTED))
			{
				RECT rc;
				if (GetWindowRect(sWnd, &rc))
				{
					SetWindowPos(sWnd, nullptr, idata->rcItem.left, button_base, (idata->rcItem.right - idata->rcItem.left), (rc.bottom - rc.top), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
					h += (rc.bottom - rc.top);
				}
				if (sWnd != this->m_hActive) {
					if (IsWindow(this->m_hActive))
						Redraw = true;
				}
				this->m_hActive = sWnd;
			}
			else
				SetWindowPos(sWnd,nullptr,0,0,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW);
		}
	}
	else if (IsWindow(m_hActive) && dcx_testflag(idata->itemState, ODS_SELECTED))
	{
		m_hActive = nullptr;
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
		//SetScrollInfo(m_Hwnd, SB_VERT, &sc, FALSE);
		Redraw = true;
	}
	if (Redraw)
		redrawWindow();
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
					if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
						execAliasEx(TEXT("dclick,%u,%d"), getUserID( ), getSelItemID());
				}
				break;
			case LBN_SELCHANGE:
				{
					if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
						this->execAliasEx(TEXT("sclick,%u,%d"), getUserID(), getSelItemID());
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
			dcxlParam(LPDELETEITEMSTRUCT, idata);
			//auto idata = reinterpret_cast<DELETEITEMSTRUCT *>(lParam);
			if (idata == nullptr)
				break;

			auto sitem = reinterpret_cast<LPDCXSITEM>(idata->itemData);
			if (sitem == nullptr)
				break;

			bParsed = TRUE;
			lRes = TRUE;
			if (sitem->pChild != nullptr) {

				auto dct = sitem->pChild->getControlType();
				if (dct == DcxControlTypes::DIALOG || dct == DcxControlTypes::WINDOW)
					delete sitem->pChild;
				else { //if ( sitem->pChild->getRefCount( ) == 0 ) {
					this->m_pParentDialog->deleteControl(sitem->pChild); // remove from internal list!
					if (IsWindow(sitem->pChild->getHwnd()))
						DestroyWindow(sitem->pChild->getHwnd());
				}

				//if ( sitem->pChild->getType( ) == TEXT("dialog") || sitem->pChild->getType( ) == TEXT("window") )
				//	delete sitem->pChild;
				//else { //if ( sitem->pChild->getRefCount( ) == 0 ) {
				//	this->m_pParentDialog->deleteControl( sitem->pChild ); // remove from internal list!
				//	if (IsWindow(sitem->pChild->getHwnd()))
				//		DestroyWindow( sitem->pChild->getHwnd() );
				//}
			}
			delete sitem;
		}
		break;

	case WM_MEASUREITEM:
		{
			dcxlParam(LPMEASUREITEMSTRUCT, idata);
			if (idata == nullptr)
				break;

			auto sitem = reinterpret_cast<LPDCXSITEM>(idata->itemData);
			if (sitem == nullptr)
				break;

			bParsed = TRUE;
			lRes = TRUE;

			auto hFont = sitem->hFont;

			RECT rcText;
			if (!GetClientRect(m_Hwnd, &rcText))
				break;

			auto h = (long)MIN_STACK_HEIGHT, w = (rcText.right - rcText.left);

			if (hFont == nullptr)
				hFont = GetWindowFont(m_Hwnd);

			LOGFONT lf = { 0 };
			if (GetObject(hFont, sizeof(LOGFONT), &lf) == 0)
				break;

			h = max(h,lf.lfHeight * 20);

			if (sitem->pChild != nullptr) {
				auto win = sitem->pChild->getHwnd();
				if (IsWindow(win) && IsWindowVisible(win)) {
					RECT rc;
					if (GetWindowRect(win, &rc))
					{
						h += (rc.bottom - rc.top);
						//w = max(w,(rc.right - rc.left));
					}
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
			DrawSItem(reinterpret_cast<LPDRAWITEMSTRUCT>(lParam));
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
		//	if (((HWND) lParam == nullptr) && this->m_bAlphaBlend) {
		//		bParsed = TRUE;
		//		//SendMessage(m_Hwnd,WM_SETREDRAW,FALSE,nullptr);
		//		LRESULT res = CallWindowProc( this->m_DefaultWindowProc, m_Hwnd, uMsg, wParam, lParam );
		//		//SendMessage(m_Hwnd,WM_SETREDRAW,TRUE,nullptr);
		//		//InvalidateRect(m_Hwnd, nullptr, FALSE);
		//		//this->redrawWindow();
		//		//RECT rc;
		//		//HWND hParent = GetParent(m_Hwnd);
		//		//GetWindowRect(m_Hwnd, &rc);
		//		//MapWindowPoints(nullptr, hParent, (LPPOINT)&rc, 2);
		//		//InvalidateRect(hParent, &rc, FALSE);
		//		//RedrawWindow(m_Hwnd,0,0,RDW_INVALIDATE|RDW_ALLCHILDREN|RDW_NOERASE);
		//		//RedrawWindow(m_Hwnd,0,0,RDW_INTERNALPAINT|RDW_FRAME|RDW_INVALIDATE|RDW_ALLCHILDREN);
		//		return res;
		//	}
		//}

	case WM_COMMAND:
		{
			if (IsWindow((HWND) lParam)) {
				auto c_this = static_cast<DcxControl *>(GetProp((HWND)lParam, TEXT("dcx_cthis")));
				if (c_this != nullptr)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_COMPAREITEM:
		{
			dcxlParam(LPCOMPAREITEMSTRUCT, idata);

			if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
				auto c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
				if (c_this != nullptr)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_DELETEITEM:
		{
			dcxlParam(LPDELETEITEMSTRUCT, idata);

			if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
				auto c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
				if (c_this != nullptr)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_MEASUREITEM:
		{
			auto cHwnd = GetDlgItem(m_Hwnd, wParam);
			if (IsWindow(cHwnd)) {
				auto c_this = static_cast<DcxControl *>(GetProp(cHwnd, TEXT("dcx_cthis")));
				if (c_this != nullptr)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_DRAWITEM:
		{
			dcxlParam(LPDRAWITEMSTRUCT, idata);

			if ((idata != nullptr) && (IsWindow(idata->hwndItem))) {
				auto c_this = static_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis")));
				if (c_this != nullptr)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_NOTIFY: 
		{
			dcxlParam(LPNMHDR, hdr);

			if (hdr == nullptr)
				break;

			if (hdr->hwndFrom == m_ToolTipHWND) {
				switch( hdr->code )
				{
					case TTN_GETDISPINFO:
						{
							dcxlParam(LPNMTTDISPINFO, di);
							if (di == nullptr)
								break;

							auto sitem = getHotItem();
							if (sitem == nullptr || sitem == (LPDCXSITEM)LB_ERR)
								break;

							di->lpszText = sitem->tsTipText.to_chr();
							di->hinst = nullptr;
							bParsed = TRUE;
						}
						break;
					case TTN_LINKCLICK:
						{
							bParsed = TRUE;
							this->execAliasEx(TEXT("%s,%d,%d"), TEXT("tooltiplink"), getUserID( ), getItemID() );
						}
						break;
				}
			}
			else if (m_Hwnd != hdr->hwndFrom)
			{
				if (IsWindow(hdr->hwndFrom)) {
					auto c_this = static_cast<DcxControl *>(GetProp(hdr->hwndFrom, TEXT("dcx_cthis")));
					if (c_this != nullptr)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
		}
		break;

	case WM_LBUTTONDBLCLK:
		break;

	case WM_LBUTTONUP:
		{
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
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
			auto lastID = getItemCount();
			if (lastID == LB_ERR)
				break;
			lastID--;
			if ((lastID == 0) || (lParam != NULL))
				DrawParentsBackground((HDC)wParam);
			else {
				RECT rcClient;
				if (GetClientRect(m_Hwnd, &rcClient))
				{
					RECT rcItem = { 0 };
					getItemRect(lastID, &rcItem);
					if (rcItem.bottom < rcClient.bottom) {
						rcClient.top = rcItem.bottom;
						DrawParentsBackground((HDC)wParam, &rcClient);
					}

					//lastID = this->getSelItemID();
					//if (lastID != LB_ERR) {
					//	getItemRect(lastID, &rcItem);
					//	DrawParentsBackground((HDC) wParam, &rcItem);
					//}
				}
			}
			return TRUE;
		}
		break;

	case WM_PAINT:
		{
			if (!m_bAlphaBlend)
				break;

			PAINTSTRUCT ps;
			auto hdc = BeginPaint(m_Hwnd, &ps);

			bParsed = TRUE;

			// Setup alpha blend if any.
			auto ai = SetupAlphaBlend(&hdc);

			lRes = CallWindowProc( m_DefaultWindowProc, m_Hwnd, uMsg, (WPARAM) hdc, lParam );

			FinishAlphaBlend(ai);

			EndPaint( m_Hwnd, &ps );
		}
		break;

	case WM_DESTROY:
		{
			delete this;
			bParsed = TRUE;
		}
		break;
	default:
		lRes = CommonMessage( uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}
