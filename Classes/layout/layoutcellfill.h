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
 * � ScriptsDB.org - 2006
 */

#ifndef _LAYOUTCELLFILL_H_
#define _LAYOUTCELLFILL_H_

#include "Classes/layout/layoutcell.h"

/*!
 * \brief blah
 *
 * blah
 */

class LayoutCellFill final
	: public LayoutCell
{
public:

	using LayoutCell::LayoutCell;

	//LayoutCellFill( );
	//LayoutCellFill(const LayoutCellFill &) = delete;
	//explicit LayoutCellFill( const HWND mHwnd );
	//explicit LayoutCellFill( DcxControl * dcxc );
	//virtual ~LayoutCellFill( );

	//LayoutCellFill &operator =(const LayoutCellFill &) = delete;	// No assignments!

	void LayoutChild() noexcept final {};
	HDWP ExecuteLayout( const HDWP hdwp ) noexcept final;
	void getMinMaxInfo( CellMinMaxInfo *const pCMMI ) const noexcept final;
	void toXml(TiXmlElement *const xml) final;
	TiXmlElement * toXml(void) final;

	const CellType getType() const noexcept final;

};

#endif // _LAYOUTCELLFILL_H_
