/*!
 * \file dcxcolorcombo.h
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

#ifndef _DCXCOLORCOMBO_H_
#define _DCXCOLORCOMBO_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

struct DCXCCOMBOITEM
{
	COLORREF clrItem{ CLR_INVALID };     //!< Item Color
	COLORREF clrText{ CLR_INVALID };     //!< Item Text Color
	TString tsItemText;   //!< Item Text

	explicit DCXCCOMBOITEM(const COLORREF& clr) noexcept : clrItem(clr) {}
};
using LPDCXCCOMBOITEM = DCXCCOMBOITEM *;

// the height for each item
constexpr auto DCX_COLORCOMBO_ITEM_HEIGHT = 16;

class DcxColorCombo final
	: public DcxControl
{
	bool m_bShowNumbers{ false };
public:
	DcxColorCombo() = delete;
	DcxColorCombo(const DcxColorCombo&) = delete;
	GSL_SUPPRESS(c.128) DcxColorCombo& operator =(const DcxColorCombo&) = delete;	// No assignments!
	DcxColorCombo(DcxColorCombo&&) = delete;
	GSL_SUPPRESS(c.128) DcxColorCombo& operator =(DcxColorCombo&&) = delete;

	DcxColorCombo(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxColorCombo() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;

	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	void setmIRCPalette();

	int insertItem(const int nPos, const DCXCCOMBOITEM* dci) noexcept;
	int getCount() const noexcept;
	int setCurSel(const int nPos) noexcept;
	LPDCXCCOMBOITEM getItemData(const int nItem) const noexcept;
	int getCurSel() const noexcept;
	int deleteItem(const int nItem) noexcept;
	int resetContent() noexcept;

	BOOL DrawItem(LPDRAWITEMSTRUCT lpdis);

	inline const TString getType() const final { return TEXT("colorcombo"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::COLORCOMBO; }

	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure

	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
};

#endif // _DCXCOLORCOMBO_H_
