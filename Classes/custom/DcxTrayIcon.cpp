#include "defines.h"
#include "dcxtrayicon.h"
#include "Dcx.h"

//std::unique_ptr<DcxTrayIcon> trayIcons = nullptr; // tray icon manager

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
mIRC(TrayIcon)
{
	TString d(data);

	data[0] = 0;

	try {
		if (!trayIcons)
			trayIcons = std::make_unique<DcxTrayIcon>();

		if (!*trayIcons)
		{
			trayIcons.reset();
			throw Dcx::dcxException("Could not start trayicon manager");
		}

		const auto numtok = d.trim().numtok();

		if (numtok < 2)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xflags(d.getfirsttok(1));
		const auto id = d.getnexttok().to_int();	// tok 2

		// create and edit can use the same function
		// Create/Edit   : xTray +c [id] [+flags] [icon index] [icon file] ([TAB] Tooltip text)
		if (xflags[TEXT('c')] || xflags[TEXT('e')])
		{
			if (numtok < 4)
				throw DcxExceptions::dcxInvalidArguments();

			// find icon id in vector
			const bool bExists = trayIcons->idExists(id);
			DWORD msg = NIM_ADD;

			// if create and it already exists
			if (xflags[TEXT('c')] && (bExists))
				throw Dcx::dcxException(TEXT("Cannot create trayicon: id % already exists"), id);

			// if edit and it doesnt exist
			if (xflags[TEXT('e')])
			{
				if (!bExists)
					throw Dcx::dcxException(TEXT("Cannot edit trayicon: id % does not exist"), id);

				msg = NIM_MODIFY;
			}

			// set up info
			const auto tsTabOne(d.getfirsttok(1, TSTABCHAR).trim());
			const auto tooltip(d.getlasttoks().trim());

			//NIF_INFO
			//Use a balloon ToolTip instead of a standard ToolTip. The szInfo, uTimeout, szInfoTitle, and dwInfoFlags members are valid.

			// load the icon
			const auto iconFlags(tsTabOne.getfirsttok(3));	// tok 3
			const auto index = tsTabOne.getnexttok().to_int();	// tok 4
			auto filename(tsTabOne.getlasttoks());			// tok 5, -1

			// add/edit the icon
			if (auto icon = dcxLoadIcon(index, filename, false, iconFlags); !trayIcons->modifyIcon(id, msg, icon, &tooltip))
				throw Dcx::dcxException("Modify trayicon failed");
		}
		// delete trayicon
		// Delete   : xTray +d [id]
		else if (xflags[TEXT('d')])
		{
			if (!trayIcons->modifyIcon(id, NIM_DELETE))
				throw Dcx::dcxException("Error deleting trayicon");
		}
		// change icon
		// Icon   : xTray +i [id] [+flags] [icon index] [icon file]
		else if (xflags[TEXT('i')])
		{
			if (numtok < 5)
				throw DcxExceptions::dcxInvalidArguments();

			// set up info
			const auto iconFlags(d.getnexttok());	// tok 3
			const auto index = d.getnexttok().to_int();	// tok 4
			auto filename(d.getlasttoks().trim());	// tok 5, -1

			// TODO: twig
			//NIF_INFO
			//Use a balloon ToolTip instead of a standard ToolTip. The szInfo, uTimeout, szInfoTitle, and dwInfoFlags members are valid.

			// load the icon
			if (auto icon = dcxLoadIcon(index, filename, false, iconFlags); !trayIcons->modifyIcon(id, NIM_MODIFY, icon))
				throw Dcx::dcxException("Error changing trayicon icon");
		}
		// change tooltip
		// Tooltip   : xTray +T [id] (tooltip)
		else if (xflags[TEXT('T')])
		{
			TString tip;

			if (numtok > 2)
				tip = d.getlasttoks();	// tok 3, -1

			if (!trayIcons->modifyIcon(id, NIM_MODIFY, nullptr, &tip))
				throw Dcx::dcxException("Error changing trayicon tooltip");
		}
		else
			//throw Dcx::dcxException("Unknown flag or insufficient parameters");
			throw DcxExceptions::dcxInvalidFlag();

		return 1;
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("/xtray"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("/xtray"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	return 0;
}

DcxTrayIcon::DcxTrayIcon(void) noexcept
{
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);

	const auto hInst = GetModuleHandle(nullptr);

	if (!GetClassInfoEx(hInst, DCXTRAYCLASS, &wc))
	{
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = 0;
		wc.lpfnWndProc = DcxTrayIcon::TrayWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = nullptr;
		wc.hCursor = nullptr;
		wc.hbrBackground = GetStockBrush(COLOR_WINDOWFRAME);
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = DCXTRAYCLASS;
		wc.hIconSm = nullptr;
		RegisterClassEx(&wc);
	}

	this->m_hwnd = CreateWindowEx(0, DCXTRAYCLASS, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hInst, nullptr);
}

DcxTrayIcon::~DcxTrayIcon(void) noexcept
{
	if (m_hwnd)
	{
		//// make copy of vector
		//const auto TempIDs(trayIconIDs);
		//
		//// use temp vector to delete icons (this modifies the original vector)
		//for (const auto& x : TempIDs)
		//	modifyIcon(x, NIM_DELETE);

		NOTIFYICONDATA nid{ sizeof(NOTIFYICONDATA), m_hwnd, 0U, NIF_MESSAGE, DCXM_TRAYICON };
		for (const auto& x : trayIconIDs)
		{
			nid.uID = gsl::narrow_cast<UINT>(x);
			Shell_NotifyIcon(NIM_DELETE, std::addressof(nid));
		}
		trayIconIDs.clear();

		m_hwndTooltip = nullptr;

		DestroyWindow(m_hwnd);
		UnregisterClass(DCXTRAYCLASS, GetModuleHandle(nullptr));
	}
}

LRESULT CALLBACK DcxTrayIcon::TrayWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if ((uMsg == DCXM_TRAYICON) && (dcxSignal.xtray))
	{
		const auto uMouseMsg = gsl::narrow_cast<UINT>(lParam);

		try {
			switch (const auto id = gsl::narrow_cast<UINT>(wParam); uMouseMsg)
			{
			case WM_LBUTTONDBLCLK:
				mIRCLinker::signal(TEXT("trayicon dclick %"), id);
				break;

			case WM_LBUTTONUP:
				mIRCLinker::signal(TEXT("trayicon sclick %"), id);
				break;

			case WM_RBUTTONUP:
			case WM_CONTEXTMENU:
				mIRCLinker::signal(TEXT("trayicon rclick %"), id);
				break;

			case WM_RBUTTONDBLCLK:
				mIRCLinker::signal(TEXT("trayicon rdclick %"), id);
				break;

			case WM_MBUTTONUP:
				mIRCLinker::signal(TEXT("trayicon mclick %"), id);
				break;

			case WM_MBUTTONDBLCLK:
				mIRCLinker::signal(TEXT("trayicon mdclick %"), id);
				break;

			default:
				break;
			}
		}
		catch (...) {}
	}

	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}

void DcxTrayIcon::AddIconId(const int id)
{
	//trayIconIDs.push_back(id);
	trayIconIDs.emplace_back(id);
}

const bool DcxTrayIcon::DeleteIconId(const int id) noexcept
{
	// remove from internal vector list

	//const auto itEnd = trayIconIDs.end();
	//if (const auto itGot = std::find(trayIconIDs.begin(), itEnd, id); itGot != itEnd)
	//{
	//	trayIconIDs.erase(itGot);
	//	return true;
	//}
	//return false;

	return Dcx::eraseIfFound(trayIconIDs, id);
}

const bool DcxTrayIcon::idExists(const int id) const noexcept
{
	// find in internal vector list

	//const auto itEnd = trayIconIDs.end();
	//const auto itGot = std::find(trayIconIDs.begin(), itEnd, id);
	//return (itGot != itEnd);

	return Dcx::find(trayIconIDs, id);
}

const bool DcxTrayIcon::modifyIcon(const int id, const DWORD msg, gsl::owner<const HICON> icon, const TString* const tooltip)
{
	// set up the icon info struct
	auto res = false;

	//NOTIFYICONDATA nid{};
	//
	//nid.cbSize = sizeof(NOTIFYICONDATA);
	//nid.hWnd = GetHwnd();
	//nid.uID = gsl::narrow_cast<UINT>(id);
	//nid.uFlags = NIF_MESSAGE;
	//nid.uCallbackMessage = DCXM_TRAYICON;

	NOTIFYICONDATA nid{ sizeof(NOTIFYICONDATA), GetHwnd(), gsl::narrow_cast<UINT>(id), NIF_MESSAGE, DCXM_TRAYICON };

	if (tooltip && !tooltip->empty())
	{
		nid.uFlags |= NIF_TIP;
		GSL_SUPPRESS(bounds.3) dcx_strcpyn(&nid.szTip[0], tooltip->to_chr(), std::size(nid.szTip)); // 128 max
	}

	if (icon)
	{
		nid.uFlags |= NIF_ICON;
		nid.hIcon = icon;
	}

	// add/edit the icon
	if (Shell_NotifyIcon(msg, &nid))
	{
		if (msg == NIM_ADD)
			AddIconId(id);
		else if (msg == NIM_DELETE)
			DeleteIconId(id);

		res = true;
	}

	if (icon)
		DestroyIcon(icon);

	return res;
}
