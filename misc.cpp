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

// DCX Stuff

/*!
* \brief Rounding function
*/
int dcx_round(const float x) noexcept
{
	const auto t = gsl::narrow_cast<int>(x);
	if ((x - gsl::narrow_cast<float>(t)) > 0.5f)
		return t +1;
	return t;
}

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
//		nLength = WideCharToMultiByte(CP_UTF8, 0, pwStr, -1, NULL, 0, NULL, NULL);
//		// obtain a new buffer for the converted characters
//		ptText = new TCHAR[nLength];
//		// convert to multi-byte characters
//		nLength = WideCharToMultiByte(CP_UTF8, 0, pwStr, -1, ptText, nLength, NULL, NULL);
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
//		nLength = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<PCSTR>(pBuffer), -1, NULL, 0);
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
	PWSTR			pwStr;		// pointer to a wchar_t buffer

	// obtain a wide character pointer to check BOMs
	pwStr = reinterpret_cast<PWSTR>(pBuffer);

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
				WCHAR	wcTemp = (WCHAR)(*pwStr >> 8);
				wcTemp |= *pwStr << 8;
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

/*!
* \brief Read File Contents
*
*/
//std::unique_ptr<BYTE[]> readFile(const PTCHAR filename)

auto readFile(const TString &filename)
-> std::unique_ptr<BYTE[]>
{
	if (filename.empty())
		return nullptr;

#if DCX_USE_WRAPPERS
	const Dcx::dcxFileResource file(filename, TEXT("rb"));

	const auto size = file.Size();

	// make data container for file contents
	auto fileContents = std::make_unique<BYTE[]>(size + 2);

	// Null terminate the string (use double zero)
	fileContents[size] = 0;
	fileContents[size + 1U] = 0;

	// read the file
	fread(fileContents.get(), 1U, size, file.get());

	// return memory block containing file data
	return fileContents;
#else
	auto file = dcx_fopen(filename, TEXT("rb"));
	
	// Open file in binary mode and read
	if (file == nullptr)
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
TString readTextFile(const TString &tFile)
{
	//auto data = readFile(tFile);
	//
	//if (data == nullptr)
	//	return TString();
	//
	//return Normalise(data.get());

	const auto data = readFile(tFile);

	return Normalise(data.get());
}

/*!
* \brief Save a TString object to file as text
*
* TODO: ...
*/
bool SaveDataToFile(const TString &tsFile, const TString &tsData)
{
#if DCX_USE_WRAPPERS
	const Dcx::dcxFileResource file(tsFile, TEXT("wb"));

	constexpr WCHAR tBOM = 0xFEFF;	// unicode BOM

	fwrite(&tBOM, sizeof(TCHAR), 1, file.get());

	// if not in unicode mode then save without BOM as ascii/utf8
	fwrite(tsData.to_chr(), sizeof(TCHAR), tsData.len(), file.get());
	fflush(file.get());
	return true;
#else
	auto file = dcx_fopen(tsFile.to_chr(), TEXT("wb"));
	
	if (file == nullptr)
		return false;
	
	Auto(fclose(file));

	constexpr WCHAR tBOM = 0xFEFF;	// unicode BOM

	fwrite(&tBOM, sizeof(TCHAR), 1, file);

	// if not in unicode mode then save without BOM as ascii/utf8
	fwrite(tsData.to_chr(), sizeof(TCHAR), tsData.len(), file);
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
bool CopyToClipboard(const HWND owner, const TString & str)
{
	if (!OpenClipboard(owner))
	{
		Dcx::error(TEXT("CopyToClipboard"),TEXT("Couldn't open clipboard"));
		return false;
	}

	Auto(CloseClipboard());

	const size_t cbsize = (str.len() +1) * sizeof(TCHAR);
	EmptyClipboard();
	auto hglbCopy = GlobalAlloc(GMEM_MOVEABLE, cbsize);

	if (hglbCopy == nullptr)
	{
		Dcx::error(TEXT("CopyToClipboard"),TEXT("Couldn't open global memory"));
		return false;
	}

	{	// simply sets scope for vars
		const auto strCopy = static_cast<TCHAR *>(GlobalLock(hglbCopy));

		if (strCopy == nullptr)
		{
			GlobalFree(hglbCopy);
			Dcx::error(TEXT("CopyToClipboard"), TEXT("Couldn't lock global memory"));
			return false;
		}

		dcx_strcpyn(strCopy, str.to_chr(), str.len() +1);

		GlobalUnlock(hglbCopy);
	}

	SetClipboardData(CF_UNICODETEXT, hglbCopy);
	return true;
}


// Turns a command (+flags CHARSET SIZE FONTNAME) into a LOGFONT struct
bool ParseCommandToLogfont(const TString& cmd, LPLOGFONT lf)
{
	if (cmd.numtok( ) < 4)
		return false;

	ZeroMemory(lf, sizeof(LOGFONT));
	const auto flags = parseFontFlags(cmd.getfirsttok( 1 ));

	if (dcx_testflag(flags, dcxFontFlags::DCF_DEFAULT))
		return (GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), lf) != 0);

	lf->lfCharSet = gsl::narrow_cast<BYTE>(parseFontCharSet(cmd.getnexttok( )) & 0xFF);	// tok 2
	const auto fSize = cmd.getnexttok( ).to_int();				// tok 3
	const auto fName(cmd.getlasttoks( ).trim());				// tok 4, -1

	if (!fSize)
		return false;

	{
		const auto hdc = GetDC(nullptr);

		if (hdc == nullptr)
			return false;

		Auto(ReleaseDC(nullptr, hdc));

		lf->lfHeight = -MulDiv(fSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	}

	if (dcx_testflag(flags, dcxFontFlags::DCF_ANTIALIASE))
		lf->lfQuality = ANTIALIASED_QUALITY;

	if (dcx_testflag(flags, dcxFontFlags::DCF_BOLD))
		lf->lfWeight = FW_BOLD;
	else
		lf->lfWeight = FW_NORMAL;

	if (dcx_testflag(flags, dcxFontFlags::DCF_ITALIC))
		lf->lfItalic = TRUE;

	if (dcx_testflag(flags, dcxFontFlags::DCF_STRIKEOUT))
		lf->lfStrikeOut = TRUE;

	if (dcx_testflag(flags, dcxFontFlags::DCF_UNDERLINE))
		lf->lfUnderline = TRUE;

	dcx_strcpyn(&lf->lfFaceName[0], fName.to_chr(), std::extent_v<decltype(lf->lfFaceName)>);
	lf->lfFaceName[std::extent_v<decltype(lf->lfFaceName)> -1] = 0;

	return true;
}


/*!
 * \brief blah
 *
 * blah
 */
UINT parseFontFlags(const TString &flags) noexcept
{
	UINT iFlags = 0U;
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
	if (xflags[TEXT('s')])
		iFlags |= dcxFontFlags::DCF_STRIKEOUT;
	if (xflags[TEXT('u')])
		iFlags |= dcxFontFlags::DCF_UNDERLINE;

	return iFlags;
}


const static std::map<std::hash<TString>::result_type, UINT> charset_map{
	{TEXT("ansi"_hash), ANSI_CHARSET},
	{TEXT("baltic"_hash), BALTIC_CHARSET},
	{TEXT("chinesebig"_hash), CHINESEBIG5_CHARSET},
	{TEXT("default"_hash), DEFAULT_CHARSET},
	{TEXT("easteurope"_hash), EASTEUROPE_CHARSET},
	{TEXT("gb2312"_hash), GB2312_CHARSET},
	{TEXT("greek"_hash), GREEK_CHARSET},
	{TEXT("hangul"_hash), HANGUL_CHARSET},
	{TEXT("mac"_hash), MAC_CHARSET},
	{TEXT("oem"_hash), OEM_CHARSET},
	{TEXT("russian"_hash), RUSSIAN_CHARSET},
	{TEXT("shiftjis"_hash), SHIFTJIS_CHARSET},
	{TEXT("symbol"_hash), SYMBOL_CHARSET},
	{TEXT("turkish"_hash), TURKISH_CHARSET},
	{TEXT("vietnamese"_hash), VIETNAMESE_CHARSET}
};

/*!
 * \brief blah
 *
 * blah
 */
UINT parseFontCharSet(const TString &charset)
{
	if (const auto it = charset_map.find(std::hash<TString>{}(charset)); it != charset_map.end())
		return it->second;

	return DEFAULT_CHARSET;
}

TString ParseLogfontToCommand(const LPLOGFONT lf)
{
	TString flags(TEXT('+')), charset(TEXT("default")), tmp;

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

	if (hdc == nullptr)
		return tmp;

	Auto(ReleaseDC(nullptr, hdc));

	const auto hf = CreateFontIndirect(lf);

	if (hf == nullptr)
		return tmp;

	Auto(DeleteFont(hf));

	const auto oldhf = SelectFont(hdc, hf);

	Auto(SelectFont(hdc, oldhf));

	if (TEXTMETRIC tm{}; GetTextMetrics(hdc, &tm))
	{
		//auto ptSize = (int) (-1 * (lfCurrent.lfHeight * 72 / GetDeviceCaps(hdc, LOGPIXELSY)));
		const auto ptSize = MulDiv(tm.tmHeight - tm.tmInternalLeading, 72, GetDeviceCaps(hdc, LOGPIXELSY));
		// [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
		//tmp.tsprintf(TEXT("%s %s %d %s"), flags.to_chr(), charset.to_chr(), ptSize, lf->lfFaceName);
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
//		return NULL;
//	}
//
//	// index is -1
//	if (index < 0) {
//		Dcx::error(TEXT("dcxLoadIcon"), TEXT("Invalid Index"));
//		return NULL;
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
//		return NULL;
//	}
//
//	// Check for valid filename
//	if (!IsFile(filename)) {
//		Dcx::errorex(TEXT("dcxLoadIcon"), TEXT("Could Not Access File: %s"), filename.to_chr());
//		return NULL;
//	}
//
//	HICON icon = NULL;
//
//	if (xflags[TEXT('a')]) {
//		WORD wIndex = (WORD)index;
//		icon = ExtractAssociatedIcon(NULL, filename.to_chr(), &wIndex);
//	}
//#ifdef DCX_USE_GDIPLUS
//	else if (xflags[TEXT('P')]) {
//		if (!Dcx::GDIModule.isUseable())
//		{
//			Dcx::error(TEXT("dcxLoadIcon"), TEXT("Invalid +P without GDI+."));
//			return NULL;
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
//				//if (p_Thumb != NULL) {
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
//			return NULL;
//		}
//	}
//#endif
//	else {
//		if (large)
//			ExtractIconEx(filename.to_chr(), index, &icon, NULL, 1);
//		else
//			ExtractIconEx(filename.to_chr(), index, NULL, &icon, 1);
//	}
//
//	if (xflags[TEXT('g')] && icon != NULL)
//		icon = CreateGrayscaleIcon(icon);
//
//	return icon;
//}

HICON dcxLoadIcon(const int index, TString &filename, const bool large, const TString &flags)
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

		//TString filetype;
		//filetype.tsprintf(TEXT(".%s"), filename.to_chr());

		//TString filetype(TEXT('.'));
		//filetype += filename;

		TString filetype{ TEXT("."), filename };

		if (SHGetFileInfo(filetype.to_chr(), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO), (UINT)(SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | (large ? SHGFI_LARGEICON : SHGFI_SMALLICON))) == 0)
			throw Dcx::dcxException("dcxLoadIcon: Unable to get filetype icon");

		return shfi.hIcon;
	}

	// Check for valid filename
	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("dcxLoadIcon: Could Not Access File: %"), filename);

	HICON icon = nullptr;

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

		const auto p_Img = std::make_unique<Gdiplus::Bitmap>(filename.to_chr());

		// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
		if (const auto status = p_Img->GetLastStatus(); status != Gdiplus::Ok)
			throw Dcx::dcxException(TEXT("dcxLoadIcon: %"), GetLastStatusStr(status));

		//int w = 0, h = 0;
		//if (large) {
		//	w = GetSystemMetrics(SM_CXICON);
		//	h = GetSystemMetrics(SM_CYICON);
		//}
		//else {
		//	w = GetSystemMetrics(SM_CXSMICON);
		//	h = GetSystemMetrics(SM_CYSMICON);
		//}
		//const auto p_Thumb = std::make_unique<Image>(p_Img->GetThumbnailImage(w,h));
		//p_Thumb->GetHICON(&icon);

		// for reasons unknown this causes a `first chance exception` to show in debug log.
		if (const auto status = p_Img->GetHICON(&icon); status != Gdiplus::Ok)
			throw Dcx::dcxException(TEXT("dcxLoadIcon: %"), GetLastStatusStr(status));

		GdiFlush();
	}
#else
	else if (xflags[TEXT('P')])
		throw Dcx::dcxException("dcxLoadIcon: Invalid +P without GDI+.");
#endif
	else {
		if (large)
			ExtractIconEx(filename.to_chr(), index, &icon, nullptr, 1);
		else
			ExtractIconEx(filename.to_chr(), index, nullptr, &icon, 1);
	}

	if (xflags[TEXT('g')])
		icon = CreateGrayscaleIcon(icon);

	if (icon == nullptr)
		throw Dcx::dcxException("dcxLoadIcon: Unable to create icon");

	return icon;
}

HBITMAP dcxLoadBitmap(HBITMAP dest, TString &filename)
{
	filename.trim();

	if (dest != nullptr)
	{
		DeleteBitmap(dest);
		dest = nullptr;
	}
	if (!IsFile(filename))
	{
		//Dcx::errorex(TEXT("dcxLoadBitmap"), TEXT("Could Not Access File: %s"), filename.to_chr());
		Dcx::error(TEXT("dcxLoadBitmap"), TEXT("Could Not Access File: %"), filename);
		return nullptr;
	}

#ifdef DCX_USE_GDIPLUS
	if (Dcx::GDIModule.isUseable())
	{
		const auto p_Img = std::make_unique<Gdiplus::Bitmap>(filename.to_wchr());

		// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
		if (auto status = p_Img->GetLastStatus(); status != Gdiplus::Status::Ok)
			Dcx::error(TEXT("dcxLoadBitmap"), GetLastStatusStr(status));
		else {
			status = p_Img->GetHBITMAP(Gdiplus::Color(), &dest);
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
		dest = (HBITMAP)LoadImage(GetModuleHandle(NULL), filename.to_chr(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
#else
	dest = (HBITMAP)LoadImage(GetModuleHandle(NULL), filename.to_chr(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
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

//COLORREF defaultGrayPalette[256];
//bool bGrayPaletteSet = false;

HICON CreateGrayscaleIcon( HICON hIcon, const COLORREF *const pPalette ) noexcept
{
	if ((hIcon == nullptr) || (pPalette == nullptr))
		return nullptr;

	auto hdc = ::GetDC(nullptr);
	Auto(::ReleaseDC(nullptr, hdc));

	HICON      hGrayIcon = nullptr;
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
		if (icInfo.hbmColor != nullptr)
		{
			Auto(::DeleteObject(icInfo.hbmColor));

			if (::GetDIBits(hdc, icInfo.hbmColor, 0, 0, nullptr, &bmpInfo, DIB_RGB_COLORS) != 0)
			{
				//SIZE sz = { 0 };
				//sz.cx = bmpInfo.bmiHeader.biWidth;
				//sz.cy = bmpInfo.bmiHeader.biHeight;

				SIZE sz{ bmpInfo.bmiHeader.biWidth, bmpInfo.bmiHeader.biHeight };

				bmpInfo.bmiHeader.biCompression = BI_RGB;

				const auto c1 = gsl::narrow_cast<DWORD>(sz.cx * sz.cy);

				const auto lpBits = (LPDWORD)::GlobalAlloc(GMEM_FIXED, (c1) * 4);

				if (lpBits && ::GetDIBits(hdc, icInfo.hbmColor, 0, gsl::narrow_cast<UINT>(sz.cy), lpBits, &bmpInfo, DIB_RGB_COLORS) != 0)
				{
					auto lpBitsPtr     = (LPBYTE)lpBits;

					for (auto i = decltype(c1){0}; i < c1; ++i)
					{
						auto off = (UINT)( 255 - (( lpBitsPtr[0] + lpBitsPtr[1] + lpBitsPtr[2] ) / 3) );

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

					if (icGrayInfo.hbmColor != nullptr)
					{
						Auto(::DeleteObject(icGrayInfo.hbmColor));

						::SetDIBits(hdc, icGrayInfo.hbmColor, 0, gsl::narrow_cast<UINT>(sz.cy), lpBits, &bmpInfo, DIB_RGB_COLORS);

						icGrayInfo.hbmMask = icInfo.hbmMask;
						icGrayInfo.fIcon   = TRUE;

						hGrayIcon = ::CreateIconIndirect(&icGrayInfo);
					}

					::GlobalFree(lpBits);
				}
			}

			if (icInfo.hbmMask != nullptr)
				::DeleteObject(icInfo.hbmMask);
		}
	}
	return hGrayIcon;
}

HICON CreateGrayscaleIcon( HICON hIcon ) noexcept
{
	if (hIcon == nullptr)
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

void AddToolTipToolInfo(const HWND tiphwnd, const HWND ctrl) noexcept
{
	TOOLINFO ti{};
	ti.cbSize = sizeof(TOOLINFO);
	//ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_PARSELINKS;
	ti.uFlags = TTF_IDISHWND | TTF_TRANSPARENT | TTF_SUBCLASS | TTF_PARSELINKS;
	ti.hwnd = ctrl;
	ti.uId = (UINT_PTR)ctrl;
	ti.lpszText = LPSTR_TEXTCALLBACK;

	SendMessage(tiphwnd, TTM_ADDTOOL, NULL, (LPARAM)&ti);
}

void dcxDrawShadowText(HDC hdc, LPCWSTR pszText, UINT cch, RECT *pRect, DWORD dwFlags, COLORREF crText, COLORREF crShadow, int ixOffset, int iyOffset) noexcept
{
	if ((hdc == nullptr) || (pszText == nullptr) || (pRect == nullptr))
		return;

	if (DrawShadowText(hdc, pszText, cch, pRect, dwFlags, crText, crShadow, ixOffset, iyOffset) == 0)
	{
		if (dcx_testflag(dwFlags,DT_CALCRECT))
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
			DrawTextW(hdc, pszText, gsl::narrow_cast<int>(cch), (LPRECT)pRect, dwFlags);
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
constexpr const TCHAR *GetLastStatusStr(Gdiplus::Status status) noexcept
{
	//if (status > DCX_MAX_GDI_ERRORS)
	//	return GDIErrors[1]; // status not in table, return GenericError
	//return GDIErrors[(UINT)status];

	if (gsl::narrow_cast<UINT>(status) >= Dcx::countof(GDIErrors))
		return GDIErrors[1]; // status not in table, return GenericError
	return GDIErrors[gsl::narrow_cast<UINT>(status)];
}
#endif

/*
 * Tell if a file exists.
 * NB: What it really does is tell us if we can access the file, it can fail to get access for various reason on XP+.
 * After a false return use GetLastError() to find the actual fail reason if you care.
*/
//bool IsFile(TString &filename)
//{
//	if (filename.empty())
//		return false;
//
//	PathUnquoteSpaces(filename.to_chr()); // Removes any "" around the path.
//
//	// try & access the filename as is first.
//	HANDLE hFile = CreateFile(filename.to_chr(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//
//	if (hFile != INVALID_HANDLE_VALUE) {
//		CloseHandle(hFile);
//		return true;
//	}
//
//	filename.strip(); // remove ctrl codes from name. this allows ctrl codes to be added to a filename that uses double spaces.
//
//	hFile = CreateFile(filename.to_chr(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (hFile != INVALID_HANDLE_VALUE) {
//		CloseHandle(hFile);
//		return true;
//	}
//
//	// if that fails try & search for the file.
//	TCHAR *buf = NULL, *f;
//
//	// find buffer size needed.
//	DWORD res = SearchPath(NULL,filename.to_chr(),NULL,0,NULL,NULL);
//	if (res > 0) {
//		// found file, alloc buffer & fill with path/file.
//		buf = new TCHAR[res + 1];
//
//		res = SearchPath(NULL,filename.to_chr(),NULL,res,buf,&f);
//	}
//	if (res == 0) {// if find failed, exit
//		delete [] buf;
//		return false;
//	}
//	// now try & access the file we found.
//	hFile = CreateFile(buf, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (hFile != INVALID_HANDLE_VALUE) {
//		CloseHandle(hFile);
//		filename = buf; // alter the filename to contain the full path.
//		delete [] buf;
//		return true;
//	}
//	delete [] buf;
//	return false;
//}

bool IsFile(TString &filename)
{
	if (filename.empty())
		return false;

	PathUnquoteSpaces(filename.to_chr()); // Removes any "" around the path.

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
		//auto buf = std::make_unique<TCHAR[]>(res + 1U);
		
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

COLORREF staticPalette[mIRC_PALETTE_SIZE] = {
	CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID,
	CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID, CLR_INVALID,
	0x470000, 0x472100, 0x474700, 0x324700, 0x004700, 0x00472c, 0x004747, 0x002747, 0x000047, 0x2e0047, 0x470047,
	0x47002a, 0x740000, 0x743a00, 0x747400, 0x517400, 0x007400, 0x007449, 0x007474, 0x004074, 0x000074, 0x4b0074,
	0x740074, 0x740045, 0xb50000, 0xb56300, 0xb5b500, 0x7db500, 0x00b500, 0x00b571, 0x00b5b5, 0x0063b5, 0x0000b5,
	0x7500b5, 0xb500b5, 0xb5006b, 0xff0000, 0xff8c00, 0xffff00, 0xb2ff00, 0x00ff00, 0x00ffa0, 0x00ffff, 0x008cff,
	0x0000ff, 0xa500ff, 0xff00ff, 0xff0098, 0xff5959, 0xffb459, 0xffff71, 0xcfff60, 0x6fff6f, 0x65ffc9, 0x6dffff,
	0x59b4ff, 0x5959ff, 0xc459ff, 0xff66ff, 0xff59bc, 0xff9c9c, 0xffd39c, 0xffff9c, 0xe2ff9c, 0x9cff9c, 0x9cffdb,
	0x9cffff, 0x9cd3ff, 0x9c9cff, 0xdc9cff, 0xff9cff, 0xff94d3, 0x000000, 0x131313, 0x282828, 0x363636, 0x4d4d4d,
	0x656565, 0x818181, 0x9f9f9f, 0xbcbcbc, 0xe2e2e2, 0xffffff
};

/*!
* \brief Get all mIRC Colours.
*
* If staticPalette[0] == CLR_INVALID - Fills the staticPalette with all mIRC's colours.
* otherwise does nothing.
*/
void getmIRCPalette()
{
	if ((staticPalette[0] == CLR_INVALID) || (!Dcx::setting_bStaticColours))
	{
		TString colors;
		static constexpr TCHAR com[] = TEXT("$color(0) $color(1) $color(2) $color(3) $color(4) $color(5) $color(6) $color(7) $color(8) $color(9) $color(10) $color(11) $color(12) $color(13) $color(14) $color(15)");
		//mIRCLinker::tsEval(colors, &com[0]);
		mIRCLinker::eval(colors, &com[0]);

		size_t i = 0;
		for (const auto &a : colors)
			staticPalette[i++] = a.to_<COLORREF>();
	}
}

/*!
* \brief Get mIRC Colours.
*
* Copies the staticPalette to Palette (updates staticPalette if needed)
*/
void getmIRCPalette(COLORREF *const Palette, const UINT PaletteItems)
{
	if ((PaletteItems > mIRC_PALETTE_SIZE) || (Palette == &staticPalette[0]))
		return;

	getmIRCPalette();

	CopyMemory(Palette, &staticPalette[0], sizeof(COLORREF) * PaletteItems);
}

/*!
* \brief Get mIRC Colours.
*
* Copies the staticPalette to Palette (updates staticPalette if needed)
*/
void getmIRCPalette(gsl::span<COLORREF> Palette)
{
	getmIRCPalette();

	gsl::copy(gsl::span<COLORREF>(staticPalette), Palette);
}

// void getmIRCPaletteMask(COLORREF *const Palette, const UINT PaletteItems, uint16_t uMask)
// Palette      - Palette to fill
// PaletteItems - Total number of items in Palette
// uMask        - bitmask of items to get.
//
// Copies staticPalette to Palette based on a 16bit mask, one bit for every colour (updates staticPalette if needed)
// NB: This ONLY updates the first 16 colours, not the full 99, > 16 are static anyway...
void getmIRCPaletteMask(COLORREF *const Palette, const UINT PaletteItems, uint16_t uMask)
{
	if ((Palette == nullptr) || (PaletteItems < 16) || (PaletteItems > std::extent_v<decltype(staticPalette)>))
		return;

	getmIRCPalette();

	for (uint32_t count = 0; count < PaletteItems; ++count)
	{
		if ((uMask & 0x01) != 0)
		{
			Palette[count] = staticPalette[count];
		}
		uMask >>= 1;
	}
}

constexpr int unfoldColor(int nColor) noexcept
{
	while (nColor > 99)
		nColor -= 100;

	return nColor;
}

int unfoldColor(const WCHAR *color) noexcept
{
	return unfoldColor(_ts_atoi(color));
}

//void calcStrippedRect(HDC hdc, const TString &txt, const UINT style, LPRECT rc, const bool ignoreleft)
//{
//	if (!ignoreleft || (dcx_testflag(style, DT_CENTER)) || (dcx_testflag(style, DT_RIGHT))) {
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

void mIRC_OutText(HDC hdc, TString &txt, LPRECT rcOut, const LPLOGFONT lf, const UINT iStyle, const COLORREF clrFG, const bool shadow) noexcept
{
	if (txt.empty())
		return;

	const auto len = txt.len();
	TEXTMETRICW tm = { 0 };
	const auto hOldFont = SelectFont(hdc, CreateFontIndirect(lf));
	GetTextMetrics(hdc, &tm);
	auto rcTmp = *rcOut;

	if (!dcx_testflag(iStyle,DT_CALCRECT))
	{	// if DT_CALCRECT flag NOT given then do calcrect here.
		//DrawText(hdc, txt.to_chr(), len, &rcTmp, iStyle | DT_CALCRECT);
		if (shadow)
			dcxDrawShadowText(hdc,txt.to_chr(), len, &rcTmp, iStyle | DT_CALCRECT, clrFG, 0, 5, 5);
		else
			DrawText(hdc, txt.to_chr(), gsl::narrow_cast<int>(len), &rcTmp, iStyle | DT_CALCRECT);
	}
	if (shadow)
		dcxDrawShadowText(hdc,txt.to_chr(), len, &rcTmp, iStyle, clrFG, 0, 5, 5);
	else
		DrawText(hdc, txt.to_chr(), gsl::narrow_cast<int>(len), &rcTmp, iStyle);

	rcOut->left += (rcTmp.right - rcTmp.left) - tm.tmOverhang;
	DeleteFont(SelectFont( hdc, hOldFont ));
	txt.clear();	// txt = TEXT("");
}

void mIRC_DrawText(HDC hdc, const TString &txt, LPRECT rc, const UINT style, const bool shadow)
{
	if (txt.empty()) // if no text just exit.
		return;

	// create an hdc buffer to avoid flicker during drawing.
	const auto hBuffer = CreateHDCBuffer(hdc, rc);

	if (hBuffer == nullptr)
		return;

	Auto(DeleteHDCBuffer(hBuffer));

	auto rcOut = *rc;
	auto oldHDC = hdc;
	hdc = *hBuffer;
	// change rcOut to be zero offset.
	OffsetRect(&rcOut,-rcOut.left, -rcOut.top);

	//savedDC = SaveDC(hdc);

	const COLORREF origFG = GetTextColor(hdc), origBG = GetBkColor(hdc);
	COLORREF clrFG = origFG, clrBG = origBG;
	COLORREF cPalette[mIRC_PALETTE_SIZE] = {CLR_INVALID}; // mIRC palette

	//getmIRCPalette(&cPalette[0], std::extent_v<decltype(cPalette)>); // get mIRC palette
	getmIRCPalette(cPalette); // get mIRC palette

	const auto hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);

	LOGFONT lf{};
	if (GetObject(hFont, sizeof(LOGFONT), &lf) == 0)
		return;

	// bug #721 -> http://dcx.scriptsdb.org/bug/index.php?do=details&task_id=721
	/*TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);*/
	/*DRAWTEXTPARAMS dtp = {0};
	dtp.cbSize = sizeof(DRAWTEXTPARAMS);
	dtp.iTabLength = 4;*/

	const auto origWeight = lf.lfWeight;
	auto origLeft = rc->left;

	SetBkMode(hdc,TRANSPARENT);

	const UINT iStyle = (style & ~(DT_CENTER | DT_RIGHT | DT_VCENTER)) | DT_LEFT; // make sure its to left

	if (!dcx_testflag(iStyle,DT_CALCRECT))
	{ // if NOT doing DT_CALCRECT calc rect needed here.
		//mIRC_DrawText(hdc, txt, &rcOut, style | DT_CALCRECT, shadow);
		if (dcx_testflag(style,DT_CENTER) || dcx_testflag(style,DT_RIGHT) || dcx_testflag(style,DT_VCENTER))
		{
			// strip out ctrl codes to correctly position text.
			auto rcTmp = *rc;
			auto t(txt);
			if (dcx_testflag(iStyle,DT_SINGLELINE))
			{
				//t.replace(TEXT('\n'), TEXT(' '));
				//t.replace(TEXT('\r'), TEXT(' '));
				t.mreplace(TEXT(' '), TEXT("\n\r"));
			}
			DrawText(hdc, t.strip().to_chr(), -1, &rcTmp, style | DT_CALCRECT /*| DT_NOPREFIX*/);	// using t.len() gives invalid length (issue with strip())
			//DrawText(hdc, t.strip().to_chr(), t.len(), &rcTmp, style | DT_CALCRECT /*| DT_NOPREFIX*/);
			//DrawText(hdc, txt, lstrlen(txt), &rcTmp, style | DT_CALCRECT /*| DT_NOPREFIX*/);
			//DrawTextEx(hdc, t.strip().to_chr(), t.len(), &rcTmp, style | DT_CALCRECT /*| DT_NOPREFIX*/, &dtp);
			// style can be either center or right, not both, but it can be center+vcenter or right+vcenter
			if (dcx_testflag(style,DT_CENTER))
			{ // get center text start offset
				// (total width) - (required width) / 2 = equal space to each side.
				origLeft = rcOut.left = (((rcOut.right - rcOut.left) - (rcTmp.right - rcTmp.left)) / 2);
			}
			else if (dcx_testflag(style,DT_RIGHT))
			{ // get right text start offset
				// adjust start to right
				origLeft = rcOut.left = (rcOut.right - (rcTmp.right - rcTmp.left));
				//origLeft = rcOut.left = (rcOut.right - (rcTmp.right - rcTmp.left) - 3);
				//origLeft = rcOut.left = (rcOut.right - (rcTmp.right - rcTmp.left) - tm.tmMaxCharWidth);
				//origLeft = rcOut.left = (rcOut.right - (rcTmp.right - rcTmp.left) - tm.tmAveCharWidth);
				//origLeft = rcOut.left = rcTmp.left;
			}
			if (dcx_testflag(style,DT_VCENTER))
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
						mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
					if (lf.lfWeight == FW_BOLD)
						lf.lfWeight = origWeight;
					else
						lf.lfWeight = FW_BOLD;
				}
				break;
				case 3: // Colour
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);

					while (wtxt[pos + 1] == 3)
						++pos; // remove multiple consecutive ctrl-k's

					SetBkMode(hdc, TRANSPARENT);

					//usingBGclr = false;
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
							clrFG = cPalette[umIRCColour];
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
								clrBG = cPalette[umIRCColour];
								SetBkMode(hdc, OPAQUE);
							}
						}
						if (usingRevTxt)
						{ // reverse text swap fg & bg colours
							const auto ct = clrFG;
							clrFG = clrBG;
							clrBG = ct;
							SetBkMode(hdc, OPAQUE);
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
					if (!tmp.empty())
						mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);

					while (wtxt[pos + 1] == 15)
						++pos; // remove multiple consecutive ctrl-o's

					lf.lfWeight = origWeight;
					lf.lfUnderline = 0;
					clrFG = origFG;
					clrBG = origBG;
					SetTextColor(hdc, origFG);
					SetBkColor(hdc, origBG);
					SetBkMode(hdc, TRANSPARENT);
					usingRevTxt = false;
				}
				break;
				case 22: // ctrl+r
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);

					usingRevTxt = (usingRevTxt ? false : true);

					if (usingRevTxt)
						SetBkMode(hdc, OPAQUE);
					else
						SetBkMode(hdc, TRANSPARENT);

					const COLORREF ct = clrBG;
					clrBG = clrFG;
					clrFG = ct;
					SetTextColor(hdc, clrFG);
					SetBkColor(hdc, clrBG);
				}
				break;
				case 29: // ctrl-i Italics
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
					lf.lfItalic = (lf.lfItalic ? 0U : 1U);
				}
				break;
				case 31: // ctrl+u
				{
					if (!tmp.empty())
						mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
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
							GetTextExtentPoint32(hdc, tmp.to_chr(), gsl::narrow_cast<int>(tlen), &sz);
							mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
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
							int nFit = 0;
							const auto tlen = gsl::narrow_cast<int>(tmp.len());
							GetTextExtentExPoint(hdc, txt.to_chr(), tlen, (rcOut.right - rcOut.left), &nFit, NULL, &sz);
							if (nFit < tlen)
							{
								if (nFit > 0)
								{
									const auto o = tmp[nFit];
									//mIRC_OutText(hdc, tmp.wsub(0,nFit), &rcOut, &lf, iStyle, clrFG, shadow);
									auto tsSub(tmp.sub(0, nFit));
									mIRC_OutText(hdc, tsSub, &rcOut, &lf, iStyle, clrFG, shadow);
									//tmp = "";
									rcOut.top += sz.cy;
									rcOut.left = origLeft;
									tmp = o;
									//mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
								}
								else
									tmp.clear();	// tmp = TEXT("");
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
			mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
	}

	//RestoreDC(hdc, savedDC);

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

struct HDCBuffer {
	HDC m_hHDC;
	HBITMAP m_hOldBitmap;
	HBITMAP m_hBitmap;
	HFONT m_hOldFont;
	// needs work...
};
using LPHDCBuffer = HDCBuffer *;

gsl::owner<HDC *> CreateHDCBuffer(HDC hdc, const LPRECT rc)
{
	//if ((hdc == nullptr) /*|| (rc == NULL)*/)
	//	return nullptr;

	// alloc buffer data
	auto buf = std::make_unique<HDCBuffer>();

	// create HDC for buffer.
	buf->m_hHDC = CreateCompatibleDC(hdc);
	if (buf->m_hHDC == nullptr)
		return nullptr;

	// get size of bitmap to alloc.
	BITMAP bm = { 0 };
	int x = 0, y = 0;

	if (rc == nullptr)
	{
		// no size specified, use hdc's bitmap size.
		if (GetObject((HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP), sizeof(BITMAP), &bm) == 0)
			return nullptr;
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

	if (buf->m_hBitmap == nullptr)
	{
#ifdef DEBUG
		const DWORD err = GetLastError(), errBufSize = 16U;
		if (const LPTSTR errBuf = nullptr; FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, err, 0, (LPTSTR)&errBuf, errBufSize, nullptr) != 0)
		{
			mIRCLinker::debug(TEXT("CreateHDCBuffer"),errBuf);
			LocalFree(errBuf);
		}
#endif
		DeleteDC(buf->m_hHDC);
		return nullptr;
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
	SetGraphicsMode(buf->m_hHDC, GetGraphicsMode(hdc));

	// copy contents of hdc within area to buffer.
	BitBlt(buf->m_hHDC, 0, 0, bm.bmWidth, bm.bmHeight, hdc, x, y, SRCCOPY);

	// buffer is an exact duplicate of the hdc within the area specified.
	// return buffer typed as an HDC *
	//return (HDC *)buf.release();
	return gsl::owner<HDC *>(buf.release());
}

void DeleteHDCBuffer(gsl::owner<HDC *> hBuffer) noexcept
{
	if (hBuffer == nullptr)
		return;

	auto buf = gsl::owner<LPHDCBuffer>(reinterpret_cast<LPHDCBuffer>(hBuffer));

	GdiFlush();
	SelectFont(buf->m_hHDC, buf->m_hOldFont);
	SelectBitmap(buf->m_hHDC,buf->m_hOldBitmap);
	DeleteBitmap(buf->m_hBitmap);
	DeleteDC(buf->m_hHDC);
	delete buf;
}

int TGetWindowText(HWND hwnd, TString &txt)
{
	if (hwnd == nullptr)
		return 0;

	// NB: needs to include space for end 0
	if (const auto nText = GetWindowTextLength(hwnd) + 2; nText > 2)
	{
		txt.reserve(gsl::narrow_cast<UINT>(nText));
		if (GetWindowText(hwnd, txt.to_chr(), nText) != 0)
			return nText;
	}
	txt.clear();	// txt = TEXT("");
	return 0;
}

TString TGetWindowText(HWND hwnd)
{
	TString txt;

	if (hwnd == nullptr)
		return txt;

	// NB: needs to include space for end 0
	if (const auto nText = GetWindowTextLength(hwnd) + 2; nText > 2)
	{
		txt.reserve(gsl::narrow_cast<UINT>(nText));
		GetWindowText(hwnd, txt.to_chr(), nText);
	}
	return txt;
}

HMODULE UXModule = nullptr;         //!< UxTheme.dll Module Handle
#ifdef DCX_USE_GDIPLUS
HMODULE GDIPlusModule = nullptr;	//!< gdiplus.dll Module Handle
ULONG_PTR gdi_token = NULL;
#endif
HMODULE DWMModule = nullptr;		//!< dwmapi.dll Module Handle

void FreeOSCompatibility(void) noexcept
{
#ifdef DCX_USE_GDIPLUS
	// Shutdown GDI+
	if (GDIPlusModule != nullptr)
	{
		if (gdi_token != NULL)
			Gdiplus::GdiplusShutdown(gdi_token);

		FreeLibrary(GDIPlusModule);
		GDIPlusModule = nullptr;
	}
#endif

	if (UXModule != nullptr)
	{
		Dcx::UXModule.dcxBufferedPaintUnInit();

		FreeLibrary(UXModule);
		UXModule = nullptr;
	}
	if (DWMModule != nullptr)
	{
		FreeLibrary(DWMModule);
		DWMModule = nullptr;
	}
}

bool isRegexMatch(const TCHAR *matchtext, const TCHAR *pattern)
{
	if ((matchtext == nullptr) || (pattern == nullptr))
		return false;

	// NB: CREGEX version is incomplete
#if DCX_USE_CREGEX
	try {
		const auto patlen = _ts_strlen(pattern);
		if ((pattern[0] == TEXT('/')) && (pattern[patlen - 1] == TEXT('/')))
		{
			// need to remove the // from the pattern so /^blah$/ becomes ^blah$
			// also need to handle m/^blah$/i etc..

			//auto tmp = std::make_unique<TCHAR[]>(patlen);
			//_ts_strcpyn(tmp.get(), pattern +1, patlen -2);
			//std::basic_regex<TCHAR> r(tmp.get(), std::regex_constants::ECMAScript);
			std::basic_regex<TCHAR> r(pattern, std::regex_constants::awk);
			return std::regex_search(matchtext, r);
		}
		else {
			std::basic_regex<TCHAR> r(pattern, std::regex_constants::awk);
			return std::regex_search(matchtext, r);
		}
	}
	catch (const std::regex_error) {
	}
	return false;
#else
	//stString<10> res;
	//mIRCLinker::execex(TEXT("/set -nu1 %%dcx_text %s"), matchtext);
	//mIRCLinker::execex(TEXT("/set -nu1 %%dcx_regex %s"), pattern);
	//mIRCLinker::eval(res, static_cast<int>(res.size()), TEXT("$regex(%dcx_text,%dcx_regex)"));
	//return (dcx_atoi(res.data()) > 0);

	stString<10> res;
	mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_text %"), matchtext);
	mIRCLinker::exec(TEXT("/set -nu1 \\%dcx_regex %"), pattern);
	mIRCLinker::eval(res, TEXT("$regex(%dcx_text,%dcx_regex)"));

	return (dcx_atoi(res.data()) > 0);
#endif // DCX_USE_CREGEX
}

void AddFileIcons(HIMAGELIST himl, TString &filename, const bool bLarge, const int iIndex, const int iStart, const int iEnd)
{
	if ((himl == nullptr) || (iStart < 0) || (iEnd != -1 && iStart > iEnd))
		throw Dcx::dcxException(TEXT("Invalid Args"));

	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("Unable to Access file: %"), filename);

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
	HICON hIcon = nullptr;
	
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
		auto IconGuard = gsl::finally([hIcon]() { DestroyIcon(hIcon); });
	
		if (ImageList_ReplaceIcon(himl, IconIndex, hIcon) == -1)
			throw Dcx::dcxException(TEXT("Unable to Add %'s Icon %"), filename, FileIndex);

		if (IconIndex != -1)
			++IconIndex;
	}
#endif
}

BOOL dcxGetWindowRect(const HWND hWnd, const LPRECT lpRect) noexcept
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
void DrawRotatedText(const TString &strDraw, const LPRECT rc, const HDC hDC, const int nAngleLine/* = 0*/, const bool bEnableAngleChar /*= false*/, const int nAngleChar /*= 0*/) noexcept
{
	if ((nAngleLine == 0) && (!bEnableAngleChar))
	{
		TextOut(hDC, rc->left, rc->bottom, strDraw.to_chr(), gsl::narrow_cast<int>(strDraw.len()));
		//DrawText(hDC, strDraw.to_chr(), strDraw.len(), rc, styles);
		return;
	}
	LOGFONT lf{};

	if (GetObject((HFONT)GetCurrentObject(hDC, OBJ_FONT), sizeof(LOGFONT), &lf) == 0)
		return;

	// Set the background mode to transparent for the
	// text-output operation.
	const auto nOldBkMode = SetBkMode(hDC, TRANSPARENT);
	Auto(SetBkMode(hDC, nOldBkMode));

	// Specify the angle to draw line
	lf.lfEscapement = nAngleLine*10;
	int nOldGMode{};
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
	// Restore old values
	Auto(SetGraphicsMode(hDC, nOldGMode));

	// Select the new font created
	if (const auto hFont = CreateFontIndirect(&lf); hFont != nullptr)
	{
		Auto(DeleteFont(hFont));

		const auto hPrevFont = SelectFont(hDC, hFont);
		Auto(SelectFont(hDC, hPrevFont));

		// Draw text to screen
		//TextOut(hDC, rc->right / 2, rc->bottom / 2, strDraw.to_chr(), strDraw.len());
		TextOut(hDC, rc->left, rc->bottom, strDraw.to_chr(), gsl::narrow_cast<int>(strDraw.len()));
		//DrawText(hDC, strDraw.to_chr(), strDraw.len(), rc, styles);
	}
}

//void DrawRotatedText(HDC hdc, TCHAR *str, LPRECT rect, double angle, UINT nOptions = 0)
//{
//	// convert angle to radian
//	constexpr double pi = 3.141592654;
//	double radian = pi * 2 / 360 * angle;
//
//	size_t len = lstrlen(str);
//
//	// get the center of a not-rotated text
//	SIZE TextSize;;
//	GetTextExtentPoint32(hdc, str, len, &TextSize);
//
//	POINT center;
//	center.x = TextSize.cx / 2;
//	center.y = TextSize.cy / 2;
//
//	// now calculate the center of the rotated text
//	POINT rcenter;
//	rcenter.x = long(cos(radian) * center.x - sin(radian) * center.y);
//	rcenter.y = long(sin(radian) * center.x + cos(radian) * center.y);
//
//	// finally draw the text and move it to the center of the rectangle
//	SetTextAlign(hdc, TA_BASELINE);
//	SetBkMode(hdc, TRANSPARENT);
//	ExtTextOut(hdc, rect->left + (rect->right - rect->left) / 2 - rcenter.x,
//		rect->top + (rect->bottom - rect->top) / 2 + rcenter.y,
//		nOptions, rect, str, len, NULL);
//}

const char *queryAttribute(const TiXmlElement *element, const char *attribute, const char *defaultValue) noexcept
{
	const auto t = element->Attribute(attribute);
	return (t != nullptr) ? t : defaultValue;
}

//gsl::not_null<const char *> queryAttribute(gsl::not_null<const TiXmlElement *> element, gsl::not_null<const char *> attribute, gsl::not_null<const char *> defaultValue) noexcept
//{
//	const auto t = element->Attribute(attribute);
//	return gsl::not_null<const char *>((t != nullptr) ? t : defaultValue.get());
//}

//std::optional<const char *> queryAttribute(gsl::not_null<const TiXmlElement *> element, gsl::not_null<const char *> attribute) noexcept
//{
//	const auto t = element->Attribute(attribute);
//	if (t != nullptr)
//		return t;
//	return {};
//}

int queryIntAttribute(const TiXmlElement *element, const char *attribute, const int defaultValue)
{
	if (const auto[iStatus, integer] = element->QueryIntAttribute(attribute); iStatus == TIXML_SUCCESS)
		return integer;

	return defaultValue;
}

//int queryIntAttribute(gsl::not_null<const TiXmlElement *> element, gsl::not_null<const char *> attribute, const int defaultValue)
//{
//	if (const auto[iStatus, integer] = element->QueryIntAttribute(attribute); iStatus == TIXML_SUCCESS)
//		return integer;
//
//	return defaultValue;
//}

//std::optional<int> queryIntAttribute(gsl::not_null<const TiXmlElement *> element, gsl::not_null<const char *> attribute)
//{
//	if (const auto[iStatus, integer] = element->QueryIntAttribute(attribute); iStatus == TIXML_SUCCESS)
//		return integer;
//
//	return {};
//}

/*
	Make the text safe for passing to the callback alias

	Change special characters $%()[]; into there $chr() equivalents
*/
TString MakeTextmIRCSafe(const TString &tsStr)
{
	return MakeTextmIRCSafe(tsStr.to_chr(), tsStr.len());
}

/*
Make the text safe for passing to the callback alias

Change special characters $%()[]; into there $chr() equivalents
*/
TString MakeTextmIRCSafe(const TCHAR *const tString)
{
	return MakeTextmIRCSafe(tString, _ts_strlen(tString));
}

/*
Make the text safe for passing to the callback alias

Change special characters $%()[]; into there $chr() equivalents
*/
TString MakeTextmIRCSafe(const TCHAR *const tString, const std::size_t len)
{
	TString tsRes((UINT)MIRC_BUFFER_SIZE_CCH);	// use MIRC_BUFFER_SIZE_CCH as starting buffer size (shouldnt be bigger...)

	if ((tString == nullptr) || (len == 0))
		return tsRes;

	bool bLastWasSpace = true;	// start as true as the beginning of the line is treated the same as a space here

	// look for ()[]%$; we dont want to change , as this is needed
	for (auto i = decltype(len){0}; i < len; ++i)
	{
		switch (const auto &c = tString[i]; c)
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
