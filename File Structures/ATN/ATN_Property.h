#pragma once

#include "IATN_Data.h"

namespace ATN
{
	class Property : public IATN_Data
	{
	private:
		std::string m_name;
		std::int32_t m_hash;
	public:
		Property(std::string name, std::int32_t hash);

		virtual std::int32_t id() const override;
		virtual const std::string &name() const override;
	};
}