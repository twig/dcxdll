/*
 * Switch Parameters Container
 *
 * The XSwitchFlags class is used to parse /xdid or /xdialog command -switches. It contains two arrays
 * populated by 26 values from a/A-z/Z and of value 1 indicating the switch was in the command or 0 if not.
 *
 * Below is a list stating which switches have been used for global commands.
 * Adding a control specific switch will override the global command.
 * It is important to update this when adding new global commands.
 *
 * a = 0		
 * b = 1		xdid
 * c = 2				XDID
 * d = 3
 * e = 4		xdid
 * f = 5		xdid	XDID
 * g = 6
 * h = 7		xdid
 * i = 8
 * j = 9				XDID
 * k = 10
 * l = 11
 * m = 12			XDID
 * n = 13
 * o = 14
 * p = 15	xdid
 * q = 16
 * r = 17			XDID
 * s = 18	xdid
 * t = 19			XDID
 * u = 20			XDID
 * v = 21
 * w = 22
 * x = 23	xdid
 * y = 24
 * z = 25			XDID
 */

#ifndef _XSWITCHFLAGS_H_
#define _XSWITCHFLAGS_H_

#include "Classes/TString/tstring.h"

class XSwitchFlags {
	public:
		XSwitchFlags(const TString &switches);
		~XSwitchFlags();

		// Function checks if flag is set
		bool isSet(const char c) const;
		bool operator[](const char c) const;

	protected:
		bool flags[26];     //!< Lowercase switches a-z
		bool flags_cap[26]; //!< Uppercase switches A-Z
};

#endif // _XSWITCHFLAGS_H_