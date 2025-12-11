/*!
 * \file dcxradio.h
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

#ifndef _DCXRADIO_H_
#define _DCXRADIO_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

class DcxRadio final
	: public DcxControl
{
public:
	DcxRadio() = delete;
	DcxRadio(const DcxRadio &) = delete;
	GSL_SUPPRESS(c.128) DcxRadio &operator =(const DcxRadio &) = delete;
	DcxRadio(DcxRadio &&) = delete;
	GSL_SUPPRESS(c.128) DcxRadio &operator =(DcxRadio &&) = delete;

	DcxRadio(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxRadio( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) noexcept final;

	TString parseInfoRequest(const TString& input) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("radio"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::RADIO; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;
	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure

	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

private:
	struct dcxRadioData
	{
		WindowStyle m_WinStyle{};

		HTHEME m_hTheme{};

		RECT m_rc{};
		RECT m_rcRadio{};
		RECT m_rcText{};
		RECT m_rcFocus{};

		SIZE m_szRadioSize{};

		HDC m_hdcPaint{};

		int m_ButtonState{};
		int m_iThemeState{};

		UINT m_DrawFrameState{};
		UINT m_DrawState{};

		DWORD m_textFlags{ DT_LEFT | DT_SINGLELINE };

		bool m_bDisabled{};
		bool m_bChecked{};
		bool m_bFocus{};
		bool m_bHot{};
		bool m_bPressed{};
		bool m_bRightJustify{};
		bool m_bVCenter{};
		bool m_bHCenter{};
		bool m_bRightButton{};

		TString m_tsText;
	};
	void DrawClientArea();
	void DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam);
	void DrawClientAreaThemed(HDC hdc, const UINT uMsg, LPARAM lParam);
	void DrawClientAreaNoTheme(HDC hdc, const UINT uMsg, LPARAM lParam);
};

#endif // _DCXRADIO_H_
