#include "defines.h"

// Constructor
XSwitchFlags::XSwitchFlags(const TString &switches)
{
	ZeroMemory(flags, sizeof(bool) * 28);
	ZeroMemory(flags_cap, sizeof(bool) * 26);

	// no - sign, & no + sign, invalid syntax
	if (switches[0] == TEXT('-'))
		flags[26] = true;
	else if (switches[0] == TEXT('+'))
		flags[27] = true;
	else
		return;

	const auto len = switches.len();

	// Parse the switches
	for (auto i = decltype(len){1}; i < len; i++)
	{
		const auto c = switches[i];

		// Check if its in the right range
		if (c >= TEXT('a') && c <= TEXT('z'))
			flags[(int) (c - TEXT('a'))] = true;
		else if (c >= TEXT('A') && (c <= TEXT('Z')))
			flags_cap[(int) (c - TEXT('A'))] = true;
	}
}

// Destructor
XSwitchFlags::~XSwitchFlags(void) {
}

// Checks if flags are set.
const bool &XSwitchFlags::isSet(const TCHAR c) const noexcept
{
	static const bool bFalse(false);

	// Lower-case
	if ((c >= TEXT('a')) && (c <= TEXT('z')))
		return flags[(int) (c - TEXT('a'))];
	// Upper-case
	else if ((c >= TEXT('A')) && (c <= TEXT('Z')))
		return flags_cap[(int) (c - TEXT('A'))];
	else if (c == TEXT('-'))	// check if - flag identifier used.
		return flags[26];
	else if (c == TEXT('+'))	// check if + flag identifier used.
		return flags[27];

	return bFalse;
}

// Wrapper for isSet()
const bool &XSwitchFlags::operator[](const TCHAR c) const noexcept {
	return this->isSet(c);
}
