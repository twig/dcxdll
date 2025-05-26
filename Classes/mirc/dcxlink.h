/*!
 * \file dcxlink.h
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

#ifndef _DCXLINK_H_
#define _DCXLINK_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

class DcxLink final
	: public DcxControl
{
public:
	DcxLink() = delete;
	DcxLink(const DcxLink&) = delete;
	GSL_SUPPRESS(c.128) DcxLink& operator =(const DcxLink&) = delete;	// No assignments!
	DcxLink(DcxLink&&) = delete;
	GSL_SUPPRESS(c.128) DcxLink& operator =(DcxLink&&) = delete;

	DcxLink(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxLink() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept final;

	TString parseInfoRequest(const TString& input) const final;
	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	inline const TString getType() const final { return TEXT("link"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::LINK; }

	void loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc) override;

	const TString getStyles(void) const final;
	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure

private:

	HICON m_hIcon{ nullptr }; //!< Link Icon

	COLORREF m_aColors[6]{ RGB(0, 0, 255), RGB(255, 0, 0), RGB(0, 0, 255), RGB(128, 128, 128), RGB(0,0,0), RGB(0,102,204) }; //!< Link Colors 0 = normal, 1 = hover, 2 = visited, 3 = disabled, 4 = pressed, 5 = hot

	bool m_bHover{ false }; //!< Hover State
	bool m_bVisited{ false }; //!< Visited State
	bool m_bUnderlineText{ true };	//!< Should link text be underlined?
	bool m_bPressed{ false };	//!< Link is currently being pressed.
	bool m_bTracking{ false }; //!< Mouse Tracking Var

	//static TString UriEncode(const TString &sSrc);
	//static TString UriDecode(const TString &sSrc);
	void DrawClientArea(HDC hdc);
	void setColour(UINT nColor, const TString& arg);
};

#endif // _DCXLINK_H_
