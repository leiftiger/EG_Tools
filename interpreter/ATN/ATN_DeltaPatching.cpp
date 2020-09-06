#include "ATN_Object.h"
#include "ATN_DeltaPatching.h"

namespace ATN
{
	std::vector<int> &DeltaMemory::translations(const std::string &type)
	{
		if (m_lockedTranslations.find(type) != m_lockedTranslations.end())
			throw ATN::Exception("Translations for \"%s\" have been locked elsewhere", type);

		if (m_translations.find(type) == m_translations.end())
			m_translations[type] = std::vector<int>();

		return m_translations[type];
	}

	bool DeltaMemory::isLocked(const std::string &type) const
	{
		return (m_lockedTranslations.find(type) != m_lockedTranslations.end());
	}

	void DeltaMemory::lock(const std::string &type)
	{
		m_lockedTranslations.emplace(type);
	}

	bool DeltaMemory::isChanged(const std::string &type) const
	{
		if (m_translations.find(type) == m_translations.end())
			return false;

		const std::vector<int> &baseTranslations = m_translations.at(type);

		for (int i = 0; i < (int)baseTranslations.size(); i++)
		{
			if (baseTranslations[i] != i)
				return true;
		}

		return false;
	}

	void DeltaMemory::updateTranslationsAfter(const std::string &type, int index, int diff)
	{
		std::vector<int> &baseTranslations = translations(type);

		for (int i = index + 1; i < (int)baseTranslations.size(); i++)
		{
			if (baseTranslations[i] != -2)
				baseTranslations[i] += diff;
		}
	}
}