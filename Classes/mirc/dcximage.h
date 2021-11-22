/*!
 * \file dcximage.h
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2006
 */

#ifndef _DCXIMAGE_H_
#define _DCXIMAGE_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

#ifdef DCX_USE_GDIPLUS
#include <thread>
#endif

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxImage final
	: public DcxControl
{
public:
	DcxImage() = delete;
	DcxImage(const DcxImage&) = delete;
	DcxImage& operator =(const DcxImage&) = delete;
	DcxImage(DcxImage&&) = delete;
	DcxImage& operator =(DcxImage&&) = delete;

	DcxImage(const UINT ID, DcxDialog* const p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxImage() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept final;

	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	inline const TString getType() const final { return TEXT("image"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::IMAGE; }

	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml() const final;

	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure

private:
#ifdef DCX_USE_GDIPLUS
	std::unique_ptr<Gdiplus::Image> m_pImage{ nullptr }; //!< GDI+ Image Object
	Gdiplus::CompositingMode m_CMode{ Gdiplus::CompositingModeSourceCopy }; // Compositing Mode
	Gdiplus::CompositingQuality m_CQuality{ Gdiplus::CompositingQualityDefault }; // Compositing Quality
	Gdiplus::InterpolationMode m_IMode{ Gdiplus::InterpolationModeDefault }; // Interpolation Mode
	Gdiplus::SmoothingMode m_SMode{ Gdiplus::SmoothingModeDefault }; // Smoothing Mode

	bool m_bIsAnimated{ false };
	std::atomic_bool m_bRunThread{ false };
	UINT m_FrameCount{};
	UINT m_FrameDelay{};
	UINT m_FrameImage{};
	std::unique_ptr<BYTE[]> m_PropertyItem{ nullptr };
	std::unique_ptr<std::thread> m_AnimThread{ nullptr };

	bool LoadGDIPlusImage(const TString& flags, TString& filename);
	void DrawGDIImage(HDC hdc, int x, int y, int w, int h);
	void SetupAnimThread()
	{
		if (!m_pImage)
			return;

		m_FrameImage = 0;

		const auto count = m_pImage->GetFrameDimensionsCount();
		auto m_DimensionIDs = std::make_unique<GUID[]>(count);
		m_pImage->GetFrameDimensionsList(m_DimensionIDs.get(), count);
		m_FrameCount = m_pImage->GetFrameCount(&m_DimensionIDs[0]);
		const auto sz = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);

		m_PropertyItem = std::make_unique<BYTE[]>(sz);

		m_pImage->GetPropertyItem(PropertyTagFrameDelay, sz, (Gdiplus::PropertyItem*)m_PropertyItem.get());

		m_bIsAnimated = (m_FrameCount > 1);
	}
	void FreeAnimThread()
	{
		StopAnimThread();

		m_PropertyItem.reset(nullptr);
	}
	void StartAnimThread()
	{
		if ((!m_PropertyItem) || (m_AnimThread))
			return;

		m_bRunThread = true;
		// start play thread.
		m_AnimThread = std::make_unique<std::thread>(DcxImage::AnimateThread, this);
	}
	void StopAnimThread()
	{
		m_bRunThread = false;
		if (m_AnimThread)
			m_AnimThread->join();
		m_AnimThread.reset(nullptr);
	}

	long getFrameDelay(UINT nFrame) const noexcept
	{
		if (m_FrameDelay)
			return m_FrameDelay;

		if (m_PropertyItem && nFrame <= m_FrameCount)
			return std::max(((long*)((Gdiplus::PropertyItem*)(m_PropertyItem.get()))->value)[nFrame] * 10, 0L);

		return 0L;
	}
	static void AnimateThread(DcxImage* const img);
#endif
	void DrawBMPImage(HDC hdc, const int x, const int y, const int w, const int h);
	void DrawClientArea(HDC hdc);

	// clears existing image and icon data and sets pointers to null
	void PreloadData(void) noexcept;

	HBITMAP m_hBitmap{ nullptr }; //!< Bitmap
	HICON m_hIcon{ nullptr }; // !< icon

	COLORREF m_clrTransColor{ CLR_INVALID }; //!< Transparent color

	DcxIconSizes m_iIconSize{ DcxIconSizes::SmallIcon };
	bool m_bResizeImage{ true };	//!< Resize Image
	bool m_bTileImage{ false };		//!< Tile Image
	bool m_bBuffer{ false };		//!< Double Buffer Rendering, needed for GDI+ when WS_EX_COMPOSITED
	bool m_bIsIcon{ false };		//!< Is this an icon?
	bool m_bKeepAspect{ false };
	bool m_bVCenterImage{ false };
	bool m_bHCenterImage{ false };

	int m_iXOffset{}, m_iYOffset{};	//!< X & Y image offsets.
	TString m_tsFilename;			//!< The loaded images filename.
};

#endif // _DCXIMAGE_H_
