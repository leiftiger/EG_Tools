#pragma once

#include <cstdint>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// A pack of files from a stored ERB file
class ResourcePack
{
public:
	class Entry
	{
	public:
		std::uint32_t m_size;
		std::uint32_t m_offset;

		std::uint32_t m_unknown1;
		std::uint32_t m_unknown2;
	};
protected:

	std::unordered_map<std::string, Entry> m_files;

	// Files stored in this resource pack
	std::vector<std::string> m_filenames;

	// Finds the full file path from just the filename
	std::unordered_map<std::string, std::string> m_expandedFilenames;

	// Name of package file
	std::string m_filename;

public:

	ResourcePack();
	ResourcePack(const std::string &filename);

	// Reads a type T in little-endian format from the given stream
	template <class T>
	T read(std::istream &stream)
	{
		char data[sizeof(T)/sizeof(char)];

		stream.read(data, sizeof(data) / sizeof(char));

		T ret = 0;

		int pow = 0;

		for (size_t i = 0; i < sizeof(data) / sizeof(char); i++)
		{
			ret |= ((T)(std::uint8_t)data[i]) << pow*8;

			pow++;
		}

		return ret;
	}

	// Returns true if the resource pack contains the given filename
	bool contains(const std::string &filename) const;

	// Opens a file for reading
	std::istream *openFile(const std::string &filename) const;

	// Retrieves a list of all filenames stored in this resource pack
	const std::vector<std::string> &files() const;

	friend std::istream &operator>>(std::istream &stream, ResourcePack &list);
	friend std::ostream &operator<<(std::ostream &stream, ResourcePack &list);
};

// Specialization for reading NUL terminated strings
template <>
inline std::string ResourcePack::read<std::string>(std::istream &stream)
{
	char cur;

	std::string str;

	while (true)
	{
		stream.read(&cur, 1);

		if (cur == '\0')
			break;

		str += cur;
	}

	return str;
}
