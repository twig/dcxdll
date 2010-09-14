#include "defines.h"
#include "Dcx.h"

mIRCLinker::mIRCLinker(void) : m_hFileMap(NULL),
                               m_pData(NULL),
                               m_mIRCHWND(NULL),
							   m_dwVersion(0x0),
							   m_iMapCnt(0),
							   m_hSwitchbar(NULL),
							   m_hToolbar(NULL),
							   m_hTreebar(NULL),
							   m_hMDI(NULL),
							   m_hTreeview(NULL),
							   m_hTreeFont(NULL),
							   m_hTreeImages(NULL),
							   //m_sLastError(""),
							   m_wpmIRCDefaultWndProc(NULL),
							   m_bDebug(false)
{
}

mIRCLinker::~mIRCLinker(void)
{
}

bool mIRCLinker::isDebug() const
{
	return m_bDebug;
}

bool mIRCLinker::isVersion(const WORD main, const WORD sub) const {
	return getMainVersion() == main && getSubVersion() == sub;
}

bool mIRCLinker::isOrNewerVersion(const WORD main, const WORD sub) const {
	return getMainVersion() > main || (getMainVersion() == main && getSubVersion() >= sub);
}

bool mIRCLinker::isAlias(const char * aliasName)
{
	// check if the alias exists
	return evalex(NULL, 0, "$isalias(%s)", aliasName);
}

void mIRCLinker::load(LOADINFO * lInfo) {
	m_mIRCHWND = lInfo->mHwnd;
	m_dwVersion = lInfo->mVersion;
	if (LOWORD(m_dwVersion) == 2) {	//Fix the problem that mIRC v6.20 reports itself as 6.2
		m_dwVersion -= 2;
		m_dwVersion += 20; // err how exactly does this fix it?
	}
	lInfo->mKeep = TRUE;
	initMapFile();

	// Check if we're in debug mode
	TString isDebug;
	tsEval(isDebug, "$debug");

	m_bDebug = (isDebug.trim().len() > 0);
	DCX_DEBUG(debug,"LoadmIRCLink", "Debug mode detected...");

	if (this->getMainVersion() == 7) {
		DCX_DEBUG(debug,"LoadmIRCLink", "mIRC V7 detected...");
		DCX_DEBUG(debug,"LoadmIRCLink", "Can't do any window mods etc..");
		return;
	}

	DCX_DEBUG(debug,"LoadmIRCLink", "Finding mIRC_Toolbar...");
	m_hToolbar = FindWindowEx(m_mIRCHWND,NULL,"mIRC_Toolbar",NULL);

	DCX_DEBUG(debug,"LoadmIRCLink", "Finding MDIClient...");
	m_hMDI = FindWindowEx(m_mIRCHWND,NULL,"MDIClient",NULL);

	DCX_DEBUG(debug,"LoadmIRCLink", "Finding mIRC_SwitchBar...");
	m_hSwitchbar = FindWindowEx(m_mIRCHWND,NULL,"mIRC_SwitchBar",NULL);

	if (isOrNewerVersion(6,30)) { // class renamed for 6.30+
		DCX_DEBUG(debug,"LoadmIRCLink", "Finding mIRC_TreeBar...");
		m_hTreebar = FindWindowEx(m_mIRCHWND,NULL,"mIRC_TreeBar",NULL);
	}
	else {
		DCX_DEBUG(debug,"LoadmIRCLink", "Finding mIRC_TreeList...");
		m_hTreebar = FindWindowEx(m_mIRCHWND,NULL,"mIRC_TreeList",NULL);
	}

	if (IsWindow(m_hTreebar)) {
		//m_hTreeview = GetWindow(mIRCLink.m_hTreebar,GW_CHILD);
		m_hTreeview = FindWindowEx(m_hTreebar,NULL,WC_TREEVIEW,NULL);
		if (IsWindow(m_hTreeview))
			m_hTreeImages = TreeView_GetImageList(m_hTreeview,TVSIL_NORMAL);
	}
}

void mIRCLinker::unload() {
	// Reset mIRC's WndProc if changed
	resetWindowProc();
	UnmapViewOfFile(m_pData);
	CloseHandle(m_hFileMap);
		
	// reset the treebars font if it's been changed.
	if (Dcx::mIRC.getTreeFont() != NULL) {
		HFONT hfont = GetWindowFont(m_hTreeview);
		if (hfont != m_hTreeFont) {
			SetWindowFont( m_hTreeview, m_hTreeFont, TRUE);
			DeleteFont(hfont);
		}
	}
}

void mIRCLinker::initMapFile() {
	int cnt = 0;
	if (isOrNewerVersion(6,20)) {
		TString map_name;
		cnt = 1;
		m_hFileMap = NULL;

		while ((m_hFileMap == NULL) && (cnt < 256)) {
			// create mapfile name.
			map_name.tsprintf("mIRC%d",cnt);
			// create mapfile.
			m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,0,PAGE_READWRITE,0,MIRC_BUFFER_SIZE_BYTES,map_name.to_chr());

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
	}

	m_iMapCnt = cnt; // set mapfile counter for SendMessage()'s

	// use old method for < mirc 6.2 or when new method fails.
	if (cnt == 0) {
		m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, MIRC_BUFFER_SIZE_BYTES, "mIRC");
	}

	m_pData = (LPSTR) MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
}

HWND mIRCLinker::getSwitchbar() const
{
	return m_hSwitchbar;
}

HWND mIRCLinker::getToolbar() const
{
	return m_hToolbar;
}

HWND mIRCLinker::getTreebar() const
{
	return m_hTreebar;
}

HWND mIRCLinker::getTreeview() const
{
	return m_hTreeview;
}

HIMAGELIST mIRCLinker::getTreeImages() const
{
	return m_hTreeImages;
}

HFONT mIRCLinker::getTreeFont() const
{
	return m_hTreeFont;
}


HWND mIRCLinker::getMDIClient() const
{
	return m_hMDI;
}


HWND mIRCLinker::getHWND() const
{
	return this->m_mIRCHWND;
}
DWORD mIRCLinker::getVersion() const {
	return m_dwVersion;
}

WORD mIRCLinker::getMainVersion() const {
	return LOWORD(m_dwVersion);
}

WORD mIRCLinker::getSubVersion() const {
	return HIWORD(m_dwVersion);
}

bool mIRCLinker::setTreeFont(HFONT newFont)
{
	HFONT f = GetWindowFont(m_hTreeview);
	if (m_hTreeFont == NULL)
		m_hTreeFont = f;
	SetWindowFont( m_hTreeview, newFont, TRUE);
	if (f != m_hTreeFont)
		DeleteFont(f);
	return true;
}

void mIRCLinker::hookWindowProc(WNDPROC newProc)
{
	if (this->getMainVersion() == 7) {
		DCX_DEBUG(debug,"LoadmIRCLink", "mIRC V7 detected...");
		DCX_DEBUG(debug,"LoadmIRCLink", "Can't do any window mods etc..");
		return;
	}
	m_wpmIRCDefaultWndProc = SubclassWindow(m_mIRCHWND, newProc);
}

void mIRCLinker::resetWindowProc(void) {
	if (m_wpmIRCDefaultWndProc != NULL)
		SubclassWindow(m_mIRCHWND, m_wpmIRCDefaultWndProc);
}

LRESULT mIRCLinker::callDefaultWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProc(m_wpmIRCDefaultWndProc, hWnd, Msg, wParam, lParam);
}

/*!
 * \brief Requests mIRC $identifiers to be evaluated.
 *
 * Allow sufficient characters to be returned.
 */
bool mIRCLinker::eval(char *res, const int maxlen, const char *data) {
	lstrcpy(m_pData, data);
	SendMessage(m_mIRCHWND, WM_MEVALUATE, 0, m_iMapCnt);
	if (res != NULL) lstrcpyn(res, m_pData, maxlen);
	if (lstrcmp(m_pData, "$false") == 0) return false;
	return true;
}

bool mIRCLinker::tsEval(TString &res, const char *data) {
	lstrcpy(m_pData, data);
	SendMessage(m_mIRCHWND, WM_MEVALUATE, 0, m_iMapCnt);
	res = m_pData;
	if (lstrcmp(m_pData, "$false") == 0) return false;
	return true;
}

bool mIRCLinker::iEval(__int64  *res, const char *data) {
	lstrcpy(m_pData, data);
	SendMessage(m_mIRCHWND, WM_MEVALUATE, 0, m_iMapCnt);
	*res = _atoi64(m_pData);
	if (*res == 0) return false;
	return true;
}

/*!
 * \brief Requests mIRC $identifiers to be evaluated.
 *
 * Allow sufficient characters to be returned.
 * Requests mIRC to perform command using vsprintf.
*/
bool mIRCLinker::evalex(char *res, const int maxlen, const char *szFormat, ...)
{
	TString line;
	va_list args;

	va_start(args, szFormat);
	line.tvprintf(szFormat, &args);
	va_end( args );

	return eval(res, maxlen, line.to_chr());
}
bool mIRCLinker::tsEvalex(TString &res, const char *szFormat, ...)
{
	TString line;
	va_list args;

	va_start(args, szFormat);
	line.tvprintf(szFormat, &args);
	va_end( args );

	return tsEval(res, line.to_chr());
}

bool mIRCLinker::exec(const char *data)
{
	lstrcpy(m_pData, data);
	SendMessage(m_mIRCHWND, WM_MCOMMAND, 0, m_iMapCnt);
	if (lstrlen(m_pData) == 0) return true;
	return false;
}

bool mIRCLinker::execex(const char *szFormat, ...)
{
	TString line;
	va_list args;

	va_start(args, szFormat);
	line.tvprintf(szFormat, &args);
	va_end( args );

	return exec(line.to_chr());
}

void mIRCLinker::signal(const char *msg) {
	wsprintf(m_pData, "//.signal -n DCX %s", msg);
	SendMessage(m_mIRCHWND, WM_MCOMMAND, 0, m_iMapCnt);
}

/*!
* \brief Sends a signal to mIRC.
*
* This method allows for multiple parameters.
*/
void mIRCLinker::signalex(const bool allow, const char *szFormat, ...) {
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
void mIRCLinker::debug(const char *cmd, const char *msg) {
	if (!isDebug()) return;
	TString err;
	err.tsprintf("D_DEBUG %s (%s)", cmd, msg);
	echo(err.to_chr());
}
#endif

/*!
* \brief Displays output text to the mIRC status window.
*/
void mIRCLinker::echo(const char *data) {
	wsprintf(m_pData, "//echo -s %s", data);
	SendMessage(m_mIRCHWND, WM_MCOMMAND, 0, m_iMapCnt);
}
