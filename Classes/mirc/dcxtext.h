/*!
 * \file dcxtext.h
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

#ifndef _DCXTEXT_H_
#define _DCXTEXT_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
//#include <ColourString.h>

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxText final
	: public DcxControl
{
public:
	DcxText() = delete;
	DcxText(const DcxText &) = delete;
	GSL_SUPPRESS(c.128) DcxText &operator =(const DcxText &) = delete;	// No assignments!
	DcxText(DcxText &&) = delete;
	GSL_SUPPRESS(c.128) DcxText &operator =(DcxText &&) = delete;

	DcxText(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxText( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) noexcept final;

	TString parseInfoRequest(const TString& input) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("text"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::TEXT; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
	
private:
	TString m_tsText;	// Edit Text
	UINT m_uiStyle{};		// text style
	bool m_bDoubleBuffer{ false };
	void DrawClientArea(HDC hdc);
};

#endif // _DCXTEXT_H_
