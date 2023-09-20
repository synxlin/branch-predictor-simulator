/*
 *	branch history table
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "bht.h"


void BHT_Initial(BHT *BranchHistoryTable, uint32_t index_width, uint32_t history_width)
{
	BranchHistoryTable->attributes.index_width = index_width;
	BranchHistoryTable->attributes.history_width = history_width;
	BranchHistoryTable->attributes.history_num = (uint32_t)pow_2(index_width);
	BranchHistoryTable->attributes.history_one = (uint64_t)pow_2(history_width - 1);
	BranchHistoryTable->history = (uint64_t *)malloc(sizeof(uint64_t) * BranchHistoryTable->attributes.history_num);
	if (BranchHistoryTable->history == NULL)
		_error_exit("malloc")
	memset(BranchHistoryTable->history, 0, sizeof(uint64_t) * BranchHistoryTable->attributes.history_num);
}

uint64_t BHT_Search(BHT *BranchHistoryTable, uint32_t addr, uint8_t two_byte_inst)
{
	uint32_t index = Get_Index(addr, BranchHistoryTable->attributes.index_width, two_byte_inst);
	return BranchHistoryTable->history[index];
}


void BHT_Update(BHT *BranchHistoryTable, uint32_t addr, uint8_t two_byte_inst, Result result)
{
	uint32_t index = Get_Index(addr, BranchHistoryTable->attributes.index_width, two_byte_inst);
	uint64_t old_history = BranchHistoryTable->history[index];
	old_history = old_history >> 1;
	if (result.actual_taken == TAKEN)
		old_history = old_history | BranchHistoryTable->attributes.history_one;
	BranchHistoryTable->history[index] = old_history;
}

void BHT_fprintf(BHT *BranchHistoryTable, FILE *fp)
{
	uint32_t i;
	for (i = 0; i < BranchHistoryTable->attributes.history_num; i++)
		fprintf(fp, "table[%u]: 0x %llx\n", i, BranchHistoryTable->history[i]);
}
