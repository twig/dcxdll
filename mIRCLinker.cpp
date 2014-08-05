#include "defines.h"
#include "Dcx.h"

namespace mIRCLinker {
	HANDLE		m_hFileMap				= NULL; //!< Handle to the mIRC DLL File Map
	PTCHAR		m_pData					= NULL; //!< Pointer to a character buffer of size MIRC_BUFFER_SIZE_CCH to send mIRC custom commands
	HWND		m_mIRCHWND				= NULL; //!< mIRC Window Handle
	DWORD		m_dwVersion				= 0;	//!< mIRC Version info.
	int			m_iMapCnt				= 0;	//!< MapFile counter.
	bool		m_bDebug				= false;//!< is mIRC is using /debug upon DCX LoadDLL().
	HWND		m_hSwitchbar			= NULL; //!< The Switchbars HWND
	HWND		m_hToolbar				= NULL; //!< The Toolbars HWND
	HWND		m_hMDI					= NULL; //!< The MDIClients HWND
	HWND		m_hTreebar				= NULL; //!< The Treebars HWND
	HWND		m_hTreeview				= NULL; //!< The TreeView control child of the Treebar.
	HFONT		m_hTreeFont				= NULL; //!< The Treebars original font.
	HIMAGELIST  m_hTreeImages			= NULL; //!< The Treebars original image list.
	TString		m_sLastError;
	WNDPROC		m_wpmIRCDefaultWndProc	= NULL;
	bool		m_bUnicodemIRC			= false;
	bool		m_bSendMessageDisabled	= false;

	bool &mIRCLinker::isDebug()
	{
		return m_bDebug;
	}

	bool mIRCLinker::isVersion(const WORD main, const WORD sub) {
		return getMainVersion() == main && getSubVersion() == sub;
	}

	bool mIRCLinker::isOrNewerVersion(const WORD main, const WORD sub) {
		return getMainVersion() > main || (getMainVersion() == main && getSubVersion() >= sub);
	}

	bool mIRCLinker::isAlias(const TCHAR * aliasName)
	{
		// check if the alias exists
		return evalex(NULL, 0, TEXT("$isalias(%s)"), aliasName);
	}

	void mIRCLinker::load(LOADINFO * lInfo) {
		m_mIRCHWND = lInfo->mHwnd;
		m_dwVersion = lInfo->mVersion;
		// v2 dll for mirc V7+ anyway.
		lInfo->mKeep = TRUE;
		lInfo->mUnicode = TRUE;
		m_bUnicodemIRC = true;

		initMapFile();

		// Check if we're in debug mode
		TString isDebug;
		tsEval(isDebug, TEXT("$debug"));

		m_bDebug = (!isDebug.trim().empty());
#ifdef DCX_DEBUG_OUTPUT
		if (m_bDebug) {
			DCX_DEBUG(debug, TEXT("LoadmIRCLink"), TEXT("Debug mode detected..."));
		}
		if (getMainVersion() >= 7) {
			DCX_DEBUG(debug, TEXT("LoadmIRCLink"), TEXT("mIRC V7+ detected..."));
		}
#endif

		DCX_DEBUG(debug, TEXT("LoadmIRCLink"), TEXT("Finding mIRC_Toolbar..."));
		m_hToolbar = FindWindowEx(m_mIRCHWND, NULL, TEXT("mIRC_Toolbar"), NULL);

		DCX_DEBUG(debug, TEXT("LoadmIRCLink"), TEXT("Finding MDIClient..."));
		m_hMDI = FindWindowEx(m_mIRCHWND, NULL, TEXT("MDIClient"), NULL);

		DCX_DEBUG(debug, TEXT("LoadmIRCLink"), TEXT("Finding mIRC_SwitchBar..."));
		m_hSwitchbar = FindWindowEx(m_mIRCHWND, NULL, TEXT("mIRC_SwitchBar"), NULL);

		// v2 dll for mirc V7+ anyway.
		DCX_DEBUG(debug, TEXT("LoadmIRCLink"), TEXT("Finding mIRC_TreeBar..."));
		m_hTreebar = FindWindowEx(m_mIRCHWND, NULL, TEXT("mIRC_TreeBar"), NULL);

		if (IsWindow(m_hTreebar)) {
			//m_hTreeview = GetWindow(mIRCLink.m_hTreebar,GW_CHILD);
			m_hTreeview = FindWindowEx(m_hTreebar, NULL, WC_TREEVIEW, NULL);
			if (IsWindow(m_hTreeview))
				m_hTreeImages = TreeView_GetImageList(m_hTreeview, TVSIL_NORMAL);
		}
	}

	void mIRCLinker::unload() {
		// Reset mIRC's WndProc if changed
		resetWindowProc();

		UnmapViewOfFile(m_pData);
		CloseHandle(m_hFileMap);

		// reset the treebars font if it's been changed.
		if (mIRCLinker::getTreeFont() != NULL) {
			HFONT hfont = GetWindowFont(m_hTreeview);
			if (hfont != m_hTreeFont) {
				SetWindowFont(m_hTreeview, m_hTreeFont, TRUE);
				DeleteFont(hfont);
			}
		}
	}

	void mIRCLinker::initMapFile() {
		int cnt = 0;
		// v2 dll for mirc V7+ anyway.
		TString map_name;
		cnt = 1;
		m_hFileMap = NULL;

		while ((m_hFileMap == NULL) && (cnt < 256)) {
			// create mapfile name.
			map_name.tsprintf(TEXT("mIRC%d"), cnt);
			// create mapfile.
			m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, MIRC_MAP_SIZE, map_name.to_chr());

			// if create failed, fall back on old method.
			if ((m_hFileMap == NULL) || (m_hFileMap == INVALID_HANDLE_VALUE)) {
				cnt = 0;
				break;
			}

			// if mapfile already exists then close & try another name.
			if (GetLastError() == ERROR_ALREADY_EXISTS) {
				CloseHandle(m_hFileMap);
				m_hFileMap = NULL;
			}
			else
				break;

			cnt++;
		}

		if (cnt == 256)
			cnt = 0;

		m_iMapCnt = cnt; // set mapfile counter for SendMessage()'s

		// use old method for < mirc 6.2 or when new method fails.
		if (cnt == 0) {
			m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, MIRC_MAP_SIZE, TEXT("mIRC"));
		}

		if (m_hFileMap != NULL)
			m_pData = (PTCHAR)MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	}

	HWND &mIRCLinker::getSwitchbar()
	{
		return m_hSwitchbar;
	}

	HWND &mIRCLinker::getToolbar()
	{
		return m_hToolbar;
	}

	HWND &mIRCLinker::getTreebar()
	{
		return m_hTreebar;
	}

	HWND &mIRCLinker::getTreeview()
	{
		return m_hTreeview;
	}

	HIMAGELIST &mIRCLinker::getTreeImages()
	{
		return m_hTreeImages;
	}

	HFONT &mIRCLinker::getTreeFont()
	{
		return m_hTreeFont;
	}


	HWND &mIRCLinker::getMDIClient()
	{
		return m_hMDI;
	}


	HWND &mIRCLinker::getHWND()
	{
		return m_mIRCHWND;
	}
	DWORD &mIRCLinker::getVersion()
	{
		return m_dwVersion;
	}

	WORD mIRCLinker::getMainVersion()
	{
		return LOWORD(m_dwVersion);
	}

	WORD mIRCLinker::getSubVersion()
	{
		return HIWORD(m_dwVersion);
	}

	bool mIRCLinker::setTreeFont(HFONT newFont)
	{
		HFONT f = GetWindowFont(m_hTreeview);
		if (m_hTreeFont == NULL)
			m_hTreeFont = f;
		SetWindowFont(m_hTreeview, newFont, TRUE);
		if (f != m_hTreeFont)
			DeleteFont(f);
		return true;
	}

	void mIRCLinker::hookWindowProc(WNDPROC newProc)
	{
		m_wpmIRCDefaultWndProc = SubclassWindow(m_mIRCHWND, newProc);
	}

	void mIRCLinker::resetWindowProc(void) {
		//if (m_wpmIRCDefaultWndProc != NULL)
		//	SubclassWindow(m_mIRCHWND, m_wpmIRCDefaultWndProc);
		//m_wpmIRCDefaultWndProc = NULL;

		if (m_wpmIRCDefaultWndProc == NULL)
			return;

		if (IsWindow(m_mIRCHWND))
		{
			if ((WNDPROC)GetWindowLongPtr(m_mIRCHWND, GWLP_WNDPROC) == Dcx::mIRCSubClassWinProc)
				SubclassWindow(m_mIRCHWND, m_wpmIRCDefaultWndProc);
		}
		m_wpmIRCDefaultWndProc = NULL;
	}

	LRESULT mIRCLinker::callDefaultWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (m_wpmIRCDefaultWndProc != NULL)
			return CallWindowProc(m_wpmIRCDefaultWndProc, hWnd, Msg, wParam, lParam);
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	bool mIRCLinker::mIRC_SndMsg(const UINT uMsg)
	{
		if (mIRCLinker::isOrNewerVersion(7,34))
		{
			m_bSendMessageDisabled = false;
			const LRESULT err = SendMessage(m_mIRCHWND, uMsg, MAKEWPARAM(MIRCF_UNICODE | MIRCF_ENHANCEDERRORS, 0), m_iMapCnt);
			if ((err & MIRCF_ERR_FAILED))
			{
				// failed
				if (err & MIRCF_ERR_DISABLED) {
					m_bSendMessageDisabled = true;
					Dcx::error(TEXT("mIRCLinker::eval()"), TEXT("SendMessage() - failed: Disabled"));
				}
				if (err & MIRCF_ERR_MAP_NAME)
					Dcx::error(TEXT("mIRCLinker::eval()"), TEXT("SendMessage() - failed: Invalid mapfile name"));
				else if (err & MIRCF_ERR_MAP_SIZE)
					Dcx::error(TEXT("mIRCLinker::eval()"), TEXT("SendMessage() - failed: Invalid mapfile size"));
				else if (err & MIRCF_ERR_EVENTID)
					Dcx::error(TEXT("mIRCLinker::eval()"), TEXT("SendMessage() - failed: Invalid eventid"));
				else if (err & MIRCF_ERR_SERVER)
					Dcx::error(TEXT("mIRCLinker::eval()"), TEXT("SendMessage() - failed: Bad Server"));
				else if (err & MIRCF_ERR_SCRIPT)
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
	bool mIRCLinker::eval(TCHAR *res, const int maxlen, const TCHAR *data) {
		if (lstrcpyn(m_pData, data, MIRC_BUFFER_SIZE_CCH) != NULL)
		{
			if (mIRC_SndMsg(WM_MEVALUATE)) {
				if (res != NULL) dcx_strcpyn(res, m_pData, maxlen);
				if (lstrcmp(m_pData, TEXT("$false")) == 0) return false;
				return true;
			}
		}
		m_pData[0] = 0;
		return false;
	}

	bool mIRCLinker::tsEval(TString &res, const TCHAR *data) {
		if (lstrcpyn(m_pData, data, MIRC_BUFFER_SIZE_CCH) != NULL)
		{
			if (mIRC_SndMsg(WM_MEVALUATE)) {
				res = m_pData;
				if (res == TEXT("$false")) return false;
				return true;
			}
		}
		m_pData[0] = 0;
		return false;
	}

	bool mIRCLinker::iEval(__int64  *res, const TCHAR *data) {
		if (lstrcpyn(m_pData, data, MIRC_BUFFER_SIZE_CCH) != NULL)
		{
			if (mIRC_SndMsg(WM_MEVALUATE)) {
				*res = dcx_atoi64(m_pData);
				if (*res == 0) return false;
				return true;
			}
		}
		m_pData[0] = 0;
		return false;
	}

	/*!
	 * \brief Requests mIRC $identifiers to be evaluated.
	 *
	 * Allow sufficient characters to be returned.
	 * Requests mIRC to perform command using vsprintf.
	 */
	bool mIRCLinker::evalex(TCHAR *res, const int maxlen, const TCHAR *szFormat, ...)
	{
		TString line;
		va_list args;

		va_start(args, szFormat);
		line.tvprintf(szFormat, &args);
		va_end(args);

		return eval(res, maxlen, line.to_chr());
	}
	bool mIRCLinker::tsEvalex(TString &res, const TCHAR *szFormat, ...)
	{
		TString line;
		va_list args;

		va_start(args, szFormat);
		line.tvprintf(szFormat, &args);
		va_end(args);

		return tsEval(res, line.to_chr());
	}

	bool mIRCLinker::exec(const TCHAR *data)
	{
		if (lstrcpyn(m_pData, data, MIRC_BUFFER_SIZE_CCH) != NULL)
		{
			// SendMessage(mHwnd, WM_MCOMMAND, MAKEWPARAM(cMethod, cEventId), cIndex)
			if (mIRC_SndMsg(WM_MCOMMAND)) {
				if (m_pData[0] == TEXT('\0'))
					return true;
			}
		}
		return false;
	}

	bool mIRCLinker::execex(const TCHAR *szFormat, ...)
	{
		TString line;
		va_list args;

		va_start(args, szFormat);
		line.tvprintf(szFormat, &args);
		va_end(args);

		return exec(line.to_chr());
	}

	void mIRCLinker::signal(const TCHAR *msg) {
		wnsprintf(m_pData, MIRC_BUFFER_SIZE_CCH, TEXT("//.signal -n DCX %s"), msg);
		mIRC_SndMsg(WM_MCOMMAND);
	}

	/*!
	* \brief Sends a signal to mIRC.
	*
	* This method allows for multiple parameters.
	*/
	void mIRCLinker::signalex(const bool allow, const TCHAR *szFormat, ...) {
		if (!allow)
			return;

		TString msg;
		va_list args;

		va_start(args, szFormat);
		msg.tvprintf(szFormat, &args);
		va_end(args);

		signal(msg.to_chr());
	}

	/*!
	 * \brief Sends a debug message to mIRC (with formatting).
	 *
	 * This method allows for multiple parameters.
	 */
#if DCX_DEBUG_OUTPUT
	void mIRCLinker::debug(const TCHAR *cmd, const TCHAR *msg) {
		if (!isDebug()) return;
		TString err;
		err.tsprintf(TEXT("D_DEBUG %s (%s)"), cmd, msg);
		echo(err.to_chr());
	}
#endif

	/*!
	* \brief Displays output text to the mIRC status window.
	*/
	void mIRCLinker::echo(const TCHAR *data) {
		wnsprintf(m_pData, MIRC_BUFFER_SIZE_CCH, TEXT("//echo -s %s"), data);
		mIRC_SndMsg(WM_MCOMMAND);
		// cant send error msg here, as msg needs this function to display
	}
}