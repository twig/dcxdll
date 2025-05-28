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

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

#define DCX_STATUSBAR_MAX_PARTS 256

struct SB_PARTINFOX
{
	DcxControl	*m_xChild{ nullptr };
	TString		m_xText;
	int			m_xiIcon{ -1 };
};
using LPSB_PARTINFOX = SB_PARTINFOX *;

using VectorOfXParts = std::vector<LPSB_PARTINFOX>;

class DcxStatusBar final
	: public DcxControl
{
public:
	DcxStatusBar() = delete;
	DcxStatusBar(const DcxStatusBar &) = delete;
	GSL_SUPPRESS(c.128) DcxStatusBar &operator =(const DcxStatusBar &) = delete;
	DcxStatusBar(DcxStatusBar &&) = delete;
	GSL_SUPPRESS(c.128) DcxStatusBar &operator =(DcxStatusBar &&) = delete;

	DcxStatusBar(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxStatusBar( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest( const TString & input ) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	HIMAGELIST getImageList( ) const noexcept;
	void setImageList( const HIMAGELIST himl ) noexcept;
	//static HIMAGELIST createImageList( );

	static UINT parseItemFlags( const TString & flags ) noexcept;

	void cleanPartIcons( ) noexcept;

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml() const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	const TString getStyles(void) const final;

	bool setParts( const int nParts, const LPINT aWidths ) noexcept;
	int getParts( const int nParts, LPINT aWidths ) const noexcept;
	bool getBorders( LPINT aWidths ) const noexcept;
	void setPartsPositions(const TString &tsPositions);
	void setPartContents(int nPos, const TString &tsFlags, int nIcon, const TString &tsText, const TString &tsTooltip);

	struct sbBorders
	{
		int horizontal{}, vertical{}, seperator{};
	};
	_NODISCARD sbBorders getBorders() const noexcept;

	COLORREF setBkColor( const COLORREF clrBk ) noexcept;
	COLORREF getBkColor() const noexcept;
	LRESULT setText( const int iPart, const int Style, const LPCTCH lpstr ) noexcept;
	LRESULT setPartInfo( const int iPart, const int Style, gsl::owner<const LPSB_PARTINFOX> pPart ) noexcept;
	LPSB_PARTINFOX getPartInfo(const int iPart) const noexcept;
	LRESULT getText( const int iPart, PTCHAR lpstr ) const noexcept;
	TString getText(const int iPart) const;
	LRESULT getTextLength( const int iPart ) const noexcept;
	void setTipText( const int iPart, const LPCTCH lpstr ) noexcept;
	void getTipText( const int iPart, const int nSize, PTCHAR lpstr ) const noexcept;
	TString getTipText(const int iPart) const;
	bool getRect( const int iPart, gsl::not_null<LPRECT> lprc ) const noexcept;
	bool setIcon( const int iPart, const HICON hIcon ) noexcept;
	HICON getIcon( const int iPart ) const noexcept;
	UINT getPartFlags(const int iPart) const noexcept;
	TString getItemFlags(int iPos) const noexcept;
	void updateParts(void);

	int hitTest( const POINT & pt ) const noexcept;

	void deletePartInfo(const int iPart) noexcept;

	void loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc) override;

	inline const TString getType() const final { return TEXT("statusbar"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::STATUSBAR; }

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

protected:

	HIMAGELIST m_hImageList{ nullptr }; //!< Internal Image List
	VectorOfXParts	m_vParts;	//!< Parts info for ownerdraw parts.
	INT m_iDynamicParts[DCX_STATUSBAR_MAX_PARTS]{}; // records parts with a percentage size.
	INT m_iFixedParts[DCX_STATUSBAR_MAX_PARTS]{}; // records parts with a fixed size.
};

#endif // _DCXSTATUSBAR_H_
