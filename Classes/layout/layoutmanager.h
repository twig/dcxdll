/*!
 * \file layoutmanager.h
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

#ifndef _LAYOUTMANAGER_H_
#define _LAYOUTMANAGER_H_

#include "layoutcell.h"
#include "../tstring/tstring.h"
//#include "tstring.h"

/*!
 * \brief blah
 *
 * blah
 */

class LayoutManager {

public:

  LayoutManager( );
  LayoutManager( HWND mHwnd );
  virtual ~LayoutManager( );

  void updateLayout( RECT & rc );

  void setRoot( LayoutCell * p_Root );
  LayoutCell * getRoot( );

  LayoutCell * getCell( TString & path );

  LayoutCell * parsePath( TString & path, LayoutCell * hParent, int depth );

protected:

  HWND m_Hwnd; //!< Dialog Window Handle

  LayoutCell * m_pRoot; //!< Root LayoutCell Element

};

#endif // _LAYOUTMANAGER_H_