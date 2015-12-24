#ifndef DCXMLPARSER_H
#define DCXMLPARSER_H

#include "dcxmlincludes.h"

class DcxmlParser {

public:
	//constructor
	DcxmlParser();
	~DcxmlParser();

	DcxmlParser(const DcxmlParser &) = delete;
	DcxmlParser &operator =(const DcxmlParser &) = delete;	// No assignments!

	bool ParseXML(const TString &tsFilePath, const TString &tsDialogMark,const TString &DialogName,const bool verbose, const bool autoClose);
	//virtual ~DCXML( );
	void parseAttributes();
	void parseAttributes(const TiXmlElement *const tElement);
	void parseControl();
	TString parseCLA(const int cCla);

	void setStyle(const TiXmlElement *const style);
	void parseStyle(int depth = 0);
	void parseIcons(int depth = 0);
	void parseItems(const TiXmlElement *const element, const UINT depth = 0, const char *const itemPath = "");
	void parseTemplate(const UINT dialogDepth=0, const char *const claPath = "root", const UINT passedid = 2000);
	void parseDialog(const UINT depth=0,const char *claPath = "root",const UINT passedid = 2000,const bool ignoreParent = false);

	//static const char *queryAttribute(const TiXmlElement *element,const char *attribute,const char *defaultValue = "");
	//static int queryIntAttribute(const TiXmlElement *element,const char *attribute,const int defaultValue = 0);

	void setDialog(const TString &dialogMark);
	void setDialogMark (const TString &v) { this->_dialogMark = v; }
	void setDialogName (const TString &v) { this->_dialogName = v; }
	void setRootElement (const TiXmlElement *const ti) noexcept { this->_rootElement = ti; }
	void setDialogsElement (const TiXmlElement *const ti) noexcept { this->_dialogsElement = ti; }
	void setDialogElement (const TiXmlElement *const ti) noexcept { this->_dialogElement = ti; }

	void loadDocument();
	void loadDialog();
	void loadDialogElement();

	void setZlayered (const bool b) noexcept { this->_zlayered = b; }

	DcxDialog *getDialog () const noexcept { return this->_dcxDialog; }
	const bool &getZlayered() const noexcept { return this->_zlayered; }
	const TString &getDialogMark () const noexcept { return this->_dialogMark; }
	const TString &getDialogName () const noexcept { return this->_dialogName; }
	const TString &getFilePath () const noexcept { return this->_filePath; }

	const TiXmlElement* getRootElement () const noexcept { return this->_rootElement; }
	const TiXmlElement* getDialogsElement () const noexcept { return this->_dialogsElement; }
	const TiXmlElement* getDialogElement () const noexcept { return this->_dialogElement; }

	const TiXmlDocument * getDocument () const noexcept { return &this->_document; }

	const bool &getLoaded() const noexcept { return loadSuccess; }

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
	const char *weight;
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

	void isVerbose(const bool b) noexcept { this->_verbose = b; }
	const bool &isVerbose() const noexcept { return this->_verbose; }
	void isAutoClose(const bool b) noexcept { this->_autoClose = b; }
	const bool &isAutoClose() const noexcept { return this->_autoClose; }

	void xdialogEX(const TCHAR *const sw,const TCHAR *const dFormat, ...);
	int mIRCEvalToUnsignedInt (const char *const value);
	void registerId(const TiXmlElement *const idElement,const int iNewID);
	int parseId(const TiXmlElement *const idElement);
	void xdidEX(const int cid,const TCHAR *const sw,const TCHAR *const dFormat, ...);

	//TiXmlElement *getDialogElement () { return this->_dialogElement; }
	typedef std::map<const char, const char> AttributesMap;

	AttributesMap _attributesMap;

	void setFilePath (const TString &tsFile) { this->_filePath = tsFile; }

	bool loadSuccess;
	bool _verbose;
	bool _autoClose;
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
