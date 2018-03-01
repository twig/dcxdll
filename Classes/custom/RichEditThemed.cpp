//////////////////////////////////////////////////////////////////////////////
//	Themed RichEdit Class Helper for Visual C++								//
//  Created by Patchou, originally for Messenger Plus!						//
//																			//
//	RichEditThemed.cpp   -   Last Revision: January 7th 2006				//
//  Purpose: adds Windows XP theme support to Rich Edit controls			//
//																			//
//  This library is free as in NOT under GPL license. You can use it in any //
//  program, in the binary form of your choice, and without being asked to  //
//  add unnecessary files to your redistribution package. Free source       //
//  should neither be restrained by a 6 page license nor should it force    //
//  you to share the source of any part of a program you do not wish to     //
//  share. The meaning of the word freedom is the same for everyone, no     //
//  license should have the pretension of redefining it. If you want to     //
//  share some source code, please use a similar copyright notice, your     //
//  peers will be grateful.													//
//																			//
//  Free redistribution and usage of the content of this file is permitted  //
//  as long as the above statement is kept, unchanged, at its original      //
//  location.																//
//																			//
//////////////////////////////////////////////////////////////////////////////

//If your project does not use a precompiled header, comment the first line and uncomment the next ones
#include "defines.h"
//#define _WIN32_WINNT 0x0503
//#include <windows.h>
//#include <richedit.h>

//Library header file (required)
#include "RichEditThemed.h"

//This STL map is used to keep track of all the instances of the class
std::map<HWND,CRichEditThemed*> CRichEditThemed::m_aInstances;

//The theme-specific functions are imported at runtime for backward compatibility reasons
//A nicer alternative if you are using Visual C++ 7.0 or above is the use of the "delay load" mechanism
HMODULE CRichEditThemed::m_hUxTheme = nullptr;
HTHEME(WINAPI *CRichEditThemed::pOpenThemeData)(HWND, LPCWSTR) = nullptr;
HRESULT(WINAPI *CRichEditThemed::pCloseThemeData)(HTHEME) = nullptr;
HRESULT(WINAPI *CRichEditThemed::pDrawThemeBackground)(HTHEME, HDC, int, int, const RECT*, const RECT *) = nullptr;
HRESULT(WINAPI *CRichEditThemed::pGetThemeBackgroundContentRect)(HTHEME, HDC, int, int, const RECT *, RECT *) = nullptr;
BOOL(WINAPI *CRichEditThemed::pIsThemeActive)() = nullptr;
HRESULT(WINAPI *CRichEditThemed::pDrawThemeParentBackground)(HWND, HDC, RECT*) = nullptr;
BOOL(WINAPI *CRichEditThemed::pIsThemeBackgroundPartiallyTransparent)(HTHEME, int, int) = nullptr;

//
// This function is the one and only public function your program must use
// It needs to be called during the creation/initialisation of the parent window
//////////////////////////////////////////////////////////////////////////////
bool CRichEditThemed::Attach(HWND hRichEdit)
{
	if(IsWindow(hRichEdit)) //It is your responsibility to ensure that the handle parameter is, indeed, a richedit window
	{
		//Prevent double subclassing
		if(CRichEditThemed::m_aInstances.find(hRichEdit) == CRichEditThemed::m_aInstances.end())
		{
			//If this function fails, this version of Windows doesn't support themes
			if(InitLibrary())
			{
				//Note: the object will be automatically deleted when the richedit control dies
				/*CRichEditThemed *obj =*/ new CRichEditThemed(hRichEdit);
				return true;
			}
		}
	}
	return false;
}

//
//////////////////////////////////////////////////////////////////////////////
CRichEditThemed::CRichEditThemed(HWND hRichEdit)
	: m_hRichEdit(hRichEdit)
{
	//Subclass the richedit control, this way, the caller doesn't have to relay the messages by itself
	m_aInstances[hRichEdit] = this;
	m_pOriginalWndProc = SubclassWindow(hRichEdit, &RichEditStyledProc);

	//Check the current state of the richedit control
	//ZeroMemory(&m_rcClientPos, sizeof(RECT));
	VerifyThemedBorderState();
}

//
//////////////////////////////////////////////////////////////////////////////
CRichEditThemed::~CRichEditThemed()
{
	//Unload the UxTheme library if it is not needed anymore by a control
	m_aInstances.erase(m_hRichEdit);
	if(m_aInstances.empty())
	{
		pOpenThemeData =  nullptr;
		pCloseThemeData = nullptr;
		pDrawThemeBackground = nullptr;
		pGetThemeBackgroundContentRect = nullptr;
		pIsThemeActive = nullptr;
		pDrawThemeParentBackground = nullptr;
		pIsThemeBackgroundPartiallyTransparent = nullptr;

		FreeLibrary(m_hUxTheme);
		m_hUxTheme = nullptr;
	}
}

//
//////////////////////////////////////////////////////////////////////////////
bool CRichEditThemed::InitLibrary() noexcept
{
	//Are we already initialised?
	if(pOpenThemeData && pCloseThemeData && pDrawThemeBackground && pGetThemeBackgroundContentRect && pIsThemeActive && pDrawThemeParentBackground && pIsThemeBackgroundPartiallyTransparent)
		return true;

	//Try to get the function pointers of the UxTheme library
	if(m_hUxTheme == nullptr)
	{
		m_hUxTheme = LoadLibrary(_T("UxTheme.dll"));
		if(m_hUxTheme == nullptr)
			return false;
	}

#pragma warning(push)
#pragma warning(disable: 4191)

	pOpenThemeData = (HTHEME (WINAPI *)(HWND, LPCWSTR))GetProcAddress(m_hUxTheme, "OpenThemeData");
	pCloseThemeData = (HRESULT (WINAPI *)(HTHEME))GetProcAddress(m_hUxTheme, "CloseThemeData");
	pDrawThemeBackground = (HRESULT (WINAPI *)(HTHEME, HDC, int, int, const RECT*, const RECT *))GetProcAddress(m_hUxTheme, "DrawThemeBackground");
	pGetThemeBackgroundContentRect = (HRESULT (WINAPI *)(HTHEME, HDC, int, int,  const RECT *, RECT *))GetProcAddress(m_hUxTheme, "GetThemeBackgroundContentRect");
	pIsThemeActive = (BOOL (WINAPI *)())GetProcAddress(m_hUxTheme, "IsThemeActive");
	pDrawThemeParentBackground = (HRESULT (WINAPI *)(HWND, HDC, RECT*))GetProcAddress(m_hUxTheme, "DrawThemeParentBackground");
	pIsThemeBackgroundPartiallyTransparent = (BOOL (WINAPI *)(HTHEME, int, int))GetProcAddress(m_hUxTheme, "IsThemeBackgroundPartiallyTransparent");

#pragma warning(pop)

	return (pOpenThemeData && pCloseThemeData && pDrawThemeBackground && pGetThemeBackgroundContentRect && pIsThemeActive && pDrawThemeParentBackground && pIsThemeBackgroundPartiallyTransparent);
}

//
//////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CRichEditThemed::RichEditStyledProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//This function is the subclassed winproc of the richedit control
	//It is used to monitor the actions of the control, in a nice and transparent manner
	
	if(const auto &itCurInstance = m_aInstances.find(hwnd); itCurInstance != m_aInstances.end())
	{
		//A winproc is always static, this one is common to all the richedit controls managed by this class
		//We need to get a pointer to the object controlling the richedit which is receiving this message
		const auto pObj = itCurInstance->second;
		
		//If you get a compilation error here, it is probably because _WIN32_WINNT is not defined to at least 0x0501
		if(uMsg == WM_THEMECHANGED || uMsg == WM_STYLECHANGED)
		{
			//Someone just changed the style of the richedit control or the user changed its theme
			//Make sure the control is being kept up to date by verifying its state
			pObj->VerifyThemedBorderState();
		}
		else if(uMsg == WM_NCPAINT)
		{
			//Let the control paint its own non-client elements (such as its scrollbars)
			const auto nOriginalReturn = CallWindowProc(pObj->m_pOriginalWndProc, hwnd, uMsg, wParam, lParam);

			//Draw the theme, if necessary
			if(pObj->OnNCPaint())
				return 0;

			return nOriginalReturn;
		}
		else if(uMsg == WM_ENABLE)
		{
			//Redraw the border depending on the state of the richedit control
			RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE|RDW_NOCHILDREN|RDW_UPDATENOW|RDW_FRAME);
		}
		else if(uMsg == WM_NCCALCSIZE)
		{
			//If wParam is FALSE, we don't need to make any calculation
			if(wParam)
			{
				//Ask the control to first calculate the space it needs
				const auto nOriginalReturn = CallWindowProc(pObj->m_pOriginalWndProc, hwnd, uMsg, wParam, lParam);

				//Alter the size for our own border, if necessary
				if(const auto csparam = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam); pObj->OnNCCalcSize(csparam))
					return WVR_REDRAW;

				return nOriginalReturn;
			}
		}
		else if(uMsg == WM_DESTROY)
		{
			////Fail-safe: don't restore the original wndproc pointer if it has been modified since the creation of this object
			//if(IsWindow(hwnd))
			//{
			//	if((WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC) == &RichEditStyledProc)
			//		SubclassWindow(hwnd, pObj->m_pOriginalWndProc);
			//}
			//
			////Send the message now so that we can safely delete the object afterward
			//LRESULT nToReturn = CallWindowProc(pObj->m_pOriginalWndProc, hwnd, uMsg, wParam, lParam);
			//
			//delete pObj;
			//return nToReturn;

			Auto(delete pObj);
			//Fail-safe: don't restore the original wndproc pointer if it has been modified since the creation of this object
			if (IsWindow(hwnd))
			{
				if ((WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC) == &RichEditStyledProc)
					SubclassWindow(hwnd, pObj->m_pOriginalWndProc);
			}

			//Send the message now so that we can safely delete the object afterward (needed here for scoped delete above)
			return CallWindowProc(pObj->m_pOriginalWndProc, hwnd, uMsg, wParam, lParam);
		}
		return CallWindowProc(pObj->m_pOriginalWndProc, hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

//
//////////////////////////////////////////////////////////////////////////////
void CRichEditThemed::VerifyThemedBorderState() noexcept
{
	const bool bCurrentThemedBorder = m_bThemedBorder;
	m_bThemedBorder = false;

	const auto dStyle = GetWindowStyle(m_hRichEdit);
	const auto dExStyle = GetWindowExStyle(m_hRichEdit);

	//First, check if the control is supposed to have a border
	if(bCurrentThemedBorder || (dStyle & WS_BORDER || dExStyle & WS_EX_CLIENTEDGE))
	{
		//Check if a theme is presently active
		if(pIsThemeActive())
		{
			//Remove the border style, we don't want the control to draw its own border
			m_bThemedBorder = true;
			if(dStyle & WS_BORDER)
				SetWindowLong(m_hRichEdit, GWL_STYLE, static_cast<long>(dStyle ^WS_BORDER));
			if(dExStyle & WS_EX_CLIENTEDGE)
				SetWindowLong(m_hRichEdit, GWL_EXSTYLE, static_cast<long>(dExStyle ^WS_EX_CLIENTEDGE));
		}
	}

	//Recalculate the NC area and repaint the window
	SetWindowPos(m_hRichEdit, nullptr, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);
	RedrawWindow(m_hRichEdit, nullptr, nullptr, RDW_INVALIDATE|RDW_NOCHILDREN|RDW_UPDATENOW|RDW_FRAME);
}

//
//////////////////////////////////////////////////////////////////////////////
bool CRichEditThemed::OnNCPaint() noexcept
{
	try
	{
		if(m_bThemedBorder)
		{
			if(const auto hTheme = pOpenThemeData(m_hRichEdit, VSCLASS_EDIT); hTheme != nullptr)
			{
				Auto(pCloseThemeData(hTheme));

				if (const auto hdc = GetWindowDC(m_hRichEdit); hdc != nullptr)
				{
					Auto(ReleaseDC(m_hRichEdit, hdc));

					//Clip the DC so that we only draw on the non-client area
					RECT rcBorder;
					GetWindowRect(m_hRichEdit, &rcBorder);
					rcBorder.right -= rcBorder.left; rcBorder.bottom -= rcBorder.top;
					rcBorder.left = rcBorder.top = 0;

					RECT rcClient;
					memcpy(&rcClient, &rcBorder, sizeof(RECT));
					rcClient.left += m_rcClientPos.left;
					rcClient.top += m_rcClientPos.top;
					rcClient.right -= m_rcClientPos.right;
					rcClient.bottom -= m_rcClientPos.bottom;
					ExcludeClipRect(hdc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

					//Make sure the background is in a proper state
					if (pIsThemeBackgroundPartiallyTransparent(hTheme, EP_EDITTEXT, ETS_NORMAL))
						pDrawThemeParentBackground(m_hRichEdit, hdc, &rcBorder);

					//Draw the border of the edit box
					int nState = ETS_NORMAL;

					if (!IsWindowEnabled(m_hRichEdit))
						nState = ETS_DISABLED;
					else if (SendMessage(m_hRichEdit, EM_GETOPTIONS, NULL, NULL) & ECO_READONLY)
						nState = ETS_READONLY;

					pDrawThemeBackground(hTheme, hdc, EP_EDITTEXT, nState, &rcBorder, nullptr);

					return true;
				}
			}
		}
	}
	catch(...)
	{
		//No exception is supposed to be thrown here but you can never be too safe
		//Trace an error here with your favorite TRACE macro
	}
	return false;
}

//
//////////////////////////////////////////////////////////////////////////////
bool CRichEditThemed::OnNCCalcSize(NCCALCSIZE_PARAMS *csparam) noexcept
{
	try
	{
		//Here, we indicate to Windows that the non-client area of the richedit control is not what it thinks it should be
		//This gives us the necessary space to draw the special border later on
		if(m_bThemedBorder)
		{
			//Load the theme associated with edit boxes
			if(const auto hTheme = pOpenThemeData(m_hRichEdit, VSCLASS_EDIT); hTheme != nullptr)
			{
				Auto(pCloseThemeData(hTheme));

				//Get the size required by the current theme to be displayed properly
				if (const auto hdc = GetDC(GetParent(m_hRichEdit)); hdc != nullptr)
				{
					Auto(ReleaseDC(GetParent(m_hRichEdit), hdc));
					
					if (RECT rcClient{}; pGetThemeBackgroundContentRect(hTheme, hdc, EP_EDITTEXT, ETS_NORMAL, &csparam->rgrc[0], &rcClient) == S_OK)
					{
						//Add a pixel to every edge so that the client area is not too close to the border drawn by the theme (thus simulating a native edit box)
						InflateRect(&rcClient, -1, -1);

						m_rcClientPos.left = rcClient.left - csparam->rgrc[0].left;
						m_rcClientPos.top = rcClient.top - csparam->rgrc[0].top;
						m_rcClientPos.right = csparam->rgrc[0].right - rcClient.right;
						m_rcClientPos.bottom = csparam->rgrc[0].bottom - rcClient.bottom;

						memcpy(&csparam->rgrc[0], &rcClient, sizeof(RECT));
						return true;
					}
				}
			}
		}

	}
	catch(...)
	{
		//No exception is supposed to be thrown here but you can never be too safe
		//Trace an error here with your favorite TRACE macro
	}
	return false;
}
