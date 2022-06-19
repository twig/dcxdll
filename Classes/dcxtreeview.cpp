/*!
 * \file dcxtreeview.cpp
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.1
 *
 * \b Revisions
 *	1.1
 *		Added Visual Studio 2005 specific code. Ook
 *
 * © ScriptsDB.org - 2006
 */
#include "defines.h"
#include "Classes/dcxtreeview.h"
#include "Classes/dcxdialog.h"
#include "Dcx.h"

 /*!
  * \brief Constructor
  *
  * \param ID Control ID
  * \param p_Dialog Parent DcxDialog Object
  * \param mParentHwnd Parent Window Handle
  * \param rc Window Rectangle
  * \param styles Window Style Tokenized List
  */

DcxTreeView::DcxTreeView(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles)
	: DcxControl(ID, p_Dialog)
{
	const auto ws = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ws.m_ExStyles | WS_EX_CLIENTEDGE,
		DCX_TREEVIEWCLASS,
		ws.m_Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID,
		this);

	if (!IsWindow(m_Hwnd))
		throw DcxExceptions::dcxUnableToCreateWindow();

	if (ws.m_NoTheme)
		Dcx::UXModule.dcxSetWindowTheme(m_Hwnd, L" ", L" ");

	setNoThemed(ws.m_NoTheme);

	SendMessage(m_Hwnd, CCM_SETVERSION, 6U, 0);

	// Ook: ExStyle or Style needs checked...
	if ((ws.m_Styles & WindowStyle::TVS_CheckBoxes) != WindowStyle::None)
		this->addStyle(WindowStyle::TVS_CheckBoxes);

	this->setToolTipHWND(TreeView_GetToolTips(m_Hwnd));
	if (styles.istok(TEXT("balloon")) && this->getToolTipHWND())
		dcxSetWindowStyle(getToolTipHWND(), dcxGetWindowStyle(getToolTipHWND()) | TTS_BALLOON);

	if (Dcx::VistaModule.isUseable())
	{
		//auto ExStylesTreeView = gsl::narrow_cast<long>(TreeView_GetExtendedStyle(m_Hwnd));
		//parseTreeViewExStyles( styles, &ExStylesTreeView);
		//TreeView_SetExtendedStyle(m_Hwnd, gsl::narrow_cast<LPARAM>(ExStylesTreeView), gsl::narrow_cast<WPARAM>(ExStylesTreeView));

		const auto ExStylesTreeView = parseTreeViewExStyles(styles);
		TreeView_SetExtendedStyle(m_Hwnd, gsl::narrow_cast<LPARAM>(ExStylesTreeView), gsl::narrow_cast<WPARAM>(ExStylesTreeView));
	}

	this->setControlFont(Dcx::dcxGetStockObject<HFONT>(DEFAULT_GUI_FONT), FALSE);

	DragAcceptFiles(m_Hwnd, TRUE);
	m_bTransparent = isExStyle(WindowExStyle::Transparent);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxTreeView::~DcxTreeView() noexcept
{
	// make sure that there are no sensless events called when deleting all items
	m_bDestroying = true;
	// clear all items
	TreeView_DeleteAllItems(m_Hwnd);

	ImageList_Destroy(this->getImageList(TVSIL_NORMAL));
	ImageList_Destroy(this->getImageList(TVSIL_STATE));

	PreloadData();
}

/*!
 * \brief blah
 *
 * blah
 */

dcxWindowStyles DcxTreeView::parseControlStyles(const TString& tsStyles)
{
	auto ws = parseGeneralControlStyles(tsStyles);

	ws.m_Styles |= TVS_INFOTIP;

	for (const auto& tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"haslines"_hash:
			ws.m_Styles |= TVS_HASLINES;
			break;
		case L"hasbuttons"_hash:
			ws.m_Styles |= TVS_HASBUTTONS;
			break;
		case L"linesatroot"_hash:
			ws.m_Styles |= TVS_LINESATROOT;
			break;
		case L"showsel"_hash:
			ws.m_Styles |= TVS_SHOWSELALWAYS;
			break;
		case L"editlabel"_hash:
			ws.m_Styles |= TVS_EDITLABELS;
			break;
		case L"nohscroll"_hash:
			ws.m_Styles |= TVS_NOHSCROLL;
			break;
		case L"notooltips"_hash:
			ws.m_Styles |= TVS_NOTOOLTIPS;
			break;
		case L"noscroll"_hash:
			ws.m_Styles |= TVS_NOSCROLL;
			break;
		case L"fullrow"_hash:
			ws.m_Styles |= TVS_FULLROWSELECT;
			break;
		case L"singleexpand"_hash:
			ws.m_Styles |= TVS_SINGLEEXPAND;
			break;
		case L"checkbox"_hash:
			ws.m_Styles |= TVS_CHECKBOXES;
			break;
		case L"custom"_hash:
			m_bCustomDraw = true;
			break;
		default:
			break;
		}
	}

	return ws;
}

WindowExStyle DcxTreeView::parseTreeViewExStyles(const TString& styles) const
{
	WindowExStyle ExStyles(gsl::narrow_cast<WindowExStyle>(TreeView_GetExtendedStyle(m_Hwnd)));

	// Vista+ ONLY!
	for (const auto& tsStyle : styles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"fadebuttons"_hash:
			ExStyles |= TVS_EX_FADEINOUTEXPANDOS;
			break;
		case L"doublebuffer"_hash:
			ExStyles |= TVS_EX_DOUBLEBUFFER;
			break;
		case L"noident"_hash:
			ExStyles |= TVS_EX_NOINDENTSTATE;
			break;
		case L"richtooltip"_hash:
			ExStyles |= TVS_EX_RICHTOOLTIP;
			break;
		case L"autohscroll"_hash:
			ExStyles |= TVS_EX_AUTOHSCROLL;
			break;
		case L"dimmedchecks"_hash:
			ExStyles |= TVS_EX_DIMMEDCHECKBOXES;
			break;
		default:
			break;
		}
	}
	return ExStyles;
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */
void DcxTreeView::parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	const auto numtok = input.numtok();
	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
		// [NAME] [ID] [PROP] [PATH]
	case L"text"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		this->getItemText(item, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	break;
	// [NAME] [ID] [PROP] [PATH]
	case L"icon"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		const auto ImageID = getItemImageID(item);
		_ts_snprintf(szReturnValue, TEXT("%d"), (ImageID > 10000 ? -2 : ImageID) + 1);
	}
	break;
	// [NAME] [ID] [PROP] [PATH]
	case L"tooltip"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getlasttoks());	// tok 4, -1
		auto item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		if (auto lpdcxtvi = getItemParam(item); lpdcxtvi)
			szReturnValue = lpdcxtvi->tsTipText.to_chr();
	}
	break;
	// [NAME] [ID] [PROP]
	case L"seltext"_hash:
	{
		if (auto hItem = TV_GetSelection(m_Hwnd); hItem)
			this->getItemText(hItem, szReturnValue, MIRC_BUFFER_SIZE_CCH);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"selpath"_hash:
	{
		if (auto hItem = TV_GetSelection(m_Hwnd); hItem)
			szReturnValue = getPathFromItem(hItem).to_chr();
	}
	break;
	// [NAME] [ID] [PROP] {TAB}[MATCHTEXT]{TAB} [T] [N] [SUBPATH]
	case L"find"_hash:
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto matchtext(input.getfirsttok(2, TSTABCHAR).trim());
		const auto params(input.getnexttok(TSTABCHAR).trim());		// tok 3

		if (matchtext.empty())
			throw Dcx::dcxException("No matchtext specified.");

		auto SearchType = CharToSearchType(params++[0]);
		HTREEITEM startingPoint = TVI_ROOT;

		const auto n = params++.to_int();	// tok 2

		if (params.numtok() > 2)
		{
			const auto path(params.getlasttoks());	// tok 3, -1

			startingPoint = this->parsePath(path);

			if (!startingPoint)
				throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);
		}

		auto matchCount = 0;

		const dcxSearchData srch_data(matchtext, SearchType);
		if (const auto result = findItemText(startingPoint, n, matchCount, srch_data); result.has_value())
			szReturnValue = getPathFromItem(*result).to_chr();
		else if (n == 0)
			_ts_snprintf(szReturnValue, TEXT("%d"), matchCount);
	}
	break;
	// [NAME] [ID] [PROP] [PATH]
	case L"state"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getlasttoks());	// tok 4, -1
		const auto* const item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		if (this->isStyle(WindowStyle::TVS_CheckBoxes))
		{
			switch (TreeView_GetCheckState(m_Hwnd, item))
			{
			case 1:
				szReturnValue = TEXT('2');
				break;
			case 0:
				szReturnValue = TEXT('1');
				break;
			default:
				szReturnValue = TEXT('0');
				break;
			}
		}
		else
			_ts_snprintf(szReturnValue, TEXT("%u"), TreeView_GetItemState(m_Hwnd, item, TVIS_STATEIMAGEMASK));
	}
	break;
	// [NAME] [ID] [PROP] [PATH]
	case L"num"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getlasttoks().trim());	// tok 4, -1
		HTREEITEM item = TVI_ROOT;

		if (path != TEXT("root"))
		{
			item = parsePath(path);

			if (!item)
				throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);
		}
		_ts_snprintf(szReturnValue, TEXT("%d"), this->getChildCount(item));
	}
	break;
	// [NAME] [ID] [PROP] [PATH]
	case L"expand"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getlasttoks());	// tok 4, -1
		const auto* const item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		dcx_ConChar(TreeView_GetItemState(m_Hwnd, item, TVIS_EXPANDED) & TVIS_EXPANDED, szReturnValue);
	}
	break;
	// [NAME] [ID] [PROP]
	case L"mouseitem"_hash:
	{
		TVHITTESTINFO tvh{};

		if (!GetCursorPos(&tvh.pt))
			throw Dcx::dcxException("Unable to get cursor position");

		MapWindowPoints(nullptr, m_Hwnd, &tvh.pt, 1);

		TreeView_HitTest(m_Hwnd, &tvh);

		if (dcx_testflag(tvh.flags, TVHT_ONITEM))
			szReturnValue = getPathFromItem(tvh.hItem).to_chr();
		else
			szReturnValue = TEXT('0');
	}
	break;
	// [NAME] [ID] [PROP] [PATH]
	case L"markeditem"_hash:
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getlasttoks().trim());	// tok 4, -1
		auto item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		if (const auto lpdcxtvitem = getItemParam(item); lpdcxtvitem)
			szReturnValue = lpdcxtvitem->tsMark.to_chr();
		else
			throw Dcx::dcxException(TEXT("Unable to retrieve item: %"), path);
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::parseCommandRequest(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -r [NAME] [ID] [SWITCH]
	if (flags[TEXT('r')])
	{
		// If the window style for a tree-view control contains TVS_SCROLL and all items are deleted,
		// new items are not displayed until the window styles are reset.
		// The following code shows one way to ensure that items are always displayed.
		const auto styles = dcxGetWindowStyle(m_Hwnd);

		TreeView_DeleteAllItems(m_Hwnd);

		dcxSetWindowStyle(m_Hwnd, styles);
	}

	// xdid -a [NAME] [ID] [SWITCH] N N N ... N[TAB][+FLAGS] [#ICON] [#SICON] [#OVERLAY] [#STATE] [#INTEGRAL] [COLOR] [BKGCOLOR] Text[TAB]Tooltip Text
	if (flags[TEXT('a')])
	{
		const auto nTabs = input.numtok(TSTABCHAR);

		if ((numtok < 4) || (nTabs < 2))
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());	// tok 1
		const auto data(input.getnexttok(TSTABCHAR).trim());	// tok 2
		TString tooltip;

		if (nTabs > 2)
			tooltip = input.getlasttoks().trim();	// tok 3, -1, TSTAB

		if (data.numtok() > 8)
			this->insertItem(path, data, tooltip);
	}
	// xdid -A [NAME] [ID] [SWITCH] N N N ... N[TAB][+FLAGS] [INFO]
	else if (flags[TEXT('A')])
	{
		if ((numtok < 4) || (input.numtok(TSTABCHAR) < 2))
			throw DcxExceptions::dcxInvalidArguments();

		auto path(input.getfirsttok(1, TSTABCHAR).trim());		// tok 1
		const auto data(input.getnexttok(TSTABCHAR).trim());	// tok 2

		if (path.numtok() < 4)
			throw Dcx::dcxException("Insufficient parameters (path)");

		if (data.numtok() < 2)
			throw Dcx::dcxException("Insufficient parameters (args)");

		path = path.gettok(4, -1);

		const auto item = parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		const XSwitchFlags xflag(data.getfirsttok(1));	// tok 1
		const auto info(data.getlasttoks());		// tok 2, -1

		if (!xflag[TEXT('M')])
			throw DcxExceptions::dcxInvalidFlag();

		if (const auto lpdcxtvitem = getItemParam(item); lpdcxtvitem)
			lpdcxtvitem->tsMark = info;
		else
			throw Dcx::dcxException(TEXT("Unable to retrieve item: %"), path);
	}
	// xdid -B [NAME] [ID] [SWITCH] N N N
	else if (flags[TEXT('B')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getlasttoks());	// tok 4, -1
		const auto* const item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		TreeView_EnsureVisible(m_Hwnd, item); // make sure selected item is visible.
		TreeView_EditLabel(m_Hwnd, item);
	}
	// xdid -c [NAME] [ID] [SWITCH] N N N
	else if (flags[TEXT('c')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getlasttoks());	// tok 4, -1
		const auto* const item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		TreeView_EnsureVisible(m_Hwnd, item); // make sure selected item is visible.
		TreeView_SelectItem(m_Hwnd, item);
	}
	// xdid -d [NAME] [ID] [SWITCH] N N N
	else if (flags[TEXT('d')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getlasttoks());	// tok 4, -1
		const auto* const item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		TreeView_DeleteItem(m_Hwnd, item);
	}
	// xdid -g [NAME] [ID] [SWITCH] [HEIGHT]
	else if (flags[TEXT('g')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		if (const auto iHeight = input.getnexttok().to_int(); iHeight > -2)
			TreeView_SetItemHeight(m_Hwnd, iHeight);
	}
	// xdid -i [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if (flags[TEXT('i')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iFlags = this->parseColorFlags(input.getnexttok());	// tok 4

		const auto clr = input.getnexttok().to_<COLORREF>();	// tok 5
		if (dcx_testflag(iFlags, TVCOLOR_B))
			TreeView_SetBkColor(m_Hwnd, clr);

		if (dcx_testflag(iFlags, TVCOLOR_L))
			TreeView_SetLineColor(m_Hwnd, clr);

		if (dcx_testflag(iFlags, TVCOLOR_T))
			TreeView_SetTextColor(m_Hwnd, clr);

		if (dcx_testflag(iFlags, TVCOLOR_S))
			this->m_colSelection = clr;

		this->redrawWindow();
	}
	// xdid -j [NAME] [ID] [SWITCH] [+FLAGS] [N N N] [TAB] [ICON] [SICON] (OVERLAY)
	else if (flags[TEXT('j')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		// Invalid parameters, missing icons segment.
		if (input.numtok(TSTABCHAR) < 2)
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());
		const auto icons(input.getnexttok(TSTABCHAR).trim());	// tok 2

		// Invalid parameters, missing icon args.
		if (icons.numtok() < 2)
			throw DcxExceptions::dcxInvalidArguments();

		const auto item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		auto nIcon = icons.getfirsttok(1).to_int() - 1;
		auto sIcon = icons.getnexttok().to_int() - 1;	// tok 2
		TVITEMEX tvi{};

		tvi.mask = TVIF_HANDLE;
		tvi.hItem = item;

		/*
		nIcon/sIcon values (actual values)
		-2 = ignore icon
		-1 = no icon
		0+ = valid icon
		*/

		// overlay
		if (icons.numtok() > 2)
		{
			// overlay is 1-based index
			if (const auto oIcon = icons.getnexttok().to_int(); oIcon > -1)
				//TreeView_SetItemState(m_Hwnd, tvi.hItem, INDEXTOOVERLAYMASK(gsl::narrow_cast<UINT>(oIcon)), TVIS_OVERLAYMASK);
				TV_SetItemState(tvi.hItem, INDEXTOOVERLAYMASK(gsl::narrow_cast<UINT>(oIcon)), TVIS_OVERLAYMASK);
		}

		// if ignoring both nIcon and sIcon (if its -2 or less)
		if ((nIcon < -1) && (sIcon < -1))
			return;

		// normal icon
		if (nIcon > -2)
		{
			tvi.mask |= TVIF_IMAGE;

			// quickfix so it doesnt display an image
			// http://dcx.scriptsdb.org/bug/?do=details&id=350
			if (nIcon == -1)
				nIcon = I_IMAGENONE; //10000;

			tvi.iImage = nIcon;
		}

		// selected icon
		if (sIcon > -2)
		{
			tvi.mask |= TVIF_SELECTEDIMAGE;

			// quickfix so it doesnt display an image
			// http://dcx.scriptsdb.org/bug/?do=details&id=350
			if (sIcon == -1)
				sIcon = I_IMAGENONE; //10000;

			tvi.iSelectedImage = sIcon;
		}

		TreeView_SetItem(m_Hwnd, &tvi);
	}
	// xdid -k [NAME] [ID] [SWITCH] [STATE] N N N
	else if (flags[TEXT('k')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto state = input.getnexttok().to_<UINT>();	// tok 4
		const auto path(input.getlasttoks());		// tok 5, -1
		const auto item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		//TreeView_SetItemState(m_Hwnd, item, INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK);
		TV_SetItemState(item, INDEXTOSTATEIMAGEMASK(state), TVIS_STATEIMAGEMASK);
	}
	// xdid -l [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('l')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		m_iIconSize = NumToIconSize(input.getnexttok().to_<int>());	// tok 4
	}
	// xdid -m [NAME] [ID] [SWITCH] N N N{TAB}N N N
	else if (flags[TEXT('m')])
	{
		if ((numtok < 4) || (input.numtok(TSTABCHAR) < 2))
			throw DcxExceptions::dcxInvalidArguments();

		const auto pathFrom(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());
		const auto pathTo(input.getnexttok(TSTABCHAR).trim());	// tok 2

		const auto* const item = this->copyAllItems(pathFrom, pathTo);

		TreeView_DeleteItem(m_Hwnd, item);
	}
	// xdid -n [NAME] [ID] [SWITCH] N N N{TAB}N N N
	else if (flags[TEXT('n')])
	{
		if ((numtok < 4) || (input.numtok(TSTABCHAR) < 2))
			throw DcxExceptions::dcxInvalidArguments();

		const auto pathFrom(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());
		const auto pathTo(input.getnexttok(TSTABCHAR).trim());	// tok 2

		this->copyAllItems(pathFrom, pathTo);
	}
	// xdid -o [NAME] [ID] [SWITCH] N N N [TAB] (Tooltip Text)
	else if (flags[TEXT('o')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());
		TString tiptext;

		if (input.numtok(TSTABCHAR) > 1)
			tiptext = input.getnexttok(TSTABCHAR).trim();	// tok 2

		const auto item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		if (const auto lpdcxtvitem = getItemParam(item); lpdcxtvitem)
			lpdcxtvitem->tsTipText = tiptext;
	}
	// xdid -Q [NAME] [ID] [SWITCH] [+FLAGS] [COLOR] N N N
	else if (flags[TEXT('Q')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iFlags = this->parseItemFlags(input.getnexttok());		// tok 4
		const auto clrText = input.getnexttok().to_<COLORREF>();	// tok 5
		const auto path(input.getlasttoks());							// tok 6, -1
		const auto item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		if (const auto lpdcxtvitem = getItemParam(item); lpdcxtvitem)
		{
			lpdcxtvitem->bUline = dcx_testflag(iFlags, TVIS_UNDERLINE);

			lpdcxtvitem->bBold = dcx_testflag(iFlags, TVIS_BOLD);

			lpdcxtvitem->bItalic = dcx_testflag(iFlags, TVIS_ITALIC);

			if (dcx_testflag(iFlags, TVIS_COLOR))
				lpdcxtvitem->clrText = clrText;
			else
				lpdcxtvitem->clrText = CLR_INVALID;
		}
		else
			throw Dcx::dcxException(TEXT("Unable to retrieve item: %"), path);

		redrawWindow();
	}
	// This is to avoid an invalid error message.
	// xdid -r [NAME] [ID] [SWITCH]
	else if (flags[TEXT('r')])
	{
	}
	// xdid -t [NAME] [ID] [SWITCH] [+FLAGS] N N N
	else if (flags[TEXT('t')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iFlags = this->parseToggleFlags(input.getnexttok());	// tok 4
		const auto path(input.getlasttoks());							// tok 5, -1

		if (path == TEXT("root"))
		{
			HTREEITEM hStart = TVI_ROOT;

			if (dcx_testflag(iFlags, TVIE_EXPALL))
				this->expandAllItems(hStart, TVE_EXPAND);
			else if (dcx_testflag(iFlags, TVIE_COLALL))
				this->expandAllItems(hStart, TVE_COLLAPSE);

			return;
		}

		const auto* const item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		if (dcx_testflag(iFlags, TVIE_EXP))
			TreeView_Expand(m_Hwnd, item, TVE_EXPAND);
		else if (dcx_testflag(iFlags, TVIE_EXPPART))
			TreeView_Expand(m_Hwnd, item, TVE_EXPAND | TVE_EXPANDPARTIAL);
		else if (dcx_testflag(iFlags, TVIE_COL))
			TreeView_Expand(m_Hwnd, item, TVE_COLLAPSE);
		else if (dcx_testflag(iFlags, TVIE_COLRES))
			TreeView_Expand(m_Hwnd, item, TVE_COLLAPSE | TVE_COLLAPSERESET);
		else if (dcx_testflag(iFlags, TVIE_TOGGLE))
			TreeView_Expand(m_Hwnd, item, TVE_TOGGLE);
	}
	// xdid -u [NAME] [ID] [SWITCH]
	else if (flags[TEXT('u')])
	{
		TreeView_SelectItem(m_Hwnd, nullptr);
	}
	// xdid -v [NAME] [ID] [SWITCH] N N N [TAB] (Item Text)
	else if (flags[TEXT('v')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto path(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());
		TString itemtext;

		if (input.numtok(TSTABCHAR) > 1)
			itemtext = input.getnexttok(TSTABCHAR).trim();	// tok 2

		const auto item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Invalid Path: %"), path);

		TVITEMEX tvi{};

		tvi.mask = TVIF_TEXT | TVIF_HANDLE;
		tvi.hItem = item;
		tvi.pszText = itemtext.to_chr();

		TreeView_SetItem(m_Hwnd, &tvi);
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		const auto tsFlags(input.getnexttok());	// tok 4
		const auto iFlags = this->parseIconFlagOptions(tsFlags);

		HICON icon{ nullptr };

		const auto index = input.getnexttok().to_int();	// tok 5
		auto filename(input.getlasttoks());			// tok 6, -1
		const auto bLarge = (m_iIconSize != DcxIconSizes::SmallIcon);

		if (index >= 0)
			icon = dcxLoadIcon(index, filename, bLarge, tsFlags);

		if (dcx_testflag(iFlags, TVIT_NORMAL))
		{
			if (auto himl = TV_GetNormalImageList(); himl)
			{
				if (index < 0)
					AddFileIcons(himl, filename, bLarge, -1);
				else if (const auto i = ImageList_AddIcon(himl, icon); tsFlags.find(TEXT('o'), 0))
				{
					// overlay image
					const auto io = tsFlags.find(TEXT('o'), 1) + 1;
					const auto o = tsFlags.mid(io, (gsl::narrow_cast<int>(tsFlags.len()) - io)).to_int();

					if (o < 1 || o > 15)
						throw Dcx::dcxException("Overlay index out of range (1 -> 15)");

					if (o > 0)
						ImageList_SetOverlayImage(himl, i, o);
				}
			}
		}

		if (dcx_testflag(iFlags, TVIT_STATE))
		{
			if (auto himl = TV_GetStateImageList(); himl)
			{
				if (index < 0)
					AddFileIcons(himl, filename, bLarge, -1);
				else
					ImageList_AddIcon(himl, icon);
			}
		}
		if (icon)
			DestroyIcon(icon);
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if (flags[TEXT('y')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto iFlags = this->parseIconFlagOptions(input.getnexttok());	// tok 4

		if (dcx_testflag(iFlags, TVIT_NORMAL))
			TV_RemoveNormalImageList();

		if (dcx_testflag(iFlags, TVIT_STATE))
			TV_RemoveStateImageList();
	}
	// xdid -z [NAME] [ID] [SWITCH] [+FLAGS] N N N [TAB] [ALIAS]
	else if (flags[TEXT('z')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		auto dtvs = std::make_unique<DCXTVSORT>();
		TVSORTCB tvs{};

		dtvs->iSortFlags = this->parseSortFlags(input.getnexttok());	// tok 4
		dtvs->pthis = this;

		const auto path(input.getfirsttok(1, TSTABCHAR).gettok(5, -1).trim());	// tok 1, TSTAB

		if (input.numtok(TSTABCHAR) > 1)
			dtvs->tsCustomAlias = input.getnexttok(TSTABCHAR).trim();	// tok 2, TSTAB

		tvs.lpfnCompare = DcxTreeView::sortItemsEx;
		tvs.lParam = reinterpret_cast<LPARAM>(dtvs.get());

		if (path == TEXT("root"))
			tvs.hParent = TVI_ROOT;
		else {

			const auto item = this->parsePath(path);

			if (!item)
				throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

			tvs.hParent = item;
		}
		if (dcx_testflag(dtvs->iSortFlags, TVSS_ALL))
			TreeView_SortChildrenCB(m_Hwnd, &tvs, TRUE); // NB: doesnt recurse!! This is a OS problem.
		else if (dcx_testflag(dtvs->iSortFlags, TVSS_SINGLE))
			TreeView_SortChildrenCB(m_Hwnd, &tvs, FALSE);
	}
	// xdid -G [NAME] [ID] [SWITCH] [+FLAGS] [X] [Y] [FILENAME|none]
	else if (flags[TEXT('G')])
	{
		if (numtok < 7)
			throw DcxExceptions::dcxInvalidArguments();

		const auto flag(input.getnexttok());			// tok 4
		this->m_iXOffset = input.getnexttok().to_int();	// tok 5
		this->m_iYOffset = input.getnexttok().to_int();	// tok 6
		auto filename(input.getlasttoks());				// tok 7, -1

		if (!this->m_bTransparent)
			this->removeExStyle(WindowExStyle::Transparent);

		if (filename != TEXT("none"))
		{
			if (!Dcx::GDIModule.isUseable())
				throw Dcx::dcxException("GDI+ Not Supported On This Machine");

			LoadGDIPlusImage(flag, filename);

			if (!this->m_bTransparent)
				this->setExStyle(WindowExStyle::Transparent);
		}
		this->redrawWindow();
	}
	// xdid -S [NAME] [ID] [SWITCH] [N (N...)][TAB][+FLAGS] [NAME] [FILENAME]
	else if (flags[TEXT('S')])
	{
		if (numtok < 6)
			throw DcxExceptions::dcxInvalidArguments();

		if (input.numtok(TSTABCHAR) != 2)
			throw Dcx::dcxException("Invalid Command Syntax.");

		const auto path(input.getfirsttok(1, TSTABCHAR).gettok(4, -1).trim());	// tok 1, TSTAB
		const auto fileData(input.getnexttok(TSTABCHAR));						// tok 2, TSTAB

		if (fileData.numtok() < 3)
			throw Dcx::dcxException("Invalid Command Syntax.");

		const auto name(fileData.getfirsttok(2).trim());		// tok 2
		auto filename(fileData.getlasttoks().trim());		// tok 3, -1

		if (name.empty())
			throw Dcx::dcxException("Invalid dataset");

		if (path.empty())
			throw Dcx::dcxException("Invalid path");

		const auto item = this->parsePath(path);

		if (!item)
			throw Dcx::dcxException(TEXT("Unable to parse path: %"), path);

		this->xmlSaveTree(item, name, filename);
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */

GSL_SUPPRESS(lifetime)
HIMAGELIST DcxTreeView::getImageList(const int type) const noexcept
{
	return TreeView_GetImageList(m_Hwnd, gsl::narrow_cast<WPARAM>(type));
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::setImageList(const HIMAGELIST himl, const int type) noexcept
{
	if (const auto o = TreeView_SetImageList(m_Hwnd, himl, gsl::narrow_cast<WPARAM>(type)); (o && o != himl)) // don't destroy if nullptr or the same list as just added.
		ImageList_Destroy(o);
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxTreeView::createImageList() noexcept
{
	return ImageList_Create(gsl::narrow_cast<int>(m_iIconSize), gsl::narrow_cast<int>(m_iIconSize), ILC_COLOR32 | ILC_MASK, 1, 0);
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::insertItem(const TString& tsPath, const TString& tsData, const TString& tsTooltip)
{
	HTREEITEM hParent = TVI_ROOT;
	HTREEITEM hAfter = TVI_ROOT;
	HTREEITEM hItem = nullptr;

	TVITEMEX tvi{};
	TVINSERTSTRUCT tvins{};

	tvi.mask = TVIF_STATE | TVIF_PARAM;

	auto iFlags = this->parseItemFlags(tsData.getfirsttok(1));	// tok 1
	const auto icon = tsData.getnexttok().to_int() - 1;			// tok 2
	const auto sicon = tsData.getnexttok().to_int() - 1;			// tok 3
	const auto overlay = tsData.getnexttok().to_int();				// tok 4
	const auto state = tsData.getnexttok().to_int();				// tok 5
	const auto integral = tsData.getnexttok().to_int() + 1;			// tok 6
	const auto clrText = tsData.getnexttok().to_<COLORREF>();	// tok 7
	const auto clrBkg = tsData.getnexttok().to_<COLORREF>();	// tok 8

	// text
	auto itemtext(tsData.getlasttoks());							// tok 9, -1

	// path
	if (!this->parsePath(tsPath, &hParent, &hAfter))
		return;

	if (dcx_testflag(iFlags, TVIS_XML))
	{
		const auto tsName(itemtext.getfirsttok(1));		// tok 1
		auto filename(itemtext.getlasttoks());			// tok 2, -1
		xmlLoadTree(hAfter, hParent, tsName, filename);
		return;
	}

	// parse DCX parameters
	auto lpmytvi = std::make_unique<DCXTVITEM>();

	lpmytvi->tsTipText = tsTooltip;

	lpmytvi->bUline = dcx_testflag(iFlags, TVIS_UNDERLINE);

	lpmytvi->bBold = dcx_testflag(iFlags, TVIS_BOLD);

	lpmytvi->bItalic = dcx_testflag(iFlags, TVIS_ITALIC);

	lpmytvi->clrText = (dcx_testflag(iFlags, TVIS_COLOR)) ? clrText : CLR_INVALID;

	lpmytvi->clrBkg = (dcx_testflag(iFlags, TVIS_BKG)) ? clrBkg : CLR_INVALID;

	{
		//if ((dcx_testflag(iFlags, TVIS_HASHITEM)) && (itemtext.numtok() == 2))
		//	mIRCLinker::tsEvalex(itemtext, TEXT("$hget(%s,%s)"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
		//else if ((dcx_testflag(iFlags, TVIS_HASHNUMBER)) && (itemtext.numtok() == 2))
		//	mIRCLinker::tsEvalex(itemtext,  TEXT("$hget(%s,%s).data"), itemtext.gettok( 1 ).to_chr(), itemtext.gettok( 2 ).to_chr());
		if ((dcx_testflag(iFlags, TVIS_HASHITEM)) && (itemtext.numtok() == 2))
			mIRCLinker::eval(itemtext, TEXT("$hget(%,%)"), itemtext.getfirsttok(1), itemtext.getnexttok());
		else if ((dcx_testflag(iFlags, TVIS_HASHNUMBER)) && (itemtext.numtok() == 2))
			mIRCLinker::eval(itemtext, TEXT("$hget(%,%).data"), itemtext.getfirsttok(1), itemtext.getnexttok());
	}

	if (!itemtext.empty())
	{
		tvi.pszText = itemtext.to_chr();
		tvi.cchTextMax = gsl::narrow_cast<int>(itemtext.len());
		tvi.mask |= TVIF_TEXT;
	}
	// icons
	// http://dcx.scriptsdb.org/bug/?do=details&id=350
	tvi.iImage = I_IMAGENONE;
	tvi.iSelectedImage = I_IMAGENONE;

	tvi.mask |= TVIF_IMAGE;
	if (icon > -1)
		tvi.iImage = icon;

	tvi.mask |= TVIF_SELECTEDIMAGE;
	if (sicon > -1)
		tvi.iSelectedImage = sicon;

	// test code for expanded image...
	tvi.iExpandedImage = I_IMAGENONE;
	if (icon > -1)
	{
		tvi.mask |= TVIF_EXPANDEDIMAGE;
		tvi.iExpandedImage = icon;
	}

	if (integral > 1)
	{
		tvi.iIntegral = integral;
		tvi.mask |= TVIF_INTEGRAL;
	}
	else
		tvi.iIntegral = 1;
	
	// TODO: Add Expanded Image support (TVIF_EXPANDEDIMAGE)

	iFlags &= ~TVIS_DCXMASK; // exclude DCX flags, they were messing up state & overlay icon settings, found when saved data didnt match what was set.

	//tvi.hItem = hAfter;
	tvi.state = iFlags;
	tvi.stateMask = iFlags;
	tvi.lParam = reinterpret_cast<LPARAM>(lpmytvi.get());

	tvins.itemex = tvi;
	tvins.hInsertAfter = hAfter;
	tvins.hParent = hParent;

	hItem = TreeView_InsertItem(m_Hwnd, &tvins);
	if (!hItem)
		throw Dcx::dcxException("Unable to add Item");

	lpmytvi->hHandle = hItem;
	lpmytvi.release();

	if (state > -1 && state < 5) // zero is no state image.
		//TreeView_SetItemState(m_Hwnd, hItem, INDEXTOSTATEIMAGEMASK(gsl::narrow_cast<UINT>(state)), TVIS_STATEIMAGEMASK);
		TV_SetItemState(hItem, INDEXTOSTATEIMAGEMASK(gsl::narrow_cast<UINT>(state)), TVIS_STATEIMAGEMASK);

	// overlay is 1-based index
	if (overlay > 0 && overlay < 16)
		//TreeView_SetItemState(m_Hwnd, hItem, INDEXTOOVERLAYMASK(gsl::narrow_cast<UINT>(overlay)), TVIS_OVERLAYMASK);
		TV_SetItemState(hItem, INDEXTOOVERLAYMASK(gsl::narrow_cast<UINT>(overlay)), TVIS_OVERLAYMASK);

	return;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseIconFlagOptions(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	UINT iFlags{};

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('n')])
		iFlags |= TVIT_NORMAL;
	if (xflags[TEXT('s')])
		iFlags |= TVIT_STATE;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseItemFlags(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('b')])
		iFlags |= TVIS_BOLD;
	if (xflags[TEXT('c')])
		iFlags |= TVIS_COLOR;
	if (xflags[TEXT('e')])
		iFlags |= TVIS_EXPANDED;
	if (xflags[TEXT('i')])
		iFlags |= TVIS_ITALIC;
	if (xflags[TEXT('s')])
		iFlags |= TVIS_SELECTED;
	if (xflags[TEXT('u')])
		iFlags |= TVIS_UNDERLINE;
	if (xflags[TEXT('g')])
		iFlags |= TVIS_BKG;
	if (xflags[TEXT('H')])
		iFlags |= TVIS_HASHITEM;
	if (xflags[TEXT('n')])
		iFlags |= TVIS_HASHNUMBER;
	if (xflags[TEXT('x')])
		iFlags |= TVIS_XML;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseSortFlags(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('a')])
		iFlags |= TVSS_ASC;
	if (xflags[TEXT('b')])
		iFlags |= TVSS_SINGLE;
	if (xflags[TEXT('c')])
		iFlags |= TVSS_CUSTOM;
	if (xflags[TEXT('d')])
		iFlags |= TVSS_DESC;
	if (xflags[TEXT('n')])
		iFlags |= TVSS_NUMERIC;
	if (xflags[TEXT('r')])
		iFlags |= TVSS_ALL;
	if (xflags[TEXT('s')])
		iFlags |= TVSS_CASE;
	if (xflags[TEXT('t')])
		iFlags |= TVSS_ALPHA;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseColorFlags(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('b')])
		iFlags |= TVCOLOR_B;
	if (xflags[TEXT('l')])
		iFlags |= TVCOLOR_L;
	if (xflags[TEXT('s')])
		iFlags |= TVCOLOR_S;
	if (xflags[TEXT('t')])
		iFlags |= TVCOLOR_T;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxTreeView::parseToggleFlags(const TString& flags) noexcept
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('a')])
		iFlags |= TVIE_EXPALL;
	if (xflags[TEXT('c')])
		iFlags |= TVIE_COL;
	if (xflags[TEXT('e')])
		iFlags |= TVIE_EXP;
	if (xflags[TEXT('p')])
		iFlags |= TVIE_EXPPART;
	if (xflags[TEXT('r')])
		iFlags |= TVIE_COLRES;
	if (xflags[TEXT('t')])
		iFlags |= TVIE_TOGGLE;
	if (xflags[TEXT('z')])
		iFlags |= TVIE_COLALL;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

int CALLBACK DcxTreeView::sortItemsEx(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	const auto ptvsort = reinterpret_cast<LPDCXTVSORT>(lParamSort);
	const auto lptvi1 = reinterpret_cast<LPDCXTVITEM>(lParam1);
	const auto lptvi2 = reinterpret_cast<LPDCXTVITEM>(lParam2);

	if ((!ptvsort) || (!lptvi1) || (!lptvi2))
		return 0;

	ptvsort->pthis->getItemText(lptvi1->hHandle, &ptvsort->itemtext1[0], MIRC_BUFFER_SIZE_CCH);
	ptvsort->pthis->getItemText(lptvi2->hHandle, &ptvsort->itemtext2[0], MIRC_BUFFER_SIZE_CCH);

	// CUSTOM Sort
	if (dcx_testflag(ptvsort->iSortFlags, TVSS_CUSTOM))
	{
		if (ptvsort->tsCustomAlias.empty())
			return 0;

		//TCHAR sRes[20];
		//mIRCLinker::evalex( sRes, static_cast<int>(sRes.size()), TEXT("$%s(%s,%s)"), ptvsort->tsCustomAlias.to_chr( ), &ptvsort->itemtext1[0], &ptvsort->itemtext2[0] );

		stString<20> sRes;
		mIRCLinker::eval(sRes, TEXT("$%(%,%)"), ptvsort->tsCustomAlias, &ptvsort->itemtext1[0], &ptvsort->itemtext2[0]);

		auto ires = dcx_atoi(sRes.data());

		if (ires < -1)
			ires = -1;
		else if (ires > 1)
			ires = 1;

		if (dcx_testflag(ptvsort->iSortFlags, TVSS_DESC))
			return ires;
		else {

			if (ires == -1)
				return 1;
			else if (ires == 1)
				return -1;
		}
	}
	// NUMERIC Sort
	else if (dcx_testflag(ptvsort->iSortFlags, TVSS_NUMERIC))
	{
		const auto i1 = dcx_atoi(&ptvsort->itemtext1[0]);
		const auto i2 = dcx_atoi(&ptvsort->itemtext2[0]);

		if (dcx_testflag(ptvsort->iSortFlags, TVSS_DESC))
		{
			if (i1 < i2)
				return 1;
			else if (i1 > i2)
				return -1;
		}
		else {
			if (i1 < i2)
				return -1;
			else if (i1 > i2)
				return 1;
		}
	}
	// Default ALPHA Sort
	else {
		if (dcx_testflag(ptvsort->iSortFlags, TVSS_DESC))
		{
			if (dcx_testflag(ptvsort->iSortFlags, TVSS_CASE))
				return -ts_strcmp(&ptvsort->itemtext1[0], &ptvsort->itemtext2[0]);

			return -ts_stricmp(&ptvsort->itemtext1[0], &ptvsort->itemtext2[0]);
		}
		else {
			if (dcx_testflag(ptvsort->iSortFlags, TVSS_CASE))
				return ts_strcmp(&ptvsort->itemtext1[0], &ptvsort->itemtext2[0]);

			return ts_stricmp(&ptvsort->itemtext1[0], &ptvsort->itemtext2[0]);
		}
	}

	return 0;
}

/*!
 * \brief blah
 *
 * blah
 */
TString DcxTreeView::getPathFromItem(const HTREEITEM item) const
{
	//HTREEITEM parent = item;
	//VectorOfInts vec;
	//
	// Loop until we reach the topmost node of the tree.
	//do {
	//	int i = 1;
	//	HTREEITEM current = parent;
	//
	//	// Count each previous node before the node containing our item.
	//	while ((current = TreeView_GetPrevSibling(m_Hwnd, current)) != nullptr)
	//		++i;
	//
	//	vec.push_back(i);
	//} while ((parent = TreeView_GetParent(m_Hwnd, parent)) != nullptr);


	VectorOfInts vec;
	vec.reserve(this->size());

	// Loop until we reach the topmost node of the tree.
	for (HTREEITEM parent = item; parent; parent = TreeView_GetParent(m_Hwnd, parent))
	{
		int i{};

		// Count each previous node before the node containing our item.
		for (HTREEITEM current = parent; current; current = TreeView_GetPrevSibling(m_Hwnd, current))
			++i;

		vec.push_back(i);
	}

	// Construct the string containing the path backwards, as we traced it backwards.

	TString result;

	// reverse iterator allows us to use addtok()
	for (auto itStart = vec.rbegin(), itEnd = vec.rend(); itStart != itEnd; ++itStart)
		result.addtok(*itStart);

	// Trim to ensure clean path.
	return result.trim();
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::getItemText(const HTREEITEM hItem, TCHAR* szBuffer, const int cchTextMax) const noexcept
{
	TVITEMEX tvi{};

	tvi.hItem = hItem;
	tvi.mask = TVIF_TEXT | TVIF_HANDLE;
	tvi.cchTextMax = cchTextMax;
	tvi.pszText = szBuffer;

	if (!TreeView_GetItem(m_Hwnd, &tvi))
		szBuffer[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */
TString DcxTreeView::getItemText(const HTREEITEM hItem) const
{
	//TVITEMEX tvi{};
	//TCHAR sBuf[MIRC_BUFFER_SIZE_CCH]{};
	//tvi.hItem = hItem;
	//tvi.mask = TVIF_TEXT | TVIF_HANDLE;
	//tvi.cchTextMax = std::size(sBuf);
	//tvi.pszText = &sBuf[0];
	//if (!TreeView_GetItem(m_Hwnd, &tvi))
	//	sBuf[0] = 0;
	//return &sBuf[0];

	TCHAR sBuf[MIRC_BUFFER_SIZE_CCH]{};

	getItemText(hItem, &sBuf[0], std::size(sBuf));

	return &sBuf[0];
}

/*
 * Returns the number of children a node has.
 */
int DcxTreeView::getChildCount(const HTREEITEM hParent) const noexcept
{
	//auto i = 0;
	//auto hItem = TreeView_GetChild(m_Hwnd, hParent);
	//
	//// No children
	//if (!hItem)
	//	return 0;
	//
	//// Child found
	//++i;
	//
	//// Iterate through sibling nodes until reaching the end
	//while ((hItem = TreeView_GetNextSibling(m_Hwnd, hItem)) != nullptr)
	//	++i;
	//
	//return i;

	int i{};
	for (auto hItem = TreeView_GetChild(m_Hwnd, hParent); hItem; hItem = TreeView_GetNextSibling(m_Hwnd, hItem))
		++i;
	return i;
}

GSL_SUPPRESS(lifetime)
LPDCXTVITEM DcxTreeView::getItemParam(const HTREEITEM hItem) const noexcept
{
	TVITEMEX tvi{};
	tvi.hItem = hItem;
	tvi.mask = TVIF_HANDLE | TVIF_PARAM;

	if (!TreeView_GetItem(m_Hwnd, &tvi))
		return nullptr;

	return reinterpret_cast<LPDCXTVITEM>(tvi.lParam);
}

int DcxTreeView::getItemImageID(const HTREEITEM hItem) const noexcept
{
	TVITEMEX tvi{};
	tvi.hItem = hItem;
	tvi.mask = TVIF_HANDLE | TVIF_IMAGE;

	if (!TreeView_GetItem(m_Hwnd, &tvi))
		return I_IMAGENONE;

	return tvi.iImage;
}

/*!
 * \brief blah
 *
 * blah
 */

GSL_SUPPRESS(bounds.4)
GSL_SUPPRESS(con.4)
GSL_SUPPRESS(r.5)
bool DcxTreeView::matchItemText(const HTREEITEM hItem, const TString& search, const DcxSearchTypes& SearchType) const
{
	auto itemtext = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);
	gsl::at(itemtext, 0) = TEXT('\0');

	getItemText(hItem, itemtext.get(), MIRC_BUFFER_SIZE_CCH);

	return DcxListHelper::matchItemText(itemtext.get(), search, SearchType);

	//auto itemtext = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);
	//gsl::at(itemtext, 0) = TEXT('\0');
	//
	//auto refText = mIRCResultString(itemtext.get());
	//
	//getItemText(hItem, refText, MIRC_BUFFER_SIZE_CCH);
	//
	//return DcxListHelper::matchItemText(refText, search, SearchType);
}

std::optional<HTREEITEM> DcxTreeView::findItemText(const HTREEITEM hStart, const TString& queryText, const int n, int& matchCount, const DcxSearchTypes& searchType) const
{
	//HTREEITEM item = TreeView_GetChild(m_Hwnd, hStart);
	//
	//// Check if it should search child nodes
	//if (item == nullptr)
	//	return { };
	//
	//do {
	//	if (this->matchItemText(item, queryText, searchType))
	//		++matchCount;
	//
	//	if (n != 0 && matchCount == n)
	//		return { item };
	//
	//	if (const auto result_item = findItemText(item, queryText, n, matchCount, searchType); result_item.has_value())
	//		return result_item;
	//
	//} while ((item = TreeView_GetNextSibling(m_Hwnd, item)) != nullptr);
	//
	//return { };

	for (HTREEITEM item = TreeView_GetChild(m_Hwnd, hStart); item; item = TreeView_GetNextSibling(m_Hwnd, item))
	{
		if (this->matchItemText(item, queryText, searchType))
			++matchCount;

		if (n != 0 && matchCount == n)
			return { item };

		if (const auto result_item = findItemText(item, queryText, n, matchCount, searchType); result_item.has_value())
			return result_item;
	}

	return {};
}

bool DcxTreeView::matchItemText(const HTREEITEM hItem, const dcxSearchData& srch_data) const
{
	auto itemtext = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);
	gsl::at(itemtext, 0) = TEXT('\0');

	getItemText(hItem, itemtext.get(), MIRC_BUFFER_SIZE_CCH);

	return DcxListHelper::matchItemText(itemtext.get(), srch_data);
}

std::optional<HTREEITEM> DcxTreeView::findItemText(const HTREEITEM hStart, const int n, int& matchCount, const dcxSearchData& srch_data) const
{
	for (HTREEITEM item = TreeView_GetChild(m_Hwnd, hStart); item; item = TreeView_GetNextSibling(m_Hwnd, item))
	{
		if (this->matchItemText(item, srch_data))
			++matchCount;

		if (n != 0 && matchCount == n)
			return { item };

		if (const auto result_item = findItemText(item, n, matchCount, srch_data); result_item.has_value())
			return result_item;
	}

	return {};
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::expandAllItems(const HTREEITEM hStart, const UINT expandOption) noexcept
{
	//HTREEITEM hCurrentItem = TreeView_GetChild(m_Hwnd, hStart);
	//
	//if (hCurrentItem == nullptr)
	//	return;
	//
	//do {
	//	expandAllItems(hCurrentItem, expandOption);
	//	TreeView_Expand(m_Hwnd, hCurrentItem, expandOption);
	//
	//} while ((hCurrentItem = TreeView_GetNextSibling(m_Hwnd, hCurrentItem)) != nullptr);

	for (HTREEITEM hCurrentItem = TreeView_GetChild(m_Hwnd, hStart); hCurrentItem; hCurrentItem = TreeView_GetNextSibling(m_Hwnd, hCurrentItem))
	{
		expandAllItems(hCurrentItem, expandOption);
		TreeView_Expand(m_Hwnd, hCurrentItem, expandOption);
	}

	//Dcx::dcxTreeItem tr(m_Hwnd, hStart);
	//for (const auto& hCurrentItem : tr)
	//{
	//	expandAllItems(hCurrentItem, expandOption);
	//	TreeView_Expand(m_Hwnd, hCurrentItem, expandOption);
	//}
}

/*!
 * \brief blah
 *
 * blah
 */

HTREEITEM DcxTreeView::cloneItem(const HTREEITEM hItem, const HTREEITEM hParentTo, const HTREEITEM hAfterTo)
{
	// Move the root node
	TCHAR itemtext[MIRC_BUFFER_SIZE_CCH]{};
	TVITEMEX tvi{};

	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE | TVIF_STATE | TVIF_INTEGRAL | TVIF_PARAM | TVIF_HANDLE;
	tvi.hItem = hItem;
	tvi.pszText = &itemtext[0];
	tvi.cchTextMax = MIRC_BUFFER_SIZE_CCH;

	if (!TreeView_GetItem(m_Hwnd, &tvi))
		return nullptr;

	const auto* const lpdcxtvitem = reinterpret_cast<LPDCXTVITEM>(tvi.lParam);

	if (!lpdcxtvitem)
		return nullptr;

	auto lpdcxtvitem2 = std::make_unique<DCXTVITEM>(*lpdcxtvitem);

	tvi.hItem = nullptr;
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE | TVIF_STATE | TVIF_INTEGRAL | TVIF_PARAM;
	tvi.lParam = reinterpret_cast<LPARAM>(lpdcxtvitem2.get());

	TVINSERTSTRUCT tvin{ hParentTo, hAfterTo, tvi };

	const auto item = TreeView_InsertItem(m_Hwnd, &tvin);
	if (item)
		lpdcxtvitem2.release();

	return item;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTreeView::copyAllItems(const HTREEITEM hItem, const HTREEITEM hParentTo)
{
	//HTREEITEM hCurrentItem = TreeView_GetChild(m_Hwnd, hItem);
	//HTREEITEM hAfterTo = TVI_LAST;
	//
	//if (!hCurrentItem)
	//	return;
	//
	//do {
	//	if (const auto hNewItem = cloneItem(hCurrentItem, hParentTo, hAfterTo); hNewItem)
	//		copyAllItems(hCurrentItem, hNewItem);
	//
	//} while ((hCurrentItem = TreeView_GetNextSibling(m_Hwnd, hCurrentItem)) != nullptr);

	HTREEITEM hAfterTo = TVI_LAST;
	for (HTREEITEM hCurrentItem = TreeView_GetChild(m_Hwnd, hItem); hCurrentItem; hCurrentItem = TreeView_GetNextSibling(m_Hwnd, hCurrentItem))
	{
		if (const auto hNewItem = cloneItem(hCurrentItem, hParentTo, hAfterTo); hNewItem)
			copyAllItems(hCurrentItem, hNewItem);
	}
}

HTREEITEM DcxTreeView::copyAllItems(const TString& pathFrom, const TString& pathTo)
{
	auto item = this->parsePath(pathFrom);

	if (!item)
		throw Dcx::dcxException(TEXT("Invalid FROM Path: %"), pathFrom);

	HTREEITEM hParentTo = TVI_ROOT;
	HTREEITEM hAfterTo = TVI_ROOT;

	if (!this->parsePath(pathTo, &hParentTo, &hAfterTo))
		throw Dcx::dcxException(TEXT("Invalid TO Path: %"), pathTo);

	// Check if we're moving parent into a child branch
	if ((hParentTo != TVI_FIRST) && (hParentTo != TVI_LAST))
	{
		auto tmp = hParentTo;

		// Trace back all the way to root
		while (tmp && tmp != TVI_ROOT)
		{
			if (tmp == item)
				throw Dcx::dcxException(TEXT("Cannot copy parent node (%) to child node (%)."), pathFrom, pathTo);

			tmp = TreeView_GetParent(m_Hwnd, tmp);
		}
	}

	const auto hNewItem = this->cloneItem(item, hParentTo, hAfterTo);

	if (!hNewItem)
		throw Dcx::dcxException("Unable to move item.");

	this->copyAllItems(item, hNewItem);
	return item;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxTreeView::ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		dcxlParam(const LPNMHDR, hdr);

		if (!hdr)
			break;

		switch (hdr->code)
		{
		case NM_CLICK:
		{
			////http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/treeview/reflist.asp
			//	TVHITTESTINFO tvh;
			//	if (!GetCursorPos(&tvh.pt))
			//		break;
			//
			//	MapWindowPoints(nullptr, m_Hwnd, &tvh.pt, 1);
			//	TreeView_HitTest(m_Hwnd, &tvh);
			//
			//	if (dcx_testflag(tvh.flags,TVHT_ONITEMBUTTON))
			//	{
			//		if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			//			execAliasEx(TEXT("buttonclick,%u,%s"), getUserID(), getPathFromItem(&tvh.hItem).to_chr());
			//	}
			//	else if (dcx_testflag(tvh.flags, TVHT_ONITEMSTATEICON)) {
			//		if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			//		{
			//			const auto path(getPathFromItem(&tvh.hItem));
			//			if (isStyle(TVS_CHECKBOXES))
			//				execAliasEx(TEXT("stateclick,%u,%u,%s"), getUserID(), (TreeView_GetCheckState(m_Hwnd, tvh.hItem) == 0 ? 2U : 1U) , path.to_chr());
			//			else
			//				execAliasEx(TEXT("stateclick,%u,%u,%s"), getUserID(), TreeView_GetItemState(m_Hwnd, tvh.hItem, TVIS_STATEIMAGEMASK), path.to_chr());
			//		}
			//	}
			//	else if ((tvh.flags & TVHT_ONITEM) != 0) // dont use dcx_testflag() here as TVHT_ONITEM is a combination of several hit types.
			//	{
			//		//TreeView_SelectItem(m_Hwnd, tvh.hItem);	// why is this here? selections shows up fine without it
			//
			//		if (dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
			//			execAliasEx(TEXT("sclick,%u,%s"), getUserID(), getPathFromItem(&tvh.hItem).to_chr());
			//	}
			//	// single click not on item
			//	else if (dcx_testflag(tvh.flags, TVHT_NOWHERE) || dcx_testflag(tvh.flags, TVHT_ONITEMRIGHT)) {
			//		if (dcx_testflag(getParentDialog()->getEventMask(),DCX_EVENT_CLICK))
			//			execAliasEx(TEXT("sclick,%u"), getUserID());
			//	}
			//
			//	bParsed = TRUE;
			//	break;

			//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/treeview/reflist.asp

			if (!dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
				break;

			TVHITTESTINFO tvh{};
			if (!GetCursorPos(&tvh.pt))
				break;

			MapWindowPoints(nullptr, m_Hwnd, &tvh.pt, 1);
			TreeView_HitTest(m_Hwnd, &tvh);

			if (dcx_testflag(tvh.flags, TVHT_ONITEMBUTTON))
				execAliasEx(TEXT("buttonclick,%u,%s"), getUserID(), getPathFromItem(tvh.hItem).to_chr());
			else if (dcx_testflag(tvh.flags, TVHT_ONITEMSTATEICON))
			{
				if (const auto path(getPathFromItem(tvh.hItem)); isStyle(WindowStyle::TVS_CheckBoxes))
					execAliasEx(TEXT("stateclick,%u,%u,%s"), getUserID(), (TreeView_GetCheckState(m_Hwnd, tvh.hItem) == 0 ? 2U : 1U), path.to_chr());
				else
					execAliasEx(TEXT("stateclick,%u,%u,%s"), getUserID(), TreeView_GetItemState(m_Hwnd, tvh.hItem, TVIS_STATEIMAGEMASK), path.to_chr());
			}
			else if ((tvh.flags & TVHT_ONITEM) != 0) // dont use dcx_testflag() here as TVHT_ONITEM is a combination of several hit types.
				execAliasEx(TEXT("sclick,%u,%s"), getUserID(), getPathFromItem(tvh.hItem).to_chr());
			// single click not on item
			else if (dcx_testflag(tvh.flags, TVHT_NOWHERE) || dcx_testflag(tvh.flags, TVHT_ONITEMRIGHT))
				execAliasEx(TEXT("sclick,%u"), getUserID());

			bParsed = TRUE;
			break;
		}

		case NM_DBLCLK:
		{
			if (!dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
				break;

			TVHITTESTINFO tvh{};

			if (!GetCursorPos(&tvh.pt))
				break;

			MapWindowPoints(nullptr, m_Hwnd, &tvh.pt, 1);
			TreeView_HitTest(m_Hwnd, &tvh);

			//|| ( (dcx_testflag(tvh.flags, TVHT_ONITEMRIGHT)) && this->isStyle( TVS_FULLROWSELECT ) ) )
			if ((tvh.flags & TVHT_ONITEM) != 0) // dont use dcx_testflag() here as TVHT_ONITEM is a combination of several hit types.
				execAliasEx(TEXT("dclick,%u,%s"), getUserID(), getPathFromItem(tvh.hItem).to_chr());

			bParsed = TRUE;
			break;
		}

		case NM_RCLICK:
		{
			if (!dcx_testflag(getParentDialog()->getEventMask(), DCX_EVENT_CLICK))
				break;

			TVHITTESTINFO tvh{};

			if (!GetCursorPos(&tvh.pt))
				break;

			MapWindowPoints(nullptr, m_Hwnd, &tvh.pt, 1);
			TreeView_HitTest(m_Hwnd, &tvh);

			//|| ( (dcx_testflag(tvh.flags, TVHT_ONITEMRIGHT)) && this->isStyle( TVS_FULLROWSELECT ) ) )
			if ((tvh.flags & TVHT_ONITEM) != 0) // dont use dcx_testflag() here as TVHT_ONITEM is a combination of several hit types.
				execAliasEx(TEXT("rclick,%u,%s"), getUserID(), getPathFromItem(tvh.hItem).to_chr());
			else
				execAliasEx(TEXT("rclick,%u"), getUserID());

			bParsed = TRUE;
			return TRUE; // stop rclick being passed down to parent controls.
		}
		break;

		case TVN_SELCHANGED:
		{
			if (dcxlParam(LPNMTREEVIEW, lpnmtv); lpnmtv && !m_bDestroying)
				execAliasEx(TEXT("selchange,%u,%s"), getUserID(), getPathFromItem(lpnmtv->itemNew.hItem).to_chr());

			bParsed = TRUE;
		}
		break;

		case TVN_GETINFOTIP:
		{
			if (dcxlParam(LPNMTVGETINFOTIP, tcgit); tcgit)
			{
				if (auto lpdcxtvi = reinterpret_cast<LPDCXTVITEM>(tcgit->lParam); lpdcxtvi)
				{
					tcgit->pszText = lpdcxtvi->tsTipText.to_chr();
					tcgit->cchTextMax = gsl::narrow_cast<int>(lpdcxtvi->tsTipText.len());
				}
			}
			bParsed = TRUE;
		}
		break;

		case TVN_ITEMEXPANDING:
		{
			// disables redraw to stop flicker with bkg image.
			if (isExStyle(WindowExStyle::Transparent))
				setRedraw(FALSE);
		}
		break;

		case TVN_ITEMEXPANDED:
		{
			dcxlParam(LPNMTREEVIEW, lpnmtv);

			if (dcx_testflag(lpnmtv->action, TVE_COLLAPSE))
				execAliasEx(TEXT("collapse,%u,%s"), getUserID(), getPathFromItem(lpnmtv->itemNew.hItem).to_chr());
			else if (dcx_testflag(lpnmtv->action, TVE_EXPAND))
				execAliasEx(TEXT("expand,%u,%s"), getUserID(), getPathFromItem(lpnmtv->itemNew.hItem).to_chr());

			// re-enables redraw & updates.
			if (isExStyle(WindowExStyle::Transparent))
			{
				setRedraw(TRUE);
				//InvalidateRect(m_Hwnd, nullptr, FALSE);
				//UpdateWindow(m_Hwnd);
				//if (getParentDialog()->IsVistaStyle())
				redrawWindow();
				//else
				//	redrawBufferedWindow();
			}

//#ifdef USE_CUSTOM_TREE_DRAWING
//			if (Dcx::GDIModule.isUseable() && m_pImage && m_Hwnd)
//			{
//				if (HDC hdc = GetDC(m_Hwnd); hdc)
//				{
//					Auto(ReleaseDC(m_Hwnd, hdc));
//
//					DrawClientArea(hdc, WM_PRINTCLIENT, PRF_CLIENT);
//				}
//			}
//#endif

			bParsed = TRUE;
		}
		break;

//#ifdef USE_CUSTOM_TREE_DRAWING
//		case WM_CTLCOLORSCROLLBAR:
//		{
//			if (m_bCustomDraw)
//			{
//				const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);
//				bParsed = TRUE;
//				return (LRESULT)GetStockBrush(BLACK_BRUSH);
//			}
//		}
//		break;
//#endif

		case TVN_BEGINLABELEDIT:
		{
			bParsed = TRUE;

			if (dcxlParam(LPNMTVDISPINFO, lptvdi); lptvdi)
			{
				TreeView_SelectItem(m_Hwnd, lptvdi->item.hItem);

				if (const auto edit_hwnd = TreeView_GetEditControl(m_Hwnd); edit_hwnd)
				{
					m_OrigEditProc = SubclassWindow(edit_hwnd, DcxTreeView::EditLabelProc);
					SetProp(edit_hwnd, TEXT("dcx_pthis"), this);
				}
			}

			return (getParentDialog()->evalAliasT(TEXT("labelbegin,%"), getUserID()).second == TEXT("noedit"));
		}
		break;
		case TVN_ENDLABELEDIT:
		{
			bParsed = TRUE;

			if (dcxlParam(LPNMTVDISPINFO, lptvdi); !lptvdi->item.pszText)
				execAliasEx(TEXT("labelcancel,%u"), getUserID());
			else
				return (getParentDialog()->evalAliasT(TEXT("labelend,%,%"), getUserID(), lptvdi->item.pszText).second == TEXT("noedit"));

			return TRUE;
		}
		break;

		case NM_CUSTOMDRAW:
		{
			dcxlParam(LPNMTVCUSTOMDRAW, lpntvcd);

			if (!lpntvcd)
				break;

			bParsed = TRUE;

			switch (lpntvcd->nmcd.dwDrawStage)
			{
			case CDDS_PREPAINT:
				return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);

			//case CDDS_ITEMPREERASE:
			//{
			//	const auto lpdcxtvi = reinterpret_cast<LPDCXTVITEM>(lpntvcd->nmcd.lItemlParam);
			//	return CDRF_DODEFAULT;
			//}
			//break;
			//case CDDS_ITEMPOSTERASE:
			//{
			//	const auto lpdcxtvi = reinterpret_cast<LPDCXTVITEM>(lpntvcd->nmcd.lItemlParam);
			//	return CDRF_DODEFAULT;
			//}
			//break;

			case CDDS_ITEMPREPAINT:
			{
				const auto lpdcxtvi = reinterpret_cast<LPDCXTVITEM>(lpntvcd->nmcd.lItemlParam);

				if (!lpdcxtvi)
					return CDRF_DODEFAULT;

				if (lpdcxtvi->clrText != CLR_INVALID)
					lpntvcd->clrText = lpdcxtvi->clrText;

				const auto bSelected = (dcx_testflag(lpntvcd->nmcd.uItemState, CDIS_SELECTED));

				if ((lpdcxtvi->clrBkg != CLR_INVALID) && !bSelected)
					lpntvcd->clrTextBk = lpdcxtvi->clrBkg;
				else if ((m_colSelection != CLR_INVALID) && bSelected)
					lpntvcd->clrTextBk = m_colSelection;

				if (lpdcxtvi->bUline || lpdcxtvi->bBold || lpdcxtvi->bItalic)
				{
					if (const auto hFont = GetWindowFont(m_Hwnd); hFont)
					{
						if (auto [code, lf] = Dcx::dcxGetObject<LOGFONT>(hFont); code != 0)
						{
							if (lpdcxtvi->bBold)
								lf.lfWeight |= FW_BOLD;
							if (lpdcxtvi->bUline)
								lf.lfUnderline = TRUE;
							if (lpdcxtvi->bItalic)
								lf.lfItalic = TRUE;

							m_hItemFont = CreateFontIndirect(&lf);
							if (m_hItemFont)
								m_hOldItemFont = Dcx::dcxSelectObject(lpntvcd->nmcd.hdc, m_hItemFont);
						}
					}
				}
			}
			//return ( CDRF_NOTIFYPOSTPAINT | CDRF_SKIPDEFAULT );
			return (CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT);

			case CDDS_ITEMPOSTPAINT:
			{
#ifdef USE_CUSTOM_TREE_DRAWING
				// this is drawn here to allow the treeview control to draw everything else first.
				if (!lpntvcd->nmcd.hdc)
					return CDRF_DODEFAULT;

				if (m_bCustomDraw)
				{
					// save HDC's context
					const auto savedDC = SaveDC(lpntvcd->nmcd.hdc);

					// make sure font is correct first (this is in an unknown state when we get here)
					if (m_hItemFont)
						Dcx::dcxSelectObject(lpntvcd->nmcd.hdc, m_hItemFont);	// set to item font if it exists.
					else if (m_hFont)
						Dcx::dcxSelectObject(lpntvcd->nmcd.hdc, m_hFont); // if no item font, set to controls font if it exists.

					const auto bSelected = (dcx_testflag(lpntvcd->nmcd.uItemState, CDIS_SELECTED));
					TVITEMEX tvitem{};
					TCHAR buf[MIRC_BUFFER_SIZE_CCH]{};
					auto hItem = reinterpret_cast<HTREEITEM>(lpntvcd->nmcd.dwItemSpec);
					tvitem.hItem = hItem;
					tvitem.mask = TVIF_TEXT;
					tvitem.pszText = &buf[0];
					tvitem.cchTextMax = std::size(buf);
					if (TreeView_GetItem(m_Hwnd, &tvitem))
					{
						TString tsItem(buf);
						RECT rcTxt{};
						RECT rcItem{};
						TreeView_GetItemRect(m_Hwnd, hItem, &rcTxt, TRUE);
						TreeView_GetItemRect(m_Hwnd, hItem, &rcItem, FALSE);
						RECT rcClear = rcItem;

						rcClear.left = rcTxt.left;

						const auto bgClr = (this->getBackColor() != CLR_INVALID) ? this->getBackColor() : GetSysColor(COLOR_WINDOW);

#ifdef DCX_USE_GDIPLUS
						// Ook: if image supplied the control isnt drawn correctly, no borders or scrollbars etc.. needs looked at
						if (Dcx::GDIModule.isUseable() && m_pImage)
						{
							if (auto hdc = CreateHDCBuffer(lpntvcd->nmcd.hdc, nullptr); hdc)
							{
								Auto(DeleteHDCBuffer(hdc));

								DrawGDIPlusImage(*hdc);

								BitBlt(lpntvcd->nmcd.hdc, rcClear.left, rcClear.top, (rcClear.right - rcClear.left), (rcClear.bottom - rcClear.top), *hdc, rcClear.left, rcClear.top, SRCCOPY);
							}
						}
						else
							Dcx::FillRectColour(lpntvcd->nmcd.hdc, std::addressof(rcClear), bgClr);
#else
						Dcx::FillRectColour(lpntvcd->nmcd.hdc, std::addressof(rcClear), bgClr);
#endif

						constexpr UINT TextSyles = /*DT_WORD_ELLIPSIS |*/ DT_LEFT | DT_SINGLELINE | DT_VCENTER;

						if (!this->IsControlCodeTextEnabled())
							tsItem.strip();

						calcTextRect(lpntvcd->nmcd.hdc, tsItem, std::addressof(rcTxt), TextSyles);

						if (bSelected)
						{
							RECT rcSelected = rcTxt;
							rcSelected.left = std::max(rcSelected.left - 2, rcItem.left);
							rcSelected.right = std::min(rcSelected.right + 5, rcItem.right);

							if (!dcxDrawTranslucentRect(lpntvcd->nmcd.hdc, std::addressof(rcSelected), lpntvcd->clrTextBk, GetSysColor(COLOR_3DHIGHLIGHT), false))
								dcxDrawRect(lpntvcd->nmcd.hdc, std::addressof(rcSelected), lpntvcd->clrTextBk, GetSysColor(COLOR_3DHIGHLIGHT), false);
						}

						SetTextColor(lpntvcd->nmcd.hdc, lpntvcd->clrText);

						if (!this->IsControlCodeTextEnabled())
						{
							SetBkMode(lpntvcd->nmcd.hdc, TRANSPARENT);
							if (bSelected && this->IsShadowTextEnabled())
								dcxDrawShadowText(lpntvcd->nmcd.hdc, tsItem.to_wchr(), tsItem.len(), std::addressof(rcTxt), TextSyles, lpntvcd->clrText, 0, 5, 5);
							else
								DrawTextW(lpntvcd->nmcd.hdc, tsItem.to_wchr(), tsItem.len(), std::addressof(rcTxt), TextSyles);
						}
						else
							mIRC_DrawText(lpntvcd->nmcd.hdc, tsItem, std::addressof(rcTxt), TextSyles, ((bSelected && this->IsShadowTextEnabled()) ? true : false));
					}

					// restore HDC's context
					RestoreDC(lpntvcd->nmcd.hdc, savedDC);
				}
#endif

				if (m_hOldItemFont)
				{
					Dcx::dcxSelectObject(lpntvcd->nmcd.hdc, m_hOldItemFont);
					m_hOldItemFont = nullptr;
				}
				if (m_hItemFont)
				{
					DeleteObject(m_hItemFont);
					m_hItemFont = nullptr;
				}

				return CDRF_DODEFAULT;
			}

			default:
				return CDRF_DODEFAULT;
			}
		}
		break;

		case TVN_DELETEITEM:
		{
			dcxlParam(LPNMTREEVIEW, lpnmtv);
			if (!lpnmtv)
				break;

			auto lpdcxtvi = reinterpret_cast<LPDCXTVITEM>(lpnmtv->itemOld.lParam);

			delete lpdcxtvi;
		}
		break;
		case TVN_KEYDOWN:
		{
			if (dcxlParam(const LPNMTVKEYDOWN, ptvkd); ptvkd && ptvkd->wVKey == VK_SPACE)
			{
				if (const auto htvi = TV_GetSelection(m_Hwnd); htvi)
				{
					const auto state = TreeView_GetCheckState(m_Hwnd, htvi);
					//this->execAliasEx(TEXT("%s,%d,%d,%d"), TEXT("stateclick"), this->getUserID( ), (state ? 0 : 1), TreeView_MapHTREEITEMToAccID(m_Hwnd, htvi) );
					this->execAliasEx(TEXT("stateclick,%u,%d,%s"), getUserID(), (state ? 0 : 1), getPathFromItem(htvi).to_chr());
				}
			}
		}
		break;
		//case TVN_ITEMCHANGED: // vista only :/
		//	{
		//		NMTVITEMCHANGE  *pnm = (NMTVITEMCHANGE *)lParam;
		//	}
		//	break;

		default:
			break;
		} // switch
	}
	break;

	default:
		break;
	}
	return 0L;
}

LRESULT DcxTreeView::OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed)
{
	switch (uMsg)
	{
	case WM_CONTEXTMENU:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONUP:
		break;

	case WM_PRINTCLIENT:
	{
		this->DrawClientArea(reinterpret_cast<HDC>(wParam), uMsg, lParam);
		bParsed = TRUE;
	}
	break;

#ifdef USE_CUSTOM_TREE_DRAWING
//#ifdef DCX_USE_GDIPLUS
//	case WM_ERASEBKGND:
//	{
//		if (Dcx::GDIModule.isUseable() && m_pImage)
//		{
//			DrawGDIPlusImage(reinterpret_cast<HDC>(wParam));
//			bParsed = TRUE;
//			return TRUE;
//		}
//	}
//	break;
//#endif

	case WM_HSCROLL:
	{
		if (Dcx::GDIModule.isUseable() && m_pImage && m_Hwnd && m_bCustomDraw)
		{
			const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);
			bParsed = TRUE;
			if (HDC hdc = GetDC(m_Hwnd); hdc)
			{
				Auto(ReleaseDC(m_Hwnd, hdc));

				DrawClientArea(hdc, WM_PRINTCLIENT, PRF_CLIENT);
			}
			return lRes;
		}
	}
	break;

	//case WM_CTLCOLORSCROLLBAR:
	//{
	//	if (m_bCustomDraw)
	//	{
	//		const auto lRes = CallDefaultClassProc(uMsg, wParam, lParam);
	//		bParsed = TRUE;
	//		return (LRESULT)GetStockBrush(BLACK_BRUSH);
	//	}
	//}
	//break;

#endif

	case WM_PAINT:
	{
		bParsed = TRUE;
		if (!wParam)
		{
			PAINTSTRUCT ps{};

			auto hdc = BeginPaint(m_Hwnd, &ps);
			Auto(EndPaint(m_Hwnd, &ps));

			DrawClientArea(hdc, uMsg, lParam);
		}
		else
			this->DrawClientArea(reinterpret_cast<HDC>(wParam), uMsg, lParam);
	}
	break;

	case WM_DESTROY:
	{
		delete this;
		bParsed = TRUE;
	}
	break;

	default:
		return CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return 0L;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT CALLBACK DcxTreeView::EditLabelProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto* const pthis = Dcx::dcxGetProp<DcxTreeView*>(mHwnd, TEXT("dcx_pthis"));

	if (!pthis)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	switch (uMsg)
	{
	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;

	case WM_DESTROY:
	{
		RemoveProp(mHwnd, TEXT("dcx_pthis"));
		if (Dcx::dcxGetWindowProc(mHwnd) == DcxTreeView::EditLabelProc)
			SubclassWindow(mHwnd, pthis->m_OrigEditProc);
	}
	break;
	default:
		break;
	}
	return CallWindowProc(pthis->m_OrigEditProc, mHwnd, uMsg, wParam, lParam);
}

void DcxTreeView::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
#ifdef DCX_USE_GDIPLUS
	if (Dcx::GDIModule.isUseable() && m_pImage)
	{
		// Setup alpha blend if any. Double Buffer is needed to stop flicker when a bkg image is used.
		const auto ai = SetupAlphaBlend(&hdc, true);
		Auto(FinishAlphaBlend(ai));

		DrawGDIPlusImage(hdc);

		CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);
		return;
	}
#endif

	CallDefaultClassProc(uMsg, reinterpret_cast<WPARAM>(hdc), lParam);

	//CallDefaultClassProc(WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdc), PRF_CLIENT | PRF_CHILDREN);
}

// clears existing image and icon data and sets pointers to null
void DcxTreeView::PreloadData() noexcept
{
#ifdef DCX_USE_GDIPLUS
	m_pImage.reset(nullptr);
#endif
}

#ifdef DCX_USE_GDIPLUS
GSL_SUPPRESS(Enum.3)
void DcxTreeView::LoadGDIPlusImage(const TString& flags, TString& filename)
{
	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("LoadGDIPlusImage() - Unable to Access File: %"), filename);

	m_pImage = std::make_unique<Gdiplus::Image>(filename.to_chr(), TRUE);

	// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
	if (const auto status = m_pImage->GetLastStatus(); status != Gdiplus::Status::Ok)
	{
		PreloadData();
		throw Dcx::dcxException(TEXT("LoadGDIPlusImage() - %"), GetLastStatusStr(status));
	}
	const XSwitchFlags xflags(flags);

	if (xflags[TEXT('h')]) { // High Quality
		this->m_CQuality = Gdiplus::CompositingQualityHighQuality;
		this->m_IMode = Gdiplus::InterpolationModeHighQualityBicubic;
	}
	else {
		this->m_CQuality = Gdiplus::CompositingQualityDefault;
		this->m_IMode = Gdiplus::InterpolationModeDefault;
	}

	if (xflags[TEXT('b')]) // Blend Image
		this->m_CMode = Gdiplus::CompositingModeSourceOver;
	else
		this->m_CMode = Gdiplus::CompositingModeSourceCopy;

	if (xflags[TEXT('a')]) // Anti-Aliased
		this->m_SMode = Gdiplus::SmoothingModeAntiAlias;
	else
		this->m_SMode = Gdiplus::SmoothingModeDefault;

	this->m_bTileImage = xflags[TEXT('t')]; // Tile

	this->m_bResizeImage = xflags[TEXT('r')]; // resize
}

void DcxTreeView::DrawGDIPlusImage(HDC hdc)
{
	if (!hdc || !m_Hwnd)
		return;

	//RECT rc{};
	//if (!GetClientRect(m_Hwnd, &rc))
	//	return;
	//const auto w = (rc.right - rc.left), h = (rc.bottom - rc.top), x = rc.left, y = rc.top;

	Dcx::dcxClientRect rc(m_Hwnd);
	if (!rc)
		return;

	const auto w = rc.Width(), h = rc.Height(), x = rc.left, y = rc.top;
	if (m_bTransparent)
	{
		if (!IsAlphaBlend())
			DrawParentsBackground(hdc, &rc);
	}
	else {
		if (auto hBrush = getBackClrBrush(); hBrush)
			FillRect(hdc, &rc, hBrush);
		else
			Dcx::FillRectColour(hdc, &rc, GetBkColor(hdc));
	}
	//GdiFlush();

	Gdiplus::Graphics grphx(hdc);

	grphx.SetCompositingQuality(m_CQuality);
	grphx.SetCompositingMode(m_CMode);
	grphx.SetSmoothingMode(m_SMode);

	if (((m_pImage->GetWidth() == 1) || (m_pImage->GetHeight() == 1)) && m_bResizeImage)
	{
		// This fixes a GDI+ bug when resizing 1 px images
		// http://www.devnewsgroups.net/group/microsoft.public.dotnet.framework.windowsforms/topic11515.aspx
		grphx.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
		grphx.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
	}
	else
		grphx.SetInterpolationMode(m_IMode);

	if (m_bTileImage)
	{
		Gdiplus::ImageAttributes imAtt;
		imAtt.SetWrapMode(Gdiplus::WrapModeTile);

		grphx.DrawImage(m_pImage.get(),
			Gdiplus::Rect(x + m_iXOffset, y + m_iYOffset, w, h),  // dest rect
			0, 0, w, h,       // source rect
			Gdiplus::UnitPixel,
			&imAtt);
	}
	else if (m_bResizeImage)
		grphx.DrawImage(m_pImage.get(), m_iXOffset, m_iYOffset, w, h);
	else
		grphx.DrawImage(m_pImage.get(), m_iXOffset, m_iYOffset);
}
#endif
/*
	xmlLoadTree()
	Loads items into a treeview control from a dcxml file.
*/
HTREEITEM DcxTreeView::xmlLoadTree(HTREEITEM hInsertAfter, HTREEITEM hParent, const TString& name, TString& filename)
{
	if (!IsFile(filename))
		throw Dcx::dcxException(TEXT("xmlLoadTree() - Unable To Access File: %"), filename);

	TiXmlDocument doc(filename.c_str());
	doc.SetCondenseWhiteSpace(false);

	if (!doc.LoadFile())
		throw Dcx::dcxException(TEXT("xmlLoadTree() - Not an XML File: %"), filename);

	const auto xRoot = doc.FirstChildElement("dcxml");
	if (!xRoot)
		throw Dcx::dcxException("xmlLoadTree() - Unable Find 'dcxml' root");

	const auto* xElm = xRoot->FirstChildElement("treeview_data");
	if (!xElm)
		throw Dcx::dcxException("xmlLoadTree() - Unable To Find 'treeview_data' element");

	xElm = xElm->FirstChildElement(name.c_str());
	if (!xElm)
		throw Dcx::dcxException(TEXT("xmlLoadTree() - Unable To Find Dataset: %"), name);

	if (hInsertAfter == TVI_ROOT)
		hInsertAfter = TreeView_GetRoot(m_Hwnd);

	this->setRedraw(FALSE);
	this->xmlInsertItems(hParent, hInsertAfter, xElm);
	this->setRedraw(TRUE);
	//InvalidateRect(m_Hwnd, nullptr, FALSE);
	//UpdateWindow(m_Hwnd);
	this->redrawWindow();
	return hInsertAfter;
}

/*
	xmlSaveTree()
	Saves Items from a treeview control to a dcxml file.
	NB: Removes any existing items from the named dataset.
*/
void DcxTreeView::xmlSaveTree(HTREEITEM hFromItem, const TString& name, TString& filename)
{
	TiXmlDocument doc(filename.c_str());
	doc.SetCondenseWhiteSpace(false);

	if (IsFile(filename))
	{
		if (!doc.LoadFile())
			throw Dcx::dcxException(TEXT("xmlSaveTree() - Not an XML File: %"), filename);
	}
	auto xRoot = doc.FirstChildElement("dcxml");
	if (!xRoot)
	{
		xRoot = dynamic_cast<TiXmlElement*>(doc.InsertEndChild(TiXmlElement("dcxml")));

		if (!xRoot)
			throw Dcx::dcxException("xmlSaveTree() - Unable To Add Root <dcxml>");
	}

	auto xData = xRoot->FirstChildElement("treeview_data");
	if (!xData)
	{
		xData = dynamic_cast<TiXmlElement*>(xRoot->InsertEndChild(TiXmlElement("treeview_data")));
		if (!xData)
			throw Dcx::dcxException("xmlSaveTree() - Unable to add <treeview_data> item");
	}

	auto xElm = xData->FirstChildElement(name.c_str());
	if (!xElm)
	{
		xElm = dynamic_cast<TiXmlElement*>(xData->InsertEndChild(TiXmlElement(name.c_str())));
		if (!xElm)
			throw Dcx::dcxException("xmlSaveTree() - Unable to add named item");
	}

	xElm->Clear();

	if (hFromItem == TVI_ROOT)
		hFromItem = TreeView_GetRoot(m_Hwnd);

	//if (auto lbuf = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH); !this->xmlGetItems(hFromItem, xElm, lbuf.get()))
	//	throw Dcx::dcxException("xmlSaveTree() - Unable To Add Items to XML");
	if (TCHAR lbuf[MIRC_BUFFER_SIZE_CCH]{}; !this->xmlGetItems(hFromItem, xElm, &lbuf[0]))
		throw Dcx::dcxException("xmlSaveTree() - Unable To Add Items to XML");

	doc.SaveFile();
}

/*
	xmlGetItems()
	Recursive function that loops through all treeview items & adds them to the xml data.
*/
bool DcxTreeView::xmlGetItems(const HTREEITEM hFirstSibling, TiXmlElement* xElm, TCHAR* buf)
{
	if (!hFirstSibling)
		return false;

	TVITEMEX tvi{};
	TiXmlElement* xTmp = nullptr;
	bool bRes = true;

	for (auto hSib = hFirstSibling; hSib; hSib = TreeView_GetNextSibling(m_Hwnd, hSib))
	{
		ZeroMemory(&tvi, sizeof(tvi));
		tvi.hItem = hSib;
		tvi.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE | TVIF_INTEGRAL | TVIF_STATE | TVIF_TEXT | TVIF_CHILDREN;
		tvi.cchTextMax = MIRC_BUFFER_SIZE_CCH;
		tvi.pszText = buf;

		if (!TreeView_GetItem(m_Hwnd, &tvi))
			break;

		auto lpmytvi = reinterpret_cast<LPDCXTVITEM>(tvi.lParam);
		if (!lpmytvi)
		{
			bRes = false;
			break;
		}
		{
			TiXmlElement xChild("tvitem");
			xChild.SetAttribute("text", TString(tvi.pszText).c_str());
			if (tvi.iImage > -1 && tvi.iImage != 10000)
				xChild.SetAttribute("image", tvi.iImage + 1);
			if (tvi.iSelectedImage > -1 && tvi.iSelectedImage != 10000)
				xChild.SetAttribute("selectedimage", tvi.iSelectedImage + 1);
			if (tvi.iExpandedImage > 0 && tvi.iExpandedImage != 10000)
				xChild.SetAttribute("expandedimage", tvi.iExpandedImage);
			if (tvi.iIntegral > 0)
				xChild.SetAttribute("itegral", tvi.iIntegral);
			if (dcx_testflag(tvi.stateMask, TVIS_SELECTED) && dcx_testflag(tvi.state, TVIS_SELECTED))
				xChild.SetAttribute("selected", 1);
			if (!lpmytvi->tsTipText.empty())
				xChild.SetAttribute("tooltip", lpmytvi->tsTipText.c_str());
			if (lpmytvi->bBold)
				xChild.SetAttribute("textbold", 1);
			if (lpmytvi->bItalic)
				xChild.SetAttribute("textitalic", 1);
			if (lpmytvi->bUline)
				xChild.SetAttribute("textunderline", 1);
			if (lpmytvi->clrBkg != CLR_INVALID)
				xChild.SetAttribute("backgroundcolor", gsl::narrow_cast<int>(lpmytvi->clrBkg));
			if (lpmytvi->clrText != CLR_INVALID)
				xChild.SetAttribute("textcolor", gsl::narrow_cast<int>(lpmytvi->clrText));
			auto i = (((tvi.state & TVIS_OVERLAYMASK) >> 8) & 0xFF);
			if (i > 0 && i < 16) // zero means no overlay, so don't save
				xChild.SetAttribute("overlay", gsl::narrow_cast<int>(i));
			i = (((tvi.state & TVIS_STATEIMAGEMASK) >> 12) & 0xFF);
			if (i > 0 && i < 5)
				xChild.SetAttribute("state", gsl::narrow_cast<int>(i)); // zero means no state image so don't save
			xTmp = xElm->InsertEndChild(xChild)->ToElement();
			if (!xTmp)
			{
				bRes = false;
				break;
			}
		}
		if (tvi.cChildren > 0 && xTmp)
			bRes = this->xmlGetItems(TreeView_GetChild(m_Hwnd, hSib), xTmp, buf);
	}
	return bRes;
}

/*
	xmlInsertItems()
	Recursive function that inserts items into a treeview control from the xml data.
*/
const TiXmlElement* DcxTreeView::xmlInsertItems(HTREEITEM hParent, HTREEITEM& hInsertAfter, const TiXmlElement* xElm)
{
	TVINSERTSTRUCT tvins{};
	const char* attr = nullptr;
	const TiXmlElement* xRes = xElm;

	for (const auto* xNode = xElm->FirstChildElement("tvitem"); xNode; xNode = xNode->NextSiblingElement("tvitem"))
	{
		ZeroMemory(&tvins, sizeof(tvins));
		tvins.hInsertAfter = hInsertAfter;
		tvins.hParent = hParent;
		{ // scope to limit usage of lpmytvi
			//auto lpmytvi = new DCXTVITEM;
			auto lpmytvi = std::make_unique<DCXTVITEM>();

			lpmytvi->hHandle = nullptr;

			tvins.itemex.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE;
			tvins.itemex.lParam = reinterpret_cast<LPARAM>(lpmytvi.get());

			// Items Icon.
			tvins.itemex.iImage = queryIntAttribute(xNode, "image", I_IMAGECALLBACK) - 1;	// NB: I_IMAGECALLBACK is change to I_IMAGENONE by the -1

			// Items Selected state icon.
			tvins.itemex.iSelectedImage = queryIntAttribute(xNode, "selectedimage", tvins.itemex.iImage + 1) - 1;

			// Items expanded state icon.
			tvins.itemex.iExpandedImage = queryIntAttribute(xNode, "expandedimage", I_IMAGECALLBACK) - 1;	// NB: I_IMAGECALLBACK is change to I_IMAGENONE by the -1

			// Items height integral.
			tvins.itemex.iIntegral = queryIntAttribute(xNode, "integral");
			if (tvins.itemex.iIntegral > 0)
				tvins.itemex.mask |= TVIF_INTEGRAL;

			// Items selected
			if (queryIntAttribute(xNode, "selected") > 0)
			{
				tvins.itemex.state |= TVIS_SELECTED;
				tvins.itemex.stateMask |= TVIS_SELECTED;
			}

			// Items tooltip
			attr = xNode->Attribute("tooltip");
			if (attr)
				lpmytvi->tsTipText = attr;

			// Items text colour.
			lpmytvi->clrText = gsl::narrow_cast<COLORREF>(queryIntAttribute(xNode, "textcolor", CLR_INVALID));

			// Items background colour.
			lpmytvi->clrBkg = gsl::narrow_cast<COLORREF>(queryIntAttribute(xNode, "backgroundcolor", CLR_INVALID));

			// Is Item text in Bold?
			if (queryIntAttribute(xNode, "textbold") > 0)
			{
				lpmytvi->bBold = true;
				tvins.itemex.state |= TVIS_BOLD;
				tvins.itemex.stateMask |= TVIS_BOLD;
			}
			else
				lpmytvi->bBold = false;

			// Is item text in italics?
			lpmytvi->bItalic = (queryIntAttribute(xNode, "textitalic") > 0);

			// Is item text underlined?
			lpmytvi->bUline = (queryIntAttribute(xNode, "textunderline") > 0);

			// Items Text.
			TString tsAttr(xNode->Attribute("text")); // MUST be before/outside if()
			if (!tsAttr.empty())
			{
				tvins.itemex.cchTextMax = gsl::narrow_cast<int>(tsAttr.len());
				tvins.itemex.pszText = tsAttr.to_chr();
				tvins.itemex.mask |= TVIF_TEXT;
			}
			hInsertAfter = TreeView_InsertItem(m_Hwnd, &tvins);
			if (!hInsertAfter)
				throw Dcx::dcxException("xmlInsertItems() - Unable To Add XML Item To TreeView");

			lpmytvi->hHandle = hInsertAfter;
			lpmytvi.release(); // <- release mem so its not deleted.
		}
		// Items state icon.
		if (auto i = queryIntAttribute(xNode, "state"); i < 5) // zero means no state icon anyway.
			//TreeView_SetItemState(m_Hwnd, hInsertAfter, INDEXTOSTATEIMAGEMASK(gsl::narrow_cast<UINT>(i)), TVIS_STATEIMAGEMASK);
			TV_SetItemState(hInsertAfter, INDEXTOSTATEIMAGEMASK(gsl::narrow_cast<UINT>(i)), TVIS_STATEIMAGEMASK);

		// Items overlay icon.
		// overlay is 1-based index
		if (auto i = queryIntAttribute(xNode, "overlay"); (i > 0 && i < 16))
			//TreeView_SetItemState(m_Hwnd, hInsertAfter, INDEXTOOVERLAYMASK(gsl::narrow_cast<UINT>(i)), TVIS_OVERLAYMASK);
			TV_SetItemState(hInsertAfter, INDEXTOOVERLAYMASK(gsl::narrow_cast<UINT>(i)), TVIS_OVERLAYMASK);

		if (xNode->FirstChild("tvitem"))
		{
			// item has children. NB: DON'T update xNode to the result of this call as this stops subsequent items being added.
			this->xmlInsertItems(hInsertAfter, hInsertAfter, xNode);
		}
		xRes = xNode;
	}
	return xRes;
}

GSL_SUPPRESS(type.4)
HTREEITEM DcxTreeView::TV_GetLastSibling(HTREEITEM child) const noexcept
{
	if (!child || !m_Hwnd)
		return nullptr;

	HTREEITEM current{};
	for (HTREEITEM tmp = TreeView_GetNextSibling(m_Hwnd, child); tmp; tmp = TreeView_GetNextSibling(m_Hwnd, current))
		current = tmp;
	return current;
}

const TString DcxTreeView::getStyles() const
{
	auto styles(__super::getStyles());

	{
		const auto Styles = dcxGetWindowStyle(m_Hwnd);

		if (dcx_testflag(Styles, TVS_HASLINES))
			styles.addtok(TEXT("haslines"));
		if (dcx_testflag(Styles, TVS_HASBUTTONS))
			styles.addtok(TEXT("hasbuttons"));
		if (dcx_testflag(Styles, TVS_LINESATROOT))
			styles.addtok(TEXT("linesatroot"));
		if (dcx_testflag(Styles, TVS_SHOWSELALWAYS))
			styles.addtok(TEXT("showsel"));
		if (dcx_testflag(Styles, TVS_EDITLABELS))
			styles.addtok(TEXT("editlabel"));
		if (dcx_testflag(Styles, TVS_NOHSCROLL))
			styles.addtok(TEXT("nohscroll"));
		if (dcx_testflag(Styles, TVS_FULLROWSELECT))
			styles.addtok(TEXT("fullrow"));
		if (dcx_testflag(Styles, TVS_SINGLEEXPAND))
			styles.addtok(TEXT("singleexpand"));
		if (dcx_testflag(Styles, TVS_CHECKBOXES))
			styles.addtok(TEXT("checkbox"));
		if (dcx_testflag(Styles, TVS_NOTOOLTIPS))
			styles.addtok(TEXT("notooltips"));
		if (dcx_testflag(Styles, TVS_NOSCROLL))
			styles.addtok(TEXT("noscroll"));
	}

	//const auto ExStyles = dcxGetWindowExStyle(m_Hwnd);

	{
		const auto Styles = TreeView_GetExtendedStyle(m_Hwnd);

		if (dcx_testflag(Styles, TVS_EX_FADEINOUTEXPANDOS))
			styles.addtok(TEXT("fadebuttons"));
		if (dcx_testflag(Styles, TVS_EX_DOUBLEBUFFER))
			styles.addtok(TEXT("doublebuffer"));
		if (dcx_testflag(Styles, TVS_EX_NOINDENTSTATE))
			styles.addtok(TEXT("noident"));
		if (dcx_testflag(Styles, TVS_EX_RICHTOOLTIP))
			styles.addtok(TEXT("richtooltips"));
		if (dcx_testflag(Styles, TVS_EX_AUTOHSCROLL))
			styles.addtok(TEXT("autohscroll"));
		if (dcx_testflag(Styles, TVS_EX_DIMMEDCHECKBOXES))
			styles.addtok(TEXT("dimmedchecks"));
	}

	return styles;
}

/*
 * Retrieves an item from the path specified.
 *
 * If the path is invalid, it will return nullptr.
 * If hParent and hInsertAt parameters are filled, it is assumed we are searching for a place to insert the item
 */
HTREEITEM DcxTreeView::parsePath(const TString& path, HTREEITEM* hParent, HTREEITEM* hInsertAt) const
{
	// Invalid path - no directives.
	if (path.empty())
		return nullptr;

	HTREEITEM current = TVI_ROOT;
	auto foundSoFar = current;
	const auto count = path.numtok();
	const auto bFillLocation = ((hParent) && (hInsertAt));

	// Iterate through each directive in the path string
	for (auto k = decltype(count){1}; k <= count; ++k)
	{
		// Convert the path directory.
		const auto dir = path.gettok(gsl::narrow_cast<int>(k)).to_int();

		// Keep track of the current parent.
		if (bFillLocation)
			*hParent = current;

		current = TreeView_GetChild(m_Hwnd, current);

		// Adding first child item, with the 2nd last directive as the parent node
		if ((!current) && (getChildCount(foundSoFar) == 0) && bFillLocation && (k == count))
		{
			*hParent = foundSoFar;
			*hInsertAt = TVI_FIRST;
			return foundSoFar;
		}

		// Return first child
		if (dir == 1)
		{
			if (bFillLocation)
				*hInsertAt = current;
		}
		// Insert at very start.
		// Not used unless for insertion or moving.
		else if (dir == 0)
		{
			if (bFillLocation)
			{
				*hParent = foundSoFar;
				*hInsertAt = TVI_FIRST;
			}
		}
		// Last item in current branch (Handles negative numbers as last item)
		// else if (dir == -1)
		else if (dir < 0)
		{
			// if we cant find any siblings for this item, then last item IS this item.
			if (auto tmp = TV_GetLastSibling(current); tmp)
				current = tmp;

			if (bFillLocation)
				*hInsertAt = TVI_LAST;
		}
		// Search for Nth child.
		else {
			//i = 1;
			//
			//while ((current = TreeView_GetNextSibling(m_Hwnd, current)) != nullptr) {
			//	if (bFillLocation)
			//		*hInsertAt = current;
			//
			//	i++;
			//
			//	// Found the branch we want
			//	if (i == dir)
			//		break;
			//}
			// this version allows using a path thats `last item + 1`

			auto i = 1;                             // iterator for counting the Nth directory

			//HTREEITEM tmp{};
			//while ((tmp = TreeView_GetNextSibling(m_Hwnd, current)))
			//{
			//	current = tmp;
			//	if (bFillLocation)
			//		*hInsertAt = current;
			//	if (++i == dir)
			//		break;
			//}
			// using last item + 1 in a path only works wenn fill location is true, and we are at the last level
			//if ((bFillLocation) && (++i == dir) && (k == count))
			//	*hInsertAt = current;
			// else we need to be sure that the last item wasn't null
			//else 
			//	current = tmp;

			for (HTREEITEM tmp = TreeView_GetNextSibling(m_Hwnd, current); tmp; tmp = TreeView_GetNextSibling(m_Hwnd, current))
			{
				current = tmp;
				if (bFillLocation)
					*hInsertAt = current;
				if (++i == dir)
					break;
			}
		}

		// Couldnt find specified path.
		if (!current)
			return nullptr;

		// Inch forward on next item.
		foundSoFar = current;
	}

	return foundSoFar;
}

void DcxTreeView::toXml(TiXmlElement* const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());
}

TiXmlElement* DcxTreeView::toXml() const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

LRESULT DcxTreeView::CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (m_hDefaultClassProc)
		return CallWindowProc(m_hDefaultClassProc, this->m_Hwnd, uMsg, wParam, lParam);

	return DefWindowProc(this->m_Hwnd, uMsg, wParam, lParam);
}
