#include "defines.h"
#include "DcxDCompModule.h"
#include "Dcx.h"

#pragma warning(push)
#pragma warning(disable: 26425)	//warning C26425 : Assigning 'nullptr' to a static variable.

DcxDCompModule::~DcxDCompModule() noexcept
{
	unload();
}

bool DcxDCompModule::load()
{
	if (isUseable())
		return false;

	// DCompModule Loading
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Loading dcomp.dll..."));
	m_hModule = LoadLibrary(TEXT("dcomp.dll"));

	if (m_hModule)
	{
		// Get function pointers.
		DCompositionCreateDeviceUx = reinterpret_cast<decltype(::DCompositionCreateDevice)*>(::GetProcAddress(m_hModule, "DCompositionCreateDevice"));
		DCompositionCreateDevice2Ux = reinterpret_cast<decltype(::DCompositionCreateDevice2)*>(::GetProcAddress(m_hModule, "DCompositionCreateDevice2"));
		DCompositionCreateDevice3Ux = reinterpret_cast<decltype(::DCompositionCreateDevice3)*>(::GetProcAddress(m_hModule, "DCompositionCreateDevice3"));
		DCompositionCreateSurfaceHandleUx = reinterpret_cast<decltype(::DCompositionCreateSurfaceHandle)*>(::GetProcAddress(m_hModule, "DCompositionCreateSurfaceHandle"));

		if (DCompositionCreateDevice2Ux)
		{
			DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Found DComp Functions"));
		}
		else {
			unload();
			// silently fail...

			DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Missing DComp Functions"));
		}
	}
	else {
		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Failed to load DComp dll"));
	}
	return isUseable();
}

bool DcxDCompModule::unload() noexcept
{
	if (isUseable())
	{
		GSL_SUPPRESS(lifetime.1) FreeLibrary(m_hModule);
		m_hModule = nullptr;
	}
	// make sure all functions are nullptr
	DCompositionCreateDevice2Ux = nullptr;

	return isUseable();
}
#pragma warning(pop)


HRESULT DcxDCompModule::dcxDCompositionCreateDevice2(IUnknown* renderingDevice, REFIID iid, void** dcompositionDevice) noexcept
{
	if (DCompositionCreateDevice2Ux)
		return DCompositionCreateDevice2Ux(renderingDevice, iid, dcompositionDevice);

	return E_NOTIMPL;
}
