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
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
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
            DCXError("DcxList", "Error applying draglist style");
      }
      else
         DCXError("DcxList", "Cannot apply draglist style with multi style");
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
   *Styles |= LBS_NOTIFY | LBS_HASSTRINGS;

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
      else if (styles.gettok(i) == "vsbar")
         *Styles |= WS_VSCROLL;
      else if (styles.gettok(i) == "alpha")
         this->m_bAlphaBlend = true;
      else if (styles.gettok(i) == "dragline")
         this->m_bUseDrawInsert = FALSE;

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
				//dcxInfoError(this->getType().to_chr(), "text", this->m_pParentDialog->getName().to_chr(), this->getUserID(), "String Too Long (Greater than 900 chars)");
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
    SendMessage(this->m_Hwnd, LB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0);
  }

  //xdid -a [NAME] [ID] [SWITCH] [N] [TEXT]
  if ( flags.switch_flags[0] && numtok > 4 ) {

    int nPos = input.gettok( 4 ).to_int( ) - 1;

    if ( nPos == -1 )
      nPos += ListBox_GetCount( this->m_Hwnd ) + 1;
    
    ListBox_InsertString( this->m_Hwnd, nPos, input.gettok( 5, -1 ).to_chr( ) );
  }
  //xdid -c [NAME] [ID] [SWITCH] [N,[N,[...]]]
  else if ( flags.switch_flags[2] && numtok > 3 ) {

    if ( this->isStyle( LBS_MULTIPLESEL ) || this->isStyle( LBS_EXTENDEDSEL ) ) {

			TString Ns(input.gettok( 4 ));

      int i = 1, n = Ns.numtok( TSCOMMA ), nItems = ListBox_GetCount( this->m_Hwnd );

      while ( i <= n ) {

        int nSel = Ns.gettok( i, TSCOMMA ).to_int( ) - 1;

        if ( nSel > -1 && nSel < nItems )
          ListBox_SetSel( this->m_Hwnd, TRUE, nSel );

        i++;
      }
    }
    else {

      int nSel = input.gettok( 4 ).to_int( ) - 1;

      if ( nSel > -1 && nSel < ListBox_GetCount( this->m_Hwnd ) )
        ListBox_SetCurSel( this->m_Hwnd, nSel );
    }
  }
  //xdid -d [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[3] && numtok > 3 ) {

    int nPos = input.gettok( 4 ).to_int( ) - 1;

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
  //xdid -o [NAME] [ID] [N] [TEXT]
	else if ( flags.switch_flags[14] ) {
		int nPos = input.gettok( 4 ).to_int() - 1;

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