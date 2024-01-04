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
	/// Safely release a COM object and set the pointer to nullptr
	/// </summary>
	/// <typeparam name="T">Some COM type.</typeparam>
	/// <param name="ptr">- Pointer to the COM object to release.</param>
	/// <returns></returns>
	//template <DcxConcepts::ImplementsReleaseFunction T>
	//void dcxSafeRelease(__maybenull T** ptr) noexcept
	//{
	//	if (!ptr)
	//		return;
	//
	//	try {
	//		if (*ptr)
	//			(*ptr)->Release();
	//	}
	//	catch (...) {}
	//
	//	*ptr = nullptr;
	//}

	/// <summary>
	/// Normalized range, Min is always zero and Min LE Value LE Max
	/// </summary>
	struct NormalizedRange
	{
		UINT Value{};
		UINT MaxValue{};
	};
}

///// <summary>
///// Base class for our com objects.
///// </summary>
///// <typeparam name="T"></typeparam>
//template <class T>
//struct MyBaseCOMClass
//{
//	using value_type = T*;
//
//	MyBaseCOMClass() noexcept = default;
//	MyBaseCOMClass(const MyBaseCOMClass&) noexcept = delete;
//	MyBaseCOMClass(MyBaseCOMClass&&) noexcept = default;
//	MyBaseCOMClass& operator =(const MyBaseCOMClass&) noexcept = delete;
//	MyBaseCOMClass& operator =(MyBaseCOMClass&&) noexcept = default;
//
//	MyBaseCOMClass(DWORD dwContext, const IID& clsid, const IID& riid) noexcept
//	{
//		try {
//			mHR = CoCreateInstance(clsid, nullptr, dwContext, riid, reinterpret_cast<void**>(&mData));
//		}
//		catch (...)
//		{
//			mHR = 0;
//		}
//	}
//	MyBaseCOMClass(IUnknown* obj, const IID& riid) noexcept
//	{
//		try {
//			if (obj)
//				mHR = obj->QueryInterface(riid, reinterpret_cast<void**>(&mData));
//			else
//				mHR = E_FAIL;
//		}
//		catch (...)
//		{
//			mHR = 0;
//		}
//	}
//	MyBaseCOMClass(IGraphBuilder* obj, const WCHAR* pName) noexcept requires DcxConcepts::IsDerivedFrom<IBaseFilter, T>
//	{
//		try {
//			if (obj && pName)
//				mHR = obj->FindFilterByName(pName, &mData);
//			else
//				mHR = E_FAIL;
//		}
//		catch (...)
//		{
//			mHR = 0;
//		}
//	}
//	virtual ~MyBaseCOMClass() noexcept
//	{
//		release();
//	}
//
//	explicit operator bool() const noexcept
//	{
//		return (SUCCEEDED(mHR) && mData);
//	}
//	value_type operator->() const noexcept
//	{
//		return mData;
//	}
//	value_type* operator&() noexcept
//	{
//		return &mData;
//	}
//	operator value_type() noexcept
//	{
//		return mData;
//	}
//	// NB: reset does NOT release object.
//	void reset() noexcept
//	{
//		mData = nullptr;
//		mHR = 0;
//	}
//	void release() noexcept
//	{
//		dcxSafeRelease<T>(&mData);
//		mHR = 0;
//	}
//	value_type mData{ nullptr };
//	HRESULT mHR{};
//};
//
//template <class T>
//struct MyCOMClass {};
//
//// for VMR-7 Display
//template <>
//struct MyCOMClass<IVMRWindowlessControl> final
//	: MyBaseCOMClass<IVMRWindowlessControl>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IVMRWindowlessControl)
//	{
//	}
//};
//
//// basic audio
//template <>
//struct MyCOMClass<IBasicAudio> final
//	: MyBaseCOMClass<IBasicAudio>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IBasicAudio)
//	{
//	}
//};
//
//template <>
//struct MyCOMClass<IBasicVideo> final
//	: MyBaseCOMClass<IBasicVideo>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IBasicVideo)
//	{
//	}
//};
//
//// for VMR-9 Display
//template <>
//struct MyCOMClass<IVMRMixerControl9> final
//	: MyBaseCOMClass<IVMRMixerControl9>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IVMRMixerControl9)
//	{
//	}
//};
//
//template <>
//struct MyCOMClass<IVMRMixerBitmap9> final
//	: MyBaseCOMClass<IVMRMixerBitmap9>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IVMRMixerBitmap9)
//	{
//	}
//};
//
//template <>
//struct MyCOMClass<IAMMediaContent> final
//	: MyBaseCOMClass<IAMMediaContent>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IAMMediaContent)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IMediaControl> final
//	: MyBaseCOMClass<IMediaControl>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IMediaControl)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IMediaEventEx> final
//	: MyBaseCOMClass<IMediaEventEx>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IMediaEventEx)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IMediaSeeking> final
//	: MyBaseCOMClass<IMediaSeeking>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IMediaSeeking)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IVMRWindowlessControl9> final
//	: MyBaseCOMClass<IVMRWindowlessControl9>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IVMRWindowlessControl9)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IVMRFilterConfig9> final
//	: MyBaseCOMClass<IVMRFilterConfig9>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IVMRFilterConfig9)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IVideoWindow> final
//	: MyBaseCOMClass<IVideoWindow>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IVideoWindow)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IDispatch> final
//	: MyBaseCOMClass<IDispatch>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IDispatch)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IGraphBuilder> final
//	: MyBaseCOMClass<IGraphBuilder>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IGraphBuilder)
//	{
//	}
//	MyCOMClass(DWORD dwContext, const IID& clsid) noexcept
//		: MyBaseCOMClass(dwContext, clsid, IID_IGraphBuilder)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IBaseFilter> final
//	: MyBaseCOMClass<IBaseFilter>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IBaseFilter)
//	{
//	}
//	MyCOMClass(DWORD dwContext, const IID& clsid) noexcept
//		: MyBaseCOMClass(dwContext, clsid, IID_IBaseFilter)
//	{
//	}
//	MyCOMClass(const MyCOMClass<IGraphBuilder>& obj, const WCHAR* pName) noexcept
//		: MyBaseCOMClass(obj.mData, pName)
//	{
//	}
//};
//
//// for EVR display
//template <>
//struct MyCOMClass<IMFVideoDisplayControl> final
//	: MyBaseCOMClass<IMFVideoDisplayControl>
//{
//	MyCOMClass() noexcept = default;
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IMFVideoDisplayControl)
//	{
//	}
//};

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
	//IVMRWindowlessControl9* InitWindowlessVMR(const HWND hwndApp, IGraphBuilder* pGraph);
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
	//IGraphBuilder* m_pGraph{ nullptr };
	//IMediaControl* m_pControl{ nullptr };
	//IMediaEventEx* m_pEvent{ nullptr };
	//IMediaSeeking* m_pSeek{ nullptr };
	//IVMRWindowlessControl9* m_pWc{ nullptr };

	//MyCOMClass<IGraphBuilder> m_pGraph;
	//MyCOMClass<IMediaControl> m_pControl;
	//MyCOMClass<IMediaEventEx> m_pEvent;
	//MyCOMClass<IMediaSeeking> m_pSeek;
	//MyCOMClass<IVMRWindowlessControl9> m_pWc;

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
