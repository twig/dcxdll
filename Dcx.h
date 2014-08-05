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
	extern HMODULE m_hRichEditLib;

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
	const BYTE &getGhostDrag();
	bool setGhostDrag(const BYTE newAlpha);
	const bool &isDX9Installed();
	bool isUnloadSafe();
	bool isFile(const WCHAR *const file);
	bool isFile(LPCSTR const file);

	void load(LOADINFO *const lInfo);
	void unload(void);
	const bool &initDirectX();
	const bool &initDirectX(TCHAR *dxResult, int dxSize);
	void error(const TCHAR *const cmd, const TCHAR *const msg);
	void errorex(const TCHAR *const cmd, const TCHAR *const szFormat, ...);
	int mark(TCHAR *const data, const TString & tsDName, const TString & tsCallbackName);
	LRESULT CALLBACK mIRCSubClassWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}
