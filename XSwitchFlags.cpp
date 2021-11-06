#include "defines.h"

// Constructor
XSwitchFlags::XSwitchFlags(const TString& switches) noexcept
	: m_dFlagMask(0ULL)
{
	// no - sign, & no + sign, invalid syntax
	if (switches[0] == TEXT('-'))
		m_dFlagMask |= (1ULL << 60);
	else if (switches[0] == TEXT('+'))
		m_dFlagMask |= (1ULL << 61);
	else
		return;	// just return with no flags set.

	const auto len = switches.len();

	// Parse the switches
	for (auto i = decltype(len){1}; i < len; ++i)
	{
		const auto c = switches[i];

		// Check if its in the right range
		if ((c >= TEXT('A')) && (c <= TEXT('z'))) // this gives us 57bits in use + 2 special cases
		{
			const auto bitn = c - TEXT('A');
			m_dFlagMask |= (1ULL << bitn);
		}
	}
}

// Checks if flags are set.
const bool XSwitchFlags::isSet(const TCHAR c) const noexcept
{
	// Lower-case & Upper-case
	if ((c >= TEXT('A')) && (c <= TEXT('z'))) // this gives us 57bits in use + 2 special cases
	{
		const auto bitn = c - TEXT('A');
		return (m_dFlagMask & (1ULL << bitn)) != 0;
	}
	else if (c == TEXT('-'))	// check if - flag identifier used.
		return (m_dFlagMask & (1ULL << 60)) != 0;
	else if (c == TEXT('+'))	// check if + flag identifier used.
		return (m_dFlagMask & (1ULL << 61)) != 0;
	return m_bFalse;
}
