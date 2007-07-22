/*!
 * \file layoutcellfill.cpp
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

#include "layoutcellfill.h"
#include <windowsx.h>

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellFill::LayoutCellFill( ) : LayoutCell( NULL ) {

}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellFill::LayoutCellFill( const HWND mHwnd ) : LayoutCell( mHwnd ) {

}

/*!
 * \brief Destructor
 *
 * blah
 */

LayoutCellFill::~LayoutCellFill( ) {

}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell::CellType LayoutCellFill::getType( ) {

  return FILL;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellFill::LayoutChild( ) {

}

/*!
 * \brief blah
 *
 * blah
 */

HDWP LayoutCellFill::ExecuteLayout( HDWP hdwp ) {

	HDWP hdwpdef = hdwp; 

	if ( this->m_Hwnd != NULL && IsWindow( this->m_Hwnd ) ) {

		RECT rc;
		this->getClientRect( rc );

		//hdwpdef = DeferWindowPos( hdwpdef, this->m_Hwnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER );
		hdwpdef = DeferWindowPos( hdwpdef, this->m_Hwnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOOWNERZORDER );
	}
	return hdwpdef;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellFill::getMinMaxInfo( CellMinMaxInfo * pCMMI ) {

  if ( this->isVisible( ) ) {

    pCMMI->m_MinSize.x = this->m_rcBorders.left + this->m_rcBorders.right;
    pCMMI->m_MinSize.y = this->m_rcBorders.top + this->m_rcBorders.bottom;

    pCMMI->m_MinSize.x = max( pCMMI->m_MinSize.x, 0 );
    pCMMI->m_MinSize.y = max( pCMMI->m_MinSize.y, 0 );
    pCMMI->m_MaxSize.x = min( pCMMI->m_MaxSize.x, GetSystemMetrics( SM_CXMAXTRACK ) );
    pCMMI->m_MaxSize.y = min( pCMMI->m_MaxSize.y, GetSystemMetrics( SM_CYMAXTRACK ) );
  }
  else {
    ZeroMemory( pCMMI, sizeof( CellMinMaxInfo ) );
  }
}
