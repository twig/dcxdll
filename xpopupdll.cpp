/*!
 * \file dll.cpp
 * \brief Main DLL File
 *
 * This file contains the main XPopup dll routines.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2006
 */

#include "defines.h"
#include "classes/xpopup/xpopupmenumanager.h"
#include "classes/xpopup/xpopupmenu.h"

mIRCDLL mIRCLink; //!< mIRC Linking Structure

XPopupMenuManager g_XPopupMenuManager; //!< Global XPopupMenu Manager

XPopupMenu * g_mIRCPopupMenu;
XPopupMenu * g_mIRCMenuBar;

BOOL isMenuBar;
BOOL isSysMenu;

BOOL bIsActiveMircPopup = FALSE;
BOOL bIsActiveMircMenubarPopup = FALSE;

HWND mhMenuOwner; //!< Menu Owner Window Which Processes WM_ Menu Messages

WNDPROC g_OldmIRCWindowProc;

/*!
 * \brief blah
 *
 * blah
 */
/*
void mIRCError( const char * data ) {

  wsprintf( mIRCLink.m_pData, "//echo -s %s", data );
  SendMessage( mIRCLink.m_mIRCHWND, WM_USER + 200, 0, 0 );
}
*/
/*!
 * \brief mIRC $identifier evaluation function
 *
 * blah
 */
/*
void mIRCeval( const char * data, char * res ) {

  lstrcpy( mIRCLink.m_pData, data );
  SendMessage( mIRCLink.m_mIRCHWND, WM_USER + 201, 0, 0 );
  lstrcpy( res, mIRCLink.m_pData );
}
*/
/*!
 * \brief mIRC /command function
 *
 * blah
 */
/*
void mIRCcom( const char * data ) {

  lstrcpy( mIRCLink.m_pData, data );
  SendMessage( mIRCLink.m_mIRCHWND, WM_USER + 200, 0, 0 );
}
*/
/*!
 * \brief mIRC DLL Load Function
 *
 * This function is called when the DLL is loaded.
 *
 * It initializes all what the DLL needs and links mIRC received information to the mIRCDLL \b mIRCLink
 * data structure to be used later in various functions in the DLL.
 *
 * \param load mIRC Load Structure Pointer
 */

void WINAPI LoadDll( LOADINFO * load ) {

  mIRCLink.m_hFileMap = CreateFileMapping( INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, 4096, "mIRC" );     
  mIRCLink.m_pData = (LPSTR) MapViewOfFile( mIRCLink.m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0 );
  mIRCLink.m_mIRCHWND = load->mHwnd;

  g_OldmIRCWindowProc = (WNDPROC) SetWindowLong( mIRCLink.m_mIRCHWND, GWL_WNDPROC, (LONG) mIRCSubClassWinProc );

  WNDCLASS wc;
  ZeroMemory( &wc, sizeof( WNDCLASS ) );
	wc.hInstance = GetModuleHandle( NULL );
  wc.lpszClassName = XPOPUPMENUCLASS;
  wc.lpfnWndProc = XPopupMenu::XPopupWinProc;
	RegisterClass( &wc );

	mhMenuOwner = CreateWindow( XPOPUPMENUCLASS, NULL, 0, 0, 0, 0, 0, 0, 0, GetModuleHandle(NULL), 0 );

  g_mIRCPopupMenu = new XPopupMenu( NULL );
  g_mIRCMenuBar = new XPopupMenu( GetMenu( mIRCLink.m_mIRCHWND ) );
}

/*!
 * \brief mIRC DLL UnLoad Function
 *
 * This function is called when the DLL is unloaded.
 *
 * It initializes all what the DLL needs and links mIRC received information to the mIRCDLL \b mIRCLink
 * data structure to be used later in various functions in the DLL.
 *
 * \param timeout Unload trigger indicator (0 = timeout unload after 10 min - 1 = exit or /dll -u)
 */

int WINAPI UnloadDll( int timeout ) {

  // DLL unloaded because mIRC exits or /dll -u used
  if ( timeout == 0 ) {

    SetWindowLong( mIRCLink.m_mIRCHWND, GWL_WNDPROC, (LONG) g_OldmIRCWindowProc );

    g_XPopupMenuManager.clearMenus( );

    delete g_mIRCPopupMenu;
    g_mIRCMenuBar->cleanMenu( GetMenu( mIRCLink.m_mIRCHWND ) );
    delete g_mIRCMenuBar;

    if ( mhMenuOwner != NULL )
      DestroyWindow( mhMenuOwner );

    UnregisterClass( XPOPUPMENUCLASS, GetModuleHandle( NULL ) );

    UnmapViewOfFile( mIRCLink.m_pData );
    CloseHandle( mIRCLink.m_hFileMap );

    return 1;
  }
  // keep DLL in memory
  else 
    return 0;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT CALLBACK mIRCSubClassWinProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

  switch ( uMsg ) {

   case WM_INITMENUPOPUP:
     {

       /*
       if ( HIWORD( lParam ) == FALSE && isMenuBarMenu( GetMenu( mHwnd ), (HMENU) wParam ) == FALSE ) {

         mIRCError( "mIRC WM_INITMENUPOPUP" );
         g_mIRCPopupMenu->convertMenu( (HMENU) wParam, FALSE );
         isMenuBar = FALSE;
       }
       else
         isMenuBar = TRUE;
         */

       if ( HIWORD( lParam ) == FALSE ) {

         // let mIRC populate the menus dynamically
         LRESULT lRes = CallWindowProc( g_OldmIRCWindowProc, mHwnd, uMsg, wParam, lParam );

         //mIRCError( "mIRC WM_INITMENUPOPUP" );

         if ( isMenuBarMenu( GetMenu( mHwnd ), (HMENU) wParam ) ) {

           //mIRCError( "Part of the mIRC Menubar" );
           isMenuBar = TRUE;
           if ( bIsActiveMircMenubarPopup == TRUE )
             g_mIRCMenuBar->convertMenu( (HMENU) wParam, TRUE );
         }
         else {
           //mIRCError( "Not Part of the mIRC Menubar" );
           isMenuBar = FALSE;
           if ( bIsActiveMircPopup == TRUE )
             g_mIRCPopupMenu->convertMenu( (HMENU) wParam, FALSE );
         }

         isSysMenu = FALSE;

         return lRes;
       }
       else
         isSysMenu = TRUE;
      
     }
     break;

   case WM_EXITMENULOOP:
     {
       //mIRCError( "mIRC WM_EXITMENULOOP" );
       if ( isMenuBar == FALSE && bIsActiveMircPopup == TRUE )
         g_mIRCPopupMenu->clearAllMenuItems( );

       //return 0L;
     }
     break;

   case WM_UNINITMENUPOPUP:
     {
       //mIRCError( "mIRC WM_UNINITMENUPOPUP" );
       if ( isMenuBar == TRUE && isSysMenu == FALSE && bIsActiveMircMenubarPopup == TRUE )
         g_mIRCMenuBar->deleteAllItemData( (HMENU) wParam );

       //return 0L;
     }
     break;

   case WM_MEASUREITEM:
     {
       LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;
       if ( lpmis->CtlType == ODT_MENU ) {

         XPopupMenuItem * p_Item = (XPopupMenuItem *) lpmis->itemData;

         if ( p_Item != NULL ) {

           SIZE size = p_Item->getItemSize( mHwnd );
           lpmis->itemWidth = size.cx;
           lpmis->itemHeight = size.cy;
           return TRUE; 
         }
       }
     }
     break;

   case WM_DRAWITEM:
     {
       LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
       if ( lpdis->CtlType == ODT_MENU ) {

         XPopupMenuItem * p_Item = (XPopupMenuItem *) lpdis->itemData;

         if ( p_Item != NULL ) {

           p_Item->DrawItem( lpdis );
           return TRUE; 
         }
       }
     }
     break;

   case WM_CLOSE:
     {

       SetWindowLong( mIRCLink.m_mIRCHWND, GWL_WNDPROC, (LONG) g_OldmIRCWindowProc );
       PostMessage( mHwnd, uMsg, 0, 0 );
       return 0L;
     }
     break;

    default:
      break;
  }

  return CallWindowProc( g_OldmIRCWindowProc, mHwnd, uMsg, wParam, lParam );
}

BOOL isMenuBarMenu( HMENU hMenu, HMENU hMatch ) {

  HMENU hTemp;
  int i = 0, n = GetMenuItemCount( hMenu );
  while ( i < n ) {

    if ( ( hTemp = GetSubMenu( hMenu, i ) ) != NULL ) {

      if ( hTemp == hMatch )
        return TRUE;

      if ( isMenuBarMenu( hTemp, hMatch ) == TRUE )
        return TRUE;

    }
    ++i;
  }
  return FALSE;
}

/*!
 * \brief DCX DLL Version Function
 */

mIRC( Version ) {

  wsprintf( data, "XPOPUP DLL %d.%d.%d %s by ClickHeRe ©2006 - http://scriptsdb.org", 
                  XPOP_DLL_VERSION, XPOP_DLL_SUBVERSION, XPOP_DLL_BUILD, XPOP_DLL_STATE );
  return 3;
}

/*!
 * \brief XPopup DLL /xpop Function
 *
 * mIRC /xpop -switch menu path [TAB] optional data
 *
 * Argument \b data contains -> [MENU] [SWITCH] [PATH] [TAB] [OPTION]
 */

mIRC( xpop ) {

  TString d( data );
  d.trim( );

  //mIRCError( d.to_chr( ) );

  if ( d.numtok( " " ) < 3 ) {
    mIRCError( "/ $+ xpop invalid arguments" );
    data[0] = 0;
    return 3;
  }

  if ( d.gettok( 1, " " ) == "mirc" || d.gettok( 1, " " ) == "mircbar" ) {

    mIRCError( "/ $+ xpop invalid menu name : mirc or mircbar menus don't have acces to this feature." );
    data[0] = 0;
    return 3;
  }

  XPopupMenu * p_Menu = g_XPopupMenuManager.getMenuByName( d.gettok( 1, " " ) );
  //mIRCError( d.gettok( 1, " " ).to_chr( ) );

  if ( p_Menu == NULL ) {
    char error[200];
    wsprintf( error, "/ $+ xpop unknown menu \"%s\": see /xpopup -c command", d.gettok( 1, " " ).to_chr( ) );
    mIRCError( error );
    data[0] = 0;
    return 3;
  }

  p_Menu->parseXPopCommand( d );
 
  return 3;
}

/*!
 * \brief XPopup DLL $xpop Function
 *
 * mIRC $xpop(menu, path, options).prop interface
 *
 * Argument \b data contains -> [MENU] [PROP] [PATH] [TAB] [OPTION]
 */

mIRC( _xpop ) {

  TString d( data );
  d.trim( );

  if ( d.numtok( " " ) < 3 ) {
    mIRCError( "$ $+ xpop invalid arguments" );
    data[0] = 0;
    return 3;
  }

  if ( d.gettok( 1, " " ) == "mirc" || d.gettok( 1, " " ) == "mircbar" ) {

    mIRCError( "$ $+ xpop invalid menu name : mirc or mircbar menus don't have acces to this feature." );
    data[0] = 0;
    return 3;
  }

  XPopupMenu * p_Menu = g_XPopupMenuManager.getMenuByName( d.gettok( 1, " " ) );
  //mIRCError( d.gettok( 1, " " ).to_chr( ) );

  if ( p_Menu == NULL ) {
    char error[200];
    wsprintf( error, "$ $+ xpop unknown menu \"%s\": see /xpopup -c command", d.gettok( 1, " " ).to_chr( ) );
    mIRCError( error );
    data[0] = 0;
    return 3;
  }

  p_Menu->parseXPopIdentifier( d, data );

  return 3;
}

/*!
 * \brief XPopup DLL /xpopup Function
 *
 * mIRC /xpopup -switch menu (options)
 *
 * Argument \b data contains -> [MENU] [SWITCH] (OPTIONS)
 */

mIRC( xpopup ) {

  TString d( data );
  d.trim( );

  if ( d.numtok( " " ) < 2 ) {
    mIRCError( "/ $+ xpopup invalid arguments" );
    data[0] = 0;
    return 3;
  }
  
  g_XPopupMenuManager.parseXPopupCommand( d );

  return 3;
}

/*!
 * \brief XPopup DLL $xpopup Function
 *
 * mIRC $xpopup(menu, options).prop interface
 *
 * Argument \b data contains -> [MENU] [PROP] (OPTIONS)
 */

mIRC( _xpopup ) {

  TString d( data );
  d.trim( );

  if ( d.numtok( " " ) < 2 ) {
    mIRCError( "$ $+ xpopup invalid arguments" );
    data[0] = 0;
    return 3;
  }

  g_XPopupMenuManager.parseXPopupIdentifier( d, data );
 
  return 3;
}

/*!
 * \brief XPopup DLL /mpopup Function
 *
 * mIRC /mpopup menuname [0|1]
 *
 * Argument \b data contains -> [MENU] [0|1]
 */

mIRC( mpopup ) {

  TString d( data );
  d.trim( );

  if ( d.numtok( " " ) < 2 ) {
    mIRCError( "/ $+ mpopup invalid arguments" );
    data[0] = 0;
    return 3;
  }

  //BOOL isActiveMircPopup;
  //BOOL isActiveMircMenubarPopup;

  if ( d.gettok( 1, " " ) == "mirc" ) {

    if ( d.gettok( 2, " " ) == "1" ) {
      bIsActiveMircPopup = TRUE;
    }
    else {
      bIsActiveMircPopup = FALSE;
    }
  }
  else if ( d.gettok( 1, " " ) == "mircbar" ) {

    if ( d.gettok( 2, " " ) == "1" ) {
      bIsActiveMircMenubarPopup = TRUE;
    }
    else {
      bIsActiveMircMenubarPopup = FALSE;
      g_mIRCMenuBar->cleanMenu( GetMenu( mIRCLink.m_mIRCHWND ) );
    }

  }
  
  data[0] = 0;
  return 3;
}
