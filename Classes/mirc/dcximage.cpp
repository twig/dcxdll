/*!
 * \file dcximage.cpp
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * � ScriptsDB.org - 2006
 */
#include "defines.h"
#include "Classes/mirc/dcximage.h"
#include "Classes/dcxdialog.h"
#include "Dcx.h"



 /*!
  * \brief Constructor
  *
  * \param ID Control ID
  * \param p_Dialog Parent DcxDialog Object
  * \param mParentHwnd Parent Window Handle
  * \param rc Window Rectangle
  * \param styles Window Style Tokenized List
  */
DcxImage::DcxImage(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog, DcxControlTypes::IMAGE)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles,
		DCX_IMAGECLASS,
		ws.m_Styles | WindowStyle::Child,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsValidWindow())
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		DcxUXModule::dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	//if (p_Dialog->getToolTip())
	{
		if (styles.istok(TEXT("tooltips")))
		{
			setToolTipHWND(p_Dialog->getToolTipHWND());
			if (!IsWindow(this->getToolTipHWND()))
				throw Dcx::dcxException("Unable to get ToolTips window");

			AddToolTipToolInfo(this->getToolTipHWND(), m_Hwnd);
		}
	}

#ifdef DCX_USE_GDIPLUS
	if (p_Dialog->isExStyle(WindowExStyle::Composited))
		this->m_bBuffer = true;
#endif

}

/*!
 * \brief blah
 *
 * blah
 */

DcxImage::~DcxImage() noexcept
{
	PreloadData();
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxImage::parseControlStyles(const TString& tsStyles)
{
	//auto ws = parseGeneralControlStyles(tsStyles);
	//ws.m_Styles |= SS_NOTIFY;
	//return ws;

	dcxWindowStyles ws;

	ws.m_Styles |= SS_NOTIFY;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"hcenter"_hash:
			m_bHCenterImage = true;
			break;
		case L"vcenter"_hash:
			m_bVCenterImage = true;
			break;
		default:
			break;
		}
	}

	return parseGeneralControlStyles(tsStyles, ws);
}

void DcxImage::loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc)
{
	TString filename(tsSrc);

	PreloadData();

	this->m_iIconIndex = tsIndex.to_int();
	this->m_tsFilename = filename;
	this->m_bIsIcon = true;
	this->m_tsLoadFlags = tsFlags;

	this->m_hIcon = dcxLoadIcon(this->m_iIconIndex, filename, (m_iIconSize != DcxIconSizes::SmallIcon), tsFlags);

	// resize window to size of icon
	RECT wnd{};

	if (!GetWindowRectParent(m_Hwnd, &wnd))
		throw Dcx::dcxException("Unable to get windows rect");

	MoveWindow(m_Hwnd, wnd.left, wnd.top, gsl::narrow_cast<int>(this->m_iIconSize), gsl::narrow_cast<int>(this->m_iIconSize), TRUE);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxImage::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
	case L"fname"_hash:
		szReturnValue = m_tsFilename.to_chr();
		break;
#ifdef DCX_USE_GDIPLUS
	case L"frames"_hash:
		_ts_snprintf(szReturnValue, TEXT("%u"), m_FrameCount);
		break;
	case L"isanimated"_hash:
		szReturnValue = dcx_truefalse(m_bIsAnimated);
		break;
		// $xdid(dname,id,frame).delay
	case L"delay"_hash:
	{
		const UINT nFrame = input.getnexttok().to_<UINT>();
		const long nDelay = getFrameDelay((nFrame == 0 ? m_FrameImage : nFrame));
		_ts_snprintf(szReturnValue, TEXT("%d"), nDelay);
	}
	break;
#else
	case L"frames"_hash:
		szReturnValue = TEXT("0");
		break;
	case L"isanimated"_hash:
		szReturnValue = TEXT("$false");
		break;
		// $xdid(dname,id,frame).delay
	case L"delay"_hash:
	{
		szReturnValue = TEXT("0");
	}
	break;
#endif
	default:
		parseGlobalInfoRequest(input, szReturnValue);
		break;
	}

	//// [NAME] [ID] [PROP]
	//if (const auto prop(input.getfirsttok(3)); prop == TEXT("fname"))
	//	szReturnValue = m_tsFilename.to_chr();
	//else
	//	parseGlobalInfoRequest(input, szReturnValue);
}

// clears existing image and icon data and sets pointers to null
void DcxImage::PreloadData() noexcept
{
	if (this->m_hBitmap)
	{
		DeleteObject(this->m_hBitmap);
		this->m_hBitmap = nullptr;
	}

	if (this->m_hIcon)
	{
		DestroyIcon(this->m_hIcon);
		this->m_hIcon = nullptr;
	}

#ifdef DCX_USE_GDIPLUS
	FreeAnimThread();

	m_FrameImage = 0;

	this->m_pImage.reset(nullptr);

#endif
	this->m_tsFilename.clear();	// = TEXT("");
}

#ifdef DCX_USE_GDIPLUS
bool DcxImage::LoadGDIPlusImage(const TString& flags, TString& filename)
{
	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("Unable to Access File: %"), filename);

	this->m_pImage = std::make_unique<Gdiplus::Image>(filename.to_chr(), TRUE);

	// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
	if (const auto status = this->m_pImage->GetLastStatus(); status != Gdiplus::Status::Ok)
	{
		PreloadData();
		throw Dcx::dcxException(TEXT("Failed to load image: %"), GetLastStatusStr(status));
	}

	const XSwitchFlags xflags(flags);

	if (xflags[TEXT('h')])
	{ // High Quality
		this->m_CQuality = Gdiplus::CompositingQualityHighQuality;
		this->m_IMode = Gdiplus::InterpolationModeHighQualityBicubic;
	}
	else
	{
		this->m_CQuality = Gdiplus::CompositingQualityDefault;
		this->m_IMode = Gdiplus::InterpolationModeDefault;
	}

	if (xflags[TEXT('b')]) // Blend Image
		this->m_CMode = Gdiplus::CompositingModeSourceOver;
	else
		this->m_CMode = Gdiplus::CompositingModeSourceCopy;

	if (xflags[TEXT('a')]) // Anti-Aliased
		this->m_SMode = Gdiplus::SmoothingModeAntiAlias;
	else
		this->m_SMode = Gdiplus::SmoothingModeDefault;

	this->m_bTileImage = xflags[TEXT('t')]; // Tile
	const bool bNoAnimation = xflags[TEXT('A')]; // No Animation even if image format supports it.
	this->m_bKeepAspect = xflags[TEXT('k')]; // keep aspect

	if (!bNoAnimation)
	{
		SetupAnimThread();

		if (m_bIsAnimated)
			StartAnimThread();
	}

	return true;
}
#endif

/*!
 * \brief blah
 *
 * blah
 */

void DcxImage::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [SIZE] [FILENAME]
	if (flags[TEXT('w')] && numtok > 6)
	{
		//const auto flag(input.getnexttok());		// tok 4
		//const auto index = input.getnexttok().to_int();	// tok 5
		//const auto size = input.getnexttok().to_int();	// tok 6
		//auto filename(input.getlasttoks().trim());	// tok 7, -1
		//
		//PreloadData();
		//
		//this->m_iIconSize = NumToIconSize(size);
		//
		//this->m_hIcon = dcxLoadIcon(index, filename, (m_iIconSize != DcxIconSizes::SmallIcon), flag);
		//
		//this->m_tsFilename = filename;
		//
		//this->m_bIsIcon = true;
		//
		//this->m_iIconIndex = index;
		//
		//this->m_tsLoadFlags = flag;
		//
		//// resize window to size of icon
		//RECT wnd{};
		//
		//if (!GetWindowRectParent(m_Hwnd, &wnd))
		//	throw Dcx::dcxException("Unable to get windows rect");
		//
		//MoveWindow(m_Hwnd, wnd.left, wnd.top, gsl::narrow_cast<int>(this->m_iIconSize), gsl::narrow_cast<int>(this->m_iIconSize), TRUE);

		const auto flag(input.getnexttok());			// tok 4
		const auto tsIndex(input.getnexttok());			// tok 5
		const auto size = input.getnexttok().to_int();	// tok 6
		auto filename(input.getlasttoks().trim());		// tok 7, -1

		this->m_iIconSize = NumToIconSize(size);

		this->loadIcon(flag, tsIndex, filename);

		this->redrawWindow();
	}
	//xdid -i [NAME] [ID] [SWITCH] [+FLAGS] [IMAGE]
	else if (flags[TEXT('i')] && numtok > 4)
	{
		const auto flag(input.getnexttok().trim());	// tok 4
		auto filename(input.getlasttoks().trim());	// tok 5, -1

		PreloadData();

		if (flag[0] != TEXT('+'))
			throw DcxExceptions::dcxInvalidFlag();

		// if name is `none` then just clear image & display nothing.
		if (filename == TEXT("none"))
		{
			InvalidateRect(m_Hwnd, nullptr, TRUE);
			return;
		}

#ifdef DCX_USE_GDIPLUS
		// using this method allows you to render BMP, ICON, GIF, JPEG, Exif, PNG, TIFF, WMF, and EMF (only gif animation supported atm)
		//if (Dcx::GDIModule.isUseable() && flag.find(TEXT('g'),0)) // makes GDI+ the default method, bitmap is only used when GDI+ isn't supported.
		if (Dcx::GDIModule.isUseable())
		{
			if (!LoadGDIPlusImage(flag, filename))
				throw Dcx::dcxException("Unable to load Image with GDI+"); // <- should never throw this
		}
		else
			this->m_hBitmap = dcxLoadBitmap(this->m_hBitmap, filename);

		if ((!this->m_hBitmap) && (!this->m_pImage))
			throw Dcx::dcxException("Failed to load image");

		this->m_tsFilename = filename;
		this->m_tsLoadFlags = flag;

#else
		this->m_hBitmap = dcxLoadBitmap(this->m_hBitmap, filename);
		if (this->m_hBitmap)
			this->m_tsFilename = filename;
#endif
		this->m_bIsIcon = false;
		//InvalidateParentRect(m_Hwnd);
		InvalidateRect(m_Hwnd, nullptr, TRUE);
	}
	// xdid -k [NAME] [ID] [SWITCH] [COLOR]
	else if (flags[TEXT('k')] && numtok > 3)
	{
		this->m_clrTransColor = input.getnexttok().to_<COLORREF>();	// tok 4
		this->redrawWindow();
	}
	// xdid -o [NAME] [ID] [SWITCH] [XOFFSET] [YOFFSET]
	else if (flags[TEXT('o')] && numtok > 4)
	{
		this->m_iXOffset = input++.to_int();	// tok 4
		this->m_iYOffset = input++.to_int();	// tok 5

		this->redrawWindow();
	}
	// xdid -S [NAME] [ID] [SWITCH] [1|0]
	else if (flags[TEXT('S')] && numtok > 3)
	{
		m_bResizeImage = (input.getnexttok().to_int() > 0);	// tok 4

		InvalidateRect(m_Hwnd, nullptr, TRUE);
		//UpdateWindow(m_Hwnd);
		//this->redrawWindow();
	}
	// xdid -F [NAME] [ID] [SWITCH] [+FLAGS] [ARGS]
	else if (flags[TEXT('F')] && numtok > 3)
	{
		const auto flag(input.getnexttok().trim());	// tok 4
		const XSwitchFlags xflags(flag);

		if (xflags[TEXT('a')])
		{
			// enable/disable animation.

			if (const bool bStart = (input.getnexttok().to_int() > 0); bStart)
			{
				if (m_bIsAnimated)
					StartAnimThread();
			}
			else
				StopAnimThread();

			this->redrawWindow();
		}
		else if (xflags[TEXT('f')])
		{
			// sets active frame
			const auto nFrame = input.getnexttok().to_int();
			if ((nFrame < 0) || (gsl::narrow_cast<UINT>(nFrame) > m_FrameCount))
				throw DcxExceptions::dcxInvalidArguments();

			if (m_pImage)
			{
				StopAnimThread();

				const GUID pageGuid = Gdiplus::FrameDimensionTime;

				m_pImage->SelectActiveFrame(&pageGuid, gsl::narrow_cast<UINT>(nFrame));
				InvalidateRect(m_Hwnd, nullptr, TRUE);
			}
			this->redrawWindow();
		}
		else if (xflags[TEXT('d')])
		{
			// manually sets a delay between frames for broken gifs etc...
			const auto nFrame = input.getnexttok().to_int();
			if (nFrame < 0)
				throw DcxExceptions::dcxInvalidArguments();

			m_FrameDelay = gsl::narrow_cast<UINT>(nFrame);
		}
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

#ifdef DCX_USE_GDIPLUS
void DcxImage::DrawGDIImage(HDC hdc, const int x, const int y, const int w, const int h)
{
	if (!hdc || !this->m_pImage)
		return;

	// NB: These values can be zero, this leads to a divide by zero below
	const auto iw = this->m_pImage->GetWidth();
	const auto ih = this->m_pImage->GetHeight();
	if ((iw == 0) || (ih == 0))	// no pixels to be drawn, just return.
		return;

	Gdiplus::Graphics grphx(hdc);

	grphx.SetCompositingQuality(this->m_CQuality);
	grphx.SetCompositingMode(this->m_CMode);
	grphx.SetSmoothingMode(this->m_SMode);

	//DcxControl::DrawCtrlBackground(grphx.GetHDC(), this);

	//grphx.Clear(Gdiplus::Color(m_clrBackground));

	if (((iw == 1) || (ih == 1)) && this->m_bResizeImage)
	{
		// This fixes a GDI+ bug when resizing 1 px images
		// http://www.devnewsgroups.net/group/microsoft.public.dotnet.framework.windowsforms/topic11515.aspx
		grphx.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
		grphx.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
	}
	else
		grphx.SetInterpolationMode(this->m_IMode);

	if (this->m_bTileImage)
	{
		Gdiplus::ImageAttributes imAtt;
		imAtt.SetWrapMode(Gdiplus::WrapModeTile);

		grphx.DrawImage(this->m_pImage.get(),
			Gdiplus::Rect(x + this->m_iXOffset, y + this->m_iYOffset, w, h),  // dest rect
			0, 0, w, h,       // source rect
			Gdiplus::UnitPixel,
			&imAtt);
	}
	else {
		if (this->m_bResizeImage)
		{
			if (m_bKeepAspect)
			{
				// This code calculates the aspect ratio in which I have to draw the image
				const float percentWidth = gsl::narrow_cast<float>(w) / gsl::narrow_cast<float>(iw);
				const float percentHeight = gsl::narrow_cast<float>(h) / gsl::narrow_cast<float>(ih);

				const float percent = percentHeight < percentWidth ? percentHeight : percentWidth;

				const int newImageWidth = gsl::narrow_cast<int>(iw * percent);
				const int newImageHeight = gsl::narrow_cast<int>(ih * percent);

				//grphx.DrawImage(this->m_pImage.get(), this->m_iXOffset, this->m_iYOffset, newImageWidth, newImageHeight);

				int newX{ x + m_iXOffset };
				int newY{ y + m_iYOffset };
				if (m_bVCenterImage)
				{
					// vertically center it.
					const int diff = (h - newImageHeight) / 2;
					newY += diff;
				}
				if (m_bHCenterImage)
				{
					// Horizontally center it.
					const int diff = (w - newImageWidth) / 2;
					newX += diff;
				}
				grphx.DrawImage(this->m_pImage.get(), newX, newY, newImageWidth, newImageHeight);
			}
			else
				grphx.DrawImage(this->m_pImage.get(), this->m_iXOffset, this->m_iYOffset, w, h);
		}
		else
			grphx.DrawImage(this->m_pImage.get(), this->m_iXOffset, this->m_iYOffset);
	}
}

void DcxImage::AnimateThread(DcxImage* const img)
{
	if (!img)
		return;

	const GUID pageGuid = Gdiplus::FrameDimensionTime;
	UINT m_nFramePosition{ img->m_FrameImage };

	for (; img->m_bRunThread;)
	{
		if (!img->m_pImage || !img->m_PropertyItem)
			return;

		//if (HDC hDC = GetDC(img->m_Hwnd); hDC)
		//{
		//	Auto(ReleaseDC(img->m_Hwnd, hDC));
		//	img->DrawClientArea(hDC);
		//}

		img->DrawClientArea(wil::GetDC(img->m_Hwnd).get());

		img->m_pImage->SelectActiveFrame(&pageGuid, m_nFramePosition++);

		if (m_nFramePosition == img->m_FrameCount)
			m_nFramePosition = 0;

		const long lPause = img->getFrameDelay(m_nFramePosition);

		const std::chrono::milliseconds tm(lPause);

		if (img->m_bRunThread)
			std::this_thread::sleep_for(tm);
	}
	img->m_FrameImage = m_nFramePosition;
}
#endif

void DcxImage::DrawBMPImage(HDC hdc, const int x, const int y, const int w, const int h)
{
	if (!hdc)
		return;

#if DCX_USE_WRAPPERS
	Dcx::dcxHDCBitmapResource hdcbmp(hdc, m_hBitmap);

	if (auto [code, bmp] = Dcx::dcxGetObject<BITMAP>(m_hBitmap); code != 0)
	{
		int newX{ x + m_iXOffset };
		int newY{ y + m_iYOffset };

		if (m_bKeepAspect)
		{
			// This code calculates the aspect ratio in which I have to draw the image
			const float percentWidth = gsl::narrow_cast<float>(w) / gsl::narrow_cast<float>(bmp.bmWidth);
			const float percentHeight = gsl::narrow_cast<float>(h) / gsl::narrow_cast<float>(bmp.bmHeight);

			const float percent = percentHeight < percentWidth ? percentHeight : percentWidth;

			const int newImageWidth = gsl::narrow_cast<int>(bmp.bmWidth * percent);
			const int newImageHeight = gsl::narrow_cast<int>(bmp.bmHeight * percent);

			//if (m_clrTransColor != CLR_INVALID)
			//	TransparentBlt(hdc, x, y, w, h, hdcbmp.get(), 0, 0, newImageWidth, newImageHeight, m_clrTransColor);
			//else
			//	StretchBlt(hdc, x, y, w, h, hdcbmp.get(), 0, 0, newImageWidth, newImageHeight, SRCCOPY);

			if (m_bVCenterImage)
			{
				// vertically center it.
				const int diff = (h - newImageHeight) / 2;
				newY += diff;
			}
			if (m_bHCenterImage)
			{
				// Horizontally center it.
				const int diff = (w - newImageWidth) / 2;
				newX += diff;
			}
			if (m_clrTransColor != CLR_INVALID)
				TransparentBlt(hdc, newX, newY, w, h, hdcbmp.get(), 0, 0, newImageWidth, newImageHeight, m_clrTransColor);
			else
				StretchBlt(hdc, newX, newY, w, h, hdcbmp.get(), 0, 0, newImageWidth, newImageHeight, SRCCOPY);
		}
		else {
			if (m_clrTransColor != CLR_INVALID)
				TransparentBlt(hdc, newX, newY, w, h, hdcbmp.get(), 0, 0, bmp.bmWidth, bmp.bmHeight, m_clrTransColor);
			else
				StretchBlt(hdc, newX, newY, w, h, hdcbmp.get(), 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		}
	}
#else
	auto hdcbmp = CreateCompatibleDC(hdc);

	if (!hdcbmp)
		return;

	Auto(DeleteDC(hdcbmp));

	if (auto [code, bmp] = Dcx::dcxGetObject<BITMAP>(m_hBitmap); code != 0)
	{
		auto oldBitmap = Dcx::dcxSelectObject<HBITMAP>(hdcbmp, m_hBitmap);

		Auto(Dcx::dcxSelectObject<HBITMAP>(hdcbmp, oldBitmap));

		if (m_clrTransColor != CLR_INVALID)
			TransparentBlt(hdc, x, y, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, m_clrTransColor);
		else
			StretchBlt(hdc, x, y, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	}
	if (auto [code, bmp] = Dcx::dcxGetObject<BITMAP>(m_hBitmap); code != 0)
	{
		auto oldBitmap = Dcx::dcxSelectObject<HBITMAP>(hdcbmp, m_hBitmap);

		Auto(Dcx::dcxSelectObject<HBITMAP>(hdcbmp, oldBitmap));

		int newX{ x + m_iXOffset };
		int newY{ y + m_iYOffset };

		if (m_bKeepAspect)
		{
			// This code calculates the aspect ratio in which I have to draw the image
			const float percentWidth = gsl::narrow_cast<float>(w) / gsl::narrow_cast<float>(bmp.bmWidth);
			const float percentHeight = gsl::narrow_cast<float>(h) / gsl::narrow_cast<float>(bmp.bmHeight);

			const float percent = percentHeight < percentWidth ? percentHeight : percentWidth;

			const int newImageWidth = gsl::narrow_cast<int>(bmp.bmWidth * percent);
			const int newImageHeight = gsl::narrow_cast<int>(bmp.bmHeight * percent);

			if (m_bVCenterImage)
			{
				// vertically center it.
				const int diff = (h - newImageHeight) / 2;
				newY += diff;
			}
			if (m_bHCenterImage)
			{
				// Horizontally center it.
				const int diff = (w - newImageWidth) / 2;
				newX += diff;
			}
			if (m_clrTransColor != CLR_INVALID)
				TransparentBlt(hdc, newX, newY, w, h, hdcbmp, 0, 0, newImageWidth, newImageHeight, m_clrTransColor);
			else
				StretchBlt(hdc, newX, newY, w, h, hdcbmp, 0, 0, newImageWidth, newImageHeight, SRCCOPY);
		}
		else {
			if (m_clrTransColor != CLR_INVALID)
				TransparentBlt(hdc, newX, newY, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, m_clrTransColor);
			else
				StretchBlt(hdc, newX, newY, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		}
	}
#endif
}

const TString DcxImage::getStyles(void) const
{
	auto styles(__super::getStyles());

	if (m_bHCenterImage)
		styles.addtok(TEXT("hcenter"));
	if (m_bVCenterImage)
		styles.addtok(TEXT("vcenter"));
	return styles;
}

void DcxImage::toXml(TiXmlElement* const xml) const
{
	if (!xml)
		return;

	__super::toXml(xml);

	if (!this->m_tsFilename.empty())
		xml->SetAttribute("src", m_tsFilename.c_str());
	if (!this->m_tsLoadFlags.empty())
		xml->SetAttribute("flags", m_tsLoadFlags.c_str());
	if (this->m_clrTransColor != CLR_INVALID)
		setColourAttribute(xml, "transparentcolour", this->m_clrTransColor);
	if (this->m_bIsIcon)
	{
		xml->SetAttribute("icon", "1");
		xml->SetAttribute("index", m_iIconIndex);
		xml->SetAttribute("iconsize", gsl::narrow_cast<int>(this->m_iIconSize));
	}
	if (this->m_bIsAnimated)
		xml->SetAttribute("anim", "1");
	if (this->m_bResizeImage)
		xml->SetAttribute("resize", "1");
	if (this->m_bTileImage)
		xml->SetAttribute("tile", "1");
	if (this->m_bKeepAspect)
		xml->SetAttribute("keepaspect", "1");
	if (this->m_iXOffset > 0)
		xml->SetAttribute("xoffset", this->m_iXOffset);
	if (this->m_iYOffset > 0)
		xml->SetAttribute("yoffset", this->m_iYOffset);
}

TiXmlElement* DcxImage::toXml() const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

void DcxImage::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis)
{
	if (!xDcxml || !xThis || !m_Hwnd)
		return;

	PreloadData();

	__super::fromXml(xDcxml, xThis);

	this->m_clrTransColor = queryColourAttribute(xThis, "transparentcolour");

	this->m_iIconSize = NumToIconSize(queryIntAttribute(xThis, "iconsize"));
	this->m_iIconIndex = queryIntAttribute(xThis, "index");
	this->m_tsLoadFlags = queryAttribute(xThis, "flags");

	this->m_bIsIcon = (queryIntAttribute(xThis, "icon") > 0);
	this->m_bIsAnimated = (queryIntAttribute(xThis, "anim") > 0);
	this->m_bResizeImage = (queryIntAttribute(xThis, "resize") > 0);
	this->m_bTileImage = (queryIntAttribute(xThis, "tile") > 0);
	this->m_bKeepAspect = (queryIntAttribute(xThis, "keepaspect") > 0);
	this->m_iXOffset = queryIntAttribute(xThis, "xoffset");
	this->m_iYOffset = queryIntAttribute(xThis, "yoffset");

	// set filename last
	if (const auto tmp = queryAttribute(xThis, "src"); tmp)
	{
		this->m_tsFilename = tmp;

		if (this->m_bIsIcon)
		{
			// load icon
			this->m_hIcon = dcxLoadIcon(this->m_iIconIndex, this->m_tsFilename, (this->m_iIconSize != DcxIconSizes::SmallIcon), this->m_tsLoadFlags);
		}
		else {
			// load image
			if (Dcx::GDIModule.isUseable())
			{
				if (!LoadGDIPlusImage(this->m_tsLoadFlags, this->m_tsFilename))
					throw Dcx::dcxException("Unable to load Image with GDI+"); // <- should never throw this
			}
			else
				this->m_hBitmap = dcxLoadBitmap(this->m_hBitmap, this->m_tsFilename);
		}
	}

}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxImage::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) noexcept
{
	return 0L;
}

LRESULT DcxImage::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_ERASEBKGND:
	{
		//<#IF USER == 'hkr' COMMENT OUT, UNLESS ITS THE TIME WHEN HE WANTS THIS>
		if (this->isExStyle(WindowExStyle::Transparent))
			this->DrawParentsBackground(reinterpret_cast<HDC>(wParam));
		else
			DcxControl::DrawCtrlBackground(reinterpret_cast<HDC>(wParam), this);

		//if (this->isExStyle(WS_EX_TRANSPARENT))
		//	this->DrawParentsBackground(reinterpret_cast<HDC>(wParam));
		//else
		//	DcxControl::DrawCtrlBackground(reinterpret_cast<HDC>(wParam),this);
		//<END>

		bParsed = TRUE;
		return TRUE;
	}
	break;

	case WM_PRINTCLIENT:
	{
		this->DrawClientArea(reinterpret_cast<HDC>(wParam));
		bParsed = TRUE;
	}
	break;

	case WM_PAINT:
	{
		bParsed = TRUE;
		if (!wParam)
		{
			//PAINTSTRUCT ps{};
			//auto hdc = BeginPaint(m_Hwnd, &ps);
			//Auto(EndPaint(m_Hwnd, &ps));
			//this->DrawClientArea(hdc);

			this->DrawClientArea(wil::BeginPaint(m_Hwnd).get());
		}
		else
			this->DrawClientArea(reinterpret_cast<HDC>(wParam));
	}
	break;

	case WM_SIZE:
	{
		InvalidateRect(m_Hwnd, nullptr, TRUE);
	}
	break;

	case WM_DESTROY:
	{
		this->CallDefaultClassProc(uMsg, wParam, lParam);

		delete this;
		bParsed = TRUE;
	}
	break;

	default:
		return this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}

void DcxImage::DrawClientArea(HDC hdc)
{
	RECT rect{};
	// default paint method
	if (!GetClientRect(m_Hwnd, &rect))
		return;

	const auto w = (rect.right - rect.left), h = (rect.bottom - rect.top), x = rect.left, y = rect.top;

	// Setup alpha blend if any.
	const bool bAnimAlpha = (m_bIsAnimated && m_bAlphaBlend);
	if (bAnimAlpha)
		m_bAlphaBlend = false;

	// Double Buffer required for GDI+ to look right in WS_EX_COMPOSITED
	const auto ai = SetupAlphaBlend(&hdc, m_bBuffer && !m_bIsAnimated);
	Auto(FinishAlphaBlend(ai));

	if (bAnimAlpha)
		m_bAlphaBlend = true;

	//DcxControl::DrawCtrlBackground(hdc, this, &rect); // this causes flicker when playing animations...

	// draw bitmap
#ifdef DCX_USE_GDIPLUS
	if ((m_hBitmap) && (!m_bIsIcon) && (!m_pImage))
		DrawBMPImage(hdc, x, y, w, h);
	// draw icon
	else if ((m_hIcon) && (m_bIsIcon))
		DrawIconEx(hdc, 0, 0, m_hIcon, gsl::narrow_cast<int>(m_iIconSize), gsl::narrow_cast<int>(m_iIconSize), 0, m_hBackBrush, DI_NORMAL | DI_COMPAT);
	else if ((m_pImage) && (Dcx::GDIModule.isUseable()))
		DrawGDIImage(hdc, x, y, w, h);
#else
	if ((m_hBitmap) && (!m_bIsIcon))
		DrawBMPImage(hdc, x, y, w, h);
	// draw icon
	else if ((m_hIcon) && (m_bIsIcon))
		DrawIconEx(hdc, 0, 0, m_hIcon, gsl::narrow_cast<int>(m_iIconSize), gsl::narrow_cast<int>(m_iIconSize), 0, m_hBackBrush, DI_NORMAL | DI_COMPAT);
#endif
}

LRESULT DcxImage::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
