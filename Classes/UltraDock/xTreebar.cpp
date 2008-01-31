
/*
*/

#include "dcxDock.h"

void TraverseChildren(const HTREEITEM hParent, TString &buf, TString &res, LPTVITEMEX pitem)
{
	ZeroMemory(pitem, sizeof(TVITEMEX));
	for (HTREEITEM ptvitem = TreeView_GetChild(mIRCLink.m_hTreeView, hParent); ptvitem != NULL; ptvitem = TreeView_GetNextSibling(mIRCLink.m_hTreeView, ptvitem)) {
		pitem->hItem = ptvitem;
		pitem->pszText = buf.to_chr();
		pitem->cchTextMax = 255;
		pitem->mask = TVIF_TEXT|TVIF_PARAM;
		if (TreeView_GetItem(mIRCLink.m_hTreeView, pitem)) {
			int i = 0;
			{
				TString tsType((UINT)64);
				DcxDock::getTreebarItemType(tsType, pitem->lParam);
				mIRCevalEX(res.to_chr(), 16, "$xtreebar_callback(geticons,%s,%800s)", tsType.to_chr(), pitem->pszText);
			}
			pitem->mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE;
			i = res.gettok( 1 ).to_int() -1;
			if (i < 0)
				i = 0;
			pitem->iImage = i;
			i = res.gettok( 2 ).to_int() -1;
			if (i < 0)
				i = 0;
			pitem->iSelectedImage = i;
			TreeView_SetItem(mIRCLink.m_hTreeView, pitem);
		}
		TraverseChildren(ptvitem, buf, res, pitem);
	}
}

void TraverseTreebarItems(void)
{
	SetWindowRedraw(mIRCLink.m_hTreeView, FALSE);
	TString buf((UINT)255);
	TString res((UINT)16);
	TVITEMEX item;
	ZeroMemory(&item, sizeof(item));
	for (HTREEITEM ptvitem = TreeView_GetRoot(mIRCLink.m_hTreeView); ptvitem != NULL; ptvitem = TreeView_GetNextSibling(mIRCLink.m_hTreeView, ptvitem)) {
		item.hItem = ptvitem;
		item.pszText = buf.to_chr();
		item.cchTextMax = 255;
		item.mask = TVIF_TEXT|TVIF_PARAM;
		if (TreeView_GetItem(mIRCLink.m_hTreeView, &item)) {
			int i = 0;
			{
				TString tsType((UINT)64);
				DcxDock::getTreebarItemType(tsType, item.lParam);
				mIRCevalEX(res.to_chr(), 16, "$xtreebar_callback(geticons,%s,%800s)", tsType.to_chr(), item.pszText);
			}
			item.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE;
			i = res.gettok( 1 ).to_int() -1;
			if (i < 0)
				i = 0;
			item.iImage = i;
			i = res.gettok( 2 ).to_int() -1;
			if (i < 0)
				i = 0;
			item.iSelectedImage = i;
			TreeView_SetItem(mIRCLink.m_hTreeView, &item);
		}
		TraverseChildren(ptvitem, buf, res, &item);
	}
	SetWindowRedraw(mIRCLink.m_hTreeView, TRUE);
}

// [SWITCH] [OPTIONS]
mIRC(xtreebar) {
	TString input(data);
	input.trim();
	data[0] = 0;

	int numtok = input.numtok( );

	if (!IsWindow(mIRCLink.m_hTreeView)) {
		DCXError("/xtreebar", "No Treebar");
		return 0;
	}

	TString switches(input.gettok(1));

	if (switches[0] != '-') {
		DCXError("/xtreebar", "Invalid Switch");
		return 0;
	}

	switch (switches[1]) {
		case 'f': // [+FONTFLAGS] [CHARSET] [SIZE] [FONTNAME]
			{
				if (numtok < 5) {
					DCXError("/xtreebar -f", "Invalid Font Args");
					return 0;
				}
				LOGFONT lf;

				if (ParseCommandToLogfont(input.gettok(2, -1), &lf)) {
					HFONT hFont = CreateFontIndirect(&lf);
					if (hFont != NULL) {
						HFONT f = GetWindowFont(mIRCLink.m_hTreeView);
						if (mIRCLink.m_hTreeFont == NULL)
							mIRCLink.m_hTreeFont = f;
						SetWindowFont( mIRCLink.m_hTreeView, hFont, TRUE);
						if (f != mIRCLink.m_hTreeFont)
							DeleteObject(f);
					}
				}
			}
			break;
		case 's': // [STYLES]
			{
				if (numtok < 2) {
					DCXError("/xtreebar -s","Invalid Style Args");
					return 0;
				}
#ifdef DCX_USE_WINSDK
				static const TString treebar_styles("trackselect notrackselect tooltips notooltips infotip noinfotip hasbuttons nohasbuttons rootlines norootlines singleexpand nosingleexpand scroll noscroll showsel noshowsel transparent notransparent fadebuttons nofadebuttons indent noident buffer nobuffer autohscroll noautohscroll richtooltip norichtooltip balloon noballoon");
#else
				static const TString treebar_styles("trackselect notrackselect tooltips notooltips infotip noinfotip hasbuttons nohasbuttons rootlines norootlines singleexpand nosingleexpand scroll noscroll showsel noshowsel transparent notransparent balloon noballoon");
#endif
				int i = 2;
				DWORD stylef = GetWindowStyle(mIRCLink.m_hTreeView);
				DWORD exstylef = GetWindowExStyle(mIRCLink.m_hTreeView);
#ifdef DCX_USE_WINSDK
				DWORD tvexstylef = 0;
				if (mIRCLink.m_bVista)
					tvexstylef = TreeView_GetExtendedStyle(mIRCLink.m_hTreeView);
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
							HWND tips = TreeView_GetToolTips(mIRCLink.m_hTreeView);
							if (tips != NULL)
								AddStyles(tips, GWL_STYLE, TTS_BALLOON);
						}
						break;
					case TS_NOBALLOON: // noballoon
						{
							HWND tips = TreeView_GetToolTips(mIRCLink.m_hTreeView);
							if (tips != NULL)
								RemStyles(tips, GWL_STYLE, TTS_BALLOON);
						}
						break;
					default: // unknown style ignore.
						{
							DCXErrorEX("/xtreebar -s", "Unknown Style: %s", input.gettok(i).to_chr());
						}
						break;
					}
					i++;
				}
				SetWindowLong(mIRCLink.m_hTreeView,GWL_STYLE, stylef);
				SetWindowLong(mIRCLink.m_hTreeView,GWL_EXSTYLE, exstylef);
#ifdef DCX_USE_WINSDK
				if (mIRCLink.m_bVista)
					TreeView_SetExtendedStyle(mIRCLink.m_hTreeView, tvexstylef, tvexstylemask);
#endif
				SetWindowPos(mIRCLink.m_hTreeView, NULL, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
			}
			break;
			// -c & -i commands are experimental & required stopping mIRC doing the item drawing.
		case 'c': // [COLOUR FLAGS] [COLOUR]
			{
				if (numtok < 3) {
					DCXError("/xtreebar -c", "Invalid Colour Args");
					return 0;
				}
				TString cflag(input.gettok(2));
				COLORREF clr = (COLORREF)input.gettok(3).to_num();

				if (cflag[0] != '+') {
					DCXError("/xtreebar -c","Invalid Colour flag");
					return 0;
				}

				switch(cflag[1])
				{
					case 't': // text colour
						TreeView_SetTextColor(mIRCLink.m_hTreeView,clr);
						break;
					case 'b': // bkg colour
						TreeView_SetBkColor(mIRCLink.m_hTreeView,clr);
						break;
					case 'l': // line colour
						TreeView_SetLineColor(mIRCLink.m_hTreeView,clr);
						break;
					case 'i': // insert mark colour
						TreeView_SetInsertMarkColor(mIRCLink.m_hTreeView,clr);
						break;
					case 's': // selected text colour
						DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_SELECTED] = clr;
						break;
					case 'S': // selected bkg colour
						DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_SELECTED_BKG] = clr;
						break;
					case 'm': // message colour
						DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_MESSAGE] = clr;
						break;
					case 'M': // message bkg colour
						DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_MESSAGE_BKG] = clr;
						break;
					case 'e': // event colour
						DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_EVENT] = clr;
						break;
					case 'E': // event bkg colour
						DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_EVENT_BKG] = clr;
						break;
					case 'z': // highlight colour
						DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HIGHLIGHT] = clr;
						break;
					case 'Z': // highlight bkg colour
						DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HIGHLIGHT_BKG] = clr;
						break;
					case 'h': // hot text colour
						DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HOT_TEXT] = clr;
						break;
					case 'H': // hot bkg colour
						DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HOT_BKG] = clr;
						break;
					default:
						DCXError("/xtreebar -c","Invalid Colour flag");
						return 0;
				}
			}
			break;
		case 'w': // [clear|default|mirc] | [index] [+flags] [icon index] [filename]
			{
				if (mIRCLink.m_hTreeImages == NULL) {
					DCXError("/xtreebar -w", "No Valid TreeView Image List");
					return 0;
				}
				TString tsIndex(input.gettok(2));
				if (tsIndex == "clear") { // no images.
					HIMAGELIST o = TreeView_SetImageList(mIRCLink.m_hTreeView,NULL,TVSIL_NORMAL);
					if (o != NULL && o != mIRCLink.m_hTreeImages)
						ImageList_Destroy(o);
				}
				else if (tsIndex == "default") { // mIRC's default image list
					HIMAGELIST o = TreeView_SetImageList(mIRCLink.m_hTreeView,mIRCLink.m_hTreeImages,TVSIL_NORMAL);
					if (o != NULL && o != mIRCLink.m_hTreeImages)
						ImageList_Destroy(o);
				}
				else if (tsIndex == "mirc") { // mIRC.exe's icons, these are ADDED to the end of the list, they DON'T replace any existing icons.
					HIMAGELIST himl = NULL, ohiml = TreeView_GetImageList( mIRCLink.m_hTreeView, TVSIL_NORMAL);
					if (ohiml != NULL && ohiml != mIRCLink.m_hTreeImages)
						himl = ohiml;
					else {
						int w, h;
						if (!ImageList_GetIconSize(mIRCLink.m_hTreeImages, &w, &h)) // try to get image size.
							w = h = 16; // default to 16x16

						himl = ImageList_Create(w,h,ILC_COLOR32|ILC_MASK,1,0);
						if (himl != NULL)
							TreeView_SetImageList(mIRCLink.m_hTreeView, himl, TVSIL_NORMAL);
					}
					if (himl != NULL) {
						TString filename((UINT)900), cflag("+");

						mIRCeval("$mircexe", filename.to_chr(), 900);
						filename.trim();

						int fIndex = 0;
						HICON hIcon = NULL;

						do {
							hIcon = dcxLoadIcon(fIndex++,filename, false, cflag);
							if (hIcon != NULL) {
								ImageList_ReplaceIcon(himl, -1, hIcon);
								DestroyIcon(hIcon);
							}
						} while (hIcon != NULL);
					}
					else {
						DCXError("/xtreebar -w", "Unable to Create ImageList");
						return 0;
					}
				}
				else { // our custom image list
					HIMAGELIST himl = NULL, ohiml = TreeView_GetImageList( mIRCLink.m_hTreeView, TVSIL_NORMAL);
					if (ohiml != NULL && ohiml != mIRCLink.m_hTreeImages)
						himl = ohiml;
					else {
						int w, h;
						if (!ImageList_GetIconSize(mIRCLink.m_hTreeImages, &w, &h)) // try to get image size.
							w = h = 16; // default to 16x16

						himl = ImageList_Create(w,h,ILC_COLOR32|ILC_MASK,1,0);
						if (himl != NULL)
							TreeView_SetImageList(mIRCLink.m_hTreeView, himl, TVSIL_NORMAL);
					}
					if (himl != NULL) {
						int iIndex = tsIndex.to_int() -1, fIndex = input.gettok(4).to_int(), iCnt = ImageList_GetImageCount(himl) -1;
						TString cflag(input.gettok(3));
						cflag.trim();
						TString filename(input.gettok(5,-1));
						filename.trim();

						// check index is within range.
						if (iCnt < iIndex) {
							DCXError("/xtreebar -w", "Image Index Out Of Range");
							return 0;
						}
						if (iIndex < 0)
							iIndex = -1; // append to end of list. make sure its only -1

						HICON hIcon = dcxLoadIcon(fIndex,filename, false, cflag);
						if (hIcon != NULL) {
							ImageList_ReplaceIcon(himl, iIndex, hIcon);
							DestroyIcon(hIcon);
						}
						else {
							DCXError("/xtreebar -w", "Unable to load icon");
							return 0;
						}
					}
					else {
						DCXError("/xtreebar -w", "Unable to Create ImageList");
						return 0;
					}
				}
			}
			break;
		case 'T': // [1|0]
			{ // Take over Treebar drawing
				DcxDock::g_bTakeOverTreebar = (input.gettok( 2 ).to_int() ? true : false);
				if (DcxDock::g_bTakeOverTreebar) {
					TString buf((UINT)32);
					mIRCeval("$isalias(xtreebar_callback)", buf.to_chr(), 32);
					if (buf == "$true")
						TraverseTreebarItems();
					else {
						DcxDock::g_bTakeOverTreebar = false;
						DCXError("/xtreebar", "No $xtreebar_callback() alias found");
						return 0;
					}
				}
			}
			break;
		default:
			DCXError("/xtreebar", "Invalid Flag");
			return 0;
	}
	RedrawWindow(mIRCLink.m_hTreeView, NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE );
	return 1;
}

// prop
mIRC(_xtreebar)
{
	TString d(data);
	d.trim();

	data[0] = 0;

	static const TString poslist("item icons");
	int nType = poslist.findtok(d.gettok( 2 ).to_chr(),1);
	int cnt = TreeView_GetCount(mIRCLink.m_hTreeView);
	int index = d.gettok( 3 ).to_int();

	if (index > cnt) {
		lstrcpy(data, "D_ERROR Invalid Item Index");
		return 3;
	}

	TVITEMEX item;
	ZeroMemory(&item,sizeof(item));

	switch (nType)
	{
	case 1: // item
		{
			if (index < 1) // if index < 1 return total items.
				wsprintf(data, "%d", cnt);
			else {
				char szbuf[900];
				item.hItem = TreeView_MapAccIDToHTREEITEM(mIRCLink.m_hTreeView, index);
				item.mask = TVIF_TEXT;
				item.pszText = szbuf;
				item.cchTextMax = 900;
				if (TreeView_GetItem(mIRCLink.m_hTreeView,&item))
					lstrcpyn(data, item.pszText, 900);
				else
					lstrcpy(data, "D_ERROR Unable To Get Item");
			}
		}
		break;
	case 2: // icons
		{
			if (index < 1) // if index < 1 make it the last item.
				index = cnt;

			item.hItem = TreeView_MapAccIDToHTREEITEM(mIRCLink.m_hTreeView, index);
			item.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE;
			if (TreeView_GetItem(mIRCLink.m_hTreeView,&item))
				wsprintf(data, "%d %d", item.iImage, item.iSelectedImage);
			else
				lstrcpy(data, "D_ERROR Unable To Get Item");
		}
		break;
	case 0: // error
	default:
		{
			TString error;
			error.sprintf("D_ERROR Invalid prop ().%s", d.gettok( 2 ).to_chr());
			lstrcpyn(data, error.to_chr(), 900);
		}
		break;
	}
	return 3;
}
