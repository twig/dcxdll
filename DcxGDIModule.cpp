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
	DcxModule::load(mIRCLink);
#ifdef DCX_USE_GDIPLUS
	// Initialize GDI+
	DCX_DEBUG(mIRCLink.debug,"LoadDLL", "Initializing GDI+...");
	m_hModule = LoadLibrary("GDIPLUS.DLL");
	if (m_hModule != NULL) {
		GdiplusStartupInput gsi;
		gsi.GdiplusVersion = 1;
		gsi.DebugEventCallback = NULL;
		gsi.SuppressBackgroundThread = FALSE;
		gsi.SuppressExternalCodecs = FALSE;
		if (GdiplusStartup(&m_GDIToken,&gsi,NULL) != Ok) {
			Dcx::error("LoadDLL", "Unable to Startup GDI+");
			Dcx::error("LoadDLL", "Warning Unable to Initialize GDIPlus.dll, Operating in reduced function mode.");
			FreeLibrary(m_hModule);
			m_hModule = NULL;
		}
	}
	else {
		mIRCLink.echo("Warning Unable to Load GDIPlus.dll, Operating in reduced function mode.");
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
