#include "defines.h"
#include "DcxDWMModule.h"

PFNDWMISCOMPOSITIONENABLED DcxDWMModule::DwmIsCompositionEnabledUx = NULL;
PFNDWMGETWINDOWATTRIBUTE DcxDWMModule::DwmGetWindowAttributeUx = NULL;
PFNDWMSETWINDOWATTRIBUTE DcxDWMModule::DwmSetWindowAttributeUx = NULL;
PFNDWMEXTENDFRAMEINTOCLIENTAREA DcxDWMModule::DwmExtendFrameIntoClientAreaUx = NULL;

DcxDWMModule::DcxDWMModule(void)
{
	m_bAero = false;
}

DcxDWMModule::~DcxDWMModule(void)
{
	if (isUseable()) unload();
}

bool DcxDWMModule::load(mIRCLinker &mIRCLink)
{
	if (isUseable()) return false;
	DcxModule::load(mIRCLink);
	DCX_DEBUG(mIRCLink.debug,"LoadDLL", "Loading DWMAPI.DLL...");
	m_hModule = LoadLibrary("dwmapi.dll");
	if (m_hModule != NULL) {
		DCX_DEBUG(mIRCLink.debug,"LoadDLL", "DWMAPI.DLL Loaded, Vista+ OS Assumed");

		DwmIsCompositionEnabledUx = (PFNDWMISCOMPOSITIONENABLED) GetProcAddress(m_hModule, "DwmIsCompositionEnabled"); // Vista ONLY!
		DwmGetWindowAttributeUx = (PFNDWMGETWINDOWATTRIBUTE) GetProcAddress(m_hModule, "DwmGetWindowAttribute"); // Vista ONLY!
		DwmSetWindowAttributeUx = (PFNDWMSETWINDOWATTRIBUTE) GetProcAddress(m_hModule, "DwmSetWindowAttribute"); // Vista ONLY!
		DwmExtendFrameIntoClientAreaUx = (PFNDWMEXTENDFRAMEINTOCLIENTAREA) GetProcAddress(m_hModule, "DwmExtendFrameIntoClientArea"); // Vista ONLY!

#if DCX_DEBUG_OUTPUT
		if (DwmIsCompositionEnabledUx != NULL)
			mIRCLink.debug("LoadDLL", "Found Vista DWM Functions");
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
