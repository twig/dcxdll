#include "defines.h"
#include "DcxDPIModule.h"
#include "Dcx.h"

#pragma warning(push)
#pragma warning(disable: 26425)	//warning C26425 : Assigning 'nullptr' to a static variable.

DcxDPIModule::~DcxDPIModule() noexcept
{
	unload();
}

bool DcxDPIModule::load()
{
	if (isUseable())
		return false;

	// DPIModule Loading
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Loading User32.DLL..."));
	m_hModule = LoadLibrary(TEXT("User32.DLL"));

	if (m_hModule)
	{
		// Get function pointers.
#pragma warning(push)
#pragma warning(disable: 4191)
#pragma warning(disable: 26493)	//warning C26493 : Don't use C-style casts that would perform a static_cast downcast, const_cast, or reinterpret_cast. (type.4: http://go.microsoft.com/fwlink/p/?LinkID=620420)

#define STRINGIFY(proc) #proc
#define ASSIGN_PROC(proc) proc ## Ux = reinterpret_cast<decltype(::proc)*>(GetProcAddress(m_hModule, STRINGIFY(proc)));

		ASSIGN_PROC(AdjustWindowRectExForDpi);
		ASSIGN_PROC(AreDpiAwarenessContextsEqual);
		ASSIGN_PROC(EnableNonClientDpiScaling);
		ASSIGN_PROC(GetAwarenessFromDpiAwarenessContext);
		ASSIGN_PROC(GetDialogControlDpiChangeBehavior);
		ASSIGN_PROC(GetDialogDpiChangeBehavior);
		ASSIGN_PROC(GetDpiAwarenessContextForProcess);
		ASSIGN_PROC(GetDpiForSystem);
		ASSIGN_PROC(GetDpiForWindow);
		ASSIGN_PROC(GetDpiFromDpiAwarenessContext);
		ASSIGN_PROC(GetSystemDpiForProcess);
		ASSIGN_PROC(GetSystemMetricsForDpi);
		ASSIGN_PROC(GetThreadDpiAwarenessContext);
		ASSIGN_PROC(GetThreadDpiHostingBehavior);
		ASSIGN_PROC(GetWindowDpiAwarenessContext);
		ASSIGN_PROC(GetWindowDpiHostingBehavior);
		ASSIGN_PROC(InheritWindowMonitor);
		ASSIGN_PROC(IsValidDpiAwarenessContext);
		ASSIGN_PROC(SetDialogControlDpiChangeBehavior);
		ASSIGN_PROC(SetDialogDpiChangeBehavior);
		ASSIGN_PROC(SetProcessDpiAwarenessContext);
		ASSIGN_PROC(SetThreadCursorCreationScaling);
		ASSIGN_PROC(SetThreadDpiAwarenessContext);
		ASSIGN_PROC(SetThreadDpiHostingBehavior);
		ASSIGN_PROC(SystemParametersInfoForDpi);

#pragma warning(pop)

		if (GetDpiForSystemUx && GetDpiForWindowUx && GetSystemMetricsForDpiUx)
		{
			DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Found Win10+ DPI Functions"));
		}
		else {
			unload();
			// silently fail...

			DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Missing DPI Functions"));

			//throw Dcx::dcxException("There was a problem loading User32 Library");
		}
	}
	return isUseable();
}

bool DcxDPIModule::unload() noexcept
{
	if (isUseable())
	{
		GSL_SUPPRESS(lifetime.1) FreeLibrary(m_hModule);
		m_hModule = nullptr;
	}
	// make sure all functions are nullptr
	AdjustWindowRectExForDpiUx = nullptr;
	AreDpiAwarenessContextsEqualUx = nullptr;
	EnableNonClientDpiScalingUx = nullptr;
	GetAwarenessFromDpiAwarenessContextUx = nullptr;
	GetDialogControlDpiChangeBehaviorUx = nullptr;
	GetDialogDpiChangeBehaviorUx = nullptr;
	GetDpiAwarenessContextForProcessUx = nullptr;
	GetDpiForSystemUx = nullptr;
	GetDpiForWindowUx = nullptr;
	GetDpiFromDpiAwarenessContextUx = nullptr;
	GetSystemDpiForProcessUx = nullptr;
	GetSystemMetricsForDpiUx = nullptr;
	GetThreadDpiAwarenessContextUx = nullptr;
	GetThreadDpiHostingBehaviorUx = nullptr;
	GetWindowDpiAwarenessContextUx = nullptr;
	GetWindowDpiHostingBehaviorUx = nullptr;
	InheritWindowMonitorUx = nullptr;
	IsValidDpiAwarenessContextUx = nullptr;
	SetDialogControlDpiChangeBehaviorUx = nullptr;
	SetDialogDpiChangeBehaviorUx = nullptr;
	SetProcessDpiAwarenessContextUx = nullptr;
	SetThreadCursorCreationScalingUx = nullptr;
	SetThreadDpiAwarenessContextUx = nullptr;
	SetThreadDpiHostingBehaviorUx = nullptr;
	SystemParametersInfoForDpiUx = nullptr;

	return isUseable();
}
#pragma warning(pop)

/// <summary>
/// Does the same as AdjustWindowRectEx() but taking a given dpi into account.
/// </summary>
/// <param name="lpRect"></param>
/// <param name="dwStyle"></param>
/// <param name="bMenu"></param>
/// <param name="dwExStyle"></param>
/// <param name="dpi"></param>
/// <returns></returns>
BOOL DcxDPIModule::dcxAdjustWindowRectExForDpi(_Inout_ LPRECT lpRect, _In_ DWORD dwStyle, _In_ BOOL bMenu, _In_ DWORD dwExStyle, _In_ UINT dpi) noexcept
{
	if (AdjustWindowRectExForDpiUx)
		return AdjustWindowRectExForDpiUx(lpRect, dwStyle, bMenu, dwExStyle, dpi);
	return AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}

/// <summary>
/// Test if two contexts are the same.
/// </summary>
/// <param name="dpiContextA"></param>
/// <param name="dpiContextB"></param>
/// <returns></returns>
BOOL DcxDPIModule::dcxAreDpiAwarenessContextsEqual(_In_ DPI_AWARENESS_CONTEXT dpiContextA, _In_ DPI_AWARENESS_CONTEXT dpiContextB) noexcept
{
	if (AreDpiAwarenessContextsEqualUx)
		return AreDpiAwarenessContextsEqualUx(dpiContextA, dpiContextB);
	SetLastError(ERROR_INVALID_FUNCTION);
	return FALSE;
}

/// <summary>
/// Enable non client scaling.
/// </summary>
/// <param name="hwnd"></param>
/// <returns></returns>
BOOL DcxDPIModule::dcxEnableNonClientDpiScaling(_In_ HWND hwnd) noexcept
{
	if (EnableNonClientDpiScalingUx)
		return EnableNonClientDpiScalingUx(hwnd);
	SetLastError(ERROR_INVALID_FUNCTION);
	return FALSE;
}

/// <summary>
/// Get the awareness value from a given context.
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
DPI_AWARENESS DcxDPIModule::dcxGetAwarenessFromDpiAwarenessContext(_In_ DPI_AWARENESS_CONTEXT value) noexcept
{
	if (GetAwarenessFromDpiAwarenessContextUx)
		return GetAwarenessFromDpiAwarenessContextUx(value);
	SetLastError(ERROR_INVALID_FUNCTION);
	return DPI_AWARENESS_INVALID;
}

/// <summary>
/// Get the behaviour of a given dialog control when the dpi changes.
/// </summary>
/// <param name="hWnd"></param>
/// <returns></returns>
DIALOG_CONTROL_DPI_CHANGE_BEHAVIORS DcxDPIModule::dcxGetDialogControlDpiChangeBehavior(HWND hWnd) noexcept
{
	if (GetDialogControlDpiChangeBehaviorUx)
		return GetDialogControlDpiChangeBehaviorUx(hWnd);
	SetLastError(ERROR_INVALID_FUNCTION);
	return DCDC_DEFAULT;
}

/// <summary>
/// Get the behaviour of a given dialog when the dpi changes.
/// </summary>
/// <param name="hDlg"></param>
/// <returns></returns>
DIALOG_DPI_CHANGE_BEHAVIORS DcxDPIModule::dcxGetDialogDpiChangeBehavior(HWND hDlg) noexcept
{
	if (GetDialogDpiChangeBehaviorUx)
		return GetDialogDpiChangeBehaviorUx(hDlg);
	SetLastError(ERROR_INVALID_FUNCTION);
	return DDC_DEFAULT;
}

/// <summary>
/// Get the awareness context for a given process.
/// </summary>
/// <param name="hProcess"></param>
/// <returns></returns>
DPI_AWARENESS_CONTEXT DcxDPIModule::dcxGetDpiAwarenessContextForProcess(HANDLE hProcess) noexcept
{
	if (GetDpiAwarenessContextForProcessUx)
		return GetDpiAwarenessContextForProcessUx(hProcess);
	SetLastError(ERROR_INVALID_FUNCTION);
	return nullptr;
}

/// <summary>
/// Get systems DPI
/// </summary>
/// <returns></returns>
UINT DcxDPIModule::dcxGetDpiForSystem() noexcept
{
	if (GetDpiForSystemUx)
		return GetDpiForSystemUx();
	return 96;
}

/// <summary>
/// Get DPI for window
/// </summary>
/// <param name="hwnd"></param>
/// <returns></returns>
UINT DcxDPIModule::dcxGetDpiForWindow(_In_opt_ HWND hwnd) noexcept
{
	if (hwnd && GetDpiForWindowUx)
		return GetDpiForWindowUx(hwnd);
	return dcxGetDpiForSystem();
}

/// <summary>
/// Get the dpi value for a given dpi awareness context.
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
UINT DcxDPIModule::dcxGetDpiFromDpiAwarenessContext(_In_ DPI_AWARENESS_CONTEXT value) noexcept
{
	if (GetDpiFromDpiAwarenessContextUx)
		return GetDpiFromDpiAwarenessContextUx(value);
	SetLastError(ERROR_INVALID_FUNCTION);
	return 0;
}

/// <summary>
/// Get the system dpi setting for a given process.
/// Just returns 96 if the dpi function is not available.
/// </summary>
/// <param name="hProcess"></param>
/// <returns></returns>
UINT DcxDPIModule::dcxGetSystemDpiForProcess(HANDLE hProcess) noexcept
{
	if (GetSystemDpiForProcessUx)
		return GetSystemDpiForProcessUx(hProcess);
	// return default dpi of 100% when no function available.
	return 96;
}

/// <summary>
/// Get system metrics adjusted for specified DPI.
/// Falls back to GetSystemMetrics() if the dpi equiv is not available.
/// </summary>
/// <param name="nIndex"></param>
/// <param name="dpi"></param>
/// <returns></returns>
UINT DcxDPIModule::dcxGetSystemMetricsForDpi(_In_ int nIndex, _In_ UINT dpi) noexcept
{
	if (GetSystemMetricsForDpiUx)
		return GetSystemMetricsForDpiUx(nIndex, dpi);
	return GetSystemMetrics(nIndex);
}

/// <summary>
/// Get the awareness context for this thread.
/// </summary>
/// <returns></returns>
DPI_AWARENESS_CONTEXT DcxDPIModule::dcxGetThreadDpiAwarenessContext() noexcept
{
	if (GetThreadDpiAwarenessContextUx)
		return GetThreadDpiAwarenessContextUx();
	return nullptr;
}

/// <summary>
/// Get the dpi hosting behaviour for this thread.
/// </summary>
/// <returns></returns>
DPI_HOSTING_BEHAVIOR DcxDPIModule::dcxGetThreadDpiHostingBehavior() noexcept
{
	if (GetThreadDpiHostingBehaviorUx)
		return GetThreadDpiHostingBehaviorUx();
	return DPI_HOSTING_BEHAVIOR_INVALID;
}

/// <summary>
/// Get the awareness context for the given window.
/// </summary>
/// <param name="hwnd"></param>
/// <returns></returns>
DPI_AWARENESS_CONTEXT DcxDPIModule::dcxGetWindowDpiAwarenessContext(_In_ HWND hwnd) noexcept
{
	if (GetWindowDpiAwarenessContextUx)
		return GetWindowDpiAwarenessContextUx(hwnd);
	return nullptr;
}

/// <summary>
/// Get the dpi hosting behaviour for the given window.
/// </summary>
/// <param name="hwnd"></param>
/// <returns></returns>
DPI_HOSTING_BEHAVIOR DcxDPIModule::dcxGetWindowDpiHostingBehavior(_In_ HWND hwnd) noexcept
{
	if (GetWindowDpiHostingBehaviorUx)
		return GetWindowDpiHostingBehaviorUx(hwnd);
	return DPI_HOSTING_BEHAVIOR_INVALID;
}

/// <summary>
/// Get the system parameter for a given dpi.
/// </summary>
/// <param name="uiAction">- The system-wide parameter to be retrieved. This function is only intended for use with SPI_GETICONTITLELOGFONT, SPI_GETICONMETRICS, or SPI_GETNONCLIENTMETRICS</param>
/// <param name="uiParam"></param>
/// <param name="pvParam"></param>
/// <param name="fWinIni"></param>
/// <param name="dpi">- The DPI to use for scaling the metric.</param>
/// <returns></returns>
BOOL DcxDPIModule::dcxSystemParametersInfoForDpi(_In_ UINT uiAction, _In_ UINT uiParam, _Inout_ PVOID pvParam, _In_ UINT fWinIni, _In_ UINT dpi) noexcept
{
	if (SystemParametersInfoForDpiUx)
		return SystemParametersInfoForDpiUx(uiAction, uiParam, pvParam, fWinIni, dpi);
	return FALSE;
}

/// <summary>
/// Get the system metrics for a specific window, based on its current dpi.
/// </summary>
/// <param name="hwnd"></param>
/// <param name="nIndex"></param>
/// <returns></returns>
UINT DcxDPIModule::dcxGetWindowMetrics(_In_opt_ HWND hwnd, _In_ int nIndex) noexcept
{
	const auto dpi = dcxGetDpiForWindow(hwnd);
	return dcxGetSystemMetricsForDpi(nIndex, dpi);
}

DPI_AWARENESS DcxDPIModule::dcxGetProcessDpiAwareness() noexcept
{
	return dcxGetAwarenessFromDpiAwarenessContext(dcxGetThreadDpiAwarenessContext());
}

BOOL DcxDPIModule::dcxSetDialogDpiChangeBehavior(HWND hDlg, _In_ DIALOG_DPI_CHANGE_BEHAVIORS mask, _In_ DIALOG_DPI_CHANGE_BEHAVIORS values) noexcept
{
	if (SetDialogDpiChangeBehaviorUx)
		return SetDialogDpiChangeBehaviorUx(hDlg, mask, values);
	return FALSE;
}

BOOL DcxDPIModule::dcxSetProcessDpiAwarenessContext(_In_ DPI_AWARENESS_CONTEXT value) noexcept
{
	if (SetProcessDpiAwarenessContextUx)
		return SetProcessDpiAwarenessContextUx(value);
	return FALSE;
}

UINT DcxDPIModule::dcxSetThreadCursorCreationScaling(_In_ UINT cursorDpi) noexcept
{
	if (SetThreadCursorCreationScalingUx)
		return SetThreadCursorCreationScalingUx(cursorDpi);
	return 96;
}

DPI_AWARENESS_CONTEXT DcxDPIModule::dcxSetThreadDpiAwarenessContext(_In_ DPI_AWARENESS_CONTEXT dpiContext) noexcept
{
	if (SetThreadDpiAwarenessContextUx)
		return SetThreadDpiAwarenessContextUx(dpiContext);
	return nullptr;
}

BOOL DcxDPIModule::dcxInheritWindowMonitor(_In_ HWND hwnd, _In_opt_ HWND hwndInherit) noexcept
{
	if (InheritWindowMonitorUx)
		return InheritWindowMonitorUx(hwnd, hwndInherit);
	return FALSE;
}

BOOL DcxDPIModule::dcxIsValidDpiAwarenessContext(_In_ DPI_AWARENESS_CONTEXT value) noexcept
{
	if (IsValidDpiAwarenessContextUx)
		return IsValidDpiAwarenessContextUx(value);
	return FALSE;
}

BOOL DcxDPIModule::dcxSetDialogControlDpiChangeBehavior(HWND hWnd, _In_ DIALOG_CONTROL_DPI_CHANGE_BEHAVIORS mask, _In_ DIALOG_CONTROL_DPI_CHANGE_BEHAVIORS values) noexcept
{
	if (SetDialogControlDpiChangeBehaviorUx)
		return SetDialogControlDpiChangeBehaviorUx(hWnd, mask, values);
	return FALSE;
}

DPI_HOSTING_BEHAVIOR DcxDPIModule::dcxSetThreadDpiHostingBehavior(_In_ DPI_HOSTING_BEHAVIOR value) noexcept
{
	if (SetThreadDpiHostingBehaviorUx)
		return SetThreadDpiHostingBehaviorUx(value);
	return DPI_HOSTING_BEHAVIOR_INVALID;
}
