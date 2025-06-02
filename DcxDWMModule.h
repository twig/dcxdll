#pragma once
#include "dcxmodule.h"

typedef HRESULT (WINAPI *PFNDWMISCOMPOSITIONENABLED)(BOOL *pfEnabled);
typedef HRESULT (WINAPI *PFNDWMGETWINDOWATTRIBUTE)(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute);
typedef HRESULT (WINAPI *PFNDWMSETWINDOWATTRIBUTE)(HWND hwnd,DWORD dwAttribute,LPCVOID pvAttribute,DWORD cbAttribute);
typedef HRESULT (WINAPI *PFNDWMEXTENDFRAMEINTOCLIENTAREA)(HWND hWnd, const MARGINS *pMarInset);
typedef HRESULT (WINAPI *PFNDWMENABLEBLURBEHINDWINDOW)(HWND hWnd, __in const DWM_BLURBEHIND *pBlurBehind);
typedef HRESULT (WINAPI *PFNDWMGETCOLORIZATIONCOLOR)( __out  DWORD *pcrColorization, __out  BOOL *pfOpaqueBlend);

class DcxDWMModule final
	: public DcxModule
{
	bool m_bAero{ false };
	static inline PFNDWMISCOMPOSITIONENABLED DwmIsCompositionEnabledUx = nullptr;
	static inline PFNDWMGETWINDOWATTRIBUTE DwmGetWindowAttributeUx = nullptr;
	static inline PFNDWMSETWINDOWATTRIBUTE DwmSetWindowAttributeUx = nullptr;
	static inline PFNDWMEXTENDFRAMEINTOCLIENTAREA DwmExtendFrameIntoClientAreaUx = nullptr;
	static inline PFNDWMENABLEBLURBEHINDWINDOW DwmEnableBlurBehindWindowUx = nullptr;
	static inline PFNDWMGETCOLORIZATIONCOLOR DwmGetColorizationColorUx = nullptr;

public:
	constexpr DcxDWMModule(void) noexcept
		: DcxModule()
	{
	}
	~DcxDWMModule(void) noexcept;

	DcxDWMModule(const DcxDWMModule &other) = delete;	// no copy constructor
	DcxDWMModule(const DcxDWMModule &&other) = delete;	// no move constructor
	GSL_SUPPRESS(c.128) DcxDWMModule &operator =(const DcxDWMModule &) = delete;	// No assignments!
	GSL_SUPPRESS(c.128) DcxDWMModule &operator =(const DcxDWMModule &&) = delete;	// No move assignments!

	bool load(void) final;
	bool unload(void) noexcept final;
	const bool &refreshComposite() noexcept;
	static HRESULT dcxDwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) noexcept;
	static HRESULT dcxDwmGetWindowAttribute(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute) noexcept;
	static HRESULT dcxDwmIsCompositionEnabled(BOOL *pfEnabled) noexcept;
	static HRESULT dcxDwmExtendFrameIntoClientArea(HWND hwnd, const MARGINS *pMarInset) noexcept;
	static HRESULT dcxDwmEnableBlurBehindWindow(HWND hwnd, __in const DWM_BLURBEHIND *pBlurBehind) noexcept;
	static HRESULT dcxDwmGetColorizationColor( __out  DWORD *pcrColorization, __out  BOOL *pfOpaqueBlend) noexcept;

	const bool &isAero(void) const noexcept { return m_bAero; };
};
