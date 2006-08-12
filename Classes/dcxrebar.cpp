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

#include "dcxrebar.h"
#include "dcxdialog.h"

#include "dcxprogressbar.h"
#include "dcxtrackbar.h"
#include "dcxcolorcombo.h"
#include "dcxcomboex.h"
#include "dcxstatusbar.h"
#include "dcxtreeview.h"
#include "dcxtoolbar.h"
#include "dcxlistview.h"
#include "dcxbutton.h"
#include "dcxrichedit.h"

#include "dcxdivider.h"
#include "dcxpanel.h"
#include "dcxtab.h"

#include "dcxmwindow.h"
#include "dcxmdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxReBar::DcxReBar( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) : DcxControl( ID, p_Dialog ) {

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles,
    DCX_REBARCTRLCLASS,
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  this->m_iClickedBand = -1;
  this->m_iRowLimit = 0;
  this->m_iWidth = 0;
  this->m_iHeight = 0;

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxReBar::DcxReBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles, 
    DCX_REBARCTRLCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  this->m_iClickedBand = -1;
  this->m_iRowLimit = 0;
  this->m_iWidth = 0;
  this->m_iHeight = 0;

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief Destructor
 *
 * blah
 */

DcxReBar::~DcxReBar( ) {

  this->resetContents( );

  ImageList_Destroy( this->getImageList( ) );

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxReBar::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  *Styles |= RBS_AUTOSIZE;
  unsigned int i = 1, numtok = styles.numtok( " " );

	//*ExStyles |= WS_EX_CONTROLPARENT;

  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "borders" ) 
      *Styles |= RBS_BANDBORDERS;
    else if ( styles.gettok( i , " " ) == "dblclktoggle" ) 
      *Styles |= RBS_DBLCLKTOGGLE;
    else if ( styles.gettok( i , " " ) == "fixedorder" ) 
      *Styles |= RBS_FIXEDORDER;
    else if ( styles.gettok( i , " " ) == "varheight" ) 
      *Styles |= RBS_VARHEIGHT;
    else if ( styles.gettok( i , " " ) == "tooltips" ) 
      *Styles |= RBS_TOOLTIPS;
    else if ( styles.gettok( i , " " ) == "verticalgrip" ) 
      *Styles |= RBS_VERTICALGRIPPER;
    else if ( styles.gettok( i , " " ) == "vertical" ) 
      *Styles |= CCS_VERT;
    else if ( styles.gettok( i , " " ) == "right" ) 
      *Styles |= CCS_RIGHT;
    else if ( styles.gettok( i , " " ) == "bottom" ) 
      *Styles |= CCS_BOTTOM;
    else if ( styles.gettok( i , " " ) == "noresize" ) 
      *Styles |= CCS_NORESIZE;
    else if ( styles.gettok( i , " " ) == "noparentalign" ) 
      *Styles |= CCS_NOPARENTALIGN ;

    i++;
  }
  this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxReBar::getImageList( ) {

  REBARINFO ri;
  ZeroMemory(&ri, sizeof(REBARINFO));
  ri.cbSize = sizeof(REBARINFO);
  ri.fMask = RBIM_IMAGELIST;
  this->getBarInfo( &ri );
  return ri.himl;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxReBar::setImageList( HIMAGELIST himl ) {

  REBARINFO ri;
  ZeroMemory( &ri, sizeof(REBARINFO) );
  ri.cbSize = sizeof(REBARINFO);
  ri.himl = himl;
  ri.fMask = RBIM_IMAGELIST;
  this->setBarInfo( &ri );
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxReBar::createImageList( ) {
  return ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxReBar::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );

  if ( input.gettok( 3, " " ) == "num" ) {

    wsprintf( szReturnValue, "%d", this->getBandCount( ) );
    return;
  }
  // [NAME] [ID] [PROP] [N]
  else if ( input.gettok( 3, " " ) == "text" && numtok > 3 ) {

    int nIndex = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {

      REBARBANDINFO rbBand;
      ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
      rbBand.cbSize = sizeof( REBARBANDINFO );
      rbBand.fMask = RBBIM_TEXT;
      rbBand.cch = 512;
      rbBand.lpText = szReturnValue;
      if ( this->getBandInfo( nIndex, &rbBand ) != 0 )
        return;
    }
  }
  if ( input.gettok( 3, " " ) == "childid" && numtok > 3 ) {

    int nItem = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nItem > -1 && nItem < this->getBandCount( ) ) {
      
      REBARBANDINFO rbBand;
      ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
      rbBand.cbSize = sizeof( REBARBANDINFO );
      rbBand.fMask = RBBIM_CHILD;

      if ( this->getBandInfo( nItem, &rbBand ) != 0 ) {

        DcxControl * c = this->m_pParentDialog->getControlByHWND( rbBand.hwndChild );
        if ( c != NULL ) 
          wsprintf( szReturnValue, "%d", c->getUserID( ) );
      }

      return;
    }
  }
  else if ( this->parseGlobalInfoRequest( input, szReturnValue ) ) {

    return;
  }

  szReturnValue[0] = 0;
}
/*!
 * \brief blah
 *
 * \param input [NAME] [SWITCH] [ID] (OPTIONS)
 */

void DcxReBar::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  // xdid -a [NAME] [ID] [SWITCH] [N] [+FLAGS] [CX] [CY] [WIDTH] [ICON] [COLOR] [Item Text][TAB][ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)[TAB]Tooltip
  if ( flags.switch_flags[0] && numtok > 9 ) {

    REBARBANDINFO rbBand;
    ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
    rbBand.cbSize = sizeof( REBARBANDINFO );
    rbBand.fMask = RBBIM_STYLE | RBBIM_LPARAM;

    TString data = input.gettok( 1, "\t" );
    data.trim( );

    TString control_data = "";
    if ( input.numtok( "\t" ) > 1 ) {

      control_data = input.gettok( 2, "\t" );
      control_data.trim( );
    }

    TString tooltip = "";
    if ( input.numtok( "\t" ) > 2 ) {

      tooltip = input.gettok( 3, "\t" );
      tooltip.trim( );
    }

    //int nIndex = atoi( data.gettok( 4, " " ).to_chr( ) ) - 1;
    int cx = atoi( data.gettok( 6, " " ).to_chr( ) );
    int cy = atoi( data.gettok( 7, " " ).to_chr( ) );
    int width = atoi( data.gettok( 8, " " ).to_chr( ) );
    int nIcon = atoi( data.gettok( 9, " " ).to_chr( ) ) - 1;
    COLORREF clrText = atol( data.gettok( 10, " " ).to_chr( ) );

    TString itemtext;
    if ( data.numtok( " " ) > 10 ) {

      itemtext = data.gettok( 11, -1, " " );
      rbBand.fMask |= RBBIM_TEXT;
      rbBand.lpText = itemtext.to_chr( );
    }

    rbBand.fStyle = this->parseBandStyleFlags( data.gettok( 5, " " ) );

    // Tooltip Handling
    LPDCXRBBAND lpdcxrbb = new DCXRBBAND;

    if ( rbBand.fStyle & RBBS_UNDERLINE )
      lpdcxrbb->bUline = TRUE;
    else
      lpdcxrbb->bUline = FALSE;

    if ( rbBand.fStyle & RBBS_BOLD )
      lpdcxrbb->bBold = TRUE;
    else
      lpdcxrbb->bBold = FALSE;

    if ( rbBand.fStyle & RBBS_COLOR )
      lpdcxrbb->clrText = clrText;
    else
      lpdcxrbb->clrText = -1;

    if ( nIcon > -1 ) {
      rbBand.iImage = nIcon;
      rbBand.fMask |= RBBIM_IMAGE;
    }

    rbBand.lParam = (LPARAM) lpdcxrbb;

    if ( control_data.numtok( " " ) > 5 ) {


      UINT ID = mIRC_ID_OFFSET + atoi( control_data.gettok( 1, " " ).to_chr( ) );

      if ( ID > mIRC_ID_OFFSET - 1 && 
        !IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
        this->m_pParentDialog->getControlByID( ID ) == NULL ) 
      {

        TString type = control_data.gettok( 2, " " );
        RECT rc;

        rc.left = atol( control_data.gettok( 3, " " ).to_chr( ) );
        rc.top = atol( control_data.gettok( 4, " " ).to_chr( ) );
        rc.right = rc.left + atol( control_data.gettok( 5, " " ).to_chr( ) );
        rc.bottom = rc.top + atol( control_data.gettok( 6, " " ).to_chr( ) );

        DcxControl * p_Control = NULL;

        TString styles = "";
        if ( control_data.numtok( " " ) > 6 ) {
          styles = control_data.gettok( 7, -1, " " );
        }

        if ( type == "trackbar" ) {
          p_Control = new DcxTrackBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles  );
        }
        else if ( type == "pbar" ) {
          p_Control = new DcxProgressBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "comboex" ) {
          p_Control = new DcxComboEx( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "toolbar" ) {
          p_Control = new DcxToolBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
          p_Control->addStyle( CCS_NOPARENTALIGN | CCS_NORESIZE );
        }
        else if ( type == "statusbar" ) {
          p_Control = new DcxStatusBar( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
          p_Control->addStyle( CCS_NOPARENTALIGN | CCS_NORESIZE );
        }
        else if ( type == "treeview" ) {
          p_Control = new DcxTreeView( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "listview" ) {
          p_Control = new DcxListView( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "colorcombo" ) {
          p_Control = new DcxColorCombo( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "button" ) {
          p_Control = new DcxButton( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "richedit" ) {
          p_Control = new DcxRichEdit( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "divider" ) {
          p_Control = new DcxDivider( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
        else if ( type == "panel" ) {
          p_Control = new DcxPanel( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }
         else if ( type == "tab" ) {
          p_Control = new DcxTab( ID, this->m_pParentDialog, this->m_Hwnd, &rc, styles );
        }

        if ( p_Control != NULL ) {

          this->m_pParentDialog->addControl( p_Control );
          rbBand.fMask |= RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE;
          rbBand.hwndChild = p_Control->getHwnd( );
          rbBand.cxMinChild = cx;
          rbBand.cyMinChild = cy;
          rbBand.cx = width;
					if (!this->isExStyle(WS_EX_CONTROLPARENT)) {
						if (p_Control->isStyle(WS_TABSTOP)) this->addExStyle(WS_EX_CONTROLPARENT);
					}
        }
      }
      else {
        TString error;
        error.sprintf("/xdid -a : Control with ID \"%d\" already exists", ID - mIRC_ID_OFFSET );
				mIRCError( error.to_chr() );
      }
    }
  
    this->insertBand( -1, &rbBand );
  }
  // xdid -d [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[3] && numtok > 3 ) {

    int nIndex = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {
     
      this->deleteBand( nIndex );
			if (GetWindow(this->m_Hwnd,GW_CHILD) == NULL) { // if no children remove style
				if (this->isExStyle(WS_EX_CONTROLPARENT)) {
					this->removeExStyle(WS_EX_CONTROLPARENT);
				}
			}
    }
  }
  // xdid -i [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[8] && numtok > 3 ) {

    int nIndex = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {
     
      this->showBand( nIndex, FALSE );
    }
  }
  // xdid -j [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[9] && numtok > 3 ) {

    int nIndex = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {
     
      this->showBand( nIndex, TRUE );
    }
  }
  // xdid -k [NAME] [ID] [SWITCH] [N] [ICON]
  else if ( flags.switch_flags[10] && numtok > 4 ) {

    REBARBANDINFO rbBand;
    ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
    rbBand.cbSize = sizeof( REBARBANDINFO );
    rbBand.fMask = RBBIM_IMAGE;

    int nIndex = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    int nIcon = atoi( input.gettok( 5, " " ).to_chr( ) ) - 1;
    if ( nIndex > -1 && nIndex < this->getBandCount( ) && nIcon > -2 ) {

      rbBand.iImage = nIcon;
      this->setBandInfo( nIndex, &rbBand );
    }
  }
  // xdid -l [NAME] [ID] [SWITCH] [N|ALL]
  else if ( flags.switch_flags[11] && numtok > 3 ) {

    REBARBANDINFO rbBand;
    ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
    rbBand.cbSize = sizeof( REBARBANDINFO );
    rbBand.fMask = RBBIM_STYLE;

    int nItems = this->getBandCount( );

    if ( input.gettok( 4, " " ) == "all" ) {

      int i = 0;

      while ( i < nItems ) {

        if ( this->getBandInfo( i, &rbBand ) != 0 ) {

          rbBand.fStyle |= RBBS_NOGRIPPER;
          this->setBandInfo( i, &rbBand );
        }
        i++;
      }
    }
    else {

      int nIndex = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

      if ( nIndex > -1 && nIndex < nItems && this->getBandInfo( nIndex, &rbBand ) != 0 ) {

        rbBand.fStyle |= RBBS_NOGRIPPER;
        this->setBandInfo( nIndex, &rbBand );
      }
    }
  }
  // xdid -m [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[12] && numtok > 3 ) {

    int nIndex = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {
     
      this->maxBand( nIndex, FALSE );
    }
  }
  // xdid -n [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[13] && numtok > 3 ) {

    int nIndex = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {
     
      this->minBand( nIndex, FALSE );
    }
  }
  // xdid -q [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[16] && numtok > 3 ) {

    int nRows = atoi( input.gettok( 4, " " ).to_chr( ) );

    if ( nRows > -1 ) {
     
      this->m_iRowLimit = nRows;
    }
  }
  // xdid -t [NAME] [ID] [SWITCH] [N] [TEXT]
  else if ( flags.switch_flags[19] && numtok > 3 ) {

    REBARBANDINFO rbBand;
    ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
    rbBand.cbSize = sizeof( REBARBANDINFO );
    rbBand.fMask = RBBIM_TEXT;

    int nIndex = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    if ( nIndex > -1 && nIndex < this->getBandCount( ) ) {

      TString itemtext;
      if ( numtok > 4 ) {

        itemtext = input.gettok( 5, -1, " " );
        itemtext.trim( );
      }
      rbBand.lpText = itemtext.to_chr( );
      this->setBandInfo( nIndex, &rbBand );
    }
  }
  // xdid -u [NAME] [ID] [SWITCH] [N|ALL]
  else if ( flags.switch_flags[20] && numtok > 3 ) {

    REBARBANDINFO rbBand;
    ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
    rbBand.cbSize = sizeof( REBARBANDINFO );
    rbBand.fMask = RBBIM_STYLE;

    int nItems = this->getBandCount( );

    if ( input.gettok( 4, " " ) == "all" ) {

      int i = 0;

      while ( i < nItems ) {

        if ( this->getBandInfo( i, &rbBand ) != 0 ) {

          rbBand.fStyle &= ~RBBS_NOGRIPPER;
          this->setBandInfo( i, &rbBand );
        }
        i++;
      }
    }
    else {

      int nIndex = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

      if ( nIndex > -1 && nIndex < nItems && this->getBandInfo( nIndex, &rbBand ) != 0 ) {

        rbBand.fStyle &= ~RBBS_NOGRIPPER;
        this->setBandInfo( nIndex, &rbBand );
      }
    }
  }
  // xdid -v [NAME] [ID] [SWITCH] [NFrom] [NTo]
  else if ( flags.switch_flags[21] && numtok > 4 ) {

    int nIndexFrom = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    int nIndexTo = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;
    int nItems = this->getBandCount( );

    if ( nIndexFrom > -1 && nIndexFrom < nItems && nIndexTo > -1 && nIndexTo < nItems ) {
     
      this->moveBand( nIndexFrom, nIndexTo );
    }
  }
  // xdid -w [NAME] [ID] [SWITCH] [INDEX] [FILENAME]
  else if ( flags.switch_flags[22] && numtok > 4 ) {

    HIMAGELIST himl;
    HICON icon;
    int index;

    if ( ( himl = this->getImageList( ) ) == NULL ) {

      himl = this->createImageList( );

      if ( himl )
        this->setImageList( himl );
    }

    index = atoi( input.gettok( 4, " ").to_chr( ) );
    TString filename = input.gettok( 5, -1, " " );
    ExtractIconEx( filename.to_chr( ), index, 0, &icon, 1 );
    ImageList_AddIcon( himl, icon );
    DestroyIcon( icon );
  }
  // xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
  else if ( flags.switch_flags[24] ) {

    ImageList_Destroy( this->getImageList( ) );
  }
  else {
    this->parseGlobalCommandRequest( input, flags );
  }
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxReBar::resetContents( ) {

  int nItems = this->getBandCount( );

    while ( nItems-- )
      this->deleteBand( nItems );
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxReBar::parseBandStyleFlags( TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if ( flags[i] == 'b' )
      iFlags |= RBBS_BOLD;
    else if ( flags[i] == 'c' )
      iFlags |= RBBS_COLOR;
    else if ( flags[i] == 'e' )
      iFlags |= RBBS_CHILDEDGE;
    else if ( flags[i] == 'f' )
      iFlags |= RBBS_FIXEDSIZE;
    else if ( flags[i] == 'g' )
      iFlags |= RBBS_GRIPPERALWAYS;
    else if ( flags[i] == 'h' )
      iFlags |= RBBS_HIDDEN;
    else if ( flags[i] == 'n' )
      iFlags |= RBBS_NOGRIPPER;
    else if ( flags[i] == 'o' )
      iFlags |= RBBS_USECHEVRON;
    else if ( flags[i] == 'u' )
      iFlags |= RBBS_UNDERLINE;
    else if ( flags[i] == 'w' )
      iFlags |= RBBS_BREAK;

    ++i;
  }
  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::insertBand(int uIndex, LPREBARBANDINFO lprbbi ) {
  return SendMessage(this->m_Hwnd, RB_INSERTBAND, (WPARAM) uIndex, (LPARAM) lprbbi ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::deleteBand( UINT uIndex ) {
  return SendMessage( this->m_Hwnd, RB_DELETEBAND, (WPARAM) uIndex, (LPARAM) 0 ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getBandInfo( UINT uBand, LPREBARBANDINFO lprbbi ) {
  return SendMessage( this->m_Hwnd, RB_GETBANDINFO, (WPARAM) uBand, (LPARAM) lprbbi ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setBandInfo( UINT uBand, LPREBARBANDINFO lprbbi ) {
  return SendMessage( this->m_Hwnd, RB_SETBANDINFO, (WPARAM) uBand, (LPARAM) lprbbi ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setBarInfo( LPREBARINFO lprbi ) {
  return SendMessage( this->m_Hwnd, RB_SETBARINFO, (WPARAM) 0, (LPARAM) lprbi ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getBarInfo( LPREBARINFO lprbi ) {
  return SendMessage( this->m_Hwnd, RB_GETBARINFO, (WPARAM) 0, (LPARAM) lprbi ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getRowCount( ) {
  return SendMessage( this->m_Hwnd, RB_GETROWCOUNT, (WPARAM) 0, (LPARAM) 0 ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::hitTest( LPRBHITTESTINFO lprbht ) {
  return SendMessage( this->m_Hwnd, RB_HITTEST, (WPARAM) 0, (LPARAM) lprbht ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getToolTips( ) {
  return SendMessage( this->m_Hwnd, RB_GETTOOLTIPS, (WPARAM) 0, (LPARAM) 0 ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setToolTips( HWND hwndToolTip ) {
  return SendMessage( this->m_Hwnd, RB_SETTOOLTIPS, (WPARAM) hwndToolTip, (LPARAM) 0 ); 
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getIDToIndex( UINT uBandID ) {
  return SendMessage( this->m_Hwnd, RB_IDTOINDEX, (WPARAM) uBandID, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::getBandCount( ) {
  return SendMessage( this->m_Hwnd, RB_GETBANDCOUNT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::setReDraw( BOOL uState ) {
  return SendMessage( this->m_Hwnd, WM_SETREDRAW, (WPARAM) uState, (LPARAM) uState );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::showBand( UINT uBand, BOOL fShow ) {
  return SendMessage( this->m_Hwnd, RB_SHOWBAND, (WPARAM) uBand, (LPARAM) fShow );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::moveBand( UINT iFrom, UINT iTo ) {
  return SendMessage( this->m_Hwnd, RB_MOVEBAND, (WPARAM) iFrom, (LPARAM) iTo );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::maxBand( UINT uBand, BOOL fIdeal ) {
  return SendMessage( this->m_Hwnd, RB_MAXIMIZEBAND, (WPARAM) uBand, (LPARAM) fIdeal );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::minBand( UINT uBand, BOOL fIdeal ) {
  return SendMessage( this->m_Hwnd, RB_MINIMIZEBAND, (WPARAM) uBand, (LPARAM) fIdeal );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxReBar::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_NOTIFY : 
      {

        //mIRCError( "Rebar WM_NOTIFY" );
        LPNMHDR hdr = (LPNMHDR) lParam;

        if (!hdr)
          break;

        char ClassName[256];

        if ( IsWindow( hdr->hwndFrom ) && GetClassName( hdr->hwndFrom, ClassName, 256 ) != 0 ) {

          switch( hdr->code ) {

            case NM_CLICK:
              {
                //mIRCError( "Rebar WM_NOTIFY - NM_CLICK" );
                if ( lstrcmp( DCX_STATUSBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                /*
                else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                */
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            case NM_DBLCLK:
              {
                //mIRCError( "Rebar WM_NOTIFY - NM_DBLCLK" );
                if ( lstrcmp( DCX_STATUSBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                 bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            case NM_RCLICK:
              {
                //mIRCError( "Rebar WM_NOTIFY - NM_RCLICK" );
                if ( lstrcmp( DCX_STATUSBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            case NM_CUSTOMDRAW:
              {
                if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
                else {

                  LPNMCUSTOMDRAW lpncd = (LPNMCUSTOMDRAW) lParam;

                  bParsed = TRUE;

                  switch( lpncd->dwDrawStage ) {

                    case CDDS_PREPAINT:
                      return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);

                    case CDDS_ITEMPREPAINT:
                      {
                        
                        LPDCXRBBAND lpdcxrbb = (LPDCXRBBAND) lpncd->lItemlParam;

                        if ( lpdcxrbb == NULL )
                          return CDRF_DODEFAULT;

                        if ( lpdcxrbb->clrText != -1 )
                          SetTextColor( lpncd->hdc, lpdcxrbb->clrText );
                      }
                     
                      return (CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT);

                    case CDDS_ITEMPOSTPAINT:
                      return CDRF_DODEFAULT;

                    default:
                      return CDRF_DODEFAULT;
                  }
                } // Rebar
              } // NM_CUSTOMDRAW
              break;

              // TreeView Related Notifications
            case TVN_GETINFOTIP:
            case TVN_ITEMEXPANDED:
            case TVN_BEGINLABELEDIT:
            case TVN_ENDLABELEDIT:
            case TVN_DELETEITEM:
              {
                if ( lstrcmp( DCX_TREEVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            case TBN_DELETINGBUTTON:
            case TBN_DROPDOWN:
            case TBN_GETINFOTIP:
              {
                if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            case LVN_BEGINLABELEDIT:
            case LVN_ENDLABELEDIT:
            case LVN_DELETEITEM:
              {
                if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
                }
              }
              break;

            case HDN_BEGINTRACK:
            case HDN_ITEMCLICK:
            case HDN_ITEMDBLCLICK:
              {
                GetClassName( GetParent( hdr->hwndFrom ), ClassName, 256 );
                if ( lstrcmp( DCX_LISTVIEWCLASS, ClassName ) == 0 ) {
                  bParsed = TRUE;
                  return SendMessage( GetParent( hdr->hwndFrom ), uMsg, wParam, lParam );
                }
              }
              break;

            case TCN_SELCHANGE:
              {
                if ( lstrcmp( DCX_TABCTRLCLASS, ClassName ) == 0 )
                  return SendMessage( hdr->hwndFrom, uMsg, wParam, lParam );
              }
              break;

            case RBN_HEIGHTCHANGE:
              {

                //mIRCSignal( "RBN_HEIGHTCHANGE" );
                bParsed = TRUE;

                /*
                LPNMREBAR lpnmrb = (LPNMREBAR) lParam;

                REBARBANDINFO rbBand;
                ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
                rbBand.cbSize = sizeof( REBARBANDINFO );
                rbBand.fMask = RBBIM_STYLE;


                char data[500];
                wsprintf( data, "RL %d - GETR %d - CLB %d", this->m_iRowLimit, this->getRowCount( ), this->m_iClickedBand );
                mIRCError( data );

                if ( this->m_iRowLimit > 0 && this->getRowCount( ) > this->m_iRowLimit ) {

                  if ( this->getBandInfo( this->m_iClickedBand, &rbBand ) != 0 ) {

                    mIRCError( "Got Band Info" );

                    if ( rbBand.fStyle & RBBS_BREAK ) {

                      mIRCError( "Reverting Band Here" );

                      //SendMessage( this->m_Hwnd, WM_SETREDRAW, FALSE, 0 );
                      rbBand.fStyle &= ~RBBS_BREAK;
                      this->setBandInfo( this->m_iClickedBand, &rbBand );
                      //SendMessage( this->m_Hwnd, WM_SETREDRAW, TRUE, 0 );
                    }
                  }
                }
                else {
                */

                RECT rc;
                GetWindowRect( this->m_Hwnd, &rc );
                int width = rc.right - rc.left;
                int height = rc.bottom - rc.top;

                if ( this->m_iWidth != width || this->m_iHeight != height ) {

                  this->callAliasEx( NULL, "%s,%d,%d,%d", "change", this->getUserID( ),
                    width, height );

                  this->m_iWidth = width;
                  this->m_iHeight = height;
                }

              /*}
                this->redrawWindow( );
                */
              }
              break;

            case RBN_ENDDRAG:
              {
                bParsed = TRUE;
                this->redrawWindow( );
              }
              break;

            case RBN_DELETINGBAND:
              {
                //mIRCSignal( "RBN_DELETINGBAND" );
                bParsed = TRUE;
                
                LPNMREBAR lpnmrb = (LPNMREBAR) lParam;

                REBARBANDINFO rbBand;
                ZeroMemory( &rbBand, sizeof( REBARBANDINFO ) );
                rbBand.cbSize = sizeof( REBARBANDINFO );
                rbBand.fMask = RBBIM_CHILD;

                if ( this->getBandInfo( lpnmrb->uBand, &rbBand ) != 0 ) {

                  if ( IsWindow( rbBand.hwndChild ) ) {

                    DcxControl * p_delControl = this->m_pParentDialog->getControlByHWND( rbBand.hwndChild );
                    this->m_pParentDialog->deleteControl( p_delControl );
                  }

                  LPDCXRBBAND lpdcxrbb = (LPDCXRBBAND) lpnmrb->lParam;
                  if ( lpdcxrbb != NULL )
                    delete lpdcxrbb;
                }
              }
              break;

          } // switch
        }
      }
      break;

    case WM_HSCROLL: 
      {
        char ClassName[256];
        if ( IsWindow( (HWND) lParam ) && GetClassName( (HWND) lParam, ClassName, 256 ) != 0 ) {

          if ( lstrcmp( DCX_TRACKBARCLASS, ClassName ) == 0 ) {
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          }
        }
      }
      break;

    case WM_VSCROLL: 
      {
        char ClassName[256];
        if ( IsWindow( (HWND) lParam ) && GetClassName( (HWND) lParam, ClassName, 256 ) != 0 ) {

          if ( lstrcmp( DCX_TRACKBARCLASS, ClassName ) == 0 ) {
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          }
        }
      }
      break;

    case WM_DELETEITEM:
      {
        //mIRCError( "Rebar WM_DELETEITEM" );

        char ClassName[256];
				DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem)) && (GetClassName( idata->hwndItem, ClassName, 256 ) != 0)) {
          if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {
            //mIRCError( "DCX_COLORCOMBOCLASS WM_DELETEITEM" );
            bParsed = TRUE;
            return SendMessage( idata->hwndItem, uMsg, wParam, lParam );
          }
				}
        //char ClassName[256];
        //HWND cHwnd = GetDlgItem( this->m_Hwnd, wParam );
        //if ( IsWindow( cHwnd ) && GetClassName( cHwnd, ClassName, 256 ) != 0) {

        //  if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {

        //    //mIRCError( "DCX_COLORCOMBOCLASS WM_DELETEITEM" );
        //    bParsed = TRUE;
        //    return SendMessage( cHwnd, uMsg, wParam, lParam );
        //  }
        //}
      }
      break;

    case WM_MEASUREITEM:
      {
        //mIRCError( "Rebar WM_MEASUREITEM" );

				if (ctrl_MeasureItem(this->m_Hwnd, wParam, lParam)) return TRUE;
        //char ClassName[256];
        //HWND cHwnd = GetDlgItem( this->m_Hwnd, wParam );
        //if ( IsWindow( cHwnd ) && GetClassName( cHwnd, ClassName, 256 ) != 0 ) {

        //  if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {

        //    //mIRCError( "DCX_COLORCOMBOCLASS WM_MEASUREITEM" );

        //    LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;

        //    if ( lpmis != NULL )
        //      lpmis->itemHeight = 16; 

        //    return TRUE;
        //  }
        //}
      }
      break;

    case WM_DRAWITEM:
      {
        //mIRCError( "Rebar WM_DRAWITEM" );

        char ClassName[256];
				DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem)) && (GetClassName( idata->hwndItem, ClassName, 256 ) != 0)) {
          if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {
            //mIRCError( "DCX_COLORCOMBOCLASS WM_DRAWITEM" );
            bParsed = TRUE;
            return SendMessage( idata->hwndItem, uMsg, wParam, lParam );
          }
				}
        //char ClassName[256];
        //HWND cHwnd = GetDlgItem( this->m_Hwnd, wParam );
        //if ( IsWindow( cHwnd ) && GetClassName( cHwnd, ClassName, 256 ) != 0) {

        //  if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {

        //    //mIRCError( "DCX_COLORCOMBOCLASS WM_DRAWITEM" );
        //    bParsed = TRUE;
        //    return SendMessage( cHwnd, uMsg, wParam, lParam );
        //  }
        //}
      }
     break;

    case WM_COMMAND:
      {
        //mIRCError( "Rebar WM_COMMAND" );
        char ClassName[256];

        if ( IsWindow( (HWND) lParam ) && GetClassName( (HWND) lParam, ClassName, 256 ) != 0 ) {

          // ComboEx Notifications
          if ( lstrcmp( DCX_COMBOEXCLASS, ClassName ) == 0 ) {

            bParsed = TRUE;
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          }
          // ColorCombo notifications
          else if ( lstrcmp( DCX_COLORCOMBOCLASS, ClassName ) == 0 ) {

            bParsed = TRUE;
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          }
          // Toolbar notifications
          else if ( lstrcmp( DCX_TOOLBARCLASS, ClassName ) == 0 ) {

            bParsed = TRUE;
            return SendMessage( (HWND) lParam, uMsg, wParam, lParam );
          }
        }
      }
      break;

    case WM_LBUTTONDOWN:
      {

        RBHITTESTINFO rbhi;
        GetCursorPos( &rbhi.pt );
        ScreenToClient( this->m_Hwnd, &rbhi.pt );
        int band = this->hitTest( &rbhi );

        if ( ( rbhi.flags & RBHT_GRABBER || rbhi.flags & RBHT_CAPTION ) && band != -1 ) {

          this->m_iClickedBand = band;

          this->callAliasEx( NULL, "%s,%d,%d", "sclick", this->getUserID( ), band + 1 );
        }
      }
      break;

    case WM_CONTEXTMENU:
      {

        RBHITTESTINFO rbhi;
        GetCursorPos( &rbhi.pt );
        ScreenToClient( this->m_Hwnd, &rbhi.pt );
        int band = this->hitTest( &rbhi );

        if ( band != -1 ) {
          this->callAliasEx( NULL, "%s,%d,%d", "rclick", this->getUserID( ), band + 1 );
        }
      }
      break;

    case WM_MOUSEMOVE:
      {
        this->m_pParentDialog->setMouseControl( this->getUserID( ) );
      }
      break;

    case WM_SETFOCUS:
      {
        this->m_pParentDialog->setFocusControl( this->getUserID( ) );
      }
      break;

    case WM_SIZE:
      {
        InvalidateRect( this->m_Hwnd, NULL, TRUE );
      }
      break;

    case WM_SETCURSOR:
      {
        if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd && this->m_hCursor != NULL ) {

          SetCursor( this->m_hCursor );
          bParsed = TRUE;
          return TRUE;
        }
      }
      break;

    case WM_DESTROY:
      {
        //mIRCError( "WM_DESTROY" );
        delete this;
        bParsed = TRUE;
      }
      break;

    default:
      break;
  }

  return 0L;
}