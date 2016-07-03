/*!
 * \file dcxline.cpp
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
#include "Classes/mirc/dcxline.h"
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

DcxLine::DcxLine(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles)
	: DcxControl(ID, p_Dialog)
	, m_bVertical(false)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_TRANSPARENT,
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

	if (bNoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	this->setControlFont(GetStockFont(DEFAULT_GUI_FONT), FALSE);
	this->registreDefaultWindowProc();
	SetProp(m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxLine::~DcxLine() {

	this->unregistreDefaultWindowProc();
}

const TString DcxLine::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);
	if (this->m_bVertical)
		styles.addtok(TEXT("vertical"));
	if (dcx_testflag(Styles, SS_LEFTNOWORDWRAP))
		styles.addtok(TEXT("nowrap"));
	if (dcx_testflag(Styles, SS_CENTER))
		styles.addtok(TEXT("center"));
	if (dcx_testflag(Styles, SS_RIGHT))
		styles.addtok(TEXT("right"));
	if (dcx_testflag(Styles, SS_NOPREFIX))
		styles.addtok(TEXT("noprefix"));
	if (dcx_testflag(Styles, SS_ENDELLIPSIS))
		styles.addtok(TEXT("endellipsis"));
	if (dcx_testflag(Styles, SS_PATHELLIPSIS))
		styles.addtok(TEXT("pathellipsis"));
	return styles;

}

void DcxLine::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);
	TString styles(xml->Attribute("styles"));
	styles.remtok(TEXT("transparent"), 1); // line always has transparent style (why?)
	if (!styles.empty())
		xml->SetAttribute("styles", styles.c_str());
	else
		xml->RemoveAttribute("styles");
	if (!this->m_sText.empty())
		xml->SetAttribute("caption", this->m_sText.c_str());
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxLine::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	for (const auto &tsStyle: styles)
	{
#if DCX_USE_HASHING
		switch (const_hash(tsStyle.to_chr()))
		{
			case L"vertical"_hash:
				m_bVertical = true;
				break;
			case L"nowrap"_hash:
				*Styles |= SS_LEFTNOWORDWRAP;
				break;
			case L"center"_hash:
				*Styles |= SS_CENTER;
				break;
			case L"right"_hash:
				*Styles |= SS_RIGHT;
				break;
			case L"noprefix"_hash:
				*Styles |= SS_NOPREFIX;
				break;
			case L"endellipsis"_hash:
				*Styles |= SS_ENDELLIPSIS;
				break;
			case L"pathellipsis"_hash:
				*Styles |= SS_PATHELLIPSIS;
			default:
				break;
		}
#else
		if ( tsStyle == TEXT("vertical") )
			this->m_bVertical = true;
		else if (tsStyle == TEXT("nowrap"))
			*Styles |= SS_LEFTNOWORDWRAP;
		else if (tsStyle == TEXT("center"))
			*Styles |= SS_CENTER;
		else if (tsStyle == TEXT("right"))
			*Styles |= SS_RIGHT;
		else if (tsStyle == TEXT("noprefix"))
			*Styles |= SS_NOPREFIX;
		else if (tsStyle == TEXT("endellipsis"))
			*Styles |= SS_ENDELLIPSIS;
		else if (tsStyle == TEXT("pathellipsis"))
			*Styles |= SS_PATHELLIPSIS;
#endif
	}

	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxLine::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	if (input.gettok(3) == TEXT("text")) {
		dcx_strcpyn(szReturnValue, this->m_sText.to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxLine::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.gettok(3));

	//xdid -t [NAME] [ID] [SWITCH] [TEXT]
	if (flags[TEXT('t')]) {
		this->m_sText = input.getlasttoks().trim();	// tok 4, -1

		// redraw if transparent
		if (this->isExStyle(WS_EX_TRANSPARENT)) {

			this->InvalidateParentRect(m_Hwnd);

			this->redrawWindow();
		}
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxLine::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxLine::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

		case WM_ERASEBKGND:
			{
				//this->DrawParentsBackground((HDC) wParam);
				bParsed = TRUE;
				return TRUE;
			}
			break;
		case WM_PRINTCLIENT:
			{
				this->DrawClientArea((HDC)wParam);
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

void DcxLine::DrawClientArea(HDC hdc)
{
	RECT rcClient = { 0 }, rcLine = { 0 }, rcText = { 0 };

	// get controls client area
	if (!GetClientRect(m_Hwnd, &rcClient))
		return;

	// Setup alpha blend if any.
	auto ai = SetupAlphaBlend(&hdc);
	Auto(FinishAlphaBlend(ai));

	// fill background.
	//DcxControl::DrawCtrlBackground(hdc,this,&rcClient);

	//res = CallWindowProc( this->m_DefaultWindowProc, m_Hwnd, uMsg, (WPARAM) hdc, lParam );
	rcLine = rcClient;
	rcText = rcClient;

	// draw text if any.
	if (!m_sText.empty()) {
		HFONT oldhFont = nullptr;
		if (this->m_hFont != nullptr)
			oldhFont = SelectFont(hdc, this->m_hFont);

		if (this->m_clrText != CLR_INVALID)
			SetTextColor(hdc, this->m_clrText);
		else
			SetTextColor(hdc, GetSysColor(IsWindowEnabled(m_Hwnd) ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));

		UINT style = 0;
		if (this->isStyle(SS_ENDELLIPSIS))
			style |= DT_END_ELLIPSIS;
		if (this->isStyle(SS_PATHELLIPSIS))
			style |= DT_PATH_ELLIPSIS;
		if (this->isStyle(SS_NOPREFIX))
			style |= DT_NOPREFIX;
		if (this->isStyle(SS_LEFTNOWORDWRAP))
			style |= DT_SINGLELINE;
		if (this->m_bVertical) {
			// orig ver, relies on vertical font being set when -t command is used.
			//SIZE sz;
			//int oMode = SetBkMode(hdc, TRANSPARENT);
			//GetTextExtentPoint32(hdc,this->m_sText.to_chr(),this->m_sText.len(), &sz);
			//rcText.bottom = rcText.top + sz.cx;
			//rcText.right = rcText.left + sz.cy;
			//if (this->isStyle(SS_CENTER))
			//	OffsetRect(&rcText,((rcClient.right - rcClient.left)/2) - ((rcText.right - rcText.left)/2),((rcClient.bottom - rcClient.top)/2) - ((rcText.bottom - rcText.top)/2));
			//else if (this->isStyle(SS_RIGHT))
			//	OffsetRect(&rcText,((rcClient.right - rcClient.left)/2) - ((rcText.right - rcText.left)/2),rcClient.bottom - (rcText.bottom - rcText.top));
			//TextOut(hdc,rcText.left, rcText.bottom, this->m_sText.to_chr(), this->m_sText.len());
			//SetBkMode(hdc, oMode);

			// new working ver that does the same as the orig but using the current font.
			SIZE sz = { 0 };
			const auto oMode = SetBkMode(hdc, TRANSPARENT);
			Auto(SetBkMode(hdc, oMode));

			GetTextExtentPoint32(hdc,this->m_sText.to_chr(),this->m_sText.len(), &sz);

			rcText.bottom = rcText.top + sz.cx;
			rcText.right = rcText.left + sz.cy;

			if (this->isStyle(SS_CENTER))
				OffsetRect(&rcText,((rcClient.right - rcClient.left)/2) - ((rcText.right - rcText.left)/2),((rcClient.bottom - rcClient.top)/2) - ((rcText.bottom - rcText.top)/2));
			else if (this->isStyle(SS_RIGHT))
				OffsetRect(&rcText,((rcClient.right - rcClient.left)/2) - ((rcText.right - rcText.left)/2),rcClient.bottom - (rcText.bottom - rcText.top));

			DrawRotatedText(this->m_sText, &rcText, hdc, 90, true, 90);

			// test ver that uses a diff routine entierly to draw vertical text
			//int oMode = SetBkMode(hdc, TRANSPARENT);
			//UINT dvStyle = DV_BOTTOM|DV_HCENTER;
			//DrawVertText(hdc, this->m_sText.to_chr(), this->m_sText.len(), &rcText, dvStyle | DV_CALCRECT);
			//if (this->isStyle(SS_CENTER))
			//	OffsetRect(&rcText,((rcClient.right - rcClient.left)/2) - ((rcText.right - rcText.left)/2),((rcClient.bottom - rcClient.top)/2) - ((rcText.bottom - rcText.top)/2));
			//else if (this->isStyle(SS_RIGHT))
			//	OffsetRect(&rcText,((rcClient.right - rcClient.left)/2) - ((rcText.right - rcText.left)/2),rcClient.bottom - (rcText.bottom - rcText.top));
			//DrawVertText(hdc, this->m_sText.to_chr(), this->m_sText.len(), &rcText, dvStyle);
			//SetBkMode(hdc, oMode);
		}
		else {
			style |= DT_LEFT|DT_VCENTER;

			//if (this->m_bCtrlCodeText)
			//	calcStrippedRect(hdc, this->m_sText, style, &rcText, false);
			//else
			//	DrawTextW(hdc, this->m_sText.to_chr(), this->m_sText.len(), &rcText, DT_CALCRECT | style);

			this->calcTextRect(hdc, this->m_sText, &rcText, style);
			if (this->isStyle(SS_CENTER))
				OffsetRect(&rcText,((rcClient.right - rcClient.left)/2) - ((rcText.right - rcText.left)/2),0);
			else if (this->isStyle(SS_RIGHT))
				OffsetRect(&rcText,rcClient.right - (rcText.right - rcText.left),0);

			// draw the text
			this->ctrlDrawText(hdc, this->m_sText, &rcText, style);
		}
		if (oldhFont != nullptr)
			SelectFont(hdc, oldhFont);

		ExcludeClipRect(hdc,rcText.left, rcText.top, rcText.right, rcText.bottom);
	}
	if (this->m_bVertical) {
		rcLine.left = rcLine.left + ((rcLine.right - rcLine.left) / 2);
		DrawEdge(hdc, &rcLine, EDGE_ETCHED, BF_LEFT);
	}
	else {
		rcLine.bottom = rcLine.bottom / 2;
		DrawEdge(hdc, &rcLine, EDGE_ETCHED, BF_BOTTOM);
	}
}
