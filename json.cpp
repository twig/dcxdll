#include "defines.h"
#include "json.h"

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
				m_tsLabel.trim();
				return pszJson;
			}
		}
		break;
		case L',':
		{
			// next object

			m_tsLabel.trim();

			if (bData)
				return --pszJson;

			m_Sibling.reset(new Json);
			return m_Sibling->parse(++pszJson, false);
		}
		break;
		case L':':
		{
			// change from label to data.
			m_tsLabel.trim();
			m_Data.reset(new Json);
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
	m_tsLabel.trim();
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
