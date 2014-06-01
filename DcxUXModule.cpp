#include "defines.h"
#include "DcxUXModule.h"
#include "Dcx.h"

// Theme functions
PFNSETTHEME DcxUXModule::SetWindowThemeUx = NULL;
PFNISTHEMEACTIVE DcxUXModule::IsThemeActiveUx = NULL;
PFNOPENTHEMEDATA DcxUXModule::OpenThemeDataUx = NULL;
PFNCLOSETHEMEDATA DcxUXModule::CloseThemeDataUx = NULL;
PFNDRAWTHEMEBACKGROUND DcxUXModule::DrawThemeBackgroundUx = NULL;
PFNGETTHEMEBACKGROUNDCONTENTRECT DcxUXModule::GetThemeBackgroundContentRectUx = NULL;
PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT DcxUXModule::IsThemeBackgroundPartiallyTransparentUx = NULL;
PFNDRAWTHEMEPARENTBACKGROUND DcxUXModule::DrawThemeParentBackgroundUx = NULL;
PFNDRAWTHEMETEXT DcxUXModule::DrawThemeTextUx = NULL;
PFNGETTHEMEBACKGROUNDREGION DcxUXModule::GetThemeBackgroundRegionUx = NULL;
PFNGETWINDOWTHEME DcxUXModule::GetWindowThemeUx = NULL;
PFNDRAWTHEMEEDGE DcxUXModule::DrawThemeEdgeUx = NULL;
PFNGETTHEMECOLOR DcxUXModule::GetThemeColorUx = NULL;
PFNDRAWTHEMEPARENTBACKGROUNDEX DcxUXModule::DrawThemeParentBackgroundExUx = NULL;
//PFNGETTHEMEBITMAP DcxUXModule::GetThemeBitmapUx = NULL;
// Vista Function pointers.
#ifdef DCX_USE_WINSDK
PFNBUFFEREDPAINTINIT DcxUXModule::BufferedPaintInitUx = NULL;
PFNBUFFEREDPAINTUNINIT DcxUXModule::BufferedPaintUnInitUx = NULL;
PFNBEGINBUFFEREDPAINT DcxUXModule::BeginBufferedPaintUx = NULL;
PFNENDBUFFEREDPAINT DcxUXModule::EndBufferedPaintUx = NULL;
#endif

DcxUXModule::DcxUXModule(void)
{
}

DcxUXModule::~DcxUXModule(void)
{
	if (isUseable()) unload();
}

bool DcxUXModule::load(void)
{
	if (isUseable())
		return false;
	// UXModule Loading
	DCX_DEBUG(mIRCLinker::debug,TEXT("LoadDLL"), TEXT("Loading UXTHEME.DLL..."));
	m_hModule = LoadLibrary(TEXT("UXTHEME.DLL"));

	if (m_hModule != NULL) {
		// Get XP+ function pointers.
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
#ifdef DCX_USE_WINSDK
		DrawThemeParentBackgroundExUx = (PFNDRAWTHEMEPARENTBACKGROUNDEX) GetProcAddress(m_hModule, "DrawThemeParentBackgroundEx"); // Vista ONLY!
		//GetThemeBitmapUx = (PFNGETTHEMEBITMAP) GetProcAddress(UXModule, "GetThemeBitmap");
		BufferedPaintInitUx = (PFNBUFFEREDPAINTINIT) GetProcAddress(m_hModule, "BufferedPaintInit");
		BufferedPaintUnInitUx = (PFNBUFFEREDPAINTUNINIT) GetProcAddress(m_hModule, "BufferedPaintUnInit");
		BeginBufferedPaintUx = (PFNBEGINBUFFEREDPAINT) GetProcAddress(m_hModule, "BeginBufferedPaint");
		EndBufferedPaintUx = (PFNENDBUFFEREDPAINT) GetProcAddress(m_hModule, "EndBufferedPaint");
#endif

		// NB: DONT count vista functions in XP+ check.
		if (SetWindowThemeUx && IsThemeActiveUx && OpenThemeDataUx && CloseThemeDataUx &&
			DrawThemeBackgroundUx && GetThemeBackgroundContentRectUx && IsThemeBackgroundPartiallyTransparentUx &&
			DrawThemeParentBackgroundUx && DrawThemeTextUx && GetThemeBackgroundRegionUx && GetWindowThemeUx && DrawThemeEdgeUx && GetThemeColorUx) {
			DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Found XP+ Theme Functions"));
#ifdef DCX_USE_WINSDK
			if (DrawThemeParentBackgroundExUx && BufferedPaintInitUx && BufferedPaintUnInitUx &&
				BeginBufferedPaintUx && EndBufferedPaintUx) {
				DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Found Vista Theme Functions"));
				BufferedPaintInitUx();
			}
#endif
		}
		else {
			FreeLibrary(m_hModule);
			m_hModule = NULL;
			// make sure all functions are NULL
			SetWindowThemeUx = NULL;
			IsThemeActiveUx = NULL;
			OpenThemeDataUx = NULL;
			CloseThemeDataUx = NULL;
			DrawThemeBackgroundUx = NULL;
			GetThemeBackgroundContentRectUx = NULL;
			IsThemeBackgroundPartiallyTransparentUx = NULL;
			DrawThemeParentBackgroundUx = NULL;
			DrawThemeTextUx = NULL;
			GetThemeBackgroundRegionUx = NULL;
			GetWindowThemeUx = NULL;
			DrawThemeEdgeUx = NULL;
			GetThemeColorUx = NULL;
			DrawThemeParentBackgroundExUx = NULL;
#ifdef DCX_USE_WINSDK
			BufferedPaintInitUx = NULL;
			BufferedPaintUnInitUx = NULL;
			BeginBufferedPaintUx = NULL;
			EndBufferedPaintUx = NULL;
#endif
			Dcx::error(TEXT("LoadDLL"),TEXT("There was a problem loading IsThemedXP"));
		}
	}
	return isUseable();
}
bool DcxUXModule::unload(void)
{
	if (isUseable()) {
#ifdef DCX_USE_WINSDK
		if (BufferedPaintUnInitUx != NULL)
			BufferedPaintUnInitUx();
#endif

		FreeLibrary(m_hModule);
		m_hModule = NULL;
		return true;
	}
	return false;
}

/*!
* \brief Check if theme is active
*
*
*/
BOOL DcxUXModule::dcxIsThemeActive(void)
{
	if (IsThemeActiveUx != NULL)
		return IsThemeActiveUx();
	return FALSE;
}
/*!
* \brief Windows Theme Setting Function
*
* Used to remove theme on controls
*/
HRESULT DcxUXModule::dcxSetWindowTheme(const HWND hwnd, const LPCWSTR pszSubAppName, const LPCWSTR pszSubIdList) {
	if (SetWindowThemeUx != NULL)
		return SetWindowThemeUx(hwnd, pszSubAppName, pszSubIdList);
	return 0;
}

HTHEME DcxUXModule::dcxGetWindowTheme(HWND hWnd)
{
	if (GetWindowThemeUx != NULL)
		return GetWindowThemeUx(hWnd);
	return NULL;
}

HTHEME DcxUXModule::dcxOpenThemeData(HWND hwnd, LPCWSTR pszClassList)
{
	if (OpenThemeDataUx != NULL)
		return OpenThemeDataUx(hwnd, pszClassList);
	return NULL;
}

HRESULT DcxUXModule::dcxCloseThemeData(HTHEME hTheme)
{
	if (CloseThemeDataUx != NULL)
		return CloseThemeDataUx(hTheme);
	return NULL;
}

//int DcxUXModule::dcxGetThemeBackgroundRegion(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPRECT pRect, HRGN *pRegion)
//{
//	if (GetThemeBackgroundRegionUx != NULL)
//		return GetThemeBackgroundRegionUx(hTheme, hdc, iPartId, iStateId, pRect, pRegion);
//	return NULL;
//}

BOOL DcxUXModule::dcxIsThemeBackgroundPartiallyTransparent(HTHEME hTheme, int iPartId, int iStateId)
{
	if (IsThemeBackgroundPartiallyTransparentUx != NULL)
		return IsThemeBackgroundPartiallyTransparentUx(hTheme, iPartId, iStateId);
	return FALSE;
}

HRESULT DcxUXModule::dcxDrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect)
{
	if (DrawThemeBackgroundUx != NULL)
		return DrawThemeBackgroundUx(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
	return NULL;
}

HRESULT DcxUXModule::dcxGetThemeBackgroundContentRect(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect)
{
	if (GetThemeBackgroundContentRectUx != NULL)
		return GetThemeBackgroundContentRectUx(hTheme, hdc, iPartId, iStateId, pBoundingRect, pContentRect);
	return NULL;
}

HRESULT DcxUXModule::dcxDrawThemeParentBackground(HWND hwnd, HDC hdc, const RECT *prc)
{
	if (DrawThemeParentBackgroundUx != NULL)
		return DrawThemeParentBackgroundUx(hwnd, hdc, prc);
	return NULL;
}

HRESULT DcxUXModule::dcxDrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect)
{
	if (DrawThemeTextUx != NULL)
		return DrawThemeTextUx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, dwTextFlags2, pRect);
	return NULL;
}

HRESULT DcxUXModule::dcxGetThemeBackgroundRegion(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HRGN *pRegion)
{
	if (GetThemeBackgroundRegionUx != NULL)
		return GetThemeBackgroundRegionUx(hTheme, hdc, iPartId, iStateId, pRect, pRegion);
	return NULL;
}

HRESULT DcxUXModule::dcxDrawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect)
{
	if (DrawThemeEdgeUx != NULL)
		return DrawThemeEdgeUx(hTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect);
	return NULL;
}

HRESULT DcxUXModule::dcxGetThemeColor(HTHEME hTheme, int iPartId, int iStateId, int iPropId, COLORREF *pColor)
{
	if (GetThemeColorUx != NULL)
		return GetThemeColorUx(hTheme, iPartId, iStateId, iPropId, pColor);
	return NULL;
}

HRESULT DcxUXModule::dcxDrawThemeParentBackgroundEx(HWND hwnd, HDC hdc, DWORD dwFlags, const RECT *prc)
{
	if (DrawThemeParentBackgroundExUx != NULL)
		return DrawThemeParentBackgroundExUx(hwnd, hdc, dwFlags, prc);
	return NULL;
}

#ifdef DCX_USE_WINSDK

HPAINTBUFFER DcxUXModule::dcxBeginBufferedPaint(HDC hdcTarget, const RECT *prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS *pPaintParams, HDC *phdc)
{
	if (BeginBufferedPaintUx != NULL)
		return BeginBufferedPaintUx(hdcTarget, prcTarget, dwFormat, pPaintParams, phdc);
	return NULL;
}

HRESULT DcxUXModule::dcxEndBufferedPaint(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget)
{
	if (EndBufferedPaintUx != NULL)
		return EndBufferedPaintUx(hBufferedPaint, fUpdateTarget);
	return NULL;
}

HRESULT DcxUXModule::dcxBufferedPaintInit(void)
{
	if (BufferedPaintInitUx != NULL)
		return BufferedPaintInitUx();
	return NULL;
}

HRESULT DcxUXModule::dcxBufferedPaintUnInit(void)
{
	if (BufferedPaintUnInitUx != NULL)
		return BufferedPaintUnInitUx();
	return NULL;
}

bool DcxUXModule::IsBufferedPaintSupported(void)
{
	return ((BufferedPaintInitUx != NULL) && (BufferedPaintUnInitUx != NULL) && (BeginBufferedPaintUx != NULL) && (EndBufferedPaintUx != NULL));
}

#endif
