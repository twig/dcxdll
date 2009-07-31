/*!
 * \file dcxtoolbar.h
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

#ifndef _DCXTOOLBAR_H_
#define _DCXTOOLBAR_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

#define TB_IML_NORMAL	0x01	//!< Toolbar Normal ImageList
#define TB_IML_DISABLE	0x02	//!< Toolbar Disabled ImageList
#define TB_IML_HOT		0x04	//!< Toolbar HotTrack ImageList
#define TB_ICO_GREY		0x08	//!< Toolbar HotTrack ImageList

#define BTNS_WIDTH				0x001000	//!< Toolbar Button Width Style
#define BTNS_BOLD				0x002000	//!< Toolbar Button Caption Bold Style
#define BTNS_UNDERLINE			0x004000	//!< Toolbar Button Caption Underline Style
#define BTNS_COLOR				0x008000	//!< Toolbar Button Caption Color Style
#define BTNS_TBKGCOLOR			0x010000	//!< Toolbar Tooltip Background Color Style
#define BTNS_TTXTCOLOR			0x020000	//!< Toolbar Tooltip Text Color Style
#define BTNS_HIGHLIGHT_TXTCOLOR	0x040000	//!< Toolbar Buttons Highlight Text colour.
#define BTNS_MARK_BKGCOLOR		0x080000	//!< Toolbar Buttons background colour when marked.
#define BTNS_BTNCOLOR			0x100000	//!< Toolbar Buttons colour.
#define BTNS_HIGHLIGHT_BTNCOLOR	0x200000	//!< Toolbar Buttons colour when highlighted.
#define BTNS_HOTTRACK_BTNCOLOR	0x400000	//!< Toolbar Buttons colour when hot.

/*!
 * \brief blah
 *
 * blah
 */

typedef struct tagDCXTBBUTTON {

	TString tsTipText;				//!< Tooltip text
	COLORREF clrText;				//!< Button Caption Color
	COLORREF clrMark;				//!< background color that the button will use when marked.
	COLORREF clrTextHighlight;		//!< Button Caption Color when highlighted. (An item is highlighted if it has the TBSTATE_MARKED style and is contained in a toolbar that has the TBSTYLE_FLAT style.)
	COLORREF clrBtnFace;			//!< The Buttons normal colour.
	COLORREF clrBtnHighlight;		//!< The Buttons highlight colour. (An item is highlighted if it has the TBSTATE_MARKED style and is contained in a toolbar that has the TBSTYLE_FLAT style.)
	COLORREF clrHighlightHotTrack;	//!< The Buttons hottrack colour (TBSTYLE_FLAT)
	BOOL bBold;						//!< Is Button Caption Bold ?
	BOOL bUline;					//!< Is Button Caption Underlined
	TString bText;					//!< Buttons text, MUST be persistant.
	int iTextBkgMode;				//!< Drawing mode for text on a normal button. (XP+)
	int iTextHighlightBkgMode;		//!< Drawing mode for text on a highlighted button. (XP+) (An item is highlighted if it has the TBSTATE_MARKED style and is contained in a toolbar that has the TBSTYLE_FLAT style.)

} DCXTBBUTTON,*LPDCXTBBUTTON;

/*!
 * \brief blah
 *
 * blah
 */

class DcxToolBar : public DcxControl {

public:

	DcxToolBar( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles );
	virtual ~DcxToolBar( );

	LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

	void parseInfoRequest( TString & input, char * szReturnValue );
	void parseCommandRequest( TString & input );
	void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );
	void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, LONG * ExStylesTb, BOOL * bNoTheme );

	HIMAGELIST getImageList( const int iImageList );
	void setImageList( HIMAGELIST himl, const int iImageList );
	static HIMAGELIST createImageList( const UINT iSize );

	LRESULT autoSize( );
	LRESULT insertButton( const int nPos, const LPTBBUTTON lpbb );
	LRESULT hitTest( const LPPOINT lpt ) const;
	LRESULT getItemRect( const int iButton, LPRECT lprc ) const;
	LRESULT getButtonCount( ) const;
	LRESULT setButtonWidth( const int cxMin, const int cxMax );
	LRESULT deleteButton( const int iButton );
	LRESULT setButtonInfo( const int idButton, LPTBBUTTONINFO lpbi );
	LRESULT getButtonInfo( const int idButton, LPTBBUTTONINFO lpbi ) const;
	LRESULT getTooltips( ) const;
	LRESULT getButtonText( const int idButton, LPSTR lpszText ) const;
	LRESULT getButton( const int iButton, LPTBBUTTON lpb ) const;
	LRESULT setButtonState( const int idButton, const UINT fState );
	LRESULT getCommandToIndex( const int idButton ) const;
	LRESULT moveButton( const int iButtonFrom, const int iButtonTo );
	LRESULT markButton( const int iButton, const bool fHighlight );
	LRESULT setButtonSize( const int dxButton, const int dyButton );

	void autoStretchButtons( );

	void autoPosition( const int width, const int height );

	inline TString getType( ) { return TString( "toolbar" ); };

protected:

	BOOL m_bAutoStretch; //!< Auto Stretch Variable

	HFONT m_hOldItemFont; //!< Item fonts (only used during custom draw)
	HFONT m_hItemFont;

	bool m_bOverrideTheme; //!< Allow drawing of custom colours when control is themed.

	static UINT parseImageListFlags( const TString & flags );

	int getFreeButtonID( ) const;
	int getIndexToCommand( const int iIndex ) const;

	static BYTE parseButtonStateFlags( const TString & flags );
	static UINT parseButtonStyleFlags( const TString & flags );

	void resetContent( );

};

#endif // _DCXTOOLBAR_H_
