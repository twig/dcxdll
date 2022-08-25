#pragma once

#if DCX_USE_CREGEX
#include <regex>
#endif

#if DCX_USE_PCRE2
#define PCRE2_CODE_UNIT_WIDTH 16
#define PCRE2_STATIC 1
#include <pcre2/pcre2.h>
#ifdef NDEBUG
#pragma comment(lib, "pcre2-16-static.lib")
#else
#pragma comment(lib, "pcre2-16-staticd.lib")
#endif
#endif

// Search types...
enum class DcxSearchTypes
	: UINT
{
	SEARCH_W = 0x01,	//!< WildCard Search
	SEARCH_R,			//!< Regex Search
	SEARCH_E			//!< Exact Match
};

struct regexOptions
{
	TString m_tsPattern;
	UINT m_Opts{};
	bool m_UsePCRE2{ false };

	regexOptions() noexcept = default;
	regexOptions(const regexOptions&) = delete;

#if DCX_USE_PCRE2
	bool m_Option_g{ false };
	bool m_Option_S{ false };
	bool m_Option_X{ false };
	bool m_Option_F{ false };

	int m_Error{};
	size_t m_Erroffset{};
	pcre2_match_data* m_Match_data{};
	pcre2_code* m_re{};
	pcre2_compile_context* m_context{};

	void swap(regexOptions& other) noexcept
	{
		using std::swap;

		swap(other.m_Erroffset, m_Erroffset);
		swap(other.m_Error, m_Error);
		swap(other.m_Option_g, m_Option_g);
		swap(other.m_Option_S, m_Option_S);
		swap(other.m_Option_X, m_Option_X);
		swap(other.m_Option_F, m_Option_F);
		swap(other.m_Opts, m_Opts);
		swap(other.m_tsPattern, m_tsPattern);
		swap(other.m_UsePCRE2, m_UsePCRE2);
		swap(other.m_Match_data, m_Match_data);
		swap(other.m_re, m_re);
		swap(other.m_context, m_context);
	}

	regexOptions(regexOptions&& other) noexcept
		: regexOptions()
	{
		this->swap(other);
	}
	regexOptions& operator =(regexOptions&& other) noexcept
	{
		if (this == &other)	// self assignment check.
			return *this;

		this->swap(other);

		return *this;
	}
#else
	regexOptions(regexOptions&&) noexcept = default;
	regexOptions& operator =(regexOptions&&) noexcept = default;
#endif
	regexOptions(const TString tsPattern)
		: regexOptions()
	{
		if (tsPattern[0] != TEXT('/'))
			m_tsPattern = tsPattern;
		else
			m_tsPattern = tsPattern.gettok(2, tsPattern.numtok(TEXT('/')) - 1, TEXT('/'));

#if DCX_USE_PCRE2
		const TString opts(tsPattern.gettok(tsPattern.numtok(TEXT('/')), TEXT('/')));
		for (UINT i = 0; i < opts.len(); ++i)
		{
			//	g - continue after first match
			//	i - case insensitive match
			//	m - multiple lines match
			//	s - dot matches newlines
			//	x - ignore white spaces

			//	A - anchored - match start of string
			//	E / D - $ dollar matches only at end
			//	U - ungreedy - reverses * and *?
			//	u - enables UTF - 8 and UCP
			//	X - strict escape parsing

			//And the following mIRC - specific modifiers :
			//	S - strip bold, underline, reverse, and color control codes
			//	F - make back - references refer to () capture groups, which is how standard regex works.

			switch (opts[i])
			{
			case TEXT('g'):
				m_Option_g = true;	// not a pcre flag, need a match loop
				break;
			case TEXT('i'):
				m_Opts |= PCRE2_CASELESS;
				break;
			case TEXT('m'):
				m_Opts |= PCRE2_MULTILINE;
				break;
			case TEXT('s'):
				m_Opts |= PCRE2_DOTALL;
				break;
			case TEXT('x'):
				m_Opts |= PCRE2_EXTENDED;
				break;
			case TEXT('A'):
				m_Opts |= PCRE2_ANCHORED;
				break;
			case TEXT('E'):
				m_Opts |= PCRE2_ENDANCHORED;
				break;
			case TEXT('D'):
				m_Opts |= PCRE2_DOLLAR_ENDONLY;
				break;
			case TEXT('U'):
				m_Opts |= PCRE2_UNGREEDY;
				break;
			case TEXT('u'):
				m_Opts |= PCRE2_UTF | PCRE2_UCP;
				break;
			case TEXT('S'):
				m_Option_S = true;	// not a pcre flag
				break;
			case TEXT('F'):
				m_Option_F = true;	// not a pcre flag
				break;
			case TEXT('X'):
				m_Option_X = true;	// set via pcre2_set_bsr()
				break;
			case TEXT('P'):
				m_UsePCRE2 = true;	// not a pcre flag
				break;

			default:
				break;
			}
		}

		if (m_UsePCRE2)
		{
			if (m_context = pcre2_compile_context_create(nullptr); !m_context)
				throw Dcx::dcxException("pcre2: Unable to create context");

			pcre2_set_bsr(m_context, (m_Option_X ? PCRE2_BSR_ANYCRLF : PCRE2_BSR_UNICODE));

			if (m_re = pcre2_compile((PCRE2_SPTR)m_tsPattern.to_chr(), PCRE2_ZERO_TERMINATED, m_Opts, &m_Error, &m_Erroffset, m_context); !m_re)
				throw Dcx::dcxException("pcre2: Unable to compile regex: % pos %", m_Error, m_Erroffset);

			// try for JIT
			pcre2_jit_compile(m_re, 0);

			if (m_Match_data = pcre2_match_data_create_from_pattern(m_re, nullptr); !m_Match_data)
				throw Dcx::dcxException("pcre2: Unable to create match data");
		}
#endif
	}
	~regexOptions()
	{
#if DCX_USE_PCRE2
		if (m_Match_data)
			pcre2_match_data_free(m_Match_data);
		if (m_re)
			pcre2_code_free(m_re);
		if (m_context)
			pcre2_compile_context_free(m_context);
#endif
	}

	regexOptions& operator =(const regexOptions&) = delete;
};

struct dcxSearchData
{
	TString m_tsSearch;
	const DcxSearchTypes m_SearchType{ DcxSearchTypes::SEARCH_W };
#if DCX_USE_PCRE2
	regexOptions m_regexOpts;
#endif

	dcxSearchData() noexcept = default;
	dcxSearchData(const dcxSearchData&) = delete;
	dcxSearchData(dcxSearchData&&) noexcept = default;
	dcxSearchData(const TString& tsSearch, const DcxSearchTypes& SearchType)
		: m_tsSearch(tsSearch)
		, m_SearchType{ SearchType }
	{
#if DCX_USE_PCRE2
		if (m_SearchType == DcxSearchTypes::SEARCH_R)
			m_regexOpts = regexOptions(m_tsSearch);
#endif
	}
	~dcxSearchData() = default;
	dcxSearchData& operator =(const dcxSearchData&) = delete;
	dcxSearchData& operator =(dcxSearchData&&) noexcept = default;
};

struct RegexResults
{
	PCRE2_SIZE nStart{};
	PCRE2_SIZE nEnd{};
};

struct DcxSearchResults
{
	bool m_MatchFound{};
	size_t m_nStart{};
	size_t m_nEnd{};
};

class DcxSearchHelper
{
public:
	static bool isRegexMatch(const TCHAR* matchtext, const TCHAR* pattern) noexcept;
	static bool isRegexMatch(const TCHAR* matchtext, const dcxSearchData& srch_data) noexcept;

	[[nodiscard]] static std::optional<RegexResults> getRegexMatchOffset(const TCHAR* matchtext, const TCHAR* pattern);
	[[nodiscard]] static std::optional<RegexResults> getRegexMatchOffset(const TCHAR* matchtext, const dcxSearchData& srch_data) noexcept;

	static bool matchItemText(const TCHAR* sItemText, const TString& search, const DcxSearchTypes& SearchType);
	static bool matchItemText(const TCHAR* sItemText, const dcxSearchData& srch) noexcept;

	[[nodiscard]] static const DcxSearchTypes StringToSearchType(const TString& tsType) noexcept;
	[[nodiscard]] static const DcxSearchTypes CharToSearchType(const TCHAR& cType) noexcept;
	[[nodiscard]] static const DcxSearchTypes FlagsToSearchType(const XSwitchFlags& xFlags) noexcept;

	[[nodiscard]] static std::optional<DcxSearchResults> matchText(const TString& txt, const UINT nChar, const TString& search, const DcxSearchTypes& SearchType);
	[[nodiscard]] static std::optional<DcxSearchResults> matchText(const TString& txt, const UINT nLine, const UINT nSubChar, const TString& search, const DcxSearchTypes& SearchType);
	[[nodiscard]] static std::optional<DcxSearchResults> matchText(const TString& txt, const UINT nChar, const dcxSearchData& srch_data);

	[[nodiscard]] static TString findTextRange(const TString& tsText, const TString& tsMatchText, const TString& tsParams);
};

