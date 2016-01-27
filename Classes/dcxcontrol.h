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

#define DCC_TEXTCOLOR		0x01	//!< Control Text Color
#define DCC_TEXTBKGCOLOR	0x02	//!< Control Text Background Color
#define DCC_BKGCOLOR		0x04	//!< Control Background Color
#define DCC_BORDERCOLOR		0x08	//!< Control Border Color
#define DCC_GRADSTARTCOLOR	0x10	//!< Colour At the start of the gradient
#define DCC_GRADENDCOLOR	0x20	//!< Colour At the end of the gradient

#define DCCS_FROMRESSOURCE	0x01	//!< Cursor from ressource
#define DCCS_FROMFILE		0x02	//!< Cursor from File

#include "../defines.h"
#include "dcxwindow.h"

#define CTLF_ALLOW_PBAR				(UINT64)0x000000001
#define CTLF_ALLOW_TRACKBAR			(UINT64)0x000000002
#define CTLF_ALLOW_COMBOEX			(UINT64)0x000000004
#define CTLF_ALLOW_COLORCOMBO		(UINT64)0x000000008
#define CTLF_ALLOW_STATUSBAR		(UINT64)0x000000010
#define CTLF_ALLOW_TOOLBAR			(UINT64)0x000000020
#define CTLF_ALLOW_TREEVIEW			(UINT64)0x000000040
#define CTLF_ALLOW_LISTVIEW			(UINT64)0x000000080
#define CTLF_ALLOW_REBAR			(UINT64)0x000000100
#define CTLF_ALLOW_BUTTON			(UINT64)0x000000200
#define CTLF_ALLOW_RICHEDIT			(UINT64)0x000000400
#define CTLF_ALLOW_EDIT				(UINT64)0x000000800
#define CTLF_ALLOW_UPDOWN			(UINT64)0x000001000
#define CTLF_ALLOW_IPADDRESS		(UINT64)0x000002000
#define CTLF_ALLOW_WEBCTRL			(UINT64)0x000004000
#define CTLF_ALLOW_CALANDER			(UINT64)0x000008000
#define CTLF_ALLOW_DIVIDER			(UINT64)0x000010000
#define CTLF_ALLOW_PANEL			(UINT64)0x000020000
#define CTLF_ALLOW_TAB				(UINT64)0x000040000
#define CTLF_ALLOW_LINE				(UINT64)0x000080000
#define CTLF_ALLOW_BOX				(UINT64)0x000100000
#define CTLF_ALLOW_RADIO			(UINT64)0x000200000
#define CTLF_ALLOW_CHECK			(UINT64)0x000400000
#define CTLF_ALLOW_TEXT				(UINT64)0x000800000
#define CTLF_ALLOW_SCROLL			(UINT64)0x001000000
#define CTLF_ALLOW_LIST				(UINT64)0x002000000
#define CTLF_ALLOW_LINK				(UINT64)0x004000000
#define CTLF_ALLOW_IMAGE			(UINT64)0x008000000
#define CTLF_ALLOW_PAGER			(UINT64)0x010000000
#define CTLF_ALLOW_DOCK				(UINT64)0x020000000 // allows @Window and Dialog docking
#define CTLF_ALLOW_DATETIME			(UINT64)0x040000000
#define CTLF_ALLOW_STACKER			(UINT64)0x080000000
#define CTLF_ALLOW_DIRECTSHOW		(UINT64)0x100000000
#define CTLF_ALLOW_ALL				(UINT64)0xFFFFFFFFFFFFFFFF
#define CTLF_ALLOW_ALLBUTDOCK		(UINT64)(CTLF_ALLOW_ALL & ~CTLF_ALLOW_DOCK)

enum class DcxAllowControls : UINT64 {
	ALLOW_PBAR = (UINT64)0x000000001,
	ALLOW_TRACKBAR = (UINT64)0x000000002,
	ALLOW_COMBOEX = (UINT64)0x000000004,
	ALLOW_COLORCOMBO = (UINT64)0x000000008,
	ALLOW_STATUSBAR = (UINT64)0x000000010,
	ALLOW_TOOLBAR = (UINT64)0x000000020,
	ALLOW_TREEVIEW = (UINT64)0x000000040,
	ALLOW_LISTVIEW = (UINT64)0x000000080,
	ALLOW_REBAR = (UINT64)0x000000100,
	ALLOW_BUTTON = (UINT64)0x000000200,
	ALLOW_RICHEDIT = (UINT64)0x000000400,
	ALLOW_EDIT = (UINT64)0x000000800,
	ALLOW_UPDOWN = (UINT64)0x000001000,
	ALLOW_IPADDRESS = (UINT64)0x000002000,
	ALLOW_WEBCTRL = (UINT64)0x000004000,
	ALLOW_CALANDER = (UINT64)0x000008000,
	ALLOW_DIVIDER = (UINT64)0x000010000,
	ALLOW_PANEL = (UINT64)0x000020000,
	ALLOW_TAB = (UINT64)0x000040000,
	ALLOW_LINE = (UINT64)0x000080000,
	ALLOW_BOX = (UINT64)0x000100000,
	ALLOW_RADIO = (UINT64)0x000200000,
	ALLOW_CHECK = (UINT64)0x000400000,
	ALLOW_TEXT = (UINT64)0x000800000,
	ALLOW_SCROLL = (UINT64)0x001000000,
	ALLOW_LIST = (UINT64)0x002000000,
	ALLOW_LINK = (UINT64)0x004000000,
	ALLOW_IMAGE = (UINT64)0x008000000,
	ALLOW_PAGER = (UINT64)0x010000000,
	ALLOW_DOCK = (UINT64)0x020000000, // allows @Window and Dialog docking
	ALLOW_DATETIME = (UINT64)0x040000000,
	ALLOW_STACKER = (UINT64)0x080000000,
	ALLOW_DIRECTSHOW = (UINT64)0x100000000,
	ALLOW_ALL = (UINT64)0xFFFFFFFFFFFFFFFF,
	ALLOW_ALLBUTDOCK = (UINT64)(ALLOW_ALL & ~ALLOW_DOCK)
};

// Control types...
enum class DcxControlTypes : UINT {
	UNKNOWN = 0, BOX = 1,
	CHECK, EDIT, IMAGE, LINE, LINK, LIST, RADIO, SCROLL, TEXT, BUTTON, CALENDAR,
	COLORCOMBO, COMBOEX, DATETIME, DIRECTSHOW, DIVIDER, IPADDRESS, LISTVIEW, DIALOG,
	WINDOW, PAGER, PANEL, PROGRESSBAR, REBAR, RICHEDIT, STACKER, STATUSBAR, TABB, TOOLBAR,
	TRACKBAR, TREEVIEW, UPDOWN, WEBCTRL
};

enum class DcxSearchTypes : UINT {
	SEARCH_W = 0x01,	//!< WildCard Search
	SEARCH_R,			//!< Regex Search
	SEARCH_E			//!< Exact Match
};

class DcxDialog;

typedef struct {
	HDC ai_hdc;
	HDC *ai_hdcBuffer;
	HDC ai_Oldhdc;
	HBITMAP ai_bitmap;
	HBITMAP ai_bkg;
	HBITMAP ai_oldBM;
	RECT ai_rcClient;
	RECT ai_rcWin;
	BLENDFUNCTION ai_bf;
	HPAINTBUFFER ai_Buffer;
} ALPHAINFO, *LPALPHAINFO;

/*!
 * \brief blah
 *
 * blah
 */
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class DcxControl : public DcxWindow {

public:
	DcxControl() = delete;	// no default constructor
	DcxControl(const DcxControl &other) = delete;	// no copy constructor
	DcxControl &operator =(const DcxControl &) = delete;	// No assignments!

	DcxControl( const UINT mID, DcxDialog *const p_Dialog );
	virtual ~DcxControl( );

	virtual void parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const = 0;
	virtual void parseCommandRequest( const TString & input ) = 0;
	virtual void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) = 0;

	void parseGeneralControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

	bool evalAliasEx(TCHAR *const szReturn, const int maxlen, const TCHAR *const szFormat, ... );

	bool execAliasEx(const TCHAR *const szFormat, ... );

	const UINT getUserID( ) const noexcept;

	virtual LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) = 0;
	virtual LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) = 0;

	LRESULT setFont( const HFONT hFont, const BOOL fRedraw );
	HFONT getFont( ) const noexcept;

	void setControlFont( const HFONT hFont, const BOOL fRedraw );

	LRESULT setRedraw( const BOOL fView );

	const HBRUSH &getBackClrBrush( ) const noexcept;
	const COLORREF &getBackColor( ) const noexcept;
	const COLORREF &getTextColor( ) const noexcept;
	const COLORREF &getStartGradientColor(void) const noexcept { return this->m_clrStartGradient; };
	const COLORREF &getEndGradientColor(void) const noexcept { return this->m_clrEndGradient; };
	const RECT getWindowPosition(void) const;

	virtual const TString getType( ) const = 0;
	virtual const DcxControlTypes getControlType() const noexcept = 0;

	virtual const TString getStyles(void) const;
	virtual const TString getBorderStyles(void) const;
	virtual void toXml(TiXmlElement *const xml) const;
	virtual TiXmlElement * toXml(void) const;

	inline void incRef( ) noexcept { ++this->m_iRefCount; };
	inline void decRef( ) noexcept { --this->m_iRefCount; };
	inline const UINT &getRefCount( ) const noexcept { return this->m_iRefCount; };

	//DcxControl *getParentCtrl() const { return this->m_pParentCtrl; };
	void updateParentCtrl(void); //!< updates controls host control pointers, MUST be called before these pointers are used.
	void DrawParentsBackground(const HDC hdc, const RECT *const rcBounds = nullptr, const HWND dHwnd = nullptr);
	LPALPHAINFO SetupAlphaBlend(HDC *hdc, const bool DoubleBuffer = false);
	void FinishAlphaBlend(LPALPHAINFO ai);
	void showError(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const err) const;
	void showErrorEx(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const fmt, ...) const;

	static LRESULT CALLBACK WindowProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static DcxControl * controlFactory(DcxDialog *const p_Dialog, const UINT mID, const TString & input, const UINT offset, const UINT64 mask = CTLF_ALLOW_ALL, HWND hParent = nullptr);
	static void DrawCtrlBackground(const HDC hdc, const DcxControl *const p_this, const LPRECT rwnd = nullptr, HTHEME hTheme = nullptr, const int iPartId = 0, const int iStateId = 0);
	static HBITMAP resizeBitmap(HBITMAP srcBM, const RECT *const rc);
	static DcxControlTypes TSTypeToControlType(const TString &t);

protected:

	DcxDialog * m_pParentDialog;	//!< Parent DcxDialog object

	WNDPROC m_DefaultWindowProc;	//!< Default window procedure

	HFONT m_hFont;					//!< Control Font

	TString m_tsMark;				//!< Mark Information (see /xdid -M)
	TString m_tsToolTip;			//!< This controls tooltip text (if any).

	COLORREF m_clrText;				//!< Font color
	COLORREF m_clrBackText;			//!< Font Back Color (not supported)
	COLORREF m_colTransparentBg;
	COLORREF m_clrBackground;		//!< Background Colour. (used to make m_hBackBrush)
	COLORREF m_clrStartGradient;
	COLORREF m_clrEndGradient;

	HBRUSH m_hBackBrush;			//!< Background control color
	HBRUSH m_hBorderBrush;			//!< Controls Border Colour.

	HBITMAP m_bitmapBg;				//!< Background bitmap

	UINT m_iRefCount;

	HCURSOR m_hCursor;				//!< Cursor Handle

	HWND m_ToolTipHWND;				//!< Tooltip window (if any)
	HWND m_pParentHWND;

	DWORD m_dEventMask;

	BYTE m_iAlphaLevel;				//!< The amount the control is alpha blended.

	bool m_bCursorFromFile;			//!< Cursor comes from a file?
	bool m_bAlphaBlend;				//!< Control is alpha blended.
	bool m_bGradientFill;
	bool m_bGradientVertical;
	bool m_bInPrint;
	bool m_bShadowText;				//!< Text is drawn with a shadow.
	bool m_bCtrlCodeText;			//!< mIRCTEXT('s ctrl codes are used to change the text')s appearance.
	bool m_bNoTheme;				//!< Control isn't themed.
	//int m_iThemePartId;

	/* ***** */

	void parseGlobalCommandRequest(const TString & input, const XSwitchFlags & flags );
	BOOL parseGlobalInfoRequest(const TString & input, TCHAR *const szReturnValue) const;

	void registreDefaultWindowProc( );
	void unregistreDefaultWindowProc( );

	LRESULT CommonMessage( const UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
	void DrawControl(HDC hDC, HWND hwnd);
	void ctrlDrawText(HDC hdc, const TString &txt, const LPRECT rc, const UINT style);
	void calcTextRect(HDC hdc, const TString &txt, LPRECT rc, const UINT style);

	static void parseBorderStyles(const TString & flags, LONG *const Styles, LONG *const ExStyles);
	static void InvalidateParentRect(HWND hwnd);
	static const UINT parseColorFlags(const TString & flags);
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXCONTROL_H_
