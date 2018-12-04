#include "defines.h"
#include "Dcx.h"

namespace mIRCLinker {
	HANDLE		m_hFileMap				= nullptr;	//!< Handle to the mIRC DLL File Map
	const refString<TCHAR, c_mIRC_Buffer_Size_cch> m_pData;	//!< Pointer to a character buffer of size MIRC_BUFFER_SIZE_CCH to send mIRC custom commands
	HWND		m_mIRCHWND				= nullptr;	//!< mIRC Window Handle
	DWORD		m_dwVersion				= 0;		//!< mIRC Version info.
	DWORD		m_dwBeta				= 0;		//!< mIRC Beta Version info.
	int			m_iMapCnt				= 0;		//!< MapFile counter.
	bool		m_bDebug				= false;	//!< is mIRC is using /debug upon DCX LoadDLL().
	HWND		m_hSwitchbar			= nullptr;	//!< The Switchbars HWND
	HWND		m_hToolbar				= nullptr;	//!< The Toolbars HWND
	HWND		m_hMDI					= nullptr;	//!< The MDIClients HWND
	HWND		m_hTreebar				= nullptr;	//!< The Treebars HWND
	HWND		m_hTreeview				= nullptr;	//!< The TreeView control child of the Treebar.
	HFONT		m_hTreeFont				= nullptr;	//!< The Treebars original font.
	HIMAGELIST  m_hTreeImages			= nullptr;	//!< The Treebars original image list.
	WNDPROC		m_wpmIRCDefaultWndProc	= nullptr;
	bool		m_bUnicodemIRC			= false;
	bool		m_bSendMessageDisabled	= false;

	bool &isDebug() noexcept
	{
		return m_bDebug;
	}

	bool isVersion(const WORD main, const WORD sub) noexcept
	{
		return getMainVersion() == main && getSubVersion() == sub;
	}

	bool isOrNewerVersion(const WORD main, const WORD sub) noexcept
	{
		return getMainVersion() > main || (getMainVersion() == main && getSubVersion() >= sub);
	}

	bool isAlias(const TString &aliasName)
	{
		// check if the alias exists
		//return evalex(nullptr, 0, TEXT("$isalias(%s)"), aliasName.to_chr());
		return eval(nullptr, TEXT("$isalias(%)"), aliasName) && !aliasName.empty();
	}

	void load(LOADINFO *const lInfo)
	{
		m_mIRCHWND = lInfo->mHwnd;
		m_dwVersion = lInfo->mVersion;
		if (m_dwVersion >= 3211271)	//3211271 = 7.49.363 Beta
			m_dwBeta = lInfo->mBeta;

		lInfo->mKeep = TRUE;
		lInfo->mUnicode = TRUE;
		m_bUnicodemIRC = true;

		initMapFile();

		// Check if we're in debug mode
		TString isDebug;
		//tsEval(isDebug, TEXT("$debug"));
		eval(isDebug, TEXT("$debug"));

		m_bDebug = (!isDebug.trim().empty());
#ifdef DCX_DEBUG_OUTPUT
		if (m_bDebug)
		{
			DCX_DEBUG(debug, TEXT("LoadmIRCLink"), TEXT("Debug mode detected..."));
		}
		if (getMainVersion() >= 7)
		{
			DCX_DEBUG(debug, TEXT("LoadmIRCLink"), TEXT("mIRC V7+ detected..."));
		}
#endif

		//WNDCLASSEX wcx;
		//wcx.cbSize = sizeof(WNDCLASSEX);
		//GetClassInfoEx(GetModuleHandle(nullptr), TEXT("mIRC_ToolBar"), &wcx);

		DCX_DEBUG(debug, TEXT("LoadmIRCLink"), TEXT("Finding mIRC_Toolbar..."));
		m_hToolbar = FindWindowEx(m_mIRCHWND, nullptr, TEXT("mIRC_Toolbar"), nullptr);

		DCX_DEBUG(debug, TEXT("LoadmIRCLink"), TEXT("Finding MDIClient..."));
		m_hMDI = FindWindowEx(m_mIRCHWND, nullptr, TEXT("MDIClient"), nullptr);

		DCX_DEBUG(debug, TEXT("LoadmIRCLink"), TEXT("Finding mIRC_SwitchBar..."));
		m_hSwitchbar = FindWindowEx(m_mIRCHWND, nullptr, TEXT("mIRC_SwitchBar"), nullptr);

		DCX_DEBUG(debug, TEXT("LoadmIRCLink"), TEXT("Finding mIRC_TreeBar..."));
		m_hTreebar = FindWindowEx(m_mIRCHWND, nullptr, TEXT("mIRC_TreeBar"), nullptr);

		if (IsWindow(m_hTreebar))
		{
			m_hTreeview = FindWindowEx(m_hTreebar, nullptr, WC_TREEVIEW, nullptr);
			if (IsWindow(m_hTreeview))
				m_hTreeImages = TreeView_GetImageList(m_hTreeview, TVSIL_NORMAL);
		}
	}

	void unload() noexcept
	{
		// Reset mIRC's WndProc if changed
		resetWindowProc();

		UnmapViewOfFile(m_pData);
		CloseHandle(m_hFileMap);

		// reset the treebars font if it's been changed.
		if (mIRCLinker::getTreeFont() != nullptr)
		{
			if (const auto hfont = GetWindowFont(m_hTreeview); hfont != m_hTreeFont)
			{
				SetWindowFont(m_hTreeview, m_hTreeFont, TRUE);
				DeleteFont(hfont);
			}
		}
	}

	void initMapFile()
	{
		int cnt = 1;
		TString map_name;
		m_hFileMap = nullptr;

		while ((m_hFileMap == nullptr) && (cnt < 256))
		{
			// create mapfile name.
			//map_name.tsprintf(TEXT("mIRC%d"), cnt);
			_ts_sprintf(map_name, TEXT("mIRC%"), cnt);
			// create mapfile.
			m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, MIRC_MAP_SIZE, map_name.to_chr());

			// if create failed, fall back on old method.
			if ((m_hFileMap == nullptr) || (m_hFileMap == INVALID_HANDLE_VALUE))
			{
				cnt = 0;
				break;
			}

			// if mapfile already exists then close & try another name.
			if (GetLastError() != ERROR_ALREADY_EXISTS)
				break;

			CloseHandle(m_hFileMap);
			m_hFileMap = nullptr;

			++cnt;
		}

		if (cnt == 256)
			cnt = 0;

		m_iMapCnt = cnt; // set mapfile counter for SendMessage()'s

		// use old method for < mirc 6.2 or when new method fails.
		if (cnt == 0)
			m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, MIRC_MAP_SIZE, TEXT("mIRC"));

		//if (m_hFileMap != nullptr)
		//	m_pData = (PTCHAR)MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		if (m_hFileMap != nullptr)
			m_pData.assign(refString<TCHAR, MIRC_BUFFER_SIZE_CCH>((PTCHAR)MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0)));
	}

	HWND &getSwitchbar() noexcept
	{
		return m_hSwitchbar;
	}

	HWND &getToolbar() noexcept
	{
		return m_hToolbar;
	}

	HWND &getTreebar() noexcept
	{
		return m_hTreebar;
	}

	HWND &getTreeview() noexcept
	{
		return m_hTreeview;
	}

	HIMAGELIST &getTreeImages() noexcept
	{
		return m_hTreeImages;
	}

	HFONT &getTreeFont() noexcept
	{
		return m_hTreeFont;
	}


	HWND &getMDIClient() noexcept
	{
		return m_hMDI;
	}


	HWND &getHWND() noexcept
	{
		return m_mIRCHWND;
	}
	
	DWORD &getVersion() noexcept
	{
		return m_dwVersion;
	}

	WORD getMainVersion() noexcept
	{
		return LOWORD(m_dwVersion);
	}

	WORD getSubVersion() noexcept
	{
		return HIWORD(m_dwVersion);
	}

	bool setTreeFont(const HFONT newFont) noexcept
	{
		const auto f = GetWindowFont(m_hTreeview);
		if (m_hTreeFont == nullptr)
			m_hTreeFont = f;
		SetWindowFont(m_hTreeview, newFont, TRUE);
		if (f != m_hTreeFont)
			DeleteFont(f);
		return true;
	}

	void hookWindowProc(WNDPROC newProc) noexcept
	{
		m_wpmIRCDefaultWndProc = SubclassWindow(m_mIRCHWND, newProc);
	}

	void resetWindowProc(void) noexcept
	{
		//if (m_wpmIRCDefaultWndProc != NULL)
		//	SubclassWindow(m_mIRCHWND, m_wpmIRCDefaultWndProc);
		//m_wpmIRCDefaultWndProc = NULL;

		if (m_wpmIRCDefaultWndProc == nullptr)
			return;

		if (IsWindow(m_mIRCHWND))
		{
			if ((WNDPROC)GetWindowLongPtr(m_mIRCHWND, GWLP_WNDPROC) == Dcx::mIRCSubClassWinProc)
				SubclassWindow(m_mIRCHWND, m_wpmIRCDefaultWndProc);
		}
		m_wpmIRCDefaultWndProc = nullptr;
	}

	LRESULT callDefaultWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		if (m_wpmIRCDefaultWndProc != nullptr)
			return CallWindowProc(m_wpmIRCDefaultWndProc, hWnd, Msg, wParam, lParam);
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	bool mIRC_SndMsg(const UINT uMsg)
	{
		if (isOrNewerVersion(7,34))
		{
			m_bSendMessageDisabled = false;
			if (const auto err = SendMessage(m_mIRCHWND, uMsg, MAKEWPARAM(MIRCF_UNICODE | MIRCF_ENHANCEDERRORS, 0), m_iMapCnt); dcx_testflag(err, MIRCF_ERR_FAILED))
			{
				// failed
				if (dcx_testflag(err, MIRCF_ERR_DISABLED))
				{
					m_bSendMessageDisabled = true;
					Dcx::error(TEXT("mIRCLinker::eval()"), TEXT("SendMessage() - failed: Disabled"));
				}
				if (dcx_testflag(err, MIRCF_ERR_MAP_NAME))
					Dcx::error(TEXT("mIRCLinker::eval()"), TEXT("SendMessage() - failed: Invalid mapfile name"));
				else if (dcx_testflag(err, MIRCF_ERR_MAP_SIZE))
					Dcx::error(TEXT("mIRCLinker::eval()"), TEXT("SendMessage() - failed: Invalid mapfile size"));
				else if (dcx_testflag(err, MIRCF_ERR_EVENTID))
					Dcx::error(TEXT("mIRCLinker::eval()"), TEXT("SendMessage() - failed: Invalid eventid"));
				else if (dcx_testflag(err, MIRCF_ERR_SERVER))
					Dcx::error(TEXT("mIRCLinker::eval()"), TEXT("SendMessage() - failed: Bad Server"));
				else if (dcx_testflag(err, MIRCF_ERR_SCRIPT))
					Dcx::error(TEXT("mIRCLinker::eval()"), TEXT("SendMessage() - failed: Bad Script"));
				else
					Dcx::error(TEXT("mIRCLinker::eval()"), TEXT("SendMessage() - failed"));
				return false;
			}
		}
		else {
			if (SendMessage(m_mIRCHWND, uMsg, MIRCF_UNICODE, m_iMapCnt) == 0)
				return false;
		}
		return true;
	}

	/*!
	 * \brief Requests mIRC $identifiers to be evaluated.
	 *
	 * Allow sufficient characters to be returned.
	 */
	//bool eval(TCHAR *const res, const int maxlen, const TCHAR *const data) {
	//	//if (ts_strcpyn(m_pData, data, MIRC_BUFFER_SIZE_CCH) != nullptr)
	//	//{
	//	//	if (mIRC_SndMsg(WM_MEVALUATE)) {
	//	//		if (res != nullptr)
	//	//			dcx_strcpyn(res, m_pData, maxlen);
	//	//
	//	//		return (ts_strcmp(m_pData, TEXT("$false")) != 0);
	//	//	}
	//	//}
	//	//m_pData[0] = 0;
	//	//return false;
	//
	//	m_pData = data;
	//	{
	//		if (mIRC_SndMsg(WM_MEVALUATE)) {
	//			if (res != nullptr)
	//				dcx_strcpyn(res, m_pData, maxlen);
	//
	//			return m_pData != TEXT("$false");
	//		}
	//	}
	//	m_pData.clear();
	//	return false;
	//}

	//bool tsEval(TString &res, const TCHAR *const data) {
	//	//if (ts_strcpyn(m_pData, data, MIRC_BUFFER_SIZE_CCH) != nullptr)
	//	//{
	//	//	if (mIRC_SndMsg(WM_MEVALUATE)) {
	//	//		res = m_pData;
	//	//		return (res != TEXT("$false"));
	//	//	}
	//	//}
	//	//m_pData[0] = 0;
	//	//return false;
	//
	//	m_pData = data;
	//	{
	//		if (mIRC_SndMsg(WM_MEVALUATE)) {
	//			res = m_pData;
	//			return (res != TEXT("$false"));
	//		}
	//	}
	//	m_pData.clear();
	//	return false;
	//}

	bool iEval(int64_t * const res, const TCHAR *const data)
	{
		//if (ts_strcpyn(m_pData, data, MIRC_BUFFER_SIZE_CCH) != nullptr)
		//{
		//	if (mIRC_SndMsg(WM_MEVALUATE)) {
		//		*res = dcx_atoi64(m_pData);
		//		return (*res != 0);
		//	}
		//}
		//m_pData[0] = 0;
		//return false;

		m_pData = data;
		{
			if (mIRC_SndMsg(WM_MEVALUATE))
			{
				*res = dcx_atoi64(m_pData.data());
				return (*res != 0);
			}
		}
		m_pData.clear();
		return false;
	}

	/*!
	 * \brief Requests mIRC $identifiers to be evaluated.
	 *
	 * Allow sufficient characters to be returned.
	 * Requests mIRC to perform command using vsprintf.
	 */
	//bool evalex(TCHAR *const res, const int maxlen, const TCHAR *const szFormat, ...)
	//{
	//	TString line;
	//	va_list args = nullptr;
	//
	//	va_start(args, szFormat);
	//	line.tvprintf(szFormat, args);
	//	va_end(args);
	//
	//	return eval(res, maxlen, line.to_chr());
	//}
	
	//bool tsEvalex(TString &res, const TCHAR *const szFormat, ...)
	//{
	//	TString line;
	//	va_list args = nullptr;
	//
	//	va_start(args, szFormat);
	//	line.tvprintf(szFormat, args);
	//	va_end(args);
	//
	//	//return tsEval(res, line.to_chr());
	//
	//	return eval(res, line);
	//}

	//bool exec(const TCHAR *const data)
	//{
	//	//if (ts_strcpyn(m_pData, data, MIRC_BUFFER_SIZE_CCH) != nullptr)
	//	//{
	//	//	// SendMessage(mHwnd, WM_MCOMMAND, MAKEWPARAM(cMethod, cEventId), cIndex)
	//	//	if (mIRC_SndMsg(WM_MCOMMAND))
	//	//		return (m_pData[0] == TEXT('\0'));
	//	//}
	//	//return false;
	//
	//	m_pData = data;
	//	{
	//		// SendMessage(mHwnd, WM_MCOMMAND, MAKEWPARAM(cMethod, cEventId), cIndex)
	//		if (mIRC_SndMsg(WM_MCOMMAND))
	//			return m_pData.empty();
	//	}
	//	return false;
	//}

	//bool execex(const TCHAR *const szFormat, ...)
	//{
	//	//TString line;
	//	//va_list args = nullptr;
	//	//
	//	//va_start(args, szFormat);
	//	//line.tvprintf(szFormat, args);
	//	//va_end(args);
	//	//
	//	//return exec(line.to_chr());
	//
	//	TString line;
	//	va_list args = nullptr;
	//
	//	va_start(args, szFormat);
	//	line.tvprintf(szFormat, args);
	//	va_end(args);
	//
	//	return exec(line);
	//}

	//void signal(const TCHAR *const msg) {
	//	//wnsprintf(m_pData, MIRC_BUFFER_SIZE_CCH, TEXT("//.signal -n DCX %s"), msg);
	//	//mIRC_SndMsg(WM_MCOMMAND);
	//
	//	_ts_sprintf(m_pData, TEXT("//.signal -n DCX %"), msg);
	//	mIRC_SndMsg(WM_MCOMMAND);
	//}

	/*!
	* \brief Sends a signal to mIRC.
	*
	* This method allows for multiple parameters.
	*/
	//void signalex(const bool allow, const TCHAR *const szFormat, ...) {
	//	if (!allow)
	//		return;
	//
	//	TString msg;
	//	va_list args = nullptr;
	//
	//	va_start(args, szFormat);
	//	msg.tvprintf(szFormat, args);
	//	va_end(args);
	//
	//	signal(msg.to_chr());
	//}

	/*!
	 * \brief Sends a debug message to mIRC (with formatting).
	 *
	 * This method allows for multiple parameters.
	 */
//#if DCX_DEBUG_OUTPUT
//	void debug(const TCHAR *const cmd, const TCHAR *const msg) {
//		if (!isDebug()) return;
//		TString err;
//		echo(_ts_sprintf(err, TEXT("D_DEBUG % (%)"), cmd, msg).to_chr());
//	}
//#endif

	/*!
	* \brief Displays output text to the mIRC status window.
	*/
	//void echo(const TCHAR *const data) {
	//	//wnsprintf(m_pData, MIRC_BUFFER_SIZE_CCH, TEXT("//echo -s %s"), data);
	//	//mIRC_SndMsg(WM_MCOMMAND);
	//	//// cant send error msg here, as msg needs this function to display
	//
	//	_ts_sprintf(m_pData, TEXT("//echo -s %"), data);
	//	mIRC_SndMsg(WM_MCOMMAND);
	//	// cant send error msg here, as msg needs this function to display
	//}
}
