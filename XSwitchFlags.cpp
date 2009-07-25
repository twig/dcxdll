#include ".\defines.h"

// Constructor
XSwitchFlags::XSwitchFlags(const TString &switches) {
	ZeroMemory(flags, sizeof(bool) * 26);
	ZeroMemory(flags_cap, sizeof(bool) * 26);

	// no - sign, invalid syntax
	if ((switches)[0] != '-')
		return;

	unsigned int i = 1;
	unsigned int len = (UINT)switches.len();
	char c;

	// Parse the switches
	while (i < len) {
		c = switches[i];

		// Check if its in the right range
		if (c >= 'a' && c <= 'z')
			flags[(int) (c - 'a')] = true;
		else if (c >= 'A' && (c <= 'Z'))
			flags_cap[(int) (c - 'A')] = true;

		i++;
	}
}

// Destructor
XSwitchFlags::~XSwitchFlags(void) {
}

// Checks if flags are set.
bool XSwitchFlags::isSet(const char c) const {
	// Lower-case
	if ((c >= 'a') && (c <= 'z'))
		return flags[(int) (c - 'a')];
	// Upper-case
	else if ((c >= 'A') && (c <= 'Z'))
		return flags_cap[(int) (c - 'A')];

	return false;
}

// Wrapper for isSet()
bool XSwitchFlags::operator[](const char c) const {
	return this->isSet(c);
}
