/*!
 * \file dcxtrackbar.h
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

#ifndef _DCXTRACKBAR_H_
#define _DCXTRACKBAR_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

#define TBCS_BACK		0x01	// background
#define TBCS_THUMB		0x02	// thumb normal
#define TBCS_THUMBDRAG	0x04	// thumb dragged
#define TBCS_CHANNEL	0x08	// track

 //#define TBBMP_BACK		0
 //#define TBBMP_THUMB		1
 //#define TBBMP_THUMBDRAG	2
 //#define TBBMP_CHANNEL		3

enum class TrackBarParts
	: UINT
{
	TBBMP_BACK,
	TBBMP_THUMB,
	TBBMP_THUMBDRAG,
	TBBMP_CHANNEL
};
class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxTrackBar final
	: public DcxControl
{
public:
	DcxTrackBar() = delete;
	DcxTrackBar(const DcxTrackBar&) = delete;
	GSL_SUPPRESS(c.128) DcxTrackBar& operator =(const DcxTrackBar&) = delete;	// No assignments!
	DcxTrackBar(DcxTrackBar&&) = delete;
	GSL_SUPPRESS(c.128) DcxTrackBar& operator =(DcxTrackBar&&) = delete;

	DcxTrackBar(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxTrackBar() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	static UINT parseImageFlags(const TString& flags) noexcept;

	LRESULT setRangeMin(const LONG iLowLim) noexcept;
	LRESULT getRangeMin() const noexcept;
	LRESULT setRangeMax(const LONG iHighLim) noexcept;
	LRESULT getRangeMax() const noexcept;
	LRESULT setRange(const LONG iLowLim, const LONG iHighLim) noexcept;
	LRESULT setPos(const LONG lPosition) noexcept;
	LRESULT getPos() const noexcept;
	void setTic(const LONG lPosition) noexcept;
	void setTicFreq(const LONG wFreq) noexcept;
	const LONG& getTickFreq() const noexcept { return m_iTickFreq; };
	LRESULT clearTics() noexcept;
	int setTipSide(const int fLocation) noexcept;
	int getTipSide() const noexcept;
	LRESULT setPageSize(const LONG lPageSize) noexcept;
	LRESULT getPageSize() const noexcept;
	LRESULT setLineSize(const LONG lLineSize) noexcept;
	LRESULT getLineSize() const noexcept;
	LRESULT setThumbLength(const UINT iLength) noexcept;
	UINT getThumbLength() const noexcept;
	void setSel(const LONG iLowLim, const LONG iHighLim) noexcept;
	LRESULT getSelStart() const noexcept;
	LRESULT getSelEnd() const noexcept;
	HWND setBuddy(const HWND mHwnd, bool bLeft) noexcept;
	HWND getBuddy(bool bLeft) const noexcept;

	inline const TString getType() const final { return TEXT("trackbar"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::TRACKBAR; }

	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml(void) const final;
	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam)  noexcept final;

private:
	bool DrawTrackBarPart(HDC hdc, const TrackBarParts iPartId, const RECT* const rc = nullptr);

	HBITMAP m_hbmp[sizeof(TrackBarParts)]{ nullptr }; // refer to TBBMP_*
	COLORREF m_colTransparent{ CLR_INVALID };
	LONG m_iTickFreq{ 1 };
	bool m_bUpdatingTooltip{ false };
	bool m_bReserved[3]{ false };
};

#endif // _DCXTRACKBAR_H_
