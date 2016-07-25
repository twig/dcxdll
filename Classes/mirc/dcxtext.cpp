/*!
 * \file dcxtext.cpp
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
#include "Classes/mirc/dcxtext.h"
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

DcxText::DcxText(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	
	parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	m_Hwnd = CreateWindowEx(
		ExStyles,
		TEXT("STATIC"),
		nullptr,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU)ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	// remove all borders
	removeStyle(WS_BORDER | WS_DLGFRAME);
	removeExStyle(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE);

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	m_clrText = GetSysColor(COLOR_WINDOWTEXT);

	setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);

	if (styles.istok(TEXT("tooltips"))) {
		if (!IsWindow(p_Dialog->getToolTip()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");

		m_ToolTipHWND = p_Dialog->getToolTip();
		AddToolTipToolInfo(m_ToolTipHWND, m_Hwnd);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

DcxText::~DcxText( ) {

	this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxText::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme)
{
	*Styles |= SS_NOTIFY;
	this->m_uiStyle = DT_LEFT;

	for (const auto &tsStyle: styles)
	{
#if DCX_USE_HASHING
		switch (std::hash<TString>{}(tsStyle))
		{
			case L"nowrap"_hash:
				m_uiStyle |= DT_SINGLELINE;
				break;
			case L"center"_hash:
				m_uiStyle |= DT_CENTER;
				break;
			case L"right"_hash:
				m_uiStyle |= DT_RIGHT;
				break;
			case L"noprefix"_hash:
				m_uiStyle |= DT_NOPREFIX;
				break;
			case L"endellipsis"_hash:
				m_uiStyle |= DT_END_ELLIPSIS;
				break;
			case L"pathellipsis"_hash:
				m_uiStyle |= DT_PATH_ELLIPSIS;
			default:
				break;
		}
#else
		if (tsStyle == TEXT("nowrap"))
			this->m_uiStyle |= DT_SINGLELINE;
		else if (tsStyle == TEXT("center"))
			this->m_uiStyle |= DT_CENTER;
		else if (tsStyle == TEXT("right"))
			this->m_uiStyle |= DT_RIGHT;
		else if (tsStyle == TEXT("noprefix"))
			this->m_uiStyle |= DT_NOPREFIX;
		else if (tsStyle == TEXT("endellipsis"))
			this->m_uiStyle |= DT_END_ELLIPSIS;
		else if (tsStyle == TEXT("pathellipsis"))
			this->m_uiStyle |= DT_PATH_ELLIPSIS;
#endif
	}

	if (!dcx_testflag(this->m_uiStyle, DT_SINGLELINE))
		this->m_uiStyle |= DT_WORDBREAK;

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

void DcxText::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	// [NAME] [ID] [PROP]
	if (input.gettok(3) == TEXT("text")) {

		GetWindowText(m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxText::parseCommandRequest(const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')]) {
		this->m_tsText.clear();	// = TEXT("");
		SetWindowText(m_Hwnd, TEXT(""));
	}

	// xdid -a [NAME] [ID] [SPACE 0|1] [TEXT]
	if (flags[TEXT('a')] && numtok > 2) {
		if (input.getnexttok( ).to_int() == 1)	// tok 4
			this->m_tsText += TEXT(' ');

		this->m_tsText += input.getlasttoks();	// tok 5, -1
		SetWindowText(m_Hwnd, this->m_tsText.to_chr());

		// redraw if transparent
		if (this->isExStyle(WS_EX_TRANSPARENT))
			this->InvalidateParentRect(m_Hwnd);
		this->redrawWindow();
	}
	// This is to avoid invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')]) {
	}
	//xdid -t [NAME] [ID] [SWITCH] [TEXT]
	else if (flags[TEXT('t')]) {
		this->m_tsText = input.getlasttoks();	// tok 4, -1
		SetWindowText(m_Hwnd, this->m_tsText.to_chr());

		// redraw if transparent
		if (this->isExStyle(WS_EX_TRANSPARENT))
			this->InvalidateParentRect(m_Hwnd);
		this->redrawWindow();
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxText::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxText::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

		case WM_ERASEBKGND:
			{
				if (this->isExStyle(WS_EX_TRANSPARENT)) {
					bParsed = TRUE;
					return TRUE;
				}
				break;
			}

		case WM_PRINTCLIENT:
			{
				this->DrawClientArea(reinterpret_cast<HDC>(wParam));
				bParsed = TRUE;
			}
			break;
		case WM_PAINT:
			{
				bParsed = TRUE;
				PAINTSTRUCT ps;

				auto hdc = BeginPaint(m_Hwnd, &ps);

				this->DrawClientArea(hdc);

				EndPaint( m_Hwnd, &ps );
			}
			break;

		case WM_ENABLE:
			{ // fixes bug with redraw when text control is enabled/disabled & formatted text is being used.
				bParsed = TRUE;
				InvalidateRect(m_Hwnd, nullptr, FALSE);
			}
			break;

		case WM_DESTROY:
			{
				delete this;
				bParsed = TRUE;
			}
			break;

		default:
			return this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return 0L;
}

void DcxText::DrawClientArea(HDC hdc)
{
	if (hdc == nullptr)
		return;

	RECT r;

	if (!GetClientRect(m_Hwnd, &r))
		return;

	// Setup alpha blend if any.
	auto ai = SetupAlphaBlend(&hdc);
	Auto(FinishAlphaBlend(ai));

	if (hdc == nullptr)
		return;

	TString wtext;
	TGetWindowText(m_Hwnd, wtext);

	DcxControl::DrawCtrlBackground(hdc,this,&r);

	HFONT oldFont = nullptr;
	COLORREF oldClr = CLR_INVALID;
	COLORREF oldBkgClr = CLR_INVALID;

	// check if font is valid & set it.
	if (this->m_hFont != nullptr)
		oldFont = SelectFont(hdc, this->m_hFont);
	// check if control is enabled.
	if (IsWindowEnabled(m_Hwnd)) {
		if (this->m_clrText != CLR_INVALID)
			oldClr = SetTextColor(hdc, this->m_clrText);
		if (this->m_clrBackText != CLR_INVALID)
			oldBkgClr = SetBkColor(hdc, this->m_clrBackText);
	}
	else { // disabled controls colouring
		oldClr = SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
		oldBkgClr = SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
	}

	//UINT style = DT_LEFT;
	//if (this->isStyle(SS_CENTER))
	//	style = DT_CENTER;
	//if (this->isStyle(SS_RIGHT))
	//	style = DT_RIGHT;
	//if (this->isStyle(SS_ENDELLIPSIS))
	//	style |= DT_END_ELLIPSIS;
	//if (this->isStyle(SS_PATHELLIPSIS))
	//	style |= DT_PATH_ELLIPSIS;
	//if (this->isStyle(SS_NOPREFIX))
	//	style |= DT_NOPREFIX;
	//if (this->isStyle(SS_LEFTNOWORDWRAP))
	//	style |= DT_SINGLELINE;
	//else
	//	style |= DT_WORDBREAK; // changed for autowrap between words
	//
	//this->ctrlDrawText(hdc, wtext, &r, style);

//#if DCX_DEBUG_OUTPUT
//	ColourString<TCHAR> tmp(wtext.to_chr());
//
//	ColourString<TCHAR>::RenderInfo ri;
//	ri.ri_dwFlags = this->m_uiStyle;
//	ri.ri_bEnableAngleChar = false;
//	ri.ri_bEnableAngleLine = true;
//	ri.ri_bEnableShadow = this->m_bShadowText;
//	ri.ri_iLineAngle = -20;
//	ri.ri_iCharAngle = 10;
//	ri.ri_crShadow = RGB(0, 0, 0);
//	ri.ri_crText = m_clrText;
//	ri.ri_ixOffset = 0;
//	ri.ri_iyOffset = 0;
//
//	getmIRCPalette(ri.ri_cPalette, Dcx::countof(ri.ri_cPalette)); // get mIRC palette
//
//	tmp.Render(hdc, &r, ri);
//
//#else

	this->ctrlDrawText(hdc, wtext, &r, this->m_uiStyle);
//#endif

	if (oldBkgClr != CLR_INVALID)
		SetBkColor(hdc, oldBkgClr);
	if (oldClr != CLR_INVALID)
		SetTextColor(hdc, oldClr);
	if (oldFont != nullptr)
		SelectFont(hdc, oldFont);
}
