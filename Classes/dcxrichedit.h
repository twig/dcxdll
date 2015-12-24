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
* © ScriptsDB.org - 2006
*/

#ifndef _DCXRICHEDIT_H_
#define _DCXRICHEDIT_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include <richedit.h>
#include <sstream>

class DcxDialog;

typedef struct {
	TString		ds_tsBuffer;
	FILE *		ds_pFile;
	bool		ds_bBold;
	bool		ds_bUnderline;
	bool		ds_bReverse;
	bool		ds_bmcolor;
	bool		ds_bbkgcolor;
	COLORREF	ds_mcolor;
	COLORREF	ds_bkgcolor;
} DCXSTREAM, LPDCXSTREAM;

/*!
* \brief blah
*
* blah
*/

class DcxRichEdit : public DcxControl {
public:
	DcxRichEdit() = delete;
	DcxRichEdit(const DcxRichEdit &) = delete;
	DcxRichEdit &operator =(const DcxRichEdit &) = delete;	// No assignments!

	DcxRichEdit(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString &styles);
	virtual ~DcxRichEdit();

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) override;

	void parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const override;
	void parseCommandRequest( const TString & input ) override;
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) override;

	void loadmIRCPalette();

	void setContentsFont();
	void clearContents();
	void parseContents(const BOOL fNewLine);

	LRESULT setAutoUrlDetect(const BOOL iEnable);
	LRESULT hideSelection(const BOOL iHide);
	LRESULT setSel(const int iStart, const int iEnd);
	LRESULT replaceSel(const BOOL bUndo, LPCTSTR lpstr);
	LRESULT getCharFormat(const UINT iType, CHARFORMAT2 *cfm) const;
	LRESULT setCharFormat(const UINT iType, CHARFORMAT2 *cfm);
	void toXml(TiXmlElement *const xml) const override;
	const TString getStyles(void) const override;

	//LRESULT getLineLength();
	//LRESULT getLineIndex();

	inline const TString getType() const override { return TEXT("richedit"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::RICHEDIT; }

protected:
	TString m_tsText; //!< RichEdit Text contents

	COLORREF m_aColorPalette[16]; //!< Richedit color palette to parse control codes

	UINT m_iFontSize;         //!< Control Font Size
	BYTE m_byteCharset;        //!< Font CharSet
	BOOL m_bFontBold;         //!< Is Font Bolded ?
	BOOL m_bFontItalic;       //!< Is Font Italicized ?
	BOOL m_bFontUnderline;    //!< Is Font Underlined ?
	BOOL m_bFontStrikeout;    //!< Is Font StrikedOut ?
	TString m_tsFontFaceName; //!< Font Face Name

	BOOL m_bIgnoreInput;
	bool m_bIgnoreRepeat;

	static int unfoldColor(const TCHAR *color);
	static DWORD CALLBACK StreamOutToVarCallback(DWORD_PTR dwCookie, const LPBYTE pbBuff, const LONG cb, LONG *pcb);
	static DWORD CALLBACK StreamOutToFileCallback(DWORD_PTR dwCookie, const LPBYTE pbBuff, const LONG cb, LONG *pcb);
	static DWORD CALLBACK StreamInFromFileCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, const LONG cb, LONG *pcb);
	static bool SaveRichTextToFile(HWND hWnd, const TCHAR *const filename);
	static bool LoadRichTextFromFile(HWND hWnd, const TCHAR *const filename);

	void insertText(const TCHAR *const text, bool bline, bool uline, bool iline, bool bcolor, COLORREF color, bool bbkgcolor, COLORREF bkgcolor, int reverse);
	void parseStringContents(const TString &tsStr, const BOOL fNewLine);
};

#endif // _DCXRICHEDIT_H_
