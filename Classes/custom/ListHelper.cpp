#include "defines.h"
#include "ListHelper.h"


void DcxListHelper::getItemRange(const TString &tsItems, const int nItemCnt, int *iStart_range, int *iEnd_range)
{
	std::tie(*iStart_range, *iEnd_range) = getItemRange(tsItems, nItemCnt);
}

std::pair<int, int> DcxListHelper::getItemRange(const TString &tsItems, const int nItemCnt)
{
	int iStart = 0, iEnd = 0;
	if (tsItems.numtok(TEXT('-')) == 2) {
		iStart = tsItems.getfirsttok(1, TEXT('-')).to_int() - 1;
		iEnd = tsItems.getnexttok(TEXT('-')).to_int() - 1;

		if (iEnd == -1)	// special case
			iEnd = nItemCnt - 1;
	}
	else {
		iEnd = tsItems.to_int() - 1;

		if (iEnd == -1)	// special case
			iStart = iEnd = nItemCnt - 1;
		else
			iStart = iEnd;
	}
	return std::make_pair(iStart, iEnd);
}

bool DcxListHelper::matchItemText(const TCHAR *const sItemText, const TString &search, const DcxSearchTypes &SearchType)
{
	switch (SearchType)
	{
	case DcxSearchTypes::SEARCH_R:	// regex match
		return isRegexMatch(sItemText, search.to_chr());
	case DcxSearchTypes::SEARCH_W:	// wildcard match
		return TString(sItemText).iswm(search);
	case DcxSearchTypes::SEARCH_E:   // exact match
		return (lstrcmp(sItemText, search.to_chr()) == 0);
	default:
		return false;
	}
}
