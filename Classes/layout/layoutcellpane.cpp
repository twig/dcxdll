/*!
 * \file layoutcellpane.cpp
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
#include "defines.h"
#include "layoutcellpane.h"
#include <windowsx.h>

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellPane::LayoutCellPane( const PaneType nType ) noexcept
: LayoutCell( )
, m_nType( nType )
, m_vpCells()
{
}

/*!
 * \brief Destructor
 *
 * blah
 */

LayoutCellPane::~LayoutCellPane( ) noexcept
{
	for (const auto &x: m_vpCells)
		delete x.first;
}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell * LayoutCellPane::addChild(gsl::owner<LayoutCell *> p_Cell, const UINT nWeight )
{
	//if (p_Cell == nullptr) // this should never happen :)
	//	throw Dcx::dcxException("addChild() - NULL Cell supplied");

	if (m_vpCells.empty())
		m_FirstChild = p_Cell;

	p_Cell->setParent( this );

	if ( !m_vpCells.empty() )
	{
		if (auto p_Last = m_vpCells.back().first; p_Last != nullptr )
			p_Last->setSibling( p_Cell );
	}

	//m_vpCells.push_back( CellNode( p_Cell, nWeight ) );
	m_vpCells.emplace_back(p_Cell, nWeight);
	++m_iCount;

	return p_Cell;
}

/*!
 * \brief blah
 *
 * blah
 */

const LayoutCell::CellType LayoutCellPane::getType( ) const noexcept
{
	return PANE;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellPane::LayoutChild( )
{
	UINT nSizeLeft=0;
	UINT nTotalWeight=0;

	// Adjust Child Cells to their minimum width/height
	// and calculate overall weight and loose size left
	this->AdjustMinSize( nSizeLeft, nTotalWeight );

	// redistribute leftover size to childs with non-zero weights
	while ( nSizeLeft && nTotalWeight )
		this->AdjustSize( nSizeLeft, nTotalWeight );

	// Adjust Final Positions
	this->AdjustPos( );

	// Send Layout Command one layer lower
	for (const auto &x: this->m_vpCells)
	{
		if (x.first != nullptr)
			x.first->LayoutChild();
	}
}

/*!
 * \brief blah
 *
 * blah
 */

HDWP LayoutCellPane::ExecuteLayout( const HDWP hdwp )
{
	HDWP hdwpdef = hdwp;
	// Send Layout Command one layer lower
	for (const auto &x: this->m_vpCells)
	{
		if (x.first != nullptr)
			hdwpdef = x.first->ExecuteLayout( hdwpdef );
	}
	return hdwpdef;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellPane::getMinMaxInfo( CellMinMaxInfo *const pCMMI ) const
{
	const auto nMaxWidthX = pCMMI->m_MaxSize.x;
	const auto nMaxWidthY = pCMMI->m_MaxSize.y;

	for (const auto &[pChild, nWeight]: this->m_vpCells)
	{
		UNREFERENCED_PARAMETER(nWeight);

		if (pChild != nullptr)
		{
			if (pChild->isVisible())
			{
				CellMinMaxInfo cmmiChild{ 0,0,nMaxWidthX,nMaxWidthY };

				pChild->getMinMaxInfo( &cmmiChild );

				if ( this->m_nType == HORZ )
				{
					pCMMI->m_MinSize.x += cmmiChild.m_MinSize.x;
					pCMMI->m_MaxSize.x += cmmiChild.m_MaxSize.x;
					pCMMI->m_MinSize.y = std::max( pCMMI->m_MinSize.y, cmmiChild.m_MinSize.y );
					pCMMI->m_MaxSize.y = std::max( pCMMI->m_MaxSize.y, cmmiChild.m_MaxSize.y );
				}
				else {
					pCMMI->m_MinSize.y += cmmiChild.m_MinSize.y;
					pCMMI->m_MaxSize.y += cmmiChild.m_MaxSize.y;
					pCMMI->m_MinSize.x = std::max( pCMMI->m_MinSize.x, cmmiChild.m_MinSize.x );
					pCMMI->m_MaxSize.x = std::max( pCMMI->m_MaxSize.x, cmmiChild.m_MaxSize.x );
				}
			}
		}
	}

	pCMMI->m_MinSize.x = std::max( pCMMI->m_MinSize.x, 0L );
	pCMMI->m_MinSize.y = std::max( pCMMI->m_MinSize.y, 0L );
	pCMMI->m_MaxSize.x = std::min( pCMMI->m_MaxSize.x, gsl::narrow_cast<LONG>(GetSystemMetrics( SM_CXMAXTRACK )) );
	pCMMI->m_MaxSize.y = std::min( pCMMI->m_MaxSize.y, gsl::narrow_cast<LONG>(GetSystemMetrics( SM_CYMAXTRACK )) );
}

void LayoutCellPane::toXml(TiXmlElement *const xml)
{
	if (this->m_nType == LayoutCellPane::HORZ)
		xml->SetAttribute("cascade", "h");
	else if (this->m_nType == LayoutCellPane::VERT)
		xml->SetAttribute("cascade", "v");

	for (const auto &[lc, weight] : this->m_vpCells)
	{
		if (lc != nullptr)
		{
			const auto inner = lc->toXml();
			if (weight != 0)
				inner->SetAttribute("weight", gsl::narrow_cast<int>(weight));
			xml->LinkEndChild(inner);
		}
	}
}

TiXmlElement * LayoutCellPane::toXml(void)
{
	auto xml = std::make_unique<TiXmlElement>("pane");
	this->toXml(xml.get());
	return xml.release();
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellPane::AdjustMinSize( UINT & nSizeLeft, UINT & nTotalWeight )
{
	RECT rc{}, rect{};
	getClientRect( rc );

	int nSize = 0;

	if ( m_nType == HORZ)
	{
		nSize = rc.bottom - rc.top;
		nSizeLeft = gsl::narrow_cast<UINT>(rc.right - rc.left);
	}
	else {
		nSize = rc.right - rc.left;
		nSizeLeft = gsl::narrow_cast<UINT>(rc.bottom - rc.top);
	}

	for (const auto &[pChild, nWeight]: m_vpCells)
	{
		if (pChild == nullptr)
			continue;
		if (!pChild->isVisible())
			continue;

		CellMinMaxInfo cmmiChild{ 0,0,rc.right - rc.left,rc.bottom - rc.top };

		pChild->getMinMaxInfo( &cmmiChild );
		pChild->getRect( rect );

		if ( m_nType == HORZ )
		{
			rect.right = rect.left + cmmiChild.m_MinSize.x;
			rect.bottom = rect.top + nSize;
			nSizeLeft -= cmmiChild.m_MinSize.x;
		}
		else {
			rect.bottom = rect.top + cmmiChild.m_MinSize.y;
			rect.right = rect.left + nSize;
			nSizeLeft -= cmmiChild.m_MinSize.y;
		}

		nTotalWeight += nWeight;
		pChild->setRect( rect );
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellPane::AdjustSize( UINT & nSizeLeft, UINT & nTotalWeight )
{
	RECT rc{};
	this->getClientRect( rc );

	UINT nNewTotalWeight = 0U;
	auto nNewSizeLeft = nSizeLeft;

	for (const auto &[pChild, nWeight]: this->m_vpCells)
	{
		if (pChild == nullptr)
			continue;

		// don't put extra width/height on items of zero weight
		if ( nWeight == 0 || pChild->isVisible( ) == FALSE )
			continue;

		const auto nAddSize = nSizeLeft * nWeight / nTotalWeight;

		RECT rectOld{};

		pChild->getRect( rectOld );
		auto rectNew = rectOld;

		if ( m_nType == HORZ )
		{
			rectNew.right += nAddSize;
			pChild->setRect( rectNew );
			pChild->getRect( rectNew );

			if ( rectOld.right != rectNew.right )
			{
				nNewSizeLeft -= rectNew.right - rectOld.right;

				CellMinMaxInfo cmmiChild{ 0,0,rc.right - rc.left,rc.bottom - rc.top };

				pChild->getMinMaxInfo( &cmmiChild );

				if ( rectNew.right - rectNew.left != cmmiChild.m_MaxSize.x )
					nNewTotalWeight += nWeight;
			}
		}
		else {
			rectNew.bottom += nAddSize;
			pChild->setRect( rectNew );
			pChild->getRect( rectNew );

			if ( rectOld.bottom != rectNew.bottom )
			{
				nNewSizeLeft -= rectNew.bottom - rectOld.bottom;

				CellMinMaxInfo cmmiChild{ 0,0,rc.right - rc.left,rc.bottom - rc.top };

				pChild->getMinMaxInfo( &cmmiChild );

				if ( rectNew.bottom - rectNew.top != cmmiChild.m_MaxSize.y )
					nNewTotalWeight += nWeight;
			}
		}
	}

	nTotalWeight = nNewTotalWeight;
	nSizeLeft = nNewSizeLeft;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellPane::AdjustPos( )
{
	int nPos = 0;
	RECT rect{};
	this->getClientRect( rect );

	if ( m_nType == HORZ )
		nPos = rect.left;
	else
		nPos = rect.top;

	for (const auto &x: this->m_vpCells)
	{
		const auto pChild = x.first;

		if (pChild == nullptr)
			continue;

		if (!pChild->isVisible( ))
			continue;

		RECT rectChild{};
		pChild->getRect( rectChild );

		if ( m_nType == HORZ )
		{
			OffsetRect( &rectChild, nPos - rectChild.left, rect.top - rectChild.top );
			nPos = rectChild.right;
		}
		else {
			OffsetRect( &rectChild, rect.left - rectChild.left, nPos - rectChild.top );
			nPos = rectChild.bottom;
		}
		pChild->setRect( rectChild );
	}
}
