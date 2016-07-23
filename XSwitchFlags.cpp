#include "defines.h"

const bool XSwitchFlags::m_bFalse{ false };

//// Constructor
//XSwitchFlags::XSwitchFlags(const TString &switches) noexcept
//	: m_bFlags{ false }
//	, m_bFlags_cap{ false }
//{
//	// no - sign, & no + sign, invalid syntax
//	if (switches[0] == TEXT('-'))
//		m_bFlags[26] = true;
//	else if (switches[0] == TEXT('+'))
//		m_bFlags[27] = true;
//	else
//		return;
//
//	const auto len = switches.len();
//
//	// Parse the switches
//	for (auto i = decltype(len){1}; i < len; i++)
//	{
//		const auto c = switches[i];
//
//		// Check if its in the right range
//		if (c >= TEXT('a') && c <= TEXT('z'))
//			m_bFlags[(int) (c - TEXT('a'))] = true;
//		else if (c >= TEXT('A') && (c <= TEXT('Z')))
//			m_bFlags_cap[(int) (c - TEXT('A'))] = true;
//	}
//}
//
//// Checks if flags are set.
//const bool &XSwitchFlags::isSet(const TCHAR c) const noexcept
//{
//	// Lower-case
//	if ((c >= TEXT('a')) && (c <= TEXT('z')))
//		return m_bFlags[(int) (c - TEXT('a'))];
//	// Upper-case
//	else if ((c >= TEXT('A')) && (c <= TEXT('Z')))
//		return m_bFlags_cap[(int) (c - TEXT('A'))];
//	else if (c == TEXT('-'))	// check if - flag identifier used.
//		return m_bFlags[26];
//	else if (c == TEXT('+'))	// check if + flag identifier used.
//		return m_bFlags[27];
//
//	return m_bFalse;
//}

// Constructor
XSwitchFlags::XSwitchFlags(const TString &switches) noexcept
	: m_dFlagMask(0ULL)
{
	// no - sign, & no + sign, invalid syntax
	if (switches[0] == TEXT('-'))
		m_dFlagMask |= (1ULL << 60);
	else if (switches[0] == TEXT('+'))
		m_dFlagMask |= (1ULL << 61);
	else
		return;

	const auto len = switches.len();

	// Parse the switches
	for (auto i = decltype(len){1}; i < len; i++)
	{
		const auto c = switches[i];

		// Check if its in the right range
		if ((c >= TEXT('a') && c <= TEXT('z')) || (c >= TEXT('A') && (c <= TEXT('Z'))))
		{
			const auto bitn = c - TEXT('A') + 1;
			m_dFlagMask |= (1ULL << bitn);
		}
	}
}

// Checks if flags are set.
const bool XSwitchFlags::isSet(const TCHAR c) const noexcept
{
	// Lower-case & Upper-case
	if ((c >= TEXT('a') && c <= TEXT('z')) || (c >= TEXT('A') && (c <= TEXT('Z'))))
	{
		const auto bitn = c - TEXT('A') + 1;
		return (m_dFlagMask & (1ULL << bitn)) != 0;
	}
	else if (c == TEXT('-'))	// check if - flag identifier used.
		return (m_dFlagMask & (1ULL << 60)) != 0;
	else if (c == TEXT('+'))	// check if + flag identifier used.
		return (m_dFlagMask & (1ULL << 61)) != 0;
	return m_bFalse;
}
