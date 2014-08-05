#include "defines.h"
#include "dcxtrayicon.h"
#include "Dcx.h"



// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/shell/reference/structures/notifyicondata.asp
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/shell/reference/functions/shell_notifyicon.asp

//extern DcxTrayIcon *trayIcons;

/*! \brief xTray [+flags] [id] [icon index] [icon file] $tab [tooltip]
 *
 * Create : xTray +c [id] [+flags] [icon index] [icon file] $tab [tooltip]
 * -------> xTray +h [id] [hwnd] $tab [tooltip]
 * Edit   : xTray +e [id] [+flags] [icon index] [icon file] $tab [tooltip]
 * Icon   : xTray +i [id] [+flags] [icon index] [icon file]
 * Tooltip: xTray +T [id] (text)
 * Delete : xTray +d [id]
*/
mIRC(TrayIcon) {
	if (trayIcons == NULL) {
		try {
			trayIcons = new DcxTrayIcon();
		}
		catch (std::bad_alloc)
		{
			Dcx::error(TEXT("/xtray"), TEXT("There was a problem creating the trayicon manager"));
			return 0;
		}
	}

	if (trayIcons->GetHwnd() == NULL) {
		Dcx::error(TEXT("/xtray"), TEXT("Could not start trayicon manager"));
		return 0;
	}

	TString d(data);

	const UINT numtok = d.trim().numtok( );

	if (numtok < 2) {
		Dcx::error(TEXT("/xtray"), TEXT("Insufficient parameters"));
		return 0;
	}

	const XSwitchFlags xflags(d.getfirsttok( 1 ));
	const int id = d.getnexttok().to_int();	// tok 2

	// create and edit can use the same function
	if ((xflags[TEXT('c')] || xflags[TEXT('e')]) && numtok > 3) {
		// find icon id in vector
		//const bool bExists = (trayIcons->idExists(id) ? true : false);
		const bool bExists = trayIcons->idExists(id);
		DWORD msg = NIM_ADD;

		// if create and it already exists
		if (xflags[TEXT('c')] && (bExists)) {
			Dcx::errorex(TEXT("/xtray"), TEXT("Cannot create trayicon: id %d already exists"), id);
			return 0;
		}

		// if edit and it doesnt exist
		if (xflags[TEXT('e')]) {
			if (!bExists) {
				Dcx::errorex(TEXT("/xtray"), TEXT("Cannot edit trayicon: id %d does not exists"), id);
				return 0;
			}

			msg = NIM_MODIFY;
		}

		// set up info
		const TString tsTabOne(d.getfirsttok(1, TSTAB).trim());
		const TString tooltip(d.getlasttoks( ).trim());

		//NIF_INFO
		//Use a balloon ToolTip instead of a standard ToolTip. The szInfo, uTimeout, szInfoTitle, and dwInfoFlags members are valid.

		// load the icon
		const TString iconFlags(tsTabOne.getfirsttok( 3 ));	// tok 3
		const int index = tsTabOne.getnexttok().to_int();	// tok 4
		TString filename(tsTabOne.getlasttoks( ));			// tok 5, -1

		HICON icon = dcxLoadIcon(index, filename, false, iconFlags);

		// add/edit the icon
		if (!trayIcons->modifyIcon(id, msg, icon, &tooltip)) {
			if (msg == NIM_ADD)
				Dcx::error(TEXT("/xtray"), TEXT("Add trayicon failed"));
			else
				Dcx::error(TEXT("/xtray"), TEXT("Edit trayicon failed"));
		}
	}
	// delete trayicon
	else if (xflags[TEXT('d')]) {
		if (!trayIcons->modifyIcon(id, NIM_DELETE)) {
			Dcx::error(TEXT("/xtray"), TEXT("Error deleting trayicon"));
		}
	}
	// change icon
	// Icon   : xTray +i [id] [+flags] [icon index] [icon file]
	else if (xflags[TEXT('i')] && (numtok > 4)) {
		// set up info
		HICON icon;
		const TString iconFlags(d.getnexttok( ));	// tok 3
		const int index = d.getnexttok( ).to_int();	// tok 4
		TString filename(d.getlasttoks().trim());	// tok 5, -1

		// TODO: twig
		//NIF_INFO
		//Use a balloon ToolTip instead of a standard ToolTip. The szInfo, uTimeout, szInfoTitle, and dwInfoFlags members are valid.

		// load the icon
		icon = dcxLoadIcon(index, filename, false, iconFlags);

		if (!trayIcons->modifyIcon(id, NIM_MODIFY, icon, NULL)) {
			Dcx::error(TEXT("/xtray"), TEXT("Error changing trayicon icon"));
		}
	}
	// change tooltip
	else if (xflags[TEXT('T')]) {
		TString tip;

		if (numtok > 2)
			tip = d.getlasttoks();	// tok 3, -1

		if (!trayIcons->modifyIcon(id, NIM_MODIFY, NULL, &tip))
			Dcx::error(TEXT("/xtray"), TEXT("Error changing trayicon tooltip"));
	}
	else
		Dcx::error(TEXT("/xtray"), TEXT("Unknown flag or insufficient parameters"));

	return 1;
}


DcxTrayIcon::DcxTrayIcon(void)
: m_hwnd(NULL)
, m_hwndTooltip(NULL)
, m_wndProc(NULL)
{
	trayIconIDs.clear();

	// create a "dialog" and dont bother showing it
	this->m_hwnd = CreateWindow(TEXT("#32770"), TEXT(""), NULL, 0, 0, 48, 48, NULL, NULL, GetModuleHandle(NULL), NULL);

	if (this->m_hwnd)
		this->m_wndProc = (WNDPROC) SetWindowLongPtr(this->m_hwnd, GWLP_WNDPROC, (LONG_PTR) DcxTrayIcon::TrayWndProc);
	else
		Dcx::error(TEXT("/xtray"), TEXT("Problem initialising trayicons"));
}

DcxTrayIcon::~DcxTrayIcon(void)
{
	if (this->m_hwnd) {
//		TString ids;
//
//#if DCX_USE_C11
//		for (const auto &x: this->trayIconIDs)
//			ids.addtok(x);
//#else
//		VectorOfInts::iterator itStart = this->trayIconIDs.begin();
//		VectorOfInts::iterator itEnd = this->trayIconIDs.end();
//
//		while (itStart != itEnd) {
//			ids.addtok(*itStart);
//
//			++itStart;
//		}
//#endif
//
//		ids.getfirsttok( 0 );
//
//		for (UINT i = 1; i <= ids.numtok( ); i++) {
//			this->modifyIcon(ids.getnexttok( ).to_int(), NIM_DELETE);	// tok i
//		}

		// make copy of vector
		VectorOfInts TempIDs(this->trayIconIDs);

		// use temp vector to delete icons (this modifies the original vector)
#if DCX_USE_C11
		for (const auto &x : TempIDs)
			this->modifyIcon(x, NIM_DELETE);
#else
		VectorOfInts::iterator itStart = TempIDs.begin();
		VectorOfInts::iterator itEnd = TempIDs.end();
		
		while (itStart != itEnd) {
			this->modifyIcon(*itStart, NIM_DELETE);

			++itStart;
		}
#endif

		SetWindowLongPtr(this->m_hwnd, GWLP_WNDPROC, (LONG_PTR) this->m_wndProc);
		this->m_hwndTooltip = NULL;
		this->m_wndProc = NULL;
		DestroyWindow(this->m_hwnd);
	}
}

HWND DcxTrayIcon::GetHwnd() const {
	return this->m_hwnd;
}

LRESULT CALLBACK DcxTrayIcon::TrayWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == DCXM_TRAYICON) {
		const UINT uMouseMsg = (UINT) lParam;
		const UINT id = (UINT) wParam;

		switch (uMouseMsg)
		{
			case WM_LBUTTONDBLCLK:
				mIRCLinker::signalex(dcxSignal.xtray, TEXT("trayicon %s %d"), TEXT("dclick"), id);
				break;

			case WM_LBUTTONUP:
				mIRCLinker::signalex(dcxSignal.xtray, TEXT("trayicon %s %d"), TEXT("sclick"), id);
				break;

			case WM_RBUTTONUP:
			case WM_CONTEXTMENU:
				mIRCLinker::signalex(dcxSignal.xtray, TEXT("trayicon %s %d"), TEXT("rclick"), id);
				break;

			case WM_RBUTTONDBLCLK:
				mIRCLinker::signalex(dcxSignal.xtray, TEXT("trayicon %s %d"), TEXT("rdclick"), id);
				break;

			case WM_MBUTTONUP:
				mIRCLinker::signalex(dcxSignal.xtray, TEXT("trayicon %s %d"), TEXT("mclick"), id);
				break;

			case WM_MBUTTONDBLCLK:
				mIRCLinker::signalex(dcxSignal.xtray, TEXT("trayicon %s %d"), TEXT("mdclick"), id);
				break;

			default:
				break;
		}
	}

	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}


void DcxTrayIcon::AddIconId(const int id) {
	this->trayIconIDs.push_back(id);
}

bool DcxTrayIcon::DeleteIconId(const int id) {
	// remove from internal vector list
	VectorOfInts::iterator itStart = trayIconIDs.begin();
	VectorOfInts::iterator itEnd = trayIconIDs.end();

	while (itStart != itEnd) {
		if (*itStart == id) {
			trayIconIDs.erase(itStart);
			return true;
		}

		++itStart;
	}

	return false;
}

bool DcxTrayIcon::idExists(const int id) const {
	// find in internal vector list
#if DCX_USE_C11
	for (const auto &x: trayIconIDs) {
		if (x == id)
			return true;
	}
	return false;
#else
	VectorOfInts::iterator itStart = const_cast<VectorOfInts &>(trayIconIDs).begin();
	VectorOfInts::iterator itEnd = const_cast<VectorOfInts &>(trayIconIDs).end();

	while (itStart != itEnd) {
		if (*itStart == id)
			return true;

		++itStart;
	}

	return false;
#endif
}

bool DcxTrayIcon::modifyIcon(const int id, const DWORD msg, const HICON icon, const TString *const tooltip) {
	// set up the icon info struct
	bool res = false;
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));

	nid.uID = id;
	nid.uFlags = NIF_MESSAGE;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = this->GetHwnd();
	nid.uCallbackMessage = DCXM_TRAYICON;

	if (tooltip != NULL && !tooltip->empty()) {
		nid.uFlags |= NIF_TIP;
		dcx_strcpyn(nid.szTip, tooltip->to_chr(), 128); // 128 max
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

	if (icon != NULL)
		DestroyIcon(icon);

	return res;
}
