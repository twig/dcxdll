
/*
*/
#include "defines.h"
#include "Classes/UltraDock/dcxDock.h"
#include "Dcx.h"



// [SWITCH] [OPTIONS]
mIRC(xstatusbar) {
	TString input(data);

	data[0] = 0;

	try {
		input.trim();

		const auto numtok = input.numtok();

		if (numtok < 1)
			throw Dcx::dcxException("Invalid Parameters");

		const auto switches(input.getfirsttok(1));	// tok 1

		switch (switches[1]) {
		case TEXT('A'): // -A [0|1] (options)=notheme grip tooltips nodivider utf8
		{
			// check syntax
			if (numtok < 2)
				throw Dcx::dcxException("Invalid Parameters");

			// Enable/Disable the Statusbar.
			// -A [0|1] [options] = notheme grip tooltips nodivider utf8
			if (input.getnexttok().to_int() > 0)					// tok 2
			{
				if (!DcxDock::InitStatusbar(input.getlasttoks()))	// tok 3, -1
					throw Dcx::dcxException("Unable to Create Statusbar");
			}
			else
				DcxDock::UnInitStatusbar();

			UpdatemIRC();
		}
		break;
		case TEXT('f'): // -f [+FLAGS] [CHARSET] [SIZE] [FONTNAME] : set font
		{
			LOGFONT lf = { 0 };

			// check syntax
			if (numtok < 5)
				throw Dcx::dcxException("Invalid Parameters");

			if (ParseCommandToLogfont(input.getlasttoks(), &lf))	// tok 2, -1
				DcxDock::status_setFont(CreateFontIndirect(&lf));
		}
		break;
		case TEXT('k'): // -k [clr] : background colour.
		{
			// check syntax
			if (numtok != 2)
				throw Dcx::dcxException("Invalid Parameters");

			const auto col = input.getnexttok().to_<COLORREF>();	// tok 2

			if (col == CLR_INVALID)
				DcxDock::status_setBkColor(CLR_DEFAULT);
			else
				DcxDock::status_setBkColor(col);
		}
		break;
		case TEXT('l'): // -l [POS [POS POS ...]] : parts
		{
			// check syntax
			if (numtok < 2)
				throw Dcx::dcxException("Invalid Parameters");

			const auto nParts = numtok - 1;
			INT parts[SB_MAX_PARTSD] = { 0 };

			auto c = 0U;
			TString p;
			for (auto i = decltype(nParts){0}; i < nParts; i++)
			{
				if (c >= 100)
					throw Dcx::dcxException("Can't Allocate Over 100% of Statusbar!");

				p = input.getnexttok();	// tok i+2

				const auto t = p.to_int();

				if (p.right(1) == TEXT('%')) {
					DcxDock::g_iDynamicParts[i] = t;
					c += t;
				}
				else
					DcxDock::g_iFixedParts[i] = t;

				parts[i] = t;
			}
			DcxDock::status_setParts(nParts, &parts[0]);
			DcxDock::status_updateParts();
		}
		break;
		case TEXT('t'): // -t N [+FLAGS] [#ICON] [BKGCOLOR] [TXTCOLOR] (Cell Text)([TAB]Tooltip Text) : set part
		{
			// check syntax (text can be blank)
			if (numtok < 6)
				throw Dcx::dcxException("Invalid Parameters");

			const auto tsTabOne(input.getfirsttok(1, TSTABCHAR));	// tok 1, TSTAB
			const auto tooltip(input.getnexttok(TSTABCHAR).trim());	// tok 2, TSTAB;
			TString itemtext;

			const auto nPos = (tsTabOne.getfirsttok(2).to_int() - 1);			// tok 2
			const auto flags(tsTabOne.getnexttok());							// tok 3
			const auto icon = (tsTabOne.getnexttok().to_int() - 1);				// tok 4
			const auto bkgClr = tsTabOne.getnexttok().to_<COLORREF>();			// tok 5
			const auto txtClr = tsTabOne.getnexttok().to_<COLORREF>();			// tok 6
			const auto iFlags = DcxDock::status_parseItemFlags(flags);

			if (tsTabOne.numtok() > 6)
				itemtext = tsTabOne.getlasttoks().trim();						// tok 7, -1

			DcxDock::status_deletePartInfo(nPos); // delete custom info if any.
			DestroyIcon(DcxDock::status_getIcon(nPos));
			DcxDock::status_setIcon(nPos, nullptr);

			if (dcx_testflag(iFlags, SBT_OWNERDRAW)) {
				auto pPart = std::make_unique<SB_PARTINFOD>();

				// commented out parts alrdy set by SB_PARTINFOD constructor
				//pPart->m_Child = nullptr;
				pPart->m_iIcon = icon;
				pPart->m_Text = itemtext;
				//pPart->m_TxtCol = CLR_INVALID;
				//pPart->m_BkgCol = nullptr;

				if (bkgClr != CLR_INVALID)
					pPart->m_BkgCol = CreateSolidBrush(bkgClr);
				if (txtClr != CLR_INVALID)
					pPart->m_TxtCol = txtClr;

				DcxDock::status_setTipText(nPos, tooltip.to_chr());
				DcxDock::status_setPartInfo(nPos, (int)iFlags, pPart.release());
			}
			else {
				if (icon > -1)
					DcxDock::status_setIcon(nPos, ImageList_GetIcon(DcxDock::status_getImageList(), icon, ILD_TRANSPARENT));
				DcxDock::status_setText(nPos, (int)iFlags, itemtext.to_chr());
				DcxDock::status_setTipText(nPos, tooltip.to_chr());
			}
		}
		break;
		case TEXT('v'): // -v [N] [BKGCOLOR] [TXTCOLOR] (TEXT) : set parts text
		{
			// check syntax (text can be blank)
			if (numtok < 4)
				throw Dcx::dcxException("Invalid Parameters");

			const auto nPos = (input.getnexttok().to_int() - 1);	// tok 2

			if (nPos > -1 && (UINT)nPos < DcxDock::status_getParts(SB_MAX_PARTSD, 0)) {

				TString itemtext;
				const auto bkgClr = input.getnexttok().to_<COLORREF>();	// tok 3
				const auto txtClr = input.getnexttok().to_<COLORREF>();	// tok 4

				if (numtok > 4)
					itemtext = input.getlasttoks();	// tok 5, -1

				const auto iFlags = DcxDock::status_getPartFlags(nPos);

				if (dcx_testflag(iFlags, SBT_OWNERDRAW)) {
					auto pPart = reinterpret_cast<LPSB_PARTINFOD>(DcxDock::status_getText(nPos, nullptr));
					if (pPart != NULL) {
						if (pPart->m_BkgCol != nullptr)
							DeleteBrush(pPart->m_BkgCol);

						pPart->m_Text = itemtext;
						if (bkgClr != CLR_INVALID)
							pPart->m_BkgCol = CreateSolidBrush(bkgClr);
						pPart->m_TxtCol = txtClr;

						DcxDock::status_setPartInfo(nPos, (int)iFlags, pPart);
					}
					else
						throw Dcx::dcxException("Unable to set item text");
				}
				else {
					auto text = std::make_unique<WCHAR[]>(DcxDock::status_getTextLength(nPos) + 1);
					DcxDock::status_setText(nPos, HIWORD(DcxDock::status_getText(nPos, text.get())), itemtext.to_chr());
				}
			}
		}
		break;
		case TEXT('w'): // -w [FLAGS] [INDEX] [FILENAME] : load an icon
		{
			// check syntax
			if (numtok < 4)
				throw Dcx::dcxException("Invalid Parameters");

			auto himl = DcxDock::status_getImageList();
			const auto flags(input.getnexttok());				// tok 2
			const auto index = input.getnexttok().to_int();		// tok 3
			auto filename(input.getlasttoks());					// tok 4, -1

			if (himl == nullptr) {
				himl = DcxDock::status_createImageList();

				if (himl != nullptr)
					DcxDock::status_setImageList(himl);
			}

			if (himl == nullptr)
				throw Dcx::dcxException("Unable To Create ImageList");

			//auto icon = dcxLoadIcon(index, filename, false, flags);
			//Auto(DestroyIcon(icon));

			Dcx::dcxIconResource icon(index, filename, false, flags);

			if (icon == nullptr)
				throw Dcx::dcxException("Unable To Load Icon");
				
			if (ImageList_AddIcon(himl, icon) == -1)
				throw Dcx::dcxException("Unable To Add Image to ImageList");
		}
		break;
		case TEXT('y'): // destroy image list.
		{
			ImageList_Destroy(DcxDock::status_getImageList());
			DcxDock::status_setImageList(nullptr);
		}
		break;
		default:
			throw Dcx::dcxException("Invalid Switch");
		}
		return 1;
	}
	catch (const std::exception &e)
	{
		Dcx::errorex(TEXT("/xstatusbar"), TEXT("\"%s\" error: %S"), input.to_chr(), e.what());
		return 0;
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/xstatusbar"), TEXT("\"%s\" error: Unknown Exception"), input.to_chr());
		return 0;
	}
}

// prop
mIRC(_xstatusbar)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

#if DCX_USE_HASHING
		switch (std::hash<TString>{}(d.getfirsttok(2)))
		{
		case TEXT("visible"_hash):
		{
			dcx_Con(DcxDock::IsStatusbar(), data);
		}
		break;
		case TEXT("text"_hash):
		{
			const auto iPart = (d.getnexttok().to_int() - 1), nParts = (int)DcxDock::status_getParts(SB_MAX_PARTSD, 0);

			if (iPart > -1 && iPart < nParts) {
				const auto iFlags = DcxDock::status_getPartFlags(iPart);

				if (dcx_testflag(iFlags, SBT_OWNERDRAW)) {
					auto pPart = reinterpret_cast<LPSB_PARTINFOD>(DcxDock::status_getText(iPart, nullptr));
					if (pPart != nullptr)
						dcx_strcpyn(data, pPart->m_Text.to_chr(), MIRC_BUFFER_SIZE_CCH);
				}
				else {
					const auto len = DcxDock::status_getTextLength(iPart);
					auto text = std::make_unique<WCHAR[]>(len + 1);

					DcxDock::status_getText(iPart, text.get());
					dcx_strcpyn(data, text.get(), MIRC_BUFFER_SIZE_CCH);
				}
			}
		}
		break;
		case TEXT("parts"_hash):
		{
			INT parts[SB_MAX_PARTSD] = { 0 };
			const auto nParts = DcxDock::status_getParts(SB_MAX_PARTSD, 0);

			DcxDock::status_getParts(SB_MAX_PARTSD, &parts[0]);

			TString tsOut((UINT)MIRC_BUFFER_SIZE_CCH);

			for (auto i = decltype(nParts){0}; i < nParts; i++)
				tsOut.addtok(parts[i]);

			dcx_strcpyn(data, tsOut.to_chr(), MIRC_BUFFER_SIZE_CCH);
		}
		break;
		case TEXT("tooltip"_hash):
		{
			const auto iPart = d.getnexttok().to_int(), nParts = (int)DcxDock::status_getParts(SB_MAX_PARTSD, 0);

			if (iPart > -1 && iPart < nParts)
				DcxDock::status_getTipText(iPart, MIRC_BUFFER_SIZE_CCH, data);
		}
		break;
		default:	// error
			throw Dcx::dcxException(TEXT("Invalid prop ().%"), d.gettok(2));
		}
#else
		static const TString poslist(TEXT("visible text parts tooltip"));
		const auto nType = poslist.findtok(d.getfirsttok(2), 1);
		switch (nType)
		{
		case 1: // visible
		{
			dcx_Con(DcxDock::IsStatusbar(), data);
		}
		break;
		case 2: // text
		{
			const auto iPart = (d.getnexttok().to_int() - 1), nParts = (int)DcxDock::status_getParts(SB_MAX_PARTSD, 0);

			if (iPart > -1 && iPart < nParts) {
				const auto iFlags = DcxDock::status_getPartFlags(iPart);

				if (dcx_testflag(iFlags, SBT_OWNERDRAW)) {
					auto pPart = reinterpret_cast<LPSB_PARTINFOD>(DcxDock::status_getText(iPart, nullptr));
					if (pPart != nullptr)
						dcx_strcpyn(data, pPart->m_Text.to_chr(), MIRC_BUFFER_SIZE_CCH);
				}
				else {
					const auto len = DcxDock::status_getTextLength(iPart);
					auto text = std::make_unique<WCHAR[]>(len + 1);

					DcxDock::status_getText(iPart, text.get());
					dcx_strcpyn(data, text.get(), MIRC_BUFFER_SIZE_CCH);
				}
			}
		}
		break;
		case 3: // parts
		{
			INT parts[SB_MAX_PARTSD] = { 0 };
			const auto nParts = DcxDock::status_getParts(SB_MAX_PARTSD, 0);

			DcxDock::status_getParts(SB_MAX_PARTSD, &parts[0]);

			TString tsOut((UINT)MIRC_BUFFER_SIZE_CCH);

			for (auto i = decltype(nParts){0}; i < nParts; i++)
				tsOut.addtok(parts[i]);

			dcx_strcpyn(data, tsOut.to_chr(), MIRC_BUFFER_SIZE_CCH);
		}
		break;
		case 4: // tooltip
		{
			const auto iPart = d.getnexttok().to_int(), nParts = (int)DcxDock::status_getParts(SB_MAX_PARTSD, 0);

			if (iPart > -1 && iPart < nParts)
				DcxDock::status_getTipText(iPart, MIRC_BUFFER_SIZE_CCH, data);
		}
		break;
		case 0: // error
		default:
			throw Dcx::dcxException(TEXT("Invalid prop ().%"), d.gettok(2));
		}
#endif
		return 3;
	}
	catch (const std::exception &e)
	{
		Dcx::errorex(TEXT("$!xstatusbar"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!xstatusbar"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
}
