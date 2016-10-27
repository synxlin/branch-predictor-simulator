/*
 *	branch prediction table
 */
#pragma once

#ifndef BPT_H_
#define BPT_H_

#define STRONGLY_TAKEN 3
#define WEAKLY_TAKEN 2
#define WEAKLY_NOT_TAKEN 1
#define STRONGLY_NOT_TAKEN 0

typedef enum Two_Bit_Counter
{
	strongly_not_taken = STRONGLY_NOT_TAKEN,
	weakly_not_taken = WEAKLY_NOT_TAKEN,
	weakly_taken = WEAKLY_TAKEN,
	strongly_taken = STRONGLY_TAKEN
} Two_Bit_Counter;

typedef struct BPT_Attributes
{
	uint64_t counter_num;
	uint32_t index_width;
}BPT_Attributes;

typedef struct BPT
{
	Two_Bit_Counter* counter;
	BPT_Attributes attributes;
}BPT;

/*
 *	Initial the branch prediction table
 *	input	:
 *		index_width	:	the width of index
 *						bimodal:	i_B		gshare:	i_G
 *						hybrid:		(bimodal) i_B	(gshare) i_G
 *						yehpatt:	p
 */
void BPT_Initial(BPT* BranchPredictionTable, uint32_t index_width);

/*
 *	Search the BranchPredictionTable for entry of "index" and make prediction
 *	input	:
 *		index	:	index of counter
 *	return	:
 *		the prediction on whether branch is taken -- taken or not_taken
 */
Taken_Result BPT_Predict(BPT* BranchPredictionTable, uint64_t index);

/*
 *	Update the BranchPredictionTable
 *	input	:
 *		index	:	index of counter
 *		result	:	struct "Result", the prediction and actual result
 */
void BPT_Update(BPT* BranchPredictionTable, uint64_t index, Result result);

/*
 * Print the content of BranchPredictionTable to file *fp
 */
void BPT_fprintf(BPT* BranchPredictionTable, FILE *fp);

#endif