
/*
*/

#include "dcxDock.h"

// [SWITCH] [OPTIONS]
mIRC(xstatusbar) {
	TString input(data);
	input = input.trim();
	data[0] = 0;

	int numtok = input.numtok( );

	if (numtok < 1) {
		DCXError("/xstatusbar","Invalid Parameters");
		return 0;
	}

	TString switches(input.gettok(1));

	switch (switches[1]) {
		case 'A':
			{
				// Enable/Disable the Statusbar.
				// [-A] [0|1] [options] = notheme grip tooltips nodivider utf8
				if (input.gettok(2).to_int() > 0) {
					if (!DcxDock::InitStatusbar(input.gettok(3,-1))) {
						DCXError("/xstatusbar -A","Unable to Create Statusbar");
						return 0;
					}
				}
				else
					DcxDock::UnInitStatusbar();

				UpdatemIRC();
			}
			break;
		case 'f': // [-f] [+FLAGS] [CHARSET] [SIZE] [FONTNAME] : set font
			{
				LOGFONT lf;

				if (ParseCommandToLogfont(input.gettok(2, -1), &lf)) {
					DcxDock::status_setFont(CreateFontIndirect(&lf));
				}
			}
			break;
		case 'k': // [clr] : background colour.
			{
				int col = input.gettok( 2 ).to_int();

				if (col < 0)
					DcxDock::status_setBkColor((COLORREF) CLR_DEFAULT);
				else
					DcxDock::status_setBkColor((COLORREF) col);
			}
			break;
		case 'l': // [POS [POS POS ...]] : parts
			{
				int nParts = numtok - 1;
				INT parts[256];

				int i = 0, c = 0, t = 0;
				TString p;
				while ( i < nParts ) {

					if (c >= 100) {
						DCXError("/xstatusbar -l","Can't Allocate Over 100% of Statusbar!");
						return 0;
					}

					p = input.gettok( i+2 );

					t = p.to_int();

					if (p.right( 1 ) == "%") {
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
		case 't': // N [+FLAGS] [#ICON] [Cell Text]([TAB]Tooltip Text) : set part
			{
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
					if (flags.find('f',0)) { // mIRC formatted text
						pPart->m_Text = itemtext;
						DcxDock::status_setTipText( nPos, tooltip.to_wchr(DcxDock::g_bUseUTF8 ) );
						DcxDock::status_setPartInfo( nPos, iFlags, pPart );
					}
					else { // child control
						DCXError("/xstatusbar -t","Child Controls Are not supported at this time.");
					}
				}
				else {
					if ( icon > -1 )
						DcxDock::status_setIcon( nPos, ImageList_GetIcon( DcxDock::status_getImageList( ), icon, ILD_TRANSPARENT ) );
					DcxDock::status_setText( nPos, iFlags, itemtext.to_wchr(DcxDock::g_bUseUTF8) );
					DcxDock::status_setTipText( nPos, tooltip.to_wchr(DcxDock::g_bUseUTF8) );
				}
			}
			break;
		case 'v': // [N] (TEXT) : set parts text
			{
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
						DcxDock::status_setText( nPos, HIWORD( DcxDock::status_getText( nPos, text ) ), itemtext.to_wchr(DcxDock::g_bUseUTF8) );
						delete [] text;
					}
				}
			}
			break;
		case 'w': // [FLAGS] [INDEX] [FILENAME] : load an icon
			{
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
					icon = dcxLoadIcon(index, filename, FALSE, flags);

					ImageList_AddIcon(himl, icon);
					DestroyIcon(icon);
				}
				else
					DCXError("/xstatusbar -w","Unable To Create ImageList");
			}
			break;
		case 'y': // destroy image list.
			{
				ImageList_Destroy( DcxDock::status_getImageList( ) );
				DcxDock::status_setImageList(NULL);
			}
			break;
		default:
			DCXError("/xstatusbar","Invalid Switch");
			return 0;
	}
	return 1;
}

// prop
mIRC(_xstatusbar)
{
	TString d(data);
	d = d.trim();

	data[0] = 0;

	static const TString poslist("visible text parts tooltip");
	int nType = poslist.findtok(d.gettok( 2 ).to_chr(),1);
	switch (nType)
	{
	case 1: // visible
		{
			if (DcxDock::IsStatusbar())
				lstrcpy(data,"$true");
			else
				lstrcpy(data,"$false");
		}
		break;
	case 2: // text
		{
			int iPart = d.gettok( 3 ).to_int( ) -1, nParts = (int)DcxDock::status_getParts( 256, 0 );

			if ( iPart > -1 && iPart < nParts ) {
				WCHAR *text = new WCHAR[DcxDock::status_getTextLength( iPart ) + 1];
				DcxDock::status_getText( iPart, text );
				WideCharToMultiByte(CP_UTF8, 0, text, -1, data, 900, NULL, NULL);
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
			char dd[10];

			while ( i < nParts ) {

				wsprintf( dd, "%d", parts[i] );

				if ( i != 0 )
					lstrcat( data, " " );
				lstrcat( data, dd );

				i++;
			}
		}
		break;
	case 4: // tooltip
		{
			int iPart = d.gettok( 3 ).to_int( ), nParts = (int)DcxDock::status_getParts( 256, 0 );

			if ( iPart > -1 && iPart < nParts ) {
				WCHAR *text = new WCHAR[900];
				DcxDock::status_getTipText( iPart, 900, text );
				WideCharToMultiByte(CP_UTF8, 0, text, -1, data, 900, NULL, NULL);
				delete [] text;
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
