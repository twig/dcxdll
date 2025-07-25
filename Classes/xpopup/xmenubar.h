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

#ifndef _XMENUBAR_H_
#define _XMENUBAR_H_

#include "Classes/xpopup/xpopupmenu.h"

class XMenuBar final
{
public:
	XMenuBar() noexcept = default;
	XMenuBar(const XMenuBar &other) = delete;	// no copy constructor
	//XMenuBar(const XMenuBar&& other) = delete;	// no move constructor

#if !defined(XPOPUP_USE_UNIQUEPTR)
	XMenuBar(const VectorOfXPopupMenu& vpXMenuBar, const TString& tsCallback)
		: m_vpXMenuBar(vpXMenuBar), m_callback(tsCallback)
	{
	}
#endif

	XMenuBar &operator = (const XMenuBar &) = delete;

	bool operator==(const XMenuBar& other) const = default;

	void parseXMenuBarCommand(const TString &input);
	void parseXMenuBarInfo(const TString &input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const;
	TString parseXMenuBarInfo(const TString& input) const;

	const bool addToMenuBar(HMENU menubar, XPopupMenu *const p_Menu, const TString &label);
	void removeFromMenuBar(HMENU menubar, const XPopupMenu *const p_Menu) noexcept;
	void setMenuBar(HMENU oldMenuBar, HMENU newMenuBar);

	void resetMenuBar();

	const bool hasCallback() const noexcept;
	const bool parseCallback(const UINT menuID);

	XPMENUBAR m_Settings;

protected:
	VectorOfXPopupMenu m_vpXMenuBar; //!< Vector of XPopupMenu objects added to the menubar.
	TString m_callback;

	const int findMenuOffset(HMENU menubar, const XPopupMenu *const p_menu) const noexcept;
	static void validateMenu(const XPopupMenu *const menu, const TString &name);
};
#endif // _XMENUBAR_H_
