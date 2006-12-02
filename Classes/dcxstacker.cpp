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

#include "dcxstacker.h"
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

DcxStacker::DcxStacker( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles )
: DcxControl( ID, p_Dialog )
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles | WS_EX_CONTROLPARENT,
    "ListBox",
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

  SendMessage( this->m_Hwnd, CCM_SETVERSION, (WPARAM) 5, (LPARAM) 0 );

	//this->m_hBackBrush = GetSysColorBrush(COLOR_3DFACE);
	this->m_hActive = NULL;

	if (p_Dialog->getToolTip() != NULL) {
		if (styles.istok("tooltips"," ")) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
	}

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxStacker::~DcxStacker( ) {

  this->unregistreDefaultWindowProc( );
}

void DcxStacker::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  *Styles |= LBS_OWNERDRAWVARIABLE|LBS_NOTIFY;
	this->m_dStyles = 0;

  unsigned int i = 1, numtok = styles.numtok( " " );

  while ( i <= numtok ) {

		if ( styles.gettok( i , " " ) == "alpha" )
			this->m_bAlphaBlend = true;
		else if (( styles.gettok( i , " " ) == "shadow" ) && isXP())
			this->m_bShadowText = true;
		else if ( styles.gettok( i , " " ) == "vscroll" )
			*Styles |= WS_VSCROLL;
		else if ( styles.gettok( i , " " ) == "gradient" )
			this->m_dStyles |= STACKERS_GRAD;
		else if ( styles.gettok( i , " " ) == "arrows" )
			this->m_dStyles |= STACKERS_ARROW;

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

void DcxStacker::parseInfoRequest( TString & input, char * szReturnValue ) {

	int numtok = input.numtok( " " );

	// [NAME] [ID] [PROP] [N]
	if ( input.gettok( 3, " " ) == "text" && numtok > 3 ) {
		int nSel = input.gettok( 4, " " ).to_int( ) - 1;

		if ( nSel > -1 && nSel < ListBox_GetCount( this->m_Hwnd ) ) {
			LPDCXSITEM sitem = this->getItem(nSel);
			if (sitem != NULL && sitem != (LPDCXSITEM)LB_ERR)
				lstrcpy(szReturnValue,sitem->tsCaption.to_chr());
			return;
		}
	}
	// [NAME] [ID] [PROP]
	else if ( input.gettok( 3, " " ) == "num" ) {
		wsprintf( szReturnValue, "%d", ListBox_GetCount( this->m_Hwnd ) );
		return;
	}
	// [NAME] [ID] [PROP]
	else if ( input.gettok( 3, " " ) == "sel" ) {
		wsprintf( szReturnValue, "%d", ListBox_GetCurSel( this->m_Hwnd ) + 1 );
		return;
	}
	// [NAME] [ID] [PROP] [N]
	else if ( input.gettok( 3, " " ) == "haschild" && numtok > 3 ) {
		int nSel = input.gettok( 4, " " ).to_int( ) - 1;

		lstrcpy(szReturnValue,"$false");

		if ( nSel > -1 && nSel < ListBox_GetCount( this->m_Hwnd ) ) {
			LPDCXSITEM sitem = this->getItem(nSel);
			if (sitem != NULL && sitem != (LPDCXSITEM)LB_ERR) {
				if (sitem->pChild != NULL)
					lstrcpy(szReturnValue,"$true");
			}
			return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if ( input.gettok( 3, " " ) == "childid" && numtok > 3 ) {
		int nSel = input.gettok( 4, " " ).to_int( ) - 1;

		lstrcpy(szReturnValue,"0");

		if ( nSel > -1 && nSel < ListBox_GetCount( this->m_Hwnd ) ) {
			LPDCXSITEM sitem = this->getItem(nSel);
			if (sitem != NULL && sitem != (LPDCXSITEM)LB_ERR) {
				if (sitem->pChild != NULL)
					wsprintf(szReturnValue,"%d",sitem->pChild->getUserID());
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

void DcxStacker::parseCommandRequest(TString &input) {
	XSwitchFlags flags;

	ZeroMemory((void*) &flags, sizeof(XSwitchFlags));
	this->parseSwitchFlags(input.gettok(3, " "), &flags);

	int numtok = input.numtok(" ");

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags.switch_flags[17]) {
    SendMessage(this->m_Hwnd, LB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0);
	}

	//xdid -a [NAME] [ID] [SWITCH] [N] [+FLAGS] [COLOR] [BGCOLOR] Item Text [TAB] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if (flags.switch_flags[0] && numtok > 7) {
		TString item(input.gettok(1,"\t"));
		item.trim();
		TString ctrl(input.gettok(2,"\t"));
		ctrl.trim();

    int nPos = item.gettok( 4, " " ).to_int( ) - 1;

    if ( nPos < 0 )
      nPos = ListBox_GetCount( this->m_Hwnd );
		if (nPos == LB_ERR)
			nPos = 0;

		LPDCXSITEM sitem = new DCXSITEM;
		sitem->clrBack = (COLORREF)input.gettok(7).to_num();
		sitem->clrText = (COLORREF)input.gettok(6).to_num();
		sitem->pChild = NULL;
		sitem->hFont = NULL;
		sitem->tsCaption = item.gettok(8,-1," ");

		if (ctrl.len() > 0) {
			UINT ID = mIRC_ID_OFFSET + (UINT)ctrl.gettok( 1 ).to_int( );

			if ( ID > mIRC_ID_OFFSET - 1 && 
				!IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
				this->m_pParentDialog->getControlByID( ID ) == NULL ) 
			{
				DcxControl * p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,ctrl,2,-1,this->m_Hwnd);

				if ( p_Control != NULL ) {
					this->m_pParentDialog->addControl( p_Control );
					sitem->pChild = p_Control;
					ShowWindow(p_Control->getHwnd(),SW_HIDE);
					this->redrawWindow( );
				}
				else {
					DCXError("/xdid -a","Error creating control");
					delete sitem;
					return;
				}
			}
			else {
				TString error;
				error.sprintf("Control with ID \"%d\" already exists", ID - mIRC_ID_OFFSET );
				DCXError("/xdid -a",error.to_chr() );
				delete sitem;
				return;
			}
		}
		if (SendMessage(this->m_Hwnd,LB_INSERTSTRING,nPos,(LPARAM)sitem) < 0) {
			delete sitem;
			DCXError("/xdid -a","Error adding item to control");
			return;
		}
	}
	// xdid -c [NAME] [ID] [SWITCH] [N]
	else if (flags.switch_flags[2] && numtok > 3) {
    int nPos = input.gettok( 4, " " ).to_int( ) - 1;

    if ( nPos > -1 && nPos < ListBox_GetCount( this->m_Hwnd ) )
			SendMessage(this->m_Hwnd,LB_SETCURSEL,nPos,NULL);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags.switch_flags[3] && (numtok > 3)) {
    int nPos = input.gettok( 4, " " ).to_int( ) - 1;

    if ( nPos > -1 && nPos < ListBox_GetCount( this->m_Hwnd ) )
        ListBox_DeleteString( this->m_Hwnd, nPos );
	}
  //xdid -r [NAME] [ID] [SWITCH]
  else if (flags.switch_flags[17]) {
  }
	//xdid -u [NAME] [ID] [SWITCH]
	else if ( flags.switch_flags[20] ) {
		ListBox_SetCurSel( this->m_Hwnd, -1 );
	}
	// xdid -T [NAME] [ID] [SWITCH] [N] (ToolTipText)
  else if (flags.switch_cap_flags[19] && numtok > 3) {
    int nPos = input.gettok( 4, " " ).to_int( ) - 1;

		if ( nPos > -1 && nPos < ListBox_GetCount( this->m_Hwnd ) ) {
			LPDCXSITEM sitem = this->getItem(nPos);
			if (sitem != NULL && sitem != (LPDCXSITEM)LB_ERR) {
				sitem->tsTipText = (numtok > 4 ? input.gettok(5, -1, " ") : "");
				sitem->tsTipText.trim();
			}
		}
  }
	else
		this->parseGlobalCommandRequest(input, flags);
}

int DcxStacker::getItemID(void) const {
	POINT pt;
	GetCursorPos( &pt );
	ScreenToClient( this->m_Hwnd, &pt );
	return (int)(LOWORD((DWORD)SendMessage(this->m_Hwnd,LB_ITEMFROMPOINT,NULL,MAKELPARAM(pt.x,pt.y))) +1);
}

int DcxStacker::getSelItemID(void) const {
	return (int)(LOWORD((DWORD)SendMessage(this->m_Hwnd,LB_GETCURSEL,NULL,NULL)) +1);
}

DWORD DcxStacker::getItemCount(void) const {
	return (DWORD)SendMessage(this->m_Hwnd,LB_GETCOUNT,NULL,NULL);
}

LPDCXSITEM DcxStacker::getItem(int nPos) const {
	return (LPDCXSITEM)SendMessage(this->m_Hwnd,LB_GETITEMDATA,nPos,NULL);
}

LPDCXSITEM DcxStacker::getHotItem(void) const {
	return (LPDCXSITEM)SendMessage(this->m_Hwnd,LB_GETITEMDATA,this->getItemID()-1,NULL);
}

void DcxStacker::getItemRect(int nPos, LPRECT rc) const {
	SendMessage(this->m_Hwnd,LB_GETITEMRECT,(WPARAM)nPos,(LPARAM)rc);
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
						this->callAliasEx( NULL, "%s,%d,%d", "dclick", this->getUserID( ), this->getSelItemID());
				}
				break;
			case LBN_SELCHANGE:
				{
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
						this->callAliasEx( NULL, "%s,%d,%d", "sclick", this->getUserID( ), this->getSelItemID());
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
				if ( sitem->pChild->getType( ) == "dialog" || sitem->pChild->getType( ) == "window" )
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
				hFont = (HFONT) SendMessage(this->m_Hwnd, WM_GETFONT, 0, 0);

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
			DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
			if (idata == NULL || idata->itemID == -1)
				break;
			LPDCXSITEM sitem = (LPDCXSITEM)idata->itemData;
			if (sitem == NULL)
				break;
			HFONT hFont = sitem->hFont;
			LOGFONT lf;
			RECT rcText = idata->rcItem;
			int h = MIN_STACK_HEIGHT, button_base = 0;
			bool Redraw = false;

			if (hFont == NULL)
				hFont = (HFONT) SendMessage(this->m_Hwnd, WM_GETFONT, 0, 0);

			GetObject(hFont, sizeof(LOGFONT), &lf);

			h = max(h,lf.lfHeight * 20);
			rcText.bottom = rcText.top + h;
			//rcText.right -= 1;
			if (rcText.bottom > idata->rcItem.bottom)
				rcText.bottom = idata->rcItem.bottom;
			button_base = rcText.bottom;

			// draw background if we need to.
			//if (this->isExStyle(WS_EX_TRANSPARENT) || this->m_bAlphaBlend) {
			//	HRGN hrgn = CreateRectRgnIndirect(&idata->rcItem);
			//	if (hrgn != NULL) {
			//		SelectClipRgn(idata->hDC,hrgn);
			//		DcxControl::DrawParentsBackground(idata->hDC);
			//		SelectClipRgn(idata->hDC,NULL);
			//		DeleteObject(hrgn);
			//	}
			//}
			// draw button for this item.
			UINT style = DFCS_BUTTONPUSH|DFCS_ADJUSTRECT;
			if (idata->itemState & ODS_DISABLED)
				style |= DFCS_INACTIVE;
			if (idata->itemState & ODS_SELECTED)
				style |= DFCS_PUSHED;

			DrawFrameControl(idata->hDC,&rcText,DFC_BUTTON,style);

			// fill background colour if any.
			if (this->m_dStyles & STACKERS_GRAD) {
				COLORREF clrbkg = sitem->clrBack;

				if (clrbkg == -1)
					clrbkg = GetSysColor(COLOR_BTNFACE);

				XPopupMenuItem::DrawGradient( idata->hDC, &rcText, clrbkg, GetSysColor(COLOR_GRADIENTACTIVECAPTION), FALSE);
			}
			else if (sitem->clrBack != -1) {
				SetBkColor(idata->hDC,sitem->clrBack);
				ExtTextOut(idata->hDC, rcText.left, rcText.top, ETO_CLIPPED | ETO_OPAQUE, &rcText, "", NULL, NULL );
			}

			// draw text if any
			if (sitem->tsCaption.len()) {
				SetBkMode(idata->hDC,TRANSPARENT);
				UINT f = DST_TEXT;
				if (idata->itemState & ODS_DISABLED)
					f |= DSS_DISABLED;
				HFONT oldFont = (HFONT)SelectObject(idata->hDC,hFont);
				// get text colour.
				COLORREF clrText = sitem->clrText;
				if (clrText == -1)
					clrText = GetSysColor(COLOR_BTNTEXT);
				// draw the text
				if (this->m_bShadowText) { // could cause problems with pre-XP as this is commctrl v6+
					DrawShadowText(idata->hDC,sitem->tsCaption.to_wchr(), sitem->tsCaption.len(),&rcText,
						DT_END_ELLIPSIS | DT_CENTER, clrText, 0, 5, 5);
				}
				else {
					if (sitem->clrText != -1)
						SetTextColor(idata->hDC,clrText);
					DrawText(idata->hDC, sitem->tsCaption.to_chr(), sitem->tsCaption.len(), &rcText, DT_CENTER | DT_END_ELLIPSIS);
				}

				SelectObject(idata->hDC,oldFont);
			}
			// draw arrows if wanted.
			if (this->m_dStyles & STACKERS_ARROW) {
				RECT rcArrow = rcText;
				SelectObject(idata->hDC,GetStockObject(BLACK_BRUSH));
				rcArrow.left = rcArrow.right - 10;
				rcArrow.top += 5;
				rcArrow.bottom -= 5;
				POINT pts[3];
				pts[0].x = rcArrow.left;
				pts[0].y = rcArrow.top;
				pts[1].x = rcArrow.right;
				pts[1].y = rcArrow.top;
				pts[2].x = rcArrow.left + (rcArrow.right - rcArrow.left)/2;
				pts[2].y = rcArrow.bottom;
				Polygon(idata->hDC,pts,3);
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
				SendMessage(this->m_Hwnd,LB_SETITEMHEIGHT,idata->itemID,h);
				Redraw = true;
			}
			if (Redraw)
				this->redrawWindow();
		}
		break;
	}
	return lRes;
}

LRESULT DcxStacker::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

		LRESULT lRes = 0L;
		switch( uMsg ) {

		case WM_HELP:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
					this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
			}
			break;
		case WM_GETDLGCODE:
			{
				bParsed = TRUE;
				return DLGC_WANTALLKEYS;
			}
			break;

    case WM_HSCROLL:
    case WM_VSCROLL:
			//{
			//	bParsed = TRUE;
			//	SendMessage(this->m_Hwnd,WM_SETREDRAW,FALSE,NULL);
			//	LRESULT res = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, wParam, lParam );
			//	SendMessage(this->m_Hwnd,WM_SETREDRAW,TRUE,NULL);
			//	//this->redrawWindow();
			//	RedrawWindow(this->m_Hwnd,0,0,RDW_INTERNALPAINT|RDW_FRAME|RDW_INVALIDATE|RDW_ALLCHILDREN);
			//	return res;
			//}
			//break;

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

				if (hdr->hwndFrom == this->m_ToolTipHWND) {
					switch( hdr->code ) {
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
							this->callAliasEx( NULL, "%s,%d,%d", "tooltiplink", this->getUserID( ), this->getItemID() );
						}
						break;
					}
				}
				else if (this->m_Hwnd != hdr->hwndFrom) {
					if (IsWindow(hdr->hwndFrom)) {
						DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,"dcx_cthis");
						if (c_this != NULL)
							lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
					}
				}
      }
      break;

		case WM_CTLCOLORDLG:
			{
				bParsed = TRUE;
				return (INT_PTR) this->getBackClrBrush( );
			}
			break;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
      {

        DcxControl * p_Control = this->m_pParentDialog->getControlByHWND( (HWND) lParam );

        if ( p_Control != NULL ) {

          COLORREF clrText = p_Control->getTextColor( );
          COLORREF clrBackText = p_Control->getBackColor( );
          HBRUSH hBackBrush = p_Control->getBackClrBrush( );

          if ( clrText != -1 )
            SetTextColor( (HDC) wParam, clrText );
 
          if ( clrBackText != -1 )
            SetBkColor( (HDC) wParam, clrBackText );

					//if (p_Control->isExStyle(WS_EX_TRANSPARENT)) {
					//	bParsed = TRUE;
					//	return (LRESULT)GetStockObject(NULL_BRUSH);
					//}

          if ( hBackBrush != NULL ) {

            bParsed = TRUE;
            return (LRESULT) hBackBrush;
          }

        }
      }
      break;

		case WM_LBUTTONDOWN:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx( NULL, "%s,%d,%d", "lbdown", this->getUserID( ), this->getItemID());
			}
			break;

		case WM_LBUTTONUP:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx( NULL, "%s,%d,%d", "lbup", this->getUserID( ), this->getItemID());
			}
			break;

		case WM_CONTEXTMENU:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx( NULL, "%s,%d,%d", "rclick", this->getUserID( ), this->getItemID());
			}
			break;

		case WM_ENABLE:
			{
				this->redrawWindow();
			}
			break;

    case WM_MOUSEMOVE:
      {
        this->m_pParentDialog->setMouseControl( this->getUserID( ) );
      }
      break;

    case WM_SETFOCUS:
      {
        this->m_pParentDialog->setFocusControl( this->getUserID( ) );
      }
      break;

		case WM_SETCURSOR:
			{
				if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd && this->m_hCursor != NULL ) {
					SetCursor( this->m_hCursor );
					bParsed = TRUE;
					return TRUE;
				}
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
			if (lastID == 0)
				this->DrawParentsBackground((HDC) wParam);
			else {
				RECT rcItem, rcClient;
				GetClientRect(this->m_Hwnd,&rcClient);
				this->getItemRect(lastID, &rcItem);
				if (rcItem.bottom < rcClient.bottom) {
					HRGN bkg_rgn = CreateRectRgn(rcClient.left,rcItem.bottom,rcClient.right,rcClient.bottom);
					if (bkg_rgn != NULL) {
						SelectClipRgn((HDC)wParam,bkg_rgn);
						this->DrawParentsBackground((HDC) wParam);
						SelectClipRgn((HDC)wParam,NULL);
						DeleteObject(bkg_rgn);
					}
				}
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

				LRESULT res = 0L;
				bParsed = TRUE;

				// Setup alpha blend if any.
				LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

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
      break;
  }

  return lRes;
}
