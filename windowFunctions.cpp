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



/*!
* \brief Finds an owner of a dialog, used with styles.
*
* Returns the owner HWND
*/
HWND FindOwner(const TString & data, const HWND defaultWnd) {

	if (data.empty())
		return defaultWnd;

	const auto i = data.findtok(TEXT("owner"), 1);

	// 'owner' token not found in data
	if (i == 0)
		return defaultWnd;

	// if there is a token after 'owner'
	if (i < data.numtok( )) {
		const auto tsHwnd(data.gettok((int)(i + 1)));
		// if it is a number (HWND) passed
		auto wnd = (HWND)tsHwnd.to_dword();

		if (wnd != nullptr)
			return wnd;

		// try to retrieve dialog hwnd from name
		wnd = GetHwndFromString(tsHwnd);

		if (wnd != nullptr)
			return wnd;
	}

	return defaultWnd;
}

/*!
* \brief Retrieves a HWND from the string.
*/
HWND GetHwndFromString(const TString &str) {
	if (str.empty())
		return 0;

	//return GetHwndFromString(str.to_chr());

	// test code to allow docking by hwnd (wtf its only 3 lines)
	auto hwnd = reinterpret_cast<HWND>(str.to_<DWORD>());
	if (IsWindow(hwnd))
		return hwnd;

	TString tsRes;
	mIRCLinker::tsEvalex(tsRes, TEXT("$dialog(%s).hwnd"), str.to_chr());

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
void RemStyles(gsl::not_null<HWND> hwnd,int parm,long RemStyles)
{
	auto Styles = (DWORD)GetWindowLong(hwnd, parm);
	Styles &= ~RemStyles;
	SetWindowLong(hwnd, parm, (LONG)Styles);
}

//	Adds window styles to a window
void AddStyles(gsl::not_null<HWND> hwnd,int parm,long AddStyles)
{
	auto Styles = (DWORD)GetWindowLong(hwnd, parm);
	Styles |= AddStyles;
	SetWindowLong(hwnd, parm, (LONG)Styles);
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
//	// If the passed bitmap is NULL, go away!
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
//		}	// if (hData != NULL)
//
//	}// if (hDC)
//
//	DeleteBitmap(SelectObject(hMemDC, hPrevBmp)); // del prev bitmap as it's the DIB Section
//	return hRegion;
//}

HRGN BitmapRegion(HBITMAP hBitmap, COLORREF cTransparentColor, BOOL bIsTransparent)
{
	// We create an empty region
	HRGN		hRegion = nullptr;

	// If the passed bitmap is NULL, go away!
	if (hBitmap == nullptr)
		return nullptr;

	// Computation of the bitmap size
	BITMAP		bmBitmap;

	if (GetObject(hBitmap, sizeof(bmBitmap), &bmBitmap) == 0)
		throw Dcx::dcxException("BitmapRegion() - Unable to get bitmap info");

	// We create a memory context for working with the bitmap
	// The memory context is compatible with the display context (screen)
	//HDC hMemDC = CreateCompatibleDC(nullptr);
	//
	//// If no context is created, go away, too!
	//if (hMemDC == nullptr)
	//	throw Dcx::dcxException("BitmapRegion() - Unable to create DC");
	//
	//Auto(DeleteDC(hMemDC));

	Dcx::dcxHDCResource hMemDC(nullptr);

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

	BITMAPINFO RGB32BITSBITMAPINFO = {
		sizeof(BITMAPINFOHEADER),
		bmBitmap.bmWidth,
		bmBitmap.bmHeight,
		1, 32, BI_RGB, 0, 0, 0, 0, 0, 0
	};

	// Here is the pointer to the bitmap data
	VOID		*pBits;

	// With the previous information, we create the new bitmap!
	//auto		hNewBitmap = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pBits, nullptr, 0);
	//
	//// If the creation process succeded...
	//if (hNewBitmap == nullptr)
	//	throw Dcx::dcxException("BitmapRegion() - CreateDIBSection() Failed: Invalid Parameter");
	//
	//Auto(DeleteBitmap(hNewBitmap));

	Dcx::dcxBitmapResource hNewBitmap(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pBits, nullptr, 0);

	GdiFlush();
	// We select the bitmap onto the created memory context
	// and then we store the previosly selected bitmap on this context!
	auto hPrevBmp = SelectBitmap(hMemDC, hNewBitmap);

	Auto(SelectBitmap(hMemDC, hPrevBmp));

	// We create another device context compatible with the first!
	//HDC hDC = CreateCompatibleDC(hMemDC);
	//
	//// If success...
	//if (hDC == nullptr)
	//	throw Dcx::dcxException("BitmapRegion() - Unable to create DC");
	//
	//Auto(DeleteDC(hDC));

	Dcx::dcxHDCResource hDC((HDC)hMemDC);

	// We compute the number of bytes per row that the bitmap contains, rounding to 32 bit-multiples
	BITMAP		bmNewBitmap;

	if (GetObject(hNewBitmap, sizeof(bmNewBitmap), &bmNewBitmap) == 0)
		throw Dcx::dcxException("BitmapRegion() - Unable to get bitmap info");

	while (bmNewBitmap.bmWidthBytes % 4)
		bmNewBitmap.bmWidthBytes++;

	// Copy of the original bitmap on the memory context!
	auto		hPrevBmpOrg = SelectBitmap(hDC, hBitmap);

	Auto(SelectBitmap(hDC, hPrevBmpOrg));

	BitBlt(hMemDC, 0, 0, bmBitmap.bmWidth, bmBitmap.bmHeight, hDC, 0, 0, SRCCOPY);

	// In order to optimize the code, we don't call the GDI each time we
	// find a transparent pixel. We use a RGN_DATA structure were we store
	// consecutive rectangles, until we have a large amount of them and then we crete
	// the composed region with ExtCreateRgn(), combining it with the main region.
	// Then we begin again initializing the RGN_DATA structure and doing another
	// iteration, until the entire bitmap is analyzed.

	// Also, in order to not saturate the Windows API with calls for reserving
	// memory, we wait until NUMRECT rectangles are stores in order to claim
	// for another NUMRECT memory space!
#define NUMRECT	100			
	DWORD maxRect = NUMRECT;

	// We create the memory data
	auto hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT)*maxRect));

	if (hData == nullptr)
		throw Dcx::dcxException("BitmapRegion() - GlobalAlloc() failed");

	Auto(GlobalFree(hData));

	auto pData = (RGNDATA*)GlobalLock(hData);

	Auto(GlobalUnlock(hData));

	pData->rdh.dwSize = sizeof(RGNDATAHEADER);
	pData->rdh.iType = RDH_RECTANGLES;
	pData->rdh.nCount = pData->rdh.nRgnSize = 0;
	SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

	// We study each pixel on the bitmap...
	auto Pixeles = (BYTE*)bmNewBitmap.bmBits + (bmNewBitmap.bmHeight - 1)*bmNewBitmap.bmWidthBytes;

	// Main loop
	for (auto Row = 0; Row < bmBitmap.bmHeight; Row++)
	{
		// Horizontal loop
		for (auto Column = 0; Column < bmBitmap.bmWidth; Column++)
		{
			// We optimized searching for adjacent transparent pixels!
			int Xo = Column;
			auto Pixel = (RGBQUAD*)Pixeles + Column;

			while (Column < bmBitmap.bmWidth)
			{
				BOOL bInRange = FALSE;

				// If the color is that indicated as transparent...
				if (Pixel->rgbRed == GetRValue(cTransparentColor) &&
					Pixel->rgbGreen == GetGValue((cTransparentColor & 0xFFFF)) &&
					Pixel->rgbBlue == GetBValue(cTransparentColor))
					bInRange = TRUE;

				if ((bIsTransparent) && (bInRange))
					break;

				if ((!bIsTransparent) && (!bInRange))
					break;

				Pixel++;
				Column++;
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
					hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT)*maxRect), GMEM_MOVEABLE);
					pData = (RGNDATA *)GlobalLock(hData);
				}	// if (pData->rdh.nCount>=maxRect)

				auto pRect = (RECT*)&pData->Buffer;
				SetRect(&pRect[pData->rdh.nCount], Xo, Row, Column, Row + 1);

				if (Xo<pData->rdh.rcBound.left)
					pData->rdh.rcBound.left = Xo;

				if (Row<pData->rdh.rcBound.top)
					pData->rdh.rcBound.top = Row;

				if (Column>pData->rdh.rcBound.right)
					pData->rdh.rcBound.right = Column;

				if (Row + 1>pData->rdh.rcBound.bottom)
					pData->rdh.rcBound.bottom = Row + 1;

				pData->rdh.nCount++;

				// In Win95/08 there is a limitation on the maximum number of
				// rectangles a RGN_DATA can store (aprox. 4500), so we call
				// the API for a creation and combination with the main region
				// each 2000 rectangles. This is a good optimization, because
				// instead of calling the routines for combining for each new
				// rectangle found, we call them every 2000 rectangles!!!
				if (pData->rdh.nCount == 2000)
				{
					auto hNewRegion = ExtCreateRegion(nullptr, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRect), pData);
					if (hNewRegion != nullptr) {
						// Si ya existe la región principal,sumamos la nueva,
						// si no,entonces de momento la principal coincide con
						// la nueva región.
						if (hRegion) {
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

	if (pData->rdh.nCount > 0) {
		// Una vez finalizado el proceso,procedemos a la fusión de la
		// región remanente desde la última fusión hasta el final			
		auto hNewRegion = ExtCreateRegion(nullptr, sizeof(RGNDATAHEADER) + (sizeof(RECT)*maxRect), pData);

		if (hNewRegion != nullptr)
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
* Returns true if successful.
*/
void ChangeHwndIcon(const HWND hwnd, const TString &flags, const int index, TString &filename)
{
	const XSwitchFlags xflags(flags);

	filename.trim();
	if (!xflags[TEXT('+')])
		throw Dcx::dcxException("ChangeHwndIcon() - Invalid Flags");

	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("ChangeHwndIcon() - Unable to Access File: %"), filename);

	HICON iconSmall = nullptr;
	HICON iconLarge = nullptr;
	// check for +s small icon flag
	if (xflags[TEXT('s')])
		iconSmall = dcxLoadIcon(index, filename, false, flags);
	// check for +b big icon flag
	if (xflags[TEXT('b')])
		iconLarge = dcxLoadIcon(index, filename, true, flags);

	if ((iconLarge == nullptr) && (iconSmall == nullptr))
	{
		// No big or small flags, so do both icons.
		iconSmall = dcxLoadIcon(index, filename, false, flags);
		iconLarge = dcxLoadIcon(index, filename, true, flags);
	}

	// set the new icons, get back the current icon
	if (iconSmall != nullptr)
		iconSmall = (HICON) SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM) iconSmall);
	if (iconLarge != nullptr)
		iconLarge = (HICON) SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM) iconLarge);

	// delete the old icons
	if (iconSmall != nullptr)
		DestroyIcon(iconSmall);
	if ((iconLarge != nullptr) && (iconSmall != iconLarge)) // dont delete twice
		DestroyIcon(iconLarge);
}
