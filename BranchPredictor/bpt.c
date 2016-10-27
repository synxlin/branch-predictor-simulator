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
	uint64_t i;
	for (i = 0; i < BranchPredictionTable->attributes.counter_num; i++)
		BranchPredictionTable->counter[i] = weakly_taken;
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
	BranchPredictionTable->stat.num_preditions++;
	switch (BranchPredictionTable->counter[index])
	{
	case strongly_taken:
	case weakly_taken:
		BranchPredictionTable->stat.num_predict_taken++;
		return taken;
	default:
		return not_taken;
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
	if (result.actual_taken != result.predict_taken[result.predict_predictor])
		BranchPredictionTable->stat.num_mispredict_taken++;

	if (result.actual_taken == taken)
	{
		switch (BranchPredictionTable->counter[index])
		{
		case strongly_not_taken:
		case weakly_not_taken:
		case weakly_taken:
			BranchPredictionTable->stat.num_updates++;
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
			BranchPredictionTable->stat.num_updates++;
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
		fprintf(fp, "table[%lu]: %u\n", i, BranchPredictionTable->counter[i]);
}