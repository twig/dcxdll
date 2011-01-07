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

#include "Classes/xpopup/xpopupmenu.h"
#include "Classes/tinyxml/tinyxml.h"

/*!
 * \brief blah
 *
 * blah
 */
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class XPopupMenuManager {

private:
	XPopupMenu * m_mIRCPopupMenu;
	XPopupMenu * m_mIRCMenuBar;

	bool m_bIsMenuBar;
	bool m_bIsSysMenu;

	bool m_bIsActiveMircPopup;
	bool m_bIsActiveMircMenubarPopup;


	HMENU m_hMenuCustom;
	HWND m_hMenuOwner; //!< Menu Owner Window Which Processes WM_ Menu Messages 


public:

	XPopupMenuManager( );
	virtual ~XPopupMenuManager( );
	void load(void);
	void unload(void);

	void parseCommand( const TString & input );
	void parseCommand( const TString & input, XPopupMenu *p_Menu );
	void parseIdentifier( const TString & input, TCHAR * szReturnValue );
	int parseMPopup(const TString & input);

	void addMenu( XPopupMenu * p_Menu );
	void deleteMenu( XPopupMenu * p_Menu );
	void clearMenus( );

	void setIsMenuBar(bool value);

	XPopupMenu* getMenuByName(const TString &tsName, BOOL checkSpecial);
	XPopupMenu* getMenuByHandle(const HMENU hMenu);
	XPopupMenu* getmIRCPopup(void);
	XPopupMenu* getmIRCMenuBar(void);
	bool isCustomMenu(const HMENU hMenu);
	bool isMenuBarMenu(const HMENU hMenu, const HMENU hMatch);

	bool isPatched(void) const { return this->m_bPatched; };

	static BOOL InterceptAPI(HMODULE hLocalModule, const TCHAR* c_szDllName, const char* c_szApiName, DWORD dwReplaced, DWORD dwTrampoline, int offset);
	static BOOL WINAPI XTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT * prcRect);
	static BOOL WINAPI XTrackPopupMenuEx(HMENU hMenu, UINT fuFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm);
	static BOOL WINAPI TrampolineTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT * prcRect);
	static BOOL WINAPI TrampolineTrackPopupMenuEx(HMENU hMenu, UINT fuFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm);

	static void LoadPopupsFromXML(TiXmlElement *popups, TiXmlElement *popup, TString &popupName, TString &popupDataset);
	static bool LoadPopupItemsFromXML(XPopupMenu *menu, HMENU hMenu, TiXmlElement *items);

	// following methods are called by dcx's mIRC WinProc

	LRESULT OnInitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnUninitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnExitMenuLoop(HWND mHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND mHwnd, WPARAM wParam, LPARAM lParam);

protected:

	static const char* XPopupMenuManager::GetMenuAttributeFromXML(const char *attrib, TiXmlElement *popup, TiXmlElement *global);

	VectorOfXPopupMenu m_vpXPMenu; //!< Vector of XPopupMenu Objects

	static UINT parseTrackFlags( const TString & flags );

	bool m_bPatched;
#ifdef DEBUG
	static WNDPROC g_OldmIRCMenusWindowProc;
	static LRESULT CALLBACK XPopupMenuManager::mIRCMenusWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
};

extern HMENU g_OriginalMenuBar;
extern XPopupMenu *g_mIRCScriptMenu;

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _XPOPUPMENUMANAGER_H_
