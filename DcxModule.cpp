#include "defines.h"
#include "DcxModule.h"

DcxModule::DcxModule(void)
: m_hModule(nullptr)
{
}

DcxModule::~DcxModule(void) 
{
}

bool DcxModule::isUseable() const
{
	return !(m_hModule == nullptr);
}

bool DcxModule::load(void)
{
	return true;
}

bool DcxModule::GetWindowVersion(DWORD *dMajor, DWORD *dMinor)
{
	// get OS version.
	OSVERSIONINFO osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

#pragma warning(push)
#pragma warning(disable:4996)
	if (GetVersionEx(&osvi)) {
		if (dMajor != nullptr)
			*dMajor = osvi.dwMajorVersion;
		if (dMinor != nullptr)
			*dMinor = osvi.dwMinorVersion;
		return true;
	}
#pragma warning(pop)
	return false;
}
