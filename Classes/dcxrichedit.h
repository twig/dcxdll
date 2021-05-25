/*!
* \file dcxrichedit.h
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

#ifndef _DCXRICHEDIT_H_
#define _DCXRICHEDIT_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include <richedit.h>
#include <sstream>

class DcxDialog;

struct DCXSTREAM {
	TString		ds_tsBuffer;
	FILE *		ds_pFile{ nullptr };
	bool		ds_bBold{ false };
	bool		ds_bUnderline{ false };
	bool		ds_bReverse{ false };
	bool		ds_bmcolor{ false };
	bool		ds_bbkgcolor{ false };
	COLORREF	ds_mcolor{ CLR_INVALID };
	COLORREF	ds_bkgcolor{ CLR_INVALID };
};
using LPDCXSTREAM = DCXSTREAM *;

/*!
* \brief blah
*
* blah
*/

class DcxRichEdit final
	: public DcxControl
{
public:
	DcxRichEdit() = delete;
	DcxRichEdit(const DcxRichEdit &) = delete;
	DcxRichEdit &operator =(const DcxRichEdit &) = delete;
	DcxRichEdit(DcxRichEdit &&) = delete;
	DcxRichEdit &operator =(DcxRichEdit &&) = delete;

	DcxRichEdit(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString &styles);
	~DcxRichEdit() noexcept = default;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest( const TString & input ) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	void setContentsFont() noexcept;
	void clearContents() noexcept;
	void parseContents(const BOOL fNewLine) noexcept;

	LRESULT setAutoUrlDetect(const BOOL iEnable) noexcept;
	LRESULT hideSelection(const BOOL iHide) noexcept;
	LRESULT setSel(const int iStart, const int iEnd) noexcept;
	LRESULT replaceSel(const BOOL bUndo, LPCTSTR lpstr) noexcept;
	LRESULT getCharFormat(const UINT iType, const CHARFORMAT2 *cfm) const noexcept;
	LRESULT setCharFormat(const UINT iType, const CHARFORMAT2 *cfm) noexcept;

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

	//LRESULT getLineLength();
	//LRESULT getLineIndex();

	inline const TString getType() const final { return TEXT("richedit"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::RICHEDIT; }

protected:
	TString m_tsText;			//!< RichEdit Text contents
	TString m_tsFontFaceName;	//!< Font Face Name

	COLORREF m_aColorPalette[mIRC_PALETTE_SIZE]{ CLR_INVALID }; //!< Richedit color palette to parse control codes

	UINT m_iFontSize{ 10 * 20 };			//!< Control Font Size
	BYTE m_byteCharset{ DEFAULT_CHARSET };			//!< Font CharSet

	bool m_bFontBold{ false };			//!< Is Font Bolded ?
	bool m_bFontItalic{ false };			//!< Is Font Italicized ?
	bool m_bFontUnderline{ false };		//!< Is Font Underlined ?
	bool m_bFontStrikeout{ false };		//!< Is Font StrikedOut ?
	bool m_bIgnoreInput{ false };		//!< Ignore keyboard input?
	bool m_bIgnoreRepeat{ true };		//!< Ignore repeat keys?

	// allows quickly changing stream from wchar to char etc..
	using stringstream_type = std::stringstream;

	static DWORD CALLBACK StreamOutToVarCallback(DWORD_PTR dwCookie, const LPBYTE pbBuff, const LONG cb, LONG *pcb);
	static DWORD CALLBACK StreamOutToFileCallback(DWORD_PTR dwCookie, const LPBYTE pbBuff, const LONG cb, LONG *pcb) noexcept;
	static DWORD CALLBACK StreamInFromFileCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, const LONG cb, LONG *pcb) noexcept;
	static bool SaveRichTextToFile(HWND hWnd, const TString &tsFilename) noexcept;
	static bool LoadRichTextFromFile(HWND hWnd, const TString &tsFilename) noexcept;
	static bool LoadRichTextFromXml(HWND hWnd, TString &tsFilename, const TString &tsDataSet);

	void insertText(const TCHAR *const text, bool bline, bool uline, bool iline, bool bcolor, COLORREF color, bool bbkgcolor, COLORREF bkgcolor, int reverse) noexcept;
	void parseStringContents(const TString &tsStr, const BOOL fNewLine) noexcept;

	bool m_bShowLineNumbers{ false };
	COLORREF m_clrGutter_selbkg{ RGB(0xFF, 0xf0, 0xff) };
	COLORREF m_clrGutter_bkg{ RGB(0xFF, 0xf0, 0xff) };
	COLORREF m_clrGutter_txt{ RGB(0xAF, 0xAF, 0xAF) };
	COLORREF m_clrGutter_seltxt{ RGB(0x0F, 0x0F, 0x0F) };

#define WM_DRAW_NUMBERS (WM_USER + 1000)
#define DCX_EDIT_GUTTER_WIDTH 35

	void DrawGutter();
	RECT getFmtRect() const noexcept
	{
		RECT rc{};
		Edit_GetRect(m_Hwnd, &rc);
		return rc;
	}
	void setFmtRect(bool bReset = false) noexcept
	{
		//if (!m_bShowLineNumbers)
		//	return;

		RECT rcClient{};
		GetClientRect(m_Hwnd, &rcClient);
		if (bReset)
		{
			Edit_SetRect(m_Hwnd, &rcClient);
		}
		else {
			rcClient.left += DCX_EDIT_GUTTER_WIDTH;
			RECT rcFmt = getFmtRect();
			//if (!EqualRect(&rcClient, &rcFmt))
			if (rcFmt.left < 5)
			{
				rcFmt = rcClient;
				Edit_SetRect(m_Hwnd, &rcFmt);
			}
		}
	}
	Dcx::range_t<DWORD> GetVisibleRange() noexcept;
	DWORD GetCaretPos() noexcept;
	DWORD GetCaretLine() noexcept;

};

#endif // _DCXRICHEDIT_H_
