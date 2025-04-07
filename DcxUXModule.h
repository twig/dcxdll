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

// Win10+ pointers...
enum class IMMERSIVE_HC_CACHE_MODE : UINT
{
	IHCM_USE_CACHED_VALUE,
	IHCM_REFRESH
};

// 1903 18362
enum class PreferredAppMode : UINT
{
	Default,
	AllowDark,
	ForceDark,
	ForceLight,
	Max
};

//enum WINDOWCOMPOSITIONATTRIB
//{
//	WCA_UNDEFINED = 0,
//	WCA_NCRENDERING_ENABLED = 1,
//	WCA_NCRENDERING_POLICY = 2,
//	WCA_TRANSITIONS_FORCEDISABLED = 3,
//	WCA_ALLOW_NCPAINT = 4,
//	WCA_CAPTION_BUTTON_BOUNDS = 5,
//	WCA_NONCLIENT_RTL_LAYOUT = 6,
//	WCA_FORCE_ICONIC_REPRESENTATION = 7,
//	WCA_EXTENDED_FRAME_BOUNDS = 8,
//	WCA_HAS_ICONIC_BITMAP = 9,
//	WCA_THEME_ATTRIBUTES = 10,
//	WCA_NCRENDERING_EXILED = 11,
//	WCA_NCADORNMENTINFO = 12,
//	WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
//	WCA_VIDEO_OVERLAY_ACTIVE = 14,
//	WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
//	WCA_DISALLOW_PEEK = 16,
//	WCA_CLOAK = 17,
//	WCA_CLOAKED = 18,
//	WCA_ACCENT_POLICY = 19,
//	WCA_FREEZE_REPRESENTATION = 20,
//	WCA_EVER_UNCLOAKED = 21,
//	WCA_VISUAL_OWNER = 22,
//	WCA_HOLOGRAPHIC = 23,
//	WCA_EXCLUDED_FROM_DDA = 24,
//	WCA_PASSIVEUPDATEMODE = 25,
//	WCA_USEDARKMODECOLORS = 26,
//	WCA_LAST = 27
//};

//struct WINDOWCOMPOSITIONATTRIBDATA
//{
//	WINDOWCOMPOSITIONATTRIB Attrib;
//	PVOID pvData;
//	SIZE_T cbData;
//};

// 1809 17763
using PFNSHOULDAPPSUSEDARKMODE = bool (WINAPI*)(); // ordinal 132
using PFNALLOWDARKMODEFORWINDOW = bool (WINAPI*)(HWND hWnd, bool allow); // ordinal 133
using PFNALLOWDARKMODEFORAPP = bool (WINAPI*)(bool allow); // ordinal 135, in 1809
//using fnFlushMenuThemes = void (WINAPI*)(); // ordinal 136
using PFNREFRESHIMMERSIVECOLORPOLICYSTATE = void (WINAPI*)(); // ordinal 104
using PFNISDARKMODEALLOWEDFORWINDOW = bool (WINAPI*)(HWND hWnd); // ordinal 137
using PFNGETISIMMERSIVECOLORUSINGHIGHCONTRAST = bool (WINAPI*)(IMMERSIVE_HC_CACHE_MODE mode); // ordinal 106
using PFNOPENNCTHEMEDATA = HTHEME(WINAPI*)(HWND hWnd, LPCWSTR pszClassList); // ordinal 49
// 1903 18362
using PFNSHOULDSYSTEMUSEDARKMODE = bool (WINAPI*)(); // ordinal 138
using PFNSETPREFERREDAPPMODE = PreferredAppMode(WINAPI*)(PreferredAppMode appMode); // ordinal 135, in 1903
using PFNISDARKMODEALLOWEDFORAPP = bool (WINAPI*)(); // ordinal 139

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
	// undocumented...
	// 1809 17763
	static inline PFNSHOULDAPPSUSEDARKMODE ShouldAppsUseDarkModeUx = nullptr;
	static inline PFNALLOWDARKMODEFORWINDOW AllowDarkModeForWindowUx = nullptr;
	static inline PFNALLOWDARKMODEFORAPP AllowDarkModeForAppUx = nullptr;
	//static inline fnFlushMenuThemes FlushMenuThemesUx = nullptr;
	static inline PFNREFRESHIMMERSIVECOLORPOLICYSTATE RefreshImmersiveColorPolicyStateUx = nullptr;
	static inline PFNISDARKMODEALLOWEDFORWINDOW IsDarkModeAllowedForWindowUx = nullptr;
	static inline PFNGETISIMMERSIVECOLORUSINGHIGHCONTRAST GetIsImmersiveColorUsingHighContrastUx = nullptr;
	static inline PFNOPENNCTHEMEDATA OpenNcThemeDataUx = nullptr;
	// 1903 18362
	static inline PFNSHOULDSYSTEMUSEDARKMODE ShouldSystemUseDarkModeUx = nullptr;
	static inline PFNSETPREFERREDAPPMODE SetPreferredAppModeUx = nullptr;
	static inline PFNISDARKMODEALLOWEDFORAPP IsDarkModeAllowedForAppUx = nullptr;

	static inline bool m_bBufferedPaintEnabled = false;
	static inline bool m_bDarkModeSupported = false;
	static inline bool m_bDarkModeEnabled = false;
	static inline DWORD m_dwBuildNumber{};

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

	/// <summary>
	/// This function performs a quick calculation of the perceived brightness of a color, and takes into consideration ways that different channels in an RGB color value contribute to how bright it looks to the human eye.
	/// See: https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/ui/apply-windows-themes#know-when-dark-mode-is-enabled
	/// </summary>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline bool IsColorLight(_In_ COLORREF clr) noexcept
	{
		return (((5 * GetGValue(clr)) + (2 * GetRValue(clr)) + GetBValue(clr)) > (8 * 128));
	}

	/// <summary>
	/// Check if dark mode is currently enabled.
	/// </summary>
	/// <returns></returns>
	inline bool dcxIsDarkModeEnabled() noexcept
	{
		return dcxShouldAppsUseDarkMode() && !dcxIsHighContrast() && dcxIsDarkModeAllowedForApp();
	}

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
	[[nodiscard("Memory Leak")]] static HTHEME dcxOpenThemeDataForDpi(_In_opt_ HWND hwnd, _In_ LPCWSTR pszClassList, _In_ UINT dpi) noexcept;
	static bool dcxShouldAppsUseDarkMode() noexcept;
	static bool dcxAllowDarkModeForWindow(_In_opt_ HWND hWnd, _In_ bool allow) noexcept;
	//static void dcxFlushMenuThemes() noexcept;
	static void dcxRefreshImmersiveColorPolicyState() noexcept;
	static bool dcxIsDarkModeAllowedForWindow(_In_opt_ HWND hWnd) noexcept;
	static bool dcxGetIsImmersiveColorUsingHighContrast(_In_ IMMERSIVE_HC_CACHE_MODE mode) noexcept;
	[[nodiscard("Memory Leak")]] static HTHEME dcxOpenNcThemeData(_In_opt_ HWND hWnd, _In_z_ LPCWSTR pszClassList) noexcept;
	static bool dcxShouldSystemUseDarkMode() noexcept;
	static PreferredAppMode dcxSetPreferredAppMode(_In_ PreferredAppMode appMode) noexcept;
	static bool dcxIsDarkModeAllowedForApp() noexcept;
	static DWORD dcxGetWindows10Build() noexcept;
	static void dcxAllowDarkModeForApp(_In_ bool allow) noexcept;
	static bool dcxIsHighContrast() noexcept;
	static void dcxRefreshTitleBarThemeColor(_In_opt_ HWND mHwnd) noexcept;

};
#endif // _DCXUXMODULES_H_
