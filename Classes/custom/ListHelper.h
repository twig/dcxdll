#pragma once

#include "simpleString.h"

// TODO: Change this to a namespace

class DcxListHelper
{
public:
	DcxListHelper() = default;
	~DcxListHelper() = default;

	static void getItemRange(const TString &tsItems, const int nItemCnt, const gsl::not_null<int *> &iStart, const gsl::not_null<int *> &iEnd);
	static std::pair<int, int> getItemRange(const TString &tsItems, const int nItemCnt);
	static bool matchItemText(const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &sItemText, const TString &search, const DcxSearchTypes &SearchType);
	static const DcxSearchTypes StringToSearchType(const TString &tsType) noexcept;
	static const DcxSearchTypes CharToSearchType(const TCHAR &cType) noexcept;
	static const DcxSearchTypes FlagsToSearchType(const XSwitchFlags &xFlags) noexcept;
};

