/*!
 * \file xpopupmenumanager.h
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

#ifndef _XPOPUPMENUMANAGER_H_
#define _XPOPUPMENUMANAGER_H_

#include "xpopupmenu.h"

/*!
 * \brief blah
 *
 * blah
 */

class XPopupMenuManager {

public:

  XPopupMenuManager( );
  virtual ~XPopupMenuManager( );

  void parseXPopupCommand( const TString & input );
  void parseXPopupCommand( const TString & input, XPopupMenu *p_Menu );
  void parseXPopupIdentifier( const TString & input, char * szReturnValue );

  void addMenu( XPopupMenu * p_Menu );
  void deleteMenu( XPopupMenu * p_Menu );
  void clearMenus( );

	XPopupMenu* getMenuByName(const TString &tsName, BOOL checkSpecial);
	//XPopupMenu* getMenuByHandle(const HMENU hMenu);

	bool isPatched(void) const { return this->m_bPatched; };
	static BOOL InterceptAPI(HMODULE hLocalModule, const char* c_szDllName, const char* c_szApiName, DWORD dwReplaced, DWORD dwTrampoline, int offset);
	static BOOL WINAPI XTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT * prcRect);
	static BOOL WINAPI XTrackPopupMenuEx(HMENU hMenu, UINT fuFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm);
	static BOOL WINAPI TrampolineTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT * prcRect);
	static BOOL WINAPI TrampolineTrackPopupMenuEx(HMENU hMenu, UINT fuFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm);

protected:

  VectorOfXPopupMenu m_vpXPMenu; //!< Vector of XPopupMenu Objects

  static UINT parseTrackFlags( const TString & flags );

	bool m_bPatched;
};

#endif // _XPOPUPMENUMANAGER_H_
