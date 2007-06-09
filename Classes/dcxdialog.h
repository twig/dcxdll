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

// Shadow Status Flags
enum ShadowStatus
{
	DCX_SS_ENABLED = 1,	// Shadow is enabled, if not, the following one is always false
	DCX_SS_VISABLE = 1 << 1,	// Shadow window is visible
	DCX_SS_PARENTVISIBLE = 1<< 2	// Parent window is visible, if not, the above one is always false
};

// dummy runtime classe definition
class DcxControl;
class DcxList;

typedef std::vector<DcxControl *> VectorOfControlPtrs; //!< blah
typedef std::vector<DcxList *> VectorOfDragListPtrs; //!< Vector of draglists

/*!
 * \brief blah
 *
 * blah
 */

class DcxDialog : public DcxWindow {

public:

  DcxDialog( const HWND mHwnd, TString & tsName, TString & tsAliasName );
  virtual ~DcxDialog( );

  const TString &getName( ) const;
  const TString &getAliasName( ) const;

  static LRESULT WINAPI WindowProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

  void parseCommandRequest( TString & input );
  void parseInfoRequest( TString & input, char * szReturnValue );

  BOOL callAliasEx( char * szReturn, const char * szFormat, ... );

  DcxControl * getControlByID( const UINT ID );
  DcxControl * getControlByHWND( const HWND mHwnd );

  void addControl( DcxControl * p_Control );
  void deleteControl( DcxControl * p_Control );
  void deleteAllControls( );

  bool updateLayout( RECT & rc );

  HBRUSH getBackClrBrush( ) const;

  void setMouseControl( const UINT mUID );
  void setFocusControl( const UINT mUID );

  void setParentName(const TString &strParent);
  const TString &getParentName() const;

  LayoutManager * m_pLayoutManager; //!< Layout Manager Object

  inline HCURSOR getCursor( ) const { return this->m_hCursor; };
	inline HWND getToolTip(void) const { return this->m_ToolTipHWND; };
  inline void incRef( ) { ++this->m_iRefCount; };
  inline void decRef( ) { --this->m_iRefCount; };
  inline UINT getRefCount( ) const { return this->m_iRefCount; };
	inline DWORD getEventMask( ) const { return this->m_dEventMask; };
	HBITMAP getBgBitmap() const { return this->m_bitmapBg; };
	COLORREF getBgTransparentCol() const { return this->m_colTransparentBg; };
	static void DrawDialogBackground(HDC hdc, DcxDialog *p_this, LPRECT rwnd);
	bool AddShadow(void);
	void RemoveShadow(void);
	void UpdateShadow(void);
	bool isShadowed(void) const;
	bool SetShadowSize(int NewSize = 0);
	bool SetShadowSharpness(unsigned int NewSharpness = 5);
	bool SetShadowDarkness(unsigned int NewDarkness = 200);
	bool SetShadowPosition(int NewXOffset = 5, int NewYOffset = 5);
	bool SetShadowColor(COLORREF NewColor = 0);

	static DWORD getAnimateStyles( TString & flags );
	void showError(const char *prop, const char *cmd, const char *err);
	void showErrorEx(const char *prop, const char *cmd, const char *fmt, ...);
#ifdef DCX_USE_GDIPLUS
	void DrawCaret(Graphics & graph);
	void DrawCtrl( Graphics & graphics, HDC hDC, HWND hWnd);
	void DrawDialog( Graphics & graphics, HDC hDC);
#endif
	void CreateVistaStyle(void);
	bool CreateVistaStyleBitmap(const SIZE &szWin);
	void RemoveVistaStyle(void);
	void UpdateVistaStyle(LPRECT rcUpdate = NULL);
	void SetVistaStylePos(void);
	void SetVistaStyleSize(void);
	bool IsVistaStyle(void) const { return this->m_bVistaStyle; };
	HDC GetVistaHDC(void) const { return this->m_hVistaHDC; };
	HWND GetVistaHWND(void) const { return this->m_hFakeHwnd; };
	SIZE GetVistaOffsets(void) const { return this->m_sVistaOffsets; };
	void MapVistaRect(HWND hwnd, LPRECT rc);

	void RegisterDragList(DcxList* list);
	void UnregisterDragList(DcxList* list);

protected:

  TString m_tsName;       //!< Dialog Name
  TString m_tsAliasName;  //!< Callback Alias Name
	TString m_tsParentName; //!< Parent name (only if docked)

  WNDPROC m_hOldWindowProc; //!< Dialog Old Window Procedure

  VectorOfControlPtrs m_vpControls; //!< Vector of DCX Controls
  VectorOfInts m_vZLayers;
  int m_zLayerCurrent;

  VectorOfDragListPtrs m_vDragLists; //!< Registered draglists

  bool m_bInSizing; //!< In Moving Motion
  bool m_bInMoving; //!< In Sizing Motion

  HBRUSH m_hBackBrush;    //!< Background control color

  UINT m_MouseID; //!< Mouse Hover ID
  UINT m_FocusID; //!< Mouse Hover ID

	XPopupMenu * m_popup;

  HCURSOR m_hCursor;  //!< Cursor Handle
  BOOL m_bCursorFromFile; //!< Cursor comes from a file?
  HBITMAP m_bitmapBg;
  UINT m_uStyleBg;
  COLORREF m_colTransparentBg;
	HWND m_ToolTipHWND; //!< Dialogs general tooltip control for use with all controls that don't have their own tooltips.
  UINT m_iRefCount;
	bool m_bDoDrag;
	bool m_bDrag;
	int m_bDoGhostDrag;
	bool m_bGhosted;
	DWORD m_dEventMask;
	BOOL m_bTracking;
	struct {
		HWND hWin; //!< The shadow window.
		BYTE Status; //!< The shadow windows status.
		unsigned char nDarkness;	// Darkness, transparency of blurred area
		unsigned char nSharpness;	// Sharpness, width of blurred border of shadow window
		signed char nSize;	// Shadow window size, relative to parent window size

		// The X and Y offsets of shadow window,
		// relative to the parent window, at center of both windows (not top-left corner), signed
		signed char nxOffset;
		signed char nyOffset;

		// Restore last parent window size, used to determine the update strategy when parent window is resized
		LPARAM WndSize;

		// Set this to true if the shadow should not be update until next WM_PAINT is received
		bool bUpdate;

		COLORREF Color;	// Color of shadow
	} m_Shadow;
	/* **** */

  static void parseBorderStyles( const TString & flags, LONG * Styles, LONG * ExStyles );

  static UINT parseLayoutFlags( const TString & flags );
  static UINT parseBkgFlags( const TString & flags );
  static UINT parseFlashFlags( const TString & flags );
  static UINT parseCursorFlags( const TString & flags );
  static LPSTR parseCursorType( const TString & cursor );
  static UINT parseTooltipFlags( const TString &flags);

	// Fill in the shadow window alpha blend bitmap with shadow image pixels
	void MakeShadow(UINT32 *pShadBits, const HWND hParent, const RECT *rcParent);

	// Helper to calculate the alpha-premultiled value for a pixel
	static inline DWORD PreMultiply(const COLORREF cl, const unsigned char nAlpha)
	{
		// It's strange that the byte order of RGB in 32b BMP is reverse to in COLORREF
		return (GetRValue(cl) * (DWORD)nAlpha / 255) << 16 |
			(GetGValue(cl) * (DWORD)nAlpha / 255) << 8 |
			(GetBValue(cl) * (DWORD)nAlpha / 255);
	}
	void PreloadData(void);
//#ifdef DCX_USE_GDIPLUS
//	Image *m_pImage;
//	bool LoadGDIPlusImage(TString &filename);
//#endif
	HWND m_hFakeHwnd;
	int m_iAlphaLevel;
	COLORREF m_cKeyColour;
	bool m_bHaveKeyColour;
	bool m_bVistaStyle;
	RECT m_GlassOffsets;
	PVOID m_pVistaBits;
	HBITMAP m_hVistaBitmap;
	HDC m_hVistaHDC;
	SIZE m_sVistaOffsets;

	static LRESULT ProcessDragListMessage(DcxDialog* p_this, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed);
};

#endif // _DCXDIALOG_H_