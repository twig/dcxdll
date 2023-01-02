#pragma once
#ifndef _DCXUXMODULES_H_
#define _DCXUXMODULES_H_

#include "dcxmodule.h"

typedef HRESULT(WINAPI* PFNSETTHEME)(_In_ HWND hwnd, _In_opt_ LPCWSTR pszSubAppName, _In_opt_ LPCWSTR pszSubIdList);
typedef BOOL(WINAPI* PFNISTHEMEACTIVE)();
typedef HTHEME(WINAPI* PFNOPENTHEMEDATA)(_In_opt_ HWND, _In_ LPCWSTR);
typedef HRESULT(WINAPI* PFNCLOSETHEMEDATA)(_In_ HTHEME);
typedef HRESULT(WINAPI* PFNDRAWTHEMEBACKGROUND)(_In_ HTHEME, _In_ HDC, _In_ int, _In_ int, _In_ LPCRECT, _In_opt_ LPCRECT);
typedef HRESULT(WINAPI* PFNGETTHEMEBACKGROUNDCONTENTRECT)(_In_ HTHEME, _In_opt_ HDC, _In_ int, _In_ int, _In_ LPCRECT, _Out_ LPRECT);
typedef BOOL(WINAPI* PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)(_In_ HTHEME, _In_ int, _In_ int);
typedef HRESULT(WINAPI* PFNDRAWTHEMEPARENTBACKGROUND)(_In_ HWND, _In_ HDC, _In_opt_ LPCRECT);
typedef HRESULT(WINAPI* PFNDRAWTHEMETEXT)(_In_ HTHEME, _In_ HDC, _In_ int, _In_ int, _In_reads_(cchText) LPCWSTR, _In_ int cchText, _In_ DWORD, _In_ DWORD, _In_ LPCRECT);
typedef HRESULT(WINAPI* PFNDRAWTHEMETEXTEX)(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_reads_(cchText) LPCWSTR pszText, _In_ int cchText, _In_ DWORD dwTextFlags, _Inout_ LPRECT pRect, _In_opt_ const DTTOPTS* pOptions);
typedef HRESULT(WINAPI* PFNGETTHEMEBACKGROUNDREGION)(_In_ HTHEME, _In_opt_ HDC, _In_ int, _In_ int, _In_ LPCRECT, _Out_ HRGN*);
typedef HTHEME(WINAPI* PFNGETWINDOWTHEME)(_In_ HWND);
typedef HRESULT(WINAPI* PFNDRAWTHEMEEDGE)(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ LPCRECT pDestRect, _In_ UINT uEdge, _In_ UINT uFlags, _Out_opt_ LPRECT pContentRect);
typedef HRESULT(WINAPI* PFNGETTHEMECOLOR)(_In_ HTHEME hTheme, _In_ int iPartId, _In_ int iStateId, _In_ int iPropId, _Out_ COLORREF* pColor);
typedef HRESULT(WINAPI* PFNGETTHEMEFONT)(_In_ HTHEME hTheme, _In_opt_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ int iPropId, _Out_ LPLOGFONT plog);
typedef HRESULT(WINAPI* PFNGETTHEMETEXTEXTENT)(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_reads_(cchCharCount) LPCWSTR pszText, _In_ int cchCharCount, _In_ DWORD dwTextFlags, _In_opt_ LPCRECT pBoundingRect, _Out_ LPRECT pExtentRect);
typedef HRESULT(WINAPI* PFNDRAWTHEMEPARENTBACKGROUNDEX)(_In_ HWND, _In_ HDC, _In_ DWORD, _In_opt_ LPCRECT);
//typedef HRESULT (WINAPI *PFNGETTHEMEBITMAP)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,ULONG dwFlags,HBITMAP *phBitmap);
// Vista Function pointers.
typedef HRESULT(WINAPI* PFNBUFFEREDPAINTINIT)(VOID);
typedef HRESULT(WINAPI* PFNBUFFEREDPAINTUNINIT)(VOID);
typedef HPAINTBUFFER(WINAPI* PFNBEGINBUFFEREDPAINT)(_In_ HDC hdcTarget, _In_ LPCRECT prcTarget, _In_ BP_BUFFERFORMAT dwFormat, _In_opt_ BP_PAINTPARAMS* pPaintParams, _Out_ HDC* phdc);
typedef HRESULT(WINAPI* PFNENDBUFFEREDPAINT)(_In_ HPAINTBUFFER hBufferedPaint, _In_ BOOL fUpdateTarget);
typedef HRESULT(WINAPI* PFBUFFEREDPAINTSETALPHA)(_In_ HPAINTBUFFER hBufferedPaint, _In_opt_ LPCRECT prc, _In_ BYTE alpha);
typedef HRESULT(WINAPI* PFBUFFEREDPAINTCLEAR)(_In_ HPAINTBUFFER hBufferedPaint, _In_opt_ LPCRECT prc);

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

	DcxUXModule(const DcxUXModule& other) = delete;	// no copy constructor
	DcxUXModule(const DcxUXModule&& other) = delete;	// no move constructor
	DcxUXModule& operator =(const DcxUXModule&) = delete;	// No assignments!
	DcxUXModule& operator =(const DcxUXModule&&) = delete;	// No move assignments!

	bool load(void) final;
	bool unload(void) noexcept final;

	static BOOL dcxIsThemeActive(void) noexcept;
	static HRESULT dcxSetWindowTheme(_In_ const HWND hwnd, _In_opt_ const LPCWSTR pszSubAppName, _In_opt_ const LPCWSTR pszSubIdList) noexcept;
	GSL_SUPPRESS(lifetime) static HTHEME dcxGetWindowTheme(_In_ HWND hWnd) noexcept;
	GSL_SUPPRESS(lifetime) [[nodiscard("Memory Leak")]] static gsl::owner<HTHEME> dcxOpenThemeData(_In_ HWND hwnd, _In_ LPCWSTR pszClassList) noexcept;
	static HRESULT dcxCloseThemeData(_In_ gsl::owner<HTHEME> hTheme) noexcept;
	static BOOL dcxIsThemeBackgroundPartiallyTransparent(_In_ HTHEME hTheme, _In_ int iPartId, _In_ int iStateId) noexcept;
	static HRESULT dcxDrawThemeBackground(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ LPCRECT pRect, _In_opt_ LPCRECT pClipRect) noexcept;
	static HRESULT dcxGetThemeBackgroundContentRect(_In_ HTHEME hTheme, _In_opt_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ LPCRECT pBoundingRect, _Out_ LPRECT pContentRect) noexcept;
	static HRESULT dcxDrawThemeParentBackground(_In_ HWND hwnd, _In_ HDC hdc, _In_opt_ LPCRECT prc) noexcept;
	static HRESULT dcxDrawThemeText(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_reads_(cchText) LPCWSTR pszText, _In_ int cchText, _In_ DWORD dwTextFlags, _In_ DWORD dwTextFlags2, _In_ LPCRECT pRect) noexcept;
	static HRESULT dcxDrawThemeTextEx(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_reads_(cchText) LPCWSTR pszText, _In_ int cchText, _In_ DWORD dwTextFlags, _Inout_ LPRECT pRect, _In_opt_ const DTTOPTS* pOptions) noexcept;
	static HRESULT dcxGetThemeBackgroundRegion(_In_ HTHEME hTheme, _In_opt_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ LPCRECT pRect, _Out_ HRGN* pRegion) noexcept;
	static HRESULT dcxDrawThemeEdge(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ LPCRECT pDestRect, _In_ UINT uEdge, _In_ UINT uFlags, _Out_opt_ LPRECT pContentRect) noexcept;
	static HRESULT dcxGetThemeColor(_In_ HTHEME hTheme, _In_ int iPartId, _In_ int iStateId, _In_ int iPropId, _Out_ COLORREF* pColor) noexcept;
	static HRESULT dcxGetThemeFont(_In_ HTHEME hTheme, _In_opt_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ int iPropId, _Out_ LPLOGFONT plog) noexcept;
	static HRESULT dcxGetThemeTextExtent(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_reads_(cchCharCount) LPCWSTR pszText, _In_ int cchCharCount, _In_ DWORD dwTextFlags, _In_opt_ LPCRECT pBoundingRect, _Out_ LPRECT pExtentRect) noexcept;
	static HRESULT dcxDrawThemeParentBackgroundEx(_In_ HWND hwnd, _In_ HDC hdc, _In_ DWORD dwFlags, _In_opt_ LPCRECT prc) noexcept;
	static inline const bool& IsBufferedPaintSupported(void) noexcept { return m_bBufferedPaintEnabled; }
	static HRESULT dcxBufferedPaintInit(void) noexcept;
	static HRESULT dcxBufferedPaintUnInit(void) noexcept;
	[[gsl::suppress(lifetime, Enum.3)]] [[nodiscard("Memory Leak")]] _Success_(return != NULL) static gsl::owner<HPAINTBUFFER> dcxBeginBufferedPaint(_In_ HDC hdcTarget, _In_ LPCRECT prcTarget, _In_ BP_BUFFERFORMAT dwFormat, _In_opt_ BP_PAINTPARAMS* pPaintParams, _Out_ HDC* phdc) noexcept;
	static HRESULT dcxEndBufferedPaint(_In_ gsl::owner<HPAINTBUFFER> hBufferedPaint, _In_ BOOL fUpdateTarget) noexcept;
	static HRESULT dcxBufferedPaintSetAlpha(_In_ HPAINTBUFFER hBufferedPaint, _In_opt_ LPCRECT prc, _In_ BYTE alpha) noexcept;
	HRESULT dcxBufferedPaintClear(_In_ HPAINTBUFFER hBufferedPaint, _In_opt_ LPCRECT prc) noexcept;
};
#endif // _DCXUXMODULES_H_
