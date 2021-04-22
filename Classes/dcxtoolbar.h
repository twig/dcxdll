/*!
 * \file dcxtoolbar.h
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

#ifndef _DCXTOOLBAR_H_
#define _DCXTOOLBAR_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

enum class dcxToolBar_ImageLists
	: UINT
{
	TB_IML_NORMAL = 0x01,	//!< Toolbar Normal ImageList
	TB_IML_DISABLE = 0x02,	//!< Toolbar Disabled ImageList
	TB_IML_HOT = 0x04,	//!< Toolbar HotTrack ImageList
	TB_ICO_GREY = 0x08	//!< ???
};
template <typename T>
constexpr dcxToolBar_ImageLists& operator |=(dcxToolBar_ImageLists& eStyle, const T& dStyle) noexcept
{
	return eStyle = static_cast<dcxToolBar_ImageLists>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}
template <typename T>
constexpr T& operator |=(T& eStyle, const dcxToolBar_ImageLists& dStyle) noexcept
{
	return eStyle = static_cast<T>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}

enum class dcxToolBar_Styles
	: DWORD
{
	BTNS_WIDTH = 0x00001000,	//!< Toolbar Button Width Style
	BTNS_BOLD = 0x00002000,	//!< Toolbar Button Caption Bold Style
	BTNS_UNDERLINE = 0x00004000,	//!< Toolbar Button Caption Underline Style
	BTNS_COLOR = 0x00008000,	//!< Toolbar Button Caption Color Style
	BTNS_TBKGCOLOR = 0x00010000,	//!< Toolbar Tooltip Background Color Style
	BTNS_TTXTCOLOR = 0x00020000,	//!< Toolbar Tooltip Text Color Style
	BTNS_HIGHLIGHT_TXTCOLOR = 0x00040000,	//!< Toolbar Buttons Highlight Text colour.
	BTNS_MARK_BKGCOLOR = 0x00080000,	//!< Toolbar Buttons background colour when marked.
	BTNS_BTNCOLOR = 0x00100000,	//!< Toolbar Buttons colour.
	BTNS_HIGHLIGHT_BTNCOLOR = 0x00200000,	//!< Toolbar Buttons colour when highlighted.
	BTNS_HOTTRACK_BTNCOLOR = 0x00400000	//!< Toolbar Buttons colour when hot.
};
template <typename T>
constexpr dcxToolBar_Styles& operator |=(dcxToolBar_Styles& eStyle, const T& dStyle) noexcept
{
	return eStyle = static_cast<dcxToolBar_Styles>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}
template <typename T>
constexpr T& operator |=(T& eStyle, const dcxToolBar_Styles& dStyle) noexcept
{
	return eStyle = static_cast<T>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}

//#define TB_IML_NORMAL	0x01	//!< Toolbar Normal ImageList
//#define TB_IML_DISABLE	0x02	//!< Toolbar Disabled ImageList
//#define TB_IML_HOT		0x04	//!< Toolbar HotTrack ImageList
//#define TB_ICO_GREY		0x08	//!< Toolbar HotTrack ImageList
//
//#define BTNS_WIDTH				0x001000	//!< Toolbar Button Width Style
//#define BTNS_BOLD				0x002000	//!< Toolbar Button Caption Bold Style
//#define BTNS_UNDERLINE			0x004000	//!< Toolbar Button Caption Underline Style
//#define BTNS_COLOR				0x008000	//!< Toolbar Button Caption Color Style
//#define BTNS_TBKGCOLOR			0x010000	//!< Toolbar Tooltip Background Color Style
//#define BTNS_TTXTCOLOR			0x020000	//!< Toolbar Tooltip Text Color Style
//#define BTNS_HIGHLIGHT_TXTCOLOR	0x040000	//!< Toolbar Buttons Highlight Text colour.
//#define BTNS_MARK_BKGCOLOR		0x080000	//!< Toolbar Buttons background colour when marked.
//#define BTNS_BTNCOLOR			0x100000	//!< Toolbar Buttons colour.
//#define BTNS_HIGHLIGHT_BTNCOLOR	0x200000	//!< Toolbar Buttons colour when highlighted.
//#define BTNS_HOTTRACK_BTNCOLOR	0x400000	//!< Toolbar Buttons colour when hot.

/*!
 * \brief blah
 *
 * blah
 */

struct DCXTBBUTTON
{
	TString tsTipText;				//!< Tooltip text
	TString bText;					//!< Buttons text, MUST be persistant.
	COLORREF clrText{ CLR_INVALID };				//!< Button Caption Color
	COLORREF clrMark{ CLR_INVALID };				//!< background color that the button will use when marked.
	COLORREF clrTextHighlight{ CLR_INVALID };		//!< Button Caption Color when highlighted. (An item is highlighted if it has the TBSTATE_MARKED style and is contained in a toolbar that has the TBSTYLE_FLAT style.)
	COLORREF clrBtnFace{ CLR_INVALID };			//!< The Buttons normal colour.
	COLORREF clrBtnHighlight{ CLR_INVALID };		//!< The Buttons highlight colour. (An item is highlighted if it has the TBSTATE_MARKED style and is contained in a toolbar that has the TBSTYLE_FLAT style.)
	COLORREF clrHighlightHotTrack{ CLR_INVALID };	//!< The Buttons hottrack colour (TBSTYLE_FLAT)
	bool bBold{ false };						//!< Is Button Caption Bold ?
	bool bUline{ false };					//!< Is Button Caption Underlined
	int iTextBkgMode{};				//!< Drawing mode for text on a normal button. (XP+)
	int iTextHighlightBkgMode{};		//!< Drawing mode for text on a highlighted button. (XP+) (An item is highlighted if it has the TBSTATE_MARKED style and is contained in a toolbar that has the TBSTYLE_FLAT style.)
};
using LPDCXTBBUTTON = DCXTBBUTTON*;

/*!
 * \brief blah
 *
 * blah
 */

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class DcxToolBar final
	: public DcxControl
{

public:
	DcxToolBar() = delete;
	DcxToolBar(const DcxToolBar&) = delete;
	DcxToolBar& operator =(const DcxToolBar&) = delete;
	DcxToolBar(DcxToolBar&&) = delete;
	DcxToolBar& operator =(DcxToolBar&&) = delete;

	DcxToolBar(const UINT ID, DcxDialog* const p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxToolBar() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;
	WindowExStyle parseControlStylesToolBar(const TString& tsStyles);

	HIMAGELIST getImageList(const dcxToolBar_ImageLists iImageList) const noexcept;
	void setImageList(const HIMAGELIST himl, const dcxToolBar_ImageLists iImageList) noexcept;
	static HIMAGELIST createImageList(const DcxIconSizes iSize) noexcept;

	LRESULT autoSize() noexcept;
	LRESULT insertButton(const int nPos, LPCTBBUTTON lpbb) noexcept;
	LRESULT hitTest(const LPPOINT lpt) const noexcept;
	LRESULT getItemRect(const int iButton, LPCRECT lprc) const noexcept;
	Dcx::CodeValue<RECT> getItemRect(const int iButton) const noexcept;
	LRESULT getButtonCount() const noexcept;
	LRESULT setButtonWidth(const int cxMin, const int cxMax) noexcept;
	LRESULT deleteButton(const int iButton) noexcept;
	LRESULT setButtonInfo(const int idButton, const LPTBBUTTONINFO lpbi) noexcept;
	LRESULT getButtonInfo(const int idButton, const LPTBBUTTONINFO lpbi) const noexcept;
	LRESULT getTooltips() const noexcept;
	LRESULT getButtonText(const int idButton, const LPSTR lpszText) const noexcept;
	LRESULT getButton(const int iButton, const LPTBBUTTON lpb) const noexcept;
	LRESULT setButtonState(const int idButton, const UINT fState) noexcept;
	LRESULT getCommandToIndex(const int idButton) const noexcept;
	LRESULT moveButton(const int iButtonFrom, const int iButtonTo) noexcept;
	LRESULT markButton(const int iButton, const bool fHighlight) noexcept;
	LRESULT setButtonSize(const int dxButton, const int dyButton) noexcept;
	LPDCXTBBUTTON getButtonData(const int idButton) const;

	void autoStretchButtons() noexcept;

	void autoPosition(const int width, const int height) noexcept;

	inline const TString getType() const final { return TEXT("toolbar"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::TOOLBAR; }

	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml(void) const final;
	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

private:
	HFONT m_hOldItemFont{ nullptr }; //!< Item fonts (only used during custom draw)
	HFONT m_hItemFont{ nullptr };

	bool m_bAutoStretch{ false }; //!< Auto Stretch Variable
	bool m_bOverrideTheme{ false }; //!< Allow drawing of custom colours when control is themed.

protected:

	static UINT parseImageListFlags(const TString& flags) noexcept;

	int getFreeButtonID() const noexcept;
	int getIndexToCommand(const int iIndex) const noexcept;

	static BYTE parseButtonStateFlags(const TString& flags) noexcept;
	static UINT parseButtonStyleFlags(const TString& flags) noexcept;

	void resetContent() noexcept;
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXTOOLBAR_H_
