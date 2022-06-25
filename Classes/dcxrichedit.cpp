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
DcxRichEdit::DcxRichEdit(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WS_EX_CLIENTEDGE,
		DCX_RICHEDITCLASS,
		ws.m_Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");
	else
		if (!CRichEditThemed::Attach(m_Hwnd))
			throw DcxExceptions::dcxUnableToCreateWindow();

	setNoThemed(ws.m_NoTheme);

	this->setBackColor(GetSysColor(COLOR_WINDOW));
	this->setTextColor(GetSysColor(COLOR_WINDOWTEXT));

	getmIRCPalette(gsl::span<COLORREF>(m_aColorPalette), false);

	this->setContentsFont();

	SendMessage(m_Hwnd, EM_SETEVENTMASK, 0, (ENM_SELCHANGE | ENM_CHANGE | ENM_LINK));
	//SendMessage(m_Hwnd, CCM_SETUNICODEFORMAT, TRUE, 0);

	if (styles.istok(TEXT("tooltips")))
	{
		if (!IsWindow(p_Dialog->getToolTip()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		setToolTipHWND(p_Dialog->getToolTip());
		AddToolTipToolInfo(getToolTipHWND(), m_Hwnd);
	}
}

dcxWindowStyles DcxRichEdit::parseControlStyles(const TString& tsStyles)
{
	auto ws = parseGeneralControlStyles(tsStyles);

	//Styles |= ES_READONLY;
	//ES_NOHIDESEL

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"multi"_hash:
			ws.m_Styles |= ES_MULTILINE | ES_WANTRETURN;
			break;
		case L"readonly"_hash:
			ws.m_Styles |= ES_READONLY;
			break;
		case L"center"_hash:
			ws.m_Styles |= ES_CENTER;
			break;
		case L"right"_hash:
			ws.m_Styles |= ES_RIGHT;
			break;
		case L"autohs"_hash:
			ws.m_Styles |= ES_AUTOHSCROLL;
			break;
		case L"autovs"_hash:
			ws.m_Styles |= ES_AUTOVSCROLL;
			break;
		case L"vsbar"_hash:
			ws.m_Styles |= WS_VSCROLL;
			break;
		case L"hsbar"_hash:
			ws.m_Styles |= WS_HSCROLL;
			break;
		case L"disablescroll"_hash:
			ws.m_Styles |= ES_DISABLENOSCROLL;
			break;
		case L"showlinenumbers"_hash:
			m_bShowLineNumbers = true;
			break;
		default:
			break;
		}
	}
	return ws;
}

/*!
* \brief $xdid Parsing Function
*
* \param input [NAME] [ID] [PROP] (OPTIONS)
* \param szReturnValue mIRC Data Container
*
* \return > void
*/
void DcxRichEdit::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	const auto numtok = input.numtok();

	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP] [N]
	case L"text"_hash:
	{
		// determine the line number
		int line = 0;

		if (numtok > 3)
			line = input.getnexttok().to_int() - 1;		// tok 4

		if ((line < 0) || (line >= Edit_GetLineCount(m_Hwnd)))
			throw Dcx::dcxException("Invalid line number.");

		// get index of first character in line
		const auto offset = SendMessage(m_Hwnd, EM_LINEINDEX, gsl::narrow_cast<WPARAM>(line), 0);
		// get length of the line we want to copy
		const auto len = SendMessage(m_Hwnd, EM_LINELENGTH, gsl::narrow_cast<WPARAM>(offset), 0) + 1;
		// create and fill the buffer
		auto p = std::make_unique<TCHAR[]>(len);
		*(reinterpret_cast<LPWORD>(p.get())) = gsl::narrow_cast<WORD>(len);
		SendMessage(m_Hwnd, EM_GETLINE, gsl::narrow_cast<WPARAM>(line), reinterpret_cast<LPARAM>(p.get()));

		// terminate the string at the right position
		gsl::at(p, len - 1) = TEXT('\0');

		// copy to result
		szReturnValue = p.get();
	}
	break;
	// [NAME] [ID] [PROP]
	case L"num"_hash:
	{
		if (this->isStyle(WindowStyle::ES_MultiLine))
			_ts_snprintf(szReturnValue, TEXT("%d"), gsl::narrow_cast<int>(SendMessage(m_Hwnd, EM_GETLINECOUNT, 0U, 0L)));
		else {
			// single line control so always 1 line.
			szReturnValue = TEXT('1');
		}
	}
	break;
	// [NAME] [ID] [PROP]
	case L"caretpos"_hash:
	{
		// caret startsel position
		const DWORD dwAbsoluteStartSelPos = GetCaretPos();

		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			// current line
			const auto iLinePos = SendMessage(m_Hwnd, EM_LINEFROMCHAR, gsl::narrow_cast<WPARAM>(-1), 0) + 1;
			// line offset
			const auto CharPos = (dwAbsoluteStartSelPos - gsl::narrow_cast<int>(SendMessage(m_Hwnd, EM_LINEINDEX, gsl::narrow_cast<WPARAM>(-1), 0)));

			_ts_snprintf(szReturnValue, TEXT("%d %u %u"), iLinePos, CharPos, dwAbsoluteStartSelPos);
		}
		else {
			// return selstart
			_ts_snprintf(szReturnValue, TEXT("1 %u %u"), dwAbsoluteStartSelPos, dwAbsoluteStartSelPos);
		}
	}
	break;
	// [NAME] [ID] [PROP]
	case L"selstart"_hash:
	{
		CHARRANGE c{};

		SendMessage(m_Hwnd, EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&c));
		_ts_snprintf(szReturnValue, TEXT("%d"), c.cpMin);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"selend"_hash:
	{
		CHARRANGE c{};

		SendMessage(m_Hwnd, EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&c));
		_ts_snprintf(szReturnValue, TEXT("%d"), c.cpMax);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"sel"_hash:
	{
		CHARRANGE c{};

		SendMessage(m_Hwnd, EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&c));
		_ts_snprintf(szReturnValue, TEXT("%d %d"), c.cpMin, c.cpMax);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"seltext"_hash:
	{
		CHARRANGE c{};

		SendMessage(m_Hwnd, EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&c));
		auto buffer = std::make_unique<TCHAR[]>(c.cpMax - c.cpMin);

		SendMessage(m_Hwnd, EM_GETSELTEXT, 0, reinterpret_cast<LPARAM>(buffer.get()));
		szReturnValue = buffer.get();
	}
	break;
	// [NAME] [ID] [PROP] [utf8|...]
	case L"rtftext"_hash:
	{
		UINT iFlags = SF_RTF;
		constexpr auto UTF8Flags = ((CP_UTF8 << 16) | SF_USECODEPAGE | SF_RTF);

		const auto tsFlags(input.getnexttok());

		stringstream_type rtf;

		EDITSTREAM es{};
		es.dwCookie = reinterpret_cast<DWORD_PTR>(&rtf);
		es.pfnCallback = &StreamOutToVarCallback;

		if (tsFlags == TEXT("utf8"))
			iFlags = gsl::narrow_cast<UINT>(UTF8Flags);

		SendMessage(m_Hwnd, EM_STREAMOUT, gsl::narrow_cast<UINT>(iFlags), reinterpret_cast<LPARAM>(&es));

		const TString tsOut(rtf.str().c_str());	// handles any char convertions needed.
		szReturnValue = tsOut.to_chr();

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
	break;
	case L"linenumbers"_hash:
	{
		szReturnValue = dcx_truefalse(m_bShowLineNumbers);
	}
	break;
	case L"guttercolors"_hash:
	case L"guttercolours"_hash:
	{
		_ts_snprintf(szReturnValue, TEXT("%u %u %u %u %u"), m_clrGutter_selbkg, m_clrGutter_bkg, m_clrGutter_seltxt, m_clrGutter_txt, m_clrGutter_border);
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
}

bool DcxRichEdit::SaveRichTextToFile(HWND hWnd, const TString& tsFilename) noexcept
{
	const auto hFile = CreateFile(tsFilename.to_chr(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	Auto(CloseHandle(hFile));

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	//EDITSTREAM es{};
	//es.dwCookie = reinterpret_cast<DWORD>(hFile);
	//es.pfnCallback = StreamOutToFileCallback;

	EDITSTREAM es{ reinterpret_cast<DWORD>(hFile), 0U, StreamOutToFileCallback };

	SendMessage(hWnd, EM_STREAMOUT, SF_RTF, reinterpret_cast<LPARAM>(&es));
	return (es.dwError == 0);
}

bool DcxRichEdit::LoadRichTextFromFile(HWND hWnd, const TString& tsFilename) noexcept
{
	const auto hFile = CreateFile(tsFilename.to_chr(), GENERIC_READ, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	Auto(CloseHandle(hFile));

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	//EDITSTREAM es{};
	//es.dwCookie = reinterpret_cast<DWORD>(hFile);
	//es.pfnCallback = StreamInFromFileCallback;

	EDITSTREAM es{ reinterpret_cast<DWORD>(hFile), 0U,StreamInFromFileCallback };

	SendMessage(hWnd, EM_STREAMIN, SF_RTF, reinterpret_cast<LPARAM>(&es));
	return (es.dwError == 0);
}

DWORD CALLBACK DcxRichEdit::StreamOutToVarCallback(DWORD_PTR dwCookie, const LPBYTE pbBuff, const LONG cb, LONG* pcb)
{
	const auto rtf = reinterpret_cast<stringstream_type*>(dwCookie);

	rtf->write(reinterpret_cast<stringstream_type::char_type*>(pbBuff), cb);

	*pcb = cb;
	return 0;

	//TString *rtf = reinterpret_cast<TString *>(dwCookie);
	//
	//rtf->append((char *)pbBuff, cb);
	//
	//*pcb = cb;
	//return 0;
}

DWORD CALLBACK DcxRichEdit::StreamOutToFileCallback(DWORD_PTR dwCookie, const LPBYTE pbBuff, const LONG cb, LONG* pcb) noexcept
{
	const auto hFile = reinterpret_cast<HANDLE>(dwCookie);
	DWORD NumberOfBytesWritten{};

	if (!WriteFile(hFile, pbBuff, cb, &NumberOfBytesWritten, nullptr))
	{
		//handle errors
		return 1;
		// or perhaps return GetLastError();
	}
	*pcb = NumberOfBytesWritten;
	return 0;
}

DWORD CALLBACK DcxRichEdit::StreamInFromFileCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, const LONG cb, LONG* pcb) noexcept
{
	const auto hFile = reinterpret_cast<HANDLE>(dwCookie);
	DWORD NumberOfBytesRead = 0;

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
GSL_SUPPRESS(bounds.3)
void DcxRichEdit::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
	{
		this->m_tsText.clear();	// = TEXT("");
		this->clearContents();
	}

	// xdid -a [NAME] [ID] [SWITCH] [TEXT]
	if (flags[TEXT('a')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto pos = this->GetCaretPos();

		//this->m_tsText += input.gettok(4, -1);
		//this->parseContents(TRUE);

		const auto tmp(input.getlasttoks());	// tok 4, -1

		this->m_tsText += tmp;
		this->parseStringContents(tmp, TRUE);
		//this->parseContents(TRUE);

		this->setCaretPos(pos);
	}
	// xdid -c [NAME] [ID] [SWITCH]
	else if (flags[TEXT('c')])
	{
		if (numtok < 3)
			throw DcxExceptions::dcxInvalidArguments();

		CopyToClipboard(m_Hwnd, this->m_tsText);
	}

	//// xdid -C [NAME] [ID] [SWITCH] [POS]
	//else if (flags[TEXT('C')])
	//{
	//	if (numtok < 4)
	//		throw DcxExceptions::dcxInvalidArguments();
	//
	//	auto pos = input.getnexttok().to_<long long>();
	//	if (pos < 0)
	//	{
	//		const auto oldPos = this->GetCaretPos();
	//		pos += oldPos;
	//		if (pos < 0)
	//			pos = 0;
	//	}
	//	this->setCaretPos(gsl::narrow_cast<DWORD>(pos));
	//}

	// xdid -d [NAME] [ID] [SWITCH] [N,N2,N3-N4...]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto pos = this->GetCaretPos();

		//if (this->isStyle(WindowStyle::ES_MultiLine))
		//{
		//	const auto nLine = input.getnexttok().to_<UINT>();	// tok 4
		//	this->m_tsText.deltok(nLine, TEXT("\r\n"));
		//}

		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			auto tsLines(input.getnexttok());

			// reverse numeric sort line numbers
			tsLines.sorttok(TEXT("nr"), TSCOMMA);

			const auto itEnd = tsLines.end();
			for (auto itStart = tsLines.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
			{
				//const TString tsLineRange(*itStart);
				//UINT nStartLine{}, nEndLine{};
				//if (tsLineRange.numtok(TEXT('-')) == 2)
				//{
				//	nStartLine = tsLineRange.getfirsttok(1, TEXT('-')).to_<UINT>();
				//	nEndLine = tsLineRange.getnexttok(TEXT('-')).to_<UINT>();
				//}
				//else {
				//	nStartLine = nEndLine = tsLineRange.to_<UINT>();
				//}
				//// delete lines from the back of the text so it doesnt change the position of other lines.
				//for (auto nLine = nEndLine; nLine >= nStartLine; --nLine)
				//	this->m_tsText.deltok(nLine, TEXT("\r\n"));

				const TString tsLineRange(*itStart);
				const auto r = Dcx::make_range(tsLineRange, this->m_tsText.numtok(TEXT("\r\n")));
				// delete lines from the back of the text so it doesnt change the position of other lines.
				for (auto nLine = r.e; nLine >= r.b; --nLine)
					this->m_tsText.deltok(nLine, TEXT("\r\n"));
			}
		}

		this->parseContents(TRUE);

		this->setCaretPos(pos);
	}
	// special richedit interception for font change
	// xdid -f [NAME] [ID] [SWITCH] [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
	else if (flags[TEXT('f')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		this->m_bIgnoreInput = true;
		{
			this->setRedraw(FALSE);
			Auto(this->setRedraw(TRUE));

			this->parseGlobalCommandRequest(input, flags);

			const auto iFontFlags = parseFontFlags(input.getnexttok());	// tok 4

			if (dcx_testflag(iFontFlags, dcxFontFlags::DCF_DEFAULT))
			{
				this->m_clrBackText = GetSysColor(COLOR_WINDOW);
				this->m_clrText = GetSysColor(COLOR_WINDOWTEXT);
				this->m_iFontSize = 10 * 20;
				this->m_bFontBold = false;
				this->m_bFontItalic = false;
				this->m_bFontUnderline = false;
				this->m_bFontStrikeout = false;
				this->m_byteCharset = DEFAULT_CHARSET;
				this->setContentsFont();
				this->parseContents(TRUE);
			}
			else if (numtok > 5)
			{
				this->m_byteCharset = gsl::narrow_cast<BYTE>(parseFontCharSet(input.getnexttok()));	// tok 5
				this->m_iFontSize = 20U * input.getnexttok().to_<UINT>();			// tok 6
				this->m_tsFontFaceName = input.getlasttoks().trim();				// tok 7, -1

				m_bFontBold = dcx_testflag(iFontFlags, dcxFontFlags::DCF_BOLD);

				m_bFontItalic = dcx_testflag(iFontFlags, dcxFontFlags::DCF_ITALIC);

				m_bFontStrikeout = dcx_testflag(iFontFlags, dcxFontFlags::DCF_STRIKEOUT);

				m_bFontUnderline = dcx_testflag(iFontFlags, dcxFontFlags::DCF_UNDERLINE);

				this->setContentsFont();
				this->parseContents(TRUE);
			}
		}
		this->m_bIgnoreInput = false;
		this->redrawWindow();
	}
	// xdid -g [NAME] [ID] [SWITCH] [Selected line Background Colour|-] (Background Colour|-) (Selected Line Text Colour|-) (Text Colour|-) (Border Colour|-)
	else if (flags[TEXT('g')])
	{
		static_assert(CheckFreeCommand(TEXT('g')), "Command in use!");

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		auto tsClr(input.getnexttok());
		if (tsClr != TEXT('-'))
			this->m_clrGutter_selbkg = tsClr.to_<COLORREF>();

		if (numtok > 4)
		{
			tsClr = input.getnexttok();
			if (tsClr != TEXT('-'))
				this->m_clrGutter_bkg = tsClr.to_<COLORREF>();

			if (numtok > 5)
			{
				tsClr = input.getnexttok();
				if (tsClr != TEXT('-'))
					this->m_clrGutter_seltxt = tsClr.to_<COLORREF>();

				if (numtok > 6)
				{
					tsClr = input.getnexttok();
					if (tsClr != TEXT('-'))
						this->m_clrGutter_txt = tsClr.to_<COLORREF>();

					if (numtok > 7)
					{
						tsClr = input.getnexttok();
						if (tsClr != TEXT('-'))
							this->m_clrGutter_border = tsClr.to_<COLORREF>();
					}
				}
			}
		}
	}
	// xdid -i [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('i')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto pos = this->GetCaretPos();

		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			const auto nLine = input.getnexttok().to_<UINT>();								// tok 4
			this->m_tsText.instok(input.getlasttoks(), nLine, TEXT("\r\n"));	// tok 5, -1
		}
		else
			this->m_tsText = input.getlasttoks();										// tok 4, -1

		this->parseContents(TRUE);

		this->setCaretPos(pos);
	}
	// xdid -I [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('I')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nChar = input.getnexttok().to_<UINT>();	// tok 4
		TString tsInsert(input.getlasttoks());
		TString tsLeft(this->m_tsText.sub(0, nChar));
		const TString tsRight(this->m_tsText.sub(nChar, this->m_tsText.len()));

		const auto pos = this->GetCaretPos();

		this->m_tsText = tsLeft + tsInsert + tsRight;

		this->parseContents(TRUE);

		this->setCaretPos(pos);
	}
	// xdid -k [NAME] [ID] [SWITCH] [COLOR]
	else if (flags[TEXT('k')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto clrColor = input.getnexttok().to_<COLORREF>();	// tok 4

		if (clrColor == CLR_INVALID)
		{
			SendMessage(m_Hwnd, EM_SETBKGNDCOLOR, 1, 0);
			this->setBackColor(GetSysColor(COLOR_WINDOWTEXT));
		}
		else
		{
			SendMessage(m_Hwnd, EM_SETBKGNDCOLOR, 0, gsl::narrow_cast<LPARAM>(clrColor));
			this->setBackColor(clrColor);
		}
		this->redrawWindow();
	}
	// xdid -l [NAME] [ID] [SWITCH] [N] [COLOR]
	else if (flags[TEXT('l')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nColor = input.getnexttok().to_int();	// tok 4

		if (nColor < 0 || nColor >= gsl::narrow_cast<int>(std::size(m_aColorPalette)))
			throw Dcx::dcxException("Invalid Colour");

		gsl::at(this->m_aColorPalette, nColor) = input.getnexttok().to_<COLORREF>();	// tok 5
		this->parseContents(TRUE);
	}
	// xdid -m [NAME] [ID] [SWITCH]
	else if (flags[TEXT('m')])
	{
		getmIRCPalette(gsl::span<COLORREF>(m_aColorPalette), true);

		this->parseContents(TRUE);
	}
	// xdid -n [NAME] [ID] [SWITCH] [BOOL]
	else if (flags[TEXT('n')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto b = (input.getnexttok().to_int() > 0);	// tok 4

		this->setAutoUrlDetect(b ? TRUE : FALSE);
	}
	// xdid -o [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('o')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nLine = input.getnexttok().to_<UINT>();	// tok 4

		if (this->isStyle(WindowStyle::ES_MultiLine))
			this->m_tsText.puttok(input.getlasttoks(), nLine, TEXT("\r\n"));	// tok 5, -1
		else
			this->m_tsText = input.getlasttoks();	// tok 5, -1

		this->parseContents(TRUE);
	}
	// xdid -P [NAME] [ID]
	else if (flags[TEXT('P')])
	{
		SendMessage(this->getHwnd(), WM_PASTE, 0, 0);
	}
	// xdid -q -> [NAME] [ID] -q [COLOR1] ... [COLOR16]
	else if (flags[TEXT('q')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nColor = numtok - 3;

		if (nColor >= std::min(std::size(m_aColorPalette), 16U))
			throw Dcx::dcxException("Invalid Colour Count");

		for (auto i = decltype(nColor){0}; i < nColor; ++i)
			gsl::at(m_aColorPalette, i) = input.getnexttok().to_<COLORREF>();	// tok 4 + i

		this->parseContents(TRUE);
	}
	// This is to avoid an invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
	{
	}
	// xdid -t [NAME] [ID] [SWITCH] [FILENAME]
	// xdid -t [NAME] [ID] [SWITCH] [+FLAGS] [FILENAME]
	else if (flags[TEXT('t')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xflags(input.getnexttok().trim());	// tok 4
		bool bOldMethod = false;
		TString tsArgs;

		if (!xflags[TEXT('+')])
		{
			bOldMethod = true;
			tsArgs = input.gettok(4, -1).trim();	// 4, -1
		}
		else
			tsArgs = input.getlasttoks().trim();	// 5, -1

		if (xflags[TEXT('o')])
			bOldMethod = true;

		if (bOldMethod)
		{
			// xdid -t [NAME] [ID] [FILENAME]
			// xdid -t [NAME] [ID] +o [FILENAME]

			if (!IsFile(tsArgs))
				throw Dcx::dcxException(TEXT("Unable to open: %"), tsArgs);

			this->m_tsText = readTextFile(tsArgs);

			const auto mask = this->m_dEventMask;
			this->m_dEventMask = 0;
			this->parseContents(TRUE);
			this->m_dEventMask = mask;
		}
		else {
			// new methods... load rtf...
			if (xflags[TEXT('x')])
			{
				// load from xml
				// xdid -t [NAME] [ID] +x [FILENAME] [data_set]
				if (tsArgs.numtok() < 2)
					throw Dcx::dcxException(TEXT("No dataset specified"));

				TString tsFile(tsArgs.gettok(1, gsl::narrow_cast<int>(tsArgs.numtok()) - 1));
				const TString tsDataSet(tsArgs.gettok(tsArgs.numtok()));

				if (!LoadRichTextFromXml(m_Hwnd, tsFile, tsDataSet))	// default load rtf text
					throw Dcx::dcxException(TEXT("Unable to load: % Dataset: %"), tsFile, tsDataSet);
			}
			else {
				if (!IsFile(tsArgs))
					throw Dcx::dcxException(TEXT("Unable to open: %"), tsArgs);

				if (!LoadRichTextFromFile(m_Hwnd, tsArgs))	// default load rtf text
					throw Dcx::dcxException(TEXT("Unable to load: %"), tsArgs);
			}
		}
	}
	// xdid -u [NAME] [ID] [SWITCH] [FILENAME]
	// xdid -u [NAME] [ID] [SWITCH] [+FLAGS] [FILENAME]
	else if (flags[TEXT('u')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

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
			// xdid -u [NAME] [ID] [FILENAME]
			// xdid -u [NAME] [ID] +o [FILENAME]
			// old way if no flags provided or old method selected.
			bRes = SaveDataToFile(tsFile, this->m_tsText);
		}
		else {
			// new method's...
			if (xflags[TEXT('x')])
			{
				// save to xml
				// xdid -u [NAME] [ID] +x [FILENAME] [data_set]
			}
			else
			{
				//	default, save as rtf text
				// xdid -u [NAME] [ID] + [FILENAME]
				bRes = SaveRichTextToFile(m_Hwnd, tsFile);
			}
		}
		if (!bRes)
			throw Dcx::dcxException(TEXT("Unable to save: %"), tsFile);
	}
	// xdid -S [NAME] [ID] [SWITCH] [START] (END)
	else if (flags[TEXT('S')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		CHARRANGE c{};
		c.cpMin = input.getnexttok().to_int();	// tok 4
		c.cpMax = (numtok > 4) ? input.getnexttok().to_int() : c.cpMin;	// tok 5

		SendMessage(m_Hwnd, EM_EXSETSEL, 0, reinterpret_cast<LPARAM>(&c));
		SendMessage(m_Hwnd, EM_SCROLLCARET, 0, 0);
	}
	// xdid -V [NAME] [ID]
	else if (flags[TEXT('V')])
	{
		SendMessage(m_Hwnd, EM_SCROLLCARET, 0, 0);
	}
	// xdid -y [NAME] [ID] [SWITCH] [0|1|-] [0|1]
	else if (flags[TEXT('y')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();
		auto tsRep(input.getnexttok());
		if (tsRep != TEXT('-'))
			this->m_bIgnoreRepeat = (tsRep.to_int() > 0);	// tok 4
		if (numtok > 4)
		{
			if (this->isStyle(WindowStyle::ES_MultiLine))
			{
				this->m_bShowLineNumbers = (input.getnexttok().to_int() > 0);	// tok 5
				setFmtRect(!m_bShowLineNumbers);
				InvalidateRect(m_Hwnd, nullptr, TRUE);
			}
		}
	}
	// xdid -z [NAME] [ID] [SWITCH] [NUMERATOR] [DENOMINATOR]
	else if (flags[TEXT('z')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto num = input.getnexttok().to_int();	// tok 4
		const auto den = input.getnexttok().to_int();	// tok 5

		if (!SendMessage(m_Hwnd, EM_SETZOOM, gsl::narrow_cast<WPARAM>(num), gsl::narrow_cast<LPARAM>(den)))
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
GSL_SUPPRESS(bounds.3)
void DcxRichEdit::setContentsFont() noexcept
{
	CHARFORMAT2 chrf{};

	chrf.cbSize = sizeof(CHARFORMAT2);
	chrf.dwMask = CFM_BACKCOLOR | CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_ITALIC | CFM_UNDERLINE | CFM_STRIKEOUT | CFM_CHARSET;
	chrf.yHeight = gsl::narrow_cast<LONG>(this->m_iFontSize);
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
	{
		dcx_strcpyn(&chrf.szFaceName[0], this->m_tsFontFaceName.to_chr(), std::size(chrf.szFaceName));
		//chrf.szFaceName[std::size(chrf.szFaceName) -1] = 0;
		gsl::at(chrf.szFaceName, std::size(chrf.szFaceName) - 1) = 0;
	}

	//this->hideSelection(TRUE);
	//this->setSel(0, -1);
	//this->setCharFormat(SCF_SELECTION, &chrf);
	//this->setSel(0, 0);
	//this->hideSelection(FALSE);

	this->setCharFormat(SCF_ALL, &chrf);
}

/*!
* \brief blah
*
* blah
*/
void DcxRichEdit::clearContents() noexcept
{
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
void DcxRichEdit::parseContents(const BOOL fNewLine) noexcept
{ // old function
	this->m_bIgnoreInput = true;
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

GSL_SUPPRESS(bounds.3)
void DcxRichEdit::parseStringContents(const TString& tsStr, const BOOL fNewLine) noexcept
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

	for (auto i = 0U; text[i] != TEXT('\0'); ++i)
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
			//TCHAR colbuf[3] = { 0 };
			//if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9'))
			//{
			//	colbuf[0] = text[i + 1];
			//	colbuf[1] = TEXT('\0');
			//	++i;
			//
			//	if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9'))
			//	{
			//		colbuf[1] = text[i + 1];
			//		++i;
			//	}
			//
			//	// color code number
			//	bmcolor = true;
			//	mcolor = this->m_aColorPalette[unfoldColor(&colbuf[0])];
			//
			//	// maybe a background color
			//	if (text[i + 1] == TEXT(','))
			//	{
			//		++i;
			//
			//		if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9'))
			//		{
			//			colbuf[0] = text[i + 1];
			//			colbuf[1] = TEXT('\0');
			//			++i;
			//
			//			if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9'))
			//			{
			//				colbuf[1] = text[i + 1];
			//				++i;
			//			}
			//
			//			// color code number
			//			bbkgcolor = true;
			//			bkgcolor = this->m_aColorPalette[unfoldColor(&colbuf[0])];
			//		}
			//	}
			//}
			//else {
			//	bmcolor = false;
			//	bbkgcolor = false;
			//}

			if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9'))
			{
				{
					UINT umIRCColour = (text[i + 1] - L'0'); // must be => 0 <= 9
					++i;

					if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9'))
					{
						umIRCColour *= 10;	// becomes a max of 90
						umIRCColour += (text[i + 1] - L'0');	// becomes a max of 99

						++i;
					}

					// color code number
					bmcolor = true;
					mcolor = gsl::at(m_aColorPalette, umIRCColour);
				}

				// maybe a background color
				if (text[i + 1] == TEXT(','))
				{
					++i;

					if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9'))
					{
						UINT umIRCColour = (text[i + 1] - L'0'); // must be => 0 <= 9
						++i;

						if (text[i + 1] >= TEXT('0') && text[i + 1] <= TEXT('9'))
						{
							umIRCColour *= 10;	// becomes a max of 90
							umIRCColour += (text[i + 1] - L'0');	// becomes a max of 99
							++i;
						}

						// color code number
						bbkgcolor = true;
						bkgcolor = gsl::at(m_aColorPalette, umIRCColour);
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

			this->insertText(&cbuf[0], bline, uline, iline, bmcolor, mcolor, bbkgcolor, bkgcolor, 0);
		}
		}
	}
	this->setSel(0, 0);
	this->setRedraw(TRUE);
	this->redrawWindow();

	this->m_bIgnoreInput = false;

	// add all data to a buffer, & add buffer in a oner?
	//SETTEXTEX TextInfo = { 0 };

	//TextInfo.flags = ST_SELECTION;
	//TextInfo.codepage = CP_UTF8;

	//SendMessage(hRichText, EM_SETTEXTEX, (WPARAM)&TextInfo, (LPARAM)Utf8); 
}

void DcxRichEdit::DrawGutter()
{
	if (!m_Hwnd)
		return;

	if (HDC hdc = GetDC(m_Hwnd); hdc)
	{
		Auto(ReleaseDC(m_Hwnd, hdc));
		DrawGutter(hdc);
	}
}

void DcxRichEdit::DrawGutter(HDC hdc)
{
	if (!hdc || !m_Hwnd)
		return;

	RECT rcClient{};
	const RECT rcFmt = getFmtRect();

	GetClientRect(m_Hwnd, &rcClient);

	RECT m_FRGutter = rcClient;
	m_FRGutter.right = std::max(rcFmt.left - 3, 0L);

	// gutter doesnt exist...
	if (m_FRGutter.right == 0)
		return;

	const auto hFont = (m_hFont != nullptr) ? m_hFont : this->getFont();
	const auto oldFont = Dcx::dcxSelectObject(hdc, hFont);
	Auto(Dcx::dcxSelectObject(hdc, oldFont));

	TEXTMETRICW lptm{};
	GetTextMetrics(hdc, &lptm);
	const auto letter_height = lptm.tmHeight;

	if (auto hdcbuf = CreateHDCBuffer(hdc, &m_FRGutter); hdcbuf)
	{
		Auto(DeleteHDCBuffer(hdcbuf));

		Dcx::FillRectColour(*hdcbuf, &m_FRGutter, m_clrGutter_bkg);

		dcxDrawEdge(*hdcbuf, &m_FRGutter, m_clrGutter_border);

		RECT RNumber = m_FRGutter;
		InflateRect(&RNumber, -5, 0);

		// get visible lines
		const auto rng = GetVisibleRange();
		// get current caret pos
		const auto pos = GetCaretLine();

		TCHAR buf[49]{};

		{
			// top line, could be a partial line
			const auto iLineChar = SNDMSG(m_Hwnd, EM_LINEINDEX, rng.b, 0);
			POINTL pl{};
			SNDMSG(m_Hwnd, EM_POSFROMCHAR, reinterpret_cast<WPARAM>(&pl), gsl::narrow_cast<LPARAM>(iLineChar));
			// NB: if a partial line pl.y will be a negative (ie off screen)
			RNumber.top = pl.y;
			RNumber.bottom = RNumber.top + letter_height;
		}

		//SetGraphicsMode(hdc, GM_ADVANCED);
		const auto oldMode = SetBkMode(*hdcbuf, TRANSPARENT);
		Auto(SetBkMode(*hdcbuf, oldMode));
		const auto oldTextColor = GetTextColor(*hdcbuf);
		Auto(SetTextColor(*hdcbuf, oldTextColor));

		// render the line numbers
		for (const auto& index : rng)
		{
			if (index == pos)
			{
				if (m_clrGutter_selbkg != m_clrGutter_bkg)
					Dcx::FillRectColour(*hdcbuf, &RNumber, m_clrGutter_selbkg);
				SetTextColor(*hdcbuf, m_clrGutter_seltxt);
			}
			else
				SetTextColor(*hdcbuf, m_clrGutter_txt);

			if (const auto l = _ts_snprintf(&buf[0], std::size(buf), _T("%u"), index + 1); l != -1)
				DrawText(*hdcbuf, &buf[0], l, &RNumber, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

			RNumber.top += letter_height;
			RNumber.bottom = RNumber.top + letter_height;
		}
		BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, *hdcbuf, 0, 0, SRCCOPY);
	}
}

Dcx::range_t<DWORD> DcxRichEdit::GetVisibleRange() const noexcept
{
	// find the index of the top visible line

	const auto start_line = gsl::narrow_cast<DWORD>(SNDMSG(m_Hwnd, EM_GETFIRSTVISIBLELINE, 0, 0));

	const RECT rc = getFmtRect();

	// find the index of the last visible line
	POINTL PEnd{ 0,  rc.bottom };
	const auto char_index = SNDMSG(m_Hwnd, EM_CHARFROMPOS, 0, reinterpret_cast<LPARAM>(&PEnd));
	const auto stop_line = gsl::narrow_cast<DWORD>(SNDMSG(m_Hwnd, EM_EXLINEFROMCHAR, 0, gsl::narrow_cast<LPARAM>(char_index)));

	// +1 to make range inclusive
	return { start_line, stop_line + 1 };
}

GSL_SUPPRESS(con.4)
DWORD DcxRichEdit::GetCaretPos() const noexcept
{
	DWORD hiPos{}, loPos{};
	SNDMSG(m_Hwnd, EM_GETSEL, reinterpret_cast<LPARAM>(&loPos), reinterpret_cast<LPARAM>(&hiPos));
	if (loPos != hiPos)
		--hiPos;
	return hiPos;

	//// windows 10 only :/
	//return Edit_GetCaretIndex(m_Hwnd);
}

DWORD DcxRichEdit::GetCaretLine() const noexcept
{
	const auto pos = GetCaretPos();
	return gsl::narrow_cast<DWORD>(SNDMSG(m_Hwnd, EM_EXLINEFROMCHAR, 0, gsl::narrow_cast<LPARAM>(pos)));
}

void DcxRichEdit::setCaretPos(DWORD pos) noexcept
{
	//SendMessage(m_Hwnd, EM_SETSEL, pos, pos);

	CHARRANGE c{ gsl::narrow_cast<long>(pos), gsl::narrow_cast<long>(pos) };

	SendMessage(m_Hwnd, EM_EXSETSEL, 0, reinterpret_cast<LPARAM>(&c));
}

/*!
* \brief blah
*
* blah
*/
void DcxRichEdit::insertText(const TCHAR* const text, bool bline, bool uline, bool iline, bool bcolor, COLORREF color, bool bbkgcolor, COLORREF bkgcolor, int reverse) noexcept
{
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

	CHARFORMAT2 chrf{};
	chrf.cbSize = sizeof(CHARFORMAT2);
	chrf.dwMask = CFM_BACKCOLOR | CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_ITALIC | CFM_UNDERLINE | CFM_STRIKEOUT | CFM_CHARSET;
	chrf.yHeight = gsl::narrow_cast<LONG>(this->m_iFontSize);
	chrf.crTextColor = this->getTextColor();
	chrf.crBackColor = this->getBackTextColor();
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
		dcx_strcpyn(&chrf.szFaceName[0], this->m_tsFontFaceName.to_chr(), std::size(chrf.szFaceName));

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
LRESULT DcxRichEdit::setAutoUrlDetect(const BOOL iEnable) noexcept
{
	return SendMessage(m_Hwnd, EM_AUTOURLDETECT, gsl::narrow_cast<WPARAM>(iEnable), gsl::narrow_cast<LPARAM>(0));
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxRichEdit::hideSelection(const BOOL iHide) noexcept
{
	return SendMessage(m_Hwnd, EM_HIDESELECTION, gsl::narrow_cast<WPARAM>(iHide), gsl::narrow_cast<LPARAM>(0));
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxRichEdit::setSel(const int iStart, const int iEnd) noexcept
{
	return SendMessage(m_Hwnd, EM_SETSEL, gsl::narrow_cast<WPARAM>(iStart), gsl::narrow_cast<LPARAM>(iEnd));
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxRichEdit::replaceSel(const BOOL bUndo, LPCTSTR lpstr) noexcept
{
	return SendMessage(m_Hwnd, EM_REPLACESEL, gsl::narrow_cast<WPARAM>(bUndo), reinterpret_cast<LPARAM>(lpstr));
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxRichEdit::getCharFormat(const UINT iType, const CHARFORMAT2* cfm) const noexcept
{
	return SendMessage(m_Hwnd, EM_GETCHARFORMAT, gsl::narrow_cast<WPARAM>(iType), reinterpret_cast<LPARAM>(cfm));
}

/*!
* \brief blah
*
* blah
*/

LRESULT DcxRichEdit::setCharFormat(const UINT iType, const CHARFORMAT2* cfm) noexcept
{
	return SendMessage(m_Hwnd, EM_SETCHARFORMAT, gsl::narrow_cast<WPARAM>(iType), reinterpret_cast<LPARAM>(cfm));
}

void DcxRichEdit::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());

	xml->LinkEndChild(new TiXmlText(this->m_tsText.c_str()));
}

TiXmlElement* DcxRichEdit::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

const TString DcxRichEdit::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

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
	if (dcx_testflag(Styles, ES_AUTOVSCROLL))
		styles.addtok(TEXT("autovs"));
	if (dcx_testflag(Styles, WS_VSCROLL))
		styles.addtok(TEXT("vsbar"));
	if (dcx_testflag(Styles, WS_HSCROLL))
		styles.addtok(TEXT("hsbar"));
	if (dcx_testflag(Styles, ES_DISABLENOSCROLL))
		styles.addtok(TEXT("disablescroll"));
	if (m_bShowLineNumbers)
		styles.addtok(TEXT("shownumbers"));
	return styles;
}

/*!
* \brief blah
*
* blah
*/
LRESULT DcxRichEdit::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (!hdr)
			break;

		switch (hdr->code)
		{
		case EN_LINK:
		{
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			{
				if (dcxlParam(ENLINK*, enl); (enl) && ((enl->msg == WM_LBUTTONDOWN) || (enl->msg == WM_LBUTTONDBLCLK) || (enl->msg == WM_RBUTTONDOWN)))
				{
					TEXTRANGE tr{};

					// get information about link text
					tr.chrg = enl->chrg;
					const auto strlen = (enl->chrg.cpMax - enl->chrg.cpMin) + 1U;

					auto str = std::make_unique<TCHAR[]>(strlen);

					tr.lpstrText = str.get();
					SendMessage(m_Hwnd, EM_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));

					TString tsEvent;
					if (enl->msg == WM_LBUTTONDOWN)
						tsEvent = TEXT("sclick");
					else if (enl->msg == WM_LBUTTONDBLCLK)
						tsEvent = TEXT("dclick");
					else if (enl->msg == WM_RBUTTONDOWN)
						tsEvent = TEXT("rclick");

					this->execAliasEx(TEXT("link,%u,%s,%s"), getUserID(), tsEvent.to_chr(), tr.lpstrText);
				}
			}
		} // EN_LINK
		break;

		//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/richedit/richeditcontrols/richeditcontrolreference/richeditmessages/em_gettextrange.asp
		case EN_SELCHANGE:
		{
			if (this->m_bIgnoreInput)
				break;

			if (const auto pSelChange = reinterpret_cast<SELCHANGE*>(lParam); pSelChange && pSelChange->seltyp == SEL_EMPTY && m_bShowLineNumbers)
				PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);

			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
			{
				if (const auto* const sel = reinterpret_cast<SELCHANGE*>(lParam); (sel) && (sel->seltyp != SEL_EMPTY))
				{
					auto str = std::make_unique<TCHAR[]>((sel->chrg.cpMax - sel->chrg.cpMin) + 1U);

					TEXTRANGE tr{};

					// get information about selected text
					tr.chrg = sel->chrg;
					tr.lpstrText = str.get();
					SendMessage(m_Hwnd, EM_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));

					this->execAliasEx(TEXT("selchange,%u,%d,%d,%s"), getUserID(), sel->chrg.cpMin, sel->chrg.cpMax, tr.lpstrText);
				}
			}
		} // EN_SELCHANGE
		break;

		case EN_UPDATE:
		{
			if (m_bShowLineNumbers)
				PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
		}
		break;

		default:
			break;
		} // switch(hdr->code)
	} // WM_NOTIFY
	break;

	case WM_COMMAND:
	{
		if (!m_bShowLineNumbers)
			break;

		switch (Dcx::dcxHIWORD(wParam))
		{
		case EN_HSCROLL:
			[[fallthrough]];
		case EN_CHANGE:
		{
			PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
		}
		break;

		default:
			break;
		}
	} // WM_COMMAND
	break;

	default:
		break;
	}	// switch (uMsg)

	return 0L;
}

LRESULT DcxRichEdit::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
		// This code enables tabstop out of richedit controls if tabstop styles used. BUT you cant use tab keys in the richedit control when tabstop used.
	case WM_GETDLGCODE:
	{
		//if (wParam != VK_TAB)
		//{
		//	bParsed = TRUE;
		//	return DLGC_WANTALLKEYS;
		//}
		//else if (!this->isStyle(WindowStyle::TabStop))
		//{
		//	bParsed = TRUE;
		//	return DLGC_WANTALLKEYS;
		//}

		//if (wParam == VK_TAB)
		//{
		//	if (this->isStyle(WindowStyle::TabStop))
		//	{
		//		bParsed = TRUE;
		//		return DefWindowProc(m_Hwnd, uMsg, wParam, lParam);
		//	}
		//}

		if (wParam == VK_TAB)
		{
			bParsed = TRUE;
			if (this->isStyle(WindowStyle::TabStop))
				return DefWindowProc(m_Hwnd, uMsg, wParam, lParam);

			return DLGC_WANTALLKEYS;
		}
		else if (wParam != VK_RETURN)
		{
			bParsed = TRUE;
			return DLGC_WANTALLKEYS;
		}
	}
	break;

	case WM_KEYDOWN:
	{
		if (m_bShowLineNumbers)
			PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);

		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
		{
			if (wParam == VK_RETURN)
				this->execAliasEx(TEXT("return,%u"), getUserID());

			if ((this->m_bIgnoreRepeat) && (lParam & 0x40000000)) // ignore repeats
				break;

			this->execAliasEx(TEXT("keydown,%u,%d"), getUserID(), wParam);
		}
	}
	break;

	case WM_KEYUP:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
			this->execAliasEx(TEXT("keyup,%u,%d"), getUserID(), wParam);

		if (m_bShowLineNumbers)
			PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
	}
	break;

	case WM_PAINT:
	{
		if (this->IsAlphaBlend())
		{
			PAINTSTRUCT ps{};

			auto hdc = BeginPaint(m_Hwnd, &ps);
			Auto(EndPaint(m_Hwnd, &ps));

			bParsed = TRUE;

			// Setup alpha blend if any.
			const auto ai = this->SetupAlphaBlend(&hdc);
			Auto(this->FinishAlphaBlend(ai));

			const auto lRes = CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);

			if (m_bShowLineNumbers)
				//PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
				DrawGutter(hdc);

			return lRes;
		}
		else if (m_bShowLineNumbers)
		{
			bParsed = TRUE;
			const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);

			//PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
			if (wParam)
				DrawGutter(reinterpret_cast<HDC>(wParam));
			else
				DrawGutter();

			return lRes;
		}
	}
	break;

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

	case WM_SIZE:
	{
		bParsed = TRUE;
		const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);

		if (m_bShowLineNumbers)
			setFmtRect();

		return lRes;
	}
	break;

	case WM_DRAW_NUMBERS:
	{
		if (m_bShowLineNumbers)
		{
			bParsed = TRUE;

			setFmtRect();
			if (wParam)
				DrawGutter(reinterpret_cast<HDC>(wParam));
			else
				DrawGutter();
		}
		return 0;
	}
	break;

	case WM_DESTROY:
	{
		delete this;
		bParsed = TRUE;
	}
	break;

	default:
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}

// Incomplete
bool DcxRichEdit::LoadRichTextFromXml(HWND hWnd, TString& tsFilename, const TString& tsDataSet)
{
	if (!IsFile(tsFilename))
		throw Dcx::dcxException(TEXT("Unable To Access File: %"), tsFilename);

	TiXmlDocument doc(tsFilename.c_str());
	doc.SetCondenseWhiteSpace(false);
	TString tsBuf;

	if (!doc.LoadFile())
		throw Dcx::dcxException(TEXT("Not an XML File: %"), tsFilename);

	const auto* const xRoot = doc.FirstChildElement("dcxml");
	if (!xRoot)
		throw Dcx::dcxException("Unable Find 'dcxml' root");

	const auto* xElm = xRoot->FirstChildElement("richedit_data");
	if (!xElm)
		throw Dcx::dcxException("Unable To Find 'richedit_data' element");

	xElm = xElm->FirstChildElement(tsDataSet.c_str());
	if (!xElm)
		throw Dcx::dcxException(TEXT("Unable To Find Dataset: %"), tsDataSet);

	//xElm->Value();

	// incomplete....

	return false;
}

LRESULT DcxRichEdit::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
