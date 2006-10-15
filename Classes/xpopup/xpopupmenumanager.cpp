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

void XPopupMenuManager::parseXPopupCommand( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 2, " " ), &flags );

  int numtok = input.numtok( " " );

  XPopupMenu * p_Menu;

  // Special mIRC Menu
  if ( input.gettok( 1, " " ) == "mirc" ) {

    p_Menu = g_mIRCPopupMenu;
  }
  else if ( input.gettok( 1, " " ) == "mircbar" ) {

    p_Menu = g_mIRCMenuBar;
  }
  else if ( ( p_Menu = this->getMenuByName( input.gettok( 1, " " ) ) ) == NULL && flags.switch_flags[2] == 0 ) {

    TString error;
		error.sprintf("\"%s\" doesn't exist : see /xpopup -c", input.gettok( 1, " " ).to_chr( ) );
		DCXError("/xpopup",error.to_chr());
    return;
  }

  // xpopup -b - [MENU] [SWITCH] [FILENAME]
  if ( flags.switch_flags[1] && input.numtok( " " ) > 2 ) {

    HBITMAP hBitmap = (HBITMAP) LoadImage( GetModuleHandle( NULL ), 
      input.gettok( 3, -1, " " ).to_chr( ), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE );

    p_Menu->setBackBitmap( hBitmap );

  }
  // xpopup -c -> [MENU] [SWITCH] [STYLE]
	else if ((flags.switch_flags[2]) &&
		(numtok > 2) && 
		(input.gettok(1, " ") != "mirc" || input.gettok(1, " ") != "mircbar")) {

		if (p_Menu != NULL) {
			TString error;
			error.sprintf("\"%s\" already exists", input.gettok( 1, " " ).to_chr( ) );
			DCXError("/xpopup -c",error.to_chr());
		}
		else {
			XPopupMenu::MenuStyle style = XPopupMenu::XPMS_OFFICE2003;

			if (input.gettok(3, " ") == "office2003rev")
				style = XPopupMenu::XPMS_OFFICE2003_REV;
			else if (input.gettok(3, " ") == "officexp")
				style = XPopupMenu::XPMS_OFFICEXP;
			else if (input.gettok(3, " ") == "icy")
				style = XPopupMenu::XPMS_ICY;
			else if (input.gettok(3, " ") == "icyrev")
				style = XPopupMenu::XPMS_ICY_REV;
			else if (input.gettok(3, " ") == "grade")
				style = XPopupMenu::XPMS_GRADE;
			else if (input.gettok(3, " ") == "graderev")
				style = XPopupMenu::XPMS_GRADE_REV;
			else if (input.gettok(3, " ") == "normal")
				style = XPopupMenu::XPMS_NORMAL;
			else if (input.gettok(3, " ") == "custom")
				style = XPopupMenu::XPMS_CUSTOM;
			else if (input.gettok(3, " ") == "vertical")
				style = XPopupMenu::XPMS_VERTICAL;
			else if (input.gettok(3, " ") == "verticalrev")
				style = XPopupMenu::XPMS_VERTICAL_REV;

			this->m_vpXPMenu.push_back(new XPopupMenu(input.gettok(1, " "), style));
		}
	}
  // xpopup -d -> [MENU] [SWITCH]
  else if ( flags.switch_flags[3] && ( input.gettok( 1, " " ) != "mirc" || input.gettok( 1, " " ) != "mircbar" ) ) {

    this->deleteMenu( p_Menu );
  }
  // xpopup -i -> [MENU] [SWITCH] [INDEX] [FILENAME]
  else if ( flags.switch_flags[8] && numtok > 3 ) {

    HIMAGELIST himl = p_Menu->getImageList( );
    HICON icon;
    int index;

    index = input.gettok( 3, " " ).to_int( );
    TString filename(input.gettok( 4, -1, " " ));
    ExtractIconEx( filename.to_chr( ), index, 0, &icon, 1 );
    ImageList_AddIcon( himl, icon );
    DestroyIcon( icon );
  }
  // xpopup -j -> [MENU] [SWITCH]
  else if ( flags.switch_flags[9] ) {

    p_Menu->destroyImageList( );
  }
  // xpopup -l -> [MENU] [SWITCH] [N] [COLOR]
  else if ( flags.switch_flags[11] && numtok > 3 ) {

    int nColor = input.gettok( 3, " " ).to_int( );
    COLORREF clrColor = (COLORREF)input.gettok( 4, " " ).to_num( );

    p_Menu->setColor( nColor, clrColor );
  }
  // xpopup -p -> [MENU] [SWITCH] [COLORS]
  else if ( flags.switch_flags[15] && numtok > 2 ) {

    TString colors(input.gettok( 3, -1, " " ));
    int i = 1, len = colors.numtok( " " );

    while ( i <= len ) {

      p_Menu->setColor( i, (COLORREF)colors.gettok( i, " " ).to_num( ) );
      ++i;
    }
  }
  // xpopup -s -> [MENU] [SWITCH] [+FLAGS] [X] [Y]
  else if ( flags.switch_flags[18] && numtok > 4 ) {

    UINT flags = this->parseTrackFlags( input.gettok( 3, " " ) );
    int x = input.gettok( 4, " " ).to_int( );
    int y = input.gettok( 5, " " ).to_int( );
    
    UINT ID = TrackPopupMenuEx( p_Menu->getMenuHandle( ), TPM_RETURNCMD | flags, x, y, mhMenuOwner, NULL );

    TString com;
    com.sprintf("//.signal -n XPopup-%s %d", p_Menu->getName( ).to_chr( ), ID );
		mIRCcom(com.to_chr());
  }
	// xpopup -t -> [MENU] [SWITCH] [STYLE]
	else if (flags.switch_flags[19] && numtok > 2) {
		XPopupMenu::MenuStyle style = XPopupMenu::XPMS_OFFICE2003;

		if (input.gettok(3, " ") == "office2003rev")
			style = XPopupMenu::XPMS_OFFICE2003_REV;
		else if (input.gettok(3, " ") == "officexp")
			style = XPopupMenu::XPMS_OFFICEXP;
		else if (input.gettok(3, " ") == "icy")
			style = XPopupMenu::XPMS_ICY;
		else if (input.gettok(3, " ") == "icyrev")
			style = XPopupMenu::XPMS_ICY_REV;
		else if (input.gettok(3, " ") == "grade")
			style = XPopupMenu::XPMS_GRADE;
		else if (input.gettok(3, " ") == "graderev")
			style = XPopupMenu::XPMS_GRADE_REV;
		else if (input.gettok(3, " ") == "vertical")
			style = XPopupMenu::XPMS_VERTICAL;
		else if (input.gettok(3, " ") == "verticalrev")
			style = XPopupMenu::XPMS_VERTICAL_REV;
		else if (input.gettok(3, " ") == "normal")
			style = XPopupMenu::XPMS_NORMAL;
		else if (input.gettok(3, " ") == "custom")
			style = XPopupMenu::XPMS_CUSTOM;

		p_Menu->setStyle(style);
	}
  // xpopup -x -> [MENU] [SWITCH] [+FLAGS]
  else if ( flags.switch_flags[23] && numtok > 2 ) {

    TString flags(input.gettok( 3, " " ));

    if ( flags[0] == '+' ) {

      UINT iStyles = 0;
      int i = 1, len = flags.len( );
      while ( i <= len ) {

        if ( flags[i] == 'i' )
          iStyles |= XPS_ICON3D;
        else if ( flags[i] == 'd' )
          iStyles |= XPS_DISABLEDSEL;
        else if ( flags[i] == 'p' )
          iStyles |= XPS_ICON3DSHADOW;

        ++i;
      }

      p_Menu->setItemStyle( iStyles );
    }
  }
}

/*!
 * \brief blah
 *
 * blah [MENU] [PROP]
 */

void XPopupMenuManager::parseXPopupIdentifier( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );
  TString prop(input.gettok( 2, " " ));

  XPopupMenu * p_Menu;
  if ( ( p_Menu = this->getMenuByName( input.gettok( 1, " " ) ) ) == NULL && prop != "ismenu" ) {

    TString error;
    error.sprintf("\"%s\" doesn't exist, see /xpopup -c", input.gettok( 1, " " ).to_chr( ) );
		DCXError("$ $+ xpopup", error.to_chr());
    return;
  }

  if ( prop == "ismenu" ) {

    lstrcpy( szReturnValue, (p_Menu != NULL)?"$true":"$false" );
    return;
  }
  else if ( prop == "style" ) {

    if ( p_Menu->getStyle( ) == XPopupMenu::XPMS_OFFICE2003  )
      lstrcpy( szReturnValue, "office2003" );
    else if ( p_Menu->getStyle( ) == XPopupMenu::XPMS_OFFICE2003_REV  )
      lstrcpy( szReturnValue, "office2003rev" );
    else if ( p_Menu->getStyle( ) == XPopupMenu::XPMS_OFFICEXP  )
      lstrcpy( szReturnValue, "officeXP" );
    else if ( p_Menu->getStyle( ) == XPopupMenu::XPMS_ICY  )
      lstrcpy( szReturnValue, "icy" );
    else if ( p_Menu->getStyle( ) == XPopupMenu::XPMS_ICY_REV  )
      lstrcpy( szReturnValue, "icyrev" );
    else if ( p_Menu->getStyle( ) == XPopupMenu::XPMS_GRADE  )
      lstrcpy( szReturnValue, "grade" );
    else if ( p_Menu->getStyle( ) == XPopupMenu::XPMS_GRADE_REV  )
      lstrcpy( szReturnValue, "graderev" );
    else if ( p_Menu->getStyle( ) == XPopupMenu::XPMS_NORMAL  )
      lstrcpy( szReturnValue, "normal" );
    else if ( p_Menu->getStyle( ) == XPopupMenu::XPMS_CUSTOM  )
      lstrcpy( szReturnValue, "custom" );

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

    int nColor = input.gettok( 3, " " ).to_int( );
    if ( nColor > 0 && nColor < 11 ) {

      wsprintf( szReturnValue, "%ld", p_Menu->getColor( nColor ) );
      return;
    }
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

XPopupMenu * XPopupMenuManager::getMenuByName( TString tsName ) { 

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

      if ( flags[i] == 'b' )
        iFlags |= TPM_BOTTOMALIGN;
      else if ( flags[i] == 'c' )
        iFlags |= TPM_CENTERALIGN;
      else if ( flags[i] == 'l' )
        iFlags |= TPM_LEFTALIGN;
      else if ( flags[i] == 'm' )
        iFlags |= TPM_LEFTBUTTON;
      else if ( flags[i] == 'n' )
        iFlags |= TPM_RIGHTBUTTON;
      else if ( flags[i] == 'r' )
        iFlags |= TPM_RIGHTALIGN;
      else if ( flags[i] == 't' )
        iFlags |= TPM_TOPALIGN;
      else if ( flags[i] == 'v' )
        iFlags |= TPM_VCENTERALIGN;

      ++i;
    }
  }

  return iFlags;
}