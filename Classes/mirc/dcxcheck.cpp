/*!
 * \file dcxcheck.cpp
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * � ScriptsDB.org - 2006
 */
#include "defines.h"
#include "Classes/mirc/dcxcheck.h"
#include "Classes/dcxdialog.h"
#include "Dcx.h"



/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxCheck::DcxCheck( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, const TString & styles ) 
: DcxControl( ID, p_Dialog )
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles, 
		TEXT("BUTTON"), 
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

	this->m_bNoTheme = (bNoTheme ? true : false);

	if (p_Dialog->getToolTip() != NULL) {
		if (styles.istok(TEXT("tooltips"))) {

			this->m_ToolTipHWND = p_Dialog->getToolTip();

			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
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

DcxCheck::~DcxCheck( ) {
	this->unregistreDefaultWindowProc( );
}


void DcxCheck::toXml(TiXmlElement * xml) const {
	TString wtext;
	__super::toXml(xml);
	TGetWindowText(this->m_Hwnd, wtext);
	xml->SetAttribute("caption", wtext.c_str());
}

TString DcxCheck::getStyles(void) const {
	TString styles(__super::getStyles());
	const DWORD Styles = GetWindowStyle(this->m_Hwnd);
	if (Styles & BS_RIGHT)
		styles.addtok(TEXT("rjustify"));
	if (Styles & BS_CENTER)
		styles.addtok(TEXT("center"));
	if (Styles & BS_LEFT)
		styles.addtok(TEXT("ljustify"));
	if (Styles & BS_RIGHTBUTTON)
		styles.addtok(TEXT("right"));
	if (Styles & BS_PUSHLIKE)
		styles.addtok(TEXT("pushlike"));
	if (Styles & BS_AUTO3STATE)
		styles.addtok(TEXT("3state"));
	return styles;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxCheck::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	*Styles |= BS_AUTOCHECKBOX;

	//const UINT numtok = styles.numtok( );
	//styles.getfirsttok( 0 );
	//for (UINT i = 1; i <= numtok; i++)
	//{
	//	const TString tsStyle(styles.getnexttok( ));	// tok i

	//	if ( tsStyle == TEXT("rjustify") )
	//		*Styles |= BS_RIGHT;
	//	else if ( tsStyle == TEXT("center") )
	//		*Styles |= BS_CENTER;
	//	else if ( tsStyle == TEXT("ljustify") )
	//		*Styles |= BS_LEFT;
	//	else if ( tsStyle == TEXT("right") )
	//		*Styles |= BS_RIGHTBUTTON;
	//	else if ( tsStyle == TEXT("pushlike") )
	//		*Styles |= BS_PUSHLIKE;
	//	else if ( tsStyle == TEXT("3state") ) {
	//		*Styles &= ~BS_AUTOCHECKBOX;
	//		*Styles |= BS_AUTO3STATE;
	//	}
	//}
	for (TString tsStyle(styles.getfirsttok( 1 )); tsStyle != TEXT(""); tsStyle = styles.getnexttok( ))
	{
		if ( tsStyle == TEXT("rjustify") )
			*Styles |= BS_RIGHT;
		else if ( tsStyle == TEXT("center") )
			*Styles |= BS_CENTER;
		else if ( tsStyle == TEXT("ljustify") )
			*Styles |= BS_LEFT;
		else if ( tsStyle == TEXT("right") )
			*Styles |= BS_RIGHTBUTTON;
		else if ( tsStyle == TEXT("pushlike") )
			*Styles |= BS_PUSHLIKE;
		else if ( tsStyle == TEXT("3state") ) {
			*Styles &= ~BS_AUTOCHECKBOX;
			*Styles |= BS_AUTO3STATE;
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

void DcxCheck::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	const TString prop(input.getfirsttok( 3 ));

	// [NAME] [ID] [PROP]
	if ( prop == TEXT("text") ) {

		GetWindowText( this->m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH );
		return;
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("state") ) {

		if ( Button_GetCheck( this->m_Hwnd ) & BST_INDETERMINATE )
			lstrcpyn( szReturnValue, TEXT("2"), MIRC_BUFFER_SIZE_CCH );
		else if ( Button_GetCheck( this->m_Hwnd ) & BST_CHECKED )
			lstrcpyn( szReturnValue, TEXT("1"), MIRC_BUFFER_SIZE_CCH );
		else
			lstrcpyn( szReturnValue, TEXT("0"), MIRC_BUFFER_SIZE_CCH );

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

void DcxCheck::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));

	//xdid -c [NAME] [ID] [SWITCH]
	if (flags[TEXT('c')]) {
		// xdid -cu
		if (flags[TEXT('u')])
			Button_SetCheck(this->m_Hwnd, BST_INDETERMINATE);
		else
			Button_SetCheck(this->m_Hwnd, BST_CHECKED);
	}
	//xdid -t [NAME] [ID] [SWITCH] ItemText
	else if (flags[TEXT('t')]) {
		SetWindowText(this->m_Hwnd, input.gettok(4, -1).trim().to_chr());
	}
  //xdid -u [NAME] [ID] [SWITCH]
  else if ( flags[TEXT('u')] ) {

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
LRESULT DcxCheck::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
   switch (uMsg)
   {
      case WM_COMMAND:
      {
         switch (HIWORD(wParam))
         {
            // catch this so we can use $xdid(checkbox).state in sclick callback
            case BN_CLICKED:
            {
               if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
               {
                  //this->execAliasEx(TEXT("%s,%d"), TEXT("sclick"), this->getUserID());

                  // /.timer repetitions delay alias dialog event id
                  Dcx::mIRC.execex(TEXT("/.timer 1 0 %s %s %s %d"),
                     this->m_pParentDialog->getAliasName().to_chr(),
                     this->m_pParentDialog->getName().to_chr(),
                     TEXT("sclick"),
                     this->getUserID());
               }

               break;
            }
         }

         break;
      }
   }

	return 0L;
}
LRESULT DcxCheck::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

		case WM_LBUTTONUP:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->execAliasEx(TEXT("%s,%d"), TEXT("lbup"), this->getUserID( ) );
			}
			break;

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
				if (GetUpdateRect( this->m_Hwnd, NULL, NULL)) {
					PAINTSTRUCT ps;
					HDC hdc = BeginPaint( this->m_Hwnd, &ps );

					this->DrawClientArea( hdc, uMsg, lParam);

					EndPaint( this->m_Hwnd, &ps );
				}
				else {
					HDC hdc = GetDC(this->m_Hwnd);

					this->DrawClientArea( hdc, uMsg, lParam);

					ReleaseDC(this->m_Hwnd, hdc);
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

void DcxCheck::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	// Setup alpha blend if any.
	LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

	if (this->m_bNoTheme || !Dcx::UXModule.dcxIsThemeActive()) {
		if (this->m_clrBackText != CLR_INVALID)
			SetBkColor(hdc, this->m_clrBackText);

		if (this->m_clrText != CLR_INVALID)
			SetTextColor(hdc, this->m_clrText);

		RECT rcClient;

		// get controls client area
		GetClientRect( this->m_Hwnd, &rcClient );

		const BOOL bWasTransp = this->isExStyle(WS_EX_TRANSPARENT);

		// fill background.
		if (bWasTransp)
		{
			if (!this->m_bAlphaBlend)
				this->DrawParentsBackground(hdc,&rcClient);
		}
		else
			DcxControl::DrawCtrlBackground(hdc,this,&rcClient);

		// This is a workaround to allow our background to be seen under the control.
		if (!bWasTransp)
			AddStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT);

		CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

		if (!bWasTransp)
			RemStyles(this->m_Hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT);
	}
	else
		CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, WM_PRINTCLIENT, (WPARAM) hdc, PRF_NONCLIENT|PRF_CLIENT|PRF_CHILDREN );
		//CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

	this->FinishAlphaBlend(ai);
}
