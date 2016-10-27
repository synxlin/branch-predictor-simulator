#include "utils.h"

uint32_t Get_Index(uint32_t addr, uint32_t index_width)
{
	return (addr << (30 - index_width)) >> (32 - index_width);
}