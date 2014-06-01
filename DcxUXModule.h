#if _MSC_VER > 1000
#pragma once
#endif
#ifndef _DCXUXMODULES_H_
#define _DCXUXMODULES_H_

#include "dcxmodule.h"

typedef HRESULT (WINAPI *PFNSETTHEME)(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);
typedef BOOL (WINAPI *PFNISTHEMEACTIVE)();
typedef HTHEME (WINAPI *PFNOPENTHEMEDATA)(HWND, LPCWSTR);
typedef HRESULT (WINAPI *PFNCLOSETHEMEDATA)(HTHEME);
typedef HRESULT (WINAPI *PFNDRAWTHEMEBACKGROUND)(HTHEME, HDC, int, int, const RECT*, const RECT *);
typedef HRESULT (WINAPI *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME, HDC, int, int,  const RECT *, RECT *);
typedef BOOL (WINAPI *PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)(HTHEME, int, int);
typedef HRESULT (WINAPI *PFNDRAWTHEMEPARENTBACKGROUND)(HWND, HDC, const RECT*);
typedef HRESULT (WINAPI *PFNDRAWTHEMETEXT)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, DWORD, const RECT *);
typedef HRESULT (WINAPI *PFNGETTHEMEBACKGROUNDREGION)(HTHEME, HDC, int, int, LPCRECT, HRGN *);
typedef HTHEME (WINAPI *PFNGETWINDOWTHEME)(HWND);
typedef HRESULT (WINAPI *PFNDRAWTHEMEEDGE)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect);
typedef HRESULT (WINAPI *PFNGETTHEMECOLOR)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,COLORREF *pColor);
typedef HRESULT (WINAPI *PFNDRAWTHEMEPARENTBACKGROUNDEX)(HWND, HDC, DWORD, const RECT*);
//typedef HRESULT (WINAPI *PFNGETTHEMEBITMAP)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,ULONG dwFlags,HBITMAP *phBitmap);
// Vista Function pointers.
#ifdef DCX_USE_WINSDK
typedef HRESULT (WINAPI *PFNBUFFEREDPAINTINIT)(VOID);
typedef HRESULT (WINAPI *PFNBUFFEREDPAINTUNINIT)(VOID);
typedef HPAINTBUFFER (WINAPI *PFNBEGINBUFFEREDPAINT)(HDC hdcTarget, const RECT *prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS *pPaintParams, HDC *phdc);
typedef HRESULT (WINAPI *PFNENDBUFFEREDPAINT)(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget);
#endif

class DcxUXModule :
	public DcxModule
{
	static PFNSETTHEME SetWindowThemeUx;
	static PFNISTHEMEACTIVE IsThemeActiveUx;
	static PFNOPENTHEMEDATA OpenThemeDataUx;
	static PFNCLOSETHEMEDATA CloseThemeDataUx;
	static PFNDRAWTHEMEBACKGROUND DrawThemeBackgroundUx;
	static PFNGETTHEMEBACKGROUNDCONTENTRECT GetThemeBackgroundContentRectUx;
	static PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT IsThemeBackgroundPartiallyTransparentUx;
	static PFNDRAWTHEMEPARENTBACKGROUND DrawThemeParentBackgroundUx;
	static PFNDRAWTHEMETEXT DrawThemeTextUx;
	static PFNGETTHEMEBACKGROUNDREGION GetThemeBackgroundRegionUx;
	static PFNGETWINDOWTHEME GetWindowThemeUx;
	static PFNDRAWTHEMEEDGE DrawThemeEdgeUx;
	static PFNGETTHEMECOLOR GetThemeColorUx;
	static PFNDRAWTHEMEPARENTBACKGROUNDEX DrawThemeParentBackgroundExUx;
	//static PFNGETTHEMEBITMAP GetThemeBitmapUx;
	// Vista Function pointers.
#ifdef DCX_USE_WINSDK
	static PFNBUFFEREDPAINTINIT BufferedPaintInitUx;
	static PFNBUFFEREDPAINTUNINIT BufferedPaintUnInitUx;
	static PFNBEGINBUFFEREDPAINT BeginBufferedPaintUx;
	static PFNENDBUFFEREDPAINT EndBufferedPaintUx;
#endif

public:
	DcxUXModule(void);
	~DcxUXModule(void);

	bool load(void);
	bool unload(void);

	static BOOL dcxIsThemeActive(void);
	static HRESULT dcxSetWindowTheme(const HWND hwnd, const LPCWSTR pszSubAppName, const LPCWSTR pszSubIdList);
	static HTHEME dcxGetWindowTheme(HWND hWnd);
	static HTHEME dcxOpenThemeData(HWND hwnd, LPCWSTR pszClassList);
	static HRESULT dcxCloseThemeData(HTHEME hTheme);
	static BOOL dcxIsThemeBackgroundPartiallyTransparent(HTHEME hTheme, int iPartId, int iStateId);
	static HRESULT dcxDrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect);
	static HRESULT dcxGetThemeBackgroundContentRect(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect);
	static HRESULT dcxDrawThemeParentBackground(HWND hwnd, HDC hdc, const RECT *prc);
	static HRESULT dcxDrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect);
	static HRESULT dcxGetThemeBackgroundRegion(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HRGN *pRegion);
	static HRESULT dcxDrawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect);
	static HRESULT dcxGetThemeColor(HTHEME hTheme, int iPartId, int iStateId, int iPropId, COLORREF *pColor);
	static HRESULT dcxDrawThemeParentBackgroundEx(HWND hwnd, HDC hdc, DWORD dwFlags, const RECT *prc);
#ifdef DCX_USE_WINSDK
	static bool IsBufferedPaintSupported(void);
	static HRESULT dcxBufferedPaintInit(void);
	static HRESULT dcxBufferedPaintUnInit(void);
	static HPAINTBUFFER dcxBeginBufferedPaint(HDC hdcTarget, const RECT *prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS *pPaintParams, HDC *phdc);
	static HRESULT dcxEndBufferedPaint(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget);
#endif
};
#endif // _DCXUXMODULES_H_
