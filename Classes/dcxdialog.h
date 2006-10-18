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

// dummy runtime classe definition
class DcxControl;

typedef std::vector<DcxControl *> VectorOfControlPtrs; //!< blah

/*!
 * \brief blah
 *
 * blah
 */

class DcxDialog : public DcxWindow {

public:

  DcxDialog( const HWND mHwnd, TString & tsName, TString & tsAliasName );
  virtual ~DcxDialog( );

  TString getName( ) const;
  TString getAliasName( ) const;

  static LRESULT WINAPI WindowProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

  void parseCommandRequest( TString & input );
  void parseInfoRequest( TString & input, char * szReturnValue );

  BOOL callAliasEx( char * szReturn, const char * szFormat, ... );

  DcxControl * getControlByID( const UINT ID );
  DcxControl * getControlByHWND( const HWND mHwnd );

  void addControl( DcxControl * p_Control );
  void deleteControl( DcxControl * p_Control );
  void deleteAllControls( );

  void updateLayout( RECT & rc );

  HBRUSH getBackClrBrush( );

  void setMouseControl( UINT mUID );
  void setFocusControl( UINT mUID );

  void setParentName(TString &strParent);
  TString getParentName();

  inline HCURSOR getCursor( ) const { return this->m_hCursor; };
	inline HWND getToolTip(void) const { return this->m_ToolTipHWND; };
  inline void incRef( ) { ++this->m_iRefCount; };
  inline void decRef( ) { --this->m_iRefCount; };
  inline UINT getRefCount( ) const { return this->m_iRefCount; };
	inline DWORD getEventMask( ) const { return this->m_dEventMask; };

protected:

  TString m_tsName;       //!< Dialog Name
  TString m_tsAliasName;  //!< Callback Alias Name
	TString m_tsParentName; //!< Parent name (only if docked)

  WNDPROC m_hOldWindowProc; //!< Dialog Old Window Procedure

  VectorOfControlPtrs m_vpControls; //!< Vector of DCX Controls

  bool m_bInSizing; //!< In Moving Motion
  bool m_bInMoving; //!< In Sizing Motion

  HBRUSH m_hBackBrush;    //!< Background control color

  UINT m_MouseID; //!< Mouse Hover ID
  UINT m_FocusID; //!< Mouse Hover ID

  LayoutManager * m_pLayoutManager; //!< Layout Manager Object

  HCURSOR m_hCursor;  //!< Cursor Handle
  BOOL m_bCursorFromFile; //!< Cursor comes from a file?
  HBITMAP m_bitmapBg;
  UINT m_uStyleBg;
  COLORREF m_colTransparentBg;
	HWND m_ToolTipHWND; //!< Dialogs general tooltip control for use with all controls that don't have their own tooltips.
	HRGN m_Region;
  UINT m_iRefCount;
	bool m_bDoDrag;
	bool m_bDrag;
	bool m_bDoGhostDrag;
	bool m_bGhosted;
	DWORD m_dEventMask;
	BOOL m_bTracking;
  /* **** */

  static void parseBorderStyles( TString & flags, LONG * Styles, LONG * ExStyles );
  static DWORD getAnimateStyles( TString & flags );

  static UINT parseLayoutFlags( TString & flags );
  static UINT parseBkgFlags( TString & flags );
  static UINT parseFlashFlags( TString & flags );
  static UINT parseCursorFlags( TString & flags );
  static LPSTR parseCursorType( TString & cursor );
  static UINT parseTooltipFlags(TString &flags);
};

#endif // _DCXDIALOG_H_