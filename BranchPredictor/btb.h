/*
 *	branch target buffer
 */
#pragma once

typedef struct Block
{
	/* uint32_t* Target_Addr; */
	uint32_t tag;
	uint8_t valid_bit;
}Block;

typedef struct Set
{
	Block *block;
	uint64_t *rank;
}Set;

typedef struct BTB_Stat
{
	uint64_t num_predictions;
	uint64_t num_updates;
	uint64_t num_predict_branch;
	uint64_t num_mispredict_branch;
}BTB_Stat;

typedef struct BTB_Attributes
{
	uint32_t assoc;
	uint32_t set_num;
	uint32_t tag_width;
	uint32_t index_width;
}BTB_Attributes;

typedef struct BTB
{
	Set *set;
	BTB_Attributes attributes;
	BTB_Stat stat;
}BTB;

void BTB_Initial(BTB* BranchTargetBuffer, uint32_t assoc, uint32_t index_width);

void Interpret_Address(BTB* BranchTargetBuffer, uint32_t addr, uint32_t *tag, uint32_t *index);

uint32_t Rebuild_Address(BTB* BranchTargetBuffer, uint32_t tag, uint32_t index);

uint32_t BTB_Search(BTB* BranchTargetBuffer, uint64_t tag, uint32_t index);

void Rank_Maintain(BTB* BranchTargetBuffer, uint32_t index, uint32_t way_num, uint64_t rank_value);

uint32_t Rank_Top(BTB* BranchTargetBuffer, uint32_t index);

void BTB_Replacement(BTB* BranchTargetBuffer, uint32_t index, uint32_t way_num, uint32_t tag);

Branch_Result BTB_Predict(BTB* BranchTargetBuffer, uint32_t addr);

void BTB_Update(BTB* BranchTargetBuffer, uint32_t addr, Result result, uint64_t rank_value);