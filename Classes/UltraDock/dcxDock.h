#pragma once

/*
	* SubClass mIRC windows for docking with new layout engine.
*/

#include "../../defines.h"

#define DOCK_TYPE_MDI		0x01
#define DOCK_TYPE_TREE	0x02

class DcxDock
{
public:
	DcxDock(HWND refHwnd, HWND dockHwnd, int dockType);
	virtual ~DcxDock(void);

	virtual bool DockWindow(HWND hwnd, TString &flag);
	virtual void UnDockWindow(HWND hwnd);
	void UpdateLayout(void) const { SendMessage(this->m_hParent,WM_SIZE,NULL,NULL); };
	bool FindDock(const HWND hwnd);
	bool isDocked(const HWND hwnd);
	LPDCXULTRADOCK GetDock(const HWND hwnd);
	virtual void AdjustRect(WINDOWPOS *wp);

protected:
	static LRESULT CALLBACK mIRCRefWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK mIRCDockWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	VectorOfDocks m_VectorDocks; //!< list of windows docked here.
	//static VectorOfDocks m_VectorAllDocks; //!< list of all docked windows.
	WNDPROC m_OldRefWndProc; //!< The Windows Old WndProc.
	WNDPROC m_OldDockWndProc; //!< The Windows Old WndProc.
	HWND m_RefHwnd; //!< The HWND that windows are docked around, usually the main child window. This window is subclassed.
	HWND m_hParent; //!< The HWND that docked windows are docked too. This window is subclassed.
	int m_iType;
};
