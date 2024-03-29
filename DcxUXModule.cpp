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

		// Win10+
		OpenThemeDataForDpiUx = reinterpret_cast<decltype(::OpenThemeDataForDpi)*>(::GetProcAddress(m_hModule, "OpenThemeDataForDpi"));

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

[[gsl::suppress(lifetime)]] gsl::owner<HTHEME> DcxUXModule::dcxOpenThemeData(_In_ HWND hwnd, _In_ LPCWSTR pszClassList) noexcept
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

HTHEME DcxUXModule::dcxOpenThemeDataForDpi(HWND hwnd, LPCWSTR pszClassList, UINT dpi) noexcept
{
	if (OpenThemeDataForDpiUx)
		return OpenThemeDataForDpiUx(hwnd, pszClassList, dpi);
	return nullptr;
}
