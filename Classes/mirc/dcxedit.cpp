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

	this->m_Hwnd = CreateWindowExW(
		ExStyles | WS_EX_CLIENTEDGE,
		L"EDIT",
		nullptr,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		reinterpret_cast<HMENU>(ID),
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(this->m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(this->m_Hwnd, L" ", L" ");

	Edit_LimitText(this->m_Hwnd, 0);

	//SendMessage(this->m_Hwnd, CCM_SETUNICODEFORMAT, TRUE, NULL);

	if (p_Dialog->getToolTip() != nullptr) {
		if (styles.istok(TEXT("tooltips"))) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
	}

	this->m_bIgnoreRepeat = true;
	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(this->m_Hwnd, TEXT("dcx_cthis"), reinterpret_cast<HANDLE>(this));
	DragAcceptFiles(this->m_Hwnd, TRUE);
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
	const auto Styles = GetWindowStyle(this->m_Hwnd);

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
	xml->SetAttribute("caption", this->m_tsText.c_str());
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
void DcxEdit::parseInfoRequest( const TString &input, PTCHAR szReturnValue) const
{
	const auto numtok = input.numtok();

	const auto prop(input.getfirsttok(3));

	// [NAME] [ID] [PROP] [N]
	if (prop == TEXT("text")) {
		if (this->isStyle(ES_MULTILINE)) {
			if (numtok > 3) {
				const auto nLine = input.getnexttok().to_int();	// tok 4

				if (nLine > 0 && nLine <= (int)this->m_tsText.numtok(TEXT("\r\n")))
					dcx_strcpyn(szReturnValue, this->m_tsText.gettok(nLine, TEXT("\r\n")).to_chr(), MIRC_BUFFER_SIZE_CCH);
			}
		}
		else
			dcx_strcpyn(szReturnValue, this->m_tsText.to_chr(), MIRC_BUFFER_SIZE_CCH);
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
		SendMessage(this->m_Hwnd, EM_GETSEL, (WPARAM) &dwAbsoluteStartSelPos, NULL);

		if (this->isStyle(ES_MULTILINE)) {
			// current line
			const auto iLinePos = SendMessage(this->m_Hwnd, EM_LINEFROMCHAR, (WPARAM)-1, NULL);
			// line offset
			const auto iAbsoluteCharPos = (int)SendMessage(this->m_Hwnd, EM_LINEINDEX, (WPARAM)-1, NULL);

			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %u"), iLinePos +1, dwAbsoluteStartSelPos - iAbsoluteCharPos);
		}
		else {
			// return selstart
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("1 %u"), dwAbsoluteStartSelPos);
		}
	}
	else if (prop == TEXT("selstart")) {
		DWORD dwSelStart = 0; // selection range starting position

		SendMessage(this->m_Hwnd, EM_GETSEL, (WPARAM) &dwSelStart, NULL);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), dwSelStart);
	}
	else if (prop == TEXT("selend")) {
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(this->m_Hwnd, EM_GETSEL, NULL, (LPARAM) &dwSelEnd);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), dwSelEnd);
	}
	else if (prop == TEXT("sel")) {
		DWORD dwSelStart = 0; // selection range starting position
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(this->m_Hwnd, EM_GETSEL, (WPARAM) &dwSelStart, (LPARAM) &dwSelEnd);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u %u"), dwSelStart, dwSelEnd);
	}
	else if (prop == TEXT("seltext")) {
		DWORD dwSelStart = 0; // selection range starting position
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(this->m_Hwnd, EM_GETSEL, (WPARAM) &dwSelStart, (LPARAM) &dwSelEnd);
		dcx_strcpyn(szReturnValue, this->m_tsText.mid(dwSelStart, dwSelEnd - dwSelStart).to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	else if (prop == TEXT("cue")) {
		if (!this->m_tsCue.empty())
			dcx_strcpyn(szReturnValue, this->m_tsCue.to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
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
		SetWindowTextW(this->m_Hwnd, L"");
	}

	// xdid -a [NAME] [ID] [SWITCH] [TEXT]
	if (flags[TEXT('a')] && numtok > 3) {
		this->m_tsText += input.getlasttoks();	// tok 4, -1
		SetWindowTextW(this->m_Hwnd, this->m_tsText.to_wchr());
	}
	// xdid -c [NAME] [ID] [SWITCH]
	else if (flags[TEXT('c')] && numtok > 2) {
		CopyToClipboard(this->m_Hwnd, this->m_tsText);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')] && numtok > 3) {
		if (this->isStyle(ES_MULTILINE)) {
			const auto nLine = input.getnexttok().to_<UINT>();	// tok 4
			this->m_tsText.deltok(nLine, TEXT("\r\n"));
			SetWindowTextW(this->m_Hwnd, this->m_tsText.to_wchr());
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
		SetWindowTextW(this->m_Hwnd, this->m_tsText.to_wchr());
	}
	// xdid -j [NAME] [ID] [SWITCH] [0|1]
	else if (flags[TEXT('j')] && numtok > 3) {
		const auto i = input.getnexttok().to_<UINT>();	// tok 4

		auto c = Edit_GetPasswordChar(this->m_Hwnd);
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

			Edit_SetPasswordChar(this->m_Hwnd, c);
		}
		else {
			this->removeStyle(ES_PASSWORD);
			Edit_SetPasswordChar(this->m_Hwnd, 0);
			this->m_PassChar = c;	// save pass char used for later
		}

		this->redrawWindow();
	}
	// xdid -l [NAME] [ID] [SWITCH] [ON|OFF]
	else if (flags[TEXT('l')] && numtok > 3) {
		const BOOL enabled = (input.getnexttok().to_int() > 0);	// tok 4

		SendMessage(this->m_Hwnd, EM_SETREADONLY, enabled, NULL);
	}
	// xdid -o [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('o')] && numtok > 3) {
		if (this->isStyle(ES_MULTILINE)) {
			const auto nLine = input.getnexttok().to_<UINT>();	// tok 4
			this->m_tsText.puttok(input.getlasttoks(), nLine, TEXT("\r\n"));	// tok 5, -1
		}
		else
			this->m_tsText = input.getlasttoks();	// tok 4, -1
		SetWindowTextW(this->m_Hwnd, this->m_tsText.to_wchr());
	}
	// xdid -P [NAME] [ID]
	else if (flags[TEXT('P')] && numtok > 1) {
		SendMessage(this->getHwnd(),WM_PASTE,NULL,NULL);
	}
	// xdid -q [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('q')] && numtok > 3) {
		const auto N = input.getnexttok().to_int();	// tok 4

		if (N > -1)
			Edit_LimitText(this->m_Hwnd, N);
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
		
		this->m_tsText = readTextFile(tsFile);
		SetWindowTextW(this->m_Hwnd, this->m_tsText.to_wchr());
	}
	// xdid -u [NAME] [ID] [SWITCH] [FILENAME]
	else if (flags[TEXT('u')] && numtok > 3) {
		const auto tsFile(input.getlasttoks().trim());	// tok 4, -1

		if (!SaveDataToFile(tsFile, this->m_tsText))
			throw Dcx::dcxException(TEXT("Unable to save: %"), tsFile);
	}
	// xdid -V [NAME] [ID]
	else if (flags[TEXT('V')]) {
		SendMessage(this->m_Hwnd, EM_SCROLLCARET, NULL, NULL);
	}
	// xdid -S [NAME] [ID] [SWITCH] [START] [END]
	else if (flags[TEXT('S')] && numtok > 3) {
		const auto istart = input.getnexttok().to_int();	// tok 4
		int iend;
		
		if (numtok > 4)
			iend = input.getnexttok( ).to_int();	// tok 5
		else
			iend = istart;

		SendMessage(this->m_Hwnd, EM_SETSEL, istart, iend);
		SendMessage(this->m_Hwnd, EM_SCROLLCARET, NULL, NULL);
	}
	// xdid -E [NAME] [ID] [SWITCH] [CUE TEXT]
	else if (flags[TEXT('E')] && numtok > 3) {
		this->m_tsCue = input.getlasttoks();	// tok 4, -1
		Edit_SetCueBannerText(this->m_Hwnd,this->m_tsCue.to_wchr());
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
					TGetWindowText(this->m_Hwnd, this->m_tsText);
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
				//	bParsed = TRUE; // prevents parent window closing.
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
					TCHAR ret[256];

					this->evalAliasEx(ret, 255, TEXT("%s,%u"), TEXT("copy"), this->getUserID());

					if (lstrcmp(TEXT("nocopy"), ret) == 0) {
						bParsed = TRUE;
						return 0L;
					}
				}
				break;
			}
		case WM_CUT:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT)) {
					TCHAR ret[256];

					this->evalAliasEx(ret, 255, TEXT("%s,%u"), TEXT("cut"), this->getUserID());

					if (lstrcmp(TEXT("nocut"), ret) == 0) {
						bParsed = TRUE;
						return 0L;
					}
				}
				break;
			}
		case WM_PASTE:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT)) {
					TCHAR ret[256];

					this->evalAliasEx(ret, 255, TEXT("%s,%u"), TEXT("paste"), this->getUserID());

					if (lstrcmp(TEXT("nopaste"), ret) == 0) {
						bParsed = TRUE;
						return 0L;
					}
				}
				break;
			}
		case WM_KEYUP:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_EDIT))
					this->execAliasEx(TEXT("%s,%u,%u"), TEXT("keyup"), this->getUserID(), wParam);
				break;
			}
		case WM_PAINT:
			{
				if (!this->m_bAlphaBlend)
					break;
				PAINTSTRUCT ps;

				auto hdc = BeginPaint(this->m_Hwnd, &ps);

				//LRESULT res = 0L;
				bParsed = TRUE;

				// Setup alpha blend if any.
				auto ai = this->SetupAlphaBlend(&hdc);

				//RECT rcTxt;
				//GetClientRect(this->m_Hwnd, &rcTxt);
				//
				//// fill background.
				//if (this->isExStyle(WS_EX_TRANSPARENT))
				//{
				//	if (!this->m_bAlphaBlend)
				//		this->DrawParentsBackground(hdc,&rcTxt);
				//}
				//else
				//	DcxControl::DrawCtrlBackground(hdc,this,&rcTxt);
				//
				//HFONT oldFont = nullptr;
				//COLORREF oldClr = CLR_INVALID;
				//COLORREF oldBkgClr = CLR_INVALID;
				//
				//// check if font is valid & set it.
				//if (this->m_hFont != nullptr)
				//	oldFont = SelectFont(hdc, this->m_hFont);
				//// check if control is enabled.
				//if (IsWindowEnabled(this->m_Hwnd)) {
				//	if (this->m_clrText != CLR_INVALID)
				//		oldClr = SetTextColor(hdc, this->m_clrText);
				//	if (this->m_clrBackText != CLR_INVALID)
				//		oldBkgClr = SetBkColor(hdc, this->m_clrBackText);
				//}
				//else { // disabled controls colouring
				//	oldClr = SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
				//	oldBkgClr = SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
				//}
				//
				//UINT style = DT_LEFT|DT_NOPREFIX|DT_VCENTER;
				//if (this->isStyle(ES_CENTER))
				//	style = DT_CENTER|DT_NOPREFIX|DT_VCENTER;
				//else if (this->isStyle(ES_RIGHT))
				//	style = DT_RIGHT|DT_NOPREFIX|DT_VCENTER;
				//if (!this->isStyle(ES_MULTILINE))
				//	style |= DT_SINGLELINE;
				//
				//if (!this->m_bCtrlCodeText) {
				//	int oldBkgMode = SetBkMode(hdc, TRANSPARENT);
				//	if (this->m_bShadowText)
				//		dcxDrawShadowText(hdc, this->m_tsText.to_wchr(this->m_bUseUTF8), this->m_tsText.wlen(), &rcTxt, style, this->m_clrText, 0, 5, 5);
				//	else
				//		DrawTextW(hdc, this->m_tsText.to_wchr(this->m_bUseUTF8), this->m_tsText.wlen(), &rcTxt, style);
				//	SetBkMode(hdc, oldBkgMode);
				//}
				//else
				//	mIRC_DrawText(hdc, this->m_tsText, &rcTxt, style, this->m_bShadowText);
				//
				//if (oldBkgClr != CLR_INVALID)
				//	SetBkColor(hdc, oldBkgClr);
				//if (oldClr != CLR_INVALID)
				//	SetTextColor(hdc, oldClr);
				//if (oldFont != nullptr)
				//	SelectFont( hdc, oldFont );

				auto res = CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM)hdc, lParam);

				this->FinishAlphaBlend(ai);

				EndPaint( this->m_Hwnd, &ps );
				return res;
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
