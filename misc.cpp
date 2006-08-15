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
* © ScriptsDB.org - 2006
*/

#include "defines.h"

extern mIRCDLL mIRCLink; //!< blah

extern PFNSETTHEME SetWindowThemeUx;  //!< blah
extern PFNISTHEMEACTIVE IsThemeActiveUx;
extern BOOL XPPlus;                   //!< Is OS WinXP+ ?


/*!
* \brief Rounding function
*/
int round(float x) { 
	if (x - (float) (int) x > 0.5)
		return (int) x +1;
	else
		return (int) x;
}

/*!
* \brief blah
*
* blah
*/
void mIRCSignal(const char *data) {
	/*
	logFile = fopen( "dcxlog.txt", "a" );

	if ( logFile ) {
	fwrite( data, sizeof( char ), strlen( data ), logFile );
	fclose( logFile );
	}
	*/

	wsprintf(mIRCLink.m_pData, "//.signal -n DCX %s", data);
	SendMessage(mIRCLink.m_mIRCHWND, WM_USER + 200, 0, mIRCLink.m_map_cnt);
}

/*!
 * \brief blah
 *
 * blah
 */
void mIRCDebug(const char *szFormat, ...) {
	va_list args;
	va_start(args, szFormat);

	char msg[2048];
	vsprintf(msg, szFormat, args);
	mIRCError(msg);
}

/*!
* \brief blah
*
* blah
*/
void mIRCError(const char *data) {
	/*
	logFile = fopen( "dcxlog.txt", "a" );

	if ( logFile ) {
	fwrite( data, sizeof( char ), strlen( data ), logFile );
	fclose( logFile );
	}
	*/

	wsprintf(mIRCLink.m_pData, "//echo -s %s", data);
	SendMessage(mIRCLink.m_mIRCHWND, WM_USER + 200, 0, mIRCLink.m_map_cnt);
}

/*!
* \brief mIRC $identifier evaluation function
*
* blah
*/
void mIRCeval(const char *data, char *res) {
	lstrcpy(mIRCLink.m_pData, data);
	SendMessage(mIRCLink.m_mIRCHWND, WM_USER + 201, 0, mIRCLink.m_map_cnt);
	lstrcpy(res, mIRCLink.m_pData);
}

/*!
* \brief mIRC /command function
*
* blah
*/
void mIRCcom(const char *data) {
	lstrcpy(mIRCLink.m_pData, data);
	SendMessage(mIRCLink.m_mIRCHWND, WM_USER + 200, 0, mIRCLink.m_map_cnt);
}

/*!
* \brief Read File Contents
*
* blah
*/
char* readFile(const char *filename) {
	//Ouvrir le fichier, read en mode binaire
	FILE *file;

	// Open file in binary mode and read
	if ((file = fopen(filename, "rb")) == NULL)
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
	char *fileContents = new char[size + 1];

	// Null terminate the string
	fileContents[size] = 0;

	// read the file, fails, destroy memory and return NULL
	if (fread(fileContents, 1, size, file) != size) {
		delete fileContents;
		return NULL;
	}

	// close file
	fclose(file);
	file = NULL;

	// return memory block containing file data
	return fileContents;
}

/*!
* \brief Windows Theme Setting Function
*
* Used to remove theme on controls
*/
HRESULT SetWindowTheme(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList) {
	if (XPPlus)
		return SetWindowThemeUx(hwnd, L" ", L" ");
	else
		return 0;
}

/*!
* \brief Check fi theme is active
*
* Used to remove theme on controls
*/
BOOL IsThemeActive() {
	if (!IsThemeActiveUx)
		return FALSE;
	else if (XPPlus)
		return IsThemeActiveUx();
	else
		return FALSE;
}

/*!
* \brief Windows XP function
*
* Returns whether the OS version is XP+ or not
*
* \return > \b true if OS is XP+ \n
*         > \b false otherwise
*/
BOOL isXP() {
	return XPPlus;
}


/*!
* \brief Shows CommonDialog for Open/Save
*
* Shows and returns the file selected
*
* \return > TString "" if cancelled
*         > TString Path+Filename
*/
TString FileDialog(TString data, TString method, HWND pWnd) {
	DWORD style = OFN_EXPLORER;
	OPENFILENAME ofn;
	char szFilename[900];

	// seperate the tokenz
	TString styles(data.gettok(1, "	"));
	TString file(data.gettok(2, "	"));
	TString filter(data.gettok(3, "	"));

	styles.trim();
	file.trim();
	filter.trim();

	// format the filter into the format WinAPI wants, with double NULL TERMINATOR at end
	if (filter == "")
		filter = "All Files (*.*)|*.*";

	filter += '\0';
	filter.replace('|', '\0');

	// set up the OFN struct
	ZeroMemory(&ofn, sizeof(ofn));
	wsprintf(szFilename, "%s", file.to_chr());

	ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
	ofn.hwndOwner = pWnd;
	ofn.lpstrFilter = filter.to_chr();
	ofn.lpstrFile = szFilename;
	ofn.nMaxFile = 900;
	ofn.lpstrDefExt = "";

	for (int i = 1; i <= styles.numtok(" "); i++) {
		if (styles.gettok(i, " ") == "multisel")
			style |= OFN_ALLOWMULTISELECT;
		else if (styles.gettok(i, " ") == "createprompt")
			style |= OFN_CREATEPROMPT;
		// FIXME: explorer style resizable on default, cant get rid of that shit
		else if (styles.gettok(i, " ") == "enablesizing")
			style |= OFN_ENABLESIZING;
		else if (styles.gettok(i, " ") == "filemustexist")
			style |= OFN_FILEMUSTEXIST; // (open)
		else if (styles.gettok(i, " ") == "showhidden")
			style |= OFN_FORCESHOWHIDDEN; // 2k/xp
		else if (styles.gettok(i, " ") == "noreadonly")
			style |= OFN_HIDEREADONLY;
		else if (styles.gettok(i, " ") == "nochangedir")
			style |= OFN_NOCHANGEDIR; // (save)
		else if (styles.gettok(i, " ") == "getshortcuts")
			style |= OFN_NODEREFERENCELINKS;
		else if (styles.gettok(i, " ") == "nonetwork")
			style |= OFN_NONETWORKBUTTON;
		else if (styles.gettok(i, " ") == "novalidate")
			style |= OFN_NOVALIDATE;
		else if (styles.gettok(i, " ") == "norecent")
			style |= OFN_DONTADDTORECENT; // 2k/xp
		else if (styles.gettok(i, " ") == "overwriteprompt")
			style |= OFN_OVERWRITEPROMPT; // save
		else if (styles.gettok(i, " ") == "pathmustexist")
			style |= OFN_PATHMUSTEXIST;
		else if (styles.gettok(i, " ") == "owner")
			ofn.hwndOwner = FindOwner(styles, pWnd);
	}

	ofn.Flags = style;

	if (method == "OPEN" && GetOpenFileName(&ofn)) {
		TString str("");

		// if there are multiple files selected
		if (style & OFN_ALLOWMULTISELECT) {
			char *p = szFilename; 

			// process the file name at p since its null terminated
			while (*p != '\0') { 
				if (str != "")
					str += "|";

				str += p;
				p += strlen(p)+1;
			} 
		}
		// copy the string directly
		else
			str = szFilename;

		return str;
	}
	else if (method == "SAVE" && GetSaveFileName(&ofn)) {
		return TString(szFilename);
	}

	return TString("");
}

/*!
* \brief Finds an owner of a dialog, used with styles
*
* Returns the owner HWND
*
*/
HWND FindOwner(TString data, HWND defaultWnd) {
	int i = data.findtok("owner", 1, " ");

	// 'owner' token not found in data
	if (!i)
		return defaultWnd;

	// if there is a token after 'owner'
	if (i < data.numtok(" ")) {
		// if it is a number (HWND) passed
		HWND wnd = (HWND) atoi(data.gettok(i +1, " ").to_chr());

		if (wnd)
			return wnd;

		// try to retrieve dialog hwnd from name
		char com[100];
		char res[10];

		wsprintf(com, "$dialog(%s).hwnd", data.gettok(i +1, " ").to_chr());
		mIRCeval(com, res);
		wnd = (HWND) atoi(res);

		if (wnd)
			return wnd;
	}

	return defaultWnd;
}




/*!
* \brief Copies string to the clipboard
*
* Returns TRUE if successful
*
* http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/dataexchange/clipboard/usingtheclipboard.asp
*/
BOOL CopyToClipboard(HWND owner, TString str) {
	if (!OpenClipboard(owner)) {
		mIRCError("D_ERROR CopyToClipboard: couldnt open clipboard");
		return FALSE;
	}

	int cbsize = (strlen(str.to_chr()) +1) * sizeof(TCHAR);
	EmptyClipboard();
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, cbsize);

	if (hglbCopy == NULL) {
		CloseClipboard();
		mIRCError("D_ERROR CopyToClipboard: couldnt open global memory");
		return FALSE;
	}

	char *strCopy = (char *) GlobalLock(hglbCopy);

	// original code, limited and doesnt copy large chunks
	//wsprintf(strCopy, "%s", str.to_chr());

	// demo code from msdn, copies everything
	memcpy(strCopy, str.to_chr(), cbsize);
	strCopy[cbsize] = (TCHAR) 0; // null character

	GlobalUnlock(hglbCopy);
	SetClipboardData(CF_TEXT, hglbCopy);
	CloseClipboard();

	return TRUE;
}


// Turns a command (+flags CHARSET SIZE FONTNAME) into a LOGFONT struct
BOOL ParseCommandToLogfont(TString cmd, LPLOGFONT lf) {
	if (cmd.numtok(" ") < 4)
		return FALSE;

	ZeroMemory(lf, sizeof(LOGFONT));
	UINT flags = parseFontFlags(cmd.gettok(1, " "));

	if (flags & DCF_DEFAULT) {
		HFONT hf = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
		GetObject(hf, sizeof(LOGFONT), lf);
		return TRUE;
	}
	else {
		int fSize = atoi(cmd.gettok(3, " ").to_chr());
		TString fName = cmd.gettok(4, -1, " ");
		fName.trim();

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

		lf->lfCharSet = parseFontCharSet(cmd.gettok(2, " "));
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
UINT parseFontFlags(TString &flags) {
	INT i = 1, len = flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+')
		return iFlags;

	while (i < len) {
		if (flags[i] == 'a')
			iFlags |= DCF_ANTIALIASE;
		else if (flags[i] == 'b')
			iFlags |= DCF_BOLD;
		else if (flags[i] == 'd')
			iFlags |= DCF_DEFAULT;
		else if (flags[i] == 'i')
			iFlags |= DCF_ITALIC;
		else if (flags[i] == 's')
			iFlags |= DCF_STRIKEOUT;
		else if (flags[i] == 'u')
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
UINT parseFontCharSet(TString &charset) {
	if (charset == "ansi")
		return ANSI_CHARSET;
	else if (charset == "baltic")
		return BALTIC_CHARSET;
	else if (charset == "chinesebig")
		return CHINESEBIG5_CHARSET;
	else if (charset == "default")
		return DEFAULT_CHARSET;
	else if (charset == "easteurope")
		return EASTEUROPE_CHARSET;
	else if (charset == "gb2312")
		return GB2312_CHARSET;
	else if (charset == "greek")
		return GREEK_CHARSET;
	else if (charset == "hangul")
		return HANGUL_CHARSET;
	else if (charset == "mac")
		return MAC_CHARSET;
	else if (charset == "oem")
		return OEM_CHARSET;
	else if (charset == "russian")
		return RUSSIAN_CHARSET;
	else if (charset == "shiftjis")
		return SHIFTJIS_CHARSET;
	else if (charset == "symbol")
		return SYMBOL_CHARSET;
	else if (charset == "turkish")
		return TURKISH_CHARSET;
	else if (charset == "vietnamese")
		return VIETNAMESE_CHARSET;

	return DEFAULT_CHARSET;
}


TString ParseLogfontToCommand(LPLOGFONT lf) {
	TString flags("+");
	TString charset("default");

	// get charset
	switch (lf->lfCharSet) {
		case ANSI_CHARSET			: charset = "ansi"; break;
		case BALTIC_CHARSET		: charset = "baltic"; break;
		case CHINESEBIG5_CHARSET: charset = "chinesebig"; break;
		case EASTEUROPE_CHARSET	: charset = "easteurope"; break;
		case GB2312_CHARSET		: charset = "gb2312"; break;
		case GREEK_CHARSET		: charset = "greek"; break;
		case HANGUL_CHARSET		: charset = "hangul"; break;
		case MAC_CHARSET			: charset = "mac"; break;
		case OEM_CHARSET			: charset = "oem"; break;
		case RUSSIAN_CHARSET		: charset = "russian"; break;
		case SHIFTJIS_CHARSET	: charset = "shiftjis"; break;
		case SYMBOL_CHARSET		: charset = "symbol"; break;
		case TURKISH_CHARSET		: charset = "turkish"; break;
		case VIETNAMESE_CHARSET	: charset = "vietnamese"; break;
		case DEFAULT_CHARSET		:
		default						: charset = "default"; break;
	}

	// get flags
	if (lf->lfQuality == ANTIALIASED_QUALITY)
		flags += "a";
	if (lf->lfWeight == FW_BOLD)
		flags += "b";
	if (lf->lfItalic)
		flags += "i";
	if (lf->lfStrikeOut)
		flags += "s";
	if (lf->lfUnderline)
		flags += "u";

	//lf.lfHeight = -MulDiv( fSize, GetDeviceCaps(hdc, LOGPIXELSY ), 72 );
	HDC hdc = GetDC(NULL);
	HFONT hf = CreateFontIndirect(lf);
	TEXTMETRIC tm;

	SelectObject(hdc, hf);
	GetTextMetrics(hdc, &tm);

	//int ptSize = (int) (-1 * (lfCurrent.lfHeight * 72 / GetDeviceCaps(hdc, LOGPIXELSY)));
	int ptSize = MulDiv(tm.tmHeight - tm.tmInternalLeading, 72, GetDeviceCaps(hdc, LOGPIXELSY));
	DeleteFont(hf);
	ReleaseDC(NULL, hdc);

	// [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
	TString tmp;
	
	tmp.sprintf("%s %s %d %s", flags.to_chr(), charset.to_chr(), ptSize, lf->lfFaceName);
	return tmp;
}


HBITMAP LoadBitmap(HBITMAP dest, TString &filename) {
	filename.trim();

	if (dest) {
		DeleteObject(dest);
		dest = NULL;
	}

	dest = (HBITMAP) LoadImage(GetModuleHandle(NULL), filename.to_chr(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);

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
HICON CreateGrayscaleIcon(HICON hIcon) {
	HICON		hGrayIcon = NULL;
	HDC		hMainDC = NULL, hMemDC1 = NULL, hMemDC2 = NULL;
	BITMAP	bmp;
	HBITMAP	hOldBmp1 = NULL, hOldBmp2 = NULL;
	ICONINFO	csII, csGrayII;
	//BOOL		bRetValue = FALSE;

	if (!GetIconInfo(hIcon, &csII))
		return NULL;

	ZeroMemory(&csGrayII, sizeof(ICONINFO));

	hMainDC = GetDC(NULL);
	hMemDC1 = CreateCompatibleDC(hMainDC);
	hMemDC2 = CreateCompatibleDC(hMainDC);

	if (hMainDC == NULL || hMemDC1 == NULL || hMemDC2 == NULL)
		return NULL;

	if (GetObject(csII.hbmColor, sizeof(BITMAP), &bmp)) {
		DWORD dwWidth = csII.xHotspot*2;
		DWORD dwHeight = csII.yHotspot*2;

		csGrayII.hbmColor = CreateBitmap(dwWidth, dwHeight, bmp.bmPlanes, bmp.bmBitsPixel, NULL);

		if (csGrayII.hbmColor) {
			hOldBmp1 = (HBITMAP) SelectObject(hMemDC1, csII.hbmColor);
			hOldBmp2 = (HBITMAP) SelectObject(hMemDC2, csGrayII.hbmColor);

			//::BitBlt(hMemDC2, 0, 0, dwWidth, dwHeight, hMemDC1, 0, 0, SRCCOPY);

			DWORD		dwLoopY = 0, dwLoopX = 0;
			COLORREF	crPixel = 0;
			BYTE	byNewPixel = 0;

			for (dwLoopY = 0; dwLoopY < dwHeight; dwLoopY++) {
				for (dwLoopX = 0; dwLoopX < dwWidth; dwLoopX++) {
					crPixel = GetPixel(hMemDC1, dwLoopX, dwLoopY);

					byNewPixel = (BYTE)((GetRValue(crPixel) * 0.299) + 
						(GetGValue(crPixel) * 0.587) + 
						(GetBValue(crPixel) * 0.114));

					if (crPixel)
						SetPixel(hMemDC2, dwLoopX, dwLoopY, RGB(byNewPixel, byNewPixel, byNewPixel));
				} // for
			} // for

			SelectObject(hMemDC1, hOldBmp1);
			SelectObject(hMemDC2, hOldBmp2);

			csGrayII.hbmMask = csII.hbmMask;
			csGrayII.fIcon = TRUE;



			hGrayIcon = ::CreateIconIndirect(&csGrayII);
		} // if

		DeleteObject(csGrayII.hbmColor);
		//::DeleteObject(csGrayII.hbmMask);
	} // if

	DeleteObject(csII.hbmColor);
	DeleteObject(csII.hbmMask);
	DeleteDC(hMemDC1);
	DeleteDC(hMemDC2);
	ReleaseDC(NULL, hMainDC);

	return hGrayIcon;
} // End of CreateGrayscaleIcon

LRESULT ctrl_MeasureItem(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	char ClassName[256];
	HWND cHwnd = GetDlgItem(mHwnd, wParam);

	if (IsWindow(cHwnd) && GetClassName(cHwnd, ClassName, 256) != 0) {
		if (lstrcmp(DCX_COLORCOMBOCLASS, ClassName) == 0) {
			LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;

			if (lpmis != NULL)
				lpmis->itemHeight = 16; 

			return TRUE;
		}
	}

	return 0L;
}
//void DrawRoundRect(HDC hdc, RECT *rc, int w, int h)
//{
//	// draw top line, left to right
//	MoveToEx(hdc,(rc->left + w),rc->top,NULL);
//	//LineTo(hdc,(rc->right - w),rc->top);
//	// draw ellipse at top right.
//	SetArcDirection(hdc,AD_CLOCKWISE);
//	ArcTo(hdc,(rc->right - w),rc->top,rc->right,(rc->top + h),(rc->right - w),rc->top,rc->right,(rc->top + h));
//	// draw line from top right to bottom right
//	ArcTo(hdc,(rc->right - w),(rc->bottom - h),rc->right,rc->bottom,(rc->right - w),(rc->bottom - h),rc->right,rc->bottom);
//}
void AddToolTipToolInfo(HWND tiphwnd, HWND ctrl)
{
		TOOLINFO ti;
		ZeroMemory(&ti,sizeof(TOOLINFO));
		ti.cbSize = sizeof(TOOLINFO);
		ti.hwnd = ctrl;
		ti.lpszText = LPSTR_TEXTCALLBACK;
		ti.uFlags = TTF_IDISHWND | TTF_TRANSPARENT | TTF_SUBCLASS;
		ti.uId = (UINT_PTR)ctrl;

		SendMessage(tiphwnd,TTM_ADDTOOL,NULL,(LPARAM)&ti);
}