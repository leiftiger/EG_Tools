#pragma once

#include "ATN/ATN_Manager.h"

#include "RP_PackLoader.h"

#include "utils.h"

namespace util
{
	// Loads all static definitions from program files
	void loadStaticDefinitions();

	// Load network resource relationships from the given resource pack (expects ATNResources.ros)
	void loadResourceRelationships(const ResourcePack &pack);

	// Loads all dynamic definition from Evil Genius files
	void loadDynamicDefinitions();
}