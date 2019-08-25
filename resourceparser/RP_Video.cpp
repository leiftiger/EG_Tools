#include "RP_Video.h"

namespace RL
{
	const char * const VideoParser::extension() const
	{
		return ".md";
	}
	std::vector<BaseResource*> VideoParser::parse(const std::string &filename, const FileLoader &loader)
	{
		std::vector<BaseResource*> res;

		std::string strVideo = IResourceParser::pathToFileName(filename) + ".md";

		res.push_back(new BaseResource("Video ID", strVideo, util::hashFNV132(strVideo)));

		return res;
	}
}