/*
 *	globle branch history register
 */

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "ghr.h"


void GHR_Initial(GHR *GlobalBranchHistoryRegister, uint32_t history_width)
{
	GlobalBranchHistoryRegister->attributes.history_width = history_width;
	if (history_width == 0)
		GlobalBranchHistoryRegister->attributes.history_one = 0;
	else
		GlobalBranchHistoryRegister->attributes.history_one = (uint32_t)pow_2(history_width - 1);
	GlobalBranchHistoryRegister->history = 0;
}

void GHR_Update(GHR *GlobalBranchHistoryRegister, Result result)
{
	uint32_t old_history = GlobalBranchHistoryRegister->history;
	old_history = old_history >> 1;
	if (result.actual_taken == TAKEN)
		old_history = old_history | GlobalBranchHistoryRegister->attributes.history_one;
	GlobalBranchHistoryRegister->history = old_history;
}

void GHR_fprintf(GHR *GlobalBranchHistoryRegister, FILE *fp)
{
	fprintf(fp, "0x\t\t%x\n", GlobalBranchHistoryRegister->history);
}