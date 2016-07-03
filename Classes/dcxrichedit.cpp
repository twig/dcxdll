/*!
* \file dcxrichedit.cpp
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
#include "defines.h"
#include "Classes/dcxrichedit.h"
#include "Classes/dcxdialog.h"
#include "Dcx.h"
#include "Classes/Custom/RichEditThemed.h"

/*!
* \brief Constructor
*
* \param ID Control ID
* \param p_Dialog Parent DcxDialog Object
* \param mParentHwnd Parent Window Handle
* \param rc Window Rectangle
* \param styles Window Style Tokenized List
*/
DcxRichEdit::DcxRichEdit(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString &styles)
	: DcxControl(ID, p_Dialog)
	, m_iFontSize(10 * 20)
	, m_bFontBold(FALSE)
	, m_bFontItalic(FALSE)
	, m_bFontUnderline(FALSE)
	, m_bFontStrikeout(FALSE)
	, m_byteCharset(DEFAULT_CHARSET)
	, m_bIgnoreRepeat(true)
	, m_bIgnoreInput(false)
//	, m_tsFontFaceName("")
//	, m_tsText("")
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CLIENTEDGE,
		DCX_RICHEDITCLASS,
		nullptr,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd , L" ", L" ");
	else
		CRichEditThemed::Attach(m_Hwnd);

	this->m_clrBackText = GetSysColor(COLOR_WINDOW);
	this->m_clrText = GetSysColor(COLOR_WINDOWTEXT);

	this->loadmIRCPalette();
	this->setContentsFont();

	SendMessage(m_Hwnd, EM_SETEVENTMASK, NULL, (LPARAM) (ENM_SELCHANGE | ENM_CHANGE | ENM_LINK));
	//SendMessage(m_Hwnd, CCM_SETUNICODEFORMAT, TRUE, NULL);

	if (p_Dialog->getToolTip() != nullptr) {
		if (styles.istok(TEXT("tooltips"))) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, m_Hwnd);
		}
	}

	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
* \brief blah
*
* blah
*/
DcxRichEdit::~DcxRichEdit() {
	this->unregistreDefaultWindowProc();
}

/*!
* \brief blah
*
* blah
*/
void DcxRichEdit::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
{
	//*Styles |= ES_READONLY;
	//ES_NOHIDESEL
	for (const auto &tsStyle: styles)
	{
		if (tsStyle == TEXT("multi"))
			*Styles |= ES_MULTILINE | ES_WANTRETURN;
		else if (tsStyle == TEXT("readonly"))
			*Styles |= ES_READONLY;
		else if (tsStyle == TEXT("center"))
			*Styles |= ES_CENTER;
		else if (tsStyle == TEXT("right"))
			*Styles |= ES_RIGHT;
		else if (tsStyle == TEXT("autohs"))
			*Styles |= ES_AUTOHSCROLL;
		else if (tsStyle == TEXT("autovs"))
			*Styles |= ES_AUTOVSCROLL;
		else if (tsStyle == TEXT("vsbar"))
			*Styles |= WS_VSCROLL;
		else if (tsStyle == TEXT("hsbar"))
			*Styles |= WS_HSCROLL;
		else if (tsStyle == TEXT("disablescroll"))
			*Styles |= ES_DISABLENOSCROLL;
	}

	this->parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
}

/*!
* \brief $xdid Parsing Function
*
* \param input [NAME] [ID] [PROP] (OPTIONS)
* \param szReturnValue mIRC Data Container
*
* \return > void
*/
void DcxRichEdit::parseInfoRequest( const TString &input, PTCHAR szReturnValue) const
{
	const auto numtok = input.numtok();
	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP] [N]
	if (prop == TEXT("text")) {
		// determine the line number
		int line = 0;

		if (numtok > 3)
			line = input.getnexttok( ).to_int() -1;		// tok 4

		if ((line < 0) || (line >= Edit_GetLineCount(m_Hwnd)))
			throw Dcx::dcxException("Invalid line number.");

		// get index of first character in line
		const auto offset = SendMessage(m_Hwnd, EM_LINEINDEX, (WPARAM)line, NULL);
		// get length of the line we want to copy
		const auto len = SendMessage(m_Hwnd, EM_LINELENGTH, (WPARAM)offset, NULL) + 1;
		// create and fill the buffer
		auto p = std::make_unique<TCHAR[]>(len);
		*((LPWORD) p.get()) = (WORD)len;
		SendMessage(m_Hwnd, EM_GETLINE, (WPARAM) line, (LPARAM) p.get());

		// terminate the string at the right position
		p[len -1] = TEXT('\0');

		// copy to result
		dcx_strcpyn(szReturnValue, p.get(), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("num")) {
		if (this->isStyle(ES_MULTILINE))
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), (int)SendMessage(m_Hwnd, EM_GETLINECOUNT, 0, 0L));
		else {
			// single line control so always 1 line.
			szReturnValue[0] = TEXT('1');
			szReturnValue[1] = 0;
		}
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("caretpos")) {
		DWORD dwAbsoluteStartSelPos = 0;

		// caret startsel position
		SendMessage(m_Hwnd, EM_GETSEL, (WPARAM) &dwAbsoluteStartSelPos, NULL);

		if (this->isStyle(ES_MULTILINE)) {
			// current line
			const auto iLinePos = SendMessage(m_Hwnd, EM_LINEFROMCHAR, (WPARAM)-1, NULL);
			// line offset
			const auto iAbsoluteCharPos = (int)SendMessage(m_Hwnd, EM_LINEINDEX, (WPARAM)-1, NULL);

			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %u"), iLinePos +1, dwAbsoluteStartSelPos - iAbsoluteCharPos);
		}
		else {
			// return selstart
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %u"), 1, dwAbsoluteStartSelPos);
		}
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("selstart")) {
		CHARRANGE c = { 0 };

		SendMessage(m_Hwnd, EM_EXGETSEL, NULL, (LPARAM) &c);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), c.cpMin);
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("selend")) {
		CHARRANGE c = { 0 };

		SendMessage(m_Hwnd, EM_EXGETSEL, NULL, (LPARAM) &c);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), c.cpMax);
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("sel")) {
		CHARRANGE c = { 0 };

		SendMessage(m_Hwnd, EM_EXGETSEL, NULL, (LPARAM) &c);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), c.cpMin, c.cpMax);
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("seltext")) {
		CHARRANGE c = { 0 };

		SendMessage(m_Hwnd, EM_EXGETSEL, NULL, (LPARAM) &c);
		auto buffer = std::make_unique<TCHAR[]>(c.cpMax - c.cpMin);

		SendMessage(m_Hwnd, EM_GETSELTEXT, NULL, (LPARAM) buffer.get());
		dcx_strcpyn(szReturnValue, buffer.get(), MIRC_BUFFER_SIZE_CCH);
	}
	// [NAME] [ID] [PROP] [utf8|...]
	else if (prop == TEXT("rtftext")) {

		UINT iFlags = SF_RTF;
		const auto tsFlags(input.getnexttok());

		std::stringstream rtf;

		EDITSTREAM es = { 0 };
		es.dwCookie = (DWORD_PTR)&rtf;
		es.pfnCallback = &StreamOutToVarCallback;

		if (tsFlags == TEXT("utf8"))
			iFlags = (UINT)((CP_UTF8 << 16) | SF_USECODEPAGE | SF_RTF);

		SendMessage(m_Hwnd, EM_STREAMOUT, (WPARAM)iFlags, (LPARAM)&es);

		const TString tsOut(rtf.str().c_str());	// handles any char convertions needed.
		dcx_strcpyn(szReturnValue, tsOut.to_chr(), MIRC_BUFFER_SIZE_CCH);

		//TString rtf;
		//
		//EDITSTREAM es = { 0 };
		//es.dwCookie = (DWORD_PTR)&rtf;
		//es.pfnCallback = &EditStreamOutCallback;
		//
		//if (tsFlags == TEXT("utf8"))
		//	iFlags = (UINT)((CP_UTF8 << 16) | SF_USECODEPAGE | SF_RTF);
		//
		//SendMessage(m_Hwnd, EM_STREAMOUT, (WPARAM)iFlags, (LPARAM)&es);
		//
		//dcx_strcpyn(szReturnValue, rtf.to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

bool DcxRichEdit::SaveRichTextToFile(HWND hWnd, const TCHAR *const filename)
{
	auto hFile = CreateFile(filename, GENERIC_WRITE, 0, nullptr,	CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	Auto(CloseHandle(hFile));

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	EDITSTREAM es = { 0 };
	es.dwCookie = (DWORD)hFile;
	es.pfnCallback = StreamOutToFileCallback;
	SendMessage(hWnd, EM_STREAMOUT, SF_RTF, (LPARAM)&es);
	return (es.dwError == 0);
}

bool DcxRichEdit::LoadRichTextFromFile(HWND hWnd, const TCHAR *const filename)
{
	auto hFile = CreateFile(filename, GENERIC_READ, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	Auto(CloseHandle(hFile));

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	EDITSTREAM es = { 0 };
	es.dwCookie = (DWORD)hFile;
	es.pfnCallback = StreamInFromFileCallback;
	SendMessage(hWnd, EM_STREAMIN, SF_RTF, (LPARAM)&es);
	return (es.dwError == 0);
}

DWORD CALLBACK DcxRichEdit::StreamOutToVarCallback(DWORD_PTR dwCookie, const LPBYTE pbBuff, const LONG cb, LONG *pcb)
{
	auto rtf = reinterpret_cast<std::stringstream *>(dwCookie);

	rtf->write((char *)pbBuff, cb);

	*pcb = cb;
	return 0;

	//TString *rtf = reinterpret_cast<TString *>(dwCookie);
	//
	//rtf->append((char *)pbBuff, cb);
	//
	//*pcb = cb;
	//return 0;
}

DWORD CALLBACK DcxRichEdit::StreamOutToFileCallback(DWORD_PTR dwCookie, const LPBYTE pbBuff, const LONG cb, LONG *pcb)
{
	auto hFile = (HANDLE)dwCookie;
	DWORD NumberOfBytesWritten;
	if (!WriteFile(hFile, pbBuff, cb, &NumberOfBytesWritten, nullptr))
	{
		//handle errors
		return 1;
		// or perhaps return GetLastError();
	}
	*pcb = NumberOfBytesWritten;
	return 0;
}

DWORD CALLBACK DcxRichEdit::StreamInFromFileCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, const LONG cb, LONG *pcb)
{
	auto hFile = (HANDLE)dwCookie;
	DWORD NumberOfBytesRead;
	if (!ReadFile(hFile, pbBuff, cb, &NumberOfBytesRead, nullptr))
	{
		//handle errors
		return 1;
		// or perhaps return GetLastError();
	}
	*pcb = NumberOfBytesRead;
	return 0;
}

/*!
* \brief blah
*
* blah
*/
void DcxRichEdit::parseCommandRequest(const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')]) {
		this->m_tsText.clear();	// = TEXT("");
		this->clearContents();
	}

	// xdid -a [NAME] [ID] [SWITCH] [TEXT]
	if (flags[TEXT('a')] && numtok > 3) {
		//this->m_tsText += input.gettok(4, -1);
		//this->parseContents(TRUE);

		const auto tmp(input.getlasttoks());	// tok 4, -1

		this->m_tsText += tmp;
		this->parseStringContents(tmp, TRUE);
	}
	// xdid -c [NAME] [ID] [SWITCH]
	else if (flags[TEXT('c')] && numtok > 2) {
		CopyToClipboard(m_Hwnd, this->m_tsText);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')] && numtok > 3) {
		if (this->isStyle(ES_MULTILINE)) {
			const auto nLine = input.getnexttok().to_int();	// tok 4
			this->m_tsText.deltok(nLine, TEXT("\r\n"));
		}

		this->parseContents(TRUE);
	}
	// special richedit interception for font change
	// xdid -f [NAME] [ID] [SWITCH] [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
	else if (flags[TEXT('f')] && numtok > 3) {
		const auto iFontFlags = parseFontFlags(input.getnexttok());	// tok 4

		if (dcx_testflag(iFontFlags, dcxFontFlags::DCF_DEFAULT)) {
			this->m_clrBackText = GetSysColor(COLOR_WINDOW);
			this->m_clrText = GetSysColor(COLOR_WINDOWTEXT);
			this->m_iFontSize = 10 * 20;
			this->m_bFontBold = FALSE;
			this->m_bFontItalic = FALSE;
			this->m_bFontUnderline = FALSE;
			this->m_bFontStrikeout = FALSE;
			this->m_byteCharset = DEFAULT_CHARSET;
			this->setContentsFont();
		}
		else if (numtok > 5) {
			this->setRedraw(FALSE);

			this->m_byteCharset = (BYTE)parseFontCharSet(input.getnexttok( ));	// tok 5
			this->m_iFontSize = 20 * input.getnexttok( ).to_int();				// tok 6
			this->m_tsFontFaceName = input.getlasttoks().trim();				// tok 7, -1

			if (dcx_testflag(iFontFlags, dcxFontFlags::DCF_BOLD))
				this->m_bFontBold = TRUE;
			else
				this->m_bFontBold = FALSE;

			if (dcx_testflag(iFontFlags, dcxFontFlags::DCF_ITALIC))
				this->m_bFontItalic = TRUE;
			else
				this->m_bFontItalic = FALSE;

			if (dcx_testflag(iFontFlags, dcxFontFlags::DCF_STRIKEOUT))
				this->m_bFontStrikeout = TRUE;
			else
				this->m_bFontStrikeout = FALSE;

			if (dcx_testflag(iFontFlags, dcxFontFlags::DCF_UNDERLINE))
				this->m_bFontUnderline = TRUE;
			else
				this->m_bFontUnderline = FALSE;

			this->parseContents(TRUE);
		}
	}
	// xdid -i [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('i')] && numtok > 4) {
		if (this->isStyle(ES_MULTILINE)) {
			const auto nLine = input.getnexttok().to_<UINT>();								// tok 4
			this->m_tsText.instok(input.getlasttoks(), nLine, TEXT("\r\n"));	// tok 5, -1
		}
		else
			this->m_tsText = input.getlasttoks();										// tok 4, -1

		this->parseContents(TRUE);
	}
	// xdid -k [NAME] [ID] [SWITCH] [COLOR]
	else if (flags[TEXT('k')] && numtok > 3) {
		const auto clrColor = input.getnexttok().to_<COLORREF>();	// tok 4

		if (clrColor == CLR_INVALID)
			SendMessage(m_Hwnd, EM_SETBKGNDCOLOR, (WPARAM) 1, (LPARAM) GetSysColor(COLOR_WINDOWTEXT));
		else
			SendMessage(m_Hwnd, EM_SETBKGNDCOLOR, (WPARAM) 0, (LPARAM) clrColor);

		this->m_clrBackText = clrColor;
		this->redrawWindow();
	}
	// xdid -l [NAME] [ID] [SWITCH] [N] [COLOR]
	else if (flags[TEXT('l')] && numtok > 4) {
		const auto nColor = input.getnexttok( ).to_int() -1;	// tok 4

		if (nColor < 0 && nColor > 15)
			throw Dcx::dcxException("Invalid Colour");

		this->m_aColorPalette[nColor] = input.getnexttok().to_<COLORREF>();	// tok 5
		this->parseContents(TRUE);
	}
	// xdid -m [NAME] [ID] [SWITCH]
	else if (flags[TEXT('m')]) {
		this->loadmIRCPalette();
		this->parseContents(TRUE);
	}
	// xdid -n [NAME] [ID] [SWITCH] [BOOL]
	else if (flags[TEXT('n')] && numtok > 3) {
		const auto b = (input.getnexttok( ).to_int() > 0);	// tok 4

		this->setAutoUrlDetect(b ? TRUE : FALSE);
	}
	// xdid -o [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('o')] && numtok > 4) {
		if (this->isStyle(ES_MULTILINE)) {
			const auto nLine = input.getnexttok().to_<UINT>();								// tok 4
			this->m_tsText.puttok(input.getlasttoks(), nLine, TEXT("\r\n"));	// tok 5, -1
		}
		else
			this->m_tsText = input.getlasttoks( );	// tok 4, -1

		this->parseContents(TRUE);
	}
	// xdid -P [NAME] [ID]
	else if (flags[TEXT('P')] && numtok > 1) {
		SendMessage(this->getHwnd(),WM_PASTE,NULL,NULL);
	}
	// xdid -q -> [NAME] [ID] -q [COLOR1] ... [COLOR16]
	else if (flags[TEXT('q')] && numtok > 3) {
		const auto nColor = numtok - 3;

		if (nColor > 15)
			throw Dcx::dcxException("Invalid Colour");

		for (auto i = decltype(nColor){0}; (i < nColor && i < Dcx::countof(m_aColorPalette)); i++)
			m_aColorPalette[i] = input.getnexttok().to_<COLORREF>();	// tok 4 + i

		this->parseContents(TRUE);
	}
	// This is to avoid an invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
	}
	// xdid -t [NAME] [ID] [SWITCH] [FILENAME]
	// xdid -t [NAME] [ID] [SWITCH] [+FLAGS] [FILENAME]
	else if (flags[TEXT('t')] && numtok > 3) {
		const XSwitchFlags xflags(input.getnexttok().trim());	// tok 4
		bool bOldMethod = false;
		TString tsFile;

		if (!xflags[TEXT('+')])
		{
			bOldMethod = true;
			tsFile = input.gettok(4, -1 ).trim();	// 4, -1
		}
		else
			tsFile = input.getlasttoks( ).trim();	// 5, -1

		if (!IsFile(tsFile))
			throw Dcx::dcxException(TEXT("Unable to open: %"), tsFile);

		if (xflags[TEXT('o')])
			bOldMethod = true;

		if (bOldMethod)
		{
			this->m_tsText = readTextFile(tsFile);

			const auto mask = this->m_dEventMask;
			this->m_dEventMask = 0;
			this->parseContents(TRUE);
			this->m_dEventMask = mask;
		}
		else {
			// new methods... load rtf...
			if (!LoadRichTextFromFile(m_Hwnd, tsFile.to_chr()))
				throw Dcx::dcxException(TEXT("Unable to open: %"), tsFile);
		}
	}
	// xdid -u [NAME] [ID] [SWITCH] [FILENAME]
	// xdid -u [NAME] [ID] [SWITCH] [+FLAGS] [FILENAME]
	else if (flags[TEXT('u')] && numtok > 3) {
		const XSwitchFlags xflags(input.getnexttok().trim());	// tok 4
		bool bOldMethod = false, bRes = false;
		TString tsFile;

		if (!xflags[TEXT('+')])
		{
			bOldMethod = true;
			tsFile = input.gettok(4, -1).trim();	// 4, -1
		}
		else
			tsFile = input.getlasttoks().trim();	// 5, -1

		if (xflags[TEXT('o')])
			bOldMethod = true;

		if (bOldMethod)
		{
			// old way if no flags provided or old method selected.
			bRes = SaveDataToFile(tsFile, this->m_tsText);
		}
		else {
			// new method's...
			bRes = SaveRichTextToFile(m_Hwnd, tsFile.to_chr());
		}
		if (!bRes)
			throw Dcx::dcxException(TEXT("Unable to save: %"), tsFile);
	}
	// xdid -S [NAME] [ID] [SWITCH] [START] [END]
	else if (flags[TEXT('S')] && numtok > 3) {
		CHARRANGE c;

		c.cpMin = input.getnexttok( ).to_int();	// tok 4

		if (numtok > 4)
			c.cpMax = input.getnexttok( ).to_int();	// tok 5
		else
			c.cpMax = c.cpMin;

		SendMessage(m_Hwnd, EM_EXSETSEL, NULL, (LPARAM) &c);
		SendMessage(m_Hwnd, EM_SCROLLCARET, NULL, NULL);

		//DWORD dwAbsoluteStartSelPos = 0;
		//// caret startsel position
		//SendMessage(m_Hwnd, EM_GETSEL, (WPARAM) &dwAbsoluteStartSelPos, NULL);
		//
		//if (this->isStyle(ES_MULTILINE)) {
		//	int iLinePos = 0;
		//
		//	// current line
		//	iLinePos = SendMessage(m_Hwnd, EM_LINEFROMCHAR, -1, NULL);
		//	POINT pt = {0};
		//	pt.y = this->m_iFontSize * iLinePos;
		//	SendMessage(m_Hwnd, EM_SETSCROLLPOS,NULL,(LPARAM)&pt);
		//}
	}
	// xdid -V [NAME] [ID]
	else if (flags[TEXT('V')]) {
		SendMessage(m_Hwnd, EM_SCROLLCARET, NULL, NULL);
	}
	// xdid -y [NAME] [ID] [SWITCH] [0|1]
	else if (flags[TEXT('y')] && numtok > 3) {
		const auto state = input.getnexttok().to_int();	// tok 4

		m_bIgnoreRepeat = (state > 0);
	}
	// xdid -z [NAME] [ID] [SWITCH] [NUMERATOR] [DENOMINATOR]
	else if (flags[TEXT('z')] && numtok > 4) {
		const auto num = input.getnexttok().to_int();	// tok 4
		const auto den = input.getnexttok().to_int();	// tok 5

		if (!SendMessage(m_Hwnd, EM_SETZOOM, (WPARAM) num, (LPARAM) den))
			throw Dcx::dcxException("Richedit zooming error");
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
* \brief blah
*
* blah
*/
void DcxRichEdit::loadmIRCPalette() {
	TString colors;
	static const TCHAR com[] = TEXT("$color(0) $color(1) $color(2) $color(3) $color(4) $color(5) $color(6) $color(7) $color(8) $color(9) $color(10) $color(11) $color(12) $color(13) $color(14) $color(15)");
	mIRCLinker::tsEval(colors, com);

	const auto len = colors.numtok();
	if (len > Dcx::countof(m_aColorPalette))
		return;	// something went very wrong...

	UINT i = 0;
	for (const auto &tmp: colors)
		m_aColorPalette[i++] = tmp.to_<COLORREF>();	// tok i + 1
}

/*!
* \brief blah
*
* blah
*/
void DcxRichEdit::setContentsFont() {
	CHARFORMAT2 chrf;
	ZeroMemory(&chrf, sizeof(CHARFORMAT2));

	chrf.cbSize = sizeof(CHARFORMAT2);
	chrf.dwMask = CFM_BACKCOLOR | CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_ITALIC | CFM_UNDERLINE | CFM_STRIKEOUT | CFM_CHARSET;
	chrf.yHeight = this->m_iFontSize;
	chrf.crTextColor = this->m_clrText;
	chrf.crBackColor = this->m_clrBackText;
	chrf.bCharSet = this->m_byteCharset;

	if (this->m_bFontBold)
		chrf.dwEffects |= CFE_BOLD;

	if (this->m_bFontItalic)
		chrf.dwEffects |= CFE_ITALIC;

	if (this->m_bFontStrikeout)
		chrf.dwEffects |= CFE_STRIKEOUT;

	if (this->m_bFontUnderline)
		chrf.dwEffects |= CFE_UNDERLINE;

	if (!this->m_tsFontFaceName.empty()) {
		dcx_strcpyn(chrf.szFaceName, this->m_tsFontFaceName.to_chr(), 31);
		chrf.szFaceName[31] = 0;
	}

	this->hideSelection(TRUE);
	this->setSel(0, -1);
	this->setCharFormat(SCF_SELECTION, &chrf);
	this->setSel(0, 0);
	this->hideSelection(FALSE);
}

/*!
* \brief blah
*
* blah
*/
void DcxRichEdit::clearContents() {
	this->hideSelection(TRUE);
	this->setSel(0, -1);
	this->replaceSel(FALSE, TEXT(""));
	this->setSel(0, 0);
	this->hideSelection(FALSE);

	//this->setContentsFont( );
}
/*!
* \brief blah
*
* blah
*/
void DcxRichEdit::parseContents(const BOOL fNewLine) { // old function
	this->m_bIgnoreInput = false;
	this->setRedraw(FALSE);
	this->clearContents();

	//TCHAR *text = this->m_tsText.to_chr();
	//
	//bool uline = false;
	//bool bline = false;
	//COLORREF mcolor = 0;
	//COLORREF bkgcolor = 0;
	//
	//bool bmcolor = true;
	//bool bbkgcolor = false;
	//
	//TCHAR cbuf[2];
	//TCHAR colbuf[3];
	//cbuf[1] = TEXT('\0');
	//colbuf[2] = TEXT('\0');
	//
	//for (int i = 0; text[i] != TEXT('\0'); i++)
	//{
	//	cbuf[0] = text[i];
	//
	//	//// if current TCHAR is a control code
	//	//if (text[i] == 2 || text[i] == 3 || text[i] == 15 || text[i] == 22 || text[i] == 31) {
	//	//	// CTRL+B Parsing
	//	//	if (text[i] == 2)
	//	//		bline = !(bline);
	//	//
	//	//	// CTRL+K Parsing
	//	//	else if (text[i] == 3) {
	//	//		if (text[i +1] >= TEXT('0') && text[i +1] <= TEXT('9')) {
	//	//			colbuf[0] = text[i +1];
	//	//			colbuf[1] = TEXT('\0');
	//	//			++i;
	//	//
	//	//			if (text[i +1] >= TEXT('0') && text[i +1] <= TEXT('9')) {
	//	//				colbuf[1] = text[i +1];
	//	//				i++;
	//	//			}
	//	//
	//	//			// color code number
	//	//			bmcolor = true;
	//	//			mcolor = this->m_aColorPalette[this->unfoldColor(colbuf)];
	//	//
	//	//			// maybe a background color
	//	//			if (text[i+1] == TEXT(',')) {
	//	//				++i;
	//	//
	//	//				if (text[i +1] >= TEXT('0') && text[i +1] <= TEXT('9')) {
	//	//					colbuf[0] = text[i +1];
	//	//					colbuf[1] = TEXT('\0');
	//	//					i++;
	//	//
	//	//					if (text[i +1] >= TEXT('0') && text[i +1] <= TEXT('9')) {
	//	//						colbuf[1] = text[i +1];
	//	//						++i;
	//	//					}
	//	//
	//	//					// color code number
	//	//					bbkgcolor = true;
	//	//					bkgcolor = this->m_aColorPalette[this->unfoldColor(colbuf)];
	//	//				}
	//	//			}
	//	//		}
	//	//		else {
	//	//			bmcolor = false;
	//	//			bbkgcolor = false;
	//	//		}
	//	//	}
	//	//	// CTRL+O Parsing
	//	//	else if (text[i] == 15) {
	//	//		bline = false;
	//	//		uline = false;
	//	//		mcolor = CLR_INVALID;
	//	//		bkgcolor = CLR_INVALID;
	//	//	}
	//	//	// CTRL+R Parsing
	//	//	else if (text[i] == 22) {
	//	//		bline = false;
	//	//		uline = false;
	//	//	}
	//	//	// CTRL+U Parsing
	//	//	else if (text[i] == 31) {
	//	//		uline = !(uline);
	//	//	}
	//	//}
	//	//// regular TCHAR
	//	//else if (fNewLine && text[i] == TEXT('\r') && text[i +1] == TEXT('\n')) {
	//	//	bline = false;
	//	//	uline = false;
	//	//	mcolor = CLR_INVALID;
	//	//	bkgcolor = CLR_INVALID;
	//	//
	//	//	//cbuf[0] = text[i];
	//	//	//this->insertText( cbuf, bline, uline, bmcolor, mcolor, bbkgcolor, bkgcolor, 0 );
	//	//	++i;
	//	//	cbuf[0] = text[i];
	//	//	this->insertText(cbuf, bline, uline, bmcolor, mcolor, bbkgcolor, bkgcolor, 0);
	//	//}
	//	//else {
	//	//	this->insertText(cbuf, bline, uline, bmcolor, mcolor, bbkgcolor, bkgcolor, 0);
	//	//}
//
	//	switch (text[i])
	//	{
	//		// CTRL+B Parsing
	//	case 2:
	//		bline = !(bline);
	//		break;
	//		// CTRL+K Parsing
	//	case 3:
	//	{
	//			  if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9')) {
	//				  colbuf[0] = text[i + 1];
	//				  colbuf[1] = TEXT('\0');
	//				  ++i;
//
	//				  if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9')) {
	//					  colbuf[1] = text[i + 1];
	//					  i++;
	//				  }
//
	//				  // color code number
	//				  bmcolor = true;
	//				  mcolor = this->m_aColorPalette[this->unfoldColor(colbuf)];
//
	//				  // maybe a background color
	//				  if (text[i + 1] == TEXT(',')) {
	//					  ++i;
//
	//					  if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9')) {
	//						  colbuf[0] = text[i + 1];
	//						  colbuf[1] = TEXT('\0');
	//						  i++;
//
	//						  if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9')) {
	//							  colbuf[1] = text[i + 1];
	//							  ++i;
	//						  }
//
	//						  // color code number
	//						  bbkgcolor = true;
	//						  bkgcolor = this->m_aColorPalette[this->unfoldColor(colbuf)];
	//					  }
	//				  }
	//			  }
	//			  else {
	//				  bmcolor = false;
	//				  bbkgcolor = false;
	//			  }
	//	}
	//		break;
	//		// CTRL+O Parsing
	//	case 15:
	//	{
	//			   bline = false;
	//			   uline = false;
	//			   mcolor = CLR_INVALID;
	//			   bkgcolor = CLR_INVALID;
	//	}
	//		break;
	//		// CTRL+R Parsing
	//	case 22:
	//	{
	//			   bline = false;
	//			   uline = false;
	//	}
	//		break;
	//		// CTRL+U Parsing
	//	case 31:
	//		uline = !(uline);
	//		break;
	//		// regular TCHAR
	//	default:
	//	{
	//			   if (fNewLine && text[i] == TEXT('\r') && text[i + 1] == TEXT('\n'))
	//			   {
	//				   bline = false;
	//				   uline = false;
	//				   mcolor = CLR_INVALID;
	//				   bkgcolor = CLR_INVALID;
//
	//				   ++i;
	//				   cbuf[0] = TEXT('\n');
	//			   }
	//			   this->insertText(cbuf, bline, uline, bmcolor, mcolor, bbkgcolor, bkgcolor, 0);
	//	}
	//	}
	//}
	//this->setSel(0, 0);
	//this->setRedraw(TRUE);
	//this->redrawWindow();
//
	//this->m_bIgnoreInput = false;

	this->parseStringContents(this->m_tsText, fNewLine);
}

void DcxRichEdit::parseStringContents(const TString &tsStr, const BOOL fNewLine)
{
	this->m_bIgnoreInput = true;
	this->setRedraw(FALSE);

	const auto text = tsStr.to_chr();

	bool uline = false;
	bool bline = false;
	bool iline = false;
	COLORREF mcolor = 0;
	COLORREF bkgcolor = 0;

	bool bmcolor = true;
	bool bbkgcolor = false;

	TCHAR cbuf[2] = { 0 };
	TCHAR colbuf[3] = { 0 };

	for (auto i = 0U; text[i] != TEXT('\0'); i++)
	{
		switch (text[i])
		{
			// CTRL+B Parsing
		case 2:
			bline = !(bline);
			break;
			// CTRL+K Parsing
		case 3:
		{
				  if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9')) {
					  colbuf[0] = text[i + 1];
					  colbuf[1] = TEXT('\0');
					  ++i;

					  if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9')) {
						  colbuf[1] = text[i + 1];
						  i++;
					  }

					  // color code number
					  bmcolor = true;
					  mcolor = this->m_aColorPalette[this->unfoldColor(colbuf)];

					  // maybe a background color
					  if (text[i + 1] == TEXT(',')) {
						  ++i;

						  if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9')) {
							  colbuf[0] = text[i + 1];
							  colbuf[1] = TEXT('\0');
							  i++;

							  if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9')) {
								  colbuf[1] = text[i + 1];
								  ++i;
							  }

							  // color code number
							  bbkgcolor = true;
							  bkgcolor = this->m_aColorPalette[this->unfoldColor(colbuf)];
						  }
					  }
				  }
				  else {
					  bmcolor = false;
					  bbkgcolor = false;
				  }
		}
			break;
			// CTRL+O Parsing
		case 15:
		{
				   bline = false;
				   uline = false;
				   iline = false;
				   mcolor = CLR_INVALID;
				   bkgcolor = CLR_INVALID;
		}
			break;
			// CTRL+R Parsing
		case 22:
		{
				   bline = false;
				   uline = false;
				   iline = false;
		}
			break;
			// CTRL+I Parsing
		case 29:
			iline = !(iline);
			break;
			// CTRL+U Parsing
		case 31:
			uline = !(uline);
			break;
			// regular TCHAR
		default:
		{
				   if (fNewLine && text[i] == TEXT('\r') && text[i + 1] == TEXT('\n'))
				   {
					   bline = false;
					   uline = false;
					   mcolor = CLR_INVALID;
					   bkgcolor = CLR_INVALID;

					   ++i;
					   cbuf[0] = TEXT('\n');
				   }
				   else
					   cbuf[0] = text[i];

				   this->insertText(cbuf, bline, uline, iline, bmcolor, mcolor, bbkgcolor, bkgcolor, 0);
		}
		}
	}
	this->setSel(0, 0);
	this->setRedraw(TRUE);
	this->redrawWindow();

	this->m_bIgnoreInput = false;
}

/*!
* \brief blah
*
* blah
*/
int DcxRichEdit::unfoldColor(const TCHAR *color) {
	auto nColor = dcx_atoi(color);

	while (nColor > 15) {
		nColor -=16;
	}

	return nColor;
}

/*!
* \brief blah
*
* blah
*/
void DcxRichEdit::insertText(const TCHAR *const text, bool bline, bool uline, bool iline, bool bcolor, COLORREF color, bool bbkgcolor, COLORREF bkgcolor, int reverse) {
	// get total length
	auto len = GetWindowTextLength(m_Hwnd);
	// get line TCHAR number from end TCHAR pos
	const auto line = Edit_LineIndex(m_Hwnd, Edit_LineFromChar(m_Hwnd, len - 1));
	// get line length
	const auto linelen = Edit_LineLength(m_Hwnd, line);
	// total length of insert point
	len = line + linelen;
	this->hideSelection(TRUE);
	this->setSel(len, -1);
	this->replaceSel(FALSE, text);
	this->setSel(-1, 0);
	this->hideSelection(FALSE);
	CHARFORMAT2 chrf;
	ZeroMemory(&chrf, sizeof(CHARFORMAT2));
	chrf.cbSize = sizeof(CHARFORMAT2);
	chrf.dwMask = CFM_BACKCOLOR | CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_ITALIC | CFM_UNDERLINE | CFM_STRIKEOUT | CFM_CHARSET;
	chrf.yHeight = this->m_iFontSize;
	chrf.crTextColor = this->m_clrText;
	chrf.crBackColor = this->m_clrBackText;
	chrf.bCharSet = this->m_byteCharset;

	if (this->m_bFontBold)
		chrf.dwEffects |= CFE_BOLD;

	if (this->m_bFontItalic)
		chrf.dwEffects |= CFE_ITALIC;

	if (this->m_bFontStrikeout)
		chrf.dwEffects |= CFE_STRIKEOUT;

	if (this->m_bFontUnderline)
		chrf.dwEffects |= CFE_UNDERLINE;

	if (!this->m_tsFontFaceName.empty())
		dcx_strcpyn(chrf.szFaceName, this->m_tsFontFaceName.to_chr(), 32);

	if (bcolor)
		chrf.crTextColor = color;
	if (bbkgcolor)
		chrf.crBackColor = bkgcolor;
	if (bline)
		chrf.dwEffects |= CFE_BOLD;
	if (uline)
		chrf.dwEffects |= CFE_UNDERLINE;
	if (iline)
		chrf.dwEffects |= CFE_ITALIC;

	this->hideSelection(TRUE);
	this->setSel(len, -1);
	this->setCharFormat(SCF_SELECTION, &chrf);
	this->setSel(0, 0);
	this->hideSelection(FALSE);
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxRichEdit::setAutoUrlDetect(const BOOL iEnable) {
	return SendMessage(m_Hwnd, EM_AUTOURLDETECT, (WPARAM) iEnable, (LPARAM) 0);
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxRichEdit::hideSelection(const BOOL iHide) {
	return SendMessage(m_Hwnd, EM_HIDESELECTION, (WPARAM) iHide, (LPARAM) 0);
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxRichEdit::setSel(const int iStart, const int iEnd) {
	return SendMessage(m_Hwnd, EM_SETSEL, (WPARAM) iStart, (LPARAM) iEnd);
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxRichEdit::replaceSel(const BOOL bUndo, LPCTSTR lpstr) {
	return SendMessage(m_Hwnd, EM_REPLACESEL, (WPARAM) bUndo, (LPARAM) lpstr);
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxRichEdit::getCharFormat(const UINT iType, CHARFORMAT2 *cfm) const {
	return SendMessage(m_Hwnd, EM_GETCHARFORMAT, (WPARAM) iType, (LPARAM) cfm);
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxRichEdit::setCharFormat(const UINT iType, CHARFORMAT2 *cfm) {
	return SendMessage(m_Hwnd, EM_SETCHARFORMAT, (WPARAM) iType, (LPARAM) cfm);
}

void DcxRichEdit::toXml(TiXmlElement *const xml) const {
	__super::toXml(xml);
	auto text = new TiXmlText(this->m_tsText.c_str());
	xml->LinkEndChild(text);
}

const TString DcxRichEdit::getStyles(void) const {
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, ES_MULTILINE) && dcx_testflag(Styles, ES_WANTRETURN))
		styles.addtok(TEXT("multi"));
	if (dcx_testflag(Styles, ES_READONLY))
		styles.addtok(TEXT("readonly"));
	if (dcx_testflag(Styles, ES_CENTER))
		styles.addtok(TEXT("center"));
	if (dcx_testflag(Styles, ES_RIGHT))
		styles.addtok(TEXT("right"));
	if (dcx_testflag(Styles, ES_AUTOHSCROLL))
		styles.addtok(TEXT("autohs"));
	if (dcx_testflag(Styles, WS_VSCROLL))
		styles.addtok(TEXT("vsbar"));
	if (dcx_testflag(Styles, WS_HSCROLL))
		styles.addtok(TEXT("hsbar"));
	if (dcx_testflag(Styles, ES_DISABLENOSCROLL))
		styles.addtok(TEXT("disablescroll"));
	return styles;
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxRichEdit::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) {
	switch (uMsg) {
		case WM_NOTIFY:
		{
			dcxlParam(LPNMHDR, hdr);

			if (hdr == nullptr)
				break;

			switch(hdr->code) {
				case EN_LINK:
				{
					if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
					{
						dcxlParam(ENLINK *, enl);

						if ((enl->msg == WM_LBUTTONDOWN) ||
							(enl->msg == WM_LBUTTONDBLCLK) ||
							(enl->msg == WM_RBUTTONDOWN))
						{
							TEXTRANGE tr;
							TString tsEvent;

							// get information about link text
							ZeroMemory(&tr, sizeof(TEXTRANGE));
							tr.chrg = enl->chrg;
							const auto strlen = (enl->chrg.cpMax - enl->chrg.cpMin) + 1;

							auto str = std::make_unique<TCHAR[]>(strlen);

							tr.lpstrText = str.get();
							SendMessage(m_Hwnd, EM_GETTEXTRANGE, NULL, (LPARAM) &tr);

							if (enl->msg == WM_LBUTTONDOWN)
								tsEvent = TEXT("sclick");
							else if (enl->msg == WM_LBUTTONDBLCLK)
								tsEvent = TEXT("dclick");
							else if (enl->msg == WM_RBUTTONDOWN)
								tsEvent = TEXT("rclick");

							this->execAliasEx(TEXT("%s,%d,%s,%s"), TEXT("link"), this->getUserID(), tsEvent.to_chr(), tr.lpstrText);
						}
					}
					break;
				} // EN_LINK
				//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/richedit/richeditcontrols/richeditcontrolreference/richeditmessages/em_gettextrange.asp
				case EN_SELCHANGE:
				{
					if (this->m_bIgnoreInput)
						break;

					if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT))
					{
						auto sel = reinterpret_cast<SELCHANGE*>(lParam);

						if (sel->seltyp != SEL_EMPTY) {
							TEXTRANGE tr;

							// get information about selected text
							ZeroMemory(&tr, sizeof(TEXTRANGE));
							tr.chrg = sel->chrg;
							auto str = std::make_unique<TCHAR[]>((sel->chrg.cpMax - sel->chrg.cpMin) +1);
							tr.lpstrText = str.get();
							SendMessage(m_Hwnd, EM_GETTEXTRANGE, NULL, (LPARAM) &tr);

							this->execAliasEx(TEXT("%s,%d,%d,%d,%s"), TEXT("selchange"), this->getUserID(), sel->chrg.cpMin, sel->chrg.cpMax, tr.lpstrText);
						}
					}
					break;
				} // EN_SELCHANGE
			} // SWITCH 

			break;
		} // WM_NOTIFY
	}

	return 0L;
}

LRESULT DcxRichEdit::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) {
	switch(uMsg) {

		case WM_KEYDOWN:
		{
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT))
			{
				if (wParam == VK_RETURN)
					this->execAliasEx(TEXT("%s,%d"), TEXT("return"), this->getUserID());

				if ((this->m_bIgnoreRepeat) && (lParam & 0x40000000)) // ignore repeats
					break;

				this->execAliasEx(TEXT("%s,%d,%d"), TEXT("keydown"), this->getUserID(), wParam);
			}
			break;
		}
		case WM_KEYUP:
		{
			if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT))
				this->execAliasEx(TEXT("%s,%d,%d"), TEXT("keyup"), this->getUserID(), wParam);
			break;
		}
		//case WM_PAINT:
		//	{
		//		if (!this->m_bAlphaBlend)
		//			break;
		//		PAINTSTRUCT ps;
		//		HDC hdc;
		//
		//		hdc = BeginPaint( m_Hwnd, &ps );
		//
		//		LRESULT res = 0L;
		//		bParsed = TRUE;
		//
		//		// Setup alpha blend if any.
		//		LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);
		//
		//		// fill background.
		//		//DcxControl::DrawCtrlBackground(hdc,this,&ps.rcPaint);
		//
		//		res = CallWindowProc( this->m_DefaultWindowProc, m_Hwnd, uMsg, (WPARAM) hdc, lParam );
		//		//res = CallWindowProc( this->m_DefaultWindowProc, m_Hwnd, WM_PRINT, (WPARAM) hdc, (LPARAM) (PRF_CLIENT|PRF_NONCLIENT|PRF_CHILDREN|PRF_OWNED) );
		//
		//		this->FinishAlphaBlend(ai);
		//
		//		EndPaint( m_Hwnd, &ps );
		//		return res;
		//	}
		//	break;
		//case WM_PRINT:
		//	{
		//		HDC hdc = (HDC)wParam;
		//		bParsed = TRUE;
		//
		//		if (lParam & PRF_ERASEBKGND)
		//			SendMessage(m_Hwnd, WM_ERASEBKGND, wParam, 0L);
		//
		//		if (lParam & PRF_CLIENT)
		//			SendMessage(m_Hwnd, WM_PRINTCLIENT, wParam, 0L);
		//
		//	}
		//	break;
		//
		//case WM_PRINTCLIENT:
		//	{
		//		HDC hdc = (HDC)wParam;
		//		bParsed = TRUE;
		//
		//		HDC tHDC = GetDC(m_Hwnd);
		//		BITMAP bm;
		//		HBITMAP cBmp = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
		//
		//		GetObject(cBmp, sizeof(BITMAP), &bm);
		//
		//		BitBlt(hdc,0,0,bm.bmWidth, bm.bmHeight, tHDC,0,0, SRCCOPY);
		//
		//		ReleaseDC(m_Hwnd, tHDC);
		//	}
		//	break;

		case WM_DESTROY:
		{
			delete this;
			bParsed = TRUE;
			break;
		}
		default:
			return this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return 0L;
}
