#include "defines.h"
#include <ExDisp.h>
#include "Dcx.h"
#include "Classes/custom/divider.h"
#include "Classes/custom/DcxTrayIcon.h"

// is it just me or is this class just a namespace?

mIRCLinker Dcx::mIRC;
TString Dcx::m_sLastError;
IClassFactory * Dcx::m_pClassFactory;
DcxGDIModule Dcx::GDIModule;
DcxUXModule Dcx::UXModule;
DcxDWMModule Dcx::VistaModule;
DcxDialogCollection Dcx::Dialogs;
XPopupMenuManager Dcx::XPopups;
XMenuBar Dcx::XMenubar;
BYTE Dcx::m_iGhostDrag = 255;
bool Dcx::m_bDX9Installed;

void Dcx::load(LOADINFO * lInfo)
{
	m_iGhostDrag = 255;
	m_bDX9Installed = false;

	// Initialize mIRCLinker
	mIRC.load(lInfo);

	// Initializing OLE Support
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Initializing OLE Support..."));
	OleInitialize(NULL);

	// Initializing GDI
	GDIModule.load(mIRC);

	UXModule.load(mIRC);

	VistaModule.load(mIRC);

	setupOSCompatibility();

	mIRC.hookWindowProc(Dcx::mIRCSubClassWinProc);	

	XPopups.load();

	//get IClassFactory* for WebBrowser
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Generating class for WebBrowser..."));
	CoGetClassObject(CLSID_WebBrowser, CLSCTX_INPROC_SERVER, 0, IID_IClassFactory, (void**) &m_pClassFactory);
	//6BF52A52-394A-11D3-B153-00C04F79FAA6

	// RichEdit DLL Loading
	DCX_DEBUG(mIRC.debug,TEXT("LoadDLL"), TEXT("Generating class for RichEdit..."));
	LoadLibrary(TEXT("RICHED20.DLL"));

	// Load Control definitions
	DCX_DEBUG(mIRC.debug,TEXT("LoadDLL"), TEXT("Loading control classes"));
	INITCOMMONCONTROLSEX icex;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_TREEVIEW_CLASSES | ICC_BAR_CLASSES | ICC_PROGRESS_CLASS | ICC_LISTVIEW_CLASSES | 
		ICC_USEREX_CLASSES | ICC_COOL_CLASSES | ICC_STANDARD_CLASSES | ICC_UPDOWN_CLASS | ICC_DATE_CLASSES | 
		ICC_TAB_CLASSES | ICC_INTERNET_CLASSES | ICC_PAGESCROLLER_CLASS;
	InitCommonControlsEx( &icex ); 
	// Initialise signals of diff types
	dcxSignal.xdock = false;
	dcxSignal.xstatusbar = true;
	dcxSignal.xtray = true;
}

void Dcx::unload(void)
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
	if (trayIcons != NULL) {
		delete trayIcons;
		trayIcons = NULL;
	}
	
	// reset the treebars font if it's been changed.
	if (Dcx::mIRC.getTreeFont() != NULL) {
		HFONT hfont = GetWindowFont(Dcx::mIRC.getTreeview());
		if (hfont != Dcx::mIRC.getTreeFont()) {
			SetWindowFont( Dcx::mIRC.getTreeview(), Dcx::mIRC.getTreeFont(), TRUE);
			DeleteFont(hfont);
		}
	}

	/***** XMenuBar Stuff *****/
	XMenubar.resetMenuBar();

	XPopups.unload();

	freeOSCompatibility();
	mIRC.unload();
}

Dcx::Dcx(void)
{
}

Dcx::~Dcx(void)
{
}

void Dcx::setupOSCompatibility(void) {

	WNDCLASSEX wc;
	ZeroMemory((void*)&wc , sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);

#define dcxRegisterClass(szClass, szDcxClass) { \
	GetClassInfoEx(NULL, (TCHAR *)(szClass), &wc); \
	wc.lpszClassName = (TCHAR *)(szDcxClass); \
	RegisterClassEx(&wc); \
};

	// Custom ProgressBar
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering ProgressBar..."));
	dcxRegisterClass(PROGRESS_CLASS, DCX_PROGRESSBARCLASS);

	// Custom TreeView
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering TreeView..."));
	dcxRegisterClass(WC_TREEVIEW, DCX_TREEVIEWCLASS);

	// Custom Toolbar
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering ToolBar..."));
	dcxRegisterClass(TOOLBARCLASSNAME, DCX_TOOLBARCLASS);

	// Custom StatusBar
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering StatusBar..."));
	dcxRegisterClass(STATUSCLASSNAME, DCX_STATUSBARCLASS);

	// Custom ListView
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering Listview..."));
	dcxRegisterClass(WC_LISTVIEW, DCX_LISTVIEWCLASS);

	// Custom ComboEx
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering ComboEx..."));
	dcxRegisterClass(WC_COMBOBOXEX, DCX_COMBOEXCLASS);

	// Custom TrackBar
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering TrackBar..."));
	dcxRegisterClass(TRACKBAR_CLASS, DCX_TRACKBARCLASS);

	// Custom RichEdit
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering RichEdit..."));
	dcxRegisterClass(RICHEDIT_CLASS, DCX_RICHEDITCLASS);

	// Custom RebarCtrl
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering ReBar..."));
	dcxRegisterClass(REBARCLASSNAME, DCX_REBARCTRLCLASS);

	// Custom Color Combo
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering ComboBox..."));
	dcxRegisterClass(TEXT("COMBOBOX"), DCX_COLORCOMBOCLASS);

	// Custom TabCtrl
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering Tab..."));
	dcxRegisterClass(WC_TABCONTROL, DCX_TABCTRLCLASS);

	// Custom UpDown
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering UpDown..."));
	dcxRegisterClass(UPDOWN_CLASS, DCX_UPDOWNCLASS);

	// Custom IpAddress
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering IpAddress..."));
	dcxRegisterClass(WC_IPADDRESS, DCX_IPADDRESSCLASS);

	// Init Divider Control
	//InitDivider( GetModuleHandle( NULL ) );

	// Custom Divider
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering Divider..."));
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = 0;
	wc.lpfnWndProc    = DividerWndProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;
	wc.hInstance      = GetModuleHandle(NULL);
	wc.hIcon          = NULL;
	wc.hCursor        = NULL;
	wc.hbrBackground  = (HBRUSH) (COLOR_3DFACE +1);
	wc.lpszMenuName   = 0;
	wc.lpszClassName  = DCX_DIVIDERCLASS;
	wc.hIconSm        = NULL;
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
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering Panel..."));
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = CS_PARENTDC;
	wc.lpfnWndProc    = DefWindowProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;
	wc.hInstance      = GetModuleHandle(NULL);
	wc.hIcon          = NULL;
	wc.hCursor        = LoadCursor( GetModuleHandle( NULL ), IDC_ARROW );
	wc.hbrBackground  = NULL; //(HBRUSH) (COLOR_3DFACE +1);
	wc.lpszMenuName   = 0;
	wc.lpszClassName  = DCX_PANELCLASS;
	wc.hIconSm        = NULL;
	RegisterClassEx(&wc);
	//dcxRegisterClass(TEXT("STATIC"), DCX_PANELCLASS);

	// Custom Box
	//DCX_DEBUG(TEXT("LoadDLL"), TEXT("Registering Box (#32770)..."));
	//GetClassInfoEx(NULL, TEXT("#32770"), &wc); // NB: using this class causes tooltips in toolbar children to not show
	//wc.lpszClassName = DCX_BOXCLASS;			// Also causes listview/treeview label edit events to fail.
	//RegisterClassEx(&wc);
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering Box..."));
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = CS_PARENTDC;
	wc.lpfnWndProc    = DefWindowProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;
	wc.hInstance      = GetModuleHandle(NULL);
	wc.hIcon          = NULL;
	wc.hCursor        = LoadCursor( GetModuleHandle( NULL ), IDC_ARROW );
	wc.hbrBackground  = NULL; //(HBRUSH) (COLOR_3DFACE +1);
	wc.lpszMenuName   = 0;
	wc.lpszClassName  = DCX_BOXCLASS;
	wc.hIconSm        = NULL;
	RegisterClassEx(&wc);

	// Custom Button
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering Button..."));
	dcxRegisterClass(TEXT("BUTTON"), DCX_BUTTONCLASS);

	// Custom Calendar
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering Calendar..."));
	dcxRegisterClass(MONTHCAL_CLASS, DCX_CALENDARCLASS);

	// Custom DateTime
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering DateTime..."));
	dcxRegisterClass(DATETIMEPICK_CLASS, DCX_DATETIMECLASS);

	// Custom Pager
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering Pager..."));
	dcxRegisterClass(WC_PAGESCROLLER, DCX_PAGERCLASS);

	// Shadow Class
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering Shadow..."));
	wc.cbSize = sizeof(WNDCLASSEX); 
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = DefWindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetModuleHandle( NULL );
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = DCX_SHADOWCLASS;
	wc.hIconSm       = NULL;
	RegisterClassEx(&wc);

	// Vista Dialog Class
	DCX_DEBUG(debug,TEXT("LoadDLL"), TEXT("Registering Vista Dialog..."));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = DefWindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetModuleHandle( NULL );
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = DCX_VISTACLASS;
	wc.hIconSm       = NULL;
	RegisterClassEx(&wc);
}

void Dcx::freeOSCompatibility(void)
{
	GDIModule.unload();
	UXModule.unload();
	VistaModule.unload();
}

const TCHAR * Dcx::getLastError()
{
	return m_sLastError.to_chr();
}

IClassFactory* Dcx::getClassFactory()
{
	return m_pClassFactory;
}

BYTE Dcx::getGhostDrag()
{
	return m_iGhostDrag;
}

bool Dcx::setGhostDrag(const BYTE newAlpha)
{
	if (newAlpha > 255) return false;
	m_iGhostDrag = newAlpha;
	return true;
}

bool Dcx::isDX9Installed() 
{
	return m_bDX9Installed;
}

bool Dcx::isUnloadSave()
{
	return Dialogs.safeToCloseAll() && !XPopups.isPatched();
}
	
bool Dcx::initDirectX() 
{
	return initDirectX(NULL, 0);
}

bool Dcx::initDirectX(TCHAR *dxResult, int dxSize) 
{
#ifdef DCX_USE_DXSDK
	DCX_DEBUG(debug,TEXT("DXSetup"), TEXT("Checking DirectX Version..."));
	DWORD dx_ver = 0;
	if (GetDXVersion(&dx_ver, dxResult, dxSize) == S_OK) {
		if (dx_ver < 0x00090000) {
			DCX_DEBUG(debug,TEXT("DXSetup"), TEXT("Got DirectX Version: Need V9+"));
			m_bDX9Installed = false;
		}
		else {
			DCX_DEBUG(debug,TEXT("DXSetup"), TEXT("Got DirectX Version: V9+ Installed"));
			m_bDX9Installed = true;
		}
	}
	else {
		DCX_DEBUG(debug,TEXT("DXSetup"), TEXT("Couldn't Get DirectX Version"));
		m_bDX9Installed = false;
	}
#endif
	return m_bDX9Installed;
}

/*!
* \brief Displays output text to the mIRC status window.
*/
#if DCX_DEBUG_OUTPUT
void Dcx::debug(const TCHAR *cmd, const TCHAR *msg)
{
	mIRC.debug(cmd, msg);
}
#endif

/*!
 * \brief Sends an error message to mIRC.
 */
void Dcx::error(const TCHAR *cmd, const TCHAR *msg)
{
	m_sLastError.tsprintf(TEXT("D_ERROR %s (%s)"), cmd, msg);
	mIRC.echo(m_sLastError.to_chr());
}

/*
 * Variable argument error message.
 */
void Dcx::errorex(const TCHAR *cmd, const TCHAR *szFormat, ...)
{
	TString temp;
	va_list args;

	va_start(args, szFormat);
	temp.tvprintf(szFormat, &args);
	va_end(args);

	error(cmd, temp.to_chr());
}

int Dcx::mark(TCHAR* data, const TString & tsDName, const TString & tsCallbackName)
{
	TCHAR res[40];

	// check if the alias exists
	if (!mIRC.isAlias(tsCallbackName.to_chr())) {
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
	Dialogs.markDialog(mHwnd, tsDName, tsCallbackName);
	{
		DcxDialog *pTmp = Dialogs.getDialogByHandle(mHwnd);
		if (pTmp != NULL) {
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
LRESULT CALLBACK Dcx::mIRCSubClassWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_SIZE:
			{
				Dcx::mIRC.signalex(dcxSignal.xdock, TEXT("size mIRC %d %d %d"), mHwnd, LOWORD(lParam), HIWORD(lParam));
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
			LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;

			if (lpmis->CtlType == ODT_MENU) {
					XPopupMenuItem *p_Item = (XPopupMenuItem*) lpmis->itemData;

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
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;

			if (lpdis->CtlType == ODT_MENU) {
					XPopupMenuItem *p_Item = (XPopupMenuItem*) lpdis->itemData;

					if (p_Item != NULL) {
						p_Item->DrawItem(lpdis);
						return TRUE; 
					}
				}

			break;
		}

		case WM_CLOSE:
		{
			Dcx::mIRC.resetWindowProc();
			PostMessage(mHwnd, uMsg, 0, 0);
			return 0L;
			//break;
		}
		// ghost drag stuff
		case WM_ENTERSIZEMOVE:
			{
				if (Dcx::getGhostDrag() < 255) {
					const long style = GetWindowExStyle(Dcx::mIRC.getHWND());
					// Set WS_EX_LAYERED on this window
					if (!(style & WS_EX_LAYERED))
						SetWindowLongPtr(Dcx::mIRC.getHWND(), GWL_EXSTYLE, style | WS_EX_LAYERED);
					// Make this window 75 alpha
					SetLayeredWindowAttributes(Dcx::mIRC.getHWND(), 0, Dcx::getGhostDrag(), LWA_ALPHA);
					SetProp(Dcx::mIRC.getHWND(), TEXT("dcx_ghosted"), (HANDLE)1);
				}
			}
			break;

		case WM_EXITSIZEMOVE:
		{
			// turn off ghosting.
			if (GetProp(Dcx::mIRC.getHWND(), TEXT("dcx_ghosted")) != NULL) {
				// Make this window solid
				SetLayeredWindowAttributes(Dcx::mIRC.getHWND(), 0, 255, LWA_ALPHA);
				RemoveProp(Dcx::mIRC.getHWND(), TEXT("dcx_ghosted"));
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

	return Dcx::mIRC.callDefaultWindowProc(mHwnd, uMsg, wParam, lParam);
}
bool Dcx::isFile(const WCHAR* file) { 
	struct _stat64i32 stFileInfo;
	if(_wstat(file,&stFileInfo) == 0)
		// We were able to get the file attributes
		// so the file obviously exists.
		return true;
	return false;
}
bool Dcx::isFile(LPCSTR file) { 
	struct stat stFileInfo;
	if(stat(file,&stFileInfo) == 0)
		// We were able to get the file attributes
		// so the file obviously exists.
		return true;
	return false;
}
