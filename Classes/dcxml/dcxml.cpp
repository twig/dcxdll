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

namespace
{
	DcxmlParser Parser;
}

mIRC(dcxml)
{
	const TString input(data);

	data[0] = 0;

	try {
		const auto numtok = input.numtok();

		if (numtok < 3)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xFlags(input.getfirsttok(1));

		// Parse XDialog XML.
		if (xFlags[TEXT('d')])
		{
			const auto tsMark(input.getnexttok());			// tok 2
			const auto tsName(input.getnexttok());			// tok 3
			auto tsFilename(input.getlasttoks().trim());	// tok 4, -1

			if (!IsFile(tsFilename))
				throw Dcx::dcxException(TEXT("Unable To Access File: %"), tsFilename);

			Parser.ParseXML(tsFilename, tsMark, tsName, (xFlags[TEXT('v')]), (xFlags[TEXT('x')]));
			return 1;
		}
		// Parse XPopup DCXML.

		else if (xFlags[TEXT('p')])
		{
			const auto popupName(input.getnexttok());		// tok 2
			const auto popupDataset(input.getnexttok());		// tok 3
			auto tsFilename(input.getlasttoks().trim());		// tok 4, -1

			{
				const auto popupNameHash = std::hash<TString>{}(popupName);
				if ((popupNameHash == L"mircbar"_hash) || (popupNameHash == L"mirc"_hash) || (popupNameHash == L"scriptpopup"_hash))
					throw Dcx::dcxException(TEXT("Menu name '%' is reserved."), popupName);
			}

			if (!IsFile(tsFilename))
				throw Dcx::dcxException(TEXT("Unable To Access File: %"), tsFilename);

			TiXmlDocument doc(tsFilename.c_str());
			if (!doc.LoadFile())
				throw Dcx::dcxException("Unable to load XML file");

			const auto dcxmlElem = doc.FirstChildElement("dcxml");
			if (dcxmlElem == nullptr)
				throw Dcx::dcxException("Unable to find <dcxml> group");

			const auto* const popups = dcxmlElem->FirstChildElement("popups");
			if (popups == nullptr)
				throw Dcx::dcxException("Unable to find <popups> group");

			XPopupMenuManager::LoadPopupsFromXML(popups, nullptr, popupName, popupDataset);
			return 1;
		}

		// Unknown flags.
		else
			throw Dcx::dcxException(TEXT("Unknown flag %"), input.gettok(1));
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("/dcxml"), TEXT("\"%\" error: %"), input, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("/dcxml"), TEXT("\"%\" error: Unknown Exception"), input);
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
			throw Dcx::dcxException("Invalid Args: A prop is required.");

		if (d.gettok(1) == TEXT("Loaded"))
		{
			dcx_Con(Parser.getLoaded(), data);
		}
		else
			throw Dcx::dcxException("Unknown Property");

		return 3;
	}
	catch (const std::exception& e)
	{
		//Dcx::errorex(TEXT("$!dcxml"), TEXT("\"%s\" error: %S"), d.to_chr(), e.what());
		Dcx::error(TEXT("$!dcxml"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		//Dcx::errorex(TEXT("$!dcxml"), TEXT("\"%s\" error: Unknown Exception"), d.to_chr());
		Dcx::error(TEXT("$!dcxml"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	return 0;
}
