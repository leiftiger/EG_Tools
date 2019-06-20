#pragma once

#include <exception>
#include <string>

namespace ATN
{
	// Class for printing helpful information as to why the program erred
	class Exception : public std::exception
	{
	private:
		static const int ERR_SIZE = 64;

	public:

		Exception(const char *msg);

		Exception(const char *msg, const char *param1);
		Exception(const char *msg, const std::string &param1);
		Exception(const char *msg, int param1);
	};
}