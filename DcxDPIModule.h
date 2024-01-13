#pragma once
#ifndef _DCXDPIMODULES_H_
#define _DCXDPIMODULES_H_

#include "dcxmodule.h"

class DcxDPIModule final
	: public DcxModule
{
	static inline decltype(::GetDpiForSystem)* GetDpiForSystemUx = nullptr;
	static inline decltype(::GetDpiForWindow)* GetDpiForWindowUx= nullptr;
	static inline decltype(::GetSystemMetricsForDpi)* GetSystemMetricsForDpiUx = nullptr;
	static inline decltype(::SetProcessDpiAwarenessContext)* SetProcessDpiAwarenessContextUx = nullptr;
	static inline decltype(::InheritWindowMonitor)* InheritWindowMonitorUx = nullptr;
	static inline decltype(::SetThreadDpiHostingBehavior)* SetThreadDpiHostingBehaviorUx = nullptr;

public:
	constexpr DcxDPIModule(void) noexcept
		: DcxModule()
	{}
	~DcxDPIModule(void) noexcept;

	DcxDPIModule(const DcxDPIModule& other) = delete;	// no copy constructor
	DcxDPIModule(const DcxDPIModule&& other) = delete;	// no move constructor
	GSL_SUPPRESS(c.128) DcxDPIModule& operator =(const DcxDPIModule&) = delete;	// No assignments!
	GSL_SUPPRESS(c.128) DcxDPIModule& operator =(const DcxDPIModule&&) = delete;	// No move assignments!

	bool load(void) final;
	bool unload(void) noexcept final;

	static UINT dcxGetDpiForSystem(void) noexcept;
	static UINT dcxGetDpiForWindow(_In_ _Maybenull_ HWND hwnd) noexcept;
	static UINT dcxGetSystemMetricsForDpi(_In_ int nIndex, _In_ UINT dpi) noexcept;
	static UINT dcxGetWindowMetrics(_In_ _Maybenull_ HWND hwnd, _In_ int nIndex) noexcept;
	static BOOL dcxSetProcessDpiAwarenessContext(_In_ DPI_AWARENESS_CONTEXT value) noexcept;
	static BOOL dcxInheritWindowMonitor(_In_ HWND hwnd, _In_ HWND hwndInherit) noexcept;
	static DPI_HOSTING_BEHAVIOR dcxSetThreadDpiHostingBehavior(_In_ DPI_HOSTING_BEHAVIOR value) noexcept;
};
#endif // _DCXUXMODULES_H_
