
/*
*/

#include "dcxDock.h"

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
#ifndef NDEBUG
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
		case 'i': // [clear|default] | [flags] [index] [filename]
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
					HIMAGELIST ohiml = TreeView_GetImageList( mIRCLink.m_hTreeView, TVSIL_NORMAL);
					HIMAGELIST himl = ImageList_Duplicate( ohiml );
					if (himl != NULL) {
						int type = 0, index = input.gettok(3).to_int();
						TString cflag(input.gettok(2));
						cflag.trim();
						TString filename(input.gettok(4,-1));
						filename.trim();
						// add replacement images.
						switch (cflag[1])
						{
						case 's': // status windows
							type = 0;
							break;
						case 'c': // channel windows
							type = 1;
							break;
						case 'C': // Custom windows
							type = 2;
							break;
						case 'S': // DCC Send windows
							type = 3;
							break;
						case 'G': // DCC Get windows
							type = 4;
							break;
						default:
							{
								DCXError("/xtreebar -i", "Invalid Image Flag");
								ImageList_Destroy(himl);
								return 0;
							}
							break;
						}
						HICON hIcon = dcxLoadIcon(index,filename, false, cflag);
						if (hIcon != NULL) {
							mIRCDebug("count: %d", ImageList_GetImageCount(himl));
							mIRCDebug("replace: %d", ImageList_ReplaceIcon(himl,type,hIcon));
							HIMAGELIST o = TreeView_SetImageList(mIRCLink.m_hTreeView, himl, TVSIL_NORMAL);
							if (o != NULL && o != mIRCLink.m_hTreeImages)
								ImageList_Destroy(o);
							DestroyIcon(hIcon);
							RedrawWindow( mIRCLink.m_hTreeView, NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE );
							{
								TVITEMEX item;
								ZeroMemory(&item,sizeof(item));
								item.hItem = TreeView_GetFirstVisible(mIRCLink.m_hTreeView);
								//item.hItem = TreeView_GetNextVisible(mIRCLink.m_hTreeView, item.hItem);
								item.mask = TVIF_IMAGE; //I_IMAGECALLBACK == -1
								mIRCDebug("getitem: %d", TreeView_GetItem(mIRCLink.m_hTreeView,&item));
								mIRCDebug("image: %d selected: %d state: %d", item.iImage, item.iSelectedImage, item.state);
								item.hItem = TreeView_GetNextVisible(mIRCLink.m_hTreeView, item.hItem);
								item.mask = TVIF_IMAGE;
								mIRCDebug("getitem2: %d", TreeView_GetItem(mIRCLink.m_hTreeView,&item));
								mIRCDebug("image: %d selected: %d state: %d", item.iImage, item.iSelectedImage, item.state);
								//item.mask = TVIF_IMAGE;
								//TreeView_SetItem(mIRCLink.m_hTreeView, &item);
							}
						}
						else {
							ImageList_Destroy(himl);
							DCXError("/xtreebar -i", "Unable to load icon");
							return 0;
						}
					}
				}
			}
			break;
#endif
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
