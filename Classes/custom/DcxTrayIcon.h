#pragma once

#ifndef _DCXTRAYICON_H_
#define _DCXTRAYICON_H_

#include "defines.h"

#define DCXTRAYCLASS TEXT("DcxTrayClass")

#define DCXM_TRAYICON 12345

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class DcxTrayIcon final
{
public:
	DcxTrayIcon(void) noexcept;
	~DcxTrayIcon(void) noexcept;

	DcxTrayIcon(const DcxTrayIcon&) = delete;
	DcxTrayIcon& operator=(const DcxTrayIcon&) = delete;
	DcxTrayIcon(DcxTrayIcon&&) = delete;
	DcxTrayIcon& operator=(DcxTrayIcon&&) = delete;

	explicit operator bool() const noexcept { return (m_hwnd); }

	const inline HWND &GetHwnd() const noexcept
	{
		return m_hwnd;
	}
	const bool idExists(const int id) const noexcept;
	const bool modifyIcon(const int id, const DWORD msg, gsl::owner<const HICON> icon = nullptr, const TString *const tooltip = nullptr);

private:
	VectorOfInts trayIconIDs;
	HWND m_hwnd{ nullptr };

	HWND m_hwndTooltip{ nullptr }; //!< Balloon tooltip control

	const bool DeleteIconId(const int id) noexcept;
	void AddIconId(const int id);
	static LRESULT CALLBACK TrayWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
};

//extern std::unique_ptr<DcxTrayIcon> trayIcons;
inline std::unique_ptr<DcxTrayIcon> trayIcons = nullptr;

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXTRAYICON_H_
