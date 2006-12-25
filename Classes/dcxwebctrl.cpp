/*!
 * \file dcxwebctrl.cpp
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

#include "dcxwebctrl.h"
#include "dcxdialog.h"

extern IClassFactory * g_pClassFactory;

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxWebControl::DcxWebControl( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles,
    "STATIC",
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | Styles,
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL),
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

  /* Web Control Stuff */

  if( g_pClassFactory	!= NULL &&
      SUCCEEDED( g_pClassFactory->CreateInstance( 0, IID_IWebBrowser2, (void**) &this->m_pWebBrowser2 ) ) && 
      SUCCEEDED( this->m_pWebBrowser2->QueryInterface( IID_IOleObject, (LPVOID*) &this->m_pOleObject ) ) && 
      SUCCEEDED( this->m_pWebBrowser2->QueryInterface( IID_IOleInPlaceObject, (LPVOID*) &this->m_pOleInPlaceObject ) ) && 
      SUCCEEDED( this->m_pWebBrowser2->QueryInterface( IID_IConnectionPointContainer, (LPVOID*) &this->m_pCPC ) ) && 
      SUCCEEDED( this->m_pOleObject->SetClientSite( (IOleClientSite*) this ) ) && 
      SUCCEEDED( this->m_pCPC->FindConnectionPoint( DIID_DWebBrowserEvents2, &this->m_pCP ) ) &&
      SUCCEEDED( this->m_pCP->Advise( (IUnknown*)(IOleClientSite*) this, &this->m_dwCookie ) ) && 
      //SUCCEEDED( this->m_pOleObject->DoVerb( OLEIVERB_UIACTIVATE, 0, (IOleClientSite*) this, 0, this->m_Hwnd, rc ) )
      SUCCEEDED( this->m_pOleObject->DoVerb( OLEIVERB_INPLACEACTIVATE, 0, (IOleClientSite*) this, 0, this->m_Hwnd, rc ) )
    )
  {
    //mIRCError( "Created Browser Window!!!" );
  }
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );

	TString url("about:blank");
  VARIANT v;
  VariantInit( &v );			
  this->m_pWebBrowser2->Navigate( url.to_wchr(), &v, &v, &v, &v );  // dont use L""
  VariantClear( &v );

}

/*!
 * \brief blah
 *
 * blah
 */

DcxWebControl::~DcxWebControl( ) {

  //Release all Web Control pointers
  if ( this->m_dwCookie )
    this->m_pCP->Unadvise( this->m_dwCookie );

  this->m_pCP->Release( );
  this->m_pCPC->Release( );
  this->m_pOleInPlaceObject->Release( );
  if ( this->m_pOleObject != NULL ) {

    this->m_pOleObject->Close( OLECLOSE_NOSAVE );
    this->m_pOleObject->Release( );
  }
  this->m_pWebBrowser2->Release( );

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxWebControl::parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme) {
	//unsigned int i = 1, numtok = styles.numtok( " " );

  /*
  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "left" ) {
      *Styles &= ~UDS_ALIGNRIGHT;
      *Styles |= UDS_ALIGNLEFT;
    }
    else if ( styles.gettok( i , " " ) == "arrowkeys" )
      *Styles |= UDS_ARROWKEYS;
    else if ( styles.gettok( i , " " ) == "horz" )
      *Styles |= UDS_HORZ;
    else if ( styles.gettok( i , " " ) == "hottrack" )
      *Styles |= UDS_HOTTRACK;
    else if ( styles.gettok( i , " " ) == "nothousands" )
      *Styles |= UDS_NOTHOUSANDS;
    else if ( styles.gettok( i , " " ) == "buddyint" )
      *Styles |= UDS_SETBUDDYINT;
    else if ( styles.gettok( i , " " ) == "wrap" )
      *Styles |= UDS_WRAP;

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

void DcxWebControl::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( " " );

  // [NAME] [ID] [PROP]
  if ( input.gettok( 3, " " ) == "url" ) {

    BSTR str;

    if( SUCCEEDED( this->m_pWebBrowser2->get_LocationURL( &str ) ) ) {

      wsprintf( szReturnValue, "%ws", str );
      SysFreeString( str );
      return;
    }
  }
  // [NAME] [ID] [PROP]
  else if ( input.gettok( 3, " " ) == "ready" ) {
  
    READYSTATE ready_state;
    if ( SUCCEEDED( this->m_pWebBrowser2->get_ReadyState( &ready_state ) ) ) {
			
      if ( ready_state == READYSTATE_COMPLETE ) {

        lstrcpy( szReturnValue, "$true" );
		    return;
      }
		}
    lstrcpy( szReturnValue, "$false" );
		return;
  }
  else if ( this->parseGlobalInfoRequest( input, szReturnValue ) ) {

    return;
  }
  
  szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxWebControl::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  // xdid -g [NAME] [ID] [SWITCH]
  if ( flags.switch_flags[6] ) {

    this->m_pWebBrowser2->GoHome( );
  }
  // xdid -i [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[8] ) {

    this->m_pWebBrowser2->GoForward( );
  }
  // xdid -j [NAME] [ID] [SWITCH] [JAVASCRIPT]
  else if ( flags.switch_flags[9] && numtok > 3 ) {

    READYSTATE ready_state;

    if ( SUCCEEDED( this->m_pWebBrowser2->get_ReadyState( &ready_state ) ) && 
      ready_state == READYSTATE_COMPLETE )
    {

      IDispatch  * htmlDisp = NULL;

      if ( SUCCEEDED(this->m_pWebBrowser2->get_Document( &htmlDisp ))) {

	      IHTMLDocument2 * doc = NULL;
        if ( SUCCEEDED(htmlDisp->QueryInterface( IID_IHTMLDocument2, (void**) &doc ))) {

          IHTMLWindow2 * window;

          if ( SUCCEEDED( doc->get_parentWindow( &window ) ) ) { 

            TString CMD(input.gettok( 4, -1, " " ));
            CMD.trim( );

            VARIANT v;
            VariantInit( &v );
						window->execScript( CMD.to_wchr(), NULL, &v );
            VariantClear( &v );

            window->Release( );
          }
					doc->Release( );
        }
				htmlDisp->Release( );
      }
    }
		else
			DCXError("/xdid -j","Browser NOT in Ready State");
  }
  // xdid -k [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[10] ) {

    this->m_pWebBrowser2->GoBack( );
  }
  // xdid -n [NAME] [ID] [SWITCH] [URL]
  else if ( flags.switch_flags[13] && numtok > 3 ) {
    
    TString URL(input.gettok( 4, -1, " " ));
    URL.trim( );

    VARIANT v;
    VariantInit( &v );			
		this->m_pWebBrowser2->Navigate( URL.to_wchr(), &v, &v, &v, &v );
    VariantClear( &v );
  }
  // xdid -r [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[17] ) {

    this->m_pWebBrowser2->Refresh( );
  }
  // xdid -t [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[19] ) {

    this->m_pWebBrowser2->Stop( );
  }
  else
    this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

HRESULT DcxWebControl::Invoke( DISPID dispIdMember, 
                               REFIID riid, 
                               LCID lcid, 
                               WORD wFlags, 
                               DISPPARAMS __RPC_FAR * pDispParams, 
                               VARIANT __RPC_FAR * pVarResult, 
                               EXCEPINFO __RPC_FAR * pExcepInfo, 
                               UINT __RPC_FAR * puArgErr ) 
{

	UINT err;
	VARIANT arg1, arg2;
	VariantInit( &arg1 );
	VariantInit( &arg2 );
	DispGetParam( pDispParams, 0, VT_BSTR, &arg1, &err );
	DispGetParam( pDispParams, 1, VT_BSTR, &arg2, &err );
	
  switch (dispIdMember) {
	
	case DISPID_NAVIGATECOMPLETE2:
    {
      this->callAliasEx( NULL, "%s,%d,%ws", "nav_complete", this->getUserID( ), arg2.bstrVal );
    }
		break;
    
	case DISPID_BEFORENAVIGATE2:
    {
      char ret[256];
      this->callAliasEx( ret, "%s,%d,%ws", "nav_begin", this->getUserID( ), arg2.bstrVal );

      if ( lstrcmpi( ret, "cancel") == 0 )
        *pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
      else
        *pDispParams->rgvarg->pboolVal = VARIANT_FALSE;
    }
		break;

	case DISPID_DOCUMENTCOMPLETE:
    {
      this->callAliasEx( NULL, "%s,%d,%ws", "doc_complete", this->getUserID( ), arg2.bstrVal );
    }
		break;

	case DISPID_DOWNLOADBEGIN:
    {
      this->callAliasEx( NULL, "%s,%d", "dl_begin", this->getUserID( ) );
    }
		break;

	case DISPID_DOWNLOADCOMPLETE:
    {
      this->callAliasEx( NULL, "%s,%d", "dl_complete", this->getUserID( ) );
    }
		break;

	case DISPID_NEWWINDOW2:
    {
      char ret[256];
      this->callAliasEx( ret, "%s,%d", "win_open", this->getUserID( ) );

      if ( lstrcmpi( ret, "cancel") == 0 )
        *pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
      else
        *pDispParams->rgvarg->pboolVal = VARIANT_FALSE;
    }
		break;

	case DISPID_STATUSTEXTCHANGE:
    {
      this->callAliasEx( NULL, "%s,%d,%ws", "status", this->getUserID( ), arg1.bstrVal );
    }
		break;

	case DISPID_TITLECHANGE:
    {
      this->callAliasEx( NULL, "%s,%d,%ws", "title", this->getUserID( ), arg1.bstrVal );
    }
		break;

	case DISPID_PROGRESSCHANGE:
    {
      this->callAliasEx( NULL, "%s,%d,%ws,%ws", "dl_progress", this->getUserID( ), arg1.bstrVal, arg2.bstrVal );
    }
		break;

	case DISPID_COMMANDSTATECHANGE:
    {
      switch( arg1.bstrVal[0] ) {

		    case L'1':
          {
            this->callAliasEx( NULL, "%s,%d,%s", "forward", this->getUserID( ), arg2.boolVal ? "$true" : "$false" );
          }
			    break;

		    case L'2':
          {
            this->callAliasEx( NULL, "%s,%d,%s", "back", this->getUserID( ), arg2.boolVal ? "$true" : "$false" );
          }
          break;
		  }
    }
    break;
	}

	VariantClear( &arg1 );
	VariantClear( &arg2 );

  return S_OK;
}

/*!
 * \brief blah
 *
 * blah
 */

HRESULT STDMETHODCALLTYPE DcxWebControl::QueryInterface( REFIID riid, void __RPC_FAR *__RPC_FAR * ppvObject ) {

	*ppvObject = NULL;
	
	if ( IID_IUnknown == riid ) { 
    *ppvObject = (IUnknown*)(IOleClientSite*) this; 
  }
	else if ( IID_IOleInPlaceSite == riid ) { 
    *ppvObject = (IOleInPlaceSite*) this; 
  }
	else if ( DIID_DWebBrowserEvents2 == riid ) { 
    *ppvObject = (DWebBrowserEvents2*) this; 
  }
	
	return *ppvObject ? S_OK : E_NOINTERFACE;
}

/*!
 * \brief blah
 *
 * blah
 */

HRESULT STDMETHODCALLTYPE DcxWebControl::GetWindowContext( IOleInPlaceFrame __RPC_FAR *__RPC_FAR *ppFrame, 
                                                          IOleInPlaceUIWindow __RPC_FAR *__RPC_FAR * ppDoc, 
                                                          LPRECT pPR, 
                                                          LPRECT pCR, 
                                                          LPOLEINPLACEFRAMEINFO pFI ) 
{
	
  *ppFrame = NULL;
	*ppDoc = NULL;
	
  GetClientRect( this->m_Hwnd, pPR );
  CopyRect( pCR, pPR ); 
  /*
	//set client area (updated in subclass_wnd_proc)
	GetClientRect(parent,pPR);
	reduce_rect(pPR,&margins);
	*pCR = *pPR;
  */
	
	return S_OK;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxWebControl::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxWebControl::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
	        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_CHILDACTIVATE:
    case WM_SIZE:
      {
			  RECT rc; 
        GetClientRect( this->m_Hwnd, &rc );
			  this->m_pOleInPlaceObject->SetObjectRects( &rc, &rc );
      }
			break;

    case WM_MOUSEMOVE:
      {
        this->m_pParentDialog->setMouseControl( this->getUserID( ) );
      }
      break;

    case WM_SETFOCUS:
      {
        this->m_pParentDialog->setFocusControl( this->getUserID( ) );
      }
      break;
		case WM_MOUSEACTIVATE:
			{
				bParsed = TRUE;
				//HWND web = NULL;
				//HRESULT r = this->m_pWebBrowser2->get_HWND((long *)&web);
				//if (SUCCEEDED(r)) {
				//	mIRCError("worked");
				//	SetActiveWindow(web);
				//}
				//else
				//	mIRCDebug("no activate r %d web %d", r, web);
				//SetActiveWindow((HWND)wParam);
				return MA_ACTIVATE;
			}
			break;
		//case WM_GETDLGCODE:
		//		{
		//			bParsed = TRUE;
		//			return DLGC_WANTALLKEYS;
		//		}
		//		break;
    case WM_DESTROY:
      {
        delete this;
        bParsed = TRUE;
      }
      break;

    default:
      break;
  }

  return 0L;
}