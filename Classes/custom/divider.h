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

typedef struct tagNMDIVIDER {

    NMHDR hdr;      //!< Notification NMHDR structure, must be first always
    UINT fMask;     //!< Valid Member Mask Flags
    UINT iPaneId;   //!< Pane ID -> DVF_PANELEFT or DVF_PANERIGHT
    UINT fStyle;    //!< Pane Style Flags
    LPARAM lParam;  //!< Extra Storage Value

} NMDIVIDER, *LPNMDIVIDER;

#define DVNM_LPARAM   0x01  //!< NMDIVIDER lParam Value is Valid
#define DVNM_STYLE    0x02  //!< NMDIVIDER fStyle Value is Valid
#define DVNM_PANEID   0x04  //!< NMDIVIDER iPaneId Value is Valid

/*!
 * \brief Divider Pane Info Structure
 */

typedef struct tagDVPANEINFO {

  UINT cbSize;    //!< DVPANEINFO Structure Size
  UINT fMask;     //!< Valid Member Mask Flags
  UINT fStyle;    //!< Divider Pane Style Flags
  UINT cxMin;     //!< Divider Pane Minimum Width/Height
  UINT cxIdeal;   //!< Divider Pane Ideal Width/Height
  HWND hChild;    //!< Pane Child Window Handle
  LPARAM lParam;  //!< Extra Storage For Each Pane

} DVPANEINFO, * LPDVPANEINFO;

/*!
 * \brief Divider Control Data Structure
 */

typedef struct tagDVCONTROLDATA {

  DVPANEINFO m_Panes[2];  //!< Divider Panes
  UINT m_iLineWidth;      //!< Divider Line Width
  BOOL m_bDragging;       //!< Are We Dragging The Bar?
  UINT m_iBarPos;         //!< Position of the bar
  int m_iOldPos;          //!< Moving Old Position

} DVCONTROLDATA, * LPDVCONTROLDATA;

LRESULT CALLBACK DividerWndProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

void Divider_SizeWindowContents( HWND mHwnd, int nWidth, int nHeight );
LRESULT Divider_OnLButtonDown( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
void DrawXorBar(HDC hdc, int x1, int y1, int width, int height );
LRESULT Divider_OnLButtonUp( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT Divider_OnMouseMove( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
void Divider_CalcBarPos( HWND mHwnd, POINT * pt, RECT * rect );
void Divider_GetChildControl( HWND mHwnd, UINT pane, LPDVPANEINFO result);

#endif // _DIVIDER_H_
