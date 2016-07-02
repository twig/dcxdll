#pragma once

#include "simpleString.h"

class DcxListHelper
{
public:
	DcxListHelper() = default;
	~DcxListHelper() = default;

	static void getItemRange(const TString &tsItems, const int nItemCnt, int *iStart, int *iEnd);
	static std::pair<int, int> getItemRange(const TString &tsItems, const int nItemCnt);
	static bool matchItemText(const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &sItemText, const TString &search, const DcxSearchTypes &SearchType);
};

