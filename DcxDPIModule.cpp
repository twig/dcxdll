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

		GetDpiForSystemUx = reinterpret_cast<decltype(::GetDpiForSystem)*>(GetProcAddress(m_hModule, "GetDpiForSystem"));
		GetDpiForWindowUx = reinterpret_cast<decltype(::GetDpiForWindow)*>(GetProcAddress(m_hModule, "GetDpiForWindow"));
		GetSystemMetricsForDpiUx = reinterpret_cast<decltype(::GetSystemMetricsForDpi)*>(GetProcAddress(m_hModule, "GetSystemMetricsForDpi"));
		SetProcessDpiAwarenessContextUx = reinterpret_cast<decltype(::SetProcessDpiAwarenessContext)*>(::GetProcAddress(m_hModule, "SetProcessDpiAwarenessContext"));
		InheritWindowMonitorUx = reinterpret_cast<decltype(::InheritWindowMonitor)*>(::GetProcAddress(m_hModule, "InheritWindowMonitor"));
		SetThreadDpiHostingBehaviorUx = reinterpret_cast<decltype(::SetThreadDpiHostingBehavior)*>(::GetProcAddress(m_hModule, "SetThreadDpiHostingBehavior"));

#pragma warning(pop)

		if (GetDpiForSystemUx && GetDpiForWindowUx && GetSystemMetricsForDpiUx && InheritWindowMonitorUx && SetThreadDpiHostingBehaviorUx)
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
	GetDpiForSystemUx = nullptr;
	GetDpiForWindowUx = nullptr;
	GetSystemMetricsForDpiUx = nullptr;
	SetProcessDpiAwarenessContextUx = nullptr;
	InheritWindowMonitorUx = nullptr;
	SetThreadDpiHostingBehaviorUx = nullptr;

	return isUseable();
}
#pragma warning(pop)

/*!
* \brief Get systems DPI
*
*
*/
UINT DcxDPIModule::dcxGetDpiForSystem() noexcept
{
	if (GetDpiForSystemUx)
		return GetDpiForSystemUx();
	return 96;
}

/*!
* \brief Get DPI for window
*
*
*/
UINT DcxDPIModule::dcxGetDpiForWindow(_In_ _Maybenull_ HWND hwnd) noexcept
{
	if (hwnd && GetDpiForWindowUx)
		return GetDpiForWindowUx(hwnd);
	return dcxGetDpiForSystem();
}

/*!
* \brief Get system metrics adjusted for specified DPI
*
*
*/
UINT DcxDPIModule::dcxGetSystemMetricsForDpi(_In_ int nIndex, _In_ UINT dpi) noexcept
{
	if (GetSystemMetricsForDpiUx)
		return GetSystemMetricsForDpiUx(nIndex, dpi);
	return GetSystemMetrics(nIndex);
}

/// <summary>
/// Get the system metrics for a specific window, based on its current dpi.
/// </summary>
/// <param name="hwnd"></param>
/// <param name="nIndex"></param>
/// <returns></returns>
UINT DcxDPIModule::dcxGetWindowMetrics(_In_ _Maybenull_ HWND hwnd, _In_ int nIndex) noexcept
{
	const auto dpi = dcxGetDpiForWindow(hwnd);
	return dcxGetSystemMetricsForDpi(nIndex, dpi);
}

BOOL DcxDPIModule::dcxSetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT value) noexcept
{
	if (SetProcessDpiAwarenessContextUx)
		return SetProcessDpiAwarenessContextUx(value);
	return FALSE;
}

BOOL DcxDPIModule::dcxInheritWindowMonitor(HWND hwnd, HWND hwndInherit) noexcept
{
	if (InheritWindowMonitorUx)
		return InheritWindowMonitorUx(hwnd, hwndInherit);
	return FALSE;
}

DPI_HOSTING_BEHAVIOR DcxDPIModule::dcxSetThreadDpiHostingBehavior(DPI_HOSTING_BEHAVIOR value) noexcept
{
	if (SetThreadDpiHostingBehaviorUx)
		return SetThreadDpiHostingBehaviorUx(value);
	return DPI_HOSTING_BEHAVIOR_INVALID;
}
