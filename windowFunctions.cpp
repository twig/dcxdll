/*!
* \file windowFunctions.cpp
* \brief Methods related to manipulation of windows or information retrieval.
*
* This file contains the informative (debug/error/status) information for DCX dll routines.
*
* \author William Nguyen ( twig at genscripts dot net )
* \version 1.0
*
* \b Revisions
*
* © ScriptsDB.org - 2007-2008
*/
#include "defines.h"
#include "Dcx.h"

/// <summary>
/// Finds an owner of a dialog, used with styles.
/// </summary>
/// <param name="data"></param>
/// <param name="defaultWnd"></param>
/// <returns>Returns the owner HWND</returns>
HWND FindOwner(const TString& data, const gsl::not_null<HWND>& defaultWnd)
{
	if (data.empty())
		return defaultWnd;

	const auto i = data.findtok(TEXT("owner"), 1);

	// 'owner' token not found in data
	if (i == 0)
		return defaultWnd;

	// if there is a token after 'owner'
	if (i < data.numtok())
	{
		const auto tsHwnd(data.gettok(gsl::narrow_cast<int>(i) + 1));

		// if it is a number (HWND) passed		
		if (const auto wnd = to_hwnd(tsHwnd.to_<size_t>()); wnd)
			return wnd;

		// try to retrieve dialog hwnd from name
		if (const auto wnd = GetHwndFromString(tsHwnd); wnd)
			return wnd;
	}

	return defaultWnd;
}

std::optional<HWND> FindOwner(const TString& data)
{
	if (data.empty())
		return {};

	const auto i = data.findtok(TEXT("owner"), 1);

	// 'owner' token not found in data
	if (i == 0)
		return {};

	// if there is a token after 'owner'
	if (i < data.numtok())
	{
		const auto tsHwnd(data.gettok(gsl::narrow_cast<int>(i) + 1));

		// if it is a number (HWND) passed		
		if (const auto wnd = to_hwnd(tsHwnd.to_<size_t>()); wnd)
			return wnd;

		// try to retrieve dialog hwnd from name
		if (const auto wnd = GetHwndFromString(tsHwnd); wnd)
			return wnd;
	}

	return {};

	//if (auto wnd = FindOwner(data, nullptr); wnd)
	//	return wnd;
	//return {};
}

/*!
* \brief Retrieves a HWND from the string.
*/
HWND GetHwndFromString(const TString& str)
{
	if (str.empty())
		return nullptr;

	//return GetHwndFromString(str.to_chr());

	// test code to allow docking by hwnd (wtf its only 3 lines)
	if (const auto hwnd = to_hwnd(str.to_<size_t>()); IsWindow(hwnd))
		return hwnd;

	TString tsRes;
	//mIRCLinker::tsEvalex(tsRes, TEXT("$dialog(%s).hwnd"), str.to_chr());
	mIRCLinker::eval(tsRes, TEXT("$dialog(%).hwnd"), str);

	return to_hwnd(tsRes.to_<size_t>());
}

/*!
* \brief Retireves a HWND from the string.
*/
//HWND GetHwndFromString(gsl::not_null<const TCHAR *> str) {
//
//	// test code to allow docking by hwnd (wtf its only 3 lines)
//	auto hwnd = (HWND)dcx_atoi64(str);
//	if (IsWindow(hwnd))
//		return hwnd;
//
//	TCHAR res[20];
//	res[0] = 0;
//	mIRCLinker::evalex( res, Dcx::countof(res), TEXT("$dialog(%s).hwnd"), str);
//
//	return (HWND) dcx_atoi(res);
//}

// Removes window style to a window
void RemStyles(const HWND hwnd, const int parm, const long RemStyles) noexcept
{
	auto Styles = gsl::narrow_cast<DWORD>(GetWindowLong(hwnd, parm));
	Styles &= ~RemStyles;
	SetWindowLong(hwnd, parm, gsl::narrow_cast<LONG>(Styles));
}

//	Adds window styles to a window
void AddStyles(const HWND hwnd, const int parm, const long AddStyles) noexcept
{
	auto Styles = gsl::narrow_cast<DWORD>(GetWindowLong(hwnd, parm));
	Styles |= AddStyles;
	SetWindowLong(hwnd, parm, gsl::narrow_cast<LONG>(Styles));
}

/***************************************************/
/*				David Gallardo Llopis			   */
/*				                     			   */
/*		     Based on the code in the book		   */
/*       PROGRAMACION AVANZADA EN WINDOWS 2000     */
/*             at McGraw-Hill (c) 2000             */
/*                       by                        */
/*        J. Pascual, F. Charte, M.J. Segarra,     */
/*            J.A. Clavijo, A. de Antonio.         */
/*                                                 */
/*  The code in this book is based on an original  */
/*       code by Jean-Edouard Lachand-Robert       */
/***************************************************/

//HRGN BitmapRegion(HBITMAP hBitmap,COLORREF cTransparentColor,BOOL bIsTransparent)
//{
//	// We create an empty region
//	HRGN		hRegion = nullptr;
//	
//	// If the passed bitmap is nullptr, go away!
//	if (hBitmap == nullptr)
//		return nullptr;
//	
//	// Computation of the bitmap size
//	BITMAP		bmBitmap;
//
//	if (GetObject(hBitmap, sizeof(bmBitmap), &bmBitmap) == 0)
//		return nullptr;
//
//	// We create a memory context for working with the bitmap
//	// The memory context is compatible with the display context (screen)
//	HDC hMemDC = CreateCompatibleDC(nullptr);
//	
//	// If no context is created, go away, too!
//	if (hMemDC == nullptr)
//		return nullptr;
//
//	Auto(DeleteDC(hMemDC));
//
//	// In order to make the space for the region, we
//	// create a bitmap with 32bit depth color and with the
//	// size of the loaded bitmap!
//	//BITMAPINFOHEADER RGB32BITSBITMAPINFO=
//	//{ 
//	//	sizeof(BITMAPINFOHEADER), 
//	//	bmBitmap.bmWidth, 
//	//	bmBitmap.bmHeight, 
//	//	1,32,BI_RGB,0,0,0,0,0 
//	//};
//	BITMAPINFO RGB32BITSBITMAPINFO = {
//		sizeof(BITMAPINFOHEADER),
//		bmBitmap.bmWidth,
//		bmBitmap.bmHeight,
//		1,32,BI_RGB,0,0,0,0,0,0
//	};
//
//	// Here is the pointer to the bitmap data
//	VOID		*pBits;
//	
//	// With the previous information, we create the new bitmap!
//	HBITMAP		hNewBitmap;
//	hNewBitmap = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pBits, nullptr, 0);
//
//	// If the creation process succeded...
//	if (hNewBitmap == nullptr)
//		throw Dcx::dcxException("BitmapRegion() - CreateDIBSection() Failed: Invalid Parameter");
//
//	Auto(DeleteBitmap(hNewBitmap));
//
//	GdiFlush();
//	// We select the bitmap onto the created memory context
//	// and then we store the previosly selected bitmap on this context!
//	HBITMAP hPrevBmp = (HBITMAP)SelectObject(hMemDC, hNewBitmap);
//
//	Auto(SelectObject(hMemDC, hPrevBmp));
//	// We create another device context compatible with the first!
//	HDC hDC = CreateCompatibleDC(hMemDC);
//
//	// If success...
//	if (hDC != nullptr)
//	{
//		// We compute the number of bytes per row that the bitmap contains, rounding to 32 bit-multiples
//		BITMAP		bmNewBitmap;
//
//		GetObject(hNewBitmap, sizeof(bmNewBitmap), &bmNewBitmap);
//
//		while (bmNewBitmap.bmWidthBytes % 4)
//			bmNewBitmap.bmWidthBytes++;
//
//		// Copy of the original bitmap on the memory context!
//		HBITMAP		hPrevBmpOrg = (HBITMAP)SelectObject(hDC, hBitmap);
//		BitBlt(hMemDC, 0, 0, bmBitmap.bmWidth, bmBitmap.bmHeight, hDC, 0, 0, SRCCOPY);
//
//		// In order to optimize the code, we don't call the GDI each time we
//		// find a transparent pixel. We use a RGN_DATA structure were we store
//		// consecutive rectangles, until we have a large amount of them and then we crete
//		// the composed region with ExtCreateRgn(), combining it with the main region.
//		// Then we begin again initializing the RGN_DATA structure and doing another
//		// iteration, until the entire bitmap is analyzed.
//
//		// Also, in order to not saturate the Windows API with calls for reserving
//		// memory, we wait until NUMRECT rectangles are stores in order to claim
//		// for another NUMRECT memory space!
//#define NUMRECT	100			
//		DWORD maxRect = NUMRECT;
//
//		// We create the memory data
//		HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT)*maxRect));
//
//		if (hData != nullptr) {
//			RGNDATA *pData = (RGNDATA*)GlobalLock(hData);
//			pData->rdh.dwSize = sizeof(RGNDATAHEADER);
//			pData->rdh.iType = RDH_RECTANGLES;
//			pData->rdh.nCount = pData->rdh.nRgnSize = 0;
//			SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
//
//			// We study each pixel on the bitmap...
//			BYTE *Pixeles = (BYTE*)bmNewBitmap.bmBits + (bmNewBitmap.bmHeight - 1)*bmNewBitmap.bmWidthBytes;
//
//			// Main loop
//			for (int Row = 0; Row < bmBitmap.bmHeight; Row++)
//			{
//				// Horizontal loop
//				for (int Column = 0; Column < bmBitmap.bmWidth; Column++)
//				{
//					// We optimized searching for adjacent transparent pixels!
//					int Xo = Column;
//					RGBQUAD *Pixel = (RGBQUAD*)Pixeles + Column;
//
//					while (Column < bmBitmap.bmWidth)
//					{
//						BOOL bInRange = FALSE;
//
//						// If the color is that indicated as transparent...
//						if (Pixel->rgbRed == GetRValue(cTransparentColor) &&
//							Pixel->rgbGreen == GetGValue((cTransparentColor & 0xFFFF)) &&
//							Pixel->rgbBlue == GetBValue(cTransparentColor))
//							bInRange = TRUE;
//
//						if ((bIsTransparent) && (bInRange))
//							break;
//
//						if ((!bIsTransparent) && (!bInRange))
//							break;
//
//						Pixel++;
//						Column++;
//					} // while (Column < bm.bmWidth)
//
//					if (Column > Xo)
//					{
//						// We add the rectangle (Xo,Row),(Column,Row+1) to the region
//
//						// If the number of rectangles is greater then NUMRECT, we claim
//						// another pack of NUMRECT memory places!
//						if (pData->rdh.nCount >= maxRect)
//						{
//							GlobalUnlock(hData);
//							maxRect += NUMRECT;
//							hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT)*maxRect), GMEM_MOVEABLE);
//							pData = (RGNDATA *)GlobalLock(hData);
//						}	// if (pData->rdh.nCount>=maxRect)
//
//						RECT *pRect = (RECT*)&pData->Buffer;
//						SetRect(&pRect[pData->rdh.nCount], Xo, Row, Column, Row + 1);
//
//						if (Xo<pData->rdh.rcBound.left)
//							pData->rdh.rcBound.left = Xo;
//
//						if (Row<pData->rdh.rcBound.top)
//							pData->rdh.rcBound.top = Row;
//
//						if (Column>pData->rdh.rcBound.right)
//							pData->rdh.rcBound.right = Column;
//
//						if (Row + 1>pData->rdh.rcBound.bottom)
//							pData->rdh.rcBound.bottom = Row + 1;
//
//						pData->rdh.nCount++;
//
//						// In Win95/08 there is a limitation on the maximum number of
//						// rectangles a RGN_DATA can store (aprox. 4500), so we call
//						// the API for a creation and combination with the main region
//						// each 2000 rectangles. This is a good optimization, because
//						// instead of calling the routines for combining for each new
//						// rectangle found, we call them every 2000 rectangles!!!
//						if (pData->rdh.nCount == 2000)
//						{
//							HRGN hNewRegion = ExtCreateRegion(nullptr, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRect), pData);
//							if (hNewRegion != nullptr) {
//								// Si ya existe la región principal,sumamos la nueva,
//								// si no,entonces de momento la principal coincide con
//								// la nueva región.
//								if (hRegion) {
//									CombineRgn(hRegion, hRegion, hNewRegion, RGN_OR);
//									DeleteObject(hNewRegion);
//								}
//								else
//									hRegion = hNewRegion;
//
//
//							}	// if (hNewRegion != nullptr)
//							// Volvemos a comenzar la suma de rectángulos
//							pData->rdh.nCount = 0;
//							SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
//						}	// if(pData->rdh.nCount==2000)
//
//					} // if (Column > Xo)
//				} // for (int  Column ...)
//
//				// Nueva Row. Lo del negativo se debe a que el bitmap está invertido
//				// verticalmente.
//				Pixeles -= bmNewBitmap.bmWidthBytes;
//
//			} // for (int Row...)			
//
//			if (pData->rdh.nCount > 0) {
//				// Una vez finalizado el proceso,procedemos a la fusión de la
//				// región remanente desde la última fusión hasta el final			
//				HRGN hNewRegion = ExtCreateRegion(nullptr, sizeof(RGNDATAHEADER) + (sizeof(RECT)*maxRect), pData);
//
//				if (hNewRegion != nullptr)
//				{
//					// If the main region does already exist, we add the new one,
//					if (hRegion)
//					{
//						CombineRgn(hRegion, hRegion, hNewRegion, RGN_OR);
//						DeleteObject(hNewRegion);
//					}
//					else
//						// if not, we consider the new one to be the main region at first!
//						hRegion = hNewRegion;
//				}	// if(hNewRegion != nullptr)
//			}	// if (pData->rdh.nCount > 0)
//			// We free the allocated memory and the rest of used ressources
//			GlobalUnlock(hData);
//			GlobalFree(hData);
//			SelectObject(hDC, hPrevBmpOrg); // don't del prev bitmap, as its our supplied one.
//			DeleteDC(hDC);
//		}	// if (hData != nullptr)
//
//	}// if (hDC)
//
//	DeleteBitmap(SelectObject(hMemDC, hPrevBmp)); // del prev bitmap as it's the DIB Section
//	return hRegion;
//}

HRGN BitmapRegion(HBITMAP hBitmap, const COLORREF cTransparentColor, const bool bIsTransparent)
{
#if DCX_USE_WRAPPERS
	// If the passed bitmap is nullptr, go away!
	if (!hBitmap)
		return nullptr;

	// We create an empty region
	HRGN		hRegion = nullptr;

	// Computation of the bitmap size
	BITMAP		bmBitmap{};

	if (GetObject(hBitmap, sizeof(bmBitmap), &bmBitmap) == 0)
		throw Dcx::dcxException("BitmapRegion() - Unable to get bitmap info");

	// We create a memory context for working with the bitmap
	// The memory context is compatible with the display context (screen)

	const Dcx::dcxHDCResource hMemDC((HDC)nullptr);

	// In order to make the space for the region, we
	// create a bitmap with 32bit depth color and with the
	// size of the loaded bitmap!

	const BITMAPINFO RGB32BITSBITMAPINFO{
		sizeof(BITMAPINFOHEADER),
		bmBitmap.bmWidth,
		bmBitmap.bmHeight,
		1, 32, BI_RGB, 0, 0, 0, 0, 0, 0
	};

	// Here is the pointer to the bitmap data
	VOID* pBits = nullptr;

	// With the previous information, we create the new bitmap!

	const Dcx::dcxBitmapResource hNewBitmap(hMemDC, (BITMAPINFO*)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pBits, nullptr, 0);

	GdiFlush();
	// We select the bitmap onto the created memory context
	// and then we store the previosly selected bitmap on this context!
	const auto hPrevBmp = SelectObject(hMemDC, hNewBitmap);

	Auto(SelectObject(hMemDC, hPrevBmp));

	// We compute the number of bytes per row that the bitmap contains, rounding to 32 bit-multiples
	BITMAP		bmNewBitmap{};

	if (GetObject(hNewBitmap, sizeof(bmNewBitmap), &bmNewBitmap) == 0)
		throw Dcx::dcxException("BitmapRegion() - Unable to get bitmap info");

	while (bmNewBitmap.bmWidthBytes % 4)
		bmNewBitmap.bmWidthBytes++;

	// We create another device context compatible with the first!
	// Copy of the original bitmap on the memory context!

	const Dcx::dcxHDCBitmapResource hDC((HDC)hMemDC, hBitmap);

	BitBlt(hMemDC, 0, 0, bmBitmap.bmWidth, bmBitmap.bmHeight, hDC, 0, 0, SRCCOPY);
#else
	// We create an empty region
	HRGN		hRegion = nullptr;

	// If the passed bitmap is nullptr, go away!
	if (!hBitmap)
		return nullptr;

	// Computation of the bitmap size
	BITMAP		bmBitmap;

	if (GetObject(hBitmap, sizeof(bmBitmap), &bmBitmap) == 0)
		throw Dcx::dcxException("BitmapRegion() - Unable to get bitmap info");

	// We create a memory context for working with the bitmap
	// The memory context is compatible with the display context (screen)

	const HDC hMemDC = CreateCompatibleDC(nullptr);

	// If no context is created, go away, too!
	if (!hMemDC)
		throw Dcx::dcxException("BitmapRegion() - Unable to create DC");

	Auto(DeleteDC(hMemDC));

	// In order to make the space for the region, we
	// create a bitmap with 32bit depth color and with the
	// size of the loaded bitmap!
	//BITMAPINFOHEADER RGB32BITSBITMAPINFO=
	//{ 
	//	sizeof(BITMAPINFOHEADER), 
	//	bmBitmap.bmWidth, 
	//	bmBitmap.bmHeight, 
	//	1,32,BI_RGB,0,0,0,0,0 
	//};

	const BITMAPINFO RGB32BITSBITMAPINFO = {
		sizeof(BITMAPINFOHEADER),
		bmBitmap.bmWidth,
		bmBitmap.bmHeight,
		1, 32, BI_RGB, 0, 0, 0, 0, 0, 0
	};

	// Here is the pointer to the bitmap data
	VOID* pBits;

	// With the previous information, we create the new bitmap!

	const auto		hNewBitmap = CreateDIBSection(hMemDC, (BITMAPINFO*)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pBits, nullptr, 0);

	// If the creation process succeded...
	if (!hNewBitmap)
		throw Dcx::dcxException("BitmapRegion() - CreateDIBSection() Failed: Invalid Parameter");

	Auto(DeleteObject(hNewBitmap));

	GdiFlush();
	// We select the bitmap onto the created memory context
	// and then we store the previosly selected bitmap on this context!
	const auto hPrevBmp = SelectObject(hMemDC, hNewBitmap);

	Auto(SelectObject(hMemDC, hPrevBmp));

	// We create another device context compatible with the first!

	HDC hDC = CreateCompatibleDC(hMemDC);

	// If success...
	if (hDC == nullptr)
		throw Dcx::dcxException("BitmapRegion() - Unable to create DC");

	Auto(DeleteDC(hDC));

	// We compute the number of bytes per row that the bitmap contains, rounding to 32 bit-multiples
	BITMAP		bmNewBitmap;

	if (GetObject(hNewBitmap, sizeof(bmNewBitmap), &bmNewBitmap) == 0)
		throw Dcx::dcxException("BitmapRegion() - Unable to get bitmap info");

	while (bmNewBitmap.bmWidthBytes % 4)
		bmNewBitmap.bmWidthBytes++;

	// Copy of the original bitmap on the memory context!
	const auto		hPrevBmpOrg = SelectObject(hDC, hBitmap);

	Auto(SelectObject(hDC, hPrevBmpOrg));

	BitBlt(hMemDC, 0, 0, bmBitmap.bmWidth, bmBitmap.bmHeight, hDC, 0, 0, SRCCOPY);
#endif
	// In order to optimize the code, we don't call the GDI each time we
	// find a transparent pixel. We use a RGN_DATA structure were we store
	// consecutive rectangles, until we have a large amount of them and then we create
	// the composed region with ExtCreateRgn(), combining it with the main region.
	// Then we begin again initializing the RGN_DATA structure and doing another
	// iteration, until the entire bitmap is analyzed.

	// Also, in order to not saturate the Windows API with calls for reserving
	// memory, we wait until NUMRECT rectangles are stored in order to claim
	// for another NUMRECT memory space!
#define NUMRECT	100U			
	DWORD maxRect = NUMRECT;

	// We create the memory data
	auto hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRect));

	if (!hData)
		throw Dcx::dcxException("BitmapRegion() - GlobalAlloc() failed");

	Auto(GlobalFree(hData));

	auto pData = (RGNDATA*)GlobalLock(hData);

	Auto(GlobalUnlock(hData));

	pData->rdh.dwSize = sizeof(RGNDATAHEADER);
	pData->rdh.iType = RDH_RECTANGLES;
	pData->rdh.nCount = pData->rdh.nRgnSize = 0;
	SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

	// We study each pixel on the bitmap...
	auto Pixeles = (BYTE*)bmNewBitmap.bmBits + (bmNewBitmap.bmHeight - 1) * bmNewBitmap.bmWidthBytes;

	// Main loop
	for (auto Row = 0; Row < bmBitmap.bmHeight; ++Row)
	{
		// Horizontal loop
		for (auto Column = 0; Column < bmBitmap.bmWidth; ++Column)
		{
			// We optimized searching for adjacent transparent pixels!
			const int Xo = Column;
			const auto* Pixel = (RGBQUAD*)Pixeles + Column;

			while (Column < bmBitmap.bmWidth)
			{
				bool bInRange = false;

				// If the color is that indicated as transparent...
				if (Pixel->rgbRed == GetRValue(cTransparentColor) &&
					Pixel->rgbGreen == GetGValue((cTransparentColor & 0xFFFF)) &&
					Pixel->rgbBlue == GetBValue(cTransparentColor))
					bInRange = true;

				if ((bIsTransparent) && (bInRange))
					break;

				if ((!bIsTransparent) && (!bInRange))
					break;

				++Pixel;
				++Column;
			} // while (Column < bm.bmWidth)

			if (Column > Xo)
			{
				// We add the rectangle (Xo,Row),(Column,Row+1) to the region

				// If the number of rectangles is greater then NUMRECT, we claim
				// another pack of NUMRECT memory places!
				if (pData->rdh.nCount >= maxRect)
				{
					GlobalUnlock(hData);
					maxRect += NUMRECT;
					hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRect), GMEM_MOVEABLE);
					pData = (RGNDATA*)GlobalLock(hData);
				}	// if (pData->rdh.nCount>=maxRect)

				const auto pRect = (RECT*)&pData->Buffer[0];
				SetRect(&pRect[pData->rdh.nCount], Xo, Row, Column, Row + 1);

				if (Xo < pData->rdh.rcBound.left)
					pData->rdh.rcBound.left = Xo;

				if (Row < pData->rdh.rcBound.top)
					pData->rdh.rcBound.top = Row;

				if (Column > pData->rdh.rcBound.right)
					pData->rdh.rcBound.right = Column;

				if (Row + 1 > pData->rdh.rcBound.bottom)
					pData->rdh.rcBound.bottom = Row + 1;

				++pData->rdh.nCount;

				// In Win95/08 there is a limitation on the maximum number of
				// rectangles a RGN_DATA can store (aprox. 4500), so we call
				// the API for a creation and combination with the main region
				// each 2000 rectangles. This is a good optimization, because
				// instead of calling the routines for combining for each new
				// rectangle found, we call them every 2000 rectangles!!!
				if (pData->rdh.nCount == 2000)
				{
					if (auto hNewRegion = ExtCreateRegion(nullptr, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRect), pData); hNewRegion)
					{
						// Si ya existe la región principal,sumamos la nueva,
						// si no,entonces de momento la principal coincide con
						// la nueva región.
						if (hRegion)
						{
							CombineRgn(hRegion, hRegion, hNewRegion, RGN_OR);
							DeleteObject(hNewRegion);
						}
						else
							hRegion = hNewRegion;


					}	// if (hNewRegion != nullptr)
					// Volvemos a comenzar la suma de rectángulos
					pData->rdh.nCount = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
				}	// if(pData->rdh.nCount==2000)

			} // if (Column > Xo)
		} // for (int  Column ...)

		// Nueva Row. Lo del negativo se debe a que el bitmap está invertido
		// verticalmente.
		Pixeles -= bmNewBitmap.bmWidthBytes;

	} // for (int Row...)			

	if (pData->rdh.nCount > 0)
	{
		// Una vez finalizado el proceso,procedemos a la fusión de la
		// región remanente desde la última fusión hasta el final			
		if (auto hNewRegion = ExtCreateRegion(nullptr, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRect), pData); hNewRegion)
		{
			// If the main region does already exist, we add the new one,
			if (hRegion)
			{
				CombineRgn(hRegion, hRegion, hNewRegion, RGN_OR);
				DeleteObject(hNewRegion);
			}
			else
				// if not, we consider the new one to be the main region at first!
				hRegion = hNewRegion;
		}	// if(hNewRegion != nullptr)
	}	// if (pData->rdh.nCount > 0)

	return hRegion;
}

/*!
* \brief Changes the icon for a window.
*
* throws dcxException on error.
*/
void ChangeHwndIcon(const HWND hwnd, const TString& flags, const int index, TString& filename)
{
	if (!hwnd)
		throw Dcx::dcxException("ChangeHwndIcon() - Invalid Window");

	const XSwitchFlags xflags(flags);

	filename.trim();
	if (!xflags[TEXT('+')])
		throw Dcx::dcxException("ChangeHwndIcon() - Invalid Flags");

	HICON iconSmall{ nullptr };
	HICON iconLarge{ nullptr };

	if (filename == TEXT("none"))
	{
		dcxSetWindowExStyle(hwnd, WindowExStyle::DialogModalFrame);

		//SetClassLongW(hwnd, GCL_HICON, 0);
		//SetClassLongW(hwnd, GCL_HICONSM, 0);

		iconSmall = (HICON)SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)iconSmall);
		iconLarge = (HICON)SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)iconLarge);
	}
	else if (filename == TEXT("default"))
	{
		if ((!xflags[TEXT('s')]) && (!xflags[TEXT('b')]))
		{
			iconSmall = (HICON)SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)iconSmall);
			iconLarge = (HICON)SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)iconLarge);
		}
		else {
			if (xflags[TEXT('s')])
				iconSmall = (HICON)SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)iconSmall);
			if (xflags[TEXT('b')])
				iconLarge = (HICON)SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)iconLarge);
		}
	}
	else {
		if (!xflags[TEXT('B')])
		{
			// not base64 data as filename
			if (!IsFile(filename))
				throw Dcx::dcxException(TEXT("ChangeHwndIcon() - Unable to Access File: %"), filename);
		}

		// check for +s small icon flag
		if (xflags[TEXT('s')])
			iconSmall = dcxLoadIcon(index, filename, false, flags);
		// check for +b big icon flag
		if (xflags[TEXT('b')])
			iconLarge = dcxLoadIcon(index, filename, true, flags);

		if ((!iconLarge) && (!iconSmall))
		{
			// No big or small flags, so do both icons.
			iconSmall = dcxLoadIcon(index, filename, false, flags);
			iconLarge = dcxLoadIcon(index, filename, true, flags);
		}
		// set the new icons, get back the current icon
		if (iconSmall)
			iconSmall = (HICON)SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)iconSmall);
		if (iconLarge)
			iconLarge = (HICON)SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)iconLarge);
	}

	// delete the old icons
	if (iconSmall)
		DestroyIcon(iconSmall);
	if ((iconLarge) && (iconSmall != iconLarge)) // dont delete twice
		DestroyIcon(iconLarge);
}

bool GetWindowRectParent(const HWND hwnd, RECT* rcWin) noexcept
{
	if ((!hwnd) || (!rcWin))
		return false;

#if DCX_USE_WRAPPERS
	const Dcx::dcxWindowRect rc(hwnd, GetParent(hwnd));
	return (CopyRect(rcWin, &rc) != FALSE);
#else
	if (GetWindowRect(hwnd, rcWin))
	{
		SetLastError(0U);
		MapWindowRect(nullptr, GetParent(hwnd), rcWin.get());
		return (GetLastError() == 0U);
	}
	return false;
#endif
}

/// <summary>
/// draws a line using a pre set pen/colour.
/// </summary>
/// <param name="hdc">- The HDC to draw on.</param>
/// <param name="x1">- The x start of the line.</param>
/// <param name="y1">- The y start of the line.</param>
/// <param name="x2">- The x end of the line.</param>
/// <param name="y2">- The y end of the line.</param>
/// <returns></returns>
void dcxDrawLine(HDC hdc, LONG x1, LONG y1, LONG x2, LONG y2) noexcept
{
	if (!hdc)
		return;

	MoveToEx(hdc, x1, y1, nullptr);
	LineTo(hdc, x2, y2);
}

/// <summary>
/// draws a edge using a specified colour.
/// falls back on DrawEdge() if it fails to allocate a pen or an invalid colour is supplied.
/// </summary>
/// <param name="hdc">- The HDC to draw on.</param>
/// <param name="rc">- The bounding rect for the edge. (atm only drawing on the right side of the rect is supported)</param>
/// <param name="clr">- The colour to use for the edge.</param>
/// <returns></returns>
void dcxDrawEdge(HDC hdc, const LPRECT rc, COLORREF clr) noexcept
{
	if (!hdc)
		return;

	if (clr != CLR_INVALID)
	{
		if (auto hPen = CreatePen(PS_SOLID, 5, clr); hPen)
		{
			Auto(DeleteObject(hPen));

			const auto oldPen = Dcx::dcxSelectObject<HPEN>(hdc, hPen);
			Auto(Dcx::dcxSelectObject<HPEN>(hdc, oldPen));

			dcxDrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom);
			return;
		}
	}

	DrawEdge(hdc, rc, EDGE_BUMP, BF_RIGHT);
}

/// <summary>
/// draws a border using a specified colour.
/// </summary>
/// <param name="hdc">- The HDC to draw on.</param>
/// <param name="lprc">- The bounding rect for the border.</param>
/// <param name="dwBorder">- The type of border to draw, (BF_LEFT BF_RIGHT BF_TOP BF_BOTTOM)</param>
/// <param name="clr">- The colour to use.</param>
/// <returns></returns>
void dcxDrawBorder(HDC hdc, LPCRECT lprc, DWORD dwBorder, COLORREF clr) noexcept
{
	if (!hdc || !lprc)
		return;

	LOGPEN oLogPen{};

	auto hOld = SelectObject(hdc, GetStockObject(BLACK_PEN));
	GetObject(hOld, sizeof(oLogPen), &oLogPen);
	oLogPen.lopnColor = clr;

	//Don't attempt to delete stock object
	SelectObject(hdc, CreatePenIndirect(&oLogPen));

	if (dwBorder & BF_LEFT)
		dcxDrawLine(hdc, lprc->left, lprc->top, lprc->left, lprc->bottom);
	if (dwBorder & BF_TOP)
		dcxDrawLine(hdc, lprc->left, lprc->top, lprc->right, lprc->top);
	if (dwBorder & BF_RIGHT)
		dcxDrawLine(hdc, lprc->right, lprc->top, lprc->right, lprc->bottom);
	if (dwBorder & BF_BOTTOM)
		dcxDrawLine(hdc, lprc->left, lprc->bottom, lprc->right, lprc->bottom);

	DeleteObject(SelectObject(hdc, hOld));
}

namespace
{
	COLORREF getCheckBoxBkgColour(const clrCheckBox* lpcol, DWORD dState) noexcept
	{
		if (!lpcol)
			return RGB(0, 0, 0);

		if (dcx_testflag(dState, CDIS_DISABLED))
			return lpcol->m_clrDisabledBackground;

		if (dcx_testflag(dState, CDIS_HOT))
			return lpcol->m_clrHotBackground;

		return lpcol->m_clrBackground;
	}

	COLORREF getCheckBoxFrameColour(const clrCheckBox* lpcol, DWORD dState) noexcept
	{
		if (!lpcol)
			return RGB(0, 0, 0);

		if (dcx_testflag(dState, CDIS_DISABLED))
			return lpcol->m_clrDisabledFrame;

		if (dcx_testflag(dState, CDIS_HOT))
			return lpcol->m_clrHotFrame;

		return lpcol->m_clrFrame;
	}

	COLORREF getCheckBoxHighliteFrameColour(const clrCheckBox* lpcol, DWORD dState) noexcept
	{
		//return GetSysColor(COLOR_3DHIGHLIGHT);
		return GetContrastColour(getCheckBoxFrameColour(lpcol, dState));
	}

	COLORREF getCheckBoxTickColour(const clrCheckBox* lpcol, DWORD dState) noexcept
	{
		if (!lpcol)
			return RGB(0, 0, 0);

		if (dcx_testflag(dState, CDIS_DISABLED))
			return lpcol->m_clrDisabledTick;

		if (dcx_testflag(dState, CDIS_HOT))
			return lpcol->m_clrHotTick;

		return lpcol->m_clrTick;
	}
}

/// <summary>
/// Draw a checkbox.
/// </summary>
/// <param name="hDC"></param>
/// <param name="rcBox"></param>
/// <param name="lpcol"></param>
/// <param name="bTicked"></param>
/// <param name="bDis"></param>
/// <param name="bRounded"></param>
/// <returns></returns>
void dcxDrawCheckBox(HDC hDC, const LPCRECT rcBox, const clrCheckBox* lpcol, const DWORD dState, const bool bTicked, const bool bRounded) noexcept
{
	if (!hDC || !lpcol || !rcBox)
		return;

	// create background brush
	const auto hBrush = CreateSolidBrush(getCheckBoxBkgColour(lpcol, dState));
	if (!hBrush)
		return;
	Auto(DeleteObject(hBrush));

	// create border pen
	const auto hPenBorder = CreatePen(PS_SOLID, 1, getCheckBoxFrameColour(lpcol, dState));
	if (!hPenBorder)
		return;
	Auto(DeleteObject(hPenBorder));

	// create border highlite pen
	const auto hPenHighBorder = CreatePen(PS_SOLID, 1, getCheckBoxHighliteFrameColour(lpcol, dState));
	if (!hPenHighBorder)
		return;
	Auto(DeleteObject(hPenHighBorder));

	// create tick pen
	const auto hPenTick = CreatePen(PS_SOLID, 1, getCheckBoxTickColour(lpcol, dState));
	if (!hPenTick)
		return;
	Auto(DeleteObject(hPenTick));

	// set background brush
	const auto hOldBrush = SelectObject(hDC, hBrush);
	Auto(SelectObject(hDC, hOldBrush));

	RECT rc = *rcBox;

	InflateRect(&rc, 0, -1);
	rc.left += 1;
	rc.right = rc.left + rc.bottom - rc.top;

	if (getCheckBoxBkgColour(lpcol, dState) != getCheckBoxFrameColour(lpcol, dState))
	{
		// draw tick box
		const auto hOldPenBorder = SelectObject(hDC, hPenBorder);
		Auto(SelectObject(hDC, hOldPenBorder));

		if (bRounded)
		{
			RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, 5, 5);

			InflateRect(&rc, -1, -1);

			const auto hOldHighPenBorder = SelectObject(hDC, hPenHighBorder);
			Auto(SelectObject(hDC, hOldHighPenBorder));

			RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, 5, 5);
		}
		else {
			//dcxDrawBorder(hDC, &rc, BF_RECT, getCheckBoxFrameColour(lpcol, dState));

			Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);

			InflateRect(&rc, -1, -1);

			const auto hOldHighPenBorder = SelectObject(hDC, hPenHighBorder);
			Auto(SelectObject(hDC, hOldHighPenBorder));

			Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
		}
	}

	if (bTicked)
	{
		// draw tick
		const auto hOldPenTick = SelectObject(hDC, hPenTick);
		Auto(SelectObject(hDC, hOldPenTick));

		const auto x = (rc.right + rc.left) / 2 - 3;
		const auto y = (rc.bottom + rc.top) / 2 - 3;

		// Draw the tick using DrawLine
		//dcxDrawLine(hDC, x, y + 2, x, y + 5);
		//dcxDrawLine(hDC, x + 1, y + 3, x + 1, y + 6);
		//dcxDrawLine(hDC, x + 2, y + 4, x + 2, y + 7);
		//dcxDrawLine(hDC, x + 3, y + 3, x + 3, y + 6);
		//dcxDrawLine(hDC, x + 4, y + 2, x + 4, y + 5);
		//dcxDrawLine(hDC, x + 5, y + 1, x + 5, y + 4);
		//dcxDrawLine(hDC, x + 6, y, x + 6, y + 3);

		// Draw the tick using Polyline
		//const POINT pts[14]{
		//	{x, y + 2},
		//	{x, y + 5},
		//	{x + 1, y + 3},
		//	{x + 1, y + 6},
		//	{x + 2, y + 4},
		//	{x + 2, y + 7},
		//	{x + 3, y + 3},
		//	{x + 3, y + 6},
		//	{x + 4, y + 2},
		//	{x + 4, y + 5},
		//	{x + 5, y + 1},
		//	{x + 5, y + 4},
		//	{x + 6, y},
		//	{x + 6, y + 3},
		//};
		//Polyline(hDC, &pts[0], std::size(pts));

		// Draw the tick using PolyDraw
		const POINT pts[14]{
			{x, y + 2},
			{x, y + 5},

			{x + 1, y + 3},
			{x + 1, y + 6},

			{x + 2, y + 4},
			{x + 2, y + 7},

			{x + 3, y + 3},
			{x + 3, y + 6},

			{x + 4, y + 2},
			{x + 4, y + 5},

			{x + 5, y + 1},
			{x + 5, y + 4},

			{x + 6, y},
			{x + 6, y + 3},
		};
		const static BYTE flags[14]{
			PT_MOVETO,
			PT_LINETO,
			PT_MOVETO,
			PT_LINETO,
			PT_MOVETO,
			PT_LINETO,
			PT_MOVETO,
			PT_LINETO,
			PT_MOVETO,
			PT_LINETO,
			PT_MOVETO,
			PT_LINETO,
			PT_MOVETO,
			PT_LINETO
		};
		PolyDraw(hDC, &pts[0], &flags[0], std::size(pts));
	}
}

/// <summary>
/// Draw a radio style selector.
/// </summary>
/// <param name="hDC"></param>
/// <param name="rcBox"></param>
/// <param name="lpcol"></param>
/// <param name="dState"></param>
/// <param name="bTicked"></param>
/// <param name="bRounded"></param>
void dcxDrawRadioBox(HDC hDC, const LPCRECT rcBox, const clrCheckBox* lpcol, const DWORD dState, const bool bTicked, const bool bRounded) noexcept
{
	if (!hDC || !lpcol || !rcBox)
		return;

	// create background brush
	const auto hBrush = CreateSolidBrush(getCheckBoxBkgColour(lpcol, dState));
	if (!hBrush)
		return;
	Auto(DeleteObject(hBrush));

	// create border pen
	const auto hPenBorder = CreatePen(PS_SOLID, 1, getCheckBoxFrameColour(lpcol, dState));
	if (!hPenBorder)
		return;
	Auto(DeleteObject(hPenBorder));

	// create border highlite pen
	const auto hPenHighBorder = CreatePen(PS_SOLID, 1, getCheckBoxHighliteFrameColour(lpcol, dState));
	if (!hPenHighBorder)
		return;
	Auto(DeleteObject(hPenHighBorder));

	// create tick pen
	const auto hPenTick = CreatePen(PS_SOLID, 1, getCheckBoxTickColour(lpcol, dState));
	if (!hPenTick)
		return;
	Auto(DeleteObject(hPenTick));

	// set background brush
	const auto hOldBrush = SelectObject(hDC, hBrush);
	Auto(SelectObject(hDC, hOldBrush));

	RECT rc = *rcBox;

	InflateRect(&rc, -5, -5);

	InflateRect(&rc, 0, -1);
	rc.left += 1;
	rc.right = rc.left + rc.bottom - rc.top;

	if (getCheckBoxBkgColour(lpcol, dState) != getCheckBoxFrameColour(lpcol, dState))
	{
		// draw tick box
		const auto hOldPenBorder = SelectObject(hDC, hPenBorder);
		Auto(SelectObject(hDC, hOldPenBorder));

		if (bRounded)
		{
			Ellipse(hDC, rc.left, rc.top, rc.right, rc.bottom);

			InflateRect(&rc, -1, -1);

			const auto hOldHighPenBorder = SelectObject(hDC, hPenHighBorder);
			Auto(SelectObject(hDC, hOldHighPenBorder));

			Ellipse(hDC, rc.left, rc.top, rc.right, rc.bottom);
		}
		else {
			Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);

			InflateRect(&rc, -1, -1);

			const auto hOldHighPenBorder = SelectObject(hDC, hPenHighBorder);
			Auto(SelectObject(hDC, hOldHighPenBorder));

			Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
		}
	}

	if (bTicked)
	{
		// draw tick
		const auto hOldPenTick = SelectObject(hDC, hPenTick);
		Auto(SelectObject(hDC, hOldPenTick));

		InflateRect(&rc, -1, -1);

		if (bRounded)
			Ellipse(hDC, rc.left, rc.top, rc.right, rc.bottom);
		else
			Dcx::FillRectColour(hDC, &rc, getCheckBoxTickColour(lpcol, dState));
	}
}

HWND dcxGetRealParent(HWND hWnd) noexcept
{
	// To obtain a window's owner window, instead of using GetParent,
	// use GetWindow with the GW_OWNER flag.

	if (HWND hWndOwner = GetWindow(hWnd, GW_OWNER); hWndOwner)
		return hWndOwner;

	// Obtain the parent window and not the owner
	return GetAncestor(hWnd, GA_PARENT);
}

GSL_SUPPRESS(lifetime)
bool dcxDrawRect(HDC hDC, LPCRECT rc, COLORREF clr, COLORREF clrBorder, bool bRounded) noexcept
{
	if (!hDC || !rc)
		return false;

	const auto hPen = CreatePen(PS_SOLID, 1, clrBorder);

	if (!hPen)
		return false;
	Auto(DeleteObject(hPen));

	const auto hOldPen = SelectObject(hDC, hPen);
	Auto(SelectObject(hDC, hOldPen));

	const auto hBrush = CreateSolidBrush(clr);
	if (!hBrush)
		return false;

	Auto(DeleteObject(hBrush));

	const auto hOldBrush = SelectObject(hDC, hBrush);
	Auto(SelectObject(hDC, hOldBrush));

	if (bRounded)
		RoundRect(hDC, rc->left, rc->top, rc->right, rc->bottom, 5, 5);
	else
		Rectangle(hDC, rc->left, rc->top, rc->right, rc->bottom);

	return true;
}

GSL_SUPPRESS(r.3)
bool dcxDrawTranslucentRect(HDC hDC, LPCRECT rc, COLORREF clr, COLORREF clrBorder, bool bRounded) noexcept
{
	if (!hDC || !rc)
		return false;

	if (!Dcx::UXModule.IsBufferedPaintSupported())
		return dcxDrawRect(hDC, rc, clr, clrBorder, bRounded);

	// 0x7f half of 0xff = 50% transparency
	// 0xCC = 80% Opaque
	const BLENDFUNCTION ai_bf{ AC_SRC_OVER, 0, 0xC0, 0 };
	BP_PAINTPARAMS paintParams{ sizeof(BP_PAINTPARAMS),BPPF_ERASE, nullptr, &ai_bf };
	HDC hdc{};

	auto ai_Buffer = Dcx::UXModule.dcxBeginBufferedPaint(hDC, rc, BPBF_COMPATIBLEBITMAP, &paintParams, &hdc);
	if (!ai_Buffer)
		return false;

	Auto(Dcx::UXModule.dcxEndBufferedPaint(ai_Buffer, TRUE));

	return dcxDrawRect(hdc, rc, clr, clrBorder, bRounded);
}

bool dcxDrawBitMap(HDC hdc, LPCRECT prc, HBITMAP hbm, bool bStretch, bool bAlpha) noexcept
{
	if (!hdc || !prc || !hbm)
		return false;

	auto hdcMemory = ::CreateCompatibleDC(hdc);

	if (!hdcMemory)
		return false;

	const auto hbmpOld = Dcx::dcxSelectObject<HBITMAP>(hdcMemory, hbm);

	Auto(Dcx::dcxSelectObject<HBITMAP>(hdcMemory, hbmpOld));

	if (bStretch || bAlpha)
	{
		if (auto [code, bm] = Dcx::dcxGetObject<BITMAP>(hbm); code != 0)
		{
			if (bStretch && !bAlpha)
				StretchBlt(hdc, prc->left, prc->top, (prc->right - prc->left), (prc->bottom - prc->top), hdcMemory, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
			else {
				const BLENDFUNCTION bf{ AC_SRC_OVER, 0, 0xC0, 0 };
				AlphaBlend(hdc, prc->left, prc->top, (prc->right - prc->left), (prc->bottom - prc->top), hdcMemory, 0, 0, bm.bmWidth, bm.bmHeight, bf);
			}
		}
	}
	else
		BitBlt(hdc, prc->left, prc->top, (prc->right - prc->left), (prc->bottom - prc->top), hdcMemory, 0, 0, SRCCOPY);


	return true;
}

int dcxSetStretchModeHalfTone(HDC hdc) noexcept
{
	//SetStretchBltMode(hdc, STRETCH_HALFTONE);
	//SetBrushOrgEx(hdc, 0, 0, nullptr);

	POINT pt{};
	GetBrushOrgEx(hdc, &pt);
	const auto oldMode = SetStretchBltMode(hdc, STRETCH_HALFTONE);
	SetBrushOrgEx(hdc, pt.x, pt.y, nullptr);
	return oldMode;
}

void dcxStretchHalfToneBlt(HDC hdcDest, const RECT& rcDest, HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc) noexcept
{
	POINT pt{};
	GetBrushOrgEx(hdcDest, &pt);
	const auto oldMode = SetStretchBltMode(hdcDest, STRETCH_HALFTONE);
	SetBrushOrgEx(hdcDest, pt.x, pt.y, nullptr);

	StretchBlt(hdcDest, rcDest.left, rcDest.top, rcDest.right - rcDest.left, rcDest.bottom - rcDest.top, hdcSrc, xSrc, ySrc, wSrc, hSrc, SRCCOPY);

	SetStretchBltMode(hdcDest, oldMode);
	SetBrushOrgEx(hdcDest, pt.x, pt.y, nullptr);
}

void dcxTransparentHalfToneBlt(HDC hdcDest, const RECT& rcDest, HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc, COLORREF clrTrans) noexcept
{
	POINT pt{};
	GetBrushOrgEx(hdcDest, &pt);
	const auto oldMode = SetStretchBltMode(hdcDest, STRETCH_HALFTONE);
	SetBrushOrgEx(hdcDest, pt.x, pt.y, nullptr);

	TransparentBlt(hdcDest, rcDest.left, rcDest.top, rcDest.right - rcDest.left, rcDest.bottom - rcDest.top, hdcSrc, xSrc, ySrc, wSrc, hSrc, clrTrans);

	SetStretchBltMode(hdcDest, oldMode);
	SetBrushOrgEx(hdcDest, pt.x, pt.y, nullptr);
}

BITMAP dcxGetCurrentBitmap(HDC hdc) noexcept
{
	BITMAP bm{};
	if (!hdc)
		return bm;

	auto hObj = Dcx::dcxGetCurrentObject<HBITMAP>(hdc, OBJ_BITMAP);
	if (!hObj)
		return bm;

	if (GetObject(hObj, sizeof(BITMAP), &bm) == 0)
		ZeroMemory(&bm, sizeof(BITMAP));

	return bm;
}

SIZE dcxGetBitmapDimensions(HBITMAP hBM) noexcept
{
	if (!hBM)
		return {};

	BITMAP bm{};
	if (GetObject(hBM, sizeof(BITMAP), &bm) == 0)
		ZeroMemory(&bm, sizeof(BITMAP));

	return { bm.bmWidth, bm.bmHeight };
}

RECT dcxGetBitmapRect(HBITMAP hBM) noexcept
{
	const auto sz = dcxGetBitmapDimensions(hBM);
	return { 0,0, sz.cx, sz.cy };
}

SIZE dcxGetCurrentBitmapDimensions(HDC hdc) noexcept
{
	const auto bm = dcxGetCurrentBitmap(hdc);
	return { bm.bmWidth, bm.bmHeight };
}

RECT dcxGetCurrentBitmapRect(HDC hdc) noexcept
{
	const auto bm = dcxGetCurrentBitmap(hdc);
	return { 0,0, bm.bmWidth, bm.bmHeight };
}

/// <summary>
/// Copy an HDC's contents to a bitmap.
/// </summary>
/// <param name="hBm"></param>
/// <param name="xDest"></param>
/// <param name="yDest"></param>
/// <param name="wDest"></param>
/// <param name="hDest"></param>
/// <param name="hdc"></param>
/// <param name="xSrc"></param>
/// <param name="ySrc"></param>
/// <returns></returns>
bool CopyHDCToBitmap(HBITMAP hBm, LONG xDest, LONG yDest, LONG wDest, LONG hDest, HDC hdc, LONG xSrc, LONG ySrc) noexcept
{
	if (!hdc || !hBm)
		return false;

	auto hTmp = CreateCompatibleDC(hdc);
	if (!hTmp)
		return false;
	Auto(DeleteDC(hTmp));

	auto hOld = Dcx::dcxSelectObject(hTmp, hBm);
	Auto(Dcx::dcxSelectObject(hTmp, hOld));

	return (BitBlt(hTmp, xDest, yDest, wDest, hDest, hdc, xSrc, ySrc, SRCCOPY) != FALSE);
}

/// <summary>
/// Copy a bitmap to an HDC
/// </summary>
/// <param name="hdc"></param>
/// <param name="xDest"></param>
/// <param name="yDest"></param>
/// <param name="wDest"></param>
/// <param name="hDest"></param>
/// <param name="hBm"></param>
/// <param name="xSrc"></param>
/// <param name="ySrc"></param>
/// <returns></returns>
bool CopyBitmapToHDC(HDC hdc, LONG xDest, LONG yDest, LONG wDest, LONG hDest, HBITMAP hBm, LONG xSrc, LONG ySrc) noexcept
{
	if (!hdc || !hBm)
		return false;

	auto hTmp = CreateCompatibleDC(hdc);
	if (!hTmp)
		return false;
	Auto(DeleteDC(hTmp));

	auto hOld = Dcx::dcxSelectObject(hTmp, hBm);
	Auto(Dcx::dcxSelectObject(hTmp, hOld));

	return (BitBlt(hdc, xDest, yDest, wDest, hDest, hTmp, xSrc, ySrc, SRCCOPY) != FALSE);
}

void dcxDrawGradientText(HDC hdc, LPCTSTR txt, int len, LPRECT pRC, UINT fmt, COLORREF clrStart, COLORREF clrEnd, bool bHoriz, bool bCtrlCodes, bool bShadow)
/// <summary>
/// <para>Draw text using a gradient effect.</para>
/// <para>NB: can NOT be used with BeginPath/EndPath etc...</para>
/// <para>NB: This function can be very slow.</para>
/// </summary>
/// <param name="hdc"></param>
/// <param name="txt"></param>
/// <param name="len"></param>
/// <param name="pRC"></param>
/// <param name="fmt"></param>
/// <param name="dTO"></param>
void dcxDrawGradientText(HDC hdc, LPCWSTR txt, int len, LPRECT pRC, UINT fmt, const dcxTextOptions& dTO)
{
	// draw text where the letters are gradient coloured.

	if (!hdc || !pRC || !txt || len <= 0 || IsRectEmpty(pRC))
		return;

	const SIZE sz{ (pRC->right - pRC->left), (pRC->bottom - pRC->top) };

	const auto hBufTxt = CreateHDCBuffer(hdc, pRC);
	if (!hBufTxt)
		return;
	Auto(DeleteHDCBuffer(hBufTxt));
	HDC hdcTxt = *hBufTxt;
	RECT rcTxt = *pRC;
	OffsetRect(&rcTxt, -rcTxt.left, -rcTxt.top);

	const auto clrTxt = GetTextColor(hdc);
	const auto bkgclr = GetContrastColour(clrTxt);
	Dcx::FillRectColour(hdcTxt, pRC, bkgclr);

	if (!dTO.m_bCtrlCodes)
	{
		SetBkMode(hdcTxt, (dTO.m_bTransparent) ? TRANSPARENT : OPAQUE);

		if (dTO.m_bShadow)
			dcxDrawShadowText(hdcTxt, txt, gsl::narrow_cast<UINT>(len), &rcTxt, fmt, clrTxt, dTO.m_clrShadow, dTO.m_uShadowXOffset, dTO.m_uShadowYOffset);
		else
			DrawTextW(hdcTxt, txt, gsl::narrow_cast<int>(len), &rcTxt, fmt);
	}
	else
		mIRC_DrawText(hdcTxt, txt, &rcTxt, fmt, dTO.m_bShadow);

	const auto hBufGrad = CreateHDCBuffer(hdc, pRC);
	if (!hBufGrad)
		return;
	Auto(DeleteHDCBuffer(hBufGrad));
	HDC hdcGrad = *hBufGrad;
	XPopupMenuItem::DrawGradient(hdcGrad, &rcTxt, dTO.m_clrGradientTextStart, dTO.m_clrGradientTextEnd, dTO.m_bHorizGradientFill);

	//GetDIBits();
	//SetDIBits();

	for (long y{}; y < rcTxt.bottom; ++y)
	{
	for (long x{}; x < rcTxt.right; ++x)
	{
			const auto pix = GetPixel(hdcTxt, x, y);
			if (pix == bkgclr)
				continue;

			const auto gradPix = GetPixel(hdcGrad, x, y);
			const BYTE newR{ gsl::narrow_cast<BYTE>((GetRValue(gradPix) * GetRValue(pix)) / BYTE{0xFFu}) };
			const BYTE newG{ gsl::narrow_cast<BYTE>((GetGValue(gradPix) * GetGValue(pix)) / BYTE{0xFFu}) };
			const BYTE newB{ gsl::narrow_cast<BYTE>((GetBValue(gradPix) * GetBValue(pix)) / BYTE{0xFFu}) };
			const COLORREF newpix = RGB(newR, newG, newB);
			SetPixelV(hdc, x, y, newpix);
		}
	}
		}

/// <summary>
/// <para>Draws text as a gradient.</para>
/// <para>NB: can NOT be used with BeginPath/EndPath etc...</para>
/// </summary>
/// <param name="hdc"></param>
/// <param name="txt"></param>
/// <param name="len"></param>
/// <param name="pRC"></param>
/// <param name="fmt"></param>
/// <param name="dTO"></param>
void dcxDrawGradientTextMasked(HDC hdc, LPCWSTR txt, int len, LPRECT pRC, UINT fmt, const dcxTextOptions& dTO)
{
	// draw text where the letters are gradient coloured.

	if (!hdc || !pRC || !txt || len <= 0 || IsRectEmpty(pRC))
		return;

	const SIZE sz{ (pRC->right - pRC->left), (pRC->bottom - pRC->top) };

	const auto hBufTxt = CreateHDCBuffer(hdc, pRC);
	if (!hBufTxt)
		return;
	Auto(DeleteHDCBuffer(hBufTxt));
	HDC hdcTxt = *hBufTxt;
	RECT rcTxt = *pRC;
	OffsetRect(&rcTxt, -rcTxt.left, -rcTxt.top);

	const auto hBufGrad = CreateHDCBuffer(hdc, pRC);
	if (!hBufGrad)
		return;
	Auto(DeleteHDCBuffer(hBufGrad));
	HDC hdcGrad = *hBufGrad;

	auto hbmMask = CreateBitmap(sz.cx, sz.cy, 1, 1, nullptr);
	if (!hbmMask)
		return;
	Auto(DeleteBitmap(hbmMask));

	//DrawTextW(hdcTxt, txt, len, &rcTxt, fmt);

	const auto vec = dcxBreakdownmIRCText(txt, gsl::narrow_cast<UINT>(len));
	if (vec.empty())
		return;
	mIRC_DrawBreakdown(hdcTxt, vec, &rcTxt, fmt, dTO);

	// create mask
	{
		const auto oldbm = SelectBitmap(hdcGrad, hbmMask);
		Auto(SelectBitmap(hdcGrad, oldbm));

		const auto oldClr = SetBkColor(hdcTxt, GetTextColor(hdc));
		BitBlt(hdcGrad, 0, 0, sz.cx, sz.cy, hdcTxt, 0, 0, SRCCOPY);
		SetBkColor(hdcTxt, oldClr);
	}

	XPopupMenuItem::DrawGradient(hdcGrad, &rcTxt, dTO.m_clrGradientTextStart, dTO.m_clrGradientTextEnd, dTO.m_bHorizGradientFill);

	////constexpr auto DSTCOPY = 0x00AA0029;
	////constexpr DWORD hafs = MAKEROP4(SRCCOPY, DSTCOPY);
	//constexpr DWORD hafs = 0xAACC0000;
	//MaskBlt(hdc, pRC->left, pRC->top, sz.cx, sz.cy, hdcGrad, 0, 0, hbmMask, 0, 0, hafs);

	const POINT pts[3]{ {pRC->left,pRC->top},{pRC->right,pRC->top},{pRC->left,pRC->bottom} };
	PlgBlt(hdc, &pts[0], hdcGrad, 0, 0, sz.cx, sz.cy, hbmMask, 0, 0);
}

/// <summary>
/// <para>Draws text as an outline.</para>
/// <para>NB: Only works with larger font sizes and only with outline fonts (TrueType or OpenType)</para>
/// <para>NB: can NOT be used with BeginPath/EndPath etc...</para>
/// </summary>
/// <param name="hdc"></param>
/// <param name="txt"></param>
/// <param name="len"></param>
/// <param name="pRC"></param>
/// <param name="fmt"></param>
/// <param name="dTO"></param>
void dcxDrawOutlineText(HDC hdc, LPCWSTR txt, int len, LPRECT pRC, UINT fmt, const dcxTextOptions& dTO)
{
	if (!hdc || !pRC || !txt || len <= 0 || IsRectEmpty(pRC))
		return;

	const auto hBufTxt = CreateHDCBuffer(hdc, pRC);
	Auto(DeleteHDCBuffer(hBufTxt));
	HDC hdcTxt = *hBufTxt;

	if (!BeginPath(hdcTxt))
		return;

	RECT rcTxt = *pRC;
	OffsetRect(&rcTxt, -rcTxt.left, -rcTxt.top);

	const auto clrTxt = (dTO.m_clrText != CLR_INVALID) ? dTO.m_clrText : GetTextColor(hdc);
	const auto clrOutline = (dTO.m_clrOutline != CLR_INVALID) ? dTO.m_clrOutline : GetContrastColour(clrTxt);

	if (!dTO.m_bCtrlCodes)
	{
		const auto oldBkgMode = SetBkMode(hdcTxt, TRANSPARENT);
		Auto(SetBkMode(hdcTxt, oldBkgMode));

		if (dTO.m_bShadow)
			dcxDrawShadowText(hdcTxt, txt, gsl::narrow_cast<UINT>(len), &rcTxt, fmt, clrTxt, dTO.m_clrShadow, dTO.m_uShadowXOffset, dTO.m_uShadowYOffset);
		else {
			SetTextColor(hdcTxt, clrTxt);
			DrawTextW(hdcTxt, txt, gsl::narrow_cast<int>(len), &rcTxt, fmt);
	}
	}
	else
		mIRC_DrawTextNoBuffer(hdcTxt, txt, &rcTxt, fmt, dTO.m_bShadow);

	EndPath(hdcTxt);

	if (auto hPen = CreatePen(PS_SOLID, dTO.m_uOutlineSize, clrOutline); hPen)
	{
		Auto(DeletePen(hPen));

		auto hOldPen = SelectPen(hdcTxt, hPen);
		Auto(SelectPen(hdcTxt, hOldPen));

		if (auto hBrush = CreateSolidBrush(clrTxt); hBrush)
		{
			auto hOldBrush = SelectBrush(hdcTxt, hBrush);
			Auto(SelectBrush(hdcTxt, hOldBrush));

			if (dTO.m_bFilledOutline)
				StrokeAndFillPath(hdcTxt);
			else
				StrokePath(hdcTxt);
		}
	}
	BitBlt(hdc, pRC->left, pRC->top, pRC->right - pRC->left, pRC->bottom - pRC->top, hdcTxt, 0, 0, SRCCOPY);
}

/// <summary>
/// Draws text based on supplied options.
/// </summary>
/// <param name="hdc"></param>
/// <param name="txt"></param>
/// <param name="len"></param>
/// <param name="pRC"></param>
/// <param name="mStyle">- The DT_* text styles to use.</param>
/// <param name="dTO"></param>
void dcxDrawTextOptions(HDC hdc, LPCWSTR txt, int len, LPRECT pRC, UINT mStyle, const dcxTextOptions& dTO)
{
	if (!hdc || !txt || !pRC || len <= 0 || IsRectEmpty(pRC))
		return;

	//if (Dcx::GDIModule.isUseable())
	//{
	//	dcxDrawGDIPlusTextOptions(hdc, txt, len, pRC, mStyle, dTO);
	//	return;
	//}

	const auto vec = dcxBreakdownmIRCText(txt);
	if (vec.empty())
		return;

	const auto hBufTxt = CreateHDCBuffer(hdc, pRC);
	if (!hBufTxt)
		return;

	Auto(DeleteHDCBuffer(hBufTxt));
	HDC hdcTxt = *hBufTxt;
	RECT rcTxt = *pRC;
	OffsetRect(&rcTxt, -rcTxt.left, -rcTxt.top);

	const auto clrTxt = (dTO.m_clrText != CLR_INVALID) ? dTO.m_clrText : GetTextColor(hdcTxt);
	const auto clrOutline = (dTO.m_clrOutline != CLR_INVALID) ? dTO.m_clrOutline : GetContrastColour(clrTxt);

	SetTextColor(hdcTxt, clrTxt);
	if (dTO.m_clrTextBackground != CLR_INVALID)
		SetBkColor(hdcTxt, dTO.m_clrTextBackground);

	const bool bDoPath = (dTO.m_bFilledOutline || dTO.m_bOutline || dTO.m_bGradientFill);

	if (dTO.m_bTransparent && !bDoPath)
		SetBkMode(hdcTxt, TRANSPARENT);

	if (bDoPath)
		BeginPath(hdcTxt);

	mIRC_DrawBreakdown(hdcTxt, vec, &rcTxt, mStyle, dTO);

	if (bDoPath)
	{
		EndPath(hdcTxt);

		auto hOutlinePen = CreatePen(PS_SOLID, dTO.m_uOutlineSize, clrOutline);
		if (!hOutlinePen)
			return;
		Auto(DeletePen(hOutlinePen));

		auto hOldPen = SelectPen(hdcTxt, hOutlinePen);
		Auto(SelectPen(hdcTxt, hOldPen));

		auto hFillBrush = CreateSolidBrush(clrTxt);
		if (!hFillBrush)
			return;
		Auto(DeleteBrush(hFillBrush));

		auto hOldBrush = SelectBrush(hdcTxt, hFillBrush);
		Auto(SelectBrush(hdcTxt, hOldBrush));

		if (!dTO.m_bGradientFill || dTO.m_bOutline)
	{
			// not a gradient so must be an outline
			if (dTO.m_bFilledOutline)
				StrokeAndFillPath(hdcTxt);
		else
				StrokePath(hdcTxt);
	}
		else {
			if (dTO.m_bGradientOutline)
				WidenPath(hdcTxt);

			// TODO: fix shadow look for gradient and outlines...
			// TODO: get gradient outlines working...

			FillPath(hdcTxt);

		//if (HRGN hRgn = PathToRegion(hdcTxt); hRgn)
		//{
		//	Auto(DeleteRgn(hRgn));
			//	const auto hBufGrad = CreateHDCBuffer(hdc, pRC);
			//	if (!hBufGrad)
			//		return;
			//	Auto(DeleteHDCBuffer(hBufGrad));
			//	HDC hdcGrad = *hBufGrad;
			//	SelectClipRgn(hdcGrad, hRgn);
			//	XPopupMenuItem::DrawGradient(hdcGrad, &rcTxt, dTO.m_clrGradientTextStart, dTO.m_clrGradientTextEnd, dTO.m_bHorizGradientFill);
			//	SelectClipRgn(hdcGrad, nullptr);
		//
			//	if (dTO.m_bFilledOutline)
		//	{
		//		if (auto hOutlineBrush = CreateSolidBrush(clrOutline); hOutlineBrush)
		//		{
		//			Auto(DeleteBrush(hOutlineBrush));
		//		
			//			FrameRgn(hdcGrad, hRgn, hOutlineBrush, dTO.m_uOutlineSize, dTO.m_uOutlineSize);
		//		}
		//	}
			//	BitBlt(hdcTxt, 0, 0, rcTxt.right, rcTxt.bottom, hdcGrad, 0, 0, SRCCOPY);
		//}

			const auto hBufGrad = CreateHDCBuffer(hdc, pRC);
			if (!hBufGrad)
				return;
			Auto(DeleteHDCBuffer(hBufGrad));
			HDC hdcGrad = *hBufGrad;

			auto hbmMask = CreateBitmap(rcTxt.right, rcTxt.bottom, 1, 1, nullptr);
			if (!hbmMask)
				return;
			Auto(DeleteBitmap(hbmMask));

			// create mask
					{
				const auto oldbm = SelectBitmap(hdcGrad, hbmMask);
				Auto(SelectBitmap(hdcGrad, oldbm));

				const auto oldClr = SetBkColor(hdcTxt, clrTxt);
				BitBlt(hdcGrad, 0, 0, rcTxt.right, rcTxt.bottom, hdcTxt, 0, 0, SRCCOPY);
				SetBkColor(hdcTxt, oldClr);
		}
			XPopupMenuItem::DrawGradient(hdcGrad, &rcTxt, dTO.m_clrGradientTextStart, dTO.m_clrGradientTextEnd, dTO.m_bHorizGradientFill);

				if (dTO.m_bFilledOutline)
				StrokePath(hdcTxt);

			//constexpr DWORD DSTCOPY = 0x00AA0029;
			//constexpr DWORD hafs = MAKEROP4(SRCCOPY, DSTCOPY);
			//constexpr DWORD hafs = 0xAACC0000;
			//MaskBlt(hdc, pRC->left, pRC->top, rcTxt.right, rcTxt.bottom, hdcGrad, 0, 0, hbmMask, 0, 0, hafs);

			// these points are the wrong way round, BUT they cause the text to be flipped, which is a nice effect for the future.
			//const POINT pts[3]{ {rcTxt.top,rcTxt.left }, { rcTxt.top, rcTxt.right }, { rcTxt.bottom, rcTxt.left } };

			const POINT pts[3]{ { rcTxt.left,rcTxt.top }, { rcTxt.right,rcTxt.top }, { rcTxt.left,rcTxt.bottom } };
			PlgBlt(hdcTxt, &pts[0], hdcGrad, 0, 0, rcTxt.right, rcTxt.bottom, hbmMask, 0, 0);
		}
	}
	BitBlt(hdc, pRC->left, pRC->top, pRC->right - pRC->left, pRC->bottom - pRC->top, hdcTxt, 0, 0, SRCCOPY);
}
