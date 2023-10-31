#pragma once
#ifndef _DCXMODULES_H_
#define _DCXMODULES_H_

//#include "defines.h"
#include <shlwapi.h>
#include "mIRCLinker.h"

//class Dcx;

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class DcxModule
{
public:
	constexpr DcxModule(void) noexcept {}
	virtual ~DcxModule(void) noexcept {};
	DcxModule(const HMODULE& m_hModule) noexcept
		: m_hModule(m_hModule)
	{
	}

	DcxModule(const DcxModule& other) = delete;	// no copy constructor
	DcxModule(const DcxModule&& other) = delete;	// no move constructor
	DcxModule& operator =(const DcxModule&) = delete;	// No assignments!
	DcxModule& operator =(const DcxModule&&) = delete;	// No move assignments!

	bool operator==(const DcxModule& other) const = default;

	bool isUseable() const noexcept { return !(m_hModule == nullptr); }

	virtual bool load() = 0;
	virtual bool unload() = 0;

protected:
	HMODULE m_hModule{ nullptr };
};

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXMODULES_H_
