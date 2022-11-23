#pragma once
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
typedef HRESULT(WINAPI* PFNDRAWTHEMETEXTEX)(HTHEME hTheme, HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_z_ LPCWSTR pszText, _In_ int cchText, _In_ DWORD dwTextFlags, _Inout_ LPRECT pRect, _In_ const DTTOPTS* pOptions);
typedef HRESULT (WINAPI *PFNGETTHEMEBACKGROUNDREGION)(HTHEME, HDC, int, int, LPCRECT, HRGN *);
typedef HTHEME (WINAPI *PFNGETWINDOWTHEME)(HWND);
typedef HRESULT (WINAPI *PFNDRAWTHEMEEDGE)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect);
typedef HRESULT (WINAPI *PFNGETTHEMECOLOR)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,COLORREF *pColor);
typedef HRESULT(WINAPI* PFNGETTHEMEFONT)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LPLOGFONT plog);
typedef HRESULT(WINAPI* PFNGETTHEMETEXTEXTENT)(HTHEME hTheme, HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_z_ LPCWSTR pszText, _In_ int cchCharCount, _In_ DWORD dwTextFlags, _In_ LPCRECT pBoundingRect, _Inout_ LPRECT pExtentRect);
typedef HRESULT (WINAPI *PFNDRAWTHEMEPARENTBACKGROUNDEX)(HWND, HDC, DWORD, const RECT*);
//typedef HRESULT (WINAPI *PFNGETTHEMEBITMAP)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,ULONG dwFlags,HBITMAP *phBitmap);
// Vista Function pointers.
typedef HRESULT (WINAPI *PFNBUFFEREDPAINTINIT)(VOID);
typedef HRESULT (WINAPI *PFNBUFFEREDPAINTUNINIT)(VOID);
typedef HPAINTBUFFER (WINAPI *PFNBEGINBUFFEREDPAINT)(HDC hdcTarget, const RECT *prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS *pPaintParams, HDC *phdc);
typedef HRESULT (WINAPI *PFNENDBUFFEREDPAINT)(HPAINTBUFFER hBufferedPaint, _In_ BOOL fUpdateTarget);
typedef HRESULT (WINAPI *PFBUFFEREDPAINTSETALPHA)(HPAINTBUFFER hBufferedPaint, _In_ const RECT *prc, _In_ BYTE alpha);
typedef HRESULT(WINAPI* PFBUFFEREDPAINTCLEAR)(HPAINTBUFFER hBufferedPaint, _In_ const RECT* prc);

class DcxUXModule final
	: public DcxModule
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
	static PFNDRAWTHEMETEXTEX DrawThemeTextExUx;
	static PFNGETTHEMEBACKGROUNDREGION GetThemeBackgroundRegionUx;
	static PFNGETWINDOWTHEME GetWindowThemeUx;
	static PFNDRAWTHEMEEDGE DrawThemeEdgeUx;
	static PFNGETTHEMECOLOR GetThemeColorUx;
	static PFNGETTHEMEFONT GetThemeFontUx;
	static PFNGETTHEMETEXTEXTENT GetThemeTextExtentUx;
	static PFNDRAWTHEMEPARENTBACKGROUNDEX DrawThemeParentBackgroundExUx;
	//static PFNGETTHEMEBITMAP GetThemeBitmapUx;
	// Vista Function pointers.
	static PFNBUFFEREDPAINTINIT BufferedPaintInitUx;
	static PFNBUFFEREDPAINTUNINIT BufferedPaintUnInitUx;
	static PFNBEGINBUFFEREDPAINT BeginBufferedPaintUx;
	static PFNENDBUFFEREDPAINT EndBufferedPaintUx;
	static PFBUFFEREDPAINTSETALPHA BufferedPaintSetAlphaUx;
	static PFBUFFEREDPAINTCLEAR BufferedPaintClearUx;

	static bool m_bBufferedPaintEnabled;

public:
	constexpr DcxUXModule(void) noexcept
		: DcxModule()
	{}
	~DcxUXModule(void) noexcept;

	DcxUXModule(const DcxUXModule &other) = delete;	// no copy constructor
	DcxUXModule(const DcxUXModule &&other) = delete;	// no move constructor
	DcxUXModule &operator =(const DcxUXModule &) = delete;	// No assignments!
	DcxUXModule &operator =(const DcxUXModule &&) = delete;	// No move assignments!

	bool load(void) final;
	bool unload(void) noexcept final;

	static BOOL dcxIsThemeActive(void) noexcept;
	static HRESULT dcxSetWindowTheme(const HWND hwnd, const LPCWSTR pszSubAppName, const LPCWSTR pszSubIdList) noexcept;
	[[gsl::suppress(lifetime)]] static HTHEME dcxGetWindowTheme(HWND hWnd) noexcept;
	[[gsl::suppress(lifetime)]] [[nodiscard("Memory Leak")]] static gsl::owner<HTHEME> dcxOpenThemeData(HWND hwnd, LPCWSTR pszClassList) noexcept;
	static HRESULT dcxCloseThemeData(gsl::owner<HTHEME> hTheme) noexcept;
	static BOOL dcxIsThemeBackgroundPartiallyTransparent(HTHEME hTheme, int iPartId, int iStateId) noexcept;
	static HRESULT dcxDrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect) noexcept;
	static HRESULT dcxGetThemeBackgroundContentRect(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect) noexcept;
	static HRESULT dcxDrawThemeParentBackground(HWND hwnd, HDC hdc, const RECT *prc) noexcept;
	static HRESULT dcxDrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect) noexcept;
	static HRESULT dcxDrawThemeTextEx(HTHEME hTheme, HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_z_ LPCWSTR pszText, _In_ int cchText, _In_ DWORD dwTextFlags, _Inout_ LPRECT pRect, _In_ const DTTOPTS* pOptions) noexcept;
	static HRESULT dcxGetThemeBackgroundRegion(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HRGN *pRegion) noexcept;
	static HRESULT dcxDrawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect) noexcept;
	static HRESULT dcxGetThemeColor(HTHEME hTheme, int iPartId, int iStateId, int iPropId, COLORREF *pColor) noexcept;
	static HRESULT dcxGetThemeFont(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LPLOGFONT plog) noexcept;
	static HRESULT dcxGetThemeTextExtent(HTHEME hTheme, HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_z_ LPCWSTR pszText, _In_ int cchCharCount, _In_ DWORD dwTextFlags, _In_ LPCRECT pBoundingRect, _Inout_ LPRECT pExtentRect) noexcept;
	static HRESULT dcxDrawThemeParentBackgroundEx(HWND hwnd, HDC hdc, DWORD dwFlags, const RECT *prc) noexcept;
	static inline const bool &IsBufferedPaintSupported(void) noexcept { return m_bBufferedPaintEnabled; }
	static HRESULT dcxBufferedPaintInit(void) noexcept;
	static HRESULT dcxBufferedPaintUnInit(void) noexcept;
	[[gsl::suppress(lifetime,Enum.3)]] [[nodiscard("Memory Leak")]] static gsl::owner<HPAINTBUFFER> dcxBeginBufferedPaint(HDC hdcTarget, const RECT *prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS *pPaintParams, HDC *phdc) noexcept;
	static HRESULT dcxEndBufferedPaint(gsl::owner<HPAINTBUFFER> hBufferedPaint, _In_ BOOL fUpdateTarget) noexcept;
	static HRESULT dcxBufferedPaintSetAlpha(HPAINTBUFFER hBufferedPaint, _In_ const RECT *prc, _In_ BYTE alpha) noexcept;
	HRESULT dcxBufferedPaintClear(HPAINTBUFFER hBufferedPaint, _In_ const RECT* prc) noexcept;
};
#endif // _DCXUXMODULES_H_
