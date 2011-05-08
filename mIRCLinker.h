#pragma once
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class mIRCLinker
{
	HANDLE		m_hFileMap; //!< Handle to the mIRC DLL File Map
	PTCHAR		m_pData;    //!< Pointer to a character buffer of size MIRC_BUFFER_SIZE_CCH to send mIRC custom commands
	HWND		m_mIRCHWND; //!< mIRC Window Handle
	DWORD		m_dwVersion;
	int			m_iMapCnt;  //!< MapFile counter.
	bool		m_bDebug;    //!< is mIRC is using /debug upon DCX LoadDLL().
	HWND		m_hSwitchbar; //!< The Switchbars HWND
	HWND		m_hToolbar; //!< The Toolbars HWND
	HWND		m_hMDI; //!< The MDIClients HWND
	HWND		m_hTreebar; //!< The Treebars HWND
	HWND		m_hTreeview; //!< The TreeView control child of the Treebar.
	HFONT		m_hTreeFont; //!< The Treebars original font.
	HIMAGELIST  m_hTreeImages; //!< The Treebars original image list.
	TString		m_sLastError;
	WNDPROC		m_wpmIRCDefaultWndProc;
	bool		m_bUnicodemIRC;

	void initMapFile();

public:
	mIRCLinker(void);
	~mIRCLinker(void);
	HWND getSwitchbar() const;
	HWND getToolbar() const;
	HWND getTreebar() const;
	HWND getTreeview() const;
	HIMAGELIST getTreeImages() const;
	HFONT getTreeFont() const;
	HWND getMDIClient() const;
	HWND getHWND() const;
	DWORD getVersion() const;
	WORD getMainVersion() const;
	WORD getSubVersion() const;
	const TString & getLastError() const;
	bool setTreeFont(HFONT newFont);
	bool isOrNewerVersion(const WORD main, const WORD sub) const;
	bool isVersion(const WORD main, const WORD sub) const;
	//bool isAero() const;
	//bool isVista() const;
	bool isDebug() const;
	bool isUnicode() const { return this->m_bUnicodemIRC; };

	//bool isDXInstalled9();
	bool isAlias(const TCHAR * aliasName);

	void load(LOADINFO * lInfo);
	void unload(void);

	void hookWindowProc(WNDPROC newProc);
	void resetWindowProc(void);
	LRESULT callDefaultWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	bool eval(TCHAR *res, const int maxlen, const TCHAR *data);
	bool tsEval(TString &res, const TCHAR *data);
	bool iEval(__int64 *res, const TCHAR *data);
	bool evalex(TCHAR *res, const int maxlen, const TCHAR *szFormat, ...);
	bool tsEvalex(TString &res, const TCHAR *szFormat, ...);
	bool exec(const TCHAR *data);
	bool execex(const TCHAR *szFormat, ...);
	void signal(const TCHAR *msg);
	void signalex(const bool allow, const TCHAR *szFormat, ...);
	void echo(const TCHAR *data);
#if DCX_DEBUG_OUTPUT
	void debug(const TCHAR *cmd, const TCHAR *msg);
#endif
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif
