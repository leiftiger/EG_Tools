#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class StringList
{
protected:

	std::vector<std::string> m_includes;

	std::vector<std::string> m_keys;
	std::unordered_map<std::string, std::string> m_map;

public:

	// Adds an include file to the list
	void addInclude(const std::string &include);

	// Removes an include file to the list
	void removeInclude(const std::string &include);

	// Returns the include files in this list
	const std::vector<std::string> &includes() const;

	// Adds a new key to the list, or sets it if it already exists
	void set(const std::string &key, const std::string &value);

	// Gets the value of the specified key
	const std::string &get(const std::string &key) const;

	// Returns whether or not this list has the specified key
	bool contains(const std::string &key) const;

	// Removes the key from the list
	void remove(const std::string &key);

	// Returns the keys of this list in write order
	const std::vector<std::string> &keys() const;

	// Returns true if the string only contains numeric characters
	static bool isNumeric(const std::string &str);

	friend std::istream &operator>>(std::istream &stream, StringList &list);
	friend std::ostream &operator<<(std::ostream &stream, const StringList &list);
};