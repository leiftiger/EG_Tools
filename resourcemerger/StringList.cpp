#include "StringList.h"

#include "utils.h"

void StringList::addInclude(const std::string &include)
{
	m_includes.push_back(include);
}

void StringList::removeInclude(const std::string &include)
{
	bool found = false;

	for (size_t i = 0; i < (m_includes.size() - 1); i++)
	{
		if (found || m_includes[i] == include)
		{
			found = true;
			m_includes[i] = m_includes[i + 1];
		}
	}

	if (found || m_includes.back() == include)
		m_includes.pop_back();
}

const std::vector<std::string> &StringList::includes() const
{
	return m_includes;
}

void StringList::set(const std::string &key, const std::string &value)
{
	if (m_map.find(key) == m_map.end())
	{
		m_keys.push_back(key);
	}

	m_map[key] = value;
}

const std::string &StringList::get(const std::string &key) const
{
	if (m_map.find(key) == m_map.end())
		throw std::runtime_error(("Key not found: \"" + key + "\"").c_str());

	return m_map.at(key);
}

bool StringList::contains(const std::string &key) const
{
	return (m_map.find(key) != m_map.end());
}

void StringList::remove(const std::string &key)
{
	if (m_map.find(key) != m_map.end())
	{
		m_map.erase(key);

		for (std::size_t i = 0; i < m_keys.size(); i++)
		{
			std::string &curKey = m_keys[i];

			if (curKey == key)
			{
				for (std::size_t i2 = i; i2 < m_keys.size() - 1; i2++)
				{
					m_keys[i2] = m_keys[i2 + 1];
				}

				m_keys.pop_back();

				break;
			}
		}
	}
}


const std::vector<std::string> &StringList::keys() const
{
	return m_keys;
}

bool StringList::isNumeric(const std::string &str)
{
	for (char c : str)
	{
		if (!isdigit(c))
			return false;
	}

	return true;
}

std::istream &operator>>(std::istream &stream, StringList &list)
{
	std::string line;

	std::string prevLine;

	bool tagOpen = false;

	std::string tagKey;
	std::string tagValue;

	while (util::getline(stream, line))
	{
		if (line.length() < 2)
		{
			prevLine = line;
			continue;
		}

		// Comment
		if (line.substr(0, 2) == "##")
		{
			prevLine = line;
			continue;
		}

		if (line.length() >= 8 && line.substr(0, 8) == "#include")
		{
			std::string filename = line.substr(strlen("#include "));

			list.addInclude(filename);
		}

		if (tagOpen)
		{
			if (line.length() >= 5 && line.substr(0, 5) == "<END>")
			{
				list.set(tagKey, tagValue);
				tagOpen = false;
			}
			else
			{
				tagValue += line + "\n";
			}
		}

		if (line.length() >= 7 && line.substr(0, 7) == "<BEGIN>")
		{
			tagOpen = true;
			tagKey = prevLine;
			tagValue = "";
		}

		prevLine = line;
	}

	return stream;
}

std::ostream &operator<<(std::ostream &stream, const StringList &list)
{
	stream << "## File merged by Mod Manager" << std::endl;
	stream << "## Original comments can be found in the base or mod file" << std::endl;

	if (list.includes().size() > 0)
		stream << std::endl;

	for (const std::string &includeFile : list.includes())
	{
		stream << "#include " << includeFile << std::endl;
	}

	for (const std::string &key : list.keys())
	{
		const std::string &value = list.get(key);

		stream << std::endl;
		stream << key << std::endl;
		stream << "<BEGIN>" << std::endl;
		stream << value; // newline included in value
		stream << "<END>" << std::endl;
	}

	return stream;
}