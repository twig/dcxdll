#pragma once

/*
	* SubClass mIRC windows for docking with new layout engine.
*/

#include "defines.h"

#define SB_MAX_PARTSD 256

enum class DockTypes
	: UINT
{
	DOCK_TYPE_SWITCH,
	DOCK_TYPE_TOOL,
	DOCK_TYPE_TREE,
	DOCK_TYPE_MDI
};

//#define DOCK_TYPE_SWITCH	0x00
//#define DOCK_TYPE_TOOL		0x01
//#define DOCK_TYPE_TREE		0x02
//#define DOCK_TYPE_MDI		0x03

enum class DockFlags
	: UINT
{
	DOCKF_NONE = 0x00,
	DOCKF_NORMAL = 0x001,			//!< No special flags.
	DOCKF_AUTOH = 0x002,			//!< Auto Horizontal size.
	DOCKF_AUTOV = 0x004,			//!< Auto Vertical size.
	DOCKF_SIZE = 0x008,				//!< Auto Horizontal & Vertical size.
	DOCKF_LEFT = 0x010,				//!< Dock to left. (UltraDock)
	DOCKF_RIGHT = 0x020,			//!< Dock to right. (UltraDock)
	DOCKF_TOP = 0x040,				//!< Dock to top. (UltraDock)
	DOCKF_BOTTOM = 0x080,			//!< Dock to bottom. (UltraDock)
	DOCKF_NOSCROLLBARS = 0x100,		//!< Disable parents scrollbars.
	DOCKF_SHOWSCROLLBARS = 0x200,	//!< Stop the auto-sized window from covering the scrollbars of its parent.
	DOCKF_UNDERSTATIC = 0x400,		//!< Keep the docked window under the parents static control. (auto sized to match width of static)
	DOCKF_ABOVESTATIC = 0x800		//!< Keep the docked window above the parents static control. (auto sized to match width of static)
};
//template <typename T>
//constexpr DockFlags& operator |=(DockFlags& eStyle, const T& dStyle) noexcept
//{
//	return eStyle = static_cast<DockFlags>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
//}
//template <typename T>
//constexpr DockFlags& operator &=(DockFlags& eStyle, const T& dStyle) noexcept
//{
//	return eStyle = static_cast<DockFlags>(static_cast<DWORD>(eStyle)& static_cast<DWORD>(dStyle));
//}
//template <typename T>
//constexpr DockFlags operator &(const DockFlags& eStyle, const T& dStyle) noexcept
//{
//	return static_cast<DockFlags>(static_cast<DWORD>(eStyle)& static_cast<DWORD>(dStyle));
//}
//template <typename T>
//constexpr DockFlags operator |(const DockFlags& eStyle, const T& dStyle) noexcept
//{
//	return static_cast<DockFlags>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
//}
//constexpr DockFlags operator ~(const DockFlags& eStyle) noexcept
//{
//	return static_cast<DockFlags>(~static_cast<DWORD>(eStyle));
//}

//#define DOCKF_NORMAL					0x001	//!< No special flags.
//#define DOCKF_AUTOH						0x002	//!< Auto Horizontal size.
//#define DOCKF_AUTOV						0x004	//!< Auto Vertical size.
//#define DOCKF_SIZE						0x008	//!< Auto Horizontal & Vertical size.
//#define DOCKF_LEFT						0x010	//!< Dock to left. (UltraDock)
//#define DOCKF_RIGHT						0x020	//!< Dock to right. (UltraDock)
//#define DOCKF_TOP						0x040	//!< Dock to top. (UltraDock)
//#define DOCKF_BOTTOM					0x080	//!< Dock to bottom. (UltraDock)
//#define DOCKF_NOSCROLLBARS				0x100	//!< Disable parents scrollbars.
//#define DOCKF_SHOWSCROLLBARS			0x200	//!< Stop the auto-sized window from covering the scrollbars of its parent.

//#define TREEBAR_COLOUR_SELECTED			0
//#define TREEBAR_COLOUR_SELECTED_BKG		1
//#define TREEBAR_COLOUR_MESSAGE			2
//#define TREEBAR_COLOUR_MESSAGE_BKG		3
//#define TREEBAR_COLOUR_EVENT			4
//#define TREEBAR_COLOUR_EVENT_BKG		5
//#define TREEBAR_COLOUR_HIGHLIGHT		6
//#define TREEBAR_COLOUR_HIGHLIGHT_BKG	7
//#define TREEBAR_COLOUR_HOT_TEXT			8
//#define TREEBAR_COLOUR_HOT_BKG			9
//#define TREEBAR_COLOUR_MAX				9

enum class TreeBarColours
	: UINT
{
	TREEBAR_COLOUR_SELECTED,
	TREEBAR_COLOUR_SELECTED_BKG,
	TREEBAR_COLOUR_MESSAGE,
	TREEBAR_COLOUR_MESSAGE_BKG,
	TREEBAR_COLOUR_EVENT,
	TREEBAR_COLOUR_EVENT_BKG,
	TREEBAR_COLOUR_HIGHLIGHT,
	TREEBAR_COLOUR_HIGHLIGHT_BKG,
	TREEBAR_COLOUR_HOT_TEXT,
	TREEBAR_COLOUR_HOT_BKG,
	TREEBAR_COLOUR_MAX = gsl::narrow_cast<UINT>(TreeBarColours::TREEBAR_COLOUR_HOT_BKG)
};

struct DCXULTRADOCK final
{
	HWND hwnd{ nullptr };
	DockFlags flags{};
	WindowStyle old_styles{};
	WindowExStyle old_exstyles{};
	RECT rc{};
};
using LPDCXULTRADOCK = DCXULTRADOCK*;

using VectorOfDocks = std::vector<LPDCXULTRADOCK>;

struct SB_PARTINFOD final
{
	HWND		m_Child{ nullptr };
	TString		m_Text;
	int			m_iIcon{ -1 };
	COLORREF	m_TxtCol{ CLR_INVALID };	// colour of the text be default in this item.
	HBRUSH		m_BkgCol{ nullptr };	// brush to be used when drawing the bkg in this item.
};
using LPSB_PARTINFOD = SB_PARTINFOD*;

using VectorOfDParts = std::vector<LPSB_PARTINFOD>;

//typedef struct tagCursor_Data {
//	HCURSOR	m_hCursor;
//	bool	m_bNoDestroy;
//} Cursor_Data, *LPCursor_Data;

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class DcxDock final
{
public:
	DcxDock() = delete;	// no default constructor
	DcxDock(const DcxDock& other) = delete;	// no copy constructor
	DcxDock& operator =(const DcxDock&) = delete;	// No assignments!
	DcxDock(DcxDock&&) = delete;
	DcxDock& operator =(DcxDock&&) = delete;

	DcxDock(HWND refHwnd, HWND dockHwnd, const DockTypes dockType) noexcept;
	~DcxDock() noexcept;

	bool DockWindow(HWND hwnd, const TString& flag);
	void UnDockWindow(const HWND hwnd);
	void UnDockWindowPtr(const gsl::owner<LPDCXULTRADOCK> ud) noexcept;
	void UnDockAll() noexcept;
	void UpdateLayout() const noexcept { SendMessage(this->m_hParent, WM_SIZE, 0, 0); };
	bool FindDock(const HWND hwnd) const;
	bool isDocked(const HWND hwnd) const;
	LPDCXULTRADOCK GetDock(const HWND hwnd) const;
	void AdjustRect(WINDOWPOS* wp) noexcept;

	// Statusbar Functions.
	static bool InitStatusbar(const TString& styles);
	static void UnInitStatusbar() noexcept;
	static bool IsStatusbar() noexcept;
	static std::tuple<NoTheme, WindowStyle, WindowExStyle> status_parseControlStyles(const TString& styles);
	static void status_getRect(LPRECT rc) noexcept;
	static void status_setBkColor(const COLORREF clr) noexcept;
	static void status_setParts(const UINT nParts, const LPINT aWidths) noexcept;
	static int status_getParts(const UINT nParts, const LPINT aWidths) noexcept;
	static void status_setText(const int iPart, const int Style, const WCHAR* const lpstr) noexcept;
	static LRESULT status_getText(const int iPart, LPWSTR lpstr) noexcept;
	static UINT status_getTextLength(const int iPart) noexcept;
	static UINT status_getPartFlags(const int iPart) noexcept;
	static void status_setTipText(const int iPart, const WCHAR* const lpstr) noexcept;
	static void status_getTipText(const int iPart, const int nSize, const LPWSTR lpstr) noexcept;
	static void status_getRect(const int iPart, const LPRECT lprc) noexcept;
	static void status_setIcon(const int iPart, const HICON hIcon) noexcept;
	static HICON status_getIcon(const int iPart) noexcept;
	static HIMAGELIST& status_getImageList() noexcept;
	static void status_setImageList(HIMAGELIST himl) noexcept;
	static HIMAGELIST status_createImageList() noexcept;
	static const UINT status_parseItemFlags(const TString& flags) noexcept;
	static void status_cleanPartIcons() noexcept;
	static bool status_getBorders(const LPINT aWidths) noexcept;
	static void status_updateParts();
	static void status_setFont(HFONT f) noexcept;
	static LRESULT status_setPartInfo(const int iPart, const int Style, const LPSB_PARTINFOD pPart) noexcept;
	static LPSB_PARTINFOD status_getPartInfo(const int iPart) noexcept;
	static void status_deletePartInfo(const int iPart) noexcept;
	//
	static const SwitchBarPos getPos(const int x, const int y, const int w, const int h) noexcept;
	//
	static WORD getTreebarItemWID(const LPARAM lParam) noexcept;
	//
	static TString getTreebarItemType(const LPARAM lParam);
	//static UINT getTreebarChildState(const HTREEITEM hParent, LPTVITEMEX pitem);

	// statusbar stuff
	static inline HWND g_StatusBar{}; //!< The Statusbar for the main mIRC window.
	static inline HIMAGELIST g_hImageList{}; //!< The Statusbar's image list.
	static inline INT g_iDynamicParts[SB_MAX_PARTSD]{};
	static inline INT g_iFixedParts[SB_MAX_PARTSD]{};
	static inline HFONT g_StatusFont{}; //!< Statusbar font.
	static inline VectorOfDParts g_vParts;	//!< Parts info for ownerdraw parts.
	// treebar stuff
	static inline bool g_bTakeOverTreebar{}; //!< take over the drawing of the treebar from mIRC.

	// 0 = selected, 1 = selected bkg, 2 = message, 3 = message bkg
	// 4 = event, 5 = event bkg, 6 = highlight, 7 = highlight bkg
	static inline COLORREF g_clrTreebarColours[gsl::narrow_cast<UINT>(TreeBarColours::TREEBAR_COLOUR_MAX) + 1]{ CLR_INVALID };

	static inline WORD g_wid{};	// currently active wid for the callback alias

protected:
	static LRESULT CALLBACK mIRCRefWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK mIRCDockWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	VectorOfDocks m_VectorDocks; //!< list of windows docked here.
	WNDPROC m_OldRefWndProc{ nullptr }; //!< The Windows Old WndProc.
	WNDPROC m_OldDockWndProc{ nullptr }; //!< The Windows Old WndProc.
	HWND m_RefHwnd{ nullptr }; //!< The HWND that windows are docked around, usually the main child window. This window is subclassed.
	HWND m_hParent{ nullptr }; //!< The HWND that docked windows are docked too. This window is subclassed.
	DockTypes m_iType{}; //!< The dock type.
};

void InitUltraDock(void);
void CloseUltraDock(void) noexcept;
const SwitchBarPos SwitchbarPos(const DockTypes type) noexcept;
void UpdatemIRC(void) noexcept;
void UltraDock(const HWND mWnd, HWND temp, const TString& flag);
void UltraUnDock(const HWND hwnd);
bool FindUltraDock(const HWND hwnd);
LPDCXULTRADOCK GetUltraDock(const HWND hwnd);
void TreebarDock(HWND temp, const TString& flag);
void TreebarUnDock(const HWND hwnd);
bool FindTreebarDock(const HWND hwnd);
LPDCXULTRADOCK GetTreebarDock(const HWND hwnd);
//void UnDock(const HWND hwnd);
BOOL CALLBACK EnumDocked(HWND hwnd, LPARAM lParam);

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop );
#endif
