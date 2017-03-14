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

	using LayoutCell::LayoutCell;

	//LayoutCellFill( );
	//LayoutCellFill(const LayoutCellFill &) = delete;
	//explicit LayoutCellFill( const HWND mHwnd );
	//explicit LayoutCellFill( DcxControl * dcxc );
	//virtual ~LayoutCellFill( );

	LayoutCellFill &operator =(const LayoutCellFill &) = delete;	// No assignments!

	virtual void LayoutChild() final;
	virtual HDWP ExecuteLayout( const HDWP hdwp ) final;
	virtual void getMinMaxInfo( CellMinMaxInfo * pCMMI ) const final;
	virtual void toXml(TiXmlElement * xml) final;
	virtual TiXmlElement * toXml(void) final;

	const CellType getType() const noexcept final;

};

#endif // _LAYOUTCELLFILL_H_
