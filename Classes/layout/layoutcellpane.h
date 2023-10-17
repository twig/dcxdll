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

class LayoutCellPane final
	: public LayoutCell
{
public:

	/*!
	* \brief blah
	*
	* blah
	*/
	enum class PaneType
		: UINT
	{
		HORZ=1,         //!< Horizontal Stacked LayoutCells
		VERT=HORZ<<1   //!< Vertical Stacked LayoutCells
	};

	//LayoutCellPane() = delete;
	LayoutCellPane(const LayoutCellPane &) = delete;
	//LayoutCellPane &operator =(const LayoutCellPane &) = delete;	// No assignments!
	LayoutCellPane(LayoutCellPane &&) = delete;
	//LayoutCellPane &operator =(LayoutCellPane &&) = delete;

	explicit LayoutCellPane(const PaneType nType = PaneType::HORZ) noexcept;
	~LayoutCellPane( ) noexcept;

	void LayoutChild() final;
	HDWP ExecuteLayout( const HDWP hdwp ) final;
	void getMinMaxInfo( CellMinMaxInfo *const pCMMI ) const noexcept final;

	void toXml(TiXmlElement *const xml) final;
	TiXmlElement * toXml(void) final;

	const CellType getType( ) const noexcept final;

	LayoutCell * addChild(gsl::owner<LayoutCell *> p_Cell, const UINT nWeight);

protected:

	PaneType m_nType; //!< Fixed Cell Type

	using CellNode = std::pair<LayoutCell *, UINT>;  //!< Child LayoutCell + Weight Pair
	using VectorOfNodePtrs = std::vector<CellNode>; //!< Vector of Node pointers

	VectorOfNodePtrs m_vpCells; //!< Vector Of Stacked LayoutCell

	void AdjustMinSize( UINT & nSizeLeft, UINT & nTotalWeight ) noexcept;
	void AdjustSize( UINT & nSizeLeft, UINT & nTotalWeight ) noexcept;
	void AdjustPos( ) noexcept;

};

#endif // _LAYOUTCELLPANE_H_
