#pragma once
#include "dcxmodule.h"

class DcxGDIModule :
	public DcxModule
{
public:
	constexpr DcxGDIModule() noexcept
		: DcxModule()
		, m_GDIToken(0UL)
	{}
	virtual ~DcxGDIModule() noexcept;

	DcxGDIModule(const DcxGDIModule &other) = delete;	// no copy constructor
	DcxGDIModule(const DcxGDIModule &&other) = delete;	// no move constructor
	DcxGDIModule &operator =(const DcxGDIModule &) = delete;	// No assignments!
	DcxGDIModule &operator =(const DcxGDIModule &&) = delete;	// No move assignments!

	bool load(void) final;
	bool unload(void) noexcept final;

private:
	ULONG_PTR m_GDIToken;
};
