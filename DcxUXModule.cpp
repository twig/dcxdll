#include "defines.h"
#include "DcxUXModule.h"
#include "Dcx.h"

#pragma warning(push)
#pragma warning(disable: 26425)	//warning C26425 : Assigning 'nullptr' to a static variable.

// Theme functions
PFNSETTHEME DcxUXModule::SetWindowThemeUx = nullptr;
PFNISTHEMEACTIVE DcxUXModule::IsThemeActiveUx = nullptr;
PFNOPENTHEMEDATA DcxUXModule::OpenThemeDataUx = nullptr;
PFNCLOSETHEMEDATA DcxUXModule::CloseThemeDataUx = nullptr;
PFNDRAWTHEMEBACKGROUND DcxUXModule::DrawThemeBackgroundUx = nullptr;
PFNGETTHEMEBACKGROUNDCONTENTRECT DcxUXModule::GetThemeBackgroundContentRectUx = nullptr;
PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT DcxUXModule::IsThemeBackgroundPartiallyTransparentUx = nullptr;
PFNDRAWTHEMEPARENTBACKGROUND DcxUXModule::DrawThemeParentBackgroundUx = nullptr;
PFNDRAWTHEMETEXT DcxUXModule::DrawThemeTextUx = nullptr;
PFNDRAWTHEMETEXTEX DcxUXModule::DrawThemeTextExUx = nullptr;
PFNGETTHEMEBACKGROUNDREGION DcxUXModule::GetThemeBackgroundRegionUx = nullptr;
PFNGETWINDOWTHEME DcxUXModule::GetWindowThemeUx = nullptr;
PFNDRAWTHEMEEDGE DcxUXModule::DrawThemeEdgeUx = nullptr;
PFNGETTHEMECOLOR DcxUXModule::GetThemeColorUx = nullptr;
PFNGETTHEMEFONT DcxUXModule::GetThemeFontUx = nullptr;
PFNGETTHEMETEXTEXTENT DcxUXModule::GetThemeTextExtentUx = nullptr;
PFNDRAWTHEMEPARENTBACKGROUNDEX DcxUXModule::DrawThemeParentBackgroundExUx = nullptr;
//PFNGETTHEMEBITMAP DcxUXModule::GetThemeBitmapUx = nullptr;
// Vista Function pointers.
PFNBUFFEREDPAINTINIT DcxUXModule::BufferedPaintInitUx = nullptr;
PFNBUFFEREDPAINTUNINIT DcxUXModule::BufferedPaintUnInitUx = nullptr;
PFNBEGINBUFFEREDPAINT DcxUXModule::BeginBufferedPaintUx = nullptr;
PFNENDBUFFEREDPAINT DcxUXModule::EndBufferedPaintUx = nullptr;
PFBUFFEREDPAINTSETALPHA DcxUXModule::BufferedPaintSetAlphaUx = nullptr;
PFBUFFEREDPAINTCLEAR DcxUXModule::BufferedPaintClearUx = nullptr;

bool DcxUXModule::m_bBufferedPaintEnabled = false;

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

		// Get Vista function pointers.
		DrawThemeParentBackgroundExUx = (PFNDRAWTHEMEPARENTBACKGROUNDEX) GetProcAddress(m_hModule, "DrawThemeParentBackgroundEx"); // Vista ONLY!
		//GetThemeBitmapUx = (PFNGETTHEMEBITMAP) GetProcAddress(UXModule, "GetThemeBitmap");
		BufferedPaintInitUx = (PFNBUFFEREDPAINTINIT) GetProcAddress(m_hModule, "BufferedPaintInit");
		BufferedPaintUnInitUx = (PFNBUFFEREDPAINTUNINIT) GetProcAddress(m_hModule, "BufferedPaintUnInit");
		BeginBufferedPaintUx = (PFNBEGINBUFFEREDPAINT) GetProcAddress(m_hModule, "BeginBufferedPaint");
		EndBufferedPaintUx = (PFNENDBUFFEREDPAINT) GetProcAddress(m_hModule, "EndBufferedPaint");
		BufferedPaintSetAlphaUx = (PFBUFFEREDPAINTSETALPHA)GetProcAddress(m_hModule, "BufferedPaintSetAlpha");
		BufferedPaintClearUx = (PFBUFFEREDPAINTCLEAR)GetProcAddress(m_hModule, "BufferedPaintClear");

#pragma warning(pop)

		// NB: DONT count vista functions in XP+ check.
		if (SetWindowThemeUx && IsThemeActiveUx && OpenThemeDataUx && CloseThemeDataUx &&
			DrawThemeBackgroundUx && GetThemeBackgroundContentRectUx && IsThemeBackgroundPartiallyTransparentUx &&
			DrawThemeParentBackgroundUx && DrawThemeTextUx && DrawThemeTextExUx && GetThemeBackgroundRegionUx &&
			GetWindowThemeUx && DrawThemeEdgeUx && GetThemeColorUx && GetThemeFontUx && GetThemeTextExtentUx)
		{
			DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Found XP+ Theme Functions"));
			if (DrawThemeParentBackgroundExUx && BufferedPaintInitUx && BufferedPaintUnInitUx
				&& BeginBufferedPaintUx && EndBufferedPaintUx && BufferedPaintSetAlphaUx && BufferedPaintClearUx)
			{
				DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Found Vista Theme Functions"));
				if (!m_bBufferedPaintEnabled)
					m_bBufferedPaintEnabled = SUCCEEDED(dcxBufferedPaintInit());
			}
		}
		else {
			unload();

			//FreeLibrary(m_hModule);
			//m_hModule = nullptr;
			//// make sure all functions are nullptr
			//SetWindowThemeUx = nullptr;
			//IsThemeActiveUx = nullptr;
			//OpenThemeDataUx = nullptr;
			//CloseThemeDataUx = nullptr;
			//DrawThemeBackgroundUx = nullptr;
			//GetThemeBackgroundContentRectUx = nullptr;
			//IsThemeBackgroundPartiallyTransparentUx = nullptr;
			//DrawThemeParentBackgroundUx = nullptr;
			//DrawThemeTextUx = nullptr;
			//DrawThemeTextExUx = nullptr;
			//GetThemeBackgroundRegionUx = nullptr;
			//GetWindowThemeUx = nullptr;
			//DrawThemeEdgeUx = nullptr;
			//GetThemeColorUx = nullptr;
			//GetThemeFontUx = nullptr;
			//GetThemeTextExtentUx = nullptr;
			//DrawThemeParentBackgroundExUx = nullptr;
			//BufferedPaintInitUx = nullptr;
			//BufferedPaintUnInitUx = nullptr;
			//BeginBufferedPaintUx = nullptr;
			//EndBufferedPaintUx = nullptr;
			//BufferedPaintSetAlphaUx = nullptr;
			//BufferedPaintClearUx = nullptr;

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

		FreeLibrary(m_hModule);
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
HRESULT DcxUXModule::dcxSetWindowTheme(const HWND hwnd, const LPCWSTR pszSubAppName, const LPCWSTR pszSubIdList) noexcept
{
	if (SetWindowThemeUx)
		return SetWindowThemeUx(hwnd, pszSubAppName, pszSubIdList);
	return E_NOTIMPL;
}

[[gsl::suppress(lifetime)]] HTHEME DcxUXModule::dcxGetWindowTheme(HWND hWnd) noexcept
{
	if (GetWindowThemeUx)
		return GetWindowThemeUx(hWnd);
	return nullptr;
}

[[gsl::suppress(lifetime)]] gsl::owner<HTHEME> DcxUXModule::dcxOpenThemeData(HWND hwnd, LPCWSTR pszClassList) noexcept
{
	if (OpenThemeDataUx)
		return OpenThemeDataUx(hwnd, pszClassList);
	return nullptr;
}

#pragma warning(push)
#pragma warning(disable: 26422)
HRESULT DcxUXModule::dcxCloseThemeData(gsl::owner<HTHEME> hTheme) noexcept
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

BOOL DcxUXModule::dcxIsThemeBackgroundPartiallyTransparent(HTHEME hTheme, int iPartId, int iStateId) noexcept
{
	if (IsThemeBackgroundPartiallyTransparentUx)
		return IsThemeBackgroundPartiallyTransparentUx(hTheme, iPartId, iStateId);
	return FALSE;
}

HRESULT DcxUXModule::dcxDrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect) noexcept
{
	if (DrawThemeBackgroundUx)
		return DrawThemeBackgroundUx(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxGetThemeBackgroundContentRect(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect) noexcept
{
	if (GetThemeBackgroundContentRectUx)
		return GetThemeBackgroundContentRectUx(hTheme, hdc, iPartId, iStateId, pBoundingRect, pContentRect);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxDrawThemeParentBackground(HWND hwnd, HDC hdc, const RECT *prc) noexcept
{
	if (DrawThemeParentBackgroundUx)
		return DrawThemeParentBackgroundUx(hwnd, hdc, prc);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxDrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect) noexcept
{
	if (DrawThemeTextUx)
		return DrawThemeTextUx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, dwTextFlags2, pRect);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxDrawThemeTextEx(HTHEME hTheme, HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_z_ LPCWSTR pszText, _In_ int cchText, _In_ DWORD dwTextFlags, _Inout_ LPRECT pRect, _In_ const DTTOPTS* pOptions) noexcept
{
	if (DrawThemeTextExUx)
		return DrawThemeTextExUx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, pRect, pOptions);
	return E_NOTIMPL;
}

[[gsl::suppress(lifetime)]] HRESULT DcxUXModule::dcxGetThemeBackgroundRegion(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HRGN *pRegion) noexcept
{
	if (GetThemeBackgroundRegionUx)
		return GetThemeBackgroundRegionUx(hTheme, hdc, iPartId, iStateId, pRect, pRegion);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxDrawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect) noexcept
{
	if (DrawThemeEdgeUx)
		return DrawThemeEdgeUx(hTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxGetThemeColor(HTHEME hTheme, int iPartId, int iStateId, int iPropId, COLORREF *pColor) noexcept
{
	if (GetThemeColorUx)
		return GetThemeColorUx(hTheme, iPartId, iStateId, iPropId, pColor);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxGetThemeFont(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LPLOGFONT plog) noexcept
{
	if (GetThemeFontUx)
		return GetThemeFontUx(hTheme, hdc, iPartId, iStateId, iPropId, plog);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxGetThemeTextExtent(HTHEME hTheme, HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_z_ LPCWSTR pszText, _In_ int cchCharCount, _In_ DWORD dwTextFlags, _In_ LPCRECT pBoundingRect, _Inout_ LPRECT pExtentRect) noexcept
{
	if (GetThemeTextExtentUx)
		return GetThemeTextExtentUx(hTheme, hdc, iPartId, iStateId, pszText, cchCharCount, dwTextFlags, pBoundingRect, pExtentRect);
	return E_NOTIMPL;
}

HRESULT DcxUXModule::dcxDrawThemeParentBackgroundEx(HWND hwnd, HDC hdc, DWORD dwFlags, const RECT *prc) noexcept
{
	if (DrawThemeParentBackgroundExUx)
		return DrawThemeParentBackgroundExUx(hwnd, hdc, dwFlags, prc);
	return E_NOTIMPL;
}

[[gsl::suppress(lifetime,Enum.3)]] gsl::owner<HPAINTBUFFER> DcxUXModule::dcxBeginBufferedPaint(HDC hdcTarget, const RECT *prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS *pPaintParams, HDC *phdc) noexcept
{
	if (BeginBufferedPaintUx)
		return BeginBufferedPaintUx(hdcTarget, prcTarget, dwFormat, pPaintParams, phdc);
	return nullptr;
}

#pragma warning(push)
#pragma warning(disable: 26422)
HRESULT DcxUXModule::dcxEndBufferedPaint(gsl::owner<HPAINTBUFFER> hBufferedPaint, _In_ BOOL fUpdateTarget) noexcept
{
	if (EndBufferedPaintUx)
		return EndBufferedPaintUx(hBufferedPaint, fUpdateTarget);
	return E_NOTIMPL;
}
#pragma warning(pop)

HRESULT DcxUXModule::dcxBufferedPaintSetAlpha(HPAINTBUFFER hBufferedPaint, _In_ const RECT * prc, _In_ BYTE alpha) noexcept
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

HRESULT DcxUXModule::dcxBufferedPaintClear(HPAINTBUFFER hBufferedPaint, _In_ const RECT* prc) noexcept
{
	if (BufferedPaintClearUx)
		return BufferedPaintClearUx(hBufferedPaint, prc);
	return E_NOTIMPL;
}
