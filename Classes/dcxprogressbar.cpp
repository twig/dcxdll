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
//, m_clrText(0)
//, m_clrGrad(0)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CLIENTEDGE,
		DCX_PROGRESSBARCLASS,
		nullptr,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(this->m_Hwnd))
		throw std::runtime_error("Unable To Create Window");

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->m_tsText = TEXT("%d %%");

	if (styles.istok(TEXT("tooltips"))) {
		if (!IsWindow(p_Dialog->getToolTip()))
			throw std::runtime_error("Unable to Initialize Tooltips");
		
		this->m_ToolTipHWND = p_Dialog->getToolTip();
		AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
	}
	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
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
	const auto Styles = GetWindowStyle(this->m_Hwnd);

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

#if TSTRING_PARTS
	for (const auto &tsStyle: styles)
	{
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
	}
#else
	for (auto tsStyle(styles.getfirsttok(1)); !tsStyle.empty(); tsStyle = styles.getnexttok())
	{
		if ( tsStyle == TEXT("smooth") ) 
			*Styles |= PBS_SMOOTH;
		else if ( tsStyle == TEXT("vertical") ) 
			*Styles |= PBS_VERTICAL;
		else if ( tsStyle == TEXT("marquee") ) 
			*Styles |= PBS_MARQUEE;
		else if ( tsStyle == TEXT("gradient") ) {
			*Styles |= PBS_SMOOTH;
			this->m_bIsGrad = true;
		}
}
#endif
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
		this->setBarColor((COLORREF) input.getnexttok( ).to_num());	// tok 4
	}
	//// xdid -g name ID [1|0]
	//else if ( flags[TEXT('g')] ) {
	//this->m_bIsGrad = input.gettok( 4 ).to_<bool>( );
	//}
	// xdid -i name ID (TEXT)
	else if (flags[TEXT('i')]) {
		if (numtok > 3)
			this->m_tsText = input.getlasttoks();	// tok 4, -1
		else
			this->m_tsText.clear();	// = TEXT("");

		this->redrawWindow();
	}
	// xdid -j name ID [a|p]
	else if (flags[TEXT('j')]) {
		this->m_bIsAbsoluteValue = (input.getnexttok() == TEXT('a'));

		this->redrawWindow();
	}
	// xdid -k name ID $rgb(color)
	else if (flags[TEXT('k')]) {
		this->setBKColor((COLORREF) input.getnexttok( ).to_num());	// tok 4
	}
	// xdid -m(o|g) name ID N
	else if (flags[TEXT('m')]) {
		// -mo
		if (flags[TEXT('o')])
			this->setMarquee(TRUE, input.getnexttok( ).to_int());	// tok 4
		// -mg
		else if (flags[TEXT('g')])
			this->setMarquee(FALSE, 0);
	}
	// xdid -q name ID [COLOR]
	else if ( flags[TEXT('q')] ) {
		this->m_clrText = (COLORREF) input.getnexttok( ).to_num();	// tok 4
		this->redrawWindow();
	}
	// xdid -r name ID RLow RHigh
	else if (flags[TEXT('r')]) {
		if (numtok > 4)
			this->setRange(input.gettok( 4 ).to_int(), input.gettok( 5 ).to_int());
	}
	// xdid -t name ID
	else if (flags[TEXT('t')]) {
		this->stepIt();
	}
	// xdid -u name ID N
	else if (flags[TEXT('u')]) {
		this->setStep(input.getnexttok( ).to_int());	// tok 4
	}
	// xdid -v name ID N
	else if (flags[TEXT('v')]) {
		if (numtok > 3)
			this->setPosition(input.getnexttok( ).to_int());	// tok 4
	}
	// xdid [-o] [NAME] [ID] [ENABLED]
	// vertical fonts [1|0]
	else if (flags[TEXT('o')]) {
		if (numtok < 4)
			return;

		LOGFONT lfCurrent;
		ZeroMemory(&lfCurrent, sizeof(LOGFONT));

		if (GetObject(this->m_hFont, sizeof(LOGFONT), &lfCurrent) == 0)
			throw std::runtime_error("Unable to get LOGFONT");

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
			DeleteFont(this->m_hfontVertical);
			this->m_hfontVertical = nullptr;
			this->redrawWindow();
			return;
		}

		if (this->m_hfontVertical)
			DeleteFont(this->m_hfontVertical);

		this->m_hfontVertical = CreateFontIndirect(&lfCurrent);
		//this->setControlFont(hfNew, FALSE);
		this->redrawWindow();
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setPosition( const int nNewPos ) {
  return SendMessage( this->m_Hwnd, PBM_SETPOS, (WPARAM) nNewPos, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setRange( const int iLowLim, const int iHighLim ) {
  return SendMessage(this->m_Hwnd, PBM_SETRANGE32, (WPARAM) iLowLim, (LPARAM) iHighLim );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::getPosition( ) const {
  return SendMessage( this->m_Hwnd, PBM_GETPOS, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::getRange( const BOOL fWhichLimit, PPBRANGE ppBRange ) const {
  return SendMessage( this->m_Hwnd, PBM_GETRANGE, (WPARAM) fWhichLimit, (LPARAM) ppBRange );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setMarquee( const BOOL fStart, const int fTime ) {
  return SendMessage( this->m_Hwnd, PBM_SETMARQUEE, (WPARAM) fStart, (LPARAM) fTime );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::stepIt( ) {
  return SendMessage( this->m_Hwnd, PBM_STEPIT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setStep( const int nStepInc ) {
  return SendMessage( this->m_Hwnd, PBM_SETSTEP, (WPARAM) nStepInc, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setBarColor( const COLORREF clrBar ) {
  this->m_clrStartGradient = clrBar;
  return SendMessage( this->m_Hwnd, PBM_SETBARCOLOR, (WPARAM) 0, (LPARAM) clrBar );
}
/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxProgressBar::setBKColor( const COLORREF clrBk ) {
	this->m_clrEndGradient = clrBk;
	return SendMessage( this->m_Hwnd, PBM_SETBKCOLOR, (WPARAM) 0, (LPARAM) clrBk ); 
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
				if (this->isExStyle(WS_EX_TRANSPARENT)) {
					this->DrawParentsBackground((HDC) wParam);
					bParsed = TRUE;
					return TRUE;
				}
			}
			break;

		case WM_PRINTCLIENT:
			{
				this->DrawClientArea((HDC)wParam, uMsg, lParam);
				bParsed = TRUE;
			}
			break;
		case WM_PAINT:
			{
				bParsed = TRUE;
				PAINTSTRUCT ps;

				auto hdc = BeginPaint(this->m_Hwnd, &ps);

				this->DrawClientArea(hdc, uMsg, lParam);

				EndPaint( this->m_Hwnd, &ps );
			}
			break;

		case WM_LBUTTONUP:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				{
					const auto iLower = this->getRange(TRUE, nullptr);
					const auto iHigher = this->getRange(FALSE, nullptr);

					this->execAliasEx(TEXT("%s,%d,%d,%d,%d,%d"), TEXT("sclick"), this->getUserID(), this->getPredictedPos(lParam, iLower, iHigher), iLower, iHigher, this->getPosition());
				}
			}
			break;

		case WM_RBUTTONUP:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				{
					const auto iLower = this->getRange(TRUE, nullptr);
					const auto iHigher = this->getRange(FALSE, nullptr);

					this->execAliasEx(TEXT("%s,%d,%d,%d,%d,%d"), TEXT("rclick"), this->getUserID(), this->getPredictedPos(lParam, iLower, iHigher), iLower, iHigher, this->getPosition());
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

						this->execAliasEx(TEXT("%s,%d,%d,%d,%d, %d"), TEXT("mousebar"), this->getUserID(), this->getPredictedPos(lParam, iLower, iHigher), iLower, iHigher, this->getPosition());
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
			return this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return 0L;
}


int DcxProgressBar::CalculatePosition() const {
	const auto iPos = this->getPosition();

	if ( this->m_bIsAbsoluteValue )
		return iPos;

	const auto iLower = this->getRange(TRUE, nullptr);
	const auto iHigher = this->getRange(FALSE, nullptr);

	return dcx_round((float) (iPos - iLower) * 100 / (iHigher - iLower ));
}

void DcxProgressBar::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	RECT rc;
	// A window may receive internal paint messages as a result of calling RedrawWindow with the RDW_INTERNALPAINT flag set.
	// In this case, the window may not have an update region.
	// An application should call the GetUpdateRect function to determine whether the window has an update region.
	// If GetUpdateRect returns zero, the application should not call the BeginPaint and EndPaint functions.
	//if (GetUpdateRect(this->m_Hwnd,&rc,FALSE) == 0) return;

	if (!GetClientRect(this->m_Hwnd, &rc))
		return;

	// Setup alpha blend if any.
	auto ai = this->SetupAlphaBlend(&hdc);

	if (this->m_bIsGrad)
	{
		DcxControl::DrawCtrlBackground(hdc, this, &rc);

		auto clrStart = this->m_clrStartGradient;
		auto clrEnd = this->m_clrEndGradient;

		if (clrEnd == CLR_INVALID)
			clrEnd = GetSysColor(COLOR_GRADIENTACTIVECAPTION);
		if (clrStart == CLR_INVALID)
			clrStart = XPopupMenuItem::DarkenColor(100,clrEnd);
		
		auto rcGrad = rc;

		if (this->isStyle(PBS_VERTICAL)) {
			rcGrad.top += (rc.bottom - rc.top) - (this->CalculatePosition() * (rc.bottom - rc.top)) / 100;
			XPopupMenuItem::DrawGradient( hdc, &rcGrad, clrStart, clrEnd, TRUE);
		}
		else {
			rcGrad.right = (this->CalculatePosition() * rc.right) / 100;
			XPopupMenuItem::DrawGradient( hdc, &rcGrad, clrStart , clrEnd , FALSE);
		}
	}
	else
		CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam);

	if (!this->m_tsText.empty()) {
		const auto oldMode = SetBkMode(hdc, TRANSPARENT);
		COLORREF oldColour = CLR_INVALID;

		if (this->m_clrText != CLR_INVALID)
			oldColour = SetTextColor(hdc, this->m_clrText);

		// used to calc text value on pbar
		TString text;
		const auto iPos = this->CalculatePosition();

		text.tsprintf(this->m_tsText.to_chr(), iPos);

		HFONT oldfont = nullptr;
// NEEDS FIXED: font selection needs looked at
		if (this->m_hFont != nullptr)
			oldfont = SelectFont(hdc, this->m_hFont);

		// rect for text
		auto rcText = rc;
		//DrawText(hdc, text.to_chr(), text.len(), &rcText, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_CALCRECT);
		this->calcTextRect(hdc, text, &rcText, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);

		const auto w = rcText.right - rcText.left;
		const auto h = rcText.bottom - rcText.top;

		// reposition the new text area to be at the center
		if (this->m_hfontVertical != nullptr) {
			rc.left = ((rc.right - rc.left) - h) /2;
			// added a +w +h as well to as text is drawn ABOVE the damn rect
			rc.top = ((rc.bottom - rc.top) + w + h) /2;
			rc.right = rc.left + h;
			rc.bottom = rc.top + w;
			SelectFont(hdc, this->m_hfontVertical);
		}
		else {
			rc.left = ((rc.right - rc.left) - w) /2;
			rc.top = ((rc.bottom - rc.top) - h) /2;
			rc.right = rc.left + w;
			rc.bottom = rc.top + h;
		}

		this->ctrlDrawText(hdc, text, &rc, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);

		if (oldfont != nullptr)
			SelectFont(hdc, oldfont);
		if (oldColour != CLR_INVALID)
			SetTextColor(hdc, oldColour);
		SetBkMode(hdc, oldMode);
	}

	this->FinishAlphaBlend(ai);
}
