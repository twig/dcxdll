/*!
 * \file dcxradio.cpp
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

#include "dcxradio.h"
#include "../dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxRadio::DcxRadio( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles, 
		"BUTTON", 
		NULL,
		WS_CHILD | Styles, 
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL), 
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw "Unable To Create Window";

	if ( bNoTheme )
		dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->m_bNoTheme = (bNoTheme ? true : false);

	if (p_Dialog->getToolTip() != NULL) {
		if (styles.istok("tooltips")) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
	}
	this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxRadio::~DcxRadio( ) {

	this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxRadio::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	unsigned int i = 1, numtok = styles.numtok( );
	*Styles |= BS_AUTORADIOBUTTON;

	while ( i <= numtok ) {

		if ( styles.gettok( i ) == "rjustify" )
			*Styles |= BS_RIGHT;
		else if ( styles.gettok( i ) == "center" )
			*Styles |= BS_CENTER;
		else if ( styles.gettok( i ) == "ljustify" )
			*Styles |= BS_LEFT;
		else if ( styles.gettok( i ) == "right" )
			*Styles |= BS_RIGHTBUTTON;
		else if ( styles.gettok( i ) == "pushlike" )
			*Styles |= BS_PUSHLIKE;
		else if ( styles.gettok( i ) == "alpha" )
			this->m_bAlphaBlend = true;
		else if ( styles.gettok( i ) == "hgradient" )
			this->m_bGradientFill = true;
		else if ( styles.gettok( i ) == "vgradient" ) {
			this->m_bGradientFill = true;
			this->m_bGradientVertical = TRUE;
		}

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

void DcxRadio::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( );

	TString prop(input.gettok( 3 ));

  // [NAME] [ID] [PROP]
  if ( prop == "text" ) {

    GetWindowText( this->m_Hwnd, szReturnValue, 900 );
    return;
  }
  // [NAME] [ID] [PROP]
  else if ( prop == "state" ) {

    if ( Button_GetCheck( this->m_Hwnd ) & BST_CHECKED )
      lstrcpy( szReturnValue, "1" );
    else
      lstrcpy( szReturnValue, "0" );

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

void DcxRadio::parseCommandRequest( TString & input ) {
	XSwitchFlags flags(input.gettok(3));
	int numtok = input.numtok( );

	//xdid -c [NAME] [ID] [SWITCH]
	if ( flags['c'] ) {
		Button_SetCheck( this->m_Hwnd, BST_CHECKED );
	}
	//xdid -t [NAME] [ID] [SWITCH]
	else if ( flags['t'] && numtok > 3 ) {
		TString text(input.gettok( 4, -1 ));
		text.trim( );
		SetWindowText( this->m_Hwnd, text.to_chr( ) );
	}
	//xdid -u [NAME] [ID] [SWITCH]
	else if ( flags['u'] ) {
		Button_SetCheck( this->m_Hwnd, BST_UNCHECKED );
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxRadio::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
 // switch( uMsg ) {
 //   case WM_COMMAND:
 //     {
 //       switch ( HIWORD( wParam ) ) {

 //         case BN_CLICKED:
 //           {
	//						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
	//			        this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID( ) );
 //           }
 //           break;
 //       }
 //     }
 //     break;
	//}
	return 0L;
}

LRESULT DcxRadio::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

		case WM_ERASEBKGND:
			{
				bParsed = TRUE;
				return TRUE;
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

				this->DrawClientArea( hdc, uMsg, lParam);

				EndPaint( this->m_Hwnd, &ps );
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

void DcxRadio::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	// Setup alpha blend if any.
	LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

	if (this->m_bNoTheme || !dcxIsThemeActive()) {
		if (this->m_clrBackText != -1)
			SetBkColor(hdc, this->m_clrBackText);

		if (this->m_clrText != -1)
			SetTextColor(hdc, this->m_clrText);

		RECT rcClient;

		// get controls client area
		GetClientRect( this->m_Hwnd, &rcClient );

		bool bWasTransp = false;
		if (this->isExStyle(WS_EX_TRANSPARENT))
			bWasTransp = true;

		// fill background.
		if (bWasTransp)
		{
			if (!this->m_bAlphaBlend)
				this->DrawParentsBackground(hdc,&rcClient);
		}
		else
			DcxControl::DrawCtrlBackground(hdc,this,&rcClient);

		if (!bWasTransp)
			AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT);

		CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

		if (!bWasTransp)
			RemStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT);
	}
	else
		CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

	this->FinishAlphaBlend(ai);
}
