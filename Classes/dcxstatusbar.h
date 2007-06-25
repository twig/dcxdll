/*!
 * \file dcxstatusbar.h
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

#ifndef _DCXSTATUSBAR_H_
#define _DCXSTATUSBAR_H_

#include "../defines.h"
#include "dcxcontrol.h"

class DcxDialog;

typedef struct tagSB_PARTINFO {
	DcxControl	*m_Child;
	TString			m_Text;
	int					m_iIcon;
} SB_PARTINFO, *LPSB_PARTINFO;

typedef std::vector<LPSB_PARTINFO> VectorOfParts;

/*!
 * \brief blah
 *
 * blah
 */

class DcxStatusBar : public DcxControl {

public:

  DcxStatusBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxStatusBar( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  HIMAGELIST getImageList( );
  void setImageList( HIMAGELIST himl );
  static HIMAGELIST createImageList( );

  static UINT parseItemFlags( TString & flags );
  void cleanPartIcons( );
  
  LRESULT setParts( const int nParts, const LPINT aWidths );
  LRESULT getParts( const int nParts, LPINT aWidths ) const;
	LRESULT getBorders( LPINT aWidths ) const;
  LRESULT setBkColor( const COLORREF clrBk );
  LRESULT setText( const int iPart, const int Style, const LPSTR lpstr );
  LRESULT setPartInfo( const int iPart, const int Style, const LPSB_PARTINFO pPart );
  LRESULT getText( const int iPart, LPSTR lpstr ) const;
  LRESULT getTextLength( const int iPart ) const;
  LRESULT setTipText( const int iPart, const LPSTR lpstr );
  LRESULT getTipText( const int iPart, const int nSize, LPSTR lpstr ) const;
  LRESULT getRect( const int iPart, LPRECT lprc ) const;
  LRESULT setIcon( const int iPart, const HICON hIcon );
  LRESULT getIcon( const int iPart ) const;

  int hitTest( const POINT & pt ) const;

	void deletePartInfo(const int iPart);

  inline TString getType( ) { return TString( "statusbar" ); };

protected:

  HIMAGELIST m_hImageList; //!< Internal Image List
  VectorOfParts	m_vParts;	//!< Parts info for ownerdraw parts.
	INT m_iDynamicParts[256]; // records parts with a percentage size.
	INT m_iFixedParts[256]; // records parts with a fixed size.
};

#endif // _DCXSTATUSBAR_H_
