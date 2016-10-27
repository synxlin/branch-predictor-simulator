/*
 *	globle branch history register
 */

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "ghr.h"


 /*
  *	Inital the global branch history table
  */
void GHR_Initial(GHR *GlobalBranchHistoryRegister, uint32_t history_width)
{
	GlobalBranchHistoryRegister->attributes.history_width = history_width;
	GlobalBranchHistoryRegister->attributes.history_one = (uint32_t)pow_2(history_width - 1);
	GlobalBranchHistoryRegister->history = 0;
}

/*
 *	Update the GlobalBranchHistoryRegister
 *	input	:
 *		result	:	struct "Result", the prediction and actual result
 */
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
	fprintf("0x\t\t%x\n", GlobalBranchHistoryRegister->history);
}