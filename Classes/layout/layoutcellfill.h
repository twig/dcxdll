/*!
 * \file layoutcellfill.h
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

#ifndef _LAYOUTCELLFILL_H_
#define _LAYOUTCELLFILL_H_

#include "layoutcell.h"

/*!
 * \brief blah
 *
 * blah
 */

class LayoutCellFill : public LayoutCell {

public:

  LayoutCellFill( );
  LayoutCellFill( const HWND mHwnd );
  virtual ~LayoutCellFill( );

  virtual void LayoutChild( );
  virtual HDWP ExecuteLayout( HDWP hdwp );
  virtual void getMinMaxInfo( CellMinMaxInfo * pCMMI );

  CellType getType( );

protected:

};

#endif // _LAYOUTCELLFILL_H_