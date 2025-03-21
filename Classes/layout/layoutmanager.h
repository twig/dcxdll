/*!
 * \file layoutmanager.h
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

#ifndef _LAYOUTMANAGER_H_
#define _LAYOUTMANAGER_H_

#include "Classes/layout/layoutcellfill.h"
#include "Classes/layout/layoutcellfixed.h"
#include "Classes/layout/layoutcellpane.h"
#include "Classes/tstring/tstring.h"

/*!
 * \brief blah
 *
 * blah
 */
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class LayoutManager final
{
public:

	LayoutManager() = default;
	~LayoutManager() = default;

	/// <summary>
	/// Constructor to handle the CLA for a specified window.
	/// </summary>
	/// <param name="mHwnd"></param>
	explicit LayoutManager( HWND mHwnd ) noexcept;

	LayoutManager(const LayoutManager &) = delete;
	LayoutManager &operator =(const LayoutManager &) = delete;	// No assignments!
	LayoutManager(LayoutManager &&) = delete;
	LayoutManager &operator =(LayoutManager &&) = delete;

	/// <summary>
	/// Update the CLA
	/// </summary>
	/// <param name="rc"></param>
	/// <returns></returns>
	const bool updateLayout(RECT & rc);

	/// <summary>
	/// Set the root cell.
	/// </summary>
	/// <param name="p_Root"></param>
	void setRoot(std::unique_ptr<LayoutCell> p_Root) noexcept;

	/// <summary>
	/// Get the root cell.
	/// </summary>
	/// <returns></returns>
	LayoutCell * getRoot() const noexcept { return m_pRoot.get(); }

	/// <summary>
	/// Get a cell based on a supplied path.
	/// </summary>
	/// <param name="path"></param>
	/// <returns>The LayoutCell requested or nullptr</returns>
	LayoutCell * getCell( const TString & path ) const;

	/// <summary>
	/// Add a LayoutCell.
	/// </summary>
	/// <param name="input"></param>
	/// <param name="iOffset"></param>
	/// <param name="dialog"></param>
	void AddCell(const TString &input, const UINT iOffset = 3, const DcxDialog * dialog = nullptr);

	/// <summary>
	/// Get the number of cells.
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	inline constexpr const size_t &size(void) const noexcept { return m_iCount; }

	/// <summary>
	/// Is the root cell set?
	/// </summary>
	/// <param name=""></param>
	/// <returns>true/false</returns>
	inline bool empty(void) const noexcept { return (m_pRoot == nullptr); }

	/// <summary>
 	/// Get a cell based on a supplied path.
	/// </summary>
	/// <param name="path"></param>
	/// <param name="hParent"></param>
	/// <param name="depth"></param>
	/// <returns></returns>
	static LayoutCell * parsePath(const TString & path, const LayoutCell *const hParent, const UINT depth);

	/// <summary>
	/// Convert a flags string into CLATypes
	/// </summary>
	/// <param name="flags"></param>
	/// <returns></returns>
	static const CLATypes parseLayoutFlags(const TString & flags) noexcept;

	// this is here for reference only
	//typedef class std::vector<LayoutCell *>::iterator iterator;
	//typedef class std::vector<LayoutCell *>::const_iterator const_iterator;
	//
	//iterator begin() { return m_data.begin(); }
	//const_iterator begin() const { return m_data.begin(); }
	//const_iterator cbegin() const { return m_data.cbegin(); }
	//iterator end() { return m_data.end(); }
	//const_iterator end() const { return m_data.end(); }
	//const_iterator cend() const { return m_data.cend(); }

protected:

	HWND m_Hwnd{ nullptr }; //!< Dialog Window Handle

	std::unique_ptr<LayoutCell> m_pRoot{ nullptr }; //!< Root LayoutCell Element
	size_t		m_iCount{};
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _LAYOUTMANAGER_H_
