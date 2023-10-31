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

 /*!
  * \brief Constructor
  *
  * \param ID Control ID
  * \param p_Dialog Parent DcxDialog Object
  * \param mParentHwnd Parent Window Handle
  * \param rc Window Rectangle
  * \param styles Window Style Tokenized List
  */
DcxWebControl::DcxWebControl(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_WEBCLASS,
		ws.m_Styles | WS_CHILD | WS_CLIPSIBLINGS,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	//	/* Web Control Stuff */
	//
	//	if (const auto cFact = Dcx::getClassFactory(); cFact &&
	//		SUCCEEDED(cFact->CreateInstance(0, IID_IWebBrowser2, (void**)&m_pWebBrowser2)) &&
	//		SUCCEEDED(m_pWebBrowser2->QueryInterface(IID_IOleObject, (LPVOID*)&m_pOleObject)) &&
	//		SUCCEEDED(m_pWebBrowser2->QueryInterface(IID_IOleInPlaceObject, (LPVOID*)&m_pOleInPlaceObject)) &&
	//		SUCCEEDED(m_pWebBrowser2->QueryInterface(IID_IConnectionPointContainer, (LPVOID*)&m_pCPC)) &&
	//		SUCCEEDED(m_pOleObject->SetClientSite((IOleClientSite*)this)) &&
	//		SUCCEEDED(m_pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_pCP)) &&
	//		SUCCEEDED(m_pCP->Advise((IUnknown*)(IOleClientSite*)this, &m_dwCookie)) &&
	//		//SUCCEEDED( m_pOleObject->DoVerb( OLEIVERB_UIACTIVATE, 0, (IOleClientSite*) this, 0, m_Hwnd, rc ) )
	//		SUCCEEDED(m_pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, 0, (IOleClientSite*)this, 0, m_Hwnd, rc))
	//		)
	//	{
	//#if DCX_USE_WRAPPERS
	//		const Dcx::dcxBSTRResource url(TEXT("about:blank"));
	//
	//		Dcx::dcxVariant v;
	//
	//		m_pWebBrowser2->Navigate(url.get(), &v, &v, &v, &v);  // dont use L""
	//#else
	//		if (const auto url = SysAllocString(TEXT("about:blank")); url)
	//		{
	//			Auto(SysFreeString(url));
	//
	//			VARIANT v;
	//			VariantInit(&v);
	//			Auto(VariantClear(&v));
	//
	//			m_pWebBrowser2->Navigate(url, &v, &v, &v, &v);  // dont use L""
	//		}
	//#endif
	//	}
	//	else {
	//		//Release all Web Control pointers
	//		SafeRelease();
	//
	//		DestroyWindow(m_Hwnd);
	//		throw Dcx::dcxException("Unable To Create Browser Window");
	//	}
}

/*!
 * \brief blah
 *
 * blah
 */

DcxWebControl::~DcxWebControl() noexcept
{
	//Release all Web Control pointers
	//SafeRelease();	// causes an odd crash when IE11 is used...
}

void DcxWebControl::SafeRelease() noexcept
{
	////Release all Web Control pointers
	//if (m_pCP != nullptr)
	//{
	//	if (m_dwCookie != 0UL)
	//	{
	//		m_pCP->Unadvise(m_dwCookie);
	//
	//		m_dwCookie = 0UL;
	//	}
	//
	//	m_pCP->Release();
	//	m_pCP = nullptr;
	//}
	//if (m_pCPC != nullptr)
	//{
	//	m_pCPC->Release();
	//	m_pCPC = nullptr;
	//}
	//if (m_pOleInPlaceObject != nullptr)
	//{
	//	m_pOleInPlaceObject->Release();
	//	m_pOleInPlaceObject = nullptr;
	//}
	//if (m_pOleObject != nullptr) {
	//	m_pOleObject->Close(OLECLOSE_NOSAVE);
	//	m_pOleObject->Release();
	//	m_pOleObject = nullptr;
	//}
	//
	//if (m_pWebBrowser2 != nullptr) {
	//	m_pWebBrowser2->Quit();
	//	m_pWebBrowser2->Release();
	//	m_pWebBrowser2 = nullptr;
	//}

	try {
		//Release all Web Control pointers
		if (m_pCP)
		{
			if (m_dwCookie != 0UL)
			{
				m_pCP->Unadvise(m_dwCookie);

				m_dwCookie = 0UL;
			}

			SafeReleaseCom(&m_pCP);
		}
		if (m_pCPC)
		{
			SafeReleaseCom(&m_pCPC);
		}
		if (m_pOleInPlaceObject)
		{
			SafeReleaseCom(&m_pOleInPlaceObject);
		}
		if (m_pOleObject)
		{
			m_pOleObject->Close(OLECLOSE_NOSAVE);
			SafeReleaseCom(&m_pOleObject);
		}

		if (m_pWebBrowser2)
		{
			m_pWebBrowser2->Quit();
			SafeReleaseCom(&m_pWebBrowser2);
		}
	}
	catch (...)
	{
	}
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxWebControl::parseControlStyles(const TString& tsStyles)
{
	return parseGeneralControlStyles(tsStyles);
}

bool DcxWebControl::InitializeInterface() noexcept
{
	try {
		/* Web Control Stuff */
		const RECT rc = this->getWindowPosition();

		if (const auto cFact = Dcx::getClassFactory(); cFact &&
			SUCCEEDED(cFact->CreateInstance(nullptr, IID_IWebBrowser2, (void**)&m_pWebBrowser2)) &&
			SUCCEEDED(m_pWebBrowser2->QueryInterface(IID_IOleObject, (LPVOID*)&m_pOleObject)) &&
			SUCCEEDED(m_pWebBrowser2->QueryInterface(IID_IOleInPlaceObject, (LPVOID*)&m_pOleInPlaceObject)) &&
			SUCCEEDED(m_pWebBrowser2->QueryInterface(IID_IConnectionPointContainer, (LPVOID*)&m_pCPC)) &&
			SUCCEEDED(m_pOleObject->SetClientSite((IOleClientSite*)this)) &&
			SUCCEEDED(m_pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_pCP)) &&
			SUCCEEDED(m_pCP->Advise((IUnknown*)(IOleClientSite*)this, &m_dwCookie)) &&
			//SUCCEEDED( m_pOleObject->DoVerb( OLEIVERB_UIACTIVATE, nullptr, (IOleClientSite*) this, 0, m_Hwnd, &rc ) )
			SUCCEEDED(m_pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, nullptr, (IOleClientSite*)this, 0, m_Hwnd, &rc))
			)
			//const auto cFact = Dcx::getClassFactory();
			//if (!cFact)
			//	throw "fail: ClassFactory";

			//if (!SUCCEEDED(cFact->CreateInstance(nullptr, IID_IWebBrowser2, (void**)&m_pWebBrowser2)) || !m_pWebBrowser2)
			//	throw "fail: WebBrowser2";

			//if (!SUCCEEDED(m_pWebBrowser2->QueryInterface(IID_IOleObject, (LPVOID*)&m_pOleObject)) || !m_pOleObject)
			//	throw "fail: OleObject";

			//if (!SUCCEEDED(m_pWebBrowser2->QueryInterface(IID_IOleInPlaceObject, (LPVOID*)&m_pOleInPlaceObject)) || !m_pOleInPlaceObject)
			//	throw "fail: OleObject";

			//if (!SUCCEEDED(m_pWebBrowser2->QueryInterface(IID_IConnectionPointContainer, (LPVOID*)&m_pCPC)) || !m_pCPC)
			//	throw "fail: OleObject";

			//if (!SUCCEEDED(m_pOleObject->SetClientSite((IOleClientSite*)this)))
			//	throw "fail: OleObject";

			//if (!SUCCEEDED(m_pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_pCP)) || !m_pCP)
			//	throw "fail: OleObject";

			//if (!SUCCEEDED(m_pCP->Advise((IUnknown*)(IOleClientSite*)this, &m_dwCookie)))
			//	throw "fail: OleObject";

			//if (!SUCCEEDED(m_pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, nullptr, (IOleClientSite*)this, 0, m_Hwnd, &rc)))
			//	throw "fail: OleObject";

			//if (m_pWebBrowser2 && m_pOleObject && m_pOleInPlaceObject && m_pCPC && m_pCP)
		{
#if DCX_USE_WRAPPERS
			const Dcx::dcxBSTRResource url(TEXT("about:blank"));

			Dcx::dcxVariant v;

			m_pWebBrowser2->Navigate(url.get(), &v, &v, &v, &v);  // dont use L""
#else
			if (const auto url = SysAllocString(TEXT("about:blank")); url)
			{
				Auto(SysFreeString(url));

				VARIANT v;
				VariantInit(&v);
				Auto(VariantClear(&v));

				m_pWebBrowser2->Navigate(url, &v, &v, &v, &v);  // dont use L""
			}
#endif
		}
		else {
			//Release all Web Control pointers
			SafeRelease();

			return false;
			//DestroyWindow(m_Hwnd);
			//throw Dcx::dcxException("Unable To Create Browser Window");
		}
	}
	catch (...)
	{
		SafeRelease();

		return false;
	}
	return true;
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */
void DcxWebControl::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP]
	case L"url"_hash:
	{
		szReturnValue = this->getURL().to_chr();
	}
	break;
	// [NAME] [ID] [PROP]
	case L"ready"_hash:
	{
		if (READYSTATE ready_state{}; SUCCEEDED(this->m_pWebBrowser2->get_ReadyState(&ready_state)))
		{
			if (ready_state == READYSTATE_COMPLETE)
			{
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
		if (this->IsStatusbarEnabled())
			szReturnValue = TEXT("$true");
		else
			szReturnValue = TEXT("$false");
	}
	break;
	case L"statustext"_hash:
	{
		szReturnValue = this->getStatusText().to_chr();
	}
	break;
	case L"script"_hash:
	{
		if (const auto tsRes(CallScript(input.getlasttoks())); !tsRes.empty())
			szReturnValue = tsRes.to_chr();
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxWebControl::parseCommandRequest(const TString& input)
{
	if (!m_pWebBrowser2)
		return;

	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -g [NAME] [ID] [SWITCH]
	if (flags[TEXT('g')])
		m_pWebBrowser2->GoHome();
	// xdid -i [NAME] [ID] [SWITCH]
	else if (flags[TEXT('i')])
		m_pWebBrowser2->GoForward();
	// xdid -j [NAME] [ID] [SWITCH] [JAVASCRIPT]
	else if (flags[TEXT('j')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto CMD(input.getlasttoks().trim());		// tok 4, -1

		const auto _d = CallScript(CMD);
	}
	// xdid -k [NAME] [ID] [SWITCH]
	else if (flags[TEXT('k')])
		m_pWebBrowser2->GoBack();
	// xdid -m [NAME] [ID] [SWITCH] [+FLAGS] [+MASK] (URL)
	// [NAME] [ID] -m [+FLAGS] [+MASK] (URL)
	else if (flags[TEXT('m')])
	{
		//		if (numtok < 5)
		//			throw DcxExceptions::dcxInvalidArguments();
		//
		//		const XSwitchFlags xflags(input.getnexttok());		// tok 4 flags to change
		//		const XSwitchFlags xmask(input.getnexttok());		// tok 5 state mask, flags here are enabled, otherwise they are disabled.
		//		const auto URL(input.getlasttoks().trim());		// tok 6, -1 optional
		//
		//#if DCX_USE_WRAPPERS
		//		Dcx::dcxVariant vEmpty;
		//		Dcx::dcxVariant vFlags;
		//		VARIANT_BOOL bEnabled = VARIANT_FALSE;
		//#else
		//		VARIANT vEmpty;
		//		VARIANT vFlags;
		//		VARIANT_BOOL bEnabled = VARIANT_FALSE;
		//		VariantInit(&vEmpty);
		//		Auto(VariantClear(&vEmpty));
		//#endif
		//
		//		V_VT(&vFlags) = VT_I4;
		//		V_I4(&vFlags) = 0;
		//
		//		if (xflags['h'] && xmask['h']) // no history
		//			V_I4(&vFlags) |= navNoHistory;
		//		if (xflags['r'] && xmask['r']) // no read from cache
		//			V_I4(&vFlags) |= navNoReadFromCache;
		//		if (xflags['w'] && xmask['w']) // no write to cache
		//			V_I4(&vFlags) |= navNoWriteToCache;
		//		if (xflags['a'] && xmask['a']) // allow auto search
		//			V_I4(&vFlags) |= navAllowAutosearch;
		//		if (xflags['e'] && xmask['e']) // enforce restricted zone
		//			V_I4(&vFlags) |= navEnforceRestricted;
		//		if (xflags['m'] && xmask['m']) // Manage popup windows.
		//			V_I4(&vFlags) |= navNewWindowsManaged;
		//		if (xflags['u'] && xmask['u']) // dont allow downloads.
		//			V_I4(&vFlags) |= navUntrustedForDownload;
		//		if (xflags['x'] && xmask['x']) // allow active x install prompts (doesnt auto install, you still need to ok the prompt)
		//			V_I4(&vFlags) |= navTrustedForActiveX;
		//		if (xflags['b']) { // toggle address bar on/off
		//			if (xmask['b'])
		//				bEnabled = VARIANT_TRUE;
		//			else
		//				bEnabled = VARIANT_FALSE;
		//			m_pWebBrowser2->put_AddressBar(bEnabled);
		//		}
		//		if (xflags['f']) { // fullscreen on/off
		//			if (xmask['f'])
		//				bEnabled = VARIANT_TRUE;
		//			else
		//				bEnabled = VARIANT_FALSE;
		//			m_pWebBrowser2->put_FullScreen(bEnabled);
		//		}
		//		if (xflags['s']) // statusbar on/off
		//			this->setStatusbarState(xmask['s']);
		//
		//		// only open url if one supplied.
		//		if (!URL.empty())
		//		{
		//#if DCX_USE_WRAPPERS
		//			const Dcx::dcxBSTRResource bstrUrl(URL.to_wchr());
		//
		//			m_pWebBrowser2->Navigate(bstrUrl.get(), &vFlags, &vEmpty, &vEmpty, &vEmpty);
		//#else
		//			auto bstrUrl = SysAllocString(URL.to_chr());
		//			if (!bstrUrl)
		//				throw Dcx::dcxException("Unable to Allocate Memory");
		//			Auto(SysFreeString(bstrUrl));
		//
		//			m_pWebBrowser2->Navigate(bstrUrl, &vFlags, &vEmpty, &vEmpty, &vEmpty);
		//#endif
		//		}

		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const TString tsFlags(input.getnexttok());		// tok 4 flags to change
		const TString tsMask(input.getnexttok());		// tok 5 state mask, flags here are enabled, otherwise they are disabled.
		const TString tsURL(input.getlasttoks().trim());// tok 6, -1 optional

		this->setURL(tsURL, tsFlags, tsMask);
	}
	// xdid -n [NAME] [ID] [SWITCH] [URL]
	// [NAME] [ID] -n [URL]
	else if (flags[TEXT('n')])
	{
		//		if (numtok < 4)
		//			throw DcxExceptions::dcxInvalidArguments();
		//
		//		const auto URL(input.getlasttoks().trim());	// tok 4, -1
		//
		//#if DCX_USE_WRAPPERS
		//		Dcx::dcxVariant v;
		//		const Dcx::dcxBSTRResource bstrUrl(URL.to_wchr());
		//
		//		m_pWebBrowser2->Navigate(bstrUrl.get(), &v, &v, &v, &v);
		//#else
		//		VARIANT v;
		//		VariantInit(&v);
		//		Auto(VariantClear(&v));
		//
		//		auto bstrUrl = SysAllocString(URL.to_wchr());
		//		if (!bstrUrl)
		//			throw Dcx::dcxException("Unable to Allocate Memory");
		//		Auto(SysFreeString(bstrUrl));
		//
		//		m_pWebBrowser2->Navigate(bstrUrl, &v, &v, &v, &v);
		//#endif

		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsURL(input.getlasttoks().trim());	// tok 4, -1
		const TString tsBlank(L"+");

		this->setURL(tsURL, tsBlank, tsBlank);
	}
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
		m_pWebBrowser2->Refresh();
	// xdid -t [NAME] [ID] [SWITCH]
	else if (flags[TEXT('t')])
		m_pWebBrowser2->Stop();
	else
		parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

HRESULT DcxWebControl::Invoke(DISPID dispIdMember,
	REFIID riid,
	LCID lcid,
	WORD wFlags,
	DISPPARAMS __RPC_FAR* pDispParams,
	VARIANT __RPC_FAR* pVarResult,
	EXCEPINFO __RPC_FAR* pExcepInfo,
	UINT __RPC_FAR* puArgErr)
{
	//if (this->m_bHideEvents) return S_OK; // cant do this here :/

	UINT err = 0;
#if DCX_USE_WRAPPERS
	Dcx::dcxVariant arg1, arg2;
#else
	VARIANT arg1, arg2;
	VariantInit(&arg1);
	Auto(VariantClear(&arg1));

	VariantInit(&arg2);
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
			switch (dispIdMember)
			{
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

				const stString<256> sRet;
				evalAliasEx(sRet, gsl::narrow_cast<int>(sRet.size()), TEXT("nav_begin,%u,%ws"), getUserID(), arg2.bstrVal);

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

				const stString<256> sRet;
				evalAliasEx(sRet, gsl::narrow_cast<int>(sRet.size()), TEXT("win_open,%u"), getUserID());

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

				constexpr TCHAR szTrue[] = TEXT("$true");
				constexpr TCHAR szFalse[] = TEXT("$false");

				switch (arg1.bstrVal[0])
				{
				case L'1':
					execAliasEx(TEXT("forward,%u,%s"), getUserID(), arg2.boolVal ? &szTrue[0] : &szFalse[0]);
					break;

				case L'2':
					execAliasEx(TEXT("back,%u,%s"), getUserID(), arg2.boolVal ? &szTrue[0] : &szFalse[0]);
					break;
				default:
					break;
				}
			}
			break;
			default:
				break;
			}
		}
		else if (dispIdMember == DISPID_DOCUMENTCOMPLETE)
			this->m_bHideEvents = false; // allow events to be seen after first doc loads `about:blank`
	}
	catch (const std::exception& e)
	{
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

HRESULT STDMETHODCALLTYPE DcxWebControl::QueryInterface(REFIID riid, void __RPC_FAR* __RPC_FAR* ppvObject) noexcept
{
	*ppvObject = nullptr;

	if (IID_IUnknown == riid)
		*ppvObject = (IUnknown*)(IOleClientSite*)this;
	else if (IID_IOleInPlaceSite == riid)
		*ppvObject = (IOleInPlaceSite*)this;
	else if (DIID_DWebBrowserEvents2 == riid)
		*ppvObject = (DWebBrowserEvents2*)this;

	return *ppvObject ? S_OK : E_NOINTERFACE;
}

void DcxWebControl::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	// url statusbar statustext
	xml->SetAttribute("url", this->getURL().c_str());
	const TString tsFlags(L"+sbf");
	TString tsMask(L"+");

	if (this->IsStatusbarEnabled())
		tsMask += L's';
	if (this->IsAddressbarEnabled())
		tsMask += L'b';
	if (this->IsFullScreenEnabled())
		tsMask += L'f';

	// addressbar fullscreen
	xml->SetAttribute("flags", tsFlags.c_str());
	xml->SetAttribute("mask", tsMask.c_str());
	xml->SetAttribute("statustext", this->getStatusText().c_str());
}

TiXmlElement* DcxWebControl::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxWebControl::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	{
		const TString tsURL(queryAttribute(xThis, "url"));
		const TString tsFlags(queryAttribute(xThis, "flags"));
		const TString tsMask(queryAttribute(xThis, "mask"));
		this->setURL(tsURL, tsFlags, tsMask);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

HRESULT STDMETHODCALLTYPE DcxWebControl::GetWindowContext(IOleInPlaceFrame __RPC_FAR* __RPC_FAR* ppFrame, IOleInPlaceUIWindow __RPC_FAR* __RPC_FAR* ppDoc, LPRECT pPR, LPRECT pCR, LPOLEINPLACEFRAMEINFO pFI) noexcept
{
	*ppFrame = nullptr;
	*ppDoc = nullptr;

	if (GetClientRect(m_Hwnd, pPR))
	{
		CopyRect(pCR, pPR);
		return S_OK;
	}
	return E_FAIL;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxWebControl::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept
{
	return 0L;
}

LRESULT DcxWebControl::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
#if DCX_DEBUG_OUTPUT
	//mIRCLinker::signalex(true,TEXT("webctrl debug %lu"), uMsg);
	mIRCLinker::signal(TEXT("webctrl debug %"), uMsg);
#endif
	switch (uMsg)
	{
	case WM_CHILDACTIVATE:
	case WM_SIZE:
	{
		if (!m_pOleInPlaceObject)
			break;

		if (RECT rc{}; GetClientRect(m_Hwnd, &rc))
			m_pOleInPlaceObject->SetObjectRects(&rc, &rc);
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
		const LRESULT lRes = CallDefaultClassProc(uMsg, wParam, lParam);

		delete this;
		bParsed = TRUE;
		return lRes;
	}
	break;
	default:
		return CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}

TString DcxWebControl::CallScript(const TString& tsCmd) const
{
	if (READYSTATE ready_state{ READYSTATE_UNINITIALIZED }; FAILED(m_pWebBrowser2->get_ReadyState(&ready_state)) || ready_state != READYSTATE_COMPLETE)
		throw Dcx::dcxException("Browser NOT in Ready State");

	TString tsRes;

	IDispatch* htmlDisp{};

	if (SUCCEEDED(m_pWebBrowser2->get_Document(&htmlDisp)))
	{
		Auto(SafeReleaseCom(&htmlDisp));

		IHTMLDocument2* doc{};
		if (SUCCEEDED(htmlDisp->QueryInterface(IID_IHTMLDocument2, (void**)&doc)))
		{
			Auto(SafeReleaseCom(&doc));

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
			IDispatch* idisp{};

			if (SUCCEEDED(doc->get_Script(&idisp)))
			{
				Auto(SafeReleaseCom(&idisp));

				DISPID dispid = DISPID_UNKNOWN;
				DISPPARAMS params{};

				Dcx::dcxVariant vArg(tsCmd.to_wchr());

				params.cArgs = 1;
				params.rgvarg = &vArg;

				{
					const Dcx::dcxBSTRResource strEval(L"eval");

					if (BSTR hm = strEval.get(); FAILED(idisp->GetIDsOfNames(IID_NULL, &hm, 1, LOCALE_SYSTEM_DEFAULT, &dispid)))
						throw Dcx::dcxException("GetIDsOfNames: failed.");
				}

				if (dispid == DISPID_UNKNOWN)
					throw Dcx::dcxException("GetIDsOfNames: Unable to find eval()");

				Dcx::dcxVariant v;

				if (FAILED(idisp->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD, &params, &v, nullptr, nullptr)))
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
			IDispatch* idisp = nullptr;

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

				if (!vArg.bstrVal)
					throw Dcx::dcxException("Unable to allocate BSTR");

				params.cArgs = 1;
				params.rgvarg = &vArg;


				BSTR hm = SysAllocString(L"eval");
				if (!hm)
					throw Dcx::dcxException("Unable to allocate BSTR");
				Auto(SysFreeString(hm));

				if (FAILED(idisp->GetIDsOfNames(IID_NULL, &hm, 1, LOCALE_SYSTEM_DEFAULT, &dispid)))
					throw Dcx::dcxException("GetIDsOfNames: failed.");

				if (dispid == DISPID_UNKNOWN)
					throw Dcx::dcxException("GetIDsOfNames: Unable to find eval()");

				VARIANT v;
				VariantInit(&v);
				Auto(VariantClear(&v));

				if (FAILED(idisp->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD, &params, &v, nullptr, nullptr)))
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

TString DcxWebControl::getURL() const
{
	TString tsURL;
	if (this->m_pWebBrowser2)
	{
		if (BSTR str = nullptr; SUCCEEDED(this->m_pWebBrowser2->get_LocationURL(&str)))
		{
			_ts_snprintf(tsURL, TEXT("%ws"), str); // possible overflow, needs fixing at some point.
			SysFreeString(str);
		}
	}
	return tsURL;
}

bool DcxWebControl::IsStatusbarEnabled() const
{
	if (this->m_pWebBrowser2)
	{
		if (VARIANT_BOOL bState = 0; SUCCEEDED(this->m_pWebBrowser2->get_StatusBar(&bState)))
		{
			if (bState == VARIANT_TRUE)
				return true;
		}
	}
	return false;
}

bool DcxWebControl::IsFullScreenEnabled() const
{
	if (this->m_pWebBrowser2)
	{
		if (VARIANT_BOOL bState = 0; SUCCEEDED(this->m_pWebBrowser2->get_FullScreen(&bState)))
		{
			if (bState == VARIANT_TRUE)
				return true;
		}
	}
	return false;
}

bool DcxWebControl::IsAddressbarEnabled() const
{
	if (this->m_pWebBrowser2)
	{
		if (VARIANT_BOOL bState = 0; SUCCEEDED(this->m_pWebBrowser2->get_AddressBar(&bState)))
		{
			if (bState == VARIANT_TRUE)
				return true;
		}
	}
	return false;
}

TString DcxWebControl::getStatusText() const
{
	TString tsText;
	if (this->m_pWebBrowser2)
	{
		if (BSTR str = nullptr; SUCCEEDED(this->m_pWebBrowser2->get_StatusText(&str)))
		{
			_ts_snprintf(tsText, TEXT("%ws"), str); // possible overflow, needs fixing at some point.
			SysFreeString(str);
		}
	}
	return tsText;
}

void DcxWebControl::setFullScreenState(bool bEnable)
{
	if (!m_pWebBrowser2)
		return;

	VARIANT_BOOL bEnabled = VARIANT_FALSE;
	if (bEnable)
		bEnabled = VARIANT_TRUE;

	m_pWebBrowser2->put_FullScreen(bEnabled);
}

void DcxWebControl::setAddressbarState(bool bEnable)
{
	if (!m_pWebBrowser2)
		return;

	VARIANT_BOOL bEnabled = VARIANT_FALSE;
	if (bEnable)
		bEnabled = VARIANT_TRUE;

	m_pWebBrowser2->put_AddressBar(bEnabled);
}

void DcxWebControl::setStatusbarState(bool bEnable)
{
	if (!m_pWebBrowser2)
		return;

	VARIANT_BOOL bEnabled = VARIANT_FALSE;
	if (bEnable)
		bEnabled = VARIANT_TRUE;

	m_pWebBrowser2->put_StatusBar(bEnabled);
}

void DcxWebControl::setStatusText(const TString& tsText)
{
	if (!m_pWebBrowser2 || tsText.empty())
		return;

	Dcx::dcxBSTRResource bstr(tsText.to_chr());
	m_pWebBrowser2->put_StatusText(bstr);
}

void DcxWebControl::setURL(const TString& tsURL, const TString& tsFlags, const TString& tsMask)
{
	if (!m_pWebBrowser2)
		return;

	const XSwitchFlags xflags(tsFlags);			// flags to change
	const XSwitchFlags xmask(tsMask);			// state mask, flags here are enabled, otherwise they are disabled.

#if DCX_USE_WRAPPERS
	Dcx::dcxVariant vEmpty;
	Dcx::dcxVariant vFlags;
	VARIANT_BOOL bEnabled = VARIANT_FALSE;
#else
	VARIANT vEmpty;
	VARIANT vFlags;
	VARIANT_BOOL bEnabled = VARIANT_FALSE;
	VariantInit(&vEmpty);
	Auto(VariantClear(&vEmpty));
#endif

	V_VT(&vFlags) = VT_I4;
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
	if (xflags['s']) // statusbar on/off
		this->setStatusbarState(xmask['s']);

	// only open url if one supplied.
	if (!tsURL.empty())
	{
#if DCX_USE_WRAPPERS
		const Dcx::dcxBSTRResource bstrUrl(tsURL.to_wchr());

		m_pWebBrowser2->Navigate(bstrUrl.get(), &vFlags, &vEmpty, &vEmpty, &vEmpty);
#else
		auto bstrUrl = SysAllocString(URL.to_chr());
		if (!bstrUrl)
			throw Dcx::dcxException("Unable to Allocate Memory");
		Auto(SysFreeString(bstrUrl));

		m_pWebBrowser2->Navigate(bstrUrl, &vFlags, &vEmpty, &vEmpty, &vEmpty);
#endif
	}
}

LRESULT DcxWebControl::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
