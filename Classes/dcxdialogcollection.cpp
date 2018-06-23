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

//DcxDialogCollection::DcxDialogCollection( )
//: m_closeall(false)
//, m_vpDialog()
//{
//}

/*!
 * \brief Destructor
 *
 * Dialog Collection Destructor.
 */

//DcxDialogCollection::~DcxDialogCollection( )
//{
//}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDialogCollection::markDialog( const HWND mHwnd, const TString & tsName, const TString & tsAliasName )
{
	m_vpDialog.push_back( new DcxDialog( mHwnd, tsName, tsAliasName ) );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDialog * DcxDialogCollection::getDialogByHandle(const HWND mHwnd) const noexcept
{
	if ((mHwnd == nullptr) || (m_vpDialog.empty()))
		return nullptr;

	for (const auto &x: m_vpDialog)
	{
		if (x->getHwnd() == mHwnd)
			return x;
	}
	return nullptr;

	//if (const auto itEnd = m_vpDialog.end(), itGot = std::find_if(m_vpDialog.begin(), itEnd, [mHwnd](const auto &x) { return (x->getHwnd() == mHwnd); }); itGot != itEnd)
	//	return *itGot;
	//return nullptr;
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDialog * DcxDialogCollection::getDialogByChildHandle(const HWND mHwnd) const noexcept
{
	if ((mHwnd == nullptr) || (m_vpDialog.empty()))
		return nullptr;

	for (const auto &x: m_vpDialog)
	{
		if (x->getControlByHWND(mHwnd) != nullptr)
			return x;
	}
	return nullptr;

	//if (const auto itEnd = m_vpDialog.end(), itGot = std::find_if(m_vpDialog.begin(), itEnd, [mHwnd](const auto &x) { return (x->getControlByHWND(mHwnd) != nullptr); }); itGot != itEnd)
	//	return *itGot;
	//return nullptr;
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDialog * DcxDialogCollection::getDialogByName(const TString & tsName) const noexcept
{
	if ((tsName.empty()) || (m_vpDialog.empty()))
		return nullptr;

	for (const auto &x: m_vpDialog)
	{
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

void DcxDialogCollection::deleteDialog( const DcxDialog *const p_Dialog )
{
	if (( p_Dialog == nullptr ) || (m_closeall) || (m_vpDialog.empty()))
		return;

	//if (const auto itEnd = m_vpDialog.end(), itGot = std::find(m_vpDialog.begin(), itEnd, p_Dialog); itGot != itEnd)
	//	m_vpDialog.erase(itGot);

	Dcx::eraseIfFound(m_vpDialog, p_Dialog);
}

/*!
 * \brief blah
 *
 * blah
 */

const bool DcxDialogCollection::safeToCloseAll(void) const noexcept
{
	if (m_vpDialog.empty())
		return true;

	for (const auto &x: m_vpDialog)
	{
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

const bool DcxDialogCollection::closeDialogs() noexcept
{
	if (!safeToCloseAll())
		return true;

	m_closeall = true;

	for (const auto &x: m_vpDialog)
		DestroyWindow(x->getHwnd());

	m_vpDialog.clear(); // clear list.
	m_closeall = false;
	
	return false;
}
