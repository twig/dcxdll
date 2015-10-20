#include "defines.h"
#include "DcxGDIModule.h"
#include "Dcx.h"

DcxGDIModule::DcxGDIModule()
: m_GDIToken(NULL)
{
}

DcxGDIModule::~DcxGDIModule(void)
{
}

bool DcxGDIModule::load(void)
{
	if (isUseable())
		return false;

#ifdef DCX_USE_GDIPLUS
	// Initialize GDI+
	DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Initializing GDI+..."));
	m_hModule = LoadLibrary(TEXT("GDIPLUS.DLL"));
	if (m_hModule != nullptr) {
		GdiplusStartupInput gsi;
		gsi.GdiplusVersion = 1;
		gsi.DebugEventCallback = nullptr;
		gsi.SuppressBackgroundThread = FALSE;
		gsi.SuppressExternalCodecs = FALSE;
		if (GdiplusStartup(&m_GDIToken,&gsi,nullptr) != Ok) {	// dont throw error just display warnings
			Dcx::error(TEXT("LoadDLL"), TEXT("Unable to Startup GDI+"));
			Dcx::error(TEXT("LoadDLL"), TEXT("Warning Unable to Initialize GDIPlus.dll, Operating in reduced function mode."));
			FreeLibrary(m_hModule);
			m_hModule = nullptr;
		}
	}
	else // dont throw error just display warnings
		Dcx::error(TEXT("LoadDLL"), TEXT("Warning Unable to Load GDIPlus.dll, Operating in reduced function mode."));
#endif
	return isUseable();
}

bool DcxGDIModule::unload(void) {
#ifdef DCX_USE_GDIPLUS
	// Shutdown GDI+
	if (isUseable()) {
		GdiplusShutdown(m_GDIToken);

		FreeLibrary(m_hModule);
		m_hModule = nullptr;
	}
#endif
	return isUseable();
}
