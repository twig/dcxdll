#include "defines.h"
#include "ListHelper.h"


void DcxListHelper::getItemRange(const TString &tsItems, const int nItemCnt, const gsl::not_null<int *> &iStart_range, const gsl::not_null<int *> &iEnd_range)
{
	std::tie(*iStart_range, *iEnd_range) = getItemRange(tsItems, nItemCnt);
}

std::pair<int, int> DcxListHelper::getItemRange(const TString &tsItems, const int nItemCnt)
{
	int iStart = 0, iEnd = 0;
	if (tsItems.numtok(TEXT('-')) == 2)
	{
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
	return { iStart, iEnd };
}

bool DcxListHelper::matchItemText(const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &sItemText, const TString &search, const DcxSearchTypes &SearchType)
{
	switch (SearchType)
	{
	case DcxSearchTypes::SEARCH_R:	// regex match
		return isRegexMatch(sItemText, search.to_chr());
	case DcxSearchTypes::SEARCH_W:	// wildcard match
		return TString(sItemText.data()).iswm(search);
	case DcxSearchTypes::SEARCH_E:   // exact match
		return search == sItemText;
	default:
		return false;
	}
}

const DcxSearchTypes DcxListHelper::StringToSearchType(const TString & tsType) noexcept
{
	const XSwitchFlags xFlags(tsType);

	return FlagsToSearchType(xFlags);
}

const DcxSearchTypes DcxListHelper::CharToSearchType(const TCHAR & cType) noexcept
{
	//auto SearchType = DcxSearchTypes::SEARCH_E;
	//if ((cType == TEXT('R')) || (cType == TEXT('r')))
	//	SearchType = DcxSearchTypes::SEARCH_R;
	//else if ((cType == TEXT('W')) || (cType == TEXT('w')))
	//	SearchType = DcxSearchTypes::SEARCH_W;
	//return SearchType;

	switch (cType)
	{
	case TEXT('R'):
	case TEXT('r'):
		return DcxSearchTypes::SEARCH_R;
	case TEXT('W'):
	case TEXT('w'):
		return DcxSearchTypes::SEARCH_W;
	default:
		return DcxSearchTypes::SEARCH_E;
	}
}

const DcxSearchTypes DcxListHelper::FlagsToSearchType(const XSwitchFlags & xFlags) noexcept
{
	auto SearchType = DcxSearchTypes::SEARCH_E;	// plain text exact match

	if (xFlags[TEXT('w')] || xFlags[TEXT('W')])
		SearchType = DcxSearchTypes::SEARCH_W;	// wildcard
	else if (xFlags[TEXT('r')] || xFlags[TEXT('R')])
		SearchType = DcxSearchTypes::SEARCH_R;	// regex

	return SearchType;
}
