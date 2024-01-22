#pragma once

#include <memory>

#include "DcxConcepts.h"
#include "Classes/tstring/tstring.h"

// {label:data,label2:data2....}
// {label:{sublabel:data1,sublabel2:data2}}
// {label:[
//		{sublabel1:data1,sublabel2:data2}
//		{sublabel3:data3,sublabel4:data4}
// ]}

/// <summary>
/// Handle JSON data.
/// NB: all data is represented as a string.
/// </summary>
class Json
{
public:
	using storage_type = TString;
	//using storage_type = std::wstring;

	/// <summary>
	/// Parse a JSON string into objects.
	/// </summary>
	/// <param name="pszJson"></param>
	void parse(const TCHAR* pszJson)
	{
		parse(pszJson, false);
		m_tsLabel.trim();
	}

	/// <summary>
	/// Clear all the stored data.
	/// </summary>
	void clear() noexcept
	{
		m_tsLabel.clear();
		m_Data.reset(nullptr);
		m_Sibling.reset(nullptr);
	}

	/// <summary>
	/// Get the label of the current object.
	/// </summary>
	/// <returns></returns>
	const storage_type& getLabel() const noexcept { return m_tsLabel; }

	/// <summary>
	/// Get the label of the current object.
	/// </summary>
	/// <returns></returns>
	storage_type& getLabel() noexcept { return m_tsLabel; }

	/// <summary>
	/// Get the JSON object that matches the provided label.
	/// </summary>
	/// <param name="szLabel"></param>
	/// <returns></returns>
	const Json& at(const TCHAR* szLabel) const noexcept;

	/// <summary>
	/// Get the JSON object that matches the provided label.
	/// </summary>
	/// <param name="szLabel"></param>
	/// <returns></returns>
	Json& at(const TCHAR* szLabel) noexcept;

	/// <summary>
	/// Get the JSON object at the specified offset.
	/// </summary>
	/// <param name="iOffset"></param>
	/// <returns></returns>
	const Json& at(int iOffset) const noexcept;

	/// <summary>
	/// Get the JSON object at the specified offset.
	/// </summary>
	/// <param name="iOffset"></param>
	/// <returns></returns>
	Json& at(int iOffset) noexcept;

	/// <summary>
	/// Get a const Json object that matches szLabel
	/// </summary>
	/// <param name="szLabel"></param>
	/// <returns></returns>
	const Json& operator[] (const TCHAR* szLabel) const noexcept
	{
		return at(szLabel);
	}

	/// <summary>
	/// Get a const Json object at iOffset
	/// </summary>
	/// <param name="iOffset"></param>
	/// <returns></returns>
	const Json& operator[] (int iOffset) const noexcept
	{
		return at(iOffset);
	}

	/// <summary>
	/// Get a Json object that matches szLabel
	/// </summary>
	/// <param name="szLabel"></param>
	/// <returns></returns>
	Json& operator[] (const TCHAR* szLabel) noexcept
	{
		return at(szLabel);
	}

	/// <summary>
	/// Get a Json object at iOffset
	/// </summary>
	/// <param name="iOffset"></param>
	/// <returns></returns>
	Json& operator[] (int iOffset) noexcept
	{
		return at(iOffset);
	}

	/// <summary>
	/// Replace the current contents with a new Json string.
	/// </summary>
	/// <param name="other"></param>
	/// <returns></returns>
	Json& operator =(const storage_type& other)
	{
		parse(other.data());
		return *this;
	}

	/// <summary>
	/// Convert object into a string.
	/// </summary>
	operator const storage_type& () const noexcept { return m_tsLabel; }
	operator storage_type& () noexcept { return m_tsLabel; }

	/// <summary>
	/// Test if the object is empty
	/// </summary>
	explicit operator bool () const noexcept { return !m_tsLabel.empty(); }

	Json() = default;
	Json(const Json& other) = default;
	Json(Json&& other) noexcept = default;
	bool operator==(const Json& other) const = default;
	GSL_SUPPRESS(c.128) Json& operator =(const Json&) = default;
	GSL_SUPPRESS(c.128) Json& operator =(Json&&) noexcept = default;

	/// <summary>
	/// Create a Json object based on a json string.
	/// </summary>
	/// <param name="pszOther"></param>
	Json(const TCHAR* const pszOther)
	{
		parse(pszOther);
	}
private:
	/// <summary>
	/// Parse a JSON string into objects.
	/// Internal function...
	/// </summary>
	/// <param name="pszJson"></param>
	/// <param name="bData"></param>
	/// <returns></returns>
	const TCHAR* parse(const TCHAR* pszJson, bool bData);

	/// <summary>
	/// Get the JSON data at the specified offset.
	/// Internal functiion...
	/// </summary>
	/// <param name="iOffset"></param>
	/// <param name="iMatch"></param>
	/// <returns></returns>
	const Json& at(int iOffset, int iMatch) const noexcept;

	/// <summary>
	/// Get the JSON data at the specified offset.
	/// Internal functiion...
	/// </summary>
	/// <param name="iOffset"></param>
	/// <param name="iMatch"></param>
	/// <returns></returns>
	Json& at(int iOffset, int iMatch) noexcept;

	storage_type m_tsLabel;
	std::unique_ptr<Json> m_Data;
	std::unique_ptr<Json> m_Sibling;
};
