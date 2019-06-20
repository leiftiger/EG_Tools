#pragma once

#include "ATN_Entry.h"

namespace ATN
{
	// An action to the game engine, uneditable
	class Effect : protected Entry
	{
		// NOT EDITABLE FOR YOUR OWN SAFETY
		virtual bool isEditable() const override;
	};
}