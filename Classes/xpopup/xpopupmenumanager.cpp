/*!
 * \file xpopupmenumanager.cpp
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

#include "xpopupmenumanager.h"

extern HWND mhMenuOwner;
extern XPopupMenu * g_mIRCPopupMenu;
extern XPopupMenu * g_mIRCMenuBar;
extern mIRCDLL mIRCLink;

/*!
 * \brief Constructor
 *
 * blah
 */

XPopupMenuManager::XPopupMenuManager( ) {

	this->m_bPatched = false;
}

/*!
 * \brief Destructor
 *
 * blah
 */

XPopupMenuManager::~XPopupMenuManager( ) {

}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::parseSwitchFlags( TString * switchs, XSwitchFlags * flags ) {

  // no -sign, missing params
  if ( (*switchs)[0] != '-' ) 
    return;

  unsigned int i = 1, len = switchs->len( );

  while ( i < len ) {

    if ( (*switchs)[i] >= 'a' && (*switchs)[i] <= 'z' )
      flags->switch_flags[ (int) ( (*switchs)[i] - 'a' ) ] = 1;
    else if ( (*switchs)[i] >= 'A' && (*switchs)[i] <= 'Z' )
      flags->switch_cap_flags[ (int) ( (*switchs)[i] - 'A' ) ] = 1;

    i++;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::parseXPopupCommand( const TString & input ) {
  XPopupMenu * p_Menu;
  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 2 ), &flags );

  // Special mIRC Menu
  if ( input.gettok( 1 ) == "mirc" ) {

    p_Menu = g_mIRCPopupMenu;
  }
  else if ( input.gettok( 1 ) == "mircbar" ) {

    p_Menu = g_mIRCMenuBar;
  }
  else if ( ( p_Menu = this->getMenuByName( input.gettok( 1 ) ) ) == NULL && flags.switch_flags[2] == 0 ) {

    TString error;
		error.sprintf("\"%s\" doesn't exist : see /xpopup -c", input.gettok( 1 ).to_chr( ) );
		DCXError("/xpopup",error.to_chr());
    return;
  }
	parseXPopupCommand(input, p_Menu);
}

void XPopupMenuManager::parseXPopupCommand( const TString & input, XPopupMenu *p_Menu ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 2 ), &flags );

  int numtok = input.numtok( );

	// xpopup -b - [MENU] [SWITCH] [FILENAME]
	if (flags.switch_flags[1]) {
		HBITMAP hBitmap = NULL;

		if (numtok > 2) {
         TString filename(input.gettok(3, -1 ));
         filename.trim();

			if (filename == "none") {
				// ignore 'none' to maintain compatibility
			}
			if (IsFile(filename)) {
				hBitmap = dcxLoadBitmap(hBitmap, filename);

				if (hBitmap == NULL)
					DCXError("/xpopup -b", "Unable to Load Image");
			}
			else
				DCXError("/xpopup -b", "Unable to Access File");
		}

		p_Menu->setBackBitmap( hBitmap );

	}
	// xpopup -c -> [MENU] [SWITCH] [STYLE]
	else if ((flags.switch_flags[2]) && (numtok > 2) && (input.gettok( 1 ) != "mirc" || input.gettok( 1 ) != "mircbar")) {

		if (p_Menu != NULL) {
			TString error;
			error.sprintf("\"%s\" already exists", input.gettok( 1 ).to_chr( ) );
			DCXError("/xpopup -c",error.to_chr());
		}
		else {
			XPopupMenu::MenuStyle style = XPopupMenu::XPMS_OFFICE2003;

			TString tsStyle(input.gettok( 3 ));

			if (tsStyle == "office2003rev")
				style = XPopupMenu::XPMS_OFFICE2003_REV;
			else if (tsStyle == "officexp")
				style = XPopupMenu::XPMS_OFFICEXP;
			else if (tsStyle == "icy")
				style = XPopupMenu::XPMS_ICY;
			else if (tsStyle == "icyrev")
				style = XPopupMenu::XPMS_ICY_REV;
			else if (tsStyle == "grade")
				style = XPopupMenu::XPMS_GRADE;
			else if (tsStyle == "graderev")
				style = XPopupMenu::XPMS_GRADE_REV;
			else if (tsStyle == "vertical")
				style = XPopupMenu::XPMS_VERTICAL;
			else if (tsStyle == "verticalrev")
				style = XPopupMenu::XPMS_VERTICAL_REV;
			else if (tsStyle == "normal")
				style = XPopupMenu::XPMS_NORMAL;
			else if (tsStyle == "custom")
				style = XPopupMenu::XPMS_CUSTOM;

			this->m_vpXPMenu.push_back(new XPopupMenu(input.gettok( 1 ), style));
		}
	}
	// xpopup -d -> [MENU] [SWITCH]
	else if ( flags.switch_flags[3] && ( input.gettok( 1 ) != "mirc" || input.gettok( 1 ) != "mircbar" ) ) {

		this->deleteMenu( p_Menu );
	}
	// xpopup -i -> [MENU] [SWITCH] [FLAGS] [INDEX] [FILENAME]
	else if ( flags.switch_flags[8] && numtok > 4 ) {

		HIMAGELIST himl = p_Menu->getImageList( );
		HICON icon;
		int index;

		index = input.gettok( 4 ).to_int( );
		TString filename(input.gettok( 5, -1 ));
		if (IsFile(filename)) {
			icon = dcxLoadIcon(index, filename, false, input.gettok( 3 ));
			if (icon != NULL) {
				ImageList_AddIcon( himl, icon );
				DestroyIcon( icon );
			}
			else
				DCXError("/xpopup -i","Unable to Load Icon");
		}
		else
			DCXError("/xpopup -i","Unable to Access File");
	}
	// xpopup -j -> [MENU] [SWITCH]
  else if ( flags.switch_flags[9] ) {

    p_Menu->destroyImageList( );
  }
  // xpopup -l -> [MENU] [SWITCH] [N] [COLOR]
  else if ( flags.switch_flags[11] && numtok > 3 ) {

    int nColor = input.gettok( 3 ).to_int( );
    COLORREF clrColor = (COLORREF)input.gettok( 4 ).to_num( );

    p_Menu->setColor( nColor, clrColor );
  }
  // xpopup -m -> mirc -m
  else if ( flags.switch_flags[12] && numtok == 2 && input.gettok( 1 ) == "mirc") {
		if (!this->m_bPatched && mIRCLink.m_bmIRCSixPointTwoZero) {
			XPopupMenuManager::InterceptAPI(GetModuleHandle(NULL), "User32.dll", "TrackPopupMenu", (DWORD)XPopupMenuManager::XTrackPopupMenu, (DWORD)XPopupMenuManager::TrampolineTrackPopupMenu, 5);
			XPopupMenuManager::InterceptAPI(GetModuleHandle(NULL), "User32.dll", "TrackPopupMenuEx", (DWORD)XPopupMenuManager::XTrackPopupMenuEx, (DWORD)XPopupMenuManager::TrampolineTrackPopupMenuEx, 5);
			this->m_bPatched = true;
		}
  }
  // xpopup -p -> [MENU] [SWITCH] [COLORS]
  else if ( flags.switch_flags[15] && numtok > 2 ) {

    TString colors(input.gettok( 3, -1 ));
    int i = 1, len = colors.numtok( );

    while ( i <= len ) {

      p_Menu->setColor( i, (COLORREF)colors.gettok( i ).to_num( ) );
      ++i;
    }
  }
  // xpopup -s -> [MENU] [SWITCH] [+FLAGS] [X] [Y] (OVER HWND)
  else if ( flags.switch_flags[18] && numtok > 4 ) {

    UINT mflags = this->parseTrackFlags( input.gettok( 3 ) );
    int x = input.gettok( 4 ).to_int( );
    int y = input.gettok( 5 ).to_int( );
    
		/*
			TODO: Add offsetting for multiple monitor based on supplied hwnd this menu is to be associated with
		*/
		HWND hTrack = (HWND)input.gettok( 6 ).to_num();

		if (hTrack != NULL && IsWindow(hTrack)) {
			RECT rc;
			GetWindowRect(hTrack, &rc);

			// make pos relative to supplied window.
			x += rc.left;
			y += rc.top;

			//HMONITOR hMon;
			//MONITORINFO mi;
			//hMon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);

			//mi.cbSize = sizeof(mi);
			//GetMonitorInfo(hMon, &mi);

			//x += mi.rcMonitor.left;
			//y += mi.rcMonitor.top;
		}

    UINT ID = TrackPopupMenuEx( p_Menu->getMenuHandle( ), TPM_RETURNCMD | mflags, x, y, mhMenuOwner, NULL );

		mIRCcomEX("//.signal -n XPopup-%s %d", p_Menu->getName( ).to_chr( ), ID );
  }
	// xpopup -t -> [MENU] [SWITCH] [STYLE]
	else if (flags.switch_flags[19] && numtok > 2) {
		XPopupMenu::MenuStyle style = XPopupMenu::XPMS_OFFICE2003;
		TString tsStyle(input.gettok( 3 ));

		if (tsStyle == "office2003rev")
			style = XPopupMenu::XPMS_OFFICE2003_REV;
		else if (tsStyle == "officexp")
			style = XPopupMenu::XPMS_OFFICEXP;
		else if (tsStyle == "icy")
			style = XPopupMenu::XPMS_ICY;
		else if (tsStyle == "icyrev")
			style = XPopupMenu::XPMS_ICY_REV;
		else if (tsStyle == "grade")
			style = XPopupMenu::XPMS_GRADE;
		else if (tsStyle == "graderev")
			style = XPopupMenu::XPMS_GRADE_REV;
		else if (tsStyle == "vertical")
			style = XPopupMenu::XPMS_VERTICAL;
		else if (tsStyle == "verticalrev")
			style = XPopupMenu::XPMS_VERTICAL_REV;
		else if (tsStyle == "normal")
			style = XPopupMenu::XPMS_NORMAL;
		else if (tsStyle == "custom")
			style = XPopupMenu::XPMS_CUSTOM;

		p_Menu->setStyle(style);
	}
  // xpopup -x -> [MENU] [SWITCH] [+FLAGS]
  else if ( flags.switch_flags[23] && numtok > 2 ) {

    TString flag(input.gettok( 3 ));

    if ( flag[0] == '+' ) {

      UINT iStyles = 0;
      int i = 1, len = flag.len( );
      while ( i <= len ) {

        if ( flag[i] == 'i' )
          iStyles |= XPS_ICON3D;
        else if ( flag[i] == 'd' )
          iStyles |= XPS_DISABLEDSEL;
        else if ( flag[i] == 'p' )
          iStyles |= XPS_ICON3DSHADOW;

        ++i;
      }

      p_Menu->setItemStyle( iStyles );
    }
  }
  // xpopup -R -> [MENU] [SWITCH] [+FLAGS] (FLAG OPTIONS)
  else if ( flags.switch_cap_flags[17] && numtok > 2 ) {

		TString flag(input.gettok( 3 ));

		if ( flag[0] == '+' ) {
			switch (flag[1]) {
				case 'r': // Set Rounded Selector on/off
					{
						p_Menu->SetRounded(((input.gettok( 4 ).to_int() > 0) ? true : false));
					}
					break;
				case 'a': // Set Alpha value of menu. 0-255
					{
						UINT alpha = input.gettok( 4 ).to_int();

						if (alpha > 255)
							alpha = 255;

						p_Menu->SetAlpha(alpha);
					}
					break;
				default:
					break;
			}
		}
	}
}

/*!
 * \brief blah
 *
 * blah [MENU] [PROP]
 */

void XPopupMenuManager::parseXPopupIdentifier( const TString & input, char * szReturnValue ) {

  int numtok = input.numtok( );
  TString prop(input.gettok( 2 ));

  XPopupMenu * p_Menu;
  if ( ( p_Menu = this->getMenuByName( input.gettok( 1 ) ) ) == NULL && prop != "ismenu" ) {

    TString error;
    error.sprintf("\"%s\" doesn't exist, see /xpopup -c", input.gettok( 1 ).to_chr( ) );
		DCXError("$!xpopup()", error.to_chr());
    return;
  }

  if ( prop == "ismenu" ) {

    lstrcpy( szReturnValue, (p_Menu != NULL)?"$true":"$false" );
    return;
  }
  else if ( prop == "style" ) {

		switch (p_Menu->getStyle( )) {
			case XPopupMenu::XPMS_OFFICE2003:
				lstrcpy( szReturnValue, "office2003" );
				break;
			case XPopupMenu::XPMS_OFFICE2003_REV:
				lstrcpy( szReturnValue, "office2003rev" );
				break;
			case XPopupMenu::XPMS_OFFICEXP:
				lstrcpy( szReturnValue, "officeXP" );
				break;
			case XPopupMenu::XPMS_ICY:
				lstrcpy( szReturnValue, "icy" );
				break;
			case XPopupMenu::XPMS_ICY_REV:
				lstrcpy( szReturnValue, "icyrev" );
				break;
			case XPopupMenu::XPMS_GRADE:
				lstrcpy( szReturnValue, "grade" );
				break;
			case XPopupMenu::XPMS_GRADE_REV:
				lstrcpy( szReturnValue, "graderev" );
				break;
			case XPopupMenu::XPMS_VERTICAL:
				lstrcpy( szReturnValue, "vertical" );
				break;
			case XPopupMenu::XPMS_VERTICAL_REV:
				lstrcpy( szReturnValue, "verticalrev" );
				break;
			case XPopupMenu::XPMS_NORMAL:
				lstrcpy( szReturnValue, "normal" );
				break;
			case XPopupMenu::XPMS_CUSTOM:
				lstrcpy( szReturnValue, "custom" );
				break;
			default:
				lstrcpy( szReturnValue, "unknown" );
				break;
		}
    return;
  }
  else if ( prop == "exstyle" ) {

    TString styles("+");
    UINT iExStyles = p_Menu->getItemStyle( );

    if ( iExStyles & XPS_ICON3D )
      styles += 'i';
    else if ( iExStyles & XPS_DISABLEDSEL )
      styles += 'd';
    else if ( iExStyles & XPS_ICON3DSHADOW )
      styles += 'p';

    lstrcpy( szReturnValue, styles.to_chr( ) );
    return;
  }
  else if ( prop == "colors" ) {

    wsprintf( szReturnValue, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", p_Menu->getColor( 1 ), p_Menu->getColor( 2 ),
      p_Menu->getColor( 3 ), p_Menu->getColor( 3 ), p_Menu->getColor( 5 ), p_Menu->getColor( 6 ),
      p_Menu->getColor( 7 ), p_Menu->getColor( 8 ), p_Menu->getColor( 9 ), p_Menu->getColor( 10 ) );
    return;

  }
  else if ( prop == "color" && numtok > 2 ) {

    int nColor = input.gettok( 3 ).to_int( );
    if ( nColor > 0 && nColor < 11 ) {

      wsprintf( szReturnValue, "%ld", p_Menu->getColor( nColor ) );
      return;
    }
  }
	else if ( prop == "isrounded") {
		if (p_Menu->IsRounded())
			lstrcpy( szReturnValue, "$true");
		else
			lstrcpy( szReturnValue, "$false");
		return;
	}
	else if ( prop == "alpha") {
		wsprintf( szReturnValue, "%ld", p_Menu->IsAlpha());
		return;
	}
  szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::addMenu( XPopupMenu * p_Menu ) {

  if ( p_Menu != NULL )
    this->m_vpXPMenu.push_back( p_Menu );
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::deleteMenu( XPopupMenu * p_Menu ) {

  VectorOfXPopupMenu::iterator itStart = this->m_vpXPMenu.begin( );
  VectorOfXPopupMenu::iterator itEnd = this->m_vpXPMenu.end( );

  while ( itStart != itEnd ) {

    if ( *itStart == p_Menu ) {

      delete *itStart;
      this->m_vpXPMenu.erase( itStart );
      return;
    }

    ++itStart;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::clearMenus( ) {

  VectorOfXPopupMenu::iterator itStart = this->m_vpXPMenu.begin( );
  VectorOfXPopupMenu::iterator itEnd = this->m_vpXPMenu.end( );

  while ( itStart != itEnd ) {

    if ( *itStart != NULL )
      delete *itStart;

    ++itStart;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

XPopupMenu * XPopupMenuManager::getMenuByName( const TString &tsName ) { 

  VectorOfXPopupMenu::iterator itStart = this->m_vpXPMenu.begin( );
  VectorOfXPopupMenu::iterator itEnd = this->m_vpXPMenu.end( );

  while ( itStart != itEnd ) {

    if ( *itStart != NULL && (*itStart)->getName( ) == tsName )
      return *itStart;

    ++itStart;
  }

  return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT XPopupMenuManager::parseTrackFlags( TString & flags ) {

  UINT iFlags = 0;

  if ( flags[0] == '+' ) {

    int i = 1, len = flags.len( );

    while ( i < len ) {

			switch (flags[i])
			{
			case 'b':
				iFlags |= TPM_BOTTOMALIGN;
				break;
			case 'c':
				iFlags |= TPM_CENTERALIGN;
				break;
			case 'l':
				iFlags |= TPM_LEFTALIGN;
				break;
			case 'm':
				iFlags |= TPM_LEFTBUTTON;
				break;
			case 'n':
				iFlags |= TPM_RIGHTBUTTON;
				break;
			case 'r':
				iFlags |= TPM_RIGHTALIGN;
				break;
			case 't':
				iFlags |= TPM_TOPALIGN;
				break;
			case 'v':
				iFlags |= TPM_VCENTERALIGN;
				break;
			}

      ++i;
    }
  }

  return iFlags;
}
/*
	Following code taken from ODMenu class & modified for XPopup
	CODMenu class
	Code copyright: R.I.Allen for plug-in stuff
	Most owner drawn menu code copyright Brent Corcum - modified by RIA
*/
BOOL WINAPI XPopupMenuManager::XTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT * prcRect)
{
	uFlags &= ~TPM_NONOTIFY;
	return XPopupMenuManager::TrampolineTrackPopupMenu(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
	//bool hooked = false;
	//if (uFlags & TPM_NONOTIFY)
	//{
	//	m_activeObject->m_menuBeingProcessed = hMenu;
	//	CMenu menu;
	//	menu.Attach(hMenu);
	//	m_activeObject->OnInitMenuPopup(&menu, 0, FALSE);
	//	menu.Detach();
	//	// hook the window message queue
	//	// so we can handle the WM_DRAWITEM/WM_MEASUREITEM messages
	//	m_oldWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)MenuWndProc);
	//	hooked = true;
	//}
	//BOOL ret = TrampolineTrackPopupMenu(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
	//if (hooked)
	//{
	//	// restore the old wndProc
	//	m_oldWndProc = (WNDPROC)SetWindowLongPtr(hWnd, 
	//	GWLP_WNDPROC, (LONG_PTR)m_oldWndProc);
	//	m_oldWndProc = NULL;
	//}
	//return ret;
}
BOOL WINAPI XPopupMenuManager::XTrackPopupMenuEx(HMENU hMenu, UINT fuFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm)
{
	fuFlags &= ~TPM_NONOTIFY;
	return XPopupMenuManager::TrampolineTrackPopupMenuEx(hMenu, fuFlags, x, y, hwnd, lptpm);
	//bool hooked = false;
	//if (fuFlags & TPM_NONOTIFY)
	//{
	//	m_activeObject->m_menuBeingProcessed = hMenu;
	//	CMenu menu;
	//	menu.Attach(hMenu);
	//	m_activeObject->OnInitMenuPopup(&menu, 0, FALSE);
	//	menu.Detach();
	//	// hook the window message queue so we can handle
	//	// the WM_DRAWITEM/WM_MEASUREITEM messages
	//	m_oldWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)MenuWndProc);
	//	hooked = true;
	//}
	//BOOL ret = TrampolineTrackPopupMenuEx(hMenu, fuFlags, x, y, hwnd, lptpm);
	//if (hooked)
	//{
	//	// restore the old wndProc
	//	m_oldWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)m_oldWndProc);
	//	m_oldWndProc = NULL;
	//}
	//return ret;
}
BOOL XPopupMenuManager::InterceptAPI(HMODULE hLocalModule, const char* c_szDllName, const char* c_szApiName, DWORD dwReplaced, DWORD dwTrampoline, int offset)
{
	int i;
	DWORD dwOldProtect;
	DWORD dwAddressToIntercept = (DWORD)GetProcAddress(GetModuleHandle((char*)c_szDllName), (char*)c_szApiName);

	BYTE *pbTargetCode = (BYTE *) dwAddressToIntercept;
	BYTE *pbReplaced = (BYTE *) dwReplaced;
	BYTE *pbTrampoline = (BYTE *) dwTrampoline;

	// Change the protection of the trampoline region
	// so that we can overwrite the first 5 + offset bytes.
	if (*pbTrampoline == 0xe9)
	{
		// target function starts with an ansolute jump
		// change tramoline to the target of the jump
		pbTrampoline++;
		int * pbOffset = (int*)pbTrampoline;
		pbTrampoline += *pbOffset + 4;
	}
	VirtualProtect((void *) pbTrampoline, 5+offset, PAGE_WRITECOPY, &dwOldProtect);
	for (i=0;i<offset;i++)
		*pbTrampoline++ = *pbTargetCode++;
	pbTargetCode = (BYTE *) dwAddressToIntercept;

	// Insert unconditional jump in the trampoline.
	*pbTrampoline++ = 0xE9;        // jump rel32
	*((signed int *)(pbTrampoline)) = (pbTargetCode+offset) - (pbTrampoline + 4);
	VirtualProtect((void *) dwTrampoline, 5+offset, PAGE_EXECUTE, &dwOldProtect);

	// Overwrite the first 5 bytes of the target function
	VirtualProtect((void *) dwAddressToIntercept, 5, PAGE_WRITECOPY, &dwOldProtect);

	// check to see whether we need to translate the pbReplaced pointer
	if (*pbReplaced == 0xe9)
	{
		pbReplaced++;
		int * pbOffset = (int*)pbReplaced;
		pbReplaced += *pbOffset + 4;
	}
	*pbTargetCode++ = 0xE9;        // jump rel32
	*((signed int *)(pbTargetCode)) = pbReplaced - (pbTargetCode +4);
	VirtualProtect((void *) dwAddressToIntercept, 5, PAGE_EXECUTE, &dwOldProtect);

	// Flush the instruction cache to make sure 
	// the modified code is executed.
	FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
	return TRUE;
}

BOOL WINAPI XPopupMenuManager::TrampolineTrackPopupMenu(
	HMENU hMenu,         // handle to shortcut menu
	UINT uFlags,         // options
	int x,               // horizontal position
	int y,               // vertical position
	int nReserved,       // reserved, must be zero
	HWND hWnd,           // handle to owner window
	CONST RECT *prcRect  // ignored
)
{
	// this procedure needs to be at least 10 bytes in length
	// it gets overwritten using self modifying code
	// it does not matter what the code is here
	double  a;
	double  b;

	a = 0.0;
	b = 1.0;
	a = a / b;
	return (a > 0);
}

BOOL WINAPI XPopupMenuManager::TrampolineTrackPopupMenuEx(
	HMENU hMenu,       // handle to shortcut menu
	UINT fuFlags,      // options
	int x,             // horizontal position
	int y,             // vertical position
	HWND hwnd,         // handle to window
	LPTPMPARAMS lptpm  // area not to overlap
)
{
	// this procedur eneeds to be at least 10 bytes in length
	// it gets overwritten using self modifying code
	// it does not matter what the code is here
	double  a;
	double  b;

	a = 0.0;
	b = 1.0;
	a = a / b;
	return (a > 0);
}
