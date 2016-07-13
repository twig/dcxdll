/*!
 * \file dcxprogressbar.cpp
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
#include "Classes/dcxprogressbar.h"
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

DcxProgressBar::DcxProgressBar( _In_ const UINT ID, _In_ DcxDialog *const p_Dialog, _In_ const HWND mParentHwnd, _In_ const RECT *const rc, _In_ const TString & styles )
: DcxControl( ID, p_Dialog )
, m_bIsAbsoluteValue(false)
, m_hfontVertical(nullptr)
, m_bIsGrad(false)
, m_tsText(TEXT("%d %%"))
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CLIENTEDGE,
		DCX_PROGRESSBARCLASS,
		nullptr,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( m_Hwnd , L" ", L" " );

	if (styles.istok(TEXT("tooltips"))) {
		if (!IsWindow(p_Dialog->getToolTip()))
			throw Dcx::dcxException("Unable to Initialize Tooltips");
		
		this->m_ToolTipHWND = p_Dialog->getToolTip();
		AddToolTipToolInfo(this->m_ToolTipHWND, m_Hwnd);
	}
	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxProgressBar::~DcxProgressBar( ) {

  this->unregistreDefaultWindowProc( );
}

const TString DcxProgressBar::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = GetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, PBS_SMOOTH))
		styles.addtok(TEXT("smooth"));
	if (dcx_testflag(Styles, PBS_VERTICAL))
		styles.addtok(TEXT("vertical"));
	if (dcx_testflag(Styles, PBS_MARQUEE))
		styles.addtok(TEXT("marquee"));
	if (this->m_bIsGrad)
		styles.addtok(TEXT("gradient"));
	return styles;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxProgressBar::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	this->m_bIsGrad = false;

	for (const auto &tsStyle: styles)
	{
#if DCX_USE_HASHING
		switch (dcx_hash(tsStyle.to_chr()))
		{
			case L"smooth"_hash:
				*Styles |= PBS_SMOOTH;
				break;
			case L"vertical"_hash:
				*Styles |= PBS_VERTICAL;
				break;
			case L"marquee"_hash:
				*Styles |= PBS_MARQUEE;
				break;
			case L"gradient"_hash:
				{
					*Styles |= PBS_SMOOTH;
					this->m_bIsGrad = true;
				}
			default:
				break;
		}
#else
		if (tsStyle == TEXT("smooth"))
			*Styles |= PBS_SMOOTH;
		else if (tsStyle == TEXT("vertical"))
			*Styles |= PBS_VERTICAL;
		else if (tsStyle == TEXT("marquee"))
			*Styles |= PBS_MARQUEE;
		else if (tsStyle == TEXT("gradient")) {
			*Styles |= PBS_SMOOTH;
			this->m_bIsGrad = true;
		}
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

void DcxProgressBar::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	const auto prop(input.getfirsttok(3));

	if ( prop == TEXT("value") ) {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getPosition( ) );
	}
	else if ( prop == TEXT("range") ) {
		PBRANGE pbr;
		this->getRange( FALSE, &pbr );
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), pbr.iLow, pbr.iHigh );
	}
	else if (prop == TEXT("text")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, this->m_tsText.to_chr(), this->CalculatePosition());
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
}
/*!
 * \brief blah
 *
 * \param input [NAME] [SWITCH] [ID] (OPTIONS)
 */

void DcxProgressBar::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -c name ID $rgb(color)
	if (flags[TEXT('c')]) {
		setBarColor(input.getnexttok( ).to_<COLORREF>());	// tok 4
	}
	//// xdid -g name ID [1|0]
	//else if ( flags[TEXT('g')] ) {
	//this->m_bIsGrad = input.gettok( 4 ).to_<bool>( );
	//}
	// xdid -i name ID (TEXT)
	else if (flags[TEXT('i')]) {
		if (numtok > 3)
			m_tsText = input.getlasttoks();	// tok 4, -1
		else
			m_tsText.clear();	// = TEXT("");

		redrawWindow();
	}
	// xdid -j name ID [a|p]
	else if (flags[TEXT('j')]) {
		m_bIsAbsoluteValue = (input.getnexttok() == TEXT('a'));

		redrawWindow();
	}
	// xdid -k name ID $rgb(color)
	else if (flags[TEXT('k')]) {
		setBKColor(input.getnexttok( ).to_<COLORREF>());	// tok 4
	}
	// xdid -m(o|g) name ID N
	else if (flags[TEXT('m')]) {
		// -mo
		if (flags[TEXT('o')])
			setMarquee(TRUE, input.getnexttok( ).to_int());	// tok 4
		// -mg
		else if (flags[TEXT('g')])
			setMarquee(FALSE, 0);
	}
	// xdid -q name ID [COLOR]
	else if ( flags[TEXT('q')] ) {
		m_clrText = input.getnexttok( ).to_<COLORREF>();	// tok 4
		redrawWindow();
	}
	// xdid -r name ID RLow RHigh
	else if (flags[TEXT('r')]) {
		if (numtok > 4)
			setRange(input.gettok( 4 ).to_int(), input.gettok( 5 ).to_int());
	}
	// xdid -t name ID
	else if (flags[TEXT('t')]) {
		stepIt();
	}
	// xdid -u name ID N
	else if (flags[TEXT('u')]) {
		setStep(input.getnexttok( ).to_int());	// tok 4
	}
	// xdid -v name ID N
	else if (flags[TEXT('v')]) {
		if (numtok > 3)
			setPosition(input.getnexttok( ).to_int());	// tok 4
	}
	// xdid [-o] [NAME] [ID] [ENABLED]
	// vertical fonts [1|0]
	else if (flags[TEXT('o')]) {
		if (numtok < 4)
			return;

		LOGFONT lfCurrent;
		ZeroMemory(&lfCurrent, sizeof(LOGFONT));

		if (GetObject(m_hFont, sizeof(LOGFONT), &lfCurrent) == 0)
			throw Dcx::dcxException("Unable to get LOGFONT");

		const auto angle = input.getnexttok().to_int();	// tok 4

		//TODO: let user specify angle of text?
		if (angle) {
			// input is angle based, expected angle = *10
			//lfCurrent.lfEscapement = angle * 10;
			//lfCurrent.lfOrientation = angle * 10;
			lfCurrent.lfEscapement = 900;
			lfCurrent.lfOrientation = 900;
		}
		else {
			DeleteFont(m_hfontVertical);
			m_hfontVertical = nullptr;
			redrawWindow();
			return;
		}

		if (m_hfontVertical)
			DeleteFont(m_hfontVertical);

		m_hfontVertical = CreateFontIndirect(&lfCurrent);
		//setControlFont(hfNew, FALSE);
		redrawWindow();
	}
	else
		parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setPosition( const int nNewPos ) {
  return SendMessage( m_Hwnd, PBM_SETPOS, (WPARAM) nNewPos, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setRange( const int iLowLim, const int iHighLim ) {
  return SendMessage(m_Hwnd, PBM_SETRANGE32, (WPARAM) iLowLim, (LPARAM) iHighLim );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::getPosition( ) const {
  return SendMessage( m_Hwnd, PBM_GETPOS, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::getRange( const BOOL fWhichLimit, PPBRANGE ppBRange ) const {
  return SendMessage( m_Hwnd, PBM_GETRANGE, (WPARAM) fWhichLimit, (LPARAM) ppBRange );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setMarquee( const BOOL fStart, const int fTime ) {
  return SendMessage( m_Hwnd, PBM_SETMARQUEE, (WPARAM) fStart, (LPARAM) fTime );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::stepIt( ) {
  return SendMessage( m_Hwnd, PBM_STEPIT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setStep( const int nStepInc ) {
  return SendMessage( m_Hwnd, PBM_SETSTEP, (WPARAM) nStepInc, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setBarColor( const COLORREF clrBar ) {
  m_clrStartGradient = clrBar;
  return SendMessage( m_Hwnd, PBM_SETBARCOLOR, (WPARAM) 0, (LPARAM) clrBar );
}
/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setBKColor( const COLORREF clrBk ) {
	m_clrEndGradient = clrBk;
	return SendMessage( m_Hwnd, PBM_SETBKCOLOR, (WPARAM) 0, (LPARAM) clrBk ); 
}

void DcxProgressBar::toXml(TiXmlElement *const xml) const {
	xml->SetAttribute("type", "pbar");
	__super::toXml(xml);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxProgressBar::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxProgressBar::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

		case WM_ERASEBKGND:
			{
				if (isExStyle(WS_EX_TRANSPARENT)) {
					DrawParentsBackground((HDC) wParam);
					bParsed = TRUE;
					return TRUE;
				}
			}
			break;

		case WM_PRINTCLIENT:
			{
				DrawClientArea((HDC)wParam, uMsg, lParam);
				bParsed = TRUE;
			}
			break;
		case WM_PAINT:
			{
				bParsed = TRUE;
				PAINTSTRUCT ps;

				auto hdc = BeginPaint(m_Hwnd, &ps);

				DrawClientArea(hdc, uMsg, lParam);

				EndPaint( m_Hwnd, &ps );
			}
			break;

		case WM_LBUTTONUP:
			{
				if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				{
					const auto iLower = getRange(TRUE, nullptr);
					const auto iHigher = getRange(FALSE, nullptr);

					execAliasEx(TEXT("sclick,%d,%d,%d,%d,%d"), getUserID(), getPredictedPos(lParam, iLower, iHigher), iLower, iHigher, getPosition());
				}
			}
			break;

		case WM_RBUTTONUP:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				{
					const auto iLower = getRange(TRUE, nullptr);
					const auto iHigher = getRange(FALSE, nullptr);

					execAliasEx(TEXT("rclick,%d,%d,%d,%d,%d"), getUserID(), getPredictedPos(lParam, iLower, iHigher), iLower, iHigher, getPosition());
				}
			}
			break;

		case WM_MOUSEMOVE:
			{
				this->m_pParentDialog->setMouseControl( this->getUserID( ) );

				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				{
					if (wParam == MK_LBUTTON) {
						const auto iLower = this->getRange(TRUE, nullptr);
						const auto iHigher = this->getRange(FALSE, nullptr);

						this->execAliasEx(TEXT("mousebar,%d,%d,%d,%d, %d"), getUserID(), getPredictedPos(lParam, iLower, iHigher), iLower, iHigher, getPosition());
					}
				}
			}
			break;

		case WM_DESTROY:
			{
				delete this;
				bParsed = TRUE;
			}
			break;

		default:
			return CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return 0L;
}


int DcxProgressBar::CalculatePosition() const {
	const auto iPos = getPosition();

	if ( m_bIsAbsoluteValue )
		return iPos;

	const auto iLower = getRange(TRUE, nullptr);
	const auto iHigher = getRange(FALSE, nullptr);

	return dcx_round((float) (iPos - iLower) * 100 / (iHigher - iLower ));
}

void DcxProgressBar::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	RECT rc;
	// A window may receive internal paint messages as a result of calling RedrawWindow with the RDW_INTERNALPAINT flag set.
	// In this case, the window may not have an update region.
	// An application should call the GetUpdateRect function to determine whether the window has an update region.
	// If GetUpdateRect returns zero, the application should not call the BeginPaint and EndPaint functions.
	//if (GetUpdateRect(m_Hwnd,&rc,FALSE) == 0) return;

	if (!GetClientRect(m_Hwnd, &rc))
		return;

	// Setup alpha blend if any.
	auto ai = SetupAlphaBlend(&hdc);
	Auto(FinishAlphaBlend(ai));

	if (m_bIsGrad)
	{
		DcxControl::DrawCtrlBackground(hdc, this, &rc);

		auto clrStart = m_clrStartGradient;
		auto clrEnd = m_clrEndGradient;

		if (clrEnd == CLR_INVALID)
			clrEnd = GetSysColor(COLOR_GRADIENTACTIVECAPTION);
		if (clrStart == CLR_INVALID)
			clrStart = XPopupMenuItem::DarkenColor(100,clrEnd);
		
		auto rcGrad = rc;

		if (isStyle(PBS_VERTICAL)) {
			rcGrad.top += (rc.bottom - rc.top) - (CalculatePosition() * (rc.bottom - rc.top)) / 100;
			XPopupMenuItem::DrawGradient( hdc, &rcGrad, clrStart, clrEnd, TRUE);
		}
		else {
			rcGrad.right = (CalculatePosition() * rc.right) / 100;
			XPopupMenuItem::DrawGradient( hdc, &rcGrad, clrStart , clrEnd , FALSE);
		}
	}
	else
		CallWindowProc(m_DefaultWindowProc, m_Hwnd, uMsg, (WPARAM) hdc, lParam);

	if (!m_tsText.empty()) {
		const auto oldMode = SetBkMode(hdc, TRANSPARENT);
		Auto(SetBkMode(hdc, oldMode));

		COLORREF oldColour = CLR_INVALID;

		if (m_clrText != CLR_INVALID)
			oldColour = SetTextColor(hdc, m_clrText);

		// used to calc text value on pbar
		TString text;
		const auto iPos = CalculatePosition();

		text.tsprintf(m_tsText.to_chr(), iPos);

		HFONT oldfont = nullptr;
// NEEDS FIXED: font selection needs looked at
		if (m_hFont != nullptr)
			oldfont = SelectFont(hdc, m_hFont);

		// rect for text
		auto rcText = rc;
		//DrawText(hdc, text.to_chr(), text.len(), &rcText, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_CALCRECT);
		calcTextRect(hdc, text, &rcText, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);

		const auto w = rcText.right - rcText.left;
		const auto h = rcText.bottom - rcText.top;

		// reposition the new text area to be at the center
		if (m_hfontVertical != nullptr) {
			rc.left = ((rc.right - rc.left) - h) /2;
			// added a +w +h as well to as text is drawn ABOVE the damn rect
			rc.top = ((rc.bottom - rc.top) + w + h) /2;
			rc.right = rc.left + h;
			rc.bottom = rc.top + w;
			SelectFont(hdc, m_hfontVertical);
		}
		else {
			rc.left = ((rc.right - rc.left) - w) /2;
			rc.top = ((rc.bottom - rc.top) - h) /2;
			rc.right = rc.left + w;
			rc.bottom = rc.top + h;
		}

		ctrlDrawText(hdc, text, &rc, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);

		if (oldfont != nullptr)
			SelectFont(hdc, oldfont);
		if (oldColour != CLR_INVALID)
			SetTextColor(hdc, oldColour);
	}
}
