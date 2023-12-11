#pragma once

#include "defines.h"
#include "Dcx.h"
#include "Classes/dcxcontrol.h"

#include <wrl.h>
#include <wil/com.h>
// <IncludeHeader>
// include WebView2 header
#include "WebView2.h"
// </IncludeHeader>

#include <dcomp.h>

#pragma comment(lib, "Urlmon.lib")

class DcxWebControl2 :
    public DcxControl
{
public:
	DcxWebControl2() = delete;
	DcxWebControl2(const DcxWebControl2&) = delete;
	GSL_SUPPRESS(c.128) DcxWebControl2& operator =(const DcxWebControl2&) = delete;	// No assignments!
	DcxWebControl2(DcxWebControl2&&) = delete;
	GSL_SUPPRESS(c.128) DcxWebControl2& operator =(DcxWebControl2&&) = delete;
	bool operator==(const DcxWebControl2& other) const = default;

	DcxWebControl2(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxWebControl2() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept final;

	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	inline const TString getType() const final { return TEXT("web2ctrl"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::WEB2CTRL; }

	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml() const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

	static DWORD __stdcall DownloadAndInstallWV2RT(_In_ LPVOID lpParameter) noexcept;

	void InstallComplete(int return_code) noexcept;

	bool InitializeInterface();

	TString getURL() const;
	bool IsStatusbarEnabled() const;
	bool IsFullScreenEnabled() const;
	bool IsScriptingEnabled() const;
	TString getStatusText() const;
	TString getReadyState() const;

	void setFullScreenState(bool bEnable) noexcept;
	void setScriptingState(bool bEnable);
	void setStatusbarState(bool bEnable);
	void setVisableState(bool bEnable);
	void setURL(const TString& tsURL, const TString& tsFlags, const TString& tsMask);
	void CallScript(const TString& tsCmd);

private:
	// Pointer to WebViewController
	wil::com_ptr<ICoreWebView2Controller> m_webviewController;

	// Pointer to WebView window
	wil::com_ptr<ICoreWebView2> m_webview;

	//
   	static inline wil::com_ptr<ICoreWebView2Environment> m_webviewEnvironment;

	// settings...
	wil::com_ptr<ICoreWebView2Settings> m_settings;

	//
	wil::com_ptr<IDCompositionDevice> m_dcompDevice;

	EventRegistrationToken m_navStartToken;
	EventRegistrationToken m_navEndToken;
	EventRegistrationToken m_titleToken;
	EventRegistrationToken m_fullscreenToken;
	EventRegistrationToken m_statusbarToken;
	EventRegistrationToken m_historyChangedToken;
	EventRegistrationToken m_faviconChangedToken;

	bool m_bFullScreen{};
	RECT m_rcSize{};
	TString m_tsHome;
	wil::unique_hicon m_favicon;

	HRESULT OnCreateCoreWebView2EnvironmentCompleted(HRESULT result, ICoreWebView2Environment* env);
	HRESULT OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2Controller* controller);
	HRESULT OnNavigationStarting(ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args);
	HRESULT OnNavigationCompleted(ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args);
	HRESULT OnDocumentTitleChanged(ICoreWebView2* sender, IUnknown* args);
	HRESULT OnExecuteScriptCompleted(HRESULT errorCode, LPCWSTR resultObjectAsJson);
	HRESULT OnContainsFullScreenElementChanged(ICoreWebView2* sender, IUnknown* args);
	HRESULT OnStatusBarTextChanged(ICoreWebView2* sender, IUnknown* args);
	HRESULT OnHistoryChanged(ICoreWebView2* sender, IUnknown* args);
	HRESULT OnFaviconChanged(ICoreWebView2* sender, IUnknown* args);
};

