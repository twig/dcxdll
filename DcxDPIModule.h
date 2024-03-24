#pragma once
#ifndef _DCXDPIMODULES_H_
#define _DCXDPIMODULES_H_

#include "dcxmodule.h"

class DcxDPIModule final
	: public DcxModule
{
	static inline decltype(::AdjustWindowRectExForDpi)* AdjustWindowRectExForDpiUx = nullptr;
	static inline decltype(::AreDpiAwarenessContextsEqual)* AreDpiAwarenessContextsEqualUx = nullptr;
	static inline decltype(::EnableNonClientDpiScaling)* EnableNonClientDpiScalingUx = nullptr;
	static inline decltype(::GetAwarenessFromDpiAwarenessContext)* GetAwarenessFromDpiAwarenessContextUx = nullptr;
	static inline decltype(::GetDialogControlDpiChangeBehavior)* GetDialogControlDpiChangeBehaviorUx = nullptr;
	static inline decltype(::GetDialogDpiChangeBehavior)* GetDialogDpiChangeBehaviorUx = nullptr;
	static inline decltype(::GetDpiAwarenessContextForProcess)* GetDpiAwarenessContextForProcessUx = nullptr;
	static inline decltype(::GetDpiForSystem)* GetDpiForSystemUx = nullptr;
	static inline decltype(::GetDpiForWindow)* GetDpiForWindowUx = nullptr;
	static inline decltype(::GetDpiFromDpiAwarenessContext)* GetDpiFromDpiAwarenessContextUx = nullptr;
	static inline decltype(::GetSystemDpiForProcess)* GetSystemDpiForProcessUx = nullptr;
	static inline decltype(::GetSystemMetricsForDpi)* GetSystemMetricsForDpiUx = nullptr;
	static inline decltype(::GetThreadDpiAwarenessContext)* GetThreadDpiAwarenessContextUx = nullptr;
	static inline decltype(::GetThreadDpiHostingBehavior)* GetThreadDpiHostingBehaviorUx = nullptr;
	static inline decltype(::GetWindowDpiAwarenessContext)* GetWindowDpiAwarenessContextUx = nullptr;
	static inline decltype(::GetWindowDpiHostingBehavior)* GetWindowDpiHostingBehaviorUx = nullptr;
	static inline decltype(::InheritWindowMonitor)* InheritWindowMonitorUx = nullptr;
	static inline decltype(::IsValidDpiAwarenessContext)* IsValidDpiAwarenessContextUx = nullptr;
	static inline decltype(::SetDialogControlDpiChangeBehavior)* SetDialogControlDpiChangeBehaviorUx = nullptr;
	static inline decltype(::SetDialogDpiChangeBehavior)* SetDialogDpiChangeBehaviorUx = nullptr;
	static inline decltype(::SetProcessDpiAwarenessContext)* SetProcessDpiAwarenessContextUx = nullptr;
	static inline decltype(::SetThreadCursorCreationScaling)* SetThreadCursorCreationScalingUx = nullptr;
	static inline decltype(::SetThreadDpiAwarenessContext)* SetThreadDpiAwarenessContextUx = nullptr;
	static inline decltype(::SetThreadDpiHostingBehavior)* SetThreadDpiHostingBehaviorUx = nullptr;
	static inline decltype(::SystemParametersInfoForDpi)* SystemParametersInfoForDpiUx = nullptr;

public:
	constexpr DcxDPIModule(void) noexcept
		: DcxModule()
	{}
	~DcxDPIModule(void) noexcept;

	DcxDPIModule(const DcxDPIModule& other) = delete;	// no copy constructor
	DcxDPIModule(const DcxDPIModule&& other) = delete;	// no move constructor
	GSL_SUPPRESS(c.128) DcxDPIModule& operator =(const DcxDPIModule&) = delete;	// No assignments!
	GSL_SUPPRESS(c.128) DcxDPIModule& operator =(DcxDPIModule&&) = delete;	// No move assignments!

	bool load(void) final;
	bool unload(void) noexcept final;

	static BOOL dcxAdjustWindowRectExForDpi(_Inout_ LPRECT lpRect,_In_ DWORD dwStyle,_In_ BOOL bMenu,_In_ DWORD dwExStyle,_In_ UINT dpi) noexcept;
	static BOOL dcxAreDpiAwarenessContextsEqual(_In_ DPI_AWARENESS_CONTEXT dpiContextA,_In_ DPI_AWARENESS_CONTEXT dpiContextB) noexcept;
	static BOOL dcxEnableNonClientDpiScaling(_In_ HWND hwnd) noexcept;
	static DPI_AWARENESS dcxGetAwarenessFromDpiAwarenessContext(_In_ DPI_AWARENESS_CONTEXT value) noexcept;
	static DIALOG_CONTROL_DPI_CHANGE_BEHAVIORS dcxGetDialogControlDpiChangeBehavior(HWND hWnd) noexcept;
	static DIALOG_DPI_CHANGE_BEHAVIORS dcxGetDialogDpiChangeBehavior(HWND hDlg) noexcept;
	static DPI_AWARENESS_CONTEXT dcxGetDpiAwarenessContextForProcess(HANDLE hProcess) noexcept;
	static UINT dcxGetDpiForSystem(void) noexcept;
	static UINT dcxGetDpiForWindow(_In_opt_ HWND hwnd) noexcept;
	static UINT dcxGetDpiFromDpiAwarenessContext(_In_ DPI_AWARENESS_CONTEXT value) noexcept;
	static UINT dcxGetSystemDpiForProcess(HANDLE hProcess) noexcept;
	static UINT dcxGetSystemMetricsForDpi(_In_ int nIndex, _In_ UINT dpi) noexcept;
	static DPI_AWARENESS_CONTEXT dcxGetThreadDpiAwarenessContext() noexcept;
	static DPI_HOSTING_BEHAVIOR dcxGetThreadDpiHostingBehavior() noexcept;
	static DPI_AWARENESS_CONTEXT dcxGetWindowDpiAwarenessContext(_In_ HWND hwnd) noexcept;
	static DPI_HOSTING_BEHAVIOR dcxGetWindowDpiHostingBehavior(_In_ HWND hwnd) noexcept;
	static BOOL dcxInheritWindowMonitor(_In_ HWND hwnd, _In_opt_ HWND hwndInherit) noexcept;
	static BOOL dcxIsValidDpiAwarenessContext(_In_ DPI_AWARENESS_CONTEXT value) noexcept;
	static BOOL dcxSetDialogControlDpiChangeBehavior(HWND hWnd, _In_ DIALOG_CONTROL_DPI_CHANGE_BEHAVIORS mask, _In_ DIALOG_CONTROL_DPI_CHANGE_BEHAVIORS values) noexcept;
	static BOOL dcxSetDialogDpiChangeBehavior(HWND hDlg, _In_ DIALOG_DPI_CHANGE_BEHAVIORS mask, _In_ DIALOG_DPI_CHANGE_BEHAVIORS values) noexcept;
	static BOOL dcxSetProcessDpiAwarenessContext(_In_ DPI_AWARENESS_CONTEXT value) noexcept;
	static UINT dcxSetThreadCursorCreationScaling(_In_ UINT cursorDpi) noexcept;
	static DPI_AWARENESS_CONTEXT dcxSetThreadDpiAwarenessContext(_In_ DPI_AWARENESS_CONTEXT dpiContext) noexcept;
	static DPI_HOSTING_BEHAVIOR dcxSetThreadDpiHostingBehavior(_In_ DPI_HOSTING_BEHAVIOR value) noexcept;

	static BOOL dcxSystemParametersInfoForDpi(_In_ UINT uiAction,_In_ UINT uiParam,_Inout_ PVOID pvParam,_In_ UINT fWinIni,_In_ UINT dpi) noexcept;
	static UINT dcxGetWindowMetrics(_In_opt_ HWND hwnd, _In_ int nIndex) noexcept;
};
#endif // _DCXUXMODULES_H_
