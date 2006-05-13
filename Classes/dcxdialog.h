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

#define DBS_BKGCOLOR		0x01 //!< Control Background Color
#define DBS_BKGBITMAP	0x02 //!< Control Background Bitmap

#define DBS_BKGNORMAL	0
#define DBS_BKGCENTER	1
#define DBS_BKGSTRETCH	2
#define DBS_BKGTILE		3

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

  DcxDialog( HWND mHwnd, TString & tsName, TString & tsAliasName );
  virtual ~DcxDialog( );

  TString getName( );
  TString getAliasName( );

  static LRESULT WINAPI WindowProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

  void parseCommandRequest( TString & input );
  void parseInfoRequest( TString & input, char * szReturnValue );

  BOOL callAliasEx( char * szReturn, const char * szFormat, ... );

  DcxControl * getControlByID( UINT ID );
  DcxControl * getControlByHWND( HWND mHwnd );

  void addControl( DcxControl * p_Control );
  void deleteControl( DcxControl * p_Control );
  void deleteAllControls( );

  void updateLayout( RECT & rc );

  HBRUSH getBackClrBrush( );

  void setMouseControl( UINT mUID );
  void setFocusControl( UINT mUID );

  inline HCURSOR getCursor( ) { return this->m_hCursor; };

protected:

  TString m_tsName;       //!< Dialog Name
  TString m_tsAliasName;  //!< Callback Alias Name

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
  /* **** */

  void parseBorderStyles( TString & flags, LONG * Styles, LONG * ExStyles );
  DWORD getAnimateStyles( TString & flags );

  UINT parseLayoutFlags( TString & flags );
  UINT parseBkgFlags( TString & flags );
  UINT parseFlashFlags( TString & flags );
  UINT parseCursorFlags( TString & flags );
  LPSTR parseCursorType( TString & cursor );
};

#endif // _DCXDIALOG_H_