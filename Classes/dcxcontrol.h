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

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxControl : public DcxWindow {

public:

  DcxControl( UINT mID, DcxDialog * p_Dialog );
  virtual ~DcxControl( );

  virtual void parseInfoRequest( TString & input, char * szReturnValue ) = 0;
  virtual void parseCommandRequest( TString & input ) = 0;
  virtual void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) = 0;

  void parseGeneralControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  BOOL callAliasEx( char * szReturn, const char * szFormat, ... );

  UINT getUserID( );

  static LRESULT CALLBACK WindowProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

  virtual LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) = 0;
  virtual LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) = 0;

  LRESULT setFont( HFONT hFont, BOOL fRedraw );
  LRESULT getFont( );

  void setControlFont( HFONT hFont, BOOL fRedraw );

  LRESULT setRedraw( BOOL fView );

  HBRUSH getBackClrBrush( );
  COLORREF getBackColor( );
  COLORREF getTextColor( );

  static DcxControl * controlFactory( DcxDialog * p_Dialog, UINT mID, TString & input );

  virtual TString getType( ) = 0;

  inline void incRef( ) { ++this->m_iRefCount; };
  inline void decRef( ) { --this->m_iRefCount; };
  inline UINT getRefCount( ) { return this->m_iRefCount; };

protected:

  DcxDialog * m_pParentDialog; //!< Parent DcxDialog object

  WNDPROC m_DefaultWindowProc;  //!< Default window procedure

  HFONT m_hFont; //!< Control Font

  TString m_tsMark; //!< Mark Information (see /xdid -M)

  COLORREF m_clrText;     //!< Font color
  COLORREF m_clrBackText; //!< Font Back Color (not supported)
  HBRUSH m_hBackBrush;    //!< Background control color

  UINT m_iRefCount;

  HCURSOR m_hCursor;  //!< Cursor Handle
  BOOL m_bCursorFromFile; //!< Cursor comes from a file?

	HWND m_ToolTipHWND; //!< Tooltip window (if any)
	TString m_tsToolTip; //!< This controls tooltip text (if any).

  /* ***** */

  void parseGlobalCommandRequest( TString & input, XSwitchFlags & flags );
  BOOL parseGlobalInfoRequest( TString & input, char * szReturnValue );

  UINT parseColorFlags( TString & flags );
  UINT parseCursorFlags( TString & flags );
  LPSTR parseCursorType( TString & cursor );

  void registreDefaultWindowProc( );
  void unregistreDefaultWindowProc( );

  void parseBorderStyles( TString & flags, LONG * Styles, LONG * ExStyles );


};

#endif // _DCXCONTROL_H_