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

#include "../defines.h"
#include "dcxcontrol.h"
#include <richedit.h>
class DcxDialog;

/*!
* \brief blah
*
* blah
*/

class DcxRichEdit : public DcxControl {
public:
	//DcxRichEdit(UINT ID, DcxDialog *p_Dialog, RECT *rc, TString &styles);
	DcxRichEdit(UINT ID, DcxDialog *p_Dialog, HWND mParentHwnd, RECT *rc, TString &styles);
	virtual ~DcxRichEdit();

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed);

	void parseInfoRequest(TString &input, char *szReturnValue);
	void parseCommandRequest(TString &input);
	void parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme);
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed);

	void loadmIRCPalette();

	void setContentsFont();
	void clearContents();
	void parseContents(BOOL fNewLine);
	void insertText(char *text, bool bline, bool uline, bool bcolor, COLORREF color, bool bbkgcolor, COLORREF bkgcolor, int reverse);

	LRESULT setAutoUrlDetect(BOOL iEnable);
	LRESULT hideSelection(BOOL iHide);
	LRESULT setSel(int iStart, int iEnd);
	LRESULT replaceSel(BOOL bUndo, LPCTSTR lpstr);
	LRESULT getCharFormat(UINT iType, CHARFORMAT2 *cfm);
	LRESULT setCharFormat(UINT iType, CHARFORMAT2 *cfm);

	LRESULT getLineLength();
	LRESULT getLineIndex();

	inline TString getType() { return TString("richedit"); };

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

	static int unfoldColor(const char *color);
};

#endif // _DCXRICHEDIT_H_