/*
 *	branch prediction table
 */


#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "bpt.h"

 /*
  *	Inital the branch prediction table
  */
void BPT_Initial(BPT* BranchPredictionTable, uint32_t index_width)
{
	BranchPredictionTable->attributes.index_width = index_width;
	BranchPredictionTable->attributes.counter_num = (uint64_t)pow_2(index_width);

	memset(&(BranchPredictionTable->stat), 0, sizeof(BPT_Stat));
	BranchPredictionTable->counter = (Two_Bit_Counter *)malloc(sizeof(Two_Bit_Counter) * BranchPredictionTable->attributes.counter_num);
	if (BranchPredictionTable->counter == NULL)
		_error_exit("malloc")
	uint32_t i;
	for (i = 0; i < BranchPredictionTable->attributes.counter_num; i++)
		BranchPredictionTable->counter[i] = WEAKLY_TAKEN;
}

/*
 *	Search the BranchPredictionTable for entry of "index" and make prediction
 *	input	:
 *		index	:	index of counter
 *	return	:
 *		the prediction on whether branch is taken -- TAKEN or NOT_TAKEN
 */
Taken_Result BPT_Predict(BPT* BranchPredictionTable, uint64_t index)
{
	switch (BranchPredictionTable->counter[index])
	{
	case STRONGLY_TAKEN:
	case WEAKLY_TAKEN:
		return TAKEN;
	default:
		return NOT_TAKEN;
	}
}

/*
 *	Update the BranchPredictionTable
 *	input	:
 *		index	:	index of counter
 *		result	:	struct "Result", the prediction and actual result
 */
void BPT_Update(BPT* BranchPredictionTable, uint64_t index, Result result)
{
	if (result.actual_taken == TAKEN)
	{
		switch (BranchPredictionTable->counter[index])
		{
		case STRONGLY_NOT_TAKEN:
		case WEAKLY_NOT_TAKEN:
		case WEAKLY_TAKEN:
			BranchPredictionTable->counter[index]++;
		default:
			return;
		}
	}
	else
	{
		switch (BranchPredictionTable->counter[index])
		{
		case STRONGLY_TAKEN:
		case WEAKLY_TAKEN:
		case WEAKLY_NOT_TAKEN:
			BranchPredictionTable->counter[index]--;
		default:
			return;
		}
	}
}