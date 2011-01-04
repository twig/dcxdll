/*!
 * \file dcxipaddress.cpp
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
#include "Classes/dcxipaddress.h"
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

DcxIpAddress::DcxIpAddress( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles, 
		DCX_IPADDRESSCLASS, 
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
		Dcx::XPPlusModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
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

	// fix bug with disabled creation
	// todo: fix this properly
	if (Styles & WS_DISABLED) {
		EnableWindow(this->m_Hwnd, TRUE);
		EnableWindow(this->m_Hwnd, FALSE);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

DcxIpAddress::~DcxIpAddress( ) {

	this->unregistreDefaultWindowProc( );
}

void DcxIpAddress::toXml(TiXmlElement * xml) {
	DWORD ip;
	char buf[64];
	this->getAddress( &ip );
	wnsprintf( buf, 64, "%d.%d.%d.%d", FIRST_IPADDRESS( ip ),
		SECOND_IPADDRESS( ip ),
		THIRD_IPADDRESS( ip ),
		FOURTH_IPADDRESS( ip ) );
	__super::toXml(xml);
	xml->SetAttribute("caption", buf);

}

/*!
 * \brief blah
 *
 * blah
 */

void DcxIpAddress::parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme) {
	//unsigned int i = 1, numtok = styles.numtok( );
/*
  while ( i <= numtok ) {

    if ( styles.gettok( i ) == "bitmap" )
      *Styles |= BS_BITMAP;
    else if ( styles.gettok( i ) == "default" )
      *Styles |= BS_DEFPUSHBUTTON;

    i++;
  }
*/

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

void DcxIpAddress::parseInfoRequest( TString & input, char * szReturnValue ) {

	// [NAME] [ID] [PROP]
	if ( input.gettok( 3 ) == "ip" ) {

		DWORD ip;
		this->getAddress( &ip );

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, "%d.%d.%d.%d", FIRST_IPADDRESS( ip ),
			SECOND_IPADDRESS( ip ),
			THIRD_IPADDRESS( ip ),
			FOURTH_IPADDRESS( ip ) );


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

void DcxIpAddress::parseCommandRequest(TString &input) {
	XSwitchFlags flags(input.gettok(3));

	int numtok = input.numtok( );

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags['r']) {
		this->clearAddress();
	}

	// xdid -a [NAME] [ID] [SWITCH] IP.IP.IP.IP
	if (flags['a'] && numtok > 3) {
		TString IP(input.gettok(4).trim());

		if (IP.numtok(".") == 4) {
			BYTE b[4];

			for (int i = 0; i < 4; i++) {
				b[i] = (BYTE) IP.gettok(i +1, ".").to_int();
			}

			DWORD adr = MAKEIPADDRESS(b[0], b[1], b[2], b[3]);
			this->setAddress(adr);
		}
	}
	// xdid -g [NAME] [ID] [SWITCH] [N] [MIN] [MAX]
	else if (flags['g'] && numtok > 5) {
		int nField	= input.gettok( 4 ).to_int() -1;
		BYTE min		= (BYTE)input.gettok( 5 ).to_int();
		BYTE max		= (BYTE)input.gettok( 6 ).to_int();

		if (nField > -1 && nField < 4)
			this->setRange(nField, min, max);
	}
	// xdid -j [NAME] [ID] [SWITCH] [N]
	else if (flags['j'] && numtok > 3) {
		int nField = input.gettok( 4 ).to_int() -1;

		if (nField > -1 && nField < 4)
			this->setFocus(nField);
	}
	// This is to avoid invalid flag message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags['r']) {
		//this->clearAddress();
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::setRange( const int nField, const BYTE iMin, const BYTE iMax ) {
  return SendMessage( this->m_Hwnd, IPM_SETRANGE, (WPARAM) nField, (LPARAM) MAKEIPRANGE( iMin, iMax ) );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::setFocus( const int nField ) {
  return SendMessage( this->m_Hwnd, IPM_SETFOCUS, (WPARAM) nField, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::setAddress( const DWORD dwIpAddress ) {
  return SendMessage( this->m_Hwnd, IPM_SETADDRESS, (WPARAM) 0, (LPARAM) dwIpAddress );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::getAddress( LPDWORD lpdwIpAddress ) const {
  return SendMessage( this->m_Hwnd, IPM_GETADDRESS, (WPARAM) 0, (LPARAM) lpdwIpAddress );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxIpAddress::clearAddress( ) {
  return SendMessage( this->m_Hwnd, IPM_CLEARADDRESS, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxIpAddress::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
  switch( uMsg ) {
    case WM_NOTIFY : 
      {
        LPNMHDR hdr = (LPNMHDR) lParam;

        if (!hdr)
          break;

        switch( hdr->code ) {
          case IPN_FIELDCHANGED:
            {
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_EDIT)
	              this->execAliasEx("%s,%d", "edit", this->getUserID( ) );
              bParsed = TRUE;
            }
            break;
        }
      }
      break;
	}
	return 0L;
}

LRESULT DcxIpAddress::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_CONTEXTMENU:
		case WM_SETCURSOR:
			break;

		case WM_MOUSEACTIVATE:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
					switch (HIWORD(lParam))
					{
					case WM_LBUTTONUP:
						{
							this->execAliasEx("%s,%d", "sclick", this->getUserID( ) );
						}
						break;
					case WM_RBUTTONUP:
						{
							this->execAliasEx("%s,%d", "rclick", this->getUserID( ) );
						}
						break;
					}
				}
				bParsed = TRUE;
				return MA_NOACTIVATE;
			}
			break;
		//case WM_SIZE:
		//	{
		//		this->redrawWindow();
		//	}
		//	break;
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
