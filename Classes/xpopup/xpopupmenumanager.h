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

  void parseXPopupCommand( TString & input );
  void parseXPopupIdentifier( TString & input, char * szReturnValue );

  void addMenu( XPopupMenu * p_Menu );
  void deleteMenu( XPopupMenu * p_Menu );
  void clearMenus( );
  XPopupMenu * getMenuByName( TString tsName );

protected:

  VectorOfXPopupMenu m_vpXPMenu; //!< Vector of XPopupMenu Objects

  static void parseSwitchFlags( TString * switchs, XSwitchFlags * flags );
  static UINT parseTrackFlags( TString & flags );

};

#endif // _XPOPUPMENUMANAGER_H_