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

#include "xpopupmenu.h"

/*!
 * \brief blah
 *
 * blah
 */

class XMenuBar {
	public:
		XMenuBar( );
		virtual ~XMenuBar( );

		void parseXMenuBarCommand(const TString &input);
		void parseXMenuBarInfo(const TString &input, char *szReturnValue);

		bool addToMenuBar(HMENU menubar, XPopupMenu *p_Menu, TString label);
		void removeFromMenuBar(HMENU menubar, XPopupMenu *p_Menu);
		void setMenuBar(HMENU oldMenuBar, HMENU newMenuBar);

		void resetMenuBar();

		bool hasCallback() const;
		bool parseCallback(const UINT menuID);

	protected:
		VectorOfXPopupMenu m_vpXMenuBar; //!< Vector of XPopupMenu objects added to the menubar.

		static UINT parseTrackFlags(TString &flags);

		int findMenuOffset(HMENU menubar, const XPopupMenu *p_menu) const;
		bool validateMenu(const XPopupMenu *menu, const TString &flag, const TString &name) const;

		TString m_callback;
};
#endif // _XMENUBAR_H_
