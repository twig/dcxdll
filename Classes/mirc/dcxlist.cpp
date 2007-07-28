/*!
 * \file dcxlist.cpp
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.1
 *
 * \b Revisions
 *	1.1
 *		Added Visual Studio 2005 specific code. Ook
 *
 * © ScriptsDB.org - 2006
 */

#include "dcxlist.h"
#include "../dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxList::DcxList( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ),
  m_iDragList(0),
  m_iLastDrawnLine(0),
  m_bUseDrawInsert(TRUE)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles | WS_EX_CLIENTEDGE, 
		"LISTBOX", 
		NULL,
		WS_CHILD | Styles, 
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL), 
		NULL);

	if ( bNoTheme )
		dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );

	// Check for "draglist" style
	if (styles.find("draglist", 0))
	{
		// if its multiple select, cant use
		if (!this->isStyle(LBS_MULTIPLESEL))
		{
			if (MakeDragList(this->m_Hwnd))
			{
				m_iDragList = RegisterWindowMessage(DRAGLISTMSGSTRING);
				this->m_pParentDialog->RegisterDragList(this);
			}
			else
				this->showError(NULL, "DcxList", "Error applying draglist style");
		}
		else
			this->showError(NULL, "DcxList", "Cannot apply draglist style with multi style");
	}

	DragAcceptFiles(this->m_Hwnd, TRUE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxList::~DcxList( ) {
	this->m_pParentDialog->UnregisterDragList(this);
	this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxList::parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
{
	unsigned int i = 1, numtok = styles.numtok();
	*Styles |= LBS_NOTIFY | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED;

	while (i <= numtok) {
		if (styles.gettok(i) == "noscroll")
			*Styles |= LBS_DISABLENOSCROLL;
		else if (styles.gettok(i) == "multi")
			*Styles |= LBS_MULTIPLESEL;
		else if (styles.gettok(i) == "extsel")
			*Styles |= LBS_EXTENDEDSEL;
		else if (styles.gettok(i) == "nointegral")
			*Styles |= LBS_NOINTEGRALHEIGHT;
		else if (styles.gettok(i) == "nosel")
			*Styles |= LBS_NOSEL;
		else if (styles.gettok(i) == "sort")
			*Styles |= LBS_SORT;
		else if (styles.gettok(i) == "tabs")
			*Styles |= LBS_USETABSTOPS;
		else if (styles.gettok(i) == "multicol")
			*Styles |= LBS_MULTICOLUMN;
		else if (styles.gettok(i) == "vsbar")
			*Styles |= WS_VSCROLL;
		else if (styles.gettok(i) == "hsbar")
			*Styles |= WS_HSCROLL;
		else if (styles.gettok(i) == "alpha")
			this->m_bAlphaBlend = true;
		else if (styles.gettok(i) == "dragline")
			this->m_bUseDrawInsert = FALSE;
		else if (styles.gettok(i) == "noformat") {
			this->m_bCtrlCodeText = false;
			*Styles &= ~LBS_OWNERDRAWFIXED;
		}
		//else if (styles.gettok(i) == "shadow") // looks crap
		//	this->m_bShadowText = true;

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

void DcxList::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( );

	TString prop(input.gettok( 3 ));

	// [NAME] [ID] [PROP] [N]
	if ( prop == "text" && numtok > 3 ) {
		int nSel = input.gettok( 4 ).to_int( ) - 1;
		if ( nSel > -1 && nSel < ListBox_GetCount( this->m_Hwnd ) ) {
			int l = ListBox_GetTextLen(this->m_Hwnd, nSel);
			if (l != LB_ERR && l < 900)
				ListBox_GetText( this->m_Hwnd, nSel, szReturnValue );
			else
				this->showError(prop.to_chr(), NULL, "String Too Long (Greater than 900 chars)");
			return;
		}
	}
  // [NAME] [ID] [PROP]
  else if ( prop == "num" ) {

    wsprintf( szReturnValue, "%d", ListBox_GetCount( this->m_Hwnd ) );
    return;
  }
	// [NAME] [ID] [PROP] (N)
	else if (prop == "sel") {
		if (this->isStyle(LBS_MULTIPLESEL) || this->isStyle(LBS_EXTENDEDSEL)) {
			int n = ListBox_GetSelCount(this->m_Hwnd);

			if (n > 0) {
				int *p = new int[n];
				ListBox_GetSelItems(this->m_Hwnd, n, p);

				// get a unique value
				if (numtok > 3) {
					int i = input.gettok( 4 ).to_int();
					TString value;

					if (i == 0) {
						value.sprintf("%d", n);
						lstrcpyn(szReturnValue, value.to_chr(), 900);
					}
					else if ((i > 0) && (i <= n)) {
						value.sprintf("%d", p[i -1] +1);
						lstrcpyn(szReturnValue, value.to_chr(), 900);
					}
				}
				else {
					// get all items in a long comma seperated string
					std::string path = "";
					char num[11];
					int i = 0;

					while (i < n) {
#ifdef VS2005
						_itoa(p[i] +1, num, 10);
#else
						itoa(p[i] +1, num, 10);
#endif
						path += num;
						path += ',';

						i++;
					}

					lstrcpyn(szReturnValue, path.c_str(), 900);
				}
				delete [] p;
				return;
			}
		}
		// single select
		else {
			wsprintf(szReturnValue, "%d", ListBox_GetCurSel(this->m_Hwnd) +1);
			return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if (prop == "tbitem") {
		int top;
		int bottom;
		int height;
		int count = ListBox_GetCount(this->m_Hwnd);
		RECT rc;

		GetClientRect(this->m_Hwnd, &rc);

		top = SendMessage(this->m_Hwnd, LB_GETTOPINDEX, NULL, NULL);
		height = SendMessage(this->m_Hwnd, LB_GETITEMHEIGHT, NULL, NULL);

		bottom = top + ((rc.bottom - rc.top) / height);

		if (bottom > count)
			bottom = count;

		wsprintf(szReturnValue, "%d %d", top, bottom);
		return;
	}
  // [NAME] [ID] [PROP] [N]
  else if ( prop == "find" ) {
    return;
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

void DcxList::parseCommandRequest( TString & input ) {

	XSwitchFlags flags;
	ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
	this->parseSwitchFlags( input.gettok( 3 ), &flags );

	int numtok = input.numtok( );

	//xdid -r [NAME] [ID] [SWITCH]
	if (flags.switch_flags[17]) {
		ListBox_ResetContent( this->m_Hwnd );
	}

	//xdid -a [NAME] [ID] [SWITCH] [N] [TEXT]
	if ( flags.switch_flags[0] && numtok > 4 ) {

		int nPos = input.gettok( 4 ).to_int( ) - 1;

		if ( nPos == -1 )
			nPos = ListBox_GetCount( this->m_Hwnd );

		ListBox_InsertString( this->m_Hwnd, nPos, input.gettok( 5, -1 ).to_chr( ) );
	}
	//xdid -A [NAME] [ID] [SWITCH] [N] [+FLAGS] [TEXT]
	else if ( flags.switch_cap_flags[0] && numtok > 5 ) {

		int nPos = input.gettok( 4 ).to_int( ) - 1;

		if ( nPos == -1 )
			nPos = ListBox_GetCount( this->m_Hwnd );

		TString opts(input.gettok( 5 ));
		TString itemtext(input.gettok( 6, -1 ));
		int nMaxStrlen = 0;
		char res[1024];

		if (opts[0] != '+') {
			this->showError(NULL, "-A", "Invalid Flags");
			return;
		}

		itemtext.trim();

		switch (opts[1])
		{
		case 'h': // [TEXT] == [table] [item]
			{
				if (itemtext.numtok() == 2) { // load single item from hash table by item name
					mIRCevalEX(res, 1024, "$hget(%s,%s)", itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
					ListBox_InsertString( this->m_Hwnd, nPos, res );
					nMaxStrlen = lstrlen(res);
				}
				else
					this->showError(NULL, "-A +h", "Invalid Syntax");
			}
			break;
		case 'n': // [TEXT] == [table] [N]
			{
				if (itemtext.numtok() == 2) { // load single item from hash table by index
					mIRCevalEX(res, 1024, "$hget(%s,%s).data", itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
					ListBox_InsertString( this->m_Hwnd, nPos, res );
					nMaxStrlen = lstrlen(res);
				}
				else
					this->showError(NULL, "-A +n", "Invalid Syntax");
			}
			break;
		case 't': // [TEXT] == [table] [startN] [endN]
			{
				if (itemtext.numtok() == 3) { // add contents of a hash table to list
					int max_items = 0, len = 0;
					TString htable(itemtext.gettok( 1 ));
					int startN = itemtext.gettok( 2 ).to_int();
					int endN = itemtext.gettok( 3 ).to_int();

					// get total items in table.
					mIRCevalEX(res, 1024, "$hget(%s,0).item", htable.to_chr());
					max_items = atoi(res);

					// no items in table.
					if (max_items == 0)
						return;

					// If neg number is given start from (last item) - startN
					if (startN < 0)
						startN = (max_items + startN);

					// if start N < 1, make it 1. Allows 0 item. Or case where higher neg number was supplied than items avail.
					if (startN < 1)
						startN = 1;

					// If neg number is given set end to (last item) - endN
					if (endN < 0)
						endN = (max_items + endN);
					// if endN > max or == 0, set to max, allows 0 item for end meaning all
					else if ((endN > max_items) || (endN == 0))
						endN = max_items;

					// if endN < 1 set it to 1
					if (endN < 1)
						endN = 1;

					// check endN comes after startN
					if (endN < startN) {
						this->showError(NULL, "-A +t", "Invalid Range");
						return;
					}

					this->setRedraw(FALSE);
					for (int i = startN; i <= endN; i++) {
						mIRCevalEX(res, 1024, "$hget(%s,%d).data", htable.to_chr(), i);
						ListBox_InsertString( this->m_Hwnd, nPos++, res );
						len = lstrlen( res );
						if (len > nMaxStrlen)
							nMaxStrlen = len;
					}
					this->setRedraw(TRUE);
					this->redrawWindow();
				}
				else
					this->showError(NULL, "-A +t", "Invalid Syntax");
			}
			break;
		case 'f': // [TEXT] == [startN] [endN] [filename]
			{
				if (itemtext.numtok() > 2) { // add contents of a file to list
					int startN = itemtext.gettok( 1 ).to_int();
					int endN = itemtext.gettok( 2 ).to_int();
					TString filename(itemtext.gettok( 3, -1));

					if (IsFile(filename)) {
						char *buf = readFile(filename.to_chr());
						if (buf != NULL) {
							int max_lines = 0, len = 0;
							TString contents(buf);
							delete [] buf;
							char *tok = "\r\n";

							max_lines = contents.numtok(tok);
							if (max_lines == 1) {
								tok = "\n";
								max_lines = contents.numtok(tok);
							}

							// no data in file.
							if (max_lines == 0)
								return;

							// If neg number is given start from (last line) - startN
							if (startN < 0)
								startN = (max_lines + startN);

							// if start N < 1, make it 1. Allows 0 item. Or case where higher neg number was supplied than lines avail.
							if (startN < 1)
								startN = 1;

							// If neg number is given set end to (last line) - endN
							if (endN < 0)
								endN = (max_lines + endN);
							// if endN > max or == 0, set to max, allows 0 for end meaning all
							else if ((endN > max_lines) || (endN == 0))
								endN = max_lines;

							// if endN < 1 set it to 1
							if (endN < 1)
								endN = 1;

							// check endN comes after startN
							if (endN < startN) {
								this->showError(NULL, "-A +f", "Invalid Range");
								return;
							}
							this->setRedraw(FALSE);

							for (int i = startN; i <= endN; i++) {
								itemtext = contents.gettok( i, tok);
								ListBox_InsertString( this->m_Hwnd, nPos++, itemtext.to_chr() );
								len = itemtext.len();
								if (len > nMaxStrlen)
									nMaxStrlen = len;
							}
							this->setRedraw(TRUE);
							this->redrawWindow();
						}
					}
					else
						this->showErrorEx(NULL, "-A", "Unable To Access File: %s", itemtext.to_chr());
				}
				else
					this->showError(NULL, "-A +f", "Invalid Syntax");
			}
			break;
		default:
			{
				ListBox_InsertString( this->m_Hwnd, nPos, itemtext.to_chr( ) );
				nMaxStrlen = itemtext.len();
			}
			break;
		}
		// Now update the horizontal scroller
		int nHorizExtent = ListBox_GetHorizontalExtent( this->m_Hwnd );

		{ // Get Font sizes (best way i can find atm, if you know something better then please let me know)
			HDC hdc = GetDC( this->m_Hwnd);
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
		}

		if (nMaxStrlen > nHorizExtent)
			ListBox_SetHorizontalExtent( this->m_Hwnd, nMaxStrlen);
	}
	//xdid -c [NAME] [ID] [SWITCH] [N,[N,[...]]]
	else if ( flags.switch_flags[2] && numtok > 3 ) {

		int nItems = ListBox_GetCount( this->m_Hwnd );

		if ( this->isStyle( LBS_MULTIPLESEL ) || this->isStyle( LBS_EXTENDEDSEL ) ) {

			TString Ns(input.gettok( 4 ));

			int i = 1, n = Ns.numtok( TSCOMMA );

			while ( i <= n ) {

				int nSel = Ns.gettok( i, TSCOMMA ).to_int( ) - 1;

				if (nSel == -1)
					nSel = nItems -1;

				if ( nSel > -1 && nSel < nItems )
					ListBox_SetSel( this->m_Hwnd, TRUE, nSel );

				i++;
			}
		}
		else {

			int nSel = input.gettok( 4 ).to_int( ) - 1;

			if (nSel == -1)
				nSel = nItems -1;

			if ( nSel > -1 && nSel < nItems )
				ListBox_SetCurSel( this->m_Hwnd, nSel );
		}
	}
	//xdid -d [NAME] [ID] [SWITCH] [N]
	else if ( flags.switch_flags[3] && numtok > 3 ) {

		int nPos = input.gettok( 4 ).to_int( ) - 1;

		if (nPos == -1)
			nPos = ListBox_GetCount( this->m_Hwnd ) -1;

		if ( nPos > -1 && nPos < ListBox_GetCount( this->m_Hwnd ) )
			ListBox_DeleteString( this->m_Hwnd, nPos );
	}
	//xdid -r [NAME] [ID] [SWITCH]
	else if (flags.switch_flags[17]) {
	}
	//xdid -u [NAME] [ID] [SWITCH]
	else if ( flags.switch_flags[20] ) {

		if ( this->isStyle( LBS_MULTIPLESEL ) || this->isStyle( LBS_EXTENDEDSEL ) )
			ListBox_SetSel( this->m_Hwnd, FALSE, -1 );
		else 
			ListBox_SetCurSel( this->m_Hwnd, -1 );
	}
	//xdid -m [NAME] [ID] [SWITCH] [+FLAGS] [N](,[N]...)
	else if ( flags.switch_flags[12] && numtok > 4 ) {
		TString opts(input.gettok( 4 ));

		if (opts.find('w',0))
			ListBox_SetColumnWidth( this->m_Hwnd, input.gettok( 5 ).to_int( ));
		else if (opts.find('t',0)) {
			//ListBox_SetTabStops( this->m_Hwnd, 1, nWidth); // needs updated for multiple tab stops
			TString Ns(input.gettok( 5 ));

			int i = 1, n = Ns.numtok( TSCOMMA );

			if (n == 1) {
				int nTab = Ns.to_int();
				if (nTab < 0)
					ListBox_SetTabStops( this->m_Hwnd, NULL, NULL);
				else
					ListBox_SetTabStops( this->m_Hwnd, 1, nTab);
			}
			else {
				int *tabs = new int[n];

				while ( i <= n ) {
					tabs[i-1] = Ns.gettok( i, TSCOMMA).to_int();
					i++;
				}
				ListBox_SetTabStops( this->m_Hwnd, n, tabs);
				delete [] tabs;
			}
		}
		else
			this->showError(NULL, "-m", "Invalid Flags");
	}
	//xdid -o [NAME] [ID] [N] [TEXT]
	else if ( flags.switch_flags[14] ) {
		int nPos = input.gettok( 4 ).to_int() - 1;

		if (nPos == -1)
			nPos = ListBox_GetCount( this->m_Hwnd ) -1;

		if (nPos > -1 && nPos < ListBox_GetCount(this->m_Hwnd)) {
			//ListBox_SetItemData(this->m_Hwnd, nPos, input.to_chr()); //.gettok(5, -1).to_chr());
			ListBox_DeleteString(this->m_Hwnd, nPos);
			ListBox_InsertString(this->m_Hwnd, nPos, input.gettok( 5, -1 ).to_chr( ));
		}
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxList::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
   if ((int) uMsg == this->m_iDragList)
   {
      bParsed = TRUE;

      LPDRAGLISTINFO dli = (LPDRAGLISTINFO) lParam;
      int item;
      int sel = ListBox_GetCurSel(this->m_Hwnd) +1;
      char ret[20];

      switch (dli->uNotification)
      {
         // begin dragging item
         case DL_BEGINDRAG:
            // callback DIALOG itemdragbegin THIS_ID DRAGGEDITEM
            callAliasEx(ret, "%s,%d,%d", "itemdragbegin", this->getUserID(), sel);

            // cancel drag event
            if (lstrcmpi(ret, "nodrag") == 0)
               return FALSE;
            
            return TRUE;

         // cancel drag
         case DL_CANCELDRAG:
            // callback DIALOG itemdragcancel THIS_ID DRAGGEDITEM
            callAliasEx(ret, "%s,%d,%d", "itemdragcancel", this->getUserID(), sel);

            if (m_bUseDrawInsert)
               this->m_pParentDialog->redrawWindow();
            else
               this->redrawWindow();

            break;

         // current dragging, as mirc if it needs to change the cursor or not
         case DL_DRAGGING:
            item = LBItemFromPt(this->m_Hwnd, dli->ptCursor, TRUE);

            if (m_bUseDrawInsert)
               DrawInsert(this->m_pParentHWND, this->m_Hwnd, item);
            else
               DrawDragLine(item);

            // callback DIALOG itemdrag THIS_ID SEL_ITEM MOUSE_OVER_ITEM
            callAliasEx(ret, "%s,%d,%d,%d", "itemdrag", this->getUserID(), sel, item +1);

            if (lstrcmpi(ret, "stop") == 0)
               return DL_STOPCURSOR;
            else if (lstrcmpi(ret, "copy") == 0)
               return DL_COPYCURSOR;
            
            return DL_MOVECURSOR;
            break;

         // finish dragging
         case DL_DROPPED:
            // callback DIALOG itemdragfinish THIS_ID SEL_ITEM MOUSE_OVER_ITEM
            item = LBItemFromPt(this->m_Hwnd, dli->ptCursor, TRUE);

            callAliasEx(NULL, "%s,%d,%d,%d", "itemdragfinish", this->getUserID(), sel, item +1);

            if (m_bUseDrawInsert)
               // refresh parent to remove drawing leftovers
               this->m_pParentDialog->redrawWindow();
            else
               this->redrawWindow();

            break;
      }

      return 0L;
   }


	switch( uMsg )
	{
		case WM_COMMAND:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
					switch ( HIWORD( wParam ) ) {
						case LBN_SELCHANGE:
						 {
							 int nItem = ListBox_GetCurSel( this->m_Hwnd );

							 if ( this->isStyle( LBS_MULTIPLESEL ) || this->isStyle( LBS_EXTENDEDSEL ) ) {
								 if ( ListBox_GetSel( this->m_Hwnd, nItem ) > 0 )
									 this->callAliasEx( NULL, "%s,%d,%d", "sclick", this->getUserID( ), nItem + 1 );
							 }
							 else if ( nItem != LB_ERR )
								 this->callAliasEx( NULL, "%s,%d,%d", "sclick", this->getUserID( ), nItem + 1 );
						 }
						 break;

						case LBN_DBLCLK:
						 {
							 int nItem = ListBox_GetCurSel( this->m_Hwnd );

							 if ( this->isStyle( LBS_MULTIPLESEL ) || this->isStyle( LBS_EXTENDEDSEL ) ) { 
								 if ( ListBox_GetSel( this->m_Hwnd, nItem ) > 0 )
									 this->callAliasEx( NULL, "%s,%d,%d", "dclick", this->getUserID( ), nItem + 1 );
							 }
							 else if ( nItem != LB_ERR )
								 this->callAliasEx( NULL, "%s,%d,%d", "dclick", this->getUserID( ), nItem + 1 );
						 }
						 break;
					} // switch ( HIWORD( wParam ) )
				}
			}
			break;
		case WM_DRAWITEM:
			{
				LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;
				int len = ListBox_GetTextLen(lpDrawItem->hwndItem, lpDrawItem->itemID);
				if (len == LB_ERR)
					break;

				TCHAR * szBuf = new TCHAR[len +1];

				ListBox_GetText(lpDrawItem->hwndItem, lpDrawItem->itemID, szBuf);

				TString txt(szBuf);
				RECT rc;
				int clrText = -1;

				CopyRect(&rc, &lpDrawItem->rcItem);

				if (this->m_hBackBrush == NULL)
					FillRect(lpDrawItem->hDC, &rc, GetStockBrush(WHITE_BRUSH));
				else
					DcxControl::DrawCtrlBackground(lpDrawItem->hDC, this, &rc);

				if (lpDrawItem->itemState & ODS_SELECTED) {
					FillRect(lpDrawItem->hDC, &rc, GetSysColorBrush(COLOR_HIGHLIGHT));
					clrText = SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
				}

				rc.left += 2;

				UINT style = DT_LEFT|DT_VCENTER;

				if (this->isStyle(LBS_USETABSTOPS))
					style |= DT_EXPANDTABS;

				calcStrippedRect(lpDrawItem->hDC, txt, style, &rc, false);

				mIRC_DrawText(lpDrawItem->hDC, txt, &rc, style, this->m_bShadowText);

				if (clrText != -1)
					SetTextColor(lpDrawItem->hDC, clrText);

				delete [] szBuf;
				return TRUE;
			}
			break;
		//case WM_MEASUREITEM:
		//	{
		//		LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT) lParam;
		//		//lpMeasureItem->itemHeight = 16;
		//		//return TRUE;
		//	}
		//	break;
	}
	return 0L;
}

LRESULT DcxList::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {
    case WM_LBUTTONUP: // Prevents CommonMessage() handling of this.
    case WM_LBUTTONDBLCLK:
			break;

		case WM_VSCROLL:
			if (LOWORD(wParam) == SB_ENDSCROLL)
				this->callAliasEx(NULL, "%s,%d", "scrollend", this->getUserID());

			break;

		case WM_MOUSEWHEEL:
			SendMessage(this->m_pParentDialog->getHwnd(), uMsg, wParam, lParam);
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
			return this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return 0L;
}

// Draws a horizontal line to insert rather than the arrow
// Ported from http://www.vb-hellfire.de/knowlib/draglist.php
void DcxList::DrawDragLine(int location)
{
   RECT rc;
   HDC  hDC;
   HPEN hPen;
   int  lWidth;

   ListBox_GetItemRect(this->m_Hwnd, location, &rc);

   if (location != m_iLastDrawnLine)
   {
      this->redrawWindow();
      m_iLastDrawnLine = location;
   }

   hDC = GetDC(this->m_Hwnd);
   hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT));

   SelectObject(hDC, hPen);

   // get width
   lWidth = rc.right - rc.left;

   MoveToEx(hDC, 0, rc.top, 0);
   LineTo(hDC, lWidth, rc.top);
   MoveToEx(hDC, 0, rc.top -1, 0);
   LineTo(hDC, lWidth, rc.top -1);

   // Spitze links:
   MoveToEx(hDC, 0, rc.top -3, 0);
   LineTo(hDC, 0, rc.top +3);
   MoveToEx(hDC, 1, rc.top -2, 0);
   LineTo(hDC, 1, rc.top +2);

   // Spitze rechts:
   MoveToEx(hDC, lWidth -1, rc.top -3, 0);
   LineTo(hDC, lWidth -1, rc.top +3);
   MoveToEx(hDC, lWidth -2, rc.top -2, 0);
   LineTo(hDC, lWidth -2, rc.top +2);

   DeleteObject(hPen);
   ReleaseDC(this->m_Hwnd, hDC);
}
