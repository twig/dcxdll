#include "defines.h"
#include <ExDisp.h>
#include "Dcx.h"
#include "Classes/custom/divider.h"
#include "Classes/custom/DcxTrayIcon.h"

namespace Dcx {
	static TCHAR szLastError[MIRC_BUFFER_SIZE_CCH]{};
	IClassFactory* m_pClassFactory{};
	DcxGDIModule GDIModule;
	DcxUXModule UXModule;
	DcxDWMModule VistaModule;
	DcxDialogCollection Dialogs;
	XPopupMenuManager XPopups;
	XMenuBar XMenubar;
	std::byte m_iGhostDrag{ 255 };
	bool m_bDX9Installed{ false };
	bool m_bErrorTriggered{ false };

	// static colours enabled by default.
	bool setting_bStaticColours{ true };
	// custom menus disabled by default.
	BYTE setting_CustomMenusAlpha{ 255 };
	bool setting_CustomMenusRounded{ false };

	MapOfCursors	m_vMapOfCursors;
	MapOfAreas		m_vMapOfAreas;
	PFNSETCURSOR SetCursorUx = nullptr;
	TString dcxException::tsErr;


	void load(mIRCLinker::LOADINFO* const lInfo)
	{
		m_iGhostDrag = std::byte{ 255 };
		m_bDX9Installed = false;
		m_pClassFactory = nullptr;
		m_bErrorTriggered = false;

		// Initialize mIRCLinker
		mIRCLinker::load(lInfo);

		// Initializing OLE Support
		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Initializing OLE Support..."));

		{
			const auto hr = OleInitialize(nullptr);
			if ((hr == OLE_E_WRONGCOMPOBJ) || (hr == RPC_E_CHANGED_MODE))
			{
				// init failed...
				DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("OLE Failed to init"));
			}
		}

		// Initializing GDI
		GDIModule.load();

		UXModule.load();

		VistaModule.load();

		// Load Control definitions
		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Loading control classes"));

		if (const INITCOMMONCONTROLSEX icex{ sizeof(INITCOMMONCONTROLSEX), ICC_TREEVIEW_CLASSES | ICC_BAR_CLASSES | ICC_PROGRESS_CLASS | ICC_LISTVIEW_CLASSES |
			ICC_USEREX_CLASSES | ICC_COOL_CLASSES | ICC_STANDARD_CLASSES | ICC_UPDOWN_CLASS | ICC_DATE_CLASSES |
			ICC_TAB_CLASSES | ICC_INTERNET_CLASSES | ICC_PAGESCROLLER_CLASS };
			!InitCommonControlsEx(&icex))
		{
			DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Unable to init common controls"));
		}

		// RichEdit DLL Loading this is required before using richedit
		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Loading Dll for RichEdit..."));
		//LoadLibrary(TEXT("RICHED20.DLL"));
		LoadLibrary(TEXT("Msftedit.dll"));

		//get IClassFactory* for WebBrowser (move this to DcxControl & setup in InitializeDcxControls())
		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Generating class factory for WebBrowser..."));
		if (FAILED(CoGetClassObject(CLSID_WebBrowser, CLSCTX_INPROC_SERVER, nullptr, IID_IClassFactory, reinterpret_cast<void**>(&m_pClassFactory))))
		{
			// failed...
			DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Unable to get WebBrowser..."));
		}

		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Initializing Controls..."));
		DcxControl::InitializeDcxControls();

		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Setup OS Compatibility..."));
		setupOSCompatibility();

		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Subclass mIRC Window..."));
		mIRCLinker::hookWindowProc(Dcx::mIRCSubClassWinProc);

		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Initializing Popups..."));
		XPopups.load();

		// Patch SetCursor() function for custom cursors
		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Patch SetCursor..."));
		SetCursorUx = static_cast<PFNSETCURSOR>(PatchAPI("User32.dll", "SetCursor", Dcx::XSetCursor));
	}

	void unload(void)
	{
		if (Dialogs.closeDialogs())
		{ // if unable to close dialogs stop unload.
			Dcx::error(__FUNCTIONW__, TEXT("Unable to Unload DLL from within the DLL"));
			//return 0; // NB: This DOESN'T stop the unload, & mIRC will still crash.
		}

		CloseUltraDock(); // UnDock All.

		RemovePatch(SetCursorUx, Dcx::XSetCursor);

		DcxControl::UnInitializeDcxControls();

		UnregisterClass(DCX_VISTACLASS, GetModuleHandle(nullptr));

		// Class Factory of Web Control
		if (m_pClassFactory)
			m_pClassFactory->Release();

		// Terminating OLE Support
		OleUninitialize();


		/***** TrayIcon Stuff *****/
		trayIcons.reset(nullptr);

		// reset the treebars font if it's been changed.
		if (mIRCLinker::getTreeFont())
		{
			if (const auto hfont = GetWindowFont(mIRCLinker::getTreeview()); hfont && hfont != mIRCLinker::getTreeFont())
			{
				SetWindowFont(mIRCLinker::getTreeview(), mIRCLinker::getTreeFont(), TRUE);
				DeleteObject(hfont);
			}
		}

		/***** XMenuBar Stuff *****/
		XMenubar.resetMenuBar();

		XPopups.unload();

		// free up custom cursors data.
		for (const auto& cd : m_vMapOfCursors)
			DestroyCursor(cd.second);

		m_vMapOfCursors.clear();

		for (const auto& cd : m_vMapOfAreas)
			DestroyCursor(cd.second);

		m_vMapOfAreas.clear();

		freeOSCompatibility();
		mIRCLinker::unload();
	}

	void setupOSCompatibility(void)
	{
		WNDCLASSEX wc{};

		// Vista Dialog Class
		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering Vista Dialog..."));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.hIcon = nullptr;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		//wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wc.hbrBackground = GetStockBrush(COLOR_WINDOWFRAME);
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = DCX_VISTACLASS;
		wc.hIconSm = nullptr;
		RegisterClassEx(&wc);
	}

	void freeOSCompatibility(void) noexcept
	{
		VistaModule.unload();
		UXModule.unload();
		GDIModule.unload();
	}

	//const TString &getLastError() noexcept
	//{
	//	return m_sLastError;
	//}
	const TCHAR* getLastError() noexcept
	{
		return &szLastError[0];
	}

	IClassFactory* const getClassFactory() noexcept
	{
		return m_pClassFactory;
	}

	const std::byte& getGhostDrag() noexcept
	{
		return m_iGhostDrag;
	}

	bool setGhostDrag(const std::byte newAlpha) noexcept
	{
		m_iGhostDrag = newAlpha;
		return true;
	}

	const bool& isDX9Installed() noexcept
	{
		return m_bDX9Installed;
	}

	bool isUnloadSafe() noexcept
	{
		return Dialogs.safeToCloseAll();
	}

	const bool& initDirectX()
	{
		return initDirectX(nullptr, 0);
	}

	const bool& initDirectX(TCHAR* dxResult, int dxSize)
	{
#ifdef DCX_USE_DXSDK
		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Checking DirectX Version..."));
		if (DWORD dx_ver = 0; GetDXVersion(&dx_ver, dxResult, dxSize) == S_OK)
		{
			if (dx_ver < 0x00090000)
			{
				DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Got DirectX Version: Need V9+"));
				m_bDX9Installed = false;
			}
			else {
				DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Got DirectX Version: V9+ Installed"));
				m_bDX9Installed = true;
			}
		}
		else {
			DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Couldn't Get DirectX Version"));
			m_bDX9Installed = false;
		}
#endif
		return m_bDX9Installed;
	}

	 /*!
	  * \brief Sends an error message to mIRC.
	  */
	void error(const TCHAR* const cmd, const TCHAR* const msg) noexcept
	{
		if (m_bErrorTriggered)
			return;

		_ts_snprintf(&szLastError[0], std::size(szLastError), TEXT("D_ERROR %s (%s)"), cmd, msg);

		m_bErrorTriggered = true;

		if (mIRCLinker::m_bSendMessageDisabled)
		{
			_ts_strncat(&szLastError[0], TEXT("\r\nSendMessage() disabled, re-enable this in mIRC's Lock Options"), std::size(szLastError));
			MessageBox(mIRCLinker::m_mIRCHWND, &szLastError[0], nullptr, MB_OK);
		}
		else
			mIRCLinker::echo(&szLastError[0]);

		m_bErrorTriggered = false;
	}

	void mark(const TString& tsDName, const TString& tsCallbackName)
	{
		// check if the alias exists
		if (!mIRCLinker::isAlias(tsCallbackName))
			throw Dcx::dcxException(TEXT("No such alias : %"), tsCallbackName);

		// check if valid dialog
		const auto mHwnd = GetHwndFromString(tsDName);

		if (!IsWindow(mHwnd))
			throw Dcx::dcxException(TEXT("Invalid Dialog Window : %"), tsDName);

		if (Dialogs.getDialogByHandle(mHwnd))
			throw Dcx::dcxException(TEXT("Window Already Marked : %"), tsDName);

		Dialogs.markDialog(mHwnd, tsDName, tsCallbackName);

		if (const auto pTmp = Dialogs.getDialogByHandle(mHwnd); pTmp)
			pTmp->SetVerbose(pTmp->evalAliasEx(nullptr, 0, TEXT("isverbose,0")));

		return;
	}

	/*!
	* \brief blah
	*
	* blah
	*/
	LRESULT CALLBACK mIRCSubClassWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_SIZE:
			if (dcxSignal.xdock)
				mIRCLinker::signal(TEXT("size mIRC % % %"), reinterpret_cast<DWORD>(mHwnd), Dcx::dcxLOWORD(lParam), Dcx::dcxHIWORD(lParam));
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
			if (wParam != 0)	// wParam == 0 means sent by menu.
				break;

			dcxlParam(LPMEASUREITEMSTRUCT, lpmis);

			if (!lpmis)
				break;

			if (lpmis->CtlType == ODT_MENU)
			{
				if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(lpmis->itemData); p_Item)
				{
					//const auto size = p_Item->getItemSize(mHwnd);
					//
					//lpmis->itemWidth = gsl::narrow_cast<UINT>(size.cx);
					//lpmis->itemHeight = gsl::narrow_cast<UINT>(size.cy);

					const auto [x, y] = p_Item->getItemSize(mHwnd);
					lpmis->itemWidth = gsl::narrow_cast<UINT>(x);
					lpmis->itemHeight = gsl::narrow_cast<UINT>(y);

					return TRUE;
				}
			}
			break;
		}

		case WM_DRAWITEM:
		{
			dcxlParam(LPDRAWITEMSTRUCT, lpdis);

			if (!lpdis)
				break;

			if (lpdis->CtlType == ODT_MENU)
			{
				if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(lpdis->itemData); p_Item)
				{
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
			if (getGhostDrag() < std::byte{ 255U })
			{
				// Set WS_EX_LAYERED on this window
				if (const auto style = dcxGetWindowExStyle(mIRCLinker::getHWND()); !dcx_testflag(style, WS_EX_LAYERED))
					dcxSetWindowExStyle(mIRCLinker::getHWND(), (style | WS_EX_LAYERED));

				// Make this window 75 alpha
				SetLayeredWindowAttributes(mIRCLinker::getHWND(), 0, std::to_integer<BYTE>(getGhostDrag()), LWA_ALPHA);
				SetProp(mIRCLinker::getHWND(), TEXT("dcx_ghosted"), reinterpret_cast<HANDLE>(1U));
			}
			if (dcxSignal.xdock)
				mIRCLinker::signal(TEXT("sizemove mIRC start %"), reinterpret_cast<DWORD>(mHwnd));
		}
		break;

		case WM_EXITSIZEMOVE:
		{
			// turn off ghosting.
			if (GetProp(mIRCLinker::getHWND(), TEXT("dcx_ghosted")))
			{
				// Make this window solid
				SetLayeredWindowAttributes(mIRCLinker::getHWND(), 0, 255, LWA_ALPHA);
				RemoveProp(mIRCLinker::getHWND(), TEXT("dcx_ghosted"));
			}
			if (dcxSignal.xdock)
				mIRCLinker::signal(TEXT("sizemove mIRC end %"), reinterpret_cast<DWORD>(mHwnd));
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
			if (!SetCursorUx)
				break;

			{
				const auto iType = gsl::narrow_cast<UINT>(Dcx::dcxLOWORD(lParam));

				if (const auto hCursor = AreaToCustomCursor(iType); hCursor)
				{
					SetCursorUx(hCursor);
					return TRUE;
				}
			}

			if (!m_vMapOfCursors.empty())
			{
				// this stops the default cursor showing, before we change it.
				ShowCursor(FALSE);
				Auto(ShowCursor(TRUE));

				const auto lRes = mIRCLinker::callDefaultWindowProc(mHwnd, uMsg, wParam, lParam);

				if (const auto hCursor = SystemToCustomCursor(GetCursor()); hCursor)
				{
					SetCursorUx(hCursor);
					return TRUE;
				}
				return lRes;
			}
			break;
		}

		default:
			break;
		}

		return mIRCLinker::callDefaultWindowProc(mHwnd, uMsg, wParam, lParam);
	}

	// convert a cursor name into a resource number.
	GSL_SUPPRESS(lifetimes.1)
	const TCHAR* const parseCursorType(const TString& cursor)
	{
		return parseCursorType(std::hash<TString>{}(cursor));
	}

	// convert a cursor name (hashed) into a resource number.
	GSL_SUPPRESS(lifetimes.1)
	const TCHAR* const parseCursorType(const std::hash<TString>::result_type& cursor)
	{
		const static std::map<std::hash<TString>::result_type, const TCHAR*> IDC_map{
			{ TEXT("appstarting"_hash), IDC_APPSTARTING },
			{ TEXT("arrow"_hash), IDC_ARROW },
			{ TEXT("cross"_hash), IDC_CROSS },
			{ TEXT("hand"_hash), IDC_HAND },
			{ TEXT("help"_hash), IDC_HELP },
			{ TEXT("ibeam"_hash), IDC_IBEAM },
			{ TEXT("no"_hash), IDC_NO },
			{ TEXT("sizeall"_hash), IDC_SIZEALL },
			{ TEXT("sizenesw"_hash), IDC_SIZENESW },
			{ TEXT("sizens"_hash), IDC_SIZENS },
			{ TEXT("sizenwse"_hash), IDC_SIZENWSE },
			{ TEXT("sizewe"_hash), IDC_SIZEWE },
			{ TEXT("uparrow"_hash), IDC_UPARROW },
			{ TEXT("wait"_hash), IDC_WAIT }
		};

		if (const auto got = IDC_map.find(cursor); got != IDC_map.end())
			return got->second;

		return nullptr;
	}

	// convert a cursor name into a system resource number.
	const DWORD parseSystemCursorType(const TString& cursor)
	{
		return parseSystemCursorType(std::hash<TString>{}(cursor));
	}

	// convert a cursor name (hashed) into a system resource number.
	const DWORD parseSystemCursorType(const std::hash<TString>::result_type& cursor)
	{
		const static std::map<const std::hash<TString>::result_type, DWORD> IDC_SystemMap{
			{ TEXT("appstarting"_hash), OCR_APPSTARTING },
			{ TEXT("normal"_hash), OCR_NORMAL },
			{ TEXT("cross"_hash), OCR_CROSS },
			{ TEXT("hand"_hash), OCR_HAND },
			//{TEXT("help"), OCR_HELP},
			{ TEXT("ibeam"_hash), OCR_IBEAM },
			{ TEXT("no"_hash), OCR_NO },
			{ TEXT("sizeall"_hash), OCR_SIZEALL },
			{ TEXT("sizenesw"_hash), OCR_SIZENESW },
			{ TEXT("sizens"_hash), OCR_SIZENS },
			{ TEXT("sizenwse"_hash), OCR_SIZENWSE },
			{ TEXT("sizewe"_hash), OCR_SIZEWE },
			//{TEXT("uparrow"_hash), OCR_UPARROW},
			{ TEXT("wait"_hash), OCR_WAIT }
		};

		if (const auto got = IDC_SystemMap.find(cursor); got != IDC_SystemMap.end())
			return got->second;
		return 0;
	}

	// convert an area name into a hit zone
	const DWORD parseAreaType(const TString& tsArea)
	{
		return parseAreaType(std::hash<TString>{}(tsArea));
	}

	// convert an area name (hashed) into a hit zone
	const DWORD parseAreaType(const std::hash<TString>::result_type& tsArea)
	{
		const static std::map<const std::hash<TString>::result_type, DWORD> mIRC_AreaMap{
			{ TEXT("client"_hash), HTCLIENT },
			//{TEXT("nowhere"_hash), HTNOWHERE},
			{ TEXT("caption"_hash), HTCAPTION },
			{ TEXT("sysmenu"_hash), HTSYSMENU },
			{ TEXT("size"_hash), HTGROWBOX },
			{ TEXT("menu"_hash), HTMENU },
			{ TEXT("vscroll"_hash), HTVSCROLL },
			{ TEXT("help"_hash), HTHELP },
			{ TEXT("hscroll"_hash), HTHSCROLL },
			{ TEXT("min"_hash), HTMINBUTTON },
			{ TEXT("max"_hash), HTMAXBUTTON },
			{ TEXT("left"_hash), HTLEFT },
			{ TEXT("right"_hash), HTRIGHT },
			{ TEXT("top"_hash), HTTOP },
			{ TEXT("topleft"_hash), HTTOPLEFT },
			{ TEXT("topright"_hash), HTTOPRIGHT },
			{ TEXT("bottom"_hash), HTBOTTOM },
			{ TEXT("bottomleft"_hash), HTBOTTOMLEFT },
			{ TEXT("bottomright"_hash), HTBOTTOMRIGHT },
			{ TEXT("border"_hash), HTBORDER },
			{ TEXT("close"_hash), HTCLOSE }
		};

		if (const auto got = mIRC_AreaMap.find(tsArea); got != mIRC_AreaMap.end())
			return got->second;

		return 0;
	}

	// load a cursor from a file
	// throws a dcxException on fail.
	HCURSOR dcxLoadCursorFromFile(const TString& filename)
	{
		const auto hCursor = static_cast<HCURSOR>(LoadImage(nullptr, filename.to_chr(), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE));
		if (!hCursor)
			throw Dcx::dcxException("Unable to load cursor file");
		return hCursor;
	}

	// load a cursor from a resource
	// throws a dcxException on fail.
	HCURSOR dcxLoadCursorFromResource(const TCHAR* CursorType)
	{
		const auto hCursor = static_cast<HCURSOR>(LoadImage(nullptr, CursorType, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
		if (!hCursor)
			throw Dcx::dcxException("Unable to load cursor resource");
		return hCursor;
	}

	// load a cursor from a file or resource, depending on flags
	// throws a dcxException on fail.
	HCURSOR dcxLoadCursor(const DcxResourceFlags iFlags, const TCHAR* CursorType, bool& bCursorFromFile, const HCURSOR oldCursor, TString& filename)
	{
		HCURSOR hCursor = nullptr;
		HCURSOR newCursor = nullptr;

		if (bCursorFromFile)
			hCursor = oldCursor;

		bCursorFromFile = false;

		if (iFlags == DcxResourceFlags::FROMRESSOURCE)
			newCursor = dcxLoadCursorFromResource(CursorType);
		else if (iFlags == DcxResourceFlags::FROMFILE)
		{
			if (!IsFile(filename))
				throw Dcx::dcxException(TEXT("Unable to Access File: %"), filename);

			newCursor = Dcx::dcxLoadCursorFromFile(filename);

			bCursorFromFile = true;
		}
		if (!newCursor)
			throw Dcx::dcxException("Unable to Load Cursor");

		if (GetCursor() == hCursor)
			SetCursor(newCursor);

		if (hCursor)
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
		if (const auto it = m_vMapOfAreas.find(iType); it != m_vMapOfAreas.end())
		{
			DestroyCursor(it->second);
			m_vMapOfAreas.erase(it);
		}
	}

	// get custom cursor to use in place of specific system cursor.
	HCURSOR SystemToCustomCursor(const HCURSOR hCursor)
	{
		if (hCursor && !m_vMapOfCursors.empty())
		{
			if (const auto it = m_vMapOfCursors.find(hCursor); it != m_vMapOfCursors.end())
				return it->second;
		}
		return nullptr;
	}

	// get the custom cursor to use with a specific area
	HCURSOR AreaToCustomCursor(const UINT iType)
	{
		if (!m_vMapOfAreas.empty())
		{
			if (const auto it = m_vMapOfAreas.find(iType); it != m_vMapOfAreas.end())
				return it->second;
		}
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
		if (const auto it = m_vMapOfCursors.find(hCursor); it != m_vMapOfCursors.end())
		{
			DestroyCursor(it->second);
			m_vMapOfCursors.erase(it);
		}
	}

	// patch a dll function using detours
	GSL_SUPPRESS(lifetimes)
	PVOID PatchAPI(const char* const c_szDllName, const char* const c_szApiName, PVOID newfPtr) noexcept
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		PVOID fPtr = DetourFindFunction(c_szDllName, c_szApiName);
		if (fPtr)
		{
			DetourAttach(&fPtr, newfPtr);
			DetourTransactionCommit();
		}
		else
			DetourTransactionAbort();

		return fPtr;
	}

	// remove a detours patch
	void RemovePatch(PVOID fPtr, PVOID newfPtr) noexcept
	{
		if (!fPtr || !newfPtr)
			return;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&fPtr, newfPtr);
		DetourTransactionCommit();
	}

	// patch function for use in custom cursors
	GSL_SUPPRESS(lifetimes)
	HCURSOR WINAPI XSetCursor(HCURSOR hCursor)
	{
		//if (!hCursor)
		//	return SetCursorUx(hCursor);
		//
		//auto hTemp = SystemToCustomCursor(hCursor);
		//if (!hTemp)
		//	hTemp = hCursor;
		//return SetCursorUx(hTemp);

		if (hCursor)
		{
			if (auto hTemp = SystemToCustomCursor(hCursor); hTemp)
				return SetCursorUx(hTemp);
		}
		return SetCursorUx(hCursor);
	}

	void FillRectColour(HDC hdc, LPCRECT prc, COLORREF clr) noexcept
	{
		if (!hdc || !prc)
			return;

		if (const auto br = CreateSolidBrush(clr); br)
		{
			FillRect(hdc, prc, br);
			DeleteBrush(br);
		}
	}
}
