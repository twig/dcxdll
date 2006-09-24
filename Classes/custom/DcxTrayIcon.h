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
	bool DeleteIconById(int id);
	void AddIconId(int id);

	static LRESULT CALLBACK TrayWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	VectorOfInts trayIconIDs;
	HWND m_hwnd;
	WNDPROC m_wndProc;
};

#endif // _DCXTRAYICON_H_