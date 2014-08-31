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

#include "Classes/layout/layoutcell.h"

/*!
 * \brief blah
 *
 * blah
 */

class LayoutCellFill : public LayoutCell {

public:

	LayoutCellFill( );
	explicit LayoutCellFill( const HWND mHwnd );
	explicit LayoutCellFill( DcxControl * dcxc );
	virtual ~LayoutCellFill( );

	virtual void LayoutChild( );
	virtual HDWP ExecuteLayout( HDWP hdwp );
	virtual void getMinMaxInfo( CellMinMaxInfo * pCMMI ) const;
	virtual void toXml(TiXmlElement * xml);
	virtual TiXmlElement * toXml(void);

	CellType getType( ) const;

protected:

};

#endif // _LAYOUTCELLFILL_H_
