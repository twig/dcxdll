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

//extern HWND hwndChild4;

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellPane::LayoutCellPane( const PaneType nType ) : LayoutCell( ), m_nType( nType ) {

}

/*!
 * \brief Destructor
 *
 * blah
 */

LayoutCellPane::~LayoutCellPane( ) {
#if DCX_USE_C11
	for (auto &x: this->m_vpCells) {
		delete x.first;
	}
#else
	VectorOfNodePtrs::iterator itStart = this->m_vpCells.begin( );
	VectorOfNodePtrs::iterator itEnd = this->m_vpCells.end( );

	while ( itStart != itEnd ) {

		delete (*itStart).first;

		itStart++;
	}
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell * LayoutCellPane::addChild( LayoutCell * p_Cell, const int nWeight ) {

	if ( this->m_vpCells.size( ) == 0 )
		this->m_FirstChild = p_Cell;

	if ( p_Cell != NULL ) {

		p_Cell->setParent( this );

		if ( this->m_vpCells.size( ) > 0 ) {

			LayoutCell * p_Last = this->m_vpCells.back( ).first;
			if ( p_Last != NULL )
				p_Last->setSibling( p_Cell );
		}
	}

	this->m_vpCells.push_back( CellNode( p_Cell, nWeight ) );

	return p_Cell;
}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell::CellType LayoutCellPane::getType( ) const {

	return PANE;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellPane::LayoutChild( ) {

	int nSizeLeft=0;
	int nTotalWeight=0;

	// Adjust Child Cells to their minimum width/height
	// and calculate overall weight and loose size left
	this->AdjustMinSize( nSizeLeft, nTotalWeight );

	// redistribute leftover size to childs with non-zero weights
	while ( nSizeLeft && nTotalWeight )
		this->AdjustSize( nSizeLeft, nTotalWeight );

	// Adjust Final Positions
	this->AdjustPos( );


	// Send Layout Command one layer lower
#if DCX_USE_C11
	for (const auto &x: this->m_vpCells) {
		if (x.first != NULL)
			x.first->LayoutChild();
	}
#else
	VectorOfNodePtrs::iterator itStart = this->m_vpCells.begin( );
	VectorOfNodePtrs::iterator itEnd = this->m_vpCells.end( );

	while ( itStart != itEnd ) {

		if ( (*itStart).first != NULL )
			(*itStart).first->LayoutChild( );

		itStart++;
	}
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

HDWP LayoutCellPane::ExecuteLayout( HDWP hdwp ) {

	HDWP hdwpdef = hdwp;
	// Send Layout Command one layer lower
#if DCX_USE_C11
	for (const auto &x: this->m_vpCells) {
		if (x.first != NULL)
			hdwpdef = x.first->ExecuteLayout( hdwpdef );
	}
#else
	VectorOfNodePtrs::iterator itStart = this->m_vpCells.begin( );
	VectorOfNodePtrs::iterator itEnd = this->m_vpCells.end( );

	while ( itStart != itEnd ) {

		if ( (*itStart).first != NULL )
			hdwpdef = (*itStart).first->ExecuteLayout( hdwpdef );

		itStart++;
	}
#endif
	return hdwpdef;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellPane::getMinMaxInfo( CellMinMaxInfo * pCMMI ) {
#if DCX_USE_C11
	const int nMaxWidthX = pCMMI->m_MaxSize.x;
	const int nMaxWidthY = pCMMI->m_MaxSize.y;

	for (const auto &x: this->m_vpCells) {
		LayoutCell *pChild = x.first;
		if (pChild != NULL) {
			if (pChild->isVisible()) {
				CellMinMaxInfo cmmiChild;
				ZeroMemory( &cmmiChild, sizeof( CellMinMaxInfo ) );
				cmmiChild.m_MaxSize.x = nMaxWidthX;
				cmmiChild.m_MaxSize.y = nMaxWidthY;

				pChild->getMinMaxInfo( &cmmiChild );

				if ( this->m_nType == HORZ ) {

					pCMMI->m_MinSize.x += cmmiChild.m_MinSize.x;
					pCMMI->m_MaxSize.x += cmmiChild.m_MaxSize.x;
					pCMMI->m_MinSize.y = max( pCMMI->m_MinSize.y, cmmiChild.m_MinSize.y );
					pCMMI->m_MaxSize.y = max( pCMMI->m_MaxSize.y, cmmiChild.m_MaxSize.y );
				}
				else {

					pCMMI->m_MinSize.y += cmmiChild.m_MinSize.y;
					pCMMI->m_MaxSize.y += cmmiChild.m_MaxSize.y;
					pCMMI->m_MinSize.x = max( pCMMI->m_MinSize.x, cmmiChild.m_MinSize.x );
					pCMMI->m_MaxSize.x = max( pCMMI->m_MaxSize.x, cmmiChild.m_MaxSize.x );
				}
			}
		}
	}

	pCMMI->m_MinSize.x = max( pCMMI->m_MinSize.x, 0 );
	pCMMI->m_MinSize.y = max( pCMMI->m_MinSize.y, 0 );
	pCMMI->m_MaxSize.x = min( pCMMI->m_MaxSize.x, GetSystemMetrics( SM_CXMAXTRACK ) );
	pCMMI->m_MaxSize.y = min( pCMMI->m_MaxSize.y, GetSystemMetrics( SM_CYMAXTRACK ) );
#else
	VectorOfNodePtrs::iterator itStart = this->m_vpCells.begin( );
	VectorOfNodePtrs::iterator itEnd = this->m_vpCells.end( );

	//RECT rc;
	//this->getClientRect( rc );
	//pCMMI->m_MinSize.x = this->m_rcBorders.left + this->m_rcBorders.right;
	//pCMMI->m_MinSize.y = this->m_rcBorders.top + this->m_rcBorders.bottom;

	const int nMaxWidthX = pCMMI->m_MaxSize.x;
	const int nMaxWidthY = pCMMI->m_MaxSize.y;

	while ( itStart != itEnd ) {

		LayoutCell * pChild = (*itStart).first;

		if (pChild->isVisible( )) {

			CellMinMaxInfo cmmiChild;
			ZeroMemory( &cmmiChild, sizeof( CellMinMaxInfo ) );
			cmmiChild.m_MaxSize.x = nMaxWidthX;
			cmmiChild.m_MaxSize.y = nMaxWidthY;

			pChild->getMinMaxInfo( &cmmiChild );

			if ( this->m_nType == HORZ ) {

				pCMMI->m_MinSize.x += cmmiChild.m_MinSize.x;
				pCMMI->m_MaxSize.x += cmmiChild.m_MaxSize.x;
				pCMMI->m_MinSize.y = max( pCMMI->m_MinSize.y, cmmiChild.m_MinSize.y );
				pCMMI->m_MaxSize.y = max( pCMMI->m_MaxSize.y, cmmiChild.m_MaxSize.y );
			}
			else {

				pCMMI->m_MinSize.y += cmmiChild.m_MinSize.y;
				pCMMI->m_MaxSize.y += cmmiChild.m_MaxSize.y;
				pCMMI->m_MinSize.x = max( pCMMI->m_MinSize.x, cmmiChild.m_MinSize.x );
				pCMMI->m_MaxSize.x = max( pCMMI->m_MaxSize.x, cmmiChild.m_MaxSize.x );
			}
		}

		itStart++;
	}

	pCMMI->m_MinSize.x = max( pCMMI->m_MinSize.x, 0 );
	pCMMI->m_MinSize.y = max( pCMMI->m_MinSize.y, 0 );
	pCMMI->m_MaxSize.x = min( pCMMI->m_MaxSize.x, GetSystemMetrics( SM_CXMAXTRACK ) );
	pCMMI->m_MaxSize.y = min( pCMMI->m_MaxSize.y, GetSystemMetrics( SM_CYMAXTRACK ) );
#endif
}

void LayoutCellPane::toXml(TiXmlElement *xml) {
	//TiXmlElement * inner;
	//LayoutCell * lc;
	//const unsigned int count = this->m_vpCells.size();
	//unsigned int weight;
	//if (this->m_nType == LayoutCellPane::HORZ)
	//	xml->SetAttribute("cascade", "h");
	//else if (this->m_nType == LayoutCellPane::VERT)
	//	xml->SetAttribute("cascade", "v");
	//if (count > 0) {
	//	for (unsigned int i = 0; i < count; i++) {
	//		lc = this->m_vpCells[i].first;
	//		weight = this->m_vpCells[i].second;
	//		inner = lc->toXml();
	//		if (weight != 0)
	//			inner->SetAttribute("weight", weight);
	//		xml->LinkEndChild(inner);
	//	}
	//}
	TiXmlElement * inner;
	LayoutCell * lc;
	unsigned int weight;

	if (this->m_nType == LayoutCellPane::HORZ)
		xml->SetAttribute("cascade", "h");
	else if (this->m_nType == LayoutCellPane::VERT)
		xml->SetAttribute("cascade", "v");

#if DCX_USE_C11
	for (const auto &x: this->m_vpCells) {
		lc = x.first;
		weight = x.second;
		inner = lc->toXml();
		if (weight != 0)
			inner->SetAttribute("weight", weight);
		xml->LinkEndChild(inner);
	}
#else
	VectorOfNodePtrs::iterator itStart = this->m_vpCells.begin( );
	VectorOfNodePtrs::iterator itEnd = this->m_vpCells.end( );

	while ( itStart != itEnd ) {

		lc = itStart->first;
		weight = itStart->second;
		inner = lc->toXml();
		if (weight != 0)
			inner->SetAttribute("weight", weight);
		xml->LinkEndChild(inner);

		itStart++;
	}
#endif
}

TiXmlElement * LayoutCellPane::toXml(void) {
	TiXmlElement * xml = new TiXmlElement("pane");
	this->toXml(xml);
	return xml;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellPane::AdjustMinSize( int & nSizeLeft, int & nTotalWeight ) {

	RECT rc, rect;
	this->getClientRect( rc );
	int nSize = 0;

	if ( m_nType == HORZ) {

		nSize = rc.bottom - rc.top;
		nSizeLeft = rc.right - rc.left;
	}
	else {

		nSize = rc.right - rc.left;
		nSizeLeft = rc.bottom - rc.top;
	}
#if DCX_USE_C11
	for (const auto &x: this->m_vpCells) {
		LayoutCell *pChild = x.first;
		const int nWeight = x.second;

		if (pChild == NULL)
			continue;
		if (pChild->isVisible() == FALSE)
			continue;

		CellMinMaxInfo cmmiChild;

		cmmiChild.m_MinSize.x = 0;
		cmmiChild.m_MinSize.y = 0;
		cmmiChild.m_MaxSize.x = rc.right - rc.left;
		cmmiChild.m_MaxSize.y = rc.bottom - rc.top;

		pChild->getMinMaxInfo( &cmmiChild );
		pChild->getRect( rect );

		if ( this->m_nType == HORZ ) {

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
#else
	VectorOfNodePtrs::iterator itStart = this->m_vpCells.begin( );
	VectorOfNodePtrs::iterator itEnd = this->m_vpCells.end( );

	while ( itStart != itEnd ) {

		LayoutCell * pChild = (*itStart).first;
		const int nWeight = (*itStart).second;

		if ( pChild->isVisible( ) == FALSE ) {

			itStart++;
			continue;
		}

		CellMinMaxInfo cmmiChild;

		cmmiChild.m_MinSize.x = 0;
		cmmiChild.m_MinSize.y = 0;
		cmmiChild.m_MaxSize.x = rc.right - rc.left;
		cmmiChild.m_MaxSize.y = rc.bottom - rc.top;

		pChild->getMinMaxInfo( &cmmiChild );
		pChild->getRect( rect );

		if ( this->m_nType == HORZ ) {

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

		itStart++;
	}
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellPane::AdjustSize( int & nSizeLeft, int & nTotalWeight ) {

	RECT rc;
	this->getClientRect( rc );

	int nNewTotalWeight = 0;
	int nNewSizeLeft = nSizeLeft;
#if DCX_USE_C11
	for (const auto &x: this->m_vpCells) {
		LayoutCell * pChild = x.first;
		const int nWeight = x.second;

		if (pChild == NULL)
			continue;
		// don't put extra width/height on items of zero weight
		if ( nWeight == 0 || pChild->isVisible( ) == FALSE )
			continue;

		const int nAddSize = nSizeLeft * nWeight / nTotalWeight;

		RECT rectNew, rectOld;

		pChild->getRect( rectOld );
		rectNew = rectOld;

		if ( m_nType == HORZ ) {

			rectNew.right += nAddSize;
			pChild->setRect( rectNew );
			pChild->getRect( rectNew );

			if ( rectOld.right != rectNew.right ) {

				nNewSizeLeft -= rectNew.right - rectOld.right;

				CellMinMaxInfo cmmiChild;

				cmmiChild.m_MinSize.x = 0;
				cmmiChild.m_MinSize.y = 0;
				cmmiChild.m_MaxSize.x = rc.right - rc.left;
				cmmiChild.m_MaxSize.y = rc.bottom - rc.top;

				pChild->getMinMaxInfo( &cmmiChild );

				if ( rectNew.right - rectNew.left != cmmiChild.m_MaxSize.x )
					nNewTotalWeight += nWeight;
			}
		}
		else {

			rectNew.bottom += nAddSize;
			pChild->setRect( rectNew );
			pChild->getRect( rectNew );

			if ( rectOld.bottom != rectNew.bottom ) {

				nNewSizeLeft -= rectNew.bottom - rectOld.bottom;

				CellMinMaxInfo cmmiChild;

				cmmiChild.m_MinSize.x = 0;
				cmmiChild.m_MinSize.y = 0;
				cmmiChild.m_MaxSize.x = rc.right - rc.left;
				cmmiChild.m_MaxSize.y = rc.bottom - rc.top;

				pChild->getMinMaxInfo( &cmmiChild );

				if ( rectNew.bottom - rectNew.top != cmmiChild.m_MaxSize.y )
					nNewTotalWeight += nWeight;
			}
		}
	}
#else
	VectorOfNodePtrs::iterator itStart = this->m_vpCells.begin( );
	VectorOfNodePtrs::iterator itEnd = this->m_vpCells.end( );

	while ( itStart != itEnd ) {

		LayoutCell * pChild = (*itStart).first;
		const int nWeight = (*itStart).second;

		// don't put extra width/height on items of zero weight
		if ( nWeight == 0 || pChild->isVisible( ) == FALSE ) {

			itStart++;
			continue;
		}

		const int nAddSize = nSizeLeft * nWeight / nTotalWeight;

		RECT rectNew, rectOld;

		pChild->getRect( rectOld );
		rectNew = rectOld;

		if ( m_nType == HORZ ) {

			rectNew.right += nAddSize;
			pChild->setRect( rectNew );
			pChild->getRect( rectNew );

			if ( rectOld.right != rectNew.right ) {

				nNewSizeLeft -= rectNew.right - rectOld.right;

				CellMinMaxInfo cmmiChild;

				cmmiChild.m_MinSize.x = 0;
				cmmiChild.m_MinSize.y = 0;
				cmmiChild.m_MaxSize.x = rc.right - rc.left;
				cmmiChild.m_MaxSize.y = rc.bottom - rc.top;

				pChild->getMinMaxInfo( &cmmiChild );

				if ( rectNew.right - rectNew.left != cmmiChild.m_MaxSize.x )
					nNewTotalWeight += nWeight;
			}
		}
		else {

			rectNew.bottom += nAddSize;
			pChild->setRect( rectNew );
			pChild->getRect( rectNew );

			if ( rectOld.bottom != rectNew.bottom ) {

				nNewSizeLeft -= rectNew.bottom - rectOld.bottom;

				CellMinMaxInfo cmmiChild;

				cmmiChild.m_MinSize.x = 0;
				cmmiChild.m_MinSize.y = 0;
				cmmiChild.m_MaxSize.x = rc.right - rc.left;
				cmmiChild.m_MaxSize.y = rc.bottom - rc.top;

				pChild->getMinMaxInfo( &cmmiChild );

				if ( rectNew.bottom - rectNew.top != cmmiChild.m_MaxSize.y )
					nNewTotalWeight += nWeight;
			}
		}

		itStart++;
	}
#endif
	nTotalWeight = nNewTotalWeight;
	nSizeLeft = nNewSizeLeft;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellPane::AdjustPos( ) {

	int nPos = 0;
	RECT rect;
	this->getClientRect( rect );

	if ( m_nType == HORZ )
		nPos = rect.left;
	else
		nPos = rect.top;
#if DCX_USE_C11
	for (const auto &x: this->m_vpCells) {
		LayoutCell *pChild = x.first;

		if (pChild == NULL)
			continue;

		if ( pChild->isVisible( ) == FALSE )
			continue;

		RECT rectChild;
		pChild->getRect( rectChild );

		if ( m_nType == HORZ ) {

			OffsetRect( &rectChild, nPos - rectChild.left, rect.top - rectChild.top );
			nPos = rectChild.right;
		}
		else {

			OffsetRect( &rectChild, rect.left - rectChild.left, nPos - rectChild.top );
			nPos = rectChild.bottom;
		}
		pChild->setRect( rectChild );
	}
#else
	VectorOfNodePtrs::iterator itStart = this->m_vpCells.begin( );
	VectorOfNodePtrs::iterator itEnd = this->m_vpCells.end( );

	while ( itStart != itEnd ) {

		LayoutCell * pChild = (*itStart).first;

		if ( pChild->isVisible( ) == FALSE ) {

			itStart++;
			continue;
		}

		RECT rectChild;
		pChild->getRect( rectChild );

		if ( m_nType == HORZ ) {

			OffsetRect( &rectChild, nPos - rectChild.left, rect.top - rectChild.top );
			nPos = rectChild.right;
		}
		else {

			OffsetRect( &rectChild, rect.left - rectChild.left, nPos - rectChild.top );
			nPos = rectChild.bottom;
		}
		pChild->setRect( rectChild );

		itStart++;
	}
#endif
}
