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

DcxWebControl::DcxWebControl(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles )
	: DcxControl(ID, p_Dialog)
	, m_bHideEvents(true)
	, m_dwCookie(0)
	, m_pCP(nullptr)
	, m_pCPC(nullptr)
	, m_pOleInPlaceObject(nullptr)
	, m_pOleObject(nullptr)
	, m_pWebBrowser2(nullptr)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;

	parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	m_Hwnd = CreateWindowEx(	
		ExStyles,
		TEXT("STATIC"),
		nullptr,
		WS_CHILD | WS_CLIPSIBLINGS | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(nullptr),
		nullptr);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( m_Hwnd , L" ", L" " );

	/* Web Control Stuff */

	if( Dcx::getClassFactory() != nullptr &&
		SUCCEEDED( Dcx::getClassFactory()->CreateInstance( 0, IID_IWebBrowser2, (void**) &m_pWebBrowser2 ) ) && 
		SUCCEEDED( m_pWebBrowser2->QueryInterface( IID_IOleObject, (LPVOID*) &m_pOleObject ) ) && 
		SUCCEEDED( m_pWebBrowser2->QueryInterface( IID_IOleInPlaceObject, (LPVOID*) &m_pOleInPlaceObject ) ) && 
		SUCCEEDED( m_pWebBrowser2->QueryInterface( IID_IConnectionPointContainer, (LPVOID*) &m_pCPC ) ) && 
		SUCCEEDED( m_pOleObject->SetClientSite( (IOleClientSite*) this ) ) && 
		SUCCEEDED( m_pCPC->FindConnectionPoint( DIID_DWebBrowserEvents2, &m_pCP ) ) &&
		SUCCEEDED( m_pCP->Advise( (IUnknown*)(IOleClientSite*) this, &m_dwCookie ) ) && 
		//SUCCEEDED( m_pOleObject->DoVerb( OLEIVERB_UIACTIVATE, 0, (IOleClientSite*) this, 0, m_Hwnd, rc ) )
		SUCCEEDED( m_pOleObject->DoVerb( OLEIVERB_INPLACEACTIVATE, 0, (IOleClientSite*) this, 0, m_Hwnd, rc ) )
		)
	{
		registreDefaultWindowProc( );
		SetProp( m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );

#if DCX_USE_WRAPPERS
		Dcx::dcxBSTRResource url(TEXT("about:blank"));

		VARIANT v;
		VariantInit(&v);
		Auto(VariantClear(&v));

		m_pWebBrowser2->Navigate(url, &v, &v, &v, &v);  // dont use L""
#else
		auto url = SysAllocString(TEXT("about:blank"));
		if (url != nullptr) {
			Auto(SysFreeString(url));

			VARIANT v;
			VariantInit( &v );
			Auto(VariantClear(&v));

			m_pWebBrowser2->Navigate( url, &v, &v, &v, &v );  // dont use L""
		}
#endif
	}
	else {
		//Release all Web Control pointers
		SafeRelease();

		DestroyWindow(m_Hwnd);
		throw Dcx::dcxException("Unable To Create Browser Window");
	}
}

/*!
 * \brief blah
 *
 * blah
 */

DcxWebControl::~DcxWebControl( ) {

	//Release all Web Control pointers
	//SafeRelease();	// causes an odd crash when IE11 is used...

	unregistreDefaultWindowProc( );
}

void DcxWebControl::SafeRelease() noexcept
{
	//Release all Web Control pointers
	if (m_pCP != nullptr)
	{
		if (m_dwCookie != 0UL)
		{
			m_pCP->Unadvise(m_dwCookie);

			m_dwCookie = 0UL;
		}

		m_pCP->Release();
		m_pCP = nullptr;
	}
	if (m_pCPC != nullptr)
	{
		m_pCPC->Release();
		m_pCPC = nullptr;
	}
	if (m_pOleInPlaceObject != nullptr)
	{
		m_pOleInPlaceObject->Release();
		m_pOleInPlaceObject = nullptr;
	}
	if (m_pOleObject != nullptr) {
		m_pOleObject->Close(OLECLOSE_NOSAVE);
		m_pOleObject->Release();
		m_pOleObject = nullptr;
	}
	if (m_pWebBrowser2 != nullptr) {
		m_pWebBrowser2->Quit();
		m_pWebBrowser2->Release();
		m_pWebBrowser2 = nullptr;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxWebControl::parseControlStyles( const TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme)
{
	parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxWebControl::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
#if DCX_USE_HASHING
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"url"_hash:
	{
		BSTR str;

		if (SUCCEEDED(this->m_pWebBrowser2->get_LocationURL(&str))) {

			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ws"), str); // possible overflow, needs fixing at some point.
			SysFreeString(str);
		}
	}
	break;
	// [NAME] [ID] [PROP]
	case L"ready"_hash:
	{
		READYSTATE ready_state;
		if (SUCCEEDED(this->m_pWebBrowser2->get_ReadyState(&ready_state))) {

			if (ready_state == READYSTATE_COMPLETE) {

				szReturnValue = TEXT("$true");
				return;
			}
		}
		szReturnValue = TEXT("$false");
	}
	break;
	// [NAME] [ID] [PROP]
	case L"statusbar"_hash:
	{
		VARIANT_BOOL bState;
		if (SUCCEEDED(this->m_pWebBrowser2->get_StatusBar(&bState))) {

			if (bState == VARIANT_TRUE) {

				szReturnValue = TEXT("$true");
				return;
			}
		}
		szReturnValue = TEXT("$false");
	}
	break;
	case L"statustext"_hash:
	{
		BSTR str;

		if (SUCCEEDED(this->m_pWebBrowser2->get_StatusText(&str))) {

			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ws"), str); // possible overflow, needs fixing at some point.
			SysFreeString(str);
		}
	}
	break;
	case L"script"_hash:
	{
		const auto tsRes(CallScript(input.getlasttoks()));
		if (!tsRes.empty())
			szReturnValue = tsRes.to_chr();
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
#else
	const auto prop(input.getfirsttok( 3 ));

	// [NAME] [ID] [PROP]
	if ( prop == TEXT("url") ) {

		BSTR str;

		if( SUCCEEDED( this->m_pWebBrowser2->get_LocationURL( &str ) ) ) {

			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ws"), str ); // possible overflow, needs fixing at some point.
			SysFreeString( str );
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("ready") ) {

		READYSTATE ready_state;
		if ( SUCCEEDED( this->m_pWebBrowser2->get_ReadyState( &ready_state ) ) ) {

			if ( ready_state == READYSTATE_COMPLETE ) {

				szReturnValue = TEXT("$true");
				return;
			}
		}
		szReturnValue = TEXT("$false");
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("statusbar") ) {

		VARIANT_BOOL bState;
		if ( SUCCEEDED( this->m_pWebBrowser2->get_StatusBar(&bState) ) ) {

			if ( bState == VARIANT_TRUE ) {

				szReturnValue = TEXT("$true");
				return;
			}
		}
		szReturnValue = TEXT("$false");
	}
	else if ( prop == TEXT("statustext") ) {

		BSTR str;

		if( SUCCEEDED( this->m_pWebBrowser2->get_StatusText( &str ) ) ) {

			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ws"), str ); // possible overflow, needs fixing at some point.
			SysFreeString( str );
		}
	}
	else
		this->parseGlobalInfoRequest(input, szReturnValue);
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxWebControl::parseCommandRequest( const TString & input) {
	const XSwitchFlags flags(input.getfirsttok( 3 ));
	const auto numtok = input.numtok();

	// xdid -g [NAME] [ID] [SWITCH]
	if ( flags[TEXT('g')] )
		m_pWebBrowser2->GoHome( );
	// xdid -i [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('i')] ) {
		m_pWebBrowser2->GoForward( );
	}
	// xdid -j [NAME] [ID] [SWITCH] [JAVASCRIPT]
	else if ( flags[TEXT('j')] && numtok > 3 )
	{
		const auto CMD(input.getlasttoks().trim());		// tok 4, -1

		CallScript(CMD);

//		READYSTATE ready_state;
//
//		if ( FAILED( m_pWebBrowser2->get_ReadyState( &ready_state ) ) || ready_state != READYSTATE_COMPLETE )
//			throw Dcx::dcxException("Browser NOT in Ready State");
//
//		IDispatch  * htmlDisp = nullptr;
//
//		if ( SUCCEEDED(m_pWebBrowser2->get_Document( &htmlDisp )))
//		{
//			Auto(htmlDisp->Release());
//
//			IHTMLDocument2 * doc = nullptr;
//			if ( SUCCEEDED(htmlDisp->QueryInterface( IID_IHTMLDocument2, (void**) &doc )))
//			{
//				Auto(doc->Release());
//
//				IHTMLWindow2 * window = nullptr;
//
//				if (SUCCEEDED(doc->get_parentWindow(&window)))
//				{
//					Auto(window->Release());
//
//					const auto CMD(input.getlasttoks().trim());		// tok 4, -1
//
//					//VARIANT v;
//					//VariantInit(&v);
//					//Auto(VariantClear(&v));
//
//					Dcx::dcxVariant v;
//
//#if DCX_USE_WRAPPERS
//					const Dcx::dcxBSTRResource strCMD(CMD.to_wchr());
//
//					//window->execScript(strCMD, nullptr, &v);	// this works well, but is deprecated in latest IE's?
//
//					IDispatch * idisp = nullptr;
//
//					if (SUCCEEDED(doc->get_Script(&idisp)))
//					{
//						Auto(idisp->Release());
//
//						DISPID dispid = -1;
//						DISPPARAMS params{};
//
//						VARIANT vArg;
//						VariantInit(&vArg);
//						Auto(VariantClear(&vArg));
//
//						//Dcx::dcxVariant vArg;
//
//						vArg.vt = VT_BSTR;
//						vArg.bstrVal = strCMD;
//						params.cArgs = 1;
//						params.rgvarg = &vArg;
//
//						const Dcx::dcxBSTRResource strEval(L"eval");
//						//LPOLESTR blah;
//						BSTR hm = strEval.get();
//						if (FAILED(idisp->GetIDsOfNames(IID_NULL, &hm, 1, LOCALE_SYSTEM_DEFAULT, &dispid)))
//							throw Dcx::dcxException("GetIDsOfNames: failed.");
//
//						if (dispid == DISPID_UNKNOWN)
//							throw Dcx::dcxException("GetIDsOfNames: Unable to find eval()");
//
//						if (FAILED(idisp->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD, &params, &v, NULL, NULL)))
//							throw Dcx::dcxException("Invoke: failed.");
//
//						//switch (v.vt)
//						//{
//						//case VT_BSTR:
//						//	wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ws"), v.bstrVal); // possible overflow, needs fixing at some point.
//						//	break;
//						//default:
//						//	break;
//						//}
//					}
//
//					// this version doesnt throw an error, but fails to work either....
//					//IDispatchEx * idisp = nullptr;
//					//
//					//if (SUCCEEDED(window->QueryInterface(IID_IDispatchEx, (void **)&idisp)))
//					//{
//					//	Auto(idisp->Release());
//					//
//					//	DISPID dispid = -1;
//					//	DISPPARAMS params{};
//					//
//					//	VARIANT vArg;
//					//	VariantInit(&vArg);
//					//	Auto(VariantClear(&vArg));
//					//
//					//	//Dcx::dcxVariant vArg;
//					//
//					//	vArg.vt = VT_BSTR;
//					//	vArg.bstrVal = strCMD;
//					//	params.cArgs = 1;
//					//	params.rgvarg = &vArg;
//					//
//					//	const Dcx::dcxBSTRResource strEval(L"alert");
//					//	LPOLESTR blah;
//					//	BSTR hm = strEval.get();
//					//	if (FAILED(idisp->GetIDsOfNames(IID_NULL, &hm, 1, LOCALE_SYSTEM_DEFAULT, &dispid)))
//					//		throw Dcx::dcxException("GetIDsOfNames: failed.");
//					//
//					//	if (dispid == DISPID_UNKNOWN)
//					//		throw Dcx::dcxException("GetIDsOfNames: Unable to find eval()");
//					//
//					//	if (FAILED(window->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD, &params, &v, NULL, NULL)))
//					//		throw Dcx::dcxException("Invoke: failed.");
//					//}
//
//					//DISPID dispid = -1;
//					//DISPPARAMS params{};
//					//
//					//VARIANT vArg;
//					//VariantInit(&vArg);
//					////Auto(VariantClear(&vArg));
//					//
//					////Dcx::dcxVariant vArg;
//					//
//					//vArg.vt = VT_BSTR;
//					//vArg.bstrVal = strCMD;
//					//params.cArgs = 1;
//					//params.rgvarg = &vArg;
//					//
//					//const Dcx::dcxBSTRResource strEval(L"eval");
//					////LPOLESTR blah;
//					//BSTR hm = strEval.get();
//					//if (FAILED(window->GetIDsOfNames(IID_NULL, &hm, 1, LOCALE_SYSTEM_DEFAULT, &dispid)))
//					//	throw Dcx::dcxException("GetIDsOfNames: failed.");
//					//
//					//if (dispid == DISPID_UNKNOWN)
//					//	throw Dcx::dcxException("GetIDsOfNames: Unable to find eval()");
//					//
//					//if (FAILED(window->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD, &params, &v, NULL, NULL)))
//					//	throw Dcx::dcxException("Invoke: failed.");
//#else
//					auto strCMD = SysAllocString(CMD.to_chr());
//					if (strCMD != nullptr)
//					{
//						Auto(SysFreeString(strCMD));
//
//						window->execScript(strCMD, nullptr, &v);
//					}
//#endif
//				}
//			}
//		}
	}
	// xdid -k [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('k')] ) {

		m_pWebBrowser2->GoBack( );
	}
	// xdid -m [NAME] [ID] [SWITCH] [+FLAGS] [+MASK] (URL)
	// [NAME] [ID] -m [+FLAGS] [+MASK] (URL)
	else if ( flags[TEXT('m')] && numtok > 4 ) {

		const XSwitchFlags xflags(input.getnexttok());		// tok 4 flags to change
		const XSwitchFlags xmask(input.getnexttok());		// tok 5 state mask, flags here are enabled, otherwise they are disabled.
		const auto URL(input.getlasttoks().trim());		// tok 6, -1 optional

		VARIANT vEmpty;
		VARIANT vFlags;
		VARIANT_BOOL bEnabled = VARIANT_FALSE;
		VariantInit( &vEmpty );
		Auto(VariantClear(&vEmpty));

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
			m_pWebBrowser2->put_AddressBar(bEnabled);
		}
		if (xflags['f']) { // fullscreen on/off
			if (xmask['f'])
				bEnabled = VARIANT_TRUE;
			else
				bEnabled = VARIANT_FALSE;
			m_pWebBrowser2->put_FullScreen(bEnabled);
		}
		if (xflags['s']) { // statusbar on/off
			if (xmask['s'])
				bEnabled = VARIANT_TRUE;
			else
				bEnabled = VARIANT_FALSE;

			m_pWebBrowser2->put_StatusBar(bEnabled);
		}
		// only open url if one supplied.
		if (!URL.empty()) {
#if DCX_USE_WRAPPERS
			Dcx::dcxBSTRResource bstrUrl(URL.to_chr());

			m_pWebBrowser2->Navigate(bstrUrl, &vFlags, &vEmpty, &vEmpty, &vEmpty);
#else
			auto bstrUrl = SysAllocString(URL.to_chr());
			if (bstrUrl == nullptr)
				throw Dcx::dcxException("Unable to Allocate Memory");
			Auto(SysFreeString(bstrUrl));

			m_pWebBrowser2->Navigate(bstrUrl, &vFlags, &vEmpty, &vEmpty, &vEmpty);
#endif
		}
	}
	// xdid -n [NAME] [ID] [SWITCH] [URL]
	// [NAME] [ID] -n [URL]
	else if ( flags[TEXT('n')] && numtok > 3 ) {

		const auto URL(input.getlasttoks().trim());	// tok 4, -1

#if DCX_USE_WRAPPERS
		Dcx::dcxVariant v;
		Dcx::dcxBSTRResource bstrUrl(URL.to_chr());

		m_pWebBrowser2->Navigate(bstrUrl, &v, &v, &v, &v);
#else
		VARIANT v;
		VariantInit(&v);
		Auto(VariantClear(&v));

		auto bstrUrl = SysAllocString(URL.to_chr());
		if (bstrUrl == nullptr)
			throw Dcx::dcxException("Unable to Allocate Memory");
		Auto(SysFreeString(bstrUrl));

		m_pWebBrowser2->Navigate(bstrUrl, &v, &v, &v, &v);
#endif
	}
	// xdid -r [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('r')] ) {

		m_pWebBrowser2->Refresh( );
	}
	// xdid -t [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('t')] ) {

		m_pWebBrowser2->Stop( );
	}
	else
		parseGlobalCommandRequest( input, flags );
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

	UINT err = 0;
#if DCX_USE_WRAPPERS
	Dcx::dcxVariant arg1, arg2;
#else
	VARIANT arg1, arg2;
	VariantInit( &arg1 );
	Auto(VariantClear(&arg1));

	VariantInit( &arg2 );
	Auto(VariantClear(&arg2));
#endif

	//if (SUCCEEDED(DispGetParam(pDispParams, 0, VT_BSTR, &arg1, &err)) && SUCCEEDED(DispGetParam(pDispParams, 1, VT_BSTR, &arg2, &err)))
	//{
	//	switch (dispIdMember) {
	//	case DISPID_NAVIGATECOMPLETE2:
	//	{
	//									 if (!this->m_bHideEvents)
	//											 this->execAliasEx(TEXT("%s,%d,%ws"), TEXT("nav_complete"), this->getUserID(), arg2.bstrVal);
	//	}
	//		break;
	//
	//	case DISPID_BEFORENAVIGATE2:
	//	{
	//								   if (!this->m_bHideEvents)
	//								   {
	//										   TCHAR ret[256];
	//										   this->evalAliasEx(ret, 255, TEXT("%s,%d,%ws"), TEXT("nav_begin"), this->getUserID(), arg2.bstrVal);
	//
	//										   if (lstrcmpi(ret, TEXT("cancel")) == 0)
	//											   *pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
	//										   else
	//											   *pDispParams->rgvarg->pboolVal = VARIANT_FALSE;
	//								   }
	//	}
	//		break;
	//
	//	case DISPID_DOCUMENTCOMPLETE:
	//	{
	//									if (!this->m_bHideEvents)
	//									{
	//											this->execAliasEx(TEXT("%s,%d,%ws"), TEXT("doc_complete"), this->getUserID(), arg2.bstrVal);
	//									}
	//									else
	//										this->m_bHideEvents = false; // allow events to be seen after first doc loads `about:blank`
	//	}
	//		break;
	//
	//	case DISPID_DOWNLOADBEGIN:
	//	{
	//								 if (!this->m_bHideEvents)
	//									 this->execAliasEx(TEXT("%s,%d"), TEXT("dl_begin"), this->getUserID());
	//	}
	//		break;
	//
	//	case DISPID_DOWNLOADCOMPLETE:
	//	{
	//									if (!this->m_bHideEvents)
	//										this->execAliasEx(TEXT("%s,%d"), TEXT("dl_complete"), this->getUserID());
	//	}
	//		break;
	//
	//	case DISPID_NEWWINDOW2:
	//	{
	//							  if (!this->m_bHideEvents)
	//							  {
	//								  TCHAR ret[256];
	//								  this->evalAliasEx(ret, 255, TEXT("%s,%d"), TEXT("win_open"), this->getUserID());
	//
	//								  if (lstrcmpi(ret, TEXT("cancel")) == 0)
	//									  *pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
	//								  else
	//									  *pDispParams->rgvarg->pboolVal = VARIANT_FALSE;
	//							  }
	//	}
	//		break;
	//
	//	case DISPID_STATUSTEXTCHANGE:
	//	{
	//									if (!this->m_bHideEvents)
	//										this->execAliasEx(TEXT("%s,%d,%ws"), TEXT("status"), this->getUserID(), arg1.bstrVal);
	//	}
	//		break;
	//
	//	case DISPID_TITLECHANGE:
	//	{
	//							   if (!this->m_bHideEvents)
	//								   this->execAliasEx(TEXT("%s,%d,%ws"), TEXT("title"), this->getUserID(), arg1.bstrVal);
	//	}
	//		break;
	//
	//	case DISPID_PROGRESSCHANGE:
	//	{
	//								  if (!this->m_bHideEvents)
	//									  this->execAliasEx(TEXT("%s,%d,%ws,%ws"), TEXT("dl_progress"), this->getUserID(), arg1.bstrVal, arg2.bstrVal);
	//	}
	//		break;
	//
	//	case DISPID_COMMANDSTATECHANGE:
	//	{
	//									  if (!this->m_bHideEvents)
	//									  {
	//										  switch (arg1.bstrVal[0]) {
	//										  case L'1':
	//											  this->execAliasEx(TEXT("%s,%d,%s"), TEXT("forward"), this->getUserID(), arg2.boolVal ? TEXT("$true") : TEXT("$false"));
	//											  break;
	//
	//										  case L'2':
	//											  this->execAliasEx(TEXT("%s,%d,%s"), TEXT("back"), this->getUserID(), arg2.boolVal ? TEXT("$true") : TEXT("$false"));
	//											  break;
	//										  }
	//									  }
	//	}
	//		break;
	//	}
	//	VariantClear(&arg1);
	//	VariantClear(&arg2);
	//}
	//else
	//	showErrorEx(nullptr, TEXT("DcxWebControl::Invoke()"), TEXT("Unable to get object state: %ld"), err);
//
	//return S_OK;

	HRESULT hRes = S_OK;

	try {
		if (!m_bHideEvents)
		{
			switch (dispIdMember) {
			case DISPID_NAVIGATECOMPLETE2:
			{
				hRes = DispGetParam(pDispParams, 1, VT_BSTR, &arg2, &err);
				if (FAILED(hRes))
					throw Dcx::dcxException(TEXT("DcxWebControl::Invoke(DISPID_NAVIGATECOMPLETE2) -> Unable to get Params: %"), err);

				execAliasEx(TEXT("nav_complete,%u,%ws"), getUserID(), arg2.bstrVal);
			}
			break;

			case DISPID_BEFORENAVIGATE2:
			{
				hRes = DispGetParam(pDispParams, 1, VT_BSTR, &arg2, &err);
				if (FAILED(hRes))
					throw Dcx::dcxException(TEXT("DcxWebControl::Invoke(DISPID_BEFORENAVIGATE2) -> Unable to get Params: %"), err);

				//TCHAR ret[256];
				//evalAliasEx(ret, Dcx::countof(ret), TEXT("nav_begin,%u,%ws"), getUserID(), arg2.bstrVal);
				//
				//if (lstrcmpi(ret, TEXT("cancel")) == 0)
				//	*pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
				//else
				//	*pDispParams->rgvarg->pboolVal = VARIANT_FALSE;

				stString<256> sRet;
				evalAliasEx(sRet, sRet.size(), TEXT("nav_begin,%u,%ws"), getUserID(), arg2.bstrVal);

				if (sRet == TEXT("cancel"))
					*pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
				else
					*pDispParams->rgvarg->pboolVal = VARIANT_FALSE;
			}
			break;

			case DISPID_DOCUMENTCOMPLETE:
			{
				hRes = DispGetParam(pDispParams, 1, VT_BSTR, &arg2, &err);
				if (FAILED(hRes))
					throw Dcx::dcxException(TEXT("DcxWebControl::Invoke(DISPID_DOCUMENTCOMPLETE) -> Unable to get Params: %"), err);

				execAliasEx(TEXT("doc_complete,%u,%ws"), getUserID(), arg2.bstrVal);
			}
			break;

			case DISPID_DOWNLOADBEGIN:
				execAliasEx(TEXT("dl_begin,%u"), getUserID());
				break;

			case DISPID_DOWNLOADCOMPLETE:
				execAliasEx(TEXT("dl_complete,%u"), getUserID());
				break;

			case DISPID_NEWWINDOW2:
			{
				//TCHAR ret[256];
				//evalAliasEx(ret, Dcx::countof(ret), TEXT("win_open,%u"), getUserID());
				//
				//if (lstrcmpi(ret, TEXT("cancel")) == 0)
				//	*pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
				//else
				//	*pDispParams->rgvarg->pboolVal = VARIANT_FALSE;

				stString<256> sRet;
				evalAliasEx(sRet, sRet.size(), TEXT("win_open,%u"), getUserID());

				if (sRet == TEXT("cancel"))
					*pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
				else
					*pDispParams->rgvarg->pboolVal = VARIANT_FALSE;
			}
			break;

			case DISPID_STATUSTEXTCHANGE:
			{
				hRes = DispGetParam(pDispParams, 0, VT_BSTR, &arg1, &err);
				if (FAILED(hRes))
					throw Dcx::dcxException(TEXT("DcxWebControl::Invoke(DISPID_STATUSTEXTCHANGE) -> Unable to get Params: %"), err);

				execAliasEx(TEXT("status,%u,%ws"), getUserID(), arg1.bstrVal);
			}
			break;

			case DISPID_TITLECHANGE:
			{
				hRes = DispGetParam(pDispParams, 0, VT_BSTR, &arg1, &err);
				if (FAILED(hRes))
					throw Dcx::dcxException(TEXT("DcxWebControl::Invoke(DISPID_TITLECHANGE) -> Unable to get Params: %"), err);

				execAliasEx(TEXT("title,%u,%ws"), getUserID(), arg1.bstrVal);
			}
			break;

			case DISPID_PROGRESSCHANGE:
			{
				hRes = DispGetParam(pDispParams, 0, VT_BSTR, &arg1, &err);
				if (FAILED(hRes))
					throw Dcx::dcxException(TEXT("DcxWebControl::Invoke(DISPID_PROGRESSCHANGE) -> Unable to get arg1: %"), err);

				hRes = DispGetParam(pDispParams, 1, VT_BSTR, &arg2, &err);
				if (FAILED(hRes))
					throw Dcx::dcxException(TEXT("DcxWebControl::Invoke(DISPID_PROGRESSCHANGE) -> Unable to get arg2: %"), err);

				execAliasEx(TEXT("dl_progress,%u,%ws,%ws"), getUserID(), arg1.bstrVal, arg2.bstrVal);
			}
			break;

			case DISPID_COMMANDSTATECHANGE:
			{
				hRes = DispGetParam(pDispParams, 0, VT_BSTR, &arg1, &err);
				if (FAILED(hRes))
					throw Dcx::dcxException(TEXT("DcxWebControl::Invoke(DISPID_COMMANDSTATECHANGE) -> Unable to get Params: %"), err);

				hRes = DispGetParam(pDispParams, 1, VT_BSTR, &arg2, &err);
				if (FAILED(hRes))
					throw Dcx::dcxException(TEXT("DcxWebControl::Invoke(DISPID_COMMANDSTATECHANGE) -> Unable to get Params: %"), err);

				switch (arg1.bstrVal[0]) {
				case L'1':
					execAliasEx(TEXT("forward,%u,%s"), getUserID(), arg2.boolVal ? TEXT("$true") : TEXT("$false"));
					break;

				case L'2':
					execAliasEx(TEXT("back,%u,%s"), getUserID(), arg2.boolVal ? TEXT("$true") : TEXT("$false"));
					break;
				}
			}
			break;
			}
		}
		else if (dispIdMember == DISPID_DOCUMENTCOMPLETE)
			this->m_bHideEvents = false; // allow events to be seen after first doc loads `about:blank`
	}
	catch (std::exception &e)
	{
		//showErrorEx(nullptr, TEXT("webctrl"), TEXT("error: %S"), e.what());
		showError(nullptr, TEXT("webctrl"), TEXT("error: %"), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		showError(nullptr, TEXT("webctrl"), TEXT("error: Unknown Exception"));
	}
	return hRes;
}

/*!
 * \brief blah
 *
 * blah
 */

HRESULT STDMETHODCALLTYPE DcxWebControl::QueryInterface( REFIID riid, void __RPC_FAR *__RPC_FAR * ppvObject )
{
	*ppvObject = nullptr;

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
	*ppFrame = nullptr;
	*ppDoc = nullptr;

	GetClientRect( m_Hwnd, pPR );
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
	mIRCLinker::signalex(true,TEXT("debug %lu"), uMsg);
#endif
	switch( uMsg ) {

	case WM_CHILDACTIVATE:
	case WM_SIZE:
		{
			if (m_pOleInPlaceObject == nullptr)
				break;

			RECT rc; 
			if (GetClientRect( m_Hwnd, &rc ))
				m_pOleInPlaceObject->SetObjectRects( &rc, &rc );
		}
		break;

	case WM_MOUSEACTIVATE:
		{
			bParsed = TRUE;
			//HWND web = nullptr;
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
		//
		//case WM_NCHITTEST:
		//	{
		//		bParsed = TRUE;
		//		return HTCLIENT;
		//		//return DefWindowProc( m_Hwnd, uMsg, wParam, lParam);
		//	}
		//	break;

		// original version
	//case WM_DESTROY:
	//{
	//	delete this;
	//	bParsed = TRUE;
	//}
	//break;

	//case WM_DESTROY:
	//	{
	//		SafeRelease();
	//		//delete this;
	//		//bParsed = TRUE;
	//	}
	//	break;

	case WM_NCDESTROY:
	{
		LRESULT lRes = 0L;
		if (m_DefaultWindowProc != nullptr)
			lRes = CallWindowProc(m_DefaultWindowProc, m_Hwnd, uMsg, wParam, lParam);
		else
			lRes = DefWindowProc(m_Hwnd, uMsg, wParam, lParam);
		delete this;
		bParsed = TRUE;
		return lRes;
	}
	break;
	default:
		return CommonMessage( uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}

TString DcxWebControl::CallScript(const TString & tsCmd) const
{
	TString tsRes;
	READYSTATE ready_state;

	if (FAILED(m_pWebBrowser2->get_ReadyState(&ready_state)) || ready_state != READYSTATE_COMPLETE)
		throw Dcx::dcxException("Browser NOT in Ready State");

	IDispatch  * htmlDisp = nullptr;

	if (SUCCEEDED(m_pWebBrowser2->get_Document(&htmlDisp)))
	{
		Auto(htmlDisp->Release());

		IHTMLDocument2 * doc = nullptr;
		if (SUCCEEDED(htmlDisp->QueryInterface(IID_IHTMLDocument2, (void**)&doc)))
		{
			Auto(doc->Release());

//			IHTMLWindow2 * window = nullptr;
//
//			if (SUCCEEDED(doc->get_parentWindow(&window)))
//			{
//				Auto(window->Release());
//
//				//VARIANT v;
//				//VariantInit(&v);
//				//Auto(VariantClear(&v));
//
//				Dcx::dcxVariant v;
//
//#if DCX_USE_WRAPPERS
//				const Dcx::dcxBSTRResource strCMD(tsCmd.to_wchr());
//
//				window->execScript(strCMD, nullptr, &v);	// this works well, but is deprecated in latest IE's?
//#else
//				auto strCMD = SysAllocString(CMD.to_chr());
//				if (strCMD != nullptr)
//				{
//					Auto(SysFreeString(strCMD));
//
//					window->execScript(strCMD, nullptr, &v);
//				}
//#endif
//			}


#if DCX_USE_WRAPPERS
			IDispatch * idisp = nullptr;

			if (SUCCEEDED(doc->get_Script(&idisp)))
			{
				Auto(idisp->Release());

				DISPID dispid = -1;
				DISPPARAMS params{};

				Dcx::dcxVariant vArg(tsCmd.to_wchr());

				params.cArgs = 1;
				params.rgvarg = &vArg;

				const Dcx::dcxBSTRResource strEval(L"eval");

				BSTR hm = strEval.get();
				if (FAILED(idisp->GetIDsOfNames(IID_NULL, &hm, 1, LOCALE_SYSTEM_DEFAULT, &dispid)))
					throw Dcx::dcxException("GetIDsOfNames: failed.");

				if (dispid == DISPID_UNKNOWN)
					throw Dcx::dcxException("GetIDsOfNames: Unable to find eval()");

				Dcx::dcxVariant v;

				if (FAILED(idisp->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD, &params, &v, NULL, NULL)))
					throw Dcx::dcxException("Invoke: failed.");

				switch (v.vt)
				{
				case VT_BSTR:
					tsRes.tsprintf(TEXT("%ws"), v.bstrVal);
					break;
					// TODO: handle other return types...
				default:
					break;
				}
			}
#else
			IDispatch * idisp = nullptr;

			if (SUCCEEDED(doc->get_Script(&idisp)))
			{
				Auto(idisp->Release());

				DISPID dispid = -1;
				DISPPARAMS params{};

				VARIANT vArg;
				VariantInit(&vArg);
				Auto(VariantClear(&vArg));

				vArg.vt = VT_BSTR;
				vArg.bstrVal = SysAllocString(tsCmd.to_wchr());

				if (vArg.bstrVal == nullptr)
					throw Dcx::dcxException("Unable to allocate BSTR");

				params.cArgs = 1;
				params.rgvarg = &vArg;

	
				BSTR hm = SysAllocString(L"eval");
				if (hm == nullptr)
					throw Dcx::dcxException("Unable to allocate BSTR");
				Auto(SysFreeString(hm));

				if (FAILED(idisp->GetIDsOfNames(IID_NULL, &hm, 1, LOCALE_SYSTEM_DEFAULT, &dispid)))
					throw Dcx::dcxException("GetIDsOfNames: failed.");

				if (dispid == DISPID_UNKNOWN)
					throw Dcx::dcxException("GetIDsOfNames: Unable to find eval()");

				VARIANT v;
				VariantInit(&v);
				Auto(VariantClear(&v));

				if (FAILED(idisp->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD, &params, &v, NULL, NULL)))
					throw Dcx::dcxException("Invoke: failed.");

				switch (v.vt)
				{
				case VT_BSTR:
					tsRes.tsprintf(TEXT("%ws"), v.bstrVal);
					break;
				default:
					break;
				}
			}
#endif
		}
	}
	return tsRes;
}
