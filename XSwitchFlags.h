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

enum class XSwitchValues
	: ULONGLONG
{
	xA = (1ULL << ('A' - 'A')), // 0x0000000000000001
	xB = (1ULL << ('B' - 'A')), // 0x0000000000000002
	xC = (1ULL << ('C' - 'A')), // 0x0000000000000004
	xD = (1ULL << ('D' - 'A')), // 0x0000000000000008
	xE = (1ULL << ('E' - 'A')), // 0x0000000000000010
	xF = (1ULL << ('F' - 'A')), // 0x0000000000000020
	xG = (1ULL << ('G' - 'A')), // 0x0000000000000040
	xH = (1ULL << ('H' - 'A')), // 0x0000000000000080
	xI = (1ULL << ('I' - 'A')), // 0x0000000000000100
	xJ = (1ULL << ('J' - 'A')), // 0x0000000000000200
	xK = (1ULL << ('K' - 'A')), // 0x0000000000000400
	xL = (1ULL << ('L' - 'A')), // 0x0000000000000800
	xM = (1ULL << ('M' - 'A')), // 0x0000000000001000
	xN = (1ULL << ('N' - 'A')), // 0x0000000000002000
	xO = (1ULL << ('O' - 'A')), // 0x0000000000004000
	xP = (1ULL << ('P' - 'A')), // 0x0000000000008000
	xQ = (1ULL << ('Q' - 'A')), // 0x0000000000010000
	xR = (1ULL << ('R' - 'A')), // 0x0000000000020000
	xS = (1ULL << ('S' - 'A')), // 0x0000000000040000
	xT = (1ULL << ('T' - 'A')), // 0x0000000000080000
	xU = (1ULL << ('U' - 'A')), // 0x0000000000100000
	xV = (1ULL << ('V' - 'A')), // 0x0000000000200000
	xW = (1ULL << ('W' - 'A')), // 0x0000000000400000
	xX = (1ULL << ('X' - 'A')), // 0x0000000000800000
	xY = (1ULL << ('Y' - 'A')), // 0x0000000001000000
	xZ = (1ULL << ('Z' - 'A')), // 0x0000000002000000
	// 26 [ 0x0000000004000000
	// 27 | 0x0000000008000000
	// 28 ] 0x0000000010000000
	// 29 ^ 0x0000000020000000
	// 30 _ 0x0000000040000000
	// 31 ' 0x0000000080000000
	xa = (1ULL << ('a' - 'A')), // 0x0000000100000000
	xb = (1ULL << ('b' - 'A')), // 0x0000000200000000
	xc = (1ULL << ('c' - 'A')), // 0x0000000400000000
	xd = (1ULL << ('d' - 'A')), // 0x0000000800000000
	xe = (1ULL << ('e' - 'A')), // 0x0000001000000000
	xf = (1ULL << ('f' - 'A')), // 0x0000002000000000
	xg = (1ULL << ('g' - 'A')), // 0x0000004000000000
	xh = (1ULL << ('h' - 'A')), // 0x0000008000000000
	xi = (1ULL << ('i' - 'A')), // 0x0000010000000000
	xj = (1ULL << ('j' - 'A')), // 0x0000020000000000
	xk = (1ULL << ('k' - 'A')), // 0x0000040000000000
	xl = (1ULL << ('l' - 'A')), // 0x0000080000000000
	xm = (1ULL << ('m' - 'A')), // 0x0000100000000000
	xn = (1ULL << ('n' - 'A')), // 0x0000200000000000
	xo = (1ULL << ('o' - 'A')), // 0x0000400000000000
	xp = (1ULL << ('p' - 'A')), // 0x0000800000000000
	xq = (1ULL << ('q' - 'A')), // 0x0001000000000000
	xr = (1ULL << ('r' - 'A')), // 0x0002000000000000
	xs = (1ULL << ('s' - 'A')), // 0x0004000000000000
	xt = (1ULL << ('t' - 'A')), // 0x0008000000000000
	xu = (1ULL << ('u' - 'A')), // 0x0010000000000000
	xv = (1ULL << ('v' - 'A')), // 0x0020000000000000
	xw = (1ULL << ('w' - 'A')), // 0x0040000000000000
	xx = (1ULL << ('x' - 'A')), // 0x0080000000000000
	xy = (1ULL << ('y' - 'A')), // 0x0100000000000000
	xz = (1ULL << ('z' - 'A')), // 0x0200000000000000
	// 58 empty					   0x0400000000000000
	// 59 empty					   0x0800000000000000
	xMinus = (1ULL << 60),		// 0x1000000000000000
	xPlus = (1ULL << 61)		// 0x2000000000000000
	// 62 empty					   0x4000000000000000
	// 63 empty					   0x8000000000000000
};
struct XSwitchFlags final
{
	XSwitchFlags() = delete;
	XSwitchFlags(const XSwitchFlags&) = delete;
	XSwitchFlags& operator =(const XSwitchFlags&) = delete;
	XSwitchFlags(XSwitchFlags&&) = delete;
	XSwitchFlags& operator =(XSwitchFlags&&) = delete;

	explicit XSwitchFlags(const TString& switches) noexcept;
	XSwitchFlags(const TCHAR* const switches, size_t len) noexcept;
	//~XSwitchFlags() = default;

	// Function checks if flag is set
	const bool isSet(const TCHAR c) const noexcept;
	// returns the whole mask
	const ULONGLONG& getMask() const noexcept { return m_dFlagMask; }

	const bool operator[](const TCHAR c) const noexcept { return isSet(c); }
	explicit operator ULONGLONG() const noexcept { return m_dFlagMask; }

private:
	ULONGLONG	m_dFlagMask{};		//!< a bitmask of all selected letters...

	static constexpr bool m_bFalse{ false };
};

#if DCX_SWITCH_OBJ
#if !__has_include("SwitchObjects.h")
#error "Switch Objects enabled: "SwitchObjects.h" Required!"
#endif
#include "SwitchObjects.h"
#endif

#endif // _XSWITCHFLAGS_H_
