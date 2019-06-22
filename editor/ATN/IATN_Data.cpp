#include "IATN_Data.h"

bool ATN::operator<(const IATN_Data &a, const IATN_Data &b)
{
	return a.id() < b.id();
}
