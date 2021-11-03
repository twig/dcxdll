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

struct DCXTCITEM
{
	TString		tsTipText;	//!< Tooltip Text
	HWND		mChildHwnd{ nullptr };	//!< Hwnd to child window
};
using LPDCXTCITEM = DCXTCITEM *;

/*!
 * \brief blah
 *
 * blah
 */

class DcxTab final
	: public DcxControl
{
public:
	DcxTab() = delete;
	DcxTab(const DcxTab &) = delete;
	DcxTab &operator =(const DcxTab &) = delete;
	DcxTab(DcxTab &&) = delete;
	DcxTab &operator =(DcxTab &&) = delete;

	DcxTab(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxTab( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest( const TString & input ) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("tab"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::TABB; };

	const TString getStyles(void) const final;
	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

	HIMAGELIST getImageList( ) const noexcept;
	void setImageList( const HIMAGELIST himl ) noexcept;
	//static HIMAGELIST createImageList( );

	void deleteLParamInfo( const int nItem ) noexcept;
	void activateSelectedTab( );
	void getTab(const int index, const LPTCITEM tcItem) const noexcept;
	int getTabCount() const noexcept;

#if DCX_USE_GDIPLUS
	void DrawGlow(const int nTabIndex, HDC hDC, const RECT &rect) const;
#endif

protected:
	//static void GetCloseButtonRect(const RECT& rcItem, RECT& rcCloseButton);
	static RECT GetCloseButtonRect(const RECT& rcItem) noexcept;
	int HitTestOnItem() const noexcept;
	bool CloseButtonHitTest(const int iTab) const noexcept;

	LRESULT DrawClientArea(HDC hdc, UINT uMsg, LPARAM lParam);

	bool m_bClosable{ false };	//!< Does tab have a close button.
	bool m_bGradient{ false };	//!< Does tab have a gradient fill. (only closeable)
};

#endif // _DCXTAB_H_
