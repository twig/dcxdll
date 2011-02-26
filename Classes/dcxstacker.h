/*!
 * \file dcxstacker.h
 * \brief blah
 *
 * blah
 *
 * \author Ook
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2006
 */

#ifndef _DCXSTACKER_H_
#define _DCXSTACKER_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/xpopup/xpopupmenuitem.h"

#define MIN_STACK_HEIGHT 20 //!< Min height for a stacker item.

#define STACKERS_GRAD			0x01
#define STACKERS_IMAGE		0x02
#define STACKERS_ARROW		0x04
#define STACKERS_COLLAPSE	0x08

#define ST_ERR	-1

#define STACKERF_SELECTED	0x01

typedef struct tagDCXSITEM {
	TString			tsTipText;	//!< Tooltip text
	TString			tsCaption;	//!< Title Buttons text
	COLORREF		clrBack;		//!< Line Background Caption Color
	COLORREF		clrText;		//!< Line Caption Color
	HFONT				hFont;			//!< Items font.
	DcxControl *pChild;			//!< Items child control
	int					iItemImg;		//!< Items Normal Image index.
	int					iSelectedItemImg;		//!< Items Selected Image index.
	RECT				itemrc;			//!< Items Rect.
	DWORD				dFlags;			//!< Items flags.
} DCXSITEM,*LPDCXSITEM;

typedef std::vector<Image *> VectorOfImages;
typedef std::vector<LPDCXSITEM> VectorOfStackerItems;

/*!
 * \brief blah
 *
 * blah
 */

class DcxStacker : public DcxControl {

public:

	DcxStacker( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, const TString & styles );
	virtual ~DcxStacker( );

	LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

	void parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const;
	void parseCommandRequest( const TString & input );
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

	inline TString getType( ) const { return TString( TEXT("stacker") ); };
	TString getStyles(void) const;

protected:
	HWND m_hActive;
	DWORD m_dStyles;
	VectorOfImages m_vImageList;
	VectorOfStackerItems m_vItems;

	//
	int getItemID(void) const;
	int getSelItemID(void) const;
	DWORD getItemCount(void) const;
	LPDCXSITEM getItem(const int nPos) const;
	LPDCXSITEM getHotItem(void) const;
	void getItemRect(const int nPos, LPRECT rc) const;
	//
	//int setItem(int nPos, LPDCXSITEM item);
	//int setSelectedItem(int nPos);
	//
	//void deleteItem(int nPos);
	//
	void DrawSItem(const LPDRAWITEMSTRUCT idata);
	static void DrawAliasedTriangle(const HDC hdc, const LPRECT rc, const COLORREF clrShape);
	void DrawItemImage(const HDC hdc, Image *img, const LPRECT rc);
	//
	void clearImageList(void);
	void clearItemList(void);
	//
	void calcItemRect(LPRECT rc);
};

#endif //_DCXSTACKER_H_
