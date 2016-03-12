#pragma once
#include "dcxmodule.h"

class DcxGDIModule :
	public DcxModule
{
	ULONG_PTR m_GDIToken;

public:
	DcxGDIModule(void);
	~DcxGDIModule(void);

	bool load(void) final;
	bool unload(void) final;
};
