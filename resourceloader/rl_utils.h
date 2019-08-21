#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include "RL_IResourceLoader.h"

namespace util
{
	std::int32_t hashFNV132(const std::string &str);

	// Like std::getline, but clears the carriage return character "\r"
	std::istream &getline(std::istream &stream, std::string &line);
}