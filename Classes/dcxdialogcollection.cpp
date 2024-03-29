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
 * � ScriptsDB.org - 2006
 */
#include "defines.h"
#include "Classes/dcxdialogcollection.h"
#include "Classes/dcxdialog.h"

/// <summary>
/// Mark a dialog for use by DCX
/// </summary>
/// <param name="mHwnd"></param>
/// <param name="tsName"></param>
/// <param name="tsAliasName"></param>
void DcxDialogCollection::markDialog(const HWND mHwnd, const TString& tsName, const TString& tsAliasName)
{
	m_vpDialog.push_back(new DcxDialog(mHwnd, tsName, tsAliasName));
	//m_vpDialog.push_back(std::make_unique<DcxDialog>(mHwnd, tsName, tsAliasName));
}

/// <summary>
/// Get a DcxDialog object from its HWND
/// </summary>
/// <param name="mHwnd"></param>
/// <returns></returns>
DcxDialog* DcxDialogCollection::getDialogByHandle(const HWND mHwnd) const noexcept
{
	if ((!mHwnd) || (m_vpDialog.empty()))
		return nullptr;

	for (const auto& x : m_vpDialog)
	{
		if (x->getHwnd() == mHwnd)
			return x;
	}
	return nullptr;

	//if (const auto itEnd = m_vpDialog.end(), itGot = std::find_if(m_vpDialog.begin(), itEnd, [mHwnd](const auto &x) { return (x->getHwnd() == mHwnd); }); itGot != itEnd)
	//	return *itGot;
	//return nullptr;

	//for (const auto& x : m_vpDialog)
	//{
	//	if (x->getHwnd() == mHwnd)
	//		return x.get();
	//}
	//return nullptr;
}

/// <summary>
/// Get a DcxDialog object from a child controls HWND
/// </summary>
/// <param name="mHwnd"></param>
/// <returns></returns>
DcxDialog* DcxDialogCollection::getDialogByChildHandle(const HWND mHwnd) const noexcept
{
	if ((!mHwnd) || (m_vpDialog.empty()))
		return nullptr;

	for (const auto& x : m_vpDialog)
	{
		if (x->getControlByHWND(mHwnd))
			return x;
	}
	return nullptr;

	//if (const auto itEnd = m_vpDialog.end(), itGot = std::find_if(m_vpDialog.begin(), itEnd, [mHwnd](const auto &x) { return (x->getControlByHWND(mHwnd) != nullptr); }); itGot != itEnd)
	//	return *itGot;
	//return nullptr;

	//for (const auto& x : m_vpDialog)
	//{
	//	if (x->getControlByHWND(mHwnd))
	//		return x.get();
	//}
	//return nullptr;
}

/// <summary>
/// Get a DcxDialog object from its name.
/// </summary>
/// <param name="tsName"></param>
/// <returns></returns>
DcxDialog* DcxDialogCollection::getDialogByName(const TString& tsName) const noexcept
{
	if ((tsName.empty()) || (m_vpDialog.empty()))
		return nullptr;

	for (const auto& x : m_vpDialog)
	{
		if (x->getName() == tsName)
			return x;
	}
	return nullptr;

	//for (const auto& x : m_vpDialog)
	//{
	//	if (x->getName() == tsName)
	//		return x.get();
	//}
	//return nullptr;
}

/// <summary>
/// Delete a DcxDialog object.
/// </summary>
/// <param name="p_Dialog"></param>
void DcxDialogCollection::deleteDialog(const DcxDialog* const p_Dialog)
{
	if ((!p_Dialog) || (m_closeall) || (m_vpDialog.empty()))
		return;

	if (const auto itEnd = m_vpDialog.end(), itGot = std::find(m_vpDialog.begin(), itEnd, p_Dialog); itGot != itEnd)
		m_vpDialog.erase(itGot);
	Dcx::eraseIfFound(m_vpDialog, p_Dialog);

	//if (const auto itEnd = m_vpDialog.end(), itGot = std::find_if(m_vpDialog.begin(), itEnd, [p_Dialog](const auto& a) noexcept { return a.get() == p_Dialog; }); itGot != itEnd)
	//	m_vpDialog.erase(itGot);	// NB: This deletes dialog object !
}

/// <summary>
/// Check if its safe to close all dialogs.
/// </summary>
/// <param name=""></param>
/// <returns></returns>
const bool DcxDialogCollection::safeToCloseAll(void) const noexcept
{
	if (m_vpDialog.empty())
		return true;

	for (const auto& x : m_vpDialog)
	{
		if (x->getRefCount() != 0)
			return false;
	}
	return true;
}

/// <summary>
/// Close all dialogs
/// </summary>
/// <returns></returns>
const bool DcxDialogCollection::closeDialogs() noexcept
{
	if (!safeToCloseAll())
		return true;

	m_closeall = true;

	for (const auto& x : m_vpDialog)
		DestroyWindow(x->getHwnd());

	m_vpDialog.clear(); // clear list.
	m_closeall = false;

	return false;
}
