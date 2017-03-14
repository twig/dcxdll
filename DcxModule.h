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
	DcxModule(void) : m_hModule(nullptr) {}
	virtual ~DcxModule(void) = default;

	bool isUseable() const noexcept	{ return !(m_hModule == nullptr); }

	virtual bool load() = 0;
	virtual bool unload() = 0;

protected:
	HMODULE m_hModule;

};

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXMODULES_H_
