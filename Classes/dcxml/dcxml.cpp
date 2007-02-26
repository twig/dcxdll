#include "../../defines.h"
#include "../tinyxml/tinyxml.h"
#include "dcxml.h"
/*
dcxml [-FLAGS] [NAME] [DNAME] "[PATH]"
*/
void parseDialog(TiXmlElement* element, char *dname, int depth=0,char *claPath = "root") {  
	TiXmlElement* child = 0;  
	int control = 0;
	int goDeeper = 1;
	int cCla = 0;
	int resetClaPath = 0;
	for( child = element->FirstChildElement(); child; child = child->NextSiblingElement() ) {
		TiXmlElement* parent = child->Parent()->ToElement();
		const char *elem = child->Value();
		const char *parentelem = parent->Value();
		//fill all required parameters with attributes or default values
		const char *tType = child->Attribute("type");
		const char *type = (tType) ? tType : "panel";
		const char *tWeigth = child->Attribute("weigth");
		const char *weigth = (tWeigth) ? tWeigth : "1";
		const char *tId = child->Attribute("id");
		const char *id = (tId) ? tId : "1";
		const char *tHeight = child->Attribute("height");
		const char *height = (tHeight) ? tHeight : "0";
		const char *tWidth = child->Attribute("width");
		const char *width = (tWidth) ? tWidth : "0";
		const char *tMargin = child->Attribute("margin");
		const char *margin = (tMargin) ? tMargin : "0 0 0 0";
		const char *tStyles = child->Attribute("styles");
		const char *styles = (tStyles) ? tStyles : "";
		const char *tCaption = child->Attribute("caption");
		const char *caption = (tCaption) ? tCaption : "caption";
		const char *tTooltip = child->Attribute("tooltip");
		const char *tooltip = (tTooltip) ? tTooltip : "";
		const char *tCascade = child->Attribute("cascade");
		const char *cascade = (tCascade) ? tCascade : "";
		//rebar specific attributes
		const char *tRebarFlags = child->Attribute("rebarFlags");
		const char *rebarFlags = (tRebarFlags) ? tRebarFlags : "ceg";
		const char *tRebarMinHeight = child->Attribute("rebarMinHeight");
		const char *rebarMinHeight = (tRebarMinHeight) ? tRebarMinHeight : "0";
		const char *tRebarMinWidth = child->Attribute("rebarMinWidth");
		const char *rebarMinWidth = (tRebarMinWidth) ? tRebarMinWidth : "0";
		const char *tRebarColour = child->Attribute("rebarColour");
		const char *rebarColour = (tRebarColour) ? tRebarColour : "0";
		//STEP 1: ADD CONTROL TO DIALOG
		TString cmd;
		if (0==lstrcmp(elem, "control")) {
			control++; cCla++;
			//check how to insert the control in the parent Control/Dialog
			//If parentNode is pane loop untill parentNode is not a pane
			while (parent) { 
				if (0==lstrcmp(parentelem, "pane")) { 
					TiXmlElement* parentFOR2 = parent;
					parent = parentFOR2->Parent()->ToElement();
					parentelem = parent->Value();
				}
				else break;	
			}
			const char *tParentType = parent->Attribute("type");
			const char *parenttype = (tParentType) ? tParentType : "panel";
			const char *tParentId = parent->Attribute("id");
			const char *parentid = (tParentId) ? tParentId : "1";
			if (0==lstrcmp(parentelem, "dialog")) { 
				cmd.sprintf("//xdialog -c %s %s %s 0 0 %s %s %s",dname,id,type,width,height,styles);
			}
			else if (0==lstrcmp(parentelem, "control")) { 
				//check if parentControl is allowed to host controls and assign propper xdid
				if (0==lstrcmp(parenttype, "panel"))
					cmd.sprintf("//xdid -c %s %s %s %s 0 0 %s %s %s", dname,parentid,id,type,width,height,styles);
				else if (0==lstrcmp(parenttype, "tab"))
					cmd.sprintf("//xdid -a %s %s 0 0 %s \t %s %s 0 0 %s %s %s $chr(9) %s",
						dname,parentid,caption,id,type,width,height,styles,tooltip);
				else if (((0==lstrcmp(parenttype, "pager")) || (0==lstrcmp(parenttype, "box"))) && (control == 1))
					cmd.sprintf("/echo -a /xdid -c %s %s %s %s 0 0 %s %s %s", dname,parentid,id,type,width,height,styles);
				else if (0==lstrcmp(parenttype, "divider") && (control <= 2)) {
					if (control > 0) {
						if (control == 1) { 
							cmd.sprintf("//xdid -l %s %s 10 0 \t %s %s 0 0 %s %s %s", dname,parentid,id,type,width,height,styles);
						}
						else if (control == 2) { 
							cmd.sprintf("//xdid -r %s %s 10 0 \t %s %s 0 0 %s %s %s", dname,parentid,id,type,width,height,styles);
						}
					}
				}
				else if (0==lstrcmp(parenttype, "rebar"))
					cmd.sprintf("//xdid -a %s %s 0 +%s %s %s %s 0 %s %s \t %s %s 0 0 %s %s %s \t %s",
						dname,parentid,rebarFlags,rebarMinHeight,
						rebarMinWidth,width,rebarColour,caption,
						id,type,width,height,styles,tooltip);
			}
			mIRCcom(cmd.to_chr());
			//STEP 2: APPLY CLA FOR CONTROL
			//TString cmd; // <- redefinition
			if (0==lstrcmp(type, "panel")) {
				cmd.sprintf("//xdid -l %s %s root \t +p%s 0 0 0 0", dname,id,cascade);
				mIRCcom(cmd.to_chr());
				cmd.sprintf("//xdid -l %s %s space root \t %s", dname,id,margin);
				mIRCcom(cmd.to_chr());
				resetClaPath = 1;
			}
			const char * fHeigth = "";
			const char * fWidth = "";
			const char * fixed = "l";
			if (child->Attribute("height")) { fHeigth = "v"; fixed = "f"; }
			if (child->Attribute("width")) { fWidth = "h"; fixed = "f"; }
			if (0==lstrcmp(parentelem, "dialog"))
				cmd.sprintf("//xdialog -l %s cell %s \t +%s%s%si %s %s %s %s",
					dname,claPath,fixed,fHeigth,fWidth,id,weigth,width,height); 
			else if (0==lstrcmp(parentelem, "control")) {
				if ((parent->Attribute("type")) && (parent->Attribute("id"))) { 
					if (0==lstrcmp(parent->Attribute("type"), "panel"))
						cmd.sprintf("//xdid -l %s %s cell %s \t +%s%s%si %s %s %s %s",
							dname,parentid,claPath,fixed,fHeigth,fWidth,id,weigth,width,height); 
					else cmd= "";
				}
			}
			mIRCcom(cmd.to_chr());
		}
		if (0==lstrcmp(elem, "style")) {
			goDeeper = 0; //No need to call walkScript again
			//Placeholder for style elements that define colours and fonts etc of the parentNode
		}
		else if (0==lstrcmp(elem, "pane")) {
			cCla++;
			while (parent) {
				if (0==lstrcmp(parentelem, "pane")) {
					TiXmlElement* parentFOR2 = parent;
					parent = parentFOR2->Parent()->ToElement();
					parentelem = parent->Value();
				}
				else break;
			}
			if (0==lstrcmp(parentelem, "dialog"))
				cmd.sprintf("//xdialog -l %s cell %s \t +p%s 0 %s 0 0", dname,claPath,cascade,weigth);
			if (0==lstrcmp(parentelem, "control")) {
				if ((parent->Attribute("type")) && (parent->Attribute("id"))) {
					if (0==lstrcmp(parent->Attribute("type"), "panel"))
						cmd.sprintf("//xdid -l %s %s cell %s \t +p%s 0 %s 0 0", dname,parent->Attribute("id"),claPath,cascade,weigth);
				}
				else cmd = "";
			}
			mIRCcom(cmd.to_chr());
		}
		char buffer [100];
		char * claPathx = 0;
		if (resetClaPath) {
			lstrcpy(buffer, "root");
			claPathx = buffer;
		}
		else if (0==lstrcmp(claPath, "root")) {
			wsprintf (buffer, "%i",cCla);
			claPathx = buffer;
		}
		else { 
			wsprintf (buffer, "%s %i",claPath,cCla);
			claPathx = buffer;

		}
		if (goDeeper) parseDialog(child->ToElement(), dname, depth+1,claPathx);  
	}
} 



mIRC(dcxml) {
	TString input(data);
	int numtok = input.numtok( );
	
	if (numtok < 3) {
		DCXError("/dcxml", "Insuffient parameters");
		return 1;
	}

	TString flags(input.gettok( 1 ));
	flags.trim();

	if ((flags[0] != '-') || (flags.len() < 2)) {
		DCXError("/dcxml","No Flags Found");
		return 0;
	}

	if ((flags.find('p', 0) == 0) && (flags.find('d', 0) == 0)) { 
		DCXError("/dcxml","Unknown Flags");
		return 0;
	}

	TiXmlDocument doc(input.gettok(2,"\"").to_chr());
	bool dcxmlFile = doc.LoadFile();
	if (!dcxmlFile) { 
		DCXError("/dcxml","Could Not Load File");
		return 0;
	}

	TiXmlElement *dcxmlRoot = 0;
	dcxmlRoot = doc.FirstChildElement("dcxml");
	if (!dcxmlRoot) {
		DCXError("/dcxml","Root Element Is Not DCXML");
		return 0;
	}
	
	if (flags.find('d', 0)) { 

		TiXmlElement *dcxmlDialogs = dcxmlRoot->FirstChildElement("dialogs");
		if (!dcxmlDialogs) { 
			DCXError("/dcxml -d","No Dialogs Group");
			return 0;
		}

		TiXmlElement* child = 0;
		TiXmlElement* dcxmlDialog = 0;
		for( child = dcxmlDialogs->FirstChildElement("dialog"); child; child = child->NextSiblingElement() ) {
			const char *name = child->Attribute("name");
			if (0==strcmp(name, input.gettok(2," ").to_chr())) { 
				dcxmlDialog = child;
				break;
			}
		}
		if (!dcxmlDialog) { 
			DCXError("/dcxml -d","Dialog name not found in <dialogs>");
			return 0;
		}
		TString cmd = "";
		const char *tCascade = dcxmlDialog->Attribute("cascade");
		const char *cascade = (tCascade) ? tCascade : "v";
		const char *tMargin = dcxmlDialog->Attribute("margin");
		const char *margin = (tMargin) ? tMargin : "0 0 0 0";
		const char *tTitle = dcxmlDialog->Attribute("title");
		cmd.sprintf("/xdialog -l %s root \t +p%s 0 0 0 0", input.gettok(3," ").to_chr(), cascade);
		mIRCcom(cmd.to_chr());
		cmd.sprintf("/xdialog -l %s space root \t %s", input.gettok(3," ").to_chr(), margin);
		mIRCcom(cmd.to_chr());
		parseDialog(dcxmlDialog,input.gettok(3," ").to_chr());
		cmd.sprintf("/.timer 1 0 xdialog -l %s update",input.gettok(3," ").to_chr());
		mIRCcom(cmd.to_chr());
		return 1;

	}
	return 1;
}

