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

DcxEdit::DcxEdit(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString &styles)
	: DcxControl(ID, p_Dialog)
{
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
		throw Dcx::dcxException("Unable To Create Window");

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

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
	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	DragAcceptFiles(m_Hwnd, TRUE);
}

/*!
* \brief blah
*
* blah
*/
DcxEdit::~DcxEdit()
{
}


const TString DcxEdit::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);

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
	return styles;
}

void DcxEdit::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("caption", m_tsText.c_str());
	xml->SetAttribute("styles", getStyles().c_str());
}

TiXmlElement * DcxEdit::toXml(void) const
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

dcxWindowStyles DcxEdit::parseControlStyles(const TString & tsStyles)
{
	dcxWindowStyles ws;

	for (const auto &tsStyle : tsStyles)
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
		default:
			break;
		}
	}

	return parseGeneralControlStyles(tsStyles, ws);
}

/*!
* \brief $xdid Parsing Function
*
* \param input [NAME] [ID] [PROP] (OPTIONS)
* \param szReturnValue mIRC Data Container
*
* \return > void
*/
void DcxEdit::parseInfoRequest(const TString &input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
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
		DWORD dwAbsoluteStartSelPos = 0;

		// caret startsel position
		SendMessage(m_Hwnd, EM_GETSEL, (WPARAM)&dwAbsoluteStartSelPos, NULL);

		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			// current line
			const auto iLinePos = SendMessage(m_Hwnd, EM_LINEFROMCHAR, gsl::narrow_cast<WPARAM>(-1), NULL) + 1;
			// line offset
			const auto CharPos = (dwAbsoluteStartSelPos - gsl::narrow_cast<int>(SendMessage(m_Hwnd, EM_LINEINDEX, gsl::narrow_cast<WPARAM>(-1), NULL)));

			_ts_snprintf(szReturnValue, TEXT("%d %u"), iLinePos, CharPos);
		}
		else {
			// return selstart
			_ts_snprintf(szReturnValue, TEXT("1 %u"), dwAbsoluteStartSelPos);
		}
	}
	break;
	case L"selstart"_hash:
	{
		DWORD dwSelStart = 0; // selection range starting position

		SendMessage(m_Hwnd, EM_GETSEL, (WPARAM)&dwSelStart, NULL);
		_ts_snprintf(szReturnValue, TEXT("%u"), dwSelStart);
	}
	break;
	case L"selend"_hash:
	{
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(m_Hwnd, EM_GETSEL, NULL, (LPARAM)&dwSelEnd);
		_ts_snprintf(szReturnValue, TEXT("%u"), dwSelEnd);
	}
	break;
	case L"sel"_hash:
	{
		DWORD dwSelStart = 0; // selection range starting position
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(m_Hwnd, EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd);
		_ts_snprintf(szReturnValue, TEXT("%u %u"), dwSelStart, dwSelEnd);
	}
	break;
	case L"seltext"_hash:
	{
		DWORD dwSelStart = 0; // selection range starting position
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(m_Hwnd, EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd);
		szReturnValue = m_tsText.mid(gsl::narrow_cast<int>(dwSelStart), gsl::narrow_cast<int>(dwSelEnd - dwSelStart)).to_chr();
	}
	break;
	case L"cue"_hash:
	{
		if (!this->m_tsCue.empty())
			szReturnValue = m_tsCue.to_chr();
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
void DcxEdit::parseCommandRequest(const TString &input)
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
			throw Dcx::dcxException("Insufficient parameters");

		this->m_tsText += input.getlasttoks();	// tok 4, -1
		SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());
	}
	// xdid -c [NAME] [ID] [SWITCH]
	else if (flags[TEXT('c')])
	{
		if (numtok < 3)
			throw Dcx::dcxException("Insufficient parameters");

		CopyToClipboard(m_Hwnd, this->m_tsText);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			const auto nLine = input.getnexttok().to_<UINT>();	// tok 4
			this->m_tsText.deltok(nLine, TEXT("\r\n"));
			SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());
		}
	}
	// xdid -i [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('i')])
	{
		if (numtok < 5)
			throw Dcx::dcxException("Insufficient parameters");

		if (this->isStyle(WindowStyle::ES_MultiLine))
		{
			const auto nLine = input.getnexttok().to_<UINT>();	// tok 4
			this->m_tsText.instok(input.getlasttoks(), nLine, TEXT("\r\n"));	// tok 5, -1
		}
		else
			this->m_tsText = input.getlasttoks();	// tok 5, -1
		SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());
	}
	// xdid -j [NAME] [ID] [SWITCH] [0|1]
	else if (flags[TEXT('j')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const auto i = input.getnexttok().to_<UINT>();	// tok 4

		auto c = Edit_GetPasswordChar(m_Hwnd);
		if (c == 0)
			c = this->m_PassChar;
		// XP actually uses the unicode `Black Circle` char U+25CF (9679)
		// The problem is getting the char set to a unicode (2-byte) one, so far it always sets to CF (207)
		if (c == 0)
		{
			if (Dcx::VistaModule.isVista())
			{
				if (Dcx::VistaModule.isWin7())
				{
					c = TEXT('\u25CF');	// Win7 char
				}
				else
					c = TEXT('•'); // Vista char (unsure if this is the right char)
			}
			else
				c = TEXT('•'); // XP char
			//c = TEXT('*'); // before win xp
		}
		if (i)
		{
			this->addStyle(WindowStyle::ES_Password);

			Edit_SetPasswordChar(m_Hwnd, c);
		}
		else {
			this->removeStyle(WindowStyle::ES_Password);
			Edit_SetPasswordChar(m_Hwnd, 0);
			this->m_PassChar = c;	// save pass char used for later
		}

		this->redrawWindow();
	}
	// xdid -l [NAME] [ID] [SWITCH] [ON|OFF]
	else if (flags[TEXT('l')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		const BOOL enabled = (input.getnexttok().to_int() > 0);	// tok 4

		SendMessage(m_Hwnd, EM_SETREADONLY, gsl::narrow_cast<WPARAM>(enabled), NULL);
	}
	// xdid -o [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('o')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

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
			throw Dcx::dcxException("Insufficient parameters");

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
			throw Dcx::dcxException("Insufficient parameters");

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
			throw Dcx::dcxException("Insufficient parameters");

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
			throw Dcx::dcxException("Insufficient parameters");

		const auto istart = input.getnexttok().to_int();	// tok 4
		const auto iend = (numtok > 4) ? input.getnexttok().to_int() : istart;

		SendMessage(m_Hwnd, EM_SETSEL, gsl::narrow_cast<WPARAM>(istart), gsl::narrow_cast<LPARAM>(iend));
		SendMessage(m_Hwnd, EM_SCROLLCARET, NULL, NULL);
	}
	// xdid -E [NAME] [ID] [SWITCH] [CUE TEXT]
	else if (flags[TEXT('E')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		this->m_tsCue = input.getlasttoks();	// tok 4, -1
		Edit_SetCueBannerText(m_Hwnd, this->m_tsCue.to_wchr());
	}
	// xdid -y [NAME] [ID] [SWITCH] [0|1]
	else if (flags[TEXT('y')])
	{
		if (numtok < 4)
			throw Dcx::dcxException("Insufficient parameters");

		this->m_bIgnoreRepeat = (input.getnexttok().to_int() > 0);	// tok 4
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxEdit::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		switch (HIWORD(wParam))
		{
		case EN_CHANGE:
		{
			TGetWindowText(m_Hwnd, this->m_tsText);
			if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
				this->execAliasEx(TEXT("edit,%u"), getUserID());
		}

		break;
		}

		break;
	} // WM_COMMAND
	}

	return 0L;
}

LRESULT DcxEdit::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed)
{
	switch (uMsg)
	{
		// disabled this to fix the tabbing problem
		//case WM_GETDLGCODE:
		//    {
		//		if (!this->isStyle(WS_TABSTOP)) {
		//			bParsed = TRUE;
		//			return DLGC_WANTALLKEYS;
		//		}
		//    }
		//    break;

	case WM_KEYDOWN:
	{
		//if (wParam == VK_ESCAPE)
		//	bParsed = TRUE; // prevents m_pParent window closing.
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
		{
			if (wParam == VK_RETURN)
				this->execAliasEx(TEXT("return,%u"), getUserID());

			if ((this->m_bIgnoreRepeat) && (lParam & 0x40000000)) // ignore repeats
				break;

			this->execAliasEx(TEXT("keydown,%u,%u"), getUserID(), wParam);
		}
		/*
		// CTRL+A, select text and return so control doesnt beep
		if ((wParam == 65) &&
		(GetKeyState(VK_CONTROL) & 0x8000))
		{
		this->callAliasEx(ret, TEXT("%s,%d"), TEXT("select"), this->getUserID());
		//			bParsed = TRUE;
		//			return TRUE;
		}
		*/

		break;
	}
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
		break;
	}
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
		break;
	}
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
		break;
	}
	case WM_KEYUP:
	{
		if (dcx_testflag(this->getParentDialog()->getEventMask(), DCX_EVENT_EDIT))
			execAliasEx(TEXT("keyup,%u,%u"), getUserID(), wParam);
		break;
	}
	case WM_PAINT:
	{
		if (!this->IsAlphaBlend())
			break;
		PAINTSTRUCT ps{};

		auto hdc = BeginPaint(m_Hwnd, &ps);
		Auto(EndPaint(m_Hwnd, &ps));

		bParsed = TRUE;

		// Setup alpha blend if any.
		const auto ai = this->SetupAlphaBlend(&hdc);
		Auto(this->FinishAlphaBlend(ai));

		return CallDefaultClassProc(uMsg, (WPARAM)hdc, lParam);
	}
	break;

	case WM_DESTROY:
	{
		delete this;
		bParsed = TRUE;
		break;
	}
	default:
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}

WNDPROC DcxEdit::m_hDefaultClassProc = nullptr;

LRESULT DcxEdit::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc != nullptr)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
