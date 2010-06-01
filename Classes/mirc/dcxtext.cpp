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

DcxText::DcxText( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(
		ExStyles,
		"STATIC",
		NULL,
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw "Unable To Create Window";

	// remove all borders
	this->removeStyle( WS_BORDER|WS_DLGFRAME );
	this->removeExStyle( WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME|WS_EX_STATICEDGE|WS_EX_WINDOWEDGE );

	if ( bNoTheme )
		Dcx::XPPlusModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	//this->m_tsText = ""; // pointless, alrdy is "".
	this->m_clrText = GetSysColor(COLOR_WINDOWTEXT);

	this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );

	if (styles.istok("tooltips")) {
		if (IsWindow(p_Dialog->getToolTip())) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
		else
			this->showError(NULL,"-c","Unable to Initialize Tooltips");
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

void DcxText::parseControlStyles(TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) {
	unsigned int i = 1, numtok = styles.numtok( );
	*Styles |= SS_NOTIFY;

	while (i <= numtok) {
		if (styles.gettok( i ) == "nowrap")
			*Styles |= SS_LEFTNOWORDWRAP;
		else if (styles.gettok( i ) == "center")
			*Styles |= SS_CENTER;
		else if (styles.gettok( i ) == "right")
			*Styles |= SS_RIGHT;
		else if (styles.gettok( i ) == "noprefix")
			*Styles |= SS_NOPREFIX;
		else if (styles.gettok( i ) == "endellipsis")
			*Styles |= SS_ENDELLIPSIS;
		else if (styles.gettok( i ) == "pathellipsis")
			*Styles |= SS_PATHELLIPSIS;

		i++;
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

void DcxText::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( );

  // [NAME] [ID] [PROP]
  if ( input.gettok( 3 ) == "text" ) {

    GetWindowText( this->m_Hwnd, szReturnValue, 900 );
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

void DcxText::parseCommandRequest(TString &input) {
	XSwitchFlags flags(input.gettok(3));
	int numtok = input.numtok( );

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags['r']) {
		this->m_tsText = "";
		SetWindowText(this->m_Hwnd, "");
	}

	// xdid -a [NAME] [ID] [SPACE 0|1] [TEXT]
	if (flags['a'] && numtok > 2) {
		if (input.gettok(4).to_int() == 1)
			this->m_tsText += " ";

		this->m_tsText += input.gettok(5, -1);
		SetWindowText(this->m_Hwnd, this->m_tsText.to_chr());

		// redraw if transparent
		if (this->isExStyle(WS_EX_TRANSPARENT))
			this->InvalidateParentRect(this->m_Hwnd);
		this->redrawWindow();
	}
	// This is to avoid invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags['r']) {
	}
	//xdid -t [NAME] [ID] [SWITCH] [TEXT]
	else if (flags['t']) {
		this->m_tsText = input.gettok(4, -1);
		SetWindowText(this->m_Hwnd, this->m_tsText.to_chr());

		// redraw if transparent
		if (this->isExStyle(WS_EX_TRANSPARENT))
			this->InvalidateParentRect(this->m_Hwnd);
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
 // switch( uMsg ) {
 //   case WM_COMMAND:
 //     {
 //       switch ( HIWORD( wParam ) ) {

 //         case STN_CLICKED:
 //           {
	//						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
	//			        this->execAliasEx("%s,%d", "sclick", this->getUserID( ) );
 //           }
 //           break;

 //         case STN_DBLCLK:
 //           {
	//						if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
	//			        this->execAliasEx("%s,%d", "dclick", this->getUserID( ) );
 //           }
 //           break;
 //       }
 //     }
 //     break;
	//}
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
				this->DrawClientArea((HDC)wParam);
				bParsed = TRUE;
			}
			break;
		case WM_PAINT:
			{
				bParsed = TRUE;
				PAINTSTRUCT ps;
				HDC hdc;

				hdc = BeginPaint( this->m_Hwnd, &ps );

				this->DrawClientArea(hdc);

				EndPaint( this->m_Hwnd, &ps );
			}
			break;

		case WM_ENABLE:
			{ // fixes bug with redraw when text control is enabled/disabled & formatted text is being used.
				bParsed = TRUE;
				InvalidateRect(this->m_Hwnd, NULL, FALSE);
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
	RECT r;
	// Setup alpha blend if any.
	LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

	TString wtext;
	TGetWindowText(this->m_Hwnd, wtext);

	GetClientRect(this->m_Hwnd, &r);

	DcxControl::DrawCtrlBackground(hdc,this,&r);

	HFONT oldFont = NULL;
	COLORREF oldClr = CLR_INVALID;
	COLORREF oldBkgClr = CLR_INVALID;

	// check if font is valid & set it.
	if (this->m_hFont != NULL)
		oldFont = SelectFont(hdc, this->m_hFont);
	// check if control is enabled.
	if (IsWindowEnabled(this->m_Hwnd)) {
		if (this->m_clrText != CLR_INVALID)
			oldClr = SetTextColor(hdc, this->m_clrText);
		if (this->m_clrBackText != CLR_INVALID)
			oldBkgClr = SetBkColor(hdc, this->m_clrBackText);
	}
	else { // disabled controls colouring
		oldClr = SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
		oldBkgClr = SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
	}

	UINT style = DT_LEFT;
	if (this->isStyle(SS_CENTER))
		style = DT_CENTER;
	if (this->isStyle(SS_RIGHT))
		style = DT_RIGHT;
	if (this->isStyle(SS_ENDELLIPSIS))
		style |= DT_END_ELLIPSIS;
	if (this->isStyle(SS_PATHELLIPSIS))
		style |= DT_PATH_ELLIPSIS;
	if (this->isStyle(SS_NOPREFIX))
		style |= DT_NOPREFIX;
	if (this->isStyle(SS_LEFTNOWORDWRAP))
		style |= DT_SINGLELINE;
	else
		style |= DT_WORDBREAK; // changed for autowrap between words

	this->ctrlDrawText(hdc, wtext, &r, style);

	if (oldBkgClr != CLR_INVALID)
		SetBkColor(hdc, oldBkgClr);
	if (oldClr != CLR_INVALID)
		SetTextColor(hdc, oldClr);
	if (oldFont != NULL)
		SelectFont(hdc, oldFont);

	this->FinishAlphaBlend(ai);
}
