/*!
 * \file dcxrebar.cpp
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2006
 */
#include "defines.h"
#include "Classes/dcxrebar.h"
#include "Classes/dcxdialog.h"
#include "Dcx.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxReBar::DcxReBar( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles )
: DcxControl( ID, p_Dialog ) 
, m_iClickedBand(-1)
, m_iRowLimit(0)
, m_iWidth(0)
, m_iHeight(0)
{
	const auto[bNoTheme, Styles, ExStyles] = parseControlStyles(styles);

	m_Hwnd = dcxCreateWindow(
		ExStyles | WS_EX_CONTROLPARENT,
		DCX_REBARCTRLCLASS,
		Styles | WS_CHILD,
		rc,
		mParentHwnd,
		ID);

	if (!IsWindow(m_Hwnd))
		throw Dcx::dcxException("Unable To Create Window");

	if ( bNoTheme )
	{
		//SendMessage( m_Hwnd, RB_SETWINDOWTHEME, NULL, L" ");
		Dcx::UXModule.dcxSetWindowTheme( m_Hwnd , L" ", L" " );
	}
	{
		this->setImageList( this->createImageList() );
		//REBARINFO rbi;
		//rbi.cbSize = sizeof(REBARINFO);
		//rbi.fMask = 0;
		//rbi.himl = nullptr;
		//this->setBarInfo( &rbi );
	}
	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );
}

/*!
 * \brief Destructor
 *
 * blah
 */

DcxReBar::~DcxReBar( )
{
  this->resetContents( );

  ImageList_Destroy( this->getImageList( ) );

  this->unregistreDefaultWindowProc( );
}

const TString DcxReBar::getStyles(void) const
{
	auto styles(__super::getStyles());
	const auto Styles = dcxGetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles, RBS_BANDBORDERS))
		styles.addtok(TEXT("borders"));
	if (dcx_testflag(Styles, RBS_DBLCLKTOGGLE))
		styles.addtok(TEXT("dblclktoggle"));
	if (dcx_testflag(Styles, RBS_FIXEDORDER))
		styles.addtok(TEXT("fixedorder"));
	if (dcx_testflag(Styles, RBS_VARHEIGHT))
		styles.addtok(TEXT("varheight"));
	if (dcx_testflag(Styles, RBS_TOOLTIPS))
		styles.addtok(TEXT("tooltips"));
	if (dcx_testflag(Styles, RBS_VERTICALGRIPPER))
		styles.addtok(TEXT("verticalgrip"));
	if (dcx_testflag(Styles, CCS_VERT))
		styles.addtok(TEXT("vertical"));
	if (dcx_testflag(Styles, CCS_RIGHT))
		styles.addtok(TEXT("right"));
	if (dcx_testflag(Styles, CCS_BOTTOM))
		styles.addtok(TEXT("bottom"));
	if (dcx_testflag(Styles, CCS_NORESIZE))
		styles.addtok(TEXT("noresize"));
	if (dcx_testflag(Styles, CCS_NOPARENTALIGN))
		styles.addtok(TEXT("noparentalign"));
	if (dcx_testflag(Styles, (CCS_NOPARENTALIGN | CCS_NORESIZE)))
		styles.addtok(TEXT("noauto"));

	return styles;
}


void DcxReBar::toXml(TiXmlElement *const xml) const
{
	__super::toXml(xml);

	xml->SetAttribute("styles", getStyles().c_str());

	if (const auto count = this->getBandCount(); count > 0)
	{
		for (auto i = decltype(count){0}; i < count; ++i)
		{
			if (const auto c = this->getControl(i); c != nullptr)
			{
				auto subs = new TiXmlElement("control");
				c->toXml(subs);
				xml->LinkEndChild(subs);
			}
		}
	}
}

TiXmlElement * DcxReBar::toXml(void) const
{
	auto xml = std::make_unique<TiXmlElement>("control");
	toXml(xml.get());
	return xml.release();
}

DcxControl * DcxReBar::getControl(const int index) const
{
	if (index > -1 && index < this->getBandCount())
	{
		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_CHILD;

		if (this->getBandInfo(gsl::narrow_cast<UINT>(index), &rbBand) != 0)
			return this->m_pParentDialog->getControlByHWND(rbBand.hwndChild);

	}
	return nullptr;
}

/*!
 * \brief blah
 *
 * blah
 */

//void DcxReBar::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
//{
//	*Styles |= RBS_AUTOSIZE;
//
//	//*ExStyles |= WS_EX_CONTROLPARENT;
//
//	for (const auto &tsStyle: styles)
//	{
//#if DCX_USE_HASHING
//		switch (std::hash<TString>{}(tsStyle))
//		{
//			case L"borders"_hash:
//				*Styles |= RBS_BANDBORDERS;
//				break;
//			case L"dblclktoggle"_hash:
//				*Styles |= RBS_DBLCLKTOGGLE;
//				break;
//			case L"fixedorder"_hash:
//				*Styles |= RBS_FIXEDORDER;
//				break;
//			case L"varheight"_hash:
//				*Styles |= RBS_VARHEIGHT;
//				break;
//			case L"tooltips"_hash:
//				*Styles |= RBS_TOOLTIPS;
//				break;
//			case L"verticalgrip"_hash:
//				*Styles |= RBS_VERTICALGRIPPER;
//				break;
//			case L"vertical"_hash:
//				*Styles |= CCS_VERT;
//				break;
//			case L"right"_hash:
//				*Styles |= CCS_RIGHT;
//				break;
//			case L"bottom"_hash:
//				*Styles |= CCS_BOTTOM;
//				break;
//			case L"noresize"_hash:
//				*Styles |= CCS_NORESIZE;
//				break;
//			case L"noparentalign"_hash:
//				*Styles |= CCS_NOPARENTALIGN;
//				break;
//			case L"noauto"_hash:
//				*Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
//			default:
//				break;
//		}
//#else
//		if ( tsStyle == TEXT("borders") )
//			*Styles |= RBS_BANDBORDERS;
//		else if ( tsStyle == TEXT("dblclktoggle") ) 
//			*Styles |= RBS_DBLCLKTOGGLE;
//		else if ( tsStyle == TEXT("fixedorder") ) 
//			*Styles |= RBS_FIXEDORDER;
//		else if ( tsStyle == TEXT("varheight") ) 
//			*Styles |= RBS_VARHEIGHT;
//		else if ( tsStyle == TEXT("tooltips") ) 
//			*Styles |= RBS_TOOLTIPS;
//		else if ( tsStyle == TEXT("verticalgrip") ) 
//			*Styles |= RBS_VERTICALGRIPPER;
//		else if ( tsStyle == TEXT("vertical") ) 
//			*Styles |= CCS_VERT;
//		else if ( tsStyle == TEXT("right") ) 
//			*Styles |= CCS_RIGHT;
//		else if ( tsStyle == TEXT("bottom") ) 
//			*Styles |= CCS_BOTTOM;
//		else if ( tsStyle == TEXT("noresize") ) 
//			*Styles |= CCS_NORESIZE;
//		else if ( tsStyle == TEXT("noparentalign") ) 
//			*Styles |= CCS_NOPARENTALIGN ;
//		else if ( tsStyle == TEXT("noauto") )
//			*Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
//#endif
//	}
//
//	this->parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
//}

std::tuple<NoTheme, WindowStyle, WindowExStyle> DcxReBar::parseControlStyles(const TString & tsStyles)
{
	auto[bNoTheme, Styles, ExStyles] = parseGeneralControlStyles(tsStyles);

	Styles |= RBS_AUTOSIZE;

	//ExStyles |= WS_EX_CONTROLPARENT;

	for (const auto &tsStyle : tsStyles)
	{
		switch (std::hash<TString>{}(tsStyle))
		{
		case L"borders"_hash:
			Styles |= RBS_BANDBORDERS;
			break;
		case L"dblclktoggle"_hash:
			Styles |= RBS_DBLCLKTOGGLE;
			break;
		case L"fixedorder"_hash:
			Styles |= RBS_FIXEDORDER;
			break;
		case L"varheight"_hash:
			Styles |= RBS_VARHEIGHT;
			break;
		case L"tooltips"_hash:
			Styles |= RBS_TOOLTIPS;
			break;
		case L"verticalgrip"_hash:
			Styles |= RBS_VERTICALGRIPPER;
			break;
		case L"vertical"_hash:
			Styles |= CCS_VERT;
			break;
		case L"right"_hash:
			Styles |= CCS_RIGHT;
			break;
		case L"bottom"_hash:
			Styles |= CCS_BOTTOM;
			break;
		case L"noresize"_hash:
			Styles |= CCS_NORESIZE;
			break;
		case L"noparentalign"_hash:
			Styles |= CCS_NOPARENTALIGN;
			break;
		case L"noauto"_hash:
			Styles |= CCS_NOPARENTALIGN | CCS_NORESIZE;
		default:
			break;
		}
	}

	return { bNoTheme, Styles, ExStyles };
}
/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxReBar::getImageList() const
{
	REBARINFO ri{};
	ri.cbSize = sizeof(REBARINFO);
	ri.fMask = RBIM_IMAGELIST;

	getBarInfo(&ri);

	return ri.himl;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxReBar::setImageList(HIMAGELIST himl)
{
	REBARINFO ri{};
	ri.cbSize = sizeof(REBARINFO);
	ri.himl = himl;
	ri.fMask = RBIM_IMAGELIST;
	
	setBarInfo(&ri);
}

/*!
 * \brief blah
 *
 * blah
 */

//HIMAGELIST DcxReBar::createImageList() {
//	return ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 0);
//}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxReBar::parseInfoRequest( const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	const auto numtok = input.numtok();

	switch (std::hash<TString>{}(input.getfirsttok(3)))
	{
	case L"num"_hash:
		_ts_snprintf(szReturnValue, TEXT("%d"), this->getBandCount());
		break;
		// [NAME] [ID] [PROP] [N]
	case L"text"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw Dcx::dcxException("Invalid Index");

		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_TEXT;
		rbBand.cch = MIRC_BUFFER_SIZE_CCH;
		rbBand.lpText = szReturnValue;

		getBandInfo(gsl::narrow_cast<UINT>(nIndex), &rbBand);
	}
	break;
	case L"childid"_hash:
	{
		if (numtok < 4)
			throw Dcx::dcxException("Invalid number of arguments");

		const auto n = input.getnexttok().to_int() - 1; // tok 4

		if (n < 0 || n >= this->getBandCount())
			throw Dcx::dcxException("Invalid Index");

		if (const auto c = this->getControl(n); c != nullptr)
			_ts_snprintf(szReturnValue, TEXT("%u"), c->getUserID());
	}
	break;
	// $xdid([NAME], [ID], [N]).[PROP]
	case L"markeditem"_hash:
	{
		const auto n = input.getnexttok().to_int() - 1;	// tok 4

		if (n < 0 || n >= this->getBandCount())
			throw Dcx::dcxException("Invalid Index");

		REBARBANDINFO rbi{};
		rbi.cbSize = sizeof(REBARBANDINFO);
		rbi.fMask = RBBIM_LPARAM;

		getBandInfo(gsl::narrow_cast<UINT>(n), &rbi);

		auto pdcxrbb = reinterpret_cast<LPDCXRBBAND>(rbi.lParam);

		szReturnValue = pdcxrbb->tsMarkText.to_chr();
	}
	break;
	default:
		parseGlobalInfoRequest(input, szReturnValue);
	}
}
/*!
 * \brief blah
 *
 * \param input [NAME] [SWITCH] [ID] (OPTIONS)
 */

void DcxReBar::parseCommandRequest( const TString & input )
{
	const XSwitchFlags flags(input.getfirsttok(3));
	const auto numtok = input.numtok();

	// xdid -a [NAME] [ID] [SWITCH] [N] [+FLAGS] [CX] [CY] [WIDTH] [ICON] [COLOR] [Item Text][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)[TAB]Tooltip
	if ( flags[TEXT('a')] && numtok > 9 )
	{
		REBARBANDINFO rbBand{};
		if (Dcx::VistaModule.isUseable()) // NB: when rbBand.cbSize is set to the Vista size on XP the insertband will FAIL!! fucking MS!
			rbBand.cbSize = sizeof( REBARBANDINFO );
		else
			rbBand.cbSize = REBARBANDINFO_V6_SIZE;

		rbBand.fMask = RBBIM_STYLE | RBBIM_LPARAM;

		const auto data(input.getfirsttok(1, TSTABCHAR).trim());
		TString control_data, tooltip;
		const auto nToks = input.numtok(TSTABCHAR);

		if (nToks > 1)
		{
			control_data = input.getnexttok( TSTABCHAR).trim();		// tok 2

			if (nToks > 2)
				tooltip = input.getnexttok( TSTABCHAR).trim();		// tok 3
		}
		auto nIndex = data.getfirsttok(4).to_<int>() - 1;
		rbBand.fStyle = parseBandStyleFlags(data.getnexttok());	// tok 5
		const auto cx = data.getnexttok().to_<UINT>();					// tok 6
		const auto cy = data.getnexttok().to_<UINT>();					// tok 7
		const auto width = data.getnexttok().to_<UINT>();				// tok 8
		const auto nIcon = data.getnexttok().to_<int>() - 1;			// tok 9
		const auto clrText = data.getnexttok().to_<COLORREF>();			// tok 10

		if (nIndex < -1)
			nIndex = -1;

		rbBand.cxMinChild = cx;
		rbBand.cyMinChild = cy;
		rbBand.cx = width;
		rbBand.cyIntegral = 1;
		rbBand.cyChild = cy;

		TString itemtext;
		if ( data.numtok( ) > 10 )
		{
			itemtext = data.getlasttoks().trim();	// tok 11, -1
			rbBand.fMask |= RBBIM_TEXT;
			rbBand.lpText = itemtext.to_chr( );
			//rbBand.cch = itemtext.len();
		}

		// Tooltip Handling
		auto lpdcxrbb = std::make_unique<DCXRBBAND>();

		lpdcxrbb->bUline = dcx_testflag(rbBand.fStyle, RBBS_UNDERLINE);

		lpdcxrbb->bBold = dcx_testflag(rbBand.fStyle, RBBS_BOLD);

		if (dcx_testflag(rbBand.fStyle, RBBS_COLOR))
			lpdcxrbb->clrText = clrText;
		else
			lpdcxrbb->clrText = CLR_INVALID;

		if ( nIcon > -1 )
		{
			rbBand.iImage = nIcon;
			rbBand.fMask |= RBBIM_IMAGE;
		}

		rbBand.lParam = (LPARAM)lpdcxrbb.get();

		DcxControl * p_Control = nullptr;
		if ( control_data.numtok( ) > 5 )
		{
			p_Control = this->m_pParentDialog->addControl(control_data, 1,
				CTLF_ALLOW_TRACKBAR |
				CTLF_ALLOW_PBAR |
				CTLF_ALLOW_COMBOEX |
				CTLF_ALLOW_TOOLBAR |
				CTLF_ALLOW_STATUSBAR |
				CTLF_ALLOW_TREEVIEW |
				CTLF_ALLOW_LISTVIEW |
				CTLF_ALLOW_COLORCOMBO |
				CTLF_ALLOW_BUTTON |
				CTLF_ALLOW_RICHEDIT |
				CTLF_ALLOW_DIVIDER |
				CTLF_ALLOW_PANEL |
				CTLF_ALLOW_TAB
				, m_Hwnd);

			if (auto dct = p_Control->getControlType(); ((dct == DcxControlTypes::STATUSBAR) || (dct == DcxControlTypes::TOOLBAR)))
				p_Control->addStyle(WindowStyle::CCS_NoParentAlign | CCS_NORESIZE);

			rbBand.fMask |= RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_ID;
			rbBand.hwndChild = p_Control->getHwnd( );
			rbBand.wID = p_Control->getID();

			//const auto ID = mIRC_ID_OFFSET + (UINT)control_data.gettok(1).to_int();
			//
			//if (this->m_pParentDialog->isIDValid(ID, true))
			//	throw Dcx::dcxException(TEXT("Control with ID \"%\" already exists"), ID - mIRC_ID_OFFSET);
			//
			//try {
			//	p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,control_data,2,
			//		CTLF_ALLOW_TRACKBAR |
			//		CTLF_ALLOW_PBAR |
			//		CTLF_ALLOW_COMBOEX |
			//		CTLF_ALLOW_TOOLBAR |
			//		CTLF_ALLOW_STATUSBAR |
			//		CTLF_ALLOW_TREEVIEW |
			//		CTLF_ALLOW_LISTVIEW |
			//		CTLF_ALLOW_COLORCOMBO |
			//		CTLF_ALLOW_BUTTON |
			//		CTLF_ALLOW_RICHEDIT |
			//		CTLF_ALLOW_DIVIDER |
			//		CTLF_ALLOW_PANEL |
			//		CTLF_ALLOW_TAB
			//		,m_Hwnd);
			//
			//	//if ((p_Control->getType() == TEXT("statusbar")) || (p_Control->getType() == TEXT("toolbar")))
			//	//	p_Control->addStyle( CCS_NOPARENTALIGN | CCS_NORESIZE );
			//
			//	auto dct = p_Control->getControlType();
			//
			//	if ((dct == DcxControlTypes::STATUSBAR) || (dct == DcxControlTypes::TOOLBAR))
			//		p_Control->addStyle(CCS_NOPARENTALIGN | CCS_NORESIZE);
			//
			//	this->m_pParentDialog->addControl( p_Control );
			//
			//	rbBand.fMask |= RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_ID;
			//	rbBand.hwndChild = p_Control->getHwnd( );
			//	//rbBand.cxMinChild = cx;
			//	//rbBand.cyMinChild = cy;
			//	//rbBand.cx = width;
			//	//rbBand.cyIntegral = 1;
			//	//rbBand.cyChild = cy;
			//	rbBand.wID = ID;
			//}
			//catch (const std::exception &e ) {
			//	showErrorEx(nullptr, TEXT("-a"), TEXT("Unable To Create Control: %d (%S)"), ID - mIRC_ID_OFFSET, e.what() );
			//	throw;
			//}
		}

		if (this->insertBand(nIndex, &rbBand) == 0L)
		{ // 0L means failed.
			this->m_pParentDialog->deleteControl(p_Control);
			if (rbBand.hwndChild != nullptr)
				DestroyWindow(rbBand.hwndChild);
	
			throw Dcx::dcxException("Unable To Add Band");
		}
		lpdcxrbb.release();
	}
	// xdid -A [NAME] [ID] [SWITCH] [N] (TEXT)
	else if (flags[TEXT('A')] && numtok > 3)
	{
		const auto n = input.getnexttok( ).to_int() - 1;	// tok 4

		if (n < 0 || n >= getBandCount())
			throw Dcx::dcxException("Invalid Index");

		REBARBANDINFO rbi{};
		rbi.cbSize = sizeof(REBARBANDINFO);
		rbi.fMask = RBBIM_LPARAM;

		this->getBandInfo(gsl::narrow_cast<UINT>(n), &rbi);
		auto pdcxrbb = reinterpret_cast<LPDCXRBBAND>(rbi.lParam);
		pdcxrbb->tsMarkText = (numtok > 4 ? input.getlasttoks() : TEXT(""));	// tok 5, -1
	}
	// xdid -d [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('d')] && numtok > 3 )
	{
		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw Dcx::dcxException("Invalid Index");

		this->deleteBand(gsl::narrow_cast<UINT>(nIndex) );
	}
	// xdid -i [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('i')] && numtok > 3 )
	{
		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw Dcx::dcxException("Invalid Index");

		this->showBand(gsl::narrow_cast<UINT>(nIndex), FALSE );
	}
	// xdid -j [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('j')] && numtok > 3 )
	{
		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw Dcx::dcxException("Invalid Index");

		this->showBand(gsl::narrow_cast<UINT>(nIndex), TRUE );
	}
	// xdid -k [NAME] [ID] [SWITCH] [N] [ICON]
	else if ( flags[TEXT('k')] && numtok > 4 )
	{
		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof( REBARBANDINFO );
		rbBand.fMask = RBBIM_IMAGE;

		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4
		const auto nIcon = input.getnexttok().to_int() - 1;	// tok 5

		if (nIndex < 0 || nIcon < -1 || nIndex >= this->getBandCount())
			throw Dcx::dcxException("Invalid Index");

		// Ook: TODO add check for nIcon being valid
		rbBand.iImage = nIcon;
		this->setBandInfo( nIndex, &rbBand );
	}
	// xdid -l -> [NAME] [ID] -l [N|ALL]
	else if ( flags[TEXT('l')] && numtok > 3 )
	{
		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof( REBARBANDINFO );
		rbBand.fMask = RBBIM_STYLE;

		const auto nItems = this->getBandCount();
		const auto tsItem(input.getnexttok());	// tok 4

		if ( tsItem == TEXT("all") )
		{
			for (auto i = decltype(nItems){0}; i < nItems; ++i)
			{
				if ( this->getBandInfo( i, &rbBand ) != 0 )
				{
					rbBand.fStyle |= RBBS_NOGRIPPER;
					this->setBandInfo( i, &rbBand );
				}
			}
		}
		else {
			const auto nIndex = tsItem.to_int() - 1;

			if ( nIndex < 0 || nIndex >= nItems || this->getBandInfo( nIndex, &rbBand ) == 0 )
				throw Dcx::dcxException("Invalid Index");

			rbBand.fStyle |= RBBS_NOGRIPPER;
			this->setBandInfo( nIndex, &rbBand );
		}
	}
	// xdid -m [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('m')] && numtok > 3 )
	{
		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw Dcx::dcxException("Invalid Index");

		this->maxBand( nIndex, FALSE );
	}
	// xdid -n [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('n')] && numtok > 3 )
	{
		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4

		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw Dcx::dcxException("Invalid Index");

		this->minBand( nIndex, FALSE );
	}
	// xdid -q [NAME] [ID] [SWITCH] [N]
	else if ( flags[TEXT('q')] && numtok > 3 )
	{
		const auto nRows = input.getnexttok().to_int();	// tok 4

		if ( nRows > -1 )
			this->m_iRowLimit = nRows;
	}
	// xdid -t [NAME] [ID] [SWITCH] [N] [TEXT]
	else if ( flags[TEXT('t')] && numtok > 3 )
	{
		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof( REBARBANDINFO );
		rbBand.fMask = RBBIM_TEXT;

		const auto nIndex = input.getnexttok().to_int() - 1;	// tok 4
		if (nIndex < 0 || nIndex >= this->getBandCount())
			throw Dcx::dcxException("Invalid Index");

		TString itemtext;
		if ( numtok > 4 )
			itemtext = input.getlasttoks().trim();	// tok 5, -1

		rbBand.lpText = itemtext.to_chr( );
		this->setBandInfo( nIndex, &rbBand );
	}
	// xdid -u [NAME] [ID] [SWITCH] [N|ALL]
	else if ( flags[TEXT('u')] && numtok > 3 )
	{
		REBARBANDINFO rbBand{};
		rbBand.cbSize = sizeof( REBARBANDINFO );
		rbBand.fMask = RBBIM_STYLE;

		const auto nItems = this->getBandCount();
		const auto tsItem(input.getnexttok());	// tok 4

		if ( tsItem == TEXT("all") )
		{
			for (auto i = decltype(nItems){0}; i < nItems; ++i)
			{
				if ( this->getBandInfo(gsl::narrow_cast<UINT>(i), &rbBand ) != 0 )
				{
					rbBand.fStyle &= ~RBBS_NOGRIPPER;
					this->setBandInfo(gsl::narrow_cast<UINT>(i), &rbBand );
				}
			}
		}
		else {
			const auto nIndex = tsItem.to_int() - 1;

			if ( nIndex < 0 || nIndex >= nItems || this->getBandInfo(gsl::narrow_cast<UINT>(nIndex), &rbBand ) == 0 )
				throw Dcx::dcxException("Invalid Index");

			rbBand.fStyle &= ~RBBS_NOGRIPPER;
			this->setBandInfo( gsl::narrow_cast<UINT>(nIndex), &rbBand );
		}
	}
	// xdid -v [NAME] [ID] [SWITCH] [NFrom] [NTo]
	else if ( flags[TEXT('v')] && numtok > 4 )
	{
		const auto nIndexFrom = input.getnexttok().to_int() - 1;	// tok 4
		const auto nIndexTo = input.getnexttok().to_int() - 1;		// tok 4
		const auto nItems = this->getBandCount();

		if ( nIndexFrom < 0 || nIndexFrom >= nItems || nIndexTo < 0 || nIndexTo >= nItems )
			throw Dcx::dcxException("Invalid Index");

		this->moveBand( gsl::narrow_cast<UINT>(nIndexFrom), gsl::narrow_cast<UINT>(nIndexTo) );
	}
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')] && numtok > 5)
	{
		const auto flag(input.getnexttok());		// tok 4
		const auto index = input.getnexttok().to_int();	// tok 5
		auto filename(input.getlasttoks());			// tok 6, -1

		auto himl = this->getImageList();

		if (himl == nullptr)
		{
			himl = this->createImageList();

			if (himl != nullptr)
				this->setImageList(himl);
		}

		if (himl == nullptr)
			throw Dcx::dcxException("Unable to get imagelist");
#if DCX_USE_WRAPPERS
		Dcx::dcxIconResource icon(index, filename, false, flag);
#else
		if (HICON icon = dcxLoadIcon(index, filename, false, flag); icon != nullptr) {
			ImageList_AddIcon(himl, icon);
			DestroyIcon(icon);
		}
#endif

		ImageList_AddIcon(himl, icon);
	}
	// xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
	else if ( flags[TEXT('y')] )
	{
		ImageList_Destroy( this->getImageList( ) );
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxReBar::resetContents( )
{
	auto nItems = gsl::narrow_cast<UINT>(this->getBandCount());

	while ( nItems-- )
		this->deleteBand( nItems );
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxReBar::parseBandStyleFlags( const TString & flags )
{
	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if ( xflags[TEXT('b')] )
		iFlags |= RBBS_BOLD;
	if ( xflags[TEXT('c')] )
		iFlags |= RBBS_COLOR;
	if ( xflags[TEXT('e')] )
		iFlags |= RBBS_CHILDEDGE;
	if ( xflags[TEXT('f')] )
		iFlags |= RBBS_FIXEDSIZE;
	if ( xflags[TEXT('g')] )
		iFlags |= RBBS_GRIPPERALWAYS;
	if ( xflags[TEXT('h')] )
		iFlags |= RBBS_HIDDEN;
	if ( xflags[TEXT('n')] )
		iFlags |= RBBS_NOGRIPPER;
	if ( xflags[TEXT('o')] )
		iFlags |= RBBS_USECHEVRON;
	if ( xflags[TEXT('u')] )
		iFlags |= RBBS_UNDERLINE;
	if ( xflags[TEXT('v')] )
		iFlags |= RBBS_VARIABLEHEIGHT;
	if ( xflags[TEXT('w')] )
		iFlags |= RBBS_BREAK;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::insertBand(const int uIndex, LPREBARBANDINFO lprbbi)
{
	return SendMessage(m_Hwnd, RB_INSERTBAND, gsl::narrow_cast<WPARAM>(uIndex), reinterpret_cast<LPARAM>(lprbbi));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::deleteBand(const UINT uIndex)
{
	return SendMessage(m_Hwnd, RB_DELETEBAND, gsl::narrow_cast<WPARAM>(uIndex), gsl::narrow_cast<LPARAM>(0));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getBandInfo(const UINT uBand, LPREBARBANDINFO lprbbi) const noexcept
{
	return SendMessage(m_Hwnd, RB_GETBANDINFO, gsl::narrow_cast<WPARAM>(uBand), reinterpret_cast<LPARAM>(lprbbi));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setBandInfo(const UINT uBand, LPREBARBANDINFO lprbbi)
{
	return SendMessage(m_Hwnd, RB_SETBANDINFO, gsl::narrow_cast<WPARAM>(uBand), reinterpret_cast<LPARAM>(lprbbi));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setBarInfo(LPREBARINFO lprbi)
{
	return SendMessage(m_Hwnd, RB_SETBARINFO, gsl::narrow_cast<WPARAM>(0), reinterpret_cast<LPARAM>(lprbi));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getBarInfo(LPREBARINFO lprbi) const noexcept
{
	return SendMessage(m_Hwnd, RB_GETBARINFO, gsl::narrow_cast<WPARAM>(0), reinterpret_cast<LPARAM>(lprbi));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getRowCount() const noexcept
{
	return SendMessage(m_Hwnd, RB_GETROWCOUNT, gsl::narrow_cast<WPARAM>(0), gsl::narrow_cast<LPARAM>(0));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::hitTest(LPRBHITTESTINFO lprbht) const noexcept
{
	return SendMessage(m_Hwnd, RB_HITTEST, gsl::narrow_cast<WPARAM>(0), reinterpret_cast<LPARAM>(lprbht));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getToolTips() const noexcept
{
	return SendMessage(m_Hwnd, RB_GETTOOLTIPS, gsl::narrow_cast<WPARAM>(0), gsl::narrow_cast<LPARAM>(0));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setToolTips(const HWND hwndToolTip)
{
	return SendMessage(m_Hwnd, RB_SETTOOLTIPS, reinterpret_cast<WPARAM>(hwndToolTip), gsl::narrow_cast<LPARAM>(0));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getIDToIndex(const UINT uBandID) const noexcept
{
	return SendMessage(m_Hwnd, RB_IDTOINDEX, gsl::narrow_cast<WPARAM>(uBandID), gsl::narrow_cast<LPARAM>(0));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getBandCount() const noexcept
{
	return SendMessage(m_Hwnd, RB_GETBANDCOUNT, gsl::narrow_cast<WPARAM>(0), gsl::narrow_cast<LPARAM>(0));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setReDraw(const BOOL uState)
{
	return SendMessage(m_Hwnd, WM_SETREDRAW, gsl::narrow_cast<WPARAM>(uState), gsl::narrow_cast<LPARAM>(uState));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::showBand(const UINT uBand, const BOOL fShow)
{
	return SendMessage(m_Hwnd, RB_SHOWBAND, gsl::narrow_cast<WPARAM>(uBand), gsl::narrow_cast<LPARAM>(fShow));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::moveBand(const UINT iFrom, const UINT iTo)
{
	return SendMessage(m_Hwnd, RB_MOVEBAND, gsl::narrow_cast<WPARAM>(iFrom), gsl::narrow_cast<LPARAM>(iTo));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::maxBand(const UINT uBand, const BOOL fIdeal)
{
	return SendMessage(m_Hwnd, RB_MAXIMIZEBAND, gsl::narrow_cast<WPARAM>(uBand), gsl::narrow_cast<LPARAM>(fIdeal));
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::minBand(const UINT uBand, const BOOL fIdeal)
{
	return SendMessage(m_Hwnd, RB_MINIMIZEBAND, gsl::narrow_cast<WPARAM>(uBand), gsl::narrow_cast<LPARAM>(fIdeal));
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxReBar::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
{
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (hdr == nullptr)
			break;

		switch (hdr->code)
		{
		case NM_CUSTOMDRAW:
		{
			dcxlParam(LPNMCUSTOMDRAW, lpncd);

			bParsed = TRUE;

			switch (lpncd->dwDrawStage)
			{
			case CDDS_PREPAINT:
				return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);
			case CDDS_ITEMPREPAINT:
			{
				auto lpdcxrbb = reinterpret_cast<LPDCXRBBAND>(lpncd->lItemlParam);

				if (lpdcxrbb == nullptr)
					return CDRF_DODEFAULT;

				if (lpdcxrbb->clrText != CLR_INVALID)
					SetTextColor(lpncd->hdc, lpdcxrbb->clrText);
			}
			return (CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT);
			case CDDS_ITEMPOSTPAINT:
				return CDRF_DODEFAULT;
			default:
				return CDRF_DODEFAULT;
			}
		} // NM_CUSTOMDRAW
		break;

		case RBN_HEIGHTCHANGE:
		{
			bParsed = TRUE;
			
			if (RECT rc{}; GetWindowRect(m_Hwnd, &rc))
			{
				if (const UINT width = gsl::narrow_cast<UINT>(rc.right - rc.left), height = gsl::narrow_cast<UINT>(rc.bottom - rc.top); (m_iWidth != width || m_iHeight != height))
				{
					execAliasEx(TEXT("change,%d,%d,%d"), getUserID(), width, height);

					m_iWidth = width;
					m_iHeight = height;
				}
			}
		}
		break;

		case RBN_ENDDRAG:
		{
			bParsed = TRUE;
			redrawWindow();
		}
		break;

		case RBN_DELETINGBAND:
		{
			bParsed = TRUE;

			dcxlParam(LPNMREBAR, lpnmrb);

			if (lpnmrb == nullptr)
				break;

			REBARBANDINFO rbBand{};
			rbBand.cbSize = sizeof(REBARBANDINFO);
			rbBand.fMask = RBBIM_CHILD;

			if (getBandInfo(lpnmrb->uBand, &rbBand) != 0)
			{
				if (IsWindow(rbBand.hwndChild))
					m_pParentDialog->deleteControl(m_pParentDialog->getControlByHWND(rbBand.hwndChild));

				auto lpdcxrbb = reinterpret_cast<LPDCXRBBAND>(lpnmrb->lParam);
				delete lpdcxrbb;
			}
		}
		break;
		} // switch
	}
	break;
	}

	return 0L;
}

LRESULT DcxReBar::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
{
	LRESULT lRes = 0L;
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		dcxlParam(LPNMHDR, hdr);

		if (hdr == nullptr)
			break;

		if (IsWindow(hdr->hwndFrom))
		{
			if (auto c_this = reinterpret_cast<DcxControl *>(GetProp(hdr->hwndFrom, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_HSCROLL:
	case WM_VSCROLL:
	case WM_COMMAND:
	{
		if (IsWindow((HWND)lParam))
		{
			if (auto c_this = reinterpret_cast<DcxControl *>(GetProp((HWND)lParam, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_DELETEITEM:
	{
		dcxlParam(LPDELETEITEMSTRUCT, idata);

		if ((idata != nullptr) && (IsWindow(idata->hwndItem)))
		{
			if (auto c_this = reinterpret_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_MEASUREITEM:
	{
		if (auto cHwnd = GetDlgItem(m_Hwnd, gsl::narrow_cast<int>(wParam)); IsWindow(cHwnd))
		{
			if (auto c_this = reinterpret_cast<DcxControl *>(GetProp(cHwnd, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_DRAWITEM:
	{
		dcxlParam(LPDRAWITEMSTRUCT, idata);

		if ((idata != nullptr) && (IsWindow(idata->hwndItem)))
		{
			if (auto c_this = reinterpret_cast<DcxControl *>(GetProp(idata->hwndItem, TEXT("dcx_cthis"))); c_this != nullptr)
				lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
		}
	}
	break;

	case WM_LBUTTONUP:
	{
		RBHITTESTINFO rbhi{};
		if (GetCursorPos(&rbhi.pt))
		{
			MapWindowPoints(nullptr, m_Hwnd, &rbhi.pt, 1);
			const auto band = hitTest(&rbhi);

			if ((dcx_testflag(rbhi.flags, RBHT_GRABBER) || dcx_testflag(rbhi.flags, RBHT_CAPTION)) && band != -1) {

				m_iClickedBand = band;

				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
					this->execAliasEx(TEXT("sclick,%d,%d"), getUserID(), band + 1);
			}
		}
	}
	break;

	case WM_CONTEXTMENU:
	{
		if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
		{
			RBHITTESTINFO rbhi{};
			if (GetCursorPos(&rbhi.pt))
			{
				MapWindowPoints(nullptr, m_Hwnd, &rbhi.pt, 1);

				if (const auto band = this->hitTest(&rbhi); band != -1)
					this->execAliasEx(TEXT("%s,%d,%d"), TEXT("rclick"), this->getUserID(), band + 1);
			}
		}
	}
	break;

	case WM_SIZE:
	{
		InvalidateRect(m_Hwnd, nullptr, TRUE);
	}
	break;

	case WM_DESTROY:
	{
		delete this;
		bParsed = TRUE;
	}
	break;

	default:
		lRes = this->CommonMessage(uMsg, wParam, lParam, bParsed);
		break;
	}

	return lRes;
}
