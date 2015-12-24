/*!
 * \file layoutcellpane.h
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

#ifndef _LAYOUTCELLPANE_H_
#define _LAYOUTCELLPANE_H_

#include "Classes/layout/layoutcell.h"

/*!
 * \brief blah
 *
 * blah
 */

class LayoutCellPane : public LayoutCell {

public:

	/*!
	* \brief blah
	*
	* blah
	*/
	enum PaneType: UINT
	{
		HORZ=1,         //!< Horizontal Stacked LayoutCells
		VERT=HORZ<<1   //!< Vertical Stacked LayoutCells
	};

	//LayoutCellPane() = delete;
	LayoutCellPane(const LayoutCellPane &) = delete;
	explicit LayoutCellPane(const PaneType nType = HORZ);
	virtual ~LayoutCellPane( );

	LayoutCellPane &operator =(const LayoutCellPane &) = delete;	// No assignments!

	virtual void LayoutChild() final;
	virtual HDWP ExecuteLayout( const HDWP hdwp ) final;
	virtual void getMinMaxInfo( CellMinMaxInfo * pCMMI ) const final;

	void toXml(TiXmlElement *xml) final;
	TiXmlElement * toXml(void) final;

	const CellType getType( ) const final;

	LayoutCell * addChild(LayoutCell * p_Cell, const UINT nWeight);

protected:

	PaneType m_nType; //!< Fixed Cell Type

	typedef std::pair<LayoutCell *, UINT> CellNode;  //!< Child LayoutCell + Weight Pair
	typedef std::vector<CellNode> VectorOfNodePtrs; //!< Vector of Node pointers

	VectorOfNodePtrs m_vpCells; //!< Vector Of Stacked LayoutCell

	void AdjustMinSize( UINT & nSizeLeft, UINT & nTotalWeight );
	void AdjustSize( UINT & nSizeLeft, UINT & nTotalWeight );
	void AdjustPos( );

};

#endif // _LAYOUTCELLPANE_H_
