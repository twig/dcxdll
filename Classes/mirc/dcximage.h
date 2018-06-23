/*!
 * \file dcximage.h
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

#ifndef _DCXIMAGE_H_
#define _DCXIMAGE_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxImage
	: public DcxControl
{
public:
	DcxImage() = delete;
	DcxImage(const DcxImage &) = delete;
	DcxImage &operator =(const DcxImage &) = delete;
	DcxImage(DcxImage &&) = delete;
	DcxImage &operator =(DcxImage &&) = delete;

	DcxImage( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxImage( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) noexcept final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	//void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) final;
	std::tuple<NoTheme, WindowStyle, WindowExStyle> parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("image"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::IMAGE; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement *toXml() const final;

	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
	static WNDPROC m_hDefaultClassProc;	//!< Default window procedure

private:
#ifdef DCX_USE_GDIPLUS
	std::unique_ptr<Gdiplus::Image> m_pImage{ nullptr }; //!< GDI+ Image Object
	Gdiplus::CompositingMode m_CMode{ Gdiplus::CompositingModeSourceCopy }; // Compositing Mode
	Gdiplus::CompositingQuality m_CQuality{ Gdiplus::CompositingQualityDefault }; // Compositing Quality
	Gdiplus::InterpolationMode m_IMode{ Gdiplus::InterpolationModeDefault }; // Interpolation Mode
	Gdiplus::SmoothingMode m_SMode{ Gdiplus::SmoothingModeDefault }; // Smoothing Mode

	bool LoadGDIPlusImage(const TString &flags, TString &filename);
	void DrawGDIImage(HDC hdc, int x, int y, int w, int h);
#endif
	void DrawBMPImage(HDC hdc, const int x, const int y, const int w, const int h);
	void DrawClientArea(HDC hdc);

	// clears existing image and icon data and sets pointers to null
	void PreloadData(void) noexcept;

	HBITMAP m_hBitmap{ nullptr }; //!< Bitmap
	HICON m_hIcon{ nullptr }; // !< icon

	COLORREF m_clrTransColor{ CLR_INVALID }; //!< Transparent color

	DcxIconSizes m_iIconSize{ DcxIconSizes::SmallIcon };
	bool m_bResizeImage{ true };	//!< Resize Image
	bool m_bTileImage{ false };		//!< Tile Image
	bool m_bBuffer{ false };		//!< Double Buffer Rendering, needed for GDI+ when WS_EX_COMPOSITED
	bool m_bIsIcon{ false };		//!< Is this an icon?
	int m_iXOffset{}, m_iYOffset{};	//!< X & Y image offsets.
	TString m_tsFilename;			//!< The loaded images filename.
};

#endif // _DCXIMAGE_H_
