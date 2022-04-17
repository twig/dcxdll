#pragma once
#ifndef _DCXDPIMODULES_H_
#define _DCXDPIMODULES_H_

#include "dcxmodule.h"

typedef UINT(WINAPI* PFNGETDPIFORSYSTEM)();
typedef UINT(WINAPI* PFNGETDPIFORWINDOW)(_In_ HWND);
typedef int(WINAPI* PFNGETSYSTEMMETRICSFORDPI)(_In_ int, _In_ UINT);

class DcxDPIModule final
	: public DcxModule
{
	static PFNGETDPIFORSYSTEM GetDpiForSystemUx;
	static PFNGETDPIFORWINDOW GetDpiForWindowUx;
	static PFNGETSYSTEMMETRICSFORDPI GetSystemMetricsForDpiUx;

public:
	constexpr DcxDPIModule(void) noexcept
		: DcxModule()
	{}
	~DcxDPIModule(void) noexcept;

	DcxDPIModule(const DcxDPIModule& other) = delete;	// no copy constructor
	DcxDPIModule(const DcxDPIModule&& other) = delete;	// no move constructor
	DcxDPIModule& operator =(const DcxDPIModule&) = delete;	// No assignments!
	DcxDPIModule& operator =(const DcxDPIModule&&) = delete;	// No move assignments!

	bool load(void) final;
	bool unload(void) noexcept final;

	static UINT dcxGetDpiForSystem(void) noexcept;
	static UINT dcxGetDpiForWindow(_In_ HWND hwnd) noexcept;
	static UINT dcxGetSystemMetricsForDpi(_In_ int nIndex, _In_ UINT dpi) noexcept;
	static UINT dcxGetWindowMetrics(_In_ HWND hwnd, _In_ int nIndex) noexcept;
};
#endif // _DCXUXMODULES_H_
