#pragma once

#ifndef _DCXTRAYICON_H_
#define _DCXTRAYICON_H_

#include "../../defines.h"

#define DCXM_TRAYICON 12345

class DcxTrayIcon
{
public:
	DcxTrayIcon(void);
	~DcxTrayIcon(void);

	HWND GetHwnd();
	bool idExists(int id);
	bool modifyIcon(int id, DWORD msg, HICON icon, TString *tooltip);

	static LRESULT CALLBACK TrayWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	VectorOfInts trayIconIDs;
	HWND m_hwnd;
	WNDPROC m_wndProc;

	HWND m_hwndTooltip; //!< Balloon tooltip control

	bool DeleteIconId(int id);
	void AddIconId(int id);
};

#endif // _DCXTRAYICON_H_