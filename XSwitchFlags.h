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

//class XSwitchFlags {
//	public:
//		XSwitchFlags() = delete;
//		XSwitchFlags(const XSwitchFlags &) = delete;
//		XSwitchFlags &operator =(const XSwitchFlags &) = delete;	// No assignments!
//
//		explicit XSwitchFlags(const TString &switches) noexcept;
//		~XSwitchFlags() = default;
//
//		// Function checks if flag is set
//		const bool &isSet(const TCHAR c) const noexcept;
//		const bool &operator[](const TCHAR c) const noexcept;
//
////	protected:
//private:
//		bool flags[28];     //!< Lowercase switches a-z
//		bool flags_cap[26]; //!< Uppercase switches A-Z
//};

struct XSwitchFlags {
	XSwitchFlags() = delete;
	XSwitchFlags(const XSwitchFlags &) = delete;
	XSwitchFlags &operator =(const XSwitchFlags &) = delete;	// No assignments!

	explicit XSwitchFlags(const TString &switches) noexcept;
	~XSwitchFlags() = default;

	// Function checks if flag is set
	//const bool isSet(const TCHAR c) const noexcept;
	//const bool &operator[](const TCHAR c) const noexcept { return isSet(c); }

	const bool isSet(const TCHAR c) const noexcept;
	const bool operator[](const TCHAR c) const noexcept { return isSet(c); }
	explicit operator ULONGLONG() const noexcept { return m_dFlagMask; }

private:
	//bool	m_bFlags[28];		//!< Lowercase switches a-z
	//bool	m_bFlags_cap[26];	//!< Uppercase switches A-Z
	ULONGLONG	m_dFlagMask;		//!< a bitmask of all selected letters...

	static const bool m_bFalse;
};

#ifdef DCX_SWITCH_OBJ
#include "SwitchObjects.h"
#endif

#endif // _XSWITCHFLAGS_H_
