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
* � ScriptsDB.org - 2007-2008
*/
#include "defines.h"
#include "Dcx.h"



/*!
* \brief Finds an owner of a dialog, used with styles.
*
* Returns the owner HWND
*/
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
		if (const auto wnd = reinterpret_cast<HWND>(tsHwnd.to_<DWORD>()); wnd)
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
		if (const auto wnd = reinterpret_cast<HWND>(tsHwnd.to_<DWORD>()); wnd)
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
	if (const auto hwnd = reinterpret_cast<HWND>(str.to_<DWORD>()); IsWindow(hwnd))
		return hwnd;

	TString tsRes;
	//mIRCLinker::tsEvalex(tsRes, TEXT("$dialog(%s).hwnd"), str.to_chr());
	mIRCLinker::eval(tsRes, TEXT("$dialog(%).hwnd"), str);

	return reinterpret_cast<HWND>(tsRes.to_<DWORD>());
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
//								// Si ya existe la regi�n principal,sumamos la nueva,
//								// si no,entonces de momento la principal coincide con
//								// la nueva regi�n.
//								if (hRegion) {
//									CombineRgn(hRegion, hRegion, hNewRegion, RGN_OR);
//									DeleteObject(hNewRegion);
//								}
//								else
//									hRegion = hNewRegion;
//
//
//							}	// if (hNewRegion != nullptr)
//							// Volvemos a comenzar la suma de rect�ngulos
//							pData->rdh.nCount = 0;
//							SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
//						}	// if(pData->rdh.nCount==2000)
//
//					} // if (Column > Xo)
//				} // for (int  Column ...)
//
//				// Nueva Row. Lo del negativo se debe a que el bitmap est� invertido
//				// verticalmente.
//				Pixeles -= bmNewBitmap.bmWidthBytes;
//
//			} // for (int Row...)			
//
//			if (pData->rdh.nCount > 0) {
//				// Una vez finalizado el proceso,procedemos a la fusi�n de la
//				// regi�n remanente desde la �ltima fusi�n hasta el final			
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
						// Si ya existe la regi�n principal,sumamos la nueva,
						// si no,entonces de momento la principal coincide con
						// la nueva regi�n.
						if (hRegion)
						{
							CombineRgn(hRegion, hRegion, hNewRegion, RGN_OR);
							DeleteObject(hNewRegion);
						}
						else
							hRegion = hNewRegion;


					}	// if (hNewRegion != nullptr)
					// Volvemos a comenzar la suma de rect�ngulos
					pData->rdh.nCount = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
				}	// if(pData->rdh.nCount==2000)

			} // if (Column > Xo)
		} // for (int  Column ...)

		// Nueva Row. Lo del negativo se debe a que el bitmap est� invertido
		// verticalmente.
		Pixeles -= bmNewBitmap.bmWidthBytes;

	} // for (int Row...)			

	if (pData->rdh.nCount > 0)
	{
		// Una vez finalizado el proceso,procedemos a la fusi�n de la
		// regi�n remanente desde la �ltima fusi�n hasta el final			
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
	const XSwitchFlags xflags(flags);

	filename.trim();
	if (!xflags[TEXT('+')])
		throw Dcx::dcxException("ChangeHwndIcon() - Invalid Flags");

	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("ChangeHwndIcon() - Unable to Access File: %"), filename);

	HICON iconSmall{ nullptr };
	HICON iconLarge{ nullptr };
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

COLORREF getCheckBoxBkgColour(const clrCheckBox* lpcol, DWORD dState) noexcept
{
	if (dcx_testflag(dState, CDIS_DISABLED))
		return lpcol->m_clrDisabledBackground;

	if (dcx_testflag(dState, CDIS_HOT))
		return lpcol->m_clrHotBackground;

	return lpcol->m_clrBackground;
}

COLORREF getCheckBoxFrameColour(const clrCheckBox* lpcol, DWORD dState) noexcept
{
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
	if (dcx_testflag(dState, CDIS_DISABLED))
		return lpcol->m_clrDisabledTick;

	if (dcx_testflag(dState, CDIS_HOT))
		return lpcol->m_clrHotTick;

	return lpcol->m_clrTick;
}

/// <summary>
/// draws a checkbox.
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

	{
		// draw tick box
		const auto hOldPenBorder = SelectObject(hDC, hPenBorder);
		Auto(SelectObject(hDC, hOldPenBorder));

		InflateRect(&rc, 0, -1);
		rc.left += 1;
		rc.right = rc.left + rc.bottom - rc.top;

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

		dcxDrawLine(hDC, x, y + 2, x, y + 5);
		dcxDrawLine(hDC, x + 1, y + 3, x + 1, y + 6);
		dcxDrawLine(hDC, x + 2, y + 4, x + 2, y + 7);
		dcxDrawLine(hDC, x + 3, y + 3, x + 3, y + 6);
		dcxDrawLine(hDC, x + 4, y + 2, x + 4, y + 5);
		dcxDrawLine(hDC, x + 5, y + 1, x + 5, y + 4);
		dcxDrawLine(hDC, x + 6, y, x + 6, y + 3);
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
		RoundRect(hDC, rc->left, rc->top, rc->right, rc->bottom, 10, 10);
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
	BLENDFUNCTION ai_bf{ AC_SRC_OVER, 0, 0xC0, 0 };
	BP_PAINTPARAMS paintParams{ sizeof(BP_PAINTPARAMS),BPPF_ERASE, nullptr, &ai_bf };
	HDC hdc{};

	auto ai_Buffer = Dcx::UXModule.dcxBeginBufferedPaint(hDC, rc, BPBF_COMPATIBLEBITMAP, &paintParams, &hdc);
	if (!ai_Buffer)
		return false;

	Auto(Dcx::UXModule.dcxEndBufferedPaint(ai_Buffer, TRUE));

	//Dcx::UXModule.dcxBufferedPaintClear(ai_Buffer, rc);

	//dcxDrawRect(hdc, rc, GetBkColor(hDC), GetBkColor(hDC), false);
	//const auto oldMode = SetBkMode(hdc, TRANSPARENT);
	//Auto(SetBkMode(hdc, oldMode));

	return dcxDrawRect(hdc, rc, clr, clrBorder, bRounded);
}
