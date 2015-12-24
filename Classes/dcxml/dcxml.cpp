/*!
 * \file dcxml.cpp
 * \brief blah
 *
 * TODO: rename all variables to use the same convention (camelCase)
 *
 * \author Martijn Laarman ( Mpdreamz at mirc dot net )
 * \version 2.2
 *
 * \b Revisions
 *        -Completely rewrote DCXML to be more OOP
 *		  -
 *
 * © ScriptsDB.org - 2006
 */
#include "defines.h"
#include "dcxmlincludes.h"
#include "dcxmlparser.h"

/*
 * /dcxml -[SWITCH] [?NAME] [DATASET] [PARMS]
 *
 * -d = [DNAME] [DATASET] [FILENAME]
 * -p = [PNAME] [DATASET] [FILENAME]
 */

DcxmlParser Parser;

mIRC(dcxml) {
	const TString input(data);

	data[0] = 0;

	try {
		const XSwitchFlags flags(input.getfirsttok(1));
		const auto numtok = input.numtok();

		if (numtok < 3)
			throw std::invalid_argument("Insuffient parameters");

		// Parse XDialog XML.
		if (flags[TEXT('d')]) {
			const auto tsMark(input.getnexttok());			// tok 2
			const auto tsName(input.getnexttok());			// tok 3
			auto tsFilename(input.getlasttoks().trim());	// tok 4, -1

			if (!IsFile(tsFilename))
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unable To Access File: %s"), tsFilename.to_chr()));

			Parser.ParseXML(tsFilename, tsMark, tsName, (flags[TEXT('v')]), (flags[TEXT('x')]));
			return 1;
		}
		// Parse XPopup DCXML.

		else if (flags[TEXT('p')]) {
			const auto popupName(input.getnexttok());		// tok 2
			const auto popupDataset(input.getnexttok());		// tok 3
			auto tsFilename(input.getlasttoks().trim());		// tok 4, -1

			if (!IsFile(tsFilename))
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unable To Access File: %s"), tsFilename.to_chr()));

			TiXmlDocument doc(tsFilename.c_str());
			if (!doc.LoadFile())
				throw std::runtime_error("Unable to load XML file");

			const auto popups = doc.FirstChildElement("dcxml")->FirstChildElement("popups");
			//std::array<TCHAR *, 3U> test = {
			//	TEXT("mircbar"),
			//	TEXT("mirc"),
			//	TEXT("scriptpopup")
			//};

			//static const TCHAR *menuNames[] = {
			//	TEXT("mircbar"),
			//	TEXT("mirc"),
			//	TEXT("scriptpopup")
			//};
			//if (popupName.acompare(menuNames) != 0)
			//	throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Menu name TEXT('%s') is reserved."), popupName.to_chr()));

			static const TString menuNames(TEXT("mircbar mirc scriptpopup"));
			if (menuNames.istok(popupName))
				throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Menu name TEXT('%s') is reserved."), popupName.to_chr()));

			//if ((popupName == TEXT("mircbar")) || (popupName == TEXT("mirc")) || (popupName == TEXT("scriptpopup")))
			//	throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Menu name TEXT('%s') is reserved."), popupName.to_chr()));

			// Couldnt find popups group.
			if (popups == nullptr)
				throw std::invalid_argument("No 'popups' Group");

			XPopupMenuManager::LoadPopupsFromXML(popups, nullptr, popupName, popupDataset);
			return 1;
		}

		// Unknown flags.
		else
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Unknown flag %s"), input.gettok(1).to_chr()));
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("/dcxml"), TEXT("\"%s\" error: %S"), input.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("/dcxml"), TEXT("\"%s\" error: Unknown Exception"), input.to_chr());
	}
	return 0;
}
mIRC(_dcxml)
{
	TString d(data);

	data[0] = 0;

	try {
		d.trim();

		if (d.numtok() != 1)
			throw std::invalid_argument("Invalid Args: A prop is required.");

		if (d.gettok(1) == TEXT("Loaded")) {
			dcx_Con(Parser.getLoaded(), data);
		}
		else
			throw std::invalid_argument("Unknown Property");

		return 3;
	}
	catch (std::exception &e)
	{
		Dcx::errorex(TEXT("$!dcxml"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::errorex(TEXT("$!dcxml"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
	}
	return 0;
}
