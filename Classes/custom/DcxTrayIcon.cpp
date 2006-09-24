#include ".\dcxtrayicon.h"

extern DcxTrayIcon *trayIcons;

// TrayIcon [+flags] [id] [icon index] [icon file] $tab [tooltip]
// +c create
// +e edit
// +i icon
// +t text
// +d delete
mIRC(TrayIcon) {
	if (trayIcons == NULL) {
		trayIcons = new DcxTrayIcon();

		if (trayIcons == NULL) {
			mIRCError("problem creating trayicon obj");
			return 1;
		}
	}

	if (!trayIcons->GetHwnd()) {
		mIRCError("couldnt init trayicon");
		return 1;
	}

	TString d(data);
	d.trim();
	int numtok = d.numtok(" ");

	if (numtok < 2) {
		mIRCError("insufficient parameters");
		return 1;
	}

	TString flags = d.gettok(1, " ");
	int id = d.gettok(2, " ").to_int();

	if (flags.find('c', 0) && numtok > 3) {
		// TODO: find icon id in vector

		HICON icon;
		NOTIFYICONDATA nid;
		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));

		nid.uID = id;
		nid.uFlags = NIF_ICON | NIF_MESSAGE;

		// if theres a tooltip
		if (d.numtok("\t") > 1) {
			TString tooltip = d.gettok(2, -1, "\t");
			tooltip.trim();

			nid.uFlags |= NIF_TIP;
			wsprintf(nid.szTip, "%s", tooltip.to_chr());
		}

		//NIF_INFO
		//Use a balloon ToolTip instead of a standard ToolTip. The szInfo, uTimeout, szInfoTitle, and dwInfoFlags members are valid.

		// load the icon
		int index = d.gettok(3, " ").to_int();
		TString filename = d.gettok(1, "\t").gettok(4, -1, " ");

		filename.trim();
		ExtractIconEx(filename.to_chr(), index, NULL, &icon, 1);


		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = trayIcons->GetHwnd();
		nid.uCallbackMessage = DCXM_TRAYICON;
		nid.hIcon = icon;

		if (Shell_NotifyIcon(NIM_ADD, &nid))
			trayIcons->AddIconId(id);
		else
			mIRCError("failed");
	}
	else if (flags.find('d', 0)) {
		trayIcons->DeleteIconById(id);
	}
	else {
		mIRCError("unknown flag or insufficient parameters");
	}


	return 1;
}


DcxTrayIcon::DcxTrayIcon(void)
{
	// create a "dialog" and dont bother showing it
	this->m_hwnd = CreateWindow("#32770", "", NULL, 0, 0, 48, 48, NULL, NULL, GetModuleHandle(NULL), NULL);

	if (this->m_hwnd) {
		this->m_wndProc = (WNDPROC) SetWindowLong(this->m_hwnd, GWL_WNDPROC, (LONG) DcxTrayIcon::TrayWndProc);
	}
	else {
		mIRCError("problem initialising trayicons");
	}

	trayIconIDs.clear();
}

DcxTrayIcon::~DcxTrayIcon(void)
{
	if (this->m_hwnd) {
		VectorOfInts::iterator itStart = this->trayIconIDs.begin();
		VectorOfInts::iterator itEnd = this->trayIconIDs.end();

		TString ids;

		while (itStart != itEnd) {
			if (ids.len() == 0)
				ids.sprintf("%d", *itStart);
			else
				ids.sprintf("%s %d", ids.to_chr(), *itStart);

			itStart++;
		}

		for (int i = 1; i <= ids.numtok(" "); i++) {
			this->DeleteIconById(ids.gettok(i, " ").to_int());
		}

		SetWindowLong(this->m_hwnd, GWL_WNDPROC, (LONG) this->m_wndProc);
		this->m_wndProc = NULL;
		DestroyWindow(this->m_hwnd);
	}
}

HWND DcxTrayIcon::GetHwnd() {
	return this->m_hwnd;
}

LRESULT CALLBACK DcxTrayIcon::TrayWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == DCXM_TRAYICON) {
		mIRCDebug("here hwnd %d, uMsg %d, W %d, L %d", mHwnd, uMsg, wParam, lParam);
	}

//void On_MYWM_NOTIFYICON(WPARAM wParam, LPARAM lParam) 
//{ 
//    UINT uID; 
//    UINT uMouseMsg; 
// 
//    uID = (UINT) wParam; 
//    uMouseMsg = (UINT) lParam; 
// 
//    if (uMouseMsg == WM_LBUTTONDOWN) 
//    { 
//        switch (uID) 
//        { 
//            case IDI_MYBATTERYICON: 
// 
//                // The user clicked the battery icon. Display the 
//                // battery status. 
//                ShowBatteryStatus(); 
//                break; 
// 
//            case IDI_MYPRINTERICON: 
// 
//                // The user clicked the printer icon. Display the 
//                // status of the print job. 
//                ShowJobStatus(); 
//                break; 
// 
//            default: 
//                break; 
//        } 
//     } 
//
//     return; 
// }

	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}


void DcxTrayIcon::AddIconId(int id) {
	this->trayIconIDs.push_back(id);
}

bool DcxTrayIcon::DeleteIconById(int id) {
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));

	nid.uID = id;
	nid.hWnd = trayIcons->GetHwnd();

	// icon with that id was found and deleted
	if (Shell_NotifyIcon(NIM_DELETE, &nid)) {
		// remove from internal vector list
		VectorOfInts::iterator itStart = trayIconIDs.begin();
		VectorOfInts::iterator itEnd = trayIconIDs.end();

		while (itStart != itEnd) {
			if (*itStart == id) {
				trayIconIDs.erase(itStart);
				break;
			}

			itStart++;
		}

		return true;
	}

	return false;
}