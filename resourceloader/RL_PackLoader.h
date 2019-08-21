#pragma once

#include "RL_FileLoader.h"

#include "ResourcePack.h"

namespace RL
{
	class PackLoader : public FileLoader
	{
	protected:
		const ResourcePack &m_resourcePack;

	public:
		PackLoader(const ResourcePack &erbFile);

		virtual std::istream *openFile(const std::string &filename) const override;
	};
}
