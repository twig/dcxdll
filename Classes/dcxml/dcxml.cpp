#include "../../defines.h"
#include "../tinyxml/tinyxml.h"
#include "dcxml.h"
/*
dcxml [-FLAGS] [NAME] [DNAME] "[PATH]"
*/
class DCXML {
public:
	TiXmlElement* root; //!< dcxml root element
	TiXmlElement* dialogs; //!< dcxml dialogs collection element
	TiXmlElement* dialog; //!< dialog element
	TiXmlElement* element; //!< current Element
	TiXmlElement* parent; //!< current Element's parent
	TString dname;
	int controls; //!< Simple counter for controls

	//Attribute vars
		int id;
		int parentid;
		const char *elem;
		const char *parentelem;
		const char *parenttype;
		const char *type;
		const char *weigth;
		const char *height;
		const char *width;
		const char *margin;
		const char *styles;
		const char *caption;
		const char *tooltip;
		const char *cascade;
		const char *colour;
		const char *icon;
		const char *tFlags;
		const char *integral;
		const char *bgcolour;
		const char *state;
		const char *indent;
		const char *src;
		const char *cells;
		const char *rebarMinHeight;
		const char *rebarMinWidth;
		const char *iconsize;
	//tempvar to dump attribute values in;
	const char *temp;
	TString cmd;

	DCXML() {
		root = 0; 
		dialogs = 0; 
		dialog = 0; 
		element = 0;
		controls = 0;
	}
	void parseAttributes() {
		elem = element->Value();
		parentelem = parent->Value();
		parenttype = (temp = parent->Attribute("type")) ? temp : "panel";
		type = (temp = element->Attribute("type")) ? temp : "panel";
		weigth = (temp = element->Attribute("weigth")) ? temp : "1";
		height = (temp = element->Attribute("height")) ? temp : "0";
		width = (temp = element->Attribute("width")) ? temp : "0";
		margin = (temp = element->Attribute("margin")) ? temp : "0 0 0 0";
		styles = (temp = element->Attribute("styles")) ? temp : "";
		caption = (temp = element->Attribute("caption")) ? temp : element->GetText();
		tooltip = (temp = element->Attribute("tooltip")) ? temp : "";
		cascade = (temp = element->Attribute("cascade")) ? temp : "";
		colour = (temp = element->Attribute("colour")) ? temp : "0";
		icon = (temp = element->Attribute("icon")) ? temp : "0";
		integral = (temp = element->Attribute("integral")) ? temp : "0";
		bgcolour = (temp = element->Attribute("bgcolour")) ? temp : "0";
		state = (temp = element->Attribute("state")) ? temp : "0";
		indent = (temp = element->Attribute("indent")) ? temp : "0";
		//flags attribute defaults different per type/item
		tFlags = element->Attribute("flags");
		src = (temp = element->Attribute("src")) ? temp : "";
		cells = (temp = element->Attribute("cells")) ? temp : "-1";
		rebarMinHeight = (temp = element->Attribute("minheight")) ? temp : "0";
		rebarMinWidth = (temp = element->Attribute("minwidth")) ? temp : "0";
		iconsize = (temp = element->Attribute("iconsize")) ? temp : "16";
	}
	void parseDialog(int depth=0,char *claPath = "root",int passedid = 2000) { 
		TiXmlElement* child = 0;
		int control = 0;
		int cCla = 0;
		int cell = 0;
		for( child = element->FirstChildElement(); child; child = child->NextSiblingElement() ) {
			cell++;
			//STEP 1: SET ELEMENT AND PARENTELEMENT
			parent = child->Parent()->ToElement();
			element = child->ToElement();
			
			//STEP 2: PARSE ATTRIBUTES OF ELEMENTS
			parseAttributes();
			if (0==lstrcmp(elem, "control")) controls++;
			if (0==lstrcmp(elem, "control")) id = (element->QueryIntAttribute("id",&id) == TIXML_SUCCESS) ? id : 2000 - controls;
			else id = passedid;
			passedid = id;
			mIRCcomEX("//echo -a dname:%s type:%s id:%i",dname,type,id);
			mIRCcomEX("//echo -a id:%i.",id);
			
			//STEP 3: IF CONTROL CREATE IT AND ITS ITEMS
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
				parenttype = (temp = parent->Attribute("type")) ? temp : "panel";
				parentid = (parent->QueryIntAttribute("id",&parentid) == TIXML_SUCCESS) ? parentid : passedid;
				//if (0==lstrcmp(parentelem, "dialog"))
					//mIRCcomEX("//echo -a xdialog -c %s %i",dname,id);
			}
			mIRCcomEX("//echo -a current depth:%i type:%s id:%i",depth,type,id);
			parseDialog(depth+1);  
		}
	} 

protected:

};
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
	DCXML oDcxml;
	TiXmlBase::SetCondenseWhiteSpace( false ); 
	TiXmlDocument doc(input.gettok(2,"\"").to_chr());
	bool dcxmlFile = doc.LoadFile();
	if (!dcxmlFile) { 
		DCXError("/dcxml","Could Not Load File");
		return 0;
	}
	oDcxml.root = doc.FirstChildElement("dcxml");
	if (!oDcxml.root) {
		DCXError("/dcxml","Root Element Is Not DCXML");
		return 0;
	}
	
	if (flags.find('d', 0)) { 

		oDcxml.dialogs = oDcxml.root->FirstChildElement("dialogs");
		if (!oDcxml.dialogs) { 
			DCXError("/dcxml","No Dialogs Group");
			return 0;
		}
		for( oDcxml.element = oDcxml.dialogs->FirstChildElement("dialog"); oDcxml.element; oDcxml.element = oDcxml.element->NextSiblingElement() ) {
			const char *name = oDcxml.element->Attribute("name");
			if (0==strcmp(name, input.gettok(2," ").to_chr())) { 
				oDcxml.dialog = oDcxml.element;
				break;
			}
		}
		if (!oDcxml.dialog) { 
			DCXError("/dcxml","Dialog name not found in <dialogs>");
			return 0;
		}
		oDcxml.dname = input.gettok(3," ").to_chr();
		oDcxml.temp = oDcxml.dialog->Attribute("cascade");
		const char *cascade = (oDcxml.temp) ? oDcxml.temp : "v";
		oDcxml.temp = oDcxml.dialog->Attribute("margin");
		const char *margin = (oDcxml.temp) ? oDcxml.temp : "0 0 0 0";
		oDcxml.temp = oDcxml.dialog->Attribute("title");
		const char *border = oDcxml.dialog->Attribute("border");
		
		if (border) mIRCcomEX("/xdialog -b %s +%s", oDcxml.dname, border);
		mIRCcomEX("/echo -a xdialog -l %s space root \t + %s", oDcxml.dname, margin);
		mIRCcomEX("/xdialog -l %s root \t +p%s 0 0 0 0",oDcxml.dname, cascade);
		mIRCcomEX("/xdialog -l %s space root \t + %s", oDcxml.dname, margin);
		oDcxml.parseDialog();
		mIRCcomEX("/.timer 1 0 xdialog -l %s update",oDcxml.dname);
		return 1;
	}
	return 1;
}
void parseIcons(TiXmlElement* root,char *dname, const char *type, const char *id) { 
	TiXmlElement* icons = root->FirstChildElement("icons");
	TiXmlElement* child = 0;
	TiXmlElement* icon = 0; 
	TString cmd;
	for( child = icons->FirstChildElement("icon"); child; child = child->NextSiblingElement()) {
		const char *tempid = child->Attribute("id");
		tempid = (tempid) ? tempid : "0";
		const char *temptype = child->Attribute("type");
		if ((0==lstrcmp(tempid, id)) || ((0==lstrcmp(temptype, type)) && (!icon))) { 
			icon = child;
			const char *tFlags = child->Attribute("flags");
			const char *flags = (tFlags) ? tFlags : "ndhs";
			const char *tIndex = child->Attribute("index");
			const char *index = (tIndex) ? tIndex : "0";
			const char *src = child->Attribute("src");
			if ((child->Attribute("indexmin")) && (child->Attribute("indexmax"))) { 
					const char *min = child->Attribute("indexmin");
					const char *max = child->Attribute("indexmax");
					cmd.sprintf(
						"//var %%x = %s | while (%%x <= %s ) { xdid -w %s %s +%s %%x %s | inc %%x }",
						min,max,dname,id,flags,src);
					mIRCcom(cmd.to_chr());
			}
			else { 
				cmd.sprintf("//xdid -w %s %s +%s %s %s",dname,id,flags,index,src);
				mIRCcom(cmd.to_chr());
			}
		}
	}
}

void parseItems(TiXmlElement* element,char *dname,const char *parentid,int depth = 0,char *itemPath = "") { 
	int item = 0;
	int cell = 0;
	TiXmlElement* child = 0;
	TString cmd;
	for( child = element->FirstChildElement(); child; child = child->NextSiblingElement() ) {
		cell++;
		TiXmlElement* parent = child->Parent()->ToElement();
		const char *elem = child->Value();
		const char *parentelem = parent->Value();
			while (parent) { 
				if (0==lstrcmp(parentelem, "item")) { 
					TiXmlElement* parentFOR2 = parent;
					parent = parentFOR2->Parent()->ToElement();
					parentelem = parent->Value();
				}
				else break;	
		}
		const char *tParentType = parent->Attribute("type");
		const char *parenttype = (tParentType) ? tParentType : "panel";
	
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
		const char *tColour = child->Attribute("colour");
		const char *colour = (tColour) ? tColour : "0";
		const char *tIcon = child->Attribute("icon");
		const char *icon = (tIcon) ? tIcon : "0";
		const char *tFlags = child->Attribute("flags");
		const char *tIntegral = child->Attribute("integral");
		const char *integral = (tIntegral) ? tIntegral : "0";
		const char *tBgcolour = child->Attribute("bgcolour");
		const char *bgcolour = (tBgcolour) ? tBgcolour : "0";
		const char *tState = child->Attribute("state");
		const char *state = (tState) ? tState : "0";
		const char *tIndent = child->Attribute("indent");
		const char *indent = (tIndent) ? tIndent : "0";
		if (0==lstrcmp(elem, "item")) { 
			item++;
			
			if (0==lstrcmp(parenttype, "toolbar")) { 
				const char *flags = (tFlags) ? tFlags : "a";
				cmd.sprintf("//xdid -a %s %s 0 +%s %s %s %s %s \t %s",
					dname,parentid,flags,width,icon,colour,caption,tooltip);
			}
			else if (0==lstrcmp(parenttype, "comboex")) { 
				cmd.sprintf("//xdid -a %s %s 0 %s %s %s 0 %s",
					dname,parentid,indent,icon,icon,caption);
			}
			else if (0==lstrcmp(parenttype, "list")) { 
				cmd.sprintf("//xdid -a %s %s 0 %s",
					dname,parentid,caption);
			}
			else if (0==lstrcmp(parenttype, "statusbar")) { 
				const char *flags = (tFlags) ? tFlags : "f";
				cmd.sprintf("//xdid -t %s %s %i +%s %s %s \t %s",
					dname,parentid,cell,flags,icon,caption,tooltip);
			}
			else if (0==lstrcmp(parenttype, "treeview")) { 
				const char *flags = (tFlags) ? tFlags : "a";
				char buffer [100];
				char * pathx = 0;
				wsprintf (buffer, "%s %i",itemPath,item);
				pathx = buffer;
				//xdid -a [DNAME] [ID] [PATH] [TAB] [+FLAGS] [ICON] [SICON] [OVERLAY] [STATE] [INTEGRAL] [COLOR] [BGCOLOR] (Item Text) [TAB] (Tooltip Text)
				cmd.sprintf("//xdid -a %s %s %s \t +%s %s %s 0 %s %s %s %s %s \t %s",
					dname,parentid,pathx,flags,icon,icon,state,integral,colour,bgcolour,caption,tooltip);
				mIRCcom(cmd.to_chr());
				parseItems(child,dname,parentid,depth,pathx);
				cmd = "";
			}

			mIRCcom(cmd.to_chr());

		}
	}
}
void parseDialog(TiXmlElement* root,TiXmlElement* element, char *dname, int depth=0,char *claPath = "root",TString passedid = "1") { 
	TiXmlElement* child = 0;  
	int control = 0;
	int goDeeper = 1;
	int cCla = 0;
	int resetClaPath = 0;
	int cell = 0;
	for( child = element->FirstChildElement(); child; child = child->NextSiblingElement() ) {
		cell++;
		TiXmlElement* parent = child->Parent()->ToElement();
		const char *elem = child->Value();
		const char *parentelem = parent->Value();
		int step2 = 1;
		int step3 = 1;
		//fill all required parameters with attributes or default values
		const char *tType = child->Attribute("type");
		const char *type = (tType) ? tType : "panel";
		const char *tWeigth = child->Attribute("weigth");
		const char *weigth = (tWeigth) ? tWeigth : "1";
		const char *tId = child->Attribute("id");
		if ((!tId) && (0==lstrcmp(elem, "control"))) { 
			char eval[100];
			TString str;
			str.sprintf("$calc(2000 - $xdialog(%s,0).id)", dname);
			mIRCeval(str.to_chr(), eval);
			tId = eval;
		}
		else if ((!tId) && (0==lstrcmp(elem, "pane"))) { 
			tId = passedid.to_chr();
		}
		TString cmd;
		const char *id = tId;
		TString idpass(id);
		const char *tHeight = child->Attribute("height");
		const char *height = (tHeight) ? tHeight : "0";
		const char *tWidth = child->Attribute("width");
		const char *width = (tWidth) ? tWidth : "0";
		const char *tMargin = child->Attribute("margin");
		const char *margin = (tMargin) ? tMargin : "0 0 0 0";
		const char *tStyles = child->Attribute("styles");
		const char *styles = (tStyles) ? tStyles : "";
		const char *caption = child->Attribute("caption");
		if (!caption) caption = child->GetText();
		const char *tTooltip = child->Attribute("tooltip");
		const char *tooltip = (tTooltip) ? tTooltip : "";
		const char *tCascade = child->Attribute("cascade");
		const char *cascade = (tCascade) ? tCascade : "";
		const char *tColour = child->Attribute("colour");
		const char *colour = (tColour) ? tColour : "0";
		const char *tbgColour = child->Attribute("bgcolour");
		const char *bgcolour = (tbgColour) ? tbgColour : "0";
		const char *tIcon = child->Attribute("icon");
		const char *icon = (tIcon) ? tIcon : "0";
		const char *tFlags = child->Attribute("flags");
		const char *tSrc = child->Attribute("src");
		const char *src = (tSrc) ? tSrc : "";
		const char *tCells = child->Attribute("cells");
		const char *cells = (tCells) ? tCells : "-1";

		//rebar specific attributes
		const char *tRebarMinHeight = child->Attribute("minheight");
		const char *rebarMinHeight = (tRebarMinHeight) ? tRebarMinHeight : "0";
		const char *tRebarMinWidth = child->Attribute("minwidth");
		const char *rebarMinWidth = (tRebarMinWidth) ? tRebarMinWidth : "0";

		//toolbar specific
		const char *tIconSize = child->Attribute("iconsize");
		const char *iconsize = (tIconSize) ? tIconSize : "16";

		//STEP 1: ADD CONTROL TO DIALOG
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
			if (!tParentId) tParentId = passedid.to_chr();
			const char *parentid = tParentId;

			//STEP 1: Creating CONTROL and ITEM elements
			if (0==lstrcmp(parentelem, "dialog")) { 
				cmd.sprintf("//xdialog -c %s %s %s 0 0 %s %s %s",dname,id,type,width,height,styles);
				mIRCcom(cmd.to_chr());
			}
			else if (0==lstrcmp(parentelem, "control")) { 

				if (0==lstrcmp(parenttype, "panel"))
					cmd.sprintf("//xdid -c %s %s %s %s 0 0 %s %s %s", dname,parentid,id,type,width,height,styles);

				else if (0==lstrcmp(parenttype, "tab"))
					cmd.sprintf("//xdid -a %s %s 0 0 %s $chr(9) %s %s 0 0 %s %s %s $chr(9) %s",
						dname,parentid,caption,id,type,width,height,styles,tooltip);

				else if (((0==lstrcmp(parenttype, "pager")) || (0==lstrcmp(parenttype, "box"))) && (control == 1))
					cmd.sprintf("//xdid -c %s %s %s %s 0 0 %s %s %s", dname,parentid,id,type,width,height,styles);

				else if (0==lstrcmp(parenttype, "divider") && (control <= 2)) {
					if (control > 0) {
						if (control == 1) { 
							cmd.sprintf("//xdid -l %s %s 10 0 $chr(9) %s %s 0 0 %s %s %s", dname,parentid,id,type,width,height,styles);
						}
						else if (control == 2) { 
							cmd.sprintf("//xdid -r %s %s 10 0 $chr(9) %s %s 0 0 %s %s %s", dname,parentid,id,type,width,height,styles);
						}
					}
				}

				else if (0==lstrcmp(parenttype, "rebar")) { 
					const char *flags = (tFlags) ? tFlags : "ceg";
					cmd.sprintf("//xdid -a %s %s 0 +%s %s %s %s %s %s %s $chr(9) %s %s 0 0 %s %s %s $chr(9) %s",
						dname,parentid,flags,rebarMinWidth,rebarMinHeight,width,icon,colour,caption,
						id,type,width,height,styles,tooltip);
				}
				else if (0==lstrcmp(parenttype, "stacker")) { 
					cmd.sprintf("//xdid -a %s %s 0 + %s %s %s \t %s %s 0 0 %s %s %s",
						dname,parentid,colour,bgcolour,caption,id,type,width,height,styles);
				}
				
				else if (0==lstrcmp(parenttype, "statusbar"))
					cmd.sprintf("//xdid -t %s %s %i +c %s %s %s 0 0 0 0 %s",
						dname,parentid,cell,icon,id,type,styles);

				//xdid -a dname parentid 0 + colour bgcolour caption \t id typ 0 0 width height styles
				mIRCcom(cmd.to_chr());
			}
			//Perform some control specific commands
			if ((0==lstrcmp(type, "toolbar")) || (0==lstrcmp(type, "button"))) { 
				cmd.sprintf("//xdid -l %s %s %s",dname,id,iconsize);
				mIRCcom(cmd.to_chr());
				parseItems(child,dname,id,0,"");
			}
			else if (0==lstrcmp(type, "treeview")) parseItems(child,dname,id,0,"");
			else if (0==lstrcmp(type, "comboex")) parseItems(child,dname,id,0,"");
			else if (0==lstrcmp(type, "list")) parseItems(child,dname,id,0,"");
			else if ((((0==lstrcmp(type, "box")) || (0==lstrcmp(type, "check")))
				|| (0==lstrcmp(type, "link"))) || (0==lstrcmp(type, "radio")))
				 { 
					 if (caption) { 
						 cmd.sprintf("//xdid -t %s %s %s",dname,id,caption);
						 mIRCcom(cmd.to_chr());
					 }
			}
			else if (0==lstrcmp(type, "text")) { 
				if (caption) { 
					TString mystring(caption);
					if (mystring.left(2) == "\r\n") mystring = mystring.right(-2);
					else if (mystring.left(1) == "\n") mystring = mystring.right(-1);
					mystring.replace("\t","");
					TString printstring;
					int textspace = 0;
					while(mystring.gettok(1," ") != "") { 
						printstring.addtok(mystring.gettok(1," ").to_chr());
						if (printstring.len() > 800) { 
							cmd.sprintf("//xdid -a %s %s %i %s",dname,id,textspace,printstring.gettok(1,-1).to_chr());
							mIRCcom(cmd.to_chr());
							printstring = "";
							textspace = 1;
						}
						mystring.deltok(1," ");
					}
					if (printstring != "") { 
						cmd.sprintf("//xdid -a %s %s %i %s",dname,id,textspace,printstring.gettok(1,-1).to_chr());
						mIRCcom(cmd.to_chr());
					}
				}
			}
			else if (0==lstrcmp(type, "edit")) { 
				if (caption) { 
					TString mystring(caption);
					if (mystring.left(2) == "\r\n") mystring = mystring.right(-2);
					else if (mystring.left(1) == "\n") mystring = mystring.right(-1);
					mystring.replace("\t","");
					mystring.replace("\n","\r\n");
					TString printstring;
					while(mystring.gettok(1," ") != "") { 
						printstring.addtok(mystring.gettok(1," ").to_chr());
						if (printstring.len() > 800) { 
							cmd.sprintf("//xdid -a %s %s %s",dname,id,printstring.gettok(1,-1).to_chr());
							mIRCcom(cmd.to_chr());
							printstring = "";
						}
						mystring.deltok(1," ");
					}
					if (printstring != "") { 
						cmd.sprintf("//xdid -a %s %s %s",dname,id,printstring.gettok(1,-1).to_chr());
						mIRCcom(cmd.to_chr());
					}
				}
			}
			else if (0==lstrcmp(type, "pbar")) { 
				if (caption) {
					cmd.sprintf("//xdid -i %s %s %s",dname,id,caption);
					mIRCcom(cmd.to_chr());
				}
			}
			else if (0==lstrcmp(type, "image")) { 
				const char *flags = (tFlags) ? tFlags : "";
				cmd.sprintf("//xdid -i %s %s +%s %s",dname,id,flags,src);
				mIRCcom(cmd.to_chr());
			}
			else if (0==lstrcmp(type, "statusbar")) { 
				cmd.sprintf("//xdid -l %s %s %s",dname,id,cells);
				mIRCcom(cmd.to_chr());
				parseItems(child,dname,id,0,"");
			}

			//STEP 2: LOOK FOR ICONS
			if (step2) parseIcons(root,dname,type,id);

			//STEP 3: APPLY CLA FOR CONTROL
			if (step3) { 
				cmd = "";
				if ((0==lstrcmp(type, "panel")) || (0==lstrcmp(type, "box"))) {
					cmd.sprintf("//xdid -l %s %s root $chr(9) +p%s 0 0 0 0", dname,id,cascade);
					mIRCcom(cmd.to_chr());
					cmd.sprintf("//xdid -l %s %s space root $chr(9) + %s", dname,id,margin);
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
					if ((parent->Attribute("type")) && (parentid)) {
						if (0==lstrcmp(parent->Attribute("type"), "panel"))
							cmd.sprintf("//xdid -l %s %s cell %s \t +%s%s%si %s %s %s %s",
								dname,parentid,claPath,fixed,fHeigth,fWidth,id,weigth,width,height); 
						else if (0==lstrcmp(parent->Attribute("type"), "box"))
							cmd.sprintf("//xdid -l %s %s cell %s \t +%s%s%si %s %s %s %s",
								dname,parentid,claPath,fixed,fHeigth,fWidth,id,weigth,width,height); 
						else cmd = "";
					}
				}
				mIRCcom(cmd.to_chr());
			}
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

			const char *tParentType = parent->Attribute("type");
			const char *parenttype = (tParentType) ? tParentType : "panel";
			const char *tParentId = parent->Attribute("id");
			if (!tParentId) tParentId = passedid.to_chr();
			const char *parentid = tParentId;

			if (0==lstrcmp(parentelem, "dialog"))
				cmd.sprintf("//xdialog -l %s cell %s \t +p%s 0 %s 0 0", dname,claPath,cascade,weigth);
			if (0==lstrcmp(parentelem, "control")) {
				if ((parenttype) && (parentid)) {
					if (0==lstrcmp(parenttype, "panel"))
						cmd.sprintf("//xdid -l %s %s cell %s \t +p%s 0 %s 0 0", dname,parentid,claPath,cascade,weigth);
					else if (0==lstrcmp(parenttype, "box"))
						cmd.sprintf("//xdid -l %s %s cell %s \t +p%s 0 %s 0 0", dname,parentid,claPath,cascade,weigth);
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
		if (goDeeper) parseDialog(root,child->ToElement(), dname, depth+1,claPathx,idpass);  
	}
} 
