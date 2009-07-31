#if _MSC_VER > 1000
#pragma once
#endif
#ifndef _DCXMODULES_H_
#define _DCXMODULES_H_

//#include "defines.h"
#include <shlwapi.h>
#include "mIRCLinker.h"

class Dcx;

class DcxModule
{
	mIRCLinker * m_mIRCLink;
	
protected:
	HMODULE m_hModule;

public:
	DcxModule(void);
	virtual ~DcxModule(void);

	bool isUseable() const;

	virtual bool load(mIRCLinker &mIRCLink);
	virtual bool unload() = 0;
};
#endif // _DCXMODULES_H_
