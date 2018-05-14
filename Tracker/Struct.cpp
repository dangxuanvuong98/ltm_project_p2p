#include "stdafx.h"

bool operator <(NODE lhs, NODE rhs)
{
	return lhs.socket < rhs.socket;
}