#include "defines.h"
#include "DcxXPPlusModule.h"

DcxXPPlusModule::DcxXPPlusModule(void)
{
}

DcxXPPlusModule::~DcxXPPlusModule(void)
{
}

bool DcxXPPlusModule::load(mIRCLinker &mIRCLink)
{
	if (isUseable()) return false;

	DWORD winMajor = 0, winMinor = 0;
	if (!GetWindowVersion(&winMajor, &winMinor))
		return false;

	const bool bIsWindowsXPorLater = ( (winMajor > 5) || ( (winMajor == 5) && (winMinor >= 1) ));

	if (bIsWindowsXPorLater)
		m_hModule = (HMODULE)1;

	return isUseable();
}