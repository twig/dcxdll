#pragma once

#include "simpleString.h"

// TODO: Change this to a namespace

class DcxListHelper
{
public:
	static std::pair<int, int> getItemRange(const TString& tsItems, const int nItemCnt);
	static Dcx::range_t<int> getItemRange2(const TString& tsItems, const int nItemCnt);

	static bool matchItemText(const TCHAR* sItemText, const TString& search, const DcxSearchTypes& SearchType);

	static bool matchItemText(const TCHAR* sItemText, const dcxSearchData& srch) noexcept;

	static const DcxSearchTypes StringToSearchType(const TString& tsType) noexcept;
	static const DcxSearchTypes CharToSearchType(const TCHAR& cType) noexcept;
	static const DcxSearchTypes FlagsToSearchType(const XSwitchFlags& xFlags) noexcept;
};

