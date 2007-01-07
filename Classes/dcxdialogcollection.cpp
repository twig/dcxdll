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
	this->m_closeall = false;
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

void DcxDialogCollection::markDialog( const HWND mHwnd, TString & tsName, TString & tsAliasName ) {

  this->m_vpDialog.push_back( new DcxDialog( mHwnd, tsName, tsAliasName ) );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDialog * DcxDialogCollection::getDialogByHandle( const HWND mHwnd ) {

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

DcxDialog * DcxDialogCollection::getDialogByName( const TString & tsName ) {

  VectorOfDialogPtrs::iterator itStart = this->m_vpDialog.begin( );
  VectorOfDialogPtrs::iterator itEnd = this->m_vpDialog.end( );

  while ( itStart != itEnd ) {

    /*
    char data[500];
    wsprintf( data, "NAME: %s VS %s -- HWND %d -- ADDRESS %x", (*itStart)->getName( ).to_chr( ), tsName.to_chr( ), (*itStart)->getHwnd( ), *itStart );
    mIRCError( data );
    mIRCError( "Not In GetDialog By Name" );
    */

    if ( *itStart != NULL && const_cast< TString &>((*itStart)->getName( )) == tsName )
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
	if (this->m_closeall)
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

bool DcxDialogCollection::safeToCloseAll(void) const {

  VectorOfDialogPtrs::iterator itStart = const_cast<DcxDialogCollection *>(this)->m_vpDialog.begin( );
  VectorOfDialogPtrs::iterator itEnd = const_cast<DcxDialogCollection *>(this)->m_vpDialog.end( );

  while ( itStart != itEnd ) {

		if ( *itStart != NULL ) {
			if ((*itStart)->getRefCount() != 0)
				return false;
		}
    itStart++;
  }
	return true;
}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxDialogCollection::closeDialogs( ) {

	if (!this->safeToCloseAll())
		return true;

	this->m_closeall = true;
  VectorOfDialogPtrs::iterator itStart = this->m_vpDialog.begin( );
  VectorOfDialogPtrs::iterator itEnd = this->m_vpDialog.end( );

	itStart = this->m_vpDialog.begin( );
	while ( itStart != itEnd ) {

		if ( *itStart != NULL ) {
			DestroyWindow( (*itStart)->getHwnd( ) );
		}

		itStart++;
	}
	this->m_vpDialog.clear(); // clear list.
	this->m_closeall = false;
	return false;
}
