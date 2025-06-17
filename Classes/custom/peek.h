//
// small window to show what is drawn on another window.
//

#pragma once

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#define PEEK_CLASS TEXT("mPeekClass")

#define PC_WM_BASE (WM_USER)
#define PC_WM_SETSOURCE (PC_WM_BASE + 1)
#define PC_WM_REMSOURCE (PC_WM_BASE + 2)
#define PC_WM_SETDESC (PC_WM_BASE + 3)
#define PC_WM_SETDATA (PC_WM_BASE + 4)
#define PC_WM_SHOW (PC_WM_BASE + 5)
#define PC_WM_SETMINMAX (PC_WM_BASE + 6)

#define PCF_HWND	0x01
#define PCF_TITLE	0x02
#define PCF_DESC	0x04
#define PCF_MIN		0x08
#define PCF_MAX		0x10

struct pkData
{
	DWORD m_dwMask{};
	HWND m_hSrc;
	SIZE m_szMin{};
	SIZE m_szMax{};
	TCHAR m_Title[64]{};
	TCHAR m_Description[64]{};
};

struct PEEK_DATA
{
	HWND m_hBase{};				// base window that joins everything together.

	HTHEME m_hTheme{};			// theme data.
	//HTHUMBNAIL m_hThumb{};
	HWND m_hCopyHwnd{};

	RECT m_rcSrc{};
	SIZE m_szMin{ 100, 30 };
	SIZE m_szMax{ 200, 200 };

	//BOOL m_Tracking{};

	UINT m_BaseID{};			// controls ID
	//UINT m_Styles{};

	TCHAR m_Title[64]{};
	TCHAR m_Description[64]{};
};

using LPPEEK_DATA = PEEK_DATA*;

LRESULT CALLBACK PeekWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//void Peek_SizeWindow(HWND mHwnd, WORD cx, WORD cy) noexcept;
//void Peek_OnLButtonDown(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
//void Peek_OnLButtonUp(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
//void Peek_OnMouseMove(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
//void Peek_OnMouseLeave(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
//void Peek_SetFocus(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

void Peek_OnThemeChange(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
LRESULT Peek_OnCreate(HWND mHwnd, WPARAM wParam, LPARAM lParam);

void Peek_OnPaint(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

//void Peek_SetFont(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
//
//void Peek_ParentNotify(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
//
//void Peek_OnMove(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
//
//void Peek_ShowWindow(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;
//
//LRESULT Peek_Command(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

LRESULT Peek_SetSource(HWND mHwnd, HWND hSrc) noexcept;

LRESULT Peek_RemSource(HWND mHwnd) noexcept;

LRESULT Peek_SetDescription(_In_ HWND mHwnd, _In_reads_z_(length) LPCTSTR str, _In_ int length) noexcept;

LRESULT Peek_SetTitle(_In_ HWND mHwnd, _In_reads_z_(length) LPCTSTR str, _In_ int length) noexcept;

LRESULT Peek_SetData(_In_ HWND mHwnd, _In_ pkData* pkd) noexcept;

LRESULT Peek_Show(_In_ HWND mHwnd, _In_ bool bShowHDC, _In_ int x, _In_ int y) noexcept;

LRESULT Peek_SetMin(_In_ HWND mHwnd, _In_ LPSIZE szMin) noexcept;

LRESULT Peek_SetMax(_In_ HWND mHwnd, _In_ LPSIZE szMax) noexcept;
