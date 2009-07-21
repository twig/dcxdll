#pragma once

/*
	* SubClass mIRC windows for docking with new layout engine.
*/

#include "../../defines.h"

#define DOCK_TYPE_SWITCH	0x00
#define DOCK_TYPE_TOOL		0x01
#define DOCK_TYPE_TREE		0x02
#define DOCK_TYPE_MDI		0x03

#define DOCKF_NORMAL					0x001	//!< No special flags.
#define DOCKF_AUTOH						0x002	//!< Auto Horizontal size.
#define DOCKF_AUTOV						0x004	//!< Auto Vertical size.
#define DOCKF_SIZE						0x008	//!< Auto Horizontal & Vertical size.
#define DOCKF_LEFT						0x010 //!< Dock to left. (UltraDock)
#define DOCKF_RIGHT						0x020	//!< Dock to right. (UltraDock)
#define DOCKF_TOP						0x040	//!< Dock to top. (UltraDock)
#define DOCKF_BOTTOM					0x080	//!< Dock to bottom. (UltraDock)
#define DOCKF_NOSCROLLBARS				0x100	//!< Disable parenst scrollbars.
#define DOCKF_SHOWSCROLLBARS			0x200 //!< Stop the auto-sized window from covering the scrollbars of its parent.

#define TREEBAR_COLOUR_SELECTED			0
#define TREEBAR_COLOUR_SELECTED_BKG		1
#define TREEBAR_COLOUR_MESSAGE			2
#define TREEBAR_COLOUR_MESSAGE_BKG		3
#define TREEBAR_COLOUR_EVENT			4
#define TREEBAR_COLOUR_EVENT_BKG		5
#define TREEBAR_COLOUR_HIGHLIGHT		6
#define TREEBAR_COLOUR_HIGHLIGHT_BKG	7
#define TREEBAR_COLOUR_HOT_TEXT			8
#define TREEBAR_COLOUR_HOT_BKG			9
#define TREEBAR_COLOUR_MAX				9

typedef struct tagDCXULTRADOCK {
	HWND hwnd;
	DWORD flags;
	DWORD old_styles;
	DWORD old_exstyles;
	RECT rc;
} DCXULTRADOCK, *LPDCXULTRADOCK;

typedef std::vector<LPDCXULTRADOCK> VectorOfDocks;

typedef struct tagSB_PARTINFO {
	HWND		m_Child;
	TString		m_Text;
	int			m_iIcon;
} SB_PARTINFO, *LPSB_PARTINFO;

typedef std::vector<LPSB_PARTINFO> VectorOfParts;

class DcxDock
{
public:
	DcxDock(HWND refHwnd, HWND dockHwnd, const int dockType);
	virtual ~DcxDock(void);

	virtual bool DockWindow(HWND hwnd, const TString &flag);
	virtual void UnDockWindow(HWND hwnd);
	void UnDockAll(void);
	void UpdateLayout(void) const { SendMessage(this->m_hParent,WM_SIZE,NULL,NULL); };
	bool FindDock(const HWND hwnd);
	bool isDocked(const HWND hwnd);
	LPDCXULTRADOCK GetDock(const HWND hwnd);
	virtual void AdjustRect(WINDOWPOS *wp);
	void RedrawRef(void);

	// Statusbar Functions.
	static bool InitStatusbar(const TString &styles);
	static void UnInitStatusbar(void);
	static bool IsStatusbar(void);
	static void status_parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );
	static void status_getRect(LPRECT rc);
	static void status_setBkColor(const COLORREF clr);
	static void status_setParts( const int nParts, const LPINT aWidths );
	static LRESULT status_getParts( const int nParts, LPINT aWidths );
	static void status_setText( const int iPart, const int Style, const LPWSTR lpstr );
	static LRESULT status_getText( const int iPart, LPWSTR lpstr );
	static UINT status_getTextLength( const int iPart );
	static UINT status_getPartFlags( const int iPart );
	static void status_setTipText( const int iPart, const LPWSTR lpstr );
	static void status_getTipText( const int iPart, const int nSize, LPWSTR lpstr );
	static void status_getRect( const int iPart, LPRECT lprc );
	static void status_setIcon( const int iPart, const HICON hIcon );
	static HICON status_getIcon( const int iPart );
	static HIMAGELIST status_getImageList(void);
	static void status_setImageList( HIMAGELIST himl );
	static HIMAGELIST status_createImageList(void);
	static UINT status_parseItemFlags( const TString & flags );
	static void status_cleanPartIcons( );
	static LRESULT status_getBorders( LPINT aWidths );
	static void status_updateParts(void);
	static void status_setFont(HFONT f);
	static LRESULT status_setPartInfo( const int iPart, const int Style, const LPSB_PARTINFO pPart);
	static void status_deletePartInfo(const int iPart);
	//
	static int getPos(const int x, const int y, const int w, const int h);
	//
	static void getTreebarItemType(TString &tsType, const LPARAM lParam);
	//static UINT getTreebarChildState(const HTREEITEM hParent, LPTVITEMEX pitem);

protected:
	static LRESULT CALLBACK mIRCRefWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK mIRCDockWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	VectorOfDocks m_VectorDocks; //!< list of windows docked here.
	//static VectorOfDocks g_VectorAllDocks; //!< list of all docked windows.
	WNDPROC m_OldRefWndProc; //!< The Windows Old WndProc.
	WNDPROC m_OldDockWndProc; //!< The Windows Old WndProc.
	HWND m_RefHwnd; //!< The HWND that windows are docked around, usually the main child window. This window is subclassed.
	HWND m_hParent; //!< The HWND that docked windows are docked too. This window is subclassed.
	int m_iType; //!< The dock type.
public:
	// statusbar stuff
	static HWND g_StatusBar; //!< The Statusbar for the main mIRC window.
	static HIMAGELIST g_hImageList; //!< The Statusbar's image list.
	static bool g_bUseUTF8;			//!< Use utf8 text
	static INT g_iDynamicParts[256];
	static INT g_iFixedParts[256];
	static HFONT g_StatusFont; //!< Statusbar font.
	static VectorOfParts g_vParts;	//!< Parts info for ownerdraw parts.
	// treebar stuff
	static bool g_bTakeOverTreebar; //!< take over the drawing of the treebar from mIRC.
	// 0 = selected, 1 = selected bkg, 2 = message, 3 = message bkg
	// 4 = event, 5 = event bkg, 6 = highlight, 7 = highlight bkg
	static COLORREF g_clrTreebarColours[TREEBAR_COLOUR_MAX +1];
};

