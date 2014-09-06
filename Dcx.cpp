#include "defines.h"
#include <ExDisp.h>
#include "Dcx.h"
#include "Classes/custom/divider.h"
#include "Classes/custom/DcxTrayIcon.h"

// is it just me or is this class just a namespace?

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

	void load(LOADINFO *const lInfo)
	{
		m_iGhostDrag = 255;
		m_bDX9Installed = false;
		m_pClassFactory = NULL;
		m_hRichEditLib = NULL;
		m_bErrorTriggered = false;

		// Initialize mIRCLinker
		mIRCLinker::load(lInfo);

		// Initializing OLE Support
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Initializing OLE Support..."));
		HRESULT hr = OleInitialize(NULL);
		if ((hr == OLE_E_WRONGCOMPOBJ) || (hr == RPC_E_CHANGED_MODE))
		{
			// init failed...
			DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("OLE Failed to init"));
		}
		// Initializing GDI
		GDIModule.load();

		UXModule.load();

		VistaModule.load();

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
	}

	void unload(void)
	{
		if (Dialogs.closeDialogs()) { // if unable to close dialogs stop unload.
			Dcx::error(TEXT("UnloadDll"), TEXT("Unable to Unload DLL from within the DLL"));
			//return 0; // NB: This DOESN'T stop the unload, & mIRC will still crash.
		}

		CloseUltraDock(); // UnDock All.

		UnregisterClass(DCX_PROGRESSBARCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_TREEVIEWCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_TOOLBARCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_STATUSBARCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_LISTVIEWCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_COMBOEXCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_TRACKBARCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_RICHEDITCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_REBARCTRLCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_COLORCOMBOCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_TABCTRLCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_UPDOWNCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_IPADDRESSCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_DIVIDERCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_PANELCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_BOXCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_BUTTONCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_CALENDARCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_DATETIMECLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_PAGERCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_SHADOWCLASS, GetModuleHandle(NULL));
		UnregisterClass(DCX_VISTACLASS, GetModuleHandle(NULL));

		// Class Factory of Web Control
		if (m_pClassFactory != NULL)
			m_pClassFactory->Release();

		// Terminating OLE Support
		OleUninitialize();


		/***** TrayIcon Stuff *****/
		delete trayIcons;
		trayIcons = NULL;

		// reset the treebars font if it's been changed.
		if (mIRCLinker::getTreeFont() != NULL) {
			HFONT hfont = GetWindowFont(mIRCLinker::getTreeview());
			if (hfont != mIRCLinker::getTreeFont()) {
				SetWindowFont(mIRCLinker::getTreeview(), mIRCLinker::getTreeFont(), TRUE);
				DeleteFont(hfont);
			}
		}

		/***** XMenuBar Stuff *****/
		XMenubar.resetMenuBar();

		if (m_hRichEditLib != NULL)
			FreeLibrary(m_hRichEditLib);

		XPopups.unload();

		freeOSCompatibility();
		mIRCLinker::unload();
	}

	void setupOSCompatibility(void) {

		WNDCLASSEX wc;
		ZeroMemory((void*)&wc, sizeof(WNDCLASSEX));
		wc.cbSize = sizeof(WNDCLASSEX);

#define dcxRegisterClass(szClass, szDcxClass) { \
	GetClassInfoEx(NULL, (TCHAR *)(szClass), &wc); \
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
		//InitDivider( GetModuleHandle( NULL ) );

		// Custom Divider
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Divider..."));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = 0;
		wc.lpfnWndProc = DividerWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hIcon = NULL;
		wc.hCursor = NULL;
		wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
		wc.lpszMenuName = 0;
		wc.lpszClassName = DCX_DIVIDERCLASS;
		wc.hIconSm = NULL;
		RegisterClassEx(&wc);

		//GetClassInfoEx( NULL, DIVIDERCLASSNAME, &wc );
		//wc.lpszClassName = ;
		//RegisterClassEx( &wc );

		/*
		// Custom Panel
		wc.cbSize			    = sizeof( WNDCLASSEX );
		wc.style		  	  = 0;
		wc.lpfnWndProc	  = DefWindowProc;
		wc.cbClsExtra		  = 0;
		wc.cbWndExtra		  = 0;
		wc.hInstance		  = GetModuleHandle( NULL );
		wc.hIcon			    = LoadCursor( GetModuleHandle( NULL ), IDC_ARROW );
		wc.hCursor			  = NULL;
		wc.hbrBackground	= NULL; //(HBRUSH)(COLOR_3DFACE+1);
		wc.lpszMenuName		= 0;
		wc.lpszClassName	= DCX_PANELCLASS;
		wc.hIconSm			  = NULL;
		RegisterClassEx( &wc );
		*/

		// Custom Panel
		//DCX_DEBUG(TEXT("LoadDLL"), TEXT("Registering Panel (#32770)..."));
		//GetClassInfoEx(NULL, TEXT("#32770"), &wc); // NB: using this class causes tooltips in toolbar children to not show
		//wc.lpszClassName = DCX_PANELCLASS;		// Also causes listview/treeview label edit events to fail.
		//RegisterClassEx(&wc);
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Panel..."));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_PARENTDC;
		wc.lpfnWndProc = DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(GetModuleHandle(NULL), IDC_ARROW);
		wc.hbrBackground = NULL; //(HBRUSH) (COLOR_3DFACE +1);
		wc.lpszMenuName = 0;
		wc.lpszClassName = DCX_PANELCLASS;
		wc.hIconSm = NULL;
		RegisterClassEx(&wc);
		//dcxRegisterClass(TEXT("STATIC"), DCX_PANELCLASS);

		// Custom Box
		//DCX_DEBUG(TEXT("LoadDLL"), TEXT("Registering Box (#32770)..."));
		//GetClassInfoEx(NULL, TEXT("#32770"), &wc); // NB: using this class causes tooltips in toolbar children to not show
		//wc.lpszClassName = DCX_BOXCLASS;			// Also causes listview/treeview label edit events to fail.
		//RegisterClassEx(&wc);
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Box..."));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_PARENTDC;
		wc.lpfnWndProc = DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(GetModuleHandle(NULL), IDC_ARROW);
		wc.hbrBackground = NULL; //(HBRUSH) (COLOR_3DFACE +1);
		wc.lpszMenuName = 0;
		wc.lpszClassName = DCX_BOXCLASS;
		wc.hIconSm = NULL;
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

		// Shadow Class
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Shadow..."));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = DCX_SHADOWCLASS;
		wc.hIconSm = NULL;
		RegisterClassEx(&wc);

		// Vista Dialog Class
		DCX_DEBUG(mIRCLinker::debug, TEXT("LoadDLL"), TEXT("Registering Vista Dialog..."));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = DCX_VISTACLASS;
		wc.hIconSm = NULL;
		RegisterClassEx(&wc);
	}

	void freeOSCompatibility(void)
	{
		GDIModule.unload();
		UXModule.unload();
		VistaModule.unload();
	}

	const TCHAR *getLastError()
	{
		return m_sLastError.to_chr();
	}

	IClassFactory *getClassFactory()
	{
		return m_pClassFactory;
	}

	const BYTE &getGhostDrag()
	{
		return m_iGhostDrag;
	}

	bool setGhostDrag(const BYTE newAlpha)
	{
		m_iGhostDrag = newAlpha;
		return true;
	}

	const bool &isDX9Installed()
	{
		return m_bDX9Installed;
	}

	bool isUnloadSafe()
	{
		return Dialogs.safeToCloseAll()/* && !XPopups.isPatched()*/;
	}

	const bool &initDirectX()
	{
		return initDirectX(NULL, 0);
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
		m_sLastError.tsprintf(TEXT("D_ERROR %s (%s)"), cmd, msg);

		if (m_bErrorTriggered)
			return;
		m_bErrorTriggered = true;

		if (mIRCLinker::m_bSendMessageDisabled)
		{
			m_sLastError.addtok(TEXT("SendMessage() disabled, re-enable this in mIRC's Lock Options"), TEXT("\r\n"));
			MessageBox(mIRCLinker::m_mIRCHWND, m_sLastError.to_chr(), NULL, MB_OK);
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
		temp.tvprintf(szFormat, &args);
		va_end(args);

		error(cmd, temp.to_chr());
	}

	int mark(TCHAR *const data, const TString & tsDName, const TString & tsCallbackName)
	{
		// check if the alias exists
		if (!mIRCLinker::isAlias(tsCallbackName.to_chr())) {
			errorex(TEXT("Mark"), TEXT("No such alias : %s"), tsCallbackName.to_chr());
			ret(getLastError());
		}

		// check if valid dialog
		HWND mHwnd = GetHwndFromString(tsDName.to_chr());

		if (IsWindow(mHwnd) == FALSE) {
			errorex(TEXT("Mark"), TEXT("Invalid Dialog Window : %s"), tsDName.to_chr());
			ret(getLastError());
		}

		if (Dialogs.getDialogByHandle(mHwnd) != NULL) {
			errorex(TEXT("Mark"), TEXT("Window Already Marked : %s"), tsDName.to_chr());
			ret(getLastError());
		}
		try {
			Dialogs.markDialog(mHwnd, tsDName, tsCallbackName);
		}
		catch (...)
		{
			ret(TEXT("D_FAIL Mark: Unable to Mark"));
		}
		{
			DcxDialog *pTmp = Dialogs.getDialogByHandle(mHwnd);
			if (pTmp != NULL) {
				TCHAR res[40];
				pTmp->evalAliasEx(res, 40, TEXT("isverbose,0"));

				if (lstrcmp(res, TEXT("$false")) == 0)
					pTmp->SetVerbose(false);
			}
		}
		ret(TEXT("D_OK Mark: Dialog Marked"));
	}

	/*!
	* \brief blah
	*
	* blah
	*/
	LRESULT CALLBACK mIRCSubClassWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_SIZE:
		{
						mIRCLinker::signalex(dcxSignal.xdock, TEXT("size mIRC %d %d %d"), mHwnd, LOWORD(lParam), HIWORD(lParam));
		}
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
							   LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;

							   if (lpmis->CtlType == ODT_MENU) {
								   //XPopupMenuItem *p_Item = (XPopupMenuItem*) lpmis->itemData;
								   XPopupMenuItem *p_Item = reinterpret_cast<XPopupMenuItem *>(lpmis->itemData);

								   if (p_Item != NULL) {
									   const SIZE size = p_Item->getItemSize(mHwnd);

									   lpmis->itemWidth = size.cx;
									   lpmis->itemHeight = size.cy;
									   return TRUE;
								   }
							   }

							   break;
		}

		case WM_DRAWITEM:
		{
							LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;

							if (lpdis->CtlType == ODT_MENU) {
								//XPopupMenuItem *p_Item = (XPopupMenuItem*)lpdis->itemData;
								XPopupMenuItem *p_Item = reinterpret_cast<XPopupMenuItem *>(lpdis->itemData);

								if (p_Item != NULL) {
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
									 const DWORD style = GetWindowExStyle(mIRCLinker::getHWND());
									 // Set WS_EX_LAYERED on this window
									 if (!(style & WS_EX_LAYERED))
										 SetWindowLongPtr(mIRCLinker::getHWND(), GWL_EXSTYLE, style | WS_EX_LAYERED);
									 // Make this window 75 alpha
									 SetLayeredWindowAttributes(mIRCLinker::getHWND(), 0, getGhostDrag(), LWA_ALPHA);
									 SetProp(mIRCLinker::getHWND(), TEXT("dcx_ghosted"), (HANDLE)1);
								 }
		}
			break;

		case WM_EXITSIZEMOVE:
		{
								// turn off ghosting.
								if (GetProp(mIRCLinker::getHWND(), TEXT("dcx_ghosted")) != NULL) {
									// Make this window solid
									SetLayeredWindowAttributes(mIRCLinker::getHWND(), 0, 255, LWA_ALPHA);
									RemoveProp(mIRCLinker::getHWND(), TEXT("dcx_ghosted"));
								}
								break;
		}

		case WM_DWMCOMPOSITIONCHANGED:
		{
										 Dcx::VistaModule.refreshComposite();
		}
			break;

		default:
			break;
		}

		return mIRCLinker::callDefaultWindowProc(mHwnd, uMsg, wParam, lParam);
	}
	bool isFile(const WCHAR *const file) {
		struct _stat64i32 stFileInfo;
		return (_wstat(file, &stFileInfo) == 0);
			// We were able to get the file attributes
			// so the file obviously exists.
	}
	bool isFile(LPCSTR const file) {
		struct stat stFileInfo;
		return (stat(file, &stFileInfo) == 0);
			// We were able to get the file attributes
			// so the file obviously exists.
	}
}