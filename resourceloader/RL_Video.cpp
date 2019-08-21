#include "RL_Video.h"

namespace RL
{
	const char * const VideoLoader::extension() const
	{
		return ".md";
	}
	std::vector<BaseResource*> VideoLoader::load(const std::string &filename, const FileLoader &loader)
	{
		std::vector<BaseResource*> res;

		std::string strVideo = IResourceLoader::pathToFileName(filename) + ".md";

		res.push_back(new BaseResource("Video ID", strVideo, util::hashFNV132(strVideo)));

		return res;
	}
}