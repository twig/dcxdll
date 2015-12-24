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

DcxDialogCollection::DcxDialogCollection( )
: m_closeall(false)
{
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

void DcxDialogCollection::markDialog( const HWND mHwnd, const TString & tsName, const TString & tsAliasName ) {

	this->m_vpDialog.push_back( new DcxDialog( mHwnd, tsName, tsAliasName ) );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDialog * DcxDialogCollection::getDialogByHandle(const HWND mHwnd) const
{
	if (mHwnd == nullptr)
		return nullptr;

	if (this->m_vpDialog.empty())
		return nullptr;

	for (const auto &x: this->m_vpDialog) {
		if (x->getHwnd() == mHwnd)
			return x;
	}
	return nullptr;

	//auto itEnd = this->m_vpDialog.end();
	//auto got = std::find_if(this->m_vpDialog.begin(), itEnd, [mHwnd](const auto &x) { return (x->getHwnd() == mHwnd); });
	//if (got != itEnd)
	//	return *got;
	//return nullptr;
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDialog * DcxDialogCollection::getDialogByChildHandle(const HWND mHwnd) const
{
	if (mHwnd == nullptr)
		return nullptr;

	if (this->m_vpDialog.empty())
		return nullptr;

	for (const auto &x: this->m_vpDialog) {
		if (x->getControlByHWND(mHwnd) != nullptr)
			return x;
	}
	return nullptr;

	//auto itEnd = this->m_vpDialog.end();
	//auto got = std::find_if(this->m_vpDialog.begin(), itEnd, [mHwnd](const auto &x) { return (x->getControlByHWND(mHwnd) != nullptr); });
	//if (got != itEnd)
	//	return *got;
	//return nullptr;
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDialog * DcxDialogCollection::getDialogByName(const TString & tsName) const
{
	if (tsName.empty())
		return nullptr;

	if (this->m_vpDialog.empty())
		return nullptr;

	for (const auto &x: this->m_vpDialog) {
		if (x->getName() == tsName)
			return x;
	}
	return nullptr;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialogCollection::deleteDialog( const DcxDialog *const p_Dialog ) {

	if (( p_Dialog == nullptr ) || (this->m_closeall))
		return;

	if (this->m_vpDialog.empty())
		return;

	//auto itStart = this->m_vpDialog.begin();
	//auto itEnd = this->m_vpDialog.end();
	//
	//while ( itStart != itEnd ) {
	//
	//	if ( *itStart == p_Dialog ) {
	//
	//		this->m_vpDialog.erase( itStart );
	//		return;
	//	} 
	//
	//	++itStart;
	//}

	//for (auto itStart = m_vpDialog.begin(), itEnd = m_vpDialog.end(); itStart != itEnd; ++itStart)
	//{
	//	if (*itStart == p_Dialog) {
	//		this->m_vpDialog.erase(itStart);
	//		return;
	//	}
	//}

	const auto itEnd = m_vpDialog.end();
	const auto itGot = std::find(m_vpDialog.begin(), itEnd, p_Dialog);
	if (itGot != itEnd)
		m_vpDialog.erase(itGot);
}

/*!
 * \brief blah
 *
 * blah
 */

const bool DcxDialogCollection::safeToCloseAll(void) const {
	if (this->m_vpDialog.empty())
		return true;

	for (const auto &x: this->m_vpDialog) {
		if (x->getRefCount() != 0)
			return false;
	}
	return true;
}

/*!
 * \brief blah
 *
 * blah
 */

const bool DcxDialogCollection::closeDialogs() {

	if (!this->safeToCloseAll())
		return true;

	this->m_closeall = true;

	for (const auto &x: this->m_vpDialog)
		DestroyWindow(x->getHwnd());

	this->m_vpDialog.clear(); // clear list.
	this->m_closeall = false;
	return false;
}
