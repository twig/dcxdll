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
