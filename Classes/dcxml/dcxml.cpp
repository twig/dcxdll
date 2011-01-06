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
#include "dcxml.h"
#include "dcxmlparser.h"

//DcxmlParser *Dcxml::Parser;
//
//Dcxml::Dcxml() {
//
//}
DcxmlParser Parser;

/*
 * /dcxml -[SWITCH] [?NAME] [DATASET] [PARMS]
 *
 * -d = [DNAME] [DATASET] [FILENAME]
 * -p = [PNAME] [DATASET] [FILENAME]
 */
mIRC(dcxml) {
    TString input(data);
	XSwitchFlags flags(input.gettok(1));
    int numtok = input.numtok();
    
    if (numtok < 3) {
        Dcx::error("/dcxml", "Insuffient parameters");
        return 0;
    }

	// Parse XDialog XML.
    if (flags['d']) {
		TString tsFilename(input.gettok( 4, -1 ).trim());
		if (!IsFile(tsFilename)) {
			Dcx::errorex("/dcxml -d", "Unable To Access File: %s", tsFilename.to_chr());
			return 0;
		}
		Parser.ParseXML(tsFilename.to_chr(), input.gettok(2).to_chr(), input.gettok(3).to_chr(), (flags['v']), (flags['x']) );
		return (Parser.loadSuccess) ? 1 : 0;
    }
	// Parse XPopup DCXML.
	/*
	else if (flags['p']) {
		TiXmlElement *popups = doc.FirstChildElement("dcxml")->FirstChildElement("popups");
		TiXmlElement *popup = NULL;
		TString popupName(input.gettok(2));
		TString popupDataset(input.gettok(3));
        
		if ((popupName == "mircbar") || (popupName == "mirc") || (popupName == "scriptpopup")) {
			Dcx::errorex("/dcxml", "Menu name '%s' is reserved.", popupName.to_chr());
			return 0;
		}

		// Couldnt find popups group.
        if (!popups) { 
            Dcx::error("/dcxml", "No 'popups' Group");
            return 0;
        }

		XPopupMenuManager::LoadPopupsFromXML(popups, popup, popupName, popupDataset);
		return 1;
	}
	*/
	// Unknown flags.
	else
		Dcx::errorex("/dcxml", "Unknown flag %s", input.gettok(1).to_chr());
	return 0;
}
mIRC(_dcxml)
{
	TString d(data);
	d.trim();

	data[0] = 0;

	if (d.numtok( ) != 1) {
		lstrcpyn(data, "D_ERROR Invalid Args: A prop is required.", MIRC_BUFFER_SIZE_CCH);
		return 3;
	}

	if (d.gettok( 1 ) == "Loaded")
		lstrcpyn(data, (Parser.loadSuccess) ? "$true" : "$false", MIRC_BUFFER_SIZE_CCH);
	return 3;
	
}
