/*!
 * \file dcxcontrol.h
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

#ifndef _DCXCONTROL_H_
#define _DCXCONTROL_H_

#define DCC_TEXTCOLOR     0x01 //!< Control Text Color
#define DCC_TEXTBKGCOLOR  0x02 //!< Control Text Background Color
#define DCC_BKGCOLOR      0x04 //!< Control Background Color

#define DCCS_FROMRESSOURCE 0x01 //!< Cursor from ressource
#define DCCS_FROMFILE      0x02 //!< Cursor from File

#include "../defines.h"
#include "dcxwindow.h"

#define CTLF_ALLOW_PBAR				0x00000001
#define CTLF_ALLOW_TRACKBAR		0x00000002
#define CTLF_ALLOW_COMBOEX			0x00000004
#define CTLF_ALLOW_COLORCOMBO		0x00000008
#define CTLF_ALLOW_STATUSBAR		0x00000010
#define CTLF_ALLOW_TOOLBAR			0x00000020
#define CTLF_ALLOW_TREEVIEW		0x00000040
#define CTLF_ALLOW_LISTVIEW		0x00000080
#define CTLF_ALLOW_REBAR			0x00000100
#define CTLF_ALLOW_BUTTON			0x00000200
#define CTLF_ALLOW_RICHEDIT		0x00000400
#define CTLF_ALLOW_EDIT				0x00000800
#define CTLF_ALLOW_UPDOWN			0x00001000
#define CTLF_ALLOW_IPADDRESS		0x00002000
#define CTLF_ALLOW_WEBCTRL			0x00004000
#define CTLF_ALLOW_CALANDER		0x00008000
#define CTLF_ALLOW_DIVIDER			0x00010000
#define CTLF_ALLOW_PANEL			0x00020000
#define CTLF_ALLOW_TAB				0x00040000
#define CTLF_ALLOW_LINE				0x00080000
#define CTLF_ALLOW_BOX				0x00100000
#define CTLF_ALLOW_RADIO			0x00200000
#define CTLF_ALLOW_CHECK			0x00400000
#define CTLF_ALLOW_TEXT				0x00800000
#define CTLF_ALLOW_SCROLL			0x01000000
#define CTLF_ALLOW_LIST				0x02000000
#define CTLF_ALLOW_LINK				0x04000000
#define CTLF_ALLOW_IMAGE			0x08000000
#define CTLF_ALLOW_PAGER			0x10000000
#define CTLF_ALLOW_DOCK				0x20000000
#define CTLF_ALLOW_DATETIME		0x40000000

class DcxDialog;

typedef struct {
	HDC ai_hdc;
	HDC ai_Oldhdc;
	HBITMAP ai_bitmap;
	RECT ai_rcClient;
	RECT ai_rcWin;
} ALPHAINFO, *LPALPHAINFO;

/*!
 * \brief blah
 *
 * blah
 */

class DcxControl : public DcxWindow {

public:

  DcxControl( const UINT mID, DcxDialog * p_Dialog );
  virtual ~DcxControl( );

  virtual void parseInfoRequest( TString & input, char * szReturnValue ) = 0;
  virtual void parseCommandRequest( TString & input ) = 0;
  virtual void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) = 0;

  static void parseGeneralControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  BOOL callAliasEx( char * szReturn, const char * szFormat, ... );

  UINT getUserID( ) const;

  static LRESULT CALLBACK WindowProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

  virtual LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) = 0;
  virtual LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) = 0;

  LRESULT setFont( HFONT hFont, BOOL fRedraw );
  LRESULT getFont( ) const;

  void setControlFont( HFONT hFont, BOOL fRedraw );

  LRESULT setRedraw( BOOL fView );

  HBRUSH getBackClrBrush( ) const;
  COLORREF getBackColor( ) const;
  COLORREF getTextColor( ) const;

  static DcxControl * controlFactory( DcxDialog * p_Dialog, const UINT mID, const TString & input, int offset, DWORD mask = -1, HWND hParent = NULL);

  virtual TString getType( ) = 0;

  inline void incRef( ) { ++this->m_iRefCount; };
  inline void decRef( ) { --this->m_iRefCount; };
  inline UINT getRefCount( ) const { return this->m_iRefCount; };
	DcxControl *getParentCtrl() const { return this->m_pParentCtrl; };
	void updateParentCtrl(void); //!< updates controls host control pointers, MUST be called before these pointers are used.
	static void DrawCtrlBackground(HDC hdc, DcxControl *p_this, LPRECT rwnd);
	void DrawParentsBackground(HDC hdc);
	LPALPHAINFO SetupAlphaBlend(HDC *hdc);
	void FinishAlphaBlend(LPALPHAINFO ai);

protected:

  DcxDialog * m_pParentDialog; //!< Parent DcxDialog object

  WNDPROC m_DefaultWindowProc;  //!< Default window procedure

  HFONT m_hFont; //!< Control Font

  TString m_tsMark; //!< Mark Information (see /xdid -M)

  COLORREF m_clrText;     //!< Font color
  COLORREF m_clrBackText; //!< Font Back Color (not supported)
  HBRUSH m_hBackBrush;    //!< Background control color
	HBITMAP m_bitmapBg;			//!< Background bitmap
	COLORREF m_colTransparentBg;

  UINT m_iRefCount;

  HCURSOR m_hCursor;  //!< Cursor Handle
  BOOL m_bCursorFromFile; //!< Cursor comes from a file?

	HWND m_ToolTipHWND; //!< Tooltip window (if any)
	TString m_tsToolTip; //!< This controls tooltip text (if any).
	DWORD m_dEventMask;
	bool m_bAlphaBlend;	//!< Control is alpha blended.
	DcxControl *m_pParentCtrl;
	HWND m_pParentHWND;
	bool m_bShadowText; //!< Text is drawn with a shadow.
  /* ***** */

  void parseGlobalCommandRequest( const TString & input, XSwitchFlags & flags );
  BOOL parseGlobalInfoRequest( const TString & input, char * szReturnValue );

  static UINT parseColorFlags( TString & flags );
  static UINT parseCursorFlags( TString & flags );
  static LPSTR parseCursorType( TString & cursor );

  void registreDefaultWindowProc( );
  void unregistreDefaultWindowProc( );

  static void parseBorderStyles( TString & flags, LONG * Styles, LONG * ExStyles );
};

#endif // _DCXCONTROL_H_