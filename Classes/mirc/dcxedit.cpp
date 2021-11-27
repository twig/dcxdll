/*!
* \file dcxedit.cpp
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
#include "Classes/mirc/dcxedit.h"
#include "Classes/dcxdialog.h"

/*!
* \brief Constructor
*
* \param ID Control ID
* \param p_Dialog Parent DcxDialog Object
* \param mParentHwnd Parent Window Handle
* \param rc Window Rectangle
* \param styles Window Style Tokenized List
*/

DcxEdit::DcxEdit(const UINT ID, DcxDialog* const p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	if (!p_Dialog)
		throw Dcx::dcxException("Unable to get controls parent dialog");

	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WindowExStyle::ClientEdge,
		DCX_EDITCLASS,
		ws.m_Styles | WindowStyle::Child,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsWindow(m_Hwnd))
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	Edit_LimitText(m_Hwnd, 0);

	//SendMessage(m_Hwnd, CCM_SETUNICODEFORMAT, TRUE, NULL);

	if (styles.istok(TEXT("tooltips")))
	{
		if (!IsWindow(p_Dialog->getToolTip()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		setToolTipHWND(p_Dialog->getToolTip());
		AddToolTipToolInfo(getToolTipHWND(), m_Hwnd);
	}

	this->m_bIgnoreRepeat = true;
	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);
	DragAcceptFiles(m_Hwnd, TRUE);
}

/*!
* \brief blah
*
* blah
*/
DcxEdit::~DcxEdit() noexcept
{
}


const TString DcxEdit::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, ES_MULTILINE))
		styles.addtok(TEXT("multi"));
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
	if (dcx_testflag(Styles, ES_LOWERCASE))
		styles.addtok(TEXT("lowercase"));
	if (dcx_testflag(Styles, ES_NUMBER))
		styles.addtok(TEXT("number"));
	if (dcx_testflag(Styles, ES_PASSWORD))
		styles.addtok(TEXT("password"));
	if (dcx_testflag(Styles, ES_UPPERCASE))
		styles.addtok(TEXT("uppercase"));
	if (dcx_testflag(Styles, ES_WANTRETURN))
		styles.addtok(TEXT("return"));
	if (dcx_testflag(Styles, ES_READONLY))
		styles.addtok(TEXT("readonly"));
	if (dcx_testflag(Styles, ES_NOHIDESEL))
		styles.addtok(TEXT("showsel"));
	if (m_bShowLineNumbers)
		styles.addtok(TEXT("showlinenumbers"));
	return styles;
}

void DcxEdit::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("caption", m_tsText.c_str());
	xml->SetAttribute("styles", getStyles().c_str());
}

TiXmlElement* DcxEdit::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

/*!
* \brief blah
*
* blah
*/

dcxWindowStyles DcxEdit::parseControlStyles(const TString& tsStyles)
{
	dcxWindowStyles ws;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"multi"_hash:
			ws.m_Styles |= ES_MULTILINE;
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
		case L"lowercase"_hash:
			ws.m_Styles |= ES_LOWERCASE;
			break;
		case L"number"_hash:
			ws.m_Styles |= ES_NUMBER;
			break;
		case L"password"_hash:
			ws.m_Styles |= ES_PASSWORD;
			break;
		case L"uppercase"_hash:
			ws.m_Styles |= ES_UPPERCASE;
			break;
		case L"return"_hash:
			ws.m_Styles |= ES_WANTRETURN;
			break;
		case L"readonly"_hash:
			ws.m_Styles |= ES_READONLY;
			break;
		case L"showsel"_hash:
			ws.m_Styles |= ES_NOHIDESEL;
			break;
		case L"showlinenumbers"_hash:
			m_bShowLineNumbers = true;
			break;
		default:
			break;
		}
	}

	return parseGeneralControlStyles(tsStyles, ws);
}

//TString DcxEdit::parseInfoRequest(const TString& input) const
//{
//	return TString();
//}

/*!
* \brief $xdid Parsing Function
*
* \param input [NAME] [ID] [PROP] (OPTIONS)
* \param szReturnValue mIRC Data Container
*
* \return > void
*/
GSL_SUPPRESS(con.4)
void DcxEdit::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP] [N]
	case L"text"_hash:
	{
		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			if (input.numtok() > 3)
			{
				if (const auto nLine = input.getnexttok().to_int(); (nLine > 0 && nLine <= gsl::narrow_cast<int>(m_tsText.numtok(TEXT("\r\n")))))
					szReturnValue = m_tsText.gettok(nLine, TEXT("\r\n")).to_chr();
			}
		}
		else
			szReturnValue = m_tsText.to_chr();
	}
	break;

	// [NAME] [ID] [PROP]
	case L"num"_hash:
	{
		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			const auto i = this->m_tsText.numtok(TEXT("\r\n"));
			_ts_snprintf(szReturnValue, TEXT("%u"), i);
		}
		else {
			// single line control so always 1 line.
			szReturnValue[0] = TEXT('1');
			szReturnValue[1] = 0;
		}
	}
	break;

	// [NAME] [ID] [PROP]
	case L"ispass"_hash:
		szReturnValue = dcx_truefalse(isStyle(WindowStyle::ES_Password));
		break;

		// [NAME] [ID] [PROP]
	case L"isreadonly"_hash:
		szReturnValue = dcx_truefalse(isStyle(WindowStyle::ES_ReadOnly));
		break;

		// [NAME] [ID] [PROP]
	case L"caretpos"_hash:
	{
		DWORD dwAbsoluteStartSelPos{};

		// caret startsel position
		SendMessage(m_Hwnd, EM_GETSEL, reinterpret_cast<WPARAM>(&dwAbsoluteStartSelPos), NULL);

		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			// current line
			const auto iLinePos = SendMessage(m_Hwnd, EM_LINEFROMCHAR, gsl::narrow_cast<WPARAM>(-1), NULL) + 1;
			// line offset
			const auto CharPos = (dwAbsoluteStartSelPos - gsl::narrow_cast<int>(SendMessage(m_Hwnd, EM_LINEINDEX, gsl::narrow_cast<WPARAM>(-1), NULL)));

			_ts_snprintf(szReturnValue, TEXT("%d %u %u"), iLinePos, CharPos, dwAbsoluteStartSelPos);
		}
		else {
			// return selstart
			_ts_snprintf(szReturnValue, TEXT("1 %u %u"), dwAbsoluteStartSelPos, dwAbsoluteStartSelPos);
		}
	}
	break;

	case L"selstart"_hash:
	{
		DWORD dwSelStart{}; // selection range starting position

		SendMessage(m_Hwnd, EM_GETSEL, reinterpret_cast<WPARAM>(&dwSelStart), NULL);
		_ts_snprintf(szReturnValue, TEXT("%u"), dwSelStart);
	}
	break;

	case L"selend"_hash:
	{
		DWORD dwSelEnd{};   // selection range ending position

		SendMessage(m_Hwnd, EM_GETSEL, NULL, reinterpret_cast<LPARAM>(&dwSelEnd));
		_ts_snprintf(szReturnValue, TEXT("%u"), dwSelEnd);
	}
	break;

	case L"sel"_hash:
	{
		DWORD dwSelStart{}; // selection range starting position
		DWORD dwSelEnd{};   // selection range ending position

		SendMessage(m_Hwnd, EM_GETSEL, reinterpret_cast<WPARAM>(&dwSelStart), reinterpret_cast<LPARAM>(&dwSelEnd));
		_ts_snprintf(szReturnValue, TEXT("%u %u"), dwSelStart, dwSelEnd);
	}
	break;

	case L"seltext"_hash:
	{
		DWORD dwSelStart{}; // selection range starting position
		DWORD dwSelEnd{};   // selection range ending position

		SendMessage(m_Hwnd, EM_GETSEL, reinterpret_cast<WPARAM>(&dwSelStart), reinterpret_cast<LPARAM>(&dwSelEnd));
		szReturnValue = m_tsText.mid(gsl::narrow_cast<int>(dwSelStart), gsl::narrow_cast<int>(dwSelEnd - dwSelStart)).to_chr();
	}
	break;

	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N]
	//case L"find"_hash:
	//{
	//	if (input.numtok() < 6)
	//		throw DcxExceptions::dcxInvalidArguments();
	//
	//	const auto matchtext(input.getfirsttok(2, TSTABCHAR).trim());
	//	const auto params(input.getnexttok(TSTABCHAR).trim());	// tok 3
	//
	//	if (matchtext.empty())
	//		throw Dcx::dcxException("No Match text supplied");
	//
	//	auto SearchType = DcxListHelper::CharToSearchType(params++[0]);
	//
	//	const auto N = params++.to_<UINT>();	// tok 2
	//
	//	// count total
	//	if (N == 0)
	//	{
	//		auto count = matchItemText(0, matchtext, SearchType);
	//
	//		_ts_snprintf(szReturnValue, TEXT("%d"), count);
	//	}
	//	// find Nth matching
	//	else {
	//		auto count = decltype(N){0};
	//
	//		if (DcxListHelper::matchItemText(N, matchtext, SearchType))
	//			++count;
	//
	//		// found Nth matching
	//		if (count == N)
	//		{
	//			++i;
	//			_ts_snprintf(szReturnValue, TEXT("%d"), i);
	//			return;
	//		}
	//	} // else
	//}
	//break;

	case L"cue"_hash:
	{
		if (!this->m_tsCue.empty())
			szReturnValue = m_tsCue.to_chr();
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
		break;
	}
}

/*!
* \brief blah
*
* blah
*/
void DcxEdit::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
	{
		this->m_tsText.clear();	// = TEXT("");
		SetWindowTextW(m_Hwnd, L"");
	}

	// xdid -a [NAME] [ID] [SWITCH] [TEXT]
	if (flags[TEXT('a')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto pos = this->GetCaretPos();

		this->m_tsText += input.getlasttoks();	// tok 4, -1
		SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());

		this->setCaretPos(pos);
	}
	// xdid -c [NAME] [ID] [SWITCH]
	else if (flags[TEXT('c')])
	{
		if (numtok < 3)
			throw DcxExceptions::dcxInvalidArguments();

		CopyToClipboard(m_Hwnd, this->m_tsText);
	}
	// xdid -C [NAME] [ID] [SWITCH] [POS]
	else if (flags[TEXT('C')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		auto pos = input.getnexttok().to_<long long>();
		if (pos < 0)
		{
			const auto oldPos = this->GetCaretPos();
			pos += oldPos;
			if (pos < 0)
				pos = 0;
		}
		this->setCaretPos(gsl::narrow_cast<DWORD>(pos));
	}
	// xdid -d [NAME] [ID] [SWITCH] [N,N2,N3-N4...]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		//if (this->isStyle(WindowStyle::ES_MultiLine))
		//{
		//	const auto nLine = input.getnexttok().to_<UINT>();	// tok 4
		//	this->m_tsText.deltok(nLine, TEXT("\r\n"));
		//	SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());
		//}

		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			auto tsLines(input.getnexttok());

			// reverse numeric sort line numbers
			tsLines.sorttok(TEXT("nr"), TSCOMMA);

			const auto pos = this->GetCaretPos();

			const auto itEnd = tsLines.end();
			for (auto itStart = tsLines.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
			{
				const TString tsLineRange(*itStart);
				UINT nStartLine{}, nEndLine{};
				if (tsLineRange.numtok(TEXT('-')) == 2)
				{
					nStartLine = tsLineRange.getfirsttok(1,TEXT('-')).to_<UINT>();
					nEndLine = tsLineRange.getnexttok(TEXT('-')).to_<UINT>();
				}
				else {
					nStartLine = nEndLine = tsLineRange.to_<UINT>();
				}
				// delete lines from the back of the text so it doesnt change the position of other lines.
				for (auto nLine = nEndLine; nLine >= nStartLine; --nLine)
					this->m_tsText.deltok(nLine, TEXT("\r\n"));
			}
			SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());

			this->setCaretPos(pos);
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
			const auto nLine = input.getnexttok().to_<UINT>();	// tok 4
			this->m_tsText.instok(input.getlasttoks(), nLine, TEXT("\r\n"));	// tok 5, -1
		}
		else
			this->m_tsText = input.getlasttoks();	// tok 5, -1
		SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());

		this->setCaretPos(pos);
	}
	// xdid -I [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('I')])
	{
		// inserts supplied [TEXT] at char pos [N]
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		//const auto nChar = input.getnexttok().to_<UINT>();	// tok 4
		//TString tsLeft(this->m_tsText.sub(0, nChar));
		//tsLeft += input.getlasttoks();
		//tsLeft += this->m_tsText.sub(nChar, this->m_tsText.len());

		const auto nChar = input.getnexttok().to_<UINT>();	// tok 4
		const TString tsInsert(input.getlasttoks());

		TString tsLeft(this->m_tsText.len() + tsInsert.len());

		tsLeft = this->m_tsText.sub(0, nChar);
		tsLeft += tsInsert;
		tsLeft += this->m_tsText.sub(nChar, this->m_tsText.len());

		const auto pos = this->GetCaretPos();

		this->m_tsText = tsLeft;

		SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());

		this->setCaretPos(pos);
	}
	// xdid -j [NAME] [ID] [SWITCH] [0|1] (CHAR)
	else if (flags[TEXT('j')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto i = input.getnexttok().to_<UINT>();	// tok 4
		TCHAR cPassChar = input.getnexttok().at(0);	// tok 5

		if (cPassChar == 0)
			cPassChar = Edit_GetPasswordChar(m_Hwnd);
		if (cPassChar == 0)
			cPassChar = this->m_PassChar;
		// XP actually uses the unicode `Black Circle` char U+25CF (9679)
		// The problem is getting the char set to a unicode (2-byte) one, so far it always sets to CF (207)
		if (cPassChar == 0)
		{
			if (Dcx::VistaModule.isVista())
			{
				if (Dcx::VistaModule.isWin7())
				{
					cPassChar = TEXT('\u25CF');	// Win7 char
				}
				else
					cPassChar = TEXT('•'); // Vista char (unsure if this is the right char)
			}
			else
				cPassChar = TEXT('•'); // XP char
			//c = TEXT('*'); // before win xp
		}
		if (i)
		{
			this->addStyle(WindowStyle::ES_Password);

			Edit_SetPasswordChar(m_Hwnd, cPassChar);
		}
		else {
			this->removeStyle(WindowStyle::ES_Password);
			Edit_SetPasswordChar(m_Hwnd, 0);
			this->m_PassChar = cPassChar;	// save pass char used for later
		}

		this->redrawWindow();
	}
	// xdid -l [NAME] [ID] [SWITCH] [ON|OFF]
	else if (flags[TEXT('l')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const BOOL enabled = (input.getnexttok().to_int() > 0);	// tok 4

		SendMessage(m_Hwnd, EM_SETREADONLY, gsl::narrow_cast<WPARAM>(enabled), NULL);
	}
	// xdid -o [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('o')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			const auto nLine = input.getnexttok().to_<UINT>();	// tok 4
			this->m_tsText.puttok(input.getlasttoks(), nLine, TEXT("\r\n"));	// tok 5, -1
		}
		else
			this->m_tsText = input.getlasttoks();	// tok 4, -1
		SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());
	}
	// xdid -P [NAME] [ID]
	else if (flags[TEXT('P')])
	{
		SendMessage(this->getHwnd(), WM_PASTE, NULL, NULL);
	}
	// xdid -q [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('q')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		if (const auto N = input.getnexttok().to_int(); N > -1)
			Edit_LimitText(m_Hwnd, N);
	}
	// Used to prevent invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
	{
	}
	// xdid -t [NAME] [ID] [SWITCH] [FILENAME]
	else if (flags[TEXT('t')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		auto tsFile(input.getlasttoks().trim());	// tok 4, -1

		if (!IsFile(tsFile))
			throw Dcx::dcxException(TEXT("Unable to open: %"), tsFile);

		m_tsText = readTextFile(tsFile);
		SetWindowTextW(m_Hwnd, m_tsText.to_wchr());
	}
	// xdid -u [NAME] [ID] [SWITCH] [FILENAME]
	else if (flags[TEXT('u')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		if (const auto tsFile(input.getlasttoks().trim()); !SaveDataToFile(tsFile, this->m_tsText))
			throw Dcx::dcxException(TEXT("Unable to save: %"), tsFile);
	}
	// xdid -V [NAME] [ID]
	else if (flags[TEXT('V')])
	{
		SendMessage(m_Hwnd, EM_SCROLLCARET, NULL, NULL);
	}
	// xdid -S [NAME] [ID] [SWITCH] [START] (END)
	else if (flags[TEXT('S')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto istart = input.getnexttok().to_int();	// tok 4
		const auto iend = (numtok > 4) ? input.getnexttok().to_int() : istart;

		SendMessage(m_Hwnd, EM_SETSEL, gsl::narrow_cast<WPARAM>(istart), gsl::narrow_cast<LPARAM>(iend));
		SendMessage(m_Hwnd, EM_SCROLLCARET, NULL, NULL);
	}
	// xdid -E [NAME] [ID] [SWITCH] [CUE TEXT]
	else if (flags[TEXT('E')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		this->m_tsCue = input.getlasttoks();	// tok 4, -1
		Edit_SetCueBannerText(m_Hwnd, this->m_tsCue.to_wchr());
	}
	// xdid -y [NAME] [ID] [SWITCH] [0|1|-] (0|1)
	else if (flags[TEXT('y')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsRep(input.getnexttok());
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
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxEdit::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		switch (Dcx::dcxHIWORD(wParam))
		{
		case EN_CHANGE:
		{
			this->m_tsText = TGetWindowText(m_Hwnd);
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
				this->execAliasEx(TEXT("edit,%u"), getUserID());

			if (m_bShowLineNumbers)
				PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
		}
		break;
		case EN_HSCROLL:
			if (m_bShowLineNumbers)
				PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
			break;
		default:
			break;
		}
	} // WM_COMMAND
	break;

	case WM_NOTIFY:
	{
		if (!m_bShowLineNumbers)
			break;

		if (const auto hdr = reinterpret_cast<NMHDR*>(lParam); hdr->hwndFrom == m_Hwnd)
		{
			// msg from richedit
			switch (hdr->code)
			{
			case EN_SELCHANGE:
			{
				if (const auto pSelChange = reinterpret_cast<SELCHANGE*>(lParam); pSelChange->seltyp == SEL_EMPTY)
					PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
			}
			break;
			case EN_UPDATE:
			{
				PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
			}
			break;
			default:
				break;
			}
		}
	}
	break;

	default:
		break;
	}

	return 0L;
}

LRESULT DcxEdit::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
		// This code enables tabstop out of edit controls if tabstop styles used. BUT you cant use tab keys in the edit control when tabstop used.
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
		//if (wParam == VK_ESCAPE)
		//	bParsed = TRUE; // prevents m_pParent window closing.

		if (m_bShowLineNumbers)
			PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);

		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
		{
			if (wParam == VK_RETURN)
				this->execAliasEx(TEXT("return,%u"), getUserID());

			if ((this->m_bIgnoreRepeat) && (lParam & 0x40000000)) // ignore repeats
				break;

			this->execAliasEx(TEXT("keydown,%u,%u"), getUserID(), wParam);
		}
	}
	break;

	case WM_COPY:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
		{
			const stString<256> szRet;

			evalAliasEx(szRet, gsl::narrow_cast<int>(szRet.size()), TEXT("copy,%u"), getUserID());

			if (szRet == TEXT("nocopy"))
			{
				bParsed = TRUE;
				return 0L;
			}
		}
	}
	break;

	case WM_CUT:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
		{
			const stString<256> szRet;

			evalAliasEx(szRet, gsl::narrow_cast<int>(szRet.size()), TEXT("cut,%u"), getUserID());

			if (szRet == TEXT("nocut"))
			{
				bParsed = TRUE;
				return 0L;
			}
		}
	}
	break;

	case WM_PASTE:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
		{
			const stString<256> szRet;

			evalAliasEx(szRet, gsl::narrow_cast<int>(szRet.size()), TEXT("paste,%u"), getUserID());

			if (szRet == TEXT("nopaste"))
			{
				bParsed = TRUE;
				return 0L;
			}
		}
	}
	break;

	case WM_KEYUP:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
			execAliasEx(TEXT("keyup,%u,%u"), getUserID(), wParam);

		if (m_bShowLineNumbers)
			PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
	}
	break;

	case WM_PRINTCLIENT:
	{
		const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);

		if (m_bShowLineNumbers)
			DrawGutter((HDC)wParam);

		bParsed = TRUE;
		return lRes;
	}
	break;

	case WM_PAINT:
	{
		//if (!this->IsAlphaBlend())
		//	break;
		//PAINTSTRUCT ps{};
		//
		//auto hdc = BeginPaint(m_Hwnd, &ps);
		//Auto(EndPaint(m_Hwnd, &ps));
		//
		//bParsed = TRUE;
		//
		//// Setup alpha blend if any.
		//const auto ai = this->SetupAlphaBlend(&hdc);
		//Auto(this->FinishAlphaBlend(ai));
		//
		//return CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);

		if (this->IsAlphaBlend())
		{
			bParsed = TRUE;

			PAINTSTRUCT ps{};
			//auto hdc = BeginPaint(m_Hwnd, &ps);
			//Auto(EndPaint(m_Hwnd, &ps));

			HDC hdc = (HDC)wParam;
			if (!wParam)
				hdc = BeginPaint(m_Hwnd, &ps);

			// Setup alpha blend if any.
			const auto ai = this->SetupAlphaBlend(&hdc);
			Auto(this->FinishAlphaBlend(ai));

			CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
			//CallDefaultClassProc(WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdc), PRF_CLIENT | PRF_CHILDREN /*| PRF_CHECKVISIBLE*/ | PRF_ERASEBKGND);
			//CallDefaultClassProc(WM_PRINT, reinterpret_cast<WPARAM>(hdc), PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE);

			if (m_bShowLineNumbers)
			{
				setFmtRect();
				DrawGutter(hdc);
			}

			if (!wParam)
				EndPaint(m_Hwnd, &ps);

			return 0L;
		}
		else if (m_bShowLineNumbers)
		{
			bParsed = TRUE;
			const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);

			PostMessage(m_Hwnd, WM_DRAW_NUMBERS, wParam, 0);

			return lRes;
		}
	}
	break;

	case WM_ERASEBKGND:
	{
		//if (isExStyle(WindowExStyle::Transparent))
		//	SetBkMode((HDC)wParam, TRANSPARENT);

		if (m_bShowLineNumbers)
		{
			//if (isExStyle(WindowExStyle::Transparent))
			//{
			//	// must call DrawGutter() directly to avoid flicker.
			//	DrawGutter((HDC)wParam);

			//	bParsed = TRUE;
			//	return TRUE;
			//}
			//else {
			const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);

			// must call DrawGutter() directly to avoid flicker.
			setFmtRect();
			DrawGutter((HDC)wParam);

			bParsed = TRUE;
			return lRes;
			//}
		}
	}
	break;

	case WM_SETCURSOR:
	{
		if ((Dcx::dcxLOWORD(lParam) != HTCLIENT) || (reinterpret_cast<HWND>(wParam) != m_Hwnd) || (!m_bShowLineNumbers))
			break;

		if (POINT pt{}; GetCursorPos(&pt))
		{
			MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
			if (const RECT rc{ getGutterRect() }; PtInRect(&rc, pt))
			{
				if (const HCURSOR hCursor = LoadCursor(nullptr, IDC_ARROW); GetCursor() != hCursor)
					SetCursor(hCursor);

				bParsed = TRUE;
				return TRUE;
			}
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
				DrawGutter((HDC)wParam);
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
		break;
	}

	case WM_LBUTTONUP:
	{
		if (m_bShowLineNumbers)
			PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
	}
	[[fallthrough]];
	default:
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}

LRESULT DcxEdit::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}

Dcx::range_t<DWORD> DcxEdit::GetVisibleRange() noexcept
{
	// find the index of the top visible line

	const auto start_line = gsl::narrow_cast<DWORD>(SNDMSG(m_Hwnd, EM_GETFIRSTVISIBLELINE, 0, 0));

	const RECT rc = getFmtRect();

	// find the index of the last visible line
	const auto char_index = SNDMSG(m_Hwnd, EM_CHARFROMPOS, 0, MAKELPARAM(0, rc.bottom));
	const auto stop_line = gsl::narrow_cast<DWORD>(SNDMSG(m_Hwnd, EM_LINEFROMCHAR, gsl::narrow_cast<LPARAM>(char_index), 0));

	// +1 to make range inclusive
	return { start_line, stop_line + 1 };
}

GSL_SUPPRESS(con.4)
GSL_SUPPRESS(lifetime.1)
DWORD DcxEdit::GetCaretPos() noexcept
{
	DWORD hiPos{}, loPos{};
	SNDMSG(m_Hwnd, EM_GETSEL, reinterpret_cast<LPARAM>(&loPos), reinterpret_cast<LPARAM>(&hiPos));
	if (loPos != hiPos)
		--hiPos;
	return hiPos;

	//// windows 10 only :/
	//return Edit_GetCaretIndex(m_Hwnd);
}

DWORD DcxEdit::GetCaretLine() noexcept
{
	const auto pos = GetCaretPos();
	return gsl::narrow_cast<DWORD>(SNDMSG(m_Hwnd, EM_LINEFROMCHAR, gsl::narrow_cast<LPARAM>(pos), 0));
}

void DcxEdit::setCaretPos(DWORD pos) noexcept
{
	SendMessage(m_Hwnd, EM_SETSEL, pos, pos);
}

void DcxEdit::DrawGutter()
{
	if (HDC hdc = GetDC(m_Hwnd); hdc)
	{
		Auto(ReleaseDC(m_Hwnd, hdc));
		DrawGutter(hdc);
	}
}

void DcxEdit::DrawGutter(HDC hdc)
{
	if (!hdc)
		return;

	RECT rcClient{};
	const RECT rcFmt = getFmtRect();

	GetClientRect(m_Hwnd, &rcClient);

	RECT m_FRGutter = rcClient;
	m_FRGutter.right = std::max(rcFmt.left - 3, 0L);

	// gutter doesnt exist...
	if (m_FRGutter.right == 0)
		return;

	auto hFont = GetWindowFont(m_Hwnd);
	const auto oldFont = Dcx::dcxSelectObject<HFONT>(hdc, hFont);
	Auto(Dcx::dcxSelectObject<HFONT>(hdc, oldFont));

	TEXTMETRICW lptm{};
	GetTextMetrics(hdc, &lptm);
	const auto letter_height = lptm.tmHeight;

	if (auto hdcbuf = CreateHDCBuffer(hdc, &m_FRGutter); hdcbuf)
	{
		Auto(DeleteHDCBuffer(hdcbuf));

		Dcx::FillRectColour(*hdcbuf, &m_FRGutter, m_clrGutter_bkg);

		//if (auto hPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 255)); hPen)
		//{
		//	Auto(DeleteObject(hPen));
		//	const auto oldPen = Dcx::dcxSelectObject<HPEN>(*hdcbuf, hPen);
		//	Auto(Dcx::dcxSelectObject<HPEN>(*hdcbuf, oldPen));
		//	//DrawEdge(*hdcbuf, &m_FRGutter, EDGE_BUMP, BF_RIGHT);
		//	MoveToEx(*hdcbuf, m_FRGutter.right, m_FRGutter.top, nullptr);
		//	LineTo(*hdcbuf, m_FRGutter.right, m_FRGutter.bottom);
		//}
		//else
		//	DrawEdge(*hdcbuf, &m_FRGutter, EDGE_BUMP, BF_RIGHT);

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

