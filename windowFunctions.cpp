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
	const int i = data.findtok(TEXT("owner"), 1);

	// TEXT('owner') token not found in data
	if (!i)
		return defaultWnd;

	// if there is a token after TEXT('owner')
	if (i < data.numtok( )) {
		// if it is a number (HWND) passed
		HWND wnd = (HWND) data.gettok( i +1 ).to_num();

		if (wnd)
			return wnd;

		// try to retrieve dialog hwnd from name
		wnd = GetHwndFromString(data.gettok( i +1 ));

		if (wnd)
			return wnd;
	}

	return defaultWnd;
}

/*!
* \brief Retrieves a HWND from the string.
*/
HWND GetHwndFromString(const TString &str) {
	return GetHwndFromString(str.to_chr());
}

/*!
* \brief Retireves a HWND from the string.
*/
HWND GetHwndFromString(const TCHAR *str) {

	// test code to allow docking by hwnd (wtf its only 3 lines)
	HWND hwnd = (HWND)dcx_atoi64(str);
	if (IsWindow(hwnd))
		return hwnd;

	TCHAR res[20];
	res[0] = 0;
	Dcx::mIRC.evalex( res, 20, TEXT("$dialog(%s).hwnd"), str);

	return (HWND) dcx_atoi(res);
}

// Removes window style to a window
void RemStyles(HWND hwnd,int parm,long RemStyles)
{
  DWORD Styles = (DWORD)GetWindowLong(hwnd, parm);
  Styles &= ~RemStyles;
  SetWindowLong(hwnd, parm, Styles);
}

//	Adds window styles to a window
void AddStyles(HWND hwnd,int parm,long AddStyles)
{
  DWORD Styles = (DWORD)GetWindowLong(hwnd, parm);
  Styles |= AddStyles;
  SetWindowLong(hwnd, parm, Styles);
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

HRGN BitmapRegion(HBITMAP hBitmap,COLORREF cTransparentColor,BOOL bIsTransparent)
{
	// We create an empty region
	HRGN		hRegion=NULL;
	
	// If the passed bitmap is NULL, go away!
	if(hBitmap == NULL)
		return NULL;
	
	// We create a memory context for working with the bitmap
	// The memory context is compatible with the display context (screen)
	HDC hMemDC = CreateCompatibleDC(NULL);
	
	// If no context is created, go away, too!
	if(hMemDC == NULL)
		return NULL;

	// Computation of the bitmap size
	BITMAP		bmBitmap;
	
	GetObject(hBitmap, sizeof(bmBitmap), &bmBitmap);

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
		1,32,BI_RGB,0,0,0,0,0,0
	};

	// Here is the pointer to the bitmap data
	VOID		*pBits;
	
	// With the previous information, we create the new bitmap!
	HBITMAP		hNewBitmap;
	hNewBitmap = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO,	DIB_RGB_COLORS, &pBits, NULL, 0);

	// If the creation process succeded...
	if(hNewBitmap != NULL)
	{
		GdiFlush();
		// We select the bitmap onto the created memory context
		// and then we store the previosly selected bitmap on this context!
		HBITMAP hPrevBmp=(HBITMAP) SelectObject(hMemDC,hNewBitmap);
		
		// We create another device context compatible with the first!
		HDC hDC=CreateCompatibleDC(hMemDC);

		// If success...
		if(hDC != NULL)
		{
			// We compute the number of bytes per row that the bitmap contains, rounding to 32 bit-multiples
			BITMAP		bmNewBitmap;
			
			GetObject(hNewBitmap,sizeof(bmNewBitmap),&bmNewBitmap);
				
			while(bmNewBitmap.bmWidthBytes % 4) 
				bmNewBitmap.bmWidthBytes++;
			
			// Copy of the original bitmap on the memory context!
			HBITMAP		hPrevBmpOrg=(HBITMAP) SelectObject(hDC,hBitmap);
			BitBlt(hMemDC,0,0,bmBitmap.bmWidth,bmBitmap.bmHeight,hDC,0,0,SRCCOPY);

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
			HANDLE hData=GlobalAlloc(GMEM_MOVEABLE,sizeof(RGNDATAHEADER)+(sizeof(RECT)*maxRect));
			RGNDATA *pData=(RGNDATA*) GlobalLock(hData);
			pData->rdh.dwSize=sizeof(RGNDATAHEADER);
			pData->rdh.iType=RDH_RECTANGLES;
			pData->rdh.nCount=pData->rdh.nRgnSize=0;
			SetRect(&pData->rdh.rcBound,MAXLONG,MAXLONG,0,0);

			// We study each pixel on the bitmap...
			BYTE *Pixeles=(BYTE*) bmNewBitmap.bmBits+(bmNewBitmap.bmHeight-1)*bmNewBitmap.bmWidthBytes;
			
			// Main loop
			for(int Row=0;Row<bmBitmap.bmHeight;Row++) 
			{
				// Horizontal loop
				for(int Column=0;Column<bmBitmap.bmWidth;Column++)
				{		
					// We optimized searching for adjacent transparent pixels!
					int Xo=Column;
					RGBQUAD *Pixel=(RGBQUAD*) Pixeles+Column;

					while(Column<bmBitmap.bmWidth) 
					{
						BOOL bInRange=FALSE;

						// If the color is that indicated as transparent...
						if(	Pixel->rgbRed==GetRValue(cTransparentColor) &&
							Pixel->rgbGreen==GetGValue((cTransparentColor & 0xFFFF)) &&
							Pixel->rgbBlue==GetBValue(cTransparentColor) )
							bInRange=TRUE;

						if((bIsTransparent) && (bInRange))
							break;

						if((!bIsTransparent) && (!bInRange))
							break;

						Pixel++;
						Column++;
					} // while (Column < bm.bmWidth)

					if(Column>Xo) 
					{
						// We add the rectangle (Xo,Row),(Column,Row+1) to the region

						// If the number of rectangles is greater then NUMRECT, we claim
						// another pack of NUMRECT memory places!
						if (pData->rdh.nCount>=maxRect)
						{
							GlobalUnlock(hData);
							maxRect+=NUMRECT;
							hData=GlobalReAlloc(hData,sizeof(RGNDATAHEADER)+(sizeof(RECT)*maxRect),GMEM_MOVEABLE);
							pData=(RGNDATA *)GlobalLock(hData);
						}

						RECT *pRect=(RECT*) &pData->Buffer;
						SetRect(&pRect[pData->rdh.nCount],Xo,Row,Column,Row+1);
								
						if(Xo<pData->rdh.rcBound.left)
							pData->rdh.rcBound.left=Xo;

						if(Row<pData->rdh.rcBound.top)
							pData->rdh.rcBound.top=Row;

						if(Column>pData->rdh.rcBound.right)
							pData->rdh.rcBound.right=Column;
						
						if(Row+1>pData->rdh.rcBound.bottom)
							pData->rdh.rcBound.bottom=Row+1;
		
						pData->rdh.nCount++;

						// In Win95/08 there is a limitation on the maximum number of
						// rectangles a RGN_DATA can store (aprox. 4500), so we call
						// the API for a creation and combination with the main region
						// each 2000 rectangles. This is a good optimization, because
						// instead of calling the routines for combining for each new
						// rectangle found, we call them every 2000 rectangles!!!
						if(pData->rdh.nCount==2000)
						{						
							HRGN hNewRegion=ExtCreateRegion(NULL,sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRect),pData);
							if (hNewRegion != NULL) {
								// Si ya existe la región principal,sumamos la nueva,
								// si no,entonces de momento la principal coincide con
								// la nueva región.
								if (hRegion) {
									CombineRgn(hRegion,hRegion,hNewRegion,RGN_OR);									
									DeleteObject(hNewRegion);
								} else
									hRegion=hNewRegion;
								
								
							}
							// Volvemos a comenzar la suma de rectángulos
							pData->rdh.nCount=0;
							SetRect(&pData->rdh.rcBound,MAXLONG,MAXLONG,0,0);
						}			
					
					} // if (Column > Xo)
				} // for (int  Column ...)

				// Nueva Row. Lo del negativo se debe a que el bitmap está invertido
				// verticalmente.
				Pixeles -= bmNewBitmap.bmWidthBytes;

			} // for (int Row...)			

			if (pData->rdh.nCount > 0) {
				// Una vez finalizado el proceso,procedemos a la fusión de la
				// región remanente desde la última fusión hasta el final			
				HRGN hNewRegion=ExtCreateRegion(NULL,sizeof(RGNDATAHEADER)+(sizeof(RECT)*maxRect),pData);

				if(hNewRegion != NULL)
				{
					// If the main region does already exist, we add the new one,
					if(hRegion)
					{
						CombineRgn(hRegion,hRegion,hNewRegion,RGN_OR);
						DeleteObject(hNewRegion);
					}
					else
						// if not, we consider the new one to be the main region at first!
						hRegion=hNewRegion;
				}
			}
			// We free the allocated memory and the rest of used ressources
			GlobalUnlock(hData);
			GlobalFree(hData);
			SelectObject(hDC,hPrevBmpOrg); // don't del prev bitmap, as its our supplied one.
			DeleteDC(hDC);

		}// if (hDC)

		DeleteBitmap(SelectObject(hMemDC,hPrevBmp)); // del prev bitmap as it's the DIB Section
	} //if (hNewBitmap)
	else {
		//DWORD err = GetLastError(); // Only ever returns `Invalid Parameters`
		Dcx::error(TEXT("BitmapRegion()"),TEXT("CreateDIBSection() Failed: Invalid Parameter"));
	}
	DeleteDC(hMemDC);
	return hRegion;
}

/*!
* \brief Changes the icon for a window.
*
* Returns true if successful.
*/
bool ChangeHwndIcon(const HWND hwnd, const TString &flags, const int index, TString &filename)
{
	filename.trim();
	if (const_cast<TString &>(flags)[0] != TEXT('+')) {
		Dcx::error(TEXT("ChangeHwndIcon"), TEXT("Invalid Flags"));
		return false;
	}
	if (!IsFile(filename)) {
		Dcx::errorex(TEXT("ChangeHwndIcon"), TEXT("Unable to Access File: %s"), filename.to_chr());
		return false;
	}

	HICON iconSmall = NULL;
	HICON iconLarge = NULL;
	// check for +s small icon flag
	const bool doSmall = (flags.find(TEXT('s'),0) ? true : false);
	// check for +b big icon flag
	const bool doBig = (flags.find(TEXT('b'),0) ? true : false);

	if (doSmall || doBig) {
		// doing big &/or small icon
		if (doSmall)
			iconSmall = dcxLoadIcon(index, filename, false, flags);
		if (doBig)
			iconLarge = dcxLoadIcon(index, filename, true, flags);
	}
	else {
		// No big or small flags, so do both icons.
		iconSmall = dcxLoadIcon(index, filename, false, flags);
		iconLarge = dcxLoadIcon(index, filename, true, flags);
	}
	if ((iconLarge == NULL) && (iconSmall == NULL)) {
		Dcx::error(TEXT("ChangeHwndIcon"), TEXT("Error Extracting Icon"));
		return false;
	}
	// set the new icons, get back the current icon
	if (iconSmall != NULL)
		iconSmall = (HICON) SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM) iconSmall);
	if (iconLarge != NULL)
		iconLarge = (HICON) SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM) iconLarge);

	// delete the old icons
	if (iconSmall != NULL)
		DestroyIcon(iconSmall);
	if ((iconLarge != NULL) && (iconSmall != iconLarge)) // dont delete twice
		DestroyIcon(iconLarge);

	//UINT cnt = 0;
	//if (flags.find(TEXT('a'),0)) {
	//	WORD wIndex = index;
	//	iconLarge = ExtractAssociatedIcon(GetModuleHandle(NULL), filename.to_chr(), &wIndex);
	//	cnt++;
	//}
	//else {
	//	cnt = ExtractIconEx(filename.to_chr(), index, NULL, &iconSmall, 1);
	//	cnt += ExtractIconEx(filename.to_chr(), index, &iconLarge, NULL, 1);
	//}
	//// NB: shouldnt this be CopyIcon() ?
	//// copy the icon over in case there was no small icon
	//if (!iconLarge)
	//	iconLarge = iconSmall;
	//// copy the icon over in case there was no large icon
	//if (!iconSmall)
	//	iconSmall = iconLarge;

	//// TODO: add more meaningful error messages
	//// No icon in file
	//if (cnt == 0) {
	//	DCXError(TEXT("ChangeHwndIcon"), TEXT("No icon in file"));
	//	return false;
	//}
	//if (!iconLarge && !iconSmall) {
	//	DCXError(TEXT("ChangeHwndIcon"), TEXT("Error Extracting Icon"));
	//	return false;
	//}

	//if (flags.find(TEXT('g'), 0)) {
	//	iconSmall = CreateGrayscaleIcon(iconSmall);
	//	if (iconSmall != iconLarge) // no need to convert twice.
	//		iconLarge = CreateGrayscaleIcon(iconLarge);
	//}

	//// set the new icons, get back the current icon
	//iconSmall = (HICON) SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM) iconSmall);
	//iconLarge = (HICON) SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM) iconLarge);

	//// delete the old icons
	//if (iconSmall)
	//	DestroyIcon(iconSmall);
	//if (iconLarge && (iconSmall != iconLarge)) // dont delete twice
	//	DestroyIcon(iconLarge);
	return true;
}
