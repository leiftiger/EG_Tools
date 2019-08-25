#pragma once

#include "ATN/ATN_Manager.h"

#include "utils.h"

namespace util
{
	// Loads all static definitions from program files
	void loadStaticDefinitions();

	// Loads all dynamic definition from Evil Genius files
	void loadDynamicDefinitions();
}