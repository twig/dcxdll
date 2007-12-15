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

#include "dcxupdown.h"
#include "dcxdialog.h"

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
		throw "Unable To Create Window";

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

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

DcxUpDown::~DcxUpDown( ) {

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxUpDown::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  unsigned int i = 1, numtok = styles.numtok( );
  *Styles |= UDS_ALIGNRIGHT;

  while ( i <= numtok ) {

    if ( styles.gettok( i ) == "left" ) {
      *Styles &= ~UDS_ALIGNRIGHT;
      *Styles |= UDS_ALIGNLEFT;
    }
    else if ( styles.gettok( i ) == "arrowkeys" )
      *Styles |= UDS_ARROWKEYS;
    else if ( styles.gettok( i ) == "horz" )
      *Styles |= UDS_HORZ;
    else if ( styles.gettok( i ) == "hottrack" )
      *Styles |= UDS_HOTTRACK;
    else if ( styles.gettok( i ) == "nothousands" )
      *Styles |= UDS_NOTHOUSANDS;
    else if ( styles.gettok( i ) == "buddyint" )
      *Styles |= UDS_SETBUDDYINT;
    else if ( styles.gettok( i ) == "wrap" )
      *Styles |= UDS_WRAP;

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

void DcxUpDown::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( );
	TString prop(input.gettok( 3 ));

  // [NAME] [ID] [PROP]
  if ( prop == "value" ) {

    BOOL bError;
    wsprintf( szReturnValue, "%d", this->getPos32( &bError ) );
    return;
  }
  // [NAME] [ID] [PROP]
  else if ( prop == "range" ) {

    int iMin, iMax;
    this->getRange32( &iMin, &iMax );
    wsprintf( szReturnValue, "%d %d", iMin, iMax );
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

void DcxUpDown::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  parseSwitchFlags(input.gettok(3), &flags);

  int numtok = input.numtok( );

  // xdid -c [NAME] [ID] [SWITCH] [BUDDYID]
  if ( flags.switch_flags[2] && numtok > 3 ) {
    
    DcxControl * p_Control = this->m_pParentDialog->getControlByID( (UINT) input.gettok( 4 ).to_int( ) + mIRC_ID_OFFSET );

    if ( p_Control != NULL ) {

      char ClassName[256];
      GetClassName( p_Control->getHwnd( ), ClassName, 256 );

      // Text notifications
      if ( lstrcmpi( "STATIC", ClassName ) == 0 || lstrcmpi( "EDIT", ClassName ) == 0 ) {

        this->setBuddy( p_Control->getHwnd( ) );
      }
    }
  }
  // xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
  else if ( flags.switch_flags[17] && numtok > 4 ) {
    
    int iMin = input.gettok( 4 ).to_int( );
    int iMax = input.gettok( 5 ).to_int( );

    this->setRange32( iMin, iMax );
  }
  // xdid -t [NAME] [ID] [SWITCH] [BASE]
  else if ( flags.switch_flags[19] && numtok > 3 ) {
    
    int nBase = input.gettok( 4 ).to_int( );

    this->setBase( nBase );
  }
  // xdid -v [NAME] [ID] [SWITCH] [POS]
  else if ( flags.switch_flags[21] && numtok > 3 ) {
    
    int nPos = input.gettok( 4 ).to_int( );

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
				        this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID( ) );
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
