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
 * � ScriptsDB.org - 2006
 */

#ifndef _DCXTAB_H_
#define _DCXTAB_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

#include "custom/peek.h"

class DcxDialog;

struct DCXTCITEM
{
	TString		tsTipText;	//!< Tooltip Text
	HWND		mChildHwnd{ nullptr };	//!< Hwnd to child window
};
using LPDCXTCITEM = DCXTCITEM *;

class DcxTab final
	: public DcxControl
{
public:
	DcxTab() = delete;
	DcxTab(const DcxTab &) = delete;
	GSL_SUPPRESS(c.128) DcxTab &operator =(const DcxTab &) = delete;
	DcxTab(DcxTab &&) = delete;
	GSL_SUPPRESS(c.128) DcxTab &operator =(DcxTab &&) = delete;

	DcxTab(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxTab( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest( const TString & input ) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	void loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc) override;

	inline const TString getType() const final { return TEXT("tab"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::TABB; };

	const TString getStyles(void) const final;
	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

	HIMAGELIST getImageList( ) const noexcept;
	HIMAGELIST setImageList( const HIMAGELIST himl ) noexcept;

	void deleteLParamInfo( const int nItem ) noexcept;

	void activateSelectedTab( );
	void activateTab(int nSel);
	void updateAllTab();

	bool getTab(const int index, const LPTCITEM tcItem) const noexcept;
	int getTabCount() const noexcept;
	DcxControl* addTab(int nIndex, int iIcon, const TString &tsText, const TString &tsCtrl, const TString &tsTooltip);

protected:
#if DCX_USE_GDIPLUS
	void DrawGlow(const int nTabIndex, HDC hDC, const RECT& rect) const;
#endif
	static RECT GetCloseButtonRect(const RECT& rcItem) noexcept;
	int HitTestOnItem() const noexcept;
	bool CloseButtonHitTest(_In_ const int iTab) const noexcept;

	LRESULT DrawClientArea(HDC hdc, UINT uMsg, LPARAM lParam);

	void CreatePeek() noexcept;
	void SetPeekSource(_In_ int iTab, _In_ int iTabSel, _In_opt_ LPCRECT rcItem) noexcept;
	void ShowPeek(int x, int y) noexcept;
	void HidePeek() noexcept;

	bool m_bClosable{ false };	//!< Does tab have a close button.
	//bool m_bGradient{ false };	//!< Does tab have a gradient fill. (only closeable)
	bool m_bPeek{};

	bool m_bTracking{};
	bool m_bHover{};
	bool m_bHot{};
	int m_iHotItem{ -1 };
	int m_iHoverItem{ -1 };
	UINT m_iHoverDelay{ HOVER_DEFAULT };
	HWND m_hPeek{};
	HFONT m_hPeekFont{};
};

#endif // _DCXTAB_H_
