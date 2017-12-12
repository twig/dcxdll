/*!
 * \file divider.h
 * \brief Custom Win32 API Divider Control
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

#ifndef _DIVIDER_H_
#define _DIVIDER_H_

#include <windows.h>
#include <windowsx.h>

// Divider Window Style
#define DVS_HORZ 0x0000 //!< Horizontal Divider (Default)
#define DVS_VERT 0x0001 //!< Vertical Divider 

// Divider Window Messages
#define DV_SETPANE    (WM_USER + 1) //!< Divder Set Pane Message
#define DV_GETPANE    (WM_USER + 2) //!< Divder Get Pane Message
#define DV_SETDIVPOS  (WM_USER + 3) //!< Divder Set Divider Position Message
#define DV_GETDIVPOS  (WM_USER + 4) //!< Divder Get Divider Position Message
#define DV_CHANGEPOS  (WM_USER + 5) //!< Divder Position Changing Message

// Divider Pane IDs
#define DVF_PANELEFT  0x01  //!< Left/Top Pane Identifier
#define DVF_PANERIGHT 0x02  //!< Right/Bottom Pane Identifier

// DVPANEINFO Mask Flags
#define DVPIM_CHILD   0x01  //!< DVPANEINFO hChild Value Is Valid
#define DVPIM_IDEAL   0x02  //!< DVPANEINFO cxIdealPane Value Is Valid
#define DVPIM_MIN     0x04  //!< DVPANEINFO cxMinPane Value Is Valid
#define DVPIM_STYLE   0x08  //!< DVPANEINFO fStyle Value Is Valid
#define DVPIM_LPARAM  0x10  //!< DVPANEINFO lParam Value Is Valid

// Divider Notifications WM_NOTIFY
#define DVN_DELETEDPANE  0x01 //!< Pane Deletion Notification

/*!
 * \brief Divider Notification Structure
 */

#pragma warning(push)
#pragma warning( disable : 26472 ) //  warning C26472 : Don't use a static_cast for arithmetic conversions. Use brace initialization, gsl::narrow_cast or gsl::narow (type.1: http://go.microsoft.com/fwlink/p/?LinkID=620417).
struct NMDIVIDER {

    NMHDR hdr;      //!< Notification NMHDR structure, must be first always
    UINT fMask;     //!< Valid Member Mask Flags
    UINT iPaneId;   //!< Pane ID -> DVF_PANELEFT or DVF_PANERIGHT
    UINT fStyle;    //!< Pane Style Flags
    LPARAM lParam;  //!< Extra Storage Value

	NMDIVIDER() noexcept
		: hdr{}
		, fMask(0)
		, iPaneId(0)
		, fStyle(0)
		, lParam(0)
	{}
	NMDIVIDER(const HWND hHwnd, const UINT uCode, const UINT uMask, const UINT uPaneId, const UINT uStyle, const LPARAM uExt) noexcept
		: hdr{hHwnd, static_cast<UINT>(GetWindowLong(hHwnd, GWL_ID)), uCode}
		, fMask(uMask)
		, iPaneId(uPaneId)
		, fStyle(uStyle)
		, lParam(uExt)
	{}
};
using LPNMDIVIDER = NMDIVIDER *;
#pragma warning(pop)

#define DVNM_LPARAM   0x01  //!< NMDIVIDER lParam Value is Valid
#define DVNM_STYLE    0x02  //!< NMDIVIDER fStyle Value is Valid
#define DVNM_PANEID   0x04  //!< NMDIVIDER iPaneId Value is Valid

#define DVNM_DRAG_START 1 // NMDIVIDER Resize start
#define DVNM_DRAG_DRAG  2 // NMDIVIDER Resize dragging
#define DVNM_DRAG_END   3 // NMDIVIDER Resize end

/*!
 * \brief Divider Pane Info Structure
 */

struct DVPANEINFO {

	UINT cbSize;    //!< DVPANEINFO Structure Size
	UINT fMask;     //!< Valid Member Mask Flags
	UINT fStyle;    //!< Divider Pane Style Flags
	UINT cxMin;     //!< Divider Pane Minimum Width/Height
	UINT cxIdeal;   //!< Divider Pane Ideal Width/Height
	HWND hChild;    //!< Pane Child Window Handle
	LPARAM lParam;  //!< Extra Storage For Each Pane

	DVPANEINFO() noexcept
		: cbSize(sizeof(DVPANEINFO))
		, fMask(0)
		, fStyle(0)
		, cxMin(0)
		, cxIdeal(0)
		, hChild(nullptr)
		, lParam(0)
	{}
};
using LPDVPANEINFO = DVPANEINFO *;

/*!
 * \brief Divider Control Data Structure
 */

struct DVCONTROLDATA {

	DVPANEINFO m_LeftTopPane;		//!< Divider Panes
	DVPANEINFO m_RightBottomPane;	//!< Divider Panes
	UINT m_iLineWidth;      //!< Divider Line Width
	UINT m_iBarPos;         //!< Position of the bar
	int m_iOldPos;          //!< Moving Old Position
	bool m_bDragging;       //!< Are We Dragging The Bar?

	DVCONTROLDATA() noexcept
		: m_LeftTopPane()
		, m_RightBottomPane()
		, m_iLineWidth(2)
		, m_iBarPos(100)
		, m_iOldPos(0)
		, m_bDragging(false)
	{}
};
using LPDVCONTROLDATA = DVCONTROLDATA *;

LRESULT CALLBACK DividerWndProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

void Divider_SizeWindowContents(HWND mHwnd, const int nWidth, const int nHeight);
LRESULT Divider_OnLButtonDown(HWND mHwnd, const UINT uMsg, WPARAM wParam, LPARAM lParam);
void DrawXorBar(HDC hdc, const int x1, const int y1, const int width, const int height);
LRESULT Divider_OnLButtonUp(HWND mHwnd, const UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT Divider_OnMouseMove(HWND mHwnd, const UINT uMsg, WPARAM wParam, LPARAM lParam);
void Divider_CalcBarPos( HWND mHwnd, POINT * pt, RECT * rect );
void Divider_GetChildControl(HWND mHwnd, const UINT pane, LPDVPANEINFO result);

#endif // _DIVIDER_H_
