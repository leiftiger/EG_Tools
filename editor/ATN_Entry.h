#pragma once

#include "IATN_Data.h"

#include <vector>

namespace ATN
{
	class Entry : public IATN_Data
	{
	private:
		std::uint32_t m_id;
		std::string m_name;

		std::string *m_file = nullptr;

	public:
		Entry();
		~Entry();

		virtual std::uint32_t id() const override;
		virtual const std::string &name() const override;

		virtual void setName(const std::string &name);
		virtual void setID(std::uint32_t id);

		// Whether or not this entry can be edited by the user (editing it may cause adverse game crashing effects!)
		virtual bool isEditable() const;

		// Converts this object into an ATN string entry (not including object header)
		virtual void serialize() const = 0;
	};
}