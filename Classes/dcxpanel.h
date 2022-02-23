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

class DcxPanel final
	: public DcxControl
{
public:
	DcxPanel() = delete;
	DcxPanel(const DcxPanel &) = delete;
	DcxPanel &operator =(const DcxPanel &) = delete;	// No assignments!
	DcxPanel(DcxPanel &&) = delete;
	DcxPanel &operator =(DcxPanel &&) = delete;

	DcxPanel(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxPanel( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) noexcept final;

	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest( const TString & input ) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("panel"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::PANEL; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

protected:

	std::unique_ptr<LayoutManager> m_pLayoutManager; //!< Layout Manager Object
};

#endif // _DCXPANEL_H_
