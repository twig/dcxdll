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
#include "defines.h"
#include "Classes/dcxdialogcollection.h"
#include "Classes/dcxdialog.h"



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

void DcxDialogCollection::markDialog( const HWND mHwnd, const TString & tsName, const TString & tsAliasName ) {

	this->m_vpDialog.push_back( new DcxDialog( mHwnd, tsName, tsAliasName ) );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDialog * DcxDialogCollection::getDialogByHandle( const HWND mHwnd )
{
	if (mHwnd == NULL)
		return NULL;
#if DCX_USE_C11
	for (const auto &x: this->m_vpDialog) {
		if (x->getHwnd() == mHwnd)
			return x;
	}
	return NULL;
#else
	VectorOfDialogPtrs::iterator itStart = this->m_vpDialog.begin( );
	VectorOfDialogPtrs::iterator itEnd = this->m_vpDialog.end( );

	while ( itStart != itEnd ) {

		if ( (*itStart)->getHwnd( ) == mHwnd ) 
			return *itStart;

		++itStart;
	}

	return NULL;
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDialog * DcxDialogCollection::getDialogByChildHandle( const HWND mHwnd )
{
	if (mHwnd == NULL)
		return NULL;
#if DCX_USE_C11
	for (const auto &x: this->m_vpDialog) {
		if (x->getControlByHWND(mHwnd) != NULL)
			return x;
	}
	return NULL;
#else
	VectorOfDialogPtrs::iterator itStart = this->m_vpDialog.begin( );
	VectorOfDialogPtrs::iterator itEnd = this->m_vpDialog.end( );

	while ( itStart != itEnd ) {

		if ((*itStart)->getControlByHWND(mHwnd) != NULL) 
			return *itStart;

		++itStart;
	}

	return NULL;
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDialog * DcxDialogCollection::getDialogByName( const TString & tsName )
{
	if (tsName.len() == 0)
		return NULL;
#if DCX_USE_C11
	for (const auto &x: this->m_vpDialog) {
		if (x->getName() == tsName)
			return x;
	}
	return NULL;
#else
	VectorOfDialogPtrs::iterator itStart = this->m_vpDialog.begin( );
	VectorOfDialogPtrs::iterator itEnd = this->m_vpDialog.end( );

	while ( itStart != itEnd ) {

		/*
		char data[500];
		wnsprintf( data, "NAME: %s VS %s -- HWND %d -- ADDRESS %x", (*itStart)->getName( ).to_chr( ), tsName.to_chr( ), (*itStart)->getHwnd( ), *itStart );
		mIRCError( data );
		mIRCError( "Not In GetDialog By Name" );
		*/

		//if ( *itStart != NULL && const_cast< TString &>((*itStart)->getName( )) == tsName )
		if ( *itStart != NULL && (*itStart)->getName( ) == tsName )
			return *itStart;

		++itStart;
	}

	//mIRCError( "Returning NULL" );

	return NULL;
#endif
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

		++itStart;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

bool DcxDialogCollection::safeToCloseAll(void) const {
#if DCX_USE_C11
	for (const auto &x: this->m_vpDialog) {
		if (x->getRefCount() != 0)
			return false;
	}
	return true;
#else
	VectorOfDialogPtrs::iterator itStart = const_cast<DcxDialogCollection *>(this)->m_vpDialog.begin( );
	VectorOfDialogPtrs::iterator itEnd = const_cast<DcxDialogCollection *>(this)->m_vpDialog.end( );

	while ( itStart != itEnd ) {

		if ( *itStart != NULL ) {
			if ((*itStart)->getRefCount() != 0)
				return false;
		}
		++itStart;
	}
	return true;
#endif
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
#if DCX_USE_C11
	for (auto &x: this->m_vpDialog)
		DestroyWindow(x->getHwnd());
#else
	VectorOfDialogPtrs::iterator itStart = this->m_vpDialog.begin( );
	VectorOfDialogPtrs::iterator itEnd = this->m_vpDialog.end( );

	while ( itStart != itEnd ) {

		if ( *itStart != NULL ) {
			DestroyWindow( (*itStart)->getHwnd( ) );
		}

		++itStart;
	}
#endif
	this->m_vpDialog.clear(); // clear list.
	this->m_closeall = false;
	return false;
}
