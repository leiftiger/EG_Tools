#pragma once

#include <exception>
#include <string>

#include "utils.h"

namespace ATN
{
	// Class for printing helpful information as to why the program erred
	class Exception : public std::exception
	{
	private:
		// Some lines can get very long...
		static const int ERR_SIZE = 1024;

		std::string m_exceptionMessage;

	public:

		Exception(const char *msg);

		Exception(const char *msg, const char *param1);
		Exception(const char *msg, const std::string &param1);
		Exception(const char *msg, int param1);

		Exception(const char *msg, int param1, const char *param2);
		Exception(const char *msg, int param1, const std::string &param2);

		Exception(const char *msg, const char *param1, const char *param2);
		Exception(const char *msg, const std::string &param1, const char *param2);
		Exception(const char *msg, const char *param1, const std::string &param2);
		Exception(const char *msg, const std::string &param1, const std::string &param2);

		virtual const char* what() const noexcept override;
	};
}