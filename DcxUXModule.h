#pragma once
#ifndef _DCXUXMODULES_H_
#define _DCXUXMODULES_H_

#include "dcxmodule.h"

//SetWindowTheme
typedef HRESULT(WINAPI* PFNSETTHEME)(_In_ HWND hwnd, _In_opt_ LPCWSTR pszSubAppName, _In_opt_ LPCWSTR pszSubIdList);
//IsThemeActive
typedef BOOL(WINAPI* PFNISTHEMEACTIVE)();
//OpenThemeData
typedef HTHEME(WINAPI* PFNOPENTHEMEDATA)(_In_opt_ HWND, _In_ LPCWSTR);
//CloseThemeData
typedef HRESULT(WINAPI* PFNCLOSETHEMEDATA)(_In_ HTHEME);
//DrawThemeBackground
typedef HRESULT(WINAPI* PFNDRAWTHEMEBACKGROUND)(_In_ HTHEME, _In_ HDC, _In_ int, _In_ int, _In_ LPCRECT, _In_opt_ LPCRECT);
//GetThemeBackgroundContentRect
typedef HRESULT(WINAPI* PFNGETTHEMEBACKGROUNDCONTENTRECT)(_In_ HTHEME, _In_opt_ HDC, _In_ int, _In_ int, _In_ LPCRECT, _Out_ LPRECT);
//IsThemeBackgroundPartiallyTransparent
typedef BOOL(WINAPI* PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)(_In_ HTHEME, _In_ int, _In_ int);
//DrawThemeParentBackground
typedef HRESULT(WINAPI* PFNDRAWTHEMEPARENTBACKGROUND)(_In_ HWND, _In_ HDC, _In_opt_ LPCRECT);
//DrawThemeText
typedef HRESULT(WINAPI* PFNDRAWTHEMETEXT)(_In_ HTHEME, _In_ HDC, _In_ int, _In_ int, _In_reads_(cchText) LPCWSTR, _In_ int cchText, _In_ DWORD, _In_ DWORD, _In_ LPCRECT);
//DrawThemeTextEx
typedef HRESULT(WINAPI* PFNDRAWTHEMETEXTEX)(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_reads_(cchText) LPCWSTR pszText, _In_ int cchText, _In_ DWORD dwTextFlags, _Inout_ LPRECT pRect, _In_opt_ const DTTOPTS* pOptions);
//GetThemeBackgroundRegion
typedef HRESULT(WINAPI* PFNGETTHEMEBACKGROUNDREGION)(_In_ HTHEME, _In_opt_ HDC, _In_ int, _In_ int, _In_ LPCRECT, _Out_ HRGN*);
//GetWindowTheme
typedef HTHEME(WINAPI* PFNGETWINDOWTHEME)(_In_ HWND);
//DrawThemeEdge
typedef HRESULT(WINAPI* PFNDRAWTHEMEEDGE)(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ LPCRECT pDestRect, _In_ UINT uEdge, _In_ UINT uFlags, _Out_opt_ LPRECT pContentRect);
//GetThemeColor
typedef HRESULT(WINAPI* PFNGETTHEMECOLOR)(_In_ HTHEME hTheme, _In_ int iPartId, _In_ int iStateId, _In_ int iPropId, _Out_ COLORREF* pColor);
//GetThemeFont
typedef HRESULT(WINAPI* PFNGETTHEMEFONT)(_In_ HTHEME hTheme, _In_opt_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ int iPropId, _Out_ LPLOGFONT plog);
//GetThemeTextExtent
typedef HRESULT(WINAPI* PFNGETTHEMETEXTEXTENT)(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_reads_(cchCharCount) LPCWSTR pszText, _In_ int cchCharCount, _In_ DWORD dwTextFlags, _In_opt_ LPCRECT pBoundingRect, _Out_ LPRECT pExtentRect);
//DrawThemeParentBackgroundEx
typedef HRESULT(WINAPI* PFNDRAWTHEMEPARENTBACKGROUNDEX)(_In_ HWND, _In_ HDC, _In_ DWORD, _In_opt_ LPCRECT);
//GetThemeBitmap
//typedef HRESULT (WINAPI *PFNGETTHEMEBITMAP)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,ULONG dwFlags,HBITMAP *phBitmap);

// Vista Function pointers.
//BufferedPaintInit
typedef HRESULT(WINAPI* PFNBUFFEREDPAINTINIT)(VOID);
//BufferedPaintUnInit
typedef HRESULT(WINAPI* PFNBUFFEREDPAINTUNINIT)(VOID);
//BeginBufferedPaint
typedef HPAINTBUFFER(WINAPI* PFNBEGINBUFFEREDPAINT)(_In_ HDC hdcTarget, _In_ LPCRECT prcTarget, _In_ BP_BUFFERFORMAT dwFormat, _In_opt_ BP_PAINTPARAMS* pPaintParams, _Out_ HDC* phdc);
//EndBufferedPaint
typedef HRESULT(WINAPI* PFNENDBUFFEREDPAINT)(_In_ HPAINTBUFFER hBufferedPaint, _In_ BOOL fUpdateTarget);
//BufferedPaintSetAlpha
typedef HRESULT(WINAPI* PFNBUFFEREDPAINTSETALPHA)(_In_ HPAINTBUFFER hBufferedPaint, _In_opt_ LPCRECT prc, _In_ BYTE alpha);
//BufferedPaintClear
typedef HRESULT(WINAPI* PFNBUFFEREDPAINTCLEAR)(_In_ HPAINTBUFFER hBufferedPaint, _In_opt_ LPCRECT prc);
//HitTestThemeBackground
typedef HRESULT(WINAPI* PFNHITTESTTHEMEBACKGROUND)(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ DWORD dwOptions, _In_ LPCRECT pRect, _In_ HRGN hrgn, _In_ POINT ptTest, _Out_ WORD* pwHitTestCode);
//GetThemeRect
typedef HRESULT(WINAPI* PFNGETTHEMERECT)(_In_ HTHEME hTheme, _In_ int iPartId, _In_ int iStateId, _In_ int iPropId, _Out_ LPRECT pRect);

//BeginBufferedAnimation
//BeginPanningFeedback
//BufferedPaintRenderAnimation
//BufferedPaintStopAllAnimations
//DllCanUnloadNow
//DllGetActivationFactory
//DllGetClassObject
//DrawThemeBackgroundEx
//DrawThemeIcon
//EnableThemeDialogTexture
//EnableTheming
//EndBufferedAnimation
//EndPanningFeedback
//GetBufferedPaintBits
//GetBufferedPaintDC
//GetBufferedPaintTargetDC
//GetBufferedPaintTargetRect
//GetColorFromPreference
//GetCurrentThemeName
//GetImmersiveColorFromColorSetEx
//GetImmersiveUserColorSetPreference
//GetThemeAnimationProperty
//GetThemeAnimationTransform
//GetThemeAppProperties
//GetThemeBackgroundExtent
//GetThemeBool
//GetThemeDocumentationProperty
//GetThemeEnumValue
//GetThemeFilename
//GetThemeInt
//GetThemeIntList
//GetThemeMargins
//GetThemeMetric
//GetThemePartSize
//GetThemePosition
//GetThemePropertyOrigin
//GetThemeStream
//GetThemeString
//GetThemeSysBool
//GetThemeSysColor
//GetThemeSysColorBrush
//GetThemeSysFont
//GetThemeSysInt
//GetThemeSysSize
//GetThemeSysString
//GetThemeTextMetrics
//GetThemeTimingFunction
//GetThemeTransitionDuration
//GetUserColorPreference
//IsAppThemed
//IsCompositionActive
//IsThemeDialogTextureEnabled
//IsThemePartDefined
//SetThemeAppProperties
//SetWindowThemeAttribute
//ThemeInitApiHook
//UpdatePanningFeedback
//OpenThemeDataEx

class DcxUXModule final
	: public DcxModule
{
	static inline PFNSETTHEME SetWindowThemeUx = nullptr;
	static inline PFNISTHEMEACTIVE IsThemeActiveUx = nullptr;
	static inline PFNOPENTHEMEDATA OpenThemeDataUx = nullptr;
	static inline PFNCLOSETHEMEDATA CloseThemeDataUx = nullptr;
	static inline PFNDRAWTHEMEBACKGROUND DrawThemeBackgroundUx = nullptr;
	static inline PFNGETTHEMEBACKGROUNDCONTENTRECT GetThemeBackgroundContentRectUx = nullptr;
	static inline PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT IsThemeBackgroundPartiallyTransparentUx = nullptr;
	static inline PFNDRAWTHEMEPARENTBACKGROUND DrawThemeParentBackgroundUx = nullptr;
	static inline PFNDRAWTHEMETEXT DrawThemeTextUx = nullptr;
	static inline PFNDRAWTHEMETEXTEX DrawThemeTextExUx = nullptr;
	static inline PFNGETTHEMEBACKGROUNDREGION GetThemeBackgroundRegionUx = nullptr;
	static inline PFNGETWINDOWTHEME GetWindowThemeUx = nullptr;
	static inline PFNDRAWTHEMEEDGE DrawThemeEdgeUx = nullptr;
	static inline PFNGETTHEMECOLOR GetThemeColorUx = nullptr;
	static inline PFNGETTHEMEFONT GetThemeFontUx = nullptr;
	static inline PFNGETTHEMETEXTEXTENT GetThemeTextExtentUx = nullptr;
	static inline PFNDRAWTHEMEPARENTBACKGROUNDEX DrawThemeParentBackgroundExUx = nullptr;
	static inline PFNGETTHEMERECT GetThemeRectUx = nullptr;
	// Vista Function pointers.
	static inline PFNBUFFEREDPAINTINIT BufferedPaintInitUx = nullptr;
	static inline PFNBUFFEREDPAINTUNINIT BufferedPaintUnInitUx = nullptr;
	static inline PFNBEGINBUFFEREDPAINT BeginBufferedPaintUx = nullptr;
	static inline PFNENDBUFFEREDPAINT EndBufferedPaintUx = nullptr;
	static inline PFNBUFFEREDPAINTSETALPHA BufferedPaintSetAlphaUx = nullptr;
	static inline PFNBUFFEREDPAINTCLEAR BufferedPaintClearUx = nullptr;
	static inline PFNHITTESTTHEMEBACKGROUND HitTestThemeBackgroundUx = nullptr;
	// Win10+ functions
	static inline decltype(::OpenThemeDataForDpi)* OpenThemeDataForDpiUx = nullptr;

	static inline bool m_bBufferedPaintEnabled = false;

public:
	constexpr DcxUXModule(void) noexcept
		: DcxModule()
	{}
	~DcxUXModule(void) noexcept;

	DcxUXModule(const DcxUXModule& other) = delete;	// no copy constructor
	DcxUXModule(const DcxUXModule&& other) = delete;	// no move constructor
	GSL_SUPPRESS(c.128) DcxUXModule& operator =(const DcxUXModule&) = delete;	// No assignments!
	GSL_SUPPRESS(c.128) DcxUXModule& operator =(const DcxUXModule&&) = delete;	// No move assignments!

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
	static HRESULT dcxBufferedPaintClear(_In_ HPAINTBUFFER hBufferedPaint, _In_opt_ LPCRECT prc) noexcept;
	static HRESULT dcxHitTestThemeBackground(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iPartId, _In_ int iStateId, _In_ DWORD dwOptions, _In_ LPCRECT pRect, _In_ HRGN hrgn, _In_ POINT ptTest, _Out_ WORD* pwHitTestCode) noexcept;
	static HRESULT dcxGetThemeRect(_In_ HTHEME hTheme, _In_ int iPartId, _In_ int iStateId, _In_ int iPropId, _Out_ LPRECT pRect) noexcept;
	[[nodiscard("Memory Leak")]] static HTHEME dcxOpenThemeDataForDpi(HWND hwnd, LPCWSTR pszClassList, UINT dpi) noexcept;

};
#endif // _DCXUXMODULES_H_
