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

	//XPopupMenuManager(std::unique_ptr<XPopupMenu>& m_mIRCPopupMenu, std::unique_ptr<XPopupMenu>& m_mIRCMenuBar, std::unique_ptr<XPopupMenu>& m_mIRCScriptMenu, bool m_bIsMenuBar, bool m_bIsSysMenu, bool m_bIsActiveMircPopup, bool m_bIsActiveMircMenubarPopup, const HMENU& m_hMenuCustom, const HWND& m_hMenuOwner, const VectorOfXPopupMenu& m_vpXPMenu)
	//	: m_mIRCPopupMenu(m_mIRCPopupMenu), m_mIRCMenuBar(m_mIRCMenuBar), m_mIRCScriptMenu(m_mIRCScriptMenu), m_bIsMenuBar(m_bIsMenuBar), m_bIsSysMenu(m_bIsSysMenu), m_bIsActiveMircPopup(m_bIsActiveMircPopup), m_bIsActiveMircMenubarPopup(m_bIsActiveMircMenubarPopup), m_hMenuCustom(m_hMenuCustom), m_hMenuOwner(m_hMenuOwner), m_vpXPMenu(m_vpXPMenu)
	//{
	//}

	bool operator==(const XPopupMenuManager& other) const = default;

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

	XPopupMenu* getMenuByHash(_In_ const std::size_t uHash, _In_ const bool bCheckSpecial) const noexcept;
	XPopupMenu* getMenuByName(_In_ const TString &tsName, _In_ const bool bCheckSpecial) const noexcept;
	XPopupMenu* getMenuByHandle(_In_opt_ const HMENU hMenu) const noexcept;
	XPopupMenu* getMenuByHWND(_In_opt_ const HWND mHwnd) const noexcept;

	XPopupMenuItem* getMenuItemByID(_In_opt_ const HMENU hMenu, _In_ const int id) const noexcept;
	XPopupMenuItem* getMenuItemByCommandID(_In_opt_ const HMENU hMenu, _In_ const UINT id) const noexcept;

	XPopupMenu* getmIRCPopup(void) const noexcept { return m_mIRCPopupMenu.get(); }
	XPopupMenu* getmIRCMenuBar(void) const noexcept { return m_mIRCMenuBar.get(); }
	XPopupMenu* getmIRCScriptMenu(void) const noexcept { return m_mIRCScriptMenu.get(); };

	/// <summary>
	/// Set the current menus owner.
	/// </summary>
	/// <param name="mHwnd"></param>
	void setOwnerWindow(HWND mHwnd) noexcept {
		m_hOwnerWindow = mHwnd;
	}

	/// <summary>
	/// Get the current menus owner.
	/// </summary>
	/// <returns></returns>
	HWND getOwnerWindow() const noexcept {
		return m_hOwnerWindow;
	}

	/// <summary>
	/// Get the HMENU for the supplied menus HWND.
	/// </summary>
	/// <param name="mHwnd"></param>
	/// <returns></returns>
	static HMENU getWindowsMenu(_In_opt_ HWND mHwnd) noexcept;

	static HWND getHWNDfromHMENU(_In_opt_ HMENU hMenu) noexcept;

	/// <summary>
	/// Get list of open menu windows.
	/// </summary>
	/// <returns></returns>
	static std::vector<HWND>& getGlobalMenuWindowList() noexcept;

	/// <summary>
	/// Get the topmost menus HWND
	/// </summary>
	/// <returns></returns>
	static HWND getBackWin();

	/// <summary>
	/// Get the first menus HWND
	/// </summary>
	/// <returns></returns>
	static HWND getFirstWin();

	/// <summary>
	/// Add a new topmost menu to the list.
	/// </summary>
	/// <param name="hwnd"></param>
	static void AddBackWin(HWND hwnd);

	/// <summary>
	/// Remove the topmost menu.
	/// </summary>
	static void RemoveBackWin();

	/// <summary>
	/// Trigger a specific menu item.
	/// </summary>
	/// <param name="hOwner"></param>
	/// <param name="hMenu"></param>
	/// <param name="mPos"></param>
	/// <param name="bByPos"></param>
	static void TriggerMenuItem(_In_opt_ HWND hOwner, _In_opt_ HMENU hMenu, _In_ UINT mPos, _In_ bool bByPos) noexcept;

	/// <summary>
	/// Trigger a specific menu item, based on its position.
	/// </summary>
	/// <param name="hOwner"></param>
	/// <param name="hMenu"></param>
	/// <param name="mPos"></param>
	static void TriggerMenuPos(_In_opt_ HWND hOwner, _In_opt_ HMENU hMenu, _In_ UINT mPos) noexcept
	{
		TriggerMenuItem(hOwner, hMenu, mPos, true);
	}

	/// <summary>
	/// Trigger a specific menu item, based onm its command id.
	/// </summary>
	/// <param name="hOwner"></param>
	/// <param name="mCmd"></param>
	static void TriggerMenuCommand(_In_opt_ HWND hOwner, _In_ UINT mCmd) noexcept
	{
		TriggerMenuItem(hOwner, nullptr, mCmd, false);
	}

	/// <summary>
	/// Set the menu to its default alpha value.
	/// This is the alpha value used when no other effects are being applied.
	/// </summary>
	/// <param name="mHwnd"></param>
	static void SetMenuAlphaToDefault(_In_opt_ HWND mHwnd) noexcept;

	/// <summary>
	/// Set the menus to its inactive alpha value.
	/// This is the alpha used when the mouse pointer is not over this menu, but is over some other menu.
	/// </summary>
	/// <param name="mHwnd"></param>
	static void SetMenuAlphaToInactive(_In_opt_ HWND mHwnd) noexcept;

	/// <summary>
	/// Set a menu items check state.
	/// </summary>
	/// <param name="hMenu"></param>
	/// <param name="mPos"></param>
	/// <param name="bByPos"></param>
	/// <param name="bCheck"></param>
	static void setCheckState(_In_opt_ HMENU hMenu, _In_ UINT mPos, _In_ BOOL bByPos, _In_ bool bCheck) noexcept;

	const bool isCustomMenu(_In_opt_ const HMENU hMenu) const noexcept;
	static const bool isMenuBarMenu(_In_opt_ const HMENU hMenu, _In_opt_ const HMENU hMatch);
	const bool isItemValid(_In_opt_ const XPopupMenuItem* const pItem) const noexcept;
	static constexpr bool isPatched(void) noexcept { return false; };

	static void LoadPopupsFromXML(const TiXmlElement *const popups, const TiXmlElement *popup, const TString &popupName, const TString &popupDataset);
	static const bool LoadPopupItemsFromXML(XPopupMenu *menu, HMENU hMenu, const TiXmlElement *const items);

	// following methods are called by dcx's mIRC WinProc

	LRESULT OnInitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnUninitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT OnExitMenuLoop(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT OnCommand(HWND mHwnd, WPARAM wParam, LPARAM lParam);

	/// <summary>
	/// Redraw all open menus.
	/// </summary>
	static void RedrawMenuIfOpen() noexcept;

	static void setMenuRegion(_In_opt_ HWND win) noexcept;

	static void setMenuRegionIfOpen() noexcept;

	static void RedrawMenuItem(_In_opt_ HWND mHwnd, _In_opt_ HMENU hMenu, _In_ UINT mID) noexcept;

	static inline std::map<HMENU, XPopupMenu*> m_vpAllMenus;	//!< Vector of all open XPopupMenu Objects

private:
	XPopupMenuItem* _getMenuItemByID(_In_opt_ const HMENU hMenu, _In_ const UINT id, _In_ BOOL bByPos) const noexcept;

protected:

	static const TString GetMenuAttributeFromXML(const char *const attrib, const TiXmlElement *const popup, const TiXmlElement *const global);
	static const UINT parseTrackFlags( const TString & flags ) noexcept;

	VectorOfXPopupMenu m_vpXPMenu;	//!< Vector of XPopupMenu Objects (custom menus only)
	HWND m_hOwnerWindow{};			//!< Menus owner window. (set in WM_INITMENU)

#if DCX_CUSTOM_MENUS
	static HWND CreateTrackingToolTip(int toolID, HWND hDlg, WCHAR* pText) noexcept;
	static void dcxCheckMenuHover() noexcept;
	static void dcxCheckMenuHover2(HWND win) noexcept;
	static LRESULT CALLBACK mIRCMenusWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
};

extern HMENU g_OriginalMenuBar;

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _XPOPUPMENUMANAGER_H_
