#pragma once

#ifndef _DCXTRAYICON_H_
#define _DCXTRAYICON_H_

#include "defines.h"

#define DCXM_TRAYICON 12345

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class DcxTrayIcon
{
public:
	DcxTrayIcon(void);
	~DcxTrayIcon(void);

	HWND GetHwnd();
	bool idExists(int id);
	bool modifyIcon(int id, DWORD msg, HICON icon = NULL, TString *tooltip = NULL);

	static LRESULT CALLBACK TrayWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	VectorOfInts trayIconIDs;
	HWND m_hwnd;
	WNDPROC m_wndProc;

	HWND m_hwndTooltip; //!< Balloon tooltip control

	bool DeleteIconId(int id);
	void AddIconId(int id);
};

extern DcxTrayIcon *trayIcons;

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXTRAYICON_H_
