
/*
*/
#include "defines.h"
#include "Classes/UltraDock/dcxDock.h"
#include "Dcx.h"



// [SWITCH] [OPTIONS]
mIRC(xstatusbar) {
	TString input(data);
	input.trim();
	data[0] = 0;

	const UINT numtok = input.numtok( );

	if (numtok < 1) {
		Dcx::error(TEXT("/xstatusbar"),TEXT("Invalid Parameters"));
		return 0;
	}
	const TString switches(input.getfirsttok(1));	// tok 1

	switch (switches[1]) {
		case TEXT('A'): // -A [0|1] (options)=notheme grip tooltips nodivider utf8
			{
				// check syntax
				if (numtok < 2) {
					Dcx::error(TEXT("/xstatusbar -A"),TEXT("Invalid Parameters"));
					return 0;
				}
				// Enable/Disable the Statusbar.
				// -A [0|1] [options] = notheme grip tooltips nodivider utf8
				if (input.getnexttok( ).to_int() > 0)					// tok 2
				{
					if (!DcxDock::InitStatusbar(input.getlasttoks()))	// tok 3, -1
					{
						Dcx::error(TEXT("/xstatusbar -A"),TEXT("Unable to Create Statusbar"));
						return 0;
					}
				}
				else
					DcxDock::UnInitStatusbar();

				UpdatemIRC();
			}
			break;
		case TEXT('f'): // -f [+FLAGS] [CHARSET] [SIZE] [FONTNAME] : set font
			{
				LOGFONT lf;

				// check syntax
				if (numtok < 5) {
					Dcx::error(TEXT("/xstatusbar -f"),TEXT("Invalid Parameters"));
					return 0;
				}

				if (ParseCommandToLogfont(input.getlasttoks(), &lf))	// tok 2, -1
					DcxDock::status_setFont(CreateFontIndirect(&lf));
			}
			break;
		case TEXT('k'): // -k [clr] : background colour.
			{
				// check syntax
				if (numtok != 2) {
					Dcx::error(TEXT("/xstatusbar -k"),TEXT("Invalid Parameters"));
					return 0;
				}

				const int col = input.getnexttok( ).to_int();	// tok 2

				if (col < 0)
					DcxDock::status_setBkColor((COLORREF) CLR_DEFAULT);
				else
					DcxDock::status_setBkColor((COLORREF) col);
			}
			break;
		case TEXT('l'): // -l [POS [POS POS ...]] : parts
			{
				// check syntax
				if (numtok < 2) {
					Dcx::error(TEXT("/xstatusbar -l"),TEXT("Invalid Parameters"));
					return 0;
				}

				const UINT nParts = numtok - 1;
				INT parts[256];

				int c = 0;
				TString p;
				for (UINT i = 0; i < nParts; i++ )
				{
					if (c >= 100) {
						Dcx::error(TEXT("/xstatusbar -l"),TEXT("Can't Allocate Over 100% of Statusbar!"));
						return 0;
					}

					p = input.getnexttok( );	// tok i+2

					const int t = p.to_int();

					if (p.right( 1 ) == TEXT('%')) {
						DcxDock::g_iDynamicParts[i] = t;
						c += t;
					}
					else
						DcxDock::g_iFixedParts[i] = t;

					parts[i] = t;
				}
				DcxDock::status_setParts( nParts, parts );
				DcxDock::status_updateParts();
			}
			break;
		case TEXT('t'): // -t N [+FLAGS] [#ICON] [BKGCOLOR] [TXTCOLOR] (Cell Text)([TAB]Tooltip Text) : set part
			{
				// check syntax (text can be blank)
				if (numtok < 6) {
					Dcx::error(TEXT("/xstatusbar -t"),TEXT("Invalid Parameters"));
					return 0;
				}
				const TString tsTabOne(input.getfirsttok(1, TSTAB));	// tok 1, TSTAB
				const TString tooltip(input.getnexttok(TSTAB).trim());	// tok 2, TSTAB;
				TString itemtext;

				const int nPos = (tsTabOne.getfirsttok(2).to_int() - 1);			// tok 2
				const TString flags(tsTabOne.getnexttok());							// tok 3
				const int icon = (tsTabOne.getnexttok().to_int() - 1);				// tok 4
				const COLORREF bkgClr = (COLORREF)tsTabOne.getnexttok().to_int();	// tok 5
				const COLORREF txtClr = (COLORREF)tsTabOne.getnexttok().to_int();	// tok 6
				const UINT iFlags = DcxDock::status_parseItemFlags( flags );

				if ( tsTabOne.numtok( ) > 6 )
					itemtext = tsTabOne.getlasttoks().trim();						// tok 7, -1

				DcxDock::status_deletePartInfo(nPos); // delete custom info if any.
				DestroyIcon( (HICON) DcxDock::status_getIcon( nPos ) );
				DcxDock::status_setIcon( nPos, NULL );

				if (iFlags & SBT_OWNERDRAW) {
					LPSB_PARTINFO pPart;

					try {
						pPart = new SB_PARTINFO;
					}
					catch (std::bad_alloc)
					{
						Dcx::error(TEXT("/xstatusbar -t"),TEXT("Unable to Allocate Memory"));
						return 0;
					}

					pPart->m_Child = NULL;
					pPart->m_iIcon = icon;
					pPart->m_Text = itemtext;
					pPart->m_TxtCol = CLR_INVALID;
					pPart->m_BkgCol = NULL;

					if (bkgClr != CLR_INVALID)
						pPart->m_BkgCol = CreateSolidBrush(bkgClr);
					if (txtClr != CLR_INVALID)
						pPart->m_TxtCol = txtClr;

					DcxDock::status_setTipText( nPos, tooltip.to_chr() );
					DcxDock::status_setPartInfo( nPos, iFlags, pPart );
				}
				else {
					if ( icon > -1 )
						DcxDock::status_setIcon( nPos, ImageList_GetIcon( DcxDock::status_getImageList( ), icon, ILD_TRANSPARENT ) );
					DcxDock::status_setText( nPos, iFlags, itemtext.to_chr() );
					DcxDock::status_setTipText(nPos, tooltip.to_chr());
				}
			}
			break;
		case TEXT('v'): // -v [N] [BKGCOLOR] [TXTCOLOR] (TEXT) : set parts text
			{
				// check syntax (text can be blank)
				if (numtok < 4) {
					Dcx::error(TEXT("/xstatusbar -v"),TEXT("Invalid Parameters"));
					return 0;
				}

				const int nPos = (input.getnexttok( ).to_int( ) - 1);	// tok 2

				if ( nPos > -1 && nPos < DcxDock::status_getParts( 256, 0 ) ) {

					TString itemtext;

					const COLORREF bkgClr = (COLORREF)input.getnexttok( ).to_int();	// tok 3
					const COLORREF txtClr = (COLORREF)input.getnexttok( ).to_int();	// tok 4

					if ( numtok > 4 )
						itemtext = input.getlasttoks();	// tok 5, -1

					const UINT iFlags = DcxDock::status_getPartFlags( nPos );

					if (iFlags & SBT_OWNERDRAW) {
						LPSB_PARTINFO pPart = (LPSB_PARTINFO)DcxDock::status_getText(nPos, NULL);
						if (pPart != NULL) {
							if (pPart->m_BkgCol != NULL)
								DeleteBrush(pPart->m_BkgCol);

							pPart->m_Text = itemtext;
							if (bkgClr != CLR_INVALID)
								pPart->m_BkgCol = CreateSolidBrush(bkgClr);
							pPart->m_TxtCol = txtClr;

							DcxDock::status_setPartInfo( nPos, iFlags, pPart );
						}
						else {
							Dcx::error(TEXT("/xstatusbar -v"),TEXT("Unable to set item text"));
							return 0;
						}
					}
					else {
						try {
#ifdef DCX_USE_C11
							std::unique_ptr<WCHAR> text(new WCHAR[DcxDock::status_getTextLength(nPos) + 1]);
							DcxDock::status_setText(nPos, HIWORD(DcxDock::status_getText(nPos, text.get())), itemtext.to_chr());
#else
							WCHAR *text = new WCHAR[DcxDock::status_getTextLength(nPos) + 1];
							DcxDock::status_setText(nPos, HIWORD(DcxDock::status_getText(nPos, text)), itemtext.to_chr());
							delete [] text;
#endif
						}
						catch (std::bad_alloc) {
							Dcx::error(TEXT("/xstatusbar -v"),TEXT("Unable to Allocate Memory"));
							return 0;
						}
					}
				}
			}
			break;
		case TEXT('w'): // -w [FLAGS] [INDEX] [FILENAME] : load an icon
			{
				// check syntax
				if (numtok < 4) {
					Dcx::error(TEXT("/xstatusbar -w"),TEXT("Invalid Parameters"));
					return 0;
				}

				HIMAGELIST himl = DcxDock::status_getImageList();
				const TString flags(input.getnexttok( ));			// tok 2
				const int index = input.getnexttok( ).to_int();		// tok 3
				TString filename(input.getlasttoks());				// tok 4, -1

				if (himl == NULL) {
					himl = DcxDock::status_createImageList();

					if (himl != NULL)
						DcxDock::status_setImageList(himl);
				}

				if (himl != NULL) {
					HICON icon = dcxLoadIcon(index, filename, false, flags);

					if (icon != NULL) {
						if (ImageList_AddIcon(himl, icon) == -1)
							Dcx::error(TEXT("/xstatusbar -w"),TEXT("Unable To Add Image to ImageList"));
					}
					else
						Dcx::error(TEXT("/xstatusbar -w"),TEXT("Unable To Load Icon"));
					DestroyIcon(icon);
				}
				else
					Dcx::error(TEXT("/xstatusbar -w"),TEXT("Unable To Create ImageList"));
			}
			break;
		case TEXT('y'): // destroy image list.
			{
				ImageList_Destroy( DcxDock::status_getImageList( ) );
				DcxDock::status_setImageList(NULL);
			}
			break;
		default:
			{
				Dcx::error(TEXT("/xstatusbar"),TEXT("Invalid Switch"));
				return 0;
			}
	}
	return 1;
}

// prop
mIRC(_xstatusbar)
{
	TString d(data);
	d.trim();

	data[0] = 0;

	static const TString poslist(TEXT("visible text parts tooltip"));
	const int nType = poslist.findtok(d.getfirsttok( 2 ).to_chr(),1);
	switch (nType)
	{
	case 1: // visible
		{
			//if (DcxDock::IsStatusbar())
			//	lstrcpyn(data,TEXT("$true"), MIRC_BUFFER_SIZE_CCH);
			//else
			//	lstrcpyn(data,TEXT("$false"), MIRC_BUFFER_SIZE_CCH);
			dcx_Con(DcxDock::IsStatusbar(), data);
		}
		break;
	case 2: // text
		{
			const int iPart = (d.getnexttok( ).to_int( ) -1), nParts = (int)DcxDock::status_getParts( 256, 0 );

			if ( iPart > -1 && iPart < nParts ) {
				const UINT iFlags = DcxDock::status_getPartFlags( iPart );

				if (iFlags & SBT_OWNERDRAW) {
					LPSB_PARTINFO pPart = (LPSB_PARTINFO)DcxDock::status_getText(iPart, NULL);
					if (pPart != NULL)
						dcx_strcpyn(data, pPart->m_Text.to_chr(), MIRC_BUFFER_SIZE_CCH);
				}
				else {
					const UINT len = DcxDock::status_getTextLength( iPart );
					try {
#ifdef DCX_USE_C11
						std::unique_ptr<WCHAR> text(new WCHAR[len + 1]);
						DcxDock::status_getText(iPart, text.get());
						dcx_strcpyn(data, text.get(), MIRC_BUFFER_SIZE_CCH);
#else
						WCHAR *text = new WCHAR[len + 1];
						DcxDock::status_getText(iPart, text);
						dcx_strcpyn(data, text, MIRC_BUFFER_SIZE_CCH);
						delete[] text;
#endif
					}
					catch (std::bad_alloc) {
						Dcx::error(TEXT("/xstatusbar -v"), TEXT("Unable to Allocate Memory"));
					}
				}
			}
		}
		break;
	case 3: // parts
		{
			INT parts[256] = {0};
			const int nParts = (int)DcxDock::status_getParts( 256, 0 );

			DcxDock::status_getParts( 256, parts );

			//TCHAR dd[10];
			//
			//for (int i = 0; i < nParts; i++ )
			//{
			//	wnsprintf( dd, 10, TEXT("%d"), parts[i] );
			//
			//	if ( i != 0 )
			//		lstrcat( data, TEXT(" ") );
			//	lstrcat( data, dd );
			//}

			TString tsOut((UINT)MIRC_BUFFER_SIZE_CCH);

			for (int i = 0; i < nParts; i++ )
				tsOut.addtok(parts[i]);

			dcx_strcpyn(data, tsOut.to_chr(), MIRC_BUFFER_SIZE_CCH);
		}
		break;
	case 4: // tooltip
		{
			const int iPart = d.getnexttok( ).to_int( ), nParts = (int)DcxDock::status_getParts( 256, 0 );

			if ( iPart > -1 && iPart < nParts )
				DcxDock::status_getTipText( iPart, MIRC_BUFFER_SIZE_CCH, data );
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
