/*!
 * \file dcxupdown.cpp
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
#include "Classes/dcxupdown.h"
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

DcxUpDown::DcxUpDown( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles, 
		DCX_UPDOWNCLASS, 
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

DcxUpDown::~DcxUpDown( ) {

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxUpDown::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

	*Styles |= UDS_ALIGNRIGHT;

	for (TString tsStyle(styles.getfirsttok(1)); !tsStyle.empty(); tsStyle = styles.getnexttok())
	{
		if ( tsStyle == TEXT("left") ) {
			*Styles &= ~UDS_ALIGNRIGHT;
			*Styles |= UDS_ALIGNLEFT;
		}
		else if ( tsStyle == TEXT("arrowkeys") )
			*Styles |= UDS_ARROWKEYS;
		else if ( tsStyle == TEXT("horz") )
			*Styles |= UDS_HORZ;
		else if ( tsStyle == TEXT("hottrack") )
			*Styles |= UDS_HOTTRACK;
		else if ( tsStyle == TEXT("nothousands") )
			*Styles |= UDS_NOTHOUSANDS;
		else if ( tsStyle == TEXT("buddyint") )
			*Styles |= UDS_SETBUDDYINT;
		else if ( tsStyle == TEXT("wrap") )
			*Styles |= UDS_WRAP;
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

void DcxUpDown::parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const
{
	const TString prop(input.getfirsttok( 3 ));

	// [NAME] [ID] [PROP]
	if ( prop == TEXT("value") ) {

		BOOL bError;
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), this->getPos32( &bError ) );
		return;
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("range") ) {

		int iMin, iMax;
		this->getRange32( &iMin, &iMax );
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), iMin, iMax );
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

void DcxUpDown::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const UINT numtok = input.numtok( );

	// xdid -c [NAME] [ID] [SWITCH] [BUDDYID]
	if ( flags[TEXT('c')] && numtok > 3 ) {

		DcxControl * p_Control = this->m_pParentDialog->getControlByID( (UINT) input.getnexttok( ).to_int( ) + mIRC_ID_OFFSET );	// tok 4

		if ( p_Control != NULL ) {

			TCHAR ClassName[256];
			GetClassName( p_Control->getHwnd( ), ClassName, 256 );

			// Text notifications
			if ( lstrcmpi( TEXT("STATIC"), ClassName ) == 0 || lstrcmpi( TEXT("EDIT"), ClassName ) == 0 ) {

				this->setBuddy( p_Control->getHwnd( ) );
			}
		}
	}
	// xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
	else if ( flags[TEXT('r')] && numtok > 4 ) {

		const int iMin = input.getnexttok( ).to_int( );	// tok 4
		const int iMax = input.getnexttok( ).to_int( );	// tok 5

		this->setRange32( iMin, iMax );
	}
	// xdid -t [NAME] [ID] [SWITCH] [BASE]
	else if ( flags[TEXT('t')] && numtok > 3 ) {

		const int nBase = input.getnexttok( ).to_int( );	// tok 4

		this->setBase( nBase );
	}
	// xdid -v [NAME] [ID] [SWITCH] [POS]
	else if ( flags[TEXT('v')] && numtok > 3 ) {

		const int nPos = input.getnexttok( ).to_int( );	// tok 4

		this->setPos32( nPos );
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::setBuddy( const HWND mHwnd ) {
  return SendMessage( this->m_Hwnd, UDM_SETBUDDY, (WPARAM) mHwnd , (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::setRange32( const int iLow, const int iHigh ) {
  return SendMessage( this->m_Hwnd, UDM_SETRANGE32, (WPARAM) iLow, (LPARAM) iHigh );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::getRange32( LPINT iLow, LPINT iHigh ) const {
  return SendMessage( this->m_Hwnd, UDM_GETRANGE32, (WPARAM) iLow, (LPARAM) iHigh );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::setBase( const int iBase ) {
  return SendMessage( this->m_Hwnd, UDM_SETBASE, (WPARAM) iBase, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::getBase( ) const {
  return SendMessage( this->m_Hwnd, UDM_GETBASE, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::setPos32( const INT nPos ) {
  return SendMessage( this->m_Hwnd, UDM_SETPOS32, (WPARAM) 0, (LPARAM) nPos );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxUpDown::getPos32( LPBOOL pfError ) const {
  return SendMessage( this->m_Hwnd, UDM_GETPOS32, (WPARAM) 0, (LPARAM) pfError );
}

TString DcxUpDown::getStyles(void) const
{
	TString styles(__super::getStyles());
	const DWORD Styles = GetWindowStyle(this->m_Hwnd);

	if (Styles & UDS_ALIGNLEFT)
		styles.addtok(TEXT("left"));
	if (Styles & UDS_HORZ)
		styles.addtok(TEXT("horz"));
	if (Styles & UDS_HOTTRACK)
		styles.addtok(TEXT("hottrack"));
	if (Styles & UDS_NOTHOUSANDS)
		styles.addtok(TEXT("nothousands"));
	if (Styles & UDS_SETBUDDYINT)
		styles.addtok(TEXT("buddyint"));
	if (Styles & UDS_WRAP)
		styles.addtok(TEXT("wrap"));
	return styles;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxUpDown::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg ) {
	case WM_NOTIFY : 
		{
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			switch( hdr->code ) {
			case UDN_DELTAPOS:
				{
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
						this->execAliasEx(TEXT("%s,%d"), TEXT("sclick"), this->getUserID( ) );
					bParsed = TRUE;
				}
				break;
			}
		}
		break;
	}
	return 0L;
}

LRESULT DcxUpDown::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

		case WM_LBUTTONUP:
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
