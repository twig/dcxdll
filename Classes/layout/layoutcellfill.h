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

class LayoutCellFill final
	: public LayoutCell
{
public:

	using LayoutCell::LayoutCell;

	void LayoutChild() noexcept final {};
	HDWP ExecuteLayout( const HDWP hdwp ) noexcept final;
	void getMinMaxInfo( CellMinMaxInfo *const pCMMI ) const noexcept final;
	void toXml(TiXmlElement *const xml) final;
	TiXmlElement * toXml(void) final;

	const CellType getType() const noexcept final;

};

#endif // _LAYOUTCELLFILL_H_
