#pragma once
//#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
//#pragma warning( push )
//#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
//#endif

namespace mIRCLinker
{
	constexpr auto		m_mIRC_Buffer_Size_cch = MIRC_BUFFER_SIZE_CCH;

	extern HANDLE		m_hFileMap;		//!< Handle to the mIRC DLL File Map
	extern const refString<TCHAR, m_mIRC_Buffer_Size_cch> m_pData;		//!< Pointer to a character buffer of size MIRC_BUFFER_SIZE_CCH to send mIRC custom commands
	extern HWND			m_mIRCHWND;		//!< mIRC Window Handle
	extern DWORD		m_dwVersion;
	extern DWORD		m_dwBeta;
	extern int			m_iMapCnt;		//!< MapFile counter.
	extern bool			m_bDebug;		//!< is mIRC is using /debug upon DCX LoadDLL().
	extern HWND			m_hSwitchbar;	//!< The Switchbars HWND
	extern HWND			m_hToolbar;		//!< The Toolbars HWND
	extern HWND			m_hMDI;			//!< The MDIClients HWND
	extern HWND			m_hTreebar;		//!< The Treebars HWND
	extern HWND			m_hTreeview;	//!< The TreeView control child of the Treebar.
	extern HFONT		m_hTreeFont;	//!< The Treebars original font.
	extern HIMAGELIST	m_hTreeImages;	//!< The Treebars original image list.
	//extern TString		m_sLastError;
	extern WNDPROC		m_wpmIRCDefaultWndProc;
	extern bool			m_bUnicodemIRC;
	extern bool			m_bSendMessageDisabled;

	// mIRC DLL Loading Structure
	struct LOADINFO {
		DWORD  mVersion; //!< mIRC Version
		HWND   mHwnd;    //!< mIRC Hwnd 
		BOOL   mKeep;    //!< mIRC variable stating to keep DLL in memory
		BOOL   mUnicode; //!< mIRC V7+ unicode enabled dll.
		DWORD  mBeta;    //!< mIRC V7.49.363+ Beta version
	};
	using LPLOADINFO = LOADINFO *;

	void initMapFile();

	HWND &getSwitchbar() noexcept;
	HWND &getToolbar() noexcept;
	HWND &getTreebar() noexcept;
	HWND &getTreeview() noexcept;
	HIMAGELIST &getTreeImages() noexcept;
	HFONT &getTreeFont() noexcept;
	HWND &getMDIClient() noexcept;
	HWND &getHWND() noexcept;
	DWORD &getVersion() noexcept;
	WORD getMainVersion() noexcept;
	WORD getSubVersion() noexcept;
	bool setTreeFont(const HFONT newFont);
	bool isOrNewerVersion(const WORD main, const WORD sub) noexcept;
	bool isVersion(const WORD main, const WORD sub) noexcept;
	bool &isDebug() noexcept;
	__inline bool &isUnicode() noexcept { return m_bUnicodemIRC; };

	//bool isDXInstalled9();
	bool isAlias(const TString &aliasName);

	void load(LOADINFO *const lInfo);
	void unload(void);

	void hookWindowProc(WNDPROC newProc);
	void resetWindowProc(void);
	LRESULT callDefaultWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	bool mIRC_SndMsg(const UINT uMsg);

	/*!
	* \brief Requests mIRC $identifiers to be evaluated.
	*
	* Allow sufficient characters to be returned.
	*/
	//template <typename Output, typename Input>
	//std::enable_if_t<!std::is_null_pointer_v<Output>, bool> eval(Output &res, const Input &data)
	//{
	//	m_pData = data;
	//	{
	//		if (mIRC_SndMsg(WM_MEVALUATE)) {
	//			res = m_pData;
	//			return (m_pData != TEXT("$false"));
	//		}
	//	}
	//	m_pData.clear();
	//	return false;
	//}

	/*!
	* \brief Requests mIRC $identifiers to be evaluated.
	*
	* Allow sufficient characters to be returned.
	*/
	//template <typename Output, typename Input>
	//std::enable_if_t<std::is_null_pointer_v<Output>, bool> eval(Output res, const Input &data)
	//{
	//	m_pData = data;
	//	{
	//		if (mIRC_SndMsg(WM_MEVALUATE))
	//			return (m_pData != TEXT("$false"));
	//	}
	//	m_pData.clear();
	//	return false;
	//}

	/*!
	* \brief Requests mIRC $identifiers to be evaluated.
	*
	* Allow sufficient characters to be returned.
	*/
	//template <typename Output, typename Input, typename Value, typename... Arguments, typename Dummy = std::enable_if_t<!std::is_null_pointer_v<Output>> >
	//bool eval(Output &res, const Input &fmt, const Value &val, Arguments&&... args)
	//{
	//	TString line;
	//	return eval(res, _ts_sprintf(line, fmt, val, args...));
	//}
	//template <typename Output, typename Input, typename Value, typename... Arguments, typename Dummy = std::enable_if_t<std::is_null_pointer_v<Output>> >
	//bool eval(Output res, const Input &fmt, const Value &val, Arguments&&... args)
	//{
	//	TString line;
	//	return eval(res, _ts_sprintf(line, fmt, val, args...));
	//}

	template <typename Output, typename Input>
	bool eval(Output &res, const Input &data)
	{
		m_pData = data;
		{
			if (mIRC_SndMsg(WM_MEVALUATE)) {
				res = m_pData;
				return (m_pData != TEXT("$false"));
			}
		}
		m_pData.clear();
		return false;
	}
	template <typename Input>
	bool eval(nullptr_t res, const Input &data)
	{
		m_pData = data;
		{
			if (mIRC_SndMsg(WM_MEVALUATE))
				return (m_pData != TEXT("$false"));
		}
		m_pData.clear();
		return false;
	}
	template <typename Output, typename Input, typename Value, typename... Arguments>
	bool eval(Output &res, const Input &fmt, const Value &val, Arguments&&... args)
	{
		TString line;
		return eval(res, _ts_sprintf(line, fmt, val, args...));
	}
	template <typename Input, typename Value, typename... Arguments>
	bool eval(nullptr_t res, const Input &fmt, const Value &val, Arguments&&... args)
	{
		TString line;
		return eval(res, _ts_sprintf(line, fmt, val, args...));
	}

	bool iEval(__int64 *const res, const TCHAR *const data);

	template <typename Input>
	bool exec(const Input &data)
	{
		if constexpr(std::is_array_v<Input> && std::is_pod_v<Input>)
			m_pData = &data[0];
		else
			m_pData = data;

		{
			// SendMessage(mHwnd, WM_MCOMMAND, MAKEWPARAM(cMethod, cEventId), cIndex)
			if (mIRC_SndMsg(WM_MCOMMAND))
				return m_pData.empty();
		}
		return false;
	}

	template <typename Input, typename Value, typename... Arguments>
	bool exec(const Input &fmt, const Value &val, Arguments&&... args)
	{
		TString line;
		return exec(_ts_sprintf(line, fmt, val, args...));
	}

	template <typename Input>
	void signal(const Input &msg)
	{
		m_pData = TEXT("//.signal -n DCX ");
		if constexpr(std::is_array_v<Input> && std::is_pod_v<Input>)
			m_pData += &msg[0];
		else
			m_pData += msg;
		mIRC_SndMsg(WM_MCOMMAND);
	}

	template <typename Input, typename Value, typename... Arguments>
	void signal(const Input &fmt, const Value &val, Arguments&&... args)
	{
		TString line;
		return signal(_ts_sprintf(line, fmt, val, args...));
	}

	template <typename Input>
	void echo(const Input &data)
	{
		m_pData = TEXT("//echo -s ");
		if constexpr(std::is_array_v<Input> && std::is_pod_v<Input>)
			m_pData += &data[0];
		else
			m_pData += data;
		mIRC_SndMsg(WM_MCOMMAND);
	}

	template <typename Input, typename Value, typename... Arguments>
	void echo(const Input &fmt, const Value &val, Arguments&&... args)
	{
		TString line;
		return echo(_ts_sprintf(line, fmt, val, args...));
	}
#if DCX_DEBUG_OUTPUT
	template <typename Command, typename Message>
	void debug(const Command &cmd, const Message &msg)
	{
		if (!isDebug()) return;
		TString err;
		echo(_ts_sprintf(err, TEXT("D_DEBUG % (%)"), cmd, msg));
	}
#endif

	//bool eval(TCHAR *res, const int maxlen, const TCHAR *const data);
	//bool evalex(TCHAR *const res, const int maxlen, const TCHAR *const szFormat, ...);
	//bool tsEval(TString &res, const TCHAR *const data);
	//bool tsEval(TString &res, const TString &data) noexcept { return tsEval(res, data.to_chr()); }
	//bool iEval(__int64 *const res, const TCHAR *const data);
	//bool tsEvalex(TString &res, const TCHAR *const szFormat, ...);
	//bool exec(const TCHAR *const data);
	//bool execex(const TCHAR *const szFormat, ...);
	//void signal(const TCHAR *const msg);
	//void signalex(const bool allow, const TCHAR *const szFormat, ...);
	//void echo(const TCHAR *const data);
//#if DCX_DEBUG_OUTPUT
//	void debug(const TCHAR *const cmd, const TCHAR *const msg);
//#endif
};
//#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
//#pragma warning( pop )
//#endif
