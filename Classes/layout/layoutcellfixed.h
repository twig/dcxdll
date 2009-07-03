/*!
 * \file layoutcellfixed.h
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

#ifndef _LAYOUTCELLFIXED_H_
#define _LAYOUTCELLFIXED_H_

#include "layoutcell.h"

/*!
 * \brief blah
 *
 * blah
 */

class LayoutCellFixed : public LayoutCell {

public:

  /*!
   * \brief blah
   *
   * blah
   */
  enum FixedType
	{
		HEIGHT=1,         //!< Fixed Size in Height
		WIDTH=HEIGHT<<1,  //!< Fixed Size in Width
		BOTH=HEIGHT|WIDTH //!< Fixed Size in Both
	};

  LayoutCellFixed( DcxControl * dcxc, const FixedType nType = BOTH );
  LayoutCellFixed( const HWND mHwnd, const FixedType nType = BOTH );
  LayoutCellFixed( const HWND mHwnd, const RECT & rc, const FixedType nType = BOTH );
  LayoutCellFixed( const RECT & rc, const FixedType nType = BOTH );
  virtual ~LayoutCellFixed( );

  virtual void LayoutChild( );
  virtual HDWP ExecuteLayout( HDWP hdwp );
  virtual void getMinMaxInfo( CellMinMaxInfo * pCMMI );
  virtual void toXml(TiXmlElement * xml);
  virtual TiXmlElement * toXml(void);

  CellType getType( );

protected:

  FixedType m_nType; //!< Fixed Cell Type

};

#endif // _LAYOUTCELLFIXED_H_

