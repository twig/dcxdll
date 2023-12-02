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

namespace Dcx
{
	int dcxEdit_GetEndOfLine(HWND hwnd) noexcept
	{
		//	EC_ENDOFLINE_CRLF	one		The end - of - line character used for new linebreaks is carriage return followed by linefeed(CRLF).
		//	EC_ENDOFLINE_CR		two		The end - of - line character used for new linebreaks is carriage return (CR).
		//	EC_ENDOFLINE_LF		three	The end - of - line character used for new linebreaks is linefeed(LF).
		// When the end-of-line character used is set to EC_ENDOFLINE_DETECTFROMCONTENT using Edit_SetEndOfLine, this message will return the detected end-of-line character.
		if (hwnd && Dcx::DwmModule.isWin10())
			return Edit_GetEndOfLine(hwnd);

		// use CRLF if not win10+
		return EC_ENDOFLINE_CRLF;
	}
	bool dcxEdit_GetZoom(HWND hwnd, int* nNumerator, int* nDenominator) noexcept
	{
		//Supported in Windows 10 1809 and later. The edit control needs to have the ES_EX_ZOOMABLE extended style set, for this message to have an effect
		//(the zoom ratio is always between 1/64 and 64) NOT inclusive, 1.0 = no zoom
		if (!hwnd || !nNumerator || !nDenominator)
			return false;

		if (Dcx::DwmModule.isWin10())
			return Edit_GetZoom(hwnd, nNumerator, nDenominator);

		*nNumerator = 1;
		*nDenominator = 0;
		return true;
	}
	TString dcxEdit_GetEndOfLineCharacters(HWND hwnd)
	{
		switch (Dcx::dcxEdit_GetEndOfLine(hwnd))
		{
		case EC_ENDOFLINE_LF:
			return TEXT("\n");
		case EC_ENDOFLINE_CR:
			return TEXT("\r");
		case EC_ENDOFLINE_CRLF:
		default:
			break;
		}
		return TEXT("\r\n");
	}
	DWORD dcxEdit_GetCaretIndex(HWND hwnd) noexcept
	{
		if (!hwnd)
			return 0;

		// windows 10 only
		if (Dcx::DwmModule.isWin10())
			return Edit_GetCaretIndex(hwnd);

		DWORD hiPos{}, loPos{};
		//Edit_GetSel(hwnd); // not usefull here.
		SNDMSG(hwnd, EM_GETSEL, reinterpret_cast<LPARAM>(&loPos), reinterpret_cast<LPARAM>(&hiPos));
		if (loPos != hiPos)
			--hiPos;
		return hiPos;
	}
	void dcxEdit_SetCaretIndex(HWND hwnd, DWORD nPos) noexcept
	{
		if (!hwnd)
			return;

		// windows 10 only
		if (Dcx::DwmModule.isWin10())
			Edit_SetCaretIndex(hwnd, nPos);
		else
			Edit_SetSel(hwnd, nPos, nPos);
	}
	DWORD dcxEdit_CharFromPos(HWND hwnd, const LONG& iPos) noexcept
	{
		if (!hwnd)
			return 0;

		constexpr LONG iZero{};
		return gsl::narrow_cast<DWORD>(SNDMSG(hwnd, EM_CHARFROMPOS, 0, Dcx::dcxMAKELPARAM(iZero, iPos)));
	}
	DWORD dcxEdit_LineFromChar(HWND hwnd, const LONG& ich) noexcept
	{
		if (!hwnd)
			return 0;
		return gsl::narrow_cast<DWORD>(Edit_LineFromChar(hwnd, ich));
	}
	void dcxEdit_GetSel(HWND hwnd, _Maybenull_ DWORD* nStart, _Maybenull_ DWORD* nEnd) noexcept
	{
		if (!hwnd)
			return;

		SendMessage(hwnd, EM_GETSEL, reinterpret_cast<WPARAM>(nStart), reinterpret_cast<LPARAM>(nEnd));
	}
}

/*!
* \brief Constructor
*
* \param ID Control ID
* \param p_Dialog Parent DcxDialog Object
* \param mParentHwnd Parent Window Handle
* \param rc Window Rectangle
* \param styles Window Style Tokenized List
*/

DcxEdit::DcxEdit(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
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

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	Edit_LimitText(m_Hwnd, 0);

	//SendMessage(m_Hwnd, CCM_SETUNICODEFORMAT, TRUE, 0);

	if (styles.istok(TEXT("tooltips")))
	{
		if (!IsWindow(p_Dialog->getToolTipHWND()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		setToolTipHWND(p_Dialog->getToolTipHWND());
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
	if (m_bLockGutter)
		styles.addtok(TEXT("unlockgutter"));
	return styles;
}

void DcxEdit::toXml(TiXmlElement* const xml) const
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

TiXmlElement* DcxEdit::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxEdit::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	if (const auto tmp = gsl::narrow_cast<TCHAR>(queryIntAttribute(xThis, "passchar")); tmp)
	{
		Edit_SetPasswordChar(m_Hwnd, tmp);
		this->m_PassChar = tmp;
	}

	m_bCueFocused = (queryIntAttribute(xThis, "cuefocused") > 0);

	if (const auto tmp = queryAttribute(xThis, "cue"); tmp)
	{
		this->m_tsCue = tmp;
		Edit_SetCueBannerTextFocused(m_Hwnd, this->m_tsCue.to_chr(), m_bCueFocused);
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
		Edit_SetModify(m_Hwnd, FALSE);
	}
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
		case L"unlockgutter"_hash:
			m_bLockGutter = false;
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
				const auto sepChars(Dcx::dcxEdit_GetEndOfLineCharacters(m_Hwnd));
				if (const auto nLine = input.getnexttok().to_int(); (nLine > 0 && nLine <= gsl::narrow_cast<int>(m_tsText.numtok(sepChars.to_chr()))))
					szReturnValue = m_tsText.gettok(nLine, sepChars.to_chr()).to_chr();
			}
			// Ook: should we return the entire line when no line number is given?
		}
		else
			szReturnValue = m_tsText.to_chr();

		// Ook: should use this instead?
		//szReturnValue = getLine(input.getnexttok().to_int()).to_chr();
	}
	break;

	// [NAME] [ID] [PROP]
	case L"num"_hash:
	{
		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			const auto i = this->m_tsText.numtok(Dcx::dcxEdit_GetEndOfLineCharacters(m_Hwnd).to_chr());
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
	case L"ismodified"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), Edit_GetModify(m_Hwnd));
		break;

		// [NAME] [ID] [PROP]
	case L"isreadonly"_hash:
		szReturnValue = dcx_truefalse(isStyle(WindowStyle::ES_ReadOnly));
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
			const auto CharPos = (dwAbsoluteStartSelPos - gsl::narrow_cast<int>(SendMessage(m_Hwnd, EM_LINEINDEX, gsl::narrow_cast<WPARAM>(-1), 0)));

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

		Dcx::dcxEdit_GetSel(m_Hwnd, &dwSelStart, nullptr);

		_ts_snprintf(szReturnValue, TEXT("%u"), dwSelStart);
	}
	break;

	case L"selend"_hash:
	{
		DWORD dwSelEnd{};   // selection range ending position

		Dcx::dcxEdit_GetSel(m_Hwnd, nullptr, &dwSelEnd);

		_ts_snprintf(szReturnValue, TEXT("%u"), dwSelEnd);
	}
	break;

	case L"sel"_hash:
	{
		DWORD dwSelStart{}; // selection range starting position
		DWORD dwSelEnd{};   // selection range ending position

		Dcx::dcxEdit_GetSel(m_Hwnd, &dwSelStart, &dwSelEnd);

		_ts_snprintf(szReturnValue, TEXT("%u %u"), dwSelStart, dwSelEnd);
	}
	break;

	case L"seltext"_hash:
	{
		DWORD dwSelStart{}; // selection range starting position
		DWORD dwSelEnd{};   // selection range ending position

		Dcx::dcxEdit_GetSel(m_Hwnd, &dwSelStart, &dwSelEnd);

		const auto tmp = (dwSelEnd - dwSelStart);
		szReturnValue = m_tsText.mid(gsl::narrow_cast<int>(dwSelStart), gsl::narrow_cast<int>(tmp)).to_chr();
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

	case L"endofline"_hash:
	{
		//	EC_ENDOFLINE_CRLF	one		The end - of - line character used for new linebreaks is carriage return followed by linefeed(CRLF).
		//	EC_ENDOFLINE_CR		two		The end - of - line character used for new linebreaks is carriage return (CR).
		//	EC_ENDOFLINE_LF		three	The end - of - line character used for new linebreaks is linefeed(LF).
		// When the end-of-line character used is set to EC_ENDOFLINE_DETECTFROMCONTENT using Edit_SetEndOfLine, this message will return the detected end-of-line character.
		_ts_snprintf(szReturnValue, TEXT("%d"), Dcx::dcxEdit_GetEndOfLine(m_Hwnd));
	}
	break;

	case L"zoom"_hash:
	{
		//Supported in Windows 10 1809 and later. The edit control needs to have the ES_EX_ZOOMABLE extended style set, for this message to have an effect
		//(the zoom ratio is always between 1/64 and 64) NOT inclusive, 1.0 = no zoom
		if (int nNumerator{}, nDenominator{}; Dcx::dcxEdit_GetZoom(m_Hwnd, &nNumerator, &nDenominator))
			_ts_snprintf(szReturnValue, TEXT("%d %d"), nNumerator, nDenominator);
		else
			szReturnValue = TEXT("0");
	}
	break;

	case L"passchar"_hash:
	{
		_ts_snprintf(szReturnValue, TEXT("%u"), Edit_GetPasswordChar(m_Hwnd));
	}
	break;

	case L"len"_hash:
	{
		if (this->isStyle(WindowStyle::ES_MultiLine) && (input.numtok() == 4))
		{
			const auto nLine = input.getnexttokas<UINT>();
			const auto sepChars(Dcx::dcxEdit_GetEndOfLineCharacters(m_Hwnd));

			if (nLine > m_tsText.numtok(sepChars.to_chr()))
				throw DcxExceptions::dcxInvalidArguments();

			const auto nLen = m_tsText.gettok(nLine, sepChars.to_chr()).len();

			_ts_snprintf(szReturnValue, TEXT("%u"), nLen);
			return;
		}
		_ts_snprintf(szReturnValue, TEXT("%u"), m_tsText.len());
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
		Edit_SetModify(m_Hwnd, FALSE);
	}

	// xdid -a [NAME] [ID] [SWITCH] [TEXT]
	if (flags[TEXT('a')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto pos = this->GetCaretPos();

		this->m_tsText += input.getlasttoks();	// tok 4, -1
		SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());
		Edit_SetModify(m_Hwnd, FALSE);

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
			const auto sepChars = Dcx::dcxEdit_GetEndOfLineCharacters(m_Hwnd);

			const auto itEnd = tsLines.end();
			for (auto itStart = tsLines.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
			{
				const TString tsLineRange(*itStart);

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

				const auto r = Dcx::make_range(tsLineRange, this->m_tsText.numtok(sepChars.to_chr()));

				// delete lines from the back of the text so it doesnt change the position of other lines.
				for (auto nLine = r.e; nLine >= r.b; --nLine)
					this->m_tsText.deltok(nLine, sepChars.to_chr());
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

		const auto nLine = input.getnexttok().to_<UINT>();	// tok 4

		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			if (nLine == 0)
				throw DcxExceptions::dcxInvalidArguments();

			this->m_tsText.instok(input.getlasttoks(), nLine, Dcx::dcxEdit_GetEndOfLineCharacters(m_Hwnd).to_chr());	// tok 5, -1
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
			if (Dcx::DwmModule.isVista())
			{
				if (Dcx::DwmModule.isWin7())
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

		Dcx::dcxEdit_SetReadOnly(m_Hwnd, enabled);
	}
	// xdid -o [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('o')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nLine = input.getnexttok().to_<UINT>();	// tok 4

		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			if (nLine == 0)
				throw DcxExceptions::dcxInvalidArguments();

			this->m_tsText.puttok(input.getlasttoks(), nLine, Dcx::dcxEdit_GetEndOfLineCharacters(m_Hwnd).to_chr());	// tok 5, -1
		}
		else
			this->m_tsText = input.getlasttoks();	// tok 4, -1
		SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());
	}
	// xdid -P [NAME] [ID]
	else if (flags[TEXT('P')])
	{
		SendMessage(this->getHwnd(), WM_PASTE, 0, 0);
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
		Edit_SetModify(m_Hwnd, FALSE);
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
		Dcx::dcxEdit_ScrollCaret(m_Hwnd);
	}
	// xdid -S [NAME] [ID] [SWITCH] [START] (END)
	else if (flags[TEXT('S')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto istart = input.getnexttok().to_int();	// tok 4
		const auto iend = (numtok > 4) ? input.getnexttok().to_int() : istart;

		SendMessage(m_Hwnd, EM_SETSEL, gsl::narrow_cast<WPARAM>(istart), gsl::narrow_cast<LPARAM>(iend));
		Dcx::dcxEdit_ScrollCaret(m_Hwnd);
	}
	// xdid -E [NAME] [ID] [SWITCH] [CUE TEXT]
	// xdid -E [NAME] [ID] [SWITCH] [+FLAGS] [CUE TEXT]
	// xdid -E [NAME] [ID] [SWITCH] + [CUE TEXT]
	// xdid -E [NAME] [ID] [SWITCH] +f [CUE TEXT]
	else if (flags[TEXT('E')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsFlags(input.gettok(4));
		if (tsFlags[0] == TEXT('+'))
		{
			const XSwitchFlags xFlags(tsFlags);
			this->m_tsCue = input.gettok(5, -1);	// tok 5, -1
			m_bCueFocused = xFlags[TEXT('f')];
			if (m_bCueFocused)
				Edit_SetCueBannerTextFocused(m_Hwnd, m_tsCue.to_wchr(), TRUE);
			else
				Edit_SetCueBannerText(m_Hwnd, this->m_tsCue.to_wchr());
		}
		else {
			m_bCueFocused = false;
			this->m_tsCue = input.getlasttoks();	// tok 4, -1
			Edit_SetCueBannerText(m_Hwnd, this->m_tsCue.to_wchr());
		}
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
				if (m_bShowLineNumbers)
					setFmtRect();
				else
					resetFmtRect();

				InvalidateRect(m_Hwnd, nullptr, TRUE);
			}
		}
	}
	// xdid -g [NAME] [ID] [SWITCH] [Selected line Background Colour|-] (Background Colour|-) (Selected Line Text Colour|-) (Text Colour|-) (Border Colour|-) (Unlock Gutter 0|1|-) (Gutter Size|-) (Gutter Border Size|-)
	else if (flags[TEXT('g')])
	{
		static_assert(CheckFreeCommand(TEXT('g')), "Command in use!");

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		//auto tsClr(input.getnexttok());
		//if (tsClr != TEXT('-'))
		//	this->m_clrGutter_selbkg = tsClr.to_<COLORREF>();
		//
		//if (numtok > 4)
		//{
		//	tsClr = input.getnexttok();
		//	if (tsClr != TEXT('-'))
		//		this->m_clrGutter_bkg = tsClr.to_<COLORREF>();
		//
		//	if (numtok > 5)
		//	{
		//		tsClr = input.getnexttok();
		//		if (tsClr != TEXT('-'))
		//			this->m_clrGutter_seltxt = tsClr.to_<COLORREF>();
		//
		//		if (numtok > 6)
		//		{
		//			tsClr = input.getnexttok();
		//			if (tsClr != TEXT('-'))
		//				this->m_clrGutter_txt = tsClr.to_<COLORREF>();
		//
		//			if (numtok > 7)
		//			{
		//				tsClr = input.getnexttok();
		//				if (tsClr != TEXT('-'))
		//					this->m_clrGutter_border = tsClr.to_<COLORREF>();
		//			}
		//		}
		//	}
		//}

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
			if (dcx_testflag(getEventMask(), DCX_EVENT_EDIT))
				this->execAliasEx(TEXT("edit,%u"), getUserID());
			if (m_bShowLineNumbers)
				PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);

			//if (!m_bIgnoreUndo)
			//{
			//	if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
			//	{
			//		//this->execAliasEx(TEXT("edit,%u"), getUserID());
			//		TCHAR szRet[256]{};
			//
			//		evalAliasEx(&szRet[0], std::size(szRet), TEXT("edit,%u"), getUserID());
			//
			//		if (_ts_strcmp(&szRet[0], TEXT("nochange")) != 0)
			//			this->m_tsText = TGetWindowText(m_Hwnd);
			//		else {
			//			m_bIgnoreUndo = true;
			//			const auto pos = this->GetCaretPos();
			//			SetWindowText(m_Hwnd, this->m_tsText.to_chr());
			//			this->setCaretPos(pos);
			//			m_bIgnoreUndo = false;
			//		}
			//	}
			//	else
			//		this->m_tsText = TGetWindowText(m_Hwnd);
			//}
			//if (m_bShowLineNumbers)
			//	PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
		}
		break;
		//case EN_VSCROLL:
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

		if (dcx_testflag(getEventMask(), DCX_EVENT_EDIT))
		{
			if (wParam == VK_RETURN)
				this->execAliasEx(TEXT("return,%u"), getUserID());

			if (!((this->m_bIgnoreRepeat) && (lParam & 0x40000000))) // ignore repeats
				this->execAliasEx(TEXT("keydown,%u,%u"), getUserID(), wParam);

			//if (!((this->m_bIgnoreRepeat) && (lParam & 0x40000000))) // ignore repeats
			//{
			//	TCHAR szRet[256]{};
			//	evalAliasEx(&szRet[0], std::size(szRet), TEXT("keydown,%u,%u"), getUserID(), wParam);
			//	if (_ts_strcmp(&szRet[0], TEXT("nochange")) == 0)
			//		bParsed = TRUE;
			//}
		}
	}
	break;

	case WM_COPY:
	{
		if (dcx_testflag(getEventMask(), DCX_EVENT_EDIT))
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
		if (dcx_testflag(getEventMask(), DCX_EVENT_EDIT))
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
		if (dcx_testflag(getEventMask(), DCX_EVENT_EDIT))
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
		if (dcx_testflag(getEventMask(), DCX_EVENT_EDIT))
		{
			//execAliasEx(TEXT("keyup,%u,%u"), getUserID(), wParam);

			if (!m_bIgnoreUndo)
			{
				TCHAR szRet[256]{};

				evalAliasEx(&szRet[0], std::size(szRet), TEXT("keyup,%u,%u"), getUserID(), wParam);

				if (_ts_strcmp(&szRet[0], TEXT("nochange")) == 0)
				{
					m_bIgnoreUndo = true;
					SendMessage(m_Hwnd, EM_UNDO, 0, 0);
					SendMessage(m_Hwnd, EM_EMPTYUNDOBUFFER, 0, 0);
					m_bIgnoreUndo = false;
					bParsed = TRUE;
					return 0L;
				}
			}
		}
		if (m_bShowLineNumbers)
			PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);
	}
	break;

	case WM_PRINTCLIENT:
	{
		//const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);
		//if (m_bShowLineNumbers)
		//	DrawGutter(reinterpret_cast<HDC>(wParam));
		//bParsed = TRUE;
		//return lRes;

		bParsed = TRUE;
		DrawClientRect(reinterpret_cast<HDC>(wParam), uMsg, lParam);
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

		//if (this->IsAlphaBlend())
		//{
		//	bParsed = TRUE;
		//
		//	PAINTSTRUCT ps{};
		//	//auto hdc = BeginPaint(m_Hwnd, &ps);
		//	//Auto(EndPaint(m_Hwnd, &ps));
		//
		//	HDC hdc = reinterpret_cast<HDC>(wParam);
		//	if (!wParam)
		//		hdc = BeginPaint(m_Hwnd, &ps);
		//
		//	// Setup alpha blend if any.
		//	const auto ai = this->SetupAlphaBlend(&hdc);
		//	Auto(this->FinishAlphaBlend(ai));
		//
		//	CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
		//	//CallDefaultClassProc(WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdc), PRF_CLIENT | PRF_CHILDREN /*| PRF_CHECKVISIBLE*/ | PRF_ERASEBKGND);
		//	//CallDefaultClassProc(WM_PRINT, reinterpret_cast<WPARAM>(hdc), PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE);
		//
		//	if (m_bShowLineNumbers)
		//	{
		//		setFmtRect();
		//		DrawGutter(hdc);
		//
		//	if (!wParam)
		//		EndPaint(m_Hwnd, &ps);
		//
		//	return 0L;
		//}
		//else if (m_bShowLineNumbers)
		//{
		//	bParsed = TRUE;
		//	const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);
		//	PostMessage(m_Hwnd, WM_DRAW_NUMBERS, wParam, 0);
		//	return lRes;
		//}

		bParsed = TRUE;

		if (!wParam)
		{
			PAINTSTRUCT ps{};

			HDC hdc = BeginPaint(m_Hwnd, &ps);
			Auto(EndPaint(m_Hwnd, &ps));

			DrawClientRect(hdc, uMsg, lParam);
		}
		else
			DrawClientRect(reinterpret_cast<HDC>(wParam), uMsg, lParam);

		return 0L;
	}
	break;

	case WM_ERASEBKGND:
	{
		//if (isExStyle(WindowExStyle::Transparent))
		//	SetBkMode(reinterpret_cast<HDC>(wParam), TRANSPARENT);

		if (m_bShowLineNumbers)
		{
			//if (isExStyle(WindowExStyle::Transparent))
			//{
			//	// must call DrawGutter() directly to avoid flicker.
			//	DrawGutter(reinterpret_cast<HDC>(wParam));
			//
			//	bParsed = TRUE;
			//	return TRUE;
			//}
			//else {
			const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);

			// must call DrawGutter() directly to avoid flicker.
			setFmtRect();
			DrawGutter(reinterpret_cast<HDC>(wParam));

			bParsed = TRUE;
			return lRes;
			//}
		}
		//else {
		//	const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);
		//
		//	HDC hdc = reinterpret_cast<HDC>(wParam);
		//
		//	const RECT rcBkg = getFmtRect();
		//	if (isExStyle(WindowExStyle::Transparent))
		//	{ // fill with parent image
		//
		//		if (!IsAlphaBlend())
		//			DrawParentsBackground(hdc, &rcBkg);
		//
		//		//SetBkMode(hdc, TRANSPARENT);
		//	}
		//	else // normal bkg
		//		DcxControl::DrawCtrlBackground(hdc, this, &rcBkg);
		//
		//	bParsed = TRUE;
		//	return lRes;
		//}
	}
	break;

	case WM_SETCURSOR:
	{
		if ((Dcx::dcxLOWORD(lParam) != HTCLIENT) || (reinterpret_cast<HWND>(wParam) != m_Hwnd) || (!m_bShowLineNumbers))
			break;

		if (IsCursorOnGutter())
		{
			if ((!m_bLockGutter) && IsCursorOnGutterBorder())
			{
				if (auto hCursor = LoadCursor(nullptr, IDC_SIZEWE); GetCursor() != hCursor)
					SetCursor(hCursor);
			}
			else if (const auto hCursor = LoadCursor(nullptr, IDC_ARROW); GetCursor() != hCursor)
				SetCursor(hCursor);

			bParsed = TRUE;
			return TRUE;
		}
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
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
				// Ook: should we reset to previous width before the drag started?
				// outside ctrl, reset to default width.
				m_GutterWidth = DCX_EDIT_GUTTER_WIDTH;
			}
			setFmtRect();
			ReleaseCapture();
			m_bDraggingGutter = false;

			bParsed = TRUE;
			return 0L;
		}

		if (m_bShowLineNumbers)
			PostMessage(m_Hwnd, WM_DRAW_NUMBERS, 0, 0);

		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
	}
	break;

	case WM_MOUSEMOVE:
	{
		if (m_bDraggingGutter)
		{
			const POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

			m_GutterWidth = std::max<long>(DCX_EDIT_GUTTER_WIDTH, pt.x);

			setFmtRect(false);

			if (auto hdc = GetDC(m_Hwnd); hdc)
			{
				Auto(ReleaseDC(m_Hwnd, hdc));
				DrawGutter(hdc);
				SendMessage(m_Hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdc), PRF_CLIENT/*|PRF_ERASEBKGND*/);
			}

			//this->redrawBufferedWindowClient();

			//if (auto hdc = GetDC(m_Hwnd); hdc)
			//{
			//	Auto(ReleaseDC(m_Hwnd, hdc));
			//	RECT rcClient{};
			//	GetClientRect(m_Hwnd, &rcClient);
			//	if (auto hdcbuf = CreateHDCBuffer(hdc, &rcClient); hdcbuf)
			//	{
			//		Auto(DeleteHDCBuffer(hdcbuf));
			//		DrawGutter(*hdcbuf);
			//		SendMessage(m_Hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(*hdcbuf), PRF_CLIENT|PRF_ERASEBKGND);
			//		BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, *hdcbuf, 0, 0, SRCCOPY);
			//	}
			//}

			bParsed = TRUE;
			return 0L;
		}
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
	}
	break;

	case WM_DESTROY:
	{
		delete this;
		bParsed = TRUE;
		break;
	}

	case WM_MOUSEHWHEEL:
	case WM_MOUSEWHEEL:
		[[fallthrough]];
	case WM_VSCROLL:
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

TString DcxEdit::getLine(int nLine) const
{
	//TCHAR szBuf[MIRC_BUFFER_SIZE_CCH]{};
	//Edit_GetLine(m_Hwnd, nLine, &szBuf[0], (MIRC_BUFFER_SIZE_CCH - 1));
	//return szBuf;

	GSL_SUPPRESS(lifetime.1) return m_tsText.gettok(nLine, Dcx::dcxEdit_GetEndOfLineCharacters(m_Hwnd).to_chr());
}

Dcx::range_t<DWORD> DcxEdit::GetVisibleRange() const noexcept
{
	// find the index of the top visible line
	const auto start_line = Dcx::dcxEdit_GetFirstVisibleLine(m_Hwnd);

	const RECT rc = getFmtRect();

	// find the index of the last visible line
	const auto char_index = Dcx::dcxEdit_CharFromPos(m_Hwnd, rc.bottom);
	const auto stop_line = Dcx::dcxEdit_LineFromChar(m_Hwnd, char_index);

	// +1 to make range inclusive
	return { start_line, stop_line + 1 };
}

GSL_SUPPRESS(con.4)
GSL_SUPPRESS(lifetime.1)
DWORD DcxEdit::GetCaretPos() const noexcept
{
	return Dcx::dcxEdit_GetCaretIndex(m_Hwnd);
}

DWORD DcxEdit::GetCaretLine() const noexcept
{
	const auto pos = GetCaretPos();
	return Dcx::dcxEdit_LineFromChar(m_Hwnd, pos);
}

void DcxEdit::setCaretPos(DWORD pos) noexcept
{
	Dcx::dcxEdit_SetCaretIndex(m_Hwnd, pos);
}

//const DcxSearchTypes DcxEdit::CharToSearchType(const TCHAR& cType) const noexcept
//{
//	switch (cType)
//	{
//	case TEXT('R'):
//	case TEXT('r'):
//		return DcxSearchTypes::SEARCH_R;
//	case TEXT('W'):
//	case TEXT('w'):
//		return DcxSearchTypes::SEARCH_W;
//	default:
//		return DcxSearchTypes::SEARCH_E;
//	}
//}

//std::optional<DcxEdit::Edit_SearchResults> DcxEdit::matchText(const UINT nLine, const UINT nSubChar, const TString& search, const DcxSearchTypes& SearchType) const
//{
//	if (nLine == 0)
//		return { };
//
//	// [LINE] & [SUBCHAR] supplied.
//	// adjust nChar to the correct pos.
//
//	// is line number to high?
//	if (nLine > m_tsText.numtok(TEXT("\r\n")))
//		return { };
//
//	// find line character offset.
//	const auto LineOffset = m_tsText.find(TEXT("\r\n"), nLine);
//
//	// couldnt find line?
//	if (LineOffset == -1)
//		return { };
//
//	const UINT nChar = LineOffset + nSubChar;
//
//	// is char outside of range allowed?
//	if (nChar >= m_tsText.len())
//		return { };
//
//	return matchText(nChar, search, SearchType);
//}

//std::optional<DcxEdit::Edit_SearchResults> DcxEdit::matchText(const UINT nChar, const TString& search, const DcxSearchTypes& SearchType) const
//{
//	const dcxSearchData srch_data(search, SearchType);
//
//	return matchText(nChar, srch_data);
//}

//std::optional<DcxEdit::Edit_SearchResults> DcxEdit::matchText(const UINT nChar, const dcxSearchData& srch_data) const
//{
//	//if (nChar == 0) // nChar must be >= 1
//	//	return { Edit_SearchResults{ false, 0, 0 } };
//
//	if (nChar >= m_tsText.len())
//		return { };
//
//	const auto szStart = &m_tsText.to_chr()[nChar];
//
//	switch (srch_data.m_SearchType)
//	{
//	case DcxSearchTypes::SEARCH_R:	// regex match
//	{
//		if (const auto m = getRegexMatchOffset(szStart, srch_data); m.has_value())
//		{
//			const auto nStart = m->nStart + nChar;
//			const auto nEnd = m->nEnd + nChar;
//
//			return { Edit_SearchResults{ true, nStart, nEnd } };
//		}
//	}
//	break;
//	case DcxSearchTypes::SEARCH_W:	// wildcard match
//	{
//		if (const auto bFound = _ts_WildcardMatch(szStart, srch_data.m_tsSearch); bFound)
//		{
//			const auto nStart = gsl::narrow_cast<size_t>(szStart - m_tsText.to_chr());
//			const auto nEnd = nStart + m_tsText.len();
//
//			return { Edit_SearchResults{ true, nStart, nEnd } };
//		}
//	}
//	break;
//	case DcxSearchTypes::SEARCH_E:   // exact match
//	{
//		if (const auto szFound = _ts_find(szStart, srch_data.m_tsSearch.to_chr()); szFound)
//		{
//			const auto nStart = gsl::narrow_cast<UINT>((szFound - m_tsText.to_chr()));
//			const auto nEnd = nStart + srch_data.m_tsSearch.len();
//
//			return { Edit_SearchResults{ true, nStart, nEnd } };
//		}
//	}
//	break;
//	default:
//		break;
//	}
//	return { };
//}

void DcxEdit::DrawClientRect(HDC hdc, unsigned int uMsg, LPARAM lParam)
{

	//// Setup alpha blend if any.
	//const auto ai = this->SetupAlphaBlend(&hdc);
	//Auto(this->FinishAlphaBlend(ai));
	//
	//CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
	//
	//if (m_bShowLineNumbers)
	//{
	//	setFmtRect();
	//	DrawGutter(hdc);
	//}

	// Setup alpha blend if any.
	const auto ai = SetupAlphaBlend(&hdc);
	Auto(FinishAlphaBlend(ai));

	if (!hdc)
		return;

	// fill background.
	//const RECT rcBkg = getFmtRect();
	//if (isExStyle(WindowExStyle::Transparent))
	//{ // fill with parent image
	//	if (!IsAlphaBlend())
	//		DrawParentsBackground(hdc, &rcBkg);
	//}
	//else // normal bkg
	//	DcxControl::DrawCtrlBackground(hdc, this, &rcBkg);

	//CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
	//CallDefaultClassProc(WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdc), PRF_CLIENT | PRF_CHILDREN | PRF_ERASEBKGND);

	if (m_bShowLineNumbers)
	{
		CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
		setFmtRect();
		DrawGutter(hdc);
	}
	else {
		// produces strange ghosted text from other edit control. Only affects transparent single line edit controls?
		//CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);

		//const auto savedc = SaveDC(hdc);
		//Auto(RestoreDC(hdc, savedc));
		//
		//RECT rcBkg = getFmtRect();
		//if ((m_pParentHWND) && (m_Hwnd))
		//	if (auto hBrush = reinterpret_cast<HBRUSH>(SendMessage(m_pParentHWND, WM_CTLCOLOREDIT, reinterpret_cast<WPARAM>(hdc), reinterpret_cast<LPARAM>(m_Hwnd))); hBrush)
		//		FillRect(hdc, &rcBkg, hBrush);
		//
		//if (isExStyle(WindowExStyle::Transparent))
		//{ // fill with parent image
		//	if (!IsAlphaBlend())
		//		DrawParentsBackground(hdc, &rcBkg);
		//}
		//else // normal bkg
		//	DcxControl::DrawCtrlBackground(hdc, this, &rcBkg);
		//
		//
		//SetTextColor(hdc,this->getTextColor());
		//Dcx::dcxSelectObject(hdc, this->getFont());
		//
		//ctrlDrawText(hdc, m_tsText, &rcBkg, DT_SINGLELINE | DT_VCENTER);

		if (isExStyle(WindowExStyle::Transparent))
		{
			RECT rcBkg{ getFmtRect() };

			if (!IsAlphaBlend())
				DrawParentsBackground(hdc, &rcBkg);

			if (auto hdcbuf = CreateHDCBuffer(hdc, &rcBkg); hdcbuf)
			{
				Auto(DeleteHDCBuffer(hdcbuf));

				// this method still produces the strange ghosting on text
				//CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(*hdcbuf), lParam);
				//TransparentBlt(hdc, 0, 0, (rcBkg.right - rcBkg.left), (rcBkg.bottom - rcBkg.top), *hdcbuf, 0, 0, (rcBkg.right - rcBkg.left), (rcBkg.bottom - rcBkg.top), GetPixel(*hdcbuf, 0, 0));

				// fixes ghost text issue.
				removeExStyle(WindowExStyle::Transparent);
				CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(*hdcbuf), lParam);
				addExStyle(WindowExStyle::Transparent);
				TransparentBlt(hdc, 0, 0, (rcBkg.right - rcBkg.left), (rcBkg.bottom - rcBkg.top), *hdcbuf, 0, 0, (rcBkg.right - rcBkg.left), (rcBkg.bottom - rcBkg.top), GetPixel(*hdcbuf, 0, 0));

				// fails to show any text
				//BeginPath(*hdcbuf);
				//CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(*hdcbuf), lParam);
				//EndPath(*hdcbuf);
				//
				//if (auto hRgn = PathToRegion(*hdcbuf); hRgn)
				//{
				//	const auto clr = getTextColor();
				//	auto hBrush = CreateSolidBrush(clr);
				//	FillRgn(hdc, hRgn, hBrush);
				//	DeleteBrush(hBrush);
				//	DeleteRgn(hRgn);
				//}
			}
		}
		else CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
	}

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

		m_FRGutter.top += 3;

		dcxDrawEdge(*hdcbuf, &m_FRGutter, m_clrGutter_border);

		m_FRGutter.top -= 3;

		RECT RNumber = m_FRGutter;
		InflateRect(&RNumber, -5, 0);

		// get visible lines
		const auto rng = GetVisibleRange();
		// get current caret pos
		const auto pos = GetCaretLine();

		TCHAR buf[49]{};

		{
			// top line, could be a partial line
			const POINTL pl = GetPosFromChar(GetLineIndex(rng.b));

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

