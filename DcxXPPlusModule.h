#pragma once

class DcxXPPlusModule :
	public DcxModule
{
public:
	DcxXPPlusModule(void);
	virtual ~DcxXPPlusModule(void);

	bool load(mIRCLinker &mIRCLink);
	bool unload(void) { return true; }
};
