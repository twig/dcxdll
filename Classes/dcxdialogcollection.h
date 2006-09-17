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

typedef std::vector<DcxDialog *> VectorOfDialogPtrs; //!< blah

/*!
 * \brief blah
 *
 * blah
 */

class DcxDialogCollection {

public:

  DcxDialogCollection( );
  virtual ~DcxDialogCollection( );

  void markDialog( HWND mHwnd, TString & tsName, TString & tsAliasName );
  void deleteDialog( DcxDialog * p_Dialog );
  DcxDialog * getDialogByHandle( HWND mHwnd );
  DcxDialog * getDialogByName( TString & tsName );

  bool closeDialogs( );

protected:

private:
	bool m_closeall; // protects against freeing of vector objects when looping through them.
  VectorOfDialogPtrs m_vpDialog;

};

#endif // _DCXDIALOGCOLLECTION_H_