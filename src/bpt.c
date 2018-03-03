/*
 *	branch prediction table
 */


#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "bpt.h"


void BPT_Initial(BPT* BranchPredictionTable, uint32_t index_width)
{
	BranchPredictionTable->attributes.index_width = index_width;
	BranchPredictionTable->attributes.counter_num = (uint64_t)pow_2(index_width);

	BranchPredictionTable->counter = (Two_Bit_Counter *)malloc(sizeof(Two_Bit_Counter) * BranchPredictionTable->attributes.counter_num);
	if (BranchPredictionTable->counter == NULL)
		_error_exit("malloc")
	uint64_t i;
	for (i = 0; i < BranchPredictionTable->attributes.counter_num; i++)
		BranchPredictionTable->counter[i] = weakly_taken;
}

Taken_Result BPT_Predict(BPT* BranchPredictionTable, uint64_t index)
{
	switch (BranchPredictionTable->counter[index])
	{
	case strongly_taken:
	case weakly_taken:
		return taken;
	default:
		return not_taken;
	}
}

void BPT_Update(BPT* BranchPredictionTable, uint64_t index, Result result)
{
	if (result.actual_taken == taken)
	{
		switch (BranchPredictionTable->counter[index])
		{
		case strongly_not_taken:
		case weakly_not_taken:
		case weakly_taken:
			BranchPredictionTable->counter[index]++;
		default:
			return;
		}
	}
	else
	{
		switch (BranchPredictionTable->counter[index])
		{
		case strongly_taken:
		case weakly_taken:
		case weakly_not_taken:
			BranchPredictionTable->counter[index]--;
		default:
			return;
		}
	}
}

void BPT_fprintf(BPT* BranchPredictionTable, FILE *fp)
{
	uint64_t i;
	for (i = 0; i < BranchPredictionTable->attributes.counter_num; i++)
		fprintf(fp, "table[%llu]: %u\n", i, BranchPredictionTable->counter[i]);
}