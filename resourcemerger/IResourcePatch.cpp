#include "IResourcePatch.h"

IResourcePatch::IResourcePatch(const std::string &filename, const std::string &modFilename) : m_filename(filename), m_modFile(modFilename)
{

}

const std::string &IResourcePatch::filename() const
{
	return m_filename;
}

std::vector<std::string> IResourcePatch::apply(const ModPack &mod, std::ostream &out) const
{

	std::istream *modFile = mod.openFile(m_modFile);

	if (modFile->fail())
	{
		return std::vector<std::string>({ "Couldn't open \"" + m_modFile + "\" for reading (mod)" });
	}

	char *buffer = new char[COPY_BUFFER_SIZE];

	std::streamsize bufferLength = 0;

	while (!modFile->eof())
	{
		int len = sizeof(buffer) / sizeof(char);

		modFile->read(buffer, COPY_BUFFER_SIZE);

		bufferLength = modFile->gcount();

		out.write(buffer, bufferLength);
	}

	delete modFile;

	delete[] buffer;

	return std::vector<std::string>();
}
