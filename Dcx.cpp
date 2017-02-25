#include "defines.h"
#include <ExDisp.h>
#include "Dcx.h"
#include "Classes/custom/divider.h"
#include "Classes/custom/DcxTrayIcon.h"

namespace Dcx {
	TString m_sLastError;
	IClassFactory * m_pClassFactory;
	DcxGDIModule GDIModule;
	DcxUXModule UXModule;
	DcxDWMModule VistaModule;
	DcxDialogCollection Dialogs;
	XPopupMenuManager XPopups;
	XMenuBar XMenubar;
	BYTE m_iGhostDrag;
	bool m_bDX9Installed;
	HMODULE m_hRichEditLib;
	bool m_bErrorTriggered;
	bool setting_bStaticColours;
	MapOfCursors	m_vMapOfCursors;
	MapOfAreas		m_vMapOfAreas;
	PFNSETCURSOR SetCursorUx = nullptr;
	TString dcxException::tsErr;


	void load(LOADINFO *const lInfo)
	{
		//Expects(lInfo != nullptr);

		m_iGhostDrag = 255;
		m_bDX9Installed = false;
		m_pClassFactory = nullptr;
		m_hRichEditLib = nullptr;
		m_bErrorTriggered = false;
		setting_bStaticColours = true;

		// Initialize mIRCLinker
		mIRCLinker::load(lInfo);

		// Initializing OLE Support
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Initializing OLE Support..."));
		auto hr = OleInitialize(nullptr);
		if ((hr == OLE_E_WRONGCOMPOBJ) || (hr == RPC_E_CHANGED_MODE))
		{
			// init failed...
			DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("OLE Failed to init"));
		}
		setupOSCompatibility();

		mIRCLinker::hookWindowProc(Dcx::mIRCSubClassWinProc);

		XPopups.load();

		//get IClassFactory* for WebBrowser
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Generating class for WebBrowser..."));
		if (FAILED(CoGetClassObject(CLSID_WebBrowser, CLSCTX_INPROC_SERVER, 0, IID_IClassFactory, (void**)&m_pClassFactory)))
		{
			// failed...
			DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Unable to get WebBrowser..."));
		}
		//6BF52A52-394A-11D3-B153-00C04F79FAA6
		//0x000006BA: The RPC server is unavailable.
		//Js::JavascriptExceptionObject
		// RichEdit DLL Loading
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Generating class for RichEdit..."));
		m_hRichEditLib = LoadLibrary(TEXT("RICHED20.DLL"));

		// Load Control definitions
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Loading control classes"));
		INITCOMMONCONTROLSEX icex;

		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_TREEVIEW_CLASSES | ICC_BAR_CLASSES | ICC_PROGRESS_CLASS | ICC_LISTVIEW_CLASSES |
			ICC_USEREX_CLASSES | ICC_COOL_CLASSES | ICC_STANDARD_CLASSES | ICC_UPDOWN_CLASS | ICC_DATE_CLASSES |
			ICC_TAB_CLASSES | ICC_INTERNET_CLASSES | ICC_PAGESCROLLER_CLASS;
		if (!InitCommonControlsEx(&icex))
		{
			DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Unable to init common controls"));
		}
		// Initialise signals of diff types
		dcxSignal.xdock = false;
		dcxSignal.xstatusbar = true;
		dcxSignal.xtray = true;
		//ReportLiveObjects();
		
		// Patch SetCursor() function for custom cursors
		SetCursorUx = (PFNSETCURSOR)PatchAPI("User32.dll", "SetCursor", Dcx::XSetCursor);
	}

	void unload(void)
	{
		if (Dialogs.closeDialogs()) { // if unable to close dialogs stop unload.
			Dcx::error(TEXT("UnloadDll"), TEXT("Unable to Unload DLL from within the DLL"));
			//return 0; // NB: This DOESN'T stop the unload, & mIRC will still crash.
		}

		CloseUltraDock(); // UnDock All.

		RemovePatch(SetCursorUx, Dcx::XSetCursor);

		UnregisterClass(DCX_PROGRESSBARCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_TREEVIEWCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_TOOLBARCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_STATUSBARCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_LISTVIEWCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_COMBOEXCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_TRACKBARCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_RICHEDITCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_REBARCTRLCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_COLORCOMBOCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_TABCTRLCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_UPDOWNCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_IPADDRESSCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_DIVIDERCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_PANELCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_BOXCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_BUTTONCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_CALENDARCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_DATETIMECLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_PAGERCLASS, GetModuleHandle(nullptr));
		//UnregisterClass(DCX_SHADOWCLASS, GetModuleHandle(nullptr));
		UnregisterClass(DCX_VISTACLASS, GetModuleHandle(nullptr));

		// Class Factory of Web Control
		if (m_pClassFactory != nullptr)
			m_pClassFactory->Release();

		// Terminating OLE Support
		OleUninitialize();


		/***** TrayIcon Stuff *****/
		delete trayIcons;
		trayIcons = nullptr;

		// reset the treebars font if it's been changed.
		if (mIRCLinker::getTreeFont() != nullptr) {
			auto hfont = GetWindowFont(mIRCLinker::getTreeview());
			if (hfont != mIRCLinker::getTreeFont()) {
				SetWindowFont(mIRCLinker::getTreeview(), mIRCLinker::getTreeFont(), TRUE);
				DeleteFont(hfont);
			}
		}

		/***** XMenuBar Stuff *****/
		XMenubar.resetMenuBar();

		if (m_hRichEditLib != nullptr)
			FreeLibrary(m_hRichEditLib);

		XPopups.unload();

		// free up custom cursors data.
		for (const auto &cd : m_vMapOfCursors)
		{
			DestroyCursor(cd.second);
		}
		m_vMapOfCursors.clear();
		for (const auto &cd : m_vMapOfAreas)
		{
			DestroyCursor(cd.second);
		}
		m_vMapOfAreas.clear();

		freeOSCompatibility();
		mIRCLinker::unload();
	}

	void setupOSCompatibility(void)
	{
		// Initializing GDI
		GDIModule.load();

		UXModule.load();

		VistaModule.load();

		WNDCLASSEX wc;
		ZeroMemory((void*)&wc, sizeof(WNDCLASSEX));
		wc.cbSize = sizeof(WNDCLASSEX);

#define dcxRegisterClass(szClass, szDcxClass) { \
	GetClassInfoEx(nullptr, (TCHAR *)(szClass), &wc); \
	wc.lpszClassName = (TCHAR *)(szDcxClass); \
	RegisterClassEx(&wc); \
		};

		// Custom ProgressBar
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ProgressBar..."));
		dcxRegisterClass(PROGRESS_CLASS, DCX_PROGRESSBARCLASS);

		// Custom TreeView
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering TreeView..."));
		dcxRegisterClass(WC_TREEVIEW, DCX_TREEVIEWCLASS);

		// Custom Toolbar
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ToolBar..."));
		dcxRegisterClass(TOOLBARCLASSNAME, DCX_TOOLBARCLASS);

		// Custom StatusBar
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering StatusBar..."));
		dcxRegisterClass(STATUSCLASSNAME, DCX_STATUSBARCLASS);

		// Custom ListView
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Listview..."));
		dcxRegisterClass(WC_LISTVIEW, DCX_LISTVIEWCLASS);

		// Custom ComboEx
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ComboEx..."));
		dcxRegisterClass(WC_COMBOBOXEX, DCX_COMBOEXCLASS);

		// Custom TrackBar
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering TrackBar..."));
		dcxRegisterClass(TRACKBAR_CLASS, DCX_TRACKBARCLASS);

		// Custom RichEdit
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering RichEdit..."));
		dcxRegisterClass(RICHEDIT_CLASS, DCX_RICHEDITCLASS);

		// Custom RebarCtrl
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ReBar..."));
		dcxRegisterClass(REBARCLASSNAME, DCX_REBARCTRLCLASS);

		// Custom Color Combo
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering ComboBox..."));
		dcxRegisterClass(TEXT("COMBOBOX"), DCX_COLORCOMBOCLASS);

		// Custom TabCtrl
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Tab..."));
		dcxRegisterClass(WC_TABCONTROL, DCX_TABCTRLCLASS);

		// Custom UpDown
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering UpDown..."));
		dcxRegisterClass(UPDOWN_CLASS, DCX_UPDOWNCLASS);

		// Custom IpAddress
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering IpAddress..."));
		dcxRegisterClass(WC_IPADDRESS, DCX_IPADDRESSCLASS);

		// Init Divider Control
		//InitDivider( GetModuleHandle( nullptr ) );

		// Custom Divider
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Divider..."));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = 0;
		wc.lpfnWndProc = DividerWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.hIcon = nullptr;
		wc.hCursor = nullptr;
		wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
		wc.lpszMenuName = 0;
		wc.lpszClassName = DCX_DIVIDERCLASS;
		wc.hIconSm = nullptr;
		RegisterClassEx(&wc);

		//GetClassInfoEx( nullptr, DIVIDERCLASSNAME, &wc );
		//wc.lpszClassName = ;
		//RegisterClassEx( &wc );

		/*
		// Custom Panel
		wc.cbSize			    = sizeof( WNDCLASSEX );
		wc.style		  	  = 0;
		wc.lpfnWndProc	  = DefWindowProc;
		wc.cbClsExtra		  = 0;
		wc.cbWndExtra		  = 0;
		wc.hInstance		  = GetModuleHandle( nullptr );
		wc.hIcon			    = LoadCursor( GetModuleHandle( nullptr ), IDC_ARROW );
		wc.hCursor			  = nullptr;
		wc.hbrBackground	= nullptr; //(HBRUSH)(COLOR_3DFACE+1);
		wc.lpszMenuName		= 0;
		wc.lpszClassName	= DCX_PANELCLASS;
		wc.hIconSm			  = nullptr;
		RegisterClassEx( &wc );
		*/

		// Custom Panel
		//DCX_DEBUG(TEXT("LoadDLL"), TEXT("Registering Panel (#32770)..."));
		//GetClassInfoEx(nullptr, TEXT("#32770"), &wc); // NB: using this class causes tooltips in toolbar children to not show
		//wc.lpszClassName = DCX_PANELCLASS;		// Also causes listview/treeview label edit events to fail.
		//RegisterClassEx(&wc);
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Panel..."));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_PARENTDC;
		wc.lpfnWndProc = DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.hIcon = nullptr;
		wc.hCursor = LoadCursor(GetModuleHandle(nullptr), IDC_ARROW);
		wc.hbrBackground = nullptr; //(HBRUSH) (COLOR_3DFACE +1);
		wc.lpszMenuName = 0;
		wc.lpszClassName = DCX_PANELCLASS;
		wc.hIconSm = nullptr;
		RegisterClassEx(&wc);
		//dcxRegisterClass(TEXT("STATIC"), DCX_PANELCLASS);

		// Custom Box
		//DCX_DEBUG(TEXT("LoadDLL"), TEXT("Registering Box (#32770)..."));
		//GetClassInfoEx(nullptr, TEXT("#32770"), &wc); // NB: using this class causes tooltips in toolbar children to not show
		//wc.lpszClassName = DCX_BOXCLASS;			// Also causes listview/treeview label edit events to fail.
		//RegisterClassEx(&wc);
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Box..."));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_PARENTDC;
		wc.lpfnWndProc = DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.hIcon = nullptr;
		wc.hCursor = LoadCursor(GetModuleHandle(nullptr), IDC_ARROW);
		wc.hbrBackground = nullptr; //(HBRUSH) (COLOR_3DFACE +1);
		wc.lpszMenuName = 0;
		wc.lpszClassName = DCX_BOXCLASS;
		wc.hIconSm = nullptr;
		RegisterClassEx(&wc);

		// Custom Button
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Button..."));
		dcxRegisterClass(TEXT("BUTTON"), DCX_BUTTONCLASS);

		// Custom Calendar
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Calendar..."));
		dcxRegisterClass(MONTHCAL_CLASS, DCX_CALENDARCLASS);

		// Custom DateTime
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering DateTime..."));
		dcxRegisterClass(DATETIMEPICK_CLASS, DCX_DATETIMECLASS);

		// Custom Pager
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Pager..."));
		dcxRegisterClass(WC_PAGESCROLLER, DCX_PAGERCLASS);

		//// Shadow Class
		//DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Shadow..."));
		//wc.cbSize = sizeof(WNDCLASSEX);
		//wc.style = CS_HREDRAW | CS_VREDRAW;
		//wc.lpfnWndProc = DefWindowProc;
		//wc.cbClsExtra = 0;
		//wc.cbWndExtra = 0;
		//wc.hInstance = GetModuleHandle(nullptr);
		//wc.hIcon = nullptr;
		//wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		//wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		//wc.lpszMenuName = nullptr;
		//wc.lpszClassName = DCX_SHADOWCLASS;
		//wc.hIconSm = nullptr;
		//RegisterClassEx(&wc);

		// Vista Dialog Class
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Vista Dialog..."));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.hIcon = nullptr;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = DCX_VISTACLASS;
		wc.hIconSm = nullptr;
		RegisterClassEx(&wc);
	}

	void freeOSCompatibility(void)
	{
		VistaModule.unload();
		UXModule.unload();
		GDIModule.unload();
	}

	const TCHAR *getLastError() noexcept
	{
		return m_sLastError.to_chr();
	}

	IClassFactory *getClassFactory() noexcept
	{
		return m_pClassFactory;
	}

	const BYTE &getGhostDrag() noexcept
	{
		return m_iGhostDrag;
	}

	bool setGhostDrag(const BYTE newAlpha)
	{
		m_iGhostDrag = newAlpha;
		return true;
	}

	const bool &isDX9Installed() noexcept
	{
		return m_bDX9Installed;
	}

	bool isUnloadSafe()
	{
		return Dialogs.safeToCloseAll()/* && !XPopups.isPatched()*/;
	}

	const bool &initDirectX()
	{
		return initDirectX(nullptr, 0);
	}

	const bool &initDirectX(TCHAR *dxResult, int dxSize)
	{
#ifdef DCX_USE_DXSDK
		DCX_DEBUG(mIRCLinker::debug, TEXT("DXSetup"), TEXT("Checking DirectX Version..."));
		DWORD dx_ver = 0;
		if (GetDXVersion(&dx_ver, dxResult, dxSize) == S_OK) {
			if (dx_ver < 0x00090000) {
				DCX_DEBUG(mIRCLinker::debug, TEXT("DXSetup"), TEXT("Got DirectX Version: Need V9+"));
				m_bDX9Installed = false;
			}
			else {
				DCX_DEBUG(mIRCLinker::debug, TEXT("DXSetup"), TEXT("Got DirectX Version: V9+ Installed"));
				m_bDX9Installed = true;
			}
		}
		else {
			DCX_DEBUG(mIRCLinker::debug, TEXT("DXSetup"), TEXT("Couldn't Get DirectX Version"));
			m_bDX9Installed = false;
		}
#endif
		return m_bDX9Installed;
	}

	/*!
	 * \brief Sends an error message to mIRC.
	 */
	void error(const TCHAR *const cmd, const TCHAR *const msg)
	{
		if (m_bErrorTriggered)
			return;

		//m_sLastError.tsprintf(TEXT("D_ERROR %s (%s)"), cmd, msg);

		_ts_sprintf(m_sLastError, TEXT("D_ERROR % (%)"), cmd, msg);

		m_bErrorTriggered = true;

		if (mIRCLinker::m_bSendMessageDisabled)
		{
			m_sLastError.addtok(TEXT("SendMessage() disabled, re-enable this in mIRC's Lock Options"), TEXT("\r\n"));
			MessageBox(mIRCLinker::m_mIRCHWND, m_sLastError.to_chr(), nullptr, MB_OK);
		}
		else
			mIRCLinker::echo(m_sLastError.to_chr());

		m_bErrorTriggered = false;
	}

	/*
	 * Variable argument error message.
	 */
	void errorex(const TCHAR *const cmd, const TCHAR *const szFormat, ...)
	{
		TString temp;
		va_list args;

		va_start(args, szFormat);
		temp.tvprintf(szFormat, args);
		va_end(args);

		error(cmd, temp.to_chr());
	}

	//int mark(TCHAR *const data, const TString & tsDName, const TString & tsCallbackName)
	//{
	//	// check if the alias exists
	//	if (!mIRCLinker::isAlias(tsCallbackName))
	//		throw Dcx::dcxException(TEXT("No such alias : %"), tsCallbackName);
	//
	//	// check if valid dialog
	//	auto mHwnd = GetHwndFromString(tsDName);
	//
	//	if (!IsWindow(mHwnd))
	//		throw Dcx::dcxException(TEXT("Invalid Dialog Window : %"), tsDName);
	//
	//	if (Dialogs.getDialogByHandle(mHwnd) != nullptr)
	//		throw Dcx::dcxException(TEXT("Window Already Marked : %"), tsDName);
	//
	//	Dialogs.markDialog(mHwnd, tsDName, tsCallbackName);
	//	{
	//		auto pTmp = Dialogs.getDialogByHandle(mHwnd);
	//		if (pTmp != nullptr) {
	//			//TCHAR res[40];
	//			//pTmp->evalAliasEx(res, countof(res), TEXT("isverbose,0"));
	//			//
	//			//if (ts_strcmp(res, TEXT("$false")) == 0)
	//			//	pTmp->SetVerbose(false);
	//
	//			//if (pTmp->evalAliasEx(nullptr, 0, TEXT("isverbose,0")))
	//			//	pTmp->SetVerbose(false);
	//
	//			
	//			pTmp->SetVerbose(pTmp->evalAliasEx(nullptr, 0, TEXT("isverbose,0")));
	//		}
	//	}
	//	ret(TEXT("D_OK Mark: Dialog Marked"));
	//}

	int mark(const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &data, const TString & tsDName, const TString & tsCallbackName)
	{
		// check if the alias exists
		if (!mIRCLinker::isAlias(tsCallbackName))
			throw Dcx::dcxException(TEXT("No such alias : %"), tsCallbackName);

		// check if valid dialog
		auto mHwnd = GetHwndFromString(tsDName);

		if (!IsWindow(mHwnd))
			throw Dcx::dcxException(TEXT("Invalid Dialog Window : %"), tsDName);

		if (Dialogs.getDialogByHandle(mHwnd) != nullptr)
			throw Dcx::dcxException(TEXT("Window Already Marked : %"), tsDName);

		Dialogs.markDialog(mHwnd, tsDName, tsCallbackName);
		{
			auto pTmp = Dialogs.getDialogByHandle(mHwnd);
			if (pTmp != nullptr) {
				pTmp->SetVerbose(pTmp->evalAliasEx(nullptr, 0, TEXT("isverbose,0")));
			}
		}
		data = TEXT("D_OK Mark: Dialog Marked");
		return 3;
	}

	/*!
	* \brief blah
	*
	* blah
	*/
	LRESULT CALLBACK mIRCSubClassWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_SIZE:
			mIRCLinker::signalex(dcxSignal.xdock, TEXT("size mIRC %d %d %d"), mHwnd, LOWORD(lParam), HIWORD(lParam));
			break;

			//		case WM_SYSCOMMAND:
			//			{
			//#define SC_SHOWSYSMENU 0xF093
			//				if ((wParam & 0xFFF0) == SC_SHOWSYSMENU) {
			//					GetSystemMenu(mHwnd,TRUE);
			//				}
			//			}
			//			break;

			//case WM_CONTEXTMENU:
			//case WM_INITMENU:
		case WM_INITMENUPOPUP:
			return Dcx::XPopups.OnInitMenuPopup(mHwnd, wParam, lParam);

		case WM_COMMAND:
			return Dcx::XPopups.OnCommand(mHwnd, wParam, lParam);

		case WM_EXITMENULOOP:
			return Dcx::XPopups.OnExitMenuLoop(mHwnd, wParam, lParam);

		case WM_UNINITMENUPOPUP:
			return Dcx::XPopups.OnUninitMenuPopup(mHwnd, wParam, lParam);

		case WM_MEASUREITEM:
		{
			dcxlParam(LPMEASUREITEMSTRUCT, lpmis);

			if (lpmis->CtlType == ODT_MENU) {
				auto p_Item = reinterpret_cast<XPopupMenuItem *>(lpmis->itemData);

				if (p_Item != nullptr) {
					const auto size = p_Item->getItemSize(mHwnd);

					lpmis->itemWidth = (UINT)size.cx;
					lpmis->itemHeight = (UINT)size.cy;
					return TRUE;
				}
			}

			break;
		}

		case WM_DRAWITEM:
		{
			dcxlParam(LPDRAWITEMSTRUCT, lpdis);

			if (lpdis->CtlType == ODT_MENU) {
				auto p_Item = reinterpret_cast<XPopupMenuItem *>(lpdis->itemData);

				if (p_Item != nullptr) {
					p_Item->DrawItem(lpdis);
					return TRUE;
				}
			}

			break;
		}

			// This msg will cause the close request dialog to mess up mirc.
			//case WM_CLOSE:
			//{
			//	mIRCLinker::resetWindowProc();
			//	PostMessage(mHwnd, uMsg, 0, 0);
			//	return 0L;
			//}

			// ghost drag stuff
		case WM_ENTERSIZEMOVE:
		{
			if (getGhostDrag() < 255) {
				const auto style = GetWindowExStyle(mIRCLinker::getHWND());
				// Set WS_EX_LAYERED on this window
				if (!dcx_testflag(style, WS_EX_LAYERED))
					SetWindowLongPtr(mIRCLinker::getHWND(), GWL_EXSTYLE, (LONG)(style | WS_EX_LAYERED));
				// Make this window 75 alpha
				SetLayeredWindowAttributes(mIRCLinker::getHWND(), 0, getGhostDrag(), LWA_ALPHA);
				SetProp(mIRCLinker::getHWND(), TEXT("dcx_ghosted"), (HANDLE)1);
			}
		}
			break;

		case WM_EXITSIZEMOVE:
		{
			// turn off ghosting.
			if (GetProp(mIRCLinker::getHWND(), TEXT("dcx_ghosted")) != nullptr) {
				// Make this window solid
				SetLayeredWindowAttributes(mIRCLinker::getHWND(), 0, 255, LWA_ALPHA);
				RemoveProp(mIRCLinker::getHWND(), TEXT("dcx_ghosted"));
			}
			break;
		}

		case WM_DWMCOMPOSITIONCHANGED:
		{
			Dcx::VistaModule.refreshComposite();
			break;
		}

			// Ook: TODO: add custom cursors!
			// mostly done, needs more work to handle child windows in mirc.

		//case WM_MOUSEMOVE:
		//{
		//	if (SetCursorUx == nullptr)
		//		break;
		//
		//	ShowCursor(FALSE);
		//	Auto(ShowCursor(TRUE));
		//
		//	auto lRes = mIRCLinker::callDefaultWindowProc(mHwnd, uMsg, wParam, lParam);
		//	auto hCursor = SystemToCustomCursor(GetCursor());
		//	if (hCursor != nullptr)
		//	{
		//		SetCursorUx(hCursor);
		//		return 0;
		//	}
		//
		//	return lRes;
		//}

		case WM_SETCURSOR:
		{
			if (SetCursorUx == nullptr)
				break;

			const auto iType = (UINT)LOWORD(lParam);
			auto hCursor = AreaToCustomCursor(iType);
			if (hCursor != nullptr)
			{
				SetCursorUx(hCursor);
				return TRUE;
			}

			ShowCursor(FALSE);
			Auto(ShowCursor(TRUE));

			auto lRes = mIRCLinker::callDefaultWindowProc(mHwnd, uMsg, wParam, lParam);
			hCursor = SystemToCustomCursor(GetCursor());
			if (hCursor != nullptr)
			{
				SetCursorUx(hCursor);
				return TRUE;
			}
			return lRes;

			//const auto iType = (UINT)LOWORD(lParam);
			//const auto hCursor = getCursor(iType);
			//
			//if (hCursor != nullptr)
			//{
			//	if (GetCursor() != hCursor)
			//		SetCursor(hCursor);
			//	return TRUE;
			//}
			//
			//const auto iType = (UINT)LOWORD(lParam);
			//auto hChild = (HWND)wParam;
			//
			//TString tsClass((UINT)32);
			//if (GetClassName(hChild, tsClass.to_chr(), 32) != 0)
			//{
			//	HCURSOR hCursor = nullptr;
			//	if (tsClass == TEXT("mIRC"))
			//	{
			//			hCursor = getCursor(IDC_ARROW);
			//	}
			//	else if (tsClass == TEXT("RichEdit20W"))
			//	{
			//		if (iType == HTCLIENT)
			//			hCursor = getCursor(IDC_IBEAM);
			//		else
			//			hCursor = getCursor(IDC_ARROW);
			//	}
			//	else if (tsClass == TEXT("mIRC_Toolbar"))
			//		hCursor = getCursor(IDC_ARROW);
			//	else if (tsClass == TEXT("mIRC_Treebar"))
			//		hCursor = getCursor(IDC_ARROW);
			//	else if (tsClass == TEXT("mIRC_Switchbar"))
			//		hCursor = getCursor(IDC_ARROW);
			//	else if (tsClass == TEXT("mIRC_Status"))
			//		hCursor = getCursor(IDC_ARROW);
			//	else if (tsClass == TEXT("SysTreeView32"))
			//		hCursor = getCursor(IDC_ARROW);
			//	else if (tsClass == TEXT("MDIClient"))
			//		hCursor = getCursor(IDC_ARROW);
			//	else if (tsClass == TEXT("ScrollBar"))
			//		hCursor = getCursor(IDC_ARROW);
			//
			//	if (hCursor != nullptr)
			//	{
			//		if (GetCursor() != hCursor)
			//			SetCursor(hCursor);
			//		return TRUE;
			//	}
			//}
			break;
		}

		default:
			break;
		}

		return mIRCLinker::callDefaultWindowProc(mHwnd, uMsg, wParam, lParam);
	}

	//// test if a file exists
	//bool isFile(const WCHAR *const file) {
	//	struct _stat64i32 stFileInfo;
	//	return (_wstat(file, &stFileInfo) == 0);
	//		// We were able to get the file attributes
	//		// so the file obviously exists.
	//}

	//// test if a file exists
	//bool isFile(LPCSTR const file) {
	//	struct stat stFileInfo;
	//	return (stat(file, &stFileInfo) == 0);
	//		// We were able to get the file attributes
	//		// so the file obviously exists.
	//}

	// Generate a formatted error string for an exception
	const char *const dcxGetFormattedString(const TCHAR *const fmt, ...)
	{
		static TString tsErr;

		va_list args;
		va_start(args, fmt);
		tsErr.tvprintf(fmt, args);
		va_end(args);

		return tsErr.c_str();
	}

	// convert a cursor name into a resource number.
	const PTCHAR parseCursorType(const TString & cursor)
	{
		static std::map<TString, PTCHAR> IDC_map;

		if (IDC_map.empty()) {
			IDC_map[TEXT("appstarting")] = IDC_APPSTARTING;
			IDC_map[TEXT("arrow")] = IDC_ARROW;
			IDC_map[TEXT("cross")] = IDC_CROSS;
			IDC_map[TEXT("hand")] = IDC_HAND;
			IDC_map[TEXT("help")] = IDC_HELP;
			IDC_map[TEXT("ibeam")] = IDC_IBEAM;
			IDC_map[TEXT("no")] = IDC_NO;
			IDC_map[TEXT("sizeall")] = IDC_SIZEALL;
			IDC_map[TEXT("sizenesw")] = IDC_SIZENESW;
			IDC_map[TEXT("sizens")] = IDC_SIZENS;
			IDC_map[TEXT("sizenwse")] = IDC_SIZENWSE;
			IDC_map[TEXT("sizewe")] = IDC_SIZEWE;
			IDC_map[TEXT("uparrow")] = IDC_UPARROW;
			IDC_map[TEXT("wait")] = IDC_WAIT;
		}

		auto got = IDC_map.find(cursor);

		if (got != IDC_map.end())
			return got->second;

		return nullptr;
	}

	// convert a cursor name into a system resource number.
	const DWORD parseSystemCursorType(const TString & cursor)
	{
		static std::map<TString, DWORD> IDC_SystemMap;
		if (IDC_SystemMap.empty()) {
			IDC_SystemMap[TEXT("appstarting")] = OCR_APPSTARTING;
			IDC_SystemMap[TEXT("normal")] = OCR_NORMAL;
			IDC_SystemMap[TEXT("cross")] = OCR_CROSS;
			IDC_SystemMap[TEXT("hand")] = OCR_HAND;
			//IDC_SystemMap[TEXT("help")] = OCR_HELP;
			IDC_SystemMap[TEXT("ibeam")] = OCR_IBEAM;
			IDC_SystemMap[TEXT("no")] = OCR_NO;
			IDC_SystemMap[TEXT("sizeall")] = OCR_SIZEALL;
			IDC_SystemMap[TEXT("sizenesw")] = OCR_SIZENESW;
			IDC_SystemMap[TEXT("sizens")] = OCR_SIZENS;
			IDC_SystemMap[TEXT("sizenwse")] = OCR_SIZENWSE;
			IDC_SystemMap[TEXT("sizewe")] = OCR_SIZEWE;
			//IDC_SystemMap[TEXT("uparrow")] = OCR_UPARROW;
			IDC_SystemMap[TEXT("wait")] = OCR_WAIT;
		}

		auto got = IDC_SystemMap.find(cursor);

		if (got != IDC_SystemMap.end())
			return got->second;
		return 0;
	}

	// convert an area name into a hit zone
	const DWORD parseAreaType(const TString &tsArea)
	{
		static std::map<TString, DWORD> mIRC_AreaMap;
		if (mIRC_AreaMap.empty()) {
			mIRC_AreaMap[TEXT("client")] = HTCLIENT;
			//mIRC_AreaMap[TEXT("nowhere")] = HTNOWHERE;
			mIRC_AreaMap[TEXT("caption")] = HTCAPTION;
			mIRC_AreaMap[TEXT("sysmenu")] = HTSYSMENU;
			mIRC_AreaMap[TEXT("size")] = HTGROWBOX;
			mIRC_AreaMap[TEXT("menu")] = HTMENU;
			mIRC_AreaMap[TEXT("vscroll")] = HTVSCROLL;
			mIRC_AreaMap[TEXT("help")] = HTHELP;
			mIRC_AreaMap[TEXT("hscroll")] = HTHSCROLL;
			mIRC_AreaMap[TEXT("min")] = HTMINBUTTON;
			mIRC_AreaMap[TEXT("max")] = HTMAXBUTTON;
			mIRC_AreaMap[TEXT("left")] = HTLEFT;
			mIRC_AreaMap[TEXT("right")] = HTRIGHT;
			mIRC_AreaMap[TEXT("top")] = HTTOP;
			mIRC_AreaMap[TEXT("topleft")] = HTTOPLEFT;
			mIRC_AreaMap[TEXT("topright")] = HTTOPRIGHT;
			mIRC_AreaMap[TEXT("bottom")] = HTBOTTOM;
			mIRC_AreaMap[TEXT("bottomleft")] = HTBOTTOMLEFT;
			mIRC_AreaMap[TEXT("bottomright")] = HTBOTTOMRIGHT;
			mIRC_AreaMap[TEXT("border")] = HTBORDER;
			mIRC_AreaMap[TEXT("close")] = HTCLOSE;
		}

		auto got = mIRC_AreaMap.find(tsArea);

		if (got != mIRC_AreaMap.end())
			return got->second;

		return 0;
	}

	// load a cursor from a file
	// throws a runtime_error on fail.
	HCURSOR dcxLoadCursorFromFile(const TString &filename)
	{
		auto hCursor = (HCURSOR)LoadImage(nullptr, filename.to_chr(), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		if (hCursor == nullptr)
			throw Dcx::dcxException("Unable to load cursor file");
		return hCursor;
	}

	// load a cursor from a resource
	// throws a runtime_error on fail.
	HCURSOR dcxLoadCursorFromResource(const PTCHAR CursorType)
	{
		auto hCursor = (HCURSOR)LoadImage(nullptr, CursorType, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		if (hCursor == nullptr)
			throw Dcx::dcxException("Unable to load cursor resource");
		return hCursor;
	}

	// load a cursor from a file or resource, depending on flags
	// throws a runtime_error on fail.
	// throws an invalid_argument on failure to access file.
	HCURSOR dcxLoadCursor(const UINT iFlags, const PTCHAR CursorType, bool &bCursorFromFile, const HCURSOR oldCursor, TString &filename)
	{
		HCURSOR hCursor = nullptr;
		HCURSOR newCursor = nullptr;

		if (bCursorFromFile)
			hCursor = oldCursor;

		bCursorFromFile = false;

		if (dcx_testflag(iFlags, DCCS_FROMRESSOURCE))
			newCursor = dcxLoadCursorFromResource(CursorType);
		else if (dcx_testflag(iFlags, DCCS_FROMFILE)) {
			if (!IsFile(filename))
				throw Dcx::dcxException(TEXT("Unable to Access File: %"), filename);

			newCursor = Dcx::dcxLoadCursorFromFile(filename);

			bCursorFromFile = true;
		}
		if (newCursor == nullptr)
			throw Dcx::dcxException("Unable to Load Cursor");

		if (GetCursor() == hCursor)
			SetCursor(newCursor);

		if (hCursor != nullptr)
			DestroyCursor(hCursor);

		return newCursor;
	}

	// sets custom cursor to use for a specific area
	void setAreaCursor(const HCURSOR hCursor, const UINT iType)
	{
		deleteAreaCursor(iType);
		m_vMapOfAreas[iType] = hCursor;
	}

	// delete custom cursor for specific area.
	void deleteAreaCursor(const UINT iType)
	{
		auto it = m_vMapOfAreas.find(iType);
		if (it != m_vMapOfAreas.end())
		{
			DestroyCursor(it->second);
			m_vMapOfAreas.erase(it);
		}
	}

	// get custom cursor to use in place of specific system cursor.
	HCURSOR SystemToCustomCursor(const HCURSOR hCursor)
	{
		auto it = m_vMapOfCursors.find(hCursor);
		if (it != m_vMapOfCursors.end())
			return it->second;

		return nullptr;
	}

	// get the custom cursor to use with a specific area
	HCURSOR AreaToCustomCursor(const UINT iType)
	{
		auto it = m_vMapOfAreas.find(iType);
		if (it != m_vMapOfAreas.end())
			return it->second;

		return nullptr;
	}

	// set the custom cursor to use in place of a specific system cursor.
	void setCursor(const HCURSOR hSystemCursor, const HCURSOR hCustomCursor)
	{
		deleteCursor(hSystemCursor);
		m_vMapOfCursors[hSystemCursor] = hCustomCursor;
	}

	// delete a custom cursor.
	void deleteCursor(const HCURSOR hCursor)
	{
		auto it = m_vMapOfCursors.find(hCursor);
		if (it != m_vMapOfCursors.end())
		{
			DestroyCursor(it->second);
			m_vMapOfCursors.erase(it);
		}
	}

	// patch a dll function using detours
	PVOID PatchAPI(const char *const c_szDllName, const char *const c_szApiName, PVOID newfPtr)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		PVOID fPtr = DetourFindFunction(c_szDllName, c_szApiName);
		if (fPtr != nullptr)
		{
			DetourAttach(&fPtr, newfPtr);
			DetourTransactionCommit();
		}
		else
			DetourTransactionAbort();

		return fPtr;
	}

	// remove a detours patch
	void RemovePatch(PVOID fPtr, PVOID newfPtr)
	{
		if (fPtr == nullptr || newfPtr == nullptr)
			return;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&fPtr, newfPtr);
		DetourTransactionCommit();
	}

	// patch function for use in custom cursors
	HCURSOR WINAPI XSetCursor(HCURSOR hCursor)
	{
		if (hCursor == nullptr)
			return SetCursorUx(hCursor);

		auto hTemp = SystemToCustomCursor(hCursor);
		if (hTemp == nullptr)
			hTemp = hCursor;
		return SetCursorUx(hTemp);
	}
}
