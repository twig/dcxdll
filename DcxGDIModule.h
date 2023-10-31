#pragma once
#include "dcxmodule.h"

class DcxGDIModule final
	: public DcxModule
{
public:
	constexpr DcxGDIModule() noexcept
		: DcxModule()
	{}
	~DcxGDIModule() noexcept;

	DcxGDIModule(const DcxGDIModule &other) = delete;	// no copy constructor
	DcxGDIModule(const DcxGDIModule &&other) = delete;	// no move constructor
	GSL_SUPPRESS(c.128) DcxGDIModule &operator =(const DcxGDIModule &) = delete;	// No assignments!
	GSL_SUPPRESS(c.128) DcxGDIModule &operator =(const DcxGDIModule &&) = delete;	// No move assignments!

	bool load(void) final;
	bool unload(void) noexcept final;

private:
	ULONG_PTR m_GDIToken{};
};
