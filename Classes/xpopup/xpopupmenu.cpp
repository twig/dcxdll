/*!
 * \file xpopupmenu.cpp
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

#include "xpopupmenu.h"

/*!
 * \brief Constructor
 *
 * blah
 */

XPopupMenu::XPopupMenu( TString & tsMenuName, MenuStyle mStyle ) : m_tsMenuName( tsMenuName ), m_MenuStyle( mStyle ) {

  this->m_hMenu = CreatePopupMenu( );
  this->m_MenuItemStyles = 0;
  this->m_hImageList = NULL;
  this->m_hBitmap = NULL;

  this->m_MenuColors.m_clrBack = RGB( 255, 255, 255 );
  this->m_MenuColors.m_clrBox =  RGB( 184, 199, 146 );
  this->m_MenuColors.m_clrCheckBox = RGB( 255, 128, 0 );
  this->m_MenuColors.m_clrDisabledCheckBox = RGB( 200, 200, 200 );
  this->m_MenuColors.m_clrDisabledSelection = RGB( 255, 255, 255 );
  this->m_MenuColors.m_clrDisabledText = RGB( 128, 128, 128 );
  this->m_MenuColors.m_clrSelection = RGB( 255, 229, 179 );
  this->m_MenuColors.m_clrSelectionBorder = RGB( 0, 0, 0 );
  this->m_MenuColors.m_clrSeparatorLine = RGB( 128, 128, 128 );
  this->m_MenuColors.m_clrText = RGB( 0, 0, 0 );
}

/*!
 * \brief Constructor
 *
 * Default constructor for testing
 */

XPopupMenu::XPopupMenu( HMENU hMenu ) {

  this->m_hMenu = hMenu;
  this->m_MenuItemStyles = 0;
  this->m_MenuStyle = XPMS_OFFICE2003;
  this->m_hImageList = NULL;
  this->m_hBitmap = NULL;
  if ( hMenu != NULL )
    this->m_tsMenuName = "mircbar";
  else
    this->m_tsMenuName = "mirc";

  this->m_MenuColors.m_clrBack = RGB( 255, 255, 255 );
  this->m_MenuColors.m_clrBox =  RGB( 184, 199, 146 );
  this->m_MenuColors.m_clrCheckBox = RGB( 255, 128, 0 );
  this->m_MenuColors.m_clrDisabledCheckBox = RGB( 200, 200, 200 );
  this->m_MenuColors.m_clrDisabledSelection = RGB( 255, 255, 255 );
  this->m_MenuColors.m_clrDisabledText = RGB( 128, 128, 128 );
  this->m_MenuColors.m_clrSelection = RGB( 255, 229, 179 );
  this->m_MenuColors.m_clrSelectionBorder = RGB( 0, 0, 0 );
  this->m_MenuColors.m_clrSeparatorLine = RGB( 128, 128, 128 );
  this->m_MenuColors.m_clrText = RGB( 0, 0, 0 );

  /*

  XPopupMenuItem * p_Item = NULL;
  TString itemtext = "";

  // 
  this->m_hMenu = CreatePopupMenu( );
  MENUITEMINFO mii;
  ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
  mii.cbSize = sizeof( MENUITEMINFO );

  mii.fMask = MIIM_DATA | MIIM_FTYPE;
  mii.fType = MFT_OWNERDRAW;
  itemtext = "Menu Item 1";
  p_Item = new XPopupMenuItem( this, itemtext, 0, FALSE );
  this->m_vpMenuItem.push_back( p_Item );
  mii.dwItemData = (ULONG_PTR) p_Item;
  InsertMenuItem( this->m_hMenu, 1, TRUE, &mii );

  mii.fMask = MIIM_SUBMENU | MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
  mii.fType = MFT_OWNERDRAW;
  HMENU hMenu = CreatePopupMenu( );
  itemtext = "Menu Item 2";
  mii.hSubMenu = hMenu;
  p_Item = new XPopupMenuItem( this, itemtext, 0, TRUE );
  this->m_vpMenuItem.push_back( p_Item );
  mii.dwItemData = (ULONG_PTR) p_Item;
  InsertMenuItem( this->m_hMenu, 2, TRUE, &mii );

  mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
  mii.fType = MFT_OWNERDRAW;
  itemtext = "Menu Item 2 1";
  p_Item = new XPopupMenuItem( this, itemtext, 0, FALSE );
  this->m_vpMenuItem.push_back( p_Item );
  mii.dwItemData = (ULONG_PTR) p_Item;
  InsertMenuItem( hMenu, 1, TRUE, &mii );

  mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
  mii.fType = MFT_SEPARATOR | MFT_OWNERDRAW;
  p_Item = new XPopupMenuItem( this, TRUE );
  this->m_vpMenuItem.push_back( p_Item );
  mii.dwItemData = (ULONG_PTR) p_Item;
  InsertMenuItem( hMenu, 2, TRUE, &mii );

  mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
  mii.fType = MFT_OWNERDRAW;
  itemtext = "Menu Item 2 3";
  mii.fState = MFS_CHECKED;
  p_Item = new XPopupMenuItem( this, itemtext, 0, FALSE );
  this->m_vpMenuItem.push_back( p_Item );
  mii.dwItemData = (ULONG_PTR) p_Item;
  InsertMenuItem( hMenu, 2, TRUE, &mii );

  mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
  mii.fType = MFT_SEPARATOR | MFT_OWNERDRAW;
  mii.fState = 0;
  p_Item = new XPopupMenuItem( this, TRUE );
  this->m_vpMenuItem.push_back( p_Item );
  mii.dwItemData = (ULONG_PTR) p_Item;
  InsertMenuItem( this->m_hMenu, 3, TRUE, &mii );

  mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
  mii.fType = MFT_OWNERDRAW;
  itemtext = "Menu Item 4";
  mii.fState = 0;
  p_Item = new XPopupMenuItem( this, itemtext, 0, FALSE );
  this->m_vpMenuItem.push_back( p_Item );
  mii.dwItemData = (ULONG_PTR) p_Item;
  InsertMenuItem( this->m_hMenu, 4, TRUE, &mii );

  mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
  mii.fType = MFT_OWNERDRAW;
  mii.fState = MFS_CHECKED;
  itemtext = "Menu Item 5";
  p_Item = new XPopupMenuItem( this, itemtext, 0, FALSE );
  this->m_vpMenuItem.push_back( p_Item );
  mii.dwItemData = (ULONG_PTR) p_Item;
  InsertMenuItem( this->m_hMenu, 5, TRUE, &mii );

  mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
  mii.fType = MFT_OWNERDRAW;
  mii.fState = MFS_CHECKED | MFS_DISABLED;
  itemtext = "Menu Item 6";
  p_Item = new XPopupMenuItem( this, itemtext, 0, FALSE );
  this->m_vpMenuItem.push_back( p_Item );
  mii.dwItemData = (ULONG_PTR) p_Item;
  InsertMenuItem( this->m_hMenu, 6, TRUE, &mii );

  mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
  mii.fType = MFT_OWNERDRAW;
  mii.fState = MFS_DISABLED;
  itemtext = "Menu Item 7";
  p_Item = new XPopupMenuItem( this, itemtext, 0, FALSE );
  this->m_vpMenuItem.push_back( p_Item );
  mii.dwItemData = (ULONG_PTR) p_Item;
  InsertMenuItem( this->m_hMenu, 7, TRUE, &mii );
  */
}

/*!
 * \brief Destructor
 *
 * blah
 */

XPopupMenu::~XPopupMenu( ) {

  this->clearAllMenuItems( );

  if ( this->m_hBitmap != NULL )
    DeleteObject( this->m_hBitmap );

  //mIRCError( "XPopupMenu::~XPopupMenu( )" );
  if ( this->m_hMenu != NULL && this->m_tsMenuName != "mircbar" ) {

    //mIRCError( "DestroyMenu( this->m_hMenu );" );
    DestroyMenu( this->m_hMenu );
  }
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::parseXPopCommand( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 2, " " ), &flags );

  TString path = input.gettok( 1, "\t" ).gettok( 3, -1, " " );
  path.trim( );

  HMENU hMenu;
  if ( path.numtok( " " ) == 1 ) {

    hMenu = this->m_hMenu;
  }
  else {

    //mIRCError( path.to_chr( ) );
    //mIRCError( path.gettok( 1, path.numtok( " " ) - 1, " " ).to_chr( ) );

    hMenu = this->parsePath( path.gettok( 1, path.numtok( " " ) - 1, " " ), this->m_hMenu );

    if ( hMenu == NULL ) {

      mIRCError( "/ $+ xpop : Invalid Menu Item Path" );
      return;
    }
    //else {
      //mIRCError( "Found SubMenu" );
    //}
  }

  // xpop -a - [MENU] [SWITCH] [PATH] [TAB] [+FLAGS] [ID] [ICON] ItemText [TAB] Command
  if ( flags.switch_flags[0] && input.numtok( "\t" ) > 1 && input.gettok( 2, "\t" ).numtok( " " ) > 3 ) {

    //mIRCError( input.to_chr( ) );

    TString itemdata = input.gettok( 2, "\t" );
    itemdata.trim( );
    TString itemcom = "";
    if ( input.numtok( "\t" ) > 2 ) {
      itemcom = input.gettok( 3, "\t" );
      itemcom.trim( );
    }

    HMENU hMenu;
    if ( path.numtok( " " ) == 1 ) {

      //mIRCError( "Root" );
      hMenu = this->m_hMenu;
    }
    else {

      hMenu = this->parsePath( path.gettok( 1, path.numtok( " " ) - 1, " " ), this->m_hMenu );

      if ( hMenu == NULL ) {

        //mIRCError( "No SubMenu" );
        return;
      }
      //else {
        //mIRCError( "Found SubMenu" );
      //}
    }

    int nPos = (int)path.gettok( path.numtok( " " ), " " ).to_num( ) - 1;
    int mID = (int)itemdata.gettok( 2, " " ).to_num( );
    int nIcon = (int)itemdata.gettok( 3, " " ).to_num( ) - 1;

    TString flags = itemdata.gettok( 1, " " );
    TString itemtext = itemdata.gettok( 4, -1 , " " );

    if ( nPos == -1 )
      nPos += GetMenuItemCount( hMenu ) + 1;

    MENUITEMINFO mii;
    ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
    mii.cbSize = sizeof( MENUITEMINFO );

    XPopupMenuItem * p_Item = NULL;

    if ( itemtext == "-" ) {

      mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
      mii.fType = MFT_OWNERDRAW | MFT_SEPARATOR;

      p_Item = new XPopupMenuItem( this, TRUE );
    }
    else {

      mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE | MIIM_ID;
      mii.fType = MFT_OWNERDRAW;
      mii.wID = mID;

      if ( flags[0] == '+' ) {

        int i = 1, len = flags.len( );
        while ( i < len ) {

          // submenu
          if ( flags[i] == 's' ) {

            mii.fMask |= MIIM_SUBMENU;
            if ( mii.fMask != NULL )
              DestroyMenu( mii.hSubMenu );

            mii.hSubMenu = CreatePopupMenu( );
          }
          else if ( flags[i] == 'c' ) {
            mii.fState |= MFS_CHECKED;
          }
          else if ( flags[i] == 'g' ) {
            mii.fState |= MFS_GRAYED;
          }

          ++i;
        }
      }
      p_Item = new XPopupMenuItem( this, itemtext, nIcon, mii.hSubMenu!=NULL?TRUE:FALSE );
    }

    this->m_vpMenuItem.push_back( p_Item );
    mii.dwItemData = (ULONG_PTR) p_Item;
    InsertMenuItem( hMenu, nPos, TRUE, &mii );
    /*
      if ( InsertMenuItem( this->m_hMenu, nPos, TRUE, &mii ) == TRUE )
      mIRCError( "Item Inserted" );
    else
      mIRCError( "Item Not Inserted" );
      */
  }
  // xpop -c - [MENU] [SWITCH] [PATH]
  else if ( flags.switch_flags[2] && input.numtok( " " ) > 2 ) {

    int nPos = (int)path.gettok( path.numtok( " " ), " " ).to_num( ) - 1;
    if ( nPos > -1 ) {

      MENUITEMINFO mii;
      ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
      mii.cbSize = sizeof( MENUITEMINFO );
      mii.fMask = MIIM_SUBMENU | MIIM_DATA;

      GetMenuItemInfo( hMenu, nPos, TRUE, &mii );
      //if ( GetMenuItemInfo( hMenu, nPos, TRUE, &mii ) == TRUE )
        //mIRCError( "Got Item Data" );

      if ( mii.hSubMenu != NULL ) {

        this->deleteAllItemData( mii.hSubMenu );
        DestroyMenu( mii.hSubMenu );
      }

      mii.hSubMenu = CreatePopupMenu( );
      
      XPopupMenuItem * p_Item = (XPopupMenuItem *) mii.dwItemData;
      if ( p_Item != NULL )
        p_Item->setSubMenu( TRUE );

      mii.fMask = MIIM_SUBMENU;

      SetMenuItemInfo( hMenu, nPos, TRUE, &mii );
    }
  }
  // xpop -d - [MENU] [SWITCH] [PATH]
  else if ( flags.switch_flags[3] && input.numtok( " " ) > 2 ) {

    int nPos = (int)path.gettok( path.numtok( " " ), " " ).to_num( ) - 1;
    if ( nPos > -1 ) {

      MENUITEMINFO mii;
      ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
      mii.cbSize = sizeof( MENUITEMINFO );
      mii.fMask = MIIM_SUBMENU | MIIM_DATA;

      GetMenuItemInfo( hMenu, nPos, TRUE, &mii );
      //if ( GetMenuItemInfo( hMenu, nPos, TRUE, &mii ) == TRUE )
        //mIRCError( "Got Item Data" );

      if ( mii.hSubMenu != NULL ) {

        //mIRCError( "mii.hSubMenu != NULL" );
        this->deleteAllItemData( mii.hSubMenu );
        DestroyMenu( mii.hSubMenu );
      }

      XPopupMenuItem * p_Item = (XPopupMenuItem *) mii.dwItemData;
      if ( p_Item != NULL )
        p_Item->setSubMenu( FALSE );

      mii.hSubMenu = NULL;
      mii.fMask = MIIM_SUBMENU;

      SetMenuItemInfo( hMenu, nPos, TRUE, &mii );
    }
  }
  // xpop -f - [MENU] [SWITCH] [PATH]
  else if ( flags.switch_flags[5] && input.numtok( " " ) > 2 ) {

    int nPos = (int)path.gettok( path.numtok( " " ), " " ).to_num( ) - 1;
    if ( nPos > -1 ) {

      MENUITEMINFO mii;
      ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
      mii.cbSize = sizeof( MENUITEMINFO );
      mii.fMask = MIIM_SUBMENU | MIIM_DATA;

      GetMenuItemInfo( hMenu, nPos, TRUE, &mii );
      //if ( GetMenuItemInfo( hMenu, nPos, TRUE, &mii ) == TRUE )
        //mIRCError( "Got Item Data" );

      if ( mii.hSubMenu != NULL ) {

        //mIRCError( "mii.hSubMenu != NULL" );
        this->deleteAllItemData( mii.hSubMenu );
        DestroyMenu( mii.hSubMenu );
      }

      XPopupMenuItem * p_Item = (XPopupMenuItem *) mii.dwItemData;
      if ( p_Item != NULL )
        this->deleteMenuItemData( p_Item );

      DeleteMenu( hMenu, nPos, MF_BYPOSITION );
    }
  }
  // xpop -i - [MENU] [SWITCH] [PATH] [TAB] [ICON]
  else if ( flags.switch_flags[8] && input.numtok( "\t" ) > 1 && input.gettok( 2, "\t" ).numtok( " " ) > 0 ) {

    int nPos = (int)path.gettok( path.numtok( " " ), " " ).to_num( ) - 1;
    int nIcon = (int)input.gettok( 2, "\t" ).to_num( );

    if ( nPos > -1 ) {

      MENUITEMINFO mii;
      ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
      mii.cbSize = sizeof( MENUITEMINFO );
      mii.fMask = MIIM_DATA;

      GetMenuItemInfo( hMenu, nPos, TRUE, &mii );
      //if ( GetMenuItemInfo( hMenu, nPos, TRUE, &mii ) == TRUE )
        //mIRCError( "Got Item Data" );

      XPopupMenuItem * p_Item = (XPopupMenuItem *) mii.dwItemData;
      if ( p_Item != NULL )
        p_Item->setItemIcon( nIcon );
    }
  }
  // xpop -s - [MENU] [SWITCH] [PATH] [TAB] [+FLAGS]
  else if ( flags.switch_flags[18] && input.numtok( "\t" ) > 1 && input.gettok( 2, "\t" ).numtok( " " ) > 0 ) {

    int nPos = (int)path.gettok( path.numtok( " " ), " " ).to_num( ) - 1;
    TString flags = input.gettok( 2, "\t" );
    flags.trim( );

    if ( nPos > -1 ) {

      MENUITEMINFO mii;
      ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
      mii.cbSize = sizeof( MENUITEMINFO );
      mii.fMask = MIIM_STATE;

      if ( flags[0] == '+' ) {

        int i = 1, len = flags.len( );
        while ( i < len ) {

          if ( flags[i] == 'c' )
            mii.fState |= MFS_CHECKED;
          else if ( flags[i] == 'g' )
            mii.fState |= MFS_GRAYED;

          ++i;
        }
      }

      SetMenuItemInfo( hMenu, nPos, TRUE, &mii );
    }
  }
  // xpop -t - [MENU] [SWITCH] [PATH] [TAB] Itemtext
  else if ( flags.switch_flags[19] && input.numtok( "\t" ) > 1 && input.gettok( 2, "\t" ).numtok( " " ) > 0 ) {

    int nPos = (int)path.gettok( path.numtok( " " ), " " ).to_num( ) - 1;

    TString itemtext = input.gettok( 2, "\t" );
    itemtext.trim( );

    if ( nPos > -1 ) {

      MENUITEMINFO mii;
      ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
      mii.cbSize = sizeof( MENUITEMINFO );
      mii.fMask = MIIM_DATA | MIIM_STATE | MIIM_SUBMENU | MIIM_FTYPE | MIIM_ID;

      GetMenuItemInfo( hMenu, nPos, TRUE, &mii );
      //if ( GetMenuItemInfo( hMenu, nPos, TRUE, &mii ) == TRUE )
        //mIRCError( "Got Item Data" );

      XPopupMenuItem * p_Item = (XPopupMenuItem *) mii.dwItemData;
      if ( p_Item != NULL )
        p_Item->setItemText( itemtext );
        
      // this is to make sure system resets the measurement of the itemwidth on next display
      DeleteMenu( hMenu, nPos, MF_BYPOSITION );
      InsertMenuItem( hMenu, nPos, TRUE, &mii );
    }
  }
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::parseXPopIdentifier( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );
  TString prop = input.gettok( 2, " " );

  //mIRCError( input.to_chr( ) );

  // [NAME] [ID] [PROP] [PATH]
  if ( prop == "num" && numtok > 2 ) {

    TString path = input.gettok( 3, -1, " " );

    HMENU hMenu;
    if ( path == "root" ) {

      hMenu = this->m_hMenu;
    }
    else {

      hMenu = this->parsePath( path, this->m_hMenu );
    }

    if ( hMenu != NULL ) {
      wsprintf( szReturnValue, "%d", GetMenuItemCount( hMenu ) );
      return;
    }
  }
  else if ( ( prop == "text" || prop == "icon" ) && numtok > 2 ) {

    TString path = input.gettok( 3, -1, " " );

    HMENU hMenu;
      
    if ( path.numtok( " " ) == 1 )
      hMenu = this->m_hMenu;
    else
      hMenu = this->parsePath( path.gettok( 1, path.numtok( " " ) - 1, " " ), this->m_hMenu );

    int nPos = (int)path.gettok( path.numtok( " " ), " " ).to_num( ) - 1;

    if ( hMenu != NULL ) {

      MENUITEMINFO mii;
      ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
      mii.cbSize = sizeof( MENUITEMINFO );
      mii.fMask = MIIM_DATA;

      if ( GetMenuItemInfo( hMenu, nPos, TRUE, &mii ) == TRUE ) {

        XPopupMenuItem * p_Item = (XPopupMenuItem *) mii.dwItemData;
        if ( p_Item != NULL ) {

          if ( prop == "text" ) 
            lstrcpy( szReturnValue, p_Item->getItemText( )->to_chr( ) );
          else if ( prop == "icon" )
            wsprintf( szReturnValue, "%d", p_Item->getItemIcon( ) + 1 );
          return;
        }
      }
    }
  }
  else if ( ( prop == "checked" || prop == "enabled" ) && numtok > 2 ) {

    TString path = input.gettok( 3, -1, " " );

    HMENU hMenu;
      
    if ( path.numtok( " " ) == 1 )
      hMenu = this->m_hMenu;
    else
      hMenu = this->parsePath( path.gettok( 1, path.numtok( " " ) - 1, " " ), this->m_hMenu );

    int nPos = (int)path.gettok( path.numtok( " " ), " " ).to_num( ) - 1;

    if ( hMenu != NULL ) {

      MENUITEMINFO mii;
      ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
      mii.cbSize = sizeof( MENUITEMINFO );
      mii.fMask = MIIM_STATE;

      if ( GetMenuItemInfo( hMenu, nPos, TRUE, &mii ) == TRUE ) {

        if ( prop == "checked" ) 
          lstrcpy( szReturnValue, mii.fState & MFS_CHECKED?"$true":"$false" );
        else if ( prop == "enabled" )
          lstrcpy( szReturnValue, !(mii.fState & MFS_GRAYED)?"$true":"$false" );
        return;
      }
    }
  }
  else if ( prop == "submenu" && numtok > 2 ) {

    TString path = input.gettok( 3, -1, " " );

    HMENU hMenu;
      
    if ( path.numtok( " " ) == 1 )
      hMenu = this->m_hMenu;
    else
      hMenu = this->parsePath( path.gettok( 1, path.numtok( " " ) - 1, " " ), this->m_hMenu );

    int nPos = (int)path.gettok( path.numtok( " " ), " " ).to_num( ) - 1;

    if ( hMenu != NULL ) {

      MENUITEMINFO mii;
      ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
      mii.cbSize = sizeof( MENUITEMINFO );
      mii.fMask = MIIM_SUBMENU;

      if ( GetMenuItemInfo( hMenu, nPos, TRUE, &mii ) == TRUE ) {

        if ( mii.hSubMenu == NULL )
          lstrcpy( szReturnValue, "0" );
        else
          lstrcpy( szReturnValue, "1" );

        return;
      }
    }
  }

  szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST XPopupMenu::getImageList( ) {

  if ( this->m_hImageList == NULL )
    this->m_hImageList = ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );

  return this->m_hImageList;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::destroyImageList( ) {

  if ( this->m_hImageList != NULL )
    ImageList_Destroy( this->m_hImageList );

  this->m_hImageList = NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

XPopupMenu::MenuStyle XPopupMenu::getStyle( ) {

  return this->m_MenuStyle;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::setStyle( MenuStyle style ) {

  this->m_MenuStyle = style;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT XPopupMenu::getItemStyle( ) {

  return this->m_MenuItemStyles;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::setItemStyle( UINT iExStyles ) {

  this->m_MenuItemStyles = iExStyles;
}

/*!
 * \brief blah
 *
 * blah
 */

TString XPopupMenu::getName( ) {

  return this->m_tsMenuName;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::parseSwitchFlags( TString * switchs, XSwitchFlags * flags ) {

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

LPXPMENUCOLORS XPopupMenu::getColors( ) {

  return (LPXPMENUCOLORS) &m_MenuColors;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::setColor( int nColor, COLORREF clrColor ) {

  switch ( nColor ) {

    case 1:
      this->m_MenuColors.m_clrBack = clrColor;
      break;

    case 2:
      this->m_MenuColors.m_clrBox = clrColor;
      break;

    case 3:
      this->m_MenuColors.m_clrCheckBox = clrColor;
      break;

    case 4:
      this->m_MenuColors.m_clrDisabledCheckBox = clrColor;
      break;

    case 5:
      this->m_MenuColors.m_clrDisabledSelection = clrColor;
      break;

    case 6:
      this->m_MenuColors.m_clrDisabledText = clrColor;
      break;

    case 7:
      this->m_MenuColors.m_clrSelection = clrColor;
      break;

    case 8:
      this->m_MenuColors.m_clrSelectionBorder = clrColor;
      break;

    case 9:
      this->m_MenuColors.m_clrSeparatorLine = clrColor;
      break;

    case 10:
      this->m_MenuColors.m_clrText = clrColor;
      break;

    default:
      break;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

COLORREF XPopupMenu::getColor( int nColor ) {

  switch ( nColor ) {

    case 1:
      return this->m_MenuColors.m_clrBack;

    case 2:
      return this->m_MenuColors.m_clrBox;

    case 3:
      return this->m_MenuColors.m_clrCheckBox;

    case 4:
      return this->m_MenuColors.m_clrDisabledCheckBox;

    case 5:
      return this->m_MenuColors.m_clrDisabledSelection;

    case 6:
      return this->m_MenuColors.m_clrDisabledText;

    case 7:
      return this->m_MenuColors.m_clrSelection;

    case 8:
      return this->m_MenuColors.m_clrSelectionBorder;

    case 9:
      return this->m_MenuColors.m_clrSeparatorLine;

    case 10:
      return this->m_MenuColors.m_clrText;

    default:
      return 0;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

HMENU XPopupMenu::parsePath( TString & path, HMENU hParent, int depth ) {

  int iItem = (int)path.gettok( depth, " " ).to_num( ) - 1;

  if ( depth == path.numtok( " " ) )
    return GetSubMenu( hParent, iItem );
  else if ( GetSubMenu( hParent, iItem ) != NULL )
    return this->parsePath( path, GetSubMenu( hParent, iItem ), depth + 1 );

  return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::deleteMenuItemData( XPopupMenuItem * p_Item ) {

  VectorOfXPopupMenuItem::iterator itStart = this->m_vpMenuItem.begin( );
  VectorOfXPopupMenuItem::iterator itEnd = this->m_vpMenuItem.end( );

  while ( itStart != itEnd ) {

    if ( *itStart == p_Item ) {
      
      delete *itStart;
      this->m_vpMenuItem.erase( itStart );
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

void XPopupMenu::deleteAllItemData( HMENU hMenu ) {

  //mIRCError( "Deleting Data" );
  MENUITEMINFO mii;
  ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
  mii.cbSize = sizeof( MENUITEMINFO );
  mii.fMask = MIIM_SUBMENU | MIIM_DATA;

  int i = 0, n = GetMenuItemCount( hMenu );

  while ( i < n ) {

    if ( GetMenuItemInfo( hMenu, i, TRUE, &mii ) == TRUE ) {

      XPopupMenuItem * p_Item = (XPopupMenuItem *) mii.dwItemData;
      if ( p_Item != NULL )
        this->deleteMenuItemData( p_Item );

      if ( mii.hSubMenu != NULL )
        this->deleteAllItemData( mii.hSubMenu );

    }
    ++i;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT CALLBACK XPopupMenu::XPopupMenuProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

  switch ( uMsg ) {

    case WM_NCDESTROY:
      {
        return DefWindowProc( mHwnd, uMsg, wParam, lParam );
      }
      break;

    default:
      return DefWindowProc( mHwnd, uMsg, wParam, lParam );
  }
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT CALLBACK XPopupMenu::XPopupWinProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

  switch ( uMsg ) {

    case WM_MEASUREITEM:
      {
        LPMEASUREITEMSTRUCT lpms = (LPMEASUREITEMSTRUCT) lParam;
        if ( lpms->CtlType == ODT_MENU ) {

          //mIRCError( "XPopup WM_MEASUREITEM" );
          OnMeasureItem( mHwnd, lpms );
          return TRUE; 
        }
      }
      break;

    case WM_DRAWITEM:
      {
        LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
        if ( lpdis->CtlType == ODT_MENU ) {
          //mIRCError( "XPopup WM_DRAWITEM" );
          OnDrawItem( mHwnd, lpdis ); 
          return TRUE; 
        }
      }
      break;

    default:
      return DefWindowProc( mHwnd, uMsg, wParam, lParam );
  }

  return 0;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT XPopupMenu::OnMeasureItem( HWND mHwnd, LPMEASUREITEMSTRUCT lpmis ) {

  XPopupMenuItem * p_Item = (XPopupMenuItem *) lpmis->itemData;

  if ( p_Item != NULL ) {

    //mIRCError( "WM_MEASUREITEM" );

    SIZE size = p_Item->getItemSize( mHwnd );
    lpmis->itemWidth = size.cx;
    lpmis->itemHeight = size.cy;
  }

  return TRUE;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT XPopupMenu::OnDrawItem( HWND mHwnd, LPDRAWITEMSTRUCT lpdis ) {

  XPopupMenuItem * p_Item = (XPopupMenuItem *) lpdis->itemData;

  if ( p_Item != NULL ) {
    //mIRCError( "WM_DRAWITEM" );
	  
    p_Item->DrawItem( lpdis );
  }
  

  return TRUE;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::convertMenu( HMENU hMenu, BOOL bForce ) {

  /*
  char data[500];
  wsprintf( data, "Population of XPopupMenuItem (s) : %d", this->m_vpMenuItem.size( ) );
  mIRCError( data );
  */

  MENUITEMINFO mii;
  ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
  mii.cbSize = sizeof( MENUITEMINFO );
  mii.fMask = MIIM_SUBMENU | MIIM_DATA | MIIM_FTYPE | MIIM_STRING;

  int i = 0, k = 0, n = GetMenuItemCount( hMenu );

  while ( i < n ) {

    char string[500];
    mii.dwTypeData = string;
    mii.cch = 499;

    if ( GetMenuItemInfo( hMenu, i, TRUE, &mii ) == TRUE ) {

      //if ( mii.hSubMenu != NULL )
        //this->convertMenu( mii.hSubMenu, bForce );

      if ( !( mii.fType & MFT_OWNERDRAW ) || bForce == TRUE ) {

        mii.fType |= MFT_OWNERDRAW;
        XPopupMenuItem * p_Item;

        if ( mii.fType & MFT_SEPARATOR ) {

          p_Item = new XPopupMenuItem( this, TRUE );
        }
        else {

          //mIRCError( string );

          TString itemtext( string );
          p_Item = new XPopupMenuItem( this, itemtext, -1, mii.hSubMenu!=NULL?TRUE:FALSE );
        }

        this->m_vpMenuItem.push_back( p_Item );
        mii.dwItemData = (ULONG_PTR) p_Item;
        SetMenuItemInfo( hMenu, i, TRUE, &mii );

        ++k;
      }
    }

    ++i;
  }

  //char data[500];
  //wsprintf( data, "Creating XPopupMenuItem : %d", k );
  //mIRCError( data );
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::cleanMenu( HMENU hMenu ) {

  //mIRCError( this->m_tsMenuName.to_chr( ) );

  /*
  char data[500];
  wsprintf( data, "Cleaning of Menu (s) : %d", this->m_vpMenuItem.size( ) );
  mIRCError( data );
  */
  
  MENUITEMINFO mii;
  ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
  mii.cbSize = sizeof( MENUITEMINFO );
  mii.fMask = MIIM_SUBMENU | MIIM_DATA | MIIM_FTYPE;

  int i = 0, n = GetMenuItemCount( hMenu );

  while ( i < n ) {

    if ( GetMenuItemInfo( hMenu, i, TRUE, &mii ) == TRUE ) {

      if ( mii.hSubMenu != NULL )
        this->cleanMenu( mii.hSubMenu );

      if ( mii.fType & MFT_OWNERDRAW ) {

        mii.fType &= ~MFT_OWNERDRAW;
        mii.dwItemData = NULL;
        SetMenuItemInfo( hMenu, i, TRUE, &mii );
      }
    }

    ++i;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::clearAllMenuItems( ) {

  /*
  mIRCError( this->m_tsMenuName.to_chr( ) );
  
  char data[500];
  wsprintf( data, "Clearing XPopupMenuItem : %d", this->m_vpMenuItem.size( ) );
  mIRCError( data );
  */
  
  VectorOfXPopupMenuItem::iterator itStart = this->m_vpMenuItem.begin( );
  VectorOfXPopupMenuItem::iterator itEnd = this->m_vpMenuItem.end( );

  while ( itStart != itEnd ) {

    if ( *itStart != NULL )
      delete *itStart;

    ++itStart;
  }

  this->m_vpMenuItem.clear( );
}

/*!
 * \brief blah
 *
 * blah
 */

HBITMAP XPopupMenu::getBackBitmap( ) {

  return this->m_hBitmap;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::setBackBitmap( HBITMAP hBitmap ) {

  if ( this->m_hBitmap != NULL )
    DeleteObject(this->m_hBitmap );

  this->m_hBitmap = hBitmap;
}