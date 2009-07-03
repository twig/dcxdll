#pragma once
#include "dcxmodule.h"

class DcxUXModule :
	public DcxModule
{

public:
	DcxUXModule(void);
	~DcxUXModule(void);

	bool load(mIRCLinker &mIRCLink);
	bool unload(void);
};
