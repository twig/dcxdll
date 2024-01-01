#pragma once
#ifndef _DCXWEBVIEW2MODULE_H_
#define _DCXWEBVIEW2MODULE_H_

#include "dcxmodule.h"

#include <WebView2.h>

class DcxWebView2Module final
	: public DcxModule
{
	static inline decltype(::GetAvailableCoreWebView2BrowserVersionString)* GetAvailableCoreWebView2BrowserVersionStringUx = nullptr;
	static inline decltype(::CreateCoreWebView2EnvironmentWithOptions)* CreateCoreWebView2EnvironmentWithOptionsUx = nullptr;
	static inline decltype(::CreateCoreWebView2Environment)* CreateCoreWebView2EnvironmentUx = nullptr;
	static inline decltype(::CompareBrowserVersions)* CompareBrowserVersionsUx = nullptr;

public:
	constexpr DcxWebView2Module(void) noexcept
		: DcxModule()
	{}
	~DcxWebView2Module(void) noexcept;

	DcxWebView2Module(const DcxWebView2Module& other) = delete;	// no copy constructor
	DcxWebView2Module(const DcxWebView2Module&& other) = delete;	// no move constructor
	GSL_SUPPRESS(c.128) DcxWebView2Module& operator =(const DcxWebView2Module&) = delete;	// No assignments!
	GSL_SUPPRESS(c.128) DcxWebView2Module& operator =(const DcxWebView2Module&&) = delete;	// No move assignments!

	bool load(void) final;
	bool unload(void) noexcept final;

	static HRESULT dcxGetAvailableCoreWebView2BrowserVersionString(PCWSTR browserExecutableFolder, LPWSTR* versionInfo) noexcept;
	static HRESULT dcxCreateCoreWebView2EnvironmentWithOptions(PCWSTR browserExecutableFolder, PCWSTR userDataFolder, ICoreWebView2EnvironmentOptions* environmentOptions, ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* environmentCreatedHandler) noexcept;
	static HRESULT dcxCreateCoreWebView2Environment(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* environmentCreatedHandler) noexcept;
	static HRESULT dcxCompareBrowserVersions(PCWSTR version1, PCWSTR version2, int* result) noexcept;
};
#endif // _DCXWEBVIEW2MODULE_H_
