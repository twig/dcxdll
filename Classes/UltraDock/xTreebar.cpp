
/*
*/
#include "defines.h"
#include "Classes/UltraDock/dcxDock.h"
#include "Dcx.h"



void TraverseChildren(const HTREEITEM hParent, TString &buf, TString &res, LPTVITEMEX pitem)
{
	ZeroMemory(pitem, sizeof(TVITEMEX));
	for (HTREEITEM ptvitem = TreeView_GetChild(Dcx::mIRC.getTreeview(), hParent); ptvitem != NULL; ptvitem = TreeView_GetNextSibling(Dcx::mIRC.getTreeview(), ptvitem)) {
		pitem->hItem = ptvitem;
		pitem->pszText = buf.to_chr();
		pitem->cchTextMax = MIRC_BUFFER_SIZE_CCH;
		pitem->mask = TVIF_TEXT|TVIF_PARAM;
		if (TreeView_GetItem(Dcx::mIRC.getTreeview(), pitem))
		{
			{
				TString tsType;
				DcxDock::getTreebarItemType(tsType, pitem->lParam);
				Dcx::mIRC.execex(TEXT("/!set -nu1 %%dcx_%d %s"), pitem->lParam, pitem->pszText );
				Dcx::mIRC.tsEvalex(res, TEXT("$xtreebar_callback(geticons,%s,%%dcx_%d)"), tsType.to_chr(), pitem->lParam);
			}
			pitem->mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE;
			int i = res.gettok( 1 ).to_int() -1;
			if (i < 0)
				i = 0;
			pitem->iImage = i;
			i = res.gettok( 2 ).to_int() -1;
			if (i < 0)
				i = 0;
			pitem->iSelectedImage = i;
			TreeView_SetItem(Dcx::mIRC.getTreeview(), pitem);
		}
		TraverseChildren(ptvitem, buf, res, pitem);
	}
}

void TraverseTreebarItems(void)
{
	SetWindowRedraw(Dcx::mIRC.getTreeview(), FALSE);
	TString buf((UINT)MIRC_BUFFER_SIZE_CCH);
	TString res;
	TVITEMEX item;
	ZeroMemory(&item, sizeof(item));
	for (HTREEITEM ptvitem = TreeView_GetRoot(Dcx::mIRC.getTreeview()); ptvitem != NULL; ptvitem = TreeView_GetNextSibling(Dcx::mIRC.getTreeview(), ptvitem)) {
		item.hItem = ptvitem;
		item.pszText = buf.to_chr();
		item.cchTextMax = MIRC_BUFFER_SIZE_CCH;
		item.mask = TVIF_TEXT|TVIF_PARAM;
		if (TreeView_GetItem(Dcx::mIRC.getTreeview(), &item))
		{
			{
				TString tsType;
				DcxDock::getTreebarItemType(tsType, item.lParam);
				Dcx::mIRC.execex(TEXT("/!set -nu1 %%dcx_%d %s"), item.lParam, item.pszText );
				Dcx::mIRC.tsEvalex(res, TEXT("$xtreebar_callback(geticons,%s,%%dcx_%d)"), tsType.to_chr(), item.lParam);
			}
			item.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE;
			int i = res.gettok( 1 ).to_int() -1;
			if (i < 0)
				i = 0;
			item.iImage = i;
			i = res.gettok( 2 ).to_int() -1;
			if (i < 0)
				i = 0;
			item.iSelectedImage = i;
			TreeView_SetItem(Dcx::mIRC.getTreeview(), &item);
		}
		TraverseChildren(ptvitem, buf, res, &item);
	}
	SetWindowRedraw(Dcx::mIRC.getTreeview(), TRUE);
}

// [SWITCH] [OPTIONS]
mIRC(xtreebar) {
	TString input(data);
	input.trim();
	data[0] = 0;

	int numtok = input.numtok( );

	if (!IsWindow(Dcx::mIRC.getTreeview())) {
		Dcx::error(TEXT("/xtreebar"), TEXT("No Treebar"));
		return 0;
	}

	TString switches(input.gettok(1));

	if (switches[0] != TEXT('-')) {
		Dcx::error(TEXT("/xtreebar"), TEXT("Invalid Switch"));
		return 0;
	}

	switch (switches[1]) {
		//case TEXT('a'):	// [] add items to treeview
		//	{
		//	}
		//	break;
		case TEXT('f'): // [+FONTFLAGS] [CHARSET] [SIZE] [FONTNAME]
			{
				if (numtok < 5) {
					Dcx::error(TEXT("/xtreebar -f"), TEXT("Invalid Font Args"));
					return 0;
				}
				LOGFONT lf;

				if (ParseCommandToLogfont(input.gettok(2, -1), &lf)) {
					HFONT hFont = CreateFontIndirect(&lf);
					if (hFont != NULL) {
						Dcx::mIRC.setTreeFont(hFont);
					}
				}
			}
			break;
		case TEXT('s'): // [STYLES]
			{
				if (numtok < 2) {
					Dcx::error(TEXT("/xtreebar -s"),TEXT("Invalid Style Args"));
					return 0;
				}
#ifdef DCX_USE_WINSDK
				static const TString treebar_styles(TEXT("trackselect notrackselect tooltips notooltips infotip noinfotip hasbuttons nohasbuttons rootlines norootlines singleexpand nosingleexpand scroll noscroll showsel noshowsel transparent notransparent fadebuttons nofadebuttons indent noident buffer nobuffer autohscroll noautohscroll richtooltip norichtooltip balloon noballoon"));
#else
				static const TString treebar_styles(TEXT("trackselect notrackselect tooltips notooltips infotip noinfotip hasbuttons nohasbuttons rootlines norootlines singleexpand nosingleexpand scroll noscroll showsel noshowsel transparent notransparent balloon noballoon"));
#endif
				int i = 2;
				DWORD stylef = GetWindowStyle(Dcx::mIRC.getTreeview());
				DWORD exstylef = GetWindowExStyle(Dcx::mIRC.getTreeview());
#ifdef DCX_USE_WINSDK
				DWORD tvexstylef = 0;
				if (Dcx::VistaModule.isUseable())
					tvexstylef = TreeView_GetExtendedStyle(Dcx::mIRC.getTreeview());
				DWORD tvexstylemask = 0;
#endif
				enum TreebarStyles {
					TS_TRACK = 1, TS_NOTRACK, TS_TOOLTIPS, TS_NOTOOLTIPS, TS_INFOTIP, TS_NOINFOTIP, TS_HASBUTTONS, TS_NOHASBUTTONS, TS_ROOTLINES, TS_NOROOTLINES, TS_SINGLEEXPAND, TS_NOSINGLEEXPAND, TS_SCROLL, TS_NOSCROLL, TS_SHOWSEL, TS_NOSHOWSEL, TS_TRANSPARENT, TS_NOTRANSPARENT,
#ifdef DCX_USE_WINSDK
					TS_FADEBUTTONS, TS_NOFADEBUTTONS, TS_INDENT, TS_NOINDENT, TS_BUFFER, TS_NOBUFFER, TS_AUTOHSCROLL, TS_NOAUTOHSCROLL, TS_RICHTOOLTIP, TS_NORICHTOOLTIP,
#endif
					TS_BALLOON, TS_NOBALLOON
				};
				while (i <= numtok) {
					switch (treebar_styles.findtok(input.gettok(i).to_chr(),1))
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
#ifdef DCX_USE_WINSDK
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
#endif
					case TS_BALLOON: // balloon (off by default)
						{
							HWND tips = TreeView_GetToolTips(Dcx::mIRC.getTreeview());
							if (tips != NULL)
								AddStyles(tips, GWL_STYLE, TTS_BALLOON);
						}
						break;
					case TS_NOBALLOON: // noballoon
						{
							HWND tips = TreeView_GetToolTips(Dcx::mIRC.getTreeview());
							if (tips != NULL)
								RemStyles(tips, GWL_STYLE, TTS_BALLOON);
						}
						break;
					default: // unknown style ignore.
						{
							Dcx::errorex(TEXT("/xtreebar -s"), TEXT("Unknown Style: %s"), input.gettok(i).to_chr());
						}
						break;
					}
					i++;
				}
				SetWindowLongPtr(Dcx::mIRC.getTreeview(),GWL_STYLE, stylef);
				SetWindowLongPtr(Dcx::mIRC.getTreeview(),GWL_EXSTYLE, exstylef);
#ifdef DCX_USE_WINSDK
				if (Dcx::VistaModule.isUseable())
					TreeView_SetExtendedStyle(Dcx::mIRC.getTreeview(), tvexstylef, tvexstylemask);
#endif
				SetWindowPos(Dcx::mIRC.getTreeview(), NULL, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
			}
			break;
			// -c & -w commands are experimental & required stopping mIRC doing the item drawing.
		case TEXT('c'): // [COLOUR FLAGS] [COLOUR]
			{
				if (numtok < 3) {
					Dcx::error(TEXT("/xtreebar -c"), TEXT("Invalid Colour Args"));
					return 0;
				}
				TString cflag(input.gettok(2));
				COLORREF clr = (COLORREF)input.gettok(3).to_num();

				if (cflag[0] != TEXT('+')) {
					Dcx::error(TEXT("/xtreebar -c"),TEXT("Invalid Colour flag"));
					return 0;
				}

				switch(cflag[1])
				{
					case TEXT('t'): // text colour
						TreeView_SetTextColor(Dcx::mIRC.getTreeview(),clr);
						break;
					case TEXT('b'): // bkg colour
						TreeView_SetBkColor(Dcx::mIRC.getTreeview(),clr);
						break;
					case TEXT('l'): // line colour
						TreeView_SetLineColor(Dcx::mIRC.getTreeview(),clr);
						break;
					case TEXT('i'): // insert mark colour
						TreeView_SetInsertMarkColor(Dcx::mIRC.getTreeview(),clr);
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
						Dcx::error(TEXT("/xtreebar -c"),TEXT("Invalid Colour flag"));
						return 0;
				}
			}
			break;
		case TEXT('w'): // [clear|default|mirc] | [index] [+flags] [icon index] [filename]
			{
				if (Dcx::mIRC.getTreeImages() == NULL) {
					Dcx::error(TEXT("/xtreebar -w"), TEXT("No Valid TreeView Image List"));
					return 0;
				}
				TString tsIndex(input.gettok(2));
				if (tsIndex == TEXT("clear")) { // no images.
					HIMAGELIST o = TreeView_SetImageList(Dcx::mIRC.getTreeview(),NULL,TVSIL_NORMAL);
					if (o != NULL && o != Dcx::mIRC.getTreeImages())
						ImageList_Destroy(o);
				}
				else if (tsIndex == TEXT("default")) { // mIRC's default image list
					HIMAGELIST o = TreeView_SetImageList(Dcx::mIRC.getTreeview(),Dcx::mIRC.getTreeImages(),TVSIL_NORMAL);
					if (o != NULL && o != Dcx::mIRC.getTreeImages())
						ImageList_Destroy(o);
				}
				else { // our custom image list
					HIMAGELIST himl = NULL, ohiml = TreeView_GetImageList( Dcx::mIRC.getTreeview(), TVSIL_NORMAL);
					if (ohiml != NULL && ohiml != Dcx::mIRC.getTreeImages())
						himl = ohiml;
					else {
						int w, h;
						if (!ImageList_GetIconSize(Dcx::mIRC.getTreeImages(), &w, &h)) // try to get image size.
							w = h = 16; // default to 16x16

						himl = ImageList_Create(w,h,ILC_COLOR32|ILC_MASK,1,0);
						if (himl != NULL)
							TreeView_SetImageList(Dcx::mIRC.getTreeview(), himl, TVSIL_NORMAL);
					}
					if (himl != NULL) {
						int iIndex = tsIndex.to_int() -1, fIndex = input.gettok(4).to_int(), iCnt = ImageList_GetImageCount(himl) -1;
						TString cflag(input.gettok(3).trim());
						TString filename(input.gettok(5,-1).trim());

						// check index is within range.
						if (iCnt < iIndex) {
							Dcx::error(TEXT("/xtreebar -w"), TEXT("Image Index Out Of Range"));
							return 0;
						}
						if (iIndex < 0)
							iIndex = -1; // append to end of list. make sure its only -1

						if (fIndex < 0) { // file index is -1, so add ALL icons in file at iIndex pos.
							if (!AddFileIcons(himl, filename, false, iIndex)) {
								Dcx::errorex(TEXT("/xtreebar -w"), TEXT("Unable to Add %s's Icons"), filename.to_chr());
								return 0;
							}
						}
						else {
							HICON hIcon = dcxLoadIcon(fIndex,filename, false, cflag);
							if (hIcon != NULL) {
								ImageList_ReplaceIcon(himl, iIndex, hIcon);
								DestroyIcon(hIcon);
							}
							else {
								Dcx::error(TEXT("/xtreebar -w"), TEXT("Unable to load icon"));
								return 0;
							}
						}
					}
					else {
						Dcx::error(TEXT("/xtreebar -w"), TEXT("Unable to Create ImageList"));
						return 0;
					}
				}
			}
			break;
		case TEXT('T'): // [1|0]
			{ // Take over Treebar drawing
				DcxDock::g_bTakeOverTreebar = (input.gettok( 2 ).to_int() ? true : false);
				if (DcxDock::g_bTakeOverTreebar) {
					if (Dcx::mIRC.isAlias(TEXT("xtreebar_callback")))
						TraverseTreebarItems();
					else {
						DcxDock::g_bTakeOverTreebar = false;
						Dcx::error(TEXT("/xtreebar"), TEXT("No $xtreebar_callback() alias found"));
						return 0;
					}
				}
			}
			break;
		default:
			Dcx::error(TEXT("/xtreebar"), TEXT("Invalid Flag"));
			return 0;
	}
	RedrawWindow(Dcx::mIRC.getTreeview(), NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE );
	return 1;
}

// prop
mIRC(_xtreebar)
{
	TString d(data);
	d.trim();

	data[0] = 0;

	if (d.numtok( ) != 3)
		ret(TEXT("D_ERROR Invalid Args: An Index & a Prop are required."));

	static const TString poslist(TEXT("item icons"));
	int nType = poslist.findtok(d.gettok( 2 ).to_chr(),1);
	int cnt = TreeView_GetCount(Dcx::mIRC.getTreeview());
	int index = d.gettok( 3 ).to_int();

	if (index > cnt)
		ret(TEXT("D_ERROR Invalid Item Index"));

	TVITEMEX item;
	ZeroMemory(&item,sizeof(item));

	switch (nType)
	{
	case 1: // item
		{
			if (index < 1) // if index < 1 return total items.
				wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), cnt);
			else {
				TCHAR szbuf[MIRC_BUFFER_SIZE_CCH];
				item.hItem = TreeView_MapAccIDToHTREEITEM(Dcx::mIRC.getTreeview(), index);
				item.mask = TVIF_TEXT;
				item.pszText = szbuf;
				item.cchTextMax = MIRC_BUFFER_SIZE_CCH;
				if (TreeView_GetItem(Dcx::mIRC.getTreeview(),&item))
					lstrcpyn(data, item.pszText, MIRC_BUFFER_SIZE_CCH);
				else
					lstrcpyn(data, TEXT("D_ERROR Unable To Get Item"), MIRC_BUFFER_SIZE_CCH);
			}
		}
		break;
	case 2: // icons
		{
			if (index < 1) // if index < 1 make it the last item.
				index = cnt;

			item.hItem = TreeView_MapAccIDToHTREEITEM(Dcx::mIRC.getTreeview(), index);
			item.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE;
			if (TreeView_GetItem(Dcx::mIRC.getTreeview(),&item))
				wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), item.iImage, item.iSelectedImage);
			else
				lstrcpyn(data, TEXT("D_ERROR Unable To Get Item"), MIRC_BUFFER_SIZE_CCH);
		}
		break;
	case 0: // error
	default:
		{
			wnsprintf(data, MIRC_BUFFER_SIZE_CCH, TEXT("D_ERROR Invalid prop ().%s"), d.gettok( 2 ).to_chr());
		}
		break;
	}
	return 3;
}
