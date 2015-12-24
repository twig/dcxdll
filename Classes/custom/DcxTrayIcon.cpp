#include "defines.h"
#include "dcxtrayicon.h"
#include "Dcx.h"



// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/shell/reference/structures/notifyicondata.asp
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/shell/reference/functions/shell_notifyicon.asp

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
	TString d(data);

	data[0] = 0;

	try {
		if (trayIcons == nullptr)
			trayIcons = new DcxTrayIcon();

		if (trayIcons->GetHwnd() == nullptr)
			throw std::runtime_error("Could not start trayicon manager");

		const auto numtok = d.trim().numtok();

		if (numtok < 2)
			throw std::invalid_argument("Insufficient parameters");

		const XSwitchFlags xflags(d.getfirsttok(1));
		const auto id = d.getnexttok().to_int();	// tok 2

		// create and edit can use the same function
		if ((xflags[TEXT('c')] || xflags[TEXT('e')]) && numtok > 3) {
			// find icon id in vector
			//const bool bExists = (trayIcons->idExists(id) ? true : false);
			const bool bExists = trayIcons->idExists(id);
			DWORD msg = NIM_ADD;

			// if create and it already exists
			if (xflags[TEXT('c')] && (bExists))
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Cannot create trayicon: id %d already exists"), id));

			// if edit and it doesnt exist
			if (xflags[TEXT('e')]) {
				if (!bExists)
					throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Cannot edit trayicon: id %d does not exists"), id));

				msg = NIM_MODIFY;
			}

			// set up info
			const auto tsTabOne(d.getfirsttok(1, TSTAB).trim());
			const auto tooltip(d.getlasttoks().trim());

			//NIF_INFO
			//Use a balloon ToolTip instead of a standard ToolTip. The szInfo, uTimeout, szInfoTitle, and dwInfoFlags members are valid.

			// load the icon
			const auto iconFlags(tsTabOne.getfirsttok(3));	// tok 3
			const auto index = tsTabOne.getnexttok().to_int();	// tok 4
			auto filename(tsTabOne.getlasttoks());			// tok 5, -1

			auto icon = dcxLoadIcon(index, filename, false, iconFlags);

			// add/edit the icon
			if (!trayIcons->modifyIcon(id, msg, icon, &tooltip))
				throw std::runtime_error("Modify trayicon failed");
		}
		// delete trayicon
		else if (xflags[TEXT('d')]) {
			if (!trayIcons->modifyIcon(id, NIM_DELETE))
				throw std::runtime_error("Error deleting trayicon");
		}
		// change icon
		// Icon   : xTray +i [id] [+flags] [icon index] [icon file]
		else if (xflags[TEXT('i')] && (numtok > 4)) {
			// set up info
			const auto iconFlags(d.getnexttok());	// tok 3
			const auto index = d.getnexttok().to_int();	// tok 4
			auto filename(d.getlasttoks().trim());	// tok 5, -1

			// TODO: twig
			//NIF_INFO
			//Use a balloon ToolTip instead of a standard ToolTip. The szInfo, uTimeout, szInfoTitle, and dwInfoFlags members are valid.

			// load the icon
			auto icon = dcxLoadIcon(index, filename, false, iconFlags);

			if (!trayIcons->modifyIcon(id, NIM_MODIFY, icon, nullptr))
				throw std::runtime_error("Error changing trayicon icon");
		}
		// change tooltip
		else if (xflags[TEXT('T')]) {
			TString tip;

			if (numtok > 2)
				tip = d.getlasttoks();	// tok 3, -1

			if (!trayIcons->modifyIcon(id, NIM_MODIFY, nullptr, &tip))
				throw std::runtime_error("Error changing trayicon tooltip");
		}
		else
			throw std::invalid_argument("Unknown flag or insufficient parameters");

		return 1;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/xtray"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/xtray"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
}


DcxTrayIcon::DcxTrayIcon(void)
: m_hwnd(nullptr)
, m_hwndTooltip(nullptr)
, m_wndProc(nullptr)
{
	trayIconIDs.clear();

	// create a "dialog" and dont bother showing it
	this->m_hwnd = CreateWindow(TEXT("#32770"), TEXT(""), 0, 0, 0, 48, 48, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

	if (!IsWindow(this->m_hwnd))
		throw std::runtime_error("Problem initialising trayicons");

	this->m_wndProc = SubclassWindow(this->m_hwnd, DcxTrayIcon::TrayWndProc);
}

DcxTrayIcon::~DcxTrayIcon(void)
{
	if (this->m_hwnd != nullptr)
	{
		// make copy of vector
		auto TempIDs(this->trayIconIDs);

		// use temp vector to delete icons (this modifies the original vector)
		for (const auto &x : TempIDs)
			this->modifyIcon(x, NIM_DELETE);

		SetWindowLongPtr(this->m_hwnd, GWLP_WNDPROC, (LONG_PTR) this->m_wndProc);
		this->m_hwndTooltip = nullptr;
		this->m_wndProc = nullptr;
		DestroyWindow(this->m_hwnd);
	}
}

HWND DcxTrayIcon::GetHwnd() const {
	return this->m_hwnd;
}

LRESULT CALLBACK DcxTrayIcon::TrayWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == DCXM_TRAYICON) {
		const auto uMouseMsg = (UINT) lParam;
		const auto id = (UINT) wParam;

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

const bool DcxTrayIcon::DeleteIconId(const int id) {
	// remove from internal vector list

	//auto itStart = trayIconIDs.begin();
	//auto itEnd = trayIconIDs.end();
	//
	//while (itStart != itEnd) {
	//	if (*itStart == id) {
	//		trayIconIDs.erase(itStart);
	//		return true;
	//	}
	//
	//	++itStart;
	//}
	//
	//return false;

	const auto itEnd = trayIconIDs.end();
	const auto itGot = std::find(trayIconIDs.begin(), itEnd, id);
	if (itGot != itEnd) {
		trayIconIDs.erase(itGot);
		return true;
	}
	return false;
}

const bool DcxTrayIcon::idExists(const int id) const {
	// find in internal vector list

	//for (const auto &x: trayIconIDs) {
	//	if (x == id)
	//		return true;
	//}
	//return false;

	//const auto itEnd = trayIconIDs.end();
	//const auto itGot = std::find(trayIconIDs.begin(), itEnd, id);
	//return (itGot != itEnd);

	return Dcx::find(trayIconIDs, id);
}

const bool DcxTrayIcon::modifyIcon(const int id, const DWORD msg, const HICON icon, const TString *const tooltip) {
	// set up the icon info struct
	auto res = false;
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));

	nid.uID = id;
	nid.uFlags = NIF_MESSAGE;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = this->GetHwnd();
	nid.uCallbackMessage = DCXM_TRAYICON;

	if (tooltip != nullptr && !tooltip->empty()) {
		nid.uFlags |= NIF_TIP;
		dcx_strcpyn(nid.szTip, tooltip->to_chr(), 128); // 128 max
	}

	if (icon != nullptr) {
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

	if (icon != nullptr)
		DestroyIcon(icon);

	return res;
}
