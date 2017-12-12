#pragma once
#include "dcxmodule.h"

class DcxGDIModule :
	public DcxModule
{
public:
	constexpr DcxGDIModule() noexcept
		: m_GDIToken(0UL)
	{}
	virtual ~DcxGDIModule() noexcept;

	bool load(void) final;
	bool unload(void) noexcept final;

private:
	ULONG_PTR m_GDIToken;
};
