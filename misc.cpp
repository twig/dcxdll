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
* © ScriptsDB.org - 2006-2008
*/

#include "defines.h"
//#include "Dcx.h"

#include <shlwapi.h>

#ifdef DCX_USE_BOOST
// for boost::regex
//#define BOOST_REGEX_NO_LIB 1
#include <boost/regex.hpp>
#endif
#ifdef DCX_USE_PCRE
#include "pcre7.2/include/pcre.h"
#endif

// DCX Stuff

/*!
* \brief Rounding function
*/
int round(const float x) {
	if (x - (float) (int) x > 0.5)
		return (int) x +1;
	return (int) x;
}

/*!
* \brief Read File Contents
*
* blah
*/
BYTE *readFile(const PTCHAR filename) {
	//Ouvrir le fichier, read en mode binaire
	if (filename == NULL)
		return NULL;

	FILE *file = _wfopen(filename, TEXT("rb"));

	// Open file in binary mode and read
	if (file == NULL)
		return NULL;

	// Seek End of file
	if (fseek(file, 0, SEEK_END))
		return NULL;

	// Read pointer location, because pointer is at the end, results into file size
	unsigned long size = ftell(file);

	// Get back to file beginning
	if (fseek(file, 0, SEEK_SET))
		return NULL;

	// make data container for file contents
	BYTE * fileContents = new BYTE[size + 1];

	if (fileContents == NULL)
		return NULL;

	// Null terminate the string
	fileContents[size] = 0;

	// read the file, fails, destroy memory and return NULL
	if (fread(fileContents, 1, size, file) != size) {
		delete fileContents;
		return NULL;
	}

	// close file
	fclose(file);

	// return memory block containing file data
	return fileContents;
}


/*!
* \brief Copies string to the clipboard
*
* Returns TRUE if successful
*
* http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/dataexchange/clipboard/usingtheclipboard.asp
*/
BOOL CopyToClipboard(const HWND owner, const TString & str) {
	if (!OpenClipboard(owner)) {
		Dcx::error(TEXT("CopyToClipboard"),TEXT("Couldn't open clipboard"));
		return FALSE;
	}

	int cbsize = (int)(str.len() +1) * sizeof(TCHAR);
	EmptyClipboard();
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, cbsize);

	if (hglbCopy == NULL) {
		CloseClipboard();
		Dcx::error(TEXT("CopyToClipboard"),TEXT("Couldn't open global memory"));
		return FALSE;
	}

	TCHAR *strCopy = (TCHAR *) GlobalLock(hglbCopy);

	// original code, limited and doesnt copy large chunks
	//wnsprintf(strCopy, MIRC_BUFFER_SIZE_CCH, TEXT("%s"), str.to_chr());

	// demo code from msdn, copies everything
	memcpy(strCopy, str.to_chr(), cbsize);
	strCopy[cbsize] = (TCHAR) 0; // null character

	GlobalUnlock(hglbCopy);
	SetClipboardData(CF_TEXT, hglbCopy);
	CloseClipboard();

	return TRUE;
}


// Turns a command (+flags CHARSET SIZE FONTNAME) into a LOGFONT struct
BOOL ParseCommandToLogfont(const TString& cmd, LPLOGFONT lf) {
	if (cmd.numtok( ) < 4)
		return FALSE;

	ZeroMemory(lf, sizeof(LOGFONT));
	UINT flags = parseFontFlags(cmd.gettok( 1 ));

	if (flags & DCF_DEFAULT) {
		HFONT hf = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
		GetObject(hf, sizeof(LOGFONT), lf);
		return TRUE;
	}
	else {
		int fSize = cmd.gettok( 3 ).to_int();
		TString fName(cmd.gettok(4, -1).trim());

		if (!fSize)
			return FALSE;

		HDC hdc = GetDC(NULL);
		lf->lfHeight = -MulDiv(fSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);

		ReleaseDC(NULL, hdc);

		if (flags & DCF_ANTIALIASE)
			lf->lfQuality = ANTIALIASED_QUALITY;

		if (flags & DCF_BOLD)
			lf->lfWeight = FW_BOLD;
		else
			lf->lfWeight = FW_NORMAL;

		if (flags & DCF_ITALIC)
			lf->lfItalic = TRUE;

		if (flags & DCF_STRIKEOUT)
			lf->lfStrikeOut = TRUE;

		if (flags & DCF_UNDERLINE)
			lf->lfUnderline = TRUE;

		lf->lfCharSet = (BYTE)parseFontCharSet(cmd.gettok( 2 ));
		lstrcpyn(lf->lfFaceName, fName.to_chr(), 31);
		lf->lfFaceName[31] = 0;
		return TRUE;
	}
}


/*!
 * \brief blah
 *
 * blah
 */
UINT parseFontFlags(const TString &flags) {
	INT i = 1, len = (int)flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != TEXT('+'))
		return iFlags;

	while (i < len) {
		if (flags[i] == TEXT('a'))
			iFlags |= DCF_ANTIALIASE;
		else if (flags[i] == TEXT('b'))
			iFlags |= DCF_BOLD;
		else if (flags[i] == TEXT('d'))
			iFlags |= DCF_DEFAULT;
		else if (flags[i] == TEXT('i'))
			iFlags |= DCF_ITALIC;
		else if (flags[i] == TEXT('s'))
			iFlags |= DCF_STRIKEOUT;
		else if (flags[i] == TEXT('u'))
			iFlags |= DCF_UNDERLINE;

		++i;
	}

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */
UINT parseFontCharSet(const TString &charset) {
	if (charset == TEXT("ansi"))
		return ANSI_CHARSET;
	else if (charset == TEXT("baltic"))
		return BALTIC_CHARSET;
	else if (charset == TEXT("chinesebig"))
		return CHINESEBIG5_CHARSET;
	else if (charset == TEXT("default"))
		return DEFAULT_CHARSET;
	else if (charset == TEXT("easteurope"))
		return EASTEUROPE_CHARSET;
	else if (charset == TEXT("gb2312"))
		return GB2312_CHARSET;
	else if (charset == TEXT("greek"))
		return GREEK_CHARSET;
	else if (charset == TEXT("hangul"))
		return HANGUL_CHARSET;
	else if (charset == TEXT("mac"))
		return MAC_CHARSET;
	else if (charset == TEXT("oem"))
		return OEM_CHARSET;
	else if (charset == TEXT("russian"))
		return RUSSIAN_CHARSET;
	else if (charset == TEXT("shiftjis"))
		return SHIFTJIS_CHARSET;
	else if (charset == TEXT("symbol"))
		return SYMBOL_CHARSET;
	else if (charset == TEXT("turkish"))
		return TURKISH_CHARSET;
	else if (charset == TEXT("vietnamese"))
		return VIETNAMESE_CHARSET;

	return DEFAULT_CHARSET;
}


TString ParseLogfontToCommand(const LPLOGFONT lf) {
	TString flags(TEXT("+"));
	TString charset(TEXT("default"));

	// get charset
	switch (lf->lfCharSet) {
		case ANSI_CHARSET			: charset = TEXT("ansi"); break;
		case BALTIC_CHARSET		: charset = TEXT("baltic"); break;
		case CHINESEBIG5_CHARSET: charset = TEXT("chinesebig"); break;
		case EASTEUROPE_CHARSET	: charset = TEXT("easteurope"); break;
		case GB2312_CHARSET		: charset = TEXT("gb2312"); break;
		case GREEK_CHARSET		: charset = TEXT("greek"); break;
		case HANGUL_CHARSET		: charset = TEXT("hangul"); break;
		case MAC_CHARSET			: charset = TEXT("mac"); break;
		case OEM_CHARSET			: charset = TEXT("oem"); break;
		case RUSSIAN_CHARSET		: charset = TEXT("russian"); break;
		case SHIFTJIS_CHARSET	: charset = TEXT("shiftjis"); break;
		case SYMBOL_CHARSET		: charset = TEXT("symbol"); break;
		case TURKISH_CHARSET		: charset = TEXT("turkish"); break;
		case VIETNAMESE_CHARSET	: charset = TEXT("vietnamese"); break;
		case DEFAULT_CHARSET		:
		default						: charset = TEXT("default"); break;
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
	HDC hdc = GetDC(NULL);
	HFONT hf = CreateFontIndirect(lf), oldhf = NULL;
	TEXTMETRIC tm;

	oldhf = SelectFont(hdc, hf);
	GetTextMetrics(hdc, &tm);

	//int ptSize = (int) (-1 * (lfCurrent.lfHeight * 72 / GetDeviceCaps(hdc, LOGPIXELSY)));
	int ptSize = MulDiv(tm.tmHeight - tm.tmInternalLeading, 72, GetDeviceCaps(hdc, LOGPIXELSY));
	SelectFont(hdc,oldhf);
	DeleteFont(hf);
	ReleaseDC(NULL, hdc);

	// [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
	TString tmp;
	
	tmp.tsprintf(TEXT("%s %s %d %s"), flags.to_chr(), charset.to_chr(), ptSize, lf->lfFaceName);
	return tmp;
}

HICON dcxLoadIcon(const int index, TString &filename, const bool large, const TString &flags) {
	filename.trim();

	if (flags[0] != TEXT('+')) {
		Dcx::error(TEXT("dcxLoadIcon"), TEXT("Invalid Flags"));
		return NULL;
	}

	// index is -1
	if (index < 0) {
		Dcx::error(TEXT("dcxLoadIcon"), TEXT("Invalid Index"));
		return NULL;
	}

	// This doesnt require a valid filename.
	if (flags.find(TEXT('f'), 0)) {
		SHFILEINFO shfi;
		TString filetype;

		ZeroMemory(&shfi, sizeof(SHFILEINFO));
		filetype.tsprintf(TEXT(".%s"), filename.to_chr());
		
		SHGetFileInfo(filetype.to_chr(), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO),
			SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | (large ? SHGFI_LARGEICON : SHGFI_SMALLICON));

		return shfi.hIcon;
	}

	// Check for valid filename
	if (!IsFile(filename)) {
		Dcx::errorex(TEXT("dcxLoadIcon"), TEXT("Could Not Access File: %s"), filename.to_chr());
		return NULL;
	}

	HICON icon = NULL;

	if (flags.find(TEXT('a'),0)) {
		WORD wIndex = (WORD)index;
		icon = ExtractAssociatedIcon(NULL, filename.to_chr(), &wIndex);
	}
#ifdef DCX_USE_GDIPLUS
	else if (flags.find(TEXT('P'),0)) {
		if (!Dcx::GDIModule.isUseable())
		{
			Dcx::error(TEXT("dcxLoadIcon"), TEXT("Invalid +P without GDI+."));
			return NULL;
		}

#if UNICODE
		Bitmap *p_Img = new Bitmap(filename.to_chr());
#else
		Bitmap *p_Img = new Bitmap(filename.to_wchr());
#endif
		if (p_Img == NULL)
			return NULL;
		// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
		Status status = p_Img->GetLastStatus();
		if (status != Ok)
			Dcx::error(TEXT("dcxLoadIcon"), GetLastStatusStr(status));
		else {
			//int w = 0, h = 0;
			//if (large) {
			//	w = GetSystemMetrics(SM_CXICON);
			//	h = GetSystemMetrics(SM_CYICON);
			//}
			//else {
			//	w = GetSystemMetrics(SM_CXSMICON);
			//	h = GetSystemMetrics(SM_CYSMICON);
			//}
			//Bitmap *p_Thumb = p_Img->GetThumbnailImage(w,h);
			//if (p_Thumb != NULL) {
			//	p_Thumb->GetHICON(&icon);
			//	delete p_Thumb;
			//}
			status = p_Img->GetHICON(&icon); // for reasons unknown this causes a `first chance exception` to show in debug log.
			if (status != Ok)
				Dcx::error(TEXT("dcxLoadIcon"), GetLastStatusStr(status));
			GdiFlush();
		}
		delete p_Img;
	}
#endif
	else {
		if (large)
			ExtractIconEx(filename.to_chr(), index, &icon, NULL, 1);
		else
			ExtractIconEx(filename.to_chr(), index, NULL, &icon, 1);
	}

	if (flags.find(TEXT('g'), 0) && icon != NULL)
		icon = CreateGrayscaleIcon(icon);

	return icon;
}

//HBITMAP GDIPlusToBitmap(Bitmap *gdiPlusBmp)
//{
//	//HDC hSrcDC = CreateCompatibleDC(NULL);
//	//HBITMAP oldSrcBmp = SelectBitmap(hSrcDC, hBmp);
//	//BITMAP bmp;
//	//GetObject(hBmp, sizeof(BITMAP), &bmp);
//	//HDC hDstDC = CreateCompatibleDC(NULL);
//	//HBITMAP hDstBmp = CreateCompatibleBitmap(hSrcDC, bmp.bmWidth, bmp.bmHeight);
//	//HBITMAP oldDstBmp = SelectBitmap(hDstDC, hDstBmp);
//
//	//BitBlt(hDstDC, 0,0, bmp.bmWidth, bmp.bmHeight, hSrcDC, 0,0, SRCCOPY);
//
//	//SelectBitmap(hDstDC, oldDstBmp);
//	//DeleteDC(hDstDC);
//	//SelectBitmap(hSrcDC, oldSrcBmp);
//	//DeleteDC(hSrcDC);
//	HDC hScreen = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
//	HDC hDstDC = CreateCompatibleDC(hScreen);
//	HBITMAP hDstBmp = CreateCompatibleBitmap(hScreen, gdiPlusBmp->GetWidth(), gdiPlusBmp->GetHeight());
//	HBITMAP oldDstBmp = SelectBitmap(hDstDC, hDstBmp);
//
//	Graphics gfx(hDstDC);
//
//	gfx.DrawImage(gdiPlusBmp, 0,0 );
//
//	gfx.ReleaseHDC(hDstDC);
//
//	SelectBitmap(hDstDC, oldDstBmp);
//	DeleteDC(hDstDC);
//	DeleteDC(hScreen);
//	return hDstBmp;
//}

HBITMAP dcxLoadBitmap(HBITMAP dest, TString &filename) {
	filename.trim();

	if (dest != NULL) {
		DeleteBitmap(dest);
		dest = NULL;
	}
	if (!IsFile(filename)) {
		Dcx::errorex(TEXT("dcxLoadBitmap"), TEXT("Could Not Access File: %s"), filename.to_chr());
		return NULL;
	}

#ifdef DCX_USE_GDIPLUS
	if (Dcx::GDIModule.isUseable()) {
#if UNICODE
		Bitmap *p_Img = new Bitmap(filename.to_chr());
#else
		Bitmap *p_Img = new Bitmap(filename.to_wchr());
#endif
		if (p_Img != NULL) {
			// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
			Status status = p_Img->GetLastStatus();
			if (status != Ok)
				Dcx::error(TEXT("dcxLoadBitmap"), GetLastStatusStr(status));
			else {
				if ((status = p_Img->GetHBITMAP(Color(),&dest)) != Ok) {
					Dcx::error(TEXT("dcxLoadBitmap"),TEXT("Unable to Get GDI+ Bitmap Info"));
					Dcx::error(TEXT("dcxLoadBitmap"), GetLastStatusStr(status));
					dest = NULL;
				}
				//if (dest != NULL) {
				//	dest = GDIPlusToBitmap(p_Img);
				//}
				GdiFlush();
			}
			delete p_Img;
		}
		else
			Dcx::error(TEXT("dcxLoadBitmap"), TEXT("Unable to Allocate Image Object"));
	}
	else
		dest = (HBITMAP) LoadImage(GetModuleHandle(NULL), filename.to_chr(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
#else
	dest = (HBITMAP) LoadImage(GetModuleHandle(NULL), filename.to_chr(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
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
//      If the function fails, the return value is NULL.
//
//HICON CreateGrayscaleIcon(HICON hIcon) {
//	HICON		hGrayIcon = NULL;
//	HDC		hMainDC = NULL, hMemDC1 = NULL, hMemDC2 = NULL;
//	BITMAP	bmp;
//	HBITMAP	hOldBmp1 = NULL, hOldBmp2 = NULL;
//	ICONINFO	csII, csGrayII;
//	//BOOL		bRetValue = FALSE;
//
//	if (!GetIconInfo(hIcon, &csII))
//		return NULL;
//
//	ZeroMemory(&csGrayII, sizeof(ICONINFO));
//
//	hMainDC = GetDC(NULL);
//	hMemDC1 = CreateCompatibleDC(hMainDC);
//	hMemDC2 = CreateCompatibleDC(hMainDC);
//
//	if (hMainDC == NULL || hMemDC1 == NULL || hMemDC2 == NULL)
//		return NULL;
//
//	if (GetObject(csII.hbmColor, sizeof(BITMAP), &bmp)) {
//		DWORD dwWidth = csII.xHotspot*2;
//		DWORD dwHeight = csII.yHotspot*2;
//
//		csGrayII.hbmColor = CreateBitmap(dwWidth, dwHeight, bmp.bmPlanes, bmp.bmBitsPixel, NULL);
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
//	ReleaseDC(NULL, hMainDC);
//
//	DestroyIcon(hIcon);
//
//	return hGrayIcon;
//} // End of CreateGrayscaleIcon

COLORREF defaultGrayPalette[256];
BOOL bGrayPaletteSet = FALSE;

HICON CreateGrayscaleIcon( HICON hIcon, COLORREF* pPalette )
{
	if (hIcon == NULL)
		return NULL;

	HDC hdc = ::GetDC(NULL);

	HICON      hGrayIcon      = NULL;
	ICONINFO   icInfo         = { 0 };
	ICONINFO   icGrayInfo     = { 0 };
	//LPDWORD    lpBits         = NULL;
	//LPBYTE     lpBitsPtr      = NULL;
	//SIZE sz;
	//DWORD c1 = 0;
	BITMAPINFO bmpInfo        = { 0 };
	bmpInfo.bmiHeader.biSize  = sizeof(BITMAPINFOHEADER);

	if (::GetIconInfo(hIcon, &icInfo))
	{
		if (::GetDIBits(hdc, icInfo.hbmColor, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS) != 0)
		{
			SIZE sz;
			bmpInfo.bmiHeader.biCompression = BI_RGB;

			sz.cx = bmpInfo.bmiHeader.biWidth;
			sz.cy = bmpInfo.bmiHeader.biHeight;
			DWORD c1 = sz.cx * sz.cy;

			LPDWORD lpBits = (LPDWORD)::GlobalAlloc(GMEM_FIXED, (c1) * 4);

			if (lpBits && ::GetDIBits(hdc, icInfo.hbmColor, 0, sz.cy, lpBits, &bmpInfo, DIB_RGB_COLORS) != 0)
			{
				LPBYTE lpBitsPtr     = (LPBYTE)lpBits;
				//UINT off      = 0;

				for (UINT i = 0; i < c1; i++)
				{
					UINT off = (UINT)( 255 - (( lpBitsPtr[0] + lpBitsPtr[1] + lpBitsPtr[2] ) / 3) );

					if (lpBitsPtr[3] != 0 || off != 255)
					{
						if (off == 0)
						{
							off = 1;
						}

						lpBits[i] = pPalette[off] | ( lpBitsPtr[3] << 24 );
					}

					lpBitsPtr += 4;
				}

				icGrayInfo.hbmColor = ::CreateCompatibleBitmap(hdc, sz.cx, sz.cy);

				if (icGrayInfo.hbmColor != NULL)
				{
					::SetDIBits(hdc, icGrayInfo.hbmColor, 0, sz.cy, lpBits, &bmpInfo, DIB_RGB_COLORS);

					icGrayInfo.hbmMask = icInfo.hbmMask;
					icGrayInfo.fIcon   = TRUE;

					hGrayIcon = ::CreateIconIndirect(&icGrayInfo);

					::DeleteObject(icGrayInfo.hbmColor);
				}

				::GlobalFree(lpBits);
				//lpBits = NULL;
			}
		}

		::DeleteObject(icInfo.hbmColor);
		::DeleteObject(icInfo.hbmMask);
	}

	::ReleaseDC(NULL,hdc);

	return hGrayIcon;
}

HICON CreateGrayscaleIcon( HICON hIcon )
{
	if (hIcon == NULL)
	{
		return NULL;
	}

	if (!bGrayPaletteSet)
	{
		for(int i = 0; i < 256; i++)
		{
			defaultGrayPalette[i] = RGB(255-i, 255-i, 255-i);
		}

		bGrayPaletteSet = TRUE;
	}

	return CreateGrayscaleIcon(hIcon, defaultGrayPalette);
}

void AddToolTipToolInfo(const HWND tiphwnd, const HWND ctrl)
{
		TOOLINFO ti;
		ZeroMemory(&ti,sizeof(TOOLINFO));
		ti.cbSize = sizeof(TOOLINFO);
		ti.hwnd = ctrl;
		ti.lpszText = LPSTR_TEXTCALLBACK;
		ti.uFlags = TTF_IDISHWND | TTF_TRANSPARENT | TTF_SUBCLASS | TTF_PARSELINKS;
		//ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_PARSELINKS;
		ti.uId = (UINT_PTR)ctrl;

		SendMessage(tiphwnd,TTM_ADDTOOL,NULL,(LPARAM)&ti);
}

void dcxDrawShadowText(HDC hdc, LPCWSTR pszText, UINT cch, RECT *pRect, DWORD dwFlags, COLORREF crText, COLORREF crShadow, int ixOffset, int iyOffset)
{
	if (DrawShadowText(hdc, pszText, cch, pRect, dwFlags, crText, crShadow, ixOffset, iyOffset) == 0)
	{
		if (dwFlags & DT_CALCRECT) {
			DrawTextW(hdc, pszText, cch, pRect, dwFlags);
			OffsetRect(pRect, ixOffset, iyOffset);
		}
		else {
			// Normal DrawShadow Failed...
			RECT rcOffset;
			COLORREF old_clr;
			CopyRect(&rcOffset, pRect);
			OffsetRect(&rcOffset, ixOffset, iyOffset);
			old_clr = SetTextColor(hdc, crShadow);
			DrawTextW(hdc, pszText, cch, &rcOffset, dwFlags);
			SetTextColor(hdc, crText);
			DrawTextW(hdc, pszText, cch, (LPRECT)pRect, dwFlags);
			SetTextColor(hdc, old_clr);
		}
	}
}
#ifdef DCX_USE_GDIPLUS
static const TCHAR *GDIErrors[] = {
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
const TCHAR *GetLastStatusStr(Status status)
{
	if (status > DCX_MAX_GDI_ERRORS)
		return GDIErrors[1]; // status not in table, return GenericError
	return GDIErrors[status];
}
#endif

/*
 * Tell if a file exists.
 * NB: What it really does is tell us if we can access the file, it can fail to get access for various reason on XP+.
 * After a false return use GetLastError() to find the actual fail reason if you care.
*/
bool IsFile(TString &filename)
{
	if (filename.len() <= 0)
		return false;

	PathUnquoteSpaces(filename.to_chr()); // Removes any "" around the path.
	// try & access the filename as is first.
	HANDLE hFile = CreateFile(filename.to_chr(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		return true;
	}
	// if that fails try & search for the file.
	TCHAR *buf = NULL, *f;
	// find buffer size needed.
	DWORD res = SearchPath(NULL,filename.to_chr(),NULL,0,buf,&f);
	if (res > 0) {
		// found file, alloc buffer & fill with path/file.
		buf = new TCHAR[res +1];
		res = SearchPath(NULL,filename.to_chr(),NULL,res,buf,&f);
	}
	if (res == 0) {// if find failed, exit
		delete [] buf;
		return false;
	}
	// now try & access the file we found.
	hFile = CreateFile(buf, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		filename = buf; // alter the filename to contain the full path.
		delete [] buf;
		return true;
	}
	delete [] buf;
	return false;
}

//#include <shlwapi.h>
//
//DWORD GetDllVersion(LPCTSTR lpszDllName)
//{
//    HINSTANCE hinstDll;
//    DWORD dwVersion = 0;
//
//    /* For security purposes, LoadLibrary should be provided with a 
//       fully-qualified path to the DLL. The lpszDllName variable should be
//       tested to ensure that it is a fully qualified path before it is used. */
//    hinstDll = LoadLibrary(lpszDllName);
//	
//    if(hinstDll)
//    {
//        DLLGETVERSIONPROC pDllGetVersion;
//        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, 
//                          TEXT("DllGetVersion"));
//
//        /* Because some DLLs might not implement this function, you
//        must test for it explicitly. Depending on the particular 
//        DLL, the lack of a DllGetVersion function can be a useful
//        indicator of the version. */
//
//        if(pDllGetVersion)
//        {
//            DLLVERSIONINFO dvi;
//            HRESULT hr;
//
//            ZeroMemory(&dvi, sizeof(dvi));
//            dvi.cbSize = sizeof(dvi);
//
//            hr = (*pDllGetVersion)(&dvi);
//
//            if(SUCCEEDED(hr))
//            {
//               dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
//            }
//        }
//
//        FreeLibrary(hinstDll);
//    }
//    return dwVersion;
//}

/*!
* \brief Render an mIRC formatted string, correctly applying ctrl codes.
*
* blah
*/
void getmIRCPalette(COLORREF *Palette, const int PaletteItems)
{
	TString colors;
	static const TCHAR com[] = TEXT("$color(0) $color(1) $color(2) $color(3) $color(4) $color(5) $color(6) $color(7) $color(8) $color(9) $color(10) $color(11) $color(12) $color(13) $color(14) $color(15)");
	Dcx::mIRC.tsEval(colors, com);

	int i = 0;

	while (i < PaletteItems) {
		Palette[i] = (COLORREF)colors.gettok( i +1 ).to_num();
		i++;
	}
}

int unfoldColor(const WCHAR *color) {
	UINT len = (2*(lstrlenW(color)+1));
	CHAR *strTmp = new CHAR[len]; // SIZE equals (2*(sizeof(tstr)+1)). This ensures enough
										 // room for the multibyte characters if they are two
										 // bytes long and a terminating null character.

	wcstombs(strTmp, (const wchar_t *) color, len);
	int nColor = atoi(strTmp);
	delete [] strTmp;
	//int nColor = atoi(color);

	while (nColor > 15) {
		nColor -=16;
	}

	return nColor;
}

//void calcStrippedRect(HDC hdc, const TString &txt, const UINT style, LPRECT rc, const bool ignoreleft)
//{
//	if (!ignoreleft || (style & DT_CENTER) || (style & DT_RIGHT)) {
//		TString stripped_txt;
//		const WCHAR *wtxt = txt.to_chr();
//		UINT pos = 0, len = (UINT)txt.len();
//
//		if ((len == 0) || (wtxt == NULL))
//			return;
//
//		// strip out ctrl codes to correctly position text.
//		for (WCHAR c = wtxt[pos]; pos < len; c = wtxt[++pos]) {
//			switch (c)
//			{
//			case 2:  // ctrl-b Bold
//			case 15: // ctrl-o
//			case 22: // ctrl-r Reverse
//			case 29: // ctrl-i Italics
//			case 31: // ctrl-u Underline
//				break;
//			case 3: // ctrl-k Colour
//				{
//					while (wtxt[pos+1] == 3) pos++; // remove multiple consecutive ctrl-k's
//					if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9') {
//						++pos;
//
//						if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9')
//							pos++;
//
//						// maybe a background color
//						if (wtxt[pos+1] == L',') {
//							++pos;
//
//							if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9') {
//								pos++;
//
//								if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9')
//									++pos;
//							}
//						}
//					}
//				}
//				break;
//			default:
//				stripped_txt += c;
//				break;
//			}
//		}
//		DrawText(hdc, stripped_txt.to_chr(), (int)stripped_txt.len(), rc, style | DT_CALCRECT);
//	}
//}

void mIRC_OutText(HDC hdc, TString &txt, LPRECT rcOut, const LPLOGFONT lf, const UINT iStyle, const COLORREF clrFG, const bool shadow)
{
	int len = (int)txt.len();
	if (len > 0) {
		TEXTMETRICW tm;
		HFONT hOldFont = SelectFont( hdc, CreateFontIndirect( lf ) );
		GetTextMetrics(hdc, &tm);
		RECT rcTmp = *rcOut;
		if ((iStyle & DT_CALCRECT) != DT_CALCRECT) {	// if DT_CALCRECT flag NOT given then do calcrect here.
			//DrawText(hdc, txt.to_chr(), len, &rcTmp, iStyle | DT_CALCRECT);
			if (shadow)
				dcxDrawShadowText(hdc,txt.to_chr(), len, &rcTmp, iStyle | DT_CALCRECT, clrFG, 0, 5, 5);
			else
				DrawText(hdc, txt.to_chr(), len, &rcTmp, iStyle | DT_CALCRECT);
		}
		if (shadow)
			dcxDrawShadowText(hdc,txt.to_chr(), len, &rcTmp, iStyle, clrFG, 0, 5, 5);
		else
			DrawText(hdc, txt.to_chr(), len, &rcTmp, iStyle);
		rcOut->left += (rcTmp.right - rcTmp.left) /*- tm.tmOverhang*/;
		DeleteFont(SelectFont( hdc, hOldFont ));
	}
	txt = TEXT("");
}

void mIRC_DrawText(HDC hdc, const TString &txt, LPRECT rc, const UINT style, const bool shadow)
{
	LOGFONT lf;
	const WCHAR *wtxt = txt.to_chr();
	int /*savedDC,*/ pos = 0, len = (int)txt.len();
	TString tmp;
	RECT rcOut = *rc;
	UINT iStyle = (style & ~(DT_CENTER|DT_RIGHT|DT_VCENTER)) | DT_LEFT; // make sure its to left
	bool /*usingBGclr = false,*/ usingRevTxt = false;

	if ((len == 0) || (wtxt == NULL)) // if no text just exit.
		return;

	// create an hdc buffer to avoid flicker during drawing.
	HDC *hBuffer = CreateHDCBuffer(hdc, rc);

	if (hBuffer == NULL)
		return;

	HDC oldHDC = hdc;
	hdc = *hBuffer;
	// change rcOut to be zero offset.
	OffsetRect(&rcOut,-rcOut.left, -rcOut.top);

	//savedDC = SaveDC(hdc);

	COLORREF clrFG, origFG = GetTextColor(hdc);
	COLORREF clrBG, origBG = GetBkColor(hdc);
	COLORREF cPalette[16] = {CLR_INVALID}; // mIRC palette

	getmIRCPalette(cPalette, 16); // get mIRC palette

	clrFG = origFG;
	clrBG = origBG;

	HFONT hFont = (HFONT) GetCurrentObject(hdc, OBJ_FONT);

	GetObject(hFont, sizeof(LOGFONT), &lf);

	LONG origWeight = lf.lfWeight;
	LONG origLeft = rc->left;

	SetBkMode(hdc,TRANSPARENT);

	if ((iStyle & DT_CALCRECT) != DT_CALCRECT) { // if NOT doing DT_CALCRECT calc rect needed here.
		//mIRC_DrawText(hdc, txt, &rcOut, style | DT_CALCRECT, shadow);
		if (((style & DT_CENTER) == DT_CENTER) || ((style & DT_RIGHT) == DT_RIGHT) || ((style & DT_VCENTER) == DT_VCENTER)) {
			// strip out ctrl codes to correctly position text.
			RECT rcTmp = *rc;
			TString t(txt);
			DrawText(hdc, t.strip().to_chr(), t.len(), &rcTmp, style | DT_CALCRECT);
			// style can be either center or right, not both, but it can be center+vcenter or right+vcenter
			if ((style & DT_CENTER) == DT_CENTER) { // get center text start offset
				// (total width) - (required width) / 2 = equal space to each side.
				origLeft = rcOut.left = (((rcOut.right - rcOut.left) - (rcTmp.right - rcTmp.left)) / 2);
			}
			else if ((style & DT_RIGHT) == DT_RIGHT) { // get right text start offset
				// adjust start to right
				origLeft = rcOut.left = rcOut.right - (rcTmp.right - rcTmp.left);
			}
			if ((style & DT_VCENTER) == DT_VCENTER) { // get Veritcal center text start offset
				rcOut.top += (((rcOut.bottom - rcOut.top) - (rcTmp.bottom - rcTmp.top)) / 2);
			}
		}

	}
	for (WCHAR c = wtxt[pos]; pos < len; c = wtxt[++pos]) {
		switch (c)
		{
		case 2: // Bold
			{
				if (tmp.len() > 0)
					mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
				if (lf.lfWeight == FW_BOLD)
					lf.lfWeight = origWeight;
				else
					lf.lfWeight = FW_BOLD;
			}
			break;
		case 3: // Colour
			{
				if (tmp.len() > 0)
					mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
				while (wtxt[pos+1] == 3) pos++; // remove multiple consecutive ctrl-k's
				SetBkMode(hdc,TRANSPARENT);
				//usingBGclr = false;
				if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9') {
					WCHAR colbuf[3];
					colbuf[0] = wtxt[pos +1];
					colbuf[1] = L'\0';
					++pos;

					if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9') {
						colbuf[1] = wtxt[pos +1];
						pos++;
					}

					// color code number
					clrFG = cPalette[unfoldColor(colbuf)];

					// maybe a background color
					if (wtxt[pos+1] == L',') {
						++pos;

						if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9') {
							colbuf[0] = wtxt[pos +1];
							colbuf[1] = L'\0';
							pos++;

							if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9') {
								colbuf[1] = wtxt[pos +1];
								++pos;
							}

							// color code number
							clrBG = cPalette[unfoldColor(colbuf)];
							SetBkMode(hdc,OPAQUE);
							//usingBGclr = true;
						}
					}
					if (usingRevTxt) { // reverse text swap fg & bg colours
						COLORREF ct = clrFG;
						clrFG = clrBG;
						clrBG = ct;
						SetBkMode(hdc,OPAQUE);
					}
				}
				else {
					clrFG = origFG;
					clrBG = origBG;
				}
				SetTextColor(hdc, clrFG);
				SetBkColor(hdc, clrBG);
			}
			break;
		case 15: // ctrl+o
			{
				if (tmp.len() > 0)
					mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
				while (wtxt[pos+1] == 15) pos++; // remove multiple consecutive ctrl-o's
				lf.lfWeight = origWeight;
				lf.lfUnderline = 0;
				clrFG = origFG;
				clrBG = origBG;
				SetTextColor(hdc, origFG);
				SetBkColor(hdc, origBG);
				SetBkMode(hdc,TRANSPARENT);
				//usingBGclr = false;
				usingRevTxt = false;
			}
			break;
		case 22: // ctrl+r
			{
				if (tmp.len() > 0)
					mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
				usingRevTxt = (usingRevTxt ? false : true);
				if (usingRevTxt)
					SetBkMode(hdc,OPAQUE);
				else
					SetBkMode(hdc,TRANSPARENT);
				COLORREF ct = clrBG;
				clrBG = clrFG;
				clrFG = ct;
				SetTextColor(hdc, clrFG);
				SetBkColor(hdc, clrBG);
			}
			break;
		case 29: // ctrl-i Italics
			{
				if (tmp.len() > 0)
					mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
				lf.lfItalic = (BYTE)(lf.lfItalic ? 0 : 1);
			}
			break;
		case 31: // ctrl+u
			{
				if (tmp.len() > 0)
					mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
				lf.lfUnderline = (BYTE)(lf.lfUnderline ? 0 : 1);
			}
			break;
		case 10:
		case 13:
			{
				if (iStyle & DT_SINGLELINE) { // when single line, replace with a space or ignore?
					//while ((wtxt[pos+1] == 13) || (wtxt[pos+1] == 10)) pos++; // remove multiple consecutive line feeds (causes exception??)
					tmp += TEXT(' '); //" ";
				}
				else {
					SIZE sz;
					int tlen = (int)tmp.len();
					GetTextExtentPoint32(hdc, tmp.to_chr(), tlen, &sz);
					if (tlen > 0)
						mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
					rcOut.top += sz.cy;
					rcOut.left = rc->left;
				}
			}
			break;
		default: // normal TCHAR
			{
				if ((iStyle & DT_SINGLELINE) != DT_SINGLELINE) { // don't bother if a single line.
					int tlen = (int)tmp.len();
					if (tlen > 0) {
						SIZE sz;
						int nFit;
						GetTextExtentExPoint(hdc, txt.to_chr(), tlen, (rcOut.right - rcOut.left), &nFit, NULL, &sz);
						if (nFit < tlen) {
							if (nFit > 0) {
								WCHAR o = tmp.to_chr()[nFit];
								//mIRC_OutText(hdc, tmp.wsub(0,nFit), &rcOut, &lf, iStyle, clrFG, shadow);
								TString tsSub(tmp.sub(0,nFit));
								mIRC_OutText(hdc, tsSub, &rcOut, &lf, iStyle, clrFG, shadow);
								//tmp = "";
								rcOut.top += sz.cy;
								rcOut.left = origLeft;
								tmp = o;
								//mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
							}
							else
								tmp = TEXT("");
						}
					}
				}
				tmp += c;
			}
			break;
		}
	}
	if (tmp.len() > 0)
		mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
	//RestoreDC(hdc, savedDC);

	if ((iStyle & DT_CALCRECT) != DT_CALCRECT)
		BitBlt(oldHDC, rc->left, rc->top, (rc->right - rc->left), (rc->bottom - rc->top), hdc, 0, 0, SRCCOPY);
	else { // calc rect only
		OffsetRect(&rcOut, rc->left, rc->top);
		CopyRect(rc, &rcOut);
	}

	hdc = oldHDC;
	DeleteHDCBuffer(hBuffer);
}

typedef struct tagHDCBuffer {
	HDC m_hHDC;
	HBITMAP m_hOldBitmap;
	HBITMAP m_hBitmap;
	HFONT m_hOldFont;
} HDCBuffer, *LPHDCBuffer;

HDC *CreateHDCBuffer(HDC hdc, const LPRECT rc)
{
	if ((hdc == NULL) /*|| (rc == NULL)*/)
		return NULL;

	// alloc buffer data
	LPHDCBuffer buf = new HDCBuffer;

	if (buf == NULL)
		return NULL;

	// create HDC for buffer.
	buf->m_hHDC = CreateCompatibleDC(hdc);
	if (buf->m_hHDC == NULL) {
		delete buf;
		return NULL;
	}

	// get size of bitmap to alloc.
	BITMAP bm;
	int x, y;

	if (rc == NULL) {
		// no size specified, use hdc's bitmap size.
		GetObject((HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP), sizeof(BITMAP), &bm);
		x = 0;
		y = 0;
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

	if (buf->m_hBitmap == NULL) {
#ifdef DEBUG
		DWORD err = GetLastError(), errBufSize = 16;
		LPTSTR errBuf = NULL;
		if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, 0, (LPTSTR)&errBuf, errBufSize, NULL) != 0) {
			Dcx::mIRC.debug(TEXT("CreateHDCBuffer"),errBuf);
			LocalFree(errBuf);
		}
#endif
		DeleteDC(buf->m_hHDC);
		delete buf;
		return NULL;
	}
	// select bitmap into hdc
	buf->m_hOldBitmap = SelectBitmap(buf->m_hHDC, buf->m_hBitmap);

	// copy settings from hdc to buffer's hdc.
	SetDCBrushColor(buf->m_hHDC, GetDCBrushColor(hdc));
	SetDCPenColor(buf->m_hHDC, GetDCPenColor(hdc));
	SetLayout(buf->m_hHDC, GetLayout(hdc));
	buf->m_hOldFont = SelectFont(buf->m_hHDC, GetCurrentObject(hdc, OBJ_FONT));
	SetTextColor(buf->m_hHDC, GetTextColor(hdc));
	SetBkColor(buf->m_hHDC, GetBkColor(hdc));
	SetBkMode(buf->m_hHDC, GetBkMode(hdc));
	SetROP2(buf->m_hHDC, GetROP2(hdc));
	SetMapMode(buf->m_hHDC, GetMapMode(hdc));
	SetPolyFillMode(buf->m_hHDC, GetPolyFillMode(hdc));
	SetStretchBltMode(buf->m_hHDC, GetStretchBltMode(hdc));

	// copy contents of hdc within area to buffer.
	BitBlt(buf->m_hHDC, 0, 0, bm.bmWidth, bm.bmHeight, hdc, x, y, SRCCOPY);

	// buffer is an exact duplicate of the hdc within the area specified.
	// return buffer typed as an HDC *
	return (HDC *)buf;
}

void DeleteHDCBuffer(HDC *hBuffer)
{
	if (hBuffer == NULL)
		return;

	LPHDCBuffer buf = (LPHDCBuffer)hBuffer;

	GdiFlush();
	SelectFont(buf->m_hHDC, buf->m_hOldFont);
	SelectBitmap(buf->m_hHDC,buf->m_hOldBitmap);
	DeleteBitmap(buf->m_hBitmap);
	DeleteDC(buf->m_hHDC);
	delete buf;
}

int TGetWindowText(HWND hwnd, TString &txt)
{
	int nText = GetWindowTextLength(hwnd);
	if (nText > 0) {
		PTCHAR text = new TCHAR[++nText];
		GetWindowText(hwnd, text, nText);
		txt = text;
		delete [] text;
	}
	else
		txt = TEXT("");
	return nText;
}

HMODULE UXModule = NULL;             //!< UxTheme.dll Module Handle
#ifdef DCX_USE_GDIPLUS
HMODULE GDIPlusModule = NULL;					//!< gdiplus.dll Module Handle
ULONG_PTR gdi_token = NULL;
#endif
HMODULE DWMModule = NULL;							//!< dwmapi.dll Module Handle

void FreeOSCompatibility(void)
{
#ifdef DCX_USE_GDIPLUS
	// Shutdown GDI+
	if (GDIPlusModule != NULL) {
		if (gdi_token != NULL)
			GdiplusShutdown(gdi_token);

		FreeLibrary(GDIPlusModule);
		GDIPlusModule = NULL;
	}
#endif

	if (UXModule != NULL) {
#ifdef DCX_USE_WINSDK
		Dcx::UXModule.dcxBufferedPaintUnInit();
#endif

		FreeLibrary(UXModule);
		UXModule = NULL;
	}
	if (DWMModule != NULL) {
		FreeLibrary(DWMModule);
		DWMModule = NULL;
	}
}

BOOL isRegexMatch(const TCHAR *matchtext, const TCHAR *pattern)
{
#ifdef DCX_USE_BOOST
		try {
			//TString pat(pattern);
			//boost::regex_constants::_match_flags rflags = boost::regex_constants::match_any;
			//if (pat.left(1) == TEXT('/')) {
			//	pat = pat.right( -1 );
			//	if (pat.right(1) == TEXT('/')) {
			//		pat = pat.left(-1);
			//	}
			//	else {
			//		for (int l = pat.len() -1; l >= 0; l--) {
			//			switch (pat[l]) {
			//				case TEXT('/'):
			//					pat = pat.left(l);
			//					l = -1;
			//					break;
			//				case TEXT('g'):
			//				case TEXT('m'):
			//					break;
			//				case TEXT('i'):
			//					break;
			//				default:
			//					l = -1;
			//					break;
			//			}
			//		}
			//	}
			//}
			boost::regex re(pattern, boost::regex_constants::perl);

			if (boost::regex_search(matchtext, re,  boost::regex_constants::match_any | boost::regex_constants::match_single_line))
				return TRUE;
		}
		catch (...) {
			return FALSE;
		}
#else
#ifdef DCX_USE_PCRE
	TString pat(pattern);
	int pcre_opts = 0;
	bool bEndOpts = false;

	if (pat[0] == TEXT('/') || pat[1] == TEXT('/')) {
		if (pat[0] == TEXT('m')) {
			pcre_opts |= PCRE_MULTILINE;
			pat = pat.right(-2);
		}
		else
			pat = pat.right(-1);

		for (int l = pat.len() -1; l >= 0; l--) {
			switch (pat[l]) {
				case TEXT('/'):
					{
						pat = pat.left(l);
						l = -1;
					}
					break;
				case TEXT('g'):
					{
						pcre_opts |= 0;
					}
					break;
				case TEXT('i'):
					{
						pcre_opts |= PCRE_CASELESS;
					}
					break;
				case TEXT('x'):
					{
						pcre_opts |= PCRE_EXTENDED;
					}
					break;
				case TEXT('s'):
					{
						pcre_opts &= ~PCRE_MULTILINE;
						pcre_opts |= PCRE_DOLLAR_ENDONLY|PCRE_DOTALL;
					}
					break;
				case TEXT('e'):
					break;
				case TEXT('A'):
					break;
				default:
					l = -1;
					break;
			}
		}
	}
	const TCHAR *err = NULL;
	int err_code = 0;
	pcre *reg = pcre_compile(pat.to_chr(), pcre_opts, &err, &err_code, NULL);

	if (reg != NULL) {
		int res[4];
		if (pcre_exec(reg, NULL, matchtext, lstrlen(matchtext),0, PCRE_NOTEMPTY, res, 3) > 0)
			return TRUE;
	}
#else
		TCHAR res[10];
		Dcx::mIRC.execex(TEXT("/set -nu1 %%dcx_text %s"), matchtext );
		Dcx::mIRC.execex(TEXT("/set -nu1 %%dcx_regex %s"), pattern );
		Dcx::mIRC.eval(res, 10, TEXT("$regex(%dcx_text,%dcx_regex)"));
		if ( dcx_atoi(res) > 0 )
			return TRUE;
#endif // DCX_USE_PCRE
#endif // DCX_USE_BOOST
		return FALSE;
}

bool AddFileIcons(HIMAGELIST himl, TString &filename, const bool bLarge, const int iIndex)
{
	if (!IsFile(filename))
		return false;

	int fIndex = 0, i = iIndex;
	HICON hIcon = NULL;

	do {
		if (bLarge)
			ExtractIconEx(filename.to_chr(), fIndex, &hIcon, NULL, 1);
		else
			ExtractIconEx(filename.to_chr(), fIndex, NULL, &hIcon, 1);

		if (hIcon != NULL) {
			if (i == -1)
				ImageList_ReplaceIcon(himl, -1, hIcon);
			else
				ImageList_ReplaceIcon(himl, i++, hIcon);
			DestroyIcon(hIcon);
		}
		fIndex++;
	} while (hIcon != NULL);
	return true;
}

BOOL dcxGetWindowRect(HWND hWnd, LPRECT lpRect)
{
	// as described in a comment at http://msdn.microsoft.com/en-us/library/ms633519(VS.85).aspx
	// GetWindowRect does not return the real size of a window if u are using vista with areo glass
	// using DwmGetWindowAttribute now to fix that (fixes bug 685)
	if (Dcx::VistaModule.isUseable())
		return (Dcx::VistaModule.dcxDwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, lpRect, sizeof(RECT)) == S_OK);

	return GetWindowRect(hWnd, lpRect);
}
/*
	*	DrawRotatedText() function taken from ms example & modified for our needs.
*/
void DrawRotatedText(const TString &strDraw, LPRECT rc, HDC hDC, const int nAngleLine/* = 0*/, const bool bEnableAngleChar /*= false*/, const int nAngleChar /*= 0*/) {

	if ((nAngleLine == 0) && (!bEnableAngleChar)) {
		TextOut(hDC, rc->left, rc->bottom, strDraw.to_chr(), strDraw.len());
		//DrawText(hDC, strDraw.to_chr(), strDraw.len(), rc, styles);
		return;
	}
	LOGFONT lf = { 0 };
	HFONT hFont = (HFONT) GetCurrentObject(hDC, OBJ_FONT);

	GetObject(hFont, sizeof(LOGFONT), &lf);

	// Set the background mode to transparent for the
	// text-output operation.
	int nOldBkMode = SetBkMode(hDC, TRANSPARENT);
	// Specify the angle to draw line
	lf.lfEscapement = nAngleLine*10;
	int nOldGMode;
	if( bEnableAngleChar ) // Enable character angle
	{
		// Set graphics mode to advance to enable orientation
		nOldGMode = SetGraphicsMode( hDC, GM_ADVANCED );
		// Specify the angle of characters
		lf.lfOrientation = nAngleChar*10;
	}
	else // Draw in normal mode
	{
		nOldGMode = SetGraphicsMode( hDC, GM_COMPATIBLE );
	}
	// Select the new font created
	hFont = CreateFontIndirect(&lf);
	if (hFont != NULL) {
		HFONT hPrevFont = SelectFont(hDC, hFont);
		// Draw text to screen
		//TextOut(hDC, rc->right / 2, rc->bottom / 2, strDraw.to_chr(), strDraw.len());
		TextOut(hDC, rc->left, rc->bottom, strDraw.to_chr(), strDraw.len());
		//DrawText(hDC, strDraw.to_chr(), strDraw.len(), rc, styles);
		SelectFont(hDC, hPrevFont);
		DeleteFont(hFont);
	}
	// Restore old values
	SetBkMode( hDC, nOldBkMode );
	SetGraphicsMode( hDC, nOldGMode );
} 
