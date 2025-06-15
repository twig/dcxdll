#include "defines.h"
#include "DcxDWMModule.h"
#include <VersionHelpers.h>

#pragma warning(push)
#pragma warning(disable: 26425)	//warning C26425 : Assigning 'nullptr' to a static variable.

DcxDWMModule::~DcxDWMModule(void) noexcept
{
	unload();
}

namespace
{
	bool IsWindows11OrGreater() noexcept
	{
		constexpr WORD wMajorVersion = HIBYTE(_WIN32_WINNT_WINTHRESHOLD), wMinorVersion = LOBYTE(_WIN32_WINNT_WINTHRESHOLD), wBuildMajor = 21996ui16;
		OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
		DWORDLONG        const dwlConditionMask = VerSetConditionMask(
			VerSetConditionMask(
				VerSetConditionMask(
					0, VER_MAJORVERSION, VER_GREATER_EQUAL),
				VER_MINORVERSION, VER_GREATER_EQUAL),
			VER_BUILDNUMBER, VER_GREATER_EQUAL);

		osvi.dwMajorVersion = wMajorVersion;
		osvi.dwMinorVersion = wMinorVersion;
		osvi.dwBuildNumber = wBuildMajor;

		return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, dwlConditionMask) != FALSE;
	}
}

bool DcxDWMModule::load(void)
{
	if (isUseable())
		return false;

	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Loading DWMAPI.DLL..."));
	m_hModule = LoadLibrary(TEXT("dwmapi.dll"));
	if (m_hModule)
	{
		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("DWMAPI.DLL Loaded, Vista+ OS Assumed"));

#pragma warning(push)
#pragma warning(disable: 4191)
#pragma warning(disable: 26493)	//warning C26493 : Don't use C-style casts that would perform a static_cast downcast, const_cast, or reinterpret_cast. (type.4: http://go.microsoft.com/fwlink/p/?LinkID=620420)

		DwmIsCompositionEnabledUx = (PFNDWMISCOMPOSITIONENABLED) GetProcAddress(m_hModule, "DwmIsCompositionEnabled"); // Vista ONLY!
		DwmGetWindowAttributeUx = (PFNDWMGETWINDOWATTRIBUTE) GetProcAddress(m_hModule, "DwmGetWindowAttribute"); // Vista ONLY!
		DwmSetWindowAttributeUx = (PFNDWMSETWINDOWATTRIBUTE) GetProcAddress(m_hModule, "DwmSetWindowAttribute"); // Vista ONLY!
		DwmExtendFrameIntoClientAreaUx = (PFNDWMEXTENDFRAMEINTOCLIENTAREA) GetProcAddress(m_hModule, "DwmExtendFrameIntoClientArea"); // Vista ONLY!
		DwmEnableBlurBehindWindowUx = (PFNDWMENABLEBLURBEHINDWINDOW) GetProcAddress(m_hModule, "DwmEnableBlurBehindWindow"); // Vista ONLY!
		DwmGetColorizationColorUx = (PFNDWMGETCOLORIZATIONCOLOR) GetProcAddress(m_hModule, "DwmGetColorizationColor"); // Vista ONLY!

		DwmRegisterThumbnailUx = reinterpret_cast<decltype(::DwmRegisterThumbnail)*>(GetProcAddress(m_hModule, "DwmRegisterThumbnail"));
		DwmUnregisterThumbnailUx = reinterpret_cast<decltype(::DwmUnregisterThumbnail)*>(GetProcAddress(m_hModule, "DwmUnregisterThumbnail"));
		DwmUpdateThumbnailPropertiesUx = reinterpret_cast<decltype(::DwmUpdateThumbnailProperties)*>(GetProcAddress(m_hModule, "DwmUpdateThumbnailProperties"));
		DwmQueryThumbnailSourceSizeUx = reinterpret_cast<decltype(::DwmQueryThumbnailSourceSize)*>(GetProcAddress(m_hModule, "DwmQueryThumbnailSourceSize"));

#pragma warning(pop)

#if DCX_DEBUG_OUTPUT
		if (DwmIsCompositionEnabledUx)
			mIRCLinker::debug(__FUNCTIONW__, TEXT("Found Vista DWM Functions"));
#endif

		refreshComposite();
	}
	return isUseable();
}

bool DcxDWMModule::unload(void) noexcept
{
	if (isUseable())
	{
		GSL_SUPPRESS(lifetime.1) FreeLibrary(m_hModule);
		m_hModule = nullptr;
		DwmIsCompositionEnabledUx = nullptr;
		DwmGetWindowAttributeUx = nullptr;
		DwmSetWindowAttributeUx = nullptr;
		DwmExtendFrameIntoClientAreaUx = nullptr;
		DwmEnableBlurBehindWindowUx = nullptr;
		DwmGetColorizationColorUx = nullptr;
		DwmRegisterThumbnailUx = nullptr;
		DwmUnregisterThumbnailUx = nullptr;
		DwmUpdateThumbnailPropertiesUx = nullptr;
		DwmQueryThumbnailSourceSizeUx = nullptr;
	}
	return isUseable();
}
#pragma warning(pop)

const bool &DcxDWMModule::refreshComposite() noexcept
{
	BOOL bAero;
	if (FAILED(dcxDwmIsCompositionEnabled(&bAero)))
		bAero = FALSE;

	m_bAero = (bAero != FALSE);
	return m_bAero;
}

HRESULT DcxDWMModule::dcxDwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) noexcept
{
	if (DwmSetWindowAttributeUx != nullptr)
		return DwmSetWindowAttributeUx(hwnd, dwAttribute, pvAttribute, cbAttribute);
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmGetWindowAttribute(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute) noexcept
{
	if (DwmGetWindowAttributeUx != nullptr)
		return DwmGetWindowAttributeUx(hwnd, dwAttribute, pvAttribute, cbAttribute);
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmIsCompositionEnabled(BOOL *pfEnabled) noexcept
{
	if (DwmIsCompositionEnabledUx != nullptr)
		return DwmIsCompositionEnabledUx(pfEnabled);
	if (pfEnabled)
		*pfEnabled = FALSE;
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmExtendFrameIntoClientArea(HWND hwnd, const MARGINS *pMarInset) noexcept
{
	if (DwmExtendFrameIntoClientAreaUx != nullptr)
		return DwmExtendFrameIntoClientAreaUx(hwnd, pMarInset);
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmEnableBlurBehindWindow(HWND hwnd, __in const DWM_BLURBEHIND *pBlurBehind) noexcept
{
	if (DwmEnableBlurBehindWindowUx != nullptr)
		return DwmEnableBlurBehindWindowUx(hwnd, pBlurBehind);
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmGetColorizationColor( __out  DWORD *pcrColorization, __out  BOOL *pfOpaqueBlend) noexcept
{
	if (DwmGetColorizationColorUx != nullptr)
		return DwmGetColorizationColorUx(pcrColorization, pfOpaqueBlend);
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmRegisterThumbnail(HWND hwndDestination, HWND hwndSource, PHTHUMBNAIL phThumbnailId) noexcept
{
	if (DwmRegisterThumbnailUx != nullptr)
		return DwmRegisterThumbnailUx(hwndDestination, hwndSource, phThumbnailId);
	return E_NOTIMPL;
}

HRESULT DcxDWMModule::dcxDwmUnregisterThumbnail(HTHUMBNAIL hThumbnailId) noexcept
{
	if (DwmUnregisterThumbnailUx != nullptr)
		return DwmUnregisterThumbnailUx(hThumbnailId);
	return E_NOTIMPL;
}

HRESULT DcxDWMModule::dcxDwmUpdateThumbnailProperties(HTHUMBNAIL hThumbnailId, const DWM_THUMBNAIL_PROPERTIES* ptnProperties) noexcept
{
	if (DwmUpdateThumbnailPropertiesUx != nullptr)
		return DwmUpdateThumbnailPropertiesUx(hThumbnailId, ptnProperties);
	return E_NOTIMPL;
}

HRESULT DcxDWMModule::dcxDwmQueryThumbnailSourceSize(HTHUMBNAIL hThumbnail, PSIZE pSize) noexcept
{
	if (DwmQueryThumbnailSourceSizeUx != nullptr)
		return DwmQueryThumbnailSourceSizeUx(hThumbnail, pSize);
	return E_NOTIMPL;
}
