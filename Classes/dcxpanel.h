/*!
 * \file dcxpanel.h
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

#ifndef _DCXPANEL_H_
#define _DCXPANEL_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/layout/layoutmanager.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxPanel : public DcxControl {

public:
	DcxPanel() = delete;
	DcxPanel(const DcxPanel &) = delete;
	DcxPanel &operator =(const DcxPanel &) = delete;	// No assignments!

	DcxPanel(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxPanel( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest( const TString & input ) override;
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) override;

	inline const TString getType() const override { return TEXT("panel"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::PANEL; }

	void toXml(TiXmlElement *const xml) const override;
	TiXmlElement * toXml(void) const override;

protected:

	//static const UINT parseLayoutFlags( const TString & flags );

	LayoutManager * m_pLayoutManager; //!< Layout Manager Object

};

#endif // _DCXPANEL_H_
