#pragma once

#include <VersionHelpers.h>

class dcxVersionInfo
{
	bool m_bVista{ false };
	bool m_bWin7{ false };
	bool m_bWin8{ false };
	bool m_bWin10{ false };
	bool m_bWin10NewerBuild{ false };
	bool m_bWin11{ false };

public:
	dcxVersionInfo() noexcept;
	~dcxVersionInfo() = default;

	const bool& isVista(void) const noexcept { return m_bVista; };
	const bool& isWin7(void) const noexcept { return m_bWin7; };
	const bool& isWin8(void) const noexcept { return m_bWin8; };
	const bool& isWin10(void) const noexcept { return m_bWin10; };
	const bool& isWin10Build18362(void) const noexcept { return m_bWin10NewerBuild; };
	const bool& isWin11(void) const noexcept { return m_bWin11; };
};

