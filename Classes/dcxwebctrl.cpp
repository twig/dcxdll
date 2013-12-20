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
#include "defines.h"
#include "Classes/dcxwebctrl.h"
#include "Classes/dcxdialog.h"

//extern IClassFactory * g_pClassFactory;

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
, m_bHideEvents(true)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles,
		TEXT("STATIC"),
		NULL,
		WS_CHILD | WS_CLIPSIBLINGS | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw TEXT("Unable To Create Window");

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	/* Web Control Stuff */

	if( Dcx::getClassFactory() != NULL &&
		SUCCEEDED( Dcx::getClassFactory()->CreateInstance( 0, IID_IWebBrowser2, (void**) &this->m_pWebBrowser2 ) ) && 
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
		this->registreDefaultWindowProc( );
		SetProp( this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );

		BSTR url = SysAllocString(TEXT("about:blank"));
		if (url != NULL) {
			VARIANT v;
			VariantInit( &v );
			this->m_pWebBrowser2->Navigate( url, &v, &v, &v, &v );  // dont use L""

			VariantClear( &v );
			SysFreeString(url);
		}
	}
	else {
		//Release all Web Control pointers
		if ( this->m_dwCookie )
			this->m_pCP->Unadvise( this->m_dwCookie );

		if (this->m_pCP != NULL)
			this->m_pCP->Release( );
		if (this->m_pCPC != NULL)
			this->m_pCPC->Release( );
		if (this->m_pOleInPlaceObject != NULL)
			this->m_pOleInPlaceObject->Release( );
		if ( this->m_pOleObject != NULL ) {

			this->m_pOleObject->Close( OLECLOSE_NOSAVE );
			this->m_pOleObject->Release( );
		}
		if (this->m_pWebBrowser2 != NULL) {
			this->m_pWebBrowser2->Quit();
			this->m_pWebBrowser2->Release( );
		}
		DestroyWindow(this->m_Hwnd);
		throw TEXT("Unable To Create Browser Window");
	}
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
	this->m_pWebBrowser2->Quit();
	this->m_pWebBrowser2->Release( );

	this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxWebControl::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme) {

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

void DcxWebControl::parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const
{
	const TString prop(input.getfirsttok( 3 ));

	// [NAME] [ID] [PROP]
	if ( prop == TEXT("url") ) {

		BSTR str;

		if( SUCCEEDED( this->m_pWebBrowser2->get_LocationURL( &str ) ) ) {

			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ws"), str ); // possible overflow, needs fixing at some point.
			SysFreeString( str );
			return;
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("ready") ) {

		READYSTATE ready_state;
		if ( SUCCEEDED( this->m_pWebBrowser2->get_ReadyState( &ready_state ) ) ) {

			if ( ready_state == READYSTATE_COMPLETE ) {

				lstrcpyn( szReturnValue, TEXT("$true"), MIRC_BUFFER_SIZE_CCH );
				return;
			}
		}
		lstrcpyn( szReturnValue, TEXT("$false"), MIRC_BUFFER_SIZE_CCH );
		return;
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("statusbar") ) {

		VARIANT_BOOL bState;
		if ( SUCCEEDED( this->m_pWebBrowser2->get_StatusBar(&bState) ) ) {

			if ( bState == VARIANT_TRUE ) {

				lstrcpyn( szReturnValue, TEXT("$true"), MIRC_BUFFER_SIZE_CCH );
				return;
			}
		}
		lstrcpyn( szReturnValue, TEXT("$false"), MIRC_BUFFER_SIZE_CCH );
		return;
	}
	else if ( prop == TEXT("statustext") ) {

		BSTR str;

		if( SUCCEEDED( this->m_pWebBrowser2->get_StatusText( &str ) ) ) {

			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ws"), str ); // possible overflow, needs fixing at some point.
			SysFreeString( str );
			return;
		}
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

void DcxWebControl::parseCommandRequest( const TString & input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const UINT numtok = input.numtok( );

	// xdid -g [NAME] [ID] [SWITCH]
	if ( flags[TEXT('g')] ) {

		this->m_pWebBrowser2->GoHome( );
	}
	// xdid -i [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('i')] ) {

		this->m_pWebBrowser2->GoForward( );
	}
	// xdid -j [NAME] [ID] [SWITCH] [JAVASCRIPT]
	else if ( flags[TEXT('j')] && numtok > 3 ) {

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

						const TString CMD(input.gettok( 4, -1 ).trim());

						VARIANT v;
						VariantInit( &v );
						BSTR strCMD = SysAllocString(CMD.to_chr());
						if (strCMD != NULL) {
							window->execScript( strCMD, NULL, &v );
							SysFreeString(strCMD);
						}
						VariantClear( &v );

						window->Release( );
					}
					doc->Release( );
				}
				htmlDisp->Release( );
			}
		}
		else
			this->showError(NULL, TEXT("-j"), TEXT("Browser NOT in Ready State"));
	}
	// xdid -k [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('k')] ) {

		this->m_pWebBrowser2->GoBack( );
	}
	// xdid -m [NAME] [ID] [SWITCH] [+FLAGS] [+MASK] (URL)
	// [NAME] [ID] -m [+FLAGS] [+MASK] (URL)
	else if ( flags[TEXT('m')] && numtok > 4 ) {

		const XSwitchFlags xflags(input.getnexttok());		// tok 4 flags to change
		const XSwitchFlags xmask(input.getnexttok());		// tok 5 state mask, flags here are enabled, otherwise they are disabled.
		const TString URL(input.gettok( 6, -1 ).trim());	// optional

		VARIANT vEmpty;
		VARIANT vFlags;
		VARIANT_BOOL bEnabled;
		VariantInit( &vEmpty );
		V_VT(&vFlags);
		V_I4(&vFlags) = 0;

		if (xflags['h'] && xmask['h']) // no history
			V_I4(&vFlags) |= navNoHistory;
		if (xflags['r'] && xmask['r']) // no read from cache
			V_I4(&vFlags) |= navNoReadFromCache;
		if (xflags['w'] && xmask['w']) // no write to cache
			V_I4(&vFlags) |= navNoWriteToCache;
		if (xflags['a'] && xmask['a']) // allow auto search
			V_I4(&vFlags) |= navAllowAutosearch;
		if (xflags['e'] && xmask['e']) // enforce restricted zone
			V_I4(&vFlags) |= navEnforceRestricted;
		if (xflags['m'] && xmask['m']) // Manage popup windows.
			V_I4(&vFlags) |= navNewWindowsManaged;
		if (xflags['u'] && xmask['u']) // dont allow downloads.
			V_I4(&vFlags) |= navUntrustedForDownload;
		if (xflags['x'] && xmask['x']) // allow active x install prompts (doesnt auto install, you still need to ok the prompt)
			V_I4(&vFlags) |= navTrustedForActiveX;
		if (xflags['b']) { // toggle address bar on/off
			if (xmask['b'])
				bEnabled = VARIANT_TRUE;
			else
				bEnabled = VARIANT_FALSE;
			this->m_pWebBrowser2->put_AddressBar(bEnabled);
		}
		if (xflags['f']) { // fullscreen on/off
			if (xmask['f'])
				bEnabled = VARIANT_TRUE;
			else
				bEnabled = VARIANT_FALSE;
			this->m_pWebBrowser2->put_FullScreen(bEnabled);
		}
		if (xflags['s']) { // statusbar on/off
			if (xmask['s'])
				bEnabled = VARIANT_TRUE;
			else
				bEnabled = VARIANT_FALSE;
			this->m_pWebBrowser2->put_StatusBar(bEnabled);
		}
		// only open url if one supplied.
		if (URL.len() > 0) {
			BSTR bstrUrl = SysAllocString(URL.to_chr());
			if (bstrUrl != NULL) {
				this->m_pWebBrowser2->Navigate( bstrUrl, &vFlags, &vEmpty, &vEmpty, &vEmpty );
				SysFreeString(bstrUrl);
			}
		}

		VariantClear( &vEmpty );
	}
	// xdid -n [NAME] [ID] [SWITCH] [URL]
	// [NAME] [ID] -n [URL]
	else if ( flags[TEXT('n')] && numtok > 3 ) {

		const TString URL(input.gettok( 4, -1 ).trim());

		VARIANT v;
		VariantInit( &v );
		BSTR bstrUrl = SysAllocString(URL.to_chr());
		if (bstrUrl != NULL) {
			this->m_pWebBrowser2->Navigate( bstrUrl, &v, &v, &v, &v );
			SysFreeString(bstrUrl);
		}

		VariantClear( &v );
	}
	// xdid -r [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('r')] ) {

		this->m_pWebBrowser2->Refresh( );
	}
	// xdid -t [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('t')] ) {

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
	//if (this->m_bHideEvents) return S_OK; // cant do this here :/

	UINT err;
	VARIANT arg1, arg2;
	VariantInit( &arg1 );
	VariantInit( &arg2 );
	if (SUCCEEDED(DispGetParam(pDispParams, 0, VT_BSTR, &arg1, &err)) && SUCCEEDED(DispGetParam(pDispParams, 1, VT_BSTR, &arg2, &err)))
	{
		switch (dispIdMember) {
		case DISPID_NAVIGATECOMPLETE2:
		{
			if (!this->m_bHideEvents)
				this->execAliasEx(TEXT("%s,%d,%ws"), TEXT("nav_complete"), this->getUserID(), arg2.bstrVal);
		}
		break;

		case DISPID_BEFORENAVIGATE2:
		{
			if (!this->m_bHideEvents)
			{
				TCHAR ret[256];
				this->evalAliasEx(ret, 255, TEXT("%s,%d,%ws"), TEXT("nav_begin"), this->getUserID(), arg2.bstrVal);

				if (lstrcmpi(ret, TEXT("cancel")) == 0)
					*pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
				else
					*pDispParams->rgvarg->pboolVal = VARIANT_FALSE;
			}
		}
		break;

		case DISPID_DOCUMENTCOMPLETE:
		{
			if (!this->m_bHideEvents)
				this->execAliasEx(TEXT("%s,%d,%ws"), TEXT("doc_complete"), this->getUserID(), arg2.bstrVal);
			else
				this->m_bHideEvents = false; // allow events to be seen after first doc loads `about:blank`
		}
		break;

		case DISPID_DOWNLOADBEGIN:
		{
			if (!this->m_bHideEvents)
				this->execAliasEx(TEXT("%s,%d"), TEXT("dl_begin"), this->getUserID());
		}
		break;

		case DISPID_DOWNLOADCOMPLETE:
		{
			if (!this->m_bHideEvents)
				this->execAliasEx(TEXT("%s,%d"), TEXT("dl_complete"), this->getUserID());
		}
		break;

		case DISPID_NEWWINDOW2:
		{
			if (!this->m_bHideEvents)
			{
				TCHAR ret[256];
				this->evalAliasEx(ret, 255, TEXT("%s,%d"), TEXT("win_open"), this->getUserID());

				if (lstrcmpi(ret, TEXT("cancel")) == 0)
					*pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
				else
					*pDispParams->rgvarg->pboolVal = VARIANT_FALSE;
			}
		}
		break;

		case DISPID_STATUSTEXTCHANGE:
		{
			if (!this->m_bHideEvents)
				this->execAliasEx(TEXT("%s,%d,%ws"), TEXT("status"), this->getUserID(), arg1.bstrVal);
		}
		break;

		case DISPID_TITLECHANGE:
		{
			if (!this->m_bHideEvents)
				this->execAliasEx(TEXT("%s,%d,%ws"), TEXT("title"), this->getUserID(), arg1.bstrVal);
		}
		break;

		case DISPID_PROGRESSCHANGE:
		{
			if (!this->m_bHideEvents)
				this->execAliasEx(TEXT("%s,%d,%ws,%ws"), TEXT("dl_progress"), this->getUserID(), arg1.bstrVal, arg2.bstrVal);
		}
		break;

		case DISPID_COMMANDSTATECHANGE:
		{
			if (!this->m_bHideEvents)
			{
				switch (arg1.bstrVal[0]) {
					case L'1':
						this->execAliasEx(TEXT("%s,%d,%s"), TEXT("forward"), this->getUserID(), arg2.boolVal ? TEXT("$true") : TEXT("$false"));
						break;

					case L'2':
						this->execAliasEx(TEXT("%s,%d,%s"), TEXT("back"), this->getUserID(), arg2.boolVal ? TEXT("$true") : TEXT("$false"));
						break;
				}
			}
		}
		break;
		}
		VariantClear(&arg1);
		VariantClear(&arg2);
	}
	else {
		this->showErrorEx(NULL, TEXT("DcxWebControl::Invoke()"), TEXT("Unable to get object state: %ld"), err);
	}

	return S_OK;
}

/*!
 * \brief blah
 *
 * blah
 */

HRESULT STDMETHODCALLTYPE DcxWebControl::QueryInterface( REFIID riid, void __RPC_FAR *__RPC_FAR * ppvObject )
{
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
LRESULT DcxWebControl::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
{
	return 0L;
}

LRESULT DcxWebControl::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
{
#if DCX_DEBUG_OUTPUT
	Dcx::mIRC.signalex(true,TEXT("debug %ld"), uMsg);
#endif
	switch( uMsg ) {

	case WM_CHILDACTIVATE:
	case WM_SIZE:
		{
			RECT rc; 
			GetClientRect( this->m_Hwnd, &rc );
			this->m_pOleInPlaceObject->SetObjectRects( &rc, &rc );
			//return 0L;
		}
		break;

	case WM_MOUSEACTIVATE:
		{
			bParsed = TRUE;
			//HWND web = NULL;
			//HRESULT r = this->m_pWebBrowser2->get_HWND((long *)&web);
			//if (SUCCEEDED(r)) {
			//	mIRCError(TEXT("worked"));
			//	SetActiveWindow(web);
			//}
			//else
			//	mIRCDebug(TEXT("no activate r %d web %d"), r, web);
			//SetActiveWindow((HWND)wParam);
			//SetFocus((HWND)wParam);
			return MA_NOACTIVATE;
		}
		break;
		//case WM_GETDLGCODE:
		//		{
		//			bParsed = TRUE;
		//			return DLGC_WANTALLKEYS;
		//		}
		//		break;
		//case WM_HSCROLL:
		//case WM_VSCROLL:
		//	{
		//		mIRCDebug(TEXT("scroll"));
		//	}
		//	break;
		//case WM_MOUSEWHEEL:
		//	{
		//		mIRCDebug(TEXT("wheel"));
		//	}
		//	break;

		//case WM_NCHITTEST:
		//	{
		//		bParsed = TRUE;
		//		return HTCLIENT;
		//		//return DefWindowProc( this->m_Hwnd, uMsg, wParam, lParam);
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
