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
	const XSwitchFlags flags(input.getfirsttok(1));
	const UINT numtok = input.numtok();

	if (numtok < 3) {
		Dcx::error(TEXT("/dcxml"), TEXT("Insuffient parameters"));
		return 0;
	}


	// Parse XDialog XML.
	if (flags[TEXT('d')]) {
		const TString tsMark(input.getnexttok());			// tok 2
		const TString tsName(input.getnexttok());			// tok 3
		TString tsFilename(input.getlasttoks( ).trim());	// tok 4, -1

		if (!IsFile(tsFilename)) {
			Dcx::errorex(TEXT("/dcxml -d"), TEXT("Unable To Access File: %s"), tsFilename.to_chr());
			return 0;
		}
		Parser.ParseXML(tsFilename, tsMark, tsName, (flags[TEXT('v')]), (flags[TEXT('x')]) );
		return (Parser.loadSuccess) ? 1 : 0;
	}
	// Parse XPopup DCXML.

	else if (flags[TEXT('p')]) {
		const TString popupName(input.getnexttok());		// tok 2
		const TString popupDataset(input.getnexttok());		// tok 3
		TString tsFilename(input.getlasttoks().trim());		// tok 4, -1

		if (!IsFile(tsFilename)) {
			Dcx::errorex(TEXT("/dcxml -p"), TEXT("Unable To Access File: %s"), tsFilename.to_chr());
			return 0;
		}
		TiXmlDocument doc(tsFilename.c_str());
		if (!doc.LoadFile()) {
			Dcx::error(TEXT("/dcxml"), TEXT("Unable to load XML file"));
			return 0;
		}
		TiXmlElement *popups = doc.FirstChildElement("dcxml")->FirstChildElement("popups");

		if ((popupName == TEXT("mircbar")) || (popupName == TEXT("mirc")) || (popupName == TEXT("scriptpopup"))) {
			Dcx::errorex(TEXT("/dcxml"), TEXT("Menu name TEXT('%s') is reserved."), popupName.to_chr());
			return 0;
		}

		// Couldnt find popups group.
		if (!popups) { 
			Dcx::error(TEXT("/dcxml"), TEXT("No TEXT('popups') Group"));
			return 0;
		}

		XPopupMenuManager::LoadPopupsFromXML(popups, NULL, popupName, popupDataset);
		return 1;
	}

	// Unknown flags.
	else
		Dcx::errorex(TEXT("/dcxml"), TEXT("Unknown flag %s"), input.gettok(1).to_chr());
	return 0;
}
mIRC(_dcxml)
{
	TString d(data);
	d.trim();

	data[0] = 0;
	if (d.numtok( ) != 1) {
		ret(TEXT("D_ERROR Invalid Args: A prop is required."));
	}

	if (d.gettok(1) == TEXT("Loaded")) {
		//dcx_strcpyn(data, ((Parser.loadSuccess) ? TEXT("$true") : TEXT("$false")), MIRC_BUFFER_SIZE_CCH);
		dcx_Con(Parser.loadSuccess, data);
	}
	return 3;
}
