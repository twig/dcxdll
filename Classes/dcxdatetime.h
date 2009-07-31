/*!
 * \file dcxdatetime.h
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

#ifndef _DCXDATETIME_H_
#define _DCXDATETIME_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxDateTime : public DcxControl {
public:
  DcxDateTime(UINT ID, DcxDialog *p_Dialog, HWND mParentHwnd, RECT *rc, TString &styles);
  virtual ~DcxDateTime();

  LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed);
  LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed);

  void parseInfoRequest(TString &input, char *szReturnValue);
  void parseCommandRequest(TString &input);
  void parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme);

  inline TString getType() { return TString("datetime"); };
  void toXml(TiXmlElement * xml);
  TString getStyles(void);
};

#endif // _DCXDATETIME_H_
