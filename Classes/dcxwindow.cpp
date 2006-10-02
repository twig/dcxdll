/*!
 * \file dcxwindow.cpp
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

#include "../defines.h"
#include "dcxwindow.h"

/*!
 * \brief Constructor
 *
 * \param mHwnd Window Handle
 * \param mID Window ID
 */

DcxWindow::DcxWindow( HWND mHwnd, UINT mID ) : m_Hwnd( mHwnd ), m_ID( mID ) {

}

/*!
 * \brief Constructor
 *
 * \param mID Window ID
 */

DcxWindow::DcxWindow( UINT mID ) : m_ID( mID ) {

}

/*!
 * \brief Destructor
 *
 * Destructor
 */

DcxWindow::~DcxWindow( ) {


}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxWindow::isStyle( LONG Styles ) {

  if ( GetWindowLong( this->m_Hwnd, GWL_STYLE ) & Styles )
    return TRUE;

  return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::removeStyle( LONG Styles ) {

  LONG winStyles = GetWindowLong( this->m_Hwnd, GWL_STYLE );
  return SetWindowLong( this->m_Hwnd, GWL_STYLE, winStyles &= ~Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::addStyle( LONG Styles ) {

  LONG winStyles = GetWindowLong( this->m_Hwnd, GWL_STYLE );
  return SetWindowLong( this->m_Hwnd, GWL_STYLE, winStyles |= Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::setStyle( LONG Styles ) {

  return SetWindowLong( this->m_Hwnd, GWL_STYLE, Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxWindow::isExStyle( LONG Styles ) {

  if ( GetWindowLong( this->m_Hwnd, GWL_EXSTYLE ) & Styles )
    return TRUE;

  return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::removeExStyle( LONG Styles ) {

  LONG winStyles = GetWindowLong( this->m_Hwnd, GWL_EXSTYLE );
  return SetWindowLong( this->m_Hwnd, GWL_EXSTYLE, winStyles &= ~Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::addExStyle( LONG Styles ) {

  LONG winStyles = GetWindowLong( this->m_Hwnd, GWL_EXSTYLE );
  return SetWindowLong( this->m_Hwnd, GWL_EXSTYLE, winStyles |= Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

LONG DcxWindow::setExStyle( LONG Styles ) {

  return SetWindowLong( this->m_Hwnd, GWL_EXSTYLE, Styles );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxWindow::parseSwitchFlags( TString & switchs, XSwitchFlags * flags ) {

  // no -sign, missing params
  if ( switchs[0] != '-' ) 
    return;

  unsigned int i = 1, len = switchs.len( );

  while ( i < len ) {

    if ( switchs[i] >= 'a' && switchs[i] <= 'z' )
      flags->switch_flags[ (int) ( switchs[i] - 'a' ) ] = 1;
    else if ( switchs[i] >= 'A' && switchs[i] <= 'Z' )
      flags->switch_cap_flags[ (int) ( switchs[i] - 'A' ) ] = 1;

    i++;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxWindow::getID( ) {

  return this->m_ID;
}

/*!
 * \brief blah
 *
 * blah
 */

HWND DcxWindow::getHwnd( ) {

  return this->m_Hwnd;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxWindow::redrawWindow( ) {

  RedrawWindow( this->m_Hwnd, NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE );
}