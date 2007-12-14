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
		void parseXMenuBarIdentifier(const TString &input, char *szReturnValue);

		bool addToMenuBar(HMENU menubar, XPopupMenu *p_Menu, TString label);
		void removeFromMenuBar(HMENU menubar, XPopupMenu *p_Menu);
		void setMenuBar(HMENU oldMenuBar, HMENU newMenuBar);

		void resetMenuBar();

		bool hasCallback();
		bool parseCallback(UINT menuID);

	protected:
		VectorOfXPopupMenu m_vpXMenuBar; //!< Vector of XPopupMenu objects added to the menubar.

		static void parseSwitchFlags(TString *switches, XSwitchFlags *flags);
		static UINT parseTrackFlags(TString &flags);

		int findMenuOffset(HMENU menubar, XPopupMenu *p_menu);
		bool validateMenu(XPopupMenu *menu, TString flag, TString &name);

		TString m_callback;
};
#endif // _XMENUBAR_H_
