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
 * � ScriptsDB.org - 2006
 */

#ifndef _DCXSTATUSBAR_H_
#define _DCXSTATUSBAR_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

#define DCX_STATUSBAR_MAX_PARTS 256

typedef struct tagSB_PARTINFOX {
	DcxControl	*m_xChild;
	TString		m_xText;
	int			m_xiIcon;

	tagSB_PARTINFOX()
		: m_xChild(nullptr)
		, m_xiIcon(-1)
	{}
} SB_PARTINFOX, *LPSB_PARTINFOX;

typedef std::vector<LPSB_PARTINFOX> VectorOfXParts;

/*!
 * \brief blah
 *
 * blah
 */

class DcxStatusBar : public DcxControl {

public:
	DcxStatusBar() = delete;
	DcxStatusBar(const DcxStatusBar &) = delete;
	DcxStatusBar &operator =(const DcxStatusBar &) = delete;	// No assignments!

	DcxStatusBar(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxStatusBar( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) override;

	void parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const override;
	void parseCommandRequest( const TString & input ) override;
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) override;

	HIMAGELIST getImageList( ) const noexcept;
	void setImageList( HIMAGELIST himl );
	static HIMAGELIST createImageList( );

	static UINT parseItemFlags( const TString & flags );
	void cleanPartIcons( );
	const TString getStyles(void) const override;

	LRESULT setParts( const int nParts, const LPINT aWidths );
	LRESULT getParts( const int nParts, LPINT aWidths ) const;
	LRESULT getBorders( LPINT aWidths ) const;
	LRESULT setBkColor( const COLORREF clrBk );
	LRESULT setText( const int iPart, const int Style, const PTCHAR lpstr );
	LRESULT setPartInfo( const int iPart, const int Style, const LPSB_PARTINFOX pPart );
	LRESULT getText( const int iPart, PTCHAR lpstr ) const;
	LRESULT getTextLength( const int iPart ) const;
	LRESULT setTipText( const int iPart, const PTCHAR lpstr );
	LRESULT getTipText( const int iPart, const int nSize, PTCHAR lpstr ) const;
	LRESULT getRect( const int iPart, LPRECT lprc ) const;
	LRESULT setIcon( const int iPart, const HICON hIcon );
	LRESULT getIcon( const int iPart ) const;
	UINT getPartFlags(const int iPart) const;
	void updateParts(void);

	int hitTest( const POINT & pt ) const;

	void deletePartInfo(const int iPart);

	inline const TString getType() const override { return TEXT("statusbar"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::STATUSBAR; }

protected:

	HIMAGELIST m_hImageList; //!< Internal Image List
	VectorOfXParts	m_vParts;	//!< Parts info for ownerdraw parts.
	INT m_iDynamicParts[DCX_STATUSBAR_MAX_PARTS]; // records parts with a percentage size.
	INT m_iFixedParts[DCX_STATUSBAR_MAX_PARTS]; // records parts with a fixed size.
};

#endif // _DCXSTATUSBAR_H_
