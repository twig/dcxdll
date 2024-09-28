#include "defines.h"
#include "dcxVersionInfo.h"

namespace
{
	bool IsWindows11OrGreater() noexcept
	{
		constexpr WORD wMajorVersion = HIBYTE(_WIN32_WINNT_WINTHRESHOLD), wMinorVersion = LOBYTE(_WIN32_WINNT_WINTHRESHOLD), wBuildMajor = 21996ui16;
		OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0,{ 0 }, 0, 0 };
		DWORDLONG        const dwlConditionMask = VerSetConditionMask(
			VerSetConditionMask(
				VerSetConditionMask(
					0, VER_MAJORVERSION, VER_GREATER_EQUAL),
				VER_MINORVERSION, VER_GREATER_EQUAL),
			VER_BUILDNUMBER, VER_GREATER_EQUAL);

		osvi.dwMajorVersion = wMajorVersion;
		osvi.dwMinorVersion = wMinorVersion;
		osvi.dwBuildNumber = wBuildMajor;

		return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, dwlConditionMask) != FALSE;
	}
}

dcxVersionInfo::dcxVersionInfo() noexcept
{
	m_bVista = IsWindowsVistaOrGreater();	// OS is Vista+
	m_bWin7 = IsWindows7OrGreater();		// OS is Windows7+
	m_bWin8 = IsWindows8OrGreater();		// OS is Windows8+
	m_bWin10 = IsWindows10OrGreater();		// OS is Windows10+
	m_bWin11 = IsWindows11OrGreater();		// OS is Windows11+
}
