#pragma once

#include "Classes\tstring\tstring.h"

class DcxListHelper
{
public:
	DcxListHelper() = default;
	~DcxListHelper() = default;

	static void getItemRange(const TString &tsItems, const int nItemCnt, int *iStart, int *iEnd);
	static std::pair<int, int> getItemRange(const TString &tsItems, const int nItemCnt);
	static bool matchItemText(const TCHAR *const sItemText, const TString &search, const DcxSearchTypes &SearchType);
};

