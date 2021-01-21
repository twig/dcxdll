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
	//void parseIdentifier( const TString & input, TCHAR *const szReturnValue ) const;
	//void parseIdentifier(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const;
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
	static WNDPROC g_OldmIRCMenusWindowProc;
	static LRESULT CALLBACK mIRCMenusWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
};

extern HMENU g_OriginalMenuBar;

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _XPOPUPMENUMANAGER_H_
