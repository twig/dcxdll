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
#define PC_WM_RESETCACHE (PC_WM_BASE + 7)
#define PC_WM_SETEXTENDEDSTYLE (PC_WM_BASE + 8)

#define PCS_CACHE_BITMAPS	(1UL << 1)		// cache the thumb sized images of the copied hwnd.

#define PCF_HWND			(1UL << 1)		// m_hSrc is valid
#define PCF_TITLE			(1UL << 2)		// m_Title is valid
#define PCF_DESC			(1UL << 3)		// m_Description is valid
#define PCF_MIN				(1UL << 4)		// m_szMin is valid
#define PCF_MAX				(1UL << 5)		// m_szMax is valid
#define PCF_BKGCOLOUR		(1UL << 6)		// m_clrBkg is valid
#define PCF_TITLECOLOUR		(1UL << 7)		// m_clrTitle is valid
#define PCF_DESCCOLOUR		(1UL << 8)		// m_clrDescription is valid
#define PCF_ROUNDED			(1UL << 9)		// m_bRoundedWindow is valid

struct pkData
{
	DWORD m_dwMask{};
	HWND m_hSrc;
	SIZE m_szMin{};
	SIZE m_szMax{};
	COLORREF m_clrBkg{ CLR_INVALID };
	COLORREF m_clrTitle{ CLR_INVALID };
	COLORREF m_clrDescription{ CLR_INVALID };
	TCHAR m_Title[64]{};
	TCHAR m_Description[64]{};
	bool m_bRoundedWindow{};
};

#define PEEK_TXT_BOTTOM_PAD 5

struct PEEK_DATA
{
	HTHEME m_hTheme{};			// theme data.
	//HTHUMBNAIL m_hThumb{};
	HWND m_hCopyHwnd{};

	RECT m_rcSrc{};
	SIZE m_szMin{ 100, 30 };
	SIZE m_szMax{ 200, 200 };

	bool m_bRounded{};

	UINT m_BaseID{};			// controls ID
	UINT m_uStyle{ PCS_CACHE_BITMAPS };

	COLORREF m_clrTitle{ CLR_INVALID };
	COLORREF m_clrDesc{ CLR_INVALID };
	COLORREF m_clrBkg{ CLR_INVALID };

	TCHAR m_Title[64]{};
	TCHAR m_Description[64]{};

	std::map<HWND, HBITMAP> m_mapCache;
};

using LPPEEK_DATA = PEEK_DATA*;

LRESULT CALLBACK PeekWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void Peek_OnThemeChange(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

LRESULT Peek_OnCreate(HWND mHwnd, WPARAM wParam, LPARAM lParam);

void Peek_OnPaint(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept;

LRESULT Peek_SetSource(HWND mHwnd, HWND hSrc) noexcept;

LRESULT Peek_RemSource(HWND mHwnd) noexcept;

LRESULT Peek_SetDescription(_In_ HWND mHwnd, _In_reads_z_(length) LPCTSTR str, _In_ int length) noexcept;

LRESULT Peek_SetTitle(_In_ HWND mHwnd, _In_reads_z_(length) LPCTSTR str, _In_ int length) noexcept;

LRESULT Peek_SetData(_In_ HWND mHwnd, _In_ pkData* pkd) noexcept;

LRESULT Peek_Show(_In_ HWND mHwnd, _In_ bool bShowHDC, _In_ int x, _In_ int y) noexcept;

LRESULT Peek_SetMin(_In_ HWND mHwnd, _In_ LPSIZE szMin) noexcept;

LRESULT Peek_SetMax(_In_ HWND mHwnd, _In_ LPSIZE szMax) noexcept;

LRESULT Peek_SetRounded(_In_ HWND mHwnd, _In_ bool bRounded) noexcept;

LRESULT Peek_ResetCache(_In_ HWND mHwnd) noexcept;

LRESULT Peek_SetExtendedStyle(_In_ HWND mHwnd, UINT uFlags) noexcept;