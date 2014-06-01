#pragma once
//#include "defines.h"
#include "mIRCLinker.h"
#include "DcxUXModule.h"
#include "DcxGDIModule.h"
#include "DcxDWMModule.h"
#include "Classes/xpopup/XPopupMenuManager.h"
#include "Classes/xpopup/xmenubar.h"
#include "Classes/DcxDialog.h"
#include <sys/stat.h> 

// is it just me or is this class just a namespace?

namespace Dcx
{
	extern TString m_sLastError;
	extern BYTE m_iGhostDrag;
	extern IClassFactory *m_pClassFactory;
	extern bool m_bDX9Installed;

	extern DcxDialogCollection Dialogs;
	extern DcxGDIModule GDIModule;
	extern DcxUXModule UXModule;
	extern DcxDWMModule VistaModule;

	/* additions */
	extern XPopupMenuManager XPopups;
	extern XMenuBar XMenubar;
	
	void setupOSCompatibility(void);
	void freeOSCompatibility(void);

	IClassFactory* getClassFactory();
	const TCHAR * getLastError();
	BYTE getGhostDrag();
	bool setGhostDrag(const BYTE newAlpha);
	bool isDX9Installed();
	bool isUnloadSave();
	bool isFile(const WCHAR* file);
	bool isFile(LPCSTR file);

	void load(LOADINFO * lInfo);
	void unload(void);
	bool initDirectX();
	bool initDirectX(TCHAR *dxResult, int dxSize);
//#if DCX_DEBUG_OUTPUT
//	void debug(const TCHAR *cmd, const TCHAR *msg);
//#endif
	void error(const TCHAR *cmd, const TCHAR *msg);
	void errorex(const TCHAR *cmd, const TCHAR *szFormat, ...);
	int mark(TCHAR* data, const TString & tsDName, const TString & tsCallbackName);
	LRESULT CALLBACK mIRCSubClassWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}
