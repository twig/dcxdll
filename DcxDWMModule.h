#pragma once
#include "dcxmodule.h"

class DcxDWMModule :
	public DcxModule
{
	bool m_bAero;

public:
	DcxDWMModule(void);
	~DcxDWMModule(void);

	bool load(mIRCLinker &mIRCLink);
	bool unload(void);
	bool refreshComposite();

	bool isAero(void);
};
