#include "defines.h"
#include "DcxUXModule.h"
#include "Dcx.h"

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
PFNGETTHEMEBACKGROUNDREGION DcxUXModule::GetThemeBackgroundRegionUx = nullptr;
PFNGETWINDOWTHEME DcxUXModule::GetWindowThemeUx = nullptr;
PFNDRAWTHEMEEDGE DcxUXModule::DrawThemeEdgeUx = nullptr;
PFNGETTHEMECOLOR DcxUXModule::GetThemeColorUx = nullptr;
PFNDRAWTHEMEPARENTBACKGROUNDEX DcxUXModule::DrawThemeParentBackgroundExUx = nullptr;
//PFNGETTHEMEBITMAP DcxUXModule::GetThemeBitmapUx = nullptr;
// Vista Function pointers.
PFNBUFFEREDPAINTINIT DcxUXModule::BufferedPaintInitUx = nullptr;
PFNBUFFEREDPAINTUNINIT DcxUXModule::BufferedPaintUnInitUx = nullptr;
PFNBEGINBUFFEREDPAINT DcxUXModule::BeginBufferedPaintUx = nullptr;
PFNENDBUFFEREDPAINT DcxUXModule::EndBufferedPaintUx = nullptr;
bool DcxUXModule::m_bBufferedPaintEnabled = false;

DcxUXModule::~DcxUXModule(void)
{
	unload();
}

bool DcxUXModule::load(void)
{
	if (isUseable())
		return false;

	// UXModule Loading
	DCX_DEBUG(mIRCLinker::debug,TEXT("LoadDLL"), TEXT("Loading UXTHEME.DLL..."));
	m_hModule = LoadLibrary(TEXT("UXTHEME.DLL"));

	if (m_hModule != nullptr) {
		// Get XP+ function pointers.
#pragma warning(push)
#pragma warning(disable: 4191)

		SetWindowThemeUx = (PFNSETTHEME) GetProcAddress(m_hModule, "SetWindowTheme");
		IsThemeActiveUx = (PFNISTHEMEACTIVE) GetProcAddress(m_hModule, "IsThemeActive");
		OpenThemeDataUx = (PFNOPENTHEMEDATA) GetProcAddress(m_hModule, "OpenThemeData");
		CloseThemeDataUx = (PFNCLOSETHEMEDATA) GetProcAddress(m_hModule, "CloseThemeData");
		DrawThemeBackgroundUx = (PFNDRAWTHEMEBACKGROUND) GetProcAddress(m_hModule, "DrawThemeBackground");
		GetThemeBackgroundContentRectUx = (PFNGETTHEMEBACKGROUNDCONTENTRECT) GetProcAddress(m_hModule, "GetThemeBackgroundContentRect");
		IsThemeBackgroundPartiallyTransparentUx = (PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT) GetProcAddress(m_hModule, "IsThemeBackgroundPartiallyTransparent");
		DrawThemeParentBackgroundUx = (PFNDRAWTHEMEPARENTBACKGROUND) GetProcAddress(m_hModule, "DrawThemeParentBackground");
		DrawThemeTextUx = (PFNDRAWTHEMETEXT) GetProcAddress(m_hModule, "DrawThemeText");
		GetThemeBackgroundRegionUx = (PFNGETTHEMEBACKGROUNDREGION) GetProcAddress(m_hModule, "GetThemeBackgroundRegion");
		GetWindowThemeUx = (PFNGETWINDOWTHEME) GetProcAddress(m_hModule, "GetWindowTheme");
		DrawThemeEdgeUx = (PFNDRAWTHEMEEDGE) GetProcAddress(m_hModule, "DrawThemeEdge");
		GetThemeColorUx = (PFNGETTHEMECOLOR) GetProcAddress(m_hModule, "GetThemeColor");

		// Get Vista function pointers.
		DrawThemeParentBackgroundExUx = (PFNDRAWTHEMEPARENTBACKGROUNDEX) GetProcAddress(m_hModule, "DrawThemeParentBackgroundEx"); // Vista ONLY!
		//GetThemeBitmapUx = (PFNGETTHEMEBITMAP) GetProcAddress(UXModule, "GetThemeBitmap");
		BufferedPaintInitUx = (PFNBUFFEREDPAINTINIT) GetProcAddress(m_hModule, "BufferedPaintInit");
		BufferedPaintUnInitUx = (PFNBUFFEREDPAINTUNINIT) GetProcAddress(m_hModule, "BufferedPaintUnInit");
		BeginBufferedPaintUx = (PFNBEGINBUFFEREDPAINT) GetProcAddress(m_hModule, "BeginBufferedPaint");
		EndBufferedPaintUx = (PFNENDBUFFEREDPAINT) GetProcAddress(m_hModule, "EndBufferedPaint");

#pragma warning(pop)

		// NB: DONT count vista functions in XP+ check.
		if (SetWindowThemeUx && IsThemeActiveUx && OpenThemeDataUx && CloseThemeDataUx &&
			DrawThemeBackgroundUx && GetThemeBackgroundContentRectUx && IsThemeBackgroundPartiallyTransparentUx &&
			DrawThemeParentBackgroundUx && DrawThemeTextUx && GetThemeBackgroundRegionUx && GetWindowThemeUx && DrawThemeEdgeUx && GetThemeColorUx)
		{
			DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Found XP+ Theme Functions"));
			if (DrawThemeParentBackgroundExUx && BufferedPaintInitUx && BufferedPaintUnInitUx && BeginBufferedPaintUx && EndBufferedPaintUx)
			{
				DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Found Vista Theme Functions"));
				if (!m_bBufferedPaintEnabled)
					m_bBufferedPaintEnabled = SUCCEEDED(dcxBufferedPaintInit());
			}
		}
		else {
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
			GetThemeBackgroundRegionUx = nullptr;
			GetWindowThemeUx = nullptr;
			DrawThemeEdgeUx = nullptr;
			GetThemeColorUx = nullptr;
			DrawThemeParentBackgroundExUx = nullptr;
			BufferedPaintInitUx = nullptr;
			BufferedPaintUnInitUx = nullptr;
			BeginBufferedPaintUx = nullptr;
			EndBufferedPaintUx = nullptr;

			throw Dcx::dcxException("There was a problem loading Theme Library");
		}
	}
	return isUseable();
}

bool DcxUXModule::unload(void)
{
	if (isUseable()) {
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
		GetThemeBackgroundRegionUx = nullptr;
		GetWindowThemeUx = nullptr;
		DrawThemeEdgeUx = nullptr;
		GetThemeColorUx = nullptr;
		DrawThemeParentBackgroundExUx = nullptr;
		BufferedPaintInitUx = nullptr;
		BufferedPaintUnInitUx = nullptr;
		BeginBufferedPaintUx = nullptr;
		EndBufferedPaintUx = nullptr;
	}
	return isUseable();
}

/*!
* \brief Check if theme is active
*
*
*/
BOOL DcxUXModule::dcxIsThemeActive(void)
{
	if (IsThemeActiveUx != nullptr)
		return IsThemeActiveUx();
	return FALSE;
}
/*!
* \brief Windows Theme Setting Function
*
* Used to remove theme on controls
*/
HRESULT DcxUXModule::dcxSetWindowTheme(const HWND hwnd, const LPCWSTR pszSubAppName, const LPCWSTR pszSubIdList) {
	if (SetWindowThemeUx != nullptr)
		return SetWindowThemeUx(hwnd, pszSubAppName, pszSubIdList);
	return 0;
}

HTHEME DcxUXModule::dcxGetWindowTheme(HWND hWnd)
{
	if (GetWindowThemeUx != nullptr)
		return GetWindowThemeUx(hWnd);
	return nullptr;
}

gsl::owner<HTHEME> DcxUXModule::dcxOpenThemeData(HWND hwnd, LPCWSTR pszClassList)
{
	if (OpenThemeDataUx != nullptr)
		return OpenThemeDataUx(hwnd, pszClassList);
	return nullptr;
}

HRESULT DcxUXModule::dcxCloseThemeData(gsl::owner<HTHEME> hTheme)
{
	if (CloseThemeDataUx != nullptr)
		return CloseThemeDataUx(hTheme);
	return NULL;
}

//int DcxUXModule::dcxGetThemeBackgroundRegion(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPRECT pRect, HRGN *pRegion)
//{
//	if (GetThemeBackgroundRegionUx != nullptr)
//		return GetThemeBackgroundRegionUx(hTheme, hdc, iPartId, iStateId, pRect, pRegion);
//	return nullptr;
//}

BOOL DcxUXModule::dcxIsThemeBackgroundPartiallyTransparent(HTHEME hTheme, int iPartId, int iStateId)
{
	if (IsThemeBackgroundPartiallyTransparentUx != nullptr)
		return IsThemeBackgroundPartiallyTransparentUx(hTheme, iPartId, iStateId);
	return FALSE;
}

HRESULT DcxUXModule::dcxDrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect)
{
	if (DrawThemeBackgroundUx != nullptr)
		return DrawThemeBackgroundUx(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
	return NULL;
}

HRESULT DcxUXModule::dcxGetThemeBackgroundContentRect(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect)
{
	if (GetThemeBackgroundContentRectUx != nullptr)
		return GetThemeBackgroundContentRectUx(hTheme, hdc, iPartId, iStateId, pBoundingRect, pContentRect);
	return NULL;
}

HRESULT DcxUXModule::dcxDrawThemeParentBackground(HWND hwnd, HDC hdc, const RECT *prc)
{
	if (DrawThemeParentBackgroundUx != nullptr)
		return DrawThemeParentBackgroundUx(hwnd, hdc, prc);
	return NULL;
}

HRESULT DcxUXModule::dcxDrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect)
{
	if (DrawThemeTextUx != nullptr)
		return DrawThemeTextUx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, dwTextFlags2, pRect);
	return NULL;
}

HRESULT DcxUXModule::dcxGetThemeBackgroundRegion(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HRGN *pRegion)
{
	if (GetThemeBackgroundRegionUx != nullptr)
		return GetThemeBackgroundRegionUx(hTheme, hdc, iPartId, iStateId, pRect, pRegion);
	return NULL;
}

HRESULT DcxUXModule::dcxDrawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect)
{
	if (DrawThemeEdgeUx != nullptr)
		return DrawThemeEdgeUx(hTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect);
	return NULL;
}

HRESULT DcxUXModule::dcxGetThemeColor(HTHEME hTheme, int iPartId, int iStateId, int iPropId, COLORREF *pColor)
{
	if (GetThemeColorUx != nullptr)
		return GetThemeColorUx(hTheme, iPartId, iStateId, iPropId, pColor);
	return NULL;
}

HRESULT DcxUXModule::dcxDrawThemeParentBackgroundEx(HWND hwnd, HDC hdc, DWORD dwFlags, const RECT *prc)
{
	if (DrawThemeParentBackgroundExUx != nullptr)
		return DrawThemeParentBackgroundExUx(hwnd, hdc, dwFlags, prc);
	return NULL;
}

gsl::owner<HPAINTBUFFER> DcxUXModule::dcxBeginBufferedPaint(HDC hdcTarget, const RECT *prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS *pPaintParams, HDC *phdc)
{
	if (BeginBufferedPaintUx != nullptr)
		return BeginBufferedPaintUx(hdcTarget, prcTarget, dwFormat, pPaintParams, phdc);
	return nullptr;
}

HRESULT DcxUXModule::dcxEndBufferedPaint(gsl::owner<HPAINTBUFFER> hBufferedPaint, BOOL fUpdateTarget)
{
	if (EndBufferedPaintUx != nullptr)
		return EndBufferedPaintUx(hBufferedPaint, fUpdateTarget);
	return NULL;
}

HRESULT DcxUXModule::dcxBufferedPaintInit(void)
{
	if (BufferedPaintInitUx != nullptr)
		return BufferedPaintInitUx();
	return NULL;
}

HRESULT DcxUXModule::dcxBufferedPaintUnInit(void)
{
	if (BufferedPaintUnInitUx != nullptr)
		return BufferedPaintUnInitUx();
	return NULL;
}
