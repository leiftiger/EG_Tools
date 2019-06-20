#pragma once

#include "ATN_Entry.h"

namespace ATN
{
	// "if" condition in the ATN, uneditable
	class Percept : protected Entry
	{
		// NOT EDITABLE FOR YOUR OWN SAFETY
		virtual bool isEditable() const override;
	};
}