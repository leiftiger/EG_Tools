#pragma once

#include <string>
#include <cstdint>

namespace RL
{
	class BaseResource
	{
	private:

		std::string m_type;
		std::string m_key;
		std::int64_t m_value;

	public:

		BaseResource(const std::string &type, const std::string &key, std::int64_t value);
		virtual ~BaseResource() = default;

		// Type of resource
		virtual const std::string &type() const;
		// User-readable definition pathToFileName
		virtual const std::string &key() const;
		// Value used in ATN files
		virtual std::int64_t value() const;
	};
}