/*!
* \file misc.cpp
* \brief Misc Library File
*
* This file contains the main DCX dll routines.
*
* \author David Legault ( clickhere at scriptsdb dot org )
* \version 1.0
*
* \b Revisions
*
* � ScriptsDB.org - 2006-2008
*/

#include "defines.h"
//#include "Dcx.h"

#include <shlwapi.h>

// DCX Stuff

/*!
* \brief Rounding function
*/
int dcx_round(const float x) noexcept
{
	const auto t = gsl::narrow_cast<int>(x);
	if ((x - gsl::narrow_cast<float>(t)) > 0.5f)
		return t + 1;
	return t;
}

namespace
{

	// taken from: http://www.codeproject.com/Tips/317642/Handling-simple-text-files-in-C-Cplusplus
	//PTSTR Normalise(PBYTE pBuffer)
	//{
	//	PTSTR			ptText;		// pointer to the text char* or wchar_t* depending on UNICODE setting
	//	PWSTR			pwStr;		// pointer to a wchar_t buffer
	//	int				nLength;	// a useful integer variable
	//
	//	//IsTextUnicode(pBuffer, iSize, iRes);
	//
	//	// obtain a wide character pointer to check BOMs
	//	pwStr = reinterpret_cast<PWSTR>(pBuffer);
	//
	//	// check if the first word is a Unicode Byte Order Mark
	//	if (*pwStr == 0xFFFE || *pwStr == 0xFEFF)
	//	{
	//		// Yes, this is Unicode data
	//		if (*pwStr++ == 0xFFFE)
	//		{
	//			// BOM says this is Big Endian so we need
	//			// to swap bytes in each word of the text
	//			while (*pwStr)
	//			{
	//				// swap bytes in each word of the buffer
	//				WCHAR	wcTemp = *pwStr >> 8;
	//				wcTemp |= *pwStr << 8;
	//				*pwStr = wcTemp;
	//				++pwStr;
	//			}
	//			// point back to the start of the text
	//			pwStr = reinterpret_cast<PWSTR>(pBuffer + 2);
	//		}
	//#if !defined(UNICODE)
	//		// This is a non-Unicode project so we need
	//		// to convert wide characters to multi-byte
	//
	//		// get calculated buffer size
	//		nLength = WideCharToMultiByte(CP_UTF8, 0, pwStr, -1, nullptr, 0, nullptr, nullptr);
	//		// obtain a new buffer for the converted characters
	//		ptText = new TCHAR[nLength];
	//		// convert to multi-byte characters
	//		nLength = WideCharToMultiByte(CP_UTF8, 0, pwStr, -1, ptText, nLength, nullptr, nullptr);
	//#else
	//		nLength = wcslen(pwStr) + 1;    // if Unicode, then copy the input text
	//		ptText = new WCHAR[nLength];    // to a new output buffer
	//		nLength *= sizeof(WCHAR);       // adjust to size in bytes
	//		memcpy_s(ptText, nLength, pwStr, nLength);
	//#endif
	//	}
	//	else
	//	{
	//		// The text data is UTF-8 or Ansi
	//#if defined(UNICODE)
	//		// This is a Unicode project so we need to convert
	//		// multi-byte or Ansi characters to Unicode.
	//
	//		// get calculated buffer size
	//		nLength = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<PCSTR>(pBuffer), -1, nullptr, 0);
	//		// obtain a new buffer for the converted characters
	//		ptText = new TCHAR[nLength];
	//		// convert to Unicode characters
	//		nLength = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<PCSTR>(pBuffer), -1, ptText, nLength);
	//#else
	//		// This is a non-Unicode project so we just need
	//		// to skip the UTF-8 BOM, if present
	//		if (memcmp(pBuffer, "\xEF\xBB\xBF", 3) == 0)
	//		{
	//			// UTF-8
	//			pBuffer += 3;
	//		}
	//		nLength = strlen(reinterpret_cast<PSTR>(pBuffer)) + 1;  // if UTF-8/ANSI, then copy the input text
	//		ptText = new char[nLength];                             // to a new output buffer
	//		memcpy_s(ptText, nLength, pBuffer, nLength);
	//#endif
	//	}
	//
	//	// return pointer to the (possibly converted) text buffer.
	//	return ptText;
	//}

	TString Normalise(PBYTE pBuffer)
	{
		// pointer to a wchar_t buffer
		// obtain a wide character pointer to check BOMs
		PWSTR pwStr = reinterpret_cast<PWSTR>(pBuffer);

		if (!pwStr)
			return TString();

		// check if the first word is a Unicode Byte Order Mark
		if (*pwStr == 0xFFFE || *pwStr == 0xFEFF)
		{
			// Yes, this is Unicode data
			if (*pwStr++ == 0xFFFE)
			{
				// BOM says this is Big Endian so we need
				// to swap bytes in each word of the text
				while (*pwStr)
				{
					// swap bytes in each word of the buffer
					WCHAR	wcTemp = static_cast<WCHAR>((*pwStr >> 8));
					wcTemp |= (*pwStr << 8);
					*pwStr = wcTemp;
					++pwStr;
				}
				// point back to the start of the text
				pwStr = reinterpret_cast<PWSTR>(pBuffer + 2);
			}
			// creation of TString object handles any conversions & memory allocations etc..
			//return pwStr;
			return TString(pwStr);
		}
		else
		{
			// The text data is UTF-8 or Ansi
			// skip the UTF-8 BOM, if present
			if (memcmp(pBuffer, "\xEF\xBB\xBF", 3) == 0)
			{
				// UTF-8
				pBuffer += 3;
			}
		}

		// return pointer to the (possibly converted) text buffer.
		// creation of TString object handles any conversions & memory allocations etc..
		return TString(reinterpret_cast<PCSTR>(pBuffer));
	}

	// Taken from msft examples.
	PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp) noexcept
	{
		if (!hBmp)
			return nullptr;

		BITMAP bmp{};
		PBITMAPINFO pbmi{};
		WORD    cClrBits{};

		// Retrieve the bitmap color format, width, and height.  
		if (!GetObject(hBmp, sizeof(BITMAP), &bmp))
			return nullptr;

		// Convert the color format to a count of bits.  
		cClrBits = gsl::narrow_cast<WORD>(bmp.bmPlanes * bmp.bmBitsPixel);
		if (cClrBits == 1)
			cClrBits = 1;
		else if (cClrBits <= 4)
			cClrBits = 4;
		else if (cClrBits <= 8)
			cClrBits = 8;
		else if (cClrBits <= 16)
			cClrBits = 16;
		else if (cClrBits <= 24)
			cClrBits = 24;
		else cClrBits = 32;

		// Allocate memory for the BITMAPINFO structure. (This structure  
		// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
		// data structures.)  

		if (cClrBits < 24)
			pbmi = static_cast<PBITMAPINFO>(LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * (static_cast<size_t>(1U) << cClrBits))));

		// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

		else
			pbmi = static_cast<PBITMAPINFO>(LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER)));

		if (!pbmi)
			return nullptr;

		// Initialize the fields in the BITMAPINFO structure.  

		pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pbmi->bmiHeader.biWidth = bmp.bmWidth;
		pbmi->bmiHeader.biHeight = bmp.bmHeight;
		pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
		pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
		if (cClrBits < 24)
			pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

		// If the bitmap is not compressed, set the BI_RGB flag.  
		pbmi->bmiHeader.biCompression = BI_RGB;

		// Compute the number of bytes in the array of color  
		// indices and store the result in biSizeImage.  
		// The width must be DWORD aligned unless the bitmap is RLE 
		// compressed. 
		pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8 * pbmi->bmiHeader.biHeight;
		// Set biClrImportant to 0, indicating that all of the  
		// device colors are important.  
		pbmi->bmiHeader.biClrImportant = 0;
		return pbmi;
	}

	// Taken from msft examples.
	bool CreateBMPFile(LPCTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC) noexcept
	{
		if ((!pszFile) || (!pbi) || (!hBMP) || (!hDC))
			return false;

		PBITMAPINFOHEADER pbih = reinterpret_cast<PBITMAPINFOHEADER>(pbi);     // bitmap info-header  
		LPBYTE lpBits = static_cast<LPBYTE>(GlobalAlloc(GMEM_FIXED, pbih->biSizeImage));              // memory pointer

		if (!lpBits)
			return false;

		Auto(GlobalFree(lpBits));

		// Retrieve the color table (RGBQUAD array) and the bits  
		// (array of palette indices) from the DIB.  
		if (!GetDIBits(hDC, hBMP, 0, pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS))
			return false;

		//Dcx::dcxFileHandleResource hf(pszFile, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		// Create the .BMP file.  
		auto hf = CreateFile(pszFile,
			GENERIC_READ | GENERIC_WRITE,
			0,
			nullptr,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);
		if (hf == INVALID_HANDLE_VALUE)
			return false;

		Auto(CloseHandle(hf));

		BITMAPFILEHEADER hdr{};       // bitmap file-header  
		hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
		// Compute the size of the entire file.  
		hdr.bfSize = (sizeof(BITMAPFILEHEADER) + pbih->biSize + (pbih->biClrUsed * sizeof(RGBQUAD)) + pbih->biSizeImage);
		hdr.bfReserved1 = 0;
		hdr.bfReserved2 = 0;

		// Compute the offset to the array of color indices.  
		hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + pbih->biSize + (pbih->biClrUsed * sizeof(RGBQUAD));

		DWORD dwTmp{};

		// Copy the BITMAPFILEHEADER into the .BMP file.  
		if (!WriteFile(hf, &hdr, sizeof(BITMAPFILEHEADER), &dwTmp, nullptr))
			return false;

		// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
		if (!WriteFile(hf, pbih, sizeof(BITMAPINFOHEADER) + (pbih->biClrUsed * sizeof(RGBQUAD)), &dwTmp, nullptr))
			return false;

		// Copy the array of color indices into the .BMP file.
		if (!WriteFile(hf, lpBits, pbih->biSizeImage, &dwTmp, nullptr))
			return false;

		return true;
	}

	/*!
	* \brief Save a HBITMAP object to file
	*
	* TODO: ...
	*/
	bool SaveDataToFile(const TString& tsFile, const HBITMAP hBm) noexcept
	{
		if ((!hBm) || (tsFile.empty()))
			return false;

		auto pbis = CreateBitmapInfoStruct(hBm);
		if (!pbis)
			return false;
		Auto(LocalFree(pbis));

		auto hDC = CreateCompatibleDC(nullptr);
		if (!hDC)
			return false;
		Auto(DeleteDC(hDC));

		auto oldBm = SelectObject(hDC, hBm);
		Auto(SelectObject(hDC, oldBm));

		return CreateBMPFile(tsFile.to_chr(), pbis, hBm, hDC);
	}

	bool SaveClipboardAsText(const TString& tsFile)
	{
		if (auto hData = GetClipboardData(CF_UNICODETEXT); hData)
		{
			// clipboard has a string
			auto szClip = static_cast<LPCWSTR>(GlobalLock(hData));
			Auto(GlobalUnlock(hData));

			return SaveDataToFile(tsFile, szClip);
		}
		return false;
	}

	bool SaveClipboardAsBitmap(const TString& tsFile) noexcept
	{
		if (auto hBm = static_cast<HBITMAP>(GetClipboardData(CF_BITMAP)); hBm)
		{
			// clipboard has a bitmap
			return SaveDataToFile(tsFile, hBm);
		}
		return false;
	}

	HICON CreateGrayscaleIcon(HICON hIcon, const COLORREF* const pPalette) noexcept
	{
		if ((!hIcon) || (!pPalette))
			return nullptr;

		auto hdc = ::GetDC(nullptr);
		Auto(::ReleaseDC(nullptr, hdc));

		HICON      hGrayIcon{ nullptr };
		ICONINFO   icInfo{};
		ICONINFO   icGrayInfo{};

		BITMAPINFO bmpInfo = { 0 };
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

		if (::GetIconInfo(hIcon, &icInfo))
		{
			if (icInfo.hbmColor)
			{
				Auto(::DeleteObject(icInfo.hbmColor));

				if (::GetDIBits(hdc, icInfo.hbmColor, 0, 0, nullptr, &bmpInfo, DIB_RGB_COLORS) != 0)
				{
					const SIZE sz{ bmpInfo.bmiHeader.biWidth, bmpInfo.bmiHeader.biHeight };

					bmpInfo.bmiHeader.biCompression = BI_RGB;

					const auto c1 = gsl::narrow_cast<DWORD>(sz.cx * sz.cy);

					const auto lpBits = static_cast<LPDWORD>(::GlobalAlloc(GMEM_FIXED, (gsl::narrow_cast<SIZE_T>(c1)) * 4));

					if (!lpBits)
						return nullptr;

					Auto(::GlobalFree(lpBits));

					if (::GetDIBits(hdc, icInfo.hbmColor, 0, gsl::narrow_cast<UINT>(sz.cy), lpBits, &bmpInfo, DIB_RGB_COLORS) != 0)
					{
						auto lpBitsPtr = reinterpret_cast<LPBYTE>(lpBits);

						for (auto i = decltype(c1){0}; i < c1; ++i)
						{
							auto off = gsl::narrow_cast<UINT>((255 - ((lpBitsPtr[0] + lpBitsPtr[1] + lpBitsPtr[2]) / 3)));

							if (lpBitsPtr[3] != 0 || off != 255)
							{
								if (off == 0)
								{
									off = 1;
								}
								lpBits[i] = pPalette[off] | (lpBitsPtr[3] << 24);
							}

							lpBitsPtr += 4;
						}

						icGrayInfo.hbmColor = ::CreateCompatibleBitmap(hdc, sz.cx, sz.cy);

						if (icGrayInfo.hbmColor)
						{
							Auto(::DeleteObject(icGrayInfo.hbmColor));

							::SetDIBits(hdc, icGrayInfo.hbmColor, 0, gsl::narrow_cast<UINT>(sz.cy), lpBits, &bmpInfo, DIB_RGB_COLORS);

							icGrayInfo.hbmMask = icInfo.hbmMask;
							icGrayInfo.fIcon = TRUE;

							hGrayIcon = ::CreateIconIndirect(&icGrayInfo);
						}

					}
				}

				if (icInfo.hbmMask)
					::DeleteObject(icInfo.hbmMask);
			}
		}
		return hGrayIcon;
	}

	constexpr int unfoldColor(int nColor) noexcept
	{
		while (nColor > 99)
			nColor -= 100;

		return nColor;
	}

	void mIRC_OutText(HDC hdc, LPCRECT rcBounds, TString& txt, LPRECT rcOut, const LPLOGFONT lf, const UINT iStyle, const COLORREF clrFG, const bool shadow) noexcept
	{
		//if (txt.empty() || !rcOut || !lf || !hdc)
		//	return;
		//
		//const auto len = txt.len();
		//auto hNewFont = CreateFontIndirectW(lf);
		//if (!hNewFont)
		//	return;
		//
		//const auto hOldFont = SelectObject(hdc, hNewFont);
		//auto rcTmp = *rcOut;
		//
		//TEXTMETRICW tm{};
		//GetTextMetricsW(hdc, std::addressof(tm));
		//
		//if (!dcx_testflag(iStyle, DT_CALCRECT))
		//{	// if DT_CALCRECT flag NOT given then do calcrect here.
		//	//if (shadow)
		//	//	dcxDrawShadowText(hdc, txt.to_wchr(), gsl::narrow_cast<UINT>(len), std::addressof(rcTmp), iStyle | DT_CALCRECT, clrFG, 0, 5, 5);
		//	//else
		//	//	DrawTextW(hdc, txt.to_wchr(), gsl::narrow_cast<int>(len), std::addressof(rcTmp), iStyle | DT_CALCRECT);
		//	DRAWTEXTPARAMS params{};
		//	params.cbSize = sizeof(DRAWTEXTPARAMS);
		//	params.iTabLength = 4;
		//
		//	DrawTextExW(hdc, txt.to_wchr(), gsl::narrow_cast<int>(len), std::addressof(rcTmp), iStyle | DT_CALCRECT, &params);
		//
		//	if (params.uiLengthDrawn < len)
		//	{
		//		if (rcTmp.left != rcBounds->left)
		//		{
		//			rcTmp.left = rcBounds->left;
		//			rcTmp.top += tm.tmHeight;
		//			rcOut->left = rcBounds->left;
		//			rcOut->top = rcTmp.top;
		//
		//			DrawTextExW(hdc, txt.to_wchr(), gsl::narrow_cast<int>(len), std::addressof(rcTmp), iStyle | DT_CALCRECT, &params);
		//		}
		//	}
		//}
		//if (shadow)
		//	dcxDrawShadowText(hdc, txt.to_wchr(), gsl::narrow_cast<UINT>(len), std::addressof(rcTmp), iStyle, clrFG, 0, 5, 5);
		//else
		//	DrawTextW(hdc, txt.to_wchr(), gsl::narrow_cast<int>(len), std::addressof(rcTmp), iStyle);
		//
		//if (tm.tmHeight != 0)
		//{
		//	if (!dcx_testflag(iStyle, DT_SINGLELINE))
		//	{
		//		if ((rcTmp.bottom - rcTmp.top) >= (tm.tmHeight * 2))
		//		{
		//			//rcOut->top += tm.tmHeight;
		//			rcOut->top += (rcTmp.bottom - rcTmp.top);
		//			rcOut->left = rcBounds->left;
		//		}
		//		else
		//			rcOut->left += (rcTmp.right - rcTmp.left) - tm.tmOverhang;
		//	}
		//	else
		//		rcOut->left += (rcTmp.right - rcTmp.left) - tm.tmOverhang;
		//}
		//else
		//	rcOut->left += (rcTmp.right - rcTmp.left);
		//
		//DeleteObject(SelectObject(hdc, hOldFont));
		//txt.clear();

		if (txt.empty() || !rcOut || !lf || !hdc)
			return;

		const auto len = txt.len();
		auto hNewFont = CreateFontIndirectW(lf);
		if (!hNewFont)
			return;

		const auto hOldFont = SelectObject(hdc, hNewFont);
		auto rcTmp = *rcOut;

		TEXTMETRICW tm{};
		GetTextMetricsW(hdc, std::addressof(tm));

		DRAWTEXTPARAMS params{};
		params.cbSize = sizeof(DRAWTEXTPARAMS);
		params.iTabLength = 4;
		WCHAR* pText = txt.to_wchr();

		for (UINT uCharsDrawn{}; uCharsDrawn < len; uCharsDrawn += params.uiLengthDrawn)
		{
			params.uiLengthDrawn = 0;
			// see if text fits...
			DrawTextExW(hdc, pText, gsl::narrow_cast<int>(len - uCharsDrawn), std::addressof(rcTmp), iStyle | DT_CALCRECT, &params);

			// doesnt all fit on this line
			if (params.uiLengthDrawn < (len - uCharsDrawn))
			{
				// if not at the start of the line, dropdown to the next & try again.
				if (rcTmp.left != rcBounds->left)
				{
					rcTmp = *rcOut;
					rcTmp.left = rcBounds->left;
					rcTmp.top += tm.tmHeight;
					rcOut->left = rcBounds->left;
					rcOut->top = rcTmp.top;

					DrawTextExW(hdc, pText, gsl::narrow_cast<int>(len - uCharsDrawn), std::addressof(rcTmp), iStyle | DT_CALCRECT, &params);
				}
			}

			if (shadow)
				dcxDrawShadowText(hdc, pText, gsl::narrow_cast<UINT>(params.uiLengthDrawn), std::addressof(rcTmp), iStyle, clrFG, 0, 5, 5);
			else
				DrawTextW(hdc, pText, gsl::narrow_cast<int>(params.uiLengthDrawn), std::addressof(rcTmp), iStyle);

			// check if all characters were drawn
			if ((params.uiLengthDrawn + uCharsDrawn) < len)
			{
				//if (rcTmp.left != rcBounds->left)
				{
					rcTmp.left = rcBounds->left;
					rcTmp.top += tm.tmHeight;
					rcOut->left = rcBounds->left;
					rcOut->top = rcTmp.top;
				}
				pText += params.uiLengthDrawn;
			}

			if (rcTmp.bottom > rcBounds->bottom)
				break;
		}

		if (tm.tmHeight != 0)
		{
			if (!dcx_testflag(iStyle, DT_SINGLELINE))
			{
				if ((rcTmp.bottom - rcTmp.top) >= (tm.tmHeight * 2))
				{
					//rcOut->top += tm.tmHeight;
					rcOut->top += (rcTmp.bottom - rcTmp.top);
					rcOut->left = rcBounds->left;
				}
				else
					rcOut->left += (rcTmp.right - rcTmp.left) - tm.tmOverhang;
			}
			else
				rcOut->left += (rcTmp.right - rcTmp.left) - tm.tmOverhang;
		}
		else
			rcOut->left += (rcTmp.right - rcTmp.left);

		DeleteObject(SelectObject(hdc, hOldFont));
		txt.clear();
	}

	double sRGBtoLin(double colorChannel) noexcept
	{
		// Send this function a decimal sRGB gamma encoded color value
		// between 0.0 and 1.0, and it returns a linearized value.

		if (colorChannel <= 0.04045)
			return colorChannel / 12.92;

		return pow(((colorChannel + 0.055) / 1.055), 2.4);
	}

	double YtoLstar(double Y) noexcept
	{
		// Send this function a luminance value between 0.0 and 1.0,
		// and it returns L* which is "perceptual lightness"

		if (Y <= (216.0 / 24389.0))       // The CIE standard states 0.008856 but 216/24389 is the intent for 0.008856451679036
			return Y * (24389.0 / 27.0);  // The CIE standard states 903.3, but 24389/27 is the intent, making 903.296296296296296

		return pow(Y, (1.0 / 3.0)) * 116.0 - 16.0;
	}

#ifdef DCX_USE_GDIPLUS
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
		//UINT  num = 0;          // number of image encoders
		//UINT  size = 0;         // size of the image encoder array in bytes
		//
		//Gdiplus::GetImageEncodersSize(&num, &size);
		//if (size == 0)
		//	return -1;  // Failure
		//
		//Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		//if (pImageCodecInfo == nullptr)
		//	return -1;  // Failure
		//
		//GetImageEncoders(num, size, pImageCodecInfo);
		//
		//for (UINT j = 0; j < num; ++j)
		//{
		//	if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		//	{
		//		*pClsid = pImageCodecInfo[j].Clsid;
		//		free(pImageCodecInfo);
		//		return j;  // Success
		//	}
		//}
		//
		//free(pImageCodecInfo);
		//return -1;  // Failure

		if (!format || !pClsid)
			return -1;

		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0)
			return -1;  // Failure

		std::vector<BYTE> codecdata;
		codecdata.reserve(size);

		Gdiplus::ImageCodecInfo* pImageCodecInfo = reinterpret_cast<Gdiplus::ImageCodecInfo*>(codecdata.data());
		if (pImageCodecInfo == nullptr)
			return -1;  // Failure

		GetImageEncoders(num, size, pImageCodecInfo);

		for (UINT j = 0; j < num; ++j)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				return j;  // Success
			}
		}
		return -1;  // Failure
	}

	bool CreatePNGFile(const WCHAR* pszFile, Gdiplus::Image& img)
	{
		if (!pszFile)
			return false;

		CLSID pngClsid;
		if (GetEncoderClsid(L"image/png", &pngClsid) != -1)
		{
			if (img.Save(pszFile, &pngClsid) == Gdiplus::Status::Ok)
				return true;
		}
		return false;
	}
#endif
}

/*!
* \brief Read File Contents
*
*/
auto readFile(const TString& filename)
-> std::unique_ptr<BYTE[]>
{
	if (filename.empty())
		return nullptr;

#if DCX_USE_WRAPPERS
	const Dcx::dcxFileResource file(filename, TEXT("rb"));

	const auto size = file.Size();

	// make data container for file contents
	auto fileContents = std::make_unique<BYTE[]>(gsl::narrow_cast<size_t>(size) + 2);

	// Null terminate the string (use double zero)
	gsl::at(fileContents, size) = 0;
	gsl::at(fileContents, gsl::narrow_cast<gsl::index>(size) + 1U) = 0;

	// read the file
	fread(fileContents.get(), 1U, size, file.get());

	// return memory block containing file data
	return fileContents;
#else
	auto file = dcx_fopen(filename, TEXT("rb"));

	// Open file in binary mode and read
	if (!file)
		return nullptr;

	Auto(fclose(file));

	// Seek End of file
	if (fseek(file, 0, SEEK_END))
		return nullptr;

	// Read pointer location, because pointer is at the end, results into file size
	const auto size = gsl::narrow_cast<size_t>(ftell(file));

	// Get back to file beginning
	if (fseek(file, 0, SEEK_SET))
		return nullptr;

	// make data container for file contents
	auto fileContents = std::make_unique<BYTE[]>(size + 2);

	// Null terminate the string (use double zero)
	fileContents[size] = 0;
	fileContents[size + 1U] = 0;

	// read the file
	fread(fileContents.get(), 1U, size, file);

	// return memory block containing file data
	return fileContents;
#endif
}

/*!
* \brief Read Text File Contents
*
* This function correctly handles BOM types for ascii,utf8,utf16BE,utf16LE & returns text in a TString object.
*/
TString readTextFile(const TString& tFile)
{
	const auto data(readFile(tFile));

	return Normalise(data.get());
}

/*!
* \brief Save a TString object to file as text
*
* TODO: ...
*/
bool SaveDataToFile(const TString& tsFile, const TString& tsData)
{
#if DCX_USE_WRAPPERS
	const Dcx::dcxFileResource file(tsFile, TEXT("wb"));

	constexpr WCHAR tBOM = 0xFEFF;	// unicode BOM

	fwrite(&tBOM, sizeof(WCHAR), 1, file.get());

	// if not in unicode mode then save without BOM as ascii/utf8
	fwrite(tsData.to_chr(), sizeof(TString::value_type), tsData.len(), file.get());
	fflush(file.get());
	return true;
#else
	auto file = dcx_fopen(tsFile.to_chr(), TEXT("wb"));

	if (!file)
		return false;

	Auto(fclose(file));

	constexpr WCHAR tBOM = 0xFEFF;	// unicode BOM

	fwrite(&tBOM, sizeof(WCHAR), 1, file);

	// if not in unicode mode then save without BOM as ascii/utf8
	fwrite(tsData.to_chr(), sizeof(TString::value_type), tsData.len(), file);
	fflush(file);
	return true;
#endif
}

/*!
* \brief Copies string to the clipboard
*
* Returns TRUE if successful
*
* http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/dataexchange/clipboard/usingtheclipboard.asp
*/
bool CopyToClipboard(const HWND owner, const TString& str) noexcept
{
	if (!OpenClipboard(owner))
	{
		Dcx::error(TEXT("CopyToClipboard"), TEXT("Couldn't open clipboard"));
		return false;
	}

	Auto(CloseClipboard());

	const size_t cbsize = (str.len() + 1) * sizeof(TCHAR);
	EmptyClipboard();
	auto hglbCopy = GlobalAlloc(GMEM_MOVEABLE, cbsize);

	if (!hglbCopy)
	{
		Dcx::error(TEXT("CopyToClipboard"), TEXT("Couldn't open global memory"));
		return false;
	}

	{	// simply sets scope for vars
		const auto strCopy = static_cast<TCHAR*>(GlobalLock(hglbCopy));

		if (!strCopy)
		{
			GlobalFree(hglbCopy);
			Dcx::error(TEXT("CopyToClipboard"), TEXT("Couldn't lock global memory"));
			return false;
		}

		dcx_strcpyn(strCopy, str.to_chr(), str.len() + 1);

		GlobalUnlock(hglbCopy);
	}

	SetClipboardData(CF_UNICODETEXT, hglbCopy);
	return true;
}

std::pair<bool, int> SaveClipboardToFile(const XSwitchFlags& xFlags, const TString& tsFile)
{
	if (!OpenClipboard(nullptr))
		throw Dcx::dcxException(TEXT("SaveClipboardToFile: %"), TEXT("Couldn't open clipboard"));

	Auto(CloseClipboard());

	if (!xFlags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	// text save only
	if (xFlags[TEXT('t')])
		return { SaveClipboardAsText(tsFile), CF_UNICODETEXT };

	// bitmap save only
	if (xFlags[TEXT('b')])
		return { SaveClipboardAsBitmap(tsFile), CF_BITMAP };

	// otherwise try both...
	if (SaveClipboardAsText(tsFile))
		return { true, CF_UNICODETEXT };

	return { SaveClipboardAsBitmap(tsFile), CF_BITMAP };
}

//Dcx::BoolValue<int> SaveClipboardToFile(const XSwitchFlags& xFlags, const TString& tsFile)
//{
//	if (!OpenClipboard(nullptr))
//		throw Dcx::dcxException(TEXT("SaveClipboardToFile: %"), TEXT("Couldn't open clipboard"));
//
//	Auto(CloseClipboard());
//
//	if (!xFlags[TEXT('+')])
//		throw DcxExceptions::dcxInvalidFlag();
//
//	// text save only
//	if (xFlags[TEXT('t')])
//		return { SaveClipboardAsText(tsFile), CF_UNICODETEXT };
//
//	// bitmap save only
//	if (xFlags[TEXT('b')])
//		return { SaveClipboardAsBitmap(tsFile), CF_BITMAP };
//
//	// otherwise try both...
//	if (SaveClipboardAsText(tsFile))
//		return { true, CF_UNICODETEXT };
//
//	return { SaveClipboardAsBitmap(tsFile), CF_BITMAP };
//}

// Turns a command (+flags CHARSET SIZE FONTNAME) into a LOGFONT struct
GSL_SUPPRESS(bounds.3) bool ParseCommandToLogfont(const TString& cmd, LPLOGFONT lf)
{
	if (!lf || cmd.numtok() < 4)
		return false;

	ZeroMemory(lf, sizeof(LOGFONT));
	const auto flags = parseFontFlags(cmd.getfirsttok(1));

	if (dcx_testflag(flags, dcxFontFlags::DCF_DEFAULT))
		return (GetObject(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), sizeof(LOGFONT), lf) != 0);

	lf->lfCharSet = gsl::narrow_cast<BYTE>(parseFontCharSet(cmd.getnexttok()) & 0xFF);	// tok 2
	const auto fSize = cmd.getnexttok().to_int();				// tok 3
	const auto fName(cmd.getlasttoks().trim());				// tok 4, -1

	if (!fSize)
		return false;

	{
		const auto hdc = GetDC(nullptr);

		if (!hdc)
			return false;

		Auto(ReleaseDC(nullptr, hdc));

		lf->lfHeight = -MulDiv(fSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	}

	if (dcx_testflag(flags, dcxFontFlags::DCF_ANTIALIASE))
		lf->lfQuality = ANTIALIASED_QUALITY;

	if (dcx_testflag(flags, dcxFontFlags::DCF_BOLD))
		lf->lfWeight = FW_BOLD;
	else if (dcx_testflag(flags, dcxFontFlags::DCF_LIGHT))
		lf->lfWeight = FW_LIGHT;
	else
		lf->lfWeight = FW_NORMAL;

	if (dcx_testflag(flags, dcxFontFlags::DCF_ITALIC))
		lf->lfItalic = TRUE;

	if (dcx_testflag(flags, dcxFontFlags::DCF_STRIKEOUT))
		lf->lfStrikeOut = TRUE;

	if (dcx_testflag(flags, dcxFontFlags::DCF_UNDERLINE))
		lf->lfUnderline = TRUE;

	dcx_strcpyn(&lf->lfFaceName[0], fName.to_chr(), std::size(lf->lfFaceName));
	gsl::at(lf->lfFaceName, std::size(lf->lfFaceName) - 1) = 0;

	return true;
}

dcxFontFlags parseFontFlags(const TString& flags) noexcept
{
	dcxFontFlags iFlags{};
	const XSwitchFlags xflags(flags);

	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('a')])
		iFlags |= dcxFontFlags::DCF_ANTIALIASE;
	if (xflags[TEXT('b')])
		iFlags |= dcxFontFlags::DCF_BOLD;
	if (xflags[TEXT('d')])
		iFlags |= dcxFontFlags::DCF_DEFAULT;
	if (xflags[TEXT('i')])
		iFlags |= dcxFontFlags::DCF_ITALIC;
	if (xflags[TEXT('l')])
		iFlags |= dcxFontFlags::DCF_LIGHT;
	if (xflags[TEXT('s')])
		iFlags |= dcxFontFlags::DCF_STRIKEOUT;
	if (xflags[TEXT('u')])
		iFlags |= dcxFontFlags::DCF_UNDERLINE;

	return iFlags;
}

UINT parseFontCharSet(const TString& charset)
{
	const static std::map<std::hash<TString>::result_type, UINT> charset_map{
		{TEXT("ansi"_hash), ANSI_CHARSET},
		{ TEXT("baltic"_hash), BALTIC_CHARSET },
		{ TEXT("chinesebig"_hash), CHINESEBIG5_CHARSET },
		{ TEXT("default"_hash), DEFAULT_CHARSET },
		{ TEXT("easteurope"_hash), EASTEUROPE_CHARSET },
		{ TEXT("gb2312"_hash), GB2312_CHARSET },
		{ TEXT("greek"_hash), GREEK_CHARSET },
		{ TEXT("hangul"_hash), HANGUL_CHARSET },
		{ TEXT("mac"_hash), MAC_CHARSET },
		{ TEXT("oem"_hash), OEM_CHARSET },
		{ TEXT("russian"_hash), RUSSIAN_CHARSET },
		{ TEXT("shiftjis"_hash), SHIFTJIS_CHARSET },
		{ TEXT("symbol"_hash), SYMBOL_CHARSET },
		{ TEXT("turkish"_hash), TURKISH_CHARSET },
		{ TEXT("vietnamese"_hash), VIETNAMESE_CHARSET },
		{ TEXT("johab"_hash), JOHAB_CHARSET },
		{ TEXT("hebrew"_hash), HEBREW_CHARSET },
		{ TEXT("arabic"_hash), ARABIC_CHARSET },
		{ TEXT("thai"_hash), THAI_CHARSET }
	};

	if (const auto it = charset_map.find(std::hash<TString>{}(charset)); it != charset_map.end())
		return it->second;

	return DEFAULT_CHARSET;
}

TString ParseLogfontToCommand(const LPLOGFONT lf)
{
	TString flags(TEXT('+')), charset(TEXT("default"_ts)), tmp;

	if (!lf)
		return tmp;

	// get charset
	switch (lf->lfCharSet)
	{
	case ANSI_CHARSET: charset = TEXT("ansi"); break;
	case BALTIC_CHARSET: charset = TEXT("baltic"); break;
	case CHINESEBIG5_CHARSET: charset = TEXT("chinesebig"); break;
	case EASTEUROPE_CHARSET: charset = TEXT("easteurope"); break;
	case GB2312_CHARSET: charset = TEXT("gb2312"); break;
	case GREEK_CHARSET: charset = TEXT("greek"); break;
	case HANGUL_CHARSET: charset = TEXT("hangul"); break;
	case MAC_CHARSET: charset = TEXT("mac"); break;
	case OEM_CHARSET: charset = TEXT("oem"); break;
	case RUSSIAN_CHARSET: charset = TEXT("russian"); break;
	case SHIFTJIS_CHARSET: charset = TEXT("shiftjis"); break;
	case SYMBOL_CHARSET: charset = TEXT("symbol"); break;
	case TURKISH_CHARSET: charset = TEXT("turkish"); break;
	case VIETNAMESE_CHARSET: charset = TEXT("vietnamese"); break;
	case JOHAB_CHARSET: charset = TEXT("johab"); break;
	case HEBREW_CHARSET: charset = TEXT("hebrew"); break;
	case ARABIC_CHARSET: charset = TEXT("arabic"); break;
	case THAI_CHARSET: charset = TEXT("thai"); break;
	case DEFAULT_CHARSET:
	default: charset = TEXT("default"); break;
	}

	// get flags
	if (lf->lfQuality == ANTIALIASED_QUALITY)
		flags += TEXT('a');
	if (lf->lfWeight == FW_BOLD)
		flags += TEXT('b');
	if (lf->lfItalic)
		flags += TEXT('i');
	if (lf->lfStrikeOut)
		flags += TEXT('s');
	if (lf->lfUnderline)
		flags += TEXT('u');

	//lf.lfHeight = -MulDiv( fSize, GetDeviceCaps(hdc, LOGPIXELSY ), 72 );
	//
	//auto hdc = GetDC(nullptr);
	//
	//if (hdc == nullptr)
	//	return tmp;
	//
	//auto hf = CreateFontIndirect(lf);
	//
	//if (hf != nullptr) {
	//	TEXTMETRIC tm;
	//
	//	const auto oldhf = SelectFont(hdc, hf);
	//	GetTextMetrics(hdc, &tm);
	//
	//	//auto ptSize = (int) (-1 * (lfCurrent.lfHeight * 72 / GetDeviceCaps(hdc, LOGPIXELSY)));
	//	const auto ptSize = MulDiv(tm.tmHeight - tm.tmInternalLeading, 72, GetDeviceCaps(hdc, LOGPIXELSY));
	//	SelectFont(hdc,oldhf);
	//	DeleteFont(hf);
	//	// [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
	//	tmp.tsprintf(TEXT("%s %s %d %s"), flags.to_chr(), charset.to_chr(), ptSize, lf->lfFaceName);
	//	//tsprintf(tmp, TEXT("% % % %"), flags, charset, ptSize, lf->lfFaceName);
	//}
	//ReleaseDC(nullptr, hdc);
	//
	//return tmp;

	const auto hdc = GetDC(nullptr);

	if (!hdc)
		return tmp;

	Auto(ReleaseDC(nullptr, hdc));

	const auto hf = CreateFontIndirectW(lf);

	if (!hf)
		return tmp;

	Auto(DeleteObject(hf));

	const auto oldhf = SelectObject(hdc, hf);

	Auto(SelectObject(hdc, oldhf));

	if (TEXTMETRIC tm{}; GetTextMetricsW(hdc, &tm))
	{
		//auto ptSize = (int) (-1 * (lfCurrent.lfHeight * 72 / GetDeviceCaps(hdc, LOGPIXELSY)));
		const auto ptSize = MulDiv(tm.tmHeight - tm.tmInternalLeading, 72, GetDeviceCaps(hdc, LOGPIXELSY));
		// [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
		_ts_sprintf(tmp, TEXT("% % % %"), flags, charset, ptSize, lf->lfFaceName);
	}
	return tmp;
}

//HICON dcxLoadIcon(const int index, TString &filename, const bool large, const TString &flags) {
//	filename.trim();
//
//	const XSwitchFlags xflags(flags);
//
//	if (!xflags[TEXT('+')]) {
//		Dcx::error(TEXT("dcxLoadIcon"), TEXT("Invalid Flags"));
//		return nullptr;
//	}
//
//	// index is -1
//	if (index < 0) {
//		Dcx::error(TEXT("dcxLoadIcon"), TEXT("Invalid Index"));
//		return nullptr;
//	}
//
//	// This doesnt require a valid filename.
//	if (xflags[TEXT('f')]) {
//		SHFILEINFO shfi;
//		TString filetype;
//
//		ZeroMemory(&shfi, sizeof(SHFILEINFO));
//		filetype.tsprintf(TEXT(".%s"), filename.to_chr());
//		
//		if (SHGetFileInfo(filetype.to_chr(), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | (large ? SHGFI_LARGEICON : SHGFI_SMALLICON)) != 0)
//			return shfi.hIcon;
//
//		Dcx::error(TEXT("dcxLoadIcon"), TEXT("Unable to get filetype icon"));
//		return nullptr;
//	}
//
//	// Check for valid filename
//	if (!IsFile(filename)) {
//		Dcx::errorex(TEXT("dcxLoadIcon"), TEXT("Could Not Access File: %s"), filename.to_chr());
//		return nullptr;
//	}
//
//	HICON icon = nullptr;
//
//	if (xflags[TEXT('a')]) {
//		WORD wIndex = (WORD)index;
//		icon = ExtractAssociatedIcon(nullptr, filename.to_chr(), &wIndex);
//	}
//#ifdef DCX_USE_GDIPLUS
//	else if (xflags[TEXT('P')]) {
//		if (!Dcx::GDIModule.isUseable())
//		{
//			Dcx::error(TEXT("dcxLoadIcon"), TEXT("Invalid +P without GDI+."));
//			return nullptr;
//		}
//
//		try {
//			Bitmap *p_Img = new Bitmap(filename.to_chr());
//
//			// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
//			Status status = p_Img->GetLastStatus();
//			if (status != Ok)
//				Dcx::error(TEXT("dcxLoadIcon"), GetLastStatusStr(status));
//			else {
//				//int w = 0, h = 0;
//				//if (large) {
//				//	w = GetSystemMetrics(SM_CXICON);
//				//	h = GetSystemMetrics(SM_CYICON);
//				//}
//				//else {
//				//	w = GetSystemMetrics(SM_CXSMICON);
//				//	h = GetSystemMetrics(SM_CYSMICON);
//				//}
//				//Bitmap *p_Thumb = p_Img->GetThumbnailImage(w,h);
//				//if (p_Thumb != nullptr) {
//				//	p_Thumb->GetHICON(&icon);
//				//	delete p_Thumb;
//				//}
//				status = p_Img->GetHICON(&icon); // for reasons unknown this causes a `first chance exception` to show in debug log.
//				if (status != Ok)
//					Dcx::error(TEXT("dcxLoadIcon"), GetLastStatusStr(status));
//				GdiFlush();
//			}
//			delete p_Img;
//		}
//		catch (const std::bad_alloc)
//		{
//			return nullptr;
//		}
//	}
//#endif
//	else {
//		if (large)
//			ExtractIconEx(filename.to_chr(), index, &icon, nullptr, 1);
//		else
//			ExtractIconEx(filename.to_chr(), index, nullptr, &icon, 1);
//	}
//
//	if (xflags[TEXT('g')] && icon != nullptr)
//		icon = CreateGrayscaleIcon(icon);
//
//	return icon;
//}

GSL_SUPPRESS(Enum.3)
HICON dcxLoadIcon(const int index, TString& filename, const bool large, const TString& flags)
{
	filename.trim();

	const XSwitchFlags xflags(flags);

	if (!xflags[TEXT('+')])
		throw Dcx::dcxException("dcxLoadIcon: Invalid Flags");

	// index is -1
	if (index < 0)
		throw Dcx::dcxException("dcxLoadIcon: Invalid Index");

	// This doesnt require a valid filename.
	if (xflags[TEXT('f')])
	{
		SHFILEINFO shfi{};

		TString filetype{ TEXT("."), filename };

		if (SHGetFileInfo(filetype.to_chr(), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO), gsl::narrow_cast<UINT>(SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | (large ? SHGFI_LARGEICON : SHGFI_SMALLICON))) == 0)
			throw Dcx::dcxException("dcxLoadIcon: Unable to get filetype icon");

		return shfi.hIcon;
	}

	// Check for valid filename
	if (!xflags[TEXT('B')])
	{
		if (!IsFile(filename))
			throw Dcx::dcxException(TEXT("dcxLoadIcon: Could Not Access File: %"), filename);
	}

	HICON icon{};

	if (xflags[TEXT('a')])
	{
		auto wIndex = gsl::narrow_cast<WORD>(index);
		icon = ExtractAssociatedIcon(nullptr, filename.to_chr(), &wIndex);
	}
#ifdef DCX_USE_GDIPLUS
	else if (xflags[TEXT('P')])
	{
		if (!Dcx::GDIModule.isUseable())
			throw Dcx::dcxException("dcxLoadIcon: Invalid +P without GDI+.");

		//const auto p_Img = std::make_unique<Gdiplus::Bitmap>(filename.to_chr());
		//
		//// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
		//if (const auto status = p_Img->GetLastStatus(); status != Gdiplus::Ok)
		//	throw Dcx::dcxException(TEXT("dcxLoadIcon: %"), GetLastStatusStr(status));
		//
		//// for reasons unknown this causes a `first chance exception` to show in debug log.
		//if (const auto status = p_Img->GetHICON(&icon); status != Gdiplus::Ok)
		//	throw Dcx::dcxException(TEXT("dcxLoadIcon: %"), GetLastStatusStr(status));

		Gdiplus::Bitmap img(filename.to_chr());

		// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
		if (const auto status = img.GetLastStatus(); status != Gdiplus::Ok)
			throw Dcx::dcxException(TEXT("dcxLoadIcon: %"), GetLastStatusStr(status));

		// for reasons unknown this causes a `first chance exception` to show in debug log.
		if (const auto status = img.GetHICON(&icon); status != Gdiplus::Ok)
			throw Dcx::dcxException(TEXT("dcxLoadIcon: %"), GetLastStatusStr(status));

		GdiFlush();
	}
#else
	else if (xflags[TEXT('P')])
		throw Dcx::dcxException("dcxLoadIcon: Invalid +P without GDI+.");
#endif
	else if (xflags[TEXT('B')])
	{
		// base64 data in filename.
		const auto hbmstring = filename.getfirsttok(1);
		const auto maskstring = filename.getlasttoks();
		if (large)
			icon = Base64ToIcon(hbmstring.to_chr(), maskstring.to_chr(), gsl::narrow_cast<long>(DcxIconSizes::LargeIcon));
		else
			icon = Base64ToIcon(hbmstring.to_chr(), maskstring.to_chr(), gsl::narrow_cast<long>(DcxIconSizes::SmallIcon));
	}
	else {
		if (large)
			ExtractIconExW(filename.to_wchr(), index, &icon, nullptr, 1);
		else
			ExtractIconExW(filename.to_wchr(), index, nullptr, &icon, 1);
	}

	if (xflags[TEXT('g')])
		icon = CreateGrayscaleIcon(icon);

	if (!icon)
		throw Dcx::dcxException("dcxLoadIcon: Unable to create icon");

	return icon;
}

HBITMAP dcxLoadBitmap(HBITMAP dest, TString& filename)
{
	filename.trim();

	if (dest)
	{
		DeleteObject(dest);
		dest = nullptr;
	}
	if (!IsFile(filename))
	{
		Dcx::error(TEXT("dcxLoadBitmap"), TEXT("Could Not Access File: %"), filename);
		return nullptr;
	}

#ifdef DCX_USE_GDIPLUS
	if (Dcx::GDIModule.isUseable())
	{
		//const auto p_Img = std::make_unique<Gdiplus::Bitmap>(filename.to_wchr());
		//
		//// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
		//if (auto status = p_Img->GetLastStatus(); status != Gdiplus::Status::Ok)
		//	Dcx::error(TEXT("dcxLoadBitmap"), GetLastStatusStr(status));
		//else {
		//	status = p_Img->GetHBITMAP(Gdiplus::Color(), &dest);
		//	if (status != Gdiplus::Status::Ok)
		//	{
		//		Dcx::error(TEXT("dcxLoadBitmap"), TEXT("Unable to Get GDI+ Bitmap Info"));
		//		Dcx::error(TEXT("dcxLoadBitmap"), GetLastStatusStr(status));
		//		dest = nullptr;
		//	}
		//	GdiFlush();
		//}

		Gdiplus::Bitmap img(filename.to_wchr());

		// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
		if (auto status = img.GetLastStatus(); status != Gdiplus::Status::Ok)
			Dcx::error(TEXT("dcxLoadBitmap"), GetLastStatusStr(status));
		else {
			status = img.GetHBITMAP(Gdiplus::Color(), &dest);
			if (status != Gdiplus::Status::Ok)
			{
				Dcx::error(TEXT("dcxLoadBitmap"), TEXT("Unable to Get GDI+ Bitmap Info"));
				Dcx::error(TEXT("dcxLoadBitmap"), GetLastStatusStr(status));
				dest = nullptr;
			}
			GdiFlush();
		}
	}
	else
		dest = static_cast<HBITMAP>(LoadImage(GetModuleHandle(nullptr), filename.to_chr(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
#else
	dest = static_cast<HBITMAP>(LoadImage(GetModuleHandle(nullptr), filename.to_chr(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
#endif
	return dest;
}

/*
NOTE: Still experimental
from
http://www.codeproject.com/bitmap/creategrayscaleicon.asp
update for xp icons
http://www.codeguru.com/cpp/g-m/bitmap/icons/comments.php/c4913/?thread=54904
further readings
- http://msdn.microsoft.com/library/default.asp?url=/library/en-us/gdi/bitmaps_233i.asp
update for 32bpp icons & rewrite
- http://www.codeproject.com/bitmap/Create_GrayscaleIcon.asp
*/
// This function creates a grayscale icon starting from a given icon.
// The resulting icon will have the same size of the original one.
//
// Parameters:
//      [IN]    hIcon
//              Handle to the original icon.
//
// Return value:
//      If the function succeeds, the return value is the handle to 
//      the newly created grayscale icon.
//      If the function fails, the return value is nullptr.
//
//HICON CreateGrayscaleIcon(HICON hIcon) {
//	HICON		hGrayIcon = nullptr;
//	HDC		hMainDC = nullptr, hMemDC1 = nullptr, hMemDC2 = nullptr;
//	BITMAP	bmp;
//	HBITMAP	hOldBmp1 = nullptr, hOldBmp2 = nullptr;
//	ICONINFO	csII, csGrayII;
//	//BOOL		bRetValue = FALSE;
//
//	if (!GetIconInfo(hIcon, &csII))
//		return nullptr;
//
//	ZeroMemory(&csGrayII, sizeof(ICONINFO));
//
//	hMainDC = GetDC(nullptr);
//	hMemDC1 = CreateCompatibleDC(hMainDC);
//	hMemDC2 = CreateCompatibleDC(hMainDC);
//
//	if (hMainDC == nullptr || hMemDC1 == nullptr || hMemDC2 == nullptr)
//		return nullptr;
//
//	if (GetObject(csII.hbmColor, sizeof(BITMAP), &bmp)) {
//		DWORD dwWidth = csII.xHotspot*2;
//		DWORD dwHeight = csII.yHotspot*2;
//
//		csGrayII.hbmColor = CreateBitmap(dwWidth, dwHeight, bmp.bmPlanes, bmp.bmBitsPixel, nullptr);
//			HBITMAP tmp = csII.hbmMask;
//
//		if (csGrayII.hbmColor) {
//			hOldBmp1 = (HBITMAP) SelectObject(hMemDC1, csII.hbmColor);
//			hOldBmp2 = (HBITMAP) SelectObject(hMemDC2, csGrayII.hbmColor);
//
//
//			//::BitBlt(hMemDC2, 0, 0, dwWidth, dwHeight, hMemDC1, 0, 0, SRCCOPY);
//
//			DWORD		dwLoopY = 0, dwLoopX = 0;
//			COLORREF	crPixel = 0;
//			COLORREF	byNewPixel = 0;
//
//			for (dwLoopY = 0; dwLoopY < dwHeight; dwLoopY++) {
//				for (dwLoopX = 0; dwLoopX < dwWidth; dwLoopX++) {
//					crPixel = GetPixel(hMemDC1, dwLoopX, dwLoopY);
//
//					byNewPixel = (COLORREF)((GetRValue(crPixel) * 0.299) + 
//						(GetGValue(crPixel) * 0.587) +
//						(GetBValue(crPixel) * 0.114));
//					if (crPixel) // copies alpha if any. (needs testing)
//						SetPixel(hMemDC2, dwLoopX, dwLoopY, RGB(byNewPixel, byNewPixel, byNewPixel) | (crPixel & 0xFF000000));
//				} // for
//			} // for
//
//			SelectObject(hMemDC1, hOldBmp1);
//			SelectObject(hMemDC2, hOldBmp2);
//
//			//csGrayII.hbmMask = csII.hbmMask;
//			csGrayII.hbmMask = tmp;
//			csGrayII.fIcon = TRUE;
//
//			hGrayIcon = ::CreateIconIndirect(&csGrayII);
//			//hGrayIcon = ::CreateIconIndirect(&csII);
//		} // if
//
//		DeleteObject(csGrayII.hbmColor);
//		//::DeleteObject(csGrayII.hbmMask);
//	} // if
//
//	DeleteObject(csII.hbmColor);
//	DeleteObject(csII.hbmMask);
//	DeleteDC(hMemDC1);
//	DeleteDC(hMemDC2);
//	ReleaseDC(nullptr, hMainDC);
//
//	DestroyIcon(hIcon);
//
//	return hGrayIcon;
//} // End of CreateGrayscaleIcon

//COLORREF defaultGrayPalette[256];
//bool bGrayPaletteSet = false;

// Taken from: https://www.codespeedy.com/make-a-grayscale-image-in-cpp-using-opencv/
//#include<opencv2/opencv.hpp>
//using namespace cv;
//int main()
//{
//	string file_name = "D/bcup/DSC_0195.jpg";//all these three strings are part of CV and not the C++ library
//	cv::string window_original = "original_image";
//	cv::string window_gray = "gray_image";
//	cv::mat img = cv::imread(file_name);//It returns a matrix object
//
//	cv::mat graymat;
//	cvtcolor(img.graymat, cv::COLOR_BGR2GRAY);
//	cv::namdwindiw(window_original, cv::WINDOW_AUTOSIZE);
//	cv::imshow(window_original, img);
//	cv::namedwindow(windwindow_gray, cv::WINDOW_AUTOSIZE);
//	cv::imshow(windwindow_gray, graymat);
//	cv::waitkey(0);
//	return 0;
//}

HICON CreateGrayscaleIcon(HICON hIcon) noexcept
{
	if (!hIcon)
		return nullptr;

	//if (!bGrayPaletteSet)
	//{
	//	//#pragma loop(hint_parallel(2))
	//	for(auto i = 0U; i < Dcx::countof(defaultGrayPalette); ++i)
	//	{
	//		defaultGrayPalette[i] = RGB(255-i, 255-i, 255-i);
	//	}
	//
	//	bGrayPaletteSet = true;
	//}

	const static COLORREF defaultGrayPalette[256] = {
		RGB(255 - 0, 255 - 0, 255 - 0), RGB(255 - 1, 255 - 1, 255 - 1), RGB(255 - 2, 255 - 2, 255 - 2), RGB(255 - 3, 255 - 3, 255 - 3),
		RGB(255 - 4, 255 - 4, 255 - 4), RGB(255 - 5, 255 - 5, 255 - 5), RGB(255 - 6, 255 - 6, 255 - 6), RGB(255 - 7, 255 - 7, 255 - 7),
		RGB(255 - 8, 255 - 8, 255 - 8), RGB(255 - 9, 255 - 9, 255 - 9), RGB(255 - 10, 255 - 10, 255 - 10), RGB(255 - 11, 255 - 11, 255 - 11),
		RGB(255 - 12, 255 - 12, 255 - 12), RGB(255 - 13, 255 - 13, 255 - 13), RGB(255 - 14, 255 - 14, 255 - 14), RGB(255 - 15, 255 - 15, 255 - 15),
		RGB(255 - 16, 255 - 16, 255 - 16), RGB(255 - 17, 255 - 17, 255 - 17), RGB(255 - 18, 255 - 18, 255 - 18), RGB(255 - 19, 255 - 19, 255 - 19),
		RGB(255 - 20, 255 - 20, 255 - 20), RGB(255 - 21, 255 - 21, 255 - 21), RGB(255 - 22, 255 - 22, 255 - 22), RGB(255 - 23, 255 - 23, 255 - 23),
		RGB(255 - 24, 255 - 24, 255 - 24), RGB(255 - 25, 255 - 25, 255 - 25), RGB(255 - 26, 255 - 26, 255 - 26), RGB(255 - 27, 255 - 27, 255 - 27),
		RGB(255 - 28, 255 - 28, 255 - 28), RGB(255 - 29, 255 - 29, 255 - 29), RGB(255 - 30, 255 - 30, 255 - 30), RGB(255 - 31, 255 - 31, 255 - 31),
		RGB(255 - 32, 255 - 32, 255 - 32), RGB(255 - 33, 255 - 33, 255 - 33), RGB(255 - 34, 255 - 34, 255 - 34), RGB(255 - 35, 255 - 35, 255 - 35),
		RGB(255 - 36, 255 - 36, 255 - 36), RGB(255 - 37, 255 - 37, 255 - 37), RGB(255 - 38, 255 - 38, 255 - 38), RGB(255 - 39, 255 - 39, 255 - 39),
		RGB(255 - 40, 255 - 40, 255 - 40), RGB(255 - 41, 255 - 41, 255 - 41), RGB(255 - 42, 255 - 42, 255 - 42), RGB(255 - 43, 255 - 43, 255 - 43),
		RGB(255 - 44, 255 - 44, 255 - 44), RGB(255 - 45, 255 - 45, 255 - 45), RGB(255 - 46, 255 - 46, 255 - 46), RGB(255 - 47, 255 - 47, 255 - 47),
		RGB(255 - 48, 255 - 48, 255 - 48), RGB(255 - 49, 255 - 49, 255 - 49), RGB(255 - 50, 255 - 50, 255 - 50), RGB(255 - 51, 255 - 51, 255 - 51),
		RGB(255 - 52, 255 - 52, 255 - 52), RGB(255 - 53, 255 - 53, 255 - 53), RGB(255 - 54, 255 - 54, 255 - 54), RGB(255 - 55, 255 - 55, 255 - 55),
		RGB(255 - 56, 255 - 56, 255 - 56), RGB(255 - 57, 255 - 57, 255 - 57), RGB(255 - 58, 255 - 58, 255 - 58), RGB(255 - 59, 255 - 59, 255 - 59),
		RGB(255 - 60, 255 - 60, 255 - 60), RGB(255 - 61, 255 - 61, 255 - 61), RGB(255 - 62, 255 - 62, 255 - 62), RGB(255 - 63, 255 - 63, 255 - 63),
		RGB(255 - 64, 255 - 64, 255 - 64), RGB(255 - 65, 255 - 65, 255 - 65), RGB(255 - 66, 255 - 66, 255 - 66), RGB(255 - 67, 255 - 67, 255 - 67),
		RGB(255 - 68, 255 - 68, 255 - 68), RGB(255 - 69, 255 - 69, 255 - 69), RGB(255 - 70, 255 - 70, 255 - 70), RGB(255 - 71, 255 - 71, 255 - 71),
		RGB(255 - 72, 255 - 72, 255 - 72), RGB(255 - 73, 255 - 73, 255 - 73), RGB(255 - 74, 255 - 74, 255 - 74), RGB(255 - 75, 255 - 75, 255 - 75),
		RGB(255 - 76, 255 - 76, 255 - 76), RGB(255 - 77, 255 - 77, 255 - 77), RGB(255 - 78, 255 - 78, 255 - 78), RGB(255 - 79, 255 - 79, 255 - 79),
		RGB(255 - 80, 255 - 80, 255 - 80), RGB(255 - 81, 255 - 81, 255 - 81), RGB(255 - 82, 255 - 82, 255 - 82), RGB(255 - 83, 255 - 83, 255 - 83),
		RGB(255 - 84, 255 - 84, 255 - 84), RGB(255 - 85, 255 - 85, 255 - 85), RGB(255 - 86, 255 - 86, 255 - 86), RGB(255 - 87, 255 - 87, 255 - 87),
		RGB(255 - 88, 255 - 88, 255 - 88), RGB(255 - 89, 255 - 89, 255 - 89), RGB(255 - 90, 255 - 90, 255 - 90), RGB(255 - 91, 255 - 91, 255 - 91),
		RGB(255 - 92, 255 - 92, 255 - 92), RGB(255 - 93, 255 - 93, 255 - 93), RGB(255 - 94, 255 - 94, 255 - 94), RGB(255 - 95, 255 - 95, 255 - 95),
		RGB(255 - 96, 255 - 96, 255 - 96), RGB(255 - 97, 255 - 97, 255 - 97), RGB(255 - 98, 255 - 98, 255 - 98), RGB(255 - 99, 255 - 99, 255 - 99),
		RGB(255 - 100, 255 - 100, 255 - 100), RGB(255 - 101, 255 - 101, 255 - 101), RGB(255 - 102, 255 - 102, 255 - 102), RGB(255 - 103, 255 - 103, 255 - 103),
		RGB(255 - 104, 255 - 104, 255 - 104), RGB(255 - 105, 255 - 105, 255 - 105), RGB(255 - 106, 255 - 106, 255 - 106), RGB(255 - 107, 255 - 107, 255 - 107),
		RGB(255 - 108, 255 - 108, 255 - 108), RGB(255 - 109, 255 - 109, 255 - 109), RGB(255 - 110, 255 - 110, 255 - 110), RGB(255 - 111, 255 - 111, 255 - 111),
		RGB(255 - 112, 255 - 112, 255 - 112), RGB(255 - 113, 255 - 113, 255 - 113), RGB(255 - 114, 255 - 114, 255 - 114), RGB(255 - 115, 255 - 115, 255 - 115),
		RGB(255 - 116, 255 - 116, 255 - 116), RGB(255 - 117, 255 - 117, 255 - 117), RGB(255 - 118, 255 - 118, 255 - 118), RGB(255 - 119, 255 - 119, 255 - 119),
		RGB(255 - 120, 255 - 120, 255 - 120), RGB(255 - 121, 255 - 121, 255 - 121), RGB(255 - 122, 255 - 122, 255 - 122), RGB(255 - 123, 255 - 123, 255 - 123),
		RGB(255 - 124, 255 - 124, 255 - 124), RGB(255 - 125, 255 - 125, 255 - 125), RGB(255 - 126, 255 - 126, 255 - 126), RGB(255 - 127, 255 - 127, 255 - 127),
		RGB(255 - 128, 255 - 128, 255 - 128), RGB(255 - 129, 255 - 129, 255 - 129), RGB(255 - 130, 255 - 130, 255 - 130), RGB(255 - 131, 255 - 131, 255 - 131),
		RGB(255 - 132, 255 - 132, 255 - 132), RGB(255 - 133, 255 - 133, 255 - 133), RGB(255 - 134, 255 - 134, 255 - 134), RGB(255 - 135, 255 - 135, 255 - 135),
		RGB(255 - 136, 255 - 136, 255 - 136), RGB(255 - 137, 255 - 137, 255 - 137), RGB(255 - 138, 255 - 138, 255 - 138), RGB(255 - 139, 255 - 139, 255 - 139),
		RGB(255 - 140, 255 - 140, 255 - 140), RGB(255 - 141, 255 - 141, 255 - 141), RGB(255 - 142, 255 - 142, 255 - 142), RGB(255 - 143, 255 - 143, 255 - 143),
		RGB(255 - 144, 255 - 144, 255 - 144), RGB(255 - 145, 255 - 145, 255 - 145), RGB(255 - 146, 255 - 146, 255 - 146), RGB(255 - 147, 255 - 147, 255 - 147),
		RGB(255 - 148, 255 - 148, 255 - 148), RGB(255 - 149, 255 - 149, 255 - 149), RGB(255 - 150, 255 - 150, 255 - 150), RGB(255 - 151, 255 - 151, 255 - 151),
		RGB(255 - 152, 255 - 152, 255 - 152), RGB(255 - 153, 255 - 153, 255 - 153), RGB(255 - 154, 255 - 154, 255 - 154), RGB(255 - 155, 255 - 155, 255 - 155),
		RGB(255 - 156, 255 - 156, 255 - 156), RGB(255 - 157, 255 - 157, 255 - 157), RGB(255 - 158, 255 - 158, 255 - 158), RGB(255 - 159, 255 - 159, 255 - 159),
		RGB(255 - 160, 255 - 160, 255 - 160), RGB(255 - 161, 255 - 161, 255 - 161), RGB(255 - 162, 255 - 162, 255 - 162), RGB(255 - 163, 255 - 163, 255 - 163),
		RGB(255 - 164, 255 - 164, 255 - 164), RGB(255 - 165, 255 - 165, 255 - 165), RGB(255 - 166, 255 - 166, 255 - 166), RGB(255 - 167, 255 - 167, 255 - 167),
		RGB(255 - 168, 255 - 168, 255 - 168), RGB(255 - 169, 255 - 169, 255 - 169), RGB(255 - 170, 255 - 170, 255 - 170), RGB(255 - 171, 255 - 171, 255 - 171),
		RGB(255 - 172, 255 - 172, 255 - 172), RGB(255 - 173, 255 - 173, 255 - 173), RGB(255 - 174, 255 - 174, 255 - 174), RGB(255 - 175, 255 - 175, 255 - 175),
		RGB(255 - 176, 255 - 176, 255 - 176), RGB(255 - 177, 255 - 177, 255 - 177), RGB(255 - 178, 255 - 178, 255 - 178), RGB(255 - 179, 255 - 179, 255 - 179),
		RGB(255 - 180, 255 - 180, 255 - 180), RGB(255 - 181, 255 - 181, 255 - 181), RGB(255 - 182, 255 - 182, 255 - 182), RGB(255 - 183, 255 - 183, 255 - 183),
		RGB(255 - 184, 255 - 184, 255 - 184), RGB(255 - 185, 255 - 185, 255 - 185), RGB(255 - 186, 255 - 186, 255 - 186), RGB(255 - 187, 255 - 187, 255 - 187),
		RGB(255 - 188, 255 - 188, 255 - 188), RGB(255 - 189, 255 - 189, 255 - 189), RGB(255 - 190, 255 - 190, 255 - 190), RGB(255 - 191, 255 - 139, 255 - 191),
		RGB(255 - 192, 255 - 192, 255 - 192), RGB(255 - 193, 255 - 193, 255 - 193), RGB(255 - 194, 255 - 194, 255 - 194), RGB(255 - 195, 255 - 195, 255 - 195),
		RGB(255 - 196, 255 - 196, 255 - 196), RGB(255 - 197, 255 - 197, 255 - 197), RGB(255 - 198, 255 - 198, 255 - 198), RGB(255 - 199, 255 - 199, 255 - 199),
		RGB(255 - 200, 255 - 200, 255 - 200), RGB(255 - 201, 255 - 201, 255 - 201), RGB(255 - 202, 255 - 202, 255 - 202), RGB(255 - 203, 255 - 203, 255 - 203),
		RGB(255 - 204, 255 - 204, 255 - 204), RGB(255 - 205, 255 - 205, 255 - 205), RGB(255 - 206, 255 - 206, 255 - 206), RGB(255 - 207, 255 - 207, 255 - 207),
		RGB(255 - 208, 255 - 208, 255 - 208), RGB(255 - 209, 255 - 209, 255 - 209), RGB(255 - 210, 255 - 210, 255 - 210), RGB(255 - 211, 255 - 211, 255 - 211),
		RGB(255 - 212, 255 - 212, 255 - 212), RGB(255 - 213, 255 - 213, 255 - 213), RGB(255 - 214, 255 - 214, 255 - 214), RGB(255 - 215, 255 - 215, 255 - 215),
		RGB(255 - 216, 255 - 216, 255 - 216), RGB(255 - 217, 255 - 217, 255 - 217), RGB(255 - 218, 255 - 218, 255 - 218), RGB(255 - 219, 255 - 219, 255 - 219),
		RGB(255 - 220, 255 - 220, 255 - 220), RGB(255 - 221, 255 - 221, 255 - 221), RGB(255 - 222, 255 - 222, 255 - 222), RGB(255 - 223, 255 - 223, 255 - 223),
		RGB(255 - 224, 255 - 224, 255 - 224), RGB(255 - 225, 255 - 225, 255 - 225), RGB(255 - 226, 255 - 226, 255 - 226), RGB(255 - 227, 255 - 227, 255 - 227),
		RGB(255 - 228, 255 - 228, 255 - 228), RGB(255 - 229, 255 - 229, 255 - 229), RGB(255 - 230, 255 - 230, 255 - 230), RGB(255 - 231, 255 - 231, 255 - 231),
		RGB(255 - 232, 255 - 232, 255 - 232), RGB(255 - 233, 255 - 233, 255 - 233), RGB(255 - 234, 255 - 234, 255 - 234), RGB(255 - 235, 255 - 235, 255 - 235),
		RGB(255 - 236, 255 - 236, 255 - 236), RGB(255 - 237, 255 - 237, 255 - 237), RGB(255 - 238, 255 - 238, 255 - 238), RGB(255 - 239, 255 - 239, 255 - 239),
		RGB(255 - 240, 255 - 240, 255 - 240), RGB(255 - 241, 255 - 241, 255 - 241), RGB(255 - 242, 255 - 242, 255 - 242), RGB(255 - 243, 255 - 243, 255 - 243),
		RGB(255 - 244, 255 - 244, 255 - 244), RGB(255 - 245, 255 - 245, 255 - 245), RGB(255 - 246, 255 - 246, 255 - 246), RGB(255 - 247, 255 - 247, 255 - 247),
		RGB(255 - 248, 255 - 248, 255 - 248), RGB(255 - 249, 255 - 249, 255 - 249), RGB(255 - 250, 255 - 250, 255 - 250), RGB(255 - 251, 255 - 251, 255 - 251),
		RGB(255 - 252, 255 - 252, 255 - 252), RGB(255 - 253, 255 - 253, 255 - 253), RGB(255 - 254, 255 - 254, 255 - 254), RGB(255 - 255, 255 - 255, 255 - 255)
	};

	return CreateGrayscaleIcon(hIcon, &defaultGrayPalette[0]);
}

GSL_SUPPRESS(type.4)
void AddToolTipToolInfo(const HWND tiphwnd, const HWND ctrl) noexcept
{
	TOOLINFO ti{};
	ti.cbSize = sizeof(TOOLINFO);
	//ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_PARSELINKS;
	ti.uFlags = TTF_IDISHWND | TTF_TRANSPARENT | TTF_SUBCLASS | TTF_PARSELINKS;
	ti.hwnd = ctrl;
	ti.uId = reinterpret_cast<UINT_PTR>(ctrl);
	ti.lpszText = LPSTR_TEXTCALLBACK;

	SendMessage(tiphwnd, TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&ti));
}

void dcxDrawShadowText(HDC hdc, LPCWSTR pszText, UINT cch, RECT* pRect, DWORD dwFlags, COLORREF crText, COLORREF crShadow, int ixOffset, int iyOffset) noexcept
{
	if ((!hdc) || (!pszText) || (!pRect))
		return;

	if (DrawShadowText(hdc, pszText, cch, pRect, dwFlags, crText, crShadow, ixOffset, iyOffset) == 0)
	{
		if (dcx_testflag(dwFlags, DT_CALCRECT))
		{
			DrawTextW(hdc, pszText, gsl::narrow_cast<int>(cch), pRect, dwFlags);
			OffsetRect(pRect, ixOffset, iyOffset);
		}
		else {
			// Normal DrawShadow Failed...
			RECT rcOffset = *pRect;
			OffsetRect(&rcOffset, ixOffset, iyOffset);

			const auto old_clr = SetTextColor(hdc, crShadow);
			Auto(SetTextColor(hdc, old_clr));

			DrawTextW(hdc, pszText, gsl::narrow_cast<int>(cch), &rcOffset, dwFlags);
			SetTextColor(hdc, crText);
			DrawTextW(hdc, pszText, gsl::narrow_cast<int>(cch), pRect, dwFlags);
		}
	}
}

#ifdef DCX_USE_GDIPLUS
static const TCHAR* GDIErrors[] = {
	TEXT("Ok"), // No Error.
	TEXT("GenericError"),
	TEXT("InvalidParameter"),
	TEXT("OutOfMemory"),
	TEXT("ObjectBusy"),
	TEXT("InsufficientBuffer"),
	TEXT("NotImplemented"),
	TEXT("Win32Error"),
	TEXT("WrongState"),
	TEXT("Aborted"),
	TEXT("FileNotFound"),
	TEXT("ValueOverflow"),
	TEXT("AccessDenied"),
	TEXT("UnknownImageFormat"),
	TEXT("FontFamilyNotFound"),
	TEXT("FontStyleNotFound"),
	TEXT("NotTrueTypeFont"),
	TEXT("UnsupportedGdiplusVersion"),
	TEXT("GdiplusNotInitialized"),
	TEXT("PropertyNotFound"),
	TEXT("PropertyNotSupported"),
	TEXT("ProfileNotFound")
};
constexpr const TCHAR* GetLastStatusStr(Gdiplus::Status status) noexcept
{
	if (gsl::narrow_cast<UINT>(status) >= std::size(GDIErrors))
		return GDIErrors[1]; // status not in table, return GenericError
	return gsl::at(GDIErrors, gsl::narrow_cast<UINT>(status));
}
#endif

/*
 * Tell if a file exists.
 * NB: What it really does is tell us if we can access the file, it can fail to get access for various reason on XP+.
 * After a false return use GetLastError() to find the actual fail reason if you care.
*/
bool IsFile(TString& filename)
{
	PathUnquoteSpaces(filename.to_chr()); // Removes any "" around the path.

	if (filename.empty())
		return false;

	// try & access the filename as is first.
	auto hFile = CreateFile(filename.to_chr(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	Auto(CloseHandle(hFile));

	if (hFile != INVALID_HANDLE_VALUE)
		return true;

	filename.strip(); // remove ctrl codes from name. this allows ctrl codes to be added to a filename that uses double spaces.

	hFile = CreateFile(filename.to_chr(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile != INVALID_HANDLE_VALUE)
		return true;

	// if that fails try & search for the file.

	// find buffer size needed.
	if (auto res = SearchPath(nullptr, filename.to_chr(), nullptr, 0, nullptr, nullptr); res > 0)
	{
		// found file, alloc buffer & fill with path/file.
#if DCX_USE_WRAPPERS
		Dcx::dcxStringResource buf(res + 1U);
#else
		stString<res + 1U> buf;
#endif
		res = SearchPath(nullptr, filename.to_chr(), nullptr, res, buf.get(), nullptr);

		if (res == 0) // if find failed, exit
			return false;

		// now try & access the file we found.
		hFile = CreateFile(buf.get(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			filename = buf.get(); // alter the filename to contain the full path.
			return true;
		}
	}
	return false;
}

// mIRC Colours, only the first 16 of these are changable.
//COLORREF staticPalette[mIRC_PALETTE_SIZE] = {
//	CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID,
//	CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID,
//	0x470000, 0x472100, 0x474700, 0x324700, 0x004700, 0x00472c, 0x004747, 0x002747, 0x000047, 0x2e0047, 0x470047,
//	0x47002a, 0x740000, 0x743a00, 0x747400, 0x517400, 0x007400, 0x007449, 0x007474, 0x004074, 0x000074, 0x4b0074,
//	0x740074, 0x740045, 0xb50000, 0xb56300, 0xb5b500, 0x7db500, 0x00b500, 0x00b571, 0x00b5b5, 0x0063b5, 0x0000b5,
//	0x7500b5, 0xb500b5, 0xb5006b, 0xff0000, 0xff8c00, 0xffff00, 0xb2ff00, 0x00ff00, 0x00ffa0, 0x00ffff, 0x008cff,
//	0x0000ff, 0xa500ff, 0xff00ff, 0xff0098, 0xff5959, 0xffb459, 0xffff71, 0xcfff60, 0x6fff6f, 0x65ffc9, 0x6dffff,
//	0x59b4ff, 0x5959ff, 0xc459ff, 0xff66ff, 0xff59bc, 0xff9c9c, 0xffd39c, 0xffff9c, 0xe2ff9c, 0x9cff9c, 0x9cffdb,
//	0x9cffff, 0x9cd3ff, 0x9c9cff, 0xdc9cff, 0xff9cff, 0xff94d3, 0x000000, 0x131313, 0x282828, 0x363636, 0x4d4d4d,
//	0x656565, 0x818181, 0x9f9f9f, 0xbcbcbc, 0xe2e2e2, 0xffffff
//};

// mIRC Colours, only the first 16 of these are changable.
COLORREF staticPalette[mIRC_PALETTE_SIZE] = {
	CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID,
	CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID,
	0x00000047 ,0x00002147 ,0x00004747 ,0x00004732 ,0x00004700 ,0x002C4700 ,0x00474700 ,0x00472700 ,0x00470000,
	0x0047002E ,0x00470047 ,0x002A0047 ,0x00000074 ,0x00003A74 ,0x00007474 ,0x00007451 ,0x00007400 ,0x00497400,
	0x00747400 ,0x00744000 ,0x00740000 ,0x0074004B ,0x00740074 ,0x00450074 ,0x000000B5 ,0x000063B5 ,0x0000B5B5,
	0x0000B57D ,0x0000B500 ,0x0071B500 ,0x00B5B500 ,0x00B56300 ,0x00B50000 ,0x00B50075 ,0x00B500B5 ,0x006B00B5,
	0x000000FF ,0x00008CFF ,0x0000FFFF ,0x0000FFB2 ,0x0000FF00 ,0x00A0FF00 ,0x00FFFF00 ,0x00FF8C00 ,0x00FF0000,
	0x00FF00A5 ,0x00FF00FF ,0x009800FF ,0x005959FF ,0x0059B4FF ,0x0071FFFF ,0x0060FFCF ,0x006FFF6F ,0x00C9FF65,
	0x00FFFF6D ,0x00FFB459 ,0x00FF5959 ,0x00FF59C4 ,0x00FF66FF ,0x00BC59FF ,0x009C9CFF ,0x009CD3FF ,0x009CFFFF,
	0x009CFFE2 ,0x009CFF9C ,0x00DBFF9C ,0x00FFFF9C ,0x00FFD39C ,0x00FF9C9C ,0x00FF9CDC ,0x00FF9CFF ,0x00D394FF,
	0x00000000 ,0x00131313 ,0x00282828 ,0x00363636 ,0x004D4D4D ,0x00656565 ,0x00818181 ,0x009F9F9F ,0x00BCBCBC,
	0x00E2E2E2 ,0x00FFFFFF ,0x00000000 
};

/*!
* \brief Get all mIRC Colours.
*
* If staticPalette[0] == CLR_INVALID - Fills the staticPalette with all mIRC's colours.
* otherwise does nothing.
*/
void getmIRCPalette(bool bForce)
{
	// if forced update set or staticPalette hasnt been filled, or static colours disabled...
	if ((bForce) || (staticPalette[0] == CLR_INVALID) || (!Dcx::setting_bStaticColours))
	{
		// max length = (number of colours * max colour string result) + spaces between colours + zero char
		// max result length = (16 * 10) + 15 + 1 = 160 + 16 = 176

		//TString colors;
		//static constexpr TCHAR com[] = TEXT("$color(0) $color(1) $color(2) $color(3) $color(4) $color(5) $color(6) $color(7) $color(8) $color(9) $color(10) $color(11) $color(12) $color(13) $color(14) $color(15)");
		//mIRCLinker::eval(colors, std::addressof(com[0]));
		//size_t i{};
		//for (const auto& a : colors)
		//	gsl::at(staticPalette, i++) = a.to_<COLORREF>();

		TString colors(gsl::narrow_cast<TString::size_type>(180U));
		static constexpr TCHAR com[] = TEXT("$color(0) $color(1) $color(2) $color(3) $color(4) $color(5) $color(6) $color(7) $color(8) $color(9) $color(10) $color(11) $color(12) $color(13) $color(14) $color(15)");
		mIRCLinker::eval(colors, std::addressof(com[0]));

		size_t i{};

		for (const auto& a : colors)
			gsl::at(staticPalette, i++) = a.to_<COLORREF>();
	}
}

/*!
* \brief Get mIRC Colours.
*
* Copies the staticPalette to Palette (updates staticPalette if needed)
*/
void getmIRCPalette(COLORREF* const Palette, const UINT PaletteItems, bool bForce)
{
	if ((PaletteItems > mIRC_PALETTE_SIZE) || (Palette == std::addressof(staticPalette[0])))
		return;

	getmIRCPalette(bForce);

	CopyMemory(Palette, std::addressof(staticPalette[0]), sizeof(COLORREF) * PaletteItems);
}

/*!
* \brief Get mIRC Colours.
*
* Copies the staticPalette to Palette (updates staticPalette if needed)
*/
void getmIRCPalette(gsl::span<COLORREF> Palette, bool bForce)
{
	getmIRCPalette(bForce);

	gsl::copy(gsl::span<COLORREF>(staticPalette), Palette);
}

// void getmIRCPaletteMask(COLORREF *const Palette, const UINT PaletteItems, uint16_t uMask)
// Palette      - Palette to fill
// PaletteItems - Total number of items in Palette
// uMask        - bitmask of items to get.
//
// Copies staticPalette to Palette based on a 16bit mask, one bit for every colour (updates staticPalette if needed)
// NB: This ONLY updates the first 16 colours, not the full 99, > 16 are static anyway...
void getmIRCPaletteMask(COLORREF* const Palette, const UINT PaletteItems, uint16_t uMask, bool bForce)
{
	if ((!Palette) || (PaletteItems < 16) || (PaletteItems > std::size(staticPalette)))
		return;

	getmIRCPalette(bForce);

	for (uint32_t count = 0; count < PaletteItems; ++count)
	{
		if ((uMask & 0x01) != 0)
		{
			Palette[count] = gsl::at(staticPalette, count);
		}
		uMask >>= 1;
	}
}

int unfoldColor(const WCHAR* color) noexcept
{
	return unfoldColor(_ts_atoi(color));
}

/// <summary>
/// Pre proccessed the mIRC text. To be used any time the text changes.
/// </summary>
/// <param name="txt"></param>
/// <param name="len"></param>
/// <returns></returns>
std::vector<dcxTextBreakdown> dcxBreakdownmIRCText(const LPCWSTR txt, UINT len)
{
	std::vector<dcxTextBreakdown> vec;

	// overkill...
	vec.reserve(len);

	const auto wtxt = txt;
	bool usingRevTxt = false;
	UINT pos = 0;

	constexpr COLORREF origFG = CLR_INVALID /*RGB(0,0,0)*/, origBG = CLR_INVALID /*RGB(255,255,255)*/;
	COLORREF cPalette[mIRC_PALETTE_SIZE] = { CLR_INVALID }; // mIRC palette

	getmIRCPalette(gsl::span<COLORREF>(cPalette), false); // get mIRC palette

	dcxTextBreakdown tmp;

	for (auto c = wtxt[pos]; pos < len; c = wtxt[++pos])
	{
		switch (c)
		{
		case 2: // Bold
		{
			if (!tmp.m_str.empty())
			{
				vec.emplace_back(tmp);
				tmp.m_str.clear();
			}
			if (tmp.m_log.lfWeight == FW_BOLD)
				tmp.m_log.lfWeight = FW_NORMAL;
			else
				tmp.m_log.lfWeight = FW_BOLD;
		}
		break;
		case 3: // Colour
		{
			if (!tmp.m_str.empty())
			{
				vec.emplace_back(tmp);
				tmp.m_str.clear();
			}

			while (wtxt[pos + 1] == 3)
				++pos; // remove multiple consecutive ctrl-k's

			if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
			{
				{
					UINT umIRCColour = (wtxt[pos + 1] - L'0'); // must be => 0 <= 9
					++pos;

					if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
					{
						umIRCColour *= 10;	// becomes a max of 90
						umIRCColour += (wtxt[pos + 1] - L'0');	// becomes a max of 99
						++pos;
					}

					// color code number
					tmp.m_clrText = gsl::at(cPalette, umIRCColour);
				}

				// maybe a background color
				if (wtxt[pos + 1] == L',')
				{
					++pos;

					if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
					{
						UINT umIRCColour = (wtxt[pos + 1] - L'0'); // must be => 0 <= 9
						++pos;

						if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
						{
							umIRCColour *= 10;	// becomes a max of 90
							umIRCColour += (wtxt[pos + 1] - L'0');	// becomes a max of 99
							++pos;
						}

						// color code number
						tmp.m_clrBack = gsl::at(cPalette, umIRCColour);
						tmp.m_bTransparent = false;
					}
				}
				if (usingRevTxt)
				{ // reverse text swap fg & bg colours
					std::swap(tmp.m_clrText, tmp.m_clrBack);
					tmp.m_bTransparent = false;
				}
			}
			else {
				tmp.m_clrText = origFG;
				tmp.m_clrBack = origBG;
				tmp.m_bTransparent = true;
			}
		}
		break;
		case 15: // ctrl+o cancel all ctrl codes in effect.
		{
			if (!tmp.m_str.empty())
			{
				vec.emplace_back(tmp);
				tmp.m_str.clear();
			}

			while (wtxt[pos + 1] == 15)
				++pos; // remove multiple consecutive ctrl-o's

			tmp.m_log.lfWeight = FW_NORMAL;
			tmp.m_log.lfUnderline = 0;
			tmp.m_log.lfItalic = 0;
			tmp.m_log.lfStrikeOut = 0;
			tmp.m_clrText = origFG;
			tmp.m_clrBack = origBG;
			tmp.m_bTransparent = true;
			usingRevTxt = false;
		}
		break;
		case 22: // ctrl+r reverse
		{
			if (!tmp.m_str.empty())
			{
				vec.emplace_back(tmp);
				tmp.m_str.clear();
			}

			usingRevTxt = (usingRevTxt ? false : true);

			if (usingRevTxt)
				tmp.m_bTransparent = false;
			else
				tmp.m_bTransparent = true;

			std::swap(tmp.m_clrText, tmp.m_clrBack);
		}
		break;
		case 29: // ctrl-i Italics
		{
			if (!tmp.m_str.empty())
			{
				vec.emplace_back(tmp);
				tmp.m_str.clear();
			}
			tmp.m_log.lfItalic = (tmp.m_log.lfItalic ? 0U : 1U);
		}
		break;
		case 30: // ctrl+e strikethrough
		{
			if (!tmp.m_str.empty())
			{
				vec.emplace_back(tmp);
				tmp.m_str.clear();
			}
			tmp.m_log.lfStrikeOut = (tmp.m_log.lfStrikeOut ? 0U : 1U);
		}
		break;
		case 31: // ctrl+u underline
		{
			if (!tmp.m_str.empty())
			{
				vec.emplace_back(tmp);
				tmp.m_str.clear();
			}
			tmp.m_log.lfUnderline = (tmp.m_log.lfUnderline ? 0U : 1U);
		}
		break;
		case 10:
		case 13:
		default: // normal TCHAR
		{
			tmp.m_str += c;
		}
		break;
		}
	}
	if (!tmp.m_str.empty())
		vec.emplace_back(tmp);

	return vec;
}

/// <summary>
/// Pre proccessed the mIRC text. To be used any time the text changes.
/// </summary>
/// <param name="txt"></param>
/// <returns></returns>
std::vector<dcxTextBreakdown> dcxBreakdownmIRCText(const TString& txt)
{
	return dcxBreakdownmIRCText(txt.to_wchr(), gsl::narrow_cast<UINT>(txt.len()));
}

RECT dcxBreakdownCalcRect(HDC hdc, const std::vector<dcxTextBreakdown>& vec, LPCRECT rc, const UINT uStyle, const dcxTextOptions& dTO) noexcept
{
	RECT rcResult{ *rc };

	if (!hdc || !rc)
		return rcResult;

	try {
		const UINT uNewStyle = uStyle | DT_CALCRECT;

	TString txt;
	txt.reserve(64 * vec.size());	// reserve 64 characters for each string (should be enuf for most cases)

	for (const auto& tbd : vec)
	{
		txt += tbd.m_str;
	}
		if (txt.empty())
			return rcResult;

		DrawTextW(hdc, txt.to_wchr(), gsl::narrow_cast<int>(txt.len()), &rcResult, uNewStyle);
	// Ook: This is a quick fix for the last letter not always being drawn.
		//rcResult.right++;

		{
		SIZE sz{};
			GetTextExtentPoint32W(hdc, &txt.to_wchr()[txt.len() -1], 1, &sz);
			rcResult.right += sz.cx;
	}

		//SIZE sz{};
		////GetTextExtentExPointW(hdc, txt.to_wchr(), gsl::narrow_cast<int>(txt.len()), 0, nullptr, nullptr, &sz);
		//GetTextExtentPoint32W(hdc, txt.to_wchr(), gsl::narrow_cast<int>(txt.len()), &sz);
		//sz.cx++;
		//rcResult.right = rcResult.left + sz.cx;
		//rcResult.bottom = rcResult.top + sz.cy;
	}
	catch (...) {}
	return rcResult;
}

void mIRC_DrawBreakdown(HDC hdc, const std::vector<dcxTextBreakdown>& vec, LPRECT rc, const UINT uStyle, const dcxTextOptions& dTO) noexcept
{
	if (!hdc || !rc || (vec.empty()) || IsRectEmpty(rc))
		return;

	const RECT rcCalc = dcxBreakdownCalcRect(hdc, vec, rc, uStyle, dTO);
	//const RECT rcCalc = *rc;
	if (dcx_testflag(uStyle, DT_CALCRECT))
	{
		*rc = rcCalc;
		return;
	}
	RECT rcArea = *rc;
	if (dcx_testflag(uStyle, DT_CENTER))
	{
		const auto halfXArea = (rcCalc.right - rcCalc.left) / 2;
		const auto halfXrc = ((rc->right - rc->left) / 2);
		const auto totalX = (halfXrc - halfXArea);
		rcArea.left = rc->left + totalX;
		rcArea.right = rc->right - totalX;
	}
	else if (dcx_testflag(uStyle, DT_RIGHT))	// cant be both center & right
	{
		const auto XArea = (rcCalc.right - rcCalc.left);
		rcArea.left = rc->right - XArea;
	}
	if (dcx_testflag(uStyle, DT_VCENTER))
	{
		const auto halfYArea = (rcCalc.bottom - rcCalc.top) / 2;
		const auto halfYrc = ((rc->bottom - rc->top) / 2);
		const auto totalY = (halfYrc - halfYArea);
		rcArea.top = rc->top + totalY;
		rcArea.bottom = rc->bottom - totalY;
	}
	int xPos{ rcArea.left }, yPos{ rcArea.top };

	auto _DrawBreakdown = [&xPos, &yPos, &uStyle, &rcArea, &hdc, &dTO, &rc](const TString& txt) noexcept -> bool {
		TEXTMETRICW tm{};
		GetTextMetricsW(hdc, std::addressof(tm));

		int iFit{};
		const WCHAR* pText{ txt.to_wchr() };
		const auto len = txt.len();
		//const UINT uNewStyle = uStyle | DT_SINGLELINE;
		const UINT uNewStyle = (uStyle & ~(DT_CENTER | DT_RIGHT | DT_VCENTER)) | DT_LEFT | DT_SINGLELINE; // make sure its to left
		const bool bDoPath = (dTO.m_bFilledOutline || dTO.m_bOutline || dTO.m_bGradientFill);

		//const auto clrTxt = (dTO.m_clrText != CLR_INVALID) ? dTO.m_clrText : GetTextColor(hdc);
		const auto clrTxt = GetTextColor(hdc);
		const auto clrOutline = (dTO.m_clrOutline != CLR_INVALID) ? dTO.m_clrOutline : GetContrastColour(clrTxt);

		if (bDoPath)
			BeginPath(hdc);

		if (xPos >= rcArea.right)
		{
			xPos = rcArea.left;
			yPos += tm.tmHeight;
		}
		for (std::remove_const_t<decltype(len)> iDrawnChars{}; iDrawnChars < len; iDrawnChars += iFit)
		{
			SIZE sz{};
			GetTextExtentExPointW(hdc, pText, gsl::narrow_cast<int>(len - iDrawnChars), (rcArea.right - xPos), &iFit, nullptr, &sz);

			if (iFit != 0)
			{
				RECT rcTxt{};
				rcTxt.top = yPos;
				rcTxt.bottom = yPos + sz.cy;
				rcTxt.left = xPos;
				rcTxt.right = rcArea.right;

				// apply glow effect.
				if (dTO.m_bGlow)
				{
					const auto clrGlow = (dTO.m_clrGlow != CLR_INVALID) ? dTO.m_clrGlow : RGB(255, 255, 255);
					dcxDrawShadowText(hdc, pText, iFit, &rcTxt, uNewStyle, clrTxt, clrGlow, 0, 0);
				}
				else if (dTO.m_bShadow)
				{
					const auto clrShadow = (dTO.m_clrShadow != CLR_INVALID) ? dTO.m_clrShadow : RGB(0, 0, 0);
					dcxDrawShadowText(hdc, pText, iFit, &rcTxt, uNewStyle, clrTxt, clrShadow, dTO.m_uShadowXOffset, dTO.m_uShadowYOffset);
				}
				else
					DrawTextW(hdc, pText, iFit, &rcTxt, uNewStyle);
			}

			if ((iFit + iDrawnChars) < len)
			{
				if (dcx_testflag(uStyle, DT_SINGLELINE))
					return false;
				xPos = rcArea.left;
				yPos += sz.cy;
				pText += iFit;
			}
			else {
				xPos += sz.cx - tm.tmOverhang;
			}
		}

		if (bDoPath)
		{
			EndPath(hdc);

			auto hOutlinePen = CreatePen(PS_SOLID, dTO.m_uOutlineSize, clrOutline);
			if (!hOutlinePen)
				return false;
			Auto(DeletePen(hOutlinePen));

			auto hOldPen = SelectPen(hdc, hOutlinePen);
			Auto(SelectPen(hdc, hOldPen));

			auto hFillBrush = CreateSolidBrush(clrTxt);
			if (!hFillBrush)
				return false;
			Auto(DeleteBrush(hFillBrush));

			auto hOldBrush = SelectBrush(hdc, hFillBrush);
			Auto(SelectBrush(hdc, hOldBrush));

			if (!dTO.m_bGradientFill || dTO.m_bOutline)
			{
				// not a gradient so must be an outline
				if (dTO.m_bFilledOutline)
					StrokeAndFillPath(hdc);
				else
					StrokePath(hdc);
			}
			else {
				if (dTO.m_bGradientOutline)
					WidenPath(hdc);

				// TODO: fix shadow look for gradient and outlines...
				// TODO: get gradient outlines working... mostly done?
				// TODO: look into SelectClipPath()

				if (dTO.m_bFilledOutline)
					StrokeAndFillPath(hdc);
				else if (dTO.m_bOutline)
					StrokePath(hdc);
				else
					FillPath(hdc);

				const auto hBufGrad = CreateHDCBuffer(hdc, rc);
				if (!hBufGrad)
					return false;
				Auto(DeleteHDCBuffer(hBufGrad));
				HDC hdcGrad = *hBufGrad;
				if (!hdcGrad)
					return false;

				// create mask
				auto hbmMask = dcxCreateMask(hdc, rc, clrTxt);
				if (!hbmMask)
					return false;
				Auto(DeleteBitmap(hbmMask));

				XPopupMenuItem::DrawGradient(hdcGrad, rc, dTO.m_clrGradientTextStart, dTO.m_clrGradientTextEnd, dTO.m_bHorizGradientFill);

				if (dTO.m_bGradientOutline)
					StrokePath(hdc);

				//if (dTO.m_bFilledOutline)
				//	StrokePath(hdc);

				//constexpr DWORD DSTCOPY = 0x00AA0029;
				//constexpr DWORD hafs = MAKEROP4(SRCCOPY, DSTCOPY);
				//constexpr DWORD hafs = 0xAACC0000;
				//MaskBlt(hdc, pRC->left, pRC->top, rcTxt.right, rcTxt.bottom, hdcGrad, 0, 0, hbmMask, 0, 0, hafs);

				// these points are the wrong way round, BUT they cause the text to be flipped, which is a nice effect for the future.
				//const POINT pts[3]{ {rcTxt.top,rcTxt.left }, { rcTxt.top, rcTxt.right }, { rcTxt.bottom, rcTxt.left } };

				const POINT pts[3]{ { rc->left,rc->top }, { rc->right,rc->top }, { rc->left,rc->bottom } };
				PlgBlt(hdc, &pts[0], hdcGrad, 0, 0, rc->right, rc->bottom, hbmMask, 0, 0);
			}
		}

		return (iFit != 0);
	};

	auto oldBkMode = GetBkMode(hdc);
	Auto(SetBkMode(hdc, oldBkMode));

	if (dTO.m_bNoCtrlCodes)
	{
		for (const auto& tbd : vec)
		{
			if (tbd.m_str.empty())
				continue;

			if (yPos >= rcArea.bottom)
				break;

			SetBkMode(hdc, (tbd.m_bTransparent) ? TRANSPARENT : OPAQUE);

			if (!_DrawBreakdown(tbd.m_str))
				break;
		}
	}
	else {
		const COLORREF origFG = GetTextColor(hdc), origBG = GetBkColor(hdc);

		auto [code, lf] = Dcx::dcxGetObject<LOGFONT>(Dcx::dcxGetCurrentObject<HFONT>(hdc, OBJ_FONT));
		if (code == 0)
			return;

		for (const auto& tbd : vec)
		{
			if (tbd.m_str.empty())
				continue;

			if (yPos >= rcArea.bottom)
				break;

			lf.lfWeight = tbd.m_log.lfWeight;
			lf.lfItalic = tbd.m_log.lfItalic;
			lf.lfStrikeOut = tbd.m_log.lfStrikeOut;
			lf.lfUnderline = tbd.m_log.lfUnderline;

			if (!dTO.m_bNoColours)
			{
				if (tbd.m_clrText != CLR_INVALID)
					SetTextColor(hdc, tbd.m_clrText);
				else
					SetTextColor(hdc, origFG);

				if (tbd.m_clrBack != CLR_INVALID)
					SetBkColor(hdc, tbd.m_clrBack);
				else
					SetBkColor(hdc, origBG);
			}

			SetBkMode(hdc, (tbd.m_bTransparent) ? TRANSPARENT : OPAQUE);

			auto hNewFont = CreateFontIndirectW(&lf);
			if (!hNewFont)
				return;
			Auto(DeleteObject(hNewFont));

			//Dcx::dcxFontResource ff(&lf);

			const auto hOldFont = SelectObject(hdc, hNewFont);
			Auto(SelectObject(hdc, hOldFont));

			if (!_DrawBreakdown(tbd.m_str))
				break;
		}
		SetTextColor(hdc, origFG);
		SetBkColor(hdc, origBG);
	}
}

/// <summary>
/// Draw text using mIRC's control codes (including colours)
/// </summary>
/// <param name="hdc">- The HDC to draw to.</param>
/// <param name="txt">- The text to draw.</param>
/// <param name="rc">- A RECT defining the draw area.</param>
/// <param name="style">- The DrawText() styles to use.</param>
/// <param name="shadow">- Give the text a shadow?</param>
void mIRC_DrawTextNoBuffer(HDC hdc, const TString& txt, LPRECT rc, const UINT style, const bool shadow, const dcxTextOptions* pdTO)
{
	if (!hdc || !rc || (txt.empty()) || IsRectEmpty(rc))
		return;

	auto rcOut = *rc;

	const COLORREF origFG = GetTextColor(hdc), origBG = GetBkColor(hdc);
	COLORREF clrFG = origFG, clrBG = origBG;
	COLORREF cPalette[mIRC_PALETTE_SIZE] = { CLR_INVALID }; // mIRC palette

	getmIRCPalette(gsl::span<COLORREF>(cPalette), false); // get mIRC palette

	auto [code, lf] = Dcx::dcxGetObject<LOGFONT>(Dcx::dcxGetCurrentObject<HFONT>(hdc, OBJ_FONT));
	if (code == 0)
		return;

	const auto origWeight = lf.lfWeight;
	auto origLeft = rc->left;

	SetBkMode(hdc, TRANSPARENT);

	const UINT iStyle = (style & ~(DT_CENTER | DT_RIGHT | DT_VCENTER)) | DT_LEFT; // make sure its to left

	if (!dcx_testflag(iStyle, DT_CALCRECT))
	{ // if NOT doing DT_CALCRECT calc rect needed here.
		if (dcx_testflag(style, DT_CENTER) || dcx_testflag(style, DT_RIGHT) || dcx_testflag(style, DT_VCENTER))
		{
			// strip out ctrl codes to correctly position text.
			auto rcTmp = *rc;
			auto t(txt);
			if (dcx_testflag(iStyle, DT_SINGLELINE))
			{
				t.mreplace(TEXT(' '), TEXT("\n\r"));
			}
			t.strip();	// removes ctrl codes & double spaces & spaces at start & end.
			DrawTextW(hdc, t.to_wchr(), -1, &rcTmp, style | DT_CALCRECT /*| DT_NOPREFIX*/);	// using t.len() gives invalid length (issue with strip())
			// style can be either center or right, not both, but it can be center+vcenter or right+vcenter
			if (dcx_testflag(style, DT_CENTER))
			{ // get center text start offset
				// (total width) - (required width) / 2 = equal space to each side.
				origLeft = rcOut.left = rcOut.left + (((rcOut.right - rcOut.left) - (rcTmp.right - rcTmp.left)) / 2);
			}
			else if (dcx_testflag(style, DT_RIGHT))
			{ // get right text start offset
				// adjust start to right
				origLeft = rcOut.left = (rcOut.right - (rcTmp.right - rcTmp.left));
			}
			if (dcx_testflag(style, DT_VCENTER))
			{ // get Veritcal center text start offset
				rcOut.top += (((rcOut.bottom - rcOut.top) - (rcTmp.bottom - rcTmp.top)) / 2);
			}
		}
	}

	{
		TString tmp;
		{
			const auto wtxt = txt.to_wchr();
			const auto len = txt.len();
			bool usingRevTxt = false;
			UINT pos = 0;

			for (auto c = wtxt[pos]; pos < len; c = wtxt[++pos])
			{
				switch (c)
				{
				case 2: // Bold
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, rc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
					if (lf.lfWeight == FW_BOLD)
						lf.lfWeight = origWeight;
					else
						lf.lfWeight = FW_BOLD;
				}
				break;
				case 3: // Colour
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, rc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);

					while (wtxt[pos + 1] == 3)
						++pos; // remove multiple consecutive ctrl-k's

					if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
					{
						{
							UINT umIRCColour = (wtxt[pos + 1] - L'0'); // must be => 0 <= 9
							++pos;

							if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
							{
								umIRCColour *= 10;	// becomes a max of 90
								umIRCColour += (wtxt[pos + 1] - L'0');	// becomes a max of 99
								++pos;
							}

							// color code number
							clrFG = gsl::at(cPalette, umIRCColour);
						}

						// maybe a background color
						if (wtxt[pos + 1] == L',')
						{
							++pos;

							if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
							{
								UINT umIRCColour = (wtxt[pos + 1] - L'0'); // must be => 0 <= 9
								++pos;

								if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
								{
									umIRCColour *= 10;	// becomes a max of 90
									umIRCColour += (wtxt[pos + 1] - L'0');	// becomes a max of 99
									++pos;
								}

								// color code number
								clrBG = gsl::at(cPalette, umIRCColour);
								SetBkMode(hdc, OPAQUE);
							}
						}
						if (usingRevTxt)
						{ // reverse text swap fg & bg colours
							std::swap(clrFG, clrBG);
							SetBkMode(hdc, OPAQUE);
						}
					}
					else {
						clrFG = origFG;
						clrBG = origBG;
						SetBkMode(hdc, TRANSPARENT);
					}
					SetTextColor(hdc, clrFG);
					SetBkColor(hdc, clrBG);
				}
				break;
				case 15: // ctrl+o cancel all ctrl codesin effect.
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, rc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);

					while (wtxt[pos + 1] == 15)
						++pos; // remove multiple consecutive ctrl-o's

					lf.lfWeight = origWeight;
					lf.lfUnderline = 0;
					lf.lfItalic = 0;
					lf.lfStrikeOut = 0;
					clrFG = origFG;
					clrBG = origBG;
					SetTextColor(hdc, origFG);
					SetBkColor(hdc, origBG);
					SetBkMode(hdc, TRANSPARENT);
					usingRevTxt = false;
				}
				break;
				case 22: // ctrl+r reverse
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, rc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);

					usingRevTxt = (usingRevTxt ? false : true);

					if (usingRevTxt)
						SetBkMode(hdc, OPAQUE);
					else
						SetBkMode(hdc, TRANSPARENT);

					std::swap(clrFG, clrBG);

					SetTextColor(hdc, clrFG);
					SetBkColor(hdc, clrBG);
				}
				break;
				case 29: // ctrl-i Italics
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, rc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
					lf.lfItalic = (lf.lfItalic ? 0U : 1U);
				}
				break;
				case 30: // ctrl+e strikethrough
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, rc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
					lf.lfStrikeOut = (lf.lfStrikeOut ? 0U : 1U);
				}
				break;
				case 31: // ctrl+u underline
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, rc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
					lf.lfUnderline = (lf.lfUnderline ? 0U : 1U);
				}
				break;
				case 10:
				case 13:
				{
					if (dcx_testflag(iStyle, DT_SINGLELINE))
					{ // when single line, replace with a space or ignore?
						//while ((wtxt[pos+1] == 13) || (wtxt[pos+1] == 10)) pos++; // remove multiple consecutive line feeds (causes exception??)
						tmp += TEXT(' '); //" ";
					}
					else {
						if (const auto tlen = tmp.len(); tlen > 0)
						{
							SIZE sz{};
							GetTextExtentPoint32W(hdc, tmp.to_wchr(), gsl::narrow_cast<int>(tlen), &sz);
							mIRC_OutText(hdc, rc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
							rcOut.top += sz.cy;
						}

						if (dcx_testflag(style, DT_RIGHT))
							rcOut.left = origLeft;
						else
							rcOut.left = rc->left;
					}
				}
				break;
				default: // normal TCHAR
				{
					if (!dcx_testflag(iStyle, DT_SINGLELINE))
					{ // don't bother if a single line.
						if (!tmp.empty())
						{
							SIZE sz{};
							int nFit{};
							const auto tlen = gsl::narrow_cast<int>(tmp.len());
							GetTextExtentExPointW(hdc, txt.to_wchr(), tlen, (rcOut.right - rcOut.left), &nFit, nullptr, &sz);
							if (nFit < tlen)
							{
								if (nFit > 0)
								{
									const auto o = tmp[nFit];
									auto tsSub(tmp.sub(0, nFit));
									mIRC_OutText(hdc, rc, tsSub, &rcOut, &lf, iStyle, clrFG, shadow);
									rcOut.top += sz.cy;
									rcOut.left = origLeft;
									tmp = o;
								}
								else
									tmp.clear();
							}
						}
					}
					tmp += c;
				}
				break;
				}
			}
		}
		if (!tmp.empty())
			mIRC_OutText(hdc, rc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
	}

	if (dcx_testflag(iStyle, DT_CALCRECT))
	{ // calc rect only
		CopyRect(rc, &rcOut);
	}
}

/// <summary>
/// Draw text using mIRC's control codes (including colours)
/// </summary>
/// <param name="hdc">- The HDC to draw to.</param>
/// <param name="txt">- The text to draw.</param>
/// <param name="rc">- A RECT defining the draw area.</param>
/// <param name="style">- The DrawText() styles to use.</param>
/// <param name="shadow">- Give the text a shadow?</param>
void mIRC_DrawText(HDC hdc, const TString& txt, LPRECT rc, const UINT style, const bool shadow)
{
	if (!hdc || !rc || (txt.empty()) || IsRectEmpty(rc))
		return;

	// create an hdc buffer to avoid flicker during drawing.
	const auto hBuffer = CreateHDCBuffer(hdc, rc);

	if (!hBuffer)
		return;

	Auto(DeleteHDCBuffer(hBuffer));

	auto rcOut = *rc;
	auto oldHDC = hdc;
	hdc = *hBuffer;
	// change rcOut to be zero offset.
	OffsetRect(&rcOut, -rcOut.left, -rcOut.top);
	const auto rcBufferBounds = rcOut;

	const COLORREF origFG = GetTextColor(hdc), origBG = GetBkColor(hdc);
	COLORREF clrFG = origFG, clrBG = origBG;
	COLORREF cPalette[mIRC_PALETTE_SIZE] = { CLR_INVALID }; // mIRC palette

	getmIRCPalette(gsl::span<COLORREF>(cPalette), false); // get mIRC palette

	auto [code, lf] = Dcx::dcxGetObject<LOGFONT>(Dcx::dcxGetCurrentObject<HFONT>(hdc, OBJ_FONT));
	if (code == 0)
		return;

	// bug #721 -> http://dcx.scriptsdb.org/bug/index.php?do=details&task_id=721
	/*TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);*/
	/*DRAWTEXTPARAMS dtp = {0};
	dtp.cbSize = sizeof(DRAWTEXTPARAMS);
	dtp.iTabLength = 4;*/

	const auto origWeight = lf.lfWeight;
	auto origLeft = rc->left;

	SetBkMode(hdc, TRANSPARENT);

	const UINT iStyle = (style & ~(DT_CENTER | DT_RIGHT | DT_VCENTER)) | DT_LEFT; // make sure its to left

	if (!dcx_testflag(iStyle, DT_CALCRECT))
	{ // if NOT doing DT_CALCRECT calc rect needed here.
		//mIRC_DrawText(hdc, txt, &rcOut, style | DT_CALCRECT, shadow);
		if (dcx_testflag(style, DT_CENTER) || dcx_testflag(style, DT_RIGHT) || dcx_testflag(style, DT_VCENTER))
		{
			// strip out ctrl codes to correctly position text.
			auto rcTmp = *rc;
			auto t(txt);

			if (dcx_testflag(iStyle, DT_SINGLELINE))
				t.mreplace(TEXT(' '), TEXT("\n\r"));	// replace any \n or \r with a space

			t.strip();	// removes ctrl codes & double spaces & spaces at start & end.
			DrawTextW(hdc, t.to_wchr(), -1, &rcTmp, style | DT_CALCRECT /*| DT_NOPREFIX*/);
			// style can be either center or right, not both, but it can be center+vcenter or right+vcenter
			if (dcx_testflag(style, DT_CENTER))
			{ // get center text start offset
				// (total width) - (required width) / 2 = equal space to each side.
				origLeft = rcOut.left = (((rcOut.right - rcOut.left) - (rcTmp.right - rcTmp.left)) / 2);
			}
			else if (dcx_testflag(style, DT_RIGHT))
			{ // get right text start offset
				// adjust start to right
				origLeft = rcOut.left = (rcOut.right - (rcTmp.right - rcTmp.left));
				//origLeft = rcOut.left = (rcOut.right - (rcTmp.right - rcTmp.left) - 3);
				//origLeft = rcOut.left = (rcOut.right - (rcTmp.right - rcTmp.left) - tm.tmMaxCharWidth);
				//origLeft = rcOut.left = (rcOut.right - (rcTmp.right - rcTmp.left) - tm.tmAveCharWidth);
				//origLeft = rcOut.left = rcTmp.left;
			}
			if (dcx_testflag(style, DT_VCENTER))
			{ // get Veritcal center text start offset
				rcOut.top += (((rcOut.bottom - rcOut.top) - (rcTmp.bottom - rcTmp.top)) / 2);
			}
		}

	}

	{
		TString tmp;
		{
			const auto wtxt = txt.to_wchr();
			const auto len = txt.len();
			bool usingRevTxt = false;
			UINT pos = 0;

			for (auto c = wtxt[pos]; pos < len; c = wtxt[++pos])
			{
				switch (c)
				{
				case 2: // Bold
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, &rcBufferBounds, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
					if (lf.lfWeight == FW_BOLD)
						lf.lfWeight = origWeight;
					else
						lf.lfWeight = FW_BOLD;
				}
				break;
				case 3: // Colour
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, &rcBufferBounds, tmp, &rcOut, &lf, iStyle, clrFG, shadow);

					while (wtxt[pos + 1] == 3)
						++pos; // remove multiple consecutive ctrl-k's

					if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
					{
						{
							UINT umIRCColour = (wtxt[pos + 1] - L'0'); // must be => 0 <= 9
							++pos;

							if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
							{
								umIRCColour *= 10;	// becomes a max of 90
								umIRCColour += (wtxt[pos + 1] - L'0');	// becomes a max of 99
								++pos;
							}

							// color code number
							clrFG = gsl::at(cPalette, umIRCColour);
						}

						// maybe a background color
						if (wtxt[pos + 1] == L',')
						{
							++pos;

							if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
							{
								UINT umIRCColour = (wtxt[pos + 1] - L'0'); // must be => 0 <= 9
								++pos;

								if (wtxt[pos + 1] >= L'0' && wtxt[pos + 1] <= L'9')
								{
									umIRCColour *= 10;	// becomes a max of 90
									umIRCColour += (wtxt[pos + 1] - L'0');	// becomes a max of 99
									++pos;
								}

								// color code number
								clrBG = gsl::at(cPalette, umIRCColour);
								SetBkMode(hdc, OPAQUE);
							}
						}
						if (usingRevTxt)
						{ // reverse text swap fg & bg colours
							std::swap(clrFG, clrBG);
							SetBkMode(hdc, OPAQUE);
						}
					}
					else {
						clrFG = origFG;
						clrBG = origBG;
						SetBkMode(hdc, TRANSPARENT);
					}
					SetTextColor(hdc, clrFG);
					SetBkColor(hdc, clrBG);
				}
				break;
				case 15: // ctrl+o end any ctrl codes in effect.
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, &rcBufferBounds, tmp, &rcOut, &lf, iStyle, clrFG, shadow);

					while (wtxt[pos + 1] == 15)
						++pos; // remove multiple consecutive ctrl-o's

					lf.lfWeight = origWeight;
					lf.lfUnderline = 0;
					lf.lfItalic = 0;
					lf.lfStrikeOut = 0;
					clrFG = origFG;
					clrBG = origBG;
					SetTextColor(hdc, origFG);
					SetBkColor(hdc, origBG);
					SetBkMode(hdc, TRANSPARENT);
					usingRevTxt = false;
				}
				break;
				case 22: // ctrl+r reverse
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, &rcBufferBounds, tmp, &rcOut, &lf, iStyle, clrFG, shadow);

					usingRevTxt = (usingRevTxt ? false : true);

					if (usingRevTxt)
						SetBkMode(hdc, OPAQUE);
					else
						SetBkMode(hdc, TRANSPARENT);

					std::swap(clrFG, clrBG);

					SetTextColor(hdc, clrFG);
					SetBkColor(hdc, clrBG);
				}
				break;
				case 29: // ctrl-i Italics
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, &rcBufferBounds, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
					lf.lfItalic = (lf.lfItalic ? 0U : 1U);
				}
				break;
				case 30: // ctrl+e strikethrough
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, rc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
					lf.lfStrikeOut = (lf.lfStrikeOut ? 0U : 1U);
				}
				break;
				case 31: // ctrl+u underline
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, &rcBufferBounds, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
					lf.lfUnderline = (lf.lfUnderline ? 0U : 1U);
				}
				break;
				case 10:
				case 13:
				{
					if (dcx_testflag(iStyle, DT_SINGLELINE))
					{ // when single line, replace with a space or ignore?
						//while ((wtxt[pos+1] == 13) || (wtxt[pos+1] == 10)) pos++; // remove multiple consecutive line feeds (causes exception??)
						tmp += TEXT(' '); //" ";
					}
					else {
						if (const auto tlen = tmp.len(); tlen > 0)
						{
							SIZE sz{};
							GetTextExtentPoint32W(hdc, tmp.to_wchr(), gsl::narrow_cast<int>(tlen), &sz);
							mIRC_OutText(hdc, &rcBufferBounds, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
							rcOut.top += sz.cy;
						}

						if (dcx_testflag(style, DT_RIGHT))
							rcOut.left = origLeft;
						else
							rcOut.left = rc->left;
					}
				}
				break;
				default: // normal TCHAR
				{
					if (!dcx_testflag(iStyle, DT_SINGLELINE))
					{ // don't bother if a single line.
						if (!tmp.empty())
						{
							SIZE sz{};
							int nFit{};
							const auto tlen = gsl::narrow_cast<int>(tmp.len());
							GetTextExtentExPointW(hdc, txt.to_wchr(), tlen, (rcOut.right - rcOut.left), &nFit, nullptr, &sz);
							if (nFit < tlen)
							{
								if (nFit > 0)
								{
									const auto o = tmp[nFit];
									auto tsSub(tmp.sub(0, nFit));
									mIRC_OutText(hdc, &rcBufferBounds, tsSub, &rcOut, &lf, iStyle, clrFG, shadow);
									rcOut.top += sz.cy;
									rcOut.left = origLeft;
									tmp = o;
								}
								else
									tmp.clear();
							}
						}
					}
					tmp += c;
				}
				break;
				}
			}
		}
		if (!tmp.empty())
			mIRC_OutText(hdc, &rcBufferBounds, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
	}

	if (!dcx_testflag(iStyle, DT_CALCRECT))
		BitBlt(oldHDC, rc->left, rc->top, (rc->right - rc->left), (rc->bottom - rc->top), hdc, 0, 0, SRCCOPY);
	else { // calc rect only
		OffsetRect(&rcOut, rc->left, rc->top);
		CopyRect(rc, &rcOut);
	}
}

//#include <ColourString.h>
//
//void mIRC_DrawText(HDC hdc, const TString &txt, LPRECT rc, const UINT style, const bool shadow)
//{
//	ColourString<TCHAR> tmp(txt.to_chr());
//
//	ColourString<TCHAR>::RenderInfo ri;
//	ri.ri_dwFlags = style;
//	ri.ri_bEnableAngleChar = false;
//	ri.ri_bEnableAngleLine = false;
//	ri.ri_bEnableShadow = shadow;
//	ri.ri_iLineAngle = 0;
//	ri.ri_iCharAngle = 0;
//	ri.ri_crShadow = RGB(0, 0, 0);
//	ri.ri_crText = GetTextColor(hdc);
//	ri.ri_ixOffset = 2;
//	ri.ri_iyOffset = 2;
//
//	getmIRCPalette(ri.ri_cPalette, Dcx::countof(ri.ri_cPalette)); // get mIRC palette
//
//	tmp.Render(hdc, rc, ri);
//}

namespace
{
	HMODULE UXModule{ nullptr };         //!< UxTheme.dll Module Handle
#ifdef DCX_USE_GDIPLUS
	HMODULE GDIPlusModule{ nullptr };	//!< gdiplus.dll Module Handle
	ULONG_PTR gdi_token{};
#endif
	HMODULE DWMModule{ nullptr };		//!< dwmapi.dll Module Handle
	struct HDCBuffer
	{
		HDC m_hHDC{};
		HBITMAP m_hOldBitmap{};
		HBITMAP m_hBitmap{};
		HFONT m_hOldFont{};
		HPAINTBUFFER m_hPaintBuffer{};

		// needs work...
	};
	using LPHDCBuffer = HDCBuffer*;
}

HFONT CopyHDCSettings(_In_ HDC hSrc, _In_ HDC hDst) noexcept
{
	if ((!hSrc) || (!hDst))
		return nullptr;

	// copy settings from hdc to buffer's hdc.
	SetDCBrushColor(hDst, GetDCBrushColor(hSrc));
	SetDCPenColor(hDst, GetDCPenColor(hSrc));
	SetLayout(hDst, GetLayout(hSrc));
	SetTextColor(hDst, GetTextColor(hSrc));
	SetTextAlign(hDst, GetTextAlign(hSrc));
	SetBkColor(hDst, GetBkColor(hSrc));
	SetBkMode(hDst, GetBkMode(hSrc));
	SetROP2(hDst, GetROP2(hSrc));
	SetMapMode(hDst, GetMapMode(hSrc));
	SetPolyFillMode(hDst, GetPolyFillMode(hSrc));
	SetStretchBltMode(hDst, GetStretchBltMode(hSrc));
	SetGraphicsMode(hDst, GetGraphicsMode(hSrc));
	return Dcx::dcxSelectObject(hDst, Dcx::dcxGetCurrentObject<HFONT>(hSrc, OBJ_FONT));
}

HANDLE CreateHDCBufferOptions(_In_ HDC hdc, _Out_ HDC* hdcOut, _In_ bool bCopySettings, _In_ bool bCopyContents, _In_ bool bAlpha, _In_ BYTE uAlpha) noexcept
{
	if ((!hdc) || (!hdcOut))
		return nullptr;

	if (!DcxUXModule::IsBufferedPaintSupported())
		return nullptr;

	auto buf = std::make_unique<HDCBuffer>();

	const RECT rcClient = dcxGetCurrentBitmapRect(hdc);

	const BLENDFUNCTION bf{ AC_SRC_OVER,0, uAlpha, 0 };
	BP_PAINTPARAMS paintParams{ sizeof(BP_PAINTPARAMS),BPPF_ERASE, nullptr, nullptr };

	if (bAlpha)
		paintParams.pBlendFunction = &bf;

	buf->m_hPaintBuffer = DcxUXModule::dcxBeginBufferedPaint(hdc, &rcClient, BPBF_TOPDOWNDIB /*BPBF_COMPATIBLEBITMAP*/, &paintParams, hdcOut);
	if (!buf->m_hPaintBuffer)
		return nullptr;

	buf->m_hHDC = *hdcOut;

	// copy settings from hdc to buffer's hdc.
	if (bCopySettings)
	buf->m_hOldFont = CopyHDCSettings(hdc, buf->m_hHDC);

	// copy contents of hdc within area to buffer.
	if (bCopyContents)
		BitBlt(buf->m_hHDC, 0, 0, rcClient.right, rcClient.bottom, hdc, 0, 0, SRCCOPY);

	return reinterpret_cast<HANDLE>(buf.release());
}

void DeleteHDCBufferOptions(_In_opt_ HANDLE hBuf, _In_ bool bDraw) noexcept
{
	if (!hBuf)
		return;

	auto buf = reinterpret_cast<LPHDCBuffer>(hBuf);

	if (buf->m_hOldFont)
		SelectObject(buf->m_hHDC, buf->m_hOldFont);

	DcxUXModule::dcxEndBufferedPaint(buf->m_hPaintBuffer, bDraw);
}

gsl::owner<HDC*> CreateHDCBuffer(HDC hdc, const LPRECT rc)
{
	if (!hdc)
		return nullptr;

	// alloc buffer data
	auto buf = std::make_unique<HDCBuffer>();

	// create HDC for buffer.
	buf->m_hHDC = CreateCompatibleDC(hdc);
	if (!buf->m_hHDC)
		return nullptr;

	// get size of bitmap to alloc.
	BITMAP bm{};
	int x{}, y{};

	if (!rc)
	{
		// no size specified, use hdc's bitmap size.
		if (auto [code, value] = Dcx::dcxGetObject<BITMAP>(Dcx::dcxGetCurrentObject<HBITMAP>(hdc, OBJ_BITMAP)); code == 0)
			return nullptr;
		else
			bm = value;
	}
	else {
		// use size specified.
		bm.bmWidth = (rc->right - rc->left);
		bm.bmHeight = (rc->bottom - rc->top);
		x = rc->left;
		y = rc->top;
	}

	// alloc bitmap for buffer.
	buf->m_hBitmap = CreateCompatibleBitmap(hdc, bm.bmWidth, bm.bmHeight);

	if (!buf->m_hBitmap)
	{
#ifdef DEBUG
		const DWORD err = GetLastError();
		constexpr DWORD errBufSize = 16U;
		if (LPTSTR errBuf{ nullptr }; FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, err, 0U, reinterpret_cast<LPTSTR>(&errBuf), errBufSize, nullptr) != 0)
		{
			Auto(LocalFree(errBuf));
			mIRCLinker::debug(TEXT("CreateHDCBuffer->CreateBitmap"), errBuf);
		}
#endif
		DeleteDC(buf->m_hHDC);
		return nullptr;
	}
	// select bitmap into hdc
	buf->m_hOldBitmap = Dcx::dcxSelectObject(buf->m_hHDC, buf->m_hBitmap);

	// copy settings from hdc to buffer's hdc.
	buf->m_hOldFont = CopyHDCSettings(hdc, buf->m_hHDC);

	// copy contents of hdc within area to buffer.
	BitBlt(buf->m_hHDC, 0, 0, bm.bmWidth, bm.bmHeight, hdc, x, y, SRCCOPY);

	// buffer is an exact duplicate of the hdc within the area specified.
	// return buffer typed as an HDC *
	//return (HDC *)buf.release();
	return gsl::owner<HDC*>(buf.release());
}

void DeleteHDCBuffer(gsl::owner<HDC*> hBuffer) noexcept
{
	if (!hBuffer)
		return;

	auto buf = gsl::owner<LPHDCBuffer>(reinterpret_cast<LPHDCBuffer>(hBuffer));

	GdiFlush();
	if (buf->m_hHDC)
	{
		if (buf->m_hOldFont)
			SelectObject(buf->m_hHDC, buf->m_hOldFont);
		if (buf->m_hOldBitmap)
			SelectObject(buf->m_hHDC, buf->m_hOldBitmap);
		if (buf->m_hBitmap)
			DeleteObject(buf->m_hBitmap);
		DeleteDC(buf->m_hHDC);
	}
	GSL_SUPPRESS(r.11) delete buf;
}

int TGetWindowText(HWND hwnd, TString& txt)
{
	txt = TGetWindowText(hwnd);
	return gsl::narrow_cast<int>(txt.len());
}

TString TGetWindowText(HWND hwnd)
{
	TString txt;

	if (!hwnd)
		return txt;

	// NB: needs to include space for end 0
	if (const auto nText = GetWindowTextLengthW(hwnd) + 2; nText > 2)
	{
		txt.reserve(gsl::narrow_cast<UINT>(nText));
		GetWindowTextW(hwnd, txt.to_chr(), nText);
	}
	return txt;
}

void FreeOSCompatibility() noexcept
{
#ifdef DCX_USE_GDIPLUS
	// Shutdown GDI+
	if (GDIPlusModule)
	{
		if (gdi_token)
			Gdiplus::GdiplusShutdown(gdi_token);

		FreeLibrary(GDIPlusModule);
		GDIPlusModule = nullptr;
	}
#endif

	if (UXModule)
	{
		DcxUXModule::dcxBufferedPaintUnInit();

		FreeLibrary(UXModule);
		UXModule = nullptr;
	}
	if (DWMModule)
	{
		FreeLibrary(DWMModule);
		DWMModule = nullptr;
	}
}

void AddFileIcons(HIMAGELIST himl, TString& filename, const bool bLarge, const int iIndex, const int iStart, int iEnd)
{
	if ((!himl) || (iStart < 0) || (iEnd != -1 && iStart > iEnd))
		throw DcxExceptions::dcxInvalidArguments();

	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("Unable to Access file: %"), filename);

	if (iEnd == -1)
		iEnd = (ExtractIconEx(filename.to_chr(), -1, nullptr, nullptr, 0) - 1);

#if DCX_USE_WRAPPERS
	for (auto FileIndex{ iStart }, IconIndex{ iIndex }; FileIndex <= iEnd; ++FileIndex)
	{
		const Dcx::dcxIconResource hIcon(filename, FileIndex, bLarge);	// throws on fail...

		if (ImageList_ReplaceIcon(himl, IconIndex, hIcon.get()) == -1)
			throw Dcx::dcxException(TEXT("Unable to Add %'s Icon %"), filename, FileIndex);

		if (IconIndex != -1)
			++IconIndex;
	}
#else
	HICON hIcon{ nullptr };

	for (auto FileIndex{ iStart }, IconIndex{ iIndex }; FileIndex <= iEnd; ++FileIndex)
	{
		if (bLarge)
			ExtractIconEx(filename.to_chr(), FileIndex, &hIcon, nullptr, 1);
		else
			ExtractIconEx(filename.to_chr(), FileIndex, nullptr, &hIcon, 1);

		if (!hIcon)
			throw Dcx::dcxException(TEXT("Unable to Add %'s Icon %"), filename, FileIndex);

		// auto free handle hIcon when going out of scope or throwing an exception
		//Auto(DestroyIcon(hIcon));

		// auto free handle hIcon when going out of scope or throwing an exception (testing)
		auto IconGuard = gsl::finally([hIcon]() noexcept { DestroyIcon(hIcon); });

		if (ImageList_ReplaceIcon(himl, IconIndex, hIcon) == -1)
			throw Dcx::dcxException(TEXT("Unable to Add %'s Icon %"), filename, FileIndex);

		if (IconIndex != -1)
			++IconIndex;
	}
#endif
}

BOOL dcxGetWindowRect(const HWND hWnd, const LPRECT lpRect) noexcept
{
	if ((!hWnd) || (!lpRect))
		return FALSE;

	// as described in a comment at http://msdn.microsoft.com/en-us/library/ms633519(VS.85).aspx
	// GetWindowRect does not return the real size of a window if you are using vista with areo glass
	// using DwmGetWindowAttribute now to fix that (fixes bug 685)
	if (Dcx::DwmModule.isUseable())
		return (DcxDWMModule::dcxDwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, lpRect, sizeof(RECT)) == S_OK);

	return GetWindowRect(hWnd, lpRect);
}
/*
	*	DrawRotatedText() function taken from ms example & modified for our needs.
*/
void DrawRotatedText(const TString& strDraw, const LPCRECT rc, const HDC hDC, const int nAngleLine/* = 0*/, const bool bEnableAngleChar /*= false*/, const int nAngleChar /*= 0*/) noexcept
{
	if ((!rc) || (!hDC))
		return;

	// Set the background mode to transparent for the
	// text-output operation.
	const auto nOldBkMode = SetBkMode(hDC, TRANSPARENT);
	Auto(SetBkMode(hDC, nOldBkMode));

	if ((nAngleLine == 0) && (!bEnableAngleChar))
	{
		TextOutW(hDC, rc->left, rc->bottom, strDraw.to_wchr(), gsl::narrow_cast<int>(strDraw.len()));
		return;
	}

	auto [code, lf] = Dcx::dcxGetObject<LOGFONT>(Dcx::dcxGetCurrentObject<HFONT>(hDC, OBJ_FONT));
	if (code == 0)
		return;

	// Specify the angle to draw line
	lf.lfEscapement = nAngleLine * 10;
	int nOldGMode{};
	if (bEnableAngleChar) // Enable character angle
	{
		// Set graphics mode to advance to enable orientation
		nOldGMode = SetGraphicsMode(hDC, GM_ADVANCED);
		// Specify the angle of characters
		lf.lfOrientation = nAngleChar * 10;
	}
	else // Draw in normal mode
	{
		nOldGMode = SetGraphicsMode(hDC, GM_COMPATIBLE);
	}
	// Restore old values
	Auto(SetGraphicsMode(hDC, nOldGMode));

	// Select the new font created
	if (const auto hFont = CreateFontIndirectW(&lf); hFont)
	{
		Auto(DeleteObject(hFont));

		const auto hPrevFont = SelectObject(hDC, hFont);
		Auto(SelectObject(hDC, hPrevFont));

		// Draw text to screen
		TextOutW(hDC, rc->left, rc->bottom, strDraw.to_wchr(), gsl::narrow_cast<int>(strDraw.len()));
	}
}

const char* queryAttribute(const TiXmlElement* element, const char* attribute, const char* defaultValue) noexcept
{
	if (!element || !attribute)
		return defaultValue;

	const auto t = element->Attribute(attribute);
	return (t) ? t : defaultValue;
}

TString queryEvalAttribute(const TiXmlElement* element, const char* attribute, const char* defaultValue)
{
	TString tsRes(queryAttribute(element, attribute, defaultValue));
	if (tsRes.empty())
		return tsRes;

	if (queryIntAttribute(element, "eval", 1) == 0)
		return tsRes;

	if (auto opt = mIRCLinker::o_eval<TString>(tsRes); opt.has_value())
	{
		if (opt.value() != tsRes)
			tsRes = opt.value();
	}

	return tsRes;
}

COLORREF queryColourAttribute(const TiXmlElement* element, const char* attribute, COLORREF defaultValue) noexcept
{
	if (!element || !attribute)
		return defaultValue;

	if (const auto t = element->Attribute(attribute); t)
	{
		char* ptr{};
		// all colour values are 32bit but only 24bit of that is used, 0x00000000 - 0x00FFFFFF
		// alpha is ignored.

		// only allows decimal numbers
		//return gsl::narrow_cast<COLORREF>(_ts_strtoul(t, &ptr, 10));

		//allows hex & decimal, hex numbers must start 0x or 0X or # for web style
		if (_ts_strnicmp(t, "0x", 2) == 0)	// base 16 hex number 0x00000000 - 0x00FFFFFF
			return gsl::narrow_cast<COLORREF>(_ts_strtoul(t, &ptr, 16));
		if (t[0] == '#')	// base 16 hex number #00000000 - #00FFFFFF
			return gsl::narrow_cast<COLORREF>(_ts_strtoul(&t[1], &ptr, 16));

		// decimal number
		return gsl::narrow_cast<COLORREF>(_ts_strtoul(t, &ptr, 10));
	}
	return defaultValue;
}

void setColourAttribute(TiXmlElement* element, const char* attribute, COLORREF Value)
{
	if (!element || !attribute)
		return;

	char szBuf[16]{};
	_ts_snprintf(&szBuf[0], std::size(szBuf), "%lu", Value);
	element->SetAttribute(attribute, &szBuf[0]);
}

int queryIntAttribute(const TiXmlElement* element, const char* attribute, const int defaultValue) noexcept
{
	if (element && attribute)
		if (const auto [iStatus, integer] = element->QueryIntAttribute(attribute); iStatus == TiXmlReturns::TIXML_SUCCESS)
			return integer;

	return defaultValue;
}

double queryDoubleAttribute(const TiXmlElement* element, const char* attribute, const double defaultValue) noexcept
{
	if (element && attribute)
		if (const auto [iStatus, integer] = element->QueryDoubleAttribute(attribute); iStatus == TiXmlReturns::TIXML_SUCCESS)
			return integer;

	return defaultValue;
}

float queryFloatAttribute(const TiXmlElement* element, const char* attribute, const float defaultValue) noexcept
{
	if (element && attribute)
		if (const auto [iStatus, integer] = element->QueryDoubleAttribute(attribute); iStatus == TiXmlReturns::TIXML_SUCCESS)
			return gsl::narrow_cast<float>(integer);

	return defaultValue;
}

/*
	Make the text safe for passing to the callback alias

	Change special characters $%()[]; into there $chr() equivalents
*/
TString MakeTextmIRCSafe(const TString& tsStr)
{
	return MakeTextmIRCSafe(tsStr.to_chr(), tsStr.len());
}

/*
Make the text safe for passing to the callback alias

Change special characters $%()[]; into there $chr() equivalents
*/
TString MakeTextmIRCSafe(const TCHAR* const tString)
{
	return MakeTextmIRCSafe(tString, _ts_strlen(tString));
}

/*
Make the text safe for passing to the callback alias

Change special characters $%()[]; into there $chr() equivalents
*/
TString MakeTextmIRCSafe(const TCHAR* const tString, const std::size_t len)
{
	TString tsRes(gsl::narrow_cast<TString::size_type>(MIRC_BUFFER_SIZE_CCH));	// use MIRC_BUFFER_SIZE_CCH as starting buffer size (shouldnt be bigger...)

	if ((!tString) || (len == 0))
		return tsRes;

	bool bLastWasSpace = true;	// start as true as the beginning of the line is treated the same as a space here

	// look for ()[]%$; we dont want to change , as this is needed
	for (auto i = decltype(len){0}; i < len; ++i)
	{
		switch (const auto& c = tString[i]; c)
		{
		case TEXT('('):
		case TEXT(')'):
		case TEXT('['):
		case TEXT(']'):
		case TEXT('%'):
		case TEXT('$'):
		case TEXT(';'):
			if (!bLastWasSpace)
				tsRes += TEXT(" $+ ");

			switch (c)
			{
			default:
				break;
			case TEXT('('):
				tsRes += TEXT("$chr(40)");
				break;
			case TEXT(')'):
				tsRes += TEXT("$chr(41)");
				break;
			case TEXT('['):
				tsRes += TEXT("$chr(91)");
				break;
			case TEXT(']'):
				tsRes += TEXT("$chr(93)");
				break;
			case TEXT('%'):
				tsRes += TEXT("$chr(37)");
				break;
			case TEXT('$'):
				tsRes += TEXT("$chr(36)");
				break;
			case TEXT(';'):
				tsRes += TEXT("$chr(59)");
				break;
			}

			if (tString[i + 1] != TEXT(' '))
				tsRes += TEXT(" $+ ");

			bLastWasSpace = false;
			break;
		case TEXT(' '):
			bLastWasSpace = true;
			tsRes += c;
			break;
		default:
			bLastWasSpace = false;
			tsRes += c;
			break;
		}
	}
	return tsRes;
}

COLORREF GetContrastColour(COLORREF sRGB) noexcept
{
	// (0.21 � R) + (0.72 � G) + (0.07 � B)
	// (0.2126*R + 0.7152*G + 0.0722*B)
	// (0.299*R + 0.587*G + 0.114*B)
	// sqrt( 0.299*R^2 + 0.587*G^2 + 0.114*B^2 )
	// 
	// vR = sR / 255;
	// vG = sG / 255;
	// vB = sB / 255;
	// Y = (0.2126 * sRGBtoLin(vR) + 0.7152 * sRGBtoLin(vG) + 0.0722 * sRGBtoLin(vB))

	const double vR = GetRValue(sRGB) / 255.0;
	const double vG = GetGValue(sRGB) / 255.0;
	const double vB = GetBValue(sRGB) / 255.0;

	const auto Y = (0.2126 * sRGBtoLin(vR) + 0.7152 * sRGBtoLin(vG) + 0.0722 * sRGBtoLin(vB));

	const auto Lstar = YtoLstar(Y);

	// white
	if (Lstar < 50.0)
		return RGB(255, 255, 255);

	// black
	return RGB(0, 0, 0);
}

RGBQUAD GetContrastColour(RGBQUAD sRGB) noexcept
{
	// (0.21 � R) + (0.72 � G) + (0.07 � B)
	// (0.2126*R + 0.7152*G + 0.0722*B)
	// (0.299*R + 0.587*G + 0.114*B)
	// sqrt( 0.299*R^2 + 0.587*G^2 + 0.114*B^2 )
	// 
	// vR = sR / 255;
	// vG = sG / 255;
	// vB = sB / 255;
	// Y = (0.2126 * sRGBtoLin(vR) + 0.7152 * sRGBtoLin(vG) + 0.0722 * sRGBtoLin(vB))

	const double vR = sRGB.rgbRed / 255.0;
	const double vG = sRGB.rgbGreen / 255.0;
	const double vB = sRGB.rgbBlue / 255.0;

	const auto Y = (0.2126 * sRGBtoLin(vR) + 0.7152 * sRGBtoLin(vG) + 0.0722 * sRGBtoLin(vB));

	const auto Lstar = YtoLstar(Y);

	// maintain the alpha value.
	// 
	// white
	if (Lstar < 50.0)
		return { 255, 255, 255, sRGB.rgbReserved };

	// black
	return { 0, 0, 0, sRGB.rgbReserved };
}

namespace
{
	std::vector<BYTE> Base64Decode(const wchar_t* str, size_t szLen)
	{
		std::vector<BYTE> data;

		if (str && szLen > 0)
		{
			DWORD bLen{};

			if (CryptStringToBinaryW(str, gsl::narrow_cast<DWORD>(szLen), CRYPT_STRING_BASE64, nullptr, &bLen, nullptr, nullptr))
			{
				data.reserve(bLen);

				CryptStringToBinaryW(str, gsl::narrow_cast<DWORD>(szLen), CRYPT_STRING_BASE64, data.data(), &bLen, nullptr, nullptr);
			}
		}
		return data;
	}
	std::vector<BYTE> Base64Decode(const char* str, size_t szLen)
	{
		std::vector<BYTE> data;

		if (str && szLen > 0)
		{
			DWORD bLen{};

			if (CryptStringToBinaryA(str, gsl::narrow_cast<DWORD>(szLen), CRYPT_STRING_BASE64, nullptr, &bLen, nullptr, nullptr))
			{
				data.reserve(bLen);

				CryptStringToBinaryA(str, gsl::narrow_cast<DWORD>(szLen), CRYPT_STRING_BASE64, data.data(), &bLen, nullptr, nullptr);
			}
		}
		return data;
	}
	TString Base64Encode(LPBYTE data, size_t nLen)
	{
		TString tsBuf;
		if (data && nLen > 0)
		{
			DWORD szLen{};

			if (CryptBinaryToStringW(data, gsl::narrow_cast<DWORD>(nLen), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &szLen))
			{
				tsBuf.reserve(szLen);

				CryptBinaryToStringW(data, gsl::narrow_cast<DWORD>(nLen), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, tsBuf.to_wchr(), &szLen);
			}
		}
		return tsBuf;
	}
}

TString BitmapToBase64(HBITMAP hBMP)
{
	if (!hBMP)
		return {};

	auto pbi = CreateBitmapInfoStruct(hBMP);
	if (!pbi)
		return {};
	Auto(LocalFree(pbi));

	PBITMAPINFOHEADER pbih = reinterpret_cast<PBITMAPINFOHEADER>(pbi);     // bitmap info-header  
	LPBYTE lpBits = static_cast<LPBYTE>(GlobalAlloc(GMEM_FIXED, pbih->biSizeImage));              // memory pointer

	if (!lpBits)
		return{};
	Auto(GlobalFree(lpBits));

	auto hDC = ::GetDC(nullptr);
	Auto(::ReleaseDC(nullptr, hDC));

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	if (!GetDIBits(hDC, hBMP, 0, pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS))
		return{};

	BITMAPFILEHEADER hdr{};       // bitmap file-header  
	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
	// Compute the size of the entire file.  
	hdr.bfSize = (sizeof(BITMAPFILEHEADER) + pbih->biSize + (pbih->biClrUsed * sizeof(RGBQUAD)) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + pbih->biSize + (pbih->biClrUsed * sizeof(RGBQUAD));

	std::string stOut;

	//auto szBuf = std::make_unique<BYTE[]>(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pbih->biSizeImage);
	auto szBuf = std::make_unique<BYTE[]>(sizeof(BITMAPFILEHEADER) + (sizeof(BITMAPINFOHEADER) + (pbih->biClrUsed * sizeof(RGBQUAD))) + pbih->biSizeImage);
	LPBYTE ptr = szBuf.get();

	DWORD dwTmp{ sizeof(BITMAPFILEHEADER) };
	DWORD dwTotal{};

	// Copy the BITMAPFILEHEADER into the .BMP file.
	memcpy(ptr, &hdr, dwTmp);
	ptr += dwTmp;
	dwTotal += dwTmp;

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	dwTmp = sizeof(BITMAPINFOHEADER) + (pbih->biClrUsed * sizeof(RGBQUAD));
	memcpy(ptr, pbih, dwTmp);
	ptr += dwTmp;
	dwTotal += dwTmp;

	// Copy the array of color indices into the .BMP file.
	dwTmp = pbih->biSizeImage;
	memcpy(ptr, lpBits, dwTmp);
	ptr += dwTmp;
	dwTotal += dwTmp;

	// Close the .BMP file.
	//return Base64::encode(szBuf.get(), dwTotal);
	return Base64Encode(szBuf.get(), dwTotal);
}

TString IconToBase64(HICON hIcon)
{
	if (hIcon)
	{
		if (ICONINFO icInfo{}; ::GetIconInfo(hIcon, &icInfo))
			return BitmapToBase64(icInfo.hbmColor) + ' ' + BitmapToBase64(icInfo.hbmMask);
	}
	return {};
}

HBITMAP Base64ToBitmap(const char* vData, long w, long h)
{
	if (_ts_isEmpty(vData))
		return nullptr;

	auto hdc = ::GetDC(nullptr);
	if (!hdc)
		return nullptr;

	Auto(::ReleaseDC(nullptr, hdc));

	BITMAPFILEHEADER hdr{};
	BITMAPINFOHEADER bmpInfo{};

	auto dec = Base64Decode(vData, _ts_strlen(vData));
	auto ptr = dec.data();

	size_t allocsz = sizeof(bmpInfo);

	memcpy(&hdr, ptr, sizeof(hdr));
	ptr += sizeof(hdr);
	memcpy(&bmpInfo, ptr, allocsz);
	//if (bmpInfo.biClrUsed < 24)
	allocsz += (bmpInfo.biClrUsed * sizeof(RGBQUAD));

	std::vector<BYTE> vBinf;
	vBinf.reserve(allocsz);

	memcpy(vBinf.data(), ptr, allocsz);

	ptr += allocsz;

	LPBYTE lpBits = ptr;

	return CreateDIBitmap(hdc, &bmpInfo, CBM_INIT, lpBits, reinterpret_cast<BITMAPINFO*>(vBinf.data()), DIB_RGB_COLORS);
}

HICON Base64ToIcon(const char* hbmData, const char* maskData, long sz)
{
	if (auto hbm = Base64ToBitmap(hbmData, sz, sz); hbm)
	{
		Auto(DeleteBitmap(hbm));

		if (auto hbmMask = Base64ToBitmap(maskData, sz, sz); hbmMask)
		{
			Auto(DeleteBitmap(hbmMask));

			ICONINFO   icInfo{};

			icInfo.hbmColor = hbm;
			icInfo.hbmMask = hbmMask;
			icInfo.fIcon = TRUE;

			return ::CreateIconIndirect(&icInfo);
		}
	}
	return nullptr;
}

HBITMAP Base64ToBitmap(const wchar_t* vData, long w, long h)
{
	if (_ts_isEmpty(vData))
		return nullptr;

	auto hdc = ::GetDC(nullptr);
	if (!hdc)
		return nullptr;

	Auto(::ReleaseDC(nullptr, hdc));

	BITMAPFILEHEADER hdr{};
	BITMAPINFOHEADER bmpInfo{};

	auto dec = Base64Decode(vData, _ts_strlen(vData));
	auto ptr = dec.data();

	size_t allocsz = sizeof(bmpInfo);

	memcpy(&hdr, ptr, sizeof(hdr));
	ptr += sizeof(hdr);
	memcpy(&bmpInfo, ptr, allocsz);
	//if (bmpInfo.biClrUsed < 24)
	allocsz += (bmpInfo.biClrUsed * sizeof(RGBQUAD));

	std::vector<BYTE> vBinf;
	vBinf.reserve(allocsz);

	memcpy(vBinf.data(), ptr, allocsz);

	ptr += allocsz;

	LPBYTE lpBits = ptr;

	return CreateDIBitmap(hdc, &bmpInfo, CBM_INIT, lpBits, reinterpret_cast<BITMAPINFO*>(vBinf.data()), DIB_RGB_COLORS);
}

HICON Base64ToIcon(const wchar_t* hbmData, const wchar_t* maskData, long sz)
{
	if (auto hbm = Base64ToBitmap(hbmData, sz, sz); hbm)
	{
		Auto(DeleteBitmap(hbm));

		if (auto hbmMask = Base64ToBitmap(maskData, sz, sz); hbmMask)
		{
			Auto(DeleteBitmap(hbmMask));

			ICONINFO   icInfo{};

			icInfo.hbmColor = hbm;
			icInfo.hbmMask = hbmMask;
			icInfo.fIcon = TRUE;

			return ::CreateIconIndirect(&icInfo);
		}
	}
	return nullptr;
}

#ifdef DCX_USE_GDIPLUS
bool SavePNGFile(TString& tsFile, Gdiplus::Image& img)
{
	if (IsFile(tsFile))
		return false;

	return CreatePNGFile(tsFile.to_wchr(), img);
}
#endif
