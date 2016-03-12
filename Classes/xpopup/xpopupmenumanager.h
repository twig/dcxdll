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

	XPopupMenuManager(const XPopupMenuManager &) = delete;
	XPopupMenuManager &operator = (const XPopupMenuManager &) = delete;

	XPopupMenuManager( );
	virtual ~XPopupMenuManager( );

	void load(void);
	void unload(void);

	void parseCommand( const TString & input );
	void parseCommand( const TString & input, XPopupMenu *const p_Menu );
	void parseIdentifier( const TString & input, TCHAR *const szReturnValue ) const;
	const int parseMPopup(const TString & input);

	void addMenu( XPopupMenu *const p_Menu );
	void deleteMenu( const XPopupMenu *const p_Menu );
	void clearMenus( );

	void setIsMenuBar(const bool value);

	XPopupMenu* getMenuByName(const TString &tsName, const bool bCheckSpecial) const;
	XPopupMenu* getMenuByHandle(const HMENU hMenu) const;
	XPopupMenu* getmIRCPopup(void) const noexcept;
	XPopupMenu* getmIRCMenuBar(void) const noexcept;
	const bool isCustomMenu(const HMENU hMenu) const;
	static const bool isMenuBarMenu(const HMENU hMenu, const HMENU hMatch);

	static const bool isPatched(void) noexcept { return false; };

	static void LoadPopupsFromXML(const TiXmlElement *const popups, const TiXmlElement *popup, const TString &popupName, const TString &popupDataset);
	static const bool LoadPopupItemsFromXML(XPopupMenu *menu, HMENU hMenu, const TiXmlElement *const items);

	// following methods are called by dcx's mIRC WinProc

	LRESULT OnInitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnUninitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnExitMenuLoop(HWND mHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND mHwnd, WPARAM wParam, LPARAM lParam);

protected:

	static const TString GetMenuAttributeFromXML(const char *const attrib, const TiXmlElement *const popup, const TiXmlElement *const global);
	static const UINT parseTrackFlags( const TString & flags );

	VectorOfXPopupMenu m_vpXPMenu; //!< Vector of XPopupMenu Objects

//#ifdef DEBUG
//	static WNDPROC g_OldmIRCMenusWindowProc;
//	static LRESULT CALLBACK mIRCMenusWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//#endif
};

extern HMENU g_OriginalMenuBar;
extern XPopupMenu *g_mIRCScriptMenu;

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _XPOPUPMENUMANAGER_H_
