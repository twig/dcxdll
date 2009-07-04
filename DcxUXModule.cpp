#include "DcxUXModule.h"
#include "Dcx.h"



DcxUXModule::DcxUXModule(void)
{
}

DcxUXModule::~DcxUXModule(void)
{
	if (isUseable()) unload();
}

bool DcxUXModule::load(mIRCLinker &mIRCLink)
{
	if (isUseable()) return false;
	DcxModule::load(mIRCLink);
	// UXModule Loading
	DCX_DEBUG(mIRCLink.debug,"LoadDLL", "Loading UXTHEME.DLL...");
	m_hModule = LoadLibrary("UXTHEME.DLL");

	if (m_hModule) {
		// Get XP+ function pointers.
		SetWindowThemeUx = (PFNSETTHEME) GetProcAddress(m_hModule, "SetWindowTheme");
		IsThemeActiveUx = (PFNISTHEMEACTIVE) GetProcAddress(m_hModule, "IsThemeActive");
		OpenThemeDataUx = (PFNOPENTHEMEDATA) GetProcAddress(m_hModule, "OpenThemeData");
		CloseThemeDataUx = (PFNCLOSETHEMEDATA) GetProcAddress(m_hModule, "CloseThemeData");
		DrawThemeBackgroundUx = (PFNDRAWTHEMEBACKGROUND) GetProcAddress(m_hModule, "DrawThemeBackground");
		GetThemeBackgroundContentRectUx = (PFNGETTHEMEBACKGROUNDCONTENTRECT) GetProcAddress(m_hModule, "GetThemeBackgroundContentRect");
		IsThemeBackgroundPartiallyTransparentUx = (PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT) GetProcAddress(m_hModule, "IsThemeBackgroundPartiallyTransparent");
		DrawThemeParentBackgroundUx = (PFNDRAWTHEMEPARENTBACKGROUND) GetProcAddress(m_hModule, "DrawThemeParentBackground");
		DrawThemeTextUx = (PFNDRAWTHEMETEXT) GetProcAddress(m_hModule, "DrawThemeText");
		GetThemeBackgroundRegionUx = (PFNGETTHEMEBACKGROUNDREGION) GetProcAddress(m_hModule, "GetThemeBackgroundRegion");
		GetWindowThemeUx = (PFNGETWINDOWTHEME) GetProcAddress(m_hModule, "GetWindowTheme");
		DrawThemeEdgeUx = (PFNDRAWTHEMEEDGE) GetProcAddress(m_hModule, "DrawThemeEdge");
		GetThemeColorUx = (PFNGETTHEMECOLOR) GetProcAddress(m_hModule, "GetThemeColor");

		// Get Vista function pointers.
#ifdef DCX_USE_WINSDK
		DrawThemeParentBackgroundExUx = (PFNDRAWTHEMEPARENTBACKGROUNDEX) GetProcAddress(m_hModule, "DrawThemeParentBackgroundEx"); // Vista ONLY!
		//GetThemeBitmapUx = (PFNGETTHEMEBITMAP) GetProcAddress(UXModule, "GetThemeBitmap");
		BufferedPaintInitUx = (PFNBUFFEREDPAINTINIT) GetProcAddress(m_hModule, "BufferedPaintInit");
		BufferedPaintUnInitUx = (PFNBUFFEREDPAINTUNINIT) GetProcAddress(m_hModule, "BufferedPaintUnInit");
		BeginBufferedPaintUx = (PFNBEGINBUFFEREDPAINT) GetProcAddress(m_hModule, "BeginBufferedPaint");
		EndBufferedPaintUx = (PFNENDBUFFEREDPAINT) GetProcAddress(m_hModule, "EndBufferedPaint");
#endif

		// NB: DONT count vista functions in XP+ check.
		if (SetWindowThemeUx && IsThemeActiveUx && OpenThemeDataUx && CloseThemeDataUx &&
			DrawThemeBackgroundUx && GetThemeBackgroundContentRectUx && IsThemeBackgroundPartiallyTransparentUx &&
			DrawThemeParentBackgroundUx && DrawThemeTextUx && GetThemeBackgroundRegionUx && GetWindowThemeUx && DrawThemeEdgeUx && GetThemeColorUx) {
				DCX_DEBUG(mIRCLink.debug,"LoadDLL", "Found XP+ Theme Functions");
#ifdef DCX_USE_WINSDK
			if (DrawThemeParentBackgroundExUx && BufferedPaintInitUx && BufferedPaintUnInitUx &&
				BeginBufferedPaintUx && EndBufferedPaintUx) {
				DCX_DEBUG(mIRCLink.debug,"LoadDLL", "Found Vista Theme Functions");
				BufferedPaintInitUx();
			}
#endif
		}
		else {
			FreeLibrary(m_hModule);
			m_hModule = NULL;
			Dcx::error("LoadDLL","There was a problem loading IsThemedXP");
		}
	}
	return isUseable();
}
bool DcxUXModule::unload(void)
{
	if (isUseable()) {
#ifdef DCX_USE_WINSDK
		if (BufferedPaintUnInitUx)
			BufferedPaintUnInitUx();
#endif

		FreeLibrary(m_hModule);
		m_hModule = NULL;
		return true;
	}
	return false;
}
