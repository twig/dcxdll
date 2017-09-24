#pragma once

// Window Styles Helper enums.

enum class WindowStyle : DWORD {
	None
};

template <typename T>
constexpr WindowStyle &operator |=(WindowStyle &eStyle, const T &dStyle)
{
	return eStyle = static_cast<WindowStyle>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowStyle &operator &=(WindowStyle &eStyle, const T &dStyle)
{
	return eStyle = static_cast<WindowStyle>(static_cast<DWORD>(eStyle) & static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowStyle operator &(const WindowStyle &eStyle, const T &dStyle)
{
	return static_cast<WindowStyle>(static_cast<DWORD>(eStyle) & static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowStyle operator |(const WindowStyle &eStyle, const T &dStyle)
{
	return static_cast<WindowStyle>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}

enum class WindowExStyle : DWORD {
	None
};

template <typename T>
constexpr WindowExStyle &operator |=(WindowExStyle &eStyle, const T &dStyle)
{
	return eStyle = static_cast<WindowExStyle>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowExStyle &operator &=(WindowExStyle &eStyle, const T &dStyle)
{
	return eStyle = static_cast<WindowExStyle>(static_cast<DWORD>(eStyle) & static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowExStyle operator &(const WindowExStyle &eStyle, const T &dStyle)
{
	return static_cast<WindowExStyle>(static_cast<DWORD>(eStyle) & static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowExStyle operator |(const WindowExStyle &eStyle, const T &dStyle)
{
	return static_cast<WindowExStyle>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}

enum class WindowAnimStyle : DWORD {
	None
};

template <typename T>
constexpr WindowAnimStyle &operator |=(WindowAnimStyle &eStyle, const T &dStyle)
{
	return eStyle = static_cast<WindowAnimStyle>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowAnimStyle &operator &=(WindowAnimStyle &eStyle, const T &dStyle)
{
	return eStyle = static_cast<WindowAnimStyle>(static_cast<DWORD>(eStyle) & static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowAnimStyle operator &(const WindowAnimStyle &eStyle, const T &dStyle)
{
	return static_cast<WindowAnimStyle>(static_cast<DWORD>(eStyle) & static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowAnimStyle operator |(const WindowAnimStyle &eStyle, const T &dStyle)
{
	return static_cast<WindowAnimStyle>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}
template <typename T>
constexpr WindowAnimStyle operator |(const T &dStyle, const WindowAnimStyle &eStyle)
{
	return static_cast<WindowAnimStyle>(static_cast<DWORD>(dStyle) | static_cast<DWORD>(eStyle));
}

inline WindowStyle dcxGetWindowStyle(HWND Hwnd)
{
	return static_cast<WindowStyle>(GetWindowStyle(Hwnd));
}

inline WindowExStyle dcxGetWindowExStyle(HWND Hwnd)
{
	return static_cast<WindowExStyle>(GetWindowExStyle(Hwnd));
}

inline WindowStyle dcxSetWindowStyle(HWND Hwnd, WindowStyle style)
{
	return static_cast<WindowStyle>(SetWindowLongPtr(Hwnd, GWL_STYLE, static_cast<LONG>(style)));
}

inline WindowExStyle dcxSetWindowExStyle(HWND Hwnd, WindowExStyle style)
{
	return static_cast<WindowExStyle>(SetWindowLongPtr(Hwnd, GWL_EXSTYLE, static_cast<LONG>(style)));
}
