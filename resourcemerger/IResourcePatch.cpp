#include "IResourcePatch.h"

IResourcePatch::IResourcePatch(const std::initializer_list<std::string> &filenames, const std::initializer_list<std::string> &modFilenames)
{
	for (const std::string &filename : filenames)
	{
		m_filenames.push_back(filename);
	}

	for (const std::string &filename : modFilenames)
	{
		m_modFiles.push_back(filename);
	}
}

const std::vector<std::string> &IResourcePatch::filenames() const
{
	return m_filenames;
}

std::vector<std::string> IResourcePatch::apply(const ModPack &mod, std::vector<std::ostream*> &outStreams) const
{
	std::vector<std::string> strOutputs;

	for (int i = 0; i < m_filenames.size(); i++)
	{
		const std::string &modFilename = m_modFiles[i];

		std::ostream *out = outStreams[i];

		std::istream *modFile = mod.openFile(modFilename);

		if (modFile->fail())
		{
			strOutputs.push_back("Couldn't open \"" + modFilename + "\" for reading (mod)");
			continue;
		}

		char *buffer = new char[COPY_BUFFER_SIZE];

		std::streamsize bufferLength = 0;

		while (!modFile->eof())
		{
			int len = sizeof(buffer) / sizeof(char);

			modFile->read(buffer, COPY_BUFFER_SIZE);

			bufferLength = modFile->gcount();

			out->write(buffer, bufferLength);
		}

		strOutputs.push_back("Copied " + m_filenames[i]);

		delete modFile;

		delete[] buffer;

	}

	return strOutputs;
}
