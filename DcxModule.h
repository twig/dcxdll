#pragma once

#include "defines.h"
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

	bool isUseable();

	virtual bool load(mIRCLinker &mIRCLink);
	virtual bool unload() = 0;
};
