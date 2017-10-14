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

struct DCXTCITEM {

	TString		tsTipText;	//!< Tooltip Text
	HWND		mChildHwnd;	//!< Hwnd to child window

	DCXTCITEM()
		: mChildHwnd(nullptr)
	{}
	~DCXTCITEM() = default;

};
using LPDCXTCITEM = DCXTCITEM *;

/*!
 * \brief blah
 *
 * blah
 */

class DcxTab : public DcxControl {

public:
	DcxTab() = delete;
	DcxTab(const DcxTab &) = delete;
	DcxTab &operator =(const DcxTab &) = delete;	// No assignments!

	DcxTab(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxTab( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest( const TString & input ) override;
	//void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;
	std::tuple<NoTheme, WindowStyle, WindowExStyle> parseControlStyles(const TString & tsStyles) override;

	inline const TString getType() const override { return TEXT("tab"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::TABB; };

	const TString getStyles(void) const override;
	void toXml(TiXmlElement *const xml) const override;
	TiXmlElement * toXml(void) const override;

	HIMAGELIST getImageList( ) const;
	void setImageList( HIMAGELIST himl );
	//static HIMAGELIST createImageList( );

	void deleteLParamInfo( const int nItem );
	void activateSelectedTab( );
	void getTab(const int index, LPTCITEM tcItem) const;
	int getTabCount() const;

#if DCX_USE_GDIPLUS
	void DrawGlow(const int32_t nTabIndex, HDC hDC, const RECT &rect) const;
#endif

protected:
	//static void GetCloseButtonRect(const RECT& rcItem, RECT& rcCloseButton);
	static RECT GetCloseButtonRect(const RECT& rcItem) noexcept;
	int HitTestOnItem() const;

	bool m_bClosable;	//!< Does tab have a close button.
	bool m_bGradient;	//!< Does tab have a gradient fill. (only closeable)
};

#endif // _DCXTAB_H_
