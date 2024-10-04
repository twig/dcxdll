#include "defines.h"
#include "json.h"

Json::Json(const Json& other)
{
	m_tsLabel = other.m_tsLabel;
	if (other.m_Data)
		m_Data = std::make_unique<Json>(*other.m_Data);
	//m_Sibling.reset(nullptr);
	if (other.m_Sibling)
		m_Sibling = std::make_unique<Json>(*other.m_Sibling);
}

Json::Json(Json&& other) noexcept
{
	std::swap(m_tsLabel, other.m_tsLabel);
	std::swap(m_Data, other.m_Data);
	std::swap(m_Sibling, other.m_Sibling);
}

Json& Json::operator=(const Json& other)
{
	if (this == &other)
		return *this;

	m_tsLabel = other.m_tsLabel;
	if (other.m_Data)
		m_Data = std::make_unique<Json>(*other.m_Data);
	if (other.m_Sibling)
		m_Sibling = std::make_unique<Json>(*other.m_Sibling);

	return *this;
}

Json& Json::operator=(Json&& other) noexcept
{
	std::swap(m_tsLabel, other.m_tsLabel);
	std::swap(m_Data, other.m_Data);
	std::swap(m_Sibling, other.m_Sibling);

	return *this;
}

const TCHAR *Json::parse(const TCHAR* pszJson, bool bData)
{
	clear();

	size_t uDepth{};

	for (; *pszJson; ++pszJson)
	{
		switch (const auto c = *pszJson; c)
		{
		case L'[':
		{
			// start of array
		}
		break;
		case L']':
		{
			// end of array
		}
		break;
		case L'{':
		{
			// start of object
			++uDepth;
			bData = false;
		}
		break;
		case L'}':
		{
			// end of object
			--uDepth;
			if (uDepth == 0)
			{
				//m_tsLabel.trim();
				_ts_trim(m_tsLabel);
				return pszJson;
			}
		}
		break;
		case L',':
		{
			// next object

			//m_tsLabel.trim();
			_ts_trim(m_tsLabel);

			if (bData)
				return --pszJson;

			//m_Sibling.reset(new Json);
			m_Sibling = std::make_unique<Json>();
			return m_Sibling->parse(++pszJson, false);
		}
		break;
		case L':':
		{
			// change from label to data.
			//m_tsLabel.trim();
			_ts_trim(m_tsLabel);
			//m_Data.reset(new Json);
			m_Data = std::make_unique<Json>();
			pszJson = m_Data->parse(++pszJson, true);
			if (!*pszJson)
				return pszJson;
		}
		break;
		case L'"':
			break;
		default:
		{
			m_tsLabel += c;
		}
		break;
		}
	}
	//m_tsLabel.trim();
	_ts_trim(m_tsLabel);

	return pszJson;
}

const Json& Json::at(const TCHAR* szLabel) const noexcept
{
	return const_cast<Json*>(this)->at(szLabel);
}

Json& Json::at(const TCHAR* szLabel) noexcept
{
	static Json jBlank;

	if (m_tsLabel == szLabel)
	{
		if (m_Data)
			return *m_Data;
		return jBlank;
	}
	if (m_Data)
	{
		if (auto& tmp = m_Data->at(szLabel); tmp)
			return tmp;
	}
	if (m_Sibling)
		return m_Sibling->at(szLabel);

	return jBlank;
}

const Json& Json::at(int iOffset) const noexcept
{
	return at(0, iOffset);
}

Json& Json::at(int iOffset) noexcept
{
	return at(0, iOffset);
}

const Json& Json::at(int iOffset, int iMatch) const noexcept
{
	return const_cast<Json*>(this)->at(iOffset, iMatch);
}

Json& Json::at(int iOffset, int iMatch) noexcept
{
	static Json jBlank;

	if (iOffset == iMatch)
	{
		if (m_Data)
			return *m_Data;
		return jBlank;
	}
	if (m_Data)
	{
		if (auto& tmp = m_Data->at(++iOffset, iMatch); tmp)
			return tmp;
	}
	if (m_Sibling)
		return m_Sibling->at(++iOffset, iMatch);

	return jBlank;
}
