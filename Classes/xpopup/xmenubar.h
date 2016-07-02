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

#ifndef _XMENUBAR_H_
#define _XMENUBAR_H_

#include "Classes/xpopup/xpopupmenu.h"

/*!
 * \brief blah
 *
 * blah
 */

class XMenuBar {
	public:
		//XMenuBar( );
		//virtual ~XMenuBar( );

		XMenuBar() = default;
		XMenuBar(const XMenuBar &other) = delete;	// no copy constructor
		XMenuBar &operator = (const XMenuBar &) = delete;

		void parseXMenuBarCommand(const TString &input);
		//void parseXMenuBarInfo(const TString &input, TCHAR *const szReturnValue) const;
		void parseXMenuBarInfo(const TString &input, const refString<TCHAR,MIRC_BUFFER_SIZE_CCH> &szReturnValue) const;

		const bool addToMenuBar(HMENU menubar, XPopupMenu *const p_Menu, const TString &label);
		void removeFromMenuBar(HMENU menubar, const XPopupMenu *const p_Menu);
		void setMenuBar(HMENU oldMenuBar, HMENU newMenuBar);

		void resetMenuBar();

		const bool hasCallback() const noexcept;
		const bool parseCallback(const UINT menuID);

	protected:
		VectorOfXPopupMenu m_vpXMenuBar; //!< Vector of XPopupMenu objects added to the menubar.
		TString m_callback;

		const int findMenuOffset(HMENU menubar, const XPopupMenu *const p_menu) const;
		static void validateMenu(const XPopupMenu *const menu, const TString &name);
};
#endif // _XMENUBAR_H_
