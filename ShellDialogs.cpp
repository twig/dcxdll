#include "defines.h"
#include "Dcx.h"



/*
 * Structure to store settings for use in BrowseFolderCallback.
 */
struct XBROWSEDIALOGSETTINGS {
	PTCHAR initialFolder{};
	UINT flags{};
};
using LPXBROWSEDIALOGSETTINGS = XBROWSEDIALOGSETTINGS*;


/*!
* \brief DCX DLL ColorDialog Function
*
* Argument \b data contains -> (DEFAULT) [STYLES]
*/
// ColorDialog (DEFAULT) [STYLES]
mIRC(ColorDialog)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		bool			retDefault(false);
		CHOOSECOLOR		cc{};
		static COLORREF clr[16]{};	// MUST be 16 (not related to mIRC colours)
		const auto		sel = d.getfirsttok(1).to_<COLORREF>();
		DWORD			styles = CC_RGBINIT;

		// initial settings
		cc.lStructSize = sizeof(CHOOSECOLOR);
		cc.hwndOwner = mWnd;

		for (const auto& tsStyle : d)
		{
			switch (std::hash<TString>{}(tsStyle))
			{
			case TEXT("anycolor"_hash):
				styles |= CC_ANYCOLOR;
				break;
			case TEXT("fullopen"_hash):
				styles |= CC_FULLOPEN;
				break;
			case TEXT("nofullopen"_hash):
				styles |= CC_PREVENTFULLOPEN;
				break;
			case TEXT("solidonly"_hash):
				styles |= CC_SOLIDCOLOR;
				break;
			case TEXT("owner"_hash):
				cc.hwndOwner = FindOwner(d, gsl::not_null(mWnd));
				break;
			case TEXT("returndefault"_hash):
				retDefault = true;
				break;
			default:
				break;
			}
		}

		cc.rgbResult = sel;
		cc.Flags = styles;
		cc.lpCustColors = &clr[0];

		// User clicked OK
		if (ChooseColor(&cc))
		{
			_ts_snprintf(data, mIRCLinker::m_dwCharacters, TEXT("%u"), cc.rgbResult);
			return 3;
		}
		// User clicked cancel, return default color
		else if (retDefault)
		{
			_ts_snprintf(data, mIRCLinker::m_dwCharacters, TEXT("%u"), sel);
			return 3;
		}
		// User clicked cancel, dont bother with default color
		else
			ret(TEXT("-1"));
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("$!dcx(ColorDialog)"), TEXT("\"%\" error: %"), d, e.what());
		//Dcx::error(__FUNCTIONW__, TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("$!dcx(ColorDialog)"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	return 0;
}


/*!
* \brief DCX DLL OpenDialog|SaveDialog Function
*
* Argument \b data contains -> (styles) [TAB] (file) [TAB] (filter)
*
* http://www.winprog.org/tutorial/app_two.html
* http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/userinput/commondialogboxlibrary/commondialogboxreference/commondialogboxstructures/openfilename.asp
*/
// OpenDialog (styles) [TAB] (file) [TAB] (filter)
mIRC(OpenDialog)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		// count number of tab tokens
		if (d.numtok(TSTABCHAR) != 3)
			throw DcxExceptions::dcxInvalidArguments();

		ret(FileDialog(d, TEXT("OPEN"), mWnd).to_chr());
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("$!dcx(OpenDialog)"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("$!dcx(OpenDialog)"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	return 0;
}

// SaveDialog (styles) [TAB] (file) [TAB] (filter)
mIRC(SaveDialog)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		// count number of tab tokens
		if (d.numtok(TSTABCHAR) != 3)
			throw DcxExceptions::dcxInvalidArguments();

		ret(FileDialog(d, TEXT("SAVE"), mWnd).to_chr());
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("$!dcx(SaveDialog)"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("$!dcx(SaveDialog)"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	return 0;
}

/*!
* \brief Shows CommonDialog for Open/Save
*
* Shows and returns the file selected
*
* \return > TString "" if cancelled
*         > TString Path+Filename
*/
TString FileDialog(const TString& data, const TString& method, const HWND pWnd)
{
	//TCHAR szFilename[MIRC_BUFFER_SIZE_CCH];

	//stString<MIRC_BUFFER_SIZE_CCH> szFilename;

	//gsl::wzstring<MIRC_BUFFER_SIZE_CCH> szFilename(L"");

	//Dcx::dcxStringResource szFilename(MIRC_BUFFER_SIZE_CCH);

	const auto szFilename = std::make_unique<TCHAR[]>(mIRCLinker::m_dwCharacters);

	// seperate the tokenz
	const auto styles(data.getfirsttok(1, TSTABCHAR).trim());
	const auto file(data.getnexttok(TSTABCHAR).trim());
	auto filter(data.getnexttok(TSTABCHAR).trim());

	// Get Current dir for resetting later.
	const UINT tsBufSize = GetCurrentDirectory(0, nullptr);
	TString tsCurrentDir(tsBufSize);
	GetCurrentDirectory(tsBufSize, tsCurrentDir.to_chr());

	// format the filter into the format WinAPI wants, with double zero TERMINATOR at end
	if (filter.empty())
		filter = TEXT("All Files (*.*)|*.*");

	filter += TEXT('|'); // <-- changed to a zero below for double null ending.
	//filter.replace(TEXT('|'), TEXT('\0'));	// Ook: issue with new replace function not handling zero chars
	filter.mreplace(TEXT('\0'), TEXT("|"));

	// set up the OFN struct
	OPENFILENAME ofn{};
	dcx_strcpyn(szFilename.get(), file.to_chr(), mIRCLinker::m_dwCharacters);
	//szFilename = file.to_chr();

	ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
	ofn.hwndOwner = pWnd;
	ofn.lpstrFilter = filter.to_chr();
	//ofn.lpstrFile = szFilename;
	ofn.lpstrFile = szFilename.get();
	ofn.nMaxFile = mIRCLinker::m_dwCharacters;
	ofn.lpstrDefExt = TEXT("");

	DWORD style = OFN_EXPLORER;

	for (const auto& tsStyle : styles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case TEXT("multisel"_hash):
			style |= OFN_ALLOWMULTISELECT;
			break;
		case TEXT("createprompt"_hash):
			style |= OFN_CREATEPROMPT;
			break;
		case TEXT("enablesizing"_hash):
			// FIXME: explorer style resizable on default, cant get rid of that shit
			style |= OFN_ENABLESIZING;
			break;
		case TEXT("filemustexist"_hash):
			style |= OFN_FILEMUSTEXIST; // (open)
			break;
		case TEXT("showhidden"_hash):
			style |= OFN_FORCESHOWHIDDEN; // 2k/xp
			break;
		case TEXT("noreadonly"_hash):
			style |= OFN_HIDEREADONLY;
			break;
		case TEXT("nochangedir"_hash):
			style |= OFN_NOCHANGEDIR; // (save)
			break;
		case TEXT("getshortcuts"_hash):
			style |= OFN_NODEREFERENCELINKS;
			break;
		case TEXT("nonetwork"_hash):
			style |= OFN_NONETWORKBUTTON;
			break;
		case TEXT("novalidate"_hash):
			style |= OFN_NOVALIDATE;
			break;
		case TEXT("norecent"_hash):
			style |= OFN_DONTADDTORECENT; // 2k/xp
			break;
		case TEXT("overwriteprompt"_hash):
			style |= OFN_OVERWRITEPROMPT; // save
			break;
		case TEXT("pathmustexist"_hash):
			style |= OFN_PATHMUSTEXIST;
			break;
		case TEXT("owner"_hash):
			ofn.hwndOwner = FindOwner(styles, gsl::not_null(pWnd));
			//ofn.hwndOwner = FindOwner(styles).value_or(pWnd);
			break;
		default:
			break;
		}
	}

	ofn.Flags = style;

	TString tsResult;

	if (method == TEXT("OPEN") && GetOpenFileName(&ofn))
	{
		// if there are multiple files selected
		if (dcx_testflag(style, OFN_ALLOWMULTISELECT))
		{
			//const TCHAR *p = szFilename;
			const TCHAR* p = szFilename.get();

			// process the file name at p since its null terminated
			while (*p != TEXT('\0'))
			{
				tsResult.addtok(p, TEXT('|'));
				p += _ts_strlen(p) + 1;
			}
		}
		// copy the string directly
		else
			tsResult = szFilename.get();
	}
	else if (method == TEXT("SAVE") && GetSaveFileName(&ofn))
		tsResult = szFilename.get();

	// Reset current dir.
	if (!tsCurrentDir.empty())
		SetCurrentDirectory(tsCurrentDir.to_chr());

	return tsResult;
}



/*!
* \brief Shows CommonDialog for Selecting Folders/Directories
*
* Shows and returns the folder selected
*
* \return > TString "" if cancelled
*         > TString [SELECTED_ITEM]
*/
mIRC(BrowseDialog)
{
	TString input(data);

	data[0] = 0;

	try {
		input.trim();

		// seperate the tokens (by tabs)
		int currentParam = 1;
		bool bInitialFolder = false;
		bool bDialogText = false;

		TString initPath(UMAX_PATH);
		TString displayPath(UMAX_PATH);
		gsl::owner<LPITEMIDLIST> pidlRoot = nullptr;

		// set up the BI structure
		XBROWSEDIALOGSETTINGS extra{};
		BROWSEINFO bi{};
		bi.hwndOwner = mWnd;                                             // Default owner: mIRC main window
		bi.lpfn = BrowseFolderCallback;
		bi.pszDisplayName = displayPath.to_chr();
		bi.ulFlags = BIF_VALIDATE;
		bi.lParam = reinterpret_cast<LPARAM>(std::addressof(extra));

		// Parse styles
		auto param(input.gettok(currentParam, TSTABCHAR));

		for (const auto& flag : param)
		{
			/*
			style1 style2 style3 $chr(9) initial folder

			//clear | echo -a Selected: $dcx(BrowseDialog, hello asd $chr(9) C:\Windows $chr(9) mr t says select something fool! ) | /udcx

			http://msdn2.microsoft.com/en-us/library/bb773205.aspx
			*/

			switch (std::hash<TString>{}(flag))
			{
			case TEXT("advanced"_hash):
				bi.ulFlags |= BIF_USENEWUI;
				break;
			case TEXT("edit"_hash):
				bi.ulFlags |= BIF_EDITBOX;
				break;
			case TEXT("newstyle"_hash):
				bi.ulFlags |= BIF_NEWDIALOGSTYLE;
				break;
			case TEXT("nonew"_hash):
				bi.ulFlags |= BIF_NONEWFOLDERBUTTON;
				break;
			case TEXT("files"_hash):
				bi.ulFlags |= BIF_BROWSEINCLUDEFILES;
				break;
			case TEXT("title"_hash):
				bDialogText = true;
				break;
			case TEXT("initfolder"_hash):
				bInitialFolder = true;
				break;
			case TEXT("computers"_hash):
			{
				// NOTE: do not use with "advanced"
				//bi.ulFlags &= ~BIF_USENEWUI;
				bi.ulFlags |= BIF_BROWSEFORCOMPUTER;
				pidlRoot = GetFolderFromCSIDL(CSIDL_NETWORK);
			}
			break;
			case TEXT("printers"_hash):
			{
				// NOTE: do not use with "advanced"
				//bi.ulFlags &= ~BIF_USENEWUI;
				bi.ulFlags |= BIF_BROWSEFORPRINTER;
				pidlRoot = GetFolderFromCSIDL(CSIDL_PRINTERS);
			}
			break;
			case TEXT("nonetwork"_hash):
				bi.ulFlags |= BIF_DONTGOBELOWDOMAIN;
				break;
			case TEXT("shortcut"_hash):
				bi.ulFlags |= BIF_NOTRANSLATETARGETS;
				break;
				// owner
			case TEXT("owner"_hash):
				bi.hwndOwner = FindOwner(param, gsl::not_null(mWnd));
				break;
			default:
				break;
			}
		}

		Auto(CoTaskMemFree(pidlRoot));

		// Set initial folder
		if (bInitialFolder && (!pidlRoot))
		{
			++currentParam;
			initPath = input.gettok(currentParam, TSTABCHAR).trim();

			extra.initialFolder = initPath.to_chr();
		}

		// Set title text.
		if (bDialogText)
		{
			++currentParam;
			param = input.gettok(currentParam, TSTABCHAR).trim();

			bi.lpszTitle = param.to_chr();
		}

		// Set root folder
		if (pidlRoot)
			bi.pidlRoot = pidlRoot;

		extra.flags = bi.ulFlags;

		const auto pidl = SHBrowseForFolder(&bi);

		// User cancelled
		if (!pidl)
			return 3;

		Auto(CoTaskMemFree(pidl));

		// If we were searching for a computer ...
		if (dcx_testflag(bi.ulFlags, BIF_BROWSEFORCOMPUTER))
			_ts_snprintf(data, mIRCLinker::m_dwCharacters, TEXT("//%s"), displayPath.to_chr());
		else {
			SHGetPathFromIDList(pidl, initPath.to_chr());
			dcx_strcpyn(data, initPath.to_chr(), mIRCLinker::m_dwCharacters);
		}

		return 3;
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("$!dcx(BrowseDialog)"), TEXT("\"%\" error: %"), input, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("$!dcx(BrowseDialog)"), TEXT("\"%\" error: Unknown Exception"), input);
	}
	return 0;
}

/*
// Some useful stuff for resolving CSIDL information.
// http://www.codeguru.com/cpp/w-p/files/browserfunctionsdialogs/article.php/c4443/

// List of CSIDL ids
// http://msdn2.microsoft.com/en-us/library/bb762494(VS.85).aspx

// TODO: make this accept CSIDL stuff as initial folder.
*/
gsl::owner<LPITEMIDLIST> GetFolderFromCSIDL(const int nCsidl) noexcept
{
	if (LPITEMIDLIST pidlRoot{ nullptr }; S_OK == SHGetFolderLocation(nullptr, nCsidl, nullptr, 0, &pidlRoot))
		return pidlRoot;

	return nullptr;                // Caller assumes responsibility
}

int CALLBACK BrowseFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) noexcept
{
	switch (const auto extra = reinterpret_cast<LPXBROWSEDIALOGSETTINGS>(lpData); uMsg)
	{
		// User typed invalid name (non-existant folder) into editbox.
		// This must return non-zero, otherwise it will close the dialog.
	case BFFM_VALIDATEFAILED:
		return TRUE;

	case BFFM_INITIALIZED:
		// Sets initial folder if it is specified.
		if (extra)
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, reinterpret_cast<LPARAM>(extra->initialFolder));
		else
			// Disable OK button.
			SendMessage(hwnd, BFFM_ENABLEOK, TRUE, FALSE);

		break;

	case BFFM_SELCHANGED:
	{
		// Dont check for COMPUTER or PRINTER browsing
		if (dcx_testflag(extra->flags, BIF_BROWSEFORCOMPUTER) || dcx_testflag(extra->flags, BIF_BROWSEFORPRINTER))
			break;

		//TString path((UINT) MAX_PATH);
		//
		//if (SHGetPathFromIDList(reinterpret_cast<LPITEMIDLIST>(lParam), path.to_chr()))
		//{
		//	SendMessage(hwnd, BFFM_ENABLEOK, TRUE, TRUE);
		//	SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM) path.to_chr());
		//}
		//else {
		//	SendMessage(hwnd, BFFM_ENABLEOK, TRUE, FALSE);
		//	SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, 0);
		//}

		const stString<MAX_PATH> sPath;

		if (SHGetPathFromIDList(reinterpret_cast<LPITEMIDLIST>(lParam), sPath))
		{
			SendMessage(hwnd, BFFM_ENABLEOK, TRUE, TRUE);
			SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, reinterpret_cast<LPARAM>(sPath.data()));
		}
		else {
			SendMessage(hwnd, BFFM_ENABLEOK, TRUE, FALSE);
			SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, 0);
		}
		break;
	}

	default:
		break;
	}

	// Return 0 by default.
	return 0L;
}

/*!
* \brief Shows CommonDialog for Selecting Fonts
*
* Shows and returns the file selected
*
* \return > "" if cancelled
*         > +flags charset size fontname
*/
mIRC(FontDialog)
{
	// seperate the tokens (by tabs)
	TString input(data);

	data[0] = TEXT('\0');

	try {
		input.trim();

		DWORD style = CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST | CF_LIMITSIZE;
		LOGFONT lf{};

		// set up the LF structure
		if (GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf) == 0)
			throw Dcx::dcxException("Unable to get LOGFONT");

		// set up the CF struct
		CHOOSEFONT cf{};
		cf.lStructSize = sizeof(CHOOSEFONT);
		cf.hwndOwner = mWnd;
		cf.lpLogFont = &lf;
		cf.nSizeMin = 8;
		cf.nSizeMax = 72;

		{
			const auto itEnd = input.end();

			for (auto itStart = input.begin(TSTABCHAR); itStart != itEnd; ++itStart)
			{
				const auto option(*itStart);
				const auto numtok = option.numtok();
				//const auto hashType = std::hash<TString>()(option.getfirsttok(1));	// tok 1

				/*
				default +flags(ibsua) charset size fontname
				flags +etc
				color rgb
				minmaxsize min max (Ranges from 8 to 72, if "D" is used 5 to 30)
				owner hwnd

				+flag to use mirc colors
				palette col...16...col

				//clear | echo -a $dcx(FontDialog, hello asd $chr(9) flags +abcdef $chr(9) color 255 $chr(9) owner dcxtest_1146984371 $chr(9) default + default 10 Verdana) | /udcx

				http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/userinput/commondialogboxlibrary/commondialogboxreference/commondialogboxstructures/choosefont.asp
				*/

				// flags +
				switch (std::hash<TString>{}(option.getfirsttok(1)))
				{
				case TEXT("flags"_hash):
				{
					if (numtok < 2)
						//throw Dcx::dcxException("Invalid Args");
						throw DcxExceptions::dcxInvalidArguments();

					const XSwitchFlags xflags(option.getnexttok());	// tok 2

					if (xflags[TEXT('a')])
						style |= CF_NOFACESEL;
					if (xflags[TEXT('b')])
						style |= CF_SCRIPTSONLY;
					if (xflags[TEXT('c')])
						style |= CF_SCALABLEONLY;// (Scalable fonts include vector fonts, scalable printer fonts, TrueType fonts, and fonts scaled by other technologies.)
					if (xflags[TEXT('e')])
						style |= CF_EFFECTS;
					if (xflags[TEXT('f')])
						style |= CF_FORCEFONTEXIST;
					if (xflags[TEXT('h')])
						style |= CF_NOSCRIPTSEL;
					if (xflags[TEXT('i')])
						style |= CF_NOSIMULATIONS;
					if (xflags[TEXT('m')])
						style |= CF_SELECTSCRIPT;
					if (xflags[TEXT('n')])
						style |= CF_PRINTERFONTS;
					if (xflags[TEXT('p')])
						style |= CF_FIXEDPITCHONLY;
					if (xflags[TEXT('r')])
						style |= CF_NOVERTFONTS;
					if (xflags[TEXT('s')])
						style |= CF_SCREENFONTS;
					if (xflags[TEXT('t')])
						style |= CF_TTONLY;
					if (xflags[TEXT('v')])
						style |= CF_NOVECTORFONTS;
					if (xflags[TEXT('w')])
						style |= CF_WYSIWYG;
					if (xflags[TEXT('y')])
						style |= CF_NOSTYLESEL;
					if (xflags[TEXT('z')])
						style |= CF_NOSIZESEL;
				}
				break;
				case TEXT("default"_hash):
				{
					if (numtok < 5)
						//throw Dcx::dcxException("Invalid Args");
						throw DcxExceptions::dcxInvalidArguments();

					ParseCommandToLogfont(option.getlasttoks(), &lf);	// tok 2, -1
				}
				break;
				case TEXT("color"_hash):
				{
					if (numtok < 2)
						//throw Dcx::dcxException("Invalid Args");
						throw DcxExceptions::dcxInvalidArguments();

					cf.rgbColors = option.getnexttok().to_<COLORREF>();	// tok 2
				}
				break;
				case TEXT("minmaxsize"_hash):
				{
					if (numtok < 3)
						//throw Dcx::dcxException("Invalid Args");
						throw DcxExceptions::dcxInvalidArguments();

					cf.nSizeMin = option.getnexttok().to_int();	// tok 2
					cf.nSizeMax = option.getnexttok().to_int();	// tok 3
				}
				break;
				case TEXT("owner"_hash):
				{
					if (numtok < 2)
						//throw Dcx::dcxException("Invalid Args");
						throw DcxExceptions::dcxInvalidArguments();

					cf.hwndOwner = FindOwner(option, gsl::not_null(mWnd));
				}
				break;
				default:
					break;
				}

				//if (hashType == TEXT("flags"_hash) && numtok > 1)
				//{
				//	const XSwitchFlags xflags(option.getnexttok());	// tok 2
				//
				//	if (xflags[TEXT('a')])
				//		style |= CF_NOFACESEL;
				//	if (xflags[TEXT('b')])
				//		style |= CF_SCRIPTSONLY;
				//	if (xflags[TEXT('c')])
				//		style |= CF_SCALABLEONLY;// (Scalable fonts include vector fonts, scalable printer fonts, TrueType fonts, and fonts scaled by other technologies.)
				//	if (xflags[TEXT('e')])
				//		style |= CF_EFFECTS;
				//	if (xflags[TEXT('f')])
				//		style |= CF_FORCEFONTEXIST;
				//	if (xflags[TEXT('h')])
				//		style |= CF_NOSCRIPTSEL;
				//	if (xflags[TEXT('i')])
				//		style |= CF_NOSIMULATIONS;
				//	if (xflags[TEXT('m')])
				//		style |= CF_SELECTSCRIPT;
				//	if (xflags[TEXT('n')])
				//		style |= CF_PRINTERFONTS;
				//	if (xflags[TEXT('p')])
				//		style |= CF_FIXEDPITCHONLY;
				//	if (xflags[TEXT('r')])
				//		style |= CF_NOVERTFONTS;
				//	if (xflags[TEXT('s')])
				//		style |= CF_SCREENFONTS;
				//	if (xflags[TEXT('t')])
				//		style |= CF_TTONLY;
				//	if (xflags[TEXT('v')])
				//		style |= CF_NOVECTORFONTS;
				//	if (xflags[TEXT('w')])
				//		style |= CF_WYSIWYG;
				//	if (xflags[TEXT('y')])
				//		style |= CF_NOSTYLESEL;
				//	if (xflags[TEXT('z')])
				//		style |= CF_NOSIZESEL;
				//}
				//// defaults +flags(ibsua) charset size fontname
				//else if (hashType == TEXT("default"_hash) && numtok > 4)
				//	ParseCommandToLogfont(option.getlasttoks(), &lf);	// tok 2, -1
				//// color rgb
				//else if (hashType == TEXT("color"_hash) && numtok > 1)
				//	cf.rgbColors = option.getnexttok().to_<COLORREF>();	// tok 2
				//// minmaxsize min max
				//else if (hashType == TEXT("minmaxsize"_hash) && numtok > 2)
				//{
				//	cf.nSizeMin = option.getnexttok().to_int();	// tok 2
				//	cf.nSizeMax = option.getnexttok().to_int();	// tok 3
				//}
				//// owner
				//else if (hashType == TEXT("owner"_hash) && numtok > 1)
				//	cf.hwndOwner = FindOwner(option, mWnd);
			}
		}

		// check that at least some fonts are showing
		if (!dcx_testflag(style, CF_PRINTERFONTS) && !dcx_testflag(style, CF_SCREENFONTS))
			style |= CF_SCREENFONTS;

		cf.Flags = style;
		cf.iPointSize = lf.lfHeight * 10;

		// show the dialog
		if (ChooseFont(&cf))
		{
			const auto fntflags(ParseLogfontToCommand(&lf));

			// color flags font info
			_ts_snprintf(data, mIRCLinker::m_dwCharacters, TEXT("%u %s"), cf.rgbColors, fntflags.to_chr());
		}
		return 3;
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("$!dcx(FontDialog)"), TEXT("\"%\" error: %"), input, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("$!dcx(FontDialog)"), TEXT("\"%\" error: Unknown Exception"), input);
	}
	return 0;
}

/*
 * /dcx MsgBox [STYLES] [TAB] [TITLE] [TAB] [MSG]
 * http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/windowing/dialogboxes/dialogboxreference/dialogboxfunctions/messagebox.asp
 */
mIRC(MsgBox)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok(TSTABCHAR) < 3)
			throw DcxExceptions::dcxInvalidArguments();

		DWORD			style = MB_DEFBUTTON1;
		const auto	strStyles(d.getfirsttok(1, TSTABCHAR).trim());		// tok 1
		const auto	strTitle(d.getnexttok(TSTABCHAR).trim());			// tok 2
		const auto	strMsg(d.getlasttoks().trim());					// tok 3, -1
		auto			owner = aWnd;

		for (const auto& tsStyle : strStyles)
		{
			//		MB_ABORTRETRYIGNORE
			//		MB_CANCELTRYCONTINUE && Dcx::XPPlusModule.isUseable()
			switch (std::hash<TString>{}(tsStyle))
			{
			case TEXT("ok"_hash):
				style |= MB_OK;
				break;
			case TEXT("okcancel"_hash):
				style |= MB_OKCANCEL;
				break;
			case TEXT("retrycancel"_hash):
				style |= MB_RETRYCANCEL;
				break;
			case TEXT("yesno"_hash):
				style |= MB_YESNO;
				break;
			case TEXT("yesnocancel"_hash):
				style |= MB_YESNOCANCEL;
				break;
			case TEXT("exclamation"_hash):
				style |= MB_ICONEXCLAMATION;
				break;
			case TEXT("warning"_hash):
				style |= MB_ICONWARNING;
				break;
			case TEXT("information"_hash):
				style |= MB_ICONINFORMATION;
				break;
			case TEXT("asterisk"_hash):
				style |= MB_ICONASTERISK;
				break;
			case TEXT("question"_hash):
				style |= MB_ICONQUESTION;
				break;
			case TEXT("stop"_hash):
				style |= MB_ICONSTOP;
				break;
			case TEXT("error"_hash):
				style |= MB_ICONERROR;
				break;
			case TEXT("hand"_hash):
				style |= MB_ICONHAND;
				break;
				//case TEXT("help"_hash):
				//	style |= MB_HELP;
				//	break;
			case TEXT("defbutton2"_hash):
				style |= MB_DEFBUTTON2;
				break;
			case TEXT("defbutton3"_hash):
				style |= MB_DEFBUTTON3;
				break;
			case TEXT("defbutton4"_hash):
				style |= MB_DEFBUTTON4;
				break;
			case TEXT("modal"_hash):
				style |= MB_APPLMODAL;
				break;
			case TEXT("sysmodal"_hash):
				style |= MB_SYSTEMMODAL;
				break;
			case TEXT("taskmodal"_hash):
				style |= MB_TASKMODAL;
				break;
			case TEXT("right"_hash):
				style |= MB_RIGHT;
				break;
			case TEXT("rtl"_hash):
				style |= MB_RTLREADING;
				break;
			case TEXT("foreground"_hash):
				style |= MB_SETFOREGROUND;
				break;
			case TEXT("topmost"_hash):
				style |= MB_TOPMOST;
				break;
			case TEXT("owner"_hash):
				owner = FindOwner(strStyles, gsl::not_null(mWnd));
				break;
			default:
				break;
			}
		}

		// if task modal, send in null to block app
		if (dcx_testflag(style, MB_TASKMODAL))
			owner = nullptr;

		//const TCHAR *const IDMSGS[] = {
		//	TEXT("abort"),
		//	TEXT("cancel"),
		//	TEXT("continue"),
		//	TEXT("ignore"),
		//	TEXT("no"),
		//	TEXT("ok"),
		//	TEXT("retry"),
		//	TEXT("tryagain"),
		//	TEXT("yes")
		//};
		//
		//switch (MessageBox(owner, strMsg.to_chr(), strTitle.to_chr(), style))
		//{
		//case IDABORT:
		//	ret(IDMSGS[0]);
		//	break;
		//case IDCANCEL:
		//	ret(IDMSGS[1]);
		//	break;
		//case IDCONTINUE:
		//	ret(IDMSGS[2]);
		//	break;
		//case IDIGNORE:
		//	ret(IDMSGS[3]);
		//	break;
		//case IDNO:
		//	ret(IDMSGS[4]);
		//	break;
		//case IDOK:
		//	ret(IDMSGS[5]);
		//	break;
		//case IDRETRY:
		//	ret(IDMSGS[6]);
		//	break;
		//case IDTRYAGAIN:
		//	ret(IDMSGS[7]);
		//	break;
		//case IDYES:
		//	ret(IDMSGS[8]);
		//	break;
		//default:
		//	break;
		//}

		const TCHAR* const IDMSGS[] = {
			nullptr,
			TEXT("ok"),
			TEXT("cancel"),
			TEXT("abort"),
			TEXT("retry"),
			TEXT("ignore"),
			TEXT("yes"),
			TEXT("no"),
			TEXT("close"),
			TEXT("help"),
			TEXT("tryagain"),
			TEXT("continue")
		};

		//switch (const auto res = MessageBox(owner, strMsg.to_chr(), strTitle.to_chr(), style); res)
		//{
		//case IDABORT:
		//case IDCANCEL:
		//case IDCONTINUE:
		//case IDIGNORE:
		//case IDNO:
		//case IDOK:
		//case IDRETRY:
		//case IDTRYAGAIN:
		//case IDYES:
		//	ret(gsl::at(IDMSGS,res));
		//	break;
		//default:
		//	break;
		//}

		ret(gsl::at(IDMSGS, MessageBox(owner, strMsg.to_chr(), strTitle.to_chr(), style)));
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("$!dcx(MsgBox)"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("$!dcx(MsgBox)"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	return 0;
}



/*
	$dcx(PickIcon,index filename)
*/
mIRC(PickIcon)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 2)
			throw DcxExceptions::dcxInvalidArguments();

		auto index = d.getfirsttok(1).to_int();	// tok 1
		auto tsFilename(d.getlasttoks());			// tok 2, -1

		if (!IsFile(tsFilename))
			throw DcxExceptions::dcxInvalidFilename();

		////TCHAR iconPath[MAX_PATH + 1];
		//
		//auto iconPath = std::make_unique<TCHAR[]>(MAX_PATH + 1U);
		//
		//GetFullPathName(filename.to_chr(), MAX_PATH, iconPath, nullptr);
		//
		//if (dcxPickIconDlg(aWnd, iconPath.get(), MAX_PATH, &index))
		//	wnsprintf(data, mIRCLinker::m_dwCharacters, TEXT("D_OK %d %s"), index, iconPath.get());
		//else
		//	wnsprintf(data, mIRCLinker::m_dwCharacters, TEXT("D_ERROR %d %s"), index, iconPath.get());
		//return 3;

		const stString<MAX_PATH + 1U> sIconPath;

		GetFullPathName(tsFilename.to_chr(), sIconPath.size(), sIconPath, nullptr);

		if (dcxPickIconDlg(aWnd, sIconPath.data(), sIconPath.size(), &index))
			_ts_snprintf(data, mIRCLinker::m_dwCharacters, TEXT("D_OK %d %s"), index, sIconPath.data());
		else
			_ts_snprintf(data, mIRCLinker::m_dwCharacters, TEXT("D_ERROR %d %s"), index, sIconPath.data());
		return 3;
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("$!dcx(PickIcon)"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("$!dcx(PickIcon)"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	return 0;
}

/*
* Icon Picker for WinXP SP2 & Win 2k3 (Others may or may not support it)
*/
int dcxPickIconDlg(const HWND hwnd, LPWSTR pszIconPath, const UINT& cchIconPath, int* piIconIndex) noexcept
{
	return PickIconDlg(hwnd, pszIconPath, cchIconPath, piIconIndex);
}

/*
	$dcx(CountIcons,filename)
*/
mIRC(CountIcons)
{
	TString filename(data);

	data[0] = 0;

	try {
		filename.trim();

		if (filename.empty())
			throw DcxExceptions::dcxInvalidArguments();

		if (!IsFile(filename))
			throw DcxExceptions::dcxInvalidFilename();

		_ts_snprintf(data, mIRCLinker::m_dwCharacters, TEXT("D_OK %u %s"), ExtractIconEx(filename.to_chr(), -1, nullptr, nullptr, 0), filename.to_chr());

		return 3;
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("$!dcx(CountIcons)"), TEXT("\"%\" error: %"), filename, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("$!dcx(CountIcons)"), TEXT("\"%\" error: Unknown Exception"), filename);
	}
	return 0;
}

