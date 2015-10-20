#pragma once
#include "dcxmodule.h"

typedef HRESULT (WINAPI *PFNDWMISCOMPOSITIONENABLED)(BOOL *pfEnabled);
typedef HRESULT (WINAPI *PFNDWMGETWINDOWATTRIBUTE)(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute);
typedef HRESULT (WINAPI *PFNDWMSETWINDOWATTRIBUTE)(HWND hwnd,DWORD dwAttribute,LPCVOID pvAttribute,DWORD cbAttribute);
typedef HRESULT (WINAPI *PFNDWMEXTENDFRAMEINTOCLIENTAREA)(HWND hWnd, const MARGINS *pMarInset);
typedef HRESULT (WINAPI *PFNDWMENABLEBLURBEHINDWINDOW)(HWND hWnd, __in const DWM_BLURBEHIND *pBlurBehind);
typedef HRESULT (WINAPI *PFNDWMGETCOLORIZATIONCOLOR)( __out  DWORD *pcrColorization, __out  BOOL *pfOpaqueBlend);

class DcxDWMModule :
	public DcxModule
{
	bool m_bAero;
	bool m_bVista;
	bool m_bWin7;
	bool m_bWin8;
	static PFNDWMISCOMPOSITIONENABLED DwmIsCompositionEnabledUx;
	static PFNDWMGETWINDOWATTRIBUTE DwmGetWindowAttributeUx;
	static PFNDWMSETWINDOWATTRIBUTE DwmSetWindowAttributeUx;
	static PFNDWMEXTENDFRAMEINTOCLIENTAREA DwmExtendFrameIntoClientAreaUx;
	static PFNDWMENABLEBLURBEHINDWINDOW DwmEnableBlurBehindWindowUx;
	static PFNDWMGETCOLORIZATIONCOLOR DwmGetColorizationColorUx;

public:
	DcxDWMModule(void);
	~DcxDWMModule(void);

	bool load(void);
	bool unload(void);
	bool refreshComposite();
	HRESULT dcxDwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
	HRESULT dcxDwmGetWindowAttribute(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute);
	HRESULT dcxDwmIsCompositionEnabled(BOOL *pfEnabled);
	HRESULT dcxDwmExtendFrameIntoClientArea(HWND hwnd, const MARGINS *pMarInset);
	HRESULT dcxDwmEnableBlurBehindWindow(HWND hwnd, __in const DWM_BLURBEHIND *pBlurBehind);
	HRESULT dcxDwmGetColorizationColor( __out  DWORD *pcrColorization, __out  BOOL *pfOpaqueBlend);

	const bool &isAero(void) const { return this->m_bAero; };
	const bool &isVista(void) const { return this->m_bVista; };
	const bool &isWin7(void) const { return this->m_bWin7; };
	const bool &isWin8(void) const { return this->m_bWin8; };
};
