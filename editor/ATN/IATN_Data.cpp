#include "IATN_Data.h"

bool ATN::operator<(const IATN_Data &a, const IATN_Data &b)
{
	return a.id() < b.id();
}

bool ATN::compareLessThanPointersIATN(const IATN_Data *a, const IATN_Data *b)
{
	return a->id() < b->id();
}

bool ATN::compareEqualPointersIATN(const IATN_Data * a, const IATN_Data * b)
{
	return a->id() == b->id();
}
