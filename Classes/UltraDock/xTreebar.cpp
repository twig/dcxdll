
/*
*/
#include "defines.h"
#include "Classes/UltraDock/dcxDock.h"
#include "Dcx.h"

void TraverseChildren(const HTREEITEM hParent, TString &buf, TString &res, LPTVITEMEX pitem)
{
	ZeroMemory(pitem, sizeof(TVITEMEX));
	for (auto ptvitem = TreeView_GetChild(mIRCLinker::getTreeview(), hParent); ptvitem != nullptr; ptvitem = TreeView_GetNextSibling(mIRCLinker::getTreeview(), ptvitem))
	{
		pitem->hItem = ptvitem;
		pitem->pszText = buf.to_chr();
		pitem->cchTextMax = MIRC_BUFFER_SIZE_CCH;
		pitem->mask = TVIF_TEXT|TVIF_PARAM;
		if (TreeView_GetItem(mIRCLinker::getTreeview(), pitem))
		{
			{
				TString tsType(DcxDock::getTreebarItemType(pitem->lParam));

				//mIRCLinker::execex(TEXT("/!set -nu1 %%dcx_%d %s"), pitem->lParam, pitem->pszText );
				//mIRCLinker::tsEvalex(res, TEXT("$xtreebar_callback(geticons,%s,%%dcx_%d)"), tsType.to_chr(), pitem->lParam);
				mIRCLinker::exec(TEXT("/!set -nu1 \\%dcx_% %"), pitem->lParam, pitem->pszText);
				mIRCLinker::eval(res, TEXT("$xtreebar_callback(geticons,%,\\%dcx_%)"), tsType, pitem->lParam);
			}
			pitem->mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_EXPANDEDIMAGE;

			// image
			auto i = res.getfirsttok(1).to_int() - 1;
			pitem->iImage = (i < 0) ? I_IMAGENONE : i;

			// selected image (if none supplied use the standard image)
			i = res.getnexttok( ).to_int() -1;
			pitem->iSelectedImage = (i < 0) ? pitem->iImage : i;

			// expanded image (if none supplied use the standard image)
			i = res.getnexttok().to_int() - 1;
			pitem->iExpandedImage = (i < 0) ? pitem->iImage : i;

			TreeView_SetItem(mIRCLinker::getTreeview(), pitem);
		}
		TraverseChildren(ptvitem, buf, res, pitem);
	}
}

void TraverseTreebarItems(void)
{
	SetWindowRedraw(mIRCLinker::getTreeview(), FALSE);
	Auto(SetWindowRedraw(mIRCLinker::getTreeview(), TRUE));

	TString buf(gsl::narrow_cast<UINT>(mIRCLinker::m_mIRC_Buffer_Size_cch));
	TString res;
	TVITEMEX item{};

	for (auto ptvitem = TreeView_GetRoot(mIRCLinker::getTreeview()); ptvitem != nullptr; ptvitem = TreeView_GetNextSibling(mIRCLinker::getTreeview(), ptvitem))
	{
		item.hItem = ptvitem;
		item.pszText = buf.to_chr();
		item.cchTextMax = MIRC_BUFFER_SIZE_CCH;
		item.mask = TVIF_TEXT|TVIF_PARAM;
		if (TreeView_GetItem(mIRCLinker::getTreeview(), &item))
		{
			{
				TString tsType(DcxDock::getTreebarItemType(item.lParam));

				//mIRCLinker::execex(TEXT("/!set -nu1 %%dcx_%d %s"), item.lParam, item.pszText );
				//mIRCLinker::tsEvalex(res, TEXT("$xtreebar_callback(geticons,%s,%%dcx_%d)"), tsType.to_chr(), item.lParam);
				mIRCLinker::exec(TEXT("/!set -nu1 \\%dcx_% %"), item.lParam, item.pszText);
				mIRCLinker::eval(res, TEXT("$xtreebar_callback(geticons,%,\\%dcx_%)"), tsType, item.lParam);
			}
			item.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_EXPANDEDIMAGE;

			// image
			auto i = res.getfirsttok(1).to_int() - 1;
			item.iImage = (i < 0) ? I_IMAGENONE : i;

			// selected image (if none supplied use the standard image)
			i = res.getnexttok( ).to_int() -1;
			item.iSelectedImage = (i < 0) ? item.iImage : i;

			// expanded image (if none supplied use the standard image)
			i = res.getnexttok().to_int() - 1;
			item.iExpandedImage = (i < 0) ? item.iImage : i;

			TreeView_SetItem(mIRCLinker::getTreeview(), &item);
		}
		TraverseChildren(ptvitem, buf, res, &item);
	}
}

// [SWITCH] [OPTIONS]
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
			throw Dcx::dcxException("Invalid Switch");

		switch (switches[1])
		{
		//case TEXT('a'):	// [] add items to treeview
		//	{
		//	}
		//	break;
		case TEXT('f'): // [+FONTFLAGS] [CHARSET] [SIZE] [FONTNAME]
		{
			if (numtok < 5)
				throw Dcx::dcxException("Invalid Font Args");

			if (LOGFONT lf{}; ParseCommandToLogfont(input.getlasttoks(), &lf))	// tok 2, -1
			{
				if (auto hFont = CreateFontIndirect(&lf); hFont != nullptr)
					mIRCLinker::setTreeFont(hFont);
			}
		}
		break;
		case TEXT('s'): // [STYLES]
		{
			if (numtok < 2)
				throw Dcx::dcxException("Invalid Style Args");

			static const TString treebar_styles(TEXT("trackselect notrackselect tooltips notooltips infotip noinfotip hasbuttons nohasbuttons rootlines norootlines singleexpand nosingleexpand scroll noscroll showsel noshowsel transparent notransparent fadebuttons nofadebuttons indent noident buffer nobuffer autohscroll noautohscroll richtooltip norichtooltip balloon noballoon"));
			auto stylef = dcxGetWindowStyle(mIRCLinker::getTreeview());
			auto exstylef = dcxGetWindowExStyle(mIRCLinker::getTreeview());

			DWORD tvexstylef = 0;
			if (Dcx::VistaModule.isUseable())
				tvexstylef = TreeView_GetExtendedStyle(mIRCLinker::getTreeview());
			DWORD tvexstylemask = 0;

			enum TreebarStyles: UINT {
				TS_TRACK = 1, TS_NOTRACK, TS_TOOLTIPS, TS_NOTOOLTIPS, TS_INFOTIP, TS_NOINFOTIP, TS_HASBUTTONS, TS_NOHASBUTTONS, TS_ROOTLINES, TS_NOROOTLINES, TS_SINGLEEXPAND, TS_NOSINGLEEXPAND, TS_SCROLL, TS_NOSCROLL, TS_SHOWSEL, TS_NOSHOWSEL, TS_TRANSPARENT, TS_NOTRANSPARENT,
				TS_FADEBUTTONS, TS_NOFADEBUTTONS, TS_INDENT, TS_NOINDENT, TS_BUFFER, TS_NOBUFFER, TS_AUTOHSCROLL, TS_NOAUTOHSCROLL, TS_RICHTOOLTIP, TS_NORICHTOOLTIP,
				TS_BALLOON, TS_NOBALLOON
			};

			for (auto i = decltype(numtok){2}; i <= numtok; ++i)
			{
				switch (treebar_styles.findtok(input.getnexttok(), 1))		// tok 2+
				{
				case TS_TRACK: // trackselect (off by default)
					stylef |= TVS_TRACKSELECT;
					break;
				case TS_NOTRACK: // notrackselect
					stylef &= ~TVS_TRACKSELECT;
					break;
				case TS_TOOLTIPS: // tooltips (on by default)
					stylef &= ~TVS_NOTOOLTIPS;
					break;
				case TS_NOTOOLTIPS: // notooltips
					stylef |= TVS_NOTOOLTIPS;
					break;
				case TS_INFOTIP: // infotip (on by default)
					stylef |= TVS_INFOTIP;
					break;
				case TS_NOINFOTIP: // noinfotip
					stylef &= ~TVS_INFOTIP;
					break;
				case TS_HASBUTTONS: // hasbuttons (on by default)
					stylef |= TVS_HASBUTTONS;
					break;
				case TS_NOHASBUTTONS: // nohasbuttons
					stylef &= ~TVS_HASBUTTONS;
					break;
				case TS_ROOTLINES: // rootlines (on by default)
					stylef |= TVS_LINESATROOT;
					break;
				case TS_NOROOTLINES: // norootlines
					stylef &= ~TVS_LINESATROOT;
					break;
				case TS_SINGLEEXPAND: // singleexpand (off by default)
					stylef |= TVS_SINGLEEXPAND;
					break;
				case TS_NOSINGLEEXPAND: // nosingleexpand
					stylef &= ~TVS_SINGLEEXPAND;
					break;
				case TS_SCROLL: // scroll (off by default)
					stylef &= ~TVS_NOSCROLL;
					break;
				case TS_NOSCROLL: // noscroll (NB: this can lead to gfx glitches with scroll bars already shown)
					stylef |= TVS_NOSCROLL;
					break;
				case TS_SHOWSEL: // showsel (on by default)
					stylef |= TVS_SHOWSELALWAYS;
					break;
				case TS_NOSHOWSEL: // noshowsel
					stylef &= ~TVS_SHOWSELALWAYS;
					break;
				case TS_TRANSPARENT: // transparent
					exstylef |= WS_EX_TRANSPARENT;
					break;
				case TS_NOTRANSPARENT: // notransparent
					exstylef &= ~WS_EX_TRANSPARENT;
					break;
				case TS_FADEBUTTONS: // fadebuttons
					tvexstylef |= TVS_EX_FADEINOUTEXPANDOS;
					tvexstylemask |= TVS_EX_FADEINOUTEXPANDOS;
					break;
				case TS_NOFADEBUTTONS: // nofadebuttons
					tvexstylef &= ~TVS_EX_FADEINOUTEXPANDOS;
					tvexstylemask |= TVS_EX_FADEINOUTEXPANDOS;
					break;
				case TS_INDENT: // indent
					tvexstylef &= ~TVS_EX_NOINDENTSTATE;
					tvexstylemask |= TVS_EX_NOINDENTSTATE;
					break;
				case TS_NOINDENT: // noident
					tvexstylef |= TVS_EX_NOINDENTSTATE;
					tvexstylemask |= TVS_EX_NOINDENTSTATE;
					break;
				case TS_BUFFER: // buffer
					tvexstylef |= TVS_EX_DOUBLEBUFFER;
					tvexstylemask |= TVS_EX_DOUBLEBUFFER;
					break;
				case TS_NOBUFFER: // nobuffer
					tvexstylef &= ~TVS_EX_DOUBLEBUFFER;
					tvexstylemask |= TVS_EX_DOUBLEBUFFER;
					break;
				case TS_AUTOHSCROLL: // autohscroll
					tvexstylef |= TVS_EX_AUTOHSCROLL;
					tvexstylemask |= TVS_EX_AUTOHSCROLL;
					break;
				case TS_NOAUTOHSCROLL: // noautohscroll
					tvexstylef &= ~TVS_EX_AUTOHSCROLL;
					tvexstylemask |= TVS_EX_AUTOHSCROLL;
					break;
				case TS_RICHTOOLTIP: // richtooltip
					tvexstylef |= TVS_EX_RICHTOOLTIP;
					tvexstylemask |= TVS_EX_RICHTOOLTIP;
					break;
				case TS_NORICHTOOLTIP: // norichtooltip
					tvexstylef &= ~TVS_EX_RICHTOOLTIP;
					tvexstylemask |= TVS_EX_RICHTOOLTIP;
					break;
				case TS_BALLOON: // balloon (off by default)
				{
					if (auto tips = TreeView_GetToolTips(mIRCLinker::getTreeview()); tips != nullptr)
						AddStyles(tips, GWL_STYLE, TTS_BALLOON);
				}
				break;
				case TS_NOBALLOON: // noballoon
				{
					if (auto tips = TreeView_GetToolTips(mIRCLinker::getTreeview()); tips != nullptr)
						RemStyles(tips, GWL_STYLE, TTS_BALLOON);
				}
				break;
				default: // unknown style ignore.
					throw Dcx::dcxException(TEXT("Unknown Style: %"), input.gettok(gsl::narrow_cast<int>(i)));
				}
			}
			dcxSetWindowStyle(mIRCLinker::getTreeview(), stylef);
			dcxSetWindowExStyle(mIRCLinker::getTreeview(), exstylef);

			if (Dcx::VistaModule.isUseable())
				TreeView_SetExtendedStyle(mIRCLinker::getTreeview(), gsl::narrow_cast<LPARAM>(tvexstylef), gsl::narrow_cast<WPARAM>(tvexstylemask));

			SetWindowPos(mIRCLinker::getTreeview(), nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
		break;
		// -c & -w commands are experimental & required stopping mIRC doing the item drawing.
		case TEXT('c'): // [COLOUR FLAGS] [COLOUR]
		{
			if (numtok < 3)
				throw Dcx::dcxException("Invalid Colour Args");

			const auto cflag(input.getnexttok());						// tok 2
			const auto clr = input.getnexttok().to_<COLORREF>();	// tok 3

			if (cflag[0] != TEXT('+'))
				throw Dcx::dcxException("Invalid Colour flag");

			switch (cflag[1])
			{
			case TEXT('t'): // text colour
				TreeView_SetTextColor(mIRCLinker::getTreeview(), clr);
				break;
			case TEXT('b'): // bkg colour
				TreeView_SetBkColor(mIRCLinker::getTreeview(), clr);
				break;
			case TEXT('l'): // line colour
				TreeView_SetLineColor(mIRCLinker::getTreeview(), clr);
				break;
			case TEXT('i'): // insert mark colour
				TreeView_SetInsertMarkColor(mIRCLinker::getTreeview(), clr);
				break;
			case TEXT('s'): // selected text colour
				DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_SELECTED] = clr;
				break;
			case TEXT('S'): // selected bkg colour
				DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_SELECTED_BKG] = clr;
				break;
			case TEXT('m'): // message colour
				DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_MESSAGE] = clr;
				break;
			case TEXT('M'): // message bkg colour
				DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_MESSAGE_BKG] = clr;
				break;
			case TEXT('e'): // event colour
				DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_EVENT] = clr;
				break;
			case TEXT('E'): // event bkg colour
				DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_EVENT_BKG] = clr;
				break;
			case TEXT('z'): // highlight colour
				DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HIGHLIGHT] = clr;
				break;
			case TEXT('Z'): // highlight bkg colour
				DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HIGHLIGHT_BKG] = clr;
				break;
			case TEXT('h'): // hot text colour
				DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HOT_TEXT] = clr;
				break;
			case TEXT('H'): // hot bkg colour
				DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HOT_BKG] = clr;
				break;
			default:
				throw Dcx::dcxException("Invalid Colour flag");
			}
		}
		break;
		case TEXT('w'): // [clear|default|mirc] | [index] [+flags] [icon index] [filename]
		{
			if (mIRCLinker::getTreeImages() == nullptr)
				throw Dcx::dcxException("No Valid TreeView Image List");

			const auto tsIndex(input.getnexttok());		// tok 2
			if (tsIndex == TEXT("clear"))
			{ // no images.
				if (auto o = TreeView_SetImageList(mIRCLinker::getTreeview(), nullptr, TVSIL_NORMAL); (o != nullptr && o != mIRCLinker::getTreeImages()))
					ImageList_Destroy(o);
			}
			else if (tsIndex == TEXT("default"))
			{ // mIRC's default image list
				if (auto o = TreeView_SetImageList(mIRCLinker::getTreeview(), mIRCLinker::getTreeImages(), TVSIL_NORMAL); (o != nullptr && o != mIRCLinker::getTreeImages()))
					ImageList_Destroy(o);
			}
			else { // our custom image list
				HIMAGELIST himl = nullptr, ohiml = TreeView_GetImageList(mIRCLinker::getTreeview(), TVSIL_NORMAL);
				if (ohiml != nullptr && ohiml != mIRCLinker::getTreeImages())
					himl = ohiml;
				else {
					int w, h;
					if (!ImageList_GetIconSize(mIRCLinker::getTreeImages(), &w, &h)) // try to get image size.
						w = h = 16; // default to 16x16

					himl = ImageList_Create(w, h, ILC_COLOR32 | ILC_MASK, 1, 0);
					if (himl != nullptr)
						TreeView_SetImageList(mIRCLinker::getTreeview(), himl, TVSIL_NORMAL);
				}
				if (himl == nullptr)
					throw Dcx::dcxException("Unable to Create ImageList");

				auto iIndex = tsIndex.to_int() - 1;
				const auto cflag(input.getnexttok().trim());	// tok 3
				const auto fIndex = input.getnexttok().to_int(), iCnt = ImageList_GetImageCount(himl) - 1;	// tok 4
				auto filename(input.getlasttoks().trim());	// tok 5, -1

				// check index is within range.
				if (iCnt < iIndex)
					throw Dcx::dcxException("Image Index Out Of Range");

				if (iIndex < 0)
					iIndex = -1; // append to end of list. make sure its only -1

				if (fIndex < 0)
				{ // file index is -1, so add ALL icons in file at iIndex pos.
					if (!AddFileIcons(himl, filename, false, iIndex))
						throw Dcx::dcxException(TEXT("Unable to Add %'s Icons"), filename);
				}
				else {
#if DCX_USE_WRAPPERS
					Dcx::dcxIconResource icon(fIndex, filename, false, cflag);

					ImageList_ReplaceIcon(himl, iIndex, icon);
#else
					HICON hIcon = dcxLoadIcon(fIndex, filename, false, cflag);
					if (hIcon == nullptr)
						throw Dcx::dcxException("Unable to load icon");
					
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
			throw Dcx::dcxException("Invalid Flag");
		}
		RedrawWindow(mIRCLinker::getTreeview(), nullptr, nullptr, RDW_INTERNALPAINT | RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_ERASE);
		return 1;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("/xtreebar"), TEXT("\"%s\" error: %S"), input.to_chr(), e.what());
		Dcx::error(TEXT("/xtreebar"), TEXT("\"%\" error: %"), input, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/xtreebar"), TEXT("\"%s\" error: Unknown Exception"), input.to_chr());
		Dcx::error(TEXT("/xtreebar"), TEXT("\"%\" error: Unknown Exception"), input);
	}
	return 0;
}

// prop
mIRC(_xtreebar)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() != 3)
			throw Dcx::dcxException("Invalid Args: An Index & a Prop are required.");

		const auto cnt = TreeView_GetCount(mIRCLinker::getTreeview());
		auto nHash = std::hash<TString>{}(d.getfirsttok(2));
		auto index = d.getnexttok().to_<UINT>();

		if (index > cnt)
			throw Dcx::dcxException("Invalid Item Index");

		TVITEMEX item{};

		switch (nHash)
		{
		case TEXT("item"_hash):
		{
			if (index < 1) // if index < 1 return total items.
				_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), cnt);
			else {
				stString<MIRC_BUFFER_SIZE_CCH> szbuf;
				item.hItem = TreeView_MapAccIDToHTREEITEM(mIRCLinker::getTreeview(), index);
				item.mask = TVIF_TEXT;
				item.pszText = szbuf;	// PVS-Studio reports `V507 pointer stored outside of scope` this is fine.
				item.cchTextMax = MIRC_BUFFER_SIZE_CCH;
				if (!TreeView_GetItem(mIRCLinker::getTreeview(), &item))
					throw Dcx::dcxException("Unable To Get Item");

				dcx_strcpyn(data, item.pszText, MIRC_BUFFER_SIZE_CCH);
			}
		}
		break;
		case TEXT("icons"_hash):
		{
			if (index < 1) // if index < 1 make it the last item.
				index = cnt;

			item.hItem = TreeView_MapAccIDToHTREEITEM(mIRCLinker::getTreeview(), index);
			item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			if (!TreeView_GetItem(mIRCLinker::getTreeview(), &item))
				throw Dcx::dcxException("Unable To Get Item");

			_ts_snprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), item.iImage, item.iSelectedImage);
		}
		break;
		default:	// error
			throw Dcx::dcxException(TEXT("Invalid prop ().%"), d.gettok(2));
		}
		return 3;
	}
	catch (const std::exception &e)
	{
		//Dcx::errorex(TEXT("/xtreebar"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("/xtreebar"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("/xtreebar"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("/xtreebar"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	return 0;
}
