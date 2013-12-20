#if _MSC_VER > 1000
#pragma once
#endif
#ifndef _DCXMODULES_H_
#define _DCXMODULES_H_

//#include "defines.h"
#include <shlwapi.h>
#include "mIRCLinker.h"

class Dcx;

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class DcxModule
{
	mIRCLinker * m_mIRCLink;
	
protected:
	HMODULE m_hModule;

public:
	DcxModule(void);
	virtual ~DcxModule(void);

	bool isUseable() const;
	static bool GetWindowVersion(DWORD *dMajor, DWORD *dMinor);

	virtual bool load(mIRCLinker &mIRCLink);
	virtual bool unload() = 0;
};

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXMODULES_H_
