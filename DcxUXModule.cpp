#include "defines.h"
#include "DcxUXModule.h"
#include "Dcx.h"

#pragma warning(push)
#pragma warning(disable: 26425)	//warning C26425 : Assigning 'nullptr' to a static variable.

DcxUXModule::~DcxUXModule() noexcept
{
	unload();
}

bool DcxUXModule::load()
{
	if (isUseable())
		return false;

	// UXModule Loading
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Loading UXTHEME.DLL..."));
	m_hModule = LoadLibrary(TEXT("UXTHEME.DLL"));

	if (m_hModule)
	{
		// Get XP+ function pointers.
#pragma warning(push)
#pragma warning(disable: 4191)
#pragma warning(disable: 26493)	//warning C26493 : Don't use C-style casts that would perform a static_cast downcast, const_cast, or reinterpret_cast. (type.4: http://go.microsoft.com/fwlink/p/?LinkID=620420)

		SetWindowThemeUx = (PFNSETTHEME) GetProcAddress(m_hModule, "SetWindowTheme");
		IsThemeActiveUx = (PFNISTHEMEACTIVE) GetProcAddress(m_hModule, "IsThemeActive");
		OpenThemeDataUx = (PFNOPENTHEMEDATA) GetProcAddress(m_hModule, "OpenThemeData");
		CloseThemeDataUx = (PFNCLOSETHEMEDATA) GetProcAddress(m_hModule, "CloseThemeData");
		DrawThemeBackgroundUx = (PFNDRAWTHEMEBACKGROUND) GetProcAddress(m_hModule, "DrawThemeBackground");
		GetThemeBackgroundContentRectUx = (PFNGETTHEMEBACKGROUNDCONTENTRECT) GetProcAddress(m_hModule, "GetThemeBackgroundContentRect");
		IsThemeBackgroundPartiallyTransparentUx = (PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT) GetProcAddress(m_hModule, "IsThemeBackgroundPartiallyTransparent");
		DrawThemeParentBackgroundUx = (PFNDRAWTHEMEPARENTBACKGROUND) GetProcAddress(m_hModule, "DrawThemeParentBackground");
		DrawThemeTextUx = (PFNDRAWTHEMETEXT) GetProcAddress(m_hModule, "DrawThemeText");
		DrawThemeTextExUx = (PFNDRAWTHEMETEXTEX)GetProcAddress(m_hModule, "DrawThemeTextEx");
		GetThemeBackgroundRegionUx = (PFNGETTHEMEBACKGROUNDREGION) GetProcAddress(m_hModule, "GetThemeBackgroundRegion");
		GetWindowThemeUx = (PFNGETWINDOWTHEME) GetProcAddress(m_hModule, "GetWindowTheme");
		DrawThemeEdgeUx = (PFNDRAWTHEMEEDGE) GetProcAddress(m_hModule, "DrawThemeEdge");
		GetThemeColorUx = (PFNGETTHEMECOLOR) GetProcAddress(m_hModule, "GetThemeColor");
		GetThemeIntUx = (PFNGETTHEMEINT)GetProcAddress(m_hModule, "GetThemeInt");
		GetThemeFontUx = (PFNGETTHEMEFONT)GetProcAddress(m_hModule, "GetThemeFont");
		GetThemeTextExtentUx = (PFNGETTHEMETEXTEXTENT)GetProcAddress(m_hModule, "GetThemeTextExtent");
		GetThemeRectUx = (PFNGETTHEMERECT)GetProcAddress(m_hModule, "GetThemeRect");

		// Get Vista function pointers.
		DrawThemeParentBackgroundExUx = (PFNDRAWTHEMEPARENTBACKGROUNDEX) GetProcAddress(m_hModule, "DrawThemeParentBackgroundEx"); // Vista ONLY!
		//GetThemeBitmapUx = (PFNGETTHEMEBITMAP) GetProcAddress(UXModule, "GetThemeBitmap");
		BufferedPaintInitUx = (PFNBUFFEREDPAINTINIT) GetProcAddress(m_hModule, "BufferedPaintInit");
		BufferedPaintUnInitUx = (PFNBUFFEREDPAINTUNINIT) GetProcAddress(m_hModule, "BufferedPaintUnInit");
		BeginBufferedPaintUx = (PFNBEGINBUFFEREDPAINT) GetProcAddress(m_hModule, "BeginBufferedPaint");
		EndBufferedPaintUx = (PFNENDBUFFEREDPAINT) GetProcAddress(m_hModule, "EndBufferedPaint");
		BufferedPaintSetAlphaUx = (PFNBUFFEREDPAINTSETALPHA)GetProcAddress(m_hModule, "BufferedPaintSetAlpha");
		BufferedPaintClearUx = (PFNBUFFEREDPAINTCLEAR)GetProcAddress(m_hModule, "BufferedPaintClear");
		HitTestThemeBackgroundUx = (PFNHITTESTTHEMEBACKGROUND)GetProcAddress(m_hModule, "HitTestThemeBackground");

		if (Dcx::VersInfo.isWin10())
		{
			// Win10+
			OpenThemeDataForDpiUx = reinterpret_cast<decltype(::OpenThemeDataForDpi)*>(::GetProcAddress(m_hModule, "OpenThemeDataForDpi"));
			OpenNcThemeDataUx = reinterpret_cast<PFNOPENNCTHEMEDATA>(GetProcAddress(m_hModule, MAKEINTRESOURCEA(49)));
			RefreshImmersiveColorPolicyStateUx = reinterpret_cast<PFNREFRESHIMMERSIVECOLORPOLICYSTATE>(GetProcAddress(m_hModule, MAKEINTRESOURCEA(104)));
			GetIsImmersiveColorUsingHighContrastUx = reinterpret_cast<PFNGETISIMMERSIVECOLORUSINGHIGHCONTRAST>(GetProcAddress(m_hModule, MAKEINTRESOURCEA(106)));
			ShouldAppsUseDarkModeUx = reinterpret_cast<PFNSHOULDAPPSUSEDARKMODE>(GetProcAddress(m_hModule, MAKEINTRESOURCEA(132)));
			AllowDarkModeForWindowUx = reinterpret_cast<PFNALLOWDARKMODEFORWINDOW>(GetProcAddress(m_hModule, MAKEINTRESOURCEA(133)));
			IsDarkModeAllowedForWindowUx = reinterpret_cast<PFNISDARKMODEALLOWEDFORWINDOW>(GetProcAddress(m_hModule, MAKEINTRESOURCEA(137)));

			//auto ord135 = GetProcAddress(m_hModule, MAKEINTRESOURCEA(135));
			//if (dcxGetWindows10Build() < 18362)
			//	AllowDarkModeForAppUx = reinterpret_cast<PFNALLOWDARKMODEFORAPP>(ord135);
			//else
			//	SetPreferredAppModeUx = reinterpret_cast<PFNSETPREFERREDAPPMODE>(ord135);

			auto ord135 = GetProcAddress(m_hModule, MAKEINTRESOURCEA(135));
			if (Dcx::VersInfo.isWin10Build18362())
				SetPreferredAppModeUx = reinterpret_cast<PFNSETPREFERREDAPPMODE>(ord135);
			else
				AllowDarkModeForAppUx = reinterpret_cast<PFNALLOWDARKMODEFORAPP>(ord135);
		}
#pragma warning(pop)

		// NB: DONT count vista functions in XP+ check.
		if (SetWindowThemeUx && IsThemeActiveUx && OpenThemeDataUx && CloseThemeDataUx &&
			DrawThemeBackgroundUx && GetThemeBackgroundContentRectUx && IsThemeBackgroundPartiallyTransparentUx &&
			DrawThemeParentBackgroundUx && DrawThemeTextUx && DrawThemeTextExUx && GetThemeBackgroundRegionUx &&
			GetWindowThemeUx && DrawThemeEdgeUx && GetThemeColorUx && GetThemeFontUx && GetThemeTextExtentUx && GetThemeRectUx)
		{
			DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Found XP+ Theme Functions"));
			if (DrawThemeParentBackgroundExUx && BufferedPaintInitUx && BufferedPaintUnInitUx
				&& BeginBufferedPaintUx && EndBufferedPaintUx && BufferedPaintSetAlphaUx && BufferedPaintClearUx && HitTestThemeBackgroundUx)
			{
				DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Found Vista Theme Functions"));
				if (!m_bBufferedPaintEnabled)
					m_bBufferedPaintEnabled = SUCCEEDED(dcxBufferedPaintInit());
#ifdef DCX_DEBUG_OUTPUT
				if (OpenThemeDataForDpiUx)
				{
					DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Found Win10+ Theme Functions"));
					if (OpenNcThemeDataUx && RefreshImmersiveColorPolicyStateUx &&
						GetIsImmersiveColorUsingHighContrastUx && ShouldAppsUseDarkModeUx &&
						AllowDarkModeForWindowUx && IsDarkModeAllowedForWindowUx)
					{
						DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Found Win10 undocumented Theme Functions"));
						m_bDarkModeSupported = true;

						//dcxAllowDarkModeForApp(true);
						//dcxRefreshImmersiveColorPolicyState();

						//m_bDarkModeEnabled = dcxShouldAppsUseDarkMode() && !dcxIsHighContrast();

						//dcxFixDarkScrollBar();

					}
				}
#endif
			}
		}
		else {
			unload();

			throw Dcx::dcxException("There was a problem loading Theme Library");
		}
	}
	return isUseable();
}

bool DcxUXModule::unload() noexcept
{
	if (isUseable())
	{
		if (m_bBufferedPaintEnabled)
			dcxBufferedPaintUnInit();
		m_bBufferedPaintEnabled = false;

		GSL_SUPPRESS(lifetime.1) FreeLibrary(m_hModule);
		m_hModule = nullptr;
		// make sure all functions are nullptr
		SetWindowThemeUx = nullptr;
		IsThemeActiveUx = nullptr;
		OpenThemeDataUx = nullptr;
		CloseThemeDataUx = nullptr;
		DrawThemeBackgroundUx = nullptr;
		GetThemeBackgroundContentRectUx = nullptr;
		IsThemeBackgroundPartiallyTransparentUx = nullptr;
		DrawThemeParentBackgroundUx = nullptr;
		DrawThemeTextUx = nullptr;
		DrawThemeTextExUx = nullptr;
		GetThemeBackgroundRegionUx = nullptr;
		GetWindowThemeUx = nullptr;
		DrawThemeEdgeUx = nullptr;
		GetThemeColorUx = nullptr;
		GetThemeFontUx = nullptr;
		GetThemeTextExtentUx = nullptr;
		DrawThemeParentBackgroundExUx = nullptr;
		BufferedPaintInitUx = nullptr;
		BufferedPaintUnInitUx = nullptr;
		BeginBufferedPaintUx = nullptr;
		EndBufferedPaintUx = nullptr;
		BufferedPaintSetAlphaUx = nullptr;
		BufferedPaintClearUx = nullptr;
		HitTestThemeBackgroundUx = nullptr;
		OpenThemeDataForDpiUx = nullptr;
		OpenNcThemeDataUx = nullptr;
		RefreshImmersiveColorPolicyStateUx = nullptr;
		GetIsImmersiveColorUsingHighContrastUx = nullptr;
		ShouldAppsUseDarkModeUx = nullptr;
		AllowDarkModeForWindowUx = nullptr;
		IsDarkModeAllowedForWindowUx = nullptr;
		AllowDarkModeForAppUx = nullptr;
		SetPreferredAppModeUx = nullptr;
	}
	return isUseable();
}
#pragma warning(pop)

/*!
* \brief Check if theme is active
*
*
*/
BOOL DcxUXModule::dcxIsThemeActive() noexcept
{
	if (IsThemeActiveUx)
		return IsThemeActiveUx();
	return FALSE;
}
/*!
* \brief Windows Theme Setting Function
*
* Used to remove theme on controls
*/
HRESULT DcxUXModule::dcxSetWindowTheme(_In_ const HWND hwnd, _In_opt_ const LPCWSTR pszSubAppName, _In_opt_ const LPCWSTR pszSubIdList) noexcept
{
	if (SetWindowThemeUx)
		return SetWindowThemeUx(hwnd, pszSubAppName, pszSubIdList);
	return E_NOTIMPL;
}

[[gsl::suppress(lifetime)]] HTHEME DcxUXModule::dcxGetWindowTheme(_In_ HWND hWnd) noexcept
{
	if (GetWindowThemeUx)
		return GetWindowThemeUx(hWnd);
	return nullptr;
}

[[gsl::suppress(lifetime)]] gsl::owner<HTHEME> DcxUXModule::dcxOpenThemeData(_In_opt_ HWND hwnd, _In_ LPCWSTR pszClassList) noexcept
{
	if (OpenThemeDataUx)
		return OpenThemeDataUx(hwnd, pszClassList);
	return nullptr;
}

#pragma warning(push)
#pragma warning(disable: 26422)
HRESULT DcxUXModule::dcxCloseThemeData(_In_ gsl::owner<HTHEME> hTheme) noexcept
{
	if (CloseThemeDataUx)
		return CloseThemeDataUx(hTheme);
	return E_NOTIMPL;
}
#pragma warning(pop)

//int DcxUXModule::dcxGetThemeBackgroundRegion(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPRECT pRect, HRGN *pRegion)
//{
//	if (GetThemeBackgroundRegionUx)
//		return GetThemeBackgroundRegionUx(hTheme, hdc, iPartId, iStateId, pRect, pRegion);
//	return nullptr;
//}

BOOL DcxUXModule::dcxIsThemeBackgroundPartiallyTransparent(_In_ HTHEME hTheme, _In_ int iPartId, _In_ int iStateId) noexcept
{
	if (IsThemeBackgroundPartiallyTransparentUx)
		return IsThemeBackgroundPartiallyTransparentUx(hTheme, iPartId, iStateId);
	return FALSE;
}

HRESULT DcxUXModule::dcxDrawThemeBackground(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ LPCRECT pRect, _In_opt_ LPCRECT pClipRect) noexcept
{
	if (DrawThemeBackgroundUx)
		return DrawThemeBackgroundUx(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxGetThemeBackgroundContentRect(_In_ HTHEME hTheme, _In_opt_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ LPCRECT pBoundingRect, _Out_ LPRECT pContentRect) noexcept
{
	if (GetThemeBackgroundContentRectUx)
		return GetThemeBackgroundContentRectUx(hTheme, hdc, iPartId, iStateId, pBoundingRect, pContentRect);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxDrawThemeParentBackground(_In_ HWND hwnd, _In_ HDC hdc, _In_opt_ LPCRECT prc) noexcept
{
	if (DrawThemeParentBackgroundUx)
		return DrawThemeParentBackgroundUx(hwnd, hdc, prc);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxDrawThemeText(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_reads_(cchText) LPCWSTR pszText, _In_ int cchText, _In_ DWORD dwTextFlags, _In_ DWORD dwTextFlags2, _In_ LPCRECT pRect) noexcept
{
	if (DrawThemeTextUx)
		return DrawThemeTextUx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, dwTextFlags2, pRect);

	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxDrawThemeTextEx(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_reads_(cchText) LPCWSTR pszText, _In_ int cchText, _In_ DWORD dwTextFlags, _Inout_ LPRECT pRect, _In_opt_ const DTTOPTS* pOptions) noexcept
{
	if (DrawThemeTextExUx)
		return DrawThemeTextExUx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, pRect, pOptions);

	return E_NOTIMPL;
}

[[gsl::suppress(lifetime)]] HRESULT DcxUXModule::dcxGetThemeBackgroundRegion(_In_ HTHEME hTheme, _In_opt_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ LPCRECT pRect, _Out_ HRGN *pRegion) noexcept
{
	if (GetThemeBackgroundRegionUx)
		return GetThemeBackgroundRegionUx(hTheme, hdc, iPartId, iStateId, pRect, pRegion);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxDrawThemeEdge(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ LPCRECT pDestRect, _In_ UINT uEdge, _In_ UINT uFlags, _Out_opt_ LPRECT pContentRect) noexcept
{
	if (DrawThemeEdgeUx)
		return DrawThemeEdgeUx(hTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxGetThemeColor(_In_ HTHEME hTheme, _In_ int iPartId, _In_ int iStateId, _In_ int iPropId, _Out_ COLORREF *pColor) noexcept
{
	if (GetThemeColorUx)
		return GetThemeColorUx(hTheme, iPartId, iStateId, iPropId, pColor);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxGetThemeInt(_In_ HTHEME hTheme, _In_ int iPartId, _In_ int iStateId, _In_ int iPropId, _Out_ int* piVal) noexcept
{
	if (GetThemeIntUx)
		return GetThemeIntUx(hTheme, iPartId, iStateId, iPropId, piVal);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxGetThemeFont(_In_ HTHEME hTheme, _In_opt_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ int iPropId, _Out_ LPLOGFONT plog) noexcept
{
	if (GetThemeFontUx)
		return GetThemeFontUx(hTheme, hdc, iPartId, iStateId, iPropId, plog);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxGetThemeTextExtent(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_reads_(cchCharCount) LPCWSTR pszText, _In_ int cchCharCount, _In_ DWORD dwTextFlags, _In_opt_ LPCRECT pBoundingRect, _Out_ LPRECT pExtentRect) noexcept
{
	if (GetThemeTextExtentUx)
		return GetThemeTextExtentUx(hTheme, hdc, iPartId, iStateId, pszText, cchCharCount, dwTextFlags, pBoundingRect, pExtentRect);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxDrawThemeParentBackgroundEx(_In_ HWND hwnd, _In_ HDC hdc, _In_ DWORD dwFlags, _In_opt_ LPCRECT prc) noexcept
{
	if (DrawThemeParentBackgroundExUx)
		return DrawThemeParentBackgroundExUx(hwnd, hdc, dwFlags, prc);
	return E_NOTIMPL;
}

[[gsl::suppress(lifetime,Enum.3)]] _Success_(return != NULL) gsl::owner<HPAINTBUFFER> DcxUXModule::dcxBeginBufferedPaint(_In_ HDC hdcTarget, _In_ LPCRECT prcTarget, _In_ BP_BUFFERFORMAT dwFormat, _In_opt_ BP_PAINTPARAMS *pPaintParams, _Out_ HDC *phdc) noexcept
{
	if (BeginBufferedPaintUx)
		return BeginBufferedPaintUx(hdcTarget, prcTarget, dwFormat, pPaintParams, phdc);
	return nullptr;
}

#pragma warning(push)
#pragma warning(disable: 26422)
HRESULT DcxUXModule::dcxEndBufferedPaint(_In_ gsl::owner<HPAINTBUFFER> hBufferedPaint, _In_ BOOL fUpdateTarget) noexcept
{
	if (EndBufferedPaintUx)
		return EndBufferedPaintUx(hBufferedPaint, fUpdateTarget);
	return E_NOTIMPL;
}
#pragma warning(pop)

HRESULT DcxUXModule::dcxBufferedPaintSetAlpha(_In_ HPAINTBUFFER hBufferedPaint, _In_opt_ LPCRECT prc, _In_ BYTE alpha) noexcept
{
	if (BufferedPaintSetAlphaUx)
		return BufferedPaintSetAlphaUx(hBufferedPaint, prc, alpha);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxBufferedPaintInit(void) noexcept
{
	if (BufferedPaintInitUx)
		return BufferedPaintInitUx();
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxBufferedPaintUnInit(void) noexcept
{
	if (BufferedPaintUnInitUx)
		return BufferedPaintUnInitUx();
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxBufferedPaintClear(_In_ HPAINTBUFFER hBufferedPaint, _In_opt_ LPCRECT prc) noexcept
{
	if (BufferedPaintClearUx)
		return BufferedPaintClearUx(hBufferedPaint, prc);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxHitTestThemeBackground(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ DWORD dwOptions, _In_ LPCRECT pRect, _In_ HRGN hrgn, _In_ POINT ptTest, _Out_ WORD* pwHitTestCode) noexcept
{
	if (HitTestThemeBackgroundUx)
		return HitTestThemeBackgroundUx(hTheme, hdc, iPartId, iStateId, dwOptions, pRect, hrgn, ptTest, pwHitTestCode);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxGetThemeRect(_In_ HTHEME hTheme, _In_ int iPartId, _In_ int iStateId, _In_ int iPropId, _Out_ LPRECT pRect) noexcept
{
	if (GetThemeRectUx)
		return GetThemeRectUx(hTheme, iPartId, iStateId, iPropId, pRect);
	return E_NOTIMPL;
}

HTHEME DcxUXModule::dcxOpenThemeDataForDpi(_In_opt_ HWND hwnd, _In_ LPCWSTR pszClassList, _In_ UINT dpi) noexcept
{
	if (OpenThemeDataForDpiUx)
		return OpenThemeDataForDpiUx(hwnd, pszClassList, dpi);
	return nullptr;
}

bool DcxUXModule::dcxShouldAppsUseDarkMode() noexcept
{
	if (ShouldAppsUseDarkModeUx)
		return ShouldAppsUseDarkModeUx();
	return false;
}

bool DcxUXModule::dcxAllowDarkModeForWindow(HWND hWnd, bool allow) noexcept
{
	if (AllowDarkModeForWindowUx)
		return AllowDarkModeForWindowUx(hWnd, allow);
	return false;
}

void DcxUXModule::dcxRefreshImmersiveColorPolicyState() noexcept
{
	if (RefreshImmersiveColorPolicyStateUx)
		RefreshImmersiveColorPolicyStateUx();
}

bool DcxUXModule::dcxIsDarkModeAllowedForWindow(HWND hWnd) noexcept
{
	if (IsDarkModeAllowedForWindowUx)
		return IsDarkModeAllowedForWindowUx(hWnd);
	return false;
}

bool DcxUXModule::dcxGetIsImmersiveColorUsingHighContrast(IMMERSIVE_HC_CACHE_MODE mode) noexcept
{
	if (GetIsImmersiveColorUsingHighContrastUx)
		return GetIsImmersiveColorUsingHighContrastUx(mode);
	return false;
}

HTHEME DcxUXModule::dcxOpenNcThemeData(HWND hWnd, LPCWSTR pszClassList) noexcept
{
	if (OpenNcThemeDataUx)
		return OpenNcThemeDataUx(hWnd, pszClassList);
	return nullptr;
}

bool DcxUXModule::dcxShouldSystemUseDarkMode() noexcept
{
	if (ShouldSystemUseDarkModeUx)
		return ShouldSystemUseDarkModeUx();
	return false;
}

PreferredAppMode DcxUXModule::dcxSetPreferredAppMode(PreferredAppMode appMode) noexcept
{
	if (SetPreferredAppModeUx)
		return SetPreferredAppModeUx(appMode);
	return PreferredAppMode::Default;
}

bool DcxUXModule::dcxIsDarkModeAllowedForApp() noexcept
{
	if (IsDarkModeAllowedForAppUx)
		return IsDarkModeAllowedForAppUx();
	return false;
}

DWORD DcxUXModule::dcxGetWindows10Build() noexcept
{
	if (m_dwBuildNumber > 0)
		return m_dwBuildNumber;

	auto hNTModule = GetModuleHandleW(L"ntdll.dll");
	if (!hNTModule)
		return 0;

	using pfnRtlGetNtVersionNumbers = void (WINAPI*)(LPDWORD major, LPDWORD minor, LPDWORD build);

	auto RtlGetNtVersionNumbers = reinterpret_cast<pfnRtlGetNtVersionNumbers>(GetProcAddress(hNTModule, "RtlGetNtVersionNumbers"));
	if (RtlGetNtVersionNumbers)
	{
		DWORD major{}, minor{};
		RtlGetNtVersionNumbers(&major, &minor, &m_dwBuildNumber);
		m_dwBuildNumber &= ~0xF0000000;
		if (major == 10 && minor == 0)
			return m_dwBuildNumber;
	}
	return 0;
}

void DcxUXModule::dcxAllowDarkModeForApp(_In_ bool allow) noexcept
{
	if (AllowDarkModeForAppUx)
		AllowDarkModeForAppUx(allow);
	else if (SetPreferredAppModeUx)
		SetPreferredAppModeUx(allow ? PreferredAppMode::AllowDark : PreferredAppMode::Default);
}

bool DcxUXModule::dcxIsHighContrast() noexcept
{
	HIGHCONTRASTW highContrast = { sizeof(highContrast) };
	if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, FALSE))
		return highContrast.dwFlags & HCF_HIGHCONTRASTON;
	return false;
}

void DcxUXModule::dcxRefreshTitleBarThemeColor(_In_opt_ HWND mHwnd) noexcept
{
	BOOL dark = FALSE;
	if (DcxUXModule::dcxIsDarkModeAllowedForWindow(mHwnd) &&
		DcxUXModule::dcxShouldAppsUseDarkMode() &&
		!DcxUXModule::dcxIsHighContrast())
	{
		dark = TRUE;
	}
	Dcx::DwmModule.dcxDwmSetWindowAttribute(mHwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
}

int DcxUXModule::dcxGetTextGlowSize() noexcept
{
	int iGlow = 12; // Default value
	// CompositedWindow::Window is declared in AeroStyle.xml
	if (HTHEME hThemeWindow = dcxOpenThemeData(nullptr, L"CompositedWindow::Window"); hThemeWindow)
	{
		dcxGetThemeInt(hThemeWindow, 0, 0, TMT_TEXTGLOWSIZE, &iGlow);
		dcxCloseThemeData(hThemeWindow);
	}
	return iGlow;
}

COLORREF DcxUXModule::dcxGetTextGlowColor() noexcept
{
	COLORREF crGlow = RGB(255,255,255); // Default value
	// CompositedWindow::Window is declared in AeroStyle.xml
	if (HTHEME hThemeWindow = dcxOpenThemeData(nullptr, L"CompositedWindow::Window"); hThemeWindow)
	{
		dcxGetThemeColor(hThemeWindow, 0, 0, TMT_GLOWCOLOR, &crGlow);
		dcxCloseThemeData(hThemeWindow);
	}
	return crGlow;
}
