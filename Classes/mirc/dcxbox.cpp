/*!
 * \file dcxbox.cpp
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

#include "dcxbox.h"
#include "../../Dcx.h"
#include "../dcxdialog.h"

#include "../layout/layoutcellfixed.h"
#include "../layout/layoutcellfill.h"
#include "../layout/layoutcellpane.h"



/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxBox::DcxBox( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->m_TitleButton = NULL;
	this->_hTheme = NULL;

	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CONTROLPARENT,
		DCX_BOXCLASS,
		NULL,
		Styles | WS_CHILD,
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
		dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->m_pLayoutManager = new LayoutManager( this->m_Hwnd );

	this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );

	if (this->m_iBoxStyles & BOXS_CHECK) {
		this->m_TitleButton = CreateWindowEx(
			ExStyles,
			"BUTTON",
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX,
			CW_USEDEFAULT,CW_USEDEFAULT,11,10,
			this->m_Hwnd,
			(HMENU) ID,
			GetModuleHandle(NULL), 
			NULL);
	}
	else if (this->m_iBoxStyles & BOXS_RADIO) {
		this->m_TitleButton = CreateWindowEx(
			ExStyles,
			"BUTTON",
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON,
			CW_USEDEFAULT,CW_USEDEFAULT,11,10,
			this->m_Hwnd,
			(HMENU) ID,
			GetModuleHandle(NULL),
			NULL);
	}
	if (IsWindow(this->m_TitleButton)) {
		if ( bNoTheme )
			dcxSetWindowTheme( this->m_TitleButton , L" ", L" " );
		if (!(Styles & WS_DISABLED))
			SendMessage(this->m_TitleButton,BM_SETCHECK,BST_CHECKED,0L);
	}
	if (Dcx::XPPlusModule.isUseable())
		this->_hTheme = OpenThemeDataUx(this->m_Hwnd,L"BUTTON");
}

/*!
* \brief blah
*
* blah
*/

DcxBox::~DcxBox( ) {

	if ( this->m_pLayoutManager != NULL )
		delete this->m_pLayoutManager;
	if (this->_hTheme)
		CloseThemeDataUx(this->_hTheme);
	this->unregistreDefaultWindowProc( );
}

/*!
* \brief blah
*
* blah
*/

void DcxBox::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	unsigned int i = 1, numtok = styles.numtok( );
	this->m_iBoxStyles = 0;

	while ( i <= numtok ) {

		if (styles.gettok( i ) == "right")
			this->m_iBoxStyles |= BOXS_RIGHT;
		else if (styles.gettok( i ) == "center")
			this->m_iBoxStyles |= BOXS_CENTER;
		else if (styles.gettok( i ) == "bottom")
			this->m_iBoxStyles |= BOXS_BOTTOM;
		else if (styles.gettok( i ) == "none")
			this->m_iBoxStyles |= BOXS_NONE;
		else if (styles.gettok( i ) == "rounded")
			this->m_iBoxStyles |= BOXS_ROUNDED;
		else if (styles.gettok( i ) == "check") {
			this->m_iBoxStyles &= ~BOXS_RADIO;
			this->m_iBoxStyles |= BOXS_CHECK;
		}
		else if (styles.gettok( i ) == "radio") {
			this->m_iBoxStyles &= ~BOXS_CHECK;
			this->m_iBoxStyles |= BOXS_RADIO;
		}
		else if (styles.gettok( i ) == "transparent")
			*ExStyles |= WS_EX_TRANSPARENT;

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

void DcxBox::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( );

	TString prop(input.gettok( 3 ));

  // [NAME] [ID] [PROP]
  if ( prop == "text" ) {

    GetWindowText( this->m_Hwnd, szReturnValue, 900 );
    return;
  }
  else if ( prop == "inbox" ) {

	  RECT rc, rcText;
	  GetClientRect( this->m_Hwnd, &rc );

	  int n = GetWindowTextLength( this->m_Hwnd );

	  InflateRect( &rc, -2, -2 );
	  if ( n > 0 ) {

		  HDC hdc = GetDC( this->m_Hwnd );
		  HFONT oldFont = NULL;

		  if (this->m_hFont != NULL)
			  oldFont = SelectFont(hdc, this->m_hFont);

		  TString text((UINT)n+1);
		  GetWindowText( this->m_Hwnd, text.to_chr(), n+1 );
		  DrawText( hdc, text.to_chr(), n, &rcText, DT_CALCRECT );

		  if (this->m_hFont != NULL)
			  SelectFont(hdc, oldFont);

		  ReleaseDC( this->m_Hwnd, hdc );

		  //int w = rcText.right - rcText.left;
		  int h = rcText.bottom - rcText.top;

		  if ( this->m_iBoxStyles & BOXS_BOTTOM ) {
			  rc.bottom = rc.bottom - h + 2;
		  }
		  else {
			  rc.top = rc.top + h - 2;
		  }
	  }

	  wsprintf( szReturnValue, "%d %d %d %d", rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top );
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

void DcxBox::parseCommandRequest( TString & input ) {

	XSwitchFlags flags(input.gettok(3));
	int numtok = input.numtok( );

	// xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if ( flags['c'] && numtok > 8 ) {

		UINT ID = mIRC_ID_OFFSET + (UINT)input.gettok( 4 ).to_int( );

		if ( ID > mIRC_ID_OFFSET - 1 && 
			!IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
			this->m_pParentDialog->getControlByID( ID ) == NULL ) 
		{
			try {
				DcxControl * p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,input,5,-1,this->m_Hwnd);

				if ( p_Control != NULL ) {
					this->m_pParentDialog->addControl( p_Control );
					this->redrawWindow( );
				}
			}
			catch ( char *err ) {
				this->showErrorEx(NULL, "-c", "Unable To Create Control %d (%s)", ID - mIRC_ID_OFFSET, err);
			}
		}
		else
			this->showErrorEx(NULL,"-c", "Control with ID \"%d\" already exists", ID - mIRC_ID_OFFSET );
	}
	// xdid -d [NAME] [ID] [SWITCH] [ID]
	else if ( flags['d'] && numtok > 3 ) {

		UINT ID = mIRC_ID_OFFSET + input.gettok( 4 ).to_int( );
		DcxControl * p_Control;

		if ( IsWindow( GetDlgItem( this->m_Hwnd, ID ) ) && 
			ID > mIRC_ID_OFFSET - 1 && ( p_Control = this->m_pParentDialog->getControlByID( ID ) ) != NULL ) 
		{

			HWND cHwnd = p_Control->getHwnd( );
			if ( p_Control->getType( ) == "dialog" || p_Control->getType( ) == "window" )
				delete p_Control;
			else if ( p_Control->getRefCount( ) == 0 ) {
				this->m_pParentDialog->deleteControl( p_Control ); // remove from internal list!
				DestroyWindow( cHwnd );
			}
			else
				this->showErrorEx(NULL,"-d", "Can't delete control with ID \"%d\" when it is inside it's own event (dialog %s)", p_Control->getUserID( ), this->m_pParentDialog->getName( ).to_chr( ) );
		}
		else
			this->showErrorEx(NULL,"-d", "Unknown control with ID \"%d\" (dialog %s)", ID - mIRC_ID_OFFSET, this->m_pParentDialog->getName( ).to_chr( ) );
	}
	/*
	//xdid -l [NAME] [ID] [SWITCH] [OPTIONS]

	[OPTIONS] :

	root [TAB]+flpiw [ID] [WEIGHT] [W] [H]
	add PATH[TAB]+flpiw [ID] [WEIGHT] [W] [H]
	space PATH[TAB]+ [L] [T] [R] [B]
	*/
	else if ( flags['l'] && numtok > 3 ) {

		if ( input.gettok( 4 ) == "update" ) {
			if ( this->m_pLayoutManager != NULL ) {
				RECT rc;
				GetClientRect( this->m_Hwnd, &rc );
				this->m_pLayoutManager->updateLayout( rc );
				this->redrawWindow();
			}
		}
		else if (input.gettok( 4 ) == "clear") {
			if (this->m_pLayoutManager != NULL)
				delete this->m_pLayoutManager;
			this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);
			//this->redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
		else if ( numtok > 8 ) {

			TString com(input.gettok(1, TSTAB).gettok(4).trim());
			TString path(input.gettok(1, TSTAB).gettok(5, -1).trim());
			TString p2(input.gettok(2, TSTAB).trim());

			UINT lflags = this->parseLayoutFlags( p2.gettok( 1 ) );
			UINT ID = p2.gettok( 2 ).to_int( );
			UINT WGT = p2.gettok( 3 ).to_int( );
			UINT W = p2.gettok( 4 ).to_int( );
			UINT H = p2.gettok( 5 ).to_int( );

			if ( com ==  "root" || com == "cell" ) {

				HWND cHwnd = GetDlgItem( this->m_Hwnd, mIRC_ID_OFFSET + ID );

				LayoutCell * p_Cell = NULL;

				// LayoutCellPane
				if ( lflags & LAYOUTPANE ) {

					if ( lflags & LAYOUTHORZ )
						p_Cell = new LayoutCellPane( LayoutCellPane::HORZ );
					else
						p_Cell = new LayoutCellPane( LayoutCellPane::VERT );
				} // if ( lflags & LAYOUTPANE )
				// LayoutFill Cell
				else if ( lflags & LAYOUTFILL ) {
					if ( lflags & LAYOUTID ) {
						if ( cHwnd != NULL && IsWindow( cHwnd ) )
							p_Cell = new LayoutCellFill( cHwnd );
						else {
							this->showErrorEx(NULL,"-l", "Cell Fill -> Invalid ID : %d", ID );
							return;
						}
					}
					else {
						p_Cell = new LayoutCellFill( );
					}
				} // else if ( lflags & LAYOUTFILL )
				// LayoutCellFixed
				else if ( lflags & LAYOUTFIXED ) {

					LayoutCellFixed::FixedType type;

					if ( lflags & LAYOUTVERT && lflags & LAYOUTHORZ )
						type = LayoutCellFixed::BOTH;
					else if ( lflags & LAYOUTVERT )
						type = LayoutCellFixed::HEIGHT;
					else
						type = LayoutCellFixed::WIDTH;

					// Defined Rectangle
					if ( lflags & LAYOUTDIM ) {

						RECT rc;
						SetRect( &rc, 0, 0, W, H );

						if ( lflags & LAYOUTID ) {

							if ( cHwnd != NULL && IsWindow( cHwnd ) )
								p_Cell = new LayoutCellFixed( cHwnd, rc, type );
							else {
								this->showErrorEx(NULL,"-l", "Cell Fixed -> Invalid ID : %d", ID );
								return;
							}
						}
						else
							p_Cell = new LayoutCellFixed( rc, type );

					}
					// No defined Rectangle
					else {

						if ( lflags & LAYOUTID ) {

							if ( cHwnd != NULL && IsWindow( cHwnd ) )
								p_Cell = new LayoutCellFixed( cHwnd, type );
							else {
								this->showErrorEx(NULL,"-l", "Cell Fixed -> Invalid ID : %d", ID );
								return;
							}
						}
					} //else
				} // else if ( lflags & LAYOUTFIXED )
				else {
					this->showError(NULL,"-l", "Unknown Cell Type");
					return;
				}

				if ( com == "root" ) {

					if ( p_Cell != NULL )
						this->m_pLayoutManager->setRoot( p_Cell );

				} // if ( com == "root" )
				else if ( com == "cell" ) {

					if ( p_Cell != NULL ) {

						LayoutCell * p_GetCell;

						if ( path == "root" )
							p_GetCell = this->m_pLayoutManager->getRoot( );
						else
							p_GetCell = this->m_pLayoutManager->getCell( path );

						if ( p_GetCell == NULL ) {
							this->showErrorEx(NULL,"-l", "Invalid item path: %s", path.to_chr( ) );
							return;
						}

						if ( p_GetCell->getType( ) == LayoutCell::PANE ) {

							LayoutCellPane * p_PaneCell = (LayoutCellPane *) p_GetCell;
							p_PaneCell->addChild( p_Cell, WGT );
						}
					}
				} // else if ( com == "cell" )
			} // if ( com ==  "root" || com == "cell" )
			else if ( com ==  "space" ) {

				LayoutCell * p_GetCell;

				if ( path == "root" )
					p_GetCell = this->m_pLayoutManager->getRoot( );
				else
					p_GetCell = this->m_pLayoutManager->getCell( path );

				if ( p_GetCell != NULL ) {

					RECT rc;
					SetRect( &rc, ID, WGT, W, H );
					p_GetCell->setBorder( rc );
				}
			} // else if ( com == "space" )
		} // if ( numtok > 7 )
	}
	//xdid -t [NAME] [ID] [SWITCH]
	else if ( flags['t'] ) {
		SetWindowText(this->m_Hwnd, input.gettok(4, -1).trim().to_chr());
		this->redrawWindow( );
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxBox::parseLayoutFlags( const TString & flags ) {

  INT i = 1, len = (INT)flags.len( );
  UINT iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if ( flags[i] == 'f' )
      iFlags |= LAYOUTFIXED;
    else if ( flags[i] == 'h' )
      iFlags |= LAYOUTHORZ;
    else if ( flags[i] == 'i' )
      iFlags |= LAYOUTID;
    else if ( flags[i] == 'l' )
      iFlags |= LAYOUTFILL ;
    else if ( flags[i] == 'p' )
      iFlags |= LAYOUTPANE;
    else if ( flags[i] == 'v' )
      iFlags |= LAYOUTVERT;
    else if ( flags[i] == 'w' )
      iFlags |= LAYOUTDIM;

    ++i;
  }

  return iFlags;
}
BOOL CALLBACK EnumBoxChildren(HWND hwnd,LPDCXENUM de)
{
	//LPDCXENUM de = (LPDCXENUM)lParam;
	if ((de->mChildHwnd != hwnd) && (GetParent(hwnd) == de->mBox))
		EnableWindow(hwnd,de->mState);

	return TRUE;
}

void DcxBox::toXml(TiXmlElement * xml) {
	TString wtext;
	int n = TGetWindowText(this->m_Hwnd, wtext);
	__super::toXml(xml);
	xml->SetAttribute("caption", wtext.to_chr());
	this->m_pLayoutManager->getRoot()->toXml(xml);
}

TString DcxBox::getStyles(void) {
	TString result;
	result = __super::getStyles();
	if (this->m_iBoxStyles & BOXS_RIGHT)
		result.addtok("right", " ");
	if (this->m_iBoxStyles & BOXS_CENTER)
		result.addtok("center", " ");
	if (this->m_iBoxStyles & BOXS_BOTTOM)
		result.addtok("bottom", " ");
	if (this->m_iBoxStyles & BOXS_NONE)
		result.addtok("none", " ");
	if (this->m_iBoxStyles & BOXS_ROUNDED)
		result.addtok("rounded", " ");
	if (this->m_iBoxStyles & BOXS_CHECK)
		result.addtok("check", " ");
	else if (this->m_iBoxStyles & BOXS_RADIO)
		result.addtok("radio", " ");

	return result;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxBox::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxBox::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
	switch( uMsg ) {

	case WM_NOTIFY:
		{
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			if (IsWindow(hdr->hwndFrom)) {
				DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,"dcx_cthis");
				if (c_this != NULL)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

    case WM_COMMAND:
			{
				if (((HWND)lParam != NULL) && ((HWND)lParam == this->m_TitleButton) ) {
					switch ( HIWORD( wParam ) )
					{
						case BN_CLICKED:
						{
							BOOL state = (SendMessage(this->m_TitleButton,BM_GETCHECK,0,0) == BST_CHECKED);
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								char ret[10];

								this->evalAliasEx(ret, 9, "%s,%d,%d", "checkchange", this->getUserID(), state);

								if (lstrcmp("nochange", ret) == 0)
									return 0L;
							}

							DCXENUM de;
							de.mChildHwnd = this->m_TitleButton;
							de.mBox = this->m_Hwnd;
							de.mState = state;

							EnumChildWindows(this->m_Hwnd,(WNDENUMPROC)EnumBoxChildren,(LPARAM)&de);
							break;
						}
					} // end switch
					break;
				}
			}
		case WM_HSCROLL:
		case WM_VSCROLL:
			{
				if (IsWindow((HWND) lParam)) {
					DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,"dcx_cthis");
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_COMPAREITEM:
			{
				LPCOMPAREITEMSTRUCT idata = (LPCOMPAREITEMSTRUCT)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DELETEITEM:
			{
				DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_MEASUREITEM:
			{
				HWND cHwnd = GetDlgItem(this->m_Hwnd, wParam);
				if (IsWindow(cHwnd)) {
					DcxControl *c_this = (DcxControl *) GetProp(cHwnd,"dcx_cthis");
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DRAWITEM:
			{
				DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,"dcx_cthis");
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_SIZE:
			{

				HWND bars = NULL;

				while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_REBARCTRLCLASS, NULL ) ) != NULL ) {

					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_STATUSBARCLASS, NULL ) ) != NULL ) {

					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_TOOLBARCLASS, NULL ) ) != NULL ) {

					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_SIZE)
					this->execAliasEx("%s,%d", "sizing", this->getUserID( ) );

				if (this->m_pLayoutManager != NULL) {
					RECT rc;
					SetRect( &rc, 0, 0, LOWORD( lParam ), HIWORD( lParam ) );
					if (this->m_pLayoutManager->updateLayout( rc ))
						this->redrawWindow( );
				}
			}
			break;

		case WM_ENABLE: {
			this->redrawWindow();
			break;
		}


		case WM_ERASEBKGND:
		{
			this->EraseBackground((HDC)wParam);
			bParsed = TRUE;
			return TRUE;
		}
		break;

		case WM_PRINTCLIENT:
			this->DrawClientArea((HDC)wParam);
			bParsed = TRUE;
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(this->m_Hwnd, &ps);

				this->DrawClientArea(hdc);

				EndPaint(this->m_Hwnd, &ps);

				bParsed = TRUE;
				return 0L;
			}
			break;

		case WM_THEMECHANGED:
			{
				if (this->_hTheme != NULL) {
					CloseThemeDataUx(this->_hTheme);
					this->_hTheme = OpenThemeDataUx(this->m_Hwnd,L"BUTTON");
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
			lRes = this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return lRes;
}
void DcxBox::EraseBackground(HDC hdc)
{
	RECT rc;
	GetClientRect( this->m_Hwnd, &rc );
	// fill background.
	if (this->isExStyle(WS_EX_TRANSPARENT) || this->m_pParentDialog->isExStyle(WS_EX_COMPOSITED))
		this->DrawParentsBackground(hdc, &rc);
	else // normal bkg
		DcxControl::DrawCtrlBackground(hdc, this, &rc);
	// Update CLA if any.
	if (this->m_pLayoutManager != NULL)
		this->m_pLayoutManager->updateLayout( rc );
}

void DcxBox::DrawClientArea(HDC hdc)
{
	RECT rc, rc2, rcText, rcText2;
	TString wtext;
	int n = TGetWindowText(this->m_Hwnd, wtext);

	GetClientRect(this->m_Hwnd, &rc);
	CopyRect(&rc2, &rc);

	// Setup alpha blend if any.
	LPALPHAINFO ai = this->SetupAlphaBlend(&hdc,true);

	//DcxControl::DrawCtrlBackground(hdc, this, &rc2); //Moved out from the if, becase of painting-bug (Alpha)
	// having this here messes up all boxes whis a border.
	// exp boxes that have a border & text.

	// if no border, dont bother
	if (this->m_iBoxStyles & BOXS_NONE) {
		DcxControl::DrawCtrlBackground(hdc, this, &rc2);
		return;
	}

	SetBkMode(hdc, TRANSPARENT);

	// no text, no box!
	if (!n) {
		if (this->m_iBoxStyles & BOXS_ROUNDED) {
			HRGN m_Region = CreateRoundRectRgn(rc2.left, rc2.top, rc2.right, rc2.bottom, 10, 10);
			if (m_Region) {
				SelectClipRgn(hdc,m_Region);
				DcxControl::DrawCtrlBackground(hdc, this, &rc2);
				SelectClipRgn(hdc,NULL);

				HBRUSH hBorderBrush = this->m_hBorderBrush;
				if (hBorderBrush == NULL)
					hBorderBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

				FrameRgn(hdc,m_Region,hBorderBrush,1,1);
				DeleteRgn(m_Region);
			}
		}
		else {
			DcxControl::DrawCtrlBackground(hdc, this, &rc2);
			DrawEdge(hdc, &rc2, EDGE_ETCHED, BF_RECT);
		}
		if (IsWindow(this->m_TitleButton))
			SetWindowPos(this->m_TitleButton,NULL,rc2.left,rc2.top,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	}
	// draw text
	else {
		// prepare for appearance
		if (this->m_hFont != NULL)
			SelectFont(hdc, this->m_hFont);

		if (this->m_clrText != -1)
			SetTextColor(hdc, this->m_clrText);
		else
			SetTextColor(hdc, GetSysColor(
				IsWindowEnabled(this->m_Hwnd) ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT)
			);

		if (this->m_bCtrlCodeText)
			calcStrippedRect(hdc, wtext, 0, &rcText, false, this->m_bUseUTF8);
		else
			DrawTextW(hdc, wtext.to_wchr(this->m_bUseUTF8), n, &rcText, DT_CALCRECT);

		int w = rcText.right - rcText.left;
		int h = rcText.bottom - rcText.top;

		// shift border and text locations
		// text at bottom?
		if (this->m_iBoxStyles & BOXS_BOTTOM) {
			rcText.top = rc2.bottom - h;
			rc2.bottom -= h/2;
		}
		// text at top
		else {
			rcText.top = rc2.top;
			rc2.top += h/2;
		}

		// text location
		rcText.bottom = rcText.top + h;

		// align text horizontally
		int bw = rc.right - rc.left - (2 * DCX_BOXTEXTSPACING);

		if (w > bw) {
			rcText.left = rc.left + DCX_BOXTEXTSPACING;
			rcText.right = rc.right - DCX_BOXTEXTSPACING;
		}
		else {
			if (this->m_iBoxStyles & BOXS_RIGHT)
				rcText.left = rc.right - DCX_BOXTEXTSPACING - w;
			else if (this->m_iBoxStyles & BOXS_CENTER)
				rcText.left = (rc.left + rc.right - w) /2;
			else
				rcText.left = rc.left + DCX_BOXTEXTSPACING;

			rcText.right = rcText.left + w;
		}

		// clear some space for the text
		CopyRect(&rcText2, &rcText);
		InflateRect(&rcText2, 3, 0);

		if (IsWindow(this->m_TitleButton))
		{
			RECT bSZ;
			GetWindowRect(this->m_TitleButton,&bSZ);
			bSZ.bottom = (bSZ.right - bSZ.left);
			SetWindowPos(this->m_TitleButton,NULL,rcText2.left,rcText2.top,bSZ.bottom,(rcText2.bottom - rcText2.top),SWP_NOZORDER|SWP_NOACTIVATE);
			rcText.left += bSZ.bottom;
			rcText.right += bSZ.bottom;
			rcText2.left += bSZ.bottom;
			rcText2.right += bSZ.bottom;
		}

		// draw the border
		if (this->m_iBoxStyles & BOXS_ROUNDED) {
			HRGN m_Region = CreateRoundRectRgn(rc2.left, rc2.top, rc2.right, rc2.bottom, 10, 10);
			if (m_Region) {
				SelectClipRgn(hdc,m_Region);
				DcxControl::DrawCtrlBackground(hdc, this, &rc2);
				SelectClipRgn(hdc,NULL);
				ExcludeClipRect(hdc, rcText2.left, rcText2.top, rcText2.right, rcText2.bottom);

				HBRUSH hBorderBrush = this->m_hBorderBrush;
				if (hBorderBrush == NULL)
					hBorderBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

				FrameRgn(hdc,m_Region,hBorderBrush,1,1);
				DeleteRgn(m_Region);
			}
		}
		else {
			DcxControl::DrawCtrlBackground(hdc, this, &rc2);
			ExcludeClipRect(hdc, rcText2.left, rcText2.top, rcText2.right, rcText2.bottom);
			DrawEdge(hdc, &rc2, EDGE_ETCHED, BF_RECT);
		}
		SelectClipRgn(hdc,NULL);

		// draw the text
		//if (!this->m_bCtrlCodeText) {
		//	if (this->m_bShadowText)
		//		dcxDrawShadowText(hdc,wtext.to_wchr(this->m_bUseUTF8), n,&rcText, DT_END_ELLIPSIS | DT_LEFT, this->m_clrText, 0, 5, 5);
		//	else
		//		DrawTextW(hdc, wtext.to_wchr(this->m_bUseUTF8), n, &rcText, DT_LEFT | DT_END_ELLIPSIS);
		//}
		//else
		//	mIRC_DrawText(hdc, wtext, &rcText, DT_LEFT | DT_END_ELLIPSIS, this->m_bShadowText, this->m_bUseUTF8);
		this->ctrlDrawText(hdc, wtext, &rcText, DT_LEFT | DT_END_ELLIPSIS);
	}

	this->FinishAlphaBlend(ai);
}
