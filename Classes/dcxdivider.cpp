/*!
 * \file dcxdivider.cpp
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
#include "Classes/dcxdivider.h"
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

DcxDivider::DcxDivider( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles )
: DcxControl( ID, p_Dialog )
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles | WS_EX_CONTROLPARENT, 
		DCX_DIVIDERCLASS, 
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

	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxDivider::~DcxDivider( ) {

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

TString DcxDivider::getStyles(void) {
	TString styles(__super::getStyles());
	DWORD Styles;
	Styles = GetWindowStyle(this->m_Hwnd);
	if (Styles & DVS_VERT)
		styles.addtok(TEXT("vertical"));
	return styles;
}


void DcxDivider::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	unsigned int i = 1, numtok = styles.numtok( );
	*Styles |= DVS_HORZ;

	while ( i <= numtok ) {

		if ( styles.gettok( i ) == TEXT("vertical") )
			*Styles |= DVS_VERT;

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

void DcxDivider::parseInfoRequest( TString & input, PTCHAR szReturnValue ) {

	//int numtok = input.numtok( );
	TString prop(input.gettok(3));

	// [NAME] [ID] [PROP]
	if (prop == TEXT("position")) {
		int iDivPos = 0;

		SendMessage(this->m_Hwnd, DV_GETDIVPOS, (WPARAM) NULL, (LPARAM) &iDivPos);
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), iDivPos);
		return;
	}
	else if (prop == TEXT("isvertical")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), (GetWindowStyle(this->m_Hwnd) & DVS_VERT));
		return;
	}

	if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
		return;

	szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxDivider::parseCommandRequest( TString & input ) {
	XSwitchFlags flags(input.gettok(3));

	int numtok = input.numtok( );

	// xdid -l|r [NAME] [ID] [SWITCH] [MIN] [IDEAL][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if ( ( flags[TEXT('l')] || flags[TEXT('r')] )&& numtok > 9 ) {

		DVPANEINFO dvpi;
		ZeroMemory( &dvpi, sizeof( DVPANEINFO ) );
		dvpi.cbSize = sizeof( DVPANEINFO );

		TString data(input.gettok(1, TSTAB).trim());
		TString control_data;

		if ( input.numtok( TSTAB ) > 1 )
			control_data = input.gettok(2, TSTAB).trim();

		dvpi.fMask = DVPIM_CHILD | DVPIM_MIN | DVPIM_IDEAL;
		dvpi.cxMin = data.gettok( 4 ).to_int( );
		dvpi.cxIdeal = data.gettok( 5 ).to_int( );

		if ( control_data.numtok( ) > 5 ) {

			UINT ID = mIRC_ID_OFFSET + control_data.gettok( 1 ).to_int( );

			if ( ID > mIRC_ID_OFFSET - 1 && 
				!IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
				this->m_pParentDialog->getControlByID( ID ) == NULL ) 
			{
				try {
					DcxControl * p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,control_data,2,CTLF_ALLOW_ALLBUTDOCK,this->m_Hwnd);

					if ( p_Control != NULL ) {

						this->m_pParentDialog->addControl( p_Control );

						dvpi.hChild = p_Control->getHwnd( );

						if ( flags[TEXT('l')] )
							this->setPane( DVF_PANELEFT, &dvpi );
						else if ( flags[TEXT('r')] )
							this->setPane( DVF_PANERIGHT, &dvpi );

						this->redrawWindow( );
					}
				}
				catch ( TCHAR *err ) {
					this->showErrorEx(NULL, TEXT("-l|r"), TEXT("Unable To Create Control %d (%s)"), this->getUserID(), err);
				}
			}
			else
				this->showErrorEx(NULL, TEXT("-l|r"), TEXT("Control with ID \"%d\" already exists"), this->getUserID());
		}
	}
	// xdid -v [NAME] [ID] [SWITCH] [POS]
	else if (flags[TEXT('v')] && numtok > 3) {
		if (!this->setDivPos(input.gettok(4).to_int()))
			this->showError(NULL, TEXT("-v"), TEXT("Divider position must be between bounds."));
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxDivider::setPane( const UINT iPaneId, LPDVPANEINFO lpdvpi ) {
  return SendMessage( this->m_Hwnd, DV_SETPANE, (WPARAM) iPaneId, (LPARAM) lpdvpi );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxDivider::setDivPos( const UINT iDivPos ) {
  return SendMessage( this->m_Hwnd, DV_SETDIVPOS, (WPARAM) 0, (LPARAM) iDivPos );
}

void DcxDivider::toXml(TiXmlElement * xml) {
	__super::toXml(xml);
	DVPANEINFO left;
	DVPANEINFO right;
	DcxControl * dcxcleft = NULL;
	DcxControl * dcxcright = NULL;
	Divider_GetChildControl(this->m_Hwnd, DVF_PANELEFT, &left);
	Divider_GetChildControl(this->m_Hwnd, DVF_PANELEFT, &right);
	if (left.hChild != NULL) {
		dcxcleft = this->m_pParentDialog->getControlByHWND(left.hChild);
		if (dcxcleft != NULL)
			xml->LinkEndChild(dcxcleft->toXml());
		else
			xml->LinkEndChild(new TiXmlElement("control"));
	}
	else
		xml->LinkEndChild(new TiXmlElement("control"));
	if (right.hChild != NULL) {
		dcxcright = this->m_pParentDialog->getControlByHWND(right.hChild);
		if (dcxcright != NULL)
			xml->LinkEndChild(dcxcright->toXml());
		else
			xml->LinkEndChild(new TiXmlElement("control"));
	}
	else
		xml->LinkEndChild(new TiXmlElement("control"));
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxDivider::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxDivider::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
	switch( uMsg ) {

	case WM_NOTIFY : 
		{
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			if (IsWindow(hdr->hwndFrom)) {
				DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,TEXT("dcx_cthis"));
				if (c_this != NULL)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_HSCROLL: 
	case WM_VSCROLL: 
	case WM_COMMAND:
		{
			if (IsWindow((HWND) lParam)) {
				DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,TEXT("dcx_cthis"));
				if (c_this != NULL)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;
	case WM_COMPAREITEM:
		{
			LPCOMPAREITEMSTRUCT idata = (LPCOMPAREITEMSTRUCT)lParam;
			if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
				DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
				if (c_this != NULL)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_DELETEITEM:
		{
			DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
			if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
				DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
				if (c_this != NULL)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_MEASUREITEM:
		{
			HWND cHwnd = GetDlgItem(this->m_Hwnd, wParam);
			if (IsWindow(cHwnd)) {
				DcxControl *c_this = (DcxControl *) GetProp(cHwnd,TEXT("dcx_cthis"));
				if (c_this != NULL)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
			if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
				DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
				if (c_this != NULL)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

	case WM_DESTROY:
		{
			delete this;
			bParsed = TRUE;
		}
		break;

	case DV_CHANGEPOS:
		{
			int phase = (int) wParam;
			LPPOINT pt = (LPPOINT) lParam;

			this->execAliasEx(TEXT("%s,%d,%d,%d"), (phase == DVNM_DRAG_START ? TEXT("dragbegin") : (phase == DVNM_DRAG_END ? TEXT("dragfinish") : TEXT("drag"))), this->getUserID(), pt->x, pt->y);
		}
		break;

	default:
		lRes = this->CommonMessage( uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}
