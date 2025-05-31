/*!
 * \file dcxbutton.h
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

#ifndef _DCXBUTTON_H_
#define _DCXBUTTON_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

#define ICON_XPAD 2 //!< Icon X Padding

#define BUTTON_YPAD 3 //!< Button Top/Bottom Padding
#define BUTTON_XPAD 4 //!< Button Left/Right Padding

#define BTNCS_NORMAL    0x01 //!< Button Normal Color Style
#define BTNCS_SELECTED  0x02 //!< Button Selected Color Style
#define BTNCS_HOVER     0x04 //!< Button Hover Color Style
#define BTNCS_DISABLED  0x08 //!< Button Disabled Color Style
#define BTNIS_GREY      0x10 //!< Button Grey Icon
#define BTNIS_ASSOC     0x20 //!< Button Icon is Associated with the file.

/*!
 * \brief blah
 *
 * blah
 */

class DcxButton final
	: public DcxControl
{
public:
	DcxButton() = delete;
	DcxButton(const DcxButton&) = delete;
	DcxButton(DcxButton&&) = delete;
	GSL_SUPPRESS(c.128) DcxButton& operator =(const DcxButton&) = delete;	// No assignments!
	GSL_SUPPRESS(c.128) DcxButton& operator =(DcxButton&&) = delete;

	DcxButton(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxButton();

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;

	TString parseInfoRequest(const TString& input) const final;
	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	const HIMAGELIST& getImageList() const noexcept;
	void setImageList(const HIMAGELIST himl) noexcept;
	HIMAGELIST createImageList() noexcept;

	void loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc) override;

	inline const TString getType() const final { return TEXT("button"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::BUTTON; }

	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;
	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure

	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

private:
	HIMAGELIST m_ImageList{ nullptr }; //!< Button Image List (normal, hover, pushed, disabled)
	COLORREF m_aColors[4]{ CLR_INVALID };  //!< Button Colors (normal, hover, pushed, disabled)
	COLORREF m_aTransp[4]{ CLR_INVALID };  //!< Transparent Colors (normal, hover, pushed, disabled)

	//HBITMAP m_aBitmaps[4]{ nullptr };  //!< Button Bitmaps (normal, hover, pushed, disabled)
	dcxImage m_aBitmaps[4];  //!< Button Bitmaps (normal, hover, pushed, disabled)

	TString m_tsCaption; //!< Button Caption Text

	bool m_bTracking{ false }; //!< Button Tracking Mouse Event State
	bool m_bHover{ false };    //!< Button Hovering State
	bool m_bTouched{ false };  //!< Button Touched by Mouse State
	bool m_bSelected{ false }; //!< Button Selected State
	bool m_bBitmapText{ false };
	bool m_bHasIcons{ false };

	DcxIconSizes m_iIconSize{ DcxIconSizes::SmallIcon }; //!< Button Icon Size 16,24,32

	static const UINT parseColorFlags(const TString& flags) noexcept;
	void DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam);
};

#endif // _DCXBUTTON_H_
