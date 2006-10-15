#include ".\dcxtrayicon.h"

// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/shell/reference/structures/notifyicondata.asp
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/shell/reference/functions/shell_notifyicon.asp

extern DcxTrayIcon *trayIcons;

// xTray [+flags] [id] [icon index] [icon file] $tab [tooltip]
// Create : xTray +c [id] [icon index] [icon file] $tab [tooltip]
// ---------> xTray +h [id] [hwnd] $tab [tooltip]
// Edit   : xTray +e [id] [icon index] [icon file] $tab [tooltip]
// Icon   : xTray +i [id] [icon index] [icon file]
// Tooltip: xTray +T [id] (text)
// Delete : xTray +d [id]
mIRC(TrayIcon) {
	if (trayIcons == NULL) {
		trayIcons = new DcxTrayIcon();

		if (trayIcons == NULL) {
			DCXError("/xTray", "There was a problem creating the trayicon manager");
			return 1;
		}
	}

	if (!trayIcons->GetHwnd()) {
		DCXError("/xTray", "Could not start trayicon manager");
		return 1;
	}

	TString d(data);
	d.trim();
	int numtok = d.numtok(" ");

	if (numtok < 2) {
		DCXError("/xTray", "Insufficient parameters");
		return 1;
	}

	TString flags = d.gettok(1, " ");
	int id = d.gettok(2, " ").to_int();

	// create and edit can use the same function
	if ((flags.find('c', 0) || flags.find('e', 0)) && numtok > 3) {
		// find icon id in vector
		bool exists = (trayIcons->idExists(id) ? true : false);
		DWORD msg = NIM_ADD;
		
		// if create and it already exists
		if (flags.find('c', 0) && (exists)) {
			TString msg;

			msg.sprintf("Cannot create trayicon: id %d already exists", id);
			DCXError("/xTrayIcon", msg.to_chr());
			return 1;
		}

		// if edit and it doesnt exist
		if (flags.find('e', 0)) {
			if (!exists) {
				TString msg;

				msg.sprintf("Cannot edit trayicon: id %d does not exists", id);
				DCXError("/xTrayIcon", msg.to_chr());
				return 1;
			}

			msg = NIM_MODIFY;
		}

		// set up info
		HICON icon;
		TString tooltip;

		// if theres a tooltip text
		if (d.numtok("\t") > 1) {
			tooltip = TString(d.gettok(2, -1, "\t"));
			tooltip.trim();
		}

		//NIF_INFO
		//Use a balloon ToolTip instead of a standard ToolTip. The szInfo, uTimeout, szInfoTitle, and dwInfoFlags members are valid.

		// load the icon
		int index = d.gettok(3, " ").to_int();
		TString filename = d.gettok(1, "\t").gettok(4, -1, " ");

		icon = dcxLoadIcon(index, filename);


		// add/edit the icon
		if (!trayIcons->modifyIcon(id, msg, icon, &tooltip)) {
			if (msg == NIM_ADD)
				DCXError("/xTrayIcon", "Add trayicon failed");
			else
				DCXError("/xTrayIcon", "Edit trayicon failed");
		}
	}
	// delete trayicon
	else if (flags.find('d', 0)) {
		if (!trayIcons->modifyIcon(id, NIM_DELETE)) {
			DCXError("/xTrayIcon", "Error deleting trayicon");
		}
	}
	// change icon
	else if (flags.find('i', 0) && (numtok > 3)) {
		// set up info
		HICON icon;
		int index = d.gettok(3, " ").to_int();
		TString filename = d.gettok(4, -1, " ");

		//NIF_INFO
		//Use a balloon ToolTip instead of a standard ToolTip. The szInfo, uTimeout, szInfoTitle, and dwInfoFlags members are valid.

		// load the icon
		filename.trim();
		icon = dcxLoadIcon(index, filename);

		if (!trayIcons->modifyIcon(id, NIM_MODIFY, icon, NULL)) {
			DCXError("/xTray", "Error changing trayicon icon");
		}
	}
	// change tooltip
	else if (flags.find('T', 0)) {
		TString tip = "";

		if (numtok > 2)
			tip = d.gettok(3, -1, " ");

		if (!trayIcons->modifyIcon(id, NIM_MODIFY, NULL, &tip)) {
			DCXError("/xTray", "Error changing trayicon tooltip");
		}
	}
	else {
		DCXError("/xTray", "Unknown flag or insufficient parameters");
	}

	return 1;
}


DcxTrayIcon::DcxTrayIcon(void)
{
	// create a "dialog" and dont bother showing it
	this->m_hwnd = CreateWindow("#32770", "", NULL, 0, 0, 48, 48, NULL, NULL, GetModuleHandle(NULL), NULL);

	if (this->m_hwnd)
		this->m_wndProc = (WNDPROC) SetWindowLong(this->m_hwnd, GWL_WNDPROC, (LONG) DcxTrayIcon::TrayWndProc);
	else
		DCXError("/xTrayIcon", "Problem initialising trayicons");

	m_hwndTooltip = NULL;

	//if (isXP())
	//	DCXError("/xTrayIcon","Try to create tooltip");

	//if (m_hwndTooltip != NULL)
	//	DCXError("/xTrayIcon","Tooltip available");
	//else
	//	DCXError("/xTrayIcon","Balloon tooltips will not be available for TrayIcon");

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
			this->modifyIcon(ids.gettok(i, " ").to_int(), NIM_DELETE);
		}

		SetWindowLong(this->m_hwnd, GWL_WNDPROC, (LONG) this->m_wndProc);
		this->m_hwndTooltip = NULL;
		this->m_wndProc = NULL;
		DestroyWindow(this->m_hwnd);
	}
}

HWND DcxTrayIcon::GetHwnd() {
	return this->m_hwnd;
}

LRESULT CALLBACK DcxTrayIcon::TrayWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == DCXM_TRAYICON) {
		UINT uMouseMsg = (UINT) lParam;
		UINT id = (UINT) wParam;

		switch (uMouseMsg)
		{
			case WM_LBUTTONDBLCLK:
				mIRCSignalDCX("trayicon %s %d", "dclick", id);
				break;

			case WM_LBUTTONUP:
				mIRCSignalDCX("trayicon %s %d", "sclick", id);
				break;

			case WM_RBUTTONUP:
			case WM_CONTEXTMENU:
				mIRCSignalDCX("trayicon %s %d", "rclick", id);
				break;

			case WM_RBUTTONDBLCLK:
				mIRCSignalDCX("trayicon %s %d", "rdclick", id);
				break;

			case WM_MBUTTONUP:
				mIRCSignalDCX("trayicon %s %d", "mclick", id);
				break;

			case WM_MBUTTONDBLCLK:
				mIRCSignalDCX("trayicon %s %d", "mdclick", id);
				break;

			default:
				break;
		}
	}

	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}


void DcxTrayIcon::AddIconId(int id) {
	this->trayIconIDs.push_back(id);
}

bool DcxTrayIcon::DeleteIconId(int id) {
	// remove from internal vector list
	VectorOfInts::iterator itStart = trayIconIDs.begin();
	VectorOfInts::iterator itEnd = trayIconIDs.end();

	while (itStart != itEnd) {
		if (*itStart == id) {
			trayIconIDs.erase(itStart);
			return true;
		}

		itStart++;
	}

	return false;
}

bool DcxTrayIcon::idExists(int id) {
	// find in internal vector list
	VectorOfInts::iterator itStart = trayIconIDs.begin();
	VectorOfInts::iterator itEnd = trayIconIDs.end();

	while (itStart != itEnd) {
		if (*itStart == id)
			return true;

		itStart++;
	}

	return false;
}

bool DcxTrayIcon::modifyIcon(int id, DWORD msg, HICON icon, TString *tooltip) {
	// set up the icon info struct
	bool res = false;
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));

	nid.uID = id;
	nid.uFlags = NIF_MESSAGE;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = this->GetHwnd();
	nid.uCallbackMessage = DCXM_TRAYICON;

	if (tooltip != NULL && tooltip->len() > 0) {
		nid.uFlags |= NIF_TIP;
		wsprintf(nid.szTip, "%s", tooltip->to_chr());
	}

	if (icon != NULL) {
		nid.uFlags |= NIF_ICON;
		nid.hIcon = icon;
	}

   // add/edit the icon
	if (Shell_NotifyIcon(msg, &nid)) {
		if (msg == NIM_ADD)
			this->AddIconId(id);
		else if (msg == NIM_DELETE)
			this->DeleteIconId(id);

		res = true;
	}

	if (icon)
		DestroyIcon(icon);

	return res;
}