#include "defines.h"
#include "DcxGDIModule.h"
#include "Dcx.h"



DcxGDIModule::DcxGDIModule()
{
}

DcxGDIModule::~DcxGDIModule(void)
{
}

bool DcxGDIModule::load(mIRCLinker &mIRCLink)
{
	if (isUseable()) return false;
	//DcxModule::load(mIRCLink); // does nothing.
#ifdef DCX_USE_GDIPLUS
	// Initialize GDI+
	DCX_DEBUG(mIRCLink.debug,TEXT("LoadDLL"), TEXT("Initializing GDI+..."));
	m_hModule = LoadLibrary(TEXT("GDIPLUS.DLL"));
	if (m_hModule != NULL) {
		GdiplusStartupInput gsi;
		gsi.GdiplusVersion = 1;
		gsi.DebugEventCallback = NULL;
		gsi.SuppressBackgroundThread = FALSE;
		gsi.SuppressExternalCodecs = FALSE;
		if (GdiplusStartup(&m_GDIToken,&gsi,NULL) != Ok) {
			Dcx::error(TEXT("LoadDLL"), TEXT("Unable to Startup GDI+"));
			Dcx::error(TEXT("LoadDLL"), TEXT("Warning Unable to Initialize GDIPlus.dll, Operating in reduced function mode."));
			FreeLibrary(m_hModule);
			m_hModule = NULL;
		}
	}
	else {
		mIRCLink.echo(TEXT("Warning Unable to Load GDIPlus.dll, Operating in reduced function mode."));
	}
#endif
	return isUseable();
}

bool DcxGDIModule::unload(void) {
#ifdef DCX_USE_GDIPLUS
	// Shutdown GDI+
	if (isUseable()) {
		GdiplusShutdown(m_GDIToken);

		FreeLibrary(m_hModule);
		m_hModule = NULL;
		return true;
	}
#endif
	return false;
}
