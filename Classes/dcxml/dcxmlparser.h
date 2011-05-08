#ifndef DCXMLPARSER_H
#define DCXMLPARSER_H

#include "dcxmlincludes.h"

class DcxmlParser {

public:
	//constructor
	DcxmlParser();
	~DcxmlParser();

	bool ParseXML(const TString &tsFilePath, const TString &tsDialogMark,const TString &DialogName,const bool verbose, const bool autoClose);
	//virtual ~DCXML( );
	void parseAttributes();
	void parseAttributes(const TiXmlElement* tElement);
	void parseControl();
	TString parseCLA(const int cCla);
	bool loadSuccess;

	void setStyle(const TiXmlElement* style);
	void parseStyle(int depth = 0);
	void parseIcons(int depth = 0);
	void parseItems(const TiXmlElement* element,int depth = 0,char *itemPath = "");
	void parseTemplate(int dialogDepth=0,const char *claPath = "root",const int passedid = 2000);
	void parseDialog(int depth=0,const char *claPath = "root",const int passedid = 2000,const int ignoreParent = 0);

	const char *queryAttribute(const TiXmlElement *element,const char *attribute,const char *defaultValue = "") const;
	int queryIntAttribute(const TiXmlElement *element,const char *attribute,const int defaultValue = 0) const;

	void setDialog(const TString &dialogMark);
	void setDialogMark (const TString &v) { this->_dialogMark = v; }
	void setDialogName (const TString &v) { this->_dialogName = v; }
	void setRootElement (const TiXmlElement * element) { this->_rootElement = element; }
	void setDialogsElement (const TiXmlElement * element) { this->_dialogsElement = element; }
	void setDialogElement (const TiXmlElement * element) { this->_dialogElement = element; }

	bool loadDocument();
	bool loadDialog();
	bool loadDialogElement();

	void setZlayered (const bool b) { this->_zlayered = b; }

	DcxDialog *getDialog () const { return this->_dcxDialog; }
	bool getZlayered () const { return this->_zlayered; }
	const TString &getDialogMark () const { return this->_dialogMark; }
	const TString &getDialogName () const { return this->_dialogName; }
	const TString &getFilePath () const { return this->_filePath; }

	const TiXmlElement* getRootElement () const { return this->_rootElement; }
	const TiXmlElement* getDialogsElement () const { return this->_dialogsElement; }
	const TiXmlElement* getDialogElement () const { return this->_dialogElement; }

	const TiXmlDocument * getDocument () const { return &this->_document; }

	DcxDialog *d_Host;
	const TiXmlElement *root; //!< dcxml root element
	const TiXmlElement *dialogs; //!< dcxml dialogs collection element
	const TiXmlElement *dialog; //!< dialog element
	const TiXmlElement *element; //!< current Element
	const TiXmlElement *parent; //!< current Element's parent
	TString dname;
	int controls; //!< Simple counter for controls
	int zlayered; //!< Simple check if dialog has zlayers
	//Attribute vars
	int id;
	int parentid;
	const char *elem;
	const char *parentelem;
	const char *parenttype;
	const char *type;
	const char *STclass;
	const char *weigth;
	const char *height;
	const char *dropdown;
	const char *width;
	const char *margin;
	const char *styles;
	const char *caption;
	const char *tooltip;
	const char *cascade;
	const char *icon;
	const char *tFlags;
	const char *integral;
	const char *state;
	const char *indent;
	const char *src;
	const char *cells;
	const char *rebarMinHeight;
	const char *rebarMinWidth;
	const char *iconsize;
	const char *fontstyle;
	const char *charset;
	const char *fontsize;
	const char *fontname;
	const char *border;
	const char *cursor;
	const char *bgcolour;
	const char *textbgcolour;
	const char *textcolour;
	const char *gradientstart;
	const char *gradientend;
	const char *disabledsrc;
	const char *hoversrc;
	const char *selectedsrc;
	/*
	const char *elementProperties [] = "type","STclass","weigth","height","width","margin","styles","caption","tooltip",
	"cascade","icon","tFlags""integral","state","indent","src",	"cells","rebarMinHeight",
	"rebarMinWidth","iconsize","fontstyle","charset","fontsize","fontname","border","cursor","bgcolour",
	"textbgcolour","textcolour","gradientstart","gradientend","disabledsrc","hoversrc","selectedsrc"
	];*/
	const TiXmlElement* templateRef;
	int templateRefcCla;
	const char *templateRefclaPath;
	std::map<const char*, const char*> template_vars;

	int eval;

	//tempvar to dump attribute values in;
	const char *temp;
	TString cmd;

	//CLA variables
	const char *g_claPath;
	const char *g_claPathx;
	int g_resetcla;
private:
	bool _verbose;
	void isVerbose(const bool b) { this->_verbose = b; }
	bool isVerbose() const { return this->_verbose; }
	bool _autoClose;
	void isAutoClose(const bool b) { this->_autoClose = b; }
	bool isAutoClose() const { return this->_autoClose; }

	void xdialogEX(const TCHAR *sw,const TCHAR *dFormat, ...);
	int mIRCEvalToUnsignedInt (const char *value);
	void registerId(const TiXmlElement *idElement,const int id);
	int parseId(const TiXmlElement* idElement);
	void xdidEX(const int id,const TCHAR *sw,const TCHAR *dFormat, ...);

	//TiXmlElement *getDialogElement () { return this->_dialogElement; }
	typedef std::map<const char, const char> AttributesMap;

	AttributesMap _attributesMap;

	void setFilePath (const TString &tsFile) { this->_filePath = tsFile; }

	bool _zlayered;
	TString _filePath;
	TString _dialogMark;
	TString _dialogName;
	DcxDialog* _dcxDialog;
	const TiXmlElement *_rootElement;
	const TiXmlElement *_dialogsElement;
	const TiXmlElement *_dialogElement;
	TiXmlDocument _document;
};
#endif
