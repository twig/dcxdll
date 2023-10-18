//
//

#pragma once

#include <windows.h>
#include <windowsx.h>

#ifndef LBCB_STARTTRACK
#define LBCB_STARTTRACK                         0x01ae
#endif
#ifndef LBCB_ENDTRACK
#define LBCB_ENDTRACK                           0x01af
#endif

#define MC_ID_EDIT		1000
#define MC_ID_BUTTON	1001
#define MC_ID_DROP		1002
#define MC_ID_DROPCHILD 1003

#define MCOMBO_EDIT_WIDTH 20
#define MCOMBO_TYPE (UINT)0xFFFFFFFa

#define MCOMBO_DROPCLASS TEXT("mComboDropClass")

// messages
#define MC_WM_BASE (WM_USER)
#define MC_WM_ADDCHILD (MC_WM_BASE + 1)
#define MC_WM_REMCHILD (MC_WM_BASE + 2)
#define MC_WM_GETDROPRECT (MC_WM_BASE + 3)
#define MC_WM_SETDROPRECT (MC_WM_BASE + 4)
#define MC_WM_UPDATEDROPRECT (MC_WM_BASE + 5)
#define MC_WM_FILLCOLOURS (MC_WM_BASE + 6)
#define MC_WM_SHOWDROP (MC_WM_BASE + 7)
#define MC_WM_GETDROPSTATE (MC_WM_BASE + 8)
#define MC_WM_GETITEMCOUNT (MC_WM_BASE + 9)
#define MC_WM_GETITEM (MC_WM_BASE + 10)
#define MC_WM_GETSEL (MC_WM_BASE + 11)
#define MC_WM_GETSTYLE (MC_WM_BASE + 12)
#define MC_WM_GETCOMBOINFO (MC_WM_BASE + 13)
#define MC_WM_ADDITEM (MC_WM_BASE + 14)
#define MC_WM_INSERTITEM (MC_WM_BASE + 15)
#define MC_WM_RESETCONTENT (MC_WM_BASE + 16)
#define MC_WM_SETSEL (MC_WM_BASE + 17)
#define MC_WM_DELETEITEM (MC_WM_BASE + 18)
#define MC_WM_SETITEM (MC_WM_BASE + 19)
#define MC_WM_GETCHILD (MC_WM_BASE + 20)

// notifications
#define MCON_SELCHANGE	(0x0000)
#define MCON_DBLCLK		(0x0001)
#define MCON_EDITCHANGE	(0x0002)

#define MCS_CUSTOM		(0x0000)
#define MCS_LISTBOX		(0x0001)
#define MCS_COLOUR		(0x0002)
//#define MCS_TREEVIEW	(0x0004)
//#define MCS_LISTVIEW	(0x0008)
//#define MCS_STYLEMASK	(MCS_LISTBOX|MCS_COLOUR|MCS_TREEVIEW|MCS_LISTVIEW)
#define MCS_STYLEMASK	(MCS_LISTBOX|MCS_COLOUR)

// item structure for colourcombo style
struct MCOMBO_ITEM {
	size_t		m_Size{ sizeof(MCOMBO_ITEM) };
	UINT		m_Type{ MCS_COLOUR };
	TString		m_tsItemText;
	COLORREF	m_clrItem{ CLR_INVALID };
	COLORREF	m_clrText{ CLR_INVALID };
};
using LPMCOMBO_ITEM = MCOMBO_ITEM*;

//// item structure for listbox style
//struct MCOMBO_LISTBOXITEM {
//	size_t		m_Size{ sizeof(MCOMBO_LISTBOXITEM) };
//	UINT		m_Type{ MCS_LISTBOX };
//	TString		m_tsItemText;
//};
//using LPMCOMBO_LISTBOXITEM = MCOMBO_LISTBOXITEM*;

//// item structure for listview style
//struct MCOMBO_LISTVIEWITEM {
//	size_t		m_Size{ sizeof(MCOMBO_LISTVIEWITEM) };
//	UINT		m_Type{ MCS_LISTVIEW };
//	LVITEM		m_Item{};
//};
//using LPMCOMBO_LISTVIEWITEM = MCOMBO_LISTVIEWITEM*;
//
//// item structure for treeview style
//struct MCOMBO_TREEVIEWITEM {
//	size_t		m_Size{ sizeof(MCOMBO_TREEVIEWITEM) };
//	UINT		m_Type{ MCS_TREEVIEW };
//	TVITEMEX	m_Item{};
//};
//using LPMCOMBO_TREEVIEWITEM = MCOMBO_TREEVIEWITEM*;

//#ifdef BUILD_MCOMBO_CLASS
struct MCOMBO_DATA
{
	HWND m_hBase{};				// base window that joins everything together.
	HWND m_hEdit{};				// edit control to contain the results.
	HWND m_hDropChild{};		// child of drop control
	HWND m_hDropCtrl{};			// control that is dropped down.

	HTHEME m_hTheme{};			// theme data.

	UINT m_BaseID{};			// combo controls ID
	UINT m_Styles{};

	RECT m_rcButton{};			// size of button.
	RECT m_rcDrop{};			// size of drop control.

	HBRUSH m_CurrentEditColour{};
	COLORREF m_CurrentEditBkgColour{ CLR_INVALID };
	COLORREF m_CurrentEditTextColour{ CLR_INVALID };

	BOOL m_Tracking{};

	bool m_bButtonPressed{};	// is button in the pressed state?
	bool m_bButtonHotTracked{};	// is the control being hottracked?
};

using LPMCOMBO_DATA = MCOMBO_DATA*;

LRESULT CALLBACK MultiComboWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void MultiCombo_SizeBaseWindowContents(HWND mHwnd, WORD cx, WORD cy) noexcept;
void MultiCombo_OnLButtonDown(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
void MultiCombo_OnLButtonUp(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
void MultiCombo_OnMouseMove(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
void MultiCombo_OnMouseLeave(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
void MultiCombo_SetFocus(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
void MultiCombo_OnThemeChange(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
RECT MultiCombo_GetEditRect(LPCRECT rcBase) noexcept;
RECT MultiCombo_GetButtonRect(LPCRECT rcBase) noexcept;
RECT MultiCombo_GetDropRect(HWND mHwnd, UINT mID) noexcept;
LRESULT MultiCombo_OnCreate(HWND mHwnd, WPARAM wParam, LPARAM lParam);

void MultiCombo_OnPaint(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

void MultiCombo_SetFont(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

void MultiCombo_ParentNotify(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

void MultiCombo_AdjustDropRectPos(HWND mHwnd, LPMCOMBO_DATA lpmcdata) noexcept;
void MultiCombo_AdjustDropRectPos(HWND mHwnd) noexcept;

void MultiCombo_OnMove(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

void MultiCombo_ShowWindow(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

void MultiCombo_Drop_ShowWindow(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

void MultiCombo_RemoveChild(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

HWND MultiCombo_GetChild(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

BOOL MultiCombo_DrawItem(HWND mHwnd, LPDRAWITEMSTRUCT lpdis);

BOOL MultiCombo_MeasureItem(HWND mHwnd, LPMEASUREITEMSTRUCT lpmis) noexcept;

BOOL MultiCombo_FillColours(HWND mHwnd, WPARAM wParam, LPARAM lParam);

int MultiCombo_GetItemCount(HWND mHwnd) noexcept;

int MultiCombo_GetSelected(HWND mHwnd) noexcept;

BOOL MultiCombo_GetItem(HWND mHwnd, WPARAM wParam, LPARAM lParam);

BOOL MultiCombo_AddItem(HWND mHwnd, WPARAM wParam, LPARAM lParam);

BOOL MultiCombo_InsertItem(HWND mHwnd, WPARAM wParam, LPARAM lParam);

LRESULT MultiCombo_Drop_Command(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

LRESULT MultiCombo_Command(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

void MultiCombo_ResetContent(HWND mHwnd) noexcept;

void MultiCombo_SetSel(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

void MultiCombo_DeleteItem(HWND mHwnd, WPARAM wParam) noexcept;

void MultiCombo_SetItem(HWND mHwnd, WPARAM wParam, LPARAM lParam);


//#endif
