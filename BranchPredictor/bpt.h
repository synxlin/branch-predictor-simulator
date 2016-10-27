/*
 *	branch prediction table
 */
#pragma once

typedef enum Two_Bit_Counter
{
	strongly_not_taken = STRONGLY_NOT_TAKEN,
	weakly_not_taken = WEAKLY_NOT_TAKEN,
	weakly_taken = WEAKLY_TAKEN,
	strongly_taken = STRONGLY_TAKEN
} Two_Bit_Counter;

typedef struct BPT_Stat
{
	uint64_t num_access;
	uint64_t num_predict_taken;
	uint64_t num_mispredict_taken;
}BPT_Stat;

typedef struct BPT_Attributes
{
	uint32_t counter_num;
	uint32_t index_width;
}BPT_Attributes;

typedef struct BPT
{
	Two_Bit_Counter* counter;
	BPT_Attributes attributes;
	BPT_Stat stat;
}BPT;

void BPT_Initial(BPT* BranchPredictionTable, uint32_t index_width);

Taken_Result BPT_Predict(BPT* BranchPredictionTable, uint32_t addr);

void BPT_Update(BPT* BranchPredictionTable, uint32_t addr, Result result);