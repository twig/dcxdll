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

DcxProgressBar::DcxProgressBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, const TString & styles )
: DcxControl( ID, p_Dialog ) 
, m_clrText(0)
, m_bIsAbsoluteValue(FALSE)
, m_hfontVertical(NULL)
, m_bIsGrad(FALSE)
//, m_clrGrad(0)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CLIENTEDGE,
		DCX_PROGRESSBARCLASS,
		NULL,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw TEXT("Unable To Create Window");

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->m_tsText = TEXT("%d %%");

	if (styles.istok(TEXT("tooltips"))) {
		if (IsWindow(p_Dialog->getToolTip())) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
		else
			this->showError(NULL,TEXT("-c"),TEXT("Unable to Initialize Tooltips"));
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

TString DcxProgressBar::getStyles(void) const
{
	TString styles(__super::getStyles());
	DWORD Styles;
	Styles = GetWindowStyle(this->m_Hwnd);
	if (Styles & PBS_SMOOTH)
		styles.addtok(TEXT("smooth"));
	if (Styles & PBS_VERTICAL)
		styles.addtok(TEXT("vertical"));
	if (Styles & PBS_MARQUEE)
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
	const UINT numtok = styles.numtok( );
	this->m_bIsGrad = FALSE;

	for (UINT i = 1;  i <= numtok; i++ )
	{
		if ( styles.gettok( i ) == TEXT("smooth") ) 
			*Styles |= PBS_SMOOTH;
		else if ( styles.gettok( i ) == TEXT("vertical") ) 
			*Styles |= PBS_VERTICAL;
		else if ( styles.gettok( i ) == TEXT("marquee") ) 
			*Styles |= PBS_MARQUEE;
		else if ( styles.gettok( i ) == TEXT("gradient") ) {
			*Styles |= PBS_SMOOTH;
			this->m_bIsGrad = TRUE;
		}
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
	const TString prop(input.gettok( 3 ));

	if ( prop == TEXT("value") ) {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getPosition( ) );
		return;
	}
	else if ( prop == TEXT("range") ) {
		PBRANGE pbr;
		this->getRange( FALSE, &pbr );
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), pbr.iLow, pbr.iHigh );
		return;
	}
	else if (prop == TEXT("text")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, this->m_tsText.to_chr(), this->CalculatePosition());
		return;
	}
	else if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
		return;

	szReturnValue[0] = 0;
}
/*!
 * \brief blah
 *
 * \param input [NAME] [SWITCH] [ID] (OPTIONS)
 */

void DcxProgressBar::parseCommandRequest( const TString &input) {
	const XSwitchFlags flags(input.gettok(3));
	const int numtok = input.numtok();

	// xdid -c name ID $rgb(color)
	if (flags[TEXT('c')]) {
		this->setBarColor((COLORREF) input.gettok( 4 ).to_num());
	}
	//// xdid -g name ID [1|0]
	//else if ( flags[TEXT('g')] ) {
	//this->m_bIsGrad = (BOOL) input.gettok( 4 ).to_num( );
	//}
	// xdid -i name ID (TEXT)
	else if (flags[TEXT('i')]) {
		if (input.numtok( ) > 3)
			this->m_tsText = input.gettok(4, -1);
		else
			this->m_tsText = TEXT("");

		this->redrawWindow();
	}
	// xdid -j name ID [a|p]
	else if (flags[TEXT('j')]) {
		if (input.gettok( 4 ) == TEXT('a'))
			this->m_bIsAbsoluteValue = TRUE;
		else
			this->m_bIsAbsoluteValue = FALSE;

		this->redrawWindow();
	}
	// xdid -k name ID $rgb(color)
	else if (flags[TEXT('k')]) {
		this->setBKColor((COLORREF) input.gettok( 4 ).to_num());
	}
	// xdid -m(o|g) name ID N
	else if (flags[TEXT('m')]) {
		// -mo
		if (flags[TEXT('o')])
			this->setMarquee(TRUE, (int)input.gettok( 4 ).to_num());
		// -mg
		else if (flags[TEXT('g')])
			this->setMarquee(FALSE, 0);
	}
	// xdid -q name ID [COLOR]
	else if ( flags[TEXT('q')] ) {
		this->m_clrText = (COLORREF) input.gettok( 4 ).to_num();
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
		this->setStep(input.gettok( 4 ).to_int());
	}
	// xdid -v name ID N
	else if (flags[TEXT('v')]) {
		if (numtok > 3)
			this->setPosition(input.gettok( 4 ).to_int());
	}
	// xdid [-o] [NAME] [ID] [ENABLED]
	// vertical fonts [1|0]
	else if (flags[TEXT('o')]) {
		if (numtok < 4)
			return;

		LOGFONT lfCurrent;
		ZeroMemory(&lfCurrent, sizeof(LOGFONT));

		GetObject(this->m_hFont, sizeof(LOGFONT), &lfCurrent);
		int angle = input.gettok( 4 ).to_int();

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
			this->m_hfontVertical = NULL;
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

void DcxProgressBar::toXml(TiXmlElement * xml) const {
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
				HDC hdc;

				hdc = BeginPaint( this->m_Hwnd, &ps );

				this->DrawClientArea(hdc, uMsg, lParam);

				EndPaint( this->m_Hwnd, &ps );
			}
			break;

		case WM_LBUTTONUP:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
					int nXPos = LOWORD(lParam);
					int iLower = this->getRange( TRUE, NULL );
					int iHigher = this->getRange( FALSE, NULL );

					RECT rc;
					GetClientRect( this->m_Hwnd, &rc );

					int nPos = iLower + round( (float)( nXPos * iHigher ) / ( rc.right - rc.left - 1 ) );

					this->execAliasEx(TEXT("%s,%d,%d,%d,%d,%d"), TEXT("sclick"), this->getUserID(), nPos, iLower, iHigher, this->getPosition());
				}
			}
			break;

		case WM_RBUTTONUP:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
					int nXPos = LOWORD(lParam);
					int iLower = this->getRange( TRUE, NULL );
					int iHigher = this->getRange( FALSE, NULL );

					RECT rc;
					GetClientRect( this->m_Hwnd, &rc );

					int nPos = iLower + round( (float)( nXPos * iHigher ) / ( rc.right - rc.left - 1 ) );

					this->execAliasEx(TEXT("%s,%d,%d,%d,%d,%d"), TEXT("rclick"), this->getUserID(), nPos, iLower, iHigher, this->getPosition());
				}
			}
			break;

		case WM_MOUSEMOVE:
			{
				this->m_pParentDialog->setMouseControl( this->getUserID( ) );

				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
					if ( wParam == MK_LBUTTON ) {

						int nXPos = LOWORD(lParam);
						int iLower = this->getRange( TRUE, NULL );
						int iHigher = this->getRange( FALSE, NULL );

						RECT rc;
						GetClientRect( this->m_Hwnd, &rc );

						int nPos = iLower + (int)( (float)( nXPos * iHigher ) / ( rc.right - rc.left - 1 ) );

						this->execAliasEx(TEXT("%s,%d,%d,%d,%d, %d"), TEXT("mousebar"), this->getUserID(), nPos, iLower, iHigher, this->getPosition());
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
	int iPos = this->getPosition();

	if ( this->m_bIsAbsoluteValue )
		return iPos;
	else {
		int iLower = this->getRange( TRUE, NULL );
		int iHigher = this->getRange( FALSE, NULL );

		return (int) ((float) (iPos - iLower) * 100 / (iHigher - iLower ));
	}
}

void DcxProgressBar::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	RECT rc;
	// A window may receive internal paint messages as a result of calling RedrawWindow with the RDW_INTERNALPAINT flag set.
	// In this case, the window may not have an update region.
	// An application should call the GetUpdateRect function to determine whether the window has an update region.
	// If GetUpdateRect returns zero, the application should not call the BeginPaint and EndPaint functions.
	//if (GetUpdateRect(this->m_Hwnd,&rc,FALSE) == 0) return;

	// Setup alpha blend if any.
	LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

	if (this->m_bIsGrad) {
		GetClientRect(this->m_Hwnd, &rc);

		DcxControl::DrawCtrlBackground(hdc, this, &rc);

		COLORREF clrStart = this->m_clrStartGradient;
		COLORREF clrEnd = this->m_clrEndGradient;

		if (clrEnd == CLR_INVALID)
			clrEnd = GetSysColor(COLOR_GRADIENTACTIVECAPTION);
		if (clrStart == CLR_INVALID)
			clrStart = XPopupMenuItem::DarkenColor(100,clrEnd);

		if (this->isStyle(PBS_VERTICAL)) {
			rc.top += (rc.bottom - rc.top) - (this->CalculatePosition() * (rc.bottom - rc.top)) / 100;
			//XPopupMenuItem::DrawGradient(hdc,&rc,XPopupMenuItem::DarkenColor(100,this->m_clrGrad),this->m_clrGrad,TRUE);
			XPopupMenuItem::DrawGradient( hdc, &rc, clrStart, clrEnd, TRUE);
		}
		else {
			rc.right = (this->CalculatePosition() * rc.right) / 100;
			//XPopupMenuItem::DrawGradient(hdc,&rc,this->m_clrGrad,XPopupMenuItem::DarkenColor(100,this->m_clrGrad),FALSE);
			XPopupMenuItem::DrawGradient( hdc, &rc, clrStart , clrEnd , FALSE);
		}
	}
	else
		CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam);

	if (this->m_tsText.len() > 0) {
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, this->m_clrText);

		// rect for control
		//RECT rc;
		GetClientRect(this->m_Hwnd, &rc);

		// used to calc text value on pbar
		TString text;
		int iPos = this->CalculatePosition();

		text.tsprintf(this->m_tsText.to_chr(), iPos);

		HFONT oldfont = NULL;
// NEEDS FIXED: font selection needs looked at
		if (this->m_hFont != NULL)
			oldfont = SelectFont(hdc, this->m_hFont);

		// rect for text
		RECT rcText = rc;
		//DrawText(hdc, text.to_chr(), text.len(), &rcText, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_CALCRECT);
		this->calcTextRect(hdc, text, &rcText, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);

		int w = rcText.right - rcText.left;
		int h = rcText.bottom - rcText.top;

		// reposition the new text area to be at the center
		if (this->m_hfontVertical != NULL) {
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

		//if (!this->m_bCtrlCodeText)
		//	DrawTextW(hdc, text.to_wchr(this->m_bUseUTF8), text.wlen(), &rc, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);
		//else
		//	mIRC_DrawText(hdc, text, &rc, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP, this->m_bShadowText, this->m_bUseUTF8);
		this->ctrlDrawText(hdc, text, &rc, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);

		if (oldfont != NULL)
			SelectFont(hdc, oldfont);
	}

	this->FinishAlphaBlend(ai);
}
