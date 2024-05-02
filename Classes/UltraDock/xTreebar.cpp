
/*
*/
#include "defines.h"
#include "Classes/UltraDock/dcxDock.h"
#include "Dcx.h"

GSL_SUPPRESS(type.4)
static void TraverseChildren(const HTREEITEM hParent, TString& buf, TString& res, LPTVITEMEX pitem)
//static void TraverseChildren(const HTREEITEM hParent)
{
	if (!hParent || !pitem)
		return;
	ZeroMemory(pitem, sizeof(TVITEMEX));
	for (auto ptvitem = Dcx::dcxTreeView_GetChild(mIRCLinker::getTreeview(), hParent); ptvitem; ptvitem = Dcx::dcxTreeView_GetNextSibling(mIRCLinker::getTreeview(), ptvitem))
	{
		pitem->hItem = ptvitem;
		pitem->pszText = buf.to_chr();
		pitem->cchTextMax = gsl::narrow_cast<int>(buf.capacity_cch());
		pitem->mask = TVIF_TEXT | TVIF_PARAM | TVIF_HANDLE;
		if (Dcx::dcxTreeView_GetItem(mIRCLinker::getTreeview(), pitem))
		{
			{
				const TString tsType(DcxDock::getTreebarItemType(pitem->lParam));
	
				mIRCLinker::exec(TEXT("/!set -nu1 \\%dcx_% %"), pitem->lParam, pitem->pszText);
				mIRCLinker::eval(res, TEXT("$xtreebar_callback(geticons,%,\\%dcx_%)"), tsType, pitem->lParam);
			}
			pitem->mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE | TVIF_HANDLE;
	
			// image
			auto i = res.getfirsttok(1).to_int() - 1;
			pitem->iImage = (i < 0) ? I_IMAGENONE : i;
	
			// selected image (if none supplied use the standard image)
			i = res.getnexttok().to_int() - 1;
			pitem->iSelectedImage = (i < 0) ? pitem->iImage : i;
	
			// expanded image (if none supplied use the standard image)
			i = res.getnexttok().to_int() - 1;
			pitem->iExpandedImage = (i < 0) ? pitem->iImage : i;
	
			Dcx::dcxTreeView_SetItem(mIRCLinker::getTreeview(), pitem);
		}
		TraverseChildren(ptvitem, buf, res, pitem);
	}

	//ZeroMemory(pitem, sizeof(TVITEMEX));
	//for (auto ptvitem: Dcx::dcxTreeItem(mIRCLinker::getTreeview(), hParent))
	//{
	//	pitem->hItem = (HTREEITEM)ptvitem;
	//	pitem->pszText = buf.to_chr();
	//	pitem->cchTextMax = buf.capacity_cch();
	//	pitem->mask = TVIF_TEXT | TVIF_PARAM;
	//	if (TreeView_GetItem(mIRCLinker::getTreeview(), pitem))
	//	{
	//		{
	//			const TString tsType(DcxDock::getTreebarItemType(pitem->lParam));
	//
	//			mIRCLinker::exec(TEXT("/!set -nu1 \\%dcx_% %"), pitem->lParam, pitem->pszText);
	//			mIRCLinker::eval(res, TEXT("$xtreebar_callback(geticons,%,\\%dcx_%)"), tsType, pitem->lParam);
	//		}
	//		pitem->mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE;
	//
	//		// image
	//		auto i = res.getfirsttok(1).to_int() - 1;
	//		pitem->iImage = (i < 0) ? I_IMAGENONE : i;
	//
	//		// selected image (if none supplied use the standard image)
	//		i = res.getnexttok().to_int() - 1;
	//		pitem->iSelectedImage = (i < 0) ? pitem->iImage : i;
	//
	//		// expanded image (if none supplied use the standard image)
	//		i = res.getnexttok().to_int() - 1;
	//		pitem->iExpandedImage = (i < 0) ? pitem->iImage : i;
	//
	//		TreeView_SetItem(mIRCLinker::getTreeview(), pitem);
	//	}
	//	TraverseChildren((HTREEITEM)ptvitem, buf, res, pitem);
	//}

	//for (auto ptvitem : Dcx::dcxTreeItem(mIRCLinker::getTreeview(), hParent))
	//{
	//	{	// limit the scope of these objects
	//		const TString tsItem(ptvitem.GetItemText());
	//		const LPARAM lparam = ptvitem.GetItemParam<LPARAM>();
	//		const TString tsType(DcxDock::getTreebarItemType(lparam));
	//		TString res;
	//
	//		mIRCLinker::exec(TEXT("/!set -nu1 \\%dcx_% %"), lparam, tsItem);
	//		mIRCLinker::eval(res, TEXT("$xtreebar_callback(geticons,%,\\%dcx_%)"), tsType, lparam);
	//		if (!res.empty())
	//		{
	//			// image
	//			const auto iImage = res.getfirsttok(1).to_int() - 1;
	//
	//			// selected image (if none supplied use the standard image)
	//			const auto iSelected = res.getnexttok().to_int() - 1;
	//
	//			// expanded image (if none supplied use the standard image)
	//			const auto iExpanded = res.getnexttok().to_int() - 1;
	//
	//			ptvitem.SetItemImages(iImage, iSelected, iExpanded);
	//		}
	//	}
	//
	//	TraverseChildren((HTREEITEM)ptvitem);
	//}
}

GSL_SUPPRESS(type.4)
GSL_SUPPRESS(es.47)
static void TraverseTreebarItems(void)
{
	SetWindowRedraw(mIRCLinker::getTreeview(), FALSE);
	Auto(SetWindowRedraw(mIRCLinker::getTreeview(), TRUE));

	TString buf(gsl::narrow_cast<TString::size_type>(mIRCLinker::c_mIRC_Buffer_Size_cch));
	TString res;
	TVITEMEX item{};

	for (auto ptvitem = Dcx::dcxTreeView_GetRoot(mIRCLinker::getTreeview()); ptvitem; ptvitem = Dcx::dcxTreeView_GetNextSibling(mIRCLinker::getTreeview(), ptvitem))
	{
		item.hItem = ptvitem;
		item.pszText = buf.to_chr();
		item.cchTextMax = gsl::narrow_cast<int>(buf.capacity_cch()); //MIRC_BUFFER_SIZE_CCH;
		item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_HANDLE;
		if (Dcx::dcxTreeView_GetItem(mIRCLinker::getTreeview(), &item))
		{
			{
				TString tsType(DcxDock::getTreebarItemType(item.lParam));
	
				mIRCLinker::exec(TEXT("/!set -nu1 \\%dcx_% %"), item.lParam, item.pszText);
				mIRCLinker::eval(res, TEXT("$xtreebar_callback(geticons,%,\\%dcx_%)"), tsType, item.lParam);
			}
			item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE | TVIF_HANDLE;
	
			// image
			auto i = res.getfirsttok(1).to_int() - 1;
			item.iImage = (i < 0) ? I_IMAGENONE : i;
	
			// selected image (if none supplied use the standard image)
			i = res.getnexttok().to_int() - 1;
			item.iSelectedImage = (i < 0) ? item.iImage : i;
	
			// expanded image (if none supplied use the standard image)
			i = res.getnexttok().to_int() - 1;
			item.iExpandedImage = (i < 0) ? item.iImage : i;
	
			Dcx::dcxTreeView_SetItem(mIRCLinker::getTreeview(), &item);
		}
		TraverseChildren(ptvitem, buf, res, &item);
	}

	//for (auto ptvitem : Dcx::dcxTreeItem(mIRCLinker::getTreeview()))
	//{
	//	item.hItem = (HTREEITEM)ptvitem;
	//	item.pszText = buf.to_chr();
	//	item.cchTextMax = buf.capacity_cch();
	//	item.mask = TVIF_TEXT | TVIF_PARAM;
	//	if (TreeView_GetItem(mIRCLinker::getTreeview(), &item))
	//	{
	//		{
	//			const TString tsType(DcxDock::getTreebarItemType(item.lParam));
	//
	//			mIRCLinker::exec(TEXT("/!set -nu1 \\%dcx_% %"), item.lParam, item.pszText);
	//			mIRCLinker::eval(res, TEXT("$xtreebar_callback(geticons,%,\\%dcx_%)"), tsType, item.lParam);
	//		}
	//
	//		if (!res.empty())
	//		{
	//			item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE;
	//
	//			// image
	//			auto i = res.getfirsttok(1).to_int() - 1;
	//			item.iImage = (i < 0) ? I_IMAGENONE : i;
	//
	//			// selected image (if none supplied use the standard image)
	//			i = res.getnexttok().to_int() - 1;
	//			item.iSelectedImage = (i < 0) ? item.iImage : i;
	//
	//			// expanded image (if none supplied use the standard image)
	//			i = res.getnexttok().to_int() - 1;
	//			item.iExpandedImage = (i < 0) ? item.iImage : i;
	//
	//			TreeView_SetItem(mIRCLinker::getTreeview(), &item);
	//		}
	//	}
	//	TraverseChildren((HTREEITEM)ptvitem, buf, res, &item);
	//}

	//for (auto ptvitem : Dcx::dcxTreeItem(mIRCLinker::getTreeview()))
	//{
	//	{	// limit the scope of these objects
	//		const TString tsItem(ptvitem.GetItemText());
	//		const LPARAM lparam = ptvitem.GetItemParam<LPARAM>();
	//		const TString tsType(DcxDock::getTreebarItemType(lparam));
	//		TString res;
	//
	//		mIRCLinker::exec(TEXT("/!set -nu1 \\%dcx_% %"), lparam, tsItem);
	//		mIRCLinker::eval(res, TEXT("$xtreebar_callback(geticons,%,\\%dcx_%)"), tsType, lparam);
	//		if (!res.empty())
	//		{
	//			// image
	//			const auto iImage = res.getfirsttok(1).to_int() - 1;
	//
	//			// selected image (if none supplied use the standard image)
	//			const auto iSelected = res.getnexttok().to_int() - 1;
	//
	//			// expanded image (if none supplied use the standard image)
	//			const auto iExpanded = res.getnexttok().to_int() - 1;
	//
	//			ptvitem.SetItemImages(iImage, iSelected, iExpanded);
	//		}
	//	}
	//	TraverseChildren((HTREEITEM)ptvitem);
	//}
}

//static HTREEITEM WalkChildItems(HWND mHwnd, HTREEITEM hParent, size_t &nCnt, size_t &nIndex) noexcept
//{
//	if ((!mHwnd) || (!hParent))
//		return nullptr;
//
//	for (auto hItem = Dcx::dcxTreeView_GetChild(mHwnd, hParent); hItem; hItem = Dcx::dcxTreeView_GetNextSibling(mHwnd, hItem))
//	{
//		if (++nCnt == nIndex)
//			return hItem;
//
//		if (auto hRes = WalkChildItems(mHwnd, hItem, nCnt, nIndex); hRes)
//			return hRes;
//	}
//	return nullptr;
//}
//
//static HTREEITEM MapIndexToItem(size_t nIndex) noexcept
//{
//	HWND mHwnd{ mIRCLinker::getTreeview() };
//	if ((!mHwnd) || (!IsWindow(mHwnd)))
//		return nullptr;
//
//	size_t nCnt{};
//
//	for (auto hItem = Dcx::dcxTreeView_GetRoot(mHwnd); hItem; hItem = Dcx::dcxTreeView_GetNextSibling(mHwnd, hItem))
//	{
//		if (++nCnt == nIndex)
//			return hItem;
//
//		if (auto hRes = WalkChildItems(mHwnd, hItem, nCnt, nIndex); hRes)
//			return hRes;
//	}
//
//	return nullptr;
//}

// [SWITCH] [OPTIONS]
GSL_SUPPRESS(type.4)
mIRC(xtreebar)
{
	TString input(data);

	data[0] = 0;

	try {
		input.trim();

		const auto numtok = input.numtok();

		if (!IsWindow(mIRCLinker::getTreeview()))
			throw Dcx::dcxException("No Treebar");

		const auto switches(input.getfirsttok(1));		// tok 1

		if (switches[0] != TEXT('-'))
			throw DcxExceptions::dcxInvalidCommand();

		switch (switches[1])
		{
			//case TEXT('a'):	// [] add items to treeview
			//	{
			//	}
			//	break;
		case TEXT('f'): // [+FONTFLAGS] [CHARSET] [SIZE] [FONTNAME]
		{
			if (numtok < 5)
				throw DcxExceptions::dcxInvalidArguments();

			if (LOGFONT lf{}; ParseCommandToLogfont(input.getlasttoks(), &lf))	// tok 2, -1
			{
				if (auto hFont = CreateFontIndirect(&lf); hFont)
					mIRCLinker::setTreeFont(hFont);
			}
		}
		break;
		case TEXT('s'): // [STYLES]
		{
			if (numtok < 2)
				throw DcxExceptions::dcxInvalidArguments();

			auto stylef = dcxGetWindowStyle(mIRCLinker::getTreeview());
			auto exstylef = dcxGetWindowExStyle(mIRCLinker::getTreeview());

			DWORD tvexstylef = 0;
			if (Dcx::DwmModule.isUseable())
				tvexstylef = Dcx::dcxTreeView_GetExtendedStyle(mIRCLinker::getTreeview());
			DWORD tvexstylemask = 0;

			const TString tsStyles(input.getlasttoks());

			for (const auto& x : tsStyles)
			{
				switch (std::hash<TString>{}(x))		// tok 2+
				{
				case L"fullrowselect"_hash: // fullrowselect (off by default)
					stylef |= TVS_FULLROWSELECT;
					stylef &= ~TVS_HASLINES;	// this style does not work with the TVS_HASLINES style
					break;
				case L"nofullrowselect"_hash: // nofullrowselect
					stylef &= ~TVS_FULLROWSELECT;
					break;
				case L"haslines"_hash: // haslines
					stylef |= TVS_HASLINES;
					stylef &= ~TVS_FULLROWSELECT;	// this style does not work with the TVS_FULLROWSELECT style
					break;
				case L"nohaslines"_hash: // haslines
					stylef &= ~TVS_HASLINES;
					break;
				case L"trackselect"_hash: // trackselect (off by default)
					stylef |= TVS_TRACKSELECT;
					break;
				case L"notrackselect"_hash: // notrackselect
					stylef &= ~TVS_TRACKSELECT;
					break;
				case L"tooltips"_hash: // tooltips (on by default)
					stylef &= ~TVS_NOTOOLTIPS;
					break;
				case L"notooltips"_hash: // notooltips
					stylef |= TVS_NOTOOLTIPS;
					break;
				case L"infotip"_hash: // infotip (on by default)
					stylef |= TVS_INFOTIP;
					break;
				case L"noinfotip"_hash: // noinfotip
					stylef &= ~TVS_INFOTIP;
					break;
				case L"hasbuttons"_hash: // hasbuttons (on by default)
					stylef |= TVS_HASBUTTONS;
					break;
				case L"nohasbuttons"_hash: // nohasbuttons
					stylef &= ~TVS_HASBUTTONS;
					break;
				case L"rootlines"_hash: // rootlines (on by default)
					stylef |= TVS_LINESATROOT;
					break;
				case L"norootlines"_hash: // norootlines
					stylef &= ~TVS_LINESATROOT;
					break;
				case L"singleexpand"_hash: // singleexpand (off by default)
					stylef |= TVS_SINGLEEXPAND;
					break;
				case L"nosingleexpand"_hash: // nosingleexpand
					stylef &= ~TVS_SINGLEEXPAND;
					break;
				case L"scroll"_hash: // scroll (off by default)
					stylef &= ~TVS_NOSCROLL;
					break;
				case L"noscroll"_hash: // noscroll (NB: this can lead to gfx glitches with scroll bars already shown)
					stylef |= TVS_NOSCROLL;
					break;
				case L"showsel"_hash: // showsel (on by default)
					stylef |= TVS_SHOWSELALWAYS;
					break;
				case L"noshowsel"_hash: // noshowsel
					stylef &= ~TVS_SHOWSELALWAYS;
					break;
				case L"transparent"_hash: // transparent
					exstylef |= WS_EX_TRANSPARENT;
					break;
				case L"notransparent"_hash: // notransparent
					exstylef &= ~WS_EX_TRANSPARENT;
					break;
				case L"fadebuttons"_hash: // fadebuttons
					tvexstylef |= TVS_EX_FADEINOUTEXPANDOS;
					tvexstylemask |= TVS_EX_FADEINOUTEXPANDOS;
					break;
				case L"nofadebuttons"_hash: // nofadebuttons
					tvexstylef &= ~TVS_EX_FADEINOUTEXPANDOS;
					tvexstylemask |= TVS_EX_FADEINOUTEXPANDOS;
					break;
				case L"ident"_hash: // indent
					tvexstylef &= ~TVS_EX_NOINDENTSTATE;
					tvexstylemask |= TVS_EX_NOINDENTSTATE;
					break;
				case L"noident"_hash: // noident
					tvexstylef |= TVS_EX_NOINDENTSTATE;
					tvexstylemask |= TVS_EX_NOINDENTSTATE;
					break;
				case L"doublebuffer"_hash: // buffer
				case L"buffer"_hash: // buffer
					tvexstylef |= TVS_EX_DOUBLEBUFFER;
					tvexstylemask |= TVS_EX_DOUBLEBUFFER;
					break;
				case L"nodoublebuffer"_hash: // nobuffer
				case L"nobuffer"_hash: // nobuffer
					tvexstylef &= ~TVS_EX_DOUBLEBUFFER;
					tvexstylemask |= TVS_EX_DOUBLEBUFFER;
					break;
				case L"autoscroll"_hash: // autohscroll
					tvexstylef |= TVS_EX_AUTOHSCROLL;
					tvexstylemask |= TVS_EX_AUTOHSCROLL;
					break;
				case L"noautoscroll"_hash: // noautohscroll
					tvexstylef &= ~TVS_EX_AUTOHSCROLL;
					tvexstylemask |= TVS_EX_AUTOHSCROLL;
					break;
				case L"richtooltip"_hash: // richtooltip
					tvexstylef |= TVS_EX_RICHTOOLTIP;
					tvexstylemask |= TVS_EX_RICHTOOLTIP;
					break;
				case L"norichtooltip"_hash: // norichtooltip
					tvexstylef &= ~TVS_EX_RICHTOOLTIP;
					tvexstylemask |= TVS_EX_RICHTOOLTIP;
					break;
				case L"balloon"_hash: // balloon (off by default)
				{
					if (const auto tips = Dcx::dcxTreeView_GetToolTips(mIRCLinker::getTreeview()); tips)
						AddStyles(tips, GWL_STYLE, TTS_BALLOON);
				}
				break;
				case L"noballoon"_hash: // noballoon
				{
					if (const auto tips = Dcx::dcxTreeView_GetToolTips(mIRCLinker::getTreeview()); tips)
						RemStyles(tips, GWL_STYLE, TTS_BALLOON);
				}
				break;
				default: // unknown style ignore.
					throw Dcx::dcxException(TEXT("Unknown Style: %"), x);
				}
			}

			dcxSetWindowStyle(mIRCLinker::getTreeview(), stylef);
			dcxSetWindowExStyle(mIRCLinker::getTreeview(), exstylef);

			if (Dcx::DwmModule.isUseable())
				Dcx::dcxTreeView_SetExtendedStyle(mIRCLinker::getTreeview(), tvexstylef, tvexstylemask);

			SetWindowPos(mIRCLinker::getTreeview(), nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
		break;
		// -c & -w commands are experimental & required stopping mIRC doing the item drawing.
		case TEXT('c'): // [COLOUR FLAGS] [COLOUR]
		{
			if (numtok < 3)
				throw DcxExceptions::dcxInvalidArguments();

			const auto cflag(input.getnexttok());						// tok 2
			const auto clr = input.getnexttok().to_<COLORREF>();	// tok 3

			if (cflag[0] != TEXT('+'))
				throw DcxExceptions::dcxInvalidFlag();

			switch (cflag[1])
			{
			case TEXT('t'): // text colour
				Dcx::dcxTreeView_SetTextColor(mIRCLinker::getTreeview(), clr);
				break;
			case TEXT('b'): // bkg colour
				Dcx::dcxTreeView_SetBkColor(mIRCLinker::getTreeview(), clr);
				break;
			case TEXT('l'): // line colour
				Dcx::dcxTreeView_SetLineColor(mIRCLinker::getTreeview(), clr);
				break;
			case TEXT('i'): // insert mark colour
				Dcx::dcxTreeView_SetInsertMarkColor(mIRCLinker::getTreeview(), clr);
				break;
			case TEXT('s'): // selected text colour
				gsl::at(DcxDock::g_clrTreebarColours, gsl::narrow_cast<UINT>(TreeBarColours::TREEBAR_COLOUR_SELECTED)) = clr;
				break;
			case TEXT('S'): // selected bkg colour
				gsl::at(DcxDock::g_clrTreebarColours, gsl::narrow_cast<UINT>(TreeBarColours::TREEBAR_COLOUR_SELECTED_BKG)) = clr;
				break;
			case TEXT('m'): // message colour
				gsl::at(DcxDock::g_clrTreebarColours, gsl::narrow_cast<UINT>(TreeBarColours::TREEBAR_COLOUR_MESSAGE)) = clr;
				break;
			case TEXT('M'): // message bkg colour
				gsl::at(DcxDock::g_clrTreebarColours, gsl::narrow_cast<UINT>(TreeBarColours::TREEBAR_COLOUR_MESSAGE_BKG)) = clr;
				break;
			case TEXT('e'): // event colour
				gsl::at(DcxDock::g_clrTreebarColours, gsl::narrow_cast<UINT>(TreeBarColours::TREEBAR_COLOUR_EVENT)) = clr;
				break;
			case TEXT('E'): // event bkg colour
				gsl::at(DcxDock::g_clrTreebarColours, gsl::narrow_cast<UINT>(TreeBarColours::TREEBAR_COLOUR_EVENT_BKG)) = clr;
				break;
			case TEXT('z'): // highlight colour
				gsl::at(DcxDock::g_clrTreebarColours, gsl::narrow_cast<UINT>(TreeBarColours::TREEBAR_COLOUR_HIGHLIGHT)) = clr;
				break;
			case TEXT('Z'): // highlight bkg colour
				gsl::at(DcxDock::g_clrTreebarColours, gsl::narrow_cast<UINT>(TreeBarColours::TREEBAR_COLOUR_HIGHLIGHT_BKG)) = clr;
				break;
			case TEXT('h'): // hot text colour
				gsl::at(DcxDock::g_clrTreebarColours, gsl::narrow_cast<UINT>(TreeBarColours::TREEBAR_COLOUR_HOT_TEXT)) = clr;
				break;
			case TEXT('H'): // hot bkg colour
				gsl::at(DcxDock::g_clrTreebarColours, gsl::narrow_cast<UINT>(TreeBarColours::TREEBAR_COLOUR_HOT_BKG)) = clr;
				break;
			default:
				throw DcxExceptions::dcxInvalidFlag();
			}
		}
		break;
		// [clear|default]
		// [index] [+flags] [N1,N2-N3] [filename]
		case TEXT('w'): // [clear|default] | [index] [+flags] [icon index] [filename]
		{
			if (!mIRCLinker::getTreeImages())
				throw Dcx::dcxException("No Valid TreeView Image List");

			const auto tsIndex(input.getnexttok());		// tok 2
			if (tsIndex == TEXT("clear"))
			{ // no images.
				if (const auto o = Dcx::dcxTreeView_SetImageList(mIRCLinker::getTreeview(), nullptr, TVSIL_NORMAL); (o && o != mIRCLinker::getTreeImages()))
					ImageList_Destroy(o);
			}
			else if (tsIndex == TEXT("default"))
			{ // mIRC's default image list
				if (const auto o = Dcx::dcxTreeView_SetImageList(mIRCLinker::getTreeview(), mIRCLinker::getTreeImages(), TVSIL_NORMAL); (o && o != mIRCLinker::getTreeImages()))
					ImageList_Destroy(o);
			}
			else { // our custom image list
				HIMAGELIST himl{ nullptr };
				if (auto ohiml = Dcx::dcxTreeView_GetImageList(mIRCLinker::getTreeview(), TVSIL_NORMAL); (ohiml && ohiml != mIRCLinker::getTreeImages()))
					himl = ohiml;
				else {
					int w{}, h{};
					if (!ImageList_GetIconSize(mIRCLinker::getTreeImages(), &w, &h)) // try to get image size.
						w = h = 16; // default to 16x16

					himl = ImageList_Create(w, h, ILC_COLOR32 | ILC_MASK, 1, 0);
					if (himl)
						Dcx::dcxTreeView_SetImageList(mIRCLinker::getTreeview(), himl, TVSIL_NORMAL);
				}
				if (!himl)
					throw DcxExceptions::dcxUnableToCreateImageList();

				auto iIndex = tsIndex.to_int() - 1;
				const auto cflag(input.getnexttok().trim());	// tok 3
				const auto fIndex = input.getnexttok().to_int(), iCnt = ImageList_GetImageCount(himl) - 1;	// tok 4
				auto filename(input.getlasttoks().trim());	// tok 5, -1

				// check index is within range.
				if (iCnt < iIndex)
					throw DcxExceptions::dcxOutOfRange();

				if (iIndex < 0)
					iIndex = -1; // append to end of list. make sure its only -1

				if (fIndex < 0)
				{ // file index is -1, so add ALL icons in file at iIndex pos.
					AddFileIcons(himl, filename, false, iIndex);
				}
				else {
#if DCX_USE_WRAPPERS
					Dcx::dcxIconResource icon(fIndex, filename, false, cflag);

					ImageList_ReplaceIcon(himl, iIndex, icon.get());
#else
					HICON hIcon = dcxLoadIcon(fIndex, filename, false, cflag);
					if (!hIcon)
						throw DcxExceptions::dcxUnableToLoadIcon();

					ImageList_ReplaceIcon(himl, iIndex, hIcon);
					DestroyIcon(hIcon);
#endif
				}
			}
		}
		break;
		case TEXT('T'): // [1|0]
		{ // Take over Treebar drawing
			DcxDock::g_bTakeOverTreebar = (input.getnexttok().to_int() > 0);	// tok 2
			if (DcxDock::g_bTakeOverTreebar)
			{
				if (mIRCLinker::isAlias(L"xtreebar_callback"_ts))
					TraverseTreebarItems();
				else {
					DcxDock::g_bTakeOverTreebar = false;
					throw Dcx::dcxException("No $xtreebar_callback() alias found");
				}
			}
		}
		break;
		default:
			throw DcxExceptions::dcxInvalidFlag();
		}
		RedrawWindow(mIRCLinker::getTreeview(), nullptr, nullptr, RDW_INTERNALPAINT | RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_ERASE);
		return 1;
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("/xtreebar"), TEXT("\"%\" error: %"), input, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("/xtreebar"), TEXT("\"%\" error: Unknown Exception"), input);
	}
	return 0;
}

// prop
GSL_SUPPRESS(type.4)
mIRC(_xtreebar)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() != 3)
			throw DcxExceptions::dcxInvalidArguments(); // Dcx::dcxException("Invalid Args: An Index & a Prop are required.");

		const auto cnt = Dcx::dcxTreeView_GetCount(mIRCLinker::getTreeview());
		const auto nHash = std::hash<TString>{}(d.getfirsttok(2));
		auto index = d.getnexttok().to_<UINT>();

		if (index > cnt)
			throw DcxExceptions::dcxInvalidItem(); // Dcx::dcxException("Invalid Item Index");

		TVITEMEX item{};

		switch (nHash)
		{
		case TEXT("item"_hash):
		{
			if (index < 1) // if index < 1 return total items.
				_ts_snprintf(data, mIRCLinker::m_dwCharacters, TEXT("%u"), cnt);
			else {
				item.hItem = Dcx::dcxTreeView_MapIndexToItem(mIRCLinker::getTreeview(), index);
				item.mask = TVIF_TEXT;
				item.pszText = data;
				item.cchTextMax = mIRCLinker::m_dwCharacters;
				if (!Dcx::dcxTreeView_GetItem(mIRCLinker::getTreeview(), &item))
					throw DcxExceptions::dcxUnableToGetItem();

				if (data != item.pszText) // item.pszText can be changed from the buffer supplied.
					dcx_strcpyn(data, item.pszText, mIRCLinker::m_dwCharacters);
			}
		}
		break;
		case TEXT("icons"_hash):
		{
			if (index < 1) // if index < 1 make it the last item.
				index = gsl::narrow_cast<UINT>(cnt - 1);

			item.hItem = Dcx::dcxTreeView_MapIndexToItem(mIRCLinker::getTreeview(), index);
			item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_EXPANDEDIMAGE;
			if (!Dcx::dcxTreeView_GetItem(mIRCLinker::getTreeview(), &item))
				throw DcxExceptions::dcxUnableToGetItem();

			_ts_snprintf(data, mIRCLinker::m_dwCharacters, TEXT("%d %d %d"), item.iImage, item.iSelectedImage, item.iExpandedImage);
		}
		break;
		default:	// error
			throw Dcx::dcxException(TEXT("Invalid prop ().%"), d.gettok(2));
		}
		return 3;
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("/xtreebar"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("/xtreebar"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	return 0;
}
