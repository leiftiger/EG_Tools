#include "ATN_Exception.h"

namespace ATN
{
	Exception::Exception(const char *msg)
	{
		m_exceptionMessage = msg;

		if (util::DEBUG_LINE != -1)
			m_exceptionMessage += "\n(line " + std::to_string(util::DEBUG_LINE) + ")";
	}

	Exception::Exception(const char *msg, const char *param1)
	{
		char err[ERR_SIZE];

		sprintf_s(err, msg, param1);

		m_exceptionMessage = err;

		if (util::DEBUG_LINE != -1)
			m_exceptionMessage += "\n(line " + std::to_string(util::DEBUG_LINE) + ")";
	}

	Exception::Exception(const char * msg, const std::string &param1)
	{
		char err[ERR_SIZE];

		sprintf_s(err, msg, param1.c_str());

		m_exceptionMessage = err;

		if (util::DEBUG_LINE != -1)
			m_exceptionMessage += "\n(line " + std::to_string(util::DEBUG_LINE) + ")";
	}

	Exception::Exception(const char *msg, int param1)
	{
		char err[ERR_SIZE];

		sprintf_s(err, msg, param1);

		m_exceptionMessage = err;

		if (util::DEBUG_LINE != -1)
			m_exceptionMessage += "\n(line " + std::to_string(util::DEBUG_LINE) + ")";
	}

	Exception::Exception(const char *msg, int param1, const char *param2)
	{
		char err[ERR_SIZE];

		sprintf_s(err, msg, param1, param2);

		m_exceptionMessage = err;

		if (util::DEBUG_LINE != -1)
			m_exceptionMessage += "\n(line " + std::to_string(util::DEBUG_LINE) + ")";
	}

	Exception::Exception(const char * msg, int param1, const std::string &param2)
	{
		char err[ERR_SIZE];

		sprintf_s(err, msg, param1, param2.c_str());

		m_exceptionMessage = err;

		if (util::DEBUG_LINE != -1)
			m_exceptionMessage += "\n(line " + std::to_string(util::DEBUG_LINE) + ")";
	}

	Exception::Exception(const char *msg, const char *param1, const char *param2)
	{
		char err[ERR_SIZE];

		sprintf_s(err, msg, param1, param2);

		m_exceptionMessage = err;

		if (util::DEBUG_LINE != -1)
			m_exceptionMessage += "\n(line " + std::to_string(util::DEBUG_LINE) + ")";
	}

	Exception::Exception(const char *msg, const std::string &param1, const char *param2)
	{
		char err[ERR_SIZE];

		sprintf_s(err, msg, param1.c_str(), param2);

		m_exceptionMessage = err;

		if (util::DEBUG_LINE != -1)
			m_exceptionMessage += "\n(line " + std::to_string(util::DEBUG_LINE) + ")";
	}

	Exception::Exception(const char *msg, const char *param1, const std::string &param2)
	{
		char err[ERR_SIZE];

		sprintf_s(err, msg, param1, param2.c_str());

		m_exceptionMessage = err;

		if (util::DEBUG_LINE != -1)
			m_exceptionMessage += "\n(line " + std::to_string(util::DEBUG_LINE) + ")";
	}

	Exception::Exception(const char *msg, const std::string &param1, const std::string &param2)
	{
		char err[ERR_SIZE];

		sprintf_s(err, msg, param1.c_str(), param2.c_str());

		m_exceptionMessage = err;

		if (util::DEBUG_LINE != -1)
			m_exceptionMessage += "\n(line " + std::to_string(util::DEBUG_LINE) + ")";
	}

	const char * Exception::what() const noexcept
	{
		return m_exceptionMessage.c_str();
	}
}