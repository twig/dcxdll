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

#define DBS_BKGCOLOR		0x00000001 //!< Control Background Color
#define DBS_BKGBITMAP	0x00000002 //!< Control Background Bitmap

#define DBS_BKGNORMAL	0x00000004 // top left, no sizing applied
#define DBS_BKGTILE		0x00000008 // tiled across whole dialog
#define DBS_BKGSTRETCH	0x00000010 // stretched to fit

// used with DBS_BKGNORMAL
#define DBS_BKGCENTER	0x00000020 // centered horizontally
#define DBS_BKGVCENTER	0x00000040 // centered vertically
#define DBS_BKGRIGHT		0x00000080 // right aligned
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

//// Shadow Status Flags
//enum ShadowStatus: UINT
//{
//	DCX_SS_ENABLED = 1,	// Shadow is enabled, if not, the following one is always false
//	DCX_SS_VISABLE = 1 << 1,	// Shadow window is visible
//	DCX_SS_PARENTVISIBLE = 1<< 2,	// Parent window is visible, if not, the above one is always false
//	DCX_SS_DISABLEDBYAERO = 1 << 3	// Shadow is enabled, but do not show because areo is enabled
//};

// dummy runtime classe definition
class DcxControl;
class DcxList;
//class Dcx;

typedef std::vector<DcxControl *> VectorOfControlPtrs; //!< blah
typedef std::vector<DcxList *> VectorOfDragListPtrs; //!< Vector of draglists
typedef std::map<TString, UINT> IntegerHash;

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

	const TString &getName() const;
	const TString &getAliasName( ) const;
	
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
	const UINT &getFocusControl() const { return m_FocusID; };

	void setParentName(const TString &strParent);
	const TString &getParentName() const;

	LayoutManager * m_pLayoutManager; //!< Layout Manager Object

	inline const HCURSOR &getCursor( ) const { return this->m_hCursor; };
	inline const HWND &getToolTip(void) const { return this->m_ToolTipHWND; };
	inline void incRef( ) { ++this->m_iRefCount; };
	inline void decRef( ) { --this->m_iRefCount; };
	inline const UINT &getRefCount( ) const { return this->m_iRefCount; };
	inline const DWORD &getEventMask( ) const { return this->m_dEventMask; };
	inline const HBITMAP &getBgBitmap() const { return this->m_bitmapBg; };
	inline const COLORREF &getBgTransparentCol() const { return this->m_colTransparentBg; };
	static void DrawDialogBackground(HDC hdc, DcxDialog *const p_this, LPCRECT rwnd);

	//const bool AddShadow(void);
	//void RemoveShadow(void);
	//void UpdateShadow(void);
	//const bool isShadowed(void) const;
	//const bool SetShadowSize(const int NewSize = 0);
	//const bool SetShadowSharpness(const UINT NewSharpness = 5);
	//const bool SetShadowDarkness(const UINT NewDarkness = 200);
	//const bool SetShadowPosition(const int NewXOffset = 5, const int NewYOffset = 5);
	//const bool SetShadowColor(const COLORREF NewColor = 0);
	//void ShowShadow(void);

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
	const bool &IsVistaStyle(void) const { return this->m_bVistaStyle; };
	const HDC &GetVistaHDC(void) const { return this->m_hVistaHDC; };
	const HWND &GetVistaHWND(void) const { return this->m_hFakeHwnd; };
	const SIZE &GetVistaOffsets(void) const { return this->m_sVistaOffsets; };
	const HBITMAP &GetVistaBitmap(void) const { return this->m_hVistaBitmap; };
	
	IntegerHash namedIds; //!< map of named Id's

	const IntegerHash &getNamedIds(void) const { return this->namedIds; };
	const bool isNamedId(const TString &NamedID) const { return (namedIds.find(NamedID) != namedIds.end()); }
	const bool AddNamedId(const TString &NamedID, const UINT local_id)
	{
		if (isNamedId(NamedID))
			return false;

		namedIds[NamedID] = local_id;
		return true;
	}
	const UINT NameToID(const TString &NamedID) const
	{
		auto it = namedIds.find(NamedID);
		if (it != namedIds.end())
			return it->second;

		return 0;
	}
	const TString &IDToName(const UINT local_id) const
	{
		//for (auto it = namedIds.begin(), itEnd = namedIds.end(); it != itEnd; ++it)
		//{
		//	if (local_id == it->second)
		//		return it->first;
		//}
		//return TEXT("");

		const auto itEnd = namedIds.end();
		const auto itGot = std::find_if(namedIds.begin(), itEnd, [local_id](IntegerHash::const_reference arg) { return (arg.second == local_id); });
		if (itGot != itEnd)
			return itGot->first;

		return TEXT("");
	}
	const UINT getUniqueID() const
	{
		for (auto iCount = 0U, i = mIRC_ID_OFFSET; iCount < mIRC_MAX_CONTROLS; iCount++) {
			++i;
			//UINT i = 0;
			//if (rand_s(&i) != 0)
			//	throw std::runtime_error("Unable to Generate ID");
			//
			//if (i <= mIRC_ID_OFFSET)
			//	i += mIRC_ID_OFFSET + 1;

			if (isIDValid(i, true))
			{
				//bool bIDTaken = false;
				//for (auto it = namedIds.begin(), itEnd = namedIds.end(); it != itEnd; ++it)
				//{
				//	if (i == it->second)
				//	{
				//		bIDTaken = true;
				//		break;
				//	}
				//}
				//if (!bIDTaken)
				//	return i;

				const auto itEnd = namedIds.end();
				const auto itGot = std::find_if(namedIds.begin(), itEnd, [i](IntegerHash::const_reference arg) { return (arg.second == i); });
				if (itGot == itEnd)
					return i;
			}
		}
		return 0;
	}

	void MapVistaRect(HWND hwnd, LPRECT rc) const;

	void RegisterDragList(DcxList *const list);
	void UnregisterDragList(const DcxList *const list);

	void SetVerbose(const bool state) { this->m_bVerboseErrors = state; };
	const bool &IsVerbose(void) const { return this->m_bVerboseErrors; };

	void toXml(TiXmlElement *const xml) const;
	TiXmlElement * toXml() const;
	TiXmlElement * toXml(const TString &name) const;
	void toXml(TiXmlElement *const xml, const TString &name) const;

	const bool isIDValid(const UINT ID, const bool bUnused = false) const;
	const bool &isDialogActive(void) const { return m_bDialogActive; };	// returns dialogs active state

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

	//struct {
	//	HWND hWin; //!< The shadow window.
	//	// Restore last parent window size, used to determine the update strategy when parent window is resized
	//	LPARAM WndSize;
	//	COLORREF Color;	// Color of shadow
	//
	//	BYTE Status; //!< The shadow windows status.
	//	unsigned char nDarkness;	// Darkness, transparency of blurred area
	//	unsigned char nSharpness;	// Sharpness, width of blurred border of shadow window
	//	signed char nSize;	// Shadow window size, relative to parent window size
	//
	//	// The X and Y offsets of shadow window,
	//	// relative to the parent window, at center of both windows (not top-left corner), signed
	//	signed char nxOffset;
	//	signed char nyOffset;
	//
	//	// Set this to true if the shadow should not be update until next WM_PAINT is received
	//	bool bUpdate;
	//} m_Shadow;
	/* **** */

	PVOID m_pVistaBits;

	HDC m_hVistaHDC;

	SIZE m_sVistaOffsets;

	RECT m_GlassOffsets;

	//// Fill in the shadow window alpha blend bitmap with shadow image pixels
	//void MakeShadow(UINT32 *const pShadBits, const HWND hParent, const RECT *const rcParent);

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
