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
#include "dcxmlincludes.h"
#include "dcxml.h"
#include "dcxmlparser.h"

DcxmlParser *Dcxml::Parser;

Dcxml::Dcxml() {

};
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
        return 1;
    }


	// Parse XDialog XML.
    if (flags['d']) {
		Dcxml::Parser = new DcxmlParser( 
			input.gettok(2,"\"").to_chr()	
			,input.gettok(2).to_chr()		
			,input.gettok(3).to_chr()
			,(flags['v'])
			,(flags['x'])
		);
		return (Dcxml::Parser->loadSuccess) ? 0 : 1;
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
	else {
		Dcx::errorex("/dcxml", "Unknown flag %s", input.gettok(1).to_chr());
		return 0;
	}

    return 1;
}
mIRC(_dcxml)
{
	TString d(data);
	d.trim();

	data[0] = 0;

	if (d.numtok( ) != 1) {
		lstrcpy(data, "D_ERROR Invalid Args: A prop is required.");
		return 3;
	}

	if (d.gettok( 1 ) == "Loaded")
	{
		lstrcpy(data, (Dcxml::Parser->loadSuccess) ? "$true" : "$false");
	}
	return 3;
	
}