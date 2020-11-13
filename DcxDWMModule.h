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
	bool m_bVista{ false };
	bool m_bWin7{ false };
	bool m_bWin8{ false };
	bool m_bWin10{ false };
	static PFNDWMISCOMPOSITIONENABLED DwmIsCompositionEnabledUx;
	static PFNDWMGETWINDOWATTRIBUTE DwmGetWindowAttributeUx;
	static PFNDWMSETWINDOWATTRIBUTE DwmSetWindowAttributeUx;
	static PFNDWMEXTENDFRAMEINTOCLIENTAREA DwmExtendFrameIntoClientAreaUx;
	static PFNDWMENABLEBLURBEHINDWINDOW DwmEnableBlurBehindWindowUx;
	static PFNDWMGETCOLORIZATIONCOLOR DwmGetColorizationColorUx;

public:
	constexpr DcxDWMModule(void) noexcept
		: DcxModule()
	{
	}
	~DcxDWMModule(void) noexcept;

	DcxDWMModule(const DcxDWMModule &other) = delete;	// no copy constructor
	DcxDWMModule(const DcxDWMModule &&other) = delete;	// no move constructor
	DcxDWMModule &operator =(const DcxDWMModule &) = delete;	// No assignments!
	DcxDWMModule &operator =(const DcxDWMModule &&) = delete;	// No move assignments!

	bool load(void) final;
	bool unload(void) noexcept final;
	const bool &refreshComposite() noexcept;
	HRESULT dcxDwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) noexcept;
	HRESULT dcxDwmGetWindowAttribute(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute) noexcept;
	HRESULT dcxDwmIsCompositionEnabled(BOOL *pfEnabled) noexcept;
	HRESULT dcxDwmExtendFrameIntoClientArea(HWND hwnd, const MARGINS *pMarInset) noexcept;
	HRESULT dcxDwmEnableBlurBehindWindow(HWND hwnd, __in const DWM_BLURBEHIND *pBlurBehind) noexcept;
	HRESULT dcxDwmGetColorizationColor( __out  DWORD *pcrColorization, __out  BOOL *pfOpaqueBlend) noexcept;

	const bool &isAero(void) const noexcept { return m_bAero; };
	const bool &isVista(void) const noexcept { return m_bVista; };
	const bool &isWin7(void) const noexcept { return m_bWin7; };
	const bool &isWin8(void) const noexcept { return m_bWin8; };
	const bool &isWin10(void) const noexcept { return m_bWin10; };
};
