/*!
 * \file dcxtab.h
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

#ifndef _DCXTAB_H_
#define _DCXTAB_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

typedef struct tagDCXTCITEM {

  TString tsTipText;  //!< Tooltip Text
  HWND mChildHwnd;    //!< Hwnd to child window

} DCXTCITEM, * LPDCXTCITEM;

/*!
 * \brief blah
 *
 * blah
 */

class DcxTab : public DcxControl {

public:

	DcxTab( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, const TString & styles );
	virtual ~DcxTab( );

	LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

	void parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const;
	void parseCommandRequest( const TString & input );
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

	HIMAGELIST getImageList( );
	void setImageList( HIMAGELIST himl );
	static HIMAGELIST createImageList( );

	void deleteLParamInfo( const int nItem );
	void activateSelectedTab( );
	void getTab(const int index, LPTCITEM tcItem) const;
	int getTabCount() const;

	inline TString getType( ) const { return TString( TEXT("tab") ); };
	TString getStyles(void) const;
	void toXml(TiXmlElement * xml) const;


protected:
	static void GetCloseButtonRect(const RECT& rcItem, RECT& rcCloseButton);

	bool m_bClosable;	//!< Does tab have a close button.
	bool m_bGradient;	//!< Does tab have a gradient fill. (only closeable)
};

#endif // _DCXTAB_H_
