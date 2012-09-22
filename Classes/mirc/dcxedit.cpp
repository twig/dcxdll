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

DcxEdit::DcxEdit(const UINT ID, DcxDialog *p_Dialog, const HWND mParentHwnd, const RECT *rc, const TString &styles)
: DcxControl(ID, p_Dialog)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	this->m_Hwnd = CreateWindowExW(
		ExStyles | WS_EX_CLIENTEDGE, 
		L"EDIT",
		NULL,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL), 
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw TEXT("Unable To Create Window");

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(this->m_Hwnd , L" ", L" ");

	Edit_LimitText(this->m_Hwnd, 0);
	//this->m_tsText = TEXT("");

	//SendMessage(this->m_Hwnd, CCM_SETUNICODEFORMAT, TRUE, NULL);

	if (p_Dialog->getToolTip() != NULL) {
		if (styles.istok(TEXT("tooltips"))) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
	}

	this->m_bIgnoreRepeat = TRUE;
	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
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


TString DcxEdit::getStyles(void) const {
	TString styles(__super::getStyles());
	const DWORD Styles = GetWindowStyle(this->m_Hwnd);
	if (Styles & ES_MULTILINE)
		styles.addtok(TEXT("multi"));
	if (Styles & ES_CENTER)
		styles.addtok(TEXT("center"));
	if (Styles & ES_RIGHT)
		styles.addtok(TEXT("right"));
	if (Styles & ES_AUTOHSCROLL)
		styles.addtok(TEXT("autohs"));
	if (Styles & ES_AUTOVSCROLL)
		styles.addtok(TEXT("autovs"));
	if (Styles & WS_VSCROLL)
		styles.addtok(TEXT("vsbar"));
	if (Styles & WS_HSCROLL)
		styles.addtok(TEXT("hsbar"));
	if (Styles & ES_LOWERCASE)
		styles.addtok(TEXT("lowercase"));
	if (Styles & ES_NUMBER)
		styles.addtok(TEXT("number"));
	if (Styles & ES_PASSWORD)
		styles.addtok(TEXT("password"));
	if (Styles & ES_UPPERCASE)
		styles.addtok(TEXT("uppercase"));
	if (Styles & ES_WANTRETURN)
		styles.addtok(TEXT("return"));
	if (Styles & ES_READONLY)
		styles.addtok(TEXT("readonly"));
	if (Styles & ES_NOHIDESEL)
		styles.addtok(TEXT("showsel"));
	return styles;
}

void DcxEdit::toXml(TiXmlElement * xml) const
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
	const UINT numtok = styles.numtok( );

	for (UINT i = 1; i <= numtok; i++)
	{
		if (styles.gettok( i ) == TEXT("multi")) 
			*Styles |= ES_MULTILINE;
		else if (styles.gettok( i ) == TEXT("center"))
			*Styles |= ES_CENTER;
		else if (styles.gettok( i ) == TEXT("right"))
			*Styles |= ES_RIGHT;
		else if (styles.gettok( i ) == TEXT("autohs"))
			*Styles |= ES_AUTOHSCROLL;
		else if (styles.gettok( i ) == TEXT("autovs"))
			*Styles |= ES_AUTOVSCROLL;
		else if (styles.gettok( i ) == TEXT("vsbar"))
			*Styles |= WS_VSCROLL;
		else if (styles.gettok( i ) == TEXT("hsbar"))
			*Styles |= WS_HSCROLL;
		else if (styles.gettok( i ) == TEXT("lowercase"))
			*Styles |= ES_LOWERCASE;
		else if (styles.gettok( i ) == TEXT("number"))
			*Styles |= ES_NUMBER;
		else if (styles.gettok( i ) == TEXT("password"))
			*Styles |= ES_PASSWORD;
		else if (styles.gettok( i ) == TEXT("uppercase"))
			*Styles |= ES_UPPERCASE;
		else if (styles.gettok( i ) == TEXT("return"))
			*Styles |= ES_WANTRETURN;
		else if (styles.gettok( i ) == TEXT("readonly"))
			*Styles |= ES_READONLY;
		else if (styles.gettok( i ) == TEXT("showsel"))
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
	const unsigned int numtok = input.numtok( );

	const TString prop(input.gettok( 3 ));

	// [NAME] [ID] [PROP] [N]
	if (prop == TEXT("text")) {
		if (this->isStyle(ES_MULTILINE)) {
			if (numtok > 3) {
				const int nLine = input.gettok( 4 ).to_int();

				if (nLine > 0 && nLine <= this->m_tsText.numtok(TEXT("\r\n"))) {
					lstrcpyn(szReturnValue, this->m_tsText.gettok(nLine, TEXT("\r\n")).to_chr(), MIRC_BUFFER_SIZE_CCH);
					return;
				}
			}
		}
		else {
			lstrcpyn(szReturnValue, this->m_tsText.to_chr(), MIRC_BUFFER_SIZE_CCH);
			return;
		}
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("num")) {
		if (this->isStyle(ES_MULTILINE))
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->m_tsText.numtok(TEXT("\r\n")));
		else
			lstrcpyn(szReturnValue,TEXT("1"), MIRC_BUFFER_SIZE_CCH);	// single line control so always 1 line.
		return;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("ispass")) {
		if (this->isStyle(ES_PASSWORD))
			lstrcpyn(szReturnValue, TEXT("$true"), MIRC_BUFFER_SIZE_CCH);
		else
			lstrcpyn(szReturnValue, TEXT("$false"), MIRC_BUFFER_SIZE_CCH);

		return;
	}
   // [NAME] [ID] [PROP]
	else if (prop == TEXT("isreadonly")) {
      if (this->isStyle(ES_READONLY))
			lstrcpyn(szReturnValue, TEXT("$true"), MIRC_BUFFER_SIZE_CCH);
		else
			lstrcpyn(szReturnValue, TEXT("$false"), MIRC_BUFFER_SIZE_CCH);

		return;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("caretpos")) {
		DWORD dwAbsoluteStartSelPos = 0;

		// caret startsel position
		SendMessage(this->m_Hwnd, EM_GETSEL, (WPARAM) &dwAbsoluteStartSelPos, NULL);

		if (this->isStyle(ES_MULTILINE)) {
			// current line
			const int iLinePos = SendMessage(this->m_Hwnd, EM_LINEFROMCHAR, (WPARAM)-1, NULL);
			// line offset
			const int iAbsoluteCharPos = (int) SendMessage(this->m_Hwnd, EM_LINEINDEX, (WPARAM)-1, NULL);

			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), iLinePos +1, dwAbsoluteStartSelPos - iAbsoluteCharPos);
		}
		else {
			// return selstart
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), 1, dwAbsoluteStartSelPos);
		}

		return;
	}
	else if (prop == TEXT("selstart")) {
		DWORD dwSelStart = 0; // selection range starting position

		SendMessage(this->m_Hwnd, EM_GETSEL, (WPARAM) &dwSelStart, NULL);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), dwSelStart);
		return;
	}
	else if (prop == TEXT("selend")) {
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(this->m_Hwnd, EM_GETSEL, NULL, (LPARAM) &dwSelEnd);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), dwSelEnd);
		return;
	}
	else if (prop == TEXT("sel")) {
		DWORD dwSelStart = 0; // selection range starting position
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(this->m_Hwnd, EM_GETSEL, (WPARAM) &dwSelStart, (LPARAM) &dwSelEnd);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), dwSelStart, dwSelEnd);
		return;
	}
	else if (prop == TEXT("seltext")) {
		DWORD dwSelStart = 0; // selection range starting position
		DWORD dwSelEnd = 0;   // selection range ending position

		SendMessage(this->m_Hwnd, EM_GETSEL, (WPARAM) &dwSelStart, (LPARAM) &dwSelEnd);
		lstrcpyn(szReturnValue, this->m_tsText.mid(dwSelStart, dwSelEnd - dwSelStart).to_chr(), MIRC_BUFFER_SIZE_CCH);
		return;
	}
	else if (prop == TEXT("cue")) {
		if (this->m_tsCue.len())
			lstrcpyn(szReturnValue, this->m_tsCue.to_chr(), MIRC_BUFFER_SIZE_CCH);
		return;
	}
	else if (this->parseGlobalInfoRequest(input, szReturnValue))
		return;

	szReturnValue[0] = 0;
}

/*!
* \brief blah
*
* blah
*/
void DcxEdit::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.gettok(3));
	const int numtok = input.numtok( );

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')]) {
		this->m_tsText = TEXT("");
		SetWindowTextW(this->m_Hwnd, L"");
	}

	// xdid -a [NAME] [ID] [SWITCH] [TEXT]
	if (flags[TEXT('a')] && numtok > 3) {
		this->m_tsText += input.gettok(4, -1);
		SetWindowTextW(this->m_Hwnd, this->m_tsText.to_chr());
	}
	// xdid -c [NAME] [ID] [SWITCH]
	else if (flags[TEXT('c')] && numtok > 2) {
		CopyToClipboard(this->m_Hwnd, this->m_tsText);
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if (flags[TEXT('d')] && numtok > 3) {
		if (this->isStyle(ES_MULTILINE)) {
			const int nLine = input.gettok( 4 ).to_int();
			this->m_tsText.deltok(nLine, TEXT("\r\n"));
			SetWindowTextW(this->m_Hwnd, this->m_tsText.to_chr());
		}
	}
	// xdid -i [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('i')] && numtok > 4) {
		if (this->isStyle(ES_MULTILINE)) {
			const int nLine = input.gettok( 4 ).to_int();
			this->m_tsText.instok(input.gettok(5, -1).to_chr(), nLine, TEXT("\r\n"));
		}
		else
			this->m_tsText = input.gettok(5, -1);
		SetWindowTextW(this->m_Hwnd, this->m_tsText.to_chr());
	}
	// xdid -j [NAME] [ID] [SWITCH] [0|1]
	else if (flags[TEXT('j')] && numtok > 3) {
		const int i = input.gettok( 4 ).to_int();

		if (i) {
			this->addStyle(ES_PASSWORD);
			TCHAR c = Edit_GetPasswordChar(this->m_Hwnd);
			// XP actually uses the unicode `Black Circle` char U+25CF (9679)
			// The problem is getting the char set to a unicode (2-byte) one, so far it always sets to CF (207)
			if (c == 0)
				c = TEXT('•'); //(Dcx::XPPlusModule.isUseable()() ? TEXT('•') : TEXT('*'));
				//c = TEXT('*'); //(Dcx::XPPlusModule.isUseable()() ? TEXT('•') : TEXT('*'));

			Edit_SetPasswordChar(this->m_Hwnd, c);
		}
		else {
			this->removeStyle(ES_PASSWORD);
			Edit_SetPasswordChar(this->m_Hwnd, 0);
		}

		this->redrawWindow();
	}
   // xdid -l [NAME] [ID] [SWITCH] [ON|OFF]
   else if (flags[TEXT('l')] && numtok > 3) {
      const BOOL enabled = (input.gettok(4).to_int() > 0 ? TRUE : FALSE);

      SendMessage(this->m_Hwnd, EM_SETREADONLY, enabled, NULL);
   }
	// xdid -o [NAME] [ID] [SWITCH] [N] [TEXT]
	else if (flags[TEXT('o')] && numtok > 3) {
		if (this->isStyle(ES_MULTILINE)) {
			const int nLine = input.gettok( 4 ).to_int();
			this->m_tsText.puttok(input.gettok(5, -1).to_chr(), nLine, TEXT("\r\n"));
		}
		else
			this->m_tsText = input.gettok(4, -1);
		SetWindowTextW(this->m_Hwnd, this->m_tsText.to_chr());
	}
	// xdid -P [NAME] [ID]
	else if (flags[TEXT('P')] && numtok > 1) {
		SendMessage(this->getHwnd(),WM_PASTE,NULL,NULL);
	}
	// xdid -q [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('q')] && numtok > 3) {
		const int N = input.gettok( 4 ).to_int();

		if (N > -1)
			Edit_LimitText(this->m_Hwnd, N);
	}
	// Used to prevent invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
	}
	// xdid -t [NAME] [ID] [SWITCH] [FILENAME]
	else if (flags[TEXT('t')] && numtok > 3) {
		const BYTE * contents = readFile(input.gettok(4, -1).to_chr());

		if (contents != NULL) {
			this->m_tsText = (PTCHAR)contents;
			SetWindowTextW(this->m_Hwnd, this->m_tsText.to_chr());
			delete [] contents;
		}
	}
	// xdid -u [NAME] [ID] [SWITCH] [FILENAME]
	else if (flags[TEXT('u')] && numtok > 3) {
		FILE *file = dcx_fopen(input.gettok(4, -1).to_chr(), TEXT("wb"));

		if (file != NULL) {
			fwrite(this->m_tsText.to_chr(), sizeof(TCHAR), this->m_tsText.len(), file);
			fflush(file);
			fclose(file);
		}
	}
	// xdid -S [NAME] [ID] [SWITCH] [START] [END]
	else if (flags[TEXT('S')] && numtok > 3) {
		const int istart = input.gettok( 4 ).to_int();
		int iend;
		
		if (numtok > 4)
			iend = input.gettok( 5 ).to_int();
		else
			iend = istart;

		SendMessage(this->m_Hwnd, EM_SETSEL, istart, iend);
		SendMessage(this->m_Hwnd, EM_SCROLLCARET, NULL, NULL);
	}
	// xdid -E [NAME] [ID] [SWITCH] [CUE TEXT]
	else if (flags[TEXT('E')] && numtok > 3) {
		this->m_tsCue = input.gettok(4, -1);
		Edit_SetCueBannerText(this->m_Hwnd,this->m_tsCue.to_chr());
	}
	// xdid -y [NAME] [ID] [SWITCH] [0|1]
	else if (flags[TEXT('y')] && numtok > 3) {
		const int state = input.gettok(4).to_int();

		this->m_bIgnoreRepeat = (state > 0 ? TRUE : FALSE);
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
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_EDIT)
						this->execAliasEx(TEXT("%s,%d"), TEXT("edit"), this->getUserID());
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
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_EDIT) {
					if (wParam == VK_RETURN)
						this->execAliasEx(TEXT("%s,%d"), TEXT("return"), this->getUserID());

					if ((this->m_bIgnoreRepeat) && (lParam & 0x40000000)) // ignore repeats
						break;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("keydown"), this->getUserID(), wParam);
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
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_EDIT) {
					TCHAR ret[256];

					this->evalAliasEx(ret, 255, TEXT("%s,%d"), TEXT("copy"), this->getUserID());

					if (lstrcmp(TEXT("nocopy"), ret) == 0) {
						bParsed = TRUE;
						return 0L;
					}
				}
				break;
			}
		case WM_CUT:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_EDIT) {
					TCHAR ret[256];

					this->evalAliasEx(ret, 255, TEXT("%s,%d"), TEXT("cut"), this->getUserID());

					if (lstrcmp(TEXT("nocut"), ret) == 0) {
						bParsed = TRUE;
						return 0L;
					}
				}
				break;
			}
		case WM_PASTE:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_EDIT) {
					TCHAR ret[256];

					this->evalAliasEx(ret, 255, TEXT("%s,%d"), TEXT("paste"), this->getUserID());

					if (lstrcmp(TEXT("nopaste"), ret) == 0) {
						bParsed = TRUE;
						return 0L;
					}
				}
				break;
			}
		case WM_KEYUP:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_EDIT)
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("keyup"), this->getUserID(), wParam);
				break;
			}
		case WM_PAINT:
			{
				if (!this->m_bAlphaBlend)
					break;
				PAINTSTRUCT ps;
				HDC hdc;

				hdc = BeginPaint( this->m_Hwnd, &ps );

				//LRESULT res = 0L;
				bParsed = TRUE;

				// Setup alpha blend if any.
				LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

				//RECT rcTxt;
				//GetClientRect(this->m_Hwnd, &rcTxt);

				//// fill background.
				//if (this->isExStyle(WS_EX_TRANSPARENT))
				//{
				//	if (!this->m_bAlphaBlend)
				//		this->DrawParentsBackground(hdc,&rcTxt);
				//}
				//else
				//	DcxControl::DrawCtrlBackground(hdc,this,&rcTxt);

				//HFONT oldFont = NULL;
				//COLORREF oldClr = CLR_INVALID;
				//COLORREF oldBkgClr = CLR_INVALID;

				//// check if font is valid & set it.
				//if (this->m_hFont != NULL)
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

				//UINT style = DT_LEFT|DT_NOPREFIX|DT_VCENTER;
				//if (this->isStyle(ES_CENTER))
				//	style = DT_CENTER|DT_NOPREFIX|DT_VCENTER;
				//else if (this->isStyle(ES_RIGHT))
				//	style = DT_RIGHT|DT_NOPREFIX|DT_VCENTER;
				//if (!this->isStyle(ES_MULTILINE))
				//	style |= DT_SINGLELINE;

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

				//if (oldBkgClr != CLR_INVALID)
				//	SetBkColor(hdc, oldBkgClr);
				//if (oldClr != CLR_INVALID)
				//	SetTextColor(hdc, oldClr);
				//if (oldFont != NULL)
				//	SelectFont( hdc, oldFont );

				LRESULT res = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

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
