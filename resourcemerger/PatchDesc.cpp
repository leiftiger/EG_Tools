#include "PatchDesc.h"

PatchDesc::PatchDesc(const std::string &outFilename, const std::string &modFilename) : IResourcePatch({ outFilename }, { modFilename })
{

}

std::vector<std::string> PatchDesc::apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const
{
	// TODO: Perform delta patching
	return std::vector<std::string>();
}

const char * const PatcherDesc::extension() const
{
	return ".desc";
}

std::vector<IResourcePatch*> PatcherDesc::createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files)
{
	std::vector<IResourcePatch*> patches;

	for (const std::string &modFile : files)
	{
		std::string filename = modFile.substr(modFile.find_last_of('/') + 1);

		int descID = std::stoi(filename.substr(0, 5));

		std::string descName = filename.substr(5);

		try
		{
			std::istream *fs = merger.resourcePacks().openFile(filename);

			patches.push_back(new PatchDesc(filename, modFile));

			// TODO: Remove when delta patching is functional
			merger.lockFile(filename);

			delete fs;
		}
		catch (std::exception e)
		{
			// New addition, we give it a unique desc ID to avoid mod conflicts
			descID = mod.translateDescID(descID, merger);

			char strDesc[6];

			sprintf_s(strDesc, "%05d", descID);

			patches.push_back(new PatchDesc(strDesc + descName, modFile));
		}
	}

	return patches;
}
