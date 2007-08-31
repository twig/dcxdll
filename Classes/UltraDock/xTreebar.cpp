
/*
*/

#include "dcxDock.h"

void TraverseSiblings(const HTREEITEM hParent, TString &buf, TString &res, LPTVITEMEX pitem)
{
	ZeroMemory(pitem, sizeof(TVITEMEX));
	for (HTREEITEM ptvitem = TreeView_GetChild(mIRCLink.m_hTreeView, hParent); ptvitem != NULL; ptvitem = TreeView_GetNextSibling(mIRCLink.m_hTreeView, ptvitem)) {
		pitem->hItem = ptvitem;
		pitem->pszText = buf.to_chr();
		pitem->cchTextMax = 255;
		pitem->mask = TVIF_TEXT;
		if (TreeView_GetItem(mIRCLink.m_hTreeView, pitem)) {
			int i = 0;
			mIRCevalEX(res.to_chr(), 16, "$xtreebar_callback(init,%s)", pitem->pszText);
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
		TraverseSiblings(ptvitem, buf, res, pitem);
	}
}

void TraverseTreebarItems(void)
{
	SetWindowRedraw(mIRCLink.m_hTreeView, FALSE);
	TString buf((UINT)255);
	TString res((UINT)16);
	HTREEITEM hRoot = TreeView_GetRoot(mIRCLink.m_hTreeView);
	TVITEMEX item;
	ZeroMemory(&item, sizeof(item));
	item.hItem = hRoot;
	item.pszText = buf.to_chr();
	item.cchTextMax = 255;
	item.mask = TVIF_TEXT;
	if (TreeView_GetItem(mIRCLink.m_hTreeView, &item)) {
		int i = 0;
		mIRCevalEX(res.to_chr(), 16, "$xtreebar_callback(init,%s)", item.pszText);
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
	TraverseSiblings(hRoot, buf, res, &item);
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
				static const TString treebar_styles("trackselect notrackselect tooltips notooltips infotip noinfotip hasbuttons nohasbuttons rootlines norootlines singleexpand nosingleexpand scroll noscroll showsel noshowsel transparent notransparent fadebuttons nofadebuttons indent noident buffer nobuffer autohscroll noautohscroll richtooltip norichtooltip");
				int i = 2;
				DWORD stylef = GetWindowStyle(mIRCLink.m_hTreeView);
				DWORD exstylef = GetWindowExStyle(mIRCLink.m_hTreeView);
#ifdef DCX_USE_WINSDK
				DWORD tvexstylef = TreeView_GetExtendedStyle(mIRCLink.m_hTreeView);
				DWORD tvexstylemask = 0;
#endif

				while (i <= numtok) {
					switch (treebar_styles.findtok(input.gettok(i).to_chr(),1))
					{
					case 1: // trackselect (off by default)
						stylef |= TVS_TRACKSELECT;
						break;
					case 2: // notrackselect
						stylef &= ~TVS_TRACKSELECT;
						break;
					case 3: // tooltips (on by default)
						stylef &= ~TVS_NOTOOLTIPS;
						break;
					case 4: // notooltips
						stylef |= TVS_NOTOOLTIPS;
						break;
					case 5: // infotips (on by default)
						stylef |= TVS_INFOTIP;
						break;
					case 6: // noinfotips
						stylef &= ~TVS_INFOTIP;
						break;
					case 7: // hasbuttons (on by default)
						stylef |= TVS_HASBUTTONS;
						break;
					case 8: // nohasbuttons
						stylef &= ~TVS_HASBUTTONS;
						break;
					case 9: // rootlines (on by default)
						stylef |= TVS_LINESATROOT;
						break;
					case 10: // norootlines
						stylef &= ~TVS_LINESATROOT;
						break;
					case 11: // singleexpand (off by default)
						stylef |= TVS_SINGLEEXPAND;
						break;
					case 12: // nosingleexpand
						stylef &= ~TVS_SINGLEEXPAND;
						break;
					case 13: // scroll (off by default)
						stylef &= ~TVS_NOSCROLL;
						break;
					case 14: // noscroll (NB: this can lead to gfx glitches with scroll bars already shown)
						stylef |= TVS_NOSCROLL;
						break;
					case 15: // showsel (on by default)
						stylef |= TVS_SHOWSELALWAYS;
						break;
					case 16: // noshowsel
						stylef &= ~TVS_SHOWSELALWAYS;
						break;
					case 17: // transparent
						exstylef |= WS_EX_TRANSPARENT;
						break;
					case 18: // notransparent
						exstylef &= ~WS_EX_TRANSPARENT;
						break;
#ifdef DCX_USE_WINSDK
					case 19: // fadebuttons
						tvexstylef |= TVS_EX_FADEINOUTEXPANDOS;
						tvexstylemask |= TVS_EX_FADEINOUTEXPANDOS;
						break;
					case 20: // nofadebuttons
						tvexstylef &= ~TVS_EX_FADEINOUTEXPANDOS;
						tvexstylemask |= TVS_EX_FADEINOUTEXPANDOS;
						break;
					case 21: // indent
						tvexstylef &= ~TVS_EX_NOINDENTSTATE;
						tvexstylemask |= TVS_EX_NOINDENTSTATE;
						break;
					case 22: // noident
						tvexstylef |= TVS_EX_NOINDENTSTATE;
						tvexstylemask |= TVS_EX_NOINDENTSTATE;
						break;
					case 23: // buffer
						tvexstylef |= TVS_EX_DOUBLEBUFFER;
						tvexstylemask |= TVS_EX_DOUBLEBUFFER;
						break;
					case 24: // nobuffer
						tvexstylef &= ~TVS_EX_DOUBLEBUFFER;
						tvexstylemask |= TVS_EX_DOUBLEBUFFER;
						break;
					case 25: // autohscroll
						tvexstylef |= TVS_EX_AUTOHSCROLL;
						tvexstylemask |= TVS_EX_AUTOHSCROLL;
						break;
					case 26: // noautohscroll
						tvexstylef &= ~TVS_EX_AUTOHSCROLL;
						tvexstylemask |= TVS_EX_AUTOHSCROLL;
						break;
					case 27: // richtooltip
						tvexstylef |= TVS_EX_RICHTOOLTIP;
						tvexstylemask |= TVS_EX_RICHTOOLTIP;
						break;
					case 28: // norichtooltip
						tvexstylef &= ~TVS_EX_RICHTOOLTIP;
						tvexstylemask |= TVS_EX_RICHTOOLTIP;
						break;
#endif
					default: // unknown style ignore.
						DCXError("/xtreebar -s", "Unknown Style");
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
				RedrawWindow(mIRCLink.m_hTreeView, NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE );
			}
			break;
			// -c & -i commands are experimental & required stopping mIRC doing the item drawing.
		case 'c': // [COLOUR FLAGS] [COLOUR]
			{
				if (numtok < 3) {
					DCXError("/xtreebar -c", "Invalid Colour Args");
					return 0;
				}
				// TODO: undocumented. Non functional, mirc does owner draw, need to subclass
				TString cflag(input.gettok(2));
				COLORREF clr = (COLORREF)input.gettok(3).to_num();

				switch(cflag[1])
				{
				case 't': // text colour
					{
						TreeView_SetTextColor(mIRCLink.m_hTreeView,clr);
					}
					break;
				case 'b': // bkg colour
					{
						TreeView_SetBkColor(mIRCLink.m_hTreeView,clr);
					}
					break;
				case 'l': // line colour
					{
						TreeView_SetLineColor(mIRCLink.m_hTreeView,clr);
					}
					break;
				case 'm': // insert mark colour
					{
						TreeView_SetInsertMarkColor(mIRCLink.m_hTreeView,clr);
					}
					break;
				default:
					{
						DCXError("/xtreebar -c","Invalid Colour flag");
						return 0;
					}
					break;
				}
			}
			break;
		case 'i': // [clear|default] | [index] [+flags] [icon index] [filename]
			{
				if (mIRCLink.m_hTreeImages == NULL) {
					DCXError("/xtreebar -i", "No Valid TreeView Image List");
					return 0;
				}
				if (input.gettok(2) == "clear") {
					HIMAGELIST o = TreeView_SetImageList(mIRCLink.m_hTreeView,NULL,TVSIL_NORMAL);
					if (o != NULL && o != mIRCLink.m_hTreeImages)
						ImageList_Destroy(o);
				}
				else if (input.gettok(2) == "default") {
					HIMAGELIST o = TreeView_SetImageList(mIRCLink.m_hTreeView,mIRCLink.m_hTreeImages,TVSIL_NORMAL);
					if (o != NULL && o != mIRCLink.m_hTreeImages)
						ImageList_Destroy(o);
				}
				else {
					HIMAGELIST himl = NULL, ohiml = TreeView_GetImageList( mIRCLink.m_hTreeView, TVSIL_NORMAL);
					if (ohiml != mIRCLink.m_hTreeImages)
						himl = ohiml;
					else {
						/*
						Duplicate existing list, but remove all images.
						*/
						//himl = ImageList_Duplicate( ohiml );
						himl = ImageList_Create(16,16,ILC_COLOR32|ILC_MASK,1,0);
						if (himl != NULL) {
							//ImageList_RemoveAll(himl);
							TreeView_SetImageList(mIRCLink.m_hTreeView, himl, TVSIL_NORMAL);
						}
					}
					if (himl != NULL) {
						int iIndex = input.gettok(2).to_int() -1, fIndex = input.gettok(4).to_int(), iCnt = ImageList_GetImageCount(himl) -1;
						TString cflag(input.gettok(3));
						cflag.trim();
						TString filename(input.gettok(5,-1));
						filename.trim();

						// check index is within range.
						if (iCnt < iIndex) {
							DCXError("/xtreebar -i", "Image Index Out Of Range");
							return 0;
						}
						if (iIndex < 0)
							iIndex = -1; // append to end of list. make sure its only -1

						HICON hIcon = dcxLoadIcon(fIndex,filename, false, cflag);
						if (hIcon != NULL) {
							ImageList_ReplaceIcon(himl, iIndex, hIcon);
							DestroyIcon(hIcon);
							RedrawWindow( mIRCLink.m_hTreeView, NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE );
						}
						else {
							DCXError("/xtreebar -i", "Unable to load icon");
							return 0;
						}
					}
				}
			}
			break;
		case 'T': // [1|0]
			{ // Take over Treebar drawing
				DcxDock::g_bTakeOverTreebar = (input.gettok( 2 ).to_int() ? true : false);
				if (DcxDock::g_bTakeOverTreebar)
					TraverseTreebarItems();
				RedrawWindow( mIRCLink.m_hTreeView, NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE );
			}
			break;
		default:
			DCXError("/xtreebar", "Invalid Flag");
			return 0;
	}
	return 1;
}

// prop
mIRC(_xtreebar)
{
	TString d(data);
	d.trim();

	data[0] = 0;

	static const TString poslist("item");
	int nType = poslist.findtok(d.gettok( 2 ).to_chr(),1);
	switch (nType)
	{
	case 1: // item
		{
			int index = d.gettok( 3 ).to_int();

			if (index < 1)
				wsprintf(data, "%d", TreeView_GetCount(mIRCLink.m_hTreeView));
			else {
				TVITEMEX item;
				ZeroMemory(&item,sizeof(item));
				char szbuf[900];
				item.hItem = TreeView_MapAccIDToHTREEITEM(mIRCLink.m_hTreeView, index);
				item.mask = TVIF_TEXT;
				item.pszText = szbuf;
				item.cchTextMax = 900;
				if (TreeView_GetItem(mIRCLink.m_hTreeView,&item))
					lstrcpyn(data, item.pszText, 900);
				else
					lstrcpy(data, "");
			}
		}
		break;
	case 0: // error
	default:
		{
			TString error;
			error.sprintf("D_ERROR Invalid prop ().%s", d.gettok( 2 ).to_chr());
			lstrcpy(data, error.to_chr());
		}
		break;
	}
	return 3;
}
