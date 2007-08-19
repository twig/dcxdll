/* \file layoutcellfixed.cpp
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

#include "layoutcellfixed.h"
#include <windowsx.h>

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellFixed::LayoutCellFixed( const HWND mHwnd, const FixedType nType ) : LayoutCell( mHwnd ), m_nType( nType ) {

}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellFixed::LayoutCellFixed( const RECT & rc, const FixedType nType ) : LayoutCell( NULL ), m_nType( nType ) {

  CopyRect( &this->m_rcWindow, &rc );
}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCellFixed::LayoutCellFixed( const HWND mHwnd, const RECT & rc, const FixedType nType ) : LayoutCell( NULL ), m_nType( nType ) {

  this->m_Hwnd = mHwnd;
  CopyRect( &this->m_rcWindow, &rc );
}

/*!
 * \brief Destructor
 *
 * blah
 */

LayoutCellFixed::~LayoutCellFixed( ) {

}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell::CellType LayoutCellFixed::getType( ) {

  return FIXED;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCellFixed::LayoutChild( ) {

}

/*!
 * \brief blah
 *
 * blah
 */

HDWP LayoutCellFixed::ExecuteLayout( HDWP hdwp ) {

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

void LayoutCellFixed::getMinMaxInfo( CellMinMaxInfo * pCMMI ) {

  if ( this->isVisible( ) ) {
    RECT rc;
    CopyRect( &rc, &this->m_rcWindow );

    if ( m_nType & WIDTH ) {

      if ( m_nType != BOTH )
        pCMMI->m_MinSize.y = this->m_rcBorders.top + this->m_rcBorders.bottom;

      pCMMI->m_MinSize.x = rc.right - rc.left;
      pCMMI->m_MaxSize.x = rc.right - rc.left;

    }

    if ( m_nType & HEIGHT ) {

      if ( m_nType != BOTH )
        pCMMI->m_MinSize.x = this->m_rcBorders.left + this->m_rcBorders.right;

      pCMMI->m_MinSize.y = rc.bottom - rc.top;
      pCMMI->m_MaxSize.y = rc.bottom - rc.top;
    }

    pCMMI->m_MinSize.x = max( pCMMI->m_MinSize.x, 0 );
    pCMMI->m_MinSize.y = max( pCMMI->m_MinSize.y, 0 );
    pCMMI->m_MaxSize.x = min( pCMMI->m_MaxSize.x, GetSystemMetrics( SM_CXMAXTRACK ) );
    pCMMI->m_MaxSize.y = min( pCMMI->m_MaxSize.y, GetSystemMetrics( SM_CYMAXTRACK ) );
  }
  else {
    ZeroMemory( pCMMI, sizeof( CellMinMaxInfo ) );
  }
}
