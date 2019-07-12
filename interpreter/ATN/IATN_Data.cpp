#include "IATN_Data.h"

namespace ATN
{
	bool operator<(const IATN_Data &a, const IATN_Data &b)
	{
		return a.id() < b.id();
	}

	bool compareLessThanPointersIATN(const IATN_Data *a, const IATN_Data *b)
	{
		return a->id() < b->id();
	}

	bool compareEqualPointersIATN(const IATN_Data * a, const IATN_Data * b)
	{
		return a->id() == b->id();
	}
}