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

// Event mask defines.
#define DCX_EVENT_MOUSE				0x00000001
#define DCX_EVENT_FOCUS				0x00000002
#define DCX_EVENT_THEME				0x00000004
#define DCX_EVENT_MOVE				0x00000008
#define DCX_EVENT_CLOSE				0x00000010
#define DCX_EVENT_SIZE				0x00000020
#define DCX_EVENT_CLICK				0x00000040
#define DCX_EVENT_DRAG				0x00000080
#define DCX_EVENT_HELP				0x00000100
#define DCX_EVENT_EDIT				0x00000200
#define DCX_EVENT_ALL				0xFFFFFFFF

#define DCX_NAMED_ID_OFFSET 3000U

// dummy runtime classe definition
class DcxControl;
class DcxList;
//class Dcx;

using VectorOfControlPtrs = std::vector<DcxControl *>; //!< blah
using VectorOfDragListPtrs = std::vector<DcxList *>; //!< Vector of draglists
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

class DcxDialog : public DcxWindow {

public:
	DcxDialog() = delete;
	DcxDialog(const DcxDialog &other) = delete;
	DcxDialog &operator =(const DcxDialog &) = delete;	// No assignments!

	DcxDialog( const HWND mHwnd, const TString & tsName, const TString & tsAliasName);
	virtual ~DcxDialog( );

	void parseCommandRequest( const TString & input );
	void parseCommandRequestEX(const TCHAR *const szFormat, ...);
	void parseComControlRequestEX(const UINT id, const TCHAR *const szFormat, ...);
	//void parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const;

	bool evalAliasEx( TCHAR *const szReturn, const int maxlen, const TCHAR *const szFormat, ... );
	bool evalAlias( TCHAR *const szReturn, const int maxlen, const TCHAR *const szArgs);
	bool execAliasEx( const TCHAR *const szFormat, ... );
	bool execAlias( const TCHAR *const szArgs);

	template <typename Format, typename Value, typename... Arguments>
	std::pair<bool,TString> evalAliasT(const Format &fmt, const Value val, Arguments&&... args)
	{
		TString tsArgs;
		return evalAliasT(_ts_sprintf(tsArgs, fmt, val, args...));
	}
	template <typename Format, typename Value>
	std::pair<bool, TString> evalAliasT(const Format &fmt, const Value val)
	{
		TString tsArgs;
		return evalAliasT(_ts_sprintf(tsArgs, fmt, val));
	}
	template <typename Value>
	std::pair<bool, TString> evalAliasT(const Value val)
	{
		TString tsArgs, tsRes;
		_ts_sprintf(tsArgs, TEXT("$%(%,%)"), getAliasName(), getName(), MakeTextmIRCSafe(val));

		incRef();
		Auto(decRef());

		const auto bRes = mIRCLinker::tsEval(tsRes, tsArgs.to_chr());

		return{ bRes, tsRes };
	}

	DcxControl * getControlByID( const UINT ID ) const;
	DcxControl * getControlByHWND( const HWND mHwnd ) const;

	void addControl( DcxControl * p_Control );
	DcxControl *addControl(const TString & input, const UINT offset, const UINT64 mask, HWND hParent);
	void deleteControl(const DcxControl *const p_Control);

	const bool updateLayout( RECT & rc );

	void setMouseControl( const UINT mUID );
	void setFocusControl( const UINT mUID );

	inline const TString &getName() const noexcept { return m_tsName; }
	inline const TString &getAliasName() const noexcept { return m_tsAliasName; }
	inline const HBRUSH &DcxDialog::getBackClrBrush() const noexcept { return m_hBackBrush; }
	inline const UINT &getFocusControl() const noexcept { return m_FocusID; };
	inline void setParentName(const TString &strParent) { m_tsParentName = strParent; }
	inline const TString &getParentName() const noexcept { return m_tsParentName; }
	inline const HCURSOR &getCursor( ) const noexcept { return m_hCursor; };
	inline const HCURSOR &getCursor(const WORD wHitCode) const noexcept {
		if (wHitCode < _countof(m_hCursorList)) return m_hCursorList[wHitCode].first;
		return m_hCursor;
	};
	inline const HWND &getToolTip(void) const noexcept { return m_ToolTipHWND; };
	inline void incRef( ) noexcept { ++m_iRefCount; };
	inline void decRef( ) noexcept { --m_iRefCount; };
	inline const UINT &getRefCount( ) const noexcept { return m_iRefCount; };
	inline const DWORD &getEventMask( ) const noexcept { return m_dEventMask; };
	inline const HBITMAP &getBgBitmap() const noexcept { return m_bitmapBg; };
	inline const COLORREF &getBgTransparentCol() const noexcept { return m_colTransparentBg; };
	inline const bool &IsVistaStyle(void) const noexcept { return m_bVistaStyle; };
	inline const HDC &GetVistaHDC(void) const noexcept { return m_hVistaHDC; };
	inline const HWND &GetVistaHWND(void) const noexcept { return m_hFakeHwnd; };
	inline const SIZE &GetVistaOffsets(void) const noexcept { return m_sVistaOffsets; };
	inline const HBITMAP &GetVistaBitmap(void) const noexcept { return m_hVistaBitmap; };
	inline void SetVerbose(const bool state) noexcept { m_bVerboseErrors = state; };
	inline const bool &IsVerbose(void) const noexcept { return m_bVerboseErrors; };
	inline const bool &isDialogActive(void) const noexcept { return m_bDialogActive; };	// returns dialogs active state
	inline const IntegerHash &getNamedIds(void) const noexcept { return m_NamedIds; };

	void showError(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const err) const;
	void showErrorEx(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const fmt, ...) const;
	void showControlError(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const err) const;
	void showControlErrorEx(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const fmt, ...) const;
#ifdef DCX_USE_GDIPLUS
	void DrawCaret(Gdiplus::Graphics & graph);
	void DrawCtrl(Gdiplus::Graphics & graphics, HDC hDC, HWND hWnd);
	void DrawDialog(Gdiplus::Graphics & graphics, HDC hDC);
#endif
	void CreateVistaStyle(void);
	const bool CreateVistaStyleBitmap(const SIZE &szWin);
	void RemoveVistaStyle(void);
	void UpdateVistaStyle(const LPRECT rcUpdate = nullptr);
	void SetVistaStylePos(void);
	void SetVistaStyleSize(void);

	const bool isNamedId(const TString &NamedID) const {
		const auto local_id = NamedID.to_<UINT>() + mIRC_ID_OFFSET;
		const auto itEnd = m_NamedIds.end();

		const auto itGot = std::find_if(m_NamedIds.begin(), itEnd, [local_id,NamedID](const auto &arg) { return ((arg.second == local_id) || (arg.first == NamedID)); });

		return (itGot != itEnd);
	}
	const bool AddNamedId(const TString &NamedID, const UINT local_id)
	{
		if (isNamedId(NamedID))
			return false;

		m_NamedIds[NamedID] = local_id;
		return true;
	}
	const UINT NameToID(const TString &NamedID) const
	{
		const auto local_id = NamedID.to_<UINT>() + mIRC_ID_OFFSET;

		for (const auto &x : m_NamedIds)
		{
			if ((x.first == NamedID) || (x.second == local_id))
				return x.second;
		}
		return 0U;
	}
	const UINT NameToUserID(const TString &NamedID) const
	{
		const auto local_id = NamedID.to_<UINT>() + mIRC_ID_OFFSET;
		//const auto itEnd = namedIds.end();

		//const auto itGot = std::find_if(namedIds.begin(), itEnd, [local_id, NamedID](const auto &arg) { return ((arg.second == local_id) || (arg.first == NamedID)); });
		//if (itGot != itEnd)
		//	return itGot->second - mIRC_ID_OFFSET;

		for (const auto &x : m_NamedIds)
		{
			if ((x.first == NamedID) || (x.second == local_id))
				return x.second - mIRC_ID_OFFSET;
		}

		return 0U;
	}
	const TString &IDToName(const UINT local_id) const
	{
		//const auto itEnd = namedIds.end();
		//const auto itGot = std::find_if(namedIds.begin(), itEnd, [local_id](const auto &arg) { return (arg.second == local_id); });
		//if (itGot != itEnd)
		//	return itGot->first;

		for (const auto &x : m_NamedIds)
		{
			if (x.second == local_id)
				return x.first;
		}

		//return TString();
		return {};
	}
	const UINT getUniqueID() const
	{
		for (auto iCount = 0U, i = mIRC_ID_OFFSET + DCX_NAMED_ID_OFFSET; iCount < mIRC_MAX_CONTROLS; iCount++) {
			++i;

			if (isIDValid(i, true))
			{
				//const auto itEnd = namedIds.end();
				//const auto itGot = std::find_if(namedIds.begin(), itEnd, [i](const auto &arg) { return (arg.second == i); });
				//if (itGot == itEnd)
				//	return i;

				bool bFound = false;
				for (const auto &x : m_NamedIds)
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
	bool deleteNamedID(const UINT local_id)
	{
		//const auto itEnd = namedIds.end();
		//const auto itGot = std::find_if(namedIds.begin(), itEnd, [local_id](const auto &arg) { return (arg.second == local_id); });
		//if (itGot != itEnd)
		//{
		//	namedIds.erase(itGot);
		//	return true;
		//}
		//return false;

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
	bool deleteNamedID(const TString &tsName)
	{
		const auto itEnd = m_NamedIds.end();
		const auto itGot = m_NamedIds.find(tsName);
		if (itGot != itEnd)
		{
			m_NamedIds.erase(itGot);
			return true;
		}
		return false;
	}

	void MapVistaRect(HWND hwnd, LPRECT rc) const;

	void RegisterDragList(DcxList *const list);
	void UnregisterDragList(const DcxList *const list);

	void toXml(TiXmlElement *const xml) const;
	TiXmlElement * toXml() const;
	TiXmlElement * toXml(const TString &name) const;
	void toXml(TiXmlElement *const xml, const TString &name) const;

	const bool isIDValid(_In_ const UINT ID, _In_ const bool bUnused = false) const;

	static LRESULT WINAPI WindowProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void DrawDialogBackground(HDC hdc, DcxDialog *const p_this, LPCRECT rwnd);
	static const DWORD getAnimateStyles(const TString & flags) noexcept;

private:

	IntegerHash m_NamedIds; //!< map of named Id's
	TString m_tsName;       //!< Dialog Name
	TString m_tsAliasName;  //!< Callback Alias Name
	TString m_tsParentName; //!< Parent name (only if docked)

	//WNDPROC m_hOldWindowProc; //!< Dialog Old Window Procedure

	VectorOfControlPtrs m_vpControls; //!< Vector of DCX Controls
	VectorOfInts m_vZLayers;
	VectorOfInts::size_type m_zLayerCurrent;

	VectorOfDragListPtrs m_vDragLists; //!< Registered draglists

	HBRUSH m_hBackBrush;    //!< Background control color

	UINT m_MouseID; //!< Mouse Hover ID
	UINT m_FocusID; //!< Mouse Hover ID
	UINT m_uStyleBg;
	UINT m_iRefCount;

	LayoutManager * m_pLayoutManager; //!< Layout Manager Object

	XPopupMenu * m_popup;

	HCURSOR m_hCursor;  //!< Cursor Handle
	std::pair<HCURSOR, bool> m_hCursorList[22];

	HBITMAP m_bitmapBg;
	HBITMAP m_hVistaBitmap;

	COLORREF m_colTransparentBg;
	COLORREF m_cKeyColour;

	HWND m_ToolTipHWND; //!< Dialogs general tooltip control for use with all controls that don't have their own tooltips.
	HWND m_hFakeHwnd;

	DWORD m_dEventMask;

	BYTE m_iAlphaLevel;
	BYTE m_uGhostDragAlpha;
	BOOL m_bTracking;

	bool m_bInSizing; //!< In Moving Motion
	bool m_bInMoving; //!< In Sizing Motion
	bool m_bCursorFromFile; //!< Cursor comes from a file?
	bool m_bDoDrag;
	bool m_bDrag;
	bool m_bGhosted;
	bool m_bVerboseErrors; //!< Should all errors be echo'd to status?
	bool m_bHaveKeyColour;
	bool m_bVistaStyle;
	mutable bool m_bErrorTriggered;		// set to true when an error has been triggered & used to avoid error loops
	mutable bool m_bDialogActive;		// set to true when dialog is active
	static bool m_bIsMenuBar;
	static bool m_bIsSysMenu;
	bool m_bReserved;				//!< Reserved for future use.

	PVOID m_pVistaBits;

	HDC m_hVistaHDC;

	SIZE m_sVistaOffsets;

	RECT m_GlassOffsets;

	void i_showError(const TCHAR *const cType, const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const err) const;
	void PreloadData(void);

	static void parseBorderStyles(const TString & flags, LONG *const Styles, LONG *const ExStyles) noexcept;
	static const UINT parseBkgFlags(const TString & flags) noexcept;
	static const UINT parseFlashFlags(const TString & flags) noexcept;
	static const UINT parseTooltipFlags(const TString &flags) noexcept;

	// Helper to calculate the alpha-premultiled value for a pixel
	constexpr static inline const DWORD PreMultiply(const COLORREF cl, const unsigned char nAlpha) noexcept
	{
		// It's strange that the byte order of RGB in 32b BMP is reverse to in COLORREF
		const DWORD dAlpha = static_cast<const DWORD>(nAlpha);
		return (GetRValue(cl) * dAlpha / 255) << 16 |
			(GetGValue((cl & 0xFFFF)) * dAlpha / 255) << 8 |
			(GetBValue(cl) * dAlpha / 255);
	}
	static LRESULT ProcessDragListMessage(DcxDialog *const p_this, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed);
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXDIALOG_H_
