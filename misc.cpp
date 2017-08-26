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
int dcx_round(const float x) {
	const auto t = static_cast<int>(x);
	if (x - static_cast<float>(t) > 0.5)
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
	//Ouvrir le fichier, read en mode binaire
	if (filename == nullptr)
		return nullptr;

#if DCX_USE_WRAPPERS
	//Dcx::dcxFileResource file(filename.to_wchr(), TEXT("rb"));
	Dcx::dcxFileResource file(filename, TEXT("rb"));
#else
	auto file = dcx_fopen(filename, TEXT("rb"));
	
	// Open file in binary mode and read
	if (file == nullptr)
		return nullptr;
	
	Auto(fclose(file));
#endif

	// Seek End of file
	if (fseek(file, 0, SEEK_END))
		return nullptr;

	// Read pointer location, because pointer is at the end, results into file size
	auto size = static_cast<size_t>(ftell(file));

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
}

/*!
* \brief Read Text File Contents
*
* This function correctly handles BOM types for ascii,utf8,utf16BE,utf16LE & returns text in a TString object.
*/
TString readTextFile(const TString &tFile)
{
	auto data = readFile(tFile);

	if (data == nullptr)
		return TString();

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
	//Dcx::dcxFileResource file(tsFile.to_wchr(), TEXT("wb"));
	Dcx::dcxFileResource file(tsFile, TEXT("wb"));
#else
	auto file = dcx_fopen(tsFile.to_chr(), TEXT("wb"));
	
	if (file == nullptr)
		return false;
	
	Auto(fclose(file));
#endif

	const static WCHAR tBOM = 0xFEFF;	// unicode BOM

	fwrite(&tBOM, sizeof(TCHAR), 1, file);

	// if not in unicode mode then save without BOM as ascii/utf8
	fwrite(tsData.to_chr(), sizeof(TCHAR), tsData.len(), file);
	fflush(file);
	return true;
}

/*!
* \brief Copies string to the clipboard
*
* Returns TRUE if successful
*
* http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/dataexchange/clipboard/usingtheclipboard.asp
*/
bool CopyToClipboard(const HWND owner, const TString & str) {
	if (!OpenClipboard(owner)) {
		Dcx::error(TEXT("CopyToClipboard"),TEXT("Couldn't open clipboard"));
		return false;
	}

	Auto(CloseClipboard());

	const size_t cbsize = (str.len() +1) * sizeof(TCHAR);
	EmptyClipboard();
	auto hglbCopy = GlobalAlloc(GMEM_MOVEABLE, cbsize);

	if (hglbCopy == nullptr) {
		Dcx::error(TEXT("CopyToClipboard"),TEXT("Couldn't open global memory"));
		return false;
	}

	{	// simply sets scope for vars
		auto strCopy = (TCHAR *)GlobalLock(hglbCopy);

		if (strCopy == nullptr) {
			GlobalFree(hglbCopy);
			Dcx::error(TEXT("CopyToClipboard"), TEXT("Couldn't lock global memory"));
			return false;
		}

		dcx_strcpyn(strCopy, str.to_chr(), cbsize);

		GlobalUnlock(hglbCopy);
	}

	SetClipboardData(CF_UNICODETEXT, hglbCopy);
	return true;
}


// Turns a command (+flags CHARSET SIZE FONTNAME) into a LOGFONT struct
bool ParseCommandToLogfont(const TString& cmd, LPLOGFONT lf) {
	if (cmd.numtok( ) < 4)
		return false;

	ZeroMemory(lf, sizeof(LOGFONT));
	const auto flags = parseFontFlags(cmd.getfirsttok( 1 ));

	if (dcx_testflag(flags, dcxFontFlags::DCF_DEFAULT))
		return (GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), lf) != 0);

	lf->lfCharSet = (BYTE)(parseFontCharSet(cmd.getnexttok( )) & 0xFF);	// tok 2
	const auto fSize = cmd.getnexttok( ).to_int();				// tok 3
	const auto fName(cmd.getlasttoks( ).trim());				// tok 4, -1

	if (!fSize)
		return false;

	{
		auto hdc = GetDC(nullptr);

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

	dcx_strcpyn(&lf->lfFaceName[0], fName.to_chr(), Dcx::countof(lf->lfFaceName));
	lf->lfFaceName[31] = 0;

	return true;
}


/*!
 * \brief blah
 *
 * blah
 */
UINT parseFontFlags(const TString &flags) {

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

/*!
 * \brief blah
 *
 * blah
 */
UINT parseFontCharSet(const TString &charset) {

//#ifndef DCX_SWITCH_OBJ
//	if (charset == TEXT("ansi"))
//		return ANSI_CHARSET;
//	else if (charset == TEXT("baltic"))
//		return BALTIC_CHARSET;
//	else if (charset == TEXT("chinesebig"))
//		return CHINESEBIG5_CHARSET;
//	else if (charset == TEXT("default"))
//		return DEFAULT_CHARSET;
//	else if (charset == TEXT("easteurope"))
//		return EASTEUROPE_CHARSET;
//	else if (charset == TEXT("gb2312"))
//		return GB2312_CHARSET;
//	else if (charset == TEXT("greek"))
//		return GREEK_CHARSET;
//	else if (charset == TEXT("hangul"))
//		return HANGUL_CHARSET;
//	else if (charset == TEXT("mac"))
//		return MAC_CHARSET;
//	else if (charset == TEXT("oem"))
//		return OEM_CHARSET;
//	else if (charset == TEXT("russian"))
//		return RUSSIAN_CHARSET;
//	else if (charset == TEXT("shiftjis"))
//		return SHIFTJIS_CHARSET;
//	else if (charset == TEXT("symbol"))
//		return SYMBOL_CHARSET;
//	else if (charset == TEXT("turkish"))
//		return TURKISH_CHARSET;
//	else if (charset == TEXT("vietnamese"))
//		return VIETNAMESE_CHARSET;
//	
//	return DEFAULT_CHARSET;
//#else
//	UINT res = DEFAULT_CHARSET;
//	Switch(charset)
//		.Case(TEXT("ansi"), [&res] { res = ANSI_CHARSET; }).Break()
//		.Case(TEXT("baltic"), [&res] { res = BALTIC_CHARSET; }).Break()
//		.Case(TEXT("chinesebig"), [&res] { res = CHINESEBIG5_CHARSET; }).Break()
//		//.Case(TEXT("default"), [&res] { res = DEFAULT_CHARSET; }).Break()
//		.Case(TEXT("easteurope"), [&res] { res = EASTEUROPE_CHARSET; }).Break()
//		.Case(TEXT("gb2312"), [&res] { res = GB2312_CHARSET; }).Break()
//		.Case(TEXT("greek"), [&res] { res = GREEK_CHARSET; }).Break()
//		.Case(TEXT("hangul"), [&res] { res = HANGUL_CHARSET; }).Break()
//		.Case(TEXT("mac"), [&res] { res = MAC_CHARSET; }).Break()
//		.Case(TEXT("oem"), [&res] { res = OEM_CHARSET; }).Break()
//		.Case(TEXT("russian"), [&res] { res = RUSSIAN_CHARSET; }).Break()
//		.Case(TEXT("shiftjis"), [&res] { res = SHIFTJIS_CHARSET; }).Break()
//		.Case(TEXT("symbol"), [&res] { res = SYMBOL_CHARSET; }).Break()
//		.Case(TEXT("turkish"), [&res] { res = TURKISH_CHARSET; }).Break()
//		.Case(TEXT("vietnamese"), [&res] { res = VIETNAMESE_CHARSET; }).Break();
//		//.Default([&res] { res = DEFAULT_CHARSET; });
//
//	return res;
//#endif

	UINT res = DEFAULT_CHARSET;
	static std::map<TString, UINT> charset_map;

	if (charset_map.empty())
	{
		charset_map[TEXT("ansi")] = ANSI_CHARSET;
		charset_map[TEXT("baltic")] = BALTIC_CHARSET;
		charset_map[TEXT("chinesebig")] = CHINESEBIG5_CHARSET;
		charset_map[TEXT("default")] = DEFAULT_CHARSET;
		charset_map[TEXT("easteurope")] = EASTEUROPE_CHARSET;
		charset_map[TEXT("gb2312")] = GB2312_CHARSET;
		charset_map[TEXT("greek")] = GREEK_CHARSET;
		charset_map[TEXT("hangul")] = HANGUL_CHARSET;
		charset_map[TEXT("mac")] = MAC_CHARSET;
		charset_map[TEXT("oem")] = OEM_CHARSET;
		charset_map[TEXT("russian")] = RUSSIAN_CHARSET;
		charset_map[TEXT("shiftjis")] = SHIFTJIS_CHARSET;
		charset_map[TEXT("symbol")] = SYMBOL_CHARSET;
		charset_map[TEXT("turkish")] = TURKISH_CHARSET;
		charset_map[TEXT("vietnamese")] = VIETNAMESE_CHARSET;
	}

	auto it = charset_map.find(charset);
	if (it != charset_map.end())
		res = it->second;

	return res;
}

TString ParseLogfontToCommand(const LPLOGFONT lf) {
	TString flags(TEXT('+')), charset(TEXT("default")), tmp;

	// get charset
	switch (lf->lfCharSet) {
		case ANSI_CHARSET			: charset = TEXT("ansi"); break;
		case BALTIC_CHARSET			: charset = TEXT("baltic"); break;
		case CHINESEBIG5_CHARSET	: charset = TEXT("chinesebig"); break;
		case EASTEUROPE_CHARSET		: charset = TEXT("easteurope"); break;
		case GB2312_CHARSET			: charset = TEXT("gb2312"); break;
		case GREEK_CHARSET			: charset = TEXT("greek"); break;
		case HANGUL_CHARSET			: charset = TEXT("hangul"); break;
		case MAC_CHARSET			: charset = TEXT("mac"); break;
		case OEM_CHARSET			: charset = TEXT("oem"); break;
		case RUSSIAN_CHARSET		: charset = TEXT("russian"); break;
		case SHIFTJIS_CHARSET		: charset = TEXT("shiftjis"); break;
		case SYMBOL_CHARSET			: charset = TEXT("symbol"); break;
		case TURKISH_CHARSET		: charset = TEXT("turkish"); break;
		case VIETNAMESE_CHARSET		: charset = TEXT("vietnamese"); break;
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

	auto hdc = GetDC(nullptr);

	if (hdc == nullptr)
		return tmp;

	Auto(ReleaseDC(nullptr, hdc));

	auto hf = CreateFontIndirect(lf);

	if (hf == nullptr)
		return tmp;

	Auto(DeleteFont(hf));

	TEXTMETRIC tm = { 0 };

	const auto oldhf = SelectFont(hdc, hf);

	Auto(SelectFont(hdc, oldhf));

	if (GetTextMetrics(hdc, &tm))
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

HICON dcxLoadIcon(const int index, TString &filename, const bool large, const TString &flags) {
	filename.trim();

	const XSwitchFlags xflags(flags);

	if (!xflags[TEXT('+')])
		throw Dcx::dcxException("dcxLoadIcon: Invalid Flags");

	// index is -1
	if (index < 0)
		throw Dcx::dcxException("dcxLoadIcon: Invalid Index");

	// This doesnt require a valid filename.
	if (xflags[TEXT('f')]) {
		SHFILEINFO shfi;
		//TString filetype;
		//filetype.tsprintf(TEXT(".%s"), filename.to_chr());

		//TString filetype(TEXT('.'));
		//filetype += filename;

		TString filetype{ TEXT("."), filename };

		ZeroMemory(&shfi, sizeof(SHFILEINFO));

		if (SHGetFileInfo(filetype.to_chr(), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO), (UINT)(SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | (large ? SHGFI_LARGEICON : SHGFI_SMALLICON))) == 0)
			throw Dcx::dcxException("dcxLoadIcon: Unable to get filetype icon");

		return shfi.hIcon;
	}

	// Check for valid filename
	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("dcxLoadIcon: Could Not Access File: %"), filename);

	HICON icon = nullptr;

	if (xflags[TEXT('a')]) {
		auto wIndex = gsl::narrow_cast<WORD>(index);
		icon = ExtractAssociatedIcon(nullptr, filename.to_chr(), &wIndex);
	}
#ifdef DCX_USE_GDIPLUS
	else if (xflags[TEXT('P')]) {
		if (!Dcx::GDIModule.isUseable())
			throw Dcx::dcxException("dcxLoadIcon: Invalid +P without GDI+.");

		const auto p_Img = std::make_unique<Gdiplus::Bitmap>(filename.to_chr());

		// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
		auto status = p_Img->GetLastStatus();
		if (status != Gdiplus::Ok)
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

		status = p_Img->GetHICON(&icon); // for reasons unknown this causes a `first chance exception` to show in debug log.
		if (status != Gdiplus::Ok)
			throw Dcx::dcxException(TEXT("dcxLoadIcon: %"), GetLastStatusStr(status));

		GdiFlush();
	}
#else
	else if (xflags[TEXT('P')]) {
		throw Dcx::dcxException("dcxLoadIcon: Invalid +P without GDI+.");
	}
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

//HBITMAP dcxLoadBitmap(HBITMAP dest, TString &filename) {
//	filename.trim();
//
//	if (dest != NULL) {
//		DeleteBitmap(dest);
//		dest = NULL;
//	}
//	if (!IsFile(filename)) {
//		Dcx::errorex(TEXT("dcxLoadBitmap"), TEXT("Could Not Access File: %s"), filename.to_chr());
//		return NULL;
//	}
//
//#ifdef DCX_USE_GDIPLUS
//	if (Dcx::GDIModule.isUseable()) {
//		try {
//#if UNICODE
//			Bitmap *p_Img = new Bitmap(filename.to_chr());
//#else
//			Bitmap *p_Img = new Bitmap(filename.to_wchr());
//#endif
//			// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
//			Status status = p_Img->GetLastStatus();
//			if (status != Ok)
//				Dcx::error(TEXT("dcxLoadBitmap"), GetLastStatusStr(status));
//			else {
//				if ((status = p_Img->GetHBITMAP(Color(), &dest)) != Ok) {
//					Dcx::error(TEXT("dcxLoadBitmap"), TEXT("Unable to Get GDI+ Bitmap Info"));
//					Dcx::error(TEXT("dcxLoadBitmap"), GetLastStatusStr(status));
//					dest = NULL;
//				}
//				//if (dest != NULL) {
//				//	dest = GDIPlusToBitmap(p_Img);
//				//}
//				GdiFlush();
//			}
//			delete p_Img;
//		}
//		catch (const std::bad_alloc)
//		{
//			Dcx::error(TEXT("dcxLoadBitmap"), TEXT("Unable to Allocate Image Object"));
//		}
//	}
//	else
//		dest = (HBITMAP) LoadImage(GetModuleHandle(NULL), filename.to_chr(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
//#else
//	dest = (HBITMAP) LoadImage(GetModuleHandle(NULL), filename.to_chr(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
//#endif
//	return dest;
//}

HBITMAP dcxLoadBitmap(HBITMAP dest, TString &filename) {
	filename.trim();

	if (dest != nullptr) {
		DeleteBitmap(dest);
		dest = nullptr;
	}
	if (!IsFile(filename)) {
		//Dcx::errorex(TEXT("dcxLoadBitmap"), TEXT("Could Not Access File: %s"), filename.to_chr());
		Dcx::error(TEXT("dcxLoadBitmap"), TEXT("Could Not Access File: %"), filename);
		return nullptr;
	}

#ifdef DCX_USE_GDIPLUS
	if (Dcx::GDIModule.isUseable()) {
		const auto p_Img = std::make_unique<Gdiplus::Bitmap>(filename.to_wchr());

		// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
		auto status = p_Img->GetLastStatus();
		if (status != Gdiplus::Status::Ok)
			Dcx::error(TEXT("dcxLoadBitmap"), GetLastStatusStr(status));
		else {
			status = p_Img->GetHBITMAP(Gdiplus::Color(), &dest);
			if (status != Gdiplus::Status::Ok) {
				Dcx::error(TEXT("dcxLoadBitmap"), TEXT("Unable to Get GDI+ Bitmap Info"));
				Dcx::error(TEXT("dcxLoadBitmap"), GetLastStatusStr(status));
				dest = nullptr;
			}
			//if (dest != NULL) {
			//	dest = GDIPlusToBitmap(p_Img);
			//}
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

COLORREF defaultGrayPalette[256];
bool bGrayPaletteSet = false;

HICON CreateGrayscaleIcon( HICON hIcon, COLORREF* pPalette )
{
	if (hIcon == nullptr)
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

				auto c1 = (DWORD)(sz.cx * sz.cy);

				auto lpBits = (LPDWORD)::GlobalAlloc(GMEM_FIXED, (c1) * 4);

				if (lpBits && ::GetDIBits(hdc, icInfo.hbmColor, 0, (UINT)sz.cy, lpBits, &bmpInfo, DIB_RGB_COLORS) != 0)
				{
					auto lpBitsPtr     = (LPBYTE)lpBits;

					for (auto i = decltype(c1){0}; i < c1; i++)
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

						::SetDIBits(hdc, icGrayInfo.hbmColor, 0, (UINT)sz.cy, lpBits, &bmpInfo, DIB_RGB_COLORS);

						icGrayInfo.hbmMask = icInfo.hbmMask;
						icGrayInfo.fIcon   = TRUE;

						hGrayIcon = ::CreateIconIndirect(&icGrayInfo);
					}

					::GlobalFree(lpBits);
					//lpBits = NULL;
				}
			}

			if (icInfo.hbmMask != nullptr)
				::DeleteObject(icInfo.hbmMask);
		}
	}
	return hGrayIcon;
}

HICON CreateGrayscaleIcon( HICON hIcon )
{
	if (hIcon == nullptr)
		return nullptr;

	if (!bGrayPaletteSet)
	{
		//#pragma loop(hint_parallel(2))
		for(auto i = 0U; i < Dcx::countof(defaultGrayPalette); i++)
		{
			defaultGrayPalette[i] = RGB(255-i, 255-i, 255-i);
		}

		bGrayPaletteSet = true;
	}

	return CreateGrayscaleIcon(hIcon, &defaultGrayPalette[0]);
}

void AddToolTipToolInfo(const HWND tiphwnd, const HWND ctrl)
{
	TOOLINFO ti;
	ZeroMemory(&ti, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);
	ti.hwnd = ctrl;
	ti.lpszText = LPSTR_TEXTCALLBACK;
	ti.uFlags = TTF_IDISHWND | TTF_TRANSPARENT | TTF_SUBCLASS | TTF_PARSELINKS;
	//ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_PARSELINKS;
	ti.uId = (UINT_PTR)ctrl;

	SendMessage(tiphwnd, TTM_ADDTOOL, NULL, (LPARAM)&ti);
}

void dcxDrawShadowText(HDC hdc, LPCWSTR pszText, UINT cch, RECT *pRect, DWORD dwFlags, COLORREF crText, COLORREF crShadow, int ixOffset, int iyOffset)
{
	if ((hdc == nullptr) || (pszText == nullptr) || (pRect == nullptr))
		return;

	if (DrawShadowText(hdc, pszText, cch, pRect, dwFlags, crText, crShadow, ixOffset, iyOffset) == 0)
	{
		if (dcx_testflag(dwFlags,DT_CALCRECT)) {
			DrawTextW(hdc, pszText, (int)cch, pRect, dwFlags);
			OffsetRect(pRect, ixOffset, iyOffset);
		}
		else {
			// Normal DrawShadow Failed...
			RECT rcOffset;
			CopyRect(&rcOffset, pRect);
			OffsetRect(&rcOffset, ixOffset, iyOffset);
			auto old_clr = SetTextColor(hdc, crShadow);
			DrawTextW(hdc, pszText, (int)cch, &rcOffset, dwFlags);
			SetTextColor(hdc, crText);
			DrawTextW(hdc, pszText, (int)cch, (LPRECT)pRect, dwFlags);
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
const TCHAR *GetLastStatusStr(Gdiplus::Status status)
{
	//if (status > DCX_MAX_GDI_ERRORS)
	//	return GDIErrors[1]; // status not in table, return GenericError
	//return GDIErrors[(UINT)status];

	if (static_cast<UINT>(status) >= Dcx::countof(GDIErrors))
		return GDIErrors[1]; // status not in table, return GenericError
	return GDIErrors[static_cast<UINT>(status)];
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
	auto res = SearchPath(nullptr, filename.to_chr(), nullptr, 0, nullptr, nullptr);
	if (res > 0) {
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
		if (hFile != INVALID_HANDLE_VALUE) {
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

/*!
* \brief Get mIRC Colours.
*
* blah
*/
COLORREF staticPalette[mIRC_PALETTE_SIZE] = { CLR_INVALID };
void getmIRCPalette()
{
	if (staticPalette[0] == CLR_INVALID)
	{
		TString colors;
		static const TCHAR com[] = TEXT("$color(0) $color(1) $color(2) $color(3) $color(4) $color(5) $color(6) $color(7) $color(8) $color(9) $color(10) $color(11) $color(12) $color(13) $color(14) $color(15)");
		//mIRCLinker::tsEval(colors, &com[0]);
		mIRCLinker::eval(colors, &com[0]);

		size_t i = 0;
		for (const auto &a : colors)
			staticPalette[i++] = a.to_<COLORREF>();
	}
}

void getmIRCPalette(COLORREF *const Palette, const UINT PaletteItems)
{
	if (PaletteItems > mIRC_PALETTE_SIZE)
		return;

	if (!Dcx::setting_bStaticColours)
		staticPalette[0] = CLR_INVALID;

	getmIRCPalette();

	CopyMemory(Palette, &staticPalette[0], sizeof(COLORREF) * PaletteItems);
}

int unfoldColor(const WCHAR *color)
{
	auto nColor = _ts_atoi(color);

	while (nColor > 15) {
		nColor -= 16;
	}

	return nColor;
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

void mIRC_OutText(HDC hdc, TString &txt, LPRECT rcOut, const LPLOGFONT lf, const UINT iStyle, const COLORREF clrFG, const bool shadow)
{
	if (txt.empty())
		return;

	const auto len = txt.len();
	TEXTMETRICW tm = { 0 };
	auto hOldFont = SelectFont(hdc, CreateFontIndirect(lf));
	GetTextMetrics(hdc, &tm);
	auto rcTmp = *rcOut;
	if (!dcx_testflag(iStyle,DT_CALCRECT)) {	// if DT_CALCRECT flag NOT given then do calcrect here.
		//DrawText(hdc, txt.to_chr(), len, &rcTmp, iStyle | DT_CALCRECT);
		if (shadow)
			dcxDrawShadowText(hdc,txt.to_chr(), len, &rcTmp, iStyle | DT_CALCRECT, clrFG, 0, 5, 5);
		else
			DrawText(hdc, txt.to_chr(), static_cast<int>(len), &rcTmp, iStyle | DT_CALCRECT);
	}
	if (shadow)
		dcxDrawShadowText(hdc,txt.to_chr(), len, &rcTmp, iStyle, clrFG, 0, 5, 5);
	else
		DrawText(hdc, txt.to_chr(), static_cast<int>(len), &rcTmp, iStyle);
	rcOut->left += (rcTmp.right - rcTmp.left) - tm.tmOverhang;
	DeleteFont(SelectFont( hdc, hOldFont ));
	txt.clear();	// txt = TEXT("");
}

void mIRC_DrawText(HDC hdc, const TString &txt, LPRECT rc, const UINT style, const bool shadow)
{
	const auto wtxt = txt.to_wchr();
	/*int savedDC;*/
	const auto len = txt.len();
	auto rcOut = *rc;
	UINT pos = 0, iStyle = (style & ~(DT_CENTER|DT_RIGHT|DT_VCENTER)) | DT_LEFT; // make sure its to left
	bool /*usingBGclr = false,*/ usingRevTxt = false;

	if ((len == 0) || (wtxt == nullptr)) // if no text just exit.
		return;

	// create an hdc buffer to avoid flicker during drawing.
	auto hBuffer = CreateHDCBuffer(hdc, rc);

	if (hBuffer == nullptr)
		return;

	Auto(DeleteHDCBuffer(hBuffer));

	auto oldHDC = hdc;
	hdc = *hBuffer;
	// change rcOut to be zero offset.
	OffsetRect(&rcOut,-rcOut.left, -rcOut.top);

	//savedDC = SaveDC(hdc);

	COLORREF origFG = GetTextColor(hdc), origBG = GetBkColor(hdc);
	COLORREF clrFG = origFG, clrBG = origBG;
	COLORREF cPalette[mIRC_PALETTE_SIZE] = {CLR_INVALID}; // mIRC palette

	getmIRCPalette(&cPalette[0], Dcx::countof(cPalette)); // get mIRC palette

	auto hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);

	LOGFONT lf = { 0 };
	if (GetObject(hFont, sizeof(LOGFONT), &lf) == 0)
		return;

	// bug #721 -> http://dcx.scriptsdb.org/bug/index.php?do=details&task_id=721
	/*TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);*/
	/*DRAWTEXTPARAMS dtp = {0};
	dtp.cbSize = sizeof(DRAWTEXTPARAMS);
	dtp.iTabLength = 4;*/

	auto origWeight = lf.lfWeight;
	auto origLeft = rc->left;

	SetBkMode(hdc,TRANSPARENT);

	if (!dcx_testflag(iStyle,DT_CALCRECT)) { // if NOT doing DT_CALCRECT calc rect needed here.
		//mIRC_DrawText(hdc, txt, &rcOut, style | DT_CALCRECT, shadow);
		if (dcx_testflag(style,DT_CENTER) || dcx_testflag(style,DT_RIGHT) || dcx_testflag(style,DT_VCENTER)) {
			// strip out ctrl codes to correctly position text.
			auto rcTmp = *rc;
			auto t(txt);
			if (dcx_testflag(iStyle,DT_SINGLELINE)) {
				//t.replace(TEXT('\n'), TEXT(' '));
				//t.replace(TEXT('\r'), TEXT(' '));
				t.mreplace(TEXT(' '), TEXT("\n\r"));
			}
			DrawText(hdc, t.strip().to_chr(), -1, &rcTmp, style | DT_CALCRECT /*| DT_NOPREFIX*/);	// using t.len() gives invalid length (issue with strip())
			//DrawText(hdc, t.strip().to_chr(), t.len(), &rcTmp, style | DT_CALCRECT /*| DT_NOPREFIX*/);
			//DrawText(hdc, txt, lstrlen(txt), &rcTmp, style | DT_CALCRECT /*| DT_NOPREFIX*/);
			//DrawTextEx(hdc, t.strip().to_chr(), t.len(), &rcTmp, style | DT_CALCRECT /*| DT_NOPREFIX*/, &dtp);
			// style can be either center or right, not both, but it can be center+vcenter or right+vcenter
			if (dcx_testflag(style,DT_CENTER)) { // get center text start offset
				// (total width) - (required width) / 2 = equal space to each side.
				origLeft = rcOut.left = (((rcOut.right - rcOut.left) - (rcTmp.right - rcTmp.left)) / 2);
			}
			else if (dcx_testflag(style,DT_RIGHT)) { // get right text start offset
				// adjust start to right
				origLeft = rcOut.left = (rcOut.right - (rcTmp.right - rcTmp.left));
				//origLeft = rcOut.left = (rcOut.right - (rcTmp.right - rcTmp.left) - 3);
				//origLeft = rcOut.left = (rcOut.right - (rcTmp.right - rcTmp.left) - tm.tmMaxCharWidth);
				//origLeft = rcOut.left = (rcOut.right - (rcTmp.right - rcTmp.left) - tm.tmAveCharWidth);
				//origLeft = rcOut.left = rcTmp.left;
			}
			if (dcx_testflag(style,DT_VCENTER)) { // get Veritcal center text start offset
				rcOut.top += (((rcOut.bottom - rcOut.top) - (rcTmp.bottom - rcTmp.top)) / 2);
			}
		}

	}

	TString tmp;

	for (auto c = wtxt[pos]; pos < len; c = wtxt[++pos]) {
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
				while (wtxt[pos+1] == 3) pos++; // remove multiple consecutive ctrl-k's
				SetBkMode(hdc,TRANSPARENT);
				//usingBGclr = false;
				if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9') {
					WCHAR colbuf[3];
					colbuf[0] = wtxt[pos +1];
					colbuf[1] = L'\0';
					colbuf[2] = L'\0';
					++pos;

					if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9') {
						colbuf[1] = wtxt[pos +1];
						pos++;
					}

					// color code number
					clrFG = cPalette[unfoldColor(&colbuf[0])];
					//clrFG = cPalette[unfoldColor<WCHAR>(colbuf)];

					// maybe a background color
					if (wtxt[pos+1] == L',') {
						++pos;

						if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9') {
							colbuf[0] = wtxt[pos +1];
							pos++;

							if (wtxt[pos +1] >= L'0' && wtxt[pos +1] <= L'9') {
								colbuf[1] = wtxt[pos +1];
								++pos;
							}
							else
								colbuf[1] = L'\0';


							// color code number
							clrBG = cPalette[unfoldColor(&colbuf[0])];
							//clrBG = cPalette[unfoldColor<WCHAR>(colbuf)];
							SetBkMode(hdc, OPAQUE);
							//usingBGclr = true;
						}
					}
					if (usingRevTxt) { // reverse text swap fg & bg colours
						const auto ct = clrFG;
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
				if (!tmp.empty())
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
				if (!tmp.empty())
					mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
				usingRevTxt = (usingRevTxt ? false : true);
				if (usingRevTxt)
					SetBkMode(hdc,OPAQUE);
				else
					SetBkMode(hdc,TRANSPARENT);
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
				lf.lfItalic = (BYTE)(lf.lfItalic ? 0 : 1);
			}
			break;
		case 31: // ctrl+u
			{
				if (!tmp.empty())
					mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
				lf.lfUnderline = (BYTE)(lf.lfUnderline ? 0 : 1);
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
					const auto tlen = tmp.len();
					if (tlen > 0)
					{
						SIZE sz = { 0 };
						GetTextExtentPoint32(hdc, tmp.to_chr(), static_cast<int>(tlen), &sz);
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
					if (!tmp.empty()) {
						SIZE sz = { 0 };
						int nFit = 0;
						const auto tlen = static_cast<int>(tmp.len());
						GetTextExtentExPoint(hdc, txt.to_chr(), tlen, (rcOut.right - rcOut.left), &nFit, NULL, &sz);
						if (nFit < tlen) {
							if (nFit > 0) {
								auto o = tmp[nFit];
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
	if (!tmp.empty())
		mIRC_OutText(hdc, tmp, &rcOut, &lf, iStyle, clrFG, shadow);
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

gsl::owner<HDC *> CreateHDCBuffer(gsl::not_null<HDC> hdc, const LPRECT rc)
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

	if (rc == nullptr) {
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

	if (buf->m_hBitmap == nullptr) {
#ifdef DEBUG
		DWORD err = GetLastError(), errBufSize = 16;
		LPTSTR errBuf = nullptr;
		if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, err, 0, (LPTSTR)&errBuf, errBufSize, nullptr) != 0) {
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

void DeleteHDCBuffer(gsl::owner<HDC *> hBuffer)
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

//int TGetWindowText(HWND hwnd, TString &txt)
//{
//	const int nText = GetWindowTextLength(hwnd);
//	if (nText > 0) {
//		PTCHAR text = new TCHAR[nText + 2];
//		GetWindowText(hwnd, text, nText + 1);	// NB: needs to include space for end 0
//		txt = text;
//		delete[] text;
//	}
//	else
//		txt.clear();	// txt = TEXT("");
//	return nText;
//}

int TGetWindowText(HWND hwnd, TString &txt)
{
	if (hwnd == nullptr)
		return 0;

	const auto nText = GetWindowTextLength(hwnd) + 2;	// NB: needs to include space for end 0
	if (nText > 2) {
		txt.reserve((UINT)(nText));
		if (GetWindowText(hwnd, txt.to_chr(), nText) != 0)
			return nText;

		//auto text = std::make_unique<TCHAR[]>(nText + 2);
		//if (GetWindowText(hwnd, text.get(), nText + 1) != 0) {	// NB: needs to include space for end 0
		//	txt = text.get();
		//	return nText;
		//}
	}
	txt.clear();	// txt = TEXT("");
	return 0;
}

HMODULE UXModule = nullptr;         //!< UxTheme.dll Module Handle
#ifdef DCX_USE_GDIPLUS
HMODULE GDIPlusModule = nullptr;	//!< gdiplus.dll Module Handle
ULONG_PTR gdi_token = NULL;
#endif
HMODULE DWMModule = nullptr;		//!< dwmapi.dll Module Handle

void FreeOSCompatibility(void)
{
#ifdef DCX_USE_GDIPLUS
	// Shutdown GDI+
	if (GDIPlusModule != nullptr) {
		if (gdi_token != NULL)
			Gdiplus::GdiplusShutdown(gdi_token);

		FreeLibrary(GDIPlusModule);
		GDIPlusModule = nullptr;
	}
#endif

	if (UXModule != nullptr) {
		Dcx::UXModule.dcxBufferedPaintUnInit();

		FreeLibrary(UXModule);
		UXModule = nullptr;
	}
	if (DWMModule != nullptr) {
		FreeLibrary(DWMModule);
		DWMModule = nullptr;
	}
}

bool isRegexMatch(const TCHAR *matchtext, const TCHAR *pattern)
{
	if ((matchtext == nullptr) || (pattern == nullptr))
		return false;

	// NB: CREGEX version is incomplete
#ifdef DCX_USE_CREGEX
	try {
		std::basic_regex<TCHAR> r(pattern, std::regex_constants::ECMAScript);
		if (std::regex_match(matchtext, r, std::regex_constants::match_default))
			return true;
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

bool AddFileIcons(HIMAGELIST himl, TString &filename, const bool bLarge, const int iIndex, const int iStart, const int iEnd)
{
	if (himl == nullptr)
		return false;

	if (!IsFile(filename))
		return false;

#if DCX_USE_WRAPPERS
	int fIndex = iStart, i = iIndex;
	auto bAdded = false, bGotIcon = false;

	if (fIndex < 0 || (iEnd != -1 && fIndex > iEnd))
		return bAdded;

	do {
		Dcx::dcxIconResource hIcon(filename, fIndex, bLarge);

		bGotIcon = (hIcon != nullptr);
		if (bGotIcon) {
			if (i == -1)
				ImageList_ReplaceIcon(himl, -1, hIcon);	// same as ImageList_AddIcon()
			else
				ImageList_ReplaceIcon(himl, i++, hIcon);
			bAdded = true;
			fIndex++;
			if (fIndex > iEnd)
				break;
		}
	} while (bGotIcon);
#else
	int fIndex = iStart, i = iIndex;
	HICON hIcon = nullptr;
	auto bAdded = false;
	
	if (fIndex < 0 || (iEnd != -1 && fIndex > iEnd))
		return bAdded;
	
	do {
		if (bLarge)
			ExtractIconEx(filename.to_chr(), fIndex, &hIcon, nullptr, 1);
		else
			ExtractIconEx(filename.to_chr(), fIndex, nullptr, &hIcon, 1);
	
		if (hIcon != nullptr) {
			// auto free handle hIcon when going out of scope or throwing an exception
			//Auto(DestroyIcon(hIcon));
	
			// auto free handle hIcon when going out of scope or throwing an exception (testing)
			auto IconGuard = gsl::finally([hIcon]() { DestroyIcon(hIcon); });
	
			if (i == -1)
				ImageList_ReplaceIcon(himl, -1, hIcon);
			else
				ImageList_ReplaceIcon(himl, i++, hIcon);
			bAdded = true;
		}
		fIndex++;
		if (fIndex > iEnd)
			break;
	} while (hIcon != nullptr);
#endif

	return bAdded;
}

BOOL dcxGetWindowRect(const gsl::not_null<HWND> &hWnd, const gsl::not_null<LPRECT> &lpRect)
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
void DrawRotatedText(const TString &strDraw, const gsl::not_null<LPRECT> &rc, const gsl::not_null<HDC> &hDC, const int nAngleLine/* = 0*/, const bool bEnableAngleChar /*= false*/, const int nAngleChar /*= 0*/) {

	if ((nAngleLine == 0) && (!bEnableAngleChar)) {
		TextOut(hDC, rc->left, rc->bottom, strDraw.to_chr(), static_cast<int>(strDraw.len()));
		//DrawText(hDC, strDraw.to_chr(), strDraw.len(), rc, styles);
		return;
	}
	LOGFONT lf = { 0 };

	if (GetObject((HFONT)GetCurrentObject(hDC, OBJ_FONT), sizeof(LOGFONT), &lf) == 0)
		return;

	// Set the background mode to transparent for the
	// text-output operation.
	const auto nOldBkMode = SetBkMode(hDC, TRANSPARENT);
	Auto(SetBkMode(hDC, nOldBkMode));

	// Specify the angle to draw line
	lf.lfEscapement = nAngleLine*10;
	int nOldGMode = 0;
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
	auto hFont = CreateFontIndirect(&lf);
	if (hFont != nullptr) {
		Auto(DeleteFont(hFont));

		auto hPrevFont = SelectFont(hDC, hFont);
		Auto(SelectFont(hDC, hPrevFont));

		// Draw text to screen
		//TextOut(hDC, rc->right / 2, rc->bottom / 2, strDraw.to_chr(), strDraw.len());
		TextOut(hDC, rc->left, rc->bottom, strDraw.to_chr(), (int)strDraw.len());
		//DrawText(hDC, strDraw.to_chr(), strDraw.len(), rc, styles);
	}
}

//void DrawRotatedText(HDC hdc, TCHAR *str, LPRECT rect, double angle, UINT nOptions = 0)
//{
//	// convert angle to radian
//	double pi = 3.141592654;
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

const char *queryAttribute(gsl::not_null<const TiXmlElement *> element, gsl::not_null<const char *> attribute, gsl::not_null<const char *> defaultValue)
{
	const auto t = element->Attribute(attribute);
	return (t != nullptr) ? t : defaultValue;
}

int queryIntAttribute(gsl::not_null<const TiXmlElement *> element, gsl::not_null<const char *> attribute, const int defaultValue)
{
	auto integer = defaultValue;
	return (element->QueryIntAttribute(attribute, &integer) == TIXML_SUCCESS) ? integer : defaultValue;
}

/*
	TString MakeTextmIRCSafe(const TCHAR *const tString)

	Make the text safe for passing to the callback alias

	Change special characters $%()[]; into there $chr() equivalents
*/
TString MakeTextmIRCSafe(const TString &tsStr)
{
	return MakeTextmIRCSafe(tsStr.to_chr());
}

TString MakeTextmIRCSafe(const TCHAR *const tString)
{
	TString tsRes((UINT)MIRC_BUFFER_SIZE_CCH);	// use MIRC_BUFFER_SIZE_CCH as starting buffer size (shouldnt be bigger...)

	if (tString == nullptr)
		return tsRes;

	const auto len = _ts_strlen(tString);
	bool bLastWasSpace = true;	// start as true as the beginning of the line is treated the same as a space here

	// look for ()[]%$; we dont want to change , as this is needed
	for (auto i = decltype(len){0}; i < len; i++)
	{
		const auto c = tString[i];
		switch (c)
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
