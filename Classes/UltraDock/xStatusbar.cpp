
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

	int numtok = input.numtok( );

	if (numtok < 1) {
		Dcx::error(TEXT("/xstatusbar"),TEXT("Invalid Parameters"));
		return 0;
	}
#ifndef UNICODE
	if (Dcx::mIRC.getMainVersion() == 7) {
		DCX_DEBUG(Dcx::debug,TEXT("xstatusbar"), TEXT("mIRC V7 detected..."));
		DCX_DEBUG(Dcx::debug,TEXT("xstatusbar"), TEXT("Can't do any window mods etc.."));
		Dcx::error(TEXT("/xstatusbar"),TEXT("Can't be used in mIRC V7"));
		return 0;
	}
#endif
	TString switches(input.gettok(1));

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
				if (input.gettok(2).to_int() > 0) {
					if (!DcxDock::InitStatusbar(input.gettok(3,-1))) {
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

				if (ParseCommandToLogfont(input.gettok(2, -1), &lf)) {
					DcxDock::status_setFont(CreateFontIndirect(&lf));
				}
			}
			break;
		case TEXT('k'): // -k [clr] : background colour.
			{
				// check syntax
				if (numtok != 2) {
					Dcx::error(TEXT("/xstatusbar -k"),TEXT("Invalid Parameters"));
					return 0;
				}

				int col = input.gettok( 2 ).to_int();

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

				int nParts = numtok - 1;
				INT parts[256];

				int i = 0, c = 0, t = 0;
				TString p;
				while ( i < nParts ) {

					if (c >= 100) {
						Dcx::error(TEXT("/xstatusbar -l"),TEXT("Can't Allocate Over 100% of Statusbar!"));
						return 0;
					}

					p = input.gettok( i+2 );

					t = p.to_int();

					if (p.right( 1 ) == TEXT("%")) {
						DcxDock::g_iDynamicParts[i] = t;
						c += t;
					}
					else
						DcxDock::g_iFixedParts[i] = t;

					parts[i] = t;
					i++;
				}
				DcxDock::status_setParts( nParts, parts );
				DcxDock::status_updateParts();
			}
			break;
		case TEXT('t'): // -t N [+FLAGS] [#ICON] (Cell Text)([TAB]Tooltip Text) : set part
			{
				// check syntax (text can be blank)
				if (numtok < 4) {
					Dcx::error(TEXT("/xstatusbar -t"),TEXT("Invalid Parameters"));
					return 0;
				}

				int nPos = input.gettok( 2 ).to_int( ) - 1;
				TString flags(input.gettok( 3 ));
				int icon = input.gettok( 4 ).to_int( ) - 1;
				UINT iFlags = DcxDock::status_parseItemFlags( flags );

				TString itemtext;

				if ( input.gettok( 1, TSTAB ).numtok( ) > 4 )
					itemtext = input.gettok( 1, TSTAB ).gettok( 5, -1).trim();

				TString tooltip;

				if ( input.numtok( TSTAB ) > 1 )
					tooltip = input.gettok( 2, TSTAB ).trim();

				DcxDock::status_deletePartInfo(nPos); // delete custom info if any.
				DestroyIcon( (HICON) DcxDock::status_getIcon( nPos ) );
				DcxDock::status_setIcon( nPos, NULL );

				if (iFlags & SBT_OWNERDRAW) {
					LPSB_PARTINFO pPart = new SB_PARTINFO;
					//ZeroMemory(pPart,sizeof(SB_PARTINFO));
					pPart->m_Child = NULL;
					pPart->m_iIcon = icon;
					if (flags.find(TEXT('f'),0)) { // mIRC formatted text
						pPart->m_Text = itemtext;
#if UNICODE
						DcxDock::status_setTipText( nPos, tooltip.to_chr() );
#else
						DcxDock::status_setTipText( nPos, tooltip.to_wchr(DcxDock::g_bUseUTF8 ) );
#endif
						DcxDock::status_setPartInfo( nPos, iFlags, pPart );
					}
					else { // child control
						Dcx::error(TEXT("/xstatusbar -t"),TEXT("Child Controls Are not supported at this time."));
					}
				}
				else {
					if ( icon > -1 )
						DcxDock::status_setIcon( nPos, ImageList_GetIcon( DcxDock::status_getImageList( ), icon, ILD_TRANSPARENT ) );
#if UNICODE
					DcxDock::status_setText( nPos, iFlags, itemtext.to_chr() );
					DcxDock::status_setTipText( nPos, tooltip.to_chr() );
#else
					DcxDock::status_setText( nPos, iFlags, itemtext.to_wchr(DcxDock::g_bUseUTF8) );
					DcxDock::status_setTipText( nPos, tooltip.to_wchr(DcxDock::g_bUseUTF8) );
#endif
				}
			}
			break;
		case TEXT('v'): // -v [N] (TEXT) : set parts text
			{
				// check syntax (text can be blank)
				if (numtok < 2) {
					Dcx::error(TEXT("/xstatusbar -v"),TEXT("Invalid Parameters"));
					return 0;
				}

				int nPos = input.gettok( 2 ).to_int( ) - 1;

				if ( nPos > -1 && nPos < DcxDock::status_getParts( 256, 0 ) ) {

					TString itemtext;
					if ( numtok > 2 )
						itemtext = input.gettok( 3, -1 );

					UINT iFlags = DcxDock::status_getPartFlags( nPos );

					if (iFlags & SBT_OWNERDRAW) {
						LPSB_PARTINFO pPart = (LPSB_PARTINFO)DcxDock::status_getText(nPos, NULL);
						pPart->m_Text = itemtext;
						DcxDock::status_setPartInfo( nPos, iFlags, pPart );
					}
					else {
						WCHAR *text = new WCHAR[DcxDock::status_getTextLength(nPos) + 1];
#if UNICODE
						DcxDock::status_setText( nPos, HIWORD( DcxDock::status_getText( nPos, text ) ), itemtext.to_chr() );
#else
						DcxDock::status_setText( nPos, HIWORD( DcxDock::status_getText( nPos, text ) ), itemtext.to_wchr(DcxDock::g_bUseUTF8) );
#endif
						delete [] text;
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

				HIMAGELIST himl;
				HICON icon;
				TString flags(input.gettok( 2 ));
				int index = input.gettok( 3 ).to_int();
				TString filename(input.gettok( 4, -1));

				if ((himl = DcxDock::status_getImageList()) == NULL) {
					himl = DcxDock::status_createImageList();

					if (himl != NULL)
						DcxDock::status_setImageList(himl);
				}

				if (himl != NULL) {
					icon = dcxLoadIcon(index, filename, false, flags);

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
			Dcx::error(TEXT("/xstatusbar"),TEXT("Invalid Switch"));
			return 0;
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
	int nType = poslist.findtok(d.gettok( 2 ).to_chr(),1);
	switch (nType)
	{
	case 1: // visible
		{
			if (DcxDock::IsStatusbar())
				lstrcpy(data,TEXT("$true"));
			else
				lstrcpy(data,TEXT("$false"));
		}
		break;
	case 2: // text
		{
			int iPart = d.gettok( 3 ).to_int( ) -1, nParts = (int)DcxDock::status_getParts( 256, 0 );

			if ( iPart > -1 && iPart < nParts ) {
				WCHAR *text = new WCHAR[DcxDock::status_getTextLength( iPart ) + 1];
				DcxDock::status_getText( iPart, text );
#if UNICODE
				lstrcpyn(data, text, MIRC_BUFFER_SIZE_CCH);
#else
				WideCharToMultiByte(CP_UTF8, 0, text, -1, data, MIRC_BUFFER_SIZE_CCH, NULL, NULL);
#endif
				delete [] text;
			}
		}
		break;
	case 3: // parts
		{
			INT parts[256];
			int nParts = (int)DcxDock::status_getParts( 256, 0 );

			DcxDock::status_getParts( 256, parts );

			int i = 0;
			TCHAR dd[10];

			while ( i < nParts ) {

				wsprintf( dd, TEXT("%d"), parts[i] );

				if ( i != 0 )
					lstrcat( data, TEXT(" ") );
				lstrcat( data, dd );

				i++;
			}
		}
		break;
	case 4: // tooltip
		{
			int iPart = d.gettok( 3 ).to_int( ), nParts = (int)DcxDock::status_getParts( 256, 0 );

			if ( iPart > -1 && iPart < nParts ) {
#if UNICODE
				DcxDock::status_getTipText( iPart, MIRC_BUFFER_SIZE_CCH, data );
#else
				WCHAR *text = new WCHAR[MIRC_BUFFER_SIZE_CCH];
				DcxDock::status_getTipText( iPart, MIRC_BUFFER_SIZE_CCH, text );
				WideCharToMultiByte(CP_UTF8, 0, text, -1, data, MIRC_BUFFER_SIZE_CCH, NULL, NULL);
				delete [] text;
#endif
			}
		}
		break;
	case 0: // error
	default:
		{
			TString error;
			error.tsprintf(TEXT("D_ERROR Invalid prop ().%s"), d.gettok( 2 ).to_chr());
			lstrcpy(data, error.to_chr());
		}
		break;
	}
	return 3;
}
