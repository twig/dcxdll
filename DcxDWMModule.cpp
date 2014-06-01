#include "defines.h"
#include "DcxDWMModule.h"

PFNDWMISCOMPOSITIONENABLED DcxDWMModule::DwmIsCompositionEnabledUx = NULL;
PFNDWMGETWINDOWATTRIBUTE DcxDWMModule::DwmGetWindowAttributeUx = NULL;
PFNDWMSETWINDOWATTRIBUTE DcxDWMModule::DwmSetWindowAttributeUx = NULL;
PFNDWMEXTENDFRAMEINTOCLIENTAREA DcxDWMModule::DwmExtendFrameIntoClientAreaUx = NULL;
PFNDWMENABLEBLURBEHINDWINDOW DcxDWMModule::DwmEnableBlurBehindWindowUx = NULL;
PFNDWMGETCOLORIZATIONCOLOR DcxDWMModule::DwmGetColorizationColorUx = NULL;

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
	if (!GetWindowVersion(&winMajor, NULL))
		return false;

	this->m_bVista = (winMajor >= 6);	// OS is Vista+
	this->m_bWin7 = (winMajor > 6);		// OS is Windows7+
	this->m_bWin8 = (winMajor > 7);		// OS is Windows8+

	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Loading DWMAPI.DLL..."));
	m_hModule = LoadLibrary(TEXT("dwmapi.dll"));
	if (m_hModule != NULL) {
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("DWMAPI.DLL Loaded, Vista+ OS Assumed"));

		DwmIsCompositionEnabledUx = (PFNDWMISCOMPOSITIONENABLED) GetProcAddress(m_hModule, "DwmIsCompositionEnabled"); // Vista ONLY!
		DwmGetWindowAttributeUx = (PFNDWMGETWINDOWATTRIBUTE) GetProcAddress(m_hModule, "DwmGetWindowAttribute"); // Vista ONLY!
		DwmSetWindowAttributeUx = (PFNDWMSETWINDOWATTRIBUTE) GetProcAddress(m_hModule, "DwmSetWindowAttribute"); // Vista ONLY!
		DwmExtendFrameIntoClientAreaUx = (PFNDWMEXTENDFRAMEINTOCLIENTAREA) GetProcAddress(m_hModule, "DwmExtendFrameIntoClientArea"); // Vista ONLY!
		DwmEnableBlurBehindWindowUx = (PFNDWMENABLEBLURBEHINDWINDOW) GetProcAddress(m_hModule, "DwmEnableBlurBehindWindow"); // Vista ONLY!
		DwmGetColorizationColorUx = (PFNDWMGETCOLORIZATIONCOLOR) GetProcAddress(m_hModule, "DwmGetColorizationColor"); // Vista ONLY!

#if DCX_DEBUG_OUTPUT
		if (DwmIsCompositionEnabledUx != NULL)
			mIRCLinker::debug(TEXT("LoadDLL"), TEXT("Found Vista DWM Functions"));
#endif

		refreshComposite();
	}
	return isUseable();
}

bool DcxDWMModule::unload(void) 
{
	if (m_hModule != NULL) {
		FreeLibrary(m_hModule);
		m_hModule = NULL;
		return true;
	}
	return false;
}

bool DcxDWMModule::refreshComposite() {
	BOOL bAero = FALSE;
	dcxDwmIsCompositionEnabled(&bAero);

	if (bAero)
		m_bAero = true;
	else
		m_bAero = false;
	return m_bAero;
}

HRESULT DcxDWMModule::dcxDwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
	if (DwmSetWindowAttributeUx != NULL)
		return DwmSetWindowAttributeUx(hwnd, dwAttribute, pvAttribute, cbAttribute);
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmGetWindowAttribute(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute)
{
	if (DwmGetWindowAttributeUx != NULL)
		return DwmGetWindowAttributeUx(hwnd, dwAttribute, pvAttribute, cbAttribute);
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmIsCompositionEnabled(BOOL *pfEnabled)
{
	if (DwmIsCompositionEnabledUx != NULL)
		return DwmIsCompositionEnabledUx(pfEnabled);
	*pfEnabled = FALSE;
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmExtendFrameIntoClientArea(HWND hwnd, const MARGINS *pMarInset)
{
	if (DwmExtendFrameIntoClientAreaUx != NULL)
		return DwmExtendFrameIntoClientAreaUx(hwnd, pMarInset);
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmEnableBlurBehindWindow(HWND hwnd, __in const DWM_BLURBEHIND *pBlurBehind)
{
	if (DwmEnableBlurBehindWindowUx != NULL)
		return DwmEnableBlurBehindWindowUx(hwnd, pBlurBehind);
	return DWM_E_COMPOSITIONDISABLED;
}

HRESULT DcxDWMModule::dcxDwmGetColorizationColor( __out  DWORD *pcrColorization, __out  BOOL *pfOpaqueBlend)
{
	if (DwmGetColorizationColorUx != NULL)
		return DwmGetColorizationColorUx(pcrColorization, pfOpaqueBlend);
	return DWM_E_COMPOSITIONDISABLED;
}
