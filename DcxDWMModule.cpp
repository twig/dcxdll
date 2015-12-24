#include "defines.h"
#include "DcxDWMModule.h"

PFNDWMISCOMPOSITIONENABLED DcxDWMModule::DwmIsCompositionEnabledUx = nullptr;
PFNDWMGETWINDOWATTRIBUTE DcxDWMModule::DwmGetWindowAttributeUx = nullptr;
PFNDWMSETWINDOWATTRIBUTE DcxDWMModule::DwmSetWindowAttributeUx = nullptr;
PFNDWMEXTENDFRAMEINTOCLIENTAREA DcxDWMModule::DwmExtendFrameIntoClientAreaUx = nullptr;
PFNDWMENABLEBLURBEHINDWINDOW DcxDWMModule::DwmEnableBlurBehindWindowUx = nullptr;
PFNDWMGETCOLORIZATIONCOLOR DcxDWMModule::DwmGetColorizationColorUx = nullptr;

DcxDWMModule::DcxDWMModule(void) :
m_bAero(false),
m_bVista(false),
m_bWin7(false),
m_bWin8(false)
{
}

DcxDWMModule::~DcxDWMModule(void)
{
	if (isUseable()) unload();
}

bool DcxDWMModule::load(void)
{
	if (isUseable())
		return false;

	DWORD winMajor = 0;
	if (!GetWindowVersion(&winMajor, nullptr))
		return false;

	this->m_bVista = (winMajor >= 6);	// OS is Vista+
	this->m_bWin7 = (winMajor > 6);		// OS is Windows7+
	this->m_bWin8 = (winMajor > 7);		// OS is Windows8+
	this->m_bWin10 = (winMajor > 8);	// OS is Windows10+

	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Loading DWMAPI.DLL..."));
	m_hModule = LoadLibrary(TEXT("dwmapi.dll"));
	if (m_hModule != nullptr) {
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("DWMAPI.DLL Loaded, Vista+ OS Assumed"));

		DwmIsCompositionEnabledUx = (PFNDWMISCOMPOSITIONENABLED) GetProcAddress(m_hModule, "DwmIsCompositionEnabled"); // Vista ONLY!
		DwmGetWindowAttributeUx = (PFNDWMGETWINDOWATTRIBUTE) GetProcAddress(m_hModule, "DwmGetWindowAttribute"); // Vista ONLY!
		DwmSetWindowAttributeUx = (PFNDWMSETWINDOWATTRIBUTE) GetProcAddress(m_hModule, "DwmSetWindowAttribute"); // Vista ONLY!
		DwmExtendFrameIntoClientAreaUx = (PFNDWMEXTENDFRAMEINTOCLIENTAREA) GetProcAddress(m_hModule, "DwmExtendFrameIntoClientArea"); // Vista ONLY!
		DwmEnableBlurBehindWindowUx = (PFNDWMENABLEBLURBEHINDWINDOW) GetProcAddress(m_hModule, "DwmEnableBlurBehindWindow"); // Vista ONLY!
		DwmGetColorizationColorUx = (PFNDWMGETCOLORIZATIONCOLOR) GetProcAddress(m_hModule, "DwmGetColorizationColor"); // Vista ONLY!

#if DCX_DEBUG_OUTPUT
		if (DwmIsCompositionEnabledUx != nullptr)
			mIRCLinker::debug(TEXT("LoadDLL"), TEXT("Found Vista DWM Functions"));
#endif

		refreshComposite();
	}
	return isUseable();
}

bool DcxDWMModule::unload(void) 
{
	if (m_hModule != nullptr) {
		FreeLibrary(m_hModule);
		m_hModule = nullptr;
		DwmIsCompositionEnabledUx = nullptr;
		DwmGetWindowAttributeUx = nullptr;
		DwmSetWindowAttributeUx = nullptr;
		DwmExtendFrameIntoClientAreaUx = nullptr;
		DwmEnableBlurBehindWindowUx = nullptr;
		DwmGetColorizationColorUx = nullptr;
	}
	return isUseable();
}

bool DcxDWMModule::refreshComposite() {
	BOOL bAero;
	if (FAILED(dcxDwmIsCompositionEnabled(&bAero)))
		bAero = FALSE;

	m_bAero = (bAero != FALSE);
	return m_bAero;
}

HRESULT DcxDWMModule::dcxDwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
	if (DwmSetWindowAttributeUx != nullptr)
		return DwmSetWindowAttributeUx(hwnd, dwAttribute, pvAttribute, cbAttribute);
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmGetWindowAttribute(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute)
{
	if (DwmGetWindowAttributeUx != nullptr)
		return DwmGetWindowAttributeUx(hwnd, dwAttribute, pvAttribute, cbAttribute);
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmIsCompositionEnabled(BOOL *pfEnabled)
{
	if (DwmIsCompositionEnabledUx != nullptr)
		return DwmIsCompositionEnabledUx(pfEnabled);
	*pfEnabled = FALSE;
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmExtendFrameIntoClientArea(HWND hwnd, const MARGINS *pMarInset)
{
	if (DwmExtendFrameIntoClientAreaUx != nullptr)
		return DwmExtendFrameIntoClientAreaUx(hwnd, pMarInset);
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmEnableBlurBehindWindow(HWND hwnd, __in const DWM_BLURBEHIND *pBlurBehind)
{
	if (DwmEnableBlurBehindWindowUx != nullptr)
		return DwmEnableBlurBehindWindowUx(hwnd, pBlurBehind);
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmGetColorizationColor( __out  DWORD *pcrColorization, __out  BOOL *pfOpaqueBlend)
{
	if (DwmGetColorizationColorUx != nullptr)
		return DwmGetColorizationColorUx(pcrColorization, pfOpaqueBlend);
	return DWM_E_COMPOSITIONDISABLED;
}
