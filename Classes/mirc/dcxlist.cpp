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
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

//DcxList::DcxList( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
//: DcxControl( ID, p_Dialog ) 
//{
//
//  LONG Styles = 0, ExStyles = 0;
//  BOOL bNoTheme = FALSE;
//  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );
//
//  this->m_Hwnd = CreateWindowEx(	
//    ExStyles | WS_EX_CLIENTEDGE, 
//    "LISTBOX", 
//    NULL,
//    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
//    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
//    p_Dialog->getHwnd( ),
//    (HMENU) ID,
//    GetModuleHandle( NULL ), 
//    NULL);
//
//  if ( bNoTheme )
//    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );
//
//  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
//  this->registreDefaultWindowProc( );
//  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
//
//  DragAcceptFiles(this->m_Hwnd, TRUE);
//}
//
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
: DcxControl( ID, p_Dialog ) 
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

  DragAcceptFiles(this->m_Hwnd, TRUE);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxList::~DcxList( ) {

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxList::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  unsigned int i = 1, numtok = styles.numtok( " " );
  *Styles |= LBS_NOTIFY | LBS_HASSTRINGS;

  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "noscroll" )
      *Styles |= LBS_DISABLENOSCROLL;
    else if ( styles.gettok( i , " " ) == "multi" )
      *Styles |= LBS_MULTIPLESEL;
    else if ( styles.gettok( i , " " ) == "extsel" )
      *Styles |= LBS_EXTENDEDSEL;
    else if ( styles.gettok( i , " " ) == "nointegral" )
      *Styles |= LBS_NOINTEGRALHEIGHT;
    else if ( styles.gettok( i , " " ) == "nosel" )
      *Styles |= LBS_NOSEL;
    else if ( styles.gettok( i , " " ) == "sort" )
      *Styles |= LBS_SORT;
    else if ( styles.gettok( i , " " ) == "tabs" )
      *Styles |= LBS_USETABSTOPS;
	 else if (styles.gettok(i, " ") == "vsbar")
		 *Styles |= WS_VSCROLL;
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

  int numtok = input.numtok( " " );

  // [NAME] [ID] [PROP] [N]
  if ( input.gettok( 3, " " ) == "text" && numtok > 3 ) {

    int nSel = input.gettok( 4, " " ).to_int( ) - 1;

    if ( nSel > -1 && nSel < ListBox_GetCount( this->m_Hwnd ) ) {
     
      ListBox_GetText( this->m_Hwnd, nSel, szReturnValue );
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

    if ( this->isStyle( LBS_MULTIPLESEL ) || this->isStyle( LBS_EXTENDEDSEL ) ) {

      int n = ListBox_GetSelCount( this->m_Hwnd );
      int * p = new int[n];

      if ( n > 0 ) {

        ListBox_GetSelItems( this->m_Hwnd, n, p );

        std::string path = "";

        char num[11];
        int i = 0;

        while ( i < n ) {
#ifdef VS2005
          _itoa( p[i]+1, num, 10 );
#else
          itoa( p[i]+1, num, 10 );
#endif
          path += num;
          path += ',';

          i++;
        }
        delete []p;
        lstrcpy( szReturnValue, path.c_str( ) );
        return;
      }
    }
    else {

      wsprintf( szReturnValue, "%d", ListBox_GetCurSel( this->m_Hwnd ) + 1 );
      return;
    }
  }
	// [NAME] [ID] [PROP] [N]
	else if (input.gettok(3, " ") == "tbitem") {
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
  else if ( input.gettok( 3, " " ) == "find" ) {
    return;
  }
  else if ( this->parseGlobalInfoRequest( input, szReturnValue ) ) {

    return;
  }
  
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
  this->parseSwitchFlags( input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  //xdid -r [NAME] [ID] [SWITCH]
  if (flags.switch_flags[17]) {
    SendMessage(this->m_Hwnd, LB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0);
  }

  //xdid -a [NAME] [ID] [SWITCH] [N] [TEXT]
  if ( flags.switch_flags[0] && numtok > 4 ) {

    int nPos = input.gettok( 4, " " ).to_int( ) - 1;

    if ( nPos == -1 )
      nPos += ListBox_GetCount( this->m_Hwnd ) + 1;
    
    ListBox_InsertString( this->m_Hwnd, nPos, input.gettok( 5, -1, " " ).to_chr( ) );
  }
  //xdid -c [NAME] [ID] [SWITCH] [N,[N,[...]]]
  else if ( flags.switch_flags[2] && numtok > 3 ) {

    if ( this->isStyle( LBS_MULTIPLESEL ) || this->isStyle( LBS_EXTENDEDSEL ) ) {

      TString Ns = input.gettok( 4, " " );
      
      int i = 1, n = Ns.numtok( "," ), nItems = ListBox_GetCount( this->m_Hwnd );

      while ( i <= n ) {

        int nSel = Ns.gettok( i, "," ).to_int( ) - 1;

        if ( nSel > -1 && nSel < nItems )
          ListBox_SetSel( this->m_Hwnd, TRUE, nSel );

        i++;
      }
    }
    else {

      int nSel = input.gettok( 4, " " ).to_int( ) - 1;

      if ( nSel > -1 && nSel < ListBox_GetCount( this->m_Hwnd ) )
        ListBox_SetCurSel( this->m_Hwnd, nSel );
    }
  }
  //xdid -d [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[3] && numtok > 3 ) {

    int nPos = input.gettok( 4, " " ).to_int( ) - 1;

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
		int nPos = input.gettok(4, " ").to_int() - 1;

		if (nPos > -1 && nPos < ListBox_GetCount(this->m_Hwnd)) {
			//ListBox_SetItemData(this->m_Hwnd, nPos, input.to_chr()); //.gettok(5, -1, " ").to_chr());
			ListBox_DeleteString(this->m_Hwnd, nPos);
			ListBox_InsertString(this->m_Hwnd, nPos, input.gettok( 5, -1, " " ).to_chr( ));
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
LRESULT DcxList::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxList::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {
		case WM_HELP:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
					this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
			}
			break;

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
							else {

							if ( nItem != LB_ERR )
							this->callAliasEx( NULL, "%s,%d,%d", "sclick", this->getUserID( ), nItem + 1 );
							}
						}
						break;

						case LBN_DBLCLK:
						{
							int nItem = ListBox_GetCurSel( this->m_Hwnd );

							if ( this->isStyle( LBS_MULTIPLESEL ) || this->isStyle( LBS_EXTENDEDSEL ) ) { 
								if ( ListBox_GetSel( this->m_Hwnd, nItem ) > 0 )
									this->callAliasEx( NULL, "%s,%d,%d", "dclick", this->getUserID( ), nItem + 1 );
							}
							else {
								if ( nItem != LB_ERR )
									this->callAliasEx( NULL, "%s,%d,%d", "dclick", this->getUserID( ), nItem + 1 );
							}
						}
						break;
					} // switch ( HIWORD( wParam ) )
				}
			}
			break;

		case WM_MOUSEMOVE:
			{
				this->m_pParentDialog->setMouseControl( this->getUserID( ) );
			}
			break;

		case WM_CONTEXTMENU:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx( NULL, "%s,%d", "rclick", this->getUserID( ) );
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

		case WM_DROPFILES:
			{
				HDROP files = (HDROP) wParam;
				char filename[500];
				int count = DragQueryFile(files, 0xFFFFFFFF,  filename, 500);

				if (count) {
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_DRAG) {
						char ret[20];

						this->callAliasEx(ret, "%s,%d,%d", "dragbegin", this->getUserID(), count);

						// cancel drag drop event
						if (lstrcmpi(ret, "cancel") == 0) {
							DragFinish(files);
							return 0L;
						}

						// for each file, send callback message
						for (int i = 0; i < count; i++) {
							if (DragQueryFile(files, i, filename, 500))
								this->callAliasEx(ret, "%s,%d,%s", "dragfile", this->getUserID(), filename);
						}

						this->callAliasEx(ret, "%s,%d", "dragfinish", this->getUserID());
					}
				}

				DragFinish(files);
				bParsed = TRUE;
				break;
			}

		case WM_VSCROLL:
			if (LOWORD(wParam) == SB_ENDSCROLL)
				this->callAliasEx(NULL, "%s,%d", "scrollend", this->getUserID());

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

	return 0L;
}