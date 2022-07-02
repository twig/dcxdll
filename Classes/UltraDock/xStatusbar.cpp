
/*
*/
#include "defines.h"
#include "Classes/UltraDock/dcxDock.h"
#include "Dcx.h"

namespace {
	TString xstatus_GetText(const int iPart)
	{
		TString tsText(DcxDock::status_getTextLength(iPart));
		DcxDock::status_getText(iPart, tsText.to_chr());

		return tsText;
	}

	LPSB_PARTINFOD xstatus_GetPart(const int iPart) noexcept
	{
		return reinterpret_cast<LPSB_PARTINFOD>(DcxDock::status_getText(iPart, nullptr));
	}
}

// [SWITCH] [OPTIONS]
mIRC(xstatusbar)
{
	TString input(data);

	data[0] = 0;

	try {
		input.trim();

		const auto numtok = input.numtok();

		if (numtok < 1)
			throw DcxExceptions::dcxInvalidArguments();

		switch (const auto switches(input.getfirsttok(1)); switches[1])
		{
		case TEXT('A'): // -A [0|1] (options)=notheme grip tooltips nodivider utf8
		{
			// check syntax
			if (numtok < 2)
				throw DcxExceptions::dcxInvalidArguments();

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
			// check syntax
			if (numtok < 5)
				throw DcxExceptions::dcxInvalidArguments();

			if (LOGFONT lf{}; ParseCommandToLogfont(input.getlasttoks(), &lf))	// tok 2, -1
				DcxDock::status_setFont(CreateFontIndirect(&lf));
		}
		break;
		case TEXT('k'): // -k [clr] : background colour.
		{
			// check syntax
			if (numtok != 2)
				throw DcxExceptions::dcxInvalidArguments();

			if (const auto col = input.getnexttok().to_<COLORREF>(); col == CLR_INVALID)
				DcxDock::status_setBkColor(CLR_DEFAULT);
			else
				DcxDock::status_setBkColor(col);
		}
		break;
		case TEXT('l'): // -l [POS [POS POS ...]] : parts
		{
			// check syntax
			if (numtok < 2)
				throw DcxExceptions::dcxInvalidArguments();

			const auto nParts = numtok - 1;
			INT parts[SB_MAX_PARTSD] = { 0 };

			auto c = 0U;
			TString p;
			for (auto i = decltype(nParts){0}; i < nParts; ++i)
			{
				if (c >= 100)
					throw Dcx::dcxException("Can't Allocate Over 100% of Statusbar!");

				p = input.getnexttok();	// tok i+2

				const auto t = p.to_int();

				if (p.right(1) == TEXT('%'))
				{
					gsl::at(DcxDock::g_iDynamicParts,i) = t;
					c += t;
				}
				else
					gsl::at(DcxDock::g_iFixedParts,i) = t;

				gsl::at(parts,i) = t;
			}
			DcxDock::status_setParts(nParts, &parts[0]);
			DcxDock::status_updateParts();
		}
		break;
		case TEXT('t'): // -t N [+FLAGS] [#ICON] [BKGCOLOR] [TXTCOLOR] (Cell Text)([TAB]Tooltip Text) : set part
		{
			// check syntax (text can be blank)
			if (numtok < 6)
				throw DcxExceptions::dcxInvalidArguments();

			const auto tsTabOne(input.getfirsttok(1, TSTABCHAR));	// tok 1, TSTAB
			const auto tooltip(input.getnexttok(TSTABCHAR).trim());	// tok 2, TSTAB;
			TString itemtext;

			const auto nPos = (tsTabOne.getfirsttok(2).to_int() - 1);			// tok 2
			const auto flags(tsTabOne.getnexttok());							// tok 3
			const auto icon = (tsTabOne.getnexttok().to_int() - 1);				// tok 4
			const auto bkgClr = tsTabOne.getnexttok().to_<COLORREF>();			// tok 5
			const auto txtClr = tsTabOne.getnexttok().to_<COLORREF>();			// tok 6

			if (flags[0] != TEXT('+'))
				throw DcxExceptions::dcxInvalidFlag();

			const auto iFlags = DcxDock::status_parseItemFlags(flags);

			if (tsTabOne.numtok() > 6)
				itemtext = tsTabOne.getlasttoks().trim();						// tok 7, -1

			DcxDock::status_deletePartInfo(nPos); // delete custom info if any.
			DestroyIcon(DcxDock::status_getIcon(nPos));
			DcxDock::status_setIcon(nPos, nullptr);

			if (dcx_testflag(iFlags, SBT_OWNERDRAW))
			{
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
				DcxDock::status_setPartInfo(nPos, gsl::narrow_cast<int>(iFlags), pPart.release());
			}
			else {
				if (icon > -1)
					DcxDock::status_setIcon(nPos, ImageList_GetIcon(DcxDock::status_getImageList(), icon, ILD_TRANSPARENT));
				DcxDock::status_setText(nPos, gsl::narrow_cast<int>(iFlags), itemtext.to_chr());
				DcxDock::status_setTipText(nPos, tooltip.to_chr());
			}
		}
		break;
		case TEXT('v'): // -v [N] [BKGCOLOR] [TXTCOLOR] (TEXT) : set parts text
		{
			// check syntax (text can be blank)
			if (numtok < 4)
				throw DcxExceptions::dcxInvalidArguments();

			if (const auto nPos = (input.getnexttok().to_int() - 1); (nPos > -1 && gsl::narrow_cast<UINT>(nPos) < DcxDock::status_getParts(SB_MAX_PARTSD, nullptr)))
			{
				TString itemtext;
				const auto bkgClr = input.getnexttok().to_<COLORREF>();	// tok 3
				const auto txtClr = input.getnexttok().to_<COLORREF>();	// tok 4

				if (numtok > 4)
					itemtext = input.getlasttoks();	// tok 5, -1

				if (const auto iFlags = DcxDock::status_getPartFlags(nPos); dcx_testflag(iFlags, SBT_OWNERDRAW))
				{
					//if (const auto pPart = reinterpret_cast<LPSB_PARTINFOD>(DcxDock::status_getText(nPos, nullptr)); pPart)
					if (const auto pPart = xstatus_GetPart(nPos); pPart)
					{
						if (pPart->m_BkgCol)
							DeleteObject(pPart->m_BkgCol);

						pPart->m_Text = itemtext;
						if (bkgClr != CLR_INVALID)
							pPart->m_BkgCol = CreateSolidBrush(bkgClr);
						pPart->m_TxtCol = txtClr;

						DcxDock::status_setPartInfo(nPos, gsl::narrow_cast<int>(iFlags), pPart);
					}
					else
						throw Dcx::dcxException("Unable to set item text");
				}
				else {
					auto text = std::make_unique<WCHAR[]>(DcxDock::status_getTextLength(nPos) + 1);
					DcxDock::status_setText(nPos, Dcx::dcxHIWORD(DcxDock::status_getText(nPos, text.get())), itemtext.to_chr());
				}
			}
		}
		break;
		case TEXT('w'): // -w [FLAGS] [INDEX] [FILENAME] : load an icon
		{
			// check syntax
			if (numtok < 4)
				throw DcxExceptions::dcxInvalidArguments();

			auto himl = DcxDock::status_getImageList();
			const auto flags(input.getnexttok());				// tok 2
			const auto index = input.getnexttok().to_int();		// tok 3
			auto filename(input.getlasttoks());					// tok 4, -1

			if (!himl)
			{
				himl = DcxDock::status_createImageList();

				if (himl)
					DcxDock::status_setImageList(himl);
			}

			if (!himl)
				throw Dcx::dcxException("Unable To Create ImageList");

#if DCX_USE_WRAPPERS
			Dcx::dcxIconResource icon(index, filename, false, flags);

			if (ImageList_AddIcon(himl, icon.get()) == -1)
				throw Dcx::dcxException("Unable To Add Image to ImageList");
#else
			auto icon = dcxLoadIcon(index, filename, false, flags);
			Auto(DestroyIcon(icon));

			if (!icon)
				throw Dcx::dcxException("Unable To Load Icon");

			if (ImageList_AddIcon(himl, icon) == -1)
				throw Dcx::dcxException("Unable To Add Image to ImageList");
#endif
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
		Dcx::error(TEXT("/xstatusbar"), TEXT("\"%\" error: %"), input, e.what());
		return 0;
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("/xstatusbar"), TEXT("\"%\" error: Unknown Exception"), input);
		return 0;
	}
}

// prop
mIRC(_xstatusbar)
{
	TString d(data);

	data[0] = 0;

	const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> refData(data);

	try {
		d.trim();

		switch (std::hash<TString>{}(d.getfirsttok(2)))
		{
		case TEXT("visible"_hash):
		{
			dcx_Con(DcxDock::IsStatusbar(), data);
		}
		break;
		case TEXT("text"_hash):
		{
			if (const auto iPart = (d.getnexttok().to_int() - 1), nParts = gsl::narrow_cast<int>(DcxDock::status_getParts(SB_MAX_PARTSD, nullptr)); (iPart > -1 && iPart < nParts))
			{
				if (const auto iFlags = DcxDock::status_getPartFlags(iPart); dcx_testflag(iFlags, SBT_OWNERDRAW))
				{
					//if (const auto *const pPart = reinterpret_cast<LPSB_PARTINFOD>(DcxDock::status_getText(iPart, nullptr)); pPart)
					//	refData = pPart->m_Text;

					if (const auto* const pPart = xstatus_GetPart(iPart); pPart)
						refData = pPart->m_Text;
				}
				else {
					//const auto len = DcxDock::status_getTextLength(iPart);
					//auto text = std::make_unique<WCHAR[]>(len + 1);
					//
					//DcxDock::status_getText(iPart, text.get());
					//refData = text.get();

					refData = xstatus_GetText(iPart);
				}
			}
		}
		break;
		case TEXT("parts"_hash):
		{
			INT parts[SB_MAX_PARTSD] = { 0 };
			const auto nParts = DcxDock::status_getParts(SB_MAX_PARTSD, nullptr);

			DcxDock::status_getParts(SB_MAX_PARTSD, &parts[0]);

			TString tsOut(gsl::narrow_cast<UINT>(mIRCLinker::c_mIRC_Buffer_Size_cch));

			for (auto i = decltype(nParts){0}; i < nParts; ++i)
				tsOut.addtok(gsl::at(parts,i));

			refData = tsOut;
		}
		break;
		case TEXT("tooltip"_hash):
		{
			if (const auto iPart = d.getnexttok().to_int(), nParts = gsl::narrow_cast<int>(DcxDock::status_getParts(SB_MAX_PARTSD, nullptr)); (iPart > -1 && iPart < nParts))
				DcxDock::status_getTipText(iPart, mIRCLinker::c_mIRC_Buffer_Size_cch, data);
		}
		break;
		default:	// error
			throw Dcx::dcxException(TEXT("Invalid prop ().%"), d.gettok(2));
		}
		return 3;
	}
	catch (const std::exception &e)
	{
		Dcx::error(TEXT("$!xstatusbar"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("$!xstatusbar"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	return 0;
}
