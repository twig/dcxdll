#include "defines.h"
#include "DcxWebControl2.h"

DcxWebControl2::DcxWebControl2(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_WEB2CLASS,
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
}

DcxWebControl2::~DcxWebControl2() noexcept
{
}

LRESULT DcxWebControl2::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
#if DCX_DEBUG_OUTPUT
	//mIRCLinker::signal(TEXT("web2ctrl debug %"), uMsg);
#endif
	switch (uMsg)
	{
	case WM_SIZE:
	{
		if (!m_webviewController)
			break;

		if (RECT rc{}; GetClientRect(m_Hwnd, &rc))
			m_webviewController->put_Bounds(rc);
	}
	break;

	// original version
//case WM_DESTROY:
//{
//	delete this;
//	bParsed = TRUE;
//}
//break;

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

LRESULT DcxWebControl2::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept
{
	return LRESULT();
}

void DcxWebControl2::parseInfoRequest(const TString& input, const refString<TCHAR, 4100>& szReturnValue) const
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
		szReturnValue = getReadyState().to_chr();
	}
	break;
	// [NAME] [ID] [PROP]
	case L"statusbar"_hash:
	{
		szReturnValue = dcx_truefalse(this->IsStatusbarEnabled());
	}
	break;
	// [NAME] [ID] [PROP]
	case L"fullscreen"_hash:
	{
		szReturnValue = dcx_truefalse(this->IsFullScreenEnabled());
	}
	break;
	// [NAME] [ID] [PROP]
	case L"scripts"_hash:
	{
		szReturnValue = dcx_truefalse(this->IsScriptingEnabled());
	}
	break;
	// [NAME] [ID] [PROP]
	case L"downloads"_hash:
	{
		szReturnValue = dcx_truefalse(this->IsDownloadingEnabled());
	}
	break;
	// [NAME] [ID] [PROP]
	case L"downloaddialog"_hash:
	{
		szReturnValue = dcx_truefalse(this->IsDownloadsDialogEnabled());
	}
	break;
	// [NAME] [ID] [PROP]
	case L"managed"_hash:
	{
		szReturnValue = dcx_truefalse(this->IsNewWindowsManaged());
	}
	break;
	// [NAME] [ID] [PROP]
	case L"statustext"_hash:
	{
		szReturnValue = this->getStatusText().to_chr();
	}
	break;
	// [NAME] [ID] [PROP]
	case L"mute"_hash:
	{
		szReturnValue = dcx_truefalse(this->IsAudioMuted());
	}
	break;
	// [NAME] [ID] [PROP]
	case L"version"_hash:
	{
		if (m_webviewEnvironment)
		{
			wil::unique_cotaskmem_string value;

			m_webviewEnvironment->get_BrowserVersionString(&value);

			szReturnValue = value.get();
		}
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
}

void DcxWebControl2::parseCommandRequest(const TString& input)
{
	if (!this->m_webview || !this->m_webviewController)
		return;

	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -g [NAME] [ID] [SWITCH] (URL)
	if (flags[TEXT('g')])
	{
		// WebView2 doesnt support a GoHome() function, so we have to make our own.

		const auto tsURL(input.getlasttoks().trim());	// tok 4, -1
		if (!tsURL.empty())
			m_tsHome = tsURL;

		if (m_tsHome.empty())
			m_tsHome = L"about:blank";

		const TString tsBlank(L"+");

		this->setURL(m_tsHome, tsBlank, tsBlank);
	}
	// xdid -i [NAME] [ID] [SWITCH]
	else if (flags[TEXT('i')])
		m_webview->GoForward();
	// xdid -j [NAME] [ID] [SWITCH] [JAVASCRIPT]
	else if (flags[TEXT('j')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto CMD(input.getlasttoks().trim());		// tok 4, -1

		CallScript(CMD);
	}
	// xdid -k [NAME] [ID] [SWITCH]
	else if (flags[TEXT('k')])
		m_webview->GoBack();
	// xdid -m [NAME] [ID] [SWITCH] [+FLAGS] [+MASK] (URL)
	// [NAME] [ID] -m [+FLAGS] [+MASK] (URL)
	else if (flags[TEXT('m')])
	{
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
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsURL(input.getlasttoks().trim());	// tok 4, -1
		const TString tsBlank(L"+");

		this->setURL(tsURL, tsBlank, tsBlank);
	}
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
	{
		m_webview->Reload();
	}
	// xdid -t [NAME] [ID] [SWITCH]
	else if (flags[TEXT('t')])
		m_webview->Stop();
	// xdid -c [NAME] [ID] [SWITCH] (type)
	else if (flags[TEXT('c')])
	{
		// clear cache
		switch (const TString tsType(input.getnexttok()); std::hash<TString>()(tsType))
		{
		case L"cookies"_hash:
			this->ClearCacheKind(COREWEBVIEW2_BROWSING_DATA_KINDS_COOKIES);
			break;
		case L"downloads"_hash:
			this->ClearCacheKind(COREWEBVIEW2_BROWSING_DATA_KINDS_DOWNLOAD_HISTORY);
			break;
		case L"browsing"_hash:
			this->ClearCacheKind(COREWEBVIEW2_BROWSING_DATA_KINDS_BROWSING_HISTORY);
			break;
		default:
			this->ClearCache();
			break;
		}
	}
	// xdid -C [NAME] [ID] [SWITCH] [+FLAGS] (FILENAME)
	else if (flags[TEXT('C')])
	{
		const XSwitchFlags xFlags(input.getnexttok());
		auto tsFilename(input.getlasttoks().trim());	// tok 4, -1

		if (!xFlags[TEXT('+')])
			throw DcxExceptions::dcxInvalidFlag();

		COREWEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT fmt = COREWEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT_PNG;
		if (xFlags[L'j'])
			fmt = COREWEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT_JPEG;

		if (tsFilename.empty())
		{
			WCHAR fileName[MAX_PATH] = L"WebView2_Screenshot.";
			if (xFlags[L'j'])
				_ts_strcat(&fileName[0], L"jpg");
			else
				_ts_strcat(&fileName[0], L"png");

			OPENFILENAME openFileName{};
			openFileName.lStructSize = sizeof(openFileName);
			openFileName.hwndOwner = nullptr;
			openFileName.hInstance = nullptr;
			openFileName.lpstrFile = &fileName[0];
			if (xFlags[L'j'])
				openFileName.lpstrFilter = L"Jpeg Files\0.jpeg;.jpg\0";
			else
				openFileName.lpstrFilter = L"PNG Files\0.png\0";
			openFileName.nMaxFile = std::size(fileName);
			openFileName.Flags = OFN_OVERWRITEPROMPT;

			if (GetSaveFileName(&openFileName))
				tsFilename = &fileName[0];
		}

		if (!tsFilename.empty())
		{
			wil::com_ptr<IStream> stream;
			if (SUCCEEDED(SHCreateStreamOnFileEx(tsFilename.to_wchr(), STGM_READWRITE | STGM_CREATE, FILE_ATTRIBUTE_NORMAL, TRUE, nullptr, &stream)))
			{
				m_webview->CapturePreview(fmt, stream.get(),
					Microsoft::WRL::Callback<ICoreWebView2CapturePreviewCompletedHandler>(
						[this](HRESULT error_code) noexcept -> HRESULT {
							//CHECK_FAILURE(error_code);

							MessageBox(m_Hwnd, L"Preview Captured", L"Preview Captured", MB_OK);
							return S_OK;
						})
					.Get());
			}
		}
	}
	else
		parseGlobalCommandRequest(input, flags);
}

dcxWindowStyles DcxWebControl2::parseControlStyles(const TString& tsStyles)
{
	return parseGeneralControlStyles(tsStyles);
}

void DcxWebControl2::toXml(TiXmlElement* const xml) const
{
	if (!xml)
		return;

	__super::toXml(xml);

	xml->SetAttribute("url", this->getURL().c_str());
	const TString tsFlags(L"+sSfuU");
	TString tsMask(L"+");

	if (this->IsStatusbarEnabled())
		tsMask += L's';
	if (this->IsScriptingEnabled())
		tsMask += L'S';
	if (this->IsFullScreenEnabled())
		tsMask += L'f';
	if (this->IsDownloadingEnabled())
		tsMask += L'u';
	if (this->IsDownloadsDialogEnabled())
		tsMask += L'U';
	if (this->IsAudioMuted())
		tsMask += L'A';

	xml->SetAttribute("flags", tsFlags.c_str());
	xml->SetAttribute("mask", tsMask.c_str());
}

TiXmlElement* DcxWebControl2::toXml() const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxWebControl2::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	__super::fromXml(xDcxml, xThis);

	{
		const TString tsURL(queryAttribute(xThis, "url"));
		const TString tsFlags(queryAttribute(xThis, "flags", "+"));
		const TString tsMask(queryAttribute(xThis, "mask", "+"));
		this->setURL(tsURL, tsFlags, tsMask);
	}
}

LRESULT DcxWebControl2::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}

// Run Download and Install in another thread so we don't block the UI thread
DWORD WINAPI DcxWebControl2::DownloadAndInstallWV2RT(_In_ LPVOID lpParameter) noexcept
{
	DcxWebControl2* p_this = static_cast<DcxWebControl2*>(lpParameter);
	if (!p_this)
		return 4;

	int returnCode = MessageBoxW(p_this->getHwnd(), L"Download WebView2 Package?", L"WebView2 Downloader", MB_YESNO);
	if (returnCode != IDYES)
	{
		p_this->InstallComplete(3);
		p_this->decRef();

		return returnCode;
	}
	returnCode = 2;

	// Use fwlink to download WebView2 Bootstrapper at runtime and invoke installation
	// Broken/Invalid Https Certificate will fail to download
	// Use of the download link below is governed by the below terms. You may acquire the link for your use at https://developer.microsoft.com/microsoft-edge/webview2/.
	// Microsoft owns all legal right, title, and interest in and to the
	// WebView2 Runtime Bootstrapper ("Software") and related documentation,
	// including any intellectual property in the Software. You must acquire all
	// code, including any code obtained from a Microsoft URL, under a separate
	// license directly from Microsoft, including a Microsoft download site
	// (e.g., https://developer.microsoft.com/microsoft-edge/webview2/).
	if (const HRESULT hr = URLDownloadToFile(nullptr, L"https://go.microsoft.com/fwlink/p/?LinkId=2124703", L".\\MicrosoftEdgeWebview2Setup.exe", 0, nullptr); hr == S_OK)
	{
		// Either Package the WebView2 Bootstrapper with your app or download it using fwlink
		// Then invoke install at Runtime.
		SHELLEXECUTEINFO shExInfo{};
		shExInfo.cbSize = sizeof(shExInfo);
		shExInfo.fMask = SEE_MASK_NOASYNC;
		shExInfo.hwnd = nullptr;
		shExInfo.lpVerb = L"runas";
		shExInfo.lpFile = L"MicrosoftEdgeWebview2Setup.exe";
		shExInfo.lpParameters = L" /silent /install";
		shExInfo.lpDirectory = nullptr;
		shExInfo.nShow = 0;
		shExInfo.hInstApp = nullptr;

		if (ShellExecuteEx(&shExInfo))
		{
			returnCode = 0; // Install successfull
		}
		else
		{
			returnCode = 1; // Install failed
		}
	}

	p_this->InstallComplete(returnCode);
	p_this->decRef();

	return returnCode;
}
void DcxWebControl2::InstallComplete(int return_code) noexcept
{
	if (return_code == 0)
	{
		try {
			this->InitializeInterface();
		}
		catch (...) {}
	}
	else if (return_code == 1)
	{
		MessageBox(m_Hwnd, L"WebView Runtime failed to Install", L"WebView Runtime Installation status", MB_OK);
	}
	else if (return_code == 2)
	{
		MessageBox(m_Hwnd, L"WebView Bootstrapper failed to download", L"WebView Bootstrapper Download status", MB_OK);
	}

	execAliasEx(L"install_complete,%u,%u", getUserID(), return_code);
}

bool DcxWebControl2::InitializeInterface()
{
	if (!Dcx::WebViewModule.isUseable())
	{
		// cant load WebView2Loader.dll
		MessageBox(m_Hwnd, L"Unable to load WebView2Loader.dll", L"Missing File!", MB_OK);
		return false;
	}
	wil::unique_cotaskmem_string version_info;

	if (const HRESULT hr = Dcx::WebViewModule.dcxGetAvailableCoreWebView2BrowserVersionString(nullptr, &version_info); (hr == S_OK) && (version_info != nullptr))
	{
#if DCX_DEBUG_OUTPUT
		mIRCLinker::signal(TEXT("web2ctrl webview2 installed: v%"), version_info.get());
#endif
		if (DcxWebControl2::m_webviewEnvironment == nullptr)
		{
			TString tsUserData("$mircdirWebView2Cache");
			mIRCLinker::eval(tsUserData, tsUserData);

			Dcx::WebViewModule.dcxCreateCoreWebView2EnvironmentWithOptions(nullptr, tsUserData.to_wchr(), nullptr, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this, &DcxWebControl2::OnCreateCoreWebView2EnvironmentCompleted).Get());
		}
		else
			OnCreateCoreWebView2EnvironmentCompleted(0, DcxWebControl2::m_webviewEnvironment.get());
	}
	else {
		// no webview2 installed!

#if DCX_DEBUG_OUTPUT
		mIRCLinker::signal(TEXT("web2ctrl no webview2 installed!"));
#endif

		this->incRef();
		CreateThread(nullptr, 0, DownloadAndInstallWV2RT, this, 0, nullptr);

		return false;
	}

	return true;
}

TString DcxWebControl2::getURL() const
{
	TString tsRes;
	if (!m_webview)
		return tsRes;

	wil::unique_cotaskmem_string uri;
	m_webview->get_Source(&uri);
	if (wcscmp(uri.get(), L"about:blank") == 0)
	{
		uri = wil::make_cotaskmem_string(L"");
	}
	tsRes = uri.get();

	return tsRes;
}

bool DcxWebControl2::IsStatusbarEnabled() const
{
	if (!m_settings)
		return false;

	BOOL bRes{};
	m_settings->get_IsStatusBarEnabled(&bRes);

	return bRes;
}

bool DcxWebControl2::IsFullScreenEnabled() const
{
	if (!m_webview)
		return false;

	BOOL bRes{};
	m_webview->get_ContainsFullScreenElement(&bRes);

	return bRes;
}

bool DcxWebControl2::IsScriptingEnabled() const
{
	if (!m_settings)
		return false;

	BOOL bRes{};
	m_settings->get_IsScriptEnabled(&bRes);

	return bRes;
}

bool DcxWebControl2::IsAudioMuted() const
{
	BOOL isMute{};
	if (auto webview2_8 = m_webview.try_query<ICoreWebView2_8>(); webview2_8)
		webview2_8->get_IsMuted(&isMute);

	return isMute;
}

bool DcxWebControl2::IsAudioPlaying() const
{
	BOOL isDocumentPlayingAudio{};
	if (auto webview2_8 = m_webview.try_query<ICoreWebView2_8>(); webview2_8)
		webview2_8->get_IsDocumentPlayingAudio(&isDocumentPlayingAudio);

	return isDocumentPlayingAudio;
}

const bool& DcxWebControl2::IsDownloadingEnabled() const noexcept
{
	return m_bAllowDownloads;
}

const bool& DcxWebControl2::IsNewWindowsManaged() const noexcept
{
	return m_bManageNewWindows;
}

const bool& DcxWebControl2::IsDownloadsDialogEnabled() const noexcept
{
	return m_bDownloadsDialogEnabled;
}

TString DcxWebControl2::getStatusText() const
{
	TString tsRes;
	if (!m_webview)
		return tsRes;

	if (auto wv = m_webview.try_query<ICoreWebView2_12>(); wv)
	{
		wil::unique_cotaskmem_string value;

		if (SUCCEEDED(wv->get_StatusBarText(&value)))
			tsRes = value.get();
	}
	return tsRes;
}

TString DcxWebControl2::getReadyState() const
{
	TString tsRes(L"$false");
	if (this->m_webview)
	{
		//if (READYSTATE ready_state{}; SUCCEEDED(this->m_pWebBrowser2->get_ReadyState(&ready_state)))
		//{
		//	if (ready_state == READYSTATE_COMPLETE)
		//	{
		//		szReturnValue = TEXT("$true");
		//		return;
		//	}
		//}
	}
	return tsRes;
}

void DcxWebControl2::setFullScreenState(bool bEnable) noexcept
{
	if (!m_webview || !m_Hwnd)
		return;

	if (bEnable)
	{
		RECT rc{};
		GetWindowRect(GetDesktopWindow(), &rc);
		if (!m_bFullScreen)
			GetWindowRect(m_Hwnd, &m_rcSize);

		SetWindowPos(m_Hwnd, HWND_TOP, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), SWP_SHOWWINDOW);
	}
	else {
		if (m_bFullScreen)
		{
			SetWindowPos(m_Hwnd, GetParent(m_Hwnd), 0, 0, (m_rcSize.right - m_rcSize.left), (m_rcSize.bottom - m_rcSize.top), 0);
		}
		SetRectEmpty(&m_rcSize);
	}
	m_bFullScreen = bEnable;
}

void DcxWebControl2::setScriptingState(bool bEnable)
{
	if (!m_settings)
		return;

	m_settings->put_IsScriptEnabled(bEnable);
}

void DcxWebControl2::setStatusbarState(bool bEnable)
{
	if (!m_settings)
		return;

	m_settings->put_IsStatusBarEnabled(bEnable);
}

void DcxWebControl2::setVisableState(bool bEnable)
{
	if (!m_webviewController)
		return;

	m_webviewController->put_IsVisible(bEnable);
}

void DcxWebControl2::setDownloadsState(bool bEnable) noexcept
{
	m_bAllowDownloads = bEnable;
}

void DcxWebControl2::setManageNewWindows(bool bEnable) noexcept
{
	m_bManageNewWindows = bEnable;
}

void DcxWebControl2::setDownloadsDialogState(bool bEnable) noexcept
{
	m_bDownloadsDialogEnabled = bEnable;
}

void DcxWebControl2::setAudioMuteState(bool bEnable)
{
	if (auto webview2_8 = m_webview.try_query<ICoreWebView2_8>(); webview2_8)
		webview2_8->put_IsMuted(bEnable);
}

void DcxWebControl2::setURL(const TString& tsURL, const TString& tsFlags, const TString& tsMask)
{
	if (!m_webview || !m_settings)
		return;

	const XSwitchFlags xflags(tsFlags);			// flags to change
	const XSwitchFlags xmask(tsMask);			// state mask, flags here are enabled, otherwise they are disabled.

	//if (xflags['h'] && xmask['h']) // no history
	//	webview->
	//if (xflags['r'] && xmask['r']) // no read from cache
	//	V_I4(&vFlags) |= navNoReadFromCache;
	//if (xflags['w'] && xmask['w']) // no write to cache
	//	V_I4(&vFlags) |= navNoWriteToCache;
	//if (xflags['a'] && xmask['a']) // allow auto search
	//	V_I4(&vFlags) |= navAllowAutosearch;
	//if (xflags['e'] && xmask['e']) // enforce restricted zone
	//	V_I4(&vFlags) |= navEnforceRestricted;
	if (xflags['m']) // Manage popup windows.
		setManageNewWindows(xmask['m']);
	if (xflags['u']) // allow downloads?
		setDownloadsState(xmask['u']);
	if (xflags['U']) // allow downloads?
		setDownloadsDialogState(xmask['U']);
	//if (xflags['x'] && xmask['x']) // allow active x install prompts (doesnt auto install, you still need to ok the prompt)
	//	V_I4(&vFlags) |= navTrustedForActiveX;
	if (xflags['f']) // fullscreen on/off
		setFullScreenState(xmask['f']);
	if (xflags['s']) // statusbar on/off
		this->setStatusbarState(xmask['s']);
	if (xflags['A']) // Mute Audio on/off
		this->setAudioMuteState(xmask['A']);

	if (!tsURL.empty())
		m_webview->Navigate(tsURL.to_wchr());
}

void DcxWebControl2::setDownloadDir(const TString& tsDir)
{
	if (auto webView2_13 = m_webview.try_query<ICoreWebView2_13>(); webView2_13)
	{
		wil::com_ptr<ICoreWebView2Profile> profile;
		if (SUCCEEDED(webView2_13->get_Profile(&profile)))
		{
			profile->put_DefaultDownloadFolderPath(tsDir.to_wchr());
		}
	}
}

void DcxWebControl2::setPreferedColourScheme(COREWEBVIEW2_PREFERRED_COLOR_SCHEME value)
{
	if (auto webView2_13 = m_webview.try_query<ICoreWebView2_13>(); webView2_13)
	{
		wil::com_ptr<ICoreWebView2Profile> profile;
		if (SUCCEEDED(webView2_13->get_Profile(&profile)))
		{
			profile->put_PreferredColorScheme(value);
		}
	}
}

void DcxWebControl2::CallScript(const TString& tsCmd)
{
	if (!m_webview)
		return;

	m_webview->ExecuteScript(tsCmd.to_chr(), Microsoft::WRL::Callback<ICoreWebView2ExecuteScriptCompletedHandler>(this, &DcxWebControl2::OnExecuteScriptCompleted).Get());

	return;
}

void DcxWebControl2::ClearCache()
{
	if (auto webView2_13 = m_webview.try_query<ICoreWebView2_13>(); webView2_13)
	{
		wil::com_ptr<ICoreWebView2Profile> profile;
		if (SUCCEEDED(webView2_13->get_Profile(&profile)))
		{
			if (auto profile2 = profile.try_query<ICoreWebView2Profile2>(); profile2)
			{
				profile2->ClearBrowsingDataAll(Microsoft::WRL::Callback<ICoreWebView2ClearBrowsingDataCompletedHandler>(this, &DcxWebControl2::OnClearBrowsingDataCompleted).Get());
			}
		}
	}
}

void DcxWebControl2::ClearCacheKind(COREWEBVIEW2_BROWSING_DATA_KINDS kind)
{
	if (auto webView2_13 = m_webview.try_query<ICoreWebView2_13>(); webView2_13)
	{
		wil::com_ptr<ICoreWebView2Profile> profile;
		if (SUCCEEDED(webView2_13->get_Profile(&profile)))
		{
			if (auto profile2 = profile.try_query<ICoreWebView2Profile2>(); profile2)
			{
				profile2->ClearBrowsingData(kind, Microsoft::WRL::Callback<ICoreWebView2ClearBrowsingDataCompletedHandler>(this, &DcxWebControl2::OnClearBrowsingDataCompleted).Get());
			}
		}
	}
}

HRESULT DcxWebControl2::OnCreateCoreWebView2EnvironmentCompleted(HRESULT result, ICoreWebView2Environment* env)
{
	if (!env)
		return E_FAIL;

	if (DcxWebControl2::m_webviewEnvironment == nullptr)
		DcxWebControl2::m_webviewEnvironment = env;

	if (m_dcompDevice)
	{
		if (auto env3 = DcxWebControl2::m_webviewEnvironment.try_query<ICoreWebView2Environment3>(); env3)
		{
			env3->CreateCoreWebView2CompositionController(m_Hwnd, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2CompositionControllerCompletedHandler>(
				[this](HRESULT result, ICoreWebView2CompositionController* compositionController) -> HRESULT {
					auto controller = wil::com_ptr<ICoreWebView2CompositionController>(compositionController).query<ICoreWebView2Controller>();
					return OnCreateCoreWebView2ControllerCompleted(result, controller.get());
				})
				.Get());

			return S_OK;
		}
	}

	// Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
	DcxWebControl2::m_webviewEnvironment->CreateCoreWebView2Controller(m_Hwnd, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(this, &DcxWebControl2::OnCreateCoreWebView2ControllerCompleted).Get());

	return S_OK;
}

HRESULT DcxWebControl2::OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2Controller* controller)
{
	if (!controller)
		return E_FAIL;

	m_webviewController = controller;
	m_webviewController->get_CoreWebView2(&m_webview);

	auto dcomp = m_webviewController.try_query<ICoreWebView2CompositionController>();

	if (!m_webview)
		return E_FAIL;

	// Add a few settings for the webview
	m_webview->get_Settings(&m_settings);

	if (!m_settings)
		return E_FAIL;

	m_settings->put_IsScriptEnabled(TRUE);
	m_settings->put_AreDefaultScriptDialogsEnabled(TRUE);
	m_settings->put_IsWebMessageEnabled(FALSE);	// TRUE

	// Resize WebView to fit the bounds of the parent window
	RECT bounds;
	GetClientRect(m_Hwnd, &bounds);
	m_webviewController->put_Bounds(bounds);

	// <NavigationEvents>
	// register an ICoreWebView2NavigationStartingEventHandler
	m_webview->add_NavigationStarting(Microsoft::WRL::Callback<ICoreWebView2NavigationStartingEventHandler>(this, &DcxWebControl2::OnNavigationStarting).Get(), &m_navStartToken);
	m_webview->add_NavigationCompleted(Microsoft::WRL::Callback<ICoreWebView2NavigationCompletedEventHandler>(this, &DcxWebControl2::OnNavigationCompleted).Get(), &m_navEndToken);
	// </NavigationEvents>

	m_webview->add_DocumentTitleChanged(Microsoft::WRL::Callback<ICoreWebView2DocumentTitleChangedEventHandler>(this, &DcxWebControl2::OnDocumentTitleChanged).Get(), &m_titleToken);
	m_webview->add_ContainsFullScreenElementChanged(Microsoft::WRL::Callback<ICoreWebView2ContainsFullScreenElementChangedEventHandler>(this, &DcxWebControl2::OnContainsFullScreenElementChanged).Get(), &m_fullscreenToken);
	m_webview->add_HistoryChanged(Microsoft::WRL::Callback<ICoreWebView2HistoryChangedEventHandler>(this, &DcxWebControl2::OnHistoryChanged).Get(), &m_historyChangedToken);
	m_webview->add_NewWindowRequested(Microsoft::WRL::Callback<ICoreWebView2NewWindowRequestedEventHandler>(this, &DcxWebControl2::OnNewWindowRequested).Get(), &m_newWindowRequestedToken);

	// these can silently fail as an unsupported feature.
	if (auto webview4 = m_webview.try_query<ICoreWebView2_4>(); webview4)
		webview4->add_DownloadStarting(Microsoft::WRL::Callback<ICoreWebView2DownloadStartingEventHandler>(this, &DcxWebControl2::OnDownloadStarting).Get(), &m_downloadStartingToken);

	if (auto webview8 = m_webview.try_query<ICoreWebView2_8>(); webview8)
	{
		webview8->add_IsDocumentPlayingAudioChanged(Microsoft::WRL::Callback<ICoreWebView2IsDocumentPlayingAudioChangedEventHandler>(this, &DcxWebControl2::OnDocumentPlayingAudioChanged).Get(), &m_documentplayingaudioToken);
		webview8->add_IsMutedChanged(Microsoft::WRL::Callback<ICoreWebView2IsMutedChangedEventHandler>(this, &DcxWebControl2::OnMutedChanged).Get(), &m_mutedToken);
	}

	if (auto webview11 = m_webview.try_query<ICoreWebView2_11>(); webview11)
		webview11->add_ContextMenuRequested(Microsoft::WRL::Callback<ICoreWebView2ContextMenuRequestedEventHandler>(this, &DcxWebControl2::OnContextMenu).Get(), &m_contextmenuToken);

	if (auto webview12 = m_webview.try_query<ICoreWebView2_12>(); webview12)
		webview12->add_StatusBarTextChanged(Microsoft::WRL::Callback<ICoreWebView2StatusBarTextChangedEventHandler>(this, &DcxWebControl2::OnStatusBarTextChanged).Get(), &m_statusbarToken);

	if (auto webview15 = m_webview.try_query<ICoreWebView2_15>(); webview15)
		webview15->add_FaviconChanged(Microsoft::WRL::Callback<ICoreWebView2FaviconChangedEventHandler>(this, &DcxWebControl2::OnFaviconChanged).Get(), &m_faviconChangedToken);

	// Schedule an async task to navigate to Bing
	//webview->Navigate(L"https://www.bing.com/");
	//webview->Navigate(L"about:blank");

	setVisableState(true);

	return S_OK;
}

HRESULT DcxWebControl2::OnNavigationStarting(ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args)
{
	if (!sender || !args)
		return E_FAIL;

	wil::unique_cotaskmem_string uri;
	args->get_Uri(&uri);

	//TCHAR szRes[64]{};
	//if (!evalAliasEx(&szRes[0], std::size(szRes), L"nav_begin,%u,%s", getUserID(), uri.get()))
	//	args->put_Cancel(true);

	if (const auto pd = getParentDialog(); pd)
	{
		TString tsBuf;
		mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_text %"), uri.get());
		mIRCLinker::eval(tsBuf, TEXT("$%(%,nav_begin,%,%dcx_text)"), pd->getAliasName(), pd->getName(), getUserID());

		if (tsBuf == L"cancel")
			args->put_Cancel(true);
	}
	return S_OK;
}

HRESULT DcxWebControl2::OnNavigationCompleted(ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args)
{
	if (!sender || !args)
		return E_FAIL;

	BOOL bOK{};
	args->get_IsSuccess(&bOK);

	execAliasEx(L"nav_complete,%u,%u", getUserID(), bOK);

	return S_OK;
}

HRESULT DcxWebControl2::OnDocumentTitleChanged(ICoreWebView2* sender, IUnknown* args)
{
	if (!sender)
		return E_FAIL;

	wil::unique_cotaskmem_string title;
	sender->get_DocumentTitle(&title);

	//execAliasEx(L"title,%u,%s", getUserID(), title.get());

	if (const auto pd = getParentDialog(); pd)
	{
		mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_text %"), title.get());
		mIRCLinker::eval(nullptr, TEXT("$%(%,title,%,%dcx_text)"), pd->getAliasName(), pd->getName(), getUserID());
	}

	return S_OK;
}

HRESULT DcxWebControl2::OnExecuteScriptCompleted(HRESULT errorCode, LPCWSTR resultObjectAsJson)
{
	//execAliasEx(L"script,%u,%s", getUserID(), resultObjectAsJson);

	if (const auto pd = getParentDialog(); pd)
	{
		mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_text %"), resultObjectAsJson);
		mIRCLinker::eval(nullptr, TEXT("$%(%,script,%,%dcx_text)"), pd->getAliasName(), pd->getName(), getUserID());
	}

	return S_OK;
}

HRESULT DcxWebControl2::OnContainsFullScreenElementChanged(ICoreWebView2* sender, IUnknown* args)
{
	if (!sender)
		return E_FAIL;

	BOOL bFullScreen{};
	sender->get_ContainsFullScreenElement(&bFullScreen);

	TCHAR szRes[64]{};
	if (evalAliasEx(&szRes[0], std::size(szRes), L"fullscreen,%u,%u", getUserID(), bFullScreen))
		setFullScreenState(bFullScreen);

	return S_OK;
}

HRESULT DcxWebControl2::OnStatusBarTextChanged(ICoreWebView2* sender, IUnknown* args)
{
	if (!sender)
		return E_FAIL;

	auto tsText(getStatusText());

	//execAliasEx(L"status,%u,%s", getUserID(), tsText.to_chr());

	if (const auto pd = getParentDialog(); pd)
	{
		mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_text %"), tsText);
		mIRCLinker::eval(nullptr, TEXT("$%(%,status,%,%dcx_text)"), pd->getAliasName(), pd->getName(), getUserID());
	}

	return S_OK;
}

HRESULT DcxWebControl2::OnHistoryChanged(ICoreWebView2* sender, IUnknown* args)
{
	if (!sender)
		return E_FAIL;

	BOOL canGoBack;
	BOOL canGoForward;
	sender->get_CanGoBack(&canGoBack);
	sender->get_CanGoForward(&canGoForward);

	execAliasEx(L"history,%u,%u,%u", getUserID(), canGoBack, canGoForward);

	return S_OK;
}

HRESULT DcxWebControl2::OnFaviconChanged(ICoreWebView2* sender, IUnknown* args)
{
	auto webview15 = m_webview.try_query<ICoreWebView2_15>();
	if (!webview15)
		return E_FAIL;

	wil::unique_cotaskmem_string url;

	webview15->get_FaviconUri(&url);
	const TString tsURL(url.get());

#ifdef DCX_USE_GDIPLUS
	TString tsBuf((UINT)MIRC_BUFFER_SIZE_CCH);
	evalAliasEx(tsBuf.to_wchr(), tsBuf.capacity_cch(), L"favicon,%u,changed,%s", getUserID(), tsURL.to_wchr());
	if (!tsBuf.empty())
	{
		if (tsBuf.getfirsttok(1) == L"save")
		{
			TString tsFile(tsBuf.getlasttoks().trim());
			return webview15->GetFavicon(COREWEBVIEW2_FAVICON_IMAGE_FORMAT_PNG,
				Microsoft::WRL::Callback<ICoreWebView2GetFaviconCompletedHandler>(
					[this, tsFile](HRESULT errorCode, IStream* iconStream) -> HRESULT
					{
						if (errorCode == S_OK)
						{
							Gdiplus::Bitmap iconBitmap(iconStream);
							TString tsLocalFile(tsFile);
							if (SavePNGFile(tsLocalFile, iconBitmap))
								execAliasEx(L"favicon,%u,saved,%s", getUserID(), tsFile.to_wchr());
							else
								execAliasEx(L"favicon,%u,failed,%s", getUserID(), tsFile.to_wchr());
							return S_OK;
						}
						return E_FAIL;
					})
				.Get());
		}
	}
	return S_OK;
#else
	TString tsBuf((UINT)MIRC_BUFFER_SIZE_CCH);
	evalAliasEx(tsBuf.to_wchr(), tsBuf.capacity_cch(), L"favicon,%u,changed,%s", getUserID(), tsURL.to_wchr());
	return S_OK;
#endif
}

HRESULT DcxWebControl2::OnBytesReceivedChanged(ICoreWebView2DownloadOperation* download, IUnknown* args)
{
	if (!download)
		return E_FAIL;

	INT64 iBytes{}, iTotalBytes{};
	download->get_BytesReceived(&iBytes);
	download->get_TotalBytesToReceive(&iTotalBytes);

	wil::unique_cotaskmem_string filename;
	download->get_ResultFilePath(&filename);
	
	TString tsBuf((UINT)MIRC_BUFFER_SIZE_CCH);
	evalAliasEx(tsBuf.to_wchr(), tsBuf.capacity_cch(), L"dl_progress,%u,%lli,%lli,%s", getUserID(), iBytes, iTotalBytes, filename.get());
	if (tsBuf == L"cancel")
		download->Cancel();
	return S_OK;
}

HRESULT DcxWebControl2::OnStateChanged(ICoreWebView2DownloadOperation* download, IUnknown* args)
{
	if (!download)
		return E_FAIL;

	COREWEBVIEW2_DOWNLOAD_STATE downloadState;
	if (FAILED(download->get_State(&downloadState)))
		return E_FAIL;

	TString tsBuf((UINT)MIRC_BUFFER_SIZE_CCH);

	wil::unique_cotaskmem_string filename;
	download->get_ResultFilePath(&filename);

	switch (downloadState)
	{
	case COREWEBVIEW2_DOWNLOAD_STATE_IN_PROGRESS:
		break;
	case COREWEBVIEW2_DOWNLOAD_STATE_INTERRUPTED:
	{
		COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON reason;
		download->get_InterruptReason(&reason);

		execAliasEx(L"dl_canceled,%u,%u,%s", getUserID(), reason, filename.get());
	}
	break;
	case COREWEBVIEW2_DOWNLOAD_STATE_COMPLETED:
	{
		execAliasEx(L"dl_completed,%u,%s", getUserID(), filename.get());
	}
	break;
	}
	return S_OK;
}

HRESULT DcxWebControl2::OnDownloadStarting(ICoreWebView2* sender, ICoreWebView2DownloadStartingEventArgs* args)
{
	if (!sender || !args)
		return E_FAIL;

	if (!this->IsDownloadingEnabled())
	{
		args->put_Cancel(TRUE);
		return S_OK;
	}
	if (!this->IsDownloadsDialogEnabled())
		args->put_Handled(TRUE);

	wil::com_ptr<ICoreWebView2DownloadOperation> download;
	if (FAILED(args->get_DownloadOperation(&download)))
		return E_FAIL;

	if (!download)
		return E_FAIL;

	INT64 iBytes{};
	download->get_TotalBytesToReceive(&iBytes);
	wil::unique_cotaskmem_string filename;
	args->get_ResultFilePath(&filename);

	TString tsBuf((UINT)MIRC_BUFFER_SIZE_CCH);
	evalAliasEx(tsBuf.to_wchr(), tsBuf.capacity_cch(), L"dl_begin,%u,%lli,%s", getUserID(), iBytes, filename.get());
	switch (std::hash<TString>()(tsBuf.getfirsttok(1)))
	{
	case L"cancel"_hash:
	{
		args->put_Cancel(TRUE);
	}
	break;
	case L"change"_hash:
	{
		const TString tsFilename(tsBuf.getlasttoks());
		args->put_ResultFilePath(tsFilename.to_wchr());
	}
	[[fallthrough]];
	default:
	{
		download->add_BytesReceivedChanged(Microsoft::WRL::Callback<ICoreWebView2BytesReceivedChangedEventHandler>(this, &DcxWebControl2::OnBytesReceivedChanged).Get(), &m_bytesReceivedChangedToken);
		download->add_StateChanged(Microsoft::WRL::Callback<ICoreWebView2StateChangedEventHandler>(this, &DcxWebControl2::OnStateChanged).Get(), &m_stateChangedToken);
	}
	break;
	}
	return S_OK;
}

HRESULT DcxWebControl2::OnClearBrowsingDataCompleted(HRESULT errorCode)
{
	execAliasEx(L"cache,%u,cleared,%u", getUserID(), errorCode);
	return S_OK;
}

HRESULT DcxWebControl2::OnNewWindowRequested(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args)
{
	if (!sender || !args)
		return E_FAIL;

	if (this->IsNewWindowsManaged())
	{
		//wil::unique_cotaskmem_string uri;
		//args->get_Uri(&uri);
		//TString tsBuf((UINT)MIRC_BUFFER_SIZE_CCH);
		//evalAliasEx(tsBuf.to_wchr(), tsBuf.capacity_cch(), L"win_open,%u,%s", getUserID(), uri.get());
		//if (tsBuf == L"cancel")
		//	args->put_Handled(TRUE);

		if (const auto pd = getParentDialog(); pd)
		{
			wil::unique_cotaskmem_string uri;
			args->get_Uri(&uri);

			TString tsBuf((UINT)MIRC_BUFFER_SIZE_CCH);

			mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_text %"), uri.get());
			mIRCLinker::eval(tsBuf, TEXT("$%(%,win_open,%,%dcx_text)"), pd->getAliasName(), pd->getName(), getUserID());

			if (tsBuf == L"cancel")
				args->put_Handled(TRUE);
		}
	}
	else
		args->put_Handled(FALSE);
	return S_OK;
}

HRESULT DcxWebControl2::OnContextMenu(ICoreWebView2* sender, ICoreWebView2ContextMenuRequestedEventArgs* eventArgs)
{
	wil::com_ptr<ICoreWebView2ContextMenuTarget> target;
	if (FAILED(eventArgs->get_ContextMenuTarget(&target)))
		return E_FAIL;

	COREWEBVIEW2_CONTEXT_MENU_TARGET_KIND targetKind;
	if (FAILED(target->get_Kind(&targetKind)))
		return E_FAIL;

	auto getKind = [](COREWEBVIEW2_CONTEXT_MENU_TARGET_KIND targetKind) noexcept {
#define __SWITCH_KIND(x) L##x
#define _SWITCH_KIND(x) __SWITCH_KIND(#x)
#define SWITCH_KIND(x) case x: return _SWITCH_KIND(x)
		switch (targetKind)
		{
			SWITCH_KIND(COREWEBVIEW2_CONTEXT_MENU_TARGET_KIND_PAGE);
			SWITCH_KIND(COREWEBVIEW2_CONTEXT_MENU_TARGET_KIND_SELECTED_TEXT);
			SWITCH_KIND(COREWEBVIEW2_CONTEXT_MENU_TARGET_KIND_IMAGE);
			SWITCH_KIND(COREWEBVIEW2_CONTEXT_MENU_TARGET_KIND_AUDIO);
			SWITCH_KIND(COREWEBVIEW2_CONTEXT_MENU_TARGET_KIND_VIDEO);
		default:
			break;
		}
		return L"COREWEBVIEW2_CONTEXT_MENU_TARGET_KIND_PAGE";
	};

	TString tsBuf((UINT)MIRC_BUFFER_SIZE_CCH);
	evalAliasEx(tsBuf.to_wchr(), tsBuf.capacity_cch(), L"contextmenu,%u,%s", getUserID(), getKind(targetKind));

	return S_OK;
}

HRESULT DcxWebControl2::OnDocumentPlayingAudioChanged(ICoreWebView2* sender, IUnknown* eventArgs)
{
	if (auto webview2_8 = m_webview.try_query<ICoreWebView2_8>(); webview2_8)
	{
		BOOL isDocumentPlayingAudio;
		webview2_8->get_IsDocumentPlayingAudio(&isDocumentPlayingAudio);

		TString tsBuf((UINT)MIRC_BUFFER_SIZE_CCH);
		evalAliasEx(tsBuf.to_wchr(), tsBuf.capacity_cch(), L"audio,%u,%s", getUserID(), (isDocumentPlayingAudio ? L"playing" : L"stopped"));
	}

	return S_OK;
}

HRESULT DcxWebControl2::OnMutedChanged(ICoreWebView2* sender, IUnknown* eventArgs)
{
	if (auto webview2_8 = m_webview.try_query<ICoreWebView2_8>(); webview2_8)
	{
		BOOL isMuted;
		webview2_8->get_IsMuted(&isMuted);

		TString tsBuf((UINT)MIRC_BUFFER_SIZE_CCH);
		evalAliasEx(tsBuf.to_wchr(), tsBuf.capacity_cch(), L"audio,%u,%s", getUserID(), (isMuted ? L"mute" : L"unmute"));
	}

	return S_OK;
}
