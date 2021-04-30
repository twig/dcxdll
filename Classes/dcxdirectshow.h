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
 //#include <dshow.h>
 //#include <initguid.h>
 //#include <qnetwork.h>
 //#include <d3d9.h>
 //#include <vmr9.h>
 //#include <DXGIDebug.h> // win8 only

class DcxDialog;

constexpr long RangeTable[] = {
	-(10000 - (100 * 00)), -(10000 - (100 * 01)), -(10000 - (100 * 02)), -(10000 - (100 * 03)), -(10000 - (100 * 04)), -(10000 - (100 * 05)), -(10000 - (100 * 06)), -(10000 - (100 * 07)), -(10000 - (100 * 8)), -(10000 - (100 * 9)),
	-(10000 - (100 * 10)), -(10000 - (100 * 11)), -(10000 - (100 * 12)), -(10000 - (100 * 13)), -(10000 - (100 * 14)), -(10000 - (100 * 15)), -(10000 - (100 * 16)), -(10000 - (100 * 17)), -(10000 - (100 * 18)), -(10000 - (100 * 19)),
	-(10000 - (100 * 20)), -(10000 - (100 * 21)), -(10000 - (100 * 22)), -(10000 - (100 * 23)), -(10000 - (100 * 24)), -(10000 - (100 * 25)), -(10000 - (100 * 26)), -(10000 - (100 * 27)), -(10000 - (100 * 28)), -(10000 - (100 * 29)),
	-(10000 - (100 * 30)), -(10000 - (100 * 31)), -(10000 - (100 * 32)), -(10000 - (100 * 33)), -(10000 - (100 * 34)), -(10000 - (100 * 35)), -(10000 - (100 * 36)), -(10000 - (100 * 37)), -(10000 - (100 * 38)), -(10000 - (100 * 39)),
	-(10000 - (100 * 40)), -(10000 - (100 * 41)), -(10000 - (100 * 42)), -(10000 - (100 * 43)), -(10000 - (100 * 44)), -(10000 - (100 * 45)), -(10000 - (100 * 46)), -(10000 - (100 * 47)), -(10000 - (100 * 48)), -(10000 - (100 * 49)),
	-(10000 - (100 * 50)), -(10000 - (100 * 51)), -(10000 - (100 * 52)), -(10000 - (100 * 53)), -(10000 - (100 * 54)), -(10000 - (100 * 55)), -(10000 - (100 * 56)), -(10000 - (100 * 57)), -(10000 - (100 * 58)), -(10000 - (100 * 59)),
	-(10000 - (100 * 60)), -(10000 - (100 * 61)), -(10000 - (100 * 62)), -(10000 - (100 * 63)), -(10000 - (100 * 64)), -(10000 - (100 * 65)), -(10000 - (100 * 66)), -(10000 - (100 * 67)), -(10000 - (100 * 68)), -(10000 - (100 * 69)),
	-(10000 - (100 * 70)), -(10000 - (100 * 71)), -(10000 - (100 * 72)), -(10000 - (100 * 73)), -(10000 - (100 * 74)), -(10000 - (100 * 75)), -(10000 - (100 * 76)), -(10000 - (100 * 77)), -(10000 - (100 * 78)), -(10000 - (100 * 79)),
	-(10000 - (100 * 80)), -(10000 - (100 * 81)), -(10000 - (100 * 82)), -(10000 - (100 * 83)), -(10000 - (100 * 84)), -(10000 - (100 * 85)), -(10000 - (100 * 86)), -(10000 - (100 * 87)), -(10000 - (100 * 88)), -(10000 - (100 * 89)),
	-(10000 - (100 * 90)), -(10000 - (100 * 91)), -(10000 - (100 * 92)), -(10000 - (100 * 93)), -(10000 - (100 * 94)), -(10000 - (100 * 95)), -(10000 - (100 * 96)), -(10000 - (100 * 97)), -(10000 - (100 * 98)), -(10000 - (100 * 99)),
	-(10000 - (100 * 100))
};

// find the percentage of 0 to -10000 range
constexpr inline long PercentageToRange(long perc) noexcept
{
	//return (long)-(10000 - (100 * perc));
	if ((perc < 0) || (perc > 100))
		return RangeTable[0];
	return gsl::at(RangeTable, perc);
}

// find the percentage of 0 to -10000 range
constexpr inline long RangeToPercentage(long range) noexcept
{
	return gsl::narrow_cast<long>((-(range) / 10000.0) * 100.0);
}

//template <class T>
//struct MyCOMClass {
//	using value_type = T *;
//	using parent_type = IUnknown *;
//
//	MyCOMClass(parent_type obj, const IID &riid)
//	{
//		mHR = obj->QueryInterface(riid, (void**)& mData);
//	}
//	explicit MyCOMClass(parent_type obj)
//	{
//		if constexpr (std::is_same_v<IBasicAudio, T>)
//			mHR = obj->QueryInterface(IID_IBasicAudio, (void**)& mData);
//		else if constexpr (std::is_same_v<IBasicVideo, T>)
//			mHR = obj->QueryInterface(IID_IBasicVideo, (void**)& mData);
//	}
//	~MyCOMClass() noexcept { if (mData) mData->Release(); };
//
//	operator bool() const noexcept
//	{
//		return (SUCCEEDED(mHR) && mData);
//	}
//	value_type mData{ nullptr };
//	HRESULT mHR{};
//};

template <class T>
struct MyBaseCOMClass {
	using value_type = T*;

	MyBaseCOMClass() noexcept = default;
	MyBaseCOMClass(const MyBaseCOMClass&) noexcept = delete;
	MyBaseCOMClass(MyBaseCOMClass&&) noexcept = default;
	MyBaseCOMClass& operator =(const MyBaseCOMClass&) noexcept = delete;
	MyBaseCOMClass& operator =(MyBaseCOMClass&&) noexcept = default;

	MyBaseCOMClass(IUnknown* obj, const IID& riid) noexcept
	{
		try {
			mHR = obj->QueryInterface(riid, reinterpret_cast<void**>(&mData));
		}
		catch (...)
		{
			mHR = NULL;
		}
	}
	MyBaseCOMClass(IGraphBuilder* obj, const WCHAR* pName) noexcept
	{
		try {
			mHR = obj->FindFilterByName(pName, &mData);
		}
		catch (...)
		{
			mHR = NULL;
		}
	}
	virtual ~MyBaseCOMClass() noexcept
	{
		try {
			if (mData)
				mData->Release();
		}
		catch (...)
		{
		}
		mData = nullptr;
	}

	explicit operator bool() const noexcept
	{
		return (SUCCEEDED(mHR) && mData);
	}
	value_type operator->() const noexcept
	{
		return mData;
	}
	// NB: reset does NOT release object.
	void reset() noexcept
	{
		mData = nullptr;
		mHR = 0;
	}
	value_type mData{ nullptr };
	HRESULT mHR{};
};

//template <class T, class IValue>
//struct MyCOMMClass final
//	: MyBaseCOMClass<T>
//{
//	explicit MyCOMMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IValue)
//	{
//	}
//};

template <class T>
struct MyCOMClass {};

template <>
struct MyCOMClass<IBasicAudio> final
	: MyBaseCOMClass<IBasicAudio>
{
	explicit MyCOMClass(IUnknown* obj) noexcept
		: MyBaseCOMClass(obj, IID_IBasicAudio)
	{
	}
};

template <>
struct MyCOMClass<IBasicVideo> final
	: MyBaseCOMClass<IBasicVideo>
{
	explicit MyCOMClass(IUnknown* obj) noexcept
		: MyBaseCOMClass(obj, IID_IBasicVideo)
	{
	}
};

template <>
struct MyCOMClass<IVMRMixerControl9> final
	: MyBaseCOMClass<IVMRMixerControl9>
{
	explicit MyCOMClass(IUnknown* obj) noexcept
		: MyBaseCOMClass(obj, IID_IVMRMixerControl9)
	{
	}
};

template <>
struct MyCOMClass<IVMRMixerBitmap9> final
	: MyBaseCOMClass<IVMRMixerBitmap9>
{
	explicit MyCOMClass(IUnknown* obj) noexcept
		: MyBaseCOMClass(obj, IID_IVMRMixerBitmap9)
	{
	}
};

template <>
struct MyCOMClass<IAMMediaContent> final
	: MyBaseCOMClass<IAMMediaContent>
{
	explicit MyCOMClass(IUnknown* obj) noexcept
		: MyBaseCOMClass(obj, IID_IAMMediaContent)
	{
	}
};
template <>
struct MyCOMClass<IVMRWindowlessControl9> final
	: MyBaseCOMClass<IVMRWindowlessControl9>
{
	explicit MyCOMClass(IUnknown* obj) noexcept
		: MyBaseCOMClass(obj, IID_IVMRWindowlessControl9)
	{
	}
};
template <>
struct MyCOMClass<IVMRFilterConfig9> final
	: MyBaseCOMClass<IVMRFilterConfig9>
{
	explicit MyCOMClass(IUnknown* obj) noexcept
		: MyBaseCOMClass(obj, IID_IVMRFilterConfig9)
	{
	}
};
template <>
struct MyCOMClass<IVideoWindow> final
	: MyBaseCOMClass<IVideoWindow>
{
	explicit MyCOMClass(IUnknown* obj) noexcept
		: MyBaseCOMClass(obj, IID_IVideoWindow)
	{
	}
};
template <>
struct MyCOMClass<IDispatch> final
	: MyBaseCOMClass<IDispatch>
{
	explicit MyCOMClass(IUnknown* obj) noexcept
		: MyBaseCOMClass(obj, IID_IDispatch)
	{
	}
};
//template <>
//struct MyCOMClass<IHTMLDocument2> final
//	: MyBaseCOMClass<IHTMLDocument2>
//{
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IHTMLDocument2)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IWebBrowser2> final
//	: MyBaseCOMClass<IWebBrowser2>
//{
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IWebBrowser2)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IOleInPlaceObject> final
//	: MyBaseCOMClass<IOleInPlaceObject>
//{
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IOleInPlaceObject)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IOleObject> final
//	: MyBaseCOMClass<IOleObject>
//{
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IOleObject)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IConnectionPointContainer> final
//	: MyBaseCOMClass<IConnectionPointContainer>
//{
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IConnectionPointContainer)
//	{
//	}
//};
//template <>
//struct MyCOMClass<IConnectionPoint> final
//	: MyBaseCOMClass<IConnectionPoint>
//{
//	explicit MyCOMClass(IUnknown* obj) noexcept
//		: MyBaseCOMClass(obj, IID_IConnectionPoint)
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
	DcxDirectshow& operator =(const DcxDirectshow&) = delete;	// No assignments!
	DcxDirectshow(DcxDirectshow&&) = delete;
	DcxDirectshow& operator =(DcxDirectshow&&) = delete;

	DcxDirectshow(const UINT ID, DcxDialog* const p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
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
	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

protected:
	IVMRWindowlessControl9* InitWindowlessVMR(const HWND hwndApp, IGraphBuilder* pGraph);
	HRESULT SetVideoPos(void);
	void ReleaseAll(void) noexcept;

	enum Properties : UINT { PROP_AUTHOR = 0, PROP_TITLE, PROP_RATING, PROP_DESCRIPTION };

	HRESULT getProperty(const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& prop, const int type) const noexcept;
	HRESULT setAlpha(float alpha);
	HRESULT setVideo(const TString& flags, const float brightness, const float contrast, const float hue, const float saturation);
	HRESULT getVideo(VMR9ProcAmpControl* amc) const;
	HRESULT getVideoRange(VMR9ProcAmpControlFlags prop, VMR9ProcAmpControlRange* acr) const;
	std::optional<VMR9ProcAmpControlRange> getVideoRange(VMR9ProcAmpControlFlags prop) const;
	UINT64 getPosition(void) const; //!< gets video position within stream
	HRESULT setPosition(const UINT64 pos); //!< sets the video position to x .
	UINT64 getDuration(void) const; //!< gets video duration.
	DWORD CheckSeekCapabilities(DWORD dwCaps) const;
	HRESULT setVolume(const long vol);
	long getVolume(void) const;
	HRESULT setBalance(const long vol);
	long getBalance(void) const;

#define WM_GRAPHNOTIFY  WM_APP + 1

private:
	IGraphBuilder* m_pGraph{ nullptr };
	IMediaControl* m_pControl{ nullptr };
	IMediaEventEx* m_pEvent{ nullptr };
	IMediaSeeking* m_pSeek{ nullptr };
	IVMRWindowlessControl9* m_pWc{ nullptr };

	TString					m_tsFilename;

	bool					m_bKeepRatio{ false };
	bool					m_bLoop{ false };
	bool					m_bReserved[2]{ false };
};

#pragma comment(lib, "Strmiids.lib")
#endif // DCX_USE_DXSDK

#endif // _DCXDIRECTSHOW_H_
