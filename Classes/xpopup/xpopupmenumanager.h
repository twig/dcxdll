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
 * � ScriptsDB.org - 2006
 */

#ifndef _XPOPUPMENUMANAGER_H_
#define _XPOPUPMENUMANAGER_H_

#include "Classes/xpopup/xpopupmenu.h"
#include "Classes/tinyxml/tinyxml.h"

/*
; MN_SETHMENU: = 0x1E0
; MN_GETHMENU: = 0x1E1
; MN_SIZEWINDOW: = 0x1E2
; MN_OPENHIERARCHY: = 0x1E3
; MN_CLOSEHIERARCHY: = 0x1E4
; MN_SELECTITEM: = 0x1E5
; MN_CANCELMENUS: = 0x1E6
; MN_SELECTFIRSTVALIDITEM: = 0x1E7
; MN_GETPPOPUPMENU: = 0x1EA
; MN_FINDMENUWINDOWFROMPOINT: = 0x1EB
; MN_SHOWPOPUPWINDOW: = 0x1EC
; MN_BUTTONDOWN: = 0x1ED
; MN_MOUSEMOVE: = 0x1EE
; MN_BUTTONUP: = 0x1EF
; MN_SETTIMERTOOPENHIERARCHY: = 0x1F0
; MN_DBLCLK: = 0x1F1
#define WM_UAHDESTROYWINDOW 0x0090
#define WM_UAHDRAWMENU 0x0091
#define WM_UAHDRAWMENUITEM 0x0092
#define WM_UAHINITMENU 0x0093
#define WM_UAHMEASUREMENUITEM 0x0094
#define WM_UAHNCPAINTMENUPOPUP 0x0095

q:: ;menus - focus the nth item
vIndex := 3
PostMessage, 0x1E5, % vIndex-1, 0,, ahk_class #32768 ;MN_SELECTITEM := 0x1E5
return

w:: ;menus - invoke the nth item
vIndex := 3
PostMessage, 0x1F1, % vIndex-1, 0,, ahk_class #32768 ;MN_DBLCLK := 0x1F1
return

e:: ;menus - cancel menu and any visible submenus
PostMessage, 0x1E6,,,, ahk_class #32768 ;MN_CANCELMENUS := 0x1E6
return

r:: ;menus - focus the first valid item
PostMessage, 0x1E7,,,, ahk_class #32768 ;MN_SELECTFIRSTVALIDITEM := 0x1E7
return

t:: ;menus - get menu handle
SendMessage, 0x1E1,,,, ahk_class #32768 ;MN_GETHMENU := 0x1E1
hMenu := ErrorLevel
MsgBox, % hMenu
return

*/

/*!
 * \brief blah
 *
 * blah
 */
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class XPopupMenuManager final
{
private:
	std::unique_ptr<XPopupMenu> m_mIRCPopupMenu{ nullptr };
	std::unique_ptr<XPopupMenu> m_mIRCMenuBar{ nullptr };
	std::unique_ptr<XPopupMenu> m_mIRCScriptMenu{ nullptr };

	bool m_bIsMenuBar{ false };
	bool m_bIsSysMenu{ false };

	bool m_bIsActiveMircPopup{ false };
	bool m_bIsActiveMircMenubarPopup{ false };


	HMENU m_hMenuCustom{ nullptr };
	HWND m_hMenuOwner{ nullptr }; //!< Menu Owner Window Which Processes WM_ Menu Messages 

public:

	XPopupMenuManager(const XPopupMenuManager &) = delete;
	XPopupMenuManager &operator = (const XPopupMenuManager &) = delete;
	XPopupMenuManager(XPopupMenuManager &&) = delete;
	XPopupMenuManager &operator =(XPopupMenuManager &&) = delete;

	XPopupMenuManager() noexcept = default;
	~XPopupMenuManager() noexcept = default;

	void load(void);
	void unload(void) noexcept;

	void parseCommand( const TString & input );
	void parseCommand( const TString & input, XPopupMenu *const p_Menu );
	TString parseIdentifier(const TString & input) const;
	const int parseMPopup(const TString & input);

	void addMenu( XPopupMenu *const p_Menu );
	void deleteMenu( const XPopupMenu *const p_Menu ) noexcept;
	void clearMenus( ) noexcept;

	constexpr void setIsMenuBar(const bool value) noexcept { m_bIsMenuBar = value; }

	XPopupMenu* getMenuByHash(const std::size_t uHash, const bool bCheckSpecial) const noexcept;
	XPopupMenu* getMenuByName(const TString &tsName, const bool bCheckSpecial) const noexcept;
	XPopupMenu* getMenuByHandle(const HMENU hMenu) const noexcept;

	XPopupMenu* getmIRCPopup(void) const noexcept { return m_mIRCPopupMenu.get(); }
	XPopupMenu* getmIRCMenuBar(void) const noexcept { return m_mIRCMenuBar.get(); }
	XPopupMenu* getmIRCScriptMenu(void) const noexcept { return m_mIRCScriptMenu.get(); };

	const bool isCustomMenu(const HMENU hMenu) const noexcept;
	static const bool isMenuBarMenu(const HMENU hMenu, const HMENU hMatch);

	static constexpr bool isPatched(void) noexcept { return false; };

	static void LoadPopupsFromXML(const TiXmlElement *const popups, const TiXmlElement *popup, const TString &popupName, const TString &popupDataset);
	static const bool LoadPopupItemsFromXML(XPopupMenu *menu, HMENU hMenu, const TiXmlElement *const items);

	// following methods are called by dcx's mIRC WinProc

	LRESULT OnInitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnUninitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnExitMenuLoop(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT OnCommand(HWND mHwnd, WPARAM wParam, LPARAM lParam);

protected:

	static const TString GetMenuAttributeFromXML(const char *const attrib, const TiXmlElement *const popup, const TiXmlElement *const global);
	static const UINT parseTrackFlags( const TString & flags ) noexcept;

	VectorOfXPopupMenu m_vpXPMenu; //!< Vector of XPopupMenu Objects

#if DCX_CUSTOM_MENUS
	static std::vector<HWND> g_winlist;
	static WNDPROC g_OldmIRCMenusWindowProc;
	static LRESULT CALLBACK mIRCMenusWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
};

extern HMENU g_OriginalMenuBar;

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _XPOPUPMENUMANAGER_H_
