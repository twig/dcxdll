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
	enum PaneType
	{
		HORZ=1,         //!< Horizontal Stacked LayoutCells
		VERT=HORZ<<1   //!< Vertical Stacked LayoutCells
	};

	explicit LayoutCellPane( const PaneType nType = HORZ );
	virtual ~LayoutCellPane( );

	virtual void LayoutChild( );
	virtual HDWP ExecuteLayout( HDWP hdwp );
	virtual void getMinMaxInfo( CellMinMaxInfo * pCMMI );


	LayoutCell * addChild( LayoutCell * p_Cell, const int nWeight );
	void toXml(TiXmlElement *xml);
	TiXmlElement * toXml(void);

	CellType getType( ) const;

protected:

	PaneType m_nType; //!< Fixed Cell Type

	typedef std::pair<LayoutCell *, int> CellNode;  //!< Child LayoutCell + Weight Pair
	typedef std::vector<CellNode> VectorOfNodePtrs; //!< Vector of Node pointers

	VectorOfNodePtrs m_vpCells; //!< Vector Of Stacked LayoutCell

	void AdjustMinSize( int & nSizeLeft, int & nTotalWeight );
	void AdjustSize( int & nSizeLeft, int & nTotalWeight );
	void AdjustPos( );

};

#endif // _LAYOUTCELLPANE_H_
