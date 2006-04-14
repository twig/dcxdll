/*!
 * \file layoutcell.h
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * � ScriptsDB.org - 2006
 */

#ifndef _LAYOUTCELL_H_
#define _LAYOUTCELL_H_

#include "../../defines.h"
//#include "defines.h"

#include <vector>

/*!
 * \brief blah
 *
 * blah
 */

typedef struct CellMinMaxInfo {

  POINT m_MinSize; //!< Minimum Cell Size
  POINT m_MaxSize; //!< Maximum Cell Size

} CellMinMaxInfo;

/*!
 * \brief blah
 *
 * blah
 */

class LayoutCell {

public:

  /*!
   * \brief blah
   *
   * blah
   */
  enum CellType {

    FIXED,
    FILL,
    PANE
  };

  LayoutCell( );
  LayoutCell( HWND mHwnd );
  LayoutCell( HWND mHwnd, RECT & rc );
  virtual ~LayoutCell( );

  void setRect( RECT & rc );
  void getRect( RECT & rc );
  void getClientRect( RECT & rc );
  void setBorder( RECT & rc );
  void setBorder( unsigned int nBorder );
  void getBorder( RECT & rc );

  virtual void LayoutChild( ) = 0;
  virtual HDWP ExecuteLayout( HDWP hdwp ) = 0;
  virtual void getMinMaxInfo( CellMinMaxInfo * pCMMI ) = 0;

  void setSibling( LayoutCell * p_Cell );
  void setParent( LayoutCell * p_Cell );

  virtual CellType getType( ) = 0;

  LayoutCell * getFirstChild( );
  LayoutCell * getParent( );
  LayoutCell * getNextSibling( );

  BOOL isVisible( );

protected:

  HWND m_Hwnd; //!< Cell Control Window Handle (NULL if Container Cell)

  RECT m_rcBorders; //!< Border defining rectangle
  RECT m_rcWindow;  //!< Available Window defining rectangle including Borders

  LayoutCell * m_Parent; //!< Used for navigation
  LayoutCell * m_FirstChild; //!< Used for navigation
  LayoutCell * m_NextSibling; //!< Used for navigation

};

#endif // _LAYOUTCELL_H_