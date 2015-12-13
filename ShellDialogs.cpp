#include "defines.h"
#include "Dcx.h"



/*
 * Structure to store settings for use in BrowseFolderCallback.
 */
typedef struct {
	PTCHAR initialFolder;
	UINT flags;
} XBROWSEDIALOGSETTINGS, *LPXBROWSEDIALOGSETTINGS;


/*!
* \brief DCX DLL ColorDialog Function
*
* Argument \b data contains -> (DEFAULT) [STYLES]
*/
// ColorDialog (DEFAULT) [STYLES]
mIRC(ColorDialog) {
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		BOOL			retDefault = FALSE;
		CHOOSECOLOR		cc;
		static COLORREF clr[16];
#if TSTRING_TEMPLATES
		const auto		sel = d.getfirsttok(1).to_<COLORREF>();
#else
		const auto		sel = (COLORREF)d.getfirsttok(1).to_num();
#endif
		DWORD			styles = CC_RGBINIT;

		ZeroMemory(&cc, sizeof(CHOOSECOLOR));

		// initial settings
		cc.lStructSize = sizeof(CHOOSECOLOR);
		cc.hwndOwner = mWnd;

#if TSTRING_PARTS
		for (const auto &tsStyle : d) {
			if (tsStyle == TEXT("anycolor"))
				styles |= CC_ANYCOLOR;
			else if (tsStyle == TEXT("fullopen"))
				styles |= CC_FULLOPEN;
			else if (tsStyle == TEXT("nofullopen"))
				styles |= CC_PREVENTFULLOPEN;
			else if (tsStyle == TEXT("solidonly"))
				styles |= CC_SOLIDCOLOR;
			else if (tsStyle == TEXT("owner"))
				cc.hwndOwner = FindOwner(d, mWnd);
			else if (tsStyle == TEXT("returndefault"))
				retDefault = TRUE;
		}
#else
		for (auto tsStyle(d.getnexttok()); !tsStyle.empty(); tsStyle = d.getnexttok()) {
			if (tsStyle == TEXT("anycolor"))
				styles |= CC_ANYCOLOR;
			else if (tsStyle == TEXT("fullopen"))
				styles |= CC_FULLOPEN;
			else if (tsStyle == TEXT("nofullopen"))
				styles |= CC_PREVENTFULLOPEN;
			else if (tsStyle == TEXT("solidonly"))
				styles |= CC_SOLIDCOLOR;
			else if (tsStyle == TEXT("owner"))
				cc.hwndOwner = FindOwner(d, mWnd);
			else if (tsStyle == TEXT("returndefault"))
				retDefault = TRUE;
		}
#endif

		cc.rgbResult = sel;
		cc.Flags = styles;
		cc.lpCustColors = clr;

		// User clicked OK
		if (ChooseColor(&cc)) {
			//wsprintf(data, TEXT("%u"), cc.rgbResult);
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), cc.rgbResult);
			return 3; //ret(data);
		}
		// User clicked cancel, return default color
		else if (retDefault) {
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), sel);
			return 3; //ret(data);
		}
		// User clicked cancel, dont bother with default color
		else
			ret(TEXT("-1"));
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!dcx(ColorDialog)"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!dcx(ColorDialog)"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
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
mIRC(OpenDialog) {
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		// count number of tab tokens
		if (d.numtok(TSTAB) != 3)
			throw std::invalid_argument("Invalid Arguments");

		ret(FileDialog(d, TEXT("OPEN"), mWnd).to_chr());
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!dcx(OpenDialog)"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!dcx(OpenDialog)"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
}

// SaveDialog (styles) [TAB] (file) [TAB] (filter)
mIRC(SaveDialog) {
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		// count number of tab tokens
		if (d.numtok(TSTAB) != 3)
			throw std::invalid_argument("Invalid Arguments");

		ret(FileDialog(d, TEXT("SAVE"), mWnd).to_chr());
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!dcx(SaveDialog)"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!dcx(SaveDialog)"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
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
TString FileDialog(const TString & data, const TString &method, const HWND pWnd) {
	DWORD style = OFN_EXPLORER;
	OPENFILENAME ofn;
	TCHAR szFilename[MIRC_BUFFER_SIZE_CCH];

	// seperate the tokenz
	const auto styles(data.getfirsttok(1, TSTAB).trim());
	const auto file(data.getnexttok(TSTAB).trim());
	auto filter(data.getnexttok(TSTAB).trim());

	// Get Current dir for resetting later.
	const UINT tsBufSize = GetCurrentDirectory(0, nullptr);
	TString tsCurrentDir((UINT)tsBufSize);
	GetCurrentDirectory(tsBufSize,tsCurrentDir.to_chr());

	// format the filter into the format WinAPI wants, with double NULL TERMINATOR at end
	if (filter.empty())
		filter = TEXT("All Files (*.*)|*.*");

	filter += TEXT('|'); // <-- changed to a zero below for double null ending.
	filter.replace(TEXT('|'), TEXT('\0'));

	// set up the OFN struct
	ZeroMemory(&ofn, sizeof(ofn));
	dcx_strcpyn(szFilename, file.to_chr(), MIRC_BUFFER_SIZE_CCH);

	ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
	ofn.hwndOwner = pWnd;
	ofn.lpstrFilter = filter.to_chr();
	ofn.lpstrFile = szFilename;
	ofn.nMaxFile = MIRC_BUFFER_SIZE_CCH;
	ofn.lpstrDefExt = TEXT("");

#if TSTRING_PARTS
	for (const auto &tsStyle: styles) {
		if (tsStyle == TEXT("multisel"))
			style |= OFN_ALLOWMULTISELECT;
		else if (tsStyle == TEXT("createprompt"))
			style |= OFN_CREATEPROMPT;
		// FIXME: explorer style resizable on default, cant get rid of that shit
		else if (tsStyle == TEXT("enablesizing"))
			style |= OFN_ENABLESIZING;
		else if (tsStyle == TEXT("filemustexist"))
			style |= OFN_FILEMUSTEXIST; // (open)
		else if (tsStyle == TEXT("showhidden"))
			style |= OFN_FORCESHOWHIDDEN; // 2k/xp
		else if (tsStyle == TEXT("noreadonly"))
			style |= OFN_HIDEREADONLY;
		else if (tsStyle == TEXT("nochangedir"))
			style |= OFN_NOCHANGEDIR; // (save)
		else if (tsStyle == TEXT("getshortcuts"))
			style |= OFN_NODEREFERENCELINKS;
		else if (tsStyle == TEXT("nonetwork"))
			style |= OFN_NONETWORKBUTTON;
		else if (tsStyle == TEXT("novalidate"))
			style |= OFN_NOVALIDATE;
		else if (tsStyle == TEXT("norecent"))
			style |= OFN_DONTADDTORECENT; // 2k/xp
		else if (tsStyle == TEXT("overwriteprompt"))
			style |= OFN_OVERWRITEPROMPT; // save
		else if (tsStyle == TEXT("pathmustexist"))
			style |= OFN_PATHMUSTEXIST;
		else if (tsStyle == TEXT("owner"))
			ofn.hwndOwner = FindOwner(styles, pWnd);
	}
#else
	for (auto tsStyle(styles.getfirsttok(1)); !tsStyle.empty(); tsStyle = styles.getnexttok()) {
		if (tsStyle == TEXT("multisel"))
			style |= OFN_ALLOWMULTISELECT;
		else if (tsStyle == TEXT("createprompt"))
			style |= OFN_CREATEPROMPT;
		// FIXME: explorer style resizable on default, cant get rid of that shit
		else if (tsStyle == TEXT("enablesizing"))
			style |= OFN_ENABLESIZING;
		else if (tsStyle == TEXT("filemustexist"))
			style |= OFN_FILEMUSTEXIST; // (open)
		else if (tsStyle == TEXT("showhidden"))
			style |= OFN_FORCESHOWHIDDEN; // 2k/xp
		else if (tsStyle == TEXT("noreadonly"))
			style |= OFN_HIDEREADONLY;
		else if (tsStyle == TEXT("nochangedir"))
			style |= OFN_NOCHANGEDIR; // (save)
		else if (tsStyle == TEXT("getshortcuts"))
			style |= OFN_NODEREFERENCELINKS;
		else if (tsStyle == TEXT("nonetwork"))
			style |= OFN_NONETWORKBUTTON;
		else if (tsStyle == TEXT("novalidate"))
			style |= OFN_NOVALIDATE;
		else if (tsStyle == TEXT("norecent"))
			style |= OFN_DONTADDTORECENT; // 2k/xp
		else if (tsStyle == TEXT("overwriteprompt"))
			style |= OFN_OVERWRITEPROMPT; // save
		else if (tsStyle == TEXT("pathmustexist"))
			style |= OFN_PATHMUSTEXIST;
		else if (tsStyle == TEXT("owner"))
			ofn.hwndOwner = FindOwner(styles, pWnd);
	}
#endif

	ofn.Flags = style;

	TString tsResult;

	if (method == TEXT("OPEN") && GetOpenFileName(&ofn)) {
		// if there are multiple files selected
		if (dcx_testflag(style, OFN_ALLOWMULTISELECT)) {
			const auto *p = szFilename;

			// process the file name at p since its null terminated
			while (*p != TEXT('\0')) {
#if TSTRING_TEMPLATES
				tsResult.addtok(p, TEXT('|'));
#else
				tsResult.addtok(p, TEXT("|"));
#endif
				p += lstrlen(p) + 1;
			} 
		}
		// copy the string directly
		else
			tsResult = szFilename;
	}
	else if (method == TEXT("SAVE") && GetSaveFileName(&ofn))
		tsResult = szFilename;

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
mIRC(BrowseDialog) {
	TString input(data);

	data[0] = 0;

	try {
		input.trim();

		BROWSEINFO bi;

		// seperate the tokens (by tabs)
		int currentParam = 1;
		bool bInitialFolder = false;
		bool bDialogText = false;

		TString initPath((UINT)MAX_PATH);
		TString displayPath((UINT)MAX_PATH);
		LPITEMIDLIST pidlRoot = nullptr;
		LPITEMIDLIST pidl;
		XBROWSEDIALOGSETTINGS extra;

		// set up the BI structure
		ZeroMemory(&bi, sizeof(BROWSEINFO));
		ZeroMemory(&extra, sizeof(XBROWSEDIALOGSETTINGS));
		bi.hwndOwner = mWnd;                                             // Default owner: mIRC main window
		bi.lpfn = BrowseFolderCallback;
		bi.pszDisplayName = displayPath.to_chr();
		bi.ulFlags = BIF_VALIDATE;
		bi.lParam = (LPARAM)&extra;

		// Parse styles
		auto param(input.gettok(currentParam, TSTAB));

#if TSTRING_PARTS
		for (const auto &flag : param) {
			/*
			style1 style2 style3 $chr(9) initial folder

			//clear | echo -a Selected: $dcx(BrowseDialog, hello asd $chr(9) C:\Windows $chr(9) mr t says select something fool! ) | /udcx

			http://msdn2.microsoft.com/en-us/library/bb773205.aspx
			*/

			if (flag == TEXT("advanced"))
				bi.ulFlags |= BIF_USENEWUI;
			else if (flag == TEXT("edit"))
				bi.ulFlags |= BIF_EDITBOX;
			else if (flag == TEXT("newstyle"))
				bi.ulFlags |= BIF_NEWDIALOGSTYLE;

			else if (flag == TEXT("nonew"))
				bi.ulFlags |= BIF_NONEWFOLDERBUTTON;
			else if (flag == TEXT("files"))
				bi.ulFlags |= BIF_BROWSEINCLUDEFILES;
			else if (flag == TEXT("title"))
				bDialogText = true;
			else if (flag == TEXT("initfolder"))
				bInitialFolder = true;

			else if (flag == TEXT("computers")) {
				// NOTE: do not use with TEXT("advanced")
				bi.ulFlags |= BIF_BROWSEFORCOMPUTER;
				pidlRoot = GetFolderFromCSIDL(CSIDL_NETWORK);
			}
			else if (flag == TEXT("printers")) {
				// NOTE: do not use with TEXT("advanced")
				bi.ulFlags |= BIF_BROWSEFORPRINTER;
				pidlRoot = GetFolderFromCSIDL(CSIDL_PRINTERS);
			}
			else if (flag == TEXT("nonetwork"))
				bi.ulFlags |= BIF_DONTGOBELOWDOMAIN;
			else if (flag == TEXT("shortcut"))
				bi.ulFlags |= BIF_NOTRANSLATETARGETS;

			// owner
			else if (flag == TEXT("owner"))
				bi.hwndOwner = FindOwner(param, mWnd);
		}
#else
		for (auto flag(param.getfirsttok( 1 )); !flag.empty(); flag = param.getnexttok( ) ) {
			/*
			style1 style2 style3 $chr(9) initial folder

			//clear | echo -a Selected: $dcx(BrowseDialog, hello asd $chr(9) C:\Windows $chr(9) mr t says select something fool! ) | /udcx

			http://msdn2.microsoft.com/en-us/library/bb773205.aspx
			*/

			if (flag == TEXT("advanced"))
				bi.ulFlags |= BIF_USENEWUI;
			else if (flag == TEXT("edit"))
				bi.ulFlags |= BIF_EDITBOX;
			else if (flag == TEXT("newstyle"))
				bi.ulFlags |= BIF_NEWDIALOGSTYLE;

			else if (flag == TEXT("nonew"))
				bi.ulFlags |= BIF_NONEWFOLDERBUTTON;
			else if (flag == TEXT("files"))
				bi.ulFlags |= BIF_BROWSEINCLUDEFILES;
			else if (flag == TEXT("title"))
				bDialogText = true;
			else if (flag == TEXT("initfolder"))
				bInitialFolder = true;

			else if (flag == TEXT("computers")) {
				// NOTE: do not use with TEXT("advanced")
				bi.ulFlags |= BIF_BROWSEFORCOMPUTER;
				pidlRoot = GetFolderFromCSIDL(CSIDL_NETWORK);
			}
			else if (flag == TEXT("printers")) {
				// NOTE: do not use with TEXT("advanced")
				bi.ulFlags |= BIF_BROWSEFORPRINTER;
				pidlRoot = GetFolderFromCSIDL(CSIDL_PRINTERS);
			}
			else if (flag == TEXT("nonetwork"))
				bi.ulFlags |= BIF_DONTGOBELOWDOMAIN;
			else if (flag == TEXT("shortcut"))
				bi.ulFlags |= BIF_NOTRANSLATETARGETS;

			// owner
			else if (flag == TEXT("owner"))
				bi.hwndOwner = FindOwner(param, mWnd);
		}
#endif

		Auto(CoTaskMemFree(pidlRoot));

		// Set initial folder
		if (bInitialFolder && (pidlRoot == nullptr)) {
			currentParam++;
			initPath = input.gettok(currentParam, TSTAB).trim();

			extra.initialFolder = initPath.to_chr();
		}

		// Set title text.
		if (bDialogText) {
			currentParam++;
			param = input.gettok(currentParam, TSTAB).trim();

			bi.lpszTitle = param.to_chr();
		}

		// Set root folder
		if (pidlRoot != nullptr)
			bi.pidlRoot = pidlRoot;

		extra.flags = bi.ulFlags;
		pidl = SHBrowseForFolder(&bi);

		// User cancelled
		if (pidl == nullptr)
			ret(TEXT(""));

		Auto(CoTaskMemFree(pidl));

		// If we were searching for a computer ...
		if (dcx_testflag(bi.ulFlags, BIF_BROWSEFORCOMPUTER))
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("//%s"), displayPath.to_chr());
		else {
			SHGetPathFromIDList(pidl, initPath.to_chr());
			//wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%s"), initPath.to_chr());
			dcx_strcpyn(data, initPath.to_chr(), MIRC_BUFFER_SIZE_CCH);
		}

		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!dcx(BrowseDialog)"), TEXT("\"%s\" error: %S"), input.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!dcx(BrowseDialog)"), TEXT("\"%s\" error: Unknown Exception"), input.to_chr());
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
LPITEMIDLIST GetFolderFromCSIDL(const int nCsidl) {
	LPITEMIDLIST pidlRoot;

	if (S_OK == SHGetFolderLocation(nullptr, nCsidl, nullptr, 0, &pidlRoot))
		return pidlRoot;

	return nullptr;                // Caller assumes responsibility
}


int CALLBACK BrowseFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
	auto extra = reinterpret_cast<LPXBROWSEDIALOGSETTINGS>(lpData);

	switch (uMsg) {
		// User typed invalid name (non-existant folder) into editbox.
		// This must return non-zero, otherwise it will close the dialog.
		case BFFM_VALIDATEFAILED:
			return TRUE;

		case BFFM_INITIALIZED:
			// Sets initial folder if it is specified.
			if (lpData != NULL)
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM) extra->initialFolder);
			else
				// Disable OK button.
				SendMessage(hwnd, BFFM_ENABLEOK, TRUE, FALSE);

			break;

		case BFFM_SELCHANGED:
		{
			// Dont check for COMPUTER or PRINTER browsing
			if (dcx_testflag(extra->flags, BIF_BROWSEFORCOMPUTER) || dcx_testflag(extra->flags, BIF_BROWSEFORPRINTER))
				break;

			TString path((UINT) MAX_PATH);

			if (SHGetPathFromIDList(reinterpret_cast<LPITEMIDLIST>(lParam), path.to_chr()))
			{
				SendMessage(hwnd, BFFM_ENABLEOK, TRUE, TRUE);
				SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM) path.to_chr());
			}
			else {
				SendMessage(hwnd, BFFM_ENABLEOK, TRUE, FALSE);
				SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, NULL);
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
mIRC(FontDialog) {
	DWORD style = CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST | CF_LIMITSIZE;
	CHOOSEFONT cf;
	LOGFONT lf;

	// seperate the tokens (by tabs)
	TString input(data);

	data[0] = TEXT('\0');

	try {
		input.trim();

		// set up the LF structure
		ZeroMemory(&lf, sizeof(LOGFONT));
		GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);

		// set up the CF struct
		ZeroMemory(&cf, sizeof(CHOOSEFONT));
		cf.lStructSize = sizeof(CHOOSEFONT);
		cf.hwndOwner = mWnd;
		cf.lpLogFont = &lf;
		cf.nSizeMin = 8;
		cf.nSizeMax = 72;

#if TSTRING_PARTS
#if TSTRING_ITERATOR
		for (auto itStart = input.begin(TSTAB), itEnd = input.end(); itStart != itEnd; ++itStart)
		{
			const auto option(*itStart);
			const auto numtok = option.numtok();
			const auto tsType(option.getfirsttok(1));	// tok 1

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
			if (tsType == TEXT("flags") && numtok > 1) {
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
			// defaults +flags(ibsua) charset size fontname
			else if (tsType == TEXT("default") && numtok > 4)
				ParseCommandToLogfont(option.getlasttoks(), &lf);	// tok 2, -1
			// color rgb
			else if (tsType == TEXT("color") && numtok > 1)
				cf.rgbColors = (COLORREF)option.getnexttok().to_num();	// tok 2
			// minmaxsize min max
			else if (tsType == TEXT("minmaxsize") && numtok > 2) {
				cf.nSizeMin = option.getnexttok().to_int();	// tok 2
				cf.nSizeMax = option.getnexttok().to_int();	// tok 3
			}
			// owner
			else if (tsType == TEXT("owner") && numtok > 1)
				cf.hwndOwner = FindOwner(option, mWnd);
		}
#else
		input.split(TSTAB);
		for (const auto &option : input) {
			const auto numtok = option.numtok();
			const auto tsType(option.getfirsttok(1));	// tok 1

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
			if (tsType == TEXT("flags") && numtok > 1) {
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
			// defaults +flags(ibsua) charset size fontname
			else if (tsType == TEXT("default") && numtok > 4)
				ParseCommandToLogfont(option.getlasttoks(), &lf);	// tok 2, -1
			// color rgb
			else if (tsType == TEXT("color") && numtok > 1)
				cf.rgbColors = (COLORREF)option.getnexttok().to_num();	// tok 2
			// minmaxsize min max
			else if (tsType == TEXT("minmaxsize") && numtok > 2) {
				cf.nSizeMin = option.getnexttok().to_int();	// tok 2
				cf.nSizeMax = option.getnexttok().to_int();	// tok 3
			}
			// owner
			else if (tsType == TEXT("owner") && numtok > 1)
				cf.hwndOwner = FindOwner(option, mWnd);
		}
#endif
#else
		const auto nToks = input.numtok(TSTAB);

		input.getfirsttok(0, TSTAB);

		for (auto i = 1U; i <= nToks; i++) {
			const auto option(input.getnexttok( TSTAB ).trim());
			const auto numtok = option.numtok( );
			const auto tsType(option.getfirsttok( 1 ));	// tok 1

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
			if (tsType == TEXT("flags") && numtok > 1) {
				const auto flag(option.getnexttok( ));	// tok 2
				const auto c = flag.len();

				for (auto j = 0U; j < c; j++)
				{
					if (flag[j] == TEXT('a'))
						style |= CF_NOFACESEL;
					else if (flag[j] == TEXT('b'))
						style |= CF_SCRIPTSONLY;
					else if (flag[j] == TEXT('c'))
						style |= CF_SCALABLEONLY;// (Scalable fonts include vector fonts, scalable printer fonts, TrueType fonts, and fonts scaled by other technologies.)
					else if (flag[j] == TEXT('e'))
						style |= CF_EFFECTS;
					else if (flag[j] == TEXT('f'))
						style |= CF_FORCEFONTEXIST;
					else if (flag[j] == TEXT('h'))
						style |= CF_NOSCRIPTSEL;
					else if (flag[j] == TEXT('i'))
						style |= CF_NOSIMULATIONS;
					else if (flag[j] == TEXT('m'))
						style |= CF_SELECTSCRIPT;
					else if (flag[j] == TEXT('n'))
						style |= CF_PRINTERFONTS;
					else if (flag[j] == TEXT('p'))
						style |= CF_FIXEDPITCHONLY;
					else if (flag[j] == TEXT('r'))
						style |= CF_NOVERTFONTS;
					else if (flag[j] == TEXT('s'))
						style |= CF_SCREENFONTS;
					else if (flag[j] == TEXT('t'))
						style |= CF_TTONLY;
					else if (flag[j] == TEXT('v'))
						style |= CF_NOVECTORFONTS;
					else if (flag[j] == TEXT('w'))
						style |= CF_WYSIWYG;
					else if (flag[j] == TEXT('y'))
						style |= CF_NOSTYLESEL;
					else if (flag[j] == TEXT('z'))
						style |= CF_NOSIZESEL;
				}
			}
			// defaults +flags(ibsua) charset size fontname
			else if (tsType == TEXT("default") && numtok > 4)
				ParseCommandToLogfont(option.getlasttoks(), &lf);	// tok 2, -1
			// color rgb
			else if (tsType == TEXT("color") && numtok > 1)
				cf.rgbColors = (COLORREF) option.getnexttok( ).to_num();	// tok 2
			// minmaxsize min max
			else if (tsType == TEXT("minmaxsize") && numtok > 2) {
				cf.nSizeMin = option.getnexttok( ).to_int();	// tok 2
				cf.nSizeMax = option.getnexttok( ).to_int();	// tok 3
			}
			// owner
			else if (tsType == TEXT("owner") && numtok > 1)
				cf.hwndOwner = FindOwner(option, mWnd);
		}
#endif

		// check that at least some fonts are showing
		if (!dcx_testflag(style, CF_PRINTERFONTS) && !dcx_testflag(style, CF_SCREENFONTS))
			style |= CF_SCREENFONTS;

		cf.Flags = style;
		cf.iPointSize = lf.lfHeight * 10;

		// show the dialog
		if (ChooseFont(&cf)) {
			const auto fntflags(ParseLogfontToCommand(&lf));

			// color flags font info
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%u %s"), cf.rgbColors, fntflags.to_chr());
		}
		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!dcx(FontDialog)"), TEXT("\"%s\" error: %S"), input.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!dcx(FontDialog)"), TEXT("\"%s\" error: Unknown Exception"), input.to_chr());
	}
	return 0;
}

/*
 * /dcx MsgBox [STYLES] [TAB] [TITLE] [TAB] [MSG]
 * http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/windowing/dialogboxes/dialogboxreference/dialogboxfunctions/messagebox.asp
 */
mIRC(MsgBox) {
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok(TSTAB) < 3)
			throw std::invalid_argument("invalid parameters");

		DWORD			style     = MB_DEFBUTTON1;
		const auto	strStyles(d.getfirsttok(1, TSTAB).trim());		// tok 1
		const auto	strTitle(d.getnexttok(TSTAB).trim());			// tok 2
		const auto	strMsg(d.getlasttoks().trim());					// tok 3, -1
		auto			owner = aWnd;

#if TSTRING_PARTS
		for (const auto &tsStyle: strStyles) {
			//		MB_ABORTRETRYIGNORE
			//		MB_CANCELTRYCONTINUE && Dcx::XPPlusModule.isUseable()
			if (tsStyle == TEXT("ok"))
				style |= MB_OK;
			else if (tsStyle == TEXT("okcancel"))
				style |= MB_OKCANCEL;
			else if (tsStyle == TEXT("retrycancel"))
				style |= MB_RETRYCANCEL;
			else if (tsStyle == TEXT("yesno"))
				style |= MB_YESNO;
			else if (tsStyle == TEXT("yesnocancel"))
				style |= MB_YESNOCANCEL;
			else if (tsStyle == TEXT("exclamation"))
				style |= MB_ICONEXCLAMATION;
			else if (tsStyle == TEXT("warning"))
				style |= MB_ICONWARNING;
			else if (tsStyle == TEXT("information"))
				style |= MB_ICONINFORMATION;
			else if (tsStyle == TEXT("asterisk"))
				style |= MB_ICONASTERISK;
			else if (tsStyle == TEXT("question"))
				style |= MB_ICONQUESTION;
			else if (tsStyle == TEXT("stop"))
				style |= MB_ICONSTOP;
			else if (tsStyle == TEXT("error"))
				style |= MB_ICONERROR;
			else if (tsStyle == TEXT("hand"))
				style |= MB_ICONHAND;
			//else if (tsStyle == TEXT("help"))
			//	style |= MB_HELP;
			else if (tsStyle == TEXT("defbutton2"))
				style |= MB_DEFBUTTON2;
			else if (tsStyle == TEXT("defbutton3"))
				style |= MB_DEFBUTTON3;
			else if (tsStyle == TEXT("defbutton4"))
				style |= MB_DEFBUTTON4;
			else if (tsStyle == TEXT("modal"))
				style |= MB_APPLMODAL;
			else if (tsStyle == TEXT("sysmodal"))
				style |= MB_SYSTEMMODAL;
			else if (tsStyle == TEXT("taskmodal"))
				style |= MB_TASKMODAL;
			else if (tsStyle == TEXT("right"))
				style |= MB_RIGHT;
			else if (tsStyle == TEXT("rtl"))
				style |= MB_RTLREADING;
			else if (tsStyle == TEXT("foreground"))
				style |= MB_SETFOREGROUND;
			else if (tsStyle == TEXT("topmost"))
				style |= MB_TOPMOST;
			else if (tsStyle == TEXT("owner"))
				owner = FindOwner(strStyles, mWnd);
		}
	#else
		for (auto tsStyle(strStyles.getfirsttok(1)); !tsStyle.empty(); tsStyle = strStyles.getnexttok()) {
	//		MB_ABORTRETRYIGNORE
	//		MB_CANCELTRYCONTINUE && Dcx::XPPlusModule.isUseable()
			if (tsStyle == TEXT("ok"))
				style |= MB_OK;
			else if (tsStyle == TEXT("okcancel"))
				style |= MB_OKCANCEL;
			else if (tsStyle == TEXT("retrycancel"))
				style |= MB_RETRYCANCEL;
			else if (tsStyle == TEXT("yesno"))
				style |= MB_YESNO;
			else if (tsStyle == TEXT("yesnocancel"))
				style |= MB_YESNOCANCEL;
			else if (tsStyle == TEXT("exclamation"))
				style |= MB_ICONEXCLAMATION;
			else if (tsStyle == TEXT("warning"))
				style |= MB_ICONWARNING;
			else if (tsStyle == TEXT("information"))
				style |= MB_ICONINFORMATION;
			else if (tsStyle == TEXT("asterisk"))
				style |= MB_ICONASTERISK;
			else if (tsStyle == TEXT("question"))
				style |= MB_ICONQUESTION;
			else if (tsStyle == TEXT("stop"))
				style |= MB_ICONSTOP;
			else if (tsStyle == TEXT("error"))
				style |= MB_ICONERROR;
			else if (tsStyle == TEXT("hand"))
				style |= MB_ICONHAND;
			//else if (tsStyle == TEXT("help"))
			//	style |= MB_HELP;
			else if (tsStyle == TEXT("defbutton2"))
				style |= MB_DEFBUTTON2;
			else if (tsStyle == TEXT("defbutton3"))
				style |= MB_DEFBUTTON3;
			else if (tsStyle == TEXT("defbutton4"))
				style |= MB_DEFBUTTON4;
			else if (tsStyle == TEXT("modal"))
				style |= MB_APPLMODAL;
			else if (tsStyle == TEXT("sysmodal"))
				style |= MB_SYSTEMMODAL;
			else if (tsStyle == TEXT("taskmodal"))
				style |= MB_TASKMODAL;
			else if (tsStyle == TEXT("right"))
				style |= MB_RIGHT;
			else if (tsStyle == TEXT("rtl"))
				style |= MB_RTLREADING;
			else if (tsStyle == TEXT("foreground"))
				style |= MB_SETFOREGROUND;
			else if (tsStyle == TEXT("topmost"))
				style |= MB_TOPMOST;
			else if (tsStyle == TEXT("owner"))
				owner = FindOwner(strStyles, mWnd);
		}
	#endif

		// if task modal, send in null to block app
		if (dcx_testflag(style, MB_TASKMODAL))
			owner = nullptr;

		switch (MessageBox(owner, strMsg.to_chr(), strTitle.to_chr(), style)) {
			case IDABORT:
				ret(TEXT("abort"));
				break;
			case IDCANCEL:
				ret(TEXT("cancel"));
				break;
			case IDCONTINUE:
				ret(TEXT("continue"));
				break;
			case IDIGNORE:
				ret(TEXT("ignore"));
				break;
			case IDNO:
				ret(TEXT("no"));
				break;
			case IDOK:
				ret(TEXT("ok"));
				break;
			case IDRETRY:
				ret(TEXT("retry"));
				break;
			case IDTRYAGAIN:
				ret(TEXT("tryagain"));
				break;
			case IDYES:
				ret(TEXT("yes"));
				break;
			default:
				ret(TEXT(""));
				break;
		}
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!dcx(MsgBox)"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!dcx(MsgBox)"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
}



/*
	$dcx(PickIcon,index filename)
*/
mIRC(PickIcon) {
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() < 2)
			throw std::invalid_argument("Invalid parameters");

		auto index = d.getfirsttok(1).to_int();	// tok 1
		auto filename(d.getlasttoks());			// tok 2, -1

		if (!IsFile(filename))
			throw std::invalid_argument("Invalid filename");

		//TCHAR iconPath[MAX_PATH + 1];
		auto iconPath = std::make_unique<TCHAR[]>(MAX_PATH + 1);

		GetFullPathName(filename.to_chr(), MAX_PATH, iconPath.get(), nullptr);

		if (dcxPickIconDlg(aWnd, iconPath.get(), MAX_PATH, &index))
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("D_OK %d %s"), index, iconPath.get());
		else
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("D_ERROR %d %s"), index, iconPath.get());
		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!dcx(PickIcon)"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!dcx(PickIcon)"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
}

/*
* Icon Picker for WinXP SP2 & Win 2k3 (Others may or may not support it)
*/
int dcxPickIconDlg(HWND hwnd, LPWSTR pszIconPath, UINT cchIconPath, int *piIconIndex)
{
	return PickIconDlg(hwnd, pszIconPath, cchIconPath, piIconIndex);
}

/*
	$dcx(CountIcons,filename)
*/
mIRC(CountIcons) {
	TString filename(data);

	data[0] = 0;

	try {
		filename.trim();

		if (filename.empty())
			throw std::invalid_argument("Invalid parameters");

		if (!IsFile(filename))
			throw std::invalid_argument("Invalid filename");

		wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("D_OK %u %s"), ExtractIconEx(filename.to_chr(), -1, nullptr, nullptr, 0), filename.to_chr());

		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!dcx(CountIcons)"), TEXT("\"%s\" error: %S"), filename.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!dcx(CountIcons)"), TEXT("\"%s\" error: Unknown Exception"), filename.to_chr());
	}
	return 0;
}

