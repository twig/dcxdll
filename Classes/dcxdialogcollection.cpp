/*!
 * \file dcxdialogcollection.cpp
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
#include "dcxdialogcollection.h"
#include "dcxdialog.h"

/*!
 * \brief Constructor
 *
 * Dialog Collection Constructor.
 */

DcxDialogCollection::DcxDialogCollection( ) {

  this->m_vpDialog.clear( );
}

/*!
 * \brief Destructor
 *
 * Dialog Collection Destructor.
 */

DcxDialogCollection::~DcxDialogCollection( ) {

  this->m_vpDialog.clear( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialogCollection::markDialog( HWND mHwnd, TString & tsName, TString & tsAliasName ) {

  this->m_vpDialog.push_back( new DcxDialog( mHwnd, tsName, tsAliasName ) );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDialog * DcxDialogCollection::getDialogByHandle( HWND mHwnd ) {

  VectorOfDialogPtrs::iterator itStart = this->m_vpDialog.begin( );
  VectorOfDialogPtrs::iterator itEnd = this->m_vpDialog.end( );

  while ( itStart != itEnd ) {

    if ( (*itStart)->getHwnd( ) == mHwnd ) 
      return *itStart;

    itStart++;
  }

  return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDialog * DcxDialogCollection::getDialogByName( TString & tsName ) {

  VectorOfDialogPtrs::iterator itStart = this->m_vpDialog.begin( );
  VectorOfDialogPtrs::iterator itEnd = this->m_vpDialog.end( );

  while ( itStart != itEnd ) {

    /*
    char data[500];
    wsprintf( data, "NAME: %s VS %s -- HWND %d -- ADDRESS %x", (*itStart)->getName( ).to_chr( ), tsName.to_chr( ), (*itStart)->getHwnd( ), *itStart );
    mIRCError( data );
    mIRCError( "Not In GetDialog By Name" );
    */

    if ( *itStart != NULL && (*itStart)->getName( ) == tsName )
      return *itStart;

    itStart++;
  }

  //mIRCError( "Returning NULL" );

  return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialogCollection::deleteDialog( DcxDialog * p_Dialog ) {

  if ( p_Dialog == NULL )
    return;

  VectorOfDialogPtrs::iterator itStart = this->m_vpDialog.begin( );
  VectorOfDialogPtrs::iterator itEnd = this->m_vpDialog.end( );

  while ( itStart != itEnd ) {

    if ( *itStart == p_Dialog ) {

      this->m_vpDialog.erase( itStart );
      return;
    } 

    itStart++;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialogCollection::closeDialogs( ) {

  VectorOfDialogPtrs::iterator itStart = this->m_vpDialog.begin( );
  VectorOfDialogPtrs::iterator itEnd = this->m_vpDialog.end( );

  while ( itStart != itEnd ) {

    if ( *itStart != NULL )
      DestroyWindow( (*itStart)->getHwnd( ) );

    itStart++;
  }
}
