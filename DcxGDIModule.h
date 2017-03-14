#pragma once
#include "dcxmodule.h"

class DcxGDIModule :
	public DcxModule
{
public:
	DcxGDIModule() : m_GDIToken(0UL) {}
	virtual ~DcxGDIModule();

	bool load(void) final;
	bool unload(void) final;

private:
	ULONG_PTR m_GDIToken;
};
