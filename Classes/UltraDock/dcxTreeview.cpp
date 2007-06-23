
/*
*/

#include "dcxDock.h"

// [SWITCH] [OPTIONS]
mIRC(xtreeview) {
	TString input(data);
	input.trim();
	data[0] = 0;

	int numtok = input.numtok( );

	if (!IsWindow(mIRCLink.m_hTreeView)) {
		DCXError("/xtreeview", "No Treebar");
		return 0;
	}

	TString switches(input.gettok(1));

	switch (switches[1]) {
		case 'f': // [+FONTFLAGS] [CHARSET] [SIZE] [FONTNAME]
			{
				if (numtok < 5) {
					DCXError("/xtreeview -f", "Invalid Font Args");
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
					DCXError("/xtreeview -s","Invalid Style Args");
					return 0;
				}
				static const TString treebar_styles("trackselect notrackselect tooltips notooltips infotip noinfotip hasbuttons nohasbuttons rootlines norootlines singleexpand nosingleexpand scroll noscroll showsel noshowsel");
				int i = 2, numtok = input.numtok();
				DWORD stylef = GetWindowStyle(mIRCLink.m_hTreeView);
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
					default: // unknown style ignore.
						break;
					}
					i++;
				}
				SetWindowLong(mIRCLink.m_hTreeView,GWL_STYLE, stylef);
				RedrawWindow(mIRCLink.m_hTreeView, NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE );
			}
			break;
#ifndef NDEBUG
			// -c & -i commands are experimental & required stopping mIRC doing the item drawing.
		case 'c': // [COLOUR FLAGS] [COLOUR]
			{
				if (numtok < 3) {
					DCXError("/xtreeview -c", "Invalid Colour Args");
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
						DCXError("/xtreeview -c","Invalid Colour flag");
						return 0;
					}
					break;
				}
			}
			break;
		case 'i': // [clear|default] | [flags] [index] [filename]
			{
				if (mIRCLink.m_hTreeImages == NULL) {
					DCXError("/xtreeview -i", "No Valid TreeView Image List");
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
								DCXError("/xtreeview -i", "Invalid Image Flag");
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
							DCXError("/xtreeview -i", "Unable to load icon");
							return 0;
						}
					}
				}
			}
			break;
#endif
		default:
			DCXError("/xtreeview", "Invalid Flag");
			return 0;
	}
	return 1;
}

// prop
mIRC(_xtreeview)
{
	TString d(data);
	d.trim();

	data[0] = 0;
	return 3;
}
