#include "RP_utils.h"

namespace util
{
	static const std::uint32_t prime = 16777619;

	// Fowler-Noll-Vo 1 32 bit hash function
	std::int32_t hashFNV132(const std::string &str)
	{
		// Hash is always of the lowercased string
		std::string lCase = str;

		std::transform(lCase.begin(), lCase.end(), lCase.begin(), ::tolower);

		std::uint8_t *buff = (std::uint8_t*)lCase.c_str();
		size_t buffLength = lCase.length();

		// We hash using unsigned, to avoid any issues with the multiplication
		// then we just return the signed value that is seemingly used in the ATNs
		std::uint32_t hash = 2166136261;

		for (size_t i = 0; i < buffLength; ++i)
		{
			hash *= prime;
			hash ^= std::uint32_t(*buff);

			buff++;
		}

		return hash;
	}

	std::istream &getline(std::istream &stream, std::string &line)
	{
		std::getline(stream, line);

		if (line.length() > 0 && line[line.length() - 1] == '\r')
			line = line.substr(0, line.length() - 1);

		return stream;
	}
}