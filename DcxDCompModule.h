#pragma once
#ifndef _DCXDCOMPMODULE_H_
#define _DCXDCOMPMODULE_H_

#include "dcxmodule.h"

#include <dcomp.h>

class DcxDCompModule final
	: public DcxModule
{
	static inline decltype(::DCompositionCreateDevice2)* DCompositionCreateDevice2Ux = nullptr;

public:
	constexpr DcxDCompModule(void) noexcept
		: DcxModule()
	{}
	~DcxDCompModule(void) noexcept;

	DcxDCompModule(const DcxDCompModule& other) = delete;	// no copy constructor
	DcxDCompModule(const DcxDCompModule&& other) = delete;	// no move constructor
	GSL_SUPPRESS(c.128) DcxDCompModule& operator =(const DcxDCompModule&) = delete;	// No assignments!
	GSL_SUPPRESS(c.128) DcxDCompModule& operator =(const DcxDCompModule&&) = delete;	// No move assignments!

	bool load(void) final;
	bool unload(void) noexcept final;

	static HRESULT dcxDCompositionCreateDevice2(
		_In_opt_ IUnknown* renderingDevice,
		_In_ REFIID iid,
		_Outptr_ void** dcompositionDevice
	);
};
#endif // _DCXDCOMPMODULE_H_
