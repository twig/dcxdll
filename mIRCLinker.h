#pragma once
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class mIRCLinker
{
	HANDLE		m_hFileMap; //!< Handle to the mIRC DLL File Map
	LPSTR		m_pData;    //!< Pointer to a character buffer of size 900 to send mIRC custom commands
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
	bool isAero() const;
	bool isVista() const;
	bool isDebug() const;
	//bool isDXInstalled9();
	bool isAlias(const char * aliasName);

	void load(LOADINFO * lInfo);
	void unload(void);

	void hookWindowProc(WNDPROC newProc);
	void resetWindowProc(void);
	LRESULT callDefaultWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	bool eval(char *res, const int maxlen, const char *data);
	bool tsEval(TString &res, const char *data);
	bool evalex(char *res, const int maxlen, const char *szFormat, ...);
	bool tsEvalex(TString &res, const char *szFormat, ...);
	bool exec(const char *data);
	bool execex(const char *szFormat, ...);
	void signal(const char *msg);
	void signalex(const bool allow, const char *szFormat, ...);
	void echo(const char *data);
#if DCX_DEBUG_OUTPUT
	void debug(const char *cmd, const char *msg);
#endif
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif
