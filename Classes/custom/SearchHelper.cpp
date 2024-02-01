#include "defines.h"
#include "SearchHelper.h"

bool DcxSearchHelper::isRegexMatch(const TCHAR* matchtext, const TCHAR* pattern) noexcept
{
	if ((!matchtext) || (!pattern))
		return false;

	const dcxSearchData srch_data(pattern, DcxSearchTypes::SEARCH_R);
	return isRegexMatch(matchtext, srch_data);
}

bool DcxSearchHelper::isRegexMatch(const TCHAR* matchtext, const dcxSearchData& srch_data) noexcept
{
	if (!matchtext)
		return false;

	try {
		// NB: CREGEX version is incomplete
#if DCX_USE_CREGEX
		try {
			if (srch_data.m_regexOpts.m_Option_S)
			{
				TString tsMatchText(matchtext);

				std::basic_regex<TCHAR> r(srch_data.m_Opts.m_tsPattern.to_chr(), std::regex_constants::awk);
				return std::regex_search(tsMatchText.strip().to_chr(), r);
			}
			else {
				std::basic_regex<TCHAR> r(srch_data.m_Opts.m_tsPattern.to_chr(), std::regex_constants::awk);
				return std::regex_search(tsMatchText.to_chr(), r);
			}
		}
		catch (const std::regex_error) {
		}
		return false;
#else
#if DCX_USE_PCRE2
		if (srch_data.m_regexOpts.m_UsePCRE2)
		{
			int res{};

			if (srch_data.m_regexOpts.m_Option_S)
			{
				TString tsMatchText(matchtext);


				//res = pcre2_match(srch_data.m_regexOpts.m_re, reinterpret_cast<PCRE2_SPTR>(tsMatchText.strip().to_chr()), PCRE2_ZERO_TERMINATED, 0, 0, srch_data.m_regexOpts.m_Match_data, nullptr);
				//res = pcre2_match(srch_data.m_regexOpts.m_re, tsMatchText.strip().to_<PCRE2_SPTR>(), PCRE2_ZERO_TERMINATED, 0, 0, srch_data.m_regexOpts.m_Match_data, nullptr);

				const auto tsStripped(tsMatchText.strip());
				// Convert to utf-32.
				auto str32(srch_data.m_regexOpts.convert_utf16_to_utf32((const char16_t*)tsStripped.to_wchr(), tsStripped.len()));
				res = pcre2_match_32(srch_data.m_regexOpts.m_re, reinterpret_cast<PCRE2_SPTR32>(str32.get()), PCRE2_ZERO_TERMINATED, 0, 0, srch_data.m_regexOpts.m_Match_data, nullptr);
			}
			else {
				//res = pcre2_match(srch_data.m_regexOpts.m_re, reinterpret_cast<PCRE2_SPTR>(matchtext), PCRE2_ZERO_TERMINATED, 0, 0, srch_data.m_regexOpts.m_Match_data, nullptr);

				// Convert to utf-32.
				auto str32(srch_data.m_regexOpts.convert_utf16_to_utf32((const char16_t*)matchtext, _ts_strlen(matchtext)));
				res = pcre2_match_32(srch_data.m_regexOpts.m_re, reinterpret_cast<PCRE2_SPTR32>(str32.get()), PCRE2_ZERO_TERMINATED, 0, 0, srch_data.m_regexOpts.m_Match_data, nullptr);
			}
			return (res > 0);
		}
		else {
			stString<10> res;
			mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_text %"), matchtext);
			mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_regex %"), srch_data.m_tsSearch);
			mIRCLinker::eval(res, TEXT("$regex(%dcx_text,%dcx_regex)"));

			return (dcx_atoi(res.data()) > 0);
		}
#else
		stString<10> res;
		mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_text %"), matchtext);
		mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_regex %"), srch_data.m_tsSearch);
		mIRCLinker::eval(res, TEXT("$regex(%dcx_text,%dcx_regex)"));

		return (dcx_atoi(res.data()) > 0);
#endif
#endif // DCX_USE_CREGEX
	}
	catch (...)
	{
		return false;
	}
}

//bool DcxSearchHelper::isRegexMatchValid(const std::optional<RegexResults> &in) noexcept
//{
//	if (in.has_value())
//	{
//		return (in->nEnd < PCRE2_SIZE_MAX);
//	}
//	return false;
//}

std::optional<RegexResults> DcxSearchHelper::getRegexMatchOffset(const TCHAR* matchtext, const TCHAR* pattern)
{
	if ((!matchtext) || (!pattern))
		return {};

	const dcxSearchData srch_data(pattern, DcxSearchTypes::SEARCH_R);
	return getRegexMatchOffset(matchtext, srch_data);
}

std::optional<RegexResults> DcxSearchHelper::getRegexMatchOffset(const TCHAR* matchtext, const dcxSearchData& srch_data) noexcept
{
	if (!matchtext)
		return {};

	try {
		// NB: CREGEX version is incomplete
#if DCX_USE_CREGEX
		if (srch_data.m_regexOpts.m_Option_S)
		{
			TString tsMatchText(matchtext);

			std::basic_regex<TCHAR> r(srch_data.m_Opts.m_tsPattern.to_chr(), std::regex_constants::awk);
			return std::regex_search(tsMatchText.strip().to_chr(), r);
		}
		else {
			std::basic_regex<TCHAR> r(srch_data.m_Opts.m_tsPattern.to_chr(), std::regex_constants::awk);
			return std::regex_search(tsMatchText.to_chr(), r);
		}
#else
#if DCX_USE_PCRE2
		if (srch_data.m_regexOpts.m_UsePCRE2)
		{
			int res{};

			if (srch_data.m_regexOpts.m_Option_S)
			{
				TString tsMatchText(matchtext);

				res = pcre2_match(srch_data.m_regexOpts.m_re, tsMatchText.strip().to_<PCRE2_SPTR>(), PCRE2_ZERO_TERMINATED, 0, 0, srch_data.m_regexOpts.m_Match_data, nullptr);
			}
			else {
				res = pcre2_match(srch_data.m_regexOpts.m_re, reinterpret_cast<PCRE2_SPTR>(matchtext), PCRE2_ZERO_TERMINATED, 0, 0, srch_data.m_regexOpts.m_Match_data, nullptr);
			}
			if (res > 0)
			{
				//size_t sz{};
				//pcre2_substring_length_bynumber(srch_data.m_regexOpts.m_Match_data, 1, &sz);
				//return pcre2_get_startchar(srch_data.m_regexOpts.m_Match_data);

				if (const auto cnt = pcre2_get_ovector_count(srch_data.m_regexOpts.m_Match_data); cnt > 0)
				{
					const auto ov = pcre2_get_ovector_pointer(srch_data.m_regexOpts.m_Match_data);
					const auto nStart = ov[0];
					const auto nEnd = ov[(cnt * 2) - 1];

					return { RegexResults{nStart, nEnd} };
				}
			}
			return {};
		}
		else {
			stString<10> res;
			mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_text %"), matchtext);
			mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_regex %"), srch_data.m_tsSearch);
			mIRCLinker::eval(res, TEXT("$regex(%dcx_text,%dcx_regex).pos"));

			const size_t nStart = dcx_atoi(res.data());
			//const size_t nStart = _ts_strtoul((const TCHAR *)res.data(), nullptr, 10);

			return { RegexResults{ nStart, PCRE2_SIZE_MAX } };
		}
#else
		stString<10> res;
		mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_text %"), matchtext);
		mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_regex %"), srch_data.m_tsSearch);
		mIRCLinker::eval(res, TEXT("$regex(%dcx_text,%dcx_regex).pos"));

		const size_t nStart = dcx_atoi(res.data());

		return { RegexResults{ nStart, PCRE2_SIZE_MAX } };
#endif
#endif // DCX_USE_CREGEX
	}
	catch (...)
	{
	}
	return {};
}

bool DcxSearchHelper::matchItemText(const TCHAR* sItemText, const TString& search, const DcxSearchTypes& SearchType)
{
	const dcxSearchData srch_data(search, SearchType);
	return matchItemText(sItemText, srch_data);
}

bool DcxSearchHelper::matchItemText(const TCHAR* sItemText, const dcxSearchData& srch) noexcept
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

const DcxSearchTypes DcxSearchHelper::StringToSearchType(const TString& tsType) noexcept
{
	const XSwitchFlags xFlags(tsType);

	return FlagsToSearchType(xFlags);
}

const DcxSearchTypes DcxSearchHelper::CharToSearchType(const TCHAR& cType) noexcept
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

const DcxSearchTypes DcxSearchHelper::FlagsToSearchType(const XSwitchFlags& xFlags) noexcept
{
	auto SearchType = DcxSearchTypes::SEARCH_E;	// plain text exact match

	if (xFlags[TEXT('w')] || xFlags[TEXT('W')])
		SearchType = DcxSearchTypes::SEARCH_W;	// wildcard
	else if (xFlags[TEXT('r')] || xFlags[TEXT('R')])
		SearchType = DcxSearchTypes::SEARCH_R;	// regex
	return SearchType;
}

std::optional<DcxSearchResults> DcxSearchHelper::matchText(const TString &txt, const UINT nChar, const TString& search, const DcxSearchTypes& SearchType)
{
	const dcxSearchData srch_data(search, SearchType);

	return matchText(txt, nChar, srch_data);
}

std::optional<DcxSearchResults> DcxSearchHelper::matchText(const TString& txt, const UINT nLine, const UINT nSubChar, const TString& search, const DcxSearchTypes& SearchType)
{
	if (nLine == 0)
		return { };

	// [LINE] & [SUBCHAR] supplied.
	// adjust nChar to the correct pos.

	// is line number to high?
	if (nLine > txt.numtok(TEXT("\r\n")))
		return { };

	// find line character offset.
	const auto LineOffset = txt.find(TEXT("\r\n"), nLine);

	// couldnt find line?
	if (LineOffset == -1)
		return { };

	const UINT nChar = LineOffset + nSubChar;

	// is char outside of range allowed?
	if (nChar >= txt.len())
		return { };

	return matchText(txt, nChar, search, SearchType);
}

std::optional<DcxSearchResults> DcxSearchHelper::matchText(const TString &txt, const UINT nChar, const dcxSearchData& srch_data)
{
	if (nChar >= txt.len())
		return { };

	const auto szStart = &txt.to_chr()[nChar];

	switch (srch_data.m_SearchType)
	{
	case DcxSearchTypes::SEARCH_R:	// regex match
	{
		if (const auto m = getRegexMatchOffset(szStart, srch_data); m.has_value())
		{
			const auto nStart = m->nStart + nChar;
			const auto nEnd = m->nEnd + nChar;

			return { DcxSearchResults{ true, nStart, nEnd } };
		}
	}
	break;
	case DcxSearchTypes::SEARCH_W:	// wildcard match
	{
		if (const auto bFound = _ts_WildcardMatch(szStart, srch_data.m_tsSearch); bFound)
		{
			const auto nStart = gsl::narrow_cast<size_t>(szStart - txt.to_chr());
			const auto nEnd = nStart + txt.len();

			return { DcxSearchResults{ true, nStart, nEnd } };
		}
	}
	break;
	case DcxSearchTypes::SEARCH_E:   // exact match
	{
		if (const auto szFound = _ts_find(szStart, srch_data.m_tsSearch.to_chr()); szFound)
		{
			const auto nStart = gsl::narrow_cast<UINT>((szFound - txt.to_chr()));
			const auto nEnd = nStart + srch_data.m_tsSearch.len();

			return { DcxSearchResults{ true, nStart, nEnd } };
		}
	}
	break;
	default:
		break;
	}
	return { };
}

TString DcxSearchHelper::findTextRange(const TString&  tsText, const TString& tsMatchText, const TString& tsParams)
{
	// get search type to use.
	const auto SearchType = CharToSearchType(tsParams++[0]);

	// find the Nth match (zero for count all matches)
	const auto N = tsParams++.to_<UINT>();		// tok 2
	// get line to search from
	const auto nLine = tsParams++.to_<UINT>();	// tok 3
	// get the char on that line to search from
	auto nChar = tsParams++.to_<UINT>();	// tok 4

	// nLine is either lines or chars, either must be > 0
	if (nLine < 1)
		throw DcxExceptions::dcxInvalidArguments();

	if (tsParams.numtok() == 4)
	{
		// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N] [LINE] [SUBCHAR]
		// [LINE] & [SUBCHAR] supplied.
		// nLine = line
		// nChar = char offset in that line (can be zero)
		// 
		// adjust nChar to the correct pos.
		if (nLine > tsText.numtok(TEXT("\r\n")))
			throw DcxExceptions::dcxInvalidArguments();

		// only add lines if > than first line.
		if (nLine > 1)
		{
			auto LineOffset = tsText.find(TEXT("\r\n"), nLine);
			if (LineOffset == -1)
				LineOffset = 0;

			nChar += LineOffset;
		}
	}
	else
		// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N] [CHAR]
		// nLine = chars
		// nChar = unset untill now
		nChar = nLine; // nLine is chars
	// NB: nChar is 1-based at this point.
	--nChar;

	TString tsRes;

	// count total
	if (N == 0)
	{
		UINT count{};
		for (auto match = matchText(tsText, nChar, tsMatchText, SearchType); match.has_value(); match = matchText(tsText, match->m_nEnd + 1, tsMatchText, SearchType))
			++count;

		_ts_snprintf(tsRes, TEXT("%u"), count);
	}
	// find Nth matching
	else {
		UINT count{};

		for (auto match = matchText(tsText, nChar, tsMatchText, SearchType); match.has_value(); match = matchText(tsText, match->m_nEnd + 1, tsMatchText, SearchType))
		{
			++count;

			// found Nth matching
			if (count == N)
			{
				_ts_snprintf(tsRes, TEXT("%u %u"), match->m_nStart + 1, match->m_nEnd); // return the char offset.
				return tsRes;
			}
		}
		tsRes = TEXT("-1");
	} // else

	return tsRes;
}
