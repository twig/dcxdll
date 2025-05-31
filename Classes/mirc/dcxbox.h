/*!
 * \file dcxbox.h
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

#ifndef _DCXBOX_H_
#define _DCXBOX_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/layout/layoutmanager.h"

#define DCX_BOXTEXTSPACING 10

#define BOXS_LEFT		0x00 //!< Box Left Style
#define BOXS_CENTER		0x01 //!< Box Center Style
#define BOXS_RIGHT		0x02 //!< Box Right Style
#define BOXS_BOTTOM		0x04 //!< Box Bottom Style
#define BOXS_NONE		0x08 //!< Box No Border Style
#define BOXS_ROUNDED	0x10 //!< Box has rounded corners.
#define BOXS_CHECK		0x20 //!< Box has check button in title to enable/disable contents.
#define BOXS_RADIO		0x40 //!< Box has radio button in title to enable/disable contents.

struct DCXENUM
{
	HWND mChildHwnd{ nullptr };	//!< Hwnd to child window
	HWND mBox{ nullptr };			//!< The Box control itself
	BOOL mState{ FALSE };		//!< Enable/Disable state
};
using LPDCXENUM = DCXENUM*;

class DcxDialog;

class DcxBox final
	: public DcxControl
{
public:
	DcxBox() = delete;
	DcxBox(const DcxBox&) = delete;
	GSL_SUPPRESS(c.128) DcxBox& operator =(const DcxBox&) = delete;
	DcxBox(DcxBox&&) = delete;
	GSL_SUPPRESS(c.128) DcxBox& operator =(DcxBox&&) = delete;

	//DCX_DELETE_CONTROL_METHODS(DcxBox);

	DcxBox(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxBox();

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept final;

	TString parseInfoRequest(const TString& input) const final;
	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	inline const TString getType() const final { return TEXT("box"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::BOX; }

	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml() const final;
	std::unique_ptr<TiXmlElement> toXml(int blah) const;
	const TString getStyles(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	//void xmlParseElements(const TString& tsPath, const TiXmlElement* xParent);
	//void xmlAddPane(const TString& tsParentPath, const TString& tsCurrentPath, const TiXmlElement* xElement);
	//bool xmlAddControl(const TString& tsParentPath, const TString& tsCurrentPath, const TiXmlElement* xParent, const TiXmlElement* xCtrl);

	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure

private:
	static BOOL CALLBACK EnumBoxChildren(HWND hwnd, const DCXENUM* const de) noexcept;
	void EraseBackground(HDC hdc);
	void DrawClientArea(HDC hdc);
	void DrawBorder(HDC hdc, RECT& rc, const LPRECT rcClip = nullptr) noexcept;
	HRGN CreateClipRegion(LPCRECT rcClip) noexcept;

	void DrawCheckButton(HDC hdc, LPRECT rcCheck) noexcept;

	LONG m_iBoxStyles{}; //!< Custom Box Styles

	std::unique_ptr<LayoutManager> m_pLayoutManager; //!< Layout Manager Object

	RECT m_rcCheck{};
	bool m_bTitleChecked{ true };
	HTHEME _hTheme{ nullptr };

};

#endif // _DCXBOX_H_
