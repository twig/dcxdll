/*
	* Pager Control
*/
#include "defines.h"
#include "Classes/dcxpager.h"
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

DcxPager::DcxPager( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, const TString & styles ) 
: DcxControl( ID, p_Dialog )
, m_ChildHWND(NULL)
{
  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles | WS_EX_CONTROLPARENT,
    DCX_PAGERCLASS,
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

	//Pager_SetButtonSize(this->m_Hwnd,15);
	//Pager_SetBkColor(this->m_Hwnd,0);

  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxPager::~DcxPager( ) {

  this->unregistreDefaultWindowProc( );
}

TString DcxPager::getStyles(void) const
{
	TString styles(__super::getStyles());
	DWORD Styles;
	Styles = GetWindowStyle(this->m_Hwnd);
	if (Styles & PGS_HORZ)
		styles.addtok(TEXT("horizontal"));
	if (Styles & PGS_AUTOSCROLL)
		styles.addtok(TEXT("autoscroll"));
	return styles;

}

void DcxPager::toXml(TiXmlElement * xml) const
{
	__super::toXml(xml);
	DcxControl * child;
	child = this->m_pParentDialog->getControlByHWND(this->m_ChildHWND);
	if (child != NULL)
		xml->LinkEndChild(child->toXml());
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxPager::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
{
	const UINT numtok = styles.numtok( );

	for (UINT i = 1;  i <= numtok; i++ ) {

		if (styles.gettok( i ) == TEXT("horizontal"))
			*Styles |= PGS_HORZ;
		else if (styles.gettok( i ) == TEXT("autoscroll"))
			*Styles |= PGS_AUTOSCROLL;
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

void DcxPager::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	//  int numtok = input.numtok( );
	const TString prop(input.gettok( 3 ));

	if ( prop == TEXT("color")) {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), Pager_GetBkColor(this->m_Hwnd) );
		return;
	}
	else if ( prop == TEXT("bsize")) {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), Pager_GetButtonSize(this->m_Hwnd) );
		return;
	}
	else if ( prop == TEXT("border")) {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), Pager_GetBorder(this->m_Hwnd) );
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

void DcxPager::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.gettok(3));
	const int numtok = input.numtok();

	// xdid -b [NAME] [ID] [SWITCH] [W]
	if ( flags[TEXT('b')] && numtok > 3 ) {
		this->setBorderSize(input.gettok( 4 ).to_int());
	}
	// xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	else if ( flags[TEXT('c')] && numtok > 8 ) {

		if (IsWindow(this->m_ChildHWND)) {
			this->showError(NULL, TEXT("-c"), TEXT("Child Control already exists"));
			//DCXError( TEXT("/xdid -c"),TEXT("Child Control already exists") );
			return;
		}
		UINT ID = mIRC_ID_OFFSET + (UINT)input.gettok( 4 ).to_int( );

		if ( ID > mIRC_ID_OFFSET - 1 && 
			!IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
			this->m_pParentDialog->getControlByID( ID ) == NULL ) 
		{
			try {
				DcxControl * p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,input,5,
					CTLF_ALLOW_TOOLBAR |
					CTLF_ALLOW_REBAR |
					CTLF_ALLOW_PANEL |
					CTLF_ALLOW_TAB |
					CTLF_ALLOW_BOX |
					CTLF_ALLOW_IMAGE |
					CTLF_ALLOW_PAGER |
					CTLF_ALLOW_DOCK
					,this->m_Hwnd);

				if ( p_Control != NULL ) {
					this->m_pParentDialog->addControl( p_Control );
					p_Control->addStyle(CCS_NORESIZE);
					this->setChild(p_Control->getHwnd());

					this->redrawWindow( );
				}
			}
			catch ( TCHAR *err ) {
				this->showErrorEx(NULL, TEXT("-c"), TEXT("Unable To Create Control %d (%s)"), ID - mIRC_ID_OFFSET, err);
			}
		}
		else
			this->showErrorEx(NULL, TEXT("-c"), TEXT("Control with ID \"%d\" already exists"), ID - mIRC_ID_OFFSET );
	}
	// xdid -d [NAME] [ID] [SWITCH] [ID]
	else if ( flags[TEXT('d')] && numtok > 3 ) {

		UINT ID = mIRC_ID_OFFSET + input.gettok( 4 ).to_int( );
		DcxControl * p_Control;

		if ( IsWindow( GetDlgItem( this->m_Hwnd, ID ) ) && (ID > mIRC_ID_OFFSET - 1) && ( p_Control = this->m_pParentDialog->getControlByID( ID ) ) != NULL ) 
		{
			HWND cHwnd = p_Control->getHwnd( );
			if ( p_Control->getType( ) == TEXT("dialog") || p_Control->getType( ) == TEXT("window") ) {
				delete p_Control;
				this->m_ChildHWND = NULL;
			}
			else if ( p_Control->getRefCount( ) == 0 ) {
				this->m_pParentDialog->deleteControl( p_Control ); // remove from internal list!
				DestroyWindow( cHwnd );
				this->m_ChildHWND = NULL;
			}
			else
				this->showErrorEx(NULL, TEXT("-d"), TEXT("Can't delete control with ID \"%d\" when it is inside it's own event (dialog %s)"), p_Control->getUserID( ), this->m_pParentDialog->getName( ).to_chr( ) );
		}
		else
			this->showErrorEx(NULL, TEXT("-d"), TEXT("Unknown control with ID \"%d\" (dialog %s)"), ID - mIRC_ID_OFFSET, this->m_pParentDialog->getName( ).to_chr( ) );
	}
	// xdid -s [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('s')] && numtok > 3) {
		this->setButtonSize((LONG)input.gettok(4, -1).to_num());
	}
	// xdid -t [NAME] [ID] [SWITCH] [COLOR]
	else if (flags[TEXT('t')] && numtok > 3) {
		this->setBkColor((COLORREF)input.gettok(4, -1).to_num());
	}
	// xdid -z [NAME] [ID] [SWITCH]
	else if (flags[TEXT('z')] && numtok > 2) {
		this->reCalcSize();
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}
void DcxPager::setChild(const HWND child)
{
	this->m_ChildHWND = child;
	Pager_SetChild(this->m_Hwnd,child);
}
void DcxPager::setBkColor(const COLORREF c)
{
	Pager_SetBkColor(this->m_Hwnd,c);
}
void DcxPager::setBorderSize(const int bSize)
{
	Pager_SetBorder(this->m_Hwnd,bSize);
}
void DcxPager::setButtonSize(const int bSize)
{
	Pager_SetButtonSize(this->m_Hwnd,bSize);
}
void DcxPager::reCalcSize(void) const
{
	Pager_RecalcSize(this->m_Hwnd);
}
/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxPager::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {
    case WM_NOTIFY : 
      {

        LPNMHDR hdr = (LPNMHDR) lParam;

        if (!hdr)
          break;

				switch( hdr->code ) {
				case PGN_CALCSIZE:
					{
						bParsed = TRUE;
						LPNMPGCALCSIZE lpnmcs = (LPNMPGCALCSIZE) lParam;
						RECT rc;
						ZeroMemory(&rc,sizeof(RECT));
						//GetWindowRect(this->m_ChildHWND,&rc);
						//if (lpnmcs->dwFlag == PGF_CALCHEIGHT)
						//	lpnmcs->iHeight = (rc.bottom - rc.top);
						//else
						//	lpnmcs->iWidth = (rc.right - rc.left);
						DcxControl *cthis = (DcxControl *)GetProp(this->m_ChildHWND,TEXT("dcx_cthis"));
						if (cthis != NULL) {
							int bSize = Pager_GetButtonSize(this->m_Hwnd);
							if (cthis->getType() == TEXT("toolbar")) {
								SIZE sz;
								SendMessage(this->m_ChildHWND, TB_GETMAXSIZE, 0, (LPARAM)&sz);
								if (lpnmcs->dwFlag == PGF_CALCHEIGHT)
									lpnmcs->iHeight = sz.cy+bSize;
								else
									lpnmcs->iWidth = sz.cx+bSize;
							}
							else {
								GetWindowRect(this->m_ChildHWND,&rc);
								if (lpnmcs->dwFlag == PGF_CALCHEIGHT)
									lpnmcs->iHeight = (rc.bottom - rc.top)+bSize;
								else
									lpnmcs->iWidth = (rc.right - rc.left)+bSize;
							}
						}
					}
					break;
				}
      }
      break;
	}
	return 0L;
}

LRESULT DcxPager::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
	switch( uMsg ) {

	case WM_NOTIFY : 
		{

			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;
			if (IsWindow(hdr->hwndFrom)) {
				DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,TEXT("dcx_cthis"));
				if (c_this != NULL) {
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			switch( hdr->code ) {
			case TBN_DELETINGBUTTON:
				{ // handle toolbar button delete.
					this->reCalcSize();
				}
				break;
			}
		}
		break;

	case WM_HSCROLL:
	case WM_VSCROLL:
	case WM_COMMAND:
		{
			if (IsWindow((HWND) lParam)) {
				DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,TEXT("dcx_cthis"));
				if (c_this != NULL) {
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
		}
		break;

	case WM_DELETEITEM:
		{
			DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
			if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
				DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
				if (c_this != NULL) {
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
		}
		break;

	case WM_MEASUREITEM:
		{
			HWND cHwnd = GetDlgItem(this->m_Hwnd, wParam);
			if (IsWindow(cHwnd)) {
				DcxControl *c_this = (DcxControl *) GetProp(cHwnd,TEXT("dcx_cthis"));
				if (c_this != NULL) {
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
		}
		break;

	case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
			if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
				DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
				if (c_this != NULL) {
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
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
				this->execAliasEx(TEXT("%s,%d"), TEXT("sizing"), this->getUserID( ) );
			this->reCalcSize();
			this->redrawWindow( );
		}
		break;

	case WM_SETFOCUS:
		{
			this->m_pParentDialog->setFocusControl( this->getUserID( ) );
			this->reCalcSize();
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
