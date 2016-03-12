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
 * © ScriptsDB.org - 2006
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

	DcxDialog( const HWND mHwnd, const TString & tsName, const TString & tsAliasName);
	virtual ~DcxDialog( );

	DcxDialog &operator =(const DcxDialog &) = delete;	// No assignments!

	const TString &getName() const noexcept;
	const TString &getAliasName( ) const noexcept;
	
	static LRESULT WINAPI WindowProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void parseCommandRequest( const TString & input );
	void parseCommandRequestEX(const TCHAR *const szFormat, ...);
	void parseComControlRequestEX(const UINT id, const TCHAR *const szFormat, ...);
	void parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const;

	bool evalAliasEx( TCHAR *const szReturn, const int maxlen, const TCHAR *const szFormat, ... );
	bool evalAlias( TCHAR *const szReturn, const int maxlen, const TCHAR *const szArgs);
	bool execAliasEx( const TCHAR *const szFormat, ... );
	bool execAlias( const TCHAR *const szArgs);

	DcxControl * getControlByID( const UINT ID ) const;
	DcxControl * getControlByHWND( const HWND mHwnd ) const;
	//DcxControl * m_pCacheControl;

	void addControl( DcxControl * p_Control );
	DcxControl *addControl(const TString & input, const UINT offset, const UINT64 mask, HWND hParent);
	void deleteControl(const DcxControl *const p_Control);
	//void deleteAllControls( );

	const bool updateLayout( RECT & rc );

	const HBRUSH &getBackClrBrush( ) const;

	void setMouseControl( const UINT mUID );
	void setFocusControl( const UINT mUID );
	const UINT &getFocusControl() const noexcept { return m_FocusID; };

	void setParentName(const TString &strParent);
	const TString &getParentName() const noexcept;

	LayoutManager * m_pLayoutManager; //!< Layout Manager Object

	inline const HCURSOR &getCursor( ) const noexcept { return this->m_hCursor; };
	inline const HCURSOR &getCursor(const WORD wHitCode) const noexcept {
		if (wHitCode < _countof(m_hCursorList)) return m_hCursorList[wHitCode].first;
		return m_hCursor;
	};
	inline const HWND &getToolTip(void) const noexcept { return this->m_ToolTipHWND; };
	inline void incRef( ) noexcept { ++this->m_iRefCount; };
	inline void decRef( ) noexcept { --this->m_iRefCount; };
	inline const UINT &getRefCount( ) const noexcept { return this->m_iRefCount; };
	inline const DWORD &getEventMask( ) const noexcept { return this->m_dEventMask; };
	inline const HBITMAP &getBgBitmap() const noexcept { return this->m_bitmapBg; };
	inline const COLORREF &getBgTransparentCol() const noexcept { return this->m_colTransparentBg; };
	static void DrawDialogBackground(HDC hdc, DcxDialog *const p_this, LPCRECT rwnd);

	static const DWORD getAnimateStyles(const TString & flags);
	void showError(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const err) const;
	void showErrorEx(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const fmt, ...) const;
	void showControlError(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const err) const;
	void showControlErrorEx(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const fmt, ...) const;
#ifdef DCX_USE_GDIPLUS
	void DrawCaret(Graphics & graph);
	void DrawCtrl( Graphics & graphics, HDC hDC, HWND hWnd);
	void DrawDialog( Graphics & graphics, HDC hDC);
#endif
	void CreateVistaStyle(void);
	const bool CreateVistaStyleBitmap(const SIZE &szWin);
	void RemoveVistaStyle(void);
	void UpdateVistaStyle(const LPRECT rcUpdate = nullptr);
	void SetVistaStylePos(void);
	void SetVistaStyleSize(void);
	const bool &IsVistaStyle(void) const noexcept { return this->m_bVistaStyle; };
	const HDC &GetVistaHDC(void) const noexcept { return this->m_hVistaHDC; };
	const HWND &GetVistaHWND(void) const noexcept { return this->m_hFakeHwnd; };
	const SIZE &GetVistaOffsets(void) const noexcept { return this->m_sVistaOffsets; };
	const HBITMAP &GetVistaBitmap(void) const noexcept { return this->m_hVistaBitmap; };
	
	IntegerHash namedIds; //!< map of named Id's

	const IntegerHash &getNamedIds(void) const noexcept { return this->namedIds; };
	const bool isNamedId(const TString &NamedID) const {
		const auto local_id = NamedID.to_<UINT>() + mIRC_ID_OFFSET;
		const auto itEnd = namedIds.end();

		const auto itGot = std::find_if(namedIds.begin(), itEnd, [local_id,NamedID](const auto &arg) { return ((arg.second == local_id) || (arg.first == NamedID)); });

		return (itGot != itEnd);
	}
	const bool AddNamedId(const TString &NamedID, const UINT local_id)
	{
		if (isNamedId(NamedID))
			return false;

		namedIds[NamedID] = local_id;
		return true;
	}
	const UINT NameToID(const TString &NamedID) const
	{
		const auto local_id = NamedID.to_<UINT>() + mIRC_ID_OFFSET;
		//const auto itEnd = namedIds.end();

		//const auto itGot = std::find_if(namedIds.begin(), itEnd, [local_id, NamedID](const auto &arg) { return ((arg.second == local_id) || (arg.first == NamedID)); });
		//if (itGot != itEnd)
		//	return itGot->second;

		for (const auto &x : namedIds)
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

		for (const auto &x : namedIds)
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

		for (const auto &x : namedIds)
		{
			if (x.second == local_id)
				return x.first;
		}

		return TString();
	}
	const UINT getUniqueID() const
	{
		for (auto iCount = 0U, i = mIRC_ID_OFFSET; iCount < mIRC_MAX_CONTROLS; iCount++) {
			++i;

			if (isIDValid(i, true))
			{
				//const auto itEnd = namedIds.end();
				//const auto itGot = std::find_if(namedIds.begin(), itEnd, [i](const auto &arg) { return (arg.second == i); });
				//if (itGot == itEnd)
				//	return i;

				bool bFound = false;
				for (const auto &x : namedIds)
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

		for (auto itStart = namedIds.begin(), itEnd = namedIds.end(); itStart != itEnd; ++itStart)
		{
			if (itStart->second == local_id)
			{
				namedIds.erase(itStart);
				return true;
			}
		}
		return false;
	}
	bool deleteNamedID(const TString &tsName)
	{
		const auto itEnd = namedIds.end();
		const auto itGot = namedIds.find(tsName);
		if (itGot != itEnd)
		{
			namedIds.erase(itGot);
			return true;
		}
		return false;
	}

	void MapVistaRect(HWND hwnd, LPRECT rc) const;

	void RegisterDragList(DcxList *const list);
	void UnregisterDragList(const DcxList *const list);

	void SetVerbose(const bool state) noexcept { this->m_bVerboseErrors = state; };
	const bool &IsVerbose(void) const noexcept { return this->m_bVerboseErrors; };

	void toXml(TiXmlElement *const xml) const;
	TiXmlElement * toXml() const;
	TiXmlElement * toXml(const TString &name) const;
	void toXml(TiXmlElement *const xml, const TString &name) const;

	const bool isIDValid(_In_ const UINT ID, _In_ const bool bUnused = false) const;
	const bool &isDialogActive(void) const noexcept { return m_bDialogActive; };	// returns dialogs active state

protected:

	TString m_tsName;       //!< Dialog Name
	TString m_tsAliasName;  //!< Callback Alias Name
	TString m_tsParentName; //!< Parent name (only if docked)

	WNDPROC m_hOldWindowProc; //!< Dialog Old Window Procedure

	VectorOfControlPtrs m_vpControls; //!< Vector of DCX Controls
	VectorOfInts m_vZLayers;
	VectorOfInts::size_type m_zLayerCurrent;

	VectorOfDragListPtrs m_vDragLists; //!< Registered draglists

	HBRUSH m_hBackBrush;    //!< Background control color

	UINT m_MouseID; //!< Mouse Hover ID
	UINT m_FocusID; //!< Mouse Hover ID
	UINT m_uStyleBg;
	UINT m_iRefCount;

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
	BYTE m_bDoGhostDrag;
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

	void PreloadData(void);

	static void parseBorderStyles(const TString & flags, LONG *const Styles, LONG *const ExStyles);
	static const UINT parseBkgFlags(const TString & flags);
	static const UINT parseFlashFlags(const TString & flags);
	static const UINT parseTooltipFlags(const TString &flags);

	// Helper to calculate the alpha-premultiled value for a pixel
	static inline const DWORD PreMultiply(const COLORREF cl, const unsigned char nAlpha)
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
