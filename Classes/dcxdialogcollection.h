/*!
 * \file dcxdialogcollection.h
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

#ifndef _DCXDIALOGCOLLECTION_H_
#define _DCXDIALOGCOLLECTION_H_

// dummy runtime class definition
class DcxDialog;

using VectorOfDialogPtrs = std::vector<DcxDialog *>; //!< blah

/*!
 * \brief blah
 *
 * blah
 */

class DcxDialogCollection {

public:

	DcxDialogCollection(const DcxDialogCollection &) = delete;
	DcxDialogCollection &operator =(const DcxDialogCollection &) = delete;
	DcxDialogCollection(DcxDialogCollection &&) = delete;
	DcxDialogCollection &operator =(DcxDialogCollection &&) = delete;

	DcxDialogCollection() noexcept = default;
	~DcxDialogCollection( ) noexcept = default;

	void markDialog(const HWND mHwnd, const TString & tsName, const TString & tsAliasName);
	void deleteDialog( const DcxDialog *const p_Dialog );
	DcxDialog * getDialogByHandle( const HWND mHwnd ) const noexcept;
	DcxDialog * getDialogByChildHandle(const HWND mHwnd) const noexcept;
	DcxDialog * getDialogByName(const TString & tsName) const noexcept;

	const bool closeDialogs() noexcept;
	const bool safeToCloseAll(void) const noexcept;

private:
	VectorOfDialogPtrs m_vpDialog;
	bool m_closeall{ false }; // protects against freeing of vector objects when looping through them.
};

#endif // _DCXDIALOGCOLLECTION_H_
