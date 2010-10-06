/*!
 * \file dcxscroll.cpp
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
#include "Classes/mirc/dcxscroll.h"
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

DcxScroll::DcxScroll( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
, m_nPage(5)
, m_nLine(1)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles, 
		TEXT("SCROLLBAR"), 
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
		Dcx::XPPlusModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	SCROLLINFO si;
	si.cbSize = sizeof( SCROLLINFO );
	si.fMask = SIF_POS | SIF_RANGE;
	si.nPos = 0;
	si.nMin = 0;
	si.nMax = 100;
	SetScrollInfo( this->m_Hwnd, SB_CTL, &si, TRUE );

	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxScroll::~DcxScroll( ) {

	this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxScroll::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	unsigned int i = 1, numtok = styles.numtok( );

	while ( i <= numtok ) {

		if ( styles.gettok( i ) == TEXT("vertical") )
			*Styles |= SBS_VERT;

		i++;
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

void DcxScroll::parseInfoRequest( TString & input, PTCHAR szReturnValue ) {

//  int numtok = input.numtok( );

	TString prop(input.gettok( 3 ));

  // [NAME] [ID] [PROP]
  if ( prop == TEXT("value") ) {

    SCROLLINFO si;
    si.cbSize = sizeof( SCROLLINFO );
    si.fMask = SIF_POS;
    GetScrollInfo( this->m_Hwnd, SB_CTL, &si );

    wsprintf( szReturnValue, TEXT("%d"), si.nPos );
    return;
  }
  // [NAME] [ID] [PROP]
  else if ( prop == TEXT("range") ) {

    SCROLLINFO si;
    si.cbSize = sizeof( SCROLLINFO );
    si.fMask = SIF_RANGE;
    GetScrollInfo( this->m_Hwnd, SB_CTL, &si );
    wsprintf( szReturnValue, TEXT("%d %d"), si.nMin, si.nMax );
    return;
  }
  // [NAME] [ID] [PROP]
  else if ( prop == TEXT("line") ) {

    wsprintf( szReturnValue, TEXT("%d %d"), this->m_nLine );
    return;
  }
  // [NAME] [ID] [PROP]
  else if ( prop == TEXT("page") ) {

    wsprintf( szReturnValue, TEXT("%d"), this->m_nPage );
    return;
  }
  else if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
    return;

  szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxScroll::parseCommandRequest( TString & input ) {
	XSwitchFlags flags(input.gettok(3));
	int numtok = input.numtok( );

  //xdid -l [NAME] [ID] [SWITCH] [N]
  if ( flags[TEXT('l')] && numtok > 3 ) {

    int nLine = input.gettok( 4 ).to_int( );

    if ( nLine > 0 )
      this->m_nLine = nLine;
  }
  //xdid -m [NAME] [ID] [SWITCH] [N]
  else if ( flags[TEXT('m')] && numtok > 3 ) {

    int nPage = input.gettok( 4 ).to_int( );

    if ( nPage > 0 )
      this->m_nPage = nPage;
  }
  //xdid -r [NAME] [ID] [SWITCH] [L] [R]
  else if ( flags[TEXT('r')] && numtok > 4 ) {

    INT L = input.gettok( 4 ).to_int( );
    INT R = input.gettok( 5 ).to_int( );

    SCROLLINFO si;
    si.cbSize = sizeof( SCROLLINFO );
    si.fMask = SIF_RANGE;
    si.nMin = L;
    si.nMax = R;
    SetScrollInfo( this->m_Hwnd, SB_CTL, &si, TRUE );
  }
  //xdid -v [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags[TEXT('v')] && numtok > 3 ) {

    int pos = input.gettok( 4 ).to_int( );

    SCROLLINFO si;
    si.cbSize = sizeof( SCROLLINFO );
    si.fMask = SIF_POS;
    si.nPos = pos;
    SetScrollInfo( this->m_Hwnd, SB_CTL, &si, TRUE );
  }
  else
    this->parseGlobalCommandRequest( input, flags );
}

TString DcxScroll::getStyles(void) {
	TString styles(__super::getStyles());
	DWORD Styles;
	Styles = GetWindowStyle(this->m_Hwnd);
	if (Styles & SBS_VERT)
		styles.addtok(TEXT("vertical"));
	return styles;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxScroll::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) {
	switch (uMsg) {
		case WM_HSCROLL: 
		{
			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
			GetScrollInfo(this->m_Hwnd, SB_CTL, &si);

			switch (LOWORD(wParam)) {
				case SB_TOP:
				{
					si.nPos = si.nMin;
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("top"), this->getUserID( ), si.nPos );
					break;
				}

				case SB_BOTTOM:
				{
					si.nPos = si.nMax;
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("bottom"), this->getUserID( ), si.nPos );
					break;
				}

				//case SB_ENDTRACK:
				//  CallAliasEx( p_Dialog, ret, TEXT("%s,%d,%d"), TEXT("sclick"), 
				//               this->getUserID( ), p_DcxTrackBar->getPos( ) );
				//  break;

				case SB_PAGEUP:
				{
					if ((si.nPos - this->m_nPage) >= si.nMin)
						si.nPos -= this->m_nPage;
					else
						si.nPos = si.nMin;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("pageup"), this->getUserID(), si.nPos);
					break;
				}

				case SB_PAGEDOWN:
				{
					if (si.nPos + this->m_nPage <= si.nMax)
						si.nPos += this->m_nPage;
					else
						si.nPos = si.nMax;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("pagedown"), this->getUserID(), si.nPos);
					break;
				}

				case SB_LINEUP:
				{
					if (si.nPos - this->m_nLine >= si.nMin)
						si.nPos -= this->m_nLine;
					else
						si.nPos = si.nMin;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("lineup"), this->getUserID( ), si.nPos );
					break;
				}

				case SB_LINEDOWN:
				{
					if (si.nPos + this->m_nLine <= si.nMax)
						si.nPos += this->m_nLine;
					else
						si.nPos = si.nMax;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("linedown"), this->getUserID(), si.nPos);
					break;
				}

				case SB_THUMBPOSITION:
				{
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("trackend"), this->getUserID(), si.nPos);
					break;
				}

				case SB_THUMBTRACK:
				{
					si.nPos = si.nTrackPos;
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("tracking"), this->getUserID(), si.nTrackPos);
					break;
				}
			}

			bParsed = TRUE;
			si.fMask = SIF_POS;
			SetScrollInfo(this->m_Hwnd, SB_CTL, &si, TRUE);
			break;
		}

		case WM_VSCROLL:
		{
			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
			GetScrollInfo(this->m_Hwnd, SB_CTL, &si);

			switch (LOWORD(wParam)) {
				case SB_TOP:
				{
					si.nPos = si.nMin;
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("top"), this->getUserID(), si.nPos);
					break;
				}

				case SB_BOTTOM:
				{
					si.nPos = si.nMax;
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("bottom"), this->getUserID(), si.nPos);
					break;
				}

				//case SB_ENDTRACK:
				//  CallAliasEx( p_Dialog, ret, TEXT("%s,%d,%d"), TEXT("sclick"), 
				//               this->getUserID( ), p_DcxTrackBar->getPos( ) );
				//  break;

				case SB_PAGEUP:
				{
					if (si.nPos - this->m_nPage >= si.nMin)
						si.nPos -= this->m_nPage;
					else
						si.nPos = si.nMin;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("pageup"), this->getUserID(), si.nPos);
					break;
				}
				
				case SB_PAGEDOWN:
				{
					if (si.nPos + this->m_nPage <= si.nMax)
						si.nPos += this->m_nPage;
					else
						si.nPos = si.nMax;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("pagedown"), this->getUserID(), si.nPos);
					break;
				}
				
				case SB_LINEUP:
				{
					if (si.nPos - this->m_nLine >= si.nMin)
						si.nPos -= this->m_nLine;
					else
						si.nPos = si.nMin;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("lineup"), this->getUserID(), si.nPos);
					break;
				}

				case SB_LINEDOWN:
				{
					if ( si.nPos + this->m_nLine <= si.nMax )
						si.nPos += this->m_nLine;
					else
						si.nPos = si.nMax;

					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("linedown"), this->getUserID( ), si.nPos );
					break;
				}

				case SB_THUMBPOSITION:
				{
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("trackend"), this->getUserID(), si.nPos);
					break;
				}

				case SB_THUMBTRACK:
				{
					si.nPos = si.nTrackPos;
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("tracking"), this->getUserID(), si.nTrackPos);
					break;
				}
			}

			bParsed = TRUE;
			si.fMask = SIF_POS;
			SetScrollInfo(this->m_Hwnd, SB_CTL, &si, TRUE);
			break;
		}
	}

	return 0L;
}

LRESULT DcxScroll::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

		case WM_PAINT:
			{
				if (!this->m_bAlphaBlend)
					break;
				PAINTSTRUCT ps;
				HDC hdc;

				hdc = BeginPaint( this->m_Hwnd, &ps );

				bParsed = TRUE;

				// Setup alpha blend if any.
				LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

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
			}
			break;

		default:
			return this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return 0L;
}
