/*!
 * \file dcxdialog.h
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * � ScriptsDB.org - 2006
 */

#ifndef _DCXDIALOG_H_
#define _DCXDIALOG_H_

#include "defines.h"
#include "dcxwindow.h"
#include "xpopup/xpopupmenu.h"
#include "layout/layoutmanager.h"
#include <map>

#define DBS_BKGCOLOR	0x00000001 //!< Control Background Color
#define DBS_BKGBITMAP	0x00000002 //!< Control Background Bitmap

#define DBS_BKGNORMAL	0x00000004 // top left, no sizing applied
#define DBS_BKGTILE		0x00000008 // tiled across whole dialog
#define DBS_BKGSTRETCH	0x00000010 // stretched to fit

 // used with DBS_BKGNORMAL
#define DBS_BKGCENTER	0x00000020 // centered horizontally
#define DBS_BKGVCENTER	0x00000040 // centered vertically
#define DBS_BKGRIGHT	0x00000080 // right aligned
#define DBS_BKGBOTTOM	0x00000100 // bottom aligned

#define DCX_NAMED_ID_OFFSET 3000U

// dummy runtime classe definition
class DcxControl;
class DcxList;

using VectorOfControlPtrs = std::vector<DcxControl*>; //!< blah
using VectorOfDragListPtrs = std::vector<DcxList*>; //!< Vector of draglists
using IntegerHash = std::map<TString, UINT>;

/*!
 * \brief blah
 *
 * blah
 */

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class DcxDialog final
	: public DcxWindow
{
public:
	DcxDialog() = delete;
	DcxDialog(const DcxDialog& other) = delete;
	DcxDialog(DcxDialog&& other) = delete;
	GSL_SUPPRESS(c.128) DcxDialog& operator =(const DcxDialog&) = delete;	// No assignments!
	GSL_SUPPRESS(c.128) DcxDialog& operator =(DcxDialog&&) = delete;	// No move assignments!

	DcxDialog(const HWND mHwnd, const TString& tsName, const TString& tsAliasName);
	~DcxDialog() noexcept;

	void parseCommandRequest(_In_ const TString& input) final;
	void parseCommandRequestEX(_In_z_ _Printf_format_string_ const TCHAR* const szFormat, ...);
	void parseComControlRequestEX(_In_ const UINT id, _In_z_ _Printf_format_string_ const TCHAR* const szFormat, ...) const;
	//void parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const;
	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const override;

	//bool evalAliasEx( TCHAR *const szReturn, const int maxlen, const TCHAR *const szFormat, ... );
	//bool evalAlias( TCHAR *const szReturn, const int maxlen, const TCHAR *const szArgs);
	bool evalAliasEx(_Out_ const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturn, _In_ const int maxlen, _In_z_ _Printf_format_string_ const TCHAR* const szFormat, ...) const;
	bool evalAlias(_Out_ const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturn, _In_ const int maxlen, _In_z_ const TCHAR* const szArgs) const;
	bool evalAlias(_In_z_ const TCHAR* const szArgs) const;
	bool execAliasEx(_In_z_ _Printf_format_string_ const TCHAR* const szFormat, ...) const;
	bool execAlias(_In_z_ const TCHAR* const szArgs) const;

	template <typename Format, typename Value, typename... Arguments>
	auto evalAliasT(const Format& fmt, const Value& val, Arguments&& ... args)
	{
		TString tsArgs;
		GSL_SUPPRESS(lifetime.1) return evalAliasT(_ts_sprintf(tsArgs, fmt, val, args...));
	}
	template <typename Format, typename Value>
	auto evalAliasT(const Format& fmt, const Value& val)
	{
		TString tsArgs;
		return evalAliasT(_ts_sprintf(tsArgs, fmt, val));
	}

	template <typename Value>
	std::pair<bool, TString> evalAliasT(const Value& val)
	{
		TString tsArgs, tsRes;
		if constexpr (std::is_array_v<Value> && Dcx::is_pod_v<Value>)
			_ts_sprintf(tsArgs, TEXT("$%(%,%)"), getAliasName(), getName(), MakeTextmIRCSafe(&val[0]));
		else
			_ts_sprintf(tsArgs, TEXT("$%(%,%)"), getAliasName(), getName(), MakeTextmIRCSafe(val));
	
		incRef();
		GSL_SUPPRESS(lifetime.1) Auto(decRef());
	
		const auto bRes = mIRCLinker::eval(tsRes, tsArgs);
	
		return{ bRes, tsRes };
	}

	//template <typename Value>
	//Dcx::BoolValue<TString> evalAliasT(const Value& val)
	//{
	//	TString tsArgs, tsRes;
	//	if constexpr (std::is_array_v<Value> && Dcx::is_pod_v<Value>)
	//		_ts_sprintf(tsArgs, TEXT("$%(%,%)"), getAliasName(), getName(), MakeTextmIRCSafe(&val[0]));
	//	else
	//		_ts_sprintf(tsArgs, TEXT("$%(%,%)"), getAliasName(), getName(), MakeTextmIRCSafe(val));
	//
	//	incRef();
	//	GSL_SUPPRESS(lifetime.1) Auto(decRef());
	//
	//	const auto bRes = mIRCLinker::eval(tsRes, tsArgs);
	//
	//	return{ bRes, tsRes };
	//}

	template <typename Value>
	auto o_evalAliasT(const Value& val)
	{
		TString tsArgs, tsRes;
		if constexpr (std::is_array_v<Value> && Dcx::is_pod_v<Value>)
			_ts_sprintf(tsArgs, TEXT("$%(%,%)"), getAliasName(), getName(), MakeTextmIRCSafe(&val[0]));
		else
			_ts_sprintf(tsArgs, TEXT("$%(%,%)"), getAliasName(), getName(), MakeTextmIRCSafe(val));

		incRef();
		Auto(decRef());

		return mIRCLinker::o_eval<TString>(tsArgs);
	}

	DcxControl* getControlByNamedID(const TString &tsID) const;
	DcxControl* getControlByID(const UINT ID) const noexcept;
	DcxControl* getControlByHWND(const HWND mHwnd) const noexcept;

	//void addControl(DcxControl* const p_Control);
	DcxControl* addControl(const TString& input, const UINT offset, const DcxAllowControls mask, HWND hParent);
	void deleteControl(const DcxControl* const p_Control) noexcept;

	void deleteAllControls() noexcept;

	const bool updateLayout(RECT& rc);

	void setMouseControl(const UINT mUID);
	void setFocusControl(const UINT mUID);

#if DCX_CUSTOM_MENUS
	//void UAHDrawMenuNCBottomLine(HWND hWnd) const noexcept;
	//void UAHDrawMenuBar(HWND mHwnd, UAHMENU* pUDM) noexcept;
	//void UAHDrawMenuBarItem(HWND mHwnd, UAHDRAWMENUITEM* pUDMI) noexcept;
#endif

	/// <summary>
	/// Get the dialogs name.
	/// </summary>
	/// <returns>TString - Dialogs name</returns>
	inline const TString& getName() const noexcept { return m_tsName; }
	/// <summary>
	/// Get alias name for this dialog
	/// </summary>
	/// <returns>TString - Alias name</returns>
	inline const TString& getAliasName() const noexcept { return m_tsAliasName; }
	/// <summary>
	/// Get background brush
	/// </summary>
	/// <returns>HBRUSH - Colour brush</returns>
	inline const HBRUSH& getBackClrBrush() const noexcept { return m_hBackBrush; }
	/// <summary>
	/// Get the ID of the control that has focus.
	/// </summary>
	/// <returns>UINT - ID number</returns>
	inline const UINT& getFocusControl() const noexcept { return m_FocusID; };
	/// <summary>
	/// Sets this dialogs parent name
	/// </summary>
	/// <param name="strParent">- TString - Name of parent</param>
	inline void setParentName(const TString& strParent) { m_tsParentName = strParent; }
	/// <summary>
	/// Gets the parent name.
	/// </summary>
	/// <returns>TString - The parents name.</returns>
	inline const TString& getParentName() const noexcept { return m_tsParentName; }
	/// <summary>
	/// Get the current cursor.
	/// </summary>
	/// <returns>HCURSOR - Current cursor</returns>
	inline const HCURSOR& getCursor() const noexcept { return m_hCursor.cursor; };
	/// <summary>
	/// Get the current cursor for a specific zone
	/// </summary>
	/// <param name="wHitCode">- WORD - Zone code</param>
	/// <returns>HCURSOR - Current cursor for specified zone. Returns nullptr on failure.</returns>
	inline const HCURSOR& getCursor(const WORD wHitCode) const noexcept
	{
		GSL_SUPPRESS(bounds) if (wHitCode < std::size(m_hCursorList)) return gsl::at(m_hCursorList, wHitCode).cursor;
		return m_hCursor.cursor;
	};
	//inline const HWND& getToolTip(void) const noexcept { return m_ToolTipHWND; };

	inline const XPopupMenu* getXMenu() const noexcept {
		return m_popup.get();
	};

	void loadCursor(const TString &tsFlags, const TString &tsFilename);

	inline void incRef() const noexcept { ++m_iRefCount; };
	inline void decRef() const noexcept { --m_iRefCount; };
	inline const UINT& getRefCount() const noexcept { return m_iRefCount; };
	inline const HBITMAP& getBgBitmap() const noexcept { return m_BackgroundImage.m_hBitmap; };
	inline const COLORREF& getBgTransparentCol() const noexcept { return m_colTransparentBg; };
	inline const bool& IsVistaStyle(void) const noexcept { return m_bVistaStyle; };
	inline const HDC& GetVistaHDC(void) const noexcept { return m_hVistaHDC; };
	inline const HWND& GetVistaHWND(void) const noexcept { return m_hFakeHwnd; };
	inline const SIZE& GetVistaOffsets(void) const noexcept { return m_sVistaOffsets; };
	inline const HBITMAP& GetVistaBitmap(void) const noexcept { return m_hVistaBitmap; };
	inline void SetVerbose(const bool state) noexcept { m_bVerboseErrors = state; };
	inline const bool& IsVerbose(void) const noexcept { return m_bVerboseErrors; };
	inline const bool& isDialogActive(void) const noexcept { return m_bDialogActive; };	// returns dialogs active state
	inline const IntegerHash& getNamedIds(void) const noexcept { return m_NamedIds; };

	void showError(const TCHAR* const prop, const TCHAR* const cmd, const TCHAR* const err) const;
	void showErrorEx(const TCHAR* const prop, const TCHAR* const cmd, _Printf_format_string_ const TCHAR* const fmt, ...) const;
	void showControlError(const TCHAR* const prop, const TCHAR* const cmd, const TCHAR* const err) const;
	void showControlErrorEx(__in_z const TCHAR* const prop, __in_z const TCHAR* const cmd, _Printf_format_string_ const TCHAR* const fmt, ...) const;
#ifdef DCX_USE_GDIPLUS
	void DrawCaret(Gdiplus::Graphics& graph) const;
	void DrawCtrl(Gdiplus::Graphics& graphics, HDC hDC, HWND hWnd) const;
	void DrawDialog(Gdiplus::Graphics& graphics, HDC hDC) const;
#endif
	void CreateVistaStyle(void) noexcept;
	const bool CreateVistaStyleBitmap(const SIZE& szWin) noexcept;
	void RemoveVistaStyle(void) noexcept;
	void UpdateVistaStyle(const RECT* const rcUpdate = nullptr);
	void SetVistaStylePos(void) noexcept;
	void SetVistaStyleSize(void) noexcept;

	void CustomMenuCommand(const TString &tsArgs);

	/// <summary>
	/// Check if a name already exists on the internal list.
	/// </summary>
	/// <param name="NamedID">- TString - The named ID.</param>
	/// <returns>bool - true/false</returns>
	GSL_SUPPRESS(lifetime) const bool isNamedId(const TString& NamedID) const
	{
		const auto local_id = NamedID.to_<UINT>() + mIRC_ID_OFFSET;
		const auto itEnd = m_NamedIds.cend();

		const auto itGot = std::find_if(m_NamedIds.cbegin(), itEnd, [local_id, NamedID](const auto& arg) noexcept { return ((arg.second == local_id) || (arg.first == NamedID)); });

		return (itGot != itEnd);
	}

	/// <summary>
	/// Add a name and ID pair to the internal list.
	/// </summary>
	/// <param name="NamedID">- The name to add</param>
	/// <param name="local_id">- The ID to associate with the given name.</param>
	/// <returns>bool - true on success</returns>
	const bool AddNamedId(const TString& NamedID, const UINT local_id)
	{
		if (isNamedId(NamedID))
			return false;

		m_NamedIds[NamedID] = local_id;
		return true;
	}

	/// <summary>
	/// Convert a name to an ID
	/// </summary>
	/// <param name="NamedID">- The name to convert</param>
	/// <returns>UINT - The ID associated with that name or zero on failure.</returns>
	const UINT NameToID(const TString& NamedID) const
	{
		const auto local_id = NamedID.to_<UINT>() + mIRC_ID_OFFSET;

		for (const auto& [tsStoredName, uStoredID] : m_NamedIds)
		{
			if ((tsStoredName == NamedID) || (uStoredID == local_id))
				return uStoredID;
		}
		return 0U;
	}

	/// <summary>
	/// Convert a name to a UserID
	/// </summary>
	/// <param name="NamedID">- The name to convert</param>
	/// <returns>UINT - The UserID associated with that name or zero on failure.</returns>
	const UINT NameToUserID(const TString& NamedID) const
	{
		//const auto local_id = NamedID.to_<UINT>() + mIRC_ID_OFFSET;
		//for (const auto& [tsStoredName, uStoredID] : m_NamedIds)
		//{
		//	if ((tsStoredName == NamedID) || (uStoredID == local_id))
		//		return uStoredID - mIRC_ID_OFFSET;
		//}
		//return 0U;

		if (const auto local_id = NameToID(NamedID); local_id > mIRC_ID_OFFSET)
			return local_id - mIRC_ID_OFFSET;
		return 0U;
	}

	/// <summary>
	/// Convert an ID number to its name.
	/// </summary>
	/// <param name="local_id">- The ID number</param>
	/// <returns>TString - The name associated with that ID number. Returns an empty string on failure.</returns>
	const TString& IDToName(const UINT local_id) const noexcept
	{
		static const TString tsEmpty;

		for (const auto& [tsStoredName, uStoredID] : m_NamedIds)
		{
			if (uStoredID == local_id)
				return tsStoredName;
		}
		return tsEmpty;
	}

	/// <summary>
	/// Convert a User ID number to its name.
	/// </summary>
	/// <param name="local_id">- The UserID number</param>
	/// <returns>TString - The name associated with that ID number. Returns an empty string on failure.</returns>
	const TString& UserIDToName(const UINT local_id) const noexcept
	{
		return IDToName(local_id + mIRC_ID_OFFSET);
	}

	/// <summary>
	/// Get a unique ID
	/// </summary>
	/// <returns>UINT - A unique ID number. Returns zero on failure</returns>
	const UINT getUniqueID() const noexcept
	{
		for (auto iCount = 0U, i = mIRC_ID_OFFSET + DCX_NAMED_ID_OFFSET; iCount < mIRC_MAX_CONTROLS; ++iCount)
		{
			++i;

			if (isIDValid(i, true))
			{
				//const auto itEnd = namedIds.end();
				//const auto itGot = std::find_if(namedIds.begin(), itEnd, [i](const auto &arg) { return (arg.second == i); });
				//if (itGot == itEnd)
				//	return i;

				bool bFound = false;
				for (const auto& x : m_NamedIds)
				{
					if (x.second == i)
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
					return i;
			}
		}
		return 0U;
	}

	/// <summary>
	/// Delete named ID matching the id provided.
	/// </summary>
	/// <param name="local_id"></param>
	/// <returns></returns>
	bool deleteNamedID(const UINT local_id) noexcept
	{
		for (auto itStart = m_NamedIds.begin(), itEnd = m_NamedIds.end(); itStart != itEnd; ++itStart)
		{
			if (itStart->second == local_id)
			{
				m_NamedIds.erase(itStart);
				return true;
			}
		}
		return false;
	}

	/// <summary>
	/// Delete a named id matching the name provided.
	/// </summary>
	/// <param name="tsName"></param>
	/// <returns></returns>
	bool deleteNamedID(const TString& tsName)
	{
		if (const auto itEnd = m_NamedIds.end(), itGot = m_NamedIds.find(tsName); itGot != itEnd)
		{
			m_NamedIds.erase(itGot);
			return true;
		}
		return false;

		//return Dcx::eraseIfFound(m_NamedIds, tsName);
	}

	void MapVistaRect(HWND hwnd, LPRECT rc) const noexcept;

	void RegisterDragList(DcxList* const list);
	void UnregisterDragList(const DcxList* const list) noexcept;

	/// <summary>
	/// Load an icon or icons
	/// </summary>
	/// <param name="tsFlags"> - Flags to define how the icon is gotten from the file.</param>
	/// <param name="tsIndex"> - The icon indexes to load.</param>
	/// <param name="tsSrc"> - The icon file to load from. The type of file depends on the flags use.</param>
	void loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc) final;

	void toXml(TiXmlElement* const xml) const override;
	[[nodiscard("Memory Leak")]] TiXmlElement* toXml() const override;
	[[nodiscard("Memory Leak")]] TiXmlElement* toXml(const TString& name) const;
	void toXml(TiXmlElement* const xml, const TString& name) const;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) override;

	const VectorOfStyles& xmlGetStyles() const noexcept { return m_xmlStyles; }
	const VectorOfTemplates& xmlGetTemplates() const noexcept { return m_xmlTemplates; }
	VectorOfIcons& xmlGetIcons() const noexcept { return m_xmlIcons; }

	const bool isIDValid(_In_ const UINT ID, _In_ const bool bUnused = false) const noexcept;

	void createTooltip(const TString& tsFlags);

	static LRESULT WINAPI WindowProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void DrawDialogBackground(HDC hdc, DcxDialog* const p_this, LPCRECT rwnd);
	static const WindowAnimStyle getAnimateStyles(const TString& flags) noexcept;

private:

	IntegerHash m_NamedIds; //!< map of named Id's
	TString m_tsName;       //!< Dialog Name
	TString m_tsAliasName;  //!< Callback Alias Name
	TString m_tsParentName; //!< Parent name (only if docked)

	VectorOfControlPtrs m_vpControls; //!< Vector of DCX Controls
	VectorOfInts m_vZLayers;
	VectorOfInts::size_type m_zLayerCurrent{};

	VectorOfDragListPtrs m_vDragLists; //!< Registered draglists

	VectorOfStyles m_xmlStyles;			//!< styles for use in this dialog (only valid during xml loading)
	VectorOfTemplates m_xmlTemplates;
	mutable VectorOfIcons m_xmlIcons;

	HBRUSH m_hBackBrush{ nullptr };    //!< Background control color

	UINT m_MouseID{}; //!< Mouse Hover ID
	UINT m_FocusID{}; //!< Mouse Focus ID
	UINT m_uStyleBg{ DBS_BKGNORMAL };
	mutable UINT m_iRefCount{};

	std::unique_ptr<XPopupMenu> m_popup;

	std::unique_ptr<LayoutManager> m_pLayoutManager; //!< Layout Manager Object

	CursorPair m_hCursor;
	CursorPair m_hCursorList[22];

	//HBITMAP m_bitmapBg{ nullptr };
	dcxImage m_BackgroundImage;

	HBITMAP m_hVistaBitmap{ nullptr };

	COLORREF m_colTransparentBg{ RGB(255,0,255) };
	COLORREF m_cKeyColour{ CLR_NONE };

	//HWND m_ToolTipHWND{ nullptr }; //!< Dialogs general tooltip control for use with all controls that don't have their own tooltips.
	HWND m_hFakeHwnd{ nullptr };

	std::byte m_iAlphaLevel{ 255 };
	std::byte m_uGhostDragAlpha{ 255 };

	bool m_bTracking{ false };
	bool m_bInSizing{ false }; //!< In Moving Motion
	bool m_bInMoving{ false }; //!< In Sizing Motion
	bool m_bDoDrag{ false };
	bool m_bDrag{ false };
	bool m_bGhosted{ false };
	bool m_bVerboseErrors{ true }; //!< Should all errors be echo'd to status?
	bool m_bHaveKeyColour{ false };
	bool m_bVistaStyle{ false };
	mutable bool m_bErrorTriggered{ false };		// set to true when an error has been triggered & used to avoid error loops
	mutable bool m_bDialogActive{ true };		// set to true when dialog is active
	static inline bool m_bIsMenuBar{ false };
	static inline bool m_bIsSysMenu{ false };

	PVOID m_pVistaBits{ nullptr };	// gfx buffer, doesnt need freed.

	HDC m_hVistaHDC{ nullptr };	// temp hdc storage, doesnt need freed.

	SIZE m_sVistaOffsets{};

	RECT m_GlassOffsets{};

	// settings for drawing the dialogs menu bar.
	XPMENUBAR m_CustomMenuBar;
	HMENU m_hMenuBackup{};

	void i_showError(const TCHAR* const cType, const TCHAR* const prop, const TCHAR* const cmd, const TCHAR* const err) const;
	void PreloadData(void) noexcept;

	void setBorderStyles(const TString& tsStyles);

	WNDPROC m_hDefaultDialogProc{ nullptr }; //!< Old Window Procedure
	LRESULT CallDefaultProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

	void xmlParseElements(const TString& tsPath, const TiXmlElement* xParent, __maybenull const TiXmlElement* xTemplate);
	void xmlAddPane(const TString& tsParentPath, const TString& tsCurrentPath, const TiXmlElement* xElement, __maybenull const TiXmlElement* xTemplate);
	bool xmlAddControl(const TString& tsParentPath, const TString& tsCurrentPath, const TiXmlElement * xParent, const TiXmlElement* xCtrl);
	void xmlCallTemplate(const TString& tsCurrentPath, const TiXmlElement* xParent, const TiXmlElement* xCallTemplate);

	/// <summary>
	/// Sets the dialogs style, as in font and colours
	/// </summary>
	/// <param name="xStyle">- Pointer to the &lt;style&gt; tag to apply or to the &lt;dialog&gt; its self if no &lt;style&gt; applies.</param>
	void xmlSetStyle(const TiXmlElement* xStyle) final;

	void xmlSetStyles();
	void xmlSetIcons();

	void xmlbuildStylesList(const TiXmlElement* xElement);
	void xmlbuildIconsList(const TiXmlElement* xElement);
	void xmlbuildTemplatesList(const TiXmlElement* xElement);

	static void xmlLoadMenubarColours(const TiXmlElement* xParent, XPMENUBARCOLORS& mColours) noexcept;
	static void xmlSaveMenubarColours(TiXmlElement* xParent, const XPMENUBARCOLORS& mColours);
	static void xmlLoadMenubar(const TiXmlElement* xMenubar, XPMENUBAR& mMenubar);
	static void xmlSaveMenubar(TiXmlElement* xParent, const XPMENUBAR& mMenuBar);

	static std::pair<WindowStyle, WindowExStyle> parseBorderStyles(const TString& flags) noexcept;
	static const UINT parseBkgFlags(const TString& flags) noexcept;
	static const UINT parseFlashFlags(const TString& flags) noexcept;
	static const UINT parseTooltipFlags(const TString& flags) noexcept;
	static TString BkgFlagsToString(UINT uFlags);
	static TString TooltipFlagsToString(UINT uFlags);

	//// Helper to calculate the alpha-premultiled value for a pixel
	//constexpr static inline const DWORD PreMultiply(const COLORREF cl, const unsigned char nAlpha) noexcept
	//{
	//	// It's strange that the byte order of RGB in 32b BMP is reverse to in COLORREF
	//	const DWORD dAlpha = gsl::narrow_cast<const DWORD>(nAlpha);
	//	return (GetRValue(cl) * dAlpha / 255) << 16 |
	//		(GetGValue((cl & 0xFFFF)) * dAlpha / 255) << 8 |
	//		(GetBValue(cl) * dAlpha / 255);
	//}
	static LRESULT ProcessDragListMessage(DcxDialog* const p_this, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed);
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXDIALOG_H_
