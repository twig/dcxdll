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
	, m_bIgnoreRepeat(false)
	, m_PassChar(0)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	m_Hwnd = CreateWindowExW(
		gsl::narrow_cast<DWORD>(ExStyles) | WS_EX_CLIENTEDGE,
		L"EDIT",
		nullptr,
		WS_CHILD | gsl::narrow_cast<DWORD>(Styles),
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		reinterpret_cast<HMENU>(ID),
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	Edit_LimitText(m_Hwnd, 0);

	//SendMessage(m_Hwnd, CCM_SETUNICODEFORMAT, TRUE, NULL);

	if (p_Dialog->getToolTip() != nullptr) {
		if (styles.istok(TEXT("tooltips"))) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, m_Hwnd);
		}
	}

	this->m_bIgnoreRepeat = true;
	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), reinterpret_cast<HANDLE>(this));
	DragAcceptFiles(m_Hwnd, TRUE);
}

/*!
* \brief blah
*
* blah
*/
DcxEdit::~DcxEdit() {
	this->unregistreDefaultWindowProc();
}


const TString DcxEdit::getStyles(void) const {
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
void DcxEdit::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
{
	for (const auto &tsStyle : styles)
	{
#if DCX_USE_HASHING
		switch (std::hash<TString>{}(tsStyle.to_chr()))
		{
			case L"multi"_hash:
				*Styles |= ES_MULTILINE;
				break;
			case L"center"_hash:
				*Styles |= ES_CENTER;
				break;
			case L"right"_hash:
				*Styles |= ES_RIGHT;
				break;
			case L"autohs"_hash:
				*Styles |= ES_AUTOHSCROLL;
				break;
			case L"autovs"_hash:
				*Styles |= ES_AUTOVSCROLL;
				break;
			case L"vsbar"_hash:
				*Styles |= WS_VSCROLL;
				break;
			case L"hsbar"_hash:
				*Styles |= WS_HSCROLL;
				break;
			case L"lowercase"_hash:
				*Styles |= ES_LOWERCASE;
				break;
			case L"number"_hash:
				*Styles |= ES_NUMBER;
				break;
			case L"password"_hash:
				*Styles |= ES_PASSWORD;
				break;
			case L"uppercase"_hash:
				*Styles |= ES_UPPERCASE;
				break;
			case L"return"_hash:
				*Styles |= ES_WANTRETURN;
				break;
			case L"readonly"_hash:
				*Styles |= ES_READONLY;
				break;
			case L"showsel"_hash:
				*Styles |= ES_NOHIDESEL;
			default:
				break;
		}
#else
		if (tsStyle == TEXT("multi")) 
			*Styles |= ES_MULTILINE;
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
		else if (tsStyle == TEXT("lowercase"))
			*Styles |= ES_LOWERCASE;
		else if (tsStyle == TEXT("number"))
			*Styles |= ES_NUMBER;
		else if (tsStyle == TEXT("password"))
			*Styles |= ES_PASSWORD;
		else if (tsStyle == TEXT("uppercase"))
			*Styles |= ES_UPPERCASE;
		else if (tsStyle == TEXT("return"))
			*Styles |= ES_WANTRETURN;
		else if (tsStyle == TEXT("readonly"))
			*Styles |= ES_READONLY;
		else if (tsStyle == TEXT("showsel"))
			*Styles |= ES_NOHIDESEL;
#endif
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
void DcxEdit::parseInfoRequest( const TString &input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
#if DCX_USE_HASHING
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP] [N]
	case L"text"_hash:
	{
		if (this->isStyle(ES_MULTILINE)) {
			if (input.numtok() > 3) {
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
		if (this->isStyle(ES_MULTILINE))
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_tsText.numtok(TEXT("\r\n")));
		else {
			// single line control so always 1 line.
			szReturnValue[0] = TEXT('1');
			szReturnValue[1] = 0;
		}
	}
	break;
	// [NAME] [ID] [PROP]
	case L"ispass"_hash:
		szReturnValue = dcx_truefalse(isStyle(ES_PASSWORD));
		break;
		// [NAME] [ID] [PROP]
	case L"isreadonly"_hash:
		szReturnValue = dcx_truefalse(isStyle(ES_READONLY));
		break;
		// [NAME] [ID] [PROP]
	case L"caretpos"_hash:
	{
		DWORD dwAbsoluteStartSelPos = 0;

		// caret startsel position
		SendMessage(m_Hwnd, EM_GETSEL, (WPARAM)&dwAbsoluteStartSelPos, NULL);

		if (this->isStyle(ES_MULTILINE)) {
			// current line
			const auto iLinePos = SendMessage(m_Hwnd, EM_LINEFROMCHAR, (WPARAM)-1, NULL);
			// line offset
			const auto iAbsoluteCharPos = gsl::narrow_cast<int>(SendMessage(m_Hwnd, EM_LINEINDEX, (WPARAM)-1, NULL));

			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %u"), iLinePos + 1, dwAbsoluteStartSelPos - iAbsoluteCharPos);
		}
		else {
			// return selstart
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("1 %u"), dwAbsoluteStartSelPos);
		}
	}
	break;
	case L"selstart"_hash:
	{
		DWORD dwSelStart = 0; // selection range starting position

		SendMessage(m_Hwnd, EM_GETSEL, (WPARAM)&dwSelStart, NULL);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), dwSelStart);
	}
	break;
	case L"selend"_hash:
	{
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(m_Hwnd, EM_GETSEL, NULL, (LPARAM)&dwSelEnd);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), dwSelEnd);
	}
	break;
	case L"sel"_hash:
	{
		DWORD dwSelStart = 0; // selection range starting position
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(m_Hwnd, EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u %u"), dwSelStart, dwSelEnd);
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
#else
	const auto numtok = input.numtok();

	// [NAME] [ID] [PROP] [N]
	if (const auto prop(input.getfirsttok(3)); prop == TEXT("text")) {
		if (this->isStyle(ES_MULTILINE)) {
			if (numtok > 3) {
				if (const auto nLine = input.getnexttok().to_int(); (nLine > 0 && nLine <= gsl::narrow_cast<int>(m_tsText.numtok(TEXT("\r\n")))))
					szReturnValue = m_tsText.gettok(nLine, TEXT("\r\n")).to_chr();
			}
		}
		else
			szReturnValue = m_tsText.to_chr();
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("num")) {
		if (this->isStyle(ES_MULTILINE))
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_tsText.numtok(TEXT("\r\n")));
		else {
			// single line control so always 1 line.
			szReturnValue[0] = TEXT('1');
			szReturnValue[1] = 0;
		}
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("ispass")) {
		dcx_Con(this->isStyle(ES_PASSWORD), szReturnValue);
	}
   // [NAME] [ID] [PROP]
	else if (prop == TEXT("isreadonly")) {
		dcx_Con(this->isStyle(ES_READONLY), szReturnValue);
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
			const auto iAbsoluteCharPos = gsl::narrow_cast<int>(SendMessage(m_Hwnd, EM_LINEINDEX, (WPARAM)-1, NULL));

			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %u"), iLinePos +1, dwAbsoluteStartSelPos - iAbsoluteCharPos);
		}
		else {
			// return selstart
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("1 %u"), dwAbsoluteStartSelPos);
		}
	}
	else if (prop == TEXT("selstart")) {
		DWORD dwSelStart = 0; // selection range starting position

		SendMessage(m_Hwnd, EM_GETSEL, (WPARAM) &dwSelStart, NULL);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), dwSelStart);
	}
	else if (prop == TEXT("selend")) {
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(m_Hwnd, EM_GETSEL, NULL, (LPARAM) &dwSelEnd);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), dwSelEnd);
	}
	else if (prop == TEXT("sel")) {
		DWORD dwSelStart = 0; // selection range starting position
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(m_Hwnd, EM_GETSEL, (WPARAM) &dwSelStart, (LPARAM) &dwSelEnd);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u %u"), dwSelStart, dwSelEnd);
	}
	else if (prop == TEXT("seltext")) {
		DWORD dwSelStart = 0; // selection range starting position
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(m_Hwnd, EM_GETSEL, (WPARAM) &dwSelStart, (LPARAM) &dwSelEnd);
		szReturnValue = m_tsText.mid(dwSelStart, dwSelEnd - dwSelStart).to_chr();
	}
	else if (prop == TEXT("cue")) {
		if (!this->m_tsCue.empty())
			szReturnValue = m_tsCue.to_chr();
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
#endif
}

/*!
* \brief blah
*
* blah
*/
void DcxEdit::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')]) {
		this->m_tsText.clear();	// = TEXT("");
		SetWindowTextW(m_Hwnd, L"");
	}

	// xdid -a [NAME] [ID] [SWITCH] [TEXT]
	if (flags[TEXT('a')] && numtok > 3) {
		this->m_tsText += input.getlasttoks();	// tok 4, -1
		SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());
	}
	// xdid -c [NAME] [ID] [SWITCH]
	else if (flags[TEXT('c')] && numtok > 2) {
		CopyToClipboard(m_Hwnd, this->m_tsText);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')] && numtok > 3) {
		if (this->isStyle(ES_MULTILINE)) {
			const auto nLine = input.getnexttok().to_<UINT>();	// tok 4
			this->m_tsText.deltok(nLine, TEXT("\r\n"));
			SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());
		}
	}
	// xdid -i [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('i')] && numtok > 4) {
		if (this->isStyle(ES_MULTILINE)) {
			const auto nLine = input.getnexttok().to_<UINT>();	// tok 4
			this->m_tsText.instok(input.getlasttoks(), nLine, TEXT("\r\n"));	// tok 5, -1
		}
		else
			this->m_tsText = input.getlasttoks();	// tok 5, -1
		SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());
	}
	// xdid -j [NAME] [ID] [SWITCH] [0|1]
	else if (flags[TEXT('j')] && numtok > 3) {
		const auto i = input.getnexttok().to_<UINT>();	// tok 4

		auto c = Edit_GetPasswordChar(m_Hwnd);
		if (c == 0)
			c = this->m_PassChar;
		// XP actually uses the unicode `Black Circle` char U+25CF (9679)
		// The problem is getting the char set to a unicode (2-byte) one, so far it always sets to CF (207)
		if (c == 0) {
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
		if (i) {
			this->addStyle(ES_PASSWORD);

			Edit_SetPasswordChar(m_Hwnd, c);
		}
		else {
			this->removeStyle(ES_PASSWORD);
			Edit_SetPasswordChar(m_Hwnd, 0);
			this->m_PassChar = c;	// save pass char used for later
		}

		this->redrawWindow();
	}
	// xdid -l [NAME] [ID] [SWITCH] [ON|OFF]
	else if (flags[TEXT('l')] && numtok > 3) {
		const BOOL enabled = (input.getnexttok().to_int() > 0);	// tok 4

		SendMessage(m_Hwnd, EM_SETREADONLY, gsl::narrow_cast<WPARAM>(enabled), NULL);
	}
	// xdid -o [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('o')] && numtok > 3) {
		if (this->isStyle(ES_MULTILINE)) {
			const auto nLine = input.getnexttok().to_<UINT>();	// tok 4
			this->m_tsText.puttok(input.getlasttoks(), nLine, TEXT("\r\n"));	// tok 5, -1
		}
		else
			this->m_tsText = input.getlasttoks();	// tok 4, -1
		SetWindowTextW(m_Hwnd, this->m_tsText.to_wchr());
	}
	// xdid -P [NAME] [ID]
	else if (flags[TEXT('P')] && numtok > 1) {
		SendMessage(this->getHwnd(),WM_PASTE,NULL,NULL);
	}
	// xdid -q [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('q')] && numtok > 3) {
		if (const auto N = input.getnexttok().to_int(); N > -1)
			Edit_LimitText(m_Hwnd, N);
	}
	// Used to prevent invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
	}
	// xdid -t [NAME] [ID] [SWITCH] [FILENAME]
	else if (flags[TEXT('t')] && numtok > 3) {
		auto tsFile(input.getlasttoks().trim());	// tok 4, -1

		if (!IsFile(tsFile))
			throw Dcx::dcxException(TEXT("Unable to open: %"), tsFile);
		
		m_tsText = readTextFile(tsFile);
		SetWindowTextW(m_Hwnd, m_tsText.to_wchr());
	}
	// xdid -u [NAME] [ID] [SWITCH] [FILENAME]
	else if (flags[TEXT('u')] && numtok > 3) {
		if (const auto tsFile(input.getlasttoks().trim()); !SaveDataToFile(tsFile, this->m_tsText))
			throw Dcx::dcxException(TEXT("Unable to save: %"), tsFile);
	}
	// xdid -V [NAME] [ID]
	else if (flags[TEXT('V')]) {
		SendMessage(m_Hwnd, EM_SCROLLCARET, NULL, NULL);
	}
	// xdid -S [NAME] [ID] [SWITCH] [START] [END]
	else if (flags[TEXT('S')] && numtok > 3) {
		const auto istart = input.getnexttok().to_int();	// tok 4
		int iend = istart;
		
		if (numtok > 4)
			iend = input.getnexttok( ).to_int();	// tok 5

		SendMessage(m_Hwnd, EM_SETSEL, static_cast<WPARAM>(istart), static_cast<LPARAM>(iend));
		SendMessage(m_Hwnd, EM_SCROLLCARET, NULL, NULL);
	}
	// xdid -E [NAME] [ID] [SWITCH] [CUE TEXT]
	else if (flags[TEXT('E')] && numtok > 3) {
		this->m_tsCue = input.getlasttoks();	// tok 4, -1
		Edit_SetCueBannerText(m_Hwnd,this->m_tsCue.to_wchr());
	}
	// xdid -y [NAME] [ID] [SWITCH] [0|1]
	else if (flags[TEXT('y')] && numtok > 3) {
		const auto state = input.getnexttok().to_int();	// tok 4

		this->m_bIgnoreRepeat = (state > 0);
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxEdit::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) {
	switch (uMsg) {
		case WM_COMMAND:
		{
			switch ( HIWORD( wParam ) ) {
				case EN_CHANGE:
				{
					TGetWindowText(m_Hwnd, this->m_tsText);
					if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT))
						this->execAliasEx(TEXT("%s,%u"), TEXT("edit"), this->getUserID());
				}

				break;
			}

			break;
		} // WM_COMMAND
	}

	return 0L;
}

LRESULT DcxEdit::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) {
	switch (uMsg) {

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
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT)) {
					if (wParam == VK_RETURN)
						this->execAliasEx(TEXT("%s,%u"), TEXT("return"), this->getUserID());

					if ((this->m_bIgnoreRepeat) && (lParam & 0x40000000)) // ignore repeats
						break;

					this->execAliasEx(TEXT("%s,%u,%u"), TEXT("keydown"), this->getUserID(), wParam);
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
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT)) {
					//TCHAR szRet[256];
					//
					//evalAliasEx(szRet, Dcx::countof(szRet), TEXT("copy,%u"), getUserID());
					//
					//if (ts_strcmp(TEXT("nocopy"), szRet) == 0) {
					//	bParsed = TRUE;
					//	return 0L;
					//}

					stString<256> szRet;

					evalAliasEx(szRet, gsl::narrow_cast<int>(szRet.size()), TEXT("copy,%u"), getUserID());

					if (szRet == TEXT("nocopy")) {
						bParsed = TRUE;
						return 0L;
					}
				}
				break;
			}
		case WM_CUT:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT)) {
					//TCHAR szRet[256];
					//
					//evalAliasEx(szRet, Dcx::countof(szRet), TEXT("cut,%u"), getUserID());
					//
					//if (ts_strcmp(TEXT("nocut"), szRet) == 0) {
					//	bParsed = TRUE;
					//	return 0L;
					//}

					stString<256> szRet;

					evalAliasEx(szRet, gsl::narrow_cast<int>(szRet.size()), TEXT("cut,%u"), getUserID());

					if (szRet == TEXT("nocut")) {
						bParsed = TRUE;
						return 0L;
					}
				}
				break;
			}
		case WM_PASTE:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT)) {
					//TCHAR szRet[256];
					//
					//evalAliasEx(szRet, Dcx::countof(szRet), TEXT("paste,%u"), getUserID());
					//
					//if (ts_strcmp(TEXT("nopaste"), szRet) == 0) {
					//	bParsed = TRUE;
					//	return 0L;
					//}

					stString<256> szRet;

					evalAliasEx(szRet, gsl::narrow_cast<int>(szRet.size()), TEXT("paste,%u"), getUserID());

					if (szRet == TEXT("nopaste")) {
						bParsed = TRUE;
						return 0L;
					}
				}
				break;
			}
		case WM_KEYUP:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT))
					execAliasEx(TEXT("keyup,%u,%u"), getUserID(), wParam);
				break;
			}
		case WM_PAINT:
			{
				if (!this->m_bAlphaBlend)
					break;
				PAINTSTRUCT ps;

				auto hdc = BeginPaint(m_Hwnd, &ps);
				Auto(EndPaint(m_Hwnd, &ps));

				bParsed = TRUE;

				// Setup alpha blend if any.
				auto ai = this->SetupAlphaBlend(&hdc);
				Auto(this->FinishAlphaBlend(ai));

				return CallDefaultProc(m_Hwnd, uMsg, (WPARAM)hdc, lParam);
			}
			break;

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
