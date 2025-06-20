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
	: DcxControl(ID, p_Dialog, DcxControlTypes::RICHEDIT)
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

	if (const auto dStyle = parseExEditStyles(styles); dStyle)
		Dcx::dcxEdit_SetExtendedStyle(m_Hwnd, dStyle, dStyle);

	if (ws.m_NoTheme)
		DcxUXModule::dcxSetWindowTheme(m_Hwnd, L" ", L" ");
	else
		if (!CRichEditThemed::Attach(m_Hwnd))
			throw DcxExceptions::dcxUnableToCreateWindow();

	setNoThemed(ws.m_NoTheme);

	this->setTextBackColor(GetSysColor(COLOR_WINDOW));
	this->setTextColor(GetSysColor(COLOR_WINDOWTEXT));

	getmIRCPalette(gsl::span<COLORREF>(m_aColorPalette), false);

	this->setContentsFont();

	Dcx::dcxRichEdit_SetEventMask(m_Hwnd, (ENM_SELCHANGE | ENM_CHANGE | ENM_LINK));

	//SendMessage(m_Hwnd, CCM_SETUNICODEFORMAT, TRUE, 0);

	if (styles.istok(TEXT("tooltips")))
	{
		if (!IsWindow(p_Dialog->getToolTipHWND()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		setToolTipHWND(p_Dialog->getToolTipHWND());
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
		case L"unlockgutter"_hash:
			m_bLockGutter = false;
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

		if ((line < 0) || (line >= Dcx::dcxEdit_GetLineCount(m_Hwnd)))
			throw Dcx::dcxException("Invalid line number.");

		// get index of first character in line
		const auto offset = Dcx::dcxEdit_GetLineIndex(m_Hwnd, line);
		// get length of the line we want to copy
		const auto len = Dcx::dcxEdit_LineLength(m_Hwnd, offset) + 1;
		// create and fill the buffer
		auto p = std::make_unique<TCHAR[]>(len);
		Dcx::dcxEdit_GetLine(m_Hwnd, line, p.get(), len);

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
			_ts_snprintf(szReturnValue, TEXT("%d"), Dcx::dcxEdit_GetLineCount(m_Hwnd));
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
			const auto iLinePos = Dcx::dcxEdit_LineFromChar(m_Hwnd, -1) + 1;
			// line offset
			const auto CharPos = (dwAbsoluteStartSelPos - Dcx::dcxEdit_GetLineIndex(m_Hwnd, -1));

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

		Dcx::dcxRichEdit_ExGetSel(m_Hwnd, &c);
		_ts_snprintf(szReturnValue, TEXT("%d"), c.cpMin);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"selend"_hash:
	{
		CHARRANGE c{};

		Dcx::dcxRichEdit_ExGetSel(m_Hwnd, &c);
		_ts_snprintf(szReturnValue, TEXT("%d"), c.cpMax);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"sel"_hash:
	{
		CHARRANGE c{};

		Dcx::dcxRichEdit_ExGetSel(m_Hwnd, &c);
		_ts_snprintf(szReturnValue, TEXT("%d %d"), c.cpMin, c.cpMax);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"seltext"_hash:
	{
		CHARRANGE c{};

		Dcx::dcxRichEdit_ExGetSel(m_Hwnd, &c);
		auto buffer = std::make_unique<TCHAR[]>(gsl::narrow_cast<size_t>(c.cpMax) - c.cpMin);

		Dcx::dcxRichEdit_GetSelText(m_Hwnd, buffer.get());
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

		Dcx::dcxRichEdit_StreamOut(m_Hwnd, iFlags, &es);

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
	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N] [LINE] [SUBCHAR]
	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N] [CHAR]
	case L"find"_hash:
	{
		if (input.numtok() < 7)
			throw DcxExceptions::dcxInvalidArguments();

		const auto matchtext(input.getfirsttok(2, TSTABCHAR).trim());
		if (matchtext.empty())
			throw Dcx::dcxException("No Match text supplied");

		const auto params(input.getnexttok(TSTABCHAR).trim());	// tok 3

		if ((params.numtok() < 3) || (params.numtok() > 4))
			throw DcxExceptions::dcxInvalidArguments();

		szReturnValue = findTextRange(m_tsText, matchtext, params).to_chr();
	}
	break;
	case L"zoom"_hash:
	{
		//Supported in Windows 10 1809 and later. The edit control needs to have the ES_EX_ZOOMABLE extended style set, for this message to have an effect
		//(the zoom ratio is always between 1/64 and 64) NOT inclusive, 1.0 = no zoom
		if (int nNumerator{}, nDenominator{}; Dcx::dcxEdit_GetZoom(m_Hwnd, &nNumerator, &nDenominator))
			_ts_snprintf(szReturnValue, TEXT("%d.%d"), nNumerator, nDenominator);
		else
			szReturnValue = TEXT("1.0");
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
	//es.dwCookie = reinterpret_cast<DWORD_PTR>(hFile);
	//es.pfnCallback = StreamOutToFileCallback;

	EDITSTREAM es{ reinterpret_cast<DWORD_PTR>(hFile), 0U, StreamOutToFileCallback };

	Dcx::dcxRichEdit_StreamOut(hWnd, SF_RTF, &es);
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

	EDITSTREAM es{ reinterpret_cast<DWORD_PTR>(hFile), 0U,StreamInFromFileCallback };

	Dcx::dcxRichEdit_StreamIn(hWnd, SF_RTF, &es);
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
		Dcx::dcxEdit_SetModify(m_Hwnd, FALSE);
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
		Dcx::dcxEdit_SetModify(m_Hwnd, FALSE);

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

		const TString tsFlags(input.getnexttok());				// tok 4
		const TString tsCharset(input.getnexttok());			// tok 5
		const UINT iSize = input.getnexttok().to_<UINT>();		// tok 6
		const TString tsFontname(input.getlasttoks().trim());	// tok 7, -1

		setRicheditFont(tsFlags, tsCharset, iSize, tsFontname);
	}
	// xdid -g [NAME] [ID] [SWITCH] [Selected line Background Colour|-] (Background Colour|-) (Selected Line Text Colour|-) (Text Colour|-) (Border Colour|-) (Unlock Gutter 0|1|-) (Gutter Size|-) (Gutter Border Size|-)
	else if (flags[TEXT('g')])
	{
		static_assert(CheckFreeCommand(TEXT('g')), "Command in use!");

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		int argcnt{ 4 };
		const TString tsArgs(input.getlasttoks());
		for (const auto& a : tsArgs)
		{
			if (a != TEXT('-'))
			{
				switch (argcnt)
				{
				case 4:
				{
					// set gutter selected background colour.
					this->m_clrGutter_selbkg = a.to_<COLORREF>();
				}
				break;
				case 5:
				{
					// set gutter background colour.
					this->m_clrGutter_bkg = a.to_<COLORREF>();
				}
				break;
				case 6:
				{
					// set gutter selected text colour.
					this->m_clrGutter_seltxt = a.to_<COLORREF>();
				}
				break;
				case 7:
				{
					// set gutter text colour.
					this->m_clrGutter_txt = a.to_<COLORREF>();
				}
				break;
				case 8:
				{
					// set gutter border colour.
					this->m_clrGutter_border = a.to_<COLORREF>();
				}
				break;
				case 9:
				{
					// set gutter lock state.
					this->m_bLockGutter = (a.to_int() == 0);
				}
				break;
				case 10:
				{
					// set gutter size.
					this->m_GutterWidth = a.to_<UINT>();
				}
				break;
				case 11:
				{
					// set gutter border size.
				}
				break;
				default:
					break;
				}
			}
			++argcnt;
		}
	}
	// xdid -i [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('i')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto pos = this->GetCaretPos();

		const auto nLine = input.getnexttok().to_<UINT>();								// tok 4

		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			if (nLine == 0)
				throw DcxExceptions::dcxInvalidArguments();

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
			Dcx::dcxRichEdit_SetBkgndColor(m_Hwnd, TRUE, 0);
			this->setTextBackColor(GetSysColor(COLOR_WINDOWTEXT));
		}
		else
		{
			Dcx::dcxRichEdit_SetBkgndColor(m_Hwnd, 0, clrColor);
			this->setTextBackColor(clrColor);
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
		{
			if (nLine == 0)
				throw DcxExceptions::dcxInvalidArguments();

			this->m_tsText.puttok(input.getlasttoks(), nLine, TEXT("\r\n"));	// tok 5, -1
		}
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

		if (nColor >= std::min(std::size(m_aColorPalette), gsl::narrow_cast<size_t>(16U)))
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

				if (!LoadRichTextFromXml(tsFile, tsDataSet))	// default load rtf text
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
		Dcx::dcxEdit_ScrollCaret(m_Hwnd);
	}
	// xdid -V [NAME] [ID]
	else if (flags[TEXT('V')])
	{
		Dcx::dcxEdit_ScrollCaret(m_Hwnd);
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
				if (m_bShowLineNumbers)
					setFmtRect();
				else
					resetFmtRect();
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

		if (!Dcx::dcxEdit_SetZoom(m_Hwnd, num, den))
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
	chrf.crTextColor = this->m_TextOptions.m_clrText;
	chrf.crBackColor = this->m_TextOptions.m_clrTextBackground;
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

void DcxRichEdit::DrawClientArea(HDC hdc)
{
	const auto nLogPixelsX = ::GetDeviceCaps(hdc, LOGPIXELSX);
	const auto nLogPixelsY = ::GetDeviceCaps(hdc, LOGPIXELSY);

	//RECT rc{};
	//GetClientRect(m_Hwnd, &rc);

	RECT rc = getFmtRect();
	if (IsRectEmpty(&rc))
		GetClientRect(m_Hwnd, &rc);

	// this is a quick fix to draw the bkg colour, will need more work to support the full range of bkg features.
	if (this->getBackTextColor() == CLR_INVALID)
		this->setTextBackColor(Dcx::dcxRichEdit_GetBkgndColor(m_Hwnd));

	if (!isExStyle(WindowExStyle::Transparent))
		Dcx::FillRectColour(hdc, &rc, this->getBackTextColor());
	//DrawCtrlBackground(hdc, this, &rc);

	rc.left -= 1;
	rc.top -= 2;

	rc.left = MulDiv(rc.left, 1440, nLogPixelsX);
	rc.top = MulDiv(rc.top, 1440, nLogPixelsY);
	rc.right = MulDiv(rc.right, 1440, nLogPixelsX);
	rc.bottom = MulDiv(rc.bottom, 1440, nLogPixelsY);

	POINTL point{};

	FORMATRANGE fr{};

	fr.hdc = hdc;
	fr.hdcTarget = hdc;
	fr.rc = rc;
	fr.rcPage = rc;
	fr.chrg.cpMin = Dcx::dcxRichEdit_CharFromPos(m_Hwnd, &point);
	fr.chrg.cpMax = -1;

	//Requesting to draw on the DC
	Dcx::dcxRichEdit_FormatRange(m_Hwnd, TRUE, &fr);
	Dcx::dcxRichEdit_FormatRange(m_Hwnd, FALSE, nullptr);

	if (m_bShowLineNumbers)
		DrawGutter(hdc);
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

	const RECT rcFmt = getFmtRect();

	//RECT rcClient{};
	//GetClientRect(m_Hwnd, &rcClient);
	//RECT m_FRGutter = rcClient;
	//m_FRGutter.right = std::max(rcFmt.left - 3, 0L);

	RECT m_FRGutter{};
	m_FRGutter.top = rcFmt.top;
	m_FRGutter.bottom = rcFmt.bottom;
	m_FRGutter.right = std::max(rcFmt.left, 0L);
	m_FRGutter.left = 0;

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
			const auto iLineChar = Dcx::dcxEdit_GetLineIndex(m_Hwnd, rng.b);
			const POINTL pl = Dcx::dcxEdit_GetPosFromChar(m_Hwnd, iLineChar);
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
		//BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, *hdcbuf, 0, 0, SRCCOPY);
		BitBlt(hdc, 0, 0, rcFmt.right, rcFmt.bottom, *hdcbuf, 0, 0, SRCCOPY);
	}
}

Dcx::range_t<DWORD> DcxRichEdit::GetVisibleRange() const noexcept
{
	// find the index of the top visible line

	const auto start_line = Dcx::dcxEdit_GetFirstVisibleLine(m_Hwnd);

	const RECT rc = getFmtRect();

	// find the index of the last visible line
	POINTL pt{ 0, rc.bottom };
	const auto char_index = Dcx::dcxRichEdit_CharFromPos(m_Hwnd, &pt);
	const auto stop_line = Dcx::dcxRichEdit_ExLineFromChar(m_Hwnd, char_index);

	// +1 to make range inclusive
	return { start_line, stop_line + 1 };
}

GSL_SUPPRESS(con.4)
DWORD DcxRichEdit::GetCaretPos() const noexcept
{
	//DWORD hiPos{}, loPos{};
	//Dcx::dcxEdit_GetSel(m_Hwnd, &loPos, &hiPos);
	//if (loPos != hiPos)
	//	--hiPos;
	//return hiPos;
	////// windows 10 only :/
	////return Edit_GetCaretIndex(m_Hwnd);

	return Dcx::dcxEdit_GetCaretIndex(m_Hwnd);
}

DWORD DcxRichEdit::GetCaretLine() const noexcept
{
	const auto pos = GetCaretPos();
	return Dcx::dcxRichEdit_ExLineFromChar(m_Hwnd, pos);
}

void DcxRichEdit::setCaretPos(DWORD pos) noexcept
{
	CHARRANGE c{ gsl::narrow_cast<long>(pos), gsl::narrow_cast<long>(pos) };

	Dcx::dcxRichEdit_ExSetSel(m_Hwnd, &c);
}

void DcxRichEdit::setRicheditFont(const TString& tsFlags, const TString& tsCharset, UINT iSize, const TString& tsFontname)
{
	this->setRedraw(FALSE);
	Auto(this->setRedraw(TRUE));

	TString tsInput;
	tsInput.addtok(tsFlags);
	tsInput.addtok(tsCharset);
	tsInput.addtok(iSize);
	tsInput.addtok(tsFontname);

	if (LOGFONT lf{ }; ParseCommandToLogfont(tsInput, &lf))
		setControlFont(CreateFontIndirect(&lf), FALSE);

	const auto iFontFlags = parseFontFlags(tsFlags);

	if (dcx_testflag(iFontFlags, dcxFontFlags::DCF_DEFAULT))
	{
		this->m_TextOptions.m_clrTextBackground = GetSysColor(COLOR_WINDOW);
		this->m_TextOptions.m_clrText = GetSysColor(COLOR_WINDOWTEXT);
		this->m_iFontSize = 10 * 20;
		this->m_bFontBold = false;
		this->m_bFontItalic = false;
		this->m_bFontUnderline = false;
		this->m_bFontStrikeout = false;
		this->m_byteCharset = DEFAULT_CHARSET;
		this->setContentsFont();
		this->parseContents(TRUE);
	}
	else if (!tsFontname.empty())
	{
		this->m_byteCharset = gsl::narrow_cast<BYTE>(parseFontCharSet(tsCharset));
		this->m_iFontSize = 20U * iSize;
		this->m_tsFontFaceName = tsFontname;

		m_bFontBold = dcx_testflag(iFontFlags, dcxFontFlags::DCF_BOLD);

		m_bFontItalic = dcx_testflag(iFontFlags, dcxFontFlags::DCF_ITALIC);

		m_bFontStrikeout = dcx_testflag(iFontFlags, dcxFontFlags::DCF_STRIKEOUT);

		m_bFontUnderline = dcx_testflag(iFontFlags, dcxFontFlags::DCF_UNDERLINE);

		this->setContentsFont();
		this->parseContents(TRUE);
	}
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
	const auto line = Dcx::dcxEdit_LineIndex(m_Hwnd, Dcx::dcxEdit_LineFromChar(m_Hwnd, len - 1));
	// get line length
	const auto linelen = Dcx::dcxEdit_LineLength(m_Hwnd, line);
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
	//return SendMessage(m_Hwnd, EM_AUTOURLDETECT, gsl::narrow_cast<WPARAM>(iEnable), 0);

	// this is ok as TRUE == 1 == AURL_ENABLEURL, & FALSE == 0 == Disable
	return Dcx::dcxRichEdit_AutoUrlDetect(m_Hwnd, iEnable, nullptr);
}

/*!
* \brief blah
*
* blah
*/
void DcxRichEdit::hideSelection(const BOOL iHide) noexcept
{
	Dcx::dcxRichEdit_HideSelection(m_Hwnd, iHide);
}

/*!
* \brief blah
*
* blah
*/
void DcxRichEdit::setSel(const int iStart, const int iEnd) noexcept
{
	Dcx::dcxEdit_SetSel(m_Hwnd, iStart, iEnd);
}

/*!
* \brief blah
*
* blah
*/
void DcxRichEdit::replaceSel(const BOOL bUndo, LPCTSTR lpstr) noexcept
{
	Dcx::dcxEdit_ReplaceSel(m_Hwnd, bUndo, lpstr);
}

/*!
* \brief blah
*
* blah
*/
DWORD DcxRichEdit::getCharFormat(const UINT iType, CHARFORMAT2* const cfm) const noexcept
{
	return Dcx::dcxRichEdit_GetCharFormat(m_Hwnd, iType, *cfm);
}

/*!
* \brief blah
*
* blah
*/

bool DcxRichEdit::setCharFormat(const UINT iType, CHARFORMAT2* const cfm) noexcept
{
	if (!cfm)
		return false;
	return Dcx::dcxRichEdit_SetCharFormat(m_Hwnd, iType, *cfm);
}

void DcxRichEdit::toXml(TiXmlElement* const xml) const
{
	if (!xml)
		return;

	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());
	if (this->m_PassChar > 0)
		xml->SetAttribute("passchar", this->m_PassChar);
	if (this->m_bCueFocused)
		xml->SetAttribute("cuefocused", "1");
	if (!this->m_tsCue.empty())
		xml->SetAttribute("cue", this->m_tsCue.c_str());

	setColourAttribute(xml, "gutterbgcolour", this->m_clrGutter_bkg);
	setColourAttribute(xml, "gutterselbgcolour", this->m_clrGutter_selbkg);
	setColourAttribute(xml, "guttertextcolour", this->m_clrGutter_txt);
	setColourAttribute(xml, "gutterseltextcolour", this->m_clrGutter_seltxt);
	setColourAttribute(xml, "gutterbordercolour", this->m_clrGutter_border);
	xml->SetAttribute("gutterwidth", this->m_GutterWidth);
	if (this->m_bLockGutter)
		xml->SetAttribute("lockgutter", "1");

	xml->LinkEndChild(new TiXmlText(this->m_tsText.c_str()));
}

TiXmlElement* DcxRichEdit::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxRichEdit::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	this->m_bIgnoreInput = true;

	{
		if (auto clr = queryAttribute(xThis, "font"); !_ts_isEmpty(clr))
		{
			// single font item
			// + charset size fontname
			TString tsFont(clr);
			TString tsFlags(tsFont.getfirsttok(1));
			TString tsCharset(tsFont.getnexttok());
			const auto iSize = tsFont.getnexttokas<UINT>();
			TString tsFontname(tsFont.getlasttoks());

			setRicheditFont(tsFlags, tsCharset, iSize, tsFontname);
		}
		else {
			// split font parts
			// fontstyle charset fontsize fontname
			const auto iSize = queryIntAttribute(xThis, "fontsize", 8);
			TString tsCharset(queryAttribute(xThis, "chatset", "ansi"));
			TString tsFlags(queryAttribute(xThis, "fontstyle", "+d"));
			TString tsFontname(queryAttribute(xThis, "fontname"));

			setRicheditFont(tsFlags, tsCharset, iSize, tsFontname);
		}
	}

	if (const auto tmp = gsl::narrow_cast<TCHAR>(queryIntAttribute(xThis, "passchar")); tmp)
	{
		Dcx::dcxEdit_SetPasswordChar(m_Hwnd, tmp);
		this->m_PassChar = tmp;
	}

	m_bCueFocused = (queryIntAttribute(xThis, "cuefocused") > 0);

	if (const auto tmp = queryAttribute(xThis, "cue"); tmp)
	{
		this->m_tsCue = tmp;
		Dcx::dcxEdit_SetCueBannerTextFocused(m_Hwnd, this->m_tsCue.to_chr(), m_bCueFocused);
	}

	if (const auto tmp = queryColourAttribute(xThis, "gutterbgcolour"); tmp != CLR_INVALID)
		this->m_clrGutter_bkg = tmp;
	if (const auto tmp = queryColourAttribute(xThis, "gutterselbgcolour"); tmp != CLR_INVALID)
		this->m_clrGutter_selbkg = tmp;
	if (const auto tmp = queryColourAttribute(xThis, "guttertextcolour"); tmp != CLR_INVALID)
		this->m_clrGutter_txt = tmp;
	if (const auto tmp = queryColourAttribute(xThis, "gutterseltextcolour"); tmp != CLR_INVALID)
		this->m_clrGutter_seltxt = tmp;
	if (const auto tmp = queryColourAttribute(xThis, "gutterbordercolour"); tmp != CLR_INVALID)
		this->m_clrGutter_border = tmp;
	if (const auto tmp = queryIntAttribute(xThis, "gutterwidth", DCX_EDIT_GUTTER_WIDTH); tmp >= DCX_EDIT_GUTTER_WIDTH)
		this->m_GutterWidth = tmp;
	m_bLockGutter = (queryIntAttribute(xThis, "lockgutter") > 0);

	{
		this->m_tsText = xThis->GetText();
		SetWindowText(m_Hwnd, this->m_tsText.to_chr());
		this->parseContents(TRUE);
		Dcx::dcxEdit_SetModify(m_Hwnd, FALSE);
	}

	this->m_bIgnoreInput = false;
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
		styles.addtok(TEXT("showlinenumbers"));

	const auto dStyles = Edit_GetExtendedStyle(m_Hwnd);
	if (dcx_testflag(dStyles, ES_EX_ZOOMABLE))
		styles.addtok(TEXT("zoomable"));
	if (dcx_testflag(dStyles, ES_EX_ALLOWEOL_LF))
		styles.addtok(TEXT("eollf"));
	if (dcx_testflag(dStyles, ES_EX_ALLOWEOL_CR))
		styles.addtok(TEXT("eolcr"));
	if (dcx_testflag(dStyles, ES_EX_CONVERT_EOL_ON_PASTE))
		styles.addtok(TEXT("convert"));

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
			if (dcx_testflag(getEventMask(), DCX_EVENT_CLICK))
			{
				if (dcxlParam(ENLINK*, enl); (enl) && ((enl->msg == WM_LBUTTONDOWN) || (enl->msg == WM_LBUTTONDBLCLK) || (enl->msg == WM_RBUTTONDOWN)))
				{
					TEXTRANGE tr{};

					// get information about link text
					tr.chrg = enl->chrg;
					const auto strlen = (enl->chrg.cpMax - enl->chrg.cpMin) + 1U;

					auto str = std::make_unique<TCHAR[]>(strlen);

					tr.lpstrText = str.get();
					Dcx::dcxRichEdit_GetTextRange(m_Hwnd, tr);

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

			if (dcx_testflag(getEventMask(), DCX_EVENT_EDIT))
			{
				if (const auto* const sel = reinterpret_cast<SELCHANGE*>(lParam); (sel) && (sel->seltyp != SEL_EMPTY))
				{
					auto str = std::make_unique<TCHAR[]>((sel->chrg.cpMax - sel->chrg.cpMin) + 1U);

					TEXTRANGE tr{};

					// get information about selected text
					tr.chrg = sel->chrg;
					tr.lpstrText = str.get();
					Dcx::dcxRichEdit_GetTextRange(m_Hwnd, tr);

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

		if (dcx_testflag(getEventMask(), DCX_EVENT_EDIT))
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
		if (dcx_testflag(getEventMask(), DCX_EVENT_EDIT))
			this->execAliasEx(TEXT("keyup,%u,%d"), getUserID(), wParam);

		if (m_bShowLineNumbers)
			PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
	}
	break;

	case WM_PRINTCLIENT:
	{
		bParsed = TRUE;

		dcxwParam(HDC, hdc);

		//CleanUpParentCache();

		// Setup alpha blend if any.
		const auto ai = this->SetupAlphaBlend(&hdc);
		Auto(this->FinishAlphaBlend(ai));

		// richedits printclient appears broken & non functional.
		//const auto lRes = CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
		//if (m_bShowLineNumbers)
		//	DrawGutter(hdc);

		DrawClientArea(hdc);

		return 0L;
	}
	break;

	case WM_PAINT:
	{
		//CleanUpParentCache();
		//
		//if (this->IsAlphaBlend())
		//{
		//	PAINTSTRUCT ps{};
		//
		//	auto hdc = BeginPaint(m_Hwnd, &ps);
		//	Auto(EndPaint(m_Hwnd, &ps));
		//
		//	bParsed = TRUE;
		//
		//	// Setup alpha blend if any.
		//	const auto ai = this->SetupAlphaBlend(&hdc);
		//	Auto(this->FinishAlphaBlend(ai));
		//
		//	const auto lRes = CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
		//
		//	if (m_bShowLineNumbers)
		//		//PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
		//		DrawGutter(hdc);
		//
		//	return lRes;
		//}
		//else if (m_bShowLineNumbers)
		//{
		//	bParsed = TRUE;
		//	const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);
		//
		//	//PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
		//	if (wParam)
		//		DrawGutter(reinterpret_cast<HDC>(wParam));
		//	else
		//		DrawGutter();
		//
		//	return lRes;
		//}

		//CleanUpParentCache();

		dcxwParam(HDC, hdc);

		if (this->IsAlphaBlend())
		{
			PAINTSTRUCT ps{};

			if (!wParam)
				hdc = BeginPaint(m_Hwnd, &ps);
			Auto({ if (!wParam) EndPaint(m_Hwnd, &ps); });

			bParsed = TRUE;

			// Setup alpha blend if any.
			const auto ai = this->SetupAlphaBlend(&hdc);
			Auto(this->FinishAlphaBlend(ai));

			const auto lRes = CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);

			if (m_bShowLineNumbers)
				DrawGutter(hdc);

			return lRes;
		}
		else if (m_bShowLineNumbers)
		{
			bParsed = TRUE;
			const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);

			if (hdc)
				DrawGutter(hdc);
			else
				DrawGutter();

			return lRes;
		}
	}
	break;

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

	case WM_SETCURSOR:
	{
		if ((!m_bLockGutter) && (Dcx::dcxLOWORD(lParam) == HTCLIENT) && (to_hwnd(wParam) == m_Hwnd) && (IsCursorOnGutterBorder()))
		{
			if (auto hCursor = LoadCursor(nullptr, IDC_SIZEWE); GetCursor() != hCursor)
				SetCursor(hCursor);
			bParsed = TRUE;
			return TRUE;
		}
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
	}
	break;

	case WM_LBUTTONDOWN:
	{
		if (IsCursorOnGutterBorder())
		{
			m_bDraggingGutter = true;
			SetCapture(m_Hwnd);

			bParsed = TRUE;
			return 0L;
		}
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
	}
	break;
	case WM_LBUTTONUP:
	{
		if (m_bDraggingGutter)
		{
			const POINT pt{ Dcx::dcxLOWORD(lParam), Dcx::dcxHIWORD(lParam) };
			RECT rcClient{};
			GetClientRect(m_Hwnd, &rcClient);
			if (!PtInRect(&rcClient, pt))
			{
				// outside ctrl, reset to default width.
				m_GutterWidth = DCX_EDIT_GUTTER_WIDTH;
			}
			setFmtRect();
			ReleaseCapture();
			m_bDraggingGutter = false;

			bParsed = TRUE;
			return 0L;
		}
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (m_bDraggingGutter)
		{
			const POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

			m_GutterWidth = std::max<long>(DCX_EDIT_GUTTER_WIDTH, pt.x);

			setFmtRect();

			//setFmtRect(false);

			//if (auto hdc = GetDC(m_Hwnd); hdc)
			//{
			//	Auto(ReleaseDC(m_Hwnd, hdc));
			//	DrawGutter(hdc);
			//	//SendMessage(m_Hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdc), PRF_CLIENT|PRF_ERASEBKGND);
			//	//SendMessage(m_Hwnd, WM_PAINT, reinterpret_cast<WPARAM>(hdc), 0);
			//}

			//DrawGutter();

			//const RECT rc = getFmtRect();
			//InvalidateRect(m_Hwnd, &rc, FALSE);

			bParsed = TRUE;
			return 0L;
		}
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
	}
	break;

	case WM_DESTROY:
	{
		this->CallDefaultClassProc(uMsg, wParam, lParam);

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
bool DcxRichEdit::LoadRichTextFromXml(TString& tsFilename, const TString& tsDataSet)
{
	if (!IsFile(tsFilename))
		throw Dcx::dcxException(TEXT("Unable To Access File: %"), tsFilename);

	TiXmlDocument doc(tsFilename.c_str());
	doc.SetCondenseWhiteSpace(false);

	if (!doc.LoadFile())
		throw Dcx::dcxException(TEXT("Not an XML File: %"), tsFilename);

	const auto* const xRoot = doc.FirstChildElement("dcxml");
	if (!xRoot)
		throw Dcx::dcxException("Unable Find <dcxml> root");

	const auto* xElm = xRoot->FirstChildElement("richedit_data");
	if (!xElm)
		throw Dcx::dcxException("Unable To Find <richedit_data> element");

	xElm = xElm->FirstChildElement(tsDataSet.c_str());
	if (!xElm)
		throw Dcx::dcxException(TEXT("Unable To Find Dataset: %"), tsDataSet);

	TString tsText(xElm->GetText());

	if (tsText.left(2) == TEXT("\r\n"))
		tsText = tsText.right(-2);
	else if (tsText[0] == TEXT('\n'))
		tsText = tsText.right(-1);

	tsText -= TEXT('\t'); // remove all tabs from text.

	// converts \c -> ctrl-k, \b -> ctrl-b, \r -> ctrl-r, \u -> ctrl-u, \i -> ctrl-i, \o -> ctrl-o
	tsText.replace(TEXT("\\c"), TEXT('\x03'));	// mirc colours
	tsText.replace(TEXT("\\b"), TEXT('\x02'));	// bold
	tsText.replace(TEXT("\\r"), TEXT('\x16'));	// reverse
	tsText.replace(TEXT("\\u"), TEXT('\x1F'));	// underline
	tsText.replace(TEXT("\\i"), TEXT('\x1D'));	// italics
	tsText.replace(TEXT("\\o"), TEXT('\x0F'));	// ctrl-o

	m_tsText = tsText;

	parseContents(TRUE);

	return true;
}

LRESULT DcxRichEdit::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}

DWORD DcxRichEdit::parseExEditStyles(const TString& tsStyles)
{
	DWORD dStyles{};

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"zoomable"_hash:
			dStyles |= ES_EX_ZOOMABLE;
			break;
		case L"eollf"_hash:
			dStyles |= ES_EX_ALLOWEOL_LF;
			break;
		case L"eolcr"_hash:
			dStyles |= ES_EX_ALLOWEOL_CR;
			break;
		case L"convert"_hash:
			dStyles |= ES_EX_CONVERT_EOL_ON_PASTE;
			break;
		default:
			break;
		}
	}

	return dStyles;
}
