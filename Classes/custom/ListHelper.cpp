#include "defines.h"
#include "ListHelper.h"


std::pair<int, int> DcxListHelper::getItemRange(const TString& tsItems, const int nItemCnt)
{
	//int iStart = 0, iEnd = 0;
	//if (tsItems.numtok(TEXT('-')) == 2)
	//{
	//	iStart = tsItems.getfirsttok(1, TEXT('-')).to_int() - 1;
	//	iEnd = tsItems.getnexttok(TEXT('-')).to_int() - 1;
	//
	//	if (iEnd == -1)	// special case
	//		iEnd = nItemCnt - 1;
	//}
	//else {
	//	iEnd = tsItems.to_int() - 1;
	//
	//	if (iEnd == -1)	// special case
	//		iStart = iEnd = nItemCnt - 1;
	//	else
	//		iStart = iEnd;
	//}
	//return { iStart, iEnd };

	const auto rng = Dcx::make_range(tsItems, nItemCnt);
	return { rng.b, rng.e };
}

Dcx::range_t<int> DcxListHelper::getItemRange2(const TString& tsItems, const int nItemCnt)
{
	//int iStart = 0, iEnd = 0;
	//if (tsItems.numtok(TEXT('-')) == 2)
	//{
	//	iStart = tsItems.getfirsttok(1, TEXT('-')).to_int() - 1;
	//	iEnd = tsItems.getnexttok(TEXT('-')).to_int() - 1;
	//
	//	if (iEnd == -1)	// special case
	//		iEnd = nItemCnt - 1;
	//}
	//else {
	//	iEnd = tsItems.to_int() - 1;
	//
	//	if (iEnd == -1)	// special case
	//		iStart = iEnd = nItemCnt - 1;
	//	else
	//		iStart = iEnd;
	//}
	//return Dcx::make_range(iStart, iEnd);

	return Dcx::make_range(tsItems, nItemCnt);
}

//bool DcxListHelper::matchItemText(const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &sItemText, const TString &search, const DcxSearchTypes &SearchType)
//{
//	switch (SearchType)
//	{
//	case DcxSearchTypes::SEARCH_R:	// regex match
//		return isRegexMatch(sItemText, search.to_chr());
//	case DcxSearchTypes::SEARCH_W:	// wildcard match
//		//return TString(sItemText.data()).iswm(search);	// makes copy of string...
//		return _ts_WildcardMatch(sItemText, search);		// avoids copy
//	case DcxSearchTypes::SEARCH_E:   // exact match
//		return search == sItemText;
//	default:
//		return false;
//	}
//}

bool DcxListHelper::matchItemText(const TCHAR* sItemText, const TString& search, const DcxSearchTypes& SearchType)
{
	const dcxSearchData srch_data(search, SearchType);
	return matchItemText(sItemText, srch_data);
}

bool DcxListHelper::matchItemText(const TCHAR* sItemText, const dcxSearchData& srch) noexcept
{
	switch (srch.m_SearchType)
	{
	case DcxSearchTypes::SEARCH_R:	// regex match
		return isRegexMatch(sItemText, srch);
	case DcxSearchTypes::SEARCH_W:	// wildcard match
		return _ts_WildcardMatch(sItemText, srch.m_tsSearch);		// avoids copy
	case DcxSearchTypes::SEARCH_E:   // exact match
		return srch.m_tsSearch == sItemText;
	default:
		return false;
	}
}

const DcxSearchTypes DcxListHelper::StringToSearchType(const TString& tsType) noexcept
{
	const XSwitchFlags xFlags(tsType);

	return FlagsToSearchType(xFlags);
}

const DcxSearchTypes DcxListHelper::CharToSearchType(const TCHAR& cType) noexcept
{
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

const DcxSearchTypes DcxListHelper::FlagsToSearchType(const XSwitchFlags& xFlags) noexcept
{
	auto SearchType = DcxSearchTypes::SEARCH_E;	// plain text exact match

	if (xFlags[TEXT('w')] || xFlags[TEXT('W')])
		SearchType = DcxSearchTypes::SEARCH_W;	// wildcard
	else if (xFlags[TEXT('r')] || xFlags[TEXT('R')])
		SearchType = DcxSearchTypes::SEARCH_R;	// regex
	return SearchType;

	//auto SearchType = DcxSearchTypes::SEARCH_E;	// plain text exact match
	//switch (to_Enum<XSwitchValues>(xFlags.getMask()))
	//{
	//case XSwitchValues::xW:
	//case XSwitchValues::xw:
	//	SearchType = DcxSearchTypes::SEARCH_W;	// wildcard
	//	break;
	//case XSwitchValues::xR:
	//case XSwitchValues::xr:
	//	SearchType = DcxSearchTypes::SEARCH_R;	// regex
	//	break;
	//default:
	//	break;
	//}
	//return SearchType;
}
