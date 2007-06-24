/*!
 * \file layoutcell.cpp
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

#include "layoutcell.h"

#include <windowsx.h>

extern HWND hwndChild4;

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCell::LayoutCell( ) {

  //MessageBox( NULL, "LayoutCell( )", "LayoutCell( )", MB_OK );

  SetRectEmpty( &this->m_rcBorders );
  SetRectEmpty( &this->m_rcWindow );

  this->m_Hwnd = NULL;

  this->m_Parent = NULL;
  this->m_FirstChild = NULL;
  this->m_NextSibling = NULL;
}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCell::LayoutCell( const HWND mHwnd ) : m_Hwnd( mHwnd ) {

  //MessageBox( NULL, "LayoutCell( HWND mHwnd )", "LayoutCell( HWND mHwnd )", MB_OK );

  if ( mHwnd != NULL )
    GetWindowRect( mHwnd, &this->m_rcWindow );
  else
    SetRectEmpty( &this->m_rcWindow );

  SetRectEmpty( &this->m_rcBorders );

  this->m_Parent = NULL;
  this->m_FirstChild = NULL;
  this->m_NextSibling = NULL;
}

/*!
 * \brief Constructor
 *
 * blah
 */

LayoutCell::LayoutCell( const HWND mHwnd, const RECT & rc ) : m_Hwnd( mHwnd ), m_rcWindow( rc ) {

  SetRectEmpty( &this->m_rcBorders );

  this->m_Parent = NULL;
  this->m_FirstChild = NULL;
  this->m_NextSibling = NULL;
}

/*!
 * \brief Destructor
 *
 * blah
 */

LayoutCell::~LayoutCell( ) {


}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::setParent( LayoutCell * p_Cell ) {

  this->m_Parent = p_Cell;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::setSibling( LayoutCell * p_Cell ) {

  this->m_NextSibling = p_Cell;
}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell * LayoutCell::getFirstChild( ) const {

  return this->m_FirstChild;
}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell * LayoutCell::getParent( ) const {

  return this->m_Parent;
}

/*!
 * \brief blah
 *
 * blah
 */

LayoutCell * LayoutCell::getNextSibling( ) const {

  return this->m_NextSibling;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::setRect( RECT & rc ) {

  //RECT rect;
  //this->getClientRect( rect );

  //MessageBox( hwndChild3, "blah", "blah", MB_OK );

  CellMinMaxInfo cmmi;
  cmmi.m_MinSize.x = 0;
  cmmi.m_MinSize.y = 0;
  cmmi.m_MaxSize.x = rc.right - rc.left;
  cmmi.m_MaxSize.y = rc.bottom - rc.top;

  this->getMinMaxInfo( &cmmi );

  /*
  char data[500];
  wsprintf( data, "RECT %d %d %d %d\r\nWRECT %d %d %d %d\r\nMAX %d %d %d %d\r\nMETRICS %d %d %d %d", 
    rect.left, rect.top, rect.right, rect.bottom,
    rc.left, rc.top, rc.right, rc.bottom,
    cmmi.m_MinSize.x, cmmi.m_MinSize.y, cmmi.m_MaxSize.x, cmmi.m_MaxSize.y, 
    GetSystemMetrics( SM_CXMINTRACK ), GetSystemMetrics( SM_CYMINTRACK ), GetSystemMetrics( SM_CXMAXTRACK ),
    GetSystemMetrics( SM_CYMAXTRACK ));

  
  if ( this->m_Hwnd != NULL )
    Edit_SetText( this->m_Hwnd, data );
  else
    Edit_SetText( hwndChild4, data );
    */

  if ( rc.right - rc.left < cmmi.m_MinSize.x )
		rc.right = rc.left + cmmi.m_MinSize.x;
	if ( rc.right - rc.left > cmmi.m_MaxSize.x )
		rc.right = rc.left + cmmi.m_MaxSize.x;
	if ( rc.bottom - rc.top < cmmi.m_MinSize.y )
		rc.bottom = rc.top + cmmi.m_MinSize.y;
	if ( rc.bottom - rc.top > cmmi.m_MaxSize.y )
		rc.bottom = rc.top + cmmi.m_MaxSize.y;
  
  if ( EqualRect( &rc, &this->m_rcWindow ) )
    return;

  this->m_rcWindow = rc;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::getRect( RECT & rc ) const {

  rc = this->m_rcWindow;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::getClientRect( RECT & rc ) const {

  CopyRect( &rc, &this->m_rcWindow );

  rc.left += this->m_rcBorders.left;
	rc.right -= this->m_rcBorders.right;
	rc.top += this->m_rcBorders.top;
	rc.bottom -= this->m_rcBorders.bottom;
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::setBorder( const RECT & rc ) {

  // remove old borders
  this->m_rcWindow.right -= this->m_rcBorders.left + this->m_rcBorders.right;
  this->m_rcWindow.bottom -= this->m_rcBorders.top + this->m_rcBorders.bottom;

  // new borders
  CopyRect( &this->m_rcBorders, &rc );

  // re-add new border settings
	this->m_rcWindow.right += this->m_rcBorders.left + this->m_rcBorders.right;
	this->m_rcWindow.bottom += this->m_rcBorders.top + this->m_rcBorders.bottom;

  /*
  this->m_rcWindow.left -= this->m_rcBorders.left;
	this->m_rcWindow.right += this->m_rcBorders.right;
	this->m_rcWindow.top -= this->m_rcBorders.top;
	this->m_rcWindow.bottom += this->m_rcBorders.bottom;
  */
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::setBorder( const unsigned int nBorder ) {

  // remove old borders
  this->m_rcWindow.right -= this->m_rcBorders.left + this->m_rcBorders.right;
  this->m_rcWindow.bottom -= this->m_rcBorders.top + this->m_rcBorders.bottom;

  // new borders
  SetRect( &this->m_rcBorders, nBorder, nBorder, nBorder, nBorder );

  // re-add new border settings
	this->m_rcWindow.right += this->m_rcBorders.left + this->m_rcBorders.right;
	this->m_rcWindow.bottom += this->m_rcBorders.top + this->m_rcBorders.bottom;

  /*
  this->m_rcWindow.left -= this->m_rcBorders.left;
	this->m_rcWindow.right += this->m_rcBorders.right;
	this->m_rcWindow.top -= this->m_rcBorders.top;
	this->m_rcWindow.bottom += this->m_rcBorders.bottom;
  */
}

/*!
 * \brief blah
 *
 * blah
 */

void LayoutCell::getBorder( RECT & rc ) const {

  rc = this->m_rcBorders;
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL LayoutCell::isVisible( ) const {

  if ( this->m_Hwnd != NULL && IsWindow( this->m_Hwnd ) && IsWindowVisible( this->m_Hwnd ) == FALSE )
    return FALSE;

  return TRUE;
}
