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
#include "defines.h"
#include "Classes/mirc/dcxlist.h"
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

DcxList::DcxList( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, const TString & styles ) 
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
		TEXT("LISTBOX"), 
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

	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );

	// Check for "draglist" style
	if (styles.find(TEXT("draglist"), 0))
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
				this->showError(NULL, TEXT("DcxList"), TEXT("Error applying draglist style"));
		}
		else
			this->showError(NULL, TEXT("DcxList"), TEXT("Cannot apply draglist style with multi style"));
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

TString DcxList::getStyles(void) const
{
	TString styles(__super::getStyles());
	DWORD Styles;
	Styles = GetWindowStyle(this->m_Hwnd);
	if (Styles & LBS_DISABLENOSCROLL)
		styles.addtok(TEXT("noscroll"));
	if (Styles & LBS_MULTIPLESEL)
		styles.addtok(TEXT("multi"));
	if (Styles & LBS_EXTENDEDSEL)
		styles.addtok(TEXT("extsel"));
	if (Styles & LBS_NOINTEGRALHEIGHT)
		styles.addtok(TEXT("nointegral"));
	if (Styles & LBS_NOSEL)
		styles.addtok(TEXT("nosel"));
	if (Styles & LBS_SORT)
		styles.addtok(TEXT("sort"));
	if (Styles & LBS_USETABSTOPS)
		styles.addtok(TEXT("tabs"));
	if (Styles & LBS_MULTICOLUMN)
		styles.addtok(TEXT("multicol"));
	if (Styles & WS_VSCROLL)
		styles.addtok(TEXT("vsbar"));
	if (Styles & WS_HSCROLL)
		styles.addtok(TEXT("hsbar"));
	if (!this->m_bUseDrawInsert)
		styles.addtok(TEXT("dragline"));
	if (~Styles & LBS_OWNERDRAWFIXED)
		styles.addtok(TEXT("noformat"));
	return styles;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxList::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
{
	unsigned int i = 1;
	const UINT numtok = styles.numtok();
	*Styles |= LBS_NOTIFY | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED;

	while (i <= numtok) {
		if (styles.gettok(i) == TEXT("noscroll"))
			*Styles |= LBS_DISABLENOSCROLL;
		else if (styles.gettok(i) == TEXT("multi"))
			*Styles |= LBS_MULTIPLESEL;
		else if (styles.gettok(i) == TEXT("extsel"))
			*Styles |= LBS_EXTENDEDSEL;
		else if (styles.gettok(i) == TEXT("nointegral"))
			*Styles |= LBS_NOINTEGRALHEIGHT;
		else if (styles.gettok(i) == TEXT("nosel"))
			*Styles |= LBS_NOSEL;
		else if (styles.gettok(i) == TEXT("sort"))
			*Styles |= LBS_SORT;
		else if (styles.gettok(i) == TEXT("tabs"))
			*Styles |= LBS_USETABSTOPS;
		else if (styles.gettok(i) == TEXT("multicol"))
			*Styles |= LBS_MULTICOLUMN;
		else if (styles.gettok(i) == TEXT("vsbar"))
			*Styles |= WS_VSCROLL;
		else if (styles.gettok(i) == TEXT("hsbar"))
			*Styles |= WS_HSCROLL;
		else if (styles.gettok(i) == TEXT("dragline"))
			this->m_bUseDrawInsert = FALSE;
		else if (styles.gettok(i) == TEXT("noformat")) // dont remove from here
			*Styles &= ~LBS_OWNERDRAWFIXED;
		//else if (styles.gettok(i) == TEXT("shadow")) // looks crap
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

void DcxList::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	const int numtok = input.numtok( );

	const TString prop(input.gettok( 3 ));

	// [NAME] [ID] [PROP] [N]
	if ( prop == TEXT("text") && numtok > 3 ) {
		int nSel = input.gettok( 4 ).to_int( ) - 1;
		if ( nSel > -1 && nSel < ListBox_GetCount( this->m_Hwnd ) ) {
			int l = ListBox_GetTextLen(this->m_Hwnd, nSel);
			if (l != LB_ERR && l < MIRC_BUFFER_SIZE_CCH)
				ListBox_GetText( this->m_Hwnd, nSel, szReturnValue );
			else
				this->showError(prop.to_chr(), NULL, TEXT("String Too Long (Greater than Max chars)"));
			return;
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("num") ) {

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), ListBox_GetCount( this->m_Hwnd ) );
		return;
	}
	// [NAME] [ID] [PROP] (N)
	else if (prop == TEXT("sel")) {
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
						value.tsprintf(TEXT("%d"), n);
						lstrcpyn(szReturnValue, value.to_chr(), MIRC_BUFFER_SIZE_CCH);
					}
					else if ((i > 0) && (i <= n)) {
						value.tsprintf(TEXT("%d"), p[i -1] +1);
						lstrcpyn(szReturnValue, value.to_chr(), MIRC_BUFFER_SIZE_CCH);
					}
				}
				else {
					// get all items in a long comma seperated string
					TString path;
					TCHAR num[11];
					int i = 0;

					while (i < n) {
#ifdef VS2005
						_itow(p[i] +1, num, 10);
#else
						itow(p[i] +1, num, 10);
#endif // VS2005
						path.addtok(num, TSCOMMA);

						i++;
					}

					lstrcpyn(szReturnValue, path.to_chr(), MIRC_BUFFER_SIZE_CCH);
				}
				delete [] p;
				return;
			}
		}
		// single select
		else {
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), ListBox_GetCurSel(this->m_Hwnd) +1);
			return;
		}
	}
	// [NAME] [ID] [PROP] [N]
	else if (prop == TEXT("tbitem")) {
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

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), top, bottom);
		return;
	}
	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N]
	else if ( prop == TEXT("find") && numtok > 5 ) {
		TString matchtext(input.gettok(2, TSTAB).trim());
		TString params(input.gettok(3, TSTAB).trim());

		if ( matchtext.len( ) > 0 ) {

			UINT SearchType;

			if ( params.gettok( 1 ) == TEXT("R") )
				SearchType = LBSEARCH_R;
			else
				SearchType = LBSEARCH_W;

			int N = params.gettok( 2 ).to_int( );

			// count total
			if ( N == 0 ) {

				int nItems = ListBox_GetCount( this->m_Hwnd );
				int count = 0;

				for ( int i = 0; i < nItems; i++ ) {

					if ( this->matchItemText( i, &matchtext, SearchType ) )
						count++;
				}

				wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), count );
				return;
			}
			// find Nth matching
			else {

				int nItems = ListBox_GetCount( this->m_Hwnd );
				int count = 0;

				for ( int i = 0; i < nItems; i++ ) {

					if ( this->matchItemText( i, &matchtext, SearchType ) )
						count++;

					// found Nth matching
					if ( count == N ) {

						wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), i + 1 );
						return;
					}
				}
			} // else
		}
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

void DcxList::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.gettok(3));
	const int numtok = input.numtok( );

	//xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')]) {
		ListBox_ResetContent( this->m_Hwnd );
	}

	//xdid -a [NAME] [ID] [SWITCH] [N] [TEXT]
	if ( flags[TEXT('a')] && numtok > 4 ) {

		int nPos = input.gettok( 4 ).to_int( ) - 1;

		if ( nPos == -1 )
			nPos = ListBox_GetCount( this->m_Hwnd );

		ListBox_InsertString( this->m_Hwnd, nPos, input.gettok( 5, -1 ).to_chr( ) );
	}
	//xdid -A [NAME] [ID] [SWITCH] [N] [+FLAGS] [TEXT]
	else if ( flags[TEXT('A')] && numtok > 5 ) {

		int nPos = input.gettok( 4 ).to_int( ) - 1;

		if ( nPos == -1 )
			nPos = ListBox_GetCount( this->m_Hwnd );

		const TString opts(input.gettok( 5 ));
		TString itemtext(input.gettok(6, -1).trim());
		int nMaxStrlen = 0;
		TString tsRes;

		if (opts[0] != TEXT('+')) {
			this->showError(NULL, TEXT("-A"), TEXT("Invalid Flags"));
			return;
		}

		switch (opts[1])
		{
		case TEXT('H'): // [TEXT] == [table] [item]
			{
				if (itemtext.numtok() == 2) { // load single item from hash table by item name
					Dcx::mIRC.tsEvalex(tsRes, TEXT("$hget(%s,%s)"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
					ListBox_InsertString( this->m_Hwnd, nPos, tsRes.to_chr() );
					nMaxStrlen = tsRes.len();
				}
				else
					this->showError(NULL, TEXT("-A +h"), TEXT("Invalid Syntax"));
			}
			break;
		case TEXT('n'): // [TEXT] == [table] [N]
			{
				if (itemtext.numtok() == 2) { // load single item from hash table by index
					Dcx::mIRC.tsEvalex(tsRes, TEXT("$hget(%s,%s).data"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
					ListBox_InsertString( this->m_Hwnd, nPos, tsRes.to_chr() );
					nMaxStrlen = tsRes.len();
				}
				else
					this->showError(NULL, TEXT("-A +n"), TEXT("Invalid Syntax"));
			}
			break;
		case TEXT('t'): // [TEXT] == [table] [startN] [endN]
			{
				if (itemtext.numtok() == 3) { // add contents of a hash table to list
					int max_items = 0, len = 0;
					TString htable(itemtext.gettok( 1 ));
					int startN = itemtext.gettok( 2 ).to_int();
					int endN = itemtext.gettok( 3 ).to_int();

					// get total items in table.
					Dcx::mIRC.tsEvalex(tsRes, TEXT("$hget(%s,0).item"), htable.to_chr());
					max_items = tsRes.to_int();

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
						this->showError(NULL, TEXT("-A +t"), TEXT("Invalid Range"));
						return;
					}

					this->setRedraw(FALSE);
					for (int i = startN; i <= endN; i++) {
						Dcx::mIRC.tsEvalex(tsRes, TEXT("$hget(%s,%d).data"), htable.to_chr(), i);
						ListBox_InsertString( this->m_Hwnd, nPos++, tsRes.to_chr() );
						len = tsRes.len();
						if (len > nMaxStrlen)
							nMaxStrlen = len;
					}
					this->setRedraw(TRUE);
					this->redrawWindow();
				}
				else
					this->showError(NULL, TEXT("-A +t"), TEXT("Invalid Syntax"));
			}
			break;
		case TEXT('f'): // [TEXT] == [startN] [endN] [filename]
			{
				if (itemtext.numtok() > 2) { // add contents of a file to list
					int startN = itemtext.gettok( 1 ).to_int();
					int endN = itemtext.gettok( 2 ).to_int();
					TString filename(itemtext.gettok( 3, -1));

					if (IsFile(filename)) {
						PTCHAR buf = (PTCHAR)readFile(filename.to_chr());
						if (buf != NULL) {
							int max_lines = 0, len = 0;
							TString contents(buf);
							delete [] buf;
							TCHAR *tok = TEXT("\r\n");

							max_lines = contents.numtok(tok);
							if (max_lines == 1) {
								tok = TEXT("\n");
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
								this->showError(NULL, TEXT("-A +f"), TEXT("Invalid Range"));
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
						this->showErrorEx(NULL, TEXT("-A"), TEXT("Unable To Access File: %s"), itemtext.to_chr());
				}
				else
					this->showError(NULL, TEXT("-A +f"), TEXT("Invalid Syntax"));
			}
			break;
		case TEXT('T'): // [TEXT] == [C] [text][c][text]......
			{
				if (itemtext.numtok() > 1) { // add tokens to list
					TCHAR tok[2];
					tok[0] = (TCHAR)itemtext.gettok(1).to_int();
					tok[1] = 0;
					TString contents(itemtext.gettok(2,-1));

					int iNumtok = contents.numtok(tok);
					int len = 0;

					this->setRedraw(FALSE);

					for (int i = 1; i <= iNumtok; i++) {
						itemtext = contents.gettok( i, tok);
						ListBox_InsertString( this->m_Hwnd, nPos++, itemtext.to_chr() );
						len = itemtext.len();
						if (len > nMaxStrlen)
							nMaxStrlen = len;
					}

					this->setRedraw(TRUE);
					this->redrawWindow();
				}
				else
					this->showError(NULL, TEXT("-A +T"), TEXT("Invalid Syntax"));
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
	else if ( flags[TEXT('c')] && numtok > 3 ) {

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
	else if ( flags[TEXT('d')] && numtok > 3 ) {

		int nPos = input.gettok( 4 ).to_int( ) - 1;

		if (nPos == -1)
			nPos = ListBox_GetCount( this->m_Hwnd ) -1;

		if ( nPos > -1 && nPos < ListBox_GetCount( this->m_Hwnd ) )
			ListBox_DeleteString( this->m_Hwnd, nPos );
	}
	// Used to prevent invalid flag message.
	//xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
	}
	//xdid -u [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('u')] ) {

		if ( this->isStyle( LBS_MULTIPLESEL ) || this->isStyle( LBS_EXTENDEDSEL ) )
			ListBox_SetSel( this->m_Hwnd, FALSE, -1 );
		else 
			ListBox_SetCurSel( this->m_Hwnd, -1 );
	}
	//xdid -m [NAME] [ID] [SWITCH] [+FLAGS] [N](,[N]...)
	else if ( flags[TEXT('m')] && numtok > 4 ) {
		TString opts(input.gettok( 4 ));

		if (opts.find(TEXT('w'),0))
			ListBox_SetColumnWidth( this->m_Hwnd, input.gettok( 5 ).to_int( ));
		else if (opts.find(TEXT('t'),0)) {
			TString Ns(input.gettok( 5 ));

			int i = 1, n = Ns.numtok( TSCOMMA );

			if (n == 1) {
				int nTab = Ns.to_int();
				if (nTab < 0)
					ListBox_SetTabStops( this->m_Hwnd, NULL, NULL);
				else
					ListBox_SetTabStops( this->m_Hwnd, 1, &nTab);
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
			this->showError(NULL, TEXT("-m"), TEXT("Invalid Flags"));
	}
	//xdid -o [NAME] [ID] [N] [TEXT]
	else if ( flags[TEXT('o')] ) {
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
      TCHAR ret[20];

      switch (dli->uNotification)
      {
         // begin dragging item
         case DL_BEGINDRAG:
            // callback DIALOG itemdragbegin THIS_ID DRAGGEDITEM
            evalAliasEx(ret, 255, TEXT("%s,%d,%d"), TEXT("itemdragbegin"), this->getUserID(), sel);

            // cancel drag event
            if (lstrcmpi(ret, TEXT("nodrag")) == 0)
               return FALSE;
            
            return TRUE;

         // cancel drag
         case DL_CANCELDRAG:
            // callback DIALOG itemdragcancel THIS_ID DRAGGEDITEM
            evalAliasEx(ret, 255, TEXT("%s,%d,%d"), TEXT("itemdragcancel"), this->getUserID(), sel);

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
            evalAliasEx(ret, 255, TEXT("%s,%d,%d,%d"), TEXT("itemdrag"), this->getUserID(), sel, item +1);

            if (lstrcmpi(ret, TEXT("stop")) == 0)
               return DL_STOPCURSOR;
            else if (lstrcmpi(ret, TEXT("copy")) == 0)
               return DL_COPYCURSOR;
            
            return DL_MOVECURSOR;
            break;

         // finish dragging
         case DL_DROPPED:
            // callback DIALOG itemdragfinish THIS_ID SEL_ITEM MOUSE_OVER_ITEM
            item = LBItemFromPt(this->m_Hwnd, dli->ptCursor, TRUE);

            execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("itemdragfinish"), this->getUserID(), sel, item +1);

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
									 this->execAliasEx(TEXT("%s,%d,%d"), TEXT("sclick"), this->getUserID( ), nItem + 1 );
							 }
							 else if ( nItem != LB_ERR )
								 this->execAliasEx(TEXT("%s,%d,%d"), TEXT("sclick"), this->getUserID( ), nItem + 1 );
						 }
						 break;

						case LBN_DBLCLK:
						 {
							 int nItem = ListBox_GetCurSel( this->m_Hwnd );

							 if ( this->isStyle( LBS_MULTIPLESEL ) || this->isStyle( LBS_EXTENDEDSEL ) ) { 
								 if ( ListBox_GetSel( this->m_Hwnd, nItem ) > 0 )
									 this->execAliasEx(TEXT("%s,%d,%d"), TEXT("dclick"), this->getUserID( ), nItem + 1 );
							 }
							 else if ( nItem != LB_ERR )
								 this->execAliasEx(TEXT("%s,%d,%d"), TEXT("dclick"), this->getUserID( ), nItem + 1 );
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

				bParsed = TRUE;
				TString txt((UINT)len+1);

				ListBox_GetText(lpDrawItem->hwndItem, lpDrawItem->itemID, txt.to_chr());

				RECT rc;
				COLORREF clrText = CLR_INVALID;

				CopyRect(&rc, &lpDrawItem->rcItem);

				if (this->m_hBackBrush == NULL)
					FillRect(lpDrawItem->hDC, &rc, GetStockBrush(WHITE_BRUSH));
				else
					DcxControl::DrawCtrlBackground(lpDrawItem->hDC, this, &rc);

				if (lpDrawItem->itemState & ODS_SELECTED) {
					// fill background with selected colour.
					FillRect(lpDrawItem->hDC, &rc, GetSysColorBrush(COLOR_HIGHLIGHT));
					// draw focus rect around selected item.
					DrawFocusRect(lpDrawItem->hDC, &rc);
					// set selected text colour.
					clrText = SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
				}
				else {
					// set text colour.
					if (this->m_clrText != CLR_INVALID)
						clrText = SetTextColor(lpDrawItem->hDC, this->m_clrText);
				}

				if (len > 0) { // Only do all this if theres any text to draw.
					rc.left += 2;

					UINT style = DT_LEFT|DT_VCENTER|DT_SINGLELINE;

					if (this->isStyle(LBS_USETABSTOPS))
						style |= DT_EXPANDTABS;

					//calcStrippedRect(lpDrawItem->hDC, txt, style, &rc, false);
					this->calcTextRect(lpDrawItem->hDC, txt, &rc, style);

					this->ctrlDrawText(lpDrawItem->hDC, txt, &rc, style);
				}

				if (clrText != CLR_INVALID)
					SetTextColor(lpDrawItem->hDC, clrText);

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
			this->execAliasEx(TEXT("%s,%d"), TEXT("scrollend"), this->getUserID());
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

			bParsed = TRUE;

			// Setup alpha blend if any.
			LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

			LRESULT res = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

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
void DcxList::DrawDragLine(const int location)
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

BOOL DcxList::matchItemText( const int nItem, const TString * search, const UINT SearchType ) const
{
	TCHAR *itemtext;
	BOOL bRes = FALSE;

	int len = ListBox_GetTextLen( this->m_Hwnd, nItem );

	itemtext = new TCHAR[len + 1];

	ListBox_GetText( this->m_Hwnd, nItem, itemtext );

	if (SearchType == LBSEARCH_R)
		bRes = isRegexMatch(itemtext, search->to_chr());
	else
		bRes = TString(itemtext).iswm(search->to_chr());
	delete [] itemtext;
	return bRes;
}
