#pragma once
#include "dcxmodule.h"

typedef HRESULT (WINAPI *PFNDWMISCOMPOSITIONENABLED)(BOOL *pfEnabled);
typedef HRESULT (WINAPI *PFNDWMGETWINDOWATTRIBUTE)(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute);
typedef HRESULT (WINAPI *PFNDWMSETWINDOWATTRIBUTE)(HWND hwnd,DWORD dwAttribute,LPCVOID pvAttribute,DWORD cbAttribute);
typedef HRESULT (WINAPI *PFNDWMEXTENDFRAMEINTOCLIENTAREA)(HWND hWnd, const MARGINS *pMarInset);

// Missing defines, since the new module way of doing things doesnt work well with missing libraries
#ifndef DCX_USE_WINSDK
#define DWM_E_COMPOSITIONDISABLED 0x80263001
#endif

class DcxDWMModule :
	public DcxModule
{
	bool m_bAero;
	static PFNDWMISCOMPOSITIONENABLED DwmIsCompositionEnabledUx;
	static PFNDWMGETWINDOWATTRIBUTE DwmGetWindowAttributeUx;
	static PFNDWMSETWINDOWATTRIBUTE DwmSetWindowAttributeUx;
	static PFNDWMEXTENDFRAMEINTOCLIENTAREA DwmExtendFrameIntoClientAreaUx;

public:
	DcxDWMModule(void);
	~DcxDWMModule(void);

	bool load(mIRCLinker &mIRCLink);
	bool unload(void);
	bool refreshComposite();
	HRESULT dcxDwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
	HRESULT dcxDwmGetWindowAttribute(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute);
	HRESULT dcxDwmIsCompositionEnabled(BOOL *pfEnabled);
	HRESULT dcxDwmExtendFrameIntoClientArea(HWND hwnd, const MARGINS *pMarInset);

	bool isAero(void) const { return this->m_bAero; };
};
