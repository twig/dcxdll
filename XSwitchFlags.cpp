#include "defines.h"

// Constructor
XSwitchFlags::XSwitchFlags(const TString &switches) {
	ZeroMemory(flags, sizeof(bool) * 26);
	ZeroMemory(flags_cap, sizeof(bool) * 26);

	// no - sign, invalid syntax
	if ((switches)[0] != TEXT('-'))
		return;

	unsigned int i = 1;
	unsigned int len = (UINT)switches.len();
	TCHAR c;

	// Parse the switches
	while (i < len) {
		c = switches[i];

		// Check if its in the right range
		if (c >= TEXT('a') && c <= TEXT('z'))
			flags[(int) (c - TEXT('a'))] = true;
		else if (c >= TEXT('A') && (c <= TEXT('Z')))
			flags_cap[(int) (c - TEXT('A'))] = true;

		i++;
	}
}

// Destructor
XSwitchFlags::~XSwitchFlags(void) {
}

// Checks if flags are set.
bool XSwitchFlags::isSet(const TCHAR c) const {
	// Lower-case
	if ((c >= TEXT('a')) && (c <= TEXT('z')))
		return flags[(int) (c - TEXT('a'))];
	// Upper-case
	else if ((c >= TEXT('A')) && (c <= TEXT('Z')))
		return flags_cap[(int) (c - TEXT('A'))];

	return false;
}

// Wrapper for isSet()
bool XSwitchFlags::operator[](const TCHAR c) const {
	return this->isSet(c);
}
