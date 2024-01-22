/*!
 * \file dcxdirectshow.h
 * \brief blah
 *
 * blah
 *
 * \author Ook
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2006
 */

#ifndef _DCXDIRECTSHOW_H_
#define _DCXDIRECTSHOW_H_

#include "defines.h"

#ifdef DCX_USE_DXSDK
#include "Classes/dcxcontrol.h"

#include <wil/com.h>

class DcxDialog;

namespace
{
	// find the range value of a percentage (0 to -10000 range)
	constexpr inline long PercentageToRange(float perc) noexcept
	{
		return gsl::narrow_cast<long>(-(10000.0 - (100.0 * perc)));
	}

	// find the percentage of 0 to -10000 range
	constexpr inline float RangeToPercentage(long range) noexcept
	{
		return gsl::narrow_cast<float>((gsl::narrow_cast<double>(-(range)) / 10000.0) * 100.0);
	}

	/// <summary>
	/// Normalized range, Min is always zero and Min LE Value LE Max
	/// </summary>
	struct NormalizedRange
	{
		UINT Value{};
		UINT MaxValue{};
	};
}

/*!
 * \brief blah
 *
 * blah
 */

class DcxDirectshow final
	: public DcxControl
{
public:
	DcxDirectshow() = delete;
	DcxDirectshow(const DcxDirectshow&) = delete;
	GSL_SUPPRESS(c.128) DcxDirectshow& operator =(const DcxDirectshow&) = delete;	// No assignments!
	DcxDirectshow(DcxDirectshow&&) = delete;
	GSL_SUPPRESS(c.128) DcxDirectshow& operator =(DcxDirectshow&&) = delete;

	DcxDirectshow(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxDirectshow();

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	inline const TString getType() const final { return TEXT("directshow"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::DIRECTSHOW; }

	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;
	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

protected:
	wil::com_ptr<IVMRWindowlessControl9> InitWindowlessVMR(const HWND hwndApp, const wil::com_ptr<IGraphBuilder>& pGraph);
	HRESULT SetVideoPos(void);
	void ReleaseAll(void) noexcept;

	enum class Properties : UINT { PROP_AUTHOR = 0, PROP_TITLE, PROP_RATING, PROP_DESCRIPTION, PROP_COPYRIGHT, PROP_MOREINFO };

	HRESULT getProperty(const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& prop, const Properties type) const noexcept;
	HRESULT setAlpha(float alpha);
	float getAlpha() const;
	HRESULT setVideo(const TString& flags, const float brightness, const float contrast, const float hue, const float saturation, const float alpha);
	HRESULT getVideo(VMR9ProcAmpControl* amc) const;
	HRESULT getVideoRange(VMR9ProcAmpControlFlags prop, VMR9ProcAmpControlRange* acr) const;
	std::optional<VMR9ProcAmpControlRange> getVideoRange(VMR9ProcAmpControlFlags prop) const;
	UINT64 getPosition(void) const; //!< gets video position within stream
	HRESULT setPosition(const UINT64 pos); //!< sets the video position to x .
	UINT64 getDuration(void) const; //!< gets video duration.
	DWORD CheckSeekCapabilities(DWORD dwCaps) const;
	HRESULT setVolume(const float vol);
	float getVolume(void) const;
	HRESULT setBalance(const long vol);
	long getBalance(void) const;

	void loadFile(const TString& tsFlags, const TString& tsFilename);

	static NormalizedRange NormalizeValue(float fValue, float fMin, float fMax, float fStep) noexcept;

#define WM_GRAPHNOTIFY  WM_APP + 1

private:
	wil::com_ptr<IGraphBuilder> m_pGraph;
	wil::com_ptr<IMediaControl> m_pControl;
	wil::com_ptr<IMediaEventEx> m_pEvent;
	wil::com_ptr<IMediaSeeking> m_pSeek;
	wil::com_ptr<IVMRWindowlessControl9> m_pWc;

	TString					m_tsFilename;

	bool					m_bKeepRatio{ false };
	bool					m_bLoop{ false };
	bool					m_bReserved[2]{ false };
};

#pragma comment(lib, "Strmiids.lib")
#endif // DCX_USE_DXSDK

#endif // _DCXDIRECTSHOW_H_
