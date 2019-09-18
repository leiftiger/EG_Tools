#include "ResourcePack.h"

ResourcePack::ResourcePack()
{

}

ResourcePack::ResourcePack(const std::string &filename) : m_filename(filename)
{
	std::ifstream file(filename, std::ios::binary);

	if (!file.good())
	{
		throw std::exception(("Couldn't open \"" + filename + "\"").c_str());
	}

	file >> *this;
}

std::istream *ResourcePack::openFile(const std::string &filename) const
{
	std::string erbFileName = filename;

	if (m_files.find(erbFileName) == m_files.end())
	{
		// The full path might not have been specified
		if (m_expandedFilenames.find(erbFileName) != m_expandedFilenames.end())
		{
			erbFileName = m_expandedFilenames.at(erbFileName);
		}
		else
		{

			// Files in the main directory for some reason don't have ./ as the lead path, so we check if it was such a file
			erbFileName = erbFileName.substr(2);

			if (m_files.find(erbFileName) == m_files.end())
			{
				throw std::exception(("File \"" + filename + "\" is not present in resource pack").c_str());
			}
		}
	}

	std::ifstream file(m_filename, std::ios::binary);

	if (!file.good())
	{
		throw std::exception(("Couldn't open resource pack \"" + m_filename + "\" for reading").c_str());
	}

	const Entry &entry = m_files.at(erbFileName);

	std::stringstream *stream = new std::stringstream();

	char *buffer = new char[entry.m_size];

	file.seekg(entry.m_offset);
	file.read(buffer, entry.m_size);

	stream->write(buffer, entry.m_size);

	delete[] buffer;

	return stream;
}

const std::vector<std::string> &ResourcePack::files() const
{
	return m_filenames;
}


std::istream &operator>>(std::istream &stream, ResourcePack &list)
{
	char header[4];

	std::uint32_t erbVersion, dirOffset, numFiles;

	stream.read(header, sizeof(header) / sizeof(char));

	if (header[0] != 'K' || header[1] != 'C' || header[2] != 'A' || header[3] != 'P')
		throw std::exception("Not an ERB file - missing \"KCAP\" header");

	erbVersion = list.read<std::uint32_t>(stream);

	if (erbVersion != 2)
		throw std::exception("This ERB Packer can only read ERB version 2");

	dirOffset = list.read<std::uint32_t>(stream);

	stream.seekg(dirOffset, std::ios::beg);

	numFiles = list.read<std::uint32_t>(stream);

	for (std::uint32_t i = 0; i < numFiles; i++)
	{
		ResourcePack::Entry entry;

		std::string entryName = list.read<std::string>(stream);

		std::string entryShortName = entryName.substr(entryName.find_last_of('/') + 1);

		entry.m_size = list.read<std::uint32_t>(stream);
		entry.m_offset = list.read<std::uint32_t>(stream);

		stream.ignore(4);

		entry.m_unknown1 = list.read<std::uint32_t>(stream);
		entry.m_unknown2 = list.read<std::uint32_t>(stream);

		list.m_files[entryName] = entry;

		list.m_filenames.push_back(entryName);

		list.m_expandedFilenames[entryShortName] = entryName;
	}

	return stream;
}

std::ostream &operator<<(std::ostream &stream, ResourcePack &list)
{
	return stream;
}