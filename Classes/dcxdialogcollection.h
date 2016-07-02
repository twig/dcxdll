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

// dummy runtime classe definition
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
	DcxDialogCollection &operator =(const DcxDialogCollection &) = delete;	// No assignments!

	DcxDialogCollection( ) = default;
	~DcxDialogCollection( ) = default;

	void markDialog(const HWND mHwnd, const TString & tsName, const TString & tsAliasName);
	void deleteDialog( const DcxDialog *const p_Dialog );
	DcxDialog * getDialogByHandle( const HWND mHwnd ) const;
	DcxDialog * getDialogByChildHandle(const HWND mHwnd) const;
	DcxDialog * getDialogByName(const TString & tsName) const;

	const bool closeDialogs();
	const bool safeToCloseAll(void) const;

private:
	VectorOfDialogPtrs m_vpDialog;
	bool m_closeall; // protects against freeing of vector objects when looping through them.
};

#endif // _DCXDIALOGCOLLECTION_H_
