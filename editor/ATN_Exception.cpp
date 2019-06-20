#include "ATN_Exception.h"

namespace ATN
{
	Exception::Exception(const char *msg)
	{
		std::exception::exception(msg);
	}

	Exception::Exception(const char *msg, const char *param1)
	{
		char err[ERR_SIZE];

		sprintf_s(err, msg, param1);

		std::exception::exception(err);
	}

	Exception::Exception(const char * msg, const std::string &param1)
	{
		char err[ERR_SIZE];

		sprintf_s(err, msg, param1.c_str());

		std::exception::exception(err);
	}

	Exception::Exception(const char *msg, int param1)
	{
		char err[ERR_SIZE];

		sprintf_s(err, msg, param1);

		std::exception::exception(err);
	}
}