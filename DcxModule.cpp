#include "defines.h"
#include "DcxModule.h"

DcxModule::DcxModule(void)
{
	m_hModule = NULL;
}

DcxModule::~DcxModule(void) 
{
}

bool DcxModule::isUseable() const
{
	return !(m_hModule == NULL);
}

bool DcxModule::load(mIRCLinker &mIRCLink) 
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
		if (dMajor != NULL)
			*dMajor = osvi.dwMajorVersion;
		if (dMinor != NULL)
			*dMinor = osvi.dwMinorVersion;
		return true;
	}
#pragma warning(pop)
	return false;
}
